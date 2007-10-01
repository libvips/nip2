/* main processing window
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

static GtkHPanedClass *parent_class = NULL;

static void
pane_destroy( GtkObject *object )
{
	Pane *pane;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PANE( object ) );

	pane = PANE( object );

#ifdef DEBUG
	printf( "pane_destroy\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	if( pane->pref ) {
		if( pane->visible )
			/* Re-read the position rather than looking at 
			 * pane_position ... pane_position is only updated on 
			 * hide.
			 */
			prefs_set( pane->pref, "%d", 
				gtk_paned_get_position( GTK_PANED( pane ) ) );
		else
			prefs_set( pane->pref, "%d", pane->position );

		IM_FREEF( g_source_remove, pane->animate_timeout );
		IM_FREE( pane->pref );
	}

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
pane_class_init( PaneClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = pane_destroy;
}

static void
pane_init( Pane *pane )
{
	pane->position = 400; 		/* overwritten on _link() */
	pane->animate_timeout = 0;
	pane->visible = FALSE;
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

static gboolean
pane_animate_timeout_cb( Pane *pane )
{
	int now = gtk_paned_get_position( GTK_PANED( pane ) );
	int target = pane->target_position;
	int new;
	gboolean more;

#ifdef DEBUG
	printf( "pane_animate_timeout_cb\n" );
#endif /*DEBUG*/

	new = now + (target - now) / 2;

	if( ABS( new - target ) < 5 || new == pane->last_set_position ) {
		/* Close enough: set exactly the target and stop animating. Or
		 * the new we set last hasn't taken .. in which case we must
		 * have hit a stop.
		 */
		new = target;
		pane->animate_timeout = 0;
		more = FALSE;
	}
	else 
		/* Keep moving.
		 */
		more = TRUE;

	gtk_paned_set_position( GTK_PANED( pane ), new );
	pane->last_set_position = new;

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

void
pane_set_visible( Pane *pane, gboolean visible )
{
#ifdef DEBUG
	printf( "pane_set_visible: %s\n", bool_to_char( visible ) );
#endif /*DEBUG*/

	if( pane->visible != visible ) {
		pane->visible = visible;

		if( visible ) {
			/* -1 means pick a position from the browser size.
			if( pane->position == -1 )
				pane->position = mainw->wsview->vp.width -
					toolkitbrowser_get_width( 
					mainw->toolkitbrowser );
			 */

			pane_animate( pane, pane->position );
		}
		else {
			pane->position = gtk_paned_get_position( 
				GTK_PANED( pane ) );

			prefs_set( pane->pref, "%d", pane->position );

			pane_animate( pane, pane->hidden_position );
		}
	}
}

static void
pane_link( Pane *pane, const char *pref, int hidden_position )
{
	pane->pref = im_strdupn( pref );
	pane->hidden_position = hidden_position;
	pane->position = watch_int_get( main_watchgroup, pref, 400 );
	pane->visible = FALSE;
	gtk_paned_set_position( GTK_PANED( pane ), hidden_position );
}

Pane *
pane_new( const char *pref, int hidden_position )
{
	Pane *pane;

#ifdef DEBUG
	printf( "pane_new: %s\n", pref );
#endif /*DEBUG*/

	pane = PANE( g_object_new( TYPE_PANE, NULL ) );
	pane_link( pane, pref, hidden_position );

	return( pane );
}
