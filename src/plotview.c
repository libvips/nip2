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

/* We have gtkplot patched into the nip sources as a temp measure, so call 
 * directly.
 */
#define HAVE_GTK_EXTRA
#include "gtkplot.h"
#include "gtkplotcanvas.h"
#include "gtkplotcanvasplot.h"
#include "gtkplotdata.h"
#include "gtkplotbar.h"

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
plotview_size_allocate( GtkWidget *widget, GtkAllocation *allocation )
{
	Plotview *plotview = PLOTVIEW( widget );

#ifdef DEBUG_GEO
	printf( "plotview_size_allocate: %d x %d:\n", 
		allocation->width, allocation->height );
#endif /*DEBUG_GEO*/

	/* Has the size changed? Queue a refresh ... on idle, we will rebuild 
	 * our canvas for the new width.
	 */
	if( allocation->width != plotview->width ) 
		vobject_refresh_queue( VOBJECT( plotview ) );

	GTK_WIDGET_CLASS( parent_class )->size_allocate( widget, allocation );
}

static void
plotview_build_plot( Plotview *plotview, int width, int height )
{
    	Plot *plot = PLOT( VOBJECT( plotview )->iobject );
	GtkPlotAxis *axis;
	GtkPlotCanvasChild *child;
	double x1, y1, x2, y2;

#ifdef DEBUG
	printf( "plotview_refresh: building canvas: was %d, now %d\n",
		plotview->width, width );
#endif /*DEBUG*/

	/* Destroying the canvas destroys the plot and the data.
	 */
	IM_FREEF( gtk_widget_destroy, plotview->canvas );
	plotview->plot = NULL;
	plotview->data = NULL;

	plotview->canvas = gtk_plot_canvas_new( width, height, 1.0 );

        gtk_box_pack_start( GTK_BOX( plotview->box ), 
		plotview->canvas, FALSE, FALSE, 0 );

	plotview->plot = gtk_plot_new_with_size( NULL, 1, 1 );
	gtk_widget_show( plotview->plot );

	/* Turn off (almost) everything ... this is just a thumbnail.
	 */

	/* gtkplot mallocs memory for every tick (!!) even if ticks
	 * are off (!!!!) make sure it doesn't.
	 */
	gtk_plot_set_ticks( GTK_PLOT( plotview->plot ), 
		GTK_PLOT_AXIS_X, plot->xmax - plot->xmin, 0 );
	gtk_plot_set_ticks( GTK_PLOT( plotview->plot ), 
		GTK_PLOT_AXIS_Y, plot->ymax - plot->ymin, 0 );

	gtk_plot_hide_legends( GTK_PLOT( plotview->plot ) );

	axis = gtk_plot_get_axis( GTK_PLOT( plotview->plot ),
		GTK_PLOT_AXIS_RIGHT );
	gtk_plot_axis_set_visible( axis, FALSE );

	axis = gtk_plot_get_axis( GTK_PLOT( plotview->plot ),
		GTK_PLOT_AXIS_TOP );
	gtk_plot_axis_set_visible( axis, FALSE );

	axis = gtk_plot_get_axis( GTK_PLOT( plotview->plot ),
		GTK_PLOT_AXIS_LEFT );
	gtk_plot_axis_hide_title( axis );
	gtk_plot_axis_show_labels( axis, GTK_PLOT_LABEL_NONE );
	gtk_plot_axis_show_ticks( axis, 
		GTK_PLOT_TICKS_NONE, GTK_PLOT_TICKS_NONE );

	axis = gtk_plot_get_axis( GTK_PLOT( plotview->plot ),
		GTK_PLOT_AXIS_BOTTOM );
	gtk_plot_axis_hide_title( axis );
	gtk_plot_axis_show_labels( axis, GTK_PLOT_LABEL_NONE );
	gtk_plot_axis_show_ticks( axis, 
		GTK_PLOT_TICKS_NONE, GTK_PLOT_TICKS_NONE );

	/* Position the plot: we want the position to be a certain number of
	 * pixels, so we must scale by width.
	 */
	x1 = 3.0 / width;
	y1 = 3.0 / height;
	x2 = (width - 3.0) / width;
	y2 = (height - 3.0)/ height;

	child = gtk_plot_canvas_plot_new( GTK_PLOT( plotview->plot ) );
	gtk_plot_canvas_put_child( GTK_PLOT_CANVAS( plotview->canvas ), 
		child, x1, y1, x2, y2 );
}

static void
plotview_refresh( vObject *vobject )
{
    	Plotview *plotview = PLOTVIEW( vobject );
    	Plot *plot = PLOT( VOBJECT( plotview )->iobject );
	GtkAllocation *allocation = &GTK_WIDGET( plotview )->allocation;

#ifdef DEBUG
    	printf( "plotview_refresh\n" );
#endif /*DEBUG*/

	/* Can't refresh before model build.
	 */
	if( plot->rows == 0 || plot->columns == 0 )
		return;

	set_gcaption( plotview->label, "%s", NN( IOBJECT( plot )->caption ) );

	while( plotview->data ) {
		GtkWidget *data = GTK_WIDGET( plotview->data->data );

		gtk_plot_remove_data( GTK_PLOT( plotview->plot ), 
			GTK_PLOT_DATA( data ) );
		plotview->data = g_slist_remove( plotview->data, data );
	}

	if( !plotview->canvas || allocation->width != plotview->width ) {
		plotview_build_plot( plotview, 
			allocation->width, DISPLAY_THUMBNAIL );
		plotview->width = allocation->width;
	}

	plotview->data = plotpresent_build_data( plot, plotview->plot );

	gtk_widget_show_all( plotview->canvas );
	gtk_plot_paint( GTK_PLOT( plotview->plot ) );
	gtk_widget_queue_draw( plotview->canvas );

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
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
    	vObjectClass *vobject_class = (vObjectClass *) class;
    	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

    	object_class->destroy = plotview_destroy;

	widget_class->size_allocate = plotview_size_allocate;

    	vobject_class->refresh = plotview_refresh;

	view_class->link = plotview_link;
}

static void
plotview_tooltip_generate( GtkWidget *widget, BufInfo *buf, Plotview *plotview )
{
	Plot *plot = PLOT( VOBJECT( plotview )->iobject );
	IMAGE *im;

	buf_rewind( buf );
	buf_appends( buf, buf_all( &plot->caption_buffer ) );

	buf_appendf( buf, ", %s, %s", 
		plot_f2c( plot->format ), plot_s2c( plot->style ) );

	if( (im = imageinfo_get( FALSE, plot->value.ii )) ) {
		buf_appends( buf, ", " );
		buf_appendi( buf, im );
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
	set_name( eb, "caption_widget" );
        set_tooltip_generate( eb,
		(TooltipGenerateFn) plotview_tooltip_generate, plotview, NULL );
	doubleclick_add( eb, FALSE,
		DOUBLECLICK_FUNC( plotview_doubleclick_one_cb ), plotview,
		DOUBLECLICK_FUNC( plotview_doubleclick_two_cb ), plotview );

	plotview->box = gtk_vbox_new( FALSE, 0 );
        gtk_container_add( GTK_CONTAINER( eb ), plotview->box );
        gtk_widget_show( plotview->box );

	plotview->canvas = NULL;
	plotview->plot = NULL;
	plotview->data = NULL;
	plotview->width = 99999;

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

