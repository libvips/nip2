/* an input option ... put/get methods
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
option_finalize( GObject *gobject )
{
	Option *option;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_OPTION( gobject ) );

	option = OPTION( gobject );

	/* My instance finalize stuff.
	 */
	IM_FREEF( slist_free_all, option->labels );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static View *
option_view_new( Model *model, View *parent )
{
	return( optionview_new() );
}

/* Members of option we automate.
 */
static ClassmodelMember option_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_CAPTION, "caption", N_( "Caption" ),
		G_STRUCT_OFFSET( iObject, caption ) },
	{ CLASSMODEL_MEMBER_STRING_LIST, NULL, 0,
		MEMBER_LABELS, "labels", N_( "Labels" ),
		G_STRUCT_OFFSET( Option, labels ) },
	{ CLASSMODEL_MEMBER_INT, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Option, value ) }
};

static void
option_class_init( OptionClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->finalize = option_finalize;

	model_class->view_new = option_view_new;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = option_members;
	classmodel_class->n_members = IM_NUMBER( option_members );
}

static void
option_init( Option *option )
{
        option->labels = NULL;
	option->value = 0;

	iobject_set( IOBJECT( option ), CLASS_OPTION, NULL );
}

GType
option_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( OptionClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) option_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Option ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) option_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Option", &info, 0 );
	}

	return( type );
}
