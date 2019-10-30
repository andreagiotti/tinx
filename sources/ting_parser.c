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
    TOKEN_LBRACKET = 273,
    TOKEN_RBRACKET = 274,
    TOKEN_LPAREN = 275,
    TOKEN_RPAREN = 276,
    TOKEN_LSQUARED = 277,
    TOKEN_RSQUARED = 278,
    TOKEN_COMMA = 279,
    TOKEN_COLON = 280,
    TOKEN_ITER = 281,
    TOKEN_WHEN = 282,
    TOKEN_ELSE = 283,
    TOKEN_DEFINE = 284,
    TOKEN_INCLUDE = 285,
    TOKEN_INPUT = 286,
    TOKEN_OUTPUT = 287,
    TOKEN_AUX = 288,
    TOKEN_INIT = 289,
    TOKEN_CODE = 290,
    TOKEN_FORALL = 291,
    TOKEN_EXISTS = 292,
    TOKEN_ONE = 293,
    TOKEN_UNIQUE = 294,
    TOKEN_EQUAL = 295,
    TOKEN_ON = 296,
    TOKEN_IN = 297,
    TOKEN_IS = 298,
    TOKEN_NEG = 299,
    TOKEN_ANY = 300,
    TOKEN_IPC = 301,
    TOKEN_FILE = 302,
    TOKEN_BINARY = 303,
    TOKEN_PACKED = 304,
    TOKEN_UNKNOWN = 305,
    TOKEN_FALSE = 306,
    TOKEN_TRUE = 307,
    TOKEN_NAME = 308,
    TOKEN_STRING = 309,
    TOKEN_ITERATOR = 310,
    TOKEN_NUMBER = 311
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

#line 255 "ting_parser.c"

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
#define YYLAST   495

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  30
/* YYNRULES -- Number of rules.  */
#define YYNRULES  110
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  237

#define YYUNDEFTOK  2
#define YYMAXUTOK   311

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
      55,    56
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   200,   200,   204,   205,   206,   210,   211,   212,   213,
     217,   218,   222,   223,   224,   227,   230,   233,   234,   238,
     239,   240,   243,   244,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   271,   272,   273,   274,   278,   279,
     281,   282,   284,   285,   286,   287,   291,   292,   296,   297,
     298,   299,   300,   301,   302,   303,   307,   308,   312,   313,
     317,   318,   322,   323,   327,   331,   332,   336,   337,   341,
     343,   344,   348,   350,   351,   355,   357,   358,   362,   366,
     370,   374,   378,   379,   380,   381,   385,   386,   387,   388,
     389,   393,   394,   395,   396,   397,   398,   399,   403,   404,
     405
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
  "TOKEN_MINUS", "TOKEN_MUL", "TOKEN_DIV", "TOKEN_LBRACKET",
  "TOKEN_RBRACKET", "TOKEN_LPAREN", "TOKEN_RPAREN", "TOKEN_LSQUARED",
  "TOKEN_RSQUARED", "TOKEN_COMMA", "TOKEN_COLON", "TOKEN_ITER",
  "TOKEN_WHEN", "TOKEN_ELSE", "TOKEN_DEFINE", "TOKEN_INCLUDE",
  "TOKEN_INPUT", "TOKEN_OUTPUT", "TOKEN_AUX", "TOKEN_INIT", "TOKEN_CODE",
  "TOKEN_FORALL", "TOKEN_EXISTS", "TOKEN_ONE", "TOKEN_UNIQUE",
  "TOKEN_EQUAL", "TOKEN_ON", "TOKEN_IN", "TOKEN_IS", "TOKEN_NEG",
  "TOKEN_ANY", "TOKEN_IPC", "TOKEN_FILE", "TOKEN_BINARY", "TOKEN_PACKED",
  "TOKEN_UNKNOWN", "TOKEN_FALSE", "TOKEN_TRUE", "TOKEN_NAME",
  "TOKEN_STRING", "TOKEN_ITERATOR", "TOKEN_NUMBER", "$accept", "input",
  "spec", "xform", "xdecl", "fblock", "dblock", "form", "inter", "decl",
  "iodlist", "iod", "dlist", "elist", "flist", "alist", "assign", "ename",
  "qname", "xname", "dname", "iname", "name", "cname", "fname", "str",
  "vrange", "srange", "expr", "num", YY_NULLPTR
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
     305,   306,   307,   308,   309,   310,   311
};
# endif

#define YYPACT_NINF -153

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-153)))

#define YYTABLE_NINF -90

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     224,    -6,    17,   253,   224,   253,    30,    34,   -32,   -38,
      -2,     0,     9,    -4,    55,    61,    69,    70,    74,    75,
     103,  -153,   224,   224,  -153,  -153,   444,   107,  -153,  -153,
     253,   253,  -153,   166,   195,   374,    12,   110,  -153,    85,
    -153,    77,  -153,    94,  -153,   410,   104,   109,  -153,   410,
     109,   109,    81,   108,   112,  -153,   117,  -153,    84,   253,
     253,   253,   253,   120,  -153,   224,   224,   224,   224,  -153,
     253,   253,   253,   253,   253,     8,     8,  -153,   290,   311,
    -153,  -153,  -153,   120,   120,   -34,  -153,  -153,   -23,   118,
      19,  -153,   110,  -153,   135,   268,   -32,   120,   -38,  -153,
    -153,  -153,  -153,  -153,  -153,  -153,  -153,     1,  -153,   120,
       9,    35,  -153,   120,    -4,     8,  -153,   138,   318,   339,
     346,   367,   315,   435,   481,    33,   133,    60,   120,   120,
    -153,   478,  -153,   478,   253,   253,    67,   173,   120,   120,
     224,   120,   120,   120,   120,   120,  -153,   224,   120,   120,
     120,  -153,   478,  -153,     9,   410,   343,  -153,     9,   400,
    -153,  -153,   478,   225,    12,    12,   110,    12,  -153,   120,
     411,   422,   392,   399,  -153,   478,   106,  -153,  -153,   130,
      67,   147,  -153,   478,   141,   142,   478,   287,   478,   109,
    -153,  -153,   120,   109,  -153,   120,  -153,   151,   254,   159,
     161,   167,   188,   449,   120,   120,  -153,  -153,   120,   224,
     224,   120,   457,   465,  -153,  -153,  -153,  -153,    12,  -153,
    -153,   136,   371,   478,  -153,  -153,  -153,  -153,   478,  -153,
    -153,   189,  -153,  -153,  -153,  -153,  -153
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    88,
       0,     2,     3,     4,     9,    11,     0,     0,    43,    81,
       0,     0,    24,     0,     0,     0,     0,     0,    89,    54,
      73,     0,    90,    55,    71,     0,    84,    49,    67,     0,
      51,    52,     0,    87,    53,    69,     0,    78,     0,     0,
       0,     0,     0,     0,     1,     6,     7,     8,    10,    18,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      17,    22,    42,     0,     0,   110,   109,   108,     0,     0,
      95,   107,     0,   110,     0,   100,     0,     0,     0,    58,
      59,    60,    61,    62,    63,    64,    65,     0,    57,     0,
       0,     0,    77,     0,     0,     0,    88,     0,     0,     0,
       0,     0,     0,    39,    37,    38,    26,    25,     0,     0,
      30,    28,    29,    27,     0,     0,   105,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    96,     0,     0,     0,
       0,    72,    74,    70,     0,     0,     0,    66,     0,     0,
      68,    76,    75,     0,     0,     0,     0,     0,    80,     0,
       0,     0,     0,     0,   106,    93,     0,    12,    19,   101,
     102,   103,   104,    94,    13,    20,    99,     0,    98,    48,
      56,    83,     0,    50,    86,     0,    91,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,     0,     0,
       0,     0,     0,     0,    40,    41,    33,    34,     0,    36,
      79,     0,     0,    92,    14,    15,    16,    21,    97,    82,
      85,     0,    47,    45,    46,    44,    35
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -153,  -153,  -153,    48,   115,  -136,  -130,    26,   -70,  -153,
     162,    57,   -10,  -153,  -153,  -153,   121,   102,  -153,  -153,
     113,   168,   160,    -3,   140,  -153,  -152,   -89,   -37,  -153
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    20,    21,    65,    66,    24,    25,    26,   130,    27,
     107,   108,    47,    54,    43,    39,    40,    55,    56,    28,
      48,    57,    29,    88,    44,   197,    89,    94,    90,    91
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      95,    50,    51,   146,   177,    41,   132,   -89,   -89,    52,
     178,   184,   199,   200,    30,   202,    42,   185,   138,   139,
      45,    38,    49,    83,   154,   155,   122,    83,   128,    32,
     129,    35,    84,   141,   142,   143,   144,    31,   131,   133,
      73,    74,    75,    76,   145,   161,   136,   137,    22,    53,
      36,    46,    33,    46,    37,    95,    78,    79,   158,   155,
     152,    93,    46,    86,    87,    85,   231,    86,    87,    75,
      76,    67,   156,   224,   226,    58,   159,   201,   162,   225,
     227,    59,    67,   143,   144,   118,   119,   120,   121,    60,
      61,   170,   171,    41,    62,    63,   123,   124,   125,   126,
     127,   175,   176,    64,   179,   180,   181,   182,   183,    96,
      77,   186,   187,   188,    67,    23,    67,    97,    98,    34,
     141,   142,   143,   144,   109,    83,   198,   115,   113,    95,
      84,   208,   203,   110,    53,    83,   114,   116,    68,   140,
      84,    74,    75,    76,   189,   142,   143,   144,   193,    68,
     141,   142,   143,   144,    92,   212,   147,   232,   213,   233,
     172,   173,   163,    93,   144,    86,    87,   221,   222,   209,
     210,   223,   214,    93,   228,    86,    87,     1,     2,     3,
     216,    68,   217,    68,     4,    80,     5,   141,   142,   143,
     144,   218,     6,     7,   174,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,     1,     2,     3,   219,
     236,   111,   190,     4,    81,     5,   160,   151,   117,    19,
     112,     6,     7,   157,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     1,     2,     3,   153,     0,
      83,     0,     4,     0,     5,    84,     0,     0,    19,     0,
       6,     7,     0,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,     1,     2,     3,     0,   141,   142,
     143,   144,     0,     5,     0,   215,     0,    19,    93,   196,
      86,    87,   141,   142,   143,   144,     0,     0,    14,    15,
      16,    17,    18,   148,    70,    71,    72,    73,    74,    75,
      76,   141,   142,   143,   144,     0,    19,     0,     0,     0,
     149,   150,   211,     0,   134,    70,    71,    72,    73,    74,
      75,    76,    70,    71,    72,    73,    74,    75,    76,   141,
     142,   143,   144,     0,     0,   135,   168,     0,     0,   169,
       0,     0,   164,    70,    71,    72,    73,    74,    75,    76,
      70,    71,    72,    73,    74,    75,    76,   141,   142,   143,
     144,     0,     0,   165,   191,     0,     0,   192,     0,     0,
     166,    70,    71,    72,    73,    74,    75,    76,    70,    71,
      72,    73,    74,    75,    76,   141,   142,   143,   144,     0,
       0,   167,   234,     0,   235,    82,    70,    71,    72,    73,
      74,    75,    76,    70,    71,    72,    73,    74,    75,    76,
       0,     0,     0,   206,   141,   142,   143,   144,     0,     0,
     207,   194,     0,     0,   195,   141,   142,   143,   144,     0,
       0,     0,   174,     0,     0,   204,   141,   142,   143,   144,
      71,    72,    73,    74,    75,    76,   205,    69,    70,    71,
      72,    73,    74,    75,    76,    99,   100,   101,   102,   103,
     104,   105,   106,   141,   142,   143,   144,     0,     0,     0,
     220,   141,   142,   143,   144,     0,     0,     0,   229,   141,
     142,   143,   144,     0,     0,     0,   230,    72,    73,    74,
      75,    76,   141,   142,   143,   144
};

static const yytype_int16 yycheck[] =
{
      37,    11,    12,    92,   140,     8,    76,    41,    42,    13,
     140,   147,   164,   165,    20,   167,    54,   147,    41,    42,
      22,    53,    22,    15,    23,    24,    63,    15,    20,     3,
      22,     5,    20,    14,    15,    16,    17,    20,    75,    76,
       7,     8,     9,    10,    25,   115,    83,    84,     0,    53,
      20,    53,     4,    53,    20,    92,    30,    31,    23,    24,
      97,    53,    53,    55,    56,    53,   218,    55,    56,     9,
      10,    23,   109,   209,   210,    20,   113,   166,   115,   209,
     210,    20,    34,    16,    17,    59,    60,    61,    62,    20,
      20,   128,   129,    96,    20,    20,    70,    71,    72,    73,
      74,   138,   139,     0,   141,   142,   143,   144,   145,    24,
       3,   148,   149,   150,    66,     0,    68,    40,    24,     4,
      14,    15,    16,    17,    20,    15,   163,    10,    20,   166,
      20,    25,   169,    24,    53,    15,    24,    53,    23,    21,
      20,     8,     9,    10,   154,    15,    16,    17,   158,    34,
      14,    15,    16,    17,    44,   192,    21,    21,   195,    23,
     134,   135,    24,    53,    17,    55,    56,   204,   205,    28,
      28,   208,    21,    53,   211,    55,    56,    11,    12,    13,
      21,    66,    21,    68,    18,    19,    20,    14,    15,    16,
      17,    24,    26,    27,    21,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    11,    12,    13,    21,
      21,    49,   155,    18,    19,    20,   114,    96,    58,    53,
      52,    26,    27,   110,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    11,    12,    13,    98,    -1,
      15,    -1,    18,    -1,    20,    20,    -1,    -1,    53,    -1,
      26,    27,    -1,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    11,    12,    13,    -1,    14,    15,
      16,    17,    -1,    20,    -1,    21,    -1,    53,    53,    54,
      55,    56,    14,    15,    16,    17,    -1,    -1,    35,    36,
      37,    38,    39,    25,     4,     5,     6,     7,     8,     9,
      10,    14,    15,    16,    17,    -1,    53,    -1,    -1,    -1,
      42,    43,    25,    -1,    24,     4,     5,     6,     7,     8,
       9,    10,     4,     5,     6,     7,     8,     9,    10,    14,
      15,    16,    17,    -1,    -1,    24,    21,    -1,    -1,    24,
      -1,    -1,    24,     4,     5,     6,     7,     8,     9,    10,
       4,     5,     6,     7,     8,     9,    10,    14,    15,    16,
      17,    -1,    -1,    24,    21,    -1,    -1,    24,    -1,    -1,
      24,     4,     5,     6,     7,     8,     9,    10,     4,     5,
       6,     7,     8,     9,    10,    14,    15,    16,    17,    -1,
      -1,    24,    21,    -1,    23,    21,     4,     5,     6,     7,
       8,     9,    10,     4,     5,     6,     7,     8,     9,    10,
      -1,    -1,    -1,    21,    14,    15,    16,    17,    -1,    -1,
      21,    21,    -1,    -1,    24,    14,    15,    16,    17,    -1,
      -1,    -1,    21,    -1,    -1,    24,    14,    15,    16,    17,
       5,     6,     7,     8,     9,    10,    24,     3,     4,     5,
       6,     7,     8,     9,    10,    45,    46,    47,    48,    49,
      50,    51,    52,    14,    15,    16,    17,    -1,    -1,    -1,
      21,    14,    15,    16,    17,    -1,    -1,    -1,    21,    14,
      15,    16,    17,    -1,    -1,    -1,    21,     6,     7,     8,
       9,    10,    14,    15,    16,    17
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    11,    12,    13,    18,    20,    26,    27,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    53,
      58,    59,    60,    61,    62,    63,    64,    66,    76,    79,
      20,    20,    64,    60,    61,    64,    20,    20,    53,    72,
      73,    80,    54,    71,    81,    22,    53,    69,    77,    22,
      69,    69,    13,    53,    70,    74,    75,    78,    20,    20,
      20,    20,    20,    20,     0,    60,    61,    60,    61,     3,
       4,     5,     6,     7,     8,     9,    10,     3,    64,    64,
      19,    19,    21,    15,    20,    53,    55,    56,    80,    83,
      85,    86,    44,    53,    84,    85,    24,    40,    24,    45,
      46,    47,    48,    49,    50,    51,    52,    67,    68,    20,
      24,    67,    78,    20,    24,    10,    53,    79,    64,    64,
      64,    64,    85,    64,    64,    64,    64,    64,    20,    22,
      65,    85,    65,    85,    24,    24,    85,    85,    41,    42,
      21,    14,    15,    16,    17,    25,    84,    21,    25,    42,
      43,    73,    85,    81,    23,    24,    85,    77,    23,    85,
      74,    65,    85,    24,    24,    24,    24,    24,    21,    24,
      85,    85,    64,    64,    21,    85,    85,    62,    63,    85,
      85,    85,    85,    85,    62,    63,    85,    85,    85,    69,
      68,    21,    24,    69,    21,    24,    54,    82,    85,    83,
      83,    84,    83,    85,    24,    24,    21,    21,    25,    28,
      28,    25,    85,    85,    21,    21,    21,    21,    24,    21,
      21,    85,    85,    85,    62,    63,    62,    63,    85,    21,
      21,    83,    21,    23,    21,    23,    21
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    59,    59,    59,    60,    60,    60,    60,
      61,    61,    62,    62,    62,    62,    62,    62,    62,    63,
      63,    63,    63,    63,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    65,    65,    65,    65,    66,    66,
      66,    66,    66,    66,    66,    66,    67,    67,    68,    68,
      68,    68,    68,    68,    68,    68,    69,    69,    70,    70,
      71,    71,    72,    72,    73,    74,    74,    75,    75,    76,
      76,    76,    77,    77,    77,    78,    78,    78,    79,    80,
      81,    82,    83,    83,    83,    83,    84,    84,    84,    84,
      84,    85,    85,    85,    85,    85,    85,    85,    86,    86,
      86
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
       1,     1,     1,     1,     1,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     3,     2,     1,     6,
       4,     1,     6,     4,     1,     6,     4,     1,     1,     1,
       1,     1,     5,     3,     3,     1,     2,     5,     3,     3,
       1,     3,     3,     3,     3,     2,     3,     1,     1,     1,
       1
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
#line 200 "ting.y"
    { *spec_handle = (yyvsp[0].specification); }
#line 1686 "ting_parser.c"
    break;

  case 3:
#line 204 "ting.y"
    { (yyval.specification) = (yyvsp[0].extformula); }
#line 1692 "ting_parser.c"
    break;

  case 4:
#line 205 "ting.y"
    { (yyval.specification) = (yyvsp[0].extdeclaration); }
#line 1698 "ting_parser.c"
    break;

  case 6:
#line 210 "ting.y"
    { (yyval.extformula) = create_operation(op_and, (yyvsp[-1].extformula), (yyvsp[0].extformula), "%s ; %s"); }
#line 1704 "ting_parser.c"
    break;

  case 7:
#line 211 "ting.y"
    { (yyval.extformula) = create_operation(op_join, (yyvsp[-1].extformula), (yyvsp[0].extdeclaration), "%s ; %s"); }
#line 1710 "ting_parser.c"
    break;

  case 8:
#line 212 "ting.y"
    { (yyval.extformula) = create_operation(op_join, (yyvsp[-1].extdeclaration), (yyvsp[0].extformula), "%s ; %s"); }
#line 1716 "ting_parser.c"
    break;

  case 9:
#line 213 "ting.y"
    { (yyval.extformula) = (yyvsp[0].formblock); }
#line 1722 "ting_parser.c"
    break;

  case 10:
#line 217 "ting.y"
    { (yyval.extdeclaration) = create_operation(op_join, (yyvsp[-1].extdeclaration), (yyvsp[0].extdeclaration), "%s ; %s"); }
#line 1728 "ting_parser.c"
    break;

  case 11:
#line 218 "ting.y"
    { (yyval.extdeclaration) = (yyvsp[0].declblock); }
#line 1734 "ting_parser.c"
    break;

  case 12:
#line 222 "ting.y"
    { (yyval.formblock) = create_operation(op_forall, (yyvsp[0].formblock), (yyvsp[-2].varrange), "iter(%2$s) { %1$s ; }"); }
#line 1740 "ting_parser.c"
    break;

  case 13:
#line 223 "ting.y"
    { (yyval.formblock) = create_operation(op_when, (yyvsp[0].formblock), (yyvsp[-2].selrange), "when(%2$s) { %1$s ; }"); }
#line 1746 "ting_parser.c"
    break;

  case 14:
#line 225 "ting.y"
    { (yyval.formblock) = create_operation(op_and, create_operation(op_when, (yyvsp[-2].formblock), (yyvsp[-4].selrange), "when(%2$s) { %1$s ; }"),
                                      create_operation(op_when, (yyvsp[0].formblock), create_operation(op_neg_range, copy_specification((yyvsp[-4].selrange)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 1753 "ting_parser.c"
    break;

  case 15:
#line 228 "ting.y"
    { (yyval.formblock) = create_operation(op_join, create_operation(op_when, (yyvsp[-2].formblock), (yyvsp[-4].selrange), "when(%2$s) { %1$s ; }"),
                                       create_operation(op_when, (yyvsp[0].declblock), create_operation(op_neg_range, copy_specification((yyvsp[-4].selrange)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 1760 "ting_parser.c"
    break;

  case 16:
#line 231 "ting.y"
    { (yyval.formblock) = create_operation(op_join, create_operation(op_when, (yyvsp[-2].declblock), (yyvsp[-4].selrange), "when(%2$s) { %1$s ; }"),
                                       create_operation(op_when, (yyvsp[0].formblock), create_operation(op_neg_range, copy_specification((yyvsp[-4].selrange)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 1767 "ting_parser.c"
    break;

  case 17:
#line 233 "ting.y"
    { (yyval.formblock) = (yyvsp[-1].extformula); }
#line 1773 "ting_parser.c"
    break;

  case 18:
#line 234 "ting.y"
    { (yyval.formblock) = (yyvsp[-1].formula); }
#line 1779 "ting_parser.c"
    break;

  case 19:
#line 238 "ting.y"
    { (yyval.declblock) = create_operation(op_iter, (yyvsp[0].declblock), (yyvsp[-2].varrange), "iter(%2$s) { %1$s ; }"); }
#line 1785 "ting_parser.c"
    break;

  case 20:
#line 239 "ting.y"
    { (yyval.declblock) = create_operation(op_when, (yyvsp[0].declblock), (yyvsp[-2].selrange), "when(%2$s) { %1$s ; }"); }
#line 1791 "ting_parser.c"
    break;

  case 21:
#line 241 "ting.y"
    { (yyval.declblock) = create_operation(op_join, create_operation(op_when, (yyvsp[-2].declblock), (yyvsp[-4].selrange), "when(%2$s) { %1$s ; }"),
                                       create_operation(op_when, (yyvsp[0].declblock), create_operation(op_neg_range, copy_specification((yyvsp[-4].selrange)), NULL, "not %s"), "when(%2$s) { %1$s ; }"), "%s ; %s"); }
#line 1798 "ting_parser.c"
    break;

  case 22:
#line 243 "ting.y"
    { (yyval.declblock) = (yyvsp[-1].extdeclaration); }
#line 1804 "ting_parser.c"
    break;

  case 23:
#line 244 "ting.y"
    { (yyval.declblock) = (yyvsp[-1].declaration); }
#line 1810 "ting_parser.c"
    break;

  case 24:
#line 248 "ting.y"
    { (yyval.formula) = create_operation(op_not, (yyvsp[0].formula), NULL, "(~ %s)"); }
#line 1816 "ting_parser.c"
    break;

  case 25:
#line 249 "ting.y"
    { (yyval.formula) = create_operation(op_and, (yyvsp[-2].formula), (yyvsp[0].formula), "(%s & %s)"); }
#line 1822 "ting_parser.c"
    break;

  case 26:
#line 250 "ting.y"
    { (yyval.formula) = create_operation(op_or, (yyvsp[-2].formula), (yyvsp[0].formula), "(%s | %s)"); }
#line 1828 "ting_parser.c"
    break;

  case 27:
#line 251 "ting.y"
    { (yyval.formula) = create_operation(op_delay, (yyvsp[-2].formula), (yyvsp[0].expression), "(%s @ %s)"); }
#line 1834 "ting_parser.c"
    break;

  case 28:
#line 252 "ting.y"
    { (yyval.formula) = create_operation(op_delay, (yyvsp[-2].formula), (yyvsp[0].expression), "(%s ? %s)"); }
#line 1840 "ting_parser.c"
    break;

  case 29:
#line 253 "ting.y"
    { (yyval.formula) = create_operation(op_at, (yyvsp[-2].formula), (yyvsp[0].interval), "(%s @ %s)"); }
#line 1846 "ting_parser.c"
    break;

  case 30:
#line 254 "ting.y"
    { (yyval.formula) = create_operation(op_happen, (yyvsp[-2].formula), (yyvsp[0].interval), "(%s ? %s)"); }
#line 1852 "ting_parser.c"
    break;

  case 31:
#line 255 "ting.y"
    { (yyval.formula) = create_operation(op_since, (yyvsp[-3].formula), (yyvsp[-1].formula), "since(%s , %s)"); }
#line 1858 "ting_parser.c"
    break;

  case 32:
#line 256 "ting.y"
    { (yyval.formula) = create_operation(op_until, (yyvsp[-3].formula), (yyvsp[-1].formula), "until(%s , %s)"); }
#line 1864 "ting_parser.c"
    break;

  case 33:
#line 257 "ting.y"
    { (yyval.formula) = create_operation(op_forall, (yyvsp[-3].formula), (yyvsp[-1].varrange), "forall(%s , %s)"); }
#line 1870 "ting_parser.c"
    break;

  case 34:
#line 258 "ting.y"
    { (yyval.formula) = create_operation(op_exists, (yyvsp[-3].formula), (yyvsp[-1].varrange), "exists(%s , %s)"); }
#line 1876 "ting_parser.c"
    break;

  case 35:
#line 259 "ting.y"
    { (yyval.formula) = create_operation(op_one, (yyvsp[-5].formula), create_operation(op_one_check, (yyvsp[-3].selrange), (yyvsp[-1].varrange), "%s , %s"), "one(%s , %s)"); }
#line 1882 "ting_parser.c"
    break;

  case 36:
#line 260 "ting.y"
    { (yyval.formula) = create_operation(op_unique, (yyvsp[-3].formula), (yyvsp[-1].varrange), "unique(%s , %s)"); }
#line 1888 "ting_parser.c"
    break;

  case 37:
#line 261 "ting.y"
    { (yyval.formula) = create_operation(op_imply, (yyvsp[-2].formula), (yyvsp[0].formula), "(%s --> %s)"); }
#line 1894 "ting_parser.c"
    break;

  case 38:
#line 262 "ting.y"
    { (yyval.formula) = create_operation(op_imply, (yyvsp[0].formula), (yyvsp[-2].formula), "(%2$s <-- %1$s)"); }
#line 1900 "ting_parser.c"
    break;

  case 39:
#line 263 "ting.y"
    { (yyval.formula) = create_operation(op_eqv, (yyvsp[-2].formula), (yyvsp[0].formula), "(%s == %s)"); }
#line 1906 "ting_parser.c"
    break;

  case 40:
#line 264 "ting.y"
    { (yyval.formula) = create_operation(op_code, (yyvsp[-3].varname), (yyvsp[-1].string), "code(%s , %s)"); }
#line 1912 "ting_parser.c"
    break;

  case 41:
#line 265 "ting.y"
    { (yyval.formula) = create_operation(op_code_num, (yyvsp[-3].varname), (yyvsp[-1].expression), "code(%s , %s)"); }
#line 1918 "ting_parser.c"
    break;

  case 42:
#line 266 "ting.y"
    { (yyval.formula) = (yyvsp[-1].formula); }
#line 1924 "ting_parser.c"
    break;

  case 43:
#line 267 "ting.y"
    { (yyval.formula) = (yyvsp[0].indexedname); }
#line 1930 "ting_parser.c"
    break;

  case 44:
#line 271 "ting.y"
    { (yyval.interval) = create_operation(op_interval_1, (yyvsp[-3].expression), (yyvsp[-1].expression), "[%s , %s]"); }
#line 1936 "ting_parser.c"
    break;

  case 45:
#line 272 "ting.y"
    { (yyval.interval) = create_operation(op_interval_2, (yyvsp[-3].expression), (yyvsp[-1].expression), "(%s , %s]"); }
#line 1942 "ting_parser.c"
    break;

  case 46:
#line 273 "ting.y"
    { (yyval.interval) = create_operation(op_interval_3, (yyvsp[-3].expression), (yyvsp[-1].expression), "[%s , %s)"); }
#line 1948 "ting_parser.c"
    break;

  case 47:
#line 274 "ting.y"
    { (yyval.interval) = create_operation(op_interval_4, (yyvsp[-3].expression), (yyvsp[-1].expression), "(%s , %s)"); }
#line 1954 "ting_parser.c"
    break;

  case 48:
#line 278 "ting.y"
    { (yyval.declaration) = create_operation(op_input, (yyvsp[0].decllist), (yyvsp[-2].io_decllist), "input[%2$s] %1$s"); }
#line 1960 "ting_parser.c"
    break;

  case 49:
#line 279 "ting.y"
    { (yyval.declaration) = create_operation(op_input, (yyvsp[0].decllist), create_operation(op_join_qual, create_operation(op_join_qual,
      create_ground(op_ioqual1, "", io_any), create_ground(op_ioqual2, "", io_binary), "%s , %s"), create_ground(op_ioqual3, "", io_unknown), "%s , %s"), "input[%2$s] %1$s"); }
#line 1967 "ting_parser.c"
    break;

  case 50:
#line 281 "ting.y"
    { (yyval.declaration) = create_operation(op_output, (yyvsp[0].decllist), (yyvsp[-2].io_decllist), "output[%2$s] %1$s"); }
#line 1973 "ting_parser.c"
    break;

  case 51:
#line 282 "ting.y"
    { (yyval.declaration) = create_operation(op_output, (yyvsp[0].decllist), create_operation(op_join_qual, create_operation(op_join_qual,
      create_ground(op_ioqual1, "", io_any), create_ground(op_ioqual2, "", io_binary), "%s , %s"), create_ground(op_ioqual3, "", io_unknown), "%s , %s"), "output[%2$s] %1$s"); }
#line 1980 "ting_parser.c"
    break;

  case 52:
#line 284 "ting.y"
    { (yyval.declaration) = create_operation(op_aux, (yyvsp[0].decllist), NULL, "aux %s"); }
#line 1986 "ting_parser.c"
    break;

  case 53:
#line 285 "ting.y"
    { (yyval.declaration) = create_operation(op_init, (yyvsp[0].eventlist), NULL, "init %s"); }
#line 1992 "ting_parser.c"
    break;

  case 54:
#line 286 "ting.y"
    { (yyval.declaration) = (yyvsp[0].assignlist); }
#line 1998 "ting_parser.c"
    break;

  case 55:
#line 287 "ting.y"
    { (yyval.declaration) = (yyvsp[0].filelist); }
#line 2004 "ting_parser.c"
    break;

  case 56:
#line 291 "ting.y"
    { (yyval.io_decllist) = create_operation(op_join_qual, (yyvsp[-2].io_decllist), (yyvsp[0].io_decl), "%s , %s"); }
#line 2010 "ting_parser.c"
    break;

  case 57:
#line 292 "ting.y"
    { (yyval.io_decllist) = (yyvsp[0].io_decl); }
#line 2016 "ting_parser.c"
    break;

  case 58:
#line 296 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual1, "any", io_any); }
#line 2022 "ting_parser.c"
    break;

  case 59:
#line 297 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual1, "ipc", io_ipc); }
#line 2028 "ting_parser.c"
    break;

  case 60:
#line 298 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual1, "file", io_file); }
#line 2034 "ting_parser.c"
    break;

  case 61:
#line 299 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual2, "binary", io_binary); }
#line 2040 "ting_parser.c"
    break;

  case 62:
#line 300 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual2, "packed", io_packed); }
#line 2046 "ting_parser.c"
    break;

  case 63:
#line 301 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual3, "unknown", io_unknown); }
#line 2052 "ting_parser.c"
    break;

  case 64:
#line 302 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual3, "false", io_false); }
#line 2058 "ting_parser.c"
    break;

  case 65:
#line 303 "ting.y"
    { (yyval.io_decl) = create_ground(op_ioqual3, "true", io_true); }
#line 2064 "ting_parser.c"
    break;

  case 66:
#line 307 "ting.y"
    { (yyval.decllist) = create_operation(op_join, (yyvsp[-2].decllist), (yyvsp[0].declname), "%s , %s"); }
#line 2070 "ting_parser.c"
    break;

  case 67:
#line 308 "ting.y"
    { (yyval.decllist) = (yyvsp[0].declname); }
#line 2076 "ting_parser.c"
    break;

  case 68:
#line 312 "ting.y"
    { (yyval.eventlist) = create_operation(op_join, (yyvsp[-2].eventlist), (yyvsp[0].eventname), "%s , %s"); }
#line 2082 "ting_parser.c"
    break;

  case 69:
#line 313 "ting.y"
    { (yyval.eventlist) = (yyvsp[0].eventname); }
#line 2088 "ting_parser.c"
    break;

  case 70:
#line 317 "ting.y"
    { (yyval.filelist) = create_operation(op_join, (yyvsp[-2].filelist), (yyvsp[0].filename), "%s , %s"); }
#line 2094 "ting_parser.c"
    break;

  case 71:
#line 318 "ting.y"
    { (yyval.filelist) = (yyvsp[0].filename); }
#line 2100 "ting_parser.c"
    break;

  case 72:
#line 322 "ting.y"
    { (yyval.assignlist) = create_operation(op_join, (yyvsp[-2].assignlist), (yyvsp[0].assignment), "%s ; %s"); }
#line 2106 "ting_parser.c"
    break;

  case 73:
#line 323 "ting.y"
    { (yyval.assignlist) = (yyvsp[0].assignment); }
#line 2112 "ting_parser.c"
    break;

  case 74:
#line 327 "ting.y"
    { (yyval.assignment) = create_operation(op_define, (yyvsp[-2].constname), (yyvsp[0].expression), "define %s = %s"); }
#line 2118 "ting_parser.c"
    break;

  case 75:
#line 331 "ting.y"
    { (yyval.eventname) = create_operation(op_var_at, (yyvsp[-2].qualname), (yyvsp[0].expression), "%s @ %s"); }
#line 2124 "ting_parser.c"
    break;

  case 76:
#line 332 "ting.y"
    { (yyval.eventname) = create_operation(op_var_at, (yyvsp[-2].qualname), (yyvsp[0].interval), "%s @ %s"); }
#line 2130 "ting_parser.c"
    break;

  case 77:
#line 336 "ting.y"
    { (yyval.qualname) = create_operation(op_not, (yyvsp[0].initname), NULL, "~ %s"); }
#line 2136 "ting_parser.c"
    break;

  case 78:
#line 337 "ting.y"
    { (yyval.qualname) = (yyvsp[0].initname); }
#line 2142 "ting_parser.c"
    break;

  case 79:
#line 341 "ting.y"
    { (yyval.indexedname) = create_operation(op_matrix, create_ground(op_name, (yyvsp[-5].symbol), 0),
      create_operation(op_interval_1, (yyvsp[-3].expression), (yyvsp[-1].expression), "%s , %s"), "%s(%s)"); }
#line 2149 "ting_parser.c"
    break;

  case 80:
#line 343 "ting.y"
    { (yyval.indexedname) = create_operation(op_vector, create_ground(op_name, (yyvsp[-3].symbol), 0), (yyvsp[-1].expression), "%s(%s)"); }
#line 2155 "ting_parser.c"
    break;

  case 81:
#line 344 "ting.y"
    { (yyval.indexedname) = (yyvsp[0].varname); }
#line 2161 "ting_parser.c"
    break;

  case 82:
#line 348 "ting.y"
    { (yyval.declname) = create_operation(op_matrix, create_ground(op_dname, (yyvsp[-5].symbol), 0),
      create_operation(op_interval_1, (yyvsp[-3].expression), (yyvsp[-1].expression), "%s , %s"), "%s(%s)"); }
#line 2168 "ting_parser.c"
    break;

  case 83:
#line 350 "ting.y"
    { (yyval.declname) = create_operation(op_vector, create_ground(op_dname, (yyvsp[-3].symbol), 0), (yyvsp[-1].expression), "%s(%s)"); }
#line 2174 "ting_parser.c"
    break;

  case 84:
#line 351 "ting.y"
    { (yyval.declname) = create_ground(op_dname, (yyvsp[0].symbol), 0); }
#line 2180 "ting_parser.c"
    break;

  case 85:
#line 355 "ting.y"
    { (yyval.initname) = create_operation(op_matrix, create_ground(op_iname, (yyvsp[-5].symbol), 0),
      create_operation(op_interval_1, (yyvsp[-3].expression), (yyvsp[-1].expression), "%s , %s"), "%s(%s)"); }
#line 2187 "ting_parser.c"
    break;

  case 86:
#line 357 "ting.y"
    { (yyval.initname) = create_operation(op_vector, create_ground(op_iname, (yyvsp[-3].symbol), 0), (yyvsp[-1].expression), "%s(%s)"); }
#line 2193 "ting_parser.c"
    break;

  case 87:
#line 358 "ting.y"
    { (yyval.initname) = create_ground(op_iname, (yyvsp[0].symbol), 0); }
#line 2199 "ting_parser.c"
    break;

  case 88:
#line 362 "ting.y"
    { (yyval.varname) = create_ground(op_name, (yyvsp[0].symbol), 0); }
#line 2205 "ting_parser.c"
    break;

  case 89:
#line 366 "ting.y"
    { (yyval.constname) = create_ground(op_cname, (yyvsp[0].symbol), 0); }
#line 2211 "ting_parser.c"
    break;

  case 90:
#line 370 "ting.y"
    { (yyval.filename) = create_ground(op_fname, (yyvsp[0].symbol), 0); }
#line 2217 "ting_parser.c"
    break;

  case 91:
#line 374 "ting.y"
    { (yyval.string) = create_ground(op_string, (yyvsp[0].symbol), 0); }
#line 2223 "ting_parser.c"
    break;

  case 92:
#line 378 "ting.y"
    { (yyval.varrange) = create_operation(op_in, (yyvsp[-4].constname), create_operation(op_interval_1, (yyvsp[-2].expression), (yyvsp[0].expression), "%s : %s"), "%s in %s"); }
#line 2229 "ting_parser.c"
    break;

  case 93:
#line 379 "ting.y"
    { (yyval.varrange) = create_operation(op_in, (yyvsp[-2].constname), create_operation(op_interval_3, create_ground(op_number, "", 0), (yyvsp[0].expression), "%s : %s - 1"), "%s on %s"); }
#line 2235 "ting_parser.c"
    break;

  case 94:
#line 380 "ting.y"
    { (yyval.varrange) = create_operation(op_interval_1, (yyvsp[-2].expression), (yyvsp[0].expression), "%s : %s"); }
#line 2241 "ting_parser.c"
    break;

  case 95:
#line 381 "ting.y"
    { (yyval.varrange) = create_operation(op_interval_3, create_ground(op_number, "", 0), (yyvsp[0].expression), "%s : %s - 1"); }
#line 2247 "ting_parser.c"
    break;

  case 96:
#line 385 "ting.y"
    { (yyval.selrange) = create_operation(op_neg_range, (yyvsp[0].selrange), NULL, "not %s"); }
#line 2253 "ting_parser.c"
    break;

  case 97:
#line 386 "ting.y"
    { (yyval.selrange) = create_operation(op_in, (yyvsp[-4].expression), create_operation(op_interval_1, (yyvsp[-2].expression), (yyvsp[0].expression), "%s : %s"), "%s in %s"); }
#line 2259 "ting_parser.c"
    break;

  case 98:
#line 387 "ting.y"
    { (yyval.selrange) = create_operation(op_in, (yyvsp[-2].expression), create_operation(op_interval_1, (yyvsp[0].expression), copy_specification((yyvsp[0].expression)), "%s : %s"), "%s is %s"); }
#line 2265 "ting_parser.c"
    break;

  case 99:
#line 388 "ting.y"
    { (yyval.selrange) = create_operation(op_interval_1, (yyvsp[-2].expression), (yyvsp[0].expression), "%s : %s"); }
#line 2271 "ting_parser.c"
    break;

  case 100:
#line 389 "ting.y"
    { (yyval.selrange) = create_operation(op_interval_1, (yyvsp[0].expression), copy_specification((yyvsp[0].expression)), "%s : %s"); }
#line 2277 "ting_parser.c"
    break;

  case 101:
#line 393 "ting.y"
    { (yyval.expression) = create_operation(op_plus, (yyvsp[-2].expression), (yyvsp[0].expression), "%s + %s"); }
#line 2283 "ting_parser.c"
    break;

  case 102:
#line 394 "ting.y"
    { (yyval.expression) = create_operation(op_minus, (yyvsp[-2].expression), (yyvsp[0].expression), "%s - %s"); }
#line 2289 "ting_parser.c"
    break;

  case 103:
#line 395 "ting.y"
    { (yyval.expression) = create_operation(op_mul, (yyvsp[-2].expression), (yyvsp[0].expression), "%s * %s"); }
#line 2295 "ting_parser.c"
    break;

  case 104:
#line 396 "ting.y"
    { (yyval.expression) = create_operation(op_div, (yyvsp[-2].expression), (yyvsp[0].expression), "%s / %s"); }
#line 2301 "ting_parser.c"
    break;

  case 105:
#line 397 "ting.y"
    { (yyval.expression) = create_operation(op_chs, (yyvsp[0].expression), NULL, "- %s"); }
#line 2307 "ting_parser.c"
    break;

  case 106:
#line 398 "ting.y"
    { (yyval.expression) = (yyvsp[-1].expression); }
#line 2313 "ting_parser.c"
    break;

  case 107:
#line 399 "ting.y"
    { (yyval.expression) = (yyvsp[0].number); }
#line 2319 "ting_parser.c"
    break;

  case 108:
#line 403 "ting.y"
    { (yyval.number) = create_ground(op_number, "", (yyvsp[0].value)); }
#line 2325 "ting_parser.c"
    break;

  case 109:
#line 404 "ting.y"
    { (yyval.number) = create_ground(op_iterator, (yyvsp[0].symbol), 0); }
#line 2331 "ting_parser.c"
    break;

  case 110:
#line 405 "ting.y"
    { (yyval.number) = create_ground(op_constant, (yyvsp[0].symbol), 0); }
#line 2337 "ting_parser.c"
    break;


#line 2341 "ting_parser.c"

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
#line 408 "ting.y"


