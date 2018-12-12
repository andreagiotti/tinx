%{

#include "ting_core.h"
#include "ting_parser.h"
#include "ting_lexer.h"

int yyerror(YYLTYPE *yylloc, btl_specification **spec_handle, yyscan_t scanner, const char *msg)
  {
    tracker tr;
    int i, k, col, len;
    char buffer[MAX_STRLEN];

    tr = yyget_extra(scanner);

    *buffer = '\0';
    len = 0;
    for(i = 1; i <= NUM_TOKENS; i++)
      {
        k = (tr.idx + i) % NUM_TOKENS;
        strcat(buffer, tr.token[k]);
        len += tr.length[k];
      }

    col = tr.column[(tr.idx + 1) % NUM_TOKENS];

    fprintf(stderr, "%s: Error, line %d, column %d-%d, \"%s\"\n", msg, tr.line[tr.idx] + 1, col, col + len - 1, buffer);

    return 0;
  }

%}

%code requires
{
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
  typedef void *yyscan_t;
#endif

#define NUM_TOKENS 3

typedef struct tracker
  {
    char *token[NUM_TOKENS];
    int line[NUM_TOKENS];
    int column[NUM_TOKENS];
    int length[NUM_TOKENS];
    int idx;
  } tracker;
}

%output  "ting_parser.c"
%defines "ting_parser.h"
 
%define api.pure
%define parse.error verbose
%lex-param   { yyscan_t scanner }
%parse-param { btl_specification **spec_handle }
%parse-param { yyscan_t scanner }

%locations

%union
{
  char symbol[MAX_NAMELEN];
  d_time value;
  btl_specification *specification;
  btl_specification *formblock;
  btl_specification *declblock;
  btl_specification *extformula;
  btl_specification *extdeclaration;
  btl_specification *formula;
  btl_specification *interval;
  btl_specification *declaration;
  btl_specification *io_decl;
  btl_specification *decllist;
  btl_specification *eventlist;
  btl_specification *filelist;
  btl_specification *assignlist;
  btl_specification *assignment;
  btl_specification *varname;
  btl_specification *declname;
  btl_specification *initname;
  btl_specification *eventname;
  btl_specification *qualname;
  btl_specification *constname;
  btl_specification *filename;
  btl_specification *expression;
  btl_specification *number;
}

%left TOKEN_SEMICOLON
%left TOKEN_EQV
%left TOKEN_IMPLY
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_HAPPEN
%left TOKEN_AT
%left TOKEN_SINCE
%left TOKEN_UNTIL
%left TOKEN_NOT
%left TOKEN_PLUS
%left TOKEN_MINUS
%left TOKEN_MUL
%left TOKEN_DIV

%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_SEMICOLON
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LSQUARED
%token TOKEN_RSQUARED
%token TOKEN_COMMA
%token TOKEN_ITER
%token TOKEN_DEFINE
%token TOKEN_INCLUDE
%token TOKEN_INPUT
%token TOKEN_OUTPUT
%token TOKEN_AUX
%token TOKEN_INIT
%token TOKEN_EQV
%token TOKEN_IMPLY
%token TOKEN_OR
%token TOKEN_AND
%token TOKEN_FORALL
%token TOKEN_EXISTS
%token TOKEN_ONE
%token TOKEN_UNIQUE
%token TOKEN_SINCE
%token TOKEN_UNTIL
%token TOKEN_AT
%token TOKEN_HAPPEN
%token TOKEN_NOT
%token TOKEN_EQUAL
%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_MUL
%token TOKEN_DIV
%token TOKEN_ANY
%token TOKEN_IPC
%token TOKEN_FILE
%token <symbol> TOKEN_NAME
%token <symbol> TOKEN_FILENAME
%token <symbol> TOKEN_ITERATOR
%token <value> TOKEN_NUMBER

%type <specification> spec
%type <formblock> fblock
%type <declblock> dblock
%type <extformula> xform
%type <extdeclaration> xdecl
%type <formula> form
%type <interval> inter
%type <declaration> decl
%type <io_decl> iod
%type <decllist> dlist
%type <eventlist> elist
%type <filelist> flist
%type <assignlist> alist
%type <assignment> assign
%type <varname> name
%type <declname> dname
%type <initname> iname
%type <eventname> ename
%type <qualname> qname
%type <constname> cname
%type <filename> fname
%type <expression> expr
%type <number> num

%%

input
    : spec[S] { *spec_handle = $S; }
    ;

spec
    : xform[X] { $$ = $X; }
    | xdecl[X] { $$ = $X; }
    | /* empty */
    ;

xform
    : xform[L] xform[R] { $$ = create_operation(op_and, $L, $R, "%s ; %s"); }
    | xform[L] xdecl[R] { $$ = create_operation(op_join, $L, $R, "%s ; %s"); }
    | xdecl[L] xform[R] { $$ = create_operation(op_join, $L, $R, "%s ; %s"); }
    | TOKEN_ITER TOKEN_LPAREN expr[L] TOKEN_RPAREN fblock[R] { $$ = create_operation(op_forall, $R, $L, "iter(%2$s) { %1$s ; }"); }
    | fblock[B] { $$ = $B; }
    ;

xdecl
    : xdecl[L] xdecl[R] { $$ = create_operation(op_join, $L, $R, "%s ; %s"); }
    | TOKEN_ITER TOKEN_LPAREN expr[L] TOKEN_RPAREN dblock[R] { $$ = create_operation(op_iter, $R, $L, "iter(%2$s) { %1$s ; }"); }
    | dblock[B] { $$ = $B; }
    ;

fblock
    : TOKEN_LBRACKET xform[B] TOKEN_RBRACKET { $$ = $B; }
    | form[F] TOKEN_SEMICOLON { $$ = $F; }
    ;

dblock
    : TOKEN_LBRACKET xdecl[B] TOKEN_RBRACKET { $$ = $B; }
    | decl[D] TOKEN_SEMICOLON { $$ = $D; }
    ;

form
    : TOKEN_NOT form[F] { $$ = create_operation(op_not, $F, NULL, "~ %s"); }
    | form[L] TOKEN_AND form[R] { $$ = create_operation(op_and, $L, $R, "%s & %s"); }
    | form[L] TOKEN_OR form[R] { $$ = create_operation(op_or, $L, $R, "%s | %s"); }
    | form[L] TOKEN_AT expr[R] { $$ = create_operation(op_delay, $L, $R, "%s @ %s"); }
    | form[L] TOKEN_HAPPEN expr[R] { $$ = create_operation(op_delay, $L, $R, "%s ? %s"); }
    | form[L] TOKEN_AT inter[R] { $$ = create_operation(op_at, $L, $R, "%s @ %s"); }
    | form[L] TOKEN_HAPPEN inter[R] { $$ = create_operation(op_happen, $L, $R, "%s ? %s"); }
    | TOKEN_SINCE TOKEN_LPAREN form[L] TOKEN_COMMA form[R] TOKEN_RPAREN { $$ = create_operation(op_since, $L, $R, "since(%s , %s)"); }
    | TOKEN_UNTIL TOKEN_LPAREN form[L] TOKEN_COMMA form[R] TOKEN_RPAREN { $$ = create_operation(op_until, $L, $R, "until(%s , %s)"); }
    | TOKEN_FORALL TOKEN_LPAREN form[L] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_forall, $L, $R, "forall(%s , %s)"); }
    | TOKEN_EXISTS TOKEN_LPAREN form[L] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_exists, $L, $R, "exists(%s , %s)"); }
    | TOKEN_ONE TOKEN_LPAREN form[L] TOKEN_COMMA expr[M] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_one, $L, create_operation(op_interval_1, $M, $R, "%s , %s"), "one(%s , %s)"); }
    | TOKEN_UNIQUE TOKEN_LPAREN form[L] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_unique, $L, $R, "unique(%s , %s)"); }
    | form[L] TOKEN_IMPLY form[R] { $$ = create_operation(op_imply, $L, $R, "%s --> %s"); }
    | form[L] TOKEN_EQV form[R] { $$ = create_operation(op_eqv, $L, $R, "%s == %s"); }
    | TOKEN_LPAREN form[F] TOKEN_RPAREN { $$ = $F; }
    | name[A] { $$ = $A; }
    ;

inter
    : TOKEN_LSQUARED expr[L] TOKEN_COMMA expr[R] TOKEN_RSQUARED { $$ = create_operation(op_interval_1, $L, $R, "[%s , %s]"); }
    | TOKEN_LPAREN expr[L] TOKEN_COMMA expr[R] TOKEN_RSQUARED { $$ = create_operation(op_interval_2, $L, $R, "(%s , %s]"); }
    | TOKEN_LSQUARED expr[L] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_interval_3, $L, $R, "[%s , %s)"); }
    | TOKEN_LPAREN expr[L] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_interval_4, $L, $R, "(%s , %s)"); }
    ;

decl
    : TOKEN_INPUT iod[B] dlist[A] { $$ = create_operation(op_input, $A, $B, "input[%2$s] %1$s"); }
    | TOKEN_INPUT dlist[A] { $$ = create_operation(op_input, $A, create_ground(op_ioqual, "", io_any), "input[%2$s] %1$s"); }
    | TOKEN_OUTPUT iod[B] dlist[A] { $$ = create_operation(op_output, $A, $B, "output[%2$s] %1$s"); }
    | TOKEN_OUTPUT dlist[A] { $$ = create_operation(op_output, $A, create_ground(op_ioqual, "", io_any), "output[%2$s] %1$s"); }
    | TOKEN_AUX dlist[A] { $$ = create_operation(op_aux, $A, NULL, "aux %s"); }
    | TOKEN_INIT elist[A] { $$ = create_operation(op_init, $A, NULL, "init %s"); }
    | TOKEN_DEFINE alist[A] { $$ = $A; }
    | TOKEN_INCLUDE flist[A] { $$ = $A; }
    ;

iod
    : TOKEN_LSQUARED TOKEN_ANY TOKEN_RSQUARED { $$ = create_ground(op_ioqual, "any", io_any); }
    | TOKEN_LSQUARED TOKEN_IPC TOKEN_RSQUARED { $$ = create_ground(op_ioqual, "ipc", io_ipc); }
    | TOKEN_LSQUARED TOKEN_FILE TOKEN_RSQUARED { $$ = create_ground(op_ioqual, "file", io_file); }
    ;

dlist
    : dlist[L] TOKEN_COMMA dname[R] { $$ = create_operation(op_join, $L, $R, "%s , %s"); }
    | dname[A] { $$ = $A; }
    ;

elist
    : elist[L] TOKEN_COMMA ename[R] { $$ = create_operation(op_join, $L, $R, "%s , %s"); }
    | ename[A] { $$ = $A; }
    ;

flist
    : flist[L] TOKEN_COMMA fname[R] { $$ = create_operation(op_join, $L, $R, "%s , %s"); }
    | fname[A] { $$ = $A; }
    ;

alist
    : alist[L] TOKEN_COMMA assign[R] { $$ = create_operation(op_join, $L, $R, "%s ; %s"); }
    | assign[A] { $$ = $A; }
    ;

assign
   : cname[L] TOKEN_EQUAL expr[R] { $$ = create_operation(op_define, $L, $R, "define %s = %s"); }
   ;

ename
    : qname[L] TOKEN_AT expr[R] { $$ = create_operation(op_var_at, $L, $R, "%s @ %s"); }
    | qname[L] TOKEN_AT inter[R] { $$ = create_operation(op_var_at, $L, $R, "%s @ %s"); }
    ;

qname
    : TOKEN_NOT iname[A] { $$ = create_operation(op_not, $A, NULL, "~ %s"); }
    | iname[A] { $$ = $A; }
    ;

name
    : TOKEN_NAME TOKEN_LPAREN expr[M] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, create_ground(op_name, $1, 0),
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | TOKEN_NAME TOKEN_LPAREN expr[R] TOKEN_RPAREN { $$ = create_operation(op_vector, create_ground(op_name, $1, 0), $R, "%s(%s)"); }
    | TOKEN_NAME { $$ = create_ground(op_name, $1, 0); }
    ;

dname
    : TOKEN_NAME TOKEN_LPAREN expr[M] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, create_ground(op_dname, $1, 0),
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | TOKEN_NAME TOKEN_LPAREN expr[R] TOKEN_RPAREN { $$ = create_operation(op_vector, create_ground(op_dname, $1, 0), $R, "%s(%s)"); }
    | TOKEN_NAME { $$ = create_ground(op_dname, $1, 0); }
    ;

iname
    : TOKEN_NAME TOKEN_LPAREN expr[M] TOKEN_COMMA expr[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, create_ground(op_iname, $1, 0),
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | TOKEN_NAME TOKEN_LPAREN expr[R] TOKEN_RPAREN { $$ = create_operation(op_vector, create_ground(op_iname, $1, 0), $R, "%s(%s)"); }
    | TOKEN_NAME { $$ = create_ground(op_iname, $1, 0); }
    ;

cname
    : TOKEN_NAME { $$ = create_ground(op_cname, $1, 0); }
    ;

fname
    : TOKEN_FILENAME { $$ = create_ground(op_fname, $1, 0); }
    ;

expr
    : expr[L] TOKEN_PLUS expr[R] { $$ = create_operation(op_plus, $L, $R, "%s + %s"); }
    | expr[L] TOKEN_MINUS expr[R] { $$ = create_operation(op_minus, $L, $R, "%s - %s"); }
    | expr[L] TOKEN_MUL expr[R] { $$ = create_operation(op_mul, $L, $R, "%s * %s"); }
    | expr[L] TOKEN_DIV expr[R] { $$ = create_operation(op_div, $L, $R, "%s / %s"); }
    | TOKEN_MINUS expr[E] { $$ = create_operation(op_chs, $E, NULL, "- %s"); }
    | TOKEN_LPAREN expr[E] TOKEN_RPAREN { $$ = $E; }
    | num[N] { $$ = $N; }
    ;

num
    : TOKEN_NUMBER { $$ = create_ground(op_number, "", $1); }
    | TOKEN_ITERATOR { $$ = create_ground(op_iterator, $1, 0); }
    | TOKEN_NAME { $$ = create_ground(op_constant, $1, 0); }
    ;

%%

