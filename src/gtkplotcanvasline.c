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
#include "gtkplotcanvasline.h"
#include "gtkplotgdk.h"
#include "gtkplotps.h"

#define DEFAULT_MARKER_SIZE 6
#define P(string) string

enum{
  ARG_0,
  ARG_LINE,
  ARG_X1,
  ARG_Y1,
  ARG_X2,
  ARG_Y2,
  ARG_POS,
  ARG_ARROW_MASK,
  ARG_ARROW_LENGTH,
  ARG_ARROW_WIDTH,
  ARG_ARROW_STYLE
};

static void gtk_plot_canvas_line_init		(GtkPlotCanvasLine *line);
static void gtk_plot_canvas_line_class_init	(GtkPlotCanvasChildClass *klass);
static void gtk_plot_canvas_line_draw_selection (GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
					 	 GtkAllocation area);
static void gtk_plot_canvas_line_draw 		(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_line_move		(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x, gdouble y);
static void gtk_plot_canvas_line_resize		(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x1, gdouble y1,
						 gdouble x2, gdouble y2);
static GtkPlotCanvasPos gtk_plot_canvas_line_button_press   
						(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gint x, gint y);
static void gtk_plot_canvas_line_button_release (GtkPlotCanvas *canvas,
                                                 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_line_size_allocate  (GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_line_get_property(GObject      *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_canvas_line_set_property(GObject      *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);


static GtkPlotCanvasChildClass *parent_class = NULL;

GtkType
gtk_plot_canvas_line_get_type (void)
{
  static GtkType plot_canvas_line_type = 0;

  if (!plot_canvas_line_type)
    {
      GtkTypeInfo plot_canvas_line_info =
      {
	"GtkPlotCanvasLine",
	sizeof (GtkPlotCanvasLine),
	sizeof (GtkPlotCanvasLineClass),
	(GtkClassInitFunc) gtk_plot_canvas_line_class_init,
	(GtkObjectInitFunc) gtk_plot_canvas_line_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      plot_canvas_line_type = gtk_type_unique (gtk_plot_canvas_child_get_type(), &plot_canvas_line_info);
    }
  return plot_canvas_line_type;
}

GtkPlotCanvasChild*
gtk_plot_canvas_line_new (GtkPlotLineStyle style,
                          gfloat width,
                          const GdkColor *color,
                          GtkPlotCanvasArrow arrow_mask)
{
  GtkPlotCanvasLine *line;
                                                                                
  line = gtk_type_new (gtk_plot_canvas_line_get_type ());
                                   
  line->line.line_width = width;                                             
  if(color) line->line.color = *color;
  line->arrow_mask = arrow_mask;
                                                                                
  return GTK_PLOT_CANVAS_CHILD (line);
}

static void
gtk_plot_canvas_line_init (GtkPlotCanvasLine *line)
{
  gdk_color_black(gdk_colormap_get_system(), &line->line.color);
                                                    
  line->line.line_style = GTK_PLOT_LINE_SOLID;                            
  line->line.line_width = 0;                            
  line->arrow_length = 8;
  line->arrow_width = 8;
  line->arrow_style = GTK_PLOT_SYMBOL_FILLED;
  line->pos = GTK_PLOT_CANVAS_OUT;
  line->x1 = line->y1 = 0;
  line->x2 = line->y2 = 0;
}

static void
gtk_plot_canvas_line_class_init (GtkPlotCanvasChildClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  parent_class = gtk_type_class (gtk_plot_canvas_child_get_type ());

  klass->draw = gtk_plot_canvas_line_draw; 
  klass->draw_selection = gtk_plot_canvas_line_draw_selection; 
  klass->move = gtk_plot_canvas_line_move; 
  klass->move_resize = gtk_plot_canvas_line_resize; 
  klass->size_allocate = gtk_plot_canvas_line_size_allocate; 
  klass->button_press = gtk_plot_canvas_line_button_press; 
  klass->button_release = gtk_plot_canvas_line_button_release;

  gobject_class->get_property = gtk_plot_canvas_line_get_property;
  gobject_class->set_property = gtk_plot_canvas_line_set_property;
                                                                                
  g_object_class_install_property (gobject_class,
                           ARG_LINE,
  g_param_spec_pointer ("line",
                           P("Line Attributes"),
                           P("Line Attributes"),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_X1,
  g_param_spec_double ("x1",
                           P("Origin X"),
                           P("Coordinate X of the first point"),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_Y1,
  g_param_spec_double ("y1",
                           P("Origin Y"),
                           P("Coordinate Y of the first point"),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_X2,
  g_param_spec_double ("x2",
                           P("End X"),
                           P("Coordinate X of the second point"),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_Y2,
  g_param_spec_double ("y2",
                           P("End Y"),
                           P("Coordinate Y of the second point"),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_POS,
  g_param_spec_int ("pos",
                           P(""),
                           P(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ARROW_MASK,
  g_param_spec_int ("arrow_mask",
                           P("Arrow Mask"),
                           P("Arrow Mask"),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ARROW_WIDTH,
  g_param_spec_int ("arrow_width",
                           P("Arrow Width"),
                           P("Arrow Width"),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ARROW_LENGTH,
  g_param_spec_int ("arrow_length",
                           P("Arrow Length"),
                           P("Arrow Length"),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ARROW_STYLE,
  g_param_spec_int ("arrow_style",
                           P("Arrow Style"),
                           P("Arrow Style"),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
}

static void
gtk_plot_canvas_line_set_property (GObject      *object,
                                    guint            prop_id,
                                    const GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasLine *line = GTK_PLOT_CANVAS_LINE (object);
                                                                                
  switch(prop_id){
    case ARG_LINE:
      line->line = *((GtkPlotLine *)g_value_get_pointer(value));
      break;
    case ARG_X1:
      line->x1 = g_value_get_double(value);
      break;
    case ARG_Y1:
      line->y1 = g_value_get_double(value);
      break;
    case ARG_X2:
      line->x2 = g_value_get_double(value);
      break;
    case ARG_Y2:
      line->y2 = g_value_get_double(value);
      break;
    case ARG_POS:
      line->pos = g_value_get_int(value);
      break;
    case ARG_ARROW_MASK:
      line->arrow_mask = g_value_get_int(value);
      break;
    case ARG_ARROW_LENGTH:
      line->arrow_length = g_value_get_int(value);
      break;
    case ARG_ARROW_WIDTH:
      line->arrow_width = g_value_get_int(value);
      break;
    case ARG_ARROW_STYLE:
      line->arrow_style = g_value_get_int(value);
      break;
  }
}

static void
gtk_plot_canvas_line_get_property (GObject      *object,
                                    guint            prop_id,
                                    GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasLine *line = GTK_PLOT_CANVAS_LINE (object);
                                                                                
  switch(prop_id){
    case ARG_LINE:
      g_value_set_pointer(value, &line->line);
      break;
    case ARG_X1:
      g_value_set_double(value, line->x1);
      break;
    case ARG_Y1:
      g_value_set_double(value, line->y1); 
      break;
    case ARG_X2:
      g_value_set_double(value, line->x2); 
      break;
    case ARG_Y2:
      g_value_set_double(value, line->y2); 
      break;
    case ARG_POS:
      g_value_set_int(value, line->pos);
      break;
    case ARG_ARROW_MASK:
      g_value_set_int(value, line->arrow_mask);
      break;
    case ARG_ARROW_LENGTH:
      g_value_set_int(value, line->arrow_length);
      break;
    case ARG_ARROW_WIDTH:
      g_value_set_int(value, line->arrow_width);
      break;
    case ARG_ARROW_STYLE:
      g_value_set_int(value, line->arrow_style);
      break;
  }
}

static void 
gtk_plot_canvas_line_draw 		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child)
{
  GtkPlotCanvasLine *line = GTK_PLOT_CANVAS_LINE(child);
  gdouble angle = 0;
  gint width = child->allocation.width;
  gint height = child->allocation.height;
  gint rx1, rx2, ry1, ry2;
  GtkPlotPoint arrow[3];
  gdouble xm, ym;
  gdouble m = canvas->magnification;

  gtk_plot_canvas_get_pixel(canvas, line->x1, line->y1, &rx1, &ry1);
  gtk_plot_canvas_get_pixel(canvas, line->x2, line->y2, &rx2, &ry2);

  if(width == 0 && height == 0) return;

  if(width != 0)
      angle = atan2((gdouble)(ry2 - ry1), (gdouble)(rx2 - rx1));
  else
      angle = asin((ry2 - ry1)/height);
                                                                                
  gtk_plot_canvas_set_line_attributes(canvas, line->line);
  gtk_plot_pc_draw_line(canvas->pc, rx1, ry1, rx2, ry2);
  gtk_plot_pc_set_lineattr(canvas->pc, line->line.line_width, 0, 0, 0);
  if(line->arrow_mask & GTK_PLOT_CANVAS_ARROW_END){
     arrow[1].x = rx2;
     arrow[1].y = ry2;
     xm = rx2 - cos(angle) * line->arrow_length * m;
     ym = ry2 - sin(angle) * line->arrow_length * m;
     arrow[0].x = xm - sin(angle)* line->arrow_width * m / 2.0;
     arrow[0].y = ym + cos(angle)* line->arrow_width * m / 2.0;
     arrow[2].x = xm + sin(angle)* line->arrow_width * m / 2.0;
     arrow[2].y = ym - cos(angle)* line->arrow_width * m / 2.0;
     switch(line->arrow_style){
       case GTK_PLOT_SYMBOL_EMPTY:
         gtk_plot_pc_draw_lines (canvas->pc, arrow, 3);
         break;
       case GTK_PLOT_SYMBOL_OPAQUE:
         gtk_plot_pc_set_color(canvas->pc, &canvas->background);
         gtk_plot_pc_draw_polygon (canvas->pc, TRUE, arrow, 3);
         gtk_plot_pc_set_color(canvas->pc, &line->line.color);
         gtk_plot_pc_draw_polygon (canvas->pc, FALSE, arrow, 3);
         break;
       case GTK_PLOT_SYMBOL_FILLED:
         gtk_plot_pc_draw_polygon (canvas->pc, TRUE, arrow, 3);
     }
  }
  if(line->arrow_mask & GTK_PLOT_CANVAS_ARROW_ORIGIN){
     arrow[1].x = rx1;
     arrow[1].y = ry1;
     xm = rx1 + cos(angle) * line->arrow_length * m;
     ym = ry1 + sin(angle) * line->arrow_length * m;
     arrow[0].x = xm + sin(angle)* line->arrow_width * m / 2.0;
     arrow[0].y = ym - cos(angle)* line->arrow_width * m / 2.0;
     arrow[2].x = xm - sin(angle)* line->arrow_width * m / 2.0;
     arrow[2].y = ym + cos(angle)* line->arrow_width * m / 2.0;
     switch(line->arrow_style){
       case GTK_PLOT_SYMBOL_EMPTY:
         gtk_plot_pc_draw_lines (canvas->pc, arrow, 3);
         break;
       case GTK_PLOT_SYMBOL_OPAQUE:
         gtk_plot_pc_set_color(canvas->pc, &canvas->background);
         gtk_plot_pc_draw_polygon (canvas->pc, TRUE, arrow, 3);
         gtk_plot_pc_set_color(canvas->pc, &line->line.color);
         gtk_plot_pc_draw_polygon (canvas->pc, FALSE, arrow, 3);
         break;
       case GTK_PLOT_SYMBOL_FILLED:
         gtk_plot_pc_draw_polygon (canvas->pc, TRUE, arrow, 3);
     }
  }
}

static void 
gtk_plot_canvas_line_draw_selection 	(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 GtkAllocation area)
{
  GtkPlotCanvasLine *line = GTK_PLOT_CANVAS_LINE(child);
  GdkGC *xor_gc = NULL;
  GdkGCValues values;
  gint x1, y1;
  gint x2, y2;
  gint dx, dy;

  dx = canvas->pointer_x - canvas->drag_x;
  dy = canvas->pointer_y - canvas->drag_y;
  
  gdk_gc_get_values(GTK_WIDGET(canvas)->style->fg_gc[0], &values);
  values.function = GDK_INVERT;
  values.foreground = GTK_WIDGET(canvas)->style->white;
  values.subwindow_mode = GDK_INCLUDE_INFERIORS;
  xor_gc = gdk_gc_new_with_values(GTK_WIDGET(canvas)->window,
                                  &values,
                                  GDK_GC_FOREGROUND |
                                  GDK_GC_FUNCTION |
                                  GDK_GC_SUBWINDOW);

  gtk_plot_canvas_get_pixel(canvas, line->x1, line->y1, &x1, &y1);
  gtk_plot_canvas_get_pixel(canvas, line->x2, line->y2, &x2, &y2);

  if(line->pos == GTK_PLOT_CANVAS_TOP_LEFT){ 
    gdk_draw_rectangle(GTK_WIDGET(canvas)->window, xor_gc, TRUE,
                       x2 - DEFAULT_MARKER_SIZE / 2, 
                       y2 - DEFAULT_MARKER_SIZE / 2,
                       DEFAULT_MARKER_SIZE + 1, DEFAULT_MARKER_SIZE + 1);

    gdk_draw_rectangle(GTK_WIDGET(canvas)->window, xor_gc, TRUE,
                       x1 + dx - DEFAULT_MARKER_SIZE / 2, 
                       y1 + dy - DEFAULT_MARKER_SIZE / 2,
                       DEFAULT_MARKER_SIZE + 1, DEFAULT_MARKER_SIZE + 1);

    gdk_gc_set_line_attributes(xor_gc, 1, 1, 0, 0);
    gdk_draw_line(GTK_WIDGET(canvas)->window, xor_gc, x2, y2, x1 + dx, y1 + dy);
 
  } else if(line->pos == GTK_PLOT_CANVAS_BOTTOM_RIGHT){ 
    gdk_draw_rectangle(GTK_WIDGET(canvas)->window, xor_gc, TRUE,
                       x1 - DEFAULT_MARKER_SIZE / 2, 
                       y1 - DEFAULT_MARKER_SIZE / 2,
                       DEFAULT_MARKER_SIZE + 1, DEFAULT_MARKER_SIZE + 1);

    gdk_draw_rectangle(GTK_WIDGET(canvas)->window, xor_gc, TRUE,
                       x2 + dx - DEFAULT_MARKER_SIZE / 2, 
                       y2 + dy - DEFAULT_MARKER_SIZE / 2,
                       DEFAULT_MARKER_SIZE + 1, DEFAULT_MARKER_SIZE + 1);

    gdk_gc_set_line_attributes(xor_gc, 1, 1, 0, 0);
    gdk_draw_line(GTK_WIDGET(canvas)->window, xor_gc, x1, y1, x2 + dx, y2 + dy);
 
  } else { 
    
    gdk_draw_rectangle(GTK_WIDGET(canvas)->window, xor_gc, TRUE,
                       x1 + dx - DEFAULT_MARKER_SIZE / 2, 
                       y1 + dy - DEFAULT_MARKER_SIZE / 2,
                       DEFAULT_MARKER_SIZE + 1, DEFAULT_MARKER_SIZE + 1);

    gdk_draw_rectangle(GTK_WIDGET(canvas)->window, xor_gc, TRUE,
                       x2 + dx - DEFAULT_MARKER_SIZE / 2, 
                       y2 + dy - DEFAULT_MARKER_SIZE / 2,
                       DEFAULT_MARKER_SIZE + 1, DEFAULT_MARKER_SIZE + 1);

    gdk_gc_set_line_attributes(xor_gc, 1, 1, 0, 0);
    gdk_draw_line(GTK_WIDGET(canvas)->window, xor_gc, 
                 x1 + dx, y1 + dy, x2 + dx, y2 + dy);
  }
  

  if(xor_gc) gdk_gc_unref(xor_gc);
}

static GtkPlotCanvasPos 
gtk_plot_canvas_line_button_press   (GtkPlotCanvas *canvas,
				     GtkPlotCanvasChild *child,
				     gint x, gint y)
{
  GtkPlotCanvasLine *line = GTK_PLOT_CANVAS_LINE(child);
  gint x1, y1, x2, y2;

  gtk_plot_canvas_get_pixel(canvas, line->x1, line->y1, &x1, &y1);
  gtk_plot_canvas_get_pixel(canvas, line->x2, line->y2, &x2, &y2);

  if(abs(x - x1) <= DEFAULT_MARKER_SIZE  / 2 && abs(y - y1) <= DEFAULT_MARKER_SIZE / 2) {
    line->pos = GTK_PLOT_CANVAS_TOP_LEFT;
    child->state = GTK_STATE_SELECTED;
    return line->pos;
  }
  if(abs(x - x2) <= DEFAULT_MARKER_SIZE / 2 && abs(y - y2) <= DEFAULT_MARKER_SIZE / 2) {
    line->pos = GTK_PLOT_CANVAS_BOTTOM_RIGHT;
    child->state = GTK_STATE_SELECTED;
    return line->pos;
  }

  if(x < MAX(x1, x2) && x > MIN(x1, x2) && y < MAX(y1, y2) && y > MIN(y1, y2)){
    gdouble dist;
    gdouble vx, vy;

    vx = x2 - x1;
    vy = y2 - y1;
    dist = (x - x1) * vy - (y - y1) * vx;
    dist /= sqrt(vx * vx + vy * vy); 
    if(fabs(dist) <= DEFAULT_MARKER_SIZE) {
      line->pos = GTK_PLOT_CANVAS_IN;
      child->state = GTK_STATE_SELECTED;
      return line->pos;
    }
  } 

  line->pos = GTK_PLOT_CANVAS_OUT;
  child->state = GTK_STATE_NORMAL;
  return GTK_PLOT_CANVAS_OUT;
}

static void 
gtk_plot_canvas_line_size_allocate  (GtkPlotCanvas *canvas,
				     GtkPlotCanvasChild *child)
{
  GtkPlotCanvasLine *line = GTK_PLOT_CANVAS_LINE(child);
  gint x1, x2, y1, y2;

  line->x1 = child->rx1;
  line->y1 = child->ry1;
  line->x2 = child->rx2;
  line->y2 = child->ry2;

  gtk_plot_canvas_get_pixel(canvas, child->rx1, child->ry1, &x1, &y1);
  gtk_plot_canvas_get_pixel(canvas, child->rx2, child->ry2, &x2, &y2);

  child->allocation.x = MIN(x1, x2);
  child->allocation.y = MIN(y1, y2);
  child->allocation.width = abs(x1 - x2);
  child->allocation.height = abs(y1 - y2);
}

static void 
gtk_plot_canvas_line_move		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble x, gdouble y)
{
  return;
}

static void 
gtk_plot_canvas_line_resize             (GtkPlotCanvas *canvas,
                                         GtkPlotCanvasChild *child,
                                         gdouble x1, gdouble y1,
                                         gdouble x2, gdouble y2)
{
  GtkPlotCanvasLine *line = GTK_PLOT_CANVAS_LINE(child);
                                                                                
  line->x1 = x1;
  line->y1 = y1;
  line->x2 = x2;
  line->y2 = y2;
                                                                                
  GTK_PLOT_CANVAS_CHILD_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(child)))->size_allocate(canvas, child);
  gtk_plot_canvas_paint(canvas);
  gtk_plot_canvas_refresh(canvas);
}

static void
gtk_plot_canvas_line_button_release     (GtkPlotCanvas *canvas,
                                         GtkPlotCanvasChild *child)
{
  GtkPlotCanvasLine *line = GTK_PLOT_CANVAS_LINE(child);
  gdouble px, py, dx, dy;

  gtk_plot_canvas_get_position(canvas, canvas->pointer_x, canvas->pointer_y, &px, &py);
  gtk_plot_canvas_get_position(canvas, canvas->drag_x, canvas->drag_y, &dx, &dy);

  if(line->pos == GTK_PLOT_CANVAS_TOP_LEFT){
    line->x1 += (px - dx);
    line->y1 += (py - dy);
  } else if(line->pos == GTK_PLOT_CANVAS_BOTTOM_RIGHT) {
    line->x2 += (px - dx);
    line->y2 += (py - dy);
  } else if(line->pos == GTK_PLOT_CANVAS_IN) {
    line->x1 += (px - dx);
    line->y1 += (py - dy);
    line->x2 += (px - dx);
    line->y2 += (py - dy);
  }

  child->rx1 = line->x1;
  child->ry1 = line->y1;
  child->rx2 = line->x2;
  child->ry2 = line->y2;

  GTK_PLOT_CANVAS_CHILD_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(child)))->size_allocate(canvas, child);
  gtk_plot_canvas_paint(canvas);
  gtk_plot_canvas_refresh(canvas);
}

void
gtk_plot_canvas_line_set_attributes(GtkPlotCanvasLine *line,
                                    GtkPlotLineStyle style,
                                    gfloat width,
                                    const GdkColor *color)
{
  if(color) line->line.color = *color;
  line->line.line_width = width;
  line->line.line_style = style;
}

void
gtk_plot_canvas_line_set_arrow(GtkPlotCanvasLine *line,
                               GtkPlotSymbolStyle style,
                               gfloat width, gfloat length,
                               GtkPlotCanvasArrow mask)
{
  line->arrow_width = width;
  line->arrow_length = length;
  line->arrow_style = style;
  line->arrow_mask = mask;
}


