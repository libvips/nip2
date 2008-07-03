/* Call vips functions from the graph reducer.
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
#define DEBUG_TIME
#define DEBUG
#define DEBUG_HISTORY_SANITY
#define DEBUG_HISTORY_MISS
#define DEBUG_HISTORY
 */

/* This is usually turned on from a -D in cflags.
#define DEBUG_LEAK
 */

/* Often want it off ... we get spurious complaints about leaks if an
 * operation has no images in or out (eg. im_version) because it'll never
 * get GCed.
#undef DEBUG_LEAK
 */

/* VIPS callbacks can keep coming in, even after we've removed something from
 * the cache. Keep a proxy between the IMAGE and the VipsCall.
 */
typedef struct _VipsProxy {
	VipsCall *call;			/* Call this image is part of */
	IMAGE *im;			/* IMAGE we proxy */
} VipsProxy;

/* The previous function calls we are caching, plus an LRU queue for flushing.
 */
static GHashTable *vips_call_table = NULL;
static Queue *vips_call_lru = NULL;
int vips_call_size = 0;

#ifdef DEBUG_LEAK
/* All the VipsCall we make ... for leak testing. Build this file with 
 * DEBUG_LEAK to enable add/remove to/from this list.
 */
static GSList *vips_call_all = NULL;
#endif /*DEBUG_LEAK*/

/* Hash from a vargv ... just look at input args and the function name.
 */
static unsigned int
vips_call_hash( VipsCall *call )
{
	int i;
	unsigned int hash;

	if( call->found_hash )
		return( call->hash );

	hash = 0;

/* add ints, floats, pointers and strings to the hash.

	FIXME ... could do better on double? could or top and bottom 32 bits
	but would this be stupid on a 64 bit machine?

 */
#define HASH_I( I ) hash = (hash << 1) | ((unsigned int) (I));
#define HASH_D( D ) hash = (hash << 1) | ((unsigned int) (D));
#define HASH_P( P ) hash = (hash << 1) | (GPOINTER_TO_UINT( P ));
#define HASH_S( S ) hash = (hash << 1) | g_str_hash( S );

	/* Add the function to the hash. We often call many functions on
	 * the same args, we'd like these calls to hash to different numbers.
	 */
	HASH_P( call->fn );

        for( i = 0; i < call->fn->argc; i++ ) {
                im_type_desc *vips = call->fn->argv[i].desc;
		VipsArgumentType vt = vips_type_find( vips->type );

                if( !(vips->flags & IM_TYPE_OUTPUT) ) {
			switch( vt ) {
			case VIPS_DOUBLE:
				HASH_D( *((double *) call->vargv[i]) );
				break;

			case VIPS_INT:
				HASH_I( *((int *) call->vargv[i]) );
				break;

			case VIPS_COMPLEX:
				HASH_D( ((double *) call->vargv[i])[0] );
				HASH_D( ((double *) call->vargv[i])[1] );
				break;

			case VIPS_STRING:
				HASH_S( (char *) call->vargv[i] );
				break;

			case VIPS_IMAGE:
				HASH_P( call->vargv[i] );
				break;

			case VIPS_DOUBLEVEC:
			{
				im_doublevec_object *v = 
					(im_doublevec_object *) call->vargv[i];
				int j;

				for( j = 0; j < v->n; j++ )
					HASH_D( v->vec[j] );

				break;
			}

			case VIPS_INTVEC:
			{
				im_intvec_object *v = 
					(im_intvec_object *) call->vargv[i];
				int j;

				for( j = 0; j < v->n; j++ )
					HASH_I( v->vec[j] );

				break;
			}

			case VIPS_DMASK:
			{
				im_mask_object *mo = call->vargv[i];
				DOUBLEMASK *mask = mo->mask;

				/* mask can be NULL if we are called after 
				 * vips_new() but before we've built the arg
				 * list.
				 */
				if( mask ) {
					int ne = mask->xsize * mask->ysize;
					int j;

					for( j = 0; j < ne; j++ )
						HASH_D( mask->coeff[j] );
					HASH_D( mask->scale );
					HASH_D( mask->offset );
				}

				break;
			}

			case VIPS_IMASK:
			{
				im_mask_object *mo = call->vargv[i];
				INTMASK *mask = mo->mask;

				/* mask can be NULL if we are called after 
				 * vips_new() but before we've built the arg
				 * list.
				 */
				if( mask ) {
					int ne = mask->xsize * mask->ysize;
					int j;

					for( j = 0; j < ne; j++ )
						HASH_I( mask->coeff[j] );
					HASH_I( mask->scale );
					HASH_I( mask->offset );
				}

				break;
			}

			case VIPS_IMAGEVEC:
			{
				im_imagevec_object *v = 
					(im_imagevec_object *) call->vargv[i];
				int j;

				for( j = 0; j < v->n; j++ )
					HASH_P( v->vec[j] );

				break;
			}

			case VIPS_GVALUE:
				HASH_P( call->vargv[i] );
				break;

			default:
			case VIPS_NONE:
				break;
			}
		}
        }

	call->found_hash = TRUE;
	call->hash = hash;

	return( hash );
}

/* Are two function calls equal. Check the func and the input args.
 */
static gboolean
vips_call_equal( VipsCall *call1, VipsCall *call2 ) 
{
	int i;
	im_function *fn = call1->fn;

	if( call1 == call2 )
		return( TRUE );

	if( call1->fn != call2->fn )
		return( FALSE );

	if( call1->found_hash && 
		call2->found_hash && 
		call1->hash != call2->hash )
		return( FALSE );

        for( i = 0; i < fn->argc; i++ ) {
                im_type_desc *vips = fn->argv[i].desc;
		VipsArgumentType vt = vips_type_find( vips->type );

                if( !(vips->flags & IM_TYPE_OUTPUT) ) {
			switch( vt ) {
			case VIPS_DOUBLE:
				if( *((double *) call1->vargv[i]) != 
					*((double *) call2->vargv[i]) )
					return( FALSE );
				break;

			case VIPS_INT:
				if( *((int *) call1->vargv[i]) != 
					*((int *) call2->vargv[i]) )
					return( FALSE );
				break;

			case VIPS_COMPLEX:
				if( ((double *) call1->vargv[i])[0] != 
					((double *) call2->vargv[i])[0] )
					return( FALSE );
				if( ((double *) call1->vargv[i])[1] != 
					((double *) call2->vargv[i])[1] )
					return( FALSE );
				break;

			case VIPS_STRING:
				if( strcmp( (char *) call1->vargv[i],
					(char *) call2->vargv[i] ) != 0 )
					return( FALSE );
				break;

			case VIPS_IMAGE:
				if( call1->vargv[i] != call2->vargv[i] )
					return( FALSE );
				break;

			case VIPS_DOUBLEVEC:
			{
				im_doublevec_object *v1 = 
					(im_doublevec_object *) call1->vargv[i];
				im_doublevec_object *v2 = 
					(im_doublevec_object *) call2->vargv[i];
				int j;

				for( j = 0; j < v1->n; j++ )
					if( v1->vec[j] != v2->vec[j] )
						return( FALSE );

				break;
			}

			case VIPS_INTVEC:
			{
				im_intvec_object *v1 = 
					(im_intvec_object *) call1->vargv[i];
				im_intvec_object *v2 = 
					(im_intvec_object *) call2->vargv[i];
				int j;

				for( j = 0; j < v1->n; j++ )
					if( v1->vec[j] != v2->vec[j] )
						return( FALSE );

				break;
			}

			case VIPS_DMASK:
			{
				im_mask_object *mo1 = 
					(im_mask_object *) call1->vargv[i];
				im_mask_object *mo2 = 
					(im_mask_object *) call2->vargv[i];
				DOUBLEMASK *mask1 = mo1->mask;
				DOUBLEMASK *mask2 = mo2->mask;
				int ne = mask1->xsize * mask2->ysize;
				int j;
	
				if( mask1->xsize != mask2->xsize ||
					mask1->ysize != mask2->ysize )
					return( FALSE );

				for( j = 0; j < ne; j++ )
					if( mask1->coeff[j] != mask2->coeff[j] )
						return( FALSE );

				if( mask1->scale != mask2->scale )
					return( FALSE );
				if( mask1->offset != mask2->offset ) 
					return( FALSE );

				break;
			}

			case VIPS_IMASK:
			{
				im_mask_object *mo1 = 
					(im_mask_object *) call1->vargv[i];
				im_mask_object *mo2 = 
					(im_mask_object *) call2->vargv[i];
				INTMASK *mask1 = mo1->mask;
				INTMASK *mask2 = mo2->mask;
				int ne = mask1->xsize * mask2->ysize;
				int j;
	
				if( mask1->xsize != mask2->xsize ||
					mask1->ysize != mask2->ysize )
					return( FALSE );

				for( j = 0; j < ne; j++ )
					if( mask1->coeff[j] != mask2->coeff[j] )
						return( FALSE );

				if( mask1->scale != mask2->scale )
					return( FALSE );
				if( mask1->offset != mask2->offset ) 
					return( FALSE );

				break;
			}

			case VIPS_IMAGEVEC:
			{
				im_imagevec_object *v1 = 
					(im_imagevec_object *) call1->vargv[i];
				im_imagevec_object *v2 = 
					(im_imagevec_object *) call2->vargv[i];
				int j;

				for( j = 0; j < v1->n; j++ )
					if( v1->vec[j] != v2->vec[j] )
						return( FALSE );

				break;
			}

			/* Very strict. Could be more generous here: we'd need
			 * to have a pspec for each argument type and then use 
			 * g_param_values_cmp() to test equality.
			 */
			case VIPS_GVALUE:
				if( call1->vargv[i] != call2->vargv[i] )
					return( FALSE );
				break;

			default:
			case VIPS_NONE:
				break;
			}
		}
        }

	return( TRUE );
}

#ifdef DEBUG_HISTORY_SANITY
static void
vips_call_sanity_sub( VipsCall *call )
{
	g_assert( g_slist_find( vips_call_lru->list, call ) );
}

static void
vips_call_sanity( void )
{
	GSList *p;

	if( !vips_call_lru || !vips_call_table )
		return;

	/* Everything that's on the LRU should be in the history table.
	 */
	for( p = vips_call_lru->list; p; p = p->next ) {
		VipsCall *call = (VipsCall *) p->data;

		g_assert( g_hash_table_lookup( vips_call_table, call ) );

		g_assert( call->fn );
		g_assert( call->fn->argc > 0 && call->fn->argc < 100 );
		g_assert( call->in_cache );
	}

	/* Everything that's on the history table should be in the LRU.
	 */
	g_hash_table_foreach( vips_call_table,
		(GHFunc) vips_call_sanity_sub, NULL );

	/* Everything that's on the LRU should be in the global vips_call
	 * list.
	 */
	for( p = vips_call_lru->list; p; p = p->next ) {
		VipsCall *call = (VipsCall *) p->data;

		/* Need to build with DEBUG on before vips_call_all is
		 * maintained.
		 */
		g_assert( g_slist_find( vips_call_all, call ) );
	}

	/* Everything on vips_call_all that's not in vips_call_table should
	 * have in_cache FALSE.
	 */
	for( p = vips_call_all; p; p = p->next ) {
		VipsCall *call = (VipsCall *) p->data;

		if( !g_hash_table_lookup( vips_call_table, call ) )
			g_assert( !call->in_cache );
	}
}
#endif /*DEBUG_HISTORY_SANITY*/

/* Is a function call in our history? Return the old one, if any. 
 */
VipsCall *
vips_call_lookup( VipsCall *call )
{
	VipsCall *old_call;

	if( !vips_call_table ) {
		vips_call_table = g_hash_table_new( 
			(GHashFunc) vips_call_hash, 
			(GEqualFunc) vips_call_equal );
		vips_call_lru = queue_new();
	}

	old_call = (VipsCall *) g_hash_table_lookup( vips_call_table, call );

#ifdef DEBUG_HISTORY
	if( old_call ) 
		printf( "vips_call_lookup: found \"%s\"\n", old_call->fn->name );
#endif /*DEBUG_HISTORY*/
#ifdef DEBUG_HISTORY_SANITY
	vips_call_sanity();
#endif /*DEBUG_HISTORY_SANITY*/

	return( old_call );
}

/* Bump to end of LRU.
 */
static void
vips_call_touch( VipsCall *call )
{
	g_assert( call->in_cache );

	queue_remove( vips_call_lru, call );
	queue_add( vips_call_lru, call );

#ifdef DEBUG_HISTORY_SANITY
	vips_call_sanity();
#endif /*DEBUG_HISTORY_SANITY*/
#ifdef DEBUG_HISTORY
	printf( "vips_call_touch: bumping \"%s\"\n", call->fn->name );
#endif /*DEBUG_HISTORY*/
}

static void
vips_call_insert( VipsCall *call )
{
	g_assert( !g_hash_table_lookup( vips_call_table, call ) );
	g_assert( !call->in_cache );

	g_hash_table_insert( vips_call_table, call, call );
	vips_call_size += 1;

	g_assert( g_hash_table_lookup( vips_call_table, call ) );

	queue_add( vips_call_lru, call );
	call->in_cache = TRUE;

#ifdef DEBUG_HISTORY_SANITY
	vips_call_sanity();
#endif /*DEBUG_HISTORY_SANITY*/
#ifdef DEBUG_HISTORY
	printf( "vips_call_insert: adding \"%s\"\n", call->fn->name );
#endif /*DEBUG_HISTORY*/
}

/* Are we in the history? Remove us.
 */
static void
vips_call_remove( VipsCall *call )
{
	if( call->in_cache ) {
		queue_remove( vips_call_lru, call );
		g_hash_table_remove( vips_call_table, call );
		vips_call_size -= 1;
		call->in_cache = FALSE;

#ifdef DEBUG_HISTORY
		printf( "vips_call_remove: removing \"%s\"\n", call->fn->name );
#endif /*DEBUG_HISTORY*/
	}

#ifdef DEBUG_HISTORY_SANITY
	vips_call_sanity();
#endif /*DEBUG_HISTORY_SANITY*/
}

static void
vips_call_destroy( VipsCall *call )
{
	GSList *p;
	int i;

#ifdef DEBUG_HISTORY
	printf( "vips_call_destroy: destroying \"%s\" (%p)\n", 
		call->fn->name, call );
#endif /*DEBUG_HISTORY*/

	/* Are we in the history? Remove us.
	 */
	vips_call_remove( call ); 
#ifdef DEBUG_LEAK
	vips_call_all = g_slist_remove( vips_call_all, call );
#endif /*DEBUG_LEAK*/

	/* Unlink this VipsCall from all the proxys.
	 */
	for( p = call->proxys; p; p = p->next ) {
		VipsProxy *proxy = (VipsProxy *) p->data;

		g_assert( proxy->call == call );
		proxy->call = NULL;
	}
	IM_FREEF( g_slist_free, call->proxys );

	/* Free any VIPS args we built and haven't used.
	 */
	for( i = 0; i < call->fn->argc; i++ ) {
		im_type_desc *ty = call->fn->argv[i].desc;
		im_object *obj = call->vargv[i];
		VipsArgumentType vt;

		/* Make sure we don't damage any error message we might
		 * have.
		 */
		error_block();
		vt = vips_type_find( ty->type );
		error_unblock();

		switch( vt ) {
		case VIPS_NONE:		/* IM_TYPE_DISPLAY */
		case VIPS_DOUBLE:
		case VIPS_INT: 	
		case VIPS_COMPLEX: 
		case VIPS_IMAGEVEC: 	/* Input only, so no freeing reqd */
		case VIPS_GVALUE:
			/* Do nothing.
			 */
			break;

		case VIPS_STRING:
			IM_FREE( obj );
			break;

		case VIPS_IMAGE:
			/* Input images are from the heap; do nothing.
			 * Output image we made ... close if there.
			 */
			if( ty->flags & IM_TYPE_OUTPUT ) {
				IMAGE **im = (IMAGE **) &obj;

				IM_FREEF( im_close, *im );
			}
			break;

		case VIPS_DOUBLEVEC:
			IM_FREE( ((im_doublevec_object *) obj)->vec );
			break;

		case VIPS_INTVEC:
			IM_FREE( ((im_intvec_object *) obj)->vec );
			break;

		case VIPS_DMASK:
			IM_FREE( ((im_mask_object *) obj)->name );
			IM_FREEF( im_free_dmask, 
				((im_mask_object *) obj)->mask );
			break;

		case VIPS_IMASK:
			IM_FREE( ((im_mask_object *) obj)->name );
			IM_FREEF( im_free_imask, 
				((im_mask_object *) obj)->mask );
			break;

		default:
			g_assert( FALSE );
		}
	}

	if( call->vargv ) {
		im_free_vargv( call->fn, call->vargv );
		IM_FREE( call->vargv );
	}
	IM_FREE( call );
}

static void
vips_call_remove_lru( void )
{
	VipsCall *call;

	call = (VipsCall *) queue_head( vips_call_lru );

#ifdef DEBUG_HISTORY
	printf( "vips_call_remove_lru: flushing \"%s\"\n", call->fn->name );
#endif /*DEBUG_HISTORY*/

	vips_call_destroy( call );

#ifdef DEBUG_HISTORY_SANITY
	vips_call_sanity();
#endif /*DEBUG_HISTORY_SANITY*/
}

static int
vips_call_close_cb( VipsProxy *proxy, IMAGE *im )
{
	g_assert( proxy->im == im );

	if( proxy->call ) {
#ifdef DEBUG_HISTORY
		printf( "vips_call_close_cb: on close, uncaching \"%s\"\n", 
			proxy->call->fn->name );
#endif /*DEBUG_HISTORY*/

		vips_call_destroy( proxy->call );
	}

	return( 0 );
}

static int
vips_call_invalidate_cb( VipsProxy *proxy, IMAGE *im )
{
	g_assert( proxy->im == im );

	if( proxy->call ) {
#ifdef DEBUG_HISTORY
		printf( "vips_call_invalidate_cb: "
			"on invalidate, uncaching \"%s\"\n", 
			proxy->call->fn->name );
#endif /*DEBUG_HISTORY*/

		vips_call_destroy( proxy->call );
	}

	return( 0 );
}

/* Associate an IMAGE with a VipsCall via a proxy.
 */
static VipsProxy *
vips_call_proxy( VipsCall *call, IMAGE *im )
{
	VipsProxy *proxy;

	if( !(proxy = IM_NEW( im, VipsProxy )) )
		return( NULL );
	proxy->im = im;
	proxy->call = call;
	call->proxys = g_slist_prepend( call->proxys, proxy );

	if( im_add_close_callback( im, 
		(im_callback_fn) vips_call_close_cb, proxy, im ) )
		return( NULL );
	if( im_add_invalidate_callback( im, 
		(im_callback_fn) vips_call_invalidate_cb, proxy, im ) )
		return( NULL );

	return( proxy );
}

/* Add a function call to the history. 
 */
static void
vips_call_add( VipsCall *call )
{
	int i;

#ifdef DEBUG_HISTORY
	printf( "vips_call_add: adding \"%s\" (%p), hash = %u\n", 
		call->fn->name, call, call->hash );
#endif /*DEBUG_HISTORY*/

	vips_call_insert( call );

	/* Link all input images to this call: we need to spot
	 * close/invalidate.
	 */
	for( i = 0; i < call->fn->argc; i++ ) {
		im_type_desc *ty = call->fn->argv[i].desc;

		if( strcmp( ty->type, IM_TYPE_IMAGE ) == 0 && 
			!(ty->flags & IM_TYPE_OUTPUT) ) 
			(void) vips_call_proxy( call, call->vargv[i] );

		if( strcmp( ty->type, IM_TYPE_IMAGEVEC ) == 0 && 
			!(ty->flags & IM_TYPE_OUTPUT) ) {
			im_imagevec_object *iv = 
				(im_imagevec_object *) call->vargv[i];
			int j;

			for( j = 0; j < iv->n; j++ ) 
				(void) vips_call_proxy( call, iv->vec[j] );
		}
	}

	/* History too big? Flush!
	 */
	if( queue_length( vips_call_lru ) > VIPS_HISTORY_MAX ) 
		vips_call_remove_lru();
}

VipsCall *
vips_call_start( im_function *fn )
{
	VipsCall *call;

	if( !fn || !(call = IM_NEW( NULL, VipsCall )) )
		return( NULL );
	call->fn = fn;
	call->vargv = NULL;
	call->found_hash = FALSE;
	call->in_cache = FALSE;
#ifdef DEBUG_LEAK
	vips_call_all = g_slist_prepend( vips_call_all, call );
#endif /*DEBUG_LEAK*/

	/* Make the call spine, alloc memory. 
	 */
	if( !(call->vargv = IM_ARRAY( NULL, call->fn->argc + 1, im_object )) ||
		im_allocate_vargv( call->fn, call->vargv ) ) {
		vips_call_destroy( call );
		return( NULL );
	}

	return( call );
}

/* VIPS types -> a string buffer. Used to update image history. Yuk! Should be
 * a method on object type.
 */
static void
vips_call_tobuf( VipsCall *call, int i, BufInfo *buf )
{
	im_object obj = call->vargv[i];
	im_type_desc *vips = call->fn->argv[i].desc;
	VipsArgumentType vt = vips_type_find( vips->type );

	switch( vt ) {
	case VIPS_DOUBLE:
		buf_appendf( buf, "%g", *((double*)obj) );
		break;

	case VIPS_INT:
		buf_appendf( buf, "%d", *((int*)obj) );
		break;

	case VIPS_COMPLEX:
		buf_appendf( buf, "(%g, %g)", 
			((double*)obj)[0], ((double*)obj)[1] );
		break;

	case VIPS_STRING:
		buf_appendf( buf, "\"%s\"", (char*)obj );
		break;

	case VIPS_IMAGE:
		buf_appendf( buf, "%s", NN( ((IMAGE*)obj)->filename ) );
		break;

	case VIPS_DMASK:
	case VIPS_IMASK:
	{
		im_mask_object *mo = obj;

		buf_appendf( buf, "%s", NN( mo->name ) );
		break;
	}

	case VIPS_DOUBLEVEC:
	{
		im_doublevec_object *v = (im_doublevec_object *) obj;
		int j;

		buf_appendf( buf, "\"" );
		for( j = 0; j < v->n; j++ )
			buf_appendf( buf, "%g ", v->vec[j] );
		buf_appendf( buf, "\"" );

		break;
	}

	case VIPS_INTVEC:
	{
		im_intvec_object *v = (im_intvec_object *) obj;
		int j;

		buf_appendf( buf, "\"" );
		for( j = 0; j < v->n; j++ )
			buf_appendf( buf, "%d ", v->vec[j] );
		buf_appendf( buf, "\"" );

		break;
	}

	case VIPS_IMAGEVEC:
	{
		im_imagevec_object *v = (im_imagevec_object *) obj;
		int j;

		buf_appendf( buf, "\"" );
		for( j = 0; j < v->n; j++ )
			buf_appendf( buf, "%s ", v->vec[j]->filename );
		buf_appendf( buf, "\"" );

		break;
	}

	case VIPS_GVALUE:
	{
		GValue *value = (GValue *) obj;

		buf_appendgv( buf, value );

		break;
	}

	case VIPS_NONE:
		if( strcmp( vips->type, IM_TYPE_DISPLAY ) == 0 ) 
			/* Just assume sRGB.
			 */
			buf_appendf( buf, "sRGB" );
		break;

	default:
		g_assert( FALSE );
	}
}

static gboolean
vips_call_build_argv( VipsCall *call, char **argv )
{
	int i;

	for( i = 0; i < call->fn->argc; i++ ) {
		BufInfo buf;
		char txt[512];

		buf_init_static( &buf, txt, 512 );
		vips_call_tobuf( call, i, &buf );
		if( !(argv[i] = im_strdup( NULL, buf_all( &buf ) )) )
			return( FALSE );
	}

#ifdef DEBUG
	printf( "vips_call_build_argv: argv for %s is:\n  ", call->fn->name );
	for( i = 0; i < call->fn->argc; i++ )
		printf( "%s ", NN( argv[i] ) );
	printf( "\n" );
#endif /*DEBUG*/

	return( TRUE );
}

static void
vips_call_free_argv( int argc, char **argv )
{
	int i;

	for( i = 0; i < argc; i++ ) {
		IM_FREE( argv[i] );
	}
	IM_FREE( argv );
}

/* Update the VIPS hist for all output images.
 */
static void
vips_call_update_hist( VipsCall *call )
{
	int argc = call->fn->argc;
	char **argv;
	int i;

#ifdef DEBUG
	printf( "vips_call_update_hist: %s\n", call->fn->name );
#endif /*DEBUG*/

	/* Build an argv for this call. +1 for NULL termination.
	 */
	if( !(argv = IM_ARRAY( NULL, argc + 1, char * )) )
		return;
	for( i = 0; i < argc + 1; i++ )
		argv[i] = NULL;
	if( !vips_call_build_argv( call, argv ) ) {
		vips_call_free_argv( argc, argv );
		return;
	}

	/* Update the history on all output images.
	 */
	for( i = 0; i < call->fn->argc; i++ ) {
		im_type_desc *ty = call->fn->argv[i].desc;

		if( strcmp( ty->type, IM_TYPE_IMAGE ) == 0 && 
			(ty->flags & IM_TYPE_OUTPUT) ) 
			im_updatehist( call->vargv[i], 
				call->fn->name, argc, argv );

		if( strcmp( ty->type, IM_TYPE_IMAGEVEC ) == 0 && 
			(ty->flags & IM_TYPE_OUTPUT) ) {
			im_imagevec_object *iv = 
				(im_imagevec_object *) call->vargv[i];
			int j;

			for( j = 0; j < iv->n; j++ ) 
				im_updatehist( iv->vec[j], 
					call->fn->name, argc, argv );
		}
	}

	vips_call_free_argv( argc, argv );
}

VipsCall *
vips_call_dispatch( VipsCall *call )
{
	VipsCall *old_call;

	/* Is this function call in the history?
	 */
	if( (old_call = vips_call_lookup( call )) ) {
		/* Yes: reuse!
		 */
		vips_call_destroy( call );
		call = old_call;

#ifdef DEBUG_HISTORY
		printf( "vips_dispatch: found %s in history\n", call->fn->name );
#endif /*DEBUG_HISTORY*/
	}
	else {
		/* No: call function.
		 */
#ifdef DEBUG_TIME
		static GTimer *timer = NULL;

		if( !timer )
			timer = g_timer_new();
		g_timer_reset( timer );
#endif /*DEBUG_TIME*/

#ifdef DEBUG_HISTORY_MISS
		printf( "vips_dispatch: calling %s\n", call->fn->name );
#endif /*DEBUG_HISTORY_MISS*/

		/* Be careful. Eval callbacks from this may do anything,
		 * including call vips_call_dispatch().
		 */
		call->result = call->fn->disp( call->vargv );

#ifdef DEBUG_TIME
		printf( "vips_dispatch: %s - %g\n", 
			call->fn->name, g_timer_elapsed( timer, NULL ) );
#endif /*DEBUG_TIME*/

		if( !call->result )
			vips_call_update_hist( call );
	}

	/* Add to our operation cache, if necessary.
	 */
	if( call->fn->flags & IM_FN_NOCACHE ) 
		/* Uncacheable operation.
		 */
		;
	else if( call->result )
		/* Failed: don't cache.
		 */
		;
	else if( call->in_cache ) 
		/* Already in the history. Just touch the time.
		 */
		vips_call_touch( call );
	else if( (old_call = vips_call_lookup( call )) ) {
		/* We have an equal but older item there? This can happen with
		 * nested calls. Touch the old one and destroy this one.
		 */
		vips_call_touch( old_call );
		vips_call_destroy( call );
		call = old_call;
	}
	else
		vips_call_add( call );

	return( call );
}

void
vips_call_stop( VipsCall *call )
{
	if( !call->in_cache )
		vips_call_destroy( call );
}

void
vips_call_check_all_destroyed( void )
{
#ifdef DEBUG_LEAK
	if( vips_call_all != NULL ) {
		GSList *p;

		printf( "** %d VipsCall leaked!\n", 
			g_slist_length( vips_call_all ) );
		printf( "(operations with no image args are not leaks)\n" );

		for( p = vips_call_all; p; p = p->next ) {
			VipsCall *call = (VipsCall *) p->data;

			printf( "\t%s\n", call->fn->name );
		}
	}
#endif /*DEBUG_LEAK*/
}
