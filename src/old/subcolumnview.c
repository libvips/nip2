/* a subcolumnview button in a workspace
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

G_DEFINE_TYPE( Subcolumnview, subcolumnview, TYPE_VIEW ); 

static void *
subcolumnview_destroy_sub( Rowview *rview, Subcolumnview *sview )
{
	DESTROY_GTK( rview ); 

	return( NULL );
}

static void
subcolumnview_destroy( GtkWidget *widget )
{
	Subcolumnview *sview;

#ifdef DEBUG
	printf( "subcolumnview_destroy\n" );
#endif /*DEBUG*/

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_SUBCOLUMNVIEW( widget ) );

	sview = SUBCOLUMNVIEW( widget );

	UNREF( sview->group );

	/* Destroying us won't automatically destroy our rowviews, since they
	 * are not true child-widgets. Do it by hand. 
	 */
	(void) view_map( VIEW( sview ), 
		(view_map_fn) subcolumnview_destroy_sub, sview, NULL );
	DESTROY_GTK( sview->grid );

	GTK_WIDGET_CLASS( subcolumnview_parent_class )->destroy( widget );
}

static void
subcolumnview_link( View *view, Model *model, View *parent )
{
	Subcolumnview *sview = SUBCOLUMNVIEW( view );
	Subcolumn *scol = SUBCOLUMN( model );

#ifdef DEBUG
	printf( "subcolumnview_link: " );
	if( HEAPMODEL( scol )->row )
		row_name_print( HEAPMODEL( scol )->row );
	else
		printf( "(null)" );
	printf( "\n" );
#endif /*DEBUG*/

	VIEW_CLASS( subcolumnview_parent_class )->link( view, model, parent );

	/* Add to enclosing column, if there is one. Attached to enclosing row
	 * by rowview_refresh() if we're a subcolumn.
	 */
	if( !scol->is_top ) 
		sview->rhsview = RHSVIEW( parent );

	gtk_widget_show( GTK_WIDGET( sview ) );
}

static void *
subcolumnview_refresh_sub( Rowview *rview, Subcolumnview *sview )
{
	Subcolumn *scol = SUBCOLUMN( VOBJECT( sview )->iobject );
	Row *row = ROW( VOBJECT( rview )->iobject );
	int i;

	/* Most predicates need a sym.
	 */
	if( !row->sym )
		return( NULL );

	for( i = 0; i <= scol->vislevel; i++ )
		if( subcolumn_visibility[i].pred( row ) ) {
			rowview_set_visible( rview, TRUE );
			sview->nvis++;
			break;
		}
	if( i > scol->vislevel )
		rowview_set_visible( rview, FALSE );

	return( NULL );
}

static void 
subcolumnview_refresh( vObject *vobject )
{
	Subcolumnview *sview = SUBCOLUMNVIEW( vobject );
	Subcolumn *scol = SUBCOLUMN( VOBJECT( sview )->iobject );
	int old_nvis = sview->nvis;
	gboolean editable = scol->top_col->ws->mode != WORKSPACE_MODE_NOEDIT;

#ifdef DEBUG
	printf( "subcolumnview_refresh\n" );
#endif /*DEBUG*/

	/* Top-level subcolumns look different in no-edit mode.
	 */
	if( scol->is_top && editable ) {
		gtk_grid_set_row_spacing( GTK_GRID( sview->grid ), 0 );
		gtk_grid_set_column_spacing( GTK_GRID( sview->grid ), 0 );
	}
	else if( scol->is_top && !editable ) {
		gtk_grid_set_row_spacing( GTK_GRID( sview->grid ), 5 );
		gtk_grid_set_column_spacing( GTK_GRID( sview->grid ), 5 );
	}

	/* Nested subcols: we just change the left indent.
	 */
	if( !scol->is_top && editable ) {
		printf( "subcolumnview_refresh: set indent\n" ); 
	}
	else if( !scol->is_top && !editable ) {
		printf( "subcolumnview_refresh: set indent\n" ); 
	}

	sview->nvis = 0;
	(void) view_map( VIEW( sview ), 
		(view_map_fn) subcolumnview_refresh_sub, sview, NULL );

	if( sview->nvis != old_nvis ) {
		view_resize( VIEW( sview ) );
		iobject_changed( IOBJECT( scol->top_col ) );
	}

	VOBJECT_CLASS( subcolumnview_parent_class )->refresh( vobject );
}

static void
subcolumnview_class_init( SubcolumnviewClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass*) class;
	vObjectClass *vobject_class = (vObjectClass*) class;
	ViewClass *view_class = (ViewClass*) class;

	widget_class->destroy = subcolumnview_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = subcolumnview_refresh;

	view_class->link = subcolumnview_link;
}

static void
subcolumnview_init( Subcolumnview *sview )
{
	sview->rhsview = NULL;

        sview->rows = 0;
        sview->nvis = 0;


        sview->grid = gtk_grid_new();
        gtk_box_pack_start( GTK_BOX( sview ), sview->grid, FALSE, FALSE, 0 );

        gtk_widget_show_all( sview->grid );

	sview->group = gtk_size_group_new( GTK_SIZE_GROUP_HORIZONTAL );
}

View *
subcolumnview_new( void )
{
	Subcolumnview *sview = g_object_new( TYPE_SUBCOLUMNVIEW, NULL );

	return( VIEW( sview ) );
}
