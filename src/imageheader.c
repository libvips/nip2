/* display an image header
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

static iDialogClass *imageheader_parent_class = NULL;

static void
imageheader_destroy( GtkObject *object )
{
	Imageheader *imageheader;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_IMAGEHEADER( object ) );

	imageheader = IMAGEHEADER( object );

	/* My instance destroy stuff.
	 */
	UNREF( imageheader->iimage );

	if( GTK_OBJECT_CLASS( imageheader_parent_class )->destroy )
		GTK_OBJECT_CLASS( imageheader_parent_class )->destroy( object );
}

/* Make a row for the header display. Align the names with "group".
 */
static GtkWidget *
imageheader_row_new( GtkSizeGroup *group, const char *field, GValue *value )
{
	GtkWidget *hbox;
	GtkWidget *label;
	const char *extra;
        char *str_value;

	hbox = gtk_hbox_new( FALSE, 12 );

	label = gtk_label_new( field );
	gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 2 );
	gtk_size_group_add_widget( group, label );

	label = gtk_label_new( "=" );
        gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 2 );

	label = gtk_label_new( "" );
        gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 2 );

	/* Look for enums and decode them.
	 */
	extra = NULL;
	if( strcmp( field, "Coding" ) == 0 )
		extra = NN( im_Coding2char( g_value_get_int( value ) ) );
	else if( strcmp( field, "BandFmt" ) == 0 )
		extra = NN( im_BandFmt2char( g_value_get_int( value ) ) );
	else if( strcmp( field, "Type" ) == 0 )
		extra = NN( im_Type2char( g_value_get_int( value ) ) );

	str_value = g_strdup_value_contents( value );
	if( extra )
		set_glabel( label, "%s - %s", str_value, extra );
	else
		set_glabel( label, "%s", str_value );
	g_free( str_value );

	return( hbox );
}

/* Mapped over the header fields to make rows.
 */
static void *
imageheader_row_map_fn( IMAGE *im, 
	const char *field, GValue *value, Imageheader *imageheader )
{
	GtkWidget *hbox;

	/* This is just there for back compt, hide it.
	 */
	if( strcmp( field, "Bbits" ) == 0 )
		return( NULL );

	hbox = imageheader_row_new( imageheader->group, field, value );
	gtk_box_pack_start( GTK_BOX( imageheader->fields ), 
		hbox, FALSE, FALSE, 0 );

	return( NULL );
}

static void
imageheader_refresh( Imageheader *imageheader )
{
	DESTROY_GTK( imageheader->fields );

	if( imageheader->iimage && imageheader->iimage->value.ii ) {
		Imageinfo *ii = imageheader->iimage->value.ii;
		IMAGE *im = imageinfo_get( FALSE, ii );

		imageheader->group = 
			gtk_size_group_new( GTK_SIZE_GROUP_HORIZONTAL );
		imageheader->fields = gtk_vbox_new( FALSE, 2 );
		gtk_scrolled_window_add_with_viewport( 
			GTK_SCROLLED_WINDOW( imageheader->swin_fields ), 
			imageheader->fields );
		im_header_map( im, 
			(im_header_map_fn) imageheader_row_map_fn,
			imageheader );
		gtk_widget_show_all( imageheader->fields );

		gtk_text_buffer_set_text( 
			gtk_text_view_get_buffer( 
				GTK_TEXT_VIEW( imageheader->history ) ),
			im_history_get( im ), -1 );
	}
	else {
		gtk_editable_delete_text( GTK_EDITABLE( imageheader->history ),
			0, -1 );
	}
}

static void
imageheader_build( GtkWidget *widget )
{
	Imageheader *imageheader = IMAGEHEADER( widget );
	iDialog *idlg = IDIALOG( widget );

	GtkWidget *label;
	GtkWidget *swin;
	GtkWidget *pane;
	GtkWidget *vbox;
	PangoFontDescription *font_desc;

#ifdef DEBUG
	printf( "imageheader_build: %s\n", IWINDOW( imageheader )->title );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( imageheader_parent_class )->build )
		(*IWINDOW_CLASS( imageheader_parent_class )->build)( widget );

	pane = gtk_vpaned_new();
        gtk_box_pack_start( GTK_BOX( idlg->work ), pane, TRUE, TRUE, 2 );

	/* Scrolled area to hold fields.
	 */
	vbox = gtk_vbox_new( FALSE, 2 );
	gtk_paned_pack1( GTK_PANED( pane ), vbox, TRUE, FALSE );
	label = gtk_label_new( _( "Image header fields" ) );
	gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( vbox ), label, FALSE, FALSE, 2 );
	imageheader->swin_fields = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( 
		GTK_SCROLLED_WINDOW( imageheader->swin_fields ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        gtk_box_pack_start( GTK_BOX( vbox ), 
		imageheader->swin_fields, TRUE, TRUE, 2 );

	/* Created in _refresh()
	 */
	imageheader->fields = NULL;

	vbox = gtk_vbox_new( FALSE, 2 );
	gtk_paned_pack2( GTK_PANED( pane ), vbox, TRUE, FALSE );
	label = gtk_label_new( _( "Image history" ) );
	gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( vbox ), label, FALSE, FALSE, 2 );
	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        gtk_box_pack_end( GTK_BOX( vbox ), swin, TRUE, TRUE, 2 );
	imageheader->history = gtk_text_view_new();
	gtk_text_view_set_editable( GTK_TEXT_VIEW( imageheader->history ), 
		FALSE );
	gtk_text_view_set_cursor_visible( GTK_TEXT_VIEW( imageheader->history ),
		FALSE );
	font_desc = pango_font_description_from_string( "Mono" );
	gtk_widget_modify_font( imageheader->history, font_desc );
	pango_font_description_free( font_desc );
	gtk_container_add( GTK_CONTAINER( swin ), imageheader->history );

	imageheader_refresh( imageheader );

        gtk_window_set_default_size( GTK_WINDOW( imageheader ), 550, 550 );

	gtk_widget_show_all( idlg->work );
}

static void
imageheader_class_init( ImageheaderClass *class )
{
	GtkObjectClass *object_class;
	iWindowClass *iwindow_class;

	object_class = (GtkObjectClass *) class;
	iwindow_class = (iWindowClass *) class;

	object_class->destroy = imageheader_destroy;
	iwindow_class->build = imageheader_build;

	imageheader_parent_class = g_type_class_peek_parent( class );
}

static void
imageheader_init( Imageheader *imageheader )
{
#ifdef DEBUG
	printf( "imageheader_init: %s\n", IWINDOW( imageheader )->title );
#endif /*DEBUG*/

	imageheader->iimage = NULL;
}

GtkType
imageheader_get_type( void )
{
	static GtkType imageheader_type = 0;

	if( !imageheader_type ) {
		static const GtkTypeInfo info = {
			"Imageheader",
			sizeof( Imageheader ),
			sizeof( ImageheaderClass ),
			(GtkClassInitFunc) imageheader_class_init,
			(GtkObjectInitFunc) imageheader_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		imageheader_type = gtk_type_unique( TYPE_IDIALOG, &info );
	}

	return( imageheader_type );
}

/* Conversion has changed signal.
 */
static void
imageheader_ii_changed( Model *model, Imageheader *imageheader )
{
	g_assert( IS_MODEL( model ) );
	g_assert( IS_IMAGEHEADER( imageheader ) );

	imageheader_refresh( imageheader );
}

static void
imageheader_link( Imageheader *imageheader, iImage *iimage )
{
	imageheader->iimage = iimage;
	g_object_ref( G_OBJECT( iimage ) );
	iobject_sink( IOBJECT( iimage ) );

	listen_add( G_OBJECT( imageheader ), (GObject **) &imageheader->iimage,
		"changed", G_CALLBACK( imageheader_ii_changed ) );
}

GtkWidget *
imageheader_new( iImage *iimage )
{
	Imageheader *imageheader = gtk_type_new( TYPE_IMAGEHEADER );

	imageheader_link( imageheader, iimage );

	return( GTK_WIDGET( imageheader ) );
}
