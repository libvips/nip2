/* Interface to VipsObject.
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

/* Maxiumum number of args to constructor.
 */
#define MAX_VIPS_ARGS (100)

/* What we track during construct.
 */
typedef struct _Vo {
	 Reduce *rc;

	/* Object we are building.
	 */
	VipsObject *object;
	const char *name;

	/* Required args supplied to us from nip.
	 */
	PElement args[MAX_VIPS_ARGS];
	int nargs_supplied;

	/* Number of required input args the object has.
	 */
	int nargs_required;

	/* Number of output args the object has.
	 */
	int nargs_output;

	/* A place to build the output, safe from the GC.
	 */
	Element out;
} Vo;

static void
vo_free( Vo *vo )
{
	heap_unregister_element( vo->rc->heap, &vo->out );
	VIPS_UNREF( vo->object );

	im_free( vo );
}

static Vo *
vo_new( Reduce *rc, const char *name )
{
	VipsObjectClass *class;
	Vo *vo;

	if( !(class = vips_class_find( "VipsObject", name )) )
		return( NULL );

	if( !(vo = INEW( NULL, Vo )) ) 
		return( NULL );
	vo->rc = rc;
	vo->object = g_object_new( G_OBJECT_CLASS_TYPE( class ), NULL );
	vo->name = class->nickname;
	vo->nargs_supplied = 0;
	vo->nargs_required = 0;
	vo->nargs_output = 0;
	vo->out.type = ELEMENT_NOVAL;
	vo->out.ele = (void *) 12;

	heap_register_element( rc->heap, &vo->out );

	return( vo );
}

static void *
vo_gather_required( PElement *item, Vo *vo )
{
	if( vo->nargs_supplied >= MAX_VIPS_ARGS ) {
		error_top( _( "Too many arguments." ) );
		error_sub( _( "No more than %d arguments allowed." ),
			MAX_VIPS_ARGS );
		return( item );
	}

	vo->args[vo->nargs_supplied] = *item;

	vo->nargs_supplied += 1;

	return( NULL );
}

static int
vo_set_property( Vo *vo, 
	const char *name, GParamSpec *pspec, GValue *value )
{
	/* If we're setting an enum from a string, look up the enum nickname.
	 */
	if( G_IS_PARAM_SPEC_ENUM( pspec ) &&
		G_VALUE_TYPE( value ) == VIPS_TYPE_REF_STRING ) {
		const char *str = vips_value_get_ref_string( value, NULL );

		if( vips_object_set_argument_from_string( vo->object, 
			name, str ) )
			return( -1 );
	}
	else
		g_object_set_property( G_OBJECT( vo->object ), name, value );

	return( 0 );
}

static void *
vo_set_required_input( VipsObject *object, GParamSpec *pspec,
        VipsArgumentClass *argument_class, 
	VipsArgumentInstance *argument_instance, Vo *vo )
{
	/* Looking for required input args ... these are the ones we can set
	 * from the supplied required list. 
	 */
	if( (argument_class->flags & VIPS_ARGUMENT_REQUIRED) &&
		(argument_class->flags & VIPS_ARGUMENT_CONSTRUCT) &&
		(argument_class->flags & VIPS_ARGUMENT_INPUT) &&
		!argument_instance->assigned &&
		vo->nargs_required < vo->nargs_supplied ) {
		const char *name = g_param_spec_get_name( pspec );
		int i = vo->nargs_required;

		GValue gvalue = { 0 };

		if( !heap_ip_to_gvalue( &vo->args[i], &gvalue ) )
			return( object );
		if( vo_set_property( vo, name, pspec, &gvalue ) ) {
			g_value_unset( &gvalue );
			return( object );
		}
		g_value_unset( &gvalue );

		vo->nargs_required += 1;
	}

	return( NULL );
}

static void *
vo_set_optional_arg( const char *name, PElement *value, Vo *vo )
{
	GParamSpec *pspec;
	VipsArgumentClass *argument_class;
	VipsArgumentInstance *argument_instance;

	/* Looking for construct-time optional input args.
	 */

	/* For optional args, we should ignore properties that don't exist. For
	 * example, we might supply ($sharpening => 12) to all interpolators,
	 * though only one interpolator uses this property.
	 */
	if( vips_object_get_argument( vo->object, name,
		&pspec, &argument_class, &argument_instance ) )
		return( NULL );

	if( !(argument_class->flags & VIPS_ARGUMENT_REQUIRED) &&
		(argument_class->flags & VIPS_ARGUMENT_CONSTRUCT) &&
		(argument_class->flags & VIPS_ARGUMENT_INPUT) &&
		!argument_instance->assigned ) {
		GValue gvalue = { 0 };

		if( !heap_ip_to_gvalue( value, &gvalue ) ) {
			g_value_unset( &gvalue );
			return( value );
		}
		if( vo_set_property( vo, name, pspec, &gvalue ) ) {
			g_value_unset( &gvalue );
			return( value );
		}
		g_value_unset( &gvalue );
	}

	return( NULL );
}

/* Set a set of optional args ... of the form [["caption", 12], ["label", 42]]
 * etc.
 */
static gboolean
vo_set_optional( Vo *vo, PElement *optional )
{
	if( heap_map_dict( optional,
		(heap_map_dict_fn) vo_set_optional_arg, vo, NULL ) )
		return( FALSE );

	return( TRUE );
}

/* Make a vo and supply args from nip2.
 */
static gboolean
vo_args( Vo *vo, PElement *required, PElement *optional )
{
	/* Gather supplied required input args list.
	 */
	if( heap_map_list( required, 
		(heap_map_list_fn) vo_gather_required, vo, NULL ) ) 
		return( FALSE );

	/* Set required input arguments.
	 */
	if( vips_argument_map( VIPS_OBJECT( vo->object ),
		(VipsArgumentMapFn) vo_set_required_input, vo, NULL ) ) 
		return( FALSE );
	if( vo->nargs_supplied != vo->nargs_required ) {
		error_top( _( "Wrong number of required arguments." ) );
		error_sub( _( "Operation \"%s\" has %d required arguments, "
			"you supplied %d." ),
			vo->name,
			vo->nargs_required,
			vo->nargs_supplied );
		return( FALSE );
	}

	/* Set all optional input args.
	 */
	if( !vo_set_optional( vo, optional ) ) 
		return( FALSE );

	return( TRUE );
}

/* Make a VipsObject.
 */
void
vo_object_new( Reduce *rc, const char *name, 
	PElement *required, PElement *optional, PElement *out )
{
	Vo *vo;
	Managedgobject *managedgobject;

	if( !(vo = vo_new( rc, name )) ) 
		reduce_throw( rc );

	if( !vo_args( vo, required, optional ) ) {
		vo_free( vo );
		reduce_throw( rc );
	}

	/* Ask the object to construct.
	 */
	if( vips_object_build( vo->object ) ) {
		error_top( _( "VIPS library error." ) );
		error_sub( "%s", im_error_buffer() );
		im_error_clear();
		vo_free( vo );
		reduce_throw( rc );
	}

	/* Return the constructed object.
	 */
	if( !(managedgobject = managedgobject_new( vo->rc->heap, 
		G_OBJECT( vo->object ) )) ) {
		vo_free( vo );
		reduce_throw( rc );
	}

	PEPUTP( out, ELEMENT_MANAGED, managedgobject );

#ifdef DEBUG
{
	char txt[1000];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	vips_object_to_string( vo->object, &buf );
	printf( "vo_object_new: built %s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG*/

	vo_free( vo );
}

/* Looking for required output args ... append to out.
 */
static void *
vo_get_required_output( VipsObject *object, GParamSpec *pspec,
        VipsArgumentClass *argument_class, 
	VipsArgumentInstance *argument_instance, Vo *vo, PElement *out )
{
	if( (argument_class->flags & VIPS_ARGUMENT_REQUIRED) &&
		(argument_class->flags & VIPS_ARGUMENT_OUTPUT) &&
		argument_instance->assigned ) {
		const char *name = g_param_spec_get_name( pspec );
		GType type = G_PARAM_SPEC_VALUE_TYPE( pspec );
		PElement lhs;

		GValue value = { 0 };

		if( !heap_list_add( vo->rc->heap, out, &lhs ) )
			return( object );
		g_value_init( &value, type );
		g_object_get_property( G_OBJECT( object ), name, &value );
		if( !heap_gvalue_to_ip( &value, &lhs ) ) {
			g_value_unset( &value );
			return( object );
		}
		g_value_unset( &value );

		(void) heap_list_next( out );
	}

	return( NULL );
}

/* Looking for construct-time optional output args. Append them to out.
 */
static void *
vo_get_optional_arg( const char *name, PElement *value, Vo *vo, PElement *out )
{
	GParamSpec *pspec;
	VipsArgumentClass *argument_class;
	VipsArgumentInstance *argument_instance;

	if( vips_object_get_argument( vo->object, name,
		&pspec, &argument_class, &argument_instance ) )
		return( NULL );

	if( !(argument_class->flags & VIPS_ARGUMENT_REQUIRED) &&
		(argument_class->flags & VIPS_ARGUMENT_OUTPUT) &&
		argument_instance->assigned ) {
		GType type = G_PARAM_SPEC_VALUE_TYPE( pspec );
		GValue gvalue = { 0 };
		PElement lhs;

		if( !heap_list_add( vo->rc->heap, out, &lhs ) )
			return( value );
		g_value_init( &gvalue, type );
		g_object_get_property( G_OBJECT( vo->object ), name, &gvalue );
		if( !heap_gvalue_to_ip( &gvalue, &lhs ) ) {
			g_value_unset( &gvalue );
			return( value );
		}
		g_value_unset( &gvalue );

		(void) heap_list_next( out );
	}

	return( NULL );
}

/* Get a set of optional args ... of the form [["caption", []], ["label", []]]
 * etc.
 */
static gboolean
vo_get_optional( Vo *vo, PElement *optional, PElement *out )
{
	if( heap_map_dict( optional,
		(heap_map_dict_fn) vo_get_optional_arg, vo, out ) )
		return( FALSE );

	return( TRUE );
}

/* Run a VipsOperation. Like vo_object_new(), but we return the output args
 * rather than the operation.
 */
void
vo_call( Reduce *rc, const char *name, 
	PElement *required, PElement *optional, PElement *out )
{
	Vo *vo;
	PElement pe;

	if( !(vo = vo_new( rc, name )) ) 
		reduce_throw( rc );

	if( !vo_args( vo, required, optional ) ) {
		vo_free( vo );
		reduce_throw( rc );
	}

	/* Ask the object to construct. This can update vo->operation with an
	 * old one from the cache.
	 */
	if( vips_cache_operation_buildp( (VipsOperation **) &vo->object ) ) {
		error_top( _( "VIPS library error." ) );
		error_sub( "%s", im_error_buffer() );
		im_error_clear();
		vips_object_unref_outputs( vo->object );
		vo_free( vo );
		reduce_throw( rc );
	}

	/* We can't build the output object directly on out, since it might be
	 * one of our inputs. We use the safe Element in vo for the build,
	 * then copy at the end.
	 */

	/* Empty output list.
	 */
	PEPOINTE( &pe, &vo->out );
	heap_list_init( &pe );

	/* Append required outputs.
	 */
	if( vips_argument_map( VIPS_OBJECT( vo->object ),
		(VipsArgumentMapFn) vo_get_required_output, vo, &pe ) ) {
		vips_object_unref_outputs( vo->object );
		vo_free( vo );
		reduce_throw( rc );
	}

	/* Append optional outputs.
	 */
	if( !vo_get_optional( vo, optional, &pe ) ) {
		vips_object_unref_outputs( vo->object );
		vo_free( vo );
		reduce_throw( rc );
	}

	/* Now write the output object to out.
	 */
	PEPUTE( out, &vo->out );

	vips_object_unref_outputs( vo->object );
	vo_free( vo );
}
