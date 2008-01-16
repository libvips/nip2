/* error window
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

static LogClass *parent_class = NULL;

static void *
error_print( Expr *expr, Error *error, gboolean *found )
{
	BufInfo buf;
	char txt[512];

	buf_init_static( &buf, txt, 512 );
	expr_error_print( expr, &buf );
	log_text( LOG( error ), buf_all( &buf ) );
	*found = TRUE;

	return( NULL );
}

static void
error_show_all( Error *error )
{
	gboolean found;

	found = FALSE;
	slist_map2( expr_error_all,
		(SListMap2Fn) error_print, error, &found );
	if( !found ) {
		log_text( LOG( error ), _( "No errors found." ) );
		log_text( LOG( error ), "\n" );
	}
}

static void
error_show_all_action_cb( GtkAction *action, Error *error )
{
	error_show_all( error );
}

static void *
unresolved_print( Toolkit *kit, Error *error, gboolean *found )
{
	BufInfo buf;
	char txt[512];

	buf_init_static( &buf, txt, 512 );
	toolkit_linkreport( kit, &buf, found );
	log_text( LOG( error ), buf_all( &buf ) );

	return( NULL );
}

static void
unresolved_show_all( Error *error )
{
	gboolean found;

	found = FALSE;
	(void) toolkitgroup_map( error->kitg,
		(toolkit_map_fn) unresolved_print, error, &found );
	if( !found ) {
		log_text( LOG( error ), _( "No unresolved symbols found." ) );
		log_text( LOG( error ), "\n" );
	}
}

static void
unresolved_show_all_action_cb( GtkAction *action, Error *error )
{
	unresolved_show_all( error );
}

/* Our actions.
 */
static GtkActionEntry error_actions[] = {
	/* Menu items.
	 */
	{ "FileMenu", NULL, "_File" },
	{ "ViewMenu", NULL, "_View" },
	{ "HelpMenu", NULL, "_Help" },

	/* Actions.
	 */
	{ "Clear", 
		NULL, N_( "_Clear" ), NULL, 
		N_( "Clear error window" ), 
		G_CALLBACK( log_clear_action_cb ) },

	{ "Errors", 
		NULL, N_( "List _Errors" ), NULL, 
		N_( "Search for all errors" ), 
		G_CALLBACK( error_show_all_action_cb ) },

	{ "Unresolved", 
		NULL, N_( "List _Unresolved" ), NULL, 
		N_( "Search for all unresolved references" ), 
		G_CALLBACK( unresolved_show_all_action_cb ) },

	{ "Close", 
		GTK_STOCK_CLOSE, N_( "_Close" ), NULL, 
		N_( "Close error window" ), 
		G_CALLBACK( iwindow_kill_action_cb ) },

	{ "Guide", 
		GTK_STOCK_HELP, N_( "_Contents" ), "F1",
		N_( "Open the users guide" ), 
		G_CALLBACK( mainw_guide_action_cb ) },

	{ "About", 
		NULL, N_( "_About" ), NULL,
		N_( "About this program" ), 
		G_CALLBACK( mainw_about_action_cb ) }
};

static const char *error_menubar_ui_description =
"<ui>"
"  <menubar name='ErrorMenubar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='Clear'/>"
"      <separator/>"
"      <menuitem action='Close'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='Errors'/>"
"      <menuitem action='Unresolved'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static void
error_class_init( ErrorClass *class )
{
	LogClass *log_class = (LogClass *) class;

	parent_class = g_type_class_peek_parent( class );

	log_class->actions = error_actions;
	log_class->n_actions = IM_NUMBER( error_actions );
	log_class->action_name = "ErrorActions";
	log_class->ui_description = error_menubar_ui_description;
	log_class->menu_bar_name = "/ErrorMenubar";
}

static void
error_init( Error *error )
{
}

GtkType
error_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Error",
			sizeof( Error ),
			sizeof( ErrorClass ),
			(GtkClassInitFunc) error_class_init,
			(GtkObjectInitFunc) error_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_LOG, &info );
	}

	return( type );
}

static void
error_link( Error *error, Toolkitgroup *kitg )
{
	error->kitg = kitg;

	destroy_if_destroyed( G_OBJECT( error ), 
		G_OBJECT( kitg ), (DestroyFn) gtk_widget_destroy );
        iwindow_set_title( IWINDOW( error ), 
		_( "Error - %s" ), IOBJECT( kitg )->name );
	gtk_window_set_default_size( GTK_WINDOW( error ), 640, 480 );
	iwindow_set_size_prefs( IWINDOW( error ), 
		"ERROR_WIDTH", "ERROR_HEIGHT" );
	iwindow_build( IWINDOW( error ) );
}

Error *
error_new( Toolkitgroup *kitg )
{
	Error *error = gtk_type_new( TYPE_ERROR );

	error_link( error, kitg );
	error_show_all( error );
	unresolved_show_all( error );

	return( error );
}
