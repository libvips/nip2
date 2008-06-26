/* Declarations for mainw.
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

#define TYPE_MAINW (mainw_get_type())
#define MAINW( obj ) (GTK_CHECK_CAST( (obj), TYPE_MAINW, Mainw ))
#define MAINW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_MAINW, MainwClass ))
#define IS_MAINW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_MAINW ))
#define IS_MAINW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_MAINW ))

/* Get a widget's enclosing Mainw.
 */
#define GET_MAINW( W ) \
	MAINW( idialog_get_root( GTK_WIDGET( W ) ) )

struct _Mainw {
	iWindow parent_object;

	/* Workspace we display.
	 */
	Workspace *ws;
	guint changed_sid;
	guint destroy_sid;

	/* Also watch for changed on heap and image, and prefs.
	 */
	guint imageinfo_changed_sid;
	guint heap_changed_sid;
	guint watch_changed_sid;

	/* Display MB free in tmp, or cells free in heap.
	 */
	gboolean free_type;

	/* View menu show/hide toggle states. The pane states are in the ws as
	 * we need to save them to the ws file.
	 */
	gboolean toolbar_visible;
	gboolean statusbar_visible;

	/* The last row we visited with the 'next-error' button.
	 */
	Row *row_last_error;

	/* Wait before popping up the compat dialog. How stupid, but we have
	 * to make sure our window is on the server before we can show an info
	 * box off it.
	 */
	guint compat_timeout;

	/* Component widgets.
	 */
	Toolkitgroupview *kitgview;
	Toolkitbrowser *toolkitbrowser;
	Workspacedefs *workspacedefs;
	Workspaceview *wsview;
	GtkActionGroup *action_group;
	GtkUIManager *ui_manager;
	GtkWidget *toolbar;
	GtkWidget *recent_menu;
	GtkWidget *jump_to_column_menu;

	GtkWidget *popup;
	GtkWidget *popup_jump;

	GtkWidget *statusbar_main;
	GtkWidget *statusbar;
	GtkWidget *space_free;	
	GtkWidget *space_free_eb;	
	GtkWidget *progress_box;
	GtkWidget *progress;

	/* Left and right panes ... program window and toolkit browser.
	 */
	Pane *lpane;
	Pane *rpane;
};

typedef struct _MainwClass {
	iWindowClass parent_class;

	/* My methods.
	 */
} MainwClass;

extern GSList *mainw_recent_workspace;
extern GSList *mainw_recent_image;
extern GSList *mainw_recent_matrix;

extern gboolean mainw_auto_recalc;

extern gboolean mainw_cancel;

void mainw_progress_set_expr( Expr *expr );
void mainw_progress_end( void );
void mainw_progress_update( int percent, int eta );

void mainw_startup( void );
void mainw_shutdown( void );
void mainw_recent_freeze( void );
void mainw_recent_thaw( void );
void mainw_recent_add( GSList **recent, const char *filename );

int mainw_number( void );
Mainw *mainw_pick_one( void );
GType mainw_get_type( void );
void mainw_about_action_cb( GtkAction *action, iWindow *iwnd );
void mainw_guide_action_cb( GtkAction *action, iWindow *iwnd );
Filemodel *mainw_open_file( Mainw *mainw, const char *filename );
Mainw *mainw_new( Workspace *ws );

