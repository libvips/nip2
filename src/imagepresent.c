/* Imagepresent widget code.
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

#include "ip.h"

/*
#define DEBUG
 */

/* Define to trace button press events.
#define EVENT
 */

/* Snap if closer than this.
 */
const int imagepresent_snap_threshold = 10;

/* Cursor shape in id for each state.
 */
iWindowShape imagepresent_cursors[IMAGEMODEL_LAST] = {
	IWINDOW_SHAPE_EDIT,		/* IMAGEMODEL_SELECT */
	IWINDOW_SHAPE_MOVE, 		/* IMAGEMODEL_PAN */
	IWINDOW_SHAPE_MAGIN,		/* IMAGEMODEL_MAGIN */
	IWINDOW_SHAPE_MAGOUT,		/* IMAGEMODEL_MAGOUT */
	IWINDOW_SHAPE_DROPPER,		/* IMAGEMODEL_DROPPER */
	IWINDOW_SHAPE_PEN,		/* IMAGEMODEL_PEN */
	IWINDOW_SHAPE_PEN,		/* IMAGEMODEL_LINE */
	IWINDOW_SHAPE_RECT,		/* IMAGEMODEL_RECT */
	IWINDOW_SHAPE_FLOOD,		/* IMAGEMODEL_FLOOD */
	IWINDOW_SHAPE_FLOOD,		/* IMAGEMODEL_BLOB */
	IWINDOW_SHAPE_TEXT,		/* IMAGEMODEL_TEXT */
	IWINDOW_SHAPE_SMUDGE		/* IMAGEMODEL_SMUDGE */
};

/* Gdk keysyms, and the zooms we set for each.
 */
typedef struct _ImagepresentKeymap { 
	guint keyval;
	int zoom;
} ImagepresentKeymap; 

static ImagepresentKeymap imagepresent_keymap[] = {
	{ GDK_1, 1 },
	{ GDK_2, 2 },
	{ GDK_3, 3 },
	{ GDK_4, 4 },
	{ GDK_5, 5 },
	{ GDK_6, 6 },
	{ GDK_7, 7 },
	{ GDK_8, 8 },
	{ GDK_9, 9 }
};

/* Parent class.
 */
static GtkBinClass *parent_class = NULL;

static void
imagepresent_destroy( GtkObject *object )
{
	Imagepresent *ip = IMAGEPRESENT( object );

#ifdef DEBUG
	printf( "imagepresent_destroy\n" );
#endif /*DEBUG*/

	IM_FREEF( g_source_remove, ip->scroll_tid );
	IM_FREEF( iwindow_cursor_context_destroy, ip->cntxt );
	DESTROY_GTK( ip->ruler_menu );

	if( ip->imagemodel ) {
		iImage *iimage = ip->imagemodel->iimage;

		if( iimage ) 
			iimage->views = g_slist_remove( iimage->views, ip );
		UNREF( ip->imagemodel );
	}

	GTK_OBJECT_CLASS( parent_class )->destroy( object );

	/* Child views should all have removed themselves.
	 */
	g_assert( ip->regionviews == NULL );
}

static void
imagepresent_size_request( GtkWidget *widget, GtkRequisition *requisition )
{
	GtkBin *bin = GTK_BIN( widget );
	gint focus_width;
	gint focus_pad;

	gtk_widget_style_get( widget, 
		"focus-line-width", &focus_width,
		"focus-padding", &focus_pad,
		NULL );

	requisition->width = 2 * (focus_width + focus_pad);
	requisition->height = 2 * (focus_width + focus_pad);

	if( bin->child && GTK_WIDGET_VISIBLE( bin->child ) ) {
		GtkRequisition child_requisition;

		gtk_widget_size_request( bin->child, &child_requisition );

		requisition->width += child_requisition.width;
		requisition->height += child_requisition.height;
	}
}

static void
imagepresent_size_allocate( GtkWidget *widget, GtkAllocation *allocation )
{
	GtkBin *bin = GTK_BIN( widget );

	widget->allocation = *allocation;

	if( bin->child && GTK_WIDGET_VISIBLE( bin->child ) ) {
		gint focus_width;
		gint focus_pad;
		GtkAllocation child_allocation;

		gtk_widget_style_get( widget, 
			"focus-line-width", &focus_width,
			"focus-padding", &focus_pad,
			NULL );

		child_allocation.x = allocation->x + focus_width + focus_pad;
		child_allocation.y = allocation->y + focus_width + focus_pad;
		child_allocation.width = IM_MAX( 1, 
			allocation->width - 2 * (focus_width + focus_pad) );
		child_allocation.height = IM_MAX( 1,
			allocation->height - 2 * (focus_width + focus_pad) );

		gtk_widget_size_allocate( bin->child, &child_allocation );
	}
}

static gboolean
imagepresent_expose_event( GtkWidget *widget, GdkEventExpose *event )
{
	if( GTK_WIDGET_DRAWABLE( widget ) ) {
		if( GTK_WIDGET_HAS_FOCUS( widget ) ) {
			gint focus_pad;
			int x, y, width, height;

			gtk_widget_style_get( widget, 
				"focus-padding", &focus_pad,
				NULL );

			x = widget->allocation.x + focus_pad;
			y = widget->allocation.y + focus_pad;
			width = widget->allocation.width - 2 * focus_pad;
			height = widget->allocation.height - 2 * focus_pad;

			gtk_paint_focus( widget->style, widget->window, 
				GTK_WIDGET_STATE( widget ),
				&event->area, widget, "imagepresent",
				x, y, width, height );
		}

		GTK_WIDGET_CLASS( parent_class )->expose_event( widget, event );
	}

	return( FALSE );
}

/* Connect to our enclosing iwnd on realize.
 */
static void
imagepresent_realize( GtkWidget *widget )
{
	Imagepresent *ip = IMAGEPRESENT( widget );
	iWindow *iwnd = IWINDOW( gtk_widget_get_toplevel( widget ) );	

	if( !ip->cntxt ) 
		ip->cntxt = iwindow_cursor_context_new( iwnd, 
			0, "imagepresent" );

	/* Set initial state. _realize() is too late ... the _refresh() has
	 * already happened.
	 */
	iwindow_cursor_context_set_cursor( ip->cntxt, 
		imagepresent_cursors[ip->imagemodel->state] );

	GTK_WIDGET_CLASS( parent_class )->realize( widget );
}

static void
imagepresent_class_init( ImagepresentClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	/* Init parent class.
	 */
	parent_class = g_type_class_peek_parent( class );

        object_class->destroy = imagepresent_destroy;

        widget_class->size_request = imagepresent_size_request;
        widget_class->size_allocate = imagepresent_size_allocate;
        widget_class->expose_event = imagepresent_expose_event;
        widget_class->realize = imagepresent_realize;

	/* Init default methods.
	 */

	/* Static class init.
	 */
}

/* Rethink rulers.
 */
static void
imagepresent_hruler_rethink( Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	IMAGE *im = imageinfo_get( FALSE, conv->ii );

	/* Try to get the ruler width: same as the whole of the scrolled
	 * window.
	 */
	int ruler_width = GTK_WIDGET( ip->swin )->allocation.width;

	double from = imagemodel->visible.left;
	double to = from + ruler_width;
	double pos = ip->last_x;

	double scale;

	if( imagemodel->rulers_offset && im ) {
		from -= im->Xoffset;
		to -= im->Xoffset;
		pos -= im->Xoffset;
	}

	scale = conversion_dmag( conv->mag );
	if( imagemodel->rulers_mm && im ) 
		scale *= im->Xres;

	from /= scale;
	to /= scale;
	pos /= scale;

	gtk_ruler_set_range( GTK_RULER( ip->hrule ), from, to, pos, to - from );
}

static void
imagepresent_vruler_rethink( Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	IMAGE *im = imageinfo_get( FALSE, conv->ii );

	/* Try to get the ruler height: same as the whole of the scrolled
	 * window.
	 */
	int ruler_height = GTK_WIDGET( ip->swin )->allocation.height;

	double from = imagemodel->visible.top;
	double to = from + ruler_height;
	double pos = ip->last_y;

	double scale;

	if( imagemodel->rulers_offset && im ) {
		from -= im->Yoffset;
		to -= im->Yoffset;
		pos -= im->Yoffset;
	}

	scale = conversion_dmag( conv->mag );
	if( imagemodel->rulers_mm && im ) 
		scale *= im->Yres;

	from /= scale;
	to /= scale;
	pos /= scale;

	gtk_ruler_set_range( GTK_RULER( ip->vrule ), from, to, pos, to - from );
}

/* Zoom with the mouse clicked at position x, y in canvas coordinates.
 */
static void
imagepresent_zoom_in( Imagepresent *ip, int x, int y )
{
	Conversion *conv = ip->imagemodel->conv;
	int ix, iy;

	conversion_disp_to_im( conv, x, y, &ix, &iy );
	imagepresent_set_mag_pos( ip, 
		conversion_double( conv->mag ), ix, iy );
}

static void
imagepresent_zoom_in_centre( Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;

	imagepresent_zoom_in( ip,
		IM_RECT_HCENTRE( &imagemodel->visible ),
		IM_RECT_VCENTRE( &imagemodel->visible ) );
}

static void
imagepresent_zoom_out( Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	int ix, iy;

	/* Current centre of window, image cods.
	 */
	conversion_disp_to_im( conv, 
		IM_RECT_HCENTRE( &imagemodel->visible ),
		IM_RECT_VCENTRE( &imagemodel->visible ),
		&ix, &iy );
	imagepresent_set_mag_pos( ip, conversion_halve( conv->mag ), ix, iy );
}

/* Scroll events ... handle mousewheel shortcuts here. 
 */
static gboolean
imagepresent_scroll_event_cb( GtkWidget *widget, 
	GdkEventScroll *ev, Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Rect *visible = &imagemodel->visible;
	gboolean handled;

	/* Gimp uses page_incr / 4 I think, but then scroll speed varies with
	 * window size, which is pretty odd. Just use a constant.
	 */
	const int incr = 50;

	handled = FALSE;

	if( ev->direction == GDK_SCROLL_UP || 
		ev->direction == GDK_SCROLL_DOWN ) {
		if( ev->state & GDK_CONTROL_MASK ) {
			if( ev->direction == GDK_SCROLL_UP )
				imagepresent_zoom_in_centre( ip ); 
			else
				imagepresent_zoom_out( ip );

			handled = TRUE;
		}
		else if( ev->state & GDK_SHIFT_MASK ) {
			if( ev->direction == GDK_SCROLL_UP )
				imagepresent_set_position( ip, 
					visible->left + incr, visible->top );
			else
				imagepresent_set_position( ip, 
					visible->left - incr, visible->top );

			handled = TRUE;
		}
		else {
			if( ev->direction == GDK_SCROLL_UP )
				imagepresent_set_position( ip, 
					visible->left, visible->top - incr );
			else
				imagepresent_set_position( ip, 
					visible->left, visible->top + incr );

			handled = TRUE;
		}
	}

	return( handled );
}

/* Our adjustments have changed (scroll or resize).
 */
static void
imagepresent_hadj_changed_cb( GtkAdjustment *adj, Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;

	imagemodel->visible.left = adj->value;
	imagemodel->visible.width = adj->page_size;

	/* Update the visible hint on the conversion.
	 */
	conv->visible = imagemodel->visible;

#ifdef DEBUG
	printf( "imagepresent_hadj_changed_cb: left = %d, width = %d\n",
		imagemodel->visible.left, imagemodel->visible.width );
#endif /*DEBUG*/

	imagepresent_hruler_rethink( ip );
}

static void
imagepresent_vadj_changed_cb( GtkAdjustment *adj, Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;

	imagemodel->visible.top = adj->value;
	imagemodel->visible.height = adj->page_size;

	/* Update the visible hint on the conversion.
	 */
	conv->visible = imagemodel->visible;

#ifdef DEBUG
	printf( "imagepresent_vadj_changed_cb: top = %d, height = %d\n",
		imagemodel->visible.top, imagemodel->visible.height );
#endif /*DEBUG*/

	imagepresent_vruler_rethink( ip );
}

static void
imagepresent_floating_new( Imagepresent *ip, 
	int left, int top, int width, int height,
	gboolean frozen, RegionviewType type, RegionviewResize resize,
	int x, int y )
{
	g_assert( !ip->regionview );

	ip->floating.left = left;
	ip->floating.top = top;
	ip->floating.width = width;
	ip->floating.height = height;
	ip->regionview = regionview_new( NULL, &ip->floating, ip );
	ip->regionview->frozen = frozen;
	ip->regionview->type = type;
	ip->regionview->resize = resize;

	regionview_attach( ip->regionview, x, y );
}

/* Need to fwd ref this.
 */
static void imagepresent_left_release( Imagepresent *ip, GdkEvent *ev, 
	int x, int y );

static gint
imagepresent_hruler_event( GtkWidget *widget, GdkEvent *ev, Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	IMAGE *im = imageinfo_get( FALSE, conv->ii );
	gboolean handled = FALSE;

	switch( ev->type ) {
	case GDK_BUTTON_PRESS:
		switch( ev->button.button ) {
		case 1:
			(void) imagemodel_set_state( imagemodel, 
				IMAGEMODEL_SELECT, NULL );
			imagepresent_floating_new( ip,
				0, 0, im->Xsize, 0,
				TRUE, REGIONVIEW_HGUIDE,
				REGIONVIEW_RESIZE_BOTTOM,
				ev->button.x, ev->button.y );

			/* The pointer will be grabbed for the drag on the
			 * ruler window. We want to track in the main image
			 * display window, so we have to explicitly ungrab.
			 */
			gdk_pointer_ungrab( ev->button.time );

			handled = TRUE;

			break;

		default:
			break;
		}
		break;

	case GDK_BUTTON_RELEASE:
		switch( ev->button.button ) {
		case 1:
			imagepresent_left_release( ip, ev, 
				ev->button.x, ev->button.y );
			handled = TRUE;
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return( handled );
}

static gint
imagepresent_vruler_event( GtkWidget *widget, GdkEvent *ev, Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	IMAGE *im = imageinfo_get( FALSE, conv->ii );
	gboolean handled = FALSE;

	switch( ev->type ) {
	case GDK_BUTTON_PRESS:
		switch( ev->button.button ) {
		case 1:
			(void) imagemodel_set_state( imagemodel, 
				IMAGEMODEL_SELECT, NULL );
			imagepresent_floating_new( ip,
				0, 0, 0, im->Ysize,
				TRUE, REGIONVIEW_VGUIDE,
				REGIONVIEW_RESIZE_RIGHT,
				ev->button.x, ev->button.y );
			gdk_pointer_ungrab( ev->button.time );
			handled = TRUE;

			break;

		default:
			break;
		}
		break;

	case GDK_BUTTON_RELEASE:
		switch( ev->button.button ) {
		case 1:
			imagepresent_left_release( ip, ev, 
				ev->button.x, ev->button.y );
			handled = TRUE;
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return( handled );
}

/* Track this during a snap.
 */
typedef struct {
	Imagepresent *ip;

	int x;			/* Start point */
	int y;
	int off_x;		/* Current snap offset */
	int off_y;
	int best_x;		/* 'Closeness' of best snap so far */
	int best_y;
} ImagepresentSnap;

static void *
imagepresent_snap_sub( Regionview *regionview, 
	ImagepresentSnap *snap, gboolean *snapped )
{
	Imagemodel *imagemodel = snap->ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	Rect area;

	/* Only static h/v guides.
	 */
	if( regionview->type != REGIONVIEW_HGUIDE && 
		regionview->type != REGIONVIEW_VGUIDE )
		return( NULL );
	if( regionview->state != REGIONVIEW_WAIT )
		return( NULL );

	/* Work in display cods.
	 */
	conversion_im_to_disp_rect( conv, &regionview->area, &area );

	if( regionview->type == REGIONVIEW_HGUIDE ) {
		int score = abs( area.top - snap->y );

		if( score < snap->best_y ) {
			snap->off_y = area.top - snap->y;
			snap->best_y = score;
			*snapped = TRUE;
		}
	}
	else {
		int score = abs( area.left - snap->x );

		if( score < snap->best_x ) {
			snap->off_x = area.left - snap->x;
			snap->best_x = score;
			*snapped = TRUE;
		}
	}

	return( NULL );
}

static gboolean
imagepresent_snap( Imagepresent *ip, ImagepresentSnap *snap )
{
	gboolean snapped;

	snap->ip = ip;
	snap->off_x = 0;
	snap->off_y = 0;
	snap->best_x = imagepresent_snap_threshold;
	snap->best_y = imagepresent_snap_threshold;

	snapped = FALSE;
	slist_map2( ip->regionviews,
		(SListMap2Fn) imagepresent_snap_sub, snap, &snapped );

	return( snapped );
}

gboolean
imagepresent_snap_point( Imagepresent *ip, int x, int y, int *sx, int *sy )
{
	ImagepresentSnap snap;
	gboolean snapped;

	snap.x = x;
	snap.y = y;

	snapped = imagepresent_snap( ip, &snap );

	*sx = x + snap.off_x;
	*sy = y + snap.off_y;

	return( snapped );
}

gboolean
imagepresent_snap_rect( Imagepresent *ip, Rect *in, Rect *out )
{
	ImagepresentSnap snap[8];
	int i, best, best_score;
	gboolean snapped;

	/* Snap the corners plus the edge centres, take the best score.
	 */
	snap[0].x = in->left;
	snap[0].y = in->top;
	snap[1].x = in->left + in->width;
	snap[1].y = in->top;
	snap[2].x = in->left + in->width;
	snap[2].y = in->top + in->height;
	snap[3].x = in->left;
	snap[3].y = in->top + in->height;
	snap[4].x = in->left + in->width / 2;
	snap[4].y = in->top;
	snap[5].x = in->left + in->width;
	snap[5].y = in->top + in->height / 2;
	snap[6].x = in->left + in->width / 2;
	snap[6].y = in->top + in->height;
	snap[7].x = in->left;
	snap[7].y = in->top + in->height / 2;

	for( snapped = FALSE, i = 0; i < 8; i++ )
		snapped |= imagepresent_snap( ip, &snap[i] );

	best = 0;
	best_score = snap[0].best_x;
	for( i = 1; i < 7; i++ )
		if( snap[i].best_x < best_score ) {
			best = i;
			best_score = snap[i].best_x;
		}
	out->left = in->left + snap[best].off_x;

	best = 0;
	best_score = snap[0].best_y;
	for( i = 1; i < 7; i++ )
		if( snap[i].best_y < best_score ) {
			best = i;
			best_score = snap[i].best_y;
		}
	out->top = in->top + snap[best].off_y;

	out->width = in->width;
	out->height = in->height;

	return( snapped );
}

/* Set position x, y in canvas coordinates as the top left of the window.
 */
void
imagepresent_set_position( Imagepresent *ip, int x, int y )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	int maxx = conv->canvas.width - imagemodel->visible.width;
	int maxy = conv->canvas.height - imagemodel->visible.height;

#ifdef DEBUG
	printf( "imagepresent_set_position: %d x %d\n", x, y );
#endif /*DEBUG*/

	adjustments_set_value( ip->hadj, ip->vadj, 
		IM_CLIP( 0, x, maxx ), IM_CLIP( 0, y, maxy ) );
}

/* Set a new magnification, and scroll for the passed x/y position in image
 * coordinates to be in the centre of the screen.
 */
void
imagepresent_set_mag_pos( Imagepresent *ip, int mag, int ix, int iy )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	int nx, ny;
	int last_x, last_y;

#ifdef DEBUG
	printf( "imagepresent_set_mag_pos: %d, %d x %d\n", mag, ix, iy );
#endif /*DEBUG*/

	/* Need to update last_x/y as well ... go to image cods around zoom
	 * operation.
	 */
	conversion_disp_to_im( conv, ip->last_x, ip->last_y, &last_x, &last_y );

	/* Take mouse pos to image cods around zoom operation.
	 */
	conversion_set_mag( conv, mag );
	conversion_im_to_disp( conv, ix, iy, &nx, &ny );

	/* ... and try to get that point in the centre of the window. We need
	 * to zap in the new adjustment upper value, since this won't
	 * otherwise get set until we get back to idle.
	 */
	ip->hadj->upper = conv->canvas.width;
	ip->vadj->upper = conv->canvas.height;
	imagepresent_set_position( ip, 
		nx - imagemodel->visible.width / 2, 
		ny - imagemodel->visible.height / 2 );
	conversion_im_to_disp( conv, 
		last_x, last_y, &ip->last_x, &ip->last_y );
}

/* Set a magnification, keeping the centre of the screen in the centre.
 */
void
imagepresent_zoom_to( Imagepresent *ip, int mag )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;

	/* If window is larger than image.
	 */
	int w = IM_MIN( imagemodel->visible.width, conv->canvas.width );
	int h = IM_MIN( imagemodel->visible.height, conv->canvas.height );

	int ix, iy;

	conversion_disp_to_im( conv, 
		imagemodel->visible.left + w / 2, 
		imagemodel->visible.top + h / 2, &ix, &iy );
	imagepresent_set_mag_pos( ip, mag, ix, iy );
}

/* Left button press event.
 */
static gboolean
imagepresent_left_press( Imagepresent *ip, GdkEvent *ev, int x, int y )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	gboolean handled = FALSE;
	IMAGE *im2;
	int ix, iy;

	/* If there's a regionview grabbed already, block other actions. This
	 * can happen with, for example, the win32 backend where we don't
	 * always see a RELEASE for every PRESS.
	 */
	if( ip->regionview )
		return( FALSE );

	switch( imagemodel->state ) {
	case IMAGEMODEL_SELECT:
		if( ev->button.state & GDK_CONTROL_MASK ) {
			imagepresent_snap_point( ip, x, y, &x, &y );
			conversion_disp_to_im( conv, x, y, &ix, &iy );
			imagepresent_floating_new( ip,
				ix, iy, 0, 0,
				FALSE, REGIONVIEW_MARK, 
					REGIONVIEW_RESIZE_BOTTOMRIGHT,
				x, y );

			handled = TRUE;
		}

		break;

	case IMAGEMODEL_PAN:
		/* Save how much we have to add to x_root to get x.  
		 */
		ip->dx = ev->button.x_root + imagemodel->visible.left;
		ip->dy = ev->button.y_root + imagemodel->visible.top;
		break;

	case IMAGEMODEL_MAGIN:
		imagepresent_zoom_in( ip, x, y );
		handled = TRUE;
		break;

	case IMAGEMODEL_MAGOUT:
		imagepresent_zoom_out( ip );
		handled = TRUE;
		break;

	case IMAGEMODEL_DROPPER:
	case IMAGEMODEL_FLOOD:
	case IMAGEMODEL_BLOB:
		break;

	case IMAGEMODEL_PEN:
	case IMAGEMODEL_SMUDGE:
		imagepresent_snap_point( ip, x, y, &x, &y );
		conversion_disp_to_im( conv, x, y, &ix, &iy );
		ip->paint_last_x = ix;
		ip->paint_last_y = iy;
		handled = TRUE;

		/* This can take ages and, via progress, actually process a
		 * few events. Do it at the end.
		 */
		imagemodel_refresh_nib( imagemodel );

		break;

	case IMAGEMODEL_LINE:
		imagepresent_snap_point( ip, x, y, &x, &y );
		conversion_disp_to_im( conv, x, y, &ix, &iy );
		ip->paint_last_x = ix;
		ip->paint_last_y = iy;
		imagepresent_floating_new( ip,
			ix, iy, 0, 0,
			TRUE, REGIONVIEW_LINE, REGIONVIEW_RESIZE_BOTTOMRIGHT,
			x, y );
		handled = TRUE;

		/* This can take ages and, via progress, actually process a
		 * few events. Do it at the end.
		 */
		imagemodel_refresh_nib( imagemodel );

		break;

	case IMAGEMODEL_RECT:
		imagepresent_snap_point( ip, x, y, &x, &y );
		conversion_disp_to_im( conv, x, y, &ix, &iy );
		imagepresent_floating_new( ip,
			ix, iy, 0, 0,
			TRUE, REGIONVIEW_BOX, REGIONVIEW_RESIZE_BOTTOMRIGHT,
			x, y );
		handled = TRUE;
		break;

	case IMAGEMODEL_TEXT:
		imagepresent_snap_point( ip, x, y, &x, &y );
		conversion_disp_to_im( conv, x, y, &ix, &iy );
		ip->paint_last_x = ix;
		ip->paint_last_y = iy;

		if( !imagemodel_refresh_text( imagemodel ) ) {
			iwindow_alert( GTK_WIDGET( ip ), GTK_MESSAGE_ERROR );
			break;
		}

		im2 = imageinfo_get( FALSE, imagemodel->text_mask );
		imagepresent_floating_new( ip,
			ix,
			iy + imagemodel->text_area.top,
			im2->Xsize, im2->Ysize,
			TRUE, REGIONVIEW_BOX, REGIONVIEW_RESIZE_EDIT,
			x, y );

		handled = TRUE;
		break;

	default:
		break;
	}

	return( handled );
}

static void
imagepresent_paint_stop( Imagepresent *ip, int x, int y )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	Imageinfo *imageinfo = conv->ii;
        Rect oper;
        int ix, iy;

        imagepresent_snap_point( ip, x, y, &x, &y );
        conversion_disp_to_im( conv, x, y, &ix, &iy );

	switch( imagemodel->state ) {
	case IMAGEMODEL_DROPPER:
		if( im_rect_includespoint( &conv->underlay, ix, iy ) ) 
			if( !imageinfo_paint_dropper( imageinfo, 
				imagemodel->ink, ix, iy ) )
				iwindow_alert( GTK_WIDGET( ip ), 
					GTK_MESSAGE_ERROR );

		break;

	case IMAGEMODEL_PEN:
		if( !imageinfo_paint_line( imageinfo, 
			imagemodel->ink, 
			imagemodel->nib, 
			ip->paint_last_x, ip->paint_last_y, ix, iy ) )
			iwindow_alert( GTK_WIDGET( ip ), GTK_MESSAGE_ERROR );

		break;

	case IMAGEMODEL_LINE:
		if( ip->regionview ) { 
			DESTROY_GTK( ip->regionview );

			if( !imageinfo_paint_line( imageinfo, 
				imagemodel->ink, 
				imagemodel->nib, 
				ip->floating.left, ip->floating.top,
				IM_RECT_RIGHT( &ip->floating ),
				IM_RECT_BOTTOM( &ip->floating ) ) )
				iwindow_alert( GTK_WIDGET( ip ), 
					GTK_MESSAGE_ERROR );
		}

		break;

	case IMAGEMODEL_RECT:
		if( ip->regionview ) { 
			DESTROY_GTK( ip->regionview );

			im_rect_normalise( &ip->floating );

			if( !imageinfo_paint_rect( imageinfo, 
				imagemodel->ink, &ip->floating ) )
				iwindow_alert( GTK_WIDGET( ip ), 
					GTK_MESSAGE_ERROR );
		}

		break;

	case IMAGEMODEL_FLOOD:
		if( !imageinfo_paint_flood( imageinfo, 
			imagemodel->ink, ix, iy, FALSE ) )
			iwindow_alert( GTK_WIDGET( ip ), GTK_MESSAGE_ERROR );

		break;

	case IMAGEMODEL_BLOB:
		if( !imageinfo_paint_flood( imageinfo, 
			imagemodel->ink, ix, iy, TRUE ) )
			iwindow_alert( GTK_WIDGET( ip ), GTK_MESSAGE_ERROR );

		break;

	case IMAGEMODEL_TEXT:
		if( ip->regionview ) { 
			DESTROY_GTK( ip->regionview );

			if( !imageinfo_paint_mask( imageinfo, 
				imagemodel->ink, imagemodel->text_mask, 
				ip->floating.left, ip->floating.top ) )
				iwindow_alert( GTK_WIDGET( ip ), 
					GTK_MESSAGE_ERROR );
		}

		break;

	case IMAGEMODEL_SMUDGE:
		/* Area to smudge in display cods.
		 */
		oper.left = -10;
		oper.top = -10;
		oper.width = 20;
		oper.height = 20;

		/* Translate to IMAGE cods.
		 */
		conversion_disp_to_im_rect( conv, &oper, &oper );

		if( !imageinfo_paint_smudge( imageinfo,
			&oper, ip->paint_last_x, ip->paint_last_y, ix, iy ) )
			iwindow_alert( GTK_WIDGET( ip ), GTK_MESSAGE_ERROR );

		break;

	default:
		break;
	}

	imagemodel_paint_recalc( imagemodel );
	imageinfo_undo_mark( imageinfo );

	/* Ask everyone to drop cache, the image has changed.
	 */
	im_invalidate( imageinfo_get( FALSE, imageinfo ) );
}

/* Left button release event.
 */
static void
imagepresent_left_release( Imagepresent *ip, GdkEvent *ev, int x, int y )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Row *row = imagemodel->iimage ? 
		HEAPMODEL( imagemodel->iimage )->row : NULL;

	switch( imagemodel->state ) {
	case IMAGEMODEL_SELECT:
		if( ip->regionview && row ) {
			/* Make a new region.
			 */
			char txt[MAX_STRSIZE];
			VipsBuf buf = VIPS_BUF_STATIC( txt );
			Symbol *sym;

			switch( ip->regionview->type ) {
			case REGIONVIEW_MARK:
				vips_buf_appendf( &buf, "%s ", CLASS_MARK );
				row_qualified_name( row, &buf );
				vips_buf_appendd( &buf, ip->floating.left );
				vips_buf_appendd( &buf, ip->floating.top );
				break;

			case REGIONVIEW_REGION:
				vips_buf_appendf( &buf, "%s ", CLASS_REGION );
				row_qualified_name( row, &buf );
				vips_buf_appendd( &buf, ip->floating.left );
				vips_buf_appendd( &buf, ip->floating.top );
				vips_buf_appendd( &buf, ip->floating.width );
				vips_buf_appendd( &buf, ip->floating.height );
				break;

			case REGIONVIEW_ARROW:
				vips_buf_appendf( &buf, "%s ", CLASS_ARROW );
				row_qualified_name( row, &buf );
				vips_buf_appendd( &buf, ip->floating.left );
				vips_buf_appendd( &buf, ip->floating.top );
				vips_buf_appendd( &buf, ip->floating.width );
				vips_buf_appendd( &buf, ip->floating.height );
				break;

			case REGIONVIEW_HGUIDE:
				vips_buf_appendf( &buf, "%s ", CLASS_HGUIDE );
				row_qualified_name( row, &buf );
				vips_buf_appendd( &buf, ip->floating.top );
				break;

			case REGIONVIEW_VGUIDE:
				vips_buf_appendf( &buf, "%s ", CLASS_VGUIDE );
				row_qualified_name( row, &buf );
				vips_buf_appendd( &buf, ip->floating.left );
				break;

			default:
				g_assert( FALSE );
			}

			DESTROY_GTK( ip->regionview );

			if( !(sym = workspace_add_def_recalc( row->ws, 
				vips_buf_all( &buf ) )) ) 
				iwindow_alert( GTK_WIDGET( ip ), 
					GTK_MESSAGE_ERROR );

			workspace_deselect_all( row->ws );
		}
		break;

	case IMAGEMODEL_DROPPER:
	case IMAGEMODEL_PEN:
	case IMAGEMODEL_LINE:
	case IMAGEMODEL_RECT:
	case IMAGEMODEL_FLOOD:
	case IMAGEMODEL_BLOB:
	case IMAGEMODEL_TEXT:
	case IMAGEMODEL_SMUDGE:
		imagepresent_paint_stop( ip, x, y );
		break;

	default:
		break;
	}
}

/* Button motion event.
 */
static void
imagepresent_button_motion( Imagepresent *ip, GdkEvent *ev )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	Imageinfo *imageinfo = conv->ii;
	Rect oper;
	int x, y;
	int ix, iy;

	imagepresent_snap_point( ip, ev->motion.x, ev->motion.y, &x, &y );
	conversion_disp_to_im( conv, x, y, &ix, &iy );

	switch( imagemodel->state ) {
	case IMAGEMODEL_SELECT:
		break;

	case IMAGEMODEL_PAN:
		imagepresent_set_position( ip, 
			(int) ip->dx - ev->motion.x_root, 
			(int) ip->dy - ev->motion.y_root );
		break;

	case IMAGEMODEL_MAGIN:
		break;

	case IMAGEMODEL_MAGOUT:
		break;

	case IMAGEMODEL_DROPPER:
		if( im_rect_includespoint( &conv->underlay, ix, iy ) ) 
			if( !imageinfo_paint_dropper( imageinfo, 
				imagemodel->ink, ix, iy ) )
				iwindow_alert( GTK_WIDGET( ip ), 
					GTK_MESSAGE_ERROR );
		break;

	case IMAGEMODEL_PEN:
		if( !imageinfo_paint_line( imageinfo, 
			imagemodel->ink, 
			imagemodel->nib, 
			ip->paint_last_x, ip->paint_last_y, ix, iy ) )
			iwindow_alert( GTK_WIDGET( ip ), GTK_MESSAGE_ERROR );
		im_invalidate( imageinfo_get( FALSE, imageinfo ) );

		ip->paint_last_x = ix;
		ip->paint_last_y = iy;
		break;

	case IMAGEMODEL_LINE:
		/* rubberband
		 */
		break;

	case IMAGEMODEL_SMUDGE:
		/* Area to smudge in display cods.
		 */
		oper.left = -10;
		oper.top = -10;
		oper.width = 20;
		oper.height = 20;

		/* Translate to IMAGE cods.
		 */
		conversion_disp_to_im_rect( conv, &oper, &oper );

		if( !imageinfo_paint_smudge( imageinfo, &oper, 
			ip->paint_last_x, ip->paint_last_y, 
			ix, iy ) )
			iwindow_alert( GTK_WIDGET( ip ), GTK_MESSAGE_ERROR );
		im_invalidate( imageinfo_get( FALSE, imageinfo ) );

		ip->paint_last_x = ix;
		ip->paint_last_y = iy;

		break;

	default:
		break;
	}

}

/* Main event loop.
 */
static gboolean
imagepresent_event_cb( GtkWidget *widget, GdkEvent *ev, Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	gboolean handled;

#ifdef EVENT
	printf( "imagepresent_event_cb %d\n", ev->type );
#endif /*EVENT*/

	handled = FALSE;

	switch( ev->type ) {
	case GDK_BUTTON_PRESS:
		if( !GTK_WIDGET_HAS_FOCUS( GTK_WIDGET( ip ) ) )
			gtk_widget_grab_focus( GTK_WIDGET( ip ) );

		switch( ev->button.button ) {
		case 1:
			handled = imagepresent_left_press( ip, ev,  
				ev->button.x, ev->button.y );

			break;

		case 2:
#ifdef EVENT
			printf( "button2 press: at %gx%g\n",
				ev->button.x, ev->button.y );
#endif /*EVENT*/

			/* Switch to pan, for this drag.
			 */
			imagemodel->save_state = imagemodel->state;
			(void) imagemodel_set_state( imagemodel, 
				IMAGEMODEL_PAN, NULL );
			handled = imagepresent_left_press( ip, ev,  
				ev->button.x, ev->button.y );

			break;

		default:
			break;
		}

		break;

	case GDK_BUTTON_RELEASE:
		switch( ev->button.button ) {
		case 1:
			imagepresent_left_release( ip, ev,
				ev->button.x, ev->button.y );

			break;

		case 2:
#ifdef EVENT
			printf( "button2 release: at %gx%g\n",
				ev->button.x, ev->button.y );
#endif /*EVENT*/

			/* Should always succeed.
			 */
			(void) imagemodel_set_state( imagemodel, 
				imagemodel->save_state, NULL );

			break;

		default:
			break;
		}

		break;

	case GDK_MOTION_NOTIFY:
		/* We're using motion hints, so we need to read the pointer to
		 * get the next one.
		 */
		widget_update_pointer( GTK_WIDGET( ip ), ev );

		ip->last_x = ev->motion.x;
		ip->last_y = ev->motion.y;

		if( ev->motion.state & GDK_BUTTON1_MASK ||
			ev->motion.state & GDK_BUTTON2_MASK ) 
			imagepresent_button_motion( ip, ev );

		/* Update tick marks on rulers, if they're being drawn.
		 */
		if( GTK_WIDGET_VISIBLE( ip->hrule ) ) {
			imagepresent_hruler_rethink( ip );
			imagepresent_vruler_rethink( ip );
		}

		break;

	case GDK_ENTER_NOTIFY:
		ip->inside = TRUE;

		break;

	case GDK_LEAVE_NOTIFY:
		ip->inside = FALSE;

		break;

	default:
		break;
	}

	return( handled );
}

static gboolean
imagepresent_key_press_event_cb( GtkWidget *widget, GdkEventKey *event, 
	Imagepresent *ip )
{
	Imagemodel *imagemodel = ip->imagemodel;
	Conversion *conv = imagemodel->conv;
	Rect *visible = &imagemodel->visible;
	GtkAdjustment *hadj = ip->hadj;
	GtkAdjustment *vadj = ip->vadj;
	gboolean handled;
	int i;

#ifdef DEBUG
	printf( "imagepresent_key_press_event_cb\n" );
#endif /*DEBUG*/

	handled = FALSE;

	switch( event->keyval ) {
	case GDK_Left:
		if( !(event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK)) )
			imagepresent_set_position( ip, 
				visible->left - hadj->step_increment, 
				visible->top );
		else if( event->state & GDK_SHIFT_MASK )
			imagepresent_set_position( ip, 
				visible->left - hadj->page_increment, 
				visible->top );
		else if( event->state & GDK_CONTROL_MASK )
			imagepresent_set_position( ip, 
				0, visible->top );
		handled = TRUE;
		break;

	case GDK_Right:
		if( !(event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK)) )
			imagepresent_set_position( ip, 
				visible->left + hadj->step_increment, 
				visible->top );
		else if( event->state & GDK_SHIFT_MASK )
			imagepresent_set_position( ip, 
				visible->left + hadj->page_increment, 
				visible->top );
		else if( event->state & GDK_CONTROL_MASK )
			imagepresent_set_position( ip, 
				conv->canvas.width, visible->top );
		handled = TRUE;
		break;

	case GDK_Up:
		if( !(event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK)) )
			imagepresent_set_position( ip, 
				visible->left,
				visible->top - vadj->step_increment );
		else if( event->state & GDK_SHIFT_MASK )
			imagepresent_set_position( ip, 
				visible->left,
				visible->top - vadj->page_increment );
		else if( event->state & GDK_CONTROL_MASK )
			imagepresent_set_position( ip, 
				visible->left, 0 );
		handled = TRUE;
		break;

	case GDK_Down:
		if( !(event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK)) )
			imagepresent_set_position( ip, 
				visible->left,
				visible->top + vadj->step_increment );
		else if( event->state & GDK_SHIFT_MASK )
			imagepresent_set_position( ip, 
				visible->left,
				visible->top + vadj->page_increment );
		else if( event->state & GDK_CONTROL_MASK )
			imagepresent_set_position( ip, 
				visible->left, conv->canvas.height );
		handled = TRUE;
		break;

	/* FIXME + and = are not always on the same key, of course :( 
	 */
	case GDK_i:
	case GDK_plus:
	case GDK_equal:
		if( ip->inside ) 
			imagepresent_zoom_in( ip, ip->last_x, ip->last_y );
		else 
			imagepresent_zoom_in_centre( ip ); 
		handled = TRUE;
		break;

	case GDK_o: 
	case GDK_minus:
		imagepresent_zoom_out( ip );
		handled = TRUE;
		break;

	case GDK_0:
		conversion_set_mag( conv, 0 );
		handled = TRUE;
		break;

	default:
		break;
	}

	/* Check the number zoom keys too.
	 */
	if( !handled ) 
		for( i = 0; i < IM_NUMBER( imagepresent_keymap ); i++ )
			if( event->keyval == imagepresent_keymap[i].keyval ) {
				int mask = event->state & GDK_CONTROL_MASK;
				int zoom = imagepresent_keymap[i].zoom;

				imagepresent_zoom_to( ip, mask ? -zoom : zoom );
				handled = TRUE;
				break;
			}

	return( handled );
}

/* ... and set the work window once that's there.
 */
static void
imagepresent_realize_id_cb( Imagedisplay *id )
{
	iWindow *iwnd = IWINDOW( gtk_widget_get_toplevel( GTK_WIDGET( id ) ) );	

	iwindow_set_work_window( iwnd, GTK_WIDGET( id )->window );
}

static void
imagepresent_rulers_mm_cb( GtkWidget *wid, GtkWidget *host, 
	Imagepresent *ip )
{
	ip->imagemodel->rulers_mm = gtk_check_menu_item_get_active( 
		GTK_CHECK_MENU_ITEM( wid ) );
	iobject_changed( IOBJECT( ip->imagemodel ) );
}

static void
imagepresent_rulers_offset_cb( GtkWidget *wid, GtkWidget *host, 
	Imagepresent *ip )
{
	ip->imagemodel->rulers_offset = gtk_check_menu_item_get_active( 
		GTK_CHECK_MENU_ITEM( wid ) );
	iobject_changed( IOBJECT( ip->imagemodel ) );
}

static void
imagepresent_ruler_hide_cb( GtkWidget *wid, GtkWidget *host, Imagepresent *ip )
{
	imagemodel_set_rulers( ip->imagemodel, FALSE );
}

static void
imagepresent_init( Imagepresent *ip )
{
	GtkWidget *port;
	GtkWidget *bar;
	GtkWidget *table;

	/* Basic init.
	 */
	ip->imagemodel = NULL;

	ip->dx = 0;
	ip->dy = 0;
	ip->last_x = 0;
	ip->last_y = 0;
	ip->inside = FALSE;
	ip->scroll_tid = 0;
	ip->u = 0;
	ip->v = 0;
	ip->regionview = NULL;
	ip->paint_last_x = 0;
	ip->paint_last_y = 0;
	ip->regionviews = NULL;
	ip->grabbed = NULL;

	/* Make main imagedisplay table.
	 */
	table = GTK_WIDGET( gtk_table_new( 2, 2, FALSE ) );
        gtk_container_add( GTK_CONTAINER( ip ), table );
	gtk_widget_show( table );

	/* Make canvas.
	 */
	ip->id = imagedisplay_new( NULL );
	GTK_WIDGET_SET_FLAGS( ip, GTK_CAN_FOCUS );
	g_signal_connect( ip->id, "realize",
		G_CALLBACK( imagepresent_realize_id_cb ), NULL );

	/* Press/release/motion-notify stuff.
	 */
	gtk_widget_add_events( GTK_WIDGET( ip->id ), 
		GDK_KEY_PRESS_MASK | 
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
		GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK ); 
	g_signal_connect_after( ip->id, "event",
		G_CALLBACK( imagepresent_event_cb ), ip );
	g_signal_connect( ip, "key_press_event",
		G_CALLBACK( imagepresent_key_press_event_cb ), ip );

	ip->swin = GTK_SCROLLED_WINDOW( gtk_scrolled_window_new( NULL, NULL ) );
	gtk_scrolled_window_add_with_viewport( ip->swin, GTK_WIDGET( ip->id ) );
        gtk_scrolled_window_set_policy( ip->swin,
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	ip->hadj = gtk_scrolled_window_get_hadjustment( ip->swin );
	ip->vadj = gtk_scrolled_window_get_vadjustment( ip->swin );
	g_signal_connect( ip->swin, "scroll_event",
		G_CALLBACK( imagepresent_scroll_event_cb ), ip );

	port = gtk_bin_get_child( GTK_BIN( ip->swin ) );
	g_assert( GTK_IS_VIEWPORT( port ) );
	g_signal_connect( ip->hadj, "changed",
		G_CALLBACK( imagepresent_hadj_changed_cb ), ip );
	g_signal_connect( ip->hadj, "value_changed",
		G_CALLBACK( imagepresent_hadj_changed_cb ), ip );
	g_signal_connect( ip->vadj, "changed",
		G_CALLBACK( imagepresent_vadj_changed_cb ), ip );
	g_signal_connect( ip->vadj, "value_changed",
		G_CALLBACK( imagepresent_vadj_changed_cb ), ip );

	bar = ip->swin->hscrollbar;
	g_assert( GTK_IS_SCROLLBAR( bar ) );
	GTK_WIDGET_UNSET_FLAGS( bar, GTK_CAN_FOCUS );
	bar = ip->swin->vscrollbar;
	g_assert( GTK_IS_SCROLLBAR( bar ) );
	GTK_WIDGET_UNSET_FLAGS( bar, GTK_CAN_FOCUS );

	/* Need one menu per image window (could have a single menu for all
	 * windows, but then we'd have to set the state of the toggle buttons
	 * before mapping)
	 */
	ip->ruler_menu = popup_build( _( "Ruler menu" ) );
	popup_add_tog( ip->ruler_menu, _( "Rulers In _mm" ), 
		POPUP_FUNC( imagepresent_rulers_mm_cb ) );
	popup_add_tog( ip->ruler_menu, _( "Show _Offset" ), 
		POPUP_FUNC( imagepresent_rulers_offset_cb ) );
	menu_add_sep( ip->ruler_menu );
	popup_add_but( ip->ruler_menu, GTK_STOCK_CLOSE,
		POPUP_FUNC( imagepresent_ruler_hide_cb ) );

	/* Make rulers.
	 */
	ip->hrule = GTK_HRULER( gtk_hruler_new() );
	gtk_ruler_set_metric( GTK_RULER( ip->hrule ), GTK_PIXELS );
	GTK_WIDGET_UNSET_FLAGS( GTK_WIDGET( ip->hrule ), GTK_CAN_FOCUS );
	gtk_widget_show( GTK_WIDGET( ip->hrule ) );

	ip->vrule = GTK_VRULER( gtk_vruler_new() );
	gtk_ruler_set_metric( GTK_RULER( ip->vrule ), GTK_PIXELS );
	GTK_WIDGET_UNSET_FLAGS( GTK_WIDGET( ip->vrule ), GTK_CAN_FOCUS );
	gtk_widget_show( GTK_WIDGET( ip->vrule ) );

	ip->heb = GTK_EVENT_BOX( gtk_event_box_new() );
        gtk_container_add( GTK_CONTAINER( ip->heb ), GTK_WIDGET( ip->hrule ) );
        g_signal_connect( ip->heb, "event",
		G_CALLBACK( imagepresent_hruler_event ), ip );
        popup_attach( GTK_WIDGET( ip->heb ), ip->ruler_menu, ip );

	ip->veb = GTK_EVENT_BOX( gtk_event_box_new() );
        gtk_container_add( GTK_CONTAINER( ip->veb ), GTK_WIDGET( ip->vrule ) );
        g_signal_connect( ip->veb, "event",
		G_CALLBACK( imagepresent_vruler_event ), ip );
        popup_attach( GTK_WIDGET( ip->veb ), ip->ruler_menu, ip );

	/* Attach all widgets to table.
	 */
	gtk_table_attach( GTK_TABLE( table ), GTK_WIDGET( ip->heb ), 
		1, 2, 0, 1,
		GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		GTK_FILL, 
		2, 2 );
	gtk_table_attach( GTK_TABLE( table ), GTK_WIDGET( ip->veb ), 
		0, 1, 1, 2,
		GTK_FILL, 
		GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		2, 2 );
	gtk_table_attach( GTK_TABLE( table ), GTK_WIDGET( ip->swin ), 
		1, 2, 1, 2,
		GTK_FILL | GTK_EXPAND | GTK_SHRINK, 
		GTK_FILL | GTK_EXPAND | GTK_SHRINK,
		2, 2 );
	gtk_widget_show( GTK_WIDGET( ip->id ) );
	gtk_widget_show( GTK_WIDGET( ip->swin ) );

	/* Set initial ruler visibility on first refresh from imagemodel.
	 */
}

GType
imagepresent_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ImagepresentClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) imagepresent_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Imagepresent ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) imagepresent_init,
		};

		type = g_type_register_static( GTK_TYPE_BIN, 
			"Imagepresent", &info, 0 );
	}

	return( type );
}

/* The model has changed ... update!
 */
static void
imagepresent_imagemodel_changed_cb( Imagemodel *imagemodel, Imagepresent *ip )
{
	if( ip->cntxt )
		iwindow_cursor_context_set_cursor( ip->cntxt, 
			imagepresent_cursors[imagemodel->state] );

	widget_visible( GTK_WIDGET( ip->heb ), imagemodel->show_rulers );
	widget_visible( GTK_WIDGET( ip->veb ), imagemodel->show_rulers );
	imagepresent_hruler_rethink( ip );
	imagepresent_vruler_rethink( ip );
}

/* The model has a new imageinfo.
 */
static void
imagepresent_imagemodel_imageinfo_changed_cb( Imagemodel *imagemodel, 
	Imagepresent *ip )
{
	/* Reset our mode. We don't want to stay painting.
	 */
	if( imagemodel_state_paint( imagemodel->state ) ) 
		imagemodel_set_state( imagemodel, IMAGEMODEL_SELECT, NULL );
}

static void
imagepresent_link( Imagepresent *ip, Imagemodel *imagemodel )
{
	ip->imagemodel = imagemodel;
	g_object_ref( G_OBJECT( ip->imagemodel ) );
	iobject_sink( IOBJECT( ip->imagemodel ) );

	g_signal_connect( G_OBJECT( imagemodel ), "changed", 
		G_CALLBACK( imagepresent_imagemodel_changed_cb ), ip );
	g_signal_connect( G_OBJECT( imagemodel ), "imageinfo_changed", 
		G_CALLBACK( imagepresent_imagemodel_imageinfo_changed_cb ), 
		ip );
	imagedisplay_set_conversion( ip->id, imagemodel->conv );

	if( imagemodel->iimage )
		imagemodel->iimage->views = 
			g_slist_prepend( imagemodel->iimage->views, ip );
}

/* Make a new Imagepresent. 
 */
Imagepresent *
imagepresent_new( Imagemodel *imagemodel )
{
	Imagepresent *ip = g_object_new( TYPE_IMAGEPRESENT, NULL );

	imagepresent_link( ip, imagemodel );

	return( ip );
}

/* Background scroller.
 */
static gboolean
imagepresent_scroll_cb( Imagepresent *ip )
{
	imagepresent_set_position( ip, 
		ip->imagemodel->visible.left + ip->u, 
		ip->imagemodel->visible.top + ip->v );

	return( TRUE );
}

void
imagepresent_scroll_start( Imagepresent *ip, int u, int v )
{
	if( !ip->scroll_tid )
		ip->scroll_tid = g_timeout_add( 100, 
			(GSourceFunc) imagepresent_scroll_cb, ip );

	ip->u = u;
	ip->v = v;
}

void
imagepresent_scroll_stop( Imagepresent *ip )
{
	IM_FREEF( g_source_remove, ip->scroll_tid );
	ip->u = 0;
	ip->v = 0;
}
