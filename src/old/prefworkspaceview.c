/* a prefworkspaceview button in a workspace
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

/* Define to trace button press events.
#define EVENT
 */

#include "ip.h"

G_DEFINE_TYPE( Prefworkspaceview, prefworkspaceview, TYPE_VIEW ); 

static void
prefworkspaceview_destroy( GtkWidget *widget )
{
	Prefworkspaceview *pwview;

#ifdef DEBUG
	printf( "prefworkspaceview_destroy\n" );
#endif /*DEBUG*/

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_PREFWORKSPACEVIEW( widget ) );

	pwview = PREFWORKSPACEVIEW( widget );

	/* Instance destroy.
	 */
	IM_FREE( pwview->caption_filter );

	GTK_WIDGET_CLASS( prefworkspaceview_parent_class )->destroy( widget );
}

static void
prefworkspaceview_child_add( View *parent, View *child )
{
	Prefworkspaceview *pwview = PREFWORKSPACEVIEW( parent );

	VIEW_CLASS( prefworkspaceview_parent_class )->child_add( parent, child );

	gtk_box_pack_end( GTK_BOX( pwview ),
		GTK_WIDGET( child ), FALSE, FALSE, 0 );
}

/* Should a child model have a display?
 */
static gboolean
prefworkspaceview_display( View *parent, Model *child )
{
	Prefworkspaceview *pwview = PREFWORKSPACEVIEW( parent );
	Column *column = COLUMN( child );

	if( pwview->caption_filter ) 
		return( strstr( IOBJECT( column )->caption, 
			pwview->caption_filter ) != NULL );
	else
		return( TRUE );
}

static void
prefworkspaceview_class_init( PrefworkspaceviewClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	widget_class->destroy = prefworkspaceview_destroy;

	view_class->child_add = prefworkspaceview_child_add;
	view_class->display = prefworkspaceview_display;
}

static void
prefworkspaceview_init( Prefworkspaceview *pwview )
{
	pwview->caption_filter = NULL; 
}

View *
prefworkspaceview_new( void )
{
	Prefworkspaceview *pwview = 
		g_object_new( TYPE_PREFWORKSPACEVIEW, NULL );

	return( VIEW( pwview ) );
}

void 
prefworkspaceview_set_caption_filter( Prefworkspaceview *pwview, 
	const char *caption_filter )
{
	IM_SETSTR( pwview->caption_filter, caption_filter );

	/* caption_filter is a property of the view, not the model, so we have
	 * to queue a refresh rather than just signalling change.
	 */
	vobject_refresh_queue( VOBJECT( pwview ) );
}
