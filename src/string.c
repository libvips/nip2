/* an editable string
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

static ClassmodelClass *parent_class = NULL;

static void
string_finalize( GObject *gobject )
{
	String *string;

#ifdef DEBUG
	printf( "string_finalize\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_STRING( gobject ) );

	string = STRING( gobject );

	IM_FREE( string->value );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static View *
string_view_new( Model *model, View *parent )
{
	return( stringview_new() );
}

/* Members of string we automate.
 */
static ClassmodelMember string_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_CAPTION, "caption", N_( "Caption" ),
		G_STRUCT_OFFSET( iObject, caption ) },
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( String, value ) }
};

static void
string_class_init( StringClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Init methods.
	 */
	gobject_class->finalize = string_finalize;

	model_class->view_new = string_view_new;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = string_members;
	classmodel_class->n_members = IM_NUMBER( string_members );
}

static void
string_init( String *string )
{
	string->value = NULL;
	IM_SETSTR( string->value, "" );

	iobject_set( IOBJECT( string ), CLASS_STRING, NULL );
}

GType
string_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( StringClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) string_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( String ),
			32,             /* n_pstringlocs */
			(GInstanceInitFunc) string_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"String", &info, 0 );
	}

	return( type );
}
