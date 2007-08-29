/* gtkplotsurface - 3d scientific plots widget for gtk+
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

#ifndef __GTK_PLOT_SURFACE_H__
#define __GTK_PLOT_SURFACE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gtkplot.h"
#include "gtkplotdt.h"

#define GTK_PLOT_SURFACE(obj)        GTK_CHECK_CAST (obj, gtk_plot_surface_get_type (), GtkPlotSurface)
#define GTK_TYPE_PLOT_SURFACE        (gtk_plot_surface_get_type ())
#define GTK_PLOT_SURFACE_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_surface_get_type(), GtkPlotSurfaceClass)
#define GTK_IS_PLOT_SURFACE(obj)     GTK_CHECK_TYPE (obj, gtk_plot_surface_get_type ())

typedef struct _GtkPlotPolygon             GtkPlotPolygon;
typedef struct _GtkPlotSurface             GtkPlotSurface;
typedef struct _GtkPlotSurfaceClass        GtkPlotSurfaceClass;

struct _GtkPlotPolygon
{
  GtkPlotDTtriangle *t;  		/* parent triangle */
  GtkPlotVector xyz[4]; 		/* points */
  GtkPlotVector p[4];			/* pixels */
  gint n;				/* number of points */
  gdouble level;
  gboolean cut_level;	
  gboolean sublevel;
};

struct _GtkPlotSurface
{
  GtkPlotData data;

  GdkColor color;
  GdkColor shadow;
  GdkColor grid_foreground;
  GdkColor grid_background;

  gboolean use_height_gradient;
  gboolean use_amplitud;

  GtkPlotVector light;
  gdouble ambient;

  gint nx, ny;

  GtkPlotDT *dt;
  gboolean recalc_dt;

  GList *polygons;

  gboolean show_grid;
  gboolean show_mesh;
  gboolean transparent;

  gdouble xstep, ystep;

  GtkPlotLine mesh_line;
};

struct _GtkPlotSurfaceClass
{
  GtkPlotDataClass parent_class;

  void 	(*build_polygons) 		(GtkPlotSurface *surface);
  void 	(*sort_polygons) 		(GtkPlotSurface *surface);
  void 	(*draw_polygons) 		(GtkPlotSurface *surface);
};


GtkType		gtk_plot_surface_get_type	(void);
GtkWidget*	gtk_plot_surface_new		(void);
GtkWidget*	gtk_plot_surface_new_function	(GtkPlotFunc3D function);

void		gtk_plot_surface_construct_function (GtkPlotSurface *surface,
				 		    GtkPlotFunc3D function);

void		gtk_plot_surface_set_color	(GtkPlotSurface *data,
						 GdkColor *color);
void		gtk_plot_surface_set_shadow	(GtkPlotSurface *data,
						 GdkColor *color);
void		gtk_plot_surface_set_grid_foreground	(GtkPlotSurface *data,
						 	 GdkColor *foreground);
void		gtk_plot_surface_set_grid_background	(GtkPlotSurface *data,
						 	 GdkColor *background);
void		gtk_plot_surface_set_grid_visible	(GtkPlotSurface *data,
						 	 gboolean visible);
gboolean	gtk_plot_surface_get_grid_visible	(GtkPlotSurface *data);
void		gtk_plot_surface_set_mesh_visible	(GtkPlotSurface *data,
						 	 gboolean visible);
gboolean	gtk_plot_surface_get_mesh_visible	(GtkPlotSurface *data);
void            gtk_plot_surface_set_light      (GtkPlotSurface *data,
                                 		 gdouble x, 
						 gdouble y, 
						 gdouble z);
void            gtk_plot_surface_set_ambient    (GtkPlotSurface *data,
                                   		 gdouble ambient);
void            gtk_plot_surface_use_height_gradient (GtkPlotSurface *data, 
						 gboolean use_gradient);
void            gtk_plot_surface_use_amplitud   (GtkPlotSurface *data, 
						 gboolean use_amplitud);
void            gtk_plot_surface_set_transparent(GtkPlotSurface *data,
                                   		 gboolean transparent);

void 		gtk_plot_surface_set_points	(GtkPlotSurface *data,
						 gdouble *x, 
                                                 gdouble *y,
                                                 gdouble *z,
						 gdouble *dx, 
                                                 gdouble *dy,
                                                 gdouble *dz,
                                                 gint nx, gint ny);
void 		gtk_plot_surface_get_points	(GtkPlotSurface *data,
						 gdouble **x, 
                                                 gdouble **y,
                                                 gdouble **z,
						 gdouble **dx, 
                                                 gdouble **dy,
                                                 gdouble **dz,
                                                 gint *nx, gint *ny);
GtkPlotArray * 	gtk_plot_surface_set_x		(GtkPlotSurface *data,
						 gdouble *x); 
GtkPlotArray * 	gtk_plot_surface_set_y		(GtkPlotSurface *data,
						 gdouble *y); 
GtkPlotArray * 	gtk_plot_surface_set_z		(GtkPlotSurface *data,
						 gdouble *z); 
GtkPlotArray * 	gtk_plot_surface_set_dx		(GtkPlotSurface *data,
						 gdouble *dx); 
GtkPlotArray *	gtk_plot_surface_set_dy		(GtkPlotSurface *data,
						 gdouble *dy); 
GtkPlotArray *	gtk_plot_surface_set_dz		(GtkPlotSurface *data,
						 gdouble *dz); 
gdouble * 	gtk_plot_surface_get_x		(GtkPlotSurface *data, 
                                                 gint *nx);
gdouble * 	gtk_plot_surface_get_y		(GtkPlotSurface *data, 
                                                 gint *ny);
gdouble * 	gtk_plot_surface_get_z		(GtkPlotSurface *data,
                                                 gint *nx, gint *ny); 
gdouble * 	gtk_plot_surface_get_dx		(GtkPlotSurface *data); 
gdouble * 	gtk_plot_surface_get_dy		(GtkPlotSurface *data); 
gdouble * 	gtk_plot_surface_get_dz		(GtkPlotSurface *data); 
void		gtk_plot_surface_set_nx  	(GtkPlotSurface *data,
                                                 gint nx);
void		gtk_plot_surface_set_ny  	(GtkPlotSurface *data,
                                                 gint ny);
gint		gtk_plot_surface_get_nx  	(GtkPlotSurface *data);
gint		gtk_plot_surface_get_ny  	(GtkPlotSurface *data);

void		gtk_plot_surface_set_xstep  	(GtkPlotSurface *data,
						 gdouble xstep);
void		gtk_plot_surface_set_ystep  	(GtkPlotSurface *data,
						 gdouble ystep);
gdouble		gtk_plot_surface_get_xstep  	(GtkPlotSurface *data);
gdouble		gtk_plot_surface_get_ystep  	(GtkPlotSurface *data);
void            gtk_plot_surface_build_mesh     (GtkPlotSurface *data);
void            gtk_plot_surface_recalc_nodes   (GtkPlotSurface *data);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_SURFACE_H__ */
