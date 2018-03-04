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

#include "tinx_mt.h"

#define VER "4.2.2 MT (multiple cores)"

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

INLINE m_time get_time()
{
  struct timespec ts;

  clock_gettime(CLOCK_TYPE, &ts);

  return ts.tv_sec + 0.000000001 * ts.tv_nsec;
}

int gcd(int p, int q)
{
  int r;

  while(q)
    {
      r = p % q;

      p = q;
      q = r;
    }

  return p;
}

/******** Inference engine ********/

INLINE void state(k_base *kb, event s, int tid)
{
  event q, r;
  priv_vars *pvp;

  assert(kb);
  assert(valid(s));
  assert(!is_stated(kb, s));

  if(kb->soundness_check && is_stated(kb, ev_neg(s)))
    {
      fprintf(stderr, "(%s, %s) @ "TIME_FMT": Soundness violation\n",
              arc_neg(s.e).vp->name, s.e.vp->name, s.t);

      irq = TRUE;
      return;
    }

  set_stated(kb, s);

  pvp = &kb->pv[tid];

  lock_delta(kb, tid);

  pvp->delta_len++;

  assert(!valid(pvp->last_input) || !is_chosen(pvp->last_input));

  if(pvp->last_input.t < s.t)
    {
      q = pvp->last_input;
      r = next(kb, q);
    }
  else
    {
      q = null_event;
      r = pvp->focus;
    }

  while(r.t < s.t)
    {
      q = r;
      r = next(kb, q);

      pvp->depth++;
    }

  if(s.t == kb->curr_time)
    pvp->last_input = q;

  if(r.t == s.t)
    {
      assert(valid(r));

      other(kb, s) = r;
      next(kb, s) = next(kb, r);

      if(pvp->last_input.t == r.t)
        pvp->last_input = s;
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
      pvp->focus = s;

      pvp->passed = kb->min_sigma < s.t;
      pvp->dstate = normal;
    }

  unlock_delta(kb, tid);

#if !defined NDEBUG
  printf("\t#%d (%s, %s) @ "TIME_FMT"\n", tid, arc_neg(s.e).vp->name, s.e.vp->name, s.t);
#endif
}

INLINE event choose(k_base *kb, int tid)
{
  event r, s;
  d_time dt;
  priv_vars *pvp;

  assert(kb);

  pvp = &kb->pv[tid];

  lock_delta(kb, tid);

  s = pvp->focus;

  if(valid(s))
    {
      assert(!is_chosen(kb, s));
      assert(!kb->strictly_causal || s.t >= kb->anchor_time);

      dt = s.t - kb->anchor_time;

      if(dt <= kb->bsd4)
        {
          set_chosen(kb, s);
          r = other(kb, s);

          if(valid(r))
            {
              next(kb, r) = next(kb, s);
              pvp->focus = r;
            }
          else
            pvp->focus = next(kb, s);

          if(pvp->last_input.t <= s.t)
            pvp->last_input = null_event;
 
          assert(pvp->delta_len > 0);

          pvp->delta_len--;
          pvp->passed = kb->min_sigma < s.t;

#if !defined NDEBUG
          printf("#%d (%s, %s) @ "TIME_FMT" ==>\n", tid, arc_neg(s.e).vp->name, s.e.vp->name, s.t);
#endif
          if(dt < - kb->bsd4)
            s = null_event;
        }
      else
        {
          assert(pvp->delta_len > 0);
 
          s = null_event;

          pvp->passed = TRUE;
          pvp->dstate = far;
        }
    }
  else
    {
      assert(pvp->delta_len == 0);

      pvp->passed = TRUE;
      pvp->dstate = empty;
    }

  unlock_delta(kb, tid);

  return s;
}

INLINE void process(k_base *kb, event s, int tid)
{
  event r;
  link_code lc1, lc2;

  assert(kb);
  assert(valid(s));

  kb->pv[tid].count++;

  switch(s.e.vp->nclass)
    {
    case gate:

      lc1 = (s.e.lc + 1) % LINK_CODES_NUMBER;
      lc2 = (s.e.lc + 2) % LINK_CODES_NUMBER;

      if(s.e.vp->pin[lc1].history[index_of(kb, s)].chosen == phase_of(kb, s))
        {
          r.e = s.e.vp->pin[lc2].e;
          r.t = s.t;

          safe_state(kb, r, tid);
        }
      else
        if(s.e.vp->pin[lc2].history[index_of(kb, s)].chosen == phase_of(kb, s))
          {
            r.e = s.e.vp->pin[lc1].e;
            r.t = s.t;

            safe_state(kb, r, tid);
          }

    break;

    case joint:

      r.t = s.t;

      if(s.e.lc == parent)
        {
          r.e = s.e.vp->pin[left_son].e;

          safe_state(kb, r, tid);

          r.e = s.e.vp->pin[right_son].e;

          safe_state(kb, r, get_tid(kb, tid));
        }
      else
        {
          r.e = s.e.vp->pin[parent].e;

          safe_state(kb, r, tid);
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
          if(r.t >= s.t)
            safe_state(kb, r, tid);
        }
      else
        safe_state(kb, r, tid);

      break;

    default:
      assert(FALSE);
    }
}

INLINE void scan_inputs(k_base *kb)
{
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
              remove_stream(&kb->io_stream[input_stream]);

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

            kb->fails = 0;
            kb->errors = 0;
          }
        else
          {
            if(kb->io_err)
              kb->errors++;
            else
              if(kb->last_far == kb->curr_time)
                kb->fails++;

            if(kb->io_num[input_stream] || (kb->fails <= kb->io_count[output_stream] && (kb->sturdy || kb->errors <= IO_ERR_LIMIT)))
              kb->io_stream[output_stream] = kb->io_stream[output_stream]->next_ios;
            else
              {
                remove_stream(&kb->io_stream[output_stream]);

                kb->io_num[output_stream]--;
                kb->io_count[output_stream]--;
                kb->io_open--;

                if(!kb->io_open)
                  kb->quiet = TRUE;
              }
          }
    }
}

INLINE bool loop(k_base *kb, int tid)
{
  event s;

  assert(kb);

  s = choose(kb, tid);

  if(kb->trace_focus)
    trace(kb, s, tid);

  if(valid(s))
    process(kb, s, tid);

  if(kb->pv[tid].passed)
    {
      kb->pv[tid].halts++;
      barrier(kb, tid);
    }

  return kb->exiting;
}

INLINE bool loop_io(k_base *kb, int tid)
{
  assert(kb);

  if(kb->curr_time - kb->anchor_time < kb->bsd4)
    {
      scan_inputs(kb);
      scan_outputs(kb);

      if(!kb->io_count[input_stream] && !kb->io_count[output_stream] && get_time() - kb->time_base >= (kb->curr_time - kb->offset + 1) * kb->step)
        {
          kb->io_count[input_stream] = kb->io_num[input_stream];
          kb->io_count[output_stream] = kb->io_num[output_stream];

          kb->curr_time++;

          if(kb->max_time > 0 && kb->curr_time >= kb->max_time)
            kb->exiting = TRUE;
        }
    }

  if(!kb->io_togo)
    {
      kb->io_togo = kb->io_slice;
      barrier(kb, tid);
    }
  else
    kb->io_togo--;

  if(kb->quiet && kb->last_empty == kb->curr_time)
    kb->exiting = TRUE;

  return kb->exiting;
}

INLINE void barrier(k_base *kb, int tid)
{
  int tid_2;
  bool all_empty, all_far;
  d_time new_anchor;

  lock_barrier(kb);

  if(kb->barrier_count < kb->num_threads)
    {
      kb->barrier_count++;

      kb->done[tid] = TRUE;
      do
        wait_deadline(kb, tid);
      while(kb->done[tid]);
    }
  else
    {
      kb->min_sigma = NULL_TIME;
      for(tid_2 = 0; tid_2 < kb->num_threads; tid_2++)
        if(kb->min_sigma > kb->pv[tid_2].focus.t)
          kb->min_sigma = kb->pv[tid_2].focus.t;

      new_anchor = min(kb->min_sigma, kb->curr_time);

      if(kb->anchor_time < new_anchor)
        {
          kb->anchor_time = new_anchor;

          for(tid_2 = 0; tid_2 < kb->num_threads; tid_2++)
            if(kb->pv[tid_2].dstate == far)
              kb->pv[tid_2].dstate = normal;
        }

      all_empty = TRUE;
      all_far = TRUE;

      for(tid_2 = 0; tid_2 < kb->num_threads; tid_2++)
        if(kb->pv[tid_2].dstate != empty)
          {
            all_empty = FALSE;

            if(kb->pv[tid_2].dstate == normal)
              {
                all_far = FALSE;

                if(kb->done[tid_2])
                  {
                    kb->barrier_count--;

                    kb->done[tid_2] = FALSE;
                    signal_deadline(kb, tid_2);
                  }
              }
          }

      if(kb->done[kb->num_threads])
        {
          kb->barrier_count--;

          kb->done[kb->num_threads] = FALSE;
          signal_deadline(kb, kb->num_threads);
        }

      if(!kb->io_count[input_stream])
        {
          if(all_empty)
            kb->last_empty = kb->curr_time;

          if(all_far)
            kb->last_far = kb->curr_time;
        }
    }

  unlock_barrier(kb);
}

INLINE int get_tid(k_base *kb, int tid)
{
  int tid_1, tid_2;

  tid_1 = kb->pv[tid].curr_tid;
  tid_2 = (tid_1 + 1) % kb->num_threads;

  if(kb->pv[tid_2].delta_len < kb->pv[tid_1].delta_len)
    kb->pv[tid].curr_tid = tid_2;

  return tid_1;
}

INLINE int get_tid_io(k_base *kb)
{
  int tid_1, tid_2;

  tid_1 = kb->io_curr_tid;
  tid_2 = (tid_1 + 1) % kb->num_threads;

  if(kb->pv[tid_2].delta_len < kb->pv[tid_1].delta_len)
    kb->io_curr_tid = tid_2;

  return tid_1;
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
      irq = TRUE;
      return FALSE;
    }

  switch(c)
    {
    case EOF:
      reset_file(ios->fp);
      return FALSE;
    break;

    case UNKNOWN_CHAR:
    case '\0':
      return TRUE;
    break;

    case LO_CHAR:
      s.e = ios->pin->e;
    break;

    case HI_CHAR:
      s.e = arc_neg(ios->pin->e);
    break;

    case END_CHAR:
      ios->open = FALSE;
      return FALSE;
    break;

    case TERM_CHAR:
      irq = TRUE;
      return FALSE;
    break;

    default:
      fprintf(stderr, "%s, %c (dec %d): Invalid character in stream\n",
              ios->file_name, c, c);
      irq = TRUE;
      return FALSE;
    }

  if(valid(s))
    {
      safe_state(kb, s, get_tid_io(kb));

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
          irq = TRUE;
          return FALSE;
        }

      kb->io_err = TRUE;
    }
  else
    kb->io_err = FALSE;

  switch(c)
    {
    case EOF:
      return FALSE;
    break;

    case UNKNOWN_CHAR:
    case '\0':
      return TRUE;
    break;

    case LO_CHAR:
      s.e = ios->pin->e;
    break;

    case HI_CHAR:
      s.e = arc_neg(ios->pin->e);
    break;

    case END_CHAR:
      ios->open = FALSE;
      return FALSE;
    break;

    case TERM_CHAR:
      irq = TRUE;
      return FALSE;
    break;

    default:
      fprintf(stderr, "%s, %c (dec %d): Invalid character in stream\n",
              ios->file_name, c, c);
      irq = TRUE;
      return FALSE;
    }

  if(valid(s))
    {
      safe_state(kb, s, get_tid_io(kb));

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
  s.e = ios->pin->e;

  if(is_stated(kb, s))
    c = LO_CHAR;
  else
    {
      s.e = arc_neg(ios->pin->e);

      if(is_stated(kb, s))
        c = HI_CHAR;
      else
        {
          if(kb->io_num[input_stream] && kb->last_far == kb->curr_time)
            c = UNKNOWN_CHAR;
          else
            return FALSE;
        }
    }

  if(put_file(ios->fp, &c))
    {
      perror(ios->file_name);
      irq = TRUE;
      return FALSE;
    }

  if(sync_file(ios->fp))
    {
      perror(ios->file_name);
      irq = TRUE;
      return FALSE;
    }

  return TRUE;
}

bool output_m(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;
  s.e = ios->pin->e;

  if(is_stated(kb, s))
    c = LO_CHAR;
  else
    {
      s.e = arc_neg(ios->pin->e);

      if(is_stated(kb, s))
        c = HI_CHAR;
      else
        {
          if(kb->io_num[input_stream] && kb->last_far == kb->curr_time)
            c = UNKNOWN_CHAR;
          else
            return FALSE;
        }
    }

  if(send_message(ios->chan, &c))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          irq = TRUE;
          return FALSE;
        }

      kb->io_err = TRUE;

      return FALSE;
    }
  else
    kb->io_err = FALSE;

  return TRUE;
}

void trace(k_base *kb, event s, int tid)
{
  char buffer[MAX_STRLEN], buffer2[MAX_STRLEN], debug[DEBUG_STRLEN];
  arc e1, e2;
  int tid_2;

  if(valid(s))
    {
      kb->pv[tid].io_busy = TRUE;

      e1 = arc_neg(s.e);
      e2 = s.e;

      *debug = '\0';
      if(e1.vp->debug && e2.vp->debug)
        sprintf(debug, ": %s - %s", e1.vp->debug, e2.vp->debug);

      if(kb->echo_stdout)
        {
          if(kb->echo_debug)
            {
              strcpy(buffer, " [");

              for(tid_2 = 0; tid_2 < kb->num_threads; tid_2++)
                {
                  if(valid(kb->pv[tid_2].focus))
                    sprintf(buffer2, " "TIME_FMT" (%d)", kb->pv[tid_2].focus.t, kb->pv[tid_2].delta_len);
                  else
                    sprintf(buffer2, " * (%d)", kb->pv[tid_2].delta_len);

                  strcat(buffer, buffer2);
                }

              sprintf(buffer2, " ] {"TIME_FMT"}", kb->anchor_time);
              strcat(buffer, buffer2);
            }
          else
            *buffer = '\0';

          printf("#%03d > "TIME_FMT": (%s, %s) @ "TIME_FMT"%s%s        \r",
                 tid, kb->curr_time, e1.vp->name, e2.vp->name, s.t, buffer, debug);
          fflush(stdout);
        }

      if(kb->logfp)
        {
          if(fprintf(kb->logfp, "(%s, %s) @ "TIME_FMT"%s\n",
                 e1.vp->name, e2.vp->name, s.t, debug) < 0)
            {
              perror(NULL);

              irq = TRUE;
            }

          if(fflush(kb->logfp))
            {
              perror(NULL);

              irq = TRUE;
            }
        }
    }
  else
    if(kb->pv[tid].io_busy)
      {
        kb->pv[tid].io_busy = FALSE;

        if(kb->echo_stdout)
          { 
            printf("#%03d %c\r", tid, kb->pv[tid].dstate != empty? '|' : '*');
            fflush(stdout);
          }
      }
}

stream *open_stream(char *name, stream_class sclass, arc e, d_time offset, bool file_io)
{
  stream *ios;

  ios = malloc(sizeof(stream));
  if(!ios)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  strcpy(ios->name, name);
  ios->sclass = sclass;

  ios->pin = &link_of(e);
  ios->pin->io_stream[sclass] = ios;
  link_of(ios->pin->e).io_stream[sclass] = ios;

  if(file_io)
    {
      strcpy(ios->file_name, name);
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
      strcpy(ios->chan_name, MAGIC_PREFIX);
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
  ios->open = TRUE;

  return ios;
}

void close_stream(stream *ios)
{
  char c;

  ios->pin->io_stream[ios->sclass] = NULL;
  link_of(ios->pin->e).io_stream[ios->sclass] = NULL;

  c = END_CHAR;

  if(ios->file_io)
    {
      if(ios->sclass == output_stream && put_file(ios->fp, &c))
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
        send_message(ios->chan, &c);

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

  close_stream(*handle);

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

          pthread_mutex_init(&vp->pin[lc].history[t].mutex, NULL);
        }

      vp->pin[lc].io_stream[input_stream] = NULL;
      vp->pin[lc].io_stream[output_stream] = NULL;
    }

  vp->debug = NULL;

  return vp;
}

void free_node(k_base *kb, node *vp)
{
  link_code lc;
  d_time t;

  for(lc = 0; lc < LINK_CODES_NUMBER; lc++)
    {
      for(t = 0; t <= kb->bsm1; t++)
        pthread_mutex_destroy(&vp->pin[lc].history[t].mutex);

      free(vp->pin[lc].history);
    }

  if(vp->debug)
    free(vp->debug);

  free(vp);
}

node *name2node(k_base *kb, char *name)
{
  node *vp;
  int h, i;

  if(*name)
    {
      h = hash(name) % HASH_SIZE;
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

k_base *open_base(char *base_name, char *logfile_name, char *xref_name, bool strictly_causal, bool soundness_check, bool echo_stdout, bool echo_debug, bool file_io, bool quiet, bool sturdy,
                  int bufexp, d_time max_time, m_time step, int num_threads)
{
  k_base *kb;
  FILE *fp;
  char file_name[MAX_STRLEN], name[MAX_NAMEBUF], type[MAX_NAMEBUF],
       up[MAX_NAMEBUF], left[MAX_NAMEBUF], right[MAX_NAMEBUF],
       name_v[MAX_NAMEBUF], name_w[MAX_NAMEBUF],
       debug[DEBUG_STRLEN];
  char c;
  d_time k, offset;
  node *vp, *wp;
  node_class nclass;
  int num_nodes[NODE_CLASSES_NUMBER];
  stream *ios;
  stream_class sclass;
  arc e;
  int bufsiz;
  int tid;
  int p, q;

  kb = malloc(sizeof(k_base));
  if(!kb)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  kb->num_threads = num_threads;

  for(tid = 0; tid < num_threads; tid++)
    {
      kb->pv[tid].focus = null_event;
      kb->pv[tid].last_input = null_event;
      kb->pv[tid].curr_tid = tid;
      kb->pv[tid].delta_len = 0;
      kb->pv[tid].dstate = normal;
      kb->pv[tid].passed = FALSE;
      kb->pv[tid].io_busy = FALSE;
      kb->pv[tid].count = 0;
      kb->pv[tid].depth = 0;
      kb->pv[tid].halts = 0;

      pthread_mutex_init(&kb->pv[tid].mutex, NULL);

      kb->done[tid] = FALSE;
      pthread_cond_init(&kb->cond_done[tid], NULL);
    }

  kb->done[num_threads] = FALSE;
  pthread_cond_init(&kb->cond_done[num_threads], NULL);

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

      vp = name2node(kb, name);
      if(vp->nclass >= 0)
        {
          fprintf(stderr, "%s, %s: Duplicate node\n", file_name, name);
          exit(EXIT_FAILURE);
        }

      vp->nclass = nclass;
      vp->k = k;

      vp->pin[parent].e.vp = name2node(kb, up);
      vp->pin[left_son].e.vp = name2node(kb, left);
      vp->pin[right_son].e.vp = name2node(kb, right);

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

  offset = NULL_TIME;
  ios = NULL;
  k = 0;

  while(fscanf(fp, " "OP_FMT" "NAME_FMT" ( "NAME_FMT" , "NAME_FMT" ) @ "
                   TIME_FMT" ",
               &c, name, name_v, name_w, &k) >= 4)
    {
      if(strlen(name) > MAX_NAMELEN)
        {
          fprintf(stderr, "%s, %s: Name too long\n",
                  file_name, name);
          exit(EXIT_FAILURE);
        }

      vp = name2node(kb, name_v);
      wp = name2node(kb, name_w);

      if(!vp || !wp)
        {
          fprintf(stderr, "%s, %s: Undefined node\n",
                  file_name, !vp? name_v : name_w);
          exit(EXIT_FAILURE);
        }

      e = arc_between(vp, wp);
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

      if(offset != NULL_TIME)
        kb->offset = offset;

      if(!quiet)
        {
          ios = open_stream(name, sclass, e, kb->offset, file_io);

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

          vp = name2node(kb, name);

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
  kb->trace_focus = echo_stdout || echo_debug || logfile_name;
  kb->echo_stdout = echo_stdout || echo_debug;
  kb->echo_debug = echo_debug;

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

  kb->io_count[input_stream] = kb->io_num[input_stream];
  kb->io_count[output_stream] = kb->io_num[output_stream];

  p = max(1, kb->io_num[input_stream]);
  q = max(1, kb->io_num[output_stream]);

  kb->io_slice = p * q / gcd(p, q);
  kb->io_togo = kb->io_slice;

  kb->io_curr_tid = 0;

  kb->barrier_count = 0;

  kb->curr_time = kb->offset;
  kb->max_time = kb->offset + max_time;
  kb->anchor_time = kb->offset;
  kb->min_sigma = kb->offset;
  kb->last_empty = NULL_TIME;
  kb->last_far = NULL_TIME;

  kb->io_err = FALSE;
  kb->exiting = FALSE;

  kb->fails = 0;
  kb->errors = 0;

  kb->step = step;

  kb->perf.count = 0;
  kb->perf.depth = 0;
  kb->perf.halts = 0;

  pthread_mutex_init(&kb->mutex_barrier, NULL);

  return kb;
}

void close_base(k_base *kb)
{
  node *vp, *wp;
  stream *ios, *next_ios;
  stream_class sclass;
  int tid;

  for(tid = 0; tid < kb->num_threads; tid++)
    {
      pthread_mutex_destroy(&kb->pv[tid].mutex);

      pthread_cond_destroy(&kb->cond_done[tid]);
    }

  pthread_cond_destroy(&kb->cond_done[kb->num_threads]);

  pthread_mutex_destroy(&kb->mutex_barrier);

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

              close_stream(ios);

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
  int tid;

  strcpy(file_name, state_name);
  strcat(file_name, EVENT_LIST_EXT);
  fp = fopen(file_name, "r");
  if(!fp)
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  tid = 0;
  while(fscanf(fp, " ( "NAME_FMT" , "NAME_FMT" ) @ "TIME_FMT" : %*[^\n]\n",
               name_v, name_w, &s.t) >= 3)
    {
      vp = name2node(kb, name_v);
      wp = name2node(kb, name_w);

      if(!vp || !wp)
        {
          fprintf(stderr, "%s, %s: Undefined node\n",
                  file_name, !vp? name_v : name_w);
          exit(EXIT_FAILURE);
        }

      s.e = arc_between(vp, wp);
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

      safe_state(kb, s, tid);

      tid = (tid + 1) % kb->num_threads;
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

void loops(thread_arg *tp)
{
  k_base *kb;
  int tid, tid_2;

  kb = tp->kb;
  tid = tp->tid;

  while(!irq && !loop(kb, tid));

  lock_barrier(kb);

  kb->barrier_count++;

  for(tid_2 = 0; tid_2 < kb->num_threads + 1; tid_2++)
    if(kb->done[tid_2])
      {
        kb->barrier_count--;

        kb->done[tid_2] = FALSE;

        signal_deadline(kb, tid_2);
      }

  unlock_barrier(kb);

  myexit(EXIT_SUCCESS);
}

void loops_io(thread_arg *tp)
{
  k_base *kb;
  int tid, tid_2;

  kb = tp->kb;
  tid = tp->tid;

  while(!irq && !loop_io(kb, tid));

  lock_barrier(kb);

  kb->barrier_count++;

  for(tid_2 = 0; tid_2 < kb->num_threads; tid_2++)
    if(kb->done[tid_2])
      {
        kb->barrier_count--;

        kb->done[tid_2] = FALSE;

        signal_deadline(kb, tid_2);
      }

  unlock_barrier(kb);

  myexit(EXIT_SUCCESS);
}

info run(char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool echo_debug, bool file_io, bool quiet, bool hard, bool sturdy,
         int bufexp, d_time max_time, m_time step, int num_threads)
{
  k_base *kb;
  info perf;
  pthread_t thread[MAX_THREADS + 1];
  pthread_attr_t attributes;
  struct sched_param spar;
  thread_arg targ[MAX_THREADS + 1];
  char buffer[MAX_STRLEN], buffer2[MAX_STRLEN];
  cpu_set_t cpuset;
  int tid;
  int i;

  kb = open_base(base_name, logfile_name, xref_name,
                 strictly_causal, soundness_check, echo_stdout, echo_debug, file_io, quiet, sturdy, bufexp, max_time, step, num_threads);
  if(state_name)
    init_state(kb, state_name);

  pthread_attr_init(&attributes);
  if(hard)
    {
      spar.sched_priority = sched_get_priority_max(SCHED_RR);

      pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
      pthread_attr_setschedpolicy(&attributes, SCHED_RR);
      pthread_attr_setschedparam(&attributes, &spar);
    }

  signal(SIGINT, (void (*)())&trap);

  kb->time_base = get_time();

  *buffer = '\0';
  for(tid = 0; tid < num_threads; tid++)
    {
      targ[tid].tid = tid;
      targ[tid].kb = kb;

      if(pthread_create(&thread[tid], &attributes, (void *)&loops, (void *)&targ[tid]))
        {
          perror("POSIX thread error");
          exit(EXIT_FAILURE);
        }

      CPU_ZERO(&cpuset);
      CPU_SET(tid, &cpuset);

      pthread_setaffinity_np(thread[tid], sizeof(cpu_set_t), &cpuset);

      if(echo_debug)
        {
          pthread_getaffinity_np(thread[tid], sizeof(cpu_set_t), &cpuset);

          sprintf(buffer2, "#%03d CPU", tid);
          strcat(buffer, buffer2);

          for(i = 0; i < num_threads + 1; i++)
            if(CPU_ISSET(i, &cpuset))
              {
                sprintf(buffer2, " %d", i);
                strcat(buffer, buffer2);
              }

          strcat(buffer, ", ");
        }
    }

  targ[num_threads].tid = num_threads;
  targ[num_threads].kb = kb;

  if(pthread_create(&thread[num_threads], &attributes, (void *)&loops_io, (void *)&targ[num_threads]))
    {
      perror("POSIX thread error");
      exit(EXIT_FAILURE);
    }

  CPU_ZERO(&cpuset);
  CPU_SET(num_threads, &cpuset);

  pthread_setaffinity_np(thread[num_threads], sizeof(cpu_set_t), &cpuset);

  if(echo_debug)
    {
      pthread_getaffinity_np(thread[num_threads], sizeof(cpu_set_t), &cpuset);

      sprintf(buffer2, "#%03d I/O", num_threads);
      strcat(buffer, buffer2);

      for(i = 0; i < num_threads + 1; i++)
        if(CPU_ISSET(i, &cpuset))
          {
            sprintf(buffer2, " %d", i);
            strcat(buffer, buffer2);
          }

      printf("%s        \n\n", buffer);
    }

  for(tid = 0; tid < num_threads + 1; tid++)
    pthread_join(thread[tid], NULL);

  kb->perf.ticks = get_time() - kb->time_base;

  signal(SIGINT, SIG_DFL);

  for(tid = 0; tid < num_threads; tid++)
    {
      kb->perf.count += kb->pv[tid].count;
      kb->perf.depth += kb->pv[tid].depth;
      kb->perf.halts += kb->pv[tid].halts;
    }

  kb->perf.horizon = kb->curr_time;

  perf = kb->perf;

  close_base(kb);

  return perf;
}

int main(int argc, char **argv)
{
  char *base_name, *state_name, *logfile_name, *xref_name, *option, *ext;
  char default_state_name[MAX_STRLEN], default_logfile_name[MAX_STRLEN], default_xref_name[MAX_STRLEN];
  bool strictly_causal, soundness_check, echo_stdout, echo_debug, file_io, quiet, hard, sturdy;
  int i;
  info perf;
  d_time max_time;
  m_time step, default_step;
  int bufexp;
  int num_threads;

  base_name = state_name = logfile_name = xref_name = NULL;
  strictly_causal = soundness_check = echo_stdout = echo_debug = file_io = quiet = hard = sturdy = FALSE;
  bufexp = DEFAULT_BUFEXP;
  max_time = 0;
  default_step = -1;
  num_threads = DEFAULT_THREADS;

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-cdDfilqsvxy] [-I state] [-L log] [-X symbols] [-n processes] [-r core] [-t step] [-z horizon] [base]\n",
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
                  num_threads = atoi(argv[i]);
                  if(num_threads < 1 || num_threads > MAX_THREADS)
                    {
                      fprintf(stderr, "%d: Argument out of range\n", num_threads);
                      exit(EXIT_FAILURE);
                    }
                } 
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

                    case 'D':
                      echo_debug = TRUE;
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
      strictly_causal, soundness_check, echo_stdout, echo_debug, file_io, quiet, hard, sturdy, bufexp, max_time, step, num_threads);

  printf("\n%s\n%lu logical inferences (%.3f %% of %d x "TIME_FMT") in %.3f seconds, %.3f KLIPS, %lu depth (avg %.3f), %lu halts (%.3f %%)\n",
	irq? "Execution interrupted" : "End of execution",
	perf.count,
	perf.horizon && perf.edges? 100.0 * perf.count / (perf.horizon * perf.edges) : 0,
	perf.edges,
	perf.horizon,
	perf.ticks,
	perf.ticks? perf.count / (1000 * perf.ticks) : 0,
	perf.depth,
	perf.count? (float)perf.depth / perf.count : 0,
	perf.halts,
        perf.count? 100.0 * perf.halts / perf.count : 0);

  return EXIT_SUCCESS;
}


