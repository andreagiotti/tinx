/*
  TING - Temporal Inference Network Generator
  Design & coding by Andrea Giotti, 2017
*/

#if !defined UNIX_FILE_IO
  #define ANSI_FILE_IO
#endif

#include "tinx_mt.h"

#define SOURCE_EXT ".btl"

#define VARSEP "][)(}{"BLANKS"~&|@?,;:*/^+-"
#define VARNAME_FMT "%"MAX_NAMEBUF_C"[^"VARSEP"]"
#define STRING_FMT "\"%[^\"]\""

typedef enum op_type
{
  op_invalid,
  op_name,
  op_dname,
  op_iname,
  op_cname,
  op_fname,
  op_number,
  op_string,
  op_constant,
  op_iterator,
  op_vector,
  op_matrix,
  op_ioqual1,
  op_ioqual2,
  op_ioqual3,
  op_ioqual4,
  op_plus,
  op_minus,
  op_mul,
  op_div,
  op_mod,
  op_pow,
  op_chs,
  op_not,
  op_and,
  op_or,
  op_delay,
  op_interval_1,
  op_interval_2,
  op_interval_3,
  op_interval_4,
  op_at,
  op_happen,
  op_since,
  op_until,
  op_forall,
  op_exists,
  op_one,
  op_one_check,
  op_unique,
  op_imply,
  op_eqv,
  op_code,
  op_code_num,
  op_input,
  op_output,
  op_aux,
  op_init,
  op_var_at,
  op_join,
  op_join_qual,
  op_define,
  op_iter,
  op_when,
  op_in,
  op_neg_range,
  op_include,
  OP_TYPES_NUMBER
} op_type;

typedef struct btl_specification
{
  op_type ot;
  char symbol[MAX_STRLEN];
  d_time value;
  struct btl_specification *left;
  struct btl_specification *right;
  char debug[DEBUG_STRLEN];
} btl_specification;

typedef struct smallnode
{
  char name[MAX_NAMELEN];
  node_class nclass;
  d_time k;
  int literal_id;
  bool neg;
  bool zombie;
  int open;
  struct smallnode *up;
  struct smallnode *up_2;
  struct smallnode *left;
  struct smallnode *right;
  char debug[DEBUG_STRLEN];
  struct smallnode *vp;
} smallnode;

typedef struct subtreeval
{
  btl_specification *btl;
  btl_specification *btldef;
  smallnode *vp;
  d_time a;
  d_time b;
  d_time *xtra;
  d_time *ytra;
  d_time *ztra;
  d_time *wtra;
  bool neg;
} subtreeval;

typedef enum io_class
{
  internal_class,
  aux_class,
  input_class,
  output_class,
  IO_CLASSES_NUMBER
} io_class;

typedef enum litval
{
  negated,
  asserted,
  undefined
} litval;

typedef struct io_signal
{
  char name[MAX_NAMELEN];
  char root[MAX_NAMELEN];
  smallnode *from;
  smallnode *to;
  link_code occurr;
  link_code occurr_neg;
  io_class sclass;
  io_type stype;
  int packed;
  int packedbit;
  io_type_3 defaultval;
  io_type_4 omissions;
  litval val;
  bool removed;
  int signal_id;
} io_signal;

typedef struct constant
{
  char name[MAX_NAMELEN];
  d_time value;
  int integer_id;
} constant;

typedef struct group
{
  char name[MAX_NAMELEN];
  int group_id;
} group;

typedef struct initial_condition
{
  char name[MAX_NAMELEN];
  bool neg;
  d_time t;
} initial_condition;

#define SOURCE_BUFSIZE 262144
#define SOURCE_BUFSIZE_C "262143"

#define SYMTAB_SIZE 8191    /* Prime */
#define SYMTAB_DEPTH 64
#define NUM_LITERALS 8192
#define NUM_OCCURRENCES 8192
#define NUM_ICS 8192
#define NUM_INTEGERS 8192
#define NUM_LEVELS 256

#define BTL_HISTORY_LEN 256

typedef struct c_base
{
  char source[SOURCE_BUFSIZE];
  smallnode *symtab[NUM_LITERALS][NUM_OCCURRENCES];
  smallnode *symptr[SYMTAB_SIZE][SYMTAB_DEPTH];
  int symcount[NUM_LITERALS];
  int num_literals;
  io_signal sigtab[NUM_LITERALS];
  io_signal *sigptr[SYMTAB_SIZE][SYMTAB_DEPTH];
  int num_signals;
  initial_condition ictab[NUM_ICS];
  int num_ics;
  constant inttab[NUM_INTEGERS];
  constant *intptr[SYMTAB_SIZE][SYMTAB_DEPTH];
  int num_integers;
  group grptab[NUM_LITERALS];
  group *grpptr[SYMTAB_SIZE][SYMTAB_DEPTH];
  int num_groups;
  int iterator[NUM_LEVELS];
  int num_nodes[NODE_CLASSES_NUMBER];
  int num_vargen;
  char path[MAX_STRLEN];
  bool seplit_fe;
  bool seplit_su;
  bool merge;
  bool constout;
  bool outaux;
  bool outint;
  smallnode *network;
} c_base;

typedef struct compinfo
{
  bool ok;
  int edges;
  int tot_nodes;
  int num_nodes[NODE_CLASSES_NUMBER];
  int num_signals;
  int num_ics;
} compinfo;

typedef enum direction
{
  dir_left,
  dir_right
} direction;

#define genup(VP) ((VP)->up? &((VP)->up) : &((VP)->up_2))

#define CREATE_IMPLY(P, Q) create_operation(op_or, create_operation(op_not, P, NULL, "(~ %s)"), Q, "(%s | %s)")
#define CREATE_EQV(P, Q) create_operation(op_and, CREATE_IMPLY(P, Q), CREATE_IMPLY(copy_specification(Q), copy_specification(P)), "(%s & %s)")

btl_specification *alloc_syntnode(void);
btl_specification *create_ground(op_type ot, char *symbol, d_time value);
btl_specification *create_operation(op_type ot, btl_specification *left, btl_specification *right, char *debug);
void delete_specification(btl_specification *sp);
btl_specification *copy_specification(btl_specification *sp);
void print_specification(btl_specification *sp);
smallnode *create_smallnode(c_base *cb, node_class nclass);
void delete_smallnode(c_base *cb, smallnode *vp);
void delete_zombies(c_base *cb);
void delete_smalltree(c_base *cb);
btl_specification *parse(char *expr);
unsigned long int hashsymbol(char *name);
smallnode *name2smallnode(c_base *cb, char *name, bool create);
io_signal *name2signal(c_base *cb, char *name, bool create);
constant *name2constant(c_base *cb, char *name, bool create);
void add_ic(c_base *cb, char *name, bool neg, d_time t);
void gensym(c_base *cb, char *symbol, char *type, litval val, bool incr);
subtreeval preval(c_base *cb, btl_specification *spec, int level, int param);
subtreeval at_happen(c_base *cb, btl_specification *spec, int level, int param, bool dual);
subtreeval since_until(c_base *cb, btl_specification *spec, int level, int param, bool dual);
subtreeval eval(c_base *cb, btl_specification *spec, smallnode *vp, bool neg, io_class sclass, io_type stype, io_type_2 packed, io_type_3 defaultval, io_type_4 omissions, d_time t);
void purge_smallnode(c_base *cb, smallnode *vp, smallnode *bp, litval val);
void close_smallbranches(c_base *cb, smallnode *xp, smallnode *yp, smallnode *bp);
void purge_smalltree(c_base *cb, smallnode *vp, smallnode *bp);
void erase_smalltree(c_base *cb, smallnode *vp, smallnode *bp);
smallnode **gendir(smallnode *vp, smallnode *bp, direction dir);
smallnode **get_neighbor_handle(smallnode *vp, smallnode *wp);
int save_smalltree(c_base *cb, FILE *fp);
int save_signals(c_base *cb, FILE *fp);
int save_ics(c_base *cb, FILE *fp);
int save_xref(c_base *cb, FILE *fp);
void link_cotree(c_base *cb);
void raise_signals(c_base *cb, smallnode *vp);
smallnode *build_smalltree(c_base *cb, int i, bool neg);
smallnode *build_twotrees(c_base *cb, int i);
smallnode *build_cotree(c_base *cb);
compinfo compile(char *source_name, char *base_name, char *state_name, char *xref_name, char *path, bool seplit_fe, bool seplit_su, bool merge, bool constant, bool outaux, bool outint);
char *opname(op_type ot);

