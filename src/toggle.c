/* a toggle button ... put/get methods
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

#include "ip.h"

/*
#define DEBUG
 */

static ClassmodelClass *parent_class = NULL;

static View *
toggle_view_new( Model *model, View *parent )
{
	return( toggleview_new() );
}

/* Members of toggle we automate.
 */
static ClassmodelMember toggle_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_CAPTION, "caption", N_( "Caption" ),
		G_STRUCT_OFFSET( iObject, caption ) },
	{ CLASSMODEL_MEMBER_BOOLEAN, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Toggle, value ) }
};

static void
toggle_class_init( ToggleClass *class )
{
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	model_class->view_new = toggle_view_new;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = toggle_members;
	classmodel_class->n_members = IM_NUMBER( toggle_members );
}

static void
toggle_init( Toggle *toggle )
{
        toggle->value = FALSE;

	iobject_set( IOBJECT( toggle ), CLASS_TOGGLE, NULL );
}

GType
toggle_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ToggleClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) toggle_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Toggle ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) toggle_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Toggle", &info, 0 );
	}

	return( type );
}
