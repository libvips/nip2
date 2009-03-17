/* make and manage base windows ... dialog (messagebox, file box), top
 * level windows
 */

/*

    Copyright (C) 1991-2001, The National Gallery

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
  
    build interface:

	iwnd = iwindow_new( type );
	iwindow_set_*( iwnd, ... );
	iwindow_build( iwnd );

    destroy interface:

    	iwindow_kill()

		'cancellable' kill ... user popdown can return IWINDOW_ERROR
		or IWINDOW_NO to prevent popdown

	gtk_widget_destroy()

		non-cancellable ... popdown is not called

	so ... don't free() in popdown, subclass iwnd and free() in _destroy()

 */

/*
#define DEBUG
 */

#include "ip.h"

/* Cursor bitmaps.
 */
#include "BITMAPS/dropper_src.xbm"
#include "BITMAPS/dropper_msk.xbm"
#include "BITMAPS/magin_src.xbm"
#include "BITMAPS/magout_src.xbm"
#include "BITMAPS/mag_msk.xbm"
#include "BITMAPS/watch_1.xbm"
#include "BITMAPS/watch_2.xbm"
#include "BITMAPS/watch_3.xbm"
#include "BITMAPS/watch_4.xbm"
#include "BITMAPS/watch_5.xbm"
#include "BITMAPS/watch_6.xbm"
#include "BITMAPS/watch_7.xbm"
#include "BITMAPS/watch_8.xbm"
#include "BITMAPS/watch_msk.xbm"

static GtkWindowClass *parent_class = NULL;

/* List of all iwindows.
 */
static GSList *iwindow_all = NULL;

/* All our cursors.
 */
static GdkCursor *iwindow_cursor[IWINDOW_SHAPE_LAST] = { NULL };

#ifdef DEBUG
/* Human-readable names for cursor shapes.
 */
static const char *iwindow_cursor_name[] = {
	"IWINDOW_SHAPE_DROPPER",
	"IWINDOW_SHAPE_PEN",
	"IWINDOW_SHAPE_SMUDGE",
	"IWINDOW_SHAPE_SMEAR",
	"IWINDOW_SHAPE_TEXT",
	"IWINDOW_SHAPE_RECT",
	"IWINDOW_SHAPE_FLOOD",
	"IWINDOW_SHAPE_MOVE",
	"IWINDOW_SHAPE_EDIT",
	"IWINDOW_SHAPE_MAGIN",	
	"IWINDOW_SHAPE_MAGOUT",	
	"IWINDOW_SHAPE_TOP",
	"IWINDOW_SHAPE_BOTTOM",
	"IWINDOW_SHAPE_LEFT",
	"IWINDOW_SHAPE_RIGHT",
	"IWINDOW_SHAPE_TOPRIGHT",
	"IWINDOW_SHAPE_TOPLEFT",
	"IWINDOW_SHAPE_BOTTOMRIGHT",
	"IWINDOW_SHAPE_BOTTOMLEFT",
	"IWINDOW_SHAPE_HGLASS1",
	"IWINDOW_SHAPE_HGLASS2",
	"IWINDOW_SHAPE_HGLASS3",
	"IWINDOW_SHAPE_HGLASS4",
	"IWINDOW_SHAPE_HGLASS5",
	"IWINDOW_SHAPE_HGLASS6",
	"IWINDOW_SHAPE_HGLASS7",
	"IWINDOW_SHAPE_HGLASS8",
	"IWINDOW_SHAPE_NONE"
};
#endif /*DEBUG*/

int
iwindow_number( void )
{
	return( g_slist_length( iwindow_all ) );
}

/* Over all windows.
 */
void *
iwindow_map_all( iWindowMapFn fn, void *a )
{
	return( slist_map( iwindow_all, (SListMapFn) fn, a ) );
}

/* Make a custom cursor ... source, mask, width, height and hot spot position.
 */
static GdkCursor *
iwindow_make_cursor_data( guchar *src_bits, guchar *msk_bits, 
	int w, int h, int x, int y )
{
	GdkPixmap *src;
	GdkPixmap *msk;
	GdkCursor *cursor;
	GdkColor fg = { 0, 255 << 8, 255 << 8, 255 << 8 };
	GdkColor bg = { 0, 0, 0, 0 };

	src = gdk_bitmap_create_from_data( NULL, 
		(const char *) src_bits, w, h );
	msk = gdk_bitmap_create_from_data( NULL, 
		(const char *) msk_bits, w, h );
	cursor = gdk_cursor_new_from_pixmap( src, msk, &fg, &bg, x, y );
	gdk_pixmap_unref( src );
	gdk_pixmap_unref( msk );

	return( cursor );
}

/* Build all the cursors.
 */
static void
iwindow_make_cursors( void )
{
	/* Init standard cursors with this table.
	 */
	static GdkCursorType standards[] = {
		GDK_CURSOR_IS_PIXMAP,	/* IWINDOW_SHAPE_DROPPER */
		GDK_PENCIL,		/* IWINDOW_SHAPE_PEN */
		GDK_HAND2,		/* IWINDOW_SHAPE_SMUDGE */
		GDK_SPIDER,		/* IWINDOW_SHAPE_SMEAR */
		GDK_GOBBLER,		/* IWINDOW_SHAPE_TEXT */
		GDK_SIZING,		/* IWINDOW_SHAPE_RECT */
		GDK_TREK,		/* IWINDOW_SHAPE_FLOOD */
		GDK_FLEUR,		/* IWINDOW_SHAPE_MOVE */
		GDK_CROSSHAIR,		/* IWINDOW_SHAPE_EDIT */
		GDK_CURSOR_IS_PIXMAP, 	/* IWINDOW_SHAPE_MAGIN */
		GDK_CURSOR_IS_PIXMAP, 	/* IWINDOW_SHAPE_MAGOUT */
		GDK_TOP_SIDE,		/* IWINDOW_SHAPE_TOP */
		GDK_BOTTOM_SIDE,	/* IWINDOW_SHAPE_BOTTOM */
		GDK_LEFT_SIDE,		/* IWINDOW_SHAPE_LEFT */
		GDK_RIGHT_SIDE,		/* IWINDOW_SHAPE_RIGHT */
		GDK_TOP_RIGHT_CORNER,	/* IWINDOW_SHAPE_TOPRIGHT */
		GDK_TOP_LEFT_CORNER,	/* IWINDOW_SHAPE_TOPLEFT */
		GDK_BOTTOM_RIGHT_CORNER,/* IWINDOW_SHAPE_BOTTOMRIGHT, */
		GDK_BOTTOM_LEFT_CORNER,	/* IWINDOW_SHAPE_BOTTOMLEFT */
	};

	/* All the bits for the rotating cursor.
	 */
	static guchar *watch_bits[] = {
		watch_1_bits,
		watch_2_bits,
		watch_3_bits,
		watch_4_bits,
		watch_5_bits,
		watch_6_bits,
		watch_7_bits,
		watch_8_bits,
	};

	int i;

	if( iwindow_cursor[0] )
		return;

	/* Easy ones first.
	 */
	for( i = 0; i < IM_NUMBER( standards ); i++ )
		if( standards[i] != GDK_CURSOR_IS_PIXMAP )
			iwindow_cursor[i] = gdk_cursor_new( standards[i] );

	/* Custom cursors.
	 */
	iwindow_cursor[IWINDOW_SHAPE_DROPPER] = iwindow_make_cursor_data( 
		dropper_src_bits, dropper_msk_bits,
		dropper_src_width, dropper_src_height, 0, 15 );
	iwindow_cursor[IWINDOW_SHAPE_MAGIN] = iwindow_make_cursor_data( 
		magin_src_bits, mag_msk_bits, 
		mag_msk_width, mag_msk_height, 6, 6 );
	iwindow_cursor[IWINDOW_SHAPE_MAGOUT] = iwindow_make_cursor_data( 
		magout_src_bits, mag_msk_bits,
		mag_msk_width, mag_msk_height, 6, 6 );

	/* The hglasses.
	 */
	for( i = 0; i < IM_NUMBER( watch_bits ); i++ )
		iwindow_cursor[IWINDOW_SHAPE_HGLASS1 + i] = 
			iwindow_make_cursor_data( 
				watch_bits[i], watch_msk_bits,
				watch_1_width, watch_1_height, 7, 7 );
}

/* Get the work window.
 */
static GdkWindow *
iwindow_get_work_window( iWindow *iwnd )
{
	if( iwnd->work_window )
		return( iwnd->work_window );
	else
		return( GTK_WIDGET( iwnd )->window );
}

/* Update the cursor for a window.
 */
static void *
iwindow_cursor_update( iWindow *iwnd )
{
	if( GTK_WIDGET_REALIZED( GTK_WIDGET( iwnd ) ) ) {
		GSList *p;
		iWindowShape best_shape;
		int best_priority;

		/* Global shape set? Use that for the whole window.
		 */
		if( iwnd->shape != IWINDOW_SHAPE_NONE ) {
			gdk_window_set_cursor( GTK_WIDGET( iwnd )->window, 
				iwindow_cursor[iwnd->shape] );
			gdk_window_set_cursor( iwindow_get_work_window( iwnd ),
				iwindow_cursor[iwnd->shape] );
			gdk_flush();

			return( NULL );
		}

		/* No global shape ... make sure there's no global cursor on
		 * this window.
		 */
		gdk_window_set_cursor( GTK_WIDGET( iwnd )->window, NULL );
		gdk_window_set_cursor( iwindow_get_work_window( iwnd ), NULL );

		/* And set the work area to the highest priority non-NONE 
		 * shape we can find .

			FIXME ... could avoid the search if we sorted the
			context list by priority on each context_new(),
			but not very important.

		 */
		best_shape = IWINDOW_SHAPE_NONE;
		best_priority = -1;
		for( p = iwnd->contexts; p; p = p->next ) {
			iWindowCursorContext *cntxt = 
				(iWindowCursorContext *) p->data;

			if( cntxt->shape != IWINDOW_SHAPE_NONE &&
				cntxt->priority > best_priority ) {
				best_shape = cntxt->shape;
				best_priority = cntxt->priority;
			}
		}

		/* Pref to disable crosshair.
		 */
		if( best_shape == IWINDOW_SHAPE_EDIT && !DISPLAY_CROSSHAIR )
			best_shape = IWINDOW_SHAPE_NONE;

		gdk_window_set_cursor( 
			iwindow_get_work_window( iwnd ),
			iwindow_cursor[best_shape] );
		gdk_flush();
	}

	return( NULL );
}

/* Set a global cursor for a window.
 */
static void *
iwindow_cursor_set( iWindow *iwnd, iWindowShape *shape )
{
	if( iwnd->shape != *shape ) {
		iwnd->shape = *shape;
		iwindow_cursor_update( iwnd );
	}

	return( NULL );
}

static gboolean hourglass_showing = FALSE;

static void
hourglass_begin( void )
{
	hourglass_showing = TRUE;
}

static void
hourglass_update( void )
{
	if( hourglass_showing ) {
		static iWindowShape shape = IWINDOW_SHAPE_HGLASS1;

		iwindow_map_all( (iWindowMapFn) iwindow_cursor_set, &shape );

		shape += 1;
		if( shape > IWINDOW_SHAPE_HGLASS8 )
			shape = IWINDOW_SHAPE_HGLASS1;
	}
}

static void
hourglass_end( void )
{
	if( hourglass_showing ) {
		iWindowShape shape = IWINDOW_SHAPE_NONE;

		iwindow_map_all( (iWindowMapFn) iwindow_cursor_set, &shape );
		hourglass_showing = FALSE;
	}
}

iWindowCursorContext *
iwindow_cursor_context_new( iWindow *iwnd, int priority, const char *name )
{
	iWindowCursorContext *cntxt = INEW( NULL, iWindowCursorContext );

#ifdef DEBUG
	printf( "iwindow_cursor_context_new: %s\n", name );
#endif /*DEBUG*/

	cntxt->iwnd = iwnd;
	cntxt->priority = priority;
	cntxt->name = name;
	cntxt->shape = IWINDOW_SHAPE_NONE;
	iwnd->contexts = g_slist_prepend( iwnd->contexts, cntxt );

	return( cntxt );
}

void
iwindow_cursor_context_destroy( iWindowCursorContext *cntxt )
{
	iWindow *iwnd = cntxt->iwnd;

	iwnd->contexts = g_slist_remove( iwnd->contexts, cntxt );
	IM_FREE( cntxt );
	iwindow_cursor_update( iwnd );
}

void
iwindow_cursor_context_set_cursor( iWindowCursorContext *cntxt, 
	iWindowShape shape )
{
	if( cntxt->shape != shape ) {
#ifdef DEBUG
		printf( "iwindow_cursor_context_set_cursor: %s = %s\n",
			cntxt->name, iwindow_cursor_name[shape] );
#endif /*DEBUG*/

		cntxt->shape = shape;
		iwindow_cursor_update( cntxt->iwnd );
	}
}

iWindowSusp *
iwindow_susp_new( iWindowFn fn, 
	iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys )
{
        iWindowSusp *susp;

        if( !(susp = INEW( NULL, iWindowSusp )) )
		return( NULL );

        susp->fn = fn; 
        susp->iwnd = iwnd; 
        susp->client = client; 
        susp->nfn = nfn;
        susp->sys = sys;

        return( susp );
}

/* Trigger a suspension's reply, and free it.
 */
void 
iwindow_susp_return( void *sys, iWindowResult result )
{
	iWindowSusp *susp = IWINDOW_SUSP( sys );

        susp->nfn( susp->sys, result );

        im_free( susp );
}

/* Compose two iWindowFns ... if this one succeeded, trigger the next in turn.
 * Otherwise bail out.
 */
void
iwindow_susp_comp( void *sys, iWindowResult result )
{
	iWindowSusp *susp = IWINDOW_SUSP( sys );

	if( result == IWINDOW_YES ) {
		susp->fn( susp->iwnd, susp->client, susp->nfn, susp->sys );
		im_free( susp );
	}
	else 
		iwindow_susp_return( sys, result );
}

/* Null window callback.
 */
void 
iwindow_true_cb( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys ) 
{ 
	nfn( sys, IWINDOW_YES );
}

void 
iwindow_false_cb( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys ) 
{ 
	nfn( sys, IWINDOW_NO );
}

/* Null notify callback.
 */
void iwindow_notify_null( void *client, iWindowResult result ) { }

/* Final end of a window. Destroy!
 */
static void
iwindow_final_death( iWindow *iwnd )
{
#ifdef DEBUG
	printf( "iwindow_final_death: %s\n", iwnd->title );
#endif /*DEBUG*/

	g_assert( iwnd->pending == 0 && iwnd->destroy );

	/* Clean up.
	 */
	gtk_widget_destroy( GTK_WIDGET( iwnd ) );
}

/* A notify comes back ... adjust the pending count. If this is a zombie and
 * this is the final pending, it's final death. 
 */
void
iwindow_notify_return( iWindow *iwnd )
{
#ifdef DEBUG
	printf( "iwindow_notify_return: %s (pending = %d)\n", 
		iwnd->title, iwnd->pending );
#endif /*DEBUG*/

	g_assert( iwnd->pending > 0 );

	iwnd->pending--;
	if( iwnd->destroy && iwnd->pending == 0 ) {
#ifdef DEBUG
		printf( "iwindow_notify_return: zombie death %s\n",
			iwnd->title );
#endif /*DEBUG*/
		iwindow_final_death( iwnd );
	}
}

/* Send a notify off, tell the client to come back to back.
 */
void
iwindow_notify_send( iWindow *iwnd, 
	iWindowFn fn, void *client, iWindowNotifyFn back, void *sys )
{
#ifdef DEBUG
	printf( "iwindow_notify_send: %s (pending = %d)\n", 
		iwnd->title, iwnd->pending );
#endif /*DEBUG*/

	iwnd->pending++;
	if( fn )
		fn( iwnd, client, back, sys );
	else
		back( sys, IWINDOW_YES );
}

static void
iwindow_finalize( GObject *gobject )
{
	iWindow *iwnd = IWINDOW( gobject );

#ifdef DEBUG
	printf( "iwindow_finalize: %s\n", iwnd->title );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	iwindow_all = g_slist_remove( iwindow_all, iwnd );
	IM_FREE( iwnd->title );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );

	/* Last window and we've got through startup? Quit the application.
	 * Test for 1, since main() makes an invisible iwindow as the root.
	 */
	if( iwindow_number() == 1 && !main_starting )
		main_quit_test();
}

static void
iwindow_destroy( GtkObject *gobject )
{
	iWindow *iwnd = IWINDOW( gobject );

#ifdef DEBUG
	printf( "iwindow_destroy: %s\n", iwnd->title );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	FREESID( iwnd->parent_unmap_sid, iwnd->parent_window );

	GTK_OBJECT_CLASS( parent_class )->destroy( gobject );
}

static void
iwindow_popdown_notify( iWindow *iwnd, iWindowResult result )
{
#ifdef DEBUG
	printf( "iwindow_popdown_notify: %s\n", iwnd->title );
#endif /*DEBUG*/

	if( result == IWINDOW_ERROR )
		box_alert( GTK_WIDGET( iwnd ) );
	else if( result == IWINDOW_YES )
		iwindow_kill( iwnd );

	if( result != IWINDOW_YES ) {
#ifdef DEBUG
		printf( "iwindow_popdown_notify: %s: kill cancelled!\n", 
			iwnd->title );
#endif /*DEBUG*/

		/* Cancel popdown.
		 */
		iwnd->destroy = FALSE;
	}
	else {
		/* Popdown confirmed! Trigger class popdown. _real_popdown()
		 * does an unmap to hide the window during the rest of the
		 * destroy.
		 */
		IWINDOW_GET_CLASS( iwnd )->popdown( GTK_WIDGET( iwnd ) );
	}

	calli_string_filenamef( (calli_string_fn) gtk_accel_map_save,
		"%s" G_DIR_SEPARATOR_S "accel_map", get_savedir() );

	/* If this is the final pending response and ->destroy is true, this 
	 * will destroy the window.
	 */
	iwindow_notify_return( iwnd );
}

static gboolean
iwindow_delete_event( GtkWidget *widget, GdkEventAny *event )
{
	iWindow *iwnd = IWINDOW( widget );

#ifdef DEBUG
	printf( "iwindow_delete_event: %s\n", iwnd->title );
#endif /*DEBUG*/

	if( !iwnd->destroy ) {
#ifdef DEBUG
		printf( "iwindow_delete_event: starting destroy\n" );
#endif /*DEBUG*/

		iwindow_kill( iwnd );
	}

	/* Never delete here ... wait for iwindow_popdown_notify to
	 * confirm the kill.
	 */
	return( TRUE );
}

static gboolean
iwindow_configure_event( GtkWidget *widget, GdkEventConfigure *event )
{
	iWindow *iwnd = IWINDOW( widget );

	if( iwnd->width_pref ) {
		/* Save window size in global prefs.
		 */
		prefs_set( iwnd->width_pref, "%d", event->width );
		prefs_set( iwnd->height_pref, "%d", event->height );
	}

	return( GTK_WIDGET_CLASS( parent_class )->
		configure_event( widget, event ) );
}

/* Our parent has been destroyed, kill us too.
 */
static void
iwindow_parent_unmap_cb( GtkWidget *par, iWindow *iwnd )
{
#ifdef DEBUG
	printf( "iwindow_parent_unmap_cb: %s\n", iwnd->title );
#endif /*DEBUG*/

	/* Here for dead parent ... if parent is dead, we won't need to remove
	 * the dead-dad signal. 
	 */
	iwnd->parent_unmap_sid = 0;

	iwindow_kill( iwnd );
}

static void 
iwindow_real_build( GtkWidget *widget )
{
	iWindow *iwnd = IWINDOW( widget );
	GdkScreen *screen = gtk_widget_get_screen( GTK_WIDGET( iwnd ) );

#ifdef DEBUG
	printf( "iwindow_real_build: %s\n", iwnd->title );
#endif /*DEBUG*/

        gtk_container_set_border_width( GTK_CONTAINER( iwnd ), 0 );

        iwnd->work = gtk_vbox_new( FALSE, 0 );
        gtk_container_add( GTK_CONTAINER( iwnd ), iwnd->work );

	/* Call per-instance build.
	 */
        if( iwnd->build )
		iwnd->build( iwnd, iwnd->work, 
			iwnd->build_a, iwnd->build_b, iwnd->build_c );

	if( iwnd->title )
		gtk_window_set_title( GTK_WINDOW( iwnd ), iwnd->title );

	if( iwnd->width_pref ) {
		int width = watch_int_get( main_watchgroup, 
			iwnd->width_pref, 640 );
		int height = watch_int_get( main_watchgroup, 
			iwnd->height_pref, 480 );

		gtk_window_set_default_size( GTK_WINDOW( iwnd ), 
			IM_MIN( width, gdk_screen_get_width( screen ) ),
			IM_MIN( height, gdk_screen_get_height( screen ) ) );
	}

	/* Link to parent.
	 */
        if( iwnd->parent_window ) {
		if( IWINDOW_GET_CLASS( iwnd )->transient )
			gtk_window_set_transient_for( GTK_WINDOW( iwnd ),
				GTK_WINDOW( iwnd->parent_window ) );

		/* We watch our parent's "unmap" rather than "destroy" since
		 * we use gtk_widget_unmap() to hide killed windows during
		 * popdown (see iwindow_popdown_notify()).
		 */
		iwnd->parent_unmap_sid = gtk_signal_connect( 
			GTK_OBJECT( iwnd->parent_window ), 
			"unmap",
			GTK_SIGNAL_FUNC( iwindow_parent_unmap_cb ), iwnd );
	}

        gtk_widget_show( iwnd->work );
}

static void 
iwindow_real_popdown( GtkWidget *widget )
{
	gtk_widget_unmap( widget );
}

static void
iwindow_class_init( iWindowClass *class )
{
	GObjectClass *object_class = (GObjectClass *) class;
	GtkObjectClass *gobject_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Init methods.
	 */
	object_class->finalize = iwindow_finalize;

	gobject_class->destroy = iwindow_destroy;

	widget_class->delete_event = iwindow_delete_event;
	widget_class->configure_event = iwindow_configure_event;

	class->build = iwindow_real_build;
	class->popdown = iwindow_real_popdown;

	class->transient = FALSE;

	/* Create signals.
	 */

	/* Static class data init.
	 */
	iwindow_make_cursors();

	/* Link to busy signals.
	 */
	g_signal_connect( progress_get(), "begin", hourglass_begin, NULL );
	g_signal_connect( progress_get(), "update", hourglass_update, NULL );
	g_signal_connect( progress_get(), "end", hourglass_end, NULL );
}

static void
iwindow_init( iWindow *iwnd )
{
#ifdef DEBUG
	printf( "iwindow_init: %s\n", iwnd->title );
#endif /*DEBUG*/

	iwnd->work = NULL;

	iwnd->parent = NULL;
	iwnd->parent_window = NULL;
        iwnd->parent_unmap_sid = 0;

	/* Might as well.
	 */
	iwnd->accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group( GTK_WINDOW( iwnd ), iwnd->accel_group );
	g_object_unref( iwnd->accel_group );

	iwnd->title = NULL;

	iwnd->build = NULL;
	iwnd->popdown = iwindow_true_cb;

	iwnd->destroy = FALSE;
	iwnd->pending = 0;

	iwnd->shape = IWINDOW_SHAPE_NONE;
	iwnd->contexts = NULL;
	iwnd->work_window = NULL;

	iwnd->width_pref = NULL;
	iwnd->height_pref = NULL;

	iwindow_all = g_slist_prepend( iwindow_all, iwnd );
}

GtkType
iwindow_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"iWindow",
			sizeof( iWindow ),
			sizeof( iWindowClass ),
			(GtkClassInitFunc) iwindow_class_init,
			(GtkObjectInitFunc) iwindow_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( GTK_TYPE_WINDOW, &info );
	}

	return( type );
}

GtkWidget *
iwindow_new( GtkWindowType type )
{
	iWindow *iwnd = gtk_type_new( TYPE_IWINDOW );
	GtkWindow *gwnd = GTK_WINDOW( iwnd );

	/* Init superclass.
	 */
	gwnd->type = type;

	return( GTK_WIDGET( iwnd ) );
}

void 
iwindow_set_title( iWindow *iwnd, const char *title, ... )
{
	va_list ap;
	char buf[1024];

        va_start( ap, title );
        (void) im_vsnprintf( buf, 1024, title, ap );
        va_end( ap );

	if( !iwnd->title || strcmp( iwnd->title, buf ) != 0 ) {
		IM_SETSTR( iwnd->title, buf );
		gtk_window_set_title( GTK_WINDOW( iwnd ), iwnd->title );
	}
}

void 
iwindow_set_build( iWindow *iwnd, 
	iWindowBuildFn build, void *build_a, void *build_b, void *build_c )
{
	iwnd->build = build;
	iwnd->build_a = build_a; 
	iwnd->build_b = build_b; 
	iwnd->build_c = build_c;
}

void 
iwindow_set_popdown( iWindow *iwnd, iWindowFn popdown, void *popdown_a )
{
	iwnd->popdown = popdown;
	iwnd->popdown_a = popdown_a;
}

void 
iwindow_set_size_prefs( iWindow *iwnd, 
	const char *width_pref, const char *height_pref )
{
	iwnd->width_pref = width_pref;
	iwnd->height_pref = height_pref;
}

void 
iwindow_set_work_window( iWindow *iwnd, GdkWindow *work_window )
{
	iwnd->work_window = work_window;
	iwindow_cursor_update( iwnd );
}

void 
iwindow_set_parent( iWindow *iwnd, GtkWidget *parent )
{
	g_assert( !iwnd->parent );

	iwnd->parent = parent;

	/* Get parent window now, we sometimes need it after parent has been
	 * destroyed.
	 */
	if( parent )
		iwnd->parent_window = 
			IWINDOW( iwindow_get_root( GTK_WIDGET( parent ) ) );
}

void *
iwindow_kill( iWindow *iwnd )
{
#ifdef DEBUG
	printf( "iwindow_kill: %s\n", iwnd->title );
#endif /*DEBUG*/

	if( !iwnd->destroy ) {
#ifdef DEBUG
		printf( "... starting destroy for %s\n", iwnd->title );
#endif /*DEBUG*/

		iwnd->destroy = TRUE;

		/* Don't kill directly, wait for popdown_notify to do it.
		 */
		iwindow_notify_send( iwnd, iwnd->popdown, iwnd->popdown_a,
			(iWindowNotifyFn) iwindow_popdown_notify, iwnd );
	}

	return( NULL );
}

/* ... as an action.
 */
void
iwindow_kill_action_cb( GtkAction *action, iWindow *iwnd )
{
	iwindow_kill( iwnd );
}

void 
iwindow_build( iWindow *iwnd )
{
#ifdef DEBUG
	printf( "iwindow_build: %s\n", iwnd->title );
#endif /*DEBUG*/

	IWINDOW_GET_CLASS( iwnd )->build( GTK_WIDGET( iwnd ) );
}

/* Get the enclosing window for a widget.
 */
GtkWidget *
iwindow_get_root( GtkWidget *widget )
{
	GtkWidget *toplevel = gtk_widget_get_toplevel( widget );
	GtkWidget *child = gtk_bin_get_child( GTK_BIN( toplevel ) );

	/* If this is a menu pane, get the widget that popped this menu up.
	 */
	if( GTK_IS_MENU( child ) ) {
		GtkWidget *parent = 
			gtk_menu_get_attach_widget( GTK_MENU( child ) );

		return( iwindow_get_root( parent ) );
	}
	else
		return( toplevel );
}

/* Get the enclosing no-parent window for a widget.
 */
GtkWidget *
iwindow_get_root_noparent( GtkWidget *widget )
{
	GtkWidget *toplevel = iwindow_get_root( widget );

	/* If this is a transient, get the window we popped up from.
	 */
	if( IS_IWINDOW( toplevel ) && IWINDOW( toplevel )->parent ) 
		return( iwindow_get_root( IWINDOW( toplevel )->parent ) );
	else
		return( toplevel );
}

