/* a pair of spin buttons, with no entry ... don't actually use buttons,
 * since we may have lots and lots of these, and we don't want to make an X
 * window for each one
 *
 * we do the event handling ourselves ... our enclosing view passes the ev 
 * to spin_event(), this triggers signals as required
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

G_DEFINE_TYPE( Spin, spin, TYPE_VIEW ); 

/* Our signals. Up and down click.
 */
enum {
	UP_CLICK,
	DOWN_CLICK,
	LAST_SIGNAL
};

static guint spin_signals[LAST_SIGNAL] = { 0 };

/* Default up and down signal handlers.
 */
static void
spin_real_up_click( Spin *spin )
{
#ifdef DEBUG
	printf( "spin_real_up_click\n" );
#endif /*DEBUG*/
}

static void
spin_real_down_click( Spin *spin )
{
#ifdef DEBUG
	printf( "spin_real_down_click\n" );
#endif /*DEBUG*/
}

static void
spin_class_init( SpinClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;

	/* Create signals.
         */
	spin_signals[UP_CLICK] = g_signal_new( "up_click",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( SpinClass, up_click ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	spin_signals[DOWN_CLICK] = g_signal_new( "down_click",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( SpinClass, down_click ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );

	class->up_click = spin_real_up_click;
	class->down_click = spin_real_down_click;
}

typedef struct {
	Spin *spin;
	int x, y;		/* Click position */
	gboolean handled;
} SpinEvent;

static void
allocation2rect( GtkAllocation *from, Rect *to )
{
        to->left = from->x;
        to->top = from->y;
        to->width = from->width;
        to->height = from->height;
}

static void
spin_button_press_event_test( GtkWidget *widget, gpointer data )
{
	SpinEvent *sev = (SpinEvent *) data;
	Rect pos;

	if( sev->handled )
		return;

	allocation2rect( &widget->allocation, &pos );
	if( im_rect_includespoint( &pos, sev->x, sev->y ) ) {
		if( GTK_IS_ARROW( widget ) ) {
			sev->handled = TRUE;

			if( GTK_ARROW( widget )->arrow_type == GTK_ARROW_UP )
				g_signal_emit( G_OBJECT( sev->spin ), 
					spin_signals[UP_CLICK], 0 );
			else
				g_signal_emit( G_OBJECT( sev->spin ), 
					spin_signals[DOWN_CLICK], 0 );
		}
	}
}

/* Event in us somewhere.
 */
static gboolean
spin_button_press_event_cb( GtkWidget *widget, GdkEventButton *event, 
	Spin *spin ) 
{
	gboolean handled = FALSE;

	if( event->type == GDK_BUTTON_PRESS ) {
		SpinEvent sev;

		if( event->button == 1 ) {
			sev.spin = spin;
			/* Find button x/y relative to top LH corner of spin.
			 */
			sev.x = event->x + GTK_WIDGET( spin )->allocation.x;
			sev.y = event->y + GTK_WIDGET( spin )->allocation.y;
			sev.handled = FALSE;
			spin_button_press_event_test( spin->up, &sev );
			spin_button_press_event_test( spin->down, &sev );

			handled = sev.handled;
		}
	}

	return( handled );
}       

static gboolean 
spin_button_enter_notify_event_cb( GtkWidget *widget, GdkEventCrossing *event,
	Spin *spin )
{
	gboolean handled = FALSE;

	if( event->detail != GDK_NOTIFY_INFERIOR ) 
		gtk_widget_set_state( widget, GTK_STATE_PRELIGHT );

	return( handled );
}

static gboolean 
spin_button_leave_notify_event_cb( GtkWidget *widget, GdkEventCrossing *event,
	Spin *spin )
{
	gboolean handled = FALSE;

	if( event->detail != GDK_NOTIFY_INFERIOR ) 
		gtk_widget_set_state( widget, GTK_STATE_NORMAL );

	return( handled );
}

static void
spin_init( Spin *spin )
{
	GtkWidget *ebox;
	GtkWidget *vbox;

	ebox = gtk_event_box_new();
	set_tooltip( ebox, _( "Expand or collapse row" ) );
	gtk_event_box_set_visible_window( GTK_EVENT_BOX( ebox ), FALSE );
        g_signal_connect( ebox, "button_press_event",
                G_CALLBACK( spin_button_press_event_cb ), spin );
        g_signal_connect( ebox, "enter_notify_event",
                G_CALLBACK( spin_button_enter_notify_event_cb ), spin );
        g_signal_connect( ebox, "leave_notify_event",
                G_CALLBACK( spin_button_leave_notify_event_cb ), spin );
        gtk_box_pack_start( GTK_BOX( spin ), ebox, FALSE, FALSE, 0 );
	gtk_widget_show( ebox );

	vbox = gtk_box_new( GTK_ORIENTATION_VERTICAL, FALSE );
	gtk_container_add( GTK_CONTAINER( ebox ), vbox );
	gtk_widget_show( vbox );

	spin->up = gtk_arrow_new( GTK_ARROW_UP, GTK_SHADOW_OUT );
        spin->down = gtk_arrow_new( GTK_ARROW_DOWN, GTK_SHADOW_OUT );
        gtk_box_pack_start( GTK_BOX( vbox ), spin->up, FALSE, FALSE, 0 );
        gtk_box_pack_end( GTK_BOX( vbox ), spin->down, FALSE, FALSE, 0 );
	gtk_widget_show( spin->up );
        gtk_widget_show( spin->down );
}

GtkWidget *
spin_new( void )
{
	Spin *spin = g_object_new( TYPE_SPIN, NULL );

	return( GTK_WIDGET( spin ) );
}
