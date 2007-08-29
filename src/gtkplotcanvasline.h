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

#ifndef __GTK_PLOT_CANVAS_LINE_H__
#define __GTK_PLOT_CANVAS_LINE_H__

#define GTK_PLOT_CANVAS_LINE(obj)        GTK_CHECK_CAST (obj, gtk_plot_canvas_line_get_type (), GtkPlotCanvasLine)
#define GTK_PLOT_CANVAS_LINE_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_canvas_line_get_type(), GtkPlotCanvasLineClass)
#define GTK_IS_PLOT_CANVAS_LINE(obj)     GTK_CHECK_TYPE (obj, gtk_plot_canvas_line_get_type ())
#define GTK_TYPE_PLOT_CANVAS_LINE (gtk_plot_canvas_line_get_type ())


#include <gdk/gdk.h>
#include "gtkplotpc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _GtkPlotCanvasLine	GtkPlotCanvasLine;
typedef struct _GtkPlotCanvasLineClass	GtkPlotCanvasLineClass;

typedef enum
{
      GTK_PLOT_CANVAS_ARROW_NONE            = 0,
      GTK_PLOT_CANVAS_ARROW_ORIGIN          = 1 << 0,
      GTK_PLOT_CANVAS_ARROW_END             = 1 << 1
} GtkPlotCanvasArrow;
                                                                                
struct _GtkPlotCanvasLine
{
  GtkPlotCanvasChild parent;

  GtkPlotLine line;

  gdouble x1, y1, x2, y2;
  GtkPlotCanvasPos pos; 

  GtkPlotCanvasArrow arrow_mask;
  gint arrow_length;
  gint arrow_width;
  GtkPlotSymbolStyle arrow_style;
};

struct _GtkPlotCanvasLineClass
{
  GtkPlotCanvasChildClass parent_class;
};

GtkType 	gtk_plot_canvas_line_get_type	(void);
GtkPlotCanvasChild * 
		gtk_plot_canvas_line_new	(GtkPlotLineStyle style,
                         			 gfloat width,
                         			 const GdkColor *color,
                         			 GtkPlotCanvasArrow arrow_mask);
void 		gtk_plot_canvas_line_set_attributes
						(GtkPlotCanvasLine *line,
                                    		 GtkPlotLineStyle style,
                                    		 gfloat width,
                                    		 const GdkColor *color);
void 		gtk_plot_canvas_line_set_arrow	(GtkPlotCanvasLine *line,
                               			 GtkPlotSymbolStyle style,
                               			 gfloat width, gfloat length,
                               			 GtkPlotCanvasArrow mask);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GTK_PLOT_CANVAS_LINE_H__ */
