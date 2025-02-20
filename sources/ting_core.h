/*
  TING - Temporal Inference Network Generator
  Design & coding by Andrea Giotti, 2017
*/

#if !defined UNIX_FILE_IO
  #define ANSI_FILE_IO
#endif

#include "tinx_mt.h"
#include <stdarg.h>

#define SOURCE_EXT ".btl"

#define VARSEP "][)(}{"BLANKS"~&|@?!,;:*/^%%+-"
#define VARNAME_FMT "%"MAX_NAMEBUF_C"[^"VARSEP"]"
#define RICHNAME_FMT "`%"MAX_NAMEBUF_C"[^"BLANKS")(`]`"
#define REALRICH_FMT "``%"MAX_NAMEBUF_C"[^"BLANKS")(`]``"
#define STRING_FMT "\"%[^\"]\""
#define NUM_COLORS 5

typedef enum op_type
{
  op_invalid = -1,
  op_name,
  op_dname,
  op_iname,
  op_cname,
  op_fname,
  op_number,
  op_real,
  op_string,
  op_constant,
  op_variable,
  op_dvariable,
  op_ivariable,
  op_iterator,
  op_vector,
  op_matrix,
  op_array3,
  op_array4,
  op_ioqual1,
  op_ioqual2,
  op_ioqual3,
  op_ioqual4,
  op_define,
  op_defarray,
  op_equal,
  op_math_eqv0,
  op_neq,
  op_math_neq0,
  op_gteq,
  op_math_gteq0,  
  op_lt,
  op_math_lt0,
  op_lteq,
  op_gt,
  op_plus,
  op_minus,
  op_mul,
  op_div,
  op_mod,
  op_pow,
  op_root,
  op_log,
  op_chs,
  op_inv,
  op_sin,
  op_cos,
  op_tan,
  op_asin,
  op_acos,
  op_atan,
  op_sum,
  op_prod,
  op_math_delay,
  op_not,
  op_and,
  op_or,
  op_delay,
  op_interval_1,
  op_interval_2,
  op_interval_3,
  op_interval_4,
  op_fourdim,
  op_at,
  op_happen,
  op_only,
  op_since,
  op_until,
  op_forall,
  op_exists,
  op_one,
  op_one_check,
  op_unique,
  op_xor,
  op_imply,
  op_eqv,
  op_code,
  op_code_num,
  op_combine,
  op_comb_pair,
  op_com_delay,
  op_varinterval_1,
  op_varinterval_2,
  op_varinterval_3,
  op_varinterval_4,
  op_varinterval_5,
  op_com_at,
  op_com_happen,
  op_com_only,
  op_pvalue,
  op_nvalue,
  op_input,
  op_output,
  op_aux,
  op_internal,
  op_init,
  op_var_at,
  op_assign,
  op_join,
  op_join_qual,
  op_join_array,
  op_iter,
  op_when,
  op_in,
  op_neg_range,
  op_include,
  op_range,
  op_down,
  OP_TYPES_NUMBER
} op_type;

typedef struct btl_specification btl_specification;

struct btl_specification
{
  op_type ot;
  char symbol[MAX_STRLEN];
  d_time value;
  real realval;
  btl_specification *left;
  btl_specification *right;
  char debug[DEBUG_STRLEN];
};

typedef struct symlist symlist;

typedef struct smallnode smallnode;

struct smallnode
{
  char name[MAX_NAMELEN];
  char root[MAX_NAMELEN];
  node_class nclass;
  d_time k;
  real realval;
  symlist *literal_symtab;
  bool neg;
  bool zombie;
  int turn;
  smallnode *up;
  link_code up_dir;
  smallnode *up_2;
  link_code up_2_dir;
  smallnode *left;
  link_code left_dir;
  smallnode *right;
  link_code right_dir;
  char debug[DEBUG_STRLEN];
  smallnode *vp;
};

#define SPECIAL ((smallnode *)(-1))

typedef struct subtreeval
{
  btl_specification *btl;
  btl_specification *btltwo;
  btl_specification *btldef;
  smallnode *vp;
  d_time a;
  d_time b;
  d_time c;
  d_time d;
  d_time *xtra;
  d_time *ytra;
  d_time *ztra;
  d_time *wtra;
  real realval;
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
  smallnode **tofrom;
  smallnode *from;
  smallnode **fromto;
  smallnode *to;
  io_class sclass;
  io_type stype;
  int packed;
  int packedbit;
  io_type_3 defaultval;
  io_type_4 omissions;
  real defaultreal;
  litval val;
  bool shared;
  bool removed;
  struct io_signal *nextsig;
} io_signal;

typedef struct constant
{
  char name[MAX_NAMELEN];
  d_time value;
  real realval;
  int scope_level;
  int scope_unit;
  bool error;
  struct constant *nextcon;
} constant;

typedef struct group
{
  char name[MAX_NAMELEN];
  int group_id;
  struct group *nextgrp;
} group;

typedef struct initial_condition
{
  char name[MAX_NAMELEN];
  bool neg;
  d_time t;
  real realval;
  struct initial_condition *nextic;
} initial_condition;

#define SOURCE_BUFSIZE 262144
#define SOURCE_BUFSIZE_C "262143"

#define SYMTAB_SIZE 8191    /* Prime */
#define SYMTAB_DEPTH 256
#define NUM_LEVELS 256
#define NUM_VECTORS 262144
#define NUM_COMBS 65536

#define BTL_HISTORY_LEN 256

typedef struct vector
{
  smallnode **bpp;
  smallnode *bp;
  smallnode *vp;
} vector;

struct symlist
{
  smallnode *vp;
  symlist *nextlit;
  symlist *nextocc;
};

typedef struct c_base
{
  char source[SOURCE_BUFSIZE];
  symlist *symtab;
  smallnode *symptr[SYMTAB_SIZE][SYMTAB_DEPTH];
  io_signal *sigtab;
  io_signal *sigptr[SYMTAB_SIZE][SYMTAB_DEPTH];
  int num_signals;
  group *grptab;
  group *grpptr[SYMTAB_SIZE][SYMTAB_DEPTH];
  int num_groups;
  constant *inttab;
  constant *intptr[SYMTAB_SIZE][SYMTAB_DEPTH];
  initial_condition *ictab;
  int num_ics;
  int iterator[NUM_LEVELS];
  int scope_unit[NUM_LEVELS];
  vector purgearrow[NUM_VECTORS];
  int start_purgearrows;
  int end_purgearrows;
  vector erasearrow[NUM_VECTORS];
  int start_erasearrows;
  int end_erasearrows;
  int part_nodes[NODE_CLASSES_NUMBER];
  int part_signals[IO_CLASSES_NUMBER];
  int num_vargen;
  char path[MAX_STRLEN];
  bool seplit_fe;
  bool seplit_su;
  bool merge;
  bool constout;
  bool constout_sugg;
  bool constout_user;
  bool outaux;
  bool outint;
  int fancymsg;
  smallnode *nullnode;
  smallnode *specnode;
  smallnode *network;
} c_base;

typedef struct compinfo
{
  bool ok;
  int edges;
  int num_nodes;
  int part_nodes[NODE_LARGE_CLASSES];
  int num_signals;
  int part_signals[IO_CLASSES_NUMBER];
  int num_ics;
} compinfo;

#define CREATE_XOR(P, Q) create_operation(op_or, create_operation(op_and, create_operation(op_not, P, NULL, "(~ %s)"), Q, "(%s & %s)"), \
                                                 create_operation(op_and, create_operation(op_not, copy_specification(Q), NULL, "(~ %s)"), copy_specification(P), "(%s & %s)"), "(%s | %s)")
#define CREATE_IMPLY(P, Q) create_operation(op_or, create_operation(op_not, P, NULL, "(~ %s)"), Q, "(%s | %s)")
#define CREATE_EQV(P, Q) create_operation(op_and, CREATE_IMPLY(P, Q), CREATE_IMPLY(copy_specification(Q), copy_specification(P)), "(%s & %s)")

btl_specification *alloc_syntnode(void);
btl_specification *create_ground(op_type ot, char *symbol, d_time value, real realval);
btl_specification *create_operation(op_type ot, btl_specification *left, btl_specification *right, char *debug);
char *richwrap(char *string);
char *richwrap2(char *string);
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
void add_ic(c_base *cb, char *name, bool neg, d_time t, real realval);
void gensym(c_base *cb, char *symbol, char *type, litval val, bool incr);
subtreeval preval(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
subtreeval forall(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
subtreeval exists(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
subtreeval unique(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
subtreeval subset_unique(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
subtreeval subset_only(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
subtreeval at_happen(c_base *cb, btl_specification *btl, btl_specification *btldef, d_time a, d_time b, int level, d_time param, real realval, bool dual);
subtreeval since_until(c_base *cb, btl_specification *btl, btl_specification *btldef, btl_specification *btl_2, btl_specification *btldef_2, int level, d_time param, real realval, bool sign);
subtreeval com_at_happen(c_base *cb, btl_specification *btl, btl_specification *btldef, btl_specification *varextr, int arity, d_time maxtime, int level, d_time param, real realval, bool dual);
subtreeval only(c_base *cb, btl_specification *btl, btl_specification *btldef, d_time a, d_time b, int level, d_time param, real realval);
subtreeval sum(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
subtreeval prod(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
btl_specification *combine(c_base *cb, btl_specification *ap[], btl_specification *spec, int h, int k, int level, d_time param, real realval);
subtreeval exec_comb(c_base *cb, btl_specification *spec, int level, d_time param, real realval);
subtreeval eval(c_base *cb, btl_specification *spec, smallnode *vp, link_code ext_dir, bool neg, io_class sclass, io_type stype, io_type_2 packed, io_type_3 defaultval, io_type_4 omissions,
                d_time t, real realval);
void purge_smallnode(c_base *cb, smallnode *vp, smallnode **bpp, smallnode **dpp, litval val, bool flip);
void close_smallbranches(c_base *cb, smallnode *xp, smallnode *yp, smallnode **xbpp, smallnode **ybpp, smallnode *vp);
void purge_smalltree(c_base *cb, smallnode *vp, smallnode **bpp);
void erase_smalltree(c_base *cb, smallnode *vp, smallnode **bpp);
smallnode **gendir(smallnode *vp, smallnode **bpp, link_code dir);
link_code *dirdir(smallnode *vp, smallnode **bpp, link_code dir);
smallnode **get_neighbor_handle(smallnode *vp, smallnode **bpp, link_code dir);
int save_smalltree(c_base *cb, FILE *fp);
int save_signals(c_base *cb, FILE *fp);
int save_ics(c_base *cb, FILE *fp);
int save_xref(c_base *cb, FILE *fp);
void link_cotree(c_base *cb);
void raise_signals(c_base *cb, smallnode *vp);
smallnode *build_smalltree(c_base *cb, symlist *sl, bool neg);
smallnode *build_twotrees(c_base *cb, symlist *si);
smallnode *build_cotree(c_base *cb);
compinfo compile(char *source_name, char *base_name, char *state_name, char *xref_name, char *path,
                 bool seplit_fe, bool seplit_su, bool merge, bool constout, bool constout_sugg, bool constout_user, bool outaux, bool outint, bool postopt, bool fancymsg);
char *opname(op_type ot);

