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
workspacegroupview_child_add( View *parent, View *child )
{
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( parent );
	Workspaceview *wview = WORKSPACEVIEW( child );
	Workspace *ws = WORKSPACE( VOBJECT( child )->iobject );

	GtkWidget *ebox;
	GtkWidget *label;

	printf( "workspacegroupview_child_add:\n" ); 

	VIEW_CLASS( parent_class )->child_add( parent, child );

        ebox = gtk_event_box_new();
	gtk_widget_add_events( GTK_WIDGET( ebox ), 
		GDK_BUTTON_PRESS_MASK ); 
	label = gtk_label_new( NN( IOBJECT( ws->sym )->name ) );
        gtk_container_add( GTK_CONTAINER( ebox ), label );
        gtk_widget_show( GTK_WIDGET( label ) );
	popup_attach( ebox, wsgview->tab_menu, wview );

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

	page = gtk_notebook_page_num( GTK_NOTEBOOK( wsgview->notebook ), 
		GTK_WIDGET( wview ) );
	gtk_notebook_set_current_page( GTK_NOTEBOOK( wsgview->notebook ),
		page );
}

static void 
workspacegroupview_refresh( vObject *vobject )
{
	/*
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( vobject );
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );
	 */

#ifdef DEBUG
	printf( "workspacegroupview_refresh: %s\n", IOBJECT( wsg )->name );
#endif /*DEBUG*/

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
workspacegroupview_class_init( WorkspacegroupviewClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	widget_class->realize = workspacegroupview_realize;

	vobject_class->refresh = workspacegroupview_refresh;

	view_class->child_add = workspacegroupview_child_add;
	view_class->child_remove = workspacegroupview_child_remove;
	view_class->child_position = workspacegroupview_child_position;
	view_class->child_front = workspacegroupview_child_front;
}

static void                
workspacegroupview_switch_page_cb( GtkNotebook *notebook, 
	GtkWidget *page, guint page_num, gpointer user_data )
{
	Workspaceview *wview = WORKSPACEVIEW( page );
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroupview *wsgview = WORKSPACEGROUPVIEW( user_data );
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );

	printf( "workspacegroupview_switch_page_cb:\n" ); 

	icontainer_child_current( ICONTAINER( wsg ), ICONTAINER( ws ) );

	if( ws->compat_major ) { 
		error_top( _( "Compatibility mode." ) );
		error_sub( _( "This workspace was created by version %d.%d.%d. "
			"A set of compatibility menus have been loaded "
			"for this window." ),
			FILEMODEL( wsg )->major,
			FILEMODEL( wsg )->minor,
			FILEMODEL( wsg )->micro );
		iwindow_alert( GTK_WIDGET( wview ), GTK_MESSAGE_INFO );
	}

	/* How bizarre, pages sometimes fail to set up correctly. Force a
	 * resize to get everything to init. 
	 */
	if( wview->fixed ) 
		gtk_container_check_resize( GTK_CONTAINER( wview->fixed ) );
}

static void                
workspacegroupview_page_removed_cb( GtkNotebook *notebook, 
	GtkWidget *page, guint page_num, gpointer user_data )
{
	Workspaceview *wview = WORKSPACEVIEW( page );
	Workspacegroupview *wsgview = 
		WORKSPACEGROUPVIEW( VIEW( wview )->parent );
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );
	Mainw *mainw = MAINW( iwindow_get_root( GTK_WIDGET( notebook ) ) );

	if( icontainer_get_n_children( ICONTAINER( wsg ) ) == 0 ) 
		iwindow_kill( IWINDOW( mainw ) );
}

static void                
workspacegroupview_page_added_cb( GtkNotebook *notebook, 
	GtkWidget *page, guint page_num, gpointer user_data )
{
	Workspaceview *wview = WORKSPACEVIEW( page );
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroup *wsg = WORKSPACEGROUP( ICONTAINER( ws )->parent );
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

	workspaceroot_name_new( wsr, name );
	new_wsg = workspacegroup_new( wsr );
	iobject_set( IOBJECT( new_wsg ), name, NULL );
	new_mainw = mainw_new( new_wsg );
	gtk_window_move( GTK_WINDOW( new_mainw ), x, y );
	gtk_widget_show( GTK_WIDGET( new_mainw ) );

	return( GTK_NOTEBOOK( new_mainw->wsgview->notebook ) ); 
}

static void                
workspacegroupview_reorder_tab_cb( GtkNotebook *notebook, 
	GtkWidget *page, guint page_num, gpointer user_data )
{
	/*
	Workspaceview *wview = WORKSPACEVIEW( page );
	Workspacegroupview *wsgview = 
		WORKSPACEGROUPVIEW( VIEW( wview )->parent );
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );
	Mainw *mainw = MAINW( iwindow_get_root( GTK_WIDGET( notebook ) ) );
	 */

	printf( "workspacegroupview_reorder_tab_cb: %d\n", page_num );
}

#ifdef USE_NOTEBOOK_ACTION
static void
workspacegroupview_add_workspace_cb( GtkWidget *wid, 
	Workspacegroupview *wsgview )
{
	Workspacegroup *wsg = WORKSPACEGROUP( VOBJECT( wsgview )->iobject );

	char name[256];
	Workspace *new_ws;

	workspaceroot_name_new( wsg->wsr, name );
	if( !(new_ws = workspace_new_blank( wsg, name )) ) {
		iwindow_alert( GTK_WIDGET( wsgview ), GTK_MESSAGE_ERROR );
		return;
	}
	model_front( MODEL( new_ws ) );
}
#endif /*USE_NOTEBOOK_ACTION*/

static void                
workspacegroupview_duplicate_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	Workspacegroup *new_wsg;
	Mainw *new_mainw;

	progress_begin();

	if( !(new_wsg = workspacegroup_duplicate( wsg )) ) {
		progress_end();
		iwindow_alert( host, GTK_MESSAGE_ERROR );
		return;
	}
	new_mainw = mainw_new( new_wsg );
	gtk_widget_show( GTK_WIDGET( new_mainw ) );
	model_front( MODEL( new_wsg ) );
	symbol_recalculate_all();

	progress_end();
}

static void                
workspacegroupview_save_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	filemodel_inter_save( IWINDOW( host ), FILEMODEL( wsg ) );
}

static void                
workspacegroupview_save_as_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	filemodel_inter_saveas( IWINDOW( host ), FILEMODEL( wsg ) );
}

static void                
workspacegroupview_close_cb( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	filemodel_inter_savenclose( IWINDOW( host ), FILEMODEL( wsg ) );
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
	g_signal_connect( wsgview->notebook, "page_removed", 
		G_CALLBACK( workspacegroupview_page_removed_cb ), wsgview );
	g_signal_connect( wsgview->notebook, "reorder_tab", 
		G_CALLBACK( workspacegroupview_reorder_tab_cb ), wsgview );
	g_signal_connect( wsgview->notebook, "create_window", 
		G_CALLBACK( workspacegroupview_create_window_cb ), wsgview );

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
	popup_add_but( wsgview->tab_menu, STOCK_DUPLICATE,
		POPUP_FUNC( workspacegroupview_duplicate_cb ) ); 
	popup_add_but( wsgview->tab_menu, GTK_STOCK_SAVE,
		POPUP_FUNC( workspacegroupview_save_cb ) ); 
	popup_add_but( wsgview->tab_menu, GTK_STOCK_SAVE_AS,
		POPUP_FUNC( workspacegroupview_save_as_cb ) ); 
	menu_add_sep( wsgview->tab_menu );
	popup_add_but( wsgview->tab_menu, GTK_STOCK_CLOSE,
		POPUP_FUNC( workspacegroupview_close_cb ) ); 
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


