/* an input pathname ... put/get methods
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

G_DEFINE_TYPE( Pathname, pathname, TYPE_CLASSMODEL ); 

static void
pathname_dispose( GObject *gobject )
{
	Pathname *pathname = PATHNAME( gobject );

#ifdef DEBUG
	printf( "pathname_dispose\n" );
#endif /*DEBUG*/

	IM_FREE( pathname->value );

	G_OBJECT_CLASS( pathname_parent_class )->dispose( gobject );
}

static View *
pathname_view_new( Model *model, View *parent )
{
	return( pathnameview_new() );
}

static void *
pathname_update_model( Heapmodel *heapmodel )
{
#ifdef DEBUG
	printf( "pathname_update_model\n" );
#endif /*DEBUG*/

	if( HEAPMODEL_CLASS( pathname_parent_class )->
		update_model( heapmodel ) )
		return( heapmodel );

	return( NULL );
}

/* Members of pathname we automate.
 */
static ClassmodelMember pathname_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_CAPTION, "caption", N_( "Caption" ),
		G_STRUCT_OFFSET( iObject, caption ) },
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Pathname, value ) }
};

static void
pathname_class_init( PathnameClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = pathname_dispose;

	model_class->view_new = pathname_view_new;

	heapmodel_class->update_model = pathname_update_model;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = pathname_members;
	classmodel_class->n_members = IM_NUMBER( pathname_members );
}

static void
pathname_init( Pathname *pathname )
{
	/* Overridden later. Just something sensible.
	 */
	pathname->value = NULL;
	IM_SETSTR( pathname->value, "no-file" );

	iobject_set( IOBJECT( pathname ), CLASS_PATHNAME, NULL );
}
