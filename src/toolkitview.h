/* View for toolkit.
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

#define TYPE_TOOLKITVIEW (toolkitview_get_type())
#define TOOLKITVIEW( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_TOOLKITVIEW, Toolkitview ))
#define TOOLKITVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_TOOLKITVIEW, ToolkitviewClass ))
#define IS_TOOLKITVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_TOOLKITVIEW ))
#define IS_TOOLKITVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_TOOLKITVIEW ))

struct _Toolkitview {
	View parent_class;

	Toolkitgroupview *kitgview;

	GtkWidget *menu;	/* Menu for this kit */
	GtkWidget *item;	/* Menu item in enclosing menu */
	guint destroy_sid;
};

typedef struct _ToolkitviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} ToolkitviewClass;

GtkType toolkitview_get_type( void );
View *toolkitview_new( void );
