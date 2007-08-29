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
#include "gtkplotcanvasellipse.h"
#include "gtkplotgdk.h"
#include "gtkplotps.h"

#define DEFAULT_MARKER_SIZE 6
#define P_(string) string

enum {
  ARG_0,
  ARG_LINE,
  ARG_FILLED,
  ARG_BG
};

static void gtk_plot_canvas_ellipse_init	(GtkPlotCanvasEllipse *ellipse);
static void gtk_plot_canvas_ellipse_class_init  (GtkPlotCanvasChildClass *klass);
static void gtk_plot_canvas_ellipse_draw 	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_ellipse_select	(GtkPlotCanvas *canvas, 
						 GtkPlotCanvasChild *child, 
						 GtkAllocation area);
static void gtk_plot_canvas_ellipse_move	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x, gdouble y);
static void gtk_plot_canvas_ellipse_resize	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x1, gdouble y1,
						 gdouble x2, gdouble y2);
static void gtk_plot_canvas_ellipse_get_property(GObject      *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_canvas_ellipse_set_property(GObject      *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);


extern inline gint roundint                     (gdouble x);
static GtkPlotCanvasChildClass *parent_class = NULL;

GtkType
gtk_plot_canvas_ellipse_get_type (void)
{
  static GtkType plot_canvas_ellipse_type = 0;

  if (!plot_canvas_ellipse_type)
    {
      GtkTypeInfo plot_canvas_ellipse_info =
      {
	"GtkPlotCanvasEllipse",
	sizeof (GtkPlotCanvasEllipse),
	sizeof (GtkPlotCanvasEllipseClass),
	(GtkClassInitFunc) gtk_plot_canvas_ellipse_class_init,
	(GtkObjectInitFunc) gtk_plot_canvas_ellipse_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      plot_canvas_ellipse_type = gtk_type_unique (gtk_plot_canvas_child_get_type(), &plot_canvas_ellipse_info);
    }
  return plot_canvas_ellipse_type;
}

GtkPlotCanvasChild*
gtk_plot_canvas_ellipse_new (GtkPlotLineStyle style,
                          gfloat width,
                          const GdkColor *fg,
                          const GdkColor *bg,
                          gboolean fill)
{
  GtkPlotCanvasEllipse *ellipse;
                                                                                
  ellipse = gtk_type_new (gtk_plot_canvas_ellipse_get_type ());
                                   
  ellipse->line.line_width = width;                                             
  if(fg) ellipse->line.color = *fg;
  if(bg) ellipse->bg = *bg;
  ellipse->filled = fill;
                                                                                
  return GTK_PLOT_CANVAS_CHILD (ellipse);
}

static void
gtk_plot_canvas_ellipse_init (GtkPlotCanvasEllipse *ellipse)
{
  gdk_color_black(gdk_colormap_get_system(), &ellipse->line.color);
  gdk_color_white(gdk_colormap_get_system(), &ellipse->bg);

  ellipse->line.line_style = GTK_PLOT_LINE_SOLID;
  ellipse->line.line_width = 0;
  ellipse->filled = TRUE;
}

static void
gtk_plot_canvas_ellipse_class_init (GtkPlotCanvasChildClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  parent_class = gtk_type_class (gtk_plot_canvas_child_get_type ());

  klass->draw = gtk_plot_canvas_ellipse_draw; 
  klass->move = gtk_plot_canvas_ellipse_move; 
  klass->move_resize = gtk_plot_canvas_ellipse_resize; 
  klass->draw_selection = gtk_plot_canvas_ellipse_select; 

  gobject_class->get_property = gtk_plot_canvas_ellipse_get_property;
  gobject_class->set_property = gtk_plot_canvas_ellipse_set_property;
                           
  g_object_class_install_property (gobject_class,
                           ARG_LINE,
  g_param_spec_pointer ("line",
                           P_("Line"),
                           P_("Line Attributes"),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_FILLED,
  g_param_spec_boolean ("filled",
                           P_("Filled"),
                           P_("Fill Figure"),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_BG,
  g_param_spec_pointer ("color_bg",
                           P_("Filling Color"),
                           P_("Filling Color"),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));

}

static void
gtk_plot_canvas_ellipse_get_property (GObject      *object,
                                    guint            prop_id,
                                    GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasEllipse *ellipse = GTK_PLOT_CANVAS_ELLIPSE (object);
                                                                                
  switch(prop_id){
    case ARG_LINE:
      g_value_set_pointer(value, &ellipse->line);
      break;
    case ARG_FILLED:
      g_value_set_boolean(value, ellipse->filled);
      break;
    case ARG_BG:
      g_value_set_pointer(value, &ellipse->bg);
      break;
  }
}
                                                                                
static void
gtk_plot_canvas_ellipse_set_property (GObject      *object,
                                    guint            prop_id,
                                    const GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasEllipse *ellipse = GTK_PLOT_CANVAS_ELLIPSE (object);
                                                                                
  switch(prop_id){
    case ARG_LINE:
      ellipse->line = *((GtkPlotLine *)g_value_get_pointer(value));
      break;
    case ARG_FILLED:
      ellipse->filled = g_value_get_boolean(value);
      break;
    case ARG_BG:
      ellipse->bg = *((GdkColor *)g_value_get_pointer(value));
      break;
  }
}

static void 
gtk_plot_canvas_ellipse_draw 		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child)
{
  GtkPlotCanvasEllipse *ellipse = GTK_PLOT_CANVAS_ELLIPSE(child);
  gint width = child->allocation.width;
  gint height = child->allocation.height;

  if(width == 0 && height == 0) return;

  if(ellipse->filled){
     gtk_plot_pc_set_color(canvas->pc, &ellipse->bg);
     gtk_plot_pc_draw_ellipse(canvas->pc, TRUE,
                              child->allocation.x, child->allocation.y, 
                              width, height);
  }
  gtk_plot_canvas_set_line_attributes(canvas, ellipse->line);
  if(ellipse->line.line_style != GTK_PLOT_LINE_NONE)
     gtk_plot_pc_draw_ellipse(canvas->pc, FALSE,
                              child->allocation.x, child->allocation.y, 
                              width, height);

}

static void
draw_marker(GtkPlotCanvas *canvas, GdkGC *gc, gint x, gint y)
{
  GdkDrawable *darea;
                                                                                
  darea = GTK_WIDGET(canvas)->window;
                                                                                
  gdk_draw_rectangle(darea, gc, TRUE,
                     x - DEFAULT_MARKER_SIZE / 2, y - DEFAULT_MARKER_SIZE / 2,
                     DEFAULT_MARKER_SIZE + 1, DEFAULT_MARKER_SIZE + 1);
}

static void
gtk_plot_canvas_ellipse_select(GtkPlotCanvas *canvas, GtkPlotCanvasChild *child, GtkAllocation area)
{
  GdkGC *xor_gc = NULL;
  GdkGCValues values;
  
  gdk_gc_get_values(GTK_WIDGET(canvas)->style->fg_gc[0], &values);
  values.function = GDK_INVERT;
  values.foreground = GTK_WIDGET(canvas)->style->white;
  values.subwindow_mode = GDK_INCLUDE_INFERIORS;
  xor_gc = gdk_gc_new_with_values(GTK_WIDGET(canvas)->window,
                                  &values,
                                  GDK_GC_FOREGROUND |
                                  GDK_GC_FUNCTION |
                                  GDK_GC_SUBWINDOW);

  gdk_draw_rectangle (GTK_WIDGET(canvas)->window,
                      xor_gc,
                      FALSE,
                      area.x, area.y,
                      area.width, area.height);
  draw_marker(canvas, xor_gc, area.x, area.y);
  draw_marker(canvas, xor_gc, area.x, area.y + area.height);
  draw_marker(canvas, xor_gc, area.x + area.width, area.y);
  draw_marker(canvas, xor_gc, area.x + area.width, area.y + area.height);
  if(area.height > DEFAULT_MARKER_SIZE * 2){
    draw_marker(canvas, xor_gc, area.x, area.y + area.height / 2);
    draw_marker(canvas, xor_gc, area.x + area.width,
                                area.y + area.height / 2);
  }
  if(area.width > DEFAULT_MARKER_SIZE * 2){
    draw_marker(canvas, xor_gc, area.x + area.width / 2, area.y);
    draw_marker(canvas, xor_gc, area.x + area.width / 2,
                                area.y + area.height);
  }

  gdk_gc_set_line_attributes(xor_gc, 1, 1, 0, 0);
  gdk_draw_arc (GTK_WIDGET(canvas)->window, xor_gc,
                FALSE,
                roundint(area.x), roundint(area.y),
                roundint(area.width), roundint(area.height), 0, 25000);

  if(xor_gc) gdk_gc_unref(xor_gc);
}


static void 
gtk_plot_canvas_ellipse_move		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble x, gdouble y)
{
  return;
}

static void 
gtk_plot_canvas_ellipse_resize		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble x1, gdouble y1, 
					 gdouble x2, gdouble y2)
{
  return;
}

void
gtk_plot_canvas_ellipse_set_attributes	(GtkPlotCanvasEllipse *ellipse,
                                    	 GtkPlotLineStyle style,
					 gdouble width,
                                         const GdkColor *fg,
                                         const GdkColor *bg,
                                         gboolean fill)
{
  if(fg) ellipse->line.color = *fg;
  if(bg) ellipse->bg = *bg;
  ellipse->line.line_width = width;
  ellipse->line.line_style = style;
  ellipse->filled = fill;
}

