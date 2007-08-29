/* gtkplotpc - gtkplot print context - a renderer for printing functions
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

#ifndef __GTK_PLOT_GDK_H__
#define __GTK_PLOT_GDK_H__

#include <stdio.h>
#include <pango/pango.h>
#include "gtkpsfont.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GTK_PLOT_GDK(obj)        GTK_CHECK_CAST (obj, gtk_plot_gdk_get_type (), GtkPlotGdk)
#define GTK_TYPE_PLOT_GDK   (gtk_plot_gdk_get_type ())

#define GTK_PLOT_GDK_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_gdk_get_type(), GtkPlotGdkClass)
#define GTK_IS_PLOT_GDK(obj)     GTK_CHECK_TYPE (obj, gtk_plot_gdk_get_type ())


typedef struct _GtkPlotGdk GtkPlotGdk;
typedef struct _GtkPlotGdkClass GtkPlotGdkClass;

struct _GtkPlotGdk
{
   GtkPlotPC pc;

   GdkGC *gc;
   GdkDrawable *drawable;

   GtkTextDirection text_direction;
   PangoContext *context;
   PangoLayout *layout;

   GdkWindow *window;

   gint ref_count;
};


struct _GtkPlotGdkClass
{
   GtkPlotPCClass parent_class;

   void (* set_drawable) 	(GtkPlotGdk *gdk, GdkDrawable *drawable);
};

GtkType    gtk_plot_gdk_get_type			(void);
GtkObject *gtk_plot_gdk_new				(GtkWidget *widget);
void	   gtk_plot_gdk_construct			(GtkPlotGdk *pc,
							 GtkWidget *widget);					 
void	   gtk_plot_gdk_set_drawable			(GtkPlotGdk *pc,
							 GdkDrawable *drawable);					 

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_GDK_H__ */

