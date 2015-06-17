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

G_DEFINE_TYPE( Statusview, statusview, GTK_TYPE_FRAME ); 

/* The popup menu.
 */
static GtkWidget *statusview_menu = NULL;

/* Sub. fn. of below. Junk the widgets inside a band display.
 */
static void *
statusviewband_destroy_sub( StatusviewBand *svb )
{
	DESTROY_GTK( svb->val );
	IM_FREE( svb );

	return( NULL );
}

/* Junk the widgets inside a band display.
 */
static void
statusviewband_destroy( Statusview *sv )
{	
	slist_map( sv->bands, 
		(SListMapFn) statusviewband_destroy_sub, NULL );
	IM_FREEF( g_slist_free, sv->bands );
}

static void
statusview_destroy( GtkWidget *widget )
{
	Statusview *sv;

	g_return_if_fail( widget != NULL );
	g_return_if_fail( IS_STATUSVIEW( widget ) );

	sv = STATUSVIEW( widget );

#ifdef DEBUG
	printf( "statusview_destroy\n" );
#endif /*DEBUG*/

	statusviewband_destroy( sv );

	GTK_WIDGET_CLASS( statusview_parent_class )->destroy( widget );
}

/* Hide this statusview.
 */
static void
statusview_hide_cb( GtkWidget *menu, GtkWidget *host, Statusview *sv )
{
	sv->imagemodel->show_status = FALSE;
	iobject_changed( IOBJECT( sv->imagemodel ) );
}

static void
statusview_class_init( StatusviewClass *class )
{
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	GtkWidget *pane;

	widget_class->destroy = statusview_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */

	pane = statusview_menu = popup_build( _( "Status bar menu" ) );
	popup_add_but( pane, "close", POPUP_FUNC( statusview_hide_cb ) );
}

static void
statusview_init( Statusview *sv )
{
	GtkWidget *vb, *hb;
	GtkWidget *eb;

	sv->imagemodel = NULL;
	sv->bands = NULL;
	sv->fmt = -1;
	sv->nb = -1;

        gtk_frame_set_shadow_type( GTK_FRAME( sv ), GTK_SHADOW_OUT );

	eb = gtk_event_box_new();
        gtk_container_add( GTK_CONTAINER( sv ), eb );
        popup_attach( eb, statusview_menu, sv );

	vb = gtk_box_new( GTK_ORIENTATION_VERTICAL, 0 );
        gtk_container_set_border_width( GTK_CONTAINER( vb ), 1 );
        gtk_container_add( GTK_CONTAINER( eb ), vb );

	sv->top = gtk_label_new( "" );
        gtk_box_pack_start( GTK_BOX( vb ), sv->top, TRUE, TRUE, 0 );

	hb = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 5 );
        gtk_box_pack_start( GTK_BOX( vb ), hb, TRUE, TRUE, 0 );

	sv->pos = gtk_label_new( "" );
	set_fixed( sv->pos, strlen( "(888888,888888)" ) );
        gtk_box_pack_start( GTK_BOX( hb ), sv->pos, FALSE, FALSE, 0 );

	sv->hb = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 5 );
        gtk_box_pack_start( GTK_BOX( hb ), sv->hb, TRUE, TRUE, 0 );

	sv->mag = gtk_label_new( "" );
        gtk_box_pack_end( GTK_BOX( hb ), sv->mag, FALSE, FALSE, 0 );

	gtk_widget_show_all( eb );
}

/* Our model has changed ... update.
 */
static void
statusview_changed_cb( Imagemodel *imagemodel, Statusview *sv )
{
	static char *sample[] = {
		/* Sample text for each BandFmt. Used to try to get
		 * the spacing right.
		 */
		"888",				/* uchar */
		"-888",				/* char */
		"88888",			/* ushort */
		"-88888",			/* short */
		"888888888",			/* int */
		"-888888888",			/* uint */
		"888888888",			/* float */
		"(88888888,888888888)",		/* complex */
		"88888888888",			/* double */
		"(8888888888,888888888)"	/* dpcomplex */
	};

	Conversion *conv = imagemodel->conv;
	iImage *iimage = imagemodel->iimage;
	IMAGE *im = imageinfo_get( FALSE, iimage->value.ii );
	double size = (double) im->Ysize * IM_IMAGE_SIZEOF_LINE( im );
	unsigned int nb;
	int fmt;
	char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

#ifdef DEBUG
	printf( "statusview_changed_cb: %p\n", imagemodel );
#endif /*DEBUG*/

	widget_visible( GTK_WIDGET( sv ), imagemodel->show_status );

	/* If we're hidden, no need to do any more.
	 */
	if( !imagemodel->show_status )
		return;

	if( conv->mag > 0 )
		set_glabel( sv->mag, "%s %d:1", 
			_( "Magnification" ), conv->mag );
	else
		set_glabel( sv->mag, "%s 1:%d", 
			_( "Magnification" ), -conv->mag );

	vips_buf_appendf( &buf, "%s, ", 
		NN( IOBJECT( iimage )->caption ) );
	vips_buf_append_size( &buf, size );
	vips_buf_appendf( &buf, ", %.3gx%.3g p/mm", im->Xres, im->Yres );
	set_gcaption( sv->top, "%s", vips_buf_all( &buf ) );

	if( im->Coding == IM_CODING_LABQ ||
		im->Coding == IM_CODING_RAD ) {
		nb = 3;
		fmt = 6;
	}
	else {
		nb = im->Bands;
		fmt = im->BandFmt;
	}

	if( sv->nb != nb || sv->fmt != fmt ) {
		/* Bands/fmt has changed ... rebuild band display widgets.
		 */
		unsigned int i;
		int width;

		statusviewband_destroy( sv );
		sv->fmt = fmt;
		sv->nb = nb;
		if( sv->fmt >= 0 && sv->fmt < IM_NUMBER( sample ) )
			width = strlen( sample[sv->fmt] );
		else
			width = 10;

		/* Don't display more than 8 bands ... it'll make the window
		 * too large.

		 	FIXME ... now very kewl

		 */
		for( i = 0; i < IM_MIN( 8, nb ); i++ ) {
			StatusviewBand *band = INEW( NULL, StatusviewBand );

			band->sv = sv;
			band->bandno = i;
			band->val = gtk_label_new( "" );
			set_fixed( band->val, width );
			gtk_box_pack_start( GTK_BOX( sv->hb ), 
				band->val, FALSE, FALSE, 0 );
			gtk_widget_show( band->val );

			sv->bands = g_slist_append( sv->bands, band );
		}
	}
}

static void
statusview_link( Statusview *sv, Imagemodel *imagemodel )
{
	sv->imagemodel = imagemodel;
	g_signal_connect( G_OBJECT( sv->imagemodel ), 
		"changed", G_CALLBACK( statusview_changed_cb ), sv );
}

Statusview *
statusview_new( Imagemodel *imagemodel )
{
	Statusview *sv = g_object_new( TYPE_STATUSVIEW, NULL );

	statusview_link( sv, imagemodel );

	return( sv );
}

/* Turn a IM_CODING_LABQ 4-band image into three floats.
 */
static void
statusview_mouse_LABPACK( Statusview *sv, int x, int y )
{
	Imagemodel *imagemodel = sv->imagemodel;
	Conversion *conv = imagemodel->conv;
	GSList *bands = sv->bands;

	/* Three widgets we update.
	 */
	StatusviewBand *b1;
	StatusviewBand *b2;
	StatusviewBand *b3;

	unsigned char *e = 
		(unsigned char *) get_element( conv->reg, x, y, 0 );

	unsigned int iL = (e[0] << 2) | (e[3] >> 6);
	float L = 100.0 * iL / 1023.0;
	signed int ia = ((signed char) e[1] << 3) | ((e[3] >> 3) & 0x7);
	float a = 0.125 * ia;
	signed int ib = ((signed char) e[2] << 3) | (e[3] & 0x7);
	float b = 0.125 * ib;

	if( g_slist_length( sv->bands ) == 3 ) {
		b1 = (StatusviewBand *) bands->data;
		b2 = (StatusviewBand *) bands->next->data;
		b3 = (StatusviewBand *) bands->next->next->data;

		set_glabel( b1->val, "%g", L );
		set_glabel( b2->val, "%g", a );
		set_glabel( b3->val, "%g", b );
	}
}

/* Turn a IM_CODING_RAD 4-band image into three floats.
 */
static void
statusview_mouse_RAD( Statusview *sv, int x, int y )
{
	Imagemodel *imagemodel = sv->imagemodel;
	Conversion *conv = imagemodel->conv;
	GSList *bands = sv->bands;

	/* Three widgets we update.
	 */
	StatusviewBand *b1;
	StatusviewBand *b2;
	StatusviewBand *b3;

	unsigned char *e = 
		(unsigned char *) get_element( conv->reg, x, y, 0 );

	double f = ldexp( 1.0, e[3] - (128 + 8) );
	float r = (e[0] + 0.5) * f;
	float g = (e[1] + 0.5) * f;
	float b = (e[2] + 0.5) * f;

	if( g_slist_length( sv->bands ) == 3 ) {
		b1 = (StatusviewBand *) bands->data;
		b2 = (StatusviewBand *) bands->next->data;
		b3 = (StatusviewBand *) bands->next->next->data;

		set_glabel( b1->val, "%g", r );
		set_glabel( b2->val, "%g", g );
		set_glabel( b3->val, "%g", b );
	}
}

/* Sub-fn of below. Remake a band in the bar.
 */
static void *
statusview_mouse_band( StatusviewBand *svb, void *e )
{
	Imagemodel *imagemodel = svb->sv->imagemodel;
	Conversion *conv = imagemodel->conv;
	REGION *reg = conv->reg;
	IMAGE *im = reg->im;

	/* Generate string for contents of band element.
	 */
	if( im->Coding == IM_CODING_NONE )
		switch( im->BandFmt ) {
		case IM_BANDFMT_UCHAR:
			set_glabel( svb->val, "%d", 
				((unsigned char *)e)[svb->bandno] );
			break;
			
		case IM_BANDFMT_CHAR:
			set_glabel( svb->val, "%d", 
				((char *)e)[svb->bandno] );
			break;
			
		case IM_BANDFMT_USHORT:
			set_glabel( svb->val, "%d", 
				((unsigned short *)e)[svb->bandno] );
			break;
			
		case IM_BANDFMT_SHORT:
			set_glabel( svb->val, "%d", 
				((short *)e)[svb->bandno] );
			break;
			
		case IM_BANDFMT_UINT:
			set_glabel( svb->val, "%d", 
				((unsigned int *)e)[svb->bandno] );
			break;
			
		case IM_BANDFMT_INT:
			set_glabel( svb->val, "%d", 
				((int *)e)[svb->bandno] );
			break;
			
		case IM_BANDFMT_FLOAT:
			set_glabel( svb->val, "%g", 
				((float *)e)[svb->bandno] );
			break;
			
		case IM_BANDFMT_COMPLEX:
			set_glabel( svb->val, "(%g,%g)", 
				((float *)e)[svb->bandno << 1], 
				((float *)e)[(svb->bandno << 1) + 1] );
			break;
			
		case IM_BANDFMT_DOUBLE:
			set_glabel( svb->val, "%g", 
				((double *)e)[svb->bandno] );
			break;
			
		case IM_BANDFMT_DPCOMPLEX:
			set_glabel( svb->val, "(%g,%g)", 
				((double *)e)[svb->bandno << 1], 
				((double *)e)[(svb->bandno << 1) + 1] );
			break;

		default:
			set_glabel( svb->val, "???" );
			break;
		}
	else
		set_glabel( svb->val, "???" );

	return( NULL );
}

void 
statusview_mouse( Statusview *sv, int x, int y )
{
	Imagemodel *imagemodel = sv->imagemodel;
	Conversion *conv = imagemodel->conv;
	IMAGE *im = imageinfo_get( FALSE, conv->ii );
	REGION *reg = conv->reg;
	double dx, dy;

	x = IM_CLIP( 0, x, conv->underlay.width - 1 );
	y = IM_CLIP( 0, y, conv->underlay.height - 1 );

	/* Calculate x/y pos we display.
	 */
	dx = x;
	dy = y;

	if( imagemodel->rulers_offset ) {
		dx -= im->Xoffset;
		dy -= im->Yoffset;
	}

	if( imagemodel->rulers_mm ) {
		dx /= im->Xres;
		dy /= im->Yres;
	}

	set_glabel( sv->pos, "(%5g, %5g)", dx, dy ); 

	/* Update value list.
	 */
	if( reg ) {
		if( reg->im->Coding == IM_CODING_LABQ )
			statusview_mouse_LABPACK( sv, x, y );
		else if( reg->im->Coding == IM_CODING_RAD )
			statusview_mouse_RAD( sv, x, y );
		else
			slist_map( sv->bands, 
				(SListMapFn) statusview_mouse_band, 
				get_element( reg, x, y, 0 ) );
	}
}
