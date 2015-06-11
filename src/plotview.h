/* a plotview in a workspace
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

#define TYPE_PLOTVIEW (plotview_get_type())
#define PLOTVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_PLOTVIEW, Plotview ))
#define PLOTVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_PLOTVIEW, PlotviewClass ))
#define IS_PLOTVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_PLOTVIEW ))
#define IS_PLOTVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PLOTVIEW ))

typedef struct _Plotview {
	Graphicview parent_object;

	GtkWidget *box;
	GtkWidget *label;
	GtkWidget *canvas;

	GogChart *gchart;
        GogPlot *gplot;
} Plotview;

typedef struct _PlotviewClass {
	GraphicviewClass parent_class;

	/* My methods.
	 */
} PlotviewClass;

GType plotview_get_type( void );
View *plotview_new( void );
