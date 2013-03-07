/* Handle feedback about eval progress.
 */

/*

    Copyright (C) 1991-2003 The National Gallery

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your watch) any later version.

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
#define DEBUG_MEMUSE
#define DEBUG
 */

#include "ip.h"

static iContainerClass *progress_parent_class = NULL;

/* Our signals. 
 */
enum {
	SIG_BEGIN,		/* Switch to busy state */
	SIG_UPDATE,		/* Busy progress update */
	SIG_END,		/* End busy state */
	SIG_LAST
};

static guint progress_signals[SIG_LAST] = { 0 };

/* Delay before we start showing busy feedback.
 */
static const double progress_busy_delay = 1.0;

/* Delay between busy updates.
 */
static const double progress_update_interval = 0.1;

void
progress_begin( void )
{
	Progress *progress = progress_get();

	g_assert( progress->count >= 0 );

#ifdef DEBUG
	printf( "progress_begin: %d\n", progress->count );
#endif /*DEBUG*/

	progress->count += 1;

	if( progress->count == 1 ) {
		g_timer_start( progress->busy_timer );
		g_timer_start( progress->update_timer );

#ifdef DEBUG_MEMUSE
		printf( "progress_begin:\n" );
		im__print_all();
#endif /*DEBUG_MEMUSE*/
	}
}

static void
progress_update( Progress *progress )
{
	/* Don't show the process and cancel button for a bit.
	 */
	if( progress->count ) {
		double elapsed = g_timer_elapsed( progress->busy_timer, NULL );

		if( !progress->busy && 
			elapsed > progress_busy_delay ) {
#ifdef DEBUG
			printf( "progress_update: displaying progress bar\n" );
#endif /*DEBUG*/

			g_signal_emit( G_OBJECT( progress ), 
				progress_signals[SIG_BEGIN], 0 );
			progress->busy = TRUE;
		}
	}
 
	/* Update regularly, even if we're not inside a begin/end
	 * block.
	 */
	if( g_timer_elapsed( progress->update_timer, NULL ) > 
		progress_update_interval ) {
		gboolean cancel;

#ifdef DEBUG
		printf( "progress_update:\n" );
#endif /*DEBUG*/

		g_timer_start( progress->update_timer );

		/* Overwrite the message if we're cancelling.
		 */
		if( progress->cancel ) {
			vips_buf_rewind( &progress->feedback );
			vips_buf_appends( &progress->feedback, 
				_( "Cancelling" ) );
			vips_buf_appends( &progress->feedback, " ..." );
		}

		cancel = FALSE;
		g_signal_emit( progress, 
			progress_signals[SIG_UPDATE], 0, &cancel );
		if( cancel )
			progress->cancel = TRUE;

		/* Rather dangerous, but we need this to give nice updates
		 * for the feedback thing.
		 */
		process_events();

#ifdef DEBUG_MEMUSE
		printf( "progress_update:\n" );
		im__print_all();
#endif /*DEBUG_MEMUSE*/
	}
}

gboolean
progress_update_percent( int percent, int eta )
{
	Progress *progress = progress_get();

	vips_buf_rewind( &progress->feedback );
	if( eta > 30 ) {
		int minutes = (eta + 30) / 60;

		vips_buf_appendf( &progress->feedback, ngettext( 
			"%d minute left", "%d minutes left", 
			minutes ), minutes );
	}
	else if( eta > 5 ) 
		vips_buf_appendf( &progress->feedback, ngettext( 
			"%d second left", "%d seconds left", 
			eta ), eta );
	else
		/* The empty string changes the height of the prgress bar
		 * argh.
		 */
		vips_buf_appendf( &progress->feedback, " " );

	progress->percent = percent;

	progress_update( progress );

	return( progress->cancel );
}

gboolean
progress_update_expr( Expr *expr )
{
	Progress *progress = progress_get();

	vips_buf_rewind( &progress->feedback );
	vips_buf_appends( &progress->feedback, _( "Calculating" ) );
	if( expr ) {
		vips_buf_appends( &progress->feedback, " " );
		expr_name( expr, &progress->feedback );
	}
	vips_buf_appends( &progress->feedback, " ..." );
	progress->percent = 0;

	progress_update( progress );

	return( progress->cancel );
}

gboolean
progress_update_loading( int percent, const char *filename )
{
	Progress *progress = progress_get();

	vips_buf_rewind( &progress->feedback );
	vips_buf_appends( &progress->feedback, _( "Loading" ) );
	vips_buf_appendf( &progress->feedback, " \"%s\"", filename );
	progress->percent = percent;

	progress_update( progress );

	return( progress->cancel );
}

gboolean
progress_update_tick( void )
{
	Progress *progress = progress_get();

	progress_update( progress );

	return( progress->cancel );
}

void
progress_end( void )
{
	Progress *progress = progress_get();

	progress->count -= 1;

#ifdef DEBUG
	printf( "progress_end: %d\n", progress->count );
#endif /*DEBUG*/

	g_assert( progress->count >= 0 );

	if( !progress->count ) {
		if( progress->busy )
			g_signal_emit( G_OBJECT( progress ), 
				progress_signals[SIG_END], 0 );

		progress->cancel = FALSE;
		progress->busy = FALSE;

#ifdef DEBUG_MEMUSE
		printf( "progress_end:\n" );
		im__print_all();
#endif /*DEBUG_MEMUSE*/
	}
}

static void
progress_class_init( ProgressClass *class )
{
	progress_parent_class = g_type_class_peek_parent( class );

	progress_signals[SIG_BEGIN] = g_signal_new( "begin",
		G_OBJECT_CLASS_TYPE( class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ProgressClass, begin ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );

	progress_signals[SIG_UPDATE] = g_signal_new( "update",
		G_OBJECT_CLASS_TYPE( class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ProgressClass, update ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		G_TYPE_POINTER );

	progress_signals[SIG_END] = g_signal_new( "end",
		G_OBJECT_CLASS_TYPE( class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ProgressClass, end ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
}

static void
progress_init( Progress *progress )
{
#ifdef DEBUG
	printf( "progress_init\n" );
#endif /*DEBUG*/

	progress->count = 0;
	progress->busy_timer = g_timer_new();
	progress->update_timer = g_timer_new();
	progress->cancel = FALSE;
	progress->busy = FALSE;
	vips_buf_init_static( &progress->feedback, 
		progress->buf, PROGRESS_FEEDBACK_SIZE );
}

GType
progress_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ProgressClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) progress_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Progress ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) progress_init,
		};

		type = g_type_register_static( TYPE_IOBJECT, 
			"Progress", &info, 0 );
	}

	return( type );
}

static Progress *
progress_new( void )
{
	Progress *progress = PROGRESS( g_object_new( TYPE_PROGRESS, NULL ) );

	return( progress );
}

Progress *
progress_get( void ) 
{
	static Progress *progress = NULL;

	if( !progress )
		progress = progress_new();

	return( progress );
}
