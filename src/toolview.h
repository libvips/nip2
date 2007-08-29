/* View a tool as a menu item.
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

#define TYPE_TOOLVIEW (toolview_get_type())
#define TOOLVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_TOOLVIEW, Toolview ))
#define TOOLVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_TOOLVIEW, ToolviewClass ))
#define IS_TOOLVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_TOOLVIEW ))
#define IS_TOOLVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_TOOLVIEW ))

/* One of these for each top-level menu.
 */
struct _Toolview {
	View parent_class;

	Toolkitview *kview;

	GtkWidget *item;	/* Menu item we made for this tool */
};

typedef struct _ToolviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} ToolviewClass;

GtkType toolview_get_type( void );
View *toolview_new( void );
