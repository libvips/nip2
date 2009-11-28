/* run the display for an image in a workspace 
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

/* Tag our drag-n-drops with these.
 */
enum {
	TARGET_COLOUR,
	TARGET_TEXT
};

static ImagedisplayClass *parent_class = NULL;

static void
colourdisplay_destroy( GtkObject *object )
{
	Colourdisplay *colourdisplay;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_COLOURDISPLAY( object ) );

	colourdisplay = COLOURDISPLAY( object );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

/* Prefer x-color drags for 3 band non-complex imageinfos, and for LABQ
 */
static void
colourdisplay_set_drag_type( Colourdisplay *colourdisplay )
{
	static const GtkTargetEntry text_targets[] = {
		{ "text/plain", 0, TARGET_TEXT },
		{ "application/x-color", 0, TARGET_COLOUR }
	};

	static const GtkTargetEntry colour_targets[] = {
		{ "application/x-color", 0, TARGET_COLOUR },
		{ "text/plain", 0, TARGET_TEXT }
	};

	Imageinfo *imageinfo = IMAGEDISPLAY( colourdisplay )->conv->ii;
	IMAGE *im = imageinfo_get( FALSE, imageinfo );
	const GtkTargetEntry *targets;

	if( !GTK_WIDGET_REALIZED( GTK_WIDGET( colourdisplay ) ) || !im ) 
		return;

	if( im->Bands == 3 && !im_iscomplex( im ) )
		targets = colour_targets;
	else if( im->Coding == IM_CODING_LABQ )
		targets = colour_targets;
	else
		targets = text_targets;

	gtk_drag_dest_unset( GTK_WIDGET( colourdisplay ) );
	gtk_drag_dest_set( GTK_WIDGET( colourdisplay ),
		GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_MOTION | 
			GTK_DEST_DEFAULT_DROP,
		targets, IM_NUMBER( text_targets ),
		GDK_ACTION_COPY );
	gtk_drag_source_unset( GTK_WIDGET( colourdisplay ) );
	gtk_drag_source_set( GTK_WIDGET( colourdisplay ),
		GDK_BUTTON1_MASK | GDK_BUTTON3_MASK,
		targets, IM_NUMBER( text_targets ),
		GDK_ACTION_COPY | GDK_ACTION_MOVE );
}

static void
colourdisplay_realize( GtkWidget *widget )
{
	Colourdisplay *colourdisplay = COLOURDISPLAY( widget );

	GTK_WIDGET_CLASS( parent_class )->realize( widget );

	colourdisplay_set_drag_type( colourdisplay );
}

static void
colourdisplay_drag_begin( GtkWidget *widget, GdkDragContext *context )
{
	Colourdisplay *colourdisplay = COLOURDISPLAY( widget );
	GtkWidget *window;
	double colours[4];
	GdkColor bg;

	window = iimageview_drag_window_new( 48, 32 ); 
	gtk_object_set_data_full( GTK_OBJECT( widget ),
		"nip2-drag-window", window,
		(GtkDestroyNotify) gtk_widget_destroy );
#ifdef DEBUG
	printf( "colourdisplay_drag_begin: generating drag swatch colour\n" );
#endif /*DEBUG*/
	imageinfo_to_rgb( IMAGEDISPLAY( colourdisplay )->conv->ii, colours );
	bg.red = 0xffff * colours[0];
	bg.green = 0xffff * colours[1];
	bg.blue = 0xffff * colours[2];
	gtk_widget_modify_bg( window, GTK_STATE_NORMAL, &bg );

	gtk_drag_set_icon_widget( context, window, -2, -2 );
}

static void
colourdisplay_drag_end( GtkWidget *widget, GdkDragContext *context )
{
	gtk_object_set_data( GTK_OBJECT( widget ), 
		"nip2-drag-window", NULL );
}

static void
colourdisplay_drag_data_get( GtkWidget *widget, GdkDragContext *context,
	GtkSelectionData *selection_data, guint info, guint time ) 
{
	Colourdisplay *colourdisplay = COLOURDISPLAY( widget );
	Imageinfo *imageinfo = IMAGEDISPLAY( colourdisplay )->conv->ii;
	double colours[3];
	guint16 vals[4];
	char vips_buf_text[256];
	VipsBuf buf = VIPS_BUF_STATIC( vips_buf_text );

	switch( info ) {
	case TARGET_COLOUR:
		imageinfo_to_rgb( imageinfo, colours );

		vals[0] = IM_RINT( colours[0] * 0xffff );
		vals[1] = IM_RINT( colours[1] * 0xffff );
		vals[2] = IM_RINT( colours[2] * 0xffff );
		vals[3] = 0xffff;

		gtk_selection_data_set( selection_data,
			gdk_atom_intern( "application/x-color", FALSE ),
			16, (guchar *) vals, 8 );

#ifdef DEBUG
		printf( "colourdisplay_drag_data_get: sending x-color\n" );
#endif /*DEBUG*/

		break;

	case TARGET_TEXT:
		imageinfo_to_text( imageinfo, &buf );
		gtk_selection_data_set( selection_data,
			gdk_atom_intern( "text/plain", FALSE ), 8, 
			(guchar *) vips_buf_all( &buf ), 
			strlen( vips_buf_all( &buf ) ) );

#ifdef DEBUG
		printf( "colourdisplay_drag_data_get: sending text/plain\n" );
#endif /*DEBUG*/

		break;

	default:
		g_assert( FALSE );
		break;
	}
}

static void
colourdisplay_drag_data_received( GtkWidget *widget, GdkDragContext *context,
	gint x, gint y, GtkSelectionData *selection_data,
	guint info, guint time ) 
{
	Colourdisplay *colourdisplay = COLOURDISPLAY( widget );
	Imageinfo *imageinfo = IMAGEDISPLAY( colourdisplay )->conv->ii;

	guint16 *vals;
	gdouble old_rgb[4];
	gdouble rgb[4];

	if( selection_data->length < 0 ) 
		return;

	switch( info ) {
	case TARGET_COLOUR: 
		if( selection_data->format != 16 || 
			selection_data->length != 8 )
			return;

#ifdef DEBUG
		printf( "colourdisplay_drag_data_received: seen x-color\n" );
#endif /*DEBUG*/

		vals = (guint16 *)selection_data->data;
		rgb[0] = (double) vals[0] / 0xffff;
		rgb[1] = (double) vals[1] / 0xffff;
		rgb[2] = (double) vals[2] / 0xffff;

		/* Dragging as RGB can't express small differences. So only
		 * accept the new value if it's sufficiently different from
		 * what we have now.
		 */
		imageinfo_to_rgb( imageinfo, old_rgb );
		if( fabs( rgb[0] - old_rgb[0] ) > (0.5 / 255) ||
			fabs( rgb[1] - old_rgb[1] ) > (0.5 / 255) ||
			fabs( rgb[2] - old_rgb[2] ) > (0.5 / 255) )
			imageinfo_from_rgb( imageinfo, rgb );

		break;

	case TARGET_TEXT:
		if( selection_data->format != 8 )
			return;

#ifdef DEBUG
		printf( "colourdisplay_drag_data_received: seen text/plain\n" );
#endif /*DEBUG*/

		if( !imageinfo_from_text( imageinfo, 
			(char *) selection_data->data ) )
			iwindow_alert( widget, GTK_MESSAGE_ERROR );
		break;

	default:
		g_assert( FALSE );
		break;
	}
}

static void
colourdisplay_generate_tooltip( Colourdisplay *colourdisplay, VipsBuf *buf )
{
	Imagedisplay *id = IMAGEDISPLAY( colourdisplay );

	if( id->conv && id->conv->ii ) {
		imageinfo_to_text( id->conv->ii, buf );
		vips_buf_appends( buf, "\n" );
		vips_buf_appends( buf, _( "Double-click to edit this color, or "
			"drag-and-drop between colors" ) );
	}
}

static void
colourdisplay_conversion_changed( Imagedisplay *id )
{
	Colourdisplay *colourdisplay = COLOURDISPLAY( id );

	IMAGEDISPLAY_CLASS( parent_class )->conversion_changed( id );

	if( id->conv )
		conversion_set_mag( id->conv, 5000 );

	colourdisplay_set_drag_type( colourdisplay );
}

static void
colourdisplay_class_init( ColourdisplayClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	ImagedisplayClass *imagedisplay_class = (ImagedisplayClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = colourdisplay_destroy;
	
	widget_class->realize = colourdisplay_realize;
	widget_class->drag_begin = colourdisplay_drag_begin;
	widget_class->drag_end = colourdisplay_drag_end;
	widget_class->drag_data_get = colourdisplay_drag_data_get;
	widget_class->drag_data_received = colourdisplay_drag_data_received;

	imagedisplay_class->conversion_changed =
		colourdisplay_conversion_changed;
}

static void
colourdisplay_init( Colourdisplay *colourdisplay )
{
#ifdef DEBUG
	printf( "colourdisplay_init\n" );
#endif /*DEBUG*/

	/* Who wants to focus one of these :/
	 */
	GTK_WIDGET_UNSET_FLAGS( GTK_WIDGET( colourdisplay ), GTK_CAN_FOCUS );

	set_tooltip_generate( GTK_WIDGET( colourdisplay ), 
		(TooltipGenerateFn) colourdisplay_generate_tooltip, 
			NULL, NULL );
}

GtkType
colourdisplay_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Colourdisplay",
			sizeof( Colourdisplay ),
			sizeof( ColourdisplayClass ),
			(GtkClassInitFunc) colourdisplay_class_init,
			(GtkObjectInitFunc) colourdisplay_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_IMAGEDISPLAY, &info );
	}

	return( type );
}

Colourdisplay *
colourdisplay_new( Conversion *conv )
{
	Colourdisplay *colourdisplay = gtk_type_new( TYPE_COLOURDISPLAY );

	if( !conv ) 
		conv = conversion_new( NULL );
	conversion_set_synchronous( conv, TRUE );
	imagedisplay_set_conversion( IMAGEDISPLAY( colourdisplay ), conv );

	return( colourdisplay );
}
