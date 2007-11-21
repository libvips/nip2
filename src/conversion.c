/* Manage display conversion parameters.
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

/* Our signals. 
 */
enum {
	SIG_AREA_CHANGED,	/* Area of repaint image has changed */
	SIG_IMAGEINFO_CHANGED,	/* The imageinfo we hold has been replaced */
	SIG_LAST
};

static guint conversion_signals[SIG_LAST] = { 0 };

static ModelClass *parent_class = NULL;

/* What we send down the queue.
 */
typedef struct _ConversionUpdate {
	Conversion *conv;
	IMAGE *im;
	Rect area;
} ConversionUpdate;

/* Queue we use for worker threads to tell us about updates.
 */
static GAsyncQueue *conversion_update_queue = NULL;
static GSource *conversion_update_source = NULL;

/* All active conversions.
 */
static GSList *conversion_all = NULL;

static void *
conversion_imageinfo_changed( Conversion *conv )
{
#ifdef DEBUG
	g_print( "conversion_imageinfo_changed: " );
	iobject_print( IOBJECT( conv ) );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( conv ), 
		conversion_signals[SIG_IMAGEINFO_CHANGED], 0 );

	return( NULL );
}

static void
conversion_dispose( GObject *gobject )
{
	Conversion *conv;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_CONVERSION( gobject ) );

	conv = CONVERSION( gobject );

#ifdef DEBUG
	g_print( "conversion_dispose: " );
	iobject_print( IOBJECT( conv ) );
#endif /*DEBUG*/

	FREESID( conv->changed_sid, conv->ii );
	FREESID( conv->area_changed_sid, conv->ii );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
conversion_finalize( GObject *gobject )
{
	Conversion *conv;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_CONVERSION( gobject ) );

	conv = CONVERSION( gobject );

#ifdef DEBUG
	g_print( "conversion_finalize: " );
	iobject_print( IOBJECT( conv ) );
#endif /*DEBUG*/

	conversion_all = g_slist_remove( conversion_all, conv );

	IM_FREEF( im_region_free, conv->ireg );
	IM_FREEF( im_region_free, conv->reg );

	MANAGED_UNREF( conv->repaint_ii );
	MANAGED_UNREF( conv->display_ii );
	MANAGED_UNREF( conv->visual_ii );
	MANAGED_UNREF( conv->ii );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

/* Make the visualisation image ... eg. we im_histplot histograms, and we
 * log scale fourier images.
 */
static IMAGE *
conversion_make_visualise( Conversion *conv, IMAGE *in )
{
	IMAGE *out = im_open( "conversion_make_visualise", "p" );
	int tconv = !(conv && conv->enabled && !conv->type);

        /* Histogram type ... plot the histogram
         */
        if( tconv && in->Type == IM_TYPE_HISTOGRAM && 
		(in->Xsize == 1 || in->Ysize == 1) ) {
                IMAGE *t[3];

		if( in->Coding == IM_CODING_LABQ ) {
			IMAGE *t = im_open_local( out, "conv:1", "p" );

			if( !t || im_LabQ2Lab( in, t ) ) {
				im_close( out );
				return( NULL );
			}

			in = t;
		}

                if( im_open_local_array( out, t, 3, "conv-1", "p" ) ||
                        im_histnorm( in, t[0] ) ||
                        im_histplot( t[0], t[1] ) ) {
                        im_close( out );
                        return( NULL );
                }

                /* Scale to a sensible size ... aim for a height of 256
                 * elements.
                 */
                if( in->Xsize == 1 && t[1]->Xsize > 256 ) {
                        if( im_subsample( t[1], t[2], t[1]->Xsize / 256, 1 ) ) {
                                im_close( out );
                                return( NULL );
                        }
                }
                else if( in->Ysize == 1 && t[1]->Ysize > 256 ) {
                        if( im_subsample( t[1], t[2], 1, t[1]->Ysize / 256 ) ) {
                                im_close( out );
                                return( NULL );
                        }
                }
                else
                        t[2] = t[1];
  
                in = t[2];
        }

	/* IM_TYPE_FOURIER type ... pow/log scale, good for fourier 
	 * transforms.
	 */
	if( tconv && in->Type == IM_TYPE_FOURIER ) {
		IMAGE *t[2];

		if( im_open_local_array( out, t, 2, "conv-1", "p" ) ||
			im_abs( in, t[0] ) || 
			im_scaleps( t[0], t[1] ) ) {
			im_close( out );
			return( NULL );
		}

		in = t[1];
	}

	if( im_copy( in, out ) ) {
		im_close( out );
		return( NULL );
	}

	return( out );
}

static void
conversion_paint_callback( IMAGE *im, Rect *area, void *client )
{
	ConversionUpdate *update = g_new( ConversionUpdate, 1 );

	/* Can't use CONVERSION() in this thread ... the GUI thread will check 
	 * this pointer for us when it reads from the queue.
	 */
	update->conv = (Conversion *) client;
	update->im = im;
	update->area = *area;

	g_async_queue_push( conversion_update_queue, update );

	/* Don't need g_main_context_wakeup( NULL ); ... not sure it's
	 * reentrant anyway.
	 */
}

static gboolean
conversion_update_callback( gpointer data )
{
	ConversionUpdate *update = (ConversionUpdate *) data;
	Conversion *conv = update->conv;
	Rect image;

	/* Must be a valid conversion, must be for the image that conversion
	 * is still using for display.
	 */
	if( !g_slist_find( conversion_all, conv ) ||
		imageinfo_get( FALSE, conv->display_ii ) != update->im ) {
#ifdef DEBUG
		g_print( "conversion_update_callback: skipping dead update\n" );
#endif /*DEBUG*/
		return( TRUE );
	}

#ifdef DEBUG
	g_print( "conversion_update_dispatch: left = %d, top = %d, "
		"width = %d, height = %d\n",
		update->area.left, update->area.top, 
		update->area.width, update->area.height );
#endif /*DEBUG*/

	conversion_disp_to_im_rect( conv, &update->area, &image );

	/* If we're zoomed out, expand the margins to allow for rounding.
	 */
	if( update->conv->mag < 0 ) {
		image.width += 1;
		image.height += 1;
	}

	/* We need to invalid the main image too, since those regions will
	 * have black in from the failed first calc.
	 */
	im_invalidate( conv->mask );
	im_invalidate( imageinfo_get( FALSE, conv->display_ii ) );

	imageinfo_area_changed( conv->ii, &image );

	return( TRUE );
}

static gboolean
conversion_update_prepare( GSource *source, gint *timeout )
{
	*timeout = -1;

	return( g_async_queue_length( conversion_update_queue ) > 0 );
}

static gboolean
conversion_update_check( GSource *source )
{
	return( g_async_queue_length( conversion_update_queue ) > 0 );
}

static gboolean
conversion_update_dispatch( GSource *source, GSourceFunc callback, 
	gpointer user_data )
{
	ConversionUpdate *update;
	gboolean result;

	update = g_async_queue_pop( conversion_update_queue );
	result = callback( update );
	g_free( update );

	return( result );
}

static GSourceFuncs conversion_update_functions = {
	conversion_update_prepare,
	conversion_update_check,
	conversion_update_dispatch,
	NULL,
};

/* How many tiles should we ask for? A bit more than the number needed to 
 * paint the screen.
 */
static int
conversion_get_default_tiles( Conversion *conv )
{
	GdkScreen *screen = gdk_screen_get_default();
	int width = gdk_screen_get_width( screen ) / conv->tile_size;
	int height = gdk_screen_get_height( screen ) / conv->tile_size;

	return( 2 * width * height );
}

/* Resize to screen coordinates and cache it.
 */
static IMAGE *
conversion_make_display( Conversion *conv, IMAGE *in, IMAGE **mask_out )
{
	IMAGE *out = im_open( "conversion_display:1", "p" );
	IMAGE *mask = im_open_local( out, "conv:1a", "p" );

	if( !out )
		return( NULL );

	if( conv->mag < 0 ) {
		/* Ordinary image ... use im_subsample().

			FIXME ... look for pyramid TIFFs here

		 */
		IMAGE *t = im_open_local( out, "conv:1", "p" );

		/* Don't shrink by more than the image size (ie. to less than
		 * 1 pixel).
		 */
		int xshrink = IM_MIN( -conv->mag, in->Xsize );
		int yshrink = IM_MIN( -conv->mag, in->Ysize );

		if( !t || im_subsample( in, t, xshrink, yshrink ) ) {
			im_close( out );
			return( NULL );
		}

		in = t;
	}

	/* Zoom, if necessary. 
	 */
	if( conv->mag > 1 ) {
		IMAGE *t = im_open_local( out, "conv:1", "p" );

		if( !t || im_zoom( in, t, conv->mag, conv->mag ) ) {
			im_close( out );
			return( NULL );
		}

		in = t;
	}

	/* Build the queue we use for comms, if it's not there.
	 */
	if( !conversion_update_queue ) {
		conversion_update_queue = g_async_queue_new();

		conversion_update_source = g_source_new(
			&conversion_update_functions, sizeof( GSource ) );
		g_source_set_callback( conversion_update_source, 
			conversion_update_callback, NULL, NULL );
		g_source_attach( conversion_update_source, NULL );
	}

	/* Cache it.
	 */
	if( conv->synchronous ) {
		if( im_copy( in, out ) ) {
			im_close( out );
			return( NULL );
		}
	}
	else
		if( im_render_fade( in, out, mask, 
			conv->tile_size, conv->tile_size, 
				conversion_get_default_tiles( conv ),
			20, conv->fade_steps,
			conv->priority,
			conversion_paint_callback, conv ) ) {
			im_close( out );
			return( NULL );
		}

	if( mask_out )
		*mask_out = mask;

	return( out );
}

static int
conversion_make_bg( Conversion *conv, IMAGE *repaint, IMAGE *out )
{
	IMAGE *tile = im_open_local( out, "tile", "t" );
	IMAGE *t1 = im_open_local( out, "t1", "p" );
	IMAGE *t2 = im_open_local( out, "t2", "p" );
	int i;

	/* Make a 2x2 pixel image with the tile pixels in.
	 */
	im_initdesc( tile, 2, 2,
		repaint->Bands, IM_BBITS_BYTE, IM_BANDFMT_UCHAR,
		IM_CODING_NONE, IM_TYPE_MULTIBAND, 
		1.0, 1.0, 0, 0 );
	if( im_setupout( tile ) )
		return( -1 );
	for( i = 0; i < tile->Bands; i++ ) {
		((PEL *)(tile->data))[i] = 100;
		((PEL *)(tile->data))[i + tile->Bands] = 90;
		((PEL *)(tile->data))[i + tile->Bands * 2] = 90;
		((PEL *)(tile->data))[i + tile->Bands * 3] = 100;
	}

	/* Blow it up, tile it and trim it.
	 */
	if( im_zoom( tile, t1, 16, 16 ) ||
		im_replicate( t1, t2, 
			1 + repaint->Xsize / 16, 1 + repaint->Ysize / 16 ) ||
		im_extract_area( t2, out, 
			0, 0, repaint->Xsize, repaint->Ysize ) )
		return( -1 );

	return( 0 );
}

/* Track during lintrauc.
 */
typedef struct {
        double a, b;
	IMAGE *in, *out;
} LintraInfo;

/* Define what we do for each band element type. Non-complex input, uchar
 * output.
 */
#define LOOP(IN) { \
        IN *p = (IN *) in; \
        PEL *q = (PEL *) out; \
        \
        for( x = 0; x < sz; x++ ) { \
		double t; \
		\
		t = a * p[x] + b; \
		\
		if( t > 255 ) \
			t = 255; \
		else if( t < 0 ) \
			t = 0; \
		\
                q[x] = t; \
	} \
}

/* Complex input, uchar output.
 */
#define LOOPCMPLX(IN) { \
        IN *p = (IN *) in; \
        PEL *q = (PEL *) out; \
        \
        for( x = 0; x < sz; x++ ) { \
		double t; \
		\
		t = a * p[x << 1] + b; \
		\
		if( t > 255 ) \
			t = 255; \
		else if( t < 0 ) \
			t = 0; \
		\
                q[x] = t; \
	} \
}

/* Lintra a buffer, 1 set of scale/offset.
 */
static int
lintrauc_gen( PEL *in, PEL *out, int width, IMAGE *im, LintraInfo *inf )
{       
        double a = inf->a;
        double b = inf->b;
        int sz = width * im->Bands;
        int x;

        /* Lintra all input types.
         */
        switch( im->BandFmt ) {
        case IM_BANDFMT_UCHAR:          
		LOOP( unsigned char ); break;
        case IM_BANDFMT_CHAR:           
		LOOP( signed char ); break; 
        case IM_BANDFMT_USHORT:         
		LOOP( unsigned short ); break;
        case IM_BANDFMT_SHORT:          
		LOOP( signed short ); break; 
        case IM_BANDFMT_UINT:           
		LOOP( unsigned int ); break;
        case IM_BANDFMT_INT:            
		LOOP( signed int );  break;
        case IM_BANDFMT_FLOAT:          
		LOOP( float ); break; 
        case IM_BANDFMT_DOUBLE:         
		LOOP( double ); break; 
        case IM_BANDFMT_COMPLEX:        
		LOOPCMPLX( float ); break; 
        case IM_BANDFMT_DPCOMPLEX:      
		LOOPCMPLX( double ); break;

        default:
                assert( 0 );
        }

        return( 0 );
}

/* im_lintra() that writes uchar (the VIPS one writes float/double).
 */
static int
im_lintrauc( double a, IMAGE *in, double b, IMAGE *out )
{ 
        LintraInfo *inf;

        /* Check args.
         */
        if( in->Coding != IM_CODING_NONE ) {
                im_error( "im_lintrauc", _( "not uncoded" ) );
                return( -1 );
        }

        if( im_cp_desc( out, in ) )
                return( -1 );
	out->Bbits = IM_BBITS_BYTE;
	out->BandFmt = IM_BANDFMT_UCHAR;

        /* Make space for a little buffer.
         */
        if( !(inf = IM_NEW( out, LintraInfo )) )
                return( -1 );
	inf->a = a;
	inf->b = b;
	inf->in = in;
	inf->out = out;

        /* Generate!
         */
	if( im_wrapone( in, out,
		(im_wrapone_fn) lintrauc_gen, in, inf ) )
		return( -1 );

        return( 0 );
}

/* Turn any IMAGE into a 1/3 band IM_BANDFMT_UCHAR ready for gdk_rgb_*().
 */
static IMAGE *
conversion_make_repaint( Conversion *conv, IMAGE *in )
{
	IMAGE *out = im_open( "conversion_apply:1", "p" );
	IMAGE *bg = im_open_local( out, "bg", "p" );

	/* 7 is sRGB.

		FIXME ... should let other displays be used here, see
		../scraps/calibrate.[hc]

	 */
	struct im_col_display *display = im_col_displays( 7 );

	/* Do we do colorimetric type conversions? Look for
	 * interpret-type-toggle.
	 */
	int tconv = !(conv && conv->enabled && !conv->type);

	if( !out )
		return( NULL );

	/* Special case: if this is a IM_CODING_LABQ, and the display control 
	 * bar is turned off, we can go straight to RGB for speed.
	 */
	if( in->Coding == IM_CODING_LABQ && !(conv && conv->enabled) ) {
		IMAGE *t = im_open_local( out, "conv:1", "p" );
		static void *table = NULL;

		/* Make sure fast LabQ2disp tables are built.
		 */
		if( !table ) 
			table = im_LabQ2disp_build_table( NULL, display );

		if( !t || im_LabQ2disp_table( in, t, table ) ) {
			im_close( out );
			return( NULL );
		}

		in = t;
	}

	/* Get the bands right. If we have >3, drop down to 3. If we have 2,
	 * drop down to 1.
	 */
	if( in->Coding == IM_CODING_NONE ) {
		if( in->Bands == 2 ) {
			IMAGE *t = im_open_local( out, "conv:1", "p" );

			if( !t || im_extract_band( in, t, 0 ) ) {
				im_close( out );
				return( NULL );
			}

			in = t;
		}
		else if( in->Bands > 3 ) {
			IMAGE *t = im_open_local( out, "conv:1", "p" );

			if( !t ||
				im_extract_bands( in, t, 0, 3 ) ) {
				im_close( out );
				return( NULL );
			}

			in = t;
		}
	}

	/* Interpret the Type field for colorimetric images.
	 */
	if( tconv &&
		in->Bands == 3 && in->BandFmt == IM_BANDFMT_SHORT && 
		in->Type == IM_TYPE_LABS ) {
		IMAGE *t = im_open_local( out, "conv:1", "p" );

		if( !t || im_LabS2LabQ( in, t ) ) {
			im_close( out );
			return( NULL );
		}

		in = t;
	}

	if( in->Coding == IM_CODING_LABQ ) {
		IMAGE *t = im_open_local( out, "conv:1", "p" );

		if( !t || im_LabQ2Lab( in, t ) ) {
			im_close( out );
			return( NULL );
		}

		in = t;
	}

	if( in->Coding != IM_CODING_NONE ) {
		im_close( out );
		return( NULL );
	}

	/* One of the colorimetric types?
	 */
	if( tconv && in->Bands == 3 && 
		(in->Type == IM_TYPE_LCH ||
		in->Type == IM_TYPE_YXY ||
		in->Type == IM_TYPE_UCS ||
		in->Type == IM_TYPE_LAB ||
		in->Type == IM_TYPE_XYZ) ) {
		IMAGE *t[2];

		/* We need to scale/offset before we go to 8 bit to work well 
		 * with HDR.
		 */
		if( conv && conv->enabled && 
			(conv->scale != 1.0 || conv->offset != 0.0) ) {
			IMAGE *t = im_open_local( out, "conv:1", "p" );

			if( !t || im_lintra( conv->scale, in, 
				conv->offset, t ) ) {
				im_close( out );
				return( NULL );
			}

			in = t;
		}

		if( in->Type == IM_TYPE_LCH ) {
			if( im_open_local_array( out, t, 2, "conv-1", "p" ) ||
				im_clip2fmt( in, t[0], IM_BANDFMT_FLOAT ) ||
				im_LCh2Lab( t[0], t[1] ) ) {
				im_close( out );
				return( NULL );
			}

			in = t[1];
		}

		if( in->Type == IM_TYPE_YXY ) {
			if( im_open_local_array( out, t, 2, "conv-1", "p" ) ||
				im_clip2fmt( in, t[0], IM_BANDFMT_FLOAT ) ||
				im_Yxy2XYZ( t[0], t[1] ) ) {
				im_close( out );
				return( NULL );
			}

			in = t[1];
		}

		if( in->Type == IM_TYPE_UCS ) {
			if( im_open_local_array( out, t, 2, "conv-1", "p" ) ||
				im_clip2fmt( in, t[0], IM_BANDFMT_FLOAT ) ||
				im_UCS2XYZ( t[0], t[1] ) ) {
				im_close( out );
				return( NULL );
			}

			in = t[1];
		}

		if( in->Type == IM_TYPE_LAB ) {
			if( im_open_local_array( out, t, 2, "conv-1", "p" ) ||
				im_clip2fmt( in, t[0], IM_BANDFMT_FLOAT ) ||
				im_Lab2XYZ( t[0], t[1] ) ) {
				im_close( out );
				return( NULL );
			}

			in = t[1];
		}

		if( in->Type == IM_TYPE_XYZ ) {
			if( im_open_local_array( out, t, 2, "conv-1", "p" ) ||
				im_clip2fmt( in, t[0], IM_BANDFMT_FLOAT ) ||
				im_XYZ2disp( t[0], t[1], display ) ) {
				im_close( out );
				return( NULL );
			}

			in = t[1];
		}
	}
	else {
		/* Not colorimetric. We can use a special ->uchar lintra for
		 * scale/offset. Don't scale/offset fourier or histogram 
		 * images, they are presented above.
		 */
		if( conv && conv->enabled &&
			(!tconv || in->Type != IM_TYPE_FOURIER) &&
			(!tconv || in->Type != IM_TYPE_HISTOGRAM) &&
			(conv->scale != 1.0 || conv->offset != 0.0) ) {
			IMAGE *t = im_open_local( out, "conv:1", "p" );

			if( !t || im_lintrauc( conv->scale, in, 
				conv->offset, t ) ) {
				im_close( out );
				return( NULL );
			}

			in = t;
		}
	}

	/* im_msb() only works for the int formats.
	 */
	if( tconv && 
		(in->Type == IM_TYPE_RGB16 || in->Type == IM_TYPE_GREY16) &&
		im_isint( in ) ) {
		IMAGE *t[1];

                if( im_open_local_array( out, t, 1, "conv-1", "p" ) ||
			im_msb( in, t[0] ) ) {
			im_close( out );
			return( NULL );
		}

		in = t[0];
	}

	/* Clip to uchar if not there already.
	 */
	if( in->BandFmt != IM_BANDFMT_UCHAR ) {
		IMAGE *t = im_open_local( out, "conv:1", "p" );

		if( !t || im_clip( in, t ) ) {
			im_close( out );
			return( NULL );
		}

		in = t;
	}

	/* Falsecolour. Just use the green channel if we're RGB.
	 */
	if( conv && conv->enabled && conv->falsecolour ) {
		IMAGE *t1 = im_open_local( out, "conv:1", "p" );

		if( !t1 ) {
			im_close( out );
			return( NULL );
		}

		if( in->Bands == 3 ) {
			IMAGE *t2 = im_open_local( out, "conv:1", "p" );

			if( im_extract_band( in, t2, 1 ) ) {
				im_close( out );
				return( NULL );
			}

			in = t2;
		}

		if( im_falsecolour( in, t1 ) ) {
			im_close( out );
			return( NULL );
		}

		in = t1;
	}

	/* Combine with background checkerboard and mask to make final image.
	 */
	if( conv->synchronous ) {
		if( im_copy( in, out ) ) {
			im_close( out );
			return( NULL );
		}
	}
	else
		if( conversion_make_bg( conv, in, bg ) ||
			im_blend( conv->mask, in, bg, out ) ) {
			im_close( out );
			return( NULL );
		}

	return( out );
}

/* Controls in the display conversion bar, or the display image have changed.
 * Remake the repaint image.
 */
static void
conversion_rebuild_repaint( Conversion *conv )
{
	IMAGE *display_im;
	IMAGE *new_repaint_im;
	Imageinfo *new_repaint_ii;
	REGION *new_ireg;

#ifdef DEBUG
	g_print( "conversion_remake_repaint: %p\n", conv );
#endif /*DEBUG*/

	if( conv->display_ii )
		display_im = imageinfo_get( FALSE, conv->display_ii );
	else
		display_im = NULL;

	/* Keep gcc quiet about annoying possible-used-before-set warnings.
	 */
	new_repaint_ii = NULL;
	new_ireg = NULL;

	/* Make the new stuff first.
	 */
	if( display_im ) {
		if( !(new_repaint_im = 
			conversion_make_repaint( conv, display_im )) ) 
			return;
		if( !(new_repaint_ii = imageinfo_new( main_imageinfogroup, 
			NULL, new_repaint_im, NULL )) ) {
			im_close( new_repaint_im );
			return;
		}
		managed_sub_add( MANAGED( new_repaint_ii ), 
			MANAGED( conv->display_ii ) );

		if( !(new_ireg = im_region_create( new_repaint_im )) ) 
			return;
	}

	IM_FREEF( im_region_free, conv->ireg );
	MANAGED_UNREF( conv->repaint_ii );

	if( display_im ) {
		conv->repaint_ii = new_repaint_ii;
		MANAGED_REF( conv->repaint_ii );
		conv->ireg = new_ireg;
	}	
}

/* The magnification or the visual image have changed ... remake the
 * display image.
 */
static void
conversion_rebuild_display( Conversion *conv )
{
	IMAGE *visual_im;
	IMAGE *new_display_im;
	Imageinfo *new_display_ii;
	IMAGE *mask;

#ifdef DEBUG
	g_print( "conversion_remake_display: %p\n", conv );
#endif /*DEBUG*/

	if( conv->visual_ii )
		visual_im = imageinfo_get( FALSE, conv->visual_ii );
	else
		visual_im = NULL;

	/* Keep gcc quiet about annoying possible-used-before-set warnings.
	 */
	new_display_ii = NULL;
	new_display_im = NULL;

	/* Make the new stuff first.
	 */
	if( visual_im ) {
		if( !(new_display_im = 
			conversion_make_display( conv, visual_im, &mask )) ) 
			return;
		if( !(new_display_ii = imageinfo_new( main_imageinfogroup, 
			NULL, new_display_im, NULL )) ) {
			im_close( new_display_im );
			return;
		}
		managed_sub_add( MANAGED( new_display_ii ), 
			MANAGED( conv->visual_ii ) );
	}

	MANAGED_UNREF( conv->display_ii );

	if( visual_im ) {
		conv->display_ii = new_display_ii;
		conv->mask = mask;
		MANAGED_REF( conv->display_ii );

		conv->canvas.width = new_display_im->Xsize;
		conv->canvas.height = new_display_im->Ysize;
	}	

	/* Certainly need a new repaint image.
	 */
	conversion_rebuild_repaint( conv );
}

/* The underlying ii has changed. Remake the visualisation image.
 */
static void
conversion_rebuild_visual( Conversion *conv )
{
	IMAGE *im = imageinfo_get( FALSE, conv->ii );
	IMAGE *new_visual_im;
	Imageinfo *new_visual_ii;
	REGION *new_reg;

#ifdef DEBUG
	g_print( "conversion_rebuild_visual: %p\n", conv );
#endif /*DEBUG*/

	/* Keep gcc quiet about annoying possible-used-before-set warnings.
	 */
	new_visual_im = NULL;
	new_visual_ii = NULL;
	new_reg = NULL;

	/* Make new visualization image.
	 */
	if( im ) {
		if( !(new_visual_im = conversion_make_visualise( conv, im )) )
			return;
		if( !(new_visual_ii = imageinfo_new( main_imageinfogroup, 
			NULL, new_visual_im, NULL )) ) {
			im_close( new_visual_im );
			return;
		}
		managed_sub_add( MANAGED( new_visual_ii ), 
			MANAGED( conv->ii ) );

		if( !(new_reg = im_region_create( im )) ) {
			IM_FREEF( im_region_free, new_reg );
			return;
		}
	}

	/* Junk old stuff.
	 */
	IM_FREEF( im_region_free, conv->reg );
	MANAGED_UNREF( conv->visual_ii );

	/* Install new stuff.
	 */
	if( im ) {
		conv->visual_ii = new_visual_ii;
		MANAGED_REF( conv->visual_ii );
		conv->image.width = new_visual_im->Xsize;
		conv->image.height = new_visual_im->Ysize;
		conv->reg = new_reg;
	}

	/* Certainly need a new display.
	 */
	conversion_rebuild_display( conv );
}

/* Something has changed ... check it out.
 */
static void
conversion_changed( iObject *iobject )
{
	Conversion *conv = CONVERSION( iobject );

	gboolean rebuild_display = FALSE;
	gboolean rebuild_repaint = FALSE;

#ifdef DEBUG
	g_print( "conversion_changed: %p\n", conv );
#endif /*DEBUG*/

	/* Need to remake the display image if mag has changed.
	 */
	if( conv->mag != conv->display_mag ) {
		rebuild_display = TRUE;
		conv->display_mag = conv->mag;
	}

	/* Need to rebuild repaint if display control bar has changed.
	 */
	if( conv->changed ) {
		conv->changed = FALSE;
		rebuild_repaint = TRUE;
	}

	if( rebuild_display )
		conversion_rebuild_display( conv );
	else if( rebuild_repaint )
		conversion_rebuild_repaint( conv );

	IOBJECT_CLASS( parent_class )->changed( iobject );
}

static void
conversion_class_init( ConversionClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = conversion_dispose;
	gobject_class->finalize = conversion_finalize;

	iobject_class->changed = conversion_changed;

	/* Create signals.
	 */
	conversion_signals[SIG_AREA_CHANGED] = g_signal_new( "area_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ConversionClass, area_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		G_TYPE_POINTER );
	conversion_signals[SIG_IMAGEINFO_CHANGED] = g_signal_new( 
		"imageinfo_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ConversionClass, imageinfo_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
}

static void
conversion_init( Conversion *conv )
{
	static const Rect emptyrect = { 0, 0, 0, 0 };

#ifdef DEBUG
	g_print( "conversion_init: " );
	iobject_print( IOBJECT( conv ) );
#endif /*DEBUG*/

	conv->ii = NULL;
	conv->changed_sid = 0;
	conv->area_changed_sid = 0;
	conv->reg = NULL;
	conv->synchronous = FALSE;
	conv->priority = 0;
	conv->visual_ii = NULL;
	conv->display_ii = NULL;
	conv->display_mag = 99999999;
	conv->repaint_ii = NULL;
	conv->ireg = NULL;

	/* Default tile size ... OK for image display, too big for
	 * thumbnails.
	 */
	conv->tile_size = 128;

	/* Default to the preference ... iimageview sets this to less to stop
	 * the tile fade animation on thumbnails.
	 */
	conv->fade_steps = DISPLAY_FADE_STEPS;

	conv->underlay = emptyrect;
	conv->image = emptyrect;
	conv->canvas = emptyrect;
	conv->visible = emptyrect;
	conv->mag = 1;

	conv->changed = FALSE;
	conv->enabled = FALSE;
	conv->scale = 1.0;
	conv->offset = 0.0;
	conv->falsecolour = FALSE;
	conv->type = TRUE;

	conversion_all = g_slist_prepend( conversion_all, conv );
}

GType
conversion_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ConversionClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) conversion_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Conversion ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) conversion_init,
		};

		type = g_type_register_static( TYPE_MODEL, 
			"Conversion", &info, 0 );
	}

	return( type );
}

static void
conversion_link( Conversion *conv, Imageinfo *ii )
{
	iobject_set( IOBJECT( conv ), "display_conversion", NULL );
	conversion_set_image( conv, ii );
}

Conversion *
conversion_new( Imageinfo *ii )
{
	Conversion *conv;

	conv = CONVERSION( g_object_new( TYPE_CONVERSION, NULL ) );
	conversion_link( conv, ii );

	return( conv );
}

/* Imageinfo has changed signal. The ii is the same, but the image it
 * represents may have changed from "p" to "r" or whatever. Assume size/etc.
 * stay the same.
 */
static void
conversion_ii_changed_cb( Imageinfo *ii, Conversion *conv )
{
	conversion_rebuild_visual( conv );
	iobject_changed( IOBJECT( conv ) );
}

static void
conversion_ii_area_changed_cb( Imageinfo *imageinfo, 
	Rect *dirty, Conversion *conv )
{
	Rect repaint;

	conversion_im_to_disp_rect( conv, dirty, &repaint );
	g_signal_emit( G_OBJECT( conv ), 
		conversion_signals[SIG_AREA_CHANGED], 0, &repaint );
}

/* Install a new image. 
 */
void 
conversion_set_image( Conversion *conv, Imageinfo *ii )
{
	/* Pointer compare is safe, since we hold a ref to conv->ii and it
	 * can't have been freed. So we can't have a new ii at the same
	 * address as the old ii.
	 */
	if( conv->ii != ii ) {
		IMAGE *im;

		if( ii )
			im = imageinfo_get( FALSE, ii );
		else
			im = NULL;

		/* Junk old stuff.
		 */
		FREESID( conv->changed_sid, conv->ii );
		FREESID( conv->area_changed_sid, conv->ii );
		MANAGED_UNREF( conv->ii );
		conv->image.width = -1;
		conv->image.height = -1;

		/* Install new stuff.
		 */
		if( ii ) {
			conv->ii = ii;
			MANAGED_REF( conv->ii );
			conv->changed_sid = g_signal_connect( 
				G_OBJECT( ii ), "changed",
				G_CALLBACK( conversion_ii_changed_cb ), 
				conv );
			conv->area_changed_sid = g_signal_connect( 
				G_OBJECT( ii ), "area_changed", 
				G_CALLBACK( conversion_ii_area_changed_cb ), 
				conv );
		}
		if( im ) {
			conv->underlay.width = im->Xsize;
			conv->underlay.height = im->Ysize;
		}

		/* Make new visualization image.
		 */
		conversion_rebuild_visual( conv );

		/* Tell everyone about our new ii.
		 */
		conversion_imageinfo_changed( conv );
	}

	iobject_changed( IOBJECT( conv ) );
}

double
conversion_dmag( int mag )
{
	assert( mag != 0 );

	if( mag > 0 )
		return( mag );
	else
		return( 1.0 / (-mag) );
}

/* Zoom in and zoom out scale factors ... a little tricky with our funny -ve
 * representation for subsample.
 */
int 
conversion_double( int mag )
{
	assert( mag != -1 );

	if( mag == -3 )
		return( -2 );
	else if( mag == -2 )
		return( 1 );
	else if( mag > 0 )
		return( mag * 2 );
	else
		return( mag / 2 );
}

int 
conversion_halve( int mag )
{
	assert( mag != -1 );

	if( mag == 1 )
		return( -2 );
	else if( mag > 1 )
		return( mag / 2 );
	else
		return( mag * 2 );
}

/* Convert display to image coordinates and back.
 */
void 
conversion_disp_to_im( Conversion *conv, int dx, int dy, int *ix, int *iy )
{
	double fmag = conversion_dmag( conv->mag );

	*ix = (int) (dx / fmag);
	*iy = (int) (dy / fmag);
}

void 
conversion_im_to_disp( Conversion *conv, int ix, int iy, int *dx, int *dy )
{
	double fmag = conversion_dmag( conv->mag );

	*dx = (int) (ix * fmag);
	*dy = (int) (iy * fmag);
}

/* Same for rects.
 */
void
conversion_disp_to_im_rect( Conversion *conv, Rect *dr, Rect *ir )
{
	double fmag = conversion_dmag( conv->mag );

	int brx, bry;
	Rect out;

	out.left = floor( dr->left / fmag );
	out.top = floor( dr->top / fmag );
	brx = ceil( IM_RECT_RIGHT( dr ) / fmag );
	bry = ceil( IM_RECT_BOTTOM( dr ) / fmag );
	out.width = brx - out.left;
	out.height = bry - out.top;

	*ir = out;
}

void
conversion_im_to_disp_rect( Conversion *conv, Rect *ir, Rect *dr )
{
	double fmag = conversion_dmag( conv->mag );

	int brx, bry;
	Rect out;

	out.left = floor( ir->left * fmag );
	out.top = floor( ir->top * fmag );
	brx = ceil( IM_RECT_RIGHT( ir ) * fmag );
	bry = ceil( IM_RECT_BOTTOM( ir ) * fmag );
	out.width = brx - out.left;
	out.height = bry - out.top;

	*dr = out;
}

void
conversion_set_mag( Conversion *conv, int mag )
{
	int x, y;

	/* Mag 0 means scale image to fit window. Use visible hint in
	 * conversion to pick the mag.
	 */
	if( mag == 0 ) {
		float xfac;
		float yfac;
		float fac;

		/* Need to have image and visible set.
		 */
		if( conv->visible.width <= 0 || 
			conv->image.width <= 0 || !conv->ii || !conv->ii->im )
			return;

		xfac = (float) conv->visible.width / conv->image.width;
		yfac = (float) conv->visible.height / conv->image.height;
		fac = IM_MIN( xfac, yfac );

		if( fac >= 1 )
			mag = (int) fac;
		else
			/* 0.999 means we don't round up on an exact fit.

			 	FIXME ... yuk

			 */
			mag = -((int) (0.99999999 + 1.0/fac));

#ifdef DEBUG
		g_print( "conversion_set_mag: shrink to fit:\n" );
		g_print( " visible %dx%d, image %dx%d\n",
			conv->visible.width, conv->visible.height,
			conv->image.width, conv->image.height );
		g_print( " picked mag of %d\n", mag );
#endif /*DEBUG*/
	}

	/* Check for this-mag-will-cause-integer-overflow. Should flag an 
	 * error, but we just bail out instead.
	 */
	if( mag > 0 &&
		((double) conv->image.width * mag > (double) INT_MAX / 2 ||
		(double) conv->image.height * mag > (double) INT_MAX / 2) ) 
		return;

	/* Will this mag result in width/height of <1? If it will, pick a
	 * mag that most nearly gives us width/height 1.
	 */
	conv->mag = mag;
	conversion_im_to_disp( conv, 
		conv->image.width, conv->image.height, &x, &y );
	if( x <= 0  || y <= 0 ) {
		conv->mag = IM_MAX( -conv->image.width, -conv->image.height );
		if( conv->mag == -1 )
			conv->mag = 1;
	}

	if( conv->mag != conv->display_mag )
		iobject_changed( IOBJECT( conv ) );
}

void
conversion_set_synchronous( Conversion *conv, gboolean synchronous )
{
	if( conv->synchronous != synchronous ) {
		conv->synchronous = synchronous;
		if( conv->ii )
			iobject_changed( IOBJECT( conv->ii ) );
	}
}

void
conversion_set_params( Conversion *conv, gboolean enabled,
	double scale, double offset, gboolean falsecolour, gboolean type )
{
	gboolean changed = FALSE;

	if( conv->enabled != enabled )
		changed = TRUE;
	if( enabled )
		if( conv->scale != scale ||
			conv->offset != offset ||
			conv->falsecolour != falsecolour ||
			conv->type != type )
		changed = TRUE;

	if( changed ) {
		conv->enabled = enabled;
		conv->scale = scale;
		conv->offset = offset;
		conv->falsecolour = falsecolour;
		conv->type = type;
		conv->changed = TRUE;
		iobject_changed( IOBJECT( conv ) );
	}
}

void
conversion_header_dialog( Conversion *conv, 
	const char *name, GtkWidget *parent )
{
	GtkWidget *imageheader;

	imageheader = imageheader_new( conv );
	iwindow_set_title( IWINDOW( imageheader ), 
		_( "Header for \"%s\"" ), name );
	idialog_set_callbacks( IDIALOG( imageheader ), NULL, NULL, NULL, NULL );
	idialog_add_ok( IDIALOG( imageheader ), iwindow_true_cb, _( "OK" ) );
	iwindow_set_parent( IWINDOW( imageheader ), parent );
	iwindow_build( IWINDOW( imageheader ) );

	gtk_widget_show( imageheader );
}
