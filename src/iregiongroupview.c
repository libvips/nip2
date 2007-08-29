/* coordinate the display of regionviews on imageviews
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

/*
#define DEBUG
 */

#include "ip.h"

static ViewClass *parent_class = NULL;

static void
iregiongroupview_destroy( GtkObject *object )
{
	iRegiongroupview *iregiongroupview;
	iObject *iobject;
	iContainer *parent;

#ifdef DEBUG
	printf( "iregiongroupview_destroy\n" );
#endif /*DEBUG*/

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_IREGIONGROUPVIEW( object ) );

	iregiongroupview = IREGIONGROUPVIEW( object );

	/* Destroy all regionviews we manage.
	 */
	if( (iobject = VOBJECT( iregiongroupview )->iobject) &&
		(parent = ICONTAINER( iobject )->parent) ) 
		slist_map( CLASSMODEL( parent )->views, 
			(SListMapFn) object_destroy, NULL );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

/* What we track during a refresh.
 */
typedef struct {
	GSList *notused;
	iRegiongroupview *iregiongroupview;
	Classmodel *classmodel;
	iImage *iimage;
	Imagepresent *ip;
} iRegiongroupviewRefreshState;

static Regionview *
iregiongroupview_refresh_imageview_test( Regionview *regionview,
	iRegiongroupviewRefreshState *irs )
{
	if( regionview->classmodel == irs->classmodel && 
		regionview->ip == irs->ip )
		return( regionview );

	return( NULL );
}

static void *
iregiongroupview_refresh_imageview( Imagepresent *ip, 
	iRegiongroupviewRefreshState *irs )
{
	Regionview *regionview;

	irs->ip = ip;

	/* Do we have a Regionview for this iv already?
	 */
	if( (regionview = slist_map( irs->notused,
		(SListMapFn) iregiongroupview_refresh_imageview_test, irs )) ) {
		/* Yes ... reuse.
		 */
		irs->notused = g_slist_remove( irs->notused, regionview );
	}
	else {
		/* Nope ... make a new one.
		 */
		iRegionInstance *instance = 
			classmodel_get_instance( irs->classmodel );
		PElement *root = &HEAPMODEL( irs->classmodel )->row->expr->root;

		if( instance ) {
			Regionview *regionview = regionview_new( 
				irs->classmodel, &instance->area, ip );

#ifdef DEBUG
			printf( "iregiongroupview_refresh_imageview: "
				"creating new regionview\n" );
#endif /*DEBUG*/

			/* Set the display type from the heap class name.
			 */
			regionview_set_type( regionview, root );
		}
	}

	return( NULL );
}

static void *
iregiongroupview_refresh_iimage( iImage *iimage, 
	iRegiongroupviewRefreshState *irs )
{
	irs->iimage = iimage;
	slist_map( iimage->views,
		(SListMapFn) iregiongroupview_refresh_imageview, irs );

	return( NULL );
}

static void 
iregiongroupview_refresh( vObject *vobject )
{
	iRegiongroupview *iregiongroupview = IREGIONGROUPVIEW( vobject );
	Classmodel *classmodel = CLASSMODEL( vobject->iobject );

	iRegiongroupviewRefreshState irs;

#ifdef DEBUG
	printf( "iregiongroupview_refresh\n" );
#endif /*DEBUG*/

	/* Make a note of all the displays we have now, loop over the 
	 * displays we should have, reusing when possible ... remove any 
	 * unused displays at the end.
	 */
	irs.classmodel = CLASSMODEL( ICONTAINER( classmodel )->parent );
	irs.notused = g_slist_copy( irs.classmodel->views );
	irs.iregiongroupview = iregiongroupview;

	slist_map( irs.classmodel->iimages,
		(SListMapFn) iregiongroupview_refresh_iimage, &irs );

	/* Remove all the regionviews we've not used. 
	 */
	slist_map( irs.notused, (SListMapFn) object_destroy, NULL );
	IM_FREEF( g_slist_free, irs.notused );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
iregiongroupview_class_init( iRegiongroupviewClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = iregiongroupview_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = iregiongroupview_refresh;
}

static void
iregiongroupview_init( iRegiongroupview *iregiongroupview )
{
#ifdef DEBUG
	printf( "iregiongroupview_init\n" );
#endif /*DEBUG*/
}

GtkType
iregiongroupview_get_type( void )
{
	static GtkType iregiongroupview_type = 0;

	if( !iregiongroupview_type ) {
		static const GtkTypeInfo info = {
			"iRegiongroupview",
			sizeof( iRegiongroupview ),
			sizeof( iRegiongroupviewClass ),
			(GtkClassInitFunc) iregiongroupview_class_init,
			(GtkObjectInitFunc) iregiongroupview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		iregiongroupview_type = gtk_type_unique( TYPE_VIEW, &info );
	}

	return( iregiongroupview_type );
}

View *
iregiongroupview_new( void )
{
	iRegiongroupview *iregiongroupview = 
		gtk_type_new( TYPE_IREGIONGROUPVIEW );

#ifdef DEBUG
	printf( "iregiongroupview_new\n" );
#endif /*DEBUG*/

	return( VIEW( iregiongroupview ) );
}
