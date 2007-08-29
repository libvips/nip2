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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gtk/gtk.h>
#include "gtkplot.h"
#include "gtkplot3d.h"
#include "gtkplotdata.h"
#include "gtkplotsurface.h"
#include "gtkpsfont.h"
#include "gtkplotpc.h"

#define P_(string) string

static void gtk_plot_surface_class_init 	(GtkPlotSurfaceClass *klass);
static void gtk_plot_surface_init 		(GtkPlotSurface *data);
static void gtk_plot_surface_destroy 		(GtkObject *object);
static void gtk_plot_surface_get_property         (GObject      *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_surface_set_property         (GObject      *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_surface_clone 		(GtkPlotData *data,
						 GtkPlotData *copy);
static void gtk_plot_surface_update_range 	(GtkPlotData *data);
static void gtk_plot_surface_real_build_mesh	(GtkPlotSurface *surface);
static void gtk_plot_surface_build_polygons 	(GtkPlotSurface *surface);
static void gtk_plot_surface_sort_polygons	(GtkPlotSurface *surface);
static void update_data 			(GtkPlotData *data, 
						 gboolean new_range);
static gboolean gtk_plot_surface_add_to_plot 	(GtkPlotData *data,
						 GtkPlot *plot);
static void gtk_plot_surface_draw_private 	(GtkPlotData *data);
static void gtk_plot_surface_draw_legend	(GtkPlotData *data, 
						 gint x, gint y);
static void gtk_plot_surface_draw_polygons 	(GtkPlotSurface *surface);
static void gtk_plot_surface_lighting 		(GdkColor *a, 
						 GdkColor *b, 
						 gdouble normal,
						 gdouble ambient);
static void clear_polygons                      (GtkPlotSurface *surface);
extern inline gint roundint				(gdouble x);
static void hsv_to_rgb 				(gdouble  h, 
						 gdouble  s, 
						 gdouble  v,
            					 gdouble *r, 
						 gdouble *g, 
						 gdouble *b);
static void rgb_to_hsv 				(gdouble  r, 
						 gdouble  g, 
						 gdouble  b,
            					 gdouble *h, 
						 gdouble *s, 
						 gdouble *v);
static gint compare_func                           (gpointer a, gpointer b);

enum {
  ARG_0,
  ARG_USE_HEIGHT,
  ARG_USE_AMPLITUD,
  ARG_LIGHT,
  ARG_AMBIENT,
  ARG_NX,
  ARG_NY,
  ARG_SHOW_GRID,
  ARG_SHOW_MESH,
  ARG_TRANSPARENT,
  ARG_XSTEP,
  ARG_YSTEP,
  ARG_MESH_STYLE,
  ARG_MESH_WIDTH,
  ARG_MESH_COLOR,
};

static GtkPlotDataClass *parent_class = NULL;

/* "stolen" from 3dgpl */
inline gulong _sqrt(register gulong arg)
{ 
   register gint i;
   register gulong nprd,msk=0x8000L,val=0,prd=0;
  
   for(i=15;i>=0;i--){
     nprd = prd + (val<<(i+1))+(msk<<i);
     if(nprd <= arg) { val |= msk; prd = nprd; }
     msk >>= 1;
   }
   return val;
} 

GtkType
gtk_plot_surface_get_type (void)
{
  static GtkType data_type = 0;

  if (!data_type)
    {
      GtkTypeInfo data_info =
      {
	"GtkPlotSurface",
	sizeof (GtkPlotSurface),
	sizeof (GtkPlotSurfaceClass),
	(GtkClassInitFunc) gtk_plot_surface_class_init,
	(GtkObjectInitFunc) gtk_plot_surface_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      data_type = gtk_type_unique (gtk_plot_data_get_type(), &data_info);
    }
  return data_type;
}

static void
gtk_plot_surface_class_init (GtkPlotSurfaceClass *klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkPlotDataClass *data_class;
  GtkPlotSurfaceClass *surface_class;
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  parent_class = gtk_type_class (gtk_plot_data_get_type ());

  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;
  data_class = (GtkPlotDataClass *) klass;
  surface_class = (GtkPlotSurfaceClass *) klass;

  object_class->destroy = gtk_plot_surface_destroy;

  gobject_class->set_property = gtk_plot_surface_set_property;
  gobject_class->get_property = gtk_plot_surface_get_property;

  g_object_class_install_property (gobject_class,
                           ARG_USE_HEIGHT,
  g_param_spec_int ("use_height",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_USE_AMPLITUD,
  g_param_spec_int ("use_amplitud",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LIGHT,
  g_param_spec_pointer ("vector_light",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_AMBIENT,
  g_param_spec_double ("ambient",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_NX,
  g_param_spec_int ("nx",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_NY,
  g_param_spec_int ("ny",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHOW_GRID,
  g_param_spec_boolean ("show_grid",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHOW_MESH,
  g_param_spec_boolean ("show_mesh",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_TRANSPARENT,
  g_param_spec_boolean ("transparent",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XSTEP,
  g_param_spec_double ("xstep",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YSTEP,
  g_param_spec_double ("ystep",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_MESH_STYLE,
  g_param_spec_int ("mesh_style",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_MESH_WIDTH,
  g_param_spec_double ("mesh_width",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_MESH_COLOR,
  g_param_spec_pointer ("mesh_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));

  data_class->clone = gtk_plot_surface_clone;
  data_class->update = update_data;
  data_class->add_to_plot = gtk_plot_surface_add_to_plot;
  data_class->draw_data = gtk_plot_surface_draw_private;
  data_class->draw_legend = gtk_plot_surface_draw_legend;
  surface_class->draw_polygons = gtk_plot_surface_draw_polygons;
  surface_class->build_polygons = gtk_plot_surface_build_polygons;
  surface_class->sort_polygons = gtk_plot_surface_sort_polygons;

}

static void
gtk_plot_surface_set_property (GObject      *object,
                             guint            prop_id,
                             const GValue          *value,
                             GParamSpec      *pspec)
{
  GtkPlotSurface *data;

  data = GTK_PLOT_SURFACE (object);

  switch (prop_id)
    {
      case ARG_USE_HEIGHT:
        data->use_height_gradient = g_value_get_boolean(value);
        break;
      case ARG_USE_AMPLITUD:
        data->use_amplitud = g_value_get_boolean(value);
        break;
      case ARG_LIGHT:
        data->light = *((GtkPlotVector *)g_value_get_pointer(value));
        break;
      case ARG_AMBIENT:
        data->ambient = g_value_get_double(value);
        break;
      case ARG_NX:
        data->nx = g_value_get_int(value);
        break;
      case ARG_NY:
        data->ny = g_value_get_int(value);
        break;
      case ARG_SHOW_GRID:
        data->show_grid = g_value_get_boolean(value);
        break;
      case ARG_SHOW_MESH:
        data->show_mesh = g_value_get_boolean(value);
        break;
      case ARG_TRANSPARENT:
        data->transparent = g_value_get_boolean(value);
        break;
      case ARG_XSTEP:
        data->xstep = g_value_get_double(value);
        break;
      case ARG_YSTEP:
        data->ystep = g_value_get_double(value);
        break;
      case ARG_MESH_STYLE:
        data->mesh_line.line_style = g_value_get_int(value);
        break;
      case ARG_MESH_WIDTH:
        data->mesh_line.line_width = g_value_get_double(value);
        break;
      case ARG_MESH_COLOR:
        data->mesh_line.color = *((GdkColor *)g_value_get_pointer(value));
        break;
    }
}

static void
gtk_plot_surface_get_property (GObject      *object,
                             guint            prop_id,
                             GValue          *value,
                             GParamSpec      *pspec)
{
  GtkPlotSurface *data;

  data = GTK_PLOT_SURFACE (object);

  switch (prop_id)
    {
      case ARG_USE_HEIGHT:
        g_value_set_boolean(value, data->use_height_gradient);
        break;
      case ARG_USE_AMPLITUD:
        g_value_set_boolean(value, data->use_amplitud);
        break;
      case ARG_LIGHT:
        g_value_set_pointer(value, &data->light);
        break;
      case ARG_AMBIENT:
        g_value_set_double(value, data->ambient);
        break;
      case ARG_NX:
        g_value_set_int(value, data->nx);
        break;
      case ARG_NY:
        g_value_set_int(value, data->ny);
        break;
      case ARG_SHOW_GRID:
        g_value_set_boolean(value, data->show_grid);
        break;
      case ARG_SHOW_MESH:
        g_value_set_boolean(value, data->show_mesh);
        break;
      case ARG_TRANSPARENT:
        g_value_set_boolean(value, data->transparent);
        break;
      case ARG_XSTEP:
        g_value_set_double(value, data->xstep);
        break;
      case ARG_YSTEP:
        g_value_set_double(value, data->ystep);
        break;
      case ARG_MESH_STYLE:
        g_value_set_int(value, data->mesh_line.line_style);
        break;
      case ARG_MESH_WIDTH:
        g_value_set_double(value, data->mesh_line.line_width);
        break;
      case ARG_MESH_COLOR:
        g_value_set_pointer(value, &data->mesh_line.color);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
update_data (GtkPlotData *data, gboolean new_range)
{
  if(new_range && data->is_function) 
    GTK_PLOT_SURFACE(data)->recalc_dt = TRUE;

  GTK_PLOT_DATA_CLASS(parent_class)->update(data, new_range);
}

static void
gtk_plot_surface_update_range (GtkPlotData *data)
{
  GtkPlotSurface *surface;
  GtkPlot *plot;

  if(!data->redraw_pending) return; 

  surface = GTK_PLOT_SURFACE(data);
  plot = data->plot;


  if(!surface->recalc_dt && data->num_points > 0){
     gtk_plot_surface_recalc_nodes(surface);
     data->redraw_pending = FALSE;
     surface->recalc_dt = FALSE;
     return;
  }

  if(!data->is_function && data->num_points > 0){
     if(surface->recalc_dt)
       gtk_plot_surface_build_mesh(surface);
     else
       gtk_plot_surface_recalc_nodes(surface);

     data->redraw_pending = FALSE;
     surface->recalc_dt = FALSE;
     return;
  }

  if(data->is_function){
     gdouble xstep, ystep;
     gdouble xmin, xmax, ymin, ymax;
     gdouble x, y;
     gdouble *fx = NULL, *fy = NULL, *fz = NULL;
     gint nx, ny;
     gint npoints;

     xmin = GTK_PLOT(plot)->xmin;
     xmax = GTK_PLOT(plot)->xmax;
     ymin = GTK_PLOT(plot)->ymin;
     ymax = GTK_PLOT(plot)->ymax;

     xstep = surface->xstep;
     surface->nx = roundint((xmax - xmin) / xstep) + 1;

     ystep = surface->ystep;
     surface->ny = roundint((ymax - ymin) / ystep) + 1;

     npoints = surface->nx * surface->ny;
     fx = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));
     fy = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));
     fz = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));

     npoints = 0;
     y = ymin;
     for(ny = 0; ny < surface->ny; ny++)
       {
         x = xmin;
         for(nx = 0; nx < surface->nx; nx++)
          {
            gboolean error;
            fx[npoints] = x;
            fy[npoints] = y;
            fz[npoints] = data->function3d(plot, data, x, y, &error);

            x += xstep;
            npoints++;
          }
         y += ystep;
     }

     gtk_plot_data_set_x(GTK_PLOT_DATA(surface), fx);
     gtk_plot_data_set_y(GTK_PLOT_DATA(surface), fy);
     gtk_plot_data_set_z(GTK_PLOT_DATA(surface), fz);
     gtk_plot_data_set_numpoints(GTK_PLOT_DATA(surface), npoints);

     gtk_plot_surface_build_mesh(surface);

     g_free(fx);
     g_free(fy);
     g_free(fz);
  }

  data->redraw_pending = FALSE;
  surface->recalc_dt = FALSE;
}

static gboolean 
gtk_plot_surface_add_to_plot (GtkPlotData *data, GtkPlot *plot)
{
  if(!data->is_function && data->num_points > 0){
    gtk_plot_surface_build_mesh(GTK_PLOT_SURFACE(data));
  }

  data->redraw_pending = TRUE;

  return TRUE;
}

static void
gtk_plot_surface_init (GtkPlotSurface *dataset)
{
  GtkWidget *widget;
  GdkColormap *colormap;
  GdkColor color;
  GtkPlotArray *dim;

  GTK_WIDGET_SET_FLAGS(dataset, GTK_NO_WINDOW);

  widget = GTK_WIDGET(dataset);
  colormap = gtk_widget_get_colormap(widget);

  gdk_color_parse("black", &color);
  gdk_color_alloc(colormap, &color);
  dataset->grid_foreground = color;

  gdk_color_parse("dark green", &color);
  gdk_color_alloc(colormap, &color);
  dataset->grid_background = color;

  gdk_color_parse("gray30", &color);
  gdk_color_alloc(colormap, &color);
  dataset->shadow = color;

  gdk_color_parse("blue", &color);
  gdk_color_alloc(colormap, &color);
  dataset->color = color;

  dataset->light.x = 0.;
  dataset->light.y = 0.;
  dataset->light.z = 1.;

  dataset->nx = 0;
  dataset->ny = 0;

  dataset->show_grid = TRUE;
  dataset->show_mesh = FALSE;
  dataset->transparent = FALSE;
  dataset->use_height_gradient = FALSE;
  dataset->use_amplitud = FALSE;

  dataset->ambient = 0.3;

  dataset->xstep = 0.05;
  dataset->ystep = 0.05;

  dataset->mesh_line.color = dataset->grid_foreground;
  dataset->mesh_line.line_width = 1;
  dataset->mesh_line.line_style = GTK_PLOT_LINE_SOLID;

  dataset->dt = GTK_PLOT_DT(gtk_plot_dt_new(0));
  dataset->dt->quadrilateral = FALSE;
  dataset->recalc_dt = TRUE;

  dataset->polygons = NULL;

  dim = gtk_plot_data_find_dimension(GTK_PLOT_DATA(dataset), "y");
  gtk_plot_array_set_independent(dim, TRUE);
  dim = gtk_plot_data_find_dimension(GTK_PLOT_DATA(dataset), "z");
  gtk_plot_array_set_required(dim, TRUE);
}

GtkWidget*
gtk_plot_surface_new (void)
{
  GtkPlotData *data;

  data = gtk_type_new (gtk_plot_surface_get_type ());

  return GTK_WIDGET (data);
}

GtkWidget*
gtk_plot_surface_new_function (GtkPlotFunc3D function)
{
  GtkWidget *dataset;

  dataset = gtk_type_new (gtk_plot_surface_get_type ());

  gtk_plot_surface_construct_function(GTK_PLOT_SURFACE(dataset), function);

  return dataset;
}

void
gtk_plot_surface_construct_function(GtkPlotSurface *surface, 
				   GtkPlotFunc3D function)
{
  GTK_PLOT_DATA(surface)->is_function = TRUE;
  GTK_PLOT_DATA(surface)->function3d = function;
}

void
gtk_plot_surface_destroy(GtkObject *object) 
{
  GtkPlotSurface *surface;
 
  surface = GTK_PLOT_SURFACE(object);

  if(surface->dt)
    gtk_object_destroy(GTK_OBJECT(surface->dt));

  clear_polygons(surface);

  if ( GTK_OBJECT_CLASS (parent_class)->destroy )
    (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
clear_polygons(GtkPlotSurface *surface)
{
  if(surface->polygons){
    GList *list;
    for (list = surface->polygons; list; list = list->next)
      if (list->data) g_free(list->data);
    g_list_free(surface->polygons);
    surface->polygons= NULL;
  }
}

static void
gtk_plot_surface_clone(GtkPlotData *real_data, GtkPlotData *copy_data)
{
  GTK_PLOT_DATA_CLASS(parent_class)->clone(real_data, copy_data);


  GTK_PLOT_SURFACE(copy_data)->color = GTK_PLOT_SURFACE(real_data)->color;
  GTK_PLOT_SURFACE(copy_data)->shadow = GTK_PLOT_SURFACE(real_data)->shadow;
  GTK_PLOT_SURFACE(copy_data)->grid_foreground = GTK_PLOT_SURFACE(real_data)->grid_foreground;
  GTK_PLOT_SURFACE(copy_data)->grid_background = GTK_PLOT_SURFACE(real_data)->grid_background;

  GTK_PLOT_SURFACE(copy_data)->use_height_gradient = GTK_PLOT_SURFACE(real_data)->use_height_gradient;
  GTK_PLOT_SURFACE(copy_data)->use_amplitud = GTK_PLOT_SURFACE(real_data)->use_amplitud;

  GTK_PLOT_SURFACE(copy_data)->light = GTK_PLOT_SURFACE(real_data)->light;
  GTK_PLOT_SURFACE(copy_data)->ambient = GTK_PLOT_SURFACE(real_data)->ambient;

  GTK_PLOT_SURFACE(copy_data)->nx = GTK_PLOT_SURFACE(real_data)->nx;
  GTK_PLOT_SURFACE(copy_data)->ny = GTK_PLOT_SURFACE(real_data)->ny;

  GTK_PLOT_SURFACE(copy_data)->show_grid = GTK_PLOT_SURFACE(real_data)->show_grid;
  GTK_PLOT_SURFACE(copy_data)->show_mesh = GTK_PLOT_SURFACE(real_data)->show_mesh;
  GTK_PLOT_SURFACE(copy_data)->transparent = GTK_PLOT_SURFACE(real_data)->transparent;

  GTK_PLOT_SURFACE(copy_data)->xstep = GTK_PLOT_SURFACE(real_data)->xstep;
  GTK_PLOT_SURFACE(copy_data)->ystep = GTK_PLOT_SURFACE(real_data)->ystep;

  GTK_PLOT_SURFACE(copy_data)->mesh_line = GTK_PLOT_SURFACE(real_data)->mesh_line;
}


static void
gtk_plot_surface_draw_private   (GtkPlotData *data)  
{
  GtkPlot *plot;
  GtkPlotSurface *surface;

  g_return_if_fail(GTK_IS_PLOT_SURFACE(data));

  surface = GTK_PLOT_SURFACE(data);
  data = GTK_PLOT_DATA(surface);

  g_return_if_fail(GTK_PLOT_DATA(data)->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(GTK_PLOT_DATA(data)->plot));

  plot = GTK_PLOT(data->plot);

  g_return_if_fail(GTK_PLOT_DATA(data)->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(GTK_PLOT_DATA(data)->plot));

  if(data->redraw_pending) gtk_plot_surface_update_range(data);

  gtk_plot_pc_gsave(plot->pc);
  GTK_PLOT_SURFACE_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(surface)))->draw_polygons(surface); 
  gtk_plot_pc_grestore(plot->pc);
}

static void
gtk_plot_surface_draw_polygons (GtkPlotSurface *surface)
{
  GtkPlot *plot;
  GtkPlotData *data;
  GtkPlotPoint t[3];
  GtkPlotDTtriangle *triangle;
  GdkDrawable *drawable;
  gboolean visible = TRUE;
  GtkPlotVector side1, side2, light, normal;
  GdkColor color, real_color;
  GtkPlotVector points[3];
  gdouble factor, norm;
  GList *list;
  gboolean color_set = FALSE;
  gdouble *array_a;
  gint n;

  data = GTK_PLOT_DATA(surface);
  plot = GTK_PLOT(data->plot);
  drawable = plot->drawable;

  gtk_plot_set_line_attributes(plot, surface->mesh_line);
  array_a = gtk_plot_data_get_a(GTK_PLOT_DATA(surface), &n);

  list = surface->polygons;
  while(list){
    GtkPlotPolygon *poly = (GtkPlotPolygon *)list->data;
 
    triangle = poly->t;
    visible = TRUE;

    t[0].x = poly->p[0].x;
    t[0].y = poly->p[0].y;
    t[1].x = poly->p[1].x;
    t[1].y = poly->p[1].y;
    t[2].x = poly->p[2].x;
    t[2].y = poly->p[2].y;

    points[0].x = triangle->na->x;
    points[0].y = triangle->na->y;
    points[0].z = triangle->na->z;
    points[1].x = triangle->nb->x;
    points[1].y = triangle->nb->y;
    points[1].z = triangle->nb->z;
    points[2].x = triangle->nc->x;
    points[2].y = triangle->nc->y;
    points[2].z = triangle->nc->z;

    if(plot->clip_data){
      gboolean discard = FALSE;
      gint i;
      if(GTK_IS_PLOT3D(plot)){
        GtkPlot3D *plot3d = GTK_PLOT3D(plot);
        for(i = 0; i < 3; i++){
          if(points[i].x < plot3d->ax->ticks.min || points[i].x > plot3d->ax->ticks.max ||
             points[i].y < plot3d->ay->ticks.min || points[i].y > plot3d->ay->ticks.max ||
             points[i].z < plot3d->az->ticks.min || points[i].z > plot3d->az->ticks.max)
             { discard = TRUE; break; }
        }
      } else {
        for(i = 0; i < 3; i++){
          if(points[i].x < plot->bottom->ticks.min || points[i].x > plot->bottom->ticks.max ||
             points[i].y < plot->left->ticks.min || points[i].y > plot->left->ticks.max)
             { discard = TRUE; break; }
        }
      }
      if(discard) { list = list->next; continue; }
    }
           
  
    side1.x = -(points[1].x - points[0].x);
    side1.y = -(points[1].y - points[0].y);
    side1.z = -(points[1].z - points[0].z);
    side2.x = -(points[2].x - points[0].x);
    side2.y = -(points[2].y - points[0].y);
    side2.z = -(points[2].z - points[0].z);
           
    if(surface->use_amplitud && array_a){
      gtk_plot_data_get_gradient_level(GTK_PLOT_DATA(surface),
  				     (array_a[triangle->na->id] +
                                      array_a[triangle->nb->id] +
  				      array_a[triangle->nc->id]) / 3.0,
  				     &real_color); 
    } else if(surface->use_height_gradient){
      gtk_plot_data_get_gradient_level(GTK_PLOT_DATA(surface),
  				     (triangle->na->z +
                                      triangle->nb->z +
  				      triangle->nc->z) / 3.0,
  				     &real_color); 
    }else{
      color = surface->color; 
      light = surface->light;
      norm = _sqrt(10000*(light.x*light.x + light.y*light.y + light.z*light.z))/100;
      light.x /= norm;
      light.y /= norm;
      light.z /= norm;
  
      factor = 1.0;
      if(GTK_IS_PLOT3D(plot)){
        normal.x = side1.y * side2.z - side1.z * side2.y;
        normal.y = side1.z * side2.x - side1.x * side2.z;
        normal.z = side1.x * side2.y - side1.y * side2.x;
  
        norm = _sqrt(100000000*(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z))/10000.;
        factor = (normal.x*light.x + normal.y*light.y + normal.z*light.z) / norm;
      }
  
      gtk_plot_surface_lighting(&color, &real_color, 
                                factor, surface->ambient); 

    }
  
    if(GTK_IS_PLOT3D(plot))
      if(((t[1].x-t[0].x)*(t[2].y-t[0].y) - (t[1].y-t[0].y)*(t[2].x-t[0].x)) > 0)
           visible = FALSE;
  
    if(visible)
           gtk_plot_pc_set_color(plot->pc, &real_color);
    else
           gtk_plot_pc_set_color(plot->pc, &surface->shadow);
  
    gtk_plot_pc_draw_polygon(plot->pc, !surface->transparent, t, 3); 
  
  
    color_set = FALSE;
    if(surface->show_mesh){
        if(visible)
           gtk_plot_pc_set_color(plot->pc, &surface->grid_foreground);
        else
           gtk_plot_pc_set_color(plot->pc, &surface->grid_background);
        color_set = TRUE;
        gtk_plot_pc_draw_polygon(plot->pc, FALSE, t, 3); 
    }
  
    if(!surface->show_mesh && surface->show_grid && surface->dt->quadrilateral){
        if(!color_set){
          if(visible)
             gtk_plot_pc_set_color(plot->pc, &surface->grid_foreground);
          else
             gtk_plot_pc_set_color(plot->pc, &surface->grid_background);
        }

        gtk_plot_pc_draw_line(plot->pc,  
                              t[1].x, t[1].y, t[2].x, t[2].y); 

    }

    list = list->next;
  }
}

static void
gtk_plot_surface_draw_legend(GtkPlotData *data, gint x, gint y)
{
  GtkPlotSurface *surface;
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  GdkRectangle area;
  gint lascent, ldescent, lheight, lwidth;
  gdouble m;

  surface = GTK_PLOT_SURFACE(data);

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));

  plot = data->plot;
  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;
  area.width = GTK_WIDGET(plot)->allocation.width;
  area.height = GTK_WIDGET(plot)->allocation.height;

  m = plot->magnification;
  legend = plot->legends_attr;

  if(data->legend)
    legend.text = data->legend;
  else
    legend.text = "";

  gtk_plot_text_get_size(legend.text, legend.angle, legend.font,
                         roundint(legend.height * m), 
                         &lwidth, &lheight,
                         &lascent, &ldescent);


  if(data->show_legend){
    if(!surface->use_height_gradient && !surface->use_amplitud){
      gtk_plot_pc_set_color(plot->pc, &surface->color);
  
      gtk_plot_pc_draw_rectangle(plot->pc, TRUE, 
                                 area.x + x, area.y + y,
                                 roundint(plot->legends_line_width * m), 
                                 lascent + ldescent);
    }else{
      gdouble level, step;
      gint lx = x, lstep;
      step = (data->gradient->ticks.max - data->gradient->ticks.min) / 10;
      lstep = roundint(plot->legends_line_width * m / 10.);
      for(level = data->gradient->ticks.min; level < data->gradient->ticks.max; level += step){
        GdkColor color;
        gtk_plot_data_get_gradient_level(data, level, &color);
        gtk_plot_pc_set_color(plot->pc, &color);
  
        gtk_plot_pc_draw_rectangle(plot->pc, TRUE, 
                                   area.x + lx, area.y + y,
                                   lstep, lascent + ldescent);
        lx += lstep;
      }
    }

    legend.x = (gdouble)(area.x + x + roundint((plot->legends_line_width + 4) * m))
               / (gdouble)area.width;
    legend.y = (gdouble)(area.y + y + lascent) / (gdouble)area.height;
  
    gtk_plot_draw_text(plot, legend);
  
    y += 2*lheight;
  } else 
    y += lheight;
}

static void
gtk_plot_surface_lighting (GdkColor *a, GdkColor *b, 
                           gdouble normal, gdouble ambient)
{
  gdouble red, green, blue;
  gdouble h, s, v;

  if(normal == 1.0){
   *b = *a;
   return;
  }

  normal = MIN(fabs(normal), 1.0);

  red = a->red;
  green = a->green;
  blue = a->blue;

  rgb_to_hsv(red, green, blue, &h, &s, &v);

  s *= normal;
  v *= normal;

  s += ambient;
  v += ambient;

  hsv_to_rgb(h, MIN(s, 1.0), MIN(v, 1.0), &red, &green, &blue);

  b->red = red;
  b->green = green;
  b->blue = blue;
}


static void
hsv_to_rgb (gdouble  h, gdouble  s, gdouble  v,
            gdouble *r, gdouble *g, gdouble *b)
{
  gint i;
  gdouble f, w, q, t;

  if (s == 0.0)
    s = 0.000001;

  if (h == -1.0)
    {
      *r = v;
      *g = v;
      *b = v;
    }
  else
    {
      if (h == 360.0) h = 0.0;
      h = h / 60.0;
      i = (gint) h;
      f = h - i;
      w = v * (1.0 - s);
      q = v * (1.0 - (s * f));
      t = v * (1.0 - (s * (1.0 - f)));

      switch (i)
      {
        case 0:
          *r = v;
          *g = t;
          *b = w;
          break;
        case 1:
          *r = q;
          *g = v;
          *b = w;
          break;
        case 2:
          *r = w;
          *g = v;
          *b = t;
          break;
        case 3:
          *r = w;
          *g = q;
          *b = v;
          break;
        case 4:
          *r = t;
          *g = w;
          *b = v;
          break;
        case 5:
          *r = v;
          *g = w;
          *b = q;
          break;
      }
    }

  *r *= 65535.;
  *g *= 65535.;
  *b *= 65535.;
}

static void
rgb_to_hsv (gdouble  r, gdouble  g, gdouble  b,
            gdouble *h, gdouble *s, gdouble *v)
{
  double max, min, delta;

  r /= 65535.;
  g /= 65535.;
  b /= 65535.;

  max = r;
  if (g > max)
    max = g;
  if (b > max)
    max = b;

  min = r;
  if (g < min)
    min = g;
  if (b < min)
    min = b;

  *v = max;
  if (max != 0.0)
    *s = (max - min) / max;
  else
    *s = 0.0;

  if (*s == 0.0)
    *h = -1.0;
  else
    {
      delta = max - min;

      if (r == max)
        *h = (g - b) / delta;
      else if (g == max)
        *h = 2.0 + (b - r) / delta;
      else if (b == max)
        *h = 4.0 + (r - g) / delta;

      *h = *h * 60.0;

      if (*h < 0.0)
        *h = *h + 360;
    }
}


/******************************************
 * gtk_plot_surface_set_color
 * gtk_plot_surface_set_shadow
 * gtk_plot_surface_set_grid_foreground
 * gtk_plot_surface_set_grid_background
 * gtk_plot_surface_set_grid_visible
 * gtk_plot_surface_set_mesh_visible
 * gtk_plot_surface_get_grid_visible
 * gtk_plot_surface_get_mesh_visible
 ******************************************/

void            
gtk_plot_surface_set_color      (GtkPlotSurface *data,
                                 GdkColor *color)
{
  data->color = *color;
}

void            
gtk_plot_surface_set_shadow     (GtkPlotSurface *data,
                                 GdkColor *color)
{
  data->shadow = *color;
}

void            
gtk_plot_surface_set_grid_foreground    (GtkPlotSurface *data,
                                         GdkColor *foreground)
{
  data->grid_foreground = *foreground;
}

void            
gtk_plot_surface_set_grid_background    (GtkPlotSurface *data,
                                         GdkColor *background)
{
  data->grid_background = *background;
}

void            
gtk_plot_surface_set_grid_visible    (GtkPlotSurface *data,
                                         gboolean visible)
{
  data->show_grid = visible;
}

gboolean            
gtk_plot_surface_get_grid_visible    (GtkPlotSurface *data)
{
  return (data->show_grid);
}

void            
gtk_plot_surface_set_mesh_visible    (GtkPlotSurface *data,
                                         gboolean visible)
{
  data->show_mesh = visible;
}

gboolean            
gtk_plot_surface_get_mesh_visible    (GtkPlotSurface *data)
{
  return (data->show_mesh);
}

void            
gtk_plot_surface_set_light      (GtkPlotSurface *data,
                                 gdouble x, gdouble y, gdouble z)
{
  data->light.x = x;
  data->light.y = y;
  data->light.z = z;
}

void            
gtk_plot_surface_use_height_gradient (GtkPlotSurface *data,
                                      gboolean use_gradient)
{
  data->use_height_gradient = use_gradient;
}

void            
gtk_plot_surface_use_amplitud (GtkPlotSurface *data,
                               gboolean use_amplitud)
{
  data->use_amplitud = use_amplitud;
}

void            
gtk_plot_surface_set_ambient      (GtkPlotSurface *data,
                                   gdouble ambient)
{
  data->ambient = ambient;
}

void            
gtk_plot_surface_set_transparent  (GtkPlotSurface *data,
                                   gboolean transparent)
{
  data->transparent = transparent;
}

/******************************************
 * gtk_plot_surface_set_points
 * gtk_plot_surface_get_points
 * gtk_plot_surface_set_x
 * gtk_plot_surface_set_y
 * gtk_plot_surface_set_z
 * gtk_plot_surface_set_dx
 * gtk_plot_surface_set_dy
 * gtk_plot_surface_set_dz
 * gtk_plot_surface_get_x
 * gtk_plot_surface_get_y
 * gtk_plot_surface_get_z
 * gtk_plot_surface_get_dx
 * gtk_plot_surface_get_dy
 * gtk_plot_surface_get_dz
 * gtk_plot_surface_set_nx
 * gtk_plot_surface_set_ny
 * gtk_plot_surface_get_nx
 * gtk_plot_surface_get_ny
 * gtk_plot_surface_set_xstep
 * gtk_plot_surface_set_ystep
 * gtk_plot_surface_get_xstep
 * gtk_plot_surface_get_ystep
 ******************************************/

void
gtk_plot_surface_set_points(GtkPlotSurface *data, 
                            gdouble *x, gdouble *y, gdouble *z,
                            gdouble *dx, gdouble *dy, gdouble *dz,
                            gint nx, gint ny)
{
  gtk_plot_data_set_x(GTK_PLOT_DATA(data), x);
  gtk_plot_data_set_y(GTK_PLOT_DATA(data), y);
  gtk_plot_data_set_z(GTK_PLOT_DATA(data), z);
  gtk_plot_data_set_dx(GTK_PLOT_DATA(data), dx);
  gtk_plot_data_set_dy(GTK_PLOT_DATA(data), dy);
  gtk_plot_data_set_dz(GTK_PLOT_DATA(data), dz);
  data->nx = nx;
  data->ny = ny;
  gtk_plot_data_set_numpoints(GTK_PLOT_DATA(data), nx * ny);

  gtk_plot_surface_build_mesh(data);
}

void
gtk_plot_surface_get_points(GtkPlotSurface *data, 
                            gdouble **x, gdouble **y, gdouble **z,
                            gdouble **dx, gdouble **dy, gdouble **dz,
                            gint *nx, gint *ny)
{
  gint n;
  *x = gtk_plot_data_get_x(GTK_PLOT_DATA(data), &n);
  *y = gtk_plot_data_get_y(GTK_PLOT_DATA(data), &n);
  *z = gtk_plot_data_get_z(GTK_PLOT_DATA(data), &n);
  *dx = gtk_plot_data_get_dx(GTK_PLOT_DATA(data), &n);
  *dy = gtk_plot_data_get_dy(GTK_PLOT_DATA(data), &n);
  *dz = gtk_plot_data_get_dz(GTK_PLOT_DATA(data), &n);
  *nx = data->nx;
  *ny = data->ny;
}

GtkPlotArray *
gtk_plot_surface_set_x(GtkPlotSurface *data, 
                       gdouble *x) 
{
  return gtk_plot_data_set_x(GTK_PLOT_DATA(data), x);
}

GtkPlotArray *
gtk_plot_surface_set_y(GtkPlotSurface *data, 
                       gdouble *y) 
{
  return gtk_plot_data_set_y(GTK_PLOT_DATA(data), y);
}

GtkPlotArray *
gtk_plot_surface_set_z(GtkPlotSurface *data, 
                       gdouble *z) 
{
  return gtk_plot_data_set_z(GTK_PLOT_DATA(data), z);
}

GtkPlotArray *
gtk_plot_surface_set_dx(GtkPlotSurface *data, 
                        gdouble *dx) 
{
  return gtk_plot_data_set_dx(GTK_PLOT_DATA(data), dx);
}

GtkPlotArray *
gtk_plot_surface_set_dy(GtkPlotSurface *data, 
                        gdouble *dy) 
{
  return gtk_plot_data_set_dy(GTK_PLOT_DATA(data), dy);
}

GtkPlotArray *
gtk_plot_surface_set_dz(GtkPlotSurface *data, 
                       gdouble *dz) 
{
  return gtk_plot_data_set_dz(GTK_PLOT_DATA(data), dz);
}

gdouble *
gtk_plot_surface_get_x(GtkPlotSurface *dataset, gint *nx)
{
  gint n;
  *nx = dataset->nx;
  return(gtk_plot_data_get_x(GTK_PLOT_DATA(dataset), &n));
}

gdouble *
gtk_plot_surface_get_y(GtkPlotSurface *dataset, gint *ny)
{
  gint n;
  *ny = dataset->ny;
  return(gtk_plot_data_get_y(GTK_PLOT_DATA(dataset), &n));
}

gdouble *
gtk_plot_surface_get_z(GtkPlotSurface *dataset, gint *nx, gint *ny)
{
  gint n;
  *nx = dataset->nx;
  *ny = dataset->ny;
  return(gtk_plot_data_get_z(GTK_PLOT_DATA(dataset), &n));
}

gdouble *
gtk_plot_surface_get_dz(GtkPlotSurface *dataset)
{
  gint n;
  return(gtk_plot_data_get_dz(GTK_PLOT_DATA(dataset), &n));
}

gdouble *
gtk_plot_surface_get_dx(GtkPlotSurface *dataset)
{
  gint n;
  return(gtk_plot_data_get_dx(GTK_PLOT_DATA(dataset), &n));
}

gdouble *
gtk_plot_surface_get_dy(GtkPlotSurface *dataset)
{
  gint n;
  return(gtk_plot_data_get_dy(GTK_PLOT_DATA(dataset), &n));
}

void
gtk_plot_surface_set_nx(GtkPlotSurface *dataset, gint nx)
{
  dataset->nx = nx;
}

void
gtk_plot_surface_set_ny(GtkPlotSurface *dataset, gint ny)
{
  dataset->ny = ny;
}

gint
gtk_plot_surface_get_nx(GtkPlotSurface *dataset)
{
  return(dataset->nx);
}

gint
gtk_plot_surface_get_ny(GtkPlotSurface *dataset)
{
  return(dataset->ny);
}

void
gtk_plot_surface_set_xstep(GtkPlotSurface *dataset, gdouble xstep)
{
  dataset->xstep = xstep;
}

void
gtk_plot_surface_set_ystep(GtkPlotSurface *dataset, gdouble ystep)
{
  dataset->ystep = ystep;
}

gdouble
gtk_plot_surface_get_xstep(GtkPlotSurface *dataset)
{
  return (dataset->xstep);
}

gdouble
gtk_plot_surface_get_ystep(GtkPlotSurface *dataset)
{
  return (dataset->ystep);
}

void
gtk_plot_surface_build_mesh(GtkPlotSurface *surface)
{
  GtkPlotData *data;
  GtkPlot *plot;

  data = GTK_PLOT_DATA(surface);
  if(!data->plot) return;
  plot = data->plot;

  if(data->is_function){
     gdouble xstep, ystep;
     gdouble xmin, xmax, ymin, ymax;
     gdouble x, y;
     gdouble *fx = NULL, *fy = NULL, *fz = NULL;
     gint nx, ny;
     gint npoints;

     xmin = GTK_PLOT(plot)->xmin;
     xmax = GTK_PLOT(plot)->xmax;
     ymin = GTK_PLOT(plot)->ymin;
     ymax = GTK_PLOT(plot)->ymax;

     xstep = surface->xstep;
     surface->nx = roundint((xmax - xmin) / xstep) + 1;

     ystep = surface->ystep;
     surface->ny = roundint((ymax - ymin) / ystep) + 1;

     npoints = surface->nx * surface->ny;
     fx = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));
     fy = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));
     fz = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));

     npoints = 0;
     y = ymin;
     for(ny = 0; ny < surface->ny; ny++)
       {
         x = xmin;
         for(nx = 0; nx < surface->nx; nx++)
          {
            gboolean error;
            fx[npoints] = x;
            fy[npoints] = y;
            fz[npoints] = data->function3d(plot, data, x, y, &error);

            x += xstep;
            npoints++;
          }
         y += ystep;
     }

     gtk_plot_data_set_x(data, fx);
     gtk_plot_data_set_y(data, fy);
     gtk_plot_data_set_z(data, fz);
     gtk_plot_data_set_numpoints(data, npoints);
     gtk_plot_surface_real_build_mesh(surface);

     g_free(fx);
     g_free(fy);
     g_free(fz);
  }
  else if(data->is_iterator){
     gdouble x, y, z, a, dx, dy, dz, da;
     gchar *label;
     gdouble *fx = NULL, *fy = NULL, *fz = NULL;
     gint iter;

     if(data->iterator_mask & GTK_PLOT_DATA_X)
       fx = g_new0(gdouble, data->num_points);
     if(data->iterator_mask & GTK_PLOT_DATA_Y)
       fy = g_new0(gdouble, data->num_points);
     if(data->iterator_mask & GTK_PLOT_DATA_Z)
       fz = g_new0(gdouble, data->num_points);

     for(iter = 0; iter < data->num_points; iter++)
       {
          gboolean error;
          data->iterator (plot, data, iter,
                          &x, &y, &z, &a, &dx, &dy, &dz, &da, &label, &error);

          if(error)
            {
               break;
            }
          else
            {
              if(data->iterator_mask & GTK_PLOT_DATA_X) fx[iter] = x;
              if(data->iterator_mask & GTK_PLOT_DATA_Y) fy[iter] = y;
              if(data->iterator_mask & GTK_PLOT_DATA_Z) fz[iter] = z;
            }
       } 

     gtk_plot_data_set_x(GTK_PLOT_DATA(surface), fx);
     gtk_plot_data_set_y(GTK_PLOT_DATA(surface), fy);
     gtk_plot_data_set_z(GTK_PLOT_DATA(surface), fz);

     gtk_plot_surface_real_build_mesh(surface);

     g_free(fx);
     g_free(fy);
     g_free(fz);
  }
  else   
     gtk_plot_surface_real_build_mesh(surface);
}

static void
gtk_plot_surface_real_build_mesh(GtkPlotSurface *surface)
{
  GtkPlotData *data;
  gdouble *array_x, *array_y, *array_z;
  GtkPlot *plot;
  gint i;

  data = GTK_PLOT_DATA(surface);
  if(!data->plot) return;
  plot = data->plot;

  if(data->num_points == 0) return;

  gtk_plot_dt_clear(surface->dt);

  array_x = gtk_plot_data_get_x(GTK_PLOT_DATA(surface), &i);
  array_y = gtk_plot_data_get_y(GTK_PLOT_DATA(surface), &i);
  array_z = gtk_plot_data_get_z(GTK_PLOT_DATA(surface), &i);

  for(i = 0; i < data->num_points; i++){
    GtkPlotDTnode node;
    node.x = array_x[i];
    node.y = array_y[i];
    node.z = 0.0;
    if(array_z) node.z = array_z[i];
    gtk_plot_dt_add_node(surface->dt, node);
  }
  gtk_plot_dt_triangulate(surface->dt);

  GTK_PLOT_SURFACE_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(surface)))->build_polygons(surface); 

  gtk_plot_surface_recalc_nodes(surface);
  surface->recalc_dt = FALSE;
}

static void
gtk_plot_surface_build_polygons(GtkPlotSurface *surface)
{
  GList *list;

  clear_polygons(surface);
  list = surface->dt->triangles;
  while(list){
    GtkPlotPolygon *polygon;
    GtkPlotDTtriangle *triangle = (GtkPlotDTtriangle *)list->data;

    polygon = g_new0(GtkPlotPolygon, 1);
    polygon->t = triangle;
    polygon->n = 3;
    polygon->cut_level = FALSE;
    polygon->xyz[0].x = triangle->na->x; 
    polygon->xyz[0].y = triangle->na->y; 
    polygon->xyz[0].z = triangle->na->z; 
    polygon->xyz[1].x = triangle->nb->x; 
    polygon->xyz[1].y = triangle->nb->y; 
    polygon->xyz[1].z = triangle->nb->z; 
    polygon->xyz[2].x = triangle->nc->x; 
    polygon->xyz[2].y = triangle->nc->y; 
    polygon->xyz[2].z = triangle->nc->z; 
    surface->polygons = g_list_append(surface->polygons, polygon);

    list = list->next;
  };
}

void
gtk_plot_surface_recalc_nodes(GtkPlotSurface *surface)
{
  GtkPlotData *data;
  GtkPlot *plot;
  GList *list;
  gint i;

  data = GTK_PLOT_DATA(surface);
  if(!data->plot) return;
  plot = data->plot;

  for(i= surface->dt->node_0; i < surface->dt->node_cnt; i++){
    GtkPlotDTnode *node;
    node = gtk_plot_dt_get_node(surface->dt,i);
    if(GTK_IS_PLOT3D(plot)){
      gtk_plot3d_get_pixel(GTK_PLOT3D(plot),
			   node->x, node->y, node->z,
			   &node->px, &node->py, &node->pz);
    } else {
      gtk_plot_get_pixel(plot,
			 node->x, node->y,
			 &node->px, &node->py);
      node->pz = 0.0;
    }
  }

  list = surface->polygons;
  while(list){
    GtkPlotPolygon *polygon = (GtkPlotPolygon *)list->data;
    
    for(i = 0; i < polygon->n; i++){
      if(GTK_IS_PLOT3D(plot)){
        gtk_plot3d_get_pixel(GTK_PLOT3D(plot),
  	            polygon->xyz[i].x, polygon->xyz[i].y, polygon->xyz[i].z,
                    &polygon->p[i].x, &polygon->p[i].y, &polygon->p[i].z);
      } else {
        gtk_plot_get_pixel(plot,
  			   polygon->xyz[i].x, polygon->xyz[i].y,
                           &polygon->p[i].x, &polygon->p[i].y);
        polygon->p[i].z = 0.0;
      }
    }

    list = list->next;
  };

  GTK_PLOT_SURFACE_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(surface)))->sort_polygons(surface); 
  
}

static void
gtk_plot_surface_sort_polygons(GtkPlotSurface *surface)
{
  if(surface->polygons)
    surface->polygons = g_list_sort(surface->polygons, (GCompareFunc)compare_func);
}

/* PAINTER'S COMPARISON */
static gint
compare_func (gpointer a, gpointer b)
{
  GtkPlotPolygon *pa, *pb;
  GtkPlotDTtriangle *ta, *tb;
  gint i, j;
  gint na[3], nb[3];
  gint naz[3], nbz[3];
  gdouble pax[3], pbx[3];
  gdouble pay[3], pby[3];
  gdouble paz[3], pbz[3];
  gint xg = 0, xl = 0;
  gint yg = 0, yl = 0;
  gint zg = 0, zl = 0;
  GtkPlotDTnode *nda[3], *ndb[3];
  gdouble cza = 0, czb = 0;
  
  pa = (GtkPlotPolygon *)a;
  pb = (GtkPlotPolygon *)b;

  ta = pa->t;
  tb = pb->t;

  if(ta == tb) {
    gdouble z1, z2;
    z1 = pa->p[0].z;
    z2 = pb->p[0].z;
    for(i = 1; i < pa->n; i++) z1 = MIN(z1, pa->p[i].z);
    for(i = 1; i < pb->n; i++) z2 = MIN(z2, pb->p[i].z);
    if(z1 == z2)
      return (pa->level > pb->level ? -1 : (pa->level == pb->level ? 0 : 1));
    else
      return (z2 > z1 ? -1 : 1);
  }

  naz[0] = nbz[0] = 0;
  naz[1] = nbz[1] = 1;
  naz[2] = nbz[2] = 2;

  paz[0] = ta->na->pz;
  paz[1] = ta->nb->pz;
  paz[2] = ta->nc->pz;
  pbz[0] = tb->na->pz;
  pbz[1] = tb->nb->pz;
  pbz[2] = tb->nc->pz;

/*
  paz[0] = pa->p[0].z;
  paz[1] = pa->p[1].z;
  paz[2] = pa->p[2].z;
  if(pa->n == 4) paz[0] = pa->p[3].z;
  pbz[0] = pb->p[0].z;
  pbz[1] = pb->p[1].z;
  pbz[2] = pb->p[2].z;
  if(pb->n == 4) pbz[0] = pb->p[3].z;
*/

  zg = zl = 0;
  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      if(paz[i] < pbz[j]) zg++;
      if(paz[i] > pbz[j]) zl++;
    }
  }
  if(zg == 9) return 1;
  if(zl == 9) return -1;

  na[0] = nb[0] = 0;
  na[1] = nb[1] = 1;
  na[2] = nb[2] = 2;

  pax[0] = ta->na->px;
  pax[1] = ta->nb->px;
  pax[2] = ta->nc->px;
  pbx[0] = tb->na->px;
  pbx[1] = tb->nb->px;
  pbx[2] = tb->nc->px;

/*
  pax[0] = pa->p[0].x;
  pax[1] = pa->p[1].x;
  pax[2] = pa->p[2].x;
  if(pa->n == 4) pax[0] = pa->p[3].x;
  pbx[0] = pb->p[0].x;
  pbx[1] = pb->p[1].x;
  pbx[2] = pb->p[2].x;
  if(pb->n == 4) pbx[0] = pb->p[3].x;
*/

  xg = xl = 0;
  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      if(pax[i] < pbx[j]) xg++;
      if(pax[i] > pbx[j]) xl++;
    }
  }
  if(xg == 9 || xl == 9){
    cza = (paz[0] + paz[1] + paz[2]);
    czb = (pbz[0] + pbz[1] + pbz[2]);
    return(czb > cza ? 1 : -1);
  }

  na[0] = nb[0] = 0;
  na[1] = nb[1] = 1;
  na[2] = nb[2] = 2;

  pay[0] = ta->na->py;
  pay[1] = ta->nb->py;
  pay[2] = ta->nc->py;
  pby[0] = tb->na->py;
  pby[1] = tb->nb->py;
  pby[2] = tb->nc->py;

/*
  pay[0] = pa->p[0].y;
  pay[1] = pa->p[1].y;
  pay[2] = pa->p[2].y;
  if(pa->n == 4) pay[0] = pa->p[3].y;
  pby[0] = pb->p[0].y;
  pby[1] = pb->p[1].y;
  pby[2] = pb->p[2].y;
  if(pb->n == 4) pby[0] = pb->p[3].y;
*/

  yg = yl = 0;
  for(i = 0; i < 3; i++){
    for(j = 0; j < 3; j++){
      if(pay[i] < pby[j]) yg++;
      if(pay[i] > pby[j]) yl++;
    }
  }
  if(yg == 9 || yl == 9){
    cza = (paz[0] + paz[1] + paz[2]);
    czb = (pbz[0] + pbz[1] + pbz[2]);
    return(czb > cza ? 1 : -1);
  }

  nda[0] = ta->na;
  nda[1] = ta->nb;
  nda[2] = ta->nc;
  ndb[0] = tb->na;
  ndb[1] = tb->nb;
  ndb[2] = tb->nc;

/*
  for(i = 0; i < 3; i++){
    gdouble n1[3], n2[3];
    gdouble t1, t2, det;
    gdouble x1, x2;
    gdouble y1, y2;
    gdouble z1, z2;
    gint ia0 = naz[i];
    gint ja0 = naz[i == 2 ? 0 : i+1];
    n1[0] = pax[ja0]-pax[ia0];
    n1[1] = pay[ja0]-pay[ia0];
    n1[2] = paz[ja0]-paz[ia0];

    for(j = 0; j < 3; j++){
      gint ib0 = nbz[j];
      gint jb0 = nbz[j == 2 ? 0 : j+1];
      gdouble c1, c2;

      c1 = (pax[ia0]-pbx[ib0]);
      c2 = (pay[ia0]-pby[ib0]);

      n2[0] = pbx[jb0]-pbx[ib0];
      n2[1] = pby[jb0]-pby[ib0];
      n2[2] = pbz[jb0]-pbz[ib0];
      det = (-n1[0]*n2[1]+n1[1]*n2[0]);
      if(fabs(det) < 1.e-5){
        coincide++;
        if(coincide == 2) return 0;
      }
    }
  }      
*/
  for(i = 0; i < 3; i++){
    gdouble n1[3], n2[3];
    gdouble t1, t2, det;
    gdouble x1, x2;
    gdouble y1, y2;
    gdouble z1, z2;
    gint ia0 = naz[i];
    gint ja0 = naz[i == 2 ? 0 : i+1];
    n1[0] = pax[ja0]-pax[ia0];
    n1[1] = pay[ja0]-pay[ia0];
    n1[2] = paz[ja0]-paz[ia0];

    for(j = 0; j < 3; j++){
      gint ib0 = nbz[j];
      gint jb0 = nbz[j == 2 ? 0 : j+1];
      gdouble c1, c2;

      if(nda[ia0] == ndb[ib0] || nda[ia0] == ndb[jb0]) continue;
      if(nda[ja0] == ndb[ib0] || nda[ja0] == ndb[jb0]) continue;
      c1 = (pax[ia0]-pbx[ib0]);
      c2 = (pay[ia0]-pby[ib0]);

      n2[0] = pbx[jb0]-pbx[ib0];
      n2[1] = pby[jb0]-pby[ib0];
      n2[2] = pbz[jb0]-pbz[ib0];
      det = (-n1[0]*n2[1]+n1[1]*n2[0]);
      if(fabs(det) > 1.e-5){
        t1 = (c1*n2[1]-c2*n2[0]) / det;
        t2 = (-n1[0]*c2+n1[1]*c1) / det;
        if(t1 < -0.0001 || t2 < -0.0001) continue;
        if(t1 > 1.0001 || t2 > 1.0001) continue;
        x1 = pax[ia0] + t1 * n1[0];
        y1 = pay[ia0] + t1 * n1[1];
        z1 = paz[ia0] + t1 * n1[2];
        x2 = pbx[ib0] + t2 * n2[0];
        y2 = pby[ib0] + t2 * n2[1];
        z2 = pbz[ib0] + t2 * n2[2];
        if(z1 < z2) return 1;
        if(z2 < z1) return -1;
      }
    }
  }

  cza = (paz[0] + paz[1] + paz[2]);
  czb = (pbz[0] + pbz[1] + pbz[2]);
  return(czb > cza ? 1 : -1);

  return 0;
}


