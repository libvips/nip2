/* Manage display conversion parameters.
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

#define TYPE_CONVERSION (conversion_get_type())
#define CONVERSION( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_CONVERSION, Conversion ))
#define CONVERSION_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_CONVERSION, ConversionClass))
#define IS_CONVERSION( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_CONVERSION ))
#define IS_CONVERSION_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_CONVERSION ))
#define CONVERSION_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_CONVERSION, ConversionClass ))

struct _Conversion {
	Model parent_class;

	/* Image.
	 */
	Imageinfo *ii;		/* Underlying image */
	guint changed_sid;	/* Watch ii with these two */
	guint area_changed_sid;	
	REGION *reg;		/* Region for input from underlying image */
	gboolean synchronous;	/* TRUE to disable BG stuff */
	int priority;		/* render priority */

	Imageinfo *visual_ii;	/* Visualisation image ... eg. histplot */
	Imageinfo *display_ii;	/* Sized and cached */
	int display_mag;	/* What mag the display_ii is built for */
	IMAGE *mask;		/* Read display mask from here */
	Imageinfo *repaint_ii;	/* Colour converted for screen */
	REGION *ireg;		/* Region for input from repaint image */
	REGION *mreg;		/* Region for input from repaint mask */
	int tile_size;		/* Set smaller for thumbnails */
	int fade_steps;		/* Set to 1 for thumbnails */

	/* Basic geometry.
	 */
	Rect underlay;		/* Size of underlying image (at 0,0) */
	Rect image;		/* Size of visualisation image (at 0,0) */
	Rect canvas;		/* Size of image we display (always at 0,0) */
	Rect visible;		/* hint ... visible region of display image */
	int mag;		/* -ve for shrink, +ve for expand */

	/* Visualisation controls. If enabled is set, we built the pipeline
	 * using these params.
	 */
	gboolean enabled;
	gboolean changed;	/* Trigger a rebuild with these */
	double scale;		/* Contrast/brightness */
	double offset;
	gboolean falsecolour;	/* False colour display on */
	gboolean type;		/* Interpret type field */
};

typedef struct _ConversionClass {
	ModelClass parent_class;

	/* My methods.

		area_changed	we forward the "area" changed signal off the
				ii we are holding ... in repaint coordinates

	 */
	void (*area_changed)( Conversion *, Rect * );

	/* The imageinfo has been swapped for a new one.
	 */
	void (*imageinfo_changed)( Conversion * );
} ConversionClass;

GType conversion_get_type( void );
Conversion *conversion_new( Imageinfo *ii );

void conversion_set_image( Conversion *conv, Imageinfo *ii );
gboolean conversion_refresh_text( Conversion *conv );

double conversion_dmag( int mag );
int conversion_double( int mag );
int conversion_halve( int mag );

void conversion_disp_to_im( Conversion *conv, 
	int dx, int dy, int *ix, int *iy );
void conversion_im_to_disp( Conversion *conv, 
	int ix, int iy, int *dx, int *dy );
void conversion_disp_to_im_rect( Conversion *conv, Rect *dr, Rect *ir );
void conversion_im_to_disp_rect( Conversion *conv, Rect *ir, Rect *dr );

void conversion_set_mag( Conversion *conv, int mag );
void conversion_set_synchronous( Conversion *conv, gboolean synchronous );
void conversion_set_params( Conversion *conv, gboolean enabled,
	double scale, double offset, gboolean falsecolour, gboolean type );

void conversion_header_dialog( Conversion *conv, 
	const char *name, GtkWidget *parent );
