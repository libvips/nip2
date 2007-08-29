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
stringview_scan( View *view )
{
	Stringview *stringview = STRINGVIEW( view );
	String *string = STRING( VOBJECT( stringview )->iobject );
    	Expr *expr = HEAPMODEL( string )->row->expr;
	char value[MAX_STRSIZE];
	char value2[MAX_STRSIZE];

#ifdef DEBUG
	Row *row = HEAPMODEL( string )->row;

	printf( "stringview_scan: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	expr_error_clear( expr );

	if( !get_geditable_string( EDITVIEW( stringview )->text, 
		value, MAX_STRSIZE ) ) {
		expr_error_set( expr );
		return( view );
	}
	my_strccpy( value2, value );

	if( strcmp( string->value, value2 ) != 0 ) {
		IM_SETSTR( string->value, value2 );
		classmodel_update( CLASSMODEL( string ) ) ;
	}

	return( VIEW_CLASS( parent_class )->scan( view ) );
}

static void 
stringview_refresh( vObject *vobject )
{
	Stringview *stringview = STRINGVIEW( vobject );
	String *string = STRING( VOBJECT( stringview )->iobject );

#ifdef DEBUG
	Row *row = HEAPMODEL( string )->row;

	printf( "stringview_refresh: " );
	row_name_print( row );
	printf( " (%p)\n", view );
#endif /*DEBUG*/

	if( string->value ) {
		char txt[MAX_STRSIZE];
		BufInfo buf;

		buf_init_static( &buf, txt, MAX_STRSIZE );
		buf_appendsc( &buf, FALSE, string->value );
		editview_set_entry( EDITVIEW( stringview ), 
			"%s", buf_all( &buf ) );
	}

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
stringview_class_init( StringviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = stringview_refresh;

	view_class->scan = stringview_scan;
}

static void
stringview_init( Stringview *stringview )
{
}

GtkType
stringview_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Stringview",
			sizeof( Stringview ),
			sizeof( StringviewClass ),
			(GtkClassInitFunc) stringview_class_init,
			(GtkObjectInitFunc) stringview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_EDITVIEW, &info );
	}

	return( type );
}

View *
stringview_new( void )
{
	Stringview *stringview = gtk_type_new( TYPE_STRINGVIEW );

	return( VIEW( stringview ) );
}
