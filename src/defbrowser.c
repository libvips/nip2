/* Defbrowser dialog.
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
defbrowser_destroy( GtkObject *object )
{
	Defbrowser *defbrowser = DEFBROWSER( object );

	UNREF( defbrowser->store );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void *
defbrowser_rebuild_item_sub( Symbol *param, VipsBuf *buf )
{
	vips_buf_appends( buf, " " );
	vips_buf_appends( buf, IOBJECT( param )->name );

	return( NULL );
}

static void *
defbrowser_rebuild_item3( Toolitem *toolitem,
	Defbrowser *defbrowser )
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
				(SListMapFn) defbrowser_rebuild_item_sub, 
				&buf );

		gtk_list_store_append( defbrowser->store, &iter );
		gtk_list_store_set( defbrowser->store, &iter,
			TOOLTIP_COLUMN, toolitem->tooltip,
			MENU_COLUMN, toolitem->user_path,
			NPARAM_COLUMN, vips_buf_all( &buf ),
			TOOLITEM_COLUMN, toolitem,
			-1 );
	}

	slist_map( toolitem->children,
		(SListMapFn) defbrowser_rebuild_item3, defbrowser );

	return( NULL );
}

static void *
defbrowser_rebuild_item2( Tool *tool, Defbrowser *defbrowser )
{
	if( tool->toolitem )
		defbrowser_rebuild_item3( tool->toolitem, defbrowser );

	return( NULL );
}

static void *
defbrowser_rebuild_item( Toolkit *kit, Defbrowser *defbrowser )
{
	toolkit_map( kit, 
		(tool_map_fn) defbrowser_rebuild_item2,
		defbrowser, NULL );

	return( NULL );
}

static void
defbrowser_refresh( vObject *vobject )
{
	Defbrowser *defbrowser = DEFBROWSER( vobject );

#ifdef DEBUG
	printf( "defbrowser_refresh:\n" );
#endif /*DEBUG*/

	gtk_list_store_clear( defbrowser->store );
	toolkitgroup_map( defbrowser->program->kitg,
		(toolkit_map_fn) defbrowser_rebuild_item, 
		defbrowser, NULL );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
defbrowser_class_init( DefbrowserClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = defbrowser_destroy;

	vobject_class->refresh = defbrowser_refresh;
}

static void
defbrowser_entry_changed_cb( GtkEditable *editable, 
	Defbrowser *defbrowser )
{
	gtk_tree_model_filter_refilter( 
		GTK_TREE_MODEL_FILTER( defbrowser->filter ) );
}

static gboolean
defbrowser_rebuild_test( Toolitem *toolitem, const char *text )
{
	if( my_strcasestr( toolitem->user_path, text ) || 
		my_strcasestr( toolitem->tooltip, text ) )
		return( TRUE );

	return( FALSE );
}

static gboolean
defbrowser_visible_func( GtkTreeModel *model, GtkTreeIter *iter, 
	gpointer data )
{
	Defbrowser *defbrowser = DEFBROWSER( data );
	const char *text = gtk_entry_get_text( 
		GTK_ENTRY( defbrowser->entry ) );
	Toolitem *toolitem;

	gtk_tree_model_get( model, iter, TOOLITEM_COLUMN, &toolitem, -1 );
	if( !toolitem )
		return( FALSE );

	return( defbrowser_rebuild_test( toolitem, text ) );
}

static Toolitem *
defbrowser_get_selected( Defbrowser *defbrowser )
{
	GtkTreeSelection *selection = gtk_tree_view_get_selection( 
		GTK_TREE_VIEW( defbrowser->tree ) );
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
defbrowser_activate_selected( Defbrowser *defbrowser )
{
	Toolitem *toolitem = defbrowser_get_selected( defbrowser ); 

        if( toolitem ) {
		/*
		if( !workspace_add_action( defbrowser->mainw->ws, 
			toolitem->name, toolitem->action, 
			toolitem->action_sym->expr->compile->nparam ) )
			return( FALSE );
		 */
        }

	return( TRUE );
}

static void
defbrowser_row_activated_cb( GtkTreeView *treeview,
	GtkTreePath *arg1, GtkTreeViewColumn *arg2, 
	Defbrowser *defbrowser )
{
	if( !defbrowser_activate_selected( defbrowser ) )
		iwindow_alert( GTK_WIDGET( defbrowser ), 
			GTK_MESSAGE_ERROR );
}

static void
defbrowser_init( Defbrowser *defbrowser )
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkWidget *label;
	GtkWidget *swin;

	defbrowser->top = gtk_hbox_new( FALSE, 12 );
	defbrowser->entry = gtk_entry_new();
        gtk_signal_connect( GTK_OBJECT( defbrowser->entry ), "changed", 
		GTK_SIGNAL_FUNC( defbrowser_entry_changed_cb ), 
		defbrowser );
	gtk_box_pack_end( GTK_BOX( defbrowser->top ), 
		defbrowser->entry, FALSE, FALSE, 2 );
	label = gtk_image_new_from_stock( GTK_STOCK_FIND, GTK_ICON_SIZE_MENU );
	gtk_box_pack_end( GTK_BOX( defbrowser->top ), 
		label, FALSE, FALSE, 0 );
        gtk_box_pack_start( GTK_BOX( defbrowser ), 
		defbrowser->top, FALSE, FALSE, 2 );
	gtk_widget_show_all( defbrowser->top );

	defbrowser->store = gtk_list_store_new( N_COLUMNS, 
		G_TYPE_STRING, 
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_POINTER );

	defbrowser->filter = gtk_tree_model_filter_new( 
		GTK_TREE_MODEL( defbrowser->store ), NULL );
	gtk_tree_model_filter_set_visible_func( 
		GTK_TREE_MODEL_FILTER( defbrowser->filter ), 
		defbrowser_visible_func, defbrowser, NULL );

	defbrowser->tree = gtk_tree_view_new_with_model( 
		GTK_TREE_MODEL( defbrowser->filter ) );
	gtk_tree_view_set_rules_hint( GTK_TREE_VIEW( defbrowser->tree ),
		TRUE );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Action" ),
		   renderer, "text", TOOLTIP_COLUMN, NULL );
	gtk_tree_view_column_set_resizable( column, TRUE );
	gtk_tree_view_column_set_reorderable( column, TRUE );
	gtk_tree_view_append_column( GTK_TREE_VIEW( defbrowser->tree ), 
		column );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Parameters" ),
		   renderer, "text", NPARAM_COLUMN, NULL );
	gtk_tree_view_column_set_resizable( column, TRUE );
	gtk_tree_view_column_set_reorderable( column, TRUE );
	gtk_tree_view_append_column( GTK_TREE_VIEW( defbrowser->tree ), 
		column );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Menu Item" ),
		   renderer, "text", MENU_COLUMN, NULL );
	gtk_tree_view_column_set_resizable( column, TRUE );
	gtk_tree_view_column_set_reorderable( column, TRUE );
	gtk_tree_view_append_column( GTK_TREE_VIEW( defbrowser->tree ), 
		column );

	g_signal_connect( G_OBJECT( defbrowser->tree ), "row-activated",
		  G_CALLBACK( defbrowser_row_activated_cb ), 
		  defbrowser );

	swin = gtk_scrolled_window_new( NULL, NULL );
        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_container_add( GTK_CONTAINER( swin ), defbrowser->tree );

        gtk_box_pack_start( GTK_BOX( defbrowser ), swin, TRUE, TRUE, 2 );
	gtk_widget_show_all( swin );
}

GtkType
defbrowser_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Defbrowser",
			sizeof( Defbrowser ),
			sizeof( DefbrowserClass ),
			(GtkClassInitFunc) defbrowser_class_init,
			(GtkObjectInitFunc) defbrowser_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_VOBJECT, &info );
	}

	return( type );
}

void
defbrowser_set_program( Defbrowser *defbrowser, Program *program )
{
	g_assert( !defbrowser->program );

#ifdef DEBUG
	printf( "defbrowser_set_program:\n" );
#endif /*DEBUG*/

	defbrowser->program = program;
}

Defbrowser *
defbrowser_new( void )
{
	Defbrowser *defbrowser = gtk_type_new( TYPE_DEFBROWSER );

	return( defbrowser );
}

/* Find the 'natural' width of the browser.
 */
int 
defbrowser_get_width( Defbrowser *defbrowser )
{
	if( defbrowser->top )
		return( defbrowser->top->requisition.width );
	else
		return( 200 );
}
