/* an input plot 
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

static ClassmodelClass *parent_class = NULL;

static void
plot_free_columns( Plot *plot )
{
	int i;

	for( i = 0; i < plot->columns; i++ ) {
		IM_FREE( plot->xcolumn[i] );
		IM_FREE( plot->ycolumn[i] );
	}
	IM_FREE( plot->xcolumn );
	IM_FREE( plot->ycolumn );
	plot->columns = 0;
	plot->rows = 0;
}

static void
plot_finalize( GObject *gobject )
{
	Plot *plot;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_PLOT( gobject ) );

	plot = PLOT( gobject );

#ifdef DEBUG
	printf( "plot_finalize\n" );
#endif /*DEBUG*/

	/* My instance finalize stuff.
	 */
	image_value_destroy( &plot->value );
	plot_free_columns( plot );
	vips_buf_destroy( &plot->caption_buffer );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

char *
plot_f2c( PlotFormat format )
{
	switch( format ) {
	case PLOT_FORMAT_YYYY: return( _( "YYYY" ) );
	case PLOT_FORMAT_XYYY: return( _( "XYYY" ) );
	case PLOT_FORMAT_XYXY: return( _( "XYXY" ) );

	default:
		g_assert( 0 );
		
		/* Keep gcc happy.
		 */
		return( 0 );
	}
}

char *
plot_s2c( PlotStyle style )
{
	switch( style ) {
	case PLOT_STYLE_POINT:	return( _( "Point" ) );
	case PLOT_STYLE_LINE:	return( _( "Line" ) );
	case PLOT_STYLE_SPLINE:	return( _( "Spline" ) );
	case PLOT_STYLE_BAR:	return( _( "Bar" ) );

	default:
		g_assert( 0 );
		
		/* Keep gcc happy.
		 */
		return( 0 );
	}
}

static const char *
plot_generate_caption( iObject *iobject )
{
	Plot *plot = PLOT( iobject );
	VipsBuf *buf = &plot->caption_buffer;

	vips_buf_rewind( buf );
	image_value_caption( &plot->value, buf );
	vips_buf_appendf( buf, ", %d series, %d points", 
			plot->columns, plot->rows );
	vips_buf_appendf( buf, ", xrange [%g, %g]", plot->xmin, plot->xmax );
	vips_buf_appendf( buf, ", yrange [%g, %g]", plot->ymin, plot->ymax );

	return( vips_buf_all( buf ) );
}

/* Unpack all data formats to XYXYXY.
 *
 * 	FIXME ... could save mem by reusing columns of Xes in YYYY and XYYY
 * 	cases
 */
static gboolean
plot_unpack( Plot *plot, DOUBLEMASK *mask )
{
	int rows, columns;
	int r, c;
	double xmin, xmax;
	double ymin, ymax;

	rows = mask->ysize;
	switch( plot->format ) {
	case PLOT_FORMAT_YYYY:
		columns = mask->xsize;
		break;

	case PLOT_FORMAT_XYYY:
		if( mask->xsize < 2 ) {
			error_top( _( "Bad value." ) );
			error_sub( _( "More than one column "
				"needed or XY plots" ) );
			return( FALSE );
		}
		columns = mask->xsize - 1;
		break;

	case PLOT_FORMAT_XYXY:
		if( (mask->xsize & 1) != 0 ) {
			error_top( _( "Bad value." ) );
			error_sub( _( "Even number of columns only for "
				"XY format plots" ) );
			return( FALSE );
		}
		columns = mask->xsize / 2;
		break;

	default:
		g_assert( 0 );
	}

	if( plot->columns != columns || plot->rows != rows ) {
		plot_free_columns( plot );

		plot->xcolumn = IM_ARRAY( NULL, columns, double * );
		plot->ycolumn = IM_ARRAY( NULL, columns, double * );

		if( !plot->xcolumn || !plot->ycolumn ) {
			plot_free_columns( plot );
			return( FALSE );
		}
		plot->columns = columns;
		plot->rows = rows;

		for( c = 0; c < columns; c++ ) {
			plot->xcolumn[c] = NULL;
			plot->ycolumn[c] = NULL;
		}

		for( c = 0; c < columns; c++ ) {
			plot->xcolumn[c] = IM_ARRAY( NULL, rows, double );
			plot->ycolumn[c] = IM_ARRAY( NULL, rows, double );
			if( !plot->xcolumn[c] || !plot->ycolumn[c] ) {
				plot_free_columns( plot );
				return( FALSE );
			}
		}
	}

	switch( plot->format ) {
	case PLOT_FORMAT_YYYY:
		for( c = 0; c < columns; c++ )
			for( r = 0; r < rows; r++ ) {
				plot->xcolumn[c][r] = r;
				plot->ycolumn[c][r] = 
					mask->coeff[c + r * mask->xsize];
			}
		break;

	case PLOT_FORMAT_XYYY:
		for( c = 0; c < columns; c++ )
			for( r = 0; r < rows; r++ ) {
				plot->xcolumn[c][r] = 
					mask->coeff[r * mask->xsize];
				plot->ycolumn[c][r] = 
					mask->coeff[c + 1 + r * mask->xsize];
			}
		break;

	case PLOT_FORMAT_XYXY:
		for( c = 0; c < columns; c++ )
			for( r = 0; r < rows; r++ ) {
				plot->xcolumn[c][r] = 
					mask->coeff[c * 2 + r * mask->xsize];
				plot->ycolumn[c][r] = 
					mask->coeff[c * 2 + 1 + 
						r * mask->xsize];
			}
		break;

	default:
		g_assert( 0 );
	}

	xmin = plot->xcolumn[0][0];
	xmax = plot->xcolumn[0][0];
	ymin = plot->ycolumn[0][0];
	ymax = plot->ycolumn[0][0];

	for( c = 0; c < columns; c++ )
		for( r = 0; r < rows; r++ ) {
			if( plot->xcolumn[c][r] > xmax )
				xmax = plot->xcolumn[c][r];
			if( plot->xcolumn[c][r] < xmin )
				xmin = plot->xcolumn[c][r];
			if( plot->ycolumn[c][r] > ymax )
				ymax = plot->ycolumn[c][r];
			if( plot->ycolumn[c][r] < ymin )
				ymin = plot->ycolumn[c][r];
		}

	if( plot->xmin == PLOT_RANGE_UNSET )
		plot->xmin = xmin;
	if( plot->xmax == PLOT_RANGE_UNSET )
		plot->xmax = xmax;
	if( plot->ymin == PLOT_RANGE_UNSET )
		plot->ymin = ymin;
	if( plot->ymax == PLOT_RANGE_UNSET )
		plot->ymax = ymax;

	return( TRUE );
}

#ifdef HAVE_LIBGOFFICE
static View *
plot_view_new( Model *model, View *parent )
{
	return( plotview_new() );
	return( NULL );
}
#endif /*HAVE_LIBGOFFICE*/

static void
plot_edit( GtkWidget *parent, Model *model )
{
#ifdef HAVE_LIBGOFFICE
        Plot *plot = PLOT( model );
	Plotwindow *plotwindow;

	plotwindow = plotwindow_new( plot, parent );

	gtk_widget_show( GTK_WIDGET( plotwindow ) );
#endif /*HAVE_LIBGOFFICE*/
}

static xmlNode *
plot_save( Model *model, xmlNode *xnode )
{
        Plot *plot = PLOT( model );
	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( !set_prop( xthis, "plot_left", "%d", plot->left ) ||
		!set_prop( xthis, "plot_top", "%d", plot->top ) ||
		!set_prop( xthis, "plot_mag", "%d", plot->mag ) ||
		!set_sprop( xthis, "show_status", 
			bool_to_char( plot->show_status ) ) )
		return( NULL );

	return( xthis );
}

static gboolean
plot_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
        Plot *plot = PLOT( model );

	g_assert( IS_RHS( parent ) );

	(void) get_iprop( xnode, "plot_left", &plot->left );
	(void) get_iprop( xnode, "plot_top", &plot->top );
	(void) get_iprop( xnode, "plot_mag", &plot->mag );
	(void) get_bprop( xnode, "show_status", &plot->show_status );

	return( MODEL_CLASS( parent_class )->load( model, 
		state, parent, xnode ) );
}

/* Members of plot we automate.
 */
static ClassmodelMember plot_options[] = {
	{ CLASSMODEL_MEMBER_ENUM, NULL, PLOT_FORMAT_LAST - 1,
		"format", "format", N_( "Format" ),
		G_STRUCT_OFFSET( Plot, format ) },
	{ CLASSMODEL_MEMBER_ENUM, NULL, PLOT_STYLE_LAST - 1,
		"style", "style", N_( "Style" ),
		G_STRUCT_OFFSET( Plot, style ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		"xmin", "xmin", N_( "Xmin" ),
		G_STRUCT_OFFSET( Plot, xmin ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		"xmax", "xmax", N_( "Xmax" ),
		G_STRUCT_OFFSET( Plot, xmax ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		"ymin", "ymin", N_( "Ymin" ),
		G_STRUCT_OFFSET( Plot, ymin ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		"ymax", "ymax", N_( "Ymax" ),
		G_STRUCT_OFFSET( Plot, ymax ) }
};

static ClassmodelMember plot_members[] = {
	{ CLASSMODEL_MEMBER_OPTIONS, &plot_options, IM_NUMBER( plot_options ), 
		MEMBER_OPTIONS, NULL, N_( "Options" ),
		0 },
	{ CLASSMODEL_MEMBER_IMAGE, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Plot, value ) }
};

/* Come here after we've read in new values from the heap.
 */
static gboolean
plot_class_get( Classmodel *classmodel, PElement *root )
{
	Plot *plot = PLOT( classmodel );
	ImageValue *value = &plot->value;
	IMAGE *im = imageinfo_get( FALSE, value->ii );
	Imageinfo *ii2;
	IMAGE *t;
	DOUBLEMASK *mask;
	int (*fn)();

	/* nx1 or 1xm images only ... use Bands for columns.
	 */
	if( im->Xsize != 1 && im->Ysize != 1 ) {
		error_top( _( "Bad value." ) );
		error_sub( _( "1xn or nx1 images only for Plot" ) );
		return( FALSE );
	}

	/* Don't ref this and it'll be removed on the next GC.
	 */
	if( !(ii2 = imageinfo_new_temp( main_imageinfogroup, 
		reduce_context->heap, NULL, "p" )) ) 
		return( FALSE );
	t = imageinfo_get( FALSE, ii2 );

	/* Rotate so that our mask will be in the correct orientation.
	 */
	if( im->Ysize == 1 )
		fn = im_rot90;
	else
		fn = im_copy;
	if( fn( im, t ) ) {
		error_top( _( "Bad value." ) );
		error_sub( _( "Unable to prepare image." ) );
		error_vips();

		return( FALSE );
	}

	/* Unpack the image to a dmask, then unpack the dmask into a set of XY
	 * columns.
	 *
	 * 	FIXME ... yuk!
	 */
	if( !(mask = im_vips2mask( t, "plot_class_get" )) ) {
		error_top( _( "Bad value." ) );
		error_sub( _( "1xn or nx1 images only" ) );
		error_vips();

		return( FALSE );
	}
	if( !plot_unpack( plot, mask ) ) {
		im_free_dmask( mask );
		return( FALSE );
	}
	im_free_dmask( mask );

	return( TRUE );
}

static void
plot_reset( Classmodel *classmodel )
{
	Plot *plot = PLOT( classmodel );

	image_value_destroy( &plot->value );
	plot->format = PLOT_FORMAT_YYYY;
	plot->style = PLOT_STYLE_LINE;
	plot->xmin = PLOT_RANGE_UNSET;
	plot->xmax = PLOT_RANGE_UNSET;
	plot->ymin = PLOT_RANGE_UNSET;
	plot->ymax = PLOT_RANGE_UNSET;
}

static gboolean
plot_graphic_save( Classmodel *classmodel, 
	GtkWidget *parent, const char *filename )
{
	Plot *plot = PLOT( classmodel );
	ImageValue *value = &plot->value;
	char buf[MAX_STRSIZE];

	strcpy( buf, filename );
	filesel_add_mode( buf );

	if( value->ii )
		if( !imageinfo_write( value->ii, buf ) )
			return( FALSE );

	mainw_recent_add( &mainw_recent_image, filename );

	return( TRUE );
}

static void
plot_class_init( PlotClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->finalize = plot_finalize;

	iobject_class->generate_caption = plot_generate_caption;

#ifdef HAVE_LIBGOFFICE
	model_class->view_new = plot_view_new;
#endif /*HAVE_LIBGOFFICE*/
	model_class->edit = plot_edit;
	model_class->save = plot_save;
	model_class->load = plot_load;

	classmodel_class->class_get = plot_class_get;
	classmodel_class->members = plot_members;
	classmodel_class->n_members = IM_NUMBER( plot_members );
	classmodel_class->reset = plot_reset;

	classmodel_class->graphic_save = plot_graphic_save;
	classmodel_class->filetype = filesel_type_image;
	classmodel_class->filetype_pref = "IMAGE_FILE_TYPE";

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
plot_init( Plot *plot )
{
#ifdef DEBUG
	printf( "plot_init\n" );
#endif /*DEBUG*/

	image_value_init( &plot->value, CLASSMODEL( plot ) );

	plot->xcolumn = NULL;
	plot->ycolumn = NULL;
	plot->rows = 0;
	plot->columns = 0;

	plot->show_status = FALSE;
	plot->mag = 100;
	plot->left = 0;
	plot->top = 0;

	vips_buf_init_dynamic( &plot->caption_buffer, MAX_LINELENGTH );

	iobject_set( IOBJECT( plot ), CLASS_PLOT, NULL );

	plot_reset( CLASSMODEL( plot ) );
}

GtkType
plot_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( PlotClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) plot_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Plot ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) plot_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Plot", &info, 0 );
	}

	return( type );
}

#ifdef HAVE_LIBGOFFICE

/* Make a GOColor from an RGB triple. Different versions of goffice have
 * different ways of doing this :(
 */
#ifdef GO_COLOR_FROM_RGB
  #define RGB( R, G, B ) GO_COLOR_FROM_RGB( R, G, B )
#else
  #define RGB( R, G, B ) RGB_TO_RGBA( RGB_TO_UINT( R, G, B ), 0xff )
#endif

/* Choose line colours with this. RGB first, then mostly random. We can't use
 * goffice's default colours because we really want the first three to be: red,
 * green, blue.
 */

static GOColor default_colour[] = {
	RGB( 255, 0, 0 ),
	RGB( 0, 255, 0 ),
	RGB( 0, 0, 255 ),
	RGB( 100, 0, 102 ),
	RGB( 17, 0, 102 ),
	RGB( 0, 0, 180 ),
	RGB( 0, 53, 255 ),
	RGB( 0, 104, 234 ),
	RGB( 0, 150, 188 ),
	RGB( 0, 205, 170 ),
	RGB( 0, 255, 139 ),
	RGB( 0, 255, 55 ),
	RGB( 40, 255, 40 ),
	RGB( 106, 255, 74 ),
	RGB( 155, 255, 48 ),
	RGB( 209, 255, 21 ),
	RGB( 239, 255, 7 ),
	RGB( 255, 176, 0 ),
	RGB( 255, 110, 0 ),
	RGB( 255, 50, 0 ),
	RGB( 196, 0, 0 )
};

/* Build a GogPlot from a Plot.
 */
GogPlot *
plot_new_gplot( Plot *plot )
{
	GogPlot *gplot;
	int i;

	if( plot->style == PLOT_STYLE_BAR )
		gplot = gog_plot_new_by_name( "GogHistogramPlot" );
	else
		gplot = gog_plot_new_by_name( "GogXYPlot" );

	switch( plot->style ) {
	case PLOT_STYLE_POINT:
		g_object_set( gplot, "default-style-has-lines", FALSE, NULL );
		break;

	case PLOT_STYLE_LINE:
		g_object_set( gplot, "default-style-has-markers", FALSE, NULL );
		break;

	case PLOT_STYLE_SPLINE:
		g_object_set( gplot, "default-style-has-markers", FALSE, NULL );
		g_object_set( gplot, "use-splines", TRUE, NULL );
		break;

	case PLOT_STYLE_BAR:
		break;

	default:
		g_assert( FALSE );
	}

	for( i = 0; i < plot->columns; i++ ) {
		GogSeries *series;
		GOData *data;
		GError *error;

                series = gog_plot_new_series( gplot );
		data = go_data_vector_val_new( plot->xcolumn[i], plot->rows, 
			NULL );
		gog_series_set_dim( series, 0, data, &error );
		data = go_data_vector_val_new( plot->ycolumn[i], plot->rows, 
			NULL );
		gog_series_set_dim( series, 1, data, &error );

		if( i < IM_NUMBER( default_colour ) ) {
			GOStyle *style;

			style = go_styled_object_get_style( 
				GO_STYLED_OBJECT( series ) );

			style->line.color = default_colour[i];
			style->line.auto_color = FALSE;

			go_marker_set_fill_color( style->marker.mark,
				default_colour[i] );
			style->marker.auto_fill_color = FALSE;

			/* Could match fill, but black everywhere looks nicer.
			 */
			go_marker_set_outline_color( style->marker.mark,
				RGB( 0, 0, 0 ) );
			style->marker.auto_outline_color = FALSE;

			gog_object_request_update( GOG_OBJECT( series ) );
		}
	}

	return( gplot );
}

#endif /*HAVE_LIBGOFFICE*/
