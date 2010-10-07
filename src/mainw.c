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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#include "ip.h"

/* 
#define DEBUG
 */

/* Load and save recent items here.
 */
#define RECENT_WORKSPACE "recent_workspace"
#define RECENT_IMAGE "recent_image"
#define RECENT_MATRIX "recent_matrix"

/* Recently loaded/saved workspaces, images and matricies.
 */
GSList *mainw_recent_workspace = NULL;
GSList *mainw_recent_image = NULL;
GSList *mainw_recent_matrix = NULL;

/* Auto-recalc state. Don't do this as a preference, since preferences are
 * workspaces and need to have recalc working to operate.
 */
gboolean mainw_auto_recalc = TRUE;

static iWindowClass *parent_class = NULL;

/* All the mainw.
 */
static GSList *mainw_all = NULL;

void
mainw_startup( void )
{
	IM_FREEF( recent_free, mainw_recent_workspace );
	IM_FREEF( recent_free, mainw_recent_image );
	IM_FREEF( recent_free, mainw_recent_matrix );

	mainw_recent_workspace = recent_load( RECENT_WORKSPACE );
	mainw_recent_image = recent_load( RECENT_IMAGE );
	mainw_recent_matrix = recent_load( RECENT_MATRIX );
}

void
mainw_shutdown( void )
{
	recent_save( mainw_recent_workspace, RECENT_WORKSPACE );
	recent_save( mainw_recent_image, RECENT_IMAGE );
	recent_save( mainw_recent_matrix, RECENT_MATRIX );

	IM_FREEF( recent_free, mainw_recent_workspace );
	IM_FREEF( recent_free, mainw_recent_image );
	IM_FREEF( recent_free, mainw_recent_matrix );
}

static int mainw_recent_freeze_count = 0;

void
mainw_recent_freeze( void )
{
	mainw_recent_freeze_count += 1;
}

void
mainw_recent_thaw( void )
{
	g_assert( mainw_recent_freeze_count > 0 );

	mainw_recent_freeze_count -= 1;
}

void
mainw_recent_add( GSList **recent, const char *filename )
{
	if( !mainw_recent_freeze_count ) {
		char buf[FILENAME_MAX];

		expand_variables( PATH_TMP, buf );
		if( filename && strcmp( filename, "" ) != 0 &&
			!is_prefix( buf, filename ) )
			*recent = recent_add( *recent, filename );
	}
}

int
mainw_number( void )
{
	return( g_slist_length( mainw_all ) );
}

/* Pick a mainw at random. Used if we need a window for a dialog, and we're
 * not sure which to pick.
 */
Mainw *
mainw_pick_one( void )
{
	if( !mainw_all )
		return( NULL );

	return( MAINW( mainw_all->data ) );
}

static void
mainw_destroy( GtkObject *object )
{
	Mainw *mainw;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_MAINW( object ) );

	mainw = MAINW( object );

#ifdef DEBUG
	printf( "mainw_destroy\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	if( mainw->ws ) {
		mainw->ws->iwnd = NULL;

		filemodel_set_window_hint( FILEMODEL( mainw->ws ), NULL );
		FREESID( mainw->destroy_sid, mainw->ws );
		FREESID( mainw->changed_sid, mainw->ws );
	}

	FREESID( mainw->imageinfo_changed_sid, main_imageinfogroup );
	FREESID( mainw->heap_changed_sid, reduce_context->heap );
	FREESID( mainw->watch_changed_sid, main_watchgroup );
	FREESID( mainw->begin_sid, progress_get() );
	FREESID( mainw->update_sid, progress_get() );
	FREESID( mainw->end_sid, progress_get() );
	DESTROY_GTK( mainw->popup );
	UNREF( mainw->kitgview );

	mainw_all = g_slist_remove( mainw_all, mainw );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static gboolean
mainw_compat_timeout( Mainw *mainw )
{
	mainw->compat_timeout = 0;

	error_top( _( "Compatibility mode." ) );
	error_sub( _( "This workspace was created by version %d.%d.%d. "
		"A set of compatibility menus have been loaded "
		"for this window." ),
		FILEMODEL( mainw->ws )->major,
		FILEMODEL( mainw->ws )->minor,
		FILEMODEL( mainw->ws )->micro );
	iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_INFO );

	return( FALSE );
}

static void
mainw_map( GtkWidget *widget )
{
	Mainw *mainw = MAINW( widget );

	g_assert( !mainw->compat_timeout );

	/* Popping up the info dialog immediately gives us stacking problems
	 * :(
	 */
	if( mainw->ws->compat_major )
		mainw->compat_timeout = g_timeout_add( 500, 
			(GSourceFunc) mainw_compat_timeout, mainw );

	GTK_WIDGET_CLASS( parent_class )->map( widget );
}

static gboolean
mainw_configure_event( GtkWidget *widget, GdkEventConfigure *event )
{
	Mainw *mainw = MAINW( widget );

	mainw->ws->window_width = event->width;
	mainw->ws->window_height = event->height;

	return( GTK_WIDGET_CLASS( parent_class )->
		configure_event( widget, event ) );
}

static void
mainw_class_init( MainwClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = mainw_destroy;

	widget_class->map = mainw_map;
	widget_class->configure_event = mainw_configure_event;
}

static void
mainw_progress_begin( Progress *progress, Mainw *mainw )
{
	mainw->cancel = FALSE;
        gtk_widget_show( mainw->progress_box );
}

static void
mainw_progress_update( Progress *progress, gboolean *cancel, Mainw *mainw )
{
	gtk_progress_bar_set_text( GTK_PROGRESS_BAR( mainw->progress ), 
		vips_buf_all( &progress->feedback ) );
	gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( mainw->progress ), 
		IM_CLIP( 0.0, (double) progress->percent / 100.0, 1.0 ) );

	if( mainw->cancel )
		*cancel = TRUE;
} 

static void
mainw_progress_end( Progress *progress, Mainw *mainw )
{
        gtk_widget_hide( mainw->progress_box );
	mainw->cancel = FALSE;
}

static void
mainw_init( Mainw *mainw )
{
	mainw->ws = NULL;
	mainw->destroy_sid = 0;
	mainw->changed_sid = 0;
	mainw->imageinfo_changed_sid = 0;
	mainw->heap_changed_sid = 0;
	mainw->watch_changed_sid = 0;

	mainw->begin_sid = g_signal_connect( progress_get(), "begin", 
		G_CALLBACK( mainw_progress_begin ), mainw );
	mainw->update_sid = g_signal_connect( progress_get(), "update", 
		G_CALLBACK( mainw_progress_update ), mainw );
	mainw->end_sid = g_signal_connect( progress_get(), "end", 
		G_CALLBACK( mainw_progress_end ), mainw );
	mainw->cancel = FALSE;

	mainw->free_type = FALSE;

	mainw->toolbar_visible = MAINW_TOOLBAR;
	mainw->statusbar_visible = MAINW_STATUSBAR;

	mainw->row_last_error = NULL;

	mainw->compat_timeout = 0;

	mainw->kitgview = NULL;
	mainw->toolkitbrowser = NULL;
	mainw->wsview = NULL;
	mainw->toolbar = NULL;

	mainw->statusbar_main = NULL;
	mainw->statusbar = NULL;
	mainw->space_free = NULL;
	mainw->space_free_eb = NULL;
	mainw->progress_box = NULL;
	mainw->progress = NULL;

	mainw->lpane = NULL;
	mainw->rpane = NULL;

	mainw_all = g_slist_prepend( mainw_all, mainw );
}

GType
mainw_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( MainwClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) mainw_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Mainw ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) mainw_init,
		};

		type = g_type_register_static( TYPE_IWINDOW, 
			"Mainw", &info, 0 );
	}

	return( type );
}

static void
mainw_cancel_cb( GtkWidget *wid, Mainw *mainw )
{
	mainw->cancel = TRUE;
}

static void
mainw_workspace_destroy_cb( Workspace *ws, Mainw *mainw )
{
#ifdef DEBUG
	printf( "mainw_workspace_destroy_cb: %p\n", mainw );
#endif /*DEBUG*/

	mainw->destroy_sid = 0;
	mainw->changed_sid = 0;
	mainw->ws = NULL;

	iwindow_kill( IWINDOW( mainw ) );
}

static void
mainw_find_disc( VipsBuf *buf )
{
	double sz = find_space( PATH_TMP );

	if( sz < 0 )
		vips_buf_appendf( buf, _( "No temp area" ) );
	else {
		char txt[MAX_STRSIZE];
		VipsBuf buf2 = VIPS_BUF_STATIC( txt );

		to_size( &buf2, sz );
		vips_buf_appendf( buf, _( "%s free" ), vips_buf_all( &buf2 ) );
	}
}

static void
mainw_find_heap( VipsBuf *buf, Heap *heap )
{
	/* How much we can still expand the heap by ... this 
	 * can be -ve if we've closed a workspace, or changed 
	 * the upper limit.
	 */
	int togo = IM_MAX( 0, (heap->mxb - heap->nb) * heap->rsz );

	vips_buf_appendf( buf, _( "%d cells free" ), heap->nfree + togo );
}

/* Update the space remaining indicator. 
 */
static void
mainw_free_update( Mainw *mainw )
{
	Heap *heap = reduce_context->heap;
	char txt[80];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	if( workspace_selected_any( mainw->ws ) ) {
		/* Display select message instead.
		 */
		vips_buf_appends( &buf, _( "Selected:" ) );
		vips_buf_appends( &buf, " " );
		workspace_selected_names( mainw->ws, &buf, ", " );
	}
	else {
		/* Out of space? Make sure we swap to cell display.
		 */
		if( !heap->free )
			mainw->free_type = FALSE;

		if( mainw->free_type ) 
			mainw_find_heap( &buf, heap );
		else
			mainw_find_disc( &buf );
	}

	set_glabel( mainw->space_free, "%s", vips_buf_all( &buf ) );
}

static void
mainw_title_update( Mainw *mainw )
{
	char txt[512];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	if( FILEMODEL( mainw->ws )->modified ) 
		vips_buf_appendf( &buf, "*" ); 
	vips_buf_appendf( &buf, "%s", NN( IOBJECT( mainw->ws->sym )->name ) );
	if( mainw->ws->compat_major ) {
		vips_buf_appends( &buf, " - " );
		vips_buf_appends( &buf, _( "compatibility mode" ) );
		vips_buf_appendf( &buf, " %d.%d", 
			mainw->ws->compat_major, 
			mainw->ws->compat_minor ); 
	}
	if( FILEMODEL( mainw->ws )->filename )
		vips_buf_appendf( &buf, " - %s", 
			FILEMODEL( mainw->ws )->filename );
	else {
		vips_buf_appends( &buf, " - " );
		vips_buf_appends( &buf, _( "unsaved workspace" ) );
	}

	iwindow_set_title( IWINDOW( mainw ), "%s", vips_buf_all( &buf ) );
}

static void 
mainw_status_update( Mainw *mainw )
{
	if( mainw->ws->status ) {
		gtk_label_set_text( GTK_LABEL( mainw->statusbar ), 
			mainw->ws->status ); 
	}
	else {
		char txt[256];

		im_snprintf( txt, 256, _( NIP_COPYRIGHT ), PACKAGE );
		gtk_label_set_markup( GTK_LABEL( mainw->statusbar ), txt );
	}
}

static gint
mainw_jump_name_compare( iContainer *a, iContainer *b )
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
mainw_jump_column_cb( GtkWidget *item, Column *column )
{
	model_scrollto( MODEL( column ), MODEL_SCROLL_TOP );
}

static void *
mainw_jump_build( Column *column, GtkWidget *menu )
{
	GtkWidget *item;
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	vips_buf_appendf( &buf, "%s - %s", 
		IOBJECT( column )->name, IOBJECT( column )->caption );
	item = gtk_menu_item_new_with_label( vips_buf_all( &buf ) );
	g_signal_connect( item, "activate",
		G_CALLBACK( mainw_jump_column_cb ), column );
	gtk_menu_append( GTK_MENU( menu ), item );
	gtk_widget_show( item );

	return( NULL );
}

static void
mainw_jump_update( Mainw *mainw, GtkWidget *menu )
{
	GtkWidget *item;
	GSList *columns;

	gtk_container_foreach( GTK_CONTAINER( menu ),
		(GtkCallback) gtk_widget_destroy, NULL );

	item = gtk_tearoff_menu_item_new();
	gtk_menu_append( GTK_MENU( menu ), item );
	gtk_widget_show( item );

	columns = icontainer_get_children( ICONTAINER( mainw->ws ) );
        columns = g_slist_sort( columns, 
		(GCompareFunc) mainw_jump_name_compare );
	slist_map( columns, (SListMapFn) mainw_jump_build, menu );

	g_slist_free( columns );
}

static void
mainw_refresh( Mainw *mainw )
{
	static GtkToolbarStyle styles[] = {
		99,			/* Overwrite with system default */
		GTK_TOOLBAR_ICONS,
		GTK_TOOLBAR_TEXT,
		GTK_TOOLBAR_BOTH,
		GTK_TOOLBAR_BOTH_HORIZ
	};

	/* Keep in step with the WorkspaceMode enum.
	 */
	const static char *view_mode[] = {
		"Normal",
		"ShowFormula",
		"NoEdit"
	};

	iWindow *iwnd = IWINDOW( mainw );
	Workspace *ws = mainw->ws;
	int pref = IM_CLIP( 0, MAINW_TOOLBAR_STYLE, IM_NUMBER( styles ) - 1 );

        GtkAction *action;

#ifdef DEBUG
	printf( "mainw_refresh: %p\n", mainw );
#endif /*DEBUG*/

	mainw_status_update( mainw );
	mainw_free_update( mainw );
	mainw_title_update( mainw );

	if( styles[0] == 99 )
		styles[0] = 
			gtk_toolbar_get_style( GTK_TOOLBAR( mainw->toolbar ) );

	gtk_toolbar_set_style( GTK_TOOLBAR( mainw->toolbar ), styles[pref] );

	action = gtk_action_group_get_action( iwnd->action_group, 
		"AutoRecalculate" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		mainw_auto_recalc );

	action = gtk_action_group_get_action( iwnd->action_group, 
		"Toolbar" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		mainw->toolbar_visible );
        widget_visible( mainw->toolbar, mainw->toolbar_visible );

	action = gtk_action_group_get_action( iwnd->action_group, 
		"Statusbar" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		mainw->statusbar_visible );
        widget_visible( mainw->statusbar_main, mainw->statusbar_visible );

	action = gtk_action_group_get_action( iwnd->action_group, 
		"WorkspaceDefs" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		mainw->lpane->open );

	action = gtk_action_group_get_action( iwnd->action_group, 
		"ToolkitBrowser" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		mainw->rpane->open );

	action = gtk_action_group_get_action( iwnd->action_group, 
		view_mode[ws->mode] );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		TRUE );

	mainw_jump_update( mainw, mainw->jump_to_column_menu );
	mainw_jump_update( mainw, mainw->popup_jump );
}

static void
mainw_workspace_changed_cb( Workspace *ws, Mainw *mainw )
{
	mainw_refresh( mainw );
}

/* Event in the "space free" display ... toggle mode on left click.
 */
static gint
mainw_space_free_event( GtkWidget *widget, GdkEvent *ev, Mainw *mainw )
{
	if( ev->type == GDK_BUTTON_RELEASE ) {
		mainw->free_type = !mainw->free_type;
		mainw_free_update( mainw );
	}

	return( FALSE );
}

static void
mainw_space_free_tooltip_generate( GtkWidget *widget, VipsBuf *buf, 
	Mainw *mainw )
{
	Heap *heap = reduce_context->heap;
	Symbol *sym = mainw->ws->sym;

	mainw_find_disc( buf );
	/* Expands to (eg.) "14GB free in /pics/tmp" */
        vips_buf_appendf( buf, _( " in \"%s\"" ), PATH_TMP );
        vips_buf_appends( buf, ", " );

        vips_buf_appendf( buf, 
		_( "%d cells in heap, %d cells free, %d cells maximum" ),
                heap->ncells, heap->nfree, heap->max_fn( heap ) );
        vips_buf_appends( buf, ", " );

        vips_buf_appendf( buf, _( "%d objects in workspace" ),
		g_slist_length( ICONTAINER( sym->expr->compile )->children ) );
        vips_buf_appends( buf, ", " );

        vips_buf_appendf( buf, _( "%d vips calls cached" ), vips_history_size );
        vips_buf_appends( buf, ", " );

        vips_buf_appendf( buf, _( "using %d threads" ), im_concurrency_get() );
}

static void
mainw_free_changed_cb( gpointer *dummy, Mainw *mainw )
{
	mainw_free_update( mainw );
	mainw_status_update( mainw );
}

/* Go to home page.
 */
void
mainw_homepage_action_cb( GtkAction *action, iWindow *iwnd )
{
	box_url( GTK_WIDGET( iwnd ), VIPS_HOMEPAGE );
}

/* About... box.
 */
void
mainw_about_action_cb( GtkAction *action, iWindow *iwnd )
{
	box_about( GTK_WIDGET( iwnd ) );
}

/* User's guide.
 */
void
mainw_guide_action_cb( GtkAction *action, iWindow *iwnd )
{
	box_url( GTK_WIDGET( iwnd ), "file://" NIP_DOCPATH "/nipguide.html" );
}

static void
mainw_clone( Mainw *mainw )
{
	Workspace *ws = mainw->ws;

	if( !workspace_selected_any( ws ) ) {
		/* Nothing selected -- select bottom object.
		 */
		Row *row = workspace_get_bottom( ws );

		if( !row ) {
			iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
			return;
		}
		row_select( row );
	}

	/* Clone selected symbols.
	 */
	progress_begin();
	if( !workspace_clone_selected( ws ) ) { 
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
		progress_end();
		return;
	}
	symbol_recalculate_all();
	workspace_deselect_all( ws );
	progress_end();

	model_scrollto( MODEL( ws->current ), MODEL_SCROLL_TOP );
}

static void
mainw_duplicate_action_cb( GtkAction *action, Mainw *mainw )
{
	mainw_clone( mainw );
}

/* Ungroup the selected object(s), or the bottom object.
 */
static void
mainw_ungroup_action_cb( GtkAction *action, Mainw *mainw )
{
	progress_begin();
	if( !workspace_selected_ungroup( mainw->ws ) )
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
	progress_end();
}

/* Group the selected object(s).
 */
static void
mainw_group_action_cb( GtkAction *action, Mainw *mainw )
{
	Workspace *ws = mainw->ws;
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	if( !workspace_selected_any( ws ) ) {
		/* Nothing selected -- select bottom object.
		 */
		Row *row = workspace_get_bottom( ws );

		if( !row ) {
			iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
			return;
		}
		row_select( row );
	}

	vips_buf_appends( &buf, "Group [" );
	workspace_selected_names( ws, &buf, "," );
	vips_buf_appends( &buf, "]" );
	if( !workspace_add_def( ws, vips_buf_all( &buf ) ) ) {
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
		return;
	}
	workspace_deselect_all( ws );
}

static void
mainw_group_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_group_action_cb( NULL, mainw );
}

/* Select all objects.
 */
static void
mainw_select_all_action_cb( GtkAction *action, Mainw *mainw )
{
	workspace_select_all( mainw->ws );
}

static void
mainw_find_action_cb( GtkAction *action, Mainw *mainw )
{
	error_top( _( "Not implemented." ) );
	error_sub( _( "Find in workspace not implemented yet." ) );
	iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_INFO );
}

static void
mainw_find_again_action_cb( GtkAction *action, Mainw *mainw )
{
	error_top( _( "Not implemented." ) );
	error_sub( _( "Find again in workspace not implemented yet." ) );
	iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_INFO );
}

static Row *
mainw_test_error( Row *row, Mainw *mainw, int *found )
{
	g_assert( row->err );

	/* Found next?
	 */
	if( *found )
		return( row );

	if( row == mainw->row_last_error ) {
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
mainw_next_error_action_cb( GtkAction *action, Mainw *mainw )
{
	char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	int found;

	if( !mainw->ws->errors ) {
		error_top( _( "No errors." ) );
		error_sub( "%s", _( "There are no errors (that I can see) "
			"in this workspace." ) );
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_INFO );
		return;
	}

	/* Search for the one after the last one.
	 */
	found = 0;
	mainw->row_last_error = (Row *) slist_map2( mainw->ws->errors, 
		(SListMap2Fn) mainw_test_error, mainw, &found );

	/* NULL? We've hit end of table, start again.
	 */
	if( !mainw->row_last_error ) {
		found = 1;
		mainw->row_last_error = (Row *) slist_map2( mainw->ws->errors, 
			(SListMap2Fn) mainw_test_error, mainw, &found );
	}

	/* *must* have one now.
	 */
	g_assert( mainw->row_last_error && mainw->row_last_error->err );

	model_scrollto( MODEL( mainw->row_last_error ), MODEL_SCROLL_TOP );

	row_qualified_name( mainw->row_last_error->expr->row, &buf );
	vips_buf_appends( &buf, ": " );
	vips_buf_appends( &buf, mainw->row_last_error->expr->error_top );
	workspace_set_status( mainw->ws, "%s", vips_buf_firstline( &buf ) );
}

static void
mainw_next_error_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_next_error_action_cb( NULL, mainw );
}

/* Callback for box_yesno in mainw_force_calc_cb. Recalc selected items.
 */
static void
mainw_selected_recalc_dia( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Mainw *mainw = MAINW( client );

	if( workspace_selected_recalc( mainw->ws ) )
		nfn( sys, IWINDOW_YES );
	else
		nfn( sys, IWINDOW_ERROR );
}

/* If symbols are selected, make them very dirty and recalculate. If not, 
 * just recalculate symbols which are already dirty.
 */
static void
mainw_force_calc_action_cb( GtkAction *action, Mainw *mainw )
{
	Workspace *ws = mainw->ws;

        if( workspace_selected_any( ws ) ) {
		char txt[30];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		workspace_selected_names( ws, &buf, ", " );

		box_yesno( GTK_WIDGET( mainw ), 
			mainw_selected_recalc_dia, iwindow_true_cb, mainw, 
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
mainw_workspace_save_action_cb( GtkAction *action, Mainw *mainw )
{
	filemodel_inter_save( IWINDOW( mainw ), FILEMODEL( mainw->ws ) );
}

/* Callback for "save as .." workspace.
 */
static void
mainw_workspace_save_as_action_cb( GtkAction *action, Mainw *mainw )
{
	filemodel_inter_saveas( IWINDOW( mainw ), FILEMODEL( mainw->ws ) );
}

static Workspace *
mainw_open_workspace( Workspacegroup *wsg, const char *filename )
{
	Workspace *ws;
	Mainw *mainw;

	if( !(ws = workspace_new_from_file( wsg, filename )) ) 
		return( NULL );

	mainw = mainw_new( ws );
	gtk_widget_show( GTK_WIDGET( mainw ) );
	mainw_recent_add( &mainw_recent_workspace, filename );

	/* Process some events to make sure we rethink the layout and
	 * are able to get the append-at-RHS offsets.
	 */
	while( g_main_context_iteration( NULL, FALSE ) )
		;

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
mainw_open_file_into_workspace( Mainw *mainw, const char *filename )
{
	Workspacegroup *wsg = WORKSPACEGROUP( ICONTAINER( mainw->ws )->parent );
	Workspace *new_ws;

	if( !(new_ws = mainw_open_workspace( wsg, filename )) ) 
		return( NULL );

	if( workspace_is_empty( mainw->ws ) ) {
		/* Make sure modified isn't set ... otherwise we'll get a
		 * "save before close" dialog.
		 */
		filemodel_set_modified( FILEMODEL( mainw->ws ), FALSE );
		iwindow_kill( IWINDOW( mainw ) );
	}

	return( new_ws );
}

/* Track these during a load.
 */
typedef struct {
	Workspacegroup *wsg;
	VipsBuf *buf;
	int nitems;
} MainwLoad;

/* Try to open a file. Workspace files we load immediately, other ones we
 * add the load to a buffer.
 */
static void *
mainw_open_fn( Filesel *filesel, const char *filename, MainwLoad *load )
{
	if( is_file_type( &filesel_wfile_type, filename ) ) {
		if( !mainw_open_workspace( load->wsg, filename ) )
			return( filesel );
	}
	else {
		if( load->nitems ) 
			vips_buf_appends( load->buf, ", " );
		if( !workspace_load_file_buf( load->buf, filename ) )
			return( filesel );
		mainw_recent_add( &mainw_recent_image, filename );
		load->nitems += 1;
	}

	return( NULL );
}

/* Callback from load browser.
 */
static void
mainw_open_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Mainw *mainw = MAINW( client );
	Filesel *filesel = FILESEL( iwnd );
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );
	MainwLoad load;

	load.wsg = WORKSPACEGROUP( ICONTAINER( mainw->ws )->parent );
	load.buf = &buf;
	load.nitems = 0;

	if( filesel_map_filename_multi( filesel,
		(FileselMapFn) mainw_open_fn, &load, NULL ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	/* If we have just loaded one or more workspaces, and the current
	 * workspace is smpty, we can junk it.
	 */
	if( !load.nitems && 
		workspace_is_empty( mainw->ws ) ) {
		/* Make sure modified isn't set ... otherwise we'll get a
		 * "save before close" dialog.
		 */
		filemodel_set_modified( FILEMODEL( mainw->ws ), FALSE );
		iwindow_kill( IWINDOW( mainw ) );
	}
	else {
		char txt2[MAX_STRSIZE];
		VipsBuf buf2 = VIPS_BUF_STATIC( txt2 );

		if( load.nitems > 1 )
			vips_buf_appendf( &buf2, "Group [%s]", 
				vips_buf_all( &buf ) );
		else
			vips_buf_appends( &buf2, vips_buf_all( &buf ) );

		/* Some actual files (image, matrix) were selected. Load into
		 * the current workspace.
		 */
		if( !workspace_add_def( mainw->ws, vips_buf_all( &buf2 ) ) ) {
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
mainw_open( Mainw *mainw )
{
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), _( "Open File" ) );
	filesel_set_flags( FILESEL( filesel ), TRUE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), 
		filesel_type_mainw, IMAGE_FILE_TYPE );
	filesel_set_filetype_pref( FILESEL( filesel ), "IMAGE_FILE_TYPE" );
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( mainw ) );
	filesel_set_done( FILESEL( filesel ), 
		mainw_open_done_cb, mainw );
	filesel_set_multi( FILESEL( filesel ), TRUE );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
mainw_open_action_cb( GtkAction *action, Mainw *mainw )
{
	mainw_open( mainw );
}

static void
mainw_open_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_open_action_cb( NULL, mainw );
}

/* Open one of the example workspaces ... just a shortcut.
 */
static void
mainw_open_examples( Mainw *mainw )
{
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), _( "Open File" ) );
	filesel_set_flags( FILESEL( filesel ), TRUE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), 
		filesel_type_workspace, 0 );
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( mainw ) );
	filesel_set_done( FILESEL( filesel ), 
		mainw_open_done_cb, mainw );
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
mainw_open_examples_action_cb( GtkAction *action, Mainw *mainw )
{
	mainw_open_examples( mainw );
}

static gboolean
mainw_recent_open( Mainw *mainw, const char *filename )
{
	if( is_file_type( &filesel_wfile_type, filename ) ) {
		if( !mainw_open_file_into_workspace( mainw, filename ) )
			return( FALSE );
	}
	else {
		if( !workspace_load_file( mainw->ws, filename ) )
			return( FALSE );
	}

	return( TRUE );
}

static void
mainw_recent_open_cb( GtkWidget *widget, const char *filename )
{
	Mainw *mainw = MAINW( iwindow_get_root( widget ) );

	progress_begin();
	if( !mainw_recent_open( mainw, filename ) ) {
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
		progress_end();
		return;
	}
	progress_end();

	symbol_recalculate_all();
}

static void
mainw_recent_build( GtkWidget *menu, GSList *recent )
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
			GTK_SIGNAL_FUNC( mainw_recent_open_cb ), 
			(char *) filename );
	}
}

static void
mainw_recent_clear_cb( GtkWidget *widget, const char *filename )
{
	IM_FREEF( recent_free, mainw_recent_workspace );
	IM_FREEF( recent_free, mainw_recent_image );
	IM_FREEF( recent_free, mainw_recent_matrix );

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
mainw_recent_map_cb( GtkWidget *widget, Mainw *mainw )
{
	GtkWidget *menu = mainw->recent_menu;
	GtkWidget *item;

	gtk_container_foreach( GTK_CONTAINER( menu ),
		(GtkCallback) gtk_widget_destroy, NULL );

	if( mainw_recent_image ) {
		item = gtk_menu_item_new_with_label( _( "Recent Images" ) );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_widget_set_sensitive( item, FALSE );
		mainw_recent_build( menu, mainw_recent_image );
	}

	if( mainw_recent_workspace ) {
		item = gtk_menu_item_new_with_label( _( "Recent Workspaces" ) );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_widget_set_sensitive( item, FALSE );
		mainw_recent_build( menu, mainw_recent_workspace );
	}

	if( mainw_recent_matrix ) {
		item = gtk_menu_item_new_with_label( _( "Recent Matricies" ) );
		gtk_menu_append( GTK_MENU( menu ), item );
		gtk_widget_show( item );
		gtk_widget_set_sensitive( item, FALSE );
		mainw_recent_build( menu, mainw_recent_matrix );
	}

	if( !mainw_recent_workspace && !mainw_recent_image &&
		!mainw_recent_matrix ) {
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
			GTK_SIGNAL_FUNC( mainw_recent_clear_cb ), NULL );
	}
}

/* Load a workspace at the top level.
 */
static void *
mainw_workspace_merge_fn( Filesel *filesel,
	const char *filename, void *a, void *b )
{
	Mainw *mainw = MAINW( a );

	if( !workspace_merge_file( mainw->ws, filename ) )
		return( filesel );

	/* Process some events to make sure we rethink the layout and
	 * are able to get the append-at-RHS offsets.
	 */
	while( g_main_context_iteration( NULL, FALSE ) )
		;

	symbol_recalculate_all();
	mainw_recent_add( &mainw_recent_workspace, filename );

	return( NULL );
}

/* Callback from load browser.
 */
static void
mainw_workspace_merge_done_cb( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Mainw *mainw = MAINW( client );

	if( filesel_map_filename_multi( filesel,
		mainw_workspace_merge_fn, mainw, NULL ) ) {
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
mainw_workspace_merge( Mainw *mainw )
{
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), 
		_( "Merge Workspace from File" ) );
	filesel_set_flags( FILESEL( filesel ), FALSE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_workspace, 0 ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( mainw ) );
	filesel_set_done( FILESEL( filesel ), 
		mainw_workspace_merge_done_cb, mainw );
	filesel_set_multi( FILESEL( filesel ), TRUE );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
mainw_workspace_duplicate_action_cb( GtkAction *action, Mainw *mainw )
{
	Workspace *new_ws;
	Mainw *new_mainw;

        progress_begin();

	if( !(new_ws = workspace_clone( mainw->ws )) ) {
		progress_end();
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
		return;
	}

	new_mainw = mainw_new( new_ws );
	gtk_widget_show( GTK_WIDGET( new_mainw ) );

	/* We have to show before recalculate or the window never appears, not
	 * sure why.
	 */
	symbol_recalculate_all();

	progress_end();
}

static void
mainw_workspace_merge_action_cb( GtkAction *action, Mainw *mainw )
{
	mainw_workspace_merge( mainw );
}

static void
mainw_workspace_merge_action_cb2( GtkWidget *wid, GtkWidget *host, 
	Mainw *mainw )
{
	mainw_workspace_merge_action_cb( NULL, mainw );
}

/* Auto recover.
 */
static void
mainw_recover_action_cb( GtkAction *action, Mainw *mainw )
{
	workspace_auto_recover( mainw );
}

/* Callback from make new column.
 */
static void
mainw_column_new_action_cb( GtkAction *action, Mainw *mainw )
{
	char *name;
	Column *col;

	name = workspace_column_name_new( mainw->ws, NULL );
	if( !(col = column_new( mainw->ws, name )) ) 
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
	else
		workspace_column_select( mainw->ws, col );
	IM_FREE( name );
}

/* Same, but from the popup.
 */
static void
mainw_column_new_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_column_new_action_cb( NULL, mainw );
}

/* Done button hit.
 */
static void
mainw_column_new_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Mainw *mainw = MAINW( client );
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

	if( !(col = column_new( mainw->ws, name_text )) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( strcmp( caption_text, "" ) != 0 )
		iobject_set( IOBJECT( col ), NULL, caption_text );

	workspace_column_select( mainw->ws, col );

	nfn( sys, IWINDOW_YES );
}

/* Make a new column with a specified name.
 */
static void
mainw_column_new_named_action_cb( GtkAction *action, Mainw *mainw )
{
	GtkWidget *ss = stringset_new();
	char *name;

	name = workspace_column_name_new( mainw->ws, NULL );

	stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), name, _( "Set column name here" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Caption" ), "", _( "Set column caption here" ) );

	iwindow_set_title( IWINDOW( ss ), _( "New Column" ) );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, mainw );
	idialog_add_ok( IDIALOG( ss ), 
		mainw_column_new_cb, _( "Create Column" ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( mainw ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );

	IM_FREE( name );
}

/* Callback from program.
 */
static void
mainw_program_new_action_cb( GtkAction *action, Mainw *mainw )
{
	Program *program;

	program = program_new( mainw->ws->kitg );

	gtk_widget_show( GTK_WIDGET( program ) ); 
}

/* New ws callback.
 */
static void
mainw_workspace_new_action_cb( GtkAction *action, Mainw *mainw )
{
	Workspacegroup *wsg = WORKSPACEGROUP( ICONTAINER( mainw->ws )->parent );

	workspacegroup_workspace_new( wsg, GTK_WIDGET( mainw ) );
}

/* Callback from auto-recalc toggle.
 */
static void
mainw_autorecalc_action_cb( GtkToggleAction *action, Mainw *mainw )
{
	GSList *i;

	mainw_auto_recalc = gtk_toggle_action_get_active( action );

	/* Yuk! We have to ask all mainw to refresh by hand, since we're not 
	 * using the prefs system for auto_recalc for reasons noted at top.
	 */
	for( i = mainw_all; i; i = i->next ) 
		mainw_refresh( MAINW( i->data ) );

	if( mainw_auto_recalc )
		symbol_recalculate_all();
}

/* Callback from show toolbar toggle.
 */
static void
mainw_toolbar_action_cb( GtkToggleAction *action, Mainw *mainw )
{
	mainw->toolbar_visible = gtk_toggle_action_get_active( action );
	prefs_set( "MAINW_TOOLBAR", 
		"%s", bool_to_char( mainw->toolbar_visible ) );
	mainw_refresh( mainw );
}

/* Callback from show statusbar toggle.
 */
static void
mainw_statusbar_action_cb( GtkToggleAction *action, Mainw *mainw )
{
	mainw->statusbar_visible = gtk_toggle_action_get_active( action );
	prefs_set( "MAINW_STATUSBAR", 
		"%s", bool_to_char( mainw->statusbar_visible ) );
	mainw_refresh( mainw );
}

/* Expose/hide the toolkit browser.
 */
static void
mainw_toolkitbrowser_action_cb( GtkToggleAction *action, Mainw *mainw )
{
	if( gtk_toggle_action_get_active( action ) )
		pane_animate_open( mainw->rpane );
	else
		pane_animate_closed( mainw->rpane );
}

/* Expose/hide the workspace defs.
 */
static void
mainw_workspacedefs_action_cb( GtkToggleAction *action, Mainw *mainw )
{
	if( gtk_toggle_action_get_active( action ) )
		pane_animate_open( mainw->lpane );
	else
		pane_animate_closed( mainw->lpane );
}

/* Layout columns.
 */
static void
mainw_layout_action_cb( GtkAction *action, Mainw *mainw )
{
	model_layout( MODEL( mainw->ws ) );
}

static void
mainw_layout_action_cb2( GtkWidget *wid, GtkWidget *host, Mainw *mainw )
{
	mainw_layout_action_cb( NULL, mainw );
}

/* Remove selected items.
 */
static void
mainw_selected_remove_action_cb( GtkAction *action, Mainw *mainw )
{
	workspace_selected_remove_yesno( mainw->ws, GTK_WIDGET( mainw ) );
}

void 
mainw_revert_ok_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys ) 
{
	Mainw *mainw = MAINW( client );

	if( FILEMODEL( mainw->ws )->filename ) {
		(void) unlinkf( "%s", FILEMODEL( mainw->ws )->filename );
		main_reload();
		symbol_recalculate_all();
	}

	nfn( sys, IWINDOW_YES );
}

void 
mainw_revert_cb( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys ) 
{
	Mainw *mainw = MAINW( client );

	box_yesno( GTK_WIDGET( iwnd ),
		mainw_revert_ok_cb, iwindow_true_cb, mainw,
		nfn, sys,
		_( "Revert to Defaults" ), 
		_( "Revert to installation defaults?" ),
		_( "Would you like to reset all preferences to their factory "
		"settings? This will delete any changes you have ever made "
		"to your preferences and may take a few seconds." ) );
}

static void
mainw_preferences_action_cb( GtkAction *action, Mainw *mainw )
{
	Prefs *prefs;

	/* Can fail if there's no prefs ws, or an error on load.
	 */
	if( !(prefs = prefs_new( NULL )) ) {
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
		return;
	}

	iwindow_set_title( IWINDOW( prefs ), _( "Preferences" ) );
	iwindow_set_parent( IWINDOW( prefs ), GTK_WIDGET( mainw ) );
	idialog_set_callbacks( IDIALOG( prefs ), 
		NULL, NULL, NULL, mainw );
	idialog_add_ok( IDIALOG( prefs ), 
		mainw_revert_cb, _( "Revert to Defaults ..." ) );
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
mainw_magic_cb( gpointer callback_data, guint callback_action,
        GtkWidget *widget )
{
	Workspace *ws = main_workspacegroup->current;
	Row *row = workspace_selected_one( ws );

	if( !row )
		box_alert( mainw,
			"Select a single object with left-click, "
			"select Magic Definition." );
	else if( !magic_sym( row->sym ) )
		iwindow_alert( GTK_WIDGET( mainw ), GTK_MESSAGE_ERROR );
	else
		workspace_deselect_all( ws );
}
 */

#ifdef HAVE_LIBGVC
static void
mainw_graph_action_cb( GtkAction *action, Mainw *mainw )
{
	Graphwindow *graphwindow;

	graphwindow = graphwindow_new( mainw->ws, GTK_WIDGET( mainw ) );
	gtk_widget_show( GTK_WIDGET( graphwindow ) );
}
#endif /*HAVE_LIBGVC*/

/* Set display mode.
 */
static void
mainw_mode_action_cb( GtkRadioAction *action, GtkRadioAction *current, 
	Mainw *mainw )
{
	workspace_set_mode( mainw->ws, 
		gtk_radio_action_get_current_value( action ) );
}

/* Our actions.
 */
static GtkActionEntry mainw_actions[] = {
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
		G_CALLBACK( mainw_column_new_action_cb ) },

	{ "NewColumnName", 
		GTK_STOCK_NEW, N_( "New Named C_olumn" ), NULL, 
		N_( "Create a new column with a specified name" ), 
		G_CALLBACK( mainw_column_new_named_action_cb ) },

	{ "NewWorkspace", 
		GTK_STOCK_NEW, N_( "New _Workspace" ), NULL, 
		N_( "Create a new workspace" ), 
		G_CALLBACK( mainw_workspace_new_action_cb ) },

	{ "Open", 
		GTK_STOCK_OPEN, N_( "_Open" ), NULL,
		N_( "Open a file" ), 
		G_CALLBACK( mainw_open_action_cb ) },

	{ "OpenExamples", 
		NULL, N_( "Open _Examples" ), NULL,
		N_( "Open example workspaces" ), 
		G_CALLBACK( mainw_open_examples_action_cb ) },

	{ "DuplicateWorkspace", 
		STOCK_DUPLICATE, N_( "_Duplicate Workspace" ), NULL,
		N_( "Duplicate workspace" ), 
		G_CALLBACK( mainw_workspace_duplicate_action_cb ) },

	{ "Merge", 
		NULL, N_( "_Merge Workspace" ), NULL, 
		N_( "Merge workspace into this workspace" ), 
		G_CALLBACK( mainw_workspace_merge_action_cb ) },

	{ "Save", 
		GTK_STOCK_SAVE, N_( "_Save Workspace" ), NULL,
		N_( "Save workspace" ), 
		G_CALLBACK( mainw_workspace_save_action_cb ) },

	{ "SaveAs", 
		GTK_STOCK_SAVE_AS, N_( "_Save Workspace As" ), NULL,
		N_( "Save workspace as" ), 
		G_CALLBACK( mainw_workspace_save_as_action_cb ) },

	{ "Recover", 
		NULL, N_( "Search for Workspace _Backups" ), NULL,
		N_( "Load last automatically backed-up workspace" ), 
		G_CALLBACK( mainw_recover_action_cb ) },

	{ "Delete", 
		GTK_STOCK_DELETE, N_( "_Delete" ), "<Shift>BackSpace",
		N_( "Delete selected items" ), 
		G_CALLBACK( mainw_selected_remove_action_cb ) },

	{ "SelectAll", 
		NULL, N_( "Select _All" ), "<control>A",
		N_( "Select all items" ), 
		G_CALLBACK( mainw_select_all_action_cb ) },

	{ "Duplicate", 
		STOCK_DUPLICATE, N_( "D_uplicate Selected" ), "<control>U",
		N_( "Duplicate selected items" ), 
		G_CALLBACK( mainw_duplicate_action_cb ) },

	{ "Recalculate", 
		NULL, N_( "_Recalculate" ), NULL,
		N_( "Recalculate selected items" ), 
		G_CALLBACK( mainw_force_calc_action_cb ) },

	{ "AlignColumns", 
		NULL, N_( "Align _Columns" ), NULL,
		N_( "Align columns to grid" ), 
		G_CALLBACK( mainw_layout_action_cb ) },

	{ "Find", 
		GTK_STOCK_FIND, N_( "_Find" ), NULL,
		N_( "Find in workspace" ), 
		G_CALLBACK( mainw_find_action_cb ) },

	{ "FindNext", 
		NULL, N_( "Find _Next" ), NULL,
		N_( "Find again in workspace" ), 
		G_CALLBACK( mainw_find_again_action_cb ) },

	{ "NextError", 
		STOCK_NEXT_ERROR, NULL, NULL,
		N_( "Jump to next error" ), 
		G_CALLBACK( mainw_next_error_action_cb ) },

	{ "Group", 
		NULL, N_( "_Group" ), NULL,
		N_( "Group selected items" ), 
		G_CALLBACK( mainw_group_action_cb ) },

	{ "Ungroup", 
		NULL, N_( "U_ngroup" ), NULL,
		N_( "Ungroup selected items" ), 
		G_CALLBACK( mainw_ungroup_action_cb ) },

	{ "Preferences", 
		GTK_STOCK_PREFERENCES, N_( "_Preferences" ), NULL,
		N_( "Edit preferences" ), 
		G_CALLBACK( mainw_preferences_action_cb ) },

#ifdef HAVE_LIBGVC
	{ "Graph", 
		NULL, N_( "Workspace as Grap_h" ), NULL,
		N_( "Show a graph of workspace dependencies" ), 
		G_CALLBACK( mainw_graph_action_cb ) },
#endif /*HAVE_LIBGVC*/

	{ "EditToolkits", 
		NULL, N_( "_Edit" ), NULL,
		N_( "Edit toolkits" ), 
		G_CALLBACK( mainw_program_new_action_cb ) }
};

static GtkToggleActionEntry mainw_toggle_actions[] = {
	{ "AutoRecalculate",
		NULL, N_( "Au_to Recalculate" ), NULL,
		N_( "Recalculate automatically on change" ),
		G_CALLBACK( mainw_autorecalc_action_cb ), TRUE },

	{ "Toolbar",
		NULL, N_( "_Toolbar" ), NULL,
		N_( "Show window toolbar" ),
		G_CALLBACK( mainw_toolbar_action_cb ), TRUE },

	{ "Statusbar",
		NULL, N_( "_Statusbar" ), NULL,
		N_( "Show window statusbar" ),
		G_CALLBACK( mainw_statusbar_action_cb ), TRUE },

	{ "ToolkitBrowser",
		NULL, N_( "Toolkit _Browser" ), NULL,
		N_( "Show toolkit browser" ),
		G_CALLBACK( mainw_toolkitbrowser_action_cb ), FALSE },

	{ "WorkspaceDefs",
		NULL, N_( "Workspace _Definitions" ), NULL,
		N_( "Show workspace definitions" ),
		G_CALLBACK( mainw_workspacedefs_action_cb ), FALSE },
};

static GtkRadioActionEntry mainw_radio_actions[] = {
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

static const char *mainw_menubar_ui_description =
"<ui>"
"  <menubar name='MainwMenubar'>"
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

static const char *mainw_toolbar_ui_description =
"<ui>"
"  <toolbar name='MainwToolbar'>"
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
mainw_lpane_changed_cb( Pane *pane, Mainw *mainw )
{
	if( mainw->ws->lpane_open != pane->open ||
		mainw->ws->lpane_position != pane->user_position ) {
		mainw->ws->lpane_open = pane->open;
		mainw->ws->lpane_position = pane->user_position;

		mainw_refresh( mainw );
	}
}

static void
mainw_rpane_changed_cb( Pane *pane, Mainw *mainw )
{
	if( mainw->ws->rpane_open != pane->open ||
		mainw->ws->rpane_position != pane->user_position ) {
		mainw->ws->rpane_open = pane->open;
		mainw->ws->rpane_position = pane->user_position;

		mainw_refresh( mainw );
	}
}

static void
mainw_watch_changed_cb( Watchgroup *watchgroup, Watch *watch, Mainw *mainw )
{
	if( strcmp( IOBJECT( watch )->name, "MAINW_TOOLBAR_STYLE" ) == 0 )
		mainw->toolbar_visible = MAINW_TOOLBAR;

	mainw_refresh( mainw );
}

/* Make the insides of the panel.
 */
static void
mainw_build( iWindow *iwnd, GtkWidget *vbox )
{
	Mainw *mainw = MAINW( iwnd );

        GtkWidget *mbar;
	GtkWidget *frame;
	GtkWidget *ebox;
	GError *error;
	GtkWidget *cancel;
	GtkWidget *item;
	Panechild *panechild;

#ifdef DEBUG
	printf( "mainw_init: %p\n", mainw );
#endif /*DEBUG*/

        /* Make main menu bar
         */
	gtk_action_group_add_actions( iwnd->action_group, 
		mainw_actions, G_N_ELEMENTS( mainw_actions ), 
		GTK_WINDOW( mainw ) );
	gtk_action_group_add_toggle_actions( iwnd->action_group, 
		mainw_toggle_actions, G_N_ELEMENTS( mainw_toggle_actions ), 
		GTK_WINDOW( mainw ) );
	gtk_action_group_add_radio_actions( iwnd->action_group,
		mainw_radio_actions, G_N_ELEMENTS( mainw_radio_actions ), 
		WORKSPACE_MODE_REGULAR,
		G_CALLBACK( mainw_mode_action_cb ),
		GTK_WINDOW( mainw ) );

	error = NULL;
	if( !gtk_ui_manager_add_ui_from_string( iwnd->ui_manager,
			mainw_menubar_ui_description, -1, &error ) ||
		!gtk_ui_manager_add_ui_from_string( iwnd->ui_manager,
			mainw_toolbar_ui_description, -1, &error ) ) {
		g_message( "building menus failed: %s", error->message );
		g_error_free( error );
		exit( EXIT_FAILURE );
	}

	mbar = gtk_ui_manager_get_widget( iwnd->ui_manager, "/MainwMenubar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mbar, FALSE, FALSE, 0 );
        gtk_widget_show( mbar );

	/* Get the dummy item on the recent menu, then get the enclosing menu
	 * for that dummy item.
	 */
        item = gtk_ui_manager_get_widget( iwnd->ui_manager,
		"/MainwMenubar/FileMenu/RecentMenu/Stub" );
	mainw->recent_menu = gtk_widget_get_parent( GTK_WIDGET( item ) );
        gtk_signal_connect( GTK_OBJECT( mainw->recent_menu ), "map",
                GTK_SIGNAL_FUNC( mainw_recent_map_cb ), mainw );

	/* Same for the column jump menu.
	 */
        item = gtk_ui_manager_get_widget( iwnd->ui_manager,
		"/MainwMenubar/EditMenu/JumpToColumnMenu/Stub" );
	mainw->jump_to_column_menu = 
		gtk_widget_get_parent( GTK_WIDGET( item ) );

	/* Attach toolbar.
  	 */
	mainw->toolbar = gtk_ui_manager_get_widget( 
		iwnd->ui_manager, "/MainwToolbar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mainw->toolbar, FALSE, FALSE, 0 );
        widget_visible( mainw->toolbar, MAINW_TOOLBAR );

	/* This will set to NULL if we don't have infobar support.
	 */
	if( (IWINDOW( mainw )->infobar = infobar_new()) ) 
		gtk_box_pack_start( GTK_BOX( vbox ), 
			GTK_WIDGET( IWINDOW( mainw )->infobar ), 
			FALSE, FALSE, 0 );

	/* hbox for status bar etc.
	 */
        mainw->statusbar_main = gtk_hbox_new( FALSE, 2 );
        gtk_box_pack_end( GTK_BOX( vbox ), 
		mainw->statusbar_main, FALSE, FALSE, 2 );
        widget_visible( mainw->statusbar_main, MAINW_STATUSBAR );

	/* Make space free label.
	 */
	mainw->space_free_eb = gtk_event_box_new();
        gtk_box_pack_start( GTK_BOX( mainw->statusbar_main ), 
		mainw->space_free_eb, FALSE, FALSE, 0 );
	gtk_widget_show( mainw->space_free_eb );
	frame = gtk_frame_new( NULL );
	gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_IN );
	gtk_container_add( GTK_CONTAINER( mainw->space_free_eb ), frame );
	gtk_widget_show( frame );
	mainw->space_free = gtk_label_new( "space_free" );
	gtk_misc_set_padding( GTK_MISC( mainw->space_free ), 2, 2 );
        gtk_container_add( GTK_CONTAINER( frame ), mainw->space_free );
        gtk_signal_connect( GTK_OBJECT( mainw->space_free_eb ), "event",
                GTK_SIGNAL_FUNC( mainw_space_free_event ), mainw );
	set_tooltip_generate( mainw->space_free_eb,
		(TooltipGenerateFn) mainw_space_free_tooltip_generate, 
		mainw, NULL );
        gtk_widget_show( mainw->space_free );
	mainw->imageinfo_changed_sid = g_signal_connect( main_imageinfogroup, 
		"changed",
		G_CALLBACK( mainw_free_changed_cb ), mainw );
	mainw->heap_changed_sid = g_signal_connect( reduce_context->heap, 
		"changed",
		G_CALLBACK( mainw_free_changed_cb ), mainw );

	/* Make message label.
	 */
	mainw->statusbar = gtk_label_new( "" );
	gtk_label_set_ellipsize( GTK_LABEL( mainw->statusbar ), 
		PANGO_ELLIPSIZE_MIDDLE );
	/* 6 is enough to stop the statusbar changing height when the progress
	 * indicator changes visibility.
	 */
	gtk_misc_set_padding( GTK_MISC( mainw->statusbar ), 2, 6 );
	gtk_misc_set_alignment( GTK_MISC( mainw->statusbar ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( mainw->statusbar_main ), 
		mainw->statusbar, TRUE, TRUE, 0 );
        gtk_widget_show( mainw->statusbar );

        mainw->progress_box = gtk_hbox_new( FALSE, 2 );

	mainw->progress = gtk_progress_bar_new();
        gtk_box_pack_end( GTK_BOX( mainw->progress_box ), mainw->progress, 
		FALSE, TRUE, 0 );
        gtk_widget_show( mainw->progress );

        cancel = gtk_button_new_with_label( "Cancel" );
        g_signal_connect( cancel, "clicked",
                G_CALLBACK( mainw_cancel_cb ), mainw );
        gtk_box_pack_end( GTK_BOX( mainw->progress_box ), cancel, 
		FALSE, TRUE, 0 );
        gtk_widget_show( cancel );

        gtk_box_pack_end( GTK_BOX( mainw->statusbar_main ), 
		mainw->progress_box, FALSE, TRUE, 0 );

	/* Make toolkit/workspace displays.
	 */
	mainw->kitgview = TOOLKITGROUPVIEW( 
		model_view_new( MODEL( mainw->ws->kitg ), NULL ) );
	g_object_ref( G_OBJECT( mainw->kitgview ) );
	gtk_object_sink( GTK_OBJECT( mainw->kitgview ) );
	toolkitgroupview_set_mainw( mainw->kitgview, mainw );
	gtk_menu_set_accel_group( GTK_MENU( mainw->kitgview->menu ),
		iwnd->accel_group );

	mainw->rpane = pane_new( PANE_HIDE_RIGHT );
	g_signal_connect( mainw->rpane, "changed",
		G_CALLBACK( mainw_rpane_changed_cb ), mainw );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( mainw->rpane ), TRUE, TRUE, 0 );
	gtk_widget_show( GTK_WIDGET( mainw->rpane ) );

	mainw->lpane = pane_new( PANE_HIDE_LEFT );
	g_signal_connect( mainw->lpane, "changed",
		G_CALLBACK( mainw_lpane_changed_cb ), mainw );
	gtk_paned_pack1( GTK_PANED( mainw->rpane ), GTK_WIDGET( mainw->lpane ),
		TRUE, FALSE );
	gtk_widget_show( GTK_WIDGET( mainw->lpane ) );

	mainw->wsview = WORKSPACEVIEW( 
		model_view_new( MODEL( mainw->ws ), NULL ) );
	gtk_paned_pack2( GTK_PANED( mainw->lpane ), GTK_WIDGET( mainw->wsview ),
		TRUE, FALSE );
	gtk_widget_show( GTK_WIDGET( mainw->wsview ) );

	mainw->popup = popup_build( _( "Workspace menu" ) );
	popup_add_but( mainw->popup, _( "New C_olumn" ),
		POPUP_FUNC( mainw_column_new_action_cb2 ) ); 
	mainw->popup_jump =
		popup_add_pullright( mainw->popup, _( "Jump to _Column" ) ); 
	popup_add_but( mainw->popup, _( "Align _Columns" ),
		POPUP_FUNC( mainw_layout_action_cb2 ) ); 
	menu_add_sep( mainw->popup );
	popup_add_but( mainw->popup, GTK_STOCK_OPEN,
		POPUP_FUNC( mainw_open_action_cb2 ) ); 
	popup_add_but( mainw->popup, _( "_Merge Workspace from File" ),
		POPUP_FUNC( mainw_workspace_merge_action_cb2 ) ); 
	menu_add_sep( mainw->popup );
	popup_add_but( mainw->popup, _( "_Group Selected" ),
		POPUP_FUNC( mainw_group_action_cb2 ) ); 
	menu_add_sep( mainw->popup );
	popup_add_but( mainw->popup, STOCK_NEXT_ERROR,
		POPUP_FUNC( mainw_next_error_action_cb2 ) ); 
        popup_attach( mainw->wsview->fixed, mainw->popup, mainw );

	/* Toolkit Browser pane.
	 */
	panechild = panechild_new( mainw->rpane, _( "Toolkit Browser" ) );

	/* Have to put toolkitbrowser in an ebox so the search entry gets
	 * clipped to the pane size.
	 */
	ebox = gtk_event_box_new();
	gtk_container_add( GTK_CONTAINER( panechild ), GTK_WIDGET( ebox ) );
	gtk_widget_show( ebox );

	mainw->toolkitbrowser = toolkitbrowser_new();
	vobject_link( VOBJECT( mainw->toolkitbrowser ), 
		IOBJECT( mainw->ws->kitg ) );
	toolkitbrowser_set_mainw( mainw->toolkitbrowser, mainw );
	gtk_container_add( GTK_CONTAINER( ebox ), 
		GTK_WIDGET( mainw->toolkitbrowser ) );
	gtk_widget_show( GTK_WIDGET( mainw->toolkitbrowser ) );

	/* Workspace-local defs pane.
	 */
	panechild = panechild_new( mainw->lpane, _( "Workspace Definitions" ) );

	mainw->workspacedefs = workspacedefs_new( mainw );
	vobject_link( VOBJECT( mainw->workspacedefs ), IOBJECT( mainw->ws ) );
	gtk_container_add( GTK_CONTAINER( panechild ), 
		GTK_WIDGET( mainw->workspacedefs ) );
	gtk_widget_show( GTK_WIDGET( mainw->workspacedefs ) );

	/* Set start state.
	 */
	(void) mainw_refresh( mainw );

	/* Any changes to prefs, refresh (yuk!).
	 */
	mainw->watch_changed_sid = g_signal_connect( main_watchgroup, 
		"watch_changed",
		G_CALLBACK( mainw_watch_changed_cb ), mainw );
}

static void
mainw_popdown( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys )
{
	Mainw *mainw = MAINW( iwnd );

	/* We can be destroyed in two ways: either our iwnd tells us to go, or
	 * our model is destroyed under us. If the model has gone, we just go.
	 * If the model is still there, we need to ask about saving and
	 * quitting.
	 */
	if( mainw->ws ) {
		/* Argh, cancel this immediately, in case we close before
		 * we're up properly.
		 */
		IM_FREEF( g_source_remove, mainw->compat_timeout );

		filemodel_inter_savenclose_cb( IWINDOW( mainw ), 
			FILEMODEL( mainw->ws ), nfn, sys );
	}
	else
		nfn( sys, IWINDOW_YES );
}

static void
mainw_link( Mainw *mainw, Workspace *ws )
{
	g_assert( !mainw->ws );

	mainw->ws = ws;
	ws->iwnd = IWINDOW( mainw );
	mainw->destroy_sid = g_signal_connect( ws, "destroy",
		G_CALLBACK( mainw_workspace_destroy_cb ), mainw );
	mainw->changed_sid = g_signal_connect( ws, "changed",
		G_CALLBACK( mainw_workspace_changed_cb ), mainw );
	iwindow_set_build( IWINDOW( mainw ), 
		(iWindowBuildFn) mainw_build, ws, NULL, NULL );
	iwindow_set_popdown( IWINDOW( mainw ), mainw_popdown, NULL );
	filemodel_set_window_hint( FILEMODEL( ws ), IWINDOW( mainw ) );

	/* If we have a saved size for this workspace, set that. Otherwise,
	 * default to the default.
	 */
	if( !ws->window_width ) {
		iwindow_set_size_prefs( IWINDOW( mainw ), 
			"MAINW_WINDOW_WIDTH", "MAINW_WINDOW_HEIGHT" );
	}

	iwindow_build( IWINDOW( mainw ) );

	if( ws->window_width ) {
		GdkScreen *screen = 
			gtk_widget_get_screen( GTK_WIDGET( mainw ) );

		gtk_window_set_default_size( GTK_WINDOW( mainw ), 
			IM_MIN( ws->window_width, 
				gdk_screen_get_width( screen ) ),
			IM_MIN( ws->window_height, 
				gdk_screen_get_height( screen ) ) );
	}

	pane_set_state( mainw->lpane, ws->lpane_open, ws->lpane_position );
	pane_set_state( mainw->rpane, ws->rpane_open, ws->rpane_position );
}

Mainw *
mainw_new( Workspace *ws )
{
	Mainw *mainw;

	/* We must have calced before we build the view so we get menus etc.
	 */
	symbol_recalculate_all_force( TRUE );

	mainw = MAINW( g_object_new( TYPE_MAINW, NULL ) );
	mainw_link( mainw, ws );

	return( mainw );
}
