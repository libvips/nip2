/* a view of a text thingy
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

static EditviewClass *parent_class = NULL;

/* Re-read the text in a tally entry. 
 */
static void *
numberview_scan( View *view )
{
	Numberview *numberview = NUMBERVIEW( view );
	Number *number = NUMBER( VOBJECT( numberview )->iobject );
    	Expr *expr = HEAPMODEL( number )->row->expr;
	double value;

#ifdef DEBUG
	Row *row = HEAPMODEL( number )->row;

	printf( "numberview_scan: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	expr_error_clear( expr );

	if( !get_geditable_double( EDITVIEW( numberview )->text, &value ) ) {
		expr_error_set( expr );
		return( view );
	}

	if( number->value != value ) {
		number->value = value;
		classmodel_update( CLASSMODEL( number ) ) ;
	}

	return( VIEW_CLASS( parent_class )->scan( view ) );
}

static void 
numberview_refresh( vObject *vobject )
{
	Numberview *numberview = NUMBERVIEW( vobject );
	Number *number = NUMBER( VOBJECT( numberview )->iobject );

#ifdef DEBUG
	Row *row = HEAPMODEL( number )->row;

	printf( "numberview_refresh: " );
	row_name_print( row );
	printf( " (%p)\n", vobject );
#endif /*DEBUG*/

	editview_set_entry( EDITVIEW( numberview ), 
		"%g", number->value );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
numberview_class_init( NumberviewClass *class )
{
	ViewClass *view_class = (ViewClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = numberview_refresh;

	view_class->scan = numberview_scan;
}

static void
numberview_init( Numberview *numberview )
{
}

GtkType
numberview_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Numberview",
			sizeof( Numberview ),
			sizeof( NumberviewClass ),
			(GtkClassInitFunc) numberview_class_init,
			(GtkObjectInitFunc) numberview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_EDITVIEW, &info );
	}

	return( type );
}

View *
numberview_new( void )
{
	Numberview *numberview = gtk_type_new( TYPE_NUMBERVIEW );

	return( VIEW( numberview ) );
}
