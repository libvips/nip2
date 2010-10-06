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

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

/*
 */
#define DEBUG

#include "ip.h"

static FloatwindowClass *parent_class = NULL;

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
	slist_map( row->sym->topchildren, 
		(SListMapFn) graph_write_row_child, buf );

	return( NULL );
}

static void *
graph_write_column( Column *col, VipsBuf *buf )
{
	static int n = 0;

	vips_buf_appendf( buf, "\tsubgraph cluster_%d {\n", n++ );
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
graphwindow_destroy( GtkObject *object )
{
	Graphwindow *graphwindow;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_GRAPHWINDOW( object ) );

	graphwindow = GRAPHWINDOW( object );

#ifdef DEBUG
	printf( "graphwindow_destroy: %p\n", graphwindow );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	IM_FREEF( g_source_remove, graphwindow->layout_timeout );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
graphwindow_class_init( GraphwindowClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = graphwindow_destroy;

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

	vips_buf_init_static( &graphwindow->dot_buf, 
		graphwindow->dot_buf_txt, MAX_STRSIZE );

	graphwindow->layout_timeout = 0;

	graphwindow->imagemodel = NULL;
	graphwindow->ip = NULL;
}

GType
graphwindow_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( GraphwindowClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) graphwindow_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Graphwindow ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) graphwindow_init,
		};

		type = g_type_register_static( TYPE_FLOATWINDOW, 
			"Graphwindow", &info, 0 );
	}

	return( type );
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
	symbol_qualified_name( ws->sym, &buf );
	iwindow_set_title( IWINDOW( graphwindow ), "%s", vips_buf_all( &buf ) );
}

static gboolean
graphwindow_layout_cb( Graphwindow *graphwindow )
{
	Workspace *ws = WORKSPACE( FLOATWINDOW( graphwindow )->model );

	graphwindow->layout_timeout = 0;

	vips_buf_rewind( &graphwindow->dot_buf );
	graph_write_dot( ws, &graphwindow->dot_buf );

#ifdef DEBUG
	printf( "graphwindow_changed_cb:\n%s\n", 
		vips_buf_all( &graphwindow->dot_buf ) );
#endif /*DEBUG*/

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

	int w, h; 

	/* Make our model.
	 */
	g_signal_connect( G_OBJECT( ws ), "changed", 
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

	/* Graph area. 
	 */
	frame = gtk_frame_new( NULL );
	gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_OUT );
	gtk_widget_show( frame );
	gtk_box_pack_start( GTK_BOX( vbox ), 
		GTK_WIDGET( frame ), TRUE, TRUE, 0 );

	/* Initial window size.
	 */
	if( MODEL( ws )->window_width == -1 ) {
		w = IM_MIN( IMAGE_WINDOW_WIDTH, 500 );
		h = IM_MIN( IMAGE_WINDOW_HEIGHT, 500 );
		gtk_window_set_default_size( GTK_WINDOW( graphwindow ), w, h );
	}
}

static void
graphwindow_link( Graphwindow *graphwindow, Workspace *ws, GtkWidget *parent )
{
	iwindow_set_build( IWINDOW( graphwindow ), 
		(iWindowBuildFn) graphwindow_build, ws, NULL, NULL );
	iwindow_set_parent( IWINDOW( graphwindow ), parent );
	floatwindow_link( FLOATWINDOW( graphwindow ), MODEL( ws ) );
	iwindow_build( IWINDOW( graphwindow ) );

	/* Initial "changed" on the model to get all views to init.
	 */
	iobject_changed( IOBJECT( ws ) );
}

Graphwindow *
graphwindow_new( Workspace *ws, GtkWidget *parent )
{
	Graphwindow *graphwindow = gtk_type_new( TYPE_GRAPHWINDOW );

	graphwindow_link( graphwindow, ws, parent );

	return( graphwindow );
}

