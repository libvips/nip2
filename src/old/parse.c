/* A Bison parser, made by GNU Bison 3.7.  */

/* Skeleton implementation for Bison GLR parsers in C

   Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

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

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 0






/* First part of user prologue.  */
#line 1 "parse.y"


/* Parse ip's macro language.
 */

/*

    Copyright (C) 1991-2003 The National Gallery

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#include "ip.h"

/*
#define DEBUG
 */

/* trace text read system
#define DEBUG_CHARACTER
 */

/* The lexer from lex.l.
 */
int yylex( void );
void yyrestart( FILE *input_file );

/* Declare file-private stuff, shared with the lexer. Bison will put this
 * stuff into parse.h, so just declare, don't define. Sadly we can't have
 * these things static :(
 */

/* Global .. the symbol whose definition we are currently parsing, the symbol
 * which all defs in this parse action should be made local to.
 */
extern Symbol *current_symbol;
extern Symbol *root_symbol;

/* The current parse context.
 */
extern Compile *current_compile;
extern ParseNode *current_parsenode;

/* The kit we are adding new symbols to.
 */
extern Toolkit *current_kit;

/* Where it should go in the kit.
 */
extern int tool_position;

/* Lineno of start of last top-level def.
 */
extern int last_top_lineno;

/* Text we've gathered in this lex.
 */
extern char lex_text_buffer[MAX_STRSIZE];

/* Stack of symbols for parser - each represents a new scope level.
 */
extern Symbol *scope_stack_symbol[MAX_SSTACK];
extern Compile *scope_stack_compile[MAX_SSTACK];
extern int scope_sp;

/* Use to generate unique ids for anonymouse parse objects (eg. lambdas etc).
 */
extern int parse_object_id;

/* Get text for parsed objects.
 */
char *input_text( char *out );
void input_reset( void );
void input_push( int n );
void input_backtoch( char ch );
void input_back1( void );
void input_pop( void );

/* Nest and unnest scopes.
 */
void scope_push( void );
void scope_pop( void );
void scope_pop_all( void );
void scope_reset( void );

/* Helper functions.
 */
void *parse_toplevel_end( Symbol *sym );
void *parse_access_end( Symbol *sym, Symbol *main );


#line 169 "parse.c"

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

#include "parse.h"

/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TK_TAG = 3,                     /* TK_TAG  */
  YYSYMBOL_TK_IDENT = 4,                   /* TK_IDENT  */
  YYSYMBOL_TK_CONST = 5,                   /* TK_CONST  */
  YYSYMBOL_TK_DOTDOTDOT = 6,               /* TK_DOTDOTDOT  */
  YYSYMBOL_TK_LAMBDA = 7,                  /* TK_LAMBDA  */
  YYSYMBOL_TK_FROM = 8,                    /* TK_FROM  */
  YYSYMBOL_TK_TO = 9,                      /* TK_TO  */
  YYSYMBOL_TK_SUCHTHAT = 10,               /* TK_SUCHTHAT  */
  YYSYMBOL_TK_UMINUS = 11,                 /* TK_UMINUS  */
  YYSYMBOL_TK_UPLUS = 12,                  /* TK_UPLUS  */
  YYSYMBOL_TK_POW = 13,                    /* TK_POW  */
  YYSYMBOL_TK_LESS = 14,                   /* TK_LESS  */
  YYSYMBOL_TK_LESSEQ = 15,                 /* TK_LESSEQ  */
  YYSYMBOL_TK_MORE = 16,                   /* TK_MORE  */
  YYSYMBOL_TK_MOREEQ = 17,                 /* TK_MOREEQ  */
  YYSYMBOL_TK_NOTEQ = 18,                  /* TK_NOTEQ  */
  YYSYMBOL_TK_LAND = 19,                   /* TK_LAND  */
  YYSYMBOL_TK_LOR = 20,                    /* TK_LOR  */
  YYSYMBOL_TK_BAND = 21,                   /* TK_BAND  */
  YYSYMBOL_TK_BOR = 22,                    /* TK_BOR  */
  YYSYMBOL_TK_JOIN = 23,                   /* TK_JOIN  */
  YYSYMBOL_TK_DIFF = 24,                   /* TK_DIFF  */
  YYSYMBOL_TK_IF = 25,                     /* TK_IF  */
  YYSYMBOL_TK_THEN = 26,                   /* TK_THEN  */
  YYSYMBOL_TK_ELSE = 27,                   /* TK_ELSE  */
  YYSYMBOL_TK_CHAR = 28,                   /* TK_CHAR  */
  YYSYMBOL_TK_SHORT = 29,                  /* TK_SHORT  */
  YYSYMBOL_TK_CLASS = 30,                  /* TK_CLASS  */
  YYSYMBOL_TK_SCOPE = 31,                  /* TK_SCOPE  */
  YYSYMBOL_TK_INT = 32,                    /* TK_INT  */
  YYSYMBOL_TK_FLOAT = 33,                  /* TK_FLOAT  */
  YYSYMBOL_TK_DOUBLE = 34,                 /* TK_DOUBLE  */
  YYSYMBOL_TK_SIGNED = 35,                 /* TK_SIGNED  */
  YYSYMBOL_TK_UNSIGNED = 36,               /* TK_UNSIGNED  */
  YYSYMBOL_TK_COMPLEX = 37,                /* TK_COMPLEX  */
  YYSYMBOL_TK_SEPARATOR = 38,              /* TK_SEPARATOR  */
  YYSYMBOL_TK_DIALOG = 39,                 /* TK_DIALOG  */
  YYSYMBOL_TK_LSHIFT = 40,                 /* TK_LSHIFT  */
  YYSYMBOL_TK_RSHIFT = 41,                 /* TK_RSHIFT  */
  YYSYMBOL_42_ = 42,                       /* ','  */
  YYSYMBOL_43_ = 43,                       /* '@'  */
  YYSYMBOL_44_ = 44,                       /* '^'  */
  YYSYMBOL_TK_EQ = 45,                     /* TK_EQ  */
  YYSYMBOL_TK_PEQ = 46,                    /* TK_PEQ  */
  YYSYMBOL_TK_PNOTEQ = 47,                 /* TK_PNOTEQ  */
  YYSYMBOL_48_ = 48,                       /* '+'  */
  YYSYMBOL_49_ = 49,                       /* '-'  */
  YYSYMBOL_50_ = 50,                       /* '*'  */
  YYSYMBOL_51_ = 51,                       /* '/'  */
  YYSYMBOL_52_ = 52,                       /* '%'  */
  YYSYMBOL_53_ = 53,                       /* '!'  */
  YYSYMBOL_54_ = 54,                       /* '~'  */
  YYSYMBOL_55_ = 55,                       /* ':'  */
  YYSYMBOL_56_ = 56,                       /* '('  */
  YYSYMBOL_57_ = 57,                       /* '['  */
  YYSYMBOL_TK_APPLICATION = 58,            /* TK_APPLICATION  */
  YYSYMBOL_59_ = 59,                       /* '?'  */
  YYSYMBOL_60_ = 60,                       /* '.'  */
  YYSYMBOL_61_ = 61,                       /* '='  */
  YYSYMBOL_62_ = 62,                       /* ';'  */
  YYSYMBOL_63_ = 63,                       /* '{'  */
  YYSYMBOL_64_ = 64,                       /* '}'  */
  YYSYMBOL_65_ = 65,                       /* ')'  */
  YYSYMBOL_66_ = 66,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 67,                  /* $accept  */
  YYSYMBOL_select = 68,                    /* select  */
  YYSYMBOL_prhs = 69,                      /* prhs  */
  YYSYMBOL_main = 70,                      /* main  */
  YYSYMBOL_single_definition = 71,         /* single_definition  */
  YYSYMBOL_directive = 72,                 /* directive  */
  YYSYMBOL_toplevel_definition = 73,       /* toplevel_definition  */
  YYSYMBOL_74_1 = 74,                      /* $@1  */
  YYSYMBOL_definition = 75,                /* definition  */
  YYSYMBOL_76_2 = 76,                      /* $@2  */
  YYSYMBOL_params_plus_rhs = 77,           /* params_plus_rhs  */
  YYSYMBOL_78_3 = 78,                      /* $@3  */
  YYSYMBOL_79_4 = 79,                      /* $@4  */
  YYSYMBOL_80_5 = 80,                      /* $@5  */
  YYSYMBOL_params = 81,                    /* params  */
  YYSYMBOL_body = 82,                      /* body  */
  YYSYMBOL_crhs = 83,                      /* crhs  */
  YYSYMBOL_84_6 = 84,                      /* $@6  */
  YYSYMBOL_rhs = 85,                       /* rhs  */
  YYSYMBOL_locals = 86,                    /* locals  */
  YYSYMBOL_optsemi = 87,                   /* optsemi  */
  YYSYMBOL_deflist = 88,                   /* deflist  */
  YYSYMBOL_89_7 = 89,                      /* $@7  */
  YYSYMBOL_90_8 = 90,                      /* $@8  */
  YYSYMBOL_cexprlist = 91,                 /* cexprlist  */
  YYSYMBOL_expr = 92,                      /* expr  */
  YYSYMBOL_lambda = 93,                    /* lambda  */
  YYSYMBOL_94_9 = 94,                      /* $@9  */
  YYSYMBOL_list_expression = 95,           /* list_expression  */
  YYSYMBOL_96_10 = 96,                     /* $@10  */
  YYSYMBOL_frompred_list = 97,             /* frompred_list  */
  YYSYMBOL_generator = 98,                 /* generator  */
  YYSYMBOL_frompred = 99,                  /* frompred  */
  YYSYMBOL_comma_list = 100,               /* comma_list  */
  YYSYMBOL_binop = 101,                    /* binop  */
  YYSYMBOL_signed = 102,                   /* signed  */
  YYSYMBOL_unsigned = 103,                 /* unsigned  */
  YYSYMBOL_uop = 104,                      /* uop  */
  YYSYMBOL_simple_pattern = 105,           /* simple_pattern  */
  YYSYMBOL_leaf_pattern = 106,             /* leaf_pattern  */
  YYSYMBOL_complex_pattern = 107,          /* complex_pattern  */
  YYSYMBOL_list_pattern = 108              /* list_pattern  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template, here we set
   the default value of $$ to a zeroed-out value.  Since the default
   value is undefined, this behavior is technically correct.  */
static YYSTYPE yyval_default;



#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#ifdef __cplusplus
  typedef bool yybool;
# define yytrue true
# define yyfalse false
#else
  /* When we move to stdbool, get rid of the various casts to yybool.  */
  typedef signed char yybool;
# define yytrue 1
# define yyfalse 0
#endif

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(Env) setjmp (Env)
/* Pacify Clang and ICC.  */
# define YYLONGJMP(Env, Val)                    \
 do {                                           \
   longjmp (Env, Val);                          \
   YY_ASSERT (0);                               \
 } while (yyfalse)
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

/* The _Noreturn keyword of C11.  */
#ifndef _Noreturn
# if (defined __cplusplus \
      && ((201103 <= __cplusplus && !(__GNUC__ == 4 && __GNUC_MINOR__ == 7)) \
          || (defined _MSC_VER && 1900 <= _MSC_VER)))
#  define _Noreturn [[noreturn]]
# elif (!defined __cplusplus                     \
        && (201112 <= (defined __STDC_VERSION__ ? __STDC_VERSION__ : 0)  \
            || 4 < __GNUC__ + (7 <= __GNUC_MINOR__) \
            || (defined __apple_build_version__ \
                ? 6000000 <= __apple_build_version__ \
                : 3 < __clang_major__ + (5 <= __clang_minor__))))
   /* _Noreturn works as-is.  */
# elif 2 < __GNUC__ + (8 <= __GNUC_MINOR__) || 0x5110 <= __SUNPRO_C
#  define _Noreturn __attribute__ ((__noreturn__))
# elif 1200 <= (defined _MSC_VER ? _MSC_VER : 0)
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  35
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1915

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  127
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  242
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 7
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   300

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
       2,     2,     2,    53,     2,     2,     2,    52,     2,     2,
      56,    65,    50,    48,    42,    49,    60,    51,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    55,    62,
       2,    61,     2,    59,    43,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    57,     2,    66,    44,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    63,     2,    64,    54,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    45,    46,    47,
      58
};

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   192,   192,   193,   194,   197,   217,   220,   225,   227,
     231,   234,   240,   251,   280,   280,   293,   293,   391,   399,
     403,   391,   441,   443,   485,   488,   494,   494,   539,   542,
     548,   549,   550,   553,   555,   559,   559,   564,   564,   572,
     575,   581,   584,   587,   591,   595,   599,   602,   605,   606,
     609,   612,   613,   617,   617,   665,   668,   671,   674,   677,
     677,   735,   738,   747,   749,   754,   777,   778,   793,   796,
     805,   808,   811,   814,   817,   820,   823,   826,   829,   832,
     835,   838,   841,   844,   847,   850,   853,   856,   859,   862,
     865,   868,   871,   874,   877,   880,   883,   894,   905,   907,
     910,   912,   916,   919,   922,   925,   928,   931,   934,   937,
     940,   943,   946,   949,   952,   955,   963,   964,   967,   970,
     973,   976,   987,   991,   999,  1005,  1009,  1012
};
#endif

#define YYPACT_NINF (-205)
#define YYTABLE_NINF (-124)

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     219,  1794,  -205,  -205,   -12,  -205,     5,  -205,  -205,  -205,
    -205,     3,  1794,  1794,  1794,  -205,  1794,  1794,  1724,   117,
     999,  -205,  -205,  -205,  -205,     4,  1794,  -205,     9,  -205,
    -205,   -40,   121,   -40,  -205,  -205,  -205,   141,   141,   767,
     141,   141,   -36,   -29,    22,    -9,   -34,   473,    -8,    60,
    -205,   651,    39,  1794,  1794,  1794,  1794,  1794,  1794,  1794,
    1794,  1794,  1794,  1794,  1794,  1794,  1794,  1794,  1794,  1794,
    1794,  1794,  1794,  1794,  1794,  1794,  1794,  1794,  1794,  1794,
    1794,   -19,  -205,  1855,    32,   -40,  -205,  -205,  -205,   130,
      28,  -205,    37,  -205,  -205,   121,  1794,  1794,  1794,    41,
    1794,    42,    45,    49,  1794,  1794,  -205,    50,    51,    53,
     350,  -205,  1794,  -205,  1173,   141,  1579,  1579,  1579,  1579,
    1463,  1289,  1231,  1521,  1347,   141,   141,  1594,  1594,  1289,
    1405,  1463,  1463,  1463,  1652,  1652,  1666,  1666,  1666,   141,
    -205,  -205,  -205,  -205,   115,    37,    81,    62,  -205,    89,
      66,   121,  -205,    76,    37,  1057,   825,   141,  1794,   141,
    1794,  1794,  1794,   141,   532,  1794,  1794,  1794,  -205,   234,
     121,   709,  -205,  -205,    40,  -205,   130,  -205,    37,  -205,
    1779,  -205,  -205,  1794,   141,   141,   141,   141,  -205,   141,
     141,   141,  -205,  -205,    26,   361,  1794,    71,  -205,  -205,
     883,   -16,  1115,   -56,  1794,  -205,   292,   941,  -205,  -205,
    -205,  1794,  -205,    34,  -205,  1809,  -205,   999,  -205,  1794,
     591,  -205,  -205,    44,    27,    75,  1739,   417,   999,  -205,
    -205,    82,   -40,  -205,  -205,    47,   125,  1794,  -205,  -205,
     -40,  -205
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     8,    39,    14,    18,     0,     5,    44,    43,
      42,     0,     0,     0,     0,    45,     0,     0,    98,     0,
       6,    48,    49,    51,    52,    14,     7,    12,     0,     3,
      10,    33,     0,    33,    22,     1,    53,   112,   115,     0,
     113,   114,     0,     0,    99,   101,     0,     0,     0,     0,
      62,    69,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    47,     9,    40,     0,    33,    11,   122,   123,     0,
       0,    15,    16,   116,     4,    19,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    41,     0,     0,     0,
       0,    59,     0,    61,    97,    78,    87,    88,    89,    90,
      92,    82,    85,    83,    86,    77,    96,    79,    80,    84,
      81,    91,    93,    94,    70,    72,    75,    74,    76,    71,
      73,    95,    13,    34,   122,   125,   116,     0,   121,   127,
       0,     0,    18,     0,    23,    54,     0,   108,     0,   109,
       0,     0,     0,   110,     0,     0,     0,     0,    55,     0,
       0,    69,    68,   124,     0,   119,     0,   120,   118,    17,
       0,    20,    25,     0,   111,   103,   105,   107,    50,   104,
     106,   102,    56,    63,     0,     0,     0,     0,   126,    26,
      28,     0,    46,     0,     0,    57,     0,    69,   117,    24,
      39,     0,    30,     0,    21,     0,    60,    65,    58,    27,
      33,    32,    35,     0,    43,    42,    98,     0,    67,    66,
      64,     0,    33,    31,    37,    43,    62,     0,    29,    36,
      33,    38
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -205,  -205,  -205,  -205,   122,  -205,  -205,  -205,  -204,  -205,
       0,  -205,  -205,  -205,  -205,  -205,  -205,  -205,   -81,  -205,
      -5,  -205,  -205,  -205,   -59,    -1,  -205,  -205,  -205,  -205,
    -205,   -62,  -205,  -109,  -205,  -205,  -205,  -205,    -2,   -88,
     -87,   -21
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,    25,    29,    30,    31,    32,    91,   152,
      33,    34,   153,   201,    95,   181,   209,   210,   182,   214,
      86,   223,   232,   240,    26,    81,    21,    96,    22,   170,
     203,   193,   230,    52,    23,    48,    49,    24,   145,    93,
     149,   150
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      20,   146,   147,   172,    -2,    35,   215,    36,    99,   222,
     216,    37,    38,    39,    84,    40,    41,    47,    51,   234,
     102,   107,    85,   103,   108,    83,    27,    28,    94,    98,
      92,   104,   144,    88,   204,  -122,   100,   142,    87,    88,
      79,    80,    27,    28,    87,    88,   212,   213,    87,    88,
     101,   173,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     143,   151,   -43,  -123,    89,    90,   197,   172,   109,   -43,
      89,    90,   151,   154,   148,   155,   156,   157,   221,   159,
      89,    90,   -43,   163,   164,   113,   158,   160,   233,   169,
     161,   171,   -43,   -43,   162,   165,   166,   -42,   167,   173,
       8,     9,    10,   174,    11,    87,    88,   175,    12,    13,
     -42,   176,   177,  -121,   144,    88,   208,   180,   146,   147,
     -42,   -42,    14,   237,     8,     9,    10,    82,    15,   178,
     238,   219,   179,   229,    54,   198,     0,   184,     0,   185,
     186,   187,     0,     0,   189,   190,   191,   -62,   194,     0,
      16,    17,    15,    18,    19,     0,     0,    89,    90,   200,
     -62,     0,   202,    50,     0,     0,    89,    90,     0,     0,
     -62,   -62,     0,     0,   206,   207,    78,    18,    19,     0,
      79,    80,     0,   217,     0,     0,     0,     0,     0,     0,
     220,    92,     0,   194,   228,   231,     0,     0,    83,     0,
       0,    92,     0,     0,     0,    47,    51,   239,     0,     0,
       0,     0,     0,     0,     0,   241,   200,     8,     9,    10,
       1,    11,     0,    53,     0,    12,    13,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    14,
       0,     2,     3,     4,     0,    15,     0,     0,     0,     5,
       0,     0,     0,     0,    66,    67,     0,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    16,    17,    78,
      18,    19,     0,    79,    80,     8,     9,    10,     0,    11,
     192,    53,     0,    12,    13,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    14,     0,     0,
       0,     0,     0,    15,     0,     0,     0,     0,     0,     0,
       0,     0,    66,    67,     0,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    16,    17,    78,    18,    19,
       0,    79,    80,     8,     9,    10,     0,    11,   218,     0,
       0,    12,    13,     0,     8,     9,    10,     0,    11,     0,
       0,     0,    12,    13,     0,    14,     0,     0,     0,     0,
       0,    15,     0,     0,     0,     0,    14,     0,     0,     0,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    16,    17,     0,    18,    19,     0,     0,
       0,     0,     0,     0,    16,    17,   168,    18,    19,     0,
       8,   235,   225,     0,    11,     0,     0,   205,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,     0,    15,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      16,    17,     0,   226,   227,     0,     8,     9,    10,     0,
      11,     0,    53,   236,    12,    13,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    14,     0,
       0,     0,     0,     0,    15,     0,     0,     0,     0,     0,
       0,     0,     0,    66,    67,   105,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    16,    17,    78,    18,
      19,     0,    79,    80,     0,     8,     9,    10,   106,    11,
       0,    53,     0,    12,    13,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    14,     0,     0,
       0,     0,     0,    15,     0,     0,     0,     0,     0,     0,
       0,     0,    66,    67,     0,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    16,    17,    78,    18,    19,
       0,    79,    80,     0,     8,     9,    10,   188,    11,     0,
      53,     0,    12,    13,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    14,     0,     0,     0,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
       0,    66,    67,     0,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    16,    17,    78,    18,    19,     0,
      79,    80,     0,    85,     8,     9,    10,   110,    11,     0,
      53,   111,    12,    13,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    14,     0,     0,     0,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
       0,    66,    67,   112,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    16,    17,    78,    18,    19,     0,
      79,    80,     8,     9,    10,   195,    11,     0,    53,     0,
      12,    13,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    14,     0,     0,     0,     0,     0,
      15,     0,     0,     0,     0,     0,     0,     0,     0,    66,
      67,   196,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    16,    17,    78,    18,    19,     0,    79,    80,
       8,     9,    10,     0,    11,     0,    53,     0,    12,    13,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    14,    97,     0,     0,     0,     0,    15,     0,
       0,     0,     0,     0,     0,     0,     0,    66,    67,     0,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      16,    17,    78,    18,    19,     0,    79,    80,     8,     9,
      10,     0,    11,     0,    53,     0,    12,    13,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      14,     0,   183,     0,     0,     0,    15,     0,     0,     0,
       0,     0,     0,     0,     0,    66,    67,     0,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    16,    17,
      78,    18,    19,     0,    79,    80,     8,     9,    10,     0,
      11,     0,    53,     0,    12,    13,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    14,     0,
       0,     0,     0,     0,    15,     0,     0,     0,     0,     0,
       0,     0,     0,    66,    67,   211,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    16,    17,    78,    18,
      19,     0,    79,    80,     8,     9,    10,     0,    11,     0,
      53,     0,    12,    13,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    14,     0,     0,     0,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
       0,    66,    67,   196,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    16,    17,    78,    18,    19,     0,
      79,    80,     8,     9,    10,     0,    11,     0,    53,     0,
      12,    13,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    14,     0,     0,     0,     0,     0,
      15,     0,     0,     0,     0,     0,     0,     0,     0,    66,
      67,     0,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    16,    17,    78,    18,    19,     0,    79,    80,
       8,     9,    10,     0,     0,     0,    53,     0,    12,    13,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    14,     0,     0,     0,     0,     0,    15,     0,
       0,     0,     0,     0,     0,     0,     0,    66,    67,     0,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      16,    17,    78,    18,    19,     0,    79,    80,     8,     9,
      10,     0,     0,     0,    53,     0,    12,    13,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
    -124,     0,     0,     0,     0,     0,    15,     0,     0,     0,
       0,     0,     0,     0,     0,    66,    67,     0,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    16,    17,
      78,    18,    19,     0,    79,    80,     8,     9,    10,     0,
       0,     0,     0,     0,    12,    13,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,     0,     0,
       0,     0,     0,     0,    15,     0,     0,     0,     0,     0,
       0,     0,     0,    66,    67,     0,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    16,    17,    78,    18,
      19,     0,    79,    80,     8,     9,    10,     0,     0,     0,
       0,     0,    12,    13,    54,    55,    56,    57,    58,    59,
      60,     0,    62,    63,    64,    65,     0,     0,     0,     0,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
       0,    66,    67,     0,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    16,    17,    78,    18,    19,     0,
      79,    80,     8,     9,    10,     0,     0,     0,     0,     0,
      12,    13,    54,    55,    56,    57,    58,    59,     0,     0,
      62,    63,    64,    65,     0,     0,     0,     0,     0,     0,
      15,     0,     0,     0,     0,     0,     0,     0,     0,    66,
      67,     0,     0,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    16,    17,    78,    18,    19,     0,    79,    80,
       8,     9,    10,     0,     0,     0,     0,     0,    12,    13,
      54,    55,    56,    57,    58,    59,     0,     0,    62,     0,
      64,    65,     0,     0,     0,     0,     0,     0,    15,     0,
       0,     0,     0,     0,     0,     0,     0,    66,    67,     0,
       0,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      16,    17,    78,    18,    19,     0,    79,    80,     8,     9,
      10,     0,     0,     0,     0,     0,    12,    13,    54,    55,
      56,    57,    58,    59,     0,     0,    62,     0,    64,    65,
       0,     0,     0,     0,     0,     0,    15,     0,     0,     0,
       0,     0,     0,     0,     0,    66,    67,     0,     0,     0,
      70,    71,    72,    73,    74,    75,    76,    77,    16,    17,
      78,    18,    19,     0,    79,    80,     8,     9,    10,     0,
       0,     0,     0,     0,    12,    13,    54,    55,    56,    57,
      58,  -124,     0,     0,     0,     0,    64,    65,     0,     0,
       0,     0,     0,     0,    15,     0,     0,     0,     0,     0,
       0,     0,     0,    66,    67,     0,     0,     0,  -124,  -124,
    -124,    73,    74,    75,    76,    77,    16,    17,    78,    18,
      19,     0,    79,    80,     8,     9,    10,     0,     0,     0,
       0,     0,    12,    13,    54,    55,    56,    57,    58,    59,
       0,     0,     0,     0,    64,    65,     0,     0,     0,     0,
       0,     0,    15,     0,     0,     0,     0,     0,     0,     0,
       0,    66,    67,     0,     0,     0,    70,    71,    72,    73,
      74,    75,    76,    77,    16,    17,    78,    18,    19,     0,
      79,    80,     8,     9,    10,     0,     0,     0,     0,     0,
      12,    13,    54,  -124,  -124,  -124,  -124,     8,     9,    10,
       0,     0,    64,    65,     0,    12,    13,    54,     0,     0,
      15,     0,     0,     0,     0,     0,     0,    64,    65,    66,
      67,     0,     0,     0,     0,    15,     0,    73,    74,    75,
      76,    77,    16,    17,    78,    18,    19,     0,    79,    80,
       0,     0,    73,    74,    75,    76,    77,    16,    17,    78,
      18,    19,     0,    79,    80,     8,     9,    10,     0,     0,
       0,     0,     0,    12,    13,    54,     0,     0,     0,     8,
       9,    10,     0,     0,     0,    64,    65,    12,    13,    54,
       0,     0,     0,    15,     0,     0,     0,     0,     0,    64,
      65,     0,     0,     0,     0,     0,     0,    15,     0,     0,
       0,     0,    75,    76,    77,    16,    17,    78,    18,    19,
       0,    79,    80,     0,     0,     0,     0,     0,     0,    16,
      17,    78,    18,    19,     0,    79,    80,     8,     9,    10,
       0,    11,     0,     0,     0,    12,    13,     0,     0,     0,
       0,     0,     8,   235,   225,     0,    11,     0,     0,    14,
      12,    13,  -100,     0,     0,    15,     0,    42,    43,    44,
      45,    46,     0,     0,    14,     0,     0,  -100,     0,     0,
      15,     0,    42,    43,    44,    45,    46,    16,    17,     0,
      18,    19,     8,     9,    10,     0,    11,     0,     0,     0,
      12,    13,    16,    17,     0,   226,   227,     8,     9,    10,
       0,    11,     0,     0,    14,    12,    13,     0,     0,   199,
      15,     0,     8,   224,   225,     0,    11,     0,     0,    14,
      12,    13,     0,     0,     0,    15,     0,     0,     0,     0,
       0,     0,    16,    17,    14,    18,    19,     0,     0,     0,
      15,     0,     0,     0,     0,     0,     0,    16,    17,     0,
      18,    19,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    16,    17,    53,   226,   227,     0,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    66,    67,     0,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,     0,     0,
      78,     0,     0,     0,    79,    80
};

static const yytype_int16 yycheck[] =
{
       1,    89,    89,   112,     0,     0,    62,     4,    37,   213,
      66,    12,    13,    14,     5,    16,    17,    18,    19,   223,
      29,    29,    62,    32,    32,    26,    38,    39,    33,    65,
      32,    65,     4,     5,     8,     8,    65,     5,     4,     5,
      59,    60,    38,    39,     4,     5,    62,    63,     4,     5,
      28,     4,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      85,    55,    55,     8,    56,    57,   174,   196,    28,    42,
      56,    57,    55,    95,    66,    96,    97,    98,    64,   100,
      56,    57,    55,   104,   105,    66,    65,    65,    64,   110,
      65,   112,    65,    66,    65,    65,    65,    42,    65,     4,
       3,     4,     5,    42,     7,     4,     5,    65,    11,    12,
      55,    42,    66,     8,     4,     5,    65,    61,   226,   226,
      65,    66,    25,    61,     3,     4,     5,    25,    31,   151,
     231,   210,   152,   215,    13,   176,    -1,   158,    -1,   160,
     161,   162,    -1,    -1,   165,   166,   167,    42,   170,    -1,
      53,    54,    31,    56,    57,    -1,    -1,    56,    57,   180,
      55,    -1,   183,    66,    -1,    -1,    56,    57,    -1,    -1,
      65,    66,    -1,    -1,   195,   196,    55,    56,    57,    -1,
      59,    60,    -1,   204,    -1,    -1,    -1,    -1,    -1,    -1,
     211,   213,    -1,   215,   215,   220,    -1,    -1,   219,    -1,
      -1,   223,    -1,    -1,    -1,   226,   227,   232,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   240,   237,     3,     4,     5,
      21,     7,    -1,     9,    -1,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      -1,    42,    43,    44,    -1,    31,    -1,    -1,    -1,    50,
      -1,    -1,    -1,    -1,    40,    41,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,     3,     4,     5,    -1,     7,
      66,     9,    -1,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    40,    41,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,     3,     4,     5,    -1,     7,    66,    -1,
      -1,    11,    12,    -1,     3,     4,     5,    -1,     7,    -1,
      -1,    -1,    11,    12,    -1,    25,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    -1,    -1,    -1,    25,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    53,    54,    -1,    56,    57,    -1,    -1,
      -1,    -1,    -1,    -1,    53,    54,    66,    56,    57,    -1,
       3,     4,     5,    -1,     7,    -1,    -1,    66,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,    54,    -1,    56,    57,    -1,     3,     4,     5,    -1,
       7,    -1,     9,    66,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    -1,     3,     4,     5,    65,     7,
      -1,     9,    -1,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    40,    41,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,     3,     4,     5,    65,     7,    -1,
       9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    -1,    62,     3,     4,     5,     6,     7,    -1,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,     3,     4,     5,     6,     7,    -1,     9,    -1,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
       3,     4,     5,    -1,     7,    -1,     9,    -1,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    -1,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,     3,     4,
       5,    -1,     7,    -1,     9,    -1,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    -1,    27,    -1,    -1,    -1,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,     3,     4,     5,    -1,
       7,    -1,     9,    -1,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,     3,     4,     5,    -1,     7,    -1,
       9,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,     3,     4,     5,    -1,     7,    -1,     9,    -1,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      41,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
       3,     4,     5,    -1,    -1,    -1,     9,    -1,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    -1,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,     3,     4,
       5,    -1,    -1,    -1,     9,    -1,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,     3,     4,     5,    -1,
      -1,    -1,    -1,    -1,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    41,    -1,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    -1,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,
      11,    12,    13,    14,    15,    16,    17,    18,    -1,    -1,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,
      41,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
       3,     4,     5,    -1,    -1,    -1,    -1,    -1,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    21,    -1,
      23,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    -1,
      -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    -1,    59,    60,     3,     4,
       5,    -1,    -1,    -1,    -1,    -1,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    -1,    21,    -1,    23,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,     3,     4,     5,    -1,
      -1,    -1,    -1,    -1,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    -1,    -1,    -1,    23,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
      -1,    -1,    -1,    -1,    23,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    -1,    -1,    -1,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,
      11,    12,    13,    14,    15,    16,    17,     3,     4,     5,
      -1,    -1,    23,    24,    -1,    11,    12,    13,    -1,    -1,
      31,    -1,    -1,    -1,    -1,    -1,    -1,    23,    24,    40,
      41,    -1,    -1,    -1,    -1,    31,    -1,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      -1,    -1,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,     3,     4,     5,    -1,    -1,
      -1,    -1,    -1,    11,    12,    13,    -1,    -1,    -1,     3,
       4,     5,    -1,    -1,    -1,    23,    24,    11,    12,    13,
      -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    -1,    23,
      24,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      54,    55,    56,    57,    -1,    59,    60,     3,     4,     5,
      -1,     7,    -1,    -1,    -1,    11,    12,    -1,    -1,    -1,
      -1,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    25,
      11,    12,    28,    -1,    -1,    31,    -1,    33,    34,    35,
      36,    37,    -1,    -1,    25,    -1,    -1,    28,    -1,    -1,
      31,    -1,    33,    34,    35,    36,    37,    53,    54,    -1,
      56,    57,     3,     4,     5,    -1,     7,    -1,    -1,    -1,
      11,    12,    53,    54,    -1,    56,    57,     3,     4,     5,
      -1,     7,    -1,    -1,    25,    11,    12,    -1,    -1,    30,
      31,    -1,     3,     4,     5,    -1,     7,    -1,    -1,    25,
      11,    12,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    54,    25,    56,    57,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    -1,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    54,     9,    56,    57,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    40,    41,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    -1,
      55,    -1,    -1,    -1,    59,    60
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    21,    42,    43,    44,    50,    68,    69,     3,     4,
       5,     7,    11,    12,    25,    31,    53,    54,    56,    57,
      92,    93,    95,   101,   104,    70,    91,    38,    39,    71,
      72,    73,    74,    77,    78,     0,     4,    92,    92,    92,
      92,    92,    33,    34,    35,    36,    37,    92,   102,   103,
      66,    92,   100,     9,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    40,    41,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    55,    59,
      60,    92,    71,    92,     5,    62,    87,     4,     5,    56,
      57,    75,   105,   106,    87,    81,    94,    26,    65,    37,
      65,    28,    29,    32,    65,    42,    65,    29,    32,    28,
       6,    10,    42,    66,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,     5,    87,     4,   105,   106,   107,    66,   107,
     108,    55,    76,    79,   105,    92,    92,    92,    65,    92,
      65,    65,    65,    92,    92,    65,    65,    65,    66,    92,
      96,    92,   100,     4,    42,    65,    42,    66,   105,    77,
      61,    82,    85,    27,    92,    92,    92,    92,    65,    92,
      92,    92,    66,    98,   105,     6,    42,   106,   108,    30,
      92,    80,    92,    97,     8,    66,    92,    92,    65,    83,
      84,    42,    62,    63,    86,    62,    66,    92,    66,    91,
      92,    64,    75,    88,     4,     5,    56,    57,    92,    98,
      99,    87,    89,    64,    75,     4,    66,    61,    85,    87,
      90,    87
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    67,    68,    68,    68,    68,    69,    69,    70,    70,
      71,    71,    72,    72,    74,    73,    76,    75,    78,    79,
      80,    77,    81,    81,    82,    82,    84,    83,    85,    85,
      86,    86,    86,    87,    87,    89,    88,    90,    88,    91,
      91,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    94,    93,    95,    95,    95,    95,    96,
      95,    95,    95,    97,    97,    98,    99,    99,   100,   100,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   102,   102,
     103,   103,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   105,   105,   105,   105,
     105,   105,   106,   106,   107,   107,   108,   108
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     2,     3,     1,     2,     2,     0,     2,
       1,     2,     1,     3,     0,     2,     0,     3,     0,     0,
       0,     6,     0,     2,     3,     1,     0,     2,     2,     6,
       1,     3,     2,     0,     2,     0,     3,     0,     4,     0,
       2,     3,     1,     1,     1,     1,     6,     2,     1,     1,
       5,     1,     1,     0,     4,     4,     5,     6,     7,     0,
       7,     3,     2,     0,     3,     3,     1,     1,     3,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     0,     1,
       0,     1,     5,     5,     5,     5,     5,     5,     4,     4,
       4,     5,     2,     2,     2,     2,     1,     5,     3,     3,
       3,     2,     1,     1,     2,     1,     3,     1
};


/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
static const yytype_int8 yydprec[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const yytype_int8 yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0
};

/* YYIMMEDIATE[RULE-NUM] -- True iff rule #RULE-NUM is not to be deferred, as
   in the case of predicates.  */
static const yybool yyimmediate[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const yytype_int8 yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     1,     0,     0,     0,     0,     0,     0,    11,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    13,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    15,    17,     0,     0,     0,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       5,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     9,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    19,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      23,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short yyconfl[] =
{
       0,   122,     0,   123,     0,   123,     0,   123,     0,   123,
       0,   122,     0,   122,     0,   122,     0,   122,     0,   121,
       0,   121,     0,   121,     0,   121,     0
};



YYSTYPE yylval;

int yynerrs;
int yychar;

enum { YYENOMEM = -2 };

typedef enum { yyok, yyaccept, yyabort, yyerr } YYRESULTTAG;

#define YYCHK(YYE)                              \
  do {                                          \
    YYRESULTTAG yychk_flag = YYE;               \
    if (yychk_flag != yyok)                     \
      return yychk_flag;                        \
  } while (0)

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
#  define YYSTACKEXPANDABLE 1
#endif

#if YYSTACKEXPANDABLE
# define YY_RESERVE_GLRSTACK(Yystack)                   \
  do {                                                  \
    if (Yystack->yyspaceLeft < YYHEADROOM)              \
      yyexpandGLRStack (Yystack);                       \
  } while (0)
#else
# define YY_RESERVE_GLRSTACK(Yystack)                   \
  do {                                                  \
    if (Yystack->yyspaceLeft < YYHEADROOM)              \
      yyMemoryExhausted (Yystack);                      \
  } while (0)
#endif

/** State numbers. */
typedef int yy_state_t;

/** Rule numbers. */
typedef int yyRuleNum;

/** Item references. */
typedef short yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState {
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yysval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yy_state_t yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the last token produced by my symbol */
  YYPTRDIFF_T yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  nonterminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yysval;
  } yysemantics;
};

struct yyGLRStateSet {
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  YYPTRDIFF_T yysize;
  YYPTRDIFF_T yycapacity;
};

struct yySemanticOption {
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrState;


  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  YYPTRDIFF_T yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack (yyGLRStack* yystackp);
#endif

_Noreturn static void
yyFail (yyGLRStack* yystackp, const char* yymsg)
{
  if (yymsg != YY_NULLPTR)
    yyerror (yymsg);
  YYLONGJMP (yystackp->yyexception_buffer, 1);
}

_Noreturn static void
yyMemoryExhausted (yyGLRStack* yystackp)
{
  YYLONGJMP (yystackp->yyexception_buffer, 2);
}

/** Accessing symbol of state YYSTATE.  */
static inline yysymbol_kind_t
yy_accessing_symbol (yy_state_t yystate)
{
  return YY_CAST (yysymbol_kind_t, yystos[yystate]);
}

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TK_TAG", "TK_IDENT",
  "TK_CONST", "TK_DOTDOTDOT", "TK_LAMBDA", "TK_FROM", "TK_TO",
  "TK_SUCHTHAT", "TK_UMINUS", "TK_UPLUS", "TK_POW", "TK_LESS", "TK_LESSEQ",
  "TK_MORE", "TK_MOREEQ", "TK_NOTEQ", "TK_LAND", "TK_LOR", "TK_BAND",
  "TK_BOR", "TK_JOIN", "TK_DIFF", "TK_IF", "TK_THEN", "TK_ELSE", "TK_CHAR",
  "TK_SHORT", "TK_CLASS", "TK_SCOPE", "TK_INT", "TK_FLOAT", "TK_DOUBLE",
  "TK_SIGNED", "TK_UNSIGNED", "TK_COMPLEX", "TK_SEPARATOR", "TK_DIALOG",
  "TK_LSHIFT", "TK_RSHIFT", "','", "'@'", "'^'", "TK_EQ", "TK_PEQ",
  "TK_PNOTEQ", "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'~'", "':'",
  "'('", "'['", "TK_APPLICATION", "'?'", "'.'", "'='", "';'", "'{'", "'}'",
  "')'", "']'", "$accept", "select", "prhs", "main", "single_definition",
  "directive", "toplevel_definition", "$@1", "definition", "$@2",
  "params_plus_rhs", "$@3", "$@4", "$@5", "params", "body", "crhs", "$@6",
  "rhs", "locals", "optsemi", "deflist", "$@7", "$@8", "cexprlist", "expr",
  "lambda", "$@9", "list_expression", "$@10", "frompred_list", "generator",
  "frompred", "comma_list", "binop", "signed", "unsigned", "uop",
  "simple_pattern", "leaf_pattern", "complex_pattern", "list_pattern", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#if YYDEBUG

# ifndef YYFPRINTF
#  define YYFPRINTF fprintf
# endif

# define YY_FPRINTF                             \
  YY_IGNORE_USELESS_CAST_BEGIN YY_FPRINTF_

# define YY_FPRINTF_(Args)                      \
  do {                                          \
    YYFPRINTF Args;                             \
    YY_IGNORE_USELESS_CAST_END                  \
  } while (0)

# define YY_DPRINTF                             \
  YY_IGNORE_USELESS_CAST_BEGIN YY_DPRINTF_

# define YY_DPRINTF_(Args)                      \
  do {                                          \
    if (yydebug)                                \
      YYFPRINTF Args;                           \
    YY_IGNORE_USELESS_CAST_END                  \
  } while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif



/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                  \
  do {                                                                  \
    if (yydebug)                                                        \
      {                                                                 \
        YY_FPRINTF ((stderr, "%s ", Title));                            \
        yy_symbol_print (stderr, Kind, Value);        \
        YY_FPRINTF ((stderr, "\n"));                                    \
      }                                                                 \
  } while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

static void yypstack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
  YY_ATTRIBUTE_UNUSED;
static void yypdumpstack (yyGLRStack* yystackp)
  YY_ATTRIBUTE_UNUSED;

#else /* !YYDEBUG */

# define YY_DPRINTF(Args) do {} while (yyfalse)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)

#endif /* !YYDEBUG */

#ifndef yystrlen
# define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
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


/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain.  */
static void yyfillin (yyGLRStackItem *, int, int) YY_ATTRIBUTE_UNUSED;
static void
yyfillin (yyGLRStackItem *yyvsp, int yylow0, int yylow1)
{
  int i;
  yyGLRState *s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0-1; i >= yylow1; i -= 1)
    {
#if YYDEBUG
      yyvsp[i].yystate.yylrState = s->yylrState;
#endif
      yyvsp[i].yystate.yyresolved = s->yyresolved;
      if (s->yyresolved)
        yyvsp[i].yystate.yysemantics.yysval = s->yysemantics.yysval;
      else
        /* The effect of using yysval or yyloc (in an immediate rule) is
         * undefined.  */
        yyvsp[i].yystate.yysemantics.yyfirstVal = YY_NULLPTR;
      s = yyvsp[i].yystate.yypred = s->yypred;
    }
}


/** If yychar is empty, fetch the next token.  */
static inline yysymbol_kind_t
yygetToken (int *yycharp)
{
  yysymbol_kind_t yytoken;
  if (*yycharp == YYEMPTY)
    {
      YY_DPRINTF ((stderr, "Reading a token\n"));
      *yycharp = yylex ();
    }
  if (*yycharp <= YYEOF)
    {
      *yycharp = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YY_DPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (*yycharp);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }
  return yytoken;
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     YY_ATTRIBUTE_UNUSED;
static inline int
yyfill (yyGLRStackItem *yyvsp, int *yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
    {
      yyfillin (yyvsp, *yylow, yylow1);
      *yylow = yylow1;
    }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT.  */
static YYRESULTTAG
yyuserAction (yyRuleNum yyn, int yyrhslen, yyGLRStackItem* yyvsp,
              yyGLRStack* yystackp,
              YYSTYPE* yyvalp)
{
  yybool yynormal YY_ATTRIBUTE_UNUSED = yystackp->yysplitPoint == YY_NULLPTR;
  int yylow;
  YYUSE (yyvalp);
  YYUSE (yyrhslen);
# undef yyerrok
# define yyerrok (yystackp->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING() (yystackp->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, (N), yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)                                              \
  return yyerror (YY_("syntax error: cannot back up")),     \
         yyerrok, yyerr

  yylow = 1;
  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yysval;
  switch (yyn)
    {
  case 4: /* select: '*' params_plus_rhs optsemi  */
#line 194 "parse.y"
                                    {
		compile_check( current_compile );
	}
#line 1925 "parse.c"
    break;

  case 5: /* select: prhs  */
#line 197 "parse.y"
             {
		char buf[MAX_STRSIZE];

		current_compile->tree = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);

		/* Junk any old text.
		 */
		IM_FREE( current_compile->text );
		IM_FREE( current_compile->prhstext );
		IM_FREE( current_compile->rhstext );

		/* Set new text.
		 */
		IM_SETSTR( current_compile->rhstext, input_text( buf ) );

		compile_check( current_compile );
	}
#line 1947 "parse.c"
    break;

  case 6: /* prhs: TK_BAND expr  */
#line 217 "parse.y"
                     {
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
	}
#line 1955 "parse.c"
    break;

  case 7: /* prhs: '@' cexprlist  */
#line 220 "parse.y"
                      {
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
	}
#line 1963 "parse.c"
    break;

  case 10: /* single_definition: directive  */
#line 231 "parse.y"
                  {
		tool_position += 1;
	}
#line 1971 "parse.c"
    break;

  case 11: /* single_definition: toplevel_definition optsemi  */
#line 234 "parse.y"
                                    {
		tool_position += 1;
	}
#line 1979 "parse.c"
    break;

  case 12: /* directive: TK_SEPARATOR  */
#line 240 "parse.y"
                     {
		Tool *tool;

		if( !is_top( current_symbol ) )
			yyerror( _( "not top level" ) );

		tool = tool_new_sep( current_kit, tool_position );
		tool->lineno = input_state.lineno;

		input_reset();
	}
#line 1995 "parse.c"
    break;

  case 13: /* directive: TK_DIALOG TK_CONST TK_CONST  */
#line 251 "parse.y"
                                    {
		Tool *tool;

		if( !is_top( current_symbol ) )
			yyerror( _( "not top level" ) );

		/* Should have two strings.
		 */
		if( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_const).type != PARSE_CONST_STR || (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_const).type != PARSE_CONST_STR )
			yyerror( _( "not strings" ) );

		/* Add tool.
		 */
		tool = tool_new_dia( current_kit, tool_position, 
			(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_const).val.str, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_const).val.str );
		if( !tool )
			yyerror( error_get_sub() );
		tool->lineno = input_state.lineno;

		/* Cast away const here.
		 */
		tree_const_destroy( (ParseConst *) &(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_const) );
		tree_const_destroy( (ParseConst *) &(YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_const) );

		input_reset();
	}
#line 2026 "parse.c"
    break;

  case 14: /* $@1: %empty  */
#line 280 "parse.y"
        {
		last_top_lineno = input_state.lineno;
		scope_reset();
		current_compile = root_symbol->expr->compile;
	}
#line 2036 "parse.c"
    break;

  case 15: /* toplevel_definition: $@1 definition  */
#line 285 "parse.y"
                   {
		input_reset();
	}
#line 2044 "parse.c"
    break;

  case 16: /* $@2: %empty  */
#line 293 "parse.y"
                       {	
		Symbol *sym;

		/* Two forms: <name pattern-list rhs>, or <pattern rhs>.
		 * Enforce the no-args-to-pattern-assignment rule in the arg
		 * pattern parser.
		 */
		if( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node)->type == NODE_LEAF ) {
			const char *name = IOBJECT( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node)->leaf )->name;

			/* Make a new defining occurence.
			 */
			sym = symbol_new_defining( current_compile, name );

			(void) symbol_user_init( sym );
			(void) compile_new_local( sym->expr );
		}
		else {
			char name[256];

			/* We have <pattern rhs>. Make an anon symbol for this
			 * value, then the variables in the pattern become
			 * toplevels which access that.
			 */
			if( !compile_pattern_has_leaf( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) ) )
				yyerror( _( "left-hand-side pattern "
					"contains no identifiers" ) );
			im_snprintf( name, 256, "$$pattern_lhs%d",
				parse_object_id++ );
			sym = symbol_new_defining( current_compile, name );
			sym->generated = TRUE;
			(void) symbol_user_init( sym );
			(void) compile_new_local( sym->expr );
		}

		/* Note on the enclosing last_sym. Things like the program
		 * window use this to work out what sym to display after a
		 * parse. symbol_dispose() is careful to NULL this out.
		 */
		current_compile->last_sym = sym;

		/* Initialise symbol parsing variables. Save old current symbol,
		 * add new one.
		 */
		scope_push();
		current_symbol = sym;
		current_compile = sym->expr->compile;

		g_assert( !current_compile->param );
		g_assert( current_compile->nparam == 0 );

		/* Junk any old def text.
		 */
		IM_FREE( current_compile->text );
		IM_FREE( current_compile->prhstext );
		IM_FREE( current_compile->rhstext );
	}
#line 2106 "parse.c"
    break;

  case 17: /* definition: simple_pattern $@2 params_plus_rhs  */
#line 350 "parse.y"
                        {
		compile_check( current_compile );

		/* Link unresolved names into the outer scope.
		 */
		compile_resolve_names( current_compile, 
			compile_get_parent( current_compile ) );

		/* Is this the end of a top-level? Needs extra work to add to
		 * the enclosing toolkit etc.
		 */
		if( is_scope( symbol_get_parent( current_symbol ) ) ) 
			parse_toplevel_end( current_symbol );

		/* Is this a pattern definition? Expand the pattern to a
		 * set of access defs.
		 */
		if( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node)->type != NODE_LEAF ) {
			Compile *parent = compile_get_parent( current_compile );
			GSList *built_syms;

			built_syms = compile_pattern_lhs( parent, 
				current_symbol, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node) );

			if( is_scope( symbol_get_parent( current_symbol ) ) )
				slist_map( built_syms,
					(SListMapFn) parse_toplevel_end, NULL );
			slist_map( built_syms,
				(SListMapFn) parse_access_end, 
				current_symbol );

			g_slist_free( built_syms );
		}

		scope_pop();
	}
#line 2147 "parse.c"
    break;

  case 18: /* $@3: %empty  */
#line 391 "parse.y"
        {	
		input_push( 1 );

		/* We've already read the character past the end of the 
		 * identifier (that's why we know the identifier is over).
		 */
		input_back1();
	}
#line 2160 "parse.c"
    break;

  case 19: /* $@4: %empty  */
#line 399 "parse.y"
               {	
		input_push( 2 );
		input_backtoch( '=' );
	}
#line 2169 "parse.c"
    break;

  case 20: /* $@5: %empty  */
#line 403 "parse.y"
             {
		current_compile->tree = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
		g_assert( current_compile->tree );
		input_push( 4 );
	}
#line 2179 "parse.c"
    break;

  case 21: /* params_plus_rhs: $@3 params $@4 body $@5 locals  */
#line 408 "parse.y"
               {
		char buf[MAX_STRSIZE];

		input_pop();

		/* Save body text as rhstext.
		 */
		IM_SETSTR( current_compile->rhstext, input_text( buf ) );
		input_pop();

		/* Save params '=' body as prhstext.
		 */
		IM_SETSTR( current_compile->prhstext, input_text( buf ) );
		input_pop();

		/* Save full text of definition.
		 */
		IM_SETSTR( current_compile->text, input_text( buf ) );

#ifdef DEBUG
		printf( "%s->compile->text = \"%s\"\n",
			IOBJECT( current_compile->sym )->name, 
			current_compile->text );
		printf( "%s->compile->prhstext = \"%s\"\n",
			IOBJECT( current_compile->sym )->name, 
			current_compile->prhstext );
		printf( "%s->compile->rhstext = \"%s\"\n",
			IOBJECT( current_compile->sym )->name, 
			current_compile->rhstext );
#endif /*DEBUG*/
	}
#line 2215 "parse.c"
    break;

  case 23: /* params: params simple_pattern  */
#line 443 "parse.y"
                              {
		Symbol *sym;

		/* If the pattern is just an identifier, make it a direct
		 * parameter. Otherwise make an anon param and put the pattern
		 * in as a local with the same id.
		 *
		 *	fred [a] = 12;
		 *
		 * parses to:
		 *
		 *	fred $$arg42 = 12 { $$patt42 = [a]; }
		 * 
		 * A later pass creates the "a = $$arg42?0" definition.
		 */
		if( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node)->type == NODE_LEAF ) {
			const char *name = IOBJECT( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node)->leaf )->name;

			/* Make defining occurence.
			 */
			sym = symbol_new_defining( current_compile, name );
			(void) symbol_parameter_init( sym );
		}
		else {
			char name[256];

			im_snprintf( name, 256, "$$arg%d", parse_object_id );
			sym = symbol_new_defining( current_compile, name );
			sym->generated = TRUE;
			(void) symbol_parameter_init( sym );

			im_snprintf( name, 256, "$$patt%d", parse_object_id++ );
			sym = symbol_new_defining( current_compile, name );
			sym->generated = TRUE;
			(void) symbol_user_init( sym );
			(void) compile_new_local( sym->expr );
			sym->expr->compile->tree = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
		}
	}
#line 2259 "parse.c"
    break;

  case 24: /* body: '=' TK_CLASS crhs  */
#line 485 "parse.y"
                          {
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
	}
#line 2267 "parse.c"
    break;

  case 25: /* body: rhs  */
#line 488 "parse.y"
            {
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
	}
#line 2275 "parse.c"
    break;

  case 26: /* $@6: %empty  */
#line 494 "parse.y"
        {
		ParseNode *pn = tree_class_new( current_compile );

		input_push( 3 );
		scope_push();
		current_symbol = current_compile->super;
		current_compile = current_symbol->expr->compile;

		current_parsenode = pn;
	}
#line 2290 "parse.c"
    break;

  case 27: /* crhs: $@6 cexprlist  */
#line 504 "parse.y"
                  {
		Compile *parent = compile_get_parent( current_compile );
		char buf[MAX_STRSIZE];
		int len;

		(void) input_text( buf );

		/* Always read 1 char too many.
		 */
		if( (len = strlen( buf )) > 0 ) 
			buf[len - 1] = '\0';

		IM_SETSTR( current_compile->rhstext, buf );
		input_pop();
		current_compile->tree = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);

		if( current_compile->tree->elist )
			parent->has_super = TRUE;

		/* Do some checking.
		 */
		compile_check( current_compile );

		/* Link unresolved names.
		 */
		compile_resolve_names( current_compile, parent );

		scope_pop();

		((*yyvalp).yy_node) = current_parsenode;
		current_parsenode = NULL;
	}
#line 2327 "parse.c"
    break;

  case 28: /* rhs: '=' expr  */
#line 539 "parse.y"
                 { 	
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
	}
#line 2335 "parse.c"
    break;

  case 29: /* rhs: '=' expr ',' expr optsemi rhs  */
#line 542 "parse.y"
                                      { 	
		((*yyvalp).yy_node) = tree_ifelse_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2343 "parse.c"
    break;

  case 35: /* $@7: %empty  */
#line 559 "parse.y"
                   {
		input_pop();
		input_push( 5 );
	}
#line 2352 "parse.c"
    break;

  case 37: /* $@8: %empty  */
#line 564 "parse.y"
                           {
		input_pop();
		input_push( 6 );
	}
#line 2361 "parse.c"
    break;

  case 39: /* cexprlist: %empty  */
#line 572 "parse.y"
                    {
		((*yyvalp).yy_node) = tree_super_new( current_compile );
	}
#line 2369 "parse.c"
    break;

  case 40: /* cexprlist: cexprlist expr  */
#line 575 "parse.y"
                                            {
		((*yyvalp).yy_node) = tree_super_extend( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2377 "parse.c"
    break;

  case 41: /* expr: '(' expr ')'  */
#line 581 "parse.y"
                     { 
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node);
	}
#line 2385 "parse.c"
    break;

  case 42: /* expr: TK_CONST  */
#line 584 "parse.y"
                 {
		((*yyvalp).yy_node) = tree_const_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_const) );
	}
#line 2393 "parse.c"
    break;

  case 43: /* expr: TK_IDENT  */
#line 587 "parse.y"
                 {
		((*yyvalp).yy_node) = tree_leaf_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
		im_free( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
	}
#line 2402 "parse.c"
    break;

  case 44: /* expr: TK_TAG  */
#line 591 "parse.y"
               {
		((*yyvalp).yy_node) = tree_tag_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
		im_free( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
	}
#line 2411 "parse.c"
    break;

  case 45: /* expr: TK_SCOPE  */
#line 595 "parse.y"
                 {
		((*yyvalp).yy_node) = tree_leaf_new( current_compile, 
			IOBJECT( symbol_get_scope( current_symbol ) )->name );
	}
#line 2420 "parse.c"
    break;

  case 46: /* expr: TK_IF expr TK_THEN expr TK_ELSE expr  */
#line 599 "parse.y"
                                                         {
		((*yyvalp).yy_node) = tree_ifelse_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2428 "parse.c"
    break;

  case 47: /* expr: expr expr  */
#line 602 "parse.y"
                                       {
		((*yyvalp).yy_node) = tree_appl_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2436 "parse.c"
    break;

  case 49: /* expr: list_expression  */
#line 606 "parse.y"
                        {
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
	}
#line 2444 "parse.c"
    break;

  case 50: /* expr: '(' expr ',' expr ')'  */
#line 609 "parse.y"
                              {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_COMMA, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2452 "parse.c"
    break;

  case 53: /* $@9: %empty  */
#line 617 "parse.y"
                                           {
		char name[256];
		Symbol *sym;

		/* Make an anonymous symbol local to the current sym, compile
		 * the expr inside that.
		 */
		im_snprintf( name, 256, "$$lambda%d", parse_object_id++ );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );

		/* Initialise symbol parsing variables. Save old current symbol,
		 * add new one.
		 */
		scope_push();
		current_symbol = sym;
		current_compile = sym->expr->compile;

		/* Make the parameter.
		 */
		sym = symbol_new_defining( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
		symbol_parameter_init( sym );
		im_free( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
	}
#line 2483 "parse.c"
    break;

  case 54: /* lambda: TK_LAMBDA TK_IDENT $@9 expr  */
#line 643 "parse.y"
             {
		Symbol *sym;

		current_compile->tree = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);

		if( !compile_check( current_compile ) )
			yyerror( error_get_sub() );

		/* Link unresolved names in to the outer scope.
		 */
		compile_resolve_names( current_compile, 
			compile_get_parent( current_compile ) );

		/* The value of the expr is the anon we defined.
		 */
		sym = current_symbol;
		scope_pop();
		((*yyvalp).yy_node) = tree_leafsym_new( current_compile, sym );
	}
#line 2507 "parse.c"
    break;

  case 55: /* list_expression: '[' expr TK_DOTDOTDOT ']'  */
#line 665 "parse.y"
                                  {
		((*yyvalp).yy_node) = tree_generator_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), NULL, NULL );
	}
#line 2515 "parse.c"
    break;

  case 56: /* list_expression: '[' expr TK_DOTDOTDOT expr ']'  */
#line 668 "parse.y"
                                       {
		((*yyvalp).yy_node) = tree_generator_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.yy_node), NULL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2523 "parse.c"
    break;

  case 57: /* list_expression: '[' expr ',' expr TK_DOTDOTDOT ']'  */
#line 671 "parse.y"
                                           {
		((*yyvalp).yy_node) = tree_generator_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), NULL );
	}
#line 2531 "parse.c"
    break;

  case 58: /* list_expression: '[' expr ',' expr TK_DOTDOTDOT expr ']'  */
#line 674 "parse.y"
                                                {
		((*yyvalp).yy_node) = tree_generator_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-5)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2539 "parse.c"
    break;

  case 59: /* $@10: %empty  */
#line 677 "parse.y"
                             {
		char name[256];
		Symbol *sym;
		Compile *enclosing = current_compile;

		/* Make an anonymous symbol local to the current sym, copy
		 * the map expr inside that. 
		 */
		im_snprintf( name, 256, "$$lcomp%d", parse_object_id++ );
		sym = symbol_new_defining( current_compile, name );
		(void) symbol_user_init( sym );
		sym->generated = TRUE;
		(void) compile_new_local( sym->expr );

		/* Push a new scope.
		 */
		scope_push();
		current_symbol = sym;
		current_compile = sym->expr->compile;

		/* Somewhere to save the result expr. We have to copy the
		 * expr, as we want it to be bound in $$lcomp's context so
		 * that it can see the generators.
		 */
		sym = symbol_new_defining( current_compile, "$$result" );
		sym->generated = TRUE;
		sym->placeholder = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = compile_copy_tree( enclosing, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node), 
			sym->expr->compile );
	}
#line 2576 "parse.c"
    break;

  case 60: /* list_expression: '[' expr TK_SUCHTHAT $@10 generator frompred_list ']'  */
#line 709 "parse.y"
                                    {
		Symbol *sym;

		/* The map expr can refer to generator names. Resolve inwards
		 * so it links to the generators.
		 */
		compile_resolve_names( compile_get_parent( current_compile ),
			current_compile ); 

		/* Generate the code for the list comp.
		 */
		compile_lcomp( current_compile );

		compile_check( current_compile );

		/* Link unresolved names outwards.
		 */
		compile_resolve_names( current_compile, 
			compile_get_parent( current_compile ) );

		/* The value of the expr is the anon we defined.
		 */
		sym = current_symbol;
		scope_pop();
		((*yyvalp).yy_node) = tree_leafsym_new( current_compile, sym );
	}
#line 2607 "parse.c"
    break;

  case 61: /* list_expression: '[' comma_list ']'  */
#line 735 "parse.y"
                           {
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node);
	}
#line 2615 "parse.c"
    break;

  case 62: /* list_expression: '[' ']'  */
#line 738 "parse.y"
                {
		ParseConst elist;

		elist.type = PARSE_CONST_ELIST;
		((*yyvalp).yy_node) = tree_const_new( current_compile, elist );
	}
#line 2626 "parse.c"
    break;

  case 63: /* frompred_list: %empty  */
#line 747 "parse.y"
                    {
	}
#line 2633 "parse.c"
    break;

  case 64: /* frompred_list: frompred_list ';' frompred  */
#line 749 "parse.y"
                                   {
	}
#line 2640 "parse.c"
    break;

  case 65: /* generator: simple_pattern TK_FROM expr  */
#line 754 "parse.y"
                                   {
		char name[256];
		Symbol *sym;

		im_snprintf( name, 256, "$$pattern%d", parse_object_id );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		sym->placeholder = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node);

		im_snprintf( name, 256, "$$generator%d", parse_object_id++ );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		sym->placeholder = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
       }
#line 2665 "parse.c"
    break;

  case 67: /* frompred: expr  */
#line 778 "parse.y"
            {
		char name[256];
		Symbol *sym;

		im_snprintf( name, 256, "$$filter%d", parse_object_id++ );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		sym->placeholder = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node);
       }
#line 2682 "parse.c"
    break;

  case 68: /* comma_list: expr ',' comma_list  */
#line 793 "parse.y"
                            {
		((*yyvalp).yy_node) = tree_lconst_extend( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2690 "parse.c"
    break;

  case 69: /* comma_list: expr  */
#line 796 "parse.y"
             {
		((*yyvalp).yy_node) = tree_lconst_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2698 "parse.c"
    break;

  case 70: /* binop: expr '+' expr  */
#line 805 "parse.y"
                      {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_ADD, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2706 "parse.c"
    break;

  case 71: /* binop: expr ':' expr  */
#line 808 "parse.y"
                      {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_CONS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2714 "parse.c"
    break;

  case 72: /* binop: expr '-' expr  */
#line 811 "parse.y"
                      {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_SUB, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2722 "parse.c"
    break;

  case 73: /* binop: expr '?' expr  */
#line 814 "parse.y"
                      {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_SELECT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2730 "parse.c"
    break;

  case 74: /* binop: expr '/' expr  */
#line 817 "parse.y"
                      {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_DIV, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2738 "parse.c"
    break;

  case 75: /* binop: expr '*' expr  */
#line 820 "parse.y"
                      {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_MUL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2746 "parse.c"
    break;

  case 76: /* binop: expr '%' expr  */
#line 823 "parse.y"
                      {
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_REM, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2754 "parse.c"
    break;

  case 77: /* binop: expr TK_JOIN expr  */
#line 826 "parse.y"
                          {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_JOIN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2762 "parse.c"
    break;

  case 78: /* binop: expr TK_POW expr  */
#line 829 "parse.y"
                         {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_POW, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2770 "parse.c"
    break;

  case 79: /* binop: expr TK_LSHIFT expr  */
#line 832 "parse.y"
                            {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_LSHIFT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2778 "parse.c"
    break;

  case 80: /* binop: expr TK_RSHIFT expr  */
#line 835 "parse.y"
                            {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_RSHIFT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2786 "parse.c"
    break;

  case 81: /* binop: expr '^' expr  */
#line 838 "parse.y"
                      {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_EOR, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2794 "parse.c"
    break;

  case 82: /* binop: expr TK_LAND expr  */
#line 841 "parse.y"
                          {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_LAND, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2802 "parse.c"
    break;

  case 83: /* binop: expr TK_BAND expr  */
#line 844 "parse.y"
                          {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_BAND, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2810 "parse.c"
    break;

  case 84: /* binop: expr '@' expr  */
#line 847 "parse.y"
                      {	
		((*yyvalp).yy_node) = tree_compose_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2818 "parse.c"
    break;

  case 85: /* binop: expr TK_LOR expr  */
#line 850 "parse.y"
                         {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_LOR, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2826 "parse.c"
    break;

  case 86: /* binop: expr TK_BOR expr  */
#line 853 "parse.y"
                         {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_BOR, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2834 "parse.c"
    break;

  case 87: /* binop: expr TK_LESS expr  */
#line 856 "parse.y"
                          {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_LESS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2842 "parse.c"
    break;

  case 88: /* binop: expr TK_LESSEQ expr  */
#line 859 "parse.y"
                            {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_LESSEQ, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2850 "parse.c"
    break;

  case 89: /* binop: expr TK_MORE expr  */
#line 862 "parse.y"
                          {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_MORE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2858 "parse.c"
    break;

  case 90: /* binop: expr TK_MOREEQ expr  */
#line 865 "parse.y"
                            {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_MOREEQ, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2866 "parse.c"
    break;

  case 91: /* binop: expr TK_EQ expr  */
#line 868 "parse.y"
                        {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_EQ, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2874 "parse.c"
    break;

  case 92: /* binop: expr TK_NOTEQ expr  */
#line 871 "parse.y"
                           {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_NOTEQ, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2882 "parse.c"
    break;

  case 93: /* binop: expr TK_PEQ expr  */
#line 874 "parse.y"
                         {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_PEQ, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2890 "parse.c"
    break;

  case 94: /* binop: expr TK_PNOTEQ expr  */
#line 877 "parse.y"
                            {	
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_PNOTEQ, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2898 "parse.c"
    break;

  case 95: /* binop: expr '.' expr  */
#line 880 "parse.y"
                      {
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_DOT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2906 "parse.c"
    break;

  case 96: /* binop: expr TK_DIFF expr  */
#line 883 "parse.y"
                          {	
		ParseNode *pn1, *pn2;

		pn1 = tree_leaf_new( current_compile, "difference" );
		pn2 = tree_leaf_new( current_compile, "equal" );
		pn1 = tree_appl_new( current_compile, pn1, pn2 );
		pn1 = tree_appl_new( current_compile, pn1, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node) );
		pn1 = tree_appl_new( current_compile, pn1, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );

		((*yyvalp).yy_node) = pn1;
	}
#line 2922 "parse.c"
    break;

  case 97: /* binop: expr TK_TO expr  */
#line 894 "parse.y"
                        {
		ParseNode *pn;

		pn = tree_leaf_new( current_compile, "mknvpair" );
		pn = tree_appl_new( current_compile, pn, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node) );
		pn = tree_appl_new( current_compile, pn, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );

		((*yyvalp).yy_node) = pn;
	}
#line 2936 "parse.c"
    break;

  case 102: /* uop: '(' unsigned TK_CHAR ')' expr  */
#line 916 "parse.y"
                                                      {	
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CUCHAR, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2944 "parse.c"
    break;

  case 103: /* uop: '(' TK_SIGNED TK_CHAR ')' expr  */
#line 919 "parse.y"
                                                       {	
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CSCHAR, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2952 "parse.c"
    break;

  case 104: /* uop: '(' signed TK_SHORT ')' expr  */
#line 922 "parse.y"
                                                     {	
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CSSHORT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2960 "parse.c"
    break;

  case 105: /* uop: '(' TK_UNSIGNED TK_SHORT ')' expr  */
#line 925 "parse.y"
                                                          {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CUSHORT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2968 "parse.c"
    break;

  case 106: /* uop: '(' signed TK_INT ')' expr  */
#line 928 "parse.y"
                                                   {	
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CSINT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2976 "parse.c"
    break;

  case 107: /* uop: '(' TK_UNSIGNED TK_INT ')' expr  */
#line 931 "parse.y"
                                                        {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CUINT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2984 "parse.c"
    break;

  case 108: /* uop: '(' TK_FLOAT ')' expr  */
#line 934 "parse.y"
                                              {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CFLOAT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 2992 "parse.c"
    break;

  case 109: /* uop: '(' TK_DOUBLE ')' expr  */
#line 937 "parse.y"
                                               {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CDOUBLE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3000 "parse.c"
    break;

  case 110: /* uop: '(' TK_COMPLEX ')' expr  */
#line 940 "parse.y"
                                                {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CCOMPLEX, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3008 "parse.c"
    break;

  case 111: /* uop: '(' TK_DOUBLE TK_COMPLEX ')' expr  */
#line 943 "parse.y"
                                                          {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_CDCOMPLEX, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3016 "parse.c"
    break;

  case 112: /* uop: TK_UMINUS expr  */
#line 946 "parse.y"
                       {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_MINUS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3024 "parse.c"
    break;

  case 113: /* uop: '!' expr  */
#line 949 "parse.y"
                 {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_NEG, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3032 "parse.c"
    break;

  case 114: /* uop: '~' expr  */
#line 952 "parse.y"
                 {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_COMPLEMENT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3040 "parse.c"
    break;

  case 115: /* uop: TK_UPLUS expr  */
#line 955 "parse.y"
                      {
		((*yyvalp).yy_node) = tree_unop_new( current_compile, UN_PLUS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3048 "parse.c"
    break;

  case 117: /* simple_pattern: '(' leaf_pattern ',' leaf_pattern ')'  */
#line 964 "parse.y"
                                              {
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_COMMA, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3056 "parse.c"
    break;

  case 118: /* simple_pattern: simple_pattern ':' simple_pattern  */
#line 967 "parse.y"
                                          { 
		((*yyvalp).yy_node) = tree_binop_new( current_compile, BI_CONS, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3064 "parse.c"
    break;

  case 119: /* simple_pattern: '(' complex_pattern ')'  */
#line 970 "parse.y"
                                {
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node);
	}
#line 3072 "parse.c"
    break;

  case 120: /* simple_pattern: '[' list_pattern ']'  */
#line 973 "parse.y"
                             {
		((*yyvalp).yy_node) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_node);
	}
#line 3080 "parse.c"
    break;

  case 121: /* simple_pattern: '[' ']'  */
#line 976 "parse.y"
                {
		ParseConst elist;

		elist.type = PARSE_CONST_ELIST;
		((*yyvalp).yy_node) = tree_const_new( current_compile, elist );
	}
#line 3091 "parse.c"
    break;

  case 122: /* leaf_pattern: TK_IDENT  */
#line 987 "parse.y"
                 {
		((*yyvalp).yy_node) = tree_leaf_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
		im_free( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
	}
#line 3100 "parse.c"
    break;

  case 123: /* leaf_pattern: TK_CONST  */
#line 991 "parse.y"
                 {
		((*yyvalp).yy_node) = tree_const_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_const) );
	}
#line 3108 "parse.c"
    break;

  case 124: /* complex_pattern: TK_IDENT TK_IDENT  */
#line 999 "parse.y"
                          {
		((*yyvalp).yy_node) = tree_pattern_class_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_name),
			tree_leaf_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) ) );
		im_free( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yysval.yy_name) );
		im_free( (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_name) );
	}
#line 3119 "parse.c"
    break;

  case 126: /* list_pattern: complex_pattern ',' list_pattern  */
#line 1009 "parse.y"
                                         {
		((*yyvalp).yy_node) = tree_lconst_extend( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3127 "parse.c"
    break;

  case 127: /* list_pattern: complex_pattern  */
#line 1012 "parse.y"
                        {
		((*yyvalp).yy_node) = tree_lconst_new( current_compile, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yysval.yy_node) );
	}
#line 3135 "parse.c"
    break;


#line 3139 "parse.c"

      default: break;
    }

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
}


static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YYUSE (yy0);
  YYUSE (yy1);

  switch (yyn)
    {

      default: break;
    }
}

                              /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void
yydestroyGLRState (char const *yymsg, yyGLRState *yys)
{
  if (yys->yyresolved)
    yydestruct (yymsg, yy_accessing_symbol (yys->yylrState),
                &yys->yysemantics.yysval);
  else
    {
#if YYDEBUG
      if (yydebug)
        {
          if (yys->yysemantics.yyfirstVal)
            YY_FPRINTF ((stderr, "%s unresolved", yymsg));
          else
            YY_FPRINTF ((stderr, "%s incomplete", yymsg));
          YY_SYMBOL_PRINT ("", yy_accessing_symbol (yys->yylrState), YY_NULLPTR, &yys->yyloc);
        }
#endif

      if (yys->yysemantics.yyfirstVal)
        {
          yySemanticOption *yyoption = yys->yysemantics.yyfirstVal;
          yyGLRState *yyrh;
          int yyn;
          for (yyrh = yyoption->yystate, yyn = yyrhsLength (yyoption->yyrule);
               yyn > 0;
               yyrh = yyrh->yypred, yyn -= 1)
            yydestroyGLRState (yymsg, yyrh);
        }
    }
}

/** Left-hand-side symbol for rule #YYRULE.  */
static inline yysymbol_kind_t
yylhsNonterm (yyRuleNum yyrule)
{
  return YY_CAST (yysymbol_kind_t, yyr1[yyrule]);
}

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

/** True iff LR state YYSTATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yy_state_t yystate)
{
  return yypact_value_is_default (yypact[yystate]);
}

/** The default reduction for YYSTATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yy_state_t yystate)
{
  return yydefact[yystate];
}

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/** The action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *YYCONFLICTS to a pointer into yyconfl to a 0-terminated list
 *  of conflicting reductions.
 */
static inline int
yygetLRActions (yy_state_t yystate, yysymbol_kind_t yytoken, const short** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yytoken == YYSYMBOL_YYerror)
    {
      // This is the error token.
      *yyconflicts = yyconfl;
      return 0;
    }
  else if (yyisDefaultedState (yystate)
           || yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyconflicts = yyconfl;
      return -yydefact[yystate];
    }
  else if (! yytable_value_is_error (yytable[yyindex]))
    {
      *yyconflicts = yyconfl + yyconflp[yyindex];
      return yytable[yyindex];
    }
  else
    {
      *yyconflicts = yyconfl + yyconflp[yyindex];
      return 0;
    }
}

/** Compute post-reduction state.
 * \param yystate   the current state
 * \param yysym     the nonterminal to push on the stack
 */
static inline yy_state_t
yyLRgotoState (yy_state_t yystate, yysymbol_kind_t yysym)
{
  int yyr = yypgoto[yysym - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yysym - YYNTOKENS];
}

static inline yybool
yyisShiftAction (int yyaction)
{
  return 0 < yyaction;
}

static inline yybool
yyisErrorAction (int yyaction)
{
  return yyaction == 0;
}

                                /* GLRStates */

/** Return a fresh GLRStackItem in YYSTACKP.  The item is an LR state
 *  if YYISSTATE, and otherwise a semantic option.  Callers should call
 *  YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 *  headroom.  */

static inline yyGLRStackItem*
yynewGLRStackItem (yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  YYRULE on the semantic values in YYRHS to the list of
 *  alternative actions for YYSTATE.  Assumes that YYRHS comes from
 *  stack #YYK of *YYSTACKP. */
static void
yyaddDeferredAction (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyGLRState* yystate,
                     yyGLRState* yyrhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption =
    &yynewGLRStackItem (yystackp, yyfalse)->yyoption;
  YY_ASSERT (!yynewOption->yyisState);
  yynewOption->yystate = yyrhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
    {
      yynewOption->yyrawchar = yychar;
      yynewOption->yyval = yylval;
    }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK (yystackp);
}

                                /* GLRStacks */

/** Initialize YYSET to a singleton set containing an empty stack.  */
static yybool
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates
    = YY_CAST (yyGLRState**,
               YYMALLOC (YY_CAST (YYSIZE_T, yyset->yycapacity)
                         * sizeof yyset->yystates[0]));
  if (! yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = YY_NULLPTR;
  yyset->yylookaheadNeeds
    = YY_CAST (yybool*,
               YYMALLOC (YY_CAST (YYSIZE_T, yyset->yycapacity)
                         * sizeof yyset->yylookaheadNeeds[0]));
  if (! yyset->yylookaheadNeeds)
    {
      YYFREE (yyset->yystates);
      return yyfalse;
    }
  memset (yyset->yylookaheadNeeds,
          0,
          YY_CAST (YYSIZE_T, yyset->yycapacity) * sizeof yyset->yylookaheadNeeds[0]);
  return yytrue;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
  YYFREE (yyset->yylookaheadNeeds);
}

/** Initialize *YYSTACKP to a single empty stack, with total maximum
 *  capacity for all stacks of YYSIZE.  */
static yybool
yyinitGLRStack (yyGLRStack* yystackp, YYPTRDIFF_T yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems
    = YY_CAST (yyGLRStackItem*,
               YYMALLOC (YY_CAST (YYSIZE_T, yysize)
                         * sizeof yystackp->yynextFree[0]));
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = YY_NULLPTR;
  yystackp->yylastDeleted = YY_NULLPTR;
  return yyinitStateSet (&yystackp->yytops);
}


#if YYSTACKEXPANDABLE
# define YYRELOC(YYFROMITEMS, YYTOITEMS, YYX, YYTYPE)                   \
  &((YYTOITEMS)                                                         \
    - ((YYFROMITEMS) - YY_REINTERPRET_CAST (yyGLRStackItem*, (YYX))))->YYTYPE

/** If *YYSTACKP is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void
yyexpandGLRStack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem* yyp0, *yyp1;
  YYPTRDIFF_T yynewSize;
  YYPTRDIFF_T yyn;
  YYPTRDIFF_T yysize = yystackp->yynextFree - yystackp->yyitems;
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted (yystackp);
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems
    = YY_CAST (yyGLRStackItem*,
               YYMALLOC (YY_CAST (YYSIZE_T, yynewSize)
                         * sizeof yynewItems[0]));
  if (! yynewItems)
    yyMemoryExhausted (yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize;
       0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
    {
      *yyp1 = *yyp0;
      if (*YY_REINTERPRET_CAST (yybool *, yyp0))
        {
          yyGLRState* yys0 = &yyp0->yystate;
          yyGLRState* yys1 = &yyp1->yystate;
          if (yys0->yypred != YY_NULLPTR)
            yys1->yypred =
              YYRELOC (yyp0, yyp1, yys0->yypred, yystate);
          if (! yys0->yyresolved && yys0->yysemantics.yyfirstVal != YY_NULLPTR)
            yys1->yysemantics.yyfirstVal =
              YYRELOC (yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
        }
      else
        {
          yySemanticOption* yyv0 = &yyp0->yyoption;
          yySemanticOption* yyv1 = &yyp1->yyoption;
          if (yyv0->yystate != YY_NULLPTR)
            yyv1->yystate = YYRELOC (yyp0, yyp1, yyv0->yystate, yystate);
          if (yyv0->yynext != YY_NULLPTR)
            yyv1->yynext = YYRELOC (yyp0, yyp1, yyv0->yynext, yyoption);
        }
    }
  if (yystackp->yysplitPoint != YY_NULLPTR)
    yystackp->yysplitPoint = YYRELOC (yystackp->yyitems, yynewItems,
                                      yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != YY_NULLPTR)
      yystackp->yytops.yystates[yyn] =
        YYRELOC (yystackp->yyitems, yynewItems,
                 yystackp->yytops.yystates[yyn], yystate);
  YYFREE (yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void
yyfreeGLRStack (yyGLRStack* yystackp)
{
  YYFREE (yystackp->yyitems);
  yyfreeStateSet (&yystackp->yytops);
}

/** Assuming that YYS is a GLRState somewhere on *YYSTACKP, update the
 *  splitpoint of *YYSTACKP, if needed, so that it is at least as deep as
 *  YYS.  */
static inline void
yyupdateSplit (yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != YY_NULLPTR && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #YYK in *YYSTACKP.  */
static inline void
yymarkStackDeleted (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  if (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = YY_NULLPTR;
}

/** Undelete the last stack in *YYSTACKP that was marked as deleted.  Can
    only be done once after a deletion, and only when all other stacks have
    been deleted.  */
static void
yyundeleteLastStack (yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == YY_NULLPTR || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YY_DPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = YY_NULLPTR;
}

static inline void
yyremoveDeletes (yyGLRStack* yystackp)
{
  YYPTRDIFF_T yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
    {
      if (yystackp->yytops.yystates[yyi] == YY_NULLPTR)
        {
          if (yyi == yyj)
            YY_DPRINTF ((stderr, "Removing dead stacks.\n"));
          yystackp->yytops.yysize -= 1;
        }
      else
        {
          yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
          /* In the current implementation, it's unnecessary to copy
             yystackp->yytops.yylookaheadNeeds[yyi] since, after
             yyremoveDeletes returns, the parser immediately either enters
             deterministic operation or shifts a token.  However, it doesn't
             hurt, and the code might evolve to need it.  */
          yystackp->yytops.yylookaheadNeeds[yyj] =
            yystackp->yytops.yylookaheadNeeds[yyi];
          if (yyj != yyi)
            YY_DPRINTF ((stderr, "Rename stack %ld -> %ld.\n",
                        YY_CAST (long, yyi), YY_CAST (long, yyj)));
          yyj += 1;
        }
      yyi += 1;
    }
}

/** Shift to a new state on stack #YYK of *YYSTACKP, corresponding to LR
 * state YYLRSTATE, at input position YYPOSN, with (resolved) semantic
 * value *YYVALP and source location *YYLOCP.  */
static inline void
yyglrShift (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yy_state_t yylrState,
            YYPTRDIFF_T yyposn,
            YYSTYPE* yyvalp)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yysval = *yyvalp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK (yystackp);
}

/** Shift stack #YYK of *YYSTACKP, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static inline void
yyglrShiftDefer (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yy_state_t yylrState,
                 YYPTRDIFF_T yyposn, yyGLRState* yyrhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;
  YY_ASSERT (yynewState->yyisState);

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = YY_NULLPTR;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction (yystackp, yyk, yynewState, yyrhs, yyrule);
}

#if !YYDEBUG
# define YY_REDUCE_PRINT(Args)
#else
# define YY_REDUCE_PRINT(Args)          \
  do {                                  \
    if (yydebug)                        \
      yy_reduce_print Args;             \
  } while (0)

/*----------------------------------------------------------------------.
| Report that stack #YYK of *YYSTACKP is going to be reduced by YYRULE. |
`----------------------------------------------------------------------*/

static inline void
yy_reduce_print (yybool yynormal, yyGLRStackItem* yyvsp, YYPTRDIFF_T yyk,
                 yyRuleNum yyrule)
{
  int yynrhs = yyrhsLength (yyrule);
  int yyi;
  YY_FPRINTF ((stderr, "Reducing stack %ld by rule %d (line %d):\n",
               YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule]));
  if (! yynormal)
    yyfillin (yyvsp, 1, -yynrhs);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YY_FPRINTF ((stderr, "   $%d = ", yyi + 1));
      yy_symbol_print (stderr,
                       yy_accessing_symbol (yyvsp[yyi - yynrhs + 1].yystate.yylrState),
                       &yyvsp[yyi - yynrhs + 1].yystate.yysemantics.yysval                       );
      if (!yyvsp[yyi - yynrhs + 1].yystate.yyresolved)
        YY_FPRINTF ((stderr, " (unresolved)"));
      YY_FPRINTF ((stderr, "\n"));
    }
}
#endif

/** Pop the symbols consumed by reduction #YYRULE from the top of stack
 *  #YYK of *YYSTACKP, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *YYVALP to the resulting value,
 *  and *YYLOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyRuleNum yyrule,
            YYSTYPE* yyvalp)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystackp->yysplitPoint == YY_NULLPTR)
    {
      /* Standard special case: single stack.  */
      yyGLRStackItem* yyrhs
        = YY_REINTERPRET_CAST (yyGLRStackItem*, yystackp->yytops.yystates[yyk]);
      YY_ASSERT (yyk == 0);
      yystackp->yynextFree -= yynrhs;
      yystackp->yyspaceLeft += yynrhs;
      yystackp->yytops.yystates[0] = & yystackp->yynextFree[-1].yystate;
      YY_REDUCE_PRINT ((yytrue, yyrhs, yyk, yyrule));
      return yyuserAction (yyrule, yynrhs, yyrhs, yystackp,
                           yyvalp);
    }
  else
    {
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yyGLRState* yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
        = yystackp->yytops.yystates[yyk];
      int yyi;
      for (yyi = 0; yyi < yynrhs; yyi += 1)
        {
          yys = yys->yypred;
          YY_ASSERT (yys);
        }
      yyupdateSplit (yystackp, yys);
      yystackp->yytops.yystates[yyk] = yys;
      YY_REDUCE_PRINT ((yyfalse, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1, yyk, yyrule));
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
                           yystackp, yyvalp);
    }
}

/** Pop items off stack #YYK of *YYSTACKP according to grammar rule YYRULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with YYRULE and store its value with the
 *  newly pushed state, if YYFORCEEVAL or if *YYSTACKP is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #YYK from
 *  *YYSTACKP.  In this case, the semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyRuleNum yyrule,
             yybool yyforceEval)
{
  YYPTRDIFF_T yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == YY_NULLPTR)
    {
      YYSTYPE yysval;

      YYRESULTTAG yyflag = yydoAction (yystackp, yyk, yyrule, &yysval);
      if (yyflag == yyerr && yystackp->yysplitPoint != YY_NULLPTR)
        YY_DPRINTF ((stderr,
                     "Parse on stack %ld rejected by rule %d (line %d).\n",
                     YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule - 1]));
      if (yyflag != yyok)
        return yyflag;
      YY_SYMBOL_PRINT ("-> $$ =", yylhsNonterm (yyrule), &yysval, &yyloc);
      yyglrShift (yystackp, yyk,
                  yyLRgotoState (yystackp->yytops.yystates[yyk]->yylrState,
                                 yylhsNonterm (yyrule)),
                  yyposn, &yysval);
    }
  else
    {
      YYPTRDIFF_T yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystackp->yytops.yystates[yyk];
      yy_state_t yynewLRState;

      for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
           0 < yyn; yyn -= 1)
        {
          yys = yys->yypred;
          YY_ASSERT (yys);
        }
      yyupdateSplit (yystackp, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YY_DPRINTF ((stderr,
                   "Reduced stack %ld by rule %d (line %d); action deferred.  "
                   "Now in state %d.\n",
                   YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule - 1],
                   yynewLRState));
      for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
        if (yyi != yyk && yystackp->yytops.yystates[yyi] != YY_NULLPTR)
          {
            yyGLRState *yysplit = yystackp->yysplitPoint;
            yyGLRState *yyp = yystackp->yytops.yystates[yyi];
            while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
              {
                if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
                  {
                    yyaddDeferredAction (yystackp, yyk, yyp, yys0, yyrule);
                    yymarkStackDeleted (yystackp, yyk);
                    YY_DPRINTF ((stderr, "Merging stack %ld into stack %ld.\n",
                                 YY_CAST (long, yyk), YY_CAST (long, yyi)));
                    return yyok;
                  }
                yyp = yyp->yypred;
              }
          }
      yystackp->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static YYPTRDIFF_T
yysplitStack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  if (yystackp->yysplitPoint == YY_NULLPTR)
    {
      YY_ASSERT (yyk == 0);
      yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
    }
  if (yystackp->yytops.yycapacity <= yystackp->yytops.yysize)
    {
      YYPTRDIFF_T state_size = YYSIZEOF (yystackp->yytops.yystates[0]);
      YYPTRDIFF_T half_max_capacity = YYSIZE_MAXIMUM / 2 / state_size;
      if (half_max_capacity < yystackp->yytops.yycapacity)
        yyMemoryExhausted (yystackp);
      yystackp->yytops.yycapacity *= 2;

      {
        yyGLRState** yynewStates
          = YY_CAST (yyGLRState**,
                     YYREALLOC (yystackp->yytops.yystates,
                                (YY_CAST (YYSIZE_T, yystackp->yytops.yycapacity)
                                 * sizeof yynewStates[0])));
        if (yynewStates == YY_NULLPTR)
          yyMemoryExhausted (yystackp);
        yystackp->yytops.yystates = yynewStates;
      }

      {
        yybool* yynewLookaheadNeeds
          = YY_CAST (yybool*,
                     YYREALLOC (yystackp->yytops.yylookaheadNeeds,
                                (YY_CAST (YYSIZE_T, yystackp->yytops.yycapacity)
                                 * sizeof yynewLookaheadNeeds[0])));
        if (yynewLookaheadNeeds == YY_NULLPTR)
          yyMemoryExhausted (yystackp);
        yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
      }
    }
  yystackp->yytops.yystates[yystackp->yytops.yysize]
    = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize]
    = yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize - 1;
}

/** True iff YYY0 and YYY1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
static yybool
yyidenticalOptions (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
    {
      yyGLRState *yys0, *yys1;
      int yyn;
      for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
           yyn = yyrhsLength (yyy0->yyrule);
           yyn > 0;
           yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
        if (yys0->yyposn != yys1->yyposn)
          return yyfalse;
      return yytrue;
    }
  else
    return yyfalse;
}

/** Assuming identicalOptions (YYY0,YYY1), destructively merge the
 *  alternative semantic values for the RHS-symbols of YYY1 and YYY0.  */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       0 < yyn;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    {
      if (yys0 == yys1)
        break;
      else if (yys0->yyresolved)
        {
          yys1->yyresolved = yytrue;
          yys1->yysemantics.yysval = yys0->yysemantics.yysval;
        }
      else if (yys1->yyresolved)
        {
          yys0->yyresolved = yytrue;
          yys0->yysemantics.yysval = yys1->yysemantics.yysval;
        }
      else
        {
          yySemanticOption** yyz0p = &yys0->yysemantics.yyfirstVal;
          yySemanticOption* yyz1 = yys1->yysemantics.yyfirstVal;
          while (yytrue)
            {
              if (yyz1 == *yyz0p || yyz1 == YY_NULLPTR)
                break;
              else if (*yyz0p == YY_NULLPTR)
                {
                  *yyz0p = yyz1;
                  break;
                }
              else if (*yyz0p < yyz1)
                {
                  yySemanticOption* yyz = *yyz0p;
                  *yyz0p = yyz1;
                  yyz1 = yyz1->yynext;
                  (*yyz0p)->yynext = yyz;
                }
              yyz0p = &(*yyz0p)->yynext;
            }
          yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
        }
    }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
static int
yypreference (yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
    {
      if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
        return 0;
      else
        return 1;
    }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG yyresolveValue (yyGLRState* yys,
                                   yyGLRStack* yystackp);


/** Resolve the previous YYN states starting at and including state YYS
 *  on *YYSTACKP. If result != yyok, some states may have been left
 *  unresolved possibly with empty semantic option chains.  Regardless
 *  of whether result = yyok, each state has been left with consistent
 *  data so that yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn,
                 yyGLRStack* yystackp)
{
  if (0 < yyn)
    {
      YY_ASSERT (yys->yypred);
      YYCHK (yyresolveStates (yys->yypred, yyn-1, yystackp));
      if (! yys->yyresolved)
        YYCHK (yyresolveValue (yys, yystackp));
    }
  return yyok;
}

/** Resolve the states for the RHS of YYOPT on *YYSTACKP, perform its
 *  user action, and return the semantic value and location in *YYVALP
 *  and *YYLOCP.  Regardless of whether result = yyok, all RHS states
 *  have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystackp,
                 YYSTYPE* yyvalp)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs = yyrhsLength (yyopt->yyrule);
  YYRESULTTAG yyflag =
    yyresolveStates (yyopt->yystate, yynrhs, yystackp);
  if (yyflag != yyok)
    {
      yyGLRState *yys;
      for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
        yydestroyGLRState ("Cleanup: popping", yys);
      return yyflag;
    }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  {
    int yychar_current = yychar;
    YYSTYPE yylval_current = yylval;
    yychar = yyopt->yyrawchar;
    yylval = yyopt->yyval;
    yyflag = yyuserAction (yyopt->yyrule, yynrhs,
                           yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
                           yystackp, yyvalp);
    yychar = yychar_current;
    yylval = yylval_current;
  }
  return yyflag;
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == YY_NULLPTR)
    {
      yyleftmost_state.yyposn = 0;
      yystates[0] = &yyleftmost_state;
    }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YY_FPRINTF ((stderr, "%*s%s -> <Rule %d, empty>\n",
                 yyindent, "", yysymbol_name (yylhsNonterm (yyx->yyrule)),
                 yyx->yyrule - 1));
  else
    YY_FPRINTF ((stderr, "%*s%s -> <Rule %d, tokens %ld .. %ld>\n",
                 yyindent, "", yysymbol_name (yylhsNonterm (yyx->yyrule)),
                 yyx->yyrule - 1, YY_CAST (long, yys->yyposn + 1),
                 YY_CAST (long, yyx->yystate->yyposn)));
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
    {
      if (yystates[yyi]->yyresolved)
        {
          if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
            YY_FPRINTF ((stderr, "%*s%s <empty>\n", yyindent+2, "",
                         yysymbol_name (yy_accessing_symbol (yystates[yyi]->yylrState))));
          else
            YY_FPRINTF ((stderr, "%*s%s <tokens %ld .. %ld>\n", yyindent+2, "",
                         yysymbol_name (yy_accessing_symbol (yystates[yyi]->yylrState)),
                         YY_CAST (long, yystates[yyi-1]->yyposn + 1),
                         YY_CAST (long, yystates[yyi]->yyposn)));
        }
      else
        yyreportTree (yystates[yyi]->yysemantics.yyfirstVal, yyindent+2);
    }
}
#endif

static YYRESULTTAG
yyreportAmbiguity (yySemanticOption* yyx0,
                   yySemanticOption* yyx1)
{
  YYUSE (yyx0);
  YYUSE (yyx1);

#if YYDEBUG
  YY_FPRINTF ((stderr, "Ambiguity detected.\n"));
  YY_FPRINTF ((stderr, "Option 1,\n"));
  yyreportTree (yyx0, 2);
  YY_FPRINTF ((stderr, "\nOption 2,\n"));
  yyreportTree (yyx1, 2);
  YY_FPRINTF ((stderr, "\n"));
#endif

  yyerror (YY_("syntax is ambiguous"));
  return yyabort;
}

/** Resolve the ambiguity represented in state YYS in *YYSTACKP,
 *  perform the indicated actions, and set the semantic value of YYS.
 *  If result != yyok, the chain of semantic options in YYS has been
 *  cleared instead or it has been left unmodified except that
 *  redundant options may have been removed.  Regardless of whether
 *  result = yyok, YYS has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest = yyoptionList;
  yySemanticOption** yypp;
  yybool yymerge = yyfalse;
  YYSTYPE yysval;
  YYRESULTTAG yyflag;

  for (yypp = &yyoptionList->yynext; *yypp != YY_NULLPTR; )
    {
      yySemanticOption* yyp = *yypp;

      if (yyidenticalOptions (yybest, yyp))
        {
          yymergeOptionSets (yybest, yyp);
          *yypp = yyp->yynext;
        }
      else
        {
          switch (yypreference (yybest, yyp))
            {
            case 0:
              return yyreportAmbiguity (yybest, yyp);
              break;
            case 1:
              yymerge = yytrue;
              break;
            case 2:
              break;
            case 3:
              yybest = yyp;
              yymerge = yyfalse;
              break;
            default:
              /* This cannot happen so it is not worth a YY_ASSERT (yyfalse),
                 but some compilers complain if the default case is
                 omitted.  */
              break;
            }
          yypp = &yyp->yynext;
        }
    }

  if (yymerge)
    {
      yySemanticOption* yyp;
      int yyprec = yydprec[yybest->yyrule];
      yyflag = yyresolveAction (yybest, yystackp, &yysval);
      if (yyflag == yyok)
        for (yyp = yybest->yynext; yyp != YY_NULLPTR; yyp = yyp->yynext)
          {
            if (yyprec == yydprec[yyp->yyrule])
              {
                YYSTYPE yysval_other;
                yyflag = yyresolveAction (yyp, yystackp, &yysval_other);
                if (yyflag != yyok)
                  {
                    yydestruct ("Cleanup: discarding incompletely merged value for",
                                yy_accessing_symbol (yys->yylrState),
                                &yysval);
                    break;
                  }
                yyuserMerge (yymerger[yyp->yyrule], &yysval, &yysval_other);
              }
          }
    }
  else
    yyflag = yyresolveAction (yybest, yystackp, &yysval);

  if (yyflag == yyok)
    {
      yys->yyresolved = yytrue;
      yys->yysemantics.yysval = yysval;
    }
  else
    yys->yysemantics.yyfirstVal = YY_NULLPTR;
  return yyflag;
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystackp)
{
  if (yystackp->yysplitPoint != YY_NULLPTR)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystackp->yytops.yystates[0];
           yys != yystackp->yysplitPoint;
           yys = yys->yypred, yyn += 1)
        continue;
      YYCHK (yyresolveStates (yystackp->yytops.yystates[0], yyn, yystackp
                             ));
    }
  return yyok;
}

static void
yycompressStack (yyGLRStack* yystackp)
{
  yyGLRState* yyp, *yyq, *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == YY_NULLPTR)
    return;

  for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = YY_NULLPTR;
       yyp != yystackp->yysplitPoint;
       yyr = yyp, yyp = yyq, yyq = yyp->yypred)
    yyp->yypred = yyr;

  yystackp->yyspaceLeft += yystackp->yynextFree - yystackp->yyitems;
  yystackp->yynextFree = YY_REINTERPRET_CAST (yyGLRStackItem*, yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= yystackp->yynextFree - yystackp->yyitems;
  yystackp->yysplitPoint = YY_NULLPTR;
  yystackp->yylastDeleted = YY_NULLPTR;

  while (yyr != YY_NULLPTR)
    {
      yystackp->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
      yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
      yystackp->yynextFree += 1;
      yystackp->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystackp, YYPTRDIFF_T yyk,
                   YYPTRDIFF_T yyposn)
{
  while (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
    {
      yy_state_t yystate = yystackp->yytops.yystates[yyk]->yylrState;
      YY_DPRINTF ((stderr, "Stack %ld Entering state %d\n",
                   YY_CAST (long, yyk), yystate));

      YY_ASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
        {
          YYRESULTTAG yyflag;
          yyRuleNum yyrule = yydefaultAction (yystate);
          if (yyrule == 0)
            {
              YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              return yyok;
            }
          yyflag = yyglrReduce (yystackp, yyk, yyrule, yyimmediate[yyrule]);
          if (yyflag == yyerr)
            {
              YY_DPRINTF ((stderr,
                           "Stack %ld dies "
                           "(predicate failure or explicit user error).\n",
                           YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              return yyok;
            }
          if (yyflag != yyok)
            return yyflag;
        }
      else
        {
          yysymbol_kind_t yytoken = yygetToken (&yychar);
          const short* yyconflicts;
          const int yyaction = yygetLRActions (yystate, yytoken, &yyconflicts);
          yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;

          for (/* nothing */; *yyconflicts; yyconflicts += 1)
            {
              YYRESULTTAG yyflag;
              YYPTRDIFF_T yynewStack = yysplitStack (yystackp, yyk);
              YY_DPRINTF ((stderr, "Splitting off stack %ld from %ld.\n",
                           YY_CAST (long, yynewStack), YY_CAST (long, yyk)));
              yyflag = yyglrReduce (yystackp, yynewStack,
                                    *yyconflicts,
                                    yyimmediate[*yyconflicts]);
              if (yyflag == yyok)
                YYCHK (yyprocessOneStack (yystackp, yynewStack,
                                          yyposn));
              else if (yyflag == yyerr)
                {
                  YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yynewStack)));
                  yymarkStackDeleted (yystackp, yynewStack);
                }
              else
                return yyflag;
            }

          if (yyisShiftAction (yyaction))
            break;
          else if (yyisErrorAction (yyaction))
            {
              YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              break;
            }
          else
            {
              YYRESULTTAG yyflag = yyglrReduce (yystackp, yyk, -yyaction,
                                                yyimmediate[-yyaction]);
              if (yyflag == yyerr)
                {
                  YY_DPRINTF ((stderr,
                               "Stack %ld dies "
                               "(predicate failure or explicit user error).\n",
                               YY_CAST (long, yyk)));
                  yymarkStackDeleted (yystackp, yyk);
                  break;
                }
              else if (yyflag != yyok)
                return yyflag;
            }
        }
    }
  return yyok;
}

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYSTACKP, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  */
static int
yypcontext_expected_tokens (const yyGLRStack* yystackp,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[yystackp->yytops.yystates[0]->yylrState];
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

static int
yy_syntax_error_arguments (const yyGLRStack* yystackp,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  yysymbol_kind_t yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
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
       action, or user semantic action that manipulated yychar.b4_lac_if([
       In the first two cases, it might appear that the current syntax
       error should have been detected in the previous state when yy_lac
       was invoked.  However, at that time, there might have been a
       different syntax error that discarded a different initial context
       during error recovery, leaving behind the current lookahead.], [
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.])
  */
  if (yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yystackp,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}



static void
yyreportSyntaxError (yyGLRStack* yystackp)
{
  if (yystackp->yyerrState != 0)
    return;
  {
  yybool yysize_overflow = yyfalse;
  char* yymsg = YY_NULLPTR;
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount
    = yy_syntax_error_arguments (yystackp, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    yyMemoryExhausted (yystackp);

  switch (yycount)
    {
#define YYCASE_(N, S)                   \
      case N:                           \
        yyformat = S;                   \
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
        YYPTRDIFF_T yysz
          = yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (YYSIZE_MAXIMUM - yysize < yysz)
          yysize_overflow = yytrue;
        else
          yysize += yysz;
      }
  }

  if (!yysize_overflow)
    yymsg = YY_CAST (char *, YYMALLOC (YY_CAST (YYSIZE_T, yysize)));

  if (yymsg)
    {
      char *yyp = yymsg;
      int yyi = 0;
      while ((*yyp = *yyformat))
        {
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
      yyerror (yymsg);
      YYFREE (yymsg);
    }
  else
    {
      yyerror (YY_("syntax error"));
      yyMemoryExhausted (yystackp);
    }
  }
  yynerrs += 1;
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the lookahead.  */
static void
yyrecoverSyntaxError (yyGLRStack* yystackp)
{
  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (yytrue)
      {
        yysymbol_kind_t yytoken;
        int yyj;
        if (yychar == YYEOF)
          yyFail (yystackp, YY_NULLPTR);
        if (yychar != YYEMPTY)
          {
            yytoken = YYTRANSLATE (yychar);
            yydestruct ("Error: discarding",
                        yytoken, &yylval);
            yychar = YYEMPTY;
          }
        yytoken = yygetToken (&yychar);
        yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
        if (yypact_value_is_default (yyj))
          return;
        yyj += yytoken;
        if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
          {
            if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
              return;
          }
        else if (! yytable_value_is_error (yytable[yyj]))
          return;
      }

  /* Reduce to one stack.  */
  {
    YYPTRDIFF_T yyk;
    for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
      if (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
        break;
    if (yyk >= yystackp->yytops.yysize)
      yyFail (yystackp, YY_NULLPTR);
    for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
      yymarkStackDeleted (yystackp, yyk);
    yyremoveDeletes (yystackp);
    yycompressStack (yystackp);
  }

  /* Pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != YY_NULLPTR)
    {
      yyGLRState *yys = yystackp->yytops.yystates[0];
      int yyj = yypact[yys->yylrState];
      if (! yypact_value_is_default (yyj))
        {
          yyj += YYSYMBOL_YYerror;
          if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYSYMBOL_YYerror
              && yyisShiftAction (yytable[yyj]))
            {
              /* Shift the error token.  */
              int yyaction = yytable[yyj];
              YY_SYMBOL_PRINT ("Shifting", yy_accessing_symbol (yyaction),
                               &yylval, &yyerrloc);
              yyglrShift (yystackp, 0, yyaction,
                          yys->yyposn, &yylval);
              yys = yystackp->yytops.yystates[0];
              break;
            }
        }
      if (yys->yypred != YY_NULLPTR)
        yydestroyGLRState ("Error: popping", yys);
      yystackp->yytops.yystates[0] = yys->yypred;
      yystackp->yynextFree -= 1;
      yystackp->yyspaceLeft += 1;
    }
  if (yystackp->yytops.yystates[0] == YY_NULLPTR)
    yyFail (yystackp, YY_NULLPTR);
}

#define YYCHK1(YYE)                                                          \
  do {                                                                       \
    switch (YYE) {                                                           \
    case yyok:                                                               \
      break;                                                                 \
    case yyabort:                                                            \
      goto yyabortlab;                                                       \
    case yyaccept:                                                           \
      goto yyacceptlab;                                                      \
    case yyerr:                                                              \
      goto yyuser_error;                                                     \
    default:                                                                 \
      goto yybuglab;                                                         \
    }                                                                        \
  } while (0)

/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  YYPTRDIFF_T yyposn;

  YY_DPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;

  if (! yyinitGLRStack (yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP (yystack.yyexception_buffer))
    {
    case 0: break;
    case 1: goto yyabortlab;
    case 2: goto yyexhaustedlab;
    default: goto yybuglab;
    }
  yyglrShift (&yystack, 0, 0, 0, &yylval);
  yyposn = 0;

  while (yytrue)
    {
      /* For efficiency, we have two loops, the first of which is
         specialized to deterministic operation (single stack, no
         potential ambiguity).  */
      /* Standard mode. */
      while (yytrue)
        {
          yy_state_t yystate = yystack.yytops.yystates[0]->yylrState;
          YY_DPRINTF ((stderr, "Entering state %d\n", yystate));
          if (yystate == YYFINAL)
            goto yyacceptlab;
          if (yyisDefaultedState (yystate))
            {
              yyRuleNum yyrule = yydefaultAction (yystate);
              if (yyrule == 0)
                {
                  yyreportSyntaxError (&yystack);
                  goto yyuser_error;
                }
              YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue));
            }
          else
            {
              yysymbol_kind_t yytoken = yygetToken (&yychar);
              const short* yyconflicts;
              int yyaction = yygetLRActions (yystate, yytoken, &yyconflicts);
              if (*yyconflicts)
                /* Enter nondeterministic mode.  */
                break;
              if (yyisShiftAction (yyaction))
                {
                  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
                  yychar = YYEMPTY;
                  yyposn += 1;
                  yyglrShift (&yystack, 0, yyaction, yyposn, &yylval);
                  if (0 < yystack.yyerrState)
                    yystack.yyerrState -= 1;
                }
              else if (yyisErrorAction (yyaction))
                {
                  /* Issue an error message unless the scanner already
                     did. */
                  if (yychar != YYerror)
                    yyreportSyntaxError (&yystack);
                  goto yyuser_error;
                }
              else
                YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue));
            }
        }

      /* Nondeterministic mode. */
      while (yytrue)
        {
          yysymbol_kind_t yytoken_to_shift;
          YYPTRDIFF_T yys;

          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            yystackp->yytops.yylookaheadNeeds[yys] = yychar != YYEMPTY;

          /* yyprocessOneStack returns one of three things:

              - An error flag.  If the caller is yyprocessOneStack, it
                immediately returns as well.  When the caller is finally
                yyparse, it jumps to an error label via YYCHK1.

              - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
                (&yystack, yys), which sets the top state of yys to NULL.  Thus,
                yyparse's following invocation of yyremoveDeletes will remove
                the stack.

              - yyok, when ready to shift a token.

             Except in the first case, yyparse will invoke yyremoveDeletes and
             then shift the next token onto all remaining stacks.  This
             synchronization of the shift (that is, after all preceding
             reductions on all stacks) helps prevent double destructor calls
             on yylval in the event of memory exhaustion.  */

          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn));
          yyremoveDeletes (&yystack);
          if (yystack.yytops.yysize == 0)
            {
              yyundeleteLastStack (&yystack);
              if (yystack.yytops.yysize == 0)
                yyFail (&yystack, YY_("syntax error"));
              YYCHK1 (yyresolveStack (&yystack));
              YY_DPRINTF ((stderr, "Returning to deterministic operation.\n"));
              yyreportSyntaxError (&yystack);
              goto yyuser_error;
            }

          /* If any yyglrShift call fails, it will fail after shifting.  Thus,
             a copy of yylval will already be on stack 0 in the event of a
             failure in the following loop.  Thus, yychar is set to YYEMPTY
             before the loop to make sure the user destructor for yylval isn't
             called twice.  */
          yytoken_to_shift = YYTRANSLATE (yychar);
          yychar = YYEMPTY;
          yyposn += 1;
          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            {
              yy_state_t yystate = yystack.yytops.yystates[yys]->yylrState;
              const short* yyconflicts;
              int yyaction = yygetLRActions (yystate, yytoken_to_shift,
                              &yyconflicts);
              /* Note that yyconflicts were handled by yyprocessOneStack.  */
              YY_DPRINTF ((stderr, "On stack %ld, ", YY_CAST (long, yys)));
              YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, &yylval, &yylloc);
              yyglrShift (&yystack, yys, yyaction, yyposn,
                          &yylval);
              YY_DPRINTF ((stderr, "Stack %ld now in state #%d\n",
                           YY_CAST (long, yys),
                           yystack.yytops.yystates[yys]->yylrState));
            }

          if (yystack.yytops.yysize == 1)
            {
              YYCHK1 (yyresolveStack (&yystack));
              YY_DPRINTF ((stderr, "Returning to deterministic operation.\n"));
              yycompressStack (&yystack);
              break;
            }
        }
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }

 yyacceptlab:
  yyresult = 0;
  goto yyreturn;

 yybuglab:
  YY_ASSERT (yyfalse);
  goto yyabortlab;

 yyabortlab:
  yyresult = 1;
  goto yyreturn;

 yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;

 yyreturn:
  if (yychar != YYEMPTY)
    yydestruct ("Cleanup: discarding lookahead",
                YYTRANSLATE (yychar), &yylval);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
    {
      yyGLRState** yystates = yystack.yytops.yystates;
      if (yystates)
        {
          YYPTRDIFF_T yysize = yystack.yytops.yysize;
          YYPTRDIFF_T yyk;
          for (yyk = 0; yyk < yysize; yyk += 1)
            if (yystates[yyk])
              {
                while (yystates[yyk])
                  {
                    yyGLRState *yys = yystates[yyk];
                    if (yys->yypred != YY_NULLPTR)
                      yydestroyGLRState ("Cleanup: popping", yys);
                    yystates[yyk] = yys->yypred;
                    yystack.yynextFree -= 1;
                    yystack.yyspaceLeft += 1;
                  }
                break;
              }
        }
      yyfreeGLRStack (&yystack);
    }

  return yyresult;
}

/* DEBUGGING ONLY */
#if YYDEBUG
static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      YY_FPRINTF ((stderr, " -> "));
    }
  YY_FPRINTF ((stderr, "%d@%ld", yys->yylrState, YY_CAST (long, yys->yyposn)));
}

static void
yypstates (yyGLRState* yyst)
{
  if (yyst == YY_NULLPTR)
    YY_FPRINTF ((stderr, "<null>"));
  else
    yy_yypstack (yyst);
  YY_FPRINTF ((stderr, "\n"));
}

static void
yypstack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  yypstates (yystackp->yytops.yystates[yyk]);
}

static void
yypdumpstack (yyGLRStack* yystackp)
{
#define YYINDEX(YYX)                                                    \
  YY_CAST (long,                                                        \
           ((YYX)                                                       \
            ? YY_REINTERPRET_CAST (yyGLRStackItem*, (YYX)) - yystackp->yyitems \
            : -1))

  yyGLRStackItem* yyp;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
    {
      YY_FPRINTF ((stderr, "%3ld. ",
                   YY_CAST (long, yyp - yystackp->yyitems)));
      if (*YY_REINTERPRET_CAST (yybool *, yyp))
        {
          YY_ASSERT (yyp->yystate.yyisState);
          YY_ASSERT (yyp->yyoption.yyisState);
          YY_FPRINTF ((stderr, "Res: %d, LR State: %d, posn: %ld, pred: %ld",
                       yyp->yystate.yyresolved, yyp->yystate.yylrState,
                       YY_CAST (long, yyp->yystate.yyposn),
                       YYINDEX (yyp->yystate.yypred)));
          if (! yyp->yystate.yyresolved)
            YY_FPRINTF ((stderr, ", firstVal: %ld",
                         YYINDEX (yyp->yystate.yysemantics.yyfirstVal)));
        }
      else
        {
          YY_ASSERT (!yyp->yystate.yyisState);
          YY_ASSERT (!yyp->yyoption.yyisState);
          YY_FPRINTF ((stderr, "Option. rule: %d, state: %ld, next: %ld",
                       yyp->yyoption.yyrule - 1,
                       YYINDEX (yyp->yyoption.yystate),
                       YYINDEX (yyp->yyoption.yynext)));
        }
      YY_FPRINTF ((stderr, "\n"));
    }

  YY_FPRINTF ((stderr, "Tops:"));
  {
    YYPTRDIFF_T yyi;
    for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
      YY_FPRINTF ((stderr, "%ld: %ld; ", YY_CAST (long, yyi),
                   YYINDEX (yystackp->yytops.yystates[yyi])));
    YY_FPRINTF ((stderr, "\n"));
  }
#undef YYINDEX
}
#endif

#undef yylval
#undef yychar
#undef yynerrs




#line 1017 "parse.y"


/* Return point on syntax error.
 */
jmp_buf parse_error_point;

/* Text we've lexed.
 */
char lex_text_buffer[MAX_STRSIZE];
VipsBuf lex_text = VIPS_BUF_STATIC( lex_text_buffer );

/* State of input system.
 */
InputState input_state;

/* Defintions for the static decls at the top. We have to put the defs down
 * here to mkake sure they don't creep in to the generated parser.h.
 */

/* Actually, we can't make these static :-( since they are declared extern at
 * the top of the file.
 */
Symbol *current_symbol;
Symbol *root_symbol;
Compile *current_compile = NULL;
ParseNode *current_parsenode = NULL;
Toolkit *current_kit;
int tool_position;
int last_top_lineno;
Symbol *scope_stack_symbol[MAX_SSTACK];
Compile *scope_stack_compile[MAX_SSTACK];
int scope_sp = 0;
int parse_object_id = 0;

/* Here for errors in parse. 
 *
 * Bison calls yyerror with only a char* arg. This printf() version is called
 * from nip2 in a few places during parse.
 */
void
nip2yyerror( const char *sub, ... )
{
	va_list ap;
 	char buf[4096];

        va_start( ap, sub );
        (void) im_vsnprintf( buf, 4096, sub, ap );
        va_end( ap );

	error_top( _( "Parse error." ) );

	if( current_compile && current_compile->last_sym ) 
		error_sub( _( "Error in %s: %s" ), 
			IOBJECT(  current_compile->last_sym )->name, buf );
	else
		error_sub( _( "Error: %s" ), buf );

	longjmp( parse_error_point, -1 );
}

/* Bison calls this.
 */
void
yyerror( const char *msg )
{
	nip2yyerror( "%s", msg );
}

/* Attach yyinput to a file.
 */
void
attach_input_file( iOpenFile *of )
{
	InputState *is = &input_state;

#ifdef DEBUG
	printf( "attach_input_file: \"%s\"\n", of->fname );
#endif /*DEBUG*/

	/* Need to clear flex/bison's buffers in case we abandoned the
	 * previous parse. 
	 */
	yyrestart( NULL );

	is->of = of;
	is->str = NULL;
	is->strpos = NULL;
	is->bwp = 0;
	is->bspsp = 0;
	is->bsp[is->bspsp] = 0;
	is->lineno = 1;
	is->charno = 0;
	is->pcharno = 0;
	is->charpos = 0;
	is->oldchar = -1;

	/* Init text gatherer.
	 */
	vips_buf_rewind( &lex_text );
}

/* Attach yyinput to a string.
 */
void
attach_input_string( const char *str )
{
	InputState *is = &input_state;

#ifdef DEBUG
	printf( "attach_input_string: \"%s\"\n", str );
#endif /*DEBUG*/

	yyrestart( NULL );

	is->of = NULL;
	is->str = (char *) str;
	is->strpos = (char *) str;
	is->bwp = 0;
	is->bspsp = 0;
	is->bsp[is->bspsp] = 0;
	is->lineno = 1;
	is->charno = 0;
	is->pcharno = 0;
	is->charpos = 0;
	is->oldchar = -1;

	/* Init text gatherer.
	 */
	vips_buf_rewind( &lex_text );
}

/* Read a character from the input.
 */
int
ip_input( void ) 
{
	InputState *is = &input_state;
	int ch;

	if( is->oldchar >= 0 ) {
		/* From unget buffer.
		 */
		ch = is->oldchar;
		is->oldchar = -1;
	}
	else if( is->of ) {
		/* Input from file. 
		 */
		if( (ch = getc( is->of->fp )) == EOF )
			return( 0 );
	}
	else {
		/* Input from string. 
		 */
		if( (ch = *is->strpos) )
			is->strpos++;
		else
			/* No counts to update!
			 */
			return( 0 );
	}

	/* Update counts.
	 */
	if( ch == '\n' ) {
		is->lineno++;
		is->pcharno = is->charno + 1;
		is->charno = 0;
	}
	is->charno++;
	is->charpos++;

	/* Add this character to the characters we have accumulated for this
	 * definition.
	 */
	if( is->bwp >= MAX_STRSIZE )
		yyerror( _( "definition is too long" ) );
	if( is->bwp >= 0 )
		is->buf[is->bwp] = ch;
	is->bwp++;

	/* Add to lex text buffer.
	 */
	if( is->charno > 0 )
		vips_buf_appendc( &lex_text, ch );

#ifdef DEBUG_CHARACTER
	printf( "ip_input: returning '%c'\n", ch ); 
#endif /*DEBUG_CHARACTER*/

	return( ch );
}

/* Unget an input character.
 */
void
ip_unput( int ch )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "ip_unput: ungetting '%c'\n", ch ); 
#endif /*DEBUG_CHARACTER*/

	/* Is lex trying to unget the end-of-file marker? Do nothing if it is.
	 */
	if( !ch )
		return;

	if( is->of ) {
		if( ungetc( ch, is->of->fp ) == EOF )
			error( "unget buffer overflow" );
	}
	else 
		/* Save extra char here.
		 */
		is->oldchar = ch;

	/* Redo counts.
	 */
	if( ch == '\n' ) {
		is->lineno--;

		/* Restore previous charno.
		 */
		is->charno = is->pcharno;
		is->pcharno = 0;
	}
	is->charno--;
	is->charpos--;
	is->bwp--;

	/* Unget from lex text buffer.
	 */
	if( is->charno > 0 )
		vips_buf_removec( &lex_text, ch );
}

/* Test for end-of-input.
 */
gboolean
is_EOF( void )
{
	InputState *is = &input_state;

	if( is->of )
		return( feof( is->of->fp ) );
	else
		return( *is->str == '\0' );
}

/* Return the text we have accumulated for the current definition. Remove
 * leading and trailing whitespace and spare semicolons. out needs to be
 * MAX_STRSIZE.
 */
char *
input_text( char *out )
{
	InputState *is = &input_state;
	const char *buf = is->buf;

	int start = is->bsp[is->bspsp];
	int end = is->bwp;
	int len;
	int i;

	for( i = start; i < end && 
		(isspace( buf[i] ) || buf[i] == ';'); i++ )
		;
	start = i;
	for( i = end - 1; i > start && 
		(isspace( buf[i] ) || buf[i] == ';'); i-- )
		;
	end = i + 1;

	len = end - start;

	g_assert( len < MAX_STRSIZE - 1 );
	im_strncpy( out, buf + start, len + 1 );
	out[len] = '\0';

#ifdef DEBUG_CHARACTER
	printf( "input_text: level %d, returning \"%s\"\n", 
		is->bspsp, out );
#endif /*DEBUG_CHARACTER*/

	return( out );
}

/* Reset/push/pop input stacks.
 */
void
input_reset( void )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "input_reset:\n" );
#endif /*DEBUG_CHARACTER*/

	is->bwp = 0;
	is->bspsp = 0;
	is->bsp[0] = 0;
	vips_buf_rewind( &lex_text );
}

void
input_push( int n )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "input_push(%d): going to level %d, %d bytes into buffer\n", 
		n, is->bspsp + 1, is->bwp );

	{
		const int len = IM_MIN( is->bwp, 20 );
		int i;

		for( i = is->bwp - len; i < is->bwp; i++ )
			if( is->buf[i] == '\n' )
				printf( "@" );
			else if( is->buf[i] == ' ' || is->buf[i] == '\t' )
				printf( "_" );
			else
				printf( "%c", is->buf[i] );
		printf( "\n" );
		for( i = 0; i < len; i++ )
			printf( "-" );
		printf( "^\n" );
	}
#endif /*DEBUG_CHARACTER*/

	is->bspsp += 1;
	if( is->bspsp >= MAX_SSTACK )
		error( "bstack overflow" );

	is->bsp[is->bspsp] = is->bwp;
}

/* Yuk! We've just done an input_push() to try to grab the RHS of a 
 * definition ... unfortunately, due to token readahead, we've probably 
 * already read the start of the RHS.
 *
 * Back up the start point to just after the last ch character.
 */
void
input_backtoch( char ch )
{
	InputState *is = &input_state;
	int i;

	for( i = is->bsp[is->bspsp] - 1; i > 0 && is->buf[i] != ch; i-- )
		;

	if( is->buf[i] == ch )
		is->bsp[is->bspsp] = i + 1;
}

/* Move the last input_push() point back 1 character.
 */
void
input_back1( void )
{
	InputState *is = &input_state;

	if( is->bsp[is->bspsp] > 0 )
		is->bsp[is->bspsp] -= 1;
}

void
input_pop( void )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "input_pop: %d bytes into buffer\n", input_state.bwp );
#endif /*DEBUG_CHARACTER*/

	if( is->bspsp <= 0 )
		error( "bstack underflow" );

	is->bspsp--;
}

void
scope_push( void )
{
	if( scope_sp == MAX_SSTACK )
		error( "sstack overflow" );

	scope_stack_symbol[scope_sp] = current_symbol;
	scope_stack_compile[scope_sp] = current_compile;
	scope_sp += 1;
}

void
scope_pop( void )
{
	if( scope_sp <= 0 )
		error( "sstack underflow" );

	scope_sp -= 1;
	current_symbol = scope_stack_symbol[scope_sp];
	current_compile = scope_stack_compile[scope_sp];
}

/* Back to the outermost scope.
 */
void
scope_pop_all( void )
{
	if( scope_sp > 0 ) {
		scope_sp = 0;
		current_symbol = scope_stack_symbol[scope_sp];
		current_compile = scope_stack_compile[scope_sp];
	}
}

/* Reset/push/pop parser stacks. 
 */
void
scope_reset( void )
{
	scope_sp = 0;
}

/* End of top level parse. Fix up the symbol.
 */
void *
parse_toplevel_end( Symbol *sym )
{
	Tool *tool;

	tool = tool_new_sym( current_kit, tool_position, sym );
	tool->lineno = last_top_lineno;
	symbol_made( sym );

	return( NULL );
}

/* Built a pattern access definition. Set the various text fragments from the
 * def we are drived from.
 */
void *
parse_access_end( Symbol *sym, Symbol *main )
{
	IM_SETSTR( sym->expr->compile->rhstext, 
		main->expr->compile->rhstext ); 
	IM_SETSTR( sym->expr->compile->prhstext, 
		main->expr->compile->prhstext ); 
	IM_SETSTR( sym->expr->compile->text, 
		main->expr->compile->text ); 

	return( NULL );
}

/* Interface to parser. 
 */
static gboolean
parse_input( int ch, Symbol *sym, Toolkit *kit, int pos )
{
	current_kit = kit;
	current_symbol = sym;
	root_symbol = sym;
	tool_position = pos;

	scope_reset();
	input_reset();

	/* Signal start nonterminal to parser.
	 */
	ip_unput( ch );

	if( setjmp( parse_error_point ) ) {
		/* Restore current_compile.
		 */
		scope_pop_all();

		if( current_compile ) 
			compile_error_set( current_compile );

		return( FALSE );
	}
	yyparse();

	/* All ok.
	 */
	return( TRUE );
}

/* Parse the input into a set of symbols at a position in a kit. 
 * kit may be NULL for no kit. 
 */
gboolean
parse_toplevel( Toolkit *kit, int pos )
{
	gboolean result;

	current_compile = NULL;
	result = parse_input( ',', kit->kitg->root, kit, pos );
	iobject_changed( IOBJECT( kit ) );

	return( result );
}

/* Parse a single top-level definition.
 */
gboolean
parse_onedef( Toolkit *kit, int pos )
{
	gboolean result;

	current_compile = NULL;
	result = parse_input( '^', kit->kitg->root, kit, pos );
	iobject_changed( IOBJECT( kit ) );

	return( result );
}

/* Parse new text into "expr". If params is set, str should be "a b = a+b"
 * (ie. include params), if not, then just rhs (eg. "a+b").
 */
gboolean
parse_rhs( Expr *expr, ParseRhsSyntax syntax )
{
	static const char start_ch_table[] = {
		'&',		/* PARSE_RHS */
		'*',		/* PARSE_PARAMS */		
		'@'		/* PARSE_SUPER */
	};

	char start_ch = start_ch_table[(int) syntax];
	Compile *compile = compile_new_local( expr );

	current_compile = compile;
	if( !parse_input( start_ch, expr->sym, NULL, -1 ) ) {
		current_compile = NULL;
		return( FALSE );
	}
	current_compile = NULL;

#ifdef DEBUG
	printf( "parse_rhs:\n" );
	dump_tree( compile->tree );
#endif /*DEBUG*/

	/* Resolve any dynamic refs.
	 */
	expr_resolve( expr );

	/* Compile.
	 */
	if( compile_object( compile ) )
		return( FALSE );

	return( TRUE );
}

/* Free any stuff the lexer might have allocated. 
 */
void
free_lex( int yychar )
{
	switch( yychar ) {
	case TK_CONST:
		tree_const_destroy( &yylval.yy_const );
		break;

	case TK_IDENT:
	case TK_TAG:
		IM_FREE( yylval.yy_name );
		break;

	default:
		break;
	}
}

/* Do we have a string of the form "IDENT = .."? Use the lexer to look along
 * the string checking components, return the IDENT if we do, NULL otherwise.  
 */
char *
parse_test_define( void )
{
	extern int yylex( void );
	int yychar;
	char *ident;

	ident = NULL;

	if( setjmp( parse_error_point ) ) {
		/* Here for yyerror in lex. 
		 */
		IM_FREE( ident );

		return( NULL ); 
	}

	if( (yychar = yylex()) != TK_IDENT ) {
		free_lex( yychar );

		return( NULL ); 
	}
	ident = yylval.yy_name;

	if( (yychar = yylex()) != '=' ) {
		free_lex( yychar );
		IM_FREE( ident ); 

		return( NULL ); 
	}

	return( ident );
}

/* Do we have a string like "Workspaces.untitled.A1 = .."? Check for the
 * symbols as we see them, make the last one and return it. Used by --set.
 */
Symbol *
parse_set_symbol( void )
{
	int yychar;
	extern int yylex( void );
	Compile *compile = symbol_root->expr->compile;
	char *ident;
	Symbol *sym;

	ident = NULL;

	if( setjmp( parse_error_point ) ) {
		/* Here for yyerror in lex. 
		 */
		IM_FREE( ident );
		return( NULL ); 
	}

	do {
		if( (yychar = yylex()) != TK_IDENT && yychar != TK_TAG ) {
			free_lex( yychar );
			yyerror( _( "identifier expected" ) );
		}
		ident = yylval.yy_name;

		switch( (yychar = yylex()) ) {
		case '.':
			/* There's a dot, so we expect another identifier to 
			 * come. Look up this one and move to that context.
			 */
			if( !(sym = compile_lookup( compile, ident )) ) 
				nip2yyerror( _( "'%s' does not exist" ), 
					ident );
			if( !sym->expr || 
				!sym->expr->compile )
				nip2yyerror( _( "'%s' has no members" ), 
					ident );
			compile = sym->expr->compile;
			IM_FREE( ident );
			break;

		case '=':
			/* This is the final identifier: create the symbol in
			 * this context.
			 */
			sym = symbol_new_defining( compile, ident );
			IM_FREE( ident );
			break;

		default:
			free_lex( yychar );
			yyerror( _( "'.' or '=' expected" ) ); 
		}
	} while( yychar != '=' );

	return( sym );
}
