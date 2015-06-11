/* a colourview in a workspace
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

#define TYPE_COLOURVIEW (colourview_get_type())
#define COLOURVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_COLOURVIEW, Colourview ))
#define COLOURVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_COLOURVIEW, ColourviewClass ))
#define IS_COLOURVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_COLOURVIEW ))
#define IS_COLOURVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_COLOURVIEW ))

typedef struct _Colourview {
	Graphicview parent_object;

	Colourdisplay *colourdisplay;
	Conversion *conv;
	GtkWidget *label;
} Colourview;

typedef struct _ColourviewClass {
	GraphicviewClass parent_class;

	/* My methods.
	 */
} ColourviewClass;

GType colourview_get_type( void );
View *colourview_new( void );
