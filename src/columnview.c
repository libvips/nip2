/* a view of a column
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

static ViewClass *parent_class = NULL;

/* The columnview popup menu.
 */
static GtkWidget *columnview_menu = NULL;

/* Edit caption ... right button menu on title bar.
 */
static void
columnview_caption_cb( GtkWidget *wid, GtkWidget *host, Columnview *cview )
{
        /* Edit caption!
         */
        if( cview->state == COL_EDIT )
                return;

        cview->state = COL_EDIT;
        vobject_refresh_queue( VOBJECT( cview ) );
}

/* Select all objects in menu's column.
 */
static void
columnview_select_cb( GtkWidget *wid, GtkWidget *host, Columnview *cview )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	Workspace *ws = col->ws;

	workspace_deselect_all( ws );
        column_select_symbols( col );
}

/* Clone a column.
 */
static void
columnview_clone_cb( GtkWidget *wid, GtkWidget *host, Columnview *cview )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	Workspace *ws = col->ws;

	char new_name[MAX_STRSIZE];
        Column *newcol;

	workspace_column_name_new( ws, new_name );
        newcol = workspace_column_get( ws, new_name );
        iobject_set( IOBJECT( newcol ), NULL, IOBJECT( col )->caption );
        newcol->x = col->x + 100;
        newcol->y = col->y;

	workspace_deselect_all( ws );
        column_select_symbols( col );
	workspace_column_select( ws, newcol );
        if( !workspace_selected_duplicate( ws ) )
		iwindow_alert( GTK_WIDGET( cview ), GTK_MESSAGE_ERROR );
	workspace_deselect_all( ws );

        symbol_recalculate_all();
}

static void
columnview_merge_sub( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Column *col = COLUMN( client );
	Workspace *ws = col->ws;
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	char *filename;
	iWindowResult result;

	result = IWINDOW_YES;
	progress_begin();

	if( (filename = filesel_get_filename( filesel )) ) {
		if( !workspacegroup_merge_rows( wsg, filename ) ) 
			result = IWINDOW_ERROR;

		g_free( filename );
	}

	symbol_recalculate_all();
	progress_end();

	nfn( sys, result );
}

static void
columnview_merge_cb( GtkWidget *wid, GtkWidget *host, Columnview *cview )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	iWindow *iwnd = IWINDOW( view_get_toplevel( VIEW( cview ) ) );
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), 
		_( "Merge Into Column \"%s\"" ), IOBJECT( col )->name );
	filesel_set_flags( FILESEL( filesel ), FALSE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_workspace, 0 ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( iwnd ) );
	idialog_set_iobject( IDIALOG( filesel ), IOBJECT( col ) );
	filesel_set_done( FILESEL( filesel ), columnview_merge_sub, col );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

/* Callback from save browser.
 */
static void
columnview_save_as_sub( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Column *col = COLUMN( client );
	Workspace *ws = col->ws;
	char *filename;

	workspace_deselect_all( ws );
        column_select_symbols( col );

	if( (filename = filesel_get_filename( filesel )) ) {
		if( workspace_selected_save( ws, filename ) ) {
			workspace_deselect_all( ws );
			nfn( sys, IWINDOW_YES );
		}
		else
			nfn( sys, IWINDOW_ERROR );

		g_free( filename );
	}
	else
		nfn( sys, IWINDOW_ERROR );
}

/* Save a column ... can't just do view_save_as_cb(), since we need to save
 * the enclosing workspace too. Hence we have to save_selected on the ws, but
 * only after we have the filename.
 */
static void
columnview_save_as_cb( GtkWidget *wid, GtkWidget *host, Columnview *cview )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	iWindow *iwnd = IWINDOW( view_get_toplevel( VIEW( cview ) ) );
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), 
		_( "Save Column \"%s\"" ), IOBJECT( col )->name );
	filesel_set_flags( FILESEL( filesel ), FALSE, TRUE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_workspace, 0 ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( iwnd ) );
	idialog_set_iobject( IDIALOG( filesel ), IOBJECT( col ) );
	filesel_set_done( FILESEL( filesel ), columnview_save_as_sub, col );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

/* Make a name for a column menu file, based on what we're going to call the
 * menu item.
 */
static void
columnview_filename( char *file, const char *caption )
{
	int i;
	char name[FILENAME_MAX];

	im_strncpy( name, caption, 10 );
	for( i = 0; i < strlen( name ); i++ )
		if( name[i] == ' ' )
			name[i] = '_';

	for( i = 0; ; i++ ) {
		im_snprintf( file, FILENAME_MAX, 
			"$SAVEDIR" G_DIR_SEPARATOR_S "data" G_DIR_SEPARATOR_S 
			"%s-%d.ws", name, i );
		if( !existsf( "%s", file ) )
			break;
	}
}

/* Remember the name of the last toolkit the user asked to add to.
 */
static char *columnview_to_menu_last_toolkit = NULL;

/* Done button hit.
 */
static void
columnview_to_menu_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Column *col = COLUMN( client );
	Workspace *ws = col->ws;
	Stringset *ss = STRINGSET( iwnd );
	StringsetChild *name = stringset_child_get( ss, _( "Name" ) );
	StringsetChild *toolkit = stringset_child_get( ss, _( "Toolkit" ) );
	StringsetChild *file = stringset_child_get( ss, _( "Filename" ) );

	char name_text[1024];
	char toolkit_text[1024];
	char file_text[1024];

	if( !get_geditable_string( name->entry, name_text, 1024 ) ||
		!get_geditable_name( toolkit->entry, toolkit_text, 1024 ) ||
		!get_geditable_filename( file->entry, file_text, 1024 ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	/* Save column to file.
	 */
	workspace_deselect_all( ws );
        column_select_symbols( col );

	if( !workspace_selected_save( ws, file_text ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	workspace_deselect_all( ws );

	if( !tool_new_dia( toolkit_by_name( ws->kitg, toolkit_text ), 
		-1, name_text, file_text ) ) {
		unlinkf( "%s", file_text );
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	IM_SETSTR( columnview_to_menu_last_toolkit, toolkit_text );

	nfn( sys, IWINDOW_YES );
}

/* Make a column into a menu item. 
 */
static void
columnview_to_menu_cb( GtkWidget *wid, GtkWidget *host, Columnview *cview )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	iWindow *iwnd = IWINDOW( view_get_toplevel( VIEW( cview ) ) );
	GtkWidget *ss = stringset_new();
	char *name;
	char *kit_name;
	char filename[FILENAME_MAX];

	if( !(name = IOBJECT( col )->caption) )
		name = "untitled";
	columnview_filename( filename, name );

	if( columnview_to_menu_last_toolkit )
		kit_name = columnview_to_menu_last_toolkit;
	else
		kit_name = "untitled";

	stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), name, _( "Set menu item text here" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Toolkit" ), kit_name, _( "Add to this toolkit" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Filename" ), filename, _( "Store column in this file" ) );

	iwindow_set_title( IWINDOW( ss ), 
		_( "New Menu Item from Column \"%s\"" ), IOBJECT( col )->name );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, col );
	idialog_set_help_tag( IDIALOG( ss ), "sec:diaref" );
	idialog_add_ok( IDIALOG( ss ), columnview_to_menu_done_cb, 
		_( "Menuize" ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( iwnd ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );
}

/* Find the position and size of a columnview.
 */
void
columnview_get_position( Columnview *cview, int *x, int *y, int *w, int *h )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );

        if( GTK_WIDGET( cview )->allocation.x < 2 || 
		GTK_WIDGET( cview )->allocation.y < 2 ) {
                /* Nothing there yet, guess.
                 */
		*x = col->x; 
		*y = col->y;
                *w = 200;
                *h = 50;
        }
        else {
                *x = GTK_WIDGET( cview )->allocation.x;
                *y = GTK_WIDGET( cview )->allocation.y;
                *w = GTK_WIDGET( cview )->allocation.width;
                *h = GTK_WIDGET( cview )->allocation.height;

#ifdef DEBUG
		printf( "columnview_get_position: %s, "
			"x = %d, y = %d, w = %d, h = %d\n",
			IOBJECT( col )->name, *x, *y, *w, *h );
#endif /*DEBUG*/
        }
}

/* Transition functions for mouse stuff on columnviews.
 */
static void
columnview_left_press( Columnview *cview, GdkEvent *ev )
{
	Workspaceview *wview = cview->wview;

        int ix, iy;
        int jx, jy;
        int kx, ky;
        int wx, wy, ww, wh;

#ifdef DEBUG
	printf( "columnview_left_press\n" );
#endif /*DEBUG*/

        /* Find pos of columnview.
         */
        columnview_get_position( cview, &wx, &wy, &ww, &wh );

        /* Position in virtual tally window.
         */
        ix = ev->button.x + wx;
        iy = ev->button.y + wy;

        /* Position in tally viewport.
         */
        jx = ix - wview->vp.left;
        jy = iy - wview->vp.top;

        /* So ... position of top LH corner of tally viewport in root window.
         */
        kx = ev->button.x_root - jx;
        ky = ev->button.y_root - jy;

        switch( cview->state ) {
        case COL_WAIT:
                cview->state = COL_SELECT;

                /* Record offset of mouse in columnview title bar.
                 */
                cview->rx = ev->button.x;
                cview->ry = ev->button.y;

                /* Position of tally window in root window.
                 */
                cview->tx = kx;
                cview->ty = ky;

                /* Start position of mouse in virtual tally window.
                 */
                cview->sx = ix;
                cview->sy = iy;

                break;

        case COL_SELECT:
        case COL_DRAG:
        case COL_EDIT:
                break;

        default:
		g_assert( FALSE );
        }
}

static void
columnview_add_shadow( Columnview *old_cview )
{
	Column *col = COLUMN( VOBJECT( old_cview )->iobject );
	Workspaceview *wview = old_cview->wview;

	if( !old_cview->shadow ) { 
		Columnview *new_cview;

		new_cview = COLUMNVIEW( columnview_new() ); 
		new_cview->wview = wview; 
		VIEW( new_cview )->parent = VIEW( wview ); 
		VOBJECT( new_cview )->iobject = IOBJECT( col ); 

		gtk_fixed_put( GTK_FIXED( wview->fixed ),
			GTK_WIDGET( new_cview ), col->x, col->y );

		gtk_widget_show( GTK_WIDGET( new_cview ) ); 

		old_cview->shadow = new_cview;
		new_cview->master = old_cview;

		/* The shadow will be on top of the real column and hide it. 
		 * Put the real column to the front.
		 */
		model_front( MODEL( col ) ); 
	}
}

static void
columnview_left_motion( Columnview *cview, GdkEvent *ev )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	Workspace *ws = col->ws;
	Workspaceview *wview = cview->wview;

	int u, v;

        /* Posn of pointer in tally viewport.
         */
        int ix = ev->motion.x_root - cview->tx;
        int iy = ev->motion.y_root - cview->ty;

        /* Posn in virtual tally cods.
         */
        int jx = ix + wview->vp.left;
        int jy = iy + wview->vp.top;

        /* Amount of drag since we started.
         */
        int xoff = jx - cview->sx;
        int yoff = jy - cview->sy;

        /* New columnview position.
         */
        int xnew = IM_MAX( 0, jx - cview->rx );
        int ynew = IM_MAX( 0, jy - cview->ry );

#ifdef DEBUG
	printf( "columnview_left_motion\n" );
#endif /*DEBUG*/

        switch( cview->state ) {
        case COL_EDIT:
        case COL_WAIT:
                break;

        case COL_SELECT:
                /* How much drag?
                 */
                if( abs( xoff ) > 5 || abs( yoff ) > 5 ) {
                        cview->state = COL_DRAG;
			workspaceview_set_cursor( wview, IWINDOW_SHAPE_MOVE );
			gtk_grab_add( cview->title ); 

			columnview_add_shadow( cview );
                }

                break;

        case COL_DRAG:
		col->x = xnew;
		col->y = ynew;

		iobject_changed( IOBJECT( col ) );

#ifdef DEBUG
		printf( "drag columnview: x=%d, y=%d", col->x, col->y );
#endif /*DEBUG*/

                /* Set vars for bg scroll.
                 */
		u = 0;
                if( ix > wview->vp.width )
                        u = 10;
                else if( ix < 0 )
                        u = -10;

		v = 0;
                if( iy > wview->vp.height )
                        v = 10;
                else if( iy < 0 )
                        v = -10;

		workspaceview_scroll_background( wview, u, v );

		/* Move other columns about.
		 */
		model_layout( MODEL( ws ) ); 

                break;

        default:
		g_assert( FALSE );
        }
}

static void
columnview_left_release( Columnview *cview, GdkEvent *ev )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	Workspace *ws = col->ws;
	Workspaceview *wview = cview->wview;

#ifdef DEBUG
	printf( "columnview_left_release\n" );
#endif /*DEBUG*/

        /* Back to wait.
         */
        switch( cview->state ) {
        case COL_SELECT:
                cview->state = COL_WAIT;
                workspace_column_select( ws, col );

                break;

        case COL_DRAG:
                cview->state = COL_WAIT;
		workspaceview_scroll_background( wview, 0, 0 );
		workspaceview_set_cursor( wview, IWINDOW_SHAPE_NONE );
		gtk_grab_remove( cview->title ); 
		DESTROY_GTK( cview->shadow ); 

		/* Move columns to their final position.
		 */
		model_layout( MODEL( ws ) ); 
		workspace_set_modified( ws, TRUE );

                break;

        case COL_EDIT:
        case COL_WAIT:
                break;

        default:
                g_assert( FALSE );
        }
}

/* Event in columnview title bar.
 */
static gboolean
columnview_title_event_cb( GtkWidget *widget, GdkEvent *ev, Columnview *cview )
{
	gboolean handled = FALSE;

#ifdef DEBUG
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );

	printf( "columnview_title_event_cb: %s %d\n", 
		IOBJECT( col )->name, 
		ev->type );
}
#endif /*DEBUG*/

        switch( ev->type ) {
        case GDK_BUTTON_PRESS:
                if( ev->button.button == 1 ) {
                        columnview_left_press( cview, ev );
                        handled = TRUE;
                }

                break;

        case GDK_2BUTTON_PRESS:
                if( ev->button.button == 1 ) {
			if( cview->state != COL_EDIT ) {
				cview->state = COL_EDIT;
				vobject_refresh_queue( VOBJECT( cview ) );
			}
                        handled = TRUE;
                }

                break;

        case GDK_MOTION_NOTIFY:
                if( ev->motion.state & GDK_BUTTON1_MASK ) {
                        columnview_left_motion( cview, ev );
                        handled = TRUE;
                }

                break;

        case GDK_BUTTON_RELEASE:
                if( ev->button.button == 1 ) {
                        columnview_left_release( cview, ev );
                        handled = TRUE;
                }

                break;

        default:
                break;
        }

        return( handled );
}

static void 
columnview_destroy( GtkObject *object )
{
	Columnview *cview;
	Column *col;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_COLUMNVIEW( object ) );

	cview = COLUMNVIEW( object );
	col = COLUMN( VOBJECT( cview )->iobject );

#ifdef DEBUG
	printf( "columnview_destroy:\n" );
#endif /*DEBUG*/

	DESTROY_GTK( cview->shadow );

	/* The column has gone .. relayout.
	 */
	if( col &&
		col->ws ) {
		workspace_set_needs_layout( col->ws, TRUE ); 
		mainw_layout();
	}

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
columnview_size_allocate( GtkWidget *widget, GtkAllocation *allocation )
{
	Columnview *cview = COLUMNVIEW( widget ); 

	if( cview->old_width != allocation->width ||
		cview->old_height != allocation->height ) { 
		Column *col = COLUMN( VOBJECT( cview )->iobject );
		Workspace *ws = col->ws;

		cview->old_width = allocation->width;
		cview->old_height = allocation->height;

		workspace_set_needs_layout( ws, TRUE ); 
		mainw_layout();
	}

	GTK_WIDGET_CLASS( parent_class )->size_allocate( widget, allocation );
}

/* Arrow button on title bar.
 */
static void
columnview_updown_cb( GtkWidget *wid, Columnview *cview )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );

	column_set_open( col, !col->open );
}

/* Delete this column from the popup menu.
 */
static void
columnview_destroy_cb( GtkWidget *wid, GtkWidget *host, Columnview *cview )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );

	model_check_destroy( view_get_toplevel( VIEW( cview ) ), 
		MODEL( col ), NULL );
}

/* Delete this column with a click on the 'x' button.
 */
static void
columnview_destroy2_cb( GtkWidget *wid, Columnview *cview )
{
	Column *col = COLUMN( VOBJECT( cview )->iobject );

	model_check_destroy( view_get_toplevel( VIEW( cview ) ), 
		MODEL( col ), NULL );
}

/* Callback for enter in caption edit box.
 */
static void
columnview_caption_enter_cb( GtkWidget *wid, Columnview *cview )
{
        const char *text = gtk_entry_get_text( GTK_ENTRY( cview->capedit ) );
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	Workspace *ws = col->ws;

        cview->state = COL_WAIT;
	iobject_changed( IOBJECT( col ) );

	if( strcmp( text, "" ) != 0 ) 
		iobject_set( IOBJECT( col ), NULL, text );

	workspace_set_modified( ws, TRUE );

	/* The ws view needs to update the jumpto menus.
	 */
	iobject_changed( IOBJECT( ws ) );
}

/* Detect cancel in a caption field.
 */
static gboolean
columnview_caption_cancel_cb( GtkWidget *widget, 
	GdkEvent *ev, Columnview *cview )
{
        if( ev->type != GDK_KEY_PRESS || ev->key.keyval != GDK_Escape )
                return( FALSE );

        /* Turn off edit.
         */
        cview->state = COL_WAIT;
        vobject_refresh_queue( VOBJECT( cview ) );

        return( TRUE );
}

/* Add a caption entry to a columnview if not there.
 */
static void
columnview_add_caption( Columnview *cview )
{
        if( cview->capedit )
                return;

        cview->capedit = gtk_entry_new();
	gtk_entry_set_has_frame( GTK_ENTRY( cview->capedit ), FALSE );
        gtk_box_pack_start( GTK_BOX( cview->titlehb ),
                cview->capedit, FALSE, FALSE, 0 );
        set_tooltip( cview->capedit, _( "Edit caption, press enter to "
                "accept changes, press escape to cancel" ) );

        gtk_signal_connect( GTK_OBJECT( cview->capedit ), "activate",
                GTK_SIGNAL_FUNC( columnview_caption_enter_cb ), cview );
        gtk_signal_connect( GTK_OBJECT( cview->capedit ), "event",
                GTK_SIGNAL_FUNC( columnview_caption_cancel_cb ), cview );
}

/* Callback for enter in new def widget.
 */
static void
columnview_text_enter_cb( GtkWidget *wid, Columnview *cview )
{
        const char *text = gtk_entry_get_text( GTK_ENTRY( cview->text ) );
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	Workspace *ws = col->ws;
        Symbol *sym;

        if( !text || strspn( text, WHITESPACE ) == strlen( text ) )
		return;

	if( !(sym = workspace_add_def_recalc( ws, text )) ) {
		iwindow_alert( wid, GTK_MESSAGE_ERROR );
		symbol_recalculate_all();
		return;
	}

	set_gentry( cview->text, NULL );
}

/* Add bottom entry widget.
 */
static void
columnview_add_text( Columnview *cview )
{
        GtkWidget *inv;

        if( cview->textfr )
                return;

        cview->textfr = gtk_hbox_new( FALSE, 0 );
        gtk_box_pack_end( GTK_BOX( cview->vbox ), 
		cview->textfr, FALSE, FALSE, 0 );
        inv = gtk_label_new( "" );
        gtk_box_pack_start( GTK_BOX( cview->textfr ), inv, FALSE, FALSE, 25 );
        gtk_widget_show( inv );
        cview->text = gtk_entry_new();
        gtk_box_pack_start( GTK_BOX( cview->textfr ), 
		cview->text, TRUE, TRUE, 0 );
        gtk_signal_connect( GTK_OBJECT( cview->text ), "activate",
                GTK_SIGNAL_FUNC( columnview_text_enter_cb ), cview );
        gtk_widget_show( cview->text );
        set_tooltip( cview->text, _( "Enter expressions here" ) );
}

static void 
columnview_refresh( vObject *vobject )
{
	Columnview *cview = COLUMNVIEW( vobject );
	Columnview *shadow = cview->shadow;
	Column *col = COLUMN( VOBJECT( cview )->iobject );
	gboolean editable = col->ws->mode != WORKSPACE_MODE_NOEDIT;

#ifdef DEBUG
	printf( "columnview_refresh: %s\n", IOBJECT( col )->name );
#endif /*DEBUG*/

	/* If this column has a shadow, workspaceview will have put a layout
	 * position into it. See workspaceview_layout_set_pos(). 
	 */
	if( shadow )  
		view_child_position( VIEW( shadow ) ); 

	if( shadow ) {
		gtk_widget_set_size_request( GTK_WIDGET( shadow->frame ), 
			GTK_WIDGET( cview->frame )->allocation.width, 
			GTK_WIDGET( cview->frame )->allocation.height );
		gtk_frame_set_shadow_type( GTK_FRAME( shadow->frame ),
			GTK_SHADOW_IN );
	}


	if( col->x != cview->lx || 
		col->y != cview->ly ) {
#ifdef DEBUG
		printf( "columnview_refresh: move column %s to %d x %d\n",
			IOBJECT( col )->name, col->x, col->y );
#endif /*DEBUG*/

		cview->lx = col->x;
		cview->ly = col->y;
		view_child_position( VIEW( cview ) ); 

		/* Update the save offset hints too.
		 */
		filemodel_set_offset( FILEMODEL( col ), cview->lx, cview->ly );
	}

	/* Turn titlebar on/off.
	 */
	widget_visible( cview->title, editable );
	if( editable ) 
		gtk_frame_set_label( GTK_FRAME( cview->frame ), NULL );
	else if( IOBJECT( col )->caption ) {
		GtkWidget *label;
		char buf[256];
		char buf2[256];

		gtk_frame_set_label( GTK_FRAME( cview->frame ), "x" );
		label = gtk_frame_get_label_widget( GTK_FRAME( cview->frame ) );
		escape_markup( IOBJECT( col )->caption, buf2, 256 );
		im_snprintf( buf, 256, "<b>%s</b>", buf2 );
		gtk_label_set_markup( GTK_LABEL( label ), buf );
		gtk_misc_set_padding( GTK_MISC( label ), 2, 6 );
	}

	/* Update names.
	 */
        set_glabel( cview->lab, "%s - ", IOBJECT( col )->name );
        if( IOBJECT( col )->caption )
		set_glabel( cview->head, "%s", IOBJECT( col )->caption );
	else {
		char buf[256];

		im_snprintf( buf, 256, "<i>%s</i>", 
			_( "doubleclick to set title" ) );
		gtk_label_set_markup( GTK_LABEL( cview->head ), buf );
	}

	/* Set open/closed.
	 */
	if( col->open ) {
                gtk_arrow_set( GTK_ARROW( cview->updown ),
                        GTK_ARROW_DOWN, GTK_SHADOW_OUT );
                set_tooltip( cview->updownb, _( "Fold the column away" ) );
	}
	else {
                gtk_arrow_set( GTK_ARROW( cview->updown ),
                        GTK_ARROW_RIGHT, GTK_SHADOW_OUT );
                set_tooltip( cview->updownb, _( "Open the column" ) );
	}
	model_display( MODEL( col->scol ), col->open );

	/* Closed columns are hidden in NOEDIT mode.
	 */
	widget_visible( GTK_WIDGET( cview ), editable || col->open );

        /* Set caption edit.
         */
        if( cview->state == COL_EDIT ) {
                columnview_add_caption( cview );

                gtk_widget_show( cview->capedit );
                gtk_widget_hide( cview->headfr );

		if( IOBJECT( col )->caption ) {
			set_gentry( cview->capedit, "%s", 
				IOBJECT( col )->caption );
			gtk_editable_select_region( 
				GTK_EDITABLE( cview->capedit ), 0, -1 );
		}
                gtk_widget_grab_focus( cview->capedit );
        }
        else {
                gtk_widget_show( cview->headfr );
                DESTROY_GTK( cview->capedit );
        }

        /* Set bottom entry.
         */
        if( col->selected && 
		col->open && 
		editable &&
		!cview->master ) {
                columnview_add_text( cview );
                gtk_widget_show( cview->textfr );
        }
        else
                DESTROY_GTK( cview->textfr );

	/* Set select state.
	 */
        if( cview->master ) 
                gtk_widget_set_name( cview->title, "shadow_widget" );
	else if( col->selected && !cview->selected ) {
                gtk_widget_set_name( cview->title, "selected_widget" );
		cview->selected = TRUE;
		if( cview->textfr )
			gtk_widget_grab_focus( cview->text );
	}
        else if( !col->selected ) {
		/* Always do this, even if cview->selected, so we set on the
		 * first _refresh().
		 */
                gtk_widget_set_name( cview->title, "column_widget" );
		cview->selected = FALSE;
	}

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
columnview_link( View *view, Model *model, View *parent )
{
	Columnview *cview = COLUMNVIEW( view );
	Workspaceview *wview = WORKSPACEVIEW( parent );

	VIEW_CLASS( parent_class )->link( view, model, parent );

	cview->wview = wview;
}

static void
columnview_child_add( View *parent, View *child )
{
	Columnview *cview = COLUMNVIEW( parent );
	Subcolumnview *sview = SUBCOLUMNVIEW( child );

	VIEW_CLASS( parent_class )->child_add( parent, child );

	gtk_container_add( GTK_CONTAINER( cview->frame ), GTK_WIDGET( sview ) );
}

/* Scroll to keep the text entry at the bottom of the columnview on screen.
 * We can't use the position/size of the text widget for positioning, since it
 * may not be properly realized yet ... make the bottom of the column visible
 * instead.
 */
static void
columnview_scrollto( View *view, ModelScrollPosition position )
{
	Columnview *cview = COLUMNVIEW( view );
	Workspaceview *wview = cview->wview;
	int x, y, w, h;

	columnview_get_position( cview, &x, &y, &w, &h );

	if( position == MODEL_SCROLL_BOTTOM )
		/* 35 is supposed to be enough to ensure the whole of the edit
		 * box gets on the screen.
		 */
		workspaceview_scroll( wview, x, y + h, w, 35 );
	else
		workspaceview_scroll( wview, 
			x, y, w, cview->title->allocation.height );
}

static void
columnview_class_init( ColumnviewClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	GtkWidget *pane;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	object_class->destroy = columnview_destroy;

	widget_class->size_allocate = columnview_size_allocate;

	vobject_class->refresh = columnview_refresh;

	view_class->link = columnview_link;
	view_class->child_add = columnview_child_add;
	view_class->scrollto = columnview_scrollto;

	pane = columnview_menu = popup_build( _( "Column menu" ) );
	popup_add_but( pane, _( "_Edit Caption" ), 
		POPUP_FUNC( columnview_caption_cb ) );
	popup_add_but( pane, _( "Select _All" ), 
		POPUP_FUNC( columnview_select_cb ) );
	popup_add_but( pane, STOCK_DUPLICATE,
		POPUP_FUNC( columnview_clone_cb ) );
	popup_add_but( pane, _( "Merge Into Column" ),
		POPUP_FUNC( columnview_merge_cb ) );
	popup_add_but( pane, GTK_STOCK_SAVE_AS,
		POPUP_FUNC( columnview_save_as_cb ) );
	menu_add_sep( pane );
	popup_add_but( pane, _( "Make Column Into _Menu Item" ),
		POPUP_FUNC( columnview_to_menu_cb ) );
	menu_add_sep( pane );
	popup_add_but( pane, GTK_STOCK_DELETE,
		POPUP_FUNC( columnview_destroy_cb ) );
}

static gboolean
columnview_event_cb( GtkWidget *wid, GdkEvent *ev, Columnview *cview )
{
	gboolean handled;

	handled = FALSE;

        switch( ev->type ) {
        case GDK_BUTTON_PRESS:
                if( ev->button.button == 1 ) 
			/* We want to sop our enclosing notebook seeing
			 * left doubleclicks and creating new tabs. We want to
			 * not block things like scroll events and
			 * middle-drag.
			 */
                        handled = TRUE;

	default:
		break;
	}

	return( handled ); 
}

static void
columnview_init( Columnview *cview )
{
        GtkWidget *sb;
        GtkWidget *frame;
        GtkWidget *icon;
        GtkWidget *but;

        /* No position yet.
         */
        cview->lx = -1;
        cview->ly = -1;

	cview->state = COL_WAIT;
	cview->selected = FALSE;

	cview->old_width = -1;
	cview->old_height = -1;

        /* Make outer vb.
         */
        cview->main = gtk_event_box_new();
	gtk_widget_add_events( GTK_WIDGET( cview->main ), 
		GDK_BUTTON_PRESS_MASK ); 
        cview->vbox = gtk_vbox_new( FALSE, 0 );
        gtk_container_add( GTK_CONTAINER( cview->main ), cview->vbox );

        /* Frame for whole title bar. Need an event_box to catch clicks.
         */
        cview->title = gtk_event_box_new();
	gtk_widget_add_events( GTK_WIDGET( cview->title ), 
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK ); 
        gtk_box_pack_start( GTK_BOX( cview->vbox ), 
		cview->title, FALSE, FALSE, 0 );
        set_tooltip( cview->title, _( "Left-drag to move, left-double-click to "
                "set title, right-click for menu" ) );
        frame = gtk_frame_new( NULL );
        gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_NONE );
        gtk_container_add( GTK_CONTAINER( cview->title ), frame );
        popup_attach( cview->title, columnview_menu, cview );
        gtk_signal_connect( GTK_OBJECT( cview->title ), "event",
                GTK_SIGNAL_FUNC( columnview_title_event_cb ), cview );

        /* Layout contents of title bar.
         */
        cview->titlehb = gtk_hbox_new( FALSE, 0 );
        gtk_container_add( GTK_CONTAINER( frame ), cview->titlehb );

        /* Up/down button.
         */
        cview->updownb = gtk_button_new();
        gtk_button_set_relief( GTK_BUTTON( cview->updownb ), GTK_RELIEF_NONE );
        gtk_container_set_border_width( GTK_CONTAINER( cview->updownb ), 0 );
        gtk_box_pack_start( GTK_BOX( cview->titlehb ),
                cview->updownb, FALSE, FALSE, 0 );
        cview->updown = gtk_arrow_new( GTK_ARROW_DOWN, GTK_SHADOW_OUT );
        gtk_container_add( GTK_CONTAINER( cview->updownb ), cview->updown );
        gtk_signal_connect( GTK_OBJECT( cview->updownb ), "clicked",
                GTK_SIGNAL_FUNC( columnview_updown_cb ), cview );

        /* Remove columnview button.
         */
        sb = gtk_vbox_new( FALSE, 0 );
        gtk_box_pack_end( GTK_BOX( cview->titlehb ), sb, FALSE, FALSE, 1 );
        but = gtk_button_new();
        gtk_button_set_relief( GTK_BUTTON( but ), GTK_RELIEF_NONE );
        gtk_box_pack_start( GTK_BOX( sb ), but, TRUE, FALSE, 0 );
        set_tooltip( but, _( "Delete the column" ) );
	icon = gtk_image_new_from_stock( GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU );
        gtk_container_add( GTK_CONTAINER( but ), icon );
        gtk_signal_connect( GTK_OBJECT( but ), "clicked",
                GTK_SIGNAL_FUNC( columnview_destroy2_cb ), cview );

        /* Columnview name.
         */
        cview->lab = gtk_label_new( "" );
        gtk_box_pack_start( GTK_BOX( cview->titlehb ), 
		cview->lab, FALSE, FALSE, 2 );

        /* Comment. Wrap a frame around it, to make it the same size as
         * an entry widget.
         */
        cview->headfr = gtk_frame_new( NULL );
        gtk_frame_set_shadow_type( GTK_FRAME( cview->headfr ), 
		GTK_SHADOW_NONE );
        gtk_box_pack_start( GTK_BOX( cview->titlehb ),
                cview->headfr, FALSE, FALSE, 0 );
        cview->head = gtk_label_new( "" );
        gtk_container_add( GTK_CONTAINER( cview->headfr ), cview->head );

        /* Make centre table for tally roll.
         */
        cview->frame = gtk_frame_new( NULL );
	gtk_frame_set_shadow_type( GTK_FRAME( cview->frame ),
		GTK_SHADOW_NONE );
        gtk_box_pack_start( GTK_BOX( cview->vbox ), 
		cview->frame, TRUE, TRUE, 0 );

        gtk_box_pack_start( GTK_BOX( cview ), cview->main, FALSE, FALSE, 0 );

	/* We need to stop our enclosing thing seeing doubeclicks and all
	 * that.
	 */
	gtk_signal_connect( GTK_OBJECT( cview ), "event", 
		GTK_SIGNAL_FUNC( columnview_event_cb ), cview );

        gtk_widget_show_all( GTK_WIDGET( cview ) );
}

GtkType
columnview_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Columnview",
			sizeof( Columnview ),
			sizeof( ColumnviewClass ),
			(GtkClassInitFunc) columnview_class_init,
			(GtkObjectInitFunc) columnview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_VIEW, &info );
	}

	return( type );
}

View *
columnview_new( void )
{
	Columnview *cview = gtk_type_new( TYPE_COLUMNVIEW );

	return( VIEW( cview ) );
}

