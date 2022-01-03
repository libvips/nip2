/* the model parts of a plot window .. all the window components watch this
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

#define TYPE_PLOTMODEL (plotmodel_get_type())
#define PLOTMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_PLOTMODEL, Plotmodel ))
#define PLOTMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_PLOTMODEL, PlotmodelClass ))
#define IS_PLOTMODEL( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_PLOTMODEL ))
#define IS_PLOTMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_PLOTMODEL ))

struct _Plotmodel {
	iObject parent_class;

	/* The class model we watch.
	 */
	Plot *plot;
	guint changed_sid;
	guint destroy_sid;

	/* The last canvas size we set ... stop resize loops with these.
	 */
	int width;
	int height;

	/* Viewer state.
	 */
	int mag;
	gboolean show_status;
};

typedef struct _PlotmodelClass {
	iObjectClass parent_class;

	/* My methods.
	 */
} PlotmodelClass;

GType plotmodel_get_type( void );
Plotmodel *plotmodel_new( Plot *plot );
void plotmodel_set_mag( Plotmodel *plotmodel, int mag );
void plotmodel_set_status( Plotmodel *plotmodel, gboolean show_status );
