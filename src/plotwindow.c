/* a plotpresent / plotmodel in a floating window
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

static FloatwindowClass *parent_class = NULL;

static void
plotwindow_destroy( GtkObject *object )
{
	Plotwindow *plotwindow;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PLOTWINDOW( object ) );

	plotwindow = PLOTWINDOW( object );

#ifdef DEBUG
	printf( "plotwindow_destroy: %p\n", plotwindow );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	UNREF( plotwindow->plotmodel );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
plotwindow_class_init( PlotwindowClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = plotwindow_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
plotwindow_init( Plotwindow *plotwindow )
{
#ifdef DEBUG
	printf( "plotwindow_init: %p\n", plotwindow );
#endif /*DEBUG*/

	plotwindow->plotmodel = NULL;
}

GtkType
plotwindow_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Plotwindow",
			sizeof( Plotwindow ),
			sizeof( PlotwindowClass ),
			(GtkClassInitFunc) plotwindow_class_init,
			(GtkObjectInitFunc) plotwindow_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_FLOATWINDOW, &info );
	}

	return( type );
}

static void
plotwindow_refresh_title( Plotwindow *plotwindow )
{
	Plotmodel *plotmodel = plotwindow->plotmodel;
	Plot *plot = plotmodel->plot;
	Row *row = HEAPMODEL( plot )->row;
	Workspace *ws = row_get_workspace( row );

#ifdef DEBUG
	printf( "plotwindow_refresh_title\n" );
#endif /*DEBUG*/

	/* Can come here during ws destroy.
	 */
	if( ws ) {
		VipsBuf buf;
		char txt[512];

		vips_buf_init_static( &buf, txt, 512 );
		row_qualified_name_relative( ws->sym, row, &buf );
		iwindow_set_title( IWINDOW( plotwindow ), "%s", 
			vips_buf_all( &buf ) );
	}
}

/* The model has changed ... update our menus and titlebar.
 */
static void
plotwindow_changed_cb( Plotmodel *plotmodel, Plotwindow *plotwindow )
{
	iWindow *iwnd = IWINDOW( plotwindow );

	GtkAction *action;

	plotwindow_refresh_title( plotwindow );

	action = gtk_action_group_get_action( iwnd->action_group, 
		"Status" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		plotmodel->show_status );
}

static void
plotwindow_mouse_move_cb( Plotpresent *plotpresent, 
	double x, double y, Plotwindow *plotwindow )
{
	plotstatus_mouse( plotwindow->plotstatus, x, y );
}

static void
plotwindow_show_status_action_cb( GtkToggleAction *action, 
	Plotwindow *plotwindow )
{
	plotmodel_set_status( plotwindow->plotmodel, 
		gtk_toggle_action_get_active( action ) );
}

static void
plotwindow_saveas_done_cb( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
#ifdef HAVE_LIBGOFFICE
#ifdef HAVE_LIBGSF
	Filesel *filesel = FILESEL( iwnd );
	Plotwindow *plotwindow = (Plotwindow *) client;
	Plotpresent *plotpresent = plotwindow->plotpresent;
	GogGraph *ggraph = plotpresent->ggraph;

	char *filename; 
	char buf[FILENAME_MAX];
	GOImageFormat format;
	GsfOutput *output;
	GError *err = NULL;
	gboolean result;

	if( !(filename = filesel_get_filename( filesel )) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	expand_variables( filename, buf );
	if( !(output = gsf_output_stdio_new( buf, &err )) ) {
		error_top( _( "Unable to write." ) );
		if( err )
			error_sub( "%s", err->message );
		IM_FREEF( g_error_free, err );
		g_free( filename ); 
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	format = go_image_get_format_from_name( filename ); 

	g_free( filename ); 

	result = gog_graph_export_image( ggraph, format, output, 72, 72 );

	UNREF( output );

	nfn( sys, result ? IWINDOW_YES : IWINDOW_ERROR );
#endif /*HAVE_LIBGSF*/
#endif /*HAVE_LIBGOFFICE*/
}

static void
plotwindow_save_action_cb( GtkAction *action, Plotwindow *plotwindow )
{
	Filesel *filesel = FILESEL( filesel_new() );

	iwindow_set_title( IWINDOW( filesel ), 
		"%s", _( "Save Plot To File As" ) ); 
	filesel_set_flags( filesel, TRUE, TRUE );
	filesel_set_filetype( filesel, filesel_type_image, IMAGE_FILE_TYPE ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( plotwindow ) );
	filesel_set_done( filesel, plotwindow_saveas_done_cb, plotwindow );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static GtkToggleActionEntry plotwindow_toggle_actions[] = {
	{ "Status",
		NULL, N_( "_Status" ), NULL,
		N_( "Show status bar" ),
		G_CALLBACK( plotwindow_show_status_action_cb ), TRUE }
};

static GtkActionEntry plotwindow_actions[] = {
	{ "SaveAs", 
		GTK_STOCK_SAVE_AS, N_( "Save As Image" ), NULL,
		N_( "Save plot to file as an image" ), 
		G_CALLBACK( plotwindow_save_action_cb ) }
};

static const char *plotwindow_menubar_ui_description =
"<ui>"
"  <menubar name='PlotwindowMenubar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='SaveAs'/>"
"      <separator/>"
"      <menuitem action='Close'/>"
"      <menuitem action='Quit'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='Status'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"      <separator/>"
"      <menuitem action='Homepage'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static void
plotwindow_build( Plotwindow *plotwindow, GtkWidget *vbox, Plot *plot )
{
	iWindow *iwnd = IWINDOW( plotwindow );

	GError *error;
	GtkWidget *mbar;
	GtkWidget *frame;
	GList *focus_chain;

	int w, h; 

	/* Make our model.
	 */
	plotwindow->plotmodel = plotmodel_new( plot );
	g_object_ref( G_OBJECT( plotwindow->plotmodel ) );
	iobject_sink( IOBJECT( plotwindow->plotmodel ) );
	g_signal_connect( G_OBJECT( plotwindow->plotmodel ), "changed", 
		G_CALLBACK( plotwindow_changed_cb ), plotwindow );

        /* Make main menu bar
         */
	gtk_action_group_add_actions( iwnd->action_group, 
		plotwindow_actions, G_N_ELEMENTS( plotwindow_actions ), 
		GTK_WINDOW( plotwindow ) );
	gtk_action_group_add_toggle_actions( iwnd->action_group,
		plotwindow_toggle_actions, 
			G_N_ELEMENTS( plotwindow_toggle_actions ), 
		GTK_WINDOW( plotwindow ) );

	error = NULL;
	if( !gtk_ui_manager_add_ui_from_string( iwnd->ui_manager,
			plotwindow_menubar_ui_description, -1, &error ) ) {
		g_message( "building menus failed: %s", error->message );
		g_error_free( error );
		exit( EXIT_FAILURE );
	}

	mbar = gtk_ui_manager_get_widget( iwnd->ui_manager, 
		"/PlotwindowMenubar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mbar, FALSE, FALSE, 0 );
        gtk_widget_show( mbar );

	/* Status bar. Show/hide set on first refresh.
	 */
	plotwindow->plotstatus = plotstatus_new( plotwindow->plotmodel );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( plotwindow->plotstatus ), FALSE, FALSE, 0 );

	/* Plot area. 
	 */
	frame = gtk_frame_new( NULL );
	gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_OUT );
	gtk_widget_show( frame );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( frame ), TRUE, TRUE, 0 );

#ifdef HAVE_LIBGOFFICE
	plotwindow->plotpresent = plotpresent_new( plotwindow->plotmodel );
#endif /*HAVE_LIBGOFFICE*/
	gtk_container_add( GTK_CONTAINER( frame ), 
		GTK_WIDGET( plotwindow->plotpresent )  );
	gtk_widget_show( GTK_WIDGET( plotwindow->plotpresent ) );
	g_signal_connect( G_OBJECT( plotwindow->plotpresent ), "mouse_move", 
		G_CALLBACK( plotwindow_mouse_move_cb ), plotwindow );

	/* Initial window size.
	 */
	if( MODEL( plot )->window_width == -1 ) {
		w = IM_MIN( IMAGE_WINDOW_WIDTH, 500 );
		h = IM_MIN( IMAGE_WINDOW_HEIGHT, 500 );
		gtk_window_set_default_size( GTK_WINDOW( plotwindow ), w, h );
	}

	/* Override the focus_chain ... we want the imagedisplay first.
	 */
	focus_chain = NULL;
	focus_chain = g_list_append( focus_chain, plotwindow->plotpresent );
	gtk_container_set_focus_chain( GTK_CONTAINER( vbox ), focus_chain );
	gtk_widget_grab_focus( GTK_WIDGET( plotwindow->plotpresent ) );
}

static void
plotwindow_popdown( iWindow *iwnd, void *client,
	iWindowNotifyFn nfn, void *sys )
{
	Plotwindow *plotwindow = PLOTWINDOW( iwnd );
	Plotmodel *plotmodel = plotwindow->plotmodel;
	Plot *plot = plotmodel->plot;

	/* We have to note position/size in popdown rather than destroy, since
	 * the widgets have to all still be extant.
	 */
	plot->show_status = plotmodel->show_status;

	nfn( sys, IWINDOW_YES );
}

static void
plotwindow_link( Plotwindow *plotwindow, Plot *plot, GtkWidget *parent )
{
	iwindow_set_build( IWINDOW( plotwindow ), 
		(iWindowBuildFn) plotwindow_build, plot, NULL, NULL );
	iwindow_set_parent( IWINDOW( plotwindow ), parent );
	iwindow_set_popdown( IWINDOW( plotwindow ), plotwindow_popdown, NULL );
	floatwindow_link( FLOATWINDOW( plotwindow ), MODEL( plot ) );
	iwindow_build( IWINDOW( plotwindow ) );

	/* Initial "changed" on the model to get all views to init.
	 */
	iobject_changed( IOBJECT( plotwindow->plotmodel ) );
}

Plotwindow *
plotwindow_new( Plot *plot, GtkWidget *parent )
{
	Plotwindow *plotwindow = gtk_type_new( TYPE_PLOTWINDOW );

	plotwindow_link( plotwindow, plot, parent );

	return( plotwindow );
}
