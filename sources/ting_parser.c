/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "ting.y"


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


#line 106 "ting_parser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "ting_parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TOKEN_LBRACKET = 3,             /* TOKEN_LBRACKET  */
  YYSYMBOL_TOKEN_RBRACKET = 4,             /* TOKEN_RBRACKET  */
  YYSYMBOL_TOKEN_SEMICOLON = 5,            /* TOKEN_SEMICOLON  */
  YYSYMBOL_TOKEN_LPAREN = 6,               /* TOKEN_LPAREN  */
  YYSYMBOL_TOKEN_RPAREN = 7,               /* TOKEN_RPAREN  */
  YYSYMBOL_TOKEN_LSQUARED = 8,             /* TOKEN_LSQUARED  */
  YYSYMBOL_TOKEN_RSQUARED = 9,             /* TOKEN_RSQUARED  */
  YYSYMBOL_TOKEN_COMMA = 10,               /* TOKEN_COMMA  */
  YYSYMBOL_TOKEN_COLON = 11,               /* TOKEN_COLON  */
  YYSYMBOL_TOKEN_ITER = 12,                /* TOKEN_ITER  */
  YYSYMBOL_TOKEN_WHEN = 13,                /* TOKEN_WHEN  */
  YYSYMBOL_TOKEN_WHENX = 14,               /* TOKEN_WHENX  */
  YYSYMBOL_TOKEN_ELSE = 15,                /* TOKEN_ELSE  */
  YYSYMBOL_TOKEN_DEFINE = 16,              /* TOKEN_DEFINE  */
  YYSYMBOL_TOKEN_INCLUDE = 17,             /* TOKEN_INCLUDE  */
  YYSYMBOL_TOKEN_INPUT = 18,               /* TOKEN_INPUT  */
  YYSYMBOL_TOKEN_OUTPUT = 19,              /* TOKEN_OUTPUT  */
  YYSYMBOL_TOKEN_AUX = 20,                 /* TOKEN_AUX  */
  YYSYMBOL_TOKEN_INIT = 21,                /* TOKEN_INIT  */
  YYSYMBOL_TOKEN_CODE = 22,                /* TOKEN_CODE  */
  YYSYMBOL_TOKEN_PVALUE = 23,              /* TOKEN_PVALUE  */
  YYSYMBOL_TOKEN_NVALUE = 24,              /* TOKEN_NVALUE  */
  YYSYMBOL_TOKEN_EQV = 25,                 /* TOKEN_EQV  */
  YYSYMBOL_TOKEN_IMPLY = 26,               /* TOKEN_IMPLY  */
  YYSYMBOL_TOKEN_INVIMPLY = 27,            /* TOKEN_INVIMPLY  */
  YYSYMBOL_TOKEN_XOR = 28,                 /* TOKEN_XOR  */
  YYSYMBOL_TOKEN_OR = 29,                  /* TOKEN_OR  */
  YYSYMBOL_TOKEN_AND = 30,                 /* TOKEN_AND  */
  YYSYMBOL_TOKEN_FORALL = 31,              /* TOKEN_FORALL  */
  YYSYMBOL_TOKEN_EXISTS = 32,              /* TOKEN_EXISTS  */
  YYSYMBOL_TOKEN_ONE = 33,                 /* TOKEN_ONE  */
  YYSYMBOL_TOKEN_UNIQUE = 34,              /* TOKEN_UNIQUE  */
  YYSYMBOL_TOKEN_COMBINE = 35,             /* TOKEN_COMBINE  */
  YYSYMBOL_TOKEN_SINCE = 36,               /* TOKEN_SINCE  */
  YYSYMBOL_TOKEN_UNTIL = 37,               /* TOKEN_UNTIL  */
  YYSYMBOL_TOKEN_ONLY = 38,                /* TOKEN_ONLY  */
  YYSYMBOL_TOKEN_HAPPEN = 39,              /* TOKEN_HAPPEN  */
  YYSYMBOL_TOKEN_AT = 40,                  /* TOKEN_AT  */
  YYSYMBOL_TOKEN_NOT = 41,                 /* TOKEN_NOT  */
  YYSYMBOL_TOKEN_EQUAL = 42,               /* TOKEN_EQUAL  */
  YYSYMBOL_TOKEN_NEQ = 43,                 /* TOKEN_NEQ  */
  YYSYMBOL_TOKEN_LT = 44,                  /* TOKEN_LT  */
  YYSYMBOL_TOKEN_GT = 45,                  /* TOKEN_GT  */
  YYSYMBOL_TOKEN_LTEQ = 46,                /* TOKEN_LTEQ  */
  YYSYMBOL_TOKEN_GTEQ = 47,                /* TOKEN_GTEQ  */
  YYSYMBOL_TOKEN_PLUS = 48,                /* TOKEN_PLUS  */
  YYSYMBOL_TOKEN_MINUS = 49,               /* TOKEN_MINUS  */
  YYSYMBOL_TOKEN_MUL = 50,                 /* TOKEN_MUL  */
  YYSYMBOL_TOKEN_DIV = 51,                 /* TOKEN_DIV  */
  YYSYMBOL_TOKEN_MOD = 52,                 /* TOKEN_MOD  */
  YYSYMBOL_TOKEN_POW = 53,                 /* TOKEN_POW  */
  YYSYMBOL_TOKEN_ROOT = 54,                /* TOKEN_ROOT  */
  YYSYMBOL_TOKEN_LOG = 55,                 /* TOKEN_LOG  */
  YYSYMBOL_TOKEN_SIN = 56,                 /* TOKEN_SIN  */
  YYSYMBOL_TOKEN_COS = 57,                 /* TOKEN_COS  */
  YYSYMBOL_TOKEN_TAN = 58,                 /* TOKEN_TAN  */
  YYSYMBOL_TOKEN_ASIN = 59,                /* TOKEN_ASIN  */
  YYSYMBOL_TOKEN_ACOS = 60,                /* TOKEN_ACOS  */
  YYSYMBOL_TOKEN_ATAN = 61,                /* TOKEN_ATAN  */
  YYSYMBOL_TOKEN_SUM = 62,                 /* TOKEN_SUM  */
  YYSYMBOL_TOKEN_PROD = 63,                /* TOKEN_PROD  */
  YYSYMBOL_TOKEN_KE = 64,                  /* TOKEN_KE  */
  YYSYMBOL_TOKEN_KPI = 65,                 /* TOKEN_KPI  */
  YYSYMBOL_TOKEN_ON = 66,                  /* TOKEN_ON  */
  YYSYMBOL_TOKEN_IN = 67,                  /* TOKEN_IN  */
  YYSYMBOL_TOKEN_IS = 68,                  /* TOKEN_IS  */
  YYSYMBOL_TOKEN_NEG = 69,                 /* TOKEN_NEG  */
  YYSYMBOL_TOKEN_ANY = 70,                 /* TOKEN_ANY  */
  YYSYMBOL_TOKEN_IPC = 71,                 /* TOKEN_IPC  */
  YYSYMBOL_TOKEN_FILE = 72,                /* TOKEN_FILE  */
  YYSYMBOL_TOKEN_REMOTE = 73,              /* TOKEN_REMOTE  */
  YYSYMBOL_TOKEN_BINARY = 74,              /* TOKEN_BINARY  */
  YYSYMBOL_TOKEN_PACKED = 75,              /* TOKEN_PACKED  */
  YYSYMBOL_TOKEN_UNKNOWN = 76,             /* TOKEN_UNKNOWN  */
  YYSYMBOL_TOKEN_FALSE = 77,               /* TOKEN_FALSE  */
  YYSYMBOL_TOKEN_TRUE = 78,                /* TOKEN_TRUE  */
  YYSYMBOL_TOKEN_DEFAULT = 79,             /* TOKEN_DEFAULT  */
  YYSYMBOL_TOKEN_RAW = 80,                 /* TOKEN_RAW  */
  YYSYMBOL_TOKEN_FILTER = 81,              /* TOKEN_FILTER  */
  YYSYMBOL_TOKEN_OMIT = 82,                /* TOKEN_OMIT  */
  YYSYMBOL_TOKEN_UNDERSCORE = 83,          /* TOKEN_UNDERSCORE  */
  YYSYMBOL_TOKEN_NAME = 84,                /* TOKEN_NAME  */
  YYSYMBOL_TOKEN_RICHNAME = 85,            /* TOKEN_RICHNAME  */
  YYSYMBOL_TOKEN_REALNAME = 86,            /* TOKEN_REALNAME  */
  YYSYMBOL_TOKEN_REALRICH = 87,            /* TOKEN_REALRICH  */
  YYSYMBOL_TOKEN_STRING = 88,              /* TOKEN_STRING  */
  YYSYMBOL_TOKEN_ITERATOR = 89,            /* TOKEN_ITERATOR  */
  YYSYMBOL_TOKEN_NUMBER = 90,              /* TOKEN_NUMBER  */
  YYSYMBOL_TOKEN_REAL = 91,                /* TOKEN_REAL  */
  YYSYMBOL_YYACCEPT = 92,                  /* $accept  */
  YYSYMBOL_input = 93,                     /* input  */
  YYSYMBOL_specification = 94,             /* specification  */
  YYSYMBOL_blocklist = 95,                 /* blocklist  */
  YYSYMBOL_block = 96,                     /* block  */
  YYSYMBOL_formula = 97,                   /* formula  */
  YYSYMBOL_atconstruct = 98,               /* atconstruct  */
  YYSYMBOL_happenconstruct = 99,           /* happenconstruct  */
  YYSYMBOL_onlyconstruct = 100,            /* onlyconstruct  */
  YYSYMBOL_postime = 101,                  /* postime  */
  YYSYMBOL_negtime = 102,                  /* negtime  */
  YYSYMBOL_vartime = 103,                  /* vartime  */
  YYSYMBOL_interval = 104,                 /* interval  */
  YYSYMBOL_varinterval = 105,              /* varinterval  */
  YYSYMBOL_declaration = 106,              /* declaration  */
  YYSYMBOL_iodecllist = 107,               /* iodecllist  */
  YYSYMBOL_iodecl = 108,                   /* iodecl  */
  YYSYMBOL_decllist = 109,                 /* decllist  */
  YYSYMBOL_eventlist = 110,                /* eventlist  */
  YYSYMBOL_filelist = 111,                 /* filelist  */
  YYSYMBOL_assignlist = 112,               /* assignlist  */
  YYSYMBOL_globveclist = 113,              /* globveclist  */
  YYSYMBOL_globvector = 114,               /* globvector  */
  YYSYMBOL_assignment = 115,               /* assignment  */
  YYSYMBOL_eventname = 116,                /* eventname  */
  YYSYMBOL_qualname = 117,                 /* qualname  */
  YYSYMBOL_varidxname = 118,               /* varidxname  */
  YYSYMBOL_indexedname = 119,              /* indexedname  */
  YYSYMBOL_mathidxname = 120,              /* mathidxname  */
  YYSYMBOL_declidxname = 121,              /* declidxname  */
  YYSYMBOL_initidxname = 122,              /* initidxname  */
  YYSYMBOL_initmathidxname = 123,          /* initmathidxname  */
  YYSYMBOL_constidxname = 124,             /* constidxname  */
  YYSYMBOL_exprorrange = 125,              /* exprorrange  */
  YYSYMBOL_name = 126,                     /* name  */
  YYSYMBOL_mathname = 127,                 /* mathname  */
  YYSYMBOL_declname = 128,                 /* declname  */
  YYSYMBOL_initname = 129,                 /* initname  */
  YYSYMBOL_initmathname = 130,             /* initmathname  */
  YYSYMBOL_constname = 131,                /* constname  */
  YYSYMBOL_filename = 132,                 /* filename  */
  YYSYMBOL_string = 133,                   /* string  */
  YYSYMBOL_varrange = 134,                 /* varrange  */
  YYSYMBOL_selrange = 135,                 /* selrange  */
  YYSYMBOL_exprrange = 136,                /* exprrange  */
  YYSYMBOL_mathrelation = 137,             /* mathrelation  */
  YYSYMBOL_mathexpression = 138,           /* mathexpression  */
  YYSYMBOL_mathexprmul = 139,              /* mathexprmul  */
  YYSYMBOL_mathexprpow = 140,              /* mathexprpow  */
  YYSYMBOL_mathexprroot = 141,             /* mathexprroot  */
  YYSYMBOL_mathexprchs = 142,              /* mathexprchs  */
  YYSYMBOL_mathexprdelay = 143,            /* mathexprdelay  */
  YYSYMBOL_mathexprgnd = 144,              /* mathexprgnd  */
  YYSYMBOL_expression = 145,               /* expression  */
  YYSYMBOL_exprmul = 146,                  /* exprmul  */
  YYSYMBOL_exprpow = 147,                  /* exprpow  */
  YYSYMBOL_exprroot = 148,                 /* exprroot  */
  YYSYMBOL_exprchs = 149,                  /* exprchs  */
  YYSYMBOL_exprgnd = 150,                  /* exprgnd  */
  YYSYMBOL_nameornum = 151,                /* nameornum  */
  YYSYMBOL_realnumber = 152,               /* realnumber  */
  YYSYMBOL_number = 153                    /* number  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  132
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1555

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  92
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  62
/* YYNRULES -- Number of rules.  */
#define YYNRULES  255
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  599

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   346


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   251,   251,   255,   256,   260,   261,   265,   266,   267,
     270,   271,   272,   276,   277,   278,   279,   280,   281,   282,
     283,   285,   286,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   303,   304,   305,   306,   310,
     311,   312,   313,   317,   318,   319,   320,   324,   328,   332,
     333,   337,   338,   339,   340,   344,   345,   346,   347,   348,
     352,   353,   356,   357,   360,   361,   362,   363,   367,   368,
     372,   373,   374,   375,   376,   377,   378,   379,   380,   381,
     382,   383,   384,   388,   389,   393,   394,   398,   399,   403,
     404,   408,   409,   413,   414,   417,   418,   422,   423,   424,
     425,   429,   430,   434,   435,   439,   441,   444,   446,   450,
     452,   455,   457,   458,   462,   464,   467,   469,   470,   474,
     476,   479,   481,   482,   486,   488,   491,   493,   494,   498,
     500,   503,   505,   506,   510,   511,   515,   516,   520,   521,
     525,   526,   527,   528,   532,   533,   537,   538,   542,   546,
     550,   554,   555,   556,   557,   561,   562,   563,   564,   565,
     569,   570,   571,   575,   576,   577,   578,   579,   580,   581,
     582,   583,   584,   585,   586,   587,   588,   589,   590,   591,
     592,   596,   597,   598,   599,   600,   601,   602,   606,   607,
     608,   609,   610,   611,   612,   613,   614,   615,   619,   620,
     621,   622,   626,   627,   628,   629,   630,   631,   632,   633,
     634,   635,   636,   637,   638,   639,   640,   644,   645,   649,
     650,   654,   655,   659,   660,   661,   665,   666,   667,   668,
     672,   673,   677,   678,   679,   680,   681,   682,   683,   684,
     685,   686,   687,   691,   692,   696,   697,   701,   702,   703,
     704,   705,   709,   710,   711,   715
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOKEN_LBRACKET",
  "TOKEN_RBRACKET", "TOKEN_SEMICOLON", "TOKEN_LPAREN", "TOKEN_RPAREN",
  "TOKEN_LSQUARED", "TOKEN_RSQUARED", "TOKEN_COMMA", "TOKEN_COLON",
  "TOKEN_ITER", "TOKEN_WHEN", "TOKEN_WHENX", "TOKEN_ELSE", "TOKEN_DEFINE",
  "TOKEN_INCLUDE", "TOKEN_INPUT", "TOKEN_OUTPUT", "TOKEN_AUX",
  "TOKEN_INIT", "TOKEN_CODE", "TOKEN_PVALUE", "TOKEN_NVALUE", "TOKEN_EQV",
  "TOKEN_IMPLY", "TOKEN_INVIMPLY", "TOKEN_XOR", "TOKEN_OR", "TOKEN_AND",
  "TOKEN_FORALL", "TOKEN_EXISTS", "TOKEN_ONE", "TOKEN_UNIQUE",
  "TOKEN_COMBINE", "TOKEN_SINCE", "TOKEN_UNTIL", "TOKEN_ONLY",
  "TOKEN_HAPPEN", "TOKEN_AT", "TOKEN_NOT", "TOKEN_EQUAL", "TOKEN_NEQ",
  "TOKEN_LT", "TOKEN_GT", "TOKEN_LTEQ", "TOKEN_GTEQ", "TOKEN_PLUS",
  "TOKEN_MINUS", "TOKEN_MUL", "TOKEN_DIV", "TOKEN_MOD", "TOKEN_POW",
  "TOKEN_ROOT", "TOKEN_LOG", "TOKEN_SIN", "TOKEN_COS", "TOKEN_TAN",
  "TOKEN_ASIN", "TOKEN_ACOS", "TOKEN_ATAN", "TOKEN_SUM", "TOKEN_PROD",
  "TOKEN_KE", "TOKEN_KPI", "TOKEN_ON", "TOKEN_IN", "TOKEN_IS", "TOKEN_NEG",
  "TOKEN_ANY", "TOKEN_IPC", "TOKEN_FILE", "TOKEN_REMOTE", "TOKEN_BINARY",
  "TOKEN_PACKED", "TOKEN_UNKNOWN", "TOKEN_FALSE", "TOKEN_TRUE",
  "TOKEN_DEFAULT", "TOKEN_RAW", "TOKEN_FILTER", "TOKEN_OMIT",
  "TOKEN_UNDERSCORE", "TOKEN_NAME", "TOKEN_RICHNAME", "TOKEN_REALNAME",
  "TOKEN_REALRICH", "TOKEN_STRING", "TOKEN_ITERATOR", "TOKEN_NUMBER",
  "TOKEN_REAL", "$accept", "input", "specification", "blocklist", "block",
  "formula", "atconstruct", "happenconstruct", "onlyconstruct", "postime",
  "negtime", "vartime", "interval", "varinterval", "declaration",
  "iodecllist", "iodecl", "decllist", "eventlist", "filelist",
  "assignlist", "globveclist", "globvector", "assignment", "eventname",
  "qualname", "varidxname", "indexedname", "mathidxname", "declidxname",
  "initidxname", "initmathidxname", "constidxname", "exprorrange", "name",
  "mathname", "declname", "initname", "initmathname", "constname",
  "filename", "string", "varrange", "selrange", "exprrange",
  "mathrelation", "mathexpression", "mathexprmul", "mathexprpow",
  "mathexprroot", "mathexprchs", "mathexprdelay", "mathexprgnd",
  "expression", "exprmul", "exprpow", "exprroot", "exprchs", "exprgnd",
  "nameornum", "realnumber", "number", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-443)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-249)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     708,   708,   225,    19,    30,   -57,   -50,    46,    50,   213,
     191,    51,    54,    75,    80,    85,    96,   108,   112,   225,
       3,   136,   164,   177,   231,   261,   267,   307,   320,   334,
     342,  -443,  -443,  -443,  -443,  -443,  -443,  -443,  -443,  -443,
     197,  -443,  -443,   708,  1071,  -443,  -443,  -443,   246,  -443,
    1091,  -443,   794,   344,  -443,  1309,    -5,   264,  -443,  -443,
    -443,   283,  1376,   116,   304,  -443,  -443,  -443,  -443,  -443,
    -443,   369,  1187,   347,   426,  1344,   882,  -443,   361,  -443,
     375,   401,  -443,   430,  -443,  1104,  -443,  -443,  -443,  -443,
     445,  -443,   405,  1104,   445,   445,    88,  -443,  -443,  -443,
    -443,   447,  -443,   453,  -443,   456,   502,   512,    91,   225,
     225,   225,   225,   225,   225,   225,  -443,   726,  -443,   526,
    -443,  -443,   726,   726,   726,   726,   726,   726,   726,   726,
     726,   726,  -443,  -443,  -443,   225,   225,   225,   225,   225,
     225,   844,   844,   844,  -443,  1388,  1388,   726,   726,   726,
     726,   726,   726,   726,   726,   726,   726,   726,   726,  1388,
     726,   726,   726,   726,   726,   726,   726,   726,   726,   726,
     726,   726,  -443,  -443,  -443,  -443,  1388,    20,   530,   531,
     540,   542,   544,   554,   562,   569,   577,   588,   182,   203,
     504,   371,   168,   446,   882,   533,   514,  -443,     4,   -57,
     944,  1004,   -50,  -443,  -443,  -443,  -443,  -443,  -443,  -443,
    -443,  -443,   481,  -443,  -443,  -443,   310,  -443,   213,  1388,
     335,  -443,   191,  1064,  1064,  1388,  1388,   582,  -443,   526,
    1012,  1429,  1455,  1461,  1482,  1488,  1509,    59,    68,     0,
       2,    49,   223,   186,   247,   349,   367,   381,   443,   505,
     507,   510,   513,   515,   517,   524,   586,   809,   838,  1284,
    1232,  1045,  1052,   206,   466,  1388,  1108,   589,   591,  -443,
    -443,  -443,  -443,  -443,   383,  -443,  -443,  -443,   383,  -443,
    -443,  -443,   383,     7,    21,   386,   399,   386,   399,   386,
     399,   386,   399,   386,   399,   386,   399,    -5,   116,    -5,
     116,   264,   304,   264,   304,   264,   304,  -443,  -443,  -443,
     386,   399,   386,   386,   386,   386,   386,    -5,   116,    -5,
     116,   264,   304,   264,   304,   264,   304,  -443,  -443,   522,
    1388,  1388,  1388,  1388,  1388,  1388,  1388,  1388,  1388,  1388,
    1388,  1388,   708,  1388,  1388,  1388,  1388,  1388,  1388,  1388,
    -443,  1388,   708,  1146,  1184,  1388,  -443,  1244,   383,  -443,
      22,   383,  -443,  1388,   213,  1104,  -443,   243,   213,  -443,
    1388,  1388,  -443,   383,   568,    64,   245,   258,  1282,  1344,
    1344,   882,  1344,   225,   225,   225,   726,   726,   726,   726,
    -443,  -443,  -443,  -443,  -443,  -443,  -443,  -443,  -443,  -443,
    -443,  -443,  1344,  1344,  1344,  1344,   294,   612,   939,    91,
      91,  -443,  1388,  -443,  1388,   948,   964,   560,   570,   572,
     610,   674,   686,   966,   968,   383,   388,  -443,   383,   168,
     168,   446,   446,   446,   383,   639,  -443,   383,  -443,   394,
     383,  1244,   442,  -443,   383,  -443,  1004,   383,   445,  -443,
    -443,  1388,   445,   298,   970,  1388,  1388,  -443,  1388,  -443,
    1388,  -443,   649,   699,   650,   672,   670,   675,  1515,  1251,
    1257,   702,   705,   710,   712,   729,   772,   807,   828,   676,
     677,   690,   700,  1326,  1326,  1326,   698,   706,   311,   315,
    1388,  1388,  1388,   708,   492,  -443,  1244,    23,   317,  1388,
    1388,   383,   383,   360,   365,  -443,  -443,  -443,  -443,  1344,
    -443,  1344,  -443,  -443,  -443,  -443,  -443,  -443,  -443,  -443,
    -443,  -443,  -443,  -443,  -443,  -443,   701,    34,   709,   374,
     722,   397,  1388,  1388,  -443,  1388,  -443,  1388,   846,   862,
     383,  -443,  -443,  -443,  -443,  1004,  -443,  1388,  -443,  1388,
    -443,  1388,   731,   739,  -443,  -443,  -443,  -443,  -443,  -443,
    -443,  -443,  -443,   864,   866,   414,   431,    89,   477,   493,
     503,  -443,  -443,  -443,  -443,  -443,  1388,  -443,  1388,  -443,
    1004,  -443,  1388,  -443,  1388,  -443,  1388,   868,   871,   745,
     911,   941,   946,  -443,  -443,  -443,  -443,  -443,  -443
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   253,   254,   137,   136,   139,   138,   249,   255,   252,
       0,     2,     3,     6,     0,    30,    31,    32,     0,    34,
     103,   222,   104,   113,    33,     0,   187,   197,   201,   216,
     218,   220,     0,   225,   229,   231,   242,   244,   246,   250,
     251,     0,     0,     0,     0,     0,     0,   148,    66,    90,
       0,   133,   149,    67,    88,     0,   141,   140,   143,   142,
      61,    84,   118,     0,    63,    64,     0,   145,   144,   147,
     146,    65,    86,     0,   102,     0,   123,   128,     0,     0,
       0,     0,     0,     0,     0,     0,    13,     0,   247,   248,
     217,   243,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     5,    11,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    12,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    10,    29,   221,   245,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   137,     0,
       0,   154,   225,   229,     0,     0,     0,   158,   159,     0,
       0,     0,     0,    70,    71,    72,    73,    74,    75,    76,
      77,    78,     0,    80,    81,    82,     0,    69,     0,     0,
       0,   101,     0,     0,     0,     0,     0,     0,   103,   104,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    26,
      24,    25,    23,    15,    14,     0,     0,     0,     0,    49,
      50,    44,    45,    46,    43,    40,    41,    42,    39,    36,
      37,    38,    35,     0,     0,   163,   169,   164,   170,   165,
     171,   166,   172,   167,   173,   168,   174,   181,   183,   182,
     184,   188,   191,   189,   192,   190,   193,   198,   199,   219,
     175,     0,   176,   177,   178,   179,   180,   185,   223,   186,
     224,   194,   226,   195,   227,   196,   228,   200,   230,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     155,     0,     0,     0,     0,     0,    89,     0,    95,   135,
       0,   134,    87,     0,     0,     0,    83,     0,     0,    85,
       0,     0,    98,    97,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     208,   234,   209,   235,   210,   236,   211,   237,   212,   238,
     213,   239,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108,     0,   112,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   152,     0,     7,   153,   223,
     224,   226,   227,   228,   161,     8,   162,   160,   156,     0,
     157,     0,     0,    92,    94,   132,     0,    79,    60,    68,
     117,     0,    62,     0,     0,     0,     0,   122,     0,   127,
       0,   150,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    96,     0,     0,     0,     0,
       0,   100,    99,     0,     0,    27,    28,    18,    19,     0,
      21,     0,    16,    17,   202,   204,   206,   232,   203,   205,
     207,   233,   214,   240,   215,   241,     0,     0,     0,     0,
       0,     0,     0,     0,   107,     0,   111,     0,     0,     0,
     151,     9,    93,    91,   131,     0,   116,     0,   121,     0,
     126,     0,     0,     0,    56,    54,    52,    59,    58,    57,
      55,    53,    51,     0,     0,     0,     0,     0,     0,     0,
       0,    20,    22,    47,    48,   106,     0,   110,     0,   130,
       0,   115,     0,   120,     0,   125,     0,     0,     0,     0,
       0,     0,     0,   105,   109,   129,   114,   119,   124
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -443,  -443,  -443,    36,  -329,    42,  -443,  -443,  -443,   187,
     449,   373,  -123,   385,  -443,   640,   390,    -6,  -443,  -443,
    -443,   333,   260,   597,   579,  -443,  -107,    76,  -443,   558,
     730,  -443,    11,  -442,   694,  -443,  -443,  -443,  -443,  -443,
     623,  -443,  -340,  -187,   473,  -443,    78,  -132,   990,  -147,
    -443,   829,  -443,    -2,   538,   476,  -153,  -443,   -12,  -443,
    -443,  -443
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    40,    41,    42,    43,    44,    45,    46,    47,   269,
     270,   271,   272,   273,    48,   216,   217,    90,   101,    83,
      78,   442,   443,    79,   102,   103,    49,   118,    51,    91,
     104,   105,   189,   360,   119,    53,    92,   106,   107,    81,
      84,   462,   190,   196,   197,    54,    55,    56,    57,    58,
      59,    60,    61,    62,   192,   193,    65,    66,    67,    68,
      69,    70
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      74,   227,    94,    95,   497,   308,   309,   350,   121,   117,
     386,   307,   387,   427,   411,   353,    80,   412,   328,   276,
     280,   297,   299,   435,   327,    75,   176,    77,   413,   445,
     544,   414,   446,   545,   317,   319,    76,    71,    82,   464,
     465,   555,   467,   556,    72,   155,   156,   157,   153,   154,
     166,   167,   344,   345,    85,   344,   345,   108,    93,   388,
     109,   116,   479,   480,   481,   482,   174,    31,    32,   344,
     345,   354,   355,   191,   198,   175,    50,    50,    50,   133,
      73,   110,   344,   345,    31,    32,   111,    33,    34,    35,
      36,   112,    37,    38,    39,    50,   579,   153,   154,   580,
     372,   374,   113,   567,    33,    34,   456,   153,   154,    37,
      38,    39,   344,   345,   114,   238,   166,   167,   115,    50,
     240,   242,   244,   246,   248,   250,   252,   254,   256,   258,
      86,    87,    88,    89,    86,    87,    88,    89,   589,   274,
     278,   282,   122,   283,   284,   286,   288,   290,   292,   294,
     296,   230,   231,   232,   233,   234,   235,   236,   311,   311,
     311,   311,   311,   311,   541,   121,   168,   169,   170,   552,
     123,   553,    97,    98,   329,    33,    34,   259,   260,   261,
     262,   263,   264,   124,   228,    50,    50,    50,    50,    50,
      50,    50,   198,   390,   466,   237,   328,   132,   358,   361,
     239,   241,   243,   245,   247,   249,   251,   253,   255,   257,
      80,    50,    50,    50,    50,    50,    50,   367,   346,   347,
     348,   373,   375,   376,   377,   285,   287,   289,   291,   293,
     295,     2,    96,   389,   153,   154,   140,   125,   310,   312,
     313,   314,   315,   316,   141,   142,   143,    11,  -148,  -148,
     450,   144,   457,   451,   391,   458,    12,    13,    14,    15,
      16,    17,    18,   406,   408,   459,    19,   126,   460,   340,
     341,   166,   167,   127,    20,    97,    98,    99,   100,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,   344,   345,   344,   345,   166,   167,    86,    87,    88,
      89,   175,   486,   487,   483,   175,   344,   345,   499,    33,
      34,    35,    36,   128,    37,    38,    39,   158,   534,   364,
     365,   535,   536,   159,   546,   537,   129,   547,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   424,   425,   426,
     130,   428,   344,   345,   368,   365,   344,   345,   131,   434,
     146,   437,   439,   440,   174,   444,   392,   171,   448,   344,
     345,   447,   452,   344,   345,   344,   345,   548,   453,   454,
     549,   199,   550,   172,   393,   551,   463,   191,   191,   198,
     191,   558,   343,   559,   472,   474,   476,   478,   394,   147,
     148,   149,   150,   151,   152,   153,   154,   153,   154,   492,
     191,   191,   191,   191,   561,   353,   562,   201,   344,   345,
     488,   219,   489,   344,   345,   166,   167,   200,    50,   344,
     345,   575,   344,   345,   576,   468,   469,   470,    50,   153,
     154,   344,   345,   175,   153,   154,   344,   345,   577,   444,
     202,   578,   344,   345,   361,   344,   345,   166,   167,   498,
     395,   495,   496,   501,   502,   218,   503,   222,   504,    50,
      50,    50,   344,   345,   471,   473,   475,   477,   160,   161,
     162,   163,   164,   165,   166,   167,    64,    64,    64,   344,
     345,   527,   529,   531,   581,   228,   228,   582,   538,   539,
     540,   166,   167,   223,   444,    64,   224,   527,   531,   349,
     583,   542,   496,   584,   141,   142,   143,   191,   225,   191,
     585,   342,   396,   586,   397,   275,   279,   398,   226,    64,
     399,   352,   400,   363,   401,   344,   345,   277,   281,   175,
     563,   564,   145,   565,   402,   566,   330,   331,    63,    63,
      63,   344,   345,   361,   351,   568,   332,   569,   333,   570,
     334,   344,   345,   153,   154,   166,   167,    63,   153,   154,
     335,   166,   167,   153,   154,   166,   167,   391,   336,    50,
     344,   345,   153,   154,   587,   337,   588,   393,   361,   395,
     590,    63,   591,   338,   592,    64,    64,    64,    64,    64,
      64,    64,   378,    64,   339,   409,   403,   410,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,   344,   345,
     455,    64,    64,    64,    64,    64,    64,   397,   344,   345,
     344,   345,   484,    64,    64,    64,    64,    64,    64,    64,
      64,   302,   304,   306,   166,   167,    64,    64,    64,    64,
      64,    64,    64,    64,   322,   324,   326,    63,    63,    63,
      63,    63,    63,    63,   493,    63,   505,   507,   344,   345,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
     526,   528,   530,    63,    63,    63,    63,    63,    63,   508,
     509,   399,   510,   522,   523,    63,    63,    63,    63,    63,
      63,   298,   300,   401,    52,    52,    52,   524,    63,    63,
      63,    63,    63,    63,   318,   320,   506,   525,   532,   514,
     554,     1,   515,    52,     2,   407,   533,   516,   557,   517,
       3,     4,   344,   345,     5,     6,     7,     8,     9,    10,
      11,   560,   117,   220,   344,   345,   518,    52,   571,    12,
      13,    14,    15,    16,    17,    18,   572,   344,   345,    19,
     153,   154,   595,   166,   167,   449,   543,    20,   153,   154,
     166,   167,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,   494,    20,   366,   153,   154,   519,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,   356,    37,    38,    39,
     145,   369,   229,    52,    52,    52,    52,    52,    52,    52,
      33,    34,    35,    36,   520,    37,    38,    39,    64,   404,
     166,   167,   431,   432,   433,   362,   221,   438,    64,    52,
      52,    52,    52,    52,    52,   521,  -248,  -248,  -248,  -248,
    -248,  -248,  -248,  -248,  -248,  -248,  -248,  -248,   405,   120,
     265,     0,   266,   517,     0,   153,   154,   153,   154,    64,
      64,    64,    64,    64,    64,    64,     0,   267,   268,   521,
       0,   573,     0,   574,     0,   593,   166,   167,   594,     0,
      63,     0,   429,   430,     0,     0,   166,   167,   176,     0,
      63,     0,     0,   177,   344,   345,     0,     0,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,    31,    32,
     344,   345,   344,   345,   344,   345,   344,   345,   596,   344,
     345,    63,    63,    63,    63,    63,    63,    63,    33,    34,
       0,   177,     0,    37,    38,    39,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,    31,    32,   597,   485,
     176,   194,   357,   598,     0,     0,     0,     0,   490,   344,
     345,     0,     0,     0,     0,   195,    33,    34,     0,    64,
       0,    37,    38,    39,   491,     0,   403,     0,   405,     0,
     500,     0,     0,     0,     0,     0,     0,   344,   345,   344,
     345,     0,     0,   177,   344,   345,   344,   345,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,    31,    32,
     176,     0,   344,   345,   344,   345,   344,   345,   344,   345,
       0,     0,   379,     0,     0,     0,     0,     0,    33,    34,
       0,    63,     0,    37,    38,    39,    52,   135,   136,   137,
     138,   139,   140,     0,     0,     0,    52,     0,     0,     0,
     141,   142,   143,   177,     0,     0,     0,     0,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,    31,    32,
     370,     0,   371,   138,   139,   140,   134,    52,    52,    52,
       0,   139,   140,   141,   142,   143,     0,   359,    33,    34,
     141,   142,   143,    37,    38,    39,   135,   136,   137,   138,
     139,   140,     0,   229,   229,     0,     0,     0,     0,   141,
     142,   143,     0,   177,   176,     0,     0,     0,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,    31,    32,
       0,     0,   268,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,  -247,  -247,  -247,  -247,   301,   303,   305,    33,    34,
       0,     0,   176,    37,    38,    39,     0,   177,   321,   323,
     325,     0,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,    31,    32,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,    52,     0,     0,
     176,     0,    33,    34,   173,   177,     0,    37,    38,    39,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
      31,    32,   135,   136,   137,   138,   139,   140,     0,     0,
       0,     0,     0,     0,     0,   141,   142,   143,     0,   436,
      33,    34,     0,   177,     0,    37,    38,    39,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,    31,    32,
     176,     0,   441,     0,     0,     0,     0,     0,   512,   137,
     138,   139,   140,     0,   513,     0,     0,   195,    33,    34,
     141,   142,   143,    37,    38,    39,   135,   136,   137,   138,
     139,   140,   135,   136,   137,   138,   139,   140,   176,   141,
     142,   143,     0,   177,     0,   141,   142,   143,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,    31,    32,
     136,   137,   138,   139,   140,     0,     0,     0,     0,     0,
       0,     0,   141,   142,   143,     0,     0,     0,    33,    34,
       0,   177,   176,    37,    38,    39,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,    31,    32,     0,   267,
     176,   147,   148,   149,   150,   151,   152,   153,   154,     0,
       0,     0,     0,     0,     0,     0,    33,    34,     0,     0,
     461,    37,    38,    39,     0,   177,     0,     0,     0,     0,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
      31,    32,     0,   177,   176,     0,     0,     0,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,    31,    32,
      33,    34,     0,     0,     0,    37,    38,    39,   160,   161,
     162,   163,   164,   165,   166,   167,     0,     0,   188,    34,
       0,     0,     0,    37,    38,    39,     0,   177,     0,   380,
       0,     0,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,    31,    32,   135,   136,   137,   138,   139,   140,
       0,     0,     0,     0,     0,   381,     0,   141,   142,   143,
       0,   382,    33,    34,     0,     0,     0,    37,    38,    39,
     135,   136,   137,   138,   139,   140,   135,   136,   137,   138,
     139,   140,   383,   141,   142,   143,     0,     0,   384,   141,
     142,   143,     0,     0,     0,     0,     0,   135,   136,   137,
     138,   139,   140,   135,   136,   137,   138,   139,   140,   385,
     141,   142,   143,     0,     0,   511,   141,   142,   143,     0,
       0,     0,     0,     0,   135,   136,   137,   138,   139,   140,
     135,   136,   137,   138,   139,   140,     0,   141,   142,   143,
       0,     0,     0,   141,   142,   143
};

static const yytype_int16 yycheck[] =
{
       2,   108,     8,     9,   446,   158,   159,   194,    20,     6,
      10,   158,    10,   342,     7,    11,     5,    10,   171,   142,
     143,   153,   154,   352,   171,     6,     6,    84,     7,     7,
       7,    10,    10,    10,   166,   167,     6,     1,    88,   379,
     380,     7,   382,     9,     2,    50,    51,    52,    48,    49,
      48,    49,    48,    49,     8,    48,    49,     6,     8,    10,
       6,    19,   402,   403,   404,   405,     7,    64,    65,    48,
      49,    67,    68,    75,    76,     7,     0,     1,     2,    43,
       2,     6,    48,    49,    64,    65,     6,    84,    85,    86,
      87,     6,    89,    90,    91,    19,     7,    48,    49,    10,
     223,   224,     6,   545,    84,    85,    42,    48,    49,    89,
      90,    91,    48,    49,     6,   117,    48,    49,     6,    43,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
      84,    85,    86,    87,    84,    85,    86,    87,   580,   141,
     142,   143,     6,   145,   146,   147,   148,   149,   150,   151,
     152,   109,   110,   111,   112,   113,   114,   115,   160,   161,
     162,   163,   164,   165,   493,   177,    50,    51,    52,   509,
       6,   511,    84,    85,   176,    84,    85,   135,   136,   137,
     138,   139,   140,     6,   108,   109,   110,   111,   112,   113,
     114,   115,   194,     7,   381,   117,   349,     0,   200,   201,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     199,   135,   136,   137,   138,   139,   140,   219,    50,    51,
      52,   223,   224,   225,   226,   147,   148,   149,   150,   151,
     152,     6,    41,    10,    48,    49,    30,     6,   160,   161,
     162,   163,   164,   165,    38,    39,    40,    22,    66,    67,
       7,     5,     7,    10,     7,    10,    31,    32,    33,    34,
      35,    36,    37,   265,   266,     7,    41,     6,    10,    66,
      67,    48,    49,     6,    49,    84,    85,    86,    87,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    48,    49,    48,    49,    48,    49,    84,    85,    86,
      87,     7,   409,   410,    10,     7,    48,    49,    10,    84,
      85,    86,    87,     6,    89,    90,    91,    53,     7,     9,
      10,    10,     7,    40,     7,    10,     6,    10,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
       6,   343,    48,    49,     9,    10,    48,    49,     6,   351,
       6,   353,   354,   355,     7,   357,     7,    53,   364,    48,
      49,   363,   368,    48,    49,    48,    49,     7,   370,   371,
      10,    10,     7,     4,     7,    10,   378,   379,   380,   381,
     382,     7,    11,     9,   386,   387,   388,   389,     7,    42,
      43,    44,    45,    46,    47,    48,    49,    48,    49,    11,
     402,   403,   404,   405,     7,    11,     9,     6,    48,    49,
     412,     6,   414,    48,    49,    48,    49,    42,   342,    48,
      49,     7,    48,    49,    10,   383,   384,   385,   352,    48,
      49,    48,    49,     7,    48,    49,    48,    49,     7,   441,
      10,    10,    48,    49,   446,    48,    49,    48,    49,   451,
       7,     9,    10,   455,   456,    10,   458,    10,   460,   383,
     384,   385,    48,    49,   386,   387,   388,   389,    42,    43,
      44,    45,    46,    47,    48,    49,     0,     1,     2,    48,
      49,   483,   484,   485,     7,   409,   410,    10,   490,   491,
     492,    48,    49,    40,   496,    19,    40,   499,   500,    53,
       7,     9,    10,    10,    38,    39,    40,   509,     6,   511,
       7,     7,     7,    10,     7,   142,   143,     7,     6,    43,
       7,     7,     7,    42,     7,    48,    49,   142,   143,     7,
     532,   533,     6,   535,    10,   537,     6,     6,     0,     1,
       2,    48,    49,   545,    11,   547,     6,   549,     6,   551,
       6,    48,    49,    48,    49,    48,    49,    19,    48,    49,
       6,    48,    49,    48,    49,    48,    49,     7,     6,   493,
      48,    49,    48,    49,   576,     6,   578,     7,   580,     7,
     582,    43,   584,     6,   586,   109,   110,   111,   112,   113,
     114,   115,    10,   117,     6,     6,    10,     6,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,    48,    49,
      42,   135,   136,   137,   138,   139,   140,     7,    48,    49,
      48,    49,    10,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,    48,    49,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   109,   110,   111,
     112,   113,   114,   115,    15,   117,     7,     7,    48,    49,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     483,   484,   485,   135,   136,   137,   138,   139,   140,     7,
      10,     7,     7,     7,     7,   147,   148,   149,   150,   151,
     152,   153,   154,     7,     0,     1,     2,     7,   160,   161,
     162,   163,   164,   165,   166,   167,     7,     7,    10,     7,
       9,     3,     7,    19,     6,   266,    10,     7,     9,     7,
      12,    13,    48,    49,    16,    17,    18,    19,    20,    21,
      22,     9,     6,    93,    48,    49,     7,    43,     7,    31,
      32,    33,    34,    35,    36,    37,     7,    48,    49,    41,
      48,    49,     7,    48,    49,   365,   496,    49,    48,    49,
      48,    49,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,   441,    49,   218,    48,    49,     7,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    84,    85,    86,    87,   199,    89,    90,    91,
       6,   222,   108,   109,   110,   111,   112,   113,   114,   115,
      84,    85,    86,    87,     7,    89,    90,    91,   342,    10,
      48,    49,   346,   347,   348,   202,    96,   354,   352,   135,
     136,   137,   138,   139,   140,     7,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    10,    20,
       6,    -1,     8,     7,    -1,    48,    49,    48,    49,   383,
     384,   385,   386,   387,   388,   389,    -1,    23,    24,     7,
      -1,     7,    -1,     7,    -1,     7,    48,    49,     7,    -1,
     342,    -1,   344,   345,    -1,    -1,    48,    49,     6,    -1,
     352,    -1,    -1,    49,    48,    49,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      48,    49,    48,    49,    48,    49,    48,    49,     7,    48,
      49,   383,   384,   385,   386,   387,   388,   389,    84,    85,
      -1,    49,    -1,    89,    90,    91,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,     7,    10,
       6,    69,     8,     7,    -1,    -1,    -1,    -1,    10,    48,
      49,    -1,    -1,    -1,    -1,    83,    84,    85,    -1,   493,
      -1,    89,    90,    91,    10,    -1,    10,    -1,    10,    -1,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    48,
      49,    -1,    -1,    49,    48,    49,    48,    49,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
       6,    -1,    48,    49,    48,    49,    48,    49,    48,    49,
      -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,   493,    -1,    89,    90,    91,   342,    25,    26,    27,
      28,    29,    30,    -1,    -1,    -1,   352,    -1,    -1,    -1,
      38,    39,    40,    49,    -1,    -1,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
       6,    -1,     8,    28,    29,    30,     5,   383,   384,   385,
      -1,    29,    30,    38,    39,    40,    -1,    83,    84,    85,
      38,    39,    40,    89,    90,    91,    25,    26,    27,    28,
      29,    30,    -1,   409,   410,    -1,    -1,    -1,    -1,    38,
      39,    40,    -1,    49,     6,    -1,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    -1,    24,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,   155,   156,   157,    84,    85,
      -1,    -1,     6,    89,    90,    91,    -1,    49,   168,   169,
     170,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,   493,    -1,    -1,
       6,    -1,    84,    85,     7,    49,    -1,    89,    90,    91,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    25,    26,    27,    28,    29,    30,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    39,    40,    -1,    83,
      84,    85,    -1,    49,    -1,    89,    90,    91,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
       6,    -1,     8,    -1,    -1,    -1,    -1,    -1,     7,    27,
      28,    29,    30,    -1,     7,    -1,    -1,    83,    84,    85,
      38,    39,    40,    89,    90,    91,    25,    26,    27,    28,
      29,    30,    25,    26,    27,    28,    29,    30,     6,    38,
      39,    40,    -1,    49,    -1,    38,    39,    40,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    38,    39,    40,    -1,    -1,    -1,    84,    85,
      -1,    49,     6,    89,    90,    91,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    -1,    23,
       6,    42,    43,    44,    45,    46,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    -1,
      88,    89,    90,    91,    -1,    49,    -1,    -1,    -1,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    49,     6,    -1,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      84,    85,    -1,    -1,    -1,    89,    90,    91,    42,    43,
      44,    45,    46,    47,    48,    49,    -1,    -1,    84,    85,
      -1,    -1,    -1,    89,    90,    91,    -1,    49,    -1,    10,
      -1,    -1,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    25,    26,    27,    28,    29,    30,
      -1,    -1,    -1,    -1,    -1,    10,    -1,    38,    39,    40,
      -1,    10,    84,    85,    -1,    -1,    -1,    89,    90,    91,
      25,    26,    27,    28,    29,    30,    25,    26,    27,    28,
      29,    30,    10,    38,    39,    40,    -1,    -1,    10,    38,
      39,    40,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,
      28,    29,    30,    25,    26,    27,    28,    29,    30,    10,
      38,    39,    40,    -1,    -1,    10,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,    25,    26,    27,    28,    29,    30,
      25,    26,    27,    28,    29,    30,    -1,    38,    39,    40,
      -1,    -1,    -1,    38,    39,    40
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     6,    12,    13,    16,    17,    18,    19,    20,
      21,    22,    31,    32,    33,    34,    35,    36,    37,    41,
      49,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    84,    85,    86,    87,    89,    90,    91,
      93,    94,    95,    96,    97,    98,    99,   100,   106,   118,
     119,   120,   126,   127,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,    95,    97,   138,   145,     6,     6,    84,   112,   115,
     124,   131,    88,   111,   132,     8,    84,    85,    86,    87,
     109,   121,   128,     8,   109,   109,    41,    84,    85,    86,
      87,   110,   116,   117,   122,   123,   129,   130,     6,     6,
       6,     6,     6,     6,     6,     6,    97,     6,   119,   126,
     143,   150,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     0,    95,     5,    25,    26,    27,    28,    29,
      30,    38,    39,    40,     5,     6,     6,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    40,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,     4,     7,     7,     7,     6,    49,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    84,   124,
     134,   145,   146,   147,    69,    83,   135,   136,   145,    10,
      42,     6,    10,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,   107,   108,    10,     6,
     107,   122,    10,    40,    40,     6,     6,   118,   119,   126,
      97,    97,    97,    97,    97,    97,    97,   138,   145,   138,
     145,   138,   145,   138,   145,   138,   145,   138,   145,   138,
     145,   138,   145,   138,   145,   138,   145,   138,   145,    97,
      97,    97,    97,    97,    97,     6,     8,    23,    24,   101,
     102,   103,   104,   105,   145,   103,   104,   105,   145,   103,
     104,   105,   145,   145,   145,   138,   145,   138,   145,   138,
     145,   138,   145,   138,   145,   138,   145,   139,   146,   139,
     146,   140,   147,   140,   147,   140,   147,   141,   148,   148,
     138,   145,   138,   138,   138,   138,   138,   139,   146,   139,
     146,   140,   147,   140,   147,   140,   147,   141,   148,   145,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
      66,    67,     7,    11,    48,    49,    50,    51,    52,    53,
     135,    11,     7,    11,    67,    68,   115,     8,   145,    83,
     125,   145,   132,    42,     9,    10,   121,   145,     9,   116,
       6,     8,   104,   145,   104,   145,   145,   145,    10,    10,
      10,    10,    10,    10,    10,    10,    10,    10,    10,    10,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,    10,    10,    10,    10,   145,   102,   145,     6,
       6,     7,    10,     7,    10,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,    96,   145,   146,
     146,   147,   147,   147,   145,    96,    83,   145,   136,   145,
     145,     8,   113,   114,   145,     7,    10,   145,   109,   108,
       7,    10,   109,   145,   145,    42,    42,     7,    10,     7,
      10,    88,   133,   145,   134,   134,   135,   134,    97,    97,
      97,   138,   145,   138,   145,   138,   145,   138,   145,   134,
     134,   134,   134,    10,    10,    10,   118,   118,   145,   145,
      10,    10,    11,    15,   113,     9,    10,   125,   145,    10,
      10,   145,   145,   145,   145,     7,     7,     7,     7,    10,
       7,    10,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,   101,   145,   101,   145,
     101,   145,    10,    10,     7,    10,     7,    10,   145,   145,
     145,    96,     9,   114,     7,    10,     7,    10,     7,    10,
       7,    10,   134,   134,     9,     7,     9,     9,     7,     9,
       9,     7,     9,   145,   145,   145,   145,   125,   145,   145,
     145,     7,     7,     7,     7,     7,    10,     7,    10,     7,
      10,     7,    10,     7,    10,     7,    10,   145,   145,   125,
     145,   145,   145,     7,     7,     7,     7,     7,     7
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    92,    93,    94,    94,    95,    95,    96,    96,    96,
      96,    96,    96,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    97,    98,    98,    98,    98,    99,
      99,    99,    99,   100,   100,   100,   100,   101,   102,   103,
     103,   104,   104,   104,   104,   105,   105,   105,   105,   105,
     106,   106,   106,   106,   106,   106,   106,   106,   107,   107,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   109,   109,   110,   110,   111,   111,   112,
     112,   113,   113,   114,   114,   115,   115,   116,   116,   116,
     116,   117,   117,   118,   118,   119,   119,   119,   119,   120,
     120,   120,   120,   120,   121,   121,   121,   121,   121,   122,
     122,   122,   122,   122,   123,   123,   123,   123,   123,   124,
     124,   124,   124,   124,   125,   125,   126,   126,   127,   127,
     128,   128,   128,   128,   129,   129,   130,   130,   131,   132,
     133,   134,   134,   134,   134,   135,   135,   135,   135,   135,
     136,   136,   136,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   138,   138,   138,   138,   138,   138,   138,   139,   139,
     139,   139,   139,   139,   139,   139,   139,   139,   140,   140,
     140,   140,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   142,   142,   143,
     143,   144,   144,   145,   145,   145,   146,   146,   146,   146,
     147,   147,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   149,   149,   150,   150,   151,   151,   151,
     151,   151,   152,   152,   152,   153
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     0,     2,     1,     5,     5,     7,
       3,     2,     2,     2,     3,     3,     6,     6,     6,     6,
       8,     6,     8,     3,     3,     3,     3,     6,     6,     3,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     6,     1,
       1,     5,     5,     5,     5,     5,     5,     5,     5,     5,
       5,     2,     5,     2,     2,     2,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     5,     3,     3,     5,
       5,     2,     1,     1,     1,    10,     8,     6,     4,    10,
       8,     6,     4,     1,    10,     8,     6,     4,     1,    10,
       8,     6,     4,     1,    10,     8,     6,     4,     1,    10,
       8,     6,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     5,     3,     3,     1,     2,     3,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     3,     3,
       3,     1,     6,     6,     6,     6,     6,     6,     4,     4,
       4,     4,     4,     4,     6,     6,     1,     2,     1,     3,
       1,     3,     1,     3,     3,     1,     3,     3,     3,     1,
       3,     1,     6,     6,     4,     4,     4,     4,     4,     4,
       6,     6,     1,     2,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, spec_handle, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, spec_handle, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, btl_specification **spec_handle, yyscan_t scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (spec_handle);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, btl_specification **spec_handle, yyscan_t scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, spec_handle, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, btl_specification **spec_handle, yyscan_t scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), spec_handle, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, spec_handle, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, btl_specification **spec_handle, yyscan_t scanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (spec_handle);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (btl_specification **spec_handle, yyscan_t scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* input: specification  */
#line 251 "ting.y"
                       { *spec_handle = (yyvsp[0].other); }
#line 2245 "ting_parser.c"
    break;

  case 3: /* specification: blocklist  */
#line 255 "ting.y"
                   { (yyval.other) = (yyvsp[0].other); }
#line 2251 "ting_parser.c"
    break;

  case 5: /* blocklist: block blocklist  */
#line 260 "ting.y"
                            { (yyval.other) = create_operation(op_join, (yyvsp[-1].other), (yyvsp[0].other), "%s ; %s"); }
#line 2257 "ting_parser.c"
    break;

  case 6: /* blocklist: block  */
#line 261 "ting.y"
               { (yyval.other) = (yyvsp[0].other); }
#line 2263 "ting_parser.c"
    break;

  case 7: /* block: TOKEN_ITER TOKEN_LPAREN varrange TOKEN_RPAREN block  */
#line 265 "ting.y"
                                                                { (yyval.other) = create_operation(op_iter, (yyvsp[0].other), (yyvsp[-2].other), "iter(%2$s) { %1$s ; }"); }
#line 2269 "ting_parser.c"
    break;

  case 8: /* block: TOKEN_WHEN TOKEN_LPAREN selrange TOKEN_RPAREN block  */
#line 266 "ting.y"
                                                                                  { (yyval.other) = create_operation(op_when, (yyvsp[0].other), (yyvsp[-2].other), "when(%2$s) { %1$s ; }"); }
#line 2275 "ting_parser.c"
    break;

  case 9: /* block: TOKEN_WHEN TOKEN_LPAREN selrange TOKEN_RPAREN block TOKEN_ELSE block  */
#line 268 "ting.y"
      { (yyval.other) = create_operation(op_join, create_operation(op_when, (yyvsp[-2].other), (yyvsp[-4].other), "when(%2$s) { %1$s ; }"),
                                       create_operation(op_when, (yyvsp[0].other), create_operation(op_neg_range, copy_specification((yyvsp[-4].other)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 2282 "ting_parser.c"
    break;

  case 10: /* block: TOKEN_LBRACKET blocklist TOKEN_RBRACKET  */
#line 270 "ting.y"
                                                 { (yyval.other) = (yyvsp[-1].other); }
#line 2288 "ting_parser.c"
    break;

  case 11: /* block: formula TOKEN_SEMICOLON  */
#line 271 "ting.y"
                                 { (yyval.other) = (yyvsp[-1].other); }
#line 2294 "ting_parser.c"
    break;

  case 12: /* block: declaration TOKEN_SEMICOLON  */
#line 272 "ting.y"
                                     { (yyval.other) = (yyvsp[-1].other); }
#line 2300 "ting_parser.c"
    break;

  case 13: /* formula: TOKEN_NOT formula  */
#line 276 "ting.y"
                           { (yyval.other) = create_operation(op_not, (yyvsp[0].other), NULL, "(~ %s)"); }
#line 2306 "ting_parser.c"
    break;

  case 14: /* formula: formula TOKEN_AND formula  */
#line 277 "ting.y"
                                      { (yyval.other) = create_operation(op_and, (yyvsp[-2].other), (yyvsp[0].other), "(%s & %s)"); }
#line 2312 "ting_parser.c"
    break;

  case 15: /* formula: formula TOKEN_OR formula  */
#line 278 "ting.y"
                                     { (yyval.other) = create_operation(op_or, (yyvsp[-2].other), (yyvsp[0].other), "(%s | %s)"); }
#line 2318 "ting_parser.c"
    break;

  case 16: /* formula: TOKEN_SINCE TOKEN_LPAREN formula TOKEN_COMMA formula TOKEN_RPAREN  */
#line 279 "ting.y"
                                                                              { (yyval.other) = create_operation(op_since, (yyvsp[-3].other), (yyvsp[-1].other), "since(%s , %s)"); }
#line 2324 "ting_parser.c"
    break;

  case 17: /* formula: TOKEN_UNTIL TOKEN_LPAREN formula TOKEN_COMMA formula TOKEN_RPAREN  */
#line 280 "ting.y"
                                                                              { (yyval.other) = create_operation(op_until, (yyvsp[-3].other), (yyvsp[-1].other), "until(%s , %s)"); }
#line 2330 "ting_parser.c"
    break;

  case 18: /* formula: TOKEN_FORALL TOKEN_LPAREN formula TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 281 "ting.y"
                                                                                { (yyval.other) = create_operation(op_forall, (yyvsp[-3].other), (yyvsp[-1].other), "forall(%s , %s)"); }
#line 2336 "ting_parser.c"
    break;

  case 19: /* formula: TOKEN_EXISTS TOKEN_LPAREN formula TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 282 "ting.y"
                                                                                { (yyval.other) = create_operation(op_exists, (yyvsp[-3].other), (yyvsp[-1].other), "exists(%s , %s)"); }
#line 2342 "ting_parser.c"
    break;

  case 20: /* formula: TOKEN_ONE TOKEN_LPAREN formula TOKEN_COMMA selrange TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 283 "ting.y"
                                                                                                     { (yyval.other) = create_operation(op_one, (yyvsp[-5].other),
                                                                                                            create_operation(op_one_check, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "one(%s , %s)"); }
#line 2349 "ting_parser.c"
    break;

  case 21: /* formula: TOKEN_UNIQUE TOKEN_LPAREN formula TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 285 "ting.y"
                                                                                { (yyval.other) = create_operation(op_unique, (yyvsp[-3].other), (yyvsp[-1].other), "unique(%s , %s)"); }
#line 2355 "ting_parser.c"
    break;

  case 22: /* formula: TOKEN_COMBINE TOKEN_LPAREN formula TOKEN_COMMA formula TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 286 "ting.y"
                                                                                                        { (yyval.other) = create_operation(op_combine,
      create_operation(op_comb_pair, (yyvsp[-5].other), (yyvsp[-3].other), "%s , %s"), (yyvsp[-1].other), "combine(%s , %s)"); }
#line 2362 "ting_parser.c"
    break;

  case 23: /* formula: formula TOKEN_XOR formula  */
#line 288 "ting.y"
                                      { (yyval.other) = create_operation(op_xor, (yyvsp[-2].other), (yyvsp[0].other), "(%s \\ %s)"); }
#line 2368 "ting_parser.c"
    break;

  case 24: /* formula: formula TOKEN_IMPLY formula  */
#line 289 "ting.y"
                                        { (yyval.other) = create_operation(op_imply, (yyvsp[-2].other), (yyvsp[0].other), "(%s --> %s)"); }
#line 2374 "ting_parser.c"
    break;

  case 25: /* formula: formula TOKEN_INVIMPLY formula  */
#line 290 "ting.y"
                                           { (yyval.other) = create_operation(op_imply, (yyvsp[0].other), (yyvsp[-2].other), "(%2$s <-- %1$s)"); }
#line 2380 "ting_parser.c"
    break;

  case 26: /* formula: formula TOKEN_EQV formula  */
#line 291 "ting.y"
                                      { (yyval.other) = create_operation(op_eqv, (yyvsp[-2].other), (yyvsp[0].other), "(%s == %s)"); }
#line 2386 "ting_parser.c"
    break;

  case 27: /* formula: TOKEN_CODE TOKEN_LPAREN varidxname TOKEN_COMMA string TOKEN_RPAREN  */
#line 292 "ting.y"
                                                                               { (yyval.other) = create_operation(op_code, (yyvsp[-3].other), (yyvsp[-1].other), "code(%s , \"%s\")"); }
#line 2392 "ting_parser.c"
    break;

  case 28: /* formula: TOKEN_CODE TOKEN_LPAREN varidxname TOKEN_COMMA expression TOKEN_RPAREN  */
#line 293 "ting.y"
                                                                                   { (yyval.other) = create_operation(op_code_num, (yyvsp[-3].other), (yyvsp[-1].other), "code(%s , %s)"); }
#line 2398 "ting_parser.c"
    break;

  case 29: /* formula: TOKEN_LPAREN formula TOKEN_RPAREN  */
#line 294 "ting.y"
                                           { (yyval.other) = (yyvsp[-1].other); }
#line 2404 "ting_parser.c"
    break;

  case 30: /* formula: atconstruct  */
#line 295 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 2410 "ting_parser.c"
    break;

  case 31: /* formula: happenconstruct  */
#line 296 "ting.y"
                         { (yyval.other) = (yyvsp[0].other); }
#line 2416 "ting_parser.c"
    break;

  case 32: /* formula: onlyconstruct  */
#line 297 "ting.y"
                       { (yyval.other) = (yyvsp[0].other); }
#line 2422 "ting_parser.c"
    break;

  case 33: /* formula: mathrelation  */
#line 298 "ting.y"
                      { (yyval.other) = (yyvsp[0].other); }
#line 2428 "ting_parser.c"
    break;

  case 34: /* formula: varidxname  */
#line 299 "ting.y"
                    { (yyval.other) = (yyvsp[0].other); }
#line 2434 "ting_parser.c"
    break;

  case 35: /* atconstruct: formula TOKEN_AT expression  */
#line 303 "ting.y"
                                        { (yyval.other) = create_operation(op_delay, (yyvsp[-2].other), (yyvsp[0].other), "(%s @ %s)"); }
#line 2440 "ting_parser.c"
    break;

  case 36: /* atconstruct: formula TOKEN_AT vartime  */
#line 304 "ting.y"
                                     { (yyval.other) = create_operation(op_com_delay, (yyvsp[-2].other), (yyvsp[0].other), "(%s @ %s)"); }
#line 2446 "ting_parser.c"
    break;

  case 37: /* atconstruct: formula TOKEN_AT interval  */
#line 305 "ting.y"
                                      { (yyval.other) = create_operation(op_at, (yyvsp[-2].other), (yyvsp[0].other), "(%s @ %s)"); }
#line 2452 "ting_parser.c"
    break;

  case 38: /* atconstruct: formula TOKEN_AT varinterval  */
#line 306 "ting.y"
                                         { (yyval.other) = create_operation(op_com_at, (yyvsp[-2].other), (yyvsp[0].other), "(%s @ %s)"); }
#line 2458 "ting_parser.c"
    break;

  case 39: /* happenconstruct: formula TOKEN_HAPPEN expression  */
#line 310 "ting.y"
                                            { (yyval.other) = create_operation(op_delay, (yyvsp[-2].other), (yyvsp[0].other), "(%s ? %s)"); }
#line 2464 "ting_parser.c"
    break;

  case 40: /* happenconstruct: formula TOKEN_HAPPEN vartime  */
#line 311 "ting.y"
                                         { (yyval.other) = create_operation(op_com_delay, (yyvsp[-2].other), (yyvsp[0].other), "(%s ? %s)"); }
#line 2470 "ting_parser.c"
    break;

  case 41: /* happenconstruct: formula TOKEN_HAPPEN interval  */
#line 312 "ting.y"
                                          { (yyval.other) = create_operation(op_happen, (yyvsp[-2].other), (yyvsp[0].other), "(%s ? %s)"); }
#line 2476 "ting_parser.c"
    break;

  case 42: /* happenconstruct: formula TOKEN_HAPPEN varinterval  */
#line 313 "ting.y"
                                             { (yyval.other) = create_operation(op_com_happen, (yyvsp[-2].other), (yyvsp[0].other), "(%s ? %s)"); }
#line 2482 "ting_parser.c"
    break;

  case 43: /* onlyconstruct: formula TOKEN_ONLY expression  */
#line 317 "ting.y"
                                          { (yyval.other) = create_operation(op_delay, (yyvsp[-2].other), (yyvsp[0].other), "(%s ! %s)"); }
#line 2488 "ting_parser.c"
    break;

  case 44: /* onlyconstruct: formula TOKEN_ONLY vartime  */
#line 318 "ting.y"
                                       { (yyval.other) = create_operation(op_com_delay, (yyvsp[-2].other), (yyvsp[0].other), "(%s ! %s)"); }
#line 2494 "ting_parser.c"
    break;

  case 45: /* onlyconstruct: formula TOKEN_ONLY interval  */
#line 319 "ting.y"
                                        { (yyval.other) = create_operation(op_only, (yyvsp[-2].other), (yyvsp[0].other), "(%s ! %s)"); }
#line 2500 "ting_parser.c"
    break;

  case 46: /* onlyconstruct: formula TOKEN_ONLY varinterval  */
#line 320 "ting.y"
                                           { (yyval.other) = create_operation(op_com_only, (yyvsp[-2].other), (yyvsp[0].other), "(%s ! %s)"); }
#line 2506 "ting_parser.c"
    break;

  case 47: /* postime: TOKEN_PVALUE TOKEN_LPAREN varidxname TOKEN_COMMA expression TOKEN_RPAREN  */
#line 324 "ting.y"
                                                                                     { (yyval.other) = create_operation(op_pvalue, (yyvsp[-3].other), (yyvsp[-1].other), "+$(%s , %s)"); }
#line 2512 "ting_parser.c"
    break;

  case 48: /* negtime: TOKEN_NVALUE TOKEN_LPAREN varidxname TOKEN_COMMA expression TOKEN_RPAREN  */
#line 328 "ting.y"
                                                                                     { (yyval.other) = create_operation(op_nvalue, (yyvsp[-3].other), (yyvsp[-1].other), "-$(%s , %s)"); }
#line 2518 "ting_parser.c"
    break;

  case 49: /* vartime: postime  */
#line 332 "ting.y"
                 { (yyval.other) = (yyvsp[0].other); }
#line 2524 "ting_parser.c"
    break;

  case 50: /* vartime: negtime  */
#line 333 "ting.y"
                 { (yyval.other) = (yyvsp[0].other); }
#line 2530 "ting_parser.c"
    break;

  case 51: /* interval: TOKEN_LSQUARED expression TOKEN_COMMA expression TOKEN_RSQUARED  */
#line 337 "ting.y"
                                                                            { (yyval.other) = create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "[%s , %s]"); }
#line 2536 "ting_parser.c"
    break;

  case 52: /* interval: TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RSQUARED  */
#line 338 "ting.y"
                                                                          { (yyval.other) = create_operation(op_interval_2, (yyvsp[-3].other), (yyvsp[-1].other), "(%s , %s]"); }
#line 2542 "ting_parser.c"
    break;

  case 53: /* interval: TOKEN_LSQUARED expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 339 "ting.y"
                                                                          { (yyval.other) = create_operation(op_interval_3, (yyvsp[-3].other), (yyvsp[-1].other), "[%s , %s)"); }
#line 2548 "ting_parser.c"
    break;

  case 54: /* interval: TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 340 "ting.y"
                                                                        { (yyval.other) = create_operation(op_interval_4, (yyvsp[-3].other), (yyvsp[-1].other), "(%s , %s)"); }
#line 2554 "ting_parser.c"
    break;

  case 55: /* varinterval: TOKEN_LSQUARED expression TOKEN_COMMA postime TOKEN_RSQUARED  */
#line 344 "ting.y"
                                                                         { (yyval.other) = create_operation(op_varinterval_1, (yyvsp[-3].other), (yyvsp[-1].other), "[%s , %s]"); }
#line 2560 "ting_parser.c"
    break;

  case 56: /* varinterval: TOKEN_LPAREN expression TOKEN_COMMA postime TOKEN_RSQUARED  */
#line 345 "ting.y"
                                                                       { (yyval.other) = create_operation(op_varinterval_2, (yyvsp[-3].other), (yyvsp[-1].other), "(%s , %s]"); }
#line 2566 "ting_parser.c"
    break;

  case 57: /* varinterval: TOKEN_LSQUARED negtime TOKEN_COMMA expression TOKEN_RSQUARED  */
#line 346 "ting.y"
                                                                         { (yyval.other) = create_operation(op_varinterval_3, (yyvsp[-3].other), (yyvsp[-1].other), "[%s , %s]"); }
#line 2572 "ting_parser.c"
    break;

  case 58: /* varinterval: TOKEN_LSQUARED negtime TOKEN_COMMA expression TOKEN_RPAREN  */
#line 347 "ting.y"
                                                                       { (yyval.other) = create_operation(op_varinterval_4, (yyvsp[-3].other), (yyvsp[-1].other), "[%s , %s)"); }
#line 2578 "ting_parser.c"
    break;

  case 59: /* varinterval: TOKEN_LSQUARED negtime TOKEN_COMMA postime TOKEN_RSQUARED  */
#line 348 "ting.y"
                                                                      { (yyval.other) = create_operation(op_varinterval_5, (yyvsp[-3].other), (yyvsp[-1].other), "[%s , %s]"); }
#line 2584 "ting_parser.c"
    break;

  case 60: /* declaration: TOKEN_INPUT TOKEN_LSQUARED iodecllist TOKEN_RSQUARED decllist  */
#line 352 "ting.y"
                                                                          { (yyval.other) = create_operation(op_input, (yyvsp[0].other), (yyvsp[-2].other), "input [%2$s] %1$s"); }
#line 2590 "ting_parser.c"
    break;

  case 61: /* declaration: TOKEN_INPUT decllist  */
#line 353 "ting.y"
                              { (yyval.other) = create_operation(op_input, (yyvsp[0].other), create_operation(op_join_qual, create_operation(op_join_qual, create_operation(op_join_qual,
      create_ground(op_ioqual1, "", io_any, REAL_MAX), create_ground(op_ioqual2, "", io_binary, REAL_MAX), "%s , %s"), create_ground(op_ioqual3, "", io_unknown, REAL_MAX), "%s , %s"),
      create_ground(op_ioqual4, "", io_raw, REAL_MAX), "%s , %s"), "input [%2$s] %1$s"); }
#line 2598 "ting_parser.c"
    break;

  case 62: /* declaration: TOKEN_OUTPUT TOKEN_LSQUARED iodecllist TOKEN_RSQUARED decllist  */
#line 356 "ting.y"
                                                                           { (yyval.other) = create_operation(op_output, (yyvsp[0].other), (yyvsp[-2].other), "output [%2$s] %1$s"); }
#line 2604 "ting_parser.c"
    break;

  case 63: /* declaration: TOKEN_OUTPUT decllist  */
#line 357 "ting.y"
                               { (yyval.other) = create_operation(op_output, (yyvsp[0].other), create_operation(op_join_qual, create_operation(op_join_qual, create_operation(op_join_qual,
      create_ground(op_ioqual1, "", io_any, REAL_MAX), create_ground(op_ioqual2, "", io_binary, REAL_MAX), "%s , %s"), create_ground(op_ioqual3, "", io_unknown, REAL_MAX), "%s , %s"),
      create_ground(op_ioqual4, "", io_raw, REAL_MAX), "%s , %s"), "output [%2$s] %1$s"); }
#line 2612 "ting_parser.c"
    break;

  case 64: /* declaration: TOKEN_AUX decllist  */
#line 360 "ting.y"
                            { (yyval.other) = create_operation(op_aux, (yyvsp[0].other), NULL, "aux %s"); }
#line 2618 "ting_parser.c"
    break;

  case 65: /* declaration: TOKEN_INIT eventlist  */
#line 361 "ting.y"
                              { (yyval.other) = create_operation(op_init, (yyvsp[0].other), NULL, "init %s"); }
#line 2624 "ting_parser.c"
    break;

  case 66: /* declaration: TOKEN_DEFINE assignlist  */
#line 362 "ting.y"
                                 { (yyval.other) = (yyvsp[0].other); }
#line 2630 "ting_parser.c"
    break;

  case 67: /* declaration: TOKEN_INCLUDE filelist  */
#line 363 "ting.y"
                                { (yyval.other) = (yyvsp[0].other); }
#line 2636 "ting_parser.c"
    break;

  case 68: /* iodecllist: iodecllist TOKEN_COMMA iodecl  */
#line 367 "ting.y"
                                          { (yyval.other) = create_operation(op_join_qual, (yyvsp[-2].other), (yyvsp[0].other), "%s , %s"); }
#line 2642 "ting_parser.c"
    break;

  case 69: /* iodecllist: iodecl  */
#line 368 "ting.y"
                { (yyval.other) = (yyvsp[0].other); }
#line 2648 "ting_parser.c"
    break;

  case 70: /* iodecl: TOKEN_ANY  */
#line 372 "ting.y"
                { (yyval.other) = create_ground(op_ioqual1, "any", io_any, REAL_MAX); }
#line 2654 "ting_parser.c"
    break;

  case 71: /* iodecl: TOKEN_IPC  */
#line 373 "ting.y"
                { (yyval.other) = create_ground(op_ioqual1, "ipc", io_ipc, REAL_MAX); }
#line 2660 "ting_parser.c"
    break;

  case 72: /* iodecl: TOKEN_FILE  */
#line 374 "ting.y"
                 { (yyval.other) = create_ground(op_ioqual1, "file", io_file, REAL_MAX); }
#line 2666 "ting_parser.c"
    break;

  case 73: /* iodecl: TOKEN_REMOTE  */
#line 375 "ting.y"
                   { (yyval.other) = create_ground(op_ioqual1, "remote", io_socket, REAL_MAX); }
#line 2672 "ting_parser.c"
    break;

  case 74: /* iodecl: TOKEN_BINARY  */
#line 376 "ting.y"
                   { (yyval.other) = create_ground(op_ioqual2, "binary", io_binary, REAL_MAX); }
#line 2678 "ting_parser.c"
    break;

  case 75: /* iodecl: TOKEN_PACKED  */
#line 377 "ting.y"
                   { (yyval.other) = create_ground(op_ioqual2, "packed", io_packed, REAL_MAX); }
#line 2684 "ting_parser.c"
    break;

  case 76: /* iodecl: TOKEN_UNKNOWN  */
#line 378 "ting.y"
                    { (yyval.other) = create_ground(op_ioqual3, "unknown", io_unknown, REAL_MAX); }
#line 2690 "ting_parser.c"
    break;

  case 77: /* iodecl: TOKEN_FALSE  */
#line 379 "ting.y"
                  { (yyval.other) = create_ground(op_ioqual3, "false", io_false, REAL_MAX); }
#line 2696 "ting_parser.c"
    break;

  case 78: /* iodecl: TOKEN_TRUE  */
#line 380 "ting.y"
                 { (yyval.other) = create_ground(op_ioqual3, "true", io_true, REAL_MAX); }
#line 2702 "ting_parser.c"
    break;

  case 79: /* iodecl: TOKEN_DEFAULT TOKEN_EQUAL expression  */
#line 381 "ting.y"
                                              { (yyval.other) = create_operation(op_ioqual3, (yyvsp[0].other), NULL, "default = %s"); }
#line 2708 "ting_parser.c"
    break;

  case 80: /* iodecl: TOKEN_RAW  */
#line 382 "ting.y"
                { (yyval.other) = create_ground(op_ioqual4, "raw", io_raw, REAL_MAX); }
#line 2714 "ting_parser.c"
    break;

  case 81: /* iodecl: TOKEN_FILTER  */
#line 383 "ting.y"
                   { (yyval.other) = create_ground(op_ioqual4, "filter", io_filter, REAL_MAX); }
#line 2720 "ting_parser.c"
    break;

  case 82: /* iodecl: TOKEN_OMIT  */
#line 384 "ting.y"
                 { (yyval.other) = create_ground(op_ioqual4, "omit", io_omit, REAL_MAX); }
#line 2726 "ting_parser.c"
    break;

  case 83: /* decllist: decllist TOKEN_COMMA declidxname  */
#line 388 "ting.y"
                                             { (yyval.other) = create_operation(op_join, (yyvsp[-2].other), (yyvsp[0].other), "%s , %s"); }
#line 2732 "ting_parser.c"
    break;

  case 84: /* decllist: declidxname  */
#line 389 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 2738 "ting_parser.c"
    break;

  case 85: /* eventlist: eventlist TOKEN_COMMA eventname  */
#line 393 "ting.y"
                                            { (yyval.other) = create_operation(op_join, (yyvsp[-2].other), (yyvsp[0].other), "%s , %s"); }
#line 2744 "ting_parser.c"
    break;

  case 86: /* eventlist: eventname  */
#line 394 "ting.y"
                   { (yyval.other) = (yyvsp[0].other); }
#line 2750 "ting_parser.c"
    break;

  case 87: /* filelist: filelist TOKEN_COMMA filename  */
#line 398 "ting.y"
                                          { (yyval.other) = create_operation(op_join, (yyvsp[-2].other), (yyvsp[0].other), "%s , %s"); }
#line 2756 "ting_parser.c"
    break;

  case 88: /* filelist: filename  */
#line 399 "ting.y"
                  { (yyval.other) = (yyvsp[0].other); }
#line 2762 "ting_parser.c"
    break;

  case 89: /* assignlist: assignlist TOKEN_COMMA assignment  */
#line 403 "ting.y"
                                              { (yyval.other) = create_operation(op_join, (yyvsp[-2].other), (yyvsp[0].other), "%s , %s"); }
#line 2768 "ting_parser.c"
    break;

  case 90: /* assignlist: assignment  */
#line 404 "ting.y"
                    { (yyval.other) = (yyvsp[0].other); }
#line 2774 "ting_parser.c"
    break;

  case 91: /* globveclist: globveclist TOKEN_COMMA globvector  */
#line 408 "ting.y"
                                               { (yyval.other) = create_operation(op_join_array, (yyvsp[-2].other), (yyvsp[0].other), "%s , %s"); }
#line 2780 "ting_parser.c"
    break;

  case 92: /* globveclist: globvector  */
#line 409 "ting.y"
                    { (yyval.other) = (yyvsp[0].other); }
#line 2786 "ting_parser.c"
    break;

  case 93: /* globvector: TOKEN_LSQUARED globveclist TOKEN_RSQUARED  */
#line 413 "ting.y"
                                                   { (yyval.other) = create_operation(op_down, (yyvsp[-1].other), NULL, "[%s]"); }
#line 2792 "ting_parser.c"
    break;

  case 94: /* globvector: expression  */
#line 414 "ting.y"
                    { (yyval.other) = (yyvsp[0].other); }
#line 2798 "ting_parser.c"
    break;

  case 95: /* assignment: constidxname TOKEN_EQUAL expression  */
#line 417 "ting.y"
                                               { (yyval.other) = create_operation(op_define, (yyvsp[-2].other), (yyvsp[0].other), "define %s = %s"); }
#line 2804 "ting_parser.c"
    break;

  case 96: /* assignment: constidxname TOKEN_EQUAL TOKEN_LSQUARED globveclist TOKEN_RSQUARED  */
#line 418 "ting.y"
                                                                              { (yyval.other) = create_operation(op_defarray, (yyvsp[-4].other), (yyvsp[-1].other), "define %s = [%s]"); }
#line 2810 "ting_parser.c"
    break;

  case 97: /* eventname: qualname TOKEN_AT expression  */
#line 422 "ting.y"
                                         { (yyval.other) = create_operation(op_var_at, (yyvsp[-2].other), (yyvsp[0].other), "%s @ %s"); }
#line 2816 "ting_parser.c"
    break;

  case 98: /* eventname: qualname TOKEN_AT interval  */
#line 423 "ting.y"
                                       { (yyval.other) = create_operation(op_var_at, (yyvsp[-2].other), (yyvsp[0].other), "%s @ %s"); }
#line 2822 "ting_parser.c"
    break;

  case 99: /* eventname: initmathidxname TOKEN_AT expression TOKEN_EQUAL expression  */
#line 424 "ting.y"
                                                                          { (yyval.other) = create_operation(op_var_at, create_operation(op_assign, (yyvsp[-4].other), (yyvsp[0].other), "%s = %s"), (yyvsp[-2].other), "%s @ %s"); }
#line 2828 "ting_parser.c"
    break;

  case 100: /* eventname: initmathidxname TOKEN_AT interval TOKEN_EQUAL expression  */
#line 425 "ting.y"
                                                                        { (yyval.other) = create_operation(op_var_at, create_operation(op_assign, (yyvsp[-4].other), (yyvsp[0].other), "%s = %s"), (yyvsp[-2].other), "%s @ %s"); }
#line 2834 "ting_parser.c"
    break;

  case 101: /* qualname: TOKEN_NOT initidxname  */
#line 429 "ting.y"
                               { (yyval.other) = create_operation(op_not, (yyvsp[0].other), NULL, "~ %s"); }
#line 2840 "ting_parser.c"
    break;

  case 102: /* qualname: initidxname  */
#line 430 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 2846 "ting_parser.c"
    break;

  case 103: /* varidxname: indexedname  */
#line 434 "ting.y"
                     { ((yyvsp[0].other))->left->ot = op_name; (yyval.other) = (yyvsp[0].other); }
#line 2852 "ting_parser.c"
    break;

  case 104: /* varidxname: name  */
#line 435 "ting.y"
              { ((yyvsp[0].other))->ot = op_name; (yyval.other) = (yyvsp[0].other); }
#line 2858 "ting_parser.c"
    break;

  case 105: /* indexedname: name TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 439 "ting.y"
                                                                                                                                       { (yyval.other) = create_operation(op_array4, (yyvsp[-9].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-7].other), (yyvsp[-5].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2865 "ting_parser.c"
    break;

  case 106: /* indexedname: name TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 441 "ting.y"
                                                                                                           { (yyval.other) = create_operation(op_array3, (yyvsp[-7].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-5].other), (yyvsp[-3].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-1].other),
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2873 "ting_parser.c"
    break;

  case 107: /* indexedname: name TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 444 "ting.y"
                                                                                 { (yyval.other) = create_operation(op_matrix, (yyvsp[-5].other),
      create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s(%s)"); }
#line 2880 "ting_parser.c"
    break;

  case 108: /* indexedname: name TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 446 "ting.y"
                                                       { (yyval.other) = create_operation(op_vector, (yyvsp[-3].other), (yyvsp[-1].other), "%s(%s)"); }
#line 2886 "ting_parser.c"
    break;

  case 109: /* mathidxname: mathname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 450 "ting.y"
                                                                                                                                           { (yyval.other) = create_operation(op_array4, (yyvsp[-9].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-7].other), (yyvsp[-5].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2893 "ting_parser.c"
    break;

  case 110: /* mathidxname: mathname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 452 "ting.y"
                                                                                                               { (yyval.other) = create_operation(op_array3, (yyvsp[-7].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-5].other), (yyvsp[-3].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-1].other),
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2901 "ting_parser.c"
    break;

  case 111: /* mathidxname: mathname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 455 "ting.y"
                                                                                     { (yyval.other) = create_operation(op_matrix, (yyvsp[-5].other),
      create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s(%s)"); }
#line 2908 "ting_parser.c"
    break;

  case 112: /* mathidxname: mathname TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 457 "ting.y"
                                                           { (yyval.other) = create_operation(op_vector, (yyvsp[-3].other), (yyvsp[-1].other), "%s(%s)"); }
#line 2914 "ting_parser.c"
    break;

  case 113: /* mathidxname: mathname  */
#line 458 "ting.y"
                  { (yyval.other) = (yyvsp[0].other); }
#line 2920 "ting_parser.c"
    break;

  case 114: /* declidxname: declname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 462 "ting.y"
                                                                                                                                           { (yyval.other) = create_operation(op_array4, (yyvsp[-9].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-7].other), (yyvsp[-5].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2927 "ting_parser.c"
    break;

  case 115: /* declidxname: declname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 464 "ting.y"
                                                                                                               { (yyval.other) = create_operation(op_array3, (yyvsp[-7].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-5].other), (yyvsp[-3].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-1].other),
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2935 "ting_parser.c"
    break;

  case 116: /* declidxname: declname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 467 "ting.y"
                                                                                     { (yyval.other) = create_operation(op_matrix, (yyvsp[-5].other),
      create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s(%s)"); }
#line 2942 "ting_parser.c"
    break;

  case 117: /* declidxname: declname TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 469 "ting.y"
                                                           { (yyval.other) = create_operation(op_vector, (yyvsp[-3].other), (yyvsp[-1].other), "%s(%s)"); }
#line 2948 "ting_parser.c"
    break;

  case 118: /* declidxname: declname  */
#line 470 "ting.y"
                  { (yyval.other) = (yyvsp[0].other); }
#line 2954 "ting_parser.c"
    break;

  case 119: /* initidxname: initname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 474 "ting.y"
                                                                                                                                           { (yyval.other) = create_operation(op_array4, (yyvsp[-9].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-7].other), (yyvsp[-5].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2961 "ting_parser.c"
    break;

  case 120: /* initidxname: initname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 476 "ting.y"
                                                                                                               { (yyval.other) = create_operation(op_array3, (yyvsp[-7].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-5].other), (yyvsp[-3].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-1].other),
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2969 "ting_parser.c"
    break;

  case 121: /* initidxname: initname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 479 "ting.y"
                                                                                     { (yyval.other) = create_operation(op_matrix, (yyvsp[-5].other),
      create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s(%s)"); }
#line 2976 "ting_parser.c"
    break;

  case 122: /* initidxname: initname TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 481 "ting.y"
                                                           { (yyval.other) = create_operation(op_vector, (yyvsp[-3].other), (yyvsp[-1].other), "%s(%s)"); }
#line 2982 "ting_parser.c"
    break;

  case 123: /* initidxname: initname  */
#line 482 "ting.y"
                  { (yyval.other) = (yyvsp[0].other); }
#line 2988 "ting_parser.c"
    break;

  case 124: /* initmathidxname: initmathname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 486 "ting.y"
                                                                                                                                               { (yyval.other) = create_operation(op_array4, (yyvsp[-9].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-7].other), (yyvsp[-5].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 2995 "ting_parser.c"
    break;

  case 125: /* initmathidxname: initmathname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 488 "ting.y"
                                                                                                                   { (yyval.other) = create_operation(op_array3, (yyvsp[-7].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-5].other), (yyvsp[-3].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-1].other),
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 3003 "ting_parser.c"
    break;

  case 126: /* initmathidxname: initmathname TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 491 "ting.y"
                                                                                         { (yyval.other) = create_operation(op_matrix, (yyvsp[-5].other),
      create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s(%s)"); }
#line 3010 "ting_parser.c"
    break;

  case 127: /* initmathidxname: initmathname TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 493 "ting.y"
                                                               { (yyval.other) = create_operation(op_vector, (yyvsp[-3].other), (yyvsp[-1].other), "%s(%s)"); }
#line 3016 "ting_parser.c"
    break;

  case 128: /* initmathidxname: initmathname  */
#line 494 "ting.y"
                      { (yyval.other) = (yyvsp[0].other); }
#line 3022 "ting_parser.c"
    break;

  case 129: /* constidxname: constname TOKEN_LPAREN exprorrange TOKEN_COMMA exprorrange TOKEN_COMMA exprorrange TOKEN_COMMA exprorrange TOKEN_RPAREN  */
#line 498 "ting.y"
                                                                                                                                                { (yyval.other) = create_operation(op_array4, (yyvsp[-9].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-7].other), (yyvsp[-5].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 3029 "ting_parser.c"
    break;

  case 130: /* constidxname: constname TOKEN_LPAREN exprorrange TOKEN_COMMA exprorrange TOKEN_COMMA exprorrange TOKEN_RPAREN  */
#line 500 "ting.y"
                                                                                                                   { (yyval.other) = create_operation(op_array3, (yyvsp[-7].other),
      create_operation(op_fourdim, create_operation(op_interval_1, (yyvsp[-5].other), (yyvsp[-3].other), "%s , %s"), create_operation(op_interval_1, (yyvsp[-1].other),
      create_ground(op_number, "", NULL_TIME, REAL_MAX), "%s , %s"), "%s , %s"), "%s(%s)"); }
#line 3037 "ting_parser.c"
    break;

  case 131: /* constidxname: constname TOKEN_LPAREN exprorrange TOKEN_COMMA exprorrange TOKEN_RPAREN  */
#line 503 "ting.y"
                                                                                        { (yyval.other) = create_operation(op_matrix, (yyvsp[-5].other),
      create_operation(op_interval_1, (yyvsp[-3].other), (yyvsp[-1].other), "%s , %s"), "%s(%s)"); }
#line 3044 "ting_parser.c"
    break;

  case 132: /* constidxname: constname TOKEN_LPAREN exprorrange TOKEN_RPAREN  */
#line 505 "ting.y"
                                                             { (yyval.other) = create_operation(op_vector, (yyvsp[-3].other), (yyvsp[-1].other), "%s(%s)"); }
#line 3050 "ting_parser.c"
    break;

  case 133: /* constidxname: constname  */
#line 506 "ting.y"
                   { (yyval.other) = (yyvsp[0].other); }
#line 3056 "ting_parser.c"
    break;

  case 134: /* exprorrange: expression  */
#line 510 "ting.y"
                    { (yyval.other) = (yyvsp[0].other); }
#line 3062 "ting_parser.c"
    break;

  case 135: /* exprorrange: TOKEN_UNDERSCORE  */
#line 511 "ting.y"
                       { (yyval.other) = create_ground(op_range, "_", NULL_TIME, REAL_MAX); }
#line 3068 "ting_parser.c"
    break;

  case 136: /* name: TOKEN_RICHNAME  */
#line 515 "ting.y"
                     { (yyval.other) = create_ground(op_constant, richwrap((yyvsp[0].symbol)), NULL_TIME, REAL_MAX); }
#line 3074 "ting_parser.c"
    break;

  case 137: /* name: TOKEN_NAME  */
#line 516 "ting.y"
                 { (yyval.other) = create_ground(op_constant, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3080 "ting_parser.c"
    break;

  case 138: /* mathname: TOKEN_REALRICH  */
#line 520 "ting.y"
                     { (yyval.other) = create_ground(op_variable, richwrap2((yyvsp[0].symbol)), NULL_TIME, REAL_MAX); }
#line 3086 "ting_parser.c"
    break;

  case 139: /* mathname: TOKEN_REALNAME  */
#line 521 "ting.y"
                     { (yyval.other) = create_ground(op_variable, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3092 "ting_parser.c"
    break;

  case 140: /* declname: TOKEN_RICHNAME  */
#line 525 "ting.y"
                     { (yyval.other) = create_ground(op_dname, richwrap((yyvsp[0].symbol)), NULL_TIME, REAL_MAX); }
#line 3098 "ting_parser.c"
    break;

  case 141: /* declname: TOKEN_NAME  */
#line 526 "ting.y"
                 { (yyval.other) = create_ground(op_dname, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3104 "ting_parser.c"
    break;

  case 142: /* declname: TOKEN_REALRICH  */
#line 527 "ting.y"
                     { (yyval.other) = create_ground(op_dvariable, richwrap2((yyvsp[0].symbol)), NULL_TIME, REAL_MAX); }
#line 3110 "ting_parser.c"
    break;

  case 143: /* declname: TOKEN_REALNAME  */
#line 528 "ting.y"
                     { (yyval.other) = create_ground(op_dvariable, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3116 "ting_parser.c"
    break;

  case 144: /* initname: TOKEN_RICHNAME  */
#line 532 "ting.y"
                     { (yyval.other) = create_ground(op_iname, richwrap((yyvsp[0].symbol)), NULL_TIME, REAL_MAX); }
#line 3122 "ting_parser.c"
    break;

  case 145: /* initname: TOKEN_NAME  */
#line 533 "ting.y"
                 { (yyval.other) = create_ground(op_iname, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3128 "ting_parser.c"
    break;

  case 146: /* initmathname: TOKEN_REALRICH  */
#line 537 "ting.y"
                     { (yyval.other) = create_ground(op_ivariable, richwrap2((yyvsp[0].symbol)), NULL_TIME, REAL_MAX); }
#line 3134 "ting_parser.c"
    break;

  case 147: /* initmathname: TOKEN_REALNAME  */
#line 538 "ting.y"
                     { (yyval.other) = create_ground(op_ivariable, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3140 "ting_parser.c"
    break;

  case 148: /* constname: TOKEN_NAME  */
#line 542 "ting.y"
                 { (yyval.other) = create_ground(op_cname, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3146 "ting_parser.c"
    break;

  case 149: /* filename: TOKEN_STRING  */
#line 546 "ting.y"
                   { (yyval.other) = create_ground(op_fname, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3152 "ting_parser.c"
    break;

  case 150: /* string: TOKEN_STRING  */
#line 550 "ting.y"
                   { (yyval.other) = create_ground(op_string, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3158 "ting_parser.c"
    break;

  case 151: /* varrange: constidxname TOKEN_IN expression TOKEN_COLON expression  */
#line 554 "ting.y"
                                                                       { (yyval.other) = create_operation(op_in, (yyvsp[-4].other), create_operation(op_interval_1, (yyvsp[-2].other), (yyvsp[0].other), "%s : %s"), "%s in %s"); }
#line 3164 "ting_parser.c"
    break;

  case 152: /* varrange: constidxname TOKEN_ON expression  */
#line 555 "ting.y"
                                              { (yyval.other) = create_operation(op_in, (yyvsp[-2].other), create_operation(op_interval_3, create_ground(op_number, "", 0, REAL_MAX), (yyvsp[0].other), "%s : %s - 1"), "%s on %s"); }
#line 3170 "ting_parser.c"
    break;

  case 153: /* varrange: expression TOKEN_COLON expression  */
#line 556 "ting.y"
                                              { (yyval.other) = create_operation(op_interval_1, (yyvsp[-2].other), (yyvsp[0].other), "%s : %s"); }
#line 3176 "ting_parser.c"
    break;

  case 154: /* varrange: expression  */
#line 557 "ting.y"
                    { (yyval.other) = create_operation(op_interval_3, create_ground(op_number, "", 0, REAL_MAX), (yyvsp[0].other), "%s : %s - 1"); }
#line 3182 "ting_parser.c"
    break;

  case 155: /* selrange: TOKEN_NEG selrange  */
#line 561 "ting.y"
                            { (yyval.other) = create_operation(op_neg_range, (yyvsp[0].other), NULL, "not %s"); }
#line 3188 "ting_parser.c"
    break;

  case 156: /* selrange: expression TOKEN_IN exprrange  */
#line 562 "ting.y"
                                          { (yyval.other) = create_operation(op_in, (yyvsp[-2].other), (yyvsp[0].other), "%s in %s"); }
#line 3194 "ting_parser.c"
    break;

  case 157: /* selrange: expression TOKEN_IS expression  */
#line 563 "ting.y"
                                            { (yyval.other) = create_operation(op_in, (yyvsp[-2].other), create_operation(op_interval_1, (yyvsp[0].other), copy_specification((yyvsp[0].other)), "%s : %s"), "%s is %s"); }
#line 3200 "ting_parser.c"
    break;

  case 158: /* selrange: exprrange  */
#line 564 "ting.y"
                   { (yyval.other) = (yyvsp[0].other); }
#line 3206 "ting_parser.c"
    break;

  case 159: /* selrange: expression  */
#line 565 "ting.y"
                    { (yyval.other) = create_operation(op_interval_1, (yyvsp[0].other), copy_specification((yyvsp[0].other)), "%s : %s"); }
#line 3212 "ting_parser.c"
    break;

  case 160: /* exprrange: expression TOKEN_COLON expression  */
#line 569 "ting.y"
                                              { (yyval.other) = create_operation(op_interval_1, (yyvsp[-2].other), (yyvsp[0].other), "%s : %s"); }
#line 3218 "ting_parser.c"
    break;

  case 161: /* exprrange: TOKEN_UNDERSCORE TOKEN_COLON expression  */
#line 570 "ting.y"
                                                 { (yyval.other) = create_operation(op_interval_1, create_ground(op_number, "", LONG_MIN, REAL_MAX), (yyvsp[0].other), "_ : %s"); }
#line 3224 "ting_parser.c"
    break;

  case 162: /* exprrange: expression TOKEN_COLON TOKEN_UNDERSCORE  */
#line 571 "ting.y"
                                                 { (yyval.other) = create_operation(op_interval_1, (yyvsp[-2].other), create_ground(op_number, "", LONG_MAX, REAL_MAX), "%s : _"); }
#line 3230 "ting_parser.c"
    break;

  case 163: /* mathrelation: mathexpression TOKEN_EQUAL mathexpression  */
#line 575 "ting.y"
                                                      { (yyval.other) = create_operation(op_equal, (yyvsp[-2].other), (yyvsp[0].other), "(%s = %s)"); }
#line 3236 "ting_parser.c"
    break;

  case 164: /* mathrelation: mathexpression TOKEN_NEQ mathexpression  */
#line 576 "ting.y"
                                                    { (yyval.other) = create_operation(op_neq, (yyvsp[-2].other), (yyvsp[0].other), "(%s ~= %s)"); }
#line 3242 "ting_parser.c"
    break;

  case 165: /* mathrelation: mathexpression TOKEN_LT mathexpression  */
#line 577 "ting.y"
                                                   { (yyval.other) = create_operation(op_lt, (yyvsp[-2].other), (yyvsp[0].other), "(%s < %s)"); }
#line 3248 "ting_parser.c"
    break;

  case 166: /* mathrelation: mathexpression TOKEN_GT mathexpression  */
#line 578 "ting.y"
                                                   { (yyval.other) = create_operation(op_gt, (yyvsp[-2].other), (yyvsp[0].other), "(%s > %s)"); }
#line 3254 "ting_parser.c"
    break;

  case 167: /* mathrelation: mathexpression TOKEN_LTEQ mathexpression  */
#line 579 "ting.y"
                                                     { (yyval.other) = create_operation(op_lteq, (yyvsp[-2].other), (yyvsp[0].other), "(%s <= %s)"); }
#line 3260 "ting_parser.c"
    break;

  case 168: /* mathrelation: mathexpression TOKEN_GTEQ mathexpression  */
#line 580 "ting.y"
                                                     { (yyval.other) = create_operation(op_gteq, (yyvsp[-2].other), (yyvsp[0].other), "(%s >= %s)"); }
#line 3266 "ting_parser.c"
    break;

  case 169: /* mathrelation: mathexpression TOKEN_EQUAL expression  */
#line 581 "ting.y"
                                                  { (yyval.other) = create_operation(op_equal, (yyvsp[-2].other), (yyvsp[0].other), "(%s = %s)"); }
#line 3272 "ting_parser.c"
    break;

  case 170: /* mathrelation: mathexpression TOKEN_NEQ expression  */
#line 582 "ting.y"
                                                { (yyval.other) = create_operation(op_neq, (yyvsp[-2].other), (yyvsp[0].other), "(%s ~= %s)"); }
#line 3278 "ting_parser.c"
    break;

  case 171: /* mathrelation: mathexpression TOKEN_LT expression  */
#line 583 "ting.y"
                                               { (yyval.other) = create_operation(op_lt, (yyvsp[-2].other), (yyvsp[0].other), "(%s < %s)"); }
#line 3284 "ting_parser.c"
    break;

  case 172: /* mathrelation: mathexpression TOKEN_GT expression  */
#line 584 "ting.y"
                                               { (yyval.other) = create_operation(op_gt, (yyvsp[-2].other), (yyvsp[0].other), "(%s > %s)"); }
#line 3290 "ting_parser.c"
    break;

  case 173: /* mathrelation: mathexpression TOKEN_LTEQ expression  */
#line 585 "ting.y"
                                                 { (yyval.other) = create_operation(op_lteq, (yyvsp[-2].other), (yyvsp[0].other), "(%s <= %s)"); }
#line 3296 "ting_parser.c"
    break;

  case 174: /* mathrelation: mathexpression TOKEN_GTEQ expression  */
#line 586 "ting.y"
                                                 { (yyval.other) = create_operation(op_gteq, (yyvsp[-2].other), (yyvsp[0].other), "(%s >= %s)"); }
#line 3302 "ting_parser.c"
    break;

  case 175: /* mathrelation: expression TOKEN_EQUAL mathexpression  */
#line 587 "ting.y"
                                                  { (yyval.other) = create_operation(op_equal, (yyvsp[-2].other), (yyvsp[0].other), "(%s = %s)"); }
#line 3308 "ting_parser.c"
    break;

  case 176: /* mathrelation: expression TOKEN_NEQ mathexpression  */
#line 588 "ting.y"
                                                { (yyval.other) = create_operation(op_neq, (yyvsp[-2].other), (yyvsp[0].other), "(%s ~= %s)"); }
#line 3314 "ting_parser.c"
    break;

  case 177: /* mathrelation: expression TOKEN_LT mathexpression  */
#line 589 "ting.y"
                                               { (yyval.other) = create_operation(op_lt, (yyvsp[-2].other), (yyvsp[0].other), "(%s < %s)"); }
#line 3320 "ting_parser.c"
    break;

  case 178: /* mathrelation: expression TOKEN_GT mathexpression  */
#line 590 "ting.y"
                                               { (yyval.other) = create_operation(op_gt, (yyvsp[-2].other), (yyvsp[0].other), "(%s > %s)"); }
#line 3326 "ting_parser.c"
    break;

  case 179: /* mathrelation: expression TOKEN_LTEQ mathexpression  */
#line 591 "ting.y"
                                                 { (yyval.other) = create_operation(op_lteq, (yyvsp[-2].other), (yyvsp[0].other), "(%s <= %s)"); }
#line 3332 "ting_parser.c"
    break;

  case 180: /* mathrelation: expression TOKEN_GTEQ mathexpression  */
#line 592 "ting.y"
                                                 { (yyval.other) = create_operation(op_gteq, (yyvsp[-2].other), (yyvsp[0].other), "(%s >= %s)"); }
#line 3338 "ting_parser.c"
    break;

  case 181: /* mathexpression: mathexpression TOKEN_PLUS mathexprmul  */
#line 596 "ting.y"
                                                  { (yyval.other) = create_operation(op_plus, (yyvsp[-2].other), (yyvsp[0].other), "(%s + %s)"); }
#line 3344 "ting_parser.c"
    break;

  case 182: /* mathexpression: mathexpression TOKEN_MINUS mathexprmul  */
#line 597 "ting.y"
                                                   { (yyval.other) = create_operation(op_minus, (yyvsp[-2].other), (yyvsp[0].other), "(%s - %s)"); }
#line 3350 "ting_parser.c"
    break;

  case 183: /* mathexpression: mathexpression TOKEN_PLUS exprmul  */
#line 598 "ting.y"
                                              { (yyval.other) = create_operation(op_plus, (yyvsp[-2].other), (yyvsp[0].other), "(%s + %s)"); }
#line 3356 "ting_parser.c"
    break;

  case 184: /* mathexpression: mathexpression TOKEN_MINUS exprmul  */
#line 599 "ting.y"
                                               { (yyval.other) = create_operation(op_minus, (yyvsp[-2].other), (yyvsp[0].other), "(%s - %s)"); }
#line 3362 "ting_parser.c"
    break;

  case 185: /* mathexpression: expression TOKEN_PLUS mathexprmul  */
#line 600 "ting.y"
                                              { (yyval.other) = create_operation(op_plus, (yyvsp[-2].other), (yyvsp[0].other), "(%s + %s)"); }
#line 3368 "ting_parser.c"
    break;

  case 186: /* mathexpression: expression TOKEN_MINUS mathexprmul  */
#line 601 "ting.y"
                                               { (yyval.other) = create_operation(op_minus, (yyvsp[-2].other), (yyvsp[0].other), "(%s - %s)"); }
#line 3374 "ting_parser.c"
    break;

  case 187: /* mathexpression: mathexprmul  */
#line 602 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 3380 "ting_parser.c"
    break;

  case 188: /* mathexprmul: mathexprmul TOKEN_MUL mathexprpow  */
#line 606 "ting.y"
                                              { (yyval.other) = create_operation(op_mul, (yyvsp[-2].other), (yyvsp[0].other), "(%s * %s)"); }
#line 3386 "ting_parser.c"
    break;

  case 189: /* mathexprmul: mathexprmul TOKEN_DIV mathexprpow  */
#line 607 "ting.y"
                                              { (yyval.other) = create_operation(op_div, (yyvsp[-2].other), (yyvsp[0].other), "(%s / %s)"); }
#line 3392 "ting_parser.c"
    break;

  case 190: /* mathexprmul: mathexprmul TOKEN_MOD mathexprpow  */
#line 608 "ting.y"
                                              { (yyval.other) = create_operation(op_mod, (yyvsp[-2].other), (yyvsp[0].other), "(%s %% %s)"); }
#line 3398 "ting_parser.c"
    break;

  case 191: /* mathexprmul: mathexprmul TOKEN_MUL exprpow  */
#line 609 "ting.y"
                                          { (yyval.other) = create_operation(op_mul, (yyvsp[-2].other), (yyvsp[0].other), "(%s * %s)"); }
#line 3404 "ting_parser.c"
    break;

  case 192: /* mathexprmul: mathexprmul TOKEN_DIV exprpow  */
#line 610 "ting.y"
                                          { (yyval.other) = create_operation(op_div, (yyvsp[-2].other), (yyvsp[0].other), "(%s / %s)"); }
#line 3410 "ting_parser.c"
    break;

  case 193: /* mathexprmul: mathexprmul TOKEN_MOD exprpow  */
#line 611 "ting.y"
                                          { (yyval.other) = create_operation(op_mod, (yyvsp[-2].other), (yyvsp[0].other), "(%s %% %s)"); }
#line 3416 "ting_parser.c"
    break;

  case 194: /* mathexprmul: exprmul TOKEN_MUL mathexprpow  */
#line 612 "ting.y"
                                          { (yyval.other) = create_operation(op_mul, (yyvsp[-2].other), (yyvsp[0].other), "(%s * %s)"); }
#line 3422 "ting_parser.c"
    break;

  case 195: /* mathexprmul: exprmul TOKEN_DIV mathexprpow  */
#line 613 "ting.y"
                                          { (yyval.other) = create_operation(op_div, (yyvsp[-2].other), (yyvsp[0].other), "(%s / %s)"); }
#line 3428 "ting_parser.c"
    break;

  case 196: /* mathexprmul: exprmul TOKEN_MOD mathexprpow  */
#line 614 "ting.y"
                                          { (yyval.other) = create_operation(op_mod, (yyvsp[-2].other), (yyvsp[0].other), "(%s %% %s)"); }
#line 3434 "ting_parser.c"
    break;

  case 197: /* mathexprmul: mathexprpow  */
#line 615 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 3440 "ting_parser.c"
    break;

  case 198: /* mathexprpow: mathexprpow TOKEN_POW mathexprroot  */
#line 619 "ting.y"
                                               { (yyval.other) = create_operation(op_pow, (yyvsp[-2].other), (yyvsp[0].other), "(%s ^ %s)"); }
#line 3446 "ting_parser.c"
    break;

  case 199: /* mathexprpow: mathexprpow TOKEN_POW exprroot  */
#line 620 "ting.y"
                                           { (yyval.other) = create_operation(op_pow, (yyvsp[-2].other), (yyvsp[0].other), "(%s ^ %s)"); }
#line 3452 "ting_parser.c"
    break;

  case 200: /* mathexprpow: exprpow TOKEN_POW mathexprroot  */
#line 621 "ting.y"
                                           { (yyval.other) = create_operation(op_pow, (yyvsp[-2].other), (yyvsp[0].other), "(%s ^ %s)"); }
#line 3458 "ting_parser.c"
    break;

  case 201: /* mathexprpow: mathexprroot  */
#line 622 "ting.y"
                      { (yyval.other) = (yyvsp[0].other); }
#line 3464 "ting_parser.c"
    break;

  case 202: /* mathexprroot: TOKEN_ROOT TOKEN_LPAREN mathexpression TOKEN_COMMA mathexpression TOKEN_RPAREN  */
#line 626 "ting.y"
                                                                                           { (yyval.other) = create_operation(op_root, (yyvsp[-3].other), (yyvsp[-1].other), "root(%s , %s)"); }
#line 3470 "ting_parser.c"
    break;

  case 203: /* mathexprroot: TOKEN_LOG TOKEN_LPAREN mathexpression TOKEN_COMMA mathexpression TOKEN_RPAREN  */
#line 627 "ting.y"
                                                                                          { (yyval.other) = create_operation(op_log, (yyvsp[-3].other), (yyvsp[-1].other), "log(%s , %s)"); }
#line 3476 "ting_parser.c"
    break;

  case 204: /* mathexprroot: TOKEN_ROOT TOKEN_LPAREN mathexpression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 628 "ting.y"
                                                                                       { (yyval.other) = create_operation(op_root, (yyvsp[-3].other), (yyvsp[-1].other), "root(%s , %s)"); }
#line 3482 "ting_parser.c"
    break;

  case 205: /* mathexprroot: TOKEN_LOG TOKEN_LPAREN mathexpression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 629 "ting.y"
                                                                                      { (yyval.other) = create_operation(op_log, (yyvsp[-3].other), (yyvsp[-1].other), "log(%s , %s)"); }
#line 3488 "ting_parser.c"
    break;

  case 206: /* mathexprroot: TOKEN_ROOT TOKEN_LPAREN expression TOKEN_COMMA mathexpression TOKEN_RPAREN  */
#line 630 "ting.y"
                                                                                       { (yyval.other) = create_operation(op_root, (yyvsp[-3].other), (yyvsp[-1].other), "root(%s , %s)"); }
#line 3494 "ting_parser.c"
    break;

  case 207: /* mathexprroot: TOKEN_LOG TOKEN_LPAREN expression TOKEN_COMMA mathexpression TOKEN_RPAREN  */
#line 631 "ting.y"
                                                                                      { (yyval.other) = create_operation(op_log, (yyvsp[-3].other), (yyvsp[-1].other), "log(%s , %s)"); }
#line 3500 "ting_parser.c"
    break;

  case 208: /* mathexprroot: TOKEN_SIN TOKEN_LPAREN mathexpression TOKEN_RPAREN  */
#line 632 "ting.y"
                                                            { (yyval.other) = create_operation(op_sin, (yyvsp[-1].other), NULL, "sin(%s)"); }
#line 3506 "ting_parser.c"
    break;

  case 209: /* mathexprroot: TOKEN_COS TOKEN_LPAREN mathexpression TOKEN_RPAREN  */
#line 633 "ting.y"
                                                            { (yyval.other) = create_operation(op_cos, (yyvsp[-1].other), NULL, "cos(%s)"); }
#line 3512 "ting_parser.c"
    break;

  case 210: /* mathexprroot: TOKEN_TAN TOKEN_LPAREN mathexpression TOKEN_RPAREN  */
#line 634 "ting.y"
                                                            { (yyval.other) = create_operation(op_tan, (yyvsp[-1].other), NULL, "tan(%s)"); }
#line 3518 "ting_parser.c"
    break;

  case 211: /* mathexprroot: TOKEN_ASIN TOKEN_LPAREN mathexpression TOKEN_RPAREN  */
#line 635 "ting.y"
                                                             { (yyval.other) = create_operation(op_asin, (yyvsp[-1].other), NULL, "asin(%s)"); }
#line 3524 "ting_parser.c"
    break;

  case 212: /* mathexprroot: TOKEN_ACOS TOKEN_LPAREN mathexpression TOKEN_RPAREN  */
#line 636 "ting.y"
                                                             { (yyval.other) = create_operation(op_acos, (yyvsp[-1].other), NULL, "acos(%s)"); }
#line 3530 "ting_parser.c"
    break;

  case 213: /* mathexprroot: TOKEN_ATAN TOKEN_LPAREN mathexpression TOKEN_RPAREN  */
#line 637 "ting.y"
                                                             { (yyval.other) = create_operation(op_atan, (yyvsp[-1].other), NULL, "atan(%s)"); }
#line 3536 "ting_parser.c"
    break;

  case 214: /* mathexprroot: TOKEN_SUM TOKEN_LPAREN mathexpression TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 638 "ting.y"
                                                                                     { (yyval.other) = create_operation(op_sum, (yyvsp[-3].other), (yyvsp[-1].other), "sum(%s, %s)"); }
#line 3542 "ting_parser.c"
    break;

  case 215: /* mathexprroot: TOKEN_PROD TOKEN_LPAREN mathexpression TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 639 "ting.y"
                                                                                      { (yyval.other) = create_operation(op_prod, (yyvsp[-3].other), (yyvsp[-1].other), "prod(%s, %s)"); }
#line 3548 "ting_parser.c"
    break;

  case 216: /* mathexprroot: mathexprchs  */
#line 640 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 3554 "ting_parser.c"
    break;

  case 217: /* mathexprchs: TOKEN_MINUS mathexprdelay  */
#line 644 "ting.y"
                                   { (yyval.other) = create_operation(op_chs, (yyvsp[0].other), NULL, "- (%s)"); }
#line 3560 "ting_parser.c"
    break;

  case 218: /* mathexprchs: mathexprdelay  */
#line 645 "ting.y"
                       { (yyval.other) = (yyvsp[0].other); }
#line 3566 "ting_parser.c"
    break;

  case 219: /* mathexprdelay: mathexprgnd TOKEN_AT exprroot  */
#line 649 "ting.y"
                                          { (yyval.other) = create_operation(op_math_delay, (yyvsp[-2].other), (yyvsp[0].other), "(%s @ %s)"); }
#line 3572 "ting_parser.c"
    break;

  case 220: /* mathexprdelay: mathexprgnd  */
#line 650 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 3578 "ting_parser.c"
    break;

  case 221: /* mathexprgnd: TOKEN_LPAREN mathexpression TOKEN_RPAREN  */
#line 654 "ting.y"
                                                  { (yyval.other) = (yyvsp[-1].other); }
#line 3584 "ting_parser.c"
    break;

  case 222: /* mathexprgnd: mathidxname  */
#line 655 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 3590 "ting_parser.c"
    break;

  case 223: /* expression: expression TOKEN_PLUS exprmul  */
#line 659 "ting.y"
                                          { (yyval.other) = create_operation(op_plus, (yyvsp[-2].other), (yyvsp[0].other), "(%s + %s)"); }
#line 3596 "ting_parser.c"
    break;

  case 224: /* expression: expression TOKEN_MINUS exprmul  */
#line 660 "ting.y"
                                           { (yyval.other) = create_operation(op_minus, (yyvsp[-2].other), (yyvsp[0].other), "(%s - %s)"); }
#line 3602 "ting_parser.c"
    break;

  case 225: /* expression: exprmul  */
#line 661 "ting.y"
                 { (yyval.other) = (yyvsp[0].other); }
#line 3608 "ting_parser.c"
    break;

  case 226: /* exprmul: exprmul TOKEN_MUL exprpow  */
#line 665 "ting.y"
                                      { (yyval.other) = create_operation(op_mul, (yyvsp[-2].other), (yyvsp[0].other), "(%s * %s)"); }
#line 3614 "ting_parser.c"
    break;

  case 227: /* exprmul: exprmul TOKEN_DIV exprpow  */
#line 666 "ting.y"
                                      { (yyval.other) = create_operation(op_div, (yyvsp[-2].other), (yyvsp[0].other), "(%s / %s)"); }
#line 3620 "ting_parser.c"
    break;

  case 228: /* exprmul: exprmul TOKEN_MOD exprpow  */
#line 667 "ting.y"
                                      { (yyval.other) = create_operation(op_mod, (yyvsp[-2].other), (yyvsp[0].other), "(%s %% %s)"); }
#line 3626 "ting_parser.c"
    break;

  case 229: /* exprmul: exprpow  */
#line 668 "ting.y"
                 { (yyval.other) = (yyvsp[0].other); }
#line 3632 "ting_parser.c"
    break;

  case 230: /* exprpow: exprpow TOKEN_POW exprroot  */
#line 672 "ting.y"
                                       { (yyval.other) = create_operation(op_pow, (yyvsp[-2].other), (yyvsp[0].other), "(%s ^ %s)"); }
#line 3638 "ting_parser.c"
    break;

  case 231: /* exprpow: exprroot  */
#line 673 "ting.y"
                  { (yyval.other) = (yyvsp[0].other); }
#line 3644 "ting_parser.c"
    break;

  case 232: /* exprroot: TOKEN_ROOT TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 677 "ting.y"
                                                                                   { (yyval.other) = create_operation(op_root, (yyvsp[-3].other), (yyvsp[-1].other), "root(%s , %s)"); }
#line 3650 "ting_parser.c"
    break;

  case 233: /* exprroot: TOKEN_LOG TOKEN_LPAREN expression TOKEN_COMMA expression TOKEN_RPAREN  */
#line 678 "ting.y"
                                                                                  { (yyval.other) = create_operation(op_log, (yyvsp[-3].other), (yyvsp[-1].other), "log(%s , %s)"); }
#line 3656 "ting_parser.c"
    break;

  case 234: /* exprroot: TOKEN_SIN TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 679 "ting.y"
                                                        { (yyval.other) = create_operation(op_sin, (yyvsp[-1].other), NULL, "sin(%s)"); }
#line 3662 "ting_parser.c"
    break;

  case 235: /* exprroot: TOKEN_COS TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 680 "ting.y"
                                                        { (yyval.other) = create_operation(op_cos, (yyvsp[-1].other), NULL, "cos(%s)"); }
#line 3668 "ting_parser.c"
    break;

  case 236: /* exprroot: TOKEN_TAN TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 681 "ting.y"
                                                        { (yyval.other) = create_operation(op_tan, (yyvsp[-1].other), NULL, "tan(%s)"); }
#line 3674 "ting_parser.c"
    break;

  case 237: /* exprroot: TOKEN_ASIN TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 682 "ting.y"
                                                         { (yyval.other) = create_operation(op_asin, (yyvsp[-1].other), NULL, "asin(%s)"); }
#line 3680 "ting_parser.c"
    break;

  case 238: /* exprroot: TOKEN_ACOS TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 683 "ting.y"
                                                         { (yyval.other) = create_operation(op_acos, (yyvsp[-1].other), NULL, "acos(%s)"); }
#line 3686 "ting_parser.c"
    break;

  case 239: /* exprroot: TOKEN_ATAN TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 684 "ting.y"
                                                         { (yyval.other) = create_operation(op_atan, (yyvsp[-1].other), NULL, "atan(%s)"); }
#line 3692 "ting_parser.c"
    break;

  case 240: /* exprroot: TOKEN_SUM TOKEN_LPAREN expression TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 685 "ting.y"
                                                                                 { (yyval.other) = create_operation(op_sum, (yyvsp[-3].other), (yyvsp[-1].other), "sum(%s, %s)"); }
#line 3698 "ting_parser.c"
    break;

  case 241: /* exprroot: TOKEN_PROD TOKEN_LPAREN expression TOKEN_COMMA varrange TOKEN_RPAREN  */
#line 686 "ting.y"
                                                                                  { (yyval.other) = create_operation(op_prod, (yyvsp[-3].other), (yyvsp[-1].other), "prod(%s, %s)"); }
#line 3704 "ting_parser.c"
    break;

  case 242: /* exprroot: exprchs  */
#line 687 "ting.y"
                 { (yyval.other) = (yyvsp[0].other); }
#line 3710 "ting_parser.c"
    break;

  case 243: /* exprchs: TOKEN_MINUS exprgnd  */
#line 691 "ting.y"
                             { (yyval.other) = create_operation(op_chs, (yyvsp[0].other), NULL, "- (%s)"); }
#line 3716 "ting_parser.c"
    break;

  case 244: /* exprchs: exprgnd  */
#line 692 "ting.y"
                 { (yyval.other) = (yyvsp[0].other); }
#line 3722 "ting_parser.c"
    break;

  case 245: /* exprgnd: TOKEN_LPAREN expression TOKEN_RPAREN  */
#line 696 "ting.y"
                                              { (yyval.other) = (yyvsp[-1].other); }
#line 3728 "ting_parser.c"
    break;

  case 246: /* exprgnd: nameornum  */
#line 697 "ting.y"
                   { (yyval.other) = (yyvsp[0].other); }
#line 3734 "ting_parser.c"
    break;

  case 247: /* nameornum: indexedname  */
#line 701 "ting.y"
                     { (yyval.other) = (yyvsp[0].other); }
#line 3740 "ting_parser.c"
    break;

  case 248: /* nameornum: name  */
#line 702 "ting.y"
              { (yyval.other) = (yyvsp[0].other); }
#line 3746 "ting_parser.c"
    break;

  case 249: /* nameornum: TOKEN_ITERATOR  */
#line 703 "ting.y"
                     { (yyval.other) = create_ground(op_iterator, (yyvsp[0].symbol), NULL_TIME, REAL_MAX); }
#line 3752 "ting_parser.c"
    break;

  case 250: /* nameornum: realnumber  */
#line 704 "ting.y"
                    { (yyval.other) = (yyvsp[0].other); }
#line 3758 "ting_parser.c"
    break;

  case 251: /* nameornum: number  */
#line 705 "ting.y"
                { (yyval.other) = (yyvsp[0].other); }
#line 3764 "ting_parser.c"
    break;

  case 252: /* realnumber: TOKEN_REAL  */
#line 709 "ting.y"
                 { (yyval.other) = create_ground(op_real, "", NULL_TIME, (yyvsp[0].realval)); }
#line 3770 "ting_parser.c"
    break;

  case 253: /* realnumber: TOKEN_KE  */
#line 710 "ting.y"
               { (yyval.other) = create_ground(op_real, "", NULL_TIME, M_E); }
#line 3776 "ting_parser.c"
    break;

  case 254: /* realnumber: TOKEN_KPI  */
#line 711 "ting.y"
                { (yyval.other) = create_ground(op_real, "", NULL_TIME, M_PI); }
#line 3782 "ting_parser.c"
    break;

  case 255: /* number: TOKEN_NUMBER  */
#line 715 "ting.y"
                   { (yyval.other) = create_ground(op_number, "", (yyvsp[0].value), REAL_MAX); }
#line 3788 "ting_parser.c"
    break;


#line 3792 "ting_parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, spec_handle, scanner, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, spec_handle, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, spec_handle, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, spec_handle, scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, spec_handle, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, spec_handle, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 718 "ting.y"



