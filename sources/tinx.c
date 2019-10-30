/*
  TINX - Temporal Inference Network eXecutor
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2019
*/

#define NDEBUG
#define INLINE static inline
#define ANSI_FILE_IO
/* #define UNIX_FILE_IO */

#include "tinx.h"

#define VER "7.5.1 (single core)"

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
  e.lc = no_link;

  if(lc < 0)
    {
      for(lc = parent; lc < LINK_CODES_NUMBER; lc++)
        if(vp == wp->pin[lc].e.vp)
          {
            e.lc = lc;
            break;
          }
    }
  else
    if(vp == wp->pin[lc].e.vp)
      e.lc = lc;

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
      fprintf(stderr, "(%s, %s) # %d @ "TIME_FMT": Soundness violation\n",
              arc_neg(s.e).vp->name, s.e.vp->name, s.e.lc, s.t);

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

          safe_state(kb, r);
        }
      else
        if(s.e.vp->pin[lc2].history[index_of(kb, s)].chosen == phase_of(kb, s))
          {
            r.e = s.e.vp->pin[lc1].e;
            r.t = s.t;

            safe_state(kb, r);
          }

    break;

    case joint:

      r.t = s.t;

      if(s.e.lc == parent)
        {
          r.e = s.e.vp->pin[left_son].e;

          safe_state(kb, r);

          r.e = s.e.vp->pin[right_son].e;

          safe_state(kb, r);
        }
      else
        {
          r.e = s.e.vp->pin[parent].e;

          safe_state(kb, r);
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

      if(!kb->strictly_causal || r.t >= s.t)
        safe_state(kb, r);

      break;

    default:
      assert(FALSE);
    }
}

INLINE void scan_ios(k_base *kb, stream_class sclass)
{
  stream *ios;

  assert(kb);

  ios = kb->io_stream[sclass];

  if(ios)
    {
      if(ios->io_perform(kb, ios))
        {
          remove_stream(&kb->io_stream[sclass]);
          add_stream(&kb->io_stream_2[sclass], ios);
        }
      else
        if(ios->open)
          kb->io_stream[sclass] = ios->next_ios;
        else
          {
            remove_stream(&kb->io_stream[sclass]);
            close_stream(ios, kb->alpha);

            kb->io_num[sclass]--;
            kb->io_open--;

            if(!kb->io_open)
              kb->quiet = TRUE;
          }
    }
}

INLINE bool loop(k_base *kb)
{
  event s;
  m_time delta_time;

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

  if(!valid(kb->focus) || kb->curr_time - kb->focus.t < kb->bsd4)
    {
      scan_ios(kb, input_stream);
      scan_ios(kb, output_stream);

      if(!kb->io_stream[input_stream] && !kb->io_stream[output_stream])
        {
          if(kb->quiet && !valid(kb->focus))
            kb->exiting = TRUE;
          else
            {
              delta_time = get_time() - kb->time_base - (kb->curr_time - kb->offset + 1) * kb->step;

              if(delta_time >= 0)
                {
                  kb->io_stream[input_stream] = kb->io_stream_2[input_stream];
                  kb->io_stream_2[input_stream] = NULL;

                  kb->io_stream[output_stream] = kb->io_stream_2[output_stream];
                  kb->io_stream_2[output_stream] = NULL;

                  kb->curr_time++;
                  kb->far = FALSE;

                  if(kb->max_time && kb->curr_time >= kb->max_time)
                    kb->exiting = TRUE;
                }
              else
                if(!kb->busywait && kb->far)
                  usleep(1000000 * (- delta_time));
            }
        }
    }

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
      if(ios->defaultval == false_symbol)
        s.e = ios->ne;
      else
        if(ios->defaultval == true_symbol)
          s.e = ios->e;
        else
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

  safe_state(kb, s);

  return TRUE;
}

bool input_packed_f(k_base *kb, stream *ios)
{
  event s;
  packet *pack;
  int h, k;

  if(!ios->packed_ios)
    pack = &ios->pack;
  else
    pack = &ios->packed_ios->pack;

  h = ios->pack.packedbit / (8 * sizeof(char));
  k = ios->pack.packedbit - h * (8 * sizeof(char));

  s.t = kb->curr_time;

  if(pack->packedcount == pack->packedtot)
    {
      if(mget_file(ios->fp, pack->packedchar, ceil(pack->packedtot / (8.0 * sizeof(char)))) && file_error(ios->fp))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }

      if(feof(ios->fp))
        {
          reset_file(ios->fp);
          return FALSE;
        }
    }

  if(!(pack->packedchar[h] & (1 << k)))
    s.e = ios->ne;
  else
    s.e = ios->e;

  (pack->packedcount)--;

  if(!pack->packedcount)
    pack->packedcount = pack->packedtot;

  safe_state(kb, s);

  return TRUE;
}

bool input_m_posix(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;

  if(read_message_posix(ios->chan, &c))
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
      if(ios->defaultval == false_symbol)
        s.e = ios->ne;
      else
        if(ios->defaultval == true_symbol)
          s.e = ios->e;
        else
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

  safe_state(kb, s);

  return TRUE;
}

bool input_packed_m_posix(k_base *kb, stream *ios)
{
  event s;
  packet *pack;
  int h, k;

  if(!ios->packed_ios)
    pack = &ios->pack;
  else
    pack = &ios->packed_ios->pack;

  h = ios->pack.packedbit / (8 * sizeof(char));
  k = ios->pack.packedbit - h * (8 * sizeof(char));

  s.t = kb->curr_time;

  if(pack->packedcount == pack->packedtot)
    {
      if(mread_message_posix(ios->chan, pack->packedchar, ceil(pack->packedtot / (8.0 * sizeof(char)))))
        {
          if(errno != EAGAIN)
            {
              perror(ios->chan_name);
              exit(EXIT_FAILURE);
            }

          ios->errors++;

          return FALSE;
        }
      else
        ios->errors = 0;
    }

  if(!(pack->packedchar[h] & (1 << k)))
    s.e = ios->ne;
  else
    s.e = ios->e;

  (pack->packedcount)--;

  if(!pack->packedcount)
    pack->packedcount = pack->packedtot;

  safe_state(kb, s);

  return TRUE;
}

bool input_m_sys5(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;

  if(read_message_sys5(ios->chan5, &c))
    {
      if(errno != ENOMSG)
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
      if(ios->defaultval == false_symbol)
        s.e = ios->ne;
      else
        if(ios->defaultval == true_symbol)
          s.e = ios->e;
        else
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

  safe_state(kb, s);

  return TRUE;
}

bool input_packed_m_sys5(k_base *kb, stream *ios)
{
  event s;
  packet *pack;
  int h, k;

  if(!ios->packed_ios)
    pack = &ios->pack;
  else
    pack = &ios->packed_ios->pack;

  h = ios->pack.packedbit / (8 * sizeof(char));
  k = ios->pack.packedbit - h * (8 * sizeof(char));

  s.t = kb->curr_time;

  if(pack->packedcount == pack->packedtot)
    {
      if(mread_message_sys5(ios->chan5, pack->packedchar, ceil(pack->packedtot / (8.0 * sizeof(char)))))
        {
          if(errno != ENOMSG)
            {
              perror(ios->chan_name);
              exit(EXIT_FAILURE);
            }

          ios->errors++;

          return FALSE;
        }
      else
        ios->errors = 0;
    }

  if(!(pack->packedchar[h] & (1 << k)))
    s.e = ios->ne;
  else
    s.e = ios->e;

  (pack->packedcount)--;

  if(!pack->packedcount)
    pack->packedcount = pack->packedtot;

  safe_state(kb, s);

  return TRUE;
}

bool output_f(k_base *kb, stream *ios)
{
  event s;
  char c;

  if(!ios->e.vp)
    c = kb->alpha[ios->defaultval];
  else
    {
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
              if(kb->io_stream[input_stream] || !kb->far)
                return FALSE;
              else
                {
                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  c = kb->alpha[ios->defaultval];
                }
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

bool output_packed_f(k_base *kb, stream *ios)
{
  event s;
  char c;
  packet *pack;
  int h, k;

  if(!ios->packed_ios)
    pack = &ios->pack;
  else
    pack = &ios->packed_ios->pack;

  h = ios->pack.packedbit / (8 * sizeof(char));
  k = ios->pack.packedbit - h * (8 * sizeof(char));

  c = pack->packedchar[h];

  if(!ios->e.vp)
    {
      if(ios->defaultval == false_symbol)
        c &= ~ (1 << k);
       else
         if(ios->defaultval == true_symbol)
           c |= (1 << k);
    }
  else
   {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          ios->fails = 0;
          c &= ~ (1 << k);
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              ios->fails = 0;
              c |= (1 << k);
            }
          else
            {
              if(kb->io_stream[input_stream] || !kb->far)
                return FALSE;
              else
                {
                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  if(ios->defaultval == false_symbol)
                    c &= ~ (1 << k);
                  else
                    if(ios->defaultval == true_symbol)
                      c |= (1 << k);
                }
            }
        }
    }

  pack->packedchar[h] = c;
  (pack->packedcount)--;

  if(!pack->packedcount)
    {
      if(mput_file(ios->fp, pack->packedchar, ceil(pack->packedtot / (8.0 * sizeof(char)))))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }

      if(sync_file(ios->fp))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }

      pack->packedcount = pack->packedtot;
    }

  return TRUE;
}

bool output_m_posix(k_base *kb, stream *ios)
{
  event s;
  char c;

  if(!ios->e.vp)
    c = kb->alpha[ios->defaultval];
  else
    {
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
              if(kb->io_stream[input_stream] || !kb->far)
                return FALSE;
              else
                {
                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  c = kb->alpha[ios->defaultval];
                }
            }
        }
    }

  if(send_message_posix(ios->chan, &c))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          exit(EXIT_FAILURE);
        }

      ios->errors++;

      if(!kb->sturdy && ios->errors > IO_ERR_LIMIT)
        {
          ios->open = FALSE;

          fprintf(stderr, "%s: IPC error bound exceeded\n", ios->chan_name);
        }

      return FALSE;
    }
  else
    ios->errors = 0;

  return TRUE;
}

bool output_packed_m_posix(k_base *kb, stream *ios)
{
  event s;
  char c;
  packet *pack;
  int h, k;

  if(!ios->packed_ios)
    pack = &ios->pack;
  else
    pack = &ios->packed_ios->pack;

  h = ios->pack.packedbit / (8 * sizeof(char));
  k = ios->pack.packedbit - h * (8 * sizeof(char));

  c = pack->packedchar[h];

  if(!ios->e.vp)
    {
      if(ios->defaultval == false_symbol)
        c &= ~ (1 << k);
       else
         if(ios->defaultval == true_symbol)
           c |= (1 << k);
    }
  else
   {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          ios->fails = 0;
          c &= ~ (1 << k);
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              ios->fails = 0;
              c |= (1 << k);
            }
          else
            {
              if(kb->io_stream[input_stream] || !kb->far)
                return FALSE;
              else
                {
                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  if(ios->defaultval == false_symbol)
                    c &= ~ (1 << k);
                  else
                    if(ios->defaultval == true_symbol)
                      c |= (1 << k);
                }
            }
        }
    }

  pack->packedchar[h] = c;
  (pack->packedcount)--;

  if(!pack->packedcount)
    {
      if(msend_message_posix(ios->chan, pack->packedchar, ceil(pack->packedtot / (8.0 * sizeof(char)))))
        {
          if(errno != EAGAIN)
            {
              perror(ios->chan_name);
              exit(EXIT_FAILURE);
            }

          ios->errors++;

          if(!kb->sturdy && ios->errors > IO_ERR_LIMIT)
            {
              ios->open = FALSE;

              fprintf(stderr, "%s: IPC error bound exceeded\n", ios->chan_name);
            }

          (pack->packedcount)++;

          return FALSE;
        }
      else
        ios->errors = 0;

      pack->packedcount = pack->packedtot;
    }

  return TRUE;
}

bool output_m_sys5(k_base *kb, stream *ios)
{
  event s;
  char c;

  if(!ios->e.vp)
    c = kb->alpha[ios->defaultval];
  else
    {
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
              if(kb->io_stream[input_stream] || !kb->far)
                return FALSE;
              else
                {
                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  c = kb->alpha[ios->defaultval];
                }
            }
        }
    }

  if(send_message_sys5(ios->chan5, &c))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          exit(EXIT_FAILURE);
        }

      ios->errors++;

      if(!kb->sturdy && ios->errors > IO_ERR_LIMIT)
        {
          ios->open = FALSE;

          fprintf(stderr, "%s: IPC error bound exceeded\n", ios->chan_name);
        }

      return FALSE;
    }
  else
    ios->errors = 0;

  return TRUE;
}

bool output_packed_m_sys5(k_base *kb, stream *ios)
{
  event s;
  char c;
  packet *pack;
  int h, k;

  if(!ios->packed_ios)
    pack = &ios->pack;
  else
    pack = &ios->packed_ios->pack;

  h = ios->pack.packedbit / (8 * sizeof(char));
  k = ios->pack.packedbit - h * (8 * sizeof(char));

  c = pack->packedchar[h];

  if(!ios->e.vp)
    {
      if(ios->defaultval == false_symbol)
        c &= ~ (1 << k);
       else
         if(ios->defaultval == true_symbol)
           c |= (1 << k);
    }
   else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          ios->fails = 0;
          c &= ~ (1 << k);
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              ios->fails = 0;
              c |= (1 << k);
            }
          else
            {
              if(kb->io_stream[input_stream] || !kb->far)
                return FALSE;
              else
                {
                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  if(ios->defaultval == false_symbol)
                    c &= ~ (1 << k);
                  else
                    if(ios->defaultval == true_symbol)
                      c |= (1 << k);
                }
            }
        }
    }

  pack->packedchar[h] = c;
  (pack->packedcount)--;

  if(!pack->packedcount)
    {
      if(msend_message_sys5(ios->chan5, pack->packedchar, ceil(pack->packedtot / (8.0 * sizeof(char)))))
        {
          if(errno != EAGAIN)
            {
              perror(ios->chan_name);
              exit(EXIT_FAILURE);
            }

          ios->errors++;

          if(!kb->sturdy && ios->errors > IO_ERR_LIMIT)
            {
              ios->open = FALSE;

              fprintf(stderr, "%s: IPC error bound exceeded\n", ios->chan_name);
            }

          (pack->packedcount)++;

          return FALSE;
        }
      else
        ios->errors = 0;

      pack->packedcount = pack->packedtot;
    }

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
        sprintf(debug, ": %s%s --> %s%s", e1.lc? "" : "~ ", e1.vp->debug, e2.lc? "~ " : "", e2.vp->debug);

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

stream *open_stream(char *name, stream_class sclass, arc e, d_time offset, bool file_io, bool sys5, char *prefix, char *path, io_symbol defaultval, int packed, int packedbit, stream *packed_ios)
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

  if(e.vp)
    {
      pin = &link_of(e);

      ios->e = e;
      ios->ne = pin->e;
    }
  else
    {
      ios->e = e;
      ios->ne = e;
    }

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
        {
          if(!packed)
            {
              ios->io_perform = &input_f;

              ios->fp = open_input_file(ios->file_name);
            }
          else
            {
              ios->io_perform = &input_packed_f;

              if(!packed_ios)
                ios->fp = open_input_file(ios->file_name);
              else
                ios->fp = packed_ios->fp;
            }
        }
      else
        {
          if(!packed)
            {
              ios->io_perform = &output_f;

              clean_file(ios->file_name);
              ios->fp = open_output_file(ios->file_name);
            }
          else
            {
              ios->io_perform = &output_packed_f;

              if(!packed_ios)
                {
                  clean_file(ios->file_name);
                  ios->fp = open_output_file(ios->file_name);
                }
              else
                ios->fp = packed_ios->fp;
            }
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

      if(!sys5)
        {
          if(sclass == input_stream)
            {
              if(!packed)
                {
                  ios->io_perform = &input_m_posix;

                  ios->chan = add_queue_posix(ios->chan_name, sclass);
                }
              else
                {
                  ios->io_perform = &input_packed_m_posix;

                  if(!packed_ios)
                    ios->chan = add_queue_posix(ios->chan_name, sclass);
                  else
                    ios->chan = packed_ios->chan;
                }
            }
          else
            {
              if(!packed)
                {
                  ios->io_perform = &output_m_posix;

                  ios->chan = add_queue_posix(ios->chan_name, sclass);
                }
              else
                {
                  ios->io_perform = &output_packed_m_posix;

                  if(!packed_ios)
                    ios->chan = add_queue_posix(ios->chan_name, sclass);
                  else
                    ios->chan = packed_ios->chan;
                }
            }

          if(failed_queue_posix(ios->chan))
            {
              perror(ios->chan_name);
              exit(EXIT_FAILURE);
            }
        }
      else
        {
          if(sclass == input_stream)
            {
              if(!packed)
                {
                  ios->io_perform = &input_m_sys5;

                  ios->chan5 = add_queue_sys5(ios->chan_name, sclass);
                }
              else
                {
                  ios->io_perform = &input_packed_m_sys5;

                  if(!packed_ios)
                    ios->chan5 = add_queue_sys5(ios->chan_name, sclass);
                  else
                    ios->chan5 = packed_ios->chan5;
                }
            }
          else
            {
              if(!packed)
                {
                  ios->io_perform = &output_m_sys5;

                  ios->chan5 = add_queue_sys5(ios->chan_name, sclass);
                }
              else
                {
                  ios->io_perform = &output_packed_m_sys5;

                  if(!packed_ios)
                    ios->chan5 = add_queue_sys5(ios->chan_name, sclass);
                  else
                    ios->chan5 = packed_ios->chan5;
                }
            }

          if(failed_queue_sys5(ios->chan5))
            {
              perror(ios->chan_name);
              exit(EXIT_FAILURE);
            }
        }
    }

  ios->defaultval = defaultval;
  ios->file_io = file_io;
  ios->sys5 = sys5;
  ios->pack.packed = packed;
  memset(ios->pack.packedchar, 0, sizeof(char) * IOS_BUFFER_SIZE);
  ios->pack.packedcount = 0;
  ios->pack.packedtot = 0;
  ios->pack.packedbit = packedbit;
  ios->packed_ios = packed_ios;
  ios->fails = 0;
  ios->errors = 0;
  ios->open = TRUE;

  return ios;
}

void close_stream(stream *ios, char *alpha)
{
  if(ios->file_io)
    {
      if(!ios->pack.packed)
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
        if(!ios->packed_ios)
          {
            ios->pack.packedcount = 0;
            ios->pack.packedtot--;

            if(close_file(ios->fp))
              {
                perror(ios->file_name);
                exit(EXIT_FAILURE);
              }
          }
        else
          ios->packed_ios->pack.packedtot--;
    }
  else
    if(!ios->sys5)
      {
        if(!ios->pack.packed)
          {
            if(ios->sclass == output_stream)
              send_message_posix(ios->chan, &alpha[end_symbol]);

            if(commit_queue_posix(ios->chan))
              {
                perror(ios->chan_name);
                exit(EXIT_FAILURE);
              }

            if(remove_queue_posix(ios->chan_name))
              {
                perror(ios->chan_name);
                exit(EXIT_FAILURE);
              }
          }
        else
          if(!ios->packed_ios)
            {
              ios->pack.packedcount = 0;
              ios->pack.packedtot--;

              if(commit_queue_posix(ios->chan))
                {
                  perror(ios->chan_name);
                  exit(EXIT_FAILURE);
                }

              if(remove_queue_posix(ios->chan_name))
                {
                  perror(ios->chan_name);
                  exit(EXIT_FAILURE);
                }
            }
          else
            ios->packed_ios->pack.packedtot--;
      }
    else
      {
        if(!ios->pack.packed && ios->sclass == output_stream)
          send_message_sys5(ios->chan5, &alpha[end_symbol]);
        else
          if(!ios->packed_ios)
            {
              ios->pack.packedcount = 0;
              ios->pack.packedtot--;
            }
          else
            ios->packed_ios->pack.packedtot--;
      }

  if(ios->packed_ios && !ios->packed_ios->pack.packedtot)
    free(ios->packed_ios);

  if(!ios->pack.packed || ios->packed_ios || !ios->pack.packedtot)
    free(ios);
}

INLINE void add_stream(stream **handle, stream *ios)
{
  ios->next_ios = (*handle);

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

   (*handle) = ios;
}

INLINE void remove_stream(stream **handle)
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

node *alloc_node(char *name)
{
  node *vp;

  vp = malloc(sizeof(node));
  if(!vp)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  strcpy(vp->name, name);
  vp->nclass = null_op;
  vp->debug = NULL;

  return vp;
}

void init_node(node *vp, node_class nclass, d_time k, int bs)
{
  link_code lc, max_lc;
  d_time t;

  vp->nclass = nclass;
  vp->k = k;

  for(lc = 0; lc < LINK_CODES_NUMBER; lc++)
    {
      vp->pin[lc].e.vp = NULL;
      vp->pin[lc].e.lc = no_link;
    }

  max_lc = (nclass == delay)? (LINK_CODES_NUMBER - 1) : LINK_CODES_NUMBER;

  for(lc = 0; lc < max_lc; lc++)
    {
      vp->pin[lc].history = malloc(sizeof(record) * bs);
      if(!vp->pin[lc].history)
        {
          perror(NULL);
          exit(EXIT_FAILURE);
        }

      for(t = 0; t < bs; t++)
        {
          vp->pin[lc].history[t].stated = NULL_PHASE;
          vp->pin[lc].history[t].chosen = NULL_PHASE;

          vp->pin[lc].history[t].other = null_event;
          vp->pin[lc].history[t].next = null_event;
        }
    }
}

void free_node(node *vp)
{
  link_code lc;

  for(lc = 0; lc < LINK_CODES_NUMBER && vp->pin[lc].e.vp; lc++)
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
          vp = alloc_node(name);
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

k_base *open_base(char *base_name, char *logfile_name, char *xref_name, bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool sys5, bool sturdy,
                  bool busywait, int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *alpha)
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
  stream *ios;
  stream *packed_ios[MAX_IOS];
  stream_class sclass;
  io_type stype;
  int packed, packedbit;
  io_symbol defaultval;
  arc e;
  int bufsiz;
  link_code lc;
  int i, n;

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
  memset(kb->io_stream_2, 0, sizeof(stream *) * STREAM_CLASSES_NUMBER);

  strcpy(file_name, base_name);
  strcat(file_name, NETWORK_EXT);
  fp = fopen(file_name, "r");
  if(!fp)
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  for(nclass = 0; nclass < NODE_CLASSES_NUMBER; nclass++)
    kb->perf.num_nodes[nclass] = 0;

  *left = *right = '\0';
  while(fscanf(fp, " "NAME_FMT" : "NAME_FMT" ; "NAME_FMT" , "
                   NAME_FMT" , "NAME_FMT" ",
               name, type, up, left, right) >= 3)
    {
      if(strlen(name) >= MAX_NAMELEN)
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

      init_node(vp, nclass, k, bufsiz);

      vp->pin[parent].e.vp = name2node(kb, up, TRUE);
      vp->pin[left_son].e.vp = name2node(kb, left, TRUE);
      vp->pin[right_son].e.vp = name2node(kb, right, TRUE);

      kb->perf.num_nodes[nclass]++;

      *left = *right = '\0';
    }

  if(ferror(fp))
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  kb->perf.nodes = kb->perf.num_nodes[gate] + kb->perf.num_nodes[joint] + kb->perf.num_nodes[delay];
  kb->perf.edges = (3 * (kb->perf.num_nodes[gate] + kb->perf.num_nodes[joint]) + 2 * kb->perf.num_nodes[delay]) / 2;

  thread_network(kb->network);

  kb->offset = 0;
  kb->io_num[input_stream] = 0;
  kb->io_num[output_stream] = 0;
  kb->io_open = 0;

  strcpy(kb->alpha, alpha);

  offset = NULL_TIME;
  ios = NULL;
  for(i = 0; i < MAX_IOS; i++)
    packed_ios[i] = NULL;

  lc = no_link;
  stype = io_any;
  packed = 0;
  packedbit = 0;
  defaultval = unknown_symbol;
  k = 0;

  while(fscanf(fp, " "OP_FMT" "FUN_FMT" ( "NAME_FMT" , "NAME_FMT" ) # %d / %u , %u , %u , %u @ "TIME_FMT" ",
               &c, name, name_v, name_w, &lc, &stype, &packed, &packedbit, &defaultval, &k) >= 4)
    {
      if(strlen(name) >= MAX_NAMELEN)
        {
          fprintf(stderr, "%s, %s: Name too long\n",
                  file_name, name);
          exit(EXIT_FAILURE);
        }

      if(packed >= MAX_IOS)
        {
          fprintf(stderr, "%s, %d: Too many packed signals\n",
                  file_name, packed);
          exit(EXIT_FAILURE);
        }

      if(packedbit >= (8 * sizeof(char) * IOS_BUFFER_SIZE))
        {
          fprintf(stderr, "%s, %d: too many characters for I/O buffer size\n",
                  file_name, packedbit);
          exit(EXIT_FAILURE);
        }

      if(!strcmp(name_v, "*") && !strcmp(name_w, "*"))
        {
          e.vp = NULL;
          e.lc = no_link;
        }
      else
        {
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
              fprintf(stderr, "%s, (%s, %s) # %d: Undefined edge\n",
                      file_name, name_v, name_w, lc);
              exit(EXIT_FAILURE);
            }
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

      if(offset != NULL_TIME)
        kb->offset = offset;

      if(!quiet)
        {
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

          ios = open_stream(name, sclass, e, kb->offset, (file_io && stype == io_any) || stype == io_file, sys5, prefix, path, defaultval, packed, packedbit, packed_ios[packed]);

          kb->io_num[sclass]++;
          kb->io_open++;

          add_stream(&kb->io_stream[sclass], ios);

          if(packed)
            {
              if(!packed_ios[packed])
                packed_ios[packed] = ios;

              packed_ios[packed]->pack.packedtot++;
              packed_ios[packed]->pack.packedcount = packed_ios[packed]->pack.packedtot;
            }
        }

      lc = no_link;
      stype = io_any;
      packed = 0;
      packedbit = 0;
      defaultval = unknown_symbol;
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
          if(strlen(name) >= MAX_NAMELEN)
            {
              fprintf(stderr, "%s, %s: Name too long\n", file_name, name);
              exit(EXIT_FAILURE);
            }

          if(strlen(debug) >= DEBUG_STRLEN)
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
  kb->trace_focus = echo_stdout || logfile_name;
  kb->echo_stdout = echo_stdout;
  kb->sys5 = sys5;
  kb->sturdy = sturdy;
  kb->busywait = busywait;

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

  n = max(kb->io_num[input_stream], kb->io_num[output_stream]);
  if(!n)
    n = 1;

  kb->max_slice = kb->perf.edges / (IO_INFERENCE_RATIO * n);
  if(!kb->max_slice)
    kb->max_slice = 1;

  kb->slice = kb->max_slice;

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
  bool file_io;

  if(kb->logfp && fclose(kb->logfp))
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  if(!kb->quiet)
    {
      file_io = TRUE;
      for(sclass = 0; sclass < STREAM_CLASSES_NUMBER; sclass++)
        {
          ios = kb->io_stream[sclass];
          if(ios)
            do
              {
                next_ios = ios->next_ios;

                file_io &= ios->file_io;
                close_stream(ios, kb->alpha);

                ios = next_ios;
              }
            while(ios != kb->io_stream[sclass]);

          ios = kb->io_stream_2[sclass];
          if(ios)
            do
              {
                next_ios = ios->next_ios;

                file_io &= ios->file_io;
                close_stream(ios, kb->alpha);

                ios = next_ios;
              }
            while(ios != kb->io_stream_2[sclass]);
        }

      if(!file_io && kb->sys5)
        delete_queues_sys5();
    }

  vp = kb->network;
  while(vp)
    {
      wp = vp->vp;
      free_node(vp);
      vp = wp;
    }

  free(kb);
}

int init_state(k_base *kb, char *state_name)
{
  FILE *fp;
  char file_name[MAX_STRLEN], name_v[MAX_NAMEBUF], name_w[MAX_NAMEBUF];
  node *vp, *wp;
  event s;
  link_code lc;
  int n;

  strcpy(file_name, state_name);
  strcat(file_name, EVENT_LIST_EXT);
  fp = fopen(file_name, "r");
  if(!fp)
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  n = 0;
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
          fprintf(stderr, "%s, (%s, %s) # %d: Undefined edge\n",
                  file_name, name_v, name_w, lc);
          exit(EXIT_FAILURE);
        }

      if(s.t <= - kb->bsd4 || s.t >= kb->bsd4)
        {
          fprintf(stderr, "%s, "TIME_FMT": Time out of range\n",
                  file_name, s.t);
          exit(EXIT_FAILURE);
        }

      safe_state(kb, s);

      n++;
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

  return n;
}

/******** Toplevel ********/

void trap()
{
  irq = TRUE;
}

info run(char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sys5, bool sturdy, bool busywait,
         int bufexp, d_time max_time, m_time step, m_time origin, char *prefix, char *path, char *alpha)
{
  k_base *kb;
  info perf;
  struct sched_param spar;
  int n;

  kb = open_base(base_name, logfile_name, xref_name,
                 strictly_causal, soundness_check, echo_stdout, file_io, quiet, sys5, sturdy, busywait, bufexp, max_time, step, prefix, path, alpha);

  printf("Network ok -- %d edges, %d nodes (%d gates + %d joints + %d delays), %d inputs, %d outputs\n",
         kb->perf.edges, kb->perf.nodes, kb->perf.num_nodes[gate], kb->perf.num_nodes[joint], kb->perf.num_nodes[delay], kb->io_num[input_stream], kb->io_num[output_stream]);

  if(state_name)
    {
      n = init_state(kb, state_name);
      printf("State ok -- %d initial conditions\n", n);
    }

  printf("Execution running...\n");

  fflush(stdout);

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
  bool strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sys5, sturdy, busywait;
  int i, k, n;
  info perf;
  d_time max_time;
  m_time step, default_step, origin;
  int bufexp;

  base_name = state_name = logfile_name = xref_name = NULL;
  strictly_causal = soundness_check = echo_stdout = file_io = quiet = hard = sys5 = sturdy = busywait = FALSE;
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
              fprintf(stderr, "Usage: %s [-cdfilqsSvVxy] [-a alphabet] [-e prefix] [-g origin] [-I state] [-L log] [-p path] [-r core] [-t step] [-X symbols] [-z horizon] [base]\n",
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

                    case 'S':
                      busywait = TRUE;
                    break;

                    case 'v':
                      soundness_check = TRUE;
                    break;

                    case 'V':
                      sys5 = TRUE;
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
         "Design & coding by Andrea Giotti, 1998-1999, 2016-2019\n\n");

  fflush(stdout);

  perf = run(base_name, state_name, logfile_name, xref_name,
      strictly_causal, soundness_check, echo_stdout, file_io, quiet, hard, sys5, sturdy, busywait, bufexp, max_time, step, origin, prefix, path, alpha);

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


