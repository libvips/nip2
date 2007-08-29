/* gtkplot - 2d scientific plots widget for gtk+
 * Copyright 1999-2001  Adrian E. Feiguin <feiguin@ifir.edu.ar>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GTK_PLOT3D_H__
#define __GTK_PLOT3D_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gtkplot.h"


#define GTK_PLOT3D(obj)        GTK_CHECK_CAST (obj, gtk_plot3d_get_type (), GtkPlot3D)
#define GTK_TYPE_PLOT3D        (gtk_plot3d_get_type ())
#define GTK_PLOT3D_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot3d_get_type(), GtkPlot3DClass)
#define GTK_IS_PLOT3D(obj)     GTK_CHECK_TYPE (obj, gtk_plot3d_get_type ())
#define GTK_PLOT3D_FLAGS(plot)         (GTK_PLOT3D(plot)->flags)
#define GTK_PLOT3D_SET_FLAGS(plot,flag) (GTK_PLOT3D_FLAGS(plot) |= (flag))
#define GTK_PLOT3D_UNSET_FLAGS(plot,flag) (GTK_PLOT3D_FLAGS(plot) &= ~(flag))

#define GTK_PLOT3D_TRANSPARENT(plot) (GTK_PLOT3D_FLAGS(plot) & GTK_PLOT3D_TRANSPARENT)

typedef struct _GtkPlot3D		GtkPlot3D;
typedef struct _GtkPlot3DClass		GtkPlot3DClass;

typedef enum
{
  GTK_PLOT_PLANE_XY	= 0,
  GTK_PLOT_PLANE_YX	= 0,
  GTK_PLOT_PLANE_XZ	= 1,
  GTK_PLOT_PLANE_ZX	= 1,
  GTK_PLOT_PLANE_YZ	= 2,
  GTK_PLOT_PLANE_ZY	= 2,
} GtkPlotPlane;

typedef enum
{
  GTK_PLOT_SIDE_XY	= 1 << 0 ,
  GTK_PLOT_SIDE_XZ	= 1 << 1 ,
  GTK_PLOT_SIDE_YX	= 1 << 2 ,
  GTK_PLOT_SIDE_YZ	= 1 << 3 ,
  GTK_PLOT_SIDE_ZX	= 1 << 4 ,
  GTK_PLOT_SIDE_ZY	= 1 << 5 ,
} GtkPlotSide;

struct _GtkPlot3D
{
  GtkPlot plot;

  GtkPlotVector e1, e2, e3;
  GtkPlotVector center;
  GtkPlotVector origin;

  gdouble a1, a2, a3;
  gdouble ncos[360];
  gdouble nsin[360];

  GtkPlotAxis *ax, *ay, *az; 

  gboolean xy_visible;
  gboolean yz_visible;
  gboolean zx_visible;

  GdkColor color_xy;
  GdkColor color_yz;
  GdkColor color_zx;

  GtkPlotLine frame;
  GtkPlotLine corner;
  gboolean corner_visible;

  gdouble zmin, zmax;
  GtkPlotScale zscale;

  gint titles_offset;

  GtkPlotAxis xy, xz;
  GtkPlotAxis yx, yz;
  GtkPlotAxis zx, zy;

  gdouble xfactor, yfactor, zfactor;
};

struct _GtkPlot3DClass
{
  GtkPlotClass parent_class;

  void	(* get_pixel) 		(GtkWidget *widget, 
                      		 gdouble x, gdouble y, gdouble z,
                     		 gdouble *px, gdouble *py, gdouble *pz);
};

/* Plot3D */

GtkType		gtk_plot3d_get_type		(void);
GtkWidget*	gtk_plot3d_new			(GdkDrawable *drawable);
GtkWidget*	gtk_plot3d_new_with_size	(GdkDrawable *drawable,
                                                 gdouble width, gdouble height);
void		gtk_plot3d_construct		(GtkPlot3D *plot,
						 GdkDrawable *drawable);
void		gtk_plot3d_construct_with_size	(GtkPlot3D *plot,
						 GdkDrawable *drawable,
                                                 gdouble width, gdouble height);
void		gtk_plot3d_autoscale		(GtkPlot3D *plot);
/* rotations around global axes */
void		gtk_plot3d_rotate		(GtkPlot3D *plot, 
						 gdouble angle_x,
						 gdouble angle_y,
						 gdouble angle_z);
void 		gtk_plot3d_rotate_vector	(GtkPlot3D *plot,
                          			 GtkPlotVector *vector,
                          			 gdouble a1, 	
						 gdouble a2, 
						 gdouble a3);

/* rotations around local axes */
void 		gtk_plot3d_reset_angles		(GtkPlot3D *plot); 
void 		gtk_plot3d_rotate_x		(GtkPlot3D *plot, 
						 gdouble angle);
void 		gtk_plot3d_rotate_y		(GtkPlot3D *plot, 
						 gdouble angle);
void 		gtk_plot3d_rotate_z		(GtkPlot3D *plot, 
						 gdouble angle);


void 		gtk_plot3d_get_pixel		(GtkPlot3D *plot, 
                          			 gdouble x, 
                                                 gdouble y, 
                                                 gdouble z,
                                                 gdouble *px, 
                                                 gdouble *py,
                                                 gdouble *pz);
void 		gtk_plot3d_set_xrange		(GtkPlot3D *plot, 
						 gdouble min, gdouble max);
void 		gtk_plot3d_set_yrange		(GtkPlot3D *plot, 
						 gdouble min, gdouble max);
void 		gtk_plot3d_set_zrange		(GtkPlot3D *plot, 
						 gdouble min, gdouble max);
void 		gtk_plot3d_set_xfactor		(GtkPlot3D *plot, 
						 gdouble xfactor); 
void 		gtk_plot3d_set_yfactor		(GtkPlot3D *plot, 
						 gdouble yfactor); 
void 		gtk_plot3d_set_zfactor		(GtkPlot3D *plot, 
						 gdouble zfactor); 
gdouble 	gtk_plot3d_get_xfactor		(GtkPlot3D *plot); 
gdouble 	gtk_plot3d_get_yfactor		(GtkPlot3D *plot); 
gdouble 	gtk_plot3d_get_zfactor		(GtkPlot3D *plot); 
/* Planes */
void		gtk_plot3d_plane_set_color	(GtkPlot3D *plot,
						 GtkPlotPlane plane,
						 const GdkColor *color);
void		gtk_plot3d_plane_set_visible	(GtkPlot3D *plot,
						 GtkPlotPlane plane,
						 gboolean visible);
gboolean	gtk_plot3d_plane_visible	(GtkPlot3D *plot,
						 GtkPlotPlane plane);

void		gtk_plot3d_corner_set_visible	(GtkPlot3D *plot,
						 gboolean visible);
gboolean	gtk_plot3d_corner_visible	(GtkPlot3D *plot);

void            gtk_plot3d_corner_set_attributes(GtkPlot3D *plot,
                                                 GtkPlotLineStyle style,                                                         gfloat width,
                                                 const GdkColor *color);
void            gtk_plot3d_corner_get_attributes(GtkPlot3D *plot,
                                                 GtkPlotLineStyle *style,
                                                 gfloat *width,
                                                 GdkColor *color);
void            gtk_plot3d_frame_set_attributes	(GtkPlot3D *plot,
                                                 GtkPlotLineStyle style,
                                                 gfloat width,
                                                 const GdkColor *color);
void            gtk_plot3d_frame_get_attributes	(GtkPlot3D *plot,
                                                 GtkPlotLineStyle *style,
                                                 gfloat *width,
                                                 GdkColor *color);


/* Axes */
GtkPlotAxis *	gtk_plot3d_get_axis		(GtkPlot3D *plot,
						 GtkPlotOrientation orientation);
GtkPlotAxis *	gtk_plot3d_get_side		(GtkPlot3D *plot,
						 GtkPlotSide side);
void		gtk_plot3d_show_major_ticks	(GtkPlot3D *plot,
                                                 GtkPlotSide side,
						 gint ticks_mask);
void		gtk_plot3d_show_minor_ticks	(GtkPlot3D *plot,
                                                 GtkPlotSide side,
						 gint ticks_mask);
void		gtk_plot3d_show_labels		(GtkPlot3D *plot,
                                                 GtkPlotSide side,
						 gint label_mask);
void		gtk_plot3d_show_title		(GtkPlot3D *plot,
                                                 GtkPlotSide side);
void		gtk_plot3d_hide_title		(GtkPlot3D *plot,
                                                 GtkPlotSide side);
void 		gtk_plot3d_set_ticks       	(GtkPlot3D *plot,
						 GtkPlotOrientation axis,
                                 		 gdouble major_step,
                                 		 gint nminor);
void 		gtk_plot3d_set_major_ticks 	(GtkPlot3D *plot,
						 GtkPlotOrientation axis,
                                 		 gdouble major_step);
void 		gtk_plot3d_set_minor_ticks 	(GtkPlot3D *plot,
						 GtkPlotOrientation axis,
                                 		 gint nminor);
void            gtk_plot3d_set_ticks_length	(GtkPlot3D *plot,
                                                 GtkPlotOrientation axis,
                                                 gint length);
void            gtk_plot3d_set_ticks_width 	(GtkPlot3D *plot,
                                                 GtkPlotOrientation axis,
                                                 gfloat width);
void            gtk_plot3d_show_ticks      	(GtkPlot3D *plot,
                                                 GtkPlotSide side,
                                                 gint major_mask,
                                                 gint minor_mask);
void 		gtk_plot3d_set_titles_offset 	(GtkPlot3D *plot,
                                                 gint offset);
gint 		gtk_plot3d_get_titles_offset 	(GtkPlot3D *plot);
void		gtk_plot3d_set_scale		(GtkPlot3D *plot,
						 GtkPlotOrientation axis,
						 GtkPlotScale scale);
GtkPlotScale	gtk_plot3d_get_scale		(GtkPlot3D *plot,
						 GtkPlotOrientation axis);
/* Grids */
void            gtk_plot3d_major_grids_set_visible    (GtkPlot3D *plot,
						       gboolean x,
						       gboolean y,
						       gboolean z);
void            gtk_plot3d_minor_grids_set_visible    (GtkPlot3D *plot,
						       gboolean x,
						       gboolean y,
						       gboolean z);
void            gtk_plot3d_major_grids_visible        (GtkPlot3D *plot,
						       gboolean *x,
						       gboolean *y,
						       gboolean *z);
void            gtk_plot3d_minor_grids_visible        (GtkPlot3D *plot,
						       gboolean *x,
						       gboolean *y,
						       gboolean *z);
void            gtk_plot3d_major_zgrid_set_attributes  (GtkPlot3D *plot,
                                                       GtkPlotLineStyle style,                                                         gfloat width,
                                                       const GdkColor *color);
void            gtk_plot3d_major_zgrid_get_attributes  (GtkPlot3D *plot,
                                                       GtkPlotLineStyle *style,
                                                       gfloat *width,
                                                       GdkColor *color);
void            gtk_plot3d_minor_zgrid_set_attributes  (GtkPlot3D *plot,
                                                       GtkPlotLineStyle style,
                                                       gfloat width,
                                                       const GdkColor *color);
void            gtk_plot3d_minor_zgrid_get_attributes  (GtkPlot3D *plot,
                                                       GtkPlotLineStyle *style,
                                                       gfloat *width,
                                                       GdkColor *color);




#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT3D_H__ */
