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

G_DEFINE_TYPE( Popupbutton, popupbutton, GTK_TYPE_TOGGLE_BUTTON ); 

static void
popupbutton_class_init( PopupbuttonClass *class )
{
}

static void
popupbutton_init( Popupbutton *popupbutton )
{
	popupbutton->menu = NULL;
}

static void
popupbutton_over_arrow( Popupbutton *popupbutton, GdkEventButton *event )
{
	GtkWidget *menu = popupbutton->menu;

	gtk_menu_popup_at_widget( GTK_MENU( menu ),
		GTK_WIDGET( popupbutton ),
		GDK_GRAVITY_SOUTH_WEST,
		GDK_GRAVITY_NORTH_WEST,
		NULL );
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

	image = gtk_image_new_from_icon_name( "execute", 
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
