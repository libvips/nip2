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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#include "ip.h"

/*
#define DEBUG
 */

static GraphicviewClass *parent_class = NULL;

/* Toggleview callback.
 */
static void
toggleview_change_cb( GtkWidget *widget, Toggleview *togview )
{
	Toggle *tog = TOGGLE( VOBJECT( togview )->iobject );
	Classmodel *classmodel = CLASSMODEL( tog );

	if( tog->value != GTK_TOGGLE_BUTTON( widget )->active ) {
		tog->value = GTK_TOGGLE_BUTTON( widget )->active;

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
	set_glabel( GTK_BIN( togview->toggle )->child, "%s", 
		IOBJECT( tog )->caption );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
toggleview_class_init( ToggleviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

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
        gtk_signal_connect( GTK_OBJECT( togview->toggle ), "clicked",
		GTK_SIGNAL_FUNC( toggleview_change_cb ), togview );

        gtk_widget_show_all( GTK_WIDGET( togview ) );
}

GtkType
toggleview_get_type( void )
{
	static GtkType toggleview_type = 0;

	if( !toggleview_type ) {
		static const GtkTypeInfo info = {
			"Toggleview",
			sizeof( Toggleview ),
			sizeof( ToggleviewClass ),
			(GtkClassInitFunc) toggleview_class_init,
			(GtkObjectInitFunc) toggleview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		toggleview_type = gtk_type_unique( TYPE_GRAPHICVIEW, &info );
	}

	return( toggleview_type );
}

View *
toggleview_new( void )
{
	Toggleview *togview = gtk_type_new( TYPE_TOGGLEVIEW );

	return( VIEW( togview ) );
}
