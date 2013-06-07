/* main processing window
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

static ViewClass *parent_class = NULL;

static void
workspacegroupview_realize( GtkWidget *widget )
{
#ifdef DEBUG
{
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( widget );
	Workspace *ws = WORKSPACE( VOBJECT( wsgview )->iobject );

	printf( "workspacegroupview_realize: %s\n", IOBJECT( ws )->name );
}
#endif /*DEBUG*/

	GTK_WIDGET_CLASS( parent_class )->realize( widget );

	/* Mark us as a symbol drag-to widget. 
	 */
	set_symbol_drag_type( widget );
}

static void
workspacegroupview_rename_sub( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Workspace *ws = WORKSPACE( client );
	Stringset *ss = STRINGSET( iwnd );
	StringsetChild *name = stringset_child_get( ss, _( "Name" ) );
	StringsetChild *caption = stringset_child_get( ss, _( "Caption" ) );

	char name_text[1024];
	char caption_text[1024];

	if( !get_geditable_name( name->entry, name_text, 1024 ) ||
		!get_geditable_string( caption->entry, caption_text, 1024 ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( !workspace_rename( ws, name_text, caption_text ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	nfn( sys, IWINDOW_YES );
}

static void                
workspacegroupview_rename_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	GtkWidget *ss = stringset_new();

	stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), IOBJECT( ws )->name, 
		_( "Set tab name here" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Caption" ), IOBJECT( ws )->caption, 
		_( "Set tab caption here" ) );

	iwindow_set_title( IWINDOW( ss ), 
		_( "Rename Tab \"%s\"" ), IOBJECT( ws )->name );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, ws );
	idialog_add_ok( IDIALOG( ss ), 
		workspacegroupview_rename_sub, _( "Rename Tab" ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( wview ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );
}

static void
workspacegroupview_rename_cb2( GtkWidget *wid, 
	Workspaceview *wview )
{
	workspacegroupview_rename_cb( wid, NULL, wview );
}

static void
workspacegroupview_child_add( View *parent, View *child )
{
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( parent );
	Workspaceview *wview = WORKSPACEVIEW( child );
	Workspace *ws = WORKSPACE( VOBJECT( child )->iobject );

	GtkWidget *ebox;
	GtkWidget *label;

	VIEW_CLASS( parent_class )->child_add( parent, child );

        ebox = gtk_event_box_new();
	gtk_widget_add_events( GTK_WIDGET( ebox ), 
		GDK_BUTTON_PRESS_MASK ); 
	label = gtk_label_new( NN( IOBJECT( ws->sym )->name ) );
        gtk_container_add( GTK_CONTAINER( ebox ), label );
        gtk_widget_show( GTK_WIDGET( label ) );
	workspaceview_set_label( wview, label );
	popup_attach( ebox, wsgview->tab_menu, wview );

        doubleclick_add( ebox, FALSE,
                NULL, NULL, 
		DOUBLECLICK_FUNC( workspacegroupview_rename_cb2 ), 
			wview );

	gtk_notebook_insert_page( GTK_NOTEBOOK( wsgview->notebook ),
		GTK_WIDGET( wview ), ebox, ICONTAINER( ws )->pos );
	gtk_notebook_set_tab_reorderable( GTK_NOTEBOOK( wsgview->notebook ),
		GTK_WIDGET( wview ), TRUE );
	gtk_notebook_set_tab_detachable( GTK_NOTEBOOK( wsgview->notebook ),
		GTK_WIDGET( wview ), TRUE );
}

static void
workspacegroupview_child_remove( View *parent, View *child )
{
	/* Stuff.
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( parent );
	Workspaceview *wview = WORKSPACEVIEW( child );

	 */

	VIEW_CLASS( parent_class )->child_remove( parent, child );
}

static void
workspacegroupview_child_position( View *parent, View *child )
{
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( parent );
	Workspaceview *wview = WORKSPACEVIEW( child );

	gtk_notebook_reorder_child( GTK_NOTEBOOK( wsgview->notebook ),
		GTK_WIDGET( wview ), ICONTAINER( wview )->pos );

	VIEW_CLASS( parent_class )->child_position( parent, child );
}

static void
workspacegroupview_child_front( View *parent, View *child )
{
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( parent );
	Workspaceview *wview = WORKSPACEVIEW( child );

	int page;
	GtkWidget *current_front;

	page = gtk_notebook_get_current_page( 
		GTK_NOTEBOOK( wsgview->notebook ) );
	current_front = gtk_notebook_get_nth_page( 
		GTK_NOTEBOOK( wsgview->notebook ), page );

	if( current_front != GTK_WIDGET( wview ) ) { 
		page = gtk_notebook_page_num( 
			GTK_NOTEBOOK( wsgview->notebook ), 
			GTK_WIDGET( wview ) );
		gtk_notebook_set_current_page( 
			GTK_NOTEBOOK( wsgview->notebook ),
			page );
	}
}

static void
workspacegroupview_class_init( WorkspacegroupviewClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	widget_class->realize = workspacegroupview_realize;

	view_class->child_add = workspacegroupview_child_add;
	view_class->child_remove = workspacegroupview_child_remove;
	view_class->child_position = workspacegroupview_child_position;
	view_class->child_front = workspacegroupview_child_front;
}

/* Called for switching the current page, and for page drags between
 * notebooks.
 */
static void                
workspacegroupview_switch_page_cb( GtkNotebook *notebook, 
	GtkWidget *page, guint page_num, gpointer user_data )
{
	Workspaceview *wview = WORKSPACEVIEW( page );
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroup *old_wsg = WORKSPACEGROUP( ICONTAINER( ws )->parent );
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( user_data );
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );

	if( ICONTAINER( ws )->parent != ICONTAINER( wsg ) ) {
		icontainer_reparent( ICONTAINER( wsg ), 
			ICONTAINER( ws ), -1 );

		filemodel_set_modified( FILEMODEL( wsg ), TRUE );
		filemodel_set_modified( FILEMODEL( old_wsg ), TRUE );

		/* If dragging the tab has emptied the old wsg, we can junk
		 * the window.
		 */
		mainw_cull();
	}

	icontainer_current( ICONTAINER( wsg ), ICONTAINER( ws ) );

	if( ws->compat_major ) { 
		error_top( _( "Compatibility mode." ) );
		error_sub( _( "This workspace was created by version %d.%d. "
			"A set of compatibility menus have been loaded "
			"for this window." ),
			ws->compat_major,
			ws->compat_minor ); 
		iwindow_alert( GTK_WIDGET( wview ), GTK_MESSAGE_INFO );
	}

	/* How bizarre, pages sometimes fail to set up correctly. Force a
	 * resize to get everything to init. 
	 */
	if( wview &&
		wview->fixed ) 
		gtk_container_check_resize( GTK_CONTAINER( wview->fixed ) );
}

static void                
workspacegroupview_page_added_cb( GtkNotebook *notebook, 
	GtkWidget *page, guint page_num, gpointer user_data )
{
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( user_data );
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );
	Mainw *mainw = MAINW( iwindow_get_root( GTK_WIDGET( notebook ) ) );

	filemodel_set_window_hint( FILEMODEL( wsg ), IWINDOW( mainw ) );
}

static GtkNotebook *                
workspacegroupview_create_window_cb( GtkNotebook *notebook, 
	GtkWidget *page, int x, int y, gpointer user_data )
{
	Workspaceview *wview = WORKSPACEVIEW( page );
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroup *wsg = WORKSPACEGROUP( ICONTAINER( ws )->parent );
	Workspaceroot *wsr = wsg->wsr; 

	Mainw *new_mainw;
	Workspacegroup *new_wsg;
	char name[256];

	/*
	printf( "workspacegroupview_create_window_cb: wsg = %s, ws = %s\n",
		NN( IOBJECT( wsg )->name ), NN( IOBJECT( ws )->name ) ); 
	printf( "workspacegroupview_create_window_cb: x = %d, y = %d\n", x, y );
	 */

	workspaceroot_name_new( wsr, name );
	new_wsg = workspacegroup_new( wsr );

	/*
	printf( "workspacegroupview_create_window_cb: new wsg = %s\n", name );
	 */

	iobject_set( IOBJECT( new_wsg ), name, NULL );
	new_mainw = mainw_new( new_wsg );
	gtk_window_move( GTK_WINDOW( new_mainw ), x, y );
	gtk_widget_show( GTK_WIDGET( new_mainw ) );

	return( GTK_NOTEBOOK( new_mainw->wsgview->notebook ) ); 
}

static void                
workspacegroupview_page_reordered_cb( GtkNotebook *notebook, 
	GtkWidget *page, guint page_num, gpointer user_data )
{
	Workspaceview *wview = WORKSPACEVIEW( page );
	Workspacegroupview *wsgview = 
		WORKSPACEGROUPVIEW( VIEW( wview )->parent );
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );

	int i;
	gboolean changed;

	changed = FALSE;

	for( i = 0; i < gtk_notebook_get_n_pages( notebook ); i++ ) {
		GtkWidget *page_n = gtk_notebook_get_nth_page( notebook, i );
		Workspaceview *wview = WORKSPACEVIEW( page_n );
		Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

		if( ICONTAINER( ws )->pos != i ) {
			ICONTAINER( ws )->pos = i;
			changed = TRUE;
		}
	}

	if( changed ) {
		icontainer_pos_sort( ICONTAINER ( wsg ) ); 
		filemodel_set_modified( FILEMODEL( wsg ), TRUE );
	}
}

static void
workspacegroupview_add_workspace_cb( GtkWidget *wid, 
	Workspacegroupview *wsgview )
{
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );

	if( !workspace_new_blank( wsg ) ) 
		iwindow_alert( GTK_WIDGET( wsgview ), GTK_MESSAGE_ERROR );
}

static void                
workspacegroupview_add_workspace_cb2( GtkWidget *wid, GtkWidget *host, 
	Workspacegroupview *wsgview )
{
	workspacegroupview_add_workspace_cb( wid, wsgview ); 
}

static void                
workspacegroupview_select_all_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

	workspace_select_all( ws );
}

static void                
workspacegroupview_duplicate_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

	if( !workspace_duplicate( ws ) ) {
		iwindow_alert( host, GTK_MESSAGE_ERROR );
		return;
	}
}

static void
workspacegroupview_merge_sub( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Workspace *ws = WORKSPACE( client );
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	char *filename;
	Column *col;

	if( (filename = filesel_get_filename( filesel )) ) {
		icontainer_current( ICONTAINER( wsg ), ICONTAINER( ws ) );

		progress_begin();

		column_clear_last_new();

		if( !workspace_merge_file( ws, filename ) ) 
			nfn( sys, IWINDOW_ERROR );
		else {
			symbol_recalculate_all();
			nfn( sys, IWINDOW_YES );
		}

		if( (col = column_get_last_new()) )
			column_scrollto( col, MODEL_SCROLL_TOP ); 

		progress_end();

		g_free( filename );
	}
	else
		nfn( sys, IWINDOW_ERROR );
}

static void                
workspacegroupview_merge_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	iWindow *iwnd = IWINDOW( view_get_toplevel( VIEW( wview ) ) );
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), 
		_( "Merge Into Tab \"%s\"" ), IOBJECT( ws )->name );
	filesel_set_flags( FILESEL( filesel ), FALSE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_workspace, 0 ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( iwnd ) );
	idialog_set_iobject( IDIALOG( filesel ), IOBJECT( ws ) );
	filesel_set_done( FILESEL( filesel ), 
		workspacegroupview_merge_sub, ws );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
workspacegroupview_save_as_sub( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Workspace *ws = WORKSPACE( client );
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	char *filename;

	if( (filename = filesel_get_filename( filesel )) ) {
		icontainer_current( ICONTAINER( wsg ), ICONTAINER( ws ) );
		if( !workspacegroup_save_current( wsg, filename ) ) 
			nfn( sys, IWINDOW_ERROR );
		else
			nfn( sys, IWINDOW_YES );

		g_free( filename );
	}
	else
		nfn( sys, IWINDOW_ERROR );
}

static void                
workspacegroupview_save_as_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	iWindow *iwnd = IWINDOW( view_get_toplevel( VIEW( wview ) ) );
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), 
		_( "Save Tab \"%s\"" ), IOBJECT( ws )->name );
	filesel_set_flags( FILESEL( filesel ), FALSE, TRUE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_workspace, 0 ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( iwnd ) );
	idialog_set_iobject( IDIALOG( filesel ), IOBJECT( ws ) );
	filesel_set_done( FILESEL( filesel ), 
		workspacegroupview_save_as_sub, ws );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

/* ws has been destroyed. 
 */
static void
workspacegroupview_delete_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	mainw_cull();

	nfn( sys, IWINDOW_YES );
}

static void                
workspacegroupview_delete_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

	model_check_destroy( view_get_toplevel( VIEW( wview ) ), 
		MODEL( ws ), workspacegroupview_delete_done_cb );
}

static void
workspacegroupview_init( Workspacegroupview *wsgview )
{
	wsgview->notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable( GTK_NOTEBOOK( wsgview->notebook ), TRUE );
	gtk_notebook_set_group_name( GTK_NOTEBOOK( wsgview->notebook ), 
		"wsgview" );
	gtk_notebook_set_tab_pos( GTK_NOTEBOOK( wsgview->notebook ), 
		GTK_POS_TOP );
	g_signal_connect( wsgview->notebook, "switch_page", 
		G_CALLBACK( workspacegroupview_switch_page_cb ), wsgview );
	g_signal_connect( wsgview->notebook, "page_added", 
		G_CALLBACK( workspacegroupview_page_added_cb ), wsgview );
	g_signal_connect( wsgview->notebook, "page_reordered", 
		G_CALLBACK( workspacegroupview_page_reordered_cb ), wsgview );
	g_signal_connect( wsgview->notebook, "create_window", 
		G_CALLBACK( workspacegroupview_create_window_cb ), wsgview );

        doubleclick_add( wsgview->notebook, FALSE,
                NULL, NULL, 
		DOUBLECLICK_FUNC( workspacegroupview_add_workspace_cb ), 
			wsgview );

	wsgview->gutter_menu = popup_build( _( "Tab gutter menu" ) );
	popup_add_but( wsgview->gutter_menu, _( "New Tab" ),
		POPUP_FUNC( workspacegroupview_add_workspace_cb2 ) ); 
	popup_attach( wsgview->notebook, wsgview->gutter_menu, wsgview );

#ifdef USE_NOTEBOOK_ACTION
{
	GtkWidget *but;
	GtkWidget *icon;

        but = gtk_button_new();
        gtk_button_set_relief( GTK_BUTTON( but ), GTK_RELIEF_NONE );
        set_tooltip( but, _( "Add a workspace" ) );
	icon = gtk_image_new_from_stock( GTK_STOCK_ADD, GTK_ICON_SIZE_MENU );
        gtk_container_add( GTK_CONTAINER( but ), icon );
	gtk_widget_show( icon );
	gtk_widget_show( but );
	gtk_notebook_set_action_widget( GTK_NOTEBOOK( wsgview->notebook ), 
		but, GTK_PACK_END );
        gtk_signal_connect( GTK_OBJECT( but ), "clicked",
                GTK_SIGNAL_FUNC( workspacegroupview_add_workspace_cb ), 
		wsgview );
}
#endif /*USE_NOTEBOOK_ACTION*/

	gtk_box_pack_start( GTK_BOX( wsgview ), 
		wsgview->notebook, TRUE, TRUE, 0 );
	gtk_widget_show( wsgview->notebook );

	wsgview->tab_menu = popup_build( _( "Tab menu" ) );
	popup_add_but( wsgview->tab_menu, _( "Rename" ),
		POPUP_FUNC( workspacegroupview_rename_cb ) ); 
	popup_add_but( wsgview->tab_menu, _( "Select All" ),
		POPUP_FUNC( workspacegroupview_select_all_cb ) ); 
	popup_add_but( wsgview->tab_menu, STOCK_DUPLICATE,
		POPUP_FUNC( workspacegroupview_duplicate_cb ) ); 
	popup_add_but( wsgview->tab_menu, _( "Merge Into Tab" ),
		POPUP_FUNC( workspacegroupview_merge_cb ) ); 
	popup_add_but( wsgview->tab_menu, GTK_STOCK_SAVE_AS,
		POPUP_FUNC( workspacegroupview_save_as_cb ) ); 
	menu_add_sep( wsgview->tab_menu );
	popup_add_but( wsgview->tab_menu, GTK_STOCK_DELETE,
		POPUP_FUNC( workspacegroupview_delete_cb ) ); 
}

GtkType
workspacegroupview_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( WorkspacegroupviewClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) workspacegroupview_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Workspacegroupview ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) workspacegroupview_init,
		};

		type = g_type_register_static( TYPE_VIEW, 
			"Workspacegroupview", &info, 0 );
	}

	return( type );
}

View *
workspacegroupview_new( void )
{
	Workspacegroupview *wsgview = gtk_type_new( TYPE_WORKSPACEGROUPVIEW );

	return( VIEW( wsgview ) );
}
