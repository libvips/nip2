/* an input real ... put/get methods
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

static ValueClass *parent_class = NULL;

static void
real_class_init( RealClass *class )
{
	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	model_register_loadable( MODEL_CLASS( class ) );
}

static void
real_init( Real *real )
{
	iobject_set( IOBJECT( real ), CLASS_REAL, NULL );
}

GType
real_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( RealClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) real_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Real ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) real_init,
		};

		type = g_type_register_static( TYPE_VALUE, 
			"Real", &info, 0 );
	}

	return( type );
}
