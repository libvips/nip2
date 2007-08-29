/* gtkplotdata - 2d scientific plots widget for gtk+
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
#include "gtkplotdata.h"
#include "gtkplotpolar.h"
#include "gtkplot3d.h"
#include "gtkplotpc.h"
#include "gtkpsfont.h"
#include "gtkextra-marshal.h"

#define DEFAULT_FONT_HEIGHT 18
#define P_(string) string

static gchar DEFAULT_FONT[] = "Helvetica";

static void gtk_plot_data_class_init 		(GtkPlotDataClass *klass);
static void gtk_plot_data_init 			(GtkPlotData *data);
static void gtk_plot_data_destroy		(GtkObject *object);
static void gtk_plot_data_get_property         (GObject      *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_data_set_property         (GObject      *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);

static void gtk_plot_data_real_clone		(GtkPlotData *real_data, 
						 GtkPlotData *copy_data);
static void gtk_plot_data_real_update		(GtkPlotData *data, 
						 gboolean new_range);
static void update_gradient			(GtkPlotData *data);
static void gtk_plot_data_draw_private 		(GtkPlotData *data);
static void gtk_plot_data_draw_legend		(GtkPlotData *data, 
						 gint x, gint y);
static void gtk_plot_data_draw_gradient		(GtkPlotData *data);
static void draw_gradient_vertical		(GtkPlotData *data, 
						 gdouble px, gdouble py);
static void draw_gradient_horizontal		(GtkPlotData *data, 
						 gdouble px, gdouble py);
static void gtk_plot_data_get_legend_size	(GtkPlotData *data, 
						 gint *width, gint *height);
static void gtk_plot_data_get_gradient_size	(GtkPlotData *data, 
						 gint *width, gint *height);
static void gtk_plot_data_real_draw		(GtkPlotData *data, 
						 gint npoints);
static void gtk_plot_data_real_real_draw	(GtkPlotData *data, 
						 gint npoints);
static void gtk_plot_data_real_draw_symbol	(GtkPlotData *data, 
						 gdouble x, 
                                                 gdouble y, 
                                                 gdouble z, 
                                                 gdouble a, 
						 gdouble dx, 
                                                 gdouble dy, 
                                                 gdouble dz, 
                                                 gdouble da); 
static void gtk_plot_data_draw_symbol_private	(GtkPlotData *data, 
						 gdouble x, gdouble y, 
						 GtkPlotSymbol symbol);
static void gtk_plot_data_draw_xyz 		(GtkPlotData *data,
						 gint npoints); 
static void gtk_plot_data_draw_errbars		(GtkPlotData *data, 
						 gdouble x, 
                                                 gdouble y, 
                                                 gdouble z, 
						 gdouble dx, 
                                                 gdouble dy, 
                                                 gdouble dz); 
static void gtk_plot_data_draw_down_triangle	(GtkPlotData *data, 
                            		  	 gdouble x,
 						 gdouble y, 
						 gdouble size, 
						 gint filled);
static void gtk_plot_data_draw_up_triangle	(GtkPlotData *data, 
                            		  	 gdouble x,
 						 gdouble y, 
						 gdouble size, 
						 gint filled);
static void gtk_plot_data_draw_right_triangle	(GtkPlotData *data, 
                            		  	 gdouble x,
 						 gdouble y, 
						 gdouble size, 
						 gint filled);
static void gtk_plot_data_draw_left_triangle	(GtkPlotData *data, 
                            		  	 gdouble x,
 						 gdouble y, 
						 gdouble size, 
						 gint filled);
static void gtk_plot_data_draw_diamond		(GtkPlotData *data, 
                            		  	 gdouble x,
 						 gdouble y, 
						 gdouble size, 
						 gint filled);
static void gtk_plot_data_draw_plus		(GtkPlotData *data, 
                            		  	 gdouble x,
 						 gdouble y, 
						 gdouble size); 
static void gtk_plot_data_draw_cross		(GtkPlotData *data, 
                            		  	 gdouble x,
 						 gdouble y, 
						 gdouble size); 
static void gtk_plot_data_draw_star		(GtkPlotData *data, 
                            		  	 gdouble x,
 						 gdouble y, 
						 gdouble size); 

static void gtk_plot_data_connect_points	(GtkPlotData *data, 
						 gint npoints);

extern inline gint roundint			(gdouble x);
static void spline_solve 			(int n, 
                                                 gdouble x[], gdouble y[], 
						 gdouble y2[]);
static gdouble spline_eval 			(int n, 
                                                 gdouble x[], 
                                                 gdouble y[], 
						 gdouble y2[], gdouble val);
static void hsv_to_rgb 				(gdouble  h, 
						 gdouble  s, 
						 gdouble  v,
						 gdouble *r,
						 gdouble *g,
						 gdouble *b);
static void rgb_to_hsv 				(gdouble  r, 
						 gdouble  g, 
						 gdouble  b,
            					 gdouble *h, 
						 gdouble *s, 
						 gdouble *v);
static void real_autoscale_gradient		(GtkPlotData *data, 
						 gdouble xmin, gdouble xmax);

static void draw_marker				(GtkPlotData *data, 
						 GtkPlotMarker *marker);

enum {
  ADD_TO_PLOT,
  UPDATE,
  DRAW_DATA,
  GRADIENT_CHANGED,
  GRADIENT_COLORS_CHANGED,
  LAST_SIGNAL
};

enum {
  ARG_0,
  ARG_NAME,
  ARG_LEGEND,
  ARG_ITERATOR,
  ARG_FUNCTION,
  ARG_ITERATOR_MASK,
  ARG_NUM_POINTS,
  ARG_FILL_AREA,
  ARG_XSTEP,
  ARG_YSTEP,
  ARG_ZSTEP,
  ARG_SYMBOL_TYPE,
  ARG_SYMBOL_STYLE,
  ARG_SYMBOL_SIZE,
  ARG_SYMBOL_COLOR,
  ARG_SYMBOL_BORDER_WIDTH,
  ARG_SYMBOL_BORDER_COLOR,
  ARG_LINE_STYLE,
  ARG_LINE_CAP,
  ARG_LINE_JOIN,
  ARG_LINE_WIDTH,
  ARG_LINE_COLOR,
  ARG_LINE_CONNECTOR,
  ARG_XLINE_STYLE,
  ARG_XLINE_CAP,
  ARG_XLINE_JOIN,
  ARG_XLINE_WIDTH,
  ARG_XLINE_COLOR,
  ARG_YLINE_STYLE,
  ARG_YLINE_CAP,
  ARG_YLINE_JOIN,
  ARG_YLINE_WIDTH,
  ARG_YLINE_COLOR,
  ARG_ZLINE_STYLE,
  ARG_ZLINE_CAP,
  ARG_ZLINE_JOIN,
  ARG_ZLINE_WIDTH,
  ARG_ZLINE_COLOR,
  ARG_SHOW_XERRBARS,
  ARG_XERRBARS_WIDTH,
  ARG_XERRBARS_CAPS,
  ARG_SHOW_YERRBARS,
  ARG_YERRBARS_WIDTH,
  ARG_YERRBARS_CAPS,
  ARG_SHOW_ZERRBARS,
  ARG_ZERRBARS_WIDTH,
  ARG_ZERRBARS_CAPS,
  ARG_SHOW_LEGENDS,
  ARG_LEGENDS_PRECISION,
  ARG_LEGENDS_STYLE,
  ARG_SHOW_LABELS,
  ARG_LABELS_TRANSPARENT,
  ARG_LABELS_ANGLE,
  ARG_LABELS_BORDER,
  ARG_LABELS_BORDER_SPACE,
  ARG_LABELS_BORDER_WIDTH,
  ARG_LABELS_BORDER_SHADOW,
  ARG_LABELS_FG,
  ARG_LABELS_BG,
  ARG_LABELS_OFFSET,
  ARG_LABELS_SIZE,
  ARG_LABELS_FONT,
  ARG_LABELS_TEXT,
  ARG_SHOW_GRADIENT,
  ARG_GRADIENT_CUSTOM,
  ARG_GRADIENT_MASK,
  ARG_COLOR_MIN,
  ARG_COLOR_MAX,
  ARG_COLOR_LT_MIN,
  ARG_COLOR_GT_MAX,
  ARG_GRADIENT_MAX,
  ARG_GRADIENT_MIN,
  ARG_GRADIENT_STEP,
  ARG_GRADIENT_NMINOR,
  ARG_GRADIENT_NMAJOR,
  ARG_GRADIENT_SCALE,
  ARG_GRADIENT_BREAK,
  ARG_GRADIENT_BREAK_MAX,
  ARG_GRADIENT_BREAK_MIN,
  ARG_GRADIENT_BREAK_STEP,
  ARG_GRADIENT_BREAK_NMINOR,
  ARG_GRADIENT_BREAK_SCALE,
  ARG_GRADIENT_BREAK_POSITION,
};

static GtkWidgetClass *parent_class = NULL;
static guint data_signals[LAST_SIGNAL] = { 0 };

GtkType
gtk_plot_marker_get_type (void)
{
  static GtkType marker_type = 0;

  if (!marker_type)
    {
      GtkTypeInfo data_info =
      {
        "GtkPlotMarker",
        0,
        0,
        (GtkClassInitFunc) NULL,
        (GtkObjectInitFunc) NULL,
        /* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      marker_type = gtk_type_unique (GTK_TYPE_BOXED, &data_info);
    }
  return marker_type;
}

GtkType
gtk_plot_data_get_type (void)
{
  static GtkType data_type = 0;

  if (!data_type)
    {
      GtkTypeInfo data_info =
      {
	"GtkPlotData",
	sizeof (GtkPlotData),
	sizeof (GtkPlotDataClass),
	(GtkClassInitFunc) gtk_plot_data_class_init,
	(GtkObjectInitFunc) gtk_plot_data_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      data_type = gtk_type_unique (GTK_TYPE_WIDGET, &data_info);
    }
  return data_type;
}

static void
gtk_plot_data_class_init (GtkPlotDataClass *klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkPlotDataClass *data_class;
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  parent_class = gtk_type_class (gtk_widget_get_type ());

  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;
  data_class = (GtkPlotDataClass *) klass;

  object_class->destroy = gtk_plot_data_destroy;

  gobject_class->set_property = gtk_plot_data_set_property;
  gobject_class->get_property = gtk_plot_data_get_property;

  g_object_class_install_property (gobject_class,
                           ARG_NAME,
  g_param_spec_string ("name",
                           P_(""),
                           P_(""),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LEGEND,
  g_param_spec_string ("legend",
                           P_(""),
                           P_(""),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ITERATOR,
  g_param_spec_boolean ("is_iterator",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_FUNCTION,
  g_param_spec_boolean ("is_function",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ITERATOR_MASK,
  g_param_spec_int ("iterator_mask",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_NUM_POINTS,
  g_param_spec_int ("num_points",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_FILL_AREA,
  g_param_spec_boolean ("fill_area",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XSTEP,
  g_param_spec_double ("xstep",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YSTEP,
  g_param_spec_double ("ystep",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ZSTEP,
  g_param_spec_double ("zstep",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SYMBOL_TYPE,
  g_param_spec_int ("symbol_type",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SYMBOL_STYLE,
  g_param_spec_int ("symbol_style",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SYMBOL_SIZE,
  g_param_spec_int ("symbol_size",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SYMBOL_COLOR,
  g_param_spec_pointer ("symbol_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SYMBOL_BORDER_WIDTH,
  g_param_spec_double ("symbol_border_width",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SYMBOL_BORDER_COLOR,
  g_param_spec_pointer ("symbol_border_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LINE_STYLE,
  g_param_spec_int ("line_style",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LINE_CAP,
  g_param_spec_int ("line_cap",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LINE_JOIN,
  g_param_spec_int ("line_join",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LINE_WIDTH,
  g_param_spec_double ("line_width",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LINE_COLOR,
  g_param_spec_pointer ("line_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LINE_CONNECTOR,
  g_param_spec_int ("line_connector",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XLINE_STYLE,
  g_param_spec_int ("xline_style",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XLINE_CAP,
  g_param_spec_int ("xline_cap",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XLINE_JOIN,
  g_param_spec_int ("xline_join",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XLINE_WIDTH,
  g_param_spec_double ("xline_width",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XLINE_COLOR,
  g_param_spec_pointer ("xline_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YLINE_STYLE,
  g_param_spec_int ("yline_style",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YLINE_CAP,
  g_param_spec_int ("yline_cap",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YLINE_JOIN,
  g_param_spec_int ("yline_join",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YLINE_WIDTH,
  g_param_spec_double ("yline_width",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YLINE_COLOR,
  g_param_spec_pointer ("yline_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ZLINE_STYLE,
  g_param_spec_int ("zline_style",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ZLINE_CAP,
  g_param_spec_int ("zline_cap",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ZLINE_JOIN,
  g_param_spec_int ("zline_join",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ZLINE_WIDTH,
  g_param_spec_double ("zline_width",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ZLINE_COLOR,
  g_param_spec_pointer ("zline_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHOW_XERRBARS,
  g_param_spec_boolean ("show_xerrbars",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XERRBARS_WIDTH,
  g_param_spec_int ("xerrbar_width",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_XERRBARS_CAPS,
  g_param_spec_int ("xerrbar_caps",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHOW_YERRBARS,
  g_param_spec_boolean ("show_yerrbars",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YERRBARS_WIDTH,
  g_param_spec_int ("yerrbar_width",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_YERRBARS_CAPS,
  g_param_spec_int ("yerrbar_caps",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHOW_ZERRBARS,
  g_param_spec_boolean ("show_zerrbars",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ZERRBARS_WIDTH,
  g_param_spec_int ("zerrbar_width",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_ZERRBARS_CAPS,
  g_param_spec_int ("zerrbar_caps",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHOW_LEGENDS,
  g_param_spec_boolean ("show_legends",
                           P_(""),
                           P_(""),
                           TRUE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LEGENDS_PRECISION,
  g_param_spec_int ("legends_precision",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LEGENDS_STYLE,
  g_param_spec_int ("legends_style",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHOW_LABELS,
  g_param_spec_boolean ("show_labels",
                           P_(""),
                           P_(""),
                           TRUE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_BORDER,
  g_param_spec_int ("labels_border",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_BORDER_SPACE,
  g_param_spec_int ("labels_border_space",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_BORDER_WIDTH,
  g_param_spec_int ("labels_border_width",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_BORDER_SHADOW,
  g_param_spec_int ("labels_border_shadow",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_TRANSPARENT,
  g_param_spec_boolean ("labels_transparent",
                           P_(""),
                           P_(""),
                           TRUE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_FG,
  g_param_spec_pointer ("labels_fg_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_BG,
  g_param_spec_pointer ("labels_bg_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_ANGLE,
  g_param_spec_int ("labels_angle",
                           P_(""),
                           P_(""),
                           0,270,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_OFFSET,
  g_param_spec_int ("labels_offset",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_SIZE,
  g_param_spec_int ("labels_size",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_FONT,
  g_param_spec_string ("labels_font",
                           P_(""),
                           P_(""),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_LABELS_TEXT,
  g_param_spec_string ("labels_text",
                           P_(""),
                           P_(""),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_SHOW_GRADIENT,
  g_param_spec_boolean ("show_gradient",
                           P_(""),
                           P_(""),
                           TRUE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_CUSTOM,
  g_param_spec_boolean ("gradient_custom",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_MASK,
  g_param_spec_int ("gradient_mask",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_COLOR_MIN,
  g_param_spec_pointer ("color_min",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_COLOR_MAX,
  g_param_spec_pointer ("color_max",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_COLOR_LT_MIN,
  g_param_spec_pointer ("color_lt_min",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_COLOR_GT_MAX,
  g_param_spec_pointer ("color_gt_max",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_MAX,
  g_param_spec_double ("gradient_max",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_MIN,
  g_param_spec_double ("gradient_min",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_STEP,
  g_param_spec_double ("gradient_step",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_NMINOR,
  g_param_spec_int ("gradient_nminor",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_NMAJOR,
  g_param_spec_int ("gradient_nmajor",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_SCALE,
  g_param_spec_int ("gradient_scale",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_BREAK,
  g_param_spec_boolean ("gradient_break",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_BREAK_MAX,
  g_param_spec_double ("gradient_break_max",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_BREAK_MIN,
  g_param_spec_double ("gradient_break_min",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_BREAK_STEP,
  g_param_spec_double ("gradient_break_step",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_BREAK_NMINOR,
  g_param_spec_int ("gradient_break_nminor",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_BREAK_SCALE,
  g_param_spec_int ("gradient_break_scale",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           ARG_GRADIENT_BREAK_POSITION,
  g_param_spec_double ("gradient_break_position",
                           P_(""),
                           P_(""),
                           0.,1.,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));

  data_class->clone = gtk_plot_data_real_clone;
  data_class->add_to_plot = NULL;
  data_class->update = gtk_plot_data_real_update;
  data_class->gradient_changed = update_gradient;
  data_class->gradient_colors_changed = update_gradient;
  data_class->draw_symbol = gtk_plot_data_real_draw_symbol;
  data_class->draw_data = gtk_plot_data_draw_private;
  data_class->draw_legend = gtk_plot_data_draw_legend;
  data_class->draw_gradient = gtk_plot_data_draw_gradient;
  data_class->get_legend_size = gtk_plot_data_get_legend_size;
  data_class->get_gradient_size = gtk_plot_data_get_gradient_size;

  data_signals[ADD_TO_PLOT] =
    gtk_signal_new ("add_to_plot",
                    GTK_RUN_LAST,
                    GTK_CLASS_TYPE(object_class),
                    GTK_SIGNAL_OFFSET (GtkPlotDataClass, add_to_plot),
                    gtkextra_BOOL__POINTER,
                    GTK_TYPE_BOOL, 1, GTK_TYPE_PLOT);

  data_signals[UPDATE] =
    gtk_signal_new ("update",
                    GTK_RUN_LAST,
                    GTK_CLASS_TYPE(object_class),
                    GTK_SIGNAL_OFFSET (GtkPlotDataClass, update),
                    gtkextra_VOID__BOOL,
                    GTK_TYPE_NONE, 1, GTK_TYPE_BOOL);

  data_signals[DRAW_DATA] =
    gtk_signal_new ("draw_data",
                    GTK_RUN_FIRST,
                    GTK_CLASS_TYPE(object_class),
                    GTK_SIGNAL_OFFSET (GtkPlotDataClass, draw_data),
                    gtkextra_VOID__VOID,
                    GTK_TYPE_NONE, 0, GTK_TYPE_NONE);

  data_signals[GRADIENT_CHANGED] =
    gtk_signal_new ("gradient_changed",
                    GTK_RUN_LAST,
                    GTK_CLASS_TYPE(object_class),
                    GTK_SIGNAL_OFFSET (GtkPlotDataClass, gradient_changed),
                    gtkextra_VOID__VOID,
                    GTK_TYPE_NONE, 0);

  data_signals[GRADIENT_COLORS_CHANGED] =
    gtk_signal_new ("gradient_colors_changed",
                    GTK_RUN_LAST,
                    GTK_CLASS_TYPE(object_class),
                    GTK_SIGNAL_OFFSET (GtkPlotDataClass, gradient_colors_changed),
                    gtkextra_VOID__VOID,
                    GTK_TYPE_NONE, 0);

}

static void
gtk_plot_data_init (GtkPlotData *dataset)
{
  GtkWidget *widget;
  GdkColormap *colormap;
  GdkColor black, white, color;

  GTK_WIDGET_SET_FLAGS(dataset, GTK_NO_WINDOW);

  widget = GTK_WIDGET(dataset);
  colormap = gtk_widget_get_colormap(widget);

  gdk_color_black(colormap, &black);
  gdk_color_white(colormap, &white);

  dataset->gradient = GTK_PLOT_AXIS(gtk_plot_axis_new(GTK_PLOT_AXIS_Y));
  gtk_object_ref(GTK_OBJECT(dataset->gradient));
  gtk_object_sink(GTK_OBJECT(dataset->gradient));

  dataset->color_lt_min = white;
  dataset->color_gt_max = white;

  gdk_color_parse("red", &color);
  gdk_color_alloc(colormap, &color);
  dataset->color_max = color;

  gdk_color_parse("blue", &color);
  gdk_color_alloc(colormap, &color);
  dataset->color_min = color;

  dataset->gradient_colors = NULL;

  g_free(dataset->gradient->title.text);
  dataset->gradient->title.text = g_strdup("Amplitude");

  dataset->gradient->ticks.min = 0.0;
  dataset->gradient->ticks.max = 1.0;
  dataset->gradient->ticks.nmajorticks = 10;
  dataset->gradient->ticks.nminorticks = 0;
  dataset->gradient->ticks.nminor = 0;
  dataset->gradient->ticks.values = NULL;
  dataset->gradient->ticks.scale = GTK_PLOT_SCALE_LINEAR;
  dataset->gradient->ticks.apply_break = FALSE;
  dataset->gradient->ticks_length = 4;
  dataset->gradient->label_precision = 3;
  dataset->gradient->label_style = GTK_PLOT_LABEL_FLOAT;
  dataset->gradient->labels_offset = 4;
  dataset->gradient_mask = GTK_PLOT_GRADIENT_H;
  dataset->gradient_title_pos = GTK_PLOT_AXIS_RIGHT;
  dataset->gradient_x = .6;
  dataset->gradient_y = .05;
  dataset->gradient->orientation = GTK_ORIENTATION_VERTICAL;
  dataset->gradient->label_mask = GTK_PLOT_LABEL_OUT;
  dataset->gradient->labels_attr.fg = widget->style->black;
  dataset->gradient->labels_attr.bg = widget->style->white;
  dataset->gradient->labels_attr.transparent = FALSE;
  dataset->gradient->labels_attr.border = 0;
  dataset->gradient->labels_attr.border_width = 0;
  dataset->gradient->labels_attr.shadow_width = 0;
  dataset->gradient_border_width = 1;
  dataset->gradient_border_offset = 4;
  dataset->gradient_shadow_width = 3;
  dataset->gradient_border = GTK_PLOT_BORDER_LINE;

/*
  g_free(dataset->gradient->title.text);
  dataset->gradient->title.text = g_strdup("AHGLGHKSHGKSHKSHLKSJHLKSHJSKLHJSKAJ");
*/

  dataset->gradient_custom = FALSE;
  dataset->gradient_line_width = 30;
  dataset->gradient_line_height = 10;
  dataset->show_gradient = FALSE;
  dataset->legends_precision = 3;
  dataset->legends_style = GTK_PLOT_LABEL_FLOAT;
  gtk_plot_data_reset_gradient(dataset);

  dataset->plot = NULL;

  dataset->is_function = FALSE;
  dataset->is_iterator = FALSE;
  dataset->iterator_mask = GTK_PLOT_DATA_X|GTK_PLOT_DATA_Y;
  dataset->show_legend = TRUE;
  dataset->show_labels = FALSE;
  dataset->fill_area = FALSE;
  dataset->function = NULL;
  dataset->iterator = NULL;
  dataset->num_points = 0;

  dataset->x_step = .5;
  dataset->y_step = .5;
  dataset->line_connector = GTK_PLOT_CONNECT_STRAIGHT;

  dataset->line.line_style = GTK_PLOT_LINE_SOLID;
  dataset->line.cap_style = 0;
  dataset->line.join_style = 0;
  dataset->line.line_width = 1;
  dataset->line.color = black; 

  dataset->x_line.line_style = GTK_PLOT_LINE_NONE;
  dataset->x_line.cap_style = 0;
  dataset->x_line.join_style = 0;
  dataset->x_line.line_width = 1;
  dataset->x_line.color = black; 

  dataset->y_line.line_style = GTK_PLOT_LINE_NONE;
  dataset->y_line.cap_style = 0;
  dataset->y_line.join_style = 0;
  dataset->y_line.line_width = 1;
  dataset->y_line.color = black; 

  dataset->z_line.line_style = GTK_PLOT_LINE_NONE;
  dataset->z_line.cap_style = 0;
  dataset->z_line.join_style = 0;
  dataset->z_line.line_width = 1;
  dataset->z_line.color = black; 

  dataset->symbol.symbol_type = GTK_PLOT_SYMBOL_NONE;
  dataset->symbol.symbol_style = GTK_PLOT_SYMBOL_EMPTY;
  dataset->symbol.size = 6;
  dataset->symbol.border.line_width = 1;
  dataset->symbol.border.color = black; 
  dataset->symbol.color = black; 

  dataset->show_xerrbars = FALSE;
  dataset->show_yerrbars = FALSE;
  dataset->show_zerrbars = FALSE;
  dataset->xerrbar_width = 1;
  dataset->yerrbar_width = 1;
  dataset->zerrbar_width = 1;
  dataset->xerrbar_caps = 8;
  dataset->yerrbar_caps = 8;
  dataset->zerrbar_caps = 8;

  dataset->legend = NULL;
  dataset->name = NULL;

  dataset->labels_attr.justification = GTK_JUSTIFY_LEFT;
  dataset->labels_attr.transparent = TRUE;
  dataset->labels_attr.font = g_strdup(DEFAULT_FONT);
  dataset->labels_attr.text = NULL;
  dataset->labels_attr.border_space = 3;
  dataset->labels_offset = 6;

  dataset->link = NULL;

  dataset->markers = NULL;
  dataset->show_markers = TRUE;

  gtk_plot_data_labels_set_attributes(dataset, 
                                      DEFAULT_FONT,
                                      DEFAULT_FONT_HEIGHT,
                                      90,
                                      &black,
                                      &white);


  gtk_psfont_init();
  dataset->redraw_pending = TRUE;

  dataset->data = GTK_PLOT_ARRAY_LIST(gtk_plot_array_list_new());
  g_object_ref(G_OBJECT(dataset->data));
  
  gtk_plot_data_add_dimension(dataset, "x", "X" , "X points", GTK_TYPE_DOUBLE, TRUE, TRUE);
  gtk_plot_data_add_dimension(dataset, "y", "Y" , "Y points", GTK_TYPE_DOUBLE, TRUE, FALSE);
  gtk_plot_data_add_dimension(dataset, "z", "Z" , "Z points", GTK_TYPE_DOUBLE, TRUE, FALSE);
  gtk_plot_data_add_dimension(dataset, "a", "Size" , "Symbol size", GTK_TYPE_DOUBLE, FALSE, FALSE);
  gtk_plot_data_add_dimension(dataset, "da", "Amp" , "Amplitude", GTK_TYPE_DOUBLE, FALSE, FALSE);
  gtk_plot_data_add_dimension(dataset, "dx", "ErrX" , "Error in X", GTK_TYPE_DOUBLE, FALSE, FALSE);
  gtk_plot_data_add_dimension(dataset, "dy", "ErrY" , "Error in Y", GTK_TYPE_DOUBLE, FALSE, FALSE);
  gtk_plot_data_add_dimension(dataset, "dz", "ErrZ" , "Error in Z", GTK_TYPE_DOUBLE, FALSE, FALSE);
  gtk_plot_data_add_dimension(dataset, "labels", "Labels" , "Data labels", GTK_TYPE_STRING, FALSE, FALSE);
}

static void
gtk_plot_data_destroy (GtkObject *object)
{
  GtkPlotData *data;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GTK_IS_PLOT_DATA (object));

  data = GTK_PLOT_DATA (object);

  if(data->labels_attr.font) g_free(data->labels_attr.font);
  if(data->labels_attr.text) g_free(data->labels_attr.text);
  if(data->legend) g_free(data->legend);
  if(data->name) g_free(data->name);

  if(data->gradient) gtk_object_unref(GTK_OBJECT(data->gradient));
  data->gradient = NULL;

  if(data->gradient_colors){
     g_free(data->gradient_colors);
     data->gradient_colors = NULL;
  }

  gtk_plot_data_remove_markers(data);

  if(data->data){
    g_object_unref(G_OBJECT(data->data));
    data->data = NULL;
  }

  gtk_psfont_unref();
}

static void
gtk_plot_data_set_property (GObject      *object,
                             guint            prop_id,
                             const GValue          *value,
                             GParamSpec      *pspec)
{
  GtkPlotData *data;

  data = GTK_PLOT_DATA (object);

  switch (prop_id)
    {
      case ARG_NAME:
        gtk_plot_data_set_name(data, g_value_get_string(value));
        break;
      case ARG_LEGEND:
        gtk_plot_data_set_legend(data, g_value_get_string(value));
        break;
      case ARG_ITERATOR:
        data->is_iterator = g_value_get_boolean(value);
        break;
      case ARG_FUNCTION:
        data->is_function = g_value_get_boolean(value);
        break;
      case ARG_ITERATOR_MASK:
        data->iterator_mask = g_value_get_int(value);
        break;
      case ARG_NUM_POINTS:
        data->num_points = g_value_get_int(value);
        break;
      case ARG_FILL_AREA:
        data->fill_area = g_value_get_boolean(value);
        break;
      case ARG_XSTEP:
        data->x_step = g_value_get_double(value);
        break;
      case ARG_YSTEP:
        data->y_step = g_value_get_double(value);
        break;
      case ARG_ZSTEP:
        data->z_step = g_value_get_double(value);
        break;
      case ARG_SYMBOL_TYPE:
        data->symbol.symbol_type = g_value_get_int(value);
        break;
      case ARG_SYMBOL_STYLE:
        data->symbol.symbol_style = g_value_get_int(value);
        break;
      case ARG_SYMBOL_SIZE:
        data->symbol.size = g_value_get_int(value);
        break;
      case ARG_SYMBOL_COLOR:
        data->symbol.color = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_SYMBOL_BORDER_WIDTH:
        data->symbol.border.line_width = g_value_get_double(value);
        break;
      case ARG_SYMBOL_BORDER_COLOR:
        data->symbol.border.color = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_LINE_STYLE:
        data->line.line_style = g_value_get_int(value);
        break;
      case ARG_LINE_CAP:
        data->line.cap_style = g_value_get_int(value);
        break;
      case ARG_LINE_JOIN:
        data->line.join_style = g_value_get_int(value);
        break;
      case ARG_LINE_WIDTH:
        data->line.line_width = g_value_get_double(value);
        break;
      case ARG_LINE_COLOR:
        data->line.color = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_LINE_CONNECTOR:
        data->line_connector = g_value_get_int(value);
        break;
      case ARG_XLINE_STYLE:
        data->x_line.line_style = g_value_get_int(value);
        break;
      case ARG_XLINE_CAP:
        data->x_line.cap_style = g_value_get_int(value);
        break;
      case ARG_XLINE_JOIN:
        data->x_line.join_style = g_value_get_int(value);
        break;
      case ARG_XLINE_WIDTH:
        data->x_line.line_width = g_value_get_double(value);
        break;
      case ARG_XLINE_COLOR:
        data->x_line.color = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_YLINE_STYLE:
        data->y_line.line_style = g_value_get_int(value);
        break;
      case ARG_YLINE_CAP:
        data->y_line.cap_style = g_value_get_int(value);
        break;
      case ARG_YLINE_JOIN:
        data->y_line.join_style = g_value_get_int(value);
        break;
      case ARG_YLINE_WIDTH:
        data->y_line.line_width = g_value_get_double(value);
        break;
      case ARG_YLINE_COLOR:
        data->y_line.color = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_ZLINE_STYLE:
        data->z_line.line_style = g_value_get_int(value);
        break;
      case ARG_ZLINE_CAP:
        data->z_line.cap_style = g_value_get_int(value);
        break;
      case ARG_ZLINE_JOIN:
        data->z_line.join_style = g_value_get_int(value);
        break;
      case ARG_ZLINE_WIDTH:
        data->z_line.line_width = g_value_get_double(value);
        break;
      case ARG_ZLINE_COLOR:
        data->z_line.color = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_SHOW_XERRBARS:
        data->show_xerrbars = g_value_get_boolean(value);
        break;
      case ARG_XERRBARS_WIDTH:
        data->xerrbar_width = g_value_get_int(value);
        break;
      case ARG_XERRBARS_CAPS:
        data->xerrbar_caps = g_value_get_int(value);
        break;
      case ARG_SHOW_YERRBARS:
        data->show_yerrbars = g_value_get_boolean(value);
        break;
      case ARG_YERRBARS_WIDTH:
        data->yerrbar_width = g_value_get_int(value);
        break;
      case ARG_YERRBARS_CAPS:
        data->yerrbar_caps = g_value_get_int(value);
        break;
      case ARG_SHOW_ZERRBARS:
        data->show_zerrbars = g_value_get_boolean(value);
        break;
      case ARG_ZERRBARS_WIDTH:
        data->zerrbar_width = g_value_get_int(value);
        break;
      case ARG_ZERRBARS_CAPS:
        data->zerrbar_caps = g_value_get_int(value);
        break;
      case ARG_SHOW_LEGENDS:
        data->show_legend = g_value_get_boolean(value);
        break;
      case ARG_LEGENDS_PRECISION:
        data->legends_precision = g_value_get_int(value);
        break;
      case ARG_LEGENDS_STYLE:
        data->legends_style = g_value_get_int(value);
        break;
      case ARG_SHOW_LABELS:
        data->show_labels = g_value_get_boolean(value);
        break;
      case ARG_LABELS_TRANSPARENT:
        data->labels_attr.transparent = g_value_get_boolean(value);
        break;
      case ARG_LABELS_ANGLE:
        data->labels_attr.angle = g_value_get_int(value);
        break;
      case ARG_LABELS_BORDER:
        data->labels_attr.border = g_value_get_int(value);
        break;
      case ARG_LABELS_BORDER_SPACE:
        data->labels_attr.border_space = g_value_get_int(value);
        break;
      case ARG_LABELS_BORDER_WIDTH:
        data->labels_attr.border_width = g_value_get_int(value);
        break;
      case ARG_LABELS_BORDER_SHADOW:
        data->labels_attr.shadow_width = g_value_get_int(value);
        break;
      case ARG_LABELS_FG:
        data->labels_attr.fg = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_LABELS_BG:
        data->labels_attr.bg = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_LABELS_OFFSET:
        data->labels_offset = g_value_get_int(value);
        break;
      case ARG_LABELS_SIZE:
        data->labels_attr.height = g_value_get_int(value);
        break;
      case ARG_LABELS_FONT:
        if(data->labels_attr.font) g_free(data->labels_attr.font);
        data->labels_attr.font = g_strdup(g_value_get_string(value));
        break;
      case ARG_LABELS_TEXT:
        if(data->labels_attr.text) g_free(data->labels_attr.text);
        data->labels_attr.text = g_strdup(g_value_get_string(value));
        break;
      case ARG_SHOW_GRADIENT:
        data->show_gradient = g_value_get_boolean(value);
        break;
      case ARG_GRADIENT_CUSTOM:
        data->gradient_custom = g_value_get_boolean(value);
        break;
      case ARG_GRADIENT_MASK:
        data->gradient_mask = g_value_get_int(value);
        break;
      case ARG_COLOR_MIN:
        data->color_min = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_COLOR_MAX:
        data->color_max = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_COLOR_LT_MIN:
        data->color_lt_min = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_COLOR_GT_MAX:
        data->color_gt_max = *((GdkColor *)g_value_get_pointer(value));
        break;
      case ARG_GRADIENT_MAX:
        data->gradient->ticks.max = g_value_get_double(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_MIN:
        data->gradient->ticks.min = g_value_get_double(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_STEP:
        data->gradient->ticks.step = g_value_get_double(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_NMINOR:
        data->gradient->ticks.nminor = g_value_get_int(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_NMAJOR:
        data->gradient->ticks.nmajorticks = g_value_get_int(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_SCALE:
        data->gradient->ticks.scale = g_value_get_int(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_BREAK:
        data->gradient->ticks.apply_break = g_value_get_boolean(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_BREAK_MAX:
        data->gradient->ticks.break_max = g_value_get_double(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_BREAK_MIN:
        data->gradient->ticks.break_min = g_value_get_double(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_BREAK_STEP:
        data->gradient->ticks.break_step = g_value_get_double(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_BREAK_NMINOR:
        data->gradient->ticks.break_nminor = g_value_get_int(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_BREAK_SCALE:
        data->gradient->ticks.break_scale = g_value_get_int(value);
        gtk_plot_data_reset_gradient(data);
        break;
      case ARG_GRADIENT_BREAK_POSITION:
        data->gradient->ticks.break_position = g_value_get_double(value);
        break;
    }
}

static void
gtk_plot_data_get_property (GObject      *object,
                             guint            prop_id,
                             GValue          *value,
                             GParamSpec      *pspec)
{
  GtkPlotData *data;

  data = GTK_PLOT_DATA (object);

  switch (prop_id)
    {
      case ARG_NAME:
        g_value_set_string(value, data->name);
        break;
      case ARG_LEGEND:
        g_value_set_string(value, data->legend);
        break;
      case ARG_ITERATOR:
        g_value_set_boolean(value, data->is_iterator);
        break;
      case ARG_FUNCTION:
        g_value_set_boolean(value, data->is_function);
        break;
      case ARG_ITERATOR_MASK:
        g_value_set_int(value, data->iterator_mask);
        break;
      case ARG_NUM_POINTS:
        g_value_set_int(value, data->num_points);
        break;
      case ARG_FILL_AREA:
        g_value_set_boolean(value, data->fill_area);
        break;
      case ARG_XSTEP:
        g_value_set_double(value, data->x_step);
        break;
      case ARG_YSTEP:
        g_value_set_double(value, data->y_step);
        break;
      case ARG_ZSTEP:
        g_value_set_double(value, data->z_step);
        break;
      case ARG_SYMBOL_TYPE:
        g_value_set_int(value, data->symbol.symbol_type);
        break;
      case ARG_SYMBOL_STYLE:
        g_value_set_int(value, data->symbol.symbol_style);
        break;
      case ARG_SYMBOL_SIZE:
        g_value_set_int(value, data->symbol.size);
        break;
      case ARG_SYMBOL_COLOR:
        g_value_set_pointer(value, &data->symbol.color);
        break;
      case ARG_SYMBOL_BORDER_WIDTH:
        g_value_set_double(value, data->symbol.border.line_width);
        break;
      case ARG_SYMBOL_BORDER_COLOR:
        g_value_set_pointer(value, &data->symbol.border.color);
        break;
      case ARG_LINE_STYLE:
        g_value_set_int(value, data->line.line_style);
        break;
      case ARG_LINE_CAP:
        g_value_set_int(value, data->line.cap_style);
        break;
      case ARG_LINE_JOIN:
        g_value_set_int(value, data->line.join_style);
        break;
      case ARG_LINE_WIDTH:
        g_value_set_double(value, data->line.line_width);
        break;
      case ARG_LINE_COLOR:
        g_value_set_pointer(value,  &data->line.color);
        break;
      case ARG_LINE_CONNECTOR:
        g_value_set_int(value, data->line_connector);
        break;
      case ARG_XLINE_STYLE:
        g_value_set_int(value, data->x_line.line_style);
        break;
      case ARG_XLINE_CAP:
        g_value_set_int(value, data->x_line.cap_style);
        break;
      case ARG_XLINE_JOIN:
        g_value_set_int(value, data->x_line.join_style);
        break;
      case ARG_XLINE_WIDTH:
        g_value_set_double(value, data->x_line.line_width);
        break;
      case ARG_XLINE_COLOR:
        g_value_set_pointer(value, &data->x_line.color);
        break;
      case ARG_YLINE_STYLE:
        g_value_set_int(value, data->y_line.line_style);
        break;
      case ARG_YLINE_CAP:
        g_value_set_int(value, data->y_line.cap_style);
        break;
      case ARG_YLINE_JOIN:
        g_value_set_int(value, data->y_line.join_style);
        break;
      case ARG_YLINE_WIDTH:
        g_value_set_double(value, data->y_line.line_width);
        break;
      case ARG_YLINE_COLOR:
        g_value_set_pointer(value, &data->y_line.color);
        break;
      case ARG_ZLINE_STYLE:
        g_value_set_int(value, data->z_line.line_style);
        break;
      case ARG_ZLINE_CAP:
        g_value_set_int(value, data->z_line.cap_style);
        break;
      case ARG_ZLINE_JOIN:
        g_value_set_int(value, data->z_line.join_style);
        break;
      case ARG_ZLINE_WIDTH:
        g_value_set_double(value, data->z_line.line_width);
        break;
      case ARG_ZLINE_COLOR:
        g_value_set_pointer(value, &data->z_line.color);
        break;
      case ARG_SHOW_XERRBARS:
        g_value_set_boolean(value, data->show_xerrbars);
        break;
      case ARG_XERRBARS_WIDTH:
        g_value_set_int(value, data->xerrbar_width);
        break;
      case ARG_XERRBARS_CAPS:
        g_value_set_int(value, data->xerrbar_caps);
        break;
      case ARG_SHOW_YERRBARS:
        g_value_set_boolean(value, data->show_yerrbars);
        break;
      case ARG_YERRBARS_WIDTH:
        g_value_set_int(value, data->yerrbar_width);
        break;
      case ARG_YERRBARS_CAPS:
        g_value_set_int(value, data->yerrbar_caps);
        break;
      case ARG_SHOW_ZERRBARS:
        g_value_set_boolean(value, data->show_zerrbars);
        break;
      case ARG_ZERRBARS_WIDTH:
        g_value_set_int(value, data->zerrbar_width);
        break;
      case ARG_ZERRBARS_CAPS:
        g_value_set_int(value, data->zerrbar_caps);
        break;
      case ARG_SHOW_LEGENDS:
        g_value_set_boolean(value, data->show_legend);
        break;
      case ARG_LEGENDS_PRECISION:
        g_value_set_int(value, data->legends_precision);
        break;
      case ARG_LEGENDS_STYLE:
        g_value_set_int(value, data->legends_style);
        break;
      case ARG_SHOW_LABELS:
        g_value_set_boolean(value, data->show_labels);
        break;
      case ARG_LABELS_TRANSPARENT:
        g_value_set_boolean(value, data->labels_attr.transparent);
        break;
      case ARG_LABELS_ANGLE:
        g_value_set_int(value, data->labels_attr.angle);
        break;
      case ARG_LABELS_BORDER:
        g_value_set_int(value, data->labels_attr.border);
        break;
      case ARG_LABELS_BORDER_SPACE:
        g_value_set_int(value, data->labels_attr.border_space);
        break;
      case ARG_LABELS_BORDER_WIDTH:
        g_value_set_int(value, data->labels_attr.border_width);
        break;
      case ARG_LABELS_BORDER_SHADOW:
        g_value_set_int(value, data->labels_attr.shadow_width);
        break;
      case ARG_LABELS_FG:
        g_value_set_pointer(value, &data->labels_attr.fg);
        break;
      case ARG_LABELS_BG:
        g_value_set_pointer(value, &data->labels_attr.bg);
        break;
      case ARG_LABELS_OFFSET:
        g_value_set_int(value, data->labels_offset);
        break;
      case ARG_LABELS_SIZE:
        g_value_set_int(value, data->labels_attr.height);
        break;
      case ARG_LABELS_FONT:
        g_value_set_string(value, data->labels_attr.font);
        break;
      case ARG_LABELS_TEXT:
        g_value_set_string(value, data->labels_attr.text);
        break;
      case ARG_SHOW_GRADIENT:
        g_value_set_boolean(value, data->show_gradient);
        break;
      case ARG_GRADIENT_CUSTOM:
        g_value_set_boolean(value, data->gradient_custom);
        break;
      case ARG_GRADIENT_MASK:
        g_value_set_int(value, data->gradient_mask);
        break;
      case ARG_COLOR_MIN:
        g_value_set_pointer(value, &data->color_min);
        break;
      case ARG_COLOR_MAX:
        g_value_set_pointer(value, &data->color_max);
        break;
      case ARG_COLOR_LT_MIN:
        g_value_set_pointer(value, &data->color_lt_min);
        break;
      case ARG_COLOR_GT_MAX:
        g_value_set_pointer(value, &data->color_gt_max);
        break;
      case ARG_GRADIENT_MAX:
        g_value_set_double(value, data->gradient->ticks.max);
        break;
      case ARG_GRADIENT_MIN:
        g_value_set_double(value, data->gradient->ticks.min);
        break;
      case ARG_GRADIENT_STEP:
        g_value_set_double(value, data->gradient->ticks.step);
        break;
      case ARG_GRADIENT_NMINOR:
        g_value_set_int(value, data->gradient->ticks.nminor);
        break;
      case ARG_GRADIENT_NMAJOR:
        g_value_set_int(value, data->gradient->ticks.nmajorticks);
        break;
      case ARG_GRADIENT_SCALE:
        g_value_set_int(value, data->gradient->ticks.scale);
        break;
      case ARG_GRADIENT_BREAK:
        g_value_set_boolean(value, data->gradient->ticks.apply_break);
        break;
      case ARG_GRADIENT_BREAK_MAX:
        g_value_set_double(value, data->gradient->ticks.break_max);
        break;
      case ARG_GRADIENT_BREAK_MIN:
        g_value_set_double(value, data->gradient->ticks.break_min);
        break;
      case ARG_GRADIENT_BREAK_STEP:
        g_value_set_double(value, data->gradient->ticks.break_step);
        break;
      case ARG_GRADIENT_BREAK_NMINOR:
        g_value_set_int(value, data->gradient->ticks.break_nminor);
        break;
      case ARG_GRADIENT_BREAK_SCALE:
        g_value_set_int(value, data->gradient->ticks.break_scale);
        break;
      case ARG_GRADIENT_BREAK_POSITION:
        g_value_set_double(value, data->gradient->ticks.break_position);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

GtkWidget*
gtk_plot_data_new (void)
{
  GtkWidget *widget;

  widget = gtk_type_new (gtk_plot_data_get_type ());

  return (widget);
}

GtkWidget*
gtk_plot_data_new_function (GtkPlotFunc function)
{
  GtkWidget *dataset;

  dataset = gtk_type_new (gtk_plot_data_get_type ());

  gtk_plot_data_construct_function (GTK_PLOT_DATA(dataset), function);

  return (dataset);
}

void
gtk_plot_data_construct_function (GtkPlotData *data, GtkPlotFunc function)
{
  data->is_function = TRUE;
  data->function = function;
}

GtkWidget*
gtk_plot_data_new_iterator (GtkPlotIterator iterator, gint npoints, guint16 mask)
{
  GtkWidget *dataset;

  dataset = gtk_type_new (gtk_plot_data_get_type ());

  gtk_plot_data_construct_iterator (GTK_PLOT_DATA(dataset), iterator, npoints, mask);

  return (dataset);
}

void
gtk_plot_data_construct_iterator (GtkPlotData *data, 
				  GtkPlotIterator iterator, gint npoints, guint16 mask)
{
  data->is_iterator = TRUE;
  data->iterator_mask = mask;
  data->iterator = iterator;
  data->num_points = npoints; 
}

void
gtk_plot_data_clone (GtkPlotData *data, GtkPlotData *copy)
{
  GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->clone(data, copy);
}

void
gtk_plot_data_update(GtkPlotData *data)
{
  GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->update(data, TRUE);
}

static void
gtk_plot_data_real_update(GtkPlotData *data, gboolean new_range)
{
  data->redraw_pending = TRUE;
}

static void
update_gradient(GtkPlotData *data)
{
  data->redraw_pending = TRUE;
}

/*
static void
gtk_plot_data_draw (GtkWidget *widget, GdkRectangle *area)
{
  if(!GTK_WIDGET_VISIBLE(widget)) return;
  gtk_signal_emit(GTK_OBJECT(widget), data_signals[DRAW_DATA], NULL);
  GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(widget)))->draw_data(GTK_PLOT_DATA(widget));

  GTK_PLOT_DATA(widget)->redraw_pending = FALSE;
}
*/

static void
draw_marker(GtkPlotData *data, GtkPlotMarker *marker)
{
  GtkPlot *plot;
  GdkColor black;
  GdkColormap *colormap = NULL;
  GtkPlotPoint p[6];
  gdouble x, y, z, a, dx, dy, dz, da;
  gchar *label;
  gboolean error;
  gdouble px, py;

  if(!data->plot) return;
  plot = data->plot;

  gtk_plot_data_get_point(data, marker->point, 
			  &x, &y, &z, &a, &dx, &dy, &dz, &da,  
			  &label, &error);

  if(x < plot->xmin || y < plot->ymin || x >= plot->xmax || y >= plot->ymax) 
     return;

  gtk_plot_get_pixel(plot, x, y, &px, &py);

  colormap = gdk_colormap_get_system();
  gdk_color_black(colormap, &black);
  gtk_plot_pc_set_color(plot->pc, &black);
  gtk_plot_pc_set_lineattr(plot->pc, 1, 0, 0, 0);

  p[0].x = px;
  p[0].y = py;
  p[1].x = px - 6;
  p[1].y = py - 12;
  p[2].x = px + 6;
  p[2].y = py - 12;
  gtk_plot_pc_draw_polygon(plot->pc, TRUE, p, 3);
  p[1].x = px - 6;
  p[1].y = py + 12;
  p[2].x = px + 6;
  p[2].y = py + 12;
  gtk_plot_pc_draw_polygon(plot->pc, TRUE, p, 3);
  p[0].x = px - 6;
  p[0].y = py;
  p[1].x = px + 7;
  p[1].y = py;
  gtk_plot_pc_set_lineattr(plot->pc, 3, 0, 0, 0);
  gtk_plot_pc_draw_polygon(plot->pc, FALSE, p, 2);
}

void
gtk_plot_data_paint (GtkPlotData *data)
{
  if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(data))) return;

  gtk_signal_emit(GTK_OBJECT(data), data_signals[DRAW_DATA], NULL);

  data->redraw_pending = FALSE;
}

static void
gtk_plot_data_draw_private (GtkPlotData *data)
{
  GList *list = NULL;

  gtk_plot_data_real_draw(data, data->num_points);

  if(data->show_markers){
    list = data->markers;
    while(list){
      GtkPlotMarker *marker;

      marker = (GtkPlotMarker *)list->data;
      draw_marker(data, marker); 
      list = list->next;
    } 
  }
}

static void
gtk_plot_data_real_draw   (GtkPlotData *dataset,  
			   gint npoints)
{
  GtkWidget *widget;
  GtkPlot *plot = NULL;
  GtkPlotData *function;
  GdkColormap *colormap;
  GdkRectangle area, clip_area;
  gdouble x, y, z = 0., a = 0.;
  gdouble dx = 0., dy = 0., dz = 0., da = 0.;
  gdouble *fx = NULL;
  gdouble *fy = NULL;
  gdouble *fz = NULL;
  gdouble *fa = NULL;
  gdouble *fdx = NULL;
  gdouble *fdy = NULL;
  gdouble *fdz = NULL;
  gdouble *fda = NULL;
  gchar **fl = NULL;
  gboolean error;
  gdouble m;

  g_return_if_fail(GTK_IS_PLOT_DATA(dataset));
  g_return_if_fail(dataset->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(dataset->plot));

  plot = dataset->plot;
  widget = GTK_WIDGET(plot);
  colormap = gdk_colormap_get_system();

  if(!GTK_WIDGET_VISIBLE(widget)) return;
  if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(dataset))) return;

  gtk_plot_pc_gsave(plot->pc);

  m = plot->magnification;

  area.x = widget->allocation.x;
  area.y = widget->allocation.y;
  area.width = widget->allocation.width;
  area.height = widget->allocation.height;

  clip_area.x = area.x + roundint(plot->x * area.width);
  clip_area.y = area.y + roundint(plot->y * area.height);
  clip_area.width = roundint(plot->width * area.width);
  clip_area.height = roundint(plot->height * area.height);

  if(dataset->is_function)
    {
       gdouble xmin, xmax, px, py;

       function = dataset;
       function->num_points = 0;
       gtk_plot_get_pixel(plot, plot->xmin, 0, &xmin, &py);
       gtk_plot_get_pixel(plot, plot->xmax, 0, &xmax, &py);
       for(px = xmin; px <= xmax; px += function->x_step) {
            function->num_points++;
            fx = (gdouble *)g_realloc(fx, function->num_points*sizeof(gdouble));
            fy = (gdouble *)g_realloc(fy, function->num_points*sizeof(gdouble)); 
            gtk_plot_get_point(plot, px, 0, &x, &y);
            y = function->function (plot, dataset, x, &error);

            if(error)
              {
                 gtk_plot_data_set_x(function, fx);
                 gtk_plot_data_set_y(function, fy);
		 function->num_points--;
                 if(function->num_points > 1)
                       gtk_plot_data_connect_points (function, function->num_points);
                 function->num_points = 0;
              }
            else
              {
                fx[function->num_points-1] = x;
                fy[function->num_points-1] = y;
              }
         }
       if(function->num_points > 1 ) 
         {
            gtk_plot_data_set_x(function, fx);
            gtk_plot_data_set_y(function, fy);
            gtk_plot_data_connect_points (function, function->num_points);
         }
       gtk_plot_data_set_x(function, NULL);
       gtk_plot_data_set_y(function, NULL);
       g_free(fx);
       g_free(fy);
    }
  else if(dataset->is_iterator)
    {
       gint iter;
       gchar *label;
   
       function = dataset;
       if(function->iterator_mask & GTK_PLOT_DATA_X)
         fx = g_new0(gdouble, npoints);
       if(function->iterator_mask & GTK_PLOT_DATA_Y)
         fy = g_new0(gdouble, npoints); 
       if(function->iterator_mask & GTK_PLOT_DATA_Z)
         fz = g_new0(gdouble, npoints);
       if(function->iterator_mask & GTK_PLOT_DATA_A)
         fa = g_new0(gdouble, npoints); 
       if(function->iterator_mask & GTK_PLOT_DATA_DX)
         fdx = g_new0(gdouble, npoints);
       if(function->iterator_mask & GTK_PLOT_DATA_DY)
         fdy = g_new0(gdouble, npoints); 
       if(function->iterator_mask & GTK_PLOT_DATA_DZ)
         fdz = g_new0(gdouble, npoints);
       if(function->iterator_mask & GTK_PLOT_DATA_DA)
         fda = g_new0(gdouble, npoints);
       if(function->iterator_mask & GTK_PLOT_DATA_LABELS)
         fl = (gchar **)g_malloc0(npoints * sizeof(gchar *)); 

       for(iter = 0; iter < npoints; iter++){
            function->iterator (plot, dataset, 
                                function->num_points - npoints + iter, 
                                &x, &y, &z, &a, &dx, &dy, &dz, &da, &label, &error);

            if(error)
              {
                 break;
              }
            else
              {
                if(function->iterator_mask & GTK_PLOT_DATA_X) fx[iter] = x;
                if(function->iterator_mask & GTK_PLOT_DATA_Y) fy[iter] = y;
                if(function->iterator_mask & GTK_PLOT_DATA_Z) fz[iter] = z;
                if(function->iterator_mask & GTK_PLOT_DATA_A) fa[iter] = a;
                if(function->iterator_mask & GTK_PLOT_DATA_DX) fdx[iter] = dx;
                if(function->iterator_mask & GTK_PLOT_DATA_DY) fdy[iter] = dy;
                if(function->iterator_mask & GTK_PLOT_DATA_DZ) fdz[iter] = dz;
                if(function->iterator_mask & GTK_PLOT_DATA_DA) fda[iter] = da;
                if(function->iterator_mask & GTK_PLOT_DATA_LABELS) 
                                                   fl[iter] = g_strdup(label);
              }
       }
       gtk_plot_data_set_x(function, fx);
       gtk_plot_data_set_y(function, fy);
       gtk_plot_data_set_z(function, fz);
       gtk_plot_data_set_a(function, fa);
       gtk_plot_data_set_dx(function, fdx);
       gtk_plot_data_set_dy(function, fdy);
       gtk_plot_data_set_dz(function, fdz);
       gtk_plot_data_set_da(function, fda);
       gtk_plot_data_set_labels(function, fl);
       gtk_plot_data_real_real_draw(function, MIN(iter, npoints));

       if(fx) g_free(fx);
       if(fy) g_free(fy);
       if(fz) g_free(fz);
       if(fa) g_free(fa);
       if(fdx) g_free(fdx);
       if(fdy) g_free(fdy);
       if(fdz) g_free(fdz);
       if(fda) g_free(fda);
       gtk_plot_data_dimension_set_points(function, "x", NULL);
       gtk_plot_data_dimension_set_points(function, "y", NULL);
       gtk_plot_data_dimension_set_points(function, "z", NULL);
       gtk_plot_data_dimension_set_points(function, "dx", NULL);
       gtk_plot_data_dimension_set_points(function, "dy", NULL);
       gtk_plot_data_dimension_set_points(function, "dz", NULL);
       gtk_plot_data_dimension_set_points(function, "a", NULL);
       gtk_plot_data_dimension_set_points(function, "da", NULL);
       if(fl) {
         for(iter = 0; iter < npoints; iter++) g_free(fl[iter]);
         g_free(fl);
       }
       gtk_plot_data_set_labels(function, NULL);
    }
  else
       gtk_plot_data_real_real_draw(dataset, npoints);

  gtk_plot_pc_grestore(plot->pc);
}

static void
gtk_plot_data_real_real_draw   (GtkPlotData *dataset,  
			   	gint npoints)
{
  GtkWidget *widget;
  GtkPlot *plot = NULL;
  GdkRectangle area;
  GdkColormap *colormap;
  gdouble *array_x = NULL, *array_y = NULL, *array_z = NULL, *array_a = NULL;
  gdouble *array_dx = NULL, *array_dy = NULL, *array_dz = NULL, *array_da = NULL;
  gchar **array_labels;
  gdouble x = 0., y = 0., z = 0., a = 0.;
  gdouble dx = 0., dy = 0., dz = 0., da = 0.;
  gdouble a_scale;
  gint n;
  gdouble m;

  g_return_if_fail(GTK_IS_PLOT_DATA(dataset));
  g_return_if_fail(dataset->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(dataset->plot));

  plot = dataset->plot;
  widget = GTK_WIDGET(plot);
  colormap = gdk_colormap_get_system();

  if(!GTK_WIDGET_VISIBLE(widget)) return;
  if(!GTK_WIDGET_VISIBLE(GTK_WIDGET(dataset))) return;

  area.x = widget->allocation.x;
  area.y = widget->allocation.y;
  area.width = widget->allocation.width;
  area.height = widget->allocation.height;

  gtk_plot_pc_gsave(plot->pc);

  m = plot->magnification;
  npoints = MIN(npoints, dataset->num_points);

  gtk_plot_data_connect_points (dataset, npoints+1);
  gtk_plot_data_draw_xyz(dataset, npoints);

  array_x = gtk_plot_data_get_x(dataset, &n);
  array_y = gtk_plot_data_get_y(dataset, &n);
  array_z = gtk_plot_data_get_z(dataset, &n);
  array_a = gtk_plot_data_get_a(dataset, &n);
  array_dx = gtk_plot_data_get_dx(dataset, &n);
  array_dy = gtk_plot_data_get_dy(dataset, &n);
  array_dz = gtk_plot_data_get_dz(dataset, &n);
  array_da = gtk_plot_data_get_da(dataset, &n);
  array_labels = gtk_plot_data_get_labels(dataset, &n);
  a_scale = gtk_plot_data_get_a_scale(dataset);

  for(n=dataset->num_points-npoints; n<=dataset->num_points-1; n++)
    {
      if(array_x) x = array_x[n];
      if(array_y) y = array_y[n];
      if(array_z) z = array_z[n];
      if(array_a) a = array_a[n];
      if(array_dx) dx = array_dx[n];
      if(array_dy) dy = array_dy[n];
      if(array_dz) dz = array_dz[n];
      if(array_da) da = array_da[n];

      if(!plot->clip_data || 
        (plot->clip_data && GTK_IS_PLOT_POLAR(plot) && x >= GTK_PLOT_POLAR(plot)->r->ticks.min && x <= GTK_PLOT_POLAR(plot)->r->ticks.max) || 
        (plot->clip_data && x >= plot->xmin && x <= plot->xmax)){
          GdkColor symbol_color, border_color;
          gint symbol_size;

          symbol_color = dataset->symbol.color;
          border_color = dataset->symbol.border.color;
          symbol_size = dataset->symbol.size;
          if(array_da){
             GdkColor level_color;
             gtk_plot_data_get_gradient_level(dataset, da, &level_color);
             dataset->symbol.color = level_color;
             dataset->symbol.border.color = level_color;
          }
          if(array_a && !GTK_IS_PLOT3D(plot)){
             gdouble px, py, px0, py0;     
             gtk_plot_get_pixel(plot, x, y, &px, &py);
             gtk_plot_get_pixel(plot, x + a, y, &px0, &py0);
             dataset->symbol.size = fabs((px - px0)/plot->magnification);
             dataset->symbol.size *= fabs(a_scale);
          }
          GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(dataset)))->draw_symbol(dataset, x, y, z, a, dx, dy, dz, da); 
          dataset->symbol.color = symbol_color;
          dataset->symbol.border.color = border_color;
          dataset->symbol.size = symbol_size;

          if(dataset->show_labels){
            if(array_labels && array_labels[n]){
               GtkPlotText label;
               gdouble px, py, pz;

               if(GTK_IS_PLOT3D(plot))
                   gtk_plot3d_get_pixel(GTK_PLOT3D(plot), x, y, z, 
                                        &px, &py, &pz);
               else
                   gtk_plot_get_pixel(plot, x, y, &px, &py);

               label = dataset->labels_attr;
               label.text = array_labels[n];
               label.x = (gdouble)px / (gdouble)area.width;
               label.y = (gdouble)(py - roundint((dataset->labels_offset + dataset->symbol.size) * m)) / (gdouble)area.height; 
               gtk_plot_draw_text(plot, label);	
            }          
          }
      }
    } 

  gtk_plot_pc_grestore(plot->pc); 
}


static void
gtk_plot_data_draw_legend(GtkPlotData *data, gint x, gint y)
{
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  GdkRectangle area;
  gint lascent = 0, ldescent = 0, lheight = 0, lwidth = 0;
  gdouble m;

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));
  g_return_if_fail(GTK_WIDGET_VISIBLE(data->plot));

  plot = data->plot;
  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;
  area.width = GTK_WIDGET(plot)->allocation.width;
  area.height = GTK_WIDGET(plot)->allocation.height;

  m = plot->magnification; 
  legend = plot->legends_attr;

  if(data->legend)
    legend.text = data->legend;
  else
    legend.text = "";

  gtk_plot_pc_gsave(plot->pc);

  gtk_plot_text_get_size(legend.text, legend.angle, legend.font,
                         roundint(legend.height * m), 
                         &lwidth, &lheight,
                         &lascent, &ldescent);

  if(data->show_legend){
    lheight = MAX(lascent+ldescent, roundint(m * (data->symbol.size + 2 * data->symbol.border.line_width)));

    legend.x = (gdouble)(area.x + x + roundint((plot->legends_line_width + 4) * m)) 
             / (gdouble)area.width;
    legend.y = (gdouble)(area.y + y + lheight - ldescent) / (gdouble)area.height;
    gtk_plot_draw_text(plot, legend);

    if(data->line_connector != GTK_PLOT_CONNECT_NONE ||
       data->symbol.symbol_type == GTK_PLOT_SYMBOL_IMPULSE)
         gtk_plot_draw_line(plot, data->line,
                            x + area.x,
                            y + area.y + lheight / 2,
                            x + area.x + roundint(plot->legends_line_width * m),
                            y + area.y + lheight / 2);

    if(data->symbol.symbol_type != GTK_PLOT_SYMBOL_IMPULSE){
         GtkPlotSymbol aux_symbol;
         gint x1, y1;
  
         x1 = x + area.x + roundint(plot->legends_line_width * m / 2.);
         y1 = y + area.y + lheight / 2;
  
         aux_symbol = data->symbol;
         aux_symbol.color = plot->background;
         aux_symbol.symbol_style = GTK_PLOT_SYMBOL_FILLED;
         aux_symbol.border.line_width = 0;
  
         if(data->symbol.symbol_style == GTK_PLOT_SYMBOL_OPAQUE)
            gtk_plot_data_draw_symbol_private (data, x1, y1, aux_symbol); 
  
         if(data->symbol.symbol_style == GTK_PLOT_SYMBOL_FILLED){
            aux_symbol.color = data->symbol.color; 
            gtk_plot_data_draw_symbol_private (data, x1, y1, aux_symbol); 
         }
  
         aux_symbol = data->symbol;
         aux_symbol.color = data->symbol.border.color; 
         aux_symbol.symbol_style = GTK_PLOT_SYMBOL_EMPTY;
         gtk_plot_data_draw_symbol_private(data, x1, y1, aux_symbol);
    }
    y += 2 * lheight;
  } else 
    y += lheight;

  gtk_plot_pc_grestore(plot->pc);
}

static void
gtk_plot_data_draw_gradient(GtkPlotData *data)
{
  if(!data->show_gradient) return;

  gtk_plot_pc_gsave(data->plot->pc);

  if(data->gradient->orientation == GTK_ORIENTATION_VERTICAL)
    draw_gradient_vertical(data, data->gradient_x, data->gradient_y);
  else
    draw_gradient_horizontal(data, data->gradient_x, data->gradient_y);
  gtk_plot_pc_grestore(data->plot->pc);
}

static void
draw_gradient_vertical(GtkPlotData *data, gdouble px, gdouble py)
{  
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  gchar text[100], new_label[100];
  GdkRectangle area;
  GdkColor color;
  gint lascent, ldescent, lheight, lwidth;
  gint minascent, mindescent, minheight, minwidth;
  gint maxascent, maxdescent, maxheight, maxwidth;
  gint tascent, tdescent, twidth, theight;
  gdouble m;
  gint level;
  gint x = 0, y = 0;
  gint y_orig;
  gint x_orig;
  gdouble ry = 0;
  gint line_height;
  gint nlevels = data->gradient->ticks.nticks;
  gint gradient_width = 0;
  gint gradient_height = 0;
  gint width = 0, height = 0;

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));
  g_return_if_fail(GTK_WIDGET_VISIBLE(data->plot));

  if(!data->show_gradient) return;

  plot = data->plot;
  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;
  area.width = GTK_WIDGET(plot)->allocation.width;
  area.height = GTK_WIDGET(plot)->allocation.height;

  x = x_orig = roundint(plot->internal_allocation.x + plot->internal_allocation.width * px);
  y = y_orig = roundint(plot->internal_allocation.y + plot->internal_allocation.height * py);

  m = plot->magnification;
  legend = data->gradient->labels_attr;

  GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->get_gradient_size(data, &width, &height);

  if(!data->gradient->labels_attr.transparent){
     gtk_plot_pc_set_color(plot->pc, &data->gradient->labels_attr.bg);
     gtk_plot_pc_draw_rectangle(plot->pc,
                                TRUE,
                                x, y,
                                width, height);
  }

  gtk_plot_pc_set_lineattr(plot->pc, data->gradient_border_width, 0, 0, 0);
  gtk_plot_pc_set_color(plot->pc, &data->gradient->labels_attr.fg);
  gtk_plot_pc_set_dash(plot->pc, 0, 0, 0);
                                                                               
  if(data->gradient_border != GTK_PLOT_BORDER_NONE)
     {
       gtk_plot_pc_draw_rectangle(plot->pc,
                                 FALSE,
                                 x, y,
                                 width, height);
     }
                                                                               
  gtk_plot_pc_set_lineattr(plot->pc, 0, 0, 0, 0);
  if(data->gradient_border == GTK_PLOT_BORDER_SHADOW)
     {
       gtk_plot_pc_draw_rectangle(plot->pc,
                                 TRUE,
                                 x + roundint(data->gradient_shadow_width * m),
                                 y + height,
                                 width,
                                 roundint(data->gradient_shadow_width * m));
       gtk_plot_pc_draw_rectangle(plot->pc,
                                 TRUE,
                                 x + width,
                                 y + roundint(data->gradient_shadow_width * m),
                                 roundint(data->gradient_shadow_width * m),
                                 height);
     }


  gtk_plot_axis_parse_label(data->gradient, data->gradient->ticks.min, data->gradient->label_precision, data->gradient->label_style, text);
  if(data->gradient->labels_prefix){
    g_snprintf(new_label, 100, "%s%s", data->gradient->labels_prefix, text);
    g_snprintf(text, 100, "%s", new_label);
  }
  if(data->gradient->labels_suffix){
    g_snprintf(new_label, 100, "%s%s", text, data->gradient->labels_suffix);
    g_snprintf(text, 100, "%s", new_label);
  }

  gtk_plot_text_get_size(text, legend.angle, legend.font,
                         roundint(legend.height * m),
                         &minwidth, &minheight,
                         &minascent, &mindescent);

  gtk_plot_axis_parse_label(data->gradient, data->gradient->ticks.max, data->gradient->label_precision, data->gradient->label_style, text);
  if(data->gradient->labels_prefix){
    g_snprintf(new_label, 100, "%s%s", data->gradient->labels_prefix, text);
    g_snprintf(text, 100, "%s", new_label);
  }
  if(data->gradient->labels_suffix){
    g_snprintf(new_label, 100, "%s%s", text, data->gradient->labels_suffix);
    g_snprintf(text, 100, "%s", new_label);
  }

  gtk_plot_text_get_size(text, legend.angle, legend.font,
                         roundint(legend.height * m),
                         &maxwidth, &maxheight,
                         &maxascent, &maxdescent);

  lwidth = MAX(minwidth, maxwidth);
  lheight = MAX(minheight, maxheight);
  lascent = MAX(minascent, maxascent);
  ldescent = MAX(mindescent, maxdescent);

  if(data->gradient_title_pos == GTK_PLOT_AXIS_LEFT ||
     data->gradient_title_pos == GTK_PLOT_AXIS_RIGHT)
    data->gradient->title.angle = 90;
  else
    data->gradient->title.angle = 0;

  gtk_plot_text_get_size(data->gradient->title.text,
			 data->gradient->title.angle, 
			 data->gradient->title.font,
                         roundint(data->gradient->title.height * m),
                         &twidth, &theight,
                         &tascent, &tdescent);

  line_height = MAX(lheight, roundint(data->gradient_line_height * m));
  gradient_height = (nlevels - 1) * line_height;
  gradient_height += 2*roundint(data->gradient_border_offset * m);

  gradient_width = 2*roundint(data->gradient_border_offset * m);
  x += roundint(data->gradient_border_offset * m);
  y += roundint(data->gradient_border_offset * m) + lheight / 2;
  if(data->gradient->label_mask & GTK_PLOT_LABEL_IN){ 
    x += lwidth + roundint(data->gradient->labels_offset * m);
    gradient_width += lwidth + roundint(data->gradient->labels_offset * m);
  }
  if(data->gradient->label_mask & GTK_PLOT_LABEL_OUT){ 
    gradient_width += lwidth + roundint(data->gradient->labels_offset * m);
  }

  if(data->gradient->title_visible){
    if(data->gradient_title_pos == GTK_PLOT_AXIS_LEFT) {
      x += twidth + roundint(data->gradient->labels_offset * m);
      if(theight > gradient_height)
        y = y + theight / 2 - gradient_height / 2;
    }
    if(data->gradient_title_pos == GTK_PLOT_AXIS_RIGHT) {
      if(theight > gradient_height)
        y = y + theight / 2 - gradient_height / 2;
    }
    if(data->gradient_title_pos == GTK_PLOT_AXIS_TOP){
      y += theight + roundint(data->gradient->labels_offset * m);
      if(twidth > gradient_width)
        x = x + twidth / 2 - gradient_width / 2;
    }
    if(data->gradient_title_pos == GTK_PLOT_AXIS_BOTTOM){
      if(twidth > gradient_width)
        x = x + twidth / 2 - gradient_width / 2;
    }
  }


  if(!data->gradient_custom){
    gint ncolors = line_height * (data->gradient->ticks.nticks - 1);
    gint cy;
    gdouble h;
    gint l;

    cy = y;
    gtk_plot_pc_set_lineattr(plot->pc, 0, 0, 0, 0);
    for(l = ncolors; l >= 0; l -= 1){
      h = gtk_plot_axis_ticks_inverse(data->gradient, (gdouble)l/(gdouble)ncolors);
      gtk_plot_data_get_gradient_level(data, h, &color);
      gtk_plot_pc_set_color(plot->pc, &color);

      gtk_plot_pc_draw_line(plot->pc,
                            x, cy,
                            x + roundint(data->gradient_line_width * m), cy);

      cy++;
    }

    gtk_plot_pc_set_color(plot->pc, &plot->legends_attr.fg);
    gtk_plot_pc_set_lineattr(plot->pc, plot->legends_border_width, 0, 0, 0);
    gtk_plot_pc_draw_rectangle(plot->pc, FALSE,
                               x, y,
                               roundint(data->gradient_line_width * m),
                               ncolors);

  } else {


    legend.x = (gdouble)(area.x + x) / (gdouble)area.width;

    ry = y;
    for(level = nlevels-1; level >= 0; level--){
      gdouble val, h;
      gboolean sublevel = FALSE;

      val = data->gradient->ticks.values[level].value;
      sublevel = data->gradient->ticks.values[level].minor;

      if(level != 0){
        h = val;
        color = data->gradient_colors[level-1];

        gtk_plot_pc_set_color(plot->pc, &color);

        gtk_plot_pc_draw_rectangle(plot->pc, TRUE,
                                   x, ry,
                                   roundint(data->gradient_line_width * m),
                                   line_height/(gdouble)(data->gradient->ticks.nminor+1)+1);
      }
      ry += line_height/(gdouble)(data->gradient->ticks.nminor+1);
    }
    gtk_plot_pc_set_color(plot->pc, &plot->legends_attr.fg);
    gtk_plot_pc_set_lineattr(plot->pc, plot->legends_border_width, 0, 0, 0);
    gtk_plot_pc_draw_rectangle(plot->pc, FALSE,
                               x, y,
                               roundint(data->gradient_line_width * m),
                               (data->gradient->ticks.nmajorticks)*line_height);

  }

  ry = y;
  for(level = nlevels-1; level >= 0; level--){
    gdouble val;
    gboolean sublevel = FALSE;

    val = data->gradient->ticks.values[level].value;
    if(data->gradient->ticks.values[level].minor) sublevel = TRUE;

    if(!sublevel){
      legend.y = (gdouble)(area.y + ry + lascent - (lascent + ldescent)/2.) / (gdouble)area.height;

      gtk_plot_axis_parse_label(data->gradient, val, data->gradient->label_precision, data->gradient->label_style, text);

      if(data->gradient->labels_prefix){
        g_snprintf(new_label, 100, "%s%s", 
                   data->gradient->labels_prefix, text);
        g_snprintf(text, 100, "%s", new_label);
      }
      if(data->gradient->labels_suffix){
        g_snprintf(new_label, 100, "%s%s", 
                   text, data->gradient->labels_suffix);
        g_snprintf(text, 100, "%s", new_label);
      }
      legend.text = text;

      if(data->gradient->label_mask & GTK_PLOT_LABEL_IN){
        legend.x = (gdouble)(area.x + x - roundint(data->gradient->labels_offset * m)) / (gdouble)area.width;
        legend.justification = GTK_JUSTIFY_RIGHT;
        gtk_plot_draw_text(plot, legend);
      }
      if(data->gradient->label_mask & GTK_PLOT_LABEL_OUT){
        legend.x = (gdouble)(area.x + x + roundint((data->gradient_line_width + data->gradient->labels_offset) * m)) / (gdouble)area.width;
        legend.justification = GTK_JUSTIFY_LEFT;
        gtk_plot_draw_text(plot, legend);
      }

      gtk_plot_pc_draw_line(plot->pc,
                            x, ry,
                            x + roundint(data->gradient->ticks_length * m), ry);
      gtk_plot_pc_draw_line(plot->pc,
                            x + roundint((data->gradient_line_width - data->gradient->ticks_length)* m),
                            ry,
                            x + roundint(data->gradient_line_width * m),
                            ry);
    }

    ry += line_height/(gdouble)(data->gradient->ticks.nminor+1);

  }
  if(data->gradient->title_visible){
    switch(data->gradient_title_pos){
      case GTK_PLOT_AXIS_LEFT:
        legend = data->gradient->title;
        legend.angle = 90;
        legend.x = (gdouble)(area.x + x_orig + tascent + tdescent + roundint(data->gradient_border_offset * m)) / (gdouble)area.width;
        legend.y = (gdouble)(area.y + y_orig + height / 2.)/(gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
        break;
      case GTK_PLOT_AXIS_RIGHT:
        legend = data->gradient->title;
	  legend.angle = 270;
        legend.x = (gdouble)(area.x + x + lwidth + roundint((data->gradient_line_width + 2*data->gradient_border_offset) * m)) / (gdouble)area.width;
        legend.y = (gdouble)(area.y + y_orig + height / 2.)/(gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
        break;
      case GTK_PLOT_AXIS_TOP:
        legend = data->gradient->title;
        legend.angle = 0;
        legend.x = (gdouble)(area.x + x_orig + width / 2) / (gdouble)area.width;
        legend.y = (gdouble)(area.y + y_orig + theight + roundint((data->gradient_border_offset) * m))/(gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
        break;
      case GTK_PLOT_AXIS_BOTTOM:
        legend = data->gradient->title;
        legend.angle = 0;
        legend.x = (gdouble)(area.x + x_orig + width / 2) / (gdouble)area.width;
        legend.y = (gdouble)(area.y + y_orig + height - roundint(data->gradient_border_offset * m))/(gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
        break;
    }
  }
}

static void
draw_gradient_horizontal(GtkPlotData *data, gdouble px, gdouble py)
{  
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  gchar text[100], new_label[100];
  GdkRectangle area;
  GdkColor color;
  gint lascent, ldescent, lheight, lwidth;
  gint minascent, mindescent, minheight, minwidth;
  gint maxascent, maxdescent, maxheight, maxwidth;
  gint tascent, tdescent, twidth, theight;
  gdouble m;
  gint level;
  gint x = 0, y = 0;
  gint width = 0, height = 0;
  gint y_orig;
  gint x_orig;
  gdouble rx = 0;
  gint line_width;
  gint nlevels = data->gradient->ticks.nticks;
  gint gradient_width = 0;
  gint gradient_height = 0;

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));
  g_return_if_fail(GTK_WIDGET_VISIBLE(data->plot));

  if(!data->show_gradient) return;

  plot = data->plot;
  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;
  area.width = GTK_WIDGET(plot)->allocation.width;
  area.height = GTK_WIDGET(plot)->allocation.height;

  x = x_orig = roundint(plot->internal_allocation.x + plot->internal_allocation.width * px);
  y = y_orig = roundint(plot->internal_allocation.y + plot->internal_allocation.height * py);

  m = plot->magnification;
  legend = data->gradient->labels_attr;

  GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->get_gradient_size(data, &width, &height);

  if(!data->gradient->labels_attr.transparent){
     gtk_plot_pc_set_color(plot->pc, &data->gradient->labels_attr.bg);
     gtk_plot_pc_draw_rectangle(plot->pc,
                                TRUE,
                                x, y,
                                width, height);
  }

  gtk_plot_pc_set_lineattr(plot->pc, data->gradient_border_width, 0, 0, 0);
  gtk_plot_pc_set_color(plot->pc, &data->gradient->labels_attr.fg);
  gtk_plot_pc_set_dash(plot->pc, 0, 0, 0);
                                                                               
  if(data->gradient_border != GTK_PLOT_BORDER_NONE)
     {
       gtk_plot_pc_draw_rectangle(plot->pc,
                                 FALSE,
                                 x, y,
                                 width, height);
     }
                                                                               
  gtk_plot_pc_set_lineattr(plot->pc, 0, 0, 0, 0);
  if(data->gradient_border == GTK_PLOT_BORDER_SHADOW)
     {
       gtk_plot_pc_draw_rectangle(plot->pc,
                                 TRUE,
                                 x + roundint(data->gradient_shadow_width * m),
                                 y + height,
                                 width,
                                 roundint(data->gradient_shadow_width * m));
       gtk_plot_pc_draw_rectangle(plot->pc,
                                 TRUE,
                                 x + width,
                                 y + roundint(data->gradient_shadow_width * m),
                                 roundint(data->gradient_shadow_width * m),
                                 height);
     }


  gtk_plot_axis_parse_label(data->gradient, data->gradient->ticks.min, data->gradient->label_precision, data->gradient->label_style, text);
  if(data->gradient->labels_prefix){
    g_snprintf(new_label, 100, "%s%s", data->gradient->labels_prefix, text);
    g_snprintf(text, 100, "%s", new_label);
  }
  if(data->gradient->labels_suffix){
    g_snprintf(new_label, 100, "%s%s", text, data->gradient->labels_suffix);
    g_snprintf(text, 100, "%s", new_label);
  }

  gtk_plot_text_get_size(text, legend.angle, legend.font,
                         roundint(legend.height * m),
                         &minwidth, &minheight,
                         &minascent, &mindescent);

  gtk_plot_axis_parse_label(data->gradient, data->gradient->ticks.max, data->gradient->label_precision, data->gradient->label_style, text);
  if(data->gradient->labels_prefix){
    g_snprintf(new_label, 100, "%s%s", data->gradient->labels_prefix, text);
    g_snprintf(text, 100, "%s", new_label);
  }
  if(data->gradient->labels_suffix){
    g_snprintf(new_label, 100, "%s%s", text, data->gradient->labels_suffix);
    g_snprintf(text, 100, "%s", new_label);
  }

  gtk_plot_text_get_size(text, legend.angle, legend.font,
                         roundint(legend.height * m),
                         &maxwidth, &maxheight,
                         &maxascent, &maxdescent);

  lwidth = MAX(minwidth, maxwidth);
  lheight = MAX(minheight, maxheight);
  lascent = MAX(minascent, maxascent);
  ldescent = MAX(mindescent, maxdescent);

  if(data->gradient_title_pos == GTK_PLOT_AXIS_LEFT ||
     data->gradient_title_pos == GTK_PLOT_AXIS_RIGHT)
    data->gradient->title.angle = 90;
  else
    data->gradient->title.angle = 0;

  gtk_plot_text_get_size(data->gradient->title.text,
			 data->gradient->title.angle, 
			 data->gradient->title.font,
                         roundint(data->gradient->title.height * m),
                         &twidth, &theight,
                         &tascent, &tdescent);

  line_width = MAX(lwidth + roundint(data->gradient->labels_offset * m), roundint(data->gradient_line_width * m));

  gradient_height = (nlevels - 1) * line_width;
  gradient_height += 2*roundint(data->gradient_border_offset * m);

  y += roundint(data->gradient_border_offset * m);
  x += roundint(data->gradient_border_offset * m);
  x += lwidth / 2.;
  gradient_width = 2*roundint(data->gradient_border_offset * m);

  if(data->gradient->label_mask & GTK_PLOT_LABEL_IN){ 
    y += lascent + ldescent + roundint(data->gradient->labels_offset * m);
    gradient_width += lascent + ldescent + roundint(data->gradient->labels_offset * m);
  }
  if(data->gradient->label_mask & GTK_PLOT_LABEL_OUT){ 
    gradient_width += lascent + ldescent + roundint(data->gradient->labels_offset * m);
  }

  if(data->gradient->title_visible){
    gradient_width += roundint(data->gradient_line_width * m);
    if(data->gradient_title_pos == GTK_PLOT_AXIS_LEFT) {
      x += twidth + roundint(data->gradient->labels_offset * m);
      if(theight > gradient_width)
        y = y + theight / 2 - gradient_width / 2;
    }
    if(data->gradient_title_pos == GTK_PLOT_AXIS_RIGHT) {
      if(theight > gradient_width)
        y = y + theight / 2 - gradient_width / 2;
    }
    if(data->gradient_title_pos == GTK_PLOT_AXIS_TOP){
      y += (tascent + tdescent) + roundint(data->gradient->labels_offset * m);
      if(twidth > gradient_height)
        x = x + twidth / 2 - gradient_height / 2;
    }
    if(data->gradient_title_pos == GTK_PLOT_AXIS_BOTTOM){
      if(twidth > gradient_height)
        x = x + twidth / 2 - gradient_height / 2;
    }
  }


  if(!data->gradient_custom){
    gint ncolors = line_width * (data->gradient->ticks.nticks - 1);
    gint cx;
    gdouble h;
    gint l;

    cx = x;
    gtk_plot_pc_set_lineattr(plot->pc, 0, 0, 0, 0);
    for(l = ncolors; l >= 0; l -= 1){
      h = gtk_plot_axis_ticks_inverse(data->gradient, (gdouble)l/(gdouble)ncolors);
      gtk_plot_data_get_gradient_level(data, h, &color);
      gtk_plot_pc_set_color(plot->pc, &color);

      gtk_plot_pc_draw_line(plot->pc,
                            cx, y,
                            cx, y + roundint(data->gradient_line_height * m));

      cx++;
    }

    gtk_plot_pc_set_color(plot->pc, &plot->legends_attr.fg);
    gtk_plot_pc_set_lineattr(plot->pc, plot->legends_border_width, 0, 0, 0);
    gtk_plot_pc_draw_rectangle(plot->pc, FALSE,
                               x, y,
                               ncolors,
                               roundint(data->gradient_line_height * m));

  } else {


    legend.x = (gdouble)(area.x + x) / (gdouble)area.width;

    rx = x;
    for(level = nlevels-1; level >= 0; level--){
      gdouble val, h;
      gboolean sublevel = FALSE;

      val = data->gradient->ticks.values[level].value;
      sublevel = data->gradient->ticks.values[level].minor;

      if(level != 0){
        h = val;
        color = data->gradient_colors[level-1];

        gtk_plot_pc_set_color(plot->pc, &color);

        gtk_plot_pc_draw_rectangle(plot->pc, TRUE,
                                   rx, y,
                                   line_width/(gdouble)(data->gradient->ticks.nminor+1)+1,
                                   roundint(data->gradient_line_height * m));
      }
      rx += line_width/(gdouble)(data->gradient->ticks.nminor+1);
    }
    gtk_plot_pc_set_color(plot->pc, &plot->legends_attr.fg);
    gtk_plot_pc_set_lineattr(plot->pc, plot->legends_border_width, 0, 0, 0);
    gtk_plot_pc_draw_rectangle(plot->pc, FALSE,
                               x, y,
                               (data->gradient->ticks.nmajorticks)*line_width,
                               roundint(data->gradient_line_height * m));

  }

  rx = x;
  for(level = 0; level < nlevels; level++){
    gdouble val;
    gboolean sublevel = FALSE;

    val = data->gradient->ticks.values[level].value;
    if(data->gradient->ticks.values[level].minor) sublevel = TRUE;

    if(!sublevel){
      legend.x = (gdouble)(area.x + rx) / (gdouble)area.width;

      gtk_plot_axis_parse_label(data->gradient, val, data->gradient->label_precision, data->gradient->label_style, text);

      if(data->gradient->labels_prefix){
        g_snprintf(new_label, 100, "%s%s", 
                   data->gradient->labels_prefix, text);
        g_snprintf(text, 100, "%s", new_label);
      }
      if(data->gradient->labels_suffix){
        g_snprintf(new_label, 100, "%s%s", 
                   text, data->gradient->labels_suffix);
        g_snprintf(text, 100, "%s", new_label);
      }
      legend.text = text;

      if(data->gradient->label_mask & GTK_PLOT_LABEL_IN){
        legend.y = (gdouble)(area.y + y - roundint(data->gradient->labels_offset * m)) / (gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
      }
      if(data->gradient->label_mask & GTK_PLOT_LABEL_OUT){
        legend.y = (gdouble)(area.y + y + lheight + roundint((data->gradient_line_height + data->gradient->labels_offset) * m)) / (gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
      }

      gtk_plot_pc_draw_line(plot->pc,
                            rx, y,
                            rx, y + roundint(data->gradient->ticks_length * m));
      gtk_plot_pc_draw_line(plot->pc,
                            rx, y + roundint((data->gradient_line_height - data->gradient->ticks_length)* m),
                            rx,
                            y + roundint(data->gradient_line_height * m));
    }

    rx += line_width/(gdouble)(data->gradient->ticks.nminor+1);

  }
  if(data->gradient->title_visible){
    switch(data->gradient_title_pos){
      case GTK_PLOT_AXIS_LEFT:
        legend = data->gradient->title;
        legend.angle = 90;
        legend.x = (gdouble)(area.x + x_orig + tascent + tdescent + roundint(data->gradient_border_offset * m)) / (gdouble)area.width;
        legend.y = (gdouble)(area.y + y_orig + height / 2.)/(gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
        break;
      case GTK_PLOT_AXIS_RIGHT:
        legend = data->gradient->title;
	  legend.angle = 270;
        legend.x = (gdouble)(area.x + x_orig + width - twidth - roundint(data->gradient_border_offset * m)) / (gdouble)area.width;
        legend.y = (gdouble)(area.y + y_orig + height / 2.)/(gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
        break;
      case GTK_PLOT_AXIS_TOP:
        legend = data->gradient->title;
        legend.angle = 0;
        legend.x = (gdouble)(area.x + x_orig + width / 2.) / (gdouble)area.width;
        legend.y = (gdouble)(area.y + y_orig + roundint(data->gradient_border_offset * m) + theight)/(gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
        break;
      case GTK_PLOT_AXIS_BOTTOM:
        legend = data->gradient->title;
        legend.angle = 0;
        legend.x = (gdouble)(area.x + x_orig + width / 2.) / (gdouble)area.width;
        legend.y = (gdouble)(area.y + y_orig + height - roundint(data->gradient_border_offset * m))/(gdouble)area.height;
        legend.justification = GTK_JUSTIFY_CENTER;
        gtk_plot_draw_text(plot, legend);
        break;
    }
  }
}

static void
gtk_plot_data_get_legend_size(GtkPlotData *data, gint *width, gint *height)
{
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  gint lascent = 0, ldescent = 0, lheight = 0, lwidth = 0;
  gdouble m;

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));

  plot = data->plot;
  m = plot->magnification; 

  legend = plot->legends_attr;

  if(data->legend)
    legend.text = data->legend;
  else
    legend.text = "";

  *height = 0;
  *width = roundint(12 * m);

  if(data->show_legend){
    gtk_plot_text_get_size(legend.text, legend.angle, legend.font,
                           roundint(legend.height * m), 
                           &lwidth, &lheight,
                           &lascent, &ldescent);
    *width = lwidth + roundint((plot->legends_line_width + 12) * m);
    *height = MAX(lascent + ldescent, roundint(data->symbol.size * m + 2 * data->symbol.border.line_width));
  }
}

GtkPlotAxis *
gtk_plot_data_gradient (GtkPlotData *data)
{
  return data->gradient;
}

void
gtk_plot_data_move_gradient(GtkPlotData *data, gdouble x, gdouble y)
{
  data->gradient_x = x;
  data->gradient_y = y;
  gtk_signal_emit(GTK_OBJECT(data), data_signals[GRADIENT_CHANGED]);
}

GtkAllocation
gtk_plot_data_get_gradient_allocation(GtkPlotData *data)
{
  GdkRectangle area;
  GtkAllocation allocation;
  GtkPlot *plot = NULL;
  gint width, height;

  plot = data->plot;
  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;

  allocation.x = roundint(area.x + plot->internal_allocation.x + plot->internal_allocation.width * data->gradient_x);
  allocation.y = roundint(area.y + plot->internal_allocation.y + plot->internal_allocation.height * data->gradient_y);
  GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(data)))->get_gradient_size(data, &width, &height);
  allocation.width = width;
  allocation.height = height;

  return allocation;
}

void
gtk_plot_data_set_gradient_size(GtkPlotData *data, gint size)
{
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  gint lascent = 0, ldescent = 0, lheight = 0, lwidth = 0;
  gint tascent = 0, tdescent = 0, theight = 0, twidth = 0;
  gint maxascent = 0, maxdescent = 0, maxheight = 0, maxwidth = 0;
  gint minascent = 0, mindescent = 0, minheight = 0, minwidth = 0;
  gdouble m;
  gchar text[100], new_label[100];
  gint gradient_width = 0;
  gint gradient_height = 0;
  gint nlevels = data->gradient->ticks.nticks;

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));

  plot = data->plot;
  m = plot->magnification; 

  legend = data->gradient->labels_attr;

  if(!data->show_gradient) return;

  gtk_plot_axis_parse_label(data->gradient, data->gradient->ticks.min, data->gradient->label_precision, data->gradient->label_style, text);

  if(data->gradient->labels_prefix){
    g_snprintf(new_label, 100, "%s%s", data->gradient->labels_prefix, text);
    g_snprintf(text, 100, "%s", new_label);
  }
  if(data->gradient->labels_suffix){
    g_snprintf(new_label, 100, "%s%s", text, data->gradient->labels_suffix);
    g_snprintf(text, 100, "%s", new_label);
  }

  gtk_plot_text_get_size(text, legend.angle, legend.font,
                         roundint(legend.height * m),
                         &minwidth, &minheight,
                         &minascent, &mindescent);

  gtk_plot_axis_parse_label(data->gradient, data->gradient->ticks.max, data->gradient->label_precision, data->gradient->label_style, text);
  if(data->gradient->labels_prefix){
    g_snprintf(new_label, 100, "%s%s", data->gradient->labels_prefix, text);
    g_snprintf(text, 100, "%s", new_label);
  }
  if(data->gradient->labels_suffix){
    g_snprintf(new_label, 100, "%s%s", text, data->gradient->labels_suffix);
    g_snprintf(text, 100, "%s", new_label);
  }

  gtk_plot_text_get_size(text, legend.angle, legend.font,
                         roundint(legend.height * m),
                         &maxwidth, &maxheight,
                         &maxascent, &maxdescent);

  lwidth = MAX(minwidth, maxwidth);
  lheight = MAX(minheight, maxheight);
  lascent = MAX(minascent, maxascent);
  ldescent = MAX(mindescent, maxdescent);

  if(data->gradient_title_pos == GTK_PLOT_AXIS_LEFT ||
     data->gradient_title_pos == GTK_PLOT_AXIS_RIGHT)
    data->gradient->title.angle = 90;
  else
    data->gradient->title.angle = 0;

  gtk_plot_text_get_size(data->gradient->title.text,
			 data->gradient->title.angle, 
			 data->gradient->title.font,
                         roundint(data->gradient->title.height * m),
                         &twidth, &theight,
                         &tascent, &tdescent);

  if(data->gradient->orientation == GTK_ORIENTATION_VERTICAL){
    gradient_height = 2*roundint(data->gradient_border_offset * m);
    gradient_height += lheight;

    gradient_width = 2*roundint(data->gradient_border_offset * m);
    gradient_width += roundint(data->gradient_line_width * m);

    if(data->gradient->label_mask & GTK_PLOT_LABEL_IN){ 
      gradient_width += lwidth + roundint(data->gradient->labels_offset * m);
    }
    if(data->gradient->label_mask & GTK_PLOT_LABEL_OUT){ 
      gradient_width += lwidth + roundint(data->gradient->labels_offset * m);
    }
  } else {
    gradient_width = 2*roundint(data->gradient_border_offset * m);
    gradient_width += lwidth + roundint(data->gradient_border_offset * m);

    gradient_height = 2*roundint(data->gradient_border_offset * m);
    gradient_height += roundint(data->gradient_line_height * m);

    if(data->gradient->label_mask & GTK_PLOT_LABEL_IN){ 
      gradient_height += lheight + roundint(data->gradient->labels_offset * m);
    }
    if(data->gradient->label_mask & GTK_PLOT_LABEL_OUT){ 
      gradient_height += lheight + roundint(data->gradient->labels_offset * m);
    }
  }

  if(data->gradient->title_visible){
    switch(data->gradient_title_pos){
      case GTK_PLOT_AXIS_LEFT:
      case GTK_PLOT_AXIS_RIGHT:
        gradient_width += twidth + roundint(data->gradient->labels_offset * m);
        break; 
      case GTK_PLOT_AXIS_TOP:
      case GTK_PLOT_AXIS_BOTTOM:
        gradient_height += theight + roundint(data->gradient->labels_offset * m);
        break; 
    }
  }

  if(data->gradient->orientation == GTK_ORIENTATION_VERTICAL){
    data->gradient_line_height = roundint((gdouble) (size - gradient_height) / (gdouble) (nlevels - 1) / m);
  } else {
    data->gradient_line_width = roundint((gdouble) (size - gradient_width) / (gdouble) (nlevels - 1) / m);
  }
}

static void
gtk_plot_data_get_gradient_size(GtkPlotData *data, gint *width, gint *height)
{
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  gint lascent = 0, ldescent = 0, lheight = 0, lwidth = 0;
  gint tascent = 0, tdescent = 0, theight = 0, twidth = 0;
  gint maxascent = 0, maxdescent = 0, maxheight = 0, maxwidth = 0;
  gint minascent = 0, mindescent = 0, minheight = 0, minwidth = 0;
  gdouble m;
  gchar text[100], new_label[100];
  gint gradient_width = 0;
  gint gradient_height = 0;
  gint line_height = 0;
  gint nlevels = data->gradient->ticks.nticks;

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));

  plot = data->plot;
  m = plot->magnification; 

  *width = 0;
  *height = 0;

  legend = data->gradient->labels_attr;

  if(!data->show_gradient) return;

  gtk_plot_axis_parse_label(data->gradient, data->gradient->ticks.min, data->gradient->label_precision, data->gradient->label_style, text);

  if(data->gradient->labels_prefix){
    g_snprintf(new_label, 100, "%s%s", data->gradient->labels_prefix, text);
    g_snprintf(text, 100, "%s", new_label);
  }
  if(data->gradient->labels_suffix){
    g_snprintf(new_label, 100, "%s%s", text, data->gradient->labels_suffix);
    g_snprintf(text, 100, "%s", new_label);
  }

  gtk_plot_text_get_size(text, legend.angle, legend.font,
                         roundint(legend.height * m),
                         &minwidth, &minheight,
                         &minascent, &mindescent);

  gtk_plot_axis_parse_label(data->gradient, data->gradient->ticks.max, data->gradient->label_precision, data->gradient->label_style, text);
  if(data->gradient->labels_prefix){
    g_snprintf(new_label, 100, "%s%s", data->gradient->labels_prefix, text);
    g_snprintf(text, 100, "%s", new_label);
  }
  if(data->gradient->labels_suffix){
    g_snprintf(new_label, 100, "%s%s", text, data->gradient->labels_suffix);
    g_snprintf(text, 100, "%s", new_label);
  }

  gtk_plot_text_get_size(text, legend.angle, legend.font,
                         roundint(legend.height * m),
                         &maxwidth, &maxheight,
                         &maxascent, &maxdescent);

  lwidth = MAX(minwidth, maxwidth);
  lheight = MAX(minheight, maxheight);
  lascent = MAX(minascent, maxascent);
  ldescent = MAX(mindescent, maxdescent);

  if(data->gradient_title_pos == GTK_PLOT_AXIS_LEFT ||
     data->gradient_title_pos == GTK_PLOT_AXIS_RIGHT)
    data->gradient->title.angle = 90;
  else
    data->gradient->title.angle = 0;

  gtk_plot_text_get_size(data->gradient->title.text,
			 data->gradient->title.angle, 
			 data->gradient->title.font,
                         roundint(data->gradient->title.height * m),
                         &twidth, &theight,
                         &tascent, &tdescent);

  if(data->gradient->orientation == GTK_ORIENTATION_VERTICAL){
    line_height = MAX(lheight, roundint(data->gradient_line_height * m));
    gradient_height = (nlevels - 1) * line_height;
    gradient_height += 2*roundint(data->gradient_border_offset * m);
    gradient_height += lheight;

    gradient_width = 2*roundint(data->gradient_border_offset * m);
    gradient_width += roundint(data->gradient_line_width * m);

    if(data->gradient->label_mask & GTK_PLOT_LABEL_IN){ 
      gradient_width += lwidth + roundint(data->gradient->labels_offset * m);
    }
    if(data->gradient->label_mask & GTK_PLOT_LABEL_OUT){ 
      gradient_width += lwidth + roundint(data->gradient->labels_offset * m);
    }
  } else {
    line_height = MAX(lwidth + roundint(data->gradient->labels_offset * m), roundint(data->gradient_line_width * m));
    gradient_width = (nlevels - 1) * line_height;
    gradient_width += 2*roundint(data->gradient_border_offset * m);
    gradient_width += lwidth + roundint(data->gradient_border_offset * m);

    gradient_height = 2*roundint(data->gradient_border_offset * m);
    gradient_height += roundint(data->gradient_line_height * m);

    if(data->gradient->label_mask & GTK_PLOT_LABEL_IN){ 
      gradient_height += lheight + roundint(data->gradient->labels_offset * m);
    }
    if(data->gradient->label_mask & GTK_PLOT_LABEL_OUT){ 
      gradient_height += lheight + roundint(data->gradient->labels_offset * m);
    }
  }

  if(data->gradient->title_visible){
    switch(data->gradient_title_pos){
      case GTK_PLOT_AXIS_LEFT:
      case GTK_PLOT_AXIS_RIGHT:
        gradient_height = MAX(gradient_height, theight + roundint(data->gradient->labels_offset * 2 * m) + lheight);
        gradient_width += twidth + roundint(data->gradient->labels_offset * m);
        break; 
      case GTK_PLOT_AXIS_TOP:
      case GTK_PLOT_AXIS_BOTTOM:
        gradient_width = MAX(gradient_width, twidth + roundint(data->gradient->labels_offset * 2 * m));
        gradient_height += theight + roundint(data->gradient->labels_offset * m);
        break; 
    }
  }

  *width = gradient_width;
  *height = gradient_height;

}

static void
gtk_plot_data_real_draw_symbol (GtkPlotData *data, 
                                gdouble x, gdouble y, gdouble z, gdouble a,
                                gdouble dx, gdouble dy, gdouble dz, gdouble da)
{
  GtkPlot *plot;
  gdouble px = 0, py = 0, pz = 0;
  
  plot = data->plot;

  if(GTK_IS_PLOT_POLAR(plot)){
    GtkPlotPolar *polar = GTK_PLOT_POLAR(plot);
    if(plot->clip_data &&
       (x < polar->r->ticks.min || x > polar->r->ticks.max || y < polar->angle->ticks.min || y > polar->angle->ticks.max)) 
             return; 
  } else if(GTK_IS_PLOT3D(plot)){
    if(plot->clip_data && (x < plot->xmin || x > plot->xmax)) return;
    if(plot->clip_data && data->symbol.symbol_type != GTK_PLOT_SYMBOL_IMPULSE &&
       (y < plot->ymin || y > plot->ymax)) return; 
  } else {
    if(plot->clip_data &&
       (x < plot->xmin || x > plot->xmax || y < plot->ymin || y > plot->ymax)) 
             return; 
  }

  if(GTK_IS_PLOT3D(plot))
       gtk_plot3d_get_pixel(GTK_PLOT3D(plot), x, y, z, 
                            &px, &py, &pz);
  else
       gtk_plot_get_pixel(plot, x, y, &px, &py);

  gtk_plot_data_draw_errbars(data, x, y, z, dx, dy, dz); 

  gtk_plot_data_draw_symbol (data, px, py); 
}


void
gtk_plot_data_draw_symbol (GtkPlotData *data, gdouble px, gdouble py) 
{
  GtkPlot *plot;
  GtkPlotSymbol aux_symbol;

  plot = data->plot;

  aux_symbol = data->symbol;
  aux_symbol.color = plot->background;
  aux_symbol.symbol_style = GTK_PLOT_SYMBOL_FILLED;
  aux_symbol.border.line_width = 0;

  if(data->symbol.symbol_style == GTK_PLOT_SYMBOL_OPAQUE)
     gtk_plot_data_draw_symbol_private (data, px, py, aux_symbol); 

  if(data->symbol.symbol_style == GTK_PLOT_SYMBOL_FILLED){
     aux_symbol.color = data->symbol.color; 
     gtk_plot_data_draw_symbol_private (data, px, py, aux_symbol); 
  }

  aux_symbol = data->symbol;
  aux_symbol.color = data->symbol.border.color; 
  aux_symbol.symbol_style = GTK_PLOT_SYMBOL_EMPTY;
  gtk_plot_data_draw_symbol_private (data, px, py, aux_symbol); 
}

static void
gtk_plot_data_draw_symbol_private (GtkPlotData *data, 
                                   gdouble x, gdouble y, 
                                   GtkPlotSymbol symbol)
{
  GtkWidget *widget;
  GtkPlot *plot;
  gdouble x0, y0;
  gdouble px0, py0; 
  GdkRectangle clip_area;
  gboolean filled;
  gint size;
  gdouble m;

  if(symbol.symbol_type == GTK_PLOT_SYMBOL_NONE) return;

  plot = data->plot;
  widget = GTK_WIDGET(plot);

  m = plot->magnification;

  clip_area.x = plot->internal_allocation.x;
  clip_area.y = plot->internal_allocation.y;
  clip_area.width = plot->internal_allocation.width;
  clip_area.height = plot->internal_allocation.height;

/*
  gdk_gc_set_clip_rectangle(gc, &clip_area);
*/

  gtk_plot_pc_set_color(plot->pc, &symbol.color);
  gtk_plot_pc_set_lineattr (plot->pc, symbol.border.line_width, 0, 0, 0);
  gtk_plot_pc_set_dash (plot->pc, 0, 0, 0);

  filled = (symbol.symbol_style == GTK_PLOT_SYMBOL_FILLED) ? TRUE : FALSE;
  size = symbol.size;

  switch(symbol.symbol_type) {
     case GTK_PLOT_SYMBOL_NONE:
            break;
     case GTK_PLOT_SYMBOL_DOT:
            gtk_plot_pc_draw_point(plot->pc, x, y);
            break;
     case GTK_PLOT_SYMBOL_SQUARE:
            gtk_plot_pc_draw_rectangle (plot->pc,
                                        filled,
                                        x-m*size/2.0, y-m*size/2.0,
                                        m*size, m*size);
            break;
     case GTK_PLOT_SYMBOL_CIRCLE:
            gtk_plot_pc_draw_circle (plot->pc, filled, x, y, m*size); 
            break;
     case GTK_PLOT_SYMBOL_UP_TRIANGLE:
            gtk_plot_data_draw_up_triangle (data, x, y, m*size, filled);                  
            break;
     case GTK_PLOT_SYMBOL_DOWN_TRIANGLE:
            gtk_plot_data_draw_down_triangle (data, x, y, m*size, filled);                  
            break;
     case GTK_PLOT_SYMBOL_RIGHT_TRIANGLE:
            gtk_plot_data_draw_right_triangle (data, x, y, m*size, filled);                  
            break;
     case GTK_PLOT_SYMBOL_LEFT_TRIANGLE:
            gtk_plot_data_draw_left_triangle (data, x, y, m*size, filled);                  
            break;
     case GTK_PLOT_SYMBOL_DIAMOND:
            gtk_plot_data_draw_diamond (data, x, y, m*size, filled);                  
            break;
     case GTK_PLOT_SYMBOL_PLUS:
            gtk_plot_data_draw_plus (data, x, y, m*size);                  
            break;
     case GTK_PLOT_SYMBOL_CROSS:
            gtk_plot_data_draw_cross (data, x, y, m*size);                  
            break;
     case GTK_PLOT_SYMBOL_STAR:
            gtk_plot_data_draw_star (data, x, y, m*size);                  
            break;
     case GTK_PLOT_SYMBOL_IMPULSE:
            x0 = x;
            y0 = 0.;
            gtk_plot_get_pixel(plot, x0, y0, &px0, &py0);
            gtk_plot_pc_draw_line(plot->pc, 
                          x, MIN(py0,y), 
                          x,
                          MAX(py0,y));
            break;
  }

/*
  gdk_gc_set_clip_rectangle(gc, NULL);
*/
}

static void
gtk_plot_data_draw_xyz (GtkPlotData *dataset, gint npoints) 
{ 
  GtkWidget *widget;
  GtkPlot *plot;
  GdkRectangle clip_area, area;
  gint n;
  gdouble x = 0., y = 0., z = 0.;
  gdouble px, py;
  gdouble x0, y0;
  gdouble *array_x = NULL, *array_y = NULL, *array_z = NULL;

  plot = dataset->plot;
  widget = GTK_WIDGET(plot);

  array_x = gtk_plot_data_get_x(dataset, &n);
  array_y = gtk_plot_data_get_y(dataset, &n);
  array_z = gtk_plot_data_get_z(dataset, &n);

  if(!array_x || !array_y) return;

  if(dataset->x_line.line_style == GTK_PLOT_LINE_NONE &&
     dataset->y_line.line_style == GTK_PLOT_LINE_NONE &&
     dataset->z_line.line_style == GTK_PLOT_LINE_NONE) return;

  widget = GTK_WIDGET(plot);

  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;
  area.width = GTK_WIDGET(plot)->allocation.width;
  area.height = GTK_WIDGET(plot)->allocation.height;

  clip_area.x = area.x + roundint(plot->x * widget->allocation.width);
  clip_area.y = area.y + roundint(plot->y * widget->allocation.height);
  clip_area.width = roundint(plot->width * widget->allocation.width);
  clip_area.height = roundint(plot->height * widget->allocation.height);

  if(plot->clip_data) gtk_plot_pc_clip(plot->pc, &clip_area);

  for(n=dataset->num_points-npoints; n<=dataset->num_points-1; n++)
    {
      x = array_x[n];
      y = array_y[n];

      if(x >= plot->xmin && x <= plot->xmax) {

          if(GTK_IS_PLOT3D(plot)){
            gdouble pz, z0;
            if(array_z != NULL) z = array_z[n];
            if(z >= GTK_PLOT3D(plot)->zmin && z <= GTK_PLOT3D(plot)->zmax){ 
              gtk_plot3d_get_pixel(GTK_PLOT3D(plot), x, y, z, &px, &py, &pz);
              gtk_plot3d_get_pixel(GTK_PLOT3D(plot), 
                                   GTK_PLOT3D(plot)->origin.x, y, z, 
                                   &x0, &y0, &z0);
              gtk_plot_draw_line(plot, dataset->x_line,
                                 px,
                                 py,
                                 x0, 
                                 y0);
              gtk_plot3d_get_pixel(GTK_PLOT3D(plot), 
                                   x, GTK_PLOT3D(plot)->origin.y, z, 
                                   &x0, &y0, &z0);
              gtk_plot_draw_line(plot, dataset->y_line,
                                 px,
                                 py,
                                 x0, 
                                 y0);
              gtk_plot3d_get_pixel(GTK_PLOT3D(plot), 
                                   x, y, GTK_PLOT3D(plot)->origin.z, 
                                   &x0, &y0, &z0);
              gtk_plot_draw_line(plot, dataset->z_line,
                                 px,
                                 py,
                                 x0, 
                                 y0);
            } 
          } else {
            gtk_plot_get_pixel(plot, x, y, &px, &py);
            gtk_plot_get_pixel(plot, x, MAX(0., plot->ymin), &x0, &y0);
            gtk_plot_draw_line(plot, dataset->x_line,
                               px,
                               py,
                               px, 
                               y0);
            gtk_plot_get_pixel(plot, MAX(0., plot->xmin) , y, &x0, &y0);
            gtk_plot_draw_line(plot, dataset->y_line,
                               px, 
                               py, 
                               x0, 
                               py);
          }
      }
    }

  if(plot->clip_data) gtk_plot_pc_clip(plot->pc, NULL);
}

static void
gtk_plot_data_draw_errbars(GtkPlotData *dataset,
                           gdouble x, gdouble y, gdouble z,
                           gdouble dx, gdouble dy, gdouble dz)
{
  GtkWidget *widget;
  GtkPlot *plot;
  GtkPlotPoint errbar[6];
  gdouble px, py;
  gdouble el_x, el_y, er_x, er_y, ed_x, ed_y, eu_x, eu_y;
  gdouble m;

  plot = dataset->plot;
  widget = GTK_WIDGET(plot);
  m = plot->magnification;

  if(!dataset->show_xerrbars && !dataset->show_yerrbars && !dataset->show_zerrbars) return;

  gtk_plot_pc_set_color (plot->pc, &dataset->symbol.color);
  gtk_plot_pc_set_lineattr (plot->pc, 
                            dataset->symbol.border.line_width/2, 0, 0, 0);

  if(GTK_IS_PLOT3D(plot)){
       gdouble pz;
       gtk_plot3d_get_pixel(GTK_PLOT3D(plot), x, y, z, &px, &py, &pz);
       gtk_plot3d_get_pixel(GTK_PLOT3D(plot), x, y, z + dz, &eu_x, &eu_y, &pz);
       gtk_plot3d_get_pixel(GTK_PLOT3D(plot), x, y, z - dz, &ed_x, &ed_y, &pz);
       if(dataset->show_zerrbars)
           {
              errbar[0].x = px - m * dataset->zerrbar_caps/2.; 
              errbar[0].y = eu_y; 
              errbar[1].x = px + m * dataset->zerrbar_caps/2.; 
              errbar[1].y = eu_y; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);

              errbar[0].x = px; 
              errbar[0].y = eu_y; 
              errbar[1].x = px; 
              errbar[1].y = ed_y; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);

              errbar[0].x = px - m * dataset->zerrbar_caps/2.; 
              errbar[0].y = ed_y; 
              errbar[1].x = px + m * dataset->zerrbar_caps/2.; 
              errbar[1].y = ed_y; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);
           }
  } else {
       gtk_plot_get_pixel(plot, x, y, &px, &py);
       gtk_plot_get_pixel(plot, x + dx, y, &er_x, &er_y);
       gtk_plot_get_pixel(plot, x - dx, y, &el_x, &el_y);
       gtk_plot_get_pixel(plot, x, y + dy, &eu_x, &eu_y);
       gtk_plot_get_pixel(plot, x, y - dy, &ed_x, &ed_y);

       if(dataset->show_xerrbars)
           {
              errbar[0].x = el_x; 
              errbar[0].y = py - m * dataset->xerrbar_caps/2.; 
              errbar[1].x = el_x; 
              errbar[1].y = py + m * dataset->xerrbar_caps/2.; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);

              errbar[0].x = el_x; 
              errbar[0].y = py; 
              errbar[1].x = er_x; 
              errbar[1].y = py; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);
     
              errbar[0].x = er_x; 
              errbar[0].y = py - m * dataset->xerrbar_caps/2.; 
              errbar[1].x = er_x; 
              errbar[1].y = py + m * dataset->xerrbar_caps/2.; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);
           }

       if(dataset->show_yerrbars)
           {
              errbar[0].x = px - m * dataset->yerrbar_caps/2.; 
              errbar[0].y = eu_y; 
              errbar[1].x = px + m * dataset->yerrbar_caps/2.; 
              errbar[1].y = eu_y; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);

              errbar[0].x = px; 
              errbar[0].y = eu_y; 
              errbar[1].x = px; 
              errbar[1].y = ed_y; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);

              errbar[0].x = px - m * dataset->yerrbar_caps/2.; 
              errbar[0].y = ed_y; 
              errbar[1].x = px + m * dataset->yerrbar_caps/2.; 
              errbar[1].y = ed_y; 
              gtk_plot_pc_draw_lines(plot->pc, errbar, 2);
           }
  }
}
 
static void
gtk_plot_data_draw_down_triangle(GtkPlotData *data,  
                                 gdouble x, gdouble y, gdouble size, gint filled)
{
  GtkPlot *plot;
  GtkPlotPoint point[3];
  gdouble pi = acos(-1.);
  gdouble m;

  plot = data->plot;
  m = plot->magnification;


  point[0].x = x - m*size*cos(pi/6.)/2.;
  point[0].y = y - m*size*sin(pi/6.)/2.;

  point[1].x = x + m*size*cos(pi/6.)/2.;
  point[1].y = y - m*size*sin(pi/6.)/2.;

  point[2].x = x;
  point[2].y = y + m*size/2.;

  gtk_plot_pc_draw_polygon (plot->pc, filled, point, 3); 
}

static void
gtk_plot_data_draw_up_triangle(GtkPlotData *data,  
                               gdouble x, gdouble y, gdouble size, gint filled)
{
  GtkPlot *plot;
  GtkPlotPoint point[3];
  gdouble pi = acos(-1.);
  gdouble m;

  plot = data->plot;
  m = plot->magnification;

  point[0].x = x - m*size*cos(pi/6.)/2.;
  point[0].y = y + m*size*sin(pi/6.)/2.;

  point[1].x = x + m*size*cos(pi/6.)/2.;
  point[1].y = y + m*size*sin(pi/6.)/2.;

  point[2].x = x;
  point[2].y = y - m*size/2.;

  gtk_plot_pc_draw_polygon (plot->pc, filled, point, 3); 
}

static void
gtk_plot_data_draw_diamond(GtkPlotData *data,  
                           gdouble x, gdouble y, gdouble size, gint filled)
{
  GtkPlot *plot;
  GtkPlotPoint point[4];
  
  plot = data->plot;
  size = plot->magnification * size;

  point[0].x = x - size/2.;
  point[0].y = y;

  point[1].x = x;
  point[1].y = y - size/2.;

  point[2].x = x + size/2.;
  point[2].y = y;

  point[3].x = x;
  point[3].y = y + size/2.;

  gtk_plot_pc_draw_polygon (plot->pc,
                           filled,
                           point,
                           4); 
}

static void
gtk_plot_data_draw_left_triangle(GtkPlotData *data,  
                                 gdouble x, gdouble y, gdouble size, gint filled)
{
  GtkPlot *plot;
  GtkPlotPoint point[3];
  gdouble pi = acos(-1.);
  gdouble m;

  plot = data->plot;
  m = plot->magnification;

  point[0].x = x + m*size*sin(pi/6.)/2.;
  point[0].y = y - m*size*cos(pi/6.)/2.;

  point[1].x = x + m*size*sin(pi/6.)/2.;
  point[1].y = y + m*size*cos(pi/6.)/2.;

  point[2].x = x - m*size/2.;
  point[2].y = y;

  gtk_plot_pc_draw_polygon (plot->pc, filled, point, 3); 
}

static void
gtk_plot_data_draw_right_triangle(GtkPlotData *data,  
                                  gdouble x, gdouble y, gdouble size, gint filled)
{
  GtkPlot *plot;
  GtkPlotPoint point[3];
  gdouble pi = acos(-1.);
  gdouble m;

  plot = data->plot;
  m = plot->magnification;

  point[0].x = x - m*(gdouble)size*sin(pi/6.)/2.;
  point[0].y = y - m*(gdouble)size*cos(pi/6.)/2.;

  point[1].x = x - m*(gdouble)size*sin(pi/6.)/2.;
  point[1].y = y + m*(gdouble)size*cos(pi/6.)/2.;

  point[2].x = x + m*size/2.;
  point[2].y = y;

  gtk_plot_pc_draw_polygon (plot->pc, filled, point, 3); 
}


static void
gtk_plot_data_draw_plus(GtkPlotData *data, gdouble x, gdouble y, gdouble size)
{
  GtkPlot *plot;
  plot = data->plot;
  size = plot->magnification * size;
  gtk_plot_pc_draw_line (plot->pc,
                        x-size/2., y, x+size/2., y);
  
  gtk_plot_pc_draw_line (plot->pc,
                        x, y-size/2., x, y+size/2.);
}

static void
gtk_plot_data_draw_cross(GtkPlotData *data, gdouble x, gdouble y, gdouble size)
{
  GtkPlot *plot;
  plot = data->plot;
  size = plot->magnification * size;
  gtk_plot_pc_draw_line (plot->pc,
                        x-size/2., y-size/2., x+size/2., y+size/2.);
  
  gtk_plot_pc_draw_line (plot->pc,
                        x-size/2., y+size/2., x+size/2., y-size/2.);
}

static void
gtk_plot_data_draw_star(GtkPlotData *data, gdouble x, gdouble y, gdouble size)
{
  GtkPlot *plot;
  gdouble s2;

  plot = data->plot;

  size = plot->magnification * size;
  s2 = size*sqrt(2.)/4.;

  gtk_plot_pc_draw_line (plot->pc,
                        x-size/2., y, x+size/2., y);
  
  gtk_plot_pc_draw_line (plot->pc,
                        x, y-size/2., x, y+size/2.);

  gtk_plot_pc_draw_line (plot->pc,
                        x-s2, y-s2, x+s2, y+s2);
  
  gtk_plot_pc_draw_line (plot->pc,
                        x-s2, y+s2, x+s2, y-s2);
}

static void
gtk_plot_data_connect_points(GtkPlotData *dataset, gint npoints)
{
  GtkPlot *plot;
  GtkWidget *widget;
  GdkRectangle clip_area, area;
  GtkPlotPoint *points;
  GtkPlotData spline;
  GtkPlotPoint *spline_points = NULL;
  gdouble *array_x = NULL, *array_y = NULL, *array_z = NULL;
  gdouble *spline_coef = NULL;
  gdouble x, y;
  gint n, n0;
  gdouble px, py;
  gdouble x1, y1;
  gdouble xmin, xmax;
  gint num_points = dataset->num_points;

  plot = dataset->plot;
  widget = GTK_WIDGET(plot);

  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;
  area.width = GTK_WIDGET(plot)->allocation.width;
  area.height = GTK_WIDGET(plot)->allocation.height;

  clip_area.x = area.x + roundint(plot->x * widget->allocation.width);
  clip_area.y = area.y + roundint(plot->y * widget->allocation.height);
  clip_area.width = roundint(plot->width * widget->allocation.width);
  clip_area.height = roundint(plot->height * widget->allocation.height);

  if(dataset->line.line_style == GTK_PLOT_LINE_NONE) return;
  npoints = MIN(npoints, dataset->num_points);
  n0 = dataset->num_points - npoints;
  points = (GtkPlotPoint *)g_malloc(2*(dataset->num_points+1)*sizeof(GtkPlotPoint));

  if(plot->clip_data) gtk_plot_pc_clip(plot->pc, &clip_area);

  gtk_plot_set_line_attributes(plot, dataset->line);

  array_x = gtk_plot_data_get_x(dataset, &n); 
  array_y = gtk_plot_data_get_y(dataset, &n); 
  array_z = gtk_plot_data_get_z(dataset, &n); 

  switch(dataset->line_connector){
   case GTK_PLOT_CONNECT_STRAIGHT:
       if(npoints == 1) break;
       num_points = npoints;
       for(n=n0; n<dataset->num_points; n++)
        {
          x = array_x[n];
          y = array_y[n];
          if(GTK_IS_PLOT3D(plot)){
            gdouble pz;
            gdouble z = 0.;

            if(array_z != NULL) z = array_z[n];
            gtk_plot3d_get_pixel(GTK_PLOT3D(plot), x, y, z, &px, &py, &pz);
          } else { 
            gtk_plot_get_pixel(plot, x, y, &px, &py);
          }
          points[n-n0].x = px;
          points[n-n0].y = py;
        }
       break;
   case GTK_PLOT_CONNECT_HV_STEP:
       if(GTK_IS_PLOT3D(plot)) break;
       if(dataset->num_points == 1) break;
       num_points=0;
       for(n=0; n < dataset->num_points; n++)
        {
          x = array_x[n];
          y = array_y[n];
          gtk_plot_get_pixel(plot, x, y, &px, &py);
          points[num_points].x = px;
          points[num_points].y = py;
          num_points++;
          if(n < dataset->num_points-1)
            {
              gtk_plot_get_pixel(plot, 
                                 array_x[n+1], 
                                 array_y[n+1], 
                                 &px, &py);
              points[num_points].x = px;
              points[num_points].y = points[num_points-1].y;
              num_points++;
            }
        }
       break;
    case GTK_PLOT_CONNECT_VH_STEP:
       if(GTK_IS_PLOT3D(plot)) break;
       if(dataset->num_points == 1) break;
       num_points=0;
       for(n=0; n < dataset->num_points; n++)
        {
          x = array_x[n];
          y = array_y[n];
          gtk_plot_get_pixel(plot, x, y, &px, &py);
          points[num_points].x = px;
          points[num_points].y = py;
          num_points++;
          if(n < dataset->num_points-1)
            {
              gtk_plot_get_pixel(plot, 
                                 array_x[n+1], 
                                 array_y[n+1], 
                                 &px, &py);
              points[num_points].x = points[num_points-1].x;
              points[num_points].y = py;
              num_points++;
            }
        }
       break;
     case GTK_PLOT_CONNECT_MIDDLE_STEP:
       if(GTK_IS_PLOT3D(plot)) break;
       if(dataset->num_points == 1) break;
       num_points=1;
       for(n=1; n < dataset->num_points; n++)
        {
          x = array_x[n];
          y = array_y[n];
          gtk_plot_get_pixel(plot, x, y, &px, &py);
          x = array_x[n-1];
          y = array_y[n-1];
          gtk_plot_get_pixel(plot, x, y, &x1, &y1);
          points[num_points].x = (px+x1)/2;
          points[num_points].y = y1;
          num_points++;
          points[num_points].x = points[num_points-1].x;
          points[num_points].y = py;
          num_points++;
        }
       x = array_x[0];
       y = array_y[0];
       gtk_plot_get_pixel(plot, x, y, &px, &py);
       points[0].x = px;
       points[0].y = py;
       x = array_x[dataset->num_points-1];
       y = array_y[dataset->num_points-1];
       gtk_plot_get_pixel(plot, x, y, &px, &py);
       points[num_points].x = px;
       points[num_points].y = py;
       num_points++;
       break;
     case GTK_PLOT_CONNECT_SPLINE:
       if(GTK_IS_PLOT3D(plot)){
         g_free(points);
         return;
       }
       spline = *dataset;
       if(dataset->num_points <= 1) break;
       spline_coef = (gdouble *)g_malloc(dataset->num_points*sizeof(gdouble));
       spline_points = (GtkPlotPoint *)g_malloc(sizeof(GtkPlotPoint));
       spline_solve(dataset->num_points, array_x, array_y, spline_coef);
       gtk_plot_get_pixel(plot, array_x[0], 0, &xmin, &py);
       gtk_plot_get_pixel(plot, array_x[dataset->num_points-1], 0,
                          &xmax, &py);

       spline.num_points = (gint) (fabs(xmax - xmin) / spline.x_step) + 1;
       spline_points = (GtkPlotPoint *)g_malloc(sizeof(GtkPlotPoint) *
                                                (spline.num_points + 2));

       for(n=0, x1 = xmin; x1 <= xmax; x1 += spline.x_step, n++) {
         gtk_plot_get_point(plot, x1, 0, &x, &y);
         y = spline_eval(dataset->num_points, array_x, array_y, spline_coef, x);
         gtk_plot_get_pixel(plot, x, y, &px, &py);
         spline_points[n].x = px;
         spline_points[n].y = py;
       }

       if(dataset->fill_area){
         x = array_x[dataset->num_points - 1];
         y = 0.0;
         gtk_plot_get_pixel(plot, x, y, &px, &py);
         spline_points[spline.num_points].x = px; 
         spline_points[spline.num_points].y = py; 
         x = array_x[0];
         gtk_plot_get_pixel(plot, x, y, &px, &py);
         spline_points[spline.num_points + 1].x = px; 
         spline_points[spline.num_points + 1].y = py; 
         spline.num_points += 2;
         gtk_plot_pc_draw_polygon(plot->pc, TRUE, spline_points, spline.num_points);
       } else {
         gtk_plot_pc_draw_lines(plot->pc, spline_points, spline.num_points);
       }

       g_free(spline_points);
       g_free(spline_coef);
       g_free(points);
       if(plot->clip_data) gtk_plot_pc_clip(plot->pc, NULL);
       return;
     case GTK_PLOT_CONNECT_NONE:
     default:
       if(plot->clip_data) gtk_plot_pc_clip(plot->pc, NULL);
       g_free(points);
       return;
    }

  if(dataset->fill_area){
    if(dataset->num_points > 1){
      x = array_x[dataset->num_points-1];
      y = 0.0;
      gtk_plot_get_pixel(plot, x, y, &px, &py);
      points[num_points].x = px; 
      points[num_points].y = py; 
      x = array_x[dataset->num_points - npoints];
      gtk_plot_get_pixel(plot, x, y, &px, &py);
      points[num_points + 1].x = px; 
      points[num_points + 1].y = py; 
      num_points += 2;
      gtk_plot_pc_draw_polygon(plot->pc, TRUE, points, num_points);
    }
  } else {
    gtk_plot_pc_draw_lines(plot->pc, points, num_points);
  }

  if(plot->clip_data) gtk_plot_pc_clip(plot->pc, NULL);
  g_free(points);
}

/* New implementation */
GtkPlotArray *
gtk_plot_data_find_dimension(GtkPlotData *data, const gchar *name)
{
  GList *list = NULL;
                                                                                
  if(!data->data) return NULL;
                                                                                
  list = data->data->arrays;
  while(list){
    GtkPlotArray *dim = GTK_PLOT_ARRAY(list->data);
    if(dim && dim->name && strcmp(dim->name, name) == 0) return dim;
    list = list->next;
  }
  return NULL;
}

void
gtk_plot_data_add_dimension(GtkPlotData *data, 
                            const gchar *name,
                            const gchar *label, 
                            const gchar *desc, 
                            GtkType value_type,
			    gboolean required,
			    gboolean independent)
{
  GtkPlotArray *dim = NULL;
  if(!name) return;
  dim = gtk_plot_data_find_dimension(data, name);
  if(!dim){
    dim = GTK_PLOT_ARRAY(gtk_plot_array_new(name, NULL, 0, value_type, FALSE));
    gtk_plot_array_set_label(dim, label);
    gtk_plot_array_set_description(dim, desc);
    gtk_plot_array_set_required(dim, required);
    gtk_plot_array_set_independent(dim, independent);
    gtk_plot_array_list_add(data->data, dim);
  }
}

void
gtk_plot_data_remove_dimension(GtkPlotData *data, 
                               const gchar *name)
{
  GList *list;

  list = data->data->arrays;
  while(list){
    GtkPlotArray *dim = GTK_PLOT_ARRAY(list->data);
    if(dim && dim->name && strcmp(dim->name, name) == 0) {
      gtk_plot_array_list_remove(data->data, dim);
      list = data->data->arrays;
    } else {
      list = list->next;
    }
  }
}

void
gtk_plot_data_dimension_set_array(GtkPlotData *data,
				  const gchar *name,
				  GtkPlotArray *array)
{
  GtkPlotArray *dim;
  dim = gtk_plot_data_find_dimension(data, name);
  if(dim){
    gtk_plot_array_set_label(dim, array->label);
    gtk_plot_array_set_description(dim, array->description);
    dim->data = array->data;
    dim->own_data = FALSE;
  }
}

GtkPlotArray *
gtk_plot_data_dimension_set_points(GtkPlotData *data, 
                            const gchar *name, gdouble *points)
{
  GtkPlotArray *dim;
  dim = gtk_plot_data_find_dimension(data, name);
  if(dim && dim->type == GTK_TYPE_DOUBLE){
    dim->data.data_double = points;
    dim->own_data = FALSE;
  }
  return dim;
}

GtkPlotArray *
gtk_plot_data_dimension_get_array(GtkPlotData *data, 
                            const gchar *name)
{
  GtkPlotArray *dim;
  dim = gtk_plot_data_find_dimension(data, name);
  return dim;  
}

guint
gtk_plot_data_required_dimensions(GtkPlotData *data)
{
  guint n = 0;
  GtkPlotArrayList *dims = data->data;
  GList *list = dims->arrays;
  while(list){
    GtkPlotArray *dim = GTK_PLOT_ARRAY(list->data);
    if(dim->required) n++; 
    list = list->next;
  }
  return n;
}

guint
gtk_plot_data_independent_dimensions(GtkPlotData *data)
{
  guint n = 0;
  GtkPlotArrayList *dims = data->data;
  GList *list = dims->arrays;
  while(list){
    GtkPlotArray *dim = GTK_PLOT_ARRAY(list->data);
    if(dim->independent) n++; 
    list = list->next;
  }
  return n;
}

/******************************************
 * gtk_plot_data_set_points
 * gtk_plot_data_get_points
 * gtk_plot_data_set_x
 * gtk_plot_data_set_y
 * gtk_plot_data_set_z
 * gtk_plot_data_set_a
 * gtk_plot_data_set_dx
 * gtk_plot_data_set_dy
 * gtk_plot_data_set_dz
 * gtk_plot_data_set_da
 * gtk_plot_data_get_x
 * gtk_plot_data_get_y
 * gtk_plot_data_get_z
 * gtk_plot_data_get_a
 * gtk_plot_data_get_dx
 * gtk_plot_data_get_dy
 * gtk_plot_data_get_dz
 * gtk_plot_data_get_da
 * gtk_plot_data_set_numpoints
 * gtk_plot_data_get_numpoints
 * gtk_plot_data_set_symbol
 * gtk_plot_data_get_symbol
 * gtk_plot_data_set_connector
 * gtk_plot_data_get_connector
 * gtk_plot_data_set_xyz_attributes
 * gtk_plot_data_show_xerrbars
 * gtk_plot_data_show_yerrbars
 * gtk_plot_data_show_zerrbars
 * gtk_plot_data_hide_xerrbars
 * gtk_plot_data_hide_yerrbars
 * gtk_plot_data_hide_zerrbars
 * gtk_plot_data_set_legend
 * gtk_plot_data_show_legend
 * gtk_plot_data_hide_legend
 * gtk_plot_data_set_name
 * gtk_plot_data_show
 * gtk_plot_data_hide
 ******************************************/

void
gtk_plot_data_draw_points(GtkPlotData *data, gint npoints)
{
  gtk_plot_data_real_draw(data, npoints);
}

void
gtk_plot_data_set_points(GtkPlotData *data, 
                         gdouble *x, gdouble *y,
                         gdouble *dx, gdouble *dy,
                         gint num_points)
{
  gtk_plot_data_set_x(data, x);
  gtk_plot_data_set_y(data, y);
  gtk_plot_data_set_dx(data, dx);
  gtk_plot_data_set_dy(data, dy);
  gtk_plot_data_set_numpoints(data, num_points);
}

void
gtk_plot_data_get_points(GtkPlotData *data, 
                            gdouble **x, gdouble **y,
                            gdouble **dx, gdouble **dy,
                            gint *num_points)
{
  gint n;
  *x = gtk_plot_data_get_x(data, &n);
  *y = gtk_plot_data_get_y(data, &n);
  *dx = gtk_plot_data_get_dx(data, &n);
  *dy = gtk_plot_data_get_dy(data, &n);
  *num_points = data->num_points;
}

void
gtk_plot_data_get_point(GtkPlotData *dataset, gint n, 
                        gdouble *x, gdouble *y, gdouble *z, gdouble *a, 
                        gdouble *dx, gdouble *dy, gdouble *dz, gdouble *da, 
                        gchar **label, gboolean *error)
{
  gdouble *array = NULL;
  gchar **labels = NULL;
  gint np;
  gboolean show = TRUE;

  *error = FALSE;

  if(dataset->is_function){
    g_warning("This functions does not work for functions");
    *error = TRUE;
    return;
  } else if(dataset->is_iterator) {
    if(n >= dataset->num_points){
       g_warning("n >= dataset->num_points");
       *error = TRUE;
       return;
    }
    dataset->iterator (GTK_PLOT(dataset->plot), dataset, n, 
                       x, y, z, a, dx, dy, dz, da, label, error);
  } else {
    if(n >= dataset->num_points){
       g_warning("n >= dataset->num_points");
       *error = TRUE;
       return;
    }
    array = gtk_plot_data_get_x(dataset, &np);
    if(array) *x = array[n]; 
    array = gtk_plot_data_get_y(dataset, &np);
    if(array) *y = array[n]; 
    array = gtk_plot_data_get_z(dataset, &np);
    if(array) *z = array[n]; 
    array = gtk_plot_data_get_a(dataset, &np);
    if(array) *a = array[n]; 
    array = gtk_plot_data_get_dx(dataset, &np);
    if(array) *dx = array[n]; 
    array = gtk_plot_data_get_dy(dataset, &np);
    if(array) *dy = array[n]; 
    array = gtk_plot_data_get_dz(dataset, &np);
    if(array) *dz = array[n]; 
    array = gtk_plot_data_get_da(dataset, &np);
    if(array) *da = array[n]; 

    labels = gtk_plot_data_get_labels(dataset, &show);
    if(labels) *label = labels[n]; 
  }
}

GtkPlotArray *
gtk_plot_data_set_x(GtkPlotData *data, 
                    gdouble *x) 
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_set_points(data, "x", x);
  return array;
}

GtkPlotArray *
gtk_plot_data_set_y(GtkPlotData *data, 
                    gdouble *y) 
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_set_points(data, "y", y);
  return array;
}

GtkPlotArray *
gtk_plot_data_set_z(GtkPlotData *data, 
                    gdouble *z) 
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_set_points(data, "z", z);
  return array;
}

GtkPlotArray *
gtk_plot_data_set_a(GtkPlotData *data, 
                    gdouble *a) 
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_set_points(data, "a", a);
  return array;
}

GtkPlotArray *
gtk_plot_data_set_dx(GtkPlotData *data, 
                     gdouble *dx) 
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_set_points(data, "dx", dx);
  return array;
}

GtkPlotArray *
gtk_plot_data_set_dy(GtkPlotData *data, 
                     gdouble *dy) 
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_set_points(data, "dy", dy);
  return array;
}


GtkPlotArray *
gtk_plot_data_set_dz(GtkPlotData *data, 
                     gdouble *dz) 
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_set_points(data, "ErrZ", dz);
  return array;
}

GtkPlotArray *
gtk_plot_data_set_da(GtkPlotData *data, 
                     gdouble *da) 
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_set_points(data, "da", da);
  return array;
}

void
gtk_plot_data_set_a_scale(GtkPlotData *data, gdouble a_scale) 
{
  GtkPlotArray *dim = gtk_plot_data_find_dimension(data, "a");
  if(dim) gtk_plot_array_set_scale(dim, a_scale);
}


GtkPlotArray *
gtk_plot_data_set_labels(GtkPlotData *data, 
                         gchar **labels) 
{
  GtkPlotArray *dim = NULL;
  if(labels){
    dim = gtk_plot_data_find_dimension(data, "labels");
    if(dim){
      gtk_plot_array_free(dim);
      dim->data.data_string = labels;
    }
  }
  return dim;
}


gdouble *
gtk_plot_data_get_x(GtkPlotData *dataset, gint *num_points)
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_get_array(dataset, "x");
  if(array){
    *num_points = gtk_plot_array_get_size(array);
    return gtk_plot_array_get_double(array);
  }
  return NULL;
}

gdouble *
gtk_plot_data_get_y(GtkPlotData *dataset, gint *num_points)
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_get_array(dataset, "y");
  if(array){
    *num_points = gtk_plot_array_get_size(array);
    return gtk_plot_array_get_double(array);
  }
  return NULL;
}

gdouble *
gtk_plot_data_get_z(GtkPlotData *dataset, gint *num_points)
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_get_array(dataset, "z");
  if(array){
    *num_points = gtk_plot_array_get_size(array);
    return gtk_plot_array_get_double(array);
  }
  return NULL;
}

gdouble *
gtk_plot_data_get_a(GtkPlotData *dataset, gint *num_points)
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_get_array(dataset, "a");
  if(array){
    *num_points = gtk_plot_array_get_size(array);
    return gtk_plot_array_get_double(array);
  }
  return NULL;
}

gdouble *
gtk_plot_data_get_dx(GtkPlotData *dataset, gint *num_points)
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_get_array(dataset, "dx");
  if(array){
    *num_points = gtk_plot_array_get_size(array);
    return gtk_plot_array_get_double(array);
  }
  return NULL;
}

gdouble *
gtk_plot_data_get_dy(GtkPlotData *dataset, gint *num_points)
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_get_array(dataset, "dy");
  if(array){
    *num_points = gtk_plot_array_get_size(array);
    return gtk_plot_array_get_double(array);
  }
  return NULL;
}

gdouble *
gtk_plot_data_get_dz(GtkPlotData *dataset, gint *num_points)
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_get_array(dataset, "ErrZ");
  if(array){
    *num_points = gtk_plot_array_get_size(array);
    return gtk_plot_array_get_double(array);
  }
  return NULL;
}

gdouble *
gtk_plot_data_get_da(GtkPlotData *dataset, gint *num_points)
{
  GtkPlotArray *array;
  array = gtk_plot_data_dimension_get_array(dataset, "da");
  if(array){
    *num_points = gtk_plot_array_get_size(array);
    return gtk_plot_array_get_double(array);
  }
  return NULL;
}

gdouble 
gtk_plot_data_get_a_scale(GtkPlotData *dataset)
{
  GtkPlotArray *dim = gtk_plot_data_find_dimension(dataset, "a");
  if(dim) return gtk_plot_array_get_scale(dim);
  return 0.;
}


gchar **
gtk_plot_data_get_labels(GtkPlotData *dataset, gboolean *show_labels)
{
  GtkPlotArray *array;
  *show_labels = dataset->show_labels;
  array = gtk_plot_data_find_dimension(dataset, "labels");
  if(array) return gtk_plot_array_get_string(array);
  return NULL;
}

void
gtk_plot_data_show_labels(GtkPlotData *dataset, gboolean show_labels)
{
  dataset->show_labels = show_labels;
}

void
gtk_plot_data_labels_set_attributes (GtkPlotData *data,	
				     const gchar *font,
                                     gint height,
                                     gint angle,
			             const GdkColor *fg,
			             const GdkColor *bg)
{
  if(!font){
   /* Use previous font */
/*
    if(data->labels_attr.font) g_free(data->labels_attr.font);
    data->labels_attr.font = g_strdup(DEFAULT_FONT);
    data->labels_attr.height = DEFAULT_FONT_HEIGHT;
*/
  } else {
    if(data->labels_attr.font) g_free(data->labels_attr.font);
    data->labels_attr.font = g_strdup(font);
    data->labels_attr.height = height;
  }

  data->labels_attr.angle = angle;

  if(fg) data->labels_attr.fg = *fg;
  if(bg) data->labels_attr.bg = *bg;

}


void
gtk_plot_data_set_numpoints(GtkPlotData *dataset, gint numpoints)
{
  GList *list;
  dataset->num_points = numpoints;
  list = dataset->data->arrays;
  while(list){
    GtkPlotArray *array = GTK_PLOT_ARRAY(list->data);
    array->size = numpoints;
    list = list->next;
  } 
}

gint
gtk_plot_data_get_numpoints(GtkPlotData *dataset)
{
  return(dataset->num_points);
}

void
gtk_plot_data_set_symbol (GtkPlotData *dataset,
		          GtkPlotSymbolType type,
		          GtkPlotSymbolStyle style,
                          gint size, gfloat line_width, 
                          const GdkColor *color, const GdkColor *border_color)
{
  dataset->symbol.symbol_type = type;
  dataset->symbol.symbol_style = style;
  dataset->symbol.size = size;
  dataset->symbol.border.line_width = line_width;
  dataset->symbol.border.color = *border_color;
  dataset->symbol.color = *color;
}

void
gtk_plot_data_get_symbol (GtkPlotData *dataset,
		          GtkPlotSymbolType *type,
		          GtkPlotSymbolStyle *style,
                          gint *size, gfloat *line_width, 
                          GdkColor *color, GdkColor *border_color)
{
  *type = dataset->symbol.symbol_type;
  *style = dataset->symbol.symbol_style;
  *size = dataset->symbol.size;
  *line_width = dataset->symbol.border.line_width;
  *color = dataset->symbol.color;
  *border_color = dataset->symbol.border.color;
}

void
gtk_plot_data_set_line_attributes (GtkPlotData *dataset, 
                                   GtkPlotLineStyle style,
                                   GdkCapStyle cap_style,
                                   GdkJoinStyle join_style,
                                   gfloat width,
                                   const GdkColor *color)
{
  dataset->line.line_style = style; 
  dataset->line.cap_style = cap_style;
  dataset->line.join_style = join_style;
  dataset->line.line_width = width; 
  dataset->line.color = *color; 
}

void
gtk_plot_data_get_line_attributes (GtkPlotData *dataset, 
                                   GtkPlotLineStyle *style,
                                   GdkCapStyle *cap_style,
                                   GdkJoinStyle *join_style,
                                   gfloat *width,
                                   GdkColor *color)
{
  *style = dataset->line.line_style; 
  *cap_style = dataset->line.cap_style; 
  *join_style = dataset->line.join_style; 
  *width = dataset->line.line_width; 
  *color = dataset->line.color; 
}


void
gtk_plot_data_set_connector (GtkPlotData *dataset,
		             GtkPlotConnector connector)
{
  dataset->line_connector = connector;
}

gint 
gtk_plot_data_get_connector (GtkPlotData *dataset)
{
  return (dataset->line_connector);
}

void
gtk_plot_data_set_x_attributes (GtkPlotData *dataset, 
                           	GtkPlotLineStyle style,
                           	GdkCapStyle cap_style,
                           	GdkJoinStyle join_style,
                            	gfloat width,
                            	const GdkColor *color)
{
  dataset->x_line.line_style = style; 
  dataset->x_line.cap_style = cap_style; 
  dataset->x_line.join_style = join_style; 
  dataset->x_line.line_width = width; 
  dataset->x_line.color = *color; 
}

void
gtk_plot_data_set_y_attributes (GtkPlotData *dataset, 
                           	GtkPlotLineStyle style,
                           	GdkCapStyle cap_style,
                           	GdkJoinStyle join_style,
                            	gfloat width,
                            	const GdkColor *color)
{
  dataset->y_line.line_style = style; 
  dataset->y_line.cap_style = cap_style; 
  dataset->y_line.join_style = join_style; 
  dataset->y_line.line_width = width; 
  dataset->y_line.color = *color; 
}

void
gtk_plot_data_set_z_attributes (GtkPlotData *dataset, 
                           	GtkPlotLineStyle style,
                           	GdkCapStyle cap_style,
                           	GdkJoinStyle join_style,
                            	gfloat width,
                            	const GdkColor *color)
{
  dataset->z_line.line_style = style; 
  dataset->z_line.cap_style = cap_style; 
  dataset->z_line.join_style = join_style; 
  dataset->z_line.line_width = width; 
  dataset->z_line.color = *color; 
}


void
gtk_plot_data_show_xerrbars(GtkPlotData *dataset) 
{
  dataset->show_xerrbars = TRUE;
}

void
gtk_plot_data_show_yerrbars(GtkPlotData *dataset) 
{
  dataset->show_yerrbars = TRUE;
}

void
gtk_plot_data_show_zerrbars(GtkPlotData *dataset) 
{
  dataset->show_zerrbars = TRUE;
}


void
gtk_plot_data_hide_xerrbars(GtkPlotData *dataset) 
{
  dataset->show_xerrbars = FALSE;
}

void
gtk_plot_data_hide_yerrbars(GtkPlotData *dataset) 
{
  dataset->show_yerrbars = FALSE;
}

void
gtk_plot_data_hide_zerrbars(GtkPlotData *dataset) 
{
  dataset->show_zerrbars = FALSE;
}

void
gtk_plot_data_fill_area(GtkPlotData *dataset, gboolean fill)
{
  dataset->fill_area = fill;
}

gboolean
gtk_plot_data_area_is_filled(GtkPlotData *dataset)
{
  return (dataset->fill_area);
}

void
gtk_plot_data_show_legend(GtkPlotData *dataset)
{
  dataset->show_legend = TRUE;
}

void
gtk_plot_data_hide_legend(GtkPlotData *dataset)
{
  dataset->show_legend = FALSE;
}

void
gtk_plot_data_set_legend(GtkPlotData *dataset,
                            const gchar *legend)
{
  if(legend){
     g_free(dataset->legend);
     dataset->legend = g_strdup(legend);
  }
}

void
gtk_plot_data_set_legend_precision (GtkPlotData *data,
                                     gint precision)
{
  data->legends_precision = precision;
}

gint
gtk_plot_data_get_legend_precision (GtkPlotData *data)
{

  return (data->legends_precision);
}


void
gtk_plot_data_set_name(GtkPlotData *dataset, const gchar *name)
{
  if(dataset->name)
     g_free(dataset->name);

  dataset->name = g_strdup(name);
}

void
gtk_plot_data_set_link(GtkPlotData *dataset, gpointer link)
{
  dataset->link = link; 
}

gpointer
gtk_plot_data_get_link(GtkPlotData *dataset)
{
  return(dataset->link); 
}

void
gtk_plot_data_remove_link(GtkPlotData *dataset)
{
  dataset->link = NULL; 
}

void
gtk_plot_data_set_gradient_mask (GtkPlotData *data, gint mask)
{
  data->gradient_mask = mask;
  gtk_signal_emit(GTK_OBJECT(data), data_signals[GRADIENT_COLORS_CHANGED]);
}

gint
gtk_plot_data_get_gradient_mask (GtkPlotData *data)
{
  return (data->gradient_mask);
}

void
gtk_plot_data_gradient_set_visible (GtkPlotData *data, gboolean visible)
{
  data->show_gradient = visible;
}

gboolean
gtk_plot_data_gradient_visible (GtkPlotData *data)
{
  return(data->show_gradient);
}

void
gtk_plot_data_gradient_autoscale_a (GtkPlotData *data)
{
  gdouble amin, amax;
  gint n;
  gboolean change = FALSE;
  gdouble *array_a = NULL;

  if(data->is_function) return;

  array_a = gtk_plot_data_get_a(data, &n);
  if(!array_a && !(data->iterator_mask & GTK_PLOT_DATA_A)) return;

  amin = 1.e16;
  amax = -1.e16;

  for(n = 0; n < data->num_points; n++){
    gdouble fa;
    fa = array_a[n];
    if(fa < amin) amin = fa;
    if(fa > amax) amax = fa;
    change = TRUE;
  }

  if(!change) return;
  real_autoscale_gradient(data, amin, amax);
}

void
gtk_plot_data_gradient_autoscale_da (GtkPlotData *data)
{
  gdouble amin, amax;
  gint n;
  gboolean change = FALSE;
  gdouble *array_da = NULL;

  if(data->is_function) return;

  array_da = gtk_plot_data_get_da(data, &n);
  if(!array_da && !(data->iterator_mask & GTK_PLOT_DATA_DA)) return;

  amin = 1.e16;
  amax = -1.e16;

  for(n = 0; n < data->num_points; n++){
    gdouble fda = array_da[n];
    if(fda < amin) amin = fda;
    if(fda > amax) amax = fda;
    change = TRUE;
  }

  if(!change) return;
  real_autoscale_gradient(data, amin, amax);
}

void
gtk_plot_data_gradient_autoscale_z (GtkPlotData *data)
{
  gdouble zmin, zmax;
  gint n;
  gboolean change = FALSE;
  gdouble *array_z = NULL;

  if(data->is_function) return;

  array_z = gtk_plot_data_get_z(data, &n);
  if(!array_z && !(data->iterator_mask & GTK_PLOT_DATA_Z)) return;

  zmin = 1.e16;
  zmax = -1.e16;

  for(n = 0; n < data->num_points; n++){
    gdouble fz = array_z[n];
    if(fz < zmin) zmin = fz;
    if(fz > zmax) zmax = fz;
    change = TRUE;
  }

  if(!change) return;
  real_autoscale_gradient(data, zmin, zmax);
}

static void
real_autoscale_gradient(GtkPlotData *data, gdouble xmin, gdouble xmax)
{
  gint p;
  gtk_plot_axis_ticks_autoscale(data->gradient, xmin, xmax, &p);
  gtk_plot_data_reset_gradient(data);
  data->legends_precision = p;
  data->legends_style = data->gradient->ticks.scale == GTK_PLOT_SCALE_LINEAR ? GTK_PLOT_LABEL_FLOAT : GTK_PLOT_LABEL_EXP;
}

void
gtk_plot_data_set_gradient_colors (GtkPlotData *data,
                                   const GdkColor *min,
                                   const GdkColor *max)
{
  data->color_min= *min;
  data->color_max = *max;
  gtk_signal_emit(GTK_OBJECT(data), data_signals[GRADIENT_COLORS_CHANGED]);
}

void
gtk_plot_data_get_gradient_colors (GtkPlotData *data,
                                   GdkColor *min,
                                   GdkColor *max)
{
  min = &data->color_min;
  max = &data->color_max;
}

void
gtk_plot_data_set_gradient_nth_color (GtkPlotData *data,
                                      guint level,
                                      GdkColor *color)
{
  if(!data->gradient_custom) return;

  if(level > data->gradient->ticks.nticks) return;
  data->gradient_colors[level] = *color;
  gtk_signal_emit(GTK_OBJECT(data), data_signals[GRADIENT_COLORS_CHANGED]);
}

const GdkColor *
gtk_plot_data_get_gradient_nth_color (GtkPlotData *data,
                                      guint level)
{
  if(level > data->gradient->ticks.nticks) return NULL;
  return &data->gradient_colors[level];
}

void
gtk_plot_data_set_gradient_outer_colors (GtkPlotData *data,
                                         const GdkColor *min,
                                         const GdkColor *max)
{
  data->color_lt_min = *min;
  data->color_gt_max = *max;
  gtk_signal_emit(GTK_OBJECT(data), data_signals[GRADIENT_COLORS_CHANGED]);
}

void
gtk_plot_data_get_gradient_outer_colors (GtkPlotData *data,
                                         GdkColor *min,
                                         GdkColor *max)
{
  min = &data->color_lt_min;
  max = &data->color_gt_max;
}

void
gtk_plot_data_set_gradient (GtkPlotData *data,
                            gdouble min, gdouble max, 
			    gint nlevels, gint nsublevels)
{
  data->gradient->ticks.min = min;
  data->gradient->ticks.max = max;
  data->gradient->ticks.nmajorticks = nlevels;
  data->gradient->ticks.nminor = nsublevels;

  gtk_plot_data_reset_gradient(data);
}

void
gtk_plot_data_get_gradient (GtkPlotData *data,
                            gdouble *min, gdouble *max, 
			    gint *nlevels, gint *nsublevels)
{
  *min = data->gradient->ticks.min;
  *max = data->gradient->ticks.max;
  *nlevels = data->gradient->ticks.nmajorticks;
  *nsublevels = data->gradient->ticks.nminor;
}

void
gtk_plot_data_get_gradient_level (GtkPlotData *data, gdouble level, GdkColor *color)
{
  GdkColor min, max;
  gdouble red, green, blue;
  gdouble h, s, v;
  gdouble h1, s1, v1;
  gdouble h2, s2, v2;
  gdouble value;
  GtkPlotTicks *ticks = &data->gradient->ticks;

  min = data->color_min; 
  max = data->color_max; 

  if(level > ticks->max) { *color = data->color_gt_max; return; }
  if(level < ticks->min) { *color = data->color_lt_min; return; }
  if(data->gradient_custom){
    gint i;
    gint start = ticks->scale == GTK_PLOT_SCALE_LINEAR ? (level - ticks->min) / (ticks->max - ticks->min) * ticks->nticks : 0;
    gint end = ticks->nticks;
    for(i = MAX(start-2,0); i < end; i++){
      if(level > ticks->values[i].value && level <= ticks->values[i+1].value)
        {
           *color = data->gradient_colors[i];
           return;
        }
    }
    *color = data->color_gt_max;
    return;
  }

  value = gtk_plot_axis_ticks_transform(data->gradient, level);

  red = min.red;
  green = min.green;
  blue = min.blue;
  rgb_to_hsv(red, green, blue, &h1, &s1, &v1);
  red = max.red;
  green = max.green;
  blue = max.blue;
  rgb_to_hsv(red, green, blue, &h2, &s2, &v2);

  s = 1.;
  v = 1.;
  h = 1.;
  if(data->gradient_mask & GTK_PLOT_GRADIENT_S)
                    s = s1 + (s2 - s1) * value;
  if(data->gradient_mask & GTK_PLOT_GRADIENT_V)
                    v = v1 + (v2 - v1) * value;
  if(data->gradient_mask & GTK_PLOT_GRADIENT_H)
                    h = h1 + (h2 - h1) * value;

  hsv_to_rgb(h, MIN(s, 1.0), MIN(v, 1.0), &red, &green, &blue);

  color->red = red;
  color->green = green;
  color->blue = blue;
  gdk_color_alloc(gtk_widget_get_colormap(GTK_WIDGET(data)), color);
}

void
gtk_plot_data_gradient_set_style        (GtkPlotData *data,
                                         GtkPlotLabelStyle style,
                                         gint precision)
{
  data->legends_style = style,
  data->legends_precision = style;
}

void            gtk_plot_data_gradient_set_scale        (GtkPlotData *data,
                                                         GtkPlotScale scale)
{
  data->gradient->ticks.scale = scale;
  gtk_plot_data_reset_gradient(data);
}

static void
hsv_to_rgb (gdouble  h, gdouble  s, gdouble  v,
            gdouble *r, gdouble *g, gdouble *b)
{
  gint i;
  gdouble f, w, q, t;

  if (s == 0.0)
    s = 0.000001;

  if (h == -1.0)
    {
      *r = v;
      *g = v;
      *b = v;
    }
  else
    {
      if (h == 360.0) h = 0.0;
      h = h / 60.0;
      i = (gint) h;
      f = h - i;
      w = v * (1.0 - s);
      q = v * (1.0 - (s * f));
      t = v * (1.0 - (s * (1.0 - f)));

      switch (i)
      {
        case 0:
          *r = v;
          *g = t;
          *b = w;
          break;
        case 1:
          *r = q;
          *g = v;
          *b = w;
          break;
        case 2:
          *r = w;
          *g = v;
          *b = t;
          break;
        case 3:
          *r = w;
          *g = q;
          *b = v;
          break;
        case 4:
          *r = t;
          *g = w;
          *b = v;
          break;
        case 5:
          *r = v;
          *g = w;
          *b = q;
          break;
      }
    }

  *r *= 65535.;
  *g *= 65535.;
  *b *= 65535.;
}


static void
rgb_to_hsv (gdouble  r, gdouble  g, gdouble  b,
            gdouble *h, gdouble *s, gdouble *v)
{
  double max, min, delta;

  r /= 65535.;
  g /= 65535.;
  b /= 65535.;

  max = r;
  if (g > max)
    max = g;
  if (b > max)
    max = b;

  min = r;
  if (g < min)
    min = g;
  if (b < min)
    min = b;

  *v = max;
  if (max != 0.0)
    *s = (max - min) / max;
  else
    *s = 0.0;

  if (*s == 0.0)
    *h = -1.0;
  else
    {
      delta = max - min;

      if (r == max)
        *h = (g - b) / delta;
      else if (g == max)
        *h = 2.0 + (b - r) / delta;
      else if (b == max)
        *h = 4.0 + (r - g) / delta;

      *h = *h * 60.0;

      if (*h < 0.0)
        *h = *h + 360;
    }
}

/* Solve the tridiagonal equation system that determines the second
   derivatives for the interpolation points.  (Based on Numerical
   Recipes 2nd Edition.) */
static void
spline_solve (int n, gdouble x[], gdouble y[], gdouble y2[])
{
  gdouble p, sig, *u;
  gint i, k;

  if(n == 1) return;

  u = g_malloc ((n - 1) * sizeof (u[0]));

  y2[0] = u[0] = 0.0;	/* set lower boundary condition to "natural" */

  for (i = 1; i < n - 1; ++i)
    {
      sig = (x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
      p = sig * y2[i - 1] + 2.0;
      y2[i] = (sig - 1.0) / p;
      u[i] = ((y[i + 1] - y[i])
	      / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]));
      u[i] = (6.0 * u[i] / (x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
    }

  y2[n - 1] = 0.0;
  for (k = n - 2; k >= 0; --k)
    y2[k] = y2[k] * y2[k + 1] + u[k];

  g_free (u);
}

static gdouble
spline_eval (int n, gdouble x[], gdouble y[], gdouble y2[], gdouble val)
{
  gint k_lo, k_hi, k;
  gdouble h, b, a;

  if(n == 1) return y[0];

  /* do a binary search for the right interval: */
  k_lo = 0; k_hi = n - 1;
  while (k_hi - k_lo > 1)
    {
      k = (k_hi + k_lo) / 2;
      if (x[k] > val)
	k_hi = k;
      else
	k_lo = k;
    }

  h = x[k_hi] - x[k_lo];
  g_assert (h > 0.0);

  a = (x[k_hi] - val) / h;
  b = (val - x[k_lo]) / h;
  return a*y[k_lo] + b*y[k_hi] +
    ((a*a*a - a)*y2[k_lo] + (b*b*b - b)*y2[k_hi]) * (h*h)/6.0;
}

void
gtk_plot_data_reset_gradient(GtkPlotData *data)
{
  gdouble min, max;
  gint nmajorticks = data->gradient->ticks.nmajorticks;
  data->gradient->ticks.step = (data->gradient->ticks.max - data->gradient->ticks.min)/data->gradient->ticks.nmajorticks;
  gtk_plot_axis_ticks_recalc(data->gradient);

  max = data->gradient->ticks.max;
  min = data->gradient->ticks.min;
  if(data->gradient->ticks.set_limits){
    max = MIN(max, data->gradient->ticks.end);
    min = MAX(min, data->gradient->ticks.begin);
  } 
  data->gradient->ticks.nmajorticks = nmajorticks;
  data->gradient->ticks.values[0].value = min;
  data->gradient->ticks.values[data->gradient->ticks.nticks-1].value = max;

  gtk_signal_emit(GTK_OBJECT(data), data_signals[GRADIENT_CHANGED]);
  gtk_plot_data_reset_gradient_colors(data);
}


void
gtk_plot_data_reset_gradient_colors(GtkPlotData *data)
{
  GtkPlotTicks *ticks;
  gdouble max = 0., min = 0.;
  gint nminor = 0;
  gint i;
  gboolean custom;
  GdkColor color;
  gdouble value;
     
  ticks = &data->gradient->ticks;
  if(ticks->nticks == 0) return;

  custom = data->gradient_custom;
  nminor = data->gradient->ticks.nminor;
  max = data->gradient->ticks.max;
  min = data->gradient->ticks.min;

  if(data->gradient_colors)
    g_free(data->gradient_colors);

  data->gradient_custom = FALSE;
  data->gradient_colors = g_new0(GdkColor, ticks->nticks + 1);
  data->gradient->ticks.max = ticks->values[ticks->nticks-1].value; 
  data->gradient->ticks.min = ticks->values[1].value; 
  data->gradient->ticks.nminor = 0;

  for(i = 1; i < data->gradient->ticks.nticks-2; i++){
    value = ticks->values[i+1].value;
    gtk_plot_data_get_gradient_level(data, value, &color);
    data->gradient_colors[i] = color;
  } 
  data->gradient_colors[0] = data->color_min;
  data->gradient_colors[data->gradient->ticks.nticks-2] = data->color_max;

  data->gradient->ticks.max = max;
  data->gradient->ticks.min = min;
  data->gradient_custom = custom;
  data->gradient->ticks.nminor = nminor;
  gtk_signal_emit(GTK_OBJECT(data), data_signals[GRADIENT_COLORS_CHANGED]);
}

void
gtk_plot_data_gradient_use_custom_colors(GtkPlotData *data, gboolean custom)
{
  data->gradient_custom = custom;
}

gboolean
gtk_plot_data_gradient_custom_colors(GtkPlotData *data)
{
  return(data->gradient_custom);
}

GtkPlotMarker *
gtk_plot_data_add_marker(GtkPlotData *data, guint point)
{
  GtkPlotMarker *marker = NULL;
/*
  GList *list = NULL;
*/

  if(point >= data->num_points) return FALSE;

/*
  list = data->markers;
  while(list){
    marker = (GtkPlotMarker *)list->data;
    if(marker->point == point) return FALSE;
    list = list->next;
  }
*/

  marker = g_new0(GtkPlotMarker, 1);

  marker->data = data;
  marker->point = point;  

  data->markers = g_list_append(data->markers, marker);
  return (marker);
}

gboolean
gtk_plot_data_remove_marker(GtkPlotData *data, GtkPlotMarker *marker)
{
  GList *list;

  list = data->markers;
  while(list){
    GtkPlotMarker *point;

    point = (GtkPlotMarker *)list->data;
    if(point == marker){
      g_free(marker);
      data->markers = g_list_remove_link(data->markers, list);
      g_list_free_1(list);
      return TRUE;
    }
    list = list->next;
  } 

  return FALSE;
} 

void
gtk_plot_data_remove_markers(GtkPlotData *data)
{
  GList *list;

  list = data->markers;
  while(list){
    g_free(list->data);
    data->markers = g_list_remove_link(data->markers, list); 
    g_list_free_1(list);
    list = data->markers; 
  }

  data->markers = NULL;
}

void
gtk_plot_data_show_markers(GtkPlotData *data, gboolean show)
{
  data->show_markers = show;
}

gboolean
gtk_plot_data_markers_visible(GtkPlotData *data)
{
  return (data->show_markers);
}

static void
gtk_plot_data_real_clone(GtkPlotData *real_data, GtkPlotData *copy_data)
{
  gdouble min, max;
  gint i, nlevels, nsublevels;

  copy_data->link = real_data->link;
  
  copy_data->is_iterator = real_data->is_iterator;
  copy_data->is_function = real_data->is_function;
  copy_data->iterator = real_data->iterator;
  copy_data->function = real_data->function;
  copy_data->function3d = real_data->function3d;
  copy_data->num_points = real_data->num_points;
  copy_data->iterator_mask = real_data->iterator_mask;
  
  copy_data->symbol = real_data->symbol;
  copy_data->line = real_data->line;
  copy_data->line_connector = real_data->line_connector;

  copy_data->show_legend = real_data->show_legend;
  copy_data->show_labels = real_data->show_labels;
  copy_data->fill_area = real_data->fill_area;
  copy_data->labels_offset = real_data->labels_offset;
  copy_data->legends_precision = real_data->legends_precision;
  copy_data->legends_style = real_data->legends_style;
  copy_data->gradient_custom = real_data->gradient_custom;
  copy_data->show_gradient = real_data->show_gradient;
  gtk_plot_data_set_legend(copy_data, real_data->legend);
  gtk_plot_data_set_name(copy_data, real_data->name);
  copy_data->x_step = real_data->x_step;
  copy_data->y_step = real_data->y_step;
  copy_data->z_step = real_data->z_step;

  copy_data->x_line = real_data->x_line;
  copy_data->y_line = real_data->y_line;
  copy_data->z_line = real_data->z_line;

  copy_data->show_xerrbars = real_data->show_xerrbars;
  copy_data->show_yerrbars = real_data->show_yerrbars;
  copy_data->show_zerrbars = real_data->show_zerrbars;

  copy_data->gradient_mask = real_data->gradient_mask;
  copy_data->color_min = real_data->color_min;
  copy_data->color_max = real_data->color_max;
  copy_data->color_lt_min = real_data->color_lt_min;
  copy_data->color_gt_max = real_data->color_gt_max;

  gtk_plot_data_get_gradient(real_data, &min, &max, &nlevels, &nsublevels);
  gtk_plot_data_set_gradient(copy_data, min, max, nlevels, nsublevels);

  for(i = 0; i < copy_data->gradient->ticks.nticks; i++){
    gtk_plot_data_set_gradient_nth_color(copy_data, i, &real_data->gradient_colors[i]);
  }

}
