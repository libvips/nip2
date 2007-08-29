/* gtkplotpolar - polar plots widget for gtk+
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

#ifndef __GTK_PLOT_POLAR_H__
#define __GTK_PLOT_POLAR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gtkplot.h"
#include "gtkplotpc.h"


#define GTK_PLOT_POLAR(obj)        GTK_CHECK_CAST (obj, gtk_plot_polar_get_type (), GtkPlotPolar)
#define GTK_TYPE_PLOT_POLAR        (gtk_plot_polar_get_type ())
#define GTK_PLOT_POLAR_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_polar_get_type, GtkPlotPolarClass)
#define GTK_IS_PLOT_POLAR(obj)     GTK_CHECK_TYPE (obj, gtk_plot_polar_get_type ())
#define GTK_PLOT_POLAR_FLAGS(plot)         (GTK_PLOT_POLAR(plot)->flags)
#define GTK_PLOT_POLAR_SET_FLAGS(plot,flag) (GTK_PLOT_POLAR_FLAGS(plot) |= (flag))
#define GTK_PLOT_POLAR_UNSET_FLAGS(plot,flag) (GTK_PLOT_POLAR_FLAGS(plot) &= ~(flag))

#define GTK_PLOT_POLAR_TRANSPARENT(plot) (GTK_PLOT_POLAR_FLAGS(plot) & GTK_PLOT_POLAR_TRANSPARENT)

typedef struct _GtkPlotPolar		GtkPlotPolar;
typedef struct _GtkPlotPolarClass		GtkPlotPolarClass;

struct _GtkPlotPolar
{
  GtkPlot plot;
  
  GtkPlotAxis *r;
  GtkPlotAxis *angle;

  gdouble rotation;
};

struct _GtkPlotPolarClass
{
  GtkPlotClass parent_class;
};

/* PlotPolar */

GtkType		gtk_plot_polar_get_type		(void);
GtkWidget*	gtk_plot_polar_new		(GdkDrawable *drawable);
GtkWidget*	gtk_plot_polar_new_with_size	(GdkDrawable *drawable,
                                                 gdouble width, gdouble height);
void		gtk_plot_polar_construct	(GtkPlotPolar *plot,
						 GdkDrawable *drawable);
void		gtk_plot_polar_construct_with_size (GtkPlotPolar *plot,
						    GdkDrawable *drawable,
						    gdouble width, 
						    gdouble height);

void		gtk_plot_polar_rotate		(GtkPlotPolar *plot,
						 gdouble angle);
gdouble         gtk_plot_polar_get_angle        (GtkPlotPolar *plot);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_POLAR_H__ */
