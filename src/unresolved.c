/* Show unresolved symbols
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
unresolved_print( Toolkit *kit, Unresolved *unresolved, gboolean *found )
{
	BufInfo buf;
	char txt[512];

	buf_init_static( &buf, txt, 512 );
	toolkit_linkreport( kit, &buf, found );
	log_text( LOG( unresolved ), buf_all( &buf ) );

	return( NULL );
}

static void
unresolved_show_all( Unresolved *unresolved )
{
	gboolean found;

	found = FALSE;
	(void) toolkitgroup_map( unresolved->kitg,
		(toolkit_map_fn) unresolved_print, unresolved, &found );
	if( !found ) {
		log_text( LOG( unresolved ), 
			_( "No unresolved symbols found." ) );
		log_text( LOG( error ), "\n" );
	}
}

static void
unresolved_show_all_action_cb( GtkAction *action, Unresolved *unresolved )
{
	unresolved_show_all( unresolved );
}

/* Our actions.
 */
static GtkActionEntry unresolved_actions[] = {
	/* Menu items.
	 */
	{ "FileMenu", NULL, "_File" },
	{ "ViewMenu", NULL, "_View" },
	{ "HelpMenu", NULL, "_Help" },

	/* Actions.
	 */
	{ "Clear", 
		NULL, N_( "_Clear" ), NULL, 
		N_( "Clear unresolved window" ), 
		G_CALLBACK( log_clear_action_cb ) },

	{ "Unresolveds", 
		NULL, N_( "Find _Unresolved" ), NULL, 
		N_( "Search for all unresolved symbols" ), 
		G_CALLBACK( unresolved_show_all_action_cb ) },

	{ "Close", 
		GTK_STOCK_CLOSE, N_( "_Close" ), NULL, 
		N_( "Close unresolved window" ), 
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

static const char *unresolved_menubar_ui_description =
"<ui>"
"  <menubar name='UnresolvedMenubar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='Clear'/>"
"      <separator/>"
"      <menuitem action='Close'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='Unresolveds'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static void
unresolved_class_init( UnresolvedClass *class )
{
	LogClass *log_class = (LogClass *) class;

	parent_class = g_type_class_peek_parent( class );

	log_class->actions = unresolved_actions;
	log_class->n_actions = IM_NUMBER( unresolved_actions );
	log_class->action_name = "UnresolvedActions";
	log_class->ui_description = unresolved_menubar_ui_description;
	log_class->menu_bar_name = "/UnresolvedMenubar";
}

static void
unresolved_init( Unresolved *unresolved )
{
}

GtkType
unresolved_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Unresolved",
			sizeof( Unresolved ),
			sizeof( UnresolvedClass ),
			(GtkClassInitFunc) unresolved_class_init,
			(GtkObjectInitFunc) unresolved_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_LOG, &info );
	}

	return( type );
}

static void
unresolved_link( Unresolved *unresolved, Toolkitgroup *kitg )
{
	unresolved->kitg = kitg;

        iwindow_set_title( IWINDOW( unresolved ), _( "Unresolved" ) );
	gtk_window_set_default_size( GTK_WINDOW( unresolved ), 640, 480 );
	iwindow_set_size_prefs( IWINDOW( unresolved ), 
		"UNRESOLVED_WIDTH", "UNRESOLVED_HEIGHT" );
	iwindow_build( IWINDOW( unresolved ) );
}

Unresolved *
unresolved_new( Toolkitgroup *kitg )
{
	Unresolved *unresolved = gtk_type_new( TYPE_UNRESOLVED );

	unresolved_link( unresolved, kitg );
	unresolved_show_all( unresolved );

	return( unresolved );
}
