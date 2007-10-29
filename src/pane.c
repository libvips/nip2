/* a side panel that can slide in and out of view
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

/* Our signals. 
 */
enum {
	SIG_CHANGED,	/* Change to position or openness */
	SIG_LAST
};

static GtkHPanedClass *parent_class = NULL;

static guint pane_signals[SIG_LAST] = { 0 };

static void
pane_changed( Pane *pane )
{
	assert( IS_PANE( pane ) );

#ifdef DEBUG
	printf( "pane_changed:\n" );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( pane ), pane_signals[SIG_CHANGED], 0 );
}

static int
pane_closed_position( Pane *pane )
{
	/* Can't use max/min since we need to be able to work before our
	 * window has been built.
	 */
	return( pane->handedness == PANE_HIDE_RIGHT ?  10000 : 0 ); 
}

/* An open position ... used in case we are asked to open, but the position is
 * already closed.
 */
static int
pane_open_position( Pane *pane )
{
	int max_position;
	int min_position;

	g_object_get( pane, 
		"max_position", &max_position, 
		"min_position", &min_position, 
		NULL );

	return( pane->handedness == PANE_HIDE_RIGHT ? 
		max_position - 100: min_position + 100 ); 
}

static void
pane_destroy( GtkObject *object )
{
	Pane *pane;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PANE( object ) );

	pane = PANE( object );

#ifdef DEBUG
	printf( "pane_destroy:\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	IM_FREEF( g_source_remove, pane->animate_timeout );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
pane_class_init( PaneClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = pane_destroy;

	class->changed = NULL;

	pane_signals[SIG_CHANGED] = g_signal_new( "changed",
		G_OBJECT_CLASS_TYPE( object_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( PaneClass, changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
}

/* Position property has changed. We block the notify signal before we set
 * position, so this change must have come from a user drag or parent window
 * resize.
 */
static void
pane_notify_position_cb( Pane *pane )
{
	int max_position;
	int min_position;
	int position;
	gboolean changed;

	/* Can get here even though we block notify during position set in
	 * animate, becauseof delays in window setup.
	 */
	if( pane->animate_timeout )
		return;

	g_object_get( pane, 
		"max_position", &max_position, 
		"min_position", &min_position, 
		NULL );

	/* We can have 10,000 in as position (meaning way to the
	 * right), take account of any clipping there may be.
	 */
	pane->position = 
		IM_CLIP( min_position, pane->position, max_position );

	/* And the new value.
	 */
	position = gtk_paned_get_position( GTK_PANED( pane ) );

#ifdef DEBUG
	printf( "pane_notify_position_cb: %d\n", position );
#endif /*DEBUG*/

	changed = FALSE;

	if( (pane->handedness == PANE_HIDE_LEFT &&
		position == min_position) ||
		(pane->handedness == PANE_HIDE_RIGHT &&
		position == max_position) ) {
		/* Position changed to closed. 
		 */
		if( pane->open ) {
			pane->open = FALSE;
			pane->position = position;
			changed = TRUE;
		}
	}
	else if( pane->position != position ) {
		/* Either dragged open, or drag while open.
		 */
		pane->open = TRUE;
		pane->position = position;
		changed = TRUE;
	}

	if( changed )
		pane_changed( pane );
}

static void
pane_init( Pane *pane )
{
	pane->position = 400; 		/* overwritten on _link() */
	pane->animate_timeout = 0;
	pane->open = FALSE;
	g_signal_connect( pane, "notify::position", 
		G_CALLBACK( pane_notify_position_cb ), NULL );
}

GType
pane_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( PaneClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) pane_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Pane ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) pane_init,
		};

		type = g_type_register_static( GTK_TYPE_HPANED, 
			"Pane", &info, 0 );
	}

	return( type );
}

/* Set the position without triggering notify.
 */
static void
pane_set_widget_position( Pane *pane, int position )
{
	g_signal_handlers_block_by_func( pane, 
		pane_notify_position_cb, NULL );
	gtk_paned_set_position( GTK_PANED( pane ), position );
	g_signal_handlers_unblock_by_func( pane, 
		pane_notify_position_cb, NULL );
}

static gboolean
pane_animate_timeout_cb( Pane *pane )
{
	int now = gtk_paned_get_position( GTK_PANED( pane ) );
	int target = pane->target_position;
	int new;
	gboolean more;

#ifdef DEBUG
	printf( "pane_animate_timeout_cb:\n", );
#endif /*DEBUG*/

	new = now + (target - now) / 2;

	if( ABS( new - target ) < 5 || new == pane->last_set_position ) {
		/* Close enough: set exactly the target and stop animating. Or
		 * the new position we set last hasn't taken .. in which case 
		 * we must have hit a stop.
		 */
		new = target;
		more = FALSE;
	}
	else 
		/* Keep moving.
		 */
		more = TRUE;

	pane->last_set_position = new;
	pane_set_widget_position( pane, new );
	pane->animate_timeout = 0;

	return( more );
}

/* Animate the pane to a new position.
 */
static void
pane_animate( Pane *pane, int target_position )
{
	int max_position;
	int min_position;

	g_object_get( pane, 
		"max_position", &max_position, 
		"min_position", &min_position, 
		NULL );

	/* Can be zero if we're here very early.
	 */
	if( max_position > 0 ) 
		target_position = 
			IM_CLIP( min_position, target_position, max_position );
	pane->target_position = target_position;

#ifdef DEBUG
	printf( "pane_animate: max = %d, min = %d, target = %d\n", 
		max_position, min_position, pane->target_position );
#endif /*DEBUG*/

	/* Invalidate the last set pos.
	 */
	pane->last_set_position = -1;

	if( !pane->animate_timeout ) 
		pane->animate_timeout = g_timeout_add( 50, 
			(GSourceFunc) pane_animate_timeout_cb, pane );
}

static void
pane_link( Pane *pane, PaneHandedness handedness )
{
	pane->handedness = handedness;
	pane_set_widget_position( pane, pane_closed_position( pane ) );
}

Pane *
pane_new( PaneHandedness handedness )
{
	Pane *pane;

#ifdef DEBUG
	printf( "pane_new:\n" );
#endif /*DEBUG*/

	pane = PANE( g_object_new( TYPE_PANE, NULL ) );
	pane_link( pane, handedness );

	return( pane );
}

/* Change state, with an animation.
 */
void
pane_set_open_position( Pane *pane, gboolean open, int position )
{
#ifdef DEBUG
	printf( "pane_set_open_position: %s %d\n", 
		bool_to_char( open ), position );
#endif /*DEBUG*/

	if( pane->animate_timeout ) 
		return;

	if( pane->open == open && pane->position == position ) 
		return;

	if( !pane->open && !open ) {
		/* Change the position while closed. No animation.
		 */
		pane->position = position;
	}
	else if( !open ) {
		/* Closing ... need an animation.
		 */
		pane->position = position;
		pane->open = FALSE;
		pane_animate( pane, pane_closed_position( pane ) );
	}
	else {
		/* Opening, or move while open. We may have previously been
		 * dragged closed, in which case position could be
		 * closed_position. 
		 */
		if( position == pane_closed_position( pane ) )
			position = pane_open_position( pane );

		pane->position = position;
		pane->open = open;
		pane_animate( pane, position );
	}

	pane_changed( pane );
}

void
pane_set_open( Pane *pane, gboolean open )
{
	pane_set_open_position( pane, open, pane->position );
}
