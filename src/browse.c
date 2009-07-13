/* thumbnail window
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

static iDialogClass *parent_class = NULL;

static void
browse_refresh_stop( Browse *browse )
{
#ifdef DEBUG
	printf( "browse_refresh_stop: idle = %d\n", browse->idle_id );
#endif /*DEBUG*/

	IM_FREEF( gtk_idle_remove, browse->idle_id );
	IM_FREEF( slist_free_all, browse->files );
}

static void
browse_destroy( GtkObject *object )
{
	Browse *browse;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_BROWSE( object ) );

	browse = BROWSE( object );

	/* My instance destroy stuff.
	 */
	browse_refresh_stop( browse );
	browse->filesel->browse = NULL;
	IM_FREE( browse->dirname );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
browse_build( GtkWidget *widget )
{
	Browse *browse = BROWSE( widget );
	iDialog *idlg = IDIALOG( widget );
	GtkWidget *work;

#ifdef DEBUG
	printf( "browse_build: %s\n", IWINDOW( browse )->title );
#endif /*DEBUG*/

	iwindow_set_title( IWINDOW( browse ), _( "Thumbnails" ) );
	idialog_set_callbacks( idlg,
		iwindow_true_cb, NULL, NULL, NULL );

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( parent_class )->build )
		(*IWINDOW_CLASS( parent_class )->build)( widget );

	/* Make a directory label. 
	 */
	work = idlg->work;
	browse->label = gtk_label_new( "" );
	gtk_misc_set_alignment( GTK_MISC( browse->label ), 0, 0.5 );
        gtk_box_pack_start( GTK_BOX( work ), browse->label, FALSE, FALSE, 2 );
	gtk_widget_show( browse->label );

	browse->swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( browse->swin ), 
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        gtk_box_pack_start( GTK_BOX( work ), browse->swin, TRUE, TRUE, 2 );
        gtk_window_set_default_size( GTK_WINDOW( browse ), 
		80 + 70 * NUM_COLUMNS, 400 );
	gtk_widget_show( browse->swin );

	gtk_widget_show_all( idlg->work );
}

static void
browse_class_init( BrowseClass *class )
{
	GtkObjectClass *object_class;
	iWindowClass *iwindow_class;

	object_class = (GtkObjectClass *) class;
	iwindow_class = (iWindowClass *) class;

	object_class->destroy = browse_destroy;
	iwindow_class->build = browse_build;

	parent_class = g_type_class_peek_parent( class );
}

static void
browse_init( Browse *browse )
{
#ifdef DEBUG
	printf( "browse_init: %s\n", IWINDOW( browse )->title );
#endif /*DEBUG*/

	browse->filesel = NULL;

	browse->swin = NULL;
	browse->label = NULL;
	browse->table = NULL;

	browse->idle_id = 0;
	browse->files = NULL;
	browse->dirname = NULL;
	browse->row = 0;
	browse->column = 0;
}

GtkType
browse_get_type( void )
{
	static GtkType type = 0;

	if( !type)  {
		static const GtkTypeInfo info = {
			"Browse",
			sizeof( Browse ),
			sizeof( BrowseClass ),
			(GtkClassInitFunc) browse_class_init,
			(GtkObjectInitFunc) browse_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_IDIALOG, &info );
	}

	return( type );
}

GtkWidget *
browse_new( void )
{
	Browse *browse = (Browse *) gtk_type_new( TYPE_BROWSE );

	return( GTK_WIDGET( browse ) );
}

void
browse_set_filesel( Browse *browse, Filesel *filesel )
{
	browse->filesel = filesel;
	iwindow_set_parent( IWINDOW( browse ), GTK_WIDGET( filesel ) );
}

/* Single click on a button.
 */
/*ARGSUSED*/
static void
button_single_cb( GtkWidget *wid, Browse *browse )
{
	Imagedisplay *id = IMAGEDISPLAY( GTK_BIN( wid )->child );

	filesel_set_filename( browse->filesel, 
		IOBJECT( id->conv->ii )->name );
}

/* Double click on a button.
 */
/*ARGSUSED*/
static void
button_double_cb( GtkWidget *wid, Browse *browse )
{
	Imagedisplay *id = IMAGEDISPLAY( GTK_BIN( wid )->child );

	filesel_set_filename( browse->filesel, 
		IOBJECT( id->conv->ii )->name );
	idialog_done_trigger( IDIALOG( browse->filesel ), 0 ); 
}

/* Add a widget to the browse window.
 */
static void
browse_add_widget( Browse *browse, GtkWidget *but )
{
	gtk_table_attach( GTK_TABLE( browse->table ), but,
		browse->column, browse->column + 1, 
		browse->row - 1, browse->row,
		GTK_SHRINK, GTK_SHRINK, 2, 2 );
	gtk_widget_show_all( but );

	/* Have we filled this row? Expand the table.
	 */
	if( ++browse->column == NUM_COLUMNS ) {
		browse->column = 0;
		browse->row++;
		gtk_table_resize( GTK_TABLE( browse->table ), 
			browse->row, NUM_COLUMNS );
	}
}

/* Load an Imageinfo as a button.
 */
static void
browse_add_image( Browse *browse, Imageinfo *ii )
{
	Conversion *conv;
	Imagedisplay *id;

	GtkWidget *but;
        char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	conv = conversion_new( ii );
	conv->tile_size = 16;
	id = imagedisplay_new( conv );
	gtk_widget_set_size_request( GTK_WIDGET( id ), 64, 64 );
	imagedisplay_set_shrink_to_fit( id, TRUE );

	but = gtk_button_new();
	gtk_container_add( GTK_CONTAINER( but ), GTK_WIDGET( id ) );
	gtk_widget_show_all( but );

	doubleclick_add( but, FALSE,
		DOUBLECLICK_FUNC( button_single_cb ), browse, 
		DOUBLECLICK_FUNC( button_double_cb ), browse );

	get_image_info( &buf, IOBJECT( ii )->name );
	set_tooltip( but, "%s", vips_buf_all( &buf ) );

	browse_add_widget( browse, but );
}

/* Make a 'bad file' error button.
 */
static void
browse_add_error( Browse *browse, char *name )
{
	GtkWidget *but;
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	but = gtk_button_new_with_label( im_skip_dir( name ) );
	gtk_widget_show_all( but );

	get_image_info( &buf, name );
	vips_buf_appends( &buf, "\n" );
	vips_buf_appends( &buf, error_get_top() );
	set_tooltip( but, "%s", vips_buf_all( &buf ) );

	browse_add_widget( browse, but );

	error_clear();
}

static void *
browse_add_file( char *name, Browse *browse )
{
	if( strcmp( im_skip_dir( name ), "." ) == 0 || 
		strcmp( im_skip_dir( name ), ".." ) == 0 )
		return( NULL );

	browse->files = g_slist_prepend( browse->files, im_strdupn( name ) );

	return( NULL );
}

static void
browse_update_status( Browse *browse )
{
	if( !browse->files )
		set_glabel( browse->label, 
			_( "Image files found in: \"%s\"" ), browse->dirname );
	else if( browse->idle_id )
		set_glabel( browse->label, _( "Searching ..." ) );
	else
		set_glabel( browse->label, _( "Search incomplete!" ) );
}

static gint
browse_idle( Browse *browse )
{
	char *name;
	Imageinfo *ii;

#ifdef DEBUG
	printf( "browse_idle\n" );
#endif /*DEBUG*/

	if( !browse->files ) {
		browse_refresh_stop( browse );
		browse_update_status( browse );
		return( FALSE );
	}

	name = (char *) browse->files->data;
	browse->files = g_slist_remove( browse->files, name );

	/* Make sure we're not freed during this bit.
	 */
	gtk_object_ref( GTK_OBJECT( browse ) );

	ii = imageinfo_new_input( main_imageinfogroup, 
		GTK_WIDGET( browse ), NULL, name );

	/* We might have had our window closed during that load.
	 */
	if( browse->idle_id ) {
		if( ii )
			browse_add_image( browse, ii );
		else
			browse_add_error( browse, name );
	}

	gtk_object_unref( GTK_OBJECT( browse ) );

	/* If we're using it, it will have been reffed by the imagedisplay in
	 * the button.
	 */
	MANAGED_UNREF( ii );

	im_free( name );

	return( TRUE );
}

/* Scan current directory and rebuild.
 */
void
browse_refresh( Browse *browse, const gchar *dirname )
{
	Filesel *filesel = browse->filesel;
	int type = filesel_get_filetype( filesel );

	char txt[FILENAME_MAX];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

#ifdef DEBUG
	printf( "browse_refresh: %s\n", dirname );
#endif /*DEBUG*/

	browse_refresh_stop( browse );

	IM_SETSTR( browse->dirname, dirname );
	IM_FREEF( gtk_widget_destroy, browse->table );
        browse->row = 0;

        browse->table = gtk_table_new( 1, NUM_COLUMNS, FALSE );
        browse->row = 1;
        browse->column = 0;
	gtk_scrolled_window_add_with_viewport( 
		GTK_SCROLLED_WINDOW( browse->swin ), browse->table );
        gtk_widget_show( browse->table );

	filesel_make_patt( filesel->type[type], &buf );

	(void) path_map_dir( dirname, vips_buf_all( &buf ), 
		(path_map_fn) browse_add_file, browse );
	browse->idle_id = gtk_idle_add( (GtkFunction) browse_idle, browse );

	browse_update_status( browse );
}
