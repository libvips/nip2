/* make and manage base dialogs ... subclass off this for others
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

#include "ip.h"

/*
#define DEBUG
 */

static iWindowClass *parent_class = NULL;

/* Handy destroy callback ... just free client.
 */
void
idialog_free_client( iDialog *idlg, void *client )
{
	IM_FREE( client );
}

/* Notify our parent.
 */
static void
idialog_notify_parent( iDialog *idlg, iWindowResult result )
{
	if( idlg->nfn ) {
		iWindowNotifyFn nfn = idlg->nfn;
		
		idlg->nfn = NULL;
		nfn( idlg->sys, result );
	}
}

static void *
idialog_set_sensitive( GtkWidget *w, gboolean state )
{
	gtk_widget_set_sensitive( w, state );

	return( NULL );
}

/* Set OK sensitivities.
 */
void
idialog_set_ok_button_state( iDialog *idlg, gboolean state )
{
	slist_map( idlg->ok_l, 
		(SListMapFn) idialog_set_sensitive, GINT_TO_POINTER( state ) );
}

/* Set all the button sensitivities.
 */
static void
idialog_set_button_state( iDialog *idlg, gboolean state )
{
	idialog_set_ok_button_state( idlg, state );
	if( idlg->but_cancel )
		gtk_widget_set_sensitive( idlg->but_cancel, state );
	if( idlg->but_help )
		gtk_widget_set_sensitive( idlg->but_help, state );
}

/* Sub-fn of below. Come back from a popdown notify.
 */
static void
idialog_popdown_notify( void *sys, iWindowResult result )
{
        iWindowSusp *susp = IWINDOW_SUSP( sys );
	iDialog *idlg = IDIALOG( susp->client );

	if( result == IWINDOW_TRUE ) 
		/* If our caller hasn't been notified yet, post off a FALSE.
		 */
		idialog_notify_parent( idlg, IWINDOW_FALSE );

	/* Pass result on to our suspension (ie. back to iwindow).
	 */
	iwindow_susp_return( susp, result );

	/* Housekeeping.
	 */
	iwindow_notify_return( IWINDOW( idlg ) );
}

/* Our popdown callback ... here from iwindow.
 */
static void 
idialog_popdown_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys ) 
{
	iDialog *idlg = IDIALOG( client );
        iWindowSusp *susp = iwindow_susp_new( NULL, iwnd, idlg, nfn, sys );

#ifdef DEBUG
	printf( "idialog_popdown_cb: %s\n", IWINDOW( idlg )->title );
#endif /*DEBUG*/

	/* Trigger user popdown.
	 */
	iwindow_notify_send( IWINDOW( idlg ), 
		idlg->popdown_cb, idlg->client, idialog_popdown_notify, susp );
}

/* Sub-fn of below. Come back from a done notify.
 */
static void
idialog_done_notify( void *sys, iWindowResult result )
{
	iDialog *idlg = IDIALOG( sys );

#ifdef DEBUG
	printf( "idialog_done_notify: %s\n", IWINDOW( idlg )->title );
#endif /*DEBUG*/

	idialog_set_button_state( idlg, TRUE );

	/* If all ok, popdown and tell our parent.
	 */
	if( result == IWINDOW_TRUE ) {
		/* Unless we're pinned up, that is.
		 */
		if( !(idlg->tog_pin && 
			gtk_toggle_button_get_active(
				GTK_TOGGLE_BUTTON( idlg->tog_pin ) )) ) {
			idialog_notify_parent( idlg, result );
			iwindow_kill( IWINDOW( idlg ) );
		}
	}

	/* Alert on failure.
	 */
	if( result == IWINDOW_ERROR )
		box_alert( GTK_WIDGET( idlg ) );

	/* Clean up.
	 */
	iwindow_notify_return( IWINDOW( idlg ) );
}

/* Make a DONE event happen. Used (for example) by the browse window to force
 * a done in the enclosing FSB on double click on icon. 
 */
void
idialog_done_trigger( iDialog *idlg, int pos )
{
	iWindowFn fn = (iWindowFn) g_slist_nth_data( idlg->ok_cb_l, pos );

#ifdef DEBUG
	printf( "idialog_done_trigger: %s, %d\n", 
		IWINDOW( idlg )->title, pos );
#endif /*DEBUG*/

	/* Trigger user done callback.
	 */
	assert( fn );
	idialog_set_button_state( idlg, FALSE );
	iwindow_notify_send( IWINDOW( idlg ), 
		fn, idlg->client, idialog_done_notify, idlg );
}

/* Sub-fn of below.
 */
static void
idialog_cancel_notify( void *sys, iWindowResult result )
{
	iDialog *idlg = IDIALOG( sys );

#ifdef DEBUG
	printf( "idialog_cancel_notify: %s\n", IWINDOW( idlg )->title );
#endif /*DEBUG*/

	idialog_set_button_state( idlg, TRUE );

	/* Send cancel message back to parent if our client cancel was OK.
	 */
	if( result == IWINDOW_TRUE ) {
		idialog_notify_parent( idlg, IWINDOW_FALSE );
		iwindow_kill( IWINDOW( idlg ) );
	}

	/* Alert on error.
	 */
	if( result == IWINDOW_ERROR )
		box_alert( GTK_WIDGET( idlg ) );

	/* Clean up.
	 */
	iwindow_notify_return( IWINDOW( idlg ) );
}

static void
idialog_cancel_trigger( iDialog *idlg )
{
#ifdef DEBUG
        printf( "idialog_cancel_trigger: %s\n", IWINDOW( idlg )->title );
#endif /*DEBUG*/

        /* Trigger user cancel function.
         */
	idialog_set_button_state( idlg, FALSE );
        iwindow_notify_send( IWINDOW( idlg ), 
		idlg->cancel_cb, idlg->client, idialog_cancel_notify, idlg );
}

/* Button callbacks from gtk.
 */
static void
idialog_done_cb( GtkWidget *w, iDialog *idlg )
{
	int pos = g_slist_index( idlg->ok_l, w );

	assert( pos != -1 );

	idialog_done_trigger( idlg, pos );
}

static void
idialog_cancel_cb( GtkWidget *w, iDialog *idlg )
{
	idialog_cancel_trigger( idlg );
}

static void
idialog_help_cb( GtkWidget *w, iDialog *idlg )
{
	if( idlg->help_tag )
		box_help( GTK_WIDGET( idlg ), idlg->help_tag );
}

static void
idialog_destroy( GtkObject *object )
{
	iDialog *idlg;

#ifdef DEBUG
	printf( "idialog_destroy\n" );
#endif /*DEBUG*/

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_IDIALOG( object ) );

	idlg = IDIALOG( object );

#ifdef DEBUG
	printf( "... %s\n", IWINDOW( idlg )->title );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */

	if( idlg->destroy_cb ) {
		idlg->destroy_cb( idlg, idlg->client );
		idlg->destroy_cb = NULL;
	}

	FREESID( idlg->destroy_sid, idlg->iobject );
	slist_map( idlg->ok_txt_l, (SListMapFn) im_free, NULL );
	IM_FREEF( g_slist_free, idlg->ok_cb_l );
	IM_FREEF( g_slist_free, idlg->ok_txt_l );
	IM_FREEF( g_slist_free, idlg->ok_l );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
idialog_realize( GtkWidget *widget )
{
	iDialog *idlg = IDIALOG( widget );

#ifdef DEBUG
	printf( "idialog_realize: %s\n", IWINDOW( idlg )->title );
#endif /*DEBUG*/

	GTK_WIDGET_CLASS( parent_class )->realize( widget );

	if( idlg->entry )
		gtk_widget_grab_focus( GTK_WIDGET( idlg->entry ) );
}

/* The object we represent has been destroyed, kill us too.
 */
static void
idialog_iobject_destroy( iObject *iobject, iDialog *idlg )
{
#ifdef DEBUG
	printf( "idialog_iobject_destroy: %s\n", IWINDOW( idlg )->title );
#endif /*DEBUG*/

	/* This object has gone.
	 */
	idlg->iobject = NULL;

	iwindow_kill( IWINDOW( idlg ) );
}

static void *
idialog_build_ok( const char *txt, iDialog *idlg )
{
	GtkWidget *but;

	but = build_button( txt, GTK_SIGNAL_FUNC( idialog_done_cb ), idlg );
	idlg->ok_l = g_slist_prepend( idlg->ok_l, but );
	gtk_box_pack_start( GTK_BOX( idlg->bb ), but, TRUE, TRUE, 0 );
	gtk_widget_show( but );

	return( NULL );
}

/* Set a button to be the dialog default. Turn off button_focus for complex
 * dialogs like file_chooser which have their on focus systems.
 */
static void
idialog_set_default( iDialog *idlg, GtkWidget *widget )
{
	if( idlg->button_focus ) 
		gtk_widget_grab_focus( widget );

	GTK_WIDGET_SET_FLAGS( widget, GTK_CAN_DEFAULT );
	gtk_window_set_default( GTK_WINDOW( idlg ), widget );
}

static void 
idialog_build( GtkWidget *widget )
{
	iDialog *idlg = IDIALOG( widget );
	iWindow *iwnd = IWINDOW( idlg );

#ifdef DEBUG
	printf( "idialog_build: %s\n", iwnd->title );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( parent_class )->build )
		(*IWINDOW_CLASS( parent_class )->build)( widget );

	/* delete_event and destroy handled by our superclass.
	 */
	iwindow_set_popdown( iwnd, idialog_popdown_cb, idlg );

        gtk_window_set_modal( GTK_WINDOW( idlg ), idlg->modal );

        idlg->work = gtk_vbox_new( FALSE, 6 );
        gtk_container_set_border_width( GTK_CONTAINER( idlg->work ), 12 );
        gtk_box_pack_start( GTK_BOX( iwnd->work ), idlg->work, TRUE, TRUE, 0 );

	if( !idlg->nosep ) {
		GtkWidget *sep;

		sep = gtk_hseparator_new();
		gtk_box_pack_start( GTK_BOX( iwnd->work ), 
			sep, FALSE, FALSE, 2 );
		gtk_widget_show( sep );
	}

	idlg->hb = gtk_hbox_new( FALSE, 6 );
        gtk_container_set_border_width( GTK_CONTAINER( idlg->hb ), 12 );
        gtk_box_pack_start( GTK_BOX( iwnd->work ), idlg->hb, FALSE, FALSE, 0 );
        gtk_widget_show( idlg->hb );

	if( idlg->pinup ) {
		idlg->tog_pin = gtk_check_button_new_with_label( 
			_( "Pin up" ) );
		set_tooltip( idlg->tog_pin, 
			_( "Check this to pin the dialog up" ) );
                gtk_box_pack_start( GTK_BOX( idlg->hb ),
                        idlg->tog_pin, FALSE, FALSE, 0 );
		gtk_widget_show( idlg->tog_pin );
	}

        idlg->bb = gtk_hbutton_box_new();
        gtk_button_box_set_layout( GTK_BUTTON_BOX( idlg->bb ), 
		GTK_BUTTONBOX_END );
	gtk_box_set_spacing( GTK_BOX( idlg->bb ), 6 );
        gtk_box_pack_end( GTK_BOX( idlg->hb ), idlg->bb, FALSE, FALSE, 0 );
        gtk_widget_show( idlg->bb );

	/* Button order: 
	 *
	 * Help        OK3 OK2 Cancel OK1
	 */

        if( idlg->help_tag ) {
                idlg->but_help = build_button( GTK_STOCK_HELP,
			GTK_SIGNAL_FUNC( idialog_help_cb ), idlg );
                gtk_box_pack_end( GTK_BOX( idlg->bb ),
                        idlg->but_help, TRUE, TRUE, 0 );
		gtk_button_box_set_child_secondary( GTK_BUTTON_BOX( idlg->bb ),
			idlg->but_help, TRUE );
                gtk_widget_show( idlg->but_help );
        }

	/* Add OK2, 3, etc.
	 */
	if( idlg->ok_txt_l && idlg->ok_txt_l->next )
		slist_map_rev( idlg->ok_txt_l->next,
			(SListMapFn) idialog_build_ok, idlg );

        if( idlg->cancel_cb ) {
                idlg->but_cancel = build_button( idlg->cancel_text,
			GTK_SIGNAL_FUNC( idialog_cancel_cb ), idlg );
                gtk_box_pack_start( GTK_BOX( idlg->bb ),
                        idlg->but_cancel, TRUE, TRUE, 0 );
                gtk_widget_show( idlg->but_cancel );

		/* Cancel grabs default if it's the only button. Set focus
		 * too; user build can change this later.
		 */
                if( !idlg->ok_l ) 
			idialog_set_default( idlg, idlg->but_cancel );
	}

	/* Make OK1
	 */
	if( idlg->ok_txt_l ) {
		const char *ok1_txt = (const char *) (idlg->ok_txt_l->data);

		idialog_build_ok( ok1_txt, idlg );
	}

	/* OK1 grabs the default.
	 */
	if( idlg->ok_l ) 
		idialog_set_default( idlg, idlg->ok_l->data );

	/* Escape triggers cancel, if there is a cancel.
	 */
	if( idlg->cancel_cb )
		gtk_widget_add_accelerator( idlg->but_cancel,
			"clicked", iwnd->accel_group, GDK_Escape, 0, 0 );
	else {
		/* If there's just 1 OK, that gets Esc too.
		 */
		if( g_slist_length( idlg->ok_l ) == 1 )
			gtk_widget_add_accelerator( 
				GTK_WIDGET( idlg->ok_l->data ), "clicked", 
				iwnd->accel_group, GDK_Escape, 0, 0 );
	}

	/* F1 triggers help.
	 */
	if( idlg->but_help ) 
		gtk_widget_add_accelerator( 
			idlg->but_help,
			"clicked", iwnd->accel_group, GDK_F1, 0, 0 );

	/* Build user dialog contents.
	 */
	if( idlg->build )
		idlg->build( iwnd, idlg->work, 
			idlg->build_a, idlg->build_b, idlg->build_c );

	if( idlg->iobject )
		idlg->destroy_sid = g_signal_connect( idlg->iobject, "destroy", 
			G_CALLBACK( idialog_iobject_destroy ), idlg );

	/* Set transient hint ... this may not have been set by our iwindow
	 * superclass if the lock-in-front pref is off.
	 */
        if( iwnd->parent_window ) 
		gtk_window_set_transient_for( GTK_WINDOW( iwnd ),
			GTK_WINDOW( iwnd->parent_window ) );

        gtk_widget_show( idlg->work );
}

static void
idialog_class_init( iDialogClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	iWindowClass *iwindow_class = (iWindowClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = idialog_destroy;

	widget_class->realize = idialog_realize;

	iwindow_class->build = idialog_build;
	iwindow_class->transient = TRUE;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
idialog_init( iDialog *idlg )
{
#ifdef DEBUG
	printf( "idialog_init: %s\n", IWINDOW( idlg )->title );
#endif /*DEBUG*/

	/* Init our instance fields.
	 */
	idlg->iobject = NULL;
        idlg->destroy_sid = 0;

	idlg->work = NULL;

	idlg->ok_l = NULL;
	idlg->ok_txt_l = NULL;
	idlg->but_cancel = NULL;
	idlg->but_help = NULL;
	idlg->tog_pin = NULL;

	idlg->entry = NULL;

	idlg->modal = FALSE;
	idlg->pinup = FALSE;
	idlg->nosep = FALSE;
	idlg->button_focus = TRUE;

	idlg->help_tag = NULL;

	idlg->cancel_text = GTK_STOCK_CANCEL;

	idlg->ok_cb_l = NULL;
	idlg->cancel_cb = NULL;
	idlg->popdown_cb = NULL;
	idlg->destroy_cb = NULL;
	idlg->client = NULL;

	idlg->arg = NULL;

	idlg->nfn = iwindow_notify_null;
	idlg->sys = NULL;

	gtk_window_set_position( GTK_WINDOW( idlg ),
		GTK_WIN_POS_CENTER_ON_PARENT );
}

GtkType
idialog_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"iDialog",
			sizeof( iDialog ),
			sizeof( iDialogClass ),
			(GtkClassInitFunc) idialog_class_init,
			(GtkObjectInitFunc) idialog_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_IWINDOW, &info );
	}

	return( type );
}

GtkWidget *
idialog_new()
{
	iDialog *idlg = gtk_type_new( TYPE_IDIALOG );
	GtkWindow *gwnd = GTK_WINDOW( idlg );

	/* Init gtk base class.
	 */
	gwnd->type = GTK_WINDOW_TOPLEVEL;

	return( GTK_WIDGET( idlg ) );
}

void 
idialog_set_iobject( iDialog *idlg, iObject *iobject )
{
	idlg->iobject = iobject;
}

void 
idialog_set_pinup( iDialog *idlg, gboolean pinup )
{
	idlg->pinup = pinup;

	if( idlg->tog_pin ) 
		gtk_toggle_button_set_active( 
			GTK_TOGGLE_BUTTON( idlg->tog_pin ), TRUE );
}

void 
idialog_set_modal( iDialog *idlg, gboolean modal )
{
	idlg->modal = modal;
}

void 
idialog_set_nosep( iDialog *idlg, gboolean nosep )
{
	idlg->nosep = nosep;
}

void 
idialog_set_button_focus( iDialog *idlg, gboolean button_focus )
{
	idlg->button_focus = button_focus;
}

void 
idialog_set_help_tag( iDialog *idlg, const char *help_tag )
{
	IM_SETSTR( idlg->help_tag, help_tag );
}

void 
idialog_set_callbacks( iDialog *idlg, 
	iWindowFn cancel_cb, iWindowFn popdown_cb, 
	iDialogFreeFn destroy_cb, void *client )
{
	idlg->cancel_cb = cancel_cb;
	idlg->popdown_cb = popdown_cb;
	idlg->destroy_cb = destroy_cb;
	idlg->client = client;
}

void 
idialog_add_ok( iDialog *idlg, iWindowFn done_cb, const char *fmt, ... )
{
	va_list ap;
	char buf[1024];

        va_start( ap, fmt );
        (void) im_vsnprintf( buf, 1024, fmt, ap );
        va_end( ap );

	/* So the last OK button added is the default one (and at the head of
	 * the list). [OK1, OK2, OK3, OK4]
	 */
	idlg->ok_cb_l = g_slist_prepend( idlg->ok_cb_l, (void *) done_cb );
	idlg->ok_txt_l = 
		g_slist_prepend( idlg->ok_txt_l, im_strdup( NULL, buf ) );
}

void 
idialog_set_notify( iDialog *idlg, iWindowNotifyFn nfn, void *sys )
{
	idlg->nfn = nfn;
	idlg->sys = sys;
}

void 
idialog_set_build( iDialog *idlg, 
	iWindowBuildFn build, void *build_a, void *build_b, void *build_c )
{
	idlg->build = build;
	idlg->build_a = build_a;
	idlg->build_b = build_b;
	idlg->build_c = build_c;
}

void 
idialog_set_cancel_text( iDialog *idlg, const char *cancel_text )
{
	idlg->cancel_text = cancel_text;
}

void 
idialog_set_default_entry( iDialog *idlg, GtkEntry *entry )
{
	gtk_entry_set_activates_default( entry, TRUE );
	idlg->entry = entry;
}

/* Set up an entry inside a dialog ... set tooltip, set start
 * value, link to OK button in enclosing dialog.
 */
void
idialog_init_entry( iDialog *idlg, GtkWidget *entry, 
	const char *tip, const char *fmt, ... )
{
	va_list ap;

	va_start( ap, fmt );
        set_gentryv( entry, fmt, ap );
	va_end( ap );
        set_tooltip( entry, "%s", tip );
	idialog_set_default_entry( idlg, GTK_ENTRY( entry ) );
}
