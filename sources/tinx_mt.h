/*
  TINX - Temporal Inference Network eXecutor
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2017
*/

#if !defined INLINE
#define INLINE
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#include <pthread.h>
#include <sched.h>

#if defined POSIX_IPC_IO
  #include <mqueue.h>
#endif

#if defined UNIX_FILE_IO || defined UNIX_IPC_IO
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/types.h>
  #include <sys/ipc.h>
  #include <sys/msg.h>
#endif

typedef char bool;

#define FALSE 0
#define TRUE (!FALSE)

#define min(X, Y) (((X) < (Y))? (X) : (Y))
#define max(X, Y) (((X) > (Y))? (X) : (Y))

typedef long int d_time;
#define TIME_FMT "%ld"
#define FTIME_FMT "%06ld"
#define NULL_TIME LONG_MAX

typedef float m_time;
#define CLOCK_TYPE CLOCK_MONOTONIC_RAW
#define DEFAULT_STEP_SEC ((m_time) 0.1)

#define DEFAULT_BUFEXP 10    /* 1024 */

#define MAX_THREADS 256
#define DEFAULT_THREADS 7    /* +1 */

typedef enum link_code
{
  no_link = -1,
  parent,
  left_son,
  right_son,
  LINK_CODES_NUMBER
} link_code;

typedef struct node node;

typedef struct arc
{
  node *vp;
  link_code lc;
} arc;

typedef struct event
{
  arc e;
  d_time t;
} event;

#define ev_eq(R, S) ((R).e.vp == (S).e.vp && (R).e.lc == (S).e.lc && \
                     (R).t == (S).t)

#if defined NDEBUG
#define valid(S) ((S).e.vp)
#else
#define valid(S) ((S).e.vp && (S).e.lc >= 0 && (S).e.lc < LINK_CODES_NUMBER)
#endif

typedef int phase;
#define NULL_PHASE INT_MAX

typedef struct record
{
  phase stated;
  phase chosen;
  event other;
  event next;
  pthread_mutex_t mutex;
} record;

typedef enum stream_class
{
  input_stream,
  output_stream,
  STREAM_CLASSES_NUMBER
} stream_class;

typedef struct stream stream;

typedef struct linkage
{
  arc e;
  /* Attributes of arc_neg(e) */
  record *history;
  stream *io_stream[STREAM_CLASSES_NUMBER];
} linkage;

#define MAX_STRLEN 256
#define MAX_NAMELEN 63
#define MAX_NAMEBUF (2 * MAX_NAMELEN + 1)
#define MAX_NAMEBUF_C "126"
#define FAKE_BASE_BITS 2    /* b = 4 = 2^2 bits (should be 36 for exact behaviour), n = 15 (lenght), 2 * b^n - 1 == LONG_MAX */
#define DEBUG_STRLEN 65536

#if defined ANSI_FILE_IO

  typedef FILE * file;

  #define open_input_file(A) fopen(A, "r")
  #define open_output_file(A) fopen(A, "w")
  #define is_file_open(C) (C)
  #define close_file(C) fclose(C)
  #define clean_file(A) remove(A)
  #define get_file(C, XP) ((* (XP) = fgetc(C)) == EOF)
  #define put_file(C, XP) (fputc(* (XP), C) == EOF)
  #define sync_file(C) fflush(C)
  #define file_error(C) ferror(C)
  #define reset_file(C) clearerr(C)

#elif defined UNIX_FILE_IO

  typedef int file;

  #define open_input_file(A) open(A, O_RDONLY | O_NONBLOCK)
  #define open_output_file(A) open(A, O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0666)
  #define is_file_open(C) ((C) >= 0)
  #define close_file(C) close(C)
  #define clean_file(A) unlink(A)
  #define get_file(C, XP) (read(C, XP, sizeof(* (XP))) < sizeof(* (XP)))
  #define put_file(C, XP) (write(C, XP, sizeof(* (XP))) < sizeof(* (XP)))
  #define sync_file(C) FALSE
  #define file_error(C) FALSE
  #define reset_file(C) FALSE

#endif

#if defined POSIX_IPC_IO

  typedef mqd_t channel;

  channel add_queue(char *name, stream_class sclass);
  int send_message(channel c, char *a);
  int read_message(channel c, char *a);

  #define failed_queue(C) ((C) < 0)
  #define commit_queue(C) mq_close(C)
  #define remove_queue(A) mq_unlink(A)

  #define MAGIC_NAME "TINX"
  #define MAGIC_PREFIX "/"MAGIC_NAME":"
  #define PERMS 0660
  #define MAX_MESSAGES 10
  #define MSG_SIZE 1024

  #define IO_ERR_LIMIT 1000000

#elif defined UNIX_IPC_IO

  typedef struct channel
  {
    int paddr;
    long int saddr;
  } channel;

  typedef struct message
  {
    long mtype;
    char mtext[1];
  } message;

  channel add_queue(char *name, stream_class sclass);
  int send_message(channel c, char *a);
  int read_message(channel c, char *a);

  #define failed_queue(C) ((C).paddr < 0)
  #define commit_queue(C) FALSE

  int remove_queue(char *name);

  #define MAGIC_PREFIX ""
  #define MAGIC_TOKEN 'G'
  #define PERMS 0660

  #define IO_ERR_LIMIT 1000000

#endif

struct stream
{
  char name[MAX_NAMELEN];
  stream_class sclass;
  linkage *pin;
  d_time deadline;
  char file_name[MAX_STRLEN];
  char chan_name[MAX_STRLEN];
  file fp;
  channel chan;
  stream *next_ios;
  stream *prev_ios;
  bool file_io;
  bool open;
};

typedef enum node_class
{
  null_op = -1,
  gate,
  joint,
  delay,
  literal,
  NODE_CLASSES_NUMBER
} node_class;

#define CLASS_SYMBOLS "GJDL"

struct node
{
  char name[MAX_NAMELEN];
  node_class nclass;
  d_time k;
  char *debug;
  linkage pin[LINK_CODES_NUMBER];
  node *vp;
};

#define link_of(E) ((E).vp->pin[(E).lc])
#define arc_neg(E) (link_of(E).e)
#define phase_of(KB, S) ((S).t >> (KB)->bsbt)
#define index_of(KB, S) ((S).t & (KB)->bsm1)
#define record_of(KB, S) (link_of((S).e).history[index_of(KB, S)])

#define is_stated(KB, S) (record_of(KB, S).stated == phase_of(KB, S))
#define is_chosen(KB, S) (record_of(KB, S).chosen == phase_of(KB, S))
#define reset_stated(KB, S) (record_of(KB, S).stated = NULL_PHASE)
#define reset_chosen(KB, S) (record_of(KB, S).chosen = NULL_PHASE)
#define set_stated(KB, S) (record_of(KB, S).stated = phase_of(KB, S))
#define set_chosen(KB, S) (record_of(KB, S).chosen = phase_of(KB, S))

#define other(KB, S) (record_of(KB, S).other)
#define next(KB, S) (record_of(KB, S).next)

#define lock_record_of(KB, S) pthread_mutex_lock(&record_of(KB, S).mutex)
#define unlock_record_of(KB, S) pthread_mutex_unlock(&record_of(KB, S).mutex)
#define safe_state(KB, S, TID) { lock_record_of(KB, S); if(!is_stated(KB, S)) state(KB, S, TID); unlock_record_of(KB, S); }

#define lock_delta(KB, TID) pthread_mutex_lock(&(KB)->pv[TID].mutex)
#define unlock_delta(KB, TID) pthread_mutex_unlock(&(KB)->pv[TID].mutex)
#define lock_barrier(KB) pthread_mutex_lock(&(KB)->mutex_barrier)
#define unlock_barrier(KB) pthread_mutex_unlock(&(KB)->mutex_barrier)

#define wait_deadline(KB, TID) pthread_cond_wait(&(KB)->cond_done[TID], &(KB)->mutex_barrier)
#define signal_deadline(KB, TID) pthread_cond_signal(&(KB)->cond_done[TID])

#define myexit(RV) pthread_exit(NULL)

#define HASH_SIZE 8191    /* Prime number */
#define HASH_DEPTH 8
#define HASH_FMT "%X"
#define hash(X) strtoul(X, NULL, 36)

#define set_fields(F, TID) ((F) |= 1 << (TID))
#define reset_fields(F, TID) ((F) &= ~ (1 << (TID)))
#define check_fields(F, TID) ((F) & 1 << (TID))

typedef enum deadline_state
{
  normal,
  empty,
  far,
  DEADLINE_STATE_NUMBER
} deadline_state;

typedef struct priv_vars
{
  event focus;
  event last_input;
  int curr_tid;
  int delta_len;
  deadline_state dstate;
  bool passed;
  bool io_busy;
  unsigned long int count;
  unsigned long int depth;
  unsigned long int halts;
  pthread_mutex_t mutex;
} priv_vars;

typedef struct info
{
  int edges;
  d_time horizon;
  unsigned long int count;
  unsigned long int depth;
  unsigned long int halts;
  m_time ticks;
} info;

typedef struct k_base k_base;

typedef struct k_base
{
  priv_vars pv[MAX_THREADS];
  int num_threads;
  int done[MAX_THREADS + 1];
  int barrier_count;
  int bsm1;
  int bsbt;
  int bsd4;
  node *network;
  stream *io_stream[STREAM_CLASSES_NUMBER];
  int io_num[STREAM_CLASSES_NUMBER];
  int io_count[STREAM_CLASSES_NUMBER];
  int io_open;
  int io_togo;
  int io_slice;
  int io_curr_tid;
  int fails;
  int errors;
  d_time curr_time;
  d_time max_time;
  d_time offset;
  d_time anchor_time;
  d_time min_sigma;
  d_time last_empty;
  d_time last_far;
  bool io_err;
  bool strictly_causal;
  bool soundness_check;
  bool trace_focus;
  bool echo_stdout;
  bool echo_debug;
  bool quiet;
  bool sturdy;
  bool exiting;
  FILE *logfp;
  m_time time_base;
  m_time step;
  info perf;
  bool (* input)(k_base *kb, stream *ios);
  bool (* output)(k_base *kb, stream *ios);
  pthread_mutex_t mutex_barrier;
  pthread_cond_t cond_done[MAX_THREADS + 1];
  node *table[HASH_SIZE][HASH_DEPTH];
} k_base;

typedef struct thread_arg
{
  int tid;
  k_base *kb;
} thread_arg;

#define BLANKS " \t\n\r"
#define SEPARATORS BLANKS"(,;:?!)@"
#define NAME_FMT "%"MAX_NAMEBUF_C"[^"SEPARATORS"]"
#define OP_FMT "%c"
#define ARG_FMT TIME_FMT

#define DEFAULT_NAME "default"
#define STATE_SUFFIX "_ic"
#define LOG_SUFFIX "_log"
#define NETWORK_EXT ".tin"
#define EVENT_LIST_EXT ".evl"
#define STREAM_EXT ".io"
#define XREF_EXT ".sym"

#define LO_CHAR '0'
#define HI_CHAR '1'
#define UNKNOWN_CHAR '?'
#define END_CHAR '.'
#define TERM_CHAR '\x1b' /* Escape */

/* Protos */

INLINE event ev_neg(event s);
arc arc_between(node *vp, node *wp);
INLINE m_time get_time(void);
int gcd(int p, int q);

INLINE void state(k_base *kb, event s, int tid);
INLINE event choose(k_base *kb, int tid);
INLINE void process(k_base *kb, event s, int tid);
INLINE void scan_inputs(k_base *kb);
INLINE void scan_outputs(k_base *kb);
INLINE bool loop(k_base *kb, int tid);
INLINE bool loop_io(k_base *kb, int tid);
INLINE int get_tid(k_base *kb, int tid);
INLINE int get_tid_io(k_base *kb);
INLINE void barrier(k_base *kb, int tid);

bool input_f(k_base *kb, stream *ios);
bool input_m(k_base *kb, stream *ios);
bool output_f(k_base *kb, stream *ios);
bool output_m(k_base *kb, stream *ios);
void trace(k_base *kb, event s, int tid);
stream *open_stream(char *name, stream_class sclass, arc e, d_time offset, bool file_io);
void close_stream(stream *ios);
void remove_stream(stream **handle);

node *alloc_node(k_base *kb, char *name);
void free_node(k_base *kb, node *vp);
node *name2node(k_base *kb, char *name);
void thread_network(node *network);
k_base *open_base(char *base_name, char *logfile_name, char *xref_name,
                  bool strictly_causal, bool soundness_check, bool echo_stdout, bool echo_debug, bool file_io, bool quiet, bool sturdy,
                  int bufexp, d_time max_time, m_time step, int num_threads);
void close_base(k_base *kb);
void init_state(k_base *kb, char *state_name);

void trap(void);
void loops(thread_arg *tp);
void loops_io(thread_arg *tp);
info run(char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool echo_debug, bool file_io, bool quiet, bool hard, bool sturdy,
         int bufexp, d_time max_time, m_time step, int num_threads);

/* End of protos */
