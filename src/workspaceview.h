/* a view of a workspace
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

#define TYPE_WORKSPACEVIEW (workspaceview_get_type())
#define WORKSPACEVIEW( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_WORKSPACEVIEW, Workspaceview ))
#define WORKSPACEVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), \
		TYPE_WORKSPACEVIEW, WorkspaceviewClass ))
#define IS_WORKSPACEVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_WORKSPACEVIEW ))
#define IS_WORKSPACEVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_WORKSPACEVIEW ))

struct _Workspaceview {
	View view;

	Workspacegroupview *wgview;

	GtkWidget *fixed;		/* GtkFixed for tally */
	GtkWidget *window;		/* ScrolledWindow holding fixed */

	/* Background window scroll.
	 */
	guint timer;
	int u;				/* Set by columnview for bg scroll */
	int v;

	/* Middle button drag scroll.
	 */
	gboolean dragging;
	int drag_x;
	int drag_y;

	/* Geometry.
	 */
	Rect vp;			/* Viewport pos and size */
	int width;			/* Size of fixed area */
	int height;
	Rect bounding;			/* Bounding box of columnviews */

	/* Placement hints for new columns.
	 */
	int next_x;
	int next_y;

	/* If set, the columnview we know is on the front of the stack.
	 */
	Columnview *front;		

	/* Context we use to change cursor shape.
	 */
	iWindowCursorContext *context;

	/* Follow prefs changes.
	 */
	guint watch_changed_sid;
};

typedef struct _WorkspaceviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} WorkspaceviewClass;

void workspaceview_scroll( Workspaceview *wview, int x, int y, int w, int h );
void workspaceview_scroll_background( Workspaceview *wview, int u, int v );

void workspaceview_set_cursor( Workspaceview *wview, iWindowShape shape );

GtkType workspaceview_get_type( void );
View *workspaceview_new( void );

void workspaceview_select( Workspaceview *wview );

void workspaceview_pick_xy( Workspaceview *wview, int *x, int *y );
