/* run the display for a plotview in a workspace 
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

/*
#define DEBUG
#define DEBUG_GEO
 */

#include "ip.h"

#ifdef HAVE_LIBGOFFICE

/* Choose line colours with this. RGB first, then mostly random.
 */
#define RGB(R, G, B) RGB_TO_RGBA( RGB_TO_UINT( R, G, B ), 0xff )
static GOColor default_colour[] = {
	RGB( 255, 0, 0 ),
	RGB( 0, 255, 0 ),
	RGB( 0, 0, 255 ),
	RGB( 100, 0, 102 ),
	RGB( 17, 0, 102 ),
	RGB( 0, 0, 180 ),
	RGB( 0, 53, 255 ),
	RGB( 0, 104, 234 ),
	RGB( 0, 150, 188 ),
	RGB( 0, 205, 170 ),
	RGB( 0, 255, 139 ),
	RGB( 0, 255, 55 ),
	RGB( 40, 255, 40 ),
	RGB( 106, 255, 74 ),
	RGB( 155, 255, 48 ),
	RGB( 209, 255, 21 ),
	RGB( 239, 255, 7 ),
	RGB( 255, 176, 0 ),
	RGB( 255, 110, 0 ),
	RGB( 255, 50, 0 ),
	RGB( 196, 0, 0 )
};

static GraphicviewClass *parent_class = NULL;

static void
plotview_destroy( GtkObject *object )
{
    	Plotview *plotview;

    	g_return_if_fail( object != NULL );
    	g_return_if_fail( IS_PLOTVIEW( object ) );

#ifdef DEBUG
    	printf( "plotview_destroy\n" );
#endif /*DEBUG*/

    	plotview = PLOTVIEW( object );

    	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
plotview_refresh( vObject *vobject )
{
    	Plotview *plotview = PLOTVIEW( vobject );
    	Plot *plot = PLOT( VOBJECT( plotview )->iobject );

	GSList *axes;
	GogAxis *axis;

	int i;

#ifdef DEBUG
    	printf( "plotview_refresh\n" );
#endif /*DEBUG*/

	/* Can't refresh before model build.
	 */
	if( plot->rows == 0 || plot->columns == 0 )
		return;

	set_gcaption( plotview->label, "%s", NN( IOBJECT( plot )->caption ) );

	/* FIXME

		We always rebuild the plot. We could perhaps just reset the 
		data in the series.

		need to change the number of series too, can we do this 
		without junking the plot?

	 */

	/* Junk the old plot child, how peculiar.
	 */
	if( plotview->gplot ) {
		gog_object_clear_parent( GOG_OBJECT( plotview->gplot ) );
		g_object_unref( plotview->gplot );
		plotview->gplot = NULL;
	}

	if( plot->style == PLOT_STYLE_BAR )
		plotview->gplot = gog_plot_new_by_name( "GogHistogramPlot" );
	else
		plotview->gplot = gog_plot_new_by_name( "GogXYPlot" );
	gog_object_add_by_name( GOG_OBJECT( plotview->gchart ), 
		"Plot", GOG_OBJECT( plotview->gplot ) );

	axes = gog_chart_get_axes( plotview->gchart, GOG_AXIS_X );
	axis = GOG_AXIS( axes->data );
	g_object_set( axis, 
		"major-tick-labeled", FALSE, 
		"major-tick-size-pts", 0,
		"pos", GOG_AXIS_CROSS,
		NULL );
	gog_axis_set_bounds( axis, plot->xmin, plot->xmax );
	g_slist_free( axes );

	axes = gog_chart_get_axes( plotview->gchart, GOG_AXIS_Y );
	axis = GOG_AXIS( axes->data );
	g_object_set( axis, 
		"major-tick-labeled", FALSE, 
		"major-tick-size-pts", 0,
		"pos", GOG_AXIS_CROSS,
		NULL );
	gog_axis_set_bounds( axis, plot->ymin, plot->ymax );
	g_slist_free( axes );

	if( plot->style == PLOT_STYLE_POINT )
		g_object_set( plotview->gplot,
			"default-style-has-lines", FALSE,
			NULL );
	else if( plot->style != PLOT_STYLE_BAR )
		g_object_set( plotview->gplot,
			"default-style-has-markers", FALSE,
			NULL );

	if( plot->style == PLOT_STYLE_SPLINE )
		g_object_set( plotview->gplot,
			"use-splines", TRUE,
			NULL );

	for( i = 0; i < plot->columns; i++ ) {
		GogSeries *series;
		GOData *data;
		GError *error;

                series = gog_plot_new_series( plotview->gplot );
		data = go_data_vector_val_new( plot->xcolumn[i], plot->rows, 
			NULL );
		gog_series_set_dim( series, 0, data, &error );
		data = go_data_vector_val_new( plot->ycolumn[i], plot->rows, 
			NULL );
		gog_series_set_dim( series, 1, data, &error );

		if( plot->style == PLOT_STYLE_BAR )
			g_object_set( series,
				"fill-type", "self",
				NULL );

		if( i < IM_NUMBER( default_colour ) ) {
			GogStyle *style;

			style = gog_styled_object_get_style( 
				GOG_STYLED_OBJECT( series ) );

			style->line.color = default_colour[i];
			style->line.auto_color = FALSE;

			go_marker_set_fill_color( style->marker.mark,
				default_colour[i] );
			style->marker.auto_fill_color = FALSE;

			/* Could match fill, but black everywhere looks nicer.
			 */
			go_marker_set_outline_color( style->marker.mark,
				RGBA_BLACK );
			style->marker.auto_outline_color = FALSE;
		}
	}

	gtk_widget_show_all( plotview->canvas );

    	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
plotview_link( View *view, Model *model, View *parent )
{
	Plotview *plotview = PLOTVIEW( view );
	Rowview *rview = ROWVIEW( parent->parent );

	VIEW_CLASS( parent_class )->link( view, model, parent );

	rowview_menu_attach( rview, GTK_WIDGET( plotview->box ) );
}

static void
plotview_class_init( PlotviewClass *class )
{
    	GtkObjectClass *object_class = (GtkObjectClass *) class;
    	vObjectClass *vobject_class = (vObjectClass *) class;
    	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

    	object_class->destroy = plotview_destroy;

    	vobject_class->refresh = plotview_refresh;

	view_class->link = plotview_link;
}

static void
plotview_tooltip_generate( GtkWidget *widget, VipsBuf *buf, Plotview *plotview )
{
	Plot *plot = PLOT( VOBJECT( plotview )->iobject );
	IMAGE *im;

	vips_buf_rewind( buf );
	vips_buf_appends( buf, vips_buf_all( &plot->caption_buffer ) );

	vips_buf_appendf( buf, ", %s, %s", 
		plot_f2c( plot->format ), plot_s2c( plot->style ) );

	if( (im = imageinfo_get( FALSE, plot->value.ii )) ) {
		vips_buf_appends( buf, ", " );
		vips_buf_appendi( buf, im );
	}
}

static void 
plotview_doubleclick_one_cb( GtkWidget *widget, Plotview *plotview, int state )
{
	Heapmodel *heapmodel = HEAPMODEL( VOBJECT( plotview )->iobject );
	Row *row = heapmodel->row;

	row_select_modifier( row, state );
}

static void 
plotview_doubleclick_two_cb( GtkWidget *widget, Plotview *plotview, int state )
{
	Plot *plot = PLOT( VOBJECT( plotview )->iobject );

	model_edit( widget, MODEL( plot ) );
}

static void
plotview_init( Plotview *plotview )
{
	GtkWidget *eb;

#ifdef DEBUG
    	printf( "plotview_init\n" );
#endif /*DEBUG*/

        eb = gtk_event_box_new();
        gtk_box_pack_start( GTK_BOX( plotview ), 
		eb, FALSE, FALSE, 0 );
	gtk_widget_show( eb );
	gtk_widget_set_name( eb, "caption_widget" );
        set_tooltip_generate( eb,
		(TooltipGenerateFn) plotview_tooltip_generate, plotview, NULL );
	doubleclick_add( eb, FALSE,
		DOUBLECLICK_FUNC( plotview_doubleclick_one_cb ), plotview,
		DOUBLECLICK_FUNC( plotview_doubleclick_two_cb ), plotview );

	plotview->box = gtk_vbox_new( FALSE, 0 );
        gtk_container_add( GTK_CONTAINER( eb ), plotview->box );
        gtk_widget_show( plotview->box );

	plotview->canvas = go_graph_widget_new( NULL );
        gtk_box_pack_start( GTK_BOX( plotview->box ), 
		plotview->canvas, FALSE, FALSE, 0 );
	plotview->gchart = go_graph_widget_get_chart( 
		GO_GRAPH_WIDGET( plotview->canvas ) );
	gtk_widget_set_size_request( GTK_WIDGET( plotview->canvas ), 
		DISPLAY_THUMBNAIL, DISPLAY_THUMBNAIL );

	plotview->gplot = NULL;

	plotview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( plotview->label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( plotview->label ), 2, 0 );
        gtk_box_pack_end( GTK_BOX( plotview->box ), 
		GTK_WIDGET( plotview->label ), FALSE, FALSE, 0 );
	gtk_widget_show( GTK_WIDGET( plotview->label ) );
}

GtkType
plotview_get_type( void )
{
    	static GtkType type = 0;

    	if( !type ) {
    		static const GtkTypeInfo info = {
    			"Plotview",
    			sizeof( Plotview ),
    			sizeof( PlotviewClass ),
    			(GtkClassInitFunc) plotview_class_init,
    			(GtkObjectInitFunc) plotview_init,
    			/* reserved_1 */ NULL,
    			/* reserved_2 */ NULL,
    			(GtkClassInitFunc) NULL,
    		};

    		type = gtk_type_unique( TYPE_GRAPHICVIEW, &info );
    	}

    	return( type );
}

View *
plotview_new( void )
{
    	Plotview *plotview = gtk_type_new( TYPE_PLOTVIEW );

    	return( VIEW( plotview ) );
}

#endif /*HAVE_LIBGOFFICE*/
