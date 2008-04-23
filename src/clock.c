/* a clock ... triggers a recomp every whenever
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

static ValueClass *parent_class = NULL;

static void
clock_dispose( GObject *gobject )
{
	Clock *clock = CLOCK( gobject );

#ifdef DEBUG
	printf( "clock_dispose\n" );
#endif /*DEBUG*/

	IM_FREEF( g_source_remove, clock->recalc_timeout );
	IM_FREEF( g_timer_destroy, clock->elapsed_timer );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
clock_done_cb( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys )
{
	Clock *clock = CLOCK( client );
	Stringset *ss = STRINGSET( iwnd );

	StringsetChild *interval = stringset_child_get( ss, _( "Interval" ) );
	StringsetChild *value = stringset_child_get( ss, _( "Elapsed time" ) );

	if( !get_geditable_double( interval->entry, &clock->interval ) ||
		!get_geditable_double( value->entry, &clock->value ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( clock->interval < 0.1 )
		clock->interval = 0.1;
	if( clock->value < 0.0 )
		clock->value = 0.0;

	/* Magic: ask for the clock timer to be reset.
	 */
	clock->time_offset = -1;

	classmodel_update( CLASSMODEL( clock ) );
	symbol_recalculate_all();

	nfn( sys, IWINDOW_YES );
}

static void 
clock_edit( GtkWidget *parent, Model *model )
{
	Clock *clock = CLOCK( model );
	GtkWidget *ss = stringset_new();
	char txt[256];
	BufInfo buf;

	im_snprintf( txt, 256, "%g", clock->interval );
	stringset_child_new( STRINGSET( ss ), 
		_( "Interval" ), txt, _( "Interval between ticks (seconds)" ) );
	im_snprintf( txt, 256, "%g", clock->value );
	stringset_child_new( STRINGSET( ss ), 
		_( "Elapsed time" ), txt, _( "Elapsed time (seconds)" ) );

	buf_init_static( &buf, txt, 100 );
	row_qualified_name( HEAPMODEL( clock )->row, &buf );
	iwindow_set_title( IWINDOW( ss ), 
		_( "Edit Clock \"%s\"" ), buf_all( &buf ) );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, clock );
	idialog_add_ok( IDIALOG( ss ), 
		clock_done_cb, _( "Set Clock" ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( parent ) );
	idialog_set_iobject( IDIALOG( ss ), IOBJECT( model ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );
}

static gboolean
clock_timeout_cb( Clock *clock )
{
#ifdef DEBUG
	printf( "clock_timeout_cb: " );
	row_name_print( HEAPMODEL( clock )->row );
	printf( " interval=%g, value=%g\n", clock->interval, clock->value );
#endif /*DEBUG*/

	/* Test autocalc ... if it's off, make sure we don't update the
	 * interface.
	 */
	if( mainw_auto_recalc ) {
		clock->value = g_timer_elapsed( clock->elapsed_timer, NULL ) + 
			clock->time_offset;

		classmodel_update( CLASSMODEL( clock ) );

		symbol_recalculate_all();
	}

	return( TRUE );
}

static void *
clock_update_model( Heapmodel *heapmodel )
{
	Clock *clock = CLOCK( heapmodel );

#ifdef DEBUG
	printf( "clock_update_model: " );
	row_name_print( HEAPMODEL( clock )->row );
	printf( " interval=%g, value=%g\n", clock->interval, clock->value );
#endif /*DEBUG*/

	if( HEAPMODEL_CLASS( parent_class )->update_model( heapmodel ) )
		return( heapmodel );

	/* Milliseconds for the update timeout ... don't let it go under 100,
	 * there's a danger the interface will lock up.
	 */
	int ms = IM_MAX( 100, clock->interval * 1000 );
	IM_FREEF( g_source_remove, clock->recalc_timeout );
	clock->recalc_timeout = g_timeout_add( ms, 
		(GSourceFunc) clock_timeout_cb, clock );

	/* Should we reset the timer from the value?
	 */
	if( clock->time_offset == -1 ) {
		g_timer_start( clock->elapsed_timer );
		clock->time_offset = clock->value;
	}

	return( NULL );
}

/* Override value_generate_caption(): pick from the model.
 */
static const char *
clock_generate_caption( iObject *iobject )
{
	Value *value = VALUE( iobject );
	ValueClass *value_class = VALUE_GET_CLASS( value );
	Clock *clock = CLOCK( iobject );
	BufInfo *buf = &value->caption_buffer;

	buf_rewind( buf );
	if( !heapmodel_name( HEAPMODEL( value ), buf ) ) 
		buf_appends( buf, G_OBJECT_CLASS_NAME( value_class ) );
	buf_appendf( buf, " %g %g", clock->interval, clock->value );

	return( buf_all( buf ) );
}

/* Members of clock we automate.
 */
static ClassmodelMember clock_members[] = {
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		MEMBER_INTERVAL, "interval", N_( "Interval" ),
		G_STRUCT_OFFSET( Clock, interval ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Clock, value ) }
};

static void
clock_class_init( ClockClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = clock_dispose;

	iobject_class->generate_caption = clock_generate_caption;

	model_class->edit = clock_edit;

	heapmodel_class->update_model = clock_update_model;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = clock_members;
	classmodel_class->n_members = IM_NUMBER( clock_members );
}

static void
clock_init( Clock *clock )
{
#ifdef DEBUG
	printf( "clock_init\n" );
#endif /*DEBUG*/

	/* Overridden later. Just something sensible.
	 */
        clock->interval = 1;
        clock->value = 0;

	/* time_offset: set to -1 means we should set the offset from value on
	 * the next rebuild.
	 */
	clock->elapsed_timer = g_timer_new();
	clock->time_offset = -1;	
        clock->recalc_timeout = 0;

	iobject_set( IOBJECT( clock ), CLASS_CLOCK, NULL );
}

GType
clock_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ClockClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) clock_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Clock ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) clock_init,
		};

		type = g_type_register_static( TYPE_VALUE, 
			"Clock", &info, 0 );
	}

	return( type );
}
