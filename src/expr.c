/* Expressions!
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

/* Trace error_set()/_clear().
#define DEBUG_ERROR
 */

/* Trace expr_clone() 
#define DEBUG_CLONE
 */

/*
#define DEBUG
 */

#include "ip.h"

/* Our signals. 
 */
enum {
	SIG_NEW_VALUE,		/* new value for root */
	SIG_LAST
};

static iContainerClass *parent_class = NULL;

static guint expr_signals[SIG_LAST] = { 0 };

/* Set of expressions containing errors.
 */
GSList *expr_error_all = NULL;

void *
expr_error_print( Expr *expr, VipsBuf *buf )
{
	g_assert( expr->err );

	vips_buf_appendf( buf, _( "error in \"%s\"" ), 
		IOBJECT( expr->sym )->name );
	if( expr->sym->tool ) 
		tool_error( expr->sym->tool, buf );
	else if( expr->row ) {
		Workspace *ws = expr->row->ws;
		Workspacegroup *wsg = workspace_get_workspacegroup( ws );

		vips_buf_appendf( buf, " (" );
		row_qualified_name( expr->row, buf );
		if( FILEMODEL( wsg )->filename )
			vips_buf_appendf( buf, " - %s", 
				FILEMODEL( wsg )->filename );
		vips_buf_appendf( buf, ")" );
	}

	/* Don't show error_top, it's just a summary of error_sub.
	 */
	vips_buf_appendf( buf, ": %s\n", expr->error_sub );

	return( NULL );
}

static Expr *
expr_map_all_sub( Symbol *sym, map_expr_fn fn, void *a )
{
	if( !sym->expr )
		return( NULL );
	else 
		return( expr_map_all( sym->expr, fn, a ) );
}

/* Apply a function to a expr ... and any local exprs.
 */
Expr *
expr_map_all( Expr *expr, map_expr_fn fn, void *a )
{
	Expr *res;

	/* Apply to this expr.
	 */
	if( (res = fn( expr, a, NULL )) )
		return( res );

	/* And over any locals.
	 */
	if( expr->compile && (res = (Expr *) 
		icontainer_map( ICONTAINER( expr->compile ), 
			(icontainer_map_fn) expr_map_all_sub, 
			(void *) fn, a )) )
		return( res );

	return( NULL );
}

void *
expr_name_print( Expr *expr )
{
	printf( "expr(%p) ", expr );
	symbol_name_print( expr->sym );

	if( expr->row ) {
		printf( "(row " );
		row_name_print( expr->row );
		printf( ") " );
	}

	return( NULL );
}

void
expr_name( Expr *expr, VipsBuf *buf )
{
	if( expr->row ) 
		row_qualified_name( expr->row, buf );
	else
		symbol_qualified_name( expr->sym, buf );
}

Expr *
expr_get_parent( Expr *expr )
{
	Symbol *sym_parent = symbol_get_parent( expr->sym );

	if( !sym_parent )
		return( NULL );

	return( sym_parent->expr );
}

/* Find the enclosing expr in the dynamic scope hierarchy.
 */
static Expr *
expr_get_parent_dynamic( Expr *expr )
{
	Row *row;

	if( !expr->row )
		return( expr_get_parent( expr ) );
	else if( (row = HEAPMODEL( expr->row )->row) )
		/* Enclosing row expr.
		 */
		return( row->expr );
	else {
		/* Enclosing workspace expr.
		 */
		Workspace *ws = expr->row->top_col->ws;

		return( ws->sym->expr );
	}
}

/* Look back up to find the root expr.
 */
Expr *
expr_get_root( Expr *expr )
{
	if( is_top( expr->sym ) )
		return( expr );
	else
		return( expr_get_root( expr_get_parent( expr ) ) );
}

/* Look back up to find the root expr using the dynamic hierarchy (if it's
 * there).
 */
Expr *
expr_get_root_dynamic( Expr *expr )
{
	Expr *parent;

	if( is_top( expr->sym ) )
		return( expr );
	else if( expr->row && expr->row->top_row && expr->row->top_row->expr )
		return( expr->row->top_row->expr );
	else if( (parent = expr_get_parent_dynamic( expr )) )
		return( expr_get_root_dynamic( parent ) );
	else
		return( NULL ); 
}

/* Is an expr part of a row, including enclosing exprs. 
 *
 * For example, row A1 could be "[x::x<-A2]", that would be expanded to 
 * something like 
 * "$lcomp0 {$lcomp0 = foldr $f0 [] A2 {$f0 x $sofar = x : $sofar}}"
 * Now, row A1 depends on A2, but expr A1 will not ... it's $lcomp0, the local
 * expr of A1, that will get called for expr_dirty.
 *
 * Return NULL for expr is not a row and has no enclosing rows.
 */
static Row *
expr_get_row( Expr *expr )
{
	if( expr->row )
		return( expr->row );
	else if( is_top( expr->sym ) )
		return( NULL );
	else
		return( expr_get_row( expr_get_parent( expr ) ) );
}

void 
expr_new_value( Expr *expr )
{
#ifdef DEBUG
{
	PElement *root = &expr->root;

	printf( "expr_new_value: " );
	symbol_name_print( expr->sym );
	printf( ": " );
	graph_pointer( root );
}
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( expr ), expr_signals[SIG_NEW_VALUE], 0 );
}

/* An expr has lost a value.
 */
void
expr_value_destroy( Expr *expr )
{
	/* Break ImageInfo link (if any).
	 */
	if( expr->imageinfo )
		imageinfo_expr_remove( expr, expr->imageinfo );
}

/* Clean up an expr, ready to have a new def parsed into it.
 */
void *
expr_strip( Expr *expr )
{
	expr_error_clear( expr );

	/* Break top links we're part of.
	 */
	if( slist_map( expr->static_links, 
		(SListMapFn) link_expr_destroy, NULL ) )
		return( expr );
	if( slist_map( expr->dynamic_links, 
		(SListMapFn) link_expr_destroy, NULL ) )
		return( expr );
	g_assert( !expr->static_links );
	g_assert( !expr->dynamic_links );

	/* Junk error stuff. 
	 */
	IM_FREE( expr->error_top );
	IM_FREE( expr->error_sub );

	/* Unref the compile.
	 */
	if( expr->compile )
		(void) compile_expr_link_break( expr->compile, expr );

	return( NULL );
}

static void 
expr_dispose( GObject *gobject )
{
	Expr *expr = EXPR( gobject );
	Symbol *sym = expr->sym;

#ifdef DEBUG
	printf( "expr_dispose: " );
	expr_name_print( expr );
	printf( "\n" );
#endif /*DEBUG*/

	expr_strip( expr );

	/* Break the value link.
	 */
	expr_value_destroy( expr );

	/* Unlink from symbol.
	 */
	if( sym->expr == expr )
		sym->expr = NULL;

	if( expr->row ) {
		Row *row = expr->row;

		/* If this is the sym for a top row, kill the row too.
		 * Otherwise just break the link and wait for the next row
		 * refresh to do the kill for us.
		 */
		if( row == row->top_row ) {
			IDESTROY( row );
		}
		else {
			row->expr = NULL;
			row->sym = NULL;

			expr->row = NULL;

			/* Make sure we will re-parse and compile any text
			 * with this sym that might have been modified from
			 * the default.
			 */
			if( row->child_rhs && row->child_rhs->itext ) {
				iText *itext = ITEXT( row->child_rhs->itext );

				if( itext->edited )
					heapmodel_set_modified( 
						HEAPMODEL( itext ), TRUE );
			}
		}
	}

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
expr_info( iObject *iobject, VipsBuf *buf )
{
	Expr *expr = EXPR( iobject );

	if( expr->err ) {
		vips_buf_appends( buf, _( "Error" ) );
		vips_buf_appendf( buf, ": %s\n%s\n", 
			expr->error_top, expr->error_sub );
	}
}

static void
expr_real_new_value( Expr *expr )
{
	PElement *root = &expr->root;

	expr_value_destroy( expr );
	if( PEISIMAGE( root ) && PEGETII( root ) ) 
		imageinfo_expr_add( PEGETII( root ), expr );

	/* If this is the main expr for this symbol, signal new value there
	 * too.
	 */
	if( expr->sym->expr == expr )
		symbol_new_value( expr->sym );
}

static void
expr_class_init( ExprClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */
	expr_signals[SIG_NEW_VALUE] = g_signal_new( "new_value",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ExprClass, new_value ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );

	/* Init methods.
	 */
	gobject_class->dispose = expr_dispose;

	iobject_class->info = expr_info;

	class->new_value = expr_real_new_value;

	/* Static init.
	 */
}

static void
expr_init( Expr *expr )
{
	expr->sym = NULL;
	expr->row = NULL;
	expr->compile = NULL;

	expr->static_links = NULL;
	expr->dynamic_links = NULL;

	expr->imageinfo = NULL;

	expr->err = FALSE;
	expr->error_top = NULL;
	expr->error_sub = NULL;
}

GType
expr_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ExprClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) expr_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Expr ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) expr_init,
		};

		type = g_type_register_static( TYPE_ICONTAINER, 
			"Expr", &info, 0 );
	}

	return( type );
}

Expr *
expr_new( Symbol *sym )
{
	Expr *expr;

	expr = EXPR( g_object_new( TYPE_EXPR, NULL ) );

	expr->sym = sym;
	PEPOINTE( &expr->root, &sym->base );
	icontainer_child_add( ICONTAINER( sym ), ICONTAINER( expr ), -1 );

#ifdef DEBUG
	printf( "expr_new: " );
	expr_name_print( expr );
	printf( "\n" );
#endif /*DEBUG*/

	return( expr );
}


/* Clone an existing expr. 
 */
Expr *
expr_clone( Symbol *sym )
{
	Expr *expr;

	if( sym->expr && sym->expr->compile ) {
		/* Make a new expr, share the compile.
		 */
                expr = expr_new( sym );
		compile_expr_link_make( sym->expr->compile, expr );
	}
	else {
		/* No existing expr to copy, make a bare one for the
		 * row, at the same scope level as sym.
		 */
                expr = expr_new( sym );
	}

	return( expr );
}

/* Mark an expression as containing an error, save the error buffers.
 */
void *
expr_error_set( Expr *expr )
{
	/* Was not in error? Add to error set.
	 */
	if( !expr->err ) {
#ifdef DEBUG_ERROR
		printf( "expr_error_set: error in " );
		symbol_name_print( expr->sym );
		printf( ": %s %s\n", error_get_top(), error_get_sub() );
#endif /*DEBUG_ERROR*/

		IM_SETSTR( expr->error_top, error_get_top() );
		IM_SETSTR( expr->error_sub, error_get_sub() );

		/* Zap the value of the expr ... it may contain pointers to
		 * dead stuff.
		 */
		PEPUTP( &expr->root, ELEMENT_NOVAL, (void *) 99 );

		expr_error_all = g_slist_prepend( expr_error_all, expr );
		expr->err = TRUE;
		if( expr->row )
			row_error_set( expr->row );

		/* If this is the value of a top-level sym, note state
		 * change on symbol.
		 */
		if( is_top( expr->sym ) && expr->sym->expr == expr )
			symbol_state_change( expr->sym );
	}

	return( NULL );
}

/* Extract the error from an expression.
 */
void
expr_error_get( Expr *expr )
{
	if( !expr->err )
		error_clear();
	else {
		g_assert( expr->error_top );
		g_assert( expr->error_sub );

		error_top( "%s", expr->error_top );
		error_sub( "%s", expr->error_sub );
	}
}

/* Clear error state.
 */
void
expr_error_clear( Expr *expr )
{
	if( expr->err ) {
#ifdef DEBUG_ERROR
		printf( "expr_error_clear: " );
		symbol_name_print( expr->sym );
		printf( "\n"  );
#endif /*DEBUG_ERROR*/

		expr->err = FALSE;
		expr_error_all = g_slist_remove( expr_error_all, expr );
		if( expr->row )
			row_error_clear( expr->row );

		if( is_top( expr->sym ) && expr->sym->expr == expr )
			symbol_state_change( expr->sym );
	}
}

/* Mark an expr dirty.
 *
 * Two cases: if expr has a row, this is part of a display. Use the row
 * stuff to mark this expr dirty. Then use symbol_dirty() to mark on from the
 * root of this row.
 *
 * Case two: this must be an expr inside a top-level ... just
 * symbol_dirty() on from that top level.
 *
 * FIXME ... we should be able to scrap this expr_get_root() ... we want the
 * 'parent' field in the Link we are probably being called from.
 */
void *
expr_dirty( Expr *expr, int serial )
{
	Row *row;

#ifdef DEBUG
	printf( "expr_dirty: " );
	symbol_name_print( expr->sym );
	printf( "\n" );
#endif /*DEBUG*/

	if( (row = expr_get_row( expr )) &&
		row->top_row->sym ) {
		Symbol *top_sym = row->top_row->sym;

		row_dirty( row, TRUE );
		symbol_dirty( top_sym, serial );
	}
	else
		symbol_dirty( expr_get_root( expr )->sym, serial );

	return( NULL );
}

void *
expr_dirty_intrans( Expr *expr, int serial )
{
	if( expr->row &&
		expr->row->top_row->sym ) {
		row_dirty_intrans( expr->row, TRUE );
		symbol_dirty( expr->row->top_row->sym, serial );
	}
	else
		symbol_dirty_intrans( expr->sym, serial );

	return( NULL );
}

void
expr_tip_sub( Expr *expr, VipsBuf *buf )
{
	Compile *compile = expr->compile;

	if( is_top( expr->sym ) ) {
		vips_buf_appends( buf, _( "top level" ) );
		vips_buf_appends( buf, " " );
	}

	if( compile && is_class( compile ) ) {
		vips_buf_appends( buf, _( "class" ) );
		vips_buf_appends( buf, " " );
		if( compile->nparam == 0 ) {
			vips_buf_appends( buf, _( "instance" ) );
			vips_buf_appends( buf, " " );
		}
		else {
			vips_buf_appends( buf, _( "definition" ) );
			vips_buf_appends( buf, " " );
		}

		vips_buf_appendf( buf, "\"%s\"", IOBJECT( expr->sym )->name );
	}
	else if( expr->sym->type == SYM_PARAM )
		vips_buf_appendf( buf, _( "parameter \"%s\"" ), 
			IOBJECT( expr->sym )->name );
	else {
		if( is_member( expr->sym ) ) {
			vips_buf_appends( buf, _( "member" ) );
			vips_buf_appends( buf, " " );
		}

		if( compile->nparam == 0 ) {
			vips_buf_appends( buf, _( "value" ) );
			vips_buf_appends( buf, " " );
		}
		else {
			vips_buf_appends( buf, _( "function" ) );
			vips_buf_appends( buf, " " );
		}

		vips_buf_appendf( buf, "\"%s\"", IOBJECT( expr->sym )->name );
	}

	if( !is_top( expr->sym ) ) {
		vips_buf_appends( buf, " " );
		vips_buf_appends( buf, _( "of" ) );
		vips_buf_appends( buf, " " );
		expr_tip_sub( expr_get_parent( expr ), buf );
	}
}

/* Look at an expr, make a tooltip.
 */
void
expr_tip( Expr *expr, VipsBuf *buf )
{
	expr_name( expr, buf ); 
	vips_buf_appends( buf, ": " );
	expr_tip_sub( expr, buf );
}

/* Bind unresolved refs in an expr. Bind for every enclosing dynamic scope.
 */
void
expr_resolve( Expr *expr )
{
	Expr *top = symbol_root->expr;
	Expr *i;

#ifdef DEBUG
	printf( "expr_resolve: " );
	expr_name_print( expr );
	printf( "\n" );
#endif /*DEBUG*/

	for( i = expr; i != top; i = expr_get_parent_dynamic( i ) ) 
		/* May try to resolve out through a parameter.
		 */
		if( i->compile )
			compile_resolve_dynamic( expr->compile, i->compile );
}
