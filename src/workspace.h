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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

/* Three sorts of workspace file load.
 */
typedef enum {
	WORKSPACE_LOAD_TOP,	/* Load as new workspace */
	WORKSPACE_LOAD_COLUMNS,	/* Merge columns into current workspace */
	WORKSPACE_LOAD_ROWS	/* Merge rows into current column */
} WorkspaceLoadType;

/* Save mode ... controls behaviour of column_save_test() and row_save_test()
 */
typedef enum {
	WORKSPACE_SAVE_ALL,	/* Save all rows */
	WORKSPACE_SAVE_SELECTED	/* Only save selected rows */
} WorkspaceSaveType;

/* A workspace.
 */
struct _Workspace {
	Filemodel parent_object;

	/* Our rows are part of this symbol.
	 */
	Symbol *sym;

	/* We are using this set of toolkits.
	 */
	Toolkitgroup *kitg;

	/* State.
	 */
	int next;		/* Index for next column name */
	Column *current;	/* Current column */
	GSList *selected;	/* Rows selected in this workspace */
	GSList *errors;		/* Rows with errors */
        WorkspaceMode mode;	/* Display mode */

	/* Compatibility. 7.8.x used different names for Marks, and had strange
	 * position behaviour
	 */
	gboolean compatibility;	/* In 7.8.x compatibility mode */

	/* Other versions (7.10 etc.) need special compat toolkits. 0 here for
	 * no compat toolkits loaded.
	 */
	int compat_major;
	int compat_minor;

	/* Control load/save for this ws.
	 */
	WorkspaceLoadType load_type;
	WorkspaceSaveType save_type;

	Rect area;		/* Rect enclosing the set of columns */
	Rect vp;		/* Viewport hint ... set by views */
	int window_width;	/* Enclosing window size ... set by views */
	int window_height;

	guint auto_save_timeout;/* Timeout for next autosave */

	char *status;		/* Status message */

	/* Toolkit menus associated with this WS need to know where to show
	 * their error boxes. Our mainws pops their pointers in here to be 
	 * picked up by them.
	 */
	iWindow *iwnd;

	/* Visualisation defaults for this ws.
	 */
	double scale;
	double offset;
};

typedef struct _WorkspaceClass {
	FilemodelClass parent_class;

	/* Methods.
	 */
} WorkspaceClass;

Workspacegroup *workspace_get_workspacegroup( Workspace *ws );

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
	BufInfo *buf, const char *separator );
void workspace_column_names( Column *col, 
	BufInfo *buf, const char *separator );
void workspace_select_all( Workspace *ws );
Column *workspace_is_one_empty( Workspace *ws );

Column *workspace_column_find( Workspace *ws, const char *name );
Column *workspace_column_get( Workspace *ws, const char *name );
char *workspace_column_name_new( Workspace *ws, xmlNode *columns );
Column *workspace_column_pick( Workspace *ws );
void workspace_column_select( Workspace *ws, Column *col );

Symbol *workspace_add_def( Workspace *ws, const char *str );
Symbol *workspace_load_file( Workspace *ws, const char *filename );

gboolean workspace_selected_save( Workspace *ws, const char *filename );
gboolean workspace_clone_selected( Workspace *ws );

void workspace_retain_clean( void );
void workspace_auto_recover( GtkWidget *parent );

GType workspace_get_type( void );
Workspace *workspace_new( Workspacegroup *wsg, const char *name );
Workspace *workspace_new_from_file( Workspacegroup *wsg, const char *fname );
Workspace *workspace_new_from_openfile( Workspacegroup *wsg, iOpenFile *of );
Workspace *workspace_new_blank( Workspacegroup *wsg, const char *name );
gboolean workspace_merge_file( Workspace *ws, const char *fname );
gboolean workspace_merge_column_file( Workspace *ws, const char *fname );

Row *workspace_get_bottom( Workspace *ws );

gboolean workspace_add_action( Workspace *ws, 
	const char *name, const char *action, int nparam );

int workspace_number( void );

Workspace *workspace_clone( Workspace *ws );
gboolean workspace_selected_recalc( Workspace *ws );
void workspace_selected_remove_yesno( Workspace *ws, GtkWidget *parent );
gboolean workspace_selected_ungroup( Workspace *ws );

void workspace_set_status( Workspace *ws, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));

void workspace_set_mode( Workspace *ws, WorkspaceMode mode );

