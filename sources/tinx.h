/*
  TINX - Temporal Inference Network eXecutor
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2017
*/

#if !defined INLINE
#define INLINE
#endif

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

#include <unistd.h>
#include <sched.h>

#include <mqueue.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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

#define DEFAULT_BUFEXP 10     /* 1024 */

#define IO_INFERENCE_RATIO 3  /* 90.625 % */

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
} linkage;

#define MAX_STRLEN 256
#define MAX_NAMELEN 63
#define MAX_NAMEBUF (2 * MAX_NAMELEN + 1)
#define MAX_NAMEBUF_C "126"
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

#define MAGIC_NAME "TINX"
#define MAGIC_PREFIX "/"MAGIC_NAME":"
#define PERMS 0660
#define MAX_MESSAGES 10
#define MSG_SIZE 128

#define MAGIC_TOKEN_I 'G'
#define MAGIC_TOKEN_O 'H'
#define FAKE_BASE_BITS 2    /* b = 4 = 2^2 bits (should be 36 for exact behaviour), n = 15 (lenght), 2 * b^n - 1 == LONG_MAX */

#define IO_ERR_LIMIT 1000000

typedef mqd_t channel_posix;

channel_posix add_queue_posix(char *name, stream_class sclass);
int send_message_posix(channel_posix c, char *a);
int read_message_posix(channel_posix c, char *a);

#define failed_queue_posix(C) ((C) < 0)
#define commit_queue_posix(C) mq_close(C)
#define remove_queue_posix(A) mq_unlink(A)
#define delete_queues_posix() FALSE

typedef struct channel_sys5
  {
    int paddr;
    long int saddr;
  } channel_sys5;

typedef struct message
  {
    long mtype;
    char mtext;
  } message;

channel_sys5 add_queue_sys5(char *name, stream_class sclass);
int send_message_sys5(channel_sys5 c, char *a);
int read_message_sys5(channel_sys5 c, char *a);
int delete_queues_sys5(void);

#define failed_queue_sys5(C) ((C).paddr < 0)
#define commit_queue_sys5(C) FALSE
#define remove_queue_sys5(C) FALSE

int delete_queues(void);

long int queue_key(char *name);

typedef enum io_type
{
  io_any,
  io_ipc,
  io_file,
  IO_TYPES_NUMBER
} io_type;

typedef struct k_base k_base;

struct stream
{
  char name[MAX_NAMELEN];
  stream_class sclass;
  arc e;
  arc ne;
  d_time deadline;
  char file_name[MAX_STRLEN];
  char chan_name[MAX_STRLEN];
  file fp;
  channel_posix chan;
  channel_sys5 chan5;
  int fails;
  int errors;
  stream *next_ios;
  stream *prev_ios;
  bool file_io;
  bool sys5;
  bool open;
  bool (* io_perform)(k_base *kb, stream *ios);
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
  linkage pin[LINK_CODES_NUMBER];
  char *debug;
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

#define safe_state(KB, S) { if(!is_stated(KB, S)) state(KB, S); }

#define HASH_SIZE 8191    /* Prime */
#define HASH_DEPTH 64
#define HASH_FMT "%X"

typedef struct info
{
  int nodes;
  int num_nodes[NODE_CLASSES_NUMBER];
  int edges;
  d_time horizon;
  unsigned long int count;
  unsigned long int depth;
  m_time ticks;
} info;

typedef enum symbol
{
  false_symbol,
  true_symbol,
  unknown_symbol,
  end_symbol,
  term_symbol,
  eof_symbol,
  SYMBOL_NUMBER
} symbol;

typedef struct k_base k_base;

typedef struct k_base
{
  event focus;
  event last_input;
  int bsm1;
  int bsbt;
  int bsd4;
  node *network;
  stream *io_stream[STREAM_CLASSES_NUMBER];
  int io_num[STREAM_CLASSES_NUMBER];
  int io_count[STREAM_CLASSES_NUMBER];
  int io_open;
  int slice;
  int max_slice;
  char alpha[SYMBOL_NUMBER + 1];
  d_time curr_time;
  d_time max_time;
  d_time offset;
  bool far;
  bool strictly_causal;
  bool soundness_check;
  bool trace_focus;
  bool echo_stdout;
  bool quiet;
  bool sys5;
  bool sturdy;
  bool busywait;
  bool io_busy;
  bool exiting;
  FILE *logfp;
  m_time time_base;
  m_time step;
  info perf;
  node *table[HASH_SIZE][HASH_DEPTH];
} k_base;

#define BLANKS " \t\n\r"
#define SEPARATORS BLANKS"(,;:.?!)@"
#define NAME_FMT "%"MAX_NAMEBUF_C"[^"SEPARATORS"]"
#define LESS_SEPARATORS BLANKS";:.?!@"
#define FUN_FMT "%"MAX_NAMEBUF_C"[^"LESS_SEPARATORS"]"
#define OP_FMT "%c"
#define ARG_FMT TIME_FMT

#define DEFAULT_NAME "default"
#define STATE_SUFFIX "_ic"
#define LOG_SUFFIX "_log"
#define NETWORK_EXT ".tin"
#define EVENT_LIST_EXT ".evl"
#define STREAM_EXT ".io"
#define XREF_EXT ".sym"

#define IO_SYMBOLS "01?.\x1b\xff"

/* Protos */

INLINE event ev_neg(event s);
arc arc_between(node *vp, node *wp, link_code lc);
INLINE m_time get_time(void);
unsigned long int hashnode(char *name);

INLINE void state(k_base *kb, event s);
INLINE event choose(k_base *kb);
INLINE void process(k_base *kb, event s);
INLINE void scan_ios(k_base *kb, stream_class sclass);
INLINE bool loop(k_base *kb);

bool input_f(k_base *kb, stream *ios);
bool output_f(k_base *kb, stream *ios);
bool input_m(k_base *kb, stream *ios);
bool output_m(k_base *kb, stream *ios);
void trace(k_base *kb, event s);
stream *open_stream(char *name, stream_class sclass, arc e, d_time offset, bool file_io, bool sys5, char *prefix, char *path);
void close_stream(stream *ios, char *alpha);
void remove_stream(stream **handle);

node *alloc_node(char *name);
void init_node(node *vp, node_class nclass, d_time k, int bs);
void free_node(node *vp);
node *name2node(k_base *kb, char *name, bool create);
void thread_network(node *network);
k_base *open_base(char *base_name, char *logfile_name, char *xref_name,
                  bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool sys5, bool sturdy, bool busywait,
                  int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *alpha);
void close_base(k_base *kb);
int init_state(k_base *kb, char *state_name);

void trap(void);
info run(char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, bool file_io, bool quiet, bool hard, bool sys5, bool sturdy, bool busywait,
         int bufexp, d_time max_time, m_time step, m_time origin, char *prefix, char *path, char *alpha);

/* End of protos */

