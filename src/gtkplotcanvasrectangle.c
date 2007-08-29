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
#include "gtkplotcanvasrectangle.h"
#include "gtkplotgdk.h"
#include "gtkplotps.h"

#define P_(string) string

enum {
  ARG_0,
  ARG_LINE,
  ARG_FILLED,
  ARG_BORDER,
  ARG_SHADOW_WIDTH,
  ARG_BG
};


static void gtk_plot_canvas_rectangle_init	(GtkPlotCanvasRectangle *rectangle);
static void gtk_plot_canvas_rectangle_class_init(GtkPlotCanvasChildClass *klass);
static void gtk_plot_canvas_rectangle_draw 	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_rectangle_move	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x, gdouble y);
static void gtk_plot_canvas_rectangle_resize	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x1, gdouble y1,
						 gdouble x2, gdouble y2);
static void gtk_plot_canvas_rectangle_get_property(GObject      *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_canvas_rectangle_set_property(GObject      *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);

extern inline gint roundint                     (gdouble x);
static GtkPlotCanvasChildClass *parent_class = NULL;

GtkType
gtk_plot_canvas_rectangle_get_type (void)
{
  static GtkType plot_canvas_rectangle_type = 0;

  if (!plot_canvas_rectangle_type)
    {
      GtkTypeInfo plot_canvas_rectangle_info =
      {
	"GtkPlotCanvasRectangle",
	sizeof (GtkPlotCanvasRectangle),
	sizeof (GtkPlotCanvasRectangleClass),
	(GtkClassInitFunc) gtk_plot_canvas_rectangle_class_init,
	(GtkObjectInitFunc) gtk_plot_canvas_rectangle_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      plot_canvas_rectangle_type = gtk_type_unique (gtk_plot_canvas_child_get_type(), &plot_canvas_rectangle_info);
    }
  return plot_canvas_rectangle_type;
}

static void
gtk_plot_canvas_rectangle_get_property (GObject      *object,
                                    guint            prop_id,
                                    GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasRectangle *rectangle = GTK_PLOT_CANVAS_RECTANGLE (object);

  switch(prop_id){
    case ARG_LINE:
      g_value_set_pointer(value, &rectangle->line);
      break;
    case ARG_FILLED:
      g_value_set_boolean(value, rectangle->filled); 
      break;
    case ARG_BORDER:
      g_value_set_int(value, rectangle->border);
      break;
    case ARG_SHADOW_WIDTH:
      g_value_set_int(value, rectangle->shadow_width); 
      break;
    case ARG_BG:
      g_value_set_pointer(value, &rectangle->bg); 
      break;
  }
}

static void
gtk_plot_canvas_rectangle_set_property (GObject      *object,
                                    guint            prop_id,
                                    const GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasRectangle *rectangle = GTK_PLOT_CANVAS_RECTANGLE (object);

  switch(prop_id){
    case ARG_LINE:
      rectangle->line = *((GtkPlotLine *)g_value_get_pointer(value));
      break;
    case ARG_FILLED:
      rectangle->filled = g_value_get_boolean(value);
      break;
    case ARG_BORDER:
      rectangle->border = g_value_get_int(value);
      break;
    case ARG_SHADOW_WIDTH:
      rectangle->shadow_width = g_value_get_int(value);
      break;
    case ARG_BG:
      rectangle->bg = *((GdkColor *)g_value_get_pointer(value));
      break;
  }
}

GtkPlotCanvasChild*
gtk_plot_canvas_rectangle_new (GtkPlotLineStyle style,
                          gfloat width,
                          const GdkColor *fg,
                          const GdkColor *bg,
			  GtkPlotBorderStyle border,
                          gboolean fill)
{
  GtkPlotCanvasRectangle *rectangle;
                                                                                
  rectangle = gtk_type_new (gtk_plot_canvas_rectangle_get_type ());
                                   
  rectangle->line.line_width = width;                                             
  if(fg) rectangle->line.color = *fg;
  if(bg) rectangle->bg = *bg;
  rectangle->border = border;
  rectangle->filled = fill;
                                                                                
  return GTK_PLOT_CANVAS_CHILD (rectangle);
}

static void
gtk_plot_canvas_rectangle_init (GtkPlotCanvasRectangle *rectangle)
{
  gdk_color_black(gdk_colormap_get_system(), &rectangle->line.color);
  gdk_color_white(gdk_colormap_get_system(), &rectangle->bg);

  rectangle->line.line_style = GTK_PLOT_LINE_SOLID;
  rectangle->line.line_width = 0;
  rectangle->shadow_width = 3;
  rectangle->border = GTK_PLOT_BORDER_LINE;
  rectangle->filled = TRUE;
}

static void
gtk_plot_canvas_rectangle_class_init (GtkPlotCanvasChildClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  parent_class = gtk_type_class (gtk_plot_canvas_child_get_type ());

  klass->draw = gtk_plot_canvas_rectangle_draw; 
  klass->move = gtk_plot_canvas_rectangle_move; 
  klass->move_resize = gtk_plot_canvas_rectangle_resize; 

  gobject_class->get_property = gtk_plot_canvas_rectangle_get_property;
  gobject_class->set_property = gtk_plot_canvas_rectangle_set_property;
                                    
  g_object_class_install_property (gobject_class,
                           ARG_LINE,
  g_param_spec_pointer ("line",
                           P_("Line Attributes"),
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
                           ARG_BORDER,
  g_param_spec_int ("border",
                           P_("Border"),
                           P_("Border Width"),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHADOW_WIDTH,
  g_param_spec_int ("shadow_width",
                           P_("Shadow Width"),
                           P_("Shadow Width"),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_BG,
  g_param_spec_pointer ("color_bg",
                           P_("Filling Color"),
                           P_("Filling Color"),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
}

static void 
gtk_plot_canvas_rectangle_draw 		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child)
{
  GtkPlotCanvasRectangle *rectangle = GTK_PLOT_CANVAS_RECTANGLE(child);
  gint width = child->allocation.width;
  gint height = child->allocation.height;
  gdouble m = canvas->magnification;

  if(width == 0 && height == 0) return;

  if(rectangle->filled){
     gtk_plot_pc_set_color(canvas->pc, &rectangle->bg);
     gtk_plot_pc_draw_rectangle(canvas->pc, TRUE,
                                child->allocation.x, child->allocation.y, 
				width, height);
  }
  if(rectangle->line.line_style != GTK_PLOT_LINE_NONE &&
     rectangle->border != GTK_PLOT_BORDER_NONE){
                                                                          
      gtk_plot_canvas_set_line_attributes(canvas, rectangle->line);
      gtk_plot_pc_draw_rectangle(canvas->pc, FALSE,
                                 child->allocation.x, child->allocation.y,
                                 width, height);
      if(rectangle->border == GTK_PLOT_BORDER_SHADOW){
        gtk_plot_pc_draw_rectangle(canvas->pc,
                           TRUE,
                           child->allocation.x + roundint(rectangle->shadow_width * m),
                           child->allocation.y + height,
                           width, roundint(rectangle->shadow_width * m));
        gtk_plot_pc_draw_rectangle(canvas->pc,
                           TRUE,
                           child->allocation.x + width,
                           child->allocation.y + roundint(rectangle->shadow_width * m),
                           roundint(rectangle->shadow_width * m), height);            }
  }
}

static void 
gtk_plot_canvas_rectangle_move		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble x, gdouble y)
{
  return;
}

static void 
gtk_plot_canvas_rectangle_resize	(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble x1, gdouble y1,
					 gdouble x2, gdouble y2)
{
  return;
}

void
gtk_plot_canvas_rectangle_set_attributes(GtkPlotCanvasRectangle *rectangle,
                                    	 GtkPlotLineStyle style,
                                     	 gfloat width,
                                         const GdkColor *fg,
                                         const GdkColor *bg,
                                         GtkPlotBorderStyle border,
					 gboolean fill)
{
  if(fg) rectangle->line.color = *fg;
  if(bg) rectangle->bg = *bg;
  rectangle->line.line_width = width;
  rectangle->line.line_style = style;
  rectangle->border = border;
  rectangle->filled = fill;
}


