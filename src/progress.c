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
	SIG_WATCH_CHANGED,	/* "changed" on one of our watches */
	SIG_LAST
};

static guint progress_signals[SIG_LAST] = { 0 };

static void 
progress_changed( Progress *progress, Watch *watch )
{
	g_signal_emit( G_OBJECT( progress ), 
		progress_signals[SIG_WATCH_CHANGED], 0, watch );
}

static void
progress_class_init( ProgressClass *class )
{
	progress_parent_class = g_type_class_peek_parent( class );

	progress_signals[SIG_WATCH_CHANGED] = g_signal_new( "watch_changed",
		G_OBJECT_CLASS_TYPE( class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ProgressClass, watch_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		G_TYPE_POINTER );
}

static void
progress_init( Progress *progress )
{
#ifdef DEBUG
	printf( "progress_init\n" );
#endif /*DEBUG*/

	progress->auto_save_timeout = 0;
}

GType
progress_get_type( void )
{
	static GType progress_type = 0;

	if( !progress_type ) {
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

		progress_type = g_type_register_static( TYPE_ICONTAINER, 
			"Progress", &info, 0 );
	}

	return( progress_type );
}

Progress *
progress_new( Workspacegroup *workspacegroup, const char *name )
{
	Progress *progress = PROGRESS( 
		g_object_new( TYPE_PROGRESS, NULL ) );

	/* Assume it's a static string.
	 */
	progress->name = name;

	progress->workspacegroup = workspacegroup;
	icontainer_set_hash( ICONTAINER( progress ) );

	return( progress );
}

/* Get the ws we are storing prefs in, and check it looks OK.
 */
static Workspace *
progress_get_workspace( Progress *progress )
{
	Compile *compile;
	Symbol *sym;

	if( !progress->workspacegroup->sym )
		return( NULL );

	compile = progress->workspacegroup->sym->expr->compile;

	if( !(sym = compile_lookup( compile, progress->name )) ||
		!sym->expr->compile ||
		sym->type != SYM_WORKSPACE ||
		!sym->ws )
		return( NULL );

	return( sym->ws );
}

static void
progress_save( Progress *progress )
{
	Workspace *ws;

	if( (ws = progress_get_workspace( progress )) ) {
		Filemodel *filemodel = FILEMODEL( ws );

		if( filemodel->modified ) {
			symbol_recalculate_all();

			/* Ignore error returns ... hmm! Tricky: we can come
			 * here during shutdown.
			 */
			(void) filemodel_save_all( filemodel, 
				filemodel->filename );

			filemodel_set_modified( filemodel, FALSE );
		}
	}
}

static gboolean
progress_dirty_timeout_cb( Progress *progress )
{
	progress->auto_save_timeout = 0;

	progress_save( progress );

	return( FALSE );
}

void
progress_dirty( Progress *progress )
{
	Workspace *ws;

	/* Find the preferences workspace.
	 */
	if( (ws = progress_get_workspace( progress )) ) {
		Filemodel *filemodel = FILEMODEL( ws );

		/* Mark ws dirty, start save timer.
		 */
		filemodel_set_modified( filemodel, TRUE );

		IM_FREEF( g_source_remove, progress->auto_save_timeout );
		progress->auto_save_timeout = g_timeout_add( 1000, 
			(GSourceFunc) progress_dirty_timeout_cb, progress );
	}
}

void
progress_flush( Progress *progress )
{
	/* Do we have a pending save?
	 */
	if( progress->auto_save_timeout ) {
		progress_save( progress );

		IM_FREEF( g_source_remove, progress->auto_save_timeout );
	}
}

/* Evaluation is progressing.
 */
static void
mainw_progress_update( int percent, int eta )
{
	char msg[100];

	if( mainw_cancel )
		sprintf( msg, _( "Cancelling ..." ) );
	else if( eta > 30 ) {
		int minutes = (eta + 30) / 60;

		im_snprintf( msg, 100, ngettext( 
			"%d minute left", 
			"%d minutes left", 
			minutes ), minutes );
	}
	else if( eta == 0 ) {
		/* A magic number reduce.c uses for eval feedback.
		 */
		VipsBuf buf;

		vips_buf_init_static( &buf, msg, 100 );
		/* Becomes eg. "Calculating A7.height ..."
		 */
		vips_buf_appends( &buf, _( "Calculating" ) );
		vips_buf_appends( &buf, " " );
		if( mainw_progress_expr ) {
			expr_name( mainw_progress_expr, &buf );
			vips_buf_appends( &buf, " " );
		}
		vips_buf_appends( &buf, "..." );
		vips_buf_all( &buf );
	}
	else
		im_snprintf( msg, 100, _( "%d seconds left" ), eta );

	slist_map2( mainw_all, 
		(SListMap2Fn) mainw_progress_update_mainw, 
		msg, &percent );
} 

/* End of eval.
 */
static void
mainw_progress_end( void )
{
	slist_map( mainw_all, 
		(SListMapFn) mainw_progress_hide, NULL ); 
	mainw_cancel = FALSE;
}

/* Busy handling. Come here from everywhere, handle delay and dispatch of busy
 * events to the cursor change system and to the busy ticker on mainw.
 */

static int mainw_busy_count = 0;
static GTimer *mainw_busy_timer = NULL;
static GTimer *mainw_busy_update_timer = NULL;

/* Delay before we start showing busy feedback.
 */
static const double mainw_busy_delay = 2.0;

/* Delay between busy updates.
 */
static const double mainw_busy_update = 0.2;

void
busy_progress( int percent, int eta )
{
	if( mainw_busy_count &&
		g_timer_elapsed( mainw_busy_timer, NULL ) > 
			mainw_busy_delay ) {

		if( g_timer_elapsed( mainw_busy_update_timer, NULL ) > 
			mainw_busy_update ) {
			animate_hourglass();
			mainw_progress_update( percent, eta );

			while( g_main_context_iteration( NULL, FALSE ) )
				;

			g_timer_start( mainw_busy_update_timer );
		}
	}
}

void
busy_begin( void )
{
	g_assert( mainw_busy_count >= 0 );

#ifdef DEBUG
	printf( "busy_begin: %d\n", mainw_busy_count );
#endif /*DEBUG*/

	mainw_busy_count += 1;

	if( mainw_busy_count == 1 ) {
		if( !mainw_busy_timer )
			mainw_busy_timer = g_timer_new();
		if( !mainw_busy_update_timer )
			mainw_busy_update_timer = g_timer_new();

		g_timer_start( mainw_busy_timer );
		g_timer_start( mainw_busy_update_timer );
	}
}

void
busy_end( void )
{
	mainw_busy_count -= 1;

#ifdef DEBUG
	printf( "busy_end: %d\n", mainw_busy_count );
#endif /*DEBUG*/

	g_assert( mainw_busy_count >= 0 );

	if( !mainw_busy_count ) {
		set_pointer();
		mainw_progress_end();
	}
}
