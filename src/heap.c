/* Heap management.
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

/* GC on every alloc too! Extraordinarily slow. Turn on DEBUG_HEAP in ip.h
 * first. Good for spotting heap pointer errors.
#define DEBUG_HEAP_GC
 */

/* Count GCs and %full, handy for tuning.
#define DEBUG_GETMEM
 */

#include "ip.h"

static iObjectClass *parent_class = NULL;

static GSList *heap_all = NULL;

/* Call a function, passing in a "safe" PElement ... ie. the PElement points
 * at a fresh element which will be safe from the GC.
 */
void *
heap_safe_pointer( Heap *heap, heap_safe_pointer_fn fn, 
	void *a, void *b, void *c, void *d )
{
	Element e;
	PElement pe;
	void *result;

	e.type = ELEMENT_NOVAL;
	e.ele = (void *) 5;
	PEPOINTE( &pe, &e );
	heap_register_element( heap, &e );

	result = fn( heap, &pe, a, b, c, d );

	heap_unregister_element( heap, &e );

	return( result );
}

/* Map a function over a piece of graph.
 */
void *
heap_map( HeapNode *hn, heap_map_fn fn, void *a, void *b )
{
	void *c;

	if( !hn )
		return( NULL );

	switch( hn->type ) {
	case TAG_APPL:
	case TAG_CONS:
		if( (c = fn( hn, a, b )) )
			return( c );

		if( GETLT( hn ) == ELEMENT_NODE && 
			(c = heap_map( GETLEFT( hn ), fn, a, b )) )
			return( c );
		if( GETRT( hn ) == ELEMENT_NODE && 
			(c = heap_map( GETRIGHT( hn ), fn, a, b )) )
			return( c );

		return( NULL );

	case TAG_REFERENCE:
	case TAG_COMPLEX:
	case TAG_GEN:
	case TAG_FILE:
	case TAG_CLASS:
	case TAG_DOUBLE:
		return( fn( hn, a, b ) );

	case TAG_SHARED:
		if( (c = fn( hn, a, b )) )
			return( c );

		return( heap_map( GETLEFT( hn ), fn, a, b ) );

	case TAG_FREE:
	default:
		g_assert( FALSE );

		/* Keep gcc happy.
		 */
		return( NULL );
	}
}

/* Debugging ... check that all nodes on the free list are TAG_FREE, and that
 * all other nodes are not TAG_FREE.
 */
static void
heap_check_free( Heap *heap )
{
	HeapNode *hn;
	HeapBlock *hb;

	/* Clear all the DEBUG flags.
	 */
	for( hb = heap->hb; hb; hb = hb->next ) {
		int i;

		for( i = 0; i < hb->sz; i++ ) {
			HeapNode *hn = &hb->node[i];

			hn->flgs &= FLAG_DEBUG ^ FLAG_ALL;
		}
	}

	/* Check free list.
	 */
	for( hn = heap->free; hn; hn = GETLEFT( hn ) ) {
		g_assert( hn->type == TAG_FREE );

		hn->flgs |= FLAG_DEBUG;
	}

	/* Check for all non-free.
	 */
	for( hb = heap->hb; hb; hb = hb->next ) {
		int i;

		for( i = 0; i < hb->sz; i++ ) {
			HeapNode *hn = &hb->node[i];

			g_assert( hn->type != TAG_FREE || 
				(hn->flgs & FLAG_DEBUG) );
		}
	}
}

#ifdef DEBUG_HEAP_GC
static void
heap_check_managed( void *key, void *value, Heap *heap )
{
	/* Validate pointer.
	 */
	(void) MANAGED( value );
}
#endif /*DEBUG_HEAP_GC*/

/* Test for sanity. 
 */
int
heap_sanity( Heap *heap )
{
	heap_check_free( heap );

#ifdef DEBUG_HEAP_GC
	heap_gc( heap );
	heap_check_free( heap );
	g_hash_table_foreach( heap->mtable, (GHFunc) heap_check_managed, heap );
#endif /*DEBUG_HEAP_GC*/

	return( 0 );
}

/* Debugging ... check that all heaps have been closed, dump any which
 * haven't.
 */
void
heap_check_all_destroyed( void )
{
	slist_map( heap_all, (SListMapFn) iobject_dump, NULL );
}

/* Free a HeapBlock.
 */
static void
heapblock_free( HeapBlock *hb )
{
#ifdef DEBUG
	printf( "heapblock_free\n" );
#endif /*DEBUG*/

	if( hb->next )
		heapblock_free( hb->next );
	if( hb->node )
		IM_FREE( hb->node );
	IM_FREE( hb );
}

static void
heap_set_flush( Heap *heap, gboolean flush )
{
	heap->flush = flush;
}

static void 
heap_dispose_print( void *key, void *value )
{ 
	Managed *managed = MANAGED( value );

	iobject_print( IOBJECT( managed ) );
}

static void
heap_dispose( GObject *gobject )
{
	Heap *heap = HEAP( gobject );

	/* Repeatedly close managed objects. Each close can trigger other
	 * closes, so we need to loop until done.
	 */
	managed_clear( heap );
	heap_set_flush( heap, TRUE );
	while( managed_free_unused( heap ) )
		;

	/* Check all managed objects are dead.
	 */
	g_hash_table_foreach( heap->mtable, 
		(GHFunc) heap_dispose_print, NULL );

	IM_FREEF( g_source_remove, heap->gc_tid );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
heap_finalize( GObject *gobject )
{
	Heap *heap = HEAP( gobject );

	if( heap->hb )
		heapblock_free( heap->hb );

	IM_FREEF( g_hash_table_destroy, heap->emark );

	IM_FREEF( g_hash_table_destroy, heap->rmark );

	IM_FREEF( g_hash_table_destroy, heap->mtable );

	heap_all = g_slist_remove( heap_all, heap );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
heap_info( iObject *iobject, BufInfo *buf )
{
	Heap *heap = HEAP( iobject );

	buf_appendf( buf, "compile = " );
	if( heap->compile )
		if( heap->compile->sym ) {
			symbol_qualified_name( heap->compile->sym, buf );
			buf_appendf( buf, "(%p) (sym)\n", heap->compile->sym ); 
		}
		else
			buf_appendf( buf, "(compile, but no sym)\n" );
	else
		buf_appendf( buf, "(no compile)\n" );
	buf_appendf( buf, "mxb (max blocks) = %d\n", heap->mxb );
	buf_appendf( buf, "rsz (nodes per block) = %d\n", heap->rsz );
	buf_appendf( buf, "nb (number of blocks) = %d\n", heap->nb );
	buf_appendf( buf, "emark = %d pointers\n", 
		g_hash_table_size( heap->emark ) );
	buf_appendf( buf, "rmark = %d pointers\n", 
		g_hash_table_size( heap->rmark ) );
	buf_appendf( buf, "ncells (cells allocated) = %d\n", heap->ncells );
	buf_appendf( buf, "nfree (cells free at last GC) = %d\n", heap->nfree );
	buf_appendf( buf, "mtable (Managed blocks) = %d pointers\n", 
		g_hash_table_size( heap->mtable ) );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );
}

/* Empty a heap block.
 */
static void
heapblock_empty( HeapBlock *hb )
{
	int i;

	/* Set as empty free-list.
	 */
	for( i = 0; i < hb->sz; i++ ) {
		HeapNode *hn = &hb->node[i];

		hn->type = TAG_FREE;
		hn->flgs = 0;
		PPUTLEFT( hn, ELEMENT_NODE, hn + 1 );
	}
	PPUTLEFT( &hb->node[hb->sz - 1], ELEMENT_NODE, NULL );
}

/* Add another HeapBlock, if we can.
 */
static gboolean
heapblock_create( Heap *heap, int sz )
{
	HeapBlock *hb;

	if( heap->nb > heap->mxb ) {
		heap->mxb = 1 + (heap->max_fn( heap ) / heap->rsz);
		if( heap->nb > heap->mxb ) 
			/* Hit limit ... caller detects full by ->free becomng
			 * NULL.
			 */
			return( TRUE );
	}

#ifdef DEBUG
	printf( "heapblock_create: new block, size %d\n", sz );
#endif /*DEBUG*/

	if( !(hb = INEW( NULL, HeapBlock )) ) 
		return( FALSE );
	hb->heap = heap;
	hb->next = NULL;
	hb->node = NULL;
	hb->sz = sz;

	if( !(hb->node = IARRAY( NULL, sz, HeapNode )) ) {
		heapblock_free( hb );
		return( FALSE );
	}
	heapblock_empty( hb );

	/* Link to existing blocks.
	 */
	hb->next = heap->hb;
	heap->hb = hb;
	PPUTLEFT( &hb->node[hb->sz - 1], ELEMENT_NODE, heap->free );
	heap->free = &hb->node[0];
	heap->nb++;

	return( TRUE );
}

static void
heap_class_init( HeapClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = heap_dispose;
	gobject_class->finalize = heap_finalize;

	iobject_class->info = heap_info;
}

static void
heap_init( Heap *heap )
{
	heap->compile = NULL;

	heap->max_fn = NULL;
	heap->mxb = -1;
	heap->rsz = 0;
	heap->nb = 0;
	heap->hb = NULL;
	heap->free = NULL;

	heap->ncells = 0;
	heap->nfree = 0;
	heap->serial = 0;
	heap->filled = FALSE;

	heap->emark = g_hash_table_new( NULL, g_direct_equal );
	heap->rmark = g_hash_table_new( NULL, g_direct_equal );
	heap->mtable = g_hash_table_new( NULL, g_direct_equal );

	heap->gc_tid = 0;

	heap->flush = FALSE;

	heap_all = g_slist_prepend( heap_all, heap );
}

GType
heap_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( HeapClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) heap_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Heap ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) heap_init,
		};

		type = g_type_register_static( TYPE_IOBJECT, 
			"Heap", &info, 0 );
	}

	return( type );
}

static void
heap_link( Heap *heap, Compile *compile, heap_max_fn max_fn, int stsz, int rsz )
{
	heap->compile = compile;
	heap->max_fn = max_fn;
	heap->rsz = rsz;

	(void) heapblock_create( heap, stsz );

	if( compile )
		iobject_set( IOBJECT( heap ), 
			IOBJECT( compile->sym )->name, NULL );

	/* Can now set max blocks.
	 */
	heap->mxb = 1 + (heap->max_fn( heap ) / rsz);
}

/* Create an empty heap. mxsz is maximum size of heap in units of nodes, 
 * stsz is start size, rsz is heap growth unit.
 */
Heap *
heap_new( Compile *compile, heap_max_fn max_fn, int stsz, int rsz )
{
	Heap *heap;

	heap = HEAP( g_object_new( TYPE_HEAP, NULL ) );
	heap_link( heap, compile, max_fn, stsz, rsz );

	return( heap );
}

/* Set flags on a heap.
 */
void
heap_set( Heap *heap, NodeFlags setmask )
{
	HeapBlock *hb;
	int i;

	for( hb = heap->hb; hb; hb = hb->next ) 
		for( i = 0; i < hb->sz; i++ )
			hb->node[i].flgs |= setmask;
}

/* Clear flags on a heap.
 */
void
heap_clear( Heap *heap, NodeFlags clearmask )
{
	HeapBlock *hb;
	int i;
	int cmask = clearmask ^ FLAG_ALL;

	for( hb = heap->hb; hb; hb = hb->next ) 
		for( i = 0; i < hb->sz; i++ )
			hb->node[i].flgs &= cmask;
}

/* Allocate a new serial number for a heap. On return, we guarantee that 
 * heap->serial is a value not used by any nodes in the heap.
 */
int
heap_serial_new( Heap *heap )
{
	heap->serial += 1;
	if( heap->serial > FLAG_SERIAL ) {
		heap->serial = 1;
		heap_clear( heap, FLAG_SERIAL );
	}

	return( heap->serial );
}

/* Mark a tree. Avoid recursion because of the danger of C stack overflow on 
 * large heaps.
 */
static void 
heap_mark_tree( Heap *heap, HeapNode *hn )
{
	GSList *pending = NULL;

	pending = g_slist_prepend( pending, hn );

	while( pending ) {
		hn = (HeapNode *) pending->data;
		pending = g_slist_remove( pending, hn );

		/* Chase down the LHS of the nodes, add the RHS nodes we pass 
		 * to the pending list.
		 */
		for(;;) {
			if( hn->flgs & FLAG_MARK ) 
				break;

			hn->flgs |= FLAG_MARK;

			/* Don't modify hn for the do-nothing case: we'll
			 * break on the next loop.
			 */
			switch( hn->type ) {
			case TAG_GEN:
			case TAG_COMPLEX:
			case TAG_CLASS:
			case TAG_APPL:
			case TAG_CONS:
				if( GETRT( hn ) == ELEMENT_MANAGED ) 
					managed_mark( (Managed *) 
						GETRIGHT( hn ) );
				if( GETLT( hn ) == ELEMENT_MANAGED ) 
					managed_mark( (Managed *) 
						GETLEFT( hn ) );

				if( GETRT( hn ) == ELEMENT_NODE ) {
					if( GETLT( hn ) == ELEMENT_NODE ) {
						pending = g_slist_prepend( 
							pending, 
							GETRIGHT( hn ) );
						hn = GETLEFT( hn );
					}
					else
						hn = GETRIGHT( hn );
				}
				else if( GETLT( hn ) == ELEMENT_NODE ) 
					hn = GETLEFT( hn );

				break;

			case TAG_FILE:
				g_assert( GETLT( hn ) == ELEMENT_MANAGED );
				managed_mark( (Managed *) GETLEFT( hn ) );
				break;

			case TAG_DOUBLE:
				break;

			case TAG_SHARED:
			case TAG_REFERENCE:
				if( GETLT( hn ) == ELEMENT_NODE ) 
					hn = GETLEFT( hn );
				break;

			case TAG_FREE:
			default:
				g_assert( FALSE );
			}
		}
	}
}

/* Mark an element.
 */
static void *
mark_pelement( PElement *base, Heap *heap )
{
	if( PEISMANAGED( base ) )
		managed_mark( MANAGED( PEGETVAL( base ) ) );
	else if( PEISNODE( base ) ) 
		heap_mark_tree( heap, PEGETVAL( base ) );

	return( NULL );
}

/* Mark an element.
 */
static void 
mark_element( void *key, void *value, Heap *heap )
{
	Element *root = (Element *) value;
	PElement base;

	PEPOINTE( &base, root );
	(void) mark_pelement( &base, heap ); 
}

/* Mark a reduce context ... the heapnodes on the spine stack etc.
 */
static void *
mark_reduce( void *key, void *value, Heap *heap )
{
	Reduce *rc = (Reduce *) value;
	int i;

#ifdef DEBUG
	printf( "mark_reduce: marking %d stack elements\n", rc->sp );
#endif /*DEBUG*/

	for( i = 0; i < rc->sp; i++ )
		heap_mark_tree( heap, rc->nstack[i] );

	return( NULL );
}

/* Do a garbage collect.
 */
gboolean
heap_gc( Heap *heap )
{
	HeapBlock *hb;
	int nfree;
	int ncells;
	int nblocks;

#ifdef DEBUG
	static GTimer *GC_timer = NULL;

	if( !GC_timer )
		GC_timer = g_timer_new();

	g_timer_reset( GC_timer );

	printf( "heap_gc: starting GC for heap %s\n", IOBJECT( heap )->name );
#endif /*DEBUG*/

	/* Clear marks on managed objects. Nodes should all be clear already.
	 */
	managed_clear( heap );

	/* All flags should be clear, so just mark.
	 */
	g_hash_table_foreach( heap->emark, (GHFunc) mark_element, heap );
	g_hash_table_foreach( heap->rmark, (GHFunc) mark_reduce, heap );

	/* And sweep up unmarked into new free list.
	 */
	heap->free = NULL;
	ncells = nfree = nblocks = 0;
	for( hb = heap->hb; hb; hb = hb->next ) {
		const int sz = hb->sz;
		int i;

		for( i = 0; i < sz; i++ ) {
			HeapNode * const hn = &hb->node[i];

			if( !(hn->flgs & FLAG_MARK) ) {
				hn->type = TAG_FREE;
				PPUTLEFT( hn, ELEMENT_NODE, heap->free );
#ifdef DEBUG_HEAP_GC
				/* Not necessary, but may be helpful to zap
				 * any pointer in there.
				 */
				PPUTRIGHT( hn, ELEMENT_NODE, NULL );
#endif /*DEBUG_HEAP_GC*/
				heap->free = hn;
				nfree += 1;
			}

			hn->flgs &= FLAG_MARK ^ FLAG_ALL;
		}

		ncells += hb->sz;
		nblocks += 1;
	}
	heap->ncells = ncells;
	heap->nfree = nfree;

	/* Close unused managed objects. It can (potentially) take a couple of
	 * passes through mtable to free everything ... but we'll do more on
	 * the next GC.
	 */
	managed_free_unused( heap );

#ifdef DEBUG
	printf( "heap_gc: %d cells in %d blocks, %d in use\n",
		ncells, nblocks, ncells - nfree );
	printf( "(GC took %gs)\n",  g_timer_elapsed( GC_timer, NULL ) );
#endif 

	return( TRUE );
}

static gint
heap_gc_request_cb( Heap *heap )
{
	heap->gc_tid = 0;

	if( !heap_gc( heap ) )
		printf( "help! delayed GC failed!\n" );

	iobject_changed( IOBJECT( heap ) );

	return( FALSE );
}

/* Request a delayed garbage collect.
 */
void
heap_gc_request( Heap *heap )
{
	IM_FREEF( g_source_remove, heap->gc_tid );
	heap->gc_tid = g_timeout_add( 1000, 
		(GSourceFunc) heap_gc_request_cb, heap );
}

/* Register a pointer into a heap.
 */
void
heap_register_element( Heap *heap, Element *root )
{
	g_hash_table_insert( heap->emark, root, root );
}

/* Unregister a pointer into a heap.
 */
void
heap_unregister_element( Heap *heap, Element *root )
{
	if( g_hash_table_remove( heap->emark, root ) ) {
#ifdef DEBUG
		printf( "heap_unregister_element: %d pointers\n",
			g_hash_table_size( heap->emark ) );
#endif 
	}
}

/* Register a Reduce working on this heap.
 */
void
heap_register_reduce( Heap *heap, Reduce *rc )
{
	g_hash_table_insert( heap->rmark, rc, rc );
}

/* Unregister a reduce context.
 */
void
heap_unregister_reduce( Heap *heap, Reduce *rc )
{
	g_hash_table_remove( heap->rmark, rc );
}

/* Allocate a new HeapNode ... long version. See NEWNODE() macro.
 */
HeapNode *
heap_getmem( Heap *heap ) 
{
	HeapNode *hn;
	int pcused;
#ifdef DEBUG_GETMEM
	static int n_heap_getmem = 0;
#endif /*DEBUG_GETMEM*/

	/* Easy case ... this should be handled by the NEWNODE macro, but do
	 * it here as well just in case.
	 */
	if( heap->free ) {
		EXTRACTNODE( heap, hn );
		return( hn );
	}

#ifdef DEBUG
	printf( "heap_getmem: GC on full heap for heap %s\n", 
		IOBJECT( heap )->name );
#endif /*DEBUG*/

	/* Try a GC.
	 */
	if( !heap_gc( heap ) )
		return( NULL );

	/* Is heap over x% full? Add another heap block if we can.
	 */
	pcused = 100 * (heap->ncells - heap->nfree) / heap->ncells;
#ifdef DEBUG_GETMEM
	n_heap_getmem += 1;
	printf( "heap_getmem: %d%% (%d)\n", pcused, n_heap_getmem );
#endif /*DEBUG_GETMEM*/

	if( pcused > 50 ) {
		int nblocks = 1 + (heap->ncells - heap->nfree) / heap->rsz;
		int i;

#ifdef DEBUG_GETMEM
		printf( "heap_getmem: %d more blocks added\n", nblocks );
#endif /*DEBUG_GETMEM*/
		for( i = 0; i < nblocks; i++ )
			if( !heapblock_create( heap, heap->rsz ) )
				return( NULL );
	}

	if( !heap->free ) {
		error_top( _( "Heap full." ) );
		if( heap->compile ) {
			BufInfo buf;
			char txt[100];

			buf_init_static( &buf, txt, 100 );
			compile_name( heap->compile, &buf );
			error_sub( _( "The compile heap for %s has filled. "
				"Make it smaller and less complicated." ),
				buf_all( &buf ) );
		}
		else
			error_sub( _( "The main calculation heap has filled. "
				"Raise the heap size limit in Preferences." ) );
		heap->filled = TRUE;
		return( NULL );
	}

	EXTRACTNODE( heap, hn );

	return( hn );
}

gboolean
heap_bool_new( Heap *heap, gboolean val, PElement *out )
{
	PEPUTP( out, ELEMENT_BOOL, val );

	return( TRUE );
}

/* Write a real to an element.
 */
gboolean
heap_real_new( Heap *heap, double in, PElement *out )
{
	HeapNode *hn;

	if( NEWNODE( heap, hn ) )
		return( FALSE );
	hn->type = TAG_DOUBLE;
	hn->body.num = in;

	PEPUTP( out, ELEMENT_NODE, hn );

	return( TRUE );
}

/* Write an element to an element.
 */
gboolean
heap_element_new( Heap *heap, Element *e, PElement *out )
{
	PEPUTE( out, e );

	return( TRUE );
}

/* Make a complex node from two elements. 
 */
gboolean
heap_complex_element_new( Heap *heap, 
	PElement *rp, PElement *ip, PElement *out )
{
	HeapNode *hn;

	if( NEWNODE( heap, hn ) )
		return( FALSE );
	hn->type = TAG_COMPLEX;
	PPUT( hn, PEGETTYPE( rp ), PEGETVAL( rp ), 
		PEGETTYPE( ip ), PEGETVAL( ip ) ); 

	PEPUTP( out, ELEMENT_NODE, hn );

	return( TRUE );
}

/* Make a complex node.
 */
gboolean
heap_complex_new( Heap *heap, double rp, double ip, PElement *out )
{
	Element dummy;
	PElement t;

	/* Form complex node.
	 */
	dummy.type = ELEMENT_NOVAL;
	dummy.ele = (void *) 6;
	PEPOINTE( &t, &dummy );
	if( !heap_complex_element_new( heap, &t, &t, out ) )
		return( FALSE );

	/* Install real and imag parts.
	 */
	PEPOINTLEFT( PEGETVAL( out ), &t );
	if( !heap_real_new( heap, rp, &t ) )
		return( FALSE );
	PEPOINTRIGHT( PEGETVAL( out ), &t );
	if( !heap_real_new( heap, ip, &t ) )
		return( FALSE );

	return( TRUE );
}

/* Set list to [].
 */
void
heap_list_init( PElement *list )
{
	PEPUTP( list, ELEMENT_ELIST, NULL );
}

/* Add new node to list, point data at new CONS LHS.
 */
gboolean
heap_list_add( Heap *heap, PElement *list, PElement *data )
{
	HeapNode *hn;

	/* Build CONS node.
	 */
	if( NEWNODE( heap, hn ) )
		return( FALSE );
	hn->type = TAG_CONS;
	PPUTLEFT( hn, ELEMENT_NOVAL, (void *) 7 );
	PEPUTRIGHT( hn, list );
	PEPUTP( list, ELEMENT_NODE, hn );

	/* Point data to new LHS.
	 */
	PEPOINTLEFT( hn, data );

	return( TRUE );
}

/* Move list on to the next RHS. list points at [], or pointer to next node.
 * Used with heap_list_init()/heap_list_add() to build lists.
 */
gboolean
heap_list_next( PElement *list )
{
	HeapNode *hn = PEGETVAL( list );

	if( hn ) {
		PEPOINTRIGHT( hn, list );
		return( TRUE );
	}
	else
		return( FALSE );
}

gboolean
heap_list_cat( Reduce *rc, PElement *a, PElement *b, PElement *out )
{
	PElement list = *out;

	REDUCE_CATCH_START( FALSE );
	reduce_clone_list( rc, a, &list );
	PEPUTPE( &list, b );
	REDUCE_CATCH_STOP;

	return( TRUE );
}

/* Start off a function application.
 */
void
heap_appl_init( PElement *base, PElement *func )
{
	PEPUTPE( base, func );
}

/* Add a new parameter to a function application. base points at the
 * function built so far ... update base to point to new node (old base
 * becomes LHS), return parm pointing to new RHS
 */
gboolean
heap_appl_add( Heap *heap, PElement *base, PElement *parm )
{
	HeapNode *hn;

	/* Build appl node.
	 */
	if( NEWNODE( heap, hn ) )
		return( FALSE );
	hn->type = TAG_APPL;
	PEPUTLEFT( hn, base );
	PPUTRIGHT( hn, ELEMENT_ELIST, NULL );
	PEPUTP( base, ELEMENT_NODE, hn );

	/* Point parm to new RHS.
	 */
	PEPOINTRIGHT( hn, parm );

	return( TRUE );
}

/* Make a lazy file read node.
 */
gboolean
heap_file_new( Heap *heap, const char *filename, PElement *out )
{
	Managedfile *managedfile;
	HeapNode *hn;

	if( !(managedfile = managedfile_new( heap, filename )) )
		return( FALSE );

	/* Make sure the managedfile survives a GC.
	 */
	MANAGED_REF( managedfile );

	if( NEWNODE( heap, hn ) ) {
		MANAGED_UNREF( managedfile );
		return( FALSE );
	}
	hn->type = TAG_FILE;
	PPUT( hn, 
		ELEMENT_MANAGED, managedfile,
		ELEMENT_ELIST, NULL );
	PEPUTP( out, ELEMENT_NODE, hn );

	MANAGED_UNREF( managedfile );

	return( TRUE );
}

/* Make a heap string.
 */
gboolean
heap_string_new( Heap *heap, const char *str, PElement *out )
{
	PElement list = *out;
	const int n = strlen( str );
	int i;

	heap_list_init( &list ); 

	for( i = 0; i < n; i++ ) {
		PElement t;

		if( !heap_list_add( heap, &list, &t ) )
			return( FALSE );
		PEPUTP( &t, ELEMENT_CHAR, (int) str[i] );
		(void) heap_list_next( &list );
	}

	return( TRUE );
}

/* Make a managed string.
 */
gboolean
heap_managedstring_new( Heap *heap, const char *str, PElement *out )
{
	Managedstring *managedstring;
		
	if( !(managedstring = managedstring_find( heap, str )) )
		return( FALSE );
	PEPUTP( out, ELEMENT_MANAGED, managedstring );

	return( TRUE );
}

/* Make a [[char]].
 */
gboolean
heap_lstring_new( Heap *heap, GSList *labels, PElement *out )
{
	PElement list = *out;
	const int n = g_slist_length( labels );
	int i;

	/* Make first RHS ... the end of the list. 
	 */
	heap_list_init( &list ); 

	/* Build a CONS node for each element. 
	 */
	for( i = 0; i < n; i++ ) {
		PElement t;

		if( !heap_list_add( heap, &list, &t ) ||
			!heap_managedstring_new( heap, 
				g_slist_nth_data( labels, i ), &t ) )
			return( FALSE );
		(void) heap_list_next( &list );
	}

	return( TRUE );

}

/* Make a realvec.
 */
gboolean
heap_realvec_new( Heap *heap, int n, double *vec, PElement *out )
{
	PElement list = *out;
	int i;

	/* Make first RHS ... the end of the list. 
	 */
	heap_list_init( &list );

	/* Build a CONS node for each element. 
	 */
	for( i = 0; i < n; i++ ) {
		PElement t;

		if( !heap_list_add( heap, &list, &t ) )
			return( FALSE );
		if( !heap_real_new( heap, vec[i], &t ) )
			return( FALSE );
		(void) heap_list_next( &list );
	}

	return( TRUE );
}

/* Make a realvec, but from an int*.
 */
gboolean
heap_intvec_new( Heap *heap, int n, int *vec, PElement *out )
{
	PElement list = *out;
	int i;

	/* Make first RHS ... the end of the list. 
	 */
	heap_list_init( &list );

	/* Build a CONS node for each element. 
	 */
	for( i = 0; i < n; i++ ) {
		PElement t;

		if( !heap_list_add( heap, &list, &t ) )
			return( FALSE );
		if( !heap_real_new( heap, (double) vec[i], &t ) )
			return( FALSE );
		(void) heap_list_next( &list );
	}

	return( TRUE );
}

/* Make a matrix.
 */
gboolean
heap_matrix_new( Heap *heap, 
	int xsize, int ysize, double *vec, PElement *out )
{
	PElement list = *out;
	int y, i;

	/* Make first RHS ... the end of the list. 
	 */
	heap_list_init( &list );

	/* Build a CONS node for each element. 
	 */
	for( i = 0, y = 0; y < ysize; y++ ) {
		PElement t;

		if( !heap_list_add( heap, &list, &t ) )
			return( FALSE );
		if( !heap_realvec_new( heap, xsize, vec + i, &t ) )
			return( FALSE );
		i += xsize;
		(void) heap_list_next( &list );
	}

	return( TRUE );
}

/* Make a typecheck error. Always return FALSE ... the gboolean is just there
 * for REDUCE_CATCH.
 */
gboolean
heap_error_typecheck( PElement *e, const char *name, const char *type )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	(void) reduce_error_typecheck( reduce_context, e, name, type );
	REDUCE_CATCH_STOP; 

	return( FALSE );
}

/* Map over a heap list. Reduce the list spine as we go, don't reduce the
 * heads. Return base on error, or whatever the user function returns (unlike
 * reduce_map_list(), which we can't just wrap).
 */
void *
heap_map_list( PElement *base, heap_map_list_fn fn, void *a, void *b )
{
	Reduce *rc = reduce_context;
	PElement e = *base;

	if( !reduce_pelement( rc, reduce_spine, &e ) )
		return( base );

	if( !PEISLIST( &e ) ) {
		heap_error_typecheck( &e, "heap_map_list", "list" );
		return( base );
	}

	while( PEISFLIST( &e ) ) {
		PElement head;
		void *res;

		/* Apply user function to the head.
		 */
		PEGETHD( &head, &e );
		if(( res = fn( &head, a, b )) )
			return( res );

		/* Reduce the tail.
		 */
		PEGETTL( &e, &e );
		if( !reduce_pelement( rc, reduce_spine, &e ) )
			return( base );
	}

	return( NULL );
}

/* Iterate over a list. Move list on to the next tl, point data at the
 * head of the current node, FALSE for []. 
 */
gboolean
heap_get_list_next( PElement *list, PElement *data )
{
	Reduce *rc = reduce_context;

	if( !reduce_pelement( rc, reduce_spine, list ) )
		return( FALSE );

	if( PEISFLIST( list ) ) {
		HeapNode *hn = PEGETVAL( list );

		PEPOINTRIGHT( hn, list );
		PEPOINTLEFT( hn, data );

		return( TRUE );
	}
	else
		return( FALSE );
}

typedef struct _HeapMapDict {
	heap_map_dict_fn fn;
	void *a;
	void *b;
} HeapMapDict;

static void *
heap_map_dict_entry( PElement *head, HeapMapDict *map_dict )
{
	Reduce *rc = reduce_context;
	char key[256];
	PElement p1, p2;
	void *result;

	if( !reduce_pelement( rc, reduce_spine, head ) )
		return( head );
	if( !PEISFLIST( head ) ) {
		heap_error_typecheck( head, "heap_map_dict", "list" );
		return( head );
	}
	PEGETHD( &p1, head );
	if( !heap_get_string( &p1, key, 256 ) )
		return( head );

	PEGETTL( &p2, head );
	if( !reduce_pelement( rc, reduce_spine, &p2 ) )
		return( head );
	if( !PEISFLIST( &p2 ) ) {
		heap_error_typecheck( &p2, "heap_map_dict", "list" );
		return( head );
	}
	PEGETHD( &p1, &p2 );
	if( (result = map_dict->fn( key, &p1, map_dict->a, map_dict->b )) )
		return( result );

	PEGETTL( &p1, &p2 );
	if( !reduce_pelement( rc, reduce_spine, &p1 ) )
		return( head );
	if( !PEISELIST( &p1 ) ) {
		heap_error_typecheck( &p1, "heap_map_dict", "[]" );
		return( head );
	}

	return( NULL );
}

/* Map over a list of ["key", value] pairs.
 */
void *
heap_map_dict( PElement *base, heap_map_dict_fn fn, void *a, void *b )
{
	HeapMapDict map_dict;

	map_dict.fn = fn;
	map_dict.a = a;
	map_dict.b = b;

	return( heap_map_list( base, 
		(heap_map_list_fn) heap_map_dict_entry, &map_dict, NULL ) );
}

/* Evaluate a PElement into a string buffer. 
 */
gboolean
heap_get_string( PElement *base, char *buf, int n )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	(void) reduce_get_string( reduce_context, base, buf, n );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

/* Evaluate a PElement to a [[char]]. 
 */
gboolean
heap_get_lstring( PElement *base, GSList **labels )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	(void) reduce_get_lstring( reduce_context, base, labels );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

/* Get an element as a bool. 
 */
gboolean
heap_get_bool( PElement *base, gboolean *out )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_get_bool( reduce_context, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

/* Get an element as a real. 
 */
gboolean
heap_get_real( PElement *base, double *out )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_get_real( reduce_context, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

/* Get an element as a class ... just reduce and typecheck.
 */
gboolean
heap_get_class( PElement *base, PElement *out )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	reduce_get_class( reduce_context, base );
	REDUCE_CATCH_STOP; 

	/* Point out at base ... for consistency with other getters.
	 */
	*out = *base;

	return( TRUE );
}

/* Get an element as an image.
 */
gboolean
heap_get_image( PElement *base, Imageinfo **out )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_get_image( reduce_context, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

/* Get an element as a realvec. Return -1 on error, or length of vector.
 */
int
heap_get_realvec( PElement *base, double *buf, int n )
{
	Reduce *rc = reduce_context;
	int l;

	REDUCE_CATCH_START( -1 );
	l = reduce_get_realvec( reduce_context, base, buf, n );
	REDUCE_CATCH_STOP; 

	return( l );
}

/* Get an element as a matrix. Return -1 on error, or length of buffer used. 
 * Write xsize/ysize to args.
 */
gboolean
heap_get_matrix_size( PElement *base, int *xsize, int *ysize )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	(void) reduce_get_matrix_size( reduce_context, base, xsize, ysize );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

/* Get an element as a matrix. Return -1 on error, or length of buffer used. 
 * Write xsize/ysize to args.
 */
gboolean
heap_get_matrix( PElement *base, double *buf, int n, int *xsize, int *ysize )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	(void) reduce_get_matrix( reduce_context, base, buf, n, xsize, ysize );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

gboolean 
heap_is_elist( PElement *base, gboolean *out ) 
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_is_elist( rc, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

gboolean 
heap_is_list( PElement *base, gboolean *out ) 
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_is_list( rc, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

/* Do a get, check it's OK. We don't get very much, in case it's a long
 * string and will take a while to eval.
 */
gboolean 
heap_is_string( PElement *base, gboolean *out ) 
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_is_string( rc, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

gboolean 
heap_is_realvec( PElement *base, gboolean *out ) 
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_is_realvec( rc, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

gboolean 
heap_is_matrix( PElement *base, gboolean *out ) 
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_is_matrix( rc, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

gboolean
heap_is_class( PElement *base, gboolean *out )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_is_class( rc, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

gboolean
heap_is_instanceof_exact( const char *name, PElement *klass, gboolean *out )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_is_instanceof_exact( rc, name, klass );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

gboolean
heap_is_instanceof( const char *name, PElement *klass, gboolean *out )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	*out = reduce_is_instanceof( rc, name, klass );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

int
heap_list_length( PElement *base )
{
	Reduce *rc = reduce_context;
	int result;

	REDUCE_CATCH_START( -1 );
	result = reduce_list_length( rc, base );
	REDUCE_CATCH_STOP; 

	return( result );
}

int
heap_list_length_max( PElement *base, int max_length )
{
	Reduce *rc = reduce_context;
	int result;

	REDUCE_CATCH_START( -1 );
	result = reduce_list_length_max( rc, base, max_length );
	REDUCE_CATCH_STOP; 

	return( result );
}

gboolean
heap_list_index( PElement *base, int n, PElement *out )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	reduce_list_index( rc, base, n, out );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

gboolean
heap_reduce_strict( PElement *base )
{
	Reduce *rc = reduce_context;

	REDUCE_CATCH_START( FALSE );
	reduce_spine_strict( rc, base );
	REDUCE_CATCH_STOP; 

	return( TRUE );
}

/* hn is a node in a compiled function, out is part of a node in reduce
 * space to which it should be copied. 
 *
 * Have to be careful to copy sym pointers in nodes from compile heap.
 */
static gboolean
copy_node( Heap *heap, HeapNode *ri[], HeapNode *hn, PElement *out )
{
	HeapNode *hn1;
	PElement pleft, pright;
	int i;

	/* Look for relocation nodes.
	 */
	if( hn->type == TAG_SHARED ) {
		/* RHS of SHARE is the index of this share node.
		 */
		i = GPOINTER_TO_INT( GETRIGHT( hn ) );

		/* Skip to shared section.
		 */
		hn = GETLEFT( hn );

		/* Copy and link on this node.
		 */
		if( NEWNODE( heap, hn1 ) )
			return( FALSE );
		*hn1 = *hn;
		PEPUTP( out, ELEMENT_NODE, hn1 );

		/* Note pointer in relocation table.
		 */
		ri[i] = hn1;
	}
	else if( hn->type == TAG_REFERENCE ) {
		/* Must have already copied this SHARE, just link back.
		 */
		hn1 = GETLEFT( hn );
		i = GPOINTER_TO_INT( GETRIGHT( hn1 ) );
		PEPUTP( out, ELEMENT_NODE, ri[i] );

		/* Done!
		 */
		return( TRUE );
	}
	else {
		/* Copy and link on this node.
		 */
		if( NEWNODE( heap, hn1 ) )
			return( FALSE );
		*hn1 = *hn;
		PEPUTP( out, ELEMENT_NODE, hn1 );
	}

	/* If it's a DOUBLE, no more to do.
	 */
	if( hn->type == TAG_DOUBLE )
		return( TRUE );

	if( hn->ltype != ELEMENT_NODE && hn->rtype == ELEMENT_NODE ) {
		/* Right pointer only. Zap pointer so we can GC
		 * safely.
		 */
		hn1->rtype = ELEMENT_CHAR;

		/* Recurse for RHS of node.
		 */
		PEPOINTRIGHT( hn1, &pright );
		if( !copy_node( heap, ri, GETRIGHT( hn ), &pright ) )
			return( FALSE );
	}
	else if( hn->ltype == ELEMENT_NODE && hn->rtype != ELEMENT_NODE ) {
		/* Left pointer only. Zap pointer so we can GC
		 * safely.
		 */
		hn1->ltype = ELEMENT_CHAR;

		/* Recurse for LHS of node.
		 */
		PEPOINTLEFT( hn1, &pleft );
		if( !copy_node( heap, ri, GETLEFT( hn ), &pleft ) )
			return( FALSE );
	}
	else if( hn->ltype == ELEMENT_NODE && hn->rtype == ELEMENT_NODE ) {
		/* Both pointers. Zap pointers so we can GC safely.
		 */
		hn1->ltype = ELEMENT_CHAR;
		hn1->rtype = ELEMENT_CHAR;

		/* Recurse for boths sides of node.
		 */
		PEPOINTLEFT( hn1, &pleft );
		PEPOINTRIGHT( hn1, &pright );
		if( !copy_node( heap, ri, GETLEFT( hn ), &pleft ) ||
			!copy_node( heap, ri, GETRIGHT( hn ), &pright ) )
			return( FALSE );
	}

	return( TRUE );
}

/* Copy a compiled graph into the main reduce space. Overwrite the node at
 * out.
 */
gboolean
heap_copy( Heap *heap, Compile *compile, PElement *out )
{
	Element *root = &compile->base;
	HeapNode *ri[MAX_RELOC];

#ifdef DEBUG
	printf( "heap_copy: " );
	symbol_name_print( compile->sym );
	printf( "\n" );
#endif /*DEBUG*/

	switch( root->type ) {
	case ELEMENT_NODE:
		/* Need a tree copy.
		 */
		if( !copy_node( heap, &ri[0], (HeapNode *) root->ele, out ) )
			return( FALSE );
		break;

	case ELEMENT_SYMBOL:
	case ELEMENT_CHAR:
	case ELEMENT_BOOL:
	case ELEMENT_BINOP:
	case ELEMENT_SYMREF:
	case ELEMENT_COMPILEREF:
	case ELEMENT_CONSTRUCTOR:
	case ELEMENT_UNOP:
	case ELEMENT_COMB:
	case ELEMENT_TAG:
	case ELEMENT_ELIST:
	case ELEMENT_MANAGED:
		/* Copy value.
		 */
		PEPUTP( out, root->type, root->ele );
		break;

	case ELEMENT_NOVAL:
		/* Not compiled yet: compile now, then copy.
		 */
		if( compile_object( compile ) )
			return( FALSE );
		if( !heap_copy( heap, compile, out ) )
			return( FALSE );
		break;

	default:
		g_assert( FALSE );
	}

	return( TRUE );
}

/* Try to make a gvalue from a heap object. 
 */
gboolean
heap_ip_to_gvalue( PElement *in, GValue *out )
{
	Reduce *rc = reduce_context;

	if( !reduce_pelement( rc, reduce_spine_strict, in ) )
		return( FALSE );

	if( PEISREAL( in ) ) {
		g_value_init( out, G_TYPE_DOUBLE );
		g_value_set_double( out, PEGETREAL( in ) );
	}
	else if( PEISBOOL( in ) ) {
		g_value_init( out, G_TYPE_BOOLEAN );
		g_value_set_boolean( out, PEGETBOOL( in ) );
	}
	else if( PEISCOMPLEX( in ) ) {
		printf( "ip_to_gvalue: no complex gtype!\n" );
		return( FALSE );
	}
	else if( PEISLIST( in ) ) {
		gboolean result;

		if( !heap_is_string( in, &result ) )
			return( FALSE );
		if( result ) {
			char name[256];

			if( !heap_get_string( in, name, 256 ) )
				return( FALSE );

			/* We want a refstring, not a G_TYPE_STRING, since
			 * this GValue will (probably) be used by vips with
			 * im_header_string() etc.
			 */
			g_value_init( out, IM_TYPE_REF_STRING );
			im_ref_string_set( out, name );
		}
		else {
			error_top( _( "Unimplemented list type." ) );
			return( FALSE );
		}
	}
	else if( PEISMANAGED( in ) && IS_MANAGEDGOBJECT( PEGETVAL( in ) ) ) {
		g_value_init( out, G_TYPE_OBJECT );
		g_value_set_object( out, 
			MANAGEDGOBJECT( PEGETMANAGED( in ) )->object );
	}
	else
		return( FALSE );

	return( TRUE );
}

/* Try to make a heap object from a gvalue.
 */
gboolean
heap_gvalue_to_ip( GValue *in, PElement *out )
{
	Reduce *rc = reduce_context;
	Heap *heap = rc->heap;

	if( G_VALUE_HOLDS_BOOLEAN( in ) ) {
		PEPUTP( out, ELEMENT_BOOL, (int) g_value_get_boolean( in ) );
	}
	else if( G_VALUE_HOLDS_CHAR( in ) ) {
		PEPUTP( out, ELEMENT_CHAR, (int) g_value_get_char( in ) );
	}
	else if( G_VALUE_HOLDS_UCHAR( in ) ) {
		PEPUTP( out, ELEMENT_CHAR, (int) g_value_get_uchar( in ) );
	}
	else if( G_VALUE_HOLDS_INT( in ) ) {
		if( !heap_real_new( heap, g_value_get_int( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_UINT( in ) ) {
		if( !heap_real_new( heap, g_value_get_uint( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_LONG( in ) ) {
		if( !heap_real_new( heap, g_value_get_long( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_ULONG( in ) ) {
		if( !heap_real_new( heap, g_value_get_ulong( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_INT64( in ) ) {
		if( !heap_real_new( heap, g_value_get_int64( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_UINT64( in ) ) {
		if( !heap_real_new( heap, g_value_get_uint64( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_FLOAT( in ) ) {
		if( !heap_real_new( heap, g_value_get_float( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_DOUBLE( in ) ) {
		if( !heap_real_new( heap, g_value_get_double( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_ENUM( in ) ) {
		if( !heap_real_new( heap, g_value_get_enum( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_STRING( in ) ) {
		if( !heap_managedstring_new( heap, 
			g_value_get_string( in ), out ) )
			return( FALSE );
	}
	else if( G_VALUE_HOLDS_OBJECT( in ) ) {
		GObject *object;
		Managed *managed;

		object = g_value_get_object( in );
		managed = MANAGED( managedgobject_new( heap, object ) );
		PEPUTP( out, ELEMENT_MANAGED, managed );
	}
	else if( g_value_type_transformable( G_VALUE_TYPE( in ), 
		G_TYPE_STRING ) ) {
		GValue temp = { 0 };

		g_value_init( &temp, G_TYPE_STRING );
		g_value_transform( in, &temp );
		if( !heap_managedstring_new( heap, 
			g_value_get_string( &temp ), out ) ) {
			return( FALSE );
			g_value_unset( &temp );
		}
		g_value_unset( &temp );
	}
	else {
		error_top( _( "Unimplemented type." ) );
		error_sub( _( "Unable to convert %s to a nip type." ), 
			G_VALUE_TYPE_NAME( in ) ); 

		return( FALSE );
	}

	return( TRUE );
}
