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

static ClassmodelClass *parent_class = NULL;

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
	buf_destroy( &value->caption_buffer );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

/* Default caption: just "class-name class.value".
 */
static const char *
value_generate_caption( iObject *iobject )
{
	Value *value = VALUE( iobject );
	ValueClass *value_class = VALUE_GET_CLASS( value );
	BufInfo *buf = &value->caption_buffer;

	buf_rewind( buf );
	if( !heapmodel_name( HEAPMODEL( value ), buf ) ) 
		buf_appends( buf, G_OBJECT_CLASS_NAME( value_class ) );
	buf_appends( buf, " " );
	heapmodel_value( HEAPMODEL( value ), buf );

	return( buf_all( buf ) );
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

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	gobject_class->finalize = value_finalize;

	iobject_class->generate_caption = value_generate_caption;

	model_class->view_new = value_view_new;
}

static void
value_init( Value *value )
{
	buf_init_dynamic( &value->caption_buffer, MAX_LINELENGTH );
}

GType
value_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ValueClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) value_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Value ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) value_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Value", &info, 0 );
	}

	return( type );
}
