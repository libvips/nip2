/* display workspaces with graphviz
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

   	TODO

	Don't generate DOT in a file, build the graph ourselves. We'd need 
	some sort of graph hash thing to avoid relayouts. Or perhaps a 
	workspace hash? A hash by column?

	After layout, don't render to a file, instead draw directly to the 
	output window with cairo. Copy/paste code from the png:cairo render.
	Have some simple culling stuff to only render visible parts of the 
	graph.

	As well as "view workspace as graph", have a "view column as graph" 
	item.
 
	Don't show nodes which are not inside this workspace.

	Possibly add graph editing, or at least edge highlighting as you 
	mouse.

	Let columns be collapsed / expanded, and start the view with columns 
	collapsed if there's more than 1. This might be easier if columns were 
	separate scopes, I suppose.

	Do we should dynamic dependencies? How about (A2 = untitled."A1") ?

 */

/*
#define DEBUG
 */

#include "ip.h"

#ifdef HAVE_LIBGVC

G_DEFINE_TYPE( Graphwindow, graphwindow, TYPE_FLOATWINDOW );

static int graph_write_cluster_index = 0;

static void *
graph_write_row_child( Link *link, VipsBuf *buf )
{
	if( link->child->expr && link->child->expr->row ) {
		vips_buf_appendf( buf, "\t\t%s -> %s;\n", 
			IOBJECT( link->child )->name,
			IOBJECT( link->parent )->name ); 
	}

	return( NULL );
}

static void *
graph_write_row( Row *row, VipsBuf *buf )
{
	if( row->sym )
		slist_map( row->sym->topchildren, 
			(SListMapFn) graph_write_row_child, buf );

	return( NULL );
}

static void *
graph_write_column( Column *col, VipsBuf *buf )
{
	vips_buf_appendf( buf, "\tsubgraph cluster_%d {\n", 
		graph_write_cluster_index++ );
	vips_buf_appendf( buf, "\t\tlabel = \"%s", IOBJECT( col )->name );
	if( IOBJECT( col )->caption )
		vips_buf_appendf( buf, " - %s", IOBJECT( col )->caption );
	vips_buf_appends( buf, "\"\n" );

	vips_buf_appends( buf, "\t\tstyle=filled;\n" );
	vips_buf_appends( buf, "\t\tcolor=lightgrey;\n" );
	vips_buf_appends( buf, "\t\tnode [style=filled,color=white];\n" );

	(void) column_map( col, 
		(row_map_fn) graph_write_row, buf, NULL );
	vips_buf_appends( buf, "\t}\n" );

	return( NULL );
}

/* Generate the workspace in dot format.
 */
static void
graph_write_dot( Workspace *ws, VipsBuf *buf )
{
	graph_write_cluster_index = 0;
	vips_buf_appends( buf, "digraph G {\n" );
	workspace_map_column( ws, 
		(column_map_fn) graph_write_column, buf );
	vips_buf_appends( buf, "}\n" );
}

/* Print the workspace in dot format. Display with something like:
 * $ dot test1.dot -o test1.png -Tpng:cairo -v
 * $ eog test1.png
 */
void
graph_write( Workspace *ws )
{
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_write_dot( ws, &buf );
	printf( "%s", vips_buf_all( &buf ) );
}

static void
graphwindow_destroy( GtkWidget *widget )
{
	Graphwindow *graphwindow;

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_GRAPHWINDOW( widget ) );

	graphwindow = GRAPHWINDOW( widget );

#ifdef DEBUG
	printf( "graphwindow_destroy: %p\n", graphwindow );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	IM_FREE( graphwindow->dot );
	IM_FREEF( g_source_remove, graphwindow->layout_timeout );

	UNREF( graphwindow->imagemodel );

	IM_FREEF( agclose, graphwindow->graph );
	IM_FREEF( gvFreeContext, graphwindow->gvc );

	FREESID( graphwindow->workspace_changed_sid, 
		FLOATWINDOW( graphwindow )->model );

	GTK_WIDGET_CLASS( graphwindow_parent_class )->destroy( widget );
}

static void
graphwindow_class_init( GraphwindowClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	widget_class->destroy = graphwindow_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
graphwindow_init( Graphwindow *graphwindow )
{
#ifdef DEBUG
	printf( "graphwindow_init: %p\n", graphwindow );
#endif /*DEBUG*/

	graphwindow->dot = NULL;
	graphwindow->layout_timeout = 0;

	graphwindow->gvc = gvContext();
}

static void
graphwindow_refresh_title( Graphwindow *graphwindow )
{
	Workspace *ws = WORKSPACE( FLOATWINDOW( graphwindow )->model );

	VipsBuf buf;
	char txt[512];

#ifdef DEBUG
	printf( "graphwindow_refresh_title\n" );
#endif /*DEBUG*/

	vips_buf_init_static( &buf, txt, 512 );
	if( ws->sym )
		symbol_qualified_name( ws->sym, &buf );
	iwindow_set_title( IWINDOW( graphwindow ), "%s", vips_buf_all( &buf ) );
}

static gboolean
graphwindow_build_graph( Graphwindow *graphwindow )
{
	char tname[FILENAME_MAX];
	iOpenFile *of;

	if( !temp_name( tname, "dot" ) ||
		!(of = ifile_open_write( "%s", tname )) )
		return( FALSE );
	if( !ifile_write( of, "%s", graphwindow->dot ) ) {
		ifile_close( of );
		unlinkf( "%s", tname );
		return( FALSE );
	}
	ifile_close( of );

	if( !(of = ifile_open_read( "%s", tname )) ) {
		unlinkf( "%s", tname );
		return( FALSE );
	}

	IM_FREEF( agclose, graphwindow->graph );

	graphwindow->graph = agread( of->fp, NULL );

	ifile_close( of );
	unlinkf( "%s", tname );

	return( TRUE );
}

static gboolean
graphwindow_update_image( Graphwindow *graphwindow )
{
	char tname[FILENAME_MAX];
	iOpenFile *of;
	Imageinfo *ii;

	if( !temp_name( tname, "png" ) ||
		!(of = ifile_open_write( "%s", tname )) )
		return( FALSE );

	gvRender( graphwindow->gvc, graphwindow->graph, "png:cairo", of->fp );

	ifile_close( of );

	if( !(ii = imageinfo_new_input( main_imageinfogroup, 
		GTK_WIDGET( graphwindow ), NULL, tname )) ) {
		unlinkf( "%s", tname );
		return( FALSE );
	}

	conversion_set_image( graphwindow->imagemodel->conv, ii );

	MANAGED_UNREF( ii );

	/* We can unlink now: the png will have been converted to vips
	 * format.
	 */
	unlinkf( "%s", tname );

	return( TRUE );
}

static gboolean
graphwindow_layout( Graphwindow *graphwindow )
{
	if( !graphwindow_build_graph( graphwindow ) )
		return( FALSE );
	gvLayout( graphwindow->gvc, graphwindow->graph, "dot" );
	if( !graphwindow_update_image( graphwindow ) )
		return( FALSE );

	return( TRUE );
}

static gboolean
graphwindow_layout_cb( Graphwindow *graphwindow )
{
	Workspace *ws = WORKSPACE( FLOATWINDOW( graphwindow )->model );

	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graphwindow->layout_timeout = 0;

	graph_write_dot( ws, &buf );
	if( !graphwindow->dot ||
		strcmp( vips_buf_all( &buf ), graphwindow->dot ) != 0 ) {
		IM_FREE( graphwindow->dot );
		graphwindow->dot = im_strdup( NULL, vips_buf_all( &buf ) );

#ifdef DEBUG
		printf( "graphwindow_changed_cb:\n%s\n", graphwindow->dot );
#endif /*DEBUG*/

		if( !graphwindow_layout( graphwindow ) )
			iwindow_alert( GTK_WIDGET( graphwindow ), 
				GTK_MESSAGE_ERROR );
	}

	/* Clear the timeout.
	 */
	return( FALSE );
}

static void
graphwindow_layout_queue( Graphwindow *graphwindow )
{
	IM_FREEF( g_source_remove, graphwindow->layout_timeout );
	graphwindow->layout_timeout = g_timeout_add( 200, 
		(GSourceFunc) graphwindow_layout_cb, graphwindow );
}

/* The model has changed.
 */
static void
graphwindow_changed_cb( Workspace *ws, Graphwindow *graphwindow )
{
#ifdef DEBUG
	printf( "graphwindow_changed_cb: %p\n", graphwindow );
#endif /*DEBUG*/

	graphwindow_refresh_title( graphwindow );
	graphwindow_layout_queue( graphwindow );
}

static const char *graphwindow_menubar_ui_description =
"<ui>"
"  <menubar name='GraphwindowMenubar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='Close'/>"
"      <menuitem action='Quit'/>"
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
graphwindow_build( Graphwindow *graphwindow, GtkWidget *vbox, Workspace *ws )
{
	iWindow *iwnd = IWINDOW( graphwindow );

	GError *error;
	GtkWidget *mbar;
	GtkWidget *frame;

	/* Make our model.
	 */
	graphwindow->imagemodel = imagemodel_new( NULL );
	g_object_ref( G_OBJECT( graphwindow->imagemodel ) );
	iobject_sink( IOBJECT( graphwindow->imagemodel ) );
	graphwindow->workspace_changed_sid = g_signal_connect( 
		G_OBJECT( ws ), "changed", 
		G_CALLBACK( graphwindow_changed_cb ), graphwindow );

        /* Make main menu bar
         */
	error = NULL;
	if( !gtk_ui_manager_add_ui_from_string( iwnd->ui_manager,
			graphwindow_menubar_ui_description, -1, &error ) ) {
		g_message( "building menus failed: %s", error->message );
		g_error_free( error );
		exit( EXIT_FAILURE );
	}

	mbar = gtk_ui_manager_get_widget( iwnd->ui_manager, 
		"/GraphwindowMenubar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mbar, FALSE, FALSE, 0 );
        gtk_widget_show( mbar );

	/* This will set to NULL if we don't have infobar support.
	 */
	if( (iwnd->infobar = infobar_new()) ) 
		gtk_box_pack_start( GTK_BOX( vbox ), 
			GTK_WIDGET( iwnd->infobar ), FALSE, FALSE, 0 );

	/* Graph area. 
	 */
	frame = gtk_frame_new( NULL );
	gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_OUT );
	gtk_widget_show( frame );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( frame ), TRUE, TRUE, 0 );
	graphwindow->ip = imagepresent_new( graphwindow->imagemodel );
	gtk_container_add( GTK_CONTAINER( frame ), 
		GTK_WIDGET( graphwindow->ip ) );
	gtk_widget_show( GTK_WIDGET( graphwindow->ip ) );
}

static void
graphwindow_link( Graphwindow *graphwindow, Workspace *ws, GtkWidget *parent )
{
	iwindow_set_build( IWINDOW( graphwindow ), 
		(iWindowBuildFn) graphwindow_build, ws, NULL, NULL );
	iwindow_set_parent( IWINDOW( graphwindow ), parent );
	floatwindow_link( FLOATWINDOW( graphwindow ), MODEL( ws ) );
	iwindow_set_size_prefs( IWINDOW( graphwindow ), 
		"GRAPH_WINDOW_WIDTH", "GRAPH_WINDOW_HEIGHT" );
	iwindow_build( IWINDOW( graphwindow ) );

	/* Initial "changed" on the model to get all views to init.
	 */
	iobject_changed( IOBJECT( ws ) );
}

Graphwindow *
graphwindow_new( Workspace *ws, GtkWidget *parent )
{
	Graphwindow *graphwindow = g_object_new( TYPE_GRAPHWINDOW, NULL );

	graphwindow_link( graphwindow, ws, parent );

	return( graphwindow );
}

#endif /*HAVE_LIBGVC*/
