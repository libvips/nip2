/* display a region in a workspace
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

static iImageviewClass *parent_class = NULL;

static void
iregionview_destroy( GtkObject *object )
{
	iRegionview *iregionview;

#ifdef DEBUG
	printf( "iregionview_destroy\n" );
#endif /*DEBUG*/

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_IREGIONVIEW( object ) );

	iregionview = IREGIONVIEW( object );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
iregionview_class_init( iRegionviewClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = iregionview_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
iregionview_init( iRegionview *iregionview )
{
#ifdef DEBUG
	printf( "iregionview_init\n" );
#endif /*DEBUG*/
}

GtkType
iregionview_get_type( void )
{
	static GtkType iregionview_type = 0;

	if( !iregionview_type ) {
		static const GtkTypeInfo info = {
			"iRegionview",
			sizeof( iRegionview ),
			sizeof( iRegionviewClass ),
			(GtkClassInitFunc) iregionview_class_init,
			(GtkObjectInitFunc) iregionview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		iregionview_type = gtk_type_unique( TYPE_IIMAGEVIEW, &info );
	}

	return( iregionview_type );
}

View *
iregionview_new( void )
{
	iRegionview *iregionview = gtk_type_new( TYPE_IREGIONVIEW );

#ifdef DEBUG
	printf( "iregionview_new\n" );
#endif /*DEBUG*/

	return( VIEW( iregionview ) );
}
