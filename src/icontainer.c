/* abstract base class for containers
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
#define DEBUG_SANITY
#define DEBUG_VERBOSE
#define DEBUG
 */

#include "ip.h"

/* Our signals. 
 */
enum {
	SIG_POS_CHANGED,	/* Member has moved */
	SIG_CHILD_ADD,		/* iContainer is about to gain a child */
	SIG_CHILD_REMOVE,	/* iContainer is about to loose a child */
	SIG_CURRENT,		/* Make child current of parent */
	SIG_CHILD_DETACH,	/* Used as a pair to do reparent */
	SIG_CHILD_ATTACH,
	SIG_LAST
};

static iObjectClass *parent_class = NULL;

static guint icontainer_signals[SIG_LAST] = { 0 };

int
icontainer_get_n_children( iContainer *icontainer )
{
	return( g_slist_length( icontainer->children ) );
}

iContainer *
icontainer_get_nth_child( iContainer *icontainer, int n )
{
	return( ICONTAINER( g_slist_nth_data( icontainer->children, n ) ) );
}

GSList *
icontainer_get_children( iContainer *icontainer )
{
	return( g_slist_copy( icontainer->children ) );
}

void *
icontainer_map( iContainer *icontainer, icontainer_map_fn fn, void *a, void *b )
{
	return( slist_map2( icontainer->children, (SListMap2Fn) fn, a, b ) );
}

void *
icontainer_map3( iContainer *icontainer, 
	icontainer_map3_fn fn, void *a, void *b, void *c )
{
	return( slist_map3( icontainer->children, (SListMap3Fn) fn, a, b, c ) );
}

void *
icontainer_map4( iContainer *icontainer, 
	icontainer_map4_fn fn, void *a, void *b, void *c, void *d )
{
	return( slist_map4( icontainer->children, 
		(SListMap4Fn) fn, a, b, c, d ) );
}

void *
icontainer_map5( iContainer *icontainer, 
	icontainer_map5_fn fn, void *a, void *b, void *c, void *d, void *e )
{
	return( slist_map5( icontainer->children, 
		(SListMap5Fn) fn, a, b, c, d, e ) );
}

/* Map in reverse order.
 */
void *
icontainer_map_rev( iContainer *icontainer, 
	icontainer_map_fn fn, void *a, void *b )
{
	return( slist_map2_rev( icontainer->children, 
		(SListMap2Fn) fn, a, b ) );
}

/* Apply a function to a tree of icontainers, bottom up.
 */
void *
icontainer_map_all( iContainer *icontainer, icontainer_map_fn fn, void *a )
{
	iContainer *result;

	if( (result = icontainer_map( icontainer, 
		(icontainer_map_fn) icontainer_map_all, (void *) fn, a )) )
		return( result );

	return( fn( icontainer, a, NULL ) );
}

void *
icontainer_map2_all( iContainer *icontainer, 
	icontainer_map_fn fn, void *a, void *b )
{
	iContainer *result;

	if( (result = icontainer_map3( icontainer, 
		(icontainer_map3_fn) icontainer_map2_all, (void *) fn, a, b )) )
		return( result );

	return( fn( icontainer, a, b ) );
}

void *
icontainer_map3_all( iContainer *icontainer, 
	icontainer_map3_fn fn, void *a, void *b, void *c )
{
	iContainer *result;

	if( (result = icontainer_map4( icontainer, 
		(icontainer_map4_fn) icontainer_map3_all, 
			(void *) fn, a, b, c )) )
		return( result );

	return( fn( icontainer, a, b, c ) );
}

void *
icontainer_map4_all( iContainer *icontainer, 
	icontainer_map4_fn fn, void *a, void *b, void *c, void *d )
{
	iContainer *result;

	if( (result = icontainer_map5( icontainer, 
		(icontainer_map5_fn) icontainer_map4_all, 
			(void *) fn, a, b, c, d )) )
		return( result );

	return( fn( icontainer, a, b, c, d ) );
}

/* Apply a function to the children of a icontainer.
 */
void *
icontainer_map_all_intrans( iContainer *icontainer, 
	icontainer_map_fn fn, void *a )
{
	return( icontainer_map( icontainer, 
		(icontainer_map_fn) icontainer_map_all, (void *) fn, a ) );
}

static void *
icontainer_sanity_child( iContainer *child, iContainer *parent )
{
	g_assert( IS_ICONTAINER( child ) );
	g_assert( IS_ICONTAINER( parent ) );
	g_assert( child->parent == parent );
	g_assert( child->pos >= 0 );
	g_assert( g_slist_find( parent->children, child ) );

	if( parent->child_hash )
		g_assert( g_hash_table_lookup( parent->child_hash, 
			IOBJECT( child )->name ) );

	return( NULL );
}

void
icontainer_sanity( iContainer *icontainer )
{
	g_assert( IS_ICONTAINER( icontainer ) );

	if( icontainer->parent )
		icontainer_sanity_child( icontainer, icontainer->parent );
	icontainer_map( icontainer, 
		(icontainer_map_fn) icontainer_sanity_child, icontainer, NULL );
}

static gint
icontainer_pos_compare( iContainer *a, iContainer *b )
{
        return( a->pos - b->pos );
}

void
icontainer_pos_sort( iContainer *icontainer )
{
        icontainer->children = g_slist_sort( icontainer->children, 
		(GCompareFunc) icontainer_pos_compare );
	iobject_changed( IOBJECT( icontainer ) );
}

static void *
icontainer_pos_last_sub( iContainer *icontainer, int *max )
{
	if( icontainer->pos > *max )
		*max = icontainer->pos;

	return( NULL );
}

int
icontainer_pos_last( iContainer *icontainer )
{
	int max = -1;

	icontainer_map( icontainer,
		(icontainer_map_fn) icontainer_pos_last_sub, &max, NULL );

	return( max );
}

static void *
icontainer_pos_changed( iContainer *icontainer )
{
#ifdef DEBUG
	printf( "icontainer_pos_changed: " );
	iobject_print( IOBJECT( icontainer ) );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( icontainer ), 
		icontainer_signals[SIG_POS_CHANGED], 0 );

	return( NULL );
}

static void *
icontainer_pos_renumber_sub( iContainer *icontainer, int *n, GSList **changed )
{
	if( icontainer->pos != *n ) {
		icontainer->pos = *n;
		*changed = g_slist_prepend( *changed, icontainer );
	}

	*n += 1;

	return( NULL );
}

#ifdef DEBUG_VERBOSE
static void *
icontainer_print_element( iContainer *element, int *n )
{
	printf( "\t%3d) pos = %d ", *n, element->pos );
	iobject_print( IOBJECT( element ) );
	*n += 1;

	return( NULL );
}
#endif /*DEBUG_VERBOSE*/

void
icontainer_pos_renumber( iContainer *icontainer )
{
	int n = 0;
	GSList *changed;

#ifdef DEBUG_VERBOSE
{
	int i;

	printf( "icontainer_pos_renumber: " );
	iobject_print( IOBJECT( icontainer ) );
	printf( "\tbefore:\n" );
	i = 0;
	icontainer_map( icontainer, 
		(icontainer_map_fn) icontainer_print_element, &i, NULL );
}
#endif /*DEBUG_VERBOSE*/

	changed = NULL;
	icontainer_map( icontainer,
		(icontainer_map_fn) icontainer_pos_renumber_sub, &n, &changed );

	/* Tell all the children that have been renumbered.
	 */
#ifdef DEBUG_VERBOSE
	if( g_slist_length( changed ) > 1 ) {
		printf( "icontainer_pos_renumber: renumbering %d children! ",
			g_slist_length( changed ) );
		iobject_print( IOBJECT( icontainer ) );
	}
#endif /*DEBUG_VERBOSE*/
	slist_map( changed,
		(SListMapFn) icontainer_pos_changed, NULL );
	g_slist_free( changed );
	iobject_changed( IOBJECT( icontainer ) );

#ifdef DEBUG_VERBOSE
{
	int i;

	printf( "icontainer_pos_renumber: " );
	iobject_print( IOBJECT( icontainer ) );
	printf( "\tafter:\n" );
	i = 0;
	icontainer_map( icontainer, 
		(icontainer_map_fn) icontainer_print_element, &i, NULL );
}
#endif /*DEBUG_VERBOSE*/
}

gint
icontainer_name_compare( iContainer *a, iContainer *b )
{
        return( strcasecmp( IOBJECT( a )->name, IOBJECT( b )->name ) );
}

void
icontainer_custom_sort( iContainer *icontainer, GCompareFunc fn )
{
        icontainer->children = g_slist_sort( icontainer->children, fn );
	icontainer_pos_renumber( icontainer );
	iobject_changed( IOBJECT( icontainer ) );
}

/* Add a child.
 */
void
icontainer_child_add( iContainer *parent, iContainer *child, int pos )
{
	g_assert( IS_ICONTAINER( parent ) );
	g_assert( IS_ICONTAINER( child ) );

#ifdef DEBUG_SANITY
	icontainer_sanity( parent );
	icontainer_sanity( child );
#endif /*DEBUG_SANITY*/

	g_signal_emit( G_OBJECT( parent ), 
		icontainer_signals[SIG_CHILD_ADD], 0, child, pos );

#ifdef DEBUG_SANITY
	icontainer_sanity( parent );
	icontainer_sanity( child );
#endif /*DEBUG_SANITY*/
}

/* Add a child before another child. after == NULL means append.
 */
void
icontainer_child_add_before( iContainer *parent, 
	iContainer *child, iContainer *before )
{
	int pos;

	g_assert( !before || IS_ICONTAINER( before ) );
	g_assert( !before || before->parent == parent );

	pos = g_slist_index( parent->children, before );
	icontainer_child_add( parent, child, pos );
}

/* pos == 0 ... move to start
 * pos == -1 ... move to end
 * pos == n ... move before sibling at position n
 */
void
icontainer_child_move( iContainer *child, int pos )
{
	iContainer *parent = child->parent;

	parent->children = g_slist_remove( parent->children, child );

        if( pos >= 0 )
                parent->children = g_slist_insert( parent->children,
                        child, pos );
        else
                parent->children = g_slist_append( parent->children, child );

        icontainer_pos_renumber( parent );
	iobject_changed( IOBJECT( child ) );
}

void *
icontainer_child_remove( iContainer *child )
{
	iContainer *parent;

	if( (parent = child->parent) ) {
		g_assert( ICONTAINER_IS_CHILD( parent, child ) );

#ifdef DEBUG
		printf( "icontainer_child_remove: (child %p)\n", child );
		printf( "\tchild: %s \"%s\"\n",
			G_OBJECT_TYPE_NAME( child ), 
			NN( IOBJECT( child )->name ) );
#endif /*DEBUG*/

#ifdef DEBUG_SANITY
		icontainer_sanity( parent );
		icontainer_sanity( child );
#endif /*DEBUG_SANITY*/

		g_signal_emit( G_OBJECT( parent ), 
			icontainer_signals[SIG_CHILD_REMOVE], 0, child );

#ifdef DEBUG_SANITY
		icontainer_sanity( parent );
#endif /*DEBUG_SANITY*/
	}

	return( NULL );
}

void 
icontainer_current( iContainer *parent, iContainer *child )
{
	g_assert( parent );
	g_assert( !child || ICONTAINER_IS_CHILD( parent, child ) );

	if( parent->current == child )
		return; 

#ifdef DEBUG
	printf( "icontainer_current: (child %p)\n", child );
	printf( "\tchild: %s \"%s\"\n",
		G_OBJECT_TYPE_NAME( child ), 
		NN( IOBJECT( child )->name ) );
#endif /*DEBUG*/

#ifdef DEBUG_SANITY
	icontainer_sanity( parent );
	if( child )
		icontainer_sanity( child );
#endif /*DEBUG_SANITY*/

	g_signal_emit( G_OBJECT( parent ), 
		icontainer_signals[SIG_CURRENT], 0, child );

#ifdef DEBUG_SANITY
	icontainer_sanity( parent );
	if( child )
		icontainer_sanity( child );
#endif /*DEBUG_SANITY*/
}

void 
icontainer_reparent( iContainer *parent, iContainer *child, int pos )
{
	iContainer *old_parent = child->parent;

	g_assert( parent );
	g_assert( child );

#ifdef DEBUG_SANITY
	icontainer_sanity( old_parent );
	icontainer_sanity( parent );
	icontainer_sanity( child );
#endif /*DEBUG_SANITY*/

	/* These must always happen as a pair.
	 */
	g_signal_emit( G_OBJECT( old_parent ), 
		icontainer_signals[SIG_CHILD_DETACH], 0, child );
	g_signal_emit( G_OBJECT( parent ), 
		icontainer_signals[SIG_CHILD_ATTACH], 0, child, pos );

        icontainer_pos_renumber( parent );
	iobject_changed( IOBJECT( parent ) );
	iobject_changed( IOBJECT( old_parent ) );
	iobject_changed( IOBJECT( child ) );

#ifdef DEBUG_SANITY
	icontainer_sanity( old_parent );
	icontainer_sanity( parent );
	icontainer_sanity( child );
#endif /*DEBUG_SANITY*/
}

static void
icontainer_dispose( GObject *gobject )
{
	iContainer *icontainer;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_ICONTAINER( gobject ) );

	icontainer = ICONTAINER( gobject );

#ifdef DEBUG
	printf( "icontainer_dispose: (%p) %s \"%s\"\n",
		icontainer,
		G_OBJECT_TYPE_NAME( icontainer ), 
		NN( IOBJECT( icontainer )->name ) );
#endif /*DEBUG*/

	icontainer_map( icontainer,
		(icontainer_map_fn) icontainer_child_remove, NULL, NULL );
	icontainer_child_remove( icontainer );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
icontainer_finalize( GObject *gobject )
{
	iContainer *icontainer;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_ICONTAINER( gobject ) );

	icontainer = ICONTAINER( gobject );

	IM_FREEF( g_hash_table_destroy, icontainer->child_hash );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
icontainer_info( iObject *iobject, VipsBuf *buf )
{
	iContainer *icontainer = ICONTAINER( iobject );

	vips_buf_appendf( buf, "pos = \"%d\"\n", icontainer->pos );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );
}

static void
icontainer_real_pos_changed( iContainer *icontainer )
{
}

static void
icontainer_link( iContainer *parent, iContainer *child, int pos )
{
        if( pos >= 0 ) 
                parent->children = g_slist_insert( parent->children,
                        child, pos );
        else
                parent->children = g_slist_append( parent->children, child );
        child->parent = parent;
        child->pos = pos;
	if( parent->child_hash ) {
		g_assert( !g_hash_table_lookup( parent->child_hash, 
			IOBJECT( child )->name ) );

		g_hash_table_insert( parent->child_hash, 
			IOBJECT( child )->name, child );
	}
}

static void
icontainer_real_child_add( iContainer *parent, iContainer *child, int pos )
{
	iContainerClass *icontainer_class = ICONTAINER_GET_CLASS( child );

        g_assert( IS_ICONTAINER( parent ) && IS_ICONTAINER( child ) );
        g_assert( child->parent == NULL );

#ifdef DEBUG
	printf( "icontainer_real_child_add:\n\tparent " );
	iobject_print( IOBJECT( parent ) );
	printf( "\tchild " );
	iobject_print( IOBJECT( child ) );
	printf( "\tpos = %d\n", pos );
#endif /*DEBUG*/

	icontainer_link( parent, child, pos ); 

	g_object_ref( G_OBJECT( child ) );
	iobject_sink( IOBJECT( child ) );

	/* Renumber to get all the pos set. 
	 */
        icontainer_pos_renumber( parent );
	iobject_changed( IOBJECT( child ) );

        /* We've made the link ... trigger the parent_add() on the child.
         */
        icontainer_class->parent_add( child );

#ifdef DEBUG_VERBOSE
        printf( "icontainer_real_child_add: " );
	iobject_print( IOBJECT( parent ) );
#endif /*DEBUG_VERBOSE*/
}

static void
icontainer_unlink( iContainer *child )
{
	iContainer *parent = child->parent;

	parent->children = g_slist_remove( parent->children, child );
	child->parent = NULL;
	if( parent->child_hash ) {
		g_assert( g_hash_table_lookup( parent->child_hash, 
			IOBJECT( child )->name ) );

		g_hash_table_remove( parent->child_hash, 
			IOBJECT( child )->name );
	}
}

static void 
icontainer_real_child_remove( iContainer *parent, iContainer *child )
{
	iContainerClass *icontainer_child_class = ICONTAINER_GET_CLASS( child );

	g_assert( IS_ICONTAINER( parent ) && IS_ICONTAINER( child ) );

#ifdef DEBUG
	printf( "icontainer_real_child_remove: parent %s \"%s\"; "
		"child %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( parent ), NN( IOBJECT( parent )->name ),
		G_OBJECT_TYPE_NAME( child ), NN( IOBJECT( child )->name ) );
#endif /*DEBUG*/

	if( parent->current == child )
		icontainer_current( parent, NULL ); 

	/* We're about to break the link ... trigger the parent_remove() on 
	 * the child.
	 */
	icontainer_child_class->parent_remove( child );

	icontainer_unlink( child ); 

	UNREF( child );

	iobject_changed( IOBJECT( parent ) );
}

static void
icontainer_real_parent_add( iContainer *child )
{
#ifdef DEBUG
	printf( "icontainer_real_parent_add: child %s \"%s\"; " 
		"parent %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( child ), 
		NN( IOBJECT( child )->name ),
		G_OBJECT_TYPE_NAME( child->parent ), 
		NN( IOBJECT( child->parent )->name ) );
#endif /*DEBUG*/
}

static void
icontainer_real_parent_remove( iContainer *child )
{
#ifdef DEBUG
{
	iContainer *parent = child->parent;

	printf( "icontainer_real_parent_remove: child %s \"%s\"; "
		"parent %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( child ), NN( IOBJECT( child )->name ),
		G_OBJECT_TYPE_NAME( parent ), NN( IOBJECT( parent )->name ) );
}
#endif /*DEBUG*/
}

static void 
icontainer_real_current( iContainer *parent, iContainer *child )
{
	iContainer *old_current;

	g_assert( IS_ICONTAINER( parent ) );
	g_assert( !child || IS_ICONTAINER( child ) );
	g_assert( !child || ICONTAINER_IS_CHILD( parent, child ) );

#ifdef DEBUG
	printf( "icontainer_real_current: parent %s \"%s\"; "
		"child %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( parent ), NN( IOBJECT( parent )->name ),
		child ? G_OBJECT_TYPE_NAME( child ) : "NULL", 
		child ? NN( IOBJECT( child )->name ) : "NULL" );
#endif /*DEBUG*/

	old_current = parent->current;
	parent->current = child;

	if( old_current != child ) {
		if( old_current )
			iobject_changed( IOBJECT( old_current ) );
		if( child )
			iobject_changed( IOBJECT( child ) );
		iobject_changed( IOBJECT( parent ) );
	}

	if( child )
		model_front( MODEL( child ) );
}

static void
icontainer_real_child_detach( iContainer *parent, iContainer *child )
{
        g_assert( IS_ICONTAINER( parent ) ); 
        g_assert( IS_ICONTAINER( child ) );
        g_assert( child->parent != NULL );
	g_assert( ICONTAINER_IS_CHILD( parent, child ) );

	icontainer_unlink( child ); 
}

static void
icontainer_real_child_attach( iContainer *parent, iContainer *child, int pos )
{
        g_assert( IS_ICONTAINER( parent ) ); 
        g_assert( IS_ICONTAINER( child ) );
        g_assert( child->parent == NULL );

	icontainer_link( parent, child, pos ); 
}

static void
icontainer_class_init( iContainerClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = icontainer_dispose;
	gobject_class->finalize = icontainer_finalize;

	iobject_class->info = icontainer_info;

	class->pos_changed = icontainer_real_pos_changed;
	class->child_add = icontainer_real_child_add;
	class->child_remove = icontainer_real_child_remove;
	class->parent_add = icontainer_real_parent_add;
	class->parent_remove = icontainer_real_parent_remove;
	class->current = icontainer_real_current;
	class->child_detach = icontainer_real_child_detach;
	class->child_attach = icontainer_real_child_attach;

	/* Create signals.
	 */
	icontainer_signals[SIG_POS_CHANGED] = g_signal_new( "pos_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( iContainerClass, pos_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );

	icontainer_signals[SIG_CHILD_ADD] = g_signal_new( "child_add",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( iContainerClass, child_add ),
		NULL, NULL,
		nip_VOID__OBJECT_INT,
		G_TYPE_NONE, 2,
		TYPE_ICONTAINER, GTK_TYPE_INT );

	icontainer_signals[SIG_CHILD_REMOVE] = g_signal_new( "child_remove",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET( iContainerClass, child_remove ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		TYPE_ICONTAINER );

	icontainer_signals[SIG_CURRENT] = g_signal_new( "current",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET( iContainerClass, current ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		TYPE_ICONTAINER );

	icontainer_signals[SIG_CHILD_DETACH] = g_signal_new( "child_detach",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET( iContainerClass, child_detach ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		TYPE_ICONTAINER );

	icontainer_signals[SIG_CHILD_ATTACH] = g_signal_new( "child_attach",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( iContainerClass, child_attach ),
		NULL, NULL,
		nip_VOID__OBJECT_INT,
		G_TYPE_NONE, 2,
		TYPE_ICONTAINER, GTK_TYPE_INT );

#ifdef DEBUG_SANITY
	printf( "*** DEBUG_SANITY is on ... expect slowness\n" );
#endif /*DEBUG_SANITY*/
}

static void
icontainer_init( iContainer *icontainer )
{
	/* Init our instance fields.
	 */
	icontainer->children = NULL;
	icontainer->pos = -1;
	icontainer->parent = NULL;
	icontainer->child_hash = NULL;
}

GType
icontainer_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( iContainerClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) icontainer_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( iContainer ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) icontainer_init,
		};

		type = g_type_register_static( TYPE_IOBJECT, 
			"iContainer", &info, 0 );
	}

	return( type );
}

/* Put the container into lookup-by-child-name mode.
 */
void
icontainer_set_hash( iContainer *icontainer )
{
	/* Can only do this once just after startup, and before there are any
	 * children.
	 */
	g_assert( !icontainer->children );
	g_assert( !icontainer->child_hash );

	icontainer->child_hash = g_hash_table_new( g_str_hash, g_str_equal );
}

iContainer *
icontainer_child_lookup( iContainer *parent, const char *name )
{
	g_assert( parent->child_hash );

	return( ICONTAINER( g_hash_table_lookup( parent->child_hash, name ) ) );
}
