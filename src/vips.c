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

/* Maxiumum number of args to a VIPS function.
 */
#define MAX_VIPS_ARGS (100)

/* Stuff we hold about a call to a VIPS function.
 */
typedef struct _VipsInfo {
	/* Environment.
	 */
	const char *name;
	im_function *fn;		/* Function we call */
	Reduce *rc;			/* RC we run inside */

	/* Args we build. Images in vargv are IMAGE* pointers.
	 */
	VipsCall *call;			/* Call handle */
	int nargs;			/* Number of args needed from ip */
	int nres;			/* Number of objects we write back */
	int nires;			/* Number of images we write back */
	int inpos[MAX_VIPS_ARGS];	/* Positions of inputs */
	int outpos[MAX_VIPS_ARGS];	/* Positions of outputs */

	/* Input images. Need to track "destroy" on each one (and kill us
	 * in turn). 
	 */
	int ninii;			
	Imageinfo *inii[MAX_VIPS_ARGS];	
	unsigned int inii_destroy_sid[MAX_VIPS_ARGS];
	unsigned int inii_paint_sid[MAX_VIPS_ARGS];

	/* Output images. 
	 */
	int noutii;
	Imageinfo *outii[MAX_VIPS_ARGS];
	unsigned int outii_destroy_sid[MAX_VIPS_ARGS];

	gboolean use_lut;		/* TRUE for using a lut */
} VipsInfo;

/* VIPS argument types we support. Keep order in sync with VipsArgumentType.
 */
static im_arg_type vips_supported[] = {
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
	IM_TYPE_GVALUE
};

#ifdef DEBUG_LEAK
/* All the VipsInfo we make ... for leak testing. Build this file with 
 * DEBUG_LEAK to enable add/remove to this list.
 */
static GSList *vips_info_all = NULL;
#endif /*DEBUG_LEAK*/

void
vips_check_all_destroyed( void )
{
#ifdef DEBUG_LEAK
	if( vips_info_all != NULL ) {
		GSList *p;

		printf( "** %d VipsInfo leaked!\n", 
			g_slist_length( vips_info_all ) );
		printf( "(operations with no image args are not leaks)\n" );

		for( p = vips_info_all; p; p = p->next ) {
			VipsInfo *vi = (VipsInfo *) p->data;

			printf( "\t%s\n", vi->name );
		}
	}
#endif /*DEBUG_LEAK*/
}

/* Error early on .. we can't print args yet.
 */
static void
vips_error( VipsInfo *vi )
{
	error_top( _( "VIPS library error." ) );
	error_sub( _( "Error calling library function \"%s\" (%s)." ), 
		vi->name, vi->fn->desc );
}

/* Look up a VIPS type. 
 */
VipsArgumentType
vips_type_find( im_arg_type type )
{
	int i;

	for( i = 0; i < IM_NUMBER( vips_supported ); i++ )
		if( strcmp( type, vips_supported[i] ) == 0 )
			return( (VipsArgumentType) i );

	error_top( _( "Unknown type." ) );
	error_sub( _( "VIPS type \"%s\" not supported" ), type );

	return( VIPS_NONE );
}

static void
vips_destroy( VipsInfo *vi )
{
	int i;

#ifdef DEBUG_HISTORY
	printf( "vips_destroy: destroying \"%s\" (%p)\n", vi->name, vi );
#endif /*DEBUG_HISTORY*/

	/* Are we in the history? Remove us.
	 */
	vips_history_remove( vi ); 
#ifdef DEBUG_LEAK
	vips_info_all = g_slist_remove( vips_info_all, vi );
#endif /*DEBUG_LEAK*/

	/* Break "destroy" links to iis.
	 */
	for( i = 0; i < vi->ninii; i++ ) 
		FREESID( vi->inii_destroy_sid[i], vi->inii[i] ); 
	for( i = 0; i < vi->noutii; i++ ) 
		FREESID( vi->outii_destroy_sid[i], vi->outii[i] ); 

	/* Break paint links.
	 */
	for( i = 0; i < vi->ninii; i++ ) 
		FREESID( vi->inii_paint_sid[i], vi->inii[i] ); 

	/* Free any VIPS args we built and haven't used.
	 */
	for( i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;
		im_object *obj = vi->vargv[i];
		VipsArgumentType vt;

		/* Make sure we don't damage any error magessage we might
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

	if( vi->vargv ) {
		im_free_vargv( vi->fn, vi->vargv );
		IM_FREE( vi->vargv );
	}
	IM_FREE( vi );
}

/* VIPS types -> a buffer. For tracing calls.
 */
static void
vips_tochar( VipsInfo *vi, int i, BufInfo *buf )
{
	im_object obj = vi->vargv[i];
	im_type_desc *vips = vi->fn->argv[i].desc;
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
		buf_appendf( buf, "\"%s\"", (char*) obj );
		break;

	case VIPS_IMAGE:
		buf_appendi( buf, (IMAGE *) obj );
		break;

	case VIPS_DMASK:
		buf_appendf( buf, "dmask" );
		break;

	case VIPS_IMASK:
		buf_appendf( buf, "imask" );
		break;

	case VIPS_DOUBLEVEC:
		buf_appendf( buf, "doublevec" );
		break;

	case VIPS_INTVEC:
		buf_appendf( buf, "intvec" );
		break;

	case VIPS_IMAGEVEC:
		buf_appendf( buf, "imagevec" );
		break;

	case VIPS_GVALUE:
	{
		GValue *value = (GValue *) obj;

		buf_appends( buf, "(gvalue" );
		buf_appendgv( buf, value );
		buf_appendf( buf, ")" );

		break;
	}

	default:
		g_assert( FALSE );
	}
}

/* Get the args from the heap.
 */
static void
vips_args_heap( VipsInfo *vi, HeapNode **arg, BufInfo *buf )
{
	int i;

	buf_appendf( buf, _( "You passed:" ) );
	buf_appendf( buf, "\n" );
	for( i = 0; i < vi->nargs; i++ ) {
		im_arg_desc *varg = &vi->fn->argv[vi->inpos[i]];
		PElement rhs;

		PEPOINTRIGHT( arg[vi->nargs - i - 1], &rhs );
		buf_appendf( buf, "  %s - ", varg->name );
		itext_value_ev( vi->rc, buf, &rhs );
		buf_appendf( buf, "\n" );
	}
}

/* Get the args from the VIPS call buffer.
 */
static void
vips_args_vips( VipsInfo *vi, BufInfo *buf )
{
	int i;

	buf_appendf( buf, _( "You passed:" ) );
	buf_appendf( buf, "\n" );
        for( i = 0; i < vi->fn->argc; i++ ) {
                im_type_desc *type = vi->fn->argv[i].desc;
                char *name = vi->fn->argv[i].name;

                if( !(type->flags & IM_TYPE_OUTPUT)  &&
                        strcmp( type->type, IM_TYPE_DISPLAY ) != 0 ) {
                        buf_appendf( buf, "   %s - ", name );
                        vips_tochar( vi, i, buf );
                        buf_appendf( buf, "\n" );
                }
        }
}

/* Make a usage error for a VIPS function.
 */
void
vips_usage( BufInfo *buf, im_function *fn )
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

		if( strcmp( arg->desc->type, IM_TYPE_DISPLAY ) == 0 ) {
			/* Special secret display argument. 
			 */
		}
		else if( arg->desc->flags & IM_TYPE_OUTPUT ) {
			strcat( output, line );
			nout++;
		}
		else {
			strcat( input, line );
			nin++;
		}
	}

	buf_appendf( buf, _( "Usage:" ) );
        buf_appends( buf, "\n" );
        buf_appendf( buf, _( "VIPS operator \"%s\"" ), fn->name );
        buf_appends( buf, "\n" );
        buf_appendf( buf, _( "%s, from package \"%s\"" ), 
		fn->desc, pack->name );
        buf_appends( buf, "\n" );

	buf_appendf( buf, 
		ngettext( "\"%s\" takes %d argument:",
			"\"%s\" takes %d arguments:",
			nin ),
		fn->name, nin );
        buf_appendf( buf, "\n%s", input );

	buf_appendf( buf, 
		ngettext( "And produces %d result:",
			"And produces %d results:",
			nout ),
		nout );
	buf_appendf( buf, "\n%s", output );

        /* Print any flags this function has.
         */
        buf_appendf( buf, _( "Flags:" ) );
        buf_appends( buf, "\n" );
	buf_appendf( buf, "   (" );
        if( fn->flags & IM_FN_PIO )
                buf_appendf( buf, _( "PIO function" ) );
        else
                buf_appendf( buf, _( "WIO function" ) );
	buf_appendf( buf, ") (" );
        if( fn->flags & IM_FN_TRANSFORM ) 
                buf_appendf( buf, _( "coordinate transformer" ) );
        else
                buf_appendf( buf, _( "no coordinate transformation" ) );
	buf_appendf( buf, ") (" );
        if( fn->flags & IM_FN_PTOP )
                buf_appendf( buf, _( "point-to-point operation" ) );
        else
                buf_appendf( buf, _( "area operation" ) );
	buf_appendf( buf, ") (" );
        if( fn->flags & IM_FN_NOCACHE )
                buf_appendf( buf, _( "uncacheable operation" ) );
        else
                buf_appendf( buf, _( "operation can be cached" ) );
        buf_appendf( buf, ")\n" );
}

/* We know there's a problem exporting a particular arg to VIPS.
 */
static void
vips_error_arg( VipsInfo *vi, HeapNode **arg, int argi )
{
	BufInfo buf;
	char txt[1000];

	error_top( _( "Bad argument." ) );

	buf_init_static( &buf, txt, 1000 );
	buf_appendf( &buf,
		_( "Argument %d (%s) to \"%s\" is the wrong type." ),
		argi + 1, vi->fn->argv[argi].name, vi->name );
	buf_appendf( &buf, "\n" );
	vips_args_heap( vi, arg, &buf );
	buf_appendf( &buf, "\n" );
	vips_usage( &buf, vi->fn );
	error_sub( "%s", buf_all( &buf ) );
}

/* There's a problem calling the function. Show args from the vips call
 * struct.
 */
static void
vips_error_fn_vips( VipsInfo *vi )
{
	BufInfo buf;
	char txt[1000];

	error_top( _( "VIPS library error." ) );

	buf_init_static( &buf, txt, 1000 );
	buf_appendf( &buf, _( "Error calling library function \"%s\" (%s)." ),
		vi->name, vi->fn->desc );
	buf_appendf( &buf, "\n" );
	buf_appendf( &buf, _( "VIPS library: %s" ),
		im_errorstring() );
	buf_appendf( &buf, "\n" );
	vips_args_vips( vi, &buf );
	buf_appendf( &buf, "\n" );
	vips_usage( &buf, vi->fn );
	error_sub( "%s", buf_all( &buf ) );
}

static VipsInfo *
vips_new( Reduce *rc, im_function *fn )
{
	VipsInfo *vi;
	int i;

	g_assert( fn->argc < MAX_VIPS_ARGS - 1 );

	if( !fn || !(vi = INEW( NULL, VipsInfo )) )
		return( NULL );
	vi->name = fn->name;
	vi->fn = fn;
	vi->rc = rc;
	vi->vargv = NULL;
	vi->nargs = 0;
	vi->nres = 0;
	vi->nires = 0;
	vi->ninii = 0;
	vi->noutii = 0;
	vi->use_lut = FALSE;		/* Set this properly later */
	vi->found_hash = FALSE;
	vi->in_cache = FALSE;
#ifdef DEBUG_LEAK
	vips_info_all = g_slist_prepend( vips_info_all, vi );
#endif /*DEBUG_LEAK*/

	/* Look over the args ... count the number of inputs we need, and 
	 * the number of outputs we generate. Note the position of each.
	 */
	for( i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;

		if( ty->flags & IM_TYPE_OUTPUT ) {
			/* Found an output object.
			 */
			vi->outpos[vi->nres] = i;
			vi->nres += 1; 

			/* Image output.
			 */
			if( strcmp( ty->type, IM_TYPE_IMAGE ) == 0 ) 
				vi->nires += 1;
		}
		else if( strcmp( ty->type, IM_TYPE_DISPLAY ) != 0 ) {
			/* Found an input object.
			 */
			vi->inpos[vi->nargs] = i;
			vi->nargs += 1; 
		}
	}

	/* Need to zero all the signals ... we don't always have signals for
	 * vi that aren't in the history yet. Can't just loop to argc, since
	 * some args may be IMAGEVEC.
	 */
	for( i = 0; i < MAX_VIPS_ARGS; i++ ) {
		vi->outii_destroy_sid[i] = 0;
		vi->inii_destroy_sid[i] = 0;
		vi->inii_paint_sid[i] = 0;
	}

	/* Make the call spine, alloc memory. 
	 */
	if( !(vi->vargv = IM_ARRAY( NULL, vi->fn->argc + 1, im_object )) ||
		im_allocate_vargv( vi->fn, vi->vargv ) ) {
		vips_error( vi );
		vips_destroy( vi );
		return( NULL );
	}

	return( vi );
}

/* Is this the sort of VIPS funtion we can call?
 */
gboolean
vips_is_callable( im_function *fn )
{
	int i;
	int nout;
	int nin;

	if( fn->argc >= MAX_VIPS_ARGS )
		return( FALSE );

        /* Check all argument types are supported. As well as the arg types
         * spotted by vips_type_find, we also allow IM_TYPE_DISPLAY.
         */
        for( i = 0; i < fn->argc; i++ ) {
                im_arg_desc *arg = &fn->argv[i];
                im_arg_type vt = arg->desc->type;
                VipsArgumentType t = vips_type_find( vt );

                if( t == VIPS_NONE ) {
                        /* Unknown type .. if DISPLAY it's OK.
                         */
                        if( strcmp( vt, IM_TYPE_DISPLAY ) != 0 )
                                return( FALSE );
                }
        }

        /* Must be at least one output argument.
         */
        nin = nout = 0;
        for( i = 0; i < fn->argc; i++ )
                if( fn->argv[i].desc->flags & IM_TYPE_OUTPUT )
                        nout++;
		else
                        nin++;
        if( nout == 0 ) 
                return( FALSE );

	/* Need at least 1 input argument: we reply on having an application
	 * node to overwrite with (I result).
	 */
        if( nin == 0 ) 
                return( FALSE );

	return( TRUE );
}

/* Count the number of args a VIPS function needs.
 */
int
vips_n_args( im_function *fn )
{
	int i;
	int nin;

        for( nin = 0, i = 0; i < fn->argc; i++ ) {
                im_arg_desc *arg = &fn->argv[i];
                im_arg_type vt = arg->desc->type;
                VipsArgumentType t = vips_type_find( vt );

                if( t == VIPS_NONE ) {
                        /* Unknown type .. if DISPLAY it's OK.
                         */
                        if( strcmp( vt, IM_TYPE_DISPLAY ) != 0 )
                                return( -1 );
                }
                else if( !(arg->desc->flags & IM_TYPE_OUTPUT) )
                        /* Found an input arg.
                         */
                        nin += 1;
        }

	return( nin );
}

/* Make an im_doublevec_object.
 */
static int
vips_make_doublevec( im_doublevec_object *dv, int n, double *vec )
{
	int i;

	dv->n = n;
	if( !(dv->vec = IARRAY( NULL, n, double )) )
		return( -1 );
	for( i = 0; i < n; i++ )
		dv->vec[i] = vec[i];

	return( 0 );
}

/* Make an im_intvec_object. Make from a vec of doubles, because that's what
 * we get from nip.
 */
static int
vips_make_intvec( im_intvec_object *dv, int n, double *vec )
{
	int i;

	dv->n = n;
	if( !(dv->vec = IARRAY( NULL, n, int )) )
		return( -1 );
	for( i = 0; i < n; i++ )
		dv->vec[i] = vec[i];

	return( 0 );
}

/* Make an im_imagevec_object.
 */
static int
vips_make_imagevec( im_imagevec_object *iv, int n, IMAGE **vec )
{
	int i;

	iv->n = n;
	if( !(iv->vec = IARRAY( NULL, n, IMAGE * )) )
		return( -1 );
	for( i = 0; i < n; i++ )
		iv->vec[i] = vec[i];

	return( 0 );
}

/* ip types -> VIPS types. Write to obj. FALSE for no conversion possible.
 */
static gboolean
vips_fromip( Reduce *rc, PElement *arg, 
	im_type_desc *vips, im_object *obj )
{
	VipsArgumentType vt = vips_type_find( vips->type );

	/* If vips_type_find failed, is it the special DISPLAY type?
	 */
	if( vt == VIPS_NONE && strcmp( vips->type, IM_TYPE_DISPLAY ) != 0 ) 
		/* Unknown type, and it's not DISPLAY. Flag an error.
		 */
		return( FALSE );

	switch( vt ) {
	case VIPS_NONE:	/* IM_TYPE_DISPLAY */
		/* Just use IM_TYPE_sRGB.
		 */
		*obj = im_col_displays( 7 );
		break;

	case VIPS_DOUBLE:
	{
		double *a = *obj;

		if( !PEISREAL( arg ) )
			return( FALSE );
		*a = PEGETREAL( arg );
		break;
	}

	case VIPS_INT:
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

	case VIPS_COMPLEX:
	{
		double *c = *obj;

		if( !PEISCOMPLEX( arg ) )
			return( FALSE );
		c[0] = PEGETREALPART( arg );
		c[1] = PEGETIMAGPART( arg );
		break;
	}

	case VIPS_STRING:
	{
		char **c = (char **) obj;
		char buf[MAX_STRSIZE];

		(void) reduce_get_string( rc, arg, buf, MAX_STRSIZE );
		*c = im_strdup( NULL, buf );
		break;
	}

	case VIPS_IMAGE:
	{
		/* Put Imageinfo in for now ... a later pass changes this to
		 * IMAGE* once we've checked all the LUTs.
		 */
		Imageinfo **im = (Imageinfo **) obj;

		if( !PEISIMAGE( arg ) )
			return( FALSE );

		/* Do an IMAGEINFO() as well as GETII to validate the pointer.
		 */
		*im = IMAGEINFO( PEGETII( arg ) );
		break;
	}

	case VIPS_DOUBLEVEC:
	{
		double buf[100];
		int n;

		n = reduce_get_realvec( rc, arg, buf, 100 );
		if( vips_make_doublevec( *obj, n, buf ) )
			return( FALSE );

		break;
	}

	case VIPS_INTVEC:
	{
		double buf[100];
		int n;

		n = reduce_get_realvec( rc, arg, buf, 100 );
		if( vips_make_intvec( *obj, n, buf ) )
			return( FALSE );

		break;
	}

	case VIPS_IMAGEVEC:
	{
		Imageinfo *buf[100];
		int n;

		/* Put Imageinfo in for now ... a later pass changes this to
		 * IMAGE* once we've checked all the LUTs.
		 */
		n = reduce_get_imagevec( rc, arg, buf, 100 );
		if( vips_make_imagevec( *obj, n, (IMAGE **) buf ) )
			return( FALSE );

		break;
	}

	case VIPS_DMASK:
	case VIPS_IMASK:
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

	case VIPS_GVALUE:
	{
		GValue *value = *obj;

		memset( value, 0, sizeof( GValue ) );
		if( !heap_ip_to_gvalue( arg, value ) )
			return( FALSE );

		break;
	}

	default:
		g_assert( FALSE );
	}

	return( TRUE );
}

/* VIPS types -> ip types. Write to arg. Use outiiindex to iterate through
 * outii[] as we find output imageinfo.
 */
static void
vips_toip( VipsInfo *vi, int i, int *outiiindex, PElement *arg )
{
	im_object obj = vi->vargv[i];
	im_type_desc *vips = vi->fn->argv[i].desc;
	VipsArgumentType vt = vips_type_find( vips->type );

#ifdef DEBUG
	printf( "vips_toip: arg[%d] (%s) = ", i, vips->type );
#endif /*DEBUG*/

	switch( vt ) {
	case VIPS_DOUBLE:
		if( !heap_real_new( vi->rc->heap, *((double*)obj), arg ) )
			reduce_throw( vi->rc );
		break;

	case VIPS_INT:
		if( !heap_real_new( vi->rc->heap, *((int*)obj), arg ) )
			reduce_throw( vi->rc );
		break;

	case VIPS_DOUBLEVEC:
	{
		im_doublevec_object *dv = obj;

		if( !heap_realvec_new( vi->rc->heap, dv->n, dv->vec, arg ) )
			reduce_throw( vi->rc );
		break;
	}

	case VIPS_INTVEC:
	{
		im_intvec_object *iv = obj;

		if( !heap_intvec_new( vi->rc->heap, iv->n, iv->vec, arg ) )
			reduce_throw( vi->rc );
		break;
	}

	case VIPS_COMPLEX:
		if( !heap_complex_new( vi->rc->heap, 
			((double*)obj)[0], ((double*)obj)[1], arg ) )
			reduce_throw( vi->rc );
		break;

	case VIPS_STRING:
		if( !heap_string_new( vi->rc->heap, (char*)obj, arg ) )
			reduce_throw( vi->rc );
		break;

	case VIPS_IMAGE:
	{
		Imageinfo *outii;

		outii = vi->outii[*outiiindex];
		*outiiindex += 1;

		PEPUTP( arg, ELEMENT_MANAGED, outii );
		break;
	}

	case VIPS_DMASK:
	{
		im_mask_object *mo = obj;
		DOUBLEMASK *mask = mo->mask;

		if( !matrix_dmask_to_heap( vi->rc->heap, mask, arg ) )
			reduce_throw( vi->rc );

		break;
	}

	case VIPS_IMASK:
	{
		im_mask_object *mo = obj;
		INTMASK *mask = mo->mask;

		if( !matrix_imask_to_heap( vi->rc->heap, mask, arg ) )
			reduce_throw( vi->rc );

		break;
	}

	case VIPS_GVALUE:
		if( !heap_gvalue_to_ip( (GValue *) obj, arg ) )
			reduce_throw( vi->rc );
		break;

	case VIPS_IMAGEVEC:
	default:
		g_assert( FALSE );
	}

#ifdef DEBUG
	pgraph( arg );
#endif /*DEBUG*/
}

/* Sort out the input images. Can we do this with a LUT?
 */
static void
vips_gather( VipsInfo *vi ) 
{
	int i, j;

	/* Find all the input images.
	 */
	for( i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;

		if( strcmp( ty->type, IM_TYPE_IMAGE ) == 0 && 
			!(ty->flags & IM_TYPE_OUTPUT) ) {
			vi->inii[vi->ninii] = vi->vargv[i];
			vi->ninii += 1;
		}

		if( strcmp( ty->type, IM_TYPE_IMAGEVEC ) == 0 && 
			!(ty->flags & IM_TYPE_OUTPUT) ) {
			im_imagevec_object *iv = 
				(im_imagevec_object *) vi->vargv[i];

			for( j = 0; j < iv->n; j++ ) {
				vi->inii[vi->ninii] = IMAGEINFO( iv->vec[j] );
				vi->ninii += 1;
			}
		}
	}

	/* No input images.
	 */
	if( vi->ninii == 0 )
		return;

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

	/* Now zap the vargv vector with the correct IMAGE pointers.
	 */
	for( i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;

		if( strcmp( ty->type, IM_TYPE_IMAGE ) == 0 && 
			!(ty->flags & IM_TYPE_OUTPUT) ) {
			if( !(vi->vargv[i] = 
				imageinfo_get( vi->use_lut, vi->vargv[i] )) )
				reduce_throw( vi->rc );
		}

		if( strcmp( ty->type, IM_TYPE_IMAGEVEC ) == 0 && 
			!(ty->flags & IM_TYPE_OUTPUT) ) {
			im_imagevec_object *iv = 
				(im_imagevec_object *) vi->vargv[i];

			/* Found an input image vector. Add all the imageinfo
			 * in the vector.
			 */
			for( j = 0; j < iv->n; j++ ) {
				Imageinfo *ii = IMAGEINFO( iv->vec[j] );

				if( !(iv->vec[j] = imageinfo_get( 
					vi->use_lut, ii )) )
					reduce_throw( vi->rc );
			}
		}
	}
}

/* Init an output slot in vargv.
 */
static void
vips_build_output( VipsInfo *vi, int i )
{
	im_type_desc *ty = vi->fn->argv[i].desc;
	VipsArgumentType vt = vips_type_find( ty->type );
	char tname[FILENAME_MAX];

	/* Provide output objects for the function to write to.
	 */
	switch( vt ) {
	case VIPS_DOUBLE:
	case VIPS_INT:
	case VIPS_COMPLEX:
	case VIPS_STRING:
		break;

	case VIPS_IMAGE:
		if( !temp_name( tname, "v" ) || 
			!(vi->vargv[i] = im_open( tname, "p" )) ) {
			vips_error( vi );
			reduce_throw( vi->rc );
		}
		break;

	case VIPS_DMASK:
	case VIPS_IMASK:
	{
		im_mask_object *mo = vi->vargv[i];

		mo->mask = NULL;
		mo->name = im_strdup( NULL, "" );

		break;
	}

	case VIPS_GVALUE:
	{
		GValue *value = vi->vargv[i];

		memset( value, 0, sizeof( GValue ) );

		break;
	}

	case VIPS_DOUBLEVEC:
	case VIPS_INTVEC:
	{
		/* intvev is also int + pointer.
		 */
		im_doublevec_object *dv = vi->vargv[i];

		dv->n = 0;
		dv->vec = NULL;

		break;
	}

	default:
		g_assert( FALSE );
	}
}

/* Fill an argument vector from our stack frame. Number of args already
 * checked. 
 */
static void
vips_fill_spine( VipsInfo *vi, HeapNode **arg )
{
	int i, j;

	/* Fully reduce all arguments. Once we've done this, we can be sure
	 * there will not be a GC while we gather, and therefore that no
	 * pointers will become invalid during this call.
	 */
	for( i = 0; i < vi->nargs; i++ ) {
		PElement rhs;

		PEPOINTRIGHT( arg[i], &rhs );
		reduce_spine_strict( vi->rc, &rhs );
	}

	for( j = 0, i = 0; i < vi->fn->argc; i++ ) {
		im_arg_desc *varg = &vi->fn->argv[i];

		if( varg->desc->flags & IM_TYPE_OUTPUT ) 
			vips_build_output( vi, i ); 
		else if( strcmp( varg->desc->type, IM_TYPE_DISPLAY ) == 0 ) {
			/* Special DISPLAY argument - don't fetch another ip
			 * argument for it.
			 */
			(void) vips_fromip( vi->rc, 
				NULL, varg->desc, &vi->vargv[i] );
		}
		else {
			PElement rhs;

#ifdef DEBUG
			printf( "vips_fill_spine: arg[%d] (%s) = ", 
				i, varg->desc->type );
#endif /*DEBUG*/

			/* Convert ip type to VIPS type.
			 */
			PEPOINTRIGHT( arg[vi->nargs - j - 1], &rhs );
			if( !vips_fromip( vi->rc, 
				&rhs, varg->desc, &vi->vargv[i] ) ) {
				vips_error_arg( vi, arg, j );
				reduce_throw( vi->rc );
			}

			j += 1;
		}
	}
}

static void *
vips_write_result_sub( Reduce *rc, PElement *safe, VipsInfo *vi, PElement *out )
{
	int outiiindex;

	/* vips_toip() uses this to iterate through outii[].
	 */
	outiiindex = 0;

	/* Write result.
	 */
	if( vi->nres == 1 )
		/* Single result.
		 */
		vips_toip( vi, vi->outpos[0], &outiiindex, safe );
	else {
		/* Have to build a list of results.
		 */
		PElement list;
		PElement t;
		int i;

		list = *safe;
		heap_list_init( &list );
		for( i = 0; i < vi->nres; i++ ) {
			if( !heap_list_add( vi->rc->heap, &list, &t ) )
				return( out );
			vips_toip( vi, vi->outpos[i], &outiiindex, &t );

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
vips_write_result( VipsInfo *vi, PElement *out )
{
	if( reduce_safe_pointer( vi->rc, 
		(reduce_safe_pointer_fn) vips_write_result_sub, 
		vi, out, NULL, NULL ) )
		return( FALSE );

	return( TRUE );
}

/* Loop over the output IMAGEs, transforming them into Imageinfo.
 */
static void
vips_wrap_output( VipsInfo *vi )
{
	int i;

	for( i = 0; i < vi->nres; i++ ) {
		int j = vi->outpos[i];
		IMAGE *im = (IMAGE *) vi->vargv[j];
		im_type_desc *vips = vi->fn->argv[j].desc;
		VipsArgumentType vt = vips_type_find( vips->type );
		Imageinfo *outii;

		if( vt != VIPS_IMAGE )
			continue;

		if( vi->use_lut ) {
			/* We are using a LUT. 
			 */
			if( !(outii = imageinfo_new_modlut( main_imageinfogroup,
				vi->rc->heap, vi->inii[0], im )) )
				reduce_throw( vi->rc );
		}
		else {
			if( !(outii = imageinfo_new( main_imageinfogroup, 
				vi->rc->heap, im, NULL )) )
				reduce_throw( vi->rc );
		}

		/* This output ii depends upon all of the input images.
		 */
		managed_sub_add_all( MANAGED( outii ), 
			vi->ninii, (Managed **) vi->inii );

		/* Junk the pointer in vargv to stop im_close() on vips end.
		 */
		vi->vargv[j] = NULL;

		/* Rewind the image.
		 */
		if( im_pincheck( im ) ) {
			vips_error( vi );
			reduce_throw( vi->rc );
		}

		/* Record on output ii table.
		 */
		vi->outii[vi->noutii++] = outii;
	}
}

/* Fill an argument vector from the C stack.
 */
static void
vips_fillva( VipsInfo *vi, va_list ap )
{
	int i, j;

	for( j = 0, i = 0; i < vi->fn->argc; i++ ) {
		im_type_desc *ty = vi->fn->argv[i].desc;
		VipsArgumentType vt = vips_type_find( ty->type );

#ifdef DEBUG
		printf( "vips_fillva: arg[%d] (%s) = ", i, ty->type );
#endif /*DEBUG*/

		if( ty->flags & IM_TYPE_OUTPUT ) 
			vips_build_output( vi, i );
		else if( strcmp( ty->type, IM_TYPE_DISPLAY ) == 0 ) {
			/* DISPLAY argument ... just IM_TYPE_sRGB.
			 */
			vi->vargv[i] = im_col_displays( 7 );
#ifdef DEBUG
			printf( "ip-display-calib\n" );
#endif /*DEBUG*/
		}
		else if( vt == VIPS_DOUBLE ) {
			double v = va_arg( ap, double );

			*((double*)vi->vargv[i]) = v;

			if( trace_flags & TRACE_VIPS ) 
				buf_appendf( trace_current(), "%g ", v );
		}
		else if( vt == VIPS_INT ) {
			int v = va_arg( ap, int );

			*((int*)vi->vargv[i]) = v;

			if( trace_flags & TRACE_VIPS ) 
				buf_appendf( trace_current(), "%d ", v );
		}
		else if( vt == VIPS_GVALUE ) {
			GValue *value = va_arg( ap, GValue * );

			vi->vargv[i] = value;

			if( trace_flags & TRACE_VIPS ) {
				buf_appendgv( trace_current(), value );
				buf_appends( trace_current(), " " );
			}
		}
		else if( vt == VIPS_IMAGE ) {
			Imageinfo *ii = va_arg( ap, Imageinfo * );

			vi->vargv[i] = ii;

			if( trace_flags & TRACE_VIPS ) {
				BufInfo *buf = trace_current();

				if( ii && ii->im ) {
					buf_appends( buf, "<" );
					buf_appendf( buf, _( "image \"%s\"" ),
						ii->im->filename ); 
					buf_appends( buf, "> " );
				}
				else {
					buf_appends( buf, "<" );
					buf_appends( buf, _( "no image" ) );
					buf_appends( buf, "> " );
				}
			}
	    	}
		else if( vt == VIPS_DOUBLEVEC ) {
			int n = va_arg( ap, int );
			double *vec = va_arg( ap, double * );

			if( vips_make_doublevec( vi->vargv[i], n, vec ) )
				reduce_throw( vi->rc );
			if( trace_flags & TRACE_VIPS ) {
				BufInfo *buf = trace_current();
				int i;

				buf_appendf( buf, "<" );
				buf_appendf( buf, _( "doublevec" ) );
				for( i = 0; i < n; i++ )
					buf_appendf( buf, " %g", vec[i] );
				buf_appends( buf, "> " );
			}
		}
		/* 

			FIXME ... add intvec perhaps

		 */
		else if( vt == VIPS_IMAGEVEC ) {
			int n = va_arg( ap, int );
			Imageinfo **vec = va_arg( ap, Imageinfo ** );

			if( vips_make_imagevec( vi->vargv[i], 
				n, (IMAGE **) vec ) )
				reduce_throw( vi->rc );
			if( trace_flags & TRACE_VIPS ) {
				BufInfo *buf = trace_current();
				int i;

				buf_appendf( buf, "<" );
				buf_appendf( buf, _( "imagevec" ) );
				for( i = 0; i < n; i++ ) {
					buf_appendf( buf, " <" );
					buf_appendf( buf, _( "image \"%s\"" ),
						vec[i]->im->filename );
					buf_appendf( buf, ">" );
				}
				buf_appends( buf, "> " );
			}
		}

		else
			g_assert( FALSE );
	}
}

static void
vips_dispatch( VipsInfo *vi, PElement *out )
{
	Reduce *rc = vi->rc;
	VipsInfo *old_vi;

	/* Look over the images we have ... turn input Imageinfo to IMAGE.
	 * If we can do this with a lut, set all that up.
	 */
	vips_gather( vi );

	/* Is this function call in the history?
	 */
	if( (old_vi = vips_history_lookup( vi )) ) {
		/* Yes: reuse!
		 */
		vips_destroy( vi );
		vi = old_vi;

		if( trace_flags & TRACE_VIPS ) 
			buf_appendf( trace_current(), "(from cache) " );

#ifdef DEBUG_HISTORY
		printf( "vips_dispatch: found %s in history\n", vi->name );
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
		printf( "vips_dispatch: calling %s\n", vi->name );
#endif /*DEBUG_HISTORY_MISS*/

		/* Be careful. This may well call back into nip2 via progress
		 * feedback and result in a recursive invocation of vips_call.
		 */
		result = vi->fn->disp( vi->vargv );

#ifdef DEBUG_TIME
		printf( "vips_dispatch: %s - %g\n", 
			vi->name, g_timer_elapsed( timer, NULL ) );
#endif /*DEBUG_TIME*/

		if( result ) {
			vips_error_fn_vips( vi );
			vips_destroy( vi );
			reduce_throw( rc );
		}
		vips_update_hist( vi );

		/* Transform output IMAGE back into Imageinfo.
		 */
		vips_wrap_output( vi );
	}

	/* Write result back to heap. 
	 */
	if( !vips_write_result( vi, out ) ) {
		vips_destroy( vi );
		reduce_throw( rc );
	}

	/* Add to our operation cache, if necessary.
	 */
	if( vi->fn->flags & IM_FN_NOCACHE ) 
		/* Uncacheable operation.
		 */
		vips_destroy( vi );
	else if( vi->in_cache ) 
		/* Already in the history. Just touch the time.
		 */
		vips_history_touch( vi );
	else if( (old_vi = vips_history_lookup( vi )) ) {
		/* We have an equal but older item there? This can happen with
		 * nested calls. Touch the old one and destroy this one.
		 */
		vips_history_touch( old_vi );
		vips_destroy( vi );
	}
	else
		vips_history_add( vi );
}

static gboolean
vips_spine_sub( VipsInfo *vi, PElement *out, HeapNode **arg )
{
	Reduce *rc = vi->rc;

	REDUCE_CATCH_START( FALSE );

	vips_fill_spine( vi, arg );

	vips_dispatch( vi, out );

	REDUCE_CATCH_STOP;

	return( TRUE );
}

/* Call a VIPS function, pick up args from the graph. 
 */
void
vips_spine( Reduce *rc, const char *name, HeapNode **arg, PElement *out )
{
	VipsInfo *vi;
	gboolean res;

#ifdef DEBUG
	printf( "vips_spine: starting for %s\n", name );
#endif /*DEBUG*/

	if( !(vi = vips_new( rc, im_find_function( name ) )) )
		reduce_throw( rc );

	if( trace_flags & TRACE_VIPS ) {
		BufInfo *buf = trace_push();

		buf_appendf( buf, "\"%s\" ", name );
		trace_args( arg, vi->nargs );
	}

	res = vips_spine_sub( vi, out, arg );

	if( !res )
		reduce_throw( rc );

	if( trace_flags & TRACE_VIPS ) {
		trace_result( TRACE_VIPS, out );
		trace_pop();
	}

#ifdef DEBUG
	printf( "vips_spine: success\n" );
#endif /*DEBUG*/
}

/* As an ActionFn.
 */
void
vips_run( Reduce *rc, Compile *compile,
	int op, const char *name, HeapNode **arg, PElement *out,
	im_function *function )
{
	VipsInfo *vi;
	gboolean res;

#ifdef DEBUG
	printf( "vips_run: starting for %s\n", name );
#endif /*DEBUG*/

#ifdef DEBUG_HISTORY_SANITY
	vips_history_sanity();
#endif /*DEBUG_HISTORY_SANITY*/

	if( !(vi = vips_new( rc, function )) )
		reduce_throw( rc );

	if( trace_flags & TRACE_VIPS ) {
		BufInfo *buf = trace_push();

		buf_appendf( buf, "\"%s\" ", name );
		trace_args( arg, vi->nargs );
	}

	res = vips_spine_sub( vi, out, arg );

	if( !res )
		reduce_throw( rc );

	if( trace_flags & TRACE_VIPS ) {
		trace_result( TRACE_VIPS, out );
		trace_pop();
	}
}

static gboolean
vipsva_sub( VipsInfo *vi, PElement *out, va_list ap )
{
	Reduce *rc = vi->rc;

	REDUCE_CATCH_START( FALSE );

	if( trace_flags & TRACE_VIPS ) 
		buf_appendf( trace_current(), "\"%s\" ", vi->name );

	/* Fill argv ... input images go in as Imageinfo here, output as
	 * IMAGE.
	 */
	vips_fillva( vi, ap );

	if( trace_flags & TRACE_VIPS ) 
		buf_appends( trace_current(), " ->\n" ); 

	vips_dispatch( vi, out );

	REDUCE_CATCH_STOP;

	return( TRUE );
}

/* Call a VIPS function, but pick up args from the function call.
 */
void
vipsva( Reduce *rc, PElement *out, const char *name, ... )
{
	va_list ap;
	VipsInfo *vi;
	gboolean res;

	if( trace_flags & TRACE_VIPS ) 
		trace_push();

#ifdef DEBUG
	printf( "vipsva: starting for %s\n", name );
#endif /*DEBUG*/

	if( !(vi = vips_new( rc, im_find_function( name ) )) )
		reduce_throw( rc );
        va_start( ap, name );
	res = vipsva_sub( vi, out, ap );
        va_end( ap );

	if( !res )
		reduce_throw( rc );

	if( trace_flags & TRACE_VIPS ) {
		trace_result( TRACE_VIPS, out );
		trace_pop();
	}

#ifdef DEBUG
	printf( "vipsva: success\n" );
#endif /*DEBUG*/
}
