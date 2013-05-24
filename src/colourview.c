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

static GraphicviewClass *parent_class = NULL;

static void
colourview_link( View *view, Model *model, View *parent )
{
	Colourview *colourview = COLOURVIEW( view );
	Rowview *rview = ROWVIEW( parent->parent );

	VIEW_CLASS( parent_class )->link( view, model, parent );

	rowview_menu_attach( rview, GTK_WIDGET( colourview->colourdisplay ) );
}

static void 
colourview_refresh( vObject *vobject )
{
	Colourview *colourview = COLOURVIEW( vobject );
	Colour *colour = COLOUR( vobject->iobject );

#ifdef DEBUG
	printf( "colourview_refresh\n" );
#endif /*DEBUG*/

	conversion_set_image( colourview->conv, colour_ii_new( colour ) );
	set_gcaption( colourview->label, "%s", vips_buf_all( &colour->caption ) );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
colourview_class_init( ColourviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = colourview_refresh;

	view_class->link = colourview_link;
}

static void
colourview_area_changed_cb( Imagedisplay *id, Rect *area, 
	Colourview *colourview )
{
	double rgb[4];

	imageinfo_to_rgb( id->conv->ii, rgb );
	colour_set_rgb( COLOUR( VOBJECT( colourview )->iobject ), rgb );
}

static void 
colourview_doubleclick_one_cb( GtkWidget *widget, 
	Colourview *colourview, int state )
{
	Heapmodel *heapmodel = HEAPMODEL( VOBJECT( colourview )->iobject );
	Row *row = heapmodel->row;

	row_select_modifier( row, state );
}

static void 
colourview_doubleclick_two_cb( GtkWidget *widget, 
	Colourview *colourview, int state )
{
	model_edit( widget, MODEL( VOBJECT( colourview )->iobject ) );
}

static void
colourview_init( Colourview *colourview )
{
	GtkWidget *eb;
	GtkWidget *vbox;

#ifdef DEBUG
	printf( "colourview_init\n" );
#endif /*DEBUG*/

        eb = gtk_event_box_new();
	gtk_widget_add_events( GTK_WIDGET( eb ), 
		GDK_POINTER_MOTION_HINT_MASK ); 
        gtk_box_pack_start( GTK_BOX( colourview ), eb, FALSE, FALSE, 0 );
	vbox = gtk_vbox_new( FALSE, 0 );
        gtk_container_add( GTK_CONTAINER( eb ), vbox );
        gtk_widget_show( vbox );

        colourview->colourdisplay = colourdisplay_new( NULL );
	colourview->conv = IMAGEDISPLAY( colourview->colourdisplay )->conv;
	gtk_widget_set_size_request( GTK_WIDGET( colourview->colourdisplay ), 
		DISPLAY_THUMBNAIL, DISPLAY_THUMBNAIL );
        gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( colourview->colourdisplay ), FALSE, FALSE, 0 );
	g_signal_connect( colourview->colourdisplay, "area_changed",
		G_CALLBACK( colourview_area_changed_cb ), colourview );
        gtk_widget_show( GTK_WIDGET( colourview->colourdisplay ) );

	colourview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( colourview->label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( colourview->label ), 2, 0 );
        gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( colourview->label ), FALSE, FALSE, 0 );
        gtk_widget_show( GTK_WIDGET( colourview->label ) );

	doubleclick_add( GTK_WIDGET( colourview ), FALSE,
		DOUBLECLICK_FUNC( colourview_doubleclick_one_cb ), colourview,
		DOUBLECLICK_FUNC( colourview_doubleclick_two_cb ), colourview );

	gtk_widget_set_name( eb, "caption_widget" );
        gtk_widget_show( eb );
}

GtkType
colourview_get_type( void )
{
	static GtkType colourview_type = 0;

	if( !colourview_type ) {
		static const GtkTypeInfo info = {
			"Colourview",
			sizeof( Colourview ),
			sizeof( ColourviewClass ),
			(GtkClassInitFunc) colourview_class_init,
			(GtkObjectInitFunc) colourview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		colourview_type = gtk_type_unique( TYPE_GRAPHICVIEW, &info );
	}

	return( colourview_type );
}

View *
colourview_new( void )
{
	Colourview *colourview = gtk_type_new( TYPE_COLOURVIEW );

	return( VIEW( colourview ) );
}
