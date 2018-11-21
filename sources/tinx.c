/*
  TINX - Temporal Inference Network eXecutor
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2017
*/

#define NDEBUG
#define INLINE inline
#define ANSI_FILE_IO
/* #define UNIX_FILE_IO */
#define POSIX_IPC_IO
/* #define UNIX_IPC_IO */

#include "tinx.h"

#define VER "6.1.1 (single core)"

const event null_event = {{NULL, no_link}, NULL_TIME};

const char class_symbol[NODE_CLASSES_NUMBER] = CLASS_SYMBOLS;

volatile sig_atomic_t irq = FALSE;

/******** Tools ********/

INLINE event ev_neg(event s)
{
  event r;

  assert(valid(s));

  r.e = arc_neg(s.e);
  r.t = s.t;

  return r;
}

arc arc_between(node *vp, node *wp, link_code lc)
{
  arc e;

  assert(vp && wp);

  e.vp = wp;

  if(vp == wp->pin[lc].e.vp)
    e.lc = lc;
  else
    e.lc = no_link;

  return e;
}

INLINE m_time get_time()
{
  struct timespec ts;

  clock_gettime(CLOCK_TYPE, &ts);

  return ts.tv_sec + 0.000000001 * ts.tv_nsec;
}

unsigned long int hashnode(char *name)
{
  unsigned long int k;
  char *c;

  k = 0;

  while(*name)
    {
      k = k * 10;

      if(isdigit(*name))
        k += *name - '0';
      else
        if(isalpha(*name))
          {
            c = strchr(class_symbol, toupper(*name));
            if(c)
              k += c - class_symbol;
          }

      name++;
    }

  return k;
}

/******** Inference engine ********/

INLINE void state(k_base *kb, event s)
{
  event q, r;

  assert(kb);
  assert(valid(s));
  assert(!is_stated(kb, s));

  if(kb->soundness_check && is_stated(kb, ev_neg(s)))
    {
      fprintf(stderr, "(%s, %s) @ "TIME_FMT": Soundness violation\n",
              arc_neg(s.e).vp->name, s.e.vp->name, s.t);

      close_base(kb);
      exit(EXIT_FAILURE);
    }

  set_stated(kb, s);

  assert(!valid(kb->last_input) || !is_chosen(kb, kb->last_input));

  if(kb->last_input.t < s.t)
    {
      q = kb->last_input;
      r = next(kb, q);
    }
  else
    {
      q = null_event;
      r = kb->focus;
    }

  while(r.t < s.t)
    {
      q = r;
      r = next(kb, q);

      kb->perf.depth++;
    }

  if(s.t == kb->curr_time)
    kb->last_input = q;

  if(r.t == s.t)
    {
      assert(valid(r));

      other(kb, s) = r;
      next(kb, s) = next(kb, r);

      if(kb->last_input.t == r.t)
        kb->last_input = s;
    }
  else
    {
      other(kb, s) = null_event;
      next(kb, s) = r;
    }

  if(valid(q))
    next(kb, q) = s;
  else
    {
      kb->focus = s;
      kb->far = FALSE;
    }

#if !defined NDEBUG
  printf("\t(%s, %s) @ "TIME_FMT"\n", arc_neg(s.e).vp->name, s.e.vp->name, s.t);
#endif
}

INLINE event choose(k_base *kb)
{
  event r, s;
  d_time dt;

  assert(kb);

  s = kb->focus;

  if(valid(s))
    {
      dt = s.t - kb->curr_time;

      if(dt <= kb->bsd4)
        {
          assert(!is_chosen(kb, s));

          set_chosen(kb, s);
          r = other(kb, s);

          if(valid(r))
            {
              next(kb, r) = next(kb, s);
              kb->focus = r;
            }
          else
            kb->focus = next(kb, s);

          if(kb->last_input.t <= s.t)
            kb->last_input = null_event;

#if !defined NDEBUG
          printf("(%s, %s) @ "TIME_FMT" ==>\n", arc_neg(s.e).vp->name, s.e.vp->name, s.t);
#endif
          if(dt < - kb->bsd4)
            s = null_event;
        }
      else
        {
          s = null_event;
          kb->far = TRUE;
        }
    }
  else
    kb->far = TRUE;

  return s;
}

INLINE void process(k_base *kb, event s)
{
  event r;
  link_code lc1, lc2;

  assert(kb);
  assert(valid(s));

  kb->perf.count++;
  switch(s.e.vp->nclass)
    {
    case gate:

      lc1 = (s.e.lc + 1) % LINK_CODES_NUMBER;
      lc2 = (s.e.lc + 2) % LINK_CODES_NUMBER;

      if(s.e.vp->pin[lc1].history[index_of(kb, s)].chosen == phase_of(kb, s))
        {
          r.e = s.e.vp->pin[lc2].e;
          r.t = s.t;

          if(!is_stated(kb, r))
            state(kb, r);
        }
      else
        if(s.e.vp->pin[lc2].history[index_of(kb, s)].chosen == phase_of(kb, s))
          {
            r.e = s.e.vp->pin[lc1].e;
            r.t = s.t;

            if(!is_stated(kb, r))
              state(kb, r);
          }

    break;

    case joint:

      r.t = s.t;

      if(s.e.lc == parent)
        {
          r.e = s.e.vp->pin[left_son].e;

          if(!is_stated(kb, r))
            state(kb, r);

          r.e = s.e.vp->pin[right_son].e;

          if(!is_stated(kb, r))
            state(kb, r);
        }
      else
        {
          r.e = s.e.vp->pin[parent].e;

          if(!is_stated(kb, r))
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
          if(r.t >= s.t && !is_stated(kb, r))
            state(kb, r);
        }
      else
        if(!is_stated(kb, r))
          state(kb, r);

      break;

    default:
      assert(FALSE);
    }
}

INLINE void scan_inputs(k_base *kb)
{
  stream *ios;

  assert(kb);

  if(kb->io_stream[input_stream])
    {
      if(kb->curr_time < kb->io_stream[input_stream]->deadline)
        kb->io_stream[input_stream] = kb->io_stream[input_stream]->next_ios;
      else
        if(kb->input(kb, kb->io_stream[input_stream]))
          {
            kb->io_stream[input_stream]->deadline++;
            kb->io_count[input_stream]--;

            kb->io_stream[input_stream] = kb->io_stream[input_stream]->next_ios;
          }
        else
          if(kb->io_stream[input_stream]->open)
            kb->io_stream[input_stream] = kb->io_stream[input_stream]->next_ios;
          else
            {
              ios = kb->io_stream[input_stream];
              remove_stream(&kb->io_stream[input_stream]);
              close_stream(ios, kb->alpha);

              kb->io_num[input_stream]--;
              kb->io_count[input_stream]--;
              kb->io_open--;

              if(!kb->io_open)
                kb->quiet = TRUE;
            }
    }
}

INLINE void scan_outputs(k_base *kb)
{
  stream *ios;

  assert(kb);

  if(kb->io_stream[output_stream])
    {
      if(kb->curr_time < kb->io_stream[output_stream]->deadline)
        kb->io_stream[output_stream] = kb->io_stream[output_stream]->next_ios;
      else
        if(kb->output(kb, kb->io_stream[output_stream]))
          {
            kb->io_stream[output_stream]->deadline++;
            kb->io_count[output_stream]--;

            kb->io_stream[output_stream] = kb->io_stream[output_stream]->next_ios;
          }
        else
          if(kb->io_stream[output_stream]->open)
            kb->io_stream[output_stream] = kb->io_stream[output_stream]->next_ios;
          else
            {
              ios = kb->io_stream[output_stream];
              remove_stream(&kb->io_stream[output_stream]);
              close_stream(ios, kb->alpha);

              kb->io_num[output_stream]--;
              kb->io_count[output_stream]--;
              kb->io_open--;

              if(!kb->io_open)
                kb->quiet = TRUE;
            }
    }
}

INLINE bool loop(k_base *kb)
{
  event s;

  assert(kb);

  if(kb->slice)
    {
      s = choose(kb);

      if(kb->trace_focus)
        trace(kb, s);

      if(valid(s))
          {
            process(kb, s);
            kb->slice--;

            return FALSE;
          }
    }

  kb->slice = kb->max_slice;

  if(valid(kb->focus) && kb->curr_time - kb->focus.t >= kb->bsd4)
    return FALSE;

  scan_inputs(kb);
  scan_outputs(kb);

  if(!kb->io_count[input_stream] && !kb->io_count[output_stream] && get_time() - kb->time_base >= (kb->curr_time - kb->offset + 1) * kb->step)
    {
      kb->io_count[input_stream] = kb->io_num[input_stream];
      kb->io_count[output_stream] = kb->io_num[output_stream];

      kb->curr_time++;
      kb->far = FALSE;

      if(kb->max_time && kb->curr_time >= kb->max_time)
        kb->exiting = TRUE;
    }

  if(kb->quiet && !valid(kb->focus))
    kb->exiting = TRUE;

  return kb->exiting;
}

/******** Input/Output ********/

bool input_f(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;

  if(get_file(ios->fp, &c) && file_error(ios->fp))
    {
      perror(ios->file_name);
      exit(EXIT_FAILURE);
    }

  switch(strchr(kb->alpha, c) - kb->alpha)
    {
    case eof_symbol:
      reset_file(ios->fp);
      return FALSE;
    break;

    case unknown_symbol:
      return TRUE;
    break;

    case false_symbol:
      s.e = ios->ne;
    break;

    case true_symbol:
      s.e = ios->e;
    break;

    case end_symbol:
      ios->open = FALSE;
      return FALSE;
    break;

    case term_symbol:
      irq = TRUE;
      return FALSE;
    break;

    default:
      fprintf(stderr, "%s, %c (dec %d): Invalid character in stream\n",
              ios->file_name, c, c);
      exit(EXIT_FAILURE);
    }

  if(valid(s))
    {
      if(!is_stated(kb, s))
        state(kb, s);

      return TRUE;
    }
  else
    return FALSE;
}

bool input_m(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;

  if(read_message(ios->chan, &c))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          exit(EXIT_FAILURE);
        }

      ios->errors++;
    }
  else
    ios->errors = 0;

  switch(strchr(kb->alpha, c) - kb->alpha)
    {
    case eof_symbol:
      return FALSE;
    break;

    case unknown_symbol:
      return TRUE;
    break;

    case false_symbol:
      s.e = ios->ne;
    break;

    case true_symbol:
      s.e = ios->e;
    break;

    case end_symbol:
      ios->open = FALSE;
      return FALSE;
    break;

    case term_symbol:
      irq = TRUE;
      return FALSE;
    break;

    default:
      fprintf(stderr, "%s, %c (dec %d): Invalid character in stream\n",
              ios->chan_name, c, c);
      exit(EXIT_FAILURE);
    }

  if(valid(s))
    {
      if(!is_stated(kb, s))
        state(kb, s);

      return TRUE;
    }
  else
    return FALSE;
}

bool output_f(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;
  s.e = ios->ne;

  if(is_stated(kb, s))
    {
      ios->fails = 0;
      c = kb->alpha[false_symbol];
    }
  else
    {
      s.e = ios->e;

      if(is_stated(kb, s))
        {
          ios->fails = 0;
          c = kb->alpha[true_symbol];
        }
      else
        {
          if(kb->io_count[input_stream] || !kb->far)
            return FALSE;
          else
            {
              ios->fails++;

              if(!kb->max_time && ios->fails > kb->bsd4)
                {
                  ios->open = FALSE;
                  return FALSE;
                }

              c = kb->alpha[unknown_symbol];
            }
        }
    }

  if(put_file(ios->fp, &c))
    {
      perror(ios->file_name);
      exit(EXIT_FAILURE);
    }

  if(sync_file(ios->fp))
    {
      perror(ios->file_name);
      exit(EXIT_FAILURE);
    }

  return TRUE;
}

bool output_m(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;
  s.e = ios->ne;

  if(is_stated(kb, s))
    {
      ios->fails = 0;
      c = kb->alpha[false_symbol];
    }
  else
    {
      s.e = ios->e;

      if(is_stated(kb, s))
        {
          ios->fails = 0;
          c = kb->alpha[true_symbol];
        }
      else
        {
          if(kb->io_count[input_stream] || !kb->far)
            return FALSE;
          else
            {
              ios->fails++;

              if(!kb->max_time && ios->fails > kb->bsd4)
                {
                  ios->open = FALSE;
                  return FALSE;
                }

              c = kb->alpha[unknown_symbol];
            }
        }
    }

  if(send_message(ios->chan, &c))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          exit(EXIT_FAILURE);
        }

      ios->errors++;

      if(!kb->sturdy && ios->errors > IO_ERR_LIMIT)
        ios->open = FALSE;

      return FALSE;
    }
  else
    ios->errors = 0;

  return TRUE;
}

void trace(k_base *kb, event s)
{
  char debug[2 * DEBUG_STRLEN + 16];
  arc e1, e2;

  if(valid(s))
    {
      kb->io_busy = TRUE;

      e1 = arc_neg(s.e);
      e2 = s.e;

      *debug = '\0';
      if(e1.vp->debug && e2.vp->debug)
        sprintf(debug, ": %s(%s) --> %s(%s)", e1.lc? "" : "~ ", e1.vp->debug, e2.lc? "~ " : "", e2.vp->debug);

      if(kb->echo_stdout)
        {
          printf(" > "TIME_FMT": (%s, %s) # %d @ "TIME_FMT"%s        \r",
                 kb->curr_time, e1.vp->name, e2.vp->name, e2.lc, s.t, debug);
          fflush(stdout);
        }

      if(kb->logfp)
        {
          if(fprintf(kb->logfp, "(%s, %s) # %d @ "TIME_FMT"%s\n",
                 e1.vp->name, e2.vp->name, e2.lc, s.t, debug) < 0)
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
    if(kb->io_busy)
      {
        kb->io_busy = FALSE;

        if(kb->echo_stdout)
          { 
            printf(" %c\r", valid(kb->focus)? '|' : '*');
            fflush(stdout);
          }
      }
}

stream *open_stream(char *name, stream_class sclass, arc e, d_time offset, bool file_io, char *prefix, char *path)
{
  stream *ios;
  linkage *pin;

  ios = malloc(sizeof(stream));
  if(!ios)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  strcpy(ios->name, name);
  ios->sclass = sclass;

  pin = &link_of(e);

  ios->e = e;
  ios->ne = pin->e;

  if(file_io)
    {
      if(*path)
        {
          strcpy(ios->file_name, path);
          strcat(ios->file_name, "/");
        }
      else
        *(ios->file_name) = '\0';

      strcat(ios->file_name, name);
      strcat(ios->file_name, STREAM_EXT);

      if(sclass == input_stream)
        ios->fp = open_input_file(ios->file_name);
      else
        {
          clean_file(ios->file_name);

          ios->fp = open_output_file(ios->file_name);
        }

      if(!is_file_open(ios->fp))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }
    }
  else
    {
      strcpy(ios->chan_name, prefix);
      strcat(ios->chan_name, name);

      ios->chan = add_queue(ios->chan_name, sclass);

      if(failed_queue(ios->chan))
        {
          perror(ios->chan_name);
          exit(EXIT_FAILURE);
        }
    }

  ios->deadline = offset;
  ios->file_io = file_io;
  ios->fails = 0;
  ios->errors = 0;
  ios->open = TRUE;

  return ios;
}

void close_stream(stream *ios, char *alpha)
{
  if(ios->file_io)
    {
      if(ios->sclass == output_stream && put_file(ios->fp, &alpha[end_symbol]))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }

      if(close_file(ios->fp))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }
    }
  else
    {
      if(ios->sclass == output_stream)
        send_message(ios->chan, &alpha[end_symbol]);

      if(commit_queue(ios->chan))
        {
          perror(ios->chan_name);
          exit(EXIT_FAILURE);
        }

      if(remove_queue(ios->chan_name))
        {
          perror(ios->chan_name);
          exit(EXIT_FAILURE);
        }
    }

  free(ios);
}

void remove_stream(stream **handle)
{
  stream *next_ios;

  next_ios = (*handle)->next_ios;

  next_ios->prev_ios = (*handle)->prev_ios;
  (*handle)->prev_ios->next_ios = next_ios;

  if(*handle == next_ios)
    (*handle) = NULL;
  else
    (*handle) = next_ios;
}

/******** Network setup ********/

node *alloc_node(k_base *kb, char *name)
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
  vp->nclass = null_op;

  for(lc = 0; lc < LINK_CODES_NUMBER; lc++)
    {
      vp->pin[lc].e.vp = NULL;
      vp->pin[lc].e.lc = no_link;

      vp->pin[lc].history = malloc(sizeof(record) * (kb->bsm1 + 1));
      if(!vp->pin[lc].history)
        {
          perror(NULL);
          exit(EXIT_FAILURE);
        }

      for(t = 0; t <= kb->bsm1; t++)
        {
          vp->pin[lc].history[t].stated = NULL_PHASE;
          vp->pin[lc].history[t].chosen = NULL_PHASE;

          vp->pin[lc].history[t].other = null_event;
          vp->pin[lc].history[t].next = null_event;
        }
    }

  vp->debug = NULL;

  return vp;
}

void free_node(k_base *kb, node *vp)
{
  link_code lc;

  for(lc = 0; lc < LINK_CODES_NUMBER; lc++)
    free(vp->pin[lc].history);

  if(vp->debug)
    free(vp->debug);

  free(vp);
}

node *name2node(k_base *kb, char *name, bool create)
{
  node *vp;
  int h, i;

  if(*name)
    {
      h = hashnode(name) % HASH_SIZE;
      i = 0;

      while((vp = kb->table[h][i]))
        {
          if(strcmp(vp->name, name))
            {
              i++;

              if(i == HASH_DEPTH)
                {
                  fprintf(stderr, "%s, %s: Node names generate duplicate hashes\n",
                      vp->name, name);

                  exit(EXIT_FAILURE);
                }
            }
          else
            return vp;
        }

      if(create)
        {
          vp = alloc_node(kb, name);
          assert(vp);

          vp->vp = kb->network;
          kb->network = vp;

          kb->table[h][i] = vp;

#if !defined NDEBUG
          printf("%s -> "HASH_FMT"\n", name, h);
#endif
          return vp;
        }
      else
        return NULL;
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
              fprintf(stderr, "%s: Edge mismatch in node declarations\n",
                      vp->name);
              exit(EXIT_FAILURE);
            }
        }

  for(vp = network; vp; vp = vp->vp)
    {
      for(lc = 0; lc < LINK_CODES_NUMBER && vp->pin[lc].e.lc >= 0; lc++);

      switch(vp->nclass)
        {
        case gate:
        case joint:
          assert(lc <= 3);
          if(lc < 3)
            {
              fprintf(stderr, "%s: Undefined edge in node declarations\n",
                      vp->name);
              exit(EXIT_FAILURE);
            }
        break;

        case delay:
          if(lc < 2)
            {
              fprintf(stderr, "%s: Undefined edge in node declarations\n",
                      vp->name);
              exit(EXIT_FAILURE);
            }
          else
            if(lc > 2)
              {
                fprintf(stderr, "%s: Edge mismatch in node declarations\n",
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
             class_symbol[vp->nclass], vp->k, vp->pin[parent].e.vp->name);

      if(vp->pin[left_son].e.vp)
        printf(", %s", vp->pin[left_son].e.vp->name);

      if(vp->pin[right_son].e.vp)
        printf(", %s", vp->pin[right_son].e.vp->name);

      printf("\n");
#endif
    }
}

k_base *open_base(char *base_name, char *logfile_name, char *xref_name, bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool sturdy,
                  int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *alpha)
{
  k_base *kb;
  FILE *fp;
  char file_name[MAX_STRLEN], name[MAX_NAMEBUF], type[MAX_NAMEBUF],
       up[MAX_NAMEBUF], left[MAX_NAMEBUF], right[MAX_NAMEBUF],
       name_v[MAX_NAMEBUF], name_w[MAX_NAMEBUF],
       debug[DEBUG_STRLEN + 1];
  char c;
  d_time k, offset;
  node *vp, *wp;
  node_class nclass;
  int num_nodes[NODE_CLASSES_NUMBER];
  stream *ios;
  stream_class sclass;
  arc e;
  int bufsiz;
  link_code lc;

  kb = malloc(sizeof(k_base));
  if(!kb)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  kb->focus = null_event;
  kb->last_input = null_event;

  bufsiz = 1 << bufexp;
  kb->bsm1 = bufsiz - 1;
  kb->bsbt = bufexp;
  kb->bsd4 = bufsiz / 4;

  kb->network = NULL;

  memset(kb->table, 0, sizeof(node *) * HASH_SIZE * HASH_DEPTH);
  memset(kb->io_stream, 0, sizeof(stream *) * STREAM_CLASSES_NUMBER);

  strcpy(file_name, base_name);
  strcat(file_name, NETWORK_EXT);
  fp = fopen(file_name, "r");
  if(!fp)
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  for(nclass = 0; nclass < NODE_CLASSES_NUMBER; nclass++)
    num_nodes[nclass] = 0;

  *left = *right = '\0';
  while(fscanf(fp, " "NAME_FMT" : "NAME_FMT" ; "NAME_FMT" , "
                   NAME_FMT" , "NAME_FMT" ",
               name, type, up, left, right) >= 3)
    {
      if(strlen(name) > MAX_NAMELEN)
        {
          fprintf(stderr, "%s, %s: Name too long\n",
                  file_name, name);
          exit(EXIT_FAILURE);
        }

      k = 1;
      sscanf(type, OP_FMT""ARG_FMT, &c, &k);

      nclass = strchr(class_symbol, toupper(c)) - class_symbol;
      if(nclass < 0)
        {
          fprintf(stderr, "%s, %s, "OP_FMT": Invalid node class\n",
                  file_name, name, c);
          exit(EXIT_FAILURE);
        }

      if(nclass == delay && (k <= - kb->bsd4 || k >= kb->bsd4))
        {
          fprintf(stderr, "%s, %s, "TIME_FMT": Delay out of range\n",
                  file_name, name, k);
          exit(EXIT_FAILURE);
        }

      vp = name2node(kb, name, TRUE);
      if(vp->nclass >= 0)
        {
          fprintf(stderr, "%s, %s: Duplicate node\n", file_name, name);
          exit(EXIT_FAILURE);
        }

      vp->nclass = nclass;
      vp->k = k;

      vp->pin[parent].e.vp = name2node(kb, up, TRUE);
      vp->pin[left_son].e.vp = name2node(kb, left, TRUE);
      vp->pin[right_son].e.vp = name2node(kb, right, TRUE);

      num_nodes[nclass]++;

      *left = *right = '\0';
    }

  if(ferror(fp))
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  kb->perf.edges = (3 * (num_nodes[gate] + num_nodes[joint]) + 2 * num_nodes[delay]) / 2;

  thread_network(kb->network);

  if(quiet)
    {
      kb->input = NULL;
      kb->output = NULL;
    }
  else
    if(file_io)
      {
        kb->input = &input_f;
        kb->output = &output_f;
      }
    else
      {
        kb->input = &input_m;
        kb->output = &output_m;
      }

  kb->offset = 0;
  kb->io_num[input_stream] = 0;
  kb->io_num[output_stream] = 0;
  kb->io_open = 0;

  strcpy(kb->alpha, alpha);

  offset = NULL_TIME;
  ios = NULL;
  k = 0;

  while(fscanf(fp, " "OP_FMT" "FUN_FMT" ( "NAME_FMT" , "NAME_FMT" ) # %d @ "TIME_FMT" ",
               &c, name, name_v, name_w, &lc, &k) >= 5)
    {
      if(strlen(name) > MAX_NAMELEN)
        {
          fprintf(stderr, "%s, %s: Name too long\n",
                  file_name, name);
          exit(EXIT_FAILURE);
        }

      vp = name2node(kb, name_v, FALSE);
      wp = name2node(kb, name_w, FALSE);

      if(!vp || !wp)
        {
          fprintf(stderr, "%s, %s: Undefined node\n",
                  file_name, !vp? name_v : name_w);
          exit(EXIT_FAILURE);
        }

      e = arc_between(vp, wp, lc);
      if(e.lc < 0)
        {
          fprintf(stderr, "%s, (%s, %s): Undefined edge\n",
                  file_name, name_v, name_w);
          exit(EXIT_FAILURE);
        }

      if(offset == NULL_TIME)
        {
          if(k <= - kb->bsd4 || k >= kb->bsd4)
            {
              fprintf(stderr, "%s, "TIME_FMT": Offset out of range\n",
                      file_name, k);
               exit(EXIT_FAILURE);
            }

          offset = k;
        }
      else
        if(offset != k)
          {
            fprintf(stderr, "%s, "TIME_FMT": Different signal offsets not supported\n",
                    file_name, k);
            exit(EXIT_FAILURE);
          }

      switch(c)
        {
        case '!':
          sclass = input_stream;
        break;

        case '?':
        case '.':
          sclass = output_stream;
        break;

        default:
          fprintf(stderr, "%s, "OP_FMT": Invalid stream class\n", file_name, c);
          exit(EXIT_FAILURE);
        }

      if(offset != NULL_TIME)
        kb->offset = offset;

      if(!quiet)
        {
          ios = open_stream(name, sclass, e, kb->offset, file_io, prefix, path);

          kb->io_num[sclass]++;
          kb->io_open++;

          ios->next_ios = kb->io_stream[sclass];

          if(ios->next_ios)
            {
              ios->prev_ios = ios->next_ios->prev_ios;

              ios->next_ios->prev_ios = ios;
              ios->prev_ios->next_ios = ios;
            }
          else
            {
              ios->next_ios = ios;
              ios->prev_ios = ios;
            }

          kb->io_stream[sclass] = ios;
        }

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

  if(xref_name)
    {
      strcpy(file_name, xref_name);
      strcat(file_name, XREF_EXT);
      fp = fopen(file_name, "r");
      if(!fp)
        {
          perror(file_name);
          exit(EXIT_FAILURE);
        }

      while(fscanf(fp, " "NAME_FMT" : %[^\n]\n", name, debug) == 2)
        {
          if(strlen(name) > MAX_NAMELEN)
            {
              fprintf(stderr, "%s, %s: Name too long\n", file_name, name);
              exit(EXIT_FAILURE);
            }

          if(strlen(debug) > DEBUG_STRLEN)
            {
              fprintf(stderr, "%s, %s: Symbol too long\n", file_name, debug);
              exit(EXIT_FAILURE);
            }

          vp = name2node(kb, name, FALSE);

          if(!vp)
            {
              fprintf(stderr, "%s, %s: Undefined node\n", file_name, name);
              exit(EXIT_FAILURE);
            }

          vp->debug = malloc(strlen(debug) + 1);
          if(!vp->debug)
            {
              perror(NULL);
              exit(EXIT_FAILURE);
            }

          strcpy(vp->debug, debug);
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

  kb->strictly_causal = strictly_causal;
  kb->soundness_check = soundness_check;
  kb->quiet = quiet || !ios;
  kb->sturdy = sturdy;
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

  kb->max_slice = max(1, kb->perf.edges / (IO_INFERENCE_RATIO * max(1, max(kb->io_num[input_stream], kb->io_num[output_stream]))));
  kb->slice = kb->max_slice;

  kb->io_count[input_stream] = kb->io_num[input_stream];
  kb->io_count[output_stream] = kb->io_num[output_stream];

  kb->curr_time = kb->offset;
  kb->max_time = kb->offset + max_time;

  kb->far = TRUE;
  kb->io_busy = FALSE;
  kb->exiting = FALSE;

  kb->step = step;

  kb->perf.count = 0;
  kb->perf.depth = 0;

  return kb;
}

void close_base(k_base *kb)
{
  node *vp, *wp;
  stream *ios, *next_ios;
  stream_class sclass;

  if(kb->logfp && fclose(kb->logfp))
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  if(!kb->quiet)
    for(sclass = 0; sclass < STREAM_CLASSES_NUMBER; sclass++)
      {
        ios = kb->io_stream[sclass];
        if(ios)
          do
            {
              next_ios = ios->next_ios;

              close_stream(ios, kb->alpha);

              ios = next_ios;
            }
          while(ios != kb->io_stream[sclass]);
      }

  vp = kb->network;
  while(vp)
    {
      wp = vp->vp;
      free_node(kb, vp);
      vp = wp;
    }

  free(kb);
}

void init_state(k_base *kb, char *state_name)
{
  FILE *fp;
  char file_name[MAX_STRLEN], name_v[MAX_NAMEBUF], name_w[MAX_NAMEBUF];
  node *vp, *wp;
  event s;
  link_code lc;

  strcpy(file_name, state_name);
  strcat(file_name, EVENT_LIST_EXT);
  fp = fopen(file_name, "r");
  if(!fp)
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  while(fscanf(fp, " ( "NAME_FMT" , "NAME_FMT" ) # %d @ "TIME_FMT" : %*[^\n]\n",
               name_v, name_w, &lc, &s.t) >= 4)
    {
      vp = name2node(kb, name_v, FALSE);
      wp = name2node(kb, name_w, FALSE);

      if(!vp || !wp)
        {
          fprintf(stderr, "%s, %s: Undefined node\n",
                  file_name, !vp? name_v : name_w);
          exit(EXIT_FAILURE);
        }

      s.e = arc_between(vp, wp, lc);
      if(s.e.lc < 0)
        {
          fprintf(stderr, "%s, (%s, %s): Undefined edge\n",
                  file_name, name_v, name_w);
          exit(EXIT_FAILURE);
        }

      if(s.t <= - kb->bsd4 || s.t >= kb->bsd4)
        {
          fprintf(stderr, "%s, "TIME_FMT": Time out of range\n",
                  file_name, s.t);
          exit(EXIT_FAILURE);
        }

      if(!is_stated(kb, s))
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

info run(char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sturdy,
         int bufexp, d_time max_time, m_time step, m_time origin, char *prefix, char *path, char *alpha)
{
  k_base *kb;
  info perf;
  struct sched_param spar;

  kb = open_base(base_name, logfile_name, xref_name,
                 strictly_causal, soundness_check, echo_stdout, file_io, quiet, sturdy, bufexp, max_time, step, prefix, path, alpha);
  if(state_name)
    init_state(kb, state_name);

  if(hard)
    {
      spar.sched_priority = sched_get_priority_max(SCHED_RR);
      if(sched_setscheduler(0, SCHED_RR, &spar))
        {
          perror("Scheduler error");
          exit(EXIT_FAILURE);
        }
    }

  signal(SIGINT, (void (*)())&trap);

  if(origin > 0)
    kb->time_base = origin;
  else
    kb->time_base = get_time();

  while(!irq && !loop(kb));

  kb->perf.ticks = get_time() - kb->time_base;

  signal(SIGINT, SIG_DFL);

  kb->perf.horizon = kb->curr_time;

  perf = kb->perf;

  close_base(kb);

  return perf;
}

int main(int argc, char **argv)
{
  char *base_name, *state_name, *logfile_name, *xref_name, *option, *ext, *prefix, *path;
  char default_state_name[MAX_STRLEN], default_logfile_name[MAX_STRLEN], default_xref_name[MAX_STRLEN], alpha[SYMBOL_NUMBER + 1];
  bool strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sturdy;
  int i, k, n;
  info perf;
  d_time max_time;
  m_time step, default_step, origin;
  int bufexp;

  base_name = state_name = logfile_name = xref_name = NULL;
  strictly_causal = soundness_check = echo_stdout = file_io = quiet = hard = sturdy = FALSE;
  bufexp = DEFAULT_BUFEXP;
  max_time = 0;
  origin = 0;
  default_step = -1;
  prefix = MAGIC_PREFIX;
  path = "";
  strcpy(alpha, IO_SYMBOLS);

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-cdfilqsvxy] [-a alphabet] [-e prefix] [-g origin] [-I state] [-L log] [-p path] [-r core] [-t step] [-X symbols] [-z horizon] [base]\n",
                      argv[0]);
              exit(EXIT_SUCCESS);
            break;

            case 'a':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  n = strlen(argv[i]);
                  if(n >= SYMBOL_NUMBER)
                    {
                      fprintf(stderr, "%s: Argument too long\n", argv[i]);
                      exit(EXIT_FAILURE);
                    }

                  for(k = 0; k < n; k++)
                    alpha[k] = argv[i][k];
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'e':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  prefix = argv[i];
                  if(prefix[0] != '/')
                    {
                      fprintf(stderr, "%s: Invalid argument\n", prefix);
                      exit(EXIT_FAILURE);
                    }
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'g':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  origin = atof(argv[i]);
                  if(origin <= 0)
                    {
                      fprintf(stderr, "%f: Argument out of range\n", origin);
                      exit(EXIT_FAILURE);
                    }
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
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

              ext = strstr(state_name, EVENT_LIST_EXT);
              if(ext && !strcmp(ext, EVENT_LIST_EXT))
                *ext = '\0';
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

              ext = strstr(logfile_name, EVENT_LIST_EXT);
              if(ext && !strcmp(ext, EVENT_LIST_EXT))
                *ext = '\0';
            break;

            case 'n':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  if(atoi(argv[i]) != 1)
                    fprintf(stderr, "Monothread version, ignoring switch -n\n");
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'p':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                path = argv[i]; 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'r':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  bufexp = atoi(argv[i]);
                  if(bufexp < 2 || bufexp > 30)
                    {
                      fprintf(stderr, "%d: Argument out of range\n", bufexp);
                      exit(EXIT_FAILURE);
                    }
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 't':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  default_step = atof(argv[i]);
                  if(default_step < 0)
                    {
                      fprintf(stderr, "%f: Argument out of range\n", default_step);
                      exit(EXIT_FAILURE);
                    }
                } 
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }
            break;

            case 'X':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(xref_name && xref_name != default_xref_name)
                {
                  fprintf(stderr, "%s: Duplicate option\n", argv[i]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                xref_name = argv[i];
              else
                {
                  fprintf(stderr, "%s: Missing argument\n", argv[--i]);
                  exit(EXIT_FAILURE);
                }

              ext = strstr(xref_name, XREF_EXT);
              if(ext && !strcmp(ext, XREF_EXT))
                *ext = '\0';
            break;

            case 'z':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                {
                  max_time = atol(argv[i]);
                  if(max_time < 0)
                    {
                      fprintf(stderr, TIME_FMT": Argument out of range\n", max_time);
                      exit(EXIT_FAILURE);
                    }
                } 
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

                    case 'f':
                      file_io = TRUE;
                    break;

                    case 'i':
                      if(!state_name)
                        state_name = default_state_name;
                    break;

                    case 'l':
                      if(!logfile_name)
                        logfile_name = default_logfile_name;
                    break;

                    case 'q':
                      quiet = TRUE;
                    break;

                    case 's':
                      hard = TRUE;
                    break;

                    case 'v':
                      soundness_check = TRUE;
                    break;

                    case 'x':
                      if(!xref_name)
                        xref_name = default_xref_name;
                    break;

                    case 'y':
                      sturdy = TRUE;
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
            {
              base_name = argv[i];

              ext = strstr(base_name, NETWORK_EXT);
              if(ext && !strcmp(ext, NETWORK_EXT))
                *ext = '\0';
            }
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

  if(xref_name == default_xref_name)
    strcpy(xref_name, base_name);

  if(default_step < 0)
    step = quiet? 0 : DEFAULT_STEP_SEC;
  else
    step = default_step;

  printf("\nTINX "VER" - Temporal Inference Network eXecutor\n"
         "Design & coding by Andrea Giotti, 1998-1999, 2016-2018\n\n");

  fflush(stdout);

  perf = run(base_name, state_name, logfile_name, xref_name,
      strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sturdy, bufexp, max_time, step, origin, prefix, path, alpha);

  printf("\n%s\n%lu logical inferences (%.3f %% of %d x "TIME_FMT") in %.3f seconds, %.3f KLIPS, %lu depth (avg %.3f)\n",
	irq? "Execution interrupted" : "End of execution",
	perf.count,
	perf.horizon && perf.edges? 100.0 * perf.count / (perf.horizon * perf.edges) : 0,
	perf.edges,
	perf.horizon,
	perf.ticks,
	perf.ticks? perf.count / (1000 * perf.ticks) : 0,
	perf.depth,
	perf.count? (float)perf.depth / perf.count : 0);

  return EXIT_SUCCESS;
}


