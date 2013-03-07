/* an input fontname ... put/get methods
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
fontname_finalize( GObject *gobject )
{
	Fontname *fontname = FONTNAME( gobject );

	IM_FREE( fontname->value );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static View *
fontname_view_new( Model *model, View *parent )
{
	return( fontnameview_new() );
}

/* Members of fontname we automate.
 */
static ClassmodelMember fontname_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_CAPTION, "caption", N_( "Caption" ),
		G_STRUCT_OFFSET( iObject, caption ) },
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Fontname, value ) }
};

static void
fontname_class_init( FontnameClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = fontname_finalize;

	model_class->view_new = fontname_view_new;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = fontname_members;
	classmodel_class->n_members = IM_NUMBER( fontname_members );
}

static void
fontname_init( Fontname *fontname )
{
	/* Overridden later. Just something sensible.
	 */
	fontname->value = NULL;
	IM_SETSTR( fontname->value, "Sans" );

	iobject_set( IOBJECT( fontname ), CLASS_FONTNAME, NULL );
}

GType
fontname_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( FontnameClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) fontname_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Fontname ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) fontname_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Fontname", &info, 0 );
	}

	return( type );
}
