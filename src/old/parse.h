/* A Bison parser, made by GNU Bison 3.7.  */

/* Skeleton interface for Bison GLR parsers in C

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

#ifndef YY_YY_PARSE_H_INCLUDED
# define YY_YY_PARSE_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TK_TAG = 258,                  /* TK_TAG  */
    TK_IDENT = 259,                /* TK_IDENT  */
    TK_CONST = 260,                /* TK_CONST  */
    TK_DOTDOTDOT = 261,            /* TK_DOTDOTDOT  */
    TK_LAMBDA = 262,               /* TK_LAMBDA  */
    TK_FROM = 263,                 /* TK_FROM  */
    TK_TO = 264,                   /* TK_TO  */
    TK_SUCHTHAT = 265,             /* TK_SUCHTHAT  */
    TK_UMINUS = 266,               /* TK_UMINUS  */
    TK_UPLUS = 267,                /* TK_UPLUS  */
    TK_POW = 268,                  /* TK_POW  */
    TK_LESS = 269,                 /* TK_LESS  */
    TK_LESSEQ = 270,               /* TK_LESSEQ  */
    TK_MORE = 271,                 /* TK_MORE  */
    TK_MOREEQ = 272,               /* TK_MOREEQ  */
    TK_NOTEQ = 273,                /* TK_NOTEQ  */
    TK_LAND = 274,                 /* TK_LAND  */
    TK_LOR = 275,                  /* TK_LOR  */
    TK_BAND = 276,                 /* TK_BAND  */
    TK_BOR = 277,                  /* TK_BOR  */
    TK_JOIN = 278,                 /* TK_JOIN  */
    TK_DIFF = 279,                 /* TK_DIFF  */
    TK_IF = 280,                   /* TK_IF  */
    TK_THEN = 281,                 /* TK_THEN  */
    TK_ELSE = 282,                 /* TK_ELSE  */
    TK_CHAR = 283,                 /* TK_CHAR  */
    TK_SHORT = 284,                /* TK_SHORT  */
    TK_CLASS = 285,                /* TK_CLASS  */
    TK_SCOPE = 286,                /* TK_SCOPE  */
    TK_INT = 287,                  /* TK_INT  */
    TK_FLOAT = 288,                /* TK_FLOAT  */
    TK_DOUBLE = 289,               /* TK_DOUBLE  */
    TK_SIGNED = 290,               /* TK_SIGNED  */
    TK_UNSIGNED = 291,             /* TK_UNSIGNED  */
    TK_COMPLEX = 292,              /* TK_COMPLEX  */
    TK_SEPARATOR = 293,            /* TK_SEPARATOR  */
    TK_DIALOG = 294,               /* TK_DIALOG  */
    TK_LSHIFT = 295,               /* TK_LSHIFT  */
    TK_RSHIFT = 296,               /* TK_RSHIFT  */
    TK_EQ = 297,                   /* TK_EQ  */
    TK_PEQ = 298,                  /* TK_PEQ  */
    TK_PNOTEQ = 299,               /* TK_PNOTEQ  */
    TK_APPLICATION = 300           /* TK_APPLICATION  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 112 "parse.y"

	struct sym_table *yy_symtab;
	ParseNode *yy_node;
	char *yy_name;
	ParseConst yy_const;
	UnOp yy_uop;
	BinOp yy_binop;

#line 113 "parse.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSE_H_INCLUDED  */
