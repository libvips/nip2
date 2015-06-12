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

G_DEFINE_TYPE( iRegionview, iregionview, TYPE_IIMAGEVIEW ); 

static void
iregionview_class_init( iRegionviewClass *class )
{
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

View *
iregionview_new( void )
{
	iRegionview *iregionview = g_object_new( TYPE_IREGIONVIEW, NULL );

#ifdef DEBUG
	printf( "iregionview_new\n" );
#endif /*DEBUG*/

	return( VIEW( iregionview ) );
}
