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

static void
error_class_init( ErrorClass *class )
{
	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
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

static void *
error_print( Expr *expr, Error *error )
{
	BufInfo buf;
	char txt[512];

	buf_init_static( &buf, txt, 512 );
	expr_error_print( expr, &buf );
	log_text( LOG( error ), buf_all( &buf ) );

	return( NULL );
}

static void
error_show_all( Error *error )
{
	slist_map( expr_error_all,
		(SListMapFn) error_print, error );
}

static void
error_show_all_action_cb( GtkAction *action, Error *error )
{
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
		NULL, N_( "Search for _Errors" ), NULL, 
		N_( "Search for all errors" ), 
		G_CALLBACK( error_show_all_action_cb ) },

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
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static void
error_build( Error *error, GtkWidget *vbox )
{
	GtkActionGroup *action_group;
	GtkUIManager *ui_manager;
	GtkAccelGroup *accel_group;
	GError *gerror;
	GtkWidget *mbar;

        /* Make main menu bar
         */
	action_group = gtk_action_group_new( "ErrorActions" );
	gtk_action_group_set_translation_domain( action_group, 
		GETTEXT_PACKAGE );
	gtk_action_group_add_actions( action_group, 
		error_actions, G_N_ELEMENTS( error_actions ), 
		GTK_WINDOW( error ) );

	ui_manager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group( ui_manager, action_group, 0 );

	accel_group = gtk_ui_manager_get_accel_group( ui_manager );
	gtk_window_add_accel_group( GTK_WINDOW( error ), accel_group );

	error = NULL;
	if( !gtk_ui_manager_add_ui_from_string( ui_manager,
		error_menubar_ui_description, -1, &gerror ) ) {
		g_message( "building menus failed: %s", gerror->message );
		g_error_free( gerror );
		exit( EXIT_FAILURE );
	}

	mbar = gtk_ui_manager_get_widget( ui_manager, 
		"/ErrorMenubar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mbar, FALSE, FALSE, 0 );
        gtk_widget_show( mbar );
}

static void
error_link( Error *error )
{
        iwindow_set_title( IWINDOW( error ), _( "Error" ) );
	gtk_window_set_default_size( GTK_WINDOW( error ), 640, 480 );
	iwindow_set_build( IWINDOW( error ), 
		(iWindowBuildFn) error_build, NULL, NULL, NULL );
	iwindow_set_size_prefs( IWINDOW( error ), 
		"ERROR_WIDTH", "ERROR_HEIGHT" );
	iwindow_build( IWINDOW( error ) );
}

Error *
error_new( void )
{
	Error *error = gtk_type_new( TYPE_ERROR );

	error_link( error );
	error_show_all( error );

	return( error );
}
