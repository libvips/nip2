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

/* We have gtksheet patched into the nip sources as a temp measure, so call 
 * directly.
 */
#ifdef USE_GTKSHEET
#include "gtksheet.h"
#endif /*USE_GTKSHEET*/

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

/* Limit non-gtksheet display size with these ... could be prefs?
 */
static const int matrixview_max_cells = 100;

/* A USE_GTKSHEET we can use in expressions.
 */
static const gboolean use_gtksheet = 
#ifdef USE_GTKSHEET
	TRUE;
#else /*!USE_GTKSHEET*/
	FALSE;
#endif /*USE_GTKSHEET*/

static GraphicviewClass *parent_class = NULL;

static void
matrixview_destroy( GtkObject *object )
{
    	Matrixview *matrixview;

    	g_return_if_fail( object != NULL );
    	g_return_if_fail( IS_MATRIXVIEW( object ) );

#ifdef DEBUG
    	printf( "matrixview_destroy\n" );
#endif /*DEBUG*/

    	matrixview = MATRIXVIEW( object );

    	/* My instance destroy stuff.
    	 */
    	IM_FREEF( g_slist_free, matrixview->items );
	IM_FREE( matrixview->cell_text );

    	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

#ifdef USE_GTKSHEET
static gboolean
matrixview_scan_string( char *txt, double *out, gboolean *changed )
{
    	double v;

    	if( sscanf( txt, "%lg", &v ) != 1 ) {
		error_top( _( "Bad floating point number." ) );
		error_sub( _( "\"%s\" is not a floating point number." ), txt );
    		return( FALSE );
    	}

    	if( *out != v ) {
    		*out = v;
    		*changed = TRUE;
    	}

    	return( TRUE );
}
#endif /*USE_GTKSHEET*/

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

    	/* Loop thru' all matrix widgets.
    	 */
    	if( matrixview->items ) 
    		for( p = matrixview->items, y = 0; y < matrixview->height; y++ )
    			for( x = 0; x < matrixview->width; x++, p = p->next ) {
    				GtkWidget *item = GTK_WIDGET( p->data );
				int i = x + y * matrix->value.width;

    				GtkWidget *entry;

    				/* It's either an entry, or a tslider.
    				 */
    				if( IS_TSLIDER( item ) ) 
    					entry = TSLIDER( item )->entry;
    				else
    					entry = item;

    				if( !matrixview_scan_text( matrixview, entry,
    					&matrix->value.coeff[i], &changed ) ) {
					error_top( _( "Bad value." ) );
					error_sub( _( "Cell (%d, %d):\n%s" ), 
    						x, y, error_get_sub() );
					expr_error_set( expr );

    					return( view );
    				}
    			}

#ifdef USE_GTKSHEET
    	if( matrixview->sheet ) {
    		GtkSheet *sheet = GTK_SHEET( matrixview->sheet );

    		for( y = 0; y < matrixview->height; y++ )
    			for( x = 0; x < matrixview->width; x++ ) {
    				char *txt = gtk_sheet_cell_get_text(
    					sheet, y, x );
				int i = x + y * matrix->value.width;

    				if( !matrixview_scan_string( txt,
    					&matrix->value.coeff[i], &changed ) ) {
					error_top( _( "Bad value." ) );
					error_sub( _( "Cell (%d, %d):\n%s" ), 
    						x, y, error_get_sub() );
					expr_error_set( expr );

    					return( view );
    				}
    			}
    	}
#endif /*USE_GTKSHEET*/

    	if( changed ) 
    		classmodel_update( CLASSMODEL( matrix ) ) ;

    	return( VIEW_CLASS( parent_class )->scan( view ) );
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
    			gtk_signal_connect( GTK_OBJECT( but ), "clicked", 
    				GTK_SIGNAL_FUNC( matrixview_toggle_change_cb ),
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

    			gtk_signal_connect_object( GTK_OBJECT( tslider ), 
    				"text_changed",
    				GTK_SIGNAL_FUNC( view_changed_cb ), 
    				GTK_OBJECT( matrixview ) );
    			gtk_signal_connect_object( GTK_OBJECT( tslider ), 
    				"activate", 
    				GTK_SIGNAL_FUNC( view_activate_cb ), 
    				GTK_OBJECT( matrixview ) );
    			gtk_signal_connect( GTK_OBJECT( tslider ), 
    				"slider_changed", 
    				GTK_SIGNAL_FUNC( matrixview_slider_change_cb ),
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

static void
matrixview_text_focus_in( GtkWidget *entry )
{
    	gtk_editable_select_region( GTK_EDITABLE( entry ), 0, -1 );
}

static void
matrixview_text_focus_out( GtkWidget *entry )
{
    	gtk_editable_select_region( GTK_EDITABLE( entry ), 0, 0 );
}

static void
matrixview_text_connect( Matrixview *matrixview, GtkWidget *txt )
{
    	gtk_signal_connect_object( GTK_OBJECT( txt ), "changed",
    		GTK_SIGNAL_FUNC( view_changed_cb ), 
    		GTK_OBJECT( matrixview ) );
    	gtk_signal_connect_object( GTK_OBJECT( txt ), "activate",
    		GTK_SIGNAL_FUNC( view_activate_cb ), 
    		GTK_OBJECT( matrixview ) );

    	/* Select text on focus-in, deselect on focus out.
    	 */
    	gtk_signal_connect( GTK_OBJECT( txt ), "focus_in_event",
    		GTK_SIGNAL_FUNC( matrixview_text_focus_in ), NULL );
    	gtk_signal_connect( GTK_OBJECT( txt ), "focus_out_event",
    		GTK_SIGNAL_FUNC( matrixview_text_focus_out ), NULL );
}

static void
matrixview_text_build_scale_offset( Matrixview *matrixview )
{
	GtkSizeGroup *group;

    	matrixview->cbox = gtk_vbox_new( FALSE, 2 );
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

#ifndef USE_GTKSHEET
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

/* Build a set of text items for a matrix. 
 */
static void
matrixview_text_build( Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

	GtkCellRenderer *renderer;
	int i;
	GtkTreeViewColumn *column;
    	int cell_height;
	GtkTreeSelection *selection;

	matrixview->store = matrixview_liststore_new( &matrix->value );
	matrixview->sheet = gtk_tree_view_new_with_model( 
		GTK_TREE_MODEL( matrixview->store ) );
	gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( matrixview->sheet ),
		FALSE );

	renderer = gtk_cell_renderer_text_new();
	g_object_set( renderer, "editable", TRUE, NULL );

	for( i = 0; i < matrix->value.width; i++ ) {
		char buf[256];

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

		/* If we're showing row/column headers, need an extra
		 * row/column.
		if( matrixview->width > matrixview_max_width )
			height += 1;
		if( matrixview->height > matrixview_max_height )
			width += 1;
		 */

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
#endif /*!USE_GTKSHEET*/

#ifdef USE_GTKSHEET
static int 
matrixview_text_traverse( GtkSheet *sheet,
    	int old_row, int old_col, int *new_row, int *new_col,
    	Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );
    	Row *row = HEAPMODEL( matrix )->row;

    	char *new_text = gtk_sheet_cell_get_text( sheet, *new_row, *new_col );

    	char txt[MAX_LINELENGTH];
    	VipsBuf buf = VIPS_BUF_STATIC( txt );

	/* Make a note of what's in this cell before any editing ... "changed"
	 * does a strcmp() on this to spot edit actions.
	 */
	IM_SETSTR( matrixview->cell_text, new_text );
	matrixview->cell_row = *new_row;
	matrixview->cell_col = *new_col;

    	row_qualified_name( row, &buf );
	/* Expands to (eg) "A2: cell (1,2): 45" ... status line display during
	 * matrix traverse.
	 */
    	vips_buf_appendf( &buf, _( ": cell (%d, %d): %s" ), 
		*new_col, *new_row, new_text );
	workspace_set_status( row->ws, "%s", vips_buf_all( &buf ) );

    	return( TRUE );
}

static void 
matrixview_text_changed( GtkSheet *sheet,
    	int row, int col, Matrixview *matrixview )
{
	/* "changed" is emitted on many changes :-( compare text with start
	 * text to see if we have an edit.
	 */
	if( matrixview->cell_text && 
		!VIEW( matrixview )->scannable &&
		row == matrixview->cell_row && col == matrixview->cell_col ) {
		char *text = gtk_sheet_cell_get_text( sheet, row, col );

		if( strcmp( matrixview->cell_text, text ) != 0 ) {
#ifdef DEBUG
			printf( "matrixview_text_changed: "
				"old = \"%s\", new = \"%s\"\n",
				matrixview->cell_text, text );
#endif /*DEBUG*/

			view_changed_cb( VIEW( matrixview ) );
		}
	}
}

static void 
matrixview_select_range( GtkSheet *sheet, 
	GtkSheetRange *range, Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

	matrix_select( matrix,
		range->col0, range->row0, 
		(range->coli - range->col0) + 1, 
		(range->rowi - range->row0) + 1 );
}

static void 
matrixview_unselect_range( GtkSheet *sheet, 
	GtkSheetRange *range, Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

	matrix_deselect( matrix );
}

static gboolean
matrixview_text_event( GtkWidget *widget, 
	GdkEvent *ev, Matrixview *matrixview )
{
        if( ev->type != GDK_KEY_PRESS || ev->key.keyval != GDK_Return )
                return( FALSE );

	view_activate_cb( VIEW( matrixview ) );

        return( FALSE );
}

static void
matrixview_text_sheet_build( Matrixview *matrixview )
{
	GtkWidget *entry;
	GtkAdjustment *hadj, *vadj;

    	int cell_width;
    	int cell_height;

    	matrixview->sheet = gtk_sheet_new( matrixview->height, 
		matrixview->width, "" );

	/* Cool, but causes too many resizing problems in an embedded widget.
	 */
	gtk_sheet_columns_set_resizable( GTK_SHEET( matrixview->sheet ), 
		FALSE );
	gtk_sheet_rows_set_resizable( GTK_SHEET( matrixview->sheet ), 
		FALSE );

    	cell_width = GTK_SHEET( matrixview->sheet )->row_title_area.width;
    	cell_height = GTK_SHEET( matrixview->sheet )->column_title_area.height;

    	gtk_signal_connect( GTK_OBJECT( matrixview->sheet ), "traverse",
    		GTK_SIGNAL_FUNC( matrixview_text_traverse ), matrixview );
    	gtk_signal_connect( GTK_OBJECT( matrixview->sheet ), "changed",
    		GTK_SIGNAL_FUNC( matrixview_text_changed ), matrixview );

    	gtk_sheet_set_selection_mode( GTK_SHEET( matrixview->sheet ), 
		GTK_SELECTION_MULTIPLE );

    	g_signal_connect( matrixview->sheet, "select-range",
    		G_CALLBACK( matrixview_select_range ), matrixview );
    	g_signal_connect( matrixview->sheet, "unselect-range",
    		G_CALLBACK( matrixview_unselect_range ), matrixview );

	/* We can't connect to "activate" on sheet's entry :-( most
	 * gtk_sheets fail to emit it. Have to parse events ourselves.
	 */
	entry = gtk_sheet_get_entry( GTK_SHEET( matrixview->sheet ) );
	gtk_signal_connect( GTK_OBJECT( entry ), "event",
    		GTK_SIGNAL_FUNC( matrixview_text_event ), 
		GTK_OBJECT( matrixview ) );

	/* For large matricies, display in a scrolled window.
	 */
	if( matrixview->width > matrixview_max_width || 
		matrixview->height > matrixview_max_height ) {
		GtkRequisition requisition;
		gint spacing;
		int border;
		int width, height;

		matrixview->swin = gtk_scrolled_window_new( NULL, NULL );
		gtk_box_pack_start( GTK_BOX( matrixview->box ), 
			matrixview->swin, FALSE, FALSE, 0 );
		gtk_container_add( GTK_CONTAINER( matrixview->swin ), 
			matrixview->sheet );
		hadj = gtk_scrolled_window_get_hadjustment( 
			GTK_SCROLLED_WINDOW( matrixview->swin ) );
		vadj = gtk_scrolled_window_get_vadjustment( 
			GTK_SCROLLED_WINDOW( matrixview->swin ) );

		/* Pick a size for the scrolled window. This is horrible! We
		 * need the inner area of the swin to be a certain size, so we
		 * need to allow extra space for the sccrollbars and the swin
		 * padding.
		 */

		/* There is a thing in swin in 2.8+ to get the scrollbar out, 
		 * but we want to work with earlier gtk as well. Yuk!
		 */
		gtk_widget_size_request( 
			GTK_SCROLLED_WINDOW( matrixview->swin )->hscrollbar, 
			&requisition);
		gtk_widget_style_get( GTK_WIDGET( matrixview->swin ),
			"scrollbar-spacing", &spacing,
			NULL );
		border = requisition.height + spacing;

		/* Subarea of matrix we show, in cells.
		 */
		width = IM_MIN( matrixview->width, matrixview_max_width );
		height = IM_MIN( matrixview->height, matrixview_max_height );

		/* If we're showing row/column headers, need an extra
		 * row/column.
		 */
		if( matrixview->width > matrixview_max_width )
			height += 1;
		if( matrixview->height > matrixview_max_height )
			width += 1;

		/* Convert to pixels.
		 */
		width *= cell_width;
		height *= cell_height;

		/* Will we be showing scrollbars? Need to add a bit.
		 */
		if( matrixview->width > matrixview_max_width )
			height += border;
		if( matrixview->height > matrixview_max_height )
			width += border;

		gtk_widget_set_size_request( GTK_WIDGET( matrixview->swin ), 
			width, height );

		/* If we're not showing scrollbars, we can hide the
		 * row/column headers.
		 */
		if( matrixview->height <= matrixview_max_height )
			gtk_sheet_hide_row_titles( 
				GTK_SHEET( matrixview->sheet ) );
		if( matrixview->width <= matrixview_max_width )
			gtk_sheet_hide_column_titles( 
				GTK_SHEET( matrixview->sheet ) );

		gtk_scrolled_window_set_policy( 
			GTK_SCROLLED_WINDOW( matrixview->swin ),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	}	
	else {
		hadj = GTK_ADJUSTMENT( 
			gtk_adjustment_new( 0, 0, 100, 10, 10, 10 ) );
		vadj = GTK_ADJUSTMENT( 
			gtk_adjustment_new( 0, 0, 100, 10, 10, 10 ) );

		/* No need for col/row titles for small matricies.
		 */
		gtk_sheet_hide_column_titles( GTK_SHEET( matrixview->sheet ) );
		gtk_sheet_hide_row_titles( GTK_SHEET( matrixview->sheet ) );

		gtk_widget_set_size_request( GTK_WIDGET( matrixview->sheet ), 
			matrixview->width * cell_width, 
			matrixview->height * cell_height );

		gtk_box_pack_start( GTK_BOX( matrixview->box ), 
			matrixview->sheet, FALSE, FALSE, 0 );
	}

    	gtk_sheet_set_hadjustment( GTK_SHEET( matrixview->sheet ), hadj );
    	gtk_sheet_set_vadjustment( GTK_SHEET( matrixview->sheet ), vadj );
}

static void
matrixview_text_build( Matrixview *matrixview )
{
	/* Sheet hates zero width/height. Can happen during class construct.
	 */
	if( matrixview->width > 0 && matrixview->height > 0 )
		matrixview_text_sheet_build( matrixview );

    	if( matrixview->display == MATRIX_DISPLAY_TEXT_SCALE_OFFSET )
    		matrixview_text_build_scale_offset( matrixview );
}
#endif /*USE_GTKSHEET*/

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
    			GTK_OBJECT( txt ), matrixview );
    		set_gentry( txt, "%g", val ); 
    		gtk_signal_handler_unblock_by_data( 
    			GTK_OBJECT( txt ), matrixview );
    	}
}

#ifndef USE_GTKSHEET
/* Fill the widgets!
 */
static void
matrixview_text_refresh( Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );

    	matrixview_text_set( matrixview, matrixview->scale, matrix->scale );
    	matrixview_text_set( matrixview, matrixview->offset, matrix->offset );
}
#endif /*!USE_GTKSHEET*/

#ifdef USE_GTKSHEET
static void
matrixview_text_refresh( Matrixview *matrixview )
{
    	Matrix *matrix = MATRIX( VOBJECT( matrixview )->iobject );
	GtkSheetRange *range = &GTK_SHEET( matrixview->sheet )->range;

    	int x, y;

	/* width and height can be zero (during class construct), in which
	 * case they'll be no gtk_sheet.
	 */
	if( !matrixview->sheet )
		return;

	gtk_sheet_freeze( GTK_SHEET( matrixview->sheet ) );

    	for( y = 0; y < matrixview->height; y++ )
    		for( x = 0; x < matrixview->width; x++ ) {
    			int i = x + y * matrix->value.width;
    			double coeff = matrix->value.coeff[i];
    			char buf[256];

    			snprintf( buf, 256, "%g", coeff );
			gtk_signal_handler_block_by_data( 
				GTK_OBJECT( matrixview->sheet ), matrixview );
    			gtk_sheet_set_cell_text( GTK_SHEET( matrixview->sheet ),
                                        y, x, buf );
			gtk_signal_handler_unblock_by_data( 
				GTK_OBJECT( matrixview->sheet ), matrixview );

			if( x == matrixview->cell_col && 
				y == matrixview->cell_row ) 
				IM_SETSTR( matrixview->cell_text, buf );
    		}

	/* Is the model's selection different from ours? Update ours if it is.
	 * Annoyingly, we can only change the selection when the widget's
	 * realized.
	 */
	if( GTK_WIDGET_REALIZED( matrixview->sheet ) ) {
		if( !matrix->selected )
			gtk_sheet_unselect_range( 
				GTK_SHEET( matrixview->sheet ) );
		else if( range->col0 != matrix->range.left ||
			range->coli != IM_RECT_RIGHT( &matrix->range ) - 1 ||
			range->row0 != matrix->range.top ||
			range->rowi != IM_RECT_BOTTOM( &matrix->range ) - 1 ) {
			GtkSheetRange new_range;

			new_range.col0 = matrix->range.left;
			new_range.row0 = matrix->range.top;
			new_range.coli = IM_RECT_RIGHT( &matrix->range );
			new_range.rowi = IM_RECT_BOTTOM( &matrix->range );
			gtk_sheet_select_range( GTK_SHEET( matrixview->sheet ),
				&new_range );
		}
	}

	gtk_sheet_thaw( GTK_SHEET( matrixview->sheet ) );

    	matrixview_text_set( matrixview, matrixview->scale, matrix->scale );
    	matrixview_text_set( matrixview, matrixview->offset, matrix->offset );
}
#endif /*USE_GTKSHEET*/

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
		IM_FREE( matrixview->cell_text );
		matrixview->cell_row = -1;
		matrixview->cell_col = -1;
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

    	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
matrixview_class_init( MatrixviewClass *class )
{
    	GtkObjectClass *object_class = (GtkObjectClass *) class;
    	vObjectClass *vobject_class = (vObjectClass *) class;
    	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

    	object_class->destroy = matrixview_destroy;

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

    	matrixview->box = gtk_hbox_new( FALSE, 12 );
        gtk_box_pack_start( GTK_BOX( matrixview ), 
    		GTK_WIDGET( matrixview->box ), FALSE, FALSE, 0 );

    	/* Build on 1st refresh.
    	 */
    	matrixview->store = NULL;
    	matrixview->sheet = NULL;
    	matrixview->swin = NULL;
    	matrixview->cell_text = NULL;
    	matrixview->cell_row = -1;
    	matrixview->cell_col = -1;
    	matrixview->table = NULL;
    	matrixview->items = NULL;
    	matrixview->width = -1;
    	matrixview->height = -1;
    	matrixview->cbox = NULL;
    	matrixview->scale = NULL;
    	matrixview->offset = NULL;
}

GtkType
matrixview_get_type( void )
{
    	static GtkType matrixview_type = 0;

    	if( !matrixview_type ) {
    		static const GtkTypeInfo info = {
    			"Matrixview",
    			sizeof( Matrixview ),
    			sizeof( MatrixviewClass ),
    			(GtkClassInitFunc) matrixview_class_init,
    			(GtkObjectInitFunc) matrixview_init,
    			/* reserved_1 */ NULL,
    			/* reserved_2 */ NULL,
    			(GtkClassInitFunc) NULL,
    		};

    		matrixview_type = gtk_type_unique( TYPE_GRAPHICVIEW, &info );
    	}

    	return( matrixview_type );
}

View *
matrixview_new( void )
{
    	Matrixview *matrixview = gtk_type_new( TYPE_MATRIXVIEW );

    	return( VIEW( matrixview ) );
}

