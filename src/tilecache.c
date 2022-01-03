#include "vipsdisp.h"

/*
#define DEBUG_VERBOSE
#define DEBUG
#define DEBUG_RENDER_TIME
 */

enum {
        /* Signals. 
         */
        SIG_CHANGED,            
        SIG_TILES_CHANGED,            
        SIG_AREA_CHANGED,            

        SIG_LAST
};

static guint tile_cache_signals[SIG_LAST] = { 0 };

G_DEFINE_TYPE( TileCache, tile_cache, G_TYPE_OBJECT );

static void
tile_cache_free_pyramid( TileCache *tile_cache )
{
        int i;

        for( i = 0; i < tile_cache->n_levels; i++ ) {
                GSList *p;

                for( p = tile_cache->tiles[i]; p; p = p->next ) {
                        Tile *tile = TILE( p->data );

                        VIPS_UNREF( tile );
                }

                VIPS_FREEF( g_slist_free, tile_cache->tiles[i] );
                VIPS_FREEF( g_slist_free, tile_cache->visible[i] );
                VIPS_FREEF( g_slist_free, tile_cache->free[i] );

                VIPS_UNREF( tile_cache->levels[i] );
        }

        VIPS_FREE( tile_cache->levels );
        tile_cache->n_levels = 0;
}

static void
tile_cache_dispose( GObject *object )
{
        TileCache *tile_cache = (TileCache *) object;

#ifdef DEBUG
        printf( "tile_cache_dispose: %p\n", object );
#endif /*DEBUG*/

        tile_cache_free_pyramid( tile_cache );

        VIPS_UNREF( tile_cache->tile_source );
	VIPS_UNREF( tile_cache->checkerboard );

        G_OBJECT_CLASS( tile_cache_parent_class )->dispose( object );
}

static void
tile_cache_changed( TileCache *tile_cache )
{
        g_signal_emit( tile_cache, 
                tile_cache_signals[SIG_CHANGED], 0 );
}

static void
tile_cache_tiles_changed( TileCache *tile_cache )
{
        g_signal_emit( tile_cache, 
                tile_cache_signals[SIG_TILES_CHANGED], 0 );
}

static void
tile_cache_area_changed( TileCache *tile_cache, VipsRect *dirty, int z )
{
        g_signal_emit( tile_cache, 
                tile_cache_signals[SIG_AREA_CHANGED], 0, dirty, z );
}

static void
tile_cache_checkerboard_destroy_notify( guchar* pixels, gpointer data )
{
        g_free( pixels );
}

/* Make a GdkTexture for the checkerboard pattern we use for compositing.
 */
static GdkTexture *
tile_cache_checkerboard( void )
{
        VipsPel *data;
        GdkPixbuf *pixbuf;
        GdkTexture *texture;
        int x, y, z;

        data = g_malloc( TILE_SIZE * TILE_SIZE * 3 );
        for( y = 0; y < TILE_SIZE; y++ )
                for( x = 0; x < TILE_SIZE; x++ )
                        for( z = 0; z < 3; z++ )
                                data[y * TILE_SIZE * 3 + x * 3 + z] =
                                       ((x >> 4) + (y >> 4)) % 2 == 0 ? 
                                               128 : 180;

        pixbuf = gdk_pixbuf_new_from_data( data, GDK_COLORSPACE_RGB,
                FALSE, 8,
                TILE_SIZE, TILE_SIZE, TILE_SIZE * 3,
                tile_cache_checkerboard_destroy_notify, NULL );

        texture = gdk_texture_new_for_pixbuf( pixbuf );

        g_object_unref( pixbuf );

        return( texture );
}

static void
tile_cache_init( TileCache *tile_cache )
{
        tile_cache->checkerboard = tile_cache_checkerboard();
}

static void
tile_cache_class_init( TileCacheClass *class )
{
        GObjectClass *gobject_class = G_OBJECT_CLASS( class );

        gobject_class->dispose = tile_cache_dispose;

        tile_cache_signals[SIG_CHANGED] = g_signal_new( "changed",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                0,
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0 ); 

        tile_cache_signals[SIG_TILES_CHANGED] = g_signal_new( "tiles-changed",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                0,
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0 ); 

        tile_cache_signals[SIG_AREA_CHANGED] = g_signal_new( "area-changed",
                G_TYPE_FROM_CLASS( class ),
                G_SIGNAL_RUN_LAST,
                0,
                NULL, NULL,
                vipsdisp_VOID__POINTER_INT,
                G_TYPE_NONE, 2,
                G_TYPE_POINTER,
                G_TYPE_INT );

}

static void
tile_cache_build_pyramid( TileCache *tile_cache )
{
        int n_levels;
        int level_width;
        int level_height;
        int i;

#ifdef DEBUG
        printf( "tile_cache_build_pyramid:\n" );
#endif /*DEBUG*/

        tile_cache_free_pyramid( tile_cache );

        /* How many levels? Keep shrinking until we get down to one tile on
         * one axis.
         */
        level_width = tile_cache->tile_source->display_width;
        level_height = tile_cache->tile_source->display_height;
        n_levels = 1;
        for(;;) {
                if( level_width <= TILE_SIZE ||
                        level_height <= TILE_SIZE )
                        break;

                level_width >>= 1;
                level_height >>= 1;
                n_levels += 1;
        }

        tile_cache->n_levels = n_levels;

        tile_cache->levels = VIPS_ARRAY( NULL, n_levels, VipsImage * );
        level_width = tile_cache->tile_source->display_width;
        level_height = tile_cache->tile_source->display_height;
        for( i = 0; i < n_levels; i++ ) {
                tile_cache->levels[i] = vips_image_new();

                vips_image_init_fields( tile_cache->levels[i],
                        level_width,
                        level_height,
                        tile_cache->tile_source->rgb->Bands,
                        tile_cache->tile_source->rgb->BandFmt,
                        tile_cache->tile_source->rgb->Coding,
                        tile_cache->tile_source->rgb->Type,
                        tile_cache->tile_source->rgb->Xres,
                        tile_cache->tile_source->rgb->Yres );

                level_width >>= 1;
                level_height >>= 1;
        }

        tile_cache->tiles = VIPS_ARRAY( NULL, n_levels, GSList * );
        tile_cache->visible = VIPS_ARRAY( NULL, n_levels, GSList * );
        tile_cache->free = VIPS_ARRAY( NULL, n_levels, GSList * );

#ifdef DEBUG
        printf( "        %d pyr levels\n", n_levels );
        for( i = 0; i < n_levels; i++ ) 
                printf( "        %d) %d x %d\n", 
                        i, 
                        tile_cache->levels[i]->Xsize,
                        tile_cache->levels[i]->Ysize );
#endif /*DEBUG*/
}

/* Expand a rect out to the set of tiles it touches on this level.
 */
static void
tile_cache_tiles_for_rect( TileCache *tile_cache, VipsRect *rect, int z,
	VipsRect *touches )
{
        int size0 = TILE_SIZE << z;
        int left = VIPS_ROUND_DOWN( rect->left, size0 );
        int top = VIPS_ROUND_DOWN( rect->top, size0 );
        int right = VIPS_ROUND_UP( VIPS_RECT_RIGHT( rect ), size0 );
        int bottom = VIPS_ROUND_UP( VIPS_RECT_BOTTOM( rect ), size0 );

        touches->left = left;
        touches->top = top;
        touches->width = right - left;
        touches->height = bottom - top;

        /* We can have rects outside the image. Make sure they stay empty.
         */
        if( vips_rect_isempty( rect ) ) {
                touches->width = 0;
                touches->height = 0;
        }
}

/* Find the first visible tile in a hole.
 */
static void
tile_cache_fill_hole( TileCache *tile_cache, VipsRect *bounds, int z )
{
        int i;

        for( i = z; i < tile_cache->n_levels; i++ ) {
                GSList *p;

                for( p = tile_cache->tiles[i]; p; p = p->next ) {
                        Tile *tile = TILE( p->data );

			/* Ignore tiles with no current or previous pixels.
			 */
			if( !tile->valid &&
				!tile->texture )
				continue;

                        if( vips_rect_overlapsrect( &tile->bounds, bounds ) ) {
				tile_touch( tile );
                                tile_cache->visible[z] = 
                                        g_slist_prepend( tile_cache->visible[z],
                                                tile );
                                return;
                        }
                }
        }
}

static int
tile_cache_sort_lru( const void *a, const void *b )
{
        Tile *t1 = TILE( a );
        Tile *t2 = TILE( b );

        return( t1->time - t2->time );
}

static void
tile_cache_free_oldest( TileCache *tile_cache, int z )
{
        int n_free = g_slist_length( tile_cache->free[z] );
        int n_to_free = VIPS_MAX( 0, n_free - MAX_TILES );

        if( n_to_free > 0 ) {
                int i;

                tile_cache->free[z] = g_slist_sort( tile_cache->free[z], 
                        tile_cache_sort_lru );

                for( i = 0; i < n_to_free; i++ ) {
                        Tile *tile = TILE( tile_cache->free[z]->data );

                        g_assert( g_slist_find( tile_cache->tiles[z], tile ) );

                        tile_cache->tiles[z] = 
                                g_slist_remove( tile_cache->tiles[z], tile );
                        tile_cache->visible[z] = 
                                g_slist_remove( tile_cache->visible[z], tile );
                        tile_cache->free[z] = 
                                g_slist_remove( tile_cache->free[z], tile );
                        VIPS_UNREF( tile );
                }
        }
}

#ifdef DEBUG_VERBOSE
static void
tile_cache_print( TileCache *tile_cache )
{
	int i;

        for( i = 0; i < tile_cache->n_levels; i++ ) {
                printf( "  level %d, %d tiles, %d visible, %d free\n",
                        i, 
                        g_slist_length( tile_cache->tiles[i] ),
                        g_slist_length( tile_cache->visible[i] ),
                        g_slist_length( tile_cache->free[i] ) );
        }

        for( i = 0; i < tile_cache->n_levels; i++ ) {
		GSList *p;

                printf( "  level %d tiles:\n", i ); 
                for( p = tile_cache->tiles[i]; p; p = p -> next ) {
                        Tile *tile = TILE( p->data );
			int visible = g_slist_index( tile_cache->visible[i], 
				tile ) >= 0;

                        printf( "    @ %d x %d, %d x %d, "
				"valid = %d, visible = %d, "
				"texture = %p\n",
                                tile->bounds.left,
                                tile->bounds.top,
                                tile->bounds.width,
                                tile->bounds.height,
                                tile->valid,
			     	visible,
			     	tile->texture );
                }
        }
}
#endif /*DEBUG_VERBOSE*/

static void
tile_cache_compute_visibility( TileCache *tile_cache, 
	VipsRect *viewport, int z )
{
        int size0 = TILE_SIZE << z;
        int start_time = tile_get_time();

        int i;
        VipsRect touches;
        int x, y;
        VipsRect bounds;
        GSList *p;

#ifdef DEBUG_VERBOSE
        printf( "tile_cache_compute_visibility:\n" ); 
#endif /*DEBUG_VERBOSE*/

        /* We're rebuilding these.
         */
        for( i = 0; i < tile_cache->n_levels; i++ ) {
                VIPS_FREEF( g_slist_free, tile_cache->visible[i] );
                VIPS_FREEF( g_slist_free, tile_cache->free[i] );
        }

        /* The rect of tiles touched by the viewport.
         */
        tile_cache_tiles_for_rect( tile_cache, viewport, z, &touches );

        /* Search for the highest res tile for every position in the 
         * viewport.
         */
        bounds.width = size0;
        bounds.height = size0;
        for( y = 0; y < touches.height; y += size0 ) 
                for( x = 0; x < touches.width; x += size0 ) {
                        bounds.left = x + touches.left;
                        bounds.top = y + touches.top;

                        tile_cache_fill_hole( tile_cache, &bounds, z );
                }

        /* So any tiles we've not touched must be invisible and therefore 
         * candidates for freeing.
         */
        for( i = 0; i < tile_cache->n_levels; i++ ) {
                for( p = tile_cache->tiles[i]; p; p = p->next ) {
                        Tile *tile = TILE( p->data );

                        if( tile->time < start_time ) 
                                tile_cache->free[i] = 
                                        g_slist_prepend( tile_cache->free[i], 
                                                tile );
                }
        }

        /* Free the oldest few unused tiles in each level. 
         *
         * Never free tiles in the lowest-res few levels. They are useful for 
         * filling in holes and take little memory.
         */
        for( i = 0; i < tile_cache->n_levels - 3; i++ ) 
                tile_cache_free_oldest( tile_cache, i );

#ifdef DEBUG_VERBOSE
	tile_cache_print( tile_cache );
#endif /*DEBUG_VERBOSE*/
}

static Tile *
tile_cache_find( TileCache *tile_cache, VipsRect *tile_rect, int z )
{
        GSList *p;
        Tile *tile;

        for( p = tile_cache->tiles[z]; p; p = p->next ) {
                tile = TILE( p->data );

                if( vips_rect_overlapsrect( &tile->bounds, tile_rect ) ) 
                        return( tile );
        }

        return( NULL );
}

/* Fetch a single tile. If we have this tile already, refresh if there are new
 * pixels available.
 */
static void
tile_cache_get( TileCache *tile_cache, VipsRect *tile_rect, int z )
{
        Tile *tile;

        /* Look for an existing tile, or make a new one.
	 *
	 * FIXME ... this could be a hash. Could other lookups be hashes as
	 * well, if we rescale x/y for changes in z?
         */
        if( !(tile = tile_cache_find( tile_cache, tile_rect, z )) ) {
                if( !(tile = tile_new( tile_cache->levels[z], 
                        tile_rect->left >> z, tile_rect->top >> z, z )) )
                        return;

                tile_cache->tiles[z] = 
                        g_slist_prepend( tile_cache->tiles[z], tile );
	}

	if( !tile->valid ) {
		/* The tile might have no pixels, or might need refreshing
		 * because the bg render has finished with it.
		 */
#ifdef DEBUG_VERBOSE
		printf( "tile_cache_get: fetching left = %d, top = %d, "
			"width = %d, height = %d, z = %d\n", 
			tile_rect->left, tile_rect->top,
			tile_rect->width, tile_rect->height,
			z );
#endif /*DEBUG_VERBOSE*/

                tile_source_fill_tile( tile_cache->tile_source, tile );
	}
}

/* Fetch the tiles in an area.
 *
 * render processes tiles in FIFO order, so we need to add in reverse order
 * of processing. We want repaint to happen in a spiral from the centre out,
 * so we have to add in a spiral from the outside in.
 */
static void
tile_cache_fetch_area( TileCache *tile_cache, VipsRect *viewport, int z )
{
        int size0 = TILE_SIZE << z;

        /* All the tiles rect touches in this pyr level.
         */
        int left = VIPS_ROUND_DOWN( viewport->left, size0 );
        int top = VIPS_ROUND_DOWN( viewport->top, size0 );
        int right = VIPS_ROUND_UP( VIPS_RECT_RIGHT( viewport ), size0 );
        int bottom = VIPS_ROUND_UP( VIPS_RECT_BOTTOM( viewport ), size0 );

        /* Do the four edges, then step in. Loop until the centre is empty.
         */
        for(;;) {
                VipsRect tile_rect;
                int x, y;

                tile_rect.width = size0;
                tile_rect.height = size0;

                if( right - left <= 0 ||
                        bottom - top <= 0 )
                        break;

                /* Top edge.
                 */
                for( x = left; x < right; x += size0 ) {
                        tile_rect.left = x;
                        tile_rect.top = top;
                        tile_cache_get( tile_cache, &tile_rect, z );
                }

                top += size0;
                if( right - left <= 0 ||
                        bottom - top <= 0 )
                        break;

                /* Bottom edge.
                 */
                for( x = left; x < right; x += size0 ) {
                        tile_rect.left = x;
                        tile_rect.top = bottom - size0;
                        tile_cache_get( tile_cache, &tile_rect, z );
                }

                bottom -= size0;
                if( right - left <= 0 ||
                        bottom - top <= 0 )
                        break;

                /* Left edge.
                 */
                for( y = top; y < bottom; y += size0 ) {
                        tile_rect.left = left;
                        tile_rect.top = y;
                        tile_cache_get( tile_cache, &tile_rect, z );
                }

                left += size0;
                if( right - left <= 0 ||
                        bottom - top <= 0 )
                        break;

                /* Right edge.
                 */
                for( y = top; y < bottom; y += size0 ) {
                        tile_rect.left = right - size0;
                        tile_rect.top = y;
                        tile_cache_get( tile_cache, &tile_rect, z );
                }

                right -= size0;
                if( right - left <= 0 ||
                        bottom - top <= 0 )
                        break;
        }
}

/* Eevetrything has changed, eg. page turn and the image geometry has changed.
 */
static void
tile_cache_source_changed( TileSource *tile_source, TileCache *tile_cache )
{
#ifdef DEBUG
        printf( "tile_cache_source_changed:\n" );
#endif /*DEBUG*/

	/* This will junk all tiles.
	 */
        tile_cache_build_pyramid( tile_cache );

        tile_cache_changed( tile_cache );
}

/* All tiles need refetching, perhaps after eg. "falsecolour" etc. Mark 
 * all tiles invalid and reemit.
 */
void
tile_cache_source_tiles_changed( TileSource *tile_source, 
	TileCache *tile_cache )
{
        int i;

#ifdef DEBUG
        printf( "tile_cache_source_tiles_changed:\n" );
#endif /*DEBUG*/

        for( i = 0; i < tile_cache->n_levels; i++ ) {
                GSList *p;

                for( p = tile_cache->tiles[i]; p; p = p->next ) {
                        Tile *tile = TILE( p->data );

			/* We must refetch.
			 */
                        tile->valid = FALSE;
                }
        }

        tile_cache_tiles_changed( tile_cache );
}

/* The bg render thread says some tiles have fresh pixels.
 */
static void
tile_cache_source_area_changed( TileSource *tile_source, 
	VipsRect *dirty, int z, TileCache *tile_cache )
{
#ifdef DEBUG_VERBOSE
        printf( "tile_cache_source_area_changed: left = %d, top = %d, "
                "width = %d, height = %d, z = %d\n", 
                dirty->left, dirty->top,
                dirty->width, dirty->height, z );
#endif /*DEBUG_VERBOSE*/

	/* Immediately fetch the updated tile. If we wait for snapshot, the
	 * animation page may have changed.
	 */
	tile_cache_fetch_area( tile_cache, dirty, z );

        tile_cache_area_changed( tile_cache, dirty, z );
}

TileCache *
tile_cache_new( TileSource *tile_source )
{
        TileCache *tile_cache = g_object_new( TILE_CACHE_TYPE, NULL );

        tile_cache->tile_source = tile_source;
        g_object_ref( tile_source );

	g_signal_connect_object( tile_source, "changed",
		G_CALLBACK( tile_cache_source_changed ), tile_cache, 0 );
	g_signal_connect_object( tile_source, "tiles-changed",
		G_CALLBACK( tile_cache_source_tiles_changed ), tile_cache, 0 );
	g_signal_connect_object( tile_source, "area-changed",
		G_CALLBACK( tile_cache_source_area_changed ), tile_cache, 0 );

        /* Don't build the pyramid yet -- the source probably hasn't loaded.
         * Wait for "changed".
         */

        return( tile_cache ); 
}

/* Scale is how much the level0 image has been scaled, x/y is the position of
 * the top-left corner of the paint_rect area in the scaled image.
 *
 * paint_rect is the pixel area in gtk coordinates that we paint in the widget.
 *
 * Set debug to draw tile boundaries for debugging.
 */
void 
tile_cache_snapshot( TileCache *tile_cache, GtkSnapshot *snapshot, 
	double scale, double x, double y,
	VipsRect *paint_rect,
	gboolean debug )
{
	VipsRect viewport;
	int z;
        int i;

	/* In debug mode, scale and offset so we can see tile clipping. 
	 */
        float debug_scale = 0.9;
        graphene_point_t debug_offset = { 32, 32 };

#ifdef DEBUG_RENDER_TIME
	GTimer *snapshot_timer = g_timer_new();
#endif /*DEBUG_RENDER_TIME*/

	if( debug ) {
		gtk_snapshot_translate( snapshot, &debug_offset );
		gtk_snapshot_scale( snapshot, debug_scale, debug_scale );
	}

#ifdef DEBUG
        printf( "tile_cache_snapshot: scale = %g, x = %g, y = %g\n", 
		scale, x, y );
#endif /*DEBUG*/

#ifdef DEBUG_VERBOSE
        printf( "  paint_rect left = %d, top = %d, "
                "width = %d, height = %d\n", 
                paint_rect->left, paint_rect->top,
                paint_rect->width, paint_rect->height );
#endif /*DEBUG_VERBOSE*/

	/* Pick a pyramid layer. For enlarging, we leave the z at 0 
	 * (the highest res layer).
	 */
	if( scale > 1.0 || 
		scale == 0 ) 
		z = 0;
	else 
		z = VIPS_CLIP( 0, 
			log( 1.0 / scale ) / log( 2.0 ), 
			tile_cache->n_levels - 1 );

	/* paint_rect in level0 coordinates.
	 */
	viewport.left = x / scale;
	viewport.top = y / scale;
	viewport.width = VIPS_MAX( 1, paint_rect->width / scale );
	viewport.height = VIPS_MAX( 1, paint_rect->height / scale );

	/* Fetch any tiles we are missing, update any tiles we have that have
	 * been flagged as having pixels ready for fetching.
	 */
	tile_cache_fetch_area( tile_cache, &viewport, z );

	/* Find the set of visible tiles, sorted back to front.
	 *
	 * FIXME ... we could often skip this, esp when panning, unless we
	 * cross a tile boundary.
	 */
	tile_cache_compute_visibility( tile_cache, &viewport, z );

        /* If there's an alpha, we'll need a backdrop.
         */
        if( vips_image_hasalpha( tile_cache->tile_source->image ) ) {
                graphene_rect_t bounds;

#ifdef DEBUG_VERBOSE
                printf( "tile_cache_snapshot: drawing checkerboard\n" );
#endif /*DEBUG_VERBOSE*/

                bounds.origin.x = paint_rect->left;
                bounds.origin.y = paint_rect->top;
                bounds.size.width = paint_rect->width;
                bounds.size.height = paint_rect->height;
                gtk_snapshot_push_repeat( snapshot, &bounds, NULL );

                bounds.origin.x = 0;
                bounds.origin.y = 0;
                bounds.size.width = TILE_SIZE;
                bounds.size.height = TILE_SIZE;
                gtk_snapshot_append_texture( snapshot, 
			tile_cache->checkerboard, &bounds );

                gtk_snapshot_pop( snapshot );
        }

	/* Draw all visible tiles, back to front.
	 */
        for( i = tile_cache->n_levels - 1; i >= z; i-- ) { 
                GSList *p;

                for( p = tile_cache->visible[i]; p; p = p->next ) {
                        Tile *tile = TILE( p->data );

                        graphene_rect_t bounds;

                        bounds.origin.x = tile->bounds.left * scale - 
				x + paint_rect->left;
                        bounds.origin.y = tile->bounds.top * scale - 
				y + paint_rect->top;  
                        bounds.size.width = tile->bounds.width * scale + 0.5;  
                        bounds.size.height = tile->bounds.height * scale + 0.5;

			gtk_snapshot_append_texture( snapshot,
				 tile_get_texture( tile ), &bounds );

			/* In debug mode, draw the edges and add text for the 
			 * tile pointer and age.
			 */
			if( debug ) {
				GskRoundedRect outline = GSK_ROUNDED_RECT_INIT( 
					bounds.origin.x,
					bounds.origin.y,
					bounds.size.width,
					bounds.size.height );
				float border_width[4] = { 2, 2, 2, 2 };
				GdkRGBA border_colour[4] = { 
					{ 0, 1, 0, 1 },
					{ 0, 1, 0, 1 },
					{ 0, 1, 0, 1 },
					{ 0, 1, 0, 1 },
				};

				gtk_snapshot_append_border( snapshot, 
					&outline, 
					border_width, border_colour );

				/* If we are drawing a low-res tile at the
				 * back of the stack, it can get extremly
				 * large with big images. Cairo hates large
				 * surfaces, so skip the text annotation in
				 * this case.
				 */
				if( bounds.size.width < 32000 &&
					bounds.size.height < 32000 ) {
					cairo_t *cr;
					char str[256];
					VipsBuf buf = VIPS_BUF_STATIC( str );

					cr = gtk_snapshot_append_cairo( 
						snapshot, &bounds );

					cairo_set_source_rgb( cr, 0, 1, 0 );
					cairo_set_font_size( cr, 12 );

					cairo_move_to( cr,
						bounds.origin.x + 
							0.1 * 
							bounds.size.width,
						bounds.origin.y + 
							0.1 * 
							bounds.size.height );
					vips_buf_appendf( &buf, "%p", tile );
					cairo_show_text( cr,
						vips_buf_all( &buf ) );

					cairo_move_to( cr, 
						bounds.origin.x + 
							0.1 * 
							bounds.size.width,
						bounds.origin.y + 
							0.2 * 
							bounds.size.height );
					vips_buf_rewind( &buf );
					vips_buf_appendf( &buf, "%d", 
						tile->time );
					cairo_show_text( cr,
						vips_buf_all( &buf ) );

					cairo_destroy( cr );
				}
			}
                }
        }

	/* Draw a box for the viewport.
	 */
	if( debug ) {
		#define BORDER ((GdkRGBA) { 1, 0, 0, 1 })

		GskRoundedRect outline;

		gsk_rounded_rect_init_from_rect( &outline, 
			&GRAPHENE_RECT_INIT(
				viewport.left * scale - x + paint_rect->left,
				viewport.top * scale - y + paint_rect->top,
				viewport.width * scale,
				viewport.height * scale
			), 
			0 );

		gtk_snapshot_append_border( snapshot, 
			&outline, 
			(float[4]) { 2, 2, 2, 2 },
			(GdkRGBA [4]) { BORDER, BORDER, BORDER, BORDER } );
	}

#ifdef DEBUG_RENDER_TIME
        printf( "tile_cache_snapshot: %g ms\n", 
		g_timer_elapsed( snapshot_timer, NULL ) * 1000 );
        g_timer_destroy( snapshot_timer );
#endif /*DEBUG_RENDER_TIME*/
}
