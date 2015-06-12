/* run the display for a slider in a workspace 
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

G_DEFINE_TYPE( Sliderview, sliderview, TYPE_GRAPHICVIEW ); 

static void 
sliderview_refresh( vObject *vobject )
{
	Sliderview *sliderview = SLIDERVIEW( vobject );
	Slider *slider = SLIDER( VOBJECT( sliderview )->iobject );
	Tslider *tslider = sliderview->tslider;

        const double range = slider->to - slider->from;
        const double lrange = log10( range );
	const char *caption = IOBJECT( slider )->caption;

#ifdef DEBUG
	printf( "sliderview_refresh\n" );
#endif /*DEBUG*/

	/* Compatibility ... we used to not have a caption. Don't display
	 * anything if there's o caption.
	 */
	if( caption ) {
		if( strcmp( caption, "" ) != 0 )
			set_glabel( sliderview->label, _( "%s:" ), 
				caption );
		else
			set_glabel( sliderview->label, "%s", "" );
	}

	tslider->from = slider->from;
	tslider->to = slider->to;
	tslider->svalue = slider->value;
	tslider->value = slider->value;

	tslider->digits = IM_MAX( 0, ceil( 2 - lrange ) );

	if( CALC_RECOMP_SLIDER )
		gtk_range_set_update_policy( GTK_RANGE( tslider->slider ), 
			GTK_UPDATE_CONTINUOUS );
	else
		gtk_range_set_update_policy( GTK_RANGE( tslider->slider ),
			GTK_UPDATE_DISCONTINUOUS );

#ifdef DEBUG
	gtk_range_set_update_policy( GTK_RANGE( tslider->slider ),
		GTK_UPDATE_DISCONTINUOUS );
#endif /*DEBUG*/

	tslider_changed( tslider );

	VOBJECT_CLASS( sliderview_parent_class )->refresh( vobject );
}

static void *
sliderview_scan( View *view )
{
	Sliderview *sliderview = SLIDERVIEW( view );
	Slider *slider = SLIDER( VOBJECT( sliderview )->iobject );
	Classmodel *classmodel = CLASSMODEL( slider );
	Expr *expr = HEAPMODEL( classmodel )->row->expr;

	double value;

	if( !get_geditable_double( sliderview->tslider->entry, &value ) ) {
		expr_error_set( expr );
		return( view );
	}

	if( slider->value != value ) {
		slider->value = value;
		classmodel_update( classmodel );
	}

	return( VIEW_CLASS( sliderview_parent_class )->scan( view ) );
}

static void
sliderview_link( View *view, Model *model, View *parent )
{
	Sliderview *sliderview = SLIDERVIEW( view );

	VIEW_CLASS( sliderview_parent_class )->link( view, model, parent );

	if( GRAPHICVIEW( view )->sview )
		gtk_size_group_add_widget( GRAPHICVIEW( view )->sview->group,   
			sliderview->label );
}

static void
sliderview_class_init( SliderviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = sliderview_refresh;

	view_class->scan = sliderview_scan;
	view_class->link = sliderview_link;
}

/* Drag on slider.
 */
static void
sliderview_change_cb( Tslider *tslider, Sliderview *sliderview )
{
	Slider *slider = SLIDER( VOBJECT( sliderview )->iobject );

#ifdef DEBUG
	printf( "sliderview_change_cb\n" );
#endif /*DEBUG*/

	if( slider->value != tslider->svalue ) {
		slider->value = tslider->svalue;

		classmodel_update( CLASSMODEL( slider ) );
		symbol_recalculate_all();
	}
}

static void
sliderview_init( Sliderview *sliderview )
{
	GtkWidget *hbox;

	hbox = gtk_hbox_new( FALSE, 12 );
        gtk_box_pack_start( GTK_BOX( sliderview ), hbox, TRUE, FALSE, 0 );

        sliderview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( sliderview->label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( sliderview->label ), 2, 1 );
	gtk_box_pack_start( GTK_BOX( hbox ), sliderview->label, 
		FALSE, FALSE, 0 );

	sliderview->tslider = tslider_new();
	tslider_set_conversions( sliderview->tslider, NULL, NULL );
        gtk_box_pack_start( GTK_BOX( hbox ), 
		GTK_WIDGET( sliderview->tslider ), TRUE, TRUE, 6 );

        g_signal_connect_object( sliderview->tslider, 
		"text_changed",
                G_CALLBACK( view_changed_cb ), 
		GTK_OBJECT( sliderview ) );
        g_signal_connect_object( sliderview->tslider, 
		"activate",
                G_CALLBACK( view_activate_cb ), 
		G_OBJECT( sliderview ) );
        g_signal_connect( sliderview->tslider, 
		"slider_changed", 
		G_CALLBACK( sliderview_change_cb ), sliderview );

        gtk_widget_show_all( GTK_WIDGET( sliderview ) );
}

View *
sliderview_new( void )
{
	Sliderview *sliderview = gtk_type_new( TYPE_SLIDERVIEW );

	return( VIEW( sliderview ) );
}
