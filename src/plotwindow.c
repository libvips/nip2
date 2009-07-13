/* a gtkplot in a floating window
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

/* We have gtkplot patched into the nip sources as a temp measure, so call 
 * directly.
 */
#define HAVE_GTK_EXTRA
#include "gtkplot.h"
#include "gtkplotcanvas.h"
#include "gtkplotcanvasplot.h"
#include "gtkplotdata.h"
#include "gtkplotbar.h"

static FloatwindowClass *parent_class = NULL;

/* All the magnification menus we have.
 */
typedef struct _PlotwindowMagmenu {
	const char *name;
	int mag;
} PlotwindowMagmenu;

static const PlotwindowMagmenu plotwindow_mags[] = {
	{ "Zoom100Mode", 100 },
	{ "Zoom200Mode", 200 },
	{ "Zoom400Mode", 400 },
	{ "Zoom800Mode", 800 }
};

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
plotwindow_realize( GtkWidget *widget )
{
	Plotwindow *plotwindow = PLOTWINDOW( widget );
	Plotmodel *plotmodel = plotwindow->plotmodel;
	Plot *plot = plotmodel->plot;

	GTK_WIDGET_CLASS( parent_class )->realize( widget );

	/* Sadly this doesn't work :-( The adjustments still haven't been set
	 * up when we come though here.
	 */
	if( CLASSMODEL( plot )->window_width != -1 ) 
		plotpresent_set_mag_position( plotwindow->plotpresent,
			plot->mag, plot->left, plot->top );
}

static void
plotwindow_class_init( PlotwindowClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = plotwindow_destroy;

        widget_class->realize = plotwindow_realize;

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
		char txt[512];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		row_qualified_name_relative( ws->sym, row, &buf );
		vips_buf_appendf( &buf, " - %d%%", plotmodel->mag );

		iwindow_set_title( IWINDOW( plotwindow ), "%s", 
			vips_buf_all( &buf ) );
	}
}

/* The model has changed ... update our menus and titlebar.
 */
static void
plotwindow_changed_cb( Plotmodel *plotmodel, Plotwindow *plotwindow )
{
	GtkAction *action;
	int i;

	plotwindow_refresh_title( plotwindow );

	action = gtk_action_group_get_action( plotwindow->action_group, 
		"Status" );
	gtk_toggle_action_set_active( GTK_TOGGLE_ACTION( action ),
		plotmodel->show_status );

	for( i = 0; i < IM_NUMBER( plotwindow_mags ); i++ )
		if( plotmodel->mag == plotwindow_mags[i].mag ) {
			action = gtk_action_group_get_action( 
				plotwindow->action_group,
				plotwindow_mags[i].name );
			gtk_toggle_action_set_active( 
				GTK_TOGGLE_ACTION( action ),
				TRUE );
			break;
		}
}

static void
plotwindow_mouse_move_cb( Plotpresent *plotpresent, 
	double x, double y, Plotwindow *plotwindow )
{
	plotstatus_mouse( plotwindow->plotstatus, x, y );
}

static void
plotwindow_zoom_in_action_cb( GtkAction *action, Plotwindow *plotwindow )
{
	Plotmodel *plotmodel = plotwindow->plotmodel;

	plotmodel_set_mag( plotmodel, plotmodel->mag + 25 );
}

static void
plotwindow_zoom_out_action_cb( GtkAction *action, Plotwindow *plotwindow )
{
	Plotmodel *plotmodel = plotwindow->plotmodel;

	plotmodel_set_mag( plotmodel, plotmodel->mag - 25 );
}

static void
plotwindow_zoom_100_action_cb( GtkAction *action, Plotwindow *plotwindow )
{
	plotmodel_set_mag( plotwindow->plotmodel, 100 );
}

static void
plotwindow_zoom_fit_action_cb( GtkAction *action, Plotwindow *plotwindow )
{
}

static void
plotwindow_show_status_action_cb( GtkToggleAction *action, 
	Plotwindow *plotwindow )
{
	plotmodel_set_status( plotwindow->plotmodel, 
		gtk_toggle_action_get_active( action ) );
}

static void
plotwindow_mag_action_cb( GtkRadioAction *action, GtkRadioAction *current, 
	Plotwindow *plotwindow )
{
	plotmodel_set_mag( plotwindow->plotmodel, 
		gtk_radio_action_get_current_value( action ) );
}

/* Our actions.
 */
static GtkActionEntry plotwindow_actions[] = {
	/* Menu items.
	 */
	{ "FileMenu", NULL, "_File" },
	{ "ViewMenu", NULL, "_View" },
	{ "ViewZoomMenu", NULL, "_Zoom" },
	{ "HelpMenu", NULL, "_Help" },

	/* Actions.
	 */
	{ "Close", 
		GTK_STOCK_CLOSE, N_( "_Close" ), NULL,
		N_( "Close" ), 
		G_CALLBACK( iwindow_kill_action_cb ) },

	{ "Guide", 
		GTK_STOCK_HELP, N_( "_Contents" ), "F1",
		N_( "Open the users guide" ), 
		G_CALLBACK( mainw_guide_action_cb ) },

	{ "About", 
		NULL, N_( "_About" ), NULL,
		N_( "About this program" ), 
		G_CALLBACK( mainw_about_action_cb ) },

	{ "ZoomIn",
		GTK_STOCK_ZOOM_IN, N_( "Zoom _In" ), "<control>plus",
		N_( "Zoom in on mouse cursor" ),
		G_CALLBACK( plotwindow_zoom_in_action_cb ) },

	{ "ZoomOut",
		GTK_STOCK_ZOOM_OUT, N_( "Zoom _Out" ), "<control>minus",
		N_( "Zoom out" ),
		G_CALLBACK( plotwindow_zoom_out_action_cb ) },

	{ "Zoom100",
		GTK_STOCK_ZOOM_100, N_( "Zoom _100%" ), "<control>equal",
		N_( "Zoom to 100%" ),
		G_CALLBACK( plotwindow_zoom_100_action_cb ) },

	{ "ZoomFit",
		GTK_STOCK_ZOOM_FIT, N_( "Zoom to _Fit" ), NULL,
		N_( "Zoom to fit plot to window" ),
		G_CALLBACK( plotwindow_zoom_fit_action_cb ) },
};

static GtkRadioActionEntry plotwindow_zoom_radio_actions[] = {
	{ "Zoom100Mode",
		NULL, N_( "100%" ), NULL, N_( "Zoom to 100%" ), 100 },
	{ "Zoom200Mode",
		NULL, N_( "200%" ), NULL, N_( "Zoom to 200%" ), 200 },
	{ "Zoom400Mode",
		NULL, N_( "400%" ), NULL, N_( "Zoom to 400%" ), 400 },
	{ "Zoom800Mode",
		NULL, N_( "800%" ), NULL, N_( "Zoom to 800%" ), 800 }
};

static GtkToggleActionEntry plotwindow_toggle_actions[] = {
	{ "Status",
		NULL, N_( "_Status" ), NULL,
		N_( "Show status bar" ),
		G_CALLBACK( plotwindow_show_status_action_cb ), TRUE }
};

static const char *plotwindow_menubar_ui_description =
"<ui>"
"  <menubar name='PlotwindowMenubar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='Close'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='Status'/>"
"      <separator/>"
"      <menuitem action='ZoomIn'/>"
"      <menuitem action='ZoomOut'/>"
"      <menuitem action='Zoom100'/>"
"      <menuitem action='ZoomFit'/>"
"      <separator/>"
"      <menuitem action='Zoom100Mode'/>"
"      <menuitem action='Zoom200Mode'/>"
"      <menuitem action='Zoom400Mode'/>"
"      <menuitem action='Zoom800Mode'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static void
plotwindow_build( Plotwindow *plotwindow, GtkWidget *vbox, Plot *plot )
{
	GtkAccelGroup *accel_group;
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
	plotwindow->action_group = gtk_action_group_new( "PlotwindowActions" );
	gtk_action_group_set_translation_domain( plotwindow->action_group, 
		GETTEXT_PACKAGE );
	gtk_action_group_add_actions( plotwindow->action_group, 
		plotwindow_actions, G_N_ELEMENTS( plotwindow_actions ), 
		GTK_WINDOW( plotwindow ) );
	gtk_action_group_add_toggle_actions( plotwindow->action_group,
		plotwindow_toggle_actions, 
			G_N_ELEMENTS( plotwindow_toggle_actions ), 
		GTK_WINDOW( plotwindow ) );
	gtk_action_group_add_radio_actions( plotwindow->action_group,
		plotwindow_zoom_radio_actions, 
			G_N_ELEMENTS( plotwindow_zoom_radio_actions ), 
		1,
		G_CALLBACK( plotwindow_mag_action_cb ),
		GTK_WINDOW( plotwindow ) );

	plotwindow->ui_manager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group( plotwindow->ui_manager, 
		plotwindow->action_group, 0 );

	accel_group = gtk_ui_manager_get_accel_group( plotwindow->ui_manager );
	gtk_window_add_accel_group( GTK_WINDOW( plotwindow ), accel_group );

	error = NULL;
	if( !gtk_ui_manager_add_ui_from_string( plotwindow->ui_manager,
			plotwindow_menubar_ui_description, -1, &error ) ) {
		g_message( "building menus failed: %s", error->message );
		g_error_free( error );
		exit( EXIT_FAILURE );
	}

	mbar = gtk_ui_manager_get_widget( plotwindow->ui_manager, 
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

	plotwindow->plotpresent = plotpresent_new( plotwindow->plotmodel );
	gtk_container_add( GTK_CONTAINER( frame ), 
		GTK_WIDGET( plotwindow->plotpresent )  );
	gtk_widget_show( GTK_WIDGET( plotwindow->plotpresent ) );
	g_signal_connect( G_OBJECT( plotwindow->plotpresent ), "mouse_move", 
		G_CALLBACK( plotwindow_mouse_move_cb ), plotwindow );

	/* Initial window size.
	 */
	if( CLASSMODEL( plot )->window_width == -1 ) {
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
	plotpresent_get_mag_position( plotwindow->plotpresent,
		&plot->mag, &plot->left, &plot->top );
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
	floatwindow_link( FLOATWINDOW( plotwindow ), CLASSMODEL( plot ) );
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
