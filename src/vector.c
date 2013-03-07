/* display a vector
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

static ValueClass *parent_class = NULL;

static void
vector_class_init( VectorClass *class )
{
	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
vector_init( Vector *vector )
{
	iobject_set( IOBJECT( vector ), CLASS_VECTOR, NULL );
}

GType
vector_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( VectorClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) vector_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Vector ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) vector_init,
		};

		type = g_type_register_static( TYPE_VALUE, 
			"Vector", &info, 0 );
	}

	return( type );
}
