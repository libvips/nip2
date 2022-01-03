/* All the model stuff for the widgets making up a single imageview window. 
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

#define TYPE_IMAGEMODEL (imagemodel_get_type())
#define IMAGEMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_IMAGEMODEL, Imagemodel ))
#define IMAGEMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_IMAGEMODEL, ImagemodelClass))
#define IS_IMAGEMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IMAGEMODEL ))
#define IS_IMAGEMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IMAGEMODEL ))
#define IMAGEMODEL_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_IMAGEMODEL, ImagemodelClass ))

/* Input states.
 */
typedef enum _ImagemodelState {
	IMAGEMODEL_SELECT = 0,		/* Pointer */
	IMAGEMODEL_PAN,			/* Hand panner */
	IMAGEMODEL_MAGIN,		/* Zoom in */
	IMAGEMODEL_MAGOUT,		/* Zoom out */
	IMAGEMODEL_DROPPER,		/* Ink dropper */
	IMAGEMODEL_PEN,			/* Pen */
	IMAGEMODEL_LINE,		/* Line drawing tool */
	IMAGEMODEL_RECT,		/* Rectangle tool */
	IMAGEMODEL_FLOOD,		/* Flood-fill tool */
	IMAGEMODEL_BLOB,		/* Blob flood-fill tool */
	IMAGEMODEL_TEXT,		/* Text tool */
	IMAGEMODEL_SMUDGE,		/* Blur */
	IMAGEMODEL_LAST	
} ImagemodelState;

struct _Imagemodel {
	iObject parent_class;

	/* Context.
	 */
	iImage *iimage;			/* iImage we represent, if any */
	guint iimage_changed_sid;
	guint iimage_destroy_sid;

	/* State held in sub-objects.
	 */
	Conversion *conv;		/* Conversion to screen */
	guint conv_changed_sid;
	guint conv_imageinfo_changed_sid;
	Rect visible;			/* Visible part of canvas */

	/* Input state.
	 */
	ImagemodelState state;
	ImagemodelState save_state; 	/* Old state, during temp actions */
	ImagemodelState pend_state; 	/* To-be state, during delayed switch */

	/* Rulers.
	 */
	gboolean show_rulers;
	gboolean rulers_mm;
	gboolean rulers_offset;

	/* Status bar.
	 */
	gboolean show_status;	

	/* Paintbox.
	 */
	gboolean show_paintbox;		/* Visible/not */
	int nib_radius;			/* Selected radius */
	Imageinfo *nib;			/* Generated nib mask */
	Imageinfo *ink;			/* 1x1 pixel ink image */
	char *font_name;		/* Selected font name */
	char *text; 			/* Text to render */
	Imageinfo *text_mask; 		/* As a bitmap */
	Rect text_area; 		/* Text geometry */

	/* Display control bar.
	 */
	gboolean show_convert;
	double scale;			/* Contrast/brightness */
	double offset;
	gboolean falsecolour;		/* False colour display on */
	gboolean type;			/* Interpret type field */
};

typedef struct _ImagemodelClass {
	iObjectClass parent_class;

	/* Imagemodel has a new imageinfo.
	 */
	void (*imageinfo_changed)( Imagemodel * );
} ImagemodelClass;

void *imagemodel_imageinfo_changed( Imagemodel *imagemodel );
gboolean imagemodel_state_paint( ImagemodelState state );

GType imagemodel_get_type( void );
Imagemodel *imagemodel_new( iImage *iimage );

gboolean imagemodel_set_state( Imagemodel *imagemodel, ImagemodelState state, 
	GtkWidget *parent );

void imagemodel_set_rulers( Imagemodel *imagemodel, gboolean show_rulers );
void imagemodel_set_paintbox( Imagemodel *imagemodel, gboolean show_paintbox );
void imagemodel_set_status( Imagemodel *imagemodel, gboolean show_status );
void imagemodel_set_convert( Imagemodel *imagemodel, gboolean show_convert );

gboolean imagemodel_refresh_text( Imagemodel *imagemodel );
gboolean imagemodel_refresh_nib( Imagemodel *imagemodel );

void imagemodel_paint_recalc( Imagemodel *imagemodel );
