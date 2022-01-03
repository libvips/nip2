/* A tile fetched from libvips and painted with gtk.
 */

#ifndef __TILE_H
#define __TILE_H

#define TYPE_TILE (tile_get_type())
#define TILE( obj ) \
        (G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_TILE, Tile ))
#define TILE_CLASS( klass ) \
        (G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_TILE, TileClass))
#define IS_TILE( obj ) \
        (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_TILE ))
#define IS_TILE_CLASS( klass ) \
        (G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_TILE ))
#define TILE_GET_CLASS( obj ) \
        (G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_TILE, TileClass ))

typedef struct _Tile {
        GObject parent_instance;

        /* Time we last used the tile, for cache flushing.
         */
        guint time;

        /* RGB or RGBA pixels coming in from libvips. A memory region, with 
         * data copied in from the end of the pipeline.
         */
        VipsRegion *region;

	/* The z layer the tile sits at.
	 */
	int z;

	/* The tile rect, in level 0 coordinates. region->valid is the rect in
	 * level z coordinates.
	 */
	VipsRect bounds;

	/* TRUE if the region contains real pixels from the image. FALSE if
	 * eg. we're waiting for computation.
	 */
	gboolean valid;

        /* Pixels going out to the scene graph. 
	 *
	 * pixbuf and texture won't make a copy of the data, so we must make a 
	 * copy ourselves, in case a later fetch from the same region produces
	 * invalid data.
         */
        VipsPel *data_copy;
	GdkPixbuf *pixbuf;
        GdkTexture *texture;

} Tile;

typedef struct _TileClass {
        GObjectClass parent_class;

} TileClass;

GType tile_get_type( void );

/* Get the current time.
 */
int tile_get_time( void );

/* Update the time on a tile.
 */
void tile_touch( Tile *tile );

/* Make a new tile on the level.
 */
Tile *tile_new( VipsImage *level, int x, int y, int z );

/* texture lifetime run by tile ... don't unref.
 */
GdkTexture *tile_get_texture( Tile *tile );

/* Free the texture to force regeneration on next use. Call this if the region
 * changes.
 */
void tile_free_texture( Tile *tile );

#endif /*__TILE_H*/
