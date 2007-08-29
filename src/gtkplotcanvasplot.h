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

#ifndef __GTK_PLOT_CANVAS_PLOT_H__
#define __GTK_PLOT_CANVAS_PLOT_H__

#define GTK_PLOT_CANVAS_PLOT(obj)        GTK_CHECK_CAST (obj, gtk_plot_canvas_plot_get_type (), GtkPlotCanvasPlot)
#define GTK_PLOT_CANVAS_PLOT_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_canvas_plot_get_type(), GtkPlotCanvasPlotClass)
#define GTK_IS_PLOT_CANVAS_PLOT(obj)     GTK_CHECK_TYPE (obj, gtk_plot_canvas_plot_get_type ())
#define GTK_TYPE_PLOT_CANVAS_PLOT (gtk_plot_canvas_plot_get_type ())
#define GTK_PLOT_CANVAS_PLOT_SELECT_POINT(plot)  (plot->flags & GTK_PLOT_CANVAS_PLOT_SELECT_POINT)
#define GTK_PLOT_CANVAS_PLOT_DND_POINT(plot)  (plot->flags & GTK_PLOT_CANVAS_PLOT_DND_POINT)
#define GTK_PLOT_CANVAS_PLOT_FLAGS(plot)     (GTK_PLOT_CANVAS_PLOT(plot)->flags)
#define GTK_PLOT_CANVAS_PLOT_SET_FLAGS(plot, flags)  (GTK_PLOT_CANVAS_PLOT_FLAGS(plot) |= (flags))
#define GTK_PLOT_CANVAS_PLOT_UNSET_FLAGS(plot, flags)  (GTK_PLOT_CANVAS_PLOT_FLAGS(plot) &= ~(flags))



#include <gdk/gdk.h>
#include "gtkplotpc.h"
#include "gtkplot.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _GtkPlotCanvasPlot	GtkPlotCanvasPlot;
typedef struct _GtkPlotCanvasPlotClass	GtkPlotCanvasPlotClass;

typedef enum
{
  GTK_PLOT_CANVAS_PLOT_OUT,
  GTK_PLOT_CANVAS_PLOT_IN_PLOT,
  GTK_PLOT_CANVAS_PLOT_IN_LEGENDS,
  GTK_PLOT_CANVAS_PLOT_IN_TITLE,
  GTK_PLOT_CANVAS_PLOT_IN_AXIS,
  GTK_PLOT_CANVAS_PLOT_IN_DATA,
  GTK_PLOT_CANVAS_PLOT_IN_GRADIENT,
  GTK_PLOT_CANVAS_PLOT_IN_MARKER,
} GtkPlotCanvasPlotPos;

/* child flags */
enum
{
      GTK_PLOT_CANVAS_PLOT_SELECT_POINT  =       1 << 0, /* Select point */
      GTK_PLOT_CANVAS_PLOT_DND_POINT     =       1 << 1, /* DnD point */
};

struct _GtkPlotCanvasPlot
{
  GtkPlotCanvasChild parent;

  GtkPlotCanvasPlotPos pos;

  GtkPlotAxis *axis;
  GtkPlotData *data;
  GtkPlotMarker *marker;
  gint datapoint;

  guint flags;

  GtkPlot *plot;
};

struct _GtkPlotCanvasPlotClass
{
  GtkPlotCanvasChildClass parent_class;
};

GtkType 	gtk_plot_canvas_plot_get_type	(void);
GtkPlotCanvasChild * 
		gtk_plot_canvas_plot_new	(GtkPlot *plot);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GTK_PLOT_CANVAS_PLOT_H__ */
