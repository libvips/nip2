/* gtkplotbar - 3d scientific plots widget for gtk+
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

#ifndef __GTK_PLOT_BAR_H__
#define __GTK_PLOT_BAR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gtkplot.h"

#define GTK_PLOT_BAR(obj)        GTK_CHECK_CAST (obj, gtk_plot_bar_get_type (), GtkPlotBar)
#define GTK_TYPE_PLOT_BAR        (gtk_plot_bar_get_type ())
#define GTK_PLOT_BAR_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_bar_get_type(), GtkPlotBarClass)
#define GTK_IS_PLOT_BAR(obj)     GTK_CHECK_TYPE (obj, gtk_plot_bar_get_type ())

typedef struct _GtkPlotBar             GtkPlotBar;
typedef struct _GtkPlotBarClass        GtkPlotBarClass;

typedef enum
{
  GTK_PLOT_BAR_POINTS,
  GTK_PLOT_BAR_RELATIVE,
  GTK_PLOT_BAR_ABSOLUTE,
} GtkPlotBarUnits;

struct _GtkPlotBar
{
  GtkPlotData data;

  GtkOrientation orientation;
  gdouble width;
};

struct _GtkPlotBarClass
{
  GtkPlotDataClass parent_class;
};


GtkType		gtk_plot_bar_get_type	(void);
GtkWidget*	gtk_plot_bar_new	(GtkOrientation orientation);

void		gtk_plot_bar_construct	(GtkPlotBar *bar, 
					 GtkOrientation orientation);
void		gtk_plot_bar_set_width	(GtkPlotBar *bar, gdouble width);
gdouble		gtk_plot_bar_get_width	(GtkPlotBar *bar);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_BAR_H__ */
