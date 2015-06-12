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

G_DEFINE_TYPE( Defbrowser, defbrowser, TYPE_VOBJECT ); 

/* Our columns.
 */
enum {
	NAME_COLUMN,			/* Kit or tool name */
	TOOLTIP_COLUMN,	
	TOOL_POINTER_COLUMN,		/* Pointer to tool */
	KIT_POINTER_COLUMN,		/* Pointer to kit (if no tool) */
	N_COLUMNS
};

static void
defbrowser_destroy( GtkWidget *widget )
{
	Defbrowser *defbrowser = DEFBROWSER( widget );

	UNREF( defbrowser->store );

	GTK_WIDGET_CLASS( defbrowser_parent_class )->destroy( widget );
}

static void 
defbrowser_rebuild_item3( Defbrowser *defbrowser, 
	const char *name, const char *tip, 
	Tool *tool, Toolkit *kit )
{
	GtkTreeIter iter;

	gtk_list_store_append( defbrowser->store, &iter );
	gtk_list_store_set( defbrowser->store, &iter,
		NAME_COLUMN, name, 
		TOOLTIP_COLUMN, tip,
		TOOL_POINTER_COLUMN, tool,
		KIT_POINTER_COLUMN, kit,
		-1 );
}

static void *
defbrowser_rebuild_item2( Tool *tool, Defbrowser *defbrowser )
{
	if( tool->toolitem &&
		tool->toolitem->tooltip )
		defbrowser_rebuild_item3( defbrowser, 
			IOBJECT( tool )->name, tool->toolitem->tooltip,
			tool, tool->kit );
	else
		defbrowser_rebuild_item3( defbrowser,
			IOBJECT( tool )->name, tool->help,
			tool, tool->kit );

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

	VOBJECT_CLASS( defbrowser_parent_class )->refresh( vobject );
}

static void
defbrowser_class_init( DefbrowserClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;

	widget_class->destroy = defbrowser_destroy;

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
defbrowser_rebuild_test( Tool *tool, const char *text )
{
	if( tool->toolitem &&
		tool->toolitem->tooltip ) {
		if( my_strcasestr( tool->toolitem->tooltip, text ) )
			return( TRUE );
	}
	if( my_strcasestr( IOBJECT( tool )->name, text ) )
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
	Tool *tool;

	gtk_tree_model_get( model, iter, 
		TOOL_POINTER_COLUMN, &tool, 
		-1 );
	if( !tool )
		return( FALSE );

	return( defbrowser_rebuild_test( tool, text ) );
}

static Tool *
defbrowser_get_selected( Defbrowser *defbrowser )
{
	GtkTreeSelection *selection = gtk_tree_view_get_selection( 
		GTK_TREE_VIEW( defbrowser->tree ) );
	GtkTreeIter iter;
	GtkTreeModel *model;
	Tool *tool; 

        if( gtk_tree_selection_get_selected( selection, &model, &iter ) ) {
		gtk_tree_model_get( model, &iter, 
			TOOL_POINTER_COLUMN, &tool, -1 );

		return( tool );
        }

	return( NULL );
}

static gboolean
defbrowser_activate_selected( Defbrowser *defbrowser )
{
	Tool *tool;

        if( (tool = defbrowser_get_selected( defbrowser )) )
		if( !program_select( defbrowser->program, MODEL( tool ) ) )
			return( FALSE );

	return( TRUE );
}

static void
defbrowser_selection_changed_cb( GtkTreeSelection *select, 
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
	GtkTreeSelection *select;

	defbrowser->top = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 12 );
	defbrowser->entry = gtk_entry_new();
        g_signal_connect( defbrowser->entry, "changed", 
		G_CALLBACK( defbrowser_entry_changed_cb ), 
		defbrowser );
	gtk_box_pack_end( GTK_BOX( defbrowser->top ), 
		defbrowser->entry, FALSE, FALSE, 2 );
	label = gtk_image_new_from_icon_name( GTK_STOCK_FIND, GTK_ICON_SIZE_MENU );
	gtk_box_pack_end( GTK_BOX( defbrowser->top ), 
		label, FALSE, FALSE, 0 );
        gtk_box_pack_start( GTK_BOX( defbrowser ), 
		defbrowser->top, FALSE, FALSE, 2 );
	gtk_widget_show_all( defbrowser->top );

	defbrowser->store = gtk_list_store_new( N_COLUMNS, 
		G_TYPE_STRING, 
		G_TYPE_STRING,
		G_TYPE_POINTER,
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
	gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( defbrowser->tree ), 
		FALSE );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Name" ),
		renderer, "text", NAME_COLUMN, NULL );
	gtk_tree_view_append_column( GTK_TREE_VIEW( defbrowser->tree ), 
		column );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Tooltip" ),
		renderer, "text", TOOLTIP_COLUMN, NULL );
	gtk_tree_view_append_column( GTK_TREE_VIEW( defbrowser->tree ), 
		column );

	select = gtk_tree_view_get_selection( 
		GTK_TREE_VIEW( defbrowser->tree ) );
	gtk_tree_selection_set_mode( select, GTK_SELECTION_SINGLE );
	g_signal_connect( G_OBJECT( select ), "changed",
		G_CALLBACK( defbrowser_selection_changed_cb ), defbrowser );

	swin = gtk_scrolled_window_new( NULL, NULL );
        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_container_add( GTK_CONTAINER( swin ), defbrowser->tree );

        gtk_box_pack_start( GTK_BOX( defbrowser ), swin, TRUE, TRUE, 2 );
	gtk_widget_show_all( swin );
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
	Defbrowser *defbrowser = g_object_new( TYPE_DEFBROWSER, NULL );

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

/* Set the filter string.
 */
void
defbrowser_set_filter( Defbrowser *defbrowser, const char *filter )
{
	gtk_entry_set_text( GTK_ENTRY( defbrowser->entry ), filter );
}
