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

EQUAL       "="
PLUS        "+"
MINUS       "-"
MUL         "*"
DIV         "/"

NOT         "~"
AND         "&"
OR          "|"
AT          "@"
HAPPEN      "?"
SINCE       "since"
UNTIL       "until"
FORALL      "forall"
EXISTS      "exists"
ONE         "one"
UNIQUE      "unique"
IMPLY       "-->"
EQV         "=="

LPAREN      "("
RPAREN      ")"
LSQUARED    "["
RSQUARED    "]"
COMMA       ","

INPUT       "input"
OUTPUT      "output"
AUX         "aux"
INIT        "init"
DEFINE      "define"
INCLUDE     "include"
ITER        "iter"

ANY         "any"
IPC         "ipc"
FILE        "file"

LBRACKET    "{"
RBRACKET    "}"
SEMICOLON   ";"

NAME        [a-zA-Z][a-zA-Z0-9_]*
FILENAME    "\""[^\"]+"\""
ITERATOR    "^"+
NUMBER      [0-9]+
BLANKS      [ \r\n\t]*

%%

^"//".*         {  }
^"/*"[^*]*"*"+([^*/][^*]*"*"+)*"/" {  }

{NOT}           { return TOKEN_NOT; }
{AND}           { return TOKEN_AND; }
{OR}            { return TOKEN_OR; }
{AT}            { return TOKEN_AT; }
{HAPPEN}        { return TOKEN_HAPPEN; }
{SINCE}         { return TOKEN_SINCE; }
{UNTIL}         { return TOKEN_UNTIL; }
{FORALL}        { return TOKEN_FORALL; }
{EXISTS}        { return TOKEN_EXISTS; }
{ONE}           { return TOKEN_ONE; }
{UNIQUE}        { return TOKEN_UNIQUE; }
{IMPLY}         { return TOKEN_IMPLY; }
{EQV}           { return TOKEN_EQV; }
{EQUAL}         { return TOKEN_EQUAL; }
{PLUS}          { return TOKEN_PLUS; }
{MINUS}         { return TOKEN_MINUS; }
{MUL}           { return TOKEN_MUL; }
{DIV}           { return TOKEN_DIV; }
{INPUT}         { return TOKEN_INPUT; }
{OUTPUT}        { return TOKEN_OUTPUT; }
{AUX}           { return TOKEN_AUX; }
{INIT}          { return TOKEN_INIT; }
{DEFINE}        { return TOKEN_DEFINE; }
{INCLUDE}       { return TOKEN_INCLUDE; }
{ITER}          { return TOKEN_ITER; }
{ANY}           { return TOKEN_ANY; }
{IPC}           { return TOKEN_IPC; }
{FILE}          { return TOKEN_FILE; }
{COMMA}         { return TOKEN_COMMA; }
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
{LSQUARED}      { return TOKEN_LSQUARED; }
{RSQUARED}      { return TOKEN_RSQUARED; }
{SEMICOLON}     { return TOKEN_SEMICOLON; }
{LBRACKET}      { return TOKEN_LBRACKET; }
{RBRACKET}      { return TOKEN_RBRACKET; }

{NAME}          { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_NAME; }
{FILENAME}      { sscanf(yytext, FILENAME_FMT, yylval->symbol); return TOKEN_FILENAME; }
{ITERATOR}      { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_ITERATOR; }
{NUMBER}        { sscanf(yytext, TIME_FMT, &yylval->value); return TOKEN_NUMBER; }
{BLANKS}        {  }
.               { yyerror(yylloc, NULL, yyg, "unknown keyword"); exit(EXIT_FAILURE); }

%%

