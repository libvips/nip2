/* Manage toolkitviews and their display.
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

/* The top n items in the toolkits menu are made by the system for us ... we
 * pop toolkit items in after these.
 */
#define TOOLKITVIEW_MENU_OFFSET 3

G_DEFINE_TYPE( Toolkitview, toolkitview, TYPE_VIEW ); 

static void 
toolkitview_destroy( GtkWidget *widget )
{	
	Toolkitview *kview;

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_TOOLKITVIEW( widget ) );

	kview = TOOLKITVIEW( widget );

#ifdef DEBUG
	printf( "toolkitview_destroy: %p\n", object );
	printf( "toolkitview_destroy: menu = %p\n", kview->menu );
	printf( "toolkitview_destroy: item = %p\n", kview->item );
#endif /*DEBUG*/

	DESTROY_GTK( kview->menu );
	DESTROY_GTK( kview->item );

	GTK_WIDGET_CLASS( toolkitview_parent_class )->destroy( widget );
}

static void
toolkitview_finalize( GObject *gobject )
{
#ifdef DEBUG
	printf( "toolkitview_finalize: %p\n", gobject );
#endif /*DEBUG*/

	G_OBJECT_CLASS( toolkitview_parent_class )->finalize( gobject );
}

/* Our widgets have been killed ... kill us in turn.
 */
static void
toolkitview_destroy_cb( GtkWidget *widget, Toolkitview *kview )
{
	/*
	printf( "toolkitview_destroy_cb: %p\n", kview );
	 */

	kview->menu = NULL;
	kview->item = NULL;
	kview->destroy_sid = 0;

	DESTROY_GTK( kview );
}

static void
toolkitview_refresh( vObject *vobject )
{
	Toolkitview *kview = TOOLKITVIEW( vobject );
	Toolkit *kit = TOOLKIT( VOBJECT( kview )->iobject );
	Toolkitgroupview *kitgview = kview->kitgview;
	GtkWidget *menu = kitgview->menu;
	gboolean changed = FALSE;

#ifdef DEBUG
	printf( "toolkitview_refresh: " );
	iobject_print( VOBJECT( kview )->iobject );
#endif /*DEBUG*/

	/* Make a button ready for the sub-menu. 
	 */
	if( !kview->item ) {
                kview->item = gtk_menu_item_new_with_label( 
			IOBJECT( kit )->name );

                gtk_menu_shell_insert( GTK_MENU_SHELL( menu ),
			kview->item, 
			ICONTAINER( kit )->pos + TOOLKITVIEW_MENU_OFFSET );
                gtk_widget_show( kview->item );
		kview->destroy_sid = g_signal_connect( kview->item, 
			"destroy",
			G_CALLBACK( toolkitview_destroy_cb ), kview );

		changed = TRUE;
	}
	if( !kview->menu ) {
		iWindow *iwnd = IWINDOW( iwindow_get_root( menu ) );
		char path[256];

		kview->menu = gtk_menu_new();
		gtk_menu_set_accel_group( GTK_MENU( kview->menu ),
			iwnd->accel_group );
		im_snprintf( path, 256, 
			"<mainw>/Toolkits/%s", IOBJECT( kit )->name );
		/* FIXME
		gtk_menu_set_accel_path( GTK_MENU( kview->menu ), path );
		 */

		changed = TRUE;
	}

	if( changed )
		gtk_menu_item_set_submenu( GTK_MENU_ITEM( kview->item ), 
			kview->menu );

	widget_visible( kview->item, ICONTAINER( kit )->children != NULL );

	VOBJECT_CLASS( toolkitview_parent_class )->refresh( vobject );
}

static void
toolkitview_link( View *view, Model *model, View *parent )
{
	Toolkitview *kview = TOOLKITVIEW( view );
	Toolkitgroupview *kitgview = TOOLKITGROUPVIEW( parent );

	kview->kitgview = kitgview;

	VIEW_CLASS( toolkitview_parent_class )->link( view, model, parent );

#ifdef DEBUG
	printf( "toolkitview_link: " );
	iobject_print( VOBJECT( kview )->iobject );
#endif /*DEBUG*/
}

static void
toolkitview_class_init( ToolkitviewClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	gobject_class->finalize = toolkitview_finalize;

	widget_class->destroy = toolkitview_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = toolkitview_refresh;

	view_class->link = toolkitview_link;
}

static void
toolkitview_init( Toolkitview *kview )
{
        kview->item = NULL;
        kview->menu = NULL;
        kview->destroy_sid = 0;
}

View *
toolkitview_new( void )
{
	Toolkitview *kview = g_object_new( TYPE_TOOLKITVIEW, NULL );

	return( VIEW( kview ) );
}

