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

/*
#define DEBUG_EVENT
#define DEBUG
 */

#include "ip.h"

#ifdef HAVE_LIBGOFFICE

static GtkBinClass *parent_class = NULL;

enum {
	SIG_MOUSE_MOVE,		/* mose drag, axies cods */
	SIG_LAST
};

static guint plotpresent_signals[SIG_LAST] = { 0 };

static void
plotpresent_mouse_move( Plotpresent *plotpresent, double x, double y )
{
	g_signal_emit( G_OBJECT( plotpresent ), 
		plotpresent_signals[SIG_MOUSE_MOVE], 0, x, y );
}

static void
plotpresent_destroy( GtkObject *object )
{
	Plotpresent *plotpresent;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PLOTPRESENT( object ) );

	plotpresent = PLOTPRESENT( object );

#ifdef DEBUG
	printf( "plotpresent_destroy: %p\n", plotpresent );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	UNREF( plotpresent->grend );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
plotpresent_size_request( GtkWidget *widget, GtkRequisition *requisition )
{
	GtkBin *bin = GTK_BIN( widget );

	if( bin->child && GTK_WIDGET_VISIBLE( bin->child ) ) 
		gtk_widget_size_request( bin->child, requisition );
}

static void
plotpresent_size_allocate( GtkWidget *widget, GtkAllocation *allocation )
{
	GtkBin *bin = GTK_BIN( widget );

	if( bin->child && GTK_WIDGET_VISIBLE( bin->child ) ) 
		gtk_widget_size_allocate( bin->child, allocation );
}

/* Spot mouse motion events, to update status bar.
 */
static gboolean
plotpresent_motion_notify_event( GtkWidget *widget, GdkEventMotion *event ) 
{
	Plotpresent *plotpresent = PLOTPRESENT( widget );
	GtkAllocation *allocation = 
		&GTK_WIDGET( plotpresent->canvas )->allocation;

	GogView *view;
	GSList *axes;
	GogAxis *x_axis;
	GogAxis *y_axis;
	GogChartMap *map;

#ifdef DEBUG_EVENT
	printf( "plotpresent_motion_notify_event: %p\n", plotpresent );
	printf( "event->x = %g, event->y = %g\n", event->x, event->y );
#endif /*DEBUG_EVENT*/

	gog_renderer_update( plotpresent->grend, 
		allocation->width, allocation->height );

	g_object_get( G_OBJECT( plotpresent->grend ), "view", &view, NULL );
	view = gog_view_find_child_view( view, 
		GOG_OBJECT( plotpresent->gplot ) );

	axes = gog_chart_get_axes( plotpresent->gchart, GOG_AXIS_X );
	x_axis = GOG_AXIS( axes->data );
	g_slist_free( axes );

	axes = gog_chart_get_axes( plotpresent->gchart, GOG_AXIS_Y );
	y_axis = GOG_AXIS( axes->data );
	g_slist_free( axes );

	map = gog_chart_map_new( plotpresent->gchart, &(view->allocation),
		x_axis, y_axis, NULL, FALSE );

	if( gog_chart_map_is_valid( map ) &&
		event->x >= view->allocation.x && 
		event->x < view->allocation.x + view->allocation.w &&
		event->y >= view->allocation.y && 
		event->y < view->allocation.y + view->allocation.h ) {
		GogAxisMap *x_map;
		GogAxisMap *y_map;
		double x;
		double y;

		x_map = gog_chart_map_get_axis_map( map, 0 );
		y_map = gog_chart_map_get_axis_map( map, 1 );
		x = gog_axis_map_from_view( x_map, event->x );
		y = gog_axis_map_from_view( y_map, event->y );

		plotpresent_mouse_move( plotpresent, x, y );
	}

        gog_chart_map_free( map );

	return( FALSE );
}

static void
plotpresent_class_init( PlotpresentClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = plotpresent_destroy;

        widget_class->size_request = plotpresent_size_request;
	widget_class->size_allocate = plotpresent_size_allocate;

	widget_class->motion_notify_event = plotpresent_motion_notify_event;

	/* Create signals.
	 */
	plotpresent_signals[SIG_MOUSE_MOVE] = g_signal_new( "mouse_move",
		G_OBJECT_CLASS_TYPE( class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( PlotpresentClass, mouse_move ),
		NULL, NULL,
		nip_VOID__DOUBLE_DOUBLE,
		G_TYPE_NONE, 2,
		G_TYPE_DOUBLE, G_TYPE_DOUBLE );

	/* Init methods.
	 */
}

static void
plotpresent_init( Plotpresent *plotpresent )
{
#ifdef DEBUG
	printf( "plotpresent_init: %p\n", plotpresent );
#endif /*DEBUG*/

	plotpresent->gplot = NULL;

	plotpresent->canvas = go_graph_widget_new( NULL );
        gtk_container_add( GTK_CONTAINER( plotpresent ), plotpresent->canvas );
	gtk_widget_show( GTK_WIDGET( plotpresent->canvas ) );

	plotpresent->ggraph = go_graph_widget_get_graph( 
		GO_GRAPH_WIDGET( plotpresent->canvas ) );
	plotpresent->gchart = go_graph_widget_get_chart( 
		GO_GRAPH_WIDGET( plotpresent->canvas ) );
	gtk_widget_add_events( plotpresent->canvas,
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK );

	/* You'd think we could set up the axies too, but we can't get them
	 * from the chart until it's realized. Wait for the first refresh.
	 */

	plotpresent->grend = gog_renderer_new( plotpresent->ggraph );
}

GType
plotpresent_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( PlotpresentClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) plotpresent_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Plotpresent ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) plotpresent_init,
		};

		type = g_type_register_static( GTK_TYPE_BIN, 
			"Plotpresent", &info, 0 );
	}

	return( type );
}

static void
plotpresent_build_plot( Plotpresent *plotpresent )
{
	Plotmodel *plotmodel = plotpresent->plotmodel;
	Plot *plot = plotmodel->plot;

#ifdef DEBUG
	printf( "plotpresent_build_plot: %p\n", plotpresent );
#endif /*DEBUG*/

	GOG_UNREF( plotpresent->gplot );

	plotpresent->gplot = plot_new_gplot( plot );
	gog_object_add_by_name( GOG_OBJECT( plotpresent->gchart ), 
		"Plot", GOG_OBJECT( plotpresent->gplot ) );

	plot_style_main( plot, plotpresent->gchart ); 
}

static void
plotpresent_changed_cb( Plotmodel *plotmodel, Plotpresent *plotpresent )
{
	Plot *plot = plotmodel->plot;

#ifdef DEBUG
	printf( "plotpresent_changed_cb: %p\n", plotpresent );
#endif /*DEBUG*/

	/* Can refresh before model build.
	 */
	if( plot->rows == 0 || 
		plot->columns == 0 )
		return;

	/* Rebuild plot and data.
	 */
	plotpresent_build_plot( plotpresent );
}

static void
plotpresent_link( Plotpresent *plotpresent, Plotmodel *plotmodel )
{
	/* All the model parts for our set of views.
	 */
	plotpresent->plotmodel = plotmodel;
	g_signal_connect( G_OBJECT( plotmodel ), "changed", 
		G_CALLBACK( plotpresent_changed_cb ), plotpresent );
}

Plotpresent *
plotpresent_new( Plotmodel *plotmodel )
{
	Plotpresent *plotpresent = g_object_new( TYPE_PLOTPRESENT, NULL );

	plotpresent_link( plotpresent, plotmodel );

	return( plotpresent );
}

#endif /*HAVE_LIBGOFFICE*/
