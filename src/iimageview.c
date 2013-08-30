/* run the display for an image in a workspace 
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

static GraphicviewClass *parent_class = NULL;

static void
iimageview_realize( GtkWidget *widget )
{
	GTK_WIDGET_CLASS( parent_class )->realize( widget );

	/* Mark us as a symbol drag-to widget. 
	 */
	set_symbol_drag_type( widget );
}

GtkWidget *
iimageview_drag_window_new( int width, int height )
{
	GtkWidget *window;

	window = gtk_window_new( GTK_WINDOW_POPUP );
	gtk_widget_set_app_paintable( GTK_WIDGET( window ), TRUE );
	gtk_widget_set_size_request( window, width, height );
	gtk_widget_realize( window );
#ifdef HAVE_SET_OPACITY
	gdk_window_set_opacity( window->window, 0.5 );
#endif /*HAVE_SET_OPACITY*/

	return( window );
}

static void
iimageview_drag_begin( GtkWidget *widget, GdkDragContext *context )
{
	iImageview *iimageview = IIMAGEVIEW( widget );
	Conversion *conv = iimageview->conv;
	GtkWidget *window;
	Imagedisplay *id;

#ifdef DEBUG
	printf( "iimageview_drag_begin: \n" );
#endif /*DEBUG*/

	window = iimageview_drag_window_new( 
		conv->canvas.width, conv->canvas.height );
	gtk_object_set_data_full( GTK_OBJECT( widget ),
		"nip2-drag-window", window,
		(GtkDestroyNotify) gtk_widget_destroy );
	id = imagedisplay_new( conv );
	gtk_container_add( GTK_CONTAINER( window ), GTK_WIDGET( id ) );
	gtk_widget_show( GTK_WIDGET( id ) );
	gtk_drag_set_icon_widget( context, window, -2, -2 );
}

static void
iimageview_drag_end( GtkWidget *widget, GdkDragContext *context )
{
#ifdef DEBUG
	printf( "iimageview_drag_end:\n" );
#endif /*DEBUG*/

	gtk_object_set_data( GTK_OBJECT( widget ), 
		"nip2-drag-window", NULL );
}

static void
iimageview_drag_data_get( GtkWidget *widget, GdkDragContext *context,
	GtkSelectionData *selection_data, guint info, guint time ) 
{
#ifdef DEBUG
	printf( "iimageview_drag_data_get:\n" );
#endif /*DEBUG*/

	if( info == TARGET_SYMBOL ) {
		iImageview *iimageview = IIMAGEVIEW( widget );
		iImage *iimage = IIMAGE( VOBJECT( iimageview )->iobject );
		Row *row = HEAPMODEL( iimage )->row;
		char txt[256];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		/* Drag the fully-qualified row name.
		 */
		row_qualified_name_relative( main_workspaceroot->sym, 
			row, &buf );
		gtk_selection_data_set( selection_data,
			gdk_atom_intern( "text/symbol", FALSE ), 8, 
			(guchar *) vips_buf_all( &buf ), 
			strlen( vips_buf_all( &buf ) ) );
	}
}

static void
iimageview_drag_data_received( GtkWidget *widget, GdkDragContext *context,
	gint x, gint y, GtkSelectionData *selection_data,
	guint info, guint time ) 
{

#ifdef DEBUG
	printf( "iimageview_drag_data_received:\n" );
#endif /*DEBUG*/

	if( info == TARGET_SYMBOL && selection_data->length > 0 && 
		selection_data->format == 8 ) {
		const char *from_row_path = (const char *) selection_data->data;
		iImageview *iimageview = IIMAGEVIEW( widget );
		iImage *iimage = IIMAGE( VOBJECT( iimageview )->iobject );
		Row *row = HEAPMODEL( iimage )->row;
		Row *from_row;

#ifdef DEBUG
		printf( " seen TARGET_SYMBOL \"%s\"\n", 
			from_row_path );
#endif /*DEBUG*/

		/* Block drags to ourselves ... pointless.
		 */
		if( (from_row = row_parse_name( main_workspaceroot->sym,
			from_row_path )) && 
			from_row != row ) {
			iText *itext = ITEXT( HEAPMODEL( iimage )->rhs->itext );
			char txt[256];
			VipsBuf buf = VIPS_BUF_STATIC( txt );

			/* Qualify relative to us. We don't want to embed
			 * workspace names unless we have to.
			 */
			if( row->top_row->sym ) 
				row_qualified_name_relative( row->top_row->sym, 
					from_row, &buf );

			if( itext_set_formula( itext, vips_buf_all( &buf ) ) ) {
				itext_set_edited( itext, TRUE );
				(void) expr_dirty( row->expr, 
					link_serial_new() );
				workspace_set_modified( row->ws, TRUE );
				symbol_recalculate_all();
			}

			/* Usually the drag-from row will be selected, very
			 * annoying. Select the drag-to row.
			 */
			row_select( row );
		}
	}
}

/* Not the same as model->edit :-( if this is a region, don't pop the region
 * edit box, pop a viewer on the image.
 */
static void
iimageview_edit( GtkWidget *parent, iImageview *iimageview )
{
	iImage *iimage = IIMAGE( VOBJECT( iimageview )->iobject );

	if( IS_IREGION( iimage ) && iimage->value.ii ) 
		imageview_new( iimage, parent );
	else 
		model_edit( parent, MODEL( iimage ) );
}

static void
iimageview_link( View *view, Model *model, View *parent )
{
	iImageview *iimageview = IIMAGEVIEW( view );

	Rowview *rview;

	VIEW_CLASS( parent_class )->link( view, model, parent );

	if( (rview = ROWVIEW( parent->parent )) ) { 
		Row *row = ROW( VOBJECT( rview )->iobject );

		rowview_menu_attach( rview, GTK_WIDGET( iimageview->id ) );

		if( row->popup && row->top_row == row ) {
			row->popup = FALSE;
			iimageview_edit( GTK_WIDGET( view ), iimageview );
		}
	}
}

static void 
iimageview_refresh( vObject *vobject )
{
	iImageview *iimageview = IIMAGEVIEW( vobject );
	iImage *iimage = IIMAGE( vobject->iobject );
	Row *row = HEAPMODEL( iimage )->row;

	int w, h;
	gboolean enabled;
	double scale, offset;
	gboolean falsecolour, type;

#ifdef DEBUG
	printf( "iimageview_refresh\n" );
#endif /*DEBUG*/

	w = IM_MAX( GTK_WIDGET( iimageview->id )->requisition.width,
		DISPLAY_THUMBNAIL );
	h = DISPLAY_THUMBNAIL;
	conversion_set_image( iimageview->conv, iimage->value.ii );
	gtk_widget_set_size_request( GTK_WIDGET( iimageview->id ), w, h );
	gtk_widget_queue_draw( GTK_WIDGET( iimageview->id ) );

	set_gcaption( iimageview->label, "%s", 
		NN( IOBJECT( iimage )->caption ) );

	/* Set scale/offset for the thumbnail. Use the prefs setting, or if
	 * there's a setting for this image, override with that.
	 */
	enabled = DISPLAY_CONVERSION;
	scale = row->ws->scale;
	offset = row->ws->offset;
	falsecolour = FALSE;
	type = TRUE;

	/* If the image_width has been set, a viewer must have popped down and
	 * set it, so the recorded settings must be valid.
	 */
	if( MODEL( iimage )->window_width != -1 ) {
		enabled = iimage->show_convert;
		scale = iimage->scale;
		offset = iimage->offset;
		falsecolour = iimage->falsecolour;
		type = iimage->type;
	}

	conversion_set_params( iimageview->conv, 
		enabled, scale, offset, falsecolour, type );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
iimageview_class_init( iImageviewClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	widget_class->realize = iimageview_realize;
	widget_class->drag_begin = iimageview_drag_begin;
	widget_class->drag_end = iimageview_drag_end;
	widget_class->drag_data_get = iimageview_drag_data_get;
	widget_class->drag_data_received = iimageview_drag_data_received;

	vobject_class->refresh = iimageview_refresh;

	view_class->link = iimageview_link;
}

static void 
iimageview_doubleclick_one_cb( GtkWidget *widget, GdkEvent *event, 
	iImageview *iimageview )
{
	Heapmodel *heapmodel = HEAPMODEL( VOBJECT( iimageview )->iobject );
	Row *row = heapmodel->row;

	row_select_modifier( row, event->button.state );
}

static void 
iimageview_doubleclick_two_cb( GtkWidget *widget, GdkEvent *event, 
	iImageview *iimageview )
{
	iimageview_edit( widget, iimageview );
}

static gboolean
iimageview_filedrop( iImageview *iimageview, const char *file )
{
	iImage *iimage = IIMAGE( VOBJECT( iimageview )->iobject );
	gboolean result;

	if( (result = iimage_replace( iimage, file )) )
		symbol_recalculate_all();

	return( result );
}

static void
iimageview_tooltip_generate( GtkWidget *widget, 
	VipsBuf *buf, iImageview *iimageview )
{
	iImage *iimage = IIMAGE( VOBJECT( iimageview )->iobject );
	Imageinfo *ii = iimage->value.ii;
	IMAGE *im = imageinfo_get( FALSE, ii );

	vips_buf_rewind( buf );
	vips_buf_appends( buf, vips_buf_all( &iimage->caption_buffer ) );
	if( im ) {
		double size = (double) im->Ysize * IM_IMAGE_SIZEOF_LINE( im );

		vips_buf_appends( buf, ", " );
		vips_buf_append_size( buf, size );
		vips_buf_appendf( buf, ", %.3gx%.3g p/mm", im->Xres, im->Yres );
	}
}

static void
iimageview_init( iImageview *iimageview )
{
	GtkWidget *eb;
	GtkWidget *vbox;

#ifdef DEBUG
	printf( "iimageview_init\n" );
#endif /*DEBUG*/

        eb = gtk_event_box_new();
        gtk_box_pack_start( GTK_BOX( iimageview ), eb, FALSE, FALSE, 0 );
	vbox = gtk_vbox_new( FALSE, 0 );
        gtk_container_add( GTK_CONTAINER( eb ), vbox );
        gtk_widget_show( vbox );

	iimageview->conv = conversion_new( NULL );
	iimageview->conv->tile_size = 16;
        iimageview->id = imagedisplay_new( iimageview->conv );
	imagedisplay_set_shrink_to_fit( iimageview->id, TRUE );
        gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( iimageview->id ), FALSE, FALSE, 0 );
	gtk_widget_show( GTK_WIDGET( iimageview->id ) );

	/* Need these events in the enclosing workspaceview.
	 */
	gtk_widget_add_events( GTK_WIDGET( iimageview->id ), 
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK ); 

	iimageview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( iimageview->label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( iimageview->label ), 2, 0 );
        gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( iimageview->label ), FALSE, FALSE, 0 );
	gtk_widget_show( GTK_WIDGET( iimageview->label ) );

	/* Set as file drop destination 
	 */
	filedrop_register( GTK_WIDGET( iimageview ), 
		(FiledropFunc) iimageview_filedrop, iimageview );

	doubleclick_add( GTK_WIDGET( iimageview ), FALSE,
		DOUBLECLICK_FUNC( iimageview_doubleclick_one_cb ), iimageview,
		DOUBLECLICK_FUNC( iimageview_doubleclick_two_cb ), iimageview );

        set_tooltip_generate( eb,
		(TooltipGenerateFn) iimageview_tooltip_generate, 
		iimageview, NULL );

	gtk_widget_set_name( eb, "caption_widget" );
        gtk_widget_show( GTK_WIDGET( eb ) );
}

GtkType
iimageview_get_type( void )
{
	static GtkType iimageview_type = 0;

	if( !iimageview_type ) {
		static const GtkTypeInfo info = {
			"iImageview",
			sizeof( iImageview ),
			sizeof( iImageviewClass ),
			(GtkClassInitFunc) iimageview_class_init,
			(GtkObjectInitFunc) iimageview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		iimageview_type = gtk_type_unique( TYPE_GRAPHICVIEW, &info );
	}

	return( iimageview_type );
}

View *
iimageview_new( void )
{
	iImageview *iimageview = gtk_type_new( TYPE_IIMAGEVIEW );

	return( VIEW( iimageview ) );
}
