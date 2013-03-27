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

/*
#define DEBUG
 */

#include "ip.h"

static ModelClass *parent_class = NULL;

static void
workspaceroot_dispose( GObject *gobject )
{
	Workspaceroot *wsr;

#ifdef DEBUG
	printf( "workspaceroot_dispose\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WORKSPACEROOT( gobject ) );

	wsr = WORKSPACEROOT( gobject );

	wsr->sym = NULL;

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
workspaceroot_child_add( iContainer *parent, iContainer *child, int pos )
{
	ICONTAINER_CLASS( parent_class )->child_add( parent, child, pos );

#ifdef DEBUG
	printf( "workspaceroot_child_add: added %s\n",
		IOBJECT( child )->name );
#endif /*DEBUG*/
}

static void
workspaceroot_child_remove( iContainer *parent, iContainer *child )
{
	ICONTAINER_CLASS( parent_class )->child_remove( parent, child );
}

static void
workspaceroot_class_init( WorkspacerootClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = workspaceroot_dispose;

	icontainer_class->child_add = workspaceroot_child_add;
	icontainer_class->child_remove = workspaceroot_child_remove;
}

static void
workspaceroot_init( Workspaceroot *wsr )
{
	wsr->sym = NULL;
}

GType
workspaceroot_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( WorkspacerootClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) workspaceroot_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Workspaceroot ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) workspaceroot_init,
		};

		type = g_type_register_static( TYPE_MODEL, 
			"Workspaceroot", &info, 0 );
	}

	return( type );
}

static void
workspaceroot_link( Workspaceroot *wsr, const char *name )
{
	Symbol *sym;

	iobject_set( IOBJECT( wsr ), name, NULL );

	wsr->sym = sym = symbol_new( symbol_root->expr->compile, name );
	sym->type = SYM_WORKSPACEROOT;
	sym->wsr = wsr;
	sym->expr = expr_new( sym );
	(void) compile_new( sym->expr );
	symbol_made( sym );
}

Workspaceroot *
workspaceroot_new( const char *name )
{
	Workspaceroot *wsr;

	if( compile_lookup( symbol_root->expr->compile, name ) ) {
		error_top( _( "Name clash." ) );
		error_sub( _( "Can't create workspaceroot \"%s\". "
			"A symbol with that name already exists." ), name );
		return( NULL );
	}

	wsr = WORKSPACEROOT( g_object_new( TYPE_WORKSPACEROOT, NULL ) );
	workspaceroot_link( wsr, name );

	return( wsr );
}

/* Make up a new workspace name.
 */
void
workspaceroot_name_new( Workspaceroot *wsr, char *name )
{
	Compile *compile = wsr->sym->expr->compile;

	strcpy( name, "untitled" );
	while( compile_lookup( compile, name ) )
		increment_name( name );
}
