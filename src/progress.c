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
static const double progress_busy_delay = 2.0;

/* Delay between busy updates.
 */
static const double progress_update_interval = 0.2;

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

		g_signal_emit( G_OBJECT( progress ), 
			progress_signals[SIG_BEGIN], 0 );
	}
}

static void
progress_update_message( Progress *progress )
{
	vips_buf_rewind( &progress->feedback );

	if( progress->cancel )
		vips_buf_appends( &progress->feedback, _( "Cancelling ..." ) );
	else if( progress->expr ) {
		/* Becomes eg. "Calculating A7.height ..."
		 */
		vips_buf_appends( &progress->feedback, _( "Calculating" ) );
		vips_buf_appends( &progress->feedback, " " );
		expr_name( progress->expr, &progress->feedback );
		vips_buf_appends( &progress->feedback, " ..." );
	}
	else if( progress->eta > 30 ) {
		int minutes = (progress->eta + 30) / 60;

		vips_buf_appendf( &progress->feedback, ngettext( 
			"%d minute left", "%d minutes left", 
			minutes ), minutes );
	}
	else
		vips_buf_appendf( &progress->feedback, 
			_( "%d seconds left" ), progress->eta );
}

static void
progress_update( Progress *progress )
{
	if( progress->count &&
		g_timer_elapsed( progress->busy_timer, NULL ) > 
			progress_busy_delay ) {

		if( g_timer_elapsed( progress->update_timer, NULL ) > 
			progress_update_interval ) {
			gboolean result;

			progress_update_message( progress );
			g_signal_emit( G_OBJECT( progress ), 
				progress_signals[SIG_UPDATE], 
				progress->percent, progress->eta, &result );

			if( !result )
				progress->cancel = TRUE;

			while( g_main_context_iteration( NULL, FALSE ) )
				;

			g_timer_start( progress->update_timer );
		}
	}
}

gboolean
progress_update_percent( int percent, int eta )
{
	Progress *progress = progress_get();

	progress->percent = percent;
	progress->eta = eta;
	progress->expr = NULL;

	progress_update( progress );

	return( progress->cancel );
}

gboolean
progress_update_expr( Expr *expr )
{
	Progress *progress = progress_get();

	progress->percent = 0;
	progress->eta = 0;
	progress->expr = expr;

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
		progress->cancel = FALSE;
		g_signal_emit( G_OBJECT( progress ), 
			progress_signals[SIG_END], 0 );
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

	progress_signals[SIG_BEGIN] = g_signal_new( "update",
		G_OBJECT_CLASS_TYPE( class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ProgressClass, begin ),
		NULL, NULL,
		nip_BOOLEAN__INT_INT,
		G_TYPE_BOOLEAN, 2,
		G_TYPE_INT,
		G_TYPE_INT );

	progress_signals[SIG_BEGIN] = g_signal_new( "end",
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
	progress->eta = 0;
	progress->percent = 0;
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
