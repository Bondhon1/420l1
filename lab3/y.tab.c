/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "22101459_22101458.y"


#include "symbol_table.h"

#define YYSTYPE symbol_info*

int yyparse(void);
int yylex(void);

extern FILE *yyin;

#include<bits/stdc++.h>
using namespace std;

ofstream outlog;
ofstream errlog;

symbol_table *symtab = nullptr;

int lines = 1;
int error_count = 0;

struct VarDecl {
    string name;
    bool is_array;
    int array_size;
};

struct ParamInfo {
    string name;
    string type;
};

vector<VarDecl> collected_vars;
vector<ParamInfo> pending_params;
unordered_set<string> pending_param_names_seen;
string pending_param_details;
int pending_param_count = 0;
string pending_func_name;
string pending_func_return_type;
bool pending_func_duplicate = false;

void report_error(const string &message)
{
    errlog << "At line no: " << lines << " " << message << endl << endl;
    error_count++;
}

void yyerror(char *s)
{
    report_error(s);
}

bool is_int_type(const string &type)
{
    return type == "int";
}

bool is_float_type(const string &type)
{
    return type == "float";
}

bool is_void_type(const string &type)
{
    return type == "void";
}

bool is_error_type(const string &type)
{
    return type.empty() || type == "error";
}

symbol_info *make_node(const string &name, const string &grammar_type, const string &data_type = "")
{
    symbol_info *node = new symbol_info(name, grammar_type);
    node->set_data_type(data_type);
    return node;
}

string node_type(symbol_info *node)
{
    if (node == nullptr) return "error";
    string type = node->get_data_type();
    if (type.empty()) return "error";
    return type;
}

void declare_variable(const string &name, const string &type, bool is_array = false, int array_size = 0, bool is_parameter = false)
{
    if (type == "void") {
        report_error("variable type can not be void ");
        return;
    }

    symbol_info *symbol = new symbol_info(name, "ID");
    symbol->set_symbol_kind(is_array ? "ARRAY" : "VARIABLE");
    symbol->set_data_type(type);
    if (is_array) symbol->set_array_size(array_size);

    if (!symtab->insert(symbol)) {
        if (is_parameter) {
            report_error("Multiple declaration of variable " + name + " in parameter of " + pending_func_name);
        } else {
            report_error("Multiple declaration of variable " + name);
        }
        delete symbol;
    }
}

void declare_function(const string &name, const string &return_type, const vector<string> &param_types, const string &param_details)
{
    symbol_info *symbol = new symbol_info(name, "ID");
    symbol->set_symbol_kind("FUNCTION");
    symbol->set_data_type(return_type);
    symbol->set_param_count(static_cast<int>(param_types.size()));
    symbol->set_param_details(param_details);
    symbol->set_param_types(param_types);

    if (!symtab->insert(symbol)) {
        report_error("Multiple declaration of function " + name);
        delete symbol;
    }
}

void insert_pending_parameters()
{
    unordered_set<string> inserted_names;
    for (const auto &param : pending_params) {
        if (param.name.empty()) continue;
        if (inserted_names.count(param.name)) continue;
        inserted_names.insert(param.name);
        declare_variable(param.name, param.type, false, 0, true);
    }
}

vector<string> get_pending_param_types()
{
    vector<string> types;
    for (const auto &param : pending_params) {
        types.push_back(param.type);
    }
    return types;
}

void check_function_call(symbol_info *function_symbol, symbol_info *arguments, const string &function_name)
{
    const vector<string> &formal_types = function_symbol->get_param_types();
    const vector<string> &actual_types = arguments->get_arg_types();

    if (formal_types.size() != actual_types.size()) {
        report_error("Inconsistencies in number of arguments in function call: " + function_name);
    }

    size_t matched = min(formal_types.size(), actual_types.size());
    for (size_t index = 0; index < matched; ++index) {
        if (actual_types[index] == "error") continue;
        if (formal_types[index] != actual_types[index]) {
            report_error("argument " + to_string(index + 1) + " type mismatch in function call: " + function_name);
        }
    }
}

void check_division_or_modulus_zero(const string &operation, symbol_info *right_node)
{
    if (right_node == nullptr) return;
    const string right_name = right_node->getname();
    if (operation == "/") {
        if (right_name == "0" || right_name == "0.0") {
            report_error("Division by 0 ");
        }
    } else if (operation == "%") {
        if (right_name == "0") {
            report_error("Modulus by 0 ");
        }
    }
}


/* Line 371 of yacc.c  */
#line 248 "y.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     CONST_INT = 259,
     CONST_FLOAT = 260,
     INT = 261,
     FLOAT = 262,
     VOID = 263,
     IF = 264,
     FOR = 265,
     WHILE = 266,
     DO = 267,
     RETURN = 268,
     PRINTLN = 269,
     CHAR = 270,
     DOUBLE = 271,
     BREAK = 272,
     SWITCH = 273,
     CASE = 274,
     DEFAULT = 275,
     CONTINUE = 276,
     GOTO = 277,
     ADDOP = 278,
     MULOP = 279,
     INCOP = 280,
     DECOP = 281,
     RELOP = 282,
     ASSIGNOP = 283,
     LOGICOP = 284,
     NOT = 285,
     LPAREN = 286,
     RPAREN = 287,
     LCURL = 288,
     RCURL = 289,
     LTHIRD = 290,
     RTHIRD = 291,
     COMMA = 292,
     COLON = 293,
     SEMICOLON = 294,
     LOWER_THAN_ELSE = 295,
     ELSE = 296
   };
#endif
/* Tokens.  */
#define ID 258
#define CONST_INT 259
#define CONST_FLOAT 260
#define INT 261
#define FLOAT 262
#define VOID 263
#define IF 264
#define FOR 265
#define WHILE 266
#define DO 267
#define RETURN 268
#define PRINTLN 269
#define CHAR 270
#define DOUBLE 271
#define BREAK 272
#define SWITCH 273
#define CASE 274
#define DEFAULT 275
#define CONTINUE 276
#define GOTO 277
#define ADDOP 278
#define MULOP 279
#define INCOP 280
#define DECOP 281
#define RELOP 282
#define ASSIGNOP 283
#define LOGICOP 284
#define NOT 285
#define LPAREN 286
#define RPAREN 287
#define LCURL 288
#define RCURL 289
#define LTHIRD 290
#define RTHIRD 291
#define COMMA 292
#define COLON 293
#define SEMICOLON 294
#define LOWER_THAN_ELSE 295
#define ELSE 296



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 396 "y.tab.c"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
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

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

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
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   143

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  65
/* YYNRULES -- Number of states.  */
#define YYNSTATES  119

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   296

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
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
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    15,    23,
      24,    31,    36,    40,    43,    45,    46,    51,    52,    56,
      60,    62,    64,    66,    70,    77,    79,    84,    86,    89,
      91,    93,    95,   103,   109,   117,   123,   129,   133,   135,
     138,   140,   145,   147,   151,   153,   157,   159,   163,   165,
     169,   171,   175,   178,   181,   183,   185,   190,   194,   196,
     198,   201,   204,   206,   207,   211
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      43,     0,    -1,    44,    -1,    44,    45,    -1,    45,    -1,
      53,    -1,    46,    -1,    -1,    54,     3,    31,    49,    32,
      47,    50,    -1,    -1,    54,     3,    31,    32,    48,    50,
      -1,    49,    37,    54,     3,    -1,    49,    37,    54,    -1,
      54,     3,    -1,    54,    -1,    -1,    33,    51,    56,    34,
      -1,    -1,    33,    52,    34,    -1,    54,    55,    39,    -1,
       6,    -1,     7,    -1,     8,    -1,    55,    37,     3,    -1,
      55,    37,     3,    35,     4,    36,    -1,     3,    -1,     3,
      35,     4,    36,    -1,    57,    -1,    56,    57,    -1,    53,
      -1,    58,    -1,    50,    -1,    10,    31,    58,    58,    60,
      32,    57,    -1,     9,    31,    60,    32,    57,    -1,     9,
      31,    60,    32,    57,    41,    57,    -1,    11,    31,    60,
      32,    57,    -1,    14,    31,     3,    32,    39,    -1,    13,
      60,    39,    -1,    39,    -1,    60,    39,    -1,     3,    -1,
       3,    35,    60,    36,    -1,    61,    -1,    59,    28,    61,
      -1,    62,    -1,    62,    29,    62,    -1,    63,    -1,    63,
      27,    63,    -1,    64,    -1,    63,    23,    64,    -1,    65,
      -1,    64,    24,    65,    -1,    23,    65,    -1,    30,    65,
      -1,    66,    -1,    59,    -1,     3,    31,    67,    32,    -1,
      31,    60,    32,    -1,     4,    -1,     5,    -1,    59,    25,
      -1,    59,    26,    -1,    68,    -1,    -1,    68,    37,    61,
      -1,    61,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   197,   197,   205,   211,   219,   225,   234,   233,   257,
     256,   281,   295,   305,   317,   330,   329,   342,   341,   355,
     371,   377,   383,   391,   398,   405,   413,   423,   429,   437,
     443,   449,   455,   461,   467,   473,   479,   491,   499,   505,
     513,   531,   551,   557,   572,   578,   589,   595,   606,   612,
     629,   635,   663,   675,   686,   694,   700,   714,   720,   726,
     732,   738,   746,   754,   760,   769
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "CONST_INT", "CONST_FLOAT", "INT",
  "FLOAT", "VOID", "IF", "FOR", "WHILE", "DO", "RETURN", "PRINTLN", "CHAR",
  "DOUBLE", "BREAK", "SWITCH", "CASE", "DEFAULT", "CONTINUE", "GOTO",
  "ADDOP", "MULOP", "INCOP", "DECOP", "RELOP", "ASSIGNOP", "LOGICOP",
  "NOT", "LPAREN", "RPAREN", "LCURL", "RCURL", "LTHIRD", "RTHIRD", "COMMA",
  "COLON", "SEMICOLON", "LOWER_THAN_ELSE", "ELSE", "$accept", "start",
  "program", "unit", "func_definition", "$@1", "$@2", "parameter_list",
  "compound_statement", "$@3", "$@4", "var_declaration", "type_specifier",
  "declaration_list", "statements", "statement", "expression_statement",
  "variable", "expression", "logic_expression", "rel_expression",
  "simple_expression", "term", "unary_expression", "factor",
  "argument_list", "arguments", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    42,    43,    44,    44,    45,    45,    47,    46,    48,
      46,    49,    49,    49,    49,    51,    50,    52,    50,    53,
      54,    54,    54,    55,    55,    55,    55,    56,    56,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    58,    58,
      59,    59,    60,    60,    61,    61,    62,    62,    63,    63,
      64,    64,    65,    65,    65,    66,    66,    66,    66,    66,
      66,    66,    67,    67,    68,    68
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     0,     7,     0,
       6,     4,     3,     2,     1,     0,     4,     0,     3,     3,
       1,     1,     1,     3,     6,     1,     4,     1,     2,     1,
       1,     1,     7,     5,     7,     5,     5,     3,     1,     2,
       1,     4,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     2,     2,     1,     1,     4,     3,     1,     1,
       2,     2,     1,     0,     3,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    20,    21,    22,     0,     2,     4,     6,     5,     0,
       1,     3,    25,     0,     0,     0,     0,    19,     9,     0,
      14,     0,    23,     0,     7,     0,    13,    26,     0,    15,
      10,     0,    12,     0,     0,     0,     8,    11,    24,    40,
      58,    59,     0,     0,     0,     0,     0,     0,     0,     0,
      38,    31,    29,     0,     0,    27,    30,    55,     0,    42,
      44,    46,    48,    50,    54,    18,    63,     0,     0,     0,
       0,     0,     0,    55,    52,    53,     0,    25,    16,    28,
      60,    61,     0,    39,     0,     0,     0,     0,    65,     0,
      62,     0,     0,     0,     0,    37,     0,    57,    43,    45,
      49,    47,    51,    56,     0,    41,     0,     0,     0,     0,
      64,    33,     0,    35,    36,     0,     0,    34,    32
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     4,     5,     6,     7,    31,    23,    19,    51,    34,
      35,    52,    53,    13,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    89,    90
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -66
static const yytype_int8 yypact[] =
{
      12,   -66,   -66,   -66,     3,    12,   -66,   -66,   -66,    49,
     -66,   -66,    -3,    14,    -1,    56,    72,   -66,   -66,   -11,
      77,     1,    47,    50,   -66,    12,   -66,   -66,    80,    51,
     -66,    50,    84,    52,   100,    55,   -66,   -66,   -66,     7,
     -66,   -66,    59,    60,    61,    95,    64,    95,    95,    95,
     -66,   -66,   -66,    98,    63,   -66,   -66,    53,    73,   -66,
      86,    34,    92,   -66,   -66,   -66,    95,    95,    95,    10,
      95,    78,   116,    33,   -66,   -66,    88,    87,   -66,   -66,
     -66,   -66,    95,   -66,    95,    95,    95,    95,   -66,    89,
      90,    93,    96,    10,   102,   -66,   103,   -66,   -66,   -66,
      92,   101,   -66,   -66,    95,   -66,   100,    95,   100,    97,
     -66,    91,   105,   -66,   -66,   100,   100,   -66,   -66
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -66,   -66,   -66,   133,   -66,   -66,   -66,   -66,    -7,   -66,
     -66,    29,    30,   -66,   -66,   -52,   -57,   -39,   -45,   -65,
      57,    54,    58,   -37,   -66,   -66,   -66
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -18
static const yytype_int8 yytable[] =
{
      71,    88,    79,    10,    76,     1,     2,     3,    73,    73,
      74,    75,    93,    39,    40,    41,    30,    98,     1,     2,
       3,    24,    91,    92,    36,    94,    25,    73,    14,     8,
       9,    18,    15,    47,     8,     9,   107,    27,    66,   110,
      48,    49,    67,    73,    20,    73,    73,    73,    73,    50,
     102,    16,    12,    17,   111,    32,   113,    85,    80,    81,
      21,    86,   112,   117,   118,    73,    39,    40,    41,     1,
       2,     3,    42,    43,    44,    22,    45,    46,    80,    81,
      26,    82,    28,    29,    33,   -17,    47,    37,    38,    65,
      68,    69,    70,    48,    49,    72,    29,    78,    39,    40,
      41,    77,    50,    39,    40,    41,     1,     2,     3,    42,
      43,    44,    83,    45,    46,    84,    87,    95,    47,    96,
      97,   103,    15,    47,    85,    48,    49,   104,   106,   105,
      48,    49,   115,    29,   108,   109,   114,   116,    11,    50,
     101,    99,     0,   100
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-66)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
      45,    66,    54,     0,    49,     6,     7,     8,    47,    48,
      47,    48,    69,     3,     4,     5,    23,    82,     6,     7,
       8,    32,    67,    68,    31,    70,    37,    66,    31,     0,
       0,    32,    35,    23,     5,     5,    93,    36,    31,   104,
      30,    31,    35,    82,    14,    84,    85,    86,    87,    39,
      87,    37,     3,    39,   106,    25,   108,    23,    25,    26,
       4,    27,   107,   115,   116,   104,     3,     4,     5,     6,
       7,     8,     9,    10,    11,     3,    13,    14,    25,    26,
       3,    28,    35,    33,     4,    34,    23,     3,    36,    34,
      31,    31,    31,    30,    31,    31,    33,    34,     3,     4,
       5,     3,    39,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    39,    13,    14,    29,    24,    39,    23,     3,
      32,    32,    35,    23,    23,    30,    31,    37,    32,    36,
      30,    31,    41,    33,    32,    32,    39,    32,     5,    39,
      86,    84,    -1,    85
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     6,     7,     8,    43,    44,    45,    46,    53,    54,
       0,    45,     3,    55,    31,    35,    37,    39,    32,    49,
      54,     4,     3,    48,    32,    37,     3,    36,    35,    33,
      50,    47,    54,     4,    51,    52,    50,     3,    36,     3,
       4,     5,     9,    10,    11,    13,    14,    23,    30,    31,
      39,    50,    53,    54,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    34,    31,    35,    31,    31,
      31,    60,    31,    59,    65,    65,    60,     3,    34,    57,
      25,    26,    28,    39,    29,    23,    27,    24,    61,    67,
      68,    60,    60,    58,    60,    39,     3,    32,    61,    62,
      64,    63,    65,    32,    37,    36,    32,    58,    32,    32,
      61,    57,    60,    57,    39,    41,    32,    57,    57
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
	    /* Fall through.  */
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

  return yystpcpy (yyres, yystr) - yyres;
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
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
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
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

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

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

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
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
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
      yychar = YYLEX;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
/* Line 1792 of yacc.c  */
#line 198 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" start : program "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "start", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 206 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" program : program unit "<<endl<<endl;
        outlog<<(yyvsp[(1) - (2)])->getname()<<"\n"<<(yyvsp[(2) - (2)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (2)])->getname()+"\n"+(yyvsp[(2) - (2)])->getname(), "program");
    }
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 212 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" program : unit "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "program");
    }
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 220 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" unit : var_declaration "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "unit", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 226 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" unit : func_definition "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "unit");
    }
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 234 "22101459_22101458.y"
    {
        pending_func_name = (yyvsp[(2) - (5)])->getname();
        pending_func_return_type = (yyvsp[(1) - (5)])->getname();
        pending_func_duplicate = (symtab->lookup(pending_func_name) != nullptr);
        if (pending_func_duplicate) {
            report_error("Multiple declaration of function " + pending_func_name);
        }
    }
    break;

  case 8:
/* Line 1792 of yacc.c  */
#line 243 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement "<<endl<<endl;
        outlog<<(yyvsp[(1) - (7)])->getname()<<" "<<(yyvsp[(2) - (7)])->getname()<<"("<<(yyvsp[(4) - (7)])->getname()<<")\n"<<(yyvsp[(7) - (7)])->getname()<<endl<<endl;
        if (!pending_func_duplicate) {
            declare_function(pending_func_name, pending_func_return_type, get_pending_param_types(), pending_param_details);
        }
        pending_params.clear();
        pending_param_names_seen.clear();
        pending_param_details = "";
        pending_param_count = 0;
        pending_func_duplicate = false;
        (yyval) = make_node((yyvsp[(1) - (7)])->getname()+" "+(yyvsp[(2) - (7)])->getname()+"("+(yyvsp[(4) - (7)])->getname()+")\n"+(yyvsp[(7) - (7)])->getname(), "func_def");
    }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 257 "22101459_22101458.y"
    {
        pending_func_name = (yyvsp[(2) - (4)])->getname();
        pending_func_return_type = (yyvsp[(1) - (4)])->getname();
        pending_params.clear();
        pending_param_names_seen.clear();
        pending_param_details = "";
        pending_param_count = 0;
        pending_func_duplicate = (symtab->lookup(pending_func_name) != nullptr);
        if (pending_func_duplicate) {
            report_error("Multiple declaration of function " + pending_func_name);
        }
    }
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 270 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" func_definition : type_specifier ID LPAREN RPAREN compound_statement "<<endl<<endl;
        outlog<<(yyvsp[(1) - (6)])->getname()<<" "<<(yyvsp[(2) - (6)])->getname()<<"()\n"<<(yyvsp[(6) - (6)])->getname()<<endl<<endl;
        if (!pending_func_duplicate) {
            declare_function(pending_func_name, pending_func_return_type, vector<string>(), "");
        }
        pending_func_duplicate = false;
        (yyval) = make_node((yyvsp[(1) - (6)])->getname()+" "+(yyvsp[(2) - (6)])->getname()+"()\n"+(yyvsp[(6) - (6)])->getname(), "func_def");
    }
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 282 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" parameter_list : parameter_list COMMA type_specifier ID "<<endl<<endl;
        outlog<<(yyvsp[(1) - (4)])->getname()<<","<<(yyvsp[(3) - (4)])->getname()<<" "<<(yyvsp[(4) - (4)])->getname()<<endl<<endl;
        if (pending_param_names_seen.count((yyvsp[(4) - (4)])->getname())) {
            report_error("Multiple declaration of variable " + (yyvsp[(4) - (4)])->getname() + " in parameter of " + pending_func_name);
        }
        pending_param_names_seen.insert((yyvsp[(4) - (4)])->getname());
        pending_params.push_back({(yyvsp[(4) - (4)])->getname(), (yyvsp[(3) - (4)])->getname()});
        if (!pending_param_details.empty()) pending_param_details += ", ";
        pending_param_details += (yyvsp[(3) - (4)])->getname()+" "+(yyvsp[(4) - (4)])->getname();
        pending_param_count++;
        (yyval) = make_node((yyvsp[(1) - (4)])->getname()+","+(yyvsp[(3) - (4)])->getname()+" "+(yyvsp[(4) - (4)])->getname(), "param_list");
    }
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 296 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" parameter_list : parameter_list COMMA type_specifier "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<","<<(yyvsp[(3) - (3)])->getname()<<endl<<endl;
        pending_params.push_back({"", (yyvsp[(3) - (3)])->getname()});
        if (!pending_param_details.empty()) pending_param_details += ", ";
        pending_param_details += (yyvsp[(3) - (3)])->getname();
        pending_param_count++;
        (yyval) = make_node((yyvsp[(1) - (3)])->getname()+","+(yyvsp[(3) - (3)])->getname(), "param_list");
    }
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 306 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" parameter_list : type_specifier ID "<<endl<<endl;
        outlog<<(yyvsp[(1) - (2)])->getname()<<" "<<(yyvsp[(2) - (2)])->getname()<<endl<<endl;
        pending_params.clear();
        pending_param_names_seen.clear();
        pending_param_names_seen.insert((yyvsp[(2) - (2)])->getname());
        pending_params.push_back({(yyvsp[(2) - (2)])->getname(), (yyvsp[(1) - (2)])->getname()});
        pending_param_details = (yyvsp[(1) - (2)])->getname()+" "+(yyvsp[(2) - (2)])->getname();
        pending_param_count = 1;
        (yyval) = make_node((yyvsp[(1) - (2)])->getname()+" "+(yyvsp[(2) - (2)])->getname(), "param_list");
    }
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 318 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" parameter_list : type_specifier "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        pending_params.clear();
        pending_params.push_back({"", (yyvsp[(1) - (1)])->getname()});
        pending_param_details = (yyvsp[(1) - (1)])->getname();
        pending_param_count = 1;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "param_list");
    }
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 330 "22101459_22101458.y"
    {
        symtab->enter_scope();
        insert_pending_parameters();
    }
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 335 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" compound_statement : LCURL statements RCURL "<<endl<<endl;
        outlog<<"{\n"<<(yyvsp[(3) - (4)])->getname()<<"\n}"<<endl<<endl;
        symtab->exit_scope();
        (yyval) = make_node("{\n"+(yyvsp[(3) - (4)])->getname()+"\n}", "compound_stmnt");
    }
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 342 "22101459_22101458.y"
    {
        symtab->enter_scope();
        insert_pending_parameters();
    }
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 347 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" compound_statement : LCURL RCURL "<<endl<<endl;
        outlog<<"{\n}"<<endl<<endl;
        symtab->exit_scope();
        (yyval) = make_node("{\n}", "compound_stmnt");
    }
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 356 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" var_declaration : type_specifier declaration_list SEMICOLON "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<" "<<(yyvsp[(2) - (3)])->getname()<<";"<<endl<<endl;
        if ((yyvsp[(1) - (3)])->getname() == "void") {
            report_error("variable type can not be void ");
        } else {
            for (const auto &var : collected_vars) {
                declare_variable(var.name, (yyvsp[(1) - (3)])->getname(), var.is_array, var.array_size, false);
            }
        }
        collected_vars.clear();
        (yyval) = make_node((yyvsp[(1) - (3)])->getname()+" "+(yyvsp[(2) - (3)])->getname()+";", "var_decl", (yyvsp[(1) - (3)])->getname());
    }
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 372 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" type_specifier : INT "<<endl<<endl;
        outlog<<"int"<<endl<<endl;
        (yyval) = make_node("int", "type_spec");
    }
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 378 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" type_specifier : FLOAT "<<endl<<endl;
        outlog<<"float"<<endl<<endl;
        (yyval) = make_node("float", "type_spec");
    }
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 384 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" type_specifier : VOID "<<endl<<endl;
        outlog<<"void"<<endl<<endl;
        (yyval) = make_node("void", "type_spec");
    }
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 392 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" declaration_list : declaration_list COMMA ID "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<","<<(yyvsp[(3) - (3)])->getname()<<endl<<endl;
        collected_vars.push_back({(yyvsp[(3) - (3)])->getname(), false, 0});
        (yyval) = make_node((yyvsp[(1) - (3)])->getname()+","+(yyvsp[(3) - (3)])->getname(), "decl_list");
    }
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 399 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD "<<endl<<endl;
        outlog<<(yyvsp[(1) - (6)])->getname()<<","<<(yyvsp[(3) - (6)])->getname()<<"["<<(yyvsp[(5) - (6)])->getname()<<"]"<<endl<<endl;
        collected_vars.push_back({(yyvsp[(3) - (6)])->getname(), true, stoi((yyvsp[(5) - (6)])->getname())});
        (yyval) = make_node((yyvsp[(1) - (6)])->getname()+","+(yyvsp[(3) - (6)])->getname()+"["+(yyvsp[(5) - (6)])->getname()+"]", "decl_list");
    }
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 406 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" declaration_list : ID "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        collected_vars.clear();
        collected_vars.push_back({(yyvsp[(1) - (1)])->getname(), false, 0});
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "decl_list");
    }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 414 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" declaration_list : ID LTHIRD CONST_INT RTHIRD "<<endl<<endl;
        outlog<<(yyvsp[(1) - (4)])->getname()<<"["<<(yyvsp[(3) - (4)])->getname()<<"]"<<endl<<endl;
        collected_vars.clear();
        collected_vars.push_back({(yyvsp[(1) - (4)])->getname(), true, stoi((yyvsp[(3) - (4)])->getname())});
        (yyval) = make_node((yyvsp[(1) - (4)])->getname()+"["+(yyvsp[(3) - (4)])->getname()+"]", "decl_list");
    }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 424 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statements : statement "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "statements");
    }
    break;

  case 28:
/* Line 1792 of yacc.c  */
#line 430 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statements : statements statement "<<endl<<endl;
        outlog<<(yyvsp[(1) - (2)])->getname()<<"\n"<<(yyvsp[(2) - (2)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (2)])->getname()+"\n"+(yyvsp[(2) - (2)])->getname(), "statements");
    }
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 438 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : var_declaration "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "stmnt");
    }
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 444 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : expression_statement "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "stmnt");
    }
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 450 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : compound_statement "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "stmnt");
    }
    break;

  case 32:
/* Line 1792 of yacc.c  */
#line 456 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement "<<endl<<endl;
        outlog<<"for("<<(yyvsp[(3) - (7)])->getname()<<(yyvsp[(4) - (7)])->getname()<<(yyvsp[(5) - (7)])->getname()<<")\n"<<(yyvsp[(7) - (7)])->getname()<<endl<<endl;
        (yyval) = make_node("for("+(yyvsp[(3) - (7)])->getname()+(yyvsp[(4) - (7)])->getname()+(yyvsp[(5) - (7)])->getname()+")\n"+(yyvsp[(7) - (7)])->getname(), "stmnt");
    }
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 462 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : IF LPAREN expression RPAREN statement "<<endl<<endl;
        outlog<<"if("<<(yyvsp[(3) - (5)])->getname()<<")\n"<<(yyvsp[(5) - (5)])->getname()<<endl<<endl;
        (yyval) = make_node("if("+(yyvsp[(3) - (5)])->getname()+")\n"+(yyvsp[(5) - (5)])->getname(), "stmnt");
    }
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 468 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : IF LPAREN expression RPAREN statement ELSE statement "<<endl<<endl;
        outlog<<"if("<<(yyvsp[(3) - (7)])->getname()<<")\n"<<(yyvsp[(5) - (7)])->getname()<<"\nelse\n"<<(yyvsp[(7) - (7)])->getname()<<endl<<endl;
        (yyval) = make_node("if("+(yyvsp[(3) - (7)])->getname()+")\n"+(yyvsp[(5) - (7)])->getname()+"\nelse\n"+(yyvsp[(7) - (7)])->getname(), "stmnt");
    }
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 474 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : WHILE LPAREN expression RPAREN statement "<<endl<<endl;
        outlog<<"while("<<(yyvsp[(3) - (5)])->getname()<<")\n"<<(yyvsp[(5) - (5)])->getname()<<endl<<endl;
        (yyval) = make_node("while("+(yyvsp[(3) - (5)])->getname()+")\n"+(yyvsp[(5) - (5)])->getname(), "stmnt");
    }
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 480 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : PRINTLN LPAREN ID RPAREN SEMICOLON "<<endl<<endl;
        outlog<<"printf("<<(yyvsp[(3) - (5)])->getname()<<");"<<endl<<endl;
        symbol_info *symbol = symtab->lookup((yyvsp[(3) - (5)])->getname());
        if (symbol == nullptr || symbol->get_symbol_kind() == "FUNCTION") {
            report_error("Undeclared variable " + (yyvsp[(3) - (5)])->getname());
        } else if (symbol->get_symbol_kind() == "ARRAY") {
            report_error("variable is of array type : " + (yyvsp[(3) - (5)])->getname());
        }
        (yyval) = make_node("printf("+(yyvsp[(3) - (5)])->getname()+");", "stmnt");
    }
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 492 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" statement : RETURN expression SEMICOLON "<<endl<<endl;
        outlog<<"return "<<(yyvsp[(2) - (3)])->getname()<<";"<<endl<<endl;
        (yyval) = make_node("return "+(yyvsp[(2) - (3)])->getname()+";", "stmnt");
    }
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 500 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" expression_statement : SEMICOLON "<<endl<<endl;
        outlog<<";"<<endl<<endl;
        (yyval) = make_node(";", "expr_stmnt");
    }
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 506 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" expression_statement : expression SEMICOLON "<<endl<<endl;
        outlog<<(yyvsp[(1) - (2)])->getname()<<";"<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (2)])->getname()+";", "expr_stmnt", node_type((yyvsp[(1) - (2)])));
    }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 514 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" variable : ID "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        symbol_info *symbol = symtab->lookup((yyvsp[(1) - (1)])->getname());
        string data_type = "error";
        if (symbol == nullptr || symbol->get_symbol_kind() == "FUNCTION") {
            report_error("Undeclared variable " + (yyvsp[(1) - (1)])->getname());
        } else {
            if (symbol->get_symbol_kind() == "ARRAY") {
                report_error("variable is of array type : " + (yyvsp[(1) - (1)])->getname());
                data_type = "error";
            } else {
                data_type = symbol->get_data_type();
            }
        }
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "variable", data_type);
    }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 532 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" variable : ID LTHIRD expression RTHIRD "<<endl<<endl;
        outlog<<(yyvsp[(1) - (4)])->getname()<<"["<<(yyvsp[(3) - (4)])->getname()<<"]"<<endl<<endl;
        symbol_info *symbol = symtab->lookup((yyvsp[(1) - (4)])->getname());
        string data_type = "error";
        if (symbol == nullptr || symbol->get_symbol_kind() == "FUNCTION") {
            report_error("Undeclared variable " + (yyvsp[(1) - (4)])->getname());
        } else if (symbol->get_symbol_kind() != "ARRAY") {
            report_error("variable is not of array type : " + (yyvsp[(1) - (4)])->getname());
        } else {
            if (node_type((yyvsp[(3) - (4)])) != "int") {
                report_error("array index is not of integer type : " + (yyvsp[(1) - (4)])->getname());
            }
            data_type = symbol->get_data_type();
        }
        (yyval) = make_node((yyvsp[(1) - (4)])->getname()+"["+(yyvsp[(3) - (4)])->getname()+"]", "variable", data_type);
    }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 552 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" expression : logic_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "expression", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 558 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" expression : variable ASSIGNOP logic_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<"="<<(yyvsp[(3) - (3)])->getname()<<endl<<endl;
        string left_type = node_type((yyvsp[(1) - (3)]));
        string right_type = node_type((yyvsp[(3) - (3)]));
        if (is_void_type(left_type) || is_void_type(right_type)) {
            report_error("operation on void type ");
        } else if (is_int_type(left_type) && is_float_type(right_type)) {
            report_error("Warning: Assignment of float value into variable of integer type ");
        }
        (yyval) = make_node((yyvsp[(1) - (3)])->getname()+"="+(yyvsp[(3) - (3)])->getname(), "expression", left_type);
    }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 573 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" logic_expression : rel_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "logic_expr", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 579 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" logic_expression : rel_expression LOGICOP rel_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<(yyvsp[(2) - (3)])->getname()<<(yyvsp[(3) - (3)])->getname()<<endl<<endl;
        if (is_void_type(node_type((yyvsp[(1) - (3)]))) || is_void_type(node_type((yyvsp[(3) - (3)])))) {
            report_error("operation on void type ");
        }
        (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "logic_expr", "int");
    }
    break;

  case 46:
/* Line 1792 of yacc.c  */
#line 590 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" rel_expression : simple_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "rel_expr", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 596 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" rel_expression : simple_expression RELOP simple_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<(yyvsp[(2) - (3)])->getname()<<(yyvsp[(3) - (3)])->getname()<<endl<<endl;
        if (is_void_type(node_type((yyvsp[(1) - (3)]))) || is_void_type(node_type((yyvsp[(3) - (3)])))) {
            report_error("operation on void type ");
        }
        (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "rel_expr", "int");
    }
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 607 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" simple_expression : term "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "simple_expr", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 613 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" simple_expression : simple_expression ADDOP term "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<(yyvsp[(2) - (3)])->getname()<<(yyvsp[(3) - (3)])->getname()<<endl<<endl;
        string left_type = node_type((yyvsp[(1) - (3)]));
        string right_type = node_type((yyvsp[(3) - (3)]));
        if (is_void_type(left_type) || is_void_type(right_type)) {
            report_error("operation on void type ");
            (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "simple_expr", "error");
        } else if (is_float_type(left_type) || is_float_type(right_type)) {
            (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "simple_expr", "float");
        } else {
            (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "simple_expr", "int");
        }
    }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 630 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" term : unary_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "term", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 636 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" term : term MULOP unary_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<(yyvsp[(2) - (3)])->getname()<<(yyvsp[(3) - (3)])->getname()<<endl<<endl;
        string operation = (yyvsp[(2) - (3)])->getname();
        string left_type = node_type((yyvsp[(1) - (3)]));
        string right_type = node_type((yyvsp[(3) - (3)]));
        if (operation == "/" || operation == "%") {
            check_division_or_modulus_zero(operation, (yyvsp[(3) - (3)]));
        }
        if (is_void_type(left_type) || is_void_type(right_type)) {
            report_error("operation on void type ");
            (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "term", "error");
        } else if (operation == "%") {
            if (!is_int_type(left_type) || !is_int_type(right_type)) {
                report_error("Modulus operator on non integer type ");
                (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "term", "error");
            } else {
                (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "term", "int");
            }
        } else if (is_float_type(left_type) || is_float_type(right_type)) {
            (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "term", "float");
        } else {
            (yyval) = make_node((yyvsp[(1) - (3)])->getname()+(yyvsp[(2) - (3)])->getname()+(yyvsp[(3) - (3)])->getname(), "term", "int");
        }
    }
    break;

  case 52:
/* Line 1792 of yacc.c  */
#line 664 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" unary_expression : ADDOP unary_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (2)])->getname()<<(yyvsp[(2) - (2)])->getname()<<endl<<endl;
        string type = node_type((yyvsp[(2) - (2)]));
        if (is_void_type(type)) {
            report_error("operation on void type ");
            (yyval) = make_node((yyvsp[(1) - (2)])->getname()+(yyvsp[(2) - (2)])->getname(), "unary_expr", "error");
        } else {
            (yyval) = make_node((yyvsp[(1) - (2)])->getname()+(yyvsp[(2) - (2)])->getname(), "unary_expr", type);
        }
    }
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 676 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" unary_expression : NOT unary_expression "<<endl<<endl;
        outlog<<"!"<<(yyvsp[(2) - (2)])->getname()<<endl<<endl;
        if (is_void_type(node_type((yyvsp[(2) - (2)])))) {
            report_error("operation on void type ");
            (yyval) = make_node("!"+(yyvsp[(2) - (2)])->getname(), "unary_expr", "error");
        } else {
            (yyval) = make_node("!"+(yyvsp[(2) - (2)])->getname(), "unary_expr", "int");
        }
    }
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 687 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" unary_expression : factor "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "unary_expr", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 695 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" factor : variable "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "factor", node_type((yyvsp[(1) - (1)])));
    }
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 701 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" factor : ID LPAREN argument_list RPAREN "<<endl<<endl;
        outlog<<(yyvsp[(1) - (4)])->getname()<<"("<<(yyvsp[(3) - (4)])->getname()<<")"<<endl<<endl;
        symbol_info *symbol = symtab->lookup((yyvsp[(1) - (4)])->getname());
        string data_type = "error";
        if (symbol == nullptr || symbol->get_symbol_kind() != "FUNCTION") {
            report_error("Undeclared function: " + (yyvsp[(1) - (4)])->getname());
        } else {
            check_function_call(symbol, (yyvsp[(3) - (4)]), (yyvsp[(1) - (4)])->getname());
            data_type = symbol->get_data_type();
        }
        (yyval) = make_node((yyvsp[(1) - (4)])->getname()+"("+(yyvsp[(3) - (4)])->getname()+")", "factor", data_type);
    }
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 715 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" factor : LPAREN expression RPAREN "<<endl<<endl;
        outlog<<"("<<(yyvsp[(2) - (3)])->getname()<<")"<<endl<<endl;
        (yyval) = make_node("("+(yyvsp[(2) - (3)])->getname()+")", "factor", node_type((yyvsp[(2) - (3)])));
    }
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 721 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" factor : CONST_INT "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "factor", "int");
    }
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 727 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" factor : CONST_FLOAT "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "factor", "float");
    }
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 733 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" factor : variable INCOP "<<endl<<endl;
        outlog<<(yyvsp[(1) - (2)])->getname()<<"++"<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (2)])->getname()+"++", "factor", node_type((yyvsp[(1) - (2)])));
    }
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 739 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" factor : variable DECOP "<<endl<<endl;
        outlog<<(yyvsp[(1) - (2)])->getname()<<"--"<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (2)])->getname()+"--", "factor", node_type((yyvsp[(1) - (2)])));
    }
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 747 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" argument_list : arguments "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "arg_list");
        (yyval)->set_arg_types((yyvsp[(1) - (1)])->get_arg_types());
    }
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 754 "22101459_22101458.y"
    {
        (yyval) = make_node("", "arg_list");
        (yyval)->set_arg_types(vector<string>());
    }
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 761 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" arguments : arguments COMMA logic_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (3)])->getname()<<","<<(yyvsp[(3) - (3)])->getname()<<endl<<endl;
        vector<string> arg_types = (yyvsp[(1) - (3)])->get_arg_types();
        arg_types.push_back(node_type((yyvsp[(3) - (3)])));
        (yyval) = make_node((yyvsp[(1) - (3)])->getname()+","+(yyvsp[(3) - (3)])->getname(), "arguments");
        (yyval)->set_arg_types(arg_types);
    }
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 770 "22101459_22101458.y"
    {
        outlog<<"At line no: "<<lines<<" arguments : logic_expression "<<endl<<endl;
        outlog<<(yyvsp[(1) - (1)])->getname()<<endl<<endl;
        (yyval) = make_node((yyvsp[(1) - (1)])->getname(), "arguments");
        vector<string> arg_types;
        arg_types.push_back(node_type((yyvsp[(1) - (1)])));
        (yyval)->set_arg_types(arg_types);
    }
    break;


/* Line 1792 of yacc.c  */
#line 2494 "y.tab.c"
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

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



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
		      yytoken, &yylval);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

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


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2055 of yacc.c  */
#line 780 "22101459_22101458.y"


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cout<<"Usage: "<<argv[0]<<" <input_file>"<<endl;
        return 1;
    }

    yyin = fopen(argv[1], "r");
    outlog.open("22101459_22101458_log.txt", ios::trunc);
    errlog.open("22101459_22101458_error.txt", ios::trunc);

    if(yyin == NULL)
    {
        cout<<"Couldn't open file"<<endl;
        return 0;
    }

    symtab = new symbol_table(10, &outlog);
    symtab->enter_scope();

    yyparse();

    outlog<<"Total lines: "<<lines<<endl;
    outlog<<"Total errors: "<<error_count<<endl;
    errlog<<"Total errors: "<<error_count<<endl;

    outlog.close();
    errlog.close();
    fclose(yyin);

    delete symtab;

    return 0;
}
