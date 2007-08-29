/* gtkplotcanvas - gtkplot canvas widget for gtk+
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
#include "gtkplotcanvas.h"
#include "gtkplotcanvaspixmap.h"
#include "gtkplotgdk.h"
#include "gtkplotps.h"

#define P_(string) string

enum {
  ARG_0,
  ARG_PIXMAP,
  ARG_MASK,
};

static void gtk_plot_canvas_pixmap_init	(GtkPlotCanvasPixmap *pixmap);
static void gtk_plot_canvas_pixmap_class_init(GtkPlotCanvasChildClass *klass);
static void gtk_plot_canvas_pixmap_destroy	(GtkObject *object);
static void gtk_plot_canvas_pixmap_draw 	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_pixmap_move	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x, gdouble y);
static void gtk_plot_canvas_pixmap_resize	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x1, gdouble y1,
						 gdouble x2, gdouble y2);
static void gtk_plot_canvas_pixmap_get_property(GObject      *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_canvas_pixmap_set_property(GObject      *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);

static GtkPlotCanvasChildClass *parent_class = NULL;

GtkType
gtk_plot_canvas_pixmap_get_type (void)
{
  static GtkType plot_canvas_pixmap_type = 0;

  if (!plot_canvas_pixmap_type)
    {
      GtkTypeInfo plot_canvas_pixmap_info =
      {
	"GtkPlotCanvasPixmap",
	sizeof (GtkPlotCanvasPixmap),
	sizeof (GtkPlotCanvasPixmapClass),
	(GtkClassInitFunc) gtk_plot_canvas_pixmap_class_init,
	(GtkObjectInitFunc) gtk_plot_canvas_pixmap_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      plot_canvas_pixmap_type = gtk_type_unique (gtk_plot_canvas_child_get_type(), &plot_canvas_pixmap_info);
    }
  return plot_canvas_pixmap_type;
}

GtkPlotCanvasChild*
gtk_plot_canvas_pixmap_new (GdkPixmap *_pixmap, GdkBitmap *mask)
{
  GtkPlotCanvasPixmap *pixmap;
                                                                                
  pixmap = gtk_type_new (gtk_plot_canvas_pixmap_get_type ());

  pixmap->pixmap = _pixmap;
  pixmap->mask = mask;

  if(_pixmap) gdk_pixmap_ref(_pixmap);
  if(mask) gdk_bitmap_ref(mask);
                                                                                
  return GTK_PLOT_CANVAS_CHILD (pixmap);
}

static void
gtk_plot_canvas_pixmap_init (GtkPlotCanvasPixmap *pixmap)
{
  pixmap->pixmap = NULL;
  pixmap->mask = NULL;
}

static void
gtk_plot_canvas_pixmap_destroy(GtkObject *object)
{
  GtkPlotCanvasPixmap *pixmap = GTK_PLOT_CANVAS_PIXMAP(object);

  if(pixmap->pixmap) gdk_pixmap_unref(pixmap->pixmap);
  if(pixmap->mask) gdk_bitmap_unref(pixmap->mask);
  pixmap->pixmap = NULL;
  pixmap->mask = NULL;
}

static void
gtk_plot_canvas_pixmap_class_init (GtkPlotCanvasChildClass *klass)
{
  GtkObjectClass *object_class = (GtkObjectClass *)klass;
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  parent_class = gtk_type_class (gtk_plot_canvas_child_get_type ());

  object_class->destroy = gtk_plot_canvas_pixmap_destroy;

  gobject_class->get_property = gtk_plot_canvas_pixmap_get_property;
  gobject_class->set_property = gtk_plot_canvas_pixmap_set_property;

  g_object_class_install_property (gobject_class,
                           ARG_PIXMAP,
  g_param_spec_pointer ("pixmap",
                           P_("Pixmap"),
                           P_("Pixmap"),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_MASK,
  g_param_spec_pointer ("mask_bitmap",
                           P_("Mask"),
                           P_("Mask"),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));

  klass->draw = gtk_plot_canvas_pixmap_draw; 
  klass->move = gtk_plot_canvas_pixmap_move; 
  klass->move_resize = gtk_plot_canvas_pixmap_resize; 
}

static void
gtk_plot_canvas_pixmap_get_property (GObject      *object,
                                    guint            prop_id,
                                    GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasPixmap *pixmap = GTK_PLOT_CANVAS_PIXMAP (object);
                                                                                
  switch(prop_id){
    case ARG_PIXMAP:
      g_value_set_pointer(value, pixmap->pixmap);
      break;
    case ARG_MASK:
      g_value_set_pointer(value, pixmap->mask);
      break;
  }
}
                                                                                
static void
gtk_plot_canvas_pixmap_set_property (GObject      *object,
                                    guint            prop_id,
                                    const GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasPixmap *pixmap = GTK_PLOT_CANVAS_PIXMAP (object);
                                                                                
  switch(prop_id){
    case ARG_PIXMAP:
      if(pixmap->pixmap) gdk_pixmap_unref(pixmap->pixmap);
      pixmap->pixmap = (GdkPixmap *)g_value_get_pointer(value);
      if(pixmap->pixmap) gdk_pixmap_ref(pixmap->pixmap);
      break;
    case ARG_MASK:
      if(pixmap->mask) gdk_bitmap_unref(pixmap->mask);
      pixmap->mask = (GdkBitmap *)g_value_get_pointer(value);
      if(pixmap->mask) gdk_bitmap_ref(pixmap->mask);
      break;
  }
}

static void 
gtk_plot_canvas_pixmap_draw 		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child)
{
  GtkPlotCanvasPixmap *pixmap = GTK_PLOT_CANVAS_PIXMAP(child);
 
  g_return_if_fail(GTK_WIDGET_VISIBLE(GTK_WIDGET(canvas)));
 
  if(pixmap->pixmap){
    gdouble scale_x, scale_y;
    gint width, height;

    gdk_window_get_size(pixmap->pixmap, &width, &height);
    scale_x = (gdouble)child->allocation.width / (gdouble)width;
    scale_y = (gdouble)child->allocation.height / (gdouble)height;

    gtk_plot_pc_draw_pixmap(canvas->pc, pixmap->pixmap, pixmap->mask,
                            0, 0,
                            child->allocation.x,
                            child->allocation.y,
                            width,
                            height,
                            scale_x, scale_y);

  } else {
    GdkColormap *colormap = gdk_colormap_get_system();
    GdkColor black, white;

    gdk_color_black(colormap, &black);
    gdk_color_white(colormap, &white);
                                                                          
    gtk_plot_pc_set_color(canvas->pc, &white);
    gtk_plot_pc_draw_rectangle(canvas->pc, TRUE,
                         child->allocation.x, child->allocation.y,
                         child->allocation.width, child->allocation.height);
    gtk_plot_pc_set_color(canvas->pc, &black);
    gtk_plot_pc_draw_rectangle(canvas->pc, FALSE,
                         child->allocation.x, child->allocation.y,
                         child->allocation.width, child->allocation.height);
  }
}

static void 
gtk_plot_canvas_pixmap_move		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble x, gdouble y)
{
  return;
}

static void 
gtk_plot_canvas_pixmap_resize	(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble x1, gdouble y1,
					 gdouble x2, gdouble y2)
{
  return;
}

