/* Declarations for the tree builder.
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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

/* The kinds of nodes we can have in a parse tree. 
 */
typedef enum {
	NODE_NONE,		/* Empty */
	NODE_APPLY,		/* A function application */
	NODE_BINOP,		/* A binary operator */
	NODE_UOP,		/* A unary operator */
	NODE_LEAF,		/* A leaf .. a symbol of some sort */
	NODE_CLASS,		/* A class */
	NODE_TAG,		/* A tag .. rhs of '.' */
	NODE_CONST,		/* A constant */
	NODE_GENERATOR,		/* A list generator */
	NODE_COMPOSE,		/* Function composition */
	NODE_SUPER,		/* Superclass constructor */
	NODE_PATTERN,		/* A pattern */
	NODE_PATTERN_CLASS,	/* A class pattern match */
	NODE_LISTCONST		/* A list expression "[1, 2, 3]" */
} NodeTypes;

/* Binary operators.

	Order important! Keep changes in step with operator_table[] in
	action.c

 */
typedef enum {
	BI_NONE = 0,		/* Nothing */
	BI_ADD,			/* Addition and subtraction */
	BI_SUB,
	BI_REM,			/* Remainder after division */
	BI_POW,			/* Raise to power */
	BI_SELECT,		/* Select a channel/subscript */
	BI_LSHIFT,		/* Shift left */
	BI_RSHIFT,		/* Shift right */
	BI_DIV,			/* Divide by a constant */
	BI_JOIN,		/* Join of two images */
	BI_DOT,			/* Projection operator */
	BI_COMMA,		/* Form complex number */
	BI_MUL,			/* Mult by a constant */
	BI_LAND,		/* Logical and */
	BI_LOR,			/* Logical or */
	BI_BAND,		/* Bitwise and */
	BI_BOR,			/* Bitwise or */
	BI_EOR,			/* Either - or */
	BI_EQ,			/* Equality */
	BI_NOTEQ,
	BI_PEQ,			/* Pointer equality */
	BI_PNOTEQ,
	BI_LESS,		/* Relational ops */
	BI_LESSEQ,
	BI_MORE,		
	BI_MOREEQ,
	BI_IF,			/* if-then-else */
	BI_CONS			/* List cons ... has to be last, see below */
} BinOp;

/* Unary operators.

	Order important! Keep changes in step with operator_table[] in
	action.c

 */
typedef enum {
	UN_NONE = BI_CONS + 1,	/* Nothing */
	UN_CSCHAR,		/* Convert to signed char */
	UN_CUCHAR,		/* Convert to unsigned char */
	UN_CSSHORT,		/* Convert to signed short */
	UN_CUSHORT,		/* Convert to unsigned short */
	UN_CSINT,		/* Convert to signed int */
	UN_CUINT,		/* Convert to unsigned int */
	UN_CFLOAT,		/* Convert to signed float */
	UN_CDOUBLE,		/* Convert to signed double */
	UN_CCOMPLEX,		/* Convert to complex */
	UN_CDCOMPLEX,		/* Convert to double complex */
	UN_MINUS,		/* Unary minus */
	UN_NEG,			/* Logical negation */
	UN_COMPLEMENT,		/* 1s complement */
	UN_PLUS			/* Unary plus */
} UnOp;

/* The sorts of constants we can have in expressions. 
 */
typedef enum {
	PARSE_CONST_NONE,
	PARSE_CONST_STR,
	PARSE_CONST_BOOL,
	PARSE_CONST_NUM,
	PARSE_CONST_COMPLEX,		/* Eg. 12j == (0, 12) */
	PARSE_CONST_CHAR,
	PARSE_CONST_ELIST		/* Empty list [] */
} ParseConstTypes;

/* Constants in expressions. 
 */
struct _ParseConst {
	ParseConstTypes type;
	union {
		double num;
		char *str;
		gboolean bool;
		int ch;
	} val;
};

/* A parse tree node.
 */
struct _ParseNode {
	/* Compiled in here.
	 */
	Compile *compile;

	NodeTypes type;

	/* Bundle for node types with up to two arguments.
	 */
	BinOp biop;
	UnOp uop;
	ParseNode *arg1;
	ParseNode *arg2;

	/* Just for generators, eg. [a, b .. c]
	 */
	ParseNode *arg3;

	/* A symbol reference.
	 */
	Symbol *leaf;

	/* A class.
	 */
	Compile *klass;

	/* Expression list ... super constructor plus args, or list constant.
	 */
	GSList *elist;

	/* A tag.
	 */
	char *tag;

	/* A constant.
	 */
	ParseConst con;

	/* A class pattern.
	 */
	char *class_name;
	char *bind_to;
};

void tree_const_destroy( ParseConst *pc );

ParseNode *tree_binop_new( Compile *compile, 
	BinOp op, ParseNode *l, ParseNode *r );
ParseNode *tree_generator_new( Compile *compile, 
	ParseNode *s, ParseNode *i, ParseNode *f );
ParseNode *tree_lconst_new( Compile *compile, ParseNode *s );
ParseNode *tree_lconst_extend( Compile *compile, ParseNode *s, ParseNode *n );
ParseNode *tree_super_new( Compile *compile );
ParseNode *tree_super_extend( Compile *compile, ParseNode *base, ParseNode *n );
ParseNode *tree_ifelse_new( Compile *compile, 
	ParseNode *c, ParseNode *t, ParseNode *e );
ParseNode *tree_appl_new( Compile *compile, ParseNode *l, ParseNode *r );
ParseNode *tree_tag_new( Compile *compile, const char *r );
ParseNode *tree_unop_new( Compile *compile, UnOp op, ParseNode *a );
ParseNode *tree_leaf_new( Compile *compile, const char *name );
ParseNode *tree_leafsym_new( Compile *compile, Symbol *sym );
ParseNode *tree_const_new( Compile *compile, ParseConst n );
ParseNode *tree_class_new( Compile *compile );
ParseNode *tree_compose_new( Compile *compile, ParseNode *f, ParseNode *g );
ParseNode *tree_pattern_new( Compile *compile );
ParseNode *tree_pattern_class_new( Compile *compile, 
	const char *class_name, const char *bind_to );

void *tree_node_destroy( ParseNode *n );

typedef ParseNode *(*tree_map_fn)( Compile *, ParseNode *, void *, void * );
ParseNode *tree_map( Compile *compile, 
	tree_map_fn fn, ParseNode *node, void *a, void *b );

/* Copy a tree into a new context.
 */
ParseNode *tree_copy( Compile *compile, ParseNode *node );
