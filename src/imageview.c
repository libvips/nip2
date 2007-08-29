/* display an image in a window ... watching an iImage model.
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

/* Define to trace button press events.
#define EVENT
 */

#include "ip.h"

static FloatwindowClass *parent_class = NULL;

/* All the magnification menus we have.
 */
typedef struct _ImageviewMagmenu {
	const char *name;
	int mag;
} ImageviewMagmenu;

static const ImageviewMagmenu imageview_mags[] = {
	{ "Zoom6Mode", -16 },
	{ "Zoom12Mode", -8 },
	{ "Zoom25Mode", -4 },
	{ "Zoom50Mode", -2 },
	{ "Zoom100Mode", 1 },
	{ "Zoom200Mode", 2 },
	{ "Zoom400Mode", 4 },
	{ "Zoom800Mode", 8 },
	{ "Zoom1600Mode", 16 }
};

static void
imageview_destroy( GtkObject *object )
{
	Imageview *iv;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_IMAGEVIEW( object ) );

	iv = IMAGEVIEW( object );

#ifdef DEBUG
	printf( "imageview_destroy\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	UNREF( iv->imagemodel );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
imageview_class_init( ImageviewClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = imageview_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
imageview_init( Imageview *iv )
{
	iv->imagemodel = NULL;
}

GtkType
imageview_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Imageview",
			sizeof( Imageview ),
			sizeof( ImageviewClass ),
			(GtkClassInitFunc) imageview_class_init,
			(GtkObjectInitFunc) imageview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_FLOATWINDOW, &info );
	}

	return( type );
}

static void
imageview_refresh_title( Imageview *iv )
{
	Imagemodel *imagemodel = iv->imagemodel;
	iImage *iimage = imagemodel->iimage;
	Row *row = HEAPMODEL( iimage )->row;
	Workspace *ws = row_get_workspace( row );

	/* Can come here during ws destroy.
	 */
	if( ws ) {
		Conversion *conv = imagemodel->conv;
		Imageinfo *ii = iimage->value.ii;

		BufInfo buf;
		char txt[512];

		buf_init_static( &buf, txt, 512 );
		row_qualified_name_relative( ws->sym, row, &buf );

		if( ii && imageinfo_is_from_file( ii ) )
			buf_appendf( &buf, " - %s", IOBJECT( ii )->name );

		buf_appendf( &buf, " - %.0f%%", 
			100.0 * conversion_dmag( conv->mag ) );

		iwindow_set_title( IWINDOW( iv ), "%s", buf_all( &buf ) );
	}
}

/* The model has changed ... update our menus and titlebar.
 */
static void
imageview_imagemodel_changed_cb( Imagemodel *imagemodel, Imageview *iv )
{
	Conversion *conv = imagemodel->conv;

	GtkAction *action;
	int i;

	action = gtk_action_group_get_action( iv->action_group, 
		"Status" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		imagemodel->show_status );

	action = gtk_action_group_get_action( iv->action_group, 
		"Control" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		imagemodel->show_convert );

	action = gtk_action_group_get_action( iv->action_group, 
		"Paint" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		imagemodel->show_paintbox );

	action = gtk_action_group_get_action( iv->action_group, 
		"Rulers" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		imagemodel->show_rulers );

	for( i = 0; i < IM_NUMBER( imageview_mags ); i++ )
		if( conv->mag == imageview_mags[i].mag ) {
			action = gtk_action_group_get_action( iv->action_group,
				imageview_mags[i].name );
			gtk_toggle_action_set_active( 
				GTK_TOGGLE_ACTION( action ),
				TRUE );
			break;
		}

	imageview_refresh_title( iv );
}

/* The model has been destroyed ... kill us too.
 */
static void
imageview_imagemodel_destroy_cb( Imagemodel *imagemodel, Imageview *iv )
{
	gtk_widget_destroy( GTK_WIDGET( iv ) );
}

/* Region class names indexed by iRegionType.
 */
static const char *imageview_region_name[] = {
	CLASS_MARK,
	CLASS_HGUIDE,
	CLASS_VGUIDE,
	CLASS_ARROW,
	CLASS_REGION,
	CLASS_AREA
};

/* Look up a iRegionType from an action name.
 */
static iRegionType
imageview_get_region_type( GtkAction *action )
{
	/* Action names indexed by iRegionType.
	 */
	static const char *action_names[] = {
		"NewMark",
		"NewHGuide",
		"NewVGuide",
		"NewArrow",
		"NewRegion"
	};

	const char *name = gtk_action_get_name( action );

	int i;

	for( i = 0; i < IM_NUMBER( action_names ); i++ )
		if( strcmp( name, action_names[i] ) == 0 )
			return( (iRegionType) i );

	assert( FALSE );
}

static void
imageview_new_arrow2_action_cb( GtkAction *action, Imageview *iv )
{
	iRegionType rt = imageview_get_region_type( action );
	Imagemodel *imagemodel = iv->imagemodel;
	iImage *iimage = imagemodel->iimage;
	Row *row = HEAPMODEL( iimage )->row;
	Workspace *ws = row_get_workspace( row );
	Conversion *conv = imagemodel->conv;
	int dx = imagemodel->visible.left + imagemodel->visible.width / 2;
	int dy = imagemodel->visible.top + imagemodel->visible.height / 2;

	char txt[MAX_STRSIZE];
	BufInfo buf;
	Symbol *sym;
	int ix, iy;

	conversion_disp_to_im( conv, dx, dy, &ix, &iy );

	buf_init_static( &buf, txt, MAX_STRSIZE );
	buf_appendf( &buf, "%s ", imageview_region_name[rt] );
	row_qualified_name_relative( ws->sym, row, &buf );
	switch( rt ) {
	case IREGION_MARK:
		buf_appendf( &buf, " (%d) (%d)", ix, iy );
		break;

	case IREGION_HGUIDE:
		buf_appendf( &buf, " (%d)", iy );
		break;

	case IREGION_VGUIDE:
		buf_appendf( &buf, " (%d)", ix );
		break;

	default:
		assert( FALSE );
	}

	if( !(sym = workspace_add_def( ws, buf_all( &buf ) )) ) {
		box_alert( GTK_WIDGET( iv ) );
		return;
	}

	workspace_deselect_all( ws );
}

static void
imageview_new_arrow4_action_cb( GtkAction *action, Imageview *iv )
{
	iRegionType rt = imageview_get_region_type( action );
	Imagemodel *imagemodel = iv->imagemodel;
	iImage *iimage = imagemodel->iimage;
	Row *row = HEAPMODEL( iimage )->row;
	Workspace *ws = row_get_workspace( row );
	Conversion *conv = imagemodel->conv;

	Rect dr, ir;
	char txt[MAX_STRSIZE];
	BufInfo buf;
	Symbol *sym;
	Column *col;

	dr.left = imagemodel->visible.left + imagemodel->visible.width / 4;
	dr.top = imagemodel->visible.top + imagemodel->visible.height / 4;
	dr.width = imagemodel->visible.width / 2;
	dr.height = imagemodel->visible.height / 2;
	conversion_disp_to_im_rect( conv, &dr, &ir );

	buf_init_static( &buf, txt, MAX_STRSIZE );
	buf_appendf( &buf, "%s ", imageview_region_name[rt] );
	row_qualified_name_relative( ws->sym, row, &buf );
	buf_appendf( &buf, " (%d) (%d) %d %d", 
		ir.left, ir.top, ir.width, ir.height );

	if( !(sym = workspace_add_def( ws, buf_all( &buf ) )) ) {
		box_alert( GTK_WIDGET( iv ) );
		return;
	}

	col = sym->expr->row->top_col;
	model_scrollto( MODEL( col ), MODEL_SCROLL_BOTTOM );
}

static void
imageview_replace_action_cb( GtkAction *action, Imageview *iv )
{
	Imagemodel *imagemodel = iv->imagemodel;
	iImage *iimage = imagemodel->iimage;

	classmodel_graphic_replace( CLASSMODEL( iimage ), GTK_WIDGET( iv ) );
}

static void
imageview_save_action_cb( GtkAction *action, Imageview *iv )
{
	Imagemodel *imagemodel = iv->imagemodel;
	iImage *iimage = imagemodel->iimage;

	classmodel_graphic_save( CLASSMODEL( iimage ), GTK_WIDGET( iv ) );
}

static void
imageview_recalc_action_cb( GtkAction *action, Imageview *iv )
{
	Imagemodel *imagemodel = iv->imagemodel;
	iImage *iimage = imagemodel->iimage;
	Row *row = HEAPMODEL( iimage )->row;

        workspace_deselect_all( row->ws );
        row_select( row );
        if( !workspace_selected_recalc( row->ws ) )
                box_alert( GTK_WIDGET( iv ) );
        workspace_deselect_all( row->ws );
}

static void
imageview_header_action_cb( GtkAction *action, Imageview *iv )
{
	Imagemodel *imagemodel = iv->imagemodel;
	iImage *iimage = imagemodel->iimage;
	Row *row = HEAPMODEL( iimage )->row;
	Workspace *ws = row_get_workspace( row );
	GtkWidget *imageheader = imageheader_new( imagemodel->conv );
	BufInfo buf;
	char txt[512];

	buf_init_static( &buf, txt, 512 );
	row_qualified_name_relative( ws->sym, row, &buf );

	iwindow_set_title( IWINDOW( imageheader ), _( "Header for \"%s\"" ), 
		buf_all( &buf ) );
	idialog_set_callbacks( IDIALOG( imageheader ), 
		NULL, NULL, NULL, iv );
	idialog_add_ok( IDIALOG( imageheader ), iwindow_true_cb, _( "OK" ) );
	iwindow_set_parent( IWINDOW( imageheader ), GTK_WIDGET( iv ) );
	iwindow_build( IWINDOW( imageheader ) );

	gtk_widget_show( imageheader );
}

static void
imageview_zoom_in_action_cb( GtkAction *action, Imageview *iv )
{
	Imagemodel *imagemodel = iv->imagemodel;
	Conversion *conv = imagemodel->conv;

	conversion_set_mag( conv, conversion_double( conv->mag ) ); 
}

static void
imageview_zoom_out_action_cb( GtkAction *action, Imageview *iv )
{
	Imagemodel *imagemodel = iv->imagemodel;
	Conversion *conv = imagemodel->conv;

	conversion_set_mag( conv, conversion_halve( conv->mag ) ); 
}

static void
imageview_zoom_100_action_cb( GtkAction *action, Imageview *iv )
{
	if( iv->ip )
		imagepresent_zoom_to( iv->ip, 1 );
}

static void
imageview_zoom_fit_action_cb( GtkAction *action, Imageview *iv )
{
	imagepresent_zoom_to( iv->ip, 0 );
}

static void
imageview_show_status_action_cb( GtkToggleAction *action, Imageview *iv )
{
	imagemodel_set_status( iv->imagemodel, 
		gtk_toggle_action_get_active( action ) );
}

static void
imageview_show_convert_action_cb( GtkToggleAction *action, Imageview *iv )
{
	imagemodel_set_convert( iv->imagemodel, 
		gtk_toggle_action_get_active( action ) );
}

static void
imageview_show_paintbox_action_cb( GtkToggleAction *action, Imageview *iv )
{
	imagemodel_set_paintbox( iv->imagemodel, 
		gtk_toggle_action_get_active( action ) );
}

static void
imageview_show_rulers_action_cb( GtkToggleAction *action, Imageview *iv )
{
	imagemodel_set_rulers( iv->imagemodel, 
		gtk_toggle_action_get_active( action ) );
}

static void
imageview_mode_action_cb( GtkRadioAction *action, GtkRadioAction *current,
	Imageview *iv )
{
	ImagemodelState state = (ImagemodelState) 
		gtk_radio_action_get_current_value( action );

	imagemodel_set_state( iv->imagemodel, state, GTK_WIDGET( iv ) );
}

static void
imageview_mag_action_cb( GtkRadioAction *action, GtkRadioAction *current, 
	Imageview *iv )
{
	if( iv->ip ) 
		imagepresent_zoom_to( iv->ip, 
			gtk_radio_action_get_current_value( action ) );
}

/* Our actions.
 */
static GtkActionEntry imageview_actions[] = {
	/* Menu items.
	 */
	{ "FileMenu", NULL, "_File" },
	{ "FileNewMenu", NULL, "_New" },
	{ "ViewMenu", NULL, "_View" },
	{ "ViewToolbarMenu", NULL, "_Toolbar" },
	{ "ViewModeMenu", NULL, "M_ode" },
	{ "ViewZoomMenu", NULL, "_Zoom" },
	{ "HelpMenu", NULL, "_Help" },

	/* Actions.
	 */
	{ "NewMark", 
		GTK_STOCK_NEW, N_( "New _Mark" ), NULL, 
		N_( "Create a new mark" ), 
		G_CALLBACK( imageview_new_arrow2_action_cb ) },

	{ "NewHGuide", 
		GTK_STOCK_NEW, N_( "New _Horizontal Guide" ), NULL, 
		N_( "Create a new horizontal guide" ), 
		G_CALLBACK( imageview_new_arrow2_action_cb ) },

	{ "NewVGuide", 
		GTK_STOCK_NEW, N_( "New _Vertical Guide" ), NULL, 
		N_( "Create a new vertical guide" ), 
		G_CALLBACK( imageview_new_arrow2_action_cb ) },

	{ "NewArrow", 
		GTK_STOCK_NEW, N_( "New _Arrow" ), NULL, 
		N_( "Create a new arrow" ), 
		G_CALLBACK( imageview_new_arrow4_action_cb ) },

	{ "NewRegion", 
		GTK_STOCK_NEW, N_( "New _Region" ), NULL, 
		N_( "Create a new region" ), 
		G_CALLBACK( imageview_new_arrow4_action_cb ) },

	{ "Replace", 
		NULL, N_( "Replace Image" ), NULL, 
		N_( "Replace image from file" ), 
		G_CALLBACK( imageview_replace_action_cb ) },

	{ "SaveAs", 
		GTK_STOCK_SAVE_AS, N_( "Save Image As" ), NULL,
		N_( "Save image to file" ), 
		G_CALLBACK( imageview_save_action_cb ) },

	{ "Recalculate", 
		NULL, N_( "Recalculate" ), "<control>C",
		N_( "Recalculate image" ), 
		G_CALLBACK( imageview_recalc_action_cb ) },

	{ "Close", 
		GTK_STOCK_CLOSE, N_( "_Close" ), NULL,
		N_( "Close" ), 
		G_CALLBACK( iwindow_kill_action_cb ) },

	{ "Header", 
		NULL, N_( "Image _Header" ), NULL,
		N_( "View image header" ), 
		G_CALLBACK( imageview_header_action_cb ) },

	{ "ZoomIn",
		GTK_STOCK_ZOOM_IN, N_( "Zoom _In" ), "<control>plus",
		N_( "Zoom in on mouse cursor" ),
		G_CALLBACK( imageview_zoom_in_action_cb ) },

	{ "ZoomOut",
		GTK_STOCK_ZOOM_OUT, N_( "Zoom _Out" ), "<control>minus",
		N_( "Zoom out" ),
		G_CALLBACK( imageview_zoom_out_action_cb ) },

	{ "Zoom100",
		GTK_STOCK_ZOOM_100, N_( "Zoom _100%" ), "<control>equal",
		N_( "Zoom to 100%" ),
		G_CALLBACK( imageview_zoom_100_action_cb ) },

	{ "ZoomFit",
		GTK_STOCK_ZOOM_FIT, N_( "Zoom to _Fit" ), NULL,
		N_( "Zoom to fit image to window" ),
		G_CALLBACK( imageview_zoom_fit_action_cb ) },

	{ "Guide", 
		GTK_STOCK_HELP, N_( "_Contents" ), "F1",
		N_( "Open the users guide" ), 
		G_CALLBACK( mainw_guide_action_cb ) },

	{ "About", 
		NULL, N_( "_About" ), NULL,
		N_( "About this program" ), 
		G_CALLBACK( mainw_about_action_cb ) }
};

static GtkToggleActionEntry imageview_toggle_actions[] = {
	{ "Status",
		NULL, N_( "_Status" ), NULL,
		N_( "Show status bar" ),
		G_CALLBACK( imageview_show_status_action_cb ), TRUE },

	{ "Control",
		NULL, N_( "_Display Control" ), NULL,
		N_( "Show display control bar" ),
		G_CALLBACK( imageview_show_convert_action_cb ), TRUE },

	{ "Paint",
		NULL, N_( "_Paint" ), NULL,
		N_( "Show paint bar" ),
		G_CALLBACK( imageview_show_paintbox_action_cb ), FALSE },

	{ "Rulers",
		NULL, N_( "_Rulers" ), NULL,
		N_( "Show rulers" ),
		G_CALLBACK( imageview_show_rulers_action_cb ), FALSE }
};

static GtkRadioActionEntry imageview_mode_radio_actions[] = {
	{ "SelectMode",
		NULL, N_( "_Select" ), NULL,
		N_( "Select and modify selections" ),
		IMAGEMODEL_SELECT },

	{ "PanMode",
		NULL, N_( "_Pan" ), NULL,
		N_( "Pan image" ),
		IMAGEMODEL_PAN },

	{ "ZoomInMode",
		NULL, N_( "Zoom _In" ), NULL,
		N_( "Zoom in on mouse cursor" ),
		IMAGEMODEL_MAGIN },

	{ "ZoomOutMode",
		NULL, N_( "Zoom _Out" ), NULL,
		N_( "Zoom out" ),
		IMAGEMODEL_MAGOUT }
};

static GtkRadioActionEntry imageview_zoom_radio_actions[] = {
	{ "Zoom6Mode",
		NULL, N_( "6%" ), NULL, N_( "Zoom to 6%" ), -16 },
	{ "Zoom12Mode",
		NULL, N_( "12%" ), NULL, N_( "Zoom to 12%" ), -8 },
	{ "Zoom25Mode",
		NULL, N_( "25%" ), NULL, N_( "Zoom to 25%" ), -4 },
	{ "Zoom50Mode",
		NULL, N_( "50%" ), NULL, N_( "Zoom to 50%" ), -2 },
	{ "Zoom100Mode",
		NULL, N_( "100%" ), NULL, N_( "Zoom to 100%" ), 1 },
	{ "Zoom200Mode",
		NULL, N_( "200%" ), NULL, N_( "Zoom to 200%" ), 2 },
	{ "Zoom400Mode",
		NULL, N_( "400%" ), NULL, N_( "Zoom to 400%" ), 4 },
	{ "Zoom800Mode",
		NULL, N_( "800%" ), NULL, N_( "Zoom to 800%" ), 8 },
	{ "Zoom1600Mode",
		NULL, N_( "1600%" ), NULL, N_( "Zoom to 1600%" ), 16 }
};

static const char *imageview_menubar_ui_description =
"<ui>"
"  <menubar name='ImageviewMenubar'>"
"    <menu action='FileMenu'>"
"      <menu action='FileNewMenu'>"
"        <menuitem action='NewMark'/>"
"        <menuitem action='NewHGuide'/>"
"        <menuitem action='NewVGuide'/>"
"        <menuitem action='NewArrow'/>"
"        <menuitem action='NewRegion'/>"
"      </menu>"
"      <separator/>"
"      <menuitem action='Replace'/>"
"      <menuitem action='SaveAs'/>"
"      <separator/>"
"      <menuitem action='Recalculate'/>"
"      <separator/>"
"      <menuitem action='Close'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menu action='ViewToolbarMenu'>"
"        <menuitem action='Status'/>"
"        <menuitem action='Control'/>"
"        <menuitem action='Paint'/>"
"        <menuitem action='Rulers'/>"
"      </menu>"
"      <menu action='ViewModeMenu'>"
"        <menuitem action='SelectMode'/>"
"        <menuitem action='PanMode'/>"
"        <menuitem action='ZoomInMode'/>"
"        <menuitem action='ZoomOutMode'/>"
"      </menu>"
"      <menuitem action='Header'/>"
"      <separator/>"
"      <menuitem action='ZoomIn'/>"
"      <menuitem action='ZoomOut'/>"
"      <menuitem action='Zoom100'/>"
"      <menuitem action='ZoomFit'/>"
"      <menu action='ViewZoomMenu'>"
"        <menuitem action='Zoom6Mode'/>"
"        <menuitem action='Zoom12Mode'/>"
"        <menuitem action='Zoom25Mode'/>"
"        <menuitem action='Zoom50Mode'/>"
"        <menuitem action='Zoom100Mode'/>"
"        <menuitem action='Zoom200Mode'/>"
"        <menuitem action='Zoom400Mode'/>"
"        <menuitem action='Zoom800Mode'/>"
"        <menuitem action='Zoom1600Mode'/>"
"      </menu>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

/* Spot mouse motion events, to update status bar.
 */
static gint
imageview_event( GtkWidget *widget, GdkEvent *event, Imageview *iv )
{
#ifdef EVENT
	if( event->type == GDK_BUTTON_PRESS )
		printf( "imageview_event: GDK_BUTTON_PRESS\n" );
#endif /*EVENT*/

	if( event->type == GDK_MOTION_NOTIFY ) { 
		Imagemodel *imagemodel = iv->imagemodel;
		Conversion *conv = imagemodel->conv;
		int ix, iy;

		conversion_disp_to_im( conv, 
			event->button.x, event->button.y, &ix, &iy );

		statusview_mouse( iv->sv, ix, iy );
	}

	return( FALSE );
}

static gboolean
imageview_filedrop( Imageview *iv, const char *file )
{
	gboolean result;

	if( (result = iimage_replace( iv->imagemodel->iimage, file )) )
		symbol_recalculate_all();

	return( result );
}

static void
imageview_build( Imageview *iv, GtkWidget *vbox, iImage *iimage )
{
	GtkAccelGroup *accel_group;
	GError *error;
	GtkWidget *mbar;
	GtkWidget *frame;
	GList *focus_chain;

	/* All the model parts for our set of views.
	 */
	iv->imagemodel = imagemodel_new( iimage );
	g_object_ref( G_OBJECT( iv->imagemodel ) );
	iobject_sink( IOBJECT( iv->imagemodel ) );
	iv->imagemodel_changed_sid = g_signal_connect( 
		G_OBJECT( iv->imagemodel ), "changed", 
		G_CALLBACK( imageview_imagemodel_changed_cb ), iv );
	iv->imagemodel_destroy_sid = g_signal_connect( 
		G_OBJECT( iv->imagemodel ), "destroy", 
		G_CALLBACK( imageview_imagemodel_destroy_cb ), iv );

        /* Make main menu bar
         */
	iv->action_group = gtk_action_group_new( "ImageviewActions" );
	gtk_action_group_set_translation_domain( iv->action_group, 
		GETTEXT_PACKAGE );
	gtk_action_group_add_actions( iv->action_group, 
		imageview_actions, G_N_ELEMENTS( imageview_actions ), 
		GTK_WINDOW( iv ) );
	gtk_action_group_add_toggle_actions( iv->action_group, 
		imageview_toggle_actions, 
			G_N_ELEMENTS( imageview_toggle_actions ), 
		GTK_WINDOW( iv ) );
	gtk_action_group_add_radio_actions( iv->action_group,
		imageview_mode_radio_actions, 
			G_N_ELEMENTS( imageview_mode_radio_actions ), 
		IMAGEMODEL_SELECT,
		G_CALLBACK( imageview_mode_action_cb ),
		GTK_WINDOW( iv ) );
	gtk_action_group_add_radio_actions( iv->action_group,
		imageview_zoom_radio_actions, 
			G_N_ELEMENTS( imageview_zoom_radio_actions ), 
		1,
		G_CALLBACK( imageview_mag_action_cb ),
		GTK_WINDOW( iv ) );

	iv->ui_manager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group( iv->ui_manager, 
		iv->action_group, 0 );

	accel_group = gtk_ui_manager_get_accel_group( iv->ui_manager );
	gtk_window_add_accel_group( GTK_WINDOW( iv ), accel_group );

	error = NULL;
	if( !gtk_ui_manager_add_ui_from_string( iv->ui_manager,
			imageview_menubar_ui_description, -1, &error ) ) {
		g_message( "building menus failed: %s", error->message );
		g_error_free( error );
		exit( EXIT_FAILURE );
	}

	mbar = gtk_ui_manager_get_widget( iv->ui_manager, 
		"/ImageviewMenubar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mbar, FALSE, FALSE, 0 );
        gtk_widget_show( mbar );

	/* Status bar.
	 */
	iv->sv = statusview_new( iv->imagemodel );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( iv->sv ), FALSE, FALSE, 0 );

	/* Conversion bar.
	 */
	iv->cv = conversionview_new( iv->imagemodel );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( iv->cv ), FALSE, FALSE, 0 );

	/* Paintbox bar.
	 */
	iv->pbv = paintboxview_new( iv->imagemodel );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( iv->pbv ), FALSE, FALSE, 0 );

	/* Image area. 
	 */
	frame = gtk_frame_new( NULL );
	gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_OUT );
	gtk_widget_show( frame );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( frame ), TRUE, TRUE, 0 );
	iv->ip = imagepresent_new( iv->imagemodel );
	gtk_container_add( GTK_CONTAINER( frame ), GTK_WIDGET( iv->ip ) );
	gtk_widget_show( GTK_WIDGET( iv->ip ) );
	gtk_signal_connect( GTK_OBJECT( iv->ip->id ), "event",
		GTK_SIGNAL_FUNC( imageview_event ), iv );

	/* Position and size to restore?
	 */
	if( CLASSMODEL( iimage )->window_width != -1 ) {
		/* Floatwindow will set pos/size.
		 */
		iv->imagemodel->show_status = iimage->show_status;
		iv->imagemodel->show_paintbox = iimage->show_paintbox;
		iv->imagemodel->show_convert = iimage->show_convert;
		iv->imagemodel->show_rulers = iimage->show_rulers;
		iv->imagemodel->scale = iimage->scale;
		iv->imagemodel->offset = iimage->offset;
		iv->imagemodel->falsecolour = iimage->falsecolour;
		iv->imagemodel->type = iimage->type;

		/* Our caller must call imagepresent_set_mag_pos() after
		 * _show(). Not accurate if we set it here.
		 */
	}
	else {
		int w, h; 

		/* Set initial size. This is really hard to do right :-( These
		 * magic numbers will break with different themes.

		 	FIXME ... maybe realize the window but don't map it,
			calculate border size, then set default size and map?
			yuk!

			the magic numbers here are hard to derive, there are 
			many, many widgets piled up together to make this 
			window

			last set correctly for clearlooks

		 */
		w = IM_MIN( IMAGE_WINDOW_WIDTH, 
			iv->imagemodel->conv->image.width + 14 );
		h = IM_MIN( IMAGE_WINDOW_HEIGHT, 
			iv->imagemodel->conv->image.height + 39 );
		gtk_window_set_default_size( GTK_WINDOW( iv ), w, h );
		conversion_set_mag( iv->imagemodel->conv, 1 );
	}

	/* Set as file drop destination 
	 */
	filedrop_register( GTK_WIDGET( iv ), 
		(FiledropFunc) imageview_filedrop, iv );

	/* Override the focus_chain ... we want the imagedisplay first.
	 */
	focus_chain = NULL;
	focus_chain = g_list_append( focus_chain, iv->ip );
	focus_chain = g_list_append( focus_chain, iv->cv );
	focus_chain = g_list_append( focus_chain, iv->pbv );
	gtk_container_set_focus_chain( GTK_CONTAINER( vbox ), focus_chain );
	gtk_widget_grab_focus( GTK_WIDGET( iv->ip->id ) );
}

static void *
imageview_add_region( Classmodel *classmodel, Imageview *iv )
{
	iRegionInstance *instance;

	if( MODEL( classmodel )->display &&
		(instance = classmodel_get_instance( classmodel )) ) {
		Regionview *regionview = regionview_new( classmodel, 
			&instance->area, iv->ip );
		PElement *root = &HEAPMODEL( classmodel )->row->expr->root;

		/* Look at the class we are drawing, set the display type.
		 */
		regionview_set_type( regionview, root );
	}

	return( NULL );
}

static void
imageview_popdown( iWindow *iwnd, void *client,
	iWindowNotifyFn nfn, void *sys )
{
	Imageview *iv = IMAGEVIEW( iwnd );
	Imagemodel *imagemodel = iv->imagemodel;
	iImage *iimage = imagemodel->iimage;
	Conversion *conv = imagemodel->conv;

	/* We have to note position/size in popdown rather than destroy, since
	 * the widgets have to all still be extant.
	 */

	/* Save the centre of the window in image cods.
	 */
	conversion_disp_to_im( conv, 
		imagemodel->visible.left + imagemodel->visible.width / 2, 
		imagemodel->visible.top + imagemodel->visible.height / 2,
		&iimage->image_left, &iimage->image_top );
	iimage->image_mag = conv->mag;

	iimage->show_status = imagemodel->show_status;
	iimage->show_paintbox = imagemodel->show_paintbox;
	iimage->show_rulers = imagemodel->show_rulers;

	/* Signal changed on iimage if we save the convert settings. This will
	 * make the thumbnail update.
	 */
	if( iimage->show_convert != imagemodel->show_convert ||
		iimage->scale != imagemodel->scale ||
		iimage->offset != imagemodel->offset ||
		iimage->falsecolour != imagemodel->falsecolour ||
		iimage->type != imagemodel->type ) {
		iimage->show_convert = imagemodel->show_convert;
		iimage->scale = imagemodel->scale;
		iimage->offset = imagemodel->offset;
		iimage->falsecolour = imagemodel->falsecolour;
		iimage->type = imagemodel->type;
		iobject_changed( IOBJECT( iimage ) );
	}

	nfn( sys, IWINDOW_TRUE );
}

static void
imageview_link( Imageview *iv, iImage *iimage, GtkWidget *parent )
{
	iwindow_set_build( IWINDOW( iv ), 
		(iWindowBuildFn) imageview_build, iimage, NULL, NULL );
	iwindow_set_popdown( IWINDOW( iv ), imageview_popdown, NULL );
	iwindow_set_parent( IWINDOW( iv ), parent );
	floatwindow_link( FLOATWINDOW( iv ), CLASSMODEL( iimage ) );
	iwindow_build( IWINDOW( iv ) );
	slist_map( iimage->classmodels,
		(SListMapFn) imageview_add_region, iv );

	/* Initial "changed" on the model to get all views to init.
	 */
	iobject_changed( IOBJECT( iv->imagemodel ) );
}

Imageview *
imageview_new( iImage *iimage, GtkWidget *parent )
{
	Imageview *iv = gtk_type_new( TYPE_IMAGEVIEW );

	imageview_link( iv, iimage, parent );

	/* This is odd ... we wouldn't normally _show() the widget in _new(),
	 * but restoring the scroll position doesn't work unless the window is
	 * visible. We have to show here.
	 */
	gtk_widget_show( GTK_WIDGET( iv ) );

	if( CLASSMODEL( iimage )->window_width != -1 ) 
		imagepresent_set_mag_pos( iv->ip, 
			iimage->image_mag,
			iimage->image_left, iimage->image_top );

	return( iv );
}

/* Make an imageview, and try to make area (image cods) visible. width/height
 * can be -ve
 */
Imageview *
imageview_new_area( iImage *iimage, Rect *area, GtkWidget *parent )
{
	Imageview *iv = imageview_new( iimage, parent );
	Imagemodel *imagemodel = iv->imagemodel;
	Conversion *conv = imagemodel->conv;
	int shrink_x, shrink_y, shrink;

	/* Calculate a shrink factor which should make all the region 
	 * visible ... don't zoom.
	 */
	shrink_x = (abs( area->width ) + conv->canvas.width) / 
		conv->canvas.width;
	shrink_y = (abs( area->height ) + conv->canvas.height) / 
		conv->canvas.height;
	shrink = -IM_MAX( 1, IM_MAX( shrink_x, shrink_y ) );
	if( shrink == -1 )
		shrink = 1;

	imagepresent_set_mag_pos( iv->ip, shrink, 
		area->left + area->width / 2, 
		area->top + area->height / 2 );

	return( iv );
}
