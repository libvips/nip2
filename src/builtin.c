/* Run builtin functions ... sin/error etc.
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

#ifdef HAVE_GSL
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_errno.h>
#endif /*HAVE_GSL*/

/* Trace builtin calls.
#define DEBUG
 */

/* Spot something that might be an arg to sin/cos/tan etc.
 */
static gboolean
ismatharg( Reduce *rc, PElement *base )
{
	return( PEISIMAGE( base ) || PEISREAL( base ) || PEISCOMPLEX( base ) );
}

/* Spot something that might be an arg to re/im etc.
 */
static gboolean
iscomplexarg( Reduce *rc, PElement *base )
{
	return( PEISIMAGE( base ) || PEISCOMPLEX( base ) );
}

/* Spot anything.
 */
static gboolean isany( Reduce *rc, PElement *base ) { return( TRUE ); }

/* Other PEIS as functions.
 */
static gboolean pe_is_image( Reduce *rc, PElement *base ) 
	{ return( PEISIMAGE( base ) ); }
static gboolean pe_is_real( Reduce *rc, PElement *base ) 
	{ return( PEISREAL( base ) ); }
static gboolean pe_is_complex( Reduce *rc, PElement *base ) 
	{ return( PEISCOMPLEX( base ) ); }
static gboolean pe_is_bool( Reduce *rc, PElement *base ) 
	{ return( PEISBOOL( base ) ); }
static gboolean pe_is_char( Reduce *rc, PElement *base ) 
	{ return( PEISCHAR( base ) ); }
static gboolean pe_is_list( Reduce *rc, PElement *base ) 
	{ return( PEISLIST( base ) ); }
static gboolean pe_is_flist( Reduce *rc, PElement *base ) 
	{ return( PEISFLIST( base ) ); }
static gboolean pe_is_class( Reduce *rc, PElement *base )
        { return( PEISCLASS( base ) ); }
static gboolean pe_is_gobject( Reduce *rc, PElement *base )
        { return( PEISMANAGEDGOBJECT( base ) ); }

/* The types we might want to spot for builtins.
 */
static BuiltinTypeSpot vimage_spot = { "vips_image", pe_is_image };
static BuiltinTypeSpot real_spot = { "real", pe_is_real };
static BuiltinTypeSpot bool_spot = { "bool", pe_is_bool };
static BuiltinTypeSpot complex_spot = { "complex|image", iscomplexarg };
static BuiltinTypeSpot flist_spot = { "non-empty list", pe_is_flist };
static BuiltinTypeSpot realvec_spot = { "[real]", reduce_is_realvec };
static BuiltinTypeSpot matrix_spot = { "[[real]]", reduce_is_matrix };
static BuiltinTypeSpot string_spot = { "[char]", reduce_is_finitestring };
static BuiltinTypeSpot list_spot = { "[*]", reduce_is_list };
static BuiltinTypeSpot math_spot = { "image|real|complex", ismatharg };
static BuiltinTypeSpot instance_spot = { "class instance", pe_is_class };
static BuiltinTypeSpot any_spot = { "any", isany };
static BuiltinTypeSpot gobject_spot = { "GObject", pe_is_gobject };

/* Args for "_".
 */
static BuiltinTypeSpot *underscore_args[] = {
        &string_spot,
        &any_spot
};

/* Do a _ call. Args already spotted.
 */
static void
apply_underscore_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
        PElement rhs;
	char text[MAX_STRSIZE];

	PEPOINTRIGHT( arg[0], &rhs );
	(void) reduce_get_string( rc, &rhs, text, MAX_STRSIZE );

	/* Pump though gettext.
	 */
	if( !heap_managedstring_new( rc->heap, _( text ), out ) )
		reduce_throw( rc );
}

/* Args for "has_member".
 */
static BuiltinTypeSpot *has_member_args[] = {
        &string_spot,
        &any_spot
};

/* Do a has_member call. Args already spotted.
 */
static void
apply_has_member_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
        PElement rhs;
	char mname[MAX_STRSIZE];
        PElement member;

	PEPOINTRIGHT( arg[1], &rhs );
	(void) reduce_get_string( rc, &rhs, mname, MAX_STRSIZE );
        PEPOINTRIGHT( arg[0], &rhs );
	PEPUTP( out, ELEMENT_BOOL, 
		class_get_member( &rhs, mname, NULL, &member ) );
}

/* Args for "is_instanceof".
 */
static BuiltinTypeSpot *is_instanceof_args[] = {
        &string_spot,
        &any_spot
};

/* Do an is_instance call. Args already spotted.
 */
static void
apply_is_instanceof_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
        PElement rhs;
	char kname[MAX_STRSIZE];

	PEPOINTRIGHT( arg[1], &rhs );
	(void) reduce_get_string( rc, &rhs, kname, MAX_STRSIZE );
        PEPOINTRIGHT( arg[0], &rhs );
	PEPUTP( out, ELEMENT_BOOL, reduce_is_instanceof( rc, kname, &rhs ) );
}

/* Args for builtin on complex.
 */
static BuiltinTypeSpot *complex_args[] = {
        &complex_spot
};

/* Do a complex op. Args already spotted.
 */
static void
apply_complex_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
        PElement rhs;

	PEPOINTRIGHT( arg[0], &rhs );

	if( PEISIMAGE( &rhs ) ) {
		if( strcmp( name, "re" ) == 0 ) 
			vips_spine( rc, "im_c2real", arg, out );
		else if( strcmp( name, "im" ) == 0 ) 
			vips_spine( rc, "im_c2imag", arg, out );
	}
	else if( PEISCOMPLEX( &rhs ) ) {
		if( strcmp( name, "re" ) == 0 ) {
			PEPUTP( out, 
				ELEMENT_NODE, GETLEFT( PEGETVAL( &rhs ) ) );
		}
		else if( strcmp( name, "im" ) == 0 ) {
			PEPUTP( out, 
				ELEMENT_NODE, GETRIGHT( PEGETVAL( &rhs ) ) );
		}
	}
	else
		error( "internal error #98743698437639487" );
}

/* Args for builtin on list.
 */
static BuiltinTypeSpot *flist_args[] = {
        &flist_spot
};

/* Do a list op. Args already spotted.
 */
static void
apply_list_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
        PElement rhs;
        PElement a;

	PEPOINTRIGHT( arg[0], &rhs );
	g_assert( PEISFLIST( &rhs ) );

	reduce_get_list( rc, &rhs );

	if( strcmp( name, "hd" ) == 0 ) {
		PEGETHD( &a, &rhs );
		PEPUTPE( out, &a );
	}
	else if( strcmp( name, "tl" ) == 0 ) {
		PEGETTL( &a, &rhs );
		PEPUTPE( out, &a );
	}
	else
		error( "internal error #098734953" );
}

/* "gammq"
 */
static BuiltinTypeSpot *gammq_args[] = { 
	&real_spot,
	&real_spot
};

static void
apply_gammq_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
	PElement rhs;
	double a, x, Q;

	PEPOINTRIGHT( arg[1], &rhs );
	a = PEGETREAL( &rhs );
	PEPOINTRIGHT( arg[0], &rhs );
	x = PEGETREAL( &rhs );

	if( a <= 0 || x < 0 ) {
		error_top( _( "Out of range." ) );
		error_sub( _( "gammq arguments must be a > 0, x >= 0." ) );
		reduce_throw( rc );
	}

#ifdef HAVE_GSL
	Q = gsl_sf_gamma_inc_Q( a, x );
#else /*!HAVE_GSL*/
	error_top( _( "Not available." ) );
	error_sub( _( "No GSL library available for gammq." ) );
	reduce_throw( rc );
#endif /*HAVE_GSL*/

	if( !heap_real_new( rc->heap, Q, out ) )
		reduce_throw( rc );
}

/* Args for "vips_image". 
 */
static BuiltinTypeSpot *image_args[] = { 
	&string_spot 
};

/* Do a image call.
 */
static void
apply_image_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
	Heap *heap = rc->heap;
	PElement rhs;
	char buf[FILENAME_MAX];
	char *fn;
	Imageinfo *ii;

	/* Get string. 
	 */
	PEPOINTRIGHT( arg[0], &rhs );
	(void) reduce_get_string( rc, &rhs, buf, FILENAME_MAX );

	/* Try to load image from given string.
	 */
	if( !(fn = path_find_file( PATH_SEARCH, buf )) )
		reduce_throw( rc );
	if( !(ii = imageinfo_new_input( 
		main_imageinfogroup, NULL, heap, fn )) ) {
		IM_FREE( fn );
		reduce_throw( rc );
	}
	IM_FREE( fn );

	PEPUTP( out, ELEMENT_MANAGED, ii );
	MANAGED_UNREF( ii );
}

/* Args for "read". 
 */
static BuiltinTypeSpot *read_args[] = { 
	&string_spot 
};

/* Do a read call.
 */
static void
apply_read_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
	PElement rhs;
	char buf[FILENAME_MAX];

	/* Get string. 
	 */
	PEPOINTRIGHT( arg[0], &rhs );
	(void) reduce_get_string( rc, &rhs, buf, FILENAME_MAX );

	if( !heap_file_new( rc->heap, buf, out ) )
		reduce_throw( rc );
}

/* Args for "math". 
 */
static BuiltinTypeSpot *math_args[] = { 
	&math_spot 
};

/* A math function ... name, number implementation, image implementation.
 */
typedef struct {
	const char *name;		/* ip name */
	double (*rfn)( double );	/* Number implementation */
	const char *ifn;		/* VIPS name */
} MathFn;

static double ip_sin( double a ) { return( sin( IM_RAD( a ) ) ); }
static double ip_cos( double a ) { return( cos( IM_RAD( a ) ) ); }
static double ip_tan( double a ) { return( tan( IM_RAD( a ) ) ); }
static double ip_asin( double a ) { return( IM_DEG( asin( a ) ) ); }
static double ip_acos( double a ) { return( IM_DEG( acos( a ) ) ); }
static double ip_atan( double a ) { return( IM_DEG( atan( a ) ) ); }
static double ip_exp10( double a ) { return( pow( 10.0, a ) ); }
static double ip_ceil( double a ) { return( ceil( a ) ); }
static double ip_floor( double a ) { return( floor( a ) ); }

/* Table of math functions ... number implementations, image implementations.
 */
static MathFn math_fn[] = {
	{ "sin", &ip_sin, "im_sintra" },
	{ "cos", &ip_cos, "im_costra" },
	{ "tan", &ip_tan, "im_tantra" },
	{ "asin", &ip_asin, "im_asintra" },
	{ "acos", &ip_acos, "im_acostra" },
	{ "atan", &ip_atan, "im_atantra" },
	{ "log", &log, "im_logtra" },
	{ "log10", &log10, "im_log10tra" },
	{ "exp", &exp, "im_exptra" },
	{ "exp10", &ip_exp10, "im_exp10tra" },
	{ "ceil", &ip_ceil, "im_ceil" },
	{ "floor", &ip_floor, "im_floor" }
};

/* Do a math function (eg. sin, cos, tan).
 */
static void
apply_math_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
	PElement rhs;
	int i;

	/* Find implementation.
	 */
	for( i = 0; i < IM_NUMBER( math_fn ); i++ )
		if( strcmp( name, math_fn[i].name ) == 0 )
			break;
	if( i == IM_NUMBER( math_fn ) )
		error( "internal error #928456936" );

	/* Get arg type ... real/complex/image
	 */
	PEPOINTRIGHT( arg[0], &rhs );
	if( PEISIMAGE( &rhs ) ) {
		/* Easy ... pass to VIPS.
		 */
		vips_spine( rc, math_fn[i].ifn, arg, out );
	}
	else if( PEISREAL( &rhs ) ) {
		double a = PEGETREAL( &rhs );
		double b = math_fn[i].rfn( a );

		if( !heap_real_new( rc->heap, b, out ) )
			reduce_throw( rc );
	}
	else if( PEISCOMPLEX( &rhs ) ) {
		error_top( _( "Not implemented." ) );
		error_sub( _( "Complex math ops not implemented." ) );
		reduce_throw( rc );
	}
	else
		error( "internal error #92870653" );
}

/* Args for "predicate". 
 */
static BuiltinTypeSpot *pred_args[] = { 
	&any_spot 
};

/* A predicate function ... name, implementation.
 */
typedef struct {
	const char *name;				/* ip name */
	gboolean (*fn)( Reduce *, PElement * );	/* Implementation */
} PredicateFn;

/* Table of predicate functions ... name and implementation.
 */
static PredicateFn predicate_fn[] = {
	{ "is_image", &pe_is_image },
	{ "is_bool", &pe_is_bool },
	{ "is_real", &pe_is_real },
	{ "is_char", &pe_is_char },
	{ "is_class", &pe_is_class },
	{ "is_list", &pe_is_list },
	{ "is_complex", &pe_is_complex }
};

/* Do a predicate function (eg. is_bool)
 */
static void
apply_pred_call( Reduce *rc, const char *name, HeapNode **arg, PElement *out )
{
	PElement rhs;
	gboolean res;
	int i;

	/* Find implementation.
	 */
	for( i = 0; i < IM_NUMBER( predicate_fn ); i++ )
		if( strcmp( name, predicate_fn[i].name ) == 0 )
			break;
	if( i == IM_NUMBER( predicate_fn ) )
		error( "internal error #928456936" );

	/* Call!
	 */
	PEPOINTRIGHT( arg[0], &rhs );
	res = predicate_fn[i].fn( rc, &rhs );
	PEPUTP( out, ELEMENT_BOOL, res );
}

/* Args for "error". 
 */
static BuiltinTypeSpot *error_args[] = { 
	&string_spot 
};

/* Do "error".
 */
static void
apply_error_call( Reduce *rc, const char *name, HeapNode **arg, PElement *out )
{
	char buf[MAX_STRSIZE];
	PElement rhs;

	/* Get string. 
	 */
	PEPOINTRIGHT( arg[0], &rhs );
	(void) reduce_get_string( rc, &rhs, buf, MAX_STRSIZE );

	error_top( _( "Macro error." ) );
	error_sub( "%s", buf );
	reduce_throw( rc );
}

/* Args for "search". 
 */
static BuiltinTypeSpot *search_args[] = { 
	&string_spot 
};

/* Do "search".
 */
static void
apply_search_call( Reduce *rc, const char *name, HeapNode **arg, PElement *out )
{
	char buf[MAX_STRSIZE];
	PElement rhs;
	char *fn;

	/* Get string. 
	 */
	PEPOINTRIGHT( arg[0], &rhs );
	(void) reduce_get_string( rc, &rhs, buf, MAX_STRSIZE );

	if( !(fn = path_find_file( PATH_SEARCH, buf )) )
		reduce_throw( rc );

	if( !heap_managedstring_new( rc->heap, fn, out ) ) {
		IM_FREE( fn );
		reduce_throw( rc );
	}
	IM_FREE( fn );
}

/* Args for "print". 
 */
static BuiltinTypeSpot *print_args[] = { 
	&any_spot 
};

/* Do "print".
 */
static void
apply_print_call( Reduce *rc, const char *name, HeapNode **arg, PElement *out )
{
	PElement rhs;
	VipsBuf buf;
	char txt[MAX_STRSIZE];

	vips_buf_init_static( &buf, txt, MAX_STRSIZE );
	PEPOINTRIGHT( arg[0], &rhs );
	itext_value_ev( rc, &buf, &rhs );

	if( !heap_managedstring_new( rc->heap, vips_buf_all( &buf ), out ) )
		reduce_throw( rc );
}

/* Args for "dir". 
 */
static BuiltinTypeSpot *dir_args[] = { 
	&any_spot 
};

static void *
dir_object_member( Symbol *sym, PElement *value, 
	Reduce *rc, PElement *list )
{
	PElement t;

	if( !heap_list_add( rc->heap, list, &t ) ||
		!heap_managedstring_new( rc->heap, IOBJECT( sym )->name, &t ) )
		reduce_throw( rc );
	(void) heap_list_next( list );

	return( NULL );
}

static void *
dir_object( Reduce *rc, PElement *list, PElement *instance, PElement *out )
{
	PElement p;

	/* p walks down the list as we build it, list stays pointing at the
	 * head ready to be written to out.
	 */
	p = *list;
	heap_list_init( &p );
	class_map( instance, (class_map_fn) dir_object_member, rc, &p );
	PEPUTPE( out, list );

	return( NULL );
}

static void *
dir_scope( Symbol *sym, Reduce *rc, PElement *list )
{
	PElement t;

	if( !heap_list_add( rc->heap, list, &t ) ||
		!heap_managedstring_new( rc->heap, IOBJECT( sym )->name, &t ) )
		reduce_throw( rc );
	(void) heap_list_next( list );

	return( NULL );
}

static void *
dir_gtype( GType type, void *a, void *b )
{
	Reduce *rc = (Reduce *) a;
	PElement *list = (PElement *) b;
	PElement t;

	if( !heap_list_add( rc->heap, list, &t ) ||
		!heap_real_new( rc->heap, type, &t ) )
		return( rc );
	(void) heap_list_next( list );

	return( NULL );
}

static void
dir_gobject( Reduce *rc, 
	GParamSpec **properties, guint n_properties, PElement *out )
{
	int i;
	PElement list;

	list = *out;
	heap_list_init( &list );

	for( i = 0; i < n_properties; i++ ) {
		PElement t;

		if( !heap_list_add( rc->heap, &list, &t ) ||
			!heap_managedstring_new( rc->heap, 
				properties[i]->name, &t ) )
			reduce_throw( rc );
		(void) heap_list_next( &list );
	}
}

/* Do "dir". 
 */
static void
apply_dir_call( Reduce *rc, const char *name, HeapNode **arg, PElement *out )
{
	PElement rhs;

        PEPOINTRIGHT( arg[0], &rhs );

	if( PEISCLASS( &rhs ) ) 
		/* This is more complex than it looks. We have to walk a class
		 * instance generating a list of member names, while not
		 * destroying the instance as we go, in the case that out will
		 * overwrite (rhs) arg[0].
		 */
		reduce_safe_pointer( rc, (reduce_safe_pointer_fn) dir_object,
			&rhs, out, NULL, NULL );
	else if( PEISSYMREF( &rhs ) ) {
		Symbol *sym = PEGETSYMREF( &rhs );

		if( is_scope( sym ) && sym->expr && sym->expr->compile ) {
			PElement list;

			list = *out;
			heap_list_init( &list );

			icontainer_map( ICONTAINER( sym->expr->compile ),
				(icontainer_map_fn) dir_scope, rc, &list );
		}
	}
	else if( PEISREAL( &rhs ) ) {
		/* Assume this is a gtype and try to get the children of that
		 * type.
		 */
		GType type = PEGETREAL( &rhs );
		PElement list;

		list = *out;
		heap_list_init( &list );

		if( !g_type_name( type ) ) {
			error_top( _( "No such type" ) );
			error_sub( _( "GType %u not found." ), 
				(unsigned int) type );
			reduce_throw( rc );
		}

		if( vips_type_map( type, dir_gtype, rc, &list ) ) 
			reduce_throw( rc );
	}
	else if( PEISMANAGEDGOBJECT( &rhs ) ) {
		guint n_properties;
		ManagedgobjectClass *class = 
			MANAGEDGOBJECT_GET_CLASS( PEGETMANAGEDGOBJECT( &rhs ) ); 
		GParamSpec **properties;

		properties = g_object_class_list_properties( 
			G_OBJECT_CLASS( class ), &n_properties );
		dir_gobject( rc, properties, n_properties, out );
		g_free( properties);
	}
	else 
		/* Just [], ie. no names possible.
		 */
		heap_list_init( out );
}

/* Args for "expand". 
 */
static BuiltinTypeSpot *expand_args[] = { 
	&string_spot 
};

/* Do "expand".
 */
static void
apply_expand_call( Reduce *rc, const char *name, HeapNode **arg, PElement *out )
{
	PElement rhs;
	char txt[FILENAME_MAX];
	char txt2[FILENAME_MAX];

	PEPOINTRIGHT( arg[0], &rhs );
	(void) reduce_get_string( rc, &rhs, txt, FILENAME_MAX );
	expand_variables( txt, txt2 );

	if( !heap_managedstring_new( rc->heap, txt2, out ) )
		reduce_throw( rc );
}

/* Args for "name2gtype". 
 */
static BuiltinTypeSpot *name2gtype_args[] = { 
	&string_spot 
};

/* Do "name2gtype".
 */
static void
apply_name2gtype_call( Reduce *rc, const char *name, 
	HeapNode **arg, PElement *out )
{
	PElement rhs;
	char txt[FILENAME_MAX];
	int gtype;

	PEPOINTRIGHT( arg[0], &rhs );
	(void) reduce_get_string( rc, &rhs, txt, FILENAME_MAX );
	gtype = g_type_from_name( txt );

	if( !heap_real_new( rc->heap, gtype, out ) )
		reduce_throw( rc );
}

/* Args for "gtype2name". 
 */
static BuiltinTypeSpot *gtype2name_args[] = { 
	&real_spot 
};

/* Do "gtype2name".
 */
static void
apply_gtype2name_call( Reduce *rc, const char *name, 
	HeapNode **arg, PElement *out )
{
	PElement rhs;
	int gtype;

	PEPOINTRIGHT( arg[0], &rhs );
	gtype = PEGETREAL( &rhs );

	if( !heap_managedstring_new( rc->heap, g_type_name( gtype ), out ) )
		reduce_throw( rc );
}

/* Args for "vips_object_new". 
 */
static BuiltinTypeSpot *vo_new_args[] = { 
	&string_spot,
	&list_spot,
	&list_spot 
};

/* Do a vips_object_new call.
 */
static void
apply_vo_new_call( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out )
{
	PElement rhs;
	char buf[256];
	PElement required;
	PElement optional;

	PEPOINTRIGHT( arg[2], &rhs );
	reduce_get_string( rc, &rhs, buf, 256 );
	PEPOINTRIGHT( arg[1], &required );
	PEPOINTRIGHT( arg[0], &optional );

	vo_object_new( rc, buf, &required, &optional, out );
}

/* All ip's builtin functions. 
 */
static BuiltinInfo builtin_table[] = {
	/* Other.
	 */
	{ "dir", FALSE, 1, &dir_args[0], &apply_dir_call },
	{ "search", FALSE, 1, &search_args[0], &apply_search_call },
	{ "error", FALSE, 1, &error_args[0], &apply_error_call },
	{ "print", FALSE, 1, &print_args[0], &apply_print_call },
	{ "expand", FALSE, 1, &expand_args[0], &apply_expand_call },
	{ "name2gtype", FALSE, 1, &name2gtype_args[0], &apply_name2gtype_call },
	{ "gtype2name", FALSE, 1, &gtype2name_args[0], &apply_gtype2name_call },
	{ "_", FALSE, 1, &underscore_args[0], &apply_underscore_call },

	/* vips8 wrapper.
	 */
	{ "vips_object_new", FALSE, IM_NUMBER( vo_new_args ), 
		&vo_new_args[0], apply_vo_new_call },

	/* Predicates.
	 */
	{ "is_image", FALSE, IM_NUMBER( pred_args ), 
		&pred_args[0], apply_pred_call },
	{ "is_bool", FALSE, IM_NUMBER( pred_args ), 
		&pred_args[0], apply_pred_call },
	{ "is_real", FALSE, IM_NUMBER( pred_args ), 
		&pred_args[0], apply_pred_call },
	{ "is_class", FALSE, IM_NUMBER( pred_args ), 
		&pred_args[0], apply_pred_call },
	{ "is_char", FALSE, IM_NUMBER( pred_args ), 
		&pred_args[0], apply_pred_call },
	{ "is_list", FALSE, IM_NUMBER( pred_args ), 
		&pred_args[0], apply_pred_call },
	{ "is_complex", FALSE, IM_NUMBER( pred_args ), 
		&pred_args[0], apply_pred_call },
	{ "is_instanceof", FALSE, IM_NUMBER( is_instanceof_args ), 
		&is_instanceof_args[0], apply_is_instanceof_call },
	{ "has_member", FALSE, IM_NUMBER( has_member_args ), 
		&has_member_args[0], apply_has_member_call },

	/* List and complex projections.
	 */
	{ "re", TRUE, IM_NUMBER( complex_args ), 
		&complex_args[0], apply_complex_call },
	{ "im", TRUE, IM_NUMBER( complex_args ), 
		&complex_args[0], apply_complex_call },
	{ "hd", TRUE, IM_NUMBER( flist_args ), 
		&flist_args[0], apply_list_call },
	{ "tl", TRUE, IM_NUMBER( flist_args ), 
		&flist_args[0], apply_list_call },

	/* Math.
	 */
	{ "sin", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "cos", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "tan", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call }, 
	{ "asin", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "acos", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "atan", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call }, 
	{ "log", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "log10", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "exp", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "exp10", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "ceil", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },
	{ "floor", TRUE, IM_NUMBER( math_args ), 
		&math_args[0], apply_math_call },

	/* Optional GSL funcs.
	 */
	{ "gammq", TRUE, IM_NUMBER( gammq_args ), 
		&gammq_args[0], apply_gammq_call },

	/* Constructors.
	 */
	{ "vips_image", FALSE,  
		IM_NUMBER( image_args ), &image_args[0], apply_image_call },
	{ "read", FALSE,  
		IM_NUMBER( read_args ), &read_args[0], apply_read_call },
};

#ifdef HAVE_GSL
static void
builtin_gsl_error( const char *reason, const char *file, 
	int line, int gsl_errno )
{
	error_top( _( "GSL library error." ) );
	error_sub( "%s - (%s:%d) - %s", 
		reason, file, line, gsl_strerror( gsl_errno ) );

	reduce_throw( reduce_context );
}
#endif /*HAVE_GSL*/

void
builtin_init( void )
{
	Toolkit *kit;
        int i;

	/* Make the _builtin toolkit and populate.
	 */
	kit = toolkit_new( main_toolkitgroup, "_builtin" );

        for( i = 0; i < IM_NUMBER( builtin_table ); i++ ) {
		Symbol *sym;

		sym = symbol_new( symbol_root->expr->compile,
			builtin_table[i].name );
		g_assert( sym->type == SYM_ZOMBIE );
		sym->type = SYM_BUILTIN;
		sym->builtin = &builtin_table[i];
		(void) tool_new_sym( kit, -1, sym );
		symbol_made( sym );
	}

	filemodel_set_auto_load( FILEMODEL( kit ) );
	filemodel_set_modified( FILEMODEL( kit ), FALSE );
	kit->pseudo = TRUE;

	/* Start up GSL, if we have it.
	 */
#ifdef HAVE_GSL
	gsl_set_error_handler( builtin_gsl_error );
#endif /*HAVE_GSL*/
}

/* Make a usage error.
 */
void
builtin_usage( VipsBuf *buf, BuiltinInfo *builtin )
{
	int i;

	vips_buf_appendf( buf, 
		ngettext( "Builtin \"%s\" takes %d argument.", 
			"Builtin \"%s\" takes %d arguments.", 
			builtin->nargs ),
		builtin->name, builtin->nargs );
        vips_buf_appends( buf, "\n" );

	for( i = 0; i < builtin->nargs; i++ )
		vips_buf_appendf( buf, "    %d - %s\n", 
		i + 1,
		builtin->args[i]->name );
}

#ifdef DEBUG
static void
builtin_trace_args( Heap *heap, const char *name, int n, HeapNode **arg )
{
	int i;
	VipsBuf buf;
	char txt[100];

	vips_buf_init_static( &buf, txt, 100 );

	for( i = 0; i < n; i++ ) {
		PElement t;

		PEPOINTRIGHT( arg[n - i - 1], &t );
		vips_buf_appends( &buf, "(" );
		graph_pelement( hi, &buf, &t, FALSE );
		vips_buf_appends( &buf, ") " );
	}

	printf( "builtin: %s %s\n", name, vips_buf_all( &buf ) );
}
#endif /*DEBUG*/

/* Execute the internal implementation of a builtin function.
 */
void
builtin_run( Reduce *rc, Compile *compile,
	int op, const char *name, HeapNode **arg, PElement *out,
	BuiltinInfo *builtin )
{
	int i;

	/* Typecheck args.
	 */
	for( i = 0; i < builtin->nargs; i++ ) {
		BuiltinTypeSpot *ts = builtin->args[i];
		PElement base;

		PEPOINTRIGHT( arg[builtin->nargs - i - 1], &base );
		if( !ts->pred( rc, &base ) ) {
			VipsBuf buf;
			char txt[100];

			vips_buf_init_static( &buf, txt, 100 );
			itext_value_ev( rc, &buf, &base );
			error_top( _( "Bad argument." ) );
			error_sub( _( "Argument %d to builtin \"%s\" should "
				"be \"%s\", you passed:\n  %s" ),
				i + 1, name, ts->name,
				vips_buf_all( &buf ) );
			reduce_throw( rc );
		}
	}

#ifdef DEBUG
	builtin_trace_args( rc->heap, name, builtin->nargs, arg );
#endif /*DEBUG*/

	builtin->fn( rc, name, arg, out );
}
