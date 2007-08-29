/* an input slider ... put/get methods
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

static View *
slider_view_new( Model *model, View *parent )
{
	return( sliderview_new() );
}

/* Members of slider we automate.
 */
static ClassmodelMember slider_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_CAPTION, "caption", N_( "Caption" ),
		G_STRUCT_OFFSET( iObject, caption ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		MEMBER_FROM, "from", N_( "From" ),
		G_STRUCT_OFFSET( Slider, from ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		MEMBER_TO, "to", N_( "To" ),
		G_STRUCT_OFFSET( Slider, to ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Slider, value ) }
};

static void
slider_class_init( SliderClass *class )
{
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	model_class->view_new = slider_view_new;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = slider_members;
	classmodel_class->n_members = IM_NUMBER( slider_members );
}

static void
slider_init( Slider *slider )
{
	/* Overridden later. Just something sensible.
	 */
        slider->from = 0;
	slider->to = 255;
	slider->value = 128;

	/* Need to set caption to something too, since it's an automated
	 * member.
	 */
	iobject_set( IOBJECT( slider ), CLASS_SLIDER, "" );
}

GType
slider_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( SliderClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) slider_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Slider ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) slider_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Slider", &info, 0 );
	}

	return( type );
}
