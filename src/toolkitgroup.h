/* Declarations for toolkitgroup.c
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

#define TYPE_TOOLKITGROUP (toolkitgroup_get_type())
#define TOOLKITGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_TOOLKITGROUP, Toolkitgroup ))
#define TOOLKITGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_TOOLKITGROUP, \
		ToolkitgroupClass))
#define IS_TOOLKITGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_TOOLKITGROUP ))
#define IS_TOOLKITGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_TOOLKITGROUP ))
#define TOOLKITGROUP_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_TOOLKITGROUP, \
		ToolkitgroupClass ))

/* A toolkitgroup.
 */
struct _Toolkitgroup {
	Model parent_class;

	/* Defs in toolkits in this group are created as locals of this
	 * symbol. This is symbol_root for the main toolkitgroup, but can be
	 * local to a workspace if we are loading a set of compatibility defs.
	 */
	Symbol *root;
};

typedef struct _ToolkitgroupClass {
	ModelClass parent_class;

	/* Methods.
	 */
} ToolkitgroupClass;

Toolkit *toolkitgroup_map( Toolkitgroup *kitg, 
	toolkit_map_fn fn, void *a, void *b );

GType toolkitgroup_get_type( void );

Toolkitgroup *toolkitgroup_new( Symbol *root );

void toolkitgroup_sort( Toolkitgroup *kitg );
