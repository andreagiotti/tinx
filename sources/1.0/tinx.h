/*
  TINX - Temporal Inference Network eXecutor
  Design & coding by Andrea Giotti, 1998-1999
*/

/* #define NDEBUG */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

typedef char bool;

#define FALSE 0
#define TRUE (!FALSE)

typedef int d_time;

#define TIME_FMT "%d"
#define TIME_LIMIT 1024

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
#define valid(S) ((S).e.vp && (S).e.lc >= 0 && (S).e.lc < LINK_CODES_NUMBER && \
                  (S).t >= 0 && (S).t < TIME_LIMIT)
#endif

typedef struct record
{
  bool stated;
  bool chosen;
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

typedef struct link
{
  arc e;
  /* Attributes of arc_neg(e) */
  record history[TIME_LIMIT];
  stream *io_stream[STREAM_CLASSES_NUMBER];
} link;

#define MAX_STRLEN 32
#define MAX_NAMELEN 24
#define MAX_NAMELEN_C "23"

struct stream
{
  char name[MAX_NAMELEN];
  stream_class class;
  link *pin;
  d_time offset;
  char file_name[MAX_STRLEN];
  FILE *fp;
  stream *next_ios;
  stream *prev_ios;
  bool open;
};

typedef enum node_class
{
  null_op = -1,
  gate,
  joint,
  delay,
  NODE_CLASSES_NUMBER
} node_class;

#define CLASS_SYMBOLS "GJD"

struct node
{
  char name[MAX_NAMELEN];
  node_class class;
  d_time k;
  link pin[LINK_CODES_NUMBER];
  node *vp;
};

#define link_of(E) ((E).vp->pin[(E).lc])
#define arc_neg(E) (link_of(E).e)
#define record_of(S) (link_of((S).e).history[(S).t])
#define stated(S) (record_of(S).stated)
#define chosen(S) (record_of(S).chosen)
#define other(S) (record_of(S).other)
#define next(S) (record_of(S).next)

#define HASH_SIZE 8191 /* Prime */
#define HASH_FMT "%X"
#define hash(X) (strtoul((X), NULL, 36) % HASH_SIZE)

typedef struct k_base
{
  event focus;
  node *network;
  node *table[HASH_SIZE];
  stream *io_stream[STREAM_CLASSES_NUMBER];
  bool strictly_causal;
  bool soundness_check;
  bool trace_focus;
  bool echo_stdout;
  FILE *logfp;
  bool idle;
} k_base;

#define BLANKS " \t\n\r"
#define SEPARATORS BLANKS"(,;:?!)@"
#define NAME_FMT "%"MAX_NAMELEN_C"[^"SEPARATORS"]"
#define OP_FMT "%c"
#define ARG_FMT TIME_FMT

#define DEFAULT_NAME "default"
#define STATE_SUFFIX "_ic"
#define LOG_SUFFIX "_log"
#define NETWORK_EXT ".tin"
#define EVENT_LIST_EXT ".evl"
#define STREAM_EXT ".io"

#define LO_CHAR '0'
#define HI_CHAR '1'
#define END_CHAR '.'
#define TERM_CHAR '\x1b' /* Escape */

/* Protos */

event ev_neg(event s);
arc arc_between(node *vp, node *wp);

void state(k_base *kb, event s);
event choose(k_base *kb);
void process(k_base *kb, event s);
bool loop(k_base *kb);

event input(stream *ios);
void output(event s);
bool scan_inputs(k_base *kb);
void trace(k_base *kb);
stream *open_stream(char *name, stream_class class, arc e, d_time offset);
void close_stream(stream *ios);

node *alloc_node(char *name);
node *name2node(k_base *kb, char *name);
void thread_network(node *network);
k_base *open_base(char *base_name, char *logfile_name,
                  bool strictly_causal, bool soundness_check, bool echo_stdout);
void close_base(k_base *kb);
void init_state(k_base *kb, char *state_name);

void trap(void);
void run(char *base_name, char *state_name, char *logfile_name,
         bool strictly_causal, bool soundness_check, bool echo_stdout);

/* End of protos */

