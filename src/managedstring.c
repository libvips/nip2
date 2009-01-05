/* a managed FILE* ... for lazy file read
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

static ManagedClass *parent_class = NULL;

static void
managedstring_dispose( GObject *gobject )
{
	Managedstring *managedstring = MANAGEDSTRING( gobject );

#ifdef DEBUG
	printf( "managedstring_dispose: " );
	iobject_print( IOBJECT( managedstring ) );
#endif /*DEBUG*/

	IM_FREE( managedstring->string );
	heap_unregister_element( heap, &managedstring->e );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
managedstring_info( iObject *iobject, BufInfo *buf )
{
	Managedstring *managedstring = MANAGEDSTRING( iobject );

	buf_appendf( buf, "managedstring->string = %s\n", 
		managedstring->string );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );
}

static void
managedstring_class_init( ManagedstringClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = managedstring_dispose;

	iobject_class->info = managedstring_info;
}

static void
managedstring_init( Managedstring *managedstring )
{
#ifdef DEBUG
	printf( "managedstring_init: %p\n", managedstring );
#endif /*DEBUG*/

	managedstring->string = NULL;
	managedstring->e.type = ELEMENT_NOVAL;
	managedstring->e.ele = NULL;
	heap_register_element( heap, &string->e );
}

GType
managedstring_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ManagedstringClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) managedstring_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Managedstring ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) managedstring_init,
		}; 
		type = g_type_register_static( TYPE_MANAGED, 
			"Managedstring", &info, 0 );
	}

	return( type );
}

Managedstring *
managedstring_new( Heap *heap, const char *string )
{
	Managedstring *managedstring;
	PElement pe;

#ifdef DEBUG
	printf( "managedstring_new: %p: %s\n", managedstring, string );
#endif /*DEBUG*/

	managedstring = g_object_new( TYPE_MANAGEDSTRING, NULL );
	managed_link_heap( MANAGED( managedstring ), heap );

	PEPOINTE( &pe, &managedstring->e );
	if( !(managedstring->string = im_strdup( NULL, string )) ||
		!heap_string_new( heap, string, &pe ) ) {
		g_object_unref( managedstring );
		return( NULL );
	}

	return( managedstring );
}

