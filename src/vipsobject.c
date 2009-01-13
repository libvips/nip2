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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#include "ip.h"

/*
 */
#define DEBUG

/* Maxiumum number of args to constructor.
 */
#define MAX_VIPS_ARGS (100)

/* What we track during construct.
 */
typedef struct _Vo {
	 Reduce *rc;

	/* Operation we are building.
	 */
	char *name;
	GType type;
	VipsObject *object;

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
} Vo;

static void
vo_free( Vo *vo )
{
	IM_FREE( vo->name );

	while( vo->unref ) {
		GObject *object = G_OBJECT( vo->unref->data );

		g_object_unref( object );

		vo->unref = g_slist_remove( vo->unref, object );
	}

	IM_FREEF( g_object_unref, vo->object );

	im_free( vo );
}

static Vo *
vo_new( Reduce *rc, const char *name )
{
	GType type;
	Vo *vo;

	if( !(type = vips_type_find( "VipsObject", name )) ) {
		error_top( _( "No such type" ) );
		error_sub( _( "Type \"%s\" not found as a subclass "
			"of VipsObject." ), name );
		return( NULL );
	}

	if( !(vo = INEW( NULL, Vo )) )
		return( NULL );
	vo->rc = rc;
	vo->name = NULL;
	vo->type = type;
	vo->object = NULL;
	vo->nargs_supplied = 0;
	vo->nargs_required = 0;
	vo->nargs_output = 0;
	vo->unref = NULL;

	if( !(vo->name = im_strdupn( name )) ) {
		vo_free( vo );
		return( NULL );
	}

	vo->object = VIPS_OBJECT( g_object_new( type, NULL ) );

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

static void *
vo_set_required_input( VipsObject *object, GParamSpec *pspec,
        VipsArgumentClass *argument_class, 
	VipsArgumentInstance *argument_instance, Vo *vo )
{
	VipsArgument *argument = (VipsArgument *) argument_class;
	const char *name = argument->pspec->name;

	/* Looking for required input args ... these are the ones we can set
	 * from the supplied required list. 
	 */
	if( (argument_class->flags & VIPS_ARGUMENT_REQUIRED) &&
		!(argument_class->flags & VIPS_ARGUMENT_CONSTRUCT) &&
		!argument_instance->assigned &&
		vo->nargs_required < vo->nargs_supplied ) {
		/* Also insist we are under the nargs
		 * limit ... our caller checks the numbers match and makes the
		 * error message.
		 */
		if( vo->nargs_required < vo->nargs_supplied ) {
			int i = vo->nargs_required;
			GValue value = { 0 };

			if( !heap_ip_to_gvalue( &vo->args[i], &value ) )
				return( object );
			g_object_set_property( G_OBJECT( object ), 
				name, &value );
			g_value_unset( &value );
		}

		vo->nargs_required += 1;
	}

	return( NULL );
}

static void *
vo_set_arg( const char *name, PElement *value, Vo *vo )
{
	GValue gvalue = { 0 };

	if( !heap_ip_to_gvalue( value, &gvalue ) )
		return( value );
	g_object_set_property( G_OBJECT( vo->object ), name, &gvalue );
	g_value_unset( &gvalue );

	return( NULL );
}

/* Set a set of optional args ... of the form [["caption", 12], ["label", 42]]
 * etc.
 */
static gboolean
vo_set_optional( Vo *vo, PElement *list )
{
	if( heap_map_dict( list,
		(heap_map_dict_fn) vo_set_arg, vo, NULL ) )
		return( FALSE );

	return( TRUE );
}

static gboolean
vo_build_result( Vo *vo, PElement *out )
{
	/* We don't have it yet, but we should loop over args looking for
	 * outputs and building a list. Paste this code back in at some
	 * point.
	 */
	Managedgobject *managedgobject;

	if( !(managedgobject = managedgobject_new( vo->rc->heap, 
		G_OBJECT( vo->object ) )) )
		return( FALSE );

	PEPUTP( out, ELEMENT_MANAGED, managedgobject );

	return( TRUE );
}

/* We know we have three args: name, required, optional.
 */
void
vo_object_new( Reduce *rc, const char *name, 
	PElement *required, PElement *optional, PElement *out )
{
	Vo *vo;

	if( !(vo = vo_new( rc, name )) ) 
		reduce_throw( rc );

	/* Gather supplied required input args list.
	 */
	if( heap_map_list( required, 
		(heap_map_list_fn) vo_gather_required, vo, NULL ) ) {
		vo_free( vo );
		reduce_throw( rc );
	}

	/* Set required input arguments.
	 */
	if( vips_argument_map( VIPS_OBJECT( vo->object ),
		(VipsArgumentMapFn) vo_set_required_input, vo, NULL ) ) {
		vo_free( vo );
		reduce_throw( rc );
	}
	if( vo->nargs_supplied != vo->nargs_required ) {
		error_top( _( "Wrong number of required arguments." ) );
		error_sub( _( "Operation \"%s\" has %d required arguments, "
			"you supplied %d." ),
			vo->name,
			vo->nargs_required,
			vo->nargs_supplied );
		vo_free( vo );
	}

	/* Init required output arguments.

	   	we don't have any output args yet: paste this stuff back in 
		once we have a VipsOperation subclass

	if( vips__argument_map( VIPS_OBJECT( vo->operation ),
		(VipsArgumentMapFn) vo_set_required_output, vo, NULL ) ) {
		vo_free( vo );
		reduce_throw( rc );
	}
	 */

	/* Set all optional input args.
	 */
	if( !vo_set_optional( vo, optional ) ) {
		vo_free( vo );
		reduce_throw( rc );
	}

	/* Ask the operation to construct.
	 */
	if( vips_object_build( vo->object ) ) {
		error_top( _( "VIPS library error." ) );
		error_sub( _( "Unable to build object." ) );
		vo_free( vo );
		reduce_throw( rc );
	}

	/* Build the return value.
	 */
	if( !vo_build_result( vo, out ) ) {
		vo_free( vo );
		reduce_throw( rc );
	}

#ifdef DEBUG
{
	VipsBuf buf;
	char txt[1000];

	vips_buf_init_static( &buf, txt, 1000 );
	vips_object_to_string( vo->object, &buf );
	printf( "vo_object_new: built %s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG*/

	vo_free( vo );
}
