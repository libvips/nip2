/* a view of a toolkitgroup
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

#define TYPE_TOOLKITGROUPVIEW (toolkitgroupview_get_type())
#define TOOLKITGROUPVIEW( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_TOOLKITGROUPVIEW, Toolkitgroupview ))
#define TOOLKITGROUPVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_TOOLKITGROUPVIEW, \
	ToolkitgroupviewClass ))
#define IS_TOOLKITGROUPVIEW( obj ) \
	(GTK_CHECK_TYPE( (obj), TYPE_TOOLKITGROUPVIEW ))
#define IS_TOOLKITGROUPVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_TOOLKITGROUPVIEW ))

struct _Toolkitgroupview {
	View parent_class;

	GtkWidget *menu;		/* Display the toolkits in this */
	Mainw *mainw;			/* Mainw these menu items act on */
};

typedef struct _ToolkitgroupviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} ToolkitgroupviewClass;

GtkType toolkitgroupview_get_type( void );
View *toolkitgroupview_new( void );
void toolkitgroupview_set_mainw( Toolkitgroupview *kitgview, Mainw *mainw );

