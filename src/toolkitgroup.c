/* Group toolkitgroup files together.
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

/*
#define DEBUG
 */

#include "ip.h"

G_DEFINE_TYPE( Toolkitgroup, toolkitgroup, TYPE_MODEL ); 

Toolkit *
toolkitgroup_map( Toolkitgroup *kitg, toolkit_map_fn fn, void *a, void *b )
{
	return( (Toolkit *) icontainer_map( ICONTAINER( kitg ), 
		(icontainer_map_fn) fn, a, b ) );
}

static void
toolkitgroup_changed( iObject *iobject )
{
#ifdef DEBUG
	g_print( "toolkitgroup_changed: " );
	iobject_print( iobject );
#endif /*DEBUG*/

	IOBJECT_CLASS( toolkitgroup_parent_class )->changed( iobject );
}

static View *
toolkitgroup_view_new( Model *model, View *parent )
{
	return( toolkitgroupview_new() );
}

static void
toolkitgroup_class_init( ToolkitgroupClass *class )
{
	iObjectClass *iobject_class = (iObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	iobject_class->changed = toolkitgroup_changed;

	model_class->view_new = toolkitgroup_view_new;
}

static void
toolkitgroup_init( Toolkitgroup *kitg )
{
}

static void
toolkitgroup_link( Toolkitgroup *kitg, Symbol *root )
{
	char buf[256];

	g_assert( root );

	kitg->root = root;

	im_snprintf( buf, 256, _( "Toolkits for %s" ),
		IOBJECT( root )->name );
	iobject_set( IOBJECT( kitg ), buf, NULL );
}

Toolkitgroup *
toolkitgroup_new( Symbol *root )
{
	Toolkitgroup *kitg;

	kitg = TOOLKITGROUP( g_object_new( TYPE_TOOLKITGROUP, NULL ) );
	toolkitgroup_link( kitg, root );

	return( kitg );
}

/* Need a special sort function ... put kits not being displayed at the end so
 * they don't mess up the numbering of the visible kits.
 */
static gint
toolkitgroup_sort_compare( Model *a, Model *b )
{
	if( !a->display && b->display )
		return( 1 );
	if( a->display && !b->display )
		return( -1 );

        return( strcasecmp( IOBJECT( a )->name, IOBJECT( b )->name ) );
}

void
toolkitgroup_sort( Toolkitgroup *kitg )
{
	iContainer *icontainer = ICONTAINER( kitg );

        icontainer->children = g_slist_sort( icontainer->children, 
		(GCompareFunc) toolkitgroup_sort_compare );
	icontainer_pos_renumber( icontainer );
}
