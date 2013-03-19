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

static VObjectClass *parent_class = NULL;

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
	UNREF( mainwtab->kitgview );

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

static void
mainwtab_class_init( MainwtabClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = mainwtab_destroy;

	widget_class->map = mainwtab_map;
	widget_class->configure_event = mainwtab_configure_event;
}

static void
mainwtab_init( Mainwtab *mainwtab )
{
	mainwtab->ws = NULL;

	mainwtab->row_last_error = NULL;

	mainwtab->kitgview = NULL;
	mainwtab->toolkitbrowser = NULL;
	mainwtab->wsview = NULL;

	mainwtab->lpane = NULL;
	mainwtab->rpane = NULL;
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

		type = g_type_register_static( TYPE_IWINDOW, 
			"Mainwtab", &info, 0 );
	}

	return( type );
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
mainwtab_refresh( Mainwtab *mainwtab )
{
	iWindow *iwnd = IWINDOW( mainwtab );
	Workspace *ws = mainwtab->ws;

#ifdef DEBUG
#endif /*DEBUG*/
	printf( "mainwtab_refresh: %p\n", mainwtab );

	mainwtab_jump_update( mainwtab, mainwtab->popup_jump );
}

static void
mainwtab_workspace_changed_cb( Workspace *ws, Mainwtab *mainwtab )
{
	mainwtab_refresh( mainwtab );
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

/* Ungroup the selected object(s), or the bottom object.
 */
gboolean 
mainwtab_ungroup( Mainwtab *mainwtab )
{
	return( workspace_selected_ungroup( mainwtab->ws ) );
}

/* Group the selected object(s).
 */
static void
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

static void
mainwtab_group_action_cb2( GtkWidget *wid, GtkWidget *host, Mainwtab *mainwtab )
{
	mainwtab_group_action_cb( NULL, mainwtab );
}

/* Select all objects.
 */
static void
mainwtab_select_all_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	workspace_select_all( mainwtab->ws );
}

static void
mainwtab_find_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	error_top( _( "Not implemented." ) );
	error_sub( _( "Find in workspace not implemented yet." ) );
	iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_INFO );
}

static void
mainwtab_find_again_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	error_top( _( "Not implemented." ) );
	error_sub( _( "Find again in workspace not implemented yet." ) );
	iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_INFO );
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

/* Callback for next-error button.
 */
static void
mainwtab_next_error_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	int found;

	if( !mainwtab->ws->errors ) {
		error_top( _( "No errors." ) );
		error_sub( "%s", _( "There are no errors (that I can see) "
			"in this workspace." ) );
		iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_INFO );
		return;
	}

	/* Search for the one after the last one.
	 */
	found = 0;
	mainwtab->row_last_error = (Row *) slist_map2( mainwtab->ws->errors, 
		(SListMap2Fn) mainwtab_test_error, mainwtab, &found );

	/* NULL? We've hit end of table, start again.
	 */
	if( !mainwtab->row_last_error ) {
		found = 1;
		mainwtab->row_last_error = (Row *) slist_map2( mainwtab->ws->errors, 
			(SListMap2Fn) mainwtab_test_error, mainwtab, &found );
	}

	/* *must* have one now.
	 */
	g_assert( mainwtab->row_last_error && mainwtab->row_last_error->err );

	model_scrollto( MODEL( mainwtab->row_last_error ), MODEL_SCROLL_TOP );

	row_qualified_name( mainwtab->row_last_error->expr->row, &buf );
	vips_buf_appends( &buf, ": " );
	vips_buf_appends( &buf, mainwtab->row_last_error->expr->error_top );
	workspace_set_status( mainwtab->ws, "%s", vips_buf_firstline( &buf ) );
}

static void
mainwtab_next_error_action_cb2( GtkWidget *wid, GtkWidget *host, Mainwtab *mainwtab )
{
	mainwtab_next_error_action_cb( NULL, mainwtab );
}

/* Callback for box_yesno in mainwtab_force_calc_cb. Recalc selected items.
 */
static void
mainwtab_selected_recalc_dia( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Mainwtab *mainwtab = MAINWTAB( client );

	if( workspace_selected_recalc( mainwtab->ws ) )
		nfn( sys, IWINDOW_YES );
	else
		nfn( sys, IWINDOW_ERROR );
}

/* If symbols are selected, make them very dirty and recalculate. If not, 
 * just recalculate symbols which are already dirty.
 */
static void
mainwtab_force_calc_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	Workspace *ws = mainwtab->ws;

        if( workspace_selected_any( ws ) ) {
		char txt[30];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		workspace_selected_names( ws, &buf, ", " );

		box_yesno( GTK_WIDGET( mainwtab ), 
			mainwtab_selected_recalc_dia, iwindow_true_cb, mainwtab, 
			iwindow_notify_null, NULL,
			_( "Recalculate" ), 
			_( "Completely recalculate?" ),
			( "Are you sure you want to "
			"completely recalculate %s?" ), vips_buf_all( &buf ) );
	}
	else 
		/* Recalculate.
		 */
		symbol_recalculate_all_force( FALSE );
}

/* Callback for save workspace.
 */
static void
mainwtab_workspace_save_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	filemodel_inter_save( IWINDOW( mainwtab ), FILEMODEL( mainwtab->ws ) );
}

/* Callback for "save as .." workspace.
 */
static void
mainwtab_workspace_save_as_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	filemodel_inter_saveas( IWINDOW( mainwtab ), FILEMODEL( mainwtab->ws ) );
}

static Workspace *
mainwtab_open_workspace( Workspacegroup *wsg, const char *filename )
{
	Workspace *ws;
	Mainwtab *mainwtab;

	if( !(ws = workspace_new_from_file( wsg, filename, NULL )) ) 
		return( NULL );

	mainwtab = mainwtab_new( ws );
	gtk_widget_show( GTK_WIDGET( mainwtab ) );
	mainwtab_recent_add( &mainwtab_recent_workspace, filename );

	symbol_recalculate_all();

	return( ws );
}

/* Open a new workspace, close the current one if it's empty. 
 *
 * The idea is that if you have a blank workspace and do file-open, you want
 * to load into the blank. However, we can't actually do this, since the new
 * workspace might need a different set of menus. 
 *
 * We could sniff the file and then call workspace_merge_file() if the
 * workspace was compatible with the current one, but why bother.
 */
Workspace *
mainwtab_open_file_into_workspace( Mainwtab *mainwtab, const char *filename )
{
	Workspacegroup *wsg = WORKSPACEGROUP( ICONTAINER( mainwtab->ws )->parent );
	Workspace *new_ws;

	if( !(new_ws = mainwtab_open_workspace( wsg, filename )) ) 
		return( NULL );

	if( workspace_is_empty( mainwtab->ws ) ) {
		/* Make sure modified isn't set ... otherwise we'll get a
		 * "save before close" dialog.
		 */
		filemodel_set_modified( FILEMODEL( mainwtab->ws ), FALSE );
		iwindow_kill( IWINDOW( mainwtab ) );
	}

	return( new_ws );
}

/* Track these during a load.
 */
typedef struct {
	Workspacegroup *wsg;
	VipsBuf *buf;
	int nitems;
} MainwtabLoad;

/* Try to open a file. Workspace files we load immediately, other ones we
 * add the load to a buffer.
 */
static void *
mainwtab_open_fn( Filesel *filesel, const char *filename, MainwtabLoad *load )
{
	if( is_file_type( &filesel_wfile_type, filename ) ) {
		if( !mainwtab_open_workspace( load->wsg, filename ) )
			return( filesel );
	}
	else {
		if( load->nitems ) 
			vips_buf_appends( load->buf, ", " );
		if( !workspace_load_file_buf( load->buf, filename ) )
			return( filesel );
		mainwtab_recent_add( &mainwtab_recent_image, filename );
		load->nitems += 1;
	}

	return( NULL );
}

/* Callback from load browser.
 */
static void
mainwtab_open_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Mainwtab *mainwtab = MAINWTAB( client );
	Filesel *filesel = FILESEL( iwnd );
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );
	MainwtabLoad load;

	load.wsg = WORKSPACEGROUP( ICONTAINER( mainwtab->ws )->parent );
	load.buf = &buf;
	load.nitems = 0;

	if( filesel_map_filename_multi( filesel,
		(FileselMapFn) mainwtab_open_fn, &load, NULL ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	/* If there's nothing left to load (we only had workspaces to load and
	 * we've loaded them all) and the current workspace is smpty, we can 
	 * junk it.
	 */
	if( !load.nitems && 
		workspace_is_empty( mainwtab->ws ) ) {
		/* Make sure modified isn't set ... otherwise we'll get a
		 * "save before close" dialog.
		 */
		filemodel_set_modified( FILEMODEL( mainwtab->ws ), FALSE );
		iwindow_kill( IWINDOW( mainwtab ) );
	}

	/* Some actual files (image, matrix) were selected. Load into
	 * the current workspace.
	 */
	if( load.nitems ) {
		char txt2[MAX_STRSIZE];
		VipsBuf buf2 = VIPS_BUF_STATIC( txt2 );

		if( load.nitems > 1 )
			vips_buf_appendf( &buf2, "Group [%s]", 
				vips_buf_all( &buf ) );
		else
			vips_buf_appends( &buf2, vips_buf_all( &buf ) );

		if( !workspace_add_def_recalc( mainwtab->ws, 
			vips_buf_all( &buf2 ) ) ) {
			error_top( _( "Load failed." ) );
			error_sub( _( "Unable to execute:\n   %s" ), 
				vips_buf_all( &buf2 ) );
			nfn( sys, IWINDOW_ERROR );
			return;
		}
	}

	nfn( sys, IWINDOW_YES );
}

/* Show an open file dialog ... any type, but default to one of the image
 * ones.
 */
static void
mainwtab_open( Mainwtab *mainwtab )
{
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), _( "Open File" ) );
	filesel_set_flags( FILESEL( filesel ), TRUE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), 
		filesel_type_mainwtab, IMAGE_FILE_TYPE );
	filesel_set_filetype_pref( FILESEL( filesel ), "IMAGE_FILE_TYPE" );
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( mainwtab ) );
	filesel_set_done( FILESEL( filesel ), 
		mainwtab_open_done_cb, mainwtab );
	filesel_set_multi( FILESEL( filesel ), TRUE );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
mainwtab_open_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	mainwtab_open( mainwtab );
}

static void
mainwtab_open_action_cb2( GtkWidget *wid, GtkWidget *host, Mainwtab *mainwtab )
{
	mainwtab_open_action_cb( NULL, mainwtab );
}

/* Open one of the example workspaces ... just a shortcut.
 */
static void
mainwtab_open_examples( Mainwtab *mainwtab )
{
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), _( "Open File" ) );
	filesel_set_flags( FILESEL( filesel ), TRUE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), 
		filesel_type_workspace, 0 );
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( mainwtab ) );
	filesel_set_done( FILESEL( filesel ), 
		mainwtab_open_done_cb, mainwtab );
	filesel_set_multi( FILESEL( filesel ), TRUE );
	iwindow_build( IWINDOW( filesel ) );
	filesel_set_filename( FILESEL( filesel ), 
		"$VIPSHOME" G_DIR_SEPARATOR_S "share" G_DIR_SEPARATOR_S 
		PACKAGE G_DIR_SEPARATOR_S "data" G_DIR_SEPARATOR_S 
		"examples" G_DIR_SEPARATOR_S "1_point_mosaic" );

	/* Reset the filetype ... setting the filename will have changed it to
	 * 'all', since there's no suffix.
	 */
	filesel_set_filetype( FILESEL( filesel ), 
		filesel_type_workspace, 0 );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
mainwtab_open_examples_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	mainwtab_open_examples( mainwtab );
}

static gboolean
mainwtab_recent_open( Mainwtab *mainwtab, const char *filename )
{
	if( is_file_type( &filesel_wfile_type, filename ) ) {
		if( !mainwtab_open_file_into_workspace( mainwtab, filename ) )
			return( FALSE );
	}
	else {
		if( !workspace_load_file( mainwtab->ws, filename ) )
			return( FALSE );
	}

	return( TRUE );
}

static void
mainwtab_recent_open_cb( GtkWidget *widget, const char *filename )
{
	Mainwtab *mainwtab = MAINWTAB( iwindow_get_root( widget ) );

	progress_begin();
	if( !mainwtab_recent_open( mainwtab, filename ) ) {
		iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_ERROR );
		progress_end();
		return;
	}
	progress_end();

	symbol_recalculate_all();
}

static void
mainwtab_recent_build( GtkWidget *menu, GSList *recent )
{
	GSList *p;

	for( p = recent; p; p = p->next ) {
		const char *filename = (const char *) p->data;
		GtkWidget *item;
		char txt[80];
		VipsBuf buf = VIPS_BUF_STATIC( txt );
		char *utf8;

		vips_buf_appendf( &buf, "    %s", im_skip_dir( filename ) );
		utf8 = f2utf8( vips_buf_all( &buf ) );
		item = gtk_menu_item_new_with_label( utf8 );
		g_free( utf8 );
		utf8 = f2utf8( filename );
		set_tooltip( item, "%s", utf8 );
		g_free( utf8 );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_signal_connect( GTK_OBJECT( item ), "activate",
			GTK_SIGNAL_FUNC( mainwtab_recent_open_cb ), 
			(char *) filename );
	}
}

static void
mainwtab_recent_clear_cb( GtkWidget *widget, const char *filename )
{
	IM_FREEF( recent_free, mainwtab_recent_workspace );
	IM_FREEF( recent_free, mainwtab_recent_image );
	IM_FREEF( recent_free, mainwtab_recent_matrix );

	/* Need to remove files too to prevent merge on quit.
	 */
	(void) unlinkf( "%s" G_DIR_SEPARATOR_S "%s", 
		get_savedir(), RECENT_WORKSPACE );
	(void) unlinkf( "%s" G_DIR_SEPARATOR_S "%s", 
		get_savedir(), RECENT_IMAGE );
	(void) unlinkf( "%s" G_DIR_SEPARATOR_S "%s", 
		get_savedir(), RECENT_MATRIX );
}

static void
mainwtab_recent_map_cb( GtkWidget *widget, Mainwtab *mainwtab )
{
	GtkWidget *menu = mainwtab->recent_menu;
	GtkWidget *item;

	gtk_container_foreach( GTK_CONTAINER( menu ),
		(GtkCallback) gtk_widget_destroy, NULL );

	if( mainwtab_recent_image ) {
		item = gtk_menu_item_new_with_label( _( "Recent Images" ) );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_widget_set_sensitive( item, FALSE );
		mainwtab_recent_build( menu, mainwtab_recent_image );
	}

	if( mainwtab_recent_workspace ) {
		item = gtk_menu_item_new_with_label( _( "Recent Workspaces" ) );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_widget_set_sensitive( item, FALSE );
		mainwtab_recent_build( menu, mainwtab_recent_workspace );
	}

	if( mainwtab_recent_matrix ) {
		item = gtk_menu_item_new_with_label( _( "Recent Matricies" ) );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_widget_set_sensitive( item, FALSE );
		mainwtab_recent_build( menu, mainwtab_recent_matrix );
	}

	if( !mainwtab_recent_workspace && !mainwtab_recent_image &&
		!mainwtab_recent_matrix ) {
		item = gtk_menu_item_new_with_label( _( "No recent items" ) );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_widget_set_sensitive( item, FALSE );
	}
	else {
		item = gtk_menu_item_new_with_label( _( "Clear Recent Menu" ) );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_signal_connect( GTK_OBJECT( item ), "activate",
			GTK_SIGNAL_FUNC( mainwtab_recent_clear_cb ), NULL );
	}
}

/* Load a workspace at the top level.
 */
static void *
mainwtab_workspace_merge_fn( Filesel *filesel,
	const char *filename, void *a, void *b )
{
	Mainwtab *mainwtab = MAINWTAB( a );

	if( !workspace_merge_file( mainwtab->ws, filename, NULL ) )
		return( filesel );

	/* Process some events to make sure we rethink the layout and
	 * are able to get the append-at-RHS offsets.
	 */
	process_events();

	symbol_recalculate_all();
	mainwtab_recent_add( &mainwtab_recent_workspace, filename );

	return( NULL );
}

/* Callback from load browser.
 */
static void
mainwtab_workspace_merge_done_cb( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Mainwtab *mainwtab = MAINWTAB( client );

	if( filesel_map_filename_multi( filesel,
		mainwtab_workspace_merge_fn, mainwtab, NULL ) ) {
		symbol_recalculate_all();
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	symbol_recalculate_all();

	nfn( sys, IWINDOW_YES );
}

/* Merge ws file into current ws.
 */
static void
mainwtab_workspace_merge( Mainwtab *mainwtab )
{
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), 
		_( "Merge Workspace from File" ) );
	filesel_set_flags( FILESEL( filesel ), FALSE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_workspace, 0 ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( mainwtab ) );
	filesel_set_done( FILESEL( filesel ), 
		mainwtab_workspace_merge_done_cb, mainwtab );
	filesel_set_multi( FILESEL( filesel ), TRUE );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
mainwtab_workspace_duplicate_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	Workspace *new_ws;
	Mainwtab *new_mainwtab;

	progress_begin();

	if( !(new_ws = workspace_clone( mainwtab->ws )) ) {
		progress_end();
		iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_ERROR );
		return;
	}

	new_mainwtab = mainwtab_new( new_ws );
	gtk_widget_show( GTK_WIDGET( new_mainwtab ) );

	/* We have to show before recalculate or the window never appears, not
	 * sure why.
	 */
	symbol_recalculate_all();

	progress_end();
}

static void
mainwtab_workspace_merge_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	mainwtab_workspace_merge( mainwtab );
}

static void
mainwtab_workspace_merge_action_cb2( GtkWidget *wid, GtkWidget *host, 
	Mainwtab *mainwtab )
{
	mainwtab_workspace_merge_action_cb( NULL, mainwtab );
}

/* Auto recover.
 */
static void
mainwtab_recover_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	workspace_auto_recover( mainwtab );
}

/* Callback from make new column.
 */
static void
mainwtab_column_new_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	char *name;
	Column *col;

	name = workspace_column_name_new( mainwtab->ws, NULL );
	if( !(col = column_new( mainwtab->ws, name )) ) 
		iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_ERROR );
	else
		workspace_column_select( mainwtab->ws, col );
	IM_FREE( name );
}

/* Same, but from the popup.
 */
static void
mainwtab_column_new_action_cb2( GtkWidget *wid, GtkWidget *host, Mainwtab *mainwtab )
{
	mainwtab_column_new_action_cb( NULL, mainwtab );
}

/* Done button hit.
 */
static void
mainwtab_column_new_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Mainwtab *mainwtab = MAINWTAB( client );
	Stringset *ss = STRINGSET( iwnd );
	StringsetChild *name = stringset_child_get( ss, _( "Name" ) );
	StringsetChild *caption = stringset_child_get( ss, _( "Caption" ) );

	Column *col;

	char name_text[1024];
	char caption_text[1024];

	if( !get_geditable_name( name->entry, name_text, 1024 ) ||
		!get_geditable_string( caption->entry, caption_text, 1024 ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( !(col = column_new( mainwtab->ws, name_text )) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( strcmp( caption_text, "" ) != 0 )
		iobject_set( IOBJECT( col ), NULL, caption_text );

	workspace_column_select( mainwtab->ws, col );

	nfn( sys, IWINDOW_YES );
}

/* Make a new column with a specified name.
 */
static void
mainwtab_column_new_named_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	GtkWidget *ss = stringset_new();
	char *name;

	name = workspace_column_name_new( mainwtab->ws, NULL );

	stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), name, _( "Set column name here" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Caption" ), "", _( "Set column caption here" ) );

	iwindow_set_title( IWINDOW( ss ), _( "New Column" ) );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, mainwtab );
	idialog_add_ok( IDIALOG( ss ), 
		mainwtab_column_new_cb, _( "Create Column" ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( mainwtab ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );

	IM_FREE( name );
}

/* Callback from program.
 */
static void
mainwtab_program_new_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	Program *program;

	program = program_new( mainwtab->ws->kitg );

	gtk_widget_show( GTK_WIDGET( program ) ); 
}

/* New ws callback.
 */
static void
mainwtab_workspace_new_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	Workspacegroup *wsg = WORKSPACEGROUP( ICONTAINER( mainwtab->ws )->parent );

	workspacegroup_workspace_new( wsg, GTK_WIDGET( mainwtab ) );
}

/* Callback from auto-recalc toggle.
 */
static void
mainwtab_autorecalc_action_cb( GtkToggleAction *action, Mainwtab *mainwtab )
{
	GSList *i;

	mainwtab_auto_recalc = gtk_toggle_action_get_active( action );

	/* Yuk! We have to ask all mainwtab to refresh by hand, since we're not 
	 * using the prefs system for auto_recalc for reasons noted at top.
	 */
	for( i = mainwtab_all; i; i = i->next ) 
		mainwtab_refresh( MAINWTAB( i->data ) );

	if( mainwtab_auto_recalc )
		symbol_recalculate_all();
}

/* Callback from show toolbar toggle.
 */
static void
mainwtab_toolbar_action_cb( GtkToggleAction *action, Mainwtab *mainwtab )
{
	mainwtab->toolbar_visible = gtk_toggle_action_get_active( action );
	prefs_set( "MAINWTAB_TOOLBAR", 
		"%s", bool_to_char( mainwtab->toolbar_visible ) );
	mainwtab_refresh( mainwtab );
}

/* Callback from show statusbar toggle.
 */
static void
mainwtab_statusbar_action_cb( GtkToggleAction *action, Mainwtab *mainwtab )
{
	mainwtab->statusbar_visible = gtk_toggle_action_get_active( action );
	prefs_set( "MAINWTAB_STATUSBAR", 
		"%s", bool_to_char( mainwtab->statusbar_visible ) );
	mainwtab_refresh( mainwtab );
}

/* Expose/hide the toolkit browser.
 */
static void
mainwtab_toolkitbrowser_action_cb( GtkToggleAction *action, Mainwtab *mainwtab )
{
	if( gtk_toggle_action_get_active( action ) )
		pane_animate_open( mainwtab->rpane );
	else
		pane_animate_closed( mainwtab->rpane );
}

/* Expose/hide the workspace defs.
 */
static void
mainwtab_workspacedefs_action_cb( GtkToggleAction *action, Mainwtab *mainwtab )
{
	if( gtk_toggle_action_get_active( action ) )
		pane_animate_open( mainwtab->lpane );
	else
		pane_animate_closed( mainwtab->lpane );
}

/* Layout columns.
 */
static void
mainwtab_layout_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	model_layout( MODEL( mainwtab->ws ) );
}

static void
mainwtab_layout_action_cb2( GtkWidget *wid, GtkWidget *host, Mainwtab *mainwtab )
{
	mainwtab_layout_action_cb( NULL, mainwtab );
}

/* Remove selected items.
 */
static void
mainwtab_selected_remove_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	workspace_selected_remove_yesno( mainwtab->ws, GTK_WIDGET( mainwtab ) );
}

void 
mainwtab_revert_ok_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys ) 
{
	Mainwtab *mainwtab = MAINWTAB( client );

	if( FILEMODEL( mainwtab->ws )->filename ) {
		(void) unlinkf( "%s", FILEMODEL( mainwtab->ws )->filename );
		main_reload();
		symbol_recalculate_all();
	}

	nfn( sys, IWINDOW_YES );
}

void 
mainwtab_revert_cb( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys ) 
{
	Mainwtab *mainwtab = MAINWTAB( client );

	box_yesno( GTK_WIDGET( iwnd ),
		mainwtab_revert_ok_cb, iwindow_true_cb, mainwtab,
		nfn, sys,
		_( "Revert to Defaults" ), 
		_( "Revert to installation defaults?" ),
		_( "Would you like to reset all preferences to their factory "
		"settings? This will delete any changes you have ever made "
		"to your preferences and may take a few seconds." ) );
}

static void
mainwtab_preferences_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	Prefs *prefs;

	/* Can fail if there's no prefs ws, or an error on load.
	 */
	if( !(prefs = prefs_new( NULL )) ) {
		iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_ERROR );
		return;
	}

	iwindow_set_title( IWINDOW( prefs ), _( "Preferences" ) );
	iwindow_set_parent( IWINDOW( prefs ), GTK_WIDGET( mainwtab ) );
	idialog_set_callbacks( IDIALOG( prefs ), 
		NULL, NULL, NULL, mainwtab );
	idialog_add_ok( IDIALOG( prefs ), 
		mainwtab_revert_cb, _( "Revert to Defaults ..." ) );
	idialog_add_ok( IDIALOG( prefs ), iwindow_true_cb, GTK_STOCK_CLOSE );
	iwindow_build( IWINDOW( prefs ) );

	/* Just big enough to avoid a horizontal scrollbar on my machine. 

		FIXME ... Yuk! There must be a better way to do this! 
		Maybe a setting in prefs to suppress the h scrollbar?

	 */
	gtk_window_set_default_size( GTK_WINDOW( prefs ), 730, 480 );

	gtk_widget_show( GTK_WIDGET( prefs ) );
}

/* Make a magic definition for the selected symbol.

	FIXME .. paste this back when magic is reinstated

static void
mainwtab_magic_cb( gpointer callback_data, guint callback_action,
        GtkWidget *widget )
{
	Workspace *ws = main_workspacegroup->current;
	Row *row = workspace_selected_one( ws );

	if( !row )
		box_alert( mainwtab,
			"Select a single object with left-click, "
			"select Magic Definition." );
	else if( !magic_sym( row->sym ) )
		iwindow_alert( GTK_WIDGET( mainwtab ), GTK_MESSAGE_ERROR );
	else
		workspace_deselect_all( ws );
}
 */

#ifdef HAVE_LIBGVC
static void
mainwtab_graph_action_cb( GtkAction *action, Mainwtab *mainwtab )
{
	Graphwindow *graphwindow;

	graphwindow = graphwindow_new( mainwtab->ws, GTK_WIDGET( mainwtab ) );
	gtk_widget_show( GTK_WIDGET( graphwindow ) );
}
#endif /*HAVE_LIBGVC*/

/* Set display mode.
 */
static void
mainwtab_mode_action_cb( GtkRadioAction *action, GtkRadioAction *current, 
	Mainwtab *mainwtab )
{
	workspace_set_mode( mainwtab->ws, 
		gtk_radio_action_get_current_value( action ) );
}

/* Our actions.
 */
static GtkActionEntry mainwtab_actions[] = {
	/* Menu items.
	 */
	{ "RecentMenu", NULL, N_( "Open _Recent" ) },
	{ "JumpToColumnMenu", NULL, N_( "Jump to _Column" ) },
	{ "ToolkitsMenu", NULL, N_( "_Toolkits" ) },

	/* Dummy action ... replaced at runtime.
	 */
	{ "Stub", 
		NULL, "", NULL, "", NULL },

	/* Actions.
	 */
	{ "NewColumn", 
		GTK_STOCK_NEW, N_( "New C_olumn" ), NULL, 
		N_( "Create a new column" ), 
		G_CALLBACK( mainwtab_column_new_action_cb ) },

	{ "NewColumnName", 
		GTK_STOCK_NEW, N_( "New Named C_olumn" ), NULL, 
		N_( "Create a new column with a specified name" ), 
		G_CALLBACK( mainwtab_column_new_named_action_cb ) },

	{ "NewWorkspace", 
		GTK_STOCK_NEW, N_( "New _Workspace" ), NULL, 
		N_( "Create a new workspace" ), 
		G_CALLBACK( mainwtab_workspace_new_action_cb ) },

	{ "Open", 
		GTK_STOCK_OPEN, N_( "_Open" ), NULL,
		N_( "Open a file" ), 
		G_CALLBACK( mainwtab_open_action_cb ) },

	{ "OpenExamples", 
		NULL, N_( "Open _Examples" ), NULL,
		N_( "Open example workspaces" ), 
		G_CALLBACK( mainwtab_open_examples_action_cb ) },

	{ "DuplicateWorkspace", 
		STOCK_DUPLICATE, N_( "_Duplicate Workspace" ), NULL,
		N_( "Duplicate workspace" ), 
		G_CALLBACK( mainwtab_workspace_duplicate_action_cb ) },

	{ "Merge", 
		NULL, N_( "_Merge Workspace" ), NULL, 
		N_( "Merge workspace into this workspace" ), 
		G_CALLBACK( mainwtab_workspace_merge_action_cb ) },

	{ "Save", 
		GTK_STOCK_SAVE, N_( "_Save Workspace" ), NULL,
		N_( "Save workspace" ), 
		G_CALLBACK( mainwtab_workspace_save_action_cb ) },

	{ "SaveAs", 
		GTK_STOCK_SAVE_AS, N_( "_Save Workspace As" ), NULL,
		N_( "Save workspace as" ), 
		G_CALLBACK( mainwtab_workspace_save_as_action_cb ) },

	{ "Recover", 
		NULL, N_( "Search for Workspace _Backups" ), NULL,
		N_( "Load last automatically backed-up workspace" ), 
		G_CALLBACK( mainwtab_recover_action_cb ) },

	{ "Delete", 
		GTK_STOCK_DELETE, N_( "_Delete" ), "<control>BackSpace",
		N_( "Delete selected items" ), 
		G_CALLBACK( mainwtab_selected_remove_action_cb ) },

	{ "SelectAll", 
		NULL, N_( "Select _All" ), "<control>A",
		N_( "Select all items" ), 
		G_CALLBACK( mainwtab_select_all_action_cb ) },

	{ "Duplicate", 
		STOCK_DUPLICATE, N_( "D_uplicate Selected" ), "<control>U",
		N_( "Duplicate selected items" ), 
		G_CALLBACK( mainwtab_duplicate_action_cb ) },

	{ "Recalculate", 
		NULL, N_( "_Recalculate" ), NULL,
		N_( "Recalculate selected items" ), 
		G_CALLBACK( mainwtab_force_calc_action_cb ) },

	{ "AlignColumns", 
		NULL, N_( "Align _Columns" ), NULL,
		N_( "Align columns to grid" ), 
		G_CALLBACK( mainwtab_layout_action_cb ) },

	{ "Find", 
		GTK_STOCK_FIND, N_( "_Find" ), NULL,
		N_( "Find in workspace" ), 
		G_CALLBACK( mainwtab_find_action_cb ) },

	{ "FindNext", 
		NULL, N_( "Find _Next" ), NULL,
		N_( "Find again in workspace" ), 
		G_CALLBACK( mainwtab_find_again_action_cb ) },

	{ "NextError", 
		STOCK_NEXT_ERROR, NULL, NULL,
		N_( "Jump to next error" ), 
		G_CALLBACK( mainwtab_next_error_action_cb ) },

	{ "Group", 
		NULL, N_( "_Group" ), NULL,
		N_( "Group selected items" ), 
		G_CALLBACK( mainwtab_group_action_cb ) },

	{ "Ungroup", 
		NULL, N_( "U_ngroup" ), NULL,
		N_( "Ungroup selected items" ), 
		G_CALLBACK( mainwtab_ungroup_action_cb ) },

	{ "Preferences", 
		GTK_STOCK_PREFERENCES, N_( "_Preferences" ), NULL,
		N_( "Edit preferences" ), 
		G_CALLBACK( mainwtab_preferences_action_cb ) },

#ifdef HAVE_LIBGVC
	{ "Graph", 
		NULL, N_( "Workspace as Grap_h" ), NULL,
		N_( "Show a graph of workspace dependencies" ), 
		G_CALLBACK( mainwtab_graph_action_cb ) },
#endif /*HAVE_LIBGVC*/

	{ "EditToolkits", 
		NULL, N_( "_Edit" ), NULL,
		N_( "Edit toolkits" ), 
		G_CALLBACK( mainwtab_program_new_action_cb ) }
};

static GtkToggleActionEntry mainwtab_toggle_actions[] = {
	{ "AutoRecalculate",
		NULL, N_( "Au_to Recalculate" ), NULL,
		N_( "Recalculate automatically on change" ),
		G_CALLBACK( mainwtab_autorecalc_action_cb ), TRUE },

	{ "Toolbar",
		NULL, N_( "_Toolbar" ), NULL,
		N_( "Show window toolbar" ),
		G_CALLBACK( mainwtab_toolbar_action_cb ), TRUE },

	{ "Statusbar",
		NULL, N_( "_Statusbar" ), NULL,
		N_( "Show window statusbar" ),
		G_CALLBACK( mainwtab_statusbar_action_cb ), TRUE },

	{ "ToolkitBrowser",
		NULL, N_( "Toolkit _Browser" ), NULL,
		N_( "Show toolkit browser" ),
		G_CALLBACK( mainwtab_toolkitbrowser_action_cb ), FALSE },

	{ "WorkspaceDefs",
		NULL, N_( "Workspace _Definitions" ), NULL,
		N_( "Show workspace definitions" ),
		G_CALLBACK( mainwtab_workspacedefs_action_cb ), FALSE },
};

static GtkRadioActionEntry mainwtab_radio_actions[] = {
	{ "Normal",
		NULL, N_( "_Normal" ), NULL,
		N_( "Normal view mode" ),
		WORKSPACE_MODE_REGULAR },

	{ "ShowFormula",
		NULL, N_( "Show _Formula" ), NULL,
		N_( "Show formula view mode" ),
		WORKSPACE_MODE_FORMULA },

	{ "NoEdit",
		NULL, N_( "No _Edits" ), NULL,
		N_( "No edits view mode" ),
		WORKSPACE_MODE_NOEDIT },
};

static const char *mainwtab_menubar_ui_description =
"<ui>"
"  <menubar name='MainwtabMenubar'>"
"    <menu action='FileMenu'>"
"      <menu action='NewMenu'>"
"        <menuitem action='NewColumnName'/>"
"        <menuitem action='NewWorkspace'/>"
"      </menu>"
"      <menuitem action='Open'/>"
"      <menu action='RecentMenu'>"
"        <menuitem action='Stub'/>"	/* Dummy ... replaced on map */
"      </menu>"
"      <menuitem action='OpenExamples'/>"
"      <separator/>"
"      <menuitem action='DuplicateWorkspace'/>"
"      <menuitem action='Merge'/>"
"      <menuitem action='Save'/>"
"      <menuitem action='SaveAs'/>"
"      <separator/>"
"      <menuitem action='Recover'/>"
"      <separator/>"
"      <menuitem action='Close'/>"
"      <menuitem action='Quit'/>"
"    </menu>"
"    <menu action='EditMenu'>"
"      <menuitem action='Delete'/>"
"      <menuitem action='SelectAll'/>"
"      <menuitem action='Duplicate'/>"
"      <menuitem action='Recalculate'/>"
"      <menuitem action='AutoRecalculate'/>"
"      <menuitem action='AlignColumns'/>"
"      <separator/>"
"      <menuitem action='Find'/>"
"      <menuitem action='FindNext'/>"
"      <menuitem action='NextError'/>"
"      <menu action='JumpToColumnMenu'>"
"        <menuitem action='Stub'/>"	/* Dummy ... replaced on map */
"      </menu>"
"      <separator/>"
"      <menuitem action='Group'/>"
"      <menuitem action='Ungroup'/>"
"      <separator/>"
"      <menuitem action='Preferences'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='Toolbar'/>"
"      <menuitem action='Statusbar'/>"
"      <menuitem action='ToolkitBrowser'/>"
"      <menuitem action='WorkspaceDefs'/>"
"      <separator/>"
#ifdef HAVE_LIBGVC
"      <menuitem action='Graph'/>"
"      <separator/>"
#endif /*HAVE_LIBGVC*/
"      <menuitem action='Normal'/>"
"      <menuitem action='ShowFormula'/>"
"      <menuitem action='NoEdit'/>"
"    </menu>"
"    <menu action='ToolkitsMenu'>"
"      <menuitem action='EditToolkits'/>" 
"      <separator/>"
"      <menuitem action='Stub'/>"	/* Toolkits pasted here at runtime */
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"      <separator/>"
"      <menuitem action='Homepage'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static const char *mainwtab_toolbar_ui_description =
"<ui>"
"  <toolbar name='MainwtabToolbar'>"
"    <toolitem action='Open'/>"
"    <toolitem action='SaveAs'/>"
"    <toolitem action='NewWorkspace'/>"
"    <toolitem action='DuplicateWorkspace'/>"
"    <separator/>"
"    <toolitem action='NewColumn'/>"
"    <toolitem action='Duplicate'/>"
"  </toolbar>"
"</ui>";

static void
mainwtab_lpane_changed_cb( Pane *pane, Mainwtab *mainwtab )
{
	if( mainwtab->ws->lpane_open != pane->open ||
		mainwtab->ws->lpane_position != pane->user_position ) {
		mainwtab->ws->lpane_open = pane->open;
		mainwtab->ws->lpane_position = pane->user_position;

		mainwtab_refresh( mainwtab );
	}
}

static void
mainwtab_rpane_changed_cb( Pane *pane, Mainwtab *mainwtab )
{
	if( mainwtab->ws->rpane_open != pane->open ||
		mainwtab->ws->rpane_position != pane->user_position ) {
		mainwtab->ws->rpane_open = pane->open;
		mainwtab->ws->rpane_position = pane->user_position;

		mainwtab_refresh( mainwtab );
	}
}

static void
mainwtab_watch_changed_cb( Watchgroup *watchgroup, Watch *watch, Mainwtab *mainwtab )
{
	if( strcmp( IOBJECT( watch )->name, "MAINWTAB_TOOLBAR_STYLE" ) == 0 )
		mainwtab->toolbar_visible = MAINWTAB_TOOLBAR;

	mainwtab_refresh( mainwtab );
}

/* Make the insides of the panel.
 */
static void
mainwtab_build( iWindow *iwnd, GtkWidget *vbox )
{
	Mainwtab *mainwtab = MAINWTAB( iwnd );

        GtkWidget *mbar;
	GtkWidget *frame;
	GtkWidget *ebox;
	GError *error;
	GtkWidget *cancel;
	GtkWidget *item;
	Panechild *panechild;

#ifdef DEBUG
	printf( "mainwtab_init: %p\n", mainwtab );
#endif /*DEBUG*/

        /* Make main menu bar
         */
	gtk_action_group_add_actions( iwnd->action_group, 
		mainwtab_actions, G_N_ELEMENTS( mainwtab_actions ), 
		GTK_WINDOW( mainwtab ) );
	gtk_action_group_add_toggle_actions( iwnd->action_group, 
		mainwtab_toggle_actions, G_N_ELEMENTS( mainwtab_toggle_actions ), 
		GTK_WINDOW( mainwtab ) );
	gtk_action_group_add_radio_actions( iwnd->action_group,
		mainwtab_radio_actions, G_N_ELEMENTS( mainwtab_radio_actions ), 
		WORKSPACE_MODE_REGULAR,
		G_CALLBACK( mainwtab_mode_action_cb ),
		GTK_WINDOW( mainwtab ) );

	error = NULL;
	if( !gtk_ui_manager_add_ui_from_string( iwnd->ui_manager,
			mainwtab_menubar_ui_description, -1, &error ) ||
		!gtk_ui_manager_add_ui_from_string( iwnd->ui_manager,
			mainwtab_toolbar_ui_description, -1, &error ) ) {
		g_message( "building menus failed: %s", error->message );
		g_error_free( error );
		exit( EXIT_FAILURE );
	}

	mbar = gtk_ui_manager_get_widget( iwnd->ui_manager, "/MainwtabMenubar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mbar, FALSE, FALSE, 0 );
        gtk_widget_show( mbar );

	/* Get the dummy item on the recent menu, then get the enclosing menu
	 * for that dummy item.
	 */
        item = gtk_ui_manager_get_widget( iwnd->ui_manager,
		"/MainwtabMenubar/FileMenu/RecentMenu/Stub" );
	mainwtab->recent_menu = gtk_widget_get_parent( GTK_WIDGET( item ) );
        gtk_signal_connect( GTK_OBJECT( mainwtab->recent_menu ), "map",
                GTK_SIGNAL_FUNC( mainwtab_recent_map_cb ), mainwtab );

	/* Same for the column jump menu.
	 */
        item = gtk_ui_manager_get_widget( iwnd->ui_manager,
		"/MainwtabMenubar/EditMenu/JumpToColumnMenu/Stub" );
	mainwtab->jump_to_column_menu = 
		gtk_widget_get_parent( GTK_WIDGET( item ) );

	/* Attach toolbar.
  	 */
	mainwtab->toolbar = gtk_ui_manager_get_widget( 
		iwnd->ui_manager, "/MainwtabToolbar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mainwtab->toolbar, FALSE, FALSE, 0 );
        widget_visible( mainwtab->toolbar, MAINWTAB_TOOLBAR );

	/* This will set to NULL if we don't have infobar support.
	 */
	if( (IWINDOW( mainwtab )->infobar = infobar_new()) ) 
		gtk_box_pack_start( GTK_BOX( vbox ), 
			GTK_WIDGET( IWINDOW( mainwtab )->infobar ), 
			FALSE, FALSE, 0 );

	/* hbox for status bar etc.
	 */
        mainwtab->statusbar_main = gtk_hbox_new( FALSE, 2 );
        gtk_box_pack_end( GTK_BOX( vbox ), 
		mainwtab->statusbar_main, FALSE, FALSE, 2 );
        widget_visible( mainwtab->statusbar_main, MAINWTAB_STATUSBAR );

	/* Make space free label.
	 */
	mainwtab->space_free_eb = gtk_event_box_new();
        gtk_box_pack_start( GTK_BOX( mainwtab->statusbar_main ), 
		mainwtab->space_free_eb, FALSE, FALSE, 0 );
	gtk_widget_show( mainwtab->space_free_eb );
	frame = gtk_frame_new( NULL );
	gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_IN );
	gtk_container_add( GTK_CONTAINER( mainwtab->space_free_eb ), frame );
	gtk_widget_show( frame );
	mainwtab->space_free = gtk_label_new( "space_free" );
	gtk_misc_set_padding( GTK_MISC( mainwtab->space_free ), 2, 2 );
        gtk_container_add( GTK_CONTAINER( frame ), mainwtab->space_free );
        gtk_signal_connect( GTK_OBJECT( mainwtab->space_free_eb ), "event",
                GTK_SIGNAL_FUNC( mainwtab_space_free_event ), mainwtab );
	set_tooltip_generate( mainwtab->space_free_eb,
		(TooltipGenerateFn) mainwtab_space_free_tooltip_generate, 
		mainwtab, NULL );
        gtk_widget_show( mainwtab->space_free );
	mainwtab->imageinfo_changed_sid = g_signal_connect( main_imageinfogroup, 
		"changed",
		G_CALLBACK( mainwtab_free_changed_cb ), mainwtab );
	mainwtab->heap_changed_sid = g_signal_connect( reduce_context->heap, 
		"changed",
		G_CALLBACK( mainwtab_free_changed_cb ), mainwtab );

	/* Make message label.
	 */
	mainwtab->statusbar = gtk_label_new( "" );
	gtk_label_set_ellipsize( GTK_LABEL( mainwtab->statusbar ), 
		PANGO_ELLIPSIZE_MIDDLE );
	/* 6 is enough to stop the statusbar changing height when the progress
	 * indicator changes visibility.
	 */
	gtk_misc_set_padding( GTK_MISC( mainwtab->statusbar ), 2, 6 );
	gtk_misc_set_alignment( GTK_MISC( mainwtab->statusbar ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( mainwtab->statusbar_main ), 
		mainwtab->statusbar, TRUE, TRUE, 0 );
        gtk_widget_show( mainwtab->statusbar );

        mainwtab->progress_box = gtk_hbox_new( FALSE, 2 );

	mainwtab->progress = gtk_progress_bar_new();
        gtk_box_pack_end( GTK_BOX( mainwtab->progress_box ), mainwtab->progress, 
		FALSE, TRUE, 0 );
        gtk_widget_show( mainwtab->progress );

        cancel = gtk_button_new_with_label( "Cancel" );
        g_signal_connect( cancel, "clicked",
                G_CALLBACK( mainwtab_cancel_cb ), mainwtab );
        gtk_box_pack_end( GTK_BOX( mainwtab->progress_box ), cancel, 
		FALSE, TRUE, 0 );
        gtk_widget_show( cancel );

        gtk_box_pack_end( GTK_BOX( mainwtab->statusbar_main ), 
		mainwtab->progress_box, FALSE, TRUE, 0 );

	/* Make toolkit/workspace displays.
	 */
	mainwtab->kitgview = TOOLKITGROUPVIEW( 
		model_view_new( MODEL( mainwtab->ws->kitg ), NULL ) );
	g_object_ref( G_OBJECT( mainwtab->kitgview ) );
	gtk_object_sink( GTK_OBJECT( mainwtab->kitgview ) );
	toolkitgroupview_set_mainwtab( mainwtab->kitgview, mainwtab );
	gtk_menu_set_accel_group( GTK_MENU( mainwtab->kitgview->menu ),
		iwnd->accel_group );

	mainwtab->rpane = pane_new( PANE_HIDE_RIGHT );
	g_signal_connect( mainwtab->rpane, "changed",
		G_CALLBACK( mainwtab_rpane_changed_cb ), mainwtab );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( mainwtab->rpane ), TRUE, TRUE, 0 );
	gtk_widget_show( GTK_WIDGET( mainwtab->rpane ) );

	mainwtab->lpane = pane_new( PANE_HIDE_LEFT );
	g_signal_connect( mainwtab->lpane, "changed",
		G_CALLBACK( mainwtab_lpane_changed_cb ), mainwtab );
	gtk_paned_pack1( GTK_PANED( mainwtab->rpane ), GTK_WIDGET( mainwtab->lpane ),
		TRUE, FALSE );
	gtk_widget_show( GTK_WIDGET( mainwtab->lpane ) );

	mainwtab->wsview = WORKSPACEVIEW( 
		model_view_new( MODEL( mainwtab->ws ), NULL ) );
	gtk_paned_pack2( GTK_PANED( mainwtab->lpane ), GTK_WIDGET( mainwtab->wsview ),
		TRUE, FALSE );
	gtk_widget_show( GTK_WIDGET( mainwtab->wsview ) );

	mainwtab->popup = popup_build( _( "Workspace menu" ) );
	popup_add_but( mainwtab->popup, _( "New C_olumn" ),
		POPUP_FUNC( mainwtab_column_new_action_cb2 ) ); 
	mainwtab->popup_jump =
		popup_add_pullright( mainwtab->popup, _( "Jump to _Column" ) ); 
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

	/* Toolkit Browser pane.
	 */
	panechild = panechild_new( mainwtab->rpane, _( "Toolkit Browser" ) );

	/* Have to put toolkitbrowser in an ebox so the search entry gets
	 * clipped to the pane size.
	 */
	ebox = gtk_event_box_new();
	gtk_container_add( GTK_CONTAINER( panechild ), GTK_WIDGET( ebox ) );
	gtk_widget_show( ebox );

	mainwtab->toolkitbrowser = toolkitbrowser_new();
	vobject_link( VOBJECT( mainwtab->toolkitbrowser ), 
		IOBJECT( mainwtab->ws->kitg ) );
	toolkitbrowser_set_mainwtab( mainwtab->toolkitbrowser, mainwtab );
	gtk_container_add( GTK_CONTAINER( ebox ), 
		GTK_WIDGET( mainwtab->toolkitbrowser ) );
	gtk_widget_show( GTK_WIDGET( mainwtab->toolkitbrowser ) );

	/* Workspace-local defs pane.
	 */
	panechild = panechild_new( mainwtab->lpane, _( "Workspace Definitions" ) );

	mainwtab->workspacedefs = workspacedefs_new( mainwtab );
	vobject_link( VOBJECT( mainwtab->workspacedefs ), IOBJECT( mainwtab->ws ) );
	gtk_container_add( GTK_CONTAINER( panechild ), 
		GTK_WIDGET( mainwtab->workspacedefs ) );
	gtk_widget_show( GTK_WIDGET( mainwtab->workspacedefs ) );

	/* Set start state.
	 */
	(void) mainwtab_refresh( mainwtab );

	/* Any changes to prefs, refresh (yuk!).
	 */
	mainwtab->watch_changed_sid = g_signal_connect( main_watchgroup, 
		"watch_changed",
		G_CALLBACK( mainwtab_watch_changed_cb ), mainwtab );
}

static void
mainwtab_popdown( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys )
{
	Mainwtab *mainwtab = MAINWTAB( iwnd );

	/* We can be destroyed in two ways: either our iwnd tells us to go, or
	 * our model is destroyed under us. If the model has gone, we just go.
	 * If the model is still there, we need to ask about saving and
	 * quitting.
	 */
	if( mainwtab->ws ) {
		/* Argh, cancel this immediately, in case we close before
		 * we're up properly.
		 */
		IM_FREEF( g_source_remove, mainwtab->compat_timeout );

		filemodel_inter_savenclose_cb( IWINDOW( mainwtab ), 
			FILEMODEL( mainwtab->ws ), nfn, sys );
	}
	else
		nfn( sys, IWINDOW_YES );
}

static void
mainwtab_link( Mainwtab *mainwtab, Workspace *ws )
{
	g_assert( !mainwtab->ws );

	mainwtab->ws = ws;
	ws->iwnd = IWINDOW( mainwtab );
	mainwtab->destroy_sid = g_signal_connect( ws, "destroy",
		G_CALLBACK( mainwtab_workspace_destroy_cb ), mainwtab );
	mainwtab->changed_sid = g_signal_connect( ws, "changed",
		G_CALLBACK( mainwtab_workspace_changed_cb ), mainwtab );
	iwindow_set_build( IWINDOW( mainwtab ), 
		(iWindowBuildFn) mainwtab_build, ws, NULL, NULL );
	iwindow_set_popdown( IWINDOW( mainwtab ), mainwtab_popdown, NULL );
	filemodel_set_window_hint( FILEMODEL( ws ), IWINDOW( mainwtab ) );

	/* If we have a saved size for this workspace, set that. Otherwise,
	 * default to the default.
	 */
	if( !ws->window_width ) {
		iwindow_set_size_prefs( IWINDOW( mainwtab ), 
			"MAINWTAB_WINDOW_WIDTH", "MAINWTAB_WINDOW_HEIGHT" );
	}

	iwindow_build( IWINDOW( mainwtab ) );

	if( ws->window_width ) {
		GdkScreen *screen = 
			gtk_widget_get_screen( GTK_WIDGET( mainwtab ) );

		gtk_window_set_default_size( GTK_WINDOW( mainwtab ), 
			IM_MIN( ws->window_width, 
				gdk_screen_get_width( screen ) ),
			IM_MIN( ws->window_height, 
				gdk_screen_get_height( screen ) ) );
	}

	pane_set_state( mainwtab->lpane, ws->lpane_open, ws->lpane_position );
	pane_set_state( mainwtab->rpane, ws->rpane_open, ws->rpane_position );
}

Mainwtab *
mainwtab_new( Workspace *ws )
{
	Mainwtab *mainwtab;

	symbol_recalculate_all_force( FALSE );

	mainwtab = MAINWTAB( g_object_new( TYPE_MAINWTAB, NULL ) );
	mainwtab_link( mainwtab, ws );

	return( mainwtab );
}
