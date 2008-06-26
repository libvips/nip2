/* Expressions.
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

#define TYPE_EXPR (expr_get_type())
#define EXPR( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_EXPR, Expr ))
#define EXPR_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_EXPR, ExprClass))
#define IS_EXPR( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_EXPR ))
#define IS_EXPR_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_EXPR ))
#define EXPR_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_EXPR, ExprClass ))

/* What we track to parse and compile some text. Can have several 
 * of these for a symbol, all with different values.
 */
struct _Expr {
	/* We don't contain anything, but we are contained by Symbol, so we
	 * need to be an iContainer subclass.
	 */
	iContainer parent_object;

	Symbol *sym;		/* We are an expr for this symbol, scopewise */
	Row *row;		/* (optional) we have this display */

	Compile *compile;	/* Our compiled code */

	GSList *static_links;	/* Static LinkExprs which reference us */
	GSList *dynamic_links;	/* Dynamic LinkExprs which reference us */

	PElement root;		/* Pointer to value of this expr */

	/* Are we recorded as having an Imageinfo as a value? Use this to
	 * unlink us from the last ii we were linked to.
	 */
	Imageinfo *imageinfo;

	gboolean err;		/* TRUE if there is an error in this expr */
	char *error_top;
	char *error_sub;
};

typedef struct _ExprClass {
	iContainerClass parent_class;

	/* 

		new_value	expr has been recalced and root points to a
				new piece of graph

	 */

	void (*new_value)( Expr *expr );
} ExprClass;

extern GSList *expr_error_all;

void *expr_error_print( Expr *expr, BufInfo *buf );

typedef void *(*map_expr_fn)( Expr *, void *, void * );
Expr *expr_map_all( Expr *expr, map_expr_fn fn, void *a );

void *expr_name_print( Expr *expr );
void expr_name( Expr *expr, BufInfo *buf );

Expr *expr_get_parent( Expr *expr );
Expr *expr_get_root( Expr *expr );
Expr *expr_get_root_dynamic( Expr *expr );

GType expr_get_type( void );
void *expr_strip( Expr *expr );
Expr *expr_new( Symbol *sym );
Expr *expr_clone( Symbol *sym );

/* Set and clear error state.
 */
void *expr_error_set( Expr *expr );
void expr_error_clear( Expr *expr );
void expr_error_get( Expr *expr );

void expr_link_make( Expr *expr, Symbol *child );
void *expr_link_break( Expr *expr, Symbol *child );
void *expr_dirty( Expr *expr, int serial );
void *expr_dirty_intrans( Expr *expr, int serial );

void expr_tip( Expr *expr, BufInfo *buf );

void expr_new_value( Expr *expr );

void expr_resolve( Expr *expr );
