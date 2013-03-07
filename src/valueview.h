/* a basic view of a model ... just show the caption
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

#define TYPE_VALUEVIEW (valueview_get_type())
#define VALUEVIEW( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_VALUEVIEW, Valueview ))
#define VALUEVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_VALUEVIEW, ValueviewClass ))
#define IS_VALUEVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_VALUEVIEW ))
#define IS_VALUEVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_VALUEVIEW ))

typedef struct _Valueview {
	Graphicview parent_object;

	GtkWidget *eb;
	GtkWidget *label;
} Valueview;

typedef struct _ValueviewClass {
	GraphicviewClass parent_class;

	/* My methods.
	 */
} ValueviewClass;

GtkType valueview_get_type( void );
View *valueview_new( void );
