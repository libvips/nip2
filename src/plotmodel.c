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

/*
#define DEBUG
 */

#include "ip.h"

static iObjectClass *parent_class = NULL;

static void
plotmodel_dispose( GObject *gobject )
{
	Plotmodel *plotmodel;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_PLOTMODEL( gobject ) );

	plotmodel = PLOTMODEL( gobject );

#ifdef DEBUG
	printf( "plotmodel_dispose %p: ", plotmodel );
	iobject_print( IOBJECT( plotmodel ) );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	FREESID( plotmodel->changed_sid, plotmodel->plot );
	FREESID( plotmodel->destroy_sid, plotmodel->plot );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
plotmodel_finalize( GObject *gobject )
{
#ifdef DEBUG
	Plotmodel *plotmodel = PLOTMODEL( gobject );

	printf( "plotmodel_finalize: %p\n", plotmodel );
#endif /*DEBUG*/

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
plotmodel_changed( iObject *iobject )
{
	Plotmodel *plotmodel = PLOTMODEL( iobject );

#ifdef DEBUG
	printf( "plotmodel_changed:\n" );
#endif /*DEBUG*/

	prefs_set( "DISPLAY_STATUS", 
		"%s", bool_to_char( plotmodel->show_status ) );

	IOBJECT_CLASS( parent_class )->changed( iobject );
}

static void
plotmodel_class_init( PlotmodelClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = plotmodel_dispose;
	gobject_class->finalize = plotmodel_finalize;

	iobject_class->changed = plotmodel_changed;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
plotmodel_init( Plotmodel *plotmodel )
{
#ifdef DEBUG
	printf( "plotmodel_init: %p\n", plotmodel );
#endif /*DEBUG*/

	plotmodel->changed_sid = 0;
	plotmodel->destroy_sid = 0;
	plotmodel->width = -1;
	plotmodel->height = -1;
	plotmodel->mag = 100;
	plotmodel->show_status = DISPLAY_STATUS;
}

GType
plotmodel_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( PlotmodelClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) plotmodel_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Plotmodel ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) plotmodel_init,
		};

		type = g_type_register_static( TYPE_IOBJECT, 
			"Plotmodel", &info, 0 );
	}

	return( type );
}

static void
plotmodel_plot_destroy_cb( Plot *plot, Plotmodel *plotmodel )
{
	plotmodel->plot = NULL;
	plotmodel->destroy_sid = 0;
	plotmodel->changed_sid = 0;
}

static void
plotmodel_plot_changed_cb( Plot *plot, Plotmodel *plotmodel )
{
	iobject_changed( IOBJECT( plotmodel ) );
}

static void
plotmodel_link( Plotmodel *plotmodel, Plot *plot )
{
	/* Don't need to listen for "destroy": our enclosing Floatwindow does
	 * that.
	 */
	plotmodel->plot = plot;
	plotmodel->destroy_sid = g_signal_connect( 
		G_OBJECT( plot ), "destroy", 
		G_CALLBACK( plotmodel_plot_destroy_cb ), plotmodel );
	plotmodel->changed_sid = g_signal_connect( 
		G_OBJECT( plot ), "changed", 
		G_CALLBACK( plotmodel_plot_changed_cb ), plotmodel );
}

Plotmodel *
plotmodel_new( Plot *plot )
{
	Plotmodel *plotmodel = g_object_new( TYPE_PLOTMODEL, NULL );

	plotmodel_link( plotmodel, plot );

	return( plotmodel );
}

void
plotmodel_set_mag( Plotmodel *plotmodel, int mag )
{
	/* Don't let mag get too large or small. GtkPlotCanvas does not
	 * display large magnifications at all well.
	 */
	mag = IM_CLIP( 100, mag, 800 );

	if( plotmodel->mag != mag ) {
#ifdef DEBUG
		printf( "plotmodel_set_mag: %d\n", mag );
#endif /*DEBUG*/

		plotmodel->mag = mag;

		/* Invaidate width so the canvas is regenerated.
		 */
		plotmodel->width = -1;

		iobject_changed( IOBJECT( plotmodel ) );
	}
}

void
plotmodel_set_status( Plotmodel *plotmodel, gboolean show_status )
{
	if( plotmodel->show_status != show_status ) {
#ifdef DEBUG
		printf( "plotmodel_set_status: %d\n", show_status );
#endif /*DEBUG*/

		plotmodel->show_status = show_status;

		iobject_changed( IOBJECT( plotmodel ) );
	}
}
