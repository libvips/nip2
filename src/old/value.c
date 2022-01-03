/* an input value ... put/get methods
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

G_DEFINE_TYPE( Value, value, TYPE_CLASSMODEL ); 

static void
value_finalize( GObject *gobject )
{
	Value *value;

#ifdef DEBUG
	printf( "value_finalize\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_VALUE( gobject ) );

	value = VALUE( gobject );

	/* My instance finalize stuff.
	 */
	vips_buf_destroy( &value->caption_buffer );

	G_OBJECT_CLASS( value_parent_class )->finalize( gobject );
}

/* Default caption: just "class-name class.value".
 */
static const char *
value_generate_caption( iObject *iobject )
{
	Value *value = VALUE( iobject );
	ValueClass *value_class = VALUE_GET_CLASS( value );
	VipsBuf *buf = &value->caption_buffer;

	vips_buf_rewind( buf );
	if( !heapmodel_name( HEAPMODEL( value ), buf ) ) 
		vips_buf_appends( buf, G_OBJECT_CLASS_NAME( value_class ) );
	vips_buf_appends( buf, " " );
	heapmodel_value( HEAPMODEL( value ), buf );

	return( vips_buf_all( buf ) );
}

static View *
value_view_new( Model *model, View *parent )
{
	return( valueview_new() );
}

static void
value_class_init( ValueClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;

	/* Create signals.
	 */

	gobject_class->finalize = value_finalize;

	iobject_class->generate_caption = value_generate_caption;

	model_class->view_new = value_view_new;
}

static void
value_init( Value *value )
{
	vips_buf_init_dynamic( &value->caption_buffer, MAX_LINELENGTH );
}
