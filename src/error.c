/* ierror window
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
ierror_print( Expr *expr, iError *ierror, gboolean *found )
{
	VipsBuf buf;
	char txt[1024];

	vips_buf_init_static( &buf, txt, 1024 );
	expr_error_print( expr, &buf );
	log_text( LOG( ierror ), vips_buf_all( &buf ) );
	*found = TRUE;

	return( NULL );
}

static void
ierror_show_all( iError *ierror )
{
	gboolean found;

	found = FALSE;
	slist_map2( expr_error_all,
		(SListMap2Fn) ierror_print, ierror, &found );
	if( !found ) {
		log_text( LOG( ierror ), _( "No ierrors found." ) );
		log_text( LOG( ierror ), "\n" );
	}
}

static void
ierror_show_all_action_cb( GtkAction *action, iError *ierror )
{
	ierror_show_all( ierror );
}

static void *
unresolved_print_tool( Tool *tool, iError *ierror, gboolean *found )
{
	VipsBuf buf;
	char txt[MAX_STRSIZE];

	vips_buf_init_static( &buf, txt, MAX_STRSIZE );
	tool_linkreport_tool( tool, &buf, found );
	log_text( LOG( ierror ), vips_buf_all( &buf ) );

	return( NULL );
}

static void *
unresolved_print( Toolkit *kit, iError *ierror, gboolean *found )
{
	toolkit_map( kit, (tool_map_fn) unresolved_print_tool, ierror, found );

	return( NULL );
}

static void
unresolved_show_all( iError *ierror )
{
	gboolean found;

	found = FALSE;
	(void) toolkitgroup_map( ierror->kitg,
		(toolkit_map_fn) unresolved_print, ierror, &found );
	if( !found ) {
		log_text( LOG( ierror ), _( "No unresolved symbols found." ) );
		log_text( LOG( ierror ), "\n" );
	}
}

static void
unresolved_show_all_action_cb( GtkAction *action, iError *ierror )
{
	unresolved_show_all( ierror );
}

/* Our actions.
 */
static GtkActionEntry ierror_actions[] = {
	/* Menu items.
	 */
	{ "FileMenu", NULL, "_File" },
	{ "ViewMenu", NULL, "_View" },
	{ "HelpMenu", NULL, "_Help" },

	/* Actions.
	 */
	{ "Clear", 
		NULL, N_( "_Clear" ), NULL, 
		N_( "Clear ierror window" ), 
		G_CALLBACK( log_clear_action_cb ) },

	{ "iErrors", 
		NULL, N_( "List _iErrors" ), NULL, 
		N_( "Search for all ierrors" ), 
		G_CALLBACK( ierror_show_all_action_cb ) },

	{ "Unresolved", 
		NULL, N_( "List _Unresolved" ), NULL, 
		N_( "Search for all unresolved references" ), 
		G_CALLBACK( unresolved_show_all_action_cb ) },

	{ "Close", 
		GTK_STOCK_CLOSE, N_( "_Close" ), NULL, 
		N_( "Close ierror window" ), 
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

static const char *ierror_menubar_ui_description =
"<ui>"
"  <menubar name='iErrorMenubar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='Clear'/>"
"      <separator/>"
"      <menuitem action='Close'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='iErrors'/>"
"      <menuitem action='Unresolved'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static void
ierror_class_init( iErrorClass *class )
{
	LogClass *log_class = (LogClass *) class;

	parent_class = g_type_class_peek_parent( class );

	log_class->actions = ierror_actions;
	log_class->n_actions = IM_NUMBER( ierror_actions );
	log_class->action_name = "iErrorActions";
	log_class->ui_description = ierror_menubar_ui_description;
	log_class->menu_bar_name = "/iErrorMenubar";
}

static void
ierror_init( iError *ierror )
{
}

GtkType
ierror_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"iError",
			sizeof( iError ),
			sizeof( iErrorClass ),
			(GtkClassInitFunc) ierror_class_init,
			(GtkObjectInitFunc) ierror_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_LOG, &info );
	}

	return( type );
}

static void
ierror_link( iError *ierror, Toolkitgroup *kitg )
{
	ierror->kitg = kitg;

	destroy_if_destroyed( G_OBJECT( ierror ), 
		G_OBJECT( kitg ), (DestroyFn) gtk_widget_destroy );
        iwindow_set_title( IWINDOW( ierror ), 
		_( "iError - %s" ), IOBJECT( kitg )->name );
	gtk_window_set_default_size( GTK_WINDOW( ierror ), 640, 480 );
	iwindow_set_size_prefs( IWINDOW( ierror ), 
		"IERROR_WIDTH", "IERROR_HEIGHT" );
	iwindow_build( IWINDOW( ierror ) );
}

iError *
ierror_new( Toolkitgroup *kitg )
{
	iError *ierror = gtk_type_new( TYPE_IERROR );

	ierror_link( ierror, kitg );
	ierror_show_all( ierror );
	unresolved_show_all( ierror );

	return( ierror );
}
