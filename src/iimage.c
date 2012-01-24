/* an image class object in a workspace
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

static ClassmodelClass *parent_class = NULL;

static void
iimage_dispose( GObject *gobject )
{
	iImage *iimage;

#ifdef DEBUG
	printf( "iimage_dispose %p\n", gobject );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_IIMAGE( gobject ) );

	iimage = IIMAGE( gobject );

	slist_map( iimage->classmodels, 
		(SListMapFn) classmodel_iimage_unlink, iimage );
	g_assert( !iimage->classmodels );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
iimage_finalize( GObject *gobject )
{
	iImage *iimage;

#ifdef DEBUG
	printf( "iimage_finalize\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_IIMAGE( gobject ) );

	iimage = IIMAGE( gobject );

	image_value_destroy( &iimage->value );
	IM_FREEF( g_slist_free, iimage->views );
	vips_buf_destroy( &iimage->caption_buffer );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

/* Return the main caption. 
 */
static const char *
iimage_generate_caption( iObject *iobject ) 
{
	iImage *iimage = IIMAGE( iobject );
	Imageinfo *ii = iimage->value.ii;
	VipsBuf *buf = &iimage->caption_buffer;

	vips_buf_rewind( buf );

	image_value_caption( &iimage->value, buf );

	if( ii ) {
		vips_buf_appends( buf, ", " );
		iobject_info( IOBJECT( iimage->value.ii ), buf );
	}

	return( vips_buf_all( buf ) );
}

static void
iimage_info( iObject *iobject, VipsBuf *buf )
{
	iImage *iimage = IIMAGE( iobject );
	Imageinfo *ii = iimage->value.ii;
	IMAGE *im;

	if( ii && (im = imageinfo_get( FALSE, ii )) ) {
		char *filename;

		if( im_header_get_typeof( im, ORIGINAL_FILENAME ) != 0 ) {
			if( !im_header_string( im, 
				ORIGINAL_FILENAME, &filename ) ) {
				vips_buf_appends( buf, 
					_( "Original filename" ) );
				vips_buf_appendf( buf, ": %s\n", filename );
			}
		}
	}
}

static View *
iimage_view_new( Model *model, View *parent )
{
	return( iimageview_new() );
}

static void
iimage_edit( GtkWidget *parent, Model *model )
{
        iImage *iimage = IIMAGE( model );

	if( iimage->value.ii ) 
		(void) imageview_new( iimage, parent );
}

void
iimage_header_dialog( GtkWidget *parent, Model *model )
{
        iImage *iimage = IIMAGE( model );
	Row *row = HEAPMODEL( iimage )->row;
	Workspace *ws = row_get_workspace( row );

	GtkWidget *imageheader;
	char txt[512];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	imageheader = imageheader_new( iimage );
	row_qualified_name_relative( ws->sym, row, &buf );
	iwindow_set_title( IWINDOW( imageheader ), 
		_( "Header for \"%s\"" ), vips_buf_all( &buf ) );
	idialog_set_callbacks( IDIALOG( imageheader ), NULL, NULL, NULL, NULL );
	idialog_add_ok( IDIALOG( imageheader ), iwindow_true_cb, _( "OK" ) );
	iwindow_set_parent( IWINDOW( imageheader ), parent );
	idialog_set_iobject( IDIALOG( imageheader ), IOBJECT( iimage ) );
	iwindow_build( IWINDOW( imageheader ) );

	gtk_widget_show( imageheader );
}

static xmlNode *
iimage_save( Model *model, xmlNode *xnode )
{
	iImage *iimage = IIMAGE( model );
	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	/* We always rebuild the value from the expr ... don't save.
	 */
	if( !set_prop( xthis, "image_left", "%d", iimage->image_left ) ||
		!set_prop( xthis, "image_top", "%d", iimage->image_top ) ||
		!set_prop( xthis, "image_mag", "%d", iimage->image_mag ) ||
		!set_sprop( xthis, "show_status",
			bool_to_char( iimage->show_status ) ) ||
		!set_sprop( xthis, "show_paintbox",
			bool_to_char( iimage->show_paintbox ) ) ||
		!set_sprop( xthis, "show_convert",
			bool_to_char( iimage->show_convert ) ) ||
		!set_sprop( xthis, "show_rulers",
			bool_to_char( iimage->show_rulers ) ) ||
		!set_dprop( xthis, "scale", iimage->scale ) ||
		!set_dprop( xthis, "offset", iimage->offset ) ||
		!set_sprop( xthis, "falsecolour", 
			bool_to_char( iimage->falsecolour ) ) ||
		!set_sprop( xthis, "type", bool_to_char( iimage->type ) ) )
		return( NULL );

	return( xthis );
}

static gboolean
iimage_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
        iImage *iimage = IIMAGE( model );

	g_assert( IS_RHS( parent ) );

	(void) get_iprop( xnode, "image_left", &iimage->image_left );
	(void) get_iprop( xnode, "image_top", &iimage->image_top );
	(void) get_iprop( xnode, "image_mag", &iimage->image_mag );
	(void) get_bprop( xnode, "show_status", &iimage->show_status );
	(void) get_bprop( xnode, "show_paintbox", &iimage->show_paintbox );
	(void) get_bprop( xnode, "show_convert", &iimage->show_convert );
	(void) get_bprop( xnode, "show_rulers", &iimage->show_rulers );
	(void) get_dprop( xnode, "scale", &iimage->scale );
	(void) get_dprop( xnode, "offset", &iimage->offset );
	(void) get_bprop( xnode, "falsecolour", &iimage->falsecolour );
	(void) get_bprop( xnode, "type", &iimage->type );

	return( MODEL_CLASS( parent_class )->load( model, 
		state, parent, xnode ) );
}

/* Need to implement _update_heap(), as not all model fields are directly
 * editable ... some are set only from expr. See also iregion.c.
 */
static void *
iimage_update_heap( Heapmodel *heapmodel )
{
	Expr *expr = heapmodel->row->expr;
        iImage *iimage = IIMAGE( heapmodel );
	ImageValue *value = &iimage->value;

	PElement pe;
	Imageinfo *ii;

#ifdef DEBUG
	printf( "iimage_update_heap: " );
	row_name_print( HEAPMODEL( iimage )->row );
	printf( "\n" );
#endif /*DEBUG*/

	/* Read the heap into the model, over the top of the unapplied edits.
	 */
	if( !class_get_exact( &expr->root, IOBJECT( heapmodel )->name, &pe ) )
		return( FALSE );
	if( !class_get_member_image( &pe, MEMBER_VALUE, &ii ) )
		return( FALSE );
	image_value_set( value, ii );

	IM_FREE( CLASSMODEL( iimage )->filename );

        if( value->ii && imageinfo_is_from_file( value->ii ) ) 
                IM_SETSTR( CLASSMODEL( iimage )->filename, 
                        IOBJECT( value->ii )->name );

	/* Classmodel _update_heap() will do _instance_new() from the fixed up
	 * model.
	 */
	return( HEAPMODEL_CLASS( parent_class )->update_heap( heapmodel ) );
}

/* Update iImage from heap.
 */
static gboolean
iimage_class_get( Classmodel *classmodel, PElement *root )
{
        iImage *iimage = IIMAGE( classmodel );
	ImageValue *value = &iimage->value;

	Imageinfo *ii;

#ifdef DEBUG
	printf( "iimage_class_get: " );
	row_name_print( HEAPMODEL( iimage )->row );
	printf( "\n" );
#endif /*DEBUG*/

	if( !class_get_member_image( root, MEMBER_VALUE, &ii ) )
		return( FALSE );
	image_value_set( value, ii );

	/* Try to update the filename for this row ... get from the meta if we
	 * can.
	 */
	IM_FREE( classmodel->filename );
        if( ii ) {
		IMAGE *im;
		char *filename;

		if( (im = imageinfo_get( FALSE, ii )) &&
			im_header_get_typeof( im, ORIGINAL_FILENAME ) != 0 ) {
			if( im_header_string( im, 
				ORIGINAL_FILENAME, &filename ) )
				return( FALSE );
		}
		else if( imageinfo_is_from_file( ii ) )
			filename = IOBJECT( ii )->name;
		else
			filename = NULL;

		IM_SETSTR( classmodel->filename, filename ); 
	}

	return( CLASSMODEL_CLASS( parent_class )->class_get( 
		classmodel, root ) );
}

/* Make a new "fn value" application.
 */
static gboolean
iimage_class_new( Classmodel *classmodel, PElement *fn, PElement *out )
{
	Heap *heap = reduce_context->heap;
        iImage *iimage = IIMAGE( classmodel );
	ImageValue *value = &iimage->value;

	PElement rhs;

#ifdef DEBUG
	printf( "iimage_class_new: " );
	row_name_print( HEAPMODEL( iimage )->row );
	printf( "\n" );
#endif /*DEBUG*/

	/* Make application nodes.
	 */
	heap_appl_init( out, fn );
	if( !heap_appl_add( heap, out, &rhs ) )
		return( FALSE );

	PEPUTP( &rhs, ELEMENT_MANAGED, value->ii );

	return( TRUE );
}

static gboolean
iimage_graphic_save( Classmodel *classmodel, 
	GtkWidget *parent, const char *filename )
{
	iImage *iimage = IIMAGE( classmodel );
	ImageValue *value = &iimage->value;
	char buf[MAX_STRSIZE];

	/* Can't happen nested-ly, so a static is OK. 
	 */
	static GTimer *timer = NULL;

	strcpy( buf, filename );
	filesel_add_mode( buf );

	if( !timer )
		timer = g_timer_new();
	g_timer_reset( timer );

	if( value->ii )
		if( !imageinfo_write( value->ii, buf ) )
			return( FALSE );

	mainw_recent_add( &mainw_recent_image, filename );

	if( main_option_time_save ) {
		double elapsed;

		elapsed = g_timer_elapsed( timer, NULL );
		error_top( _( "Save timer." ) );
		error_sub( _( "Image save took %g seconds." ), elapsed );

		return( FALSE );
	}

	return( TRUE );
}

gboolean
iimage_replace( iImage *iimage, const char *filename )
{
	Row *row = HEAPMODEL( iimage )->row;
	iText *itext = ITEXT( HEAPMODEL( iimage )->rhs->itext );
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	vips_buf_appends( &buf, "Image_file \"" );
	vips_buf_appendsc( &buf, TRUE, filename );
	vips_buf_appends( &buf, "\"" );

	if( itext_set_formula( itext, vips_buf_all( &buf ) ) ) {
		itext_set_edited( itext, TRUE );
		filemodel_set_modified( FILEMODEL( row->ws ), TRUE );
		(void) expr_dirty( row->expr, link_serial_new() );

		mainw_recent_add( &mainw_recent_image, filename );
	}

	return( TRUE );
}

static gboolean
iimage_graphic_replace( Classmodel *classmodel, 
	GtkWidget *parent, const char *filename )
{
	return( iimage_replace( IIMAGE( classmodel ), filename ) );
}

static void
iimage_class_init( iImageClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = iimage_dispose;
	gobject_class->finalize = iimage_finalize;

	iobject_class->generate_caption = iimage_generate_caption;
	iobject_class->info = iimage_info;

	model_class->view_new = iimage_view_new;
	model_class->edit = iimage_edit;
	model_class->info = iimage_header_dialog;
	model_class->save = iimage_save;
	model_class->load = iimage_load;

	heapmodel_class->update_heap = iimage_update_heap;

	classmodel_class->class_get = iimage_class_get;
	classmodel_class->class_new = iimage_class_new;

	classmodel_class->graphic_save = iimage_graphic_save;
	classmodel_class->graphic_replace = iimage_graphic_replace;

	classmodel_class->filetype = filesel_type_image;
	classmodel_class->filetype_pref = "IMAGE_FILE_TYPE";

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
iimage_init( iImage *iimage )
{
	image_value_init( &iimage->value, CLASSMODEL( iimage ) );

	iimage->classmodels = NULL;

	iimage->views = NULL;

	iimage->image_left = 0;
	iimage->image_top = 0;
	iimage->image_mag = 0;

	iimage->show_status = FALSE;
	iimage->show_paintbox = FALSE;
	iimage->show_convert = FALSE;
	iimage->show_rulers = FALSE;

	iimage->scale = 0.0;
	iimage->offset = 0.0;
	iimage->falsecolour = FALSE;
	iimage->type = TRUE;

	vips_buf_init_dynamic( &iimage->caption_buffer, MAX_LINELENGTH );

	iobject_set( IOBJECT( iimage ), CLASS_IMAGE, NULL );
}

GtkType
iimage_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( iImageClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) iimage_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( iImage ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) iimage_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"iImage", &info, 0 );
	}

	return( type );
}
