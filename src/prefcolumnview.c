/* a view of a column
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

G_DEFINE_TYPE( Prefcolumnview, prefcolumnview, TYPE_VIEW ); 

static void 
prefcolumnview_refresh( vObject *vobject )
{
	Prefcolumnview *pcview = PREFCOLUMNVIEW( vobject );
	Column *col = COLUMN( VOBJECT( pcview )->iobject );
	char buf[256];
	char buf2[256];

	escape_markup( IOBJECT( col )->caption, buf2, 256 );
	im_snprintf( buf, 256, "<b>%s</b>", buf2 );
	gtk_label_set_markup( GTK_LABEL( pcview->lab ), buf );

	/* Closed columns are hidden.
	 */
	widget_visible( GTK_WIDGET( pcview ), col->open );

	VOBJECT_CLASS( prefcolumnview_parent_class )->refresh( vobject );
}

static void
prefcolumnview_child_add( View *parent, View *child )
{
	Prefcolumnview *pcview = PREFCOLUMNVIEW( parent );
	Subcolumnview *sview = SUBCOLUMNVIEW( child );

	VIEW_CLASS( prefcolumnview_parent_class )->child_add( parent, child );

	gtk_box_pack_end( GTK_BOX( pcview ), GTK_WIDGET( sview ), 
		FALSE, FALSE, 0 );
}

static void
prefcolumnview_class_init( PrefcolumnviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = prefcolumnview_refresh;

	view_class->child_add = prefcolumnview_child_add;
}

static void
prefcolumnview_init( Prefcolumnview *pcview )
{
        pcview->lab = gtk_label_new( "" );
        gtk_box_pack_start( GTK_BOX( pcview ), pcview->lab, FALSE, FALSE, 2 );

        gtk_widget_show_all( GTK_WIDGET( pcview ) );
}

View *
prefcolumnview_new( void )
{
	Prefcolumnview *pcview = g_object_new( TYPE_PREFCOLUMNVIEW, NULL );

	return( VIEW( pcview ) );
}

