#include "vipsdisp.h"

/*
#define DEBUG_VERBOSE
#define DEBUG
 */

enum {
	CHANGED,		
	ACTIVATE,	
	SLIDER_CHANGED,
	TEXT_CHANGED,	

	LAST
};

static guint tslider_signals[LAST] = { 0 };

/* Are two doubles more or less equal. We need this when we check the sliders
 * for update to stop loops. The 0.0001 is a bit of a fudge :-(
 */
#define DEQ( A, B ) (VIPS_ABS((A) - (B)) < 0.0001)

G_DEFINE_TYPE( Tslider, tslider, GTK_TYPE_WIDGET );

static void
tslider_dispose( GObject *object )
{
	Tslider *tslider = (Tslider *) object;

#ifdef DEBUG
	printf( "tslider_dispose:\n" ); 
#endif /*DEBUG*/

	VIPS_FREEF( gtk_widget_unparent, tslider->box );

	G_OBJECT_CLASS( tslider_parent_class )->dispose( object );
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

	g_signal_handlers_block_matched( G_OBJECT( adj ), 
		G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, tslider );
	g_signal_handlers_block_matched( G_OBJECT( entry ), 
		G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, tslider );

	/* Some libc's hate out-of-bounds precision, so clip, just in case.
	 */
	set_gentry( tslider->entry, "%.*f", 
		VIPS_CLIP( 0, tslider->digits, 100 ), tslider->value );
	gtk_scale_set_digits( GTK_SCALE( tslider->scale ), tslider->digits );

	if( !DEQ( tslider->from, tslider->last_from ) || 
		!DEQ( tslider->to, tslider->last_to ) ) {
		double range = tslider->to - tslider->from;

		gtk_adjustment_set_step_increment( adj, range / 100 ); 
		gtk_adjustment_set_page_increment( adj, range / 10 ); 
		gtk_adjustment_set_page_size( adj, range / 10 ); 
		gtk_adjustment_set_lower( adj, tslider->from ); 
		gtk_adjustment_set_upper( adj, 
			tslider->to + gtk_adjustment_get_page_size( adj ) ); 

		tslider->last_to = tslider->to;
		tslider->last_from = tslider->from;
	}

	if( !DEQ( tslider->svalue, tslider->last_svalue ) ) 
		gtk_adjustment_set_value( adj, tslider->svalue ); 

	g_signal_handlers_unblock_matched( G_OBJECT( adj ), 
		G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, tslider );
	g_signal_handlers_unblock_matched( G_OBJECT( entry ), 
		G_SIGNAL_MATCH_DATA, 0, 0, NULL, NULL, tslider );
}

/* From/to/value have changed ... tell everyone.
 */
void
tslider_changed( Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_changed\n" );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( tslider ), 
		tslider_signals[CHANGED], 0 );
}

/* Activated!
 */
static void
tslider_activate( Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_activate\n" );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( tslider ), 
		tslider_signals[ACTIVATE], 0 );
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

	g_signal_emit( G_OBJECT( tslider ), 
		tslider_signals[TEXT_CHANGED], 0 );
}

/* Enter in entry widget
 */
static void
tslider_value_activate_cb( GtkWidget *entry, Tslider *tslider )
{
	double value;

#ifdef DEBUG
	printf( "tslider_value_activate_cb:\n" );
#endif /*DEBUG*/

	if( get_geditable_double( entry, &value ) &&
		tslider->value != value ) {
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
	printf( "tslider_value_changed_cb:\n" );
#endif /*DEBUG*/

	if( tslider->svalue != gtk_adjustment_get_value( adj ) ) {
		tslider->svalue = gtk_adjustment_get_value( adj );

		if( tslider->auto_link ) {
			tslider->value = tslider_slider_to_value( 
				tslider, tslider->svalue ); 

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
	printf( "tslider_text_changed_cb:\n" );
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

static void
tslider_init( Tslider *tslider )
{
#ifdef DEBUG
	printf( "tslider_init:\n" ); 
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

	gtk_widget_init_template( GTK_WIDGET( tslider ) );
	
        g_signal_connect( tslider->entry, "activate",
                G_CALLBACK( tslider_value_activate_cb ), tslider );
        g_signal_connect( tslider->entry, "changed",
                G_CALLBACK( tslider_text_changed_cb ), tslider );

        g_signal_connect( tslider->adj, "value_changed", 
		G_CALLBACK( tslider_value_changed_cb ), tslider );

	tslider->auto_link = TRUE;
	tslider->slider_to_value = tslider_conversion_id;
	tslider->value_to_slider = tslider_conversion_id;

	tslider_changed( tslider );
}

#define BIND( field ) \
	gtk_widget_class_bind_template_child( GTK_WIDGET_CLASS( class ), \
		Tslider, field );

static void
tslider_class_init( TsliderClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS( class );

#ifdef DEBUG
	printf( "tslider_class_init:\n" ); 
#endif /*DEBUG*/

	G_OBJECT_CLASS( class )->dispose = tslider_dispose;

	class->changed = tslider_real_changed;

	gtk_widget_class_set_layout_manager_type( widget_class, 
		GTK_TYPE_BOX_LAYOUT );
	gtk_widget_class_set_template_from_resource( GTK_WIDGET_CLASS( class ),
		APP_PATH "/tslider.ui");

	BIND( adj );
	BIND( box );
	BIND( entry );
	BIND( scale );

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
		0, NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	tslider_signals[SLIDER_CHANGED] = g_signal_new( "slider_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		0, NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	tslider_signals[TEXT_CHANGED] = g_signal_new( "text_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		0, NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );

}

Tslider *
tslider_new( void ) 
{
	Tslider *tslider;

#ifdef DEBUG
	printf( "tslider_new:\n" ); 
#endif /*DEBUG*/

	tslider = g_object_new( TSLIDER_TYPE, NULL );

	return( tslider ); 
}

void
tslider_set_conversions( Tslider *tslider, 
	TsliderFn value_to_slider, TsliderFn slider_to_value )
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

