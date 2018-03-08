%{

#include "ting_core.h"
#include "ting_parser.h"

#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno + 1; \
                       yylloc->first_column = yycolumn; \
                       yylloc->last_column = yycolumn + yyleng - 1; \
                       yycolumn += yyleng;

int yyerror(YYLTYPE *yylloc, btl_specification **spec_handle, yyscan_t scanner, const char *msg);

%}

%option outfile="ting_lexer.c" header-file="ting_lexer.h"
%option warn nodefault
 
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

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
LSQUAR      "["
RSQUAR      "]"
COMMA       ","

INPUT       "input"
OUTPUT      "output"
AUX         "aux"
INIT        "init"
DEFINE      "define"
ITER        "iter"

LBRA        "{"
RBRA        "}"
SEMICOLON   ";"

NAME        [a-zA-Z][a-zA-Z0-9_]*
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
{ITER}          { return TOKEN_ITER; }

{COMMA}         { return TOKEN_COMMA; }
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
{LSQUAR}        { return TOKEN_LSQUAR; }
{RSQUAR}        { return TOKEN_RSQUAR; }
{SEMICOLON}     { return TOKEN_SEMICOLON; }
{LBRA}          { return TOKEN_LBRA; }
{RBRA}          { return TOKEN_RBRA; }

{NAME}          { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_NAME; }
{ITERATOR}      { sscanf(yytext, VARNAME_FMT, yylval->symbol); return TOKEN_ITERATOR; }
{NUMBER}        { sscanf(yytext, TIME_FMT, &yylval->value); return TOKEN_NUMBER; }
{BLANKS}        {  }

.               { yyerror(yylloc, NULL, NULL, "unknown keyword"); exit(EXIT_FAILURE); }

%%

