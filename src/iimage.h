/* a ip image class in a workspace
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

#define TYPE_IIMAGE (iimage_get_type())
#define IIMAGE( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_IIMAGE, iImage ))
#define IIMAGE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_IIMAGE, iImageClass))
#define IS_IIMAGE( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IIMAGE ))
#define IS_IIMAGE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IIMAGE ))
#define IIMAGE_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_IIMAGE, iImageClass ))

struct _iImage {
	Classmodel parent_class;

	/* Class fields.
	 */
	ImageValue value;

	/* List of classmodel which have displays on us.
	 */
	GSList *classmodels;	

	/* List of popup imageview windows we've made. 
	 */
	GSList *views;

	/* Track display pos/size/etc. here.
	 */
	int image_left, image_top;	/* Scroll position */
	int image_mag;			/* Scale */

	/* View attachments.
	 */
	gboolean show_status;
	gboolean show_paintbox;
	gboolean show_convert;
	gboolean show_rulers;	

	/* Bar settings we remember.
	 */
	double scale, offset;
	gboolean falsecolour;
	gboolean type;

	/* Private ... build iobject caption here.
	 */
	VipsBuf caption_buffer;
};

typedef struct _iImageClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} iImageClass;

GType iimage_get_type( void );
gboolean iimage_replace( iImage *iimage, const char *filename );
void iimage_header( GtkWidget *parent, Model *model );
