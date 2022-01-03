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

G_DEFINE_TYPE( Pathnameview, pathnameview, TYPE_GRAPHICVIEW ); 

static void
pathnameview_link( View *view, Model *model, View *parent )
{
	Pathnameview *pathnameview = PATHNAMEVIEW( view );

	VIEW_CLASS( pathnameview_parent_class )->link( view, model, parent );

	if( GRAPHICVIEW( view )->sview )
		gtk_size_group_add_widget( GRAPHICVIEW( view )->sview->group,   
			pathnameview->label );
}

static void 
pathnameview_refresh( vObject *vobject )
{
	Pathnameview *pathnameview = PATHNAMEVIEW( vobject );
	Pathname *pathname = PATHNAME( VOBJECT( vobject )->iobject );

#ifdef DEBUG
	printf( "pathnameview_refresh: " );
	row_name_print( HEAPMODEL( pathname )->row );
	printf( "\n" );
#endif /*DEBUG*/

	if( vobject->iobject->caption )
		set_glabel( pathnameview->label, _( "%s:" ), 
			vobject->iobject->caption );
	if( pathname->value ) 
		gtk_button_set_label( GTK_BUTTON( pathnameview->button ), 
			im_skip_dir( pathname->value ) );

	VOBJECT_CLASS( pathnameview_parent_class )->refresh( vobject );
}

static void
pathnameview_class_init( PathnameviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = pathnameview_refresh;

	view_class->link = pathnameview_link;
}

static void
pathnameview_edit_ok( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Pathname *pathname = PATHNAME( client );
	char *fname;

	if( (fname = filesel_get_filename( filesel )) ) {
		IM_SETSTR( pathname->value, fname );
		classmodel_update( CLASSMODEL( pathname ) );
		symbol_recalculate_all();

		g_free( fname );

		nfn( sys, IWINDOW_YES );
	}
	else
		nfn( sys, IWINDOW_ERROR );
}

static void
pathnameview_clicked_cb( GtkWidget *widget, Pathnameview *pathnameview )
{
	Pathname *pathname = PATHNAME( VOBJECT( pathnameview )->iobject );
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), 
		"%s", IOBJECT( pathname )->caption );
	filesel_set_flags( FILESEL( filesel ), TRUE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_any, 0 );
	iwindow_set_parent( IWINDOW( filesel ), widget );
	idialog_set_iobject( IDIALOG( filesel ), IOBJECT( pathname ) );
	filesel_set_done( FILESEL( filesel ), pathnameview_edit_ok, pathname );
	iwindow_build( IWINDOW( filesel ) );
	filesel_set_filename( FILESEL( filesel ), pathname->value );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
pathnameview_init( Pathnameview *pathnameview )
{
	GtkWidget *hbox;

#ifdef DEBUG
	printf( "pathnameview_init\n" );
#endif /*DEBUG*/

	hbox = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 12 );
        gtk_box_pack_start( GTK_BOX( pathnameview ), hbox, TRUE, FALSE, 0 );

        pathnameview->label = gtk_label_new( "" );
	gtk_box_pack_start( GTK_BOX( hbox ), 
		pathnameview->label, FALSE, FALSE, 2 );

	pathnameview->button = gtk_button_new_with_label( "" );
        gtk_box_pack_start( GTK_BOX( hbox ), pathnameview->button, 
		TRUE, TRUE, 0 );
        g_signal_connect( pathnameview->button, "clicked",
                G_CALLBACK( pathnameview_clicked_cb ), pathnameview );
        set_tooltip( pathnameview->button, _( "Select a new file name" ) );

        gtk_widget_show_all( GTK_WIDGET( hbox ) );
}

View *
pathnameview_new( void )
{
	Pathnameview *pathnameview = g_object_new( TYPE_PATHNAMEVIEW, NULL );

	return( VIEW( pathnameview ) );
}
