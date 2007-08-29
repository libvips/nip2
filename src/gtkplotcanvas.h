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

#ifndef __GTK_PLOT_CANVAS_H__
#define __GTK_PLOT_CANVAS_H__


#include <gdk/gdk.h>
#include "gtkplot.h"
#include "gtkplotpc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* canvas flags */
enum
{
      GTK_PLOT_CANVAS_CAN_SELECT	=	1 << 0, /* Select region */
      GTK_PLOT_CANVAS_CAN_SELECT_ITEM	=	1 << 1, /* Select item */
      GTK_PLOT_CANVAS_CAN_DND		=	1 << 2, /* DnD item */
};


/* canvas actions */
typedef enum
{
      GTK_PLOT_CANVAS_ACTION_INACTIVE,
      GTK_PLOT_CANVAS_ACTION_SELECTION,
      GTK_PLOT_CANVAS_ACTION_DRAG,
      GTK_PLOT_CANVAS_ACTION_RESIZE,
} GtkPlotCanvasAction;

typedef enum
{
      GTK_PLOT_CANVAS_FROZEN            = 0,
      GTK_PLOT_CANVAS_CAN_MOVE          = 1 << 0,
      GTK_PLOT_CANVAS_CAN_RESIZE	= 1 << 1,
} GtkPlotCanvasFlag;

typedef enum
{
      GTK_PLOT_CANVAS_SELECT_NONE,    
      GTK_PLOT_CANVAS_SELECT_MARKERS,
      GTK_PLOT_CANVAS_SELECT_TARGET,
} GtkPlotCanvasSelection;

typedef enum
{
      GTK_PLOT_CANVAS_SELECT_CLICK_1,    
      GTK_PLOT_CANVAS_SELECT_CLICK_2,
} GtkPlotCanvasSelectionMode;

typedef enum
{
      GTK_PLOT_CANVAS_OUT,
      GTK_PLOT_CANVAS_IN,
      GTK_PLOT_CANVAS_LEFT,
      GTK_PLOT_CANVAS_RIGHT,
      GTK_PLOT_CANVAS_TOP,
      GTK_PLOT_CANVAS_BOTTOM,
      GTK_PLOT_CANVAS_TOP_LEFT,
      GTK_PLOT_CANVAS_TOP_RIGHT,
      GTK_PLOT_CANVAS_BOTTOM_LEFT,
      GTK_PLOT_CANVAS_BOTTOM_RIGHT,
} GtkPlotCanvasPos;

#define GTK_PLOT_CANVAS_DND_FLAGS      (GTK_PLOT_CANVAS_CAN_SELECT_ITEM |  \
                                        GTK_PLOT_CANVAS_CAN_DND ) 

#define GTK_PLOT_CANVAS(obj)        GTK_CHECK_CAST (obj, gtk_plot_canvas_get_type (), GtkPlotCanvas)
#define GTK_PLOT_CANVAS_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_canvas_get_type(), GtkPlotCanvasClass)
#define GTK_IS_PLOT_CANVAS(obj)     GTK_CHECK_TYPE (obj, gtk_plot_canvas_get_type ())
#define GTK_TYPE_PLOT_CANVAS (gtk_plot_canvas_get_type ())

#define GTK_PLOT_CANVAS_FLAGS(canvas)	  (GTK_PLOT_CANVAS(canvas)->flags)
#define GTK_PLOT_CANVAS_SET_FLAGS(canvas, flags)  (GTK_PLOT_CANVAS_FLAGS(canvas) |= (flags))
#define GTK_PLOT_CANVAS_UNSET_FLAGS(canvas, flags)  (GTK_PLOT_CANVAS_FLAGS(canvas) &= ~(flags))
#define GTK_PLOT_CANVAS_CAN_DND_POINT(canvas)  (GTK_PLOT_CANVAS_FLAGS(canvas) & GTK_PLOT_CANVAS_CAN_DND_POINT)
#define GTK_PLOT_CANVAS_CAN_DND(canvas)  (GTK_PLOT_CANVAS_FLAGS(canvas) & GTK_PLOT_CANVAS_CAN_DND)
#define GTK_PLOT_CANVAS_CAN_SELECT_POINT(canvas)  (GTK_PLOT_CANVAS_FLAGS(canvas) & GTK_PLOT_CANVAS_CAN_SELECT_POINT)
#define GTK_PLOT_CANVAS_CAN_SELECT_ITEM(canvas)  (GTK_PLOT_CANVAS_FLAGS(canvas) & GTK_PLOT_CANVAS_CAN_SELECT_ITEM)
#define GTK_PLOT_CANVAS_CAN_SELECT(canvas)  (GTK_PLOT_CANVAS_FLAGS(canvas) & GTK_PLOT_CANVAS_CAN_SELECT)

#define GTK_PLOT_CANVAS_CHILD(obj)        GTK_CHECK_CAST (obj, gtk_plot_canvas_child_get_type (), GtkPlotCanvasChild)
#define GTK_PLOT_CANVAS_CHILD_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_canvas_child_get_type(), GtkPlotCanvasChildClass)
#define GTK_IS_PLOT_CANVAS_CHILD(obj)     GTK_CHECK_TYPE (obj, gtk_plot_canvas_child_get_type ())
#define GTK_TYPE_PLOT_CANVAS_CHILD (gtk_plot_canvas_child_get_type ())


typedef struct _GtkPlotCanvas	GtkPlotCanvas;
typedef struct _GtkPlotCanvasClass	GtkPlotCanvasClass;
typedef struct _GtkPlotCanvasChild	GtkPlotCanvasChild;
typedef struct _GtkPlotCanvasChildClass	GtkPlotCanvasChildClass;

struct _GtkPlotCanvasChild
{
  GtkObject object;

  GtkPlotCanvas *parent;

  gdouble rx1, rx2, ry1, ry2;
  gint min_width, min_height;

  GtkAllocation allocation;
  GtkAllocation drag_area;

  guint state;

  GtkPlotCanvasFlag flags;
  GtkPlotCanvasSelection selection;
  GtkPlotCanvasSelectionMode mode;
};

struct _GtkPlotCanvasChildClass
{
  GtkObjectClass parent_class;

  void (*draw) (GtkPlotCanvas *canvas, GtkPlotCanvasChild *child);
  void (*draw_selection) (GtkPlotCanvas *canvas, GtkPlotCanvasChild *child, GtkAllocation allocation);
  void (*size_allocate) (GtkPlotCanvas *canvas, GtkPlotCanvasChild *child);
  void (*move) (GtkPlotCanvas *canvas, GtkPlotCanvasChild *child,
	        gdouble x, gdouble y);
  void (*move_resize) (GtkPlotCanvas *canvas, GtkPlotCanvasChild *child,
	          gdouble x1, gdouble y1, gdouble x2, gdouble y2);
  GtkPlotCanvasPos (*button_press) 	(GtkPlotCanvas *canvas, 
					 GtkPlotCanvasChild *child, 
					 gint x, gint y);
  void (*button_release) 		(GtkPlotCanvas *canvas, 
					 GtkPlotCanvasChild *child);
  void (*unselect) (GtkPlotCanvas *canvas, GtkPlotCanvasChild *child);
  void (*set_magnification) (GtkPlotCanvas *canvas, GtkPlotCanvasChild *child, gdouble m);
};

struct _GtkPlotCanvas
{
  GtkFixed fixed;

  guint16 flags;
  guint state;

  guint freeze_count;

  gint pixmap_width, pixmap_height;
  gint width, height;

  gdouble magnification;
  
  gboolean show_grid;
  gdouble grid_step;
  GtkPlotLine grid;

  GtkPlotCanvasAction action;

  GdkPixmap *pixmap;

  GdkColor background;
  gboolean transparent;

  gdouble active_x, active_y;

  GtkPlotCanvasChild *active_item;

  GtkPlotCanvasPos drag_point;
  gint drag_x, drag_y;
  gint pointer_x, pointer_y;
  GtkAllocation drag_area;

  GList *childs;

  GdkCursor *cursor;
  GtkPlotPC *pc;
};

struct _GtkPlotCanvasClass
{
  GtkLayoutClass parent_class;

  gboolean (*select_item)	(GtkPlotCanvas *canvas,
				 GdkEventButton *event,
                                 GtkPlotCanvasChild *item);

  gboolean (*move_item)		(GtkPlotCanvas *canvas,
                                 GtkPlotCanvasChild *item,
				 gdouble new_x, gdouble new_y);

  gboolean (*resize_item)	(GtkPlotCanvas *canvas,
                                 GtkPlotCanvasChild *item,
				 gdouble new_width, gdouble new_height);

  void (*add_item)		(GtkPlotCanvas *canvas,
                                 GtkPlotCanvasChild *item);

  gboolean (*delete_item)	(GtkPlotCanvas *canvas,
                                 GtkPlotCanvasChild *item);

  void (*select_region)		(GtkPlotCanvas *canvas,
				 gdouble xmin, gdouble ymin,
				 gdouble xmax, gdouble ymax);

  void (*changed)		(GtkPlotCanvas *canvas);
};


GtkType		gtk_plot_canvas_get_type	(void);
GtkType		gtk_plot_canvas_child_get_type	(void);
GtkWidget*	gtk_plot_canvas_new		(gint width, gint height,
                                                 gdouble magnification);
void		gtk_plot_canvas_construct       (GtkPlotCanvas *canvas,
						 gint width, gint height,
                                                 gdouble magnification);
void		gtk_plot_canvas_set_pc          (GtkPlotCanvas *canvas,
						 GtkPlotPC *pc);
void		gtk_plot_canvas_paint           (GtkPlotCanvas *canvas);
void		gtk_plot_canvas_refresh         (GtkPlotCanvas *canvas);
void 		gtk_plot_canvas_freeze		(GtkPlotCanvas *canvas);
void 		gtk_plot_canvas_thaw		(GtkPlotCanvas *canvas);
void		gtk_plot_canvas_grid_set_visible(GtkPlotCanvas *canvas,
						 gboolean visible);
void		gtk_plot_canvas_grid_set_step	(GtkPlotCanvas *canvas,
						 gdouble step);
void		gtk_plot_canvas_grid_set_attributes(GtkPlotCanvas *canvas,
                         			 GtkPlotLineStyle style,
                         			 gint width,
                         			 const GdkColor *color);
void		gtk_plot_canvas_cancel_action	(GtkPlotCanvas *plot_canvas);
void		gtk_plot_canvas_unselect	(GtkPlotCanvas *plot_canvas);

GtkPlotCanvasChild *   
		gtk_plot_canvas_get_active_item (GtkPlotCanvas *canvas);
void            gtk_plot_canvas_set_size        (GtkPlotCanvas *canvas,
                                                 gint width, gint height);
void            gtk_plot_canvas_set_magnification (GtkPlotCanvas *canvas,
                                                 gdouble magnification);
void 		gtk_plot_canvas_set_transparent (GtkPlotCanvas *canvas, 
						 gboolean transparent);
gboolean 	gtk_plot_canvas_transparent 	(GtkPlotCanvas *canvas);
void		gtk_plot_canvas_set_background  (GtkPlotCanvas *canvas,
						 const GdkColor *background);
void            gtk_plot_canvas_get_pixel       (GtkPlotCanvas *plot_canvas,
                                                 gdouble px, gdouble py,
                                                 gint *x, gint *y);
void            gtk_plot_canvas_get_position    (GtkPlotCanvas *plot_canvas,
                                                 gint x, gint y,
                                                 gdouble *px, gdouble *py);
void 		gtk_plot_canvas_put_child	(GtkPlotCanvas *canvas,
                          			 GtkPlotCanvasChild *child,
                          			 gdouble x1, gdouble y1,
                          			 gdouble x2, gdouble y2);
void 		gtk_plot_canvas_remove_child	(GtkPlotCanvas *canvas,
                          			 GtkPlotCanvasChild *child);
void 		gtk_plot_canvas_child_move	(GtkPlotCanvas *canvas,
                          			 GtkPlotCanvasChild *child,
                          			 gdouble x1, gdouble y1);
void 		gtk_plot_canvas_child_move_resize		
						(GtkPlotCanvas *canvas,
                                 		 GtkPlotCanvasChild *child,
                                 		 gdouble x1, gdouble y1,
                                  		 gdouble x2, gdouble y2);

void 		gtk_plot_canvas_child_set_selection	
						(GtkPlotCanvasChild *child,
						 GtkPlotCanvasSelection selection);
void 		gtk_plot_canvas_child_set_selection_mode	
						(GtkPlotCanvasChild *child,
						 GtkPlotCanvasSelectionMode mode);

gboolean	gtk_plot_canvas_child_get_position	
					       	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child,
						 double *x1, double *y1,
						 double *x2, double *y2);
void 		gtk_plot_canvas_set_line_attributes
						(GtkPlotCanvas *canvas, 
						 GtkPlotLine line);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_CANVAS_H__ */
