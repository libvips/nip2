#ifndef __TILE_SOURCE_H
#define __TILE_SOURCE_H

#define TILE_SOURCE_TYPE (tile_source_get_type())
#define TILE_SOURCE( obj ) \
        (G_TYPE_CHECK_INSTANCE_CAST( (obj), TILE_SOURCE_TYPE, TileSource ))
#define TILE_SOURCE_CLASS( klass ) \
        (G_TYPE_CHECK_CLASS_CAST( (klass), TILE_SOURCE_TYPE, TileSourceClass))
#define IS_TILE_SOURCE( obj ) \
        (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TILE_SOURCE_TYPE ))
#define IS_TILE_SOURCE_CLASS( klass ) \
        (G_TYPE_CHECK_CLASS_TYPE( (klass), TILE_SOURCE_TYPE ))
#define TILE_SOURCE_GET_CLASS( obj ) \
        (G_TYPE_INSTANCE_GET_CLASS( (obj), TILE_SOURCE_TYPE, TileSourceClass ))

/* The three basic types of image we support.
 *
 * MULTIPAGE
 *
 *      Pages differ in size or perhaps format, so must be loaded as separate
 *      images. Pages can have subifd pyramids. Includes single-page images.
 *      Reload on page change.
 *
 * PAGE_PYRAMID 
 *
 *      "page" param is pyr levels. We load a single page and reload on
 *      magnification change.
 *
 * TOILET_ROLL
 *
 *      All pages are the identical, so we open as a single, tall, thin strip 
 *      and the viewer does any presenting as pages / animation / etc. during 
 *      conversion to the screen display image.
 *      These images can have subifd pyramids.
 */
typedef enum _TileSourceType {
        TILE_SOURCE_TYPE_MULTIPAGE,
        TILE_SOURCE_TYPE_PAGE_PYRAMID,
        TILE_SOURCE_TYPE_TOILET_ROLL,
} TileSourceType;

/* The modes of image display we support.
 *
 * TOILET_ROLL
 *
 *      Just show the whole image (no crop). Page control disabled. Reload on
 *      mag change if there's a pyramid.
 *
 * MULTIPAGE
 *
 *      Behaviour depends on TileSourceImage:
 *
 *      TILE_SOURCE_TYPE_PAGE_PYRAMID
 *              
 *              Disable page controls. No crop. Reload on mag change.
 *
 *      TILE_SOURCE_TYPE_TOILET_ROLL
 *
 *              Enable page control iff > 1 page. Crop in display conversion
 *              to select page.
 *
 *      TILE_SOURCE_TYPE_MULTIPAGE
 *
 *              Enable page control iff > 1 page.
 *
 * ANIMATED
 *
 *      Just like MULTIPAGE, except page flip is driven by a timeout.
 *
 * PAGES_AS_BANDS
 *
 *      Just like toilet roll, exccept that we chop the image into pages and
 *      bandjoin them all. Handy for OME-TIFF, which has a one-band image
 *      in each page.
 */
typedef enum _TileSourceMode {
        TILE_SOURCE_MODE_TOILET_ROLL,
        TILE_SOURCE_MODE_MULTIPAGE,
        TILE_SOURCE_MODE_ANIMATED,
        TILE_SOURCE_MODE_PAGES_AS_BANDS,
        TILE_SOURCE_MODE_LAST
} TileSourceMode;

/* Max number of levels we allow in a pyramidal image.
 */
#define MAX_LEVELS (256)

typedef struct _TileSource {
        GObject parent_instance;

        /* The loader and the source we have loaded. We may need to reload on
         * a zoom or page change, so we need to keep the source.
         */
        const char *loader;
        VipsSource *source;

        /* The image we are displaying, and something to fetch pixels from it
         * with. 
         */
        VipsImage *image;
        VipsRegion *image_region;

        /* What sort of image we have, and how we are displaying it.
         */
        TileSourceType type;
        TileSourceMode mode;

        /* This is a TIFF subifd pyramid.
         */
        gboolean subifd_pyramid;

        /* This is a page pyramid (TIFF, jp2k etc.).
         */
        gboolean page_pyramid;

        /* Basic image geometry. The tilecache pyramid is based on this.
         */
        int width;
        int height;
        int bands;
        int n_pages;
        int n_subifds;
        int *delay;
        int n_delay;

        /* If all the pages are the same size and format, we can load as a 
         * toilet roll.
         */
        gboolean pages_same_size;

        /* If all the pages are the same size and format, and also all mono,
         * we can display pages as bands. 
         */
        gboolean all_mono;

        /* For pyramidal formats, we need to read out the size of each level.
         * Largest level first.
         */
        int level_count;
        int level_width[MAX_LEVELS];
        int level_height[MAX_LEVELS];

        /* Display transform parameters.
         */
        int page;
        gboolean active;
        double scale;
        double offset;
        gboolean falsecolour;
        gboolean log;

        /* The size of the image with this view mode. So in toilet-roll mode
         * (for example), display_height is height * n_pages.
         */
        int display_width;
        int display_height;

        /* The current z for display, mask, rgb. We need to rebuild the
         * pipeline on z changes.
         */
        int current_z;

        /* The image resized for the display, ie. including shrink & zoom, and
         * a cache mask.
         */
        VipsImage *display;
        VipsImage *mask;

        /* The display image converted to display RGB for painting.
         */
        VipsImage *rgb;
        VipsRegion *rgb_region;
        VipsRegion *mask_region;

        /* For animations, the timeout we use for page flip.
         */
        guint page_flip_id;

        /* TRUE when the image has fully loaded (ie. postload has fired) and we
         * can start looking at pixels.
         */
        gboolean loaded;

} TileSource;

typedef struct _TileSourceClass {
        GObjectClass parent_class;

        /* Signal image load.
         */
        void (*preeval)( TileSource *tile_source, VipsProgress *progress );
        void (*eval)( TileSource *tile_source, VipsProgress *progress );
        void (*posteval)( TileSource *tile_source, VipsProgress *progress );

        /* Everything has changed, so image geometry and pixels. Perhaps a
         * new page in a multi-page TIFF where pages change in size. 
         */
        void (*changed)( TileSource *tile_source );

        /* All tiles have changed, but image geometry has not. Falsecolour,
         * page flip in a GIF, etc.
         */
        void (*tiles_changed)( TileSource *tile_source );

        /* A set of tiles on a certain level have new pixels now that a
         * background render has completed.
         */
        void (*area_changed)( TileSource *tile_source, VipsRect *area, int z );

        /* The page has changed. Just for updating the page number display.
         */
        void (*page_changed)( TileSource *tile_source );

} TileSourceClass;

GType tile_source_get_type( void );

TileSource *tile_source_new_from_source( VipsSource *source );
TileSource *tile_source_new_from_file( GFile *file );

int tile_source_fill_tile( TileSource *tile_source, Tile *tile );

const char *tile_source_get_path( TileSource *tile_source );
GFile *tile_source_get_file( TileSource *tile_source );
int tile_source_write_to_file( TileSource *tile_source, GFile *file );

VipsImage *tile_source_get_image( TileSource *tile_source );
VipsPel *tile_source_get_pixel( TileSource *tile_source, int x, int y );
TileSource *tile_source_duplicate( TileSource *tile_source );

#endif /*__TILE_SOURCE_H*/
