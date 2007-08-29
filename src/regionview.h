/* draw a view of a region in an imageview
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

#define TYPE_REGIONVIEW (regionview_get_type())
#define REGIONVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_REGIONVIEW, Regionview ))
#define REGIONVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_REGIONVIEW, RegionviewClass ))
#define IS_REGIONVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_REGIONVIEW ))
#define IS_REGIONVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_REGIONVIEW ))

#define REGIONVIEW_LABEL_MAX (256)

/* States for the region view.
 */
typedef enum {
	REGIONVIEW_WAIT,	/* Waiting for left down */
	REGIONVIEW_MOVE,	/* Dragging on label */
	REGIONVIEW_RESIZE	/* Dragging on resize handle */
} RegionviewState;

/* Draw types.
 */
typedef enum {
	REGIONVIEW_REGION,	/* width & height > 0 */
	REGIONVIEW_AREA,	/* width & height > 0 and locked */
	REGIONVIEW_MARK,	/* width & height == 0 */
	REGIONVIEW_ARROW,	/* width & height unconstrained */
	REGIONVIEW_HGUIDE,	/* width == image width, height == 0 */
	REGIONVIEW_VGUIDE,	/* width == 0, height == image height */
	REGIONVIEW_LINE,	/* floating dashed line for paintbox */
	REGIONVIEW_BOX		/* floating dashed box for paintbox */
} RegionviewType;

/* Resize types.
 */
typedef enum {
	REGIONVIEW_RESIZE_NONE,
	REGIONVIEW_RESIZE_MOVE,
	REGIONVIEW_RESIZE_EDIT,
	REGIONVIEW_RESIZE_TOPLEFT,
	REGIONVIEW_RESIZE_TOP,
	REGIONVIEW_RESIZE_TOPRIGHT,
	REGIONVIEW_RESIZE_RIGHT,
	REGIONVIEW_RESIZE_BOTTOMRIGHT,
	REGIONVIEW_RESIZE_BOTTOM,
	REGIONVIEW_RESIZE_BOTTOMLEFT,
	REGIONVIEW_RESIZE_LEFT,
	REGIONVIEW_RESIZE_LAST
} RegionviewResize;

struct _Regionview {
	View view;

	RegionviewType type;
	gboolean frozen;	/* type is frozen ... not rethought on resize */

	/* State for resize/move etc.
	 */
	RegionviewState state;
	RegionviewResize resize;/* Resize type */
	int dx, dy;		/* Drag offset */
	gboolean grabbed;	/* Currently tracking with mouse */

	/* The model we show.
	 */
	Classmodel *classmodel;
	Rect *model_area;	/* What we read/write to talk to the model */
	Rect our_area;		/* Same, but our copy ... origin top left */

	/* The imagepresent we draw on.
	 */
	Imagepresent *ip;
	iWindowCursorContext *cntxt;

	/* The signals we've connected to.
	 */
	guint expose_sid;
	guint destroy_sid;
	guint event_sid;
	guint changed_sid;
	guint conv_destroy_sid;
	guint model_changed_sid;

	/* Model info we read for display.
	 */
	GtkStateType paint_state;/* prelight/normal/etc. */

	/* What's on the screen.
	 */
	gboolean unpainting;	/* We are unpainting */
	Rect area;		/* Area of region ... image coordinates */
	Rect label;		/* Area covered by label ... canvas cods */
	int ascent;		/* Height of ascenders for text */
	int dash_offset;
	guint dash_crawl;	/* Timer for dash crawl animation */
	GtkStateType last_paint_state;
	RegionviewType last_type;
	gboolean first;		/* Initial draw (no old pos to remove) */
	gboolean label_geo;	/* Redo the label geo on refresh, please */

	/* Text of label we display 
	 */
	BufInfo caption;
};

typedef struct _RegionviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} RegionviewClass;

void regionview_attach( Regionview *regionview, int x, int y );

GtkType regionview_get_type( void );
Regionview *regionview_new( Classmodel *classmodel, 
	Rect *model_area, Imagepresent *ip );

void regionview_set_type( Regionview *regionview, PElement *root );
