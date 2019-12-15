/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "ting.y"


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


#line 102 "ting_parser.c"

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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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
#line 34 "ting.y"

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

#line 154 "ting_parser.c"

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
    TOKEN_MOD = 273,
    TOKEN_POW = 274,
    TOKEN_LBRACKET = 275,
    TOKEN_RBRACKET = 276,
    TOKEN_LPAREN = 277,
    TOKEN_RPAREN = 278,
    TOKEN_LSQUARED = 279,
    TOKEN_RSQUARED = 280,
    TOKEN_COMMA = 281,
    TOKEN_COLON = 282,
    TOKEN_ITER = 283,
    TOKEN_WHEN = 284,
    TOKEN_ELSE = 285,
    TOKEN_DEFINE = 286,
    TOKEN_INCLUDE = 287,
    TOKEN_INPUT = 288,
    TOKEN_OUTPUT = 289,
    TOKEN_AUX = 290,
    TOKEN_INIT = 291,
    TOKEN_CODE = 292,
    TOKEN_FORALL = 293,
    TOKEN_EXISTS = 294,
    TOKEN_ONE = 295,
    TOKEN_UNIQUE = 296,
    TOKEN_EQUAL = 297,
    TOKEN_ON = 298,
    TOKEN_IN = 299,
    TOKEN_IS = 300,
    TOKEN_NEG = 301,
    TOKEN_ANY = 302,
    TOKEN_IPC = 303,
    TOKEN_FILE = 304,
    TOKEN_BINARY = 305,
    TOKEN_PACKED = 306,
    TOKEN_UNKNOWN = 307,
    TOKEN_FALSE = 308,
    TOKEN_TRUE = 309,
    TOKEN_RAW = 310,
    TOKEN_FILTER = 311,
    TOKEN_OMIT = 312,
    TOKEN_NAME = 313,
    TOKEN_STRING = 314,
    TOKEN_ITERATOR = 315,
    TOKEN_NUMBER = 316
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 64 "ting.y"

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
  btl_specification *io_decllist;
  btl_specification *decllist;
  btl_specification *eventlist;
  btl_specification *filelist;
  btl_specification *assignlist;
  btl_specification *assignment;
  btl_specification *varname;
  btl_specification *indexedname;
  btl_specification *declname;
  btl_specification *initname;
  btl_specification *eventname;
  btl_specification *qualname;
  btl_specification *constname;
  btl_specification *filename;
  btl_specification *expression;
  btl_specification *varrange;
  btl_specification *selrange;
  btl_specification *number;
  btl_specification *string;

#line 260 "ting_parser.c"

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



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
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
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  64
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   540

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  115
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  244

#define YYUNDEFTOK  2
#define YYMAXUTOK   316

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
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
      55,    56,    57,    58,    59,    60,    61
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   207,   207,   211,   212,   213,   217,   218,   219,   220,
     224,   225,   229,   230,   231,   234,   237,   240,   241,   245,
     246,   247,   250,   251,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   278,   279,   280,   281,   285,   286,
     289,   290,   293,   294,   295,   296,   300,   301,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   319,
     320,   324,   325,   329,   330,   334,   335,   339,   343,   344,
     348,   349,   353,   355,   356,   360,   362,   363,   367,   369,
     370,   374,   378,   382,   386,   390,   391,   392,   393,   397,
     398,   399,   400,   401,   405,   406,   407,   408,   409,   410,
     411,   412,   413,   417,   418,   419
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOKEN_SEMICOLON", "TOKEN_EQV",
  "TOKEN_IMPLY", "TOKEN_INVIMPLY", "TOKEN_OR", "TOKEN_AND", "TOKEN_HAPPEN",
  "TOKEN_AT", "TOKEN_SINCE", "TOKEN_UNTIL", "TOKEN_NOT", "TOKEN_PLUS",
  "TOKEN_MINUS", "TOKEN_MUL", "TOKEN_DIV", "TOKEN_MOD", "TOKEN_POW",
  "TOKEN_LBRACKET", "TOKEN_RBRACKET", "TOKEN_LPAREN", "TOKEN_RPAREN",
  "TOKEN_LSQUARED", "TOKEN_RSQUARED", "TOKEN_COMMA", "TOKEN_COLON",
  "TOKEN_ITER", "TOKEN_WHEN", "TOKEN_ELSE", "TOKEN_DEFINE",
  "TOKEN_INCLUDE", "TOKEN_INPUT", "TOKEN_OUTPUT", "TOKEN_AUX",
  "TOKEN_INIT", "TOKEN_CODE", "TOKEN_FORALL", "TOKEN_EXISTS", "TOKEN_ONE",
  "TOKEN_UNIQUE", "TOKEN_EQUAL", "TOKEN_ON", "TOKEN_IN", "TOKEN_IS",
  "TOKEN_NEG", "TOKEN_ANY", "TOKEN_IPC", "TOKEN_FILE", "TOKEN_BINARY",
  "TOKEN_PACKED", "TOKEN_UNKNOWN", "TOKEN_FALSE", "TOKEN_TRUE",
  "TOKEN_RAW", "TOKEN_FILTER", "TOKEN_OMIT", "TOKEN_NAME", "TOKEN_STRING",
  "TOKEN_ITERATOR", "TOKEN_NUMBER", "$accept", "input", "spec", "xform",
  "xdecl", "fblock", "dblock", "form", "inter", "decl", "iodlist", "iod",
  "dlist", "elist", "flist", "alist", "assign", "ename", "qname", "xname",
  "dname", "iname", "name", "cname", "fname", "str", "vrange", "srange",
  "expr", "num", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316
};
# endif

#define YYPACT_NINF -165

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-165)))

#define YYTABLE_NINF -93

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     233,     8,    23,   129,   233,   129,    28,    40,    -4,   -27,
     -15,    -9,     6,    -6,    84,   101,   113,   117,   122,   123,
      48,  -165,   233,   233,  -165,  -165,   530,    74,  -165,  -165,
     129,   129,  -165,   171,   202,   345,   128,     5,  -165,   110,
    -165,   105,  -165,   131,  -165,   425,   132,   145,  -165,   425,
     145,   145,   118,   150,   151,  -165,   169,  -165,   127,   129,
     129,   129,   129,   158,  -165,   233,   233,   233,   233,  -165,
     129,   129,   129,   129,   129,   100,   100,  -165,   271,   278,
    -165,  -165,  -165,   158,   158,   -31,  -165,  -165,   -26,   167,
      55,  -165,     5,  -165,   178,   111,    -4,   158,   -27,  -165,
    -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,  -165,
      33,  -165,   158,     6,    42,  -165,   158,    -6,   100,  -165,
     168,   301,   308,   315,   338,   314,   242,   286,    14,    32,
      92,   158,   158,  -165,   284,  -165,   284,   129,   129,    18,
     469,   158,   158,   233,   158,   158,   158,   158,   158,   158,
     158,  -165,   233,   158,   158,   158,  -165,   284,  -165,     6,
     425,   389,  -165,     6,   402,  -165,  -165,   284,   137,   128,
     128,     5,   128,  -165,   158,   415,   428,   352,   372,  -165,
     284,   369,  -165,  -165,   354,    18,   102,   114,   198,  -165,
     284,   190,   191,   284,   375,   284,   145,  -165,  -165,   158,
     145,  -165,   158,  -165,   203,   479,   204,   205,   199,   209,
     489,   158,   158,  -165,  -165,   158,   233,   233,   158,   499,
     509,  -165,  -165,  -165,  -165,   128,  -165,  -165,   434,   446,
     284,  -165,  -165,  -165,  -165,   284,  -165,  -165,   231,  -165,
    -165,  -165,  -165,  -165
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    91,
       0,     2,     3,     4,     9,    11,     0,     0,    43,    84,
       0,     0,    24,     0,     0,     0,     0,     0,    92,    54,
      76,     0,    93,    55,    74,     0,    87,    49,    70,     0,
      51,    52,     0,    90,    53,    72,     0,    81,     0,     0,
       0,     0,     0,     0,     1,     6,     7,     8,    10,    18,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      17,    22,    42,     0,     0,   115,   114,   113,     0,     0,
      98,   112,     0,   115,     0,   103,     0,     0,     0,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
       0,    57,     0,     0,     0,    80,     0,     0,     0,    91,
       0,     0,     0,     0,     0,     0,    39,    37,    38,    26,
      25,     0,     0,    30,    28,    29,    27,     0,     0,   110,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    99,     0,     0,     0,     0,    75,    77,    73,     0,
       0,     0,    69,     0,     0,    71,    79,    78,     0,     0,
       0,     0,     0,    83,     0,     0,     0,     0,     0,   111,
      96,     0,    12,    19,   104,   105,   106,   107,   108,   109,
      97,    13,    20,   102,     0,   101,    48,    56,    86,     0,
      50,    89,     0,    94,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,     0,     0,     0,     0,
       0,    40,    41,    33,    34,     0,    36,    82,     0,     0,
      95,    14,    15,    16,    21,   100,    85,    88,     0,    47,
      45,    46,    44,    35
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -165,  -165,  -165,    10,    80,  -127,  -124,    26,   -65,  -165,
     207,    97,   -10,  -165,  -165,  -165,   162,   142,  -165,  -165,
     176,   211,   232,    -5,   228,  -165,  -164,   -88,   -37,  -165
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    20,    21,    65,    66,    24,    25,    26,   133,    27,
     110,   111,    47,    54,    43,    39,    40,    55,    56,    28,
      48,    57,    29,    88,    44,   204,    89,    94,    90,    91
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      95,    50,    51,    41,   151,   206,   207,    52,   209,    45,
      22,   135,   -92,   -92,    33,    49,   182,   141,   142,   183,
      83,    73,    74,    75,    76,   191,   125,    84,   192,    32,
      30,    35,    42,    67,   146,   147,   148,   149,   134,   136,
      74,    75,    76,    46,    67,    31,   139,   140,    64,    46,
      36,    92,    53,   166,    38,    95,    78,    79,   159,   160,
     157,   238,    37,    93,    46,    86,    87,   163,   160,   144,
     145,   146,   147,   148,   149,   161,    67,    77,    67,   164,
      23,   167,   150,   208,    34,   121,   122,   123,   124,   231,
     233,    41,   232,   234,   175,   176,   126,   127,   128,   129,
     130,    75,    76,    68,   180,   181,    58,   184,   185,   186,
     187,   188,   189,   190,    68,    83,   193,   194,   195,   147,
     148,   149,   131,    59,   132,   144,   145,   146,   147,   148,
     149,   205,   148,   149,    95,    60,    96,   210,   153,    61,
       1,     2,     3,    83,    62,    63,    68,    97,    68,   196,
      84,     5,    83,   200,   112,   154,   155,    98,    93,    84,
      86,    87,   219,   177,   178,   220,    14,    15,    16,    17,
      18,   113,   116,    83,   228,   229,    53,   117,   230,   118,
      84,   235,     1,     2,     3,   119,    85,    19,    86,    87,
     143,     4,    80,     5,   168,    93,   203,    86,    87,     6,
       7,   152,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,     1,     2,     3,    93,   149,    86,    87,
     216,   217,     4,    81,     5,   225,   221,   223,   224,    19,
       6,     7,   226,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,     1,     2,     3,    71,    72,    73,
      74,    75,    76,     4,   243,     5,   114,   197,   156,   165,
      19,     6,     7,   115,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    70,    71,    72,    73,    74,
      75,    76,    70,    71,    72,    73,    74,    75,    76,   162,
     120,    19,    72,    73,    74,    75,    76,   137,   144,   145,
     146,   147,   148,   149,   138,    70,    71,    72,    73,    74,
      75,    76,    70,    71,    72,    73,    74,    75,    76,    70,
      71,    72,    73,    74,    75,    76,   158,   169,   144,   145,
     146,   147,   148,   149,   170,     0,     0,   173,     0,     0,
     174,   171,    70,    71,    72,    73,    74,    75,    76,    70,
      71,    72,    73,    74,    75,    76,    70,    71,    72,    73,
      74,    75,    76,     0,   172,     0,     0,     0,    82,   145,
     146,   147,   148,   149,     0,   213,    70,    71,    72,    73,
      74,    75,    76,   144,   145,   146,   147,   148,   149,   144,
     145,   146,   147,   148,   149,   214,   215,     0,     0,     0,
       0,     0,   218,   144,   145,   146,   147,   148,   149,     0,
       0,     0,   198,     0,     0,   199,   144,   145,   146,   147,
     148,   149,     0,     0,     0,   201,     0,     0,   202,   144,
     145,   146,   147,   148,   149,     0,     0,     0,   179,     0,
       0,   211,   144,   145,   146,   147,   148,   149,   144,   145,
     146,   147,   148,   149,   212,     0,     0,   239,     0,   240,
     144,   145,   146,   147,   148,   149,     0,     0,     0,   241,
       0,   242,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   144,   145,   146,   147,   148,   149,     0,
       0,     0,   179,   144,   145,   146,   147,   148,   149,     0,
       0,     0,   222,   144,   145,   146,   147,   148,   149,     0,
       0,     0,   227,   144,   145,   146,   147,   148,   149,     0,
       0,     0,   236,   144,   145,   146,   147,   148,   149,     0,
       0,     0,   237,    69,    70,    71,    72,    73,    74,    75,
      76
};

static const yytype_int16 yycheck[] =
{
      37,    11,    12,     8,    92,   169,   170,    13,   172,    24,
       0,    76,    43,    44,     4,    24,   143,    43,    44,   143,
      15,     7,     8,     9,    10,   152,    63,    22,   152,     3,
      22,     5,    59,    23,    16,    17,    18,    19,    75,    76,
       8,     9,    10,    58,    34,    22,    83,    84,     0,    58,
      22,    46,    58,   118,    58,    92,    30,    31,    25,    26,
      97,   225,    22,    58,    58,    60,    61,    25,    26,    14,
      15,    16,    17,    18,    19,   112,    66,     3,    68,   116,
       0,   118,    27,   171,     4,    59,    60,    61,    62,   216,
     217,    96,   216,   217,   131,   132,    70,    71,    72,    73,
      74,     9,    10,    23,   141,   142,    22,   144,   145,   146,
     147,   148,   149,   150,    34,    15,   153,   154,   155,    17,
      18,    19,    22,    22,    24,    14,    15,    16,    17,    18,
      19,   168,    18,    19,   171,    22,    26,   174,    27,    22,
      11,    12,    13,    15,    22,    22,    66,    42,    68,   159,
      22,    22,    15,   163,    22,    44,    45,    26,    58,    22,
      60,    61,   199,   137,   138,   202,    37,    38,    39,    40,
      41,    26,    22,    15,   211,   212,    58,    26,   215,    10,
      22,   218,    11,    12,    13,    58,    58,    58,    60,    61,
      23,    20,    21,    22,    26,    58,    59,    60,    61,    28,
      29,    23,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    11,    12,    13,    58,    19,    60,    61,
      30,    30,    20,    21,    22,    26,    23,    23,    23,    58,
      28,    29,    23,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    11,    12,    13,     5,     6,     7,
       8,     9,    10,    20,    23,    22,    49,   160,    96,   117,
      58,    28,    29,    52,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,     4,     5,     6,     7,     8,
       9,    10,     4,     5,     6,     7,     8,     9,    10,   113,
      58,    58,     6,     7,     8,     9,    10,    26,    14,    15,
      16,    17,    18,    19,    26,     4,     5,     6,     7,     8,
       9,    10,     4,     5,     6,     7,     8,     9,    10,     4,
       5,     6,     7,     8,     9,    10,    98,    26,    14,    15,
      16,    17,    18,    19,    26,    -1,    -1,    23,    -1,    -1,
      26,    26,     4,     5,     6,     7,     8,     9,    10,     4,
       5,     6,     7,     8,     9,    10,     4,     5,     6,     7,
       8,     9,    10,    -1,    26,    -1,    -1,    -1,    23,    15,
      16,    17,    18,    19,    -1,    23,     4,     5,     6,     7,
       8,     9,    10,    14,    15,    16,    17,    18,    19,    14,
      15,    16,    17,    18,    19,    23,    27,    -1,    -1,    -1,
      -1,    -1,    27,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    23,    -1,    -1,    26,    14,    15,    16,    17,
      18,    19,    -1,    -1,    -1,    23,    -1,    -1,    26,    14,
      15,    16,    17,    18,    19,    -1,    -1,    -1,    23,    -1,
      -1,    26,    14,    15,    16,    17,    18,    19,    14,    15,
      16,    17,    18,    19,    26,    -1,    -1,    23,    -1,    25,
      14,    15,    16,    17,    18,    19,    -1,    -1,    -1,    23,
      -1,    25,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    23,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    23,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    23,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    23,    14,    15,    16,    17,    18,    19,    -1,
      -1,    -1,    23,     3,     4,     5,     6,     7,     8,     9,
      10
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    11,    12,    13,    20,    22,    28,    29,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    58,
      63,    64,    65,    66,    67,    68,    69,    71,    81,    84,
      22,    22,    69,    65,    66,    69,    22,    22,    58,    77,
      78,    85,    59,    76,    86,    24,    58,    74,    82,    24,
      74,    74,    13,    58,    75,    79,    80,    83,    22,    22,
      22,    22,    22,    22,     0,    65,    66,    65,    66,     3,
       4,     5,     6,     7,     8,     9,    10,     3,    69,    69,
      21,    21,    23,    15,    22,    58,    60,    61,    85,    88,
      90,    91,    46,    58,    89,    90,    26,    42,    26,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      72,    73,    22,    26,    72,    83,    22,    26,    10,    58,
      84,    69,    69,    69,    69,    90,    69,    69,    69,    69,
      69,    22,    24,    70,    90,    70,    90,    26,    26,    90,
      90,    43,    44,    23,    14,    15,    16,    17,    18,    19,
      27,    89,    23,    27,    44,    45,    78,    90,    86,    25,
      26,    90,    82,    25,    90,    79,    70,    90,    26,    26,
      26,    26,    26,    23,    26,    90,    90,    69,    69,    23,
      90,    90,    67,    68,    90,    90,    90,    90,    90,    90,
      90,    67,    68,    90,    90,    90,    74,    73,    23,    26,
      74,    23,    26,    59,    87,    90,    88,    88,    89,    88,
      90,    26,    26,    23,    23,    27,    30,    30,    27,    90,
      90,    23,    23,    23,    23,    26,    23,    23,    90,    90,
      90,    67,    68,    67,    68,    90,    23,    23,    88,    23,
      25,    23,    25,    23
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    62,    63,    64,    64,    64,    65,    65,    65,    65,
      66,    66,    67,    67,    67,    67,    67,    67,    67,    68,
      68,    68,    68,    68,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    69,    69,    70,    70,    70,    70,    71,    71,
      71,    71,    71,    71,    71,    71,    72,    72,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    74,
      74,    75,    75,    76,    76,    77,    77,    78,    79,    79,
      80,    80,    81,    81,    81,    82,    82,    82,    83,    83,
      83,    84,    85,    86,    87,    88,    88,    88,    88,    89,
      89,    89,    89,    89,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    91,    91,    91
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     0,     2,     2,     2,     1,
       2,     1,     5,     5,     7,     7,     7,     3,     2,     5,
       5,     7,     3,     2,     2,     3,     3,     3,     3,     3,
       3,     6,     6,     6,     6,     8,     6,     3,     3,     3,
       6,     6,     3,     1,     5,     5,     5,     5,     5,     2,
       5,     2,     2,     2,     2,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     3,     3,
       2,     1,     6,     4,     1,     6,     4,     1,     6,     4,
       1,     1,     1,     1,     1,     5,     3,     3,     1,     2,
       5,     3,     3,     1,     3,     3,     3,     3,     3,     3,
       2,     3,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

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

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, spec_handle, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, btl_specification **spec_handle, yyscan_t scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  YYUSE (spec_handle);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, btl_specification **spec_handle, yyscan_t scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp, spec_handle, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, btl_specification **spec_handle, yyscan_t scanner)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , spec_handle, scanner);
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
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
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
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
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
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
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
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
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
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
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, btl_specification **spec_handle, yyscan_t scanner)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (spec_handle);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (btl_specification **spec_handle, yyscan_t scanner)
{
/* The lookahead symbol.  */
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
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
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
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
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
  case 2:
#line 207 "ting.y"
    { *spec_handle = (yyvsp[0].specification); }
#line 1706 "ting_parser.c"
    break;

  case 3:
#line 211 "ting.y"
    { (yyval.specification) = (yyvsp[0].extformula); }
#line 1712 "ting_parser.c"
    break;

  case 4:
#line 212 "ting.y"
    { (yyval.specification) = (yyvsp[0].extdeclaration); }
#line 1718 "ting_parser.c"
    break;

  case 6:
#line 217 "ting.y"
    { (yyval.extformula) = create_operation(op_and, (yyvsp[-1].extformula), (yyvsp[0].extformula), "%s ; %s"); }
#line 1724 "ting_parser.c"
    break;

  case 7:
#line 218 "ting.y"
    { (yyval.extformula) = create_operation(op_join, (yyvsp[-1].extformula), (yyvsp[0].extdeclaration), "%s ; %s"); }
#line 1730 "ting_parser.c"
    break;

  case 8:
#line 219 "ting.y"
    { (yyval.extformula) = create_operation(op_join, (yyvsp[-1].extdeclaration), (yyvsp[0].extformula), "%s ; %s"); }
#line 1736 "ting_parser.c"
    break;

  case 9:
#line 220 "ting.y"
    { (yyval.extformula) = (yyvsp[0].formblock); }
#line 1742 "ting_parser.c"
    break;

  case 10:
#line 224 "ting.y"
    { (yyval.extdeclaration) = create_operation(op_join, (yyvsp[-1].extdeclaration), (yyvsp[0].extdeclaration), "%s ; %s"); }
#line 1748 "ting_parser.c"
    break;

  case 11:
#line 225 "ting.y"
    { (yyval.extdeclaration) = (yyvsp[0].declblock); }
#line 1754 "ting_parser.c"
    break;

  case 12:
#line 229 "ting.y"
    { (yyval.formblock) = create_operation(op_forall, (yyvsp[0].formblock), (yyvsp[-2].varrange), "iter(%2$s) { %1$s ; }"); }
#line 1760 "ting_parser.c"
    break;

  case 13:
#line 230 "ting.y"
    { (yyval.formblock) = create_operation(op_when, (yyvsp[0].formblock), (yyvsp[-2].selrange), "when(%2$s) { %1$s ; }"); }
#line 1766 "ting_parser.c"
    break;

  case 14:
#line 232 "ting.y"
    { (yyval.formblock) = create_operation(op_and, create_operation(op_when, (yyvsp[-2].formblock), (yyvsp[-4].selrange), "when(%2$s) { %1$s ; }"),
                                      create_operation(op_when, (yyvsp[0].formblock), create_operation(op_neg_range, copy_specification((yyvsp[-4].selrange)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 1773 "ting_parser.c"
    break;

  case 15:
#line 235 "ting.y"
    { (yyval.formblock) = create_operation(op_join, create_operation(op_when, (yyvsp[-2].formblock), (yyvsp[-4].selrange), "when(%2$s) { %1$s ; }"),
                                       create_operation(op_when, (yyvsp[0].declblock), create_operation(op_neg_range, copy_specification((yyvsp[-4].selrange)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 1780 "ting_parser.c"
    break;

  case 16:
#line 238 "ting.y"
    { (yyval.formblock) = create_operation(op_join, create_operation(op_when, (yyvsp[-2].declblock), (yyvsp[-4].selrange), "when(%2$s) { %1$s ; }"),
                                       create_operation(op_when, (yyvsp[0].formblock), create_operation(op_neg_range, copy_specification((yyvsp[-4].selrange)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 1787 "ting_parser.c"
    break;

  case 17:
#line 240 "ting.y"
    { (yyval.formblock) = (yyvsp[-1].extformula); }
#line 1793 "ting_parser.c"
    break;

  case 18:
#line 241 "ting.y"
    { (yyval.formblock) = (yyvsp[-1].formula); }
#line 1799 "ting_parser.c"
    break;

  case 19:
#line 245 "ting.y"
    { (yyval.declblock) = create_operation(op_iter, (yyvsp[0].declblock), (yyvsp[-2].varrange), "iter(%2$s) { %1$s ; }"); }
#line 1805 "ting_parser.c"
    break;

  case 20:
#line 246 "ting.y"
    { (yyval.declblock) = create_operation(op_when, (yyvsp[0].declblock), (yyvsp[-2].selrange), "when(%2$s) { %1$s ; }"); }
#line 1811 "ting_parser.c"
    break;

  case 21:
#line 248 "ting.y"
    { (yyval.declblock) = create_operation(op_join, create_operation(op_when, (yyvsp[-2].declblock), (yyvsp[-4].selrange), "when(%2$s) { %1$s ; }"),
                                       create_operation(op_when, (yyvsp[0].declblock), create_operation(op_neg_range, copy_specification((yyvsp[-4].selrange)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 1818 "ting_parser.c"
    break;

  case 22:
#line 250 "ting.y"
    { (yyval.declblock) = (yyvsp[-1].extdeclaration); }
#line 1824 "ting_parser.c"
    break;

  case 23:
#line 251 "ting.y"
    { (yyval.declblock) = (yyvsp[-1].declaration); }
#line 1830 "ting_parser.c"
    break;

  case 24:
#line 255 "ting.y"
    { (yyval.formula) = create_operation(op_not, (yyvsp[0].formula), NULL, "(~ %s)"); }
#line 1836 "ting_parser.c"
    break;

  case 25:
#line 256 "ting.y"
    { (yyval.formula) = create_operation(op_and, (yyvsp[-2].formula), (yyvsp[0].formula), "(%s & %s)"); }
#line 1842 "ting_parser.c"
    break;

  case 26:
#line 257 "ting.y"
    { (yyval.formula) = create_operation(op_or, (yyvsp[-2].formula), (yyvsp[0].formula), "(%s | %s)"); }
#line 1848 "ting_parser.c"
    break;

  case 27:
#line 258 "ting.y"
    { (yyval.formula) = create_operation(op_delay, (yyvsp[-2].formula), (yyvsp[0].expression), "(%s @ %s)"); }
#line 1854 "ting_parser.c"
    break;

  case 28:
#line 259 "ting.y"
    { (yyval.formula) = create_operation(op_delay, (yyvsp[-2].formula), (yyvsp[0].expression), "(%s ? %s)"); }
#line 1860 "ting_parser.c"
    break;

  case 29:
#line 260 "ting.y"
    { (yyval.formula) = create_operation(op_at, (yyvsp[-2].formula), (yyvsp[0].interval), "(%s @ %s)"); }
#line 1866 "ting_parser.c"
    break;

  case 30:
#line 261 "ting.y"
    { (yyval.formula) = create_operation(op_happen, (yyvsp[-2].formula), (yyvsp[0].interval), "(%s ? %s)"); }
#line 1872 "ting_parser.c"
    break;

  case 31:
#line 262 "ting.y"
    { (yyval.formula) = create_operation(op_since, (yyvsp[-3].formula), (yyvsp[-1].formula), "since(%s , %s)"); }
#line 1878 "ting_parser.c"
    break;

  case 32:
#line 263 "ting.y"
    { (yyval.formula) = create_operation(op_until, (yyvsp[-3].formula), (yyvsp[-1].formula), "until(%s , %s)"); }
#line 1884 "ting_parser.c"
    break;

  case 33:
#line 264 "ting.y"
    { (yyval.formula) = create_operation(op_forall, (yyvsp[-3].formula), (yyvsp[-1].varrange), "forall(%s , %s)"); }
#line 1890 "ting_parser.c"
    break;

  case 34:
#line 265 "ting.y"
    { (yyval.formula) = create_operation(op_exists, (yyvsp[-3].formula), (yyvsp[-1].varrange), "exists(%s , %s)"); }
#line 1896 "ting_parser.c"
    break;

  case 35:
#line 266 "ting.y"
    { (yyval.formula) = create_operation(op_one, (yyvsp[-5].formula), create_operation(op_one_check, (yyvsp[-3].selrange), (yyvsp[-1].varrange), "%s , %s"), "one(%s , %s)"); }
#line 1902 "ting_parser.c"
    break;

  case 36:
#line 267 "ting.y"
    { (yyval.formula) = create_operation(op_unique, (yyvsp[-3].formula), (yyvsp[-1].varrange), "unique(%s , %s)"); }
#line 1908 "ting_parser.c"
    break;

  case 37:
#line 268 "ting.y"
    { (yyval.formula) = create_operation(op_imply, (yyvsp[-2].formula), (yyvsp[0].formula), "(%s --> %s)"); }
#line 1914 "ting_parser.c"
    break;

  case 38:
#line 269 "ting.y"
    { (yyval.formula) = create_operation(op_imply, (yyvsp[0].formula), (yyvsp[-2].formula), "(%2$s <-- %1$s)"); }
#line 1920 "ting_parser.c"
    break;

  case 39:
#line 270 "ting.y"
    { (yyval.formula) = create_operation(op_eqv, (yyvsp[-2].formula), (yyvsp[0].formula), "(%s == %s)"); }
#line 1926 "ting_parser.c"
    break;

  case 40:
#line 271 "ting.y"
    { (yyval.formula) = create_operation(op_code, (yyvsp[-3].varname), (yyvsp[-1].string), "code(%s , %s)"); }
#line 1932 "ting_parser.c"
    break;

  case 41:
#line 272 "ting.y"
    { (yyval.formula) = create_operation(op_code_num, (yyvsp[-3].varname), (yyvsp[-1].expression), "code(%s , %s)"); }
#line 1938 "ting_parser.c"
    break;

  case 42:
#line 273 "ting.y"
    { (yyval.formula) = (yyvsp[-1].formula); }
#line 1944 "ting_parser.c"
    break;

  case 43:
#line 274 "ting.y"
    { (yyval.formula) = (yyvsp[0].indexedname); }
#line 1950 "ting_parser.c"
    break;

  case 44:
#line 278 "ting.y"
    { (yyval.interval) = create_operation(op_interval_1, (yyvsp[-3].expression), (yyvsp[-1].expression), "[%s , %s]"); }
#line 1956 "ting_parser.c"
    break;

  case 45:
#line 279 "ting.y"
    { (yyval.interval) = create_operation(op_interval_2, (yyvsp[-3].expression), (yyvsp[-1].expression), "(%s , %s]"); }
#line 1962 "ting_parser.c"
    break;

  case 46:
#line 280 "ting.y"
    { (yyval.interval) = create_operation(op_interval_3, (yyvsp[-3].expression), (yyvsp[-1].expression), "[%s , %s)"); }
#line 1968 "ting_parser.c"
    break;

  case 47:
#line 281 "ting.y"
    { (yyval.interval) = create_operation(op_interval_4, (yyvsp[-3].expression), (yyvsp[-1].expression), "(%s , %s)"); }
#line 1974 "ting_parser.c"
    break;

  case 48:
#line 285 "ting.y"
    { (yyval.declaration) = create_operation(op_input, (yyvsp[0].decllist), (yyvsp[-2].io_decllist), "input[%2$s] %1$s"); }
#line 1980 "ting_parser.c"
    break;

  case 49:
#line 286 "ting.y"
    { (yyval.declaration) = create_operation(op_input, (yyvsp[0].decllist), create_operation(op_join_qual, create_operation(op_join_qual, create_operation(op_join_qual,
      create_ground(op_ioqual1, "", io_any), create_ground(op_ioqual2, "", io_binary), "%s , %s"), create_ground(op_ioqual3, "", io_unknown), "%s , %s"),
      create_ground(op_ioqual4, "", io_raw), "%s , %s"), "input[%2$s] %1$s"); }
#line 1988 "ting_parser.c"
    break;

  case 50:
#line 289 "ting.y"
    { (yyval.declaration) = create_operation(op_output, (yyvsp[0].decllist), (yyvsp[-2].io_decllist), "output[%2$s] %1$s"); }
#line 1994 "ting_parser.c"
    break;

  case 51:
#line 290 "ting.y"
    { (yyval.declaration) = create_operation(op_output, (yyvsp[0].decllist), create_operation(op_join_qual, create_operation(op_join_qual, create_operation(op_join_qual,
      create_ground(op_ioqual1, "", io_any), create_ground(op_ioqual2, "", io_binary), "%s , %s"), create_ground(op_ioqual3, "", io_unknown), "%s , %s"),
      create_ground(op_ioqual4, "", io_raw), "%s , %s"), "output[%2$s] %1$s"); }
#line 2002 "ting_parser.c"
    break;

  case 52:
#line 293 "ting.y"
    { (yyval.declaration) = create_operation(op_aux, (yyvsp[0].decllist), NULL, "aux %s"); }
#line 2008 "ting_parser.c"
    break;

  case 53:
#line 294 "ting.y"
    { (yyval.declaration) = create_operation(op_init, (yyvsp[0].eventlist), NULL, "init %s"); }
#line 2014 "ting_parser.c"
    break;

  case 54:
#line 295 "ting.y"
    { (yyval.declaration) = (yyvsp[0].assignlist); }
#line 2020 "ting_parser.c"
    break;

  case 55:
#line 296 "ting.y"
    { (yyval.declaration) = (yyvsp[0].filelist); }
#line 2026 "ting_parser.c"
    break;

  case 56:
#line 300 "ting.y"
    { (yyval.io_decllist) = create_operation(op_join_qual, (yyvsp[-2].io_decllist), (yyvsp[0].io_decl), "%s , %s"); }
#line 2032 "ting_parser.c"
    break;

  case 57:
#line 301 "ting.y"
    { (yyval.io_decllist) = (yyvsp[0].io_decl); }
#line 2038 "ting_parser.c"
    break;

  case 58:
#line 305 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual1, "any", io_any); }
#line 2044 "ting_parser.c"
    break;

  case 59:
#line 306 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual1, "ipc", io_ipc); }
#line 2050 "ting_parser.c"
    break;

  case 60:
#line 307 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual1, "file", io_file); }
#line 2056 "ting_parser.c"
    break;

  case 61:
#line 308 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual2, "binary", io_binary); }
#line 2062 "ting_parser.c"
    break;

  case 62:
#line 309 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual2, "packed", io_packed); }
#line 2068 "ting_parser.c"
    break;

  case 63:
#line 310 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual3, "unknown", io_unknown); }
#line 2074 "ting_parser.c"
    break;

  case 64:
#line 311 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual3, "false", io_false); }
#line 2080 "ting_parser.c"
    break;

  case 65:
#line 312 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual3, "true", io_true); }
#line 2086 "ting_parser.c"
    break;

  case 66:
#line 313 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual4, "raw", io_raw); }
#line 2092 "ting_parser.c"
    break;

  case 67:
#line 314 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual4, "filter", io_filter); }
#line 2098 "ting_parser.c"
    break;

  case 68:
#line 315 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual4, "omit", io_omit); }
#line 2104 "ting_parser.c"
    break;

  case 69:
#line 319 "ting.y"
    { (yyval.decllist) = create_operation(op_join, (yyvsp[-2].decllist), (yyvsp[0].declname), "%s , %s"); }
#line 2110 "ting_parser.c"
    break;

  case 70:
#line 320 "ting.y"
    { (yyval.decllist) = (yyvsp[0].declname); }
#line 2116 "ting_parser.c"
    break;

  case 71:
#line 324 "ting.y"
    { (yyval.eventlist) = create_operation(op_join, (yyvsp[-2].eventlist), (yyvsp[0].eventname), "%s , %s"); }
#line 2122 "ting_parser.c"
    break;

  case 72:
#line 325 "ting.y"
    { (yyval.eventlist) = (yyvsp[0].eventname); }
#line 2128 "ting_parser.c"
    break;

  case 73:
#line 329 "ting.y"
    { (yyval.filelist) = create_operation(op_join, (yyvsp[-2].filelist), (yyvsp[0].filename), "%s , %s"); }
#line 2134 "ting_parser.c"
    break;

  case 74:
#line 330 "ting.y"
    { (yyval.filelist) = (yyvsp[0].filename); }
#line 2140 "ting_parser.c"
    break;

  case 75:
#line 334 "ting.y"
    { (yyval.assignlist) = create_operation(op_join, (yyvsp[-2].assignlist), (yyvsp[0].assignment), "%s ; %s"); }
#line 2146 "ting_parser.c"
    break;

  case 76:
#line 335 "ting.y"
    { (yyval.assignlist) = (yyvsp[0].assignment); }
#line 2152 "ting_parser.c"
    break;

  case 77:
#line 339 "ting.y"
    { (yyval.assignment) = create_operation(op_define, (yyvsp[-2].constname), (yyvsp[0].expression), "define %s = %s"); }
#line 2158 "ting_parser.c"
    break;

  case 78:
#line 343 "ting.y"
    { (yyval.eventname) = create_operation(op_var_at, (yyvsp[-2].qualname), (yyvsp[0].expression), "%s @ %s"); }
#line 2164 "ting_parser.c"
    break;

  case 79:
#line 344 "ting.y"
    { (yyval.eventname) = create_operation(op_var_at, (yyvsp[-2].qualname), (yyvsp[0].interval), "%s @ %s"); }
#line 2170 "ting_parser.c"
    break;

  case 80:
#line 348 "ting.y"
    { (yyval.qualname) = create_operation(op_not, (yyvsp[0].initname), NULL, "~ %s"); }
#line 2176 "ting_parser.c"
    break;

  case 81:
#line 349 "ting.y"
    { (yyval.qualname) = (yyvsp[0].initname); }
#line 2182 "ting_parser.c"
    break;

  case 82:
#line 353 "ting.y"
    { (yyval.indexedname) = create_operation(op_matrix, create_ground(op_name, (yyvsp[-5].symbol), 0),
      create_operation(op_interval_1, (yyvsp[-3].expression), (yyvsp[-1].expression), "%s , %s"), "%s(%s)"); }
#line 2189 "ting_parser.c"
    break;

  case 83:
#line 355 "ting.y"
    { (yyval.indexedname) = create_operation(op_vector, create_ground(op_name, (yyvsp[-3].symbol), 0), (yyvsp[-1].expression), "%s(%s)"); }
#line 2195 "ting_parser.c"
    break;

  case 84:
#line 356 "ting.y"
    { (yyval.indexedname) = (yyvsp[0].varname); }
#line 2201 "ting_parser.c"
    break;

  case 85:
#line 360 "ting.y"
    { (yyval.declname) = create_operation(op_matrix, create_ground(op_dname, (yyvsp[-5].symbol), 0),
      create_operation(op_interval_1, (yyvsp[-3].expression), (yyvsp[-1].expression), "%s , %s"), "%s(%s)"); }
#line 2208 "ting_parser.c"
    break;

  case 86:
#line 362 "ting.y"
    { (yyval.declname) = create_operation(op_vector, create_ground(op_dname, (yyvsp[-3].symbol), 0), (yyvsp[-1].expression), "%s(%s)"); }
#line 2214 "ting_parser.c"
    break;

  case 87:
#line 363 "ting.y"
    { (yyval.declname) = create_ground(op_dname, (yyvsp[0].symbol), 0); }
#line 2220 "ting_parser.c"
    break;

  case 88:
#line 367 "ting.y"
    { (yyval.initname) = create_operation(op_matrix, create_ground(op_iname, (yyvsp[-5].symbol), 0),
      create_operation(op_interval_1, (yyvsp[-3].expression), (yyvsp[-1].expression), "%s , %s"), "%s(%s)"); }
#line 2227 "ting_parser.c"
    break;

  case 89:
#line 369 "ting.y"
    { (yyval.initname) = create_operation(op_vector, create_ground(op_iname, (yyvsp[-3].symbol), 0), (yyvsp[-1].expression), "%s(%s)"); }
#line 2233 "ting_parser.c"
    break;

  case 90:
#line 370 "ting.y"
    { (yyval.initname) = create_ground(op_iname, (yyvsp[0].symbol), 0); }
#line 2239 "ting_parser.c"
    break;

  case 91:
#line 374 "ting.y"
    { (yyval.varname) = create_ground(op_name, (yyvsp[0].symbol), 0); }
#line 2245 "ting_parser.c"
    break;

  case 92:
#line 378 "ting.y"
    { (yyval.constname) = create_ground(op_cname, (yyvsp[0].symbol), 0); }
#line 2251 "ting_parser.c"
    break;

  case 93:
#line 382 "ting.y"
    { (yyval.filename) = create_ground(op_fname, (yyvsp[0].symbol), 0); }
#line 2257 "ting_parser.c"
    break;

  case 94:
#line 386 "ting.y"
    { (yyval.string) = create_ground(op_string, (yyvsp[0].symbol), 0); }
#line 2263 "ting_parser.c"
    break;

  case 95:
#line 390 "ting.y"
    { (yyval.varrange) = create_operation(op_in, (yyvsp[-4].constname), create_operation(op_interval_1, (yyvsp[-2].expression), (yyvsp[0].expression), "%s : %s"), "%s in %s"); }
#line 2269 "ting_parser.c"
    break;

  case 96:
#line 391 "ting.y"
    { (yyval.varrange) = create_operation(op_in, (yyvsp[-2].constname), create_operation(op_interval_3, create_ground(op_number, "", 0), (yyvsp[0].expression), "%s : %s - 1"), "%s on %s"); }
#line 2275 "ting_parser.c"
    break;

  case 97:
#line 392 "ting.y"
    { (yyval.varrange) = create_operation(op_interval_1, (yyvsp[-2].expression), (yyvsp[0].expression), "%s : %s"); }
#line 2281 "ting_parser.c"
    break;

  case 98:
#line 393 "ting.y"
    { (yyval.varrange) = create_operation(op_interval_3, create_ground(op_number, "", 0), (yyvsp[0].expression), "%s : %s - 1"); }
#line 2287 "ting_parser.c"
    break;

  case 99:
#line 397 "ting.y"
    { (yyval.selrange) = create_operation(op_neg_range, (yyvsp[0].selrange), NULL, "not %s"); }
#line 2293 "ting_parser.c"
    break;

  case 100:
#line 398 "ting.y"
    { (yyval.selrange) = create_operation(op_in, (yyvsp[-4].expression), create_operation(op_interval_1, (yyvsp[-2].expression), (yyvsp[0].expression), "%s : %s"), "%s in %s"); }
#line 2299 "ting_parser.c"
    break;

  case 101:
#line 399 "ting.y"
    { (yyval.selrange) = create_operation(op_in, (yyvsp[-2].expression), create_operation(op_interval_1, (yyvsp[0].expression), copy_specification((yyvsp[0].expression)), "%s : %s"), "%s is %s"); }
#line 2305 "ting_parser.c"
    break;

  case 102:
#line 400 "ting.y"
    { (yyval.selrange) = create_operation(op_interval_1, (yyvsp[-2].expression), (yyvsp[0].expression), "%s : %s"); }
#line 2311 "ting_parser.c"
    break;

  case 103:
#line 401 "ting.y"
    { (yyval.selrange) = create_operation(op_interval_1, (yyvsp[0].expression), copy_specification((yyvsp[0].expression)), "%s : %s"); }
#line 2317 "ting_parser.c"
    break;

  case 104:
#line 405 "ting.y"
    { (yyval.expression) = create_operation(op_plus, (yyvsp[-2].expression), (yyvsp[0].expression), "%s + %s"); }
#line 2323 "ting_parser.c"
    break;

  case 105:
#line 406 "ting.y"
    { (yyval.expression) = create_operation(op_minus, (yyvsp[-2].expression), (yyvsp[0].expression), "%s - %s"); }
#line 2329 "ting_parser.c"
    break;

  case 106:
#line 407 "ting.y"
    { (yyval.expression) = create_operation(op_mul, (yyvsp[-2].expression), (yyvsp[0].expression), "%s * %s"); }
#line 2335 "ting_parser.c"
    break;

  case 107:
#line 408 "ting.y"
    { (yyval.expression) = create_operation(op_div, (yyvsp[-2].expression), (yyvsp[0].expression), "%s / %s"); }
#line 2341 "ting_parser.c"
    break;

  case 108:
#line 409 "ting.y"
    { (yyval.expression) = create_operation(op_mod, (yyvsp[-2].expression), (yyvsp[0].expression), "%s % %s"); }
#line 2347 "ting_parser.c"
    break;

  case 109:
#line 410 "ting.y"
    { (yyval.expression) = create_operation(op_pow, (yyvsp[-2].expression), (yyvsp[0].expression), "%s ^ %s"); }
#line 2353 "ting_parser.c"
    break;

  case 110:
#line 411 "ting.y"
    { (yyval.expression) = create_operation(op_chs, (yyvsp[0].expression), NULL, "- %s"); }
#line 2359 "ting_parser.c"
    break;

  case 111:
#line 412 "ting.y"
    { (yyval.expression) = (yyvsp[-1].expression); }
#line 2365 "ting_parser.c"
    break;

  case 112:
#line 413 "ting.y"
    { (yyval.expression) = (yyvsp[0].number); }
#line 2371 "ting_parser.c"
    break;

  case 113:
#line 417 "ting.y"
    { (yyval.number) = create_ground(op_number, "", (yyvsp[0].value)); }
#line 2377 "ting_parser.c"
    break;

  case 114:
#line 418 "ting.y"
    { (yyval.number) = create_ground(op_iterator, (yyvsp[0].symbol), 0); }
#line 2383 "ting_parser.c"
    break;

  case 115:
#line 419 "ting.y"
    { (yyval.number) = create_ground(op_constant, (yyvsp[0].symbol), 0); }
#line 2389 "ting_parser.c"
    break;


#line 2393 "ting_parser.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, spec_handle, scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, spec_handle, scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp, yylsp, spec_handle, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, spec_handle, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
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
                  yystos[*yyssp], yyvsp, yylsp, spec_handle, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 422 "ting.y"


