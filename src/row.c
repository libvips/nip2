/* A row in a workspace ... not a widget, part of subcolumn
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

/* Mad detail.
#define DEBUG
 */

/* Show each row being calculated.
#define DEBUG_ROW
 */

/* Making and removing links between rows.
#define DEBUG_LINK
 */

/* Time row recomp.
#define DEBUG_TIME
#define DEBUG_TIME_SORT
 */

/* Trace create/destroy.
#define DEBUG_NEW
 */

/* Dirty/clean stuff.
#define DEBUG_DIRTY
 */

/* Error set/clear.
#define DEBUG_ERROR
 */

/* Show row recomp order decisions.
#define DEBUG_SORT_VERBOSE
#define DEBUG_SORT
 */

#include "ip.h"

static HeapmodelClass *parent_class = NULL;

static void *
row_map_all_sub( Model *model, row_map_fn fn, void *a, void *b, void *c )
{
	if( IS_ROW( model ) )
		return( fn( ROW( model ), a, b, c ) );

	return( NULL );
}

static void *
row_map_all( Row *row, row_map_fn fn, void *a, void *b, void *c )
{
	return( icontainer_map4_all( ICONTAINER( row ),
		(icontainer_map4_fn) row_map_all_sub, (void *) fn, a, b, c ) );
}

const char *
row_name( Row *row )
{
	if( row->sym )
		return( IOBJECT( row->sym )->name );
	else
		return( IOBJECT( row )->name );
}

static Row *
row_get_parent( Row *row )
{
	return( HEAPMODEL( row )->row );
}

/* Make a fully-qualified name for a row's symbol ... walk back up the tally
 * hierarchy. eg. "A1.fred.x" ... produce a name which will find this row from
 * a local of context.
 */
void
row_qualified_name_relative( Symbol *context, Row *row, VipsBuf *buf )
{
	if( !row_get_parent( row ) ) {
		if( !row->sym )
			vips_buf_appends( buf, "(null)" );
		else
			symbol_qualified_name_relative( context, 
				row->sym, buf );
	}
	else {
		/* Qualify our parents, then do us.
		 */
		row_qualified_name_relative( context, 
			row_get_parent( row ), buf );
		vips_buf_appends( buf, "." );
		vips_buf_appends( buf, row_name( row ) );
	}
}

/* Make a fully-qualified name for a row's symbol ... walk back up the tally
 * hierarchy. eg. "A1.fred.x".
 */
void
row_qualified_name( Row *row, VipsBuf *buf )
{
	if( row->ws )
		row_qualified_name_relative( row->ws->sym, row, buf );
}

/* Convenience ... print a row name out, identifying by tally heirarchy.
 */
void *
row_name_print( Row *row )
{
	if( row ) {
		char txt[100];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		row_qualified_name( row, &buf );
		printf( "%s ", vips_buf_all( &buf ) );
	}
	else
		printf( "(null)" );

	return( NULL );
}

static void *
row_dirty_clear( Row *row )
{
#ifdef DEBUG_DIRTY
{
	Row *top_row = row->top_row;

	if( row->dirty )
		g_assert( g_slist_find( top_row->recomp, row ) );
}
#endif /*DEBUG_DIRTY*/

	if( row->dirty ) {
		Row *top_row = row->top_row;

		row->dirty = FALSE;
		top_row->recomp = g_slist_remove( top_row->recomp, row );

#ifdef DEBUG_DIRTY
		printf( "row_dirty_clear: " );
		row_name_print( row );
		printf( "\n" );
#endif /*DEBUG_DIRTY*/

		iobject_changed( IOBJECT( row ) );
	}

	return( NULL );
}

/* Set a single row dirty.
 */
static void *
row_dirty_set_single( Row *row, gboolean clear_error )
{
#ifdef DEBUG_DIRTY
{
	Row *top_row = row->top_row;

	if( row->dirty )
		g_assert( g_slist_find( top_row->recomp, row ) );
	if( !row->dirty )
		g_assert( !g_slist_find( top_row->recomp, row ) );
}
#endif /*DEBUG_DIRTY*/

	if( !row->dirty ) {
		Row *top_row = row->top_row;

		row->dirty = TRUE;
		top_row->recomp = g_slist_prepend( top_row->recomp, row );

		iobject_changed( IOBJECT( row ) );

#ifdef DEBUG_DIRTY
		printf( "row_dirty_set_single: " );
		row_name_print( row );
		printf( " clear_error = %s\n", bool_to_char( clear_error ) );
#endif /*DEBUG_DIRTY*/

		/* Make sure error is clear ... we want to recomp.
		 */
		if( row->expr && clear_error )
			expr_error_clear( row->expr );
	}

	return( NULL );
}

static void *
row_dirty_set_sub( Model *model, gboolean clear_error )
{
	if( IS_ROW( model ) ) {
		Row *row = ROW( model );
		Rhs *rhs = row->child_rhs;

		g_assert( !rhs || IS_RHS( rhs ) );

		if( rhs && rhs->itext && ITEXT( rhs->itext )->edited )
			row_dirty_set_single( row, clear_error );
		else if( rhs && rhs->graphic && 
			CLASSMODEL( rhs->graphic )->edited )
			row_dirty_set_single( row, clear_error );
	}

	return( NULL );
}

/* When we mark a row dirty, we need to mark any subrows with non-default
 * values dirty too so that they will get a chance to reapply their edits over
 * the top of the new value we will make for this row.
 */
static void *
row_dirty_set( Row *row, gboolean clear_error )
{
	row_dirty_set_single( row, clear_error );

	return( icontainer_map_all( ICONTAINER( row ),
		(icontainer_map_fn) row_dirty_set_sub, 
			GINT_TO_POINTER( clear_error ) ) );
}

/* Mark a row as containing an error ... called from expr_error_set()
 * ... don't call this directly.
 */
void
row_error_set( Row *row )
{
	if( !row->err ) {
		Workspace *ws = row->ws;
		gboolean was_clear = ws->errors == NULL;

		ws->errors = g_slist_prepend( ws->errors, row );
		row->err = TRUE;

#ifdef DEBUG_ERROR
		printf( "row_error_set: " );
		row_name_print( row );
		printf( "\n" );
#endif /*DEBUG_ERROR*/

		iobject_changed( IOBJECT( row ) );

		/* First error? State change on workspace.
		 */
		if( was_clear )
			iobject_changed( IOBJECT( ws ) );

		/* If this is a local row, mark the top row in error too to end
		 * recomp on this tree.
		 */
		if( row != row->top_row ) {
			char txt[100];
			VipsBuf buf = VIPS_BUF_STATIC( txt );

			row_qualified_name( row, &buf );

			error_top( _( "Error in row." ) );
			/* Elements are name of row, principal error,
			 * secondary error.
			 */
			error_sub( _( "Error in row %s: %s\n%s" ),	
				vips_buf_all( &buf ),
				row->expr->error_top,
				row->expr->error_sub );

			expr_error_set( row->top_row->expr );
		}
	}
}

/* Clear error state ... called from expr_error_clear() ... don't call this
 * directly.
 */
void
row_error_clear( Row *row )
{
	if( row->err ) {
		Workspace *ws = row->ws;

		ws->errors = g_slist_remove( ws->errors, row );
		row->err = FALSE;

		iobject_changed( IOBJECT( row ) );

#ifdef DEBUG_ERROR
		printf( "row_error_clear: " );
		row_name_print( row );
		printf( "\n" );
#endif /*DEBUG_ERROR*/

		/* Mark our text modified to make sure we reparse and compile.
		 * The code may contain pointers to dead symbols if we were in
		 * error because they were undefined.
		 */
		if( row->child_rhs && row->child_rhs->itext )
			heapmodel_set_modified( 
				HEAPMODEL( row->child_rhs->itext ), TRUE );

		/* All errors gone? Ws changed too.
		 */
		if( !ws->errors )
			iobject_changed( IOBJECT( ws ) );

		/* Is this a local row? Clear the top row error as well, in
		 * case it's in error because of us.
		 */
		if( row != row->top_row && row->top_row->expr ) {
			expr_error_clear( row->top_row->expr );
			row_dirty_set( row->top_row, TRUE );
		}
	}
}

/* Break a dependency.
 */
static void *
row_link_break( Row *parent, Row *child )
{
	/* Must be there.
	 */
	g_assert( g_slist_find( parent->children, child ) &&
		g_slist_find( child->parents, parent ) );

	parent->children = g_slist_remove( parent->children, child );
	child->parents = g_slist_remove( child->parents, parent );

#ifdef DEBUG_LINK
	printf( "row_link_break: breaking link from " );
	row_name_print( parent );
	printf( "to " );
	row_name_print( child );
	printf( "\n" );
#endif /*DEBUG_LINK*/

	return( NULL );
}

static void *
row_link_break_rev( Row *child, Row *parent )
{
	return( row_link_break( parent, child ) );
}

static void
row_dispose( GObject *gobject )
{
	Row *row = ROW( gobject );

#ifdef DEBUG_NEW
	/* Can't use row_name_print(), we may not have a parent.
	 */
	printf( "row_dispose: %s", NN( IOBJECT( row )->name ) );
	if( row->sym ) 
		printf( " (%s)", symbol_name( row->sym ) );
	printf( "\n" );
#endif /*DEBUG_NEW*/

	/* Reset state. Also see row_parent_remove().
	 */
	row_hide_dependents( row );
	if( row->expr )
		expr_error_clear( row->expr );
	if( row->top_col && row->top_col->last_select == row )
		row->top_col->last_select = NULL;
	row_deselect( row );

	/* Break all recomp links.
	 */
	slist_map( row->parents, (SListMapFn) row_link_break, row );
	slist_map( row->children, (SListMapFn) row_link_break_rev, row );
	g_assert( !row->parents && !row->children );
	(void) slist_map( row->recomp, (SListMapFn) row_dirty_clear, NULL );
	if( row->top_row )
		row->top_row->recomp_save = 
			g_slist_remove( row->top_row->recomp_save, row );
	IM_FREEF( g_slist_free, row->recomp_save );

	g_assert( !row->recomp );

	if( row->expr ) {
		g_assert( row->expr->row == row );

		/* If we're a local row, we will have a private expr 
		 * allocated for us. Junk it.
		 */
		if( row != row->top_row ) 
			icontainer_child_remove( ICONTAINER( row->expr ) );
		else {
			/* Top-level row, we were zapping the sym's expr.
			 * Break the link to it.
			 */
			row->expr->row = NULL;
			row->expr = NULL;
		}
	}

	/* Is this a top-level row? Kill the symbol too. Need to do this after
	 * sorting out row->expr, since otherwise killing the symbol will kill
	 * us again in turn.
	 */
	if( row == row->top_row ) 
		IDESTROY( row->sym );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void *
row_add_parent_name( Link *link, VipsBuf *buf )
{
	Row *row;

	if( link->parent->expr && (row = link->parent->expr->row) ) {
		row_qualified_name_relative( link->child, row, buf );
		vips_buf_appends( buf, " " );
	}

	return( NULL );
}

static void *
row_add_child_name( Link *link, VipsBuf *buf )
{
	Row *row;

	if( link->child->expr && (row = link->child->expr->row) ) {
		row_qualified_name_relative( link->parent, row, buf );
		vips_buf_appends( buf, " " );
	}

	return( NULL );
}

static void *
row_add_dirty_child_name( Link *link, VipsBuf *buf )
{
	if( link->child->dirty ) {
		symbol_qualified_name( link->child, buf );
		vips_buf_appends( buf, " " );
	}

	return( NULL );
}

static void
row_info( iObject *iobject, VipsBuf *buf )
{
	Row *row = ROW( iobject );

	vips_buf_appends( buf, _( "Name" ) );
	vips_buf_appends( buf, ": " );
	row_qualified_name( row, buf );
	vips_buf_appends( buf, "\n" );

	if( row->expr ) 
		iobject_info( IOBJECT( row->expr ), buf );
	if( row->child_rhs->itext ) 
		iobject_info( IOBJECT( row->child_rhs->itext ), buf );
	if( row->child_rhs->graphic ) 
		iobject_info( IOBJECT( row->child_rhs->graphic ), buf );
	if( row->top_row->sym ) {
		if( row->top_row->sym->topchildren ) {
			row_qualified_name( row, buf );
			vips_buf_appends( buf, " " );
			/* Expands to eg. "B1 refers to: B2, B3".
			 */
			vips_buf_appends( buf, _( "refers to" ) );
			vips_buf_appends( buf, ": " );
			slist_map( row->top_row->sym->topchildren, 
				(SListMapFn) row_add_child_name, buf );
			vips_buf_appends( buf, "\n" );
		}
		if( row->top_row->sym->topparents ) {
			row_qualified_name( row, buf );
			vips_buf_appends( buf, " " );
			/* Expands to eg. "B1 is referred to by: B2, B3".
			 */
			vips_buf_appends( buf, _( "is referred to by" ) );
			vips_buf_appends( buf, ": " );
			slist_map( row->top_row->sym->topparents, 
				(SListMapFn) row_add_parent_name, buf );
			vips_buf_appends( buf, "\n" );
		}
	}
	if( row == row->top_row && row->sym->dirty ) {
		Symbol *sym = row->sym;

		if( sym->ndirtychildren ) {
			row_qualified_name( row, buf );
			vips_buf_appends( buf, " " );
			vips_buf_appends( buf, _( "is blocked on" ) );
			vips_buf_appends( buf, ": " );
			slist_map( sym->topchildren,
				(SListMapFn) row_add_dirty_child_name, buf );
			vips_buf_appends( buf, "\n" );
		}
	}
}

static Rhs *
row_get_rhs( Row *row )
{
	g_assert( g_slist_length( ICONTAINER( row )->children ) == 1 );

	return( RHS( ICONTAINER( row )->children->data ) );
}

static void
row_child_add( iContainer *parent, iContainer *child, int pos )
{
	Row *row = ROW( parent );

	ICONTAINER_CLASS( parent_class )->child_add( parent, child, pos );

	/* Update our context.
	 */
	row->child_rhs = row_get_rhs( row );
}

static Subcolumn *
row_get_subcolumn( Row *row )
{
	return( SUBCOLUMN( ICONTAINER( row )->parent ) );
}

static Column *
row_get_column( Row *row )
{
	Subcolumn *scol = row_get_subcolumn( row );

	if( scol )
		return( scol->top_col ); 
	else
		return( NULL ); 
}

/* Search back up the widget hierarchy for the base row for this
 * row ... eg "A7"->expr->row.
 */
static Row *
row_get_root( Row *row )
{
	Row *enclosing = row_get_parent( row );

	if( !enclosing )
		return( row );
	else
		return( row_get_root( enclosing ) );
}

Workspace *
row_get_workspace( Row *row )
{
	Column *col = row_get_column( row );

	if( col )
		return( col->ws );
	else
		return( NULL );
}

static void
row_parent_add( iContainer *child )
{
	Row *row = ROW( child );

	g_assert( IS_SUBCOLUMN( child->parent ) );

	ICONTAINER_CLASS( parent_class )->parent_add( child );

	/* Update our context.
	 */
	row->scol = row_get_subcolumn( row );
	row->top_col = row_get_column( row );
	row->ws = row_get_workspace( row );
	row->top_row = row_get_root( row );
}

static void
row_parent_remove( iContainer *child )
{
	Row *row = ROW( child );

	/* Reset the parts of state which touch our parent.
	 */
	row_dirty_clear( row );
	row_deselect( row );

	/* Don't clear error ... we may no longer have the link to expr. See
	 * row_dispose() for that.
	 */

	ICONTAINER_CLASS( parent_class )->parent_remove( child );
}

static View *
row_view_new( Model *model, View *parent )
{
	return( rowview_new() );
}

static void
row_scrollto( Model *model, ModelScrollPosition position )
{
	Row *row = ROW( model );
	Column *col = row->top_col;

	/* If our column is closed, there won't be a view to scrollto, ouch!
	 * Need to open the column first, then scroll to that column. We can't
	 * scroll to the exact row, since there's no view for it, and won't be
	 * for a while after we hit the idle loop again.
	 */
	if( !col->open ) {
		column_set_open( col, TRUE );
		model_scrollto( MODEL( col ), position );
	}

	MODEL_CLASS( parent_class )->scrollto( model, position );
}

static gboolean
row_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	Row *row = ROW( model );
	Subcolumn *scol = SUBCOLUMN( parent );

	char name[256];

	g_assert( IS_SUBCOLUMN( parent ) );

	if( !get_sprop( xnode, "name", name, 256 ) ) 
		return( FALSE );
	IM_SETSTR( IOBJECT( row )->name, name );

#ifdef DEBUG
	printf( "row_load: loading row %s (xmlNode %p)\n", name, xnode );
#endif /*DEBUG*/

	/* Popup is optional (top level only)
	 */
	(void) get_bprop( xnode, "popup", &row->popup );

	if( scol->is_top ) {
		Column *col = scol->top_col;
		Workspace *ws = col->ws;

		Symbol *sym;

		sym = symbol_new( ws->sym->expr->compile, name );
		symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		row_link_symbol( row, sym, NULL );

		/* We can't symbol_made() here, we've not parsed our value
		 * yet. See below ... we just make sure we're on the recomp
		 * lists.
		 */
	}

	if( !MODEL_CLASS( parent_class )->load( model, state, parent, xnode ) )
		return( FALSE );

	/* If we've loaded a complete row system, mark this row plus any 
	 * edited subrows dirty, and make sure this sym is dirty too.
	 */
	if( scol->is_top ) {
		row_dirty_set( row, TRUE );
		expr_dirty( row->sym->expr, link_serial_new() );
	}

	/* If we've loaded a toplevel into a closed column, make sure display
	 * is off.
	 */
	if( scol->is_top && !scol->top_col->open )
		model_set_display( MODEL( row ), FALSE );

	return( TRUE );
}

/* Should we display this row. Non-displayed rows don't have rhs, don't
 * appear on the screen, and aren't saved. They do have rows though, so their
 * dependencies are tracked.
 *
 * We work off sym rather than row so that we can work before the row is fully
 * built.
 */
static gboolean
row_is_displayable( Symbol *sym )
{
	if( is_system( sym ) )
		return( FALSE );
	if( sym->expr && sym->expr->compile && sym->expr->compile->nparam > 0 )
		return( FALSE );
	if( is_super( sym ) && sym->expr ) {
		Expr *expr = sym->expr;
		PElement *root = &expr->root;

		/* Empty superclass.
		 */
		if( PEISELIST( root ) )
			return( FALSE );
	}

	return( TRUE );
}

static xmlNode *
row_save( Model *model, xmlNode *xnode )
{
	Row *row = ROW( model );

	xmlNode *xthis;

	/* Don't save system rows, or empty superclasses.
	 */
	if( row->sym ) {
		if( !row_is_displayable( row->sym ) )
			/* Need to return non-NULL for abort with no error.
			 */
			return( (xmlNode *) -1 );
	}

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	/* Top-level only.
	 */
	if( row->top_row == row ) 
		if( !set_sprop( xthis, "popup", bool_to_char( row->popup ) ) )
			return( NULL );
	if( !set_sprop( xthis, "name", IOBJECT( row )->name ) )
		return( NULL );

	return( xthis );
}

static void *
row_clear_to_save( Model *model )
{
	if( IS_ROW( model ) ) 
		ROW( model )->to_save = FALSE;

	return( NULL );
}

static void *
row_set_to_save( Row *row )
{
	Row *enclosing;

	if( !row->to_save ) {
		row->to_save = TRUE;

		/* All peers must be saved. When we reload, we want to keep
		 * row ordering. If we just save modded row, they'll move to
		 * the front of the row list on reload, since they'll be made
		 * first.
		 */
		icontainer_map( ICONTAINER( row->scol ),
			(icontainer_map_fn) row_set_to_save, NULL, NULL );

		/* All rows back up to the top level must also be saved.
		 */
		for( enclosing = row; enclosing != row->top_row; 
			enclosing = row_get_parent( enclosing ) )
			row_set_to_save( enclosing );
	}

	return( NULL );
}

static void *
row_calculate_to_save( Model *model )
{
	if( IS_ROW( model ) ) {
		Row *row = ROW( model );
		Rhs *rhs = row->child_rhs;

		if( row != row->top_row && rhs && !row->to_save ) {
			if( rhs->itext && ITEXT( rhs->itext )->edited )
				row_set_to_save( row );
			else if( rhs->graphic && 
				CLASSMODEL( rhs->graphic )->edited )
				row_set_to_save( row );
		}
	}

	return( NULL );
}

static gboolean
row_save_test( Model *model )
{
	Row *row = ROW( model );
	Workspace *ws = row->ws;
	gboolean save;

	if( row == row->top_row ) {
		/* This is a top-level row ... save unless we're in
		 * only-save-selected mode.
		 */
		if( ws->save_type == WORKSPACE_SAVE_SELECTED )
			save = row->selected;
		else
			save = TRUE;

		/* If we're going to save this row, clear all the to_save
		 * flags, then walk the tree working out which bits we will need
		 * to write.
		 */
		if( save ) {
			icontainer_map_all( ICONTAINER( row ),
				(icontainer_map_fn) row_clear_to_save, NULL );
			icontainer_map_all( ICONTAINER( row ),
				(icontainer_map_fn) row_calculate_to_save, 
				NULL );
		}

	}
	else 
		save = row->to_save;

	return( save );
}

static void *
row_new_heap( Heapmodel *heapmodel, PElement *root )
{
	Row *row = ROW( heapmodel );
	Expr *expr = row->expr;

#ifdef DEBUG
	printf( "row_new_heap: " );
	row_name_print( row );
	printf( "\n" );

	printf( "row_new_heap: new value is " );
	pgraph( root );

	printf( "row_new_heap: top value is " );
	pgraph( &row->top_row->expr->root );
#endif /*DEBUG*/

	if( row_is_displayable( row->sym ) ) {
		/* Hide superclasses whose constructor starts with "_".
		 */
		if( is_super( row->sym ) && PEISCLASS( root ) &&
			*IOBJECT( PEGETCLASSCOMPILE( root )->sym )->name == 
			'_' )
			model_set_display( MODEL( row ), FALSE );
		/* Hide top-level rows in closed columns.
		 */
		else if( row->scol->is_top && !row->top_col->open )
			model_set_display( MODEL( row ), FALSE );
		else
			model_set_display( MODEL( row ), TRUE );
	}

	/* New value ... reset error state.
	 */
	expr_error_clear( expr );
	expr->root = *root;
	expr_new_value( expr );

	if( row->child_rhs &&
		heapmodel_new_heap( HEAPMODEL( row->child_rhs ), root ) )
		return( row );

	/* Class display only for non-param classes.
	 */
	row->is_class = PEISCLASS( root ) && row->sym->type != SYM_PARAM;

	/* Set the default vis level.
	 */
	if( row->child_rhs && row->child_rhs->vislevel == -1 ) {
		PElement member;
		double value;
		gboolean is_class;

		if( !heap_is_class( root, &is_class ) )
			return( row );

		/* If it's a class with a vis hint, use that.
		 */
		if( is_class && 
			class_get_member( root, MEMBER_VISLEVEL, 
				NULL, &member ) &&
			heap_get_real( &member, &value ) ) 
			rhs_set_vislevel( row->child_rhs, value );

		/* Non-parameter rows get higher vislevel, except for super. 
		 */
		else if( row->sym->type != SYM_PARAM && !is_super( row->sym ) )
			rhs_set_vislevel( row->child_rhs, 1 );
		else 
			rhs_set_vislevel( row->child_rhs, 0 );
	}

	return( HEAPMODEL_CLASS( parent_class )->new_heap( heapmodel, root ) ); }

static void *
row_update_model( Heapmodel *heapmodel )
{
	Row *row = ROW( heapmodel );

	if( row->expr )
		expr_new_value( row->expr );

	return( HEAPMODEL_CLASS( parent_class )->update_model( heapmodel ) );
}

static void
row_class_init( RowClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = row_dispose;

	iobject_class->info = row_info;

	icontainer_class->child_add = row_child_add;
	icontainer_class->parent_add = row_parent_add;
	icontainer_class->parent_remove = row_parent_remove;

	model_class->view_new = row_view_new;
	model_class->scrollto = row_scrollto;
	model_class->load = row_load;
	model_class->save = row_save;
	model_class->save_test = row_save_test;

	heapmodel_class->new_heap = row_new_heap;
	heapmodel_class->update_model = row_update_model;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
row_init( Row *row )
{
#ifdef DEBUG
	printf( "row_init\n" );
#endif /*DEBUG*/

	row->scol = NULL;
	row->child_rhs = NULL;
	row->top_col = NULL;
	row->ws = NULL;
	row->top_row = NULL;

	row->sym = NULL;

	row->expr = NULL;
	row->err = FALSE;

	row->selected = FALSE;
	row->is_class = FALSE;
	row->popup = POPUP_NEW_ROWS;
	row->to_save = FALSE;

	/* Init recomp stuff.
	 */
	row->parents = NULL;
	row->children = NULL;
	row->dirty = FALSE;
	row->recomp = NULL;
	row->recomp_save = NULL;

	row->depend = FALSE;

	row->show = ROW_SHOW_NONE;
}

GType
row_get_type( void )
{
	static GType row_type = 0;

	if( !row_type ) {
		static const GTypeInfo info = {
			sizeof( RowClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) row_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Row ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) row_init,
		};

		row_type = g_type_register_static( TYPE_HEAPMODEL, 
			"Row", &info, 0 );
	}

	return( row_type );
}

/* After making a row and adding it to model tree ... attach the symbol and
 * value this row displays.
 */
void
row_link_symbol( Row *row, Symbol *sym, PElement *root )
{
	g_assert( !row->sym );
	g_assert( !row->expr );
	g_assert( !sym->expr || !sym->expr->row );

	row->sym = sym;

	/* Code we display/update ... if this is a top-level row, we
	 * directly change the symbol's expr. If it's a sub-row, we need a
	 * cloned expr for us to fiddle with.
	 */
	if( is_top( sym ) ) {
		row->expr = sym->expr;
		g_assert( !row->expr->row );
		row->expr->row = row;
	}
	else {
		row->expr = expr_clone( sym );
		row->expr->row = row;

		if( root ) {
			row->expr->root = *root;
			expr_new_value( row->expr );
		}
	}
}

Row *
row_new( Subcolumn *scol, Symbol *sym, PElement *root )
{
	Row *row = g_object_new( TYPE_ROW, NULL );

#ifdef DEBUG_NEW
	printf( "row_new: " );
	dump_tiny( sym );
	printf( "\n" );
#endif /*DEBUG_NEW*/

	/* Don't make a display or a RHS for invisible rows.
	 */
	if( !row_is_displayable( sym ) )
		MODEL( row )->display = FALSE;
	else 
		(void) rhs_new( row ); 

	iobject_set( IOBJECT( row ), IOBJECT( sym )->name, NULL );
	icontainer_child_add( ICONTAINER( scol ), ICONTAINER( row ), -1 );

	row_link_symbol( row, sym, root );

	return( row );
}

/* Make a dependency. parent is displaying an expression which 
 * refers to the symbol being displayed by child.
 */
static void *
row_link_make( Row *parent, Row *child )
{
	/* Already a dependency? Don't make a second link.
	 */
	if( g_slist_find( parent->children, child ) ) 
		return( NULL );

	/* Don't link to self (harmless, but pointless too).
	 */
	if( parent == child )
		return( NULL );
	
	/* New link, each direction.
	 */
	parent->children = g_slist_prepend( parent->children, child );
	child->parents = g_slist_prepend( child->parents, parent );

#ifdef DEBUG_LINK
	printf( "row_link_make: " );
	row_name_print( parent );
	printf( "refers to " );
	row_name_print( child );
	printf( "\n" );
#endif /*DEBUG_LINK*/

	return( NULL );
}

static void *
row_link_build4( Expr *child_expr, Row *row )
{
	if( child_expr->row && child_expr->row->top_row == row )
		return( child_expr->row );

	return( NULL );
}

/* Does child have a display in the same tally heirarchy as row? Make a link!
 */
static void *
row_link_build3( Symbol *child, Row *row )
{
	Row *child_row;

	child_row = (Row *) icontainer_map( ICONTAINER( child ),
		(icontainer_map_fn) row_link_build4, row->top_row, NULL );

	if( child_row ) 
		(void) row_link_make( row, child_row );

	return( NULL );
}

static void *row_link_build2( Expr *expr, Row *row );

static void *
row_link_build2_sym( Symbol *sym, Row *row )
{
	if( sym->expr && row_link_build2( sym->expr, row ) )
		return( row );
	
	return( NULL );
}

static void *
row_link_build2( Expr *expr, Row *row )
{
	/* Make links to anything expr refers to in this tree.
	 */
	if( expr->compile &&
		slist_map( expr->compile->children, 
			(SListMapFn) row_link_build3, row ) )
		return( expr );

	/* Recurse for any locals of expr. 
	 * Exception: 
	 * 
	 *	f = class {
	 *		g = class {
	 *			a = 12;
	 *		}
	 *	}
	 *
	 * zero-arg local classes will have rows anyway, so we don't need to
	 * check inside them for locals, since we'll do them anyway at the top
	 * level.
	 *
	 * zero-arg hidden classes do need to be checked inside though :-(
	 * since we will only have a row for the top element.
	 */
	if( expr->compile && 
		!(is_class( expr->compile ) && expr->compile->nparam == 0 &&
			expr->row && MODEL( expr->row )->display) &&
		icontainer_map( ICONTAINER( expr->compile ),
			(icontainer_map_fn) row_link_build2_sym, row, NULL ) )
		return( expr );

	return( NULL );
}

/* Scan a row, adding links for any dependencies we find.
 */
static void *
row_link_build( Row *row )
{
#ifdef DEBUG_LINK
	printf( "row_link_build: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG_LINK*/

	/* Build new recomp list. Only for class displays.
	 */
	if( !row->scol->is_top && row->expr && 
		row_link_build2( row->expr, row ) )
		return( row );

	return( NULL );
}

/* Remove any links on a row.
 */
static void *
row_link_destroy( Row *row )
{
	slist_map( row->children, 
		(SListMapFn) row_link_break_rev, row );

	return( NULL );
}

static void *row_dependent_map_sub( Row *row, row_map_fn fn, void *a );

/* Do this row, and any that depend on it.
 */
static void *
row_dependent_mark( Row *row, row_map_fn fn, void *a )
{
	void *res;

	/* Done this one already?
	 */
	if( row->depend )
		return( NULL );

	row->depend = TRUE;
	if( (res = fn( row, a, NULL, NULL )) )
		return( res );

	return( row_dependent_map_sub( row, fn, a ) );
}

/* Apply to all dependents of row.
 */
static void *
row_dependent_map_sub( Row *row, row_map_fn fn, void *a )
{
	Row *i;
	void *res;

	/* Things that refer to us.
	 */
	if( (res = slist_map2( row->parents, 
		(SListMap2Fn) row_dependent_mark, (void *) fn, a )) )
		return( res );

	/* Things that refer to our enclosing syms ... eg. if A1.fred.x 
	 * changes, we don't want to recalc A1.fred, we do want to recalc 
	 * anything that refers to A1.fred.
	 */
	for( i = row; (i = HEAPMODEL( i )->row); ) 
		if( (res = row_dependent_map_sub( i, fn, a )) )
			return( res );

	/* We are not going to spot things that refer to this.us :-( we could
	 * say anything that depends on "this" depends on us, but that's much 
	 * too broad (and much too slow).

	 	FIXME ... could use dynamic dependency stuff to find things
		that refer to this.us?
	 */

	return( NULL );
}

static void *
row_dependent_clear( Row *row )
{
	row->depend = FALSE;

	return( NULL );
}

/* Apply a function to all rows in this tree which depend on this row.
 */
void *
row_dependent_map( Row *row, row_map_fn fn, void *a )
{
	/* Clear the flags we use to spot loops.
	 */
	row_map_all( row->top_row,
		(row_map_fn) row_dependent_clear, NULL, NULL, NULL );

	return( row_dependent_map_sub( row, fn, a ) );
}

/* This row has changed ... mark all dependents (direct and indirect) 
 * dirty.
 */
void *
row_dirty( Row *row, gboolean clear_error )
{
	(void) row_dirty_set( row, clear_error );
	(void) row_dependent_map( row, 
		(row_map_fn) row_dirty_set, GINT_TO_POINTER( clear_error ) );

	return( NULL );
}

/* This tally has changed ... mark all dependents (but not this one!)
 * dirty.
 */
void *
row_dirty_intrans( Row *row, gboolean clear_error )
{
	(void) row_dependent_map( row, 
		(row_map_fn) row_dirty_set, GINT_TO_POINTER( clear_error ) );

	return( NULL );
}

/* Find the 'depth' of a row ... 0 is top level.
 */
static int
row_recomp_depth( Row *row )
{
	if( row == row->top_row )
		return( 0 );

	return( 1 + row_recomp_depth( row_get_parent( row ) ) );
}

/* Compare func for row recomp sort.
 */
static int
row_recomp_sort_func( Row *a, Row *b )
{
	int order;
#ifdef DEBUG_TIME_SORT
	static GTimer *sort_func_timer = NULL;

	if( !sort_func_timer )
		sort_func_timer = g_timer_new();

	g_timer_reset( sort_func_timer );
#endif /*DEBUG_TIME_SORT*/

#ifdef DEBUG_SORT_VERBOSE
	printf( "row_recomp_sort_func: " );
#endif /*DEBUG_SORT_VERBOSE*/

	/* If b depends on a, want a first.
	 */
	if( row_dependent_map( a, (row_map_fn) map_equal, b ) ) {
#ifdef DEBUG_SORT_VERBOSE
		row_name_print( a );
		printf( "before " );
		row_name_print( b );
		printf( "(2nd depends on 1st)\n" );
#endif /*DEBUG_SORT_VERBOSE*/

		order = -1;
	}
	else if( row_dependent_map( b, (row_map_fn) map_equal, a ) ) {
#ifdef DEBUG_SORT_VERBOSE
		row_name_print( b );
		printf( "before " );
		row_name_print( a );
		printf( "(2nd depends on 1st #2)\n" );
#endif /*DEBUG_SORT_VERBOSE*/

		order = 1;
	}
	else {
		int adepth = row_recomp_depth( a );
		int bdepth = row_recomp_depth( b );

#ifdef DEBUG_SORT_VERBOSE
		if( adepth < bdepth ) {
			row_name_print( a );
			printf( "before " );
			row_name_print( b );
			printf( "(1st shallower)\n" );
		}
		else if( bdepth < adepth ) {
			row_name_print( b );
			printf( "before " );
			row_name_print( a );
			printf( "(1st shallower)\n" );
		}
		else {
			row_name_print( a );
			printf( "and " );
			row_name_print( b );
			printf( "independent\n" );
		}
#endif /*DEBUG_SORT_VERBOSE*/

		/* No dependency ... want shallower first.
		 */
		order = adepth - bdepth;
	}

#ifdef DEBUG_TIME_SORT
	printf( "row_recomp_sort_func: took %gs\n",  
		g_timer_elapsed( sort_func_timer, NULL ) );
#endif /*DEBUG_TIME_SORT*/

	return( order );
}

/* Insert-sort an slist.
 */
static GSList *
row_recomp_sort_slist( GSList *old )
{
	GSList *new;
	GSList *p;

	new = NULL;

	for( p = old; p; p = p->next ) {
		Row *a = (Row *) p->data;
		Row *b;
		GSList *q;

		for( q = new; q; q = q->next ) {
			b = (Row *) q->data;

			if( row_recomp_sort_func( a, b ) < 0 )
				break;
		}

		if( q ) {
			q->data = a;
			q->next = g_slist_prepend( q->next, b );
		}
		else
			new = g_slist_append( new, a );
	}

	g_slist_free( old );

	return( new );
}

/* Sort dirties into recomp order.
 */
static void
row_recomp_sort( Row *row )
{
#ifdef DEBUG_TIME_SORT
	static GTimer *sort_timer = NULL;

	if( !sort_timer )
		sort_timer = g_timer_new();

	g_timer_reset( sort_timer );
#endif /*DEBUG_TIME_SORT*/

	g_assert( row == row->top_row );

	/* Nope, can't use g_slist_sort(). We have a partial order and
	 * g_slist_sort() uses an algorithm that assumes a full order. Do a
	 * simple insert-sort, it'll do enough comparisons that we won't miss
	 * things.

		row->recomp = g_slist_sort( row->recomp, 
			(GCompareFunc) row_recomp_sort_func );

	 */
	row->recomp = row_recomp_sort_slist( row->recomp );

#ifdef DEBUG_TIME_SORT
	printf( "row_recomp_sort: took %gs\n",  
		g_timer_elapsed( sort_timer, NULL ) );
#endif /*DEBUG_TIME_SORT*/

#ifdef DEBUG_SORT
	printf( "row_recomp: sorted dirties are: " );
	slist_map( row->recomp, (SListMapFn) row_name_print, NULL );
	printf( "\n" );
#endif /*DEBUG_SORT*/
}

static gboolean
row_regenerate( Row *row )
{
	Expr *expr = row->expr;
	PElement base;

	/* Regenerate any compiled code.
	 */
	if( expr->compile ) {
		PEPOINTE( &base, &expr->compile->base );

		if( !PEISNOVAL( &base ) ) {
			PElement *root = &expr->root;

			if( row == row->top_row ) {
				/* Recalcing base of tally display ... not a 
				 * class member, must be a sym with a value.
				 */
				gboolean res;

				res = reduce_regenerate( expr, root );
				expr_new_value( expr );

				if( !res )
					return( FALSE );
			}
			else {
				/* Recalcing a member somewhere inside ... 
				 * regen (member this) pair. Get the "this"
				 * for the enclosing class instance ... the
				 * top one won't always be right (eg. for
				 * local classes); the enclosing one should
				 * be the same as the most enclosing this.
				 */
				Row *this = row->scol->this;
				gboolean res;

				res = reduce_regenerate_member( expr, 
					&this->expr->root, root );
				expr_new_value( expr );

				if( !res )
					return( FALSE );
			}

			/* We may have made a new class instance ... all our 
			 * children need to update their heap pointers.
			 */
			if( heapmodel_new_heap( HEAPMODEL( row ), root ) ) 
				return( FALSE );
		}
	}

	return( TRUE );
}

static gboolean
row_recomp_row( Row *row )
{
	Rhs *rhs = row->child_rhs;

#ifdef DEBUG
	printf( "row_recomp_row: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	/* Not much we can do. 
	 */
	if( !row->expr )
		return( TRUE );

	/* Clear old error state.
	 */
	expr_error_clear( row->expr );

	/* Parse and compile any changes to our text since we last came through.
	 */
	if( rhs && rhs->itext && 
		heapmodel_update_heap( HEAPMODEL( rhs->itext ) ) ) 
		return( FALSE );

	/* We're about to zap the graph: make sure this tree of rows has a
	 * private copy.
	 */
	if( !subcolumn_make_private( row->scol ) )
		return( FALSE );

	/* Regenerate from the expr.
	 */
	if( !row_regenerate( row ) ) 
		return( FALSE );

	/* Reapply any graphic mods.
	 */
	if( rhs && rhs->graphic ) {
		Classmodel *classmodel = CLASSMODEL( rhs->graphic );

		/* If the graphic is non-default, need to set modified to make
		 * sure we reapply the changes.
		 */
		if( classmodel->edited )
			heapmodel_set_modified( HEAPMODEL( classmodel ), TRUE );

		if( heapmodel_update_heap( HEAPMODEL( classmodel ) ) )
			return( FALSE );
	}

	progress_update_tick();

	return( TRUE );
}

static void
row_recomp_all( Row *top_row )
{
	/* Rebuild all dirty rows.
	 */
	while( !top_row->err && top_row->recomp ) {
		Row *dirty_row = ROW( top_row->recomp->data );

#ifdef DEBUG_ROW
		static GTimer *timer = NULL;

		if( !timer )
			timer = g_timer_new();
		g_timer_reset( timer );
#endif /*DEBUG_ROW*/

		if( !row_recomp_row( dirty_row ) ) 
			/* This will set top_row->err and end the loop.
			 */
			expr_error_set( dirty_row->expr );
		else
			row_dirty_clear( dirty_row );

#ifdef DEBUG_ROW
		printf( "row_recomp_all: done row " );
		row_name_print( dirty_row );
		printf( " - %gs\n", g_timer_elapsed( timer, NULL ) );
#endif /*DEBUG_ROW*/

#ifdef DEBUG
		printf( "row_recomp_all: after row recomp, top value now " );
		pgraph( &top_row->expr->root );
#endif /*DEBUG*/
	}
}

void
row_recomp( Row *row )
{
	Row *top_row = row->top_row;

#ifdef DEBUG_TIME
	static GTimer *recomp_timer = NULL;

	if( !recomp_timer )
		recomp_timer = g_timer_new();

	g_timer_reset( recomp_timer );

	printf( "row_recomp: starting for dirties on " );
	row_name_print( top_row );
	printf( "\n" );
#endif /*DEBUG_TIME*/

	/* Sort dirties into recomp order.
	 */
	row_recomp_sort( top_row );

	/* Take a copy of the recomp list for later testing.
	 */
	IM_FREEF( g_slist_free, top_row->recomp_save );
	top_row->recomp_save = g_slist_copy( top_row->recomp );

	/* Remove all top-level dependencies.
	 */
	symbol_link_destroy( top_row->sym );

	/* Remove any row recomp links we have.
	 */
	(void) row_map_all( top_row,
		(row_map_fn) row_link_destroy, NULL, NULL, NULL );

	/* Rebuild all dirty rows. This may add some dynamic top links.
	 */
	row_recomp_all( top_row );

	/* Add all static row links. Have to do this after any 
	 * parsing in row_recomp_all().
	 */
	(void) row_map_all( top_row,
		(row_map_fn) row_link_build, NULL, NULL, NULL );

	/* Remake all static top-level links.
	 */
	(void) symbol_link_build( top_row->sym );

	/* Now we know dependencies ... mark everything dirty again. This may
	 * pick up stuff we missed last time and may change the order we
	 * recomp rows in.
	 *
	 * Be careful not to wipe out any errors we found on this first pass.
	 */
	slist_map( top_row->recomp_save, (SListMapFn) row_dirty, FALSE );

	/* Is this topsym still a leaf? We may have discovered an external 
	 * reference to another dirty top-level sym. We can come back here
	 * later.
	 */
	if( top_row->sym->ndirtychildren != 0 ) {
		IM_FREEF( g_slist_free, top_row->recomp_save );

#ifdef DEBUG_TIME
		printf( "row_recomp: delaying recomp of " );
		row_name_print( top_row );
		printf( "after %gs\n",  
			g_timer_elapsed( recomp_timer, NULL ) );
#endif /*DEBUG_TIME*/

		return;
	}

	/* Sort dirties into recomp order.
	 */
	row_recomp_sort( top_row );

	/* Now: if the recomp list is the same as last time, we don't need to
	 * recalc again.
	 */
	if( slist_equal( top_row->recomp_save, top_row->recomp ) ) {
		/* Provided we didn't abandon recomp on an error, we can 
		 * just mark all rows clean.
		 */
		if( !top_row->err )
			slist_map( top_row->recomp, 
				(SListMapFn) row_dirty_clear, NULL );
	}
	else {
#ifdef DEBUG_DIRTY
		printf( "row_recomp: recomp list has changed ... pass 2\n" );
#endif /*DEBUG_DIRTY*/

		/* Rebuild all dirty rows in a second pass.
		 */
		row_recomp_all( top_row );
	}

	IM_FREEF( g_slist_free, top_row->recomp_save );

	/* The symbol can be cleared as well.
	 */
	if( !top_row->err )
		symbol_dirty_clear( top_row->sym );

	/* Now we're clean, all models can update from the heap. Rows
	 * containing errors can have bad pointers in, so careful.
	 */
	if( !top_row->err && icontainer_map_all( ICONTAINER( top_row ),
		(icontainer_map_fn) heapmodel_update_model, NULL ) )
		expr_error_set( top_row->expr );

#ifdef DEBUG_TIME
	printf( "row_recomp: done for dirties of " );
	row_name_print( top_row );
	printf( "in %gs\n",  g_timer_elapsed( recomp_timer, NULL ) );
#endif /*DEBUG_TIME*/

#ifdef DEBUG
	printf( "row_recomp: value of " );
	row_name_print( top_row );
	printf( "is " );
	pgraph( &top_row->expr->root );
#endif /*DEBUG*/
}

/* Test, suitable for mapping.
 */
void *
row_is_selected( Row *row )
{
	if( row->selected )
		return( row );

	return( NULL );
}

/* Deselect a row. 
 */
void *
row_deselect( Row *row )
{
	Workspace *ws = row->ws;

	if( !row->selected )
		return( NULL );

	g_assert( ws && IS_WORKSPACE( ws ) );
	g_assert( g_slist_find( ws->selected, row ) );

	ws->selected = g_slist_remove( ws->selected, row );
	row->selected = FALSE;

	/* Hack: if this is a matrix with selected cells, deselect the matrix
	 * sellection too. We should really have a row method for this I
	 * guess :-( See also workspace_selected_names_sub().
	 */
	if( row->child_rhs && row->child_rhs->graphic &&
		IS_MATRIX( row->child_rhs->graphic ) &&
		MATRIX( row->child_rhs->graphic )->selected ) 
		matrix_deselect( MATRIX( row->child_rhs->graphic ) );

	iobject_changed( IOBJECT( row ) );
	iobject_changed( IOBJECT( ws ) );

	return( NULL );
}

/* Select a row. 
 */
static void
row_select2( Row *row )
{
	if( !row->selected ) {
		Workspace *ws = row->ws;

		row->selected = TRUE;
		ws->selected = g_slist_append( ws->selected, row );

		iobject_changed( IOBJECT( row ) );
		iobject_changed( IOBJECT( ws ) );
	}
}

/* Make sure a row is selected ... used for (eg.) select changed on gktsheet.
 * No deselection.
 */
void *
row_select_ensure( Row *row )
{
	row_select2( row );

	/* Note for extend select.
	 */
	row->top_col->last_select = row;

	return( NULL );
}

/* Select a row, deselecting others first.
 */
void *
row_select( Row *row )
{
	Workspace *ws = row->ws;

	workspace_deselect_all( ws );
	row_select2( row );

	/* Note for extend select.
	 */
	row->top_col->last_select = row;

	return( NULL );
}

/* Extend the previous selection.
 */
void *
row_select_extend( Row *row )
{
	Column *col = row->top_col;
	Row *last_select = col->last_select;

	/* Range select if there was a previous selection, and it was in the
	 * same subcolumn.
	 */
	if( last_select && row->scol == last_select->scol ) {
		Subcolumn *scol = row->scol;
		GSList *rows = ICONTAINER( scol )->children;
		int pos = g_slist_index( rows, row );
		int pos_last = g_slist_index( rows, last_select );
		int step = pos > pos_last ? 1 : -1;
		int i;

		g_assert( pos != -1 && pos_last != -1 );

		for( i = pos_last; i != pos + step; i += step )
			row_select2( ROW( g_slist_nth_data( rows, i ) ) );
	}
	else 
		row_select2( row );

	/* Note for extend select.
	 */
	col->last_select = row;

	return( NULL );
}

/* Toggle a selection.
 */
void *
row_select_toggle( Row *row )
{
	if( row->selected ) {
		row_deselect( row );
		row->top_col->last_select = NULL;
	}
	else {
		row_select2( row );
		row->top_col->last_select = row;
	}

	return( NULL );
}

/* Do a select action using a modifier.
 */
void 
row_select_modifier( Row *row, guint state )
{
	if( state & GDK_CONTROL_MASK ) 
		row_select_toggle( row );
	else if( state & GDK_SHIFT_MASK ) 
		row_select_extend( row );
	else 
		row_select( row );
}

static void
row_set_show( Row *row, RowShowState show )
{
	if( row->show != show ) {
		row->show = show;
		iobject_changed( IOBJECT( row ) );
	}
}

static void *
row_show_parent( Link *link, RowShowState show )
{
	if( link->parent->expr && link->parent->expr->row ) 
		row_set_show( link->parent->expr->row, show );

	return( NULL );
}

static void *
row_show_child( Link *link, RowShowState show )
{
	if( link->child->expr && link->child->expr->row ) 
		row_set_show( link->child->expr->row, show );

	return( NULL );
}

void
row_show_dependents( Row *row )
{
	Symbol *topsym = row->top_row->sym;

#ifdef DEBUG
	printf( "row_show_dependents: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	if( topsym ) {
		slist_map( topsym->topparents,
			(SListMapFn) row_show_parent, 
			GUINT_TO_POINTER( ROW_SHOW_PARENT ) );
		slist_map( topsym->topchildren,
			(SListMapFn) row_show_child, 
			GUINT_TO_POINTER( ROW_SHOW_CHILD ) );
	}
}

void
row_hide_dependents( Row *row )
{
	Symbol *topsym;

#ifdef DEBUG
	printf( "row_hide_dependents: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	if( row->top_row && (topsym = row->top_row->sym) ) {
		slist_map( topsym->topparents,
			(SListMapFn) row_show_parent, 
			GUINT_TO_POINTER( ROW_SHOW_NONE ) );
		slist_map( topsym->topchildren,
			(SListMapFn) row_show_child, 
			GUINT_TO_POINTER( ROW_SHOW_NONE ) );
	}
}

/* Set help for a row. Used by rowview and itextview etc. on mouseover.
 */
void
row_set_status( Row *row )
{
	Expr *expr = row->expr;

	char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	/* No symbol? eg. on load error.
	 */
	if( !expr )
		return;

	row_qualified_name( row, &buf );

	if( expr->err ) {
		vips_buf_appends( &buf, ": " );
		vips_buf_appends( &buf, expr->error_top );
	}
	else if( row->child_rhs->itext ) {
		iText *itext = ITEXT( row->child_rhs->itext );

		vips_buf_appends( &buf, " = " );

		if( row->ws->mode != WORKSPACE_MODE_FORMULA )
			vips_buf_appends( &buf, NN( itext->formula ) );
		else 
			vips_buf_appends( &buf, vips_buf_all( &itext->value ) );
	}

	workspace_set_status( row->ws, "%s", vips_buf_firstline( &buf ) );
}

/* Sub fn of below ... search inside a row hierarcy. Context is (eg.) row
 * "A1", path is (eg.) "super.name".
 */
static Row *
row_parse_name_row( Row *context, const char *path )
{
	char name[256];
	char *tail;
	Row *row;
	Subcolumn *scol;

#ifdef DEBUG
	printf( "row_parse_name_row: \"%s\"\n", path );
#endif /*DEBUG*/

	/* Break the name into "thing.tail", where tail could contain other
	 * "." qualifiers.
	 */
	im_strncpy( name, path, 256 );
	if( !(tail = break_token( name, "." )) )
		/* Passed empty string.
		 */
		return( context );

	/* Needs to be a subcolumn to look inside. We could search the value,
	 * but it's safer to look inside the model we've built from the value.
	 */
	if( !context->child_rhs ||
		!context->child_rhs->scol ||
		!(scol = SUBCOLUMN( context->child_rhs->scol )) )
		return( NULL );

	if( !(row = subcolumn_map( scol, 
		(row_map_fn) iobject_test_name, name, NULL )) )
		return( NULL );

	return( row_parse_name_row( row, tail ) );
}

/* Parse a qualified name .. eg. "untitled.A1.name" and find the row. Find
 * relative to context. Context is a sym, so we can have workspacegroups etc.
 */
Row *
row_parse_name( Symbol *context, const char *path )
{
	char name[256];
	char *tail;
	Symbol *sym;
	Row *row;

#ifdef DEBUG
	printf( "row_parse_name: \"%s\"\n", path );
#endif /*DEBUG*/

	/* Break the name into "thing.tail", where tail could contain other
	 * "." qualifiers.
	 */
	im_strncpy( name, path, 256 );
	if( !(tail = break_token( name, "." )) ) {
		/* Run out of names ... return this row, if we've found one.
		 */
		if( context->expr )
			return( context->expr->row );
		else
			return( NULL );
	}

	/* Try to look up name in context. For scopes, we can do it
	 * statically. For other syms, look up in the value of the symbol.
	 */
	switch( context->type ) {
	case SYM_WORKSPACE:
	case SYM_WORKSPACEGROUP:
	case SYM_ROOT:
		if( !(sym = compile_lookup( context->expr->compile, name )) ) 
			return( NULL );
		break;

	case SYM_VALUE:
		if( !(row = context->expr->row) )
			return( NULL );

		/* Hand off to the row searcher.
		 */
		return( row_parse_name_row( row, path ) );

	case SYM_ZOMBIE:
	case SYM_PARAM:
	case SYM_EXTERNAL:
	case SYM_BUILTIN:
	default:
		/* How odd.
		 */
		return( NULL );
	}

	return( row_parse_name( sym, tail ) );
}
