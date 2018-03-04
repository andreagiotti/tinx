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
#line 17 "ting.y" /* yacc.c:1909  */

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
  typedef void *yyscan_t;
#endif

#line 51 "ting_parser.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOKEN_SEMICOLON = 258,
    TOKEN_EQV = 259,
    TOKEN_IMPLY = 260,
    TOKEN_OR = 261,
    TOKEN_AND = 262,
    TOKEN_HAPPEN = 263,
    TOKEN_AT = 264,
    TOKEN_SINCE = 265,
    TOKEN_UNTIL = 266,
    TOKEN_NOT = 267,
    TOKEN_PLUS = 268,
    TOKEN_MINUS = 269,
    TOKEN_MUL = 270,
    TOKEN_DIV = 271,
    TOKEN_LBRA = 272,
    TOKEN_RBRA = 273,
    TOKEN_LPAREN = 274,
    TOKEN_RPAREN = 275,
    TOKEN_LSQUAR = 276,
    TOKEN_RSQUAR = 277,
    TOKEN_COMMA = 278,
    TOKEN_ITER = 279,
    TOKEN_DEFINE = 280,
    TOKEN_INPUT = 281,
    TOKEN_OUTPUT = 282,
    TOKEN_AUX = 283,
    TOKEN_INIT = 284,
    TOKEN_FORALL = 285,
    TOKEN_EXISTS = 286,
    TOKEN_ONE = 287,
    TOKEN_UNIQUE = 288,
    TOKEN_EQUAL = 289,
    TOKEN_NAME = 290,
    TOKEN_ITERATOR = 291,
    TOKEN_NUMBER = 292
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 35 "ting.y" /* yacc.c:1909  */

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
  btl_specification *decllist;
  btl_specification *eventlist;
  btl_specification *assignlist;
  btl_specification *assignment;
  btl_specification *varname;
  btl_specification *declname;
  btl_specification *initname;
  btl_specification *eventname;
  btl_specification *qualname;
  btl_specification *constname;
  btl_specification *expression;
  btl_specification *number;

#line 126 "ting_parser.h" /* yacc.c:1909  */
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