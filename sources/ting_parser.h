/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_TING_PARSER_H_INCLUDED
# define YY_YY_TING_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 38 "ting.y"

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

#line 67 "ting_parser.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOKEN_LBRACKET = 258,          /* TOKEN_LBRACKET  */
    TOKEN_RBRACKET = 259,          /* TOKEN_RBRACKET  */
    TOKEN_SEMICOLON = 260,         /* TOKEN_SEMICOLON  */
    TOKEN_LPAREN = 261,            /* TOKEN_LPAREN  */
    TOKEN_RPAREN = 262,            /* TOKEN_RPAREN  */
    TOKEN_LSQUARED = 263,          /* TOKEN_LSQUARED  */
    TOKEN_RSQUARED = 264,          /* TOKEN_RSQUARED  */
    TOKEN_COMMA = 265,             /* TOKEN_COMMA  */
    TOKEN_COLON = 266,             /* TOKEN_COLON  */
    TOKEN_ITER = 267,              /* TOKEN_ITER  */
    TOKEN_WHEN = 268,              /* TOKEN_WHEN  */
    TOKEN_WHENX = 269,             /* TOKEN_WHENX  */
    TOKEN_ELSE = 270,              /* TOKEN_ELSE  */
    TOKEN_DEFINE = 271,            /* TOKEN_DEFINE  */
    TOKEN_INCLUDE = 272,           /* TOKEN_INCLUDE  */
    TOKEN_INPUT = 273,             /* TOKEN_INPUT  */
    TOKEN_OUTPUT = 274,            /* TOKEN_OUTPUT  */
    TOKEN_AUX = 275,               /* TOKEN_AUX  */
    TOKEN_INIT = 276,              /* TOKEN_INIT  */
    TOKEN_CODE = 277,              /* TOKEN_CODE  */
    TOKEN_PVALUE = 278,            /* TOKEN_PVALUE  */
    TOKEN_NVALUE = 279,            /* TOKEN_NVALUE  */
    TOKEN_EQV = 280,               /* TOKEN_EQV  */
    TOKEN_IMPLY = 281,             /* TOKEN_IMPLY  */
    TOKEN_INVIMPLY = 282,          /* TOKEN_INVIMPLY  */
    TOKEN_XOR = 283,               /* TOKEN_XOR  */
    TOKEN_OR = 284,                /* TOKEN_OR  */
    TOKEN_AND = 285,               /* TOKEN_AND  */
    TOKEN_FORALL = 286,            /* TOKEN_FORALL  */
    TOKEN_EXISTS = 287,            /* TOKEN_EXISTS  */
    TOKEN_ONE = 288,               /* TOKEN_ONE  */
    TOKEN_UNIQUE = 289,            /* TOKEN_UNIQUE  */
    TOKEN_COMBINE = 290,           /* TOKEN_COMBINE  */
    TOKEN_SINCE = 291,             /* TOKEN_SINCE  */
    TOKEN_UNTIL = 292,             /* TOKEN_UNTIL  */
    TOKEN_ONLY = 293,              /* TOKEN_ONLY  */
    TOKEN_HAPPEN = 294,            /* TOKEN_HAPPEN  */
    TOKEN_AT = 295,                /* TOKEN_AT  */
    TOKEN_NOT = 296,               /* TOKEN_NOT  */
    TOKEN_EQUAL = 297,             /* TOKEN_EQUAL  */
    TOKEN_NEQ = 298,               /* TOKEN_NEQ  */
    TOKEN_LT = 299,                /* TOKEN_LT  */
    TOKEN_GT = 300,                /* TOKEN_GT  */
    TOKEN_LTEQ = 301,              /* TOKEN_LTEQ  */
    TOKEN_GTEQ = 302,              /* TOKEN_GTEQ  */
    TOKEN_PLUS = 303,              /* TOKEN_PLUS  */
    TOKEN_MINUS = 304,             /* TOKEN_MINUS  */
    TOKEN_MUL = 305,               /* TOKEN_MUL  */
    TOKEN_DIV = 306,               /* TOKEN_DIV  */
    TOKEN_MOD = 307,               /* TOKEN_MOD  */
    TOKEN_POW = 308,               /* TOKEN_POW  */
    TOKEN_ROOT = 309,              /* TOKEN_ROOT  */
    TOKEN_LOG = 310,               /* TOKEN_LOG  */
    TOKEN_SIN = 311,               /* TOKEN_SIN  */
    TOKEN_COS = 312,               /* TOKEN_COS  */
    TOKEN_TAN = 313,               /* TOKEN_TAN  */
    TOKEN_ASIN = 314,              /* TOKEN_ASIN  */
    TOKEN_ACOS = 315,              /* TOKEN_ACOS  */
    TOKEN_ATAN = 316,              /* TOKEN_ATAN  */
    TOKEN_SUM = 317,               /* TOKEN_SUM  */
    TOKEN_PROD = 318,              /* TOKEN_PROD  */
    TOKEN_KE = 319,                /* TOKEN_KE  */
    TOKEN_KPI = 320,               /* TOKEN_KPI  */
    TOKEN_ON = 321,                /* TOKEN_ON  */
    TOKEN_IN = 322,                /* TOKEN_IN  */
    TOKEN_IS = 323,                /* TOKEN_IS  */
    TOKEN_NEG = 324,               /* TOKEN_NEG  */
    TOKEN_ANY = 325,               /* TOKEN_ANY  */
    TOKEN_IPC = 326,               /* TOKEN_IPC  */
    TOKEN_FILE = 327,              /* TOKEN_FILE  */
    TOKEN_REMOTE = 328,            /* TOKEN_REMOTE  */
    TOKEN_BINARY = 329,            /* TOKEN_BINARY  */
    TOKEN_PACKED = 330,            /* TOKEN_PACKED  */
    TOKEN_UNKNOWN = 331,           /* TOKEN_UNKNOWN  */
    TOKEN_FALSE = 332,             /* TOKEN_FALSE  */
    TOKEN_TRUE = 333,              /* TOKEN_TRUE  */
    TOKEN_DEFAULT = 334,           /* TOKEN_DEFAULT  */
    TOKEN_RAW = 335,               /* TOKEN_RAW  */
    TOKEN_FILTER = 336,            /* TOKEN_FILTER  */
    TOKEN_OMIT = 337,              /* TOKEN_OMIT  */
    TOKEN_UNDERSCORE = 338,        /* TOKEN_UNDERSCORE  */
    TOKEN_NAME = 339,              /* TOKEN_NAME  */
    TOKEN_RICHNAME = 340,          /* TOKEN_RICHNAME  */
    TOKEN_REALNAME = 341,          /* TOKEN_REALNAME  */
    TOKEN_REALRICH = 342,          /* TOKEN_REALRICH  */
    TOKEN_STRING = 343,            /* TOKEN_STRING  */
    TOKEN_ITERATOR = 344,          /* TOKEN_ITERATOR  */
    TOKEN_NUMBER = 345,            /* TOKEN_NUMBER  */
    TOKEN_REAL = 346               /* TOKEN_REAL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 69 "ting.y"

  char symbol[MAX_NAMELEN];
  d_time value;
  real realval;
  btl_specification *other;

#line 182 "ting_parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int yyparse (btl_specification **spec_handle, yyscan_t scanner);


#endif /* !YY_YY_TING_PARSER_H_INCLUDED  */
