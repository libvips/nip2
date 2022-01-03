/* the display part of a toggle button 
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

#include "ip.h"

/*
#define DEBUG
 */

G_DEFINE_TYPE( Toggleview, toggleview, TYPE_GRAPHICVIEW ); 

/* Toggleview callback.
 */
static void
toggleview_change_cb( GtkWidget *widget, Toggleview *togview )
{
	Toggle *tog = TOGGLE( VOBJECT( togview )->iobject );
	Classmodel *classmodel = CLASSMODEL( tog );

	if( tog->value != 
		gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) ) {
		tog->value = gtk_toggle_button_get_active( 
			GTK_TOGGLE_BUTTON( widget ) );  

		classmodel_update( classmodel );
		symbol_recalculate_all();
	}
}

static void 
toggleview_refresh( vObject *vobject )
{
	Toggleview *togview = TOGGLEVIEW( vobject );
	Toggle *tog = TOGGLE( VOBJECT( togview )->iobject );

        gtk_toggle_button_set_active( 
		GTK_TOGGLE_BUTTON( togview->toggle ), tog->value );
	set_glabel( gtk_bin_get_child( GTK_BIN( togview->toggle ) ), "%s", 
		IOBJECT( tog )->caption );

	VOBJECT_CLASS( toggleview_parent_class )->refresh( vobject );
}

static void
toggleview_class_init( ToggleviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = toggleview_refresh;
}

static void
toggleview_init( Toggleview *togview )
{
        togview->toggle = build_gtoggle( GTK_WIDGET( togview ), "" );
        set_tooltip( togview->toggle, _( "Left-click to change value" ) );
        g_signal_connect( togview->toggle, "clicked",
		G_CALLBACK( toggleview_change_cb ), togview );

        gtk_widget_show_all( GTK_WIDGET( togview ) );
}

View *
toggleview_new( void )
{
	Toggleview *togview = g_object_new( TYPE_TOGGLEVIEW, NULL );

	return( VIEW( togview ) );
}
