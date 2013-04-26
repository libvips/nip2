/* preferences dialog
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

#include "ip.h"

/* 
#define DEBUG
 */

static iDialogClass *parent_class = NULL;

static void
prefs_destroy( GtkObject *object )
{
	Prefs *prefs = PREFS( object );

#ifdef DEBUG
	printf( "prefs_destroy\n" );
#endif /*DEBUG*/

	if( prefs->ws ) {
		Workspacegroup *wsg = workspace_get_workspacegroup( prefs->ws );
		Filemodel *filemodel = FILEMODEL( wsg );

		/* Force a recalc, in case we've changed the autorecalc 
		 * settings. Also does a scan on any widgets.
		 */
		symbol_recalculate_all_force( TRUE );

		if( filemodel->modified &&
			filemodel_top_save( filemodel, filemodel->filename ) ) 
			filemodel_set_modified( filemodel, FALSE );
	}

	/* My instance destroy stuff.
	 */
	FREESID( prefs->destroy_sid, prefs->ws );
	IM_FREE( prefs->caption_filter );
	prefs->ws = NULL;

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
prefs_build( GtkWidget *widget )
{
	Prefs *prefs = PREFS( widget );
	GtkWidget *work;

#ifdef DEBUG
	printf( "prefs_build: %p\n", prefs );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	IWINDOW_CLASS( parent_class )->build( widget );

	work = IDIALOG( prefs )->work;

	prefs->pwview = PREFWORKSPACEVIEW( prefworkspaceview_new() );
	prefworkspaceview_set_caption_filter( prefs->pwview, 
		prefs->caption_filter );
	view_link( VIEW( prefs->pwview ), MODEL( prefs->ws ), NULL );

	if( prefs->caption_filter ) {
		gtk_box_pack_start( GTK_BOX( work ), 
			GTK_WIDGET( prefs->pwview ), TRUE, TRUE, 0 );

		gtk_widget_show( GTK_WIDGET( prefs->pwview ) );
	}
	else {
		/* No caption_filter set, so this is probably a big prefs
		 * window. Build a scrolledwindow for the content.
		 */
		GtkWidget *window;

		window = gtk_scrolled_window_new( NULL, NULL );
		gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( window ), 
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
		gtk_scrolled_window_add_with_viewport( 
			GTK_SCROLLED_WINDOW( window ), 
			GTK_WIDGET( prefs->pwview ) );
		gtk_viewport_set_shadow_type( 
			GTK_VIEWPORT( GTK_BIN( window )->child ), 
			GTK_SHADOW_NONE );
		gtk_box_pack_start( GTK_BOX( work ), 
			GTK_WIDGET( window ), TRUE, TRUE, 0 );

		gtk_widget_show( GTK_WIDGET( prefs->pwview ) );
		gtk_widget_show( window );
	}
}

static void
prefs_class_init( PrefsClass *class )
{
	GtkObjectClass *gobject_class = (GtkObjectClass *) class;
	iWindowClass *iwindow_class = (iWindowClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->destroy = prefs_destroy;

	iwindow_class->build = prefs_build;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
prefs_init( Prefs *prefs )
{
	prefs->ws = NULL;
	prefs->destroy_sid = 0;
}

GType
prefs_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( PrefsClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) prefs_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Prefs ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) prefs_init,
		};

		type = g_type_register_static( TYPE_IDIALOG, 
			"Prefs", &info, 0 );
	}

	return( type );
}

static void
prefs_workspace_destroy_cb( Workspace *ws, Prefs *prefs )
{
	prefs->destroy_sid = 0;
	prefs->ws = NULL;

	iwindow_kill( IWINDOW( prefs ) );
}

static void
prefs_link( Prefs *prefs, Workspace *ws )
{
	g_assert( !prefs->ws );

	prefs->ws = ws;
	prefs->ws->mode = WORKSPACE_MODE_NOEDIT;
	prefs->destroy_sid = g_signal_connect( ws, "destroy",
		G_CALLBACK( prefs_workspace_destroy_cb ), prefs );
}

static gint 
prefs_column_compare( Column *a, Column *b )
{
	return( b->y - a->y );
}

Prefs *
prefs_new( const char *caption_filter )
{
	Symbol *wsr_sym = main_workspaceroot->sym;
	Symbol *ws_sym = SYMBOL( icontainer_child_lookup( 
		ICONTAINER( wsr_sym->expr->compile ), "Preferences" ) );
	Prefs *prefs;

	if( !ws_sym ) {
		/* Probably failed to load prefs on startup for some reason.
		 */
		error_top( _( "Unable to display preferences." ) );
		error_sub( _( "No preferences workspace was found. "
			"Preferences probably failed to load when "
			"%s started." ),
			PACKAGE );
		return( NULL );
	}

	icontainer_custom_sort( ICONTAINER( ws_sym->ws ),
		(GCompareFunc) prefs_column_compare );
	prefs = PREFS( g_object_new( TYPE_PREFS, NULL ) );
	IM_SETSTR( prefs->caption_filter, caption_filter );
	prefs_link( prefs, ws_sym->ws );

	return( prefs );
}

gboolean
prefs_set( const char *name, const char *fmt, ... )
{
	Watch *watch;

	if( main_watchgroup &&
		(watch = watch_find( main_watchgroup, name )) ) {
		va_list args;

		va_start( args, fmt );
		watch_vset( watch, fmt, args );
		va_end( args );
	}

	return( TRUE );
}
