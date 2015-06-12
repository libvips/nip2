/* the rhs of a tallyrow ... group together everything to the right of the
 * button
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

G_DEFINE_TYPE( Rhsview, rhsview, TYPE_VIEW ); 

/* Get this if ws->mode changes.
 */
static void
rhsview_reset( View *view )
{
	Rhsview *rhsview = RHSVIEW( view );
	Rhs *rhs = RHS( VOBJECT( rhsview )->iobject );
	Row *row = HEAPMODEL( rhs )->row;

	model_display( rhs->itext, 
		row->ws->mode == WORKSPACE_MODE_FORMULA || 
		rhs->flags & RHS_ITEXT );

	VIEW_CLASS( rhsview_parent_class )->reset( view );
}

static void 
rhsview_refresh( vObject *vobject )
{
	Rhsview *rhsview = RHSVIEW( vobject );
	Rhs *rhs = RHS( VOBJECT( rhsview )->iobject );
	Row *row = HEAPMODEL( rhs )->row;

#ifdef DEBUG
	printf( "rhsview_refresh: " );
	row_name_print( HEAPMODEL( rhs )->row );
	printf( " " );
	if( rhs->flags & RHS_GRAPHIC )
		printf( "RHS_GRAPHIC " );
	if( rhs->flags & RHS_SCOL )
		printf( "RHS_SCOL " );
	if( rhs->flags & RHS_ITEXT )
		printf( "RHS_ITEXT " );
	printf( "\n" );
#endif /*DEBUG*/

	/* Add/remove children according to rhs->flags. 
	 */
	model_display( rhs->graphic, rhs->flags & RHS_GRAPHIC );
	model_display( rhs->scol, rhs->flags & RHS_SCOL );

	switch( row->ws->mode ) {
	case WORKSPACE_MODE_REGULAR:
		model_display( rhs->itext, rhs->flags & RHS_ITEXT );
		break;

	case WORKSPACE_MODE_FORMULA:
		model_display( rhs->itext, TRUE );
		break;

	case WORKSPACE_MODE_NOEDIT:
		/* Only show the text if it's the only this we have for this
		 * row.
		 */
		if( rhs->graphic &&
			rhs->flags & RHS_GRAPHIC )
			model_display( rhs->itext, FALSE );
		else if( rhs->scol &&
			rhs->flags & RHS_SCOL )
			model_display( rhs->itext, FALSE );
		else
			model_display( rhs->itext, 
				rhs->flags & RHS_ITEXT );
		break;

	default:
		g_assert( 0 );
	}

	VOBJECT_CLASS( rhsview_parent_class )->refresh( vobject );
}

static void
rhsview_link( View *view, Model *model, View *parent )
{
	Rhsview *rhsview = RHSVIEW( view );
	Rowview *rview = ROWVIEW( parent );

#ifdef DEBUG
	printf( "rhsview_link: " );
	row_name_print( ROW( VOBJECT( rview )->iobject ) );
	printf( "\n" );
#endif /*DEBUG*/

	VIEW_CLASS( rhsview_parent_class )->link( view, model, parent );

	rhsview->rview = rview;
}

static void
rhsview_child_add( View *parent, View *child )
{
	Rhsview *rhsview = RHSVIEW( parent );

	if( IS_SUBCOLUMNVIEW( child ) ) {
		gtk_table_attach_defaults( GTK_TABLE( rhsview->table ),
			GTK_WIDGET( child ), 0, 1, 1, 2 );
		rhsview->scol = child;
	}
	else if( IS_ITEXTVIEW( child ) ) {
		gtk_table_attach_defaults( GTK_TABLE( rhsview->table ),
			GTK_WIDGET( child ), 0, 1, 2, 3 );
		rhsview->itext = child;
	}
	else {
		gtk_table_attach_defaults( GTK_TABLE( rhsview->table ),
			GTK_WIDGET( child ), 0, 1, 0, 1 );
		rhsview->graphic = child;
		g_assert( IS_GRAPHICVIEW( child ) );
	}

	VIEW_CLASS( rhsview_parent_class )->child_add( parent, child );
}

static void
rhsview_child_remove( View *parent, View *child )
{
	Rhsview *rhsview = RHSVIEW( parent );

	if( IS_SUBCOLUMNVIEW( child ) ) 
		rhsview->scol = NULL;
	else if( IS_ITEXTVIEW( child ) ) 
		rhsview->itext = NULL;
	else 
		rhsview->graphic = NULL;

	VIEW_CLASS( rhsview_parent_class )->child_remove( parent, child );
}

static void
rhsview_class_init( RhsviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass*) class;
	ViewClass *view_class = (ViewClass*) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = rhsview_refresh;

	view_class->link = rhsview_link;
	view_class->child_add = rhsview_child_add;
	view_class->child_remove = rhsview_child_remove;
	view_class->reset = rhsview_reset;
}

static void
rhsview_init( Rhsview *rhsview )
{
	rhsview->rview = NULL;

	/* Attached on refresh. 
	 */
	rhsview->graphic = NULL;
	rhsview->scol = NULL;
	rhsview->itext = NULL;

	rhsview->table = gtk_table_new( 3, 1, FALSE );
        gtk_box_pack_start( GTK_BOX( rhsview ), 
		rhsview->table, TRUE, FALSE, 0 );
        gtk_widget_show( rhsview->table );
	rhsview->flags = 0;

        gtk_widget_show( GTK_WIDGET( rhsview ) );
}

View *
rhsview_new( void )
{
	Rhsview *rhsview = gtk_type_new( TYPE_RHSVIEW );

	return( VIEW( rhsview ) );
}
