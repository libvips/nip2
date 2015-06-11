/* a plot widget, plus some navigation stuff
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

#define TYPE_PLOTPRESENT (plotpresent_get_type())
#define PLOTPRESENT( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_PLOTPRESENT, Plotpresent ))
#define PLOTPRESENT_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_PLOTPRESENT, PlotpresentClass ))
#define IS_PLOTPRESENT( obj ) (GTK_CHECK_TYPE( (obj), TYPE_PLOTPRESENT ))
#define IS_PLOTPRESENT_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PLOTPRESENT ))

struct _Plotpresent {
	GtkBin parent_class;

	/* Context.
	 */
	Plotmodel *plotmodel;		/* Keep model parts of widgets here */

	/* Widgets.
	 */
	GtkWidget *canvas;

#ifdef HAVE_LIBGOFFICE
	GogRenderer *grend;
	GogChart *gchart;
	GogGraph *ggraph;
        GogPlot *gplot;
#endif /*HAVE_LIBGOFFICE*/
};

typedef struct _PlotpresentClass {
	GtkBinClass parent_class;

	/* My methods.
	 */

	/* A mouse movement within the plot area. xy are in axies coordinates.
	 */
	void (*mouse_move)( Plotpresent *, double, double );
} PlotpresentClass;

GType plotpresent_get_type( void );
Plotpresent *plotpresent_new( Plotmodel *plotmodel );

