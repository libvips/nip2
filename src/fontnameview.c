/* run the display for an arrow in a workspace 
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

G_DEFINE_TYPE( fontnameview, Fontnameview, TYPE_GRAPHICVIEW ); 

static void
fontnameview_link( View *view, Model *model, View *parent )
{
	Fontnameview *fontnameview = FONTNAMEVIEW( view );

	VIEW_CLASS( fontnameview_parent_class )->link( view, model, parent );

	if( GRAPHICVIEW( view )->sview )
		gtk_size_group_add_widget( GRAPHICVIEW( view )->sview->group,   
			fontnameview->label );
}

static void 
fontnameview_refresh( vObject *vobject )
{
	Fontnameview *fontnameview = FONTNAMEVIEW( vobject );
	Fontname *fontname = FONTNAME( VOBJECT( vobject )->iobject );

#ifdef DEBUG
	printf( "fontnameview_refresh: " );
	row_name_print( HEAPMODEL( fontname )->row );
	printf( "\n" );
#endif /*DEBUG*/

	if( vobject->iobject->caption ) 
		set_glabel( fontnameview->label, _( "%s:" ), 
			vobject->iobject->caption );
	if( fontname->value )
		fontbutton_set_font_name( fontnameview->fontbutton, 
			fontname->value );

	VOBJECT_CLASS( fontnameview_parent_class )->refresh( vobject );
}

static void
fontnameview_class_init( FontnameviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = fontnameview_refresh;

	view_class->link = fontnameview_link;
}

static void
fontnameview_changed_cb( Fontbutton *fontbutton, Fontnameview *fontnameview )
{
	Fontname *fontname = FONTNAME( VOBJECT( fontnameview )->iobject );
	const char *font_name = fontbutton_get_font_name( fontbutton );

	if( strcmp( font_name, fontname->value ) != 0 ) {
		IM_SETSTR( fontname->value, font_name );
		classmodel_update( CLASSMODEL( fontname ) );
		symbol_recalculate_all();
	}
}

static void
fontnameview_init( Fontnameview *fontnameview )
{
	GtkWidget *hbox;

#ifdef DEBUG
	printf( "fontnameview_init\n" );
#endif /*DEBUG*/

	hbox = gtk_hbox_new( FALSE, 12 );
        gtk_box_pack_start( GTK_BOX( fontnameview ), hbox, TRUE, FALSE, 0 );

        fontnameview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( fontnameview->label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( fontnameview->label ), 2, 7 );
	gtk_box_pack_start( GTK_BOX( hbox ), GTK_WIDGET( fontnameview->label ), 
		FALSE, FALSE, 2 );

        fontnameview->fontbutton = fontbutton_new();
	gtk_box_pack_start( GTK_BOX( hbox ), 
		GTK_WIDGET( fontnameview->fontbutton ), TRUE, TRUE, 0 );
        g_signal_connect( fontnameview->fontbutton, "changed",
                G_CALLBACK( fontnameview_changed_cb ), fontnameview );

        gtk_widget_show_all( GTK_WIDGET( hbox ) );
}

View *
fontnameview_new( void )
{
	Fontnameview *fontnameview = gtk_type_new( TYPE_FONTNAMEVIEW );

	return( VIEW( fontnameview ) );
}
