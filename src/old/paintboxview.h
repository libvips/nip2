/* Decls for paintboxview.c ... widgets in the paint bar
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

extern iWindowShape paintboxview_shape[];

#define TYPE_PAINTBOXVIEW (paintboxview_get_type())
#define PAINTBOXVIEW( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_PAINTBOXVIEW, Paintboxview ))
#define PAINTBOXVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_PAINTBOXVIEW, PaintboxviewClass ))
#define IS_PAINTBOXVIEW( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_PAINTBOXVIEW ))
#define IS_PAINTBOXVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_PAINTBOXVIEW ))

struct _Paintboxview {
	GtkFrame parent_class;

	Imagemodel *imagemodel;

	/* Spot undo/redo changes on imagemodel->conv->ii.
	 */
	Imageinfo *ii;
	guint ii_undo_changed_sid;
	guint ii_destroy_sid;

	GtkWidget *undo;
	GtkWidget *redo;
	GtkWidget *clear;
	GtkWidget *tool[IMAGEMODEL_LAST];
	Tslider *nib;
	GtkWidget *ink;
	GtkWidget *font;
	GtkWidget *text;
};

typedef struct _PaintboxviewClass {
	GtkFrameClass parent_class;

	/* My methods.
	 */
} PaintboxviewClass;

GType paintboxview_get_type( void );
Paintboxview *paintboxview_new( Imagemodel *imagemodel );
