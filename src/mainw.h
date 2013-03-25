/* A top level window holding some workspaces
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

	/* We make and manage workspaces inside this.
	 */
	Workspacegroup *wsg;

	/* Set of workspace tabs we display.
	 */
	Mainwtab *current_tab;

	/* WS in current tab has changed, plus the ws we attached the 
	 * signal to.
	 */
	guint ws_changed_sid;
	Workspace *ws_changed;

	/* Watch for changed on heap and image, and prefs. Use to update
	 * status bar and space free.
	 */
	guint imageinfo_changed_sid;
	guint heap_changed_sid;
	guint watch_changed_sid;

	/* Link to progress system.
	 */
	guint begin_sid;	
	guint update_sid;	
	guint end_sid;	
	gboolean cancel;

	/* Batch refresh with this, it's slow.
	 */
	guint refresh_timeout;

	/* Display MB free in tmp, or cells free in heap.
	 */
	gboolean free_type;

	/* View menu show/hide toggle states. The pane states are in the ws as
	 * we need to save them to the ws file.
	 */
	gboolean toolbar_visible;
	gboolean statusbar_visible;

	/* The kitg the toolkit menu is currently displaying. Use this to
	 * avoid rebuilding the toolkit menu on every tab switch.
	 */
	Toolkitgroup *kitg;

	/* Component widgets.
	 */
	Toolkitgroupview *kitgview;
	GtkWidget *toolbar;
	GtkWidget *recent_menu;
	GtkWidget *jump_to_column_menu;
	GtkWidget *tab_menu;
	GtkWidget *toolkit_menu;

	GtkWidget *notebook;

	GtkWidget *statusbar_main;
	GtkWidget *statusbar;
	GtkWidget *space_free;	
	GtkWidget *space_free_eb;	
	GtkWidget *progress_box;
	GtkWidget *progress;
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

void mainw_startup( void );
void mainw_shutdown( void );
void mainw_recent_freeze( void );
void mainw_recent_thaw( void );
void mainw_recent_add( GSList **recent, const char *filename );

int mainw_number( void );
Mainw *mainw_pick_one( void );
GType mainw_get_type( void );

void mainw_find_disc( VipsBuf *buf );
void mainw_find_heap( VipsBuf *buf, Heap *heap );
Workspace *mainw_get_workspace( Mainw *mainw );

void mainw_homepage_action_cb( GtkAction *action, iWindow *iwnd );
void mainw_about_action_cb( GtkAction *action, iWindow *iwnd );
void mainw_guide_action_cb( GtkAction *action, iWindow *iwnd );

void mainw_column_new_action_cb( GtkAction *action, Mainw *mainw );
void mainw_workspace_merge_action_cb( GtkAction *action, Mainw *mainw );
void mainw_layout_action_cb( GtkAction *action, Mainw *mainw );
void mainw_group_action_cb( GtkAction *action, Mainw *mainw );
void mainw_next_error_action_cb( GtkAction *action, Mainw *mainw );
void mainw_open_action_cb( GtkAction *action, Mainw *mainw );

Mainwtab *mainw_add_workspace( Mainw *mainw, 
	Mainwtab *old_tab, Workspace *ws, gboolean trim );
Workspace *mainw_open_workspace( Mainw *mainw, 
	const char *filename, gboolean trim, gboolean select );

Mainw *mainw_new( Workspacegroup *wsg );

int mainw_get_n_tabs( Mainw *mainw );
Mainwtab *mainw_get_nth_tab( Mainw *mainw, int i );

