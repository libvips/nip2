/* a gtkplot widget, plus some navigation stuff
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

static GtkBinClass *parent_class = NULL;

/* Choose line colours with this. RGB first, then mostly random.
 */
#define RGB(R, G, B) { 0, (R) << 8, (G) << 8, (B) << 8 }
static GdkColor default_colour[] = {
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

/* Choose symbols shapes with this. 
 */
static GtkPlotSymbolType default_symbol_type[] = {
	GTK_PLOT_SYMBOL_SQUARE,
	GTK_PLOT_SYMBOL_CIRCLE,
	GTK_PLOT_SYMBOL_UP_TRIANGLE,
	GTK_PLOT_SYMBOL_DOWN_TRIANGLE,
	GTK_PLOT_SYMBOL_RIGHT_TRIANGLE,
	GTK_PLOT_SYMBOL_LEFT_TRIANGLE,
	GTK_PLOT_SYMBOL_DIAMOND,
	GTK_PLOT_SYMBOL_PLUS,
	GTK_PLOT_SYMBOL_CROSS,
	GTK_PLOT_SYMBOL_STAR,
	GTK_PLOT_SYMBOL_DOT,
	GTK_PLOT_SYMBOL_IMPULSE
};

/* Gdk keysyms, and the zooms we set for each.
 */
typedef struct _PlotpresentKeymap { 
	guint keyval;
	int zoom;
} PlotpresentKeymap; 

static PlotpresentKeymap plotpresent_keymap[] = {
	{ GDK_1, 100 },
	{ GDK_2, 120 },
	{ GDK_3, 130 },
	{ GDK_4, 140 },
	{ GDK_5, 150 },
	{ GDK_6, 160 },
	{ GDK_7, 170 },
	{ GDK_8, 180 },
	{ GDK_9, 190 }
};

enum {
	SIG_MOUSE_MOVE,		/* mose drag, axies cods */
	SIG_LAST
};

static guint plotpresent_signals[SIG_LAST] = { 0 };

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
	IM_FREEF( g_source_remove, plotpresent->resize_timeout );
	IM_FREEF( iwindow_cursor_context_destroy, plotpresent->cntxt );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
plotpresent_mouse_move( Plotpresent *plotpresent, double x, double y )
{
	g_signal_emit( G_OBJECT( plotpresent ), 
		plotpresent_signals[SIG_MOUSE_MOVE], 0, x, y );
}

static void
plotpresent_set_position( Plotpresent *plotpresent, double x, double y )
{
	GtkAdjustment *adj;

#ifdef DEBUG
	printf( "plotpresent_set_position: x = %g, y = %g\n", x, y );
#endif /*DEBUG*/

	adj = gtk_scrolled_window_get_hadjustment( 
		GTK_SCROLLED_WINDOW( plotpresent->swin ) );
	x = IM_CLIP( 0, x, adj->upper - adj->page_size );
	if( adj->value != x )
		gtk_adjustment_set_value( adj, x );
	adj = gtk_scrolled_window_get_vadjustment( 
		GTK_SCROLLED_WINDOW( plotpresent->swin ) );
	y = IM_CLIP( 0, y, adj->upper - adj->page_size );
	if( adj->value != y )
		gtk_adjustment_set_value( adj, y );
}

static void
plotpresent_get_viewport( Plotpresent *plotpresent, Rect *rect )
{
	GtkAdjustment *adj;
	GtkWidget *port = gtk_bin_get_child( GTK_BIN( plotpresent->swin ) );
	int width = port ? port->allocation.width - 2 : 100;
	int height = port ? port->allocation.height - 2 : 100;

	adj = gtk_scrolled_window_get_hadjustment( 
		GTK_SCROLLED_WINDOW( plotpresent->swin ) );
	rect->left = adj->value;
	rect->width = width;
	adj = gtk_scrolled_window_get_vadjustment( 
		GTK_SCROLLED_WINDOW( plotpresent->swin ) );
	rect->top = adj->value;
	rect->height = height;
}

/* Spot mouse motion events, to update status bar.
 */
static gboolean
plotpresent_event_cb( GtkWidget *widget, GdkEvent *event, 
	Plotpresent *plotpresent )
{
	gboolean handled;
	Rect rect;

#ifdef DEBUG
	printf( "plotpresent_event_cb: %p\n", plotpresent );
#endif /*DEBUG*/

	handled = FALSE;

	plotpresent_get_viewport( plotpresent, &rect );

	switch( event->type ) {
	case GDK_BUTTON_PRESS:
		switch( event->button.button ) {
		case 1:
			if( !GTK_WIDGET_HAS_FOCUS( 
				GTK_WIDGET( plotpresent ) ) ) {
				gtk_widget_grab_focus( 
					GTK_WIDGET( plotpresent ) );
				handled = TRUE;
			}
		break;
			break;

		default:
			break;
		}

		/* Note drag start position for panning.
		 */
		if( event->button.button == 1 || event->button.button == 2 ) {
#ifdef DEBUG
			printf( "plotpresent_event_cb: drag start\n" );
#endif /*DEBUG*/

			plotpresent->dx = event->button.x_root + rect.left;
			plotpresent->dy = event->button.y_root + rect.top;
			iwindow_cursor_context_set_cursor( plotpresent->cntxt, 
				IWINDOW_SHAPE_MOVE );
			handled = TRUE;
		}

		break;

	case GDK_BUTTON_RELEASE:
		if( event->button.state & GDK_BUTTON1_MASK ||
			event->button.state & GDK_BUTTON2_MASK ) {
#ifdef DEBUG
			printf( "plotpresent_event_cb: drag stop\n" );
#endif /*DEBUG*/

			iwindow_cursor_context_set_cursor( plotpresent->cntxt, 
				IWINDOW_SHAPE_NONE );
			handled = TRUE;
		}

		break;

	case GDK_MOTION_NOTIFY: {
		double cx, cy;
		double x1, y1;
		double x2, y2;
		double dx, dy;
		GtkPlotAxis *axis;
		double px, py;

		/* We're using motion hints, so we need to read the pointer to
		 * get the next one.
		 */
		widget_update_pointer( widget, event );

		/* Position in window.
		 */
		plotpresent->last_x = event->motion.x;
		plotpresent->last_y = event->motion.y;

		/* Position in canvas.
		 */
		gtk_plot_canvas_get_position( 
			GTK_PLOT_CANVAS( plotpresent->canvas ),
			plotpresent->last_x, plotpresent->last_y, &cx, &cy );

		/* Position of child.
		 */
		gtk_plot_canvas_child_get_position(
			GTK_PLOT_CANVAS( plotpresent->canvas ),
			plotpresent->child,
			&x1, &y1, &x2, &y2 );

		/* Position in child, origin in bottom left, child cods.
		 */
		dx = (cx - x1) / (x2 - x1);
		dy = ((y2 - y1) - (cy - y1)) / (y2 - y1);

		/* Position in plot data.
		 */
		axis = gtk_plot_get_axis( GTK_PLOT( plotpresent->plot ),
			GTK_PLOT_AXIS_BOTTOM );
		px = gtk_plot_axis_ticks_inverse( axis, dx );
		axis = gtk_plot_get_axis( GTK_PLOT( plotpresent->plot ),
			GTK_PLOT_AXIS_LEFT );
		py = gtk_plot_axis_ticks_inverse( axis, dy );

		/* Point is inside child?
		 */
		if( cx > x1 && cx < x2 && cy > y1 && cy < y2 )
			plotpresent_mouse_move( plotpresent, px, py );

		if( event->motion.state & GDK_BUTTON1_MASK ||
			event->motion.state & GDK_BUTTON2_MASK )  {
#ifdef DEBUG
			printf( "plotpresent_event_cb: drag\n" );
#endif /*DEBUG*/

			plotpresent_set_position( plotpresent, 
				(int) plotpresent->dx - event->motion.x_root, 
				(int) plotpresent->dy - event->motion.y_root );
		}

		break;
	}

	case GDK_ENTER_NOTIFY:
		plotpresent->inside = TRUE;

		break;

	case GDK_LEAVE_NOTIFY:
		plotpresent->inside = FALSE;

		break;

	default:
		break;
	}

	return( handled );
}

static void
plotpresent_scroll( Plotpresent *plotpresent, int u, int v )
{
	Rect rect;

	plotpresent_get_viewport( plotpresent, &rect );
	plotpresent_set_position( plotpresent, rect.left + u, rect.top + v );
}

static void
plotpresent_scroll_modifier( Plotpresent *plotpresent, 
	gboolean shift, gboolean ctrl, int u, int v )
{
	GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment( 
		GTK_SCROLLED_WINDOW( plotpresent->swin ) );
	GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment( 
		GTK_SCROLLED_WINDOW( plotpresent->swin ) );

	if( shift ) {
		u *=  hadj->page_increment;
		v *=  vadj->page_increment;
	}
	else if( ctrl ) {
		u *=  hadj->upper - hadj->lower;
		v *=  vadj->upper - hadj->lower;
	}
	else {
		u *=  hadj->step_increment;
		v *=  vadj->step_increment;
	}

	plotpresent_scroll( plotpresent, u, v );
}

/* Set a zoom, put the xy position in the current view at the centre of the new
 * display.
 */
void
plotpresent_set_mag_position( Plotpresent *plotpresent, int mag, int x, int y )
{
	Plotmodel *plotmodel = plotpresent->plotmodel;

	/* Map xy to zoom 100% coordinates.
	 */
	double x1 = x / (plotmodel->mag / 100.0);
	double y1 = y / (plotmodel->mag / 100.0);

	/* Now map to new zoom cods.
	 */
	double xn = x1 * (mag / 100.0);
	double yn = y1 * (mag / 100.0);

	Rect rect;

#ifdef DEBUG
	printf( "plotpresent_set_mag_position\n" );
#endif /*DEBUG*/

	plotmodel_set_mag( plotmodel, mag );
	plotpresent_get_viewport( plotpresent, &rect );
	plotpresent_set_position( plotpresent, 
		xn - rect.width / 2, yn - rect.height / 2 );
}

/* Return the current magnification and the position of the centre of the
 * window.
 */
void
plotpresent_get_mag_position( Plotpresent *plotpresent, 
	int *mag, int *x, int *y )
{
	Plotmodel *plotmodel = plotpresent->plotmodel;

	Rect rect;

	plotpresent_get_viewport( plotpresent, &rect );

	*mag = plotmodel->mag;
	*x = rect.left + rect.width / 2;
	*y = rect.top + rect.height / 2;
}

static void
plotpresent_set_mag_centre( Plotpresent *plotpresent, int mag )
{
	Rect rect;

	plotpresent_get_viewport( plotpresent, &rect );
	plotpresent_set_mag_position( plotpresent, mag,
		rect.left + rect.width / 2,
		rect.top + rect.height / 2 );
}

static gboolean
plotpresent_key_press_event_cb( GtkWidget *widget, GdkEventKey *event, 
	Plotpresent *plotpresent )
{
	Plotmodel *plotmodel = plotpresent->plotmodel;
	gboolean handled;
	int i;

#ifdef DEBUG
	printf( "imagepresent_key_press_event_cb\n" );
#endif /*DEBUG*/

	handled = FALSE;

	switch( event->keyval ) {
	case GDK_Left:
		plotpresent_scroll_modifier( plotpresent, 
			event->state & GDK_SHIFT_MASK,
			event->state & GDK_CONTROL_MASK,
			-1, 0 );
		handled = TRUE;
		break;

	case GDK_Right:
		plotpresent_scroll_modifier( plotpresent, 
			event->state & GDK_SHIFT_MASK,
			event->state & GDK_CONTROL_MASK,
			1, 0 );
		handled = TRUE;
		break;

	case GDK_Up:
		plotpresent_scroll_modifier( plotpresent, 
			event->state & GDK_SHIFT_MASK,
			event->state & GDK_CONTROL_MASK,
			0, -1 );
		handled = TRUE;
		break;

	case GDK_Down:
		plotpresent_scroll_modifier( plotpresent, 
			event->state & GDK_SHIFT_MASK,
			event->state & GDK_CONTROL_MASK,
			0, 1 );
		handled = TRUE;
		break;

	case GDK_i:
		if( plotpresent->inside ) 
			plotpresent_set_mag_position( plotpresent, 
				plotmodel->mag * 2, 
				plotpresent->last_x,
				plotpresent->last_y );
		handled = TRUE;
		break;

	case GDK_o: 
		if( plotpresent->inside ) 
			plotpresent_set_mag_centre( plotpresent, 
				plotmodel->mag / 2 ); 
		handled = TRUE;
		break;

	default:
		break;
	}

	/* Check the number zoom keys too.
	 */
	if( !handled ) 
		for( i = 0; i < IM_NUMBER( plotpresent_keymap ); i++ )
			if( event->keyval == plotpresent_keymap[i].keyval ) {
				plotmodel_set_mag( plotmodel, 
					plotpresent_keymap[i].zoom );
				handled = TRUE;
				break;
			}

	return( handled );
}

static void
plotpresent_build_plot( Plotpresent *plotpresent, int width, int height )
{
	Plotmodel *plotmodel = plotpresent->plotmodel;
	Plot *plot = plotmodel->plot;

	GtkPlotAxis *axis;
	int xticks, yticks;
	double xstep, ystep;
	double x1, y1, x2, y2;
	int xpixels, ypixels;
	int nxminor, nyminor;

#ifdef DEBUG
	printf( "plotpresent_build_plot: building canvas: "
		"was %d x %d, now %d x %d (mag = %d)\n",
		plotmodel->width, plotmodel->height,
		width, height,
		plotmodel->mag );
#endif /*DEBUG*/

	/* Destroying the canvas destroys the plot and the data.
	 */
	IM_FREEF( gtk_widget_destroy, plotpresent->canvas );
	plotpresent->plot = NULL;
	plotpresent->data = NULL;

	plotpresent->canvas = gtk_plot_canvas_new( width, height, 
		plotmodel->mag / 100.0 );
	plotmodel->width = width;
	plotmodel->height = height;
	gtk_widget_add_events( plotpresent->canvas,
		GDK_KEY_PRESS_MASK | 
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
		GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK ); 
	gtk_signal_connect( GTK_OBJECT( plotpresent->canvas ), "event",
		GTK_SIGNAL_FUNC( plotpresent_event_cb ), plotpresent );
	gtk_signal_connect( GTK_OBJECT( plotpresent ), 
		"key_press_event",
		GTK_SIGNAL_FUNC( plotpresent_key_press_event_cb ), 
		plotpresent );

	gtk_scrolled_window_add_with_viewport( 
		GTK_SCROLLED_WINDOW( plotpresent->swin ), 
		plotpresent->canvas );

	plotpresent->plot = gtk_plot_new_with_size( NULL, 1, 1 );
	gtk_widget_show( plotpresent->plot );

	/* How many ticks per axis? Scale with pixels, but aim for a
	 * tick every 50 pixels or so.
	 */
	xticks = width / 50;
	yticks = height / 50;

	/* So tick step is ... round to next highest power of 10.
	 */
	xstep = (plot->xmax - plot->xmin) / xticks;
	xstep = pow( 10, ceil( log10( xstep ) ) );
	ystep = (plot->ymax - plot->ymin) / yticks;
	ystep = pow( 10, ceil( log10( ystep ) ) );

	/* Don't let step get > range or gtkplot segvs.
	 */
	xstep = IM_MIN( plot->xmax - plot->xmin, xstep );
	ystep = IM_MIN( plot->ymax - plot->ymin, ystep );

	/* Estimate pixels per tick.
	 */
	xpixels = width / ((plot->xmax - plot->xmin) / xstep);
	ypixels = height / ((plot->ymax - plot->ymin) / ystep);

	/* Don't let minors get too close.
	 */
	if( xpixels > 200 )
		nxminor = 9;
	else if( xpixels > 100 )
		nxminor = 4;
	else if( xpixels > 50 )
		nxminor = 1;
	else
		nxminor = 0;
	if( ypixels > 200 )
		nyminor = 9;
	else if( ypixels > 100 )
		nyminor = 4;
	else if( ypixels > 50 )
		nyminor = 1;
	else
		nyminor = 0;

	gtk_plot_set_ticks( GTK_PLOT( plotpresent->plot ), 
		GTK_PLOT_AXIS_X, xstep, nxminor );
	gtk_plot_set_ticks( GTK_PLOT( plotpresent->plot ), 
		GTK_PLOT_AXIS_Y, ystep, nyminor );

	axis = gtk_plot_get_axis( GTK_PLOT( plotpresent->plot ),
		GTK_PLOT_AXIS_RIGHT );
	gtk_plot_axis_set_visible( axis, FALSE );

	axis = gtk_plot_get_axis( GTK_PLOT( plotpresent->plot ),
		GTK_PLOT_AXIS_TOP );
	gtk_plot_axis_set_visible( axis, FALSE );

	axis = gtk_plot_get_axis( GTK_PLOT( plotpresent->plot ),
		GTK_PLOT_AXIS_LEFT );
	gtk_plot_axis_hide_title( axis );

	axis = gtk_plot_get_axis( GTK_PLOT( plotpresent->plot ),
		GTK_PLOT_AXIS_BOTTOM );
	gtk_plot_axis_hide_title( axis );

	gtk_plot_grids_set_visible( GTK_PLOT( plotpresent->plot ),
		TRUE, TRUE, TRUE, TRUE );

	/* Position the plot: we want the position to be a certain number of
	 * pixels, so we must scale by width.
	 */
	x1 = 80.0 / width;
	y1 = 5.0 / height;
	x2 = (width - 5.0) / width;
	y2 = (height - 20.0)/ height;

	plotpresent->child = gtk_plot_canvas_plot_new( 
		GTK_PLOT( plotpresent->plot ) );
	gtk_plot_canvas_put_child( 
		GTK_PLOT_CANVAS( plotpresent->canvas ), 
		plotpresent->child, x1, y1, x2, y2 );
}

GSList *
plotpresent_build_data( Plot *plot, GtkWidget *widget )
{
	/* How odd, 0.5 is a full bar width.
	 */
	const double bar_width = 0.45;

	GtkWidget *data;
	int i;
	GSList *all_data;
	char label[256];

	/* Bars are shown centered on the x position. We need to move xmin
	 * left by half a bar, and xmax right by half a bar.
	 */
	if( plot->style == PLOT_STYLE_BAR )
		gtk_plot_set_range( GTK_PLOT( widget ), 
			plot->xmin - bar_width, plot->xmax + bar_width, 
			plot->ymin, plot->ymax );
	else
		gtk_plot_set_range( GTK_PLOT( widget ), 
			plot->xmin, plot->xmax, plot->ymin, plot->ymax );

	all_data = NULL;
	for( i = 0; i < plot->columns; i++ ) {
		GdkColor *colour = &default_colour[i % 
			IM_NUMBER( default_colour )];
		GtkPlotSymbolType symbol_type = default_symbol_type[i % 
			IM_NUMBER( default_symbol_type )];

		switch( plot->style ) {
		case PLOT_STYLE_POINT:
			data = gtk_plot_data_new();
			gtk_plot_data_set_line_attributes(
				GTK_PLOT_DATA( data ),
				GTK_PLOT_LINE_NONE,
                                0, 0, 1, colour );

			gtk_plot_data_set_symbol( GTK_PLOT_DATA( data ),
				symbol_type,
				GTK_PLOT_SYMBOL_OPAQUE,
				5, 2, colour, colour );

			break;

		case PLOT_STYLE_LINE:
			data = gtk_plot_data_new();
			gtk_plot_data_set_line_attributes(
				GTK_PLOT_DATA( data ),
				GTK_PLOT_LINE_SOLID,
                                0, 0, 1, colour );
			break;


		case PLOT_STYLE_SPLINE:
			data = gtk_plot_data_new();
			gtk_plot_data_set_line_attributes(
				GTK_PLOT_DATA( data ),
				GTK_PLOT_LINE_SOLID,
                                0, 0, 1, colour );
			gtk_plot_data_set_connector( GTK_PLOT_DATA( data ),
				GTK_PLOT_CONNECT_SPLINE );
			break;

		case PLOT_STYLE_BAR:
			data = gtk_plot_bar_new( GTK_ORIENTATION_VERTICAL );

			gtk_plot_bar_set_width( GTK_PLOT_BAR( data ), 
				bar_width );

			gtk_plot_data_set_symbol( GTK_PLOT_DATA( data ),
				GTK_PLOT_SYMBOL_NONE,
				GTK_PLOT_SYMBOL_EMPTY,
				20, 2, colour, colour );

			gtk_plot_data_set_line_attributes( 
				GTK_PLOT_DATA( data ),
				GTK_PLOT_LINE_NONE,
				0, 0, 1, colour );

			break;

		default:
			assert( 0 );
		}

		gtk_plot_data_set_x( GTK_PLOT_DATA( data ), plot->xcolumn[i] );
		gtk_plot_data_set_y( GTK_PLOT_DATA( data ), plot->ycolumn[i] );
		gtk_plot_data_set_numpoints( GTK_PLOT_DATA( data ), 
			plot->rows );
		im_snprintf( label, 256, "Series %d", i );
		gtk_plot_data_set_legend( GTK_PLOT_DATA( data ), label );

		gtk_plot_add_data( GTK_PLOT( widget ), GTK_PLOT_DATA( data ) );
		gtk_widget_show( data );

		all_data = g_slist_prepend( all_data, data );
	}

	return( all_data );
}

static void
plotpresent_refresh_plot( Plotpresent *plotpresent )
{
	Plotmodel *plotmodel = plotpresent->plotmodel;
	Plot *plot = plotmodel->plot;
	GtkWidget *port = gtk_bin_get_child( GTK_BIN( plotpresent->swin ) );
	int width = port ? port->allocation.width - 2 : 100;
	int height = port ? port->allocation.height - 2 : 100;

#ifdef DEBUG
	printf( "plotpresent_refresh_plot:\n" );
#endif /*DEBUG*/

	/* Can refresh before model build.
	 */
	if( plot->rows == 0 || plot->columns == 0 )
		return;

	/* Junk old data.
	 */
	while( plotpresent->data ) {
		GtkWidget *data = GTK_WIDGET( plotpresent->data->data );

		gtk_plot_remove_data( GTK_PLOT( plotpresent->plot ), 
			GTK_PLOT_DATA( data ) );
		plotpresent->data = g_slist_remove( plotpresent->data, data );
	}

	/* Rebuild canvas and axies
	 */
	plotpresent_build_plot( plotpresent, width, height );

	/* Remake data.
	 */
	plotpresent->data = 
		plotpresent_build_data( plot, plotpresent->plot );

	/* How odd, but seems to be necessary to get GtkPlotCanvas to paint.
	 */
	gtk_widget_show_all( plotpresent->canvas );
	gtk_plot_paint( GTK_PLOT( plotpresent->plot ) );
	gtk_widget_queue_draw( plotpresent->canvas );
}

static gboolean
plotpresent_refresh_timeout_cb( Plotpresent *plotpresent )
{
	Plotmodel *plotmodel = plotpresent->plotmodel;
	GtkWidget *port = gtk_bin_get_child( GTK_BIN( plotpresent->swin ) );
	int width = port ? port->allocation.width - 2 : 100;
	int height = port ? port->allocation.height - 2 : 100;

	plotpresent->resize_timeout = 0;

#ifdef DEBUG
	printf( "plotpresent_resize_timeout_cb: %d x %d:\n", width, height );
#endif /*DEBUG*/

	/* We want to not rebuild if there's no size change ... otherwise
	 * we'll trigger a size_allocate, which will retrigger us.
	 */
	if( !plotpresent->canvas ||
		plotmodel->width != width || plotmodel->height != height ) 
		plotpresent_refresh_plot( plotpresent );

	/* Remove timeout.
	 */
	return( FALSE );
}

static void
plotpresent_queue_refresh( Plotpresent *plotpresent )
{
	IM_FREEF( g_source_remove, plotpresent->resize_timeout );
	plotpresent->resize_timeout = g_timeout_add( 100, 
		(GSourceFunc) plotpresent_refresh_timeout_cb, plotpresent );
}

static void
plotpresent_size_request( GtkWidget *widget, GtkRequisition *requisition )
{
	GtkBin *bin = GTK_BIN( widget );
	gint focus_width;
	gint focus_pad;

	gtk_widget_style_get( widget, 
		"focus-line-width", &focus_width,
		"focus-padding", &focus_pad,
		NULL );

	requisition->width = 2 * (focus_width + focus_pad);
	requisition->height = 2 * (focus_width + focus_pad);

	if( bin->child && GTK_WIDGET_VISIBLE( bin->child ) ) {
		GtkRequisition child_requisition;

		gtk_widget_size_request( bin->child, &child_requisition );

		requisition->width += child_requisition.width;
		requisition->height += child_requisition.height;
	}

	requisition->width += 10;
	requisition->height += 10;
}

static void
plotpresent_size_allocate( GtkWidget *widget, GtkAllocation *allocation )
{
	GtkBin *bin = GTK_BIN( widget );

	widget->allocation = *allocation;

	if( bin->child && GTK_WIDGET_VISIBLE( bin->child ) ) {
		gint focus_width;
		gint focus_pad;
		GtkAllocation child_allocation;

		gtk_widget_style_get( widget, 
			"focus-line-width", &focus_width,
			"focus-padding", &focus_pad,
			NULL );

		child_allocation.x = allocation->x + focus_width + focus_pad;
		child_allocation.y = allocation->y + focus_width + focus_pad;
		child_allocation.width = IM_MAX( 1, 
			allocation->width - 2 * (focus_width + focus_pad) );
		child_allocation.height = IM_MAX( 1,
			allocation->height - 2 * (focus_width + focus_pad) );

		child_allocation.x += 5;
		child_allocation.y += 5;
		child_allocation.width -= 10;
		child_allocation.height -= 10;

		gtk_widget_size_allocate( bin->child, &child_allocation );
	}

	plotpresent_queue_refresh( PLOTPRESENT( widget ) );
}

static gboolean
plotpresent_expose_event( GtkWidget *widget, GdkEventExpose *event )
{
	if( GTK_WIDGET_DRAWABLE( widget ) ) {
		if( GTK_WIDGET_HAS_FOCUS( widget ) ) {
			gint focus_pad;
			int x, y, width, height;

			gtk_widget_style_get( widget, 
				"focus-padding", &focus_pad,
				NULL );

			x = widget->allocation.x + focus_pad;
			y = widget->allocation.y + focus_pad;
			width = widget->allocation.width - 2 * focus_pad;
			height = widget->allocation.height - 2 * focus_pad;

			gtk_paint_focus( widget->style, widget->window, 
				GTK_WIDGET_STATE( widget ),
				&event->area, widget, "plotpresent",
				x, y, width, height );
		}

		GTK_WIDGET_CLASS( parent_class )->expose_event( widget, event );
	}

	return( FALSE );
}

/* Connect to our enclosing iwnd on realize.
 */
static void
plotpresent_realize( GtkWidget *widget )
{
	Plotpresent *plotpresent = PLOTPRESENT( widget );
	iWindow *iwnd = IWINDOW( gtk_widget_get_toplevel( widget ) );	

	if( !plotpresent->cntxt ) 
		plotpresent->cntxt = iwindow_cursor_context_new( iwnd, 
			0, "imagepresent" );

	GTK_WIDGET_CLASS( parent_class )->realize( widget );
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
        widget_class->expose_event = plotpresent_expose_event;
        widget_class->realize = plotpresent_realize;

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

/* Scroll events ... handle mousewheel shortcuts here. 
 */
static gboolean
plotpresent_scroll_event_cb( GtkWidget *widget, 
	GdkEventScroll *ev, Plotpresent *plotpresent )
{
	Plotmodel *plotmodel = plotpresent->plotmodel;
	gboolean handled;

	/* Gimp uses page_incr / 4 I think, but then scroll speed varies with
	 * window size, which is pretty odd. Just use a constant.
	 */
	const int incr = 50;

	handled = FALSE;

	if( ev->direction == GDK_SCROLL_UP || 
		ev->direction == GDK_SCROLL_DOWN ) {
		if( ev->state & GDK_CONTROL_MASK ) {
			if( ev->direction == GDK_SCROLL_UP )
				plotpresent_set_mag_centre( plotpresent, 
					plotmodel->mag * 1.1 );
			else
				plotpresent_set_mag_centre( plotpresent, 
					plotmodel->mag / 1.1 );

			handled = TRUE;
		}
		else if( ev->state & GDK_SHIFT_MASK ) {
			if( ev->direction == GDK_SCROLL_UP )
				plotpresent_scroll( plotpresent, incr, 0 );
			else
				plotpresent_scroll( plotpresent, -incr, 0 );

			handled = TRUE;
		}
		else {
			if( ev->direction == GDK_SCROLL_UP )
				plotpresent_scroll( plotpresent, 0, -incr );
			else
				plotpresent_scroll( plotpresent, 0, incr );

			handled = TRUE;
		}
	}

	return( handled );
}

static void
plotpresent_init( Plotpresent *plotpresent )
{
	GtkWidget *bar;

#ifdef DEBUG
	printf( "plotpresent_init: %p\n", plotpresent );
#endif /*DEBUG*/

	plotpresent->last_x = 0.0;
	plotpresent->last_y = 0.0;
	plotpresent->inside = FALSE;
	plotpresent->data = NULL;
	plotpresent->canvas = NULL;
	plotpresent->cntxt = NULL;
	plotpresent->resize_timeout = 0;
	plotpresent->plot = NULL;

	plotpresent->swin = gtk_scrolled_window_new( NULL, NULL );
        gtk_container_add( GTK_CONTAINER( plotpresent ), plotpresent->swin );
	gtk_widget_show( plotpresent->swin );
	gtk_signal_connect( GTK_OBJECT( plotpresent->swin ), "scroll_event",
		GTK_SIGNAL_FUNC( plotpresent_scroll_event_cb ), plotpresent );

	GTK_WIDGET_SET_FLAGS( plotpresent, GTK_CAN_FOCUS );

	/* We have our own kb nav system: turn off the bar's keynav.
	 */
	bar = GTK_SCROLLED_WINDOW( plotpresent->swin )->hscrollbar;
	g_assert( GTK_IS_SCROLLBAR( bar ) );
	GTK_WIDGET_UNSET_FLAGS( bar, GTK_CAN_FOCUS );
	bar = GTK_SCROLLED_WINDOW( plotpresent->swin )->vscrollbar;
	g_assert( GTK_IS_SCROLLBAR( bar ) );
	GTK_WIDGET_UNSET_FLAGS( bar, GTK_CAN_FOCUS );
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
plotpresent_changed_cb( Plotmodel *plotmodel, Plotpresent *plotpresent )
{
#ifdef DEBUG
	printf( "plotpresent_changed_cb: %p\n", plotpresent );
#endif /*DEBUG*/

	/* Invalidate width so the axies are regenerated ... the range may have
	 * changed when the data changed, even if the canvas size hasn't.
	 */
	plotmodel->width = -1;
	plotpresent_refresh_plot( plotpresent );
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
	Plotpresent *plotpresent = gtk_type_new( TYPE_PLOTPRESENT );

	plotpresent_link( plotpresent, plotmodel );

	return( plotpresent );
}
