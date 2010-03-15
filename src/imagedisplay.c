/* Imagedisplay widget code ... display entire image, place this widget in a * scrolledwindow to get clipping/scrolling behaviour.
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

/* Trace painting actions
#define DEBUG_PAINT
 */

/*
#define DEBUG_GEO
 */

#include "ip.h"

enum {
	SIG_AREA_CHANGED,	/* xywh area changed, canvas cods */
	SIG_LAST
};

static GtkDrawingAreaClass *parent_class = NULL;

static guint imagedisplay_signals[SIG_LAST] = { 0 };

/* Handy!
 */
void
imagedisplay_queue_draw_area( Imagedisplay *id, Rect *area )
{
#ifdef DEBUG_PAINT
	printf( "imagedisplay_queue_draw_area: "
		"left = %d, top = %d, width = %d, height = %d\n",
		area->left, area->top, area->width, area->height );
#endif /*DEBUG_PAINT*/

	gtk_widget_queue_draw_area( GTK_WIDGET( id ),
		area->left, area->top, area->width, area->height ); 
}

/* Repaint an area of the image.
 */
static void
imagedisplay_paint_image( Imagedisplay *id, Rect *area )
{
	Conversion *conv = id->conv;

	guchar *buf;
	int lsk;

#ifdef DEBUG_PAINT
	g_print( "imagedisplay_paint_image: at %d x %d, size %d x %d ",
		area->left, area->top, area->width, area->height );
	gobject_print( G_OBJECT( id ) );
#endif /*DEBUG_PAINT*/

	/* Request pixels. We ask the mask first, to get an idea of what's
	 * currently in cache, then request tiles of pixels. We must always
	 * request pixels, even if the mask is blank, because the request
	 * will trigger a notify later which will reinvoke us.
	 */
	if( conv->mreg &&
		im_prepare( conv->mreg, area ) ) {
#ifdef DEBUG_PAINT
		printf( "imagedisplay_paint_image: mask paint error\n" );
		printf( "\t%s\n", im_error_buffer() );
#endif /*DEBUG_PAINT*/

		return;
	}
	if( im_prepare( conv->ireg, area ) ) {
#ifdef DEBUG_PAINT
		printf( "imagedisplay_paint_image: paint error\n" );
		printf( "\t%s\n", im_error_buffer() );
#endif /*DEBUG_PAINT*/

		im_error_clear();

		return;
	}

	/* Is the mask all zero? Skip the paint.
	 */
	if( conv->mreg ) {
		gboolean found;
		int x, y;

		buf = (guchar *) 
			IM_REGION_ADDR( conv->mreg, area->left, area->top );
		lsk = IM_REGION_LSKIP( conv->mreg );
		found = FALSE;

		for( y = 0; y < area->height; y++ ) {
			for( x = 0; x < area->width; x++ )
				if( buf[x] ) {
					found = TRUE;
					break;
				}

			if( found )
				break;

			buf += lsk;
		}

		if( !found ) {
#ifdef DEBUG_PAINT
			printf( "imagedisplay_paint_image: zero mask\n" );
#endif /*DEBUG_PAINT*/

			return;
		}
	}

	/* Paint into window.
	 */

	buf = (guchar *) IM_REGION_ADDR( conv->ireg, area->left, area->top );
	lsk = IM_REGION_LSKIP( conv->ireg );

	if( conv->ireg->im->Bands == 3 )
		gdk_draw_rgb_image( GTK_WIDGET( id )->window,
			GTK_WIDGET( id )->style->white_gc,
			area->left, area->top, area->width, area->height,
			GDK_RGB_DITHER_MAX,
			buf, lsk );
	else if( conv->ireg->im->Bands == 1 )
		gdk_draw_gray_image( GTK_WIDGET( id )->window,
			GTK_WIDGET( id )->style->white_gc,
			area->left, area->top, area->width, area->height,
			GDK_RGB_DITHER_MAX,
			buf, lsk );
}

/* Paint an area with the background pattern.
 */
static void
imagedisplay_paint_background( Imagedisplay *id, Rect *expose )
{
#ifdef DEBUG_PAINT
	g_print( "imagedisplay_paint_background: at %d x %d, size %d x %d\n",
		expose->left, expose->top, expose->width, expose->height );
#endif /*DEBUG_PAINT*/

	gdk_draw_rectangle( GTK_WIDGET( id )->window, 
		id->back_gc, TRUE,
		expose->left, expose->top, expose->width, expose->height );
}

/* Paint areas outside the image.
 */
static void
imagedisplay_paint_background_clipped( Imagedisplay *id, Rect *expose )
{
	Conversion *conv = id->conv;
	Rect clip;

#ifdef DEBUG_PAINT
	g_print( "imagedisplay_paint_background_clipped: canvas %d x %d\n",
		conv->canvas.width, conv->canvas.height );
#endif /*DEBUG_PAINT*/

	/* If the expose touches the image, we cut it into two parts:
	 * everything to the right of the image, and everything strictly
	 * below.
	 */
	im_rect_intersectrect( expose, &conv->canvas, &clip );
	if( !im_rect_isempty( &clip ) ) {
		Rect area;

		area = *expose;
		area.left = conv->canvas.width;
		area.width -= clip.width;
		if( area.width > 0 )
			imagedisplay_paint_background( id, &area );

		area = *expose;
		area.top = conv->canvas.height;
		area.width = clip.width;
		area.height -= clip.height;
		if( area.height > 0 )
			imagedisplay_paint_background( id, &area );
	}
	else
		imagedisplay_paint_background( id, expose );
}

static void
imagedisplay_paint( Imagedisplay *id, Rect *area )
{
	Conversion *conv = id->conv;
	const int tsize = conv->tile_size;

	Rect clip;
	int xs, ys;
	int x, y;

	/* Clip non-image parts of the expose.
	 */
	im_rect_intersectrect( area, &conv->canvas, &clip );
	if( im_rect_isempty( &clip ) )
		return;

#ifdef DEBUG_PAINT
	g_print( "imagedisplay_paint: at %d x %d, size %d x %d\n",
		clip.left, clip.top, clip.width, clip.height );
#endif /*DEBUG_PAINT*/

	/* Round left/top down to the start tile.
	 */
	xs = (clip.left / tsize) * tsize;
	ys = (clip.top / tsize) * tsize;

	/* Now loop painting image tiles.
	 */
	for( y = ys; y < IM_RECT_BOTTOM( &clip ); y += tsize )
		for( x = xs; x < IM_RECT_RIGHT( &clip ); x += tsize ) {
			Rect tile;
			Rect tile2;

			tile.left = x;
			tile.top = y;
			tile.width = conv->tile_size;
			tile.height = conv->tile_size;
			im_rect_intersectrect( &tile, &clip, &tile2 );

			imagedisplay_paint_image( id, &tile2 );
		}
}

/* Expose signal handler.
 */
static gint
imagedisplay_expose( GtkWidget *widget, GdkEventExpose *event )
{
	Imagedisplay *id = IMAGEDISPLAY( widget );
	Conversion *conv = id->conv;

	GdkRectangle *rect;
	int i, n;

	if( !GTK_WIDGET_DRAWABLE( id ) ||
		event->area.width == 0 || 
		event->area.height == 0 ||
		!GTK_WIDGET( id )->window ||
		!GTK_WIDGET_VISIBLE( id ) ||
		!conv->ireg )
		return( FALSE );

	gdk_region_get_rectangles( event->region, &rect, &n );
#ifdef DEBUG_PAINT
	g_print( "imagedisplay_expose: %d rectangles\n", n ); 
#endif /*DEBUG_PAINT*/
	for( i = 0; i < n; i++ ) {
		Rect area;

		area.left = rect[i].x;
		area.top = rect[i].y;
		area.width = rect[i].width;
		area.height = rect[i].height;

		/* Clear to background. Always do this, to make sure we paint 
		 * outside the image area.
		 */
		imagedisplay_paint_background_clipped( id, &area );

		/* And paint pixels.
		 */
		imagedisplay_paint( id, &area );
	}
	g_free( rect );

        return( FALSE );
}

/* Resize signal.
 */
static gboolean
imagedisplay_configure_event( GtkWidget *widget, GdkEventConfigure *event )
{
	Imagedisplay *id = IMAGEDISPLAY( widget );

#ifdef DEBUG_GEO
	g_print( "imagedisplay_configure_event: %d x %d:\n", 
		event->width, event->height );
#endif /*DEBUG_GEO*/

	/* Note new size in visible hint. Except if parent is a viewport ...
	 * if it's a viewport, someone else will have to track the visible
	 * area.
	 */
	if( !GTK_IS_VIEWPORT( gtk_widget_get_parent( widget ) ) ) { 
		id->conv->visible.width = event->width;
		id->conv->visible.height = event->height;
	}

	/* Recalculate shrink to fit, if necessary.
	 */
	if( id->shrink_to_fit ) {
#ifdef DEBUG_GEO
		g_print( "imagedisplay_configure_event_cb: shrink-to-fit\n" );
#endif /*DEBUG_GEO*/

		conversion_set_mag( id->conv, 0 );
	}

        return( FALSE );
}

static void
imagedisplay_destroy( GtkObject *object )
{
	Imagedisplay *id = IMAGEDISPLAY( object );

#ifdef DEBUG
	g_print( "imagedisplay_destroy: " );
	gobject_print( G_OBJECT( id ) );
#endif /*DEBUG*/

	FREESID( id->changed_sid, id->conv );
	FREESID( id->area_changed_sid, id->conv );
	UNREF( id->conv );

	UNREF( id->back_gc );
	UNREF( id->top_gc );
	UNREF( id->bottom_gc );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

/* Conversion has changed ... resize to fit.
 */
static void
imagedisplay_real_conversion_changed( Imagedisplay *id )
{
	GtkRequisition *requisition = &GTK_WIDGET( id )->requisition;
	Rect *canvas = &id->conv->canvas;

	g_assert( IS_IMAGEDISPLAY( id ) );

#ifdef DEBUG
	g_print( "imagedisplay_real_conversion_changed: " );
	gobject_print( G_OBJECT( id ) );
#endif /*DEBUG*/

	/* If we're in shrink-to-fit mode, do a shrink.
	 * Otherwise resize to hold the new image.
	 */
	if( id->shrink_to_fit )
		conversion_set_mag( id->conv, 0 );
	else if( requisition->width != canvas->width ||
		requisition->height != canvas->height ) {
#ifdef DEBUG_GEO
		g_print( "imagedisplay_real_conversion_"
			"changed: requesting new size "
			"%d x %d\n",
			id->conv->canvas.width,
			id->conv->canvas.height );
#endif /*DEBUG_GEO*/

		requisition->width = canvas->width;
		requisition->height = canvas->height;
		gtk_widget_queue_resize( GTK_WIDGET( id ) );
	}
}

static void
imagedisplay_real_area_changed( Imagedisplay *id, Rect *dirty )
{
	imagedisplay_queue_draw_area( id, dirty );
}

static void
imagedisplay_realize( GtkWidget *widget )
{
	GTK_WIDGET_CLASS( parent_class )->realize( widget );

	gdk_window_set_back_pixmap( widget->window, NULL, FALSE );
	gtk_widget_set_double_buffered( widget, FALSE );
}

/* Init Imagedisplay class.
 */
static void
imagedisplay_class_init( ImagedisplayClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
        GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	parent_class = g_type_class_peek_parent( class );

        object_class->destroy = imagedisplay_destroy;

	widget_class->expose_event = imagedisplay_expose;
	widget_class->configure_event = imagedisplay_configure_event;
	widget_class->realize = imagedisplay_realize;

	class->conversion_changed = imagedisplay_real_conversion_changed;
	class->area_changed = imagedisplay_real_area_changed;

	imagedisplay_signals[SIG_AREA_CHANGED] = g_signal_new( "area_changed",
		G_OBJECT_CLASS_TYPE( class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ImagedisplayClass, area_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		G_TYPE_POINTER );
}

static void
imagedisplay_make_gcs( Imagedisplay *id )
{
	/* Background stipple.
	 */
	static guchar stipple[8] = {
		0xF0,    /*  ####----  */
		0xE1,    /*  ###----#  */
		0xC3,    /*  ##----##  */
		0x87,    /*  #----###  */
		0x0F,    /*  ----####  */
		0x1E,    /*  ---####-  */
		0x3C,    /*  --####--  */
		0x78,    /*  -####---  */
	};

	GdkGCValues values;
	GdkColor fg, bg;

	if( id->back_gc )
		return;

	values.fill = GDK_OPAQUE_STIPPLED;
	values.stipple = gdk_bitmap_create_from_data( main_window_gdk,
		(char *) stipple, 8, 8 );
	id->back_gc = gdk_gc_new_with_values( main_window_gdk, &values,
		GDK_GC_FILL | GDK_GC_STIPPLE );
	fg.red = fg.green = fg.blue = 0x90 << 8;
	bg.red = bg.green = bg.blue = 0xA0 << 8;
	gdk_gc_set_rgb_fg_color( id->back_gc, &fg );
	gdk_gc_set_rgb_bg_color( id->back_gc, &bg );

	id->top_gc = gdk_gc_new( main_window_gdk );
	id->bottom_gc = gdk_gc_new( main_window_gdk );
}

static void
imagedisplay_init( Imagedisplay *id )
{
	id->conv = NULL;
	id->changed_sid = 0;
	id->area_changed_sid = 0;
	id->shrink_to_fit = FALSE;

	id->back_gc = NULL;
	id->top_gc = NULL;
	id->bottom_gc = NULL;

	imagedisplay_make_gcs( id );
}

GType
imagedisplay_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ImagedisplayClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) imagedisplay_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Imagedisplay ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) imagedisplay_init,
		};

		type = g_type_register_static( GTK_TYPE_DRAWING_AREA, 
			"Imagedisplay", &info, 0 );
	}

	return( type );
}

/* Conversion has changed ... repaint everything.
 */
static void
imagedisplay_conversion_changed_cb( Conversion *conv, Imagedisplay *id )
{
#ifdef DEBUG
	printf( "imagedisplay_conversion_changed_cb: " );
	gobject_print( G_OBJECT( id ) );
#endif /*DEBUG*/

	IMAGEDISPLAY_GET_CLASS( id )->conversion_changed( id );

	g_signal_emit( G_OBJECT( id ), 
		imagedisplay_signals[SIG_AREA_CHANGED], 0, &conv->canvas );
}

/* Part of the repaint has changed. 
 */
static void
imagedisplay_conversion_area_changed_cb( Conversion *conv, 
	Rect *dirty, Imagedisplay *id )
{
#ifdef DEBUG
	printf( "imagedisplay_conversion_area_changed_cb: " 
		"left = %d, top = %d, width = %d, height = %d, ",
		dirty->left, dirty->top, dirty->width, dirty->height );
	gobject_print( G_OBJECT( id ) );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( id ), 
		imagedisplay_signals[SIG_AREA_CHANGED], 0, dirty );
}

/* Install a conversion. Only allow this once.
 */
void
imagedisplay_set_conversion( Imagedisplay *id, Conversion *conv )
{
	g_assert( !id->conv );

	if( conv ) {
		id->conv = conv;
		id->changed_sid = g_signal_connect( id->conv, "changed", 
			G_CALLBACK( imagedisplay_conversion_changed_cb ), id );
		id->area_changed_sid = g_signal_connect( id->conv, 
			"area_changed", 
			G_CALLBACK( imagedisplay_conversion_area_changed_cb ), 
			id );
		g_object_ref( G_OBJECT( conv ) );
		iobject_sink( IOBJECT( conv ) );

		/* Trigger a change on the conv so we update.
		 */
		iobject_changed( IOBJECT( conv ) );
	}
}

/* Make a new Imagedisplay. Pass in the conversion we should show, conv can
 * be NULL ... wait for one to be installed.
 */
Imagedisplay *
imagedisplay_new( Conversion *conv )
{
	Imagedisplay *id = g_object_new( TYPE_IMAGEDISPLAY, NULL );

#ifdef DEBUG
	g_print( "imagedisplay_new: " );
	gobject_print( G_OBJECT( id ) );
#endif /*DEBUG*/

	imagedisplay_set_conversion( id, conv );

	return( id );
}

void 
imagedisplay_set_shrink_to_fit( Imagedisplay *id, gboolean shrink_to_fit )
{
	id->shrink_to_fit = shrink_to_fit;

	if( shrink_to_fit )
		conversion_set_mag( id->conv, 0 );
}
