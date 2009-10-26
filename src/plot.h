/* a plot in a workspace
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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

*/

#define TYPE_PLOT (plot_get_type())
#define PLOT( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_PLOT, Plot ))
#define PLOT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_PLOT, PlotClass))
#define IS_PLOT( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_PLOT ))
#define IS_PLOT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_PLOT ))
#define PLOT_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_PLOT, PlotClass ))

typedef enum {
	PLOT_FORMAT_YYYY = 0,
	PLOT_FORMAT_XYYY,
	PLOT_FORMAT_XYXY,
	PLOT_FORMAT_LAST
} PlotFormat;

typedef enum {
	PLOT_STYLE_POINT = 0,
	PLOT_STYLE_LINE,
	PLOT_STYLE_SPLINE,
	PLOT_STYLE_BAR,
	PLOT_STYLE_LAST
} PlotStyle;

/* Magic number for 'range value unset' (ie. should auto-range).
 */
#define PLOT_RANGE_UNSET (-999999)

struct _Plot {
	Classmodel model;

	/* Base class fields.
	 */
	ImageValue value;
	PlotFormat format;
	PlotStyle style;
	double xmin;
	double xmax;
	double ymin;
	double ymax;

	/* Unpack image to a set of xy columns here.
	 */
	double **xcolumn;
	double **ycolumn;
	int rows;
	int columns;

	/* Save x/y/mag/status here. Init plot windows from this, save and
	 * load from workspaces.
	 */
	gboolean show_status;
	int mag;
	int left, top;

	/* Private ... build iobject caption here.
	 */
	VipsBuf caption_buffer;
};

typedef struct _PlotClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} PlotClass;

GType plot_get_type( void );

char *plot_f2c( PlotFormat format );
char *plot_s2c( PlotStyle style );

#ifdef HAVE_LIBGOFFICE
GogPlot *plot_new_gplot( Plot *plot );
#endif /*HAVE_LIBGOFFICE*/

