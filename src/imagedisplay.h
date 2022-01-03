#ifndef __IMAGE_DISPLAY_H
#define __IMAGE_DISPLAY_H

#define IMAGEDISPLAY_TYPE (imagedisplay_get_type())

G_DECLARE_FINAL_TYPE( Imagedisplay, imagedisplay, 
	VIPSDISP, IMAGEDISPLAY, GtkDrawingArea )

void imagedisplay_image_to_gtk( Imagedisplay *imagedisplay, 
	double x_image, double y_image, double *x_gtk, double *y_gtk );
void imagedisplay_gtk_to_image( Imagedisplay *imagedisplay, 
	double x_gtk, double y_gtk, double *x_image, double *y_image );

Imagedisplay *imagedisplay_new( TileCache *tile_cache ); 

#endif /* __IMAGE_DISPLAY_H */
