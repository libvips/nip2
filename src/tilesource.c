/*
#define DEBUG_VERBOSE
#define DEBUG
 */

#include "vipsdisp.h"

/* Use this threadpool to do background loads of images.
 */
static GThreadPool *tile_source_background_load_pool = NULL;

G_DEFINE_TYPE( TileSource, tile_source, G_TYPE_OBJECT );

enum {
        /* Properties.
         */
        PROP_MODE = 1,
        PROP_SCALE,
        PROP_OFFSET,
        PROP_PAGE,
        PROP_FALSECOLOUR,
        PROP_LOG,
        PROP_ACTIVE,
        PROP_LOADED,

        /* Signals. 
         */
        SIG_PREEVAL,
        SIG_EVAL,
        SIG_POSTEVAL,
        SIG_CHANGED,            
        SIG_TILES_CHANGED,            
        SIG_AREA_CHANGED,            
        SIG_PAGE_CHANGED,            

        SIG_LAST
};

static guint tile_source_signals[SIG_LAST] = { 0 };

static void
tile_source_dispose( GObject *object )
{
        TileSource *tile_source = TILE_SOURCE( object );

#ifdef DEBUG
        printf( "tile_source_dispose:\n" ); 
#endif /*DEBUG*/

	VIPS_FREEF( g_source_remove, tile_source->page_flip_id );

        VIPS_UNREF( tile_source->source );
        VIPS_UNREF( tile_source->image );
        VIPS_UNREF( tile_source->image_region );
        VIPS_UNREF( tile_source->display );
        VIPS_UNREF( tile_source->mask );
        VIPS_UNREF( tile_source->rgb );
        VIPS_UNREF( tile_source->rgb_region );
        VIPS_UNREF( tile_source->mask_region );

        VIPS_FREE( tile_source->delay );

        G_OBJECT_CLASS( tile_source_parent_class )->dispose( object );
}

static void
tile_source_changed( TileSource *tile_source )
{
        g_signal_emit( tile_source, 
                tile_source_signals[SIG_CHANGED], 0 );
}

static void
tile_source_tiles_changed( TileSource *tile_source )
{
        g_signal_emit( tile_source, 
                tile_source_signals[SIG_TILES_CHANGED], 0 );
}

static void
tile_source_area_changed( TileSource *tile_source, VipsRect *dirty, int z )
{
        g_signal_emit( tile_source, 
                tile_source_signals[SIG_AREA_CHANGED], 0, dirty, z );
}

static void
tile_source_page_changed( TileSource *tile_source )
{
        g_signal_emit( tile_source, 
                tile_source_signals[SIG_PAGE_CHANGED], 0 );
}

typedef struct _TileSourceUpdate {
        TileSource *tile_source;
        VipsImage *image;
        VipsRect rect;
        int z;
} TileSourceUpdate;

/* Open a specified level. Take page (if relevant) from the tile_source.
 */
static VipsImage *
tile_source_open( TileSource *tile_source, int level )
{
        /* In toilet-roll and pages-as-bands modes, we open all pages
         * together.
         */
        gboolean all_pages = tile_source->type == TILE_SOURCE_TYPE_TOILET_ROLL;
        int n = all_pages ? -1 : 1;
        int page = all_pages ? 0 : tile_source->page;

        VipsImage *image;

        /* Only for tiles_sources which have something you can open.
         */
        g_assert( tile_source->source );

        if( vips_isprefix( "openslide", tile_source->loader ) ) {
                /* These only have a "level" dimension.
                 */
                image = vips_image_new_from_source( tile_source->source, 
                        "", 
                        "level", level,
                        NULL );
        }
        else if( vips_isprefix( "tiff", tile_source->loader ) ) {
                /* We support three modes: subifd pyramids, page-based
                 * pyramids, and simple multi-page TIFFs (no pyramid).
                 */
                if( tile_source->subifd_pyramid ) 
                        /* subifd == -1 means the main image. subifd 0 picks
                         * the first subifd.
                         */
                        image = vips_image_new_from_source( tile_source->source,
                                "", 
                                "page", page,
                                "subifd", level - 1,
                                "n", n,
                                NULL );
                else if( tile_source->page_pyramid )
                        /* No "n" here since pages are mag levels.
                         */
                        image = vips_image_new_from_source( tile_source->source,
                                "", 
                                "page", level,
                                NULL );
                else
                        /* Pages are regular pages.
                         */
                        image = vips_image_new_from_source( tile_source->source,
                                "", 
                                "page", page,
                                "n", n,
                                NULL );
        }
        else if( vips_isprefix( "jp2k", tile_source->loader ) ) {
                /* These only have a "page" param.
                 */
                image = vips_image_new_from_source( tile_source->source,
                        "", 
                        "page", level,
                        NULL );
        }
        else if( vips_isprefix( "pdf", tile_source->loader ) ||
                vips_isprefix( "webp", tile_source->loader ) ||
                vips_isprefix( "gif", tile_source->loader ) ) {
                /* Support page and n.
                 */
                image = vips_image_new_from_source( tile_source->source,
                        "", 
                        "page", level,
                        "n", n,
                        NULL );
        }
        else 
                /* Don't support any page spec.
                 */
                image = vips_image_new_from_source( tile_source->source, 
                        "", 
                        NULL );

        return( image );
}

/* Run by the main GUI thread when a notify comes in from libvips that a tile 
 * we requested is now available.
 */
static gboolean
tile_source_render_notify_idle( void *user_data )
{
        TileSourceUpdate *update = (TileSourceUpdate *) user_data;
        TileSource *tile_source = update->tile_source;

        /* Only bother fetching the updated tile if it's from our current 
         * pipeline.
         */
        if( update->image == tile_source->display ) 
                tile_source_area_changed( tile_source, 
                        &update->rect, update->z );

        /* The update that's just for this one event needs freeing.
         */
        g_free( update );

        return( FALSE );
}

/* Come here from the vips_sink_screen() background thread when a tile has been
 * calculated. This is a bbackground thread, so we add an idle callback 
 * which will be run by the main thread when it next hits the mainloop.
 */
static void
tile_source_render_notify( VipsImage *image, VipsRect *rect, void *client )
{
        TileSourceUpdate *update = (TileSourceUpdate *) client;

        /* We're passed an update made by tile_source_display_image() to track
         * just this image. We need one dedicated to this single event.
         */
        TileSourceUpdate *new_update = g_new( TileSourceUpdate, 1 );

        *new_update = *update;
        new_update->rect = *rect;
        new_update->z = update->z;

        g_idle_add( tile_source_render_notify_idle, new_update );
}

/* Build the first half of the render pipeline. This ends in the sink_screen
 * which will issue any repaints.
 */
static VipsImage *
tile_source_display_image( TileSource *tile_source, VipsImage **mask_out )
{
        VipsImage *image;
        VipsImage *x;
        VipsImage *mask;
        int n_bands;
        TileSourceUpdate *update;

        g_assert( mask_out ); 

        if( tile_source->level_count ) {
                /* There's a pyramid ... compute the size of image we need,
                 * then find the layer which is one larger.
                 */
                int required_width = 
                        tile_source->display_width >> tile_source->current_z;

                int i;
                int level;

                for( i = 0; i < tile_source->level_count; i++ ) 
                        if( tile_source->level_width[i] < required_width )
                                break;
                level = VIPS_CLIP( 0, i - 1, tile_source->level_count - 1 );

#ifdef DEBUG
                printf( "tile_source_display_image: loading level %d\n", 
                        level ); 
#endif /*DEBUG*/

                if( !(image = tile_source_open( tile_source, level )) )
                        return( NULL );
        }
        else if( tile_source->type == TILE_SOURCE_TYPE_MULTIPAGE ) {
#ifdef DEBUG
                printf( "tile_source_display_image: loading page %d\n", 
                        tile_source->page ); 
#endif /*DEBUG*/

                if( !(image = tile_source_open( tile_source, 
                        tile_source->page )) )
                        return( NULL );
        }
        else {
                image = tile_source->image;
                g_object_ref( image ); 
        }

        /* In multipage display mode, crop out the page we want. 
         *
         * We need to crop using the page size on image, since it might have 
         * been shrunk by shrink-on-load above ^^
         */
        if( tile_source->type == TILE_SOURCE_TYPE_TOILET_ROLL &&
                (tile_source->mode == TILE_SOURCE_MODE_MULTIPAGE ||
                 tile_source->mode == TILE_SOURCE_MODE_ANIMATED) ) {
                int page_width = image->Xsize;
                int page_height = vips_image_get_page_height( image );

                VipsImage *x;

                if( vips_crop( image, &x, 
                        0, tile_source->page * page_height, 
                        page_width, page_height, NULL ) ) {
                        VIPS_UNREF( image );
                        return( NULL );
                }
                VIPS_UNREF( image );
                image = x;
        }

        /* In pages-as-bands mode, crop out all pages and join band-wise. 
         * 
         * We need to crop using the page size on image, since it might 
         * have been shrunk by shrink-on-load above ^^
         */
        if( tile_source->type == TILE_SOURCE_TYPE_TOILET_ROLL &&
                tile_source->mode == TILE_SOURCE_MODE_PAGES_AS_BANDS ) {
                int page_width = image->Xsize;
                int page_height = vips_image_get_page_height( image );

                VipsObject *context = VIPS_OBJECT( vips_image_new() );
                VipsImage **t = (VipsImage **) 
                        vips_object_local_array( context, 
                                tile_source->n_pages );

                int page;
                VipsImage *x;

                for( page = 0; page < tile_source->n_pages; page++ ) 
                        if( vips_crop( image, &t[page], 
                                0, page * page_height, 
                                page_width, page_height, 
                                NULL ) ) {
                                VIPS_UNREF( context );
                                VIPS_UNREF( image );
                                return( NULL );
                        }
                if( vips_bandjoin( t, &x, tile_source->n_pages, NULL ) ) {
                        VIPS_UNREF( context );
                        VIPS_UNREF( image );
                        return( NULL );
                }

                x->Type = VIPS_INTERPRETATION_MULTIBAND;

                VIPS_UNREF( image );
                VIPS_UNREF( context );
                image = x;
        }

        /* Histogram type ... plot the histogram. 
         */
        if( image->Type == VIPS_INTERPRETATION_HISTOGRAM &&
                (image->Xsize == 1 || image->Ysize == 1) ) {
                VipsImage *context = vips_image_new();
                VipsImage **t = (VipsImage **) 
                        vips_object_local_array( VIPS_OBJECT( context ), 7 );

                /* So image will be unreffed when we unref context.
                 */
                t[0] = image;
                x = t[0];

                if( x->Coding == VIPS_CODING_LABQ ) {
                        if( vips_LabQ2Lab( x, &t[1], NULL ) ) {
                                VIPS_UNREF( context );
                                return( NULL );
                        }
                        x = t[1];
                }

                if( x->Coding == VIPS_CODING_RAD ) {
                        if( vips_rad2float( x, &t[2], NULL ) ) {
                                VIPS_UNREF( context );
                                return( NULL );
                        }
                        x = t[2];
                }

                if( vips_hist_norm( x, &t[3], NULL ) ||
                        vips_hist_plot( t[3], &t[4], NULL ) ) {
                        VIPS_UNREF( context );
                        return( NULL );
                }
                x = t[4];

                /* Scale to a sensible size ... aim for a height of 256
                 * elements.
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
                 */

                image = x;
                g_object_ref( image ); 
                VIPS_UNREF( context );
        }

        if( tile_source->current_z > 0 ) {
                /* We may have already zoomed out a bit because we've loaded
                 * some layer other than the base one. Calculate the
                 * subsample as (current_width / required_width).
                 */
                int subsample = image->Xsize / 
                        (tile_source->display_width >> tile_source->current_z);

                if( vips_subsample( image, &x, subsample, subsample, NULL ) ) {
                        VIPS_UNREF( image );
                        return( NULL ); 
                }
                VIPS_UNREF( image );
                image = x;
        }

        if( vips_colourspace( image, &x, VIPS_INTERPRETATION_sRGB, NULL ) ) {
                VIPS_UNREF( image );
                return( NULL ); 
        }
        VIPS_UNREF( image );
        image = x;

        /* Remove any extra bands to leave just RGB or RGBA.
         */
        n_bands = vips_image_hasalpha( image ) ? 4 : 3;
        if( image->Bands > n_bands ) {
                if( vips_extract_band( image, &x, 0, "n", n_bands, NULL ) ) {
                        VIPS_UNREF( image );
                        return( NULL ); 
                }
                VIPS_UNREF( image );
                image = x;
        }

	/* A slow operation, handy for checking rendering order.
	 *
	if( vips_gaussblur( image, &x, 100, NULL ) ) {
		VIPS_UNREF( image );
		return( NULL ); 
	}
	VIPS_UNREF( image );
	image = x;
	 *
	 */

        /* Need something to track the z at which we made this sink_screen.
         */
        update = VIPS_NEW( image, TileSourceUpdate );
        update->tile_source = tile_source;
        update->z = tile_source->current_z;

        x = vips_image_new();
        mask = vips_image_new();
        if( vips_sink_screen( image, x, mask, 
                TILE_SIZE, TILE_SIZE, MAX_TILES, 0, 
                tile_source_render_notify, update ) ) {
                VIPS_UNREF( x );
                VIPS_UNREF( mask );
                VIPS_UNREF( image );
                return( NULL );
        }
        VIPS_UNREF( image );
        image = x;

        update->image = image;

        *mask_out = mask;

        return( image );
}

static VipsImage *
tile_source_image_log( VipsImage *image )
{
        static const double power = 0.25;
        const double scale = 255.0 / log10( 1.0 + pow( 255.0, power ) );

        VipsImage *context = vips_image_new();
        VipsImage **t = (VipsImage **) 
                vips_object_local_array( VIPS_OBJECT( context ), 7 );

        VipsImage *x;

        if( vips_pow_const1( image, &t[0], power, NULL ) ||
                vips_linear1( t[0], &t[1], 1.0, 1.0, NULL ) ||
                vips_log10( t[1], &t[2], NULL ) ||
                /* Add 0.5 to get round to nearest.
                 */
                vips_linear1( t[2], &x, scale, 0.5, NULL ) ) {
                g_object_unref( context );
                return( NULL ); 
        }
        VIPS_UNREF( context );
        image = x;

        return( image );
}

/* Build the second half of the image pipeline.
 */
static VipsImage *
tile_source_rgb_image( TileSource *tile_source, VipsImage *in ) 
{
        VipsImage *image;
        VipsImage *x;

        image = in;
        g_object_ref( image ); 

        if( tile_source->active &&
                (tile_source->scale != 1.0 ||
                 tile_source->offset != 0.0 ||
                 tile_source->falsecolour ||
                 tile_source->log ||
                 image->Type == VIPS_INTERPRETATION_FOURIER) ) {
                VipsImage *alpha;

                /* We don't want these to touch alpha (if any) ... remove and 
                 * reattach at the end.
                 */
                alpha = NULL;
                if( vips_image_hasalpha( image ) ) { 
                        if( vips_extract_band( image, &alpha, 3, NULL ) ) {
                                VIPS_UNREF( image );
                                return( NULL ); 
                        }
                        if( vips_extract_band( image, &x, 0, 
                                "n", 3, NULL ) ) {
                                VIPS_UNREF( image );
                                VIPS_UNREF( alpha );
                                return( NULL ); 
                        }
                        VIPS_UNREF( image );
                        image = x;
                }

                if( tile_source->log ||
                        image->Type == VIPS_INTERPRETATION_FOURIER ) { 
                        if( !(x = tile_source_image_log( image )) ) {
                                VIPS_UNREF( image );
                                VIPS_UNREF( alpha );
                                return( NULL ); 
                        }
                        VIPS_UNREF( image );
                        image = x;
                }

                if( tile_source->scale != 1.0 ||
                        tile_source->offset != 0.0 ) {
                        if( vips_linear1( image, &x, 
                                tile_source->scale, tile_source->offset, 
                                "uchar", TRUE, 
                                NULL ) ) {
                                VIPS_UNREF( image );
                                VIPS_UNREF( alpha );
                                return( NULL ); 
                        }
                        VIPS_UNREF( image );
                        image = x;
                }

                if( tile_source->falsecolour ) {
                        if( vips_falsecolour( image, &x, NULL ) ) {
                                VIPS_UNREF( image );
                                VIPS_UNREF( alpha );
                                return( NULL ); 
                        }
                        VIPS_UNREF( image );
                        image = x;
                }

                if( alpha ) {
                        if( vips_bandjoin2( image, alpha, &x, NULL ) ) {
                                VIPS_UNREF( image );
                                VIPS_UNREF( alpha );
                                return( NULL ); 
                        }

                        VIPS_UNREF( image );
                        VIPS_UNREF( alpha );
                        image = x;
                }
        }

        if( vips_cast_uchar( image, &x, NULL ) ) {
                VIPS_UNREF( image );
                return( NULL ); 
        }
        VIPS_UNREF( image );
        image = x;

        return( image );
}

/* Rebuild just the second half of the image pipeline, eg. after a change to
 * falsecolour, or if current_z changes.
 */
static int
tile_source_update_rgb( TileSource *tile_source )
{
        if( tile_source->display ) { 
                VipsImage *rgb;

                if( !(rgb = tile_source_rgb_image( tile_source, 
                        tile_source->display )) ) 
                        return( -1 ); 
                VIPS_UNREF( tile_source->rgb );
                tile_source->rgb = rgb;

                VIPS_UNREF( tile_source->rgb_region );
                tile_source->rgb_region = vips_region_new( tile_source->rgb );
        }

        return( 0 );
}

/* Rebuild the entire display pipeline eg. after a page flip, or if current_z 
 * changes. 
 */
static int
tile_source_update_display( TileSource *tile_source )
{
        VipsImage *display;
        VipsImage *mask;

#ifdef DEBUG
        printf( "tile_source_update_display:\n" );
#endif /*DEBUG*/

        /* Don't update if we're still loading.
         */
        if( !tile_source->loaded ||
                !tile_source->image )
                return( 0 );

        if( !(display = tile_source_display_image( tile_source, &mask )) ) {
#ifdef DEBUG
                printf( "tile_source_update_display: build failed\n" );
#endif /*DEBUG*/
                return( -1 ); 
        }

        VIPS_UNREF( tile_source->display );
        VIPS_UNREF( tile_source->mask );
        tile_source->display = display;
        tile_source->mask = mask;

        VIPS_UNREF( tile_source->mask_region );
        tile_source->mask_region = vips_region_new( tile_source->mask );

        if( tile_source_update_rgb( tile_source ) )
                return( -1 );

        return( 0 );
}

#ifdef DEBUG
static const char *
tile_source_property_name( guint prop_id )
{
        switch( prop_id ) {
        case PROP_MODE:
                return( "MODE" );
                break;

        case PROP_SCALE:
                return( "SCALE" );
                break;

        case PROP_OFFSET:
                return( "OFFSET" );
                break;

        case PROP_PAGE:
                return( "PAGE" );
                break;

        case PROP_FALSECOLOUR:
                return( "FALSECOLOUR" );
                break;

        case PROP_LOG:
                return( "LOG" );
                break;

        case PROP_ACTIVE:
                return( "ACTIVE" );
                break;

        case PROP_LOADED:
                return( "LOADED" );
                break;

        default:
                return( "<unknown>" );
        }
}
#endif /*DEBUG*/

/* Each timeout fires once, sets the next timeout, and flips the page.
 */
static gboolean
tile_source_page_flip( void *user_data )
{
        TileSource *tile_source = (TileSource *) user_data;
        int page = VIPS_CLIP( 0, tile_source->page, tile_source->n_pages - 1 );

        int timeout;

        /* By convention, GIFs default to 10fps.
         */
        timeout = 100;

        if( tile_source->delay ) {
                int i = VIPS_MIN( page, tile_source->n_delay - 1 );

                /* By GIF convention, timeout 0 means unset.
                 */
                if( tile_source->delay[i] )
                        timeout = tile_source->delay[i];
        }

        /* vipsdisp struggles at more than 30fps.
         */
        timeout = VIPS_CLIP( 33, timeout, 100000 );

        tile_source->page_flip_id = 
                g_timeout_add( timeout, tile_source_page_flip, tile_source );

#ifdef DEBUG
        printf(  "tile_source_page_flip: timeout %d ms\n", timeout ); 
#endif /*DEBUG*/

        /* Only flip the page if everything has loaded.
         */
        if( tile_source->rgb ) {
                g_object_set( tile_source,
                        "page", (page + 1) % tile_source->n_pages,
                        NULL );
        }

        return( FALSE );
}

static void
tile_source_set_property( GObject *object, 
        guint prop_id, const GValue *value, GParamSpec *pspec )
{
        TileSource *tile_source = (TileSource *) object;

        int i;
        double d;
        gboolean b;

#ifdef DEBUG
{
        char *str;

        str = g_strdup_value_contents( value );
        printf( "tile_source_set_property: %s %s\n", 
                tile_source_property_name( prop_id ), str ); 
        g_free( str );
}
#endif /*DEBUG*/

        switch( prop_id ) {
        case PROP_MODE:
                i = g_value_get_int( value );
                if( i >= 0 &&
                        i < TILE_SOURCE_MODE_LAST &&
                        tile_source->mode != i ) {
                        tile_source->mode = i;
                        tile_source->display_width = tile_source->width;
                        tile_source->display_height = tile_source->height;
                        if( tile_source->mode == TILE_SOURCE_MODE_TOILET_ROLL )
                                tile_source->display_height *= 
                                        tile_source->n_pages;

                        tile_source_update_display( tile_source );

                        tile_source_changed( tile_source );

                        /* In animation mode, create the page flip timeout.
                         */
                        if( tile_source->page_flip_id )
                                VIPS_FREEF( g_source_remove, 
                                        tile_source->page_flip_id );
                        if( tile_source->mode == TILE_SOURCE_MODE_ANIMATED )
                                tile_source->page_flip_id = g_timeout_add( 100, 
                                        tile_source_page_flip, tile_source );
                }
                break;

        case PROP_SCALE:
                d = g_value_get_double( value );
                if( d > 0 &&
                        d <= 1000000 &&
                        tile_source->scale != d ) { 
                        tile_source->scale = d;
                        tile_source_update_rgb( tile_source );

                        tile_source_tiles_changed( tile_source );
                }
                break;

        case PROP_OFFSET:
                d = g_value_get_double( value );
                if( d >= -100000 &&
                        d <= 1000000 &&
                        tile_source->offset != d ) { 
                        tile_source->offset = d;
                        tile_source_update_rgb( tile_source );

                        tile_source_tiles_changed( tile_source );
                }
                break;

        case PROP_PAGE:
                i = g_value_get_int( value );
                if( i >= 0 &&
                        i <= 1000000 &&
                        tile_source->page != i ) {
                        tile_source->page = i;
                        tile_source_update_display( tile_source );

                        /* If all pages have the same size, we can flip pages
                         * without rebuilding the pyramid.
                         */
                        if( tile_source->pages_same_size )
                                tile_source_tiles_changed( tile_source );
                        else
                                tile_source_changed( tile_source );

                        tile_source_page_changed( tile_source );
                }
                break;

        case PROP_FALSECOLOUR:
                b = g_value_get_boolean( value );
                if( tile_source->falsecolour != b ) { 
                        tile_source->falsecolour = b;
                        tile_source_update_rgb( tile_source );

                        tile_source_tiles_changed( tile_source );
                }
                break;

        case PROP_LOG:
                b = g_value_get_boolean( value );
                if( tile_source->log != b ) { 
                        tile_source->log = b;
                        tile_source_update_rgb( tile_source );

                        tile_source_tiles_changed( tile_source );
                }
                break;

        case PROP_ACTIVE:
                b = g_value_get_boolean( value );
                if( tile_source->active != b ) { 
                        tile_source->active = b;
                        tile_source_update_rgb( tile_source );

                        tile_source_tiles_changed( tile_source );
                }
                break;

        case PROP_LOADED:
                b = g_value_get_boolean( value );
                if( tile_source->loaded != b ) { 
                        tile_source->loaded = b;
                        tile_source_update_display( tile_source );

                        tile_source_changed( tile_source );
                }
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID( object, prop_id, pspec );
                break;
        }
}

static void
tile_source_get_property( GObject *object, 
        guint prop_id, GValue *value, GParamSpec *pspec )
{
        TileSource *tile_source = (TileSource *) object;

        switch( prop_id ) {
        case PROP_MODE:
                g_value_set_int( value, tile_source->mode );
                break;

        case PROP_SCALE:
                g_value_set_double( value, tile_source->scale );
                break;

        case PROP_OFFSET:
                g_value_set_double( value, tile_source->offset );
                break;

        case PROP_PAGE:
                g_value_set_int( value, tile_source->page );
                break;

        case PROP_FALSECOLOUR:
                g_value_set_boolean( value, tile_source->falsecolour );
                break;

        case PROP_LOG:
                g_value_set_boolean( value, tile_source->log );
                break;

        case PROP_ACTIVE:
                g_value_set_boolean( value, tile_source->active );
                break;

        case PROP_LOADED:
                g_value_set_boolean( value, tile_source->loaded );
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID( object, prop_id, pspec );
                break;
        }

#ifdef DEBUG
{
        char *str;

        str = g_strdup_value_contents( value );
        printf( "tile_source_get_property: %s %s\n", 
                tile_source_property_name( prop_id ), str ); 
        g_free( str );
}
#endif /*DEBUG*/
}

static void
tile_source_init( TileSource *tile_source )
{
        tile_source->scale = 1.0;
}

static void
tile_source_force_load( TileSource *tile_source )
{
        if( tile_source->image_region &&
                !tile_source->loaded ) { 
                VipsRect rect;

                rect.left = 0;
                rect.top = 0;
                rect.width = 1;
                rect.height = 1;
                (void) vips_region_prepare( tile_source->image_region, &rect );
        }
}

/* This runs in the main thread when the bg load is done. We can't use
 * postload since that will only fire if we are actually loading, and not if
 * the image is coming from cache.
 */
static gboolean
tile_source_background_load_done_idle( void *user_data )
{
        TileSource *tile_source = (TileSource *) user_data;

#ifdef DEBUG
        printf( "tile_source_background_load_done_cb:\n" );
#endif /*DEBUG*/

        /* You can now fetch pixels.
         */
        g_object_set( tile_source, "loaded", TRUE, NULL );

        /* Drop the ref that kept this tile_source alive during load.
         */
        g_object_unref( tile_source ); 

        return( FALSE ); 
}

/* This runs for the background load threadpool.
 */
static void 
tile_source_background_load( void *data, void *user_data )
{
        TileSource *tile_source = (TileSource *) data;

#ifdef DEBUG
        printf( "tile_source_background_load: starting ..\n" );
#endif /*DEBUG*/

        g_assert( tile_source->image_region ); 

        tile_source_force_load( tile_source );

        g_idle_add( tile_source_background_load_done_idle, tile_source );

#ifdef DEBUG
        printf( "tile_source_background_load: .. done\n" );
#endif /*DEBUG*/
}

static void
tile_source_class_init( TileSourceClass *class )
{
        GObjectClass *gobject_class = G_OBJECT_CLASS( class );

        gobject_class->dispose = tile_source_dispose;
        gobject_class->set_property = tile_source_set_property;
        gobject_class->get_property = tile_source_get_property;

        g_object_class_install_property( gobject_class, PROP_MODE,
                g_param_spec_int( "mode",
                        _( "Mode" ),
                        _( "Display mode" ),
                        0, TILE_SOURCE_MODE_LAST - 1, 
                        TILE_SOURCE_MODE_MULTIPAGE,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_SCALE,
                g_param_spec_double( "scale",
                        _( "scale" ),
                        _( "Scale" ),
                        -1000000, 1000000, 1,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_OFFSET,
                g_param_spec_double( "offset",
                        _( "offset" ),
                        _( "Offset" ),
                        -1000000, 1000000, 0,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_PAGE,
                g_param_spec_int( "page",
                        _( "Page" ),
                        _( "Page number" ),
                        0, 1000000, 0,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_FALSECOLOUR,
                g_param_spec_boolean( "falsecolour",
                        _( "falsecolour" ),
                        _( "False colour" ),
                        FALSE,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_LOG,
                g_param_spec_boolean( "log",
                        _( "log" ),
                        _( "Log" ),
                        FALSE,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_ACTIVE,
                g_param_spec_boolean( "active",
                        _( "Active" ),
                        _( "Visualisation controls are active" ),
                        FALSE,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_LOADED,
                g_param_spec_boolean( "loaded",
                        _( "loaded" ),
                        _( "Image has finished loading" ),
                        FALSE,
                        G_PARAM_READWRITE ) );

        tile_source_signals[SIG_PREEVAL] = g_signal_new( "preeval",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET( TileSourceClass, preeval ), 
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1,
                G_TYPE_POINTER );

        tile_source_signals[SIG_EVAL] = g_signal_new( "eval",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET( TileSourceClass, eval ), 
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1,
                G_TYPE_POINTER );

        tile_source_signals[SIG_POSTEVAL] = g_signal_new( "posteval",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET( TileSourceClass, posteval ), 
                NULL, NULL,
                g_cclosure_marshal_VOID__POINTER,
                G_TYPE_NONE, 1,
                G_TYPE_POINTER );

        tile_source_signals[SIG_CHANGED] = g_signal_new( "changed",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET( TileSourceClass, changed ), 
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0 ); 

        tile_source_signals[SIG_TILES_CHANGED] = g_signal_new( "tiles-changed",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET( TileSourceClass, tiles_changed ), 
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0 ); 

        tile_source_signals[SIG_AREA_CHANGED] = g_signal_new( "area-changed",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET( TileSourceClass, area_changed ), 
                NULL, NULL,
                vipsdisp_VOID__POINTER_INT,
                G_TYPE_NONE, 2,
                G_TYPE_POINTER,
                G_TYPE_INT );

        tile_source_signals[SIG_PAGE_CHANGED] = g_signal_new( "page-changed",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET( TileSourceClass, page_changed ), 
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0 ); 

        g_assert( !tile_source_background_load_pool );
        tile_source_background_load_pool = g_thread_pool_new(
                tile_source_background_load,
                NULL, -1, FALSE, NULL );

}

#ifdef DEBUG
static const char *
type_name( TileSourceType type ) 
{
        switch( type ) {
        case TILE_SOURCE_TYPE_PAGE_PYRAMID:
                return( "pyramid" );
        case TILE_SOURCE_TYPE_TOILET_ROLL:
                return( "toilet-roll" );
        case TILE_SOURCE_TYPE_MULTIPAGE:
                return( "multipage" );
        default:
                return( "<unknown>" );
        }
}

static const char *
mode_name( TileSourceMode mode ) 
{
        switch( mode ) {
        case TILE_SOURCE_MODE_TOILET_ROLL:
                return( "toilet-roll" );
        case TILE_SOURCE_MODE_MULTIPAGE:
                return( "multipage" );
        case TILE_SOURCE_MODE_ANIMATED:
                return( "animated" );
        case TILE_SOURCE_MODE_PAGES_AS_BANDS:
                return( "pages-as-bands" );
        default:
                return( "<unknown>" );
        }
}

static void
tile_source_print( TileSource *tile_source )
{
        int i;

        printf( "tile_source: %p\n", tile_source );
        printf( "\tloader = %s\n", tile_source->loader );
        printf( "\twidth = %d\n", tile_source->width );
        printf( "\theight = %d\n", tile_source->height );
        printf( "\tn_pages = %d\n", tile_source->n_pages );
        printf( "\tn_subifds = %d\n", tile_source->n_subifds );
        printf( "\tsubifd_pyramid = %d\n", tile_source->subifd_pyramid );
        printf( "\tpage_pyramid = %d\n", tile_source->page_pyramid );
        printf( "\tlevel_count = %d\n", tile_source->level_count );

        for( i = 0; i < tile_source->level_count; i++ )
                printf( "\t%2d) %d x %d\n", 
                        i,
                        tile_source->level_width[i], 
                        tile_source->level_height[i] ); 

        printf( "\tpages_same_size = %d\n", tile_source->pages_same_size );
        printf( "\tall_mono = %d\n", tile_source->all_mono );
        printf( "\ttype = %s\n", type_name( tile_source->type ) );
        printf( "\tmode = %s\n", mode_name( tile_source->mode ) );
        printf( "\tdelay = %p\n", tile_source->delay );
        printf( "\tn_delay = %d\n", tile_source->n_delay );
        printf( "\tdisplay_width = %d\n", tile_source->display_width );
        printf( "\tdisplay_height = %d\n", tile_source->display_height );
}
#endif /*DEBUG*/

/* Sniff basic image properties.
 */
static int
tile_source_set_image( TileSource *tile_source, VipsImage *image )
{
#ifdef DEBUG
        printf( "tile_source_set_image:\n" );
#endif /*DEBUG*/

        tile_source->width = image->Xsize;
        tile_source->height = vips_image_get_page_height( image );
        tile_source->bands = image->Bands;
        tile_source->n_pages = vips_image_get_n_pages( image );
        tile_source->n_subifds = vips_image_get_n_subifds( image );

        /* No reopening, so have (in effect) all pages open at once.
         */
        tile_source->type = TILE_SOURCE_TYPE_TOILET_ROLL;

        if( vips_image_get_typeof( image, "delay" ) ) {
                int *delay;
                int n_delay;

                if( vips_image_get_array_int( image, "delay",
                        &delay, &n_delay ) )
                        return( -1 );

                tile_source->delay = g_new( int, n_delay );
                memcpy( tile_source->delay, delay, n_delay * sizeof( int ) );
                tile_source->n_delay = n_delay;
        }

#ifdef DEBUG
        tile_source_print( tile_source );
#endif /*DEBUG*/

        return( 0 );
}

TileSource *
tile_source_new_from_image( VipsImage *image )
{
        TileSource *tile_source = g_object_new( TILE_SOURCE_TYPE, NULL );

        /* Only call this once.
         */
        g_assert( !tile_source->image );

        if( tile_source_set_image( tile_source, image ) ) {
                VIPS_UNREF( tile_source );
                return( NULL );
        }

        tile_source->image = image;
        g_object_ref( image );
        tile_source->image_region = vips_region_new( tile_source->image );

        return( tile_source );
}

/* Detect a TIFF pyramid made of subifds following a roughly /2 shrink.
 */
static void
tile_source_get_pyramid_subifd( TileSource *tile_source )
{
        int i;

#ifdef DEBUG
        printf( "tile_source_get_pyramid_subifd:\n" );
#endif /*DEBUG*/

        for( i = 0; i < tile_source->n_subifds; i++ ) {
                VipsImage *level;
                int level_width;
                int level_height;
                int expected_level_width;
                int expected_level_height;

                /* Just bail out if there are too many levels.
                 */
                if( i >= MAX_LEVELS )
                        break;

                if( !(level = tile_source_open( tile_source, i )) )
                        return;
                level_width = level->Xsize;
                level_height = level->Ysize;
                VIPS_UNREF( level );

                expected_level_width = tile_source->width / (1 << i);
                expected_level_height = tile_source->height / (1 << i);

                /* This won't be exact due to rounding etc.
                 */
                if( abs( level_width - expected_level_width ) > 5 ||
                        level_width < 2 ||
                        abs( level_height - expected_level_height ) > 5 ||
                        level_height < 2 ) {
#ifdef DEBUG
                        printf( "  bad subifd level %d\n", i );
#endif /*DEBUG*/
                        return;
                }

                tile_source->level_width[i] = level_width;
                tile_source->level_height[i] = level_height;
        }

        /* Tag as a subifd pyramid.
         */
        tile_source->subifd_pyramid = TRUE;
        tile_source->level_count = tile_source->n_subifds;
}

/* Detect a pyramid made of pages following a roughly /2 shrink. Can be eg.
 * jp2k or TIFF.
 */
static void
tile_source_get_pyramid_page( TileSource *tile_source )
{
        int i;

#ifdef DEBUG
        printf( "tile_source_get_pyramid_page:\n" );
#endif /*DEBUG*/

        /* Single-page docs can't be pyramids.
         */
        if( tile_source->n_pages < 2 )
                return;

        for( i = 0; i < tile_source->n_pages; i++ ) {
                VipsImage *level;
                int level_width;
                int level_height;
                int expected_level_width;
                int expected_level_height;

                /* Just bail out if there are too many levels.
                 */
                if( i >= MAX_LEVELS )
                        break;

                if( !(level = tile_source_open( tile_source, i )) )
                        return;
                level_width = level->Xsize;
                level_height = level->Ysize;
                VIPS_UNREF( level );

                expected_level_width = tile_source->width / (1 << i);
                expected_level_height = tile_source->height / (1 << i);

                /* This won't be exact due to rounding etc.
                 */
                if( abs( level_width - expected_level_width ) > 5 ||
                        level_width < 2 )
                        return;
                if( abs( level_height - expected_level_height ) > 5 ||
                        level_height < 2 )
                        return;

                tile_source->level_width[i] = level_width;
                tile_source->level_height[i] = level_height;
        }

        /* Tag as a page pyramid.
         */
        tile_source->page_pyramid = TRUE;
        tile_source->level_count = tile_source->n_pages;
}

static void
tile_source_preeval( VipsImage *image, 
        VipsProgress *progress, TileSource *tile_source )
{
        g_signal_emit( tile_source, 
                tile_source_signals[SIG_PREEVAL], 0, progress );
}

static void
tile_source_eval( VipsImage *image, 
        VipsProgress *progress, TileSource *tile_source )
{
        g_signal_emit( tile_source, 
                tile_source_signals[SIG_EVAL], 0, progress );
}

static void
tile_source_posteval( VipsImage *image, 
        VipsProgress *progress, TileSource *tile_source )
{
        g_signal_emit( tile_source, 
                tile_source_signals[SIG_POSTEVAL], 0, progress );
}

static void
tile_source_attach_progress( TileSource *tile_source )
{
#ifdef DEBUG
        printf( "tile_source_attach_progress:\n" ); 
#endif /*DEBUG*/

        vips_image_set_progress( tile_source->image, TRUE ); 
        g_signal_connect_object( tile_source->image, "preeval", 
                G_CALLBACK( tile_source_preeval ), tile_source, 0 );
        g_signal_connect_object( tile_source->image, "eval",
                G_CALLBACK( tile_source_eval ), tile_source, 0 );
        g_signal_connect_object( tile_source->image, "posteval",
                G_CALLBACK( tile_source_posteval ), tile_source, 0 );
}

/* Fetch a string-encoded int image header field, eg. from openslide. These
 * are all represented as strings. Return the default value if there's any
 * problem.
 */
static int
get_int( VipsImage *image, const char *field, int default_value )
{
        const char *str;

        if( vips_image_get_typeof( image, field ) &&
                !vips_image_get_string( image, field, &str ) )
                return( atoi( str ) );

        return( default_value );
}

TileSource *
tile_source_new_from_source( VipsSource *source )
{
        TileSource *tile_source = g_object_new( TILE_SOURCE_TYPE, NULL );

        const char *loader;
        VipsImage *image;
        VipsImage *x;
        TileSourceMode mode;

#ifdef DEBUG
        printf( "tile_source_new_from_source: starting ..\n" );
#endif /*DEBUG*/

        tile_source->source = source; 
        g_object_ref( source );

        if( !(loader = vips_foreign_find_load_source( source )) ) {
                VIPS_UNREF( tile_source );
                return( NULL );
        }

        /* vips_foreign_find_load_source() gives us eg.
         * "VipsForeignLoadNsgifFile", but we need "gifload_source", the
         * generic name.
         */
        tile_source->loader = vips_nickname_find( g_type_from_name( loader ) );

        /* A very basic open to fetch metadata. 
         */
        if( !(image = vips_image_new_from_source( source, "", 
		NULL )) ) {
                VIPS_UNREF( tile_source );
                return( NULL );
        }

        if( tile_source_set_image( tile_source, image ) ) {
                VIPS_UNREF( image );
                VIPS_UNREF( tile_source );
                return( NULL );
        }

        /* For openslide, we can read out the level structure directly.
         */
        if( vips_image_get_typeof( image, "openslide.level-count" ) ) {
                int level_count;
                int level;

                level_count = get_int( image, "openslide.level-count", 1 );
                level_count = VIPS_CLIP( 1, level_count, MAX_LEVELS );
                tile_source->level_count = level_count;

                for( level = 0; level < level_count; level++ ) {
                        char name[256];

                        vips_snprintf( name, 256,
                                "openslide.level[%d].width", level );
                        tile_source->level_width[level] =
                                 get_int( image, name, 0 );
                        vips_snprintf( name, 256,
                                "openslide.level[%d].height", level );
                        tile_source->level_height[level] =
                                get_int( image, name, 0 );
                }
        }

        VIPS_UNREF( image );

        /* Can we open in toilet-roll mode? We need to test that n_pages and
         * page_size are sane too. 
         */
#ifdef DEBUG
        printf( "tile_source_new_from_source: test toilet-roll mode\n" );
#endif /*DEBUG*/

        /* Block error messages from eg. page-pyramidal TIFFs, where pages
         * are not all the same size.
         */
        tile_source->type = TILE_SOURCE_TYPE_TOILET_ROLL;
        vips_error_freeze();
        x = tile_source_open( tile_source, 0 );
        vips_error_thaw();
        if( x ) {
                /* Toilet-roll mode worked. Check sanity of page height,
                 * n_pages and Ysize too.
                 */
                if( tile_source->n_pages * tile_source->height != x->Ysize ||
                        tile_source->n_pages <= 0 ||
                        tile_source->n_pages > 10000 ) {
#ifdef DEBUG
                        printf( "tile_source_new_from_source: "
                                "bad page layout\n" );
#endif /*DEBUG*/

                        tile_source->n_pages = 1;
                        tile_source->height = x->Ysize;
                        VIPS_FREE( tile_source->delay );
                        tile_source->n_delay = 0;
                }
                else
                        /* Everything looks good.
                         */
                        tile_source->pages_same_size = TRUE;

                VIPS_UNREF( x );
        }

        /* Back to plain multipage for the rest of the sniff period. For
         * example, subifd pyramid needs single page opening.
         *
         * We reset this at the end.
         */
        tile_source->type = TILE_SOURCE_TYPE_MULTIPAGE;

        /* Are all pages the same size and format, and also all mono (one
         * band)? We can display pages-as-bands.
         */
        tile_source->all_mono = 
                tile_source->pages_same_size && 
                tile_source->bands == 1;

        /* Test for a subifd pyr first, since we can do that from just
         * one page.
         */
        if( !tile_source->level_count ) {
                tile_source->subifd_pyramid = TRUE;
                tile_source_get_pyramid_subifd( tile_source );
                if( !tile_source->level_count )
                        tile_source->subifd_pyramid = FALSE;
        }

        /* If that failed, try to read as a page pyramid.
         */
        if( !tile_source->level_count ) {
                tile_source->page_pyramid = TRUE;
                tile_source_get_pyramid_page( tile_source );
                if( !tile_source->level_count )
                        tile_source->page_pyramid = FALSE;
        }

        /* Sniffing is done ... set the image type.
         */
        if( tile_source->pages_same_size )
                tile_source->type = TILE_SOURCE_TYPE_TOILET_ROLL;
        else {
                if( tile_source->page_pyramid )
                        tile_source->type = TILE_SOURCE_TYPE_PAGE_PYRAMID;
                else
                        tile_source->type = TILE_SOURCE_TYPE_MULTIPAGE;
        }

        /* Pick a default display mode.
         */
        if( tile_source->type == TILE_SOURCE_TYPE_TOILET_ROLL ) {
                if( tile_source->delay )
                        mode = TILE_SOURCE_MODE_ANIMATED;
                else if( tile_source->all_mono )
                        mode = TILE_SOURCE_MODE_PAGES_AS_BANDS;
                else
                        mode = TILE_SOURCE_MODE_MULTIPAGE;
        }
        else 
                mode = TILE_SOURCE_MODE_MULTIPAGE;

#ifdef DEBUG
        printf( "tile_source_new_from_source: after sniff\n" );
        tile_source_print( tile_source );
#endif /*DEBUG*/

        /* And now we can reopen in the correct mode.
         */
        if( !(image = tile_source_open( tile_source, 0 )) ) {
                VIPS_UNREF( tile_source );
                return( NULL );
        }
	g_assert( !tile_source->image );
	g_assert( !tile_source->image_region );
        tile_source->image = image;
        tile_source->image_region = vips_region_new( tile_source->image );

	/* image_region is used by the bg load thread.
	 */
	vips__region_no_ownership( tile_source->image_region );

        g_object_set( tile_source, 
                "mode", mode, 
                NULL );

        /* We ref this tile_source so it won't die before the
         * background load is done. The matching unref is at the end
         * of bg load.
         */
        g_object_ref( tile_source );

        /* This will be set TRUE again at the end of the background
         * load. This will trigger tile_source_update_display() for us.
         */
        g_object_set( tile_source, 
                "loaded", FALSE, 
                NULL );

        tile_source_attach_progress( tile_source ); 

        g_thread_pool_push( tile_source_background_load_pool, 
                tile_source, NULL );

        return( tile_source );
}

TileSource *
tile_source_new_from_file( GFile *file )
{
        GError *error = NULL;

        VipsSource *source;
        char *path;
        TileSource *tile_source;

        if( (path = g_file_get_path( file )) ) {
                /* If this GFile is a path to a file on disc, we can
                 * make a source directly from it. This will allow
                 * things like mmap and openslide to work.
                 */

#ifdef DEBUG
                printf( "tile_source_set_file: connecting via path\n" );
#endif /*DEBUG*/

                if( !(source = vips_source_new_from_file( path )) ) {
                        g_free( path );
                        return( NULL );
                }
                g_free( path );
        }
        else {
                /* Otherwise, this is perhaps a pipe or an area of
                 * memory. We can connect via g_input_stream.
                 */
                GInputStream *stream;

#ifdef DEBUG
                printf( "tile_source_set_file: connecting via "
                        "ginputstream\n" );
#endif /*DEBUG*/

                if( !(stream = G_INPUT_STREAM( 
                        g_file_read( file, NULL, &error ) )) ) {
                        vips_error_g( &error );
                        return( NULL );
                }

                if( !(source = VIPS_SOURCE( 
                        vips_source_g_input_stream_new( stream ) )) ) {
                        VIPS_UNREF( stream );
                        return( NULL );
                }
                VIPS_UNREF( stream );
        }

        if( !(tile_source = tile_source_new_from_source( source )) ) {
                VIPS_UNREF( source );
                return( NULL );
        }
        VIPS_UNREF( source );

        return( tile_source );
}

int
tile_source_fill_tile( TileSource *tile_source, Tile *tile ) 
{
#ifdef DEBUG_VERBOSE
        printf( "tile_source_fill_tile: %d x %d\n",
             tile->region->valid.left, tile->region->valid.top ); 
#endif /*DEBUG_VERBOSE*/

        /* Change z if necessary.
         */
        if( tile_source->current_z != tile->z ||
                !tile_source->display ) {
                tile_source->current_z = tile->z;
                tile_source_update_display( tile_source );
        }

        if( vips_region_prepare( tile_source->mask_region, 
                &tile->region->valid ) )
                return( -1 );

        /* tile is within a single tile, so we only need to test the first byte
         * of the mask. 
         */
        tile->valid = VIPS_REGION_ADDR( tile_source->mask_region, 
                tile->region->valid.left, tile->region->valid.top )[0];

#ifdef DEBUG_VERBOSE
        printf( "  valid = %d\n", tile->valid ); 
#endif /*DEBUG_VERBOSE*/

        /* We must always prepare the region, even if we know it's blank,
         * since this will trigger the background render.
         */
        if( vips_region_prepare_to( tile_source->rgb_region, tile->region,
                &tile->region->valid, 
                tile->region->valid.left, 
                tile->region->valid.top ) )
                return( -1 );

        /* Do we have new, valid pixels? Update the texture. We need to do
	 * this now since the data in the region may change later.
         */
        if( tile->valid ) {
                tile_free_texture( tile );
		tile_get_texture( tile );
	}

        return( 0 );
}

const char *
tile_source_get_path( TileSource *tile_source )
{
        if( tile_source->source )
                return( vips_connection_filename( 
                        VIPS_CONNECTION( tile_source->source ) ) );

        return( NULL );
}

GFile *
tile_source_get_file( TileSource *tile_source )
{
        const char *path;

        if( (path = tile_source_get_path( tile_source )) ) 
                return( g_file_new_for_path( path ) );

        return( NULL );
}

int
tile_source_write_to_file( TileSource *tile_source, GFile *file )
{
        char *path;
        int result;

        if( !(path = g_file_get_path( file )) )
                return( -1 );

        vips_image_set_progress( tile_source->image, TRUE ); 
        result = vips_image_write_to_file( tile_source->image, path, NULL );
        g_free( path );

        return( result );
}

VipsImage *
tile_source_get_image( TileSource *tile_source )
{
        return( tile_source->image );
}

VipsPel *
tile_source_get_pixel( TileSource *tile_source, int x, int y )
{
        VipsRect rect;

        if( !tile_source->loaded ||
                !tile_source->image ||
                !tile_source->image_region )
                return( NULL );

        rect.left = x;
        rect.top = y;
        rect.width = 1;
        rect.height = 1;
        if( vips_region_prepare( tile_source->image_region, &rect ) )
                return( NULL );

        return( VIPS_REGION_ADDR( tile_source->image_region, x, y ) );
}

TileSource *
tile_source_duplicate( TileSource *tile_source )
{
        g_assert( FALSE );

        /* FIXME ... see conversion_set_conversion()
         *
         *
         * tile_source_get_file(), then new_from_file, then copy settings
         */

        return( NULL );
}
