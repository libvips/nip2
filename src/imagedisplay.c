#include "vipsdisp.h"

/*
#define DEBUG_VERBOSE
#define DEBUG
 */

struct _Imagedisplay {
        GtkDrawingArea parent_instance;

        /* The tilecache whose output we display.
         */
        TileCache *tile_cache;

        /* We implement a scrollable interface.
         */
        GtkAdjustment *hadj;
        GtkAdjustment *vadj;
        guint hscroll_policy;
        guint vscroll_policy;

        /* image_rect is the bounds of image space .. 0,0 to image->Xsize,
         * image->Ysize
         */
        VipsRect image_rect;

        /* The rect of the widget.
         */
        VipsRect widget_rect;

        /* The sub-area of widget_rect that we paint. Very zoomed out images
         * are centred in the widget.
         */
        VipsRect paint_rect;

        /* How we transform the image_rect to widget space. 
         *
         * scale is how much we zoom/reduce the image by. 
         * x, y is the position of the top-left of the widget in the scaled
         * image.
         */
        double scale;
        double x, y;

	/* Draw the screen in debug mode.
	 */
	gboolean debug;

	/* _layout will pick a scale to fit the image to the window.
	 */
	gboolean bestfit;
};

/* imagedisplay is actually a drawing area the size of the widget on screen: we 
 * do all scrolling ourselves.
 */
G_DEFINE_TYPE_WITH_CODE( Imagedisplay, imagedisplay, GTK_TYPE_DRAWING_AREA,
        G_IMPLEMENT_INTERFACE( GTK_TYPE_SCROLLABLE, NULL ) );

enum {
        /* Set the tile_cache we display.
         */
        PROP_TILE_CACHE = 1,

        /* The props we implement for the scrollable interface.
         */
        PROP_HADJUSTMENT,
        PROP_HSCROLL_POLICY,
        PROP_VADJUSTMENT,
        PROP_VSCROLL_POLICY,

        /* Control transform with this.
         */
        PROP_SCALE,
        PROP_X,
        PROP_Y,

        /* Draw snapshot in debug mode.
         */
        PROP_DEBUG,

        SIG_LAST
};

static void
imagedisplay_dispose( GObject *object )
{
        Imagedisplay *imagedisplay = (Imagedisplay *) object;

#ifdef DEBUG
        printf( "imagedisplay_dispose:\n" ); 
#endif /*DEBUG*/

        VIPS_UNREF( imagedisplay->tile_cache );

        G_OBJECT_CLASS( imagedisplay_parent_class )->dispose( object );
}

static void
imagedisplay_set_transform( Imagedisplay *imagedisplay, 
        double scale, double x, double y )
{
        /* Sanity limits.
         */
        if( scale > 100000 || 
                scale < (1.0 / 100000) )
                return;
        if( x < -1000 ||
                x > 2 * VIPS_MAX_COORD ||
                y < -1000 ||
                y > 2 * VIPS_MAX_COORD )
                return;

#ifdef DEBUG
	printf( "imagedisplay_set_transform: "
		"x = %g, y = %g, scale = %g\n", x, y, scale );
#endif /*DEBUG*/

        imagedisplay->scale = scale;
        imagedisplay->x = x;
        imagedisplay->y = y;
}

static void
imagedisplay_adjustment_changed( GtkAdjustment *adjustment, 
        Imagedisplay *imagedisplay )
{
        if( gtk_widget_get_realized( GTK_WIDGET( imagedisplay ) ) ) {
                double left = gtk_adjustment_get_value( imagedisplay->hadj );
                double top = gtk_adjustment_get_value( imagedisplay->vadj );

#ifdef DEBUG
                printf( "imagedisplay_adjustment_changed: %g x %g\n", 
                        left, top );
#endif /*DEBUG*/

                imagedisplay_set_transform( imagedisplay, 
                        imagedisplay->scale, left, top );
		gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
        }
}

static gboolean
imagedisplay_set_adjustment( Imagedisplay *imagedisplay,
        GtkAdjustment **adjustment_slot, GtkAdjustment *new_adjustment )
{
#ifdef DEBUG
        printf( "imagedisplay_set_adjustment:\n" ); 
#endif /*DEBUG*/

        if( new_adjustment && 
                *adjustment_slot == new_adjustment )
                return( FALSE );

        if( *adjustment_slot ) {
                g_signal_handlers_disconnect_by_func( *adjustment_slot,
                            imagedisplay_adjustment_changed, imagedisplay );
                VIPS_UNREF( *adjustment_slot );
        }

        if( !new_adjustment )
                new_adjustment = 
                        gtk_adjustment_new( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );

        g_signal_connect( new_adjustment, "value-changed",
                G_CALLBACK( imagedisplay_adjustment_changed ), imagedisplay );
        *adjustment_slot = g_object_ref_sink( new_adjustment );

        return( TRUE );
}

static void
imagedisplay_set_adjustment_values( Imagedisplay *imagedisplay, 
        GtkAdjustment *adjustment, int axis_size, int window_size ) 
{
        double old_value;
        double new_value;
        double new_upper;
        double page_size;

#ifdef DEBUG
        printf( "imagedisplay_set_adjustment_values: axis_size = %d, "
                "window_size = %d\n", axis_size, window_size );
#endif /*DEBUG*/

        old_value = gtk_adjustment_get_value( adjustment );
        page_size = window_size;
        new_upper = VIPS_MAX( axis_size, page_size );

        g_object_set( adjustment,
                "lower", 0.0,
                "upper", new_upper,
                "page-size", page_size,
                "step-increment", 10.0,
                "page-increment", page_size * 0.9,
                NULL );

        new_value = VIPS_CLIP( 0, old_value, new_upper - page_size );
        if( new_value != old_value )
                gtk_adjustment_set_value( adjustment, new_value );
}

static void
imagedisplay_set_hadjustment_values( Imagedisplay *imagedisplay ) 
{
        imagedisplay_set_adjustment_values( imagedisplay, 
                imagedisplay->hadj, 
                imagedisplay->image_rect.width * imagedisplay->scale, 
                imagedisplay->paint_rect.width );
}

static void
imagedisplay_set_vadjustment_values( Imagedisplay *imagedisplay ) 
{
        imagedisplay_set_adjustment_values( imagedisplay, 
                imagedisplay->vadj, 
                imagedisplay->image_rect.height * imagedisplay->scale, 
                imagedisplay->paint_rect.height );
}

static void
imagedisplay_layout( Imagedisplay *imagedisplay )
{
#ifdef DEBUG
        printf( "imagedisplay_layout:\n" ); 
#endif /*DEBUG*/

        imagedisplay->widget_rect.width = 
                gtk_widget_get_width( GTK_WIDGET( imagedisplay ) );
        imagedisplay->widget_rect.height = 
                gtk_widget_get_height( GTK_WIDGET( imagedisplay ) );

	/* width and height will be 0 if _layout runs too early to be useful.
	 */
	if( !imagedisplay->widget_rect.width ||
		!imagedisplay->widget_rect.height )
		return;

	/* If there's no image yet, we can't do anything.
	 */
	if( !imagedisplay->tile_cache )
		return;

	/* Do this the first time we have the image.
	 */
	if( imagedisplay->bestfit ) {
		double hscale = (double) imagedisplay->widget_rect.width / 
			imagedisplay->image_rect.width;
		double vscale = (double) imagedisplay->widget_rect.height / 
			imagedisplay->image_rect.height;

                imagedisplay_set_transform( imagedisplay, 
			VIPS_MIN( hscale, vscale ),
                        imagedisplay->x, 
                        imagedisplay->y ); 

#ifdef DEBUG
		printf( "imagedisplay_layout: bestfit sets scale = %g\n",
			imagedisplay->scale );
#endif /*DEBUG*/

		imagedisplay->bestfit = FALSE;
	}

        imagedisplay->paint_rect.width = VIPS_MIN( 
                imagedisplay->widget_rect.width, 
                imagedisplay->image_rect.width * imagedisplay->scale );
        imagedisplay->paint_rect.height = VIPS_MIN( 
                imagedisplay->widget_rect.height, 
                imagedisplay->image_rect.height * imagedisplay->scale );

        /* If we've zoomed right out, centre the image in the window.
         */
        imagedisplay->paint_rect.left = VIPS_MAX( 0,
                (imagedisplay->widget_rect.width - 
                 imagedisplay->paint_rect.width) / 2 ); 
        imagedisplay->paint_rect.top = VIPS_MAX( 0,
                (imagedisplay->widget_rect.height - 
                 imagedisplay->paint_rect.height) / 2 ); 

        imagedisplay_set_hadjustment_values( imagedisplay );
        imagedisplay_set_vadjustment_values( imagedisplay );
}

/* Large change, we need to relayout.
 */
static void
imagedisplay_tile_cache_changed( TileCache *tile_cache, 
        Imagedisplay *imagedisplay ) 
{
#ifdef DEBUG
        printf( "imagedisplay_tile_cache_changed:\n" ); 
#endif /*DEBUG*/

	/* Always shrink-to-fit new image sources.
	 */
	imagedisplay->bestfit = TRUE;

        imagedisplay->image_rect.width = 
                tile_cache->tile_source->display_width;
        imagedisplay->image_rect.height = 
                tile_cache->tile_source->display_height;

        imagedisplay_layout( imagedisplay );

	gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
}

/* Tiles have changed, but not image geometry. Perhaps falsecolour.
 */
static void
imagedisplay_tile_cache_tiles_changed( TileCache *tile_cache, 
        Imagedisplay *imagedisplay ) 
{
#ifdef DEBUG
        printf( "imagedisplay_tile_cache_tiles_changed:\n" ); 
#endif /*DEBUG*/

        gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
}

static void
imagedisplay_tile_cache_area_changed( TileCache *tile_cache, 
	VipsRect *dirty, int z, Imagedisplay *imagedisplay ) 
{
#ifdef DEBUG_VERBOSE
        printf( "imagedisplay_tile_cache_area_changed: "
                "at %d x %d, size %d x %d, z = %d\n",
                dirty->left, dirty->top,
                dirty->width, dirty->height,
                z );
#endif /*DEBUG_VERBOSE*/

        /* Sadly, gtk4 only has this and we can't redraw areas. Perhaps we
         * could just renegerate part of the snapshot?
         */
        gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) );
}

static void
imagedisplay_set_tile_cache( Imagedisplay *imagedisplay, 
        TileCache *tile_cache )
{
        VIPS_UNREF( imagedisplay->tile_cache );
        imagedisplay->tile_cache = tile_cache;
        g_object_ref( imagedisplay->tile_cache );

        g_signal_connect_object( tile_cache, "changed", 
                G_CALLBACK( imagedisplay_tile_cache_changed ), 
                imagedisplay, 0 );
        g_signal_connect_object( tile_cache, "tiles-changed", 
                G_CALLBACK( imagedisplay_tile_cache_tiles_changed ), 
                imagedisplay, 0 );
        g_signal_connect_object( tile_cache, "area-changed", 
                G_CALLBACK( imagedisplay_tile_cache_area_changed ), 
                imagedisplay, 0 );

	/* Do initial change to init.
	 */
	imagedisplay_tile_cache_changed( tile_cache, imagedisplay );
}

static void
imagedisplay_set_property( GObject *object, 
        guint prop_id, const GValue *value, GParamSpec *pspec )
{
        Imagedisplay *imagedisplay = (Imagedisplay *) object;

        switch( prop_id ) {
        case PROP_HADJUSTMENT:
                if( imagedisplay_set_adjustment( imagedisplay, 
                        &imagedisplay->hadj, 
                        g_value_get_object( value ) ) ) { 
                        imagedisplay_set_hadjustment_values( imagedisplay );
                        g_object_notify( G_OBJECT( imagedisplay ), 
                                "hadjustment" );
                }
                break;

        case PROP_VADJUSTMENT:
                if( imagedisplay_set_adjustment( imagedisplay, 
                        &imagedisplay->vadj, 
                        g_value_get_object( value ) ) ) { 
                        imagedisplay_set_vadjustment_values( imagedisplay );
                        g_object_notify( G_OBJECT( imagedisplay ), 
                                "vadjustment" );
                }
                break;

        case PROP_HSCROLL_POLICY:
                if( imagedisplay->hscroll_policy != 
                        g_value_get_enum( value ) ) {
                        imagedisplay->hscroll_policy = 
                                g_value_get_enum( value );
                        gtk_widget_queue_resize( GTK_WIDGET( imagedisplay ) );
                        g_object_notify_by_pspec( object, pspec );
                }
                break;

        case PROP_VSCROLL_POLICY:
                if( imagedisplay->vscroll_policy != 
                        g_value_get_enum( value ) ) {
                        imagedisplay->vscroll_policy = 
                                g_value_get_enum( value );
                        gtk_widget_queue_resize( GTK_WIDGET( imagedisplay ) );
                        g_object_notify_by_pspec( object, pspec );
                }
                break;

        case PROP_TILE_CACHE:
                imagedisplay_set_tile_cache( imagedisplay, 
                        g_value_get_object( value ) );
                break;

        case PROP_SCALE:
                imagedisplay_set_transform( imagedisplay, 
			g_value_get_double( value ),
                        imagedisplay->x, 
                        imagedisplay->y ); 
                imagedisplay_layout( imagedisplay );
		gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
                break;

        case PROP_X:
                imagedisplay_set_transform( imagedisplay, 
                        imagedisplay->scale,
                        g_value_get_double( value ),
                        imagedisplay->y );
		gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
                break;

        case PROP_Y:
                imagedisplay_set_transform( imagedisplay, 
                        imagedisplay->scale,
                        imagedisplay->x, 
                        g_value_get_double( value ) );
		gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
                break;

        case PROP_DEBUG:
                imagedisplay->debug = g_value_get_boolean( value );
		gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID( object, prop_id, pspec );
                break;
        }
}

static void
imagedisplay_get_property( GObject *object, 
        guint prop_id, GValue *value, GParamSpec *pspec )
{
        Imagedisplay *imagedisplay = (Imagedisplay *) object;

        switch( prop_id ) {
        case PROP_HADJUSTMENT:
                g_value_set_object( value, imagedisplay->hadj );
                break;

        case PROP_VADJUSTMENT:
                g_value_set_object( value, imagedisplay->vadj );
                break;

        case PROP_HSCROLL_POLICY:
                g_value_set_enum( value, imagedisplay->hscroll_policy );
                break;

        case PROP_VSCROLL_POLICY:
                g_value_set_enum( value, imagedisplay->vscroll_policy );
                break;

        case PROP_TILE_CACHE:
                g_value_set_object( value, imagedisplay->tile_cache );
                break;

        case PROP_SCALE:
                g_value_set_double( value, imagedisplay->scale );
                break;

        case PROP_X:
                g_value_set_double( value, imagedisplay->x );
                break;

        case PROP_Y:
                g_value_set_double( value, imagedisplay->y );
                break;

        case PROP_DEBUG:
                g_value_set_boolean( value, imagedisplay->debug );
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID( object, prop_id, pspec );
                break;
        }
}

static void
imagedisplay_snapshot( GtkWidget *widget, GtkSnapshot *snapshot )
{
        Imagedisplay *imagedisplay = VIPSDISP_IMAGEDISPLAY( widget );

#ifdef DEBUG
        printf( "imagedisplay_snapshot:\n" );
#endif /*DEBUG*/

        /* Clip to the widget area, or we may paint over the display control
         * bar.
         */
        gtk_snapshot_push_clip( snapshot, &GRAPHENE_RECT_INIT(
                0, 
                0, 
                gtk_widget_get_width( widget ),
                gtk_widget_get_height( widget ) )); 

	if( imagedisplay->tile_cache &&
		imagedisplay->tile_cache->tiles )
		tile_cache_snapshot( imagedisplay->tile_cache, snapshot, 
			imagedisplay->scale, imagedisplay->x, imagedisplay->y,
			&imagedisplay->paint_rect,
		        imagedisplay->debug );

        gtk_snapshot_pop( snapshot );

         /* I wasn't able to get gtk_snapshot_render_focus() working. Draw
          * the focus rect ourselves.
          */
        if( gtk_widget_has_focus( widget ) ) {
                #define BORDER ((GdkRGBA) { 0.4, 0.4, 0.6, 1 })

                GskRoundedRect outline;

                gsk_rounded_rect_init_from_rect( &outline, 
                        &GRAPHENE_RECT_INIT(
                                3, 
                                3, 
                                gtk_widget_get_width( widget ) - 6,
                                gtk_widget_get_height( widget ) - 6
                        ), 
                        5 );

                gtk_snapshot_append_border( snapshot, 
                        &outline, 
                        (float[4]) { 2, 2, 2, 2 },
                        (GdkRGBA [4]) { BORDER, BORDER, BORDER, BORDER } );
        }
}

static void
imagedisplay_resize( GtkWidget *widget, int width, int height )
{
        Imagedisplay *imagedisplay = (Imagedisplay *) widget;

#ifdef DEBUG
        printf( "imagedisplay_resize: %d x %d\n", width, height ); 
#endif /*DEBUG*/

        imagedisplay_layout( imagedisplay );

	gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
}

static void
imagedisplay_focus_enter( GtkEventController *controller, 
        Imagedisplay *imagedisplay )
{
#ifdef DEBUG
        printf( "imagedisplay_focus_enter:\n" );
#endif /*DEBUG*/

        gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
}

static void
imagedisplay_focus_leave( GtkEventController *controller, 
        Imagedisplay *imagedisplay )
{
#ifdef DEBUG
        printf( "imagedisplay_focus_leave:\n" );
#endif /*DEBUG*/

        gtk_widget_queue_draw( GTK_WIDGET( imagedisplay ) ); 
}

static void 
imagedisplay_click( GtkEventController *controller, 
        int n_press, double x, double y, Imagedisplay *imagedisplay )
{
        gtk_widget_grab_focus( GTK_WIDGET( imagedisplay ) );
}

static void
imagedisplay_init( Imagedisplay *imagedisplay )
{
        GtkEventController *controller;

#ifdef DEBUG
        printf( "imagedisplay_init:\n" ); 
#endif /*DEBUG*/

        imagedisplay->scale = 1;

        gtk_widget_set_focusable( GTK_WIDGET( imagedisplay ), TRUE );

        g_signal_connect( GTK_DRAWING_AREA( imagedisplay ), "resize",
                G_CALLBACK( imagedisplay_resize ), NULL);

        controller = gtk_event_controller_focus_new();
        g_signal_connect( controller, "enter", 
                G_CALLBACK( imagedisplay_focus_enter ), imagedisplay );
        g_signal_connect( controller, "leave", 
                G_CALLBACK( imagedisplay_focus_leave ), imagedisplay );
        gtk_widget_add_controller( GTK_WIDGET( imagedisplay ), controller );

        controller = GTK_EVENT_CONTROLLER( gtk_gesture_click_new() );
        g_signal_connect( controller, "pressed", 
                G_CALLBACK( imagedisplay_click ), imagedisplay );
        gtk_widget_add_controller( GTK_WIDGET( imagedisplay ), controller );
}

static void
imagedisplay_class_init( ImagedisplayClass *class )
{
        GObjectClass *gobject_class = G_OBJECT_CLASS( class );
        GtkWidgetClass *widget_class = GTK_WIDGET_CLASS( class );

#ifdef DEBUG
        printf( "imagedisplay_class_init:\n" ); 
#endif /*DEBUG*/

        gobject_class->dispose = imagedisplay_dispose;
        gobject_class->set_property = imagedisplay_set_property;
        gobject_class->get_property = imagedisplay_get_property;

        widget_class->snapshot = imagedisplay_snapshot;

        g_object_class_install_property( gobject_class, PROP_TILE_CACHE,
                g_param_spec_object( "tile-cache",
                        _( "Tile cache" ),
                        _( "The tile cache to be displayed" ),
                        TILE_CACHE_TYPE,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_SCALE,
                g_param_spec_double( "scale",
                        _( "Scale" ),
                        _( "Scale of viewport" ),
                        -VIPS_MAX_COORD, VIPS_MAX_COORD, 0,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_X,
                g_param_spec_double( "x",
                        _( "x" ),
                        _( "Horizontal position of viewport" ),
                        -VIPS_MAX_COORD, VIPS_MAX_COORD, 0,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_Y,
                g_param_spec_double( "y",
                        _( "y" ),
                        _( "Vertical position of viewport" ),
                        -VIPS_MAX_COORD, VIPS_MAX_COORD, 0,
                        G_PARAM_READWRITE ) );

        g_object_class_install_property( gobject_class, PROP_DEBUG,
                g_param_spec_boolean( "debug",
                        _( "Debug" ),
                        _( "Render snapshot in debug mode" ),
			FALSE,
                        G_PARAM_READWRITE ) );

        g_object_class_override_property( gobject_class, 
                PROP_HADJUSTMENT, "hadjustment" );
        g_object_class_override_property( gobject_class, 
                PROP_VADJUSTMENT, "vadjustment" );
        g_object_class_override_property( gobject_class, 
                PROP_HSCROLL_POLICY, "hscroll-policy" );
        g_object_class_override_property( gobject_class, 
                PROP_VSCROLL_POLICY, "vscroll-policy" );
}

Imagedisplay *
imagedisplay_new( TileCache *tile_cache ) 
{
        Imagedisplay *imagedisplay;

#ifdef DEBUG
        printf( "imagedisplay_new:\n" ); 
#endif /*DEBUG*/

        imagedisplay = g_object_new( imagedisplay_get_type(),
                "tile-cache", tile_cache,
                NULL );

        return( imagedisplay ); 
}

/* image        level0 image coordinates ... this is the coordinate space we
 *              pass down to tile_cache
 *
 * gtk          screen cods, so the coordinates we use to render tiles
 */

void
imagedisplay_image_to_gtk( Imagedisplay *imagedisplay, 
        double x_image, double y_image, double *x_gtk, double *y_gtk )
{
        *x_gtk = x_image * imagedisplay->scale - 
                imagedisplay->x + 
                imagedisplay->paint_rect.left;
        *y_gtk = y_image * imagedisplay->scale - 
                imagedisplay->y + 
                imagedisplay->paint_rect.top;
}

void
imagedisplay_gtk_to_image( Imagedisplay *imagedisplay, 
        double x_gtk, double y_gtk, double *x_image, double *y_image )
{
        *x_image = (imagedisplay->x + 
                x_gtk - 
                imagedisplay->paint_rect.left) / imagedisplay->scale;
        *y_image = (imagedisplay->y + 
                y_gtk - 
                imagedisplay->paint_rect.top) / imagedisplay->scale;

        *x_image = VIPS_CLIP( 0, *x_image, 
                imagedisplay->image_rect.width - 1 );
        *y_image = VIPS_CLIP( 0, *y_image, 
                imagedisplay->image_rect.height - 1 );
}
