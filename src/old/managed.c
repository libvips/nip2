/* managed objects ... things like Imageinfo which are lifetime managed by
 * both the GC and by pointers from C: we need to both mark/sweep and refcount
 * 
 * abstract class: Managedgvalue, Imageinfo, etc. build off this
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

/* get -DDEBUG_LEAK from the gcc command-line
#define DEBUG
 */

G_DEFINE_TYPE( Managed, managed, TYPE_ICONTAINER ); 

#ifdef DEBUG_LEAK
static GSList *managed_all = NULL;
#endif /*DEBUG_LEAK*/

#ifdef DEBUG_LEAK
static void *
managed_print_info( Managed *managed, VipsBuf *buf )
{
	iobject_info( IOBJECT( managed ), buf );
	vips_buf_appends( buf, "\n" );

	return( NULL );
}
#endif /*DEBUG_LEAK*/

/* Debugging ... check that all manageds have been closed, dump any which
 * haven't.
 */
void
managed_check_all_destroyed( void )
{
#ifdef DEBUG_LEAK
	if( managed_all ) {
		char txt[1000];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		printf( "managed_check_all_destroyed:\n" );
		slist_map( managed_all, (SListMapFn) managed_print_info, &buf );
		printf( "%s", vips_buf_all( &buf ) );
	}
#endif /*DEBUG_LEAK*/
}

void 
managed_link_heap( Managed *managed, Heap *heap )
{
	g_assert( !managed->heap );

	if( heap == NULL )
		heap = reduce_context->heap;
	managed->heap = heap;
	g_hash_table_insert( heap->mtable, managed, managed );
	managed->attached = TRUE;

	/* The mtable owns our ref.
	 */
	g_object_ref( G_OBJECT( managed ) );
	iobject_sink( IOBJECT( managed ) );
}

static void 
managed_unlink_heap( Managed *managed )
{
	if( managed->attached && managed->heap ) {
		g_hash_table_remove( managed->heap->mtable, managed );
		managed->attached = FALSE;
		g_object_unref( G_OBJECT( managed ) );
	}
}

/* managed no longer depends upon in.
 */
void *
managed_sub_remove( Managed *in, Managed *managed )
{
	g_assert( g_slist_find( managed->sub, in ) );

	managed->sub = g_slist_remove( managed->sub, in );
	managed_destroy_nonheap( in );

	return( NULL );
}

static void
managed_dispose( GObject *gobject )
{
	Managed *managed = MANAGED( gobject );

#ifdef DEBUG
	printf( "managed_dispose: " );
	iobject_print( IOBJECT( managed ) );
#endif /*DEBUG*/

	g_assert( managed->count == 0 );

	managed_unlink_heap( managed );
	slist_map( managed->sub, 
		(SListMapFn) managed_sub_remove, managed );
	g_assert( !managed->sub );

	G_OBJECT_CLASS( managed_parent_class )->dispose( gobject );
}

/* Final death!
 */
static void 
managed_finalize( GObject *gobject )
{
#ifdef DEBUG
	Managed *managed = MANAGED( gobject );

	printf( "managed_finalize:" ); 
	iobject_print( IOBJECT( managed ) );
#endif /*DEBUG*/

#ifdef DEBUG_LEAK
	managed_all = g_slist_remove( managed_all, gobject );
#endif /*DEBUG_LEAK*/

	G_OBJECT_CLASS( managed_parent_class )->finalize( gobject );
}

/* _info() is used by itext.c to display managed objects. Don't chain
 * up, don't print more than one line.
 */
static void
managed_info( iObject *iobject, VipsBuf *buf )
{
#ifdef DEBUG
	Managed *managed = MANAGED( iobject );

	vips_buf_appendf( buf, "managed-object %p\n", managed );
	vips_buf_appendf( buf, "managed->count = %d\n", managed->count );
	vips_buf_appendf( buf, "managed->marked = %d\n", managed->marked );
#endif /*DEBUG*/

	vips_buf_appendf( buf, "%s %p", G_OBJECT_TYPE_NAME( iobject ), iobject );
}

static void
managed_class_init( ManagedClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );

	gobject_class->dispose = managed_dispose;
	gobject_class->finalize = managed_finalize;

	iobject_class->info = managed_info;

	class->keepalive = 0;
}

static void
managed_init( Managed *managed )
{
#ifdef DEBUG
	printf( "managed_init: %p\n", managed );
#endif /*DEBUG*/

	managed->heap = NULL;
	managed->attached = FALSE;

	/* Init to TRUE, so we won't close until (at least) the next GC.
	 */
	managed->marked = TRUE;

	/* Start with a count of zero (unlike gobject!). We will be deleted
	 * on the next GC unless our caller refs us.
	 */
	managed->count = 0;

	/* When we're unreffed, become a zombie first, then destroy after a
	 * (possibly zero) interval.
	 */
	managed->zombie = FALSE;
	managed->time = 0;

	managed->sub = NULL;

#ifdef DEBUG_LEAK
	managed_all = g_slist_prepend( managed_all, managed );
#endif /*DEBUG_LEAK*/
}

/* From heap_gc() ... no heap pointers left, delete if there are no
 * non-heap pointers either.
 */
void
managed_destroy_heap( Managed *managed )
{
#ifdef DEBUG
	printf( "managed_destroy_heap: " );
	iobject_print( IOBJECT( managed ) );
#endif /*DEBUG*/

	/* All non-heaps gone too?
	 */
	if( !managed->count ) 
		IDESTROY( managed );
}

/* destroy() for non-heap pointers.
 */
void *
managed_destroy_nonheap( Managed *managed )
{
	g_assert( managed->count > 0 );

#ifdef DEBUG
	printf( "managed_destroy_nonheap: count = %d ", managed->count );
	iobject_print( IOBJECT( managed ) );
#endif /*DEBUG*/

	managed->count--;

	/* We can't destroy the managed if count == 0 && it's not marked,
	 * since a heap pointer might have been created to it since the last
	 * GC. Queue a GC to clean off stray manageds.
	 */
	heap_gc_request( managed->heap );

	return( NULL );
}

/* Create a new non-heap pointer.
 */
void
managed_dup_nonheap( Managed *managed )
{
	g_assert( managed->count >= 0 );

	managed->count++;

#ifdef DEBUG
	printf( "managed_dup_nonheap: count = %d ", managed->count );
	iobject_print( IOBJECT( managed ) );
#endif /*DEBUG*/
}

/* managed depends on in ... add a dependency.
 */
void 
managed_sub_add( Managed *managed, Managed *in )
{
	g_assert( managed && in );

	managed->sub = g_slist_prepend( managed->sub, in );
	managed_dup_nonheap( in );
}

/* out needs all of in[], add to sub-mark-list.
 */
void 
managed_sub_add_all( Managed *out, int nin, Managed **in )
{
	int i;

	if( out )
		for( i = 0; i < nin; i++ )
			managed_sub_add( out, in[i] );
}


static void 
managed_clear_sub( void *key, Managed *managed )
{
	managed->marked = FALSE;
}

void 
managed_clear( Heap *heap )
{
	g_hash_table_foreach( heap->mtable, 
		(GHFunc) managed_clear_sub, NULL );
}

/* Mark as being used ... also mark all sub-objects.
 */
void 
managed_mark( Managed *managed )
{
	if( !managed->marked ) {
		managed->marked = TRUE;
		(void) slist_map( managed->sub, 
			(SListMapFn) managed_mark, NULL );
	}
}

/* Use a timer to remove unreffed keepalive objects after some
 * interval.
 */
static GTimer *zombie_timer = NULL;
static double zombie_elapsed;

static gboolean 
managed_free_unused_sub( void *key, Managed *managed, gboolean *changed )
{ 
	ManagedClass *managed_class = MANAGED_GET_CLASS( managed );
	Heap *heap = managed->heap;
	gboolean remove = FALSE;

	if( !managed->marked && !managed->count ) {
		if( !managed->zombie ) {
			/* Unreffed, but not marked as a zombie.
			 */
#ifdef DEBUG
			printf( "managed_free: zombiefying: " );
			iobject_print( IOBJECT( managed ) );
#endif /*DEBUG*/

			managed->zombie = TRUE;
			managed->time = zombie_elapsed;
		}
	}
	else {
		if( managed->zombie ) {
			/* Reffed, but marked as a zombie. Back to life again.
			 */
#ifdef DEBUG
			printf( "managed_free: resuscitating: " );
			iobject_print( IOBJECT( managed ) );
#endif /*DEBUG*/
			
			managed->zombie = FALSE;
			managed->time = 0;
		}
	}

	/* Is this an old zombie? Or a not-so-old one and we're flushing?
	 * Junk.
	 */
	if( managed->zombie && 
		zombie_elapsed - managed->time >= managed_class->keepalive ) 
		remove = TRUE;
	if( managed->zombie && heap->flush )
		remove = TRUE;

	if( remove ) {
#ifdef DEBUG
		printf( "managed_free: closing unreferenced object: " );
		iobject_print( IOBJECT( managed ) );
		printf( "managed_free: after %g s as a zombie\n", 
			zombie_elapsed - managed->time );
#endif /*DEBUG*/

		/* We will return TRUE to unlink us from the hash table. Stop 
		 * managed_dispose unlinking for us, and drop the hash table's
		 * reference.
		 */
		managed->attached = FALSE;
		managed_destroy_heap( managed );
		g_object_unref( G_OBJECT( managed ) );

		*changed = TRUE;
	}

	return( remove );
}

/* Make one sweep and destroy all unused managed objects. Return TRUE if we 
 * removed any.
 */
gboolean
managed_free_unused( Heap *heap ) 
{
	gboolean changed;

	if( !zombie_timer )
		zombie_timer = g_timer_new();
	zombie_elapsed = g_timer_elapsed( zombie_timer, NULL );

	changed = FALSE;
	g_hash_table_foreach_remove( heap->mtable, 
		(GHRFunc) managed_free_unused_sub, &changed );

	return( changed );
}
