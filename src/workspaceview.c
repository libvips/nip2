/* a workspaceview button in a workspace
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

/* Define to trace button press events.
#define EVENT
 */

#include "ip.h"

G_DEFINE_TYPE( Workspaceview, workspaceview, TYPE_VIEW ); 

/* Params for "Align Columns" function.
 */
static const int workspaceview_layout_snap_threshold = 30;
static const int workspaceview_layout_hspacing = 10;
static const int workspaceview_layout_vspacing = 10;
static const int workspaceview_layout_left = WORKSPACEVIEW_MARGIN_LEFT;
static const int workspaceview_layout_top = WORKSPACEVIEW_MARGIN_TOP;

static void
workspaceview_scroll_to( Workspaceview *wview, int x, int y )
{
	GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment( 
		GTK_SCROLLED_WINDOW( wview->window ) );
	GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment( 
		GTK_SCROLLED_WINDOW( wview->window ) );
        int nx, ny;

        nx = IM_CLIP( 0, x, wview->width - wview->vp.width );
        ny = IM_CLIP( 0, y, wview->height - wview->vp.height );

	adjustments_set_value( hadj, vadj, nx, ny );
}

/* Scroll by an amount horizontally and vertically.
 */
static void
workspaceview_displace( Workspaceview *wview, int u, int v )
{
	workspaceview_scroll_to( wview, wview->vp.left + u, wview->vp.top + v );
}

/* Scroll to make an xywh area visible. If the area is larger than the
 * viewport, position the view at the bottom left if the xywh area ... 
 * this is usually right for workspaces.
 */
void
workspaceview_scroll( Workspaceview *wview, int x, int y, int w, int h )
{
	GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment( 
		GTK_SCROLLED_WINDOW( wview->window ) );
	GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment( 
		GTK_SCROLLED_WINDOW( wview->window ) );
        VipsRect *vp = &wview->vp;
        int nx, ny;

        nx = gtk_adjustment_get_value( hadj ); 
        if( x + w > VIPS_RECT_RIGHT( vp ) )
                nx = VIPS_MAX( 0, (x + w) - vp->width );
        if( x < nx )
                nx = x;

        ny = gtk_adjustment_get_value( vadj ); 
        if( y + h > VIPS_RECT_BOTTOM( vp ) )
                ny = VIPS_MAX( 0, (y + h) - vp->height );
        if( y < ny )
                ny = y;

#ifdef DEBUG
        printf( "workspaceview_scroll: x=%d, y=%d, w=%d, h=%d, "
                "nx = %d, ny = %d\n", x, y, w, h, nx, ny );
#endif /*DEBUG*/

	adjustments_set_value( hadj, vadj, nx, ny );
}

/* Update our geometry from the fixed widget.
 */
static void
workspaceview_scroll_update( Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment( 
		GTK_SCROLLED_WINDOW( wview->window ) );
	GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment( 
		GTK_SCROLLED_WINDOW( wview->window ) );

        wview->vp.left = hadj->value;
        wview->vp.top = vadj->value;
        wview->vp.width = hadj->page_size;
        wview->vp.height = vadj->page_size;

        wview->width = hadj->upper;
        wview->height = vadj->upper;

	/* Update vp hint in model too.
	 */
	ws->vp = wview->vp;

#ifdef DEBUG
	printf( "workspaceview_scroll_update: %s\n", IOBJECT( ws )->name );
	printf( "  wview->vp: l=%d, t=%d, w=%d, h=%d; fixed w=%d; h=%d\n",
		wview->vp.left, wview->vp.top, 
		wview->vp.width, wview->vp.height,
		wview->width, wview->height );
#endif /*DEBUG*/
}

static void
workspaceview_watch_changed_cb( Watchgroup *watchgroup, Watch *watch, 
	Workspaceview *wview )
{
	/* Names of prefs we watch. These are really rowview preferences, but
	 * we follow them here to prevent every rowview having to have it's
	 * own connection.
	 */
	static char *watch_names[] = {
		"CALC_DISPLAY_LED"
	};

	int i;

	for( i = 0; i < IM_NUMBER( watch_names ); i++ ) 
		if( strcmp( IOBJECT( watch )->name, watch_names[i] ) == 0 ) {
			view_map_all( VIEW( wview ), 
				(view_map_fn) vobject_refresh_queue, NULL );
			break;
		}
}

/* Scroll events ... handle mousewheel shortcuts here. Do this ourselves
 * (rather than just relying on the scrollbars) so we can do shift + wheel == 
 * left/right.
 */
static gboolean
workspaceview_scroll_event_cb( GtkWidget *widget, 
	GdkEventScroll *ev, Workspaceview *wview )
{
	gboolean handled = FALSE;

	/* Gimp uses page_incr / 4 I think, but then scroll speed varies with
	 * window size, which is pretty odd. Just use a constant.
	 */
	const int incr = 50;

	if( ev->direction == GDK_SCROLL_UP || 
		ev->direction == GDK_SCROLL_DOWN ) {
		if( ev->state & GDK_SHIFT_MASK ) {
			if( ev->direction == GDK_SCROLL_UP )
				workspaceview_scroll_to( wview, 
					wview->vp.left + incr, wview->vp.top );
			else
				workspaceview_scroll_to( wview, 
					wview->vp.left - incr, wview->vp.top );

			handled = TRUE;
		}
		else {
			if( ev->direction == GDK_SCROLL_UP )
				workspaceview_scroll_to( wview, 
					wview->vp.left, wview->vp.top - incr );
			else
				workspaceview_scroll_to( wview, 
					wview->vp.left, wview->vp.top + incr );

			handled = TRUE;
		}
	}

	return( handled );
}

static void
workspaceview_realize_cb( GtkWidget *wid, Workspaceview *wview )
{
	g_assert( wid->window );

	gtk_widget_add_events( wid, GDK_BUTTON_PRESS_MASK );
}

void
workspaceview_set_cursor( Workspaceview *wview, iWindowShape shape )
{
	if( !wview->context ) 
		wview->context = iwindow_cursor_context_new( 
			IWINDOW( view_get_toplevel( VIEW( wview ) ) ), 0, 
			"workspaceview" );

	iwindow_cursor_context_set_cursor( wview->context, shape );
}

typedef struct _WorkspaceviewFindColumnview {
	Workspaceview *wview;
	int x;
	int y;
} WorkspaceviewFindColumnview;

static void *
workspaceview_find_columnview_sub( View *view, 
	WorkspaceviewFindColumnview *args )
{
	Columnview *cview = COLUMNVIEW( view );
	Rect col;
	int x, y, w, h;

	columnview_get_position( cview, &x, &y, &w, &h );
	col.left = x;
	col.top = y;
	col.width = w;
	col.height = h;

	if( im_rect_includespoint( &col, args->x, args->y ) )
		return( cview );

	return( NULL );
}

/* Test for a point is workspaceview background ... ie. is not enclosed by one
 * of our columns.
 */
static Columnview *
workspaceview_find_columnview( Workspaceview *wview, int x, int y )
{
	WorkspaceviewFindColumnview args;
	void *res;

	args.wview = wview;
	args.x = x;
	args.y = y;

	res = view_map( VIEW( wview ),
		(view_map_fn) workspaceview_find_columnview_sub, &args, NULL );

	if( res )
		return( COLUMNVIEW( res ) );
	else
		return( NULL );
}

/* Is this event on the workspaceview background.
 */
static gboolean
workspaceview_is_background( Workspaceview *wview, 
	GdkWindow *window, int x, int y )
{
	/* If the event window is not our window, it must have occured in a
	 * sub-GdkWindow (eg. an image thumbnail), so can't be a background
	 * click.
	 */
	if( window != wview->fixed->window )
		return( FALSE );

	/* Could be a click in a non-window widget (eg. a label); search
	 * all columnviews for a hit.
	 */
	return( !workspaceview_find_columnview( wview, x, y ) );
}

static gboolean
workspaceview_fixed_event_cb( GtkWidget *widget, 
	GdkEvent *ev, Workspaceview *wview )
{
	gboolean handled = FALSE;

#ifdef EVENT
	printf( "workspaceview_fixed_event_cb %d\n", ev->type );
#endif /*EVENT*/

        switch( ev->type ) {
        case GDK_BUTTON_PRESS:
                if( ev->button.button == 1 ) {
			Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

			if( workspaceview_is_background( wview, 
				ev->button.window, 
				ev->button.x, ev->button.y ) ) {
				workspace_deselect_all( ws );
				handled = TRUE;
			}
                }
		else if( ev->button.button == 2 ) {
			wview->drag_x = ev->button.x_root + wview->vp.left;
			wview->drag_y = ev->button.y_root + wview->vp.top;
			workspaceview_set_cursor( wview, IWINDOW_SHAPE_MOVE );
			wview->dragging = TRUE;

                        handled = TRUE;
		}

                break;

        case GDK_BUTTON_RELEASE:
                if( ev->button.button == 2 ) {
			workspaceview_set_cursor( wview, IWINDOW_SHAPE_NONE );
			wview->dragging = FALSE;

                        handled = TRUE;
                }

                break;

        case GDK_MOTION_NOTIFY:
                if( wview->dragging && ev->motion.state & GDK_BUTTON2_MASK ) {
			workspaceview_scroll_to( wview, 
				wview->drag_x - ev->motion.x_root,
				wview->drag_y - ev->motion.y_root );

                        handled = TRUE;
                }

                break;

        default:
                break;
        }

        return( handled );
}

static void
workspaceview_scroll_adjustment_cb( GtkAdjustment *adj, Workspaceview *wview )
{
	workspaceview_scroll_update( wview );
}

/* Timer callback for background scroll.
 */
static gboolean
workspaceview_scroll_time_cb( Workspaceview *wview )
{
	/* Perform scroll.
	 */
	workspaceview_scroll_update( wview );
	if( wview->u != 0 || wview->v != 0 ) 
		workspaceview_displace( wview, wview->u, wview->v );

	/* Start timer again.
	 */
	return( TRUE );
}

/* Stop the tally_scroll timer.
 */
static void
workspaceview_scroll_stop( Workspaceview *wview )
{	
        IM_FREEF( g_source_remove, wview->timer );
}

/* Start the tally_scroll timer.
 */
static void
workspaceview_scroll_start( Workspaceview *wview )
{
	workspaceview_scroll_stop( wview );
        wview->timer = g_timeout_add( 30,
                (GSourceFunc) workspaceview_scroll_time_cb, wview );
}

/* Set a background scroll. Pass both zero to stop scroll.
 */
void
workspaceview_scroll_background( Workspaceview *wview, int u, int v )
{
	wview->u = u;
	wview->v = v;

	if( u == 0 && v == 0 )
		workspaceview_scroll_stop( wview );
	else
		workspaceview_scroll_start( wview );
}

static void
workspaceview_destroy( GtkWidget *widget )
{
	Workspaceview *wview;

#ifdef DEBUG
	printf( "workspaceview_destroy: %p\n", widget );
#endif /*DEBUG*/

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_WORKSPACEVIEW( widget ) );

	wview = WORKSPACEVIEW( widget );

	/* Instance destroy.
	 */
	workspaceview_scroll_stop( wview );
	IM_FREEF( iwindow_cursor_context_destroy, wview->context );
	FREESID( wview->watch_changed_sid, main_watchgroup );
	DESTROY_GTK( wview->popup );

	GTK_WIDGET_CLASS( workspaceview_parent_class )->destroy( widget );
}

static void
workspaceview_realize( GtkWidget *widget )
{
#ifdef DEBUG
{
	Workspaceview *wview = WORKSPACEVIEW( widget );
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

	printf( "workspaceview_realize: %s\n", IOBJECT( ws )->name );
}
#endif /*DEBUG*/

	GTK_WIDGET_CLASS( workspaceview_parent_class )->realize( widget );

	/* Mark us as a symbol drag-to widget. 
	 */
	set_symbol_drag_type( widget );
}

static void
workspaceview_drag_data_received( GtkWidget *widget, GdkDragContext *context,
	gint x, gint y, GtkSelectionData *selection_data,
	guint info, guint time ) 
{
	Workspaceview *wview = WORKSPACEVIEW( widget );
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	const char *from_row_path = (const char *) selection_data->data;
	Row *from_row;

#ifdef DEBUG
	printf( "workspaceview_drag_data_received:\n" );
#endif /*DEBUG*/

	/* We seem to rx drag events with x/y relative to the viewport.
	 */
	x += wview->vp.left;
	y += wview->vp.top;

	if( info == TARGET_SYMBOL && selection_data->length > 0 && 
		selection_data->format == 8 &&
		workspaceview_is_background( wview, 
			GTK_WIDGET( wview->fixed )->window, x, y ) &&
		(from_row = row_parse_name( main_workspaceroot->sym, 
			from_row_path )) ) {
		char new_name[MAX_STRSIZE];
		Column *col;
		char vips_buf_text[256];
		VipsBuf buf = VIPS_BUF_STATIC( vips_buf_text );
		Symbol *sym;

		workspace_column_name_new( ws, new_name );
		col = column_new( ws, new_name );

		col->x = x;
		col->y = y;
		workspace_column_select( ws, col );

		/* Qualify relative to us. We don't want to embed
		 * workspace names unless we have to.
		 */
		row_qualified_name_relative( ws->sym, from_row, &buf );

		if( !(sym = workspace_add_def( ws, vips_buf_all( &buf ) )) ) 
			iwindow_alert( widget, GTK_MESSAGE_ERROR );

		symbol_recalculate_all();

		/* Usually the drag-from row will be selected, very
		 * annoying. Select the drag-to row.
		 */
		if( sym && 
			sym->expr && 
			sym->expr->row )
			row_select( sym->expr->row );
	}
}

static void *
workspaceview_child_size_sub( Columnview *cview, Rect *area )
{
	int x, y, w, h;
	Rect col;

	columnview_get_position( cview, &x, &y, &w, &h );

	col.left = x;
	col.top = y;
	col.width = w;
	col.height = h;

	im_rect_unionrect( area, &col, area );

	return( NULL );
}

static void
workspaceview_child_size_cb( Columnview *cview, 
	GtkAllocation *allocation, Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	int right, bottom;

	g_assert( IS_WORKSPACEVIEW( wview ) );

	/* Compute a new bounding box for our children.
	 */
	wview->bounding.left = 0;
	wview->bounding.top = 0;
	wview->bounding.width = 0;
	wview->bounding.height = 0;

	(void) view_map( VIEW( wview ),
		(view_map_fn) workspaceview_child_size_sub, 
		&wview->bounding, NULL );

	wview->bounding.width += 1000;
	wview->bounding.height += 1000;

#ifdef DEBUG
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );

	printf( "workspaceview_child_size_cb: cview %s "
		"bb left=%d, top=%d, width=%d, height=%d\n",
		IOBJECT( col )->name, 
		wview->bounding.left,
		wview->bounding.top,
		wview->bounding.width,
		wview->bounding.height );
}
#endif /*DEBUG*/

	/* Resize our fixed if necessary.
	 */
	right = IM_RECT_RIGHT( &wview->bounding );
	bottom = IM_RECT_BOTTOM( &wview->bounding );
	if( right != wview->width || bottom != wview->height ) {
		gtk_widget_set_size_request( GTK_WIDGET( wview->fixed ), 
			right, bottom ); 

		/* Update the model hints ... it uses bounding to position
		 * loads and saves.
		 */
		ws->area = wview->bounding;
		filemodel_set_offset( FILEMODEL( wsg ), 
			ws->area.left, ws->area.top );
	}
}

/* Pick an xy position for the next column.
 */
static void
workspaceview_pick_xy( Workspaceview *wview, int *x, int *y )
{
	/* Position already set? No change.
	 */
	if( *x >= 0 )
		return;

	/* Set this position.
	 */
	*x = wview->next_x + wview->vp.left;
	*y = wview->next_y + wview->vp.top;

	/* And move on.
	 */
	wview->next_x += 30;
	wview->next_y += 30;
	if( wview->next_x > 300 )
		wview->next_x = 3;
	if( wview->next_y > 200 )
		wview->next_y = 3;
}

static void
workspaceview_link( View *view, Model *model, View *parent )
{
	Workspaceview *wview = WORKSPACEVIEW( view );
	Workspace *ws = WORKSPACE( model );

	VIEW_CLASS( workspaceview_parent_class )->link( view, model, parent );

	vobject_link( VOBJECT( wview->toolkitbrowser ), 
		IOBJECT( ws->kitg ) );
	vobject_link( VOBJECT( wview->workspacedefs ), IOBJECT( ws ) );

	toolkitbrowser_set_workspace( wview->toolkitbrowser, ws ); 

	pane_set_state( wview->rpane, ws->rpane_open, ws->rpane_position ); 
	pane_set_state( wview->lpane, ws->lpane_open, ws->lpane_position ); 
}

static void
workspaceview_child_add( View *parent, View *child )
{
	Columnview *cview = COLUMNVIEW( child );
	Column *column = COLUMN( VOBJECT( cview )->iobject );
	Workspaceview *wview = WORKSPACEVIEW( parent );

	g_signal_connect( child, "size_allocate", 
		G_CALLBACK( workspaceview_child_size_cb ), parent );

	VIEW_CLASS( workspaceview_parent_class )->child_add( parent, child );

	/* Pick start xy pos. 
	 */
        workspaceview_pick_xy( wview, &column->x, &column->y );
	gtk_fixed_put( GTK_FIXED( wview->fixed ),
		GTK_WIDGET( cview ), column->x, column->y );
	cview->lx = column->x;
	cview->ly = column->y;
}

static void
workspaceview_child_position( View *parent, View *child )
{
	Workspaceview *wview = WORKSPACEVIEW( parent );
	Columnview *cview = COLUMNVIEW( child );

	gtk_fixed_move( GTK_FIXED( wview->fixed ),
		GTK_WIDGET( cview ), cview->lx, cview->ly );

	VIEW_CLASS( workspaceview_parent_class )->child_position( parent, child );
}

static void
workspaceview_child_front( View *parent, View *child )
{
	Workspaceview *wview = WORKSPACEVIEW( parent );
	Columnview *cview = COLUMNVIEW( child );

		gtk_widget_ref( GTK_WIDGET( cview ) );
		gtk_container_remove( GTK_CONTAINER( wview->fixed ),
			GTK_WIDGET( cview ) );
		gtk_fixed_put( GTK_FIXED( wview->fixed ),
			GTK_WIDGET( cview ), cview->lx, cview->ly );
		gtk_widget_unref( GTK_WIDGET( cview ) );
}

static void 
workspaceview_refresh( vObject *vobject )
{
	Workspaceview *wview = WORKSPACEVIEW( vobject );
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

#ifdef DEBUG
	printf( "workspaceview_refresh: %p %s\n", ws, IOBJECT( ws )->name );
#endif /*DEBUG*/

	gtk_widget_set_sensitive( GTK_WIDGET( wview ), !ws->locked );

	workspace_jump_update( ws, wview->popup_jump );

	if( ws->rpane_open && !wview->rpane->open )
		pane_animate_open( wview->rpane );
	if( !ws->rpane_open && wview->rpane->open )
		pane_animate_closed( wview->rpane );

	if( ws->lpane_open && !wview->lpane->open )
		pane_animate_open( wview->lpane );
	if( !ws->lpane_open && wview->lpane->open )
		pane_animate_closed( wview->lpane );

	if( wview->label ) {
		gtk_label_set_text( GTK_LABEL( wview->label ),
			IOBJECT( ws )->name );

		if( IOBJECT( ws )->caption )
			set_tooltip( wview->label, 
				"%s", IOBJECT( ws )->caption );

		if( ws->locked ) 
			gtk_image_set_from_stock( GTK_IMAGE( wview->padlock ), 
				STOCK_LOCK, GTK_ICON_SIZE_MENU );
		else
			gtk_image_clear( GTK_IMAGE( wview->padlock ) );  

		if( ws->errors ) 
			gtk_image_set_from_stock( GTK_IMAGE( wview->alert ), 
				STOCK_ALERT, GTK_ICON_SIZE_MENU );
		else
			gtk_image_clear( GTK_IMAGE( wview->alert ) );  

	}

	VOBJECT_CLASS( workspaceview_parent_class )->refresh( vobject );
}

/* What we track during a layout.
 */
typedef struct _WorkspaceLayout {
	/* Context.
	 */
	Workspaceview *wview;

	/* Set of columnviews still to be laid out.
	 */
	GSList *undone_columns;

	/* Track the current set of columns here.
	 */
	GSList *current_columns;

	/* Current position for write.
	 */
	int out_x, out_y;

	/* Accumulate the size of the current set of columns here.
	 */
	Rect area;

	/* Track the current columnview here.
	 */
	Columnview *cview;
} WorkspaceLayout;

static void *
workspaceview_layout_add( View *view, WorkspaceLayout *layout )
{
	layout->undone_columns = 
		g_slist_prepend( layout->undone_columns, view );

	return( NULL );
}

static void *
workspaceview_layout_find_leftmost( Columnview *cview, WorkspaceLayout *layout )
{
	if( GTK_WIDGET( cview )->allocation.x < layout->area.left ) {
		layout->area.left = GTK_WIDGET( cview )->allocation.x;
		layout->cview = cview;
	}

	return( NULL );
}

static void *
workspaceview_layout_find_similar_x( Columnview *cview, 
	WorkspaceLayout *layout )
{
	int x = GTK_WIDGET( cview )->allocation.x;

	gboolean snap;

	snap = FALSE;

	/* Special case: a colum at zero makes a new column on the far left.
	 */
	if( layout->area.left == 0 && 
		x == 0 ) 
		snap = TRUE;

	if( layout->area.left > 0 &&
		ABS( x - layout->area.left ) < 
			workspaceview_layout_snap_threshold ) 
		snap = TRUE;
	
	if( snap ) { 
		layout->current_columns = g_slist_prepend(
			layout->current_columns, cview );
		layout->area.width = IM_MAX( layout->area.width, 
			GTK_WIDGET( cview )->allocation.width ); 
	}

	return( NULL );
}

/* Compare func for row recomp sort.
 */
static int
workspaceview_layout_sort_y( Columnview *a, Columnview *b )
{
	return( GTK_WIDGET( a )->allocation.y - GTK_WIDGET( b )->allocation.y );
}

static void *
workspaceview_layout_set_pos( Columnview *cview, WorkspaceLayout *layout )
{
	Column *column = COLUMN( VOBJECT( cview )->iobject );

	gboolean changed;

	changed = FALSE;

	/* If this column is being dragged, put the xy we allocate into the
	 * shadow instead. 
	 */
	if( cview->shadow ) {
		if( cview->shadow->lx != layout->out_x ||
			cview->shadow->ly != layout->out_y ) {
			cview->shadow->lx = layout->out_x;
			cview->shadow->ly = layout->out_y;
			changed = TRUE;
		}
	}
	else {
		if( column->x != layout->out_x ||
			column->y != layout->out_y ) { 
			column->x = layout->out_x;
			column->y = layout->out_y;
			changed = TRUE;
		}
	}

	layout->out_y += GTK_WIDGET( cview )->allocation.height +
		workspaceview_layout_vspacing;

	if( changed ) 
		iobject_changed( IOBJECT( column ) ); 

	return( NULL );
}

static void *
workspaceview_layout_strike( Columnview *cview, WorkspaceLayout *layout )
{
	layout->undone_columns = g_slist_remove( layout->undone_columns, 
		cview );

	return( NULL );
}

static void
workspaceview_layout_loop( WorkspaceLayout *layout )
{
	layout->cview = NULL;
	layout->area.left = INT_MAX;
	slist_map( layout->undone_columns,
		(SListMapFn) workspaceview_layout_find_leftmost, layout );

	layout->current_columns = NULL;
	layout->area.width = GTK_WIDGET( layout->cview )->allocation.width;
	slist_map( layout->undone_columns,
		(SListMapFn) workspaceview_layout_find_similar_x, layout );

	layout->current_columns = g_slist_sort( layout->current_columns,
		(GCompareFunc) workspaceview_layout_sort_y );

	layout->out_y = workspaceview_layout_top;
	slist_map( layout->current_columns,
		(SListMapFn) workspaceview_layout_set_pos, layout );

	layout->out_x += layout->area.width + workspaceview_layout_hspacing;

	slist_map( layout->current_columns,
		(SListMapFn) workspaceview_layout_strike, layout );

	IM_FREEF( g_slist_free, layout->current_columns );
}

/* Autolayout ... try to rearrange columns so they don't overlap. 

	Strategy:

	search for left-most column

	search for all columns with a 'small' overlap

	lay those columns out vertically with some space between them ... keep
	the vertical ordering we had before

	find the width of the widest, move output over that much

	strike that set of columns from the list of columns to be laid out
 */
static void
workspaceview_layout( View *view )
{
	Workspaceview *wview = WORKSPACEVIEW( view );
	WorkspaceLayout layout;

	layout.wview = wview;
	layout.undone_columns = NULL;
	layout.current_columns = NULL;
	layout.out_x = workspaceview_layout_left;

	view_map( VIEW( wview ),
		(view_map_fn) workspaceview_layout_add, &layout, NULL );

	while( layout.undone_columns )
		workspaceview_layout_loop( &layout );
}

static void
workspaceview_class_init( WorkspaceviewClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	widget_class->destroy = workspaceview_destroy;
	widget_class->realize = workspaceview_realize;
	widget_class->drag_data_received = workspaceview_drag_data_received;

	vobject_class->refresh = workspaceview_refresh;

	view_class->link = workspaceview_link;
	view_class->child_add = workspaceview_child_add;
	view_class->child_position = workspaceview_child_position;
	view_class->child_front = workspaceview_child_front;
	view_class->layout = workspaceview_layout;
}

/* Can't use main_load(), we want to select wses after load.
 */
static gboolean
workspaceview_load( Workspace *ws, const char *filename )
{
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );
	Workspaceroot *wsr = wsg->wsr; 

	Workspacegroup *new_wsg;

	if( (new_wsg = mainw_open_workspace( wsr, filename )) ) 
		return( TRUE );

	error_clear();

	/* workspace_load_file() needs to recalc to work, try to avoid that by
	 * doing .defs first.
	 */
	if( is_file_type( &filesel_dfile_type, filename ) ) {
		if( toolkit_new_from_file( main_toolkitgroup, filename ) )
			return( TRUE );

		error_clear();
	}

	/* Try as matrix or image. Have to do these via definitions.
	 */
	if( workspace_load_file( ws, filename ) ) 
		return( TRUE );

	error_clear();

	error_top( _( "Unknown file type." ) );
	error_sub( _( "Unable to load \"%s\"." ), filename );

	return( FALSE );
}

static void
workspaceview_lpane_changed_cb( Pane *pane, Workspaceview *wview )
{
	Workspace *ws;

	if( (ws = WORKSPACE( VOBJECT( wview )->iobject )) ) 
		if( ws->lpane_open != pane->open ||
			ws->lpane_position != pane->user_position ) {
			ws->lpane_open = pane->open;
			ws->lpane_position = pane->user_position;

			prefs_set( "WORKSPACE_LPANE_OPEN", "%d", 
				ws->lpane_open ); 
			prefs_set( "WORKSPACE_LPANE_POSITION", "%d", 
				ws->lpane_position ); 

			iobject_changed( IOBJECT( ws ) );
		}
}

static void
workspaceview_rpane_changed_cb( Pane *pane, Workspaceview *wview )
{
	Workspace *ws;

	if( (ws = WORKSPACE( VOBJECT( wview )->iobject )) ) 
		if( ws->rpane_open != pane->open ||
			ws->rpane_position != pane->user_position ) {
			ws->rpane_open = pane->open;
			ws->rpane_position = pane->user_position;

			prefs_set( "WORKSPACE_RPANE_OPEN", "%d", 
				ws->rpane_open ); 
			prefs_set( "WORKSPACE_RPANE_POSITION", "%d", 
				ws->rpane_position ); 

			iobject_changed( IOBJECT( ws ) );
		}
}

static gboolean
workspaceview_filedrop( Workspaceview *wview, const char *filename )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

	gboolean result;

	result = workspaceview_load( ws, filename );
	if( result )
		symbol_recalculate_all();

	return( result );
}

static void
workspaceview_column_new_action_cb2( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

	if( !workspace_column_new( ws ) ) 
		iwindow_alert( GTK_WIDGET( wview ), GTK_MESSAGE_ERROR );
}

static void
workspaceview_group_action_cb2( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

	workspace_selected_group( ws );
}

static void
workspaceview_next_error_action_cb2( GtkWidget *wid, GtkWidget *host, 
	Workspaceview *wview )
{
	Workspace *ws = WORKSPACE( VOBJECT( wview )->iobject );

	if( !workspace_next_error( ws ) ) {
		error_top( _( "No errors in tab." ) );
		error_sub( "%s", _( "There are no errors (that I can see) "
			"in this tab." ) );
		iwindow_alert( GTK_WIDGET( wview ), GTK_MESSAGE_INFO );
	}
}

static void
workspaceview_init( Workspaceview *wview )
{
	GtkAdjustment *hadj;
	GtkAdjustment *vadj;
	Panechild *panechild;
	GtkWidget *ebox;

	wview->fixed = NULL;
	wview->window = NULL;

	wview->timer = 0;
	wview->u = 0;
	wview->v = 0;

	wview->dragging = FALSE;
	wview->drag_x = 0;
	wview->drag_y = 0;

	wview->vp.left = 0;
	wview->vp.top = 0;
	wview->vp.width = 0;
	wview->vp.height = 0;
	wview->width = -1;
	wview->height = -1;
	wview->bounding.left = 0;
	wview->bounding.top = 0;
	wview->bounding.width = 0;
	wview->bounding.height = 0;

	wview->next_x = 3;
	wview->next_y = 3;

	wview->context = NULL; 

	wview->watch_changed_sid = g_signal_connect( main_watchgroup, 
		"watch_changed",
		G_CALLBACK( workspaceview_watch_changed_cb ), wview );

	wview->rpane = pane_new( PANE_HIDE_RIGHT );
	g_signal_connect( wview->rpane, "changed",
		G_CALLBACK( workspaceview_rpane_changed_cb ), wview );
	gtk_box_pack_start( GTK_BOX( wview ), 
		GTK_WIDGET( wview->rpane ), TRUE, TRUE, 2 );
	gtk_widget_show( GTK_WIDGET( wview->rpane ) );

	wview->lpane = pane_new( PANE_HIDE_LEFT );
	g_signal_connect( wview->lpane, "changed",
		G_CALLBACK( workspaceview_lpane_changed_cb ), wview );
	gtk_paned_pack1( GTK_PANED( wview->rpane ), 
		GTK_WIDGET( wview->lpane ), TRUE, FALSE );
	gtk_widget_show( GTK_WIDGET( wview->lpane ) );

	/* Ask for our own window so we can spot events on the window 
	 * background.
	 */
	wview->fixed = gtk_fixed_new();
	gtk_widget_add_events( GTK_WIDGET( wview->fixed ), 
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK ); 
	gtk_fixed_set_has_window( GTK_FIXED( wview->fixed ), TRUE );
	wview->window = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wview->window ), 
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_scrolled_window_add_with_viewport( 
		GTK_SCROLLED_WINDOW( wview->window ), wview->fixed );
	gtk_viewport_set_shadow_type( 
		GTK_VIEWPORT( GTK_BIN( wview->window )->child ), 
		GTK_SHADOW_NONE );
	g_signal_connect( wview->window, "scroll_event",
		G_CALLBACK( workspaceview_scroll_event_cb ), wview );
	g_signal_connect( wview->fixed, "realize", 
		G_CALLBACK( workspaceview_realize_cb ), wview );
        g_signal_connect( wview->fixed, "event",
                G_CALLBACK( workspaceview_fixed_event_cb ), wview );
	gtk_widget_add_events( GTK_WIDGET( wview->fixed ), 
		GDK_BUTTON_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
		GDK_BUTTON_RELEASE_MASK ); 

	hadj = gtk_scrolled_window_get_hadjustment( 
		GTK_SCROLLED_WINDOW( wview->window ) );
	vadj = gtk_scrolled_window_get_vadjustment( 
		GTK_SCROLLED_WINDOW( wview->window ) );
	g_signal_connect( hadj, "value_changed", 
		G_CALLBACK( workspaceview_scroll_adjustment_cb ), wview );
	g_signal_connect( hadj, "changed", 
		G_CALLBACK( workspaceview_scroll_adjustment_cb ), wview );
	g_signal_connect( vadj, "value_changed", 
		G_CALLBACK( workspaceview_scroll_adjustment_cb ), wview );
	g_signal_connect( vadj, "changed", 
		G_CALLBACK( workspaceview_scroll_adjustment_cb ), wview );

        /* We can't use gtk_container_set_focus_hadjustment() etc. since our
         * workspace contains a lot of nested structures, and hadjustment()
         * only works for single-layer things. Instead, do focus scrolling
         * ourselves .. see rowview.c.
         */

	gtk_paned_pack2( GTK_PANED( wview->lpane ), 
		GTK_WIDGET( wview->window ), TRUE, FALSE );

	/* Toolkit Browser pane.
	 */
	panechild = panechild_new( wview->rpane, _( "Toolkit Browser" ) );

	/* Have to put toolkitbrowser in an ebox so the search entry gets
	 * clipped to the pane size.
	 */
	ebox = gtk_event_box_new();
	gtk_container_add( GTK_CONTAINER( panechild ), GTK_WIDGET( ebox ) );
	gtk_widget_show( ebox );

	wview->toolkitbrowser = toolkitbrowser_new();
	gtk_container_add( GTK_CONTAINER( ebox ), 
		GTK_WIDGET( wview->toolkitbrowser ) );
	gtk_widget_show( GTK_WIDGET( wview->toolkitbrowser ) );

	/* Workspace-local defs pane.
	 */
	panechild = panechild_new( wview->lpane, _( "Tab Definitions" ) );

	wview->workspacedefs = workspacedefs_new();
	gtk_container_add( GTK_CONTAINER( panechild ), 
		GTK_WIDGET( wview->workspacedefs ) );
	gtk_widget_show( GTK_WIDGET( wview->workspacedefs ) );

	filedrop_register( GTK_WIDGET( wview ),
		(FiledropFunc) workspaceview_filedrop, wview );

	wview->popup = popup_build( _( "Workspace menu" ) );

	popup_add_but( wview->popup, _( "New C_olumn" ),
		POPUP_FUNC( workspaceview_column_new_action_cb2 ) ); 
	wview->popup_jump = popup_add_pullright( wview->popup, 
		_( "Jump to _Column" ) ); 
	menu_add_sep( wview->popup );
	popup_add_but( wview->popup, _( "_Group Selected" ),
		POPUP_FUNC( workspaceview_group_action_cb2 ) ); 
	menu_add_sep( wview->popup );
	popup_add_but( wview->popup, STOCK_NEXT_ERROR,
		POPUP_FUNC( workspaceview_next_error_action_cb2 ) ); 
	popup_attach( wview->fixed, wview->popup, wview );

	gtk_widget_show_all( wview->window );
}

View *
workspaceview_new( void )
{
	Workspaceview *wview = g_object_new( TYPE_WORKSPACEVIEW, NULL );

	return( VIEW( wview ) );
}

void
workspaceview_set_label( Workspaceview *wview, 
	GtkWidget *label, GtkWidget *padlock, GtkWidget *alert )
{
	g_assert( !wview->label );
	g_assert( !wview->padlock );
	g_assert( !wview->alert );

	wview->label = label;
	wview->padlock = padlock;
	wview->alert = alert;
}
