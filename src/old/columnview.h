/* view of a column
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

#define TYPE_COLUMNVIEW (columnview_get_type())
#define COLUMNVIEW( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_COLUMNVIEW, Columnview ))
#define COLUMNVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_COLUMNVIEW, ColumnviewClass ))
#define IS_COLUMNVIEW( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_COLUMNVIEW ))
#define IS_COLUMNVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_COLUMNVIEW ))

/* State ... for mouse titlebar interactions.
 */
typedef enum {
	COL_WAIT,		/* Rest state */
	COL_SELECT,		/* Select start, but no drag yet */
	COL_DRAG,		/* Drag state */
        COL_EDIT		/* Editing caption */
} ColumnviewState;

struct _Columnview {
	View view;

	/* Our enclosing workspaceview.
	 */
	Workspaceview *wview;

        /* Display parts.
         */
        GtkWidget *main; 		/* Enclosing window for whole cview */
        GtkWidget *lab;               	/* Columnview name label */
        GtkWidget *vbox;               	/* Outermost vbox for cview */
        GtkWidget *frame;              	/* Enclosing frame for tally stuff */
        GtkWidget *title;              	/* Eventbox wrapper for title bar */
        GtkWidget *titlehb;            	/* Title bar hbox */
        GtkWidget *updown;             	/* Fold up/down arrow */
        GtkWidget *updownb;            	/* Fold up/down button */
        GtkWidget *head;               	/* Label on columnview */
        GtkWidget *headfr;             	/* Frame wrapper around label */
        GtkWidget *text;               	/* Text entry at bottom */
        GtkWidget *textfr;             	/* Enclosing stuff for text entry */
        GtkWidget *capedit;            	/* Shadow text for editing caption */

	/* A shadow for this cview, used during drag to show where this column
	 * will end up.
	 *
	 * And if we are a shadow, the master cview we are the shadow for.
	 */
	Columnview *shadow;
	Columnview *master;

        /* Appearance state info.
         */
        int lx, ly;			/* last pos we set */
        ColumnviewState state;         	/* Waiting or dragging */
        int sx, sy;                     /* Drag start point */
        int rx, ry;                     /* Drag offset */
        int tx, ty;                     /* Tally window pos in root cods */
	gboolean selected;		/* Last drawn in selected state? */

	/* We watch resize events and trigger a workspace relayout with these.
	 */
	int old_width;
	int old_height;
};

typedef struct _ColumnviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} ColumnviewClass;

void columnview_get_position( Columnview *cview, 
	int *x, int *y, int *w, int *h );

GType columnview_get_type( void );
View *columnview_new( void );
