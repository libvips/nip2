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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */

/*

    These files are distributed with CALL - http://www.vips.ecs.soton.ac.uk

 */

#include "ip.h"

/*
#define DEBUG_TIME
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

G_DEFINE_TYPE( CallInfo, call_info, TYPE_IOBJECT ); 

/* CALL argument types we support. Keep order in sync with CallArgumentType.
 */
static im_arg_type call_supported[] = {
	IM_TYPE_DOUBLE,		
	IM_TYPE_INT,	
	IM_TYPE_COMPLEX,
	IM_TYPE_STRING,
	IM_TYPE_IMAGE,
	IM_TYPE_DOUBLEVEC,
	IM_TYPE_DMASK,
	IM_TYPE_IMASK,
	IM_TYPE_IMAGEVEC,
	IM_TYPE_INTVEC,
	IM_TYPE_GVALUE,
	IM_TYPE_INTERPOLATE
};


/* All the CallInfo we make ... for leak and sanity testing. Build this file 
 * with DEBUG_LEAK to enable add/remove to this list.
 */
GSList *call_info_all = NULL;

void
call_check_all_destroyed( void )
{
#ifdef DEBUG_LEAK
	int n_leaks;
	GSList *p;

	n_leaks = 0;
	for( p = call_info_all; p; p = p->next ) {
		CallInfo *vi = (CallInfo *) p->data;
	
		/* Operations which don't take an image as either an input or 
		 * output will stay in the cache. Don't report them.
		 */
		if( vi->ninii || vi->noutii ) {
			n_leaks += 1;
			printf( "\t%s\n", vi->name );
		}
	}

	if( n_leaks ) 
		printf( "** %d CallInfo leaked!\n", n_leaks ); 
#endif /*DEBUG_LEAK*/
}

/* Does a vips argument type require an argument from nip2?
 */
gboolean
call_type_needs_input( im_type_desc *ty )
{
	/* We supply these.
	 */
	if( strcmp( ty->type, IM_TYPE_DISPLAY ) == 0 ) 
		return( FALSE );

	if( !(ty->flags & IM_TYPE_OUTPUT) )
		return( TRUE );

	if( ty->flags & IM_TYPE_RW ) 
		return( TRUE );

	return( FALSE );
}

/* Will a vips argument type generate a result for nip2?
 */
gboolean
call_type_makes_output( im_type_desc *ty )
{
	/* We ignore these.
	 */
	if( strcmp( ty->type, IM_TYPE_DISPLAY ) == 0 ) 
		return( FALSE );

	if( ty->flags & (IM_TYPE_OUTPUT | IM_TYPE_RW) ) 
		return( TRUE );

	return( FALSE );
}

/* Error early on .. we can't print args yet.
 */
void
call_error( CallInfo *vi )
{
	error_top( _( "CALL library error." ) );
	error_sub( _( "Error calling library function \"%s\" (%s)." ), 
		vi->name, vi->fn->desc );
}

/* Get the args from the heap.
 */
static void
call_args_heap( CallInfo *vi, HeapNode **arg, VipsBuf *buf )
{
	int i;

	vips_buf_appendf( buf, _( "You passed:" ) );
	vips_buf_appendf( buf, "\n" );
	for( i = 0; i < vi->nargs; i++ ) {
		im_arg_desc *varg = &vi->fn->argv[vi->inpos[i]];
		PElement rhs;

		PEPOINTRIGHT( arg[vi->nargs - i - 1], &rhs );
		vips_buf_appendf( buf, "  %s - ", varg->name );
		itext_value_ev( vi->rc, buf, &rhs );
		vips_buf_appendf( buf, "\n" );
	}
}

/* Make a usage error for a CALL function.
 */
void
call_usage( VipsBuf *buf, im_function *fn )
{
	im_package *pack = im_package_of_function( fn->name );
	char input[MAX_STRSIZE];
	char output[MAX_STRSIZE];
	int nout, nin;
	int i;

	strcpy( input, "" );
	strcpy( output, "" );
	nin = 0;
	nout = 0;
	for( i = 0; i < fn->argc; i++ ) {
		im_arg_desc *arg = &fn->argv[i];
		char line[256];

		/* Format name, type message.
		 */
		im_snprintf( line, 256, 
			"   %s  - %s\n", arg->name, arg->desc->type );

		if( call_type_makes_output( arg->desc ) ) {
			strcat( output, line );
			nout++;
		}

		if( call_type_needs_input( arg->desc ) ) {
			strcat( input, line );
			nin++;
		}
	}

	vips_buf_appendf( buf, _( "Usage:" ) );
        vips_buf_appends( buf, "\n" );
        vips_buf_appendf( buf, _( "CALL operator \"%s\"" ), fn->name );
        vips_buf_appends( buf, "\n" );
        vips_buf_appendf( buf, _( "%s, from package \"%s\"" ), 
		fn->desc, pack->name );
        vips_buf_appends( buf, "\n" );

	vips_buf_appendf( buf, 
		ngettext( "\"%s\" takes %d argument:",
			"\"%s\" takes %d arguments:",
			nin ),
		fn->name, nin );
        vips_buf_appendf( buf, "\n%s", input );

	vips_buf_appendf( buf, 
		ngettext( "And produces %d result:",
			"And produces %d results:",
			nout ),
		nout );
	vips_buf_appendf( buf, "\n%s", output );

        /* Print any flags this function has.
         */
        vips_buf_appendf( buf, _( "Flags:" ) );
        vips_buf_appends( buf, "\n" );
	vips_buf_appendf( buf, "   (" );
        if( fn->flags & IM_FN_PIO )
                vips_buf_appendf( buf, _( "PIO function" ) );
        else
                vips_buf_appendf( buf, _( "WIO function" ) );
	vips_buf_appendf( buf, ") (" );
        if( fn->flags & IM_FN_TRANSFORM ) 
                vips_buf_appendf( buf, _( "coordinate transformer" ) );
        else
                vips_buf_appendf( buf, _( "no coordinate transformation" ) );
	vips_buf_appendf( buf, ") (" );
        if( fn->flags & IM_FN_PTOP )
                vips_buf_appendf( buf, _( "point-to-point operation" ) );
        else
                vips_buf_appendf( buf, _( "area operation" ) );
	vips_buf_appendf( buf, ") (" );
        if( fn->flags & IM_FN_NOCACHE )
                vips_buf_appendf( buf, _( "uncacheable operation" ) );
        else
                vips_buf_appendf( buf, _( "operation can be cached" ) );
        vips_buf_appendf( buf, ")\n" );
}

/* We know there's a problem exporting a particular arg to CALL.
 */
static void
call_error_arg( CallInfo *vi, HeapNode **arg, int argi )
{
	char txt[10000];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	error_top( _( "Bad argument." ) );

	vips_buf_appendf( &buf,
		_( "Argument %d (%s) to \"%s\" is the wrong type." ),
		argi + 1, vi->fn->argv[vi->inpos[argi]].name, vi->name );
	vips_buf_appendf( &buf, "\n" );
	call_args_heap( vi, arg, &buf );
	vips_buf_appendf( &buf, "\n" );
	call_usage( &buf, vi->fn );
	error_sub( "%s", vips_buf_all( &buf ) );
}

/* Too many args.
 */
void
call_error_toomany( CallInfo *vi )
{
	char txt[1000];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	error_top( _( "Too many arguments." ) );

	vips_buf_appendf( &buf,
		_( "Too many arguments to \"%s\"." ),
		vi->name );
	vips_buf_appendf( &buf, "\n" );
	call_usage( &buf, vi->fn );
	error_sub( "%s", vips_buf_all( &buf ) );
}

/* Look up a CALL type. 
 */
CallArgumentType
call_lookup_type( im_arg_type type )
{
	int i;

	for( i = 0; i < IM_NUMBER( call_supported ); i++ )
		if( strcmp( type, call_supported[i] ) == 0 )
			return( (CallArgumentType) i );

	error_top( _( "Unknown type." ) );
	error_sub( _( "CALL type \"%s\" not supported" ), type );

	return( CALL_NONE );
}

/* Is this the sort of CALL function we can call?
 */
gboolean
call_is_callable( im_function *fn )
{
	int i;
	int nout;
	int nin;

	if( fn->argc >= MAX_CALL_ARGS )
		return( FALSE );

        /* Check all argument types are supported. As well as the arg types
         * spotted by call_lookup_type, we also allow IM_TYPE_DISPLAY.
         */
        for( i = 0; i < fn->argc; i++ ) {
                im_arg_desc *arg = &fn->argv[i];
                im_arg_type vt = arg->desc->type;

                if( call_lookup_type( vt ) == CALL_NONE ) {
                        /* Unknown type .. if DISPLAY it's OK.
                         */
                        if( strcmp( vt, IM_TYPE_DISPLAY ) != 0 )
                                return( FALSE );
                }
        }

        nin = nout = 0;
        for( i = 0; i < fn->argc; i++ ) {
		im_type_desc *ty = fn->argv[i].desc;

		if( call_type_makes_output( ty ) ) 
			nout += 1;

		if( call_type_needs_input( ty ) ) 
			nin += 1;
	}

        /* Must be at least one output argument.
         */

        /* Must be at least one output argument.
         */
        if( nout == 0 ) 
                return( FALSE );

	/* Need at least 1 input argument: we reply on having an application
	 * node to overwrite with (I result).
	 */
        if( nin == 0 ) 
                return( FALSE );

	return( TRUE );
}

/* Count the number of args a CALL function needs.
 */
int
call_n_args( im_function *fn )
{
	int i;
	int nin;

        for( nin = 0, i = 0; i < fn->argc; i++ ) {
		im_type_desc *ty = fn->argv[i].desc;

		if( call_type_needs_input( ty ) ) 
                        nin += 1;
        }

	return( nin );
}

/* Make an im_doublevec_object.
 */
static int
call_make_doublevec( im_doublevec_object *dv, int n, double *vec )
{
	int i;

	dv->n = n;
	dv->vec = NULL;

	if( n > 0 ) {
		if( !(dv->vec = IARRAY( NULL, n, double )) )
			return( -1 );
		for( i = 0; i < n; i++ )
			dv->vec[i] = vec[i];
	}

	return( 0 );
}

/* Make an im_intvec_object. Make from a vec of doubles, because that's what
 * we get from nip.
 */
static int
call_make_intvec( im_intvec_object *dv, int n, double *vec )
{
	int i;

	dv->n = n;
	dv->vec = NULL;

	if( n > 0 ) {
		if( !(dv->vec = IARRAY( NULL, n, int )) )
			return( -1 );
		for( i = 0; i < n; i++ )
			dv->vec[i] = vec[i];
	}

	return( 0 );
}

/* Make an im_imagevec_object.
 */
static int
call_make_imagevec( im_imagevec_object *iv, int n )
{
	int i;

	iv->n = n;
	iv->vec = NULL;

	if( n > 0 ) {
		if( !(iv->vec = IARRAY( NULL, n, IMAGE * )) )
			return( -1 );
		for( i = 0; i < n; i++ )
			iv->vec[i] = NULL;
	}

	return( 0 );
}

/* Add another ii to inii.
 */
static gboolean
call_add_input_ii( CallInfo *vi, Imageinfo *ii )
{
	if( vi->ninii > MAX_CALL_ARGS ) {
		call_error_toomany( vi );
		return( FALSE );
	}

	vi->inii[vi->ninii] = ii;
	vi->ninii += 1;

	/* We hold a ref to the ii until the call is done and the result
	 * written back to nip2. If we cache the result, we make a new
	 * weakref.
	 */
	managed_dup_nonheap( MANAGED( ii ) );
	vi->must_drop = TRUE;

	return( TRUE );
}

/* ip types -> CALL types. Write to obj. FALSE for no conversion possible.
 */
static gboolean
call_fromip( CallInfo *vi, int i, PElement *arg )
{
	im_type_desc *ty = vi->fn->argv[i].desc;
	CallArgumentType vt = call_lookup_type( ty->type );
	im_object *obj = &vi->vargv[i];

	/* If call_lookup_type failed, is it the special DISPLAY type?
	 */
	if( vt == CALL_NONE && strcmp( ty->type, IM_TYPE_DISPLAY ) != 0 ) 
		/* Unknown type, and it's not DISPLAY. Flag an error.
		 */
		return( FALSE );

	switch( vt ) {
	case CALL_NONE:	/* IM_TYPE_DISPLAY */
		/* Just use IM_TYPE_sRGB.
		 */
		*obj = im_col_displays( 7 );

		break;

	case CALL_DOUBLE:
	{
		double *a = *obj;

		if( !PEISREAL( arg ) )
			return( FALSE );
		*a = PEGETREAL( arg );

		break;
	}

	case CALL_INT:
	{
		int *i = *obj;

		if( PEISREAL( arg ) ) {
			double t = PEGETREAL( arg );

			*i = (int) t;
		}
		else if( PEISBOOL( arg ) )
			*i = PEGETBOOL( arg );
		else
			return( FALSE );

		break;
	}

	case CALL_COMPLEX:
	{
		double *c = *obj;

		if( !PEISCOMPLEX( arg ) )
			return( FALSE );
		c[0] = PEGETREALPART( arg );
		c[1] = PEGETIMAGPART( arg );

		break;
	}

	case CALL_STRING:
	{
		char **c = (char **) obj;
		char buf[MAX_STRSIZE];

		if( !heap_get_string( arg, buf, MAX_STRSIZE ) )
			return( FALSE );
		*c = im_strdup( NULL, buf );

		break;
	}

	case CALL_IMAGE:
		/* Just note the Imageinfo for now ... a later pass sets vargv 
		 * once we've checked all the LUTs.
		 */
		if( !PEISIMAGE( arg ) ||
			!call_add_input_ii( vi, IMAGEINFO( PEGETII( arg ) ) ) )
			return( FALSE );

		break;

	case CALL_DOUBLEVEC:
	{
		double buf[MAX_VEC];
		int n;

		if( (n = heap_get_realvec( arg, buf, MAX_VEC )) < 0 ||
			call_make_doublevec( *obj, n, buf ) )
			return( FALSE );

		break;
	}

	case CALL_INTVEC:
	{
		double buf[MAX_VEC];
		int n;

		if( (n = heap_get_realvec( arg, buf, MAX_VEC )) < 0 ||
			call_make_intvec( *obj, n, buf ) )
			return( FALSE );

		break;
	}

	case CALL_IMAGEVEC:
	{
		Imageinfo *buf[MAX_VEC];
		int n;
		int i;

		/* Put Imageinfo in for now ... a later pass changes this to
		 * IMAGE* once we've checked all the LUTs.
		 */
		if( (n = heap_get_imagevec( arg, buf, MAX_VEC )) < 0 ||
			call_make_imagevec( *obj, n ) )
			return( FALSE );

		for( i = 0; i < n; i++ )
			if( !call_add_input_ii( vi, buf[i] ) )
				return( FALSE );

		break;
	}

	case CALL_DMASK:
	case CALL_IMASK:
	{
		im_mask_object **mo = (im_mask_object **) obj;

		if( vt == 6 ) {
			DOUBLEMASK *mask;

			if( !(mask = matrix_ip_to_dmask( arg )) )
				return( FALSE );
			(*mo)->mask = mask;
			(*mo)->name = im_strdupn( mask->filename );
		}
		else {
			INTMASK *mask;

			if( !(mask = matrix_ip_to_imask( arg )) )
				return( FALSE );
			(*mo)->mask = mask;
			(*mo)->name = im_strdupn( mask->filename );
		}

		break;
	}

	case CALL_GVALUE:
	{
		GValue *value = *obj;

		memset( value, 0, sizeof( GValue ) );
		if( !heap_ip_to_gvalue( arg, value ) )
			return( FALSE );

		break;
	}

	case CALL_INTERPOLATE:
		if( !PEISMANAGEDGOBJECT( arg ) )
			return( FALSE );
		*obj = PEGETMANAGEDGOBJECT( arg );

		break;

	default:
		g_assert( FALSE );
	}

	return( TRUE );
}

/* CALL types -> ip types. Write to arg. Use outiiindex to iterate through
 * outii[] as we find output imageinfo.
 */
static gboolean
call_toip( CallInfo *vi, int i, int *outiiindex, PElement *arg )
{
	im_object obj = vi->vargv[i];
	im_type_desc *ty = vi->fn->argv[i].desc;

#ifdef DEBUG
	printf( "call_toip: arg[%d] (%s) = ", i, ty->type );
#endif /*DEBUG*/

	switch( call_lookup_type( ty->type ) ) {
	case CALL_DOUBLE:
		if( !heap_real_new( vi->rc->heap, *((double*)obj), arg ) )
			return( FALSE );

		break;

	case CALL_INT:
		if( !heap_real_new( vi->rc->heap, *((int*)obj), arg ) )
			return( FALSE );

		break;

	case CALL_DOUBLEVEC:
	{
		im_doublevec_object *dv = obj;

		if( !heap_realvec_new( vi->rc->heap, dv->n, dv->vec, arg ) )
			return( FALSE );

		break;
	}

	case CALL_INTVEC:
	{
		im_intvec_object *iv = obj;

		if( !heap_intvec_new( vi->rc->heap, iv->n, iv->vec, arg ) )
			return( FALSE );

		break;
	}

	case CALL_COMPLEX:
		if( !heap_complex_new( vi->rc->heap, 
			((double*)obj)[0], ((double*)obj)[1], arg ) )
			return( FALSE );

		break;

	case CALL_STRING:
		if( !heap_managedstring_new( vi->rc->heap, (char *) obj, arg ) )
			return( FALSE );

		break;

	case CALL_IMAGE:
	{
		Imageinfo *outii;

		outii = vi->outii[*outiiindex];
		*outiiindex += 1;

		PEPUTP( arg, ELEMENT_MANAGED, outii );

		break;
	}

	case CALL_DMASK:
	{
		im_mask_object *mo = obj;
		DOUBLEMASK *mask = mo->mask;

		if( !matrix_dmask_to_heap( vi->rc->heap, mask, arg ) )
			return( FALSE );

		break;
	}

	case CALL_IMASK:
	{
		im_mask_object *mo = obj;
		INTMASK *mask = mo->mask;

		if( !matrix_imask_to_heap( vi->rc->heap, mask, arg ) )
			return( FALSE );

		break;
	}

	case CALL_GVALUE:
		if( !heap_gvalue_to_ip( (GValue *) obj, arg ) )
			return( FALSE );

		break;

	case CALL_IMAGEVEC:
	case CALL_INTERPOLATE:
	default:
		g_assert( FALSE );
	}

#ifdef DEBUG
	pgraph( arg );
#endif /*DEBUG*/

	return( TRUE );
}

static void *
call_write_result_sub( Reduce *rc, PElement *safe, CallInfo *vi, PElement *out )
{
	int outiiindex;

	/* call_toip() uses this to iterate through outii[].
	 */
	outiiindex = 0;

	/* Write result.
	 */
	if( vi->nres == 1 ) {
		/* Single result.
		 */
		if( !call_toip( vi, vi->outpos[0], &outiiindex, safe ) )
			return( out );
	}
	else {
		/* Have to build a list of results.
		 */
		PElement list;
		PElement t;
		int i;

		list = *safe;
		heap_list_init( &list );
		for( i = 0; i < vi->nres; i++ ) {
			if( !heap_list_add( vi->rc->heap, &list, &t ) ||
				!call_toip( vi, 
					vi->outpos[i], &outiiindex, &t ) )
				return( out );

			(void) heap_list_next( &list );
		}
	}

	/* Now overwrite out with safe.
	 */
	PEPUTPE( out, safe );

	return( NULL );
}

/* Write the results back to the heap. We have to so this in two stages:
 * build the output object linked off a new managed Element, then once it's
 * built, overwrite our output
 */
static gboolean
call_write_result( CallInfo *vi, PElement *out )
{
	if( reduce_safe_pointer( vi->rc, 
		(reduce_safe_pointer_fn) call_write_result_sub, 
		vi, out, NULL, NULL ) )
		return( FALSE );

	return( TRUE );
}

/* Junk all the refs we were holding during the call. See call_add_input_ii() 
 * and call_add_output_ii().
 *
 * This gets called explicitly after we have handed the ii refs back to nip2
 * during normal processing, or from _dispose() if we bomb out early and
 * unref.
 */
static void
call_drop_refs( CallInfo *vi )
{
	if( vi->must_drop ) {
		int i;

#ifdef DEBUG
		printf( "call_drop_refs: dropping %d in refs\n", vi->ninii );
		printf( "call_drop_refs: dropping %d out refs\n", vi->noutii );
#endif /*DEBUG*/

		for( i = 0; i < vi->ninii; i++ )
			managed_destroy_nonheap( MANAGED( vi->inii[i] ) );
		for( i = 0; i < vi->noutii; i++ )
			managed_destroy_nonheap( MANAGED( vi->outii[i] ) );

		vi->must_drop = FALSE;
	}
}

static void
call_info_dispose( GObject *gobject )
{
	CallInfo *vi;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_CALL_INFO( gobject ) );

	vi = CALL_INFO( gobject );

#ifdef DEBUG
	printf( "call_info_dispose: (%p) %s \"%s\"\n",
		vi, G_OBJECT_TYPE_NAME( vi ), vi->name );
#endif /*DEBUG*/

	/* Are we in the history? Remove us.
	 */
	cache_history_remove( vi ); 

	/* Drop any refs we may have left dangling.
	 */
	call_drop_refs( vi );

	G_OBJECT_CLASS( call_info_parent_class )->dispose( gobject );
}

/* Junk stuff we may have attached to vargv.
 */
static void
call_vargv_free( im_function *fn, im_object *vargv )
{
	int i;

	/* Free any CALL args we built and haven't used.
	 */
	for( i = 0; i < fn->argc; i++ ) {
		im_type_desc *ty = fn->argv[i].desc;
		im_object *obj = vargv[i];
		CallArgumentType vt;

		/* Make sure we don't damage any error message we might
		 * have.
		 */
		error_block();
		vt = call_lookup_type( ty->type );
		error_unblock();

		switch( vt ) {
		case CALL_NONE:		/* IM_TYPE_DISPLAY */
		case CALL_DOUBLE:
		case CALL_INT: 	
		case CALL_COMPLEX: 
		case CALL_GVALUE:
		case CALL_INTERPOLATE:
		case CALL_IMAGE:
			/* Do nothing.
			 */
			break;

		case CALL_STRING:
			IM_FREE( obj );
			break;

		case CALL_IMAGEVEC: 	
			IM_FREE( ((im_imagevec_object *) obj)->vec );
			break;

		case CALL_DOUBLEVEC:
			IM_FREE( ((im_doublevec_object *) obj)->vec );
			break;

		case CALL_INTVEC:
			IM_FREE( ((im_intvec_object *) obj)->vec );
			break;

		case CALL_DMASK:
			IM_FREE( ((im_mask_object *) obj)->name );
			IM_FREEF( im_free_dmask, 
				((im_mask_object *) obj)->mask );
			break;

		case CALL_IMASK:
			IM_FREE( ((im_mask_object *) obj)->name );
			IM_FREEF( im_free_imask, 
				((im_mask_object *) obj)->mask );
			break;

		default:
			g_assert( FALSE );
		}
	}
}

static void
call_info_finalize( GObject *gobject )
{
	CallInfo *vi;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_CALL_INFO( gobject ) );

	vi = CALL_INFO( gobject );

#ifdef DEBUG_LEAK
	call_info_all = g_slist_remove( call_info_all, vi );
#endif /*DEBUG_LEAK*/

	if( vi->vargv ) {
		call_vargv_free( vi->fn, vi->vargv );
		im_free_vargv( vi->fn, vi->vargv );
		IM_FREE( vi->vargv );
	}

	G_OBJECT_CLASS( call_info_parent_class )->finalize( gobject );
}

static void
call_info_info( iObject *iobject, VipsBuf *buf )
{
	CallInfo *vi = CALL_INFO( iobject );

	vips_buf_appendf( buf, "call_info_info: (%p) %s \"%s\"\n",
		vi, G_OBJECT_TYPE_NAME( vi ), NN( IOBJECT( vi )->name ) );
}

static void
call_info_class_init( CallInfoClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );

	gobject_class->dispose = call_info_dispose;
	gobject_class->finalize = call_info_finalize;

	iobject_class->info = call_info_info;
}

static void
call_info_init( CallInfo *vi )
{
	int i;

	vi->name = NULL;
	vi->fn = NULL;
	vi->rc = NULL;
	vi->vargv = NULL;
	vi->nargs = 0;
	vi->nres = 0;
	vi->nires = 0;
	vi->ninii = 0;
	vi->noutii = 0;
	vi->use_lut = FALSE;		/* Set this properly later */
	vi->found_hash = FALSE;
	vi->in_cache = FALSE;
	vi->must_drop = FALSE;

#ifdef DEBUG_LEAK
	call_info_all = g_slist_prepend( call_info_all, vi );
#endif /*DEBUG_LEAK*/

	for( i = 0; i < MAX_CALL_ARGS; i++ ) {
		vi->outii_destroy_sid[i] = 0;
		vi->inii_destroy_sid[i] = 0;
		vi->inii_invalidate_sid[i] = 0;
	}
}

static CallInfo *
call_new( Reduce *rc, im_function *fn )
{
	CallInfo *vi;
	int i;

	g_assert( fn->argc < MAX_CALL_ARGS - 1 );

	if( !fn || 
		!(vi = CALL_INFO( g_object_new( TYPE_CALL_INFO, NULL ) )) )
		return( NULL );
	vi->name = fn->name;
	vi->fn = fn;
	vi->rc = rc;

	/* Look over the args ... count the number of inputs we need, and 
	 * the number of outputs we generate. Note the position of each.
	 */
	for( i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;

		if( call_type_makes_output( ty ) ) {
			vi->outpos[vi->nres] = i;
			vi->nres += 1; 

			/* Image output.
			 */
			if( strcmp( ty->type, IM_TYPE_IMAGE ) == 0 ) 
				vi->nires += 1;
		}

		if( call_type_needs_input( ty ) ) {
			vi->inpos[vi->nargs] = i;
			vi->nargs += 1; 
		}
	}

	/* Make the call spine, alloc memory. 
	 */
	if( !(vi->vargv = IM_ARRAY( NULL, vi->fn->argc + 1, im_object )) ||
		im_allocate_vargv( vi->fn, vi->vargv ) ) {
		call_error( vi );
		g_object_unref( vi );
		return( NULL );
	}

	return( vi );
}

/* Add another ii to outii.
 */
static gboolean
call_add_output_ii( CallInfo *vi, Imageinfo *ii )
{
	if( vi->noutii > MAX_CALL_ARGS ) {
		call_error_toomany( vi );
		return( FALSE );
	}

	vi->outii[vi->noutii] = ii;
	vi->noutii += 1;

	/* We hold a ref to the ii until the call is done and the result
	 * written back to nip2. If we cache the result, we make a new
	 * weakref.
	 */
	managed_dup_nonheap( MANAGED( ii ) );
	vi->must_drop = TRUE;

	return( TRUE );
}

/* Init an output slot in vargv.
 */
static gboolean
call_build_output( CallInfo *vi, int i )
{
	im_type_desc *ty = vi->fn->argv[i].desc;

	/* Provide output objects for the function to write to.
	 */
	switch( call_lookup_type( ty->type ) ) {
	case CALL_DOUBLE:
	case CALL_INT:
	case CALL_COMPLEX:
	case CALL_STRING:
		break;

	case CALL_IMAGE:
{
		Imageinfo *ii;

		if( !(ii = imageinfo_new_temp( main_imageinfogroup, 
			vi->rc->heap, NULL, "p" )) ||
			!call_add_output_ii( vi, ii ) ||
			!(vi->vargv[i] = imageinfo_get( FALSE, ii )) )
			return( FALSE );

		break;
}

	case CALL_DMASK:
	case CALL_IMASK:
	{
		im_mask_object *mo = vi->vargv[i];

		mo->mask = NULL;
		mo->name = im_strdup( NULL, "" );

		break;
	}

	case CALL_GVALUE:
	{
		GValue *value = vi->vargv[i];

		memset( value, 0, sizeof( GValue ) );

		break;
	}

	case CALL_DOUBLEVEC:
	case CALL_INTVEC:
	{
		/* intvec is also int + pointer.
		 */
		im_doublevec_object *dv = vi->vargv[i];

		dv->n = 0;
		dv->vec = NULL;

		break;
	}

	default:
		g_assert( FALSE );
	}

	return( TRUE );
}

static gboolean
call_build_inputva( CallInfo *vi, int i, va_list *ap )
{
	im_type_desc *ty = vi->fn->argv[i].desc;

	switch( call_lookup_type( ty->type ) ) {
	case CALL_DOUBLE:
	{
		double v = va_arg( *ap, double );

#ifdef DEBUG
		printf( "%g\n", v );
#endif /*DEBUG*/

		*((double*)vi->vargv[i]) = v;

		if( trace_flags & TRACE_VIPS ) 
			vips_buf_appendf( trace_current(), "%g ", v );

		break;
	}

	case CALL_INT:
	{
		int v = va_arg( *ap, int );

#ifdef DEBUG
		printf( "%d\n", v );
#endif /*DEBUG*/

		*((int*)vi->vargv[i]) = v;

		if( trace_flags & TRACE_VIPS ) 
			vips_buf_appendf( trace_current(), "%d ", v );

		break;
	}

	case CALL_GVALUE:
	{
		GValue *value = va_arg( *ap, GValue * );

#ifdef DEBUG
		printf( "gvalue %p\n", value );
#endif /*DEBUG*/

		vi->vargv[i] = value;

		if( trace_flags & TRACE_VIPS ) {
			vips_buf_appendgv( trace_current(), value );
			vips_buf_appends( trace_current(), " " );
		}

		break;
	}

	case CALL_INTERPOLATE:
	{
		VipsInterpolate *value = 
			va_arg( *ap, VipsInterpolate * );

#ifdef DEBUG
		printf( "interpolate %p\n", value );
#endif /*DEBUG*/

		vi->vargv[i] = value;

		if( trace_flags & TRACE_VIPS ) {
			vips_object_to_string( VIPS_OBJECT( value ), 
				trace_current() );
			vips_buf_appends( trace_current(), " " );
		}

		break;
	}

	case CALL_IMAGE:
	{
		Imageinfo *ii = va_arg( *ap, Imageinfo * );

#ifdef DEBUG
		printf( "imageinfo %p\n", ii );
#endif /*DEBUG*/

		if( !call_add_input_ii( vi, ii ) )
			return( FALSE );

		/* Filled in later.
		 */
		vi->vargv[i] = NULL;

		if( trace_flags & TRACE_VIPS ) {
			VipsBuf *buf = trace_current();

			if( ii && ii->im ) {
				vips_buf_appends( buf, "<" );
				vips_buf_appendf( buf, 
					_( "image \"%s\"" ),
					ii->im->filename ); 
				vips_buf_appends( buf, "> " );
			}
			else {
				vips_buf_appends( buf, "<" );
				vips_buf_appends( buf, 
					_( "no image" ) );
				vips_buf_appends( buf, "> " );
			}
		}

		break;
	}

	case CALL_DOUBLEVEC:
	{
		int n = va_arg( *ap, int );
		double *vec = va_arg( *ap, double * );

#ifdef DEBUG
{
		int i;

		for( i = 0; i < n; i++ )
			printf( "%g, ", vec[i] );
		printf( "\n" );
}
#endif /*DEBUG*/

		if( call_make_doublevec( vi->vargv[i], n, vec ) )
			return( FALSE );

		if( trace_flags & TRACE_VIPS ) {
			VipsBuf *buf = trace_current();
			int i;

			vips_buf_appendf( buf, "<" );
			vips_buf_appendf( buf, _( "doublevec" ) );
			for( i = 0; i < n; i++ )
				vips_buf_appendf( buf, " %g", vec[i] );
			vips_buf_appends( buf, "> " );
		}

		break;
	}

	/* 

		FIXME ... add intvec perhaps

	 */

	case CALL_IMAGEVEC:
	{
		int n = va_arg( *ap, int );
		Imageinfo **vec = va_arg( *ap, Imageinfo ** );

#ifdef DEBUG
{
		int i;

		for( i = 0; i < n; i++ )
			printf( "%p, ", vec[i] );
		printf( "\n" );
}
#endif /*DEBUG*/

		if( call_make_imagevec( vi->vargv[i], n ) )
			return( FALSE );

		for( i = 0; i < n; i++ )
			if( !call_add_input_ii( vi, vec[i] ) )
				return( FALSE );

		if( trace_flags & TRACE_VIPS ) {
			VipsBuf *buf = trace_current();
			int i;

			vips_buf_appendf( buf, "<" );
			vips_buf_appendf( buf, _( "imagevec" ) );
			for( i = 0; i < n; i++ ) {
				vips_buf_appendf( buf, " <" );
				vips_buf_appendf( buf, 
					_( "image \"%s\"" ),
					vec[i]->im->filename );
				vips_buf_appendf( buf, ">" );
			}
			vips_buf_appends( buf, "> " );
		}

		break;
	}

	default:
		g_assert( FALSE );
	}

	return( TRUE );
}

/* Fill an argument vector from the C stack.
 */
static gboolean
call_fillva( CallInfo *vi, va_list *ap )
{
	int i;

	g_assert( vi->ninii == 0 );
	g_assert( vi->noutii == 0 );

	for( i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;

#ifdef DEBUG
		printf( "call_fillva: arg[%d] (%s) = ", i, ty->type );
#endif /*DEBUG*/

		if( call_type_makes_output( ty ) ) {
			if( !call_build_output( vi, i ) )
				return( FALSE );
#ifdef DEBUG
			printf( " output\n" );
#endif /*DEBUG*/
		}

		if( strcmp( ty->type, IM_TYPE_DISPLAY ) == 0 ) {
			/* DISPLAY argument ... just IM_TYPE_sRGB.
			 */
			vi->vargv[i] = im_col_displays( 7 );

#ifdef DEBUG
			printf( " display\n" );
#endif /*DEBUG*/
		}

		if( call_type_needs_input( ty ) ) {
			if( !call_build_inputva( vi, i, ap ) )
				return( FALSE );
		}
	}

	/* Every output ii depends upon all of the input ii.
	 */
	for( i = 0; i < vi->noutii; i++ ) 
		managed_sub_add_all( MANAGED( vi->outii[i] ), 
			vi->ninii, (Managed **) vi->inii );

#ifdef DEBUG
	printf( "call_fill_spine: reffed %d in\n", vi->ninii );
	printf( "call_fill_spine: created %d out\n", vi->noutii );
#endif /*DEBUG*/

	return( TRUE );
}

static gboolean
callva_sub( Reduce *rc, const char *name, PElement *out, va_list *ap )
{
	CallInfo *vi;
	gboolean result;

	if( trace_flags & TRACE_VIPS ) 
		trace_push();

	if( !(vi = call_new( rc, im_find_function( name ) )) )
		return( FALSE );

	if( trace_flags & TRACE_VIPS ) 
		vips_buf_appendf( trace_current(), "\"%s\" ", vi->name );

	result = TRUE;

	if( !call_fillva( vi, ap ) )
		result = FALSE;

	if( trace_flags & TRACE_VIPS ) 
		vips_buf_appends( trace_current(), " ->\n" ); 

	if( result && (
		!(vi = cache_dispatch( vi, out )) ||
		!call_write_result( vi, out ) ) )  
		result = FALSE;

	if( trace_flags & TRACE_VIPS ) {
		trace_result( TRACE_VIPS, out );
		trace_pop();
	}

	if( vi ) {
		/* We must drop refs explicitly, since this unref might not
		 * dispose the vi.
		 */
		call_drop_refs( vi );

		g_object_unref( vi );
	}

	return( result );
}

/* Call a CALL function picking up args from the function call.
 */
void
callva( Reduce *rc, PElement *out, const char *name, ... )
{
	va_list ap;
	gboolean result;

#ifdef DEBUG
	printf( "** callva: starting for %s\n", name );
#endif /*DEBUG*/

        va_start( ap, name );
	result = callva_sub( rc, name, out, &ap );
        va_end( ap );

#ifdef DEBUG
	printf( "callva: done\n" );
#endif /*DEBUG*/

	if( !result )
		reduce_throw( rc );
}

/* Fill an argument vector from our stack frame. Number of args already
 * checked. 
 */
static gboolean
call_fill_spine( CallInfo *vi, HeapNode **arg )
{
	int i, j;

	g_assert( vi->ninii == 0 );
	g_assert( vi->noutii == 0 );

	/* Fully reduce all arguments. Once we've done this, we can be sure
	 * there will not be a GC while we gather, and therefore that no
	 * pointers will become invalid during this call.
	 */
	for( i = 0; i < vi->nargs; i++ ) {
		PElement rhs;

		PEPOINTRIGHT( arg[i], &rhs );
		if( !heap_reduce_strict( &rhs ) )
			return( FALSE );
	}

	for( j = 0, i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;

		if( call_type_makes_output( ty ) ) 
			if( !call_build_output( vi, i ) )
				return( FALSE );

		if( strcmp( ty->type, IM_TYPE_DISPLAY ) == 0 ) {
			/* Special DISPLAY argument - don't fetch another ip
			 * argument for it.
			 */
			(void) call_fromip( vi, i, NULL );
		}

		if( call_type_needs_input( ty ) ) {
			PElement rhs;

			/* Convert ip type to CALL type.
			 */
			PEPOINTRIGHT( arg[vi->nargs - j - 1], &rhs );
			if( !call_fromip( vi, i, &rhs ) ) {
				call_error_arg( vi, arg, j );
				return( FALSE );
			}

			j += 1;
		}
	}

	/* Every output ii depends upon all of the input ii.
	 */
	for( i = 0; i < vi->noutii; i++ ) 
		managed_sub_add_all( MANAGED( vi->outii[i] ), 
			vi->ninii, (Managed **) vi->inii );

#ifdef DEBUG
	printf( "call_fill_spine: reffed %d inii\n", vi->ninii );
	printf( "call_fill_spine: created %d outii\n", vi->noutii );
#endif /*DEBUG*/

	return( TRUE );
}

static gboolean
call_spine_sub( Reduce *rc, const char *name, im_function *fn,
	PElement *out, HeapNode **arg )
{
	CallInfo *vi;
	gboolean result;

#ifdef DEBUG
	printf( "** call_spine: starting for %s\n", name );
#endif /*DEBUG*/

	if( !(vi = call_new( rc, fn )) )
		return( FALSE );

	if( trace_flags & TRACE_VIPS ) {
		VipsBuf *buf = trace_push();

		vips_buf_appendf( buf, "\"%s\" ", name );
		trace_args( arg, vi->nargs );
	}

	result = TRUE;

	if( !call_fill_spine( vi, arg ) ||
		!(vi = cache_dispatch( vi, out )) ||
		!call_write_result( vi, out ) )
		result = FALSE;

	if( trace_flags & TRACE_VIPS ) {
		trace_result( TRACE_VIPS, out );
		trace_pop();
	}

	if( vi ) {
		/* We must drop refs explicitly, since this unref might not
		 * dispose the vi.
		 */
		call_drop_refs( vi );

		g_object_unref( vi );
	}

#ifdef DEBUG
	printf( "call_spine: done\n" );
#endif /*DEBUG*/

	return( result );
}

/* Call a CALL function, pick up args from the graph. 
 */
void
call_spine( Reduce *rc, const char *name, HeapNode **arg, PElement *out )
{
	if( !call_spine_sub( rc, name, im_find_function( name ), out, arg ) )
		reduce_throw( rc );
}

/* As an ActionFn.
 */
void
call_run( Reduce *rc, Compile *compile,
	int op, const char *name, HeapNode **arg, PElement *out,
	im_function *function )
{
	if( !call_spine_sub( rc, name, function, out, arg ) )
		reduce_throw( rc );
}
