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

/* Number of columns of pixmaps we display.
 */
#define NUM_COLUMNS (4)

G_DEFINE_TYPE( preview, Preview, TYPE_IMAGEDISPLAY ); 

static void
preview_destroy( GtkObject *object )
{
	Preview *preview;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PREVIEW( object ) );

	preview = PREVIEW( object );

	UNREF( preview->conv );
	IM_FREE( preview->filename );

	GTK_OBJECT_CLASS( preview_parent_class )->destroy( object );
}

static void
preview_class_init( PreviewClass *class )
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) class;

	object_class->destroy = preview_destroy;
}

static void
preview_init( Preview *preview )
{
#ifdef DEBUG
	printf( "preview_init: %p\n", preview );
#endif /*DEBUG*/

	preview->filename = NULL;
	preview->conv = conversion_new( NULL );
	preview->conv->tile_size = 16;
	gtk_widget_set_size_request( GTK_WIDGET( preview ), 128, 128 );
	imagedisplay_set_conversion( IMAGEDISPLAY( preview ), preview->conv );
	imagedisplay_set_shrink_to_fit( IMAGEDISPLAY( preview ), TRUE );
	g_object_ref( G_OBJECT( preview->conv ) );
}

Preview *
preview_new( void )
{
	Preview *preview = (Preview *) gtk_type_new( TYPE_PREVIEW );

	return( preview );
}

static void
preview_set_filename_idle( Preview *preview, char *filename )
{
	Imageinfo *ii;

	/* Make sure our enclosing preview wasn't been killed before this idle
	 * starts.
	 */
	if( !preview->conv )
		return;

	/* This is the call that can take ages and kill everything.
	 */
	if( !(ii = imageinfo_new_input( main_imageinfogroup, 
		GTK_WIDGET( preview ), NULL, filename )) ) 
		return;

	/* So test for alive-ness again.
	 */
	if( preview->conv ) {
		char txt[MAX_LINELENGTH];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		conversion_set_image( preview->conv, ii );
		IM_SETSTR( preview->filename, filename );

		/* How strange, we need this to get the 
		 * background to clear fully.
		 */
		gtk_widget_queue_draw( GTK_WIDGET( preview ) );

		get_image_info( &buf, IOBJECT( preview->conv->ii )->name );
		set_tooltip( GTK_WIDGET( preview ), 
			"%s", vips_buf_all( &buf ) );
	}

	MANAGED_UNREF( ii );
}

typedef struct _UpdateProxy {
	Preview *preview;
	char *filename;
} UpdateProxy;

static gboolean
preview_set_filename_idle_cb( UpdateProxy *proxy )
{
	preview_set_filename_idle( proxy->preview, proxy->filename );

	UNREF( proxy->preview );
	g_free( proxy );

	/* Don't run again.
	 */
	return( FALSE );
}

/* We can't load in-line, it can take ages and trigger progress callbacks,
 * which in turn, could kill our enclosing widget.
 *
 * Instead, we do the load in a idle callback and update the preview at the
 * end, if it's still valid.
 */
void
preview_set_filename( Preview *preview, char *filename )
{
	UpdateProxy *proxy = g_new( UpdateProxy, 1 );

	/* We are going to put the preview into the idle queue. It must remain
	 * valid until the idle handler is handled, so we ref.
	 */
	g_object_ref( preview );

	proxy->preview = preview;
	proxy->filename = g_strdup( filename );

	g_idle_add( (GSourceFunc) preview_set_filename_idle_cb, proxy );
}
