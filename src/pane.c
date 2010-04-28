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

#ifdef DEBUG
static char *
pane_handedness2char( PaneHandedness handedness )
{
	switch( handedness ) {
	case PANE_HIDE_LEFT:	return( "PANE_HIDE_LEFT" );
	case PANE_HIDE_RIGHT:	return( "PANE_HIDE_RIGHT" );
	default:		g_assert( 0 );
	}
}
#endif /*DEBUG*/

static void
pane_changed( Pane *pane )
{
	g_assert( IS_PANE( pane ) );

#ifdef DEBUG
	printf( "pane_changed: %p %s\n", 
		pane, pane_handedness2char( pane->handedness ) );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( pane ), pane_signals[SIG_CHANGED], 0 );
}

static int
pane_closed_position( Pane *pane )
{
	/* Can't use max/min since we need to be able to work before our
	 * window has been built.
	 */
	return( pane->handedness == PANE_HIDE_RIGHT ? 10000 : 0 ); 
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
		max_position - 200: min_position + 200 ); 
}

static void
pane_destroy( GtkObject *object )
{
	Pane *pane;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PANE( object ) );

	pane = PANE( object );

#ifdef DEBUG
	printf( "pane_destroy: %p %s\n",
		pane, pane_handedness2char( pane->handedness ) );
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

	/* Can get here even though we block notify during position set in
	 * animate, because of delays in window setup.
	 */
	if( pane->animate_timeout )
		return;

	g_object_get( pane, 
		"max_position", &max_position, 
		"min_position", &min_position, 
		NULL );

	/* We can have 10,000 as position (meaning way to the
	 * right), take account of any clipping there may be.
	 */
	pane->position = IM_CLIP( min_position, pane->position, max_position );

	/* And the new value.
	 */
	position = gtk_paned_get_position( GTK_PANED( pane ) );

#ifdef DEBUG
	printf( "pane_notify_position_cb: %p %s %d\n", 
		pane, pane_handedness2char( pane->handedness ), position );
#endif /*DEBUG*/

	pane_set_position( pane, position );
	pane_set_user_position( pane, position );

	/* Look for dragged close.
	 */
	if( pane->open &&
		pane->handedness == PANE_HIDE_LEFT && 
		position == min_position ) 
		pane_set_open( pane, FALSE );
	if( pane->open &&
		pane->handedness == PANE_HIDE_RIGHT &&
		position == max_position )
		pane_set_open( pane, FALSE );
}

static void
pane_init( Pane *pane )
{
	pane->handedness = PANE_HIDE_LEFT;
	pane->panechild = NULL;
	pane->open = FALSE;
	pane->position = 0; 		
	pane->user_position = 400; 		/* overwritten on _link() */
	pane->target_position = 0;
	pane->close_on_end = FALSE;
	pane->last_set_position = 0;
	pane->animate_timeout = 0;

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

/* Operations on the model.
 */

void
pane_set_position( Pane *pane, int position )
{
	if( pane->position != position ) {
#ifdef DEBUG
		printf( "pane_set_position: %p %s %d\n",
			pane, pane_handedness2char( pane->handedness ),
			position );
#endif /*DEBUG*/

		g_signal_handlers_block_by_func( pane, 
			pane_notify_position_cb, NULL );
		gtk_paned_set_position( GTK_PANED( pane ), position );
		g_signal_handlers_unblock_by_func( pane, 
			pane_notify_position_cb, NULL );

		pane->position = position;
		pane_changed( pane );
	}
}

void
pane_set_user_position( Pane *pane, int user_position )
{
	if( pane->user_position != user_position ) {
		pane->user_position = user_position;
		pane_changed( pane );
	}
}

void
pane_set_open( Pane *pane, gboolean open )
{
	if( pane->open != open ) {
#ifdef DEBUG
		printf( "pane_set_open: %p %s %d\n",
			pane, pane_handedness2char( pane->handedness ),
			open );
#endif /*DEBUG*/

		widget_visible( GTK_WIDGET( pane->panechild ), open );
		pane->open = open;
		pane_changed( pane );
	}
}

void
pane_set_child( Pane *pane, Panechild *panechild )
{
	g_assert( !pane->panechild );

	pane->panechild = panechild;

	if( pane->handedness == PANE_HIDE_LEFT )
		gtk_paned_pack1( GTK_PANED( pane ), 
			GTK_WIDGET( panechild ), TRUE, TRUE );
	else
		gtk_paned_pack2( GTK_PANED( pane ), 
			GTK_WIDGET( panechild ), TRUE, TRUE );
}

/* Control.
 */

static gboolean
pane_animate_timeout_cb( Pane *pane )
{
	int position = pane->position;
	int target = pane->target_position;

	int new;
	gboolean more;

#ifdef DEBUG
	printf( "pane_animate_timeout_cb: %p %s\n",
		pane, pane_handedness2char( pane->handedness ) );
#endif /*DEBUG*/

	more = TRUE;
	new = position + (target - position) / 2;
	if( ABS( position - target ) < 5 || 
		new == pane->last_set_position ) {
		/* At our target!
		 */
		new = target;
		more = FALSE;
		pane->animate_timeout = 0;
	}

	pane_set_position( pane, new );
	pane->last_set_position = new;

	if( !more &&
		pane->close_on_end )
		pane_set_open( pane, FALSE );

	return( more );
}

/* Close the pane with an animation.
 */
void
pane_animate_closed( Pane *pane )
{
	if( !pane->animate_timeout && 
		pane->open ) {
		int max_position;
		int min_position;
		int target_position;

		target_position = pane_closed_position( pane );
		g_object_get( pane, 
			"max_position", &max_position, 
			"min_position", &min_position, 
			NULL );

		/* Can be zero if we're here very early.
		 */
		if( max_position > 0 ) 
			target_position = 
				IM_CLIP( min_position, 
					target_position, max_position );
		pane->target_position = target_position;
		pane->close_on_end = TRUE;
		pane->last_set_position = -1;

		pane->animate_timeout = g_timeout_add( 50, 
			(GSourceFunc) pane_animate_timeout_cb, pane );
	}
}

/* Open the pane with an animation.
 */
void
pane_animate_open( Pane *pane )
{
	if( !pane->animate_timeout && 
		!pane->open ) {
		int max_position;
		int min_position;
		int target_position;

		target_position = pane->user_position;
		g_object_get( pane, 
			"max_position", &max_position, 
			"min_position", &min_position, 
			NULL );

		/* Can be zero if we're here very early.
		 */
		if( max_position > 0 ) 
			target_position = 
				IM_CLIP( min_position, 
					target_position, max_position );

		/* user_position can be max or min if the pane was dragged
		 * closed.
		 */
		if( target_position == max_position ||
			target_position == min_position )
			target_position = pane_open_position( pane );

		pane->target_position = target_position;
		pane->close_on_end = FALSE;
		pane->last_set_position = -1;
		pane_set_open( pane, TRUE );

		pane->animate_timeout = g_timeout_add( 50, 
			(GSourceFunc) pane_animate_timeout_cb, pane );
	}
}

static void
pane_link( Pane *pane, PaneHandedness handedness )
{
#ifdef DEBUG
	printf( "pane_link: %p %s\n",
		pane, pane_handedness2char( handedness ) );
#endif /*DEBUG*/

	pane->handedness = handedness;
	pane_set_open( pane, FALSE );
}

Pane *
pane_new( PaneHandedness handedness )
{
	Pane *pane;

	pane = PANE( g_object_new( TYPE_PANE, NULL ) );
	pane_link( pane, handedness );

	return( pane );
}
