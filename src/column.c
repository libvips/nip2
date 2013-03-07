/* a column button in a workspace
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

static FilemodelClass *parent_class = NULL;

/* Offset for this column load/save.
 */
static int column_left_offset = 0;
static int column_top_offset = 0;

static const int column_open_max_frames = 10;	/* Max frames we animate */

/* Map down a column.
 */
void *
column_map( Column *col, row_map_fn fn, void *a, void *b )
{
	Subcolumn *scol = col->scol;

	return( subcolumn_map( scol, fn, a, b ) );
}

void *
column_map_symbol_sub( Row *row, symbol_map_fn fn, void *a )
{
	return( fn( row->sym, a, NULL, NULL ) );
}

/* Map down a column, applying to the symbol of the row.
 */
void *
column_map_symbol( Column *col, symbol_map_fn fn, void *a )
{
	return( column_map( col, 
		(row_map_fn) column_map_symbol_sub, (void *) fn, a ) );
}

static void
column_finalize( GObject *gobject )
{
	Column *col;

#ifdef DEBUG
	printf( "column_finalize\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_COLUMN( gobject ) );

	col = COLUMN( gobject );
	IM_FREEF( g_source_remove, col->open_timeout );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

/* Select all things in a column.
 */
void *
column_select_symbols( Column *col )
{
	return( column_map( col, (row_map_fn) row_select_extend, NULL, NULL ) );
}

static Subcolumn *
column_get_subcolumn( Column *col )
{
	g_assert( g_slist_length( ICONTAINER( col )->children ) == 1 );

	return( SUBCOLUMN( ICONTAINER( col )->children->data ) );
}

static void
column_child_add( iContainer *parent, iContainer *child, int pos )
{
	Column *col = COLUMN( parent );

	ICONTAINER_CLASS( parent_class )->child_add( parent, child, pos );

	/* Update our context.
	 */
	col->scol = column_get_subcolumn( col );
}

static void
column_child_remove( iContainer *parent, iContainer *child )
{
	Column *col = COLUMN( parent );

	filemodel_set_modified( FILEMODEL( col->ws ), TRUE );

	ICONTAINER_CLASS( parent_class )->child_remove( parent, child );
}

static Workspace *
column_get_workspace( Column *col )
{
	return( WORKSPACE( ICONTAINER( col )->parent ) );
}

static void
column_parent_add( iContainer *child )
{
	Column *col = COLUMN( child );

	g_assert( IS_WORKSPACE( child->parent ) );

	ICONTAINER_CLASS( parent_class )->parent_add( child );

	/* Update our context.
	 */
	col->ws = column_get_workspace( col );
}

static View *
column_view_new( Model *model, View *parent )
{
	if( IS_PREFWORKSPACEVIEW( parent ) )
		return( prefcolumnview_new() );
	else
		return( columnview_new() );
}

static xmlNode *
column_save( Model *model, xmlNode *xnode )
{
	Column *col = COLUMN( model );
	int x = IM_MAX( 0, col->x - column_left_offset );
	int y = IM_MAX( 0, col->y - column_top_offset );

	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	/* Save sform for backwards compat with nip 7.8 ... now a workspace
	 * property.
	 */
	if( !set_prop( xthis, "x", "%d", x ) ||
		!set_prop( xthis, "y", "%d", y ) ||
		!set_sprop( xthis, "open", bool_to_char( col->open ) ) ||
		!set_sprop( xthis, "selected",
			bool_to_char( col->selected ) ) ||
		!set_sprop( xthis, "sform", bool_to_char( FALSE ) ) ||
		!set_prop( xthis, "next", "%d", col->next ) || 
		!set_sprop( xthis, "name", IOBJECT( col )->name ) )
		return( NULL );

	/* Caption can be NULL for untitled columns.
	 */
	if( IOBJECT( col )->caption )
		if( !set_sprop( xthis, "caption", IOBJECT( col )->caption ) ) 
			return( NULL );

	return( xthis );
}

static gboolean
column_save_test( Model *model )
{
	Column *col = COLUMN( model );
	Workspace *ws = col->ws;

	if( ws->save_type == WORKSPACE_SAVE_SELECTED ) 
		/* Only save columns containing selected rows.
		 */
		return( column_map( col, 
			(row_map_fn) row_is_selected, NULL, NULL ) != NULL );

	return( TRUE );
}

static gboolean
column_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	Column *col = COLUMN( model );
	int x = col->x;
	int y = col->y;

	char buf[256];

	g_assert( IS_WORKSPACE( parent ) );

	if( !get_iprop( xnode, "x", &x ) ||
		!get_iprop( xnode, "y", &y ) ||
		!get_bprop( xnode, "open", &col->open ) ||
		!get_bprop( xnode, "selected", &col->selected ) ||
		!get_iprop( xnode, "next", &col->next ) )
		return( FALSE );

	col->x = x + column_left_offset;
	col->y = y + column_top_offset;

	/* Don't use iobject_set(): we don't want to trigger _changed during
	 * load.
	 */
	if( get_sprop( xnode, "caption", buf, 256 ) ) {
		IM_SETSTR( IOBJECT( col )->caption, buf );
	}
	if( get_sprop( xnode, "name", buf, 256 ) ) {
		IM_SETSTR( IOBJECT( col )->name, buf );
	}

	return( MODEL_CLASS( parent_class )->load( model, 
		state, parent, xnode ) );
}

static void
column_class_init( ColumnClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	FilemodelClass *filemodel_class = (FilemodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = column_finalize;

	/* Create signals.
	 */

	/* Init methods.
	 */
	icontainer_class->child_add = column_child_add;
	icontainer_class->child_remove = column_child_remove;
	icontainer_class->parent_add = column_parent_add;

	model_class->view_new = column_view_new;
	model_class->save = column_save;
	model_class->save_test = column_save_test;
	model_class->load = column_load;

	filemodel_class->filetype = filesel_type_workspace;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
column_init( Column *col )
{
#ifdef DEBUG
	printf( "column_init\n" );
#endif /*DEBUG*/

	col->scol = NULL;
	col->ws = NULL;

        col->x = 0;
        col->y = 0;
        col->open = TRUE;
        col->selected = FALSE;

        col->next = 1;
        col->last_select = NULL;
}

GType
column_get_type( void )
{
	static GType column_type = 0;

	if( !column_type ) {
		static const GTypeInfo info = {
			sizeof( ColumnClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) column_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Column ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) column_init,
		};

		column_type = g_type_register_static( TYPE_FILEMODEL, 
			"Column", &info, 0 );
	}

	return( column_type );
}

Column *
column_new( Workspace *ws, const char *name )
{
	static int left = 0;
	static int top = 0;

	Column *col;

	if( workspace_column_find( ws, name ) ) {
		error_top( _( "Name clash." ) );
		error_sub( _( "Can't create column \"%s\". A column with that "
			"name already exists." ), name );
		return( NULL );
	}

	col = COLUMN( g_object_new( TYPE_COLUMN, NULL ) );
	iobject_set( IOBJECT( col ), name, NULL );
	icontainer_child_add( ICONTAINER( ws ), ICONTAINER( col ), -1 );

        subcolumn_new( NULL, col );

	/* Pick an initial position for it. 
	 */
	left += 10;
	top += 10;
	if( top > 200 )
		top = 0;
	col->x = left + ws->vp.left;
	col->y = top + ws->vp.top;

	return( col );
}

/* Find the bottom of the column.
 */
Row *
column_get_bottom( Column *col )
{
	Subcolumn *scol = col->scol;
	GSList *children = ICONTAINER( scol )->children;

	if( children ) {
		Row *row = ROW( g_slist_last( children )->data );

		return( row );
	}
	else {
		error_top( _( "Empty column." ) );
		error_sub( _( "There are no objects in the current column." ) );
		return( NULL );
	}
}

/* Add the last n names from a column to a buffer. Error if there are too few 
 * there.
 */
gboolean
column_add_n_names( Column *col, const char *name, VipsBuf *buf, int nparam )
{
	Subcolumn *scol = col->scol;
	GSList *children = ICONTAINER( scol )->children;
	int len = g_slist_length( children );
	GSList *i;

	g_assert( nparam >= 0 );

	if( nparam > 0 && nparam > len ) {
		error_top( _( "Too few items." ) );
		error_sub( _( "This column only has %d items, "
			"but %s needs %d items." ), len, name, nparam );
		return( FALSE );
	}

	for( i = g_slist_nth( children, len - nparam ); i; i = i->next ) {
		Row *row = ROW( i->data );

		vips_buf_appends( buf, " " );
		vips_buf_appends( buf, IOBJECT( row->sym )->name );
	}

	return( TRUE );
}

/* Is a column empty?
 */
gboolean
column_is_empty( Column *col )
{
	Subcolumn *scol = col->scol;
	GSList *children = ICONTAINER( scol )->children;

	return( children == NULL );
}

/* Set the load/save offsets.
 */
void
column_set_offset( int x_off, int y_off )
{
#ifdef DEBUG
	printf( "column_set_offset: load offset %d x %d\n", x_off, y_off );
#endif /*DEBUG*/

	column_left_offset = x_off;
	column_top_offset = y_off;
}

char *
column_name_new( Column *col )
{
	char buf[256];

	do {
		im_snprintf( buf, 256, "%s%d", 
			IOBJECT( col )->name, col->next++ );
	} while( compile_lookup( col->ws->sym->expr->compile, buf ) );

	return( im_strdup( NULL, buf ) );
}

static Row *
column_open_find( Row *row, Column *col )
{
	if( MODEL( row )->display != col->open )
		return( row );

	return( NULL );
}

static gboolean
column_open_timeout_cb( Column *col )
{
	void *(*map_fn)( iContainer *, icontainer_map_fn, void *, void * );
	Row *row;
	gboolean again = TRUE;

	if( !col->scol ) {
		col->open_timeout = 0;
		return( FALSE );
	}

	if( col->open ) 
		map_fn = icontainer_map_rev;
	else
		map_fn = icontainer_map;

	while( (row = ROW( map_fn( ICONTAINER( col->scol ),
		(icontainer_map_fn) column_open_find, col, NULL ) )) ) {
		model_set_display( MODEL( row ), col->open );
		col->open_frames += 1;

		if( col->open_frames < column_open_max_frames )
			break;
	}

	if( !row ) {
		col->open_timeout = 0;
		again = FALSE;
	}

	return( again );
}

void 
column_set_open( Column *col, gboolean open )
{
        if( col->open != open ) {
		Workspace *ws = col->ws;

		col->open = open;
		filemodel_set_modified( FILEMODEL( ws ), TRUE );
		iobject_changed( IOBJECT( col ) );

		if( !col->open_timeout ) {
			col->open_frames = 0;
			col->open_timeout = g_timeout_add( 20, 
				(GSourceFunc) column_open_timeout_cb, col );
		}
	}
}
