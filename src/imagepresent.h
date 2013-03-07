/* Imagepresent widget stuff.
 */

/*

    Copyright (C) 1991-2001 The Natoinal Gallery

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

#define TYPE_IMAGEPRESENT (imagepresent_get_type())
#define IMAGEPRESENT( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_IMAGEPRESENT, Imagepresent ))
#define IMAGEPRESENT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_IMAGEPRESENT, ImagepresentClass))
#define IS_IMAGEPRESENT( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IMAGEPRESENT ))
#define IS_IMAGEPRESENT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IMAGEPRESENT ))
#define IMAGEPRESENT_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), \
		TYPE_IMAGEPRESENT, ImagepresentClass ))

/* Track an image view canvas in one of these.
 */
struct _Imagepresent {
	GtkBin parent_object;

	/* Context.
	 */
	Imagemodel *imagemodel;		/* Keep model parts of widgets here */

	/* Sub-widgets.
	 */
	Imagedisplay *id;		/* Image we display */
	GtkScrolledWindow *swin;
	GtkAdjustment *hadj;
	GtkAdjustment *vadj;
	GtkHRuler *hrule;		/* Rulers */
	GtkVRuler *vrule;
	GtkEventBox *heb;		/* EventBoxes holding rulers */
	GtkEventBox *veb;
	iWindowCursorContext *cntxt;
	GtkWidget *ruler_menu;

	/* Panning stuff.
	 */
	guint dx, dy;		/* Drag start position */

	/* Last known mouse position, mouse in window.
	 */
	int last_x, last_y;
	gboolean inside;

	/* Background scroll stuff.
	 */
	guint scroll_tid;
	int u, v;

	/* Rubberbanding.
	 */
	Regionview *regionview;	/* region rubberband display */
	Rect floating;		/* rubberband area */

	/* Painting stuff.
	 */
	int paint_last_x;
	int paint_last_y;

	/* Regionviews drawing on us. Used for snap-to-guide stuff.
	 */
	GSList *regionviews;

	/* The regionview that's currently grabbed ... maintained for us by
	 * regionview.c ... see regionview_attach()/_detach()
	 */
	Regionview *grabbed;
};

/* Class structure.
 */
typedef struct _ImagepresentClass {
	/* Our parent.
	 */
	GtkBinClass parent_class;
} ImagepresentClass;

gboolean imagepresent_snap_point( Imagepresent *ip, 
	int x, int y, int *sx, int *sy );
gboolean imagepresent_snap_rect( Imagepresent *ip, Rect *in, Rect *out );

void imagepresent_paint_recalc( Imagepresent *ip );

GType imagepresent_get_type( void );
Imagepresent *imagepresent_new( Imagemodel *imagemodel );

void imagepresent_set_position( Imagepresent *ip, int x, int w );
void imagepresent_set_mag_pos( Imagepresent *ip, int mag, int ix, int iy );
void imagepresent_zoom_to( Imagepresent *ip, int mag );

void imagepresent_scroll_start( Imagepresent *ip, int u, int v );
void imagepresent_scroll_stop( Imagepresent *ip );
