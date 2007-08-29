/* gtkplotps - postscript driver
 * Copyright 1999-2001  Adrian E. Feiguin <feiguin@ifir.edu.ar>
 *
 * Some few lines of code borrowed from
 * DiaCanvas -- a technical canvas widget
 * Copyright (C) 1999 Arjan Molenaar
 * Dia -- an diagram creation/manipulation program
 * Copyright (C) 1998 Alexander Larsson
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

#ifndef __GTK_PLOT_PS_H__
#define __GTK_PLOT_PS_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GTK_PLOT_PS(obj)        GTK_CHECK_CAST (obj, gtk_plot_ps_get_type (), GtkPlotPS)
#define GTK_TYPE_PLOT_PS   (gtk_plot_ps_get_type ())

#define GTK_PLOT_PS_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_ps_get_type(), GtkPlotPSClass)
#define GTK_IS_PLOT_PS(obj)     GTK_CHECK_TYPE (obj, gtk_plot_ps_get_type ())


typedef struct _GtkPlotPS GtkPlotPS;
typedef struct _GtkPlotPSClass GtkPlotPSClass;

struct _GtkPlotPS
{
   GtkPlotPC pc;

   FILE *psfile;
   gchar *psname;
 
   gint orientation;
   gint epsflag;

   gint units;
   gint page_size;
   gint width, height;

   gint page_width;
   gint page_height;

   gdouble scalex, scaley;

   gboolean gsaved;
};

struct _GtkPlotPSClass
{
   GtkPlotPCClass parent_class;
};

GtkType    gtk_plot_ps_get_type                        (void);
GtkObject *gtk_plot_ps_new                             (const gchar *psname,
                                                        gint orientation,
                                                        gint epsflag,
                                                        gint page_size,
							gdouble scalex,
							gdouble scaley);

GtkObject *gtk_plot_ps_new_with_size                   (const gchar *psname,
                                                        gint orientation,
                                                        gint epsflag,
                                                        gint units,
                                                        gdouble width,
                                                        gdouble height,
							gdouble scalex,
							gdouble scaley);

void	gtk_plot_ps_construct                          (GtkPlotPS *ps,
							const gchar *psname,
                                                        gint orientation,
                                                        gint epsflag,
                                                        gint page_size,
							gdouble scalex,
							gdouble scaley);
void	gtk_plot_ps_construct_with_size                (GtkPlotPS *ps,
							const gchar *psname,
                                                        gint orientation,
                                                        gint epsflag,
                                                        gint units,
							gdouble width,
							gdouble height,
							gdouble scalex,
							gdouble scaley);

void gtk_plot_ps_set_size                              (GtkPlotPS *ps,
                                                        gint units,
                                                        gdouble width,
                                                        gdouble height);

void gtk_plot_ps_set_scale                             (GtkPlotPS *ps,
                                                        gdouble scalex,
                                                        gdouble scaley);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_PS_H__ */

