/* a side panel that can slide in and out of view
 */

/*

    Copyright (C) 2007 The National Gallery

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

#define TYPE_PANE (pane_get_type())
#define PANE( obj ) (GTK_CHECK_CAST( (obj), TYPE_PANE, Pane ))
#define PANE_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_PANE, PaneClass ))
#define IS_PANE( obj ) (GTK_CHECK_TYPE( (obj), TYPE_PANE ))
#define IS_PANE_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PANE ))

/* Can hide on the left or the right hand side of a window.
 */
typedef enum {
	PANE_HIDE_LEFT,
	PANE_HIDE_RIGHT
} PaneHandedness;

typedef struct _Pane {
	GtkHPaned parent_object;

	PaneHandedness handedness;	/* Hide on left or right */

	/* The child pane we show on left or right.
	 */
	Panechild *panechild;

	/* Are we visible or not.
	 */
	gboolean open;

	/* The position of the divider. This changes as the pane is animated
	 * open and closed and does not reflect the position the user has
	 * selected by dragging.
	 */
	int position;		

	/* The position the user wants the pane to sit at.
	 */
	int user_position;

	/* Animating towards this position. If close_on_end is true, close the
	 * pane at the end of animation.
	 */
	int target_position;
	gboolean close_on_end;

	/* Set animation speed with this.
	 */
	int last_set_position;		

	/* Timeout for animation.
	 */
	guint animate_timeout;
} Pane;

typedef struct _PaneClass {
	GtkHPanedClass parent_class;

	/* Either position or open have changed.
	 */
	void (*changed)( Pane * );
} PaneClass;

GType pane_get_type( void );

Pane *pane_new( PaneHandedness handedness );

void pane_set_position( Pane *pane, int position );
void pane_set_user_position( Pane *pane, int user_position );
void pane_set_open( Pane *pane, gboolean open );
void pane_set_child( Pane *pane, Panechild *panechild );

void pane_animate_closed( Pane *pane );
void pane_animate_open( Pane *pane );

