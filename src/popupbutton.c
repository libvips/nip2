/* a button that displays a popup menu
 *
 * quick hack from totem-plugin-viewer.c
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
 */

#include "ip.h"

static GtkToggleButtonClass *popupbutton_parent_class = NULL;

static void
popupbutton_destroy( GtkObject *object )
{
	Popupbutton *popupbutton;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_POPUPBUTTON( object ) );

	popupbutton = POPUPBUTTON( object );

	VIPS_UNREF( popupbutton->menu );

	GTK_OBJECT_CLASS( popupbutton_parent_class )->destroy( object );
}

static void
popupbutton_class_init( PopupbuttonClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	popupbutton_parent_class = g_type_class_peek_parent( class );

	object_class->destroy = popupbutton_destroy;
}

static void
popupbutton_init( Popupbutton *popupbutton )
{
	popupbutton->menu = NULL;
}

GType
popupbutton_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( PopupbuttonClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) popupbutton_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Popupbutton ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) popupbutton_init,
		};

		type = g_type_register_static( GTK_TYPE_TOGGLE_BUTTON, 
			"Popupbutton", &info, 0 );
	}

	return( type );
}

static void
popupbutton_position_func( GtkMenu *menu, 
	gint *x, gint *y, gboolean *push_in, GtkWidget *button )
{
	GtkRequisition menu_req;
	GtkTextDirection direction;
	GtkAllocation allocation;

	gtk_widget_size_request( GTK_WIDGET( menu ), &menu_req);

	direction = gtk_widget_get_direction( button );

	gdk_window_get_origin( gtk_widget_get_window( button ), x, y );
	gtk_widget_get_allocation( button, &allocation );
	*x += allocation.x;
	*y += allocation.y;

	if( direction == GTK_TEXT_DIR_LTR )
		*x += VIPS_MAX( allocation.width - menu_req.width, 0 );
	else if( menu_req.width > allocation.width )
		*x -= menu_req.width - allocation.width;

	/* This might not work properly if the popup button is right at the
	 * top of the screen, but really, what are the chances 
	 */
	*y -= menu_req.height;

	*push_in = FALSE;
}

static void
popupbutton_over_arrow( Popupbutton *popupbutton, GdkEventButton *event )
{
	GtkWidget *menu = popupbutton->menu;

	gtk_menu_popup( GTK_MENU( menu ), NULL, NULL, 
		(GtkMenuPositionFunc) popupbutton_position_func,
		popupbutton,
		event ? event->button : 0,
		event ? event->time : gtk_get_current_event_time() );
}

static void
popupbutton_toggled_cb( Popupbutton *popupbutton )
{
	GtkWidget *menu = popupbutton->menu;

	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( popupbutton ) ) && 
		!gtk_widget_get_visible( menu ) ) {
		/* We get here only when the menu is activated by a key
		 * press, so that we can select the first menu item.
		 */
		popupbutton_over_arrow( popupbutton, NULL );
		gtk_menu_shell_select_first( GTK_MENU_SHELL( menu ), FALSE );
	}
}

static gboolean
popupbutton_button_press_event_cb( Popupbutton *popupbutton,
	GdkEventButton *event )
{
	if( event->button == 1 ) {
		GtkWidget *menu = popupbutton->menu;

		if( !gtk_widget_get_visible( menu ) ) {
			popupbutton_over_arrow( popupbutton, event );
			gtk_toggle_button_set_active( 
				GTK_TOGGLE_BUTTON( popupbutton ), TRUE );
		} 
		else {
			gtk_menu_popdown( GTK_MENU( menu ) );
			gtk_toggle_button_set_active( 
				GTK_TOGGLE_BUTTON( popupbutton ), FALSE );
		}

		return TRUE;
	}

	return FALSE;
}

Popupbutton *
popupbutton_new( void )
{
	Popupbutton *popupbutton;
	GtkWidget *image;

	popupbutton = g_object_new( TYPE_POPUPBUTTON, NULL );

	image = gtk_image_new_from_stock( GTK_STOCK_EXECUTE, 
		GTK_ICON_SIZE_MENU );
	gtk_container_add( GTK_CONTAINER( popupbutton ), image );
	gtk_widget_show( image );

	g_signal_connect( popupbutton, "toggled",
		G_CALLBACK( popupbutton_toggled_cb ), NULL );
	g_signal_connect( popupbutton, "button-press-event",
		G_CALLBACK( popupbutton_button_press_event_cb ), NULL );

	return( popupbutton );
}

static void
popupbutton_menu_unmap_cb( GtkWidget *menu,
		     Popupbutton *popupbutton )
{
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( popupbutton ), FALSE );
}

void
popupbutton_set_menu( Popupbutton *popupbutton, GtkWidget *menu )
{
	g_assert( !popupbutton->menu );

	popupbutton->menu = menu;

	g_signal_connect( menu, "unmap",
		G_CALLBACK( popupbutton_menu_unmap_cb ), popupbutton );
}
