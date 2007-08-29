/* Imagedisplay widget stuff.
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

#define TYPE_IMAGEDISPLAY (imagedisplay_get_type())
#define IMAGEDISPLAY( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_IMAGEDISPLAY, Imagedisplay ))
#define IMAGEDISPLAY_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_IMAGEDISPLAY, ImagedisplayClass))
#define IS_IMAGEDISPLAY( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IMAGEDISPLAY ))
#define IS_IMAGEDISPLAY_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IMAGEDISPLAY ))
#define IMAGEDISPLAY_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), \
		TYPE_IMAGEDISPLAY, ImagedisplayClass ))

/* Display an entire image. Put in a scrolled window to see just part of it.
 */
struct _Imagedisplay {
	GtkDrawingArea parent_object;

	/* Image we display.
	 */
	Conversion *conv;		/* Conversion we display */
	guint changed_sid;		/* Watch conv with these */
	guint area_changed_sid; 
	gboolean shrink_to_fit; 	/* Auto-shrink mode */

	/* GCs also used by region paint.
	 */
	GdkGC *back_gc;
	GdkGC *xor_gc;
	GdkGC *top_gc;
	GdkGC *bottom_gc;
};

/* Class structure.
 */
typedef struct _ImagedisplayClass {
	/* Drawing area we paint in.
	 */
	GtkDrawingAreaClass parent_class;

	/* Virtual methods.
	 */
	void (*conversion_changed)( Imagedisplay * );
	void (*area_changed)( Imagedisplay *, Rect * );
} ImagedisplayClass;

void imagedisplay_queue_draw_area( Imagedisplay *id, Rect *area );
GType imagedisplay_get_type( void );
void imagedisplay_set_conversion( Imagedisplay *id, Conversion *conv );
Imagedisplay *imagedisplay_new( Conversion *conv );
void imagedisplay_set_shrink_to_fit( Imagedisplay *id, gboolean shrink_to_fit );
