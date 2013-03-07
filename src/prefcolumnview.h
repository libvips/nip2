/* view of a column in a preferences window
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

#define TYPE_PREFCOLUMNVIEW (prefcolumnview_get_type())
#define PREFCOLUMNVIEW( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_PREFCOLUMNVIEW, Prefcolumnview ))
#define PREFCOLUMNVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), \
		TYPE_PREFCOLUMNVIEW, PrefcolumnviewClass ))
#define IS_PREFCOLUMNVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_PREFCOLUMNVIEW ))
#define IS_PREFCOLUMNVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PREFCOLUMNVIEW ))

struct _Prefcolumnview {
	View view;

        /* Display parts.
         */
        GtkWidget *lab;               	/* Prefcolumnview name label */
};

typedef struct _PrefcolumnviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} PrefcolumnviewClass;

GtkType prefcolumnview_get_type( void );
View *prefcolumnview_new( void );
