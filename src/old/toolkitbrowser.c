/* Toolkitbrowser dialog.
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

/*
#define DEBUG
 */

#include "ip.h"

G_DEFINE_TYPE( Toolkitbrowser, toolkitbrowser, TYPE_VOBJECT ); 

/* Our columns.
 */
enum {
	TOOLTIP_COLUMN,		/* Visible columns */
	MENU_COLUMN, 		
	NPARAM_COLUMN,
	TOOLITEM_COLUMN,	/* Secret column */
	N_COLUMNS
};

static void
toolkitbrowser_destroy( GtkWidget *widget )
{
	Toolkitbrowser *toolkitbrowser = TOOLKITBROWSER( widget );

	UNREF( toolkitbrowser->filter );
	UNREF( toolkitbrowser->store );

	GTK_WIDGET_CLASS( toolkitbrowser_parent_class )->destroy( widget );
}

static void *
toolkitbrowser_rebuild_item_sub( Symbol *param, VipsBuf *buf )
{
	vips_buf_appends( buf, " " );
	vips_buf_appends( buf, IOBJECT( param )->name );

	return( NULL );
}

static void *
toolkitbrowser_rebuild_item3( Toolitem *toolitem,
	Toolkitbrowser *toolkitbrowser )
{
	if( !toolitem->is_pullright && 
		!toolitem->is_separator &&
		toolitem->compile ) {
		char txt[256];
		VipsBuf buf = VIPS_BUF_STATIC( txt );
		GtkTreeIter iter;

		if( toolitem->action_sym && 
			toolitem->action_sym->expr &&
			toolitem->action_sym->expr->compile->param ) 
			slist_map( toolitem->action_sym->expr->compile->param,
				(SListMapFn) toolkitbrowser_rebuild_item_sub, 
				&buf );

		gtk_list_store_append( toolkitbrowser->store, &iter );
		gtk_list_store_set( toolkitbrowser->store, &iter,
			TOOLTIP_COLUMN, toolitem->tooltip,
			MENU_COLUMN, toolitem->user_path,
			NPARAM_COLUMN, vips_buf_all( &buf ),
			TOOLITEM_COLUMN, toolitem,
			-1 );
	}

	slist_map( toolitem->children,
		(SListMapFn) toolkitbrowser_rebuild_item3, toolkitbrowser );

	return( NULL );
}

static void *
toolkitbrowser_rebuild_item2( Tool *tool, Toolkitbrowser *toolkitbrowser )
{
	if( tool->toolitem )
		toolkitbrowser_rebuild_item3( tool->toolitem, toolkitbrowser );

	return( NULL );
}

static void *
toolkitbrowser_rebuild_item( Toolkit *kit, Toolkitbrowser *toolkitbrowser )
{
	toolkit_map( kit, 
		(tool_map_fn) toolkitbrowser_rebuild_item2,
		toolkitbrowser, NULL );

	return( NULL );
}

static void
toolkitbrowser_refresh( vObject *vobject )
{
	Toolkitbrowser *toolkitbrowser = TOOLKITBROWSER( vobject );

#ifdef DEBUG
	printf( "toolkitbrowser_refresh:\n" );
#endif /*DEBUG*/

	gtk_list_store_clear( toolkitbrowser->store );
	toolkitgroup_map( toolkitbrowser->kitg,
		(toolkit_map_fn) toolkitbrowser_rebuild_item, 
		toolkitbrowser, NULL );

	VOBJECT_CLASS( toolkitbrowser_parent_class )->refresh( vobject );
}

static void
toolkitbrowser_link( vObject *vobject, iObject *iobject )
{
	Toolkitbrowser *toolkitbrowser = TOOLKITBROWSER( vobject );
	Toolkitgroup *kitg = TOOLKITGROUP( iobject );

	g_assert( !toolkitbrowser->kitg );

	toolkitbrowser->kitg = kitg;

	VOBJECT_CLASS( toolkitbrowser_parent_class )->link( vobject, iobject );
}

static void
toolkitbrowser_class_init( ToolkitbrowserClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;

	widget_class->destroy = toolkitbrowser_destroy;

	vobject_class->refresh = toolkitbrowser_refresh;
	vobject_class->link = toolkitbrowser_link;
}

static void
toolkitbrowser_entry_changed_cb( GtkEditable *editable, 
	Toolkitbrowser *toolkitbrowser )
{
	gtk_tree_model_filter_refilter( 
		GTK_TREE_MODEL_FILTER( toolkitbrowser->filter ) );
}

static gboolean
toolkitbrowser_rebuild_test( Toolitem *toolitem, const char *text )
{
	if( my_strcasestr( toolitem->user_path, text ) || 
		my_strcasestr( toolitem->tooltip, text ) )
		return( TRUE );

	return( FALSE );
}

static gboolean
toolkitbrowser_visible_func( GtkTreeModel *model, GtkTreeIter *iter, 
	gpointer data )
{
	Toolkitbrowser *toolkitbrowser = TOOLKITBROWSER( data );
	const char *text = gtk_entry_get_text( 
		GTK_ENTRY( toolkitbrowser->entry ) );
	Toolitem *toolitem;

	gtk_tree_model_get( model, iter, TOOLITEM_COLUMN, &toolitem, -1 );
	if( !toolitem )
		return( FALSE );

	return( toolkitbrowser_rebuild_test( toolitem, text ) );
}

static Toolitem *
toolkitbrowser_get_selected( Toolkitbrowser *toolkitbrowser )
{
	GtkTreeSelection *selection = gtk_tree_view_get_selection( 
		GTK_TREE_VIEW( toolkitbrowser->tree ) );
	GtkTreeIter iter;
	GtkTreeModel *model;
	Toolitem *toolitem; 

        if( gtk_tree_selection_get_selected( selection, &model, &iter ) ) {
		gtk_tree_model_get( model, &iter, 
			TOOLITEM_COLUMN, &toolitem, -1 );

		return( toolitem );
        }

	return( NULL );
}

static gboolean
toolkitbrowser_activate_selected( Toolkitbrowser *toolkitbrowser )
{
	Toolitem *toolitem;

        if( (toolitem = toolkitbrowser_get_selected( toolkitbrowser )) ) {
		if( !workspace_add_action( toolkitbrowser->ws, 
			toolitem->name, toolitem->action, 
			toolitem->action_sym->expr->compile->nparam ) )
			return( FALSE );
        }

	return( TRUE );
}

static void
toolkitbrowser_row_activated_cb( GtkTreeView *treeview,
	GtkTreePath *arg1, GtkTreeViewColumn *arg2, 
	Toolkitbrowser *toolkitbrowser )
{
	if( !toolkitbrowser_activate_selected( toolkitbrowser ) )
		iwindow_alert( GTK_WIDGET( toolkitbrowser ), 
			GTK_MESSAGE_ERROR );
}

static void
toolkitbrowser_init( Toolkitbrowser *toolkitbrowser )
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkWidget *label;
	GtkWidget *swin;

	toolkitbrowser->top = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 12 );
	toolkitbrowser->entry = gtk_entry_new();
        g_signal_connect( toolkitbrowser->entry, "changed", 
		G_CALLBACK( toolkitbrowser_entry_changed_cb ), 
		toolkitbrowser );
	gtk_box_pack_end( GTK_BOX( toolkitbrowser->top ), 
		toolkitbrowser->entry, FALSE, FALSE, 2 );
	label = gtk_image_new_from_icon_name( "find", GTK_ICON_SIZE_MENU );
	gtk_box_pack_end( GTK_BOX( toolkitbrowser->top ), 
		label, FALSE, FALSE, 0 );
        gtk_box_pack_start( GTK_BOX( toolkitbrowser ), 
		toolkitbrowser->top, FALSE, FALSE, 2 );
	gtk_widget_show_all( toolkitbrowser->top );

	toolkitbrowser->store = gtk_list_store_new( N_COLUMNS, 
		G_TYPE_STRING, 
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_POINTER );

	toolkitbrowser->filter = gtk_tree_model_filter_new( 
		GTK_TREE_MODEL( toolkitbrowser->store ), NULL );
	gtk_tree_model_filter_set_visible_func( 
		GTK_TREE_MODEL_FILTER( toolkitbrowser->filter ), 
		toolkitbrowser_visible_func, toolkitbrowser, NULL );

	toolkitbrowser->tree = gtk_tree_view_new_with_model( 
		GTK_TREE_MODEL( toolkitbrowser->filter ) );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Action" ),
		   renderer, "text", TOOLTIP_COLUMN, NULL );
	gtk_tree_view_column_set_resizable( column, TRUE );
	gtk_tree_view_column_set_reorderable( column, TRUE );
	gtk_tree_view_append_column( GTK_TREE_VIEW( toolkitbrowser->tree ), 
		column );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Parameters" ),
		   renderer, "text", NPARAM_COLUMN, NULL );
	gtk_tree_view_column_set_resizable( column, TRUE );
	gtk_tree_view_column_set_reorderable( column, TRUE );
	gtk_tree_view_append_column( GTK_TREE_VIEW( toolkitbrowser->tree ), 
		column );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Menu Item" ),
		   renderer, "text", MENU_COLUMN, NULL );
	gtk_tree_view_column_set_resizable( column, TRUE );
	gtk_tree_view_column_set_reorderable( column, TRUE );
	gtk_tree_view_append_column( GTK_TREE_VIEW( toolkitbrowser->tree ), 
		column );

	g_signal_connect( G_OBJECT( toolkitbrowser->tree ), "row-activated",
		  G_CALLBACK( toolkitbrowser_row_activated_cb ), 
		  toolkitbrowser );

	swin = gtk_scrolled_window_new( NULL, NULL );
        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_container_add( GTK_CONTAINER( swin ), toolkitbrowser->tree );

        gtk_box_pack_start( GTK_BOX( toolkitbrowser ), swin, TRUE, TRUE, 2 );
	gtk_widget_show_all( swin );
}

Toolkitbrowser *
toolkitbrowser_new( void )
{
	Toolkitbrowser *toolkitbrowser = 
		g_object_new( TYPE_TOOLKITBROWSER, NULL );

	return( toolkitbrowser );
}

/* Find the 'natural' width of the browser.
 */
int 
toolkitbrowser_get_width( Toolkitbrowser *toolkitbrowser )
{
	if( toolkitbrowser->top ) {
		GtkRequisition minimum_size;
		GtkRequisition natural_size;

		gtk_widget_get_preferred_size( 
			GTK_WIDGET( toolkitbrowser->top ), 
			&minimum_size, &natural_size );

		return( natural_size.width );
	}
	else
		return( 200 );
}

void
toolkitbrowser_set_workspace( Toolkitbrowser *toolkitbrowser, Workspace *ws )
{
	g_assert( !toolkitbrowser->ws );

	toolkitbrowser->ws = ws;
}
