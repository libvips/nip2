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

#ifndef __GTK_PLOT_CANVAS_ELLIPSE_H__
#define __GTK_PLOT_CANVAS_ELLIPSE_H__

#define GTK_PLOT_CANVAS_ELLIPSE(obj)        GTK_CHECK_CAST (obj, gtk_plot_canvas_ellipse_get_type (), GtkPlotCanvasEllipse)
#define GTK_PLOT_CANVAS_ELLIPSE_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_canvas_ellipse_get_type(), GtkPlotCanvasEllipseClass)
#define GTK_IS_PLOT_CANVAS_ELLIPSE(obj)     GTK_CHECK_TYPE (obj, gtk_plot_canvas_ellipse_get_type ())
#define GTK_TYPE_PLOT_CANVAS_ELLIPSE (gtk_plot_canvas_ellipse_get_type ())


#include <gdk/gdk.h>
#include "gtkplotpc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _GtkPlotCanvasEllipse	GtkPlotCanvasEllipse;
typedef struct _GtkPlotCanvasEllipseClass	GtkPlotCanvasEllipseClass;

struct _GtkPlotCanvasEllipse
{
  GtkPlotCanvasChild parent;

  GtkPlotLine line;

  gboolean filled;
  GdkColor bg;
};

struct _GtkPlotCanvasEllipseClass
{
  GtkPlotCanvasChildClass parent_class;
};

GtkType 	gtk_plot_canvas_ellipse_get_type	(void);
GtkPlotCanvasChild * 
		gtk_plot_canvas_ellipse_new	(GtkPlotLineStyle style,
                              			 gfloat width,
                     			         const GdkColor *fg,
                       			         const GdkColor *bg,
                             			 gboolean fill);
void 		gtk_plot_canvas_ellipse_set_attributes(
						 GtkPlotCanvasEllipse *ellipse,
                                         	 GtkPlotLineStyle style,
                                         	 gdouble width,
                                         	 const GdkColor *fg,
                                         	 const GdkColor *bg,
                                         	 gboolean fill);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GTK_PLOT_CANVAS_ELLIPSE_H__ */
