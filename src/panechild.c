/* The thing that sits in a pane showing the title and close button.
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

G_DEFINE_TYPE( panechild, Panechild, TYPE_VOBJECT ); 

static void
panechild_finalize( GObject *gobject )
{
	Panechild *panechild = PANECHILD( gobject );

#ifdef DEBUG
	printf( "panechild_finalize\n" );
#endif /*DEBUG*/

	/* My instance finalize stuff.
	 */
	IM_FREE( panechild->title );

	G_OBJECT_CLASS( panechild_parent_class )->finalize( gobject );
}

static void
panechild_refresh( vObject *vobject )
{
	Panechild *panechild = PANECHILD( vobject );

#ifdef DEBUG
	printf( "panechild_refresh:\n" );
#endif /*DEBUG*/

	set_glabel( panechild->label, "%s", panechild->title );

	VOBJECT_CLASS( panechild_parent_class )->refresh( vobject );
}

static void
panechild_class_init( PanechildClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;

	gobject_class->finalize = panechild_finalize;

	vobject_class->refresh = panechild_refresh;
}

static void
panechild_hide_cb( GtkWidget *wid, Panechild *panechild )
{
	pane_animate_closed( panechild->pane );
}

static void
panechild_init( Panechild *panechild )
{
	GtkWidget *hbox;
	GtkWidget *but;
        GtkWidget *icon;

#ifdef DEBUG
	printf( "panechild_init:\n" );
#endif /*DEBUG*/

	panechild->pane = NULL;
	panechild->title = NULL;
	panechild->label = NULL;

	hbox = gtk_hbox_new( FALSE, 7 );
	gtk_box_pack_start( GTK_BOX( panechild ), hbox, FALSE, FALSE, 0 );

        but = gtk_button_new();
        gtk_button_set_relief( GTK_BUTTON( but ), GTK_RELIEF_NONE );
        gtk_box_pack_end( GTK_BOX( hbox ), but, FALSE, FALSE, 0 );
        set_tooltip( but, _( "Close the pane" ) );
	icon = gtk_image_new_from_stock( GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU );
        gtk_container_add( GTK_CONTAINER( but ), icon );
        g_signal_connect( but, "clicked",
                G_CALLBACK( panechild_hide_cb ), panechild );

        panechild->label = gtk_label_new( "" );
	gtk_misc_set_alignment( GTK_MISC( panechild->label ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( hbox ), panechild->label, TRUE, TRUE, 2 );

	gtk_widget_show_all( hbox );
}

Panechild *
panechild_new( Pane *pane, const char *title )
{
	Panechild *panechild = gtk_type_new( TYPE_PANECHILD );

	IM_SETSTR( panechild->title, title );

	panechild->pane = pane;
	pane_set_child( pane, panechild );

	return( panechild );
}

