/* A set of workspaces loaded and saved from a ws file.
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

#define TYPE_WORKSPACEGROUP (workspacegroup_get_type())
#define WORKSPACEGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), \
		TYPE_WORKSPACEGROUP, Workspacegroup ))
#define WORKSPACEGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_WORKSPACEGROUP, WorkspacegroupClass))
#define IS_WORKSPACEGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WORKSPACEGROUP ))
#define IS_WORKSPACEGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WORKSPACEGROUP ))
#define WORKSPACEGROUP_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), \
		TYPE_WORKSPACEGROUP, WorkspacegroupClass ))

/* Three sorts of workspace file load.
 */
typedef enum {
	WORKSPACEGROUP_LOAD_NEW,	/* Load as new workspace */
	WORKSPACEGROUP_LOAD_COLUMNS,	/* Merge into current workspace */
	WORKSPACEGROUP_LOAD_ROWS	/* Merge rows into current column */
} WorkspacegroupLoadType;

/* Save mode ... controls behaviour of column_save_test() and row_save_test()
 */
typedef enum {
	WORKSPACEGROUP_SAVE_ALL,	/* Save everything */
	WORKSPACEGROUP_SAVE_WORKSPACE,	/* Save current workspace */
	WORKSPACEGROUP_SAVE_SELECTED	/* Only save selected rows */
} WorkspacegroupSaveType;

/* Workspacegroups: group workspaces with these. One workspacegroup per 
 * file loaded.
 */
struct _Workspacegroup {
	Filemodel parent_class;

	Workspaceroot *wsr;

	/* Control load/save for this wsg.
	 */
	WorkspacegroupLoadType load_type;
	WorkspacegroupSaveType save_type;

	guint autosave_timeout;

};

typedef struct _WorkspacegroupClass {
	FilemodelClass parent_class;

	/* My methods.
	 */
} WorkspacegroupClass;

void workspacegroup_set_load_type( Workspacegroup *wsg, 
	WorkspacegroupLoadType load_type );
void workspacegroup_set_save_type( Workspacegroup *wsg, 
	WorkspacegroupSaveType save_type );

Workspace *workspacegroup_map( Workspacegroup *wsg, 
	workspace_map_fn fn, void *a, void *b );

GType workspacegroup_get_type( void );

gboolean workspacegroup_is_empty( Workspacegroup *wsg );

Workspacegroup *workspacegroup_new( Workspaceroot *wsr );
Workspacegroup *workspacegroup_new_blank( Workspaceroot *wsr, 
	const char *name );
Workspacegroup *workspacegroup_new_filename( Workspaceroot *wsr, 
	const char *filename );
Workspacegroup *workspacegroup_new_from_file( Workspaceroot *wsr, 
	const char *filename, const char *filename_user );
Workspacegroup *workspacegroup_new_from_openfile( Workspaceroot *wsr, 
	iOpenFile *of );

gboolean workspacegroup_merge_workspaces( Workspacegroup *wsg, 
	const char *filename );
gboolean workspacegroup_merge_columns( Workspacegroup *wsg, 
	const char *filename );
gboolean workspacegroup_merge_rows( Workspacegroup *wsg, 
	const char *filename );

gboolean workspacegroup_save_selected( Workspacegroup *wsg, 
	const char *filename );
gboolean workspacegroup_save_current( Workspacegroup *wsg, 
	const char *filename );
gboolean workspacegroup_save_all( Workspacegroup *wsg, 
	const char *filename );

Workspacegroup *workspacegroup_duplicate( Workspacegroup *wsg );

char *workspacegroup_autosave_recover( void );
void workspacegroup_autosave_clean( void );

