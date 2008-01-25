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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

/*
#define DEBUG
 */

#include "ip.h"

static ClassmodelClass *parent_class = NULL;

static void
pathname_dispose( GObject *gobject )
{
	Pathname *pathname = PATHNAME( gobject );

#ifdef DEBUG
	printf( "pathname_dispose\n" );
#endif /*DEBUG*/

	IM_FREE( pathname->value );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static View *
pathname_view_new( Model *model, View *parent )
{
	return( pathnameview_new() );
}

static void *
pathname_update_model( Heapmodel *heapmodel )
{
	Pathname *pathname = PATHNAME( heapmodel );

#ifdef DEBUG
	printf( "pathname_update_model\n" );
#endif /*DEBUG*/

	if( HEAPMODEL_CLASS( parent_class )->update_model( heapmodel ) )
		return( heapmodel );

	/* Make sure this directory is on the session path.
	 */
	path_add_file( pathname->value );

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

	parent_class = g_type_class_peek_parent( class );

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

GType
pathname_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( PathnameClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) pathname_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Pathname ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) pathname_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Pathname", &info, 0 );
	}

	return( type );
}
