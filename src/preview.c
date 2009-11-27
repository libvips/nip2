/* thumbnail widget
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

/* Number of columns of pixmaps we display.
 */
#define NUM_COLUMNS (4)

static ImagedisplayClass *parent_class = NULL;

static void
preview_destroy( GtkObject *object )
{
	Preview *preview;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PREVIEW( object ) );

	preview = PREVIEW( object );

	/* My instance destroy stuff.
	 */
	IM_FREE( preview->filename );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
preview_class_init( PreviewClass *class )
{
	GtkObjectClass *object_class;
	iWindowClass *iwindow_class;

	object_class = (GtkObjectClass *) class;
	iwindow_class = (iWindowClass *) class;

	object_class->destroy = preview_destroy;

	parent_class = g_type_class_peek_parent( class );
}

static void
preview_init( Preview *preview )
{
#ifdef DEBUG
	printf( "preview_init: %s\n", IWINDOW( preview )->title );
#endif /*DEBUG*/

	preview->filename = NULL;
	preview->conv = conversion_new( NULL );
	preview->conv->tile_size = 16;
	gtk_widget_set_size_request( GTK_WIDGET( preview ), 128, 128 );
	imagedisplay_set_conversion( IMAGEDISPLAY( preview ), preview->conv );
	imagedisplay_set_shrink_to_fit( IMAGEDISPLAY( preview ), TRUE );
}

GtkType
preview_get_type( void )
{
	static GtkType type = 0;

	if( !type)  {
		static const GtkTypeInfo info = {
			"Preview",
			sizeof( Preview ),
			sizeof( PreviewClass ),
			(GtkClassInitFunc) preview_class_init,
			(GtkObjectInitFunc) preview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_IMAGEDISPLAY, &info );
	}

	return( type );
}

Preview *
preview_new( void )
{
	Preview *preview = (Preview *) gtk_type_new( TYPE_PREVIEW );

	return( preview );
}

/* Return FALSE for unable to preview file.
 */
gboolean
preview_set_filename( Preview *preview, char *filename )
{
	Imageinfo *ii;
        char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	if( !(ii = imageinfo_new_input( main_imageinfogroup, 
		GTK_WIDGET( preview ), NULL, filename )) )
		return( FALSE );
	conversion_set_image( preview->conv, ii );
	MANAGED_UNREF( ii );

	IM_SETSTR( preview->filename, filename );

	/* How strange, we need this to get the background to clear fully.
	 */
	gtk_widget_queue_draw( GTK_WIDGET( preview ) );

	get_image_info( &buf, IOBJECT( preview->conv->ii )->name );
	set_tooltip( GTK_WIDGET( preview ), "%s", vips_buf_all( &buf ) );

	return( TRUE );
}
