/* Imagedisplay widget code ... display entire image, place this widget in a
 * scrolledwindow to get clipping/scrolling behaviour.
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

/* Make paints slow and flickery so we can see what's going on.
#define DEBUG_SLOW_PAINT
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
	gtk_widget_queue_draw_area( GTK_WIDGET( id ),
		area->left, area->top, area->width, area->height ); 
}

/* Repaint an area of the image.
 */
static void
imagedisplay_repaint_area( Imagedisplay *id, GdkRectangle *expose )
{
	Conversion *conv = id->conv;

	GdkRectangle clip;
	GdkRectangle canvas;
	Rect vclip;
	guchar *buf;
	int lsk;

	if( !GTK_WIDGET( id )->window )
		return;
	if( !GTK_WIDGET_VISIBLE( id ) )
		return;
	if( !conv->ireg )
		return;

	/* Clip as much as we can.
	 */
	canvas.x = 0;
	canvas.y = 0;
	canvas.width = conv->canvas.width;
	canvas.height = conv->canvas.height;
	if( !gdk_rectangle_intersect( expose, &canvas, &clip ) )
		return;

#ifdef DEBUG_PAINT
	g_print( "imagedisplay_repaint_area: at %d x %d, size %d x %d ",
		clip.x, clip.y, clip.width, clip.height );
	gobject_print( G_OBJECT( id ) );
#endif /*DEBUG_PAINT*/

	/* Generate pixels we need. 
	 */
	vclip.left = clip.x;
	vclip.top = clip.y;
	vclip.width = clip.width;
	vclip.height = clip.height;
	if( im_prepare( conv->ireg, &vclip ) )
		return;

        /* Find a pointer to the start of the data, plus a line skip value.
         */
        buf = (guchar *) IM_REGION_ADDR( conv->ireg, vclip.left, vclip.top );
        lsk = IM_REGION_LSKIP( conv->ireg );

	/* Paint into window.
 	 */
	if( conv->ireg->im->Bands == 3 )
		gdk_draw_rgb_image( GTK_WIDGET( id )->window,
			GTK_WIDGET( id )->style->white_gc,
			clip.x, clip.y, clip.width, clip.height,
			GDK_RGB_DITHER_MAX,
			buf, lsk );
	else if( conv->ireg->im->Bands == 1 )
		gdk_draw_gray_image( GTK_WIDGET( id )->window,
			GTK_WIDGET( id )->style->white_gc,
			clip.x, clip.y, clip.width, clip.height,
			GDK_RGB_DITHER_MAX,
			buf, lsk );
}

/* Paint an area with the background pattern.
 */
static void
imagedisplay_paint_background( Imagedisplay *id, GdkRectangle *expose )
{
#ifdef DEBUG_PAINT
	g_print( "imagedisplay_paint_background: at %d x %d, size %d x %d\n",
		expose->x, expose->y, expose->width, expose->height );
#endif /*DEBUG_PAINT*/

	gdk_draw_rectangle( GTK_WIDGET( id )->window, 
		id->back_gc, TRUE,
		expose->x, expose->y, expose->width, expose->height );
}

/* Paint areas outside the image.
 */
static void
imagedisplay_paint_background_clipped( Imagedisplay *id, GdkRectangle *expose )
{
#ifdef DEBUG_SLOW_PAINT
	/* Paint entire expose.
	 */
	imagedisplay_paint_background( id, expose );
#else /*!DEBUG_SLOW_PAINT*/
	GdkRectangle area, clip;

	/* Any stuff to the right of the image?
	 */
	area.x = GTK_WIDGET( id )->requisition.width;
	area.y = 0;
	area.width = IM_MAX( 0, 
		GTK_WIDGET( id )->allocation.width -
		GTK_WIDGET( id )->requisition.width ); 
	area.height = GTK_WIDGET( id )->allocation.height;
	if( gdk_rectangle_intersect( expose, &area, &clip ) )
		imagedisplay_paint_background( id, &clip );

	/* Any stuff below the image?
	 */
	area.x = 0;
	area.y = GTK_WIDGET( id )->requisition.height;
	area.width = GTK_WIDGET( id )->requisition.width;
	area.height = IM_MAX( 0, 
		GTK_WIDGET( id )->allocation.height - 
		GTK_WIDGET( id )->requisition.height );
	if( gdk_rectangle_intersect( expose, &area, &clip ) )
		imagedisplay_paint_background( id, &clip );
#endif /*DEBUG_SLOW_PAINT*/
}

static void
imagedisplay_paint( Imagedisplay *id, GdkRectangle *area )
{
#ifdef DEBUG_PAINT
	g_print( "imagedisplay_repaint: at %d x %d, size %d x %d\n",
		area->x, area->y, area->width, area->height );
#endif /*DEBUG_PAINT*/
#ifdef DEBUG_SLOW_PAINT
	imagedisplay_paint_background_clipped( id, area );
	gdk_flush();
	usleep( 50000 );
#endif /*DEBUG_SLOW_PAINT*/

	/* Clear to background. Always do this, to make sure we paint 
	 * outside the image area.
	 */
	imagedisplay_paint_background_clipped( id, area );

	/* Paint image.
	 */
	imagedisplay_repaint_area( id, area );
}

/* Expose signal handler.
 */
static gint
imagedisplay_expose( GtkWidget *widget, GdkEventExpose *event )
{
	Imagedisplay *id = IMAGEDISPLAY( widget );
	GdkRectangle *rect;
	int i, n;

	if( !GTK_WIDGET_DRAWABLE( id ) ||
		event->area.width == 0 || event->area.height == 0 )
		return( FALSE );

	gdk_region_get_rectangles( event->region, &rect, &n );
	for( i = 0; i < n; i++ ) 
		imagedisplay_paint( id, &rect[i] );
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
	g_print( "imagedisplay_configure_event_cb: %d x %d:\n", 
		event->width, event->height );
#endif /*DEBUG_GEO*/

	/* Note new size in visible hint. Except if parent is a viewport ...
	 * if it's a viewport, someone else will have to track the visble
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
	UNREF( id->xor_gc );
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

	assert( IS_IMAGEDISPLAY( id ) );

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
#ifdef DEBUG_PAINT
	g_print( "imagedisplay_real_area_changed: left = %d, top = %d, "
		"width = %d, height = %d\n",
		dirty->left, dirty->top, dirty->width, dirty->height );
#endif /*DEBUG_PAINT*/

	imagedisplay_queue_draw_area( id, dirty );
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

	if( id->xor_gc )
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

	values.foreground = main_window_top->style->white;
	values.function = GDK_XOR;
	id->xor_gc = gdk_gc_new_with_values( main_window_gdk, &values,
		GDK_GC_FOREGROUND | GDK_GC_FUNCTION );
	gdk_gc_set_line_attributes( id->xor_gc, 
		0, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER );

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
	id->xor_gc = NULL;
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

/* Conversion has changed ... resize and repaint.
 */
static void
imagedisplay_conversion_changed_cb( Conversion *conv, Imagedisplay *id )
{
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
	g_signal_emit( G_OBJECT( id ), 
		imagedisplay_signals[SIG_AREA_CHANGED], 0, dirty );
}

/* Install a conversion. Only allow this once.
 */
void
imagedisplay_set_conversion( Imagedisplay *id, Conversion *conv )
{
	assert( !id->conv );

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
