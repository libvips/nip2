/* a managedgvalue gvalue 
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

#include "ip.h"

/* 
#define DEBUG
 */

static ManagedClass *parent_class = NULL;

static void
managedgvalue_dispose( GObject *gobject )
{
	Managedgvalue *managedgvalue = MANAGEDGVALUE( gobject );

#ifdef DEBUG
	printf( "managedgvalue_dispose: " );
	iobject_print( IOBJECT( managedgvalue ) );
#endif /*DEBUG*/

	g_value_unset( &managedgvalue->value );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
managedgvalue_info( iObject *iobject, VipsBuf *buf )
{
	Managedgvalue *managedgvalue = MANAGEDGVALUE( iobject );
	char *value_str;

	value_str = g_strdup_value_contents( &managedgvalue->value );
	vips_buf_appendf( buf, "managedgvalue->value = %s\n", value_str );
	g_free( value_str );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );
}

static void
managedgvalue_class_init( ManagedgvalueClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = managedgvalue_dispose;

	iobject_class->info = managedgvalue_info;
}

static void
managedgvalue_init( Managedgvalue *managedgvalue )
{
#ifdef DEBUG
	printf( "managedgvalue_init: %p\n", managedgvalue );
#endif /*DEBUG*/

	memset( &managedgvalue->value, 0, sizeof( GValue ) );
}

GType
managedgvalue_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ManagedgvalueClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) managedgvalue_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Managedgvalue ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) managedgvalue_init,
		};

		type = g_type_register_static( TYPE_MANAGED, 
			"Managedgvalue", &info, 0 );
	}

	return( type );
}

void 
managedgvalue_set_value( Managedgvalue *managedgvalue, GValue *value )
{
	g_value_unset( &managedgvalue->value );
	g_value_init( &managedgvalue->value, G_VALUE_TYPE( value ) );
	g_value_copy( &managedgvalue->value, value );
}

Managedgvalue *
managedgvalue_new( Heap *heap, GValue *value )
{
	Managedgvalue *managedgvalue = g_object_new( TYPE_MANAGEDGVALUE, NULL );

	managed_link_heap( MANAGED( managedgvalue ), heap );
	managedgvalue_set_value( managedgvalue, value );

	/* Not a very good hash.
	 */
	MANAGED( managedgvalue )->hash = (guint) G_VALUE_TYPE( value );

	return( managedgvalue );
}
