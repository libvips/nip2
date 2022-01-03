/* display an image header
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

G_DEFINE_TYPE( Imageheader, imageheader, TYPE_IDIALOG ); 

/* Our columns.
 */
enum {
	NAME_COLUMN,		
	VALUE_COLUMN,	
	N_COLUMNS
};

static void
imageheader_destroy( GtkWidget *widget )
{
	Imageheader *imageheader;

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_IMAGEHEADER( widget ) );

	imageheader = IMAGEHEADER( widget );

	/* My instance destroy stuff.
	 */
	UNREF( imageheader->store );

	GTK_WIDGET_CLASS( imageheader_parent_class )->destroy( widget );
}

static void *
imageheader_add_item( IMAGE *im, 
	const char *field, GValue *value, Imageheader *imageheader )
{
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );
	GtkTreeIter iter;

	/* Show the nicks for enums.
	 */
	if( G_VALUE_HOLDS_ENUM( value ) ) 
		vips_buf_appendf( &buf, "%s", 
			vips_enum_nick( G_VALUE_TYPE( value ), 
				g_value_get_enum( value ) ) );
	else {
		char *value_str;

		value_str = g_strdup_value_contents( value );
		vips_buf_appendf( &buf, "%s", value_str );
		g_free( value_str );
	}

	gtk_list_store_append( imageheader->store, &iter );
	gtk_list_store_set( imageheader->store, &iter,
		NAME_COLUMN, field,
		VALUE_COLUMN, vips_buf_all( &buf ),
		-1 );

	return( NULL );
}

static void
imageheader_refresh( Imageheader *imageheader )
{
	gtk_list_store_clear( imageheader->store );

	if( imageheader->iimage && 
		imageheader->iimage->value.ii ) {
		Imageinfo *ii = imageheader->iimage->value.ii;
		IMAGE *im = imageinfo_get( FALSE, ii );

		im_header_map( im, 
			(im_header_map_fn) imageheader_add_item,
			imageheader );

		gtk_text_buffer_set_text( 
			gtk_text_view_get_buffer( 
				GTK_TEXT_VIEW( imageheader->history ) ),
			im_history_get( im ), -1 );
	}
	else {
		gtk_editable_delete_text( GTK_EDITABLE( imageheader->history ),
			0, -1 );
	}
}

static void
imageheader_entry_changed_cb( GtkEditable *editable, 
	Imageheader *imageheader )
{
	gtk_tree_model_filter_refilter( 
		GTK_TREE_MODEL_FILTER( imageheader->filter ) );
}

static gboolean
imageheader_visible_func( GtkTreeModel *model, GtkTreeIter *iter, 
	gpointer data )
{
	Imageheader *imageheader = IMAGEHEADER( data );
	const char *text = gtk_entry_get_text( 
		GTK_ENTRY( imageheader->entry ) );
	char *name;
	char *value;
	gboolean found;

	found = FALSE;

	gtk_tree_model_get( model, iter, NAME_COLUMN, &name, -1 );
	if( name ) {
		found = my_strcasestr( name, text ) != NULL;
		g_free( name );
	}

	if( found )
		return( TRUE );

	gtk_tree_model_get( model, iter, VALUE_COLUMN, &value, -1 );
	if( value ) {
		found = my_strcasestr( value, text ) != NULL;
		g_free( value );
	}

	return( found );
}

static void
imageheader_build( GtkWidget *widget )
{
	Imageheader *imageheader = IMAGEHEADER( widget );
	iDialog *idlg = IDIALOG( widget );
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	GtkWidget *top;
	GtkWidget *label;
	GtkWidget *swin;
	GtkWidget *pane;
	GtkWidget *vbox;

#ifdef DEBUG
	printf( "imageheader_build: %s\n", IWINDOW( imageheader )->title );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( imageheader_parent_class )->build )
		(*IWINDOW_CLASS( imageheader_parent_class )->build)( widget );

	pane = gtk_paned_new( GTK_ORIENTATION_VERTICAL );
        gtk_box_pack_start( GTK_BOX( idlg->work ), pane, TRUE, TRUE, 2 );

	vbox = gtk_box_new( GTK_ORIENTATION_VERTICAL, 2 );
	gtk_paned_pack1( GTK_PANED( pane ), vbox, TRUE, FALSE );

	top = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 12 );
        gtk_box_pack_start( GTK_BOX( vbox ), top, FALSE, FALSE, 2 );

	imageheader->entry = gtk_entry_new();
        g_signal_connect( imageheader->entry, "changed", 
		G_CALLBACK( imageheader_entry_changed_cb ), imageheader );
	gtk_box_pack_end( GTK_BOX( top ), 
		imageheader->entry, FALSE, FALSE, 2 );

	label = gtk_image_new_from_icon_name( "find", GTK_ICON_SIZE_MENU );
	gtk_box_pack_end( GTK_BOX( top ), label, FALSE, FALSE, 0 );

	swin = gtk_scrolled_window_new( NULL, NULL );
        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        gtk_box_pack_start( GTK_BOX( vbox ), swin, TRUE, TRUE, 2 );

	imageheader->store = gtk_list_store_new( N_COLUMNS, 
		G_TYPE_STRING, 
		G_TYPE_STRING );

	imageheader->filter = gtk_tree_model_filter_new( 
		GTK_TREE_MODEL( imageheader->store ), NULL );
	gtk_tree_model_filter_set_visible_func( 
		GTK_TREE_MODEL_FILTER( imageheader->filter ), 
		imageheader_visible_func, imageheader, NULL );

	imageheader->tree = gtk_tree_view_new_with_model( 
		GTK_TREE_MODEL( imageheader->filter ) );
	gtk_container_add( GTK_CONTAINER( swin ), imageheader->tree );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Field" ),
		   renderer, "text", NAME_COLUMN, NULL );
	gtk_tree_view_column_set_resizable( column, TRUE );
	gtk_tree_view_append_column( GTK_TREE_VIEW( imageheader->tree ), 
		column );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Value" ),
		   renderer, "text", VALUE_COLUMN, NULL );
	gtk_tree_view_column_set_resizable( column, TRUE );
	gtk_tree_view_append_column( GTK_TREE_VIEW( imageheader->tree ), 
		column );

	vbox = gtk_box_new( GTK_ORIENTATION_VERTICAL, 2 );
	gtk_paned_pack2( GTK_PANED( pane ), vbox, TRUE, FALSE );
	label = gtk_label_new( _( "Image history" ) );
        gtk_box_pack_start( GTK_BOX( vbox ), label, FALSE, FALSE, 2 );
	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        gtk_box_pack_end( GTK_BOX( vbox ), swin, TRUE, TRUE, 2 );
	imageheader->history = gtk_text_view_new();
	gtk_text_view_set_editable( GTK_TEXT_VIEW( imageheader->history ), 
		FALSE );
	gtk_text_view_set_cursor_visible( GTK_TEXT_VIEW( imageheader->history ),
		FALSE );
	gtk_container_add( GTK_CONTAINER( swin ), imageheader->history );

	imageheader_refresh( imageheader );

        gtk_window_set_default_size( GTK_WINDOW( imageheader ), 550, 550 );
	gtk_paned_set_position( GTK_PANED( pane ), 350 );

	gtk_widget_show_all( idlg->work );
}

static void
imageheader_class_init( ImageheaderClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	iWindowClass *iwindow_class = (iWindowClass *) class;

	widget_class->destroy = imageheader_destroy;

	iwindow_class->build = imageheader_build;
}

static void
imageheader_init( Imageheader *imageheader )
{
#ifdef DEBUG
	printf( "imageheader_init: %s\n", IWINDOW( imageheader )->title );
#endif /*DEBUG*/

	imageheader->iimage = NULL;
}

/* Conversion has changed signal.
 */
static void
imageheader_ii_changed( Model *model, Imageheader *imageheader )
{
	g_assert( IS_MODEL( model ) );
	g_assert( IS_IMAGEHEADER( imageheader ) );

	imageheader_refresh( imageheader );
}

static void
imageheader_link( Imageheader *imageheader, iImage *iimage )
{
	imageheader->iimage = iimage;

	listen_add( G_OBJECT( imageheader ), (GObject **) &imageheader->iimage,
		"changed", G_CALLBACK( imageheader_ii_changed ) );
}

GtkWidget *
imageheader_new( iImage *iimage )
{
	Imageheader *imageheader = g_object_new( TYPE_IMAGEHEADER, NULL );

	imageheader_link( imageheader, iimage );

	return( GTK_WIDGET( imageheader ) );
}
