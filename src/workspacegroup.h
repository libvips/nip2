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

/* Workspacegroups: group workspaces with these. One workspacegroup per 
 * file loaded.
 */
struct _Workspacegroup {
	Filemodel parent_class;

	Workspaceroot *wsr;

};

typedef struct _WorkspacegroupClass {
	FilemodelClass parent_class;

	/* My methods.
	 */
} WorkspacegroupClass;

Workspace *workspacegroup_map( Workspacegroup *wsg, 
	workspace_map_fn fn, void *a, void *b );

GType workspacegroup_get_type( void );

Workspacegroup *workspacegroup_new( Workspaceroot *wsr, const char *filename );

Workspacegroup *workspacegroup_new_filename( Workspaceroot *wsr, 
	const char *filename );
Workspacegroup *workspacegroup_new_from_file( Workspaceroot *wsr, 
	const char *filename );
