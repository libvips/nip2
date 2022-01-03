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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

*/

#define TYPE_WORKSPACEVIEW (workspaceview_get_type())
#define WORKSPACEVIEW( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WORKSPACEVIEW, Workspaceview ))
#define WORKSPACEVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_WORKSPACEVIEW, WorkspaceviewClass ))
#define IS_WORKSPACEVIEW( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WORKSPACEVIEW ))
#define IS_WORKSPACEVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WORKSPACEVIEW ))

/* Column margins.
 */
#define WORKSPACEVIEW_MARGIN_LEFT (5)
#define WORKSPACEVIEW_MARGIN_TOP (5)

struct _Workspaceview {
	View view;

	GtkWidget *fixed;		/* GtkFixed for tally */
	GtkWidget *window;		/* ScrolledWindow holding fixed */
	Toolkitbrowser *toolkitbrowser;
	Workspacedefs *workspacedefs;
	GtkWidget *label;		/* Tab label */
	GtkWidget *padlock;		/* The padlock icon */
	GtkWidget *alert;		/* The alert icon */

	/* Left and right panes ... program window and toolkit browser.
	 */
	Pane *lpane;
	Pane *rpane;

	GtkWidget *popup;
	GtkWidget *popup_jump;

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

	/* Context we use to change cursor shape.
	 */
	iWindowCursorContext *context;

	/* Follow prefs changes.
	 */
	guint watch_changed_sid;

	/* Only show the compat warning once.
	 */
	gboolean popped_compat;
};

typedef struct _WorkspaceviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} WorkspaceviewClass;

void workspaceview_scroll( Workspaceview *wview, int x, int y, int w, int h );
void workspaceview_scroll_background( Workspaceview *wview, int u, int v );

void workspaceview_set_cursor( Workspaceview *wview, iWindowShape shape );

GType workspaceview_get_type( void );
View *workspaceview_new( void );

void workspaceview_set_label( Workspaceview *wview, 
	GtkWidget *label, GtkWidget *padlock, GtkWidget *alert );
