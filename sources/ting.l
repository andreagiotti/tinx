%{

#include "ting_core.h"
#include "ting_parser.h"

#define YY_USER_ACTION tr.token[tr.idx] = strdup(yytext); \
                       tr.line[tr.idx] = yylineno; \
                       tr.column[tr.idx] = yycolumn; \
                       tr.length[tr.idx] = yyleng; \
                       yyset_extra(tr, yyg); \
                       tr.idx = (tr.idx + 1) % NUM_TOKENS; \
                       yycolumn += yyleng;

tracker tr;

int yyerror(YYLTYPE *yylloc, btl_specification **spec_handle, yyscan_t scanner, const char *msg);

%}

%option outfile="ting_lexer.c" header-file="ting_lexer.h"
%option warn nodefault
 
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge
%option extra-type = "tracker"

%option yylineno bison-locations noinput nounput

NOT         "~"
AND         "&"
OR          "|"
AT          "@"
HAPPEN      "?"
ONLY        "!"
SINCE       "since"
UNTIL       "until"
FORALL      "forall"
EXISTS      "exists"
ONE         "one"
UNIQUE      "unique"
COMBINE     "combine"
XOR         "\\"
IMPLY       "-->"
INVIMPLY    "<--"
EQV         "=="
CODE        "code"
PVALUE      "+$"
NVALUE      "-$"

EQUAL       "="
NEQ         "~="
LT          "<"
GT          ">"
LTEQ        "<="
GTEQ        ">="
PLUS        "+"
MINUS       "-"
MUL         "*"
DIV         "/"
MOD         "%"
POW         "^"
ROOT        "root"
LOG         "log"
SIN         "sin"
COS         "cos"
TAN         "tan"
ASIN        "asin"
ACOS        "acos"
ATAN        "atan"
SUM         "sum"
PROD        "prod"
KE          "Ke"
KPI         "Kpi"

LPAREN      "("
RPAREN      ")"
LSQUARED    "["
RSQUARED    "]"
COMMA       ","
COLON       ":"

INPUT       "input"
OUTPUT      "output"
AUX         "aux"
INIT        "init"
DEFINE      "define"
INCLUDE     "include"
ITER        "iter"
WHEN        "when"
ELSE        "else"

ON          "on"
IN          "in"
IS          "is"
NEG         "not"

ANY         "any"
IPC         "ipc"
FILE        "file"
REMOTE      "remote"
BINARY      "binary"
PACKED      "packed"
UNKNOWN     "unknown"
FALSE       "false"
TRUE        "true"
RAW         "raw"
FILTER      "filter"
OMIT        "omit"
DEFAULT     "default"

LBRACKET    "{"
RBRACKET    "}"
SEMICOLON   ";"
UNDERSCORE  "_"

NAME        [a-z][a-zA-Z0-9_]*
REALNAME    [A-Z][a-zA-Z0-9_]*
RICHNAME    "`"[^)(`]+"`"
REALRICH    "``"[^)(`]+"``"
STRING      "\""[^\"]+"\""
ITERATOR    "#"+
NUMBER      [-+]?[0-9]+
REAL        [-+]?[0-9]*\.[0-9]+
BLANKS      [ \r\n\t]*

%%

"//".*         {  }
"/*"([^*]|("*"[^/]))*"*/" {  }

{NOT}           { return TOKEN_NOT; }
{AND}           { return TOKEN_AND; }
{OR}            { return TOKEN_OR; }
{XOR}           { return TOKEN_XOR; }
{AT}            { return TOKEN_AT; }
{HAPPEN}        { return TOKEN_HAPPEN; }
{ONLY}          { return TOKEN_ONLY; }
{SINCE}         { return TOKEN_SINCE; }
{UNTIL}         { return TOKEN_UNTIL; }
{FORALL}        { return TOKEN_FORALL; }
{EXISTS}        { return TOKEN_EXISTS; }
{ONE}           { return TOKEN_ONE; }
{UNIQUE}        { return TOKEN_UNIQUE; }
{COMBINE}       { return TOKEN_COMBINE; }
{IMPLY}         { return TOKEN_IMPLY; }
{INVIMPLY}      { return TOKEN_INVIMPLY; }
{EQV}           { return TOKEN_EQV; }
{CODE}          { return TOKEN_CODE; }
{PVALUE}        { return TOKEN_PVALUE; }
{NVALUE}        { return TOKEN_NVALUE; }
{EQUAL}         { return TOKEN_EQUAL; }
{NEQ}           { return TOKEN_NEQ; }
{LT}            { return TOKEN_LT; }
{GT}            { return TOKEN_GT; }
{LTEQ}          { return TOKEN_LTEQ; }
{GTEQ}          { return TOKEN_GTEQ; }
{PLUS}          { return TOKEN_PLUS; }
{MINUS}         { return TOKEN_MINUS; }
{MUL}           { return TOKEN_MUL; }
{DIV}           { return TOKEN_DIV; }
{MOD}           { return TOKEN_MOD; }
{POW}           { return TOKEN_POW; }
{ROOT}          { return TOKEN_ROOT; }
{LOG}           { return TOKEN_LOG; }
{SIN}           { return TOKEN_SIN; }
{COS}           { return TOKEN_COS; }
{TAN}           { return TOKEN_TAN; }
{ASIN}          { return TOKEN_ASIN; }
{ACOS}          { return TOKEN_ACOS; }
{ATAN}          { return TOKEN_ATAN; }
{SUM}           { return TOKEN_SUM; }
{PROD}          { return TOKEN_PROD; }
{KE}            { return TOKEN_KE; }
{KPI}           { return TOKEN_KPI; }
{INPUT}         { return TOKEN_INPUT; }
{OUTPUT}        { return TOKEN_OUTPUT; }
{AUX}           { return TOKEN_AUX; }
{INIT}          { return TOKEN_INIT; }
{DEFINE}        { return TOKEN_DEFINE; }
{INCLUDE}       { return TOKEN_INCLUDE; }
{ITER}          { return TOKEN_ITER; }
{WHEN}          { return TOKEN_WHEN; }
{ELSE}          { return TOKEN_ELSE; }
{ON}            { return TOKEN_ON; }
{IN}            { return TOKEN_IN; }
{IS}            { return TOKEN_IS; }
{NEG}           { return TOKEN_NEG; }
{ANY}           { return TOKEN_ANY; }
{IPC}           { return TOKEN_IPC; }
{FILE}          { return TOKEN_FILE; }
{REMOTE}        { return TOKEN_REMOTE; }
{BINARY}        { return TOKEN_BINARY; }
{PACKED}        { return TOKEN_PACKED; }
{UNKNOWN}       { return TOKEN_UNKNOWN; }
{FALSE}         { return TOKEN_FALSE; }
{TRUE}          { return TOKEN_TRUE; }
{RAW}           { return TOKEN_RAW; }
{FILTER}        { return TOKEN_FILTER; }
{OMIT}          { return TOKEN_OMIT; }
{DEFAULT}       { return TOKEN_DEFAULT; }
{COMMA}         { return TOKEN_COMMA; }
{COLON}         { return TOKEN_COLON; }
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
{LSQUARED}      { return TOKEN_LSQUARED; }
{RSQUARED}      { return TOKEN_RSQUARED; }
{LBRACKET}      { return TOKEN_LBRACKET; }
{RBRACKET}      { return TOKEN_RBRACKET; }
{SEMICOLON}     { return TOKEN_SEMICOLON; }
{UNDERSCORE}    { return TOKEN_UNDERSCORE; }

{NAME}          { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_NAME; }
{RICHNAME}      { sscanf(yytext, RICHNAME_FMT, yylval->symbol); return TOKEN_RICHNAME; }
{REALNAME}      { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_REALNAME; }
{REALRICH}      { sscanf(yytext, REALRICH_FMT, yylval->symbol); return TOKEN_REALRICH; }
{STRING}        { sscanf(yytext, STRING_FMT, yylval->symbol); return TOKEN_STRING; }
{ITERATOR}      { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_ITERATOR; }
{NUMBER}        { sscanf(yytext, TIME_FMT, &yylval->value); return TOKEN_NUMBER; }
{REAL}          { sscanf(yytext, REAL_IN_FMT, &yylval->realval); return TOKEN_REAL; }
{BLANKS}        {  }
.               { yyerror(yylloc, NULL, yyg, "unknown keyword"); exit(EXIT_FAILURE); }

%%


