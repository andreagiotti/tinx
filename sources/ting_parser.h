/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#line 34 "ting.y" /* yacc.c:1909  */

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

#line 62 "ting_parser.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOKEN_SEMICOLON = 258,
    TOKEN_EQV = 259,
    TOKEN_IMPLY = 260,
    TOKEN_INVIMPLY = 261,
    TOKEN_OR = 262,
    TOKEN_AND = 263,
    TOKEN_HAPPEN = 264,
    TOKEN_AT = 265,
    TOKEN_SINCE = 266,
    TOKEN_UNTIL = 267,
    TOKEN_NOT = 268,
    TOKEN_PLUS = 269,
    TOKEN_MINUS = 270,
    TOKEN_MUL = 271,
    TOKEN_DIV = 272,
    TOKEN_LBRACKET = 273,
    TOKEN_RBRACKET = 274,
    TOKEN_LPAREN = 275,
    TOKEN_RPAREN = 276,
    TOKEN_LSQUARED = 277,
    TOKEN_RSQUARED = 278,
    TOKEN_COMMA = 279,
    TOKEN_ITER = 280,
    TOKEN_DEFINE = 281,
    TOKEN_INCLUDE = 282,
    TOKEN_INPUT = 283,
    TOKEN_OUTPUT = 284,
    TOKEN_AUX = 285,
    TOKEN_INIT = 286,
    TOKEN_FORALL = 287,
    TOKEN_EXISTS = 288,
    TOKEN_ONE = 289,
    TOKEN_UNIQUE = 290,
    TOKEN_EQUAL = 291,
    TOKEN_ANY = 292,
    TOKEN_IPC = 293,
    TOKEN_FILE = 294,
    TOKEN_NAME = 295,
    TOKEN_FILENAME = 296,
    TOKEN_ITERATOR = 297,
    TOKEN_NUMBER = 298
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 64 "ting.y" /* yacc.c:1909  */

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

#line 146 "ting_parser.h" /* yacc.c:1909  */
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
