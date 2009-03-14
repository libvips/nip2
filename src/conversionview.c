/* display an image in a window ... watching an Image model.
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

#include "ip.h"

static GtkEventBoxClass *parent_class = NULL;

static void
conversionview_destroy( GtkObject *object )
{
	Conversionview *cv;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_CONVERSIONVIEW( object ) );

	cv = CONVERSIONVIEW( object );

	/* My instance destroy stuff.
	 */

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

/* Find max and min of visible area of image.
 */
static gboolean
conversionview_findmaxmin( Imagemodel *imagemodel, double *min, double *max )
{
	Conversion *conv = imagemodel->conv;
	Rect a, b;

	conversion_disp_to_im_rect( conv, &imagemodel->visible, &a );
	im_rect_intersectrect( &a, &conv->image, &b );
	if( findmaxmin( imageinfo_get( FALSE, conv->ii ), 
		b.left, b.top, b.width, b.height, min, max ) ) {
		error_top( _( "Unable to find image range." ) );
		error_sub( _( "Find image range failed." ) );
		error_vips();
		return( FALSE );
	}

	return( TRUE );
}

static void
conversionview_scale_cb( GtkWidget *wid, Conversionview *cv )
{
	Imagemodel *imagemodel = cv->imagemodel;
	double min, max;

	progress_begin();
	if( !conversionview_findmaxmin( imagemodel, &min, &max ) ) {
		progress_end();
		box_alert( wid );
		return;
	}
	progress_end();

        if( max - min < 1e-20 ) {
                error_top( _( "Unable to scale image." ) );
                error_sub( _( "Maximum and minimum pixel values are equal." ) );
                box_alert( wid );
                return;
        }

	imagemodel->scale = 255.0 / (max - min);
	imagemodel->offset = -(min * imagemodel->scale);
	iobject_changed( IOBJECT( imagemodel ) );
}

static void
conversionview_falsecolour_cb( GtkWidget *wid, Conversionview *cv )
{
	Imagemodel *imagemodel = cv->imagemodel;
	GtkCheckMenuItem *item = GTK_CHECK_MENU_ITEM( wid );

	imagemodel->falsecolour = item->active;
	iobject_changed( IOBJECT( imagemodel ) );
}

static void
conversionview_interpret_cb( GtkWidget *wid, Conversionview *cv )
{
	Imagemodel *imagemodel = cv->imagemodel;
	GtkCheckMenuItem *item = GTK_CHECK_MENU_ITEM( wid );

	imagemodel->type = item->active;
	iobject_changed( IOBJECT( imagemodel ) );
}

static void
conversionview_reset_cb( GtkWidget *wid, Conversionview *cv )
{
	Imagemodel *imagemodel = cv->imagemodel;

	if( imagemodel->iimage ) {
		Row *row = HEAPMODEL( imagemodel->iimage )->row;

		imagemodel->scale = row->ws->scale;
		imagemodel->offset = row->ws->offset;
	}
	else {
		imagemodel->scale = 1.0;
		imagemodel->offset = 0.0;
	}

	imagemodel->falsecolour = FALSE;
	imagemodel->type = TRUE;
	iobject_changed( IOBJECT( imagemodel ) );
}

static void
conversionview_set_default_cb( GtkWidget *wid, Conversionview *cv )
{
	Imagemodel *imagemodel = cv->imagemodel;

	if( imagemodel->iimage ) {
		Row *row = HEAPMODEL( imagemodel->iimage )->row;

		row->ws->scale = imagemodel->scale;
		row->ws->offset = imagemodel->offset;
	}
}

static void
conversionview_hide_cb( GtkWidget *wid, Conversionview *cv )
{
	Imagemodel *imagemodel = cv->imagemodel;

	imagemodel_set_convert( imagemodel, FALSE );
}

static void
conversionview_class_init( ConversionviewClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = conversionview_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */

}

/* Value changed in scale adjustment.
 */
static void
conversionview_scale_change_cb( Tslider *tslider, Conversionview *cv )
{
	Imagemodel *imagemodel = cv->imagemodel;

	if( imagemodel->scale != tslider->value ) {
		imagemodel->scale = tslider->value; 
		iobject_changed( IOBJECT( imagemodel ) );
	}
}

/* Value changed in offset adjustment.
 */
static void
conversionview_offset_change_cb( Tslider *tslider, Conversionview *cv )
{
	Imagemodel *imagemodel = cv->imagemodel;

	if( imagemodel->offset != tslider->value ) {
		imagemodel->offset = tslider->value; 
		iobject_changed( IOBJECT( imagemodel ) );
	}
}

static void
conversionview_init( Conversionview *cv )
{
	GtkWidget *but;
	GtkWidget *arrow;
	GtkWidget *hb;
	GtkWidget *sep;
	GtkWidget *mb;

	GtkWidget *pane;

	cv->imagemodel = NULL;

        gtk_frame_set_shadow_type( GTK_FRAME( cv ), GTK_SHADOW_OUT );

	hb = gtk_hbox_new( FALSE, 2 );
        gtk_container_set_border_width( GTK_CONTAINER( hb ), 2 );
        gtk_container_add( GTK_CONTAINER( cv ), hb );

        /* Build menu. One for each window, as we need to track falsecolour
	 * etc. toggles. Could just have one, and modify pre-popup, but this
	 * is easier.
         */
	pane = menu_build( _( "Convert menu" ) );
	menu_add_but( pane, _( "_Scale" ), 
		GTK_SIGNAL_FUNC( conversionview_scale_cb ), cv );
	cv->falsecolour = menu_add_tog( pane, _( "_False Color" ), 
		GTK_SIGNAL_FUNC( conversionview_falsecolour_cb ), cv );
	cv->type = menu_add_tog( pane, _( "_Interpret" ), 
		GTK_SIGNAL_FUNC( conversionview_interpret_cb ), cv );
	menu_add_but( pane, _( "_Reset" ), 
		GTK_SIGNAL_FUNC( conversionview_reset_cb ), cv );
	menu_add_but( pane, _( "Set As Workspace _Default" ), 
		GTK_SIGNAL_FUNC( conversionview_set_default_cb ), cv );
	menu_add_sep( pane );
	menu_add_but( pane, GTK_STOCK_CLOSE,
		GTK_SIGNAL_FUNC( conversionview_hide_cb ), cv );

        arrow = gtk_arrow_new( GTK_ARROW_RIGHT, GTK_SHADOW_OUT );
	but = gtk_menu_item_new();
        gtk_container_add( GTK_CONTAINER( but ), arrow );
	gtk_menu_item_set_submenu( GTK_MENU_ITEM( but ), pane );
	mb = gtk_menu_bar_new();
	gtk_menu_bar_append( GTK_MENU_BAR( mb ), but );
        gtk_box_pack_start( GTK_BOX( hb ), mb, FALSE, FALSE, 0 );

	cv->scale = tslider_new();
	tslider_set_conversions( cv->scale, 
		tslider_log_value_to_slider, tslider_log_slider_to_value );
	cv->scale->from = 0.001;
	cv->scale->to = 255.0;
	cv->scale->value = 1.0;
	cv->scale->svalue = 128;
	cv->scale->digits = 3;
	tslider_changed( cv->scale );
        gtk_box_pack_start( GTK_BOX( hb ), 
		GTK_WIDGET( cv->scale ), TRUE, TRUE, 0 );
        gtk_signal_connect( GTK_OBJECT( cv->scale ), "changed", 
		GTK_SIGNAL_FUNC( conversionview_scale_change_cb ), cv );

	sep = gtk_vseparator_new();
        gtk_box_pack_start( GTK_BOX( hb ), sep, FALSE, FALSE, 0 );

	cv->offset = tslider_new();
	cv->offset->from = -128;
	cv->offset->to = 128;
	cv->offset->value = 0;
	cv->offset->svalue = 0;
	cv->offset->digits = 1;
	tslider_changed( cv->offset );
        gtk_box_pack_start( GTK_BOX( hb ), 
		GTK_WIDGET( cv->offset ), TRUE, TRUE, 0 );
        gtk_signal_connect( GTK_OBJECT( cv->offset ), "changed", 
		GTK_SIGNAL_FUNC( conversionview_offset_change_cb ), cv );

	gtk_widget_show_all( hb );
}

GtkType
conversionview_get_type( void )
{
	static GtkType conversionview_type = 0;

	if( !conversionview_type ) {
		static const GtkTypeInfo sinfo = {
			"Conversionview",
			sizeof( Conversionview ),
			sizeof( ConversionviewClass ),
			(GtkClassInitFunc) conversionview_class_init,
			(GtkObjectInitFunc) conversionview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		conversionview_type = 
			gtk_type_unique( GTK_TYPE_FRAME, &sinfo );
	}

	return( conversionview_type );
}

/* Our conversion has changed ... update.
 */
static void
conversionview_changed_cb( Imagemodel *imagemodel, Conversionview *cv )
{
	GtkCheckMenuItem *item;

	widget_visible( GTK_WIDGET( cv ), imagemodel->show_convert );
	if( !imagemodel->show_convert )
		return;

	if( cv->scale->value != imagemodel->scale ) {
		cv->scale->value = imagemodel->scale;
		tslider_changed( cv->scale );
	}

	if( cv->offset->value != imagemodel->offset ) {
		cv->offset->value = imagemodel->offset;
		tslider_changed( cv->offset );
	}

	item = GTK_CHECK_MENU_ITEM( cv->falsecolour );
	if( item->active != imagemodel->falsecolour ) 
		gtk_check_menu_item_set_active( item, imagemodel->falsecolour );

	item = GTK_CHECK_MENU_ITEM( cv->type );
	if( item->active != imagemodel->type ) 
		gtk_check_menu_item_set_active( item, imagemodel->type );
}

static void
conversionview_link( Conversionview *cv, Imagemodel *imagemodel )
{
	g_assert( !cv->imagemodel );

	cv->imagemodel = imagemodel;
	g_signal_connect( G_OBJECT( cv->imagemodel ), 
		"changed", G_CALLBACK( conversionview_changed_cb ), cv );
}

Conversionview *
conversionview_new( Imagemodel *imagemodel )
{
	Conversionview *cv = gtk_type_new( TYPE_CONVERSIONVIEW );

	conversionview_link( cv, imagemodel );

	return( cv );
}
