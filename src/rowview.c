/* A rowview in a workspace ... not a widget, part of column
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

G_DEFINE_TYPE( Rowview, rowview, TYPE_VIEW ); 

enum {
	ROWVIEW_TARGET_STRING,
};

static GtkTargetEntry rowview_target_table[] = {
	{ "STRING", 0, ROWVIEW_TARGET_STRING },
	{ "text/plain", 0, ROWVIEW_TARGET_STRING }
};

/* Just one popup for all tally buttons.
 */
static GtkWidget *rowview_popup_menu = NULL;

static void 
rowview_destroy( GtkWidget *widget )
{
	Rowview *rview;

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_ROWVIEW( widget ) );

	rview = ROWVIEW( widget );

#ifdef DEBUG
	printf( "rowview_destroy: " );
	row_name_print( ROW( VOBJECT( rview )->iobject ) );
	printf( "\n" );
#endif /*DEBUG*/

	IM_FREE( rview->last_tooltip );

	/* Kill children ... must do this ourselves, since we are not a
	 * self-contained widget.
	 */
	DESTROY_GTK( rview->but );
	DESTROY_GTK( rview->spin );
	DESTROY_GTK( rview->led );

	GTK_WIDGET_CLASS( rowview_parent_class )->destroy( widget );
}

static void
rowview_attach( Rowview *rview, GtkWidget *child, int x, 
	GtkAttachOptions xoptions, GtkAttachOptions yoptions )
{
	Subcolumnview *sview = rview->sview;

	g_object_ref( child );

	if( gtk_widget_get_parent( child ) )
		gtk_container_remove( GTK_CONTAINER( sview->grid ), child );
	gtk_grid_attach( GTK_GRID( sview->grid ), child,
		x, x + 1, rview->rnum, rview->rnum + 1 );  

	g_object_unref( child );
}

static void
rowview_update_widgets( Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );
	int pos = ICONTAINER( row )->pos;
	gboolean editable = row->ws->mode != WORKSPACE_MODE_NOEDIT;

#ifdef DEBUG
	printf( "rowview_refresh: " );
	row_name_print( row );
	printf( "\n" );
	printf( "\teditable == %d\n", editable ); 
#endif /*DEBUG*/

	/* Attach widgets to parent in new place.
	 */
        if( rview->rnum != pos ) {
#ifdef DEBUG
		printf( "rowview_refresh: move from row %d to row %d\n", 
			rview->rnum, pos );
#endif /*DEBUG*/

		rview->rnum = pos;

		rowview_attach( rview, rview->spin, 
			0, GTK_FILL, GTK_FILL );
		rowview_attach( rview, rview->but, 
			1, GTK_FILL, GTK_EXPAND | GTK_FILL );
		rowview_attach( rview, rview->led, 
			2, GTK_FILL, GTK_EXPAND | GTK_FILL );
		if( rview->rhsview )
			rowview_attach( rview, GTK_WIDGET( rview->rhsview ), 
				3, 
				GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL );
	}

        /* Set colours.
         */
	if( CALC_DISPLAY_LED ) {
		char *icon_name;

		icon_name = STOCK_LED_OFF;
		if( row->selected )
			icon_name = STOCK_LED_GREEN;
		else if( row->show == ROW_SHOW_PARENT )
			icon_name = STOCK_LED_CYAN;
		else if( row->show == ROW_SHOW_CHILD )
			icon_name = STOCK_LED_BLUE;
		else if( row->err )
			icon_name = STOCK_LED_RED;
		else if( row->dirty )
			icon_name = STOCK_LED_YELLOW;

		gtk_image_set_from_icon_name( GTK_IMAGE( rview->led ), 
			icon_name, GTK_ICON_SIZE_MENU );
	}
	else {
		gchar *name = "";

		if( row->selected )
			name = "selected_widget";
		else if( row->show == ROW_SHOW_PARENT )
			name = "parent_widget";
		else if( row->show == ROW_SHOW_CHILD )
			name = "child_widget";
		else if( row->err )
			name = "error_widget";
		else if( row->dirty )
			name = "dirty_widget";

		gtk_widget_set_name( rview->but, name );
	}
	widget_visible( rview->led, 
		rview->visible && CALC_DISPLAY_LED && editable );

	/* Update button.
	 */
        set_glabel( rview->label, "%s", row_name( row ) );
	widget_visible( rview->but, rview->visible && editable );

	/* Spin visible only if this is a class. 
	 */
	widget_visible( rview->spin, 
		rview->visible && row->is_class && editable );

	if( rview->rhsview )
		widget_visible( GTK_WIDGET( rview->rhsview ), rview->visible );
}

static void 
rowview_reset( View *view )
{
	Rowview *rview = ROWVIEW( view );

	rowview_update_widgets( rview );

	VIEW_CLASS( rowview_parent_class )->reset( view );
}

static void 
rowview_refresh( vObject *vobject )
{
	Rowview *rview = ROWVIEW( vobject );

	rowview_update_widgets( rview );

	VOBJECT_CLASS( rowview_parent_class )->refresh( vobject );
}

/* Single click on button callback.
 */
static void
rowview_single_cb( GtkWidget *wid, GdkEvent *event, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );

	row_select_modifier( row, event->button.state );
}

/* Edit our object.
 */
static gboolean
rowview_edit( Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );
	Model *graphic = row->child_rhs->graphic;

	if( graphic )
		model_edit( GTK_WIDGET( rview->sview ), graphic );

	return( TRUE );
}

/* Double click on button callback.
 */
static void
rowview_double_cb( GtkWidget *button, GdkEvent *event, Rowview *rview )
{
	if( !rowview_edit( rview ) ) 
		iwindow_alert( button, GTK_MESSAGE_ERROR );
}

/* Edit in menu.
 */
static void
rowview_edit_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	if( !rowview_edit( rview ) ) 
		iwindow_alert( button, GTK_MESSAGE_ERROR );
}

/* Show info.
 */
static gboolean
rowview_header( Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );
	Model *graphic = row->child_rhs->graphic;

	if( graphic )
		model_header( GTK_WIDGET( rview->sview ), graphic );

	return( TRUE );
}

/* Info in menu.
 */
static void
rowview_header_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	if( !rowview_header( rview ) ) 
		iwindow_alert( button, GTK_MESSAGE_ERROR );
}

/* Clone the current item.
 */
static void
rowview_clone_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );
	Workspace *ws = row->top_col->ws;

	/* Only allow clone of top level rows.
	 */
	if( row->top_row != row ) {
		error_top( _( "Can't duplicate." ) );
		error_sub( "%s", 
			_( "You can only duplicate top level rows." ) );
		iwindow_alert( button, GTK_MESSAGE_INFO );
		return;
	}

        workspace_deselect_all( ws );
        row_select( row );
        if( !workspace_selected_duplicate( ws ) )
		iwindow_alert( button, GTK_MESSAGE_ERROR );
        workspace_deselect_all( ws );

        symbol_recalculate_all();
}

/* Ungroup the current item.
 */
static void
rowview_ungroup_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );

        workspace_deselect_all( row->ws );
        row_select( row );
        if( !workspace_selected_ungroup( row->ws ) )
		iwindow_alert( button, GTK_MESSAGE_ERROR );
        symbol_recalculate_all();
}

/* Save the current item.
 */
static void
rowview_save_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	iWindow *iwnd = IWINDOW( view_get_toplevel( VIEW( rview ) ) );
	Row *row = ROW( VOBJECT( rview )->iobject );
	Model *graphic = row->child_rhs->graphic;

	if( graphic )
		classmodel_graphic_save( CLASSMODEL( graphic ),
			GTK_WIDGET( iwnd ) ); 
}

/* Replace the current item.
 */
static void
rowview_replace_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	iWindow *iwnd = IWINDOW( view_get_toplevel( VIEW( rview ) ) );
	Row *row = ROW( VOBJECT( rview )->iobject );
	Model *graphic = row->child_rhs->graphic;

	if( graphic )
		classmodel_graphic_replace( CLASSMODEL( graphic ),
			GTK_WIDGET( iwnd ) ); 
}

/* Recalculate the current item.
 */
static void
rowview_recalc_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );
	Workspace *ws = row->top_col->ws;

	/* Mark dirty from this sym on, and force a recalc even if recalc is
	 * off.
	 */
        workspace_deselect_all( ws );
        row_select( row );
        if( !workspace_selected_recalc( ws ) )
		iwindow_alert( button, GTK_MESSAGE_ERROR );
        workspace_deselect_all( ws );

	/* Now ... pick up any errors.
	 */
	if( row->sym &&
		!symbol_recalculate_check( row->sym ) )
		iwindow_alert( button, GTK_MESSAGE_ERROR );
}

/* Reset the current item.
 */
static void
rowview_clear_edited_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );

	(void) icontainer_map_all( ICONTAINER( row ),
		(icontainer_map_fn) model_clear_edited, NULL );
	symbol_recalculate_all();
}

/* Remove the current item.
 */
static void
rowview_remove_cb( GtkWidget *menu, GtkWidget *button, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );
	Workspace *ws = row->top_col->ws;

	workspace_deselect_all( ws );
	row_select( row );
	workspace_selected_remove_yesno( ws, button );
}

/* Callback for up/down spin button.
 */
static void
rowview_spin_up_cb( GtkWidget *widget, gpointer client )
{
	Rowview *rview = ROWVIEW( client );
	Row *row = ROW( VOBJECT( rview )->iobject );
	Rhs *rhs = row->child_rhs;

	rhs_vislevel_down( rhs );
	workspace_set_modified( row->ws, TRUE );
}

static void
rowview_spin_down_cb( GtkWidget *widget, gpointer client )
{
	Rowview *rview = ROWVIEW( client );
	Row *row = ROW( VOBJECT( rview )->iobject );
	Rhs *rhs = row->child_rhs;

	rhs_vislevel_up( rhs );
	workspace_set_modified( row->ws, TRUE );
}

/* Scroll to make tally entry visible. 
 */
static void
rowview_scrollto( View *view, ModelScrollPosition position )
{
	Rowview *rview = ROWVIEW( view );
	Columnview *cview = view_get_columnview( VIEW( rview ) );
	Workspaceview *wview = cview->wview;

        int x, y, w, h;

        /* Extract position of tally row in RC widget.
         */
        rowview_get_position( rview, &x, &y, &w, &h );
        workspaceview_scroll( wview, x, y, w, h );
}

static void
rowview_drag( Rowview *rview_from, Rowview *rview_to )
{
	Row *row_from = ROW( VOBJECT( rview_from )->iobject );
	Row *row_to = ROW( VOBJECT( rview_to )->iobject );

	if( row_from->top_col != row_to->top_col ) {
		error_top( _( "Not implemented." ) );
		error_sub( _( "Drag between columns not yet implemented." ) );
		iwindow_alert( GTK_WIDGET( rview_from ), GTK_MESSAGE_ERROR );
		return;
	}

	icontainer_child_move( ICONTAINER( row_from ), 
		ICONTAINER( row_to )->pos );

	/* Refresh all the rows, to make sure we move all rows to their new
	 * slots.
	 */
	icontainer_map( ICONTAINER( row_from->scol ),
		(icontainer_map_fn) iobject_changed, NULL, NULL );

        workspace_deselect_all( row_from->ws );
}

static void
rowview_drag_data_get( GtkWidget *but,
	GdkDragContext *context, GtkSelectionData *selection_data,
	guint info, guint time, Rowview *rview )
{
	if( info == ROWVIEW_TARGET_STRING ) {
		/* Send a pointer to us.
 		 */
		gtk_selection_data_set( selection_data,
			gtk_selection_data_get_target( selection_data ),
			8, (const guchar *) &rview, sizeof( Rowview * ) );
	}
}

static void
rowview_drag_data_received( GtkWidget *but,
	GdkDragContext *context, gint x, gint y,
	GtkSelectionData *data, guint info, guint time, Rowview *rview_to )
{
	if( gtk_selection_data_get_length( data ) == sizeof( Rowview * ) && 
		gtk_selection_data_get_format( data ) == 8 && 
		info == ROWVIEW_TARGET_STRING ) {
		Rowview *rview_from = 
			*((Rowview **) gtk_selection_data_get_data( data ));

		if( IS_ROWVIEW( rview_from ) ) {
			rowview_drag( rview_from, rview_to );
			gtk_drag_finish( context, TRUE, FALSE, time );
			return;
		}
	}

	gtk_drag_finish( context, FALSE, FALSE, time );
}

/* Attach the rowview menu to a widget ... also used by iimageview
 */
guint
rowview_menu_attach( Rowview *rview, GtkWidget *widget )
{
	return( popup_attach( widget, rowview_popup_menu, rview ) );
}

static void
rowview_link( View *view, Model *model, View *parent )
{
	Row *row = ROW( model );
	Rowview *rview = ROWVIEW( view );
	Subcolumnview *sview = SUBCOLUMNVIEW( parent );

	VIEW_CLASS( rowview_parent_class )->link( view, model, parent );

	rview->sview = sview;

	/* Only drag n drop top level rows.
	 */
	if( row->top_row == row ) {
		/* FIXME
		gtk_drag_source_set( rview->but, GDK_BUTTON1_MASK, 
			rowview_target_table, IM_NUMBER( rowview_target_table ),
			GDK_ACTION_COPY );
			 */
		g_signal_connect( rview->but, "drag_data_get",
			G_CALLBACK( rowview_drag_data_get ), rview );

		gtk_drag_dest_set( rview->but, GTK_DEST_DEFAULT_ALL,
			rowview_target_table, IM_NUMBER( rowview_target_table ),
			GDK_ACTION_COPY );
		g_signal_connect( rview->but, 
			"drag_data_received",
			G_CALLBACK( rowview_drag_data_received ), rview );
	}

	rowview_menu_attach( rview, rview->but );
}

static void
rowview_child_add( View *parent, View *child )
{
	Rowview *rowview = ROWVIEW( parent );

	g_assert( IS_RHSVIEW( child ) );
	g_assert( !rowview->rhsview );

	rowview->rhsview = RHSVIEW( child );

	VIEW_CLASS( rowview_parent_class )->child_add( parent, child );
}

static void
rowview_child_remove( View *parent, View *child )
{
	Rowview *rowview = ROWVIEW( parent );

	g_assert( IS_RHSVIEW( child ) );
	g_assert( rowview->rhsview );

	rowview->rhsview = NULL;

	VIEW_CLASS( rowview_parent_class )->child_remove( parent, child );
}

static void
rowview_class_init( RowviewClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	GtkWidget *pane;

	/* Create signals.
	 */

	/* Init methods.
	 */
	widget_class->destroy = rowview_destroy;

	vobject_class->refresh = rowview_refresh;

	view_class->link = rowview_link;
	view_class->child_add = rowview_child_add;
	view_class->child_remove = rowview_child_remove;
	view_class->reset = rowview_reset;
	view_class->scrollto = rowview_scrollto;

        /* Other init.
         */
	pane = rowview_popup_menu = popup_build( _( "Row menu" ) );
	popup_add_but( pane, _( "_Edit" ), 
		POPUP_FUNC( rowview_edit_cb ) );
	popup_add_but( pane, _( "_Header" ), 
		POPUP_FUNC( rowview_header_cb ) );
	popup_add_but( pane, "duplicate",
		POPUP_FUNC( rowview_clone_cb ) );
	popup_add_but( pane, _( "U_ngroup" ), 
		POPUP_FUNC( rowview_ungroup_cb ) );
	popup_add_but( pane, "save-as",
		POPUP_FUNC( rowview_save_cb ) );
	popup_add_but( pane, _( "Replace From _File" ), 
		POPUP_FUNC( rowview_replace_cb ) );
	popup_add_but( pane, _( "_Recalculate" ), 
		POPUP_FUNC( rowview_recalc_cb ) );
	popup_add_but( pane, _( "Re_set" ), 
		POPUP_FUNC( rowview_clear_edited_cb ) );
	menu_add_sep( pane );
	popup_add_but( pane, "delete",
		POPUP_FUNC( rowview_remove_cb ) );
}

static void
rowview_enter_cb( GtkWidget *widget, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );

	row_set_status( row );
	row_show_dependents( row );
}

static void
rowview_leave_cb( GtkWidget *widget, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );

	row_hide_dependents( row );
}

static gboolean
rowview_focus_cb( GtkWidget *widget, GtkDirectionType dir, Rowview *rview )
{
        view_scrollto( VIEW( rview ), MODEL_SCROLL_TOP );

        return( FALSE );
}

static void
rowview_tooltip_generate( GtkWidget *widget, VipsBuf *buf, Rowview *rview )
{
	Row *row = ROW( VOBJECT( rview )->iobject );

	iobject_info( IOBJECT( row ), buf );
	vips_buf_removec( buf, '\n' );
}

static void
rowview_init( Rowview *rview )
{
        rview->visible = TRUE; 
	rview->rnum = -1;
        rview->last_tooltip = NULL; 

	/* Make leds.
	 */
	rview->led = gtk_image_new_from_icon_name( STOCK_LED_OFF, 
		GTK_ICON_SIZE_MENU );

        /* Make fold/unfold button.
         */
	rview->spin = spin_new();
        g_signal_connect( rview->spin, "up_click",
                G_CALLBACK( rowview_spin_up_cb ), rview );
        g_signal_connect( rview->spin, "down_click",
                G_CALLBACK( rowview_spin_down_cb ), rview );
        gtk_widget_show( rview->spin );
	set_tooltip( rview->spin, _( "Click to open or close class" ) );

        /* Make name button.
         */
        rview->but = gtk_button_new();
        gtk_widget_show( rview->but );
        doubleclick_add( rview->but, FALSE,
                DOUBLECLICK_FUNC( rowview_single_cb ), rview, 
		DOUBLECLICK_FUNC( rowview_double_cb ), rview );
        rview->label = gtk_label_new( "" );
        gtk_container_add( GTK_CONTAINER( rview->but ), rview->label );
        gtk_widget_show( rview->label );
        g_signal_connect( rview->but, "enter",
                G_CALLBACK( rowview_enter_cb ), rview );
        g_signal_connect( rview->but, "leave",
                G_CALLBACK( rowview_leave_cb ), rview );
        g_signal_connect( rview->but, "focus",
                G_CALLBACK( rowview_focus_cb ), rview );
	set_tooltip_generate( rview->but, 
		(TooltipGenerateFn) rowview_tooltip_generate, rview, NULL );
}

View *
rowview_new( void )
{
	Rowview *rview = g_object_new( TYPE_ROWVIEW, NULL );

	return( VIEW( rview ) );
}

/* Find the position and size of a row in the enclosing GtkFixed.
 */
void
rowview_get_position( Rowview *rview, int *x, int *y, int *w, int *h )
{
        Columnview *cview = view_get_columnview( VIEW( rview ) );

	GtkAllocation allocation;

        if( gtk_widget_get_visible( rview->spin ) ) {
		gtk_widget_get_allocation( rview->spin, &allocation ); 
                *x = allocation.x;
                *y = allocation.y;
                *w = allocation.width;
                *h = allocation.height;
        }
        else {
		gtk_widget_get_allocation( rview->but, &allocation ); 
                *x = allocation.x;
                *y = allocation.y;
                *w = 0;
                *h = 0;
        }

	gtk_widget_get_allocation( rview->but, &allocation ); 
        *w += allocation.width;
        *h = VIPS_MAX( allocation.height, *h );

        if( gtk_widget_get_visible( rview->led ) ) {
		gtk_widget_get_allocation( rview->led, &allocation ); 
                *w += allocation.width;
                *h = VIPS_MAX( allocation.height, *h );
        }

	gtk_widget_get_allocation( GTK_WIDGET( rview->rhsview ), &allocation ); 
        *w += allocation.width;
        *h = VIPS_MAX( allocation.height, *h );

        /* Title bar, plus separator.
         */
	gtk_widget_get_allocation( cview->title, &allocation ); 
        *y += allocation.height + 2;

	gtk_widget_get_allocation( cview->main, &allocation ); 
        *x += allocation.x;
        *y += allocation.y;

#ifdef DEBUG
        printf( "rowview_get_position: " );
        row_name_print( ROW( VOBJECT( rview )->iobject ) );
        printf( ": x = %d, y = %d, w = %d, h = %d\n", *x, *y, *w, *h );
#endif /*DEBUG*/
}

/* Hide/show a row. 
 */
void 
rowview_set_visible( Rowview *rview, gboolean visible )
{
	if( rview->visible != visible ) {
		rview->visible = visible;
		rowview_update_widgets( rview );
	}
}

gboolean 
rowview_get_visible( Rowview *rview )
{
	return( rview->visible );
}
