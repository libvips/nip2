/* Stuff to parse and compile text.
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

/* Maximum number of shared sections of code in a copy.
 */
#define MAX_RELOC (1000)

#define TYPE_COMPILE (compile_get_type())
#define COMPILE( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_COMPILE, Compile ))
#define COMPILE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_COMPILE, CompileClass))
#define IS_COMPILE( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_COMPILE ))
#define IS_COMPILE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_COMPILE ))
#define COMPILE_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_COMPILE, CompileClass ))

/* What we track to parse and compile some text. Our children are our locals.
 */
struct _Compile {
	iContainer parent_object;

	Symbol *sym;		/* We are part of this symbol, scopewise */

	GSList *exprs;		/* We are used by these expressions */

	gboolean is_klass;	/* True if this is a class */
	gboolean has_super;	/* True if has a super-class */

	char *text;		/* The original text */
	char *prhstext;		/* Parameters plus the RHS of the definition */
	char *rhstext;		/* Just the RHS of the definition */

	ParseNode *tree;	/* Parse tree we built */
	GSList *treefrag;	/* List of tree bits for easy freeing */
	Symbol *last_sym;	/* The last child we added in this context */

	int nparam;		/* Number of real parameters */
	GSList *param;		/* Pointers into locals for real params */
	int nsecret;		/* Number of secret parameters */
	GSList *secret;		/* Pointers into locals for secret params */
	Symbol *this;		/* If we are a class, the "this" local */
	Symbol *super;		/* If we are a class, the "super" local */
	GSList *children;	/* Symbols which we directly refer to */

	Element base; 		/* Base of compiled code */
	Heap *heap;		/* Heap containing compiled code */
	GSList *statics;	/* Static strings we built */
};

typedef struct _CompileClass {
	iContainerClass parent_class;

	/* My methods.
	 */
} CompileClass;

Compile *compile_get_parent( Compile *compile );
void *compile_name_print( Compile *compile );
void compile_name( Compile *compile, VipsBuf *buf );

typedef void *(*map_compile_fn)( Compile *, void * );
Compile *compile_map_all( Compile *compile, map_compile_fn fn, void *a );

Symbol *compile_lookup( Compile *compile, const char *name );

void compile_link_make( Compile *compile, Symbol *child );
void *compile_link_break( Compile *compile, Symbol *child );

GType compile_get_type( void );

void *compile_expr_link_break( Compile *compile, Expr *expr );
void *compile_expr_link_break_rev( Expr *expr, Compile *compile );
void compile_expr_link_make( Compile *compile, Expr *expr );

Compile *compile_new( Expr *expr );
Compile *compile_new_toplevel( Expr *expr );
Compile *compile_new_local( Expr *expr );

void *compile_object( Compile *compile );
void *compile_toolkit( Toolkit *kit );

void compile_error_set( Compile *compile );
gboolean compile_check( Compile *compile );

void compile_resolve_names( Compile *inner, Compile *outer );
Symbol *compile_resolve_top( Symbol *sym );
void compile_resolve_dynamic( Compile *tab, Compile *context );

Symbol *compile_get_member( Compile *compile, const char *name );
const char *compile_get_member_string( Compile *compile, const char *name );

ParseNode *compile_copy_tree( Compile *fromscope, ParseNode *tree,
	Compile *toscope );

void compile_lcomp( Compile *compile );

GSList *compile_pattern_lhs( Compile *compile, Symbol *sym, ParseNode *node );
gboolean compile_pattern_has_leaf( ParseNode *node );
gboolean compile_pattern_has_args( Compile *compile );
