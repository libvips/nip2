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

/* Track all instances here. 
 */
static GHashTable *managedstring_all = NULL;

#ifdef DEBUG
/* Number of managed strings, number we have expanded to the heap.
 */
int managed_total = 0;
int managed_expanded = 0;
#endif /*DEBUG*/

static void
managedstring_finalize( GObject *gobject )
{
	Managedstring *managedstring = MANAGEDSTRING( gobject );

#ifdef DEBUG
	printf( "managedstring_finalize: \"%s\", ", managedstring->string );
	iobject_print( IOBJECT( managedstring ) );
#endif /*DEBUG*/

#ifdef DEBUG
{
	PElement pe;

	PEPOINTE( &pe, &managedstring->e );
	if( !PEISNOVAL( &pe ) ) 
		managed_expanded -= 1;
	managed_total -= 1;
}
#endif /*DEBUG*/

	heap_unregister_element( MANAGED( managedstring )->heap, 
		&managedstring->e );
	g_hash_table_remove( managedstring_all, managedstring );
	IM_FREE( managedstring->string );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
managedstring_info( iObject *iobject, VipsBuf *buf )
{
	Managedstring *managedstring = MANAGEDSTRING( iobject );

	vips_buf_appendf( buf, "managedstring->string = \"%s\"\n", 
		managedstring->string );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );
}

/* Hash and equality for a managed string: we need the string and the heap to
 * match.
 */
static unsigned int
managedstring_hash( Managedstring *managedstring )
{
	return( g_str_hash( managedstring->string ) | 
		GPOINTER_TO_UINT( ((Managed *) managedstring)->heap ) );
}

static gboolean
managedstring_equal( Managedstring *a, Managedstring *b ) 
{
	return( ((Managed *) a)->heap == ((Managed *) b)->heap &&
		g_str_equal( a->string, b->string ) );
}

static void
managedstring_all_init( void )
{
	if( !managedstring_all )
		managedstring_all = g_hash_table_new( 
			(GHashFunc) managedstring_hash, 
			(GEqualFunc) managedstring_equal );
}

static void
managedstring_class_init( ManagedstringClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );

	parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = managedstring_finalize;

	iobject_class->info = managedstring_info;

	managedstring_all_init();
}

static void
managedstring_init( Managedstring *managedstring )
{
#ifdef DEBUG
	printf( "managedstring_init: %p\n", managedstring );
#endif /*DEBUG*/

#ifdef DEBUG
	managed_total += 1;
#endif /*DEBUG*/

	managedstring->string = NULL;
	managedstring->e.type = ELEMENT_NOVAL;
	managedstring->e.ele = NULL;
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

static Managedstring *
managedstring_new( Heap *heap, const char *string )
{
	Managedstring *managedstring;

#ifdef DEBUG
	printf( "managedstring_new: %p, %s\n", heap, string );
#endif /*DEBUG*/

	/* Disallow "" as string, we want to represent that as [].
	 */
	g_assert( strcmp( string, "" ) != 0 );

	managedstring = g_object_new( TYPE_MANAGEDSTRING, NULL );
	managed_link_heap( MANAGED( managedstring ), heap );
	heap_register_element( heap, &managedstring->e );
	if( !(managedstring->string = im_strdup( NULL, string )) ) 
		return( NULL );

	g_assert( !g_hash_table_lookup( managedstring_all, managedstring ) );
	g_hash_table_insert( managedstring_all, managedstring, managedstring );

	return( managedstring );
}

Managedstring *
managedstring_lookup( Heap *heap, const char *string )
{
	Managedstring managedstring;

	((Managed *) &managedstring)->heap = heap;
	managedstring.string = string;
	managedstring_all_init();

	return( g_hash_table_lookup( managedstring_all, &managedstring ) );
}

Managedstring *
managedstring_find( Heap *heap, const char *string )
{
	Managedstring *managedstring;

	if( !(managedstring = managedstring_lookup( heap, string )) )
		if( !(managedstring = managedstring_new( heap, string )) )
			return( NULL );

	return( managedstring );
}

gboolean
managedstring_get( Managedstring *managedstring, PElement *out )
{
	PElement pe;

	PEPOINTE( &pe, &managedstring->e );
	if( PEISNOVAL( &pe ) ) {
		if( !heap_string_new( MANAGED( managedstring )->heap, 
			managedstring->string, &pe ) ) 
			return( FALSE );

#ifdef DEBUG
		managed_expanded += 1;
		printf( "expanding %s to the heap\n", managedstring->string );
		printf( "\t(%d of %d now expanded)\n", 
			managed_expanded, managed_total );
#endif /*DEBUG*/
	}

	PEPUTE( out, &managedstring->e );

	return( TRUE );
}
