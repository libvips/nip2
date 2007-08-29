/* an editable number number
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
number_finalize( GObject *gobject )
{
	Number *number;

#ifdef DEBUG
	printf( "number_finalize\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_NUMBER( gobject ) );

	number = NUMBER( gobject );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static View *
number_view_new( Model *model, View *parent )
{
	return( numberview_new() );
}

/* Members of number we automate.
 */
static ClassmodelMember number_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_CAPTION, "caption", N_( "Caption" ),
		G_STRUCT_OFFSET( iObject, caption ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Number, value ) }
};

static void
number_class_init( NumberClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );


	/* Init methods.
	 */
	gobject_class->finalize = number_finalize;

	model_class->view_new = number_view_new;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = number_members;
	classmodel_class->n_members = IM_NUMBER( number_members );
}

static void
number_init( Number *number )
{
	number->value = 0.0;

	iobject_set( IOBJECT( number ), CLASS_NUMBER, NULL );
}

GType
number_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( NumberClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) number_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Number ),
			32,             /* n_pnumberlocs */
			(GInstanceInitFunc) number_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Number", &info, 0 );
	}

	return( type );
}
