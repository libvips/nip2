/* base model for a client regions on an imageview
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
#define DEBUG
 */

#include "ip.h"

G_DEFINE_TYPE( iRegiongroup, iregiongroup, TYPE_CLASSMODEL ); 

static void *
iregiongroup_update_model( Heapmodel *heapmodel )
{
#ifdef DEBUG
	printf( "iregiongroup_update_model: " );
	row_name_print( heapmodel->row );
	printf( "\n" );
#endif /*DEBUG*/

	if( HEAPMODEL_CLASS( iregiongroup_parent_class )->
		update_model( heapmodel ) )
		return( heapmodel );

	/* Only display most-derived classes. Don't display "this".
	 */
	if( heapmodel->row->sym )
		model_display( MODEL( heapmodel ),
			!is_super( heapmodel->row->sym ) &&
			!is_this( heapmodel->row->sym ) );

	return( NULL );
}

static View *
iregiongroup_view_new( Model *model, View *parent )
{
	return( iregiongroupview_new() );
}

static void
iregiongroup_class_init( iRegiongroupClass *class )
{
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	heapmodel_class->update_model = iregiongroup_update_model;

	model_class->view_new = iregiongroup_view_new;
}

static void
iregiongroup_init( iRegiongroup *iregiongroup )
{
	/* Display turned on in _update_model() above.
	 */
	MODEL( iregiongroup )->display = FALSE;
}

iRegiongroup *
iregiongroup_new( Classmodel *classmodel )
{
	iRegiongroup *iregiongroup;

	iregiongroup = IREGIONGROUP( g_object_new( TYPE_IREGIONGROUP, NULL ) );

	icontainer_child_add( ICONTAINER( classmodel ), 
		ICONTAINER( iregiongroup ), -1 );

#ifdef DEBUG
	printf( "iregiongroup_new: " );
	row_name_print( HEAPMODEL( classmodel )->row );
	printf( "\n" );
#endif /*DEBUG*/

	return( iregiongroup );
}
