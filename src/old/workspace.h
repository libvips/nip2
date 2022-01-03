/* Declarations for workspace.c.
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

#define TYPE_WORKSPACE (workspace_get_type())
#define WORKSPACE( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WORKSPACE, Workspace ))
#define WORKSPACE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WORKSPACE, WorkspaceClass))
#define IS_WORKSPACE( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WORKSPACE ))
#define IS_WORKSPACE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WORKSPACE ))
#define WORKSPACE_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WORKSPACE, WorkspaceClass ))

/* Three sorts of workspace file load.
 */
typedef enum {
	WORKSPACE_MODE_REGULAR, /* Vanilla! */
	WORKSPACE_MODE_FORMULA, /* Show formula, not values */
	WORKSPACE_MODE_NOEDIT	/* Shut down all edits */
} WorkspaceMode;

/* A workspace.
 */
struct _Workspace {
	Model parent_object;

	/* Our rows are part of this symbol.
	 */
	Symbol *sym;

	/* We are using this set of toolkits.
	 */
	Toolkitgroup *kitg;

	/* State.
	 */
	int next;		/* Index for next column name */
	GSList *selected;	/* Rows selected in this workspace */
	GSList *errors;		/* Rows with errors */
        WorkspaceMode mode;	/* Display mode */
	gboolean locked;	/* WS has been locked */

	/* The nip version that made this workspace.
	 */
	int major;
	int minor;

	/* We may load some compat definitions to support this workspace, if it
	 * was written by an older version.
	 *
	 * The version number of the compat stuff we loaded. Zero for no compat
	 * stuff loaded.
	 */
	int compat_major;
	int compat_minor;

	/* The last row we scrolled to on next-error.
	 */
	Row *last_error;

	Rect area;		/* Rect enclosing the set of columns */
	Rect vp;		/* Viewport hint ... set by views */
	gboolean lpane_open;	/* Pane model */
	int lpane_position;	
	gboolean rpane_open;
	int rpane_position;

	char *status;		/* Status message */

	/* Visualisation defaults for this ws.
	 */
	double scale;
	double offset;

	/* Workspace-local defs, displayed in the left pane. All in a private
	 * kitg and kit.
	 */
	char *local_defs;
	Toolkitgroup *local_kitg;
	Toolkit *local_kit;

	/* Some view inside this ws has changed and this ws needs a relayout.
	 * Use in_dispose to stop relayout during ws shutdown.
	 */
	gboolean needs_layout;
	gboolean in_dispose;
};

typedef struct _WorkspaceClass {
	ModelClass parent_class;

	/* Methods.
	 */
} WorkspaceClass;

void workspace_set_needs_layout( Workspace *ws, gboolean needs_layout );
GSList *workspace_get_needs_layout();

Workspacegroup *workspace_get_workspacegroup( Workspace *ws );
Workspaceroot *workspace_get_workspaceroot( Workspace *ws );
void workspace_set_modified( Workspace *ws, gboolean modified );

void *workspace_map( workspace_map_fn fn, void *a, void *b );
void *workspace_map_column( Workspace *ws, column_map_fn fn, void *a );
void *workspace_map_symbol( Workspace *ws, symbol_map_fn fn, void *a );
void *workspace_map_view( Workspace *ws, view_map_fn fn, void *a );

gboolean workspace_is_empty( Workspace *ws );

void *workspace_selected_map( Workspace *ws, row_map_fn fn, void *a, void *b );
void *workspace_selected_map_sym( Workspace *ws, 
	symbol_map_fn fn, void *a, void *b );
gboolean workspace_selected_any( Workspace *ws );
int workspace_selected_num( Workspace *ws );
gboolean workspace_selected_sym( Workspace *ws, Symbol *sym );
Row *workspace_selected_one( Workspace *ws );
void workspace_deselect_all( Workspace *ws );
void workspace_selected_names( Workspace *ws, 
	VipsBuf *buf, const char *separator );
void workspace_column_names( Column *col, 
	VipsBuf *buf, const char *separator );
void workspace_select_all( Workspace *ws );
Column *workspace_is_one_empty( Workspace *ws );

Column *workspace_column_find( Workspace *ws, const char *name );
Column *workspace_column_get( Workspace *ws, const char *name );
void workspace_column_name_new( Workspace *ws, char *name );
Column *workspace_column_pick( Workspace *ws );
void workspace_column_select( Workspace *ws, Column *col );
Column *workspace_column_new( Workspace *ws );

Symbol *workspace_add_def( Workspace *ws, const char *str );
Symbol *workspace_add_def_recalc( Workspace *ws, const char *str );
gboolean workspace_load_file_buf( VipsBuf *buf, const char *filename );
Symbol *workspace_load_file( Workspace *ws, const char *filename );

void workspace_get_version( Workspace *ws, int *major, int *minor );
int workspace_have_compat( int major, int minor, 
	int *best_major, int *best_minor );
gboolean workspace_load_compat( Workspace *ws, int major, int minor );

GType workspace_get_type( void );
Workspace *workspace_new( Workspacegroup *wsg, const char *name );
Workspace *workspace_new_blank( Workspacegroup *wsg );

gboolean workspace_add_action( Workspace *ws, 
	const char *name, const char *action, int nparam );

int workspace_number( void );

gboolean workspace_selected_recalc( Workspace *ws );
void workspace_selected_remove_yesno( Workspace *ws, GtkWidget *parent );
gboolean workspace_selected_ungroup( Workspace *ws );
gboolean workspace_selected_group( Workspace *ws );

gboolean workspace_next_error( Workspace *ws );

void workspace_set_status( Workspace *ws, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));

void workspace_set_mode( Workspace *ws, WorkspaceMode mode );

gboolean workspace_local_set( Workspace *ws, const char *txt );
gboolean workspace_local_set_from_file( Workspace *ws, const char *fname );

void workspace_jump_update( Workspace *ws, GtkWidget *menu );

gboolean workspace_merge_file( Workspace *ws, const char *filename );
gboolean workspace_selected_duplicate( Workspace *ws );
gboolean workspace_selected_save( Workspace *ws, const char *filename );

gboolean workspace_rename( Workspace *ws, 
	const char *name, const char *caption );
void workspace_set_locked( Workspace *ws, gboolean locked );
gboolean workspace_duplicate( Workspace *ws );
