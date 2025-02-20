
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

        if(tr.token[k])
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
  real realval;
  btl_specification *other;
}

%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_SEMICOLON
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LSQUARED
%token TOKEN_RSQUARED
%token TOKEN_COMMA
%token TOKEN_COLON
%token TOKEN_ITER
%token TOKEN_WHEN
%token TOKEN_WHENX
%token TOKEN_ELSE
%token TOKEN_DEFINE
%token TOKEN_INCLUDE
%token TOKEN_INPUT
%token TOKEN_OUTPUT
%token TOKEN_AUX
%token TOKEN_INIT
%token TOKEN_CODE
%token TOKEN_PVALUE
%token TOKEN_NVALUE
%token TOKEN_EQV
%token TOKEN_IMPLY
%token TOKEN_INVIMPLY
%token TOKEN_XOR
%token TOKEN_OR
%token TOKEN_AND
%token TOKEN_FORALL
%token TOKEN_EXISTS
%token TOKEN_ONE
%token TOKEN_UNIQUE
%token TOKEN_COMBINE
%token TOKEN_SINCE
%token TOKEN_UNTIL
%token TOKEN_ONLY
%token TOKEN_HAPPEN
%token TOKEN_AT
%token TOKEN_NOT
%token TOKEN_EQUAL
%token TOKEN_NEQ
%token TOKEN_LT
%token TOKEN_GT
%token TOKEN_LTEQ
%token TOKEN_GTEQ
%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_MUL
%token TOKEN_DIV
%token TOKEN_MOD
%token TOKEN_POW
%token TOKEN_ROOT
%token TOKEN_LOG
%token TOKEN_SIN
%token TOKEN_COS
%token TOKEN_TAN
%token TOKEN_ASIN
%token TOKEN_ACOS
%token TOKEN_ATAN
%token TOKEN_SUM
%token TOKEN_PROD
%token TOKEN_KE
%token TOKEN_KPI
%token TOKEN_ON
%token TOKEN_IN
%token TOKEN_IS
%token TOKEN_NEG
%token TOKEN_ANY
%token TOKEN_IPC
%token TOKEN_FILE
%token TOKEN_REMOTE
%token TOKEN_BINARY
%token TOKEN_PACKED
%token TOKEN_UNKNOWN
%token TOKEN_FALSE
%token TOKEN_TRUE
%token TOKEN_DEFAULT
%token TOKEN_RAW
%token TOKEN_FILTER
%token TOKEN_OMIT
%token TOKEN_UNDERSCORE
%token <symbol> TOKEN_NAME
%token <symbol> TOKEN_RICHNAME
%token <symbol> TOKEN_REALNAME
%token <symbol> TOKEN_REALRICH
%token <symbol> TOKEN_STRING
%token <symbol> TOKEN_ITERATOR
%token <value> TOKEN_NUMBER
%token <realval> TOKEN_REAL

%left TOKEN_SEMICOLON
%left TOKEN_EQV
%left TOKEN_IMPLY
%left TOKEN_INVIMPLY
%left TOKEN_XOR
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_ONLY
%left TOKEN_HAPPEN
%left TOKEN_AT
%left TOKEN_NOT
%left TOKEN_PLUS
%left TOKEN_MINUS
%left TOKEN_MUL
%left TOKEN_DIV
%left TOKEN_MOD
%left TOKEN_POW

%nonassoc TOKEN_WHENX
%nonassoc TOKEN_ELSE

%type <other> specification
%type <other> block
%type <other> blocklist
%type <other> formula
%type <other> interval
%type <other> declaration
%type <other> iodecl
%type <other> iodecllist
%type <other> decllist
%type <other> eventlist
%type <other> filelist
%type <other> assignlist
%type <other> globveclist
%type <other> assignment
%type <other> name
%type <other> mathname
%type <other> declname
%type <other> initname
%type <other> indexedname
%type <other> varidxname
%type <other> mathidxname
%type <other> declidxname
%type <other> initidxname
%type <other> initmathidxname
%type <other> constidxname
%type <other> eventname
%type <other> qualname
%type <other> initmathname
%type <other> constname
%type <other> exprorrange
%type <other> globvector
%type <other> filename
%type <other> expression
%type <other> exprmul
%type <other> exprchs
%type <other> exprpow
%type <other> exprroot
%type <other> exprgnd
%type <other> varrange
%type <other> selrange
%type <other> exprrange
%type <other> number
%type <other> realnumber
%type <other> nameornum
%type <other> string
%type <other> vartime
%type <other> postime
%type <other> negtime
%type <other> varinterval
%type <other> atconstruct
%type <other> happenconstruct
%type <other> onlyconstruct
%type <other> mathrelation
%type <other> mathexpression
%type <other> mathexprmul
%type <other> mathexprchs
%type <other> mathexprpow
%type <other> mathexprroot
%type <other> mathexprdelay
%type <other> mathexprgnd

%%

input
    : specification[S] { *spec_handle = $S; }
    ;

specification
    : blocklist[X] { $$ = $X; }
    | /* empty */
    ;

blocklist
    : block[L] blocklist[R] { $$ = create_operation(op_join, $L, $R, "%s ; %s"); }
    | block[B] { $$ = $B; }
    ;

block
    : TOKEN_ITER TOKEN_LPAREN varrange[L] TOKEN_RPAREN block[R] { $$ = create_operation(op_iter, $R, $L, "iter(%2$s) { %1$s ; }"); }
    | TOKEN_WHEN TOKEN_LPAREN selrange[L] TOKEN_RPAREN block[R] %prec TOKEN_WHENX { $$ = create_operation(op_when, $R, $L, "when(%2$s) { %1$s ; }"); }
    | TOKEN_WHEN TOKEN_LPAREN selrange[L] TOKEN_RPAREN block[M] TOKEN_ELSE block[R]
      { $$ = create_operation(op_join, create_operation(op_when, $M, $L, "when(%2$s) { %1$s ; }"),
                                       create_operation(op_when, $R, create_operation(op_neg_range, copy_specification($L), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
    | TOKEN_LBRACKET blocklist[B] TOKEN_RBRACKET { $$ = $B; }
    | formula[F] TOKEN_SEMICOLON { $$ = $F; }
    | declaration[D] TOKEN_SEMICOLON { $$ = $D; }
    ;

formula
    : TOKEN_NOT formula[F] { $$ = create_operation(op_not, $F, NULL, "(~ %s)"); }
    | formula[L] TOKEN_AND formula[R] { $$ = create_operation(op_and, $L, $R, "(%s & %s)"); }
    | formula[L] TOKEN_OR formula[R] { $$ = create_operation(op_or, $L, $R, "(%s | %s)"); }
    | TOKEN_SINCE TOKEN_LPAREN formula[L] TOKEN_COMMA formula[R] TOKEN_RPAREN { $$ = create_operation(op_since, $L, $R, "since(%s , %s)"); }
    | TOKEN_UNTIL TOKEN_LPAREN formula[L] TOKEN_COMMA formula[R] TOKEN_RPAREN { $$ = create_operation(op_until, $L, $R, "until(%s , %s)"); }
    | TOKEN_FORALL TOKEN_LPAREN formula[L] TOKEN_COMMA varrange[R] TOKEN_RPAREN { $$ = create_operation(op_forall, $L, $R, "forall(%s , %s)"); }
    | TOKEN_EXISTS TOKEN_LPAREN formula[L] TOKEN_COMMA varrange[R] TOKEN_RPAREN { $$ = create_operation(op_exists, $L, $R, "exists(%s , %s)"); }
    | TOKEN_ONE TOKEN_LPAREN formula[L] TOKEN_COMMA selrange[M] TOKEN_COMMA varrange[R] TOKEN_RPAREN { $$ = create_operation(op_one, $L,
                                                                                                            create_operation(op_one_check, $M, $R, "%s , %s"), "one(%s , %s)"); }
    | TOKEN_UNIQUE TOKEN_LPAREN formula[L] TOKEN_COMMA varrange[R] TOKEN_RPAREN { $$ = create_operation(op_unique, $L, $R, "unique(%s , %s)"); }
    | TOKEN_COMBINE TOKEN_LPAREN formula[L] TOKEN_COMMA formula[M] TOKEN_COMMA varrange[R] TOKEN_RPAREN { $$ = create_operation(op_combine,
      create_operation(op_comb_pair, $L, $M, "%s , %s"), $R, "combine(%s , %s)"); }
    | formula[L] TOKEN_XOR formula[R] { $$ = create_operation(op_xor, $L, $R, "(%s \\ %s)"); }
    | formula[L] TOKEN_IMPLY formula[R] { $$ = create_operation(op_imply, $L, $R, "(%s --> %s)"); }
    | formula[L] TOKEN_INVIMPLY formula[R] { $$ = create_operation(op_imply, $R, $L, "(%2$s <-- %1$s)"); }
    | formula[L] TOKEN_EQV formula[R] { $$ = create_operation(op_eqv, $L, $R, "(%s == %s)"); }
    | TOKEN_CODE TOKEN_LPAREN varidxname[L] TOKEN_COMMA string[R] TOKEN_RPAREN { $$ = create_operation(op_code, $L, $R, "code(%s , \"%s\")"); }
    | TOKEN_CODE TOKEN_LPAREN varidxname[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_code_num, $L, $R, "code(%s , %s)"); }
    | TOKEN_LPAREN formula[F] TOKEN_RPAREN { $$ = $F; }
    | atconstruct[C] { $$ = $C; }
    | happenconstruct[C] { $$ = $C; }
    | onlyconstruct[C] { $$ = $C; }
    | mathrelation[C] { $$ = $C; }
    | varidxname[A] { $$ = $A; }
    ;

atconstruct
    : formula[L] TOKEN_AT expression[R] { $$ = create_operation(op_delay, $L, $R, "(%s @ %s)"); }
    | formula[L] TOKEN_AT vartime[R] { $$ = create_operation(op_com_delay, $L, $R, "(%s @ %s)"); }
    | formula[L] TOKEN_AT interval[R] { $$ = create_operation(op_at, $L, $R, "(%s @ %s)"); }
    | formula[L] TOKEN_AT varinterval[R] { $$ = create_operation(op_com_at, $L, $R, "(%s @ %s)"); }
    ;

happenconstruct
    : formula[L] TOKEN_HAPPEN expression[R] { $$ = create_operation(op_delay, $L, $R, "(%s ? %s)"); }
    | formula[L] TOKEN_HAPPEN vartime[R] { $$ = create_operation(op_com_delay, $L, $R, "(%s ? %s)"); }
    | formula[L] TOKEN_HAPPEN interval[R] { $$ = create_operation(op_happen, $L, $R, "(%s ? %s)"); }
    | formula[L] TOKEN_HAPPEN varinterval[R] { $$ = create_operation(op_com_happen, $L, $R, "(%s ? %s)"); }
    ;

onlyconstruct
    : formula[L] TOKEN_ONLY expression[R] { $$ = create_operation(op_delay, $L, $R, "(%s ! %s)"); }
    | formula[L] TOKEN_ONLY vartime[R] { $$ = create_operation(op_com_delay, $L, $R, "(%s ! %s)"); }
    | formula[L] TOKEN_ONLY interval[R] { $$ = create_operation(op_only, $L, $R, "(%s ! %s)"); }
    | formula[L] TOKEN_ONLY varinterval[R] { $$ = create_operation(op_com_only, $L, $R, "(%s ! %s)"); }
    ;

postime
    : TOKEN_PVALUE TOKEN_LPAREN varidxname[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_pvalue, $L, $R, "+$(%s , %s)"); }
    ;

negtime
    : TOKEN_NVALUE TOKEN_LPAREN varidxname[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_nvalue, $L, $R, "-$(%s , %s)"); }
    ;

vartime
    : postime[T] { $$ = $T; }
    | negtime[T] { $$ = $T; }
    ;

interval
    : TOKEN_LSQUARED expression[L] TOKEN_COMMA expression[R] TOKEN_RSQUARED { $$ = create_operation(op_interval_1, $L, $R, "[%s , %s]"); }
    | TOKEN_LPAREN expression[L] TOKEN_COMMA expression[R] TOKEN_RSQUARED { $$ = create_operation(op_interval_2, $L, $R, "(%s , %s]"); }
    | TOKEN_LSQUARED expression[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_interval_3, $L, $R, "[%s , %s)"); }
    | TOKEN_LPAREN expression[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_interval_4, $L, $R, "(%s , %s)"); }
    ;

varinterval
    : TOKEN_LSQUARED expression[L] TOKEN_COMMA postime[R] TOKEN_RSQUARED { $$ = create_operation(op_varinterval_1, $L, $R, "[%s , %s]"); }
    | TOKEN_LPAREN expression[L] TOKEN_COMMA postime[R] TOKEN_RSQUARED { $$ = create_operation(op_varinterval_2, $L, $R, "(%s , %s]"); }
    | TOKEN_LSQUARED negtime[L] TOKEN_COMMA expression[R] TOKEN_RSQUARED { $$ = create_operation(op_varinterval_3, $L, $R, "[%s , %s]"); }
    | TOKEN_LSQUARED negtime[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_varinterval_4, $L, $R, "[%s , %s)"); }
    | TOKEN_LSQUARED negtime[L] TOKEN_COMMA postime[R] TOKEN_RSQUARED { $$ = create_operation(op_varinterval_5, $L, $R, "[%s , %s]"); }
    ;

declaration
    : TOKEN_INPUT TOKEN_LSQUARED iodecllist[B] TOKEN_RSQUARED decllist[A] { $$ = create_operation(op_input, $A, $B, "input [%2$s] %1$s"); }
    | TOKEN_INPUT decllist[A] { $$ = create_operation(op_input, $A, create_operation(op_join_qual, create_operation(op_join_qual, create_operation(op_join_qual,
      create_ground(op_ioqual1, "", io_any, REAL_MAX), create_ground(op_ioqual2, "", io_binary, REAL_MAX), "%s , %s"), create_ground(op_ioqual3, "", io_unknown, REAL_MAX), "%s , %s"),
      create_ground(op_ioqual4, "", io_raw, REAL_MAX), "%s , %s"), "input [%2$s] %1$s"); }
    | TOKEN_OUTPUT TOKEN_LSQUARED iodecllist[B] TOKEN_RSQUARED decllist[A] { $$ = create_operation(op_output, $A, $B, "output [%2$s] %1$s"); }
    | TOKEN_OUTPUT decllist[A] { $$ = create_operation(op_output, $A, create_operation(op_join_qual, create_operation(op_join_qual, create_operation(op_join_qual,
      create_ground(op_ioqual1, "", io_any, REAL_MAX), create_ground(op_ioqual2, "", io_binary, REAL_MAX), "%s , %s"), create_ground(op_ioqual3, "", io_unknown, REAL_MAX), "%s , %s"),
      create_ground(op_ioqual4, "", io_raw, REAL_MAX), "%s , %s"), "output [%2$s] %1$s"); }
    | TOKEN_AUX decllist[A] { $$ = create_operation(op_aux, $A, NULL, "aux %s"); }
    | TOKEN_INIT eventlist[A] { $$ = create_operation(op_init, $A, NULL, "init %s"); }
    | TOKEN_DEFINE assignlist[A] { $$ = $A; }
    | TOKEN_INCLUDE filelist[A] { $$ = $A; }
    ;

iodecllist
    : iodecllist[L] TOKEN_COMMA iodecl[R] { $$ = create_operation(op_join_qual, $L, $R, "%s , %s"); }
    | iodecl[A] { $$ = $A; }
    ;

iodecl
    : TOKEN_ANY { $$ = create_ground(op_ioqual1, "any", io_any, REAL_MAX); }
    | TOKEN_IPC { $$ = create_ground(op_ioqual1, "ipc", io_ipc, REAL_MAX); }
    | TOKEN_FILE { $$ = create_ground(op_ioqual1, "file", io_file, REAL_MAX); }
    | TOKEN_REMOTE { $$ = create_ground(op_ioqual1, "remote", io_socket, REAL_MAX); }
    | TOKEN_BINARY { $$ = create_ground(op_ioqual2, "binary", io_binary, REAL_MAX); }
    | TOKEN_PACKED { $$ = create_ground(op_ioqual2, "packed", io_packed, REAL_MAX); }
    | TOKEN_UNKNOWN { $$ = create_ground(op_ioqual3, "unknown", io_unknown, REAL_MAX); }
    | TOKEN_FALSE { $$ = create_ground(op_ioqual3, "false", io_false, REAL_MAX); }
    | TOKEN_TRUE { $$ = create_ground(op_ioqual3, "true", io_true, REAL_MAX); }
    | TOKEN_DEFAULT TOKEN_EQUAL expression[E] { $$ = create_operation(op_ioqual3, $E, NULL, "default = %s"); }
    | TOKEN_RAW { $$ = create_ground(op_ioqual4, "raw", io_raw, REAL_MAX); }
    | TOKEN_FILTER { $$ = create_ground(op_ioqual4, "filter", io_filter, REAL_MAX); }
    | TOKEN_OMIT { $$ = create_ground(op_ioqual4, "omit", io_omit, REAL_MAX); }
    ;

decllist
    : decllist[L] TOKEN_COMMA declidxname[R] { $$ = create_operation(op_join, $L, $R, "%s , %s"); }
    | declidxname[A] { $$ = $A; }
    ;

eventlist
    : eventlist[L] TOKEN_COMMA eventname[R] { $$ = create_operation(op_join, $L, $R, "%s , %s"); }
    | eventname[A] { $$ = $A; }
    ;

filelist
    : filelist[L] TOKEN_COMMA filename[R] { $$ = create_operation(op_join, $L, $R, "%s , %s"); }
    | filename[A] { $$ = $A; }
    ;

assignlist
    : assignlist[L] TOKEN_COMMA assignment[R] { $$ = create_operation(op_join, $L, $R, "%s , %s"); }
    | assignment[A] { $$ = $A; }
    ;

globveclist
    : globveclist[L] TOKEN_COMMA globvector[R] { $$ = create_operation(op_join_array, $L, $R, "%s , %s"); }
    | globvector[G] { $$ = $G; }
    ;

globvector
    : TOKEN_LSQUARED globveclist[G] TOKEN_RSQUARED { $$ = create_operation(op_down, $G, NULL, "[%s]"); }
    | expression[E] { $$ = $E; }

assignment
   : constidxname[L] TOKEN_EQUAL expression[R] { $$ = create_operation(op_define, $L, $R, "define %s = %s"); }
   | constidxname[L] TOKEN_EQUAL TOKEN_LSQUARED globveclist[R] TOKEN_RSQUARED { $$ = create_operation(op_defarray, $L, $R, "define %s = [%s]"); }
   ;

eventname
    : qualname[L] TOKEN_AT expression[R] { $$ = create_operation(op_var_at, $L, $R, "%s @ %s"); }
    | qualname[L] TOKEN_AT interval[R] { $$ = create_operation(op_var_at, $L, $R, "%s @ %s"); }
    | initmathidxname[L] TOKEN_AT expression[M] TOKEN_EQUAL expression[R] { $$ = create_operation(op_var_at, create_operation(op_assign, $L, $R, "%s = %s"), $M, "%s @ %s"); }
    | initmathidxname[L] TOKEN_AT interval[M] TOKEN_EQUAL expression[R] { $$ = create_operation(op_var_at, create_operation(op_assign, $L, $R, "%s = %s"), $M, "%s @ %s"); }
    ;

qualname
    : TOKEN_NOT initidxname[A] { $$ = create_operation(op_not, $A, NULL, "~ %s"); }
    | initidxname[A] { $$ = $A; } 
    ;

varidxname
    : indexedname[A] { ($A)->left->ot = op_name; $$ = $A; }
    | name[A] { ($A)->ot = op_name; $$ = $A; }
    ;

indexedname
    : name[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M1] TOKEN_COMMA expression[M2] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array4, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M1, "%s , %s"), create_operation(op_interval_1, $M2, $R, "%s , %s"), "%s , %s"), "%s(%s)"); }
    | name[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array3, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M, "%s , %s"), create_operation(op_interval_1, $R,
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
    | name[L0] TOKEN_LPAREN expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, $L0,
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | name[L0] TOKEN_LPAREN expression[R] TOKEN_RPAREN { $$ = create_operation(op_vector, $L0, $R, "%s(%s)"); }
    ;

mathidxname
    : mathname[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M1] TOKEN_COMMA expression[M2] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array4, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M1, "%s , %s"), create_operation(op_interval_1, $M2, $R, "%s , %s"), "%s , %s"), "%s(%s)"); }
    | mathname[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array3, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M, "%s , %s"), create_operation(op_interval_1, $R,
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
    | mathname[L0] TOKEN_LPAREN expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, $L0,
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | mathname[L0] TOKEN_LPAREN expression[R] TOKEN_RPAREN { $$ = create_operation(op_vector, $L0, $R, "%s(%s)"); }
    | mathname[A] { $$ = $A; }
    ;

declidxname
    : declname[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M1] TOKEN_COMMA expression[M2] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array4, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M1, "%s , %s"), create_operation(op_interval_1, $M2, $R, "%s , %s"), "%s , %s"), "%s(%s)"); }
    | declname[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array3, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M, "%s , %s"), create_operation(op_interval_1, $R,
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
    | declname[L0] TOKEN_LPAREN expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, $L0,
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | declname[L0] TOKEN_LPAREN expression[R] TOKEN_RPAREN { $$ = create_operation(op_vector, $L0, $R, "%s(%s)"); }
    | declname[A] { $$ = $A; }
    ;

initidxname
    : initname[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M1] TOKEN_COMMA expression[M2] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array4, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M1, "%s , %s"), create_operation(op_interval_1, $M2, $R, "%s , %s"), "%s , %s"), "%s(%s)"); }
    | initname[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array3, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M, "%s , %s"), create_operation(op_interval_1, $R,
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
    | initname[L0] TOKEN_LPAREN expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, $L0,
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | initname[L0] TOKEN_LPAREN expression[R] TOKEN_RPAREN { $$ = create_operation(op_vector, $L0, $R, "%s(%s)"); }
    | initname[A] { $$ = $A; }
    ;

initmathidxname
    : initmathname[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M1] TOKEN_COMMA expression[M2] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array4, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M1, "%s , %s"), create_operation(op_interval_1, $M2, $R, "%s , %s"), "%s , %s"), "%s(%s)"); }
    | initmathname[L0] TOKEN_LPAREN expression[L] TOKEN_COMMA expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_array3, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M, "%s , %s"), create_operation(op_interval_1, $R,
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
    | initmathname[L0] TOKEN_LPAREN expression[M] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, $L0,
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | initmathname[L0] TOKEN_LPAREN expression[R] TOKEN_RPAREN { $$ = create_operation(op_vector, $L0, $R, "%s(%s)"); }
    | initmathname[A] { $$ = $A; }
    ;

constidxname
    : constname[L0] TOKEN_LPAREN exprorrange[L] TOKEN_COMMA exprorrange[M1] TOKEN_COMMA exprorrange[M2] TOKEN_COMMA exprorrange[R] TOKEN_RPAREN { $$ = create_operation(op_array4, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M1, "%s , %s"), create_operation(op_interval_1, $M2, $R, "%s , %s"), "%s , %s"), "%s(%s)"); }
    | constname[L0] TOKEN_LPAREN exprorrange[L] TOKEN_COMMA exprorrange[M] TOKEN_COMMA exprorrange[R] TOKEN_RPAREN { $$ = create_operation(op_array3, $L0,
      create_operation(op_fourdim, create_operation(op_interval_1, $L, $M, "%s , %s"), create_operation(op_interval_1, $R,
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
    | constname[L0] TOKEN_LPAREN exprorrange[M] TOKEN_COMMA exprorrange[R] TOKEN_RPAREN { $$ = create_operation(op_matrix, $L0,
      create_operation(op_interval_1, $M, $R, "%s , %s"), "%s(%s)"); }
    | constname[L0] TOKEN_LPAREN exprorrange[R] TOKEN_RPAREN { $$ = create_operation(op_vector, $L0, $R, "%s(%s)"); }
    | constname[A] { $$ = $A; }
    ;

exprorrange
    : expression[E] { $$ = $E; }
    | TOKEN_UNDERSCORE { $$ = create_ground(op_range, "_", NULL_TIME, REAL_MAX); }
    ;

name
    : TOKEN_RICHNAME { $$ = create_ground(op_constant, richwrap($1), NULL_TIME, REAL_MAX); }
    | TOKEN_NAME { $$ = create_ground(op_constant, $1, NULL_TIME, REAL_MAX); }
    ;

mathname
    : TOKEN_REALRICH { $$ = create_ground(op_variable, richwrap2($1), NULL_TIME, REAL_MAX); }
    | TOKEN_REALNAME { $$ = create_ground(op_variable, $1, NULL_TIME, REAL_MAX); }
    ;

declname
    : TOKEN_RICHNAME { $$ = create_ground(op_dname, richwrap($1), NULL_TIME, REAL_MAX); }
    | TOKEN_NAME { $$ = create_ground(op_dname, $1, NULL_TIME, REAL_MAX); }
    | TOKEN_REALRICH { $$ = create_ground(op_dvariable, richwrap2($1), NULL_TIME, REAL_MAX); }
    | TOKEN_REALNAME { $$ = create_ground(op_dvariable, $1, NULL_TIME, REAL_MAX); }
    ;

initname
    : TOKEN_RICHNAME { $$ = create_ground(op_iname, richwrap($1), NULL_TIME, REAL_MAX); }
    | TOKEN_NAME { $$ = create_ground(op_iname, $1, NULL_TIME, REAL_MAX); }
    ;

initmathname
    : TOKEN_REALRICH { $$ = create_ground(op_ivariable, richwrap2($1), NULL_TIME, REAL_MAX); }
    | TOKEN_REALNAME { $$ = create_ground(op_ivariable, $1, NULL_TIME, REAL_MAX); }
    ;

constname
    : TOKEN_NAME { $$ = create_ground(op_cname, $1, NULL_TIME, REAL_MAX); }
    ;

filename
    : TOKEN_STRING { $$ = create_ground(op_fname, $1, NULL_TIME, REAL_MAX); }
    ;

string
    : TOKEN_STRING { $$ = create_ground(op_string, $1, NULL_TIME, REAL_MAX); }
    ;

varrange
    : constidxname[L] TOKEN_IN expression[M] TOKEN_COLON expression[R] { $$ = create_operation(op_in, $L, create_operation(op_interval_1, $M, $R, "%s : %s"), "%s in %s"); }
    | constidxname[L] TOKEN_ON expression[R]  { $$ = create_operation(op_in, $L, create_operation(op_interval_3, create_ground(op_number, "", 0, REAL_MAX), $R, "%s : %s - 1"), "%s on %s"); }
    | expression[L] TOKEN_COLON expression[R] { $$ = create_operation(op_interval_1, $L, $R, "%s : %s"); }
    | expression[E] { $$ = create_operation(op_interval_3, create_ground(op_number, "", 0, REAL_MAX), $E, "%s : %s - 1"); }
    ;

selrange
    : TOKEN_NEG selrange[Z] { $$ = create_operation(op_neg_range, $Z, NULL, "not %s"); }
    | expression[L] TOKEN_IN exprrange[R] { $$ = create_operation(op_in, $L, $R, "%s in %s"); }
    | expression[L] TOKEN_IS expression[R]  { $$ = create_operation(op_in, $L, create_operation(op_interval_1, $R, copy_specification($R), "%s : %s"), "%s is %s"); }
    | exprrange[E] { $$ = $E; }
    | expression[E] { $$ = create_operation(op_interval_1, $E, copy_specification($E), "%s : %s"); }
    ;

exprrange
    : expression[L] TOKEN_COLON expression[R] { $$ = create_operation(op_interval_1, $L, $R, "%s : %s"); }
    | TOKEN_UNDERSCORE TOKEN_COLON expression[R] { $$ = create_operation(op_interval_1, create_ground(op_number, "", LONG_MIN, REAL_MAX), $R, "_ : %s"); }
    | expression[L] TOKEN_COLON TOKEN_UNDERSCORE { $$ = create_operation(op_interval_1, $L, create_ground(op_number, "", LONG_MAX, REAL_MAX), "%s : _"); }
    ;

mathrelation
    : mathexpression[L] TOKEN_EQUAL mathexpression[R] { $$ = create_operation(op_equal, $L, $R, "(%s = %s)"); }
    | mathexpression[L] TOKEN_NEQ mathexpression[R] { $$ = create_operation(op_neq, $L, $R, "(%s ~= %s)"); }
    | mathexpression[L] TOKEN_LT mathexpression[R] { $$ = create_operation(op_lt, $L, $R, "(%s < %s)"); }
    | mathexpression[L] TOKEN_GT mathexpression[R] { $$ = create_operation(op_gt, $L, $R, "(%s > %s)"); }
    | mathexpression[L] TOKEN_LTEQ mathexpression[R] { $$ = create_operation(op_lteq, $L, $R, "(%s <= %s)"); }
    | mathexpression[L] TOKEN_GTEQ mathexpression[R] { $$ = create_operation(op_gteq, $L, $R, "(%s >= %s)"); }
    | mathexpression[L] TOKEN_EQUAL expression[R] { $$ = create_operation(op_equal, $L, $R, "(%s = %s)"); }
    | mathexpression[L] TOKEN_NEQ expression[R] { $$ = create_operation(op_neq, $L, $R, "(%s ~= %s)"); }
    | mathexpression[L] TOKEN_LT expression[R] { $$ = create_operation(op_lt, $L, $R, "(%s < %s)"); }
    | mathexpression[L] TOKEN_GT expression[R] { $$ = create_operation(op_gt, $L, $R, "(%s > %s)"); }
    | mathexpression[L] TOKEN_LTEQ expression[R] { $$ = create_operation(op_lteq, $L, $R, "(%s <= %s)"); }
    | mathexpression[L] TOKEN_GTEQ expression[R] { $$ = create_operation(op_gteq, $L, $R, "(%s >= %s)"); }
    | expression[L] TOKEN_EQUAL mathexpression[R] { $$ = create_operation(op_equal, $L, $R, "(%s = %s)"); }
    | expression[L] TOKEN_NEQ mathexpression[R] { $$ = create_operation(op_neq, $L, $R, "(%s ~= %s)"); }
    | expression[L] TOKEN_LT mathexpression[R] { $$ = create_operation(op_lt, $L, $R, "(%s < %s)"); }
    | expression[L] TOKEN_GT mathexpression[R] { $$ = create_operation(op_gt, $L, $R, "(%s > %s)"); }
    | expression[L] TOKEN_LTEQ mathexpression[R] { $$ = create_operation(op_lteq, $L, $R, "(%s <= %s)"); }
    | expression[L] TOKEN_GTEQ mathexpression[R] { $$ = create_operation(op_gteq, $L, $R, "(%s >= %s)"); }
    ;

mathexpression
    : mathexpression[L] TOKEN_PLUS mathexprmul[R] { $$ = create_operation(op_plus, $L, $R, "(%s + %s)"); }
    | mathexpression[L] TOKEN_MINUS mathexprmul[R] { $$ = create_operation(op_minus, $L, $R, "(%s - %s)"); }
    | mathexpression[L] TOKEN_PLUS exprmul[R] { $$ = create_operation(op_plus, $L, $R, "(%s + %s)"); }
    | mathexpression[L] TOKEN_MINUS exprmul[R] { $$ = create_operation(op_minus, $L, $R, "(%s - %s)"); }
    | expression[L] TOKEN_PLUS mathexprmul[R] { $$ = create_operation(op_plus, $L, $R, "(%s + %s)"); }
    | expression[L] TOKEN_MINUS mathexprmul[R] { $$ = create_operation(op_minus, $L, $R, "(%s - %s)"); }
    | mathexprmul[E] { $$ = $E; }
    ;

mathexprmul
    : mathexprmul[L] TOKEN_MUL mathexprpow[R] { $$ = create_operation(op_mul, $L, $R, "(%s * %s)"); }
    | mathexprmul[L] TOKEN_DIV mathexprpow[R] { $$ = create_operation(op_div, $L, $R, "(%s / %s)"); }
    | mathexprmul[L] TOKEN_MOD mathexprpow[R] { $$ = create_operation(op_mod, $L, $R, "(%s %% %s)"); }
    | mathexprmul[L] TOKEN_MUL exprpow[R] { $$ = create_operation(op_mul, $L, $R, "(%s * %s)"); }
    | mathexprmul[L] TOKEN_DIV exprpow[R] { $$ = create_operation(op_div, $L, $R, "(%s / %s)"); }
    | mathexprmul[L] TOKEN_MOD exprpow[R] { $$ = create_operation(op_mod, $L, $R, "(%s %% %s)"); }
    | exprmul[L] TOKEN_MUL mathexprpow[R] { $$ = create_operation(op_mul, $L, $R, "(%s * %s)"); }
    | exprmul[L] TOKEN_DIV mathexprpow[R] { $$ = create_operation(op_div, $L, $R, "(%s / %s)"); }
    | exprmul[L] TOKEN_MOD mathexprpow[R] { $$ = create_operation(op_mod, $L, $R, "(%s %% %s)"); }
    | mathexprpow[E] { $$ = $E; }
    ;

mathexprpow
    : mathexprpow[L] TOKEN_POW mathexprroot[R] { $$ = create_operation(op_pow, $L, $R, "(%s ^ %s)"); }
    | mathexprpow[L] TOKEN_POW exprroot[R] { $$ = create_operation(op_pow, $L, $R, "(%s ^ %s)"); }
    | exprpow[L] TOKEN_POW mathexprroot[R] { $$ = create_operation(op_pow, $L, $R, "(%s ^ %s)"); }
    | mathexprroot[E] { $$ = $E; }
    ;

mathexprroot
    : TOKEN_ROOT TOKEN_LPAREN mathexpression[L] TOKEN_COMMA mathexpression[R] TOKEN_RPAREN { $$ = create_operation(op_root, $L, $R, "root(%s , %s)"); }
    | TOKEN_LOG TOKEN_LPAREN mathexpression[L] TOKEN_COMMA mathexpression[R] TOKEN_RPAREN { $$ = create_operation(op_log, $L, $R, "log(%s , %s)"); }
    | TOKEN_ROOT TOKEN_LPAREN mathexpression[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_root, $L, $R, "root(%s , %s)"); }
    | TOKEN_LOG TOKEN_LPAREN mathexpression[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_log, $L, $R, "log(%s , %s)"); }
    | TOKEN_ROOT TOKEN_LPAREN expression[L] TOKEN_COMMA mathexpression[R] TOKEN_RPAREN { $$ = create_operation(op_root, $L, $R, "root(%s , %s)"); }
    | TOKEN_LOG TOKEN_LPAREN expression[L] TOKEN_COMMA mathexpression[R] TOKEN_RPAREN { $$ = create_operation(op_log, $L, $R, "log(%s , %s)"); }
    | TOKEN_SIN TOKEN_LPAREN mathexpression[L] TOKEN_RPAREN { $$ = create_operation(op_sin, $L, NULL, "sin(%s)"); }
    | TOKEN_COS TOKEN_LPAREN mathexpression[L] TOKEN_RPAREN { $$ = create_operation(op_cos, $L, NULL, "cos(%s)"); }
    | TOKEN_TAN TOKEN_LPAREN mathexpression[L] TOKEN_RPAREN { $$ = create_operation(op_tan, $L, NULL, "tan(%s)"); }
    | TOKEN_ASIN TOKEN_LPAREN mathexpression[L] TOKEN_RPAREN { $$ = create_operation(op_asin, $L, NULL, "asin(%s)"); }
    | TOKEN_ACOS TOKEN_LPAREN mathexpression[L] TOKEN_RPAREN { $$ = create_operation(op_acos, $L, NULL, "acos(%s)"); }
    | TOKEN_ATAN TOKEN_LPAREN mathexpression[L] TOKEN_RPAREN { $$ = create_operation(op_atan, $L, NULL, "atan(%s)"); }
    | TOKEN_SUM TOKEN_LPAREN mathexpression[L] TOKEN_COMMA varrange[R] TOKEN_RPAREN  { $$ = create_operation(op_sum, $L, $R, "sum(%s, %s)"); }
    | TOKEN_PROD TOKEN_LPAREN mathexpression[L] TOKEN_COMMA varrange[R] TOKEN_RPAREN  { $$ = create_operation(op_prod, $L, $R, "prod(%s, %s)"); }
    | mathexprchs[E] { $$ = $E; }
    ;

mathexprchs
    : TOKEN_MINUS mathexprdelay[E] { $$ = create_operation(op_chs, $E, NULL, "- (%s)"); }
    | mathexprdelay[E] { $$ = $E; }
    ;

mathexprdelay
    : mathexprgnd[L] TOKEN_AT exprroot[R] { $$ = create_operation(op_math_delay, $L, $R, "(%s @ %s)"); }
    | mathexprgnd[E] { $$ = $E; }
    ;

mathexprgnd
    : TOKEN_LPAREN mathexpression[E] TOKEN_RPAREN { $$ = $E; }
    | mathidxname[A] { $$ = $A; }
    ;

expression
    : expression[L] TOKEN_PLUS exprmul[R] { $$ = create_operation(op_plus, $L, $R, "(%s + %s)"); }
    | expression[L] TOKEN_MINUS exprmul[R] { $$ = create_operation(op_minus, $L, $R, "(%s - %s)"); }
    | exprmul[E] { $$ = $E; }
    ;

exprmul
    : exprmul[L] TOKEN_MUL exprpow[R] { $$ = create_operation(op_mul, $L, $R, "(%s * %s)"); }
    | exprmul[L] TOKEN_DIV exprpow[R] { $$ = create_operation(op_div, $L, $R, "(%s / %s)"); }
    | exprmul[L] TOKEN_MOD exprpow[R] { $$ = create_operation(op_mod, $L, $R, "(%s %% %s)"); }
    | exprpow[E] { $$ = $E; }
    ;

exprpow
    : exprpow[L] TOKEN_POW exprroot[R] { $$ = create_operation(op_pow, $L, $R, "(%s ^ %s)"); }
    | exprroot[E] { $$ = $E; }
    ;

exprroot
    : TOKEN_ROOT TOKEN_LPAREN expression[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_root, $L, $R, "root(%s , %s)"); }
    | TOKEN_LOG TOKEN_LPAREN expression[L] TOKEN_COMMA expression[R] TOKEN_RPAREN { $$ = create_operation(op_log, $L, $R, "log(%s , %s)"); }
    | TOKEN_SIN TOKEN_LPAREN expression[L] TOKEN_RPAREN { $$ = create_operation(op_sin, $L, NULL, "sin(%s)"); }
    | TOKEN_COS TOKEN_LPAREN expression[L] TOKEN_RPAREN { $$ = create_operation(op_cos, $L, NULL, "cos(%s)"); }
    | TOKEN_TAN TOKEN_LPAREN expression[L] TOKEN_RPAREN { $$ = create_operation(op_tan, $L, NULL, "tan(%s)"); }
    | TOKEN_ASIN TOKEN_LPAREN expression[L] TOKEN_RPAREN { $$ = create_operation(op_asin, $L, NULL, "asin(%s)"); }
    | TOKEN_ACOS TOKEN_LPAREN expression[L] TOKEN_RPAREN { $$ = create_operation(op_acos, $L, NULL, "acos(%s)"); }
    | TOKEN_ATAN TOKEN_LPAREN expression[L] TOKEN_RPAREN { $$ = create_operation(op_atan, $L, NULL, "atan(%s)"); }
    | TOKEN_SUM TOKEN_LPAREN expression[L] TOKEN_COMMA varrange[R] TOKEN_RPAREN  { $$ = create_operation(op_sum, $L, $R, "sum(%s, %s)"); }
    | TOKEN_PROD TOKEN_LPAREN expression[L] TOKEN_COMMA varrange[R] TOKEN_RPAREN  { $$ = create_operation(op_prod, $L, $R, "prod(%s, %s)"); }
    | exprchs[E] { $$ = $E; }
    ;

exprchs
    : TOKEN_MINUS exprgnd[E] { $$ = create_operation(op_chs, $E, NULL, "- (%s)"); }
    | exprgnd[E] { $$ = $E; }
    ;

exprgnd
    : TOKEN_LPAREN expression[E] TOKEN_RPAREN { $$ = $E; }
    | nameornum[X] { $$ = $X; }
    ;

nameornum
    : indexedname[A] { $$ = $A; }
    | name[A] { $$ = $A; }
    | TOKEN_ITERATOR { $$ = create_ground(op_iterator, $1, NULL_TIME, REAL_MAX); }
    | realnumber[X] { $$ = $X; }
    | number[N] { $$ = $N; }
    ;

realnumber
    : TOKEN_REAL { $$ = create_ground(op_real, "", NULL_TIME, $1); }
    | TOKEN_KE { $$ = create_ground(op_real, "", NULL_TIME, M_E); }
    | TOKEN_KPI { $$ = create_ground(op_real, "", NULL_TIME, M_PI); }    
    ;

number
    : TOKEN_NUMBER { $$ = create_ground(op_number, "", $1, REAL_MAX); }
    ;

%%


