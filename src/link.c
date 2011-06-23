/* Links between top-level syms and the exprs which reference them
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

/*
#define DEBUG
#define DEBUG_DIRTY
 */

#include "ip.h"

void *
link_expr_destroy( LinkExpr *le )
{
	GSList **llinks = le->dynamic ? &le->link->dynamic_links : 
		&le->link->static_links;
	GSList **elinks = le->dynamic ? &le->expr->dynamic_links : 
		&le->expr->static_links;

#ifdef DEBUG
	printf( "link_expr_destroy: removing expr " );
	symbol_name_print( le->expr->sym );
	printf( "referencing link->child = " );
	symbol_name_print( le->link->child );
	printf( "\n" );
#endif /*DEBUG*/

	*llinks = slist_remove_all( *llinks, le );
	*elinks = slist_remove_all( *elinks, le );

        im_free( le );

	return( NULL );
}

static LinkExpr *
link_expr_new( Link *link, Expr *expr, gboolean dynamic )
{
	GSList **llinks = dynamic ? &link->dynamic_links : &link->static_links;
	GSList **elinks = dynamic ? &expr->dynamic_links : &expr->static_links;
        LinkExpr *le;

	g_assert( expr_get_root_dynamic( expr )->sym == link->parent );

#ifdef DEBUG
	printf( "link_expr_new: expr " );
	symbol_name_print( expr->sym );
	printf( "references link->child = " );
	symbol_name_print( link->child );
	printf( "\n" );
#endif /*DEBUG*/

        if( !(le = INEW( NULL, LinkExpr )) )
		return( NULL );

        le->link = link; 
        le->expr = expr; 
        le->count = 1; 
        le->dynamic = dynamic; 

	*llinks = g_slist_prepend( *llinks, le );
	*elinks = g_slist_prepend( *elinks, le );

        return( le );
}

/* Make a new serial number.
 */
int
link_serial_new( void )
{
	static int serial = 0;

	return( serial++ );
}

/* Fwd ref. 
 */
static void *symbol_dirty_set( Symbol *sym );

/* child has become dirty ... update parent's dirty count.
 */
static void *
link_dirty_child( Link *link )
{
	g_assert( link->parent->ndirtychildren >= 0 );

	link->parent->ndirtychildren += 1;

	if( link->parent->ndirtychildren == 1 ) 
		/* Parent had no dirty children ... it does now.
		 */
		symbol_dirty_set( link->parent );

	symbol_state_change( link->parent );

	return( NULL );
}

/* link->parent no longer has link->child as a dirty child (cleaned or
 * removed) ... update counts.
 */
static void *
link_clean_child( Link *link )
{
	Symbol *parent = link->parent;

	/* One fewer dirty children!
	 */
	parent->ndirtychildren--;
	g_assert( parent->ndirtychildren >= 0 );

	/* Have we just cleaned the last dirty child of link->parent? If we
	 * have and if link->parent has an error, clear the error so that
	 * link->parent gets a chance to recalc. The new value of
	 * link->child might fix the problem.
	 */
	if( parent->ndirtychildren == 0 ) 
		expr_error_clear( parent->expr );

	symbol_state_change( parent );

	return( NULL );
}

/* Junk a link.
 */
void *
link_destroy( Link *link )
{
#ifdef DEBUG
	printf( "link_destroy: destroying link from " );
	symbol_name_print( link->parent );
	printf( "to " );
	symbol_name_print( link->child );
	printf( "\n" );
#endif /*DEBUG*/

	if( link->child->dirty )
		(void) link_clean_child( link );

	link->parent->topchildren = 
		slist_remove_all( link->parent->topchildren, link );
	link->child->topparents = 
		slist_remove_all( link->child->topparents, link );
	slist_map( link->static_links, 
		(SListMapFn) link_expr_destroy, NULL );
	slist_map( link->dynamic_links, 
		(SListMapFn) link_expr_destroy, NULL );

        im_free( link );

	return( NULL );
}

/* Make a new link.
 */
static Link *
link_new( Symbol *child, Symbol *parent )
{
        Link *link;

	g_assert( is_top( parent ) && is_top( child ) );
	g_assert( parent != child );

#ifdef DEBUG
	printf( "link_new: making link from " );
	symbol_name_print( parent );
	printf( "to " );
	symbol_name_print( child );
	printf( "\n" );
#endif /*DEBUG*/

        if( !(link = INEW( NULL, Link )) )
		return( NULL );

        link->parent = parent; 
        link->child = child; 
        link->serial = 0; 
        link->static_links = NULL; 
        link->dynamic_links = NULL; 

	parent->topchildren = g_slist_prepend( parent->topchildren, link );
	child->topparents = g_slist_prepend( child->topparents, link );

	/* If the new child is dirty, note it.
	 */
	if( child->dirty )
		link_dirty_child( link );

        return( link );
}

static Link *
link_find_child_sub( Link *link, Symbol *child )
{
	if( link->child == child )
		return( link );

	return( NULL );
}

/* Look up connection between child and parent.
 */
static Link *
link_find_child( Symbol *child, Symbol *parent )
{
	return( (Link *) slist_map( parent->topchildren,
		(SListMapFn) link_find_child_sub, child ) );
}

static void *
link_expr_find_expr_sub( LinkExpr *le, Expr *expr )
{
	if( le->expr == expr )
		return( le );

	return( NULL );
}

/* Look up a linkexpr by expr.
 */
static LinkExpr *
link_expr_find_expr( Link *link, Expr *expr, gboolean dynamic )
{
	GSList *links = dynamic ? link->dynamic_links : link->static_links;

	return( (LinkExpr *) slist_map( links,
		(SListMapFn) link_expr_find_expr_sub, expr ) );
}

/* Add a reference from expr to child to the link graph.
 */
void *
link_add( Symbol *child, Expr *expr, gboolean dynamic )
{
	Expr *parent = expr_get_root_dynamic( expr );
	Link *link;
	LinkExpr *le;

#ifdef DEBUG
	printf( "link_add: child = " );
	symbol_name_print( child );
	printf( "; expr = " );
	expr_name_print( expr );
	printf( "; dynamic = %s\n", bool_to_char( dynamic ) );
#endif /*DEBUG*/

	g_assert( parent );
	g_assert( parent->sym );
	g_assert( is_top( child ) && is_top( parent->sym ) );
	g_assert( child != parent->sym );

	if( !(link = link_find_child( child, parent->sym )) ) {
		if( !(link = link_new( child, parent->sym )) )
			return( child );
	}

	if( !(le = link_expr_find_expr( link, expr, dynamic )) ) {
		if( !(le = link_expr_new( link, expr, dynamic )) )
			return( child );
	}
	else
		le->count++;

	return( NULL );
}

/* Remove a ref from expr to child.
 */
void *
link_remove( Symbol *child, Expr *expr, gboolean dynamic )
{
	Symbol *parent = expr_get_root_dynamic( expr )->sym;
	Link *link = link_find_child( child, parent );
	LinkExpr *le = link_expr_find_expr( link, expr, dynamic );

	g_assert( is_top( parent ) && is_top( child ) );
	g_assert( parent != child );
	g_assert( link );

	le->count--;
	if( le->count == 0 ) {
		if( link_expr_destroy( le ) )
			return( child );
	}
	if( !link->static_links && !link->dynamic_links ) {
		if( link_destroy( link ) )
			return( child );
	}

	return( NULL );
}

/* Is this a ref to a top-level? Add to link graph if it is.
 */
static void *
link_children_expr_sub( Symbol *child, Expr *expr )
{
	if( is_top( child ) ) {
		Expr *root = expr_get_root_dynamic( expr );

		/* Don't need to record recursive refs.
		 */
		if( root && root->sym && root->sym != child ) {
			if( link_add( child, expr, FALSE ) )
				return( child );
		}
	}

	return( NULL );
}

/* Fwd.
 */
static void *link_children( Symbol *child, Symbol *parent );

/* Add any refs to top-level syms within this local to the
 * top-level sym we are within.
 */
static void *
link_children_expr( Expr *expr, Symbol *parent )
{
	if( expr->compile ) {
		Compile *compile = expr->compile;

		/* Add refs which local makes directly.
		 */
		if( slist_map( compile->children, 
			(SListMapFn) link_children_expr_sub, expr ) )
			return( expr );

		/* ... and recurse for sub-children.
		 */
		(void) icontainer_map( ICONTAINER( compile ),
			(icontainer_map_fn) link_children, parent, NULL );
	}

        return( NULL );
}

/* Add any refs to top-level syms within this local to the
 * top-level sym we are within.
 */
static void *
link_children( Symbol *child, Symbol *parent )
{
        if( child->expr ) {
		if( link_children_expr( child->expr, parent ) )
			return( child );
	}

        return( NULL );
}

/* row is editing sym's value ... add any dependancies the user has included
 * there.
 */
static void *
link_row( Model *model, Symbol *parent )
{
	if( !IS_ROW( model ) || !ROW( model )->expr ) 
		return( NULL );

        /* Add any stuff in this row.
         */
        return( link_children_expr( ROW( model )->expr, parent ) );
}

static void *
symbol_ndirty_sub( Link *link, int *nd )
{
	if( link->child->dirty )
		*nd += 1;

	return( NULL );
}

/* Count the number of dirty children. Used to generate initial leaf counts
 * and for assert() checking.
 */
int
symbol_ndirty( Symbol *sym )
{
	int nd = 0;

	(void) slist_map( sym->topchildren, 
		(SListMapFn) symbol_ndirty_sub, &nd );

	return( nd );
}

/* Fix a leaf count. 
 */
void *
symbol_fix_counts( Symbol *sym )
{
	int old_count = sym->ndirtychildren;

	sym->ndirtychildren = symbol_ndirty( sym );

	g_assert( sym->ndirtychildren == old_count );

	symbol_state_change( sym );

	return( NULL );
}

/* Junk all old links, static + dynamic.
 */
void
symbol_link_destroy( Symbol *sym )
{
	(void) slist_map( sym->topchildren, (SListMapFn) link_destroy, NULL );
}

/* Scan a symbol, remaking all the links.
 */
void
symbol_link_build( Symbol *sym )
{
	g_assert( is_top( sym ) );

        /* Make static links for our expr and all subexprs. If this symbol 
	 * is being edited, get stuff from the edited value.
         */
        if( sym->expr ) {
		if( sym->expr->row )
			(void) icontainer_map_all( 
				ICONTAINER( sym->expr->row ), 
				(icontainer_map_fn) link_row, sym );
		else
			(void) link_children_expr( sym->expr, sym );
	}

#ifdef DEBUG
	printf( "symbol_link_build: " );
	symbol_name_print( sym );
	printf( "\n" );
	dump_links( sym );
#endif /*DEBUG*/

}

static void *
link_dirty_set_sub( LinkExpr *le, int serial )
{
	return( expr_dirty( le->expr, serial ) );
}

/* Mark exprs in parent dirty. These may be sub exprs, so parent is not
 * necessarily going to be symbol_dirty_set() ... eg. A2 may be displaying an
 * instance of class "fred", and we might have edited one of A2's members to
 * refer to A1 ... but A2 depends on A1, fred does not.
 */
static void *
link_dirty_set( Link *link, int serial )
{
	/* Mark exprs in parent dirty.
	 */
	if( slist_map( link->static_links, 
		(SListMapFn) link_dirty_set_sub, GINT_TO_POINTER( serial ) ) ||
	    slist_map( link->dynamic_links, 
	    	(SListMapFn) link_dirty_set_sub, GINT_TO_POINTER( serial ) ) )
		return( link );

	return( NULL );
}

/* Walk the link graph, marking stuff for recomputation ... link->child has
 * changed, mark link->parent dirty.
 */
static void *
link_dirty_walk( Link *link, int serial )
{
	/* Have we walked down this link before? 
	 */
	if( link->serial == serial )
		return( NULL );
	link->serial = serial;

	/* Mark all exprs in parent dirty.
	 */
	return( link_dirty_set( link, serial ) );
}

/* A symbol has changed ... walk the link graph, marking stuff dirty as
 * required. We don't mark this sym dirty.
 */
void *
symbol_dirty_intrans( Symbol *sym, int serial )
{
	g_assert( is_top( sym ) );

	return( slist_map( sym->topparents, 
		(SListMapFn) link_dirty_walk, GINT_TO_POINTER( serial ) ) );
}

static void *
symbol_dirty_set( Symbol *sym )
{
	g_assert( is_top( sym ) );

	/* Clear error, to make sure we will recomp it.
	 */
	if( sym->expr )
		expr_error_clear( sym->expr );

	if( !sym->dirty ) {
#ifdef DEBUG_DIRTY
		printf( "symbol_dirty_set: " );
		symbol_name_print( sym );
		printf( "(%p)\n", sym );
#endif /*DEBUG_DIRTY*/

		/* Change of state.
		 */
		sym->dirty = TRUE;

		/* Update dirty counts on our parents.
		 */
		(void) slist_map( sym->topparents,
			(SListMapFn) link_dirty_child, NULL );

		/* Note change in leaf set and display.
		 */
		symbol_state_change( sym );
	}

	return( NULL );
}

/* ... mark this one as well.
 */
void *
symbol_dirty( Symbol *sym, int serial )
{
	g_assert( is_top( sym ) );

	symbol_dirty_set( sym );

	return( symbol_dirty_intrans( sym, serial ) );
}

void *
link_dirty_total( Link *link, int serial )
{
	static int recursion_depth = 0;

	/* Entering: note new recursion.
	 */
	if( recursion_depth++ > 1000 ) {
		error_top( _( "Circular dependency." ) );
		error_sub( _( "Circular dependency detected near "
			"symbol \"%s\"." ),
			IOBJECT( link->parent )->name );
		recursion_depth = 0;
		return( link );
	}

	/* Mark this sub-tree as dirty.
	 */
	symbol_dirty( link->child, serial );

	/* ... and repeat for any parents.
	 */
	if( link->child->type != SYM_ZOMBIE )
		if( slist_map( link->child->topchildren, 
			(SListMapFn) link_dirty_total, 
				GINT_TO_POINTER( serial ) ) )
			return( link );

	/* Pop recursion measure.
	 */
	recursion_depth--;

	return( NULL );
}

/* As above, but mark children as dirty as well. Used by force recalc to make
 * sure that everything is completely rebuilt. Be careful of cycles!
 */
void *
symbol_dirty_total( Symbol *sym, int serial )
{
	if( sym->type == SYM_ZOMBIE )
		return( NULL );

	/* No children: just mark this sub-tree as dirty.
	 */
	if( !sym->topchildren && symbol_dirty( sym, serial ) )
		return( sym );

	if( slist_map( sym->topchildren, 
		(SListMapFn) link_dirty_total, GINT_TO_POINTER( serial ) ) )
		return( sym );

	return( NULL );
}

/* Mark a symbol as clean. Knock down the leaf count of the things which refer
 * to us ... one of them may turn into a leaf as a result.
 */
void *
symbol_dirty_clear( Symbol *sym )
{
	g_assert( is_top( sym ) );

	if( sym->dirty ) {
#ifdef DEBUG_DIRTY
		printf( "symbol_dirty_clear: " );
		symbol_name_print( sym );
		printf( "(%p)\n", sym );
#endif /*DEBUG_DIRTY*/

		/* Change of state.
		 */
		sym->dirty = FALSE;
		symbol_state_change( sym );

		/* Update dirty counts on our parents.
		 */
		(void) slist_map( sym->topparents,
			(SListMapFn) link_clean_child, NULL );
	}

	return( NULL );
}
