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
SINCE       "since"
UNTIL       "until"
FORALL      "forall"
EXISTS      "exists"
ONE         "one"
UNIQUE      "unique"
IMPLY       "-->"
INVIMPLY    "<--"
EQV         "=="
CODE        "code"

EQUAL       "="
PLUS        "+"
MINUS       "-"
MUL         "*"
DIV         "/"

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
BINARY      "binary"
PACKED      "packed"
UNKNOWN     "unknown"
FALSE       "false"
TRUE        "true"
OMIT        "omit"

LBRACKET    "{"
RBRACKET    "}"
SEMICOLON   ";"

NAME        [a-zA-Z][a-zA-Z0-9_]*
STRING      "\""[^\"]+"\""
ITERATOR    "^"+
NUMBER      [0-9]+
BLANKS      [ \r\n\t]*

%%

"//".*         {  }
"/*"([^*]|("*"[^/]))*"*/" {  }

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
{INVIMPLY}      { return TOKEN_INVIMPLY; }
{EQV}           { return TOKEN_EQV; }
{CODE}          { return TOKEN_CODE; }
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
{WHEN}          { return TOKEN_WHEN; }
{ELSE}          { return TOKEN_ELSE; }
{ON}            { return TOKEN_ON; }
{IN}            { return TOKEN_IN; }
{IS}            { return TOKEN_IS; }
{NEG}           { return TOKEN_NEG; }
{ANY}           { return TOKEN_ANY; }
{IPC}           { return TOKEN_IPC; }
{FILE}          { return TOKEN_FILE; }
{BINARY}        { return TOKEN_BINARY; }
{PACKED}        { return TOKEN_PACKED; }
{UNKNOWN}       { return TOKEN_UNKNOWN; }
{FALSE}         { return TOKEN_FALSE; }
{TRUE}          { return TOKEN_TRUE; }
{OMIT}          { return TOKEN_OMIT; }
{COMMA}         { return TOKEN_COMMA; }
{COLON}         { return TOKEN_COLON; }
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
{LSQUARED}      { return TOKEN_LSQUARED; }
{RSQUARED}      { return TOKEN_RSQUARED; }
{LBRACKET}      { return TOKEN_LBRACKET; }
{RBRACKET}      { return TOKEN_RBRACKET; }
{SEMICOLON}     { return TOKEN_SEMICOLON; }

{NAME}          { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_NAME; }
{STRING}        { sscanf(yytext, STRING_FMT, yylval->symbol); return TOKEN_STRING; }
{ITERATOR}      { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_ITERATOR; }
{NUMBER}        { sscanf(yytext, TIME_FMT, &yylval->value); return TOKEN_NUMBER; }
{BLANKS}        {  }
.               { yyerror(yylloc, NULL, yyg, "unknown keyword"); exit(EXIT_FAILURE); }

%%

