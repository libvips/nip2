/* Decls for imageview.c ... display an image in a window.
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

#define TYPE_IMAGEVIEW (imageview_get_type())
#define IMAGEVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_IMAGEVIEW, Imageview ))
#define IMAGEVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_IMAGEVIEW, ImageviewClass ))
#define IS_IMAGEVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_IMAGEVIEW ))
#define IS_IMAGEVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_IMAGEVIEW ))

typedef struct _Imageview {
	Floatwindow parent_class;

	/* Model stuff here.
	 */
	Imagemodel *imagemodel;
	guint imagemodel_changed_sid;

	Imagepresent *ip;
	Conversionview *cv;
	Statusview *sv;
	Paintboxview *pbv;

	GtkActionGroup *action_group;
	GtkUIManager *ui_manager;
} Imageview;

typedef struct _ImageviewClass {
	FloatwindowClass parent_class;

	/* My methods.
	 */
} ImageviewClass;

GtkType imageview_get_type( void );
void imageview_set_paint( Imageview *iv, gboolean paint );
Imageview *imageview_new( iImage *iimage, GtkWidget *parent );
Imageview *imageview_new_area( iImage *iimage, Rect *area, GtkWidget *parent );
