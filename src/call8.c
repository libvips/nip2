/* Call vips8 functions with args from C and from the graph.
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
#define DEBUG
#define DEBUG_HISTORY_MISS
#define DEBUG_HISTORY
 */

/* Disable for now ... vips8 API is in flux again.
 */
#undef HAVE_VIPS8

#ifdef HAVE_VIPS8

/* Maxiumum number of args to a VIPS function.
 */
#define MAX_VIPS_ARGS (100)

/* What we track while we call a vips operation.
 */
typedef struct _Call8 {
	 Reduce *rc;

	/* Operation we are building.
	 */
	char *name;
	VipsOperation *operation;

	/* Required args supplied to us from nip.
	 */
	PElement args[MAX_VIPS_ARGS];
	int nargs_supplied;

	/* Number of required input args the operation actually has.
	 */
	int nargs_required;

	/* Number of ouput args the operation has.
	 */
	int nargs_output;

	/* Output objects we have built and should unref.
	 */
	GSList *unref;
} Call8;

static void
call8_free( Call8 *call8 )
{
	IM_FREE( call8->name );

	while( call8->unref ) {
		GObject *object = G_OBJECT( call8->unref->data );

		g_object_unref( object );

		call8->unref = g_slist_remove( call8->unref, object );
	}

	IM_FREEF( g_object_unref, call8->operation );

	im_free( call8 );
}

static void
call8_error( GError **error )
{
	error_top( _( "VIPS8 error." ) );

	if( *error ) {
		error_sub( "%s", (*error)->message );
		g_error_free( *error );
		*error = NULL;
	}
}

static Call8 *
call8_new( Reduce *rc, const char *name )
{
	Call8 *call8 = INEW( NULL, Call8 );
	GError *error = NULL;

	if( !call8 )
		return( NULL );
	call8->rc = rc;
	call8->name = NULL;
	call8->operation = NULL;
	call8->nargs_supplied = 0;
	call8->nargs_required = 0;
	call8->nargs_output = 0;
	call8->unref = NULL;

	if( !(call8->name = im_strdupn( name )) ) {
		call8_free( call8 );
		return( NULL );
	}

	if( !(call8->operation = vips_operation_new( name, &error )) ) {
		call8_error( &error );
		call8_free( call8 );
		return( NULL );
	}

	return( call8 );
}

static void *
call8_gather_required( PElement *item, Call8 *call8 )
{
	if( call8->nargs_supplied >= MAX_VIPS_ARGS ) {
		error_top( _( "Too many arguments." ) );
		error_sub( _( "No more than %d arguments allowed." ),
			MAX_VIPS_ARGS );
		return( item );
	}

	call8->args[call8->nargs_supplied] = *item;

	call8->nargs_supplied += 1;

	return( NULL );
}

static void *
call8_set_required_input( VipsObject *object, GParamSpec *pspec,
        VipsArgumentClass *argument_class, 
	VipsArgumentInstance *argument_instance, Call8 *call8 )
{
	VipsArgument *argument = (VipsArgument *) argument_class;
	const char *name = argument->pspec->name;

	/* Looking for required input args ... these are the ones we can set
	 * from the supplied required list. 
	 */
	if( (argument_class->flags & VIPS_ARGUMENT_REQUIRED) &&
		!(argument_class->flags & VIPS_ARGUMENT_OUTPUT) &&
		!argument_instance->assigned &&
		call8->nargs_required < call8->nargs_supplied ) {
		/* Also insist we are under the nargs
		 * limit ... our caller checks the numbers match and makes the
		 * error message.
		 */
		if( call8->nargs_required < call8->nargs_supplied ) {
			int i = call8->nargs_required;
			GValue value = { 0 };

			if( !heap_ip_to_gvalue( &call8->args[i], &value ) )
				return( object );
			vips_object_header_set( object, name, &value );
			g_value_unset( &value );
		}

		call8->nargs_required += 1;
	}

	return( NULL );
}

static void *
call8_set_required_output( VipsObject *object, GParamSpec *pspec,
        VipsArgumentClass *argument_class, 
	VipsArgumentInstance *argument_instance, Call8 *call8 )
{
	VipsArgument *argument = (VipsArgument *) argument_class;
	const char *name = argument->pspec->name;

	/* Looking for required output args.
	 */
	if( (argument_class->flags & VIPS_ARGUMENT_REQUIRED) &&
		(argument_class->flags & VIPS_ARGUMENT_OUTPUT) &&
		!argument_instance->assigned ) {
		GValue value = { 0 };

		if( G_PARAM_SPEC_VALUE_TYPE( pspec ) == VIPS_TYPE_IMAGE ) {
			VipsImage *image;

			image = vips_image_new( "p", "call8.c:314", 
				NULL, NULL );
			g_value_init( &value, VIPS_TYPE_IMAGE );
			g_value_set_object( &value, image );
			vips_object_header_set( object, name, &value );
			g_value_unset( &value );

			/* Don't unref the image yet ... do this after we've
			 * sent the final output objects back to nip and got
			 * them managed.
			 */
			call8->unref = g_slist_prepend( call8->unref, image );
		}
	}

	return( NULL );
}

/* Set a set of optional args ... of the form ["caption", 12, "label", 42]
 * etc.
 */
static gboolean
call8_set_optional( VipsObject *object, PElement *list )
{
	PElement data;

	while( heap_get_list_next( list, &data ) ) {
		char name[256];
		GValue value = { 0 };

		if( !heap_get_string( &data, name, 256 ) ||
			!heap_get_list_next( list, &data ) ||
			!heap_ip_to_gvalue( &data, &value ) )
			return( FALSE );
		vips_object_header_set( object, name, &value );
		g_value_unset( &value );
	}

	return( TRUE );
}

static void *
call8_build_result_item( VipsObject *object, GParamSpec *pspec,
        VipsArgumentClass *argument_class, 
	VipsArgumentInstance *argument_instance, Call8 *call8, PElement *list )
{
	VipsArgument *argument = (VipsArgument *) argument_class;
	const char *name = argument->pspec->name;
	GError *error = NULL;

	/* Looking for required output args ... write to the list.
	 */
	if( (argument_class->flags & VIPS_ARGUMENT_REQUIRED) &&
		(argument_class->flags & VIPS_ARGUMENT_OUTPUT) ) {
		PElement data;
		GValue value = { 0 };

		if( !heap_list_add( call8->rc->heap, list, &data ) )
			return( object );
		if( !vips_object_header_get( object, name, &value, &error ) ) {
			call8_error( &error );
			return( object );
		}
		if( !heap_gvalue_to_ip( &value, &data ) )
			return( object );

		g_value_unset( &value );

		(void) heap_list_next( list );
	}

	return( NULL );
}

static gboolean
call8_build_result( Call8 *call8, PElement *out )
{
	/* Have to build a list of results. We don't need to worry about
	 * overwriting our args, since vipsoperation will be keeping a ref to
	 * them all for us.
	 */
	PElement list;

	list = *out;
	heap_list_init( &list );
	if( vips__argument_map( VIPS_OBJECT( call8->operation ),
		(VipsArgumentMapFn) call8_build_result_item, call8, &list ) ) 
		return( FALSE );

	return( TRUE );
}

/* We know we have three args: name, required, optional.
 */
void
call8_vips( Reduce *rc, const char *name, 
	PElement *required, PElement *optional, PElement *out )
{
	Call8 *call8;
	GError *error = NULL;

	if( !(call8 = call8_new( rc, name )) ) 
		reduce_throw( rc );

	/* Gather supplied required input args list.
	 */
	if( heap_map_list( required, 
		(heap_map_list_fn) call8_gather_required, call8, NULL ) ) {
		call8_free( call8 );
		reduce_throw( rc );
	}

	/* Set required input arguments.
	 */
	if( vips__argument_map( VIPS_OBJECT( call8->operation ),
		(VipsArgumentMapFn) call8_set_required_input, call8, NULL ) ) {
		call8_free( call8 );
		reduce_throw( rc );
	}
	if( call8->nargs_supplied != call8->nargs_required ) {
		error_top( _( "Wrong number of required arguments." ) );
		error_sub( _( "Operation \"%s\" has %d required arguments, "
			"you supplied %d." ),
			call8->name,
			call8->nargs_required,
			call8->nargs_supplied );
		call8_free( call8 );
	}

	/* Init required output arguments.
	 */
	if( vips__argument_map( VIPS_OBJECT( call8->operation ),
		(VipsArgumentMapFn) call8_set_required_output, call8, NULL ) ) {
		call8_free( call8 );
		reduce_throw( rc );
	}

	/* Set all optional input args.
	 */
	if( !call8_set_optional( VIPS_OBJECT( call8->operation ), optional ) ) {
		call8_free( call8 );
		reduce_throw( rc );
	}

	/* Ask the operation to construct.
	 */
	if( !vips_object_build( VIPS_OBJECT( call8->operation ), &error ) ) {
		call8_error( &error );
		call8_free( call8 );
		reduce_throw( rc );
	}

	/* Build the return value.
	 */
	if( !call8_build_result( call8, out ) ) {
		call8_free( call8 );
		reduce_throw( rc );
	}

	call8_free( call8 );
}

/* Wrap vips_object_header_get().
 */
void
call8_header_get( Reduce *rc, 
	const char *name, VipsObject *object, PElement *out )
{
	GError *error = NULL;
	GValue value = { 0 };

	if( !vips_object_header_get( object, name, &value, &error ) ) {
		call8_error( &error );
		reduce_throw( rc );
	}

	if( !heap_gvalue_to_ip( &value, out ) )
		reduce_throw( rc );

	g_value_unset( &value );
}

Managed *
call8_image_new( Reduce *rc, const char *filename, PElement *optional )
{
	Heap *heap = rc->heap;
	VipsImage *image;
	GError *error = NULL;
	Managed *managed;

	image = g_object_new( VIPS_TYPE_IMAGE, 
		"filename", filename,
		"mode", "r", 
		NULL );

	if( !call8_set_optional( VIPS_OBJECT( image ), optional ) ) {
		g_object_unref( image );
		reduce_throw( rc );
	}
	if( !vips_object_build( VIPS_OBJECT( image ), &error ) ) {
		call8_error( &error );
		g_object_unref( image );
		reduce_throw( rc );
	}

	managed = MANAGED( managedgobject_new( heap, G_OBJECT( image ) ) );
	managed_dup_nonheap( managed );

	/* managedgobject now holds a ref  to the image ... we can drop the 
	 * one that g_object_new() gave us.
	 */
	g_object_unref( image );

	return( managed );
}

#else /*!HAVE_VIPS8*/

/* Funny, youu'd think the noreturn would propogate from reduce_throw() to
 * call8_novips().
 */
static void call8_novips8( Reduce *rc ) __attribute__((noreturn));

static void
call8_novips8( Reduce *rc ) 
{
	error_top( _( "VIPS8 not linked." ) );
	reduce_throw( rc );
}

void
call8_vips( Reduce *rc, const char *name, 
	PElement *required, PElement *optional, PElement *out )
{
	call8_novips8( rc );
}

void
call8_header_get( Reduce *rc, 
	const char *name, VipsObject *object, PElement *out )
{
	call8_novips8( rc );
}

Managed *
call8_image_new( Reduce *rc, const char *filename, PElement *optional )
{
	call8_novips8( rc );
}

#endif /*HAVE_VIPS8*/
