/*
  TINX - Temporal Inference Network eXecutor
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2024
*/

#define NDEBUG
#define INLINE static inline
#define ANSI_FILE_IO
/* #define UNIX_FILE_IO */

#include "tinx_dt.h"

#define VER "11.0.9 DT (dual core)"

const arc null_arc = {NULL, no_link};
const event null_event = {{NULL, no_link}, NULL_TIME};

const char class_symbol[NODE_CLASSES_NUMBER + 1] = CLASS_SYMBOLS;

volatile sig_atomic_t rst = FALSE;
volatile sig_atomic_t irq = FALSE;
volatile sig_atomic_t frz = FALSE;

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

INLINE void state(k_base *kb, event s, bool soundness_check)
{
  event q, r;

  assert(kb);
  assert(valid(s));
  assert(!is_stated(kb, s));

  if(soundness_check && is_stated(kb, ev_neg(s)))
    {
      if(s.e.vp->debug)
        fprintf(stderr, "(%s, %s) # %d @ "TIME_FMT": Soundness violation%s%s\n",
                arc_neg(s.e).vp->name, s.e.vp->name, s.e.lc, s.t, s.e.lc? ": ~ " : ": ", s.e.vp->debug);
      else
        fprintf(stderr, "(%s, %s) # %d @ "TIME_FMT": Soundness violation\n",
                arc_neg(s.e).vp->name, s.e.vp->name, s.e.lc, s.t);
      
      irq = TRUE;
      return;
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

  if(s.t == kb->safe_time)
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
  d_time dt, mt;

  assert(kb);

  s = kb->focus;

  if(valid(s))
    {
      dt = s.t - kb->anchor_time;

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
            {
              kb->focus = next(kb, s);

              mt = min(kb->focus.t, kb->safe_time);

              if(kb->anchor_time < mt)
                kb->anchor_time = mt;
            }

          if(kb->last_input.t <= s.t)
            kb->last_input = null_event;

#if !defined NDEBUG
          printf("(%s, %s) @ "TIME_FMT" ==>\n", arc_neg(s.e).vp->name, s.e.vp->name, s.t);
#endif
          kb->bound = kb->strictly_causal || (dt < - kb->bsd4);
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

INLINE void math_safe_state(k_base *kb, event s, real value)
{
  if(is_math_stated(kb, s))
    {
      if(kb->soundness_check && value_of(kb, s) != value)
        {
          if(s.e.lc >= 0 && s.e.vp->debug)
            fprintf(stderr, "(%s, %s) # %d @ "TIME_FMT" = "REAL_OUT_FMT" ~= "REAL_OUT_FMT" (diff = %e): Math soundness violation, same path%s%s\n",
                    arc_neg(s.e).vp->name, s.e.vp->name, s.e.lc, s.t, value, value_of(kb, s), value - value_of(kb, s), s.e.lc? ": ~ " : ": ", s.e.vp->debug);
          else
            fprintf(stderr, "(%s, %s) # %d @ "TIME_FMT" = "REAL_OUT_FMT" ~= "REAL_OUT_FMT" (diff = %e): Math soundness violation, same path\n",
                    arc_neg(s.e).vp->name, s.e.vp->name, s.e.lc, s.t, value, value_of(kb, s), value - value_of(kb, s));

          irq = TRUE;
          return;
        }
    }
  else
    {
      value_of(kb, s) = value;

      state(kb, s, FALSE);
    }
}

INLINE void math_opt_state(k_base *kb, event s, real value)
{
  event r;

  r = ev_neg(s);

  if(is_math_stated(kb, r))
    {
      if(kb->soundness_check && value_of(kb, r) != value)
        {
          if(s.e.vp->debug)
            fprintf(stderr, "(%s, %s) # %d @ "TIME_FMT" = "REAL_OUT_FMT" ~= "REAL_OUT_FMT" (diff = %e): Math soundness violation, different path%s%s\n",
                    arc_neg(s.e).vp->name, s.e.vp->name, s.e.lc, s.t, value, value_of(kb, r), value - value_of(kb, r), s.e.lc? ": ~ " : ": ", s.e.vp->debug);
          else
            fprintf(stderr, "(%s, %s) # %d @ "TIME_FMT" = "REAL_OUT_FMT" ~= "REAL_OUT_FMT" (diff = %e): Math soundness violation, different path\n",
                    arc_neg(s.e).vp->name, s.e.vp->name, s.e.lc, s.t, value, value_of(kb, r), value - value_of(kb, r));

          irq = TRUE;
          return;
        }
    }
  else
    math_safe_state(kb, s, value);
}

INLINE void update_literals(k_base *kb, event s)
{
  event r;

  if(s.e.vp->ls)
    {
      r.e.vp = (node *)s.e.vp->ls;
      r.e.lc = no_link;
      r.t = s.t;

      math_safe_state(kb, r, value_of(kb, s));
    }
}

INLINE void share_literals(k_base *kb, event s)
{
  event q, r;
  sh_literal *ls;
  real val;

  ls = (sh_literal *)s.e.vp;
  val = value_of(kb, s);

  r.e = ls->e;
  r.t = s.t;

  math_opt_state(kb, r, val);

  q.e.vp = (node *)ls->other;
  q.e.lc = no_link;
  q.t = s.t;

  math_safe_state(kb, q, val);
}

INLINE void process(k_base *kb, event s)
{
  event r;
  linkage *pin1, *pin2;
  phase phi;
  d_time idx;
  real value_x, value_y, value_z;

  assert(kb);

  if(s.e.lc < 0)
    {
      share_literals(kb, s);
      return;
    }

  assert(valid(s));

  kb->perf.count++;
  switch(s.e.vp->nclass)
    {
    case gate:

      pin1 = &s.e.vp->pin[(s.e.lc + 1) % LINK_CODES_NUMBER];
      pin2 = &s.e.vp->pin[(s.e.lc + 2) % LINK_CODES_NUMBER];

      phi = phase_of(kb, s);
      idx = index_of(kb, s);

      if(pin1->history[idx].chosen == phi)
        {
          r.e = pin2->e;
          r.t = s.t;

          safe_state(kb, r);
        }
      else
        if(pin2->history[idx].chosen == phi)
          {
            r.e = pin1->e;
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

      if(!kb->bound || r.t >= s.t)
        safe_state(kb, r);

    break;

    case math_chs:

      r.t = s.t;

      if(s.e.lc == parent)
        r.e = s.e.vp->pin[left_son].e;
      else
        r.e = s.e.vp->pin[parent].e;

      math_opt_state(kb, r, - value_of(kb, s));

    break;

    case math_inv:

      r.t = s.t;

      if(s.e.lc == parent)
        r.e = s.e.vp->pin[left_son].e;
      else
        r.e = s.e.vp->pin[parent].e;

      value_z = 1 / value_of(kb, s);

      math_opt_state(kb, r, value_z);

    break;

    case math_sin:

      r.t = s.t;

      if(s.e.lc == parent)
        {
          r.e = s.e.vp->pin[left_son].e;

          value_z = asin(value_of(kb, s));
        }
      else
        {
          r.e = s.e.vp->pin[parent].e;

          value_z = sin(value_of(kb, s));
        }

      math_opt_state(kb, r, value_z);

    break;

    case math_add:

      phi = phase_of(kb, s);
      idx = index_of(kb, s);

      value_x = s.e.vp->pin[s.e.lc].history[idx].value;

      if(s.e.lc == parent)
        {
          pin1 = &s.e.vp->pin[left_son];
          pin2 = &s.e.vp->pin[right_son];
          
          if(pin1->history[idx].chosen == phi || pin1->history[idx].chosen == CONSTANT_PHASE)
            {
              r.e = pin2->e;
              r.t = s.t;

              value_y = pin1->history[idx].value;
              value_z = value_x - value_y;

              math_opt_state(kb, r, value_z);
            }
          else
            if(pin2->history[idx].chosen == phi || pin2->history[idx].chosen == CONSTANT_PHASE)
              {
                r.e = pin1->e;
                r.t = s.t;

                value_y = pin2->history[idx].value;
                value_z = value_x - value_y;

                math_opt_state(kb, r, value_z);
              }
        }
      else
        {
          pin1 = &s.e.vp->pin[s.e.lc == left_son? right_son : left_son];
          pin2 = &s.e.vp->pin[parent];

          if(pin1->history[idx].chosen == phi || pin1->history[idx].chosen == CONSTANT_PHASE)
            {
              r.e = pin2->e;
              r.t = s.t;

              value_y = pin1->history[idx].value;
              value_z = value_x + value_y;

              math_opt_state(kb, r, value_z);
            }
          else
            if(pin2->history[idx].chosen == phi || pin2->history[idx].chosen == CONSTANT_PHASE)
              {
                r.e = pin1->e;
                r.t = s.t;

                value_y = pin2->history[idx].value;
                value_z = value_y - value_x;

                math_opt_state(kb, r, value_z);
             }
        }

    break;

    case math_mul:

      phi = phase_of(kb, s);
      idx = index_of(kb, s);

      value_x = s.e.vp->pin[s.e.lc].history[idx].value;

      if(s.e.lc == parent)
        {
          pin1 = &s.e.vp->pin[left_son];
          pin2 = &s.e.vp->pin[right_son];

          if(pin1->history[idx].chosen == phi || pin1->history[idx].chosen == CONSTANT_PHASE)
            {
              r.e = pin2->e;
              r.t = s.t;

              value_y = pin1->history[idx].value;
              value_z = value_x / value_y;

              math_opt_state(kb, r, value_z);
            }
          else
            if(pin2->history[idx].chosen == phi || pin2->history[idx].chosen == CONSTANT_PHASE)
              {
                r.e = pin1->e;
                r.t = s.t;

                value_y = pin2->history[idx].value;
                value_z = value_x / value_y;

                math_opt_state(kb, r, value_z);
             }
        }
      else
        {
          pin1 = &s.e.vp->pin[s.e.lc == left_son? right_son : left_son];
          pin2 = &s.e.vp->pin[parent];

          if(pin1->history[idx].chosen == phi || pin1->history[idx].chosen == CONSTANT_PHASE)
            {
              r.e = pin2->e;
              r.t = s.t;

              value_y = pin1->history[idx].value;
              value_z = value_x * value_y;

              math_opt_state(kb, r, value_z);
            }
          else
            if(pin2->history[idx].chosen == phi || pin2->history[idx].chosen == CONSTANT_PHASE)
              {
                r.e = pin1->e;
                r.t = s.t;

                value_y = pin2->history[idx].value;
                value_z = value_y / value_x;

                math_opt_state(kb, r, value_z);
             }
        }

    break;

    case math_pow:

      phi = phase_of(kb, s);
      idx = index_of(kb, s);

      value_x = s.e.vp->pin[s.e.lc].history[idx].value;

      if(s.e.lc == parent)
        {
          pin1 = &s.e.vp->pin[left_son];
          pin2 = &s.e.vp->pin[right_son];

          if(pin1->history[idx].chosen == phi || pin1->history[idx].chosen == CONSTANT_PHASE)
            {
              r.e = pin2->e;
              r.t = s.t;

              value_y = pin1->history[idx].value;
              value_z = log(value_x) / log(value_y);

              math_opt_state(kb, r, value_z);
            }
          else
            if(pin2->history[idx].chosen == phi || pin2->history[idx].chosen == CONSTANT_PHASE)
              {
                r.e = pin1->e;
                r.t = s.t;

                value_y = pin2->history[idx].value;
                value_z = pow(value_x, 1 / value_y);

                math_opt_state(kb, r, value_z);
             }
        }
      else
        {
          pin1 = &s.e.vp->pin[s.e.lc == left_son? right_son : left_son];
          pin2 = &s.e.vp->pin[parent];

          if(pin1->history[idx].chosen == phi || pin1->history[idx].chosen == CONSTANT_PHASE)
            {
              r.e = pin2->e;
              r.t = s.t;

              value_y = pin1->history[idx].value;

              if(s.e.lc == left_son)
                value_z = pow(value_x, value_y);
              else
                value_z = pow(value_y, value_x);

              math_opt_state(kb, r, value_z);
            }
          else
            if(pin2->history[idx].chosen == phi || pin2->history[idx].chosen == CONSTANT_PHASE)
              {
                r.e = pin1->e;
                r.t = s.t;

                value_y = pin2->history[idx].value;

                if(s.e.lc == left_son)
                  value_z = log(value_y) / log(value_x);
                else
                  value_z = pow(value_y, 1 / value_x);

                math_opt_state(kb, r, value_z);
             }
        }

    break;

    case math_eqv0:

      if(s.e.lc == parent)
        {
          r.e = s.e.vp->pin[left_son].e;
          r.t = s.t;
          
          math_opt_state(kb, r, 0);
        }
      else
        if(value_of(kb, s))
          {
            r.e = s.e.vp->pin[parent].e;
            r.t = s.t;

            safe_state(kb, r);
          }

    break;

    case math_neq0:

      if(s.e.lc != parent && !value_of(kb, s))
          {
            r.e = s.e.vp->pin[parent].e;
            r.t = s.t;

            safe_state(kb, r);
          }

    break;

    case math_gteq0:

      if(s.e.lc != parent && value_of(kb, s) < 0)
          {
            r.e = s.e.vp->pin[parent].e;
            r.t = s.t;

            safe_state(kb, r);
          }

    break;

    case math_lt0:

      if(s.e.lc != parent && value_of(kb, s) >= 0)
          {
            r.e = s.e.vp->pin[parent].e;
            r.t = s.t;

            safe_state(kb, r);
          }

    break;

    case math_delay:
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

      if(!kb->bound || r.t >= s.t)
        math_opt_state(kb, r, value_of(kb, s));
    break;

    case literal:

      update_literals(kb, s);

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

INLINE void update_stream_cache(k_base *kb, stream *ios, event s, real val)
{
  ios->s = s;
  ios->val = val;
  
  ios->skip_ios = NULL;
  *kb->prod_cache = ios;

  kb->prod_cache = &ios->skip_ios;  
}

INLINE void scan_ios_cache(k_base *kb)
{
  stream *ios;

  assert(kb);

  ios = *kb->cons_cache;

  if(ios)
    {
      if(ios->val != REAL_MAX)
        {
          math_safe_state(kb, ios->s, ios->val);
          math_safe_state(kb, ev_neg(ios->s), ios->val);
        }
      else
        safe_state(kb, ios->s);

      kb->cons_cache = &ios->skip_ios;
      kb->in_count--;
      
      if(!kb->in_count)
        {
          kb->in_count = kb->io_num[input_stream];
          
          kb->skip_ios = NULL;
          kb->cons_cache = &kb->skip_ios;

          kb->all_taken = kb->safe_time;
        }
    }
}

INLINE void join_barrier(k_base *kb)
{
  lock_barrier(kb);

  if(kb->awake)
    kb->awake = FALSE;
  else
    {
      kb->done = TRUE;
      do
        wait_deadline(kb);
      while(kb->done);
    }

  unlock_barrier(kb);
}

INLINE void leave_barrier(k_base *kb)
{
  lock_barrier(kb);

  if(kb->done)
    {
      kb->done = FALSE;
      signal_deadline(kb);
    }
  else
    kb->awake = TRUE;

  unlock_barrier(kb);
}

INLINE bool loop(k_base *kb)
{
  event s;
  d_time mt;

  assert(kb);

  if(frz)
    {
      usleep(1000);
      kb->time_base = get_time() - (kb->curr_time - kb->offset) * kb->step;
    }
  else
    {
      kb->safe_time = kb->curr_time;

      if(kb->slice)
        {
          kb->slice--;

          s = choose(kb);

          if(kb->trace_focus)
            trace(kb, s);

          if(valid(s))
            {
              process(kb, s);
              return kb->exiting;
            }
        }
      else
        kb->slice = kb->max_slice;

      if(kb->io_num[input_stream])
        scan_ios_cache(kb);
      else
        kb->all_taken = kb->safe_time;

      mt = min(kb->focus.t, kb->safe_time);

      if(kb->anchor_time < mt)
        {
          kb->anchor_time = mt;
          kb->far = FALSE;
        }

      if(kb->all_taken == kb->safe_time)
        {
          if(kb->quiet && !valid(kb->focus))
            kb->exiting = TRUE;
          else
            {
              if(kb->far || (kb->strictly_causal && kb->safe_time < kb->focus.t))
                kb->abandon = kb->safe_time;

              if(!kb->busywait && kb->far)
                join_barrier(kb);
            }
        }
    }

  return kb->exiting;
}

INLINE bool loop_io(k_base *kb)
{
  m_time delta_time;

  assert(kb);

  if(frz)
    {
      usleep(1000);
      kb->time_base = get_time() - (kb->curr_time - kb->offset) * kb->step;
    }
  else
    {
      if(kb->curr_time - kb->anchor_time < kb->bsd4)
        {
          scan_ios(kb, input_stream);
          scan_ios(kb, output_stream);

          if(kb->all_taken == kb->curr_time && !kb->io_stream[output_stream])
            {
              delta_time = get_time() - kb->time_base - (kb->curr_time - kb->offset + 1) * kb->step;

              if(delta_time >= 0)
                {
                  kb->io_stream[input_stream] = kb->io_stream_2[input_stream];
                  kb->io_stream_2[input_stream] = NULL;

                  kb->io_stream[output_stream] = kb->io_stream_2[output_stream];
                  kb->io_stream_2[output_stream] = NULL;

                  kb->prod_cache = &kb->skip_ios;

                  kb->curr_time++;

                  if(kb->max_time && kb->curr_time >= kb->max_time)
                    kb->exiting = TRUE;

                  if(!kb->busywait)
                    leave_barrier(kb);
                }
              else
                if(!kb->busywait)
                  {
                    usleep(1000000 * (- delta_time));
                    leave_barrier(kb);
                  }
            }
        }
    }

  if(irq)
    kb->exiting = TRUE;

  return kb->exiting;
}

/******** Input/Output ********/

bool input_f(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;

  c = EOF;
  if(get_file(ios->fp, &c) && file_error(ios->fp))
    {
      perror(ios->file_name);
      irq = TRUE;
      return FALSE;
    }

  switch(strchr(kb->alpha, c) - kb->alpha)
    {
    case eof_symbol:
      reset_file(ios->fp);

      if(!ios->skip[ios->defaultval] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
        return FALSE;

      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
          s.e = ios->e;
        else
          return TRUE;
    break;

    case unknown_symbol:
      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
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
      irq = TRUE;
      return FALSE;
    }

  update_stream_cache(kb, ios, s, REAL_MAX);

  return TRUE;
}

bool input_num_f(k_base *kb, stream *ios)
{
  event s;
  real val;
  char blanks[MAX_STRLEN];

  s.t = kb->curr_time;

  ios->buffer[ios->pos] = '\0';
  if(fscanf(ios->fp, FUN_FMT, &ios->buffer[ios->pos]) == EOF)
    {
      if(file_error(ios->fp))
        {
          perror(ios->file_name);
          irq = TRUE;
          return FALSE;
        }
      else
        reset_file(ios->fp);
    }

  blanks[0] = '\0';
  if(fscanf(ios->fp, "%"MAX_NAMEBUF_C"["BLANKS"]", blanks) == EOF)
    {
      if(file_error(ios->fp))
        {
          perror(ios->file_name);
          irq = TRUE;
          return FALSE;
        }
      else
        reset_file(ios->fp);
    }

  if(blanks[0] == '\0')
    ios->pos = strlen(ios->buffer);
  else
    ios->pos = 0;

  if(ios->buffer[0] == kb->alpha[end_symbol] && ios->buffer[1] == '\0')
    {
      ios->open = FALSE;
      return FALSE;
    }

  if(ios->buffer[0] == '\0' || ios->pos)
    {
      if(!ios->skip[io_other] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
        return FALSE;
      else
        val = ios->defaultreal;
    }
  else
    if(sscanf(ios->buffer, REAL_IN_FMT, &val) != 1)
      {
        fprintf(stderr, "%s, %s: Invalid number in stream\n", ios->file_name, ios->buffer);
        irq = TRUE;
        return FALSE;
      }

  s.e = ios->e;

  update_stream_cache(kb, ios, s, val);

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

  h = ios->pack.packedbit >> 3;
  k = ios->pack.packedbit & 7;

  s.t = kb->curr_time;

  if(pack->packedcount == pack->packedtot)
    {
      if(mget_file(ios->fp, pack->packedchar, (pack->packedtot & 7)? (pack->packedtot >> 3) + 1 : pack->packedtot >> 3) && file_error(ios->fp))
        {
          perror(ios->file_name);
          irq = TRUE;
          return FALSE;
        }

      if(feof(ios->fp))
        {
          reset_file(ios->fp);

          if(!ios->skip[ios->defaultval] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
            return FALSE;

          pack->gen = FALSE;
        }
      else
        pack->gen = TRUE;
    }

  (pack->packedcount)--;

  if(!pack->packedcount)
    pack->packedcount = pack->packedtot;

  if(pack->gen)
    {
      if(!(pack->packedchar[h] & (1 << k)))
        s.e = ios->ne;
      else
        s.e = ios->e;
    }
  else
    {
      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
          s.e = ios->e;
        else
          return TRUE;
    }

  update_stream_cache(kb, ios, s, REAL_MAX);

  return TRUE;
}

bool input_s(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;

  c = EOF;
  if(get_socket(ios->sock, &c))
    {
      if(errno != EAGAIN)
        {
          perror(ios->socket_name);
          irq = TRUE;
          return FALSE;
        }

      ios->errors++;
    }
  else
    ios->errors = 0;

  switch(strchr(kb->alpha, c) - kb->alpha)
    {
    case eof_symbol:
      if(!ios->skip[ios->defaultval] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
        return FALSE;

      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
          s.e = ios->e;
        else
          return TRUE;
    break;

    case unknown_symbol:
      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
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
              ios->socket_name, c, c);
      irq = TRUE;
      return FALSE;
    }

  update_stream_cache(kb, ios, s, REAL_MAX);

  return TRUE;
}

bool input_num_s(k_base *kb, stream *ios)
{
  event s;
  real val;
  char buffer[MAX_STRLEN];

  s.t = kb->curr_time;

  *buffer = EOF;
  if(mget_socket(ios->sock, buffer, MSG_SIZE))
    {
      if(errno != EAGAIN)
        {
          perror(ios->socket_name);
          irq = TRUE;
          return FALSE;
        }

      ios->errors++;
    }
  else
    ios->errors = 0;

  if(*buffer == EOF)
    {
      if(!ios->skip[io_other] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
        return FALSE;
      else
        val = ios->defaultreal;
    }
  else
    {
      if(buffer[0] == kb->alpha[end_symbol] && buffer[1] == '\0')
        {
          ios->open = FALSE;
          return FALSE;
        }

      if(sscanf(buffer, REAL_IN_FMT, &val) != 1)
        {
          fprintf(stderr, "%s, %s: Invalid number in stream\n", ios->socket_name, buffer);
          irq = TRUE;
          return FALSE;
        }
    }

  s.e = ios->e;

  update_stream_cache(kb, ios, s, val);

  return TRUE;
}

bool input_packed_s(k_base *kb, stream *ios)
{
  event s;
  packet *pack;
  int h, k;

  if(!ios->packed_ios)
    pack = &ios->pack;
  else
    pack = &ios->packed_ios->pack;

  h = ios->pack.packedbit >> 3;
  k = ios->pack.packedbit & 7;

  s.t = kb->curr_time;

  if(pack->packedcount == pack->packedtot)
    {
      if(mget_socket(ios->sock, pack->packedchar, (pack->packedtot & 7)? (pack->packedtot >> 3) + 1 : pack->packedtot >> 3))
        {
          if(errno != EAGAIN)
            {
              perror(ios->socket_name);
              irq = TRUE;
              return FALSE;
            }

          ios->errors++;

          if(!ios->skip[ios->defaultval] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
            return FALSE;

          pack->gen = FALSE;
        }
      else
        {
          ios->errors = 0;
          pack->gen = TRUE;
        }
    }

  (pack->packedcount)--;

  if(!pack->packedcount)
    pack->packedcount = pack->packedtot;

  if(pack->gen)
    {
      if(!(pack->packedchar[h] & (1 << k)))
        s.e = ios->ne;
      else
        s.e = ios->e;
    }
  else
    {
      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
          s.e = ios->e;
        else
          return TRUE;
    }

  update_stream_cache(kb, ios, s, REAL_MAX);

  return TRUE;
}

bool input_m_posix(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;

  c = EOF;
  if(read_message_posix(ios->chan, &c))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          irq = TRUE;
          return FALSE;
        }

      ios->errors++;
    }
  else
    ios->errors = 0;

  switch(strchr(kb->alpha, c) - kb->alpha)
    {
    case eof_symbol:
      if(!ios->skip[ios->defaultval] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
        return FALSE;

      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
          s.e = ios->e;
        else
          return TRUE;
    break;

    case unknown_symbol:
      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
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
      irq = TRUE;
      return FALSE;
    }

  update_stream_cache(kb, ios, s, REAL_MAX);

  return TRUE;
}

bool input_num_m_posix(k_base *kb, stream *ios)
{
  event s;
  real val;
  char buffer[MAX_STRLEN];

  s.t = kb->curr_time;

  *buffer = EOF;
  if(mread_message_posix(ios->chan, buffer, MSG_SIZE))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          irq = TRUE;
          return FALSE;
        }

      ios->errors++;
    }
  else
    ios->errors = 0;

  if(*buffer == EOF)
    {
      if(!ios->skip[io_other] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
        return FALSE;
      else
        val = ios->defaultreal;
    }
  else
    {
      if(buffer[0] == kb->alpha[end_symbol] && buffer[1] == '\0')
        {
          ios->open = FALSE;
          return FALSE;
        }

      if(sscanf(buffer, REAL_IN_FMT, &val) != 1)
        {
          fprintf(stderr, "%s, %s: Invalid number in stream\n", ios->chan_name, buffer);
          irq = TRUE;
          return FALSE;
        }
    }

  s.e = ios->e;

  update_stream_cache(kb, ios, s, val);

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

  h = ios->pack.packedbit >> 3;
  k = ios->pack.packedbit & 7;

  s.t = kb->curr_time;

  if(pack->packedcount == pack->packedtot)
    {
      if(mread_message_posix(ios->chan, pack->packedchar, (pack->packedtot & 7)? (pack->packedtot >> 3) + 1 : pack->packedtot >> 3))
        {
          if(errno != EAGAIN)
            {
              perror(ios->chan_name);
              irq = TRUE;
              return FALSE;
            }

          ios->errors++;

          if(!ios->skip[ios->defaultval] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
            return FALSE;

          pack->gen = FALSE;
        }
      else
        {
          ios->errors = 0;
          pack->gen = TRUE;
        }
    }

  (pack->packedcount)--;

  if(!pack->packedcount)
    pack->packedcount = pack->packedtot;

  if(pack->gen)
    {
      if(!(pack->packedchar[h] & (1 << k)))
        s.e = ios->ne;
      else
        s.e = ios->e;
    }
  else
    {
      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
          s.e = ios->e;
        else
          return TRUE;
    }

  update_stream_cache(kb, ios, s, REAL_MAX);

  return TRUE;
}

bool input_m_sys5(k_base *kb, stream *ios)
{
  event s;
  char c;

  s.t = kb->curr_time;

  c = EOF;
  if(read_message_sys5(ios->chan5, &c))
    {
      if(errno != ENOMSG)
        {
          perror(ios->chan_name);
          irq = TRUE;
          return FALSE;
        }

      ios->errors++;
    }
  else
    ios->errors = 0;

  switch(strchr(kb->alpha, c) - kb->alpha)
    {
    case eof_symbol:
      if(!ios->skip[ios->defaultval] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
        return FALSE;

      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
          s.e = ios->e;
        else
          return TRUE;
    break;

    case unknown_symbol:
      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
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
      irq = TRUE;
      return FALSE;
    }

  update_stream_cache(kb, ios, s, REAL_MAX);

  return TRUE;
}

bool input_num_m_sys5(k_base *kb, stream *ios)
{
  event s;
  real val;
  char buffer[MAX_STRLEN];

  s.t = kb->curr_time;

  *buffer = EOF;
  if(mread_message_sys5(ios->chan5, buffer, MSG_SIZE))
    {
      if(errno != ENOMSG)
        {
          perror(ios->chan_name);
          irq = TRUE;
          return FALSE;
        }

      ios->errors++;
    }
  else
    ios->errors = 0;

  if(*buffer == EOF)
    {
      if(!ios->skip[io_other] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
        return FALSE;
      else
        val = ios->defaultreal;
    }
  else
    {
      if(buffer[0] == kb->alpha[end_symbol] && buffer[1] == '\0')
        {
          ios->open = FALSE;
          return FALSE;
        }

      if(sscanf(buffer, REAL_IN_FMT, &val) != 1)
        {
          fprintf(stderr, "%s, %s: Invalid number in stream\n", ios->chan_name, buffer);
          irq = TRUE;
          return FALSE;
        }
    }

  s.e = ios->e;

  update_stream_cache(kb, ios, s, val);

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

  h = ios->pack.packedbit >> 3;
  k = ios->pack.packedbit & 7;

  s.t = kb->curr_time;

  if(pack->packedcount == pack->packedtot)
    {
      if(mread_message_sys5(ios->chan5, pack->packedchar, (pack->packedtot & 7)? (pack->packedtot >> 3) + 1 : pack->packedtot >> 3))
        {
          if(errno != ENOMSG)
            {
              perror(ios->chan_name);
              irq = TRUE;
              return FALSE;
            }

          ios->errors++;

          if(!ios->skip[ios->defaultval] || get_time() - kb->time_base < (kb->curr_time - kb->offset + 1) * kb->step)
            return FALSE;

          pack->gen = FALSE;
        }
      else
        {
          ios->errors = 0;
          pack->gen = TRUE;
        }
    }

  (pack->packedcount)--;

  if(!pack->packedcount)
    pack->packedcount = pack->packedtot;

  if(pack->gen)
    {
      if(!(pack->packedchar[h] & (1 << k)))
        s.e = ios->ne;
      else
        s.e = ios->e;
    }
  else
    {
      if(ios->defaultval == io_false)
        s.e = ios->ne;
      else
        if(ios->defaultval == io_true)
          s.e = ios->e;
        else
          return TRUE;
    }

  update_stream_cache(kb, ios, s, REAL_MAX);

  return TRUE;
}

bool output_f(k_base *kb, stream *ios)
{
  event s;
  char c;

  if(!ios->e.vp)
    {
      if(ios->skip[ios->defaultval])
        return TRUE;

      c = kb->alpha[ios->defaultval];
    }
  else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          if(ios->skip[io_false])
            return TRUE;

          ios->fails = 0;
          c = kb->alpha[io_false];
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              if(ios->skip[io_true])
                return TRUE;

              ios->fails = 0;
              c = kb->alpha[io_true];
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->skip[io_unknown])
                    return TRUE;

                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  if(ios->skip[ios->defaultval])
                    c = kb->alpha[io_unknown];
                  else
                    c = kb->alpha[ios->defaultval];
                }
            }
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

bool output_num_f(k_base *kb, stream *ios)
{
  event s;
  real val;
  char buffer[MAX_STRLEN];

  if(!ios->e.vp)
    {
      if(ios->skip[io_other])
        return TRUE;

      val = ios->defaultreal;
    }
  else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_math_stated(kb, s))
        {
          val = value_of(kb, s);

          if(ios->skip[io_other] && val == ios->defaultreal)
            return TRUE;

          ios->fails = 0;
        }
      else
        {
          s.e = ios->e;

          if(is_math_stated(kb, s))
            {
              val = value_of(kb, s);

              if(ios->skip[io_other] && val == ios->defaultreal)
                return TRUE;

              ios->fails = 0;
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->skip[io_unknown])
                    return TRUE;

                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  val = ios->defaultreal;
                }
            }
        }
    }

  sprintf(buffer, REAL_OUT_FMT"\n", val);

  if(mput_file(ios->fp, buffer, strlen(buffer)))
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

  h = ios->pack.packedbit >> 3;
  k = ios->pack.packedbit & 7;

  c = pack->packedchar[h];

  if(!ios->e.vp)
    {
      if(ios->defaultval == io_false)
        c &= ~ (1 << k);
       else
         if(ios->defaultval == io_true)
           c |= (1 << k);

      if(!ios->skip[ios->defaultval])
        pack->gen = TRUE;
    }
  else
   {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          c &= ~ (1 << k);

          if(!ios->skip[io_false])
            {
              ios->fails = 0;
              pack->gen = TRUE;
            }
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              c |= (1 << k);

              if(!ios->skip[io_true])
                {
                  ios->fails = 0;
                  pack->gen = TRUE;
                }
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->defaultval == io_false)
                    c &= ~ (1 << k);
                  else
                    if(ios->defaultval == io_true)
                      c |= (1 << k);

                  if(!ios->skip[io_unknown])
                    {
                      ios->fails++;

                      if(!kb->max_time && ios->fails > kb->bsd4)
                        {
                          ios->open = FALSE;
                          return FALSE;
                        }

                      pack->gen = TRUE;
                    }
                }
            }
        }
    }

  pack->packedchar[h] = c;
  (pack->packedcount)--;

  if(!pack->packedcount)
    {
      if(pack->gen)
        {
          if(mput_file(ios->fp, pack->packedchar, (pack->packedtot & 7)? (pack->packedtot >> 3) + 1 : pack->packedtot >> 3))
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

          pack->gen = FALSE;
        }

      pack->packedcount = pack->packedtot;
    }

  return TRUE;
}

bool output_s(k_base *kb, stream *ios)
{
  event s;
  char c;

  if(!ios->e.vp)
    {
      if(ios->skip[ios->defaultval])
        return TRUE;

      c = kb->alpha[ios->defaultval];
    }
  else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          if(ios->skip[io_false])
            return TRUE;

          ios->fails = 0;
          c = kb->alpha[io_false];
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              if(ios->skip[io_true])
                return TRUE;

              ios->fails = 0;
              c = kb->alpha[io_true];
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->skip[io_unknown])
                    return TRUE;

                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  if(ios->skip[ios->defaultval])
                    c = kb->alpha[io_unknown];
                  else
                    c = kb->alpha[ios->defaultval];
                }
            }
        }
    }

  if(put_socket(ios->sock, &c))
    {
      if(errno != EAGAIN)
        {
          perror(ios->socket_name);
          irq = TRUE;
          return FALSE;
        }

      ios->errors++;

      if(!kb->sturdy && ios->errors > IO_ERR_LIMIT)
        {
          ios->open = FALSE;

          fprintf(stderr, "%s: Remote channel error bound exceeded\n", ios->socket_name);
        }

      return FALSE;
    }
  else
    ios->errors = 0;

  if(sync_socket(ios->sock))
    {
      perror(ios->socket_name);
      irq = TRUE;
      return FALSE;
    }

  return TRUE;
}

bool output_num_s(k_base *kb, stream *ios)
{
  event s;
  real val;
  char buffer[MAX_STRLEN];

  if(!ios->e.vp)
    {
      if(ios->skip[io_other])
        return TRUE;

      val = ios->defaultreal;
    }
  else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_math_stated(kb, s))
        {
          val = value_of(kb, s);

          if(ios->skip[io_other] && val == ios->defaultreal)
            return TRUE;

          ios->fails = 0;
        }
      else
        {
          s.e = ios->e;

          if(is_math_stated(kb, s))
            {
              val = value_of(kb, s);

              if(ios->skip[io_other] && val == ios->defaultreal)
                return TRUE;

              ios->fails = 0;
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->skip[io_unknown])
                    return TRUE;

                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  val = ios->defaultreal;
                }
            }
        }
    }

  sprintf(buffer, REAL_OUT_FMT"\n", val);

  if(mput_socket(ios->sock, buffer, strlen(buffer)))
    {
      if(errno != EAGAIN)
        {
          perror(ios->socket_name);
          irq = TRUE;
          return FALSE;
        }

      ios->errors++;

      if(!kb->sturdy && ios->errors > IO_ERR_LIMIT)
        {
          ios->open = FALSE;

          fprintf(stderr, "%s: Remote channel error bound exceeded\n", ios->socket_name);
        }

      return FALSE;
    }
  else
    ios->errors = 0;

  if(sync_socket(ios->sock))
    {
      perror(ios->socket_name);
      irq = TRUE;
      return FALSE;
    }

  return TRUE;
}

bool output_packed_s(k_base *kb, stream *ios)
{
  event s;
  char c;
  packet *pack;
  int h, k;

  if(!ios->packed_ios)
    pack = &ios->pack;
  else
    pack = &ios->packed_ios->pack;

  h = ios->pack.packedbit >> 3;
  k = ios->pack.packedbit & 7;

  c = pack->packedchar[h];

  if(!ios->e.vp)
    {
      if(ios->defaultval == io_false)
        c &= ~ (1 << k);
       else
         if(ios->defaultval == io_true)
           c |= (1 << k);

      if(!ios->skip[ios->defaultval])
        pack->gen = TRUE;
    }
  else
   {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          c &= ~ (1 << k);

          if(!ios->skip[io_false])
            {
              ios->fails = 0;
              pack->gen = TRUE;
            }
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              c |= (1 << k);

              if(!ios->skip[io_true])
                {
                  ios->fails = 0;
                  pack->gen = TRUE;
                }
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->defaultval == io_false)
                    c &= ~ (1 << k);
                  else
                    if(ios->defaultval == io_true)
                      c |= (1 << k);

                  if(!ios->skip[io_unknown])
                    {
                      ios->fails++;

                      if(!kb->max_time && ios->fails > kb->bsd4)
                        {
                          ios->open = FALSE;
                          return FALSE;
                        }

                      pack->gen = TRUE;
                    }
                }
            }
        }
    }

  pack->packedchar[h] = c;
  (pack->packedcount)--;

  if(!pack->packedcount)
    {
      if(pack->gen)
        {
          if(mput_socket(ios->sock, pack->packedchar, (pack->packedtot & 7)? (pack->packedtot >> 3) + 1 : pack->packedtot >> 3))
            {
              if(errno != EAGAIN)
                {
                  perror(ios->socket_name);
                  irq = TRUE;
                  return FALSE;
                }

              ios->errors++;

              if(!kb->sturdy && ios->errors > IO_ERR_LIMIT)
                {
                  ios->open = FALSE;

                  fprintf(stderr, "%s: Remote channel error bound exceeded\n", ios->socket_name);
                }

              (pack->packedcount)++;

              return FALSE;
            }
          else
            ios->errors = 0;

          if(sync_socket(ios->sock))
            {
              perror(ios->socket_name);
              irq = TRUE;
              return FALSE;
            }

          pack->gen = FALSE;
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
    {
      if(ios->skip[ios->defaultval])
        return TRUE;

      c = kb->alpha[ios->defaultval];
    }
  else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          if(ios->skip[io_false])
            return TRUE;

          ios->fails = 0;
          c = kb->alpha[io_false];
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              if(ios->skip[io_true])
                return TRUE;

              ios->fails = 0;
              c = kb->alpha[io_true];
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->skip[io_unknown])
                    return TRUE;

                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  if(ios->skip[ios->defaultval])
                    c = kb->alpha[io_unknown];
                  else
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
          irq = TRUE;
          return FALSE;
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

bool output_num_m_posix(k_base *kb, stream *ios)
{
  event s;
  real val;
  char buffer[MAX_STRLEN];

  if(!ios->e.vp)
    {
      if(ios->skip[io_other])
        return TRUE;

      val = ios->defaultreal;
    }
  else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_math_stated(kb, s))
        {
          val = value_of(kb, s);

          if(ios->skip[io_other] && val == ios->defaultreal)
            return TRUE;

          ios->fails = 0;
        }
      else
        {
          s.e = ios->e;

          if(is_math_stated(kb, s))
            {
              val = value_of(kb, s);

              if(ios->skip[io_other] && val == ios->defaultreal)
                return TRUE;

              ios->fails = 0;
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->skip[io_unknown])
                    return TRUE;

                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  val = ios->defaultreal;
                }
            }
        }
    }

  sprintf(buffer, REAL_OUT_FMT"\n", val);

  if(msend_message_posix(ios->chan, buffer, strlen(buffer)))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          irq = TRUE;
          return FALSE;
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

  h = ios->pack.packedbit >> 3;
  k = ios->pack.packedbit & 7;

  c = pack->packedchar[h];

  if(!ios->e.vp)
    {
      if(ios->defaultval == io_false)
        c &= ~ (1 << k);
       else
         if(ios->defaultval == io_true)
           c |= (1 << k);

      if(!ios->skip[ios->defaultval])
        pack->gen = TRUE;
    }
  else
   {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          c &= ~ (1 << k);

          if(!ios->skip[io_false])
            {
              ios->fails = 0;
              pack->gen = TRUE;
            }
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              c |= (1 << k);

              if(!ios->skip[io_true])
                {
                  ios->fails = 0;
                  pack->gen = TRUE;
                }
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->defaultval == io_false)
                    c &= ~ (1 << k);
                  else
                    if(ios->defaultval == io_true)
                      c |= (1 << k);

                  if(!ios->skip[io_unknown])
                    {
                      ios->fails++;

                      if(!kb->max_time && ios->fails > kb->bsd4)
                        {
                          ios->open = FALSE;
                          return FALSE;
                        }

                      pack->gen = TRUE;
                    }
                }
            }
        }
    }

  pack->packedchar[h] = c;
  (pack->packedcount)--;

  if(!pack->packedcount)
    {
      if(pack->gen)
        {
          if(msend_message_posix(ios->chan, pack->packedchar, (pack->packedtot & 7)? (pack->packedtot >> 3) + 1 : pack->packedtot >> 3))
            {
              if(errno != EAGAIN)
                {
                  perror(ios->chan_name);
                  irq = TRUE;
                  return FALSE;
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

          pack->gen = FALSE;
        }

      pack->packedcount = pack->packedtot;
    }

  return TRUE;
}

bool output_m_sys5(k_base *kb, stream *ios)
{
  event s;
  char c;

  if(!ios->e.vp)
    {
      if(ios->skip[ios->defaultval])
        return TRUE;

      c = kb->alpha[ios->defaultval];
    }
  else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          if(ios->skip[io_false])
            return TRUE;

          ios->fails = 0;
          c = kb->alpha[io_false];
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              if(ios->skip[io_true])
                return TRUE;

              ios->fails = 0;
              c = kb->alpha[io_true];
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->skip[io_unknown])
                    return TRUE;

                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  if(ios->skip[ios->defaultval])
                    c = kb->alpha[io_unknown];
                  else
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
          irq = TRUE;
          return FALSE;
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

bool output_num_m_sys5(k_base *kb, stream *ios)
{
  event s;
  real val;
  char buffer[MAX_STRLEN];

  if(!ios->e.vp)
    {
      if(ios->skip[io_other])
        return TRUE;

      val = ios->defaultreal;
    }
  else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_math_stated(kb, s))
        {
          val = value_of(kb, s);

          if(ios->skip[io_other] && val == ios->defaultreal)
            return TRUE;

          ios->fails = 0;
        }
      else
        {
          s.e = ios->e;

          if(is_math_stated(kb, s))
            {
              val = value_of(kb, s);

              if(ios->skip[io_other] && val == ios->defaultreal)
                return TRUE;

              ios->fails = 0;
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->skip[io_unknown])
                    return TRUE;

                  ios->fails++;

                  if(!kb->max_time && ios->fails > kb->bsd4)
                    {
                      ios->open = FALSE;
                      return FALSE;
                    }

                  val = ios->defaultreal;
                }
            }
        }
    }

  sprintf(buffer, REAL_OUT_FMT"\n", val);

  if(msend_message_sys5(ios->chan5, buffer, strlen(buffer)))
    {
      if(errno != EAGAIN)
        {
          perror(ios->chan_name);
          irq = TRUE;
          return FALSE;
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

  h = ios->pack.packedbit >> 3;
  k = ios->pack.packedbit & 7;

  c = pack->packedchar[h];

  if(!ios->e.vp)
    {
      if(ios->defaultval == io_false)
        c &= ~ (1 << k);
       else
         if(ios->defaultval == io_true)
           c |= (1 << k);

      if(!ios->skip[ios->defaultval])
        pack->gen = TRUE;
    }
   else
    {
      s.t = kb->curr_time;
      s.e = ios->ne;

      if(is_stated(kb, s))
        {
          c &= ~ (1 << k);

          if(!ios->skip[io_false])
            {
              ios->fails = 0;
              pack->gen = TRUE;
            }
        }
      else
        {
          s.e = ios->e;

          if(is_stated(kb, s))
            {
              c |= (1 << k);

              if(!ios->skip[io_true])
                {
                  ios->fails = 0;
                  pack->gen = TRUE;
                }
            }
          else
            {
              if(kb->abandon != kb->curr_time)
                return FALSE;
              else
                {
                  if(ios->defaultval == io_false)
                    c &= ~ (1 << k);
                  else
                    if(ios->defaultval == io_true)
                      c |= (1 << k);

                  if(!ios->skip[io_unknown])
                    {
                      ios->fails++;

                      if(!kb->max_time && ios->fails > kb->bsd4)
                        {
                          ios->open = FALSE;
                          return FALSE;
                        }

                      pack->gen = TRUE;
                    }
                }
            }
        }
    }

  pack->packedchar[h] = c;
  (pack->packedcount)--;

  if(!pack->packedcount)
    {
      if(pack->gen)
        {
          if(msend_message_sys5(ios->chan5, pack->packedchar, (pack->packedtot & 7)? (pack->packedtot >> 3) + 1 : pack->packedtot >> 3))
            {
              if(errno != EAGAIN)
                {
                  perror(ios->chan_name);
                  irq = TRUE;
                  return FALSE;
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

          pack->gen = FALSE;
        }

      pack->packedcount = pack->packedtot;
    }

  return TRUE;
}

void trace(k_base *kb, event s)
{
  char valbuf[MAX_STRLEN], debug[2 * DEBUG_STRLEN + 16];
  arc e1, e2;
  real val;

  if(valid(s))
    {
      if(s.e.lc < 0)
        return;

      kb->io_busy = TRUE;

      e1 = arc_neg(s.e);
      e2 = s.e;

      *valbuf = '\0';
      val = value_of(kb, s);
      if(val != REAL_MAX)
        sprintf(valbuf, " = "REAL_OUT_FMT, val);

      *debug = '\0';
      if(e1.vp->debug && e2.vp->debug)
        sprintf(debug, ": %s%s --> %s%s", e1.lc? "" : "~ ", e1.vp->debug, e2.lc? "~ " : "", e2.vp->debug);

      if(kb->echo_stdout)
        {
          printf(" > "TIME_FMT": (%s, %s) # %d @ "TIME_FMT"%s%s        \r",
                 kb->safe_time, e1.vp->name, e2.vp->name, e2.lc, s.t, valbuf, debug);
          fflush(stdout);
        }

      if(kb->logfp)
        {
          if(fprintf(kb->logfp, "(%s, %s) # %d @ "TIME_FMT"%s%s\n",
                 e1.vp->name, e2.vp->name, e2.lc, s.t, valbuf, debug) < 0)
            {
              perror(NULL);
              irq = TRUE;
              return;
            }

          if(fflush(kb->logfp))
            {
              perror(NULL);
              irq = TRUE;
              return;
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

stream *open_stream(char *name, stream_class sclass, arc e, d_time offset, io_type stype, bool sys5, char *prefix, char *path, char *netpath, io_type_3 defaultval, real defaultreal, io_type_4 omissions,
                    int packed, int packedbit, stream *packed_ios)
{
  stream *ios;
  linkage *pin;
  io_type_3 i;

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

  switch(stype)
    {
      case io_file:

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
          if(packed <= 0)
            {
              if(!packed)
                ios->io_perform = &input_f;
              else
                ios->io_perform = &input_num_f;

              ios->fp = open_input_file(ios->file_name);
            }
          else
            {
              ios->io_perform = &input_packed_f;

              if(!packed_ios)
                ios->fp = open_input_file(ios->file_name);
              else
                {
                  if(packed_ios->stype == io_file)
                    ios->fp = packed_ios->fp;
                  else
                    {
                      fprintf(stderr, "%s: Packed signal method mismatch\n", name);
                      exit(EXIT_FAILURE);
                    }
                }
            }
        }
      else
        {
          if(packed <= 0)
            {
              if(!packed)
                ios->io_perform = &output_f;
              else
                ios->io_perform = &output_num_f;

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
                {
                  if(packed_ios->stype == io_file)
                    ios->fp = packed_ios->fp;
                  else
                    {
                      fprintf(stderr, "%s: Packed signal method mismatch\n", name);
                      exit(EXIT_FAILURE);
                    }
                }
            }
        }

      if(!is_file_open(ios->fp))
        {
          perror(ios->file_name);
          exit(EXIT_FAILURE);
        }
    
      break;

      case io_socket:

      if(*netpath)
        {
          strcpy(ios->socket_name, netpath);
          strcat(ios->socket_name, ".");
        }
      else
        *(ios->socket_name) = '\0';

      strcat(ios->socket_name, name);

      if(sclass == input_stream)
        {
          if(packed <= 0)
            {
              if(!packed)
                ios->io_perform = &input_s;
              else
                ios->io_perform = &input_num_s;

              printf("Opening remote channnel to %s for input\n", ios->socket_name);
              ios->sock = open_input_socket(ios->socket_name);
            }
          else
            {
              ios->io_perform = &input_packed_s;

              if(!packed_ios)
                {
                  printf("Opening remote channnel to %s for input\n", ios->socket_name);
                  ios->sock = open_input_socket(ios->socket_name);
                }
              else
                {
                  if(packed_ios->stype == io_socket)
                    ios->sock = packed_ios->sock;
                  else
                    {
                      fprintf(stderr, "%s: Packed signal method mismatch\n", name);
                      exit(EXIT_FAILURE);
                    }
                }
            }
        }
      else
        {
          if(packed <= 0)
            {
              if(!packed)
                ios->io_perform = &output_s;
              else
                ios->io_perform = &output_num_s;

              printf("Opening remote channnel to %s for output\n", ios->socket_name);
              ios->sock = open_output_socket(ios->socket_name);
            }
          else
            {
              ios->io_perform = &output_packed_s;

              if(!packed_ios)
                {
                  printf("Opening remote channnel to %s for output\n", ios->socket_name);
                  ios->sock = open_output_socket(ios->socket_name);
                }
              else
                {
                  if(packed_ios->stype == io_socket)
                    ios->sock = packed_ios->sock;
                  else
                    {
                      fprintf(stderr, "%s: Packed signal method mismatch\n", name);
                      exit(EXIT_FAILURE);
                    }
                }
            }
        }

      if(!is_socket_open(ios->sock))
        {
          perror(ios->socket_name);
          exit(EXIT_FAILURE);
        }

      break;

      case io_ipc:

      strcpy(ios->chan_name, prefix);
      strcat(ios->chan_name, name);

      if(!sys5)
        {
          if(sclass == input_stream)
            {
              if(packed <= 0)
                {
                  if(!packed)
                    ios->io_perform = &input_m_posix;
                  else
                    ios->io_perform = &input_num_m_posix;

                  ios->chan = add_queue_posix(ios->chan_name, sclass);
                }
              else
                {
                  ios->io_perform = &input_packed_m_posix;

                  if(!packed_ios)
                    ios->chan = add_queue_posix(ios->chan_name, sclass);
                  else
                    {
                      if(packed_ios->stype == io_ipc)
                        ios->chan = packed_ios->chan;
                      else
                        {
                          fprintf(stderr, "%s: Packed signal method mismatch\n", name);
                          exit(EXIT_FAILURE);
                        }
                    }
                }
            }
          else
            {
              if(packed <= 0)
                {
                  if(!packed)
                    ios->io_perform = &output_m_posix;
                  else
                    ios->io_perform = &output_num_m_posix;

                  ios->chan = add_queue_posix(ios->chan_name, sclass);
                }
              else
                {
                  ios->io_perform = &output_packed_m_posix;

                  if(!packed_ios)
                    ios->chan = add_queue_posix(ios->chan_name, sclass);
                  else
                    {
                      if(packed_ios->stype == io_ipc)
                        ios->chan = packed_ios->chan;
                      else
                        {
                          fprintf(stderr, "%s: Packed signal method mismatch\n", name);
                          exit(EXIT_FAILURE);
                        }
                    }
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
              if(packed <= 0)
                {
                  if(!packed)
                    ios->io_perform = &input_m_sys5;
                  else
                    ios->io_perform = &input_num_m_sys5;

                  ios->chan5 = add_queue_sys5(ios->chan_name, sclass);
                }
              else
                {
                  ios->io_perform = &input_packed_m_sys5;

                  if(!packed_ios)
                    ios->chan5 = add_queue_sys5(ios->chan_name, sclass);
                  else
                    {
                      if(packed_ios->stype == io_ipc)
                        ios->chan5 = packed_ios->chan5;
                      else
                        {
                          fprintf(stderr, "%s: Packed signal method mismatch\n", name);
                          exit(EXIT_FAILURE);
                        }
                    }
                }
            }
          else
            {
              if(packed <= 0)
                {
                  if(!packed)
                    ios->io_perform = &output_m_sys5;
                  else
                    ios->io_perform = &output_num_m_sys5;

                  ios->chan5 = add_queue_sys5(ios->chan_name, sclass);
                }
              else
                {
                  ios->io_perform = &output_packed_m_sys5;

                  if(!packed_ios)
                    ios->chan5 = add_queue_sys5(ios->chan_name, sclass);
                  else
                    {
                      if(packed_ios->stype == io_ipc)
                        ios->chan5 = packed_ios->chan5;
                      else
                        {
                          fprintf(stderr, "%s: Packed signal method mismatch\n", name);
                          exit(EXIT_FAILURE);
                        }
                    }
                }
            }

          if(failed_queue_sys5(ios->chan5))
            {
              perror(ios->chan_name);
              exit(EXIT_FAILURE);
            }
        }
          
      break;
      
      default:
      break;
    }

  for(i = 0; i < IO_TYPES_3_NUMBER; i++)
    ios->skip[i] = (omissions >= io_filter && (i == defaultval || i == io_other)) || (omissions == io_omit && i == io_unknown);

  ios->stype = stype;
  ios->sys5 = sys5;
  ios->defaultval = defaultval;
  ios->defaultreal = defaultreal;
  ios->pack.packed = packed;

  memset(ios->pack.packedchar, 0, sizeof(char) * IOS_BUFFER_SIZE);

  ios->pack.packedcount = 0;
  ios->pack.packedtot = 0;
  ios->pack.packedbit = packedbit;
  ios->packed_ios = packed_ios;
  ios->next_ios = NULL;
  ios->prev_ios = NULL;
  ios->fails = 0;
  ios->errors = 0;
  ios->open = TRUE;
  ios->pos = 0;
  ios->s = null_event;
  ios->val = REAL_MAX;
  ios->skip_ios = NULL;

  return ios;
}

bool close_stream(stream *ios, char *alpha)
{
  switch(ios->stype)
    {
      case io_file:

      if(ios->pack.packed <= 0)
        {
          if(ios->sclass == output_stream && put_file(ios->fp, &alpha[end_symbol]))
            {
              perror(ios->file_name);
              irq = TRUE;
              return FALSE;
            }

          if(close_file(ios->fp))
            {
              perror(ios->file_name);
              irq = TRUE;
              return FALSE;
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
                irq = TRUE;
                return FALSE;
              }
          }
        else
          ios->packed_ios->pack.packedtot--;

      break;

      case io_socket:

      if(ios->pack.packed <= 0)
        {
          if(ios->sclass == output_stream && put_socket(ios->sock, &alpha[end_symbol]))
            {
              perror(ios->socket_name);
              irq = TRUE;
              return FALSE;
            }

          if(close_socket(ios->sock))
            {
              perror(ios->socket_name);
              irq = TRUE;
              return FALSE;
            }
        }
      else
        if(!ios->packed_ios)
          {
            ios->pack.packedcount = 0;
            ios->pack.packedtot--;

            if(close_socket(ios->sock))
              {
                perror(ios->socket_name);
                irq = TRUE;
                return FALSE;
              }
          }
        else
          ios->packed_ios->pack.packedtot--;

      break;

      case io_ipc:

      if(!ios->sys5)
        {
          if(ios->pack.packed <= 0)
            {
              if(ios->sclass == output_stream)
                send_message_posix(ios->chan, &alpha[end_symbol]);

              if(commit_queue_posix(ios->chan))
                {
                  perror(ios->chan_name);
                  irq = TRUE;
                  return FALSE;
                }

              if(remove_queue_posix(ios->chan_name))
                {
                  perror(ios->chan_name);
                  irq = TRUE;
                  return FALSE;
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
                    irq = TRUE;
                    return FALSE;
                  }

                if(remove_queue_posix(ios->chan_name))
                  {
                    perror(ios->chan_name);
                    irq = TRUE;
                    return FALSE;
                  }
              }
            else
              ios->packed_ios->pack.packedtot--;
        }
      else
        {
          if(ios->pack.packed <= 0 && ios->sclass == output_stream)
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
        
      break;
      
      default:
      break;
    }

  if(ios->packed_ios && !ios->packed_ios->pack.packedtot)
    free(ios->packed_ios);

  if(ios->pack.packed <= 0 || ios->packed_ios || !ios->pack.packedtot)
    free(ios);

  return TRUE;
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
  vp->ls = NULL;
  vp->debug = NULL;
  vp->vp = NULL;

  return vp;
}

void init_node(node *vp, node_class nclass, real val, int bs)
{
  link_code lc, max_lc;
  d_time t;

  vp->nclass = nclass;

  if(val == REAL_MAX)
    vp->k = 1;
  else
    vp->k = floor(val);

  for(lc = 0; lc < LINK_CODES_NUMBER; lc++)
    {
      vp->pin[lc].e.vp = NULL;
      vp->pin[lc].e.lc = no_link;
    }

  if(nclass == gate || nclass == joint || nclass == math_add || nclass == math_mul || nclass == math_pow)
    max_lc = LINK_CODES_NUMBER;
  else
    if(nclass != literal)
      max_lc = (LINK_CODES_NUMBER - 1);
    else
      max_lc = (LINK_CODES_NUMBER - 2);

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

          if(nclass == literal)
            vp->pin[lc].history[t].value = val;
          else
            vp->pin[lc].history[t].value = REAL_MAX;

          vp->pin[lc].history[t].other = null_event;
          vp->pin[lc].history[t].next = null_event;
        }
    }
}

void init_literal(sh_literal *lp, char *name, int bs)
{
  d_time t;

  strcpy(lp->name, name);
  
  lp->history = malloc(sizeof(record) * bs);
  if(!lp->history)
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  for(t = 0; t < bs; t++)
    {
      lp->history[t].stated = NULL_PHASE;
      lp->history[t].chosen = NULL_PHASE;

      lp->history[t].value = REAL_MAX;

      lp->history[t].other = null_event;
      lp->history[t].next = null_event;
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

void free_literal(sh_literal *lp)
{
  free(lp->history);
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

              if((vp->pin[lc].e.vp != vp || lc != lc1) && vp->pin[lc].e.vp->pin[lc1].e.vp == vp && vp->pin[lc].e.vp->pin[lc1].e.lc < 0)
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
        case math_add:
        case math_mul:
        case math_pow:
          assert(lc <= 3);
          if(lc < 3)
            {
              fprintf(stderr, "%s: Undefined edge in node declarations\n",
                      vp->name);
              exit(EXIT_FAILURE);
            }
        break;

        case delay:
        case math_chs:
        case math_inv:
        case math_sin:
        case math_eqv0:
        case math_neq0:
        case math_gteq0:
        case math_lt0:
        case math_delay:
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

        case literal:
          if(lc < 1)
            {
              fprintf(stderr, "%s: Undefined edge in node declarations\n",
                      vp->name);
              exit(EXIT_FAILURE);
            }
          else
            if(lc > 1)
              {
                fprintf(stderr, "%s: Edge mismatch in node declarations\n",
                        vp->name);
                exit(EXIT_FAILURE);
              }
        break;

        default:
          fprintf(stderr, "%s: Reference to undefined node\n", vp->name);
          exit(EXIT_FAILURE);
        break;
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

k_base *open_base(char *base_name, char *logfile_name, char *xref_name, bool strictly_causal, bool soundness_check, bool echo_stdout, io_type rtype, bool sys5, bool sturdy,
                  bool busywait, int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *netpath, char *alpha)
{
  k_base *kb;
  FILE *fp;
  char file_name[MAX_STRLEN], name[MAX_NAMEBUF], type[MAX_NAMEBUF], tail[MAX_NAMEBUF],
       up[MAX_NAMEBUF], left[MAX_NAMEBUF], right[MAX_NAMEBUF],
       name_v[MAX_NAMEBUF], name_w[MAX_NAMEBUF],
       debug[DEBUG_STRLEN + 1];
  char str[3];
  char c, d;
  real val;
  d_time k, offset;
  node *vp, *wp;
  node_class nclass, lclass;
  stream *ios;
  stream *packed_ios[MAX_IOS];
  stream_class sclass;
  io_type stype;
  int packed, packedbit;
  io_type_3 defaultval;
  io_type_4 omissions;
  real defaultreal;
  arc e;
  int bufsiz;
  link_code lc, lcup, lcleft, lcright;
  int i, j, n, num_ls, max_ls, t;
  sh_literal *ls;

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

  kb->skip_ios = NULL;
  kb->prod_cache = &kb->skip_ios;
  kb->cons_cache = &kb->skip_ios;

  strcpy(file_name, base_name);
  strcat(file_name, NETWORK_EXT);
  fp = fopen(file_name, "r");
  if(!fp)
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  kb->perf.nodes = 0;
  kb->perf.edges = 0;

  for(lclass = 0; lclass < NODE_LARGE_CLASSES; lclass++)
    kb->perf.num_nodes[lclass] = 0;

  *left = *right = '\0';
  lcup = lcleft = lcright = no_link;

  while(fscanf(fp, " "NAME_FMT" : "FUN_FMT" ; "NAME_FMT" # %d , "
                   NAME_FMT" # %d , "NAME_FMT" # %d \n",
               name, type, up, &lcup, left, &lcleft, right, &lcright) >= 4)
    {
      if(strlen(name) >= MAX_NAMELEN)
        {
          fprintf(stderr, "%s, %s: Name too long\n",
                  file_name, name);
          exit(EXIT_FAILURE);
        }

      val = REAL_MAX;
      *tail = '\0';
      sscanf(type, OP_FMT""REAL_IN_FMT"%s", &c, &val, tail);
      if(*tail != '\0')
        val = REAL_MAX;

      if(val == REAL_MAX)
        k = 1;
      else
        k = floor(val);

      nclass = strchr(class_symbol, toupper(c)) - class_symbol;
      if(nclass < 0)
        {
          fprintf(stderr, "%s, %s, "OP_FMT": Invalid node class\n",
                  file_name, name, c);
          exit(EXIT_FAILURE);
        }

      if((nclass == delay || nclass == math_delay) && (k <= - kb->bsd4 || k >= kb->bsd4))
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

      init_node(vp, nclass, val, bufsiz);

      vp->pin[parent].e.vp = name2node(kb, up, TRUE);
      vp->pin[left_son].e.vp = name2node(kb, left, TRUE);
      vp->pin[right_son].e.vp = name2node(kb, right, TRUE);

      vp->pin[parent].e.lc = lcup;

      if(nclass != literal)
        vp->pin[left_son].e.lc = lcleft;

      if(nclass == gate || nclass == joint || nclass == math_add || nclass == math_mul || nclass == math_pow)
        vp->pin[right_son].e.lc = lcright;

      if(nclass < NODE_MATH_BASE)
        lclass = nclass;
      else
        if(nclass < NODE_REL_BASE)
          lclass = NODE_LARGE_MATH;
        else
          lclass = NODE_LARGE_REL;

      kb->perf.nodes++;
      kb->perf.num_nodes[lclass]++;

      if(nclass == literal)
        kb->perf.edges++;
      else
        if(nclass == gate || nclass == joint || nclass == math_add || nclass == math_mul || nclass == math_pow)
          kb->perf.edges += 3;
        else
          kb->perf.edges += 2;

      *left = *right = '\0';
      lcup = lcleft = lcright = no_link;
    }

  if(ferror(fp))
    {
      perror(file_name);
      exit(EXIT_FAILURE);
    }

  kb->perf.edges /= 2;

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

  ls = malloc(MAX_NUMERIC_LITERALS * sizeof(sh_literal));
  max_ls = 0;

  lc = no_link;
  stype = io_any;
  packed = 0;
  packedbit = 0;
  defaultval = io_unknown;
  defaultreal = REAL_MAX;
  omissions = io_raw;
  k = 0;

  while(fscanf(fp, " "FUN_FMT" "FUN_FMT" ( "NAME_FMT" , "NAME_FMT" ) # %d / %u , %d , %d , "REAL_IN_FMT" , %u @ "TIME_FMT" \n",
               str, name, name_v, name_w, &lc, &stype, &packed, &packedbit, &defaultreal, &omissions, &k) >= 4)
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

      d = '\0';
      sscanf(str, OP_FMT""OP_FMT, &c, &d);

      switch(c)
        {
          case '!':
            sclass = input_stream;
          break;

          case '?':
          case '.':
            sclass = output_stream;
          break;

          case '_':
            sclass = quiet_stream;
          break;

          default:
            fprintf(stderr, "%s, "OP_FMT": Invalid stream class\n", file_name, c);
            exit(EXIT_FAILURE);
          break;
        }

      if(d == '^')
        {
          num_ls = max_ls;
          
          for(i = 0; i < num_ls; i += 2)
            if(!strcmp(ls[i].name, name) && arc_eq(ls[i].e, null_arc))
              {
                init_literal(&ls[max_ls], name, bufsiz);

                ls[max_ls].ne = ls[i].ne;
                ls[max_ls].e = arc_neg(e);

                max_ls++;

                init_literal(&ls[max_ls], name, bufsiz);

                ls[max_ls].ne = e;
                ls[max_ls].e = ls[i + 1].e;

                max_ls++;
              }

          init_literal(&ls[max_ls], name, bufsiz);

          ls[max_ls].ne = e;
          ls[max_ls].e = null_arc;

          max_ls++;

          init_literal(&ls[max_ls], name, bufsiz);
 
          ls[max_ls].ne = null_arc;
          ls[max_ls].e = arc_neg(e);

          max_ls++;

          if(max_ls >= MAX_NUMERIC_LITERALS - 1)
            {
              fprintf(stderr, "%s: Too many numeric signals\n", file_name);
              exit(EXIT_FAILURE);
            }
        }

      if(rtype != io_quiet && sclass != quiet_stream)
        {
          if(defaultreal >= 0 && defaultreal < io_other && defaultreal == floor(defaultreal))
            defaultval = floor(defaultreal);

          ios = open_stream(name, sclass, e, kb->offset, stype == io_any? rtype : stype, sys5, prefix, path, netpath, defaultval, defaultreal, omissions,
                            packed, packedbit, packed > 0? packed_ios[packed] : NULL);

          kb->io_num[sclass]++;
          kb->io_open++;

          add_stream(&kb->io_stream[sclass], ios);

          if(packed > 0)
            {
              if(!packed_ios[packed])
                packed_ios[packed] = ios;

              packed_ios[packed]->pack.packedtot++;
              packed_ios[packed]->pack.packedcount = packed_ios[packed]->pack.packedtot;
              packed_ios[packed]->pack.gen = FALSE;
            }
        }

      lc = no_link;
      stype = io_any;
      packed = 0;
      packedbit = 0;
      defaultval = io_unknown;
      defaultreal = REAL_MAX;
      omissions = io_raw;
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
  kb->quiet = (rtype == io_quiet) || !ios;
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

  kb->in_count = kb->io_num[input_stream];

  kb->curr_time = kb->offset;
  kb->safe_time = kb->offset;
  kb->max_time = kb->offset + max_time;
  kb->anchor_time = kb->offset;
  kb->all_taken = NULL_TIME;
  kb->abandon = NULL_TIME;
  
  kb->far = TRUE;
  kb->bound = FALSE;
  kb->io_busy = FALSE;
  kb->exiting = FALSE;

  kb->step = step;

  kb->perf.count = 0;
  kb->perf.depth = 0;

  if(max_ls)
    {
      kb->lits = malloc(max_ls * sizeof(sh_literal));

      kb->max_lit = 0;
      for(i = 0; i < max_ls; i++)
        {
          if(!arc_eq(ls[i].e, null_arc) && !arc_eq(ls[i].ne, null_arc))
            {
              kb->lits[kb->max_lit] = ls[i];
              kb->max_lit++;
            }
          else
            free_literal(&ls[i]);
        }

      if(kb->max_lit)
        {
          for(i = 0; i < kb->max_lit; i++)
            {
              e = kb->lits[i].ne;

              for(j = 0; j < kb->max_lit; j++)
                {
                  if(arc_eq(e, kb->lits[(j + i + 1) % kb->max_lit].ne))
                    {
                      kb->lits[i].other = &kb->lits[(j + i + 1) % kb->max_lit];
                      break;
                    }
                }
            }
        }
      else
        {
          free(kb->lits);
          kb->lits = NULL;
        }
    }
  else
    {
      kb->lits = NULL;
      kb->max_lit = 0;
    }

  free(ls);

  n = kb->io_num[input_stream];
  if(!n)
    n = 1;

  kb->max_slice = kb->perf.edges / (IO_INFERENCE_RATIO * n);
  if(!kb->max_slice)
    kb->max_slice = 1;

  kb->slice = kb->max_slice;

  vp = kb->network;
  while(vp)
    {
      if(vp->nclass == literal)
        {
          if(vp->pin[0].history[0].value != REAL_MAX)
            {
              wp = vp->pin[0].e.vp;
              lc = vp->pin[0].e.lc;

              for(t = 0; t < bufsiz; t++)
                {
                  vp->pin[0].history[t].stated = CONSTANT_PHASE;
                  vp->pin[0].history[t].chosen = CONSTANT_PHASE;

                  wp->pin[lc].history[t].stated = CONSTANT_PHASE;
                  wp->pin[lc].history[t].chosen = CONSTANT_PHASE;

                  wp->pin[lc].history[t].value = vp->pin[0].history[t].value;
                }
            }
          else
            {
              e = arc_neg(vp->pin[0].e);

              for(i = 0; i < kb->max_lit; i++)
                if(arc_eq(kb->lits[i].ne, e))
                  {
                    vp->ls = &kb->lits[i];
                    break;
                  }
            }
        }

      vp = vp->vp;
    }

  kb->awake = FALSE;
  kb->done = FALSE;

  pthread_cond_init(&kb->cond_done, NULL);
  pthread_mutex_init(&kb->mutex_barrier, NULL);

  return kb;
}

void close_base(k_base *kb)
{
  node *vp, *wp;
  stream *ios, *next_ios;
  stream_class sclass;
  bool ipc_io;
  int i;

  pthread_mutex_destroy(&kb->mutex_barrier);
  pthread_cond_destroy(&kb->cond_done);

  if(kb->logfp && fclose(kb->logfp))
    {
      perror(NULL);
      exit(EXIT_FAILURE);
    }

  if(!kb->quiet)
    {
      ipc_io = FALSE;
      for(sclass = 0; sclass < STREAM_CLASSES_NUMBER; sclass++)
        {
          ios = kb->io_stream[sclass];
          if(ios)
            do
              {
                next_ios = ios->next_ios;

                if(ios->stype == io_ipc)
                  ipc_io = TRUE;

                if(!close_stream(ios, kb->alpha))
                  return;

                ios = next_ios;
              }
            while(ios != kb->io_stream[sclass]);

          ios = kb->io_stream_2[sclass];
          if(ios)
            do
              {
                next_ios = ios->next_ios;

                if(ios->stype == io_ipc)
                  ipc_io = TRUE;
  
                if(!close_stream(ios, kb->alpha))
                  return;

                ios = next_ios;
              }
            while(ios != kb->io_stream_2[sclass]);
        }

      if(ipc_io && kb->sys5)
        delete_queues_sys5();
    }

  vp = kb->network;
  while(vp)
    {
      wp = vp->vp;
      free_node(vp);
      vp = wp;
    }

  if(kb->lits)
    {
      for(i = 0; i < kb->max_lit; i++)
        free_literal(&kb->lits[i]);

      free(kb->lits);
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
  real value;
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
  value = REAL_MAX;
  while(fscanf(fp, " ( "NAME_FMT" , "NAME_FMT" ) # %d @ "TIME_FMT" = "REAL_IN_FMT" \n",
               name_v, name_w, &lc, &s.t, &value) >= 4)
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

      if(value != REAL_MAX)
        {
          math_safe_state(kb, s, value);
          math_safe_state(kb, ev_neg(s), value);
          value = REAL_MAX;
        }
      else
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
  rst = FALSE;
  irq = TRUE;
  frz = FALSE;
}

void trap1()
{
  rst = TRUE;
  irq = TRUE;
  frz = FALSE;
}

void trap2()
{
  rst = FALSE;
  irq = FALSE;
  frz = !frz;
}

void loops(thread_arg *tp)
{
  k_base *kb;

  kb = tp->kb;

  while(!loop(kb));
  
  myexit(EXIT_SUCCESS);
}

void loops_io(thread_arg *tp)
{
  k_base *kb;

  kb = tp->kb;

  while(!loop_io(kb));

  if(!kb->busywait)
    leave_barrier(kb);

  myexit(EXIT_SUCCESS);
}

void run(char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, io_type rtype, bool hard, bool sys5, bool sturdy, bool busywait,
         int bufexp, d_time max_time, m_time step, m_time origin, char *prefix, char *path, char *netpath, char *alpha)
{
  k_base *kb;
  pthread_t thread_cpu, thread_io;
  pthread_attr_t attributes;
  struct sched_param spar;
  thread_arg targ_cpu, targ_io;
  cpu_set_t cpuset;
  int n;

  kb = open_base(base_name, logfile_name, xref_name,
                 strictly_causal, soundness_check, echo_stdout, rtype, sys5, sturdy, busywait, bufexp, max_time, step, prefix, path, netpath, alpha);

  printf("Network ok -- %d edges, %d nodes (%d gates + %d joints + %d delays + %d literals + %d math ops + %d math rels), %d inputs, %d outputs\n",
         kb->perf.edges, kb->perf.nodes, kb->perf.num_nodes[gate], kb->perf.num_nodes[joint], kb->perf.num_nodes[delay], kb->perf.num_nodes[literal],
         kb->perf.num_nodes[NODE_LARGE_MATH], kb->perf.num_nodes[NODE_LARGE_REL],
         kb->io_num[input_stream], kb->io_num[output_stream]);

  if(state_name)
    {
      n = init_state(kb, state_name);
      printf("State ok -- %d initial conditions\n", n);
    }

  printf("Execution running...\n");

  fflush(stdout);

  pthread_attr_init(&attributes);
  if(hard)
    {
      spar.sched_priority = sched_get_priority_max(SCHED_RR);

      pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
      pthread_attr_setschedpolicy(&attributes, SCHED_RR);
      pthread_attr_setschedparam(&attributes, &spar);
    }

  if(origin > 0)
    kb->time_base = origin;
  else
    kb->time_base = get_time();

  targ_cpu.kb = kb;

  if(pthread_create(&thread_cpu, &attributes, (void *)&loops, (void *)&targ_cpu))
    {
      perror("POSIX thread error");
      exit(EXIT_FAILURE);
    }

  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);

  pthread_setaffinity_np(thread_cpu, sizeof(cpu_set_t), &cpuset);

  targ_io.kb = kb;

  if(pthread_create(&thread_io, &attributes, (void *)&loops_io, (void *)&targ_io))
    {
      perror("POSIX thread error");
      exit(EXIT_FAILURE);
    }

  CPU_ZERO(&cpuset);
  CPU_SET(1, &cpuset);

  pthread_setaffinity_np(thread_io, sizeof(cpu_set_t), &cpuset);

  pthread_join(thread_cpu, NULL);
  pthread_join(thread_io, NULL);

  kb->perf.ticks = get_time() - kb->time_base;
  kb->perf.horizon = kb->curr_time;

  printf("\n%s\n%lu logical inferences (%.3f %% of %d x "TIME_FMT") in %.6f seconds, %.3f KLIPS, %lu depth (avg %.3f)\n",
	irq? "Execution interrupted" : "End of execution",
	kb->perf.count,
	kb->perf.horizon && kb->perf.edges? 100.0 * kb->perf.count / (kb->perf.horizon * kb->perf.edges) : 0,
	kb->perf.edges,
	kb->perf.horizon,
	kb->perf.ticks,
	kb->perf.ticks? kb->perf.count / (1000 * kb->perf.ticks) : 0,
	kb->perf.depth,
	kb->perf.count? (double)kb->perf.depth / kb->perf.count : 0);

  close_base(kb);
}

int main(int argc, char **argv)
{
  char *base_name, *state_name, *logfile_name, *xref_name, *option, *ext, *prefix, *path, *netpath;
  char default_state_name[MAX_STRLEN], default_logfile_name[MAX_STRLEN], default_xref_name[MAX_STRLEN], alpha[SYMBOL_NUMBER + 1];
  bool strictly_causal, soundness_check, echo_stdout, file_io, socket_io, quiet, hard, sys5, sturdy, busywait;
  int i, k, n;
  d_time max_time;
  m_time step, default_step, origin;
  int bufexp;
  io_type rtype;

  signal(SIGINT, (void (*)())&trap);
  signal(SIGUSR1, (void (*)())&trap1);
  signal(SIGUSR2, (void (*)())&trap2);

  base_name = state_name = logfile_name = xref_name = NULL;
  strictly_causal = soundness_check = echo_stdout = file_io = socket_io = quiet = hard = sys5 = sturdy = busywait = FALSE;
  bufexp = DEFAULT_BUFEXP;
  max_time = 0;
  origin = 0;
  default_step = -1;
  prefix = MAGIC_PREFIX;
  path = "";
  netpath = "";
  strcpy(alpha, IO_SYMBOLS);

  for(i = 1; i < argc; i++)
    {
      if(*argv[i] == '-')
        {
          option = argv[i] + 1;
          switch(*option)
            {
            case 'h':
              fprintf(stderr, "Usage: %s [-cdfFilqsSvVxy] [-a alphabet] [-e prefix] [-g origin] [-I state] [-j network] [-L log] [-p path] [-r core] [-t step] [-X symbols] [-z horizon] [base]\n",
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

            case 'j':
              if(*(++option))
                {
                  fprintf(stderr, "%s, %c: Invalid command line option"
                                  " (%s -h for help)\n",
                          argv[i], *option, argv[0]);
                  exit(EXIT_FAILURE);
                }

              if(++i < argc)
                netpath = argv[i]; 
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

                    case 'F':
                      socket_io = TRUE;
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

  if(quiet)
    rtype = io_quiet;
  else
    if(file_io)
      rtype = io_file;
    else
      if(socket_io)
        rtype = io_socket;
      else
        rtype = io_ipc;

  printf("\nTINX "VER" - Temporal Inference Network eXecutor\n"
         "Design & coding by Andrea Giotti, 1998-1999, 2016-2024\n\n");

  fflush(stdout);

  do
    {
      rst = FALSE;
      irq = FALSE;

      run(base_name, state_name, logfile_name, xref_name,
          strictly_causal, soundness_check, echo_stdout, rtype, hard, sys5, sturdy, busywait, bufexp, max_time, step, origin, prefix, path, netpath, alpha);
    }
  while(rst);

  signal(SIGINT, SIG_DFL);
  signal(SIGUSR1, SIG_DFL);
  signal(SIGUSR2, SIG_DFL);

  return EXIT_SUCCESS;
}


