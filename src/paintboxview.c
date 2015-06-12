/* widgets for the paintbox bar
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

/*
#define DEBUG
 */

#include "ip.h"

/* The popup menu.
 */
static GtkWidget *paintboxview_menu = NULL;

G_DEFINE_TYPE( Paintboxview, paintboxview, GTK_TYPE_FRAME ); 

static void
paintboxview_destroy( GtkObject *object )
{
	Paintboxview *pbv;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PAINTBOXVIEW( object ) );

	pbv = PAINTBOXVIEW( object );

#ifdef DEBUG
	printf( "paintboxview_destroy: %p\n", pbv );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	FREESID( pbv->ii_undo_changed_sid, pbv->ii );
	FREESID( pbv->ii_destroy_sid, pbv->ii );

	GTK_OBJECT_CLASS( paintboxview_parent_class )->destroy( object );
}

static void
paintboxview_realize( GtkWidget *widget )
{
	Paintboxview *pbv = PAINTBOXVIEW( widget );
	iWindow *iwnd = IWINDOW( iwindow_get_root( widget ) );
	guint key;
	GdkModifierType mods;

	gtk_accelerator_parse( "<ctrl>z", &key, &mods );
	gtk_widget_add_accelerator( GTK_WIDGET( pbv->undo ), "clicked",
		iwnd->accel_group, key, mods, 0 );
	gtk_accelerator_parse( "<shift><ctrl>z", &key, &mods );
	gtk_widget_add_accelerator( GTK_WIDGET( pbv->redo ), "clicked",
		iwnd->accel_group, key, mods, 0 );

	GTK_WIDGET_CLASS( paintboxview_parent_class )->realize( widget );
}

/* Hide this paintboxview.
 */
static void
paintboxview_hide_cb( GtkWidget *menu, GtkWidget *host, Paintboxview *pbv )
{
	imagemodel_set_paintbox( pbv->imagemodel, FALSE );
}

static void
paintboxview_class_init( PaintboxviewClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	GtkWidget *pane;

	object_class->destroy = paintboxview_destroy;
	widget_class->realize = paintboxview_realize;

	/* Create signals.
	 */

	/* Init methods.
	 */

	pane = paintboxview_menu = popup_build( _( "Paintbox bar menu" ) );
	popup_add_but( pane, GTK_STOCK_CLOSE,
		POPUP_FUNC( paintboxview_hide_cb ) );
}

/* "toggled" on a tool select button
 */
static void
paintboxview_tool_toggled_cb( GtkWidget *wid, Paintboxview *pbv )
{
	if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( wid ) ) ) {
		Imagemodel *imagemodel = pbv->imagemodel;
		int i;

		for( i = 0; i < IMAGEMODEL_LAST; i++ )
			if( wid == pbv->tool[i] )
				break;

		if( i != (int) IMAGEMODEL_LAST ) 
			imagemodel_set_state( imagemodel, i, wid );
	}
}

/* New nib selected.
 */
static void
paintboxview_scale_change_cb( Tslider *tslider, Paintboxview *pbv )
{
	Imagemodel *imagemodel = pbv->imagemodel;

	if( imagemodel->nib_radius != tslider->value ) {
		imagemodel->nib_radius = tslider->value; 
		iobject_changed( IOBJECT( imagemodel ) );
	}
}

static void
paintboxview_double_cb( GtkWidget *wid, GdkEvent *event, 
	Paintboxview *pbv )
{
	imageinfo_colour_edit( wid, IMAGEDISPLAY( pbv->ink )->conv->ii );
}

static void
paintboxview_font_changed_cb( GtkWidget *widget, Paintboxview *pbv )
{
	Fontbutton *fontbutton = FONTBUTTON( widget );
	Imagemodel *imagemodel = pbv->imagemodel;
	const char *font_name;

	font_name = fontbutton_get_font_name( fontbutton ); 
	if( strcmp( font_name, imagemodel->font_name ) != 0 ) {
		IM_SETSTR( imagemodel->font_name, font_name );
		iobject_changed( IOBJECT( imagemodel ) );
	}
}

static void
paintboxview_undo_cb( GtkWidget *widget, Paintboxview *pbv )
{
	if( !imageinfo_undo( pbv->ii ) )
		iwindow_alert( widget, GTK_MESSAGE_ERROR );

	/* Ask everyone to drop cache, the image has changed.
	 */
	im_invalidate( imageinfo_get( FALSE, pbv->ii ) );

	imagemodel_paint_recalc( pbv->imagemodel );
}

static void
paintboxview_redo_cb( GtkWidget *widget, Paintboxview *pbv )
{
	if( !imageinfo_redo( pbv->ii ) )
		iwindow_alert( widget, GTK_MESSAGE_ERROR );

	/* Ask everyone to drop cache, the image has changed.
	 */
	im_invalidate( imageinfo_get( FALSE, pbv->ii ) );

	imagemodel_paint_recalc( pbv->imagemodel );
}

static void
paintboxview_clear_cb2( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Paintboxview *pbv = PAINTBOXVIEW( client );

	imageinfo_undo_clear( pbv->ii );

	nfn( sys, IWINDOW_YES );
}

static void
paintboxview_clear_cb( GtkWidget *widget, Paintboxview *pbv )
{
	box_yesno( GTK_WIDGET( widget ),
		paintboxview_clear_cb2, iwindow_true_cb, pbv,
		iwindow_notify_null, NULL,
		GTK_STOCK_CLEAR,
		_( "Clear undo history?" ), 
		_( "Are you sure you want to clear all undo and redo? "
		"This will free up memory, but you will no longer be "
		"able to undo or redo any of the painting you have "
		"done so far." ) );
}

static void
paintboxview_text_changed_cb( GtkWidget *widget, Paintboxview *pbv )
{
	const char *text = gtk_entry_get_text( GTK_ENTRY( pbv->text ) );

	IM_SETSTR( pbv->imagemodel->text, text );
}

static void
paintboxview_init( Paintboxview *pbv )
{
	/* Order important! Keep in sync with ImagemodelState.
	 */
	static const char *tool_names[IMAGEMODEL_LAST] = {
		STOCK_SELECT, 		/* IMAGEMODEL_SELECT */
		STOCK_MOVE, 		/* IMAGEMODEL_PAN */
		GTK_STOCK_ZOOM_IN, 	/* IMAGEMODEL_MAGIN */
		GTK_STOCK_ZOOM_OUT,	/* IMAGEMODEL_MAGOUT*/
		STOCK_DROPPER,		/* IMAGEMODEL_DROPPER */
		STOCK_PAINTBRUSH,	/* IMAGEMODEL_PEN */
		STOCK_LINE,		/* IMAGEMODEL_LINE */
		STOCK_RECT,		/* IMAGEMODEL_RECT */
		STOCK_FLOOD,		/* IMAGEMODEL_FLOOD */
		STOCK_FLOOD_BLOB,	/* IMAGEMODEL_BLOB */
		STOCK_TEXT,		/* IMAGEMODEL_TEXT */
		STOCK_SMUDGE 		/* IMAGEMODEL_SMUDGE */
	};

	static const char *tool_tooltips[] = {
		N_( "Manipulate regions" ), 		/* IMAGEMODEL_SELECT */
		N_( "Pan window" ),	 		/* IMAGEMODEL_PAN */
		N_( "Zoom in on mouse" ), 		/* IMAGEMODEL_MAGIN */
		N_( "Zoom out" ),			/* IMAGEMODEL_MAGOUT*/
		N_( "Read pixel into inkwell" ),	/* IMAGEMODEL_DROPPER */
		N_( "Freehand draw " ),			/* IMAGEMODEL_PEN */
		N_( "Draw straight lines" ),		/* IMAGEMODEL_LINE */
		N_( "Fill rectangles" ),		/* IMAGEMODEL_RECT */
		N_( "Flood while pixel not equal to ink" ),		
							/* IMAGEMODEL_FLOOD */
		N_( "Flood while pixel equal to click" ),
							/* IMAGEMODEL_BLOB */
		N_( "Draw text" ),			/* IMAGEMODEL_TEXT */
		N_( "Smudge" ) 				/* IMAGEMODEL_SMUDGE */
	};

	GtkWidget *eb;
	GtkWidget *hb, *hb2;
	GtkWidget *image;
	int i;

	pbv->imagemodel = NULL;
	pbv->ii_undo_changed_sid = 0;
	pbv->ii_destroy_sid = 0;
	pbv->ii = NULL;

        gtk_frame_set_shadow_type( GTK_FRAME( pbv ), GTK_SHADOW_OUT );

	eb = gtk_event_box_new();
        gtk_container_add( GTK_CONTAINER( pbv ), eb );
        popup_attach( eb, paintboxview_menu, pbv );

	hb = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 4 );
        gtk_container_set_border_width( GTK_CONTAINER( hb ), 1 );
        gtk_container_add( GTK_CONTAINER( eb ), hb );

	/* The first 4 tools are harmless (region, move, zoom in, zoom out)
	 * and not linked to the paint actions .. so have them first on their
	 * own.
	 */
	hb2 = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 0 );
	for( i = 0; i < 4; i++ ) {
		pbv->tool[i] = gtk_toggle_button_new();
		g_signal_connect( pbv->tool[i], "toggled", 
			G_CALLBACK( paintboxview_tool_toggled_cb ), pbv );
		image = gtk_image_new_from_icon_name( tool_names[i],
			GTK_ICON_SIZE_BUTTON );
		set_tooltip( pbv->tool[i], "%s", tool_tooltips[i] );
		gtk_container_add( GTK_CONTAINER( pbv->tool[i] ), image );

		gtk_box_pack_start( GTK_BOX( hb2 ), 
			pbv->tool[i], FALSE, FALSE, 0 );
	}
	gtk_box_pack_start( GTK_BOX( hb ), hb2, FALSE, FALSE, 0 );

	hb2 = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 0 );

	pbv->undo = gtk_button_new();
	image = gtk_image_new_from_icon_name( GTK_STOCK_UNDO, 
		GTK_ICON_SIZE_BUTTON );
	gtk_container_add( GTK_CONTAINER( pbv->undo ), image );
	g_signal_connect( pbv->undo, "clicked", 
		G_CALLBACK( paintboxview_undo_cb ), pbv );
	set_tooltip( pbv->undo, _( "Undo last paint action" ) );
        gtk_box_pack_start( GTK_BOX( hb2 ), pbv->undo, FALSE, FALSE, 0 );

	pbv->redo = gtk_button_new();
	image = gtk_image_new_from_icon_name( GTK_STOCK_REDO, 
		GTK_ICON_SIZE_BUTTON );
	gtk_container_add( GTK_CONTAINER( pbv->redo ), image );
	g_signal_connect( pbv->redo, "clicked", 
		G_CALLBACK( paintboxview_redo_cb ), pbv );
	set_tooltip( pbv->redo, _( "Redo last paint action" ) );
        gtk_box_pack_start( GTK_BOX( hb2 ), pbv->redo, FALSE, FALSE, 0 );

	pbv->clear = gtk_button_new();
	image = gtk_image_new_from_icon_name( GTK_STOCK_CLEAR, 
		GTK_ICON_SIZE_BUTTON );
	gtk_container_add( GTK_CONTAINER( pbv->clear ), image );
	g_signal_connect( pbv->clear, "clicked", 
		G_CALLBACK( paintboxview_clear_cb ), pbv );
	set_tooltip( pbv->clear, _( "Clear all undo and redo buffers" ) );
        gtk_box_pack_start( GTK_BOX( hb2 ), pbv->clear, FALSE, FALSE, 0 );

	gtk_box_pack_start( GTK_BOX( hb ), hb2, FALSE, FALSE, 0 );

	hb2 = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 0 );
	for( i = 4; i < IM_NUMBER( tool_names ); i++ ) {
		pbv->tool[i] = gtk_toggle_button_new();
		g_signal_connect( pbv->tool[i], "toggled", 
			G_CALLBACK( paintboxview_tool_toggled_cb ), pbv );
		image = gtk_image_new_from_icon_name( tool_names[i],
			GTK_ICON_SIZE_BUTTON );
		set_tooltip( pbv->tool[i], "%s", tool_tooltips[i] );
		gtk_container_add( GTK_CONTAINER( pbv->tool[i] ), image );

		gtk_box_pack_start( GTK_BOX( hb2 ), 
			pbv->tool[i], FALSE, FALSE, 0 );
	}
	gtk_box_pack_start( GTK_BOX( hb ), hb2, FALSE, FALSE, 0 );

	pbv->nib = tslider_new();
	pbv->nib->from = 0;
	pbv->nib->to = 64;
	pbv->nib->value = 0;
	pbv->nib->svalue = 1;
	pbv->nib->digits = 2;
	tslider_changed( pbv->nib );
        gtk_box_pack_start( GTK_BOX( hb ), 
		GTK_WIDGET( pbv->nib ), FALSE, TRUE, 0 );
        g_signal_connect( pbv->nib, "changed", 
		G_CALLBACK( paintboxview_scale_change_cb ), pbv );
	tslider_set_ignore_scroll( pbv->nib, FALSE );

	pbv->ink = (GtkWidget *) colourdisplay_new( NULL );
        doubleclick_add( GTK_WIDGET( pbv->ink ), FALSE,
                NULL, NULL, 
		DOUBLECLICK_FUNC( paintboxview_double_cb ), pbv );
	gtk_widget_set_size_request( GTK_WIDGET( pbv->ink ), 
		20, 10 );
        gtk_box_pack_start( GTK_BOX( hb ), pbv->ink, FALSE, TRUE, 0 );

	pbv->font = GTK_WIDGET( fontbutton_new() );
        gtk_box_pack_start( GTK_BOX( hb ), pbv->font, FALSE, TRUE, 0 );
	g_signal_connect( pbv->font, "changed", 
		G_CALLBACK( paintboxview_font_changed_cb ), pbv );

	pbv->text = gtk_entry_new();
        gtk_box_pack_start( GTK_BOX( hb ), pbv->text, TRUE, TRUE, 0 );
	g_signal_connect( pbv->text, "changed", 
		G_CALLBACK( paintboxview_text_changed_cb ), pbv );
	set_tooltip( pbv->text, _( "Enter text for text tool" ) );

	gtk_widget_show_all( eb );
}

static void
paintboxview_ii_undo_changed_cb( Imageinfo *imageinfo, Paintboxview *pbv )
{
	gtk_widget_set_sensitive( GTK_WIDGET( pbv->undo ), 
		imageinfo->undo != NULL );
	gtk_widget_set_sensitive( GTK_WIDGET( pbv->redo ), 
		imageinfo->redo != NULL );
	gtk_widget_set_sensitive( GTK_WIDGET( pbv->clear ), 
		imageinfo->undo != NULL || imageinfo->redo != NULL );
}

static void
paintboxview_ii_destroy_cb( Imageinfo *imageinfo, Paintboxview *pbv )
{
	pbv->ii_destroy_sid = 0;
	pbv->ii_undo_changed_sid = 0;
	pbv->ii = NULL;
}

/* Our model has changed ... update.
 */
static void
paintboxview_changed_cb( Imagemodel *imagemodel, Paintboxview *pbv )
{
	Conversion *conv = imagemodel->conv;
	Colourdisplay *ink = COLOURDISPLAY( pbv->ink );
	int i;

#ifdef DEBUG
	printf( "paintboxview_conv_changed_cb: %p\n", conv );
#endif /*DEBUG*/

	/* Has the ii changed? Link to it for undo/redo changes.
	 */
	if( pbv->ii != conv->ii ) {
		FREESID( pbv->ii_undo_changed_sid, pbv->ii );
		FREESID( pbv->ii_destroy_sid, pbv->ii );

		pbv->ii = conv->ii;

		if( conv->ii ) {
			pbv->ii_undo_changed_sid = g_signal_connect( 
				G_OBJECT( conv->ii ), "undo_changed", 
				G_CALLBACK( paintboxview_ii_undo_changed_cb ), 
				pbv );
			pbv->ii_destroy_sid = g_signal_connect( 
				G_OBJECT( conv->ii ), "destroy", 
				G_CALLBACK( paintboxview_ii_destroy_cb ), 
				pbv );
			paintboxview_ii_undo_changed_cb( conv->ii, pbv );
		}

		/* Update ink display for the new image.
		 */
		conversion_set_image( IMAGEDISPLAY( ink )->conv, 
			imagemodel->ink );
	}

	widget_visible( GTK_WIDGET( pbv ), imagemodel->show_paintbox );
	if( !imagemodel->show_paintbox )
		return;

	for( i = 0; i < IMAGEMODEL_LAST; i++ ) 
		gtk_toggle_button_set_active( 
			GTK_TOGGLE_BUTTON( pbv->tool[i] ), 
			i == (int) imagemodel->state );

	fontbutton_set_font_name( FONTBUTTON( pbv->font ),
		pbv->imagemodel->font_name );
}

static void
paintboxview_link( Paintboxview *pbv, Imagemodel *imagemodel )
{
#ifdef DEBUG
	printf( "paintboxview_link: %p\n", pbv );
#endif /*DEBUG*/

	pbv->imagemodel = imagemodel;
	g_signal_connect( G_OBJECT( imagemodel ), "changed", 
		G_CALLBACK( paintboxview_changed_cb ), pbv );
}

Paintboxview *
paintboxview_new( Imagemodel *imagemodel )
{
	Paintboxview *pbv = g_object_new( TYPE_PAINTBOXVIEW, NULL );

	paintboxview_link( pbv, imagemodel );

	return( pbv );
}

