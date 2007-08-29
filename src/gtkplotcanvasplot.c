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
#include "gtkplot3d.h"
#include "gtkplotdata.h"
#include "gtkplotcanvas.h"
#include "gtkplotcanvasplot.h"
#include "gtkplotgdk.h"
#include "gtkplotps.h"

#define DEFAULT_MARKER_SIZE 6

static void gtk_plot_canvas_plot_init		(GtkPlotCanvasPlot *plot);
static void gtk_plot_canvas_plot_class_init(GtkPlotCanvasChildClass *klass);
static void gtk_plot_canvas_plot_destroy	(GtkObject *object);
static void gtk_plot_canvas_plot_draw 		(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_plot_move		(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x, gdouble y);
static void gtk_plot_canvas_plot_resize		(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble x1, gdouble y1,
						 gdouble x2, gdouble y2);
static void gtk_plot_canvas_plot_size_allocate	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_plot_set_magnification(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 gdouble m);
static void gtk_plot_canvas_plot_unselect	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static GtkPlotCanvasPos gtk_plot_canvas_plot_button_press
						(GtkPlotCanvas *canvas, 
				  		 GtkPlotCanvasChild *child, 
				  		 gint x, gint y);
static void gtk_plot_canvas_plot_button_release (GtkPlotCanvas *canvas, 
						 GtkPlotCanvasChild *child);
static void reset_plot_allocation		(GtkPlotCanvas *canvas, 
						 GtkPlotCanvasPlot *child);
extern inline gint roundint                     (gdouble x);
static GtkPlotCanvasChildClass *parent_class = NULL;

extern GtkPlotCanvasPos possible_selection	(GtkAllocation area, 
						 gint x, gint y);

GtkType
gtk_plot_canvas_plot_get_type (void)
{
  static GtkType plot_canvas_plot_type = 0;

  if (!plot_canvas_plot_type)
    {
      GtkTypeInfo plot_canvas_plot_info =
      {
	"GtkPlotCanvasPlot",
	sizeof (GtkPlotCanvasPlot),
	sizeof (GtkPlotCanvasPlotClass),
	(GtkClassInitFunc) gtk_plot_canvas_plot_class_init,
	(GtkObjectInitFunc) gtk_plot_canvas_plot_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      plot_canvas_plot_type = gtk_type_unique (gtk_plot_canvas_child_get_type(), &plot_canvas_plot_info);
    }
  return plot_canvas_plot_type;
}

GtkPlotCanvasChild*
gtk_plot_canvas_plot_new (GtkPlot *plot)
{
  GtkPlotCanvasPlot *child;
                                                                                
  child = gtk_type_new (gtk_plot_canvas_plot_get_type ());
  child->plot = plot;
                                 
  return GTK_PLOT_CANVAS_CHILD (child);
}

static void
gtk_plot_canvas_plot_init (GtkPlotCanvasPlot *plot)
{
  plot->plot = NULL;
  plot->pos = GTK_PLOT_CANVAS_PLOT_IN_PLOT;
  plot->axis = NULL;
  plot->data = NULL;
  plot->marker = NULL;
  GTK_PLOT_CANVAS_PLOT(plot)->datapoint = -1;
  GTK_PLOT_CANVAS_PLOT(plot)->flags = 0; /* GTK_PLOT_CANVAS_PLOT_SELECT_POINT; */
}

static void
gtk_plot_canvas_plot_class_init (GtkPlotCanvasChildClass *klass)
{
  GtkObjectClass *object_class = (GtkObjectClass *)klass;

  parent_class = gtk_type_class (gtk_plot_canvas_child_get_type ());

  klass->draw = gtk_plot_canvas_plot_draw; 
  klass->move = gtk_plot_canvas_plot_move; 
  klass->move_resize = gtk_plot_canvas_plot_resize; 
  klass->size_allocate = gtk_plot_canvas_plot_size_allocate; 
  klass->button_press = gtk_plot_canvas_plot_button_press;
  klass->button_release = gtk_plot_canvas_plot_button_release;
  klass->unselect = gtk_plot_canvas_plot_unselect;
  klass->set_magnification = gtk_plot_canvas_plot_set_magnification; 

  object_class->destroy = gtk_plot_canvas_plot_destroy;
}

static void
gtk_plot_canvas_plot_destroy(GtkObject *object)
{
  GtkWidget *widget = GTK_WIDGET(GTK_PLOT_CANVAS_PLOT(object)->plot);
  gtk_widget_unref(widget);
}

static void 
gtk_plot_canvas_plot_draw 		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child)
{
  GtkPlotCanvasPlot *plot_child = GTK_PLOT_CANVAS_PLOT(child);
  GtkPlot *plot = plot_child->plot;
  gint width = child->allocation.width;
  gint height = child->allocation.height;
  gdouble m = canvas->magnification;
  GtkPlotPC *pc;

  if(width == 0 && height == 0) return;

  gtk_plot_set_drawable(plot, canvas->pixmap);
  GTK_WIDGET(plot)->allocation.x = 0;
  GTK_WIDGET(plot)->allocation.y = 0;
  GTK_WIDGET(plot)->allocation.width = canvas->pixmap_width;
  GTK_WIDGET(plot)->allocation.height = canvas->pixmap_height;
  gtk_plot_set_magnification(plot, m);
  reset_plot_allocation(canvas, plot_child);

  pc = plot->pc;
  plot->pc = canvas->pc;

  gtk_plot_paint(GTK_PLOT_CANVAS_PLOT(child)->plot);

  plot->pc = pc;
}

static void
reset_plot_allocation(GtkPlotCanvas *canvas, GtkPlotCanvasPlot *child)
{
  GtkPlot *plot = child->plot;
  gdouble x1 = plot->internal_allocation.x;
  gdouble y1 = plot->internal_allocation.y;
  gdouble x2 = x1 + plot->internal_allocation.width;
  gdouble y2 = y1 + plot->internal_allocation.height;

  GTK_PLOT_CANVAS_CHILD(child)->allocation = plot->internal_allocation;
  GTK_PLOT_CANVAS_CHILD(child)->rx1 = x1 / (gdouble) canvas->pixmap_width;
  GTK_PLOT_CANVAS_CHILD(child)->rx2 = x2 / (gdouble) canvas->pixmap_width;
  GTK_PLOT_CANVAS_CHILD(child)->ry1 = y1 / (gdouble) canvas->pixmap_height;
  GTK_PLOT_CANVAS_CHILD(child)->ry2 = y2 / (gdouble) canvas->pixmap_height;
/*
  GTK_PLOT_CANVAS_CHILD(child)->selection = GTK_PLOT_CANVAS_SELECT_MARKERS;
  GTK_PLOT_CANVAS_CHILD(child)->mode = GTK_PLOT_CANVAS_SELECT_CLICK_2;
  GTK_PLOT_CANVAS_CHILD(child)->flags = GTK_PLOT_CANVAS_CAN_MOVE | GTK_PLOT_CANVAS_CAN_RESIZE;
*/
}

static void 
gtk_plot_canvas_plot_move		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble x, gdouble y)
{
  GtkPlotCanvasPlot *cplot = GTK_PLOT_CANVAS_PLOT(child);
  GtkPlot *plot = cplot->plot;
  if(!plot) return;
}

static void 
gtk_plot_canvas_plot_resize	(GtkPlotCanvas *canvas,
				 GtkPlotCanvasChild *child,
				 gdouble x1, gdouble y1,
				 gdouble x2, gdouble y2)
{
  GtkPlotCanvasPlot *cplot = GTK_PLOT_CANVAS_PLOT(child);
  GtkPlot *plot = cplot->plot;

  if(!plot) return;

  gtk_plot_move_resize(plot, x1, y1, fabs(x2-x1), fabs(y2-y1));
  GTK_PLOT_CANVAS_CHILD_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(child)))->size_allocate(canvas, child);
  gtk_plot_canvas_paint(canvas);
  gtk_plot_canvas_refresh(canvas);
}

static void 
gtk_plot_canvas_plot_button_release	(GtkPlotCanvas *canvas,
				 	 GtkPlotCanvasChild *child)
{
  GtkPlotCanvasPlot *cplot = GTK_PLOT_CANVAS_PLOT(child);
  GtkPlot *plot = cplot->plot;
  gdouble fx, fy, fz, fa;
  gdouble fdx, fdy, fdz, fda;
  gdouble mx, my;
  gchar *label = NULL;
  gboolean error;
  gint i;
  gdouble new_x, new_y, new_width, new_height;
  gdouble old_x, old_y;
  gdouble dx, dy;
  gdouble x1, x2, y1, y2;
  gboolean moved = FALSE;

  gtk_plot_canvas_get_position(canvas,
                               child->drag_area.x,
                               child->drag_area.y,
                               &old_x, &old_y);

  gtk_plot_canvas_get_position(canvas,
                               canvas->drag_area.width,
                               canvas->drag_area.height,
                               &new_width, &new_height);
                                                                                
  gtk_plot_canvas_get_position(canvas,
                               canvas->drag_area.x,
                               canvas->drag_area.y,
                               &new_x, &new_y);

  dx = new_x - old_x;                                             
  dy = new_y - old_y;                                             
  x1 = MIN(child->rx1, child->rx2);
  y1 = MIN(child->ry1, child->ry2);
  x2 = MAX(child->rx1, child->rx2);
  y2 = MAX(child->ry1, child->ry2);
  x1 += dx;
  y1 += dy;
  x2 = x1 + new_width;
  y2 = y1 + new_height;

  if(!plot) return;

  switch(cplot->pos){
    case GTK_PLOT_CANVAS_PLOT_IN_LEGENDS:
      gtk_plot_legends_move(plot, (new_x - plot->x)*canvas->pixmap_width/plot->internal_allocation.width, (new_y - plot->y)*canvas->pixmap_height/plot->internal_allocation.height);
      child->drag_area = canvas->drag_area;
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_TITLE:
      if(!GTK_IS_PLOT3D(plot) && cplot->axis){
         GtkPlotText *text = &cplot->axis->title;
         text->x += (new_x - old_x);
         text->y += (new_y - old_y);
         child->drag_area = canvas->drag_area;
      }
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_DATA:
      if(GTK_PLOT_CANVAS_PLOT_DND_POINT(cplot)){
        gdouble *array_x = NULL;
        gdouble *array_y = NULL;
        gdouble px, py;
        gint n;
	gdouble x, y;
        array_x = gtk_plot_data_get_x(cplot->data, &n);
        array_y = gtk_plot_data_get_y(cplot->data, &n);
	gtk_plot_get_pixel(plot, array_x[cplot->datapoint], array_y[cplot->datapoint], &px, &py);
        new_x = px + (canvas->pointer_x - canvas->drag_x); 
        new_y = py + (canvas->pointer_y - canvas->drag_y); 
	gtk_plot_get_point(plot, new_x, new_y, &x, &y);
        array_x[cplot->datapoint] = x;
        array_y[cplot->datapoint] = y;
        gtk_plot_canvas_get_position(canvas, new_x, new_y,
                                     &child->rx1, &child->ry1);
        child->drag_area.x = new_x - DEFAULT_MARKER_SIZE;
        child->drag_area.y = new_y - DEFAULT_MARKER_SIZE;
      }
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_MARKER:
      gtk_plot_data_get_point(cplot->marker->data, cplot->marker->point,
                              &fx, &fy, &fz, &fa, &fdx, &fdy, &fdz, &fda,
                              &label, &error);

      gtk_plot_get_pixel(plot,
                         fx, fy,
                         &mx, &my);

      new_x = mx + (canvas->pointer_x - canvas->drag_x);
      new_y = my + (canvas->pointer_y - canvas->drag_y);

      for(i = 0; i < cplot->marker->data->num_points; i++){
        gdouble px, py;

        gtk_plot_data_get_point(cplot->marker->data, i,
                                &fx, &fy, &fz, &fa, &fdx, &fdy, &fdz, &fda,
                                &label, &error);

        gtk_plot_get_pixel(plot,
                           fx, fy,
                           &px, &py);

        if(abs(new_x - px) <= DEFAULT_MARKER_SIZE &&
           abs(new_y - py) <= DEFAULT_MARKER_SIZE){
             cplot->marker->point = i;
             cplot->datapoint = i;
             gtk_plot_canvas_get_position(canvas, px, py,
                                          &child->rx1, &child->ry1);
             child->drag_area.x = px - 10;
             child->drag_area.y = py - 10;
             canvas->drag_area = child->drag_area;
             moved = TRUE;
             break;
        }
      }
      if(!moved){
        child->drag_area.x = mx - 10;
        child->drag_area.y = my - 10;
        canvas->drag_area = child->drag_area;
      }
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_GRADIENT:
      gtk_plot_data_move_gradient(cplot->data, (x1 - plot->x)*canvas->pixmap_width/plot->internal_allocation.width, (y1 - plot->y)*canvas->pixmap_height/plot->internal_allocation.height);
      child->drag_area.x = roundint(child->rx1 * canvas->pixmap_width);
      child->drag_area.y = roundint(child->ry1 * canvas->pixmap_height);
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_AXIS:
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_PLOT:
    default:
      child->rx1 = x1;
      child->ry1 = y1;
      child->rx2 = x2;
      child->ry2 = y2;
      child->drag_area = canvas->drag_area;
      gtk_plot_move_resize(plot, 
  		           x1, y1, fabs(x2-x1), fabs(y2-y1));
      GTK_PLOT_CANVAS_CHILD_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(child)))->size_allocate(canvas, child);
      break;
  }
  gtk_plot_canvas_paint(canvas);
  gtk_plot_canvas_refresh(canvas);
}

static void 
gtk_plot_canvas_plot_set_magnification	(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child,
					 gdouble m)
{
  GtkPlot *plot = GTK_PLOT_CANVAS_PLOT(child)->plot;

  gtk_plot_set_magnification(plot, m);
}

static void 
gtk_plot_canvas_plot_size_allocate	(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child)
{
  GtkPlot *plot = GTK_PLOT_CANVAS_PLOT(child)->plot;
  if(!plot) return;

  switch(GTK_PLOT_CANVAS_PLOT(child)->pos){
    case GTK_PLOT_CANVAS_PLOT_IN_PLOT:
    case GTK_PLOT_CANVAS_PLOT_OUT:
      GTK_WIDGET(plot)->allocation.x = 0;
      GTK_WIDGET(plot)->allocation.y = 0;
      GTK_WIDGET(plot)->allocation.width = canvas->pixmap_width;
      GTK_WIDGET(plot)->allocation.height = canvas->pixmap_height;

      if(!GTK_WIDGET(plot)->parent) 
        gtk_widget_set_parent(GTK_WIDGET(plot), GTK_WIDGET(canvas));

      gtk_plot_move_resize(plot, child->rx1, child->ry1, 
                           fabs(child->rx2-child->rx1), fabs(child->ry2-child->ry1));
      reset_plot_allocation(canvas, GTK_PLOT_CANVAS_PLOT(child));
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_LEGENDS:
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_GRADIENT:
      child->drag_area.x = roundint(child->rx1 * canvas->pixmap_width);
      child->drag_area.y = roundint(child->ry1 * canvas->pixmap_height);
      break;
    case GTK_PLOT_CANVAS_PLOT_IN_TITLE:
      {
        GtkPlotText text = GTK_PLOT_CANVAS_PLOT(child)->axis->title;
        gint tx, ty, twidth, theight;
        gint rx, ry;
                                                                                
        gtk_plot_text_get_area(text.text, text.angle,
                               text.justification,
                               text.font, 
		   	       roundint(canvas->magnification * text.height),
                               &tx, &ty, &twidth, &theight);
                                                                                
        gtk_plot_canvas_get_pixel(canvas,
                                  text.x, text.y,
                                  &rx, &ry);

        if(text.border != GTK_PLOT_BORDER_NONE){
          tx -= text.border_space;
          ty -= text.border_space;
          twidth += 2 * text.border_space;
          theight += 2 * text.border_space;
        }
                                                                                
        child->drag_area.x = tx + rx;
        child->drag_area.y = ty + ry;
        child->drag_area.width = twidth;
        child->drag_area.height = theight;
      }
      break;
    default:
      break;
  }
}

static void
gtk_plot_canvas_plot_unselect(GtkPlotCanvas *canvas, GtkPlotCanvasChild *child)
{
  reset_plot_allocation(canvas, GTK_PLOT_CANVAS_PLOT(child));
}

static void
canvas_unselect(GtkPlotCanvas *canvas, GtkPlotCanvasChild *child)
{
  gtk_plot_canvas_unselect(canvas);
  reset_plot_allocation(canvas, GTK_PLOT_CANVAS_PLOT(child));
  GTK_PLOT_CANVAS_CHILD(child)->selection = GTK_PLOT_CANVAS_SELECT_MARKERS;
  GTK_PLOT_CANVAS_CHILD(child)->mode = GTK_PLOT_CANVAS_SELECT_CLICK_2;
  GTK_PLOT_CANVAS_CHILD(child)->flags = GTK_PLOT_CANVAS_CAN_MOVE | GTK_PLOT_CANVAS_CAN_RESIZE;
}

static GtkPlotCanvasPos 
gtk_plot_canvas_plot_button_press(GtkPlotCanvas *canvas, 
				  GtkPlotCanvasChild *child, 
				  gint x, gint y)
{
  GtkAllocation area;
  GtkPlot *plot = GTK_PLOT_CANVAS_PLOT(child)->plot;
  GtkPlotCanvasPos pos;
  GList *dataset = NULL;

  if(!plot) return GTK_PLOT_CANVAS_OUT;

  /* Legends */

  area = gtk_plot_legends_get_allocation(plot);
  if((pos = possible_selection(area, x, y)) != GTK_PLOT_CANVAS_OUT) {
    if(canvas->active_item == child &&
       GTK_PLOT_CANVAS_PLOT(child)->pos == GTK_PLOT_CANVAS_PLOT_IN_LEGENDS)
         return pos;

    canvas_unselect(canvas, child);
    child->drag_area = area;

    GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_IN_LEGENDS;
    child->selection = GTK_PLOT_CANVAS_SELECT_MARKERS;
    child->mode = GTK_PLOT_CANVAS_SELECT_CLICK_2;
    child->flags = GTK_PLOT_CANVAS_CAN_MOVE;
    child->state = GTK_STATE_SELECTED;
    return pos;
  }

  /* Gradient */

  dataset = plot->data_sets;
                                                                                
  while(dataset) {
    GtkPlotData *data;
    data = GTK_PLOT_DATA(dataset->data);
                                                                               
    area = gtk_plot_data_get_gradient_allocation(data);
    if((pos = possible_selection(area, x, y)) != GTK_PLOT_CANVAS_OUT) {
      if(canvas->active_item == child &&
         GTK_PLOT_CANVAS_PLOT(child)->pos == GTK_PLOT_CANVAS_PLOT_IN_GRADIENT &&
         GTK_PLOT_CANVAS_PLOT(child)->data == data) return pos;
         
      canvas_unselect(canvas, child);
      child->drag_area = area;

      GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_IN_GRADIENT;
      GTK_PLOT_CANVAS_PLOT(child)->data = data;
      child->selection = GTK_PLOT_CANVAS_SELECT_MARKERS;
      child->mode = GTK_PLOT_CANVAS_SELECT_CLICK_2;
      child->flags = GTK_PLOT_CANVAS_CAN_MOVE;
      child->state = GTK_STATE_SELECTED;
      return pos;
    }
    dataset = dataset->next;
  }

  /* Titles */

  if(!GTK_IS_PLOT3D(plot)){
    GtkPlotAxis *axis[4];
    gint i;

    axis[0]=plot->left;
    axis[1]=plot->right;
    axis[2]=plot->top;
    axis[3]=plot->bottom;

    for(i = 0; i <= 3; i++){
      if(axis[i]->title_visible){
        GtkPlotText *child_text = &axis[i]->title;
        gint tx, ty, twidth, theight;
        gint rx, ry;

        gtk_plot_canvas_get_pixel(canvas,
                                  child_text->x, child_text->y,
                                  &rx, &ry);

        gtk_plot_text_get_area(child_text->text,
                               child_text->angle,
                               child_text->justification,
                               child_text->font,
                               roundint(child_text->height * canvas->magnification),
                               &tx, &ty, &twidth, &theight);
                                                   
        area.x = rx + tx;                       
        area.y = ry + ty;                       
        area.width = twidth;                       
        area.height = theight;                       

        if(child_text->border != GTK_PLOT_BORDER_NONE){
          area.x -= child_text->border_space;
          area.y -= child_text->border_space;
          area.width += 2 * child_text->border_space;
          area.height += 2 * child_text->border_space;
        }

        if((pos = possible_selection(area, x, y)) != GTK_PLOT_CANVAS_OUT){
          if(canvas->active_item == child && 
             GTK_PLOT_CANVAS_PLOT(child)->pos == GTK_PLOT_CANVAS_PLOT_IN_TITLE &&
             GTK_PLOT_CANVAS_PLOT(child)->axis == axis[i])
            return pos;

          canvas_unselect(canvas, child);
          child->drag_area = area;

          GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_IN_TITLE;
          GTK_PLOT_CANVAS_PLOT(child)->axis = axis[i];
          child->selection = GTK_PLOT_CANVAS_SELECT_MARKERS;
          child->mode = GTK_PLOT_CANVAS_SELECT_CLICK_2;
          child->flags = GTK_PLOT_CANVAS_CAN_MOVE;
          child->state = GTK_STATE_SELECTED;
          return pos;
        }
      }
    }

  }

  /* Plot markers */

  area = plot->internal_allocation;
  if((pos = possible_selection(area, x, y)) != GTK_PLOT_CANVAS_OUT &&
      pos != GTK_PLOT_CANVAS_IN) {
    if(canvas->active_item == child &&
       GTK_PLOT_CANVAS_PLOT(child)->pos == GTK_PLOT_CANVAS_PLOT_IN_PLOT)
         return pos;     

    canvas_unselect(canvas, child);
    child->drag_area = area;

    GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_IN_PLOT;
    child->selection = GTK_PLOT_CANVAS_SELECT_MARKERS;
    child->mode = GTK_PLOT_CANVAS_SELECT_CLICK_2;
    child->flags = GTK_PLOT_CANVAS_CAN_MOVE;
    child->state = GTK_STATE_SELECTED;
    return pos;
  }

  /* Axes */

  if(!GTK_IS_PLOT3D(plot)){
    gint px, py, pwidth, pheight;
    GtkAllocation _area[4];
    GtkPlotAxis *axis[4];
    gint i;

    axis[0]=plot->left;
    axis[1]=plot->right;
    axis[2]=plot->top;
    axis[3]=plot->bottom;

    px = plot->internal_allocation.x;
    py = plot->internal_allocation.y;
    pwidth = plot->internal_allocation.width;
    pheight = plot->internal_allocation.height;

    _area[0].x = px - 6;
    _area[0].y = _area[1].y = py;
    _area[0].width = _area[1].width = 6;
    _area[0].height = _area[1].height = pheight;
    _area[1].x = px + pwidth;
    _area[2].x = _area[3].x = px;
    _area[2].y = py - 6;
    _area[2].width = _area[3].width = pwidth;
    _area[2].height = _area[3].height = 6;
    _area[3].y = py + pheight;

    for(i = 0; i < 4; i++){
      area = _area[i];
      if((pos = possible_selection(area, x, y)) != GTK_PLOT_CANVAS_OUT){

        if(canvas->active_item == child && 
           GTK_PLOT_CANVAS_PLOT(child)->pos == GTK_PLOT_CANVAS_PLOT_IN_AXIS &&
           GTK_PLOT_CANVAS_PLOT(child)->axis == axis[i])
          return pos;

        canvas_unselect(canvas, child);
        child->drag_area = area;

        GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_IN_AXIS;
        GTK_PLOT_CANVAS_PLOT(child)->axis = axis[i];
        child->state = GTK_STATE_SELECTED;
        child->flags = GTK_PLOT_CANVAS_FROZEN;
        child->selection = GTK_PLOT_CANVAS_SELECT_MARKERS;
        child->mode = GTK_PLOT_CANVAS_SELECT_CLICK_2;
        return pos;
      }
    }
  }

  /* Markers */

  if(!GTK_IS_PLOT3D(plot)){
    dataset = plot->data_sets;
                                                                                
    while(dataset) {
      GtkPlotData *data = GTK_PLOT_DATA(dataset->data);
      GList *markers = data->markers;                                                             
      while(markers){
        GtkPlotMarker *marker;
        gdouble fx, fy, fz, fa, fdx, fdy, fdz, fda;
        gboolean error;
        gchar *label;
        gdouble xi, yi;
                             
        marker = (GtkPlotMarker *) markers->data;
                                                  
        gtk_plot_data_get_point(data, marker->point,
                                &fx, &fy, &fz, &fa, &fdx, &fdy, &fdz, &fda,
                                &label, &error);
        gtk_plot_get_pixel(GTK_PLOT(plot), fx, fy, &xi, &yi);
        if(abs(xi-x) <= 20 && abs(yi-y) <= 20){

          if(canvas->active_item == child && 
             GTK_PLOT_CANVAS_PLOT(child)->pos == GTK_PLOT_CANVAS_PLOT_IN_MARKER &&
	     GTK_PLOT_CANVAS_PLOT(child)->marker == marker)
             return GTK_PLOT_CANVAS_IN;

          canvas_unselect(canvas, child);

          GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_IN_MARKER;
          GTK_PLOT_CANVAS_PLOT(child)->data = data;
          GTK_PLOT_CANVAS_PLOT(child)->marker = marker;
          GTK_PLOT_CANVAS_PLOT(child)->datapoint = marker->point;
          child->drag_area.x = xi - 10;
          child->drag_area.y = yi - 10;
          child->drag_area.width = 20;
          child->drag_area.height = 20;
          canvas->drag_area = child->drag_area;
          child->state = GTK_STATE_SELECTED;
          child->flags = GTK_PLOT_CANVAS_CAN_MOVE;
          child->selection = GTK_PLOT_CANVAS_SELECT_TARGET;
          child->mode = GTK_PLOT_CANVAS_SELECT_CLICK_1;
          return GTK_PLOT_CANVAS_IN;
        }
        markers = markers->next;
      }
      dataset = dataset->next;
    }
  }

  /* Dataset */

  if(!GTK_IS_PLOT3D(plot)
     && GTK_PLOT_CANVAS_PLOT_SELECT_POINT(GTK_PLOT_CANVAS_PLOT(child))){
    dataset = plot->data_sets;
                                                                                
    while(dataset) {
      GtkPlotData *data = GTK_PLOT_DATA(dataset->data);
                                                                                
      if(!data->is_function){
        gdouble fx, fy, fz, fa, fdx, fdy, fdz, fda;
        gboolean error;
        gchar *label;
        gint i;
                                                                               
        for(i = 0; i < data->num_points; i++){
          gdouble xi, yi;
          gtk_plot_data_get_point(data, i,
                                  &fx, &fy, &fz, &fa, &fdx, &fdy, &fdz, &fda,
                                  &label, &error);
          gtk_plot_get_pixel(GTK_PLOT(plot), fx, fy, &xi, &yi);
          if(abs(xi-x) <= DEFAULT_MARKER_SIZE &&
             abs(yi-y) <= DEFAULT_MARKER_SIZE){

            if(canvas->active_item == child && 
               GTK_PLOT_CANVAS_PLOT(child)->pos == GTK_PLOT_CANVAS_PLOT_IN_DATA &&
               GTK_PLOT_CANVAS_PLOT(child)->data == data &&
               GTK_PLOT_CANVAS_PLOT(child)->datapoint == i)
               return GTK_PLOT_CANVAS_IN;

            canvas_unselect(canvas, child);

            GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_IN_DATA;
            GTK_PLOT_CANVAS_PLOT(child)->data = data;
            GTK_PLOT_CANVAS_PLOT(child)->datapoint = i;
            child->drag_area.x = xi - DEFAULT_MARKER_SIZE;
            child->drag_area.y = yi - DEFAULT_MARKER_SIZE;
            child->drag_area.width = 2*DEFAULT_MARKER_SIZE;
            child->drag_area.height = 2*DEFAULT_MARKER_SIZE;
            canvas->drag_area = child->drag_area;
            child->state = GTK_STATE_SELECTED;
            child->flags = GTK_PLOT_CANVAS_FROZEN;
            if(GTK_PLOT_CANVAS_PLOT_DND_POINT(GTK_PLOT_CANVAS_PLOT(child)))
              child->flags = GTK_PLOT_CANVAS_CAN_MOVE;
            child->selection = GTK_PLOT_CANVAS_SELECT_TARGET;
            child->mode = GTK_PLOT_CANVAS_SELECT_CLICK_1;
            return GTK_PLOT_CANVAS_IN;
          }
        }
      }
      dataset = dataset->next;
    }
  }
                                                                                
  /* Actual plot */

  area = plot->internal_allocation;
  if((pos = possible_selection(area, x, y)) != GTK_PLOT_CANVAS_OUT) {
    if(canvas->active_item == child &&
       GTK_PLOT_CANVAS_PLOT(child)->pos == GTK_PLOT_CANVAS_PLOT_IN_PLOT)
       return pos;

    canvas_unselect(canvas, child);
    child->drag_area = area;

    GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_IN_PLOT;
    child->state = GTK_STATE_SELECTED;
    return pos;
  }


  GTK_PLOT_CANVAS_PLOT(child)->pos = GTK_PLOT_CANVAS_PLOT_OUT;
  return GTK_PLOT_CANVAS_OUT;
}

