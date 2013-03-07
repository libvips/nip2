/* a subcolumn 
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

/* 
#define DEBUG
 */

#include "ip.h"

static HeapmodelClass *parent_class = NULL;

static gboolean
subcolumn_row_pred_none( Row *row )
{
	return( FALSE );
}

/* No params, no super.
 */
static gboolean
subcolumn_row_pred_members( Row *row )
{
	if( is_system( row->sym ) )
		return( FALSE );

	if( is_super( row->sym ) )
		return( FALSE );

	if( row->sym->type == SYM_PARAM )
		return( FALSE );

	return( TRUE );
}

static gboolean
subcolumn_row_pred_params( Row *row )
{
	return( row->sym->type == SYM_PARAM );
}

/* Everything but empty superclasses.
 */
static gboolean
subcolumn_row_pred_super( Row *row )
{
	if( is_super( row->sym ) && PEISELIST( &row->expr->root ) )
		return( FALSE );

	return( TRUE );
}

/* Array of these guys control member visibility, scol->vislevel indexes this
 * array, one of preds from vislevel down has to be TRUE for the row to be
 * visible.
 */
const SubcolumnVisibility subcolumn_visibility[] = {
	{ "none", subcolumn_row_pred_none },
	{ "members", subcolumn_row_pred_members },
	{ "params", subcolumn_row_pred_params },
	{ "super", subcolumn_row_pred_super }
};
const int subcolumn_nvisibility = IM_NUMBER( subcolumn_visibility );

/* Map down a Subcolumn.
 */
void *
subcolumn_map( Subcolumn *scol, row_map_fn fn, void *a, void *b )
{
	return( icontainer_map( ICONTAINER( scol ), 
		(icontainer_map_fn) fn, a, b ) );
}

static void
subcolumn_dispose( GObject *gobject )
{
	Subcolumn *scol;

#ifdef DEBUG
	printf( "subcolumn_dispose\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_SUBCOLUMN( gobject ) );

	scol = SUBCOLUMN( gobject );

	scol->col = NULL;
	scol->scol = NULL;
	scol->top_col = NULL;

	heap_unregister_element( reduce_context->heap, &scol->base );
	scol->base.type = ELEMENT_NOVAL;
	scol->base.ele = (void *) 13;

	scol->this = NULL;
	scol->super = NULL;

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

/* Stuff we track during class instance display update.
 */
typedef struct {
	Subcolumn *scol;		/* Enclosing column */
	GSList *notused;		/* List of row we've not used */
} ClassRefreshInfo;

/* Test for row represents a sym.
 */
static void *
subcolumn_test_sym( Row *row, Symbol *sym )
{
	if( row->sym == sym )
		return( row );

	return( NULL );
}

/* Test for row has a zombie of the same name.
 */
static void *
subcolumn_test_row_name( Row *row, Symbol *sym )
{
	if( !row->sym && 
		strcmp( IOBJECT( row )->name, IOBJECT( sym )->name ) == 0 )
		return( row );

	return( NULL );
}

/* Refresh one line of a subcolumn.
 */
static void
subcolumn_class_new_heap_sub( ClassRefreshInfo *cri,
	Symbol *sym, PElement *value )
{
	Row *row;

#ifdef DEBUG
	char txt[200];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	symbol_qualified_name( sym, &buf );
	printf( "subcolumn_class_new_heap_sub: %s\n", vips_buf_all( &buf ) );
#endif /*DEBUG*/

	/* Do we have a row for this symbol?
	 */
	if( (row = (Row *) slist_map( cri->notused, 
		(SListMapFn) subcolumn_test_sym, sym )) ) {
		/* Update it.
		 */
		if( heapmodel_new_heap( HEAPMODEL( row ), value ) ) 
			expr_error_set( row->expr );

		cri->notused = g_slist_remove( cri->notused, row );
	}
	else if( (row = (Row *) slist_map( cri->notused, 
		(SListMapFn) subcolumn_test_row_name, sym )) ) {
		/* There's a blank row of the same name, left for us by XML 
		 * load. Update the row with the correct symbol.
		 */
		row_link_symbol( row, sym, NULL );
		if( heapmodel_new_heap( HEAPMODEL( row ), value ) ) 
			expr_error_set( row->expr );

		cri->notused = g_slist_remove( cri->notused, row );
	}
	else {
		row = row_new( cri->scol, sym, value );
		if( heapmodel_new_heap( HEAPMODEL( row ), value ) ) 
			expr_error_set( row->expr );
	}
}

#ifdef DEBUG
static void *
subcolumn_class_dump_tiny_row( Row *row )
{
	row_name_print( row );
	printf( " " );

	return( NULL );
}
#endif /*DEBUG*/

/* A new scrap of heap for a subcolumn.
 */
static gboolean
subcolumn_class_new_heap( Subcolumn *scol, PElement *root )
{
	PElement instance = *root;

	Expr *expr;
	Row *row;
	gboolean result;
	PElement base, member;
	HeapNode *p;
	ClassRefreshInfo cri;

	/* Must be a class display.
	 */
	g_assert( !scol->is_top );
	row = HEAPMODEL( scol )->row;
	expr = row->expr;

#ifdef DEBUG
	printf( "subcolumn_class_new_heap: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	/* Can loop here for some recursive classes.

		FIXME

	if( mainw_countdown_animate( 99 ) )
		return( FALSE );
	 */

	/* No displays for system rows.
	 */
	if( is_system( row->sym ) )
		return( TRUE );

	/* If we are the top of a class instance display, get a new serial.
	 * As we recurse down refreshing our contents, this should stop 
	 * circular structures looping the browser.

	 	FIXME ... clear flags for a whole class, then do a complete
		redisplay? more reliable, but even slower :-(

	 */
	if( scol->scol->is_top )
		heap_serial_new( reduce_context->heap );

	/* Is it a class with a typecheck member? Go through
	 * that. Do an isclass first to force eval.
	 */
	if( !heap_is_class( &instance, &result ) )
		return( FALSE );
	if( result &&
		class_get_member( &instance, MEMBER_CHECK, NULL, &member ) ) {
#ifdef DEBUG
		printf( "subcolumn_class_new_heap: invoking arg checker\n" );
#endif 

		/* Force eval of the typecheck member.
		 */
		if( !heap_is_class( &member, &result ) || !result )
			return( FALSE );
	}

	/* Have we already displayed this class?
	 */
	if( (PEGETVAL( &instance )->flgs & FLAG_SERIAL) == 
		reduce_context->heap->serial ) {
		/* 
		
			FIXME ... display something here? "circular"?

		 */
		return( TRUE );
	}
	SETSERIAL( PEGETVAL( &instance )->flgs, reduce_context->heap->serial );

	/* Note the heap root ... if this is the top of a row tree, then we
	 * clone the class and use that private copy.
	 */
	PEPOINTE( &base, &(SUBCOLUMN( scol ))->base );
	PEPUTPE( &base, &instance );
	PEPUTPE( &expr->root, &base );

	/* Init rebuild params. We make a list of all the existing
	 * row objects for this class display, and every time we
	 * manage to reuse one of them, we knock it off the list. At the
	 * end, remove all unused rows.
	 */
	cri.scol = scol;
	cri.notused = g_slist_copy( ICONTAINER( scol )->children );

#ifdef DEBUG
	printf( "subcolumn_class_new_heap: existing rows: " );
	icontainer_map( ICONTAINER( scol ), 
		(icontainer_map_fn) subcolumn_class_dump_tiny_row, NULL, NULL );
	printf( "\n" );
#endif /*DEBUG*/

	/* Loop along the members, updating row entries.
	 */
	PEGETCLASSMEMBER( &member, &base );

	if( PEISNODE( &member ) )
		for( p = PEGETVAL( &member ); p; p = GETRIGHT( p ) ) {
			PElement s, v;
			HeapNode *hn;
			Symbol *sym;

			/* Get the sym/value pair.
			 */
			hn = GETLEFT( p );
			PEPOINTLEFT( hn, &s );
			PEPOINTRIGHT( hn, &v );
			sym = SYMBOL( PEGETSYMREF( &s ) );

			/* We don't make rows for the default constructor, or
			 * for ".name". These things don't change, so there's
			 * no point (and the default constructor has no text
			 * equivalent anyway).
			 */
			if( strcmp( IOBJECT( sym )->name, MEMBER_NAME ) == 0 )
				continue;
			if( is_member( sym ) && 
				strcmp( IOBJECT( sym )->name, 
				IOBJECT( symbol_get_parent( sym ) )->name ) == 
				0 )
				continue;

			/* Display!
			 */
			subcolumn_class_new_heap_sub( &cri, sym, &v );
		}

	/* Remove all the rows we've not used. 
	 */
	slist_map( cri.notused, (SListMapFn) iobject_destroy, NULL );
	IM_FREEF( g_slist_free, cri.notused );

	return( TRUE );
}

static void *
subcolumn_new_heap( Heapmodel *heapmodel, PElement *root )
{
	Subcolumn *scol = SUBCOLUMN( heapmodel );

	/* New heap for a class display? CLear known_private, we've no idea
	 * where this heap came from.
	 */
	if( scol == scol->top_scol )
		scol->known_private = FALSE;

	/* A bunch of locals? Update them all.
	 */
	if( !scol->is_top && !subcolumn_class_new_heap( scol, root ) )
		return( scol );

	return( HEAPMODEL_CLASS( parent_class )->new_heap( heapmodel, root ) );
}

static void
subcolumn_child_add( iContainer *parent, iContainer *child, int pos )
{
	Subcolumn *scol = SUBCOLUMN( parent );
	Row *row = ROW( child );

	/* May not have a symbol yet during ws load.
	 *
	 * Can't use is_this()/is_super(), not everything has been built yet.
	 * We don't do this often, so strcmp() it.
	 */
	const char *name = 
		row->sym ? IOBJECT( row->sym )->name : IOBJECT( row )->name;

	if( strcmp( name, MEMBER_THIS ) == 0 ) 
		scol->this = row;

	if( strcmp( name, MEMBER_SUPER ) == 0 ) 
		scol->super = row;

	ICONTAINER_CLASS( parent_class )->child_add( parent, child, pos );
}

static void
subcolumn_child_remove( iContainer *parent, iContainer *child )
{
	Subcolumn *scol = SUBCOLUMN( parent );
	Row *row = ROW( child );

	ICONTAINER_CLASS( parent_class )->child_remove( parent, child );

	if( scol->this == row )
		scol->this = NULL;
	if( scol->super == row )
		scol->super = NULL;
}

/* If this is a top-level subcolumn, get the enclosing column.
 */
static Column *
subcolumn_get_column( Subcolumn *scol )
{
	g_assert( scol->is_top );

	return( COLUMN( ICONTAINER( scol )->parent ) );
}

/* If this is a nested subcolumn, get the enclosing subcolumn.
 */
static Subcolumn *
subcolumn_get_subcolumn( Subcolumn *scol )
{
	Rhs *rhs;
	Row *row;
	Subcolumn *escol;

	g_assert( !scol->is_top );

	rhs = HEAPMODEL( scol )->rhs;
	row = HEAPMODEL( rhs )->row;
	escol = row->scol;

	return( escol );
}

/* Return the enclosing column for a Subcolumn.
 */
static Column *
subcolumn_get_top_column( Subcolumn *scol )
{
	if( !scol->is_top ) 
		return( subcolumn_get_top_column( 
			subcolumn_get_subcolumn( scol ) ) );

	return( subcolumn_get_column( scol ) );
}

/* Return the enclosing subcolumn ... but not the is_top one. Ie. the enclosing
 * subcolumn which has the base for this class tree.
 */
static Subcolumn *
subcolumn_get_top_subcolumn( Subcolumn *scol )
{
	Subcolumn *enclosing;

	if( scol->is_top )
		return( NULL );

	enclosing = subcolumn_get_subcolumn( scol );
	if( enclosing->is_top )
		return( scol );
	else 
		return( subcolumn_get_top_subcolumn( enclosing ) );
}

static void
subcolumn_parent_add( iContainer *child )
{
	Subcolumn *scol = SUBCOLUMN( child );

	ICONTAINER_CLASS( parent_class )->parent_add( child );

	g_assert( IS_COLUMN( child->parent ) || IS_RHS( child->parent ) );
	g_assert( !IS_COLUMN( child->parent ) || 
		g_slist_length( child->parent->children ) == 1 );

	scol->is_top = IS_COLUMN( child->parent );

	/* For sub-columns, default to nothing visible.
	 */
	if( !scol->is_top )
		scol->vislevel = 0;

	/* Update context pointers.
	 */
	if( scol->is_top )
		scol->col = subcolumn_get_column( scol );
	else
		scol->col = NULL;

	if( !scol->is_top )
		scol->scol = subcolumn_get_subcolumn( scol );
	else
		scol->scol = NULL;

	scol->top_col = subcolumn_get_top_column( scol );
	scol->top_scol = subcolumn_get_top_subcolumn( scol );

	/* Top level subcolumns default to display on, others to display off.
	 */
	MODEL( scol )->display = scol->is_top;
}

static View *
subcolumn_view_new( Model *model, View *parent )
{
	return( subcolumnview_new() );
}

static gboolean
subcolumn_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	Subcolumn *scol = SUBCOLUMN( model );

	g_assert( IS_COLUMN( parent ) || IS_RHS( parent ) );

	if( !get_iprop( xnode, "vislevel", &scol->vislevel ) )
		return( FALSE );

	if( !MODEL_CLASS( parent_class )->load( model, state, parent, xnode ) )
		return( FALSE );

	return( TRUE );
}

static xmlNode *
subcolumn_save( Model *model, xmlNode *xnode )
{
	Subcolumn *scol = SUBCOLUMN( model );

	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( !set_prop( xthis, "vislevel", "%d", scol->vislevel ) )
		return( NULL );

	return( xthis );
}

static void
subcolumn_class_init( SubcolumnClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = subcolumn_dispose;

	icontainer_class->child_add = subcolumn_child_add;
	icontainer_class->child_remove = subcolumn_child_remove;
	icontainer_class->parent_add = subcolumn_parent_add;

	model_class->view_new = subcolumn_view_new;
	model_class->load = subcolumn_load;
	model_class->save = subcolumn_save;

	heapmodel_class->new_heap = subcolumn_new_heap;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
subcolumn_init( Subcolumn *scol )
{
#ifdef DEBUG
	printf( "subcolumn_init\n" );
#endif /*DEBUG*/

	scol->col = NULL;
	scol->scol = NULL;
	scol->top_col = NULL;

        scol->vislevel = subcolumn_nvisibility - 1;

	scol->base.type = ELEMENT_NOVAL;
	scol->base.ele = (void *) 14;
	heap_register_element( reduce_context->heap, &scol->base );
	scol->known_private = FALSE;

	scol->this = NULL;
	scol->super = NULL;
}

GType
subcolumn_get_type( void )
{
	static GType subcolumn_type = 0;

	if( !subcolumn_type ) {
		static const GTypeInfo info = {
			sizeof( SubcolumnClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) subcolumn_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Subcolumn ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) subcolumn_init,
		};

		subcolumn_type = g_type_register_static( TYPE_HEAPMODEL, 
			"Subcolumn", &info, 0 );
	}

	return( subcolumn_type );
}

static void
subcolumn_link( Subcolumn *scol, Rhs *rhs, Column *col )
{
	g_assert( rhs == NULL || col == NULL );

	/* parent_add() sets is_top for us.
	 */
	if( rhs ) 
		icontainer_child_add( ICONTAINER( rhs ), 
			ICONTAINER( scol ), -1 );
	else 
		icontainer_child_add( ICONTAINER( col ), 
			ICONTAINER( scol ), -1 );
}

Subcolumn *
subcolumn_new( Rhs *rhs, Column *col )
{
	Subcolumn *scol;

	scol = SUBCOLUMN( g_object_new( TYPE_SUBCOLUMN, NULL ) );
	subcolumn_link( scol, rhs, col );

	return( scol );
}

void
subcolumn_set_vislevel( Subcolumn *scol, int vislevel )
{
	scol->vislevel = IM_CLIP( 0, vislevel, subcolumn_nvisibility - 1 );

#ifdef DEBUG
	printf( "subcolumn_set_vislevel: %d\n", scol->vislevel );
#endif /*DEBUG*/

	iobject_changed( IOBJECT( scol ) );
}

/* Make sure we have a private copy of the graph for this tree of stuff.
 */
gboolean
subcolumn_make_private( Subcolumn *scol )
{
	Subcolumn *top_scol = scol->top_scol;
	PElement base;

	if( !top_scol || top_scol->known_private )
		return( TRUE );

#ifdef DEBUG
{
	Row *row = HEAPMODEL( top_scol )->row;

	printf( "subcolumn_make_private: cloning " );
	row_name_print( row );
	printf( "\n" );
}
#endif /*DEBUG*/

	/* Clone from the class args and rebuild our tree.
	 */
	PEPOINTE( &base, &top_scol->base );
	if( !class_clone_args( reduce_context->heap, &base, &base ) ||
		heapmodel_new_heap( HEAPMODEL( top_scol ), &base ) )
		return( FALSE );

	top_scol->known_private = TRUE;

	return( TRUE );
}

