/* run the display for a plotview in a workspace 
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
#define DEBUG_GEO
#define DEBUG
 */

#include "ip.h"

#ifdef HAVE_LIBGOFFICE

G_DEFINE_TYPE( plotview, Plotview, TYPE_GRAPHICVIEW ); 

static void
plotview_destroy( GtkObject *object )
{
    	Plotview *plotview;

    	g_return_if_fail( object != NULL );
    	g_return_if_fail( IS_PLOTVIEW( object ) );

#ifdef DEBUG
    	printf( "plotview_destroy\n" );
#endif /*DEBUG*/

    	plotview = PLOTVIEW( object );

	GOG_UNREF( plotview->gplot );

    	GTK_OBJECT_CLASS( plotview_parent_class )->destroy( object );
}

static void
plotview_refresh( vObject *vobject )
{
    	Plotview *plotview = PLOTVIEW( vobject );
    	Plot *plot = PLOT( VOBJECT( plotview )->iobject );


#ifdef DEBUG
    	printf( "plotview_refresh\n" );
#endif /*DEBUG*/

	/* Can't refresh before model build.
	 */
	if( plot->rows == 0 || 
		plot->columns == 0 )
		return;

	set_gcaption( plotview->label, "%s", NN( IOBJECT( plot )->caption ) );

	GOG_UNREF( plotview->gplot );

	plotview->gplot = plot_new_gplot( plot );
	gog_object_add_by_name( GOG_OBJECT( plotview->gchart ), 
		"Plot", GOG_OBJECT( plotview->gplot ) );

	plot_style_thumbnail( plot, plotview->gchart ); 

	gtk_widget_show_all( plotview->canvas );

    	VOBJECT_CLASS( plotview_parent_class )->refresh( vobject );
}

static void
plotview_link( View *view, Model *model, View *parent )
{
	Plotview *plotview = PLOTVIEW( view );
	Rowview *rview = ROWVIEW( parent->parent );

	VIEW_CLASS( plotview_parent_class )->link( view, model, parent );

	rowview_menu_attach( rview, GTK_WIDGET( plotview->box ) );
}

static void
plotview_class_init( PlotviewClass *class )
{
    	GtkObjectClass *object_class = (GtkObjectClass *) class;
    	vObjectClass *vobject_class = (vObjectClass *) class;
    	ViewClass *view_class = (ViewClass *) class;

    	object_class->destroy = plotview_destroy;

    	vobject_class->refresh = plotview_refresh;

	view_class->link = plotview_link;
}

static void
plotview_tooltip_generate( GtkWidget *widget, VipsBuf *buf, Plotview *plotview )
{
	Plot *plot = PLOT( VOBJECT( plotview )->iobject );
	IMAGE *im;

	vips_buf_rewind( buf );
	vips_buf_appends( buf, vips_buf_all( &plot->caption_buffer ) );

	vips_buf_appendf( buf, ", %s, %s", 
		plot_f2c( plot->format ), plot_s2c( plot->style ) );

	if( (im = imageinfo_get( FALSE, plot->value.ii )) ) {
		vips_buf_appends( buf, ", " );
		vips_buf_appendi( buf, im );
	}
}

static void 
plotview_doubleclick_one_cb( GtkWidget *widget, GdkEvent *event, 
	Plotview *plotview )
{
	Heapmodel *heapmodel = HEAPMODEL( VOBJECT( plotview )->iobject );
	Row *row = heapmodel->row;

	row_select_modifier( row, event->button.state );
}

static void 
plotview_doubleclick_two_cb( GtkWidget *widget, GdkEvent *event, 
	Plotview *plotview )
{
	Plot *plot = PLOT( VOBJECT( plotview )->iobject );

	model_edit( widget, MODEL( plot ) );
}

static void
plotview_init( Plotview *plotview )
{
	GtkWidget *eb;

#ifdef DEBUG
    	printf( "plotview_init\n" );
#endif /*DEBUG*/

        eb = gtk_event_box_new();
        gtk_box_pack_start( GTK_BOX( plotview ), 
		eb, FALSE, FALSE, 0 );
	gtk_widget_show( eb );
	gtk_widget_set_name( eb, "caption_widget" );
        set_tooltip_generate( eb,
		(TooltipGenerateFn) plotview_tooltip_generate, plotview, NULL );
	doubleclick_add( eb, FALSE,
		DOUBLECLICK_FUNC( plotview_doubleclick_one_cb ), plotview,
		DOUBLECLICK_FUNC( plotview_doubleclick_two_cb ), plotview );

	plotview->box = gtk_vbox_new( FALSE, 0 );
        gtk_container_add( GTK_CONTAINER( eb ), plotview->box );
        gtk_widget_show( plotview->box );

	plotview->canvas = go_graph_widget_new( NULL );
        gtk_box_pack_start( GTK_BOX( plotview->box ), 
		plotview->canvas, FALSE, FALSE, 0 );
	plotview->gchart = go_graph_widget_get_chart( 
		GO_GRAPH_WIDGET( plotview->canvas ) );
	gtk_widget_set_size_request( GTK_WIDGET( plotview->canvas ), 
		DISPLAY_THUMBNAIL, DISPLAY_THUMBNAIL );

	plotview->gplot = NULL;

	plotview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( plotview->label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( plotview->label ), 2, 0 );
        gtk_box_pack_end( GTK_BOX( plotview->box ), 
		GTK_WIDGET( plotview->label ), FALSE, FALSE, 0 );
	gtk_widget_show( GTK_WIDGET( plotview->label ) );
}

View *
plotview_new( void )
{
    	Plotview *plotview = gtk_type_new( TYPE_PLOTVIEW );

    	return( VIEW( plotview ) );
}

#endif /*HAVE_LIBGOFFICE*/
