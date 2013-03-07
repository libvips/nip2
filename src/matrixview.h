/* a matrixview in a workspace
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

#define TYPE_MATRIXVIEW (matrixview_get_type())
#define MATRIXVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_MATRIXVIEW, Matrixview ))
#define MATRIXVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_MATRIXVIEW, MatrixviewClass ))
#define IS_MATRIXVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_MATRIXVIEW ))
#define IS_MATRIXVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_MATRIXVIEW ))

typedef struct _Matrixview {
	Graphicview parent_object;

	GtkWidget *box;			/* Top level hbox we lay out in */

	/* If we're displaying a matrix with a gtktreeview. 
	 */
	GtkListStore *store;
	GtkWidget *sheet;
	GtkWidget *swin;

	/* Displaying a table of widgets: sliders or toggles. 
	 */
	GtkWidget *table;		/* Matrix table */
	GSList *items; 			/* Widgets for elems */
	MatrixDisplayType display;	/* What's in items at the mo */
	int width;			/* Size of mat panel we have */
	int height;

	GtkWidget *cbox;		/* Convolution only: scale & offset */
	GtkWidget *scale;		
	GtkWidget *offset;
} Matrixview;

typedef struct _MatrixviewClass {
	GraphicviewClass parent_class;

	/* My methods.
	 */
} MatrixviewClass;

GtkType matrixview_get_type( void );
View *matrixview_new( void );
