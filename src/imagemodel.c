/* All the model stuff for an imageview window.
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

/* Our signals. 
 */
enum {
	SIG_IMAGEINFO_CHANGED,	/* Imageinfo we hold has been replaced */
	SIG_LAST
};

static iObjectClass *parent_class = NULL;

static guint imagemodel_signals[SIG_LAST] = { 0 };

void *
imagemodel_imageinfo_changed( Imagemodel *imagemodel )
{
#ifdef DEBUG
	printf( "imagemodel_imageinfo_changed: " );
	iobject_print( IOBJECT( imagemodel ) );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( imagemodel ), 
		imagemodel_signals[SIG_IMAGEINFO_CHANGED], 0 );

	return( NULL );
}

/* Is a state a paint state, ie. one which might alter the image? We warn
 * before going to one of these.
 */
gboolean
imagemodel_state_paint( ImagemodelState state )
{
	static gboolean state_paint[IMAGEMODEL_LAST] = {
		FALSE,		/* IMAGEMODEL_SELECT */
		FALSE,		/* IMAGEMODEL_PAN */
		FALSE,		/* IMAGEMODEL_MAGIN */
		FALSE,		/* IMAGEMODEL_MAGOUT */
		FALSE,		/* IMAGEMODEL_DROPPER */
		TRUE,		/* IMAGEMODEL_PEN */
		TRUE,		/* IMAGEMODEL_LINE */
		TRUE,		/* IMAGEMODEL_RECT */
		TRUE,		/* IMAGEMODEL_FLOOD */
		TRUE,		/* IMAGEMODEL_BLOB */
		TRUE,		/* IMAGEMODEL_TEXT */
		TRUE		/* IMAGEMODEL_SMUDGE */
	};

	assert( state < IMAGEMODEL_LAST );

	return( state_paint[state] );
}

#ifdef DEBUG
static const char *
imagemodel_state( ImagemodelState state )
{
	switch( state ) {
	case IMAGEMODEL_SELECT: 	return( "IMAGEMODEL_SELECT" );
	case IMAGEMODEL_PAN: 		return( "IMAGEMODEL_PAN" );
	case IMAGEMODEL_MAGIN: 		return( "IMAGEMODEL_MAGIN" );
	case IMAGEMODEL_MAGOUT:		return( "IMAGEMODEL_MAGOUT" );
	case IMAGEMODEL_DROPPER: 	return( "IMAGEMODEL_DROPPER" );
	case IMAGEMODEL_PEN: 		return( "IMAGEMODEL_PEN" );
	case IMAGEMODEL_LINE: 		return( "IMAGEMODEL_LINE" );
	case IMAGEMODEL_RECT: 		return( "IMAGEMODEL_RECT" );
	case IMAGEMODEL_FLOOD: 		return( "IMAGEMODEL_FLOOD" );
	case IMAGEMODEL_BLOB: 		return( "IMAGEMODEL_BLOB" );
	case IMAGEMODEL_TEXT: 		return( "IMAGEMODEL_TEXT" );
	case IMAGEMODEL_SMUDGE: 	return( "IMAGEMODEL_SMUDGE" );

	default:
		assert( FALSE );
	}
}
#endif /*DEBUG*/

static void
imagemodel_dispose( GObject *gobject )
{
	Imagemodel *imagemodel;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_IMAGEMODEL( gobject ) );

	imagemodel = IMAGEMODEL( gobject );

#ifdef DEBUG
	printf( "imagemodel_dispose: " );
	iobject_print( IOBJECT( imagemodel ) );
#endif /*DEBUG*/

	FREESID( imagemodel->iimage_changed_sid, imagemodel->iimage );
	FREESID( imagemodel->iimage_destroy_sid, imagemodel->iimage );
	FREESID( imagemodel->conv_changed_sid, imagemodel->conv );
	FREESID( imagemodel->conv_imageinfo_changed_sid, imagemodel->conv );
	UNREF( imagemodel->conv );
	MANAGED_UNREF( imagemodel->ink );
	IM_FREE( imagemodel->font_name );
	IM_FREE( imagemodel->text );
	MANAGED_UNREF( imagemodel->text_mask );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
imagemodel_finalize( GObject *gobject )
{
	Imagemodel *imagemodel;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_IMAGEMODEL( gobject ) );

	imagemodel = IMAGEMODEL( gobject );

#ifdef DEBUG
	printf( "imagemodel_finalize: " );
	iobject_print( IOBJECT( imagemodel ) );
#endif /*DEBUG*/

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
imagemodel_changed( iObject *iobject )
{
	Imagemodel *imagemodel = IMAGEMODEL( iobject );

#ifdef DEBUG
	printf( "imagemodel_changed: state = %s ", 
		imagemodel_state( imagemodel->state ) );
	iobject_print( IOBJECT( imagemodel ) );
#endif /*DEBUG*/

	conversion_set_params( imagemodel->conv,
		imagemodel->show_convert,
		imagemodel->scale, imagemodel->offset,
		imagemodel->falsecolour, imagemodel->type );

	/* Update prefs.
	 */
	prefs_set( "DISPLAY_RULERS", 
		"%s", bool_to_char( imagemodel->show_rulers ) );
	prefs_set( "DISPLAY_STATUS", 
		"%s", bool_to_char( imagemodel->show_status ) );
	prefs_set( "DISPLAY_CONVERSION", 
		"%s", bool_to_char( imagemodel->show_convert ) );

	/* If the paint bar is on, we want to be in synchronous paint
	 * mode. Even if we're not painting, we need this for
	 * undo/redo to work.
	 */
	conversion_set_synchronous( imagemodel->conv, 
		imagemodel->show_paintbox );

	IOBJECT_CLASS( parent_class )->changed( iobject );
}

static void
imagemodel_class_init( ImagemodelClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = imagemodel_dispose;
	gobject_class->finalize = imagemodel_finalize;

	iobject_class->changed = imagemodel_changed;

	imagemodel_signals[SIG_IMAGEINFO_CHANGED] = g_signal_new( 
		"imageinfo_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ImagemodelClass, imageinfo_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
}

/* Remake the ink image to match ii.
 */
static void
imagemodel_refresh_ink( Imagemodel *imagemodel, Imageinfo *ii )
{
	IMAGE *main_im = imageinfo_get( FALSE, ii );
	IMAGE *ink_im = imageinfo_get( FALSE, imagemodel->ink );

	if( ink_im &&
		ink_im->Bands == main_im->Bands &&
		ink_im->BandFmt == main_im->BandFmt &&
		ink_im->Coding == main_im->Coding &&
		ink_im->Type == main_im->Type ) 
		return;

	MANAGED_UNREF( imagemodel->ink );

	if( (imagemodel->ink = imageinfo_new_temp( 
		main_imageinfogroup, 
		reduce_context->heap, NULL, "t" )) ) {
		MANAGED_REF( imagemodel->ink );
		im_initdesc( imagemodel->ink->im, 
			1, 1, main_im->Bands, 
			main_im->Bbits, main_im->BandFmt, 
			main_im->Coding, main_im->Type, 
			1.0, 1.0, 0, 0 );
		if( im_setupout( imagemodel->ink->im ) ) 
			MANAGED_UNREF( imagemodel->ink );
	}

	if( imagemodel->ink && imagemodel->ink->im && 
		imagemodel->ink->im->data )
		memset( imagemodel->ink->im->data, 0, 
			IM_IMAGE_SIZEOF_LINE( imagemodel->ink->im ) );
}

static void
imagemodel_conv_changed_cb( Conversion *conv, Imagemodel *imagemodel )
{
	imagemodel_refresh_ink( imagemodel, conv->ii );

	iobject_changed( IOBJECT( imagemodel ) );
}

static void
imagemodel_conv_imageinfo_changed_cb( Conversion *conv, Imagemodel *imagemodel )
{
	imagemodel_imageinfo_changed( imagemodel );
}

static void
imagemodel_init( Imagemodel *imagemodel )
{
	imagemodel->conv = conversion_new( NULL );
	g_object_ref( G_OBJECT( imagemodel->conv ) );
	iobject_sink( IOBJECT( imagemodel->conv ) );

	imagemodel->conv_changed_sid = g_signal_connect( 
		G_OBJECT( imagemodel->conv ), "changed", 
		G_CALLBACK( imagemodel_conv_changed_cb ), imagemodel );
	imagemodel->conv_imageinfo_changed_sid = g_signal_connect( 
		G_OBJECT( imagemodel->conv ), "imageinfo_changed", 
		G_CALLBACK( imagemodel_conv_imageinfo_changed_cb ), 
		imagemodel );

	imagemodel->conv->priority = 1;

	imagemodel->show_rulers = DISPLAY_RULERS;
	imagemodel->rulers_mm = FALSE;
	imagemodel->rulers_offset = FALSE;

	imagemodel->show_status = DISPLAY_STATUS;

	imagemodel->show_paintbox = FALSE;
	imagemodel->nib = PAINTBOX_1ROUND;
	imagemodel->ink = NULL;
	imagemodel->font_name = im_strdup( NULL, PAINTBOX_FONT );
	imagemodel->text = NULL;
	imagemodel->text_mask = NULL;

	imagemodel->show_convert = DISPLAY_CONVERSION;
	imagemodel->scale = 1.0;
	imagemodel->offset = 0.0;
	imagemodel->falsecolour = FALSE;
	imagemodel->type = TRUE;
}

GType
imagemodel_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ImagemodelClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) imagemodel_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Imagemodel ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) imagemodel_init,
		};

		type = g_type_register_static( TYPE_IOBJECT, 
			"Imagemodel", &info, 0 );
	}

	return( type );
}

static void
imagemodel_iimage_changed_cb( iImage *iimage, Imagemodel *imagemodel )
{
	conversion_set_image( imagemodel->conv, iimage->value.ii );
}

static void
imagemodel_iimage_destroy_cb( iImage *iimage, Imagemodel *imagemodel )
{
	iobject_destroy( IOBJECT( imagemodel ) );
}

static void
imagemodel_link( Imagemodel *imagemodel, iImage *iimage )
{
	Row *row = HEAPMODEL( iimage )->row;

	imagemodel->iimage = iimage;
	imagemodel->iimage_changed_sid = g_signal_connect( G_OBJECT( iimage ), 
		"changed", 
		G_CALLBACK( imagemodel_iimage_changed_cb ), imagemodel );
	imagemodel->iimage_destroy_sid = g_signal_connect( G_OBJECT( iimage ), 
		"destroy", 
		G_CALLBACK( imagemodel_iimage_destroy_cb ), imagemodel );
	imagemodel->scale = row->ws->scale;
	imagemodel->offset = row->ws->offset;

	/* Install image.
	 */
	conversion_set_image( imagemodel->conv, iimage->value.ii );

	/* Set name ... handy for debugging.
	 */
	iobject_set( IOBJECT( imagemodel ), 
		row_name( HEAPMODEL( iimage )->row ), NULL );
}

Imagemodel *
imagemodel_new( iImage *iimage )
{
	Imagemodel *imagemodel;

	imagemodel = g_object_new( TYPE_IMAGEMODEL, NULL );
	imagemodel_link( imagemodel, iimage );

#ifdef DEBUG
	printf( "imagemodel_new: " );
	iobject_print( IOBJECT( imagemodel ) );
#endif /*DEBUG*/

	return( imagemodel );
}

/* Callback for check_paintable() in imagemodel_set_state. 
 */
static void
imagemodel_set_paintbox_cb( void *client, iWindowResult result )
{
	Imagemodel *imagemodel = IMAGEMODEL( client );

#ifdef DEBUG
	printf( "imagemodel_set_paintbox_cb: pend_state = %s\n", 
		imagemodel_state( imagemodel->pend_state ) );
#endif /*DEBUG*/

	if( result == IWINDOW_TRUE ) {
		imagemodel_set_paintbox( imagemodel, TRUE );

		if( imagemodel->state != imagemodel->pend_state ) {
			imagemodel->state = imagemodel->pend_state;
			iobject_changed( IOBJECT( imagemodel ) );
		}
	}
}

/* Set the viewer state. We can't always do this immediately, we may need to
 * ask the user if the change is OK. Return TRUE if we were able to make the
 * change now.
 */
gboolean
imagemodel_set_state( Imagemodel *imagemodel, ImagemodelState state, 
	GtkWidget *parent )
{
	gboolean changed = FALSE;

#ifdef DEBUG
	printf( "imagemodel_set_state: %s\n", imagemodel_state( state ) );
#endif /*DEBUG*/

	if( state != imagemodel->state && imagemodel_state_paint( state ) ) {
		/* Check and warn on this image first.
		 */
		imagemodel->pend_state = state;
		imageinfo_check_paintable( imagemodel->conv->ii, 
			parent, imagemodel_set_paintbox_cb, imagemodel );

		/* We may not have set the state yet ... signal "changed" 
		 * to flick whatever asked for this change (eg. View
		 * menu) back to the old state.
		 */
		changed = TRUE;
	}
	else if( state != imagemodel->state ) {
		imagemodel->state = state;
		changed = TRUE;
	}

	if( changed )
		iobject_changed( IOBJECT( imagemodel ) );

	return( imagemodel->state == state );
}

void
imagemodel_set_rulers( Imagemodel *imagemodel, gboolean show_rulers )
{
	if( imagemodel->show_rulers != show_rulers ) {
		imagemodel->show_rulers = show_rulers;
		iobject_changed( IOBJECT( imagemodel ) );
	}
}

void
imagemodel_set_paintbox( Imagemodel *imagemodel, gboolean show_paintbox )
{
	if( imagemodel->show_paintbox != show_paintbox ) {
#ifdef DEBUG
		printf( "imagemodel_set_paintbox: " );
		iobject_print( IOBJECT( imagemodel ) );
#endif /*DEBUG*/

		imagemodel->show_paintbox = show_paintbox;

		/* If the paint bar is off, we want to not be in a paint mode.
		 */
		if( !imagemodel->show_paintbox && 
			imagemodel_state_paint( imagemodel->state ) )
			imagemodel_set_state( imagemodel, 
				IMAGEMODEL_SELECT, NULL );

		iobject_changed( IOBJECT( imagemodel ) );
	}
}

void
imagemodel_set_status( Imagemodel *imagemodel, gboolean show_status )
{
	if( imagemodel->show_status != show_status ) {
		imagemodel->show_status = show_status;
		iobject_changed( IOBJECT( imagemodel ) );
	}
}

void
imagemodel_set_convert( Imagemodel *imagemodel, gboolean show_convert )
{
	if( imagemodel->show_convert != show_convert ) {
		imagemodel->show_convert = show_convert;
		iobject_changed( IOBJECT( imagemodel ) );
	}
}

/* Update the text_mask. imagemodel->text is kept up to date with what's in the
 * paintbox text widget, call this just before a paint action to render the
 * mask.
 */
gboolean
imagemodel_refresh_text( Imagemodel *imagemodel )
{
	const char *text = imagemodel->text;

	if( !text || strspn( text, WHITESPACE ) == strlen( text ) ) {
		error_top( _( "No text specified." ) );
		error_sub( _( "Enter some text to paint in the entry widget at "
			"the top of the window." ) );
		return( FALSE );
	}

	MANAGED_UNREF( imagemodel->text_mask );

	if( !(imagemodel->text_mask = imageinfo_new_temp( main_imageinfogroup, 
			reduce_context->heap, NULL, "t" )) ) 
		return( FALSE );

	MANAGED_REF( imagemodel->text_mask );

	if( !imageinfo_paint_text( imagemodel->text_mask, 
		imagemodel->font_name, imagemodel->text, 
		&imagemodel->text_area ) )
		return( FALSE );

	return( TRUE );
}

/* After a paint action: mark all subsequent things dirty, recalc if prefs say
 * so.
 */
void
imagemodel_paint_recalc( Imagemodel *imagemodel )
{
	Classmodel *classmodel = CLASSMODEL( imagemodel->iimage );
	Row *row = HEAPMODEL( classmodel )->row;

#ifdef DEBUG
	printf( "imagemodel_paint_recalc: " );
	iobject_print( IOBJECT( imagemodel ) );
#endif /*DEBUG*/

	expr_dirty_intrans( row->expr, link_serial_new() );

	if( PAINTBOX_RECOMP )
		symbol_recalculate_all();
}
