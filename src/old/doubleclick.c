/* ip: display VASARI format files.
 * 
 * doubleclick.c: separate single and double clicks on a widget
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

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /*HAVE_UNISTD_H*/
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <vips/vips.h>
#include <vips/vips7compat.h>
#include <vips/util.h>

#include "doubleclick.h"

#define FREEFI( F, S ) { if( S ) { (void) F( S ); (S) = 0; } }

/* For debugging.
#define DEBUG
 */

/* The struct we hold our private stuff inside.
 */
typedef struct doubleclick_info {
	GtkWidget *wid;		/* Widget we are attached to */
	guint click;		/* Timer for click determination */
	gboolean dsingle;	/* Do single click on first click of double */
	GdkEvent event;		/* Copy of last event for clients */

	DoubleclickFunc single;	/* Callback for single click */
	void *clients;		/* Client data for single */
	DoubleclickFunc dub;	/* Callback for double click */
	void *clientd;		/* Client data for double */
} Doubleclick; 

/* Allocate and free clicks.
 */
static Doubleclick *
doubleclick_new()
{
	Doubleclick *click;

	if( !(click = IM_NEW( NULL, Doubleclick )) )
		return( NULL );

	click->wid = NULL;
	click->click = 0;
	click->dsingle = FALSE;
	click->single = NULL;
	click->dub = NULL;

	return( click );
}

void
doubleclick_free( Doubleclick *click )
{
	im_free( click );
}

/* Timer callback for multiclick detection.
 */
static gboolean
doubleclick_time_cb( Doubleclick *click )
{
#ifdef DEBUG
	g_message( "doubleclick: timeout" );
#endif /*DEBUG*/

	click->click = 0;

	/* There has been no second click before the timeout: we do a single
	 * click. If st->dsingle is set though, we have already delivered the
	 * single-click event, so don't bother.
	 */
	if( !click->dsingle && click->single ) {
#ifdef DEBUG
		g_message( "doubleclick: timeout - calling single" );
#endif /*DEBUG*/
		click->single( click->wid, &click->event, click->clients );
	}

	/* Stop timer.
	 */
	return( FALSE );
}

/* There has been an event. Is it single or double?
 */
static gboolean
doubleclick_trigger_cb( GtkWidget *wid, GdkEvent *ev, Doubleclick *click )
{
	gboolean handled = FALSE;

	/* Make sure we have a button 1 press.
	 */
	if( ev->type != GDK_BUTTON_PRESS || ev->button.button != 1 )
		return( handled );

	/* Note event for client.
	 */
	click->event = *ev;

	if( click->click ) {
		/* There is a timeout pending - ie. there was a click
		 * recently. This must be the second part of a double click.
		 * Cancel the timeout and do a double click action.
		 */
		FREEFI( g_source_remove, click->click );
		if( click->dub ) {
#ifdef DEBUG
			g_message( "doubleclick: seen double" );
#endif /*DEBUG*/
			click->dub( click->wid, &click->event, click->clientd ); 
			handled = TRUE;
		}
	}
	else {
		int double_click_time;

		g_object_get( gtk_settings_get_default(), 
			"gtk-double-click-time", &double_click_time, NULL );

#ifdef DEBUG
		g_message( "doubleclick: starting timer" );
#endif /*DEBUG*/
		/* No previous click. This may be either. Start a timeout to
		 * help us decide.
		 *
		 * We aren't supposed to look at double_click_time, but
		 * there's no access method, I think.
		 */
		click->click = g_timeout_add( 
			double_click_time, 
			(GSourceFunc) doubleclick_time_cb, click );

		/* If do-single-on-double is set, we can trigger a
		 * single-click now.
		 */
		if( click->dsingle && click->single ) {
			click->single( click->wid, &click->event, 
				click->clients ); 
			handled = TRUE;
		}
	}

	return( handled );
}

/* Destroy a doubleclick_info.
 */
/*ARGSUSED*/
static void
doubleclick_destroy_cb( GtkWidget *wid, Doubleclick *click )
{
#ifdef DEBUG
	g_message( "doubleclick: destroyed" );
#endif /*DEBUG*/

	if( click->click ) {
		/* Don't trigger a single-click, even though there was one
		 * recently, since our widget is being destroyed.
		 */
		FREEFI( g_source_remove, click->click );
	}

	doubleclick_free( click );
}

static void
doubleclick_realize_cb( GtkWidget *wid )
{
	gtk_widget_add_events( wid, GDK_BUTTON_PRESS_MASK );
}

/* Attach callbacks to a widget.
 */
void
doubleclick_add( GtkWidget *wid, gboolean dsingle,
	DoubleclickFunc single, void *clients,
	DoubleclickFunc dub, void *clientd )
{
	Doubleclick *click = doubleclick_new();

	/* Complete fields.
	 */
	click->wid = wid;
	click->dsingle = dsingle;

	click->single = single;
	click->dub = dub;
	click->clients = clients;
	click->clientd = clientd;

	/* Add callbacks.
	 */
	g_signal_connect( wid, "destroy", 
		G_CALLBACK( doubleclick_destroy_cb ), click );
	g_signal_connect( wid, "event", 
		G_CALLBACK( doubleclick_trigger_cb ), click );
	g_signal_connect( wid, "realize", 
		G_CALLBACK( doubleclick_realize_cb ), NULL );
}
