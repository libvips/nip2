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

static vObjectClass *parent_class = NULL;

static void
mainwtab_destroy( GtkObject *object )
{
	Mainwtab *mainwtab;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_MAINWTAB( object ) );

	mainwtab = MAINWTAB( object );

#ifdef DEBUG
	printf( "mainwtab_destroy\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	if( mainwtab->ws ) {
		mainwtab->ws->iwnd = NULL;

		filemodel_set_window_hint( FILEMODEL( mainwtab->ws ), NULL );
	}

	DESTROY_GTK( mainwtab->popup );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
mainwtab_map( GtkWidget *widget )
{
	Mainwtab *mainwtab = MAINWTAB( widget );

	if( mainwtab->ws->compat_major ) {
		error_top( _( "Compatibility mode." ) );
		error_sub( _( "This workspace was created by version %d.%d.%d. "
			"A set of compatibility menus have been loaded "
			"for this window." ),
			FILEMODEL( mainwtab->ws )->major,
			FILEMODEL( mainwtab->ws )->minor,
			FILEMODEL( mainwtab->ws )->micro );
		iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_INFO );
	}

	GTK_WIDGET_CLASS( parent_class )->map( widget );
}

static gboolean
mainwtab_configure_event( GtkWidget *widget, GdkEventConfigure *event )
{
	Mainwtab *mainwtab = MAINWTAB( widget );

	mainwtab->ws->window_width = event->width;
	mainwtab->ws->window_height = event->height;

	return( GTK_WIDGET_CLASS( parent_class )->
		configure_event( widget, event ) );
}

static gint
mainwtab_jump_name_compare( iContainer *a, iContainer *b )
{
	int la = strlen( IOBJECT( a )->name );
	int lb = strlen( IOBJECT( b )->name );

	/* Smaller names first.
	 */
	if( la == lb )
		return( strcmp( IOBJECT( a )->name, IOBJECT( b )->name ) );
	else
		return( la - lb );
}

static void
mainwtab_jump_column_cb( GtkWidget *item, Column *column )
{
	model_scrollto( MODEL( column ), MODEL_SCROLL_TOP );
}

static void *
mainwtab_jump_build( Column *column, GtkWidget *menu )
{
	GtkWidget *item;
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	vips_buf_appendf( &buf, "%s - %s", 
		IOBJECT( column )->name, IOBJECT( column )->caption );
	item = gtk_menu_item_new_with_label( vips_buf_all( &buf ) );
	g_signal_connect( item, "activate",
		G_CALLBACK( mainwtab_jump_column_cb ), column );
	gtk_menu_append( GTK_MENU( menu ), item );
	gtk_widget_show( item );

	return( NULL );
}

/* Update a menu with the set of current columns.
 */
void
mainwtab_jump_update( Mainwtab *mainwtab, GtkWidget *menu )
{
	GtkWidget *item;
	GSList *columns;

	gtk_container_foreach( GTK_CONTAINER( menu ),
		(GtkCallback) gtk_widget_destroy, NULL );

	item = gtk_tearoff_menu_item_new();
	gtk_menu_append( GTK_MENU( menu ), item );
	gtk_widget_show( item );

	columns = icontainer_get_children( ICONTAINER( mainwtab->ws ) );
        columns = g_slist_sort( columns, 
		(GCompareFunc) mainwtab_jump_name_compare );
	slist_map( columns, (SListMapFn) mainwtab_jump_build, menu );

	g_slist_free( columns );
}

static void 
mainwtab_refresh( vObject *vobject )
{
	Mainwtab *mainwtab = MAINWTAB( vobject );

#ifdef DEBUG
	printf( "mainwtab_refresh\n" );
#endif /*DEBUG*/

	mainwtab_jump_update( mainwtab, mainwtab->popup_jump );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
mainwtab_column_new_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_column_new_action_cb( NULL, mainw );
}

static void
mainwtab_workspace_merge_action_cb2( GtkWidget *wid, GtkWidget *host, 
	Mainw *mainw )
{
	mainw_workspace_merge_action_cb( NULL, mainw );
}

static void
mainwtab_layout_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_layout_action_cb( NULL, mainw );
}

static void
mainwtab_group_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_group_action_cb( NULL, mainw );
}

static void
mainwtab_next_error_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_next_error_action_cb( NULL, mainw );
}

static void
mainwtab_open_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_open_action_cb( NULL, mainw );
}

static void
mainwtab_link( vObject *vobject, iObject *iobject )
{
	Mainwtab *mainwtab = MAINWTAB( vobject );
	Workspace *ws = WORKSPACE( iobject );

	g_assert( !mainwtab->ws );

	mainwtab->ws = ws;

	mainwtab->popup = popup_build( _( "Workspace menu" ) );

	popup_add_but( mainwtab->popup, _( "New C_olumn" ),
		POPUP_FUNC( mainwtab_column_new_action_cb2 ) ); 
	mainwtab->popup_jump = popup_add_pullright( mainwtab->popup, 
		_( "Jump to _Column" ) ); 
	popup_add_but( mainwtab->popup, _( "Align _Columns" ),
		POPUP_FUNC( mainwtab_layout_action_cb2 ) ); 
	menu_add_sep( mainwtab->popup );
	popup_add_but( mainwtab->popup, GTK_STOCK_OPEN,
		POPUP_FUNC( mainwtab_open_action_cb2 ) ); 
	popup_add_but( mainwtab->popup, _( "_Merge Workspace from File" ),
		POPUP_FUNC( mainwtab_workspace_merge_action_cb2 ) ); 
	menu_add_sep( mainwtab->popup );
	popup_add_but( mainwtab->popup, _( "_Group Selected" ),
		POPUP_FUNC( mainwtab_group_action_cb2 ) ); 
	menu_add_sep( mainwtab->popup );
	popup_add_but( mainwtab->popup, STOCK_NEXT_ERROR,
		POPUP_FUNC( mainwtab_next_error_action_cb2 ) ); 
	popup_attach( mainwtab->wsview->fixed, mainwtab->popup, mainwtab );

	view_link( VIEW( mainwtab->wsview ), MODEL( ws ), NULL );

	vobject_link( VOBJECT( mainwtab->toolkitbrowser ), 
		IOBJECT( mainwtab->ws->kitg ) );

	vobject_link( VOBJECT( mainwtab->workspacedefs ), 
		IOBJECT( mainwtab->ws ) );

	pane_set_state( mainwtab->lpane, ws->lpane_open, ws->lpane_position );
	pane_set_state( mainwtab->rpane, ws->rpane_open, ws->rpane_position );

	VOBJECT_CLASS( parent_class )->link( vobject, iobject );
}

static void
mainwtab_class_init( MainwtabClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = mainwtab_destroy;

	widget_class->map = mainwtab_map;
	widget_class->configure_event = mainwtab_configure_event;

	widget_class->configure_event = mainwtab_configure_event;

	vobject_class->refresh = mainwtab_refresh;
	vobject_class->link = mainwtab_link;
}

static void
mainwtab_lpane_changed_cb( Pane *pane, Mainwtab *mainwtab )
{
	if( mainwtab->ws->lpane_open != pane->open ||
		mainwtab->ws->lpane_position != pane->user_position ) {
		mainwtab->ws->lpane_open = pane->open;
		mainwtab->ws->lpane_position = pane->user_position;

		iobject_changed( IOBJECT( mainwtab->ws ) );
	}
}

static void
mainwtab_rpane_changed_cb( Pane *pane, Mainwtab *mainwtab )
{
	if( mainwtab->ws->rpane_open != pane->open ||
		mainwtab->ws->rpane_position != pane->user_position ) {
		mainwtab->ws->rpane_open = pane->open;
		mainwtab->ws->rpane_position = pane->user_position;

		iobject_changed( IOBJECT( mainwtab->ws ) );
	}
}

static void
mainwtab_init( Mainwtab *mainwtab )
{
        GtkWidget *mbar;
	GtkWidget *frame;
	GtkWidget *ebox;
	GError *error;
	GtkWidget *cancel;
	GtkWidget *item;
	Panechild *panechild;

	mainwtab->ws = NULL;
	mainwtab->row_last_error = NULL;

#ifdef DEBUG
	printf( "mainwtab_init: %p\n", mainwtab );
#endif /*DEBUG*/

	mainwtab->rpane = pane_new( PANE_HIDE_RIGHT );
	g_signal_connect( mainwtab->rpane, "changed",
		G_CALLBACK( mainwtab_rpane_changed_cb ), mainwtab );
	gtk_box_pack_start( GTK_BOX( mainwtab ), 
		GTK_WIDGET( mainwtab->rpane ), FALSE, FALSE, 2 );
	gtk_widget_show( GTK_WIDGET( mainwtab->rpane ) );

	mainwtab->lpane = pane_new( PANE_HIDE_LEFT );
	g_signal_connect( mainwtab->lpane, "changed",
		G_CALLBACK( mainwtab_lpane_changed_cb ), mainwtab );
	gtk_paned_pack1( GTK_PANED( mainwtab->rpane ), 
		GTK_WIDGET( mainwtab->lpane ), TRUE, FALSE );
	gtk_widget_show( GTK_WIDGET( mainwtab->lpane ) );

	mainwtab->wsview = WORKSPACEVIEW( workspaceview_new() ); 
	gtk_paned_pack2( GTK_PANED( mainwtab->lpane ), 
		GTK_WIDGET( mainwtab->wsview ), TRUE, FALSE );
	gtk_widget_show( GTK_WIDGET( mainwtab->wsview ) );

	/* Toolkit Browser pane.
	 */
	panechild = panechild_new( mainwtab->rpane, 
		_( "Toolkit Browser" ) );

	/* Have to put toolkitbrowser in an ebox so the search entry gets
	 * clipped to the pane size.
	 */
	ebox = gtk_event_box_new();
	gtk_container_add( GTK_CONTAINER( panechild ), GTK_WIDGET( ebox ) );
	gtk_widget_show( ebox );

	mainwtab->toolkitbrowser = toolkitbrowser_new();

	gtk_container_add( GTK_CONTAINER( ebox ), 
		GTK_WIDGET( mainwtab->toolkitbrowser ) );
	gtk_widget_show( GTK_WIDGET( mainwtab->toolkitbrowser ) );

	/* Workspace-local defs pane.
	 */
	panechild = panechild_new( mainwtab->lpane, 
		_( "Workspace Definitions" ) );

	mainwtab->workspacedefs = workspacedefs_new();

	gtk_container_add( GTK_CONTAINER( panechild ), 
		GTK_WIDGET( mainwtab->workspacedefs ) );
	gtk_widget_show( GTK_WIDGET( mainwtab->workspacedefs ) );
}

GType
mainwtab_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( MainwtabClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) mainwtab_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Mainwtab ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) mainwtab_init,
		};

		type = g_type_register_static( TYPE_VOBJECT, 
			"Mainwtab", &info, 0 );
	}

	return( type );
}

Workspace *
mainwtab_get_workspace( Mainwtab *mainwtab )
{
	return( mainwtab->ws );
}

gboolean
mainwtab_clone( Mainwtab *mainwtab )
{
	Workspace *ws = mainwtab->ws;

	if( !workspace_selected_any( ws ) ) {
		Row *row;

		if( !(row = workspace_get_bottom( ws )) )
			return( FALSE );

		row_select( row );
	}

	/* Clone selected symbols.
	 */
	if( !workspace_clone_selected( ws ) )  
		return( FALSE );
	symbol_recalculate_all();
	workspace_deselect_all( ws );

	model_scrollto( MODEL( ws->current ), MODEL_SCROLL_TOP );

	return( TRUE );
}

/* Group the selected object(s).
 */
gboolean
mainwtab_group( Mainwtab *mainwtab )
{
	Workspace *ws = mainwtab->ws;
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	if( !workspace_selected_any( ws ) ) {
		Row *row;

		if( !(row = workspace_get_bottom( ws )) )
			return( FALSE );
		row_select( row );
	}

	vips_buf_appends( &buf, "Group [" );
	workspace_selected_names( ws, &buf, "," );
	vips_buf_appends( &buf, "]" );
	if( !workspace_add_def_recalc( ws, vips_buf_all( &buf ) ) ) 
		return( FALSE );
	workspace_deselect_all( ws );

	return( TRUE );
}

static Row *
mainwtab_test_error( Row *row, Mainwtab *mainwtab, int *found )
{
	g_assert( row->err );

	/* Found next?
	 */
	if( *found )
		return( row );

	if( row == mainwtab->row_last_error ) {
		/* Found the last one ... return the next one.
		 */
		*found = 1;
		return( NULL );
	}

	return( NULL );
}

/* FALSE for no errors.
 */
gboolean
mainwtab_next_error( Mainwtab *mainwtab )
{
	Workspace *ws = mainwtab->ws;

	char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	int found;

	if( !ws->errors ) 
		return( FALSE ); 

	/* Search for the one after the last one.
	 */
	found = 0;
	mainwtab->row_last_error = (Row *) slist_map2( ws->errors, 
		(SListMap2Fn) mainwtab_test_error, mainwtab, &found );

	/* NULL? We've hit end of table, start again.
	 */
	if( !mainwtab->row_last_error ) {
		found = 1;
		mainwtab->row_last_error = (Row *) slist_map2( ws->errors, 
			(SListMap2Fn) mainwtab_test_error, mainwtab, &found );
	}

	/* *must* have one now.
	 */
	g_assert( mainwtab->row_last_error && mainwtab->row_last_error->err );

	model_scrollto( MODEL( mainwtab->row_last_error ), MODEL_SCROLL_TOP );

	row_qualified_name( mainwtab->row_last_error->expr->row, &buf );
	vips_buf_appends( &buf, ": " );
	vips_buf_appends( &buf, mainwtab->row_last_error->expr->error_top );
	workspace_set_status( ws, "%s", vips_buf_firstline( &buf ) );

	return( TRUE ); 
}

Mainwtab *
mainwtab_new( void )
{
	Mainwtab *mainwtab;

	mainwtab = MAINWTAB( g_object_new( TYPE_MAINWTAB, NULL ) );

	return( mainwtab );
}
