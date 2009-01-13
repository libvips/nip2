/* an ip arrow class object in a workspace
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
iarrow_finalize( GObject *gobject )
{
	iArrow *iarrow;

#ifdef DEBUG
	printf( "iarrow_finalize\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_IARROW( gobject ) );

	iarrow = IARROW( gobject );

	/* My instance finalize stuff.
	 */
	iregion_instance_destroy( &iarrow->instance );
	vips_buf_destroy( &iarrow->caption_buffer );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void *
iarrow_generate_caption_sub( iImage *iimage, iArrow *iarrow, gboolean *first )
{
	Workspace *ws = HEAPMODEL( iarrow )->row->ws;
	Row *row = HEAPMODEL( iimage )->row;

	/* Suppress this name in the caption if it's a superclass. If this
	 * thing is on a super, it's on the subclass too ... not helpful to
	 * have it twice.
	 */
	if( !is_super( row->sym ) ) {
		if( *first )
			*first = FALSE;
		else 
			vips_buf_appends( &iarrow->caption_buffer, ", " );

		row_qualified_name_relative( ws->sym, row,
			&iarrow->caption_buffer );
	}

	return( NULL );
}

static const char *
iarrow_generate_caption( iObject *iobject )
{
	static const char *names_current[] = {
		CLASS_HGUIDE,
		CLASS_VGUIDE,
		CLASS_MARK,
		CLASS_ARROW,
		NULL
	};

	/* 7.8 used slightly different names :-(
	 */
	static const char *names_compatibility[] = {
		CLASS_HGUIDE,
		CLASS_VGUIDE,
		CLASS_POINT,
		CLASS_ARROW,
		NULL
	};

	iArrow *iarrow = IARROW( iobject );
	VipsBuf *buf = &iarrow->caption_buffer;
	const int nimages = g_slist_length( CLASSMODEL( iarrow )->iimages );
	Expr *expr;
	gboolean result;
	const char **names;
	gboolean first;
	int i;

	if( !HEAPMODEL( iarrow )->row ||
		!(expr = HEAPMODEL( iarrow )->row->expr) || 
		!heap_is_class( &expr->root, &result ) || 
		!result )
		return( _( "No image" ) );

	if( HEAPMODEL( iarrow )->row->ws->compat_78 ) 
		names = names_compatibility;
	else
		names = names_current;

	vips_buf_rewind( buf );
	heapmodel_name( HEAPMODEL( iarrow ), buf );
	vips_buf_appendf( buf, " " );

	/* Used in (eg.) "Mark at (10, 10) on [A1, A2]"
	 */
	vips_buf_appendf( buf, _( "on" ) );
	vips_buf_appendf( buf, " " );
	if( nimages > 1 )
		vips_buf_appendf( buf, "[" );
	first = TRUE;
	slist_map2( CLASSMODEL( iarrow )->iimages,
		(SListMap2Fn) iarrow_generate_caption_sub, iarrow, &first );
	if( nimages > 1 )
		vips_buf_appendf( buf, "]" );
	vips_buf_appendf( buf, " " );

	for( i = 0; names[i]; i++ ) {
		if( !heap_is_instanceof( names[i], &expr->root, &result ) )
			break;

		if( result ) {
			switch( i ) {
			case 0:
				vips_buf_appendf( buf, _( "at %d" ),
					iarrow->instance.area.top );
				break;

			case 1:
				vips_buf_appendf( buf, _( "at %d" ),
					iarrow->instance.area.left );
				break;

			case 2:
				vips_buf_appendf( buf, _( "at (%d, %d)" ),
					iarrow->instance.area.left,
					iarrow->instance.area.top );
				break;

			case 3:
				vips_buf_appendf( buf, 
					_( "at (%d, %d), offset (%d, %d)" ),
					iarrow->instance.area.left, 
					iarrow->instance.area.top,
					iarrow->instance.area.width, 
					iarrow->instance.area.height );
				break;

			default:
				g_assert( 0 );
			}

			break;
		}
	}

	return( vips_buf_all( buf ) );
}

static View *
iarrow_view_new( Model *model, View *parent )
{
	return( valueview_new() );
}

static void *
iarrow_update_model( Heapmodel *heapmodel )
{
	/* Parent first ... this will update our instance vars.
	 */
	if( HEAPMODEL_CLASS( parent_class )->update_model( heapmodel ) )
		return( heapmodel );

	if( heapmodel->row->expr ) {
		iArrow *iarrow = IARROW( heapmodel );

		/* Update who-has-displays-on-what stuff.
		 */
		classmodel_iimage_update( CLASSMODEL( iarrow ), 
			iarrow->instance.ii );

		/* Need to make sure the caption is regenerated.
		 */
		iobject_changed( IOBJECT( heapmodel ) );
	}

	return( NULL );
}

static void *
iarrow_get_instance( Classmodel *classmodel )
{
	iArrow *iarrow = IARROW( classmodel );

	return( &iarrow->instance );
}

static void
iarrow_class_init( iArrowClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	/* We share methods with iregion in a sort of MI way ... iregion needs
	 * to be initialised before we can work. Force it to start up.
	 */
	(void) iregion_get_type();

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->finalize = iarrow_finalize;

	iobject_class->generate_caption = iarrow_generate_caption;

	icontainer_class->parent_add = iregion_parent_add;

	model_class->view_new = iarrow_view_new;
	model_class->edit = iregion_edit;
	model_class->save = iregion_save;
	model_class->load = iregion_load;

	heapmodel_class->update_model = iarrow_update_model;
	heapmodel_class->update_heap = iregion_update_heap;

	classmodel_class->class_get = iregion_class_get;
	classmodel_class->class_new = iregion_class_new;
	classmodel_class->get_instance = iarrow_get_instance;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
iarrow_init( iArrow *iarrow )
{
	iregion_instance_init( &iarrow->instance, CLASSMODEL( iarrow ) );
	vips_buf_init_dynamic( &iarrow->caption_buffer, MAX_LINELENGTH );

	iobject_set( IOBJECT( iarrow ), CLASS_ARROW, NULL );
}

GType
iarrow_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( iArrowClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) iarrow_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( iArrow ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) iarrow_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"iArrow", &info, 0 );
	}

	return( type );
}
