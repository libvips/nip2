/* Graph reducer.
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

/* trace each regeneration
#define DEBUG_REGEN
 */

/* trace copies of code from compile heap to main heap.
#define DEBUG_COPY
 */

/* trace just member regeneration
#define DEBUG_REGEN_MEMBER
 */

/* Turn on WHNF tests.
#define WHNF_DEBUG
 */

/* regular tests that we stay in weak head normal form
#define WHNF_DEBUG
 */

/* State of the reduction engine.
 */
Reduce *reduce_context;

/* Index with a CombinatorType, get the number of args that combinator takes.
        COMB_S = 0,		
        COMB_SL,	
        COMB_SR,
	COMB_I,
	COMB_K,
	COMB_GEN,
 */
static int nargs[] = {3, 3, 3, 1, 2, 3};

/* Recomps this time.
 */
int reduce_total_recomputations = 0;

/* The current expr being reduced. Used for computation feedback messages.
 */
static Expr *reduce_current_expr = NULL;

/* Eval error here. Longjmp back a ways.
 */
void
reduce_throw( Reduce *rc )
{
	if( !rc->running ) 
		error( "panic: uncaught exception in reduce_throw()!" );
	else
		longjmp( rc->error[--rc->running], -1 );
}

/* Call a function, passing in a "safe" PElement ... ie. the PElement points
 * at a fresh element which will be safe from the GC.
 */
void *
reduce_safe_pointer( Reduce *rc, reduce_safe_pointer_fn fn, 
	void *a, void *b, void *c, void *d )
{
	Element e;
	PElement pe;
	void *result;

	e.type = ELEMENT_NOVAL;
	e.ele = (void *) 12;
	PEPOINTE( &pe, &e );
	heap_register_element( rc->heap, &e );

	result = fn( rc, &pe, a, b, c, d );

	heap_unregister_element( rc->heap, &e );

	return( result );
}

void
reduce_error_typecheck( Reduce *rc, 
	PElement *e, const char *name, const char *type )
{
	VipsBuf buf;
	char txt[1024];

	error_top( _( "Typecheck error." ) );
	vips_buf_init_static( &buf, txt, 1024 );
	vips_buf_appendf( &buf, _( "%s expected %s, instead saw:" ), name, type );
	vips_buf_appends( &buf, "\n  " );
	itext_value_ev( rc, &buf, e );
	error_sub( "%s", vips_buf_all( &buf ) );

	reduce_throw( rc );
}

static void
reduce_error_toobig( Reduce *rc, const char *name )
{
	error_top( _( "Overflow error." ) );
	error_sub( _( "%s too long." ), name );
	reduce_throw( rc );
}

/* 'get' a list: convert a MANAGEDSTRING into a list, if necessary.
 */
void
reduce_get_list( Reduce *rc, PElement *list )
{
	if( !heap_get_list( list ) )
		reduce_throw( rc );
}

/* Map over a heap list. Reduce the list spine as we go, don't reduce the
 * heads. 
 */
void *
reduce_map_list( Reduce *rc, 
	PElement *base, reduce_map_list_fn fn, void *a, void *b )
{
	PElement e = *base;

	reduce_spine( rc, &e );

	if( !PEISLIST( &e ) ) 
		reduce_error_typecheck( rc, &e, "reduce_map_list", "list" );

	while( PEISFLIST( &e ) ) {
		PElement head;
		void *res;

		reduce_get_list( rc, &e );

		/* Apply user function to the head.
		 */
		PEGETHD( &head, &e );
		if( (res = fn( rc, &head, a, b )) )
			return( res );

		/* Reduce the tail.
		 */
		PEGETTL( &e, &e );
		reduce_spine( rc, &e );
	}

	return( NULL );
}

typedef struct _ReduceMapDict {
	reduce_map_dict_fn fn;
	void *a;
	void *b;
} ReduceMapDict;

static void *
reduce_map_dict_entry( Reduce *rc, PElement *head, ReduceMapDict *map_dict )
{
	char key[256];
	PElement p1, p2;
	void *result;

	reduce_spine( rc, head );
	if( !PEISFLIST( head ) ) 
		reduce_error_typecheck( rc, head, "reduce_map_dict", "[*]" );

	reduce_get_list( rc, head );
	PEGETHD( &p1, head );
	reduce_get_string( rc, &p1, key, 256 );
	PEGETTL( &p2, head );

	reduce_spine( rc, &p2 );
	if( !PEISFLIST( &p2 ) ) 
		reduce_error_typecheck( rc, &p2, "reduce_map_dict", "[*]" );

	reduce_get_list( rc, &p2 );
	PEGETHD( &p1, &p2 );
	if( (result = map_dict->fn( rc, key, &p1, map_dict->a, map_dict->b )) )
		return( result );

	PEGETTL( &p1, &p2 );
	reduce_spine( rc, &p1 );
	if( !PEISELIST( &p1 ) ) 
		reduce_error_typecheck( rc, &p1, "reduce_map_dict", "[]" );

	return( NULL );
}

/* Map over a list of ["key", value] pairs.
 */
void *
reduce_map_dict( Reduce *rc, PElement *base, 
	reduce_map_dict_fn fn, void *a, void *b )
{
	ReduceMapDict map_dict;

	map_dict.fn = fn;
	map_dict.a = a;
	map_dict.b = b;

	return( reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_map_dict_entry, &map_dict, NULL ) );
}

static void *
reduce_clone_list_sub( Reduce *rc, PElement *data, PElement *out )
{
	PElement lhs;

	if( !heap_list_add( rc->heap, out, &lhs ) )
		reduce_throw( rc );
	PEPUTPE( &lhs, data );

	heap_list_next( out );

	return( NULL );
}

/* Clone a list ... just clone the spine, copy pointers to the heads. Reduce
 * the list as we go (strict shallow clone). We update out as we go, so that
 * on return it points to the tail (always []) of the cloned list.
 */
void
reduce_clone_list( Reduce *rc, PElement *base, PElement *out )
{
	heap_list_init( out );

	(void) reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_clone_list_sub, out, NULL );
}

/* Sub-fn of below. Add a character to the buffer.
 */
static void *
reduce_add_char( Reduce *rc, PElement *base, char **buf, int *sz )
{
	/* Overflow?
	 */
	if( *sz == 0 ) 
		reduce_error_toobig( rc, "[char]" );

	/* Reduce this list element.
	 */
	reduce_spine( rc, base );

	/* Should be a char.
	 */
	if( !PEISCHAR( base ) ) 
		reduce_error_typecheck( rc, base, "reduce_add_char", "char" );

	/* Add to buffer.
	 */
	**buf = PEGETCHAR( base );
	(*buf)++;
	(*sz)--;

	return( NULL );
}

/* Evaluate a PElement into a string buffer. Return the number of characters 
 * in string, not including '\0' terminator.
 */
int
reduce_get_string( Reduce *rc, PElement *base, char *buf, int n )
{
	int sz = n - 1;

	reduce_spine( rc, base );

	if( PEISMANAGEDSTRING( base ) ) {
		/* A static string ... rather than expanding to a list and
		 * parsing, we can copy directly.
		 */
		Managedstring *managedstring = PEGETMANAGEDSTRING( base );

		im_strncpy( buf, managedstring->string, n );
		sz -= strlen( buf );
	}
	else {
		(void) reduce_map_list( rc, base, 
			(reduce_map_list_fn) reduce_add_char, &buf, &sz );

		/* Add '\0' terminator.
		 */
		*buf = '\0';
	}

	return( n - sz - 1 );
}

static void *
reduce_get_lstring_sub( Reduce *rc, PElement *base, GSList **labels, int *n )
{
	char buf[MAX_STRSIZE];

	(void) reduce_get_string( rc, base, buf, MAX_STRSIZE );
	*labels = g_slist_append( *labels, g_strdup( buf ) );

	return( NULL );
}

/* Evaluate to [[char]]. Return the number of strings we read. 
 */
int
reduce_get_lstring( Reduce *rc, PElement *base, GSList **labels )
{
	int n;

	n = 0;
	*labels = NULL;
	(void) reduce_map_list( rc, base,
		(reduce_map_list_fn) reduce_get_lstring_sub, labels, &n );

	return( n );
}

/* Get an element as a boolean. 
 */
gboolean
reduce_get_bool( Reduce *rc, PElement *base )
{
	reduce_spine( rc, base );

	if( !PEISBOOL( base ) ) 
		reduce_error_typecheck( rc, base, "reduce_get_bool", "bool" );

	return( PEGETBOOL( base ) );
}

/* Get an element as a real. 
 */
double
reduce_get_real( Reduce *rc, PElement *base )
{
	/* Reduce this element.
	 */
	reduce_spine( rc, base );

	/* Should be a real.
	 */
	if( !PEISREAL( base ) ) 
		reduce_error_typecheck( rc, base, "reduce_get_real", "real" );

	return( PEGETREAL( base ) );
}

/* Get an element as a class. 
 */
void
reduce_get_class( Reduce *rc, PElement *base )
{
	/* Reduce this element.
	 */
	reduce_spine( rc, base );

	/* Should be a class.
	 */
	if( !PEISCLASS( base ) ) 
		reduce_error_typecheck( rc, base, "reduce_get_class", "class" );
}

/* Get an element as an image. 
 */
Imageinfo *
reduce_get_image( Reduce *rc, PElement *base )
{
	/* Reduce this element.
	 */
	reduce_spine( rc, base );

	/* Should be an image.
	 */
	if( !PEISIMAGE( base ) ) 
		reduce_error_typecheck( rc, base, "reduce_get_image", "image" );

	return( PEGETII( base ) );
}

/* Sub-fn of below. Add a real to the buffer.
 */
static void *
reduce_add_real( Reduce *rc, PElement *base, double **buf, int *sz )
{
	/* Overflow?
	 */
	if( *sz == 0 ) 
		reduce_error_toobig( rc, "[real]" );

	/* Add to buffer.
	 */
	**buf = reduce_get_real( rc, base );
	(*buf)++;
	(*sz)--;

	return( NULL );
}

/* Get an element as a realvec. Return length of vector.
 */
int
reduce_get_realvec( Reduce *rc, PElement *base, double *buf, int n )
{
	int sz = n;

	(void) reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_add_real, &buf, &sz );

	return( n - sz );
}

/* Sub-fn of below. Add an ii to the buffer.
 */
static void *
reduce_add_image( Reduce *rc, PElement *base, Imageinfo ***buf, int *sz )
{
	/* Overflow?
	 */
	if( *sz == 0 ) 
		reduce_error_toobig( rc, "[image]" );

	/* Add to buffer.
	 */
	**buf = reduce_get_image( rc, base );
	(*buf)++;
	(*sz)--;

	return( NULL );
}

/* Get an element as a realvec. Return length of vector.
 */
int
reduce_get_imagevec( Reduce *rc, PElement *base, Imageinfo **buf, int n )
{
	int sz = n;

	(void) reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_add_image, &buf, &sz );

	return( n - sz );
}

/* Test for 1st sz elements are reals. Init sz < 0 for unlimited test.
 */
static void *
reduce_test_real( Reduce *rc, PElement *base, int *sz )
{
	/* Tested enough?
	 */
	if( *sz == 0 ) 
		return( NULL );

	(void) reduce_get_real( rc, base );
	(*sz)--;

	return( NULL );
}

/* Sub fn ... get the length of a list of real.
 */
int
reduce_get_real_size( Reduce *rc, PElement *base )
{
	int n;

	n = -1;
	(void) reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_test_real, &n, NULL );

	return( -1 - n );
}

/* Sub fn of below ... get the length of one line from a matrix.
 */
static void *
reduce_get_line_size( Reduce *rc, PElement *base, int *w, int *h )
{
	int l;

	l = reduce_get_real_size( rc, base );

	if( *w == 0 )
		*w = l;
	else if( *w != l ) {
		error_top( _( "Not rectangular." ) );
		error_sub( _( "Matrix of real is not rectangular. "
			"Found row of length %d, should be %d." ), l, *w );
		reduce_throw( rc );
	}

	*h += 1;

	return( NULL );
}

/* Find the size of a matrix. Write xsize/ysize to args.
 */
void
reduce_get_matrix_size( Reduce *rc, 
	PElement *base, int *xsize, int *ysize )
{
	int w, h;

	w = 0; 
	h = 0;
	(void) reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_get_line_size, &w, &h );

	*xsize = w;
	*ysize = h;
}

/* Track stuff during a get_matrix in one of these.
 */
typedef struct {
	double *buf;		/* Start of output buffer */
	int mx;			/* Size of output buffer */
	int w, h;		/* Size of matrix we have generated */
	int i;			/* Current write point */
} GetMatrixInfo;

/* Sub-fn of below ... get another line of the matrix.
 */
static void *
reduce_get_line( Reduce *rc, PElement *base, GetMatrixInfo *gmi )
{
	int l;
	int remain = gmi->mx - gmi->i;

	/* Read next line from matrix.
	 */
	l = reduce_get_realvec( rc, base, gmi->buf + gmi->i, remain );

	/* Overflow?
	 */
	if( l > remain ) 
		reduce_error_toobig( rc, "Matrix" );

	/* 1st line?
	 */
	if( gmi->h == 0 )
		gmi->w = l;
	else if( l != gmi->w ) {
		error_top( _( "Not rectangular." ) );
		error_sub( _( "Matrix of real is not rectangular. "
			"Found row of length %d, should be %d." ), l, gmi->w );
		reduce_throw( rc );
	}

	/* Move pointers on!
	 */
	gmi->h++;
	gmi->i += l;

	return( NULL );
}

/* Get an element as a matrix. Return length of buffer used. 
 * Write xsize/ysize to args.
 */
int
reduce_get_matrix( Reduce *rc, 
	PElement *base, double *buf, int n, int *xsize, int *ysize )
{
	GetMatrixInfo gmi;

	gmi.buf = buf;
	gmi.mx = n;
	gmi.w = gmi.h = 0;
	gmi.i = 0;

	(void) reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_get_line, &gmi, NULL );

	*xsize = gmi.w;
	*ysize = gmi.h;

	return( gmi.i );
}

/* Test for object is the empty list.
 */
gboolean
reduce_is_elist( Reduce *rc, PElement *base )
{
	reduce_spine( rc, base );
	if( PEISELIST( base ) ) 
		return( TRUE );

	return( FALSE );
}

/* Test for object is any list.
 */
gboolean
reduce_is_list( Reduce *rc, PElement *base )
{
	reduce_spine( rc, base );
	if( PEISLIST( base ) ) 
		return( TRUE );

	return( FALSE );
}

/* Sub-fn of below. Test for 1st sz elements are char. We have several
 * possible return values :-(
 *
 * - evaluation error ... we can throw an exception
 * - we find a non-char in the first n elements ... return -1
 * - we have tested the first n and want to stop looking ... return -2
 * - all OK so far, but we want to keep looking ... return NULL
 */
static void *
reduce_test_char( Reduce *rc, PElement *base, int *sz )
{
	/* Tested enough?
	 */
	if( *sz == 0 ) 
		return( (void *) -2 );

	/* Reduce this list element.
	 */
	reduce_spine( rc, base );

	/* Should be a char.
	 */
	if( !PEISCHAR( base ) ) 
		return( (void *) -1 );

	/* Move on.
	 */
	(*sz)--;

	return( NULL );
}

/* Test the first n elements of a list are char. n < 0 means test all
 * elements.
 */
static gboolean
reduce_n_is_string( Reduce *rc, PElement *base, int sz )
{
	reduce_spine( rc, base );

	if( !PEISLIST( base ) ) 
		return( FALSE );

	/* We know managedstrings are strings without needing to expand them.
	 */
	if( PEISMANAGEDSTRING( base ) )
		return( TRUE );
	else {
		void *result;

		result = reduce_map_list( rc, base, 
			(reduce_map_list_fn) reduce_test_char, &sz, NULL );

		if( result == (void *) -1 )
			return( FALSE );

		return( TRUE );
	}
}

/* Test for object is string. Just test the first few elements, so we
 * allow infinite strings.
 */
gboolean
reduce_is_string( Reduce *rc, PElement *base )
{
	return( reduce_n_is_string( rc, base, 4 ) );
}

/* Test for list is a finite string. 
 */
gboolean
reduce_is_finitestring( Reduce *rc, PElement *base )
{
	return( reduce_n_is_string( rc, base, -1 ) );
}

/* Test for list is realvec.
 */
gboolean
reduce_is_realvec( Reduce *rc, PElement *base )
{
	int sz = 4;

	reduce_spine( rc, base );
	if( !PEISLIST( base ) ) 
		return( FALSE );

	if( reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_test_real, &sz, NULL ) )
		return( FALSE );

	return( TRUE );
}

/* Sub-fn of below ... test another line of the matrix.
 */
static void *
reduce_test_line( Reduce *rc, PElement *base, int *w, int *h )
{
	/* Test next line from matrix.
	 */
	if( !reduce_is_realvec( rc, base ) )
		return( base );

	return( NULL );
}

/* Test for object is [[real]] .. don't test for rectangularness.
 */
gboolean
reduce_is_matrix( Reduce *rc, PElement *base )
{
	reduce_spine( rc, base );
	if( !PEISLIST( base ) ) 
		return( FALSE );

	if( reduce_map_list( rc, base, 
		(reduce_map_list_fn) reduce_test_line, NULL, NULL ) )
		return( FALSE );

	return( TRUE );
}

/* Test for object is a class.
 */
gboolean
reduce_is_class( Reduce *rc, PElement *klass )
{
	reduce_spine( rc, klass );
	if( PEISCLASS( klass ) )
		return( TRUE );

	return( FALSE );
}

/* Test for instance is an exact instance ... ie. no inheritance.

	FIXME ... yuk! strcmp()!!

 */
gboolean
reduce_is_instanceof_exact( Reduce *rc, const char *name, PElement *instance )
{
        VipsBuf buf;
        char txt[256];

	if( !reduce_is_class( rc, instance ) )
		return( FALSE );

        vips_buf_init_static( &buf, txt, 256 );
        symbol_qualified_name( PEGETCLASSCOMPILE( instance )->sym, &buf );
        if( strcmp( name, vips_buf_all( &buf ) ) == 0 )
                return( TRUE );

	return( FALSE );
}

/* Test for thing is an instance of the named class symbol.
 */
gboolean
reduce_is_instanceof( Reduce *rc, const char *name, PElement *instance )
{
	PElement super;

	reduce_spine( rc, instance );
	if( !PEISCLASS( instance ) )
		return( FALSE );
	if( reduce_is_instanceof_exact( rc, name, instance ) )
		return( TRUE );
	if( class_get_super( instance, &super ) && !PEISELIST( &super ) ) 
		return( reduce_is_instanceof( rc, name, &super ) );

	return( FALSE );
}

/* Find the length of a list, with a bailout for the largest size we test.
 * Handy for avoiding finding the length of "[1..]".
 */
int
reduce_list_length_max( Reduce *rc, PElement *base, int max_length )
{
	PElement p;
	int i;

	/* Reduce to first element.
	 */
	p = *base;
	reduce_spine( rc, &p );

	/* Does it look like the start of a list? 
	 */
	if( !PEISLIST( &p ) ) 
		reduce_error_typecheck( rc, &p, _( "List length" ), "list" );

	if( PEISMANAGEDSTRING( &p ) ) {
		Managedstring *managedstring = PEGETMANAGEDSTRING( &p );

		i = strlen( managedstring->string );
	}
	else {
		/* Loop down list.
		 */
		for( i = 0; PEISFLIST( &p ); i++ ) {
			HeapNode *hn;

			if( max_length != -1 && i > max_length ) 
				reduce_error_toobig( rc, "list" );

			reduce_get_list( rc, &p );

			hn = PEGETVAL( &p );
			PEPOINTRIGHT( hn, &p );

			reduce_spine( rc, &p );
		}

		g_assert( PEISELIST( &p ) );
	}

	return( i );
}

/* Find the length of a list.
 */
int
reduce_list_length( Reduce *rc, PElement *base )
{
	return( reduce_list_length_max( rc, base, -1 ) );
}

/* Point "out" at the nth element of a list. Index from 0.
 */
void
reduce_list_index( Reduce *rc, PElement *base, int n, PElement *out )
{
	PElement p;
	int i;
	HeapNode *hn;

	if( n < 0 ) {
		error_top( _( "Bad argument." ) );
		error_sub( _( "List index must be positive, not %d" ), n );
		reduce_throw( rc );
	}

	p = *base;
	reduce_spine( rc, &p );

	if( !PEISLIST( &p ) ) 
		reduce_error_typecheck( rc, &p, _( "List index" ), "list" );

	for( i = n;; ) {
		if( PEISELIST( &p ) ) {
			error_top( _( "Bad argument." ) );
			error_sub( _( "List only has %d elements, "
				"unable to get element %d." ), n - i, n );
			reduce_throw( rc );
		}

		g_assert( PEISFLIST( &p ) );

		reduce_get_list( rc, &p );

		hn = PEGETVAL( &p );
		PEPOINTRIGHT( hn, &p );

		if( --i < 0 )
			break;

		reduce_spine( rc, &p );
	}

	if( trace_flags & TRACE_OPERATOR ) {
		VipsBuf *buf = trace_push();

		trace_pelement( base );
		vips_buf_appendf( buf, " \"?\" %d ->\n", n );
	}

	PEPOINTLEFT( hn, out );

	if( trace_flags & TRACE_OPERATOR ) {
		trace_result( TRACE_OPERATOR, out );
		trace_pop();
	}
}

/* No args allowed error.
 */
static void
argserror( Reduce *rc,  PElement *a )
{
	VipsBuf buf;
	char txt[MAX_ERROR_FRAG];

	vips_buf_init_static( &buf, txt, MAX_ERROR_FRAG );
	itext_value_ev( rc, &buf, a );

	error_top( _( "No arguments allowed." ) );
	error_sub( _( "Object \"%s\" should have no arguments." ),
		vips_buf_all( &buf ) );
	reduce_throw( rc );
}

#ifdef WHNF_DEBUG
/* Test for PElement is in weak head-normal form.
 */
static gboolean
is_WHNF( PElement *out )
{
	PElement spine;
	int i;
	HeapNode *hn;
	Symbol *sym;
	Compile *compile;
	int na;

	/* Might be a base type ...
	 */
	if( PEISREAL( out ) || 
		PEISCOMPLEX( out ) || PEISNUM( out ) || PEISCHAR( out ) ||
		PEISBOOL( out ) || PEISTAG( out ) || PEISIMAGE( out ) ||
		PEISLIST( out ) || PEISCLASS( out ) || PEISSYMREF( out ) ||
		PEISCOMPILEREF( out ) || PEISNOVAL( out ) )
		return( TRUE );

	/* Must be a function or generator ... loop down the spine, counting 
	 * args.
	 */
	for( spine = *out, i = 0; PEGETTYPE( &spine ) == ELEMENT_NODE; i++ ) {
		hn = PEGETVAL( &spine );

		if( hn->type != TAG_APPL )
			break;

		PEPOINTLEFT( PEGETVAL( &spine ), &spine );
	}

	if( PEISBINOP( &spine ) ) {
		if( i > 1 )
			return( FALSE );
	}
	else if( PEISUNOP( &spine ) ) {
		if( i > 0 )
			return( FALSE );
	}
	else if( PEISCOMB( &spine ) ) {
		if( i > nargs[(int) PEGETCOMB( &spine )] - 1 )
			return( FALSE );
	}
	else if( PEISCONSTRUCTOR( &spine ) ) {
		compile = PEGETCOMPILE( &spine );
		na = compile->nparam + compile->nsecret;

		if( i > na ) {
			printf( "constructor %s with %d args ", 
				symbol_name( sym ), i );
			printf( "should have %d args\n", compile->nparam ); 
			return( FALSE );
		}
	}
	else if( PEISSYMBOL( &spine ) ) {
		/* If it's a VIPS or a builtin with too few args, it's OK.
		 */
		sym = SYMBOL( PEGETVAL( &spine ) );

		if( sym->type == SYM_EXTERNAL ) {
			if( i < sym->fn_nargs )
				return( TRUE );
		}
		else if( sym->type == SYM_BUILTIN ) {
			if( i < sym->builtin->nargs )
				return( TRUE );
		}

		/* Nope ... should have been reduced.
		 */
		return( FALSE );
	}
	else {
		return( FALSE );
	}

	return( TRUE );
}
#endif /*WHNF_DEBUG*/

/* Main reduction machine loop.
 */
void
reduce_spine( Reduce *rc, PElement *out )
{
	Heap *heap = rc->heap;
	PElement np;

	/* Check for possible C stack overflow ... can't go over 2M on most
	 * systems if we're using (or any of our libs are using) threads.
	 */
	if( (char *) main_c_stack_base - (char *) &rc > 2000000 ) {
		error_top( _( "Overflow error." ) );
		error_sub( _( "C stack overflow. Expression too complex." ) );
		reduce_throw( rc );
	}

	/* Point node pointer at reduction start.
	 */
	np = *out;

	/* Start a new frame.
	 */
	RSPUSHFRAME( rc, out ); 

reduce_start:
	reduce_total_recomputations += 1;
	if( (reduce_total_recomputations % 100000) == 0 ) {
		if( progress_update_expr( reduce_current_expr ) ) {
			error_top( _( "Cancelled." ) );
			error_sub( _( "Evaluation cancelled." ) );
			reduce_throw( rc );
		}
	}

	switch( PEGETTYPE( &np ) ) {
	case ELEMENT_CHAR:
	case ELEMENT_BOOL:
	case ELEMENT_ELIST:
	case ELEMENT_TAG:
	case ELEMENT_SYMREF:
	case ELEMENT_COMPILEREF:
	case ELEMENT_MANAGED:
		/* Base type .. no more reduction needed.
		 */

		/* Should have no args.
		 */
		if( RSFRAMESIZE( rc ) != 0 ) 
			argserror( rc, &np );

		break;

	case ELEMENT_CONSTRUCTOR:
	{
		Compile *compile;
		HeapNode **arg;
		PElement rhs1;
		int na;

		/* Class constructor.
		 */
		compile = PEGETCOMPILE( &np );
		g_assert( is_class( compile ) );

		/* Class args ... real params, secret params.
		 */
		na = compile->nparam + compile->nsecret;

		/* Get args.
		 */
		if( !RSCHECKARGS( rc, na ) ) 
			break;
		arg = &RSGET( rc, na - 1 );

		if( na == 0 ) {
			/* Zero args ... just construct on top of the current
			 * node pointer.
			 */
			action_proc_construct( rc, compile, arg, &np );
			goto reduce_start;
		}

		/* Overwrite RHS of arg[0], make LHS into COMB_I.
		 */
		PEPOINTRIGHT( arg[0], &rhs1 ); 
		action_proc_construct( rc, compile, arg, &rhs1 );
		PPUTLEFT( arg[0], ELEMENT_COMB, COMB_I );

		RSPOP( rc, na );
		if( RSFRAMEEMPTY( rc ) )
			np = RSGETWB( rc ); 
		else
			PEPOINTLEFT( RSGET( rc, 0 ), &np );
		PEPUTP( &np, 
			GETRT( arg[0] ), GETRIGHT( arg[0] ) );

		goto reduce_start;
	}

	case ELEMENT_SYMBOL:
	{
		Symbol *sym = PEGETSYMBOL( &np );

		g_assert( sym );

		switch( sym->type ) {
		case SYM_VALUE:
		{
			Compile *compile = sym->expr->compile;

			/* Make sure it's clean ... we can get 
			 * links to dirty syms through dynamic dependencies.
			 */
			if( sym->dirty ) {
				error_top( _( "No value." ) );
				error_sub( _( "Symbol \"%s\" has no value" ), 
					symbol_name( sym ) );
				reduce_throw( rc );
			}

			/* We copy code, but link to values. We have to take a
			 * fresh copy of code as (together with any args our
			 * context might supply) it will expand to a value,
			 * which we might then edit in a row. We want to make 
			 * sure any edits do not zap the original code.
 			 */
			if( compile->nparam + compile->nsecret == 0 ) {
				/* Make sure the value has copied to the main
				 * heap.
				 */
				if( PEISNOVAL( &sym->expr->root ) ) {
					gboolean res;

					res = reduce_regenerate( sym->expr, 
						&sym->expr->root );
					expr_new_value( sym->expr );

					if( !res ) 
						reduce_throw( rc );
				}

				/* Link to this sym's value.
				 */
				PEPUTPE( &np, &sym->expr->root );
			}
			else 
				/* Copy compiled code from the private compile
				 * heap.
				 */
				if( !heap_copy( rc->heap, compile, &np ) )
					reduce_throw( rc );

			goto reduce_start;
		}

		case SYM_PARAM:
			/* All params should be taken out by var abstract.
			 */
			g_assert( FALSE );
			break;

		case SYM_EXTERNAL:
		{
			HeapNode **arg;
			int na;

			/* A VIPS function.
			 */
			na = sym->fn_nargs;

			/* Get args. 
			 */
			if( !RSCHECKARGS( rc, na ) ) 
				/* Not enough ... function result. 
				 */
				break;

			/* Run strictly.
			 */
			arg = &RSGET( rc, na - 1 );

			action_dispatch( rc, NULL, reduce_spine,
				-1, sym->function->name, FALSE,
				(ActionFn) vips_run, na, arg, 
				sym->function );

			/* Find output element.
			 */
			RSPOP( rc, na );

			if( RSFRAMEEMPTY( rc ) )
				np = RSGETWB( rc ); 
			else
				PEPOINTLEFT( RSGET( rc, 0 ), &np );

			/* Write to node above.
			 */
			PEPUTP( &np, 
				GETRT( arg[0] ), GETRIGHT( arg[0] ) );

			goto reduce_start;
		}

		case SYM_BUILTIN:
		{
			HeapNode **arg;
			int na;

			/* A builtin function.
			 */
			na = sym->builtin->nargs;

			/* Get args. 
			 */
			if( !RSCHECKARGS( rc, na ) ) 
				/* Not enough ... function result. 
				 */
				break;

			/* Run strictly.
			 */
			arg = &RSGET( rc, na - 1 );

			action_dispatch( rc, NULL, reduce_spine,
				-1, sym->builtin->name, sym->builtin->override,
				(ActionFn) builtin_run, 
				na, arg, sym->builtin );

			/* Find output element.
			 */
			RSPOP( rc, na );

			if( RSFRAMEEMPTY( rc ) )
				np = RSGETWB( rc ); 
			else
				PEPOINTLEFT( RSGET( rc, 0 ), &np );

			/* Write to node above.
			 */
			PEPUTP( &np, 
				GETRT( arg[0] ), GETRIGHT( arg[0] ) );

			goto reduce_start;
		}

		case SYM_ZOMBIE:
		{
			Symbol *new_sym;

			/* Could be defined on an enclosing scope. Search
			 * outwards for a definition.
			 */
			if( !(new_sym = compile_resolve_top( sym )) ) {
				symbol_not_defined( sym );
				reduce_throw( rc );
			}

			/* Zap linked symbol into graph.
			 */
			PEPUTP( &np, ELEMENT_SYMBOL, new_sym );

			goto reduce_start;
		}

		case SYM_ROOT:
		case SYM_WORKSPACE:
		case SYM_WORKSPACEGROUP:
			/* Becomes a symref ... base type.
			 */
			PEPUTP( &np, ELEMENT_SYMREF, sym );

			/* Should have no args.
			 */
			if( RSFRAMESIZE( rc ) != 0 ) 
				argserror( rc, &np );

			break;

		default:
			g_assert( FALSE );
		}

		break;
	}

	case ELEMENT_NODE:
	{
		HeapNode *hn;

		/* Get the node that np points to.
		 */
		hn = PEGETVAL( &np );

		switch( hn->type ) {
		case TAG_CONS:
		case TAG_DOUBLE:
		case TAG_COMPLEX:
		case TAG_CLASS:
			/* Base type ... reduction all done! We don't test
			 * that class's superclasses are base, as they aren't
			 * always for non-top-level base types ... see 
			 * reduce_pelement().
			 */

			/* Should have no args.
			 */
			if( RSFRAMESIZE( rc ) != 0 )
				argserror( rc, &np );

			break;

		case TAG_APPL:
			/* Function application ... push this node and loop
			 * down the LHS looking for a combinator.
			 */

			/* Push this node.
			 */
			RSPUSH( rc, hn );

			/* Move down left branch.
			 */
			PEPOINTLEFT( hn, &np );

			goto reduce_start;

		case TAG_GEN:
		{
			double d1;
			double d2;
			double d3 = 0.0;	/* keeps gcc happy */
			gboolean limit;
			HeapNode *hn1, *hn2;

			/* Extract next, step, final.
			 */
			d1 = GETLEFT( hn )->body.num;
			d2 = GETLEFT( GETRIGHT( hn ) )->body.num;
			limit = GETRT( GETRIGHT( hn ) ) != ELEMENT_ELIST;
			if( limit )
				d3 = GETRIGHT( GETRIGHT( hn ) )->body.num;

			if( trace_flags & TRACE_OPERATOR ) {
				VipsBuf *buf = trace_push();

				if( limit )
					vips_buf_appendf( buf, 
						"generator %g %g %g ->\n",
						d1, d2, d3 );
				else
					vips_buf_appendf( buf, 
						"generator %g %g ->\n",
						d1, d2 );
			}

			/* At end?
			 */
			if( GETRT( GETRIGHT( hn ) ) != ELEMENT_ELIST &&
				((d2 > 0 && d1 > d3) || 
					(d2 < 0 && d1 < d3)) ) {
				/* Make I node for end.
				 */
				hn->type = TAG_APPL;
				PPUT( hn, 
					ELEMENT_COMB, COMB_I,
					ELEMENT_ELIST, NULL ); 

				/* Write back to node above.
				 */
				PEPUTP( &np, ELEMENT_ELIST, NULL );

				if( trace_flags & TRACE_OPERATOR ) {
					trace_result( TRACE_OPERATOR, &np );
					trace_pop();
				}

				/* All done!
				 */
				break;
			}

			/* Not at end, or no final. Generate new gen node.
			 */
			if( NEWNODE( heap, hn1 ) )
				reduce_throw( rc );
			*hn1 = *hn;

			/* Change hn into CONS node.
			 */
			hn->type = TAG_CONS;
			PPUTRIGHT( hn, ELEMENT_NODE, hn1 ); 

			/* Generate new number.
			 */
			if( NEWNODE( heap, hn2 ) )
				reduce_throw( rc );
			hn2->type = TAG_DOUBLE;
			hn2->body.num = d1 + d2;
			PPUTLEFT( hn1, 
				ELEMENT_NODE, hn2 ); 

			if( trace_flags & TRACE_OPERATOR ) {
				trace_result( TRACE_OPERATOR, &np );
				trace_pop();
			}

			/* And loop!
			 */
			goto reduce_start;
		}

		case TAG_FILE:
		{
			Managedfile *managedfile = MANAGEDFILE( GETLEFT( hn ) );
			int ch = managedfile_getc( managedfile );

			/* -1 means error, 0 means EOF.
			 */
			if( ch == -1 )
				reduce_throw( rc );
			else if( ch == 0 ) {
				/* Turn us into [].
				 */
				hn->type = TAG_APPL;
				PPUT( hn, 
					ELEMENT_COMB, COMB_I,
					ELEMENT_ELIST, NULL ); 
			}
			else {
				HeapNode *hn1;

				/* Not at end ... make another CONS.
				 */
				if( NEWNODE( heap, hn1 ) )
					reduce_throw( rc );
				*hn1 = *hn;
				hn->type = TAG_CONS;
				PPUT( hn, 
					ELEMENT_CHAR, GUINT_TO_POINTER( ch ), 
					ELEMENT_NODE, hn1 );
			}

			/* Loop again with new np.
			 */
			goto reduce_start;
		}

		case TAG_FREE:
			g_assert( FALSE );

		default:
			g_assert( FALSE );
		}

		break;
	}

	case ELEMENT_COMB:
	{
		CombinatorType comb = PEGETCOMB( &np );
		HeapNode *hn1, *hn2;
		HeapNode **arg;
		int na;

		na = nargs[(int) comb];

		/* Get args. 
		 */
		if( !RSCHECKARGS( rc, na ) ) 
			/* Not enough ... function result. 
			 */
			break;

		/* Extract args.
		 */
		arg = &RSGET( rc, na - 1 );

		switch( comb ) {
		case COMB_S:
			/* Rewrite graph for S a b c => (a c) (b c).
			 */

			/* Make (b c) appl node.
			 */
			if( NEWNODE( heap, hn1 ) )
				reduce_throw( rc );
			*hn1 = *arg[0];
			PPUTLEFT( hn1, 
				GETRT( arg[1] ), GETRIGHT( arg[1] ) ); 
			PPUTRIGHT( arg[0], 
				ELEMENT_NODE, hn1 );

			/* Make (a c) appl node.
			 */
			if( NEWNODE( heap, hn2 ) )
				reduce_throw( rc );
			*hn2 = *hn1;
			PPUTLEFT( hn2, 
				 GETRT( arg[2] ), GETRIGHT( arg[2] ) );
			PPUTLEFT( arg[0], 
				ELEMENT_NODE, hn2 ); 

			/* End of S ... now pop three, push 1 and loop.
			 */
			RSPOP( rc, 2 );
			PEPOINTLEFT( arg[0], &np );
			goto reduce_start;

		case COMB_SL:
			/* Rewrite graph for Sl a b c => (a c) b.
			 */

			/* Make (a c) appl node.
			 */
			if( NEWNODE( heap, hn1 ) )
				reduce_throw( rc );
			*hn1 = *arg[0];
			PPUTLEFT( hn1, 
				GETRT( arg[2] ), GETRIGHT( arg[2] ) );
			PPUT( arg[0], 
				ELEMENT_NODE, hn1,
				GETRT( arg[1] ), GETRIGHT( arg[1] ) );

			/* End of SL ... now pop three, push 1 and loop.
			 */
			RSPOP( rc, 2 );
			PEPOINTLEFT( arg[0], &np );
			goto reduce_start;

		case COMB_SR:
			/* Rewrite graph for Sr a b c => a (b c).
			 */

			/* Make (b c) appl node.
			 */
			if( NEWNODE( heap, hn1 ) )
				reduce_throw( rc );
			*hn1 = *arg[0];
			PPUTLEFT( hn1, 
				GETRT( arg[1] ), GETRIGHT( arg[1] ) );
			PPUT( arg[0],
				GETRT( arg[2] ), GETRIGHT( arg[2] ),
				ELEMENT_NODE, hn1 );

			/* End of SR ... now pop three, push 1 and loop.
			 */
			RSPOP( rc, 2 );
			PEPOINTLEFT( arg[0], &np );
			goto reduce_start;

		case COMB_I:
			/* No action necessary.
			 */
			break;

		case COMB_K:
			/* Make I node. 
			 */
			PPUT( arg[0], 
				ELEMENT_COMB, COMB_I,
				GETRT( arg[1] ), GETRIGHT( arg[1] ) );

			break;

		case COMB_GEN:
		{
			double d1;
			double d2 = 0.0;	/* Don't need to init, but */
			double d3 = 0.0;	/* keeps gcc happy */
			PElement rhs1, rhs2, rhs3;

			PEPOINTRIGHT( arg[2], &rhs1 );
			PEPOINTRIGHT( arg[1], &rhs2 );
			PEPOINTRIGHT( arg[0], &rhs3 );
			reduce_spine_strict( rc, &rhs1 );
			reduce_spine_strict( rc, &rhs2 );
			reduce_spine_strict( rc, &rhs3 );

			/* May have done ourselves in the process.
			 */
			if( arg[0]->type != TAG_APPL )
				break;

			/* Typecheck.
			 */
			if( !PEISREAL( &rhs1 ) ) 
				reduce_error_typecheck( rc, &rhs1, 
					_( "List generator" ), "real" );
			d1 = PEGETREAL( &rhs1 );

			if( !PEISELIST( &rhs2 ) && !PEISREAL( &rhs2 ) ) 
				reduce_error_typecheck( rc, &rhs2, 
					_( "List generator" ), "real" );
			if( PEISREAL( &rhs2 ) )
				d2 = PEGETREAL( &rhs2 ); 

			if( !PEISELIST( &rhs3 ) && !PEISREAL( &rhs3 ) ) 
				reduce_error_typecheck( rc, &rhs3, 
					_( "List generator" ), "real" );
			if( PEISREAL( &rhs3 ) )
				d3 = PEGETREAL( &rhs3 ); 

			if( trace_flags & TRACE_OPERATOR ) {
				VipsBuf *buf = trace_push();

				vips_buf_appends( buf, "generator constructor " );
				trace_args( arg, 3 );
			}

			/* If next is missing, set default.
			 */
			if( PEISREAL( &rhs2 ) ) 
				/* Next is there, calculate step.
				 */
				d2 = d2 - d1;
			else {
				/* If final is missing, default is 1.
				 */
				if( PEISELIST( &rhs3 ) ) 
					d2 = 1;
				else {
					/* Final is there, choose 1 or -1.
					 */
					if( d1 < d3 )
						d2 = 1;
					else
						d2 = -1;
				}
			}

			/* Make node for pairing next and final fields.
			 */
			if( NEWNODE( heap, hn1 ) )
				reduce_throw( rc );
			hn1->type = TAG_COMPLEX;
			PPUT( hn1, 
				GETRT( arg[1] ), GETRIGHT( arg[1] ),
				GETRT( arg[0] ), GETRIGHT( arg[0] ) );

			/* Link to old root, make gen node.
			 */
			arg[0]->type = TAG_GEN;
			PPUT( arg[0],
				GETRT( arg[2] ), GETRIGHT( arg[2] ),
				ELEMENT_NODE, hn1 );

			/* Make step node.
			 */
			if( NEWNODE( heap, hn2 ) )
				reduce_throw( rc );
			hn2->type = TAG_DOUBLE;
			hn2->body.num = d2;
			PPUTLEFT( hn1,
				ELEMENT_NODE, hn2 );

			if( trace_flags & TRACE_OPERATOR ) {
				VipsBuf *buf = trace_current();

				vips_buf_appends( buf, "    " ); 
				trace_node( arg[0] );
				vips_buf_appends( buf, "\n" ); 

				trace_text( TRACE_OPERATOR, 
					"%s", vips_buf_all( buf ) ); 

				trace_pop();
			}

			/* Find output element.
			 */
			RSPOP( rc, 3 );
			if( RSFRAMEEMPTY( rc ) )
				np = RSGETWB( rc ); 
			else
				PEPOINTLEFT( RSGET( rc, 0 ), &np );

			/* Restart from there.
			 */
			goto reduce_start;
		}

		default:
			g_assert( FALSE );
		}

		/* Find output element.
		 */
		RSPOP( rc, na );
		if( RSFRAMEEMPTY( rc ) )
			np = RSGETWB( rc ); 
		else
			PEPOINTLEFT( RSGET( rc, 0 ), &np );

		/* Write to above node.
		 */
		PEPUTP( &np, 
			 GETRT( arg[0] ), GETRIGHT( arg[0] ) );

		/* Loop again with new np.
		 */
		goto reduce_start;
		/*NOTREACHED*/
	}

	case ELEMENT_BINOP:
	{
		BinOp bop = PEGETBINOP( &np );
		HeapNode **arg;
		Compile *compile;
		PElement rhs1, rhs2;

		/* Three args to binops ... first is the Compile that built us
		 * (for error messages), other two are actual args.
		 */
		if( !RSCHECKARGS( rc, 3 ) )
			/* Not enough ... function result.
			 */
			break;

		/* Extract args.
		 */
		arg = &RSGET( rc, 2 );
		compile = COMPILE( GETRIGHT( arg[2] ) );

		/* CONS is very, very lazy ... more like a combinator.
		 */
		if( bop == BI_CONS ) {
			PEPOINTRIGHT( arg[1], &rhs1 );

			if( trace_flags & TRACE_OPERATOR ) {
				trace_push();

				PEPOINTRIGHT( arg[0], &rhs2 );
				trace_binop( compile, &rhs1, bop, &rhs2 );
			}

			arg[0]->type = TAG_CONS;
			PPUTLEFT( arg[0], 
				PEGETTYPE( &rhs1 ), PEGETVAL( &rhs1 ) );

			if( trace_flags & TRACE_OPERATOR ) {
				VipsBuf *buf = trace_current();

				vips_buf_appends( buf, "    " ); 
				trace_node( arg[0] );
				vips_buf_appends( buf, "\n" ); 

				trace_text( TRACE_OPERATOR, 
					"%s", vips_buf_all( buf ) );

				trace_pop();
			}

			RSPOP( rc, 3 );

			break;
		}

		action_proc_bop( rc, compile, bop, arg );

		/* Find output element.
		 */
		RSPOP( rc, 3 );

		if( RSFRAMEEMPTY( rc ) )
			np = RSGETWB( rc ); 
		else
			PEPOINTLEFT( RSGET( rc, 0 ), &np );

		/* Write to node above.
		 */
		PEPUTP( &np, 
			GETRT( arg[0] ), GETRIGHT( arg[0] ) );

		/* Loop again with new np.
		 */
		goto reduce_start;
	}

	case ELEMENT_UNOP:
	{
		HeapNode **arg;
		Compile *compile;

		/* Some unary operator. First arg is the compile that built
		 * us, 2nd is the actual arg that might need reducing.
		 */
		if( !RSCHECKARGS( rc, 2 ) )
			/* Not enough ... function result.
			 */
			break;

		/* Extract arg.
		 */
		arg = &RSGET( rc, 1 );
		compile = COMPILE( GETRIGHT( arg[1] ) );

		action_dispatch( rc, compile, reduce_spine,
			PEGETUNOP( &np ), OPERATOR_NAME( PEGETUNOP( &np ) ),
			TRUE, (ActionFn) action_proc_uop, 1, arg, NULL );

		/* Find output element.
		 */
		RSPOP( rc, 2 );
		if( RSFRAMEEMPTY( rc ) )
			np = RSGETWB( rc ); 
		else
			PEPOINTLEFT( RSGET( rc, 0 ), &np );

		/* Write to above node.
		 */
		PEPUTP( &np, 
			 GETRT( arg[0] ), GETRIGHT( arg[0] ) );

		/* Loop again with new np.
		 */
		goto reduce_start;
	}

	case ELEMENT_NOVAL:
		break;

	default:
		g_assert( FALSE );
	}

	/* Unwind stack, restore frame pointer.
	 */
	RSPOPFRAME( rc ); 

#ifdef WHNF_DEBUG
	/* Should now be in WHNF ... test!
	 */
	if( !is_WHNF( out ) ) {
		VipsBuf buf;
		char txt[1000];

		vips_buf_init_static( &buf, txt, 1000 );
		graph_pelement( heap, &buf, out, TRUE );
		printf( "*** internal error:\n" );
		printf( "result of reduce_spine not in WHNF: " );
		printf( "%s\n", vips_buf_all( &buf ) );
		reduce_throw( rc );
	}
#endif /*WHNF_DEBUG*/
}

/* Strict reduction ... fully eval all lists etc.
 */
void
reduce_spine_strict( Reduce *rc, PElement *np )
{
	PElement rhs, lhs;

	/* Make sure this element is reduced.
	 */
	reduce_spine( rc, np );

	/* If it's a non-empty list, may need to reduce inside. Not managed
	 * strings though, we can leave them unevaluated.
	 */
	if( PEISFLIST( np ) && !PEISMANAGEDSTRING( np ) ) {
		/* Recurse for head and tail.
		 */
		HeapNode *hn = PEGETVAL( np );

		PEPOINTLEFT( hn, &lhs );
		PEPOINTRIGHT( hn, &rhs );
		reduce_spine_strict( rc, &lhs );
		reduce_spine_strict( rc, &rhs );
	}
}

/* Free a Reduce.
 */
void
reduce_destroy( Reduce *rc )
{
	heap_unregister_reduce( rc->heap, rc );
	UNREF( rc->heap );
	IM_FREE( rc );
}

/* Max cells function for main reduce engine. Read from Preferences, and scale
 * by the number of workspaces we have open.
 */
static int
reduce_heap_max_fn( Heap *heap )
{
	return( workspace_number() * MAX_HEAPSIZE );
}

/* Build a Reduce.
 */
Reduce *
reduce_new( void )
{
	/* Initial heap size. Big enough that we won't need to grow just
	 * loading prefs and standard stuff.
	 */
	const int stsz = 100000;

	/* Heap increment..
	 */
	const int incr = 2000;

	Reduce *rc = INEW( NULL, Reduce );

	if( !rc )
		return( NULL );
	rc->sp = 0;
	rc->fsp = 0;
	rc->heap = NULL;
	rc->running = 0;

	rc->heap = heap_new( NULL, reduce_heap_max_fn, stsz, incr );
	g_object_ref( G_OBJECT( rc->heap ) );
	iobject_sink( IOBJECT( rc->heap ) );
	heap_register_reduce( rc->heap, rc );
	iobject_set( IOBJECT( rc->heap ), "reduce-heap", NULL );

	return( rc );
}

/* Reduce a PElement to a base type. Return TRUE/FALSE, no longjmp.
 */
gboolean
reduce_pelement( Reduce *rc, ReduceFunction fn, PElement *out )
{
	gboolean res = TRUE;

	REDUCE_CATCH_START( FALSE );
	fn( reduce_context, out );
	REDUCE_CATCH_STOP;

	return( res );
}

/* Make sure a symbol's value is registered with the main GC.
 */
void
reduce_register( Symbol *sym )
{
	Reduce *rc = reduce_context;
	Heap *heap = rc->heap;

	heap_register_element( heap, &sym->base );
}

/* Make sure a symbol's value is not registered with the main GC.
 */
void
reduce_unregister( Symbol *sym )
{
	Reduce *rc = reduce_context;
	Heap *heap = rc->heap;

	heap_unregister_element( heap, &sym->base );
}

/* Copy and evaluate compiled code into element pointed to by out.
 */
gboolean
reduce_regenerate( Expr *expr, PElement *out )
{
	Reduce *rc = reduce_context;
	Heap *heap = rc->heap;

	/* Clear any run state from old expr value.
	 */
	expr_error_clear( expr );
	if( slist_map( expr->dynamic_links, 
		(SListMapFn) link_expr_destroy, NULL ) )
		return( FALSE );

	/* Copy new code in.
	 */
	if( !heap_copy( heap, expr->compile, out ) ) {
		expr_error_set( expr );
		return( FALSE );
	}

#ifdef DEBUG_REGEN
{
	VipsBuf buf;
	char txt[1024];

	vips_buf_init_static( &buf, txt, 1024 );
	graph_pelement( heap, &buf, out, TRUE );
	printf( "reduce_regenerate: reducing " );
	expr_name_print( expr );
	printf( "graph: %s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG_REGEN*/

	reduce_current_expr = expr;
	if( !reduce_pelement( rc, reduce_spine, out ) ) {
		reduce_current_expr = NULL;
		expr_error_set( expr );
		(void) heap_gc( heap );
		return( FALSE );
	}
	reduce_current_expr = NULL;

#ifdef DEBUG_REGEN
{
	VipsBuf buf;
	char txt[1024];

	/* Force immediate GC to pick up any stray pointers.
	 */
	if( !heap_gc( heap ) ) {
		expr_error_set( expr );
		return( FALSE );
	}

	vips_buf_init_static( &buf, txt, 1024 );
	graph_pelement( heap, &buf, out, TRUE );
	printf( "reduce_regenerate: reduced " );
	expr_name_print( expr );
	printf( " to: %s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG_REGEN*/

	return( TRUE );
}

/* Regenerate an (expr this) pair.
 */
gboolean
reduce_regenerate_member( Expr *expr, PElement *ths, PElement *out )
{
	Reduce *rc = reduce_context;
	Heap *heap = rc->heap;

	PElement e;
	HeapNode *apl;

	/* New (NULL this) pair.
	 */
	if( NEWNODE( heap, apl ) ) {
		expr_error_set( expr );
		return( FALSE );
	}
	apl->type = TAG_APPL;
	PPUT( apl, ELEMENT_NOVAL, (void *) 10, 
		PEGETTYPE( ths ), PEGETVAL( ths ) ); 
	PEPUTP( out, ELEMENT_NODE, apl );

	/* Link code to node.
	 */
	PEPOINTLEFT( apl, &e );
	if( !reduce_regenerate( expr, &e ) ) 
		return( FALSE );

#ifdef DEBUG_REGEN_MEMBER
{
	VipsBuf buf;
	char txt[1024];

	vips_buf_init_static( &buf, txt, 1024 );
	graph_pelement( heap, &buf, out, TRUE );
	printf( "reduce_regenerate_member: " );
	expr_name_print( expr );
	printf( " new code: %s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG_REGEN_MEMBER*/

	/* Do initial reduction.
	 */
	if( !reduce_pelement( rc, reduce_spine, out ) ) {
		/* Failure! Junk the half-made value. 
		 */
		expr_error_set( expr );
		(void) heap_gc( heap );
		return( FALSE );
	}

	/* Special case: if this is a "super" row, we need to rebuild the
	 * class.
	 */
	if( is_super( expr->compile->sym ) ) {
		Compile *parent = compile_get_parent( expr->compile );
		PElement instance;

		PEPOINTE( &instance, &expr->row->scol->base );

		if( !class_new_super( heap, parent, ths, &instance ) )
			return( FALSE );
	}

	return( TRUE );
}
