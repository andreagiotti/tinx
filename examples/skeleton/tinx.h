/*
  TINX - Temporal Inference Network eXecutor
  Design & coding by Andrea Giotti, 1998-1999
  Revised 2016-2021
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

typedef char bool;

#define FALSE 0
#define TRUE (!FALSE)

#define min(X, Y) (((X) < (Y))? (X) : (Y))
#define max(X, Y) (((X) > (Y))? (X) : (Y))

typedef long int d_time;
#define TIME_FMT "%ld"
#define FTIME_FMT "%06ld"
#define NULL_TIME LONG_MAX

typedef double m_time;
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
  #define put_file(C, XP) (fputc(* (XP), (C)) == EOF)
  #define mget_file(C, XP, N) (fread((XP), sizeof(* (XP)), (N), (C)) < (N))
  #define mput_file(C, XP, N) (fwrite((XP), sizeof(* (XP)), (N), (C)) < (N))
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
  #define get_file(C, XP) (read((C), (XP), sizeof(* (XP))) < sizeof(* (XP)))
  #define put_file(C, XP) (write((C), (XP), sizeof(* (XP))) < sizeof(* (XP)))
  #define mget_file(C, XP, N) (read((C), (XP), (N) * sizeof(* (XP))) < (N) * sizeof(* (XP)))
  #define mput_file(C, XP, N) (write((C), (XP), (N) * sizeof(* (XP))) < (N) * sizeof(* (XP)))
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

#define IO_ERR_LIMIT 16777216

typedef mqd_t channel_posix;

channel_posix add_queue_posix(char *name, stream_class sclass);
#define send_message_posix(C, A) msend_message_posix((C), (A), 1)
#define read_message_posix(C, A) mread_message_posix((C), (A), 1)
int msend_message_posix(channel_posix c, char *a, int n);
int mread_message_posix(channel_posix c, char *a, int n);
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
    char mtext[MSG_SIZE];
  } message;

channel_sys5 add_queue_sys5(char *name, stream_class sclass);
#define send_message_sys5(C, A) msend_message_sys5((C), (A), 1)
#define read_message_sys5(C, A) mread_message_sys5((C), (A), 1)
int msend_message_sys5(channel_sys5 c, char *a, int n);
int mread_message_sys5(channel_sys5 c, char *a, int n);
#define failed_queue_sys5(C) ((C).paddr < 0)
#define commit_queue_sys5(C) FALSE
#define remove_queue_sys5(C) FALSE
int delete_queues_sys5(void);

long int queue_key(char *name);

typedef struct safesocket
{
  int lsid;
  int asid;
} safesocket;

#define PORTBASE 7777
#define SOCKETFLAGS MSG_DONTWAIT

safesocket add_socket(char *name, stream_class sclass);
#define open_input_socket(A) add_socket((A), input_stream)
#define open_output_socket(A) add_socket((A), output_stream)
#define is_socket_open(S) ((S).asid >= 0)
int close_socket(safesocket sock);
#define clean_socket(A) FALSE
#define get_socket(S, XP) (recv((S).asid, (XP), sizeof(* (XP)), SOCKETFLAGS) < sizeof(* (XP)))
#define put_socket(S, XP) (send((S).asid, (XP), sizeof(* (XP)), SOCKETFLAGS) < sizeof(* (XP)))
#define mget_socket(S, XP, N) (recv((S).asid, (XP), (N) * sizeof(* (XP)), SOCKETFLAGS) < (N) * sizeof(* (XP)))
#define mput_socket(S, XP, N) (send((S).asid, (XP), (N) * sizeof(* (XP)), SOCKETFLAGS) < (N) * sizeof(* (XP)))
#define sync_socket(S) FALSE
#define socket_error(S) FALSE
#define reset_socket(S) FALSE

typedef enum io_type
{
  io_any,
  io_ipc,
  io_file,
  io_socket,
  io_quiet,
  IO_TYPES_NUMBER
} io_type;

typedef enum io_type_2
{
  io_binary,
  io_packed,
  IO_TYPES_2_NUMBER
} io_type_2;

typedef enum io_type_3
{
  io_false,
  io_true,
  io_unknown,
  IO_TYPES_3_NUMBER
} io_type_3;

typedef enum io_type_4
{
  io_raw,
  io_filter,
  io_omit,
  IO_TYPES_4_NUMBER
} io_type_4;

typedef struct k_base k_base;

typedef enum io_symbol
{
  false_symbol,
  true_symbol,
  unknown_symbol,
  end_symbol,
  term_symbol,
  eof_symbol,
  SYMBOL_NUMBER
} io_symbol;

#define MAX_IOS 4096
#define IOS_BUFFER_SIZE 1024

typedef struct packet
{
  int packed;
  char packedchar[IOS_BUFFER_SIZE];
  int packedcount;
  int packedtot;
  int packedbit;
  bool gen;
} packet;

struct stream
{
  char name[MAX_NAMELEN];
  stream_class sclass;
  arc e;
  arc ne;
  io_type stype;
  io_type_3 defaultval;
  bool sys5;
  bool skip[IO_TYPES_3_NUMBER];
  char file_name[MAX_STRLEN];
  char chan_name[MAX_STRLEN];
  char socket_name[MAX_STRLEN];
  file fp;
  channel_posix chan;
  channel_sys5 chan5;
  safesocket sock;
  int fails;
  int errors;
  packet pack;
  stream *packed_ios;
  stream *next_ios;
  stream *prev_ios;
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

typedef struct k_base
{
  event focus;
  event last_input;
  int bsm1;
  int bsbt;
  int bsd4;
  node *network;
  stream *io_stream[STREAM_CLASSES_NUMBER];
  stream *io_stream_2[STREAM_CLASSES_NUMBER];
  int io_num[STREAM_CLASSES_NUMBER];
  int io_open;
  int slice;
  int max_slice;
  char alpha[SYMBOL_NUMBER + 1];
  d_time curr_time;
  d_time max_time;
  d_time offset;
  d_time anchor_time;
  bool far;
  bool bound;
  bool strictly_causal;
  bool soundness_check;
  bool trace_focus;
  bool echo_stdout;
  bool quiet;
  bool sys5;
  bool sturdy;
  bool busywait;
  bool io_busy;
  FILE *logfp;
  m_time time_base;
  m_time step;
  info perf;
  node *table[HASH_SIZE][HASH_DEPTH];
} k_base;

#define BLANKS " \t\n\r"
#define SEPARATORS BLANKS"(,;:.?!)@"
#define NAME_FMT "%"MAX_NAMEBUF_C"[^"SEPARATORS"]"
#define FUN_FMT "%"MAX_NAMEBUF_C"[^"BLANKS"]"
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
stream *open_stream(char *name, stream_class sclass, arc e, d_time offset, io_type stype, bool sys5,
                    char *prefix, char *path, char *netpath, io_type_3 defaultval, io_type_4 omissions, int packed, int packedbit, stream *packed_ios);
void close_stream(stream *ios, char *alpha);
INLINE void add_stream(stream **handle, stream *ios);
INLINE void remove_stream(stream **handle);

node *alloc_node(char *name);
void init_node(node *vp, node_class nclass, d_time k, int bs);
void free_node(node *vp);
node *name2node(k_base *kb, char *name, bool create);
void thread_network(node *network);
k_base *open_base(char *base_name, char *logfile_name, char *xref_name,
                  bool strictly_causal, bool soundness_check, bool echo_stdout, io_type rtype, bool sys5, bool sturdy, bool busywait,
                  int bufexp, d_time max_time, m_time step, char *prefix, char *path, char *netpath, char *alpha);
void close_base(k_base *kb);
int init_state(k_base *kb, char *state_name);

void trap(void);
info run(char *base_name, char *state_name, char *logfile_name, char *xref_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout, io_type rtype, bool hard, bool sys5, bool sturdy, bool busywait,
         int bufexp, d_time max_time, m_time step, m_time origin, char *prefix, char *path, char *netpath, char *alpha);

/* End of protos */

