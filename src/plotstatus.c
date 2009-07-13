/* widgets for the status bar
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

static GtkFrameClass *parent_class = NULL;

/* The popup menu.
 */
static GtkWidget *plotstatus_menu = NULL;

static void
plotstatus_columns_destroy( Plotstatus *plotstatus )
{
	int i;

	for( i = 0; i < plotstatus->columns; i++ ) 
		DESTROY_GTK( plotstatus->label[i] );

	IM_FREE( plotstatus->label );

	plotstatus->columns = 0;
}

static void
plotstatus_destroy( GtkObject *object )
{
	Plotstatus *plotstatus;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PLOTSTATUS( object ) );

	plotstatus = PLOTSTATUS( object );

#ifdef DEBUG
	printf( "plotstatus_destroy\n" );
#endif /*DEBUG*/

	plotstatus_columns_destroy( plotstatus );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

/* Hide this plotstatus.
 */
static void
plotstatus_hide_cb( GtkWidget *menu, GtkWidget *host, Plotstatus *plotstatus )
{
	plotmodel_set_status( plotstatus->plotmodel, FALSE );
}

static void
plotstatus_class_init( PlotstatusClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	GtkWidget *pane;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = plotstatus_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */

	pane = plotstatus_menu = popup_build( _( "Status bar menu" ) );
	popup_add_but( pane, GTK_STOCK_CLOSE, 
		POPUP_FUNC( plotstatus_hide_cb ) );
}

static void
plotstatus_init( Plotstatus *plotstatus )
{
	GtkWidget *vb, *hb;
	GtkWidget *eb;

	plotstatus->plotmodel = NULL;
	plotstatus->label = NULL;
	plotstatus->columns = 0;

        gtk_frame_set_shadow_type( GTK_FRAME( plotstatus ), GTK_SHADOW_OUT );

	eb = gtk_event_box_new();
        gtk_container_add( GTK_CONTAINER( plotstatus ), eb );
        popup_attach( eb, plotstatus_menu, plotstatus );

	vb = gtk_vbox_new( FALSE, 0 );
        gtk_container_set_border_width( GTK_CONTAINER( vb ), 1 );
        gtk_container_add( GTK_CONTAINER( eb ), vb );

	plotstatus->top = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( plotstatus->top ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( vb ), plotstatus->top, TRUE, TRUE, 0 );

	hb = gtk_hbox_new( FALSE, 5 );
        gtk_box_pack_start( GTK_BOX( vb ), hb, TRUE, TRUE, 0 );

	plotstatus->pos = gtk_label_new( "" );
	set_fixed( plotstatus->pos, strlen( "(8888888,8888888)" ) );
        gtk_misc_set_alignment( GTK_MISC( plotstatus->pos ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( hb ), plotstatus->pos, FALSE, FALSE, 0 );

	plotstatus->hb = gtk_hbox_new( FALSE, 5 );
        gtk_box_pack_start( GTK_BOX( hb ), plotstatus->hb, TRUE, TRUE, 0 );

	plotstatus->mag = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( plotstatus->mag ), 0.0, 0.5 );
        gtk_box_pack_end( GTK_BOX( hb ), plotstatus->mag, FALSE, FALSE, 0 );

	gtk_widget_show_all( eb );
}

GtkType
plotstatus_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Plotstatus",
			sizeof( Plotstatus ),
			sizeof( PlotstatusClass ),
			(GtkClassInitFunc) plotstatus_class_init,
			(GtkObjectInitFunc) plotstatus_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( GTK_TYPE_FRAME, &info );
	}

	return( type );
}

/* Model has changed: rebuild everything.
 */
static void
plotstatus_refresh( Plotstatus *plotstatus )
{
	Plotmodel *plotmodel = plotstatus->plotmodel;
	Plot *plot = plotmodel->plot;
	char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

#ifdef DEBUG
	printf( "plotstatus_refresh: %p\n", plotstatus );
	printf( "  show_status = %d\n", plotmodel->show_status );
#endif /*DEBUG*/

	widget_visible( GTK_WIDGET( plotstatus ), plotmodel->show_status );

	/* If we're hidden, no need to do any more.
	 */
	if( !plotmodel->show_status )
		return;

	set_glabel( plotstatus->mag, "%s %d%%", 
		_( "Magnification" ), plotmodel->mag );

	set_gcaption( plotstatus->top, "%s", IOBJECT( plot )->caption );

	if( plotstatus->columns != plot->columns ) {
		/* Bands/fmt has changed ... rebuild band display widgets.
		 */
		int columns;
		int i;

		/* Don't display more than 8 series ... it'll make the window
		 * too large.

		 	FIXME ... not very kewl

		 */
		plotstatus_columns_destroy( plotstatus );
		columns = IM_MIN( 8, plot->columns ); 
		if( !(plotstatus->label = 
			IM_ARRAY( NULL, columns, GtkWidget * )) )
			return;
		for( i = 0; i < columns; i++ )
			plotstatus->label[i] = NULL;
		plotstatus->columns = columns;

		for( i = 0; i < columns; i++ ) {
			GtkWidget *label;

			plotstatus->label[i] = label = gtk_label_new( "" );
			gtk_misc_set_alignment( GTK_MISC( label ), 
				0.0, 0.5 );
			set_fixed( label, 8 );
			gtk_box_pack_start( GTK_BOX( plotstatus->hb ), 
				label, FALSE, FALSE, 0 );
			gtk_widget_show( label );
		}
	}
}

static void
plotstatus_changed_cb( Plotmodel *plotmodel, Plotstatus *plotstatus )
{
	plotstatus_refresh( plotstatus );
}

Plotstatus *
plotstatus_new( Plotmodel *plotmodel )
{
	Plotstatus *plotstatus = gtk_type_new( TYPE_PLOTSTATUS );

	plotstatus->plotmodel = plotmodel;
	g_signal_connect( G_OBJECT( plotmodel ), "changed", 
		G_CALLBACK( plotstatus_changed_cb ), plotstatus );

	return( plotstatus );
}

/* Find nearest x, display that y.
 */
static void
plotstatus_series_update( GtkWidget *widget, 
	Plot *plot, int column, double x, double y )
{
	double *xcolumn = plot->xcolumn[column];
	double *ycolumn = plot->ycolumn[column];
	int i;
	int best;
	gdouble best_score;

	best = 0;
	best_score = IM_ABS( x - xcolumn[0] );
	for( i = 1; i < plot->rows; i++ ) {
		double score = IM_ABS( x - xcolumn[i] );

		if( score < best_score ) {
			best_score = score;
			best = i;
		}
	}

	set_glabel( widget, "%g", ycolumn[best] );
}

void 
plotstatus_mouse( Plotstatus *plotstatus, double x, double y )
{
	Plotmodel *plotmodel = plotstatus->plotmodel;
	Plot *plot = plotmodel->plot;
	int i;

	set_glabel( plotstatus->pos, "(%05g, %05g)", x, y ); 

	g_assert( plotstatus->columns <= plot->columns );

	for( i = 0; i < plotstatus->columns; i++ ) 
		plotstatus_series_update( plotstatus->label[i], 
			plot, i, x, y );
}
