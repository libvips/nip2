/* abstract base class for text editable view widgets
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

#define TYPE_EDITVIEW (editview_get_type())
#define EDITVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_EDITVIEW, Editview ))
#define EDITVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_EDITVIEW, EditviewClass ))
#define IS_EDITVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_EDITVIEW ))
#define IS_EDITVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_EDITVIEW ))

typedef struct _Editview {
	Graphicview parent_object;

	/* Widgets.
	 */
        GtkWidget *label;		/* Display caption here */
        GtkWidget *text;		/* Edit value here */
} Editview;

typedef struct _EditviewClass {
	GraphicviewClass parent_class;

	/* My methods.
	 */
} EditviewClass;

GtkType editview_get_type( void );
void editview_set_entry( Editview *editview, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
