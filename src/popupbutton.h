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

#define TYPE_POPUPBUTTON (popupbutton_get_type())
#define POPUPBUTTON( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_POPUPBUTTON, Popupbutton ))
#define POPUPBUTTON_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_POPUPBUTTON, PopupbuttonClass ))
#define IS_POPUPBUTTON( obj ) (GTK_CHECK_TYPE( (obj), TYPE_POPUPBUTTON ))
#define IS_POPUPBUTTON_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_POPUPBUTTON ))

typedef struct _Popupbutton {
	GtkToggleButton parent_object;

	GtkWidget *menu;
} Popupbutton;

typedef struct _PopupbuttonClass {
	GtkToggleButtonClass parent_class;

} PopupbuttonClass;

GType popupbutton_get_type( void );
Popupbutton *popupbutton_new( void );
void popupbutton_set_menu( Popupbutton *Popupbutton, GtkWidget *menu );
