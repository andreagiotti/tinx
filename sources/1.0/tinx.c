/*
  TINX - Temporal Inference Network eXecutor
  Design & coding by Andrea Giotti, 1998-1999
*/

#include "tinx.h"

#define VER "1.00"

const event null_event = {{NULL, no_link}, -1};

const char class_symbol[NODE_CLASSES_NUMBER] = CLASS_SYMBOLS;

volatile sig_atomic_t irq = FALSE;

/******** Tools ********/

event ev_neg(event s)
{
  event r;

  assert(valid(s));

  r.e = arc_neg(s.e);
  r.t = s.t;

  return r;
}

arc arc_between(node *vp, node *wp)
{
  arc e;
  link_code lc;

  assert(vp && wp);

  e.vp = wp;
  e.lc = no_link;

  for(lc = parent; lc < LINK_CODES_NUMBER; lc++)
    if(vp == wp->pin[lc].e.vp)
      {
        e.lc = lc;
        break;
      }

  return e;
}

/******** Inference engine ********/

void state(k_base *kb, event s)
{
  event q, r;

  assert(kb);
  assert(valid(s));
  assert(!stated(s));

  if(kb->soundness_check && stated(ev_neg(s)))
    {
      fprintf(stderr, "(%s, %s)@"TIME_FMT": Soundness violation\n",
              arc_neg(s.e).vp->name, s.e.vp->name, s.t);
      close_base(kb);
      exit(EXIT_FAILURE);
    }

  stated(s) = TRUE;

  q = null_event;
  r = kb->focus;
  while(valid(r) && r.t < s.t)
    {
      q = r;
      r = next(r);
    }

  if(valid(q))
    next(q) = s;
  else
    kb->focus = s;

  if(r.t == s.t)
    {
      assert(valid(r));

      other(s) = r;
      next(s) = next(r);
    }
  else
    {
      other(s) = null_event;
      next(s) = r;
    }

#if !defined NDEBUG
  printf("\t(%s, %s)@"TIME_FMT"\n", arc_neg(s.e).vp->name, s.e.vp->name, s.t);
#endif

  if(link_of(s.e).io_stream[output_stream])
    output(s);
}

event choose(k_base *kb)
{
  event r, s;

  assert(kb);

  s = kb->focus;

  if(valid(s))
    {
      assert(!chosen(s));

      chosen(s) = TRUE;
      r = other(s);

      if(valid(r))
        {
          next(r) = next(s);
          kb->focus = r;
        }
      else
        kb->focus = next(s);

#if !defined NDEBUG
      printf("(%s, %s)@"TIME_FMT"\n", arc_neg(s.e).vp->name, s.e.vp->name, s.t);
#endif
    }

  return s;
}

void process(k_base *kb, event s)
{
  event r;
  link_code lc1, lc2;

  assert(kb);
  assert(valid(s));

  switch(s.e.vp->class)
    {
    case gate:

      lc1 = (s.e.lc + 1) % LINK_CODES_NUMBER;
      lc2 = (s.e.lc + 2) % LINK_CODES_NUMBER;

      if(s.e.vp->pin[lc1].history[s.t].chosen)
        {
          r.e = s.e.vp->pin[lc2].e;
          r.t = s.t;

          if(!stated(r))
            state(kb, r);
        }
      else
        if(s.e.vp->pin[lc2].history[s.t].chosen)
          {
            r.e = s.e.vp->pin[lc1].e;
            r.t = s.t;

            if(!stated(r))
              state(kb, r);
          }

      break;

    case joint:

      r.t = s.t;

      if(s.e.lc == parent)
        {
          r.e = s.e.vp->pin[left_son].e;

          if(!stated(r))
            state(kb, r);

          r.e = s.e.vp->pin[right_son].e;

          if(!stated(r))
            state(kb, r);
        }
      else
        {
          r.e = s.e.vp->pin[parent].e;

          if(!stated(r))
            state(kb, r);
        }

      break;

    case delay:

      if(s.e.lc == parent)
        {
          r.e = s.e.vp->pin[left_son].e;
          r.t = s.t - s.e.vp->k;
        }
      else
        {
          r.e = s.e.vp->pin[parent].e;
          r.t = s.t + s.e.vp->k;
        }

      if(kb->strictly_causal)
        {
          if(r.t >= s.t && r.t < TIME_LIMIT && !stated(r))
            state(kb, r);
        }
      else
        if(r.t >= 0 && r.t < TIME_LIMIT && !stated(r))
          state(kb, r);

      break;

    default:
      assert(FALSE);
    }
}

bool loop(k_base *kb)
{
  event s;

  if(kb->trace_focus)
    trace(kb);

  s = choose(kb);

  if(valid(s))
    {
      process(kb, s);
      scan_inputs(kb);
      return TRUE;
    }
  else
    return scan_inputs(kb);
}

/******** Input/Output ********/

event input(stream *ios)
{
  event s;
  char c;

  s.t = ftell(ios->fp);
  if(s.t == -1)
    {
      perror(ios->file_name);
      exit(EXIT_FAILURE);
    }
  else
    s.t += ios->offset;

  c = fgetc(ios->fp);

  switch(c)
    {
    case EOF:
      if(ferror(ios->fp))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }
      else
        {
          clearerr(ios->fp);
          s = null_event;
        }
      break;

    case '\0':
      s = null_event;
      break;

    case LO_CHAR:
      if(s.t < 0 || s.t >= TIME_LIMIT)
        s = null_event;
      else
        s.e = ios->pin->e;
      break;

    case HI_CHAR:
      if(s.t < 0 || s.t >= TIME_LIMIT)
        s = null_event;
      else
        s.e = arc_neg(ios->pin->e);
      break;

    case END_CHAR:
      ios->open = FALSE;
      s = null_event;
      break;

    case TERM_CHAR:
      irq = TRUE;
      s = null_event;
      break;

    default:
      fprintf(stderr, "%s, %c (dec %d): Invalid character in stream\n",
              ios->file_name, c, c);
      exit(EXIT_FAILURE);
    }

  return s;
}

void output(event s)
{
  stream *ios;

  ios = link_of(s.e).io_stream[output_stream];

  if(s.t - ios->offset >= 0)
    {
      if(fseek(ios->fp, s.t - ios->offset, SEEK_SET))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }

      if(fputc((ios->pin->e.vp == s.e.vp)? LO_CHAR : HI_CHAR, ios->fp) == EOF)
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }

      if(fflush(ios->fp))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }
    }
}

bool scan_inputs(k_base *kb)
{
  stream *ios, *next_ios;
  event s;

  ios = kb->io_stream[input_stream];
  while(ios)
    {
      if(ios->open)
        {
          s = input(ios);

          if(valid(s) && !stated(s))
            state(kb, s);

          ios = ios->next_ios;
        }
      else
        {
          next_ios = ios->next_ios;

          if(next_ios)
            next_ios->prev_ios = ios->prev_ios;

          if(ios->prev_ios)
            ios->prev_ios->next_ios = next_ios;
          else
            kb->io_stream[input_stream] = next_ios;

          close_stream(ios);
          ios = next_ios;
        }
    }

  return (kb->io_stream[input_stream] != NULL);
}

void trace(k_base *kb)
{
  event s;

  if(valid(kb->focus))
    {
      s = kb->focus;
      kb->idle = FALSE;

      if(kb->echo_stdout)
        {
          printf(" > (%s, %s)@"TIME_FMT"        \r",
                 arc_neg(s.e).vp->name, s.e.vp->name, s.t);
          fflush(stdout);
        }

      if(kb->logfp)
        {
          if(fprintf(kb->logfp, "(%s, %s)@"TIME_FMT"\n",
                     arc_neg(s.e).vp->name, s.e.vp->name, s.t) < 0)
            {
              perror(NULL);
              exit(EXIT_FAILURE);
            }

          if(fflush(kb->logfp))
            {
              perror(NULL);
              exit(EXIT_FAILURE);
            }
        }
    }
  else
    if(!kb->idle)
      {
        kb->idle = TRUE;

        if(kb->echo_stdout)
          { 
            printf(" *\r");
            fflush(stdout);
          }
      }
}

stream *open_stream(char *name, stream_class class, arc e, d_time offset)
{
  stream *ios;

  ios = malloc(sizeof(stream));
  if(!ios)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  strcpy(ios->name, name);
  ios->class = class;

  ios->pin = &link_of(e);
  ios->pin->io_stream[class] = ios;
  link_of(ios->pin->e).io_stream[class] = ios;

  ios->offset = offset;

  strcpy(ios->file_name, name);
  strcat(ios->file_name, STREAM_EXT);

  ios->fp = fopen(ios->file_name, (class == input_stream)? "r" : "w");

  if(!ios->fp)
    {
      perror(ios->file_name);
      exit(EXIT_FAILURE);
    }

  ios->open = TRUE;

  return ios;
}

void close_stream(stream *ios)
{
  ios->pin->io_stream[ios->class] = NULL;
  link_of(ios->pin->e).io_stream[ios->class] = NULL;

  if(ios->class == output_stream &&
     (fseek(ios->fp, 0, SEEK_END) || fputc(END_CHAR, ios->fp) == EOF))
    {
      perror(ios->file_name);
      exit(EXIT_FAILURE);
    }

  if(fclose(ios->fp))
    {
      perror(ios->file_name);
      exit(EXIT_FAILURE);
    }

  free(ios);
}

/******** Network setup ********/

node *alloc_node(char *name)
{
  node *vp;
  link_code lc;
  d_time t;

  vp = malloc(sizeof(node));
  if(!vp)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  strcpy(vp->name, name);
  vp->class = null_op;

  for(lc = 0; lc < LINK_CODES_NUMBER; lc++)
    {
      vp->pin[lc].e.vp = NULL;
      vp->pin[lc].e.lc = no_link;

      for(t = 0; t < TIME_LIMIT; t++)
        {
          vp->pin[lc].history[t].stated = FALSE;
          vp->pin[lc].history[t].chosen = FALSE;
          vp->pin[lc].history[t].other = null_event;
          vp->pin[lc].history[t].next = null_event;
        }

      vp->pin[lc].io_stream[input_stream] = NULL;
      vp->pin[lc].io_stream[output_stream] = NULL;
    }

  return vp;
}

node *name2node(k_base *kb, char *name)
{
  node *vp;
  int h;

  if(*name)
    {
      h = hash(name);

      if(kb->table[h])
        {
          vp = kb->table[h];
          if(strcmp(vp->name, name))
            {
              fprintf(stderr, "%s, %s: Node names generate duplicate hashes\n",
                      vp->name, name);
              exit(EXIT_FAILURE);
            }
        }
      else
        {
          vp = alloc_node(name);
          assert(vp);

          vp->vp = kb->network;
          kb->network = vp;

          kb->table[h] = vp;

#if !defined NDEBUG
          printf("%s -> "HASH_FMT"\n", name, h);
#endif
        }
      return vp;
    }
  else
    return NULL;
}

void thread_network(node *network)
{
  node *vp;
  link_code lc, lc1;

  for(vp = network; vp; vp = vp->vp)
    for(lc = 0; lc < LINK_CODES_NUMBER && vp->pin[lc].e.vp; lc++)
      if(vp->pin[lc].e.lc < 0)
        {
          for(lc1 = 0; lc1 < LINK_CODES_NUMBER; lc1++)
            {
              if(!vp->pin[lc].e.vp->pin[lc1].e.vp)
                vp->pin[lc].e.vp->pin[lc1].e.vp = vp;

              if(vp->pin[lc].e.vp->pin[lc1].e.vp == vp &&
                 vp->pin[lc].e.vp->pin[lc1].e.lc < 0)
                {
                  vp->pin[lc].e.vp->pin[lc1].e.lc = lc;
                  vp->pin[lc].e.lc = lc1;

                  break;
                }
            }
          if(lc1 == LINK_CODES_NUMBER)
            {
              fprintf(stderr, "%s: Arc mismatch in node declarations\n",
                      vp->name);
              exit(EXIT_FAILURE);
            }
        }

  for(vp = network; vp; vp = vp->vp)
    {
      for(lc = 0; lc < LINK_CODES_NUMBER && vp->pin[lc].e.lc >= 0; lc++);

      switch(vp->class)
        {
        case gate:
        case joint:
          assert(lc <= 3);
          if(lc < 3)
            {
              fprintf(stderr, "%s: Undefined arc in node declarations\n",
                      vp->name);
              exit(EXIT_FAILURE);
            }
          break;

        case delay:
          if(lc < 2)
            {
              fprintf(stderr, "%s: Undefined arc in node declarations\n",
                      vp->name);
              exit(EXIT_FAILURE);
            }
          else
            if(lc > 2)
              {
                fprintf(stderr, "%s: Arc mismatch in node declarations\n",
                        vp->name);
                exit(EXIT_FAILURE);
              }
          break;

        default:
          fprintf(stderr, "%s: Reference to undefined node\n", vp->name);
          exit(EXIT_FAILURE);
        }

#if !defined NDEBUG
      printf("%s: "OP_FMT""TIME_FMT" ; %s", vp->name,
             class_symbol[vp->class], vp->k, vp->pin[parent].e.vp->name);

      if(vp->pin[left_son].e.vp)
        printf(", %s", vp->pin[left_son].e.vp->name);

      if(vp->pin[right_son].e.vp)
        printf(", %s", vp->pin[right_son].e.vp->name);

      printf("\n");
#endif
    }
}

k_base *open_base(char *base_name, char *logfile_name,
                  bool strictly_causal, bool soundness_check, bool echo_stdout)
{
  k_base *kb;
  FILE *fp;
  char file_name[MAX_STRLEN], name[MAX_NAMELEN], type[MAX_NAMELEN],
       up[MAX_NAMELEN], left[MAX_NAMELEN], right[MAX_NAMELEN],
       name_v[MAX_NAMELEN], name_w[MAX_NAMELEN];
  char c;
  d_time k;
  node *vp, *wp;
  node_class nclass;
  stream *ios;
  stream_class sclass;
  arc e;

  kb = malloc(sizeof(k_base));
  if(!kb)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  kb->focus = null_event;
  kb->network = NULL;

  memset(kb->table, 0, sizeof(node *) * HASH_SIZE);
  memset(kb->io_stream, 0, sizeof(stream *) * STREAM_CLASSES_NUMBER);

  strcpy(file_name, base_name);
  strcat(file_name, NETWORK_EXT);
  fp = fopen(file_name, "r");
  if(!fp)
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  *left = *right = '\0';
  while(fscanf(fp, " "NAME_FMT" : "NAME_FMT" ; "NAME_FMT" , "
                   NAME_FMT" , "NAME_FMT" ",
               name, type, up, left, right) >= 3)
    {
      k = 1;
      sscanf(type, OP_FMT""ARG_FMT, &c, &k);

      nclass = strchr(class_symbol, toupper(c)) - class_symbol;
      if(nclass < 0)
        {
          fprintf(stderr, "%s, %s, "OP_FMT": Invalid node class\n",
                  file_name, name, c);
          exit(EXIT_FAILURE);
        }

      if(k <= -TIME_LIMIT || k >= TIME_LIMIT)
        {
          fprintf(stderr, "%s, %s, "TIME_FMT": Delay out of range\n",
                  file_name, name, k);
          exit(EXIT_FAILURE);
        }

      vp = name2node(kb, name);
      if(vp->class >= 0)
        {
          fprintf(stderr, "%s, %s: Duplicate node\n", file_name, name);
          exit(EXIT_FAILURE);
        }

      vp->class = nclass;
      vp->k = k;

      vp->pin[parent].e.vp = name2node(kb, up);
      vp->pin[left_son].e.vp = name2node(kb, left);
      vp->pin[right_son].e.vp = name2node(kb, right);

      *left = *right = '\0';
    }

  if(ferror(fp))
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  thread_network(kb->network);

  k = 0;
  while(fscanf(fp, " "OP_FMT" "NAME_FMT" ( "NAME_FMT" , "NAME_FMT" ) @ "
                   TIME_FMT" ",
               &c, name, name_v, name_w, &k) >= 4)
    {
      vp = kb->table[hash(name_v)];
      wp = kb->table[hash(name_w)];

      if(!vp || !wp)
        {
          fprintf(stderr, "%s, %s: Undefined node\n",
                  file_name, !vp? name_v : name_w);
          exit(EXIT_FAILURE);
        }

      e = arc_between(vp, wp);
      if(e.lc < 0)
        {
          fprintf(stderr, "%s, (%s, %s): Undefined arc\n",
                  file_name, name_v, name_w);
          exit(EXIT_FAILURE);
        }

      if(k <= -TIME_LIMIT || k >= TIME_LIMIT)
        {
          fprintf(stderr, "%s, "TIME_FMT": Offset out of range\n",
                  file_name, k);
          exit(EXIT_FAILURE);
        }

      switch(c)
        {
        case '!':
          sclass = input_stream;
          break;

        case '?':
          sclass = output_stream;
          break;

        default:
          fprintf(stderr, "%s, "OP_FMT": Invalid stream class\n", file_name, c);
          exit(EXIT_FAILURE);
        }

      if(link_of(e).io_stream[sclass])
        {
          fprintf(stderr, "%s, "OP_FMT"%s (%s, %s): Duplicate stream\n",
                  file_name, c, name, name_v, name_w);
          exit(EXIT_FAILURE);
        }

      ios = open_stream(name, sclass, e, k);

      ios->next_ios = kb->io_stream[sclass];
      ios->prev_ios = NULL;

      if(ios->next_ios)
        ios->next_ios->prev_ios = ios;

      kb->io_stream[sclass] = ios;

      k = 0;
    }

  if(ferror(fp))
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  if(!feof(fp))
    {
      fprintf(stderr, "%s: Parser error\n", file_name);
      exit(EXIT_FAILURE);
    }

  if(fclose(fp))
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  kb->strictly_causal = strictly_causal;
  kb->soundness_check = soundness_check;
  kb->trace_focus = echo_stdout || logfile_name;
  kb->echo_stdout = echo_stdout;

  if(logfile_name)
    {
      strcpy(file_name, logfile_name);
      strcat(file_name, EVENT_LIST_EXT);
      kb->logfp = fopen(file_name, "w");
      if(!kb->logfp)
        {
          perror(file_name);
          exit(EXIT_FAILURE);
        }
    }

  kb->idle = TRUE;

  return kb;
}

void close_base(k_base *kb)
{
  node *vp, *wp;
  stream *ios, *next_ios;
  stream_class class;

  if(kb->logfp && fclose(kb->logfp))
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  for(class = 0; class < STREAM_CLASSES_NUMBER; class++)
    {
      ios = kb->io_stream[class];
      while(ios)
        {
          next_ios = ios->next_ios;
          close_stream(ios);
          ios = next_ios;
        }
    }

  vp = kb->network;
  while(vp)
    {
      wp = vp->vp;
      free(vp);
      vp = wp;
    }

  free(kb);
}

void init_state(k_base *kb, char *state_name)
{
  FILE *fp;
  char file_name[MAX_STRLEN], name_v[MAX_NAMELEN], name_w[MAX_NAMELEN];
  node *vp, *wp;
  event s;

  strcpy(file_name, state_name);
  strcat(file_name, EVENT_LIST_EXT);
  fp = fopen(file_name, "r");
  if(!fp)
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  while(fscanf(fp, " ( "NAME_FMT" , "NAME_FMT" ) @ "TIME_FMT" ",
               name_v, name_w, &s.t) == 3)
    {
      vp = kb->table[hash(name_v)];
      wp = kb->table[hash(name_w)];

      if(!vp || !wp)
        {
          fprintf(stderr, "%s, %s: Undefined node\n",
                  file_name, !vp? name_v : name_w);
          exit(EXIT_FAILURE);
        }

      s.e = arc_between(vp, wp);
      if(s.e.lc < 0)
        {
          fprintf(stderr, "%s, (%s, %s): Undefined arc\n",
                  file_name, name_v, name_w);
          exit(EXIT_FAILURE);
        }

      if(s.t < 0 || s.t >= TIME_LIMIT)
        {
          fprintf(stderr, "%s, "TIME_FMT": Time out of range\n",
                  file_name, s.t);
          exit(EXIT_FAILURE);
        }

      if(!stated(s))
        state(kb, s);
    }

  if(ferror(fp))
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  if(!feof(fp))
    {
      fprintf(stderr, "%s: Parser error\n", file_name);
      exit(EXIT_FAILURE);
    }

  if(fclose(fp))
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }
}

/******** Toplevel ********/

void trap()
{
  irq = TRUE;
}

void run(char *base_name, char *state_name, char *logfile_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout)
{
  k_base *kb;

  kb = open_base(base_name, logfile_name,
                 strictly_causal, soundness_check, echo_stdout);
  if(state_name)
    init_state(kb, state_name);

  signal(SIGINT, (void (*)())&trap);
  while(!irq && loop(kb));
  signal(SIGINT, SIG_DFL);

  close_base(kb);
}

int main(int argc, char **argv)
{
  char *base_name, *state_name, *logfile_name, *option;
  char default_state_name[MAX_STRLEN], default_logfile_name[MAX_STRLEN];
  bool strictly_causal, soundness_check, echo_stdout;
  int i;

  base_name = state_name = logfile_name = NULL;
  strictly_causal = soundness_check = echo_stdout = FALSE;

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-cdilv] [-I state] [-L log] [base]\n",
                      argv[0]);
              exit(EXIT_SUCCESS);
              break;

            case 'I':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(state_name && state_name != default_state_name)
                {
                  fprintf(stderr, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                state_name = argv[i];
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
              break;

            case 'L':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(logfile_name && logfile_name != default_logfile_name)
                {
                  fprintf(stderr, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                logfile_name = argv[i];
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
              break;

            default:
              do
                {
                  switch(*option)
                    {
                    case 'c':
                      strictly_causal = TRUE;
                      break;

                    case 'd':
                      echo_stdout = TRUE;
                      break;

                    case 'i':
                      if(!state_name)
                        state_name = default_state_name;
                      break;

                    case 'l':
                      if(!logfile_name)
                        logfile_name = default_logfile_name;
                      break;

                    case 'v':
                      soundness_check = TRUE;
                      break;

                    default:
                      fprintf(stderr, "%s, %c: Invalid command line option"
                                      " (%s -h for help)\n",
                              argv[i], *option, argv[0]);
                      exit(EXIT_FAILURE);
                    }
                }
              while(*(++option));
            }
        }
      else
        {
          if(!base_name)
            base_name = argv[i];
          else
            fprintf(stderr, "%s: Extra argument ignored\n", argv[i]);
        }
    }

  if(!base_name)
    base_name = DEFAULT_NAME;

  if(state_name == default_state_name)
    {
      strcpy(state_name, base_name);
      strcat(state_name, STATE_SUFFIX);
    }

  if(logfile_name == default_logfile_name)
    {
      strcpy(logfile_name, base_name);
      strcat(logfile_name, LOG_SUFFIX);
    }

  printf("\nTINX "VER" - Temporal Inference Network eXecutor\n"
         "Design & coding by Andrea Giotti, 1998-1999\n\n");

  run(base_name, state_name, logfile_name,
      strictly_causal, soundness_check, echo_stdout);

  if(irq)
    printf("\nExecution interrupted\n");
  else
    printf("\nEnd of execution\n");

  return EXIT_SUCCESS;
}

