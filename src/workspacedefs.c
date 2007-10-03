/* Workspace-local defs.
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

static ViewClass *parent_class = NULL;

static void
workspacedefs_destroy( GtkObject *object )
{
	Workspacedefs *workspacedefs = WORKSPACEDEFS( object );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
workspacedefs_refresh( vObject *vobject )
{
	Workspacedefs *workspacedefs = WORKSPACEDEFS( vobject );
	Mainw *mainw = workspacedefs->mainw;
	Workspace *ws = mainw->ws;

#ifdef DEBUG
	printf( "workspacedefs_refresh:\n" );
#endif /*DEBUG*/

	text_view_set_text( GTK_TEXT_VIEW( workspacedefs->text ), 
		ws->local_defs, TRUE );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
workspacedefs_class_init( WorkspacedefsClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = workspacedefs_destroy;

	vobject_class->refresh = workspacedefs_refresh;
}

/* "Process" in defs area.
 */
static void
workspacedefs_process_cb( GtkWidget *wid, Workspacedefs *workspacedefs )
{
	Mainw *mainw = workspacedefs->mainw;
	Workspace *ws = mainw->ws;
	char *txt;

	txt = text_view_get_text( GTK_TEXT_VIEW( workspacedefs->text ) );
	if( !workspace_local_set( ws, txt ) ) {
		text_view_select_text( GTK_TEXT_VIEW( workspacedefs->text ), 
			input_state.charpos - yyleng, input_state.charpos );
		box_alert( GTK_WIDGET( mainw ) );
	}
	g_free( txt );

	symbol_recalculate_all();
}

static void
workspacedefs_init( Workspacedefs *workspacedefs )
{
	GtkWidget *swin;
	GtkWidget *hbox;
	GtkWidget *but;

	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_box_pack_start( GTK_BOX( workspacedefs ), swin, TRUE, TRUE, 0 );
	gtk_widget_show( swin );
	workspacedefs->text = program_text_new();
	gtk_container_add( GTK_CONTAINER( swin ), workspacedefs->text );
	gtk_widget_show( workspacedefs->text );
	hbox = gtk_hbox_new( FALSE, 0 );
	gtk_box_pack_end( GTK_BOX( workspacedefs ), hbox, FALSE, FALSE, 0 );
	gtk_widget_show( hbox );
	but = gtk_button_new_with_label( _( "Process" ) );
	gtk_box_pack_end( GTK_BOX( hbox ), but, FALSE, FALSE, 0 );
	gtk_widget_show( but );
        g_signal_connect( G_OBJECT( but ), "clicked",
                G_CALLBACK( workspacedefs_process_cb ), workspacedefs );
}

GtkType
workspacedefs_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Workspacedefs",
			sizeof( Workspacedefs ),
			sizeof( WorkspacedefsClass ),
			(GtkClassInitFunc) workspacedefs_class_init,
			(GtkObjectInitFunc) workspacedefs_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_VOBJECT, &info );
	}

	return( type );
}

Workspacedefs *
workspacedefs_new( Mainw *mainw )
{
	Workspacedefs *workspacedefs = gtk_type_new( TYPE_WORKSPACEDEFS );

	workspacedefs->mainw = mainw;

	return( workspacedefs );
}

