/* Declarations for workspacegroup.c
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

#define TYPE_WORKSPACEGROUP (workspacegroup_get_type())
#define WORKSPACEGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WORKSPACEGROUP, \
		Workspacegroup ))
#define WORKSPACEGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WORKSPACEGROUP, \
		WorkspacegroupClass))
#define IS_WORKSPACEGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WORKSPACEGROUP ))
#define IS_WORKSPACEGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WORKSPACEGROUP ))
#define WORKSPACEGROUP_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WORKSPACEGROUP, \
		WorkspacegroupClass ))

/* A workspacegroup.
 */
struct _Workspacegroup {
	Model parent_object;

	Symbol *sym;			/* Workspace in this group in this */
};

typedef struct _WorkspacegroupClass {
	ModelClass parent_class;

	/* Methods.
	 */
} WorkspacegroupClass;

GType workspacegroup_get_type( void );

Workspacegroup *workspacegroup_new( const char *name );

void workspacegroup_name_new( Workspacegroup *wsg, char *name );
void workspacegroup_workspace_new( Workspacegroup *wsg, GtkWidget *parent );
