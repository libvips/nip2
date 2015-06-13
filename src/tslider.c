/* a slider with an entry widget
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

/* Our signals. 
 */
enum {
	CHANGED,		
	ACTIVATE,	
	SLIDER_CHANGED,
	TEXT_CHANGED,	
	LAST_SIGNAL
};

G_DEFINE_TYPE( Tslider, tslider, GTK_TYPE_HBOX ); 

static guint tslider_signals[LAST_SIGNAL] = { 0 };

/* Are two doubles more or less equal. We need this when we check the sliders
 * for update to stop loops. The 0.0001 is a bit of a fudge :-(
 */
#define DEQ( A, B ) (ABS((A) - (B)) < 0.0001)

static void
tslider_destroy( GtkWidget *widget )
{
	Tslider *tslider;

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_TSLIDER( widget ) );

	tslider = TSLIDER( widget );

#ifdef DEBUG
	printf( "tslider_destroy: %p\n", tslider );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	if( tslider->adj ) {
		g_signal_disconnect_by_data( G_OBJECT( tslider->adj ),
			(gpointer) tslider );
		tslider->adj = NULL;
	}

	GTK_WIDGET_CLASS( tslider_parent_class )->destroy( widget );
}

/* Map a value to a slider position.
 */
static double
tslider_value_to_slider( Tslider *tslider, double value )
{
	/* Map our range to 0-1.
	 */
	const double scale = 1.0 / (tslider->to - tslider->from);
	const double to01 = (value - tslider->from) * scale;

	/* Pass through user fn.
	 */
	const double mapped = tslider->value_to_slider( 
		tslider->from, tslider->to, to01 );
	const double nvalue = mapped / scale + tslider->from;

#ifdef DEBUG
	printf( "tslider_value_to_slider: %g, to %g\n", value, nvalue );
#endif /*DEBUG*/

	/* Map back to main range.
	 */
	return( nvalue );
}

/* Map a slider position to a value.
 */
static double
tslider_slider_to_value( Tslider *tslider, double value )
{
	/* Map our range to 0-1.
	 */
	const double scale = 1.0 / (tslider->to - tslider->from);
	const double to01 = (value - tslider->from) * scale;

	/* Pass through user fn.
	 */
	const double mapped = tslider->slider_to_value( 
		tslider->from, tslider->to, to01 );
	const double nvalue = mapped / scale + tslider->from;

#ifdef DEBUG
	printf( "tslider_slider_to_value: %g, to %g\n", value, nvalue );
#endif /*DEBUG*/

	/* Map back to main range.
	 */
	return( nvalue );
}

/* from/to/value have changed ... update the widgets.
 */
static void
tslider_real_changed( Tslider *tslider )
{
	GtkAdjustment *adj = tslider->adj;
	GtkWidget *entry = tslider->entry;

#ifdef DEBUG
	printf( "tslider_real_changed: %p, val = %g\n", 
		tslider, tslider->value );
#endif /*DEBUG*/

	if( tslider->auto_link ) 
		tslider->svalue = tslider_value_to_slider( tslider, 
			tslider->value );

	g_signal_handler_block_by_data( G_OBJECT( adj ), tslider );
	g_signal_handler_block_by_data( G_OBJECT( entry ), tslider );

	/* Some libc's hate out-of-bounds precision, so clip, just in case.
	 */
	set_gentry( tslider->entry, "%.*f", 
		IM_CLIP( 0, tslider->digits, 100 ), tslider->value );
	gtk_scale_set_digits( GTK_SCALE( tslider->slider ), tslider->digits );

	if( !DEQ( tslider->from, tslider->last_from ) || 
		!DEQ( tslider->to, tslider->last_to ) ) {
		double range = tslider->to - tslider->from;

		adj->step_increment = range / 100;
		adj->page_increment = range / 10;
		adj->page_size = range / 10;

		adj->lower = tslider->from;
		adj->upper = tslider->to + adj->page_size;

		tslider->last_to = tslider->to;
		tslider->last_from = tslider->from;

		gtk_adjustment_changed( adj );
	}

	if( !DEQ( tslider->svalue, tslider->last_svalue ) ) {
		adj->value = tslider->svalue;
		tslider->last_svalue = tslider->svalue;

		gtk_adjustment_value_changed( adj );
	}

	g_signal_handler_unblock_by_data( G_OBJECT( adj ), tslider );
	g_signal_handler_unblock_by_data( G_OBJECT( entry ), tslider );
}

static void
tslider_class_init( TsliderClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	widget_class->destroy = tslider_destroy;

	class->changed = tslider_real_changed;
	class->slider_changed = NULL;
	class->activate = NULL;

	/* Create signals.
	 */
	tslider_signals[CHANGED] = g_signal_new( "changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( TsliderClass, changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	tslider_signals[ACTIVATE] = g_signal_new( "activate",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( TsliderClass, activate ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	tslider_signals[SLIDER_CHANGED] = g_signal_new( "slider_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( TsliderClass, slider_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	tslider_signals[TEXT_CHANGED] = g_signal_new( "text_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( TsliderClass, text_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );

	/* Init methods.
	 */
}

/* From/to/value have changed ... tell everyone.
 */
void
tslider_changed( Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_changed\n" );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( tslider ), tslider_signals[CHANGED], 0 );
}

/* Activated!
 */
static void
tslider_activate( Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_activate\n" );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( tslider ), tslider_signals[ACTIVATE], 0 );
}

/* Just the slider changed.
 */
static void
tslider_slider_changed( Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_slider_changed\n" );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( tslider ), 
		tslider_signals[SLIDER_CHANGED], 0 );
}

/* Text has been touched.
 */
static void
tslider_text_changed( Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_text_changed\n" );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( tslider ), tslider_signals[TEXT_CHANGED], 0 );
}

/* Enter in entry widget
 */
static void
tslider_value_activate_cb( GtkWidget *entry, Tslider *tslider )
{
	double value;

	if( !get_geditable_double( entry, &value ) ) {
		iwindow_alert( entry, GTK_MESSAGE_ERROR );
		return;
	}

	if( tslider->value != value ) {
		tslider->value = value;

		if( tslider->auto_link ) 
			tslider_changed( tslider );
		else
			tslider_activate( tslider );
	}
}

/* Drag on slider.
 */
static void
tslider_value_changed_cb( GtkAdjustment *adj, Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_value_changed_cb\n" );
#endif /*DEBUG*/

	if( tslider->svalue != adj->value ) {
		tslider->svalue = adj->value;

		if( tslider->auto_link ) {
			tslider->value = 
				tslider_slider_to_value( tslider, adj->value );

			tslider_changed( tslider );
		}
		else 
			tslider_slider_changed( tslider );
	}
}

/* Text has changed (and may need to be scanned later).
 */
static void
tslider_text_changed_cb( GtkWidget *widget, Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_text_changed_cb\n" );
#endif /*DEBUG*/

	tslider_text_changed( tslider );
}

/* Default identity conversion.
 */
static double
tslider_conversion_id( double from, double to, double value )
{
	return( value );
}

static gboolean
tslider_scroll_cb( GtkWidget *wid, GdkEvent *event, Tslider *tslider )
{
	gboolean handled;

	handled = FALSE;

	/* Stop any other scroll handlers running. We don't want the scroll 
	 * wheel to change widgets while we're moving.
	 */
	if( tslider->ignore_scroll )
		handled = TRUE;

	return( handled ); 
}

static void
tslider_init( Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_init: %p\n", tslider );
#endif /*DEBUG*/

	/* Any old start values ... overridden later.
	 */
	tslider->from = -1;
	tslider->to = -1;
	tslider->value = -1;
	tslider->svalue = -1;
	tslider->digits = -1;
	tslider->last_to = -1;
	tslider->last_from = -1;
	tslider->last_svalue = -1;
	tslider->ignore_scroll = TRUE;

        gtk_box_set_spacing( GTK_BOX( tslider ), 2 );

	tslider->entry = build_entry( 5 );
	gtk_entry_set_max_length( GTK_ENTRY( tslider->entry ), 10 );
        set_tooltip( tslider->entry, _( "Slider value ... edit!" ) );
        gtk_box_pack_start( GTK_BOX( tslider ), 
		tslider->entry, FALSE, FALSE, 0 );
        g_signal_connect( tslider->entry, "activate",
                G_CALLBACK( tslider_value_activate_cb ), tslider );
        g_signal_connect( tslider->entry, "changed",
                G_CALLBACK( tslider_text_changed_cb ), tslider );
	gtk_widget_show( tslider->entry );

        tslider->slider = gtk_hscale_new( NULL );
	tslider->adj = gtk_range_get_adjustment( GTK_RANGE( tslider->slider ) );
        gtk_range_set_update_policy( GTK_RANGE( tslider->slider ),
		GTK_UPDATE_CONTINUOUS );
#ifdef DEBUG
        gtk_range_set_update_policy( GTK_RANGE( tslider->slider ),
		GTK_UPDATE_DISCONTINUOUS );
#endif /*DEBUG*/
        gtk_scale_set_draw_value( GTK_SCALE( tslider->slider ), FALSE );
	gtk_widget_set_size_request( GTK_WIDGET( tslider->slider ), 100, -1 );
        gtk_box_pack_start( GTK_BOX( tslider ), 
		tslider->slider, TRUE, TRUE, 0 );
        set_tooltip( tslider->slider, _( "Left-drag to set number" ) );
        g_signal_connect( tslider->adj, "value_changed", 
		G_CALLBACK( tslider_value_changed_cb ), tslider );
	g_signal_connect( tslider->slider, "scroll-event", 
		G_CALLBACK( tslider_scroll_cb ), tslider );
	gtk_widget_show( tslider->slider );

	tslider->auto_link = TRUE;
	tslider->slider_to_value = tslider_conversion_id;
	tslider->value_to_slider = tslider_conversion_id;
}

Tslider *
tslider_new()
{
	Tslider *tslider = g_object_new( TYPE_TSLIDER, NULL );

	return( tslider );
}

void
tslider_set_conversions( Tslider *tslider, 
	tslider_fn value_to_slider, tslider_fn slider_to_value )
{
	tslider->value_to_slider = value_to_slider;
	tslider->slider_to_value = slider_to_value;

	tslider->auto_link = value_to_slider && slider_to_value;
}

double
tslider_log_value_to_slider( double from, double to, double value )
{
	/* What does 1.0 map to on our [0,1] scale?
	 */
	const double mapped1 = (1.0 - from) / (to - from);

	/* We want an exponent which maps the mid point on the slider to 1.
	 */
	const double a = log( mapped1 ) / log( 0.5 );

	const double nvalue = pow( value, 1.0 / a );

	return( nvalue );
}

double
tslider_log_slider_to_value( double from, double to, double value )
{
	/* What does 1.0 map to on our [0,1] scale?
	 */
	const double mapped1 = (1.0 - from) / (to - from);

	/* We want an exponent which maps the mid point on the slider to 1.
	 */
	const double a = log( mapped1 ) / log( 0.5 );

	const double nvalue = pow( value, a );

	return( nvalue );
}

void
tslider_set_ignore_scroll( Tslider *tslider, gboolean ignore_scroll )
{
	tslider->ignore_scroll = ignore_scroll;
}
