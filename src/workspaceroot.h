/* The root of all workspaces. A singleton all workspaces are children of.
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

#define TYPE_WORKSPACEROOT (workspaceroot_get_type())
#define WORKSPACEROOT( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WORKSPACEROOT, \
		Workspaceroot ))
#define WORKSPACEROOT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WORKSPACEROOT, \
		WorkspacerootClass))
#define IS_WORKSPACEROOT( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WORKSPACEROOT ))
#define IS_WORKSPACEROOT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WORKSPACEROOT ))
#define WORKSPACEROOT_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WORKSPACEROOT, \
		WorkspacerootClass ))

/* A workspaceroot.
 */
struct _Workspaceroot {
	Model parent_object;

	Symbol *sym;			/* Workspace in this group in this */
};

typedef struct _WorkspacerootClass {
	ModelClass parent_class;

	/* Methods.
	 */
} WorkspacerootClass;

GType workspaceroot_get_type( void );

Workspaceroot *workspaceroot_new( const char *name );

void workspaceroot_name_new( Workspaceroot *wsr, char *name );
