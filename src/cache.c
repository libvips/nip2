/* Call vips functions, cache recent results
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
#define DEBUG_TIME
#define DEBUG_HISTORY_SANITY
#define DEBUG_HISTORY_MISS
#define DEBUG_HISTORY
#define DEBUG
 */

/* This is usually turned on from a -D in cflags.
#define DEBUG_LEAK
 */

/* Often want it off ... we get spurious complaints about leaks if an
 * operation has no images in or out (eg. im_version) because it'll never
 * get GCed.
#undef DEBUG_LEAK
 */

/* The previous function calls we are caching, plus an LRU queue for flushing.
 */
static GHashTable *cache_history_table = NULL;
static Queue *cache_history_lru = NULL;
int cache_history_size = 0;

/* Hash from a vargv ... just look at input args and the function name.
 */
static unsigned int
cache_hash( CallInfo *vi )
{
	int i;
	unsigned int hash;

	if( vi->found_hash )
		return( vi->hash );

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
	HASH_P( vi->fn );

        for( i = 0; i < vi->fn->argc; i++ ) {
                im_type_desc *ty = vi->fn->argv[i].desc;

                if( call_type_needs_input( ty ) ) {
			switch( call_lookup_type( ty->type ) ) {
			case CALL_DOUBLE:
				HASH_D( *((double *) vi->vargv[i]) );
				break;

			case CALL_INT:
				HASH_I( *((int *) vi->vargv[i]) );
				break;

			case CALL_COMPLEX:
				HASH_D( ((double *) vi->vargv[i])[0] );
				HASH_D( ((double *) vi->vargv[i])[1] );
				break;

			case CALL_STRING:
				HASH_S( (char *) vi->vargv[i] );
				break;

			case CALL_GVALUE:
			case CALL_INTERPOLATE:
				break;

			case CALL_DOUBLEVEC:
			{
				im_doublevec_object *v = 
					(im_doublevec_object *) vi->vargv[i];
				int j;

				for( j = 0; j < v->n; j++ )
					HASH_D( v->vec[j] );

				break;
			}

			case CALL_INTVEC:
			{
				im_intvec_object *v = 
					(im_intvec_object *) vi->vargv[i];
				int j;

				for( j = 0; j < v->n; j++ )
					HASH_I( v->vec[j] );

				break;
			}

			case CALL_DMASK:
			{
				im_mask_object *mo = vi->vargv[i];
				DOUBLEMASK *mask = mo->mask;

				/* mask can be NULL if we are called after 
				 * call_new() but before we've built the arg
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

			case CALL_IMASK:
			{
				im_mask_object *mo = vi->vargv[i];
				INTMASK *mask = mo->mask;

				/* mask can be NULL if we are called after 
				 * call_new() but before we've built the arg
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

			default:
			case CALL_NONE:
				break;
			}
		}
        }

	/* And the input images.
	 */
	for( i = 0; i < vi->ninii; i++ )
		HASH_P( vi->inii[i] );

	vi->found_hash = TRUE;
	vi->hash = hash;

	return( hash );
}

/* Are two function calls equal. Check the func and the input args.
 */
static gboolean
cache_equal( CallInfo *vi1, CallInfo *vi2 ) 
{
	int i;
	im_function *fn = vi1->fn;

	if( vi1 == vi2 )
		return( TRUE );

	if( vi1->fn != vi2->fn )
		return( FALSE );

        for( i = 0; i < fn->argc; i++ ) {
                im_type_desc *ty = fn->argv[i].desc;

                if( call_type_needs_input( ty ) ) {
			switch( call_lookup_type( ty->type ) ) {
			case CALL_DOUBLE:
				if( *((double *) vi1->vargv[i]) != 
					*((double *) vi2->vargv[i]) )
					return( FALSE );
				break;

			case CALL_INT:
				if( *((int *) vi1->vargv[i]) != 
					*((int *) vi2->vargv[i]) )
					return( FALSE );
				break;

			case CALL_COMPLEX:
				if( ((double *) vi1->vargv[i])[0] != 
					((double *) vi2->vargv[i])[0] )
					return( FALSE );
				if( ((double *) vi1->vargv[i])[1] != 
					((double *) vi2->vargv[i])[1] )
					return( FALSE );
				break;

			case CALL_STRING:
				if( strcmp( (char *) vi1->vargv[i],
					(char *) vi2->vargv[i] ) != 0 )
					return( FALSE );
				break;

			case CALL_DOUBLEVEC:
			{
				im_doublevec_object *v1 = 
					(im_doublevec_object *) vi1->vargv[i];
				im_doublevec_object *v2 = 
					(im_doublevec_object *) vi2->vargv[i];
				int j;

				for( j = 0; j < v1->n; j++ )
					if( v1->vec[j] != v2->vec[j] )
						return( FALSE );

				break;
			}

			case CALL_INTVEC:
			{
				im_intvec_object *v1 = 
					(im_intvec_object *) vi1->vargv[i];
				im_intvec_object *v2 = 
					(im_intvec_object *) vi2->vargv[i];
				int j;

				for( j = 0; j < v1->n; j++ )
					if( v1->vec[j] != v2->vec[j] )
						return( FALSE );

				break;
			}

			case CALL_DMASK:
			{
				im_mask_object *mo1 = 
					(im_mask_object *) vi1->vargv[i];
				im_mask_object *mo2 = 
					(im_mask_object *) vi2->vargv[i];
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

			case CALL_IMASK:
			{
				im_mask_object *mo1 = 
					(im_mask_object *) vi1->vargv[i];
				im_mask_object *mo2 = 
					(im_mask_object *) vi2->vargv[i];
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

			case CALL_IMAGEVEC:
			{
				im_imagevec_object *v1 = 
					(im_imagevec_object *) vi1->vargv[i];
				im_imagevec_object *v2 = 
					(im_imagevec_object *) vi2->vargv[i];

				if( v1->n != v2->n )
					return( FALSE );

				break;
			}

			/* Very strict. Could be more generous here: we'd need
			 * to have a pspec for each argument type and then use 
			 * g_param_values_cmp() to test equality.
			 */
			case CALL_GVALUE:
				if( vi1->vargv[i] != vi2->vargv[i] )
					return( FALSE );
				break;

			case CALL_INTERPOLATE:
				if( vi1->vargv[i] != vi2->vargv[i] )
					return( FALSE );
				break;

			default:
			case CALL_NONE:
				break;
			}
		}
        }

	/* And the input images.
	 */
	if( vi1->ninii != vi2->ninii )
		return( FALSE );
	for( i = 0; i < vi1->ninii; i++ )
		if( vi1->inii[i] != vi2->inii[i] )
			return( FALSE );

	return( TRUE );
}

#ifdef DEBUG_HISTORY_SANITY
static void
cache_history_sanity_sub( CallInfo *vi )
{
	g_assert( g_slist_find( cache_history_lru->list, vi ) );
}

static void
cache_history_sanity( void )
{
	GSList *p;

	if( !cache_history_lru || !cache_history_table )
		return;

	/* Everything that's on the LRU should be in the history table.
	 */
	for( p = cache_history_lru->list; p; p = p->next ) {
		CallInfo *vi = (CallInfo *) p->data;

		g_assert( g_hash_table_lookup( cache_history_table, vi ) );

		g_assert( vi->fn );
		g_assert( vi->fn->argc > 0 && vi->fn->argc < MAX_CALL_ARGS );
		g_assert( vi->in_cache );
	}

	/* Everything that's on the history table should be in the LRU.
	 */
	g_hash_table_foreach( cache_history_table,
		(GHFunc) cache_history_sanity_sub, NULL );
}
#endif /*DEBUG_HISTORY_SANITY */

/* Is a function call in our history? Return the old one. 
 */
static CallInfo *
cache_history_lookup( CallInfo *vi )
{
	CallInfo *old_vi;

	if( !cache_history_table ) {
		cache_history_table = g_hash_table_new( 
			(GHashFunc) cache_hash, (GEqualFunc) cache_equal );
		cache_history_lru = queue_new();
	}

	old_vi = (CallInfo *) g_hash_table_lookup( cache_history_table, vi );

#ifdef DEBUG_HISTORY
	if( old_vi ) 
		printf( "cache_history_lookup: found \"%s\"\n", old_vi->name );
#endif /*DEBUG_HISTORY*/
#ifdef DEBUG_HISTORY_SANITY
	cache_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/

	return( old_vi );
}

/* Bump to end of LRU.
 */
static void
cache_history_touch( CallInfo *vi )
{
	g_assert( vi->in_cache );

	queue_remove( cache_history_lru, vi );
	queue_add( cache_history_lru, vi );

#ifdef DEBUG_HISTORY_SANITY
	cache_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/
#ifdef DEBUG_HISTORY
	printf( "cache_history_touch: bumping \"%s\"\n", vi->name );
#endif /*DEBUG_HISTORY*/
}

/* Are we in the history? Remove us. Called from cache_info_dispose() on unref, 
 * don't call this directly.
 */
void
cache_history_remove( CallInfo *vi )
{
	int i;

	if( vi->in_cache ) {
		queue_remove( cache_history_lru, vi );
		g_hash_table_remove( cache_history_table, vi );
		cache_history_size -= 1;
		vi->in_cache = FALSE;

#ifdef DEBUG_HISTORY
		printf( "cache_history_remove: removing \"%s\"\n", vi->name );
#endif /*DEBUG_HISTORY*/
	}

	/* Disconnect signals.
	 */
	for( i = 0; i < vi->noutii; i++ ) 
		FREESID( vi->outii_destroy_sid[i], vi->outii[i] ); 
	for( i = 0; i < vi->ninii; i++ ) {
		FREESID( vi->inii_destroy_sid[i], vi->inii[i] ); 
		FREESID( vi->inii_invalidate_sid[i], vi->inii[i] ); 
	}

#ifdef DEBUG_HISTORY_SANITY
	cache_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/
}

static void
cache_history_remove_lru( void )
{
	CallInfo *vi;

	vi = (CallInfo *) queue_head( cache_history_lru );

#ifdef DEBUG_HISTORY
	printf( "cache_history_remove_lru: flushing \"%s\"\n", vi->name );
#endif /*DEBUG_HISTORY*/

	g_object_unref( vi );

#ifdef DEBUG_HISTORY_SANITY
	cache_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/
}

static void
cache_history_destroy_cb( Imageinfo *ii, CallInfo *vi )
{
#ifdef DEBUG_HISTORY
	printf( "cache_history_destroy_cb: on death of ii, uncaching \"%s\"\n", 
		vi->name );
#endif /*DEBUG_HISTORY*/

	g_object_unref( vi );
}

static void
cache_history_invalidate_cb( Imageinfo *ii, CallInfo *vi )
{
#ifdef DEBUG_HISTORY
	printf( "cache_history_invalidate_cb: "
		"on invalidate of ii, uncaching \"%s\"\n", vi->name );
#endif /*DEBUG_HISTORY*/

	g_object_unref( vi );
}

/* Add a function call to the history. 
 */
static void
cache_history_add( CallInfo *vi )
{
	int i;

#ifdef DEBUG_HISTORY_SANITY
	cache_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/

#ifdef DEBUG_HISTORY
	printf( "cache_history_add: adding \"%s\" (%p), hash = %u\n", 
		vi->name, vi, vi->hash );
#endif /*DEBUG_HISTORY*/

	g_assert( !g_hash_table_lookup( cache_history_table, vi ) );
	g_assert( !vi->in_cache );

	g_hash_table_insert( cache_history_table, vi, vi );
	cache_history_size += 1;

	g_assert( g_hash_table_lookup( cache_history_table, vi ) );

	queue_add( cache_history_lru, vi );
	vi->in_cache = TRUE;
	g_object_ref( vi );

	/* If any of our ii are destroyed, we must go too.
	 */
	for( i = 0; i < vi->noutii; i++ )
		vi->outii_destroy_sid[i] = g_signal_connect( vi->outii[i], 
			"destroy", 
			G_CALLBACK( cache_history_destroy_cb ), vi );

	/* If any of our input ii are destroyed or painted on, we must also 
	 * uncache.
	 */
	for( i = 0; i < vi->ninii; i++ ) {
		vi->inii_destroy_sid[i] = g_signal_connect( vi->inii[i], 
			"destroy", 
			G_CALLBACK( cache_history_destroy_cb ), vi );
		vi->inii_invalidate_sid[i] = g_signal_connect( vi->inii[i], 
			"invalidate", 
			G_CALLBACK( cache_history_invalidate_cb ), vi );
	}

	/* History too big? Flush!
	 */
	if( queue_length( cache_history_lru ) > CALL_HISTORY_MAX ) 
		cache_history_remove_lru();

#ifdef DEBUG_HISTORY_SANITY
	cache_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/
}

/* Sort out the input images. 
 */
static gboolean
cache_gather( CallInfo *vi ) 
{
	int i, j;
	int ni;

	/* No input images.
	 */
	if( vi->ninii == 0 )
		return( TRUE );

	/* Can we LUT? Function needs to be LUTable, all input images
	 * have to be the same underlying image, and image must be uncoded
	 * IM_BANDFMT_UCHAR.
	 */
	vi->use_lut = (vi->fn->flags & IM_FN_PTOP) && 
		imageinfo_same_underlying( vi->inii, vi->ninii ) &&
		imageinfo_get_underlying( vi->inii[0] )->Coding == 
			IM_CODING_NONE &&
		imageinfo_get_underlying( vi->inii[0] )->BandFmt == 
			IM_BANDFMT_UCHAR;

	if( vi->use_lut ) 
		for( i = 0; i < vi->noutii; i++ )
			imageinfo_set_underlying( vi->outii[i], vi->inii[0] );

	/* Now fill the vargv vector with the IMAGE pointers.
	 */
	ni = 0;
	for( i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;

		if( !call_type_needs_input( ty ) ) 
			continue;

		if( strcmp( ty->type, IM_TYPE_IMAGE ) == 0 ) { 
			Imageinfo *inii = vi->inii[ni++];
			IMAGE *im;

			if( !(im = imageinfo_get( vi->use_lut, inii )) )
				return( FALSE );

			/* RW operations need an extra copy. Tyhe vargv will
			 * already have been created by cache_build_output().
			 */
			if( ty->flags & IM_TYPE_RW ) {
				if( im_copy( im, vi->vargv[i] ) )
					return( FALSE );
			}
			else
				vi->vargv[i] = im; 
		}

		if( strcmp( ty->type, IM_TYPE_IMAGEVEC ) == 0 ) {
			im_imagevec_object *iv = 
				(im_imagevec_object *) vi->vargv[i];

			/* Found an input image vector. Add all the imageinfo
			 * in the vector.
			 */
			for( j = 0; j < iv->n; j++ ) {
				Imageinfo *inii = vi->inii[ni++];
				IMAGE *im;

				if( !(im = imageinfo_get( vi->use_lut, inii )) )
					return( FALSE );

				iv->vec[j] = im;
			}
		}
	}

	/* We should have used up all the images exactly.
	 */
	g_assert( ni == vi->ninii );

	return( TRUE );
}

/* VIPS types -> a string buffer. Yuk! Should be a method on object type. This
 * is used to generate vips history, so it has to be in sh format.
 */
void
cache_tochar_shell( CallInfo *vi, int i, VipsBuf *buf )
{
	im_object obj = vi->vargv[i];
	im_type_desc *ty = vi->fn->argv[i].desc;

	switch( call_lookup_type( ty->type ) ) {
	case CALL_DOUBLE:
		vips_buf_appendf( buf, "%g", *((double*)obj) );
		break;

	case CALL_INT:
		vips_buf_appendf( buf, "%d", *((int*)obj) );
		break;

	case CALL_COMPLEX:
		vips_buf_appendf( buf, "(%g, %g)", 
			((double*)obj)[0], ((double*)obj)[1] );
		break;

	case CALL_STRING:
		vips_buf_appendf( buf, "\"%s\"", (char*)obj );
		break;

	case CALL_IMAGE:
{
		IMAGE *im = (IMAGE *) obj;

		/* In quotes, in case there are spaces in the
		 * filename. We also need to test im, as we might be called
		 * before the im has been generated.
		 */
		vips_buf_appendf( buf, "\"%s\"", im ? im->filename : "null" );

		break;
}

	case CALL_DMASK:
	case CALL_IMASK:
	{
		im_mask_object *mo = obj;

		vips_buf_appendf( buf, "%s", NN( mo->name ) );

		break;
	}

	case CALL_DOUBLEVEC:
	{
		im_doublevec_object *v = (im_doublevec_object *) obj;
		int j;

		vips_buf_appendf( buf, "\"" );
		for( j = 0; j < v->n; j++ )
			vips_buf_appendf( buf, "%g ", v->vec[j] );
		vips_buf_appendf( buf, "\"" );

		break;
	}

	case CALL_INTVEC:
	{
		im_intvec_object *v = (im_intvec_object *) obj;
		int j;

		vips_buf_appendf( buf, "\"" );
		for( j = 0; j < v->n; j++ )
			vips_buf_appendf( buf, "%d ", v->vec[j] );
		vips_buf_appendf( buf, "\"" );

		break;
	}

	case CALL_IMAGEVEC:
	{
		im_imagevec_object *v = (im_imagevec_object *) obj;
		int j;

		vips_buf_appendf( buf, "\"" );
		for( j = 0; j < v->n; j++ ) 
			vips_buf_appendf( buf, "%s ", v->vec[j]->filename );
		vips_buf_appendf( buf, "\"" );

		break;
	}

	case CALL_GVALUE:
	{
		GValue *value = (GValue *) obj;

		vips_buf_appendgv( buf, value );

		break;
	}

	case CALL_INTERPOLATE:
		vips_object_to_string( VIPS_OBJECT( obj ), buf );
		break;

	case CALL_NONE:
		if( strcmp( ty->type, IM_TYPE_DISPLAY ) == 0 ) 
			/* Just assume sRGB.
			 */
			vips_buf_appendf( buf, "sRGB" );
		break;

	default:
		g_assert( FALSE );
	}
}

/* VIPS types -> a buffer. For tracing calls and debug.
 */
void
cache_tochar_trace( CallInfo *vi, int i, VipsBuf *buf )
{
	im_object obj = vi->vargv[i];
	im_type_desc *vips = vi->fn->argv[i].desc;

	switch( call_lookup_type( vips->type ) ) {
	case CALL_DOUBLE:
		vips_buf_appendf( buf, "%g", *((double*)obj) );
		break;

	case CALL_INT:
		vips_buf_appendf( buf, "%d", *((int*)obj) );
		break;

	case CALL_COMPLEX:
		vips_buf_appendf( buf, "(%g, %g)", 
			((double*)obj)[0], ((double*)obj)[1] );
		break;

	case CALL_STRING:
		vips_buf_appendf( buf, "\"%s\"", (char*) obj );
		break;

	case CALL_IMAGE:
		vips_buf_appendi( buf, (IMAGE *) obj );
		break;

	case CALL_DMASK:
		vips_buf_appendf( buf, "dmask" );
		break;

	case CALL_IMASK:
		vips_buf_appendf( buf, "imask" );
		break;

	case CALL_DOUBLEVEC:
		vips_buf_appendf( buf, "doublevec" );
		break;

	case CALL_INTVEC:
		vips_buf_appendf( buf, "intvec" );
		break;

	case CALL_IMAGEVEC:
		vips_buf_appendf( buf, "imagevec" );
		break;

	case CALL_GVALUE:
	{
		GValue *value = (GValue *) obj;

		vips_buf_appends( buf, "(gvalue" );
		vips_buf_appendgv( buf, value );
		vips_buf_appendf( buf, ")" );

		break;
	}

	case CALL_INTERPOLATE:
		vips_object_to_string( VIPS_OBJECT( obj ), buf );
		break;

	default:
		g_assert( FALSE );
	}
}

/* Get the args from the VIPS call buffer.
 */
static void
cache_args_vips( CallInfo *vi, VipsBuf *buf )
{
	int i;

	vips_buf_appendf( buf, _( "You passed:" ) );
	vips_buf_appendf( buf, "\n" );
        for( i = 0; i < vi->fn->argc; i++ ) {
                im_type_desc *ty = vi->fn->argv[i].desc;
                char *name = vi->fn->argv[i].name;

                if( call_type_needs_input( ty ) ) {
                        vips_buf_appendf( buf, "   %s - ", name );
                        cache_tochar_trace( vi, i, buf );
                        vips_buf_appendf( buf, "\n" );
                }
        }
}

/* There's a problem calling the function. Show args from the vips call
 * struct.
 */
static void
cache_error_fn_vips( CallInfo *vi )
{
	char txt[1000];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	error_top( _( "VIPS library error." ) );

	vips_buf_appendf( &buf, 
		_( "Error calling library function \"%s\" (%s)." ),
		vi->name, vi->fn->desc );
	vips_buf_appendf( &buf, "\n" );
	vips_buf_appendf( &buf, _( "VIPS library: %s" ), im_error_buffer() );
	im_error_clear();
	vips_buf_appendf( &buf, "\n" );
	cache_args_vips( vi, &buf );
	vips_buf_appendf( &buf, "\n" );
	call_usage( &buf, vi->fn );
	error_sub( "%s", vips_buf_all( &buf ) );
}

static gboolean
cache_build_argv( CallInfo *vi, char **argv )
{
	int i;

	for( i = 0; i < vi->fn->argc; i++ ) {
		char txt[512];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		cache_tochar_shell( vi, i, &buf );
		if( !(argv[i] = im_strdup( NULL, vips_buf_all( &buf ) )) )
			return( FALSE );
	}

#ifdef DEBUG
	printf( "cache_build_argv: argv for %s is:\n  ", vi->fn->name );
	for( i = 0; i < vi->fn->argc; i++ )
		printf( "%s ", NN( argv[i] ) );
	printf( "\n" );
#endif /*DEBUG*/

	return( TRUE );
}

static void
cache_free_argv( int argc, char **argv )
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
cache_update_hist( CallInfo *vi )
{
	int argc = vi->fn->argc;
	char **argv;
	int i;

#ifdef DEBUG
	printf( "cache_update_hist: %s\n", vi->name );
#endif /*DEBUG*/

	/* No output images? Nothing to do.
	 */
	if( vi->nires == 0 )
		return;

	/* Build an argv for this call. +1 for NULL termination.
	 */
	if( !(argv = IM_ARRAY( NULL, argc + 1, char * )) )
		return;
	for( i = 0; i < argc + 1; i++ )
		argv[i] = NULL;
	if( !cache_build_argv( vi, argv ) ) {
		cache_free_argv( argc, argv );
		return;
	}

	for( i = 0; i < vi->nres; i++ ) {
		int j = vi->outpos[i];
		im_type_desc *ty = vi->fn->argv[j].desc;

		/* Image output.
		 */
		if( call_lookup_type( ty->type ) == CALL_IMAGE ) {
#ifdef DEBUG
			printf( "cache_update_hist: adding to arg %d\n", j );
#endif /*DEBUG*/

			im_updatehist( vi->vargv[j], vi->fn->name, argc, argv );
		}
	}

	cache_free_argv( argc, argv );
}

/* Call a vips operation. 
 *
 * The cache takes ownership of the CallInfo passed in, and returns a ref to a
 * CallInfo (might be a different one) that contains the result. Should be 
 * unreffed when you're done with it.
 *
 * On error, return NULL.
 */
CallInfo *
cache_dispatch( CallInfo *vi, PElement *out )
{
	CallInfo *old_vi;

#ifdef DEBUG_HISTORY_SANITY
	cache_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/

	/* Calculate the hash for this vi after building it, but before we do
	 * cache_gather();
	 *
	 * We want the hash to reflect the args as supplied by nip2, not the
	 * args as transformed by cache_gather() for this specific call.
	 */
	(void) cache_hash( vi );

	/* Look over the images we have and turn input Imageinfos to IMAGEs.
	 * If we can do this with a lut, set all that up.
	 */
	if( !cache_gather( vi ) ) {
		g_object_unref( vi );
		return( NULL );
	}

	/* We have to show args after gather, since the tracer wants IMAGE not
	 * Imageinfo.
	 */
#ifdef DEBUG
{
	int i;

	for( i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;

		char txt[512];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		printf( "cache_fill_spine: arg[%d] (%s) = ", i, ty->type );
		cache_tochar_trace( vi, i, &buf );
		printf( "%s\n", vips_buf_all( &buf ) );
	}
}
#endif /*DEBUG*/

	/* Is this function call in the history?
	 */
	if( (old_vi = cache_history_lookup( vi )) ) {
		/* Yes: reuse! unref our arg to junk it, adda ref to the
		 * cached call for our caller.
		 */
		g_object_unref( vi );
		vi = old_vi;
		g_object_ref( vi );

		if( trace_flags & TRACE_VIPS ) 
			vips_buf_appendf( trace_current(), "(from cache) " );

#ifdef DEBUG_HISTORY
		printf( "cache_dispatch: found %s in history\n", vi->name );
#endif /*DEBUG_HISTORY*/
	}
	else {
		/* No: call function.
		 */
		int result;

#ifdef DEBUG_TIME
		static GTimer *timer = NULL;

		if( !timer )
			timer = g_timer_new();
		g_timer_reset( timer );
#endif /*DEBUG_TIME*/

#ifdef DEBUG_HISTORY_MISS
		printf( "cache_dispatch: calling %s\n", vi->name );
#endif /*DEBUG_HISTORY_MISS*/

		/* Be careful. Eval callbacks from this may do anything,
		 * including call cache_dispatch().
		 */
		result = vi->fn->disp( vi->vargv );

#ifdef DEBUG_TIME
		printf( "cache_dispatch: %s - %g seconds\n", 
			vi->name, g_timer_elapsed( timer, NULL ) );
#endif /*DEBUG_TIME*/

		if( result ) {
			cache_error_fn_vips( vi );
			g_object_unref( vi );
			return( NULL );
		}
		cache_update_hist( vi );
	}

	/* Add to our operation cache, if necessary.
	 */
	if( !(vi->fn->flags & IM_FN_NOCACHE) ) {
		if( vi->in_cache ) 
			/* Already in the history. Just touch the time.
			 */
			cache_history_touch( vi );
		else if( (old_vi = cache_history_lookup( vi )) ) {
			/* We have an equal but older item there? This can 
			 * happen with nested calls. Touch the old one.
			 */
			cache_history_touch( old_vi );
			vi = old_vi;
		}
		else
			cache_history_add( vi );
	}

#ifdef DEBUG_HISTORY_SANITY
	cache_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/

	return( vi );
}

