/* run the display for an input matrixview in a workspace 
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

/* Round N down to P boundary.
 */
#define ROUND_DOWN(N,P) ((N) - ((N) % P))

/* Round N up to P boundary.
 */
#define ROUND_UP(N,P) (ROUND_DOWN( (N) + (P) - 1, (P) ))

/* The size in cells at which we switch from displaying the whole matrix to
 * displaying part of it in a scrolled window.
 */
static const int matrixview_max_width = 7;
static const int matrixview_max_height = 10;

/* Show a matrix with fixed-width columns.
 */
static const int matrixview_column_width = 70;

/* Limit number of sub-widgets with this ... could be prefs?
 */
static const int matrixview_max_cells = 100;

G_DEFINE_TYPE( Matrixview, matrixview, TYPE_GRAPHICVIEW ); 

static void
matrixview_destroy( GtkWidget *widget )
{
    	Matrixview *matrixview;

    	g_return_if_fail( widget != NULL );
    	g_return_if_fail( IS_MATRIXVIEW( widget ) );

#ifdef DEBUG
    	printf( "matrixview_destroy\n" );
#endif /*DEBUG*/

    	matrixview = MATRIXVIEW( widget );

    	/* My instance destroy stuff.
    	 */
    	IM_FREEF( g_slist_free, matrixview->items );

    	GTK_WIDGET_CLASS( matrixview_parent_class )->destroy( widget );
}

static gboolean
matrixview_scan_text( Matrixview *matrixview, GtkWidget *txt, 
    	double *out, gboolean *changed )
{
    	double v;

    	if( !get_geditable_double( txt, &v ) ) 
    		return( FALSE );

    	if( *out != v ) {
    		*out = v;
    		*changed = TRUE;
    	}

    	return( TRUE );
}

/* Search and read all text widgets and refill matrix. set_dirty this symbol
 * if there was a change. Return non-NULL if we found an error.
 */
static void *
matrixview_scan( View *view )
{
    	Matrixview *matrixview = MATRIXVIEW( view );
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );
	int width = matrix->value.width;
	int height = matrix->value.height;
    	Expr *expr = HEAPMODEL( matrix )->row->expr;

    	gboolean changed;
    	int x, y;
    	GSList *p;

#ifdef DEBUG
    	printf( "matrixview_scan\n" );
#endif /*DEBUG*/

    	/* Should be text widgets there ... either text or tslider.
    	 */
    	if( matrixview->display != MATRIX_DISPLAY_TEXT && 
    		matrixview->display != MATRIX_DISPLAY_TEXT_SCALE_OFFSET && 
    		matrixview->display != MATRIX_DISPLAY_SLIDER )
    		return( NULL );

    	expr_error_clear( expr );
    	changed = FALSE;

    	/* Check for scale and offset, if present.
    	 */
    	if( matrixview->scale && 
		!matrixview_scan_text( matrixview,
			matrixview->scale, &matrix->scale, &changed ) ) {
		expr_error_set( expr );
    		return( view );
	}
    	if( matrixview->offset && 
		!matrixview_scan_text( matrixview, 
			matrixview->offset, &matrix->offset, &changed ) ) {
		expr_error_set( expr );
    		return( view );
    	}

    	/* Loop thru' all matrix widgets. tsliders have text fields we must
	 * scan too.
    	 */
    	if( matrixview->items ) 
    		for( p = matrixview->items, y = 0; y < height; y++ )
    			for( x = 0; x < width; x++, p = p->next ) {
    				GtkWidget *item = GTK_WIDGET( p->data );
    				GtkWidget *entry = TSLIDER( item )->entry;
				int i = x + y * width;

    				if( !matrixview_scan_text( matrixview, entry,
    					&matrix->value.coeff[i], &changed ) ) {
					error_top( _( "Bad value." ) );
					error_sub( _( "Cell (%d, %d):\n%s" ), 
    						x, y, error_get_sub() );
					expr_error_set( expr );

    					return( view );
    				}
    			}

	if( matrixview->store ) {
		GtkTreeModel *tree = GTK_TREE_MODEL( matrixview->store );

		GtkTreeIter iter;

		gtk_tree_model_get_iter_first( tree, &iter );

		for( y = 0; y < height; y++ ) {
			for( x = 0; x < width; x++ ) {
				double *out = 
					&matrix->value.coeff[x + y * width];

				double d;

				gtk_tree_model_get( tree, &iter, x, &d, -1 );

				if( *out != d ) {
					*out = d;
					changed = TRUE;
				}
			}

			gtk_tree_model_iter_next( tree, &iter );
		}
	}

    	if( changed ) 
    		classmodel_update( CLASSMODEL( matrix ) ) ;

    	return( VIEW_CLASS( matrixview_parent_class )->scan( view ) );
}

/* Change to a toggle widget. 
 */
/*ARGSUSED*/
static void
matrixview_toggle_change_cb( GtkWidget *widget, Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );
    	int pos = g_slist_index( matrixview->items, widget );
	int x = pos % matrixview->width;
	int y = pos / matrixview->width;
	int i = x + y * matrix->value.width;

#ifdef DEBUG
    	printf( "matrixview_toggle_change_cb\n" );
#endif /*DEBUG*/

    	/* Cycle value.
    	 */
    	switch( (int) matrix->value.coeff[i] ) {
    	case 0:
    		matrix->value.coeff[i] = 128.0;
    		break;

    	case 255:
    		matrix->value.coeff[i] = 0.0;
    		break;

    	default:
    		matrix->value.coeff[i] = 255.0;
    		break;
    	}

    	classmodel_update( CLASSMODEL( matrix ) );
    	symbol_recalculate_all();
}

/* Build a set of toggle items for a matrix. 
 */
static void
matrixview_toggle_build( Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

    	int x, y;
    	int cx, cy;

    	matrixview->table = gtk_table_new( 
		matrixview->height, matrixview->width, TRUE );
    	gtk_box_pack_start( GTK_BOX( matrixview->box ), 
    		matrixview->table, FALSE, FALSE, 0 );

    	/* Find the centre position, if there is one. We give this a special
    	 * name; it is highlit by our resource file.
    	 */
    	cx = -1; cy = -1;
    	if( matrix->value.height & 0x1 )
    		cy = matrix->value.height >> 1;
    	if( matrix->value.width & 0x1 )
    		cx = matrix->value.width >> 1;

    	/* Build contents.
    	 */
    	for( y = 0; y < matrixview->height; y++ )
    		for( x = 0; x < matrixview->width; x++ ) {
    			GtkWidget *but;

    			but = gtk_button_new_with_label( "0" );
    			g_signal_connect( but, "clicked", 
    				G_CALLBACK( matrixview_toggle_change_cb ),
    				matrixview );
    			if( x == cx && y == cy )
    				gtk_widget_set_name( but, "centre_widget" );
			/*

				FIXME ... this b0rks thanks to pangolayout
				confusion

    			set_fixed( GTK_BIN( but )->child, 1 );
			 */

    			gtk_table_attach( GTK_TABLE( matrixview->table ), but,
    				x, x + 1, y, y + 1, GTK_FILL, GTK_FILL, 2, 2 );
    			matrixview->items = 
    				g_slist_append( matrixview->items, but );
    		}
}

/* Change to a scale in a Tslider. 
 */
/*ARGSUSED*/
static void
matrixview_slider_change_cb( Tslider *tslider, Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );
    	int pos = g_slist_index( matrixview->items, tslider );
	int x = pos % matrixview->width;
	int y = pos / matrixview->width;
	int i = x + y * matrix->value.width;

    	g_assert( pos >= 0 );

    	/* Install value.
    	 */
    	if( matrix->value.coeff[i] != tslider->svalue ) {
    		matrix->value.coeff[i] = tslider->svalue;

    		classmodel_update( CLASSMODEL( matrix ) );
    		symbol_recalculate_all();
    	}
}

/* Build a set of slider items for a matrix. 
 */
static void
matrixview_slider_build( Matrixview *matrixview )
{
    	int x, y;

    	matrixview->table = gtk_table_new( matrixview->height, 
		matrixview->width, TRUE );
    	gtk_box_pack_start( GTK_BOX( matrixview->box ), 
    		matrixview->table, TRUE, TRUE, 0 );

    	for( y = 0; y < matrixview->height; y++ )
    		for( x = 0; x < matrixview->width; x++ ) {
    			Tslider *tslider = tslider_new();

    			tslider_set_conversions( tslider, NULL, NULL );
    			tslider->from = -2;
    			tslider->to = 2;
    			tslider->digits = 3;

    			g_signal_connect_object( tslider, "text_changed",
    				G_CALLBACK( view_changed_cb ), 
    				G_OBJECT( matrixview ) );
    			g_signal_connect_object( tslider, "activate", 
    				G_CALLBACK( view_activate_cb ), 
    				G_OBJECT( matrixview ) );
    			g_signal_connect( tslider, 
    				"slider_changed", 
    				G_CALLBACK( matrixview_slider_change_cb ),
    				matrixview );

    			gtk_container_set_border_width( 
    				GTK_CONTAINER( tslider ), 2 );
    			gtk_table_attach_defaults( 
    				GTK_TABLE( matrixview->table ), 
    				GTK_WIDGET( tslider ),
    				x, x + 1, y, y + 1 );
    			matrixview->items = g_slist_append( matrixview->items, 
    				tslider );
    		}
}

static gboolean
matrixview_text_focus_in( GtkWidget *entry, GdkEvent *event, void *data )
{
    	gtk_editable_select_region( GTK_EDITABLE( entry ), 0, -1 );

	return( FALSE );
}

static gboolean
matrixview_text_focus_out( GtkWidget *entry, GdkEvent *event, void *data )
{
    	gtk_editable_select_region( GTK_EDITABLE( entry ), 0, 0 );

	return( FALSE );
}

static void
matrixview_text_connect( Matrixview *matrixview, GtkWidget *txt )
{
    	g_signal_connect_object( txt, "changed",
    		G_CALLBACK( view_changed_cb ), G_OBJECT( matrixview ) );
    	g_signal_connect_object( txt, "activate",
    		G_CALLBACK( view_activate_cb ), G_OBJECT( matrixview ) );

    	/* Select text on focus-in, deselect on focus out.
    	 */
    	g_signal_connect( txt, "focus_in_event",
    		G_CALLBACK( matrixview_text_focus_in ), NULL );
    	g_signal_connect( txt, "focus_out_event",
    		G_CALLBACK( matrixview_text_focus_out ), NULL );
}

static void
matrixview_text_build_scale_offset( Matrixview *matrixview )
{
	GtkSizeGroup *group;

    	matrixview->cbox = gtk_box_new( GTK_ORIENTATION_VERTICAL, 2 );
        gtk_box_pack_end( GTK_BOX( matrixview->box ), 
    		GTK_WIDGET( matrixview->cbox ), FALSE, FALSE, 0 );

	group = gtk_size_group_new( GTK_SIZE_GROUP_HORIZONTAL );

	matrixview->scale = 
		build_glabeltext4( matrixview->cbox, group, _( "Scale" ) );
	gtk_entry_set_width_chars( GTK_ENTRY( matrixview->scale ), 6 );
    	matrixview_text_connect( matrixview, matrixview->scale );

	matrixview->offset = 
		build_glabeltext4( matrixview->cbox, group, _( "Offset" ) );
	gtk_entry_set_width_chars( GTK_ENTRY( matrixview->offset ), 6 );
    	matrixview_text_connect( matrixview, matrixview->offset );

	UNREF( group );
}

/* Make a GtkListStore from a MatrixValue.
 */
GtkListStore *
matrixview_liststore_new( MatrixValue *matrixvalue )
{
	int width = matrixvalue->width;
	int height = matrixvalue->height;

	GType *types;
	int i, y;
	GtkListStore *store;

	types = g_new( GType, width );
	for( i = 0; i < width; i++ )
		types[i] = G_TYPE_DOUBLE;
	store = gtk_list_store_newv( width, types );
	g_free( types );

	for( y = 0; y < height; y++ ) {
		GtkTreeIter iter;

		gtk_list_store_append( store, &iter );

		for( i = 0; i < width; i++ ) 
			gtk_list_store_set( store, &iter, 
				i, matrixvalue->coeff[y * width + i], -1 );
	}

	return( store );
}

static void
matrixview_edited_cb( GtkCellRendererText *renderer, 
	char *path, char *new_text, void *user_data )
{
	Matrixview *matrixview = MATRIXVIEW( user_data );
	GtkTreeModel *tree = GTK_TREE_MODEL( matrixview->store );
	GtkTreeIter iter;

	if( gtk_tree_model_get_iter_from_string( tree, &iter, path ) ) {
		int col = GPOINTER_TO_INT( g_object_get_data( 
			G_OBJECT( renderer ), "nip2_column_num" ) );

		gtk_list_store_set( GTK_LIST_STORE( tree ), &iter, 
			col, atof( new_text ),
			-1 ); 

		view_scannable_register( VIEW( matrixview ) );
		symbol_recalculate_all();
	}
}

static void
matrixview_cell_data_cb( GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
	GtkTreeModel *tree, GtkTreeIter *iter, void *data )
{
	int col = GPOINTER_TO_INT( g_object_get_data( 
		G_OBJECT( cell ), "nip2_column_num" ) );
	double d;
	char buf[256];

	gtk_tree_model_get( tree, iter, col, &d, -1 );
	vips_snprintf( buf, 256, "%g", d ); 
	g_object_set( cell, "text", buf, NULL );
}

/* Build a set of text items for a matrix. 
 */
static void
matrixview_text_build( Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

	int i;
	GtkTreeViewColumn *column;
    	int cell_height;
	GtkTreeSelection *selection;

	if( !matrix->value.coeff )
		return;

	matrixview->store = matrixview_liststore_new( &matrix->value );
	matrixview->sheet = gtk_tree_view_new_with_model( 
		GTK_TREE_MODEL( matrixview->store ) );
	gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( matrixview->sheet ),
		FALSE );

	for( i = 0; i < matrix->value.width; i++ ) {
		GtkCellRenderer *renderer;
		char buf[256];

		renderer = gtk_cell_renderer_text_new();
		g_object_set( renderer, "editable", TRUE, NULL );
		g_object_set_data( G_OBJECT( renderer ), 
			"nip2_column_num", GINT_TO_POINTER( i ) );
		g_signal_connect( G_OBJECT( renderer ), "edited",
			G_CALLBACK( matrixview_edited_cb ), matrixview );

		column = gtk_tree_view_column_new();
		gtk_tree_view_column_set_sizing( column, 
			GTK_TREE_VIEW_COLUMN_FIXED );
		gtk_tree_view_column_set_fixed_width( column, 
			matrixview_column_width );
		im_snprintf( buf, 256, "%d", i );
		gtk_tree_view_column_set_title( column, buf );
		gtk_tree_view_column_pack_start( column, renderer, FALSE );
		gtk_tree_view_column_set_attributes( column, renderer, 
			"text", i, 
			NULL );
		gtk_tree_view_column_set_cell_data_func( column, renderer, 
			matrixview_cell_data_cb, NULL, NULL ); 
		gtk_tree_view_append_column( GTK_TREE_VIEW( matrixview->sheet ),
			column );
	}

	gtk_tree_view_set_fixed_height_mode( GTK_TREE_VIEW( matrixview->sheet ),
		TRUE );
	gtk_tree_view_column_cell_get_size( column,
		NULL, NULL, NULL, NULL, &cell_height );

	selection = gtk_tree_view_get_selection( 
		GTK_TREE_VIEW( matrixview->sheet ) );
	gtk_tree_selection_set_mode( selection, GTK_SELECTION_MULTIPLE );
	gtk_tree_view_set_rubber_banding( GTK_TREE_VIEW( matrixview->sheet ), 
		TRUE );

	gtk_tree_view_set_grid_lines( GTK_TREE_VIEW( matrixview->sheet ), 
		GTK_TREE_VIEW_GRID_LINES_BOTH );

	if( matrix->value.width > matrixview_max_width || 
		matrix->value.height > matrixview_max_height ) {
		GtkRequisition requisition;
		gint spacing;
		int border;
		int width, height;

		if( matrix->value.width > matrixview_max_width )
			gtk_tree_view_set_headers_visible( 
				GTK_TREE_VIEW( matrixview->sheet ),
				TRUE );

		matrixview->swin = gtk_scrolled_window_new( NULL, NULL );
		gtk_scrolled_window_set_policy( 
			GTK_SCROLLED_WINDOW( matrixview->swin ),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
		gtk_container_add( GTK_CONTAINER( matrixview->swin ), 
			matrixview->sheet );

		/* Calculate how big we should make the scrolled window. We
		 * need to leave space for the scrollbars.
		 */
		gtk_widget_size_request( 
			gtk_scrolled_window_get_hscrollbar( 
				GTK_SCROLLED_WINDOW( matrixview->swin ) ),
			&requisition );
		gtk_widget_style_get( GTK_WIDGET( matrixview->swin ),
			"scrollbar-spacing", &spacing,
			NULL );
		border = requisition.height + spacing;

		/* Subarea of matrix we show, in cells.
		 */
		width = IM_MIN( matrix->value.width, matrixview_max_width );
		height = IM_MIN( matrix->value.height, matrixview_max_height );

		/* Convert to pixels.
		 */
		width *= matrixview_column_width;
		height *= cell_height;

		/* Will we be showing scrollbars? Need to add a bit.
		 */
		if( matrixview->width > matrixview_max_width )
			height += border;
		if( matrixview->height > matrixview_max_height )
			width += border;

		gtk_widget_set_size_request( GTK_WIDGET( matrixview->swin ), 
			width + 5, height + 5 );

		gtk_box_pack_start( GTK_BOX( matrixview->box ), 
			matrixview->swin, FALSE, FALSE, 0 );
	}
	else {
		gtk_box_pack_start( GTK_BOX( matrixview->box ), 
			matrixview->sheet, FALSE, FALSE, 0 );
	}

    	if( matrixview->display == MATRIX_DISPLAY_TEXT_SCALE_OFFSET )
    		/* Make the scale/offset widgets too.
    		 */
    		matrixview_text_build_scale_offset( matrixview );
}

/* Set the label on a toggle button to reflect its value.
 */
static void
matrixview_toggle_set_label( GtkWidget *button, double v )
{
    	GtkWidget *label = GTK_BIN( button )->child;

    	g_return_if_fail( GTK_IS_LABEL( label ) );

    	switch( (int) v ) {
    	case 0:
    		set_glabel( label, "0" );
    		break;

    	case 255:
    		set_glabel( label, "1" );
    		break;

    	default:
    		set_glabel( label, "*" );
    		break;
    	}
}

/* Refresh a set of toggle items for a matrix. 
 */
static void
matrixview_toggle_refresh( Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

    	int x, y;
    	GSList *p;

    	for( p = matrixview->items, y = 0; y < matrixview->height; y++ )
    		for( x = 0; x < matrixview->width; x++, p = p->next ) {
    			GtkWidget *wid = GTK_WIDGET( p->data );
    			int i = x + y * matrix->value.width;

    			matrixview_toggle_set_label( wid, 
				matrix->value.coeff[i] );
    		}
}

/* Refresh a set of slider items for a matrix. 
 */
static void
matrixview_slider_refresh( Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

    	int x, y;
    	GSList *p;

    	for( p = matrixview->items, y = 0; y < matrixview->height; y++ )
    		for( x = 0; x < matrixview->width; x++, p = p->next ) {
    			Tslider *tslider = TSLIDER( p->data );
    			int i = x + y * matrix->value.width;

    			tslider->value = matrix->value.coeff[i];
    			tslider->svalue = matrix->value.coeff[i];

    			tslider_changed( tslider );
    		}
}

static void
matrixview_text_set( Matrixview *matrixview, GtkWidget *txt, double val )
{
    	if( txt ) {
    		gtk_signal_handler_block_by_data( 
    			G_OBJECT( txt ), matrixview );
    		set_gentry( txt, "%g", val ); 
    		gtk_signal_handler_unblock_by_data( 
    			G_OBJECT( txt ), matrixview );
    	}
}

/* Fill the widgets!
 */
static void
matrixview_text_refresh( Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );
	MatrixValue *matrixvalue = &matrix->value;
	int width = matrixvalue->width;
	int height = matrixvalue->height;
	GtkTreeModel *tree = GTK_TREE_MODEL( matrixview->store );

	int x, y;
	GtkTreeIter iter;

	if( !matrixvalue->coeff )
		return;

    	matrixview_text_set( matrixview, matrixview->scale, matrix->scale );
    	matrixview_text_set( matrixview, matrixview->offset, matrix->offset );

	gtk_tree_model_get_iter_first( tree, &iter );

	for( y = 0; y < height; y++ ) {
		for( x = 0; x < width; x++ ) 
			gtk_list_store_set( matrixview->store, &iter, 
				x, matrixvalue->coeff[x + y * width], 
				-1 );

		gtk_tree_model_iter_next( tree, &iter );
	}
}

static void
matrixview_refresh( vObject *vobject )
{
    	Matrixview *matrixview = MATRIXVIEW( vobject );
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

    	gboolean built;
    	gboolean hclip;
    	gboolean vclip;
	int width, height;
	int i;

    	built = FALSE;
    	hclip = FALSE;
    	vclip = FALSE;

	/* Find required size ... limit displays which are tables of widgets 
	 * to avoid huge slowness.
	 */
	width = matrix->value.width;
	height = matrix->value.height;

	if( matrix->display == MATRIX_DISPLAY_TOGGLE ||
		matrix->display == MATRIX_DISPLAY_SLIDER ) {
		if( width * height > matrixview_max_cells ) {
			if( width > height ) {
				width = IM_CLIP( 1, 
					matrixview_max_cells / height, 
					matrix->value.width );
				hclip = TRUE;
			}
			else {
				height = IM_CLIP( 1, 
					matrixview_max_cells / width, 
					matrix->value.height );
				vclip = TRUE;
			}
		}

		/* Clip twice to make sure we clip in both directions if 
		 * necessary.
		 */
		if( width * height > matrixview_max_cells ) {
			if( width > height ) {
				width = IM_CLIP( 1, 
					matrixview_max_cells / height, 
					matrix->value.width );
				hclip = TRUE;
			}
			else {
				height = IM_CLIP( 1, 
					matrixview_max_cells / width, 
					matrix->value.height );
				vclip = TRUE;
			}
		}
	}

#ifdef DEBUG
    	printf( "matrixview_refresh\n" );
#endif /*DEBUG*/

    	/* Is there a UI already there we can reuse? Has to be same size and
    	 * type.
    	 */
    	if( matrixview->display != matrix->display || 
    		matrixview->width != width || 
    		matrixview->height != height ) {
    		/* Kill old UI stuff.
    		 */
    		IM_FREEF( gtk_widget_destroy, matrixview->sheet );
    		IM_FREEF( gtk_widget_destroy, matrixview->table );
    		IM_FREEF( gtk_widget_destroy, matrixview->swin );
    		IM_FREEF( g_slist_free, matrixview->items );
    		IM_FREEF( gtk_widget_destroy, matrixview->cbox );
    		matrixview->scale = NULL;
    		matrixview->offset = NULL;

		/* So the builders know how many widgets to make.
		 */
    		matrixview->width = width;
    		matrixview->height = height;
    		matrixview->display = matrix->display;

    		/* Make new contents. 
    		 */
		switch( matrix->display ) {
		case MATRIX_DISPLAY_TOGGLE:
			matrixview_toggle_build( matrixview );
			break;

		case MATRIX_DISPLAY_SLIDER:
			matrixview_slider_build( matrixview );
			break;

		case MATRIX_DISPLAY_TEXT:
		case MATRIX_DISPLAY_TEXT_SCALE_OFFSET:
			matrixview_text_build( matrixview );
			break;

		default:
			g_assert( FALSE );
		}

		if( hclip ) {
			gtk_table_resize( GTK_TABLE( matrixview->table ), 
				matrixview->height, matrixview->width + 1 );

			for( i = 0; i < matrixview->height; i++ ) {
				GtkWidget *lab;

				lab = gtk_label_new( "---" );
				gtk_table_attach( 
					GTK_TABLE( matrixview->table ), lab,
					matrixview->width, 
					matrixview->width + 1, 
					i, i + 1, 
					GTK_FILL, GTK_FILL, 2, 2 );
			}
		}

		if( vclip ) {
			gtk_table_resize( GTK_TABLE( matrixview->table ), 
				matrixview->height + 1, matrixview->width );

			for( i = 0; i < matrixview->width; i++ ) {
				GtkWidget *lab;

				lab = gtk_label_new( "|" );
				gtk_table_attach( 
					GTK_TABLE( matrixview->table ), lab,
					i, i + 1, 
					matrixview->height, 
					matrixview->height + 1, 
					GTK_FILL, GTK_FILL, 2, 2 );
			}
		}

    		built = TRUE;
    	}

    	switch( matrixview->display ) {
    	case MATRIX_DISPLAY_TOGGLE:
    		matrixview_toggle_refresh( matrixview );
    		break;

    	case MATRIX_DISPLAY_SLIDER:
    		matrixview_slider_refresh( matrixview );
    		break;

    	case MATRIX_DISPLAY_TEXT:
    	case MATRIX_DISPLAY_TEXT_SCALE_OFFSET:
    		matrixview_text_refresh( matrixview );
    		break;

    	default:
    		g_assert( FALSE );
    	}

    	/* If we've built a new display, need to show after _refresh.
    	 */
    	if( built ) {
    		gtk_widget_show_all( GTK_WIDGET( matrixview ) );
    		view_resize( VIEW( matrixview ) );
    	}

    	VOBJECT_CLASS( matrixview_parent_class )->refresh( vobject );
}

static void
matrixview_class_init( MatrixviewClass *class )
{
    	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
    	vObjectClass *vobject_class = (vObjectClass *) class;
    	ViewClass *view_class = (ViewClass *) class;

    	widget_class->destroy = matrixview_destroy;

    	/* Create signals.
    	 */

    	/* Init methods.
    	 */
    	vobject_class->refresh = matrixview_refresh;

    	view_class->scan = matrixview_scan;
}

static void
matrixview_init( Matrixview *matrixview )
{
#ifdef DEBUG
    	printf( "matrixview_init\n" );
#endif /*DEBUG*/

    	matrixview->box = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 12 );
        gtk_box_pack_start( GTK_BOX( matrixview ), 
    		GTK_WIDGET( matrixview->box ), FALSE, FALSE, 0 );

    	/* Build on 1st refresh.
    	 */
    	matrixview->store = NULL;
    	matrixview->sheet = NULL;
    	matrixview->swin = NULL;
    	matrixview->table = NULL;
    	matrixview->items = NULL;
    	matrixview->width = -1;
    	matrixview->height = -1;
    	matrixview->cbox = NULL;
    	matrixview->scale = NULL;
    	matrixview->offset = NULL;
}

View *
matrixview_new( void )
{
    	Matrixview *matrixview = g_object_new( TYPE_MATRIXVIEW, NULL );

    	return( VIEW( matrixview ) );
}

