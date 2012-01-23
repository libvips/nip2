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
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

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

	if( workspacedefs->mainw->ws->local_kit ) {
		int n = icontainer_get_n_children( ICONTAINER( 
			workspacedefs->mainw->ws->local_kit ) );

		vips_buf_appendf( &buf, ngettext( "%d definition", 
			"%d definitions", n ), n );
	}
	if( workspacedefs->errors ) {
		if( !vips_buf_is_empty( &buf ) )
			vips_buf_appendf( &buf, ", " ); 
		vips_buf_appendf( &buf, _( "errors" ) ); 
	}
	if( workspacedefs->changed ) {
		if( !vips_buf_is_empty( &buf ) )
			vips_buf_appendf( &buf, ", " ); 
		vips_buf_appendf( &buf, _( "modified" ) ); 
	}
	set_glabel( workspacedefs->status, "%s", vips_buf_all( &buf ) );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
workspacedefs_class_init( WorkspacedefsClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	vobject_class->refresh = workspacedefs_refresh;
}

static gboolean
workspacedefs_set_text_from_file( Workspacedefs *workspacedefs, 
	const char *fname )
{
	Mainw *mainw = workspacedefs->mainw;
	Workspace *ws = mainw->ws;

	workspacedefs->changed = FALSE;
	workspacedefs->errors = FALSE;
	if( !workspace_local_set_from_file( ws, fname ) ) {
		text_view_select_text( GTK_TEXT_VIEW( workspacedefs->text ), 
			input_state.charpos - yyleng, input_state.charpos );
		workspacedefs->errors = TRUE;

		return( FALSE );
	}

	symbol_recalculate_all();

	return( TRUE );
}

/* Callback from load browser.
 */
static void
workspacedefs_load_file_cb( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Workspacedefs *workspacedefs = WORKSPACEDEFS( client );
	char *fname;

	if( !(fname = filesel_get_filename( filesel )) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( !workspacedefs_set_text_from_file( workspacedefs, fname ) ) {
		g_free( fname );
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	g_free( fname );

	nfn( sys, IWINDOW_YES );
}

static void
workspacedefs_replace_cb( GtkWidget *wid, Workspacedefs *workspacedefs )
{
	Mainw *mainw = workspacedefs->mainw;
	GtkWidget *filesel;

	filesel = filesel_new();
	iwindow_set_title( IWINDOW( filesel ), 
		_( "Replace Definition From File" ) );
	filesel_set_flags( FILESEL( filesel ), FALSE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_definition, 0 ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( mainw ) );
	filesel_set_done( FILESEL( filesel ), 
		workspacedefs_load_file_cb, workspacedefs );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
workspacedefs_save_as_cb( GtkWidget *wid, Workspacedefs *workspacedefs )
{
	Mainw *mainw = workspacedefs->mainw;
	Workspace *ws = mainw->ws;

	if( ws->local_kit )
		filemodel_inter_saveas( IWINDOW( workspacedefs->mainw ), 
			FILEMODEL( ws->local_kit ) );
}

static gboolean
workspacedefs_set_text( Workspacedefs *workspacedefs, const char *txt )
{
	Mainw *mainw = workspacedefs->mainw;
	Workspace *ws = mainw->ws;

	workspacedefs->changed = FALSE;
	workspacedefs->errors = FALSE;
	workspacedefs->text_hash = g_str_hash( txt );
	if( !workspace_local_set( ws, txt ) ) {
		text_view_select_text( GTK_TEXT_VIEW( workspacedefs->text ), 
			input_state.charpos - yyleng, input_state.charpos );
		workspacedefs->errors = TRUE;

		return( FALSE );
	}

	symbol_recalculate_all();

	return( TRUE );
}

/* "Process" in defs area.
 */
static void
workspacedefs_process_cb( GtkWidget *wid, Workspacedefs *workspacedefs )
{
	char *txt;

#ifdef DEBUG
	printf( "workspacedefs_process_cb:\n" );
	printf( "\tchanged = FALSE\n" );
#endif /*DEBUG*/

	txt = text_view_get_text( GTK_TEXT_VIEW( workspacedefs->text ) );
	if( !workspacedefs_set_text( workspacedefs, txt ) )
		iwindow_alert( wid, GTK_MESSAGE_ERROR );
	g_free( txt );
}

static void
workspacedefs_init( Workspacedefs *workspacedefs )
{
	GtkWidget *pane;
	Popupbutton *popupbutton;
	GtkWidget *swin;
	GtkWidget *hbox;
	GtkWidget *but;

#ifdef DEBUG
	printf( "workspacedefs_init:\n" );
#endif /*DEBUG*/

	workspacedefs->changed = FALSE;
	workspacedefs->errors = FALSE;
	workspacedefs->text_hash = 0;

	pane = menu_build( _( "Workspace definitions" ) );
	menu_add_but( pane, _( "Replace From _File" ),
		GTK_SIGNAL_FUNC( workspacedefs_replace_cb ), workspacedefs );
	menu_add_but( pane, GTK_STOCK_SAVE_AS,
		GTK_SIGNAL_FUNC( workspacedefs_save_as_cb ), workspacedefs );

	hbox = gtk_hbox_new( FALSE, 7 );
	gtk_box_pack_start( GTK_BOX( workspacedefs ), hbox, FALSE, FALSE, 0 );
	gtk_widget_show( hbox );

	popupbutton = popupbutton_new();
	popupbutton_set_menu( popupbutton, pane );
        gtk_box_pack_start( GTK_BOX( hbox ), GTK_WIDGET( popupbutton ), 
		FALSE, FALSE, 0 );
	gtk_widget_show( GTK_WIDGET( popupbutton ) );

	but = gtk_button_new_with_label( _( "Process" ) );
        g_signal_connect( G_OBJECT( but ), "clicked",
                G_CALLBACK( workspacedefs_process_cb ), workspacedefs );
	gtk_box_pack_start( GTK_BOX( hbox ), but, FALSE, FALSE, 0 );
	gtk_widget_show( but );
	workspacedefs->status = gtk_label_new( NULL );
	gtk_misc_set_alignment( GTK_MISC( workspacedefs->status ), 0, 0.5 );
	gtk_box_pack_start( GTK_BOX( hbox ), 
		workspacedefs->status, TRUE, TRUE, 0 );
	gtk_widget_show( workspacedefs->status );

	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_box_pack_end( GTK_BOX( workspacedefs ), swin, TRUE, TRUE, 0 );
	gtk_widget_show( swin );
	workspacedefs->text = program_text_new();
        g_signal_connect( gtk_text_view_get_buffer( 
		GTK_TEXT_VIEW( workspacedefs->text ) ), "changed",
                G_CALLBACK( workspacedefs_text_changed ), workspacedefs );
	gtk_container_add( GTK_CONTAINER( swin ), workspacedefs->text );
	gtk_widget_show( workspacedefs->text );
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

