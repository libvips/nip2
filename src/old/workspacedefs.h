/* Workspace-local defs
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

#define TYPE_WORKSPACEDEFS (workspacedefs_get_type())
#define WORKSPACEDEFS( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WORKSPACEDEFS, Workspacedefs ))
#define WORKSPACEDEFS_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_WORKSPACEDEFS, WorkspacedefsClass ))
#define IS_WORKSPACEDEFS( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WORKSPACEDEFS ))
#define IS_WORKSPACEDEFS_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WORKSPACEDEFS ))

struct _Workspacedefs {
	vObject parent_object;

	Workspace *ws;			/* Workspace we explore */

	GtkWidget *text;	
	gboolean changed;		/* Text has been edited */
	gboolean errors;		/* Error on last process */
	guint text_hash;		/* Hash of the last text we set */
	GtkWidget *status;	
};

typedef struct _WorkspacedefsClass {
	vObjectClass parent_class;

} WorkspacedefsClass;

GType workspacedefs_get_type( void );
Workspacedefs *workspacedefs_new( void );
