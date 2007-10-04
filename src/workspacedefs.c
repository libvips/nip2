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
 */
#define DEBUG

#include "ip.h"

static ViewClass *parent_class = NULL;

static void
workspacedefs_destroy( GtkObject *object )
{
	Workspacedefs *workspacedefs = WORKSPACEDEFS( object );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
workspacedefs_text_changed( GtkTextBuffer *buffer, 
	Workspacedefs *workspacedefs )
{
#ifdef DEBUG
	printf( "workspacedefs_text_changed\n" );
#endif /*DEBUG*/

	if( !workspacedefs->changed ) {
		workspacedefs->changed = TRUE;

#ifdef DEBUG
		printf( "\t(changed = TRUE)\n" );
#endif /*DEBUG*/

		/* The workspace hasn't changed, but this will queue a refresh
		 * on us.
		 */
		iobject_changed( IOBJECT( workspacedefs->mainw->ws ) );
	}
}

static void
workspacedefs_refresh( vObject *vobject )
{
	Workspacedefs *workspacedefs = WORKSPACEDEFS( vobject );
	Mainw *mainw = workspacedefs->mainw;
	Workspace *ws = mainw->ws;
	BufInfo buf;
	char txt[256];

#ifdef DEBUG
	printf( "workspacedefs_refresh:\n" );
#endif /*DEBUG*/

	if( !workspacedefs->changed ) {
		guint text_hash = g_str_hash( ws->local_defs );

		if( text_hash != workspacedefs->text_hash ) {
			g_signal_handlers_block_by_func( 
				gtk_text_view_get_buffer( 
					GTK_TEXT_VIEW( workspacedefs->text ) ),
				workspacedefs_text_changed, workspacedefs );
			text_view_set_text( 
				GTK_TEXT_VIEW( workspacedefs->text ), 
				ws->local_defs, TRUE );
			g_signal_handlers_unblock_by_func( 
				gtk_text_view_get_buffer( 
					GTK_TEXT_VIEW( workspacedefs->text ) ),
				workspacedefs_text_changed, workspacedefs );

			workspacedefs->text_hash = text_hash;
		}
	}

	buf_init_static( &buf, txt, 512 );
	if( workspacedefs->mainw->ws->local_kit ) 
		buf_appendf( &buf, _( "%d definitions" ), 
			icontainer_get_n_children( ICONTAINER( 
				workspacedefs->mainw->ws->local_kit ) ) );
	if( workspacedefs->errors ) {
		buf_appendf( &buf, ", " ); 
		buf_appendf( &buf, _( "errors " ) ); 
	}
	if( workspacedefs->changed ) {
		buf_appendf( &buf, ", " ); 
		buf_appendf( &buf, _( "modified" ) ); 
	}
	set_glabel( workspacedefs->status, "%s", buf_all( &buf ) );

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

#ifdef DEBUG
	printf( "workspacedefs_process_cb:\n" );
	printf( "\tchanged = FALSE\n" );
#endif /*DEBUG*/

	txt = text_view_get_text( GTK_TEXT_VIEW( workspacedefs->text ) );
	workspacedefs->changed = FALSE;
	workspacedefs->errors = FALSE;
	if( !workspace_local_set( ws, txt ) ) {
		text_view_select_text( GTK_TEXT_VIEW( workspacedefs->text ), 
			input_state.charpos - yyleng, input_state.charpos );
		box_alert( GTK_WIDGET( mainw ) );
		workspacedefs->errors = TRUE;
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

#ifdef DEBUG
	printf( "workspacedefs_init:\n" );
#endif /*DEBUG*/

	workspacedefs->changed = FALSE;
	workspacedefs->errors = FALSE;
	workspacedefs->text_hash = 0;

	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_box_pack_start( GTK_BOX( workspacedefs ), swin, TRUE, TRUE, 0 );
	gtk_widget_show( swin );
	workspacedefs->text = program_text_new();
        g_signal_connect( gtk_text_view_get_buffer( 
		GTK_TEXT_VIEW( workspacedefs->text ) ), "changed",
                G_CALLBACK( workspacedefs_text_changed ), workspacedefs );
	gtk_container_add( GTK_CONTAINER( swin ), workspacedefs->text );
	gtk_widget_show( workspacedefs->text );
	hbox = gtk_hbox_new( FALSE, 0 );
	gtk_box_pack_end( GTK_BOX( workspacedefs ), hbox, FALSE, FALSE, 0 );
	gtk_widget_show( hbox );
	but = gtk_button_new_with_label( _( "Process" ) );
        g_signal_connect( G_OBJECT( but ), "clicked",
                G_CALLBACK( workspacedefs_process_cb ), workspacedefs );
	gtk_box_pack_end( GTK_BOX( hbox ), but, FALSE, FALSE, 0 );
	gtk_widget_show( but );
	workspacedefs->status = gtk_label_new( NULL );
	gtk_misc_set_alignment( GTK_MISC( workspacedefs->status ), 1.0, 0.5 );
	gtk_box_pack_end( GTK_BOX( hbox ), 
		workspacedefs->status, TRUE, TRUE, 0 );
	gtk_widget_show( workspacedefs->status );
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

