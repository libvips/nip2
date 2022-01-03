/* Groups of tools!
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

#define TYPE_TOOLKIT (toolkit_get_type())
#define TOOLKIT( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_TOOLKIT, Toolkit ))
#define TOOLKIT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_TOOLKIT, ToolkitClass))
#define IS_TOOLKIT( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_TOOLKIT ))
#define IS_TOOLKIT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_TOOLKIT ))
#define TOOLKIT_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_TOOLKIT, ToolkitClass ))

/* Toolkits: group definitions with these guys. One toolkit per definition file
 * loaded.
 */
struct _Toolkit {
	Filemodel parent_class;

	Toolkitgroup *kitg;

	/* Set this for auto-generated toolkits (eg. packages of function from
	 * VIPS) ... blocks edit etc. in program window.
	 */
	gboolean pseudo;
};

typedef struct _ToolkitClass {
	FilemodelClass parent_class;

	/* My methods.
	 */
} ToolkitClass;

Tool *toolkit_map( Toolkit *kit, tool_map_fn fn, void *a, void *b );

GType toolkit_get_type( void );

Toolkit *toolkit_find( Toolkitgroup *kitg, const char *name );
Toolkit *toolkit_by_name( Toolkitgroup *kitg, const char *name );

Toolkit *toolkit_new( Toolkitgroup *kitg, const char *filename );
Toolkit *toolkit_new_filename( Toolkitgroup *kitg, const char *filename );
Toolkit *toolkit_new_from_file( Toolkitgroup *kitg, const char *filename );
Toolkit *toolkit_new_from_openfile( Toolkitgroup *kitg, iOpenFile *of );

void *toolkit_linkreport( Toolkit *kit, VipsBuf *buf, gboolean *bad_links );
