/* gtkplot - 2d scientific plots widget for gtk+
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
#include "gtkpsfont.h"
#include "gtkplotgdk.h"
#include "gtkextra-marshal.h"

#define DEFAULT_WIDTH 420
#define DEFAULT_HEIGHT 340
#define DEFAULT_FONT_HEIGHT 12
#define LABEL_MAX_LENGTH 100

#define P_(string) string

static gchar DEFAULT_FONT[] = "Helvetica";

/* Signals */

extern void
_gtkextra_signal_emit(GtkObject *object, guint signal_id, ...);

enum
{
  ADD_DATA,
  CHANGED,
  UPDATE,
  MOVED,
  RESIZED,
  LAST_SIGNAL
};

enum
{
  TICK_LABEL,
  AXIS_CHANGED,
  LAST_AXIS_SIGNAL
};

enum
{
  ARG_PLOT_0,
  ARG_BOTTOM,
  ARG_TOP,
  ARG_LEFT,
  ARG_RIGHT,
  ARG_ALLOCATION_X,
  ARG_ALLOCATION_Y,
  ARG_ALLOCATION_WIDTH,
  ARG_ALLOCATION_HEIGHT,
  ARG_USE_PIXMAP,
  ARG_BG_PIXMAP,
  ARG_TRANSPARENT,
  ARG_MAGNIFICATION,
  ARG_CLIP_DATA,
  ARG_BG,
  ARG_GRIDS_ON_TOP,
  ARG_SHOW_X0,
  ARG_SHOW_Y0,
  ARG_X0_LINE,
  ARG_Y0_LINE,
  ARG_XMIN,
  ARG_XMAX,
  ARG_YMIN,
  ARG_YMAX,
  ARG_X,
  ARG_Y,
  ARG_WIDTH,
  ARG_HEIGHT,
  ARG_XSCALE,
  ARG_YSCALE,
  ARG_REFLECT_X,
  ARG_REFLECT_Y,
  ARG_BOTTOM_ALIGN,
  ARG_TOP_ALIGN,
  ARG_LEFT_ALIGN,
  ARG_RIGHT_ALIGN,
  ARG_LEGENDS_X,
  ARG_LEGENDS_Y,
  ARG_LEGENDS_WIDTH,
  ARG_LEGENDS_HEIGHT,
  ARG_LEGENDS_BORDER,
  ARG_LEGENDS_LINE_WIDTH,
  ARG_LEGENDS_BORDER_WIDTH,
  ARG_LEGENDS_SHADOW_WIDTH,
  ARG_LEGENDS_SHOW,
  ARG_LEGENDS_ATTR,
  ARG_LEGENDS_TRANSPARENT,
};

enum
{
  ARG_AXIS_0,
  ARG_VISIBLE,
  ARG_TITLE,
  ARG_TITLE_VISIBLE,
  ARG_ORIENTATION,
  ARG_LINE,
  ARG_MAJOR_GRID,
  ARG_MINOR_GRID,
  ARG_MAJOR_MASK,
  ARG_MINOR_MASK,
  ARG_TICKS_LENGTH,
  ARG_TICKS_WIDTH,
  ARG_CUSTOM_LABELS,
  ARG_LABELS_OFFSET,
  ARG_LABELS_PREFIX,
  ARG_LABELS_SUFFIX,
  ARG_SHOW_MAJOR_GRID,
  ARG_SHOW_MINOR_GRID,
  ARG_LABELS_ATTR,
  ARG_LABELS_PRECISION,
  ARG_LABELS_STYLE,
  ARG_LABELS_MASK,
  ARG_TICKS_MIN,
  ARG_TICKS_MAX,
  ARG_TICK_LABELS,
  ARG_SCALE,
  ARG_NMAJORTICKS,
  ARG_NMINORTICKS,
  ARG_NTICKS,
  ARG_STEP,
  ARG_NMINOR,
  ARG_APPLY_BREAK,
  ARG_BREAK_SCALE,
  ARG_BREAK_STEP,
  ARG_BREAK_NMINOR,
  ARG_BREAK_MIN,
  ARG_BREAK_MAX,
  ARG_BREAK_POSITION,
  ARG_SET_LIMITS,
  ARG_BEGIN,
  ARG_END
};

/* Private methods for ticks */
void gtk_plot_ticks_recalc		(GtkPlotAxis *ticks);
void gtk_plot_ticks_autoscale		(GtkPlotAxis *ticks, 
					 gdouble xmin, gdouble xmax, 
					 gint *precision);
gdouble gtk_plot_ticks_transform	(GtkPlotAxis *ticks, gdouble y);
gdouble gtk_plot_ticks_inverse		(GtkPlotAxis *ticks, gdouble x);
void gtk_plot_parse_label	        (GtkPlotAxis *axis,
					 gdouble val, 
					 gint precision, 
					 gint style,
                                         gchar *label);


static void gtk_plot_class_init 		(GtkPlotClass *klass);
static void gtk_plot_init 			(GtkPlot *plot);
static void gtk_plot_set_property             	(GObject *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_get_property             	(GObject *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_axis_class_init 		(GtkPlotAxisClass *klass);
static void gtk_plot_axis_init 			(GtkPlotAxis *axis);
static void gtk_plot_axis_set_property          (GObject *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_axis_get_property        	(GObject *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_destroy	 		(GtkObject *object);
static void gtk_plot_axis_destroy 		(GtkObject *object);
static void gtk_plot_real_set_pc                (GtkPlot *plot, GtkPlotPC *pc);
static void gtk_plot_real_set_drawable          (GtkPlot *plot, GdkDrawable *drawable);
static void gtk_plot_size_request 		(GtkWidget *widget, 
                                                 GtkRequisition *requisition);
static void gtk_plot_size_allocate 		(GtkWidget *widget, 
                                                 GtkAllocation *allocation);
static void gtk_plot_show_all 			(GtkWidget *widget); 
static void gtk_plot_draw_grids                 (GtkPlot *plot); 
static void gtk_plot_draw_axis			(GtkPlot *plot, 
					 	 GtkPlotAxis *axis, 
					 	 GtkPlotVector tick_direction);
static void gtk_plot_draw_labels		(GtkPlot *plot, 
						 GtkPlotAxis *axis, 
						 GtkPlotVector tick_direction); 
static void gtk_plot_draw_legends		(GtkWidget *widget);
static void gtk_plot_real_paint			(GtkWidget *widget); 
static void gtk_plot_paint_text			(GtkPlot *plot, 
                   				 gint x, gint y, 
                  				 GtkPlotText text);
static void gtk_plot_real_get_pixel		(GtkWidget *widget, 
						 gdouble xx, gdouble yy , 
						 gdouble *x, gdouble *y); 
static void gtk_plot_real_get_point		(GtkWidget *widget, 
						 gint x, gint y, 
					 	 gdouble *px, gdouble *py);
void   gtk_plot_remove_dimension		(GtkPlot *plot, 
						 const gchar *dimension);
inline gint roundint				(gdouble x);
static void update_datasets			(GtkPlot *plot, gboolean new_range);

static GtkWidgetClass *parent_class = NULL;
static guint plot_signals[LAST_SIGNAL] = {0};
static guint axis_signals[LAST_AXIS_SIGNAL] = {0};


GtkType
gtk_plot_get_type (void)
{
  static GtkType plot_type = 0;

  if (!plot_type)
    {
      GtkTypeInfo plot_info =
      {
	"GtkPlot",
	sizeof (GtkPlot),
	sizeof (GtkPlotClass),
	(GtkClassInitFunc) gtk_plot_class_init,
	(GtkObjectInitFunc) gtk_plot_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      plot_type = gtk_type_unique (GTK_TYPE_MISC, &plot_info);
    }
  return plot_type;
}

static void
gtk_plot_class_init (GtkPlotClass *klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkPlotClass *plot_class;
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  parent_class = gtk_type_class (gtk_widget_get_type ());

  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;
  plot_class = (GtkPlotClass *) klass;

  widget_class->show_all = gtk_plot_show_all;
  widget_class->size_request = gtk_plot_size_request;
  widget_class->size_allocate = gtk_plot_size_allocate;

  plot_signals[ADD_DATA] = 
    gtk_signal_new("add_data",
                   GTK_RUN_LAST,
                   GTK_CLASS_TYPE(object_class),
                   GTK_SIGNAL_OFFSET (GtkPlotClass, add_data),
                   gtkextra_BOOL__POINTER,
                   GTK_TYPE_BOOL, 1, GTK_TYPE_PLOT_DATA); 

  plot_signals[CHANGED] = 
    gtk_signal_new("changed",
                   GTK_RUN_LAST,
                   GTK_CLASS_TYPE(object_class),
                   GTK_SIGNAL_OFFSET (GtkPlotClass, changed),
                   gtkextra_VOID__VOID,
                   GTK_TYPE_NONE, 0); 

  plot_signals[UPDATE] = 
    gtk_signal_new("update",
                   GTK_RUN_LAST,
                   GTK_CLASS_TYPE(object_class),
                   GTK_SIGNAL_OFFSET (GtkPlotClass, update),
                   gtkextra_VOID__BOOL,
                   GTK_TYPE_NONE, 1, GTK_TYPE_BOOL); 

  plot_signals[MOVED] = 
    gtk_signal_new("moved",
                   GTK_RUN_LAST,
                   GTK_CLASS_TYPE(object_class),
                   GTK_SIGNAL_OFFSET (GtkPlotClass, moved),
                   gtkextra_BOOL__POINTER_POINTER,
                   GTK_TYPE_BOOL, 2, GTK_TYPE_POINTER, GTK_TYPE_POINTER); 

  plot_signals[RESIZED] = 
    gtk_signal_new("resized",
                   GTK_RUN_LAST,
                   GTK_CLASS_TYPE(object_class),
                   GTK_SIGNAL_OFFSET (GtkPlotClass, resized),
                   gtkextra_BOOL__POINTER_POINTER,
                   GTK_TYPE_BOOL, 2, GTK_TYPE_POINTER, GTK_TYPE_POINTER); 

  object_class->destroy = gtk_plot_destroy;
  gobject_class->set_property = gtk_plot_set_property;
  gobject_class->get_property = gtk_plot_get_property;

  klass->changed = NULL;
  klass->moved = NULL;
  klass->resized = NULL;

  plot_class->set_pc = gtk_plot_real_set_pc;
  plot_class->set_drawable = gtk_plot_real_set_drawable;
  plot_class->add_data = NULL;
  plot_class->update = update_datasets;
  plot_class->plot_paint = gtk_plot_real_paint;
  plot_class->draw_legends = gtk_plot_draw_legends;
  plot_class->get_point = gtk_plot_real_get_point;
  plot_class->get_pixel = gtk_plot_real_get_pixel;

  g_object_class_install_property(gobject_class,
                           ARG_BOTTOM,
  g_param_spec_object ("bottom_axis",
                           P_(""),
                           P_(""),
                           GTK_TYPE_PLOT_AXIS,
                           G_PARAM_READABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TOP,
  g_param_spec_object ("top_axis",
                           P_(""),
                           P_(""),
                           GTK_TYPE_PLOT_AXIS,
                           G_PARAM_READABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEFT,
  g_param_spec_object ("left_axis",
                           P_(""),
                           P_(""),
                           GTK_TYPE_PLOT_AXIS,
                           G_PARAM_READABLE));
  g_object_class_install_property(gobject_class,
                           ARG_RIGHT,
  g_param_spec_object ("right_axis",
                           P_(""),
                           P_(""),
                           GTK_TYPE_PLOT_AXIS,
                           G_PARAM_READABLE));
  g_object_class_install_property(gobject_class,
                           ARG_ALLOCATION_X,
  g_param_spec_int ("allocation_x",
                           P_(""),
                           P_(""),
                           -G_MAXINT,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_ALLOCATION_Y,
  g_param_spec_int ("allocation_y",
                           P_(""),
                           P_(""),
                           -G_MAXINT,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_ALLOCATION_WIDTH,
  g_param_spec_int ("allocation_width",
                           P_(""),
                           P_(""),
                           -G_MAXINT,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_ALLOCATION_HEIGHT,
  g_param_spec_int ("allocation_height",
                           P_(""),
                           P_(""),
                           -G_MAXINT,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_USE_PIXMAP,
  g_param_spec_boolean ("use_pixmap",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BG_PIXMAP,
  g_param_spec_pointer ("bg_pixmap",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TRANSPARENT,
  g_param_spec_boolean ("transparent",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_MAGNIFICATION,
  g_param_spec_double ("magnification",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_CLIP_DATA,
  g_param_spec_boolean ("clip_data",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BG,
  g_param_spec_pointer ("bg_color",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_GRIDS_ON_TOP,
  g_param_spec_boolean ("grids_on_top",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_SHOW_X0,
  g_param_spec_boolean ("show_x0",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_SHOW_Y0,
  g_param_spec_boolean ("show_y0",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_X0_LINE,
  g_param_spec_pointer ("x0_line",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_Y0_LINE,
  g_param_spec_pointer ("y0_line",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_XMIN,
  g_param_spec_double ("xmin",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_XMAX,
  g_param_spec_double ("xmax",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_YMIN,
  g_param_spec_double ("ymin",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_YMAX,
  g_param_spec_double ("ymax",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_X,
  g_param_spec_double ("x",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_Y,
  g_param_spec_double ("y",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_WIDTH,
  g_param_spec_double ("width",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_HEIGHT,
  g_param_spec_double ("height",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_XSCALE,
  g_param_spec_int ("xscale",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_YSCALE,
  g_param_spec_int ("yscale",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_REFLECT_X,
  g_param_spec_boolean ("reflect_x",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_REFLECT_Y,
  g_param_spec_boolean ("reflect_y",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BOTTOM_ALIGN,
  g_param_spec_double ("bottom_align",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TOP_ALIGN,
  g_param_spec_double ("top_align",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEFT_ALIGN,
  g_param_spec_double ("left_align",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_RIGHT_ALIGN,
  g_param_spec_double ("right_align",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_X,
  g_param_spec_double ("legends_x",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_Y,
  g_param_spec_double ("legends_y",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_WIDTH,
  g_param_spec_int ("legends_width",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_HEIGHT,
  g_param_spec_int ("legends_height",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_BORDER,
  g_param_spec_int ("legends_border",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_LINE_WIDTH,
  g_param_spec_int ("legends_line_width",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_BORDER_WIDTH,
  g_param_spec_int ("legends_border_width",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_SHADOW_WIDTH,
  g_param_spec_int ("legends_shadow_width",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_SHOW,
  g_param_spec_boolean ("legends_show",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_ATTR,
  g_param_spec_pointer ("legends_attr_text",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LEGENDS_TRANSPARENT,
  g_param_spec_boolean ("legends_transparent",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
}

GtkType
gtk_plot_axis_get_type (void)
{
  static GtkType axis_type = 0;

  if (!axis_type)
    {
      GtkTypeInfo axis_info =
      {
	"GtkPlotAxis",
	sizeof (GtkPlotAxis),
	sizeof (GtkPlotAxisClass),
	(GtkClassInitFunc) gtk_plot_axis_class_init,
	(GtkObjectInitFunc) gtk_plot_axis_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      axis_type = gtk_type_unique (GTK_TYPE_OBJECT, &axis_info);
    }
  return axis_type;
}


static void
gtk_plot_axis_class_init (GtkPlotAxisClass *klass)
{
  GtkObjectClass *object_class;
  GtkPlotAxisClass *axis_class;
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  object_class = (GtkObjectClass *) klass;
  axis_class = (GtkPlotAxisClass *) klass;

  axis_signals[TICK_LABEL] = 
    gtk_signal_new("tick_label",
                   GTK_RUN_LAST,
                   GTK_CLASS_TYPE(object_class),
                   GTK_SIGNAL_OFFSET (GtkPlotAxisClass, tick_label),
                   gtkextra_BOOL__POINTER_POINTER,
                   GTK_TYPE_BOOL, 2, GTK_TYPE_POINTER, GTK_TYPE_POINTER); 

  axis_signals[AXIS_CHANGED] = 
    gtk_signal_new("changed",
                   GTK_RUN_LAST,
                   GTK_CLASS_TYPE(object_class),
                   GTK_SIGNAL_OFFSET (GtkPlotAxisClass, changed),
                   gtkextra_VOID__VOID,
                   GTK_TYPE_NONE, 0); 

  object_class->destroy = gtk_plot_axis_destroy;
  gobject_class->set_property = gtk_plot_axis_set_property;
  gobject_class->get_property = gtk_plot_axis_get_property;

  axis_class->tick_label = NULL;

  g_object_class_install_property(gobject_class,
                           ARG_VISIBLE,
  g_param_spec_boolean ("visible",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TITLE,
  g_param_spec_pointer ("title_text",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TITLE_VISIBLE,
  g_param_spec_boolean ("title_visible",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_ORIENTATION,
  g_param_spec_enum ("orientation",
                           P_(""),
                           P_(""),
                           GTK_TYPE_ORIENTATION,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LINE,
  g_param_spec_pointer ("line",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_MAJOR_GRID,
  g_param_spec_pointer ("major_grid_line",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_MINOR_GRID,
  g_param_spec_pointer ("minor_grid_line",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_MAJOR_MASK,
  g_param_spec_int ("major_mask",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_MINOR_MASK,
  g_param_spec_int ("minor_mask",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TICKS_LENGTH,
  g_param_spec_int ("ticks_length",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TICKS_WIDTH,
  g_param_spec_double ("ticks_width",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_CUSTOM_LABELS,
  g_param_spec_boolean ("custom_labels",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TICK_LABELS,
  g_param_spec_object ("labels_array",
                           P_(""),
                           P_(""),
                           GTK_TYPE_PLOT_ARRAY,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LABELS_OFFSET,
  g_param_spec_int ("labels_offset",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LABELS_PREFIX,
  g_param_spec_string ("labels_prefix",
                           P_(""),
                           P_(""),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LABELS_SUFFIX,
  g_param_spec_string ("labels_suffix",
                           P_(""),
                           P_(""),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_SHOW_MAJOR_GRID,
  g_param_spec_boolean ("show_major_grid",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_SHOW_MINOR_GRID,
  g_param_spec_boolean ("show_minor_grid",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LABELS_ATTR,
  g_param_spec_pointer ("labels_text",
                           P_(""),
                           P_(""),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LABELS_PRECISION,
  g_param_spec_int ("labels_precision",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LABELS_STYLE,
  g_param_spec_int ("labels_style",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_LABELS_MASK,
  g_param_spec_int ("labels_mask",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TICKS_MIN,
  g_param_spec_double ("min",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_TICKS_MAX,
  g_param_spec_double ("max",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_SCALE,
  g_param_spec_int ("scale",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_NMAJORTICKS,
  g_param_spec_int ("nmajorticks",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_NMINORTICKS,
  g_param_spec_int ("nminorticks",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_NTICKS,
  g_param_spec_int ("nticks",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_STEP,
  g_param_spec_double ("step",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_NMINOR,
  g_param_spec_int ("nminor",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_APPLY_BREAK,
  g_param_spec_boolean ("apply_break",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BREAK_SCALE,
  g_param_spec_int ("break_scale",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BREAK_MIN,
  g_param_spec_double ("break_min",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BREAK_MAX,
  g_param_spec_double ("break_max",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BREAK_STEP,
  g_param_spec_double ("break_step",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BREAK_NMINOR,
  g_param_spec_int ("break_nminor",
                           P_(""),
                           P_(""),
                           0,G_MAXINT,0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BREAK_POSITION,
  g_param_spec_double ("break_position",
                           P_(""),
                           P_(""),
                           0,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_SET_LIMITS,
  g_param_spec_boolean ("set_limits",
                           P_(""),
                           P_(""),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_BEGIN,
  g_param_spec_double ("begin",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property(gobject_class,
                           ARG_END,
  g_param_spec_double ("end",
                           P_(""),
                           P_(""),
                           -G_MAXDOUBLE,G_MAXDOUBLE,0.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
}

static void
axis_changed(GtkPlotAxis *axis, GtkPlot *plot)
{
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

static void
gtk_plot_init (GtkPlot *plot)
{
  GtkWidget *widget;
  GTK_WIDGET_SET_FLAGS(plot, GTK_NO_WINDOW);

  widget = GTK_WIDGET(plot);
  gdk_color_black(gtk_widget_get_colormap(widget), &widget->style->black);
  gdk_color_white(gtk_widget_get_colormap(widget), &widget->style->white);

  plot->bg_pixmap = NULL;
  plot->transparent = FALSE;
  plot->clip_data = FALSE;
  plot->grids_on_top = FALSE;

  plot->reflect_x = FALSE;
  plot->reflect_y = FALSE;

  plot->magnification = 1.;

  plot->xmin = 0.;
  plot->xmax = 1.000000;
  plot->ymin = 0.;
  plot->ymax = 1.000000;

  plot->show_x0 = FALSE;
  plot->show_y0 = FALSE;

  plot->right = GTK_PLOT_AXIS(gtk_plot_axis_new(GTK_PLOT_AXIS_Y));  
  gtk_object_ref(GTK_OBJECT(plot->right));
  gtk_object_sink(GTK_OBJECT(plot->right));
  gtk_signal_connect(GTK_OBJECT(plot->right), "changed", 
                     GTK_SIGNAL_FUNC(axis_changed), plot);
  plot->left = GTK_PLOT_AXIS(gtk_plot_axis_new(GTK_PLOT_AXIS_Y));  
  gtk_object_ref(GTK_OBJECT(plot->left));
  gtk_object_sink(GTK_OBJECT(plot->left));
  gtk_signal_connect(GTK_OBJECT(plot->left), "changed", 
                     GTK_SIGNAL_FUNC(axis_changed), plot);
  plot->top = GTK_PLOT_AXIS(gtk_plot_axis_new(GTK_PLOT_AXIS_X));  
  gtk_object_ref(GTK_OBJECT(plot->top));
  gtk_object_sink(GTK_OBJECT(plot->top));
  gtk_signal_connect(GTK_OBJECT(plot->top), "changed", 
                     GTK_SIGNAL_FUNC(axis_changed), plot);
  plot->bottom = GTK_PLOT_AXIS(gtk_plot_axis_new(GTK_PLOT_AXIS_X));  
  gtk_object_ref(GTK_OBJECT(plot->bottom));
  gtk_object_sink(GTK_OBJECT(plot->bottom));
  gtk_signal_connect(GTK_OBJECT(plot->bottom), "changed", 
                     GTK_SIGNAL_FUNC(axis_changed), plot);

  plot->left->labels_attr.justification = GTK_JUSTIFY_RIGHT;
  plot->right->labels_attr.justification = GTK_JUSTIFY_LEFT;
  plot->right->title.angle = 270;

  gtk_plot_axis_ticks_recalc(plot->left);
  gtk_plot_axis_ticks_recalc(plot->right);
  gtk_plot_axis_ticks_recalc(plot->bottom);
  gtk_plot_axis_ticks_recalc(plot->top);

  plot->bottom_align = 0.;
  plot->top_align = 1.;
  plot->left_align = 0.;
  plot->right_align = 1.;

  plot->x0_line.line_style = GTK_PLOT_LINE_SOLID;
  plot->x0_line.cap_style = 0;
  plot->x0_line.join_style = 0;
  plot->x0_line.line_width = 0;
  plot->x0_line.color = widget->style->black; 

  plot->y0_line.line_style = GTK_PLOT_LINE_SOLID;
  plot->y0_line.line_width = 0;
  plot->y0_line.cap_style = 0;
  plot->y0_line.join_style = 0;
  plot->y0_line.color = widget->style->black; 

  plot->legends_x = .6;
  plot->legends_y = .1;
  plot->legends_width = 0;
  plot->legends_height = 0;
  plot->legends_line_width = 30;
  plot->legends_border_width = 1;
  plot->legends_shadow_width = 3;
  plot->legends_border = GTK_PLOT_BORDER_LINE;
  plot->show_legends =  TRUE;
  plot->legends_attr.text = NULL;
  plot->legends_attr.font = g_strdup(DEFAULT_FONT);
  plot->legends_attr.height = DEFAULT_FONT_HEIGHT;
  plot->legends_attr.fg = widget->style->black;
  plot->legends_attr.bg = widget->style->white;
  plot->legends_attr.transparent = FALSE;
  plot->legends_attr.border = 0;
  plot->legends_attr.border_width = 0;
  plot->legends_attr.shadow_width = 0;
  
  plot->background = widget->style->white;

  plot->xscale = GTK_PLOT_SCALE_LINEAR;
  plot->yscale = GTK_PLOT_SCALE_LINEAR;

  plot->data_sets = NULL;
  plot->text = NULL;

  gtk_psfont_init();
  plot->drawable = NULL;

  plot->pc = NULL;
  gtk_plot_set_pc(plot, NULL);
}

static void
gtk_plot_get_property (GObject      *object,
                         guint            prop_id,
                         GValue          *value,
                         GParamSpec      *pspec)
{
  GtkPlot *plot;

  plot = GTK_PLOT (object);

  switch(prop_id){
    case ARG_BOTTOM:
      g_value_set_object(value, GTK_OBJECT(plot->bottom));
      break;
    case ARG_TOP:
      g_value_set_object(value, GTK_OBJECT(plot->top));
      break;
    case ARG_LEFT:
      g_value_set_object(value, GTK_OBJECT(plot->left));
      break;
    case ARG_RIGHT:
      g_value_set_object(value, GTK_OBJECT(plot->right));
      break;
    case ARG_ALLOCATION_X:
      g_value_set_int(value, plot->internal_allocation.x);
      break;
    case ARG_ALLOCATION_Y:
      g_value_set_int(value, plot->internal_allocation.y);
      break;
    case ARG_ALLOCATION_WIDTH:
      g_value_set_int(value, plot->internal_allocation.width);
      break;
    case ARG_ALLOCATION_HEIGHT:
      g_value_set_int(value, plot->internal_allocation.height);
      break;
    case ARG_USE_PIXMAP:
      g_value_set_boolean(value, plot->use_pixmap);
      break;
    case ARG_BG_PIXMAP:
      g_value_set_pointer(value, plot->bg_pixmap);
      break;
    case ARG_TRANSPARENT:
      g_value_set_boolean(value, plot->transparent);
      break;
    case ARG_MAGNIFICATION:
      g_value_set_double(value, plot->magnification);
      break;
    case ARG_CLIP_DATA:
      g_value_set_boolean(value, plot->clip_data);
      break;
    case ARG_BG:
      g_value_set_pointer(value, &plot->background);
      break;
    case ARG_GRIDS_ON_TOP:
      g_value_set_boolean(value, plot->grids_on_top);
      break;
    case ARG_SHOW_X0:
      g_value_set_boolean(value, plot->show_x0);
      break;
    case ARG_SHOW_Y0:
      g_value_set_boolean(value, plot->show_y0);
      break;
    case ARG_X0_LINE:
      g_value_set_pointer(value, &plot->x0_line);
      break;
    case ARG_Y0_LINE:
      g_value_set_pointer(value, &plot->y0_line);
      break;
    case ARG_XMIN:
      g_value_set_double(value, plot->xmin);
      break;
    case ARG_XMAX:
      g_value_set_double(value, plot->xmax);
      break;
    case ARG_YMIN:
      g_value_set_double(value, plot->ymin);
      break;
    case ARG_YMAX:
      g_value_set_double(value, plot->ymax);
      break;
    case ARG_X:
      g_value_set_double(value, plot->x);
      break;
    case ARG_Y:
      g_value_set_double(value, plot->y);
      break;
    case ARG_WIDTH:
      g_value_set_double(value, plot->width);
      break;
    case ARG_HEIGHT:
      g_value_set_double(value, plot->height);
      break;
    case ARG_XSCALE:
      g_value_set_int(value, plot->xscale);
      break;
    case ARG_YSCALE:
      g_value_set_int(value, plot->yscale);
      break;
    case ARG_REFLECT_X:
      g_value_set_boolean(value, plot->reflect_x);
      break;
    case ARG_REFLECT_Y:
      g_value_set_boolean(value, plot->reflect_y);
      break;
    case ARG_BOTTOM_ALIGN:
      g_value_set_double(value, plot->bottom_align);
      break;
    case ARG_TOP_ALIGN:
      g_value_set_double(value, plot->top_align);
      break;
    case ARG_LEFT_ALIGN:
      g_value_set_double(value, plot->left_align);
      break;
    case ARG_RIGHT_ALIGN:
      g_value_set_double(value, plot->right_align);
      break;
    case ARG_LEGENDS_X:
      g_value_set_double(value, plot->legends_x);
      break;
    case ARG_LEGENDS_Y:
      g_value_set_double(value, plot->legends_y);
      break;
    case ARG_LEGENDS_WIDTH:
      g_value_set_int(value, plot->legends_width);
      break;
    case ARG_LEGENDS_HEIGHT:
      g_value_set_int(value, plot->legends_height);
      break;
    case ARG_LEGENDS_BORDER:
      g_value_set_int(value, plot->legends_border);
      break;
    case ARG_LEGENDS_LINE_WIDTH:
      g_value_set_int(value, plot->legends_line_width);
      break;
    case ARG_LEGENDS_BORDER_WIDTH:
      g_value_set_int(value, plot->legends_border_width);
      break;
    case ARG_LEGENDS_SHADOW_WIDTH:
      g_value_set_int(value, plot->legends_shadow_width);
      break;
    case ARG_LEGENDS_SHOW:
      g_value_set_boolean(value, plot->show_legends);
      break;
    case ARG_LEGENDS_ATTR:
      g_value_set_pointer(value, &plot->legends_attr);
      break;
    case ARG_LEGENDS_TRANSPARENT:
      g_value_set_boolean(value, plot->legends_attr.transparent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_plot_set_property (GObject      *object,
                         guint            prop_id,
                         const GValue          *value,
                         GParamSpec      *pspec)
{
  GtkPlot *plot;
  GtkPlotText *text = NULL;

  plot = GTK_PLOT (object);

  switch(prop_id){
    case ARG_ALLOCATION_X:
      plot->internal_allocation.x = g_value_get_int(value);
      break;
    case ARG_ALLOCATION_Y:
      plot->internal_allocation.y = g_value_get_int(value);
      break;
    case ARG_ALLOCATION_WIDTH:
      plot->internal_allocation.width = g_value_get_int(value);
      break;
    case ARG_ALLOCATION_HEIGHT:
      plot->internal_allocation.height = g_value_get_int(value);
      break;
    case ARG_USE_PIXMAP:
      plot->use_pixmap = g_value_get_boolean(value);
      break;
    case ARG_BG_PIXMAP:
      if(plot->bg_pixmap) gdk_pixmap_unref(plot->bg_pixmap);
      plot->bg_pixmap = (GdkPixmap *)g_value_get_pointer(value);
      if(plot->bg_pixmap) gdk_pixmap_ref(plot->bg_pixmap);
      break;
    case ARG_TRANSPARENT:
      plot->transparent = g_value_get_boolean(value);
      break;
    case ARG_MAGNIFICATION:
      plot->magnification = g_value_get_double(value);
      break;
    case ARG_CLIP_DATA:
      plot->clip_data = g_value_get_boolean(value);
      break;
    case ARG_BG:
      plot->background = *((GdkColor *)g_value_get_pointer(value));
      break;
    case ARG_GRIDS_ON_TOP:
      plot->grids_on_top = g_value_get_boolean(value);
      break;
    case ARG_SHOW_X0:
      plot->show_x0 = g_value_get_boolean(value);
      break;
    case ARG_SHOW_Y0:
      plot->show_y0 = g_value_get_boolean(value);
      break;
    case ARG_X0_LINE:
      plot->x0_line = *((GtkPlotLine *)g_value_get_pointer(value));
      break;
    case ARG_Y0_LINE:
      plot->y0_line = *((GtkPlotLine *)g_value_get_pointer(value));
      break;
    case ARG_XMIN:
      plot->xmin = g_value_get_double(value);
      break;
    case ARG_XMAX:
      plot->xmax = g_value_get_double(value);
      break;
    case ARG_YMIN:
      plot->ymin = g_value_get_double(value);
      break;
    case ARG_YMAX:
      plot->ymax = g_value_get_double(value);
      break;
    case ARG_X:
      plot->x = g_value_get_double(value);
      break;
    case ARG_Y:
      plot->y = g_value_get_double(value);
      break;
    case ARG_WIDTH:
      plot->width = g_value_get_double(value);
      break;
    case ARG_HEIGHT:
      plot->height = g_value_get_double(value);
      break;
    case ARG_XSCALE:
      plot->xscale = g_value_get_int(value);
      break;
    case ARG_YSCALE:
      plot->yscale = g_value_get_int(value);
      break;
    case ARG_REFLECT_X:
      plot->reflect_x = g_value_get_boolean(value);
      break;
    case ARG_REFLECT_Y:
      plot->reflect_y = g_value_get_boolean(value);
      break;
    case ARG_BOTTOM_ALIGN:
      plot->bottom_align = g_value_get_double(value);
      break;
    case ARG_TOP_ALIGN:
      plot->top_align = g_value_get_double(value);
      break;
    case ARG_LEFT_ALIGN:
      plot->left_align = g_value_get_double(value);
      break;
    case ARG_RIGHT_ALIGN:
      plot->right_align = g_value_get_double(value);
      break;
    case ARG_LEGENDS_X:
      plot->legends_x = g_value_get_double(value);
      break;
    case ARG_LEGENDS_Y:
      plot->legends_y = g_value_get_double(value);
      break;
    case ARG_LEGENDS_WIDTH:
      plot->legends_width = g_value_get_int(value);
      break;
    case ARG_LEGENDS_HEIGHT:
      plot->legends_height = g_value_get_int(value);
      break;
    case ARG_LEGENDS_BORDER:
      plot->legends_border = g_value_get_int(value);
      break;
    case ARG_LEGENDS_LINE_WIDTH:
      plot->legends_line_width = g_value_get_int(value);
      break;
    case ARG_LEGENDS_BORDER_WIDTH:
      plot->legends_border_width = g_value_get_int(value);
      break;
    case ARG_LEGENDS_SHADOW_WIDTH:
      plot->legends_shadow_width = g_value_get_int(value);
      break;
    case ARG_LEGENDS_SHOW:
      plot->show_legends = g_value_get_boolean(value);
      break;
    case ARG_LEGENDS_ATTR:
      text = (GtkPlotText *)g_value_get_pointer(value);
      gtk_plot_legends_set_attributes(plot,
                                      text->font,
                                      text->height,
                                      &text->fg,
                                      &text->bg);
      break;
    case ARG_LEGENDS_TRANSPARENT:
      plot->legends_attr.transparent = g_value_get_boolean(value);
      break;
  }
}

void
gtk_plot_set_pc(GtkPlot *plot, GtkPlotPC *pc)
{
  GTK_PLOT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(plot)))->set_pc(plot, pc);
}

static void
gtk_plot_real_set_pc(GtkPlot *plot, GtkPlotPC *pc)
{
  if(plot->pc)
    gtk_object_unref(GTK_OBJECT(plot->pc));

  if(!pc){
    plot->pc = GTK_PLOT_PC(gtk_plot_gdk_new(GTK_WIDGET(plot)));
    gtk_object_ref(GTK_OBJECT(plot->pc));
    gtk_object_sink(GTK_OBJECT(plot->pc));
  } else {
    plot->pc = pc;
    gtk_object_ref(GTK_OBJECT(plot->pc));
    gtk_object_sink(GTK_OBJECT(plot->pc));
  }
}

static void
gtk_plot_axis_init (GtkPlotAxis *axis)
{
  GdkColor black, white;

  gdk_color_black(gdk_colormap_get_system(), &black);
  gdk_color_white(gdk_colormap_get_system(), &white);

  axis->ticks.nmajorticks = 0;
  axis->ticks.nminorticks = 0;
  axis->ticks.values = NULL;
  axis->ticks.set_limits = FALSE;
  axis->ticks.begin = 0;
  axis->ticks.end = 0;
  axis->ticks.step = .100000000;
  axis->ticks.nminor = 1;
  axis->ticks.scale = GTK_PLOT_SCALE_LINEAR;
  axis->ticks.apply_break = FALSE;
  axis->ticks.break_min = 0.;
  axis->ticks.break_max = 0.;
  axis->ticks.break_step = .1;
  axis->ticks.break_nminor = 1;
  axis->ticks.break_scale = GTK_PLOT_SCALE_LINEAR;
  axis->ticks.break_position = 0.5; 

  axis->ticks.min = 0.0;
  axis->ticks.max = 1.0;
  axis->major_mask = GTK_PLOT_TICKS_IN;
  axis->minor_mask = GTK_PLOT_TICKS_IN;
  axis->ticks_length = 8;
  axis->ticks_width = 1;
  axis->labels_offset = 10;
  axis->orientation = GTK_PLOT_AXIS_X;
  axis->is_visible = TRUE;
  axis->custom_labels = FALSE;

  axis->line.line_style = GTK_PLOT_LINE_SOLID;
  axis->line.cap_style = 3;
  axis->line.join_style = 0;
  axis->line.line_width = 2;
  axis->line.color = black; 
  axis->labels_attr.text = NULL;
  axis->labels_attr.font = g_strdup(DEFAULT_FONT);
  axis->labels_attr.height = DEFAULT_FONT_HEIGHT;
  axis->labels_attr.fg = black;
  axis->labels_attr.bg = white;
  axis->labels_attr.transparent = TRUE;
  axis->labels_attr.angle = 0;
  axis->labels_attr.border = 0;
  axis->labels_attr.border_width = 0;
  axis->labels_attr.shadow_width = 0;
  axis->label_mask = GTK_PLOT_LABEL_OUT;
  axis->label_style = GTK_PLOT_LABEL_FLOAT;
  axis->label_precision = 1;
  axis->labels_attr.justification = GTK_JUSTIFY_CENTER;
  axis->labels_prefix = NULL;
  axis->labels_suffix = NULL;
  axis->title.angle = 0;
  axis->title.justification = GTK_JUSTIFY_CENTER;
  axis->title.font = g_strdup(DEFAULT_FONT);
  axis->title.height = DEFAULT_FONT_HEIGHT;
  axis->title.fg = black;
  axis->title.bg = white;
  axis->title.transparent = TRUE;
  axis->title.text = g_strdup("Title");
  axis->title.border = 0;
  axis->title.border_width = 0;
  axis->title.shadow_width = 0;
  axis->title_visible = TRUE;

  axis->direction.x = 1.;
  axis->direction.y = 0.;
  axis->direction.z = 0.;

  axis->show_major_grid = FALSE;
  axis->show_minor_grid = FALSE;

  axis->major_grid.line_style = GTK_PLOT_LINE_SOLID;
  axis->major_grid.cap_style = 0;
  axis->major_grid.join_style = 0;
  axis->major_grid.line_width = 0;
  axis->major_grid.color = black; 

  axis->minor_grid.line_style = GTK_PLOT_LINE_DOTTED;
  axis->minor_grid.cap_style = 0;
  axis->minor_grid.join_style = 0;
  axis->minor_grid.line_width = 0;
  axis->minor_grid.color = black;

  axis->tick_labels = NULL;
} 

static void
gtk_plot_axis_set_property (GObject      *object,
                         guint            prop_id,
                         const GValue          *value,
                         GParamSpec      *pspec)
{
  GtkPlotAxis *axis;
  GtkPlotText *text = NULL;

  axis = GTK_PLOT_AXIS (object);
  switch(prop_id){
    case ARG_VISIBLE:
      axis->is_visible = g_value_get_boolean(value);
      break;
    case ARG_TITLE:
      text = (GtkPlotText *)g_value_get_pointer(value);
      if(axis->title.text) g_free(axis->title.text);
      if(axis->title.font) g_free(axis->title.font);
      axis->title = *text;
      axis->title.text = g_strdup(text->text);
      axis->title.font = g_strdup(text->font);
      break;
    case ARG_TITLE_VISIBLE:
      axis->title_visible = g_value_get_boolean(value);
      break;
    case ARG_ORIENTATION:
      axis->orientation = g_value_get_enum(value);
      break;
    case ARG_LINE:
      axis->line = *((GtkPlotLine *)g_value_get_pointer(value));
      break;
    case ARG_MAJOR_GRID:
      axis->major_grid = *((GtkPlotLine *)g_value_get_pointer(value));
      break;
    case ARG_MINOR_GRID:
      axis->minor_grid = *((GtkPlotLine *)g_value_get_pointer(value));
      break;
    case ARG_MAJOR_MASK:
      axis->major_mask = g_value_get_int(value);
      break;
    case ARG_MINOR_MASK:
      axis->minor_mask = g_value_get_int(value);
      break;
    case ARG_TICKS_LENGTH:
      axis->ticks_length = g_value_get_int(value);
      break;
    case ARG_TICKS_WIDTH:
      axis->ticks_width = g_value_get_double(value);
      break;
    case ARG_CUSTOM_LABELS:
      axis->custom_labels = g_value_get_boolean(value);
      break;
    case ARG_LABELS_OFFSET:
      axis->labels_offset = g_value_get_int(value);
      break;
    case ARG_LABELS_PREFIX:
      if(axis->labels_prefix) g_free(axis->labels_prefix);
      axis->labels_prefix = g_strdup(g_value_get_string(value));
      break;
    case ARG_LABELS_SUFFIX:
      if(axis->labels_suffix) g_free(axis->labels_suffix);
      axis->labels_suffix = g_strdup(g_value_get_string(value));
      break;
    case ARG_SHOW_MAJOR_GRID:
      axis->show_major_grid = g_value_get_boolean(value);
      break;
    case ARG_SHOW_MINOR_GRID:
      axis->show_minor_grid = g_value_get_boolean(value);
      break;
    case ARG_LABELS_ATTR:
      text = (GtkPlotText *)g_value_get_pointer(value);
      if(axis->labels_attr.text) g_free(axis->labels_attr.text);
      if(axis->labels_attr.font) g_free(axis->labels_attr.font);
      axis->labels_attr = *text;
      axis->labels_attr.text = g_strdup(text->text);
      axis->labels_attr.font = g_strdup(text->font);
      break;
    case ARG_LABELS_PRECISION:
      axis->label_precision = g_value_get_int(value);
      break;
    case ARG_LABELS_STYLE:
      axis->label_style = g_value_get_int(value);
      break;
    case ARG_LABELS_MASK:
      axis->label_mask = g_value_get_int(value);
      break;
    case ARG_TICKS_MIN:
      axis->ticks.min = g_value_get_double(value);
      break;
    case ARG_TICKS_MAX:
      axis->ticks.max = g_value_get_double(value);
      break;
    case ARG_SCALE:
      axis->ticks.scale = g_value_get_int(value);
      break;
    case ARG_NMAJORTICKS:
      axis->ticks.nmajorticks = g_value_get_int(value);
      break;
    case ARG_NMINORTICKS:
      axis->ticks.nminorticks = g_value_get_int(value);
      break;
    case ARG_NTICKS:
      axis->ticks.nticks = g_value_get_int(value);
      break;
    case ARG_STEP:
      axis->ticks.step = g_value_get_double(value);
      break;
    case ARG_NMINOR:
      axis->ticks.nminor = g_value_get_int(value);
      break;
    case ARG_APPLY_BREAK:
      axis->ticks.apply_break = g_value_get_boolean(value);
      break;
    case ARG_BREAK_SCALE:
      axis->ticks.break_scale = g_value_get_int(value);
      break;
    case ARG_BREAK_STEP:
      axis->ticks.break_step = g_value_get_double(value);
      break;
    case ARG_BREAK_NMINOR:
      axis->ticks.break_nminor = g_value_get_int(value);
      break;
    case ARG_BREAK_MIN:
      axis->ticks.break_min = g_value_get_double(value);
      break;
    case ARG_BREAK_MAX:
      axis->ticks.break_max = g_value_get_double(value);
      break;
    case ARG_BREAK_POSITION:
      axis->ticks.break_position = g_value_get_double(value);
      break;
    case ARG_SET_LIMITS:
      axis->ticks.set_limits = g_value_get_boolean(value);
      break;
    case ARG_BEGIN:
      axis->ticks.begin = g_value_get_double(value);
      break;
    case ARG_END:
      axis->ticks.end = g_value_get_double(value);
      break;
    case ARG_TICK_LABELS:
      if(g_value_get_object(value))
        gtk_plot_axis_set_tick_labels(axis, GTK_PLOT_ARRAY(g_value_get_object(value)));
      else
        gtk_plot_axis_set_tick_labels(axis, NULL);
      break;
  }
}

static void
gtk_plot_axis_get_property (GObject      *object,
                         guint            prop_id,
                         GValue          *value,
                         GParamSpec      *pspec)
{
  GtkPlotAxis *axis;

  axis = GTK_PLOT_AXIS (object);
  switch(prop_id){
    case ARG_VISIBLE:
      g_value_set_boolean(value, axis->is_visible);
      break;
    case ARG_TITLE:
      g_value_set_pointer(value, &axis->title);
      break;
    case ARG_TITLE_VISIBLE:
      g_value_set_boolean(value, axis->title_visible);
      break;
    case ARG_ORIENTATION:
      g_value_set_enum(value, axis->orientation);
      break;
    case ARG_LINE:
      g_value_set_pointer(value, &axis->line);
      break;
    case ARG_MAJOR_GRID:
      g_value_set_pointer(value, &axis->major_grid);
      break;
    case ARG_MINOR_GRID:
      g_value_set_pointer(value, &axis->minor_grid);
      break;
    case ARG_MAJOR_MASK:
      g_value_set_int(value, axis->major_mask);
      break;
    case ARG_MINOR_MASK:
      g_value_set_int(value, axis->minor_mask);
      break;
    case ARG_TICKS_LENGTH:
      g_value_set_int(value, axis->ticks_length);
      break;
    case ARG_TICKS_WIDTH:
      g_value_set_double(value, axis->ticks_width);
      break;
    case ARG_CUSTOM_LABELS:
      g_value_set_boolean(value, axis->custom_labels);
      break;
    case ARG_LABELS_OFFSET:
      g_value_set_int(value, axis->labels_offset);
      break;
    case ARG_LABELS_PREFIX:
      g_value_set_string(value, axis->labels_prefix);
      break;
    case ARG_LABELS_SUFFIX:
      g_value_set_string(value, axis->labels_suffix);
      break;
    case ARG_SHOW_MAJOR_GRID:
      g_value_set_boolean(value, axis->show_major_grid);
      break;
    case ARG_SHOW_MINOR_GRID:
      g_value_set_boolean(value, axis->show_minor_grid);
      break;
    case ARG_LABELS_ATTR:
      g_value_set_pointer(value, &axis->labels_attr);
      break;
    case ARG_LABELS_PRECISION:
      g_value_set_int(value, axis->label_precision);
      break;
    case ARG_LABELS_STYLE:
      g_value_set_int(value, axis->label_style);
      break;
    case ARG_LABELS_MASK:
      g_value_set_int(value, axis->label_mask);
      break;
    case ARG_TICKS_MIN:
      g_value_set_double(value, axis->ticks.min);
      break;
    case ARG_TICKS_MAX:
      g_value_set_double(value, axis->ticks.max);
      break;
    case ARG_SCALE:
      g_value_set_int(value, axis->ticks.scale);
      break;
    case ARG_NMAJORTICKS:
      g_value_set_int(value, axis->ticks.nmajorticks);
      break;
    case ARG_NMINORTICKS:
      g_value_set_int(value, axis->ticks.nminorticks);
      break;
    case ARG_NTICKS:
      g_value_set_int(value, axis->ticks.nticks);
      break;
    case ARG_STEP:
      g_value_set_double(value, axis->ticks.step);
      break;
    case ARG_NMINOR:
      g_value_set_int(value, axis->ticks.nminor);
      break;
    case ARG_APPLY_BREAK:
      g_value_set_boolean(value, axis->ticks.apply_break);
      break;
    case ARG_BREAK_SCALE:
      g_value_set_int(value, axis->ticks.break_scale);
      break;
    case ARG_BREAK_STEP:
      g_value_set_double(value, axis->ticks.break_step);
      break;
    case ARG_BREAK_NMINOR:
      g_value_set_int(value, axis->ticks.break_nminor);
      break;
    case ARG_BREAK_MIN:
      g_value_set_double(value, axis->ticks.break_min);
      break;
    case ARG_BREAK_MAX:
      g_value_set_double(value, axis->ticks.break_max);
      break;
    case ARG_BREAK_POSITION:
      g_value_set_double(value, axis->ticks.break_position);
      break;
    case ARG_SET_LIMITS:
      g_value_set_boolean(value, axis->ticks.set_limits);
      break;
    case ARG_BEGIN:
      g_value_set_double(value, axis->ticks.begin);
      break;
    case ARG_END:
      g_value_set_double(value, axis->ticks.end);
      break;
    case ARG_TICK_LABELS:
      if(axis->tick_labels)
        g_value_set_object(value, GTK_OBJECT(axis->tick_labels));
      else
        g_value_set_object(value, NULL);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_plot_destroy (GtkObject *object)
{
  GtkPlot *plot;
  GList *list;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GTK_IS_PLOT (object));

  plot = GTK_PLOT (object);

  gtk_object_unref(GTK_OBJECT(plot->top));
  gtk_object_unref(GTK_OBJECT(plot->bottom));
  gtk_object_unref(GTK_OBJECT(plot->left));
  gtk_object_unref(GTK_OBJECT(plot->right));

  if(plot->legends_attr.font) g_free (plot->legends_attr.font);

  plot->legends_attr.font = NULL;

  list = plot->text;
  while(list){
    GtkPlotText *text;

    text = (GtkPlotText *) list->data;
    if(text->text) g_free(text->text);
    if(text->font) g_free(text->font);

    g_free(text);
    plot->text = g_list_remove_link(plot->text, list);
    g_list_free_1(list);
    list = plot->text;
  }
  plot->text = NULL;


  list = plot->data_sets;
  while(list){
    gtk_widget_unref(GTK_WIDGET(list->data));

    plot->data_sets = g_list_remove_link(plot->data_sets, list);
    g_list_free_1(list);
    list = plot->data_sets;
  }
  plot->data_sets = NULL;

  if ( GTK_OBJECT_CLASS (parent_class)->destroy )
    (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);

  if(plot->pc)
    gtk_object_unref(GTK_OBJECT(plot->pc));
  plot->pc = NULL;

  gtk_psfont_unref();
}

static void
gtk_plot_axis_destroy (GtkObject *object)
{
  GtkPlotAxis *axis;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GTK_IS_PLOT_AXIS (object));

  axis = GTK_PLOT_AXIS (object);

  if(axis->labels_attr.font) g_free (axis->labels_attr.font);
  if(axis->title.font) g_free (axis->title.font);
  if(axis->title.text) g_free (axis->title.text);

  axis->labels_attr.font = NULL;
  axis->title.font = NULL;
  axis->title.text = NULL;

  if(axis->labels_prefix) g_free(axis->labels_prefix);
  if(axis->labels_suffix) g_free(axis->labels_suffix);

  if(axis->ticks.values){
    g_free (axis->ticks.values);
    axis->ticks.values = NULL;
  }

  if(axis->tick_labels) g_object_unref(G_OBJECT(axis->tick_labels));
  axis->tick_labels = NULL;
}

static void
gtk_plot_show_all (GtkWidget *widget)
{
  GtkPlot *plot;
  GList *list;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_PLOT (widget));

  plot = GTK_PLOT (widget);

  list = plot->data_sets;
  while(list)
    {
      if (GTK_IS_WIDGET (list->data))
      gtk_widget_show_all (GTK_WIDGET (list->data));
      list = list->next;
    }
  
  gtk_widget_show (widget);
}

void
gtk_plot_paint (GtkPlot *plot)
{
  if(!plot->drawable) return;

  gtk_plot_pc_init(plot->pc);
  GTK_PLOT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(plot)))->plot_paint(GTK_WIDGET(plot)); 
  gtk_plot_pc_leave(plot->pc);
}

static void
gtk_plot_real_paint (GtkWidget *widget)
{
  GtkPlot *plot;
  GtkPlotText *child_text;
  GtkStyle *style;
  GdkPixmap *pixmap;
  GList *dataset;
  GList *text;
  gint width, height;
  gint xoffset, yoffset ;

  plot = GTK_PLOT(widget);

  xoffset = plot->internal_allocation.x;
  yoffset = plot->internal_allocation.y;
  width = plot->internal_allocation.width;
  height = plot->internal_allocation.height;

  style = gtk_widget_get_style(widget);

  pixmap = plot->drawable;

  gtk_plot_pc_gsave(plot->pc);
  gtk_plot_pc_set_color(plot->pc, &plot->background);

  if(!gtk_plot_is_transparent(plot) && !plot->bg_pixmap)
    gtk_plot_pc_draw_rectangle (plot->pc, TRUE,
  		               xoffset, yoffset,
		               width , height);

  if(!gtk_plot_is_transparent(plot) && plot->bg_pixmap){
    gint pwidth, pheight;
    gdouble scale_x, scale_y;

    gdk_window_get_size(plot->bg_pixmap, &pwidth, &pheight);

    scale_x = (gdouble)width / (gdouble)pwidth;
    scale_y = (gdouble)height / (gdouble)pheight;

    gtk_plot_pc_draw_pixmap (plot->pc, plot->bg_pixmap, NULL,
                             0, 0,
  		             xoffset, yoffset,
		             pwidth , pheight, scale_x, scale_y);
  }

  /* draw frame to guide the eyes*/
/*
    gtk_plot_pc_draw_rectangle (plot->pc, FALSE,
  		                xoffset, yoffset,
		                width , height);
*/

  /* draw the ticks & grid lines */

  gtk_plot_axis_ticks_recalc(plot->left);
  gtk_plot_axis_ticks_recalc(plot->right);
  gtk_plot_axis_ticks_recalc(plot->bottom);
  gtk_plot_axis_ticks_recalc(plot->top);

  if(!plot->grids_on_top)
    gtk_plot_draw_grids(plot);
  else {
    dataset = plot->data_sets;
    while(dataset)
     {
       if(GTK_IS_PLOT_DATA(dataset->data))
         gtk_plot_data_paint(GTK_PLOT_DATA(dataset->data));
       dataset = dataset->next;
     }
  }

  if(plot->bottom->is_visible)
    {
      GtkPlotVector tick;
      gboolean reflect;

      tick.x = 0.;
      tick.y = -1.;
      plot->bottom->origin.x = plot->xmin;
      plot->bottom->origin.y = plot->ymin*(1.0-plot->bottom_align) + 
                               plot->ymax*plot->bottom_align;
      reflect = plot->reflect_y; 
      plot->reflect_y = FALSE;
      gtk_plot_draw_axis(plot, plot->bottom, tick); 
      gtk_plot_draw_labels(plot, plot->bottom, tick); 
      plot->reflect_y = reflect;
    }

  if(plot->top->is_visible)
    {
      GtkPlotVector tick;
      gboolean reflect;

      tick.x = 0.;
      tick.y = 1.;
      plot->top->origin.x = plot->xmin;
      plot->top->origin.y = plot->ymin*(1.0-plot->top_align) + 
                            plot->ymax*plot->top_align;
      reflect = plot->reflect_y; 
      plot->reflect_y = FALSE;
      gtk_plot_draw_axis(plot, plot->top, tick);
      gtk_plot_draw_labels(plot, plot->top, tick);
      plot->reflect_y = reflect;
    }

  if(plot->left->is_visible)
    {
      GtkPlotVector tick;
      gboolean reflect;

      tick.x = 1.;
      tick.y = 0.;
      plot->left->origin.x = plot->xmin*(1.0-plot->left_align) +
                             plot->xmax*plot->left_align;
      plot->left->origin.y = plot->ymin;
      reflect = plot->reflect_x; 
      plot->reflect_x = FALSE;
      gtk_plot_draw_axis(plot, plot->left, tick);
      gtk_plot_draw_labels(plot, plot->left, tick);
      plot->reflect_x = reflect;
    }

  if(plot->right->is_visible)
    {
      GtkPlotVector tick;
      gboolean reflect;

      tick.x = -1.;
      tick.y = 0.;
      plot->right->origin.x = plot->xmin*(1.0-plot->right_align) +
                              plot->xmax*plot->right_align;
      plot->right->origin.y = plot->ymin;
      reflect = plot->reflect_x; 
      plot->reflect_x = FALSE;
      gtk_plot_draw_axis(plot, plot->right, tick);
      gtk_plot_draw_labels(plot, plot->right, tick);
      plot->reflect_x = reflect;
    }

  if(plot->grids_on_top)
    gtk_plot_draw_grids(plot);
  else {
    dataset = plot->data_sets;
    while(dataset)
     {
       if(GTK_IS_PLOT_DATA(dataset->data))
         gtk_plot_data_paint(GTK_PLOT_DATA(dataset->data));
       dataset = dataset->next;
     }
  }


  text = plot->text;
  while(text)
   {
     child_text = (GtkPlotText *) text->data;  
     gtk_plot_draw_text(plot, *child_text);
     text = text->next;
   }

  GTK_PLOT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(plot)))->draw_legends(GTK_WIDGET(plot));

  gtk_plot_pc_grestore(plot->pc);
}

void 
gtk_plot_refresh (GtkPlot *plot, GdkRectangle *drawing_area)
{
  GtkWidget *widget;
  GdkPixmap *pixmap;
  GdkRectangle area;

  widget = GTK_WIDGET(plot);
  if(!GTK_WIDGET_VISIBLE(widget)) return;

  if(!plot->drawable) return;
  pixmap = plot->drawable;

  if(drawing_area == NULL){
     area.x = widget->allocation.x;
     area.y = widget->allocation.y;
     area.width = widget->allocation.width;
     area.height = widget->allocation.height;
  } else {
     area = *drawing_area;
  }

  gdk_draw_pixmap(widget->window,
                  widget->style->fg_gc[GTK_STATE_NORMAL],
                  pixmap,
                  area.x, 
                  area.y, 
                  widget->allocation.x, 
                  widget->allocation.y, 
                  widget->allocation.width, 
                  widget->allocation.height);  
  
}

static void
gtk_plot_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
  GtkPlot *plot;

  plot = GTK_PLOT(widget);

  requisition->width =  DEFAULT_WIDTH;
  requisition->height =  DEFAULT_HEIGHT;
}

static void
gtk_plot_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
  GtkPlot *plot;

  plot = GTK_PLOT(widget);

  widget->allocation = *allocation;

  plot->internal_allocation.x = GTK_WIDGET(plot)->allocation.x + roundint(plot->x * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.y = GTK_WIDGET(plot)->allocation.y + roundint(plot->y * GTK_WIDGET(plot)->allocation.height);
  plot->internal_allocation.width = roundint(plot->width * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.height = roundint(plot->height * GTK_WIDGET(plot)->allocation.height);

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], FALSE);
}

GtkObject*
gtk_plot_axis_new (GtkPlotOrientation orientation)
{
  GtkObject *axis;

  axis = gtk_type_new (gtk_plot_axis_get_type ());

  gtk_plot_axis_construct(GTK_PLOT_AXIS(axis), orientation);

  return GTK_OBJECT (axis);
}

void
gtk_plot_axis_construct(GtkPlotAxis *axis, GtkPlotOrientation orientation)
{
  axis->orientation = orientation;

  axis->title.border = GTK_PLOT_BORDER_NONE;
  axis->title.border_width = 0;
  axis->title.border_space = 2;
  axis->title.shadow_width = 3;
  axis->ticks.values = NULL;

  axis->ticks_transform = gtk_plot_ticks_transform;
  axis->ticks_inverse = gtk_plot_ticks_inverse;
  axis->ticks_recalc = gtk_plot_ticks_recalc;
  axis->ticks_autoscale = gtk_plot_ticks_autoscale;
  axis->parse_label = gtk_plot_parse_label;

  switch(orientation){
   case GTK_PLOT_AXIS_X:
     axis->direction.x = 1.0;
     axis->direction.y = 0.0;
     axis->direction.z = 0.0;
     g_free(axis->title.text);
     axis->title.text = g_strdup("X Title");
     axis->title.angle = 0;
     break;
   case GTK_PLOT_AXIS_Y:
     axis->direction.x = 0.0;
     axis->direction.y = -1.0;
     axis->direction.z = 0.0;
     g_free(axis->title.text);
     axis->title.text = g_strdup("Y Title");
     axis->title.angle = 90;
     break;
   case GTK_PLOT_AXIS_Z:
     axis->direction.x = 0.0;
     axis->direction.y = 0.0;
     axis->direction.z = 1.0;
     g_free(axis->title.text);
     axis->title.text = g_strdup("Z Title");
     axis->title.angle = 0;
     break;
  }
}

GtkWidget*
gtk_plot_new (GdkDrawable *drawable)
{
  GtkPlot *plot;

  plot = gtk_type_new (gtk_plot_get_type ());

  gtk_plot_construct(GTK_PLOT(plot), drawable);

  return GTK_WIDGET (plot);
}

void
gtk_plot_construct(GtkPlot *plot, GdkDrawable *drawable)
{
  gtk_plot_set_drawable(plot, drawable);

  plot->x = .15;
  plot->y = .1;
  plot->width = .6;
  plot->height = .6;

  plot->internal_allocation.x = GTK_WIDGET(plot)->allocation.x + roundint(plot->x * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.y = GTK_WIDGET(plot)->allocation.y + roundint(plot->y * GTK_WIDGET(plot)->allocation.height);
  plot->internal_allocation.width = roundint(plot->width * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.height = roundint(plot->height * GTK_WIDGET(plot)->allocation.height);

  plot->left->title.x = plot->x;  
  plot->left->title.y = plot->y + plot->height / 2.;
  plot->right->title.x = plot->x + plot->width;  
  plot->right->title.y = plot->y + plot->height / 2.;
  plot->top->title.x = plot->x + plot->width / 2.;  
  plot->top->title.y = plot->y;
  plot->bottom->title.x = plot->x + plot->width / 2.;  
  plot->bottom->title.y = plot->y + plot->height;

  plot->left->title.x -= 45. / (gdouble)DEFAULT_WIDTH;  
  plot->right->title.x += 45. / (gdouble)DEFAULT_WIDTH;  
  plot->top->title.y -= 35. / (gdouble)DEFAULT_HEIGHT;
  plot->bottom->title.y += 35. / (gdouble)DEFAULT_HEIGHT;
}

GtkWidget*
gtk_plot_new_with_size (GdkDrawable *drawable, gdouble width, gdouble height)
{
  GtkWidget *plot; 

  plot = gtk_type_new (gtk_plot_get_type ());

  gtk_plot_construct_with_size(GTK_PLOT(plot), drawable, width, height);

  return(plot);
}

void
gtk_plot_construct_with_size (GtkPlot *plot,
			      GdkDrawable *drawable, 
                              gdouble width, gdouble height)
{
  gtk_plot_construct(plot, drawable);

  gtk_plot_resize (GTK_PLOT(plot), width, height);
}

void
gtk_plot_set_drawable (GtkPlot *plot, GdkDrawable *drawable)
{
  GTK_PLOT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(plot)))->set_drawable(plot, drawable);
}

static void
gtk_plot_real_set_drawable (GtkPlot *plot, GdkDrawable *drawable)
{
  plot->drawable = drawable;

  if(GTK_IS_PLOT_GDK(plot->pc))
    gtk_plot_gdk_set_drawable(GTK_PLOT_GDK(plot->pc), drawable);
}

GdkDrawable *
gtk_plot_get_drawable (GtkPlot *plot)
{
  return(plot->drawable);
}

void
gtk_plot_set_background_pixmap (GtkPlot *plot, GdkPixmap *pixmap)
{
  if(plot->bg_pixmap) gdk_pixmap_unref(plot->bg_pixmap);

  plot->bg_pixmap = pixmap;
  if(pixmap) gdk_pixmap_ref(pixmap);
}

void
gtk_plot_set_transparent (GtkPlot *plot, gboolean transparent)
{
  plot->transparent = transparent;
}

gboolean
gtk_plot_is_transparent (GtkPlot *plot)
{
  return(plot->transparent);
}

static void
gtk_plot_draw_grids(GtkPlot *plot)
{
  GtkWidget *widget;
  GdkRectangle clip_area;
  gdouble width, height;
  gdouble xp, yp;
  gdouble x1, y1, x2, y2;
  gdouble xx, yy;
  gdouble x_tick;
  gint ntick;

  widget = GTK_WIDGET(plot);

  xp = plot->internal_allocation.x;
  yp = plot->internal_allocation.y;
  width = plot->internal_allocation.width;
  height = plot->internal_allocation.height;
 
  clip_area.x = xp;
  clip_area.y = yp;
  clip_area.width = width;
  clip_area.height = height;

  gtk_plot_pc_clip(plot->pc, &clip_area);
 
  if(plot->show_x0)
    {
          if(plot->xmin <= 0. && plot->xmax >= 0.)
            {
              gtk_plot_get_pixel(plot, 0., plot->ymin, &x1, &y1);
              gtk_plot_get_pixel(plot, 0., plot->ymax, &x2, &y2);
              gtk_plot_draw_line(plot, plot->x0_line,
                                 x1, y1, x2, y2); 
            }
    }

  if(plot->show_y0)
    {
          if(plot->ymin <= 0. && plot->ymax >= 0.)
            {
              gtk_plot_get_pixel(plot, plot->xmin, 0., &x1, &y1);
              gtk_plot_get_pixel(plot, plot->xmax, 0., &x2, &y2);
              gtk_plot_draw_line(plot, plot->y0_line,
                                 x1, y1, x2, y2); 
            }
    }

  if(plot->bottom->show_minor_grid)
    {
          for(ntick = 0; ntick < plot->bottom->ticks.nticks; ntick++){
            if(!plot->bottom->ticks.values[ntick].minor) continue; 
            if(plot->bottom->ticks.values[ntick].value >= plot->bottom->ticks.min){
              x_tick = plot->bottom->ticks.values[ntick].value;
              xx = x_tick; 
              yy = plot->ymin;
              gtk_plot_get_pixel(plot, xx, yy, &x1, &y1);
              xx = x_tick; 
              yy = plot->ymax;
              gtk_plot_get_pixel(plot, xx, yy, &x2, &y2);
              gtk_plot_draw_line(plot, plot->bottom->minor_grid,
                                 x1, y1, x2, y2);
            }
          }
    }

  if(plot->bottom->show_major_grid)
    {
          for(ntick = 0; ntick < plot->bottom->ticks.nticks; ntick++){
            if(plot->bottom->ticks.values[ntick].minor) continue;
            if(plot->bottom->ticks.values[ntick].value >= plot->bottom->ticks.min){
              x_tick = plot->bottom->ticks.values[ntick].value;
              xx = x_tick; 
              yy = plot->ymin;
              gtk_plot_get_pixel(plot, xx, yy, &x1, &y1);
              xx = x_tick; 
              yy = plot->ymax;
              gtk_plot_get_pixel(plot, xx, yy, &x2, &y2);
              gtk_plot_draw_line(plot, plot->bottom->major_grid,
                                 x1, y1, x2, y2); 
           }
          }
    }

  if(plot->left->show_minor_grid)
    {
          for(ntick = 0; ntick < plot->left->ticks.nticks; ntick++){
            if(!plot->left->ticks.values[ntick].minor) continue;
            if(plot->left->ticks.values[ntick].value >= plot->left->ticks.min){
              x_tick = plot->left->ticks.values[ntick].value;
              xx = plot->xmin; 
              yy = x_tick;
              gtk_plot_get_pixel(plot, xx, yy, &x1, &y1);
              xx = plot->xmax; 
              yy = x_tick;
              gtk_plot_get_pixel(plot, xx, yy, &x2, &y2);
              gtk_plot_draw_line(plot, plot->left->minor_grid,
                                 x1, y1, x2, y2); 
            }
          }
    }

  if(plot->left->show_major_grid)
    {
          for(ntick = 0; ntick < plot->left->ticks.nticks; ntick++){
            if(plot->left->ticks.values[ntick].minor) continue;
            if(plot->left->ticks.values[ntick].value >= plot->left->ticks.min){
              x_tick = plot->left->ticks.values[ntick].value;
              xx = plot->xmin; 
              yy = x_tick;
              gtk_plot_get_pixel(plot, xx, yy, &x1, &y1);
              xx = plot->xmax; 
              yy = x_tick;
              gtk_plot_get_pixel(plot, xx, yy, &x2, &y2);
              gtk_plot_draw_line(plot, plot->left->major_grid,
                                 x1, y1, x2, y2); 
           }
          }
    }

  gtk_plot_pc_clip(plot->pc, NULL);
}

static void
gtk_plot_draw_axis(GtkPlot *plot, GtkPlotAxis *axis, GtkPlotVector tick_direction)
{
  GtkWidget *widget;
  gdouble x_tick;
  gdouble xp, yp, width, height;
  gint ntick;
  gdouble m = plot->magnification;
  gdouble x1, y1, x2, y2;
  gdouble px, py;
  gdouble xx, yy;

  widget = GTK_WIDGET(plot); 
  xp = plot->internal_allocation.x;
  yp = plot->internal_allocation.y;
  width = plot->internal_allocation.width;
  height = plot->internal_allocation.height;

  gtk_plot_get_pixel(plot, axis->origin.x, axis->origin.y, &x1, &y1);

  if(axis->ticks.apply_break){
    gdouble l = m * axis->ticks_length; 

    gtk_plot_get_pixel(plot, 
  	  	       axis->origin.x+axis->direction.x*(axis->ticks.break_min-axis->ticks.min),
		       axis->origin.y-axis->direction.y*(axis->ticks.break_min-axis->ticks.min),
                       &x2, &y2);
    gtk_plot_draw_line(plot, axis->line, x1, y1, x2, y2); 
    x1 = x2 + axis->direction.x * 6 * m;
    y1 = y2 + axis->direction.y * 6 * m;
    gtk_plot_draw_line(plot, axis->line, 
                       x1 - axis->direction.y * l + axis->direction.x * l/2, 
                       y1 - axis->direction.x * l + axis->direction.y * l/2, 
                       x1 + axis->direction.y * l - axis->direction.x * l/2, 
                       y1 + axis->direction.x * l - axis->direction.y * l/2);
    gtk_plot_draw_line(plot, axis->line, 
                       x2 - axis->direction.y * l + axis->direction.x * l/2, 
                       y2 - axis->direction.x * l + axis->direction.y * l/2, 
                       x2 + axis->direction.y * l - axis->direction.x * l/2, 
                       y2 + axis->direction.x * l - axis->direction.y * l/2);
    gtk_plot_get_pixel(plot, 
  	  	       axis->origin.x+axis->direction.x*(axis->ticks.max-axis->ticks.min),
		       axis->origin.y-axis->direction.y*(axis->ticks.max-axis->ticks.min),
                       &x2, &y2);
    gtk_plot_draw_line(plot, axis->line, x1, y1, x2, y2); 
  
  } else {
    gtk_plot_get_pixel(plot, 
  	  	       axis->origin.x+axis->direction.x*(axis->ticks.max-axis->ticks.min),
		       axis->origin.y-axis->direction.y*(axis->ticks.max-axis->ticks.min),
                       &x2, &y2);

    gtk_plot_draw_line(plot, axis->line, x1, y1, x2, y2); 
  }


  gtk_plot_pc_set_lineattr(plot->pc, axis->ticks_width, 0, 1, 0);

  for(ntick = 0; ntick < axis->ticks.nticks; ntick++){
    GtkPlotTick tick = axis->ticks.values[ntick];
    x_tick = tick.value;
    xx = -axis->direction.y * axis->origin.x + x_tick * axis->direction.x; 
    yy = axis->direction.x * axis->origin.y - x_tick * axis->direction.y;
    gtk_plot_get_pixel(plot, xx, yy, &px, &py);
    if(!tick.minor && x_tick >= axis->ticks.min){
      if(axis->major_mask & GTK_PLOT_TICKS_IN)
         gtk_plot_pc_draw_line(plot->pc,
                       px, 
                       py,
                       px + tick_direction.x * m * axis->ticks_length, 
                       py + tick_direction.y * m * axis->ticks_length);
      if(axis->major_mask & GTK_PLOT_TICKS_OUT)
         gtk_plot_pc_draw_line(plot->pc,
                       px, 
                       py,
                       px - tick_direction.x * m * axis->ticks_length, 
                       py - tick_direction.y * m * axis->ticks_length);
    }
    if(tick.minor && x_tick >= axis->ticks.min){
      if(axis->minor_mask & GTK_PLOT_TICKS_IN)
         gtk_plot_pc_draw_line(plot->pc,
                       px, 
                       py,
                       px + tick_direction.x * m * axis->ticks_length/2., 
                       py + tick_direction.y * m * axis->ticks_length/2.);
      if(axis->minor_mask & GTK_PLOT_TICKS_OUT)
         gtk_plot_pc_draw_line(plot->pc,
                       px, 
                       py,
                       px - tick_direction.x * m * axis->ticks_length/2., 
                       py - tick_direction.y * m * axis->ticks_length/2.);
    }
  }     
}


static void
gtk_plot_draw_labels(GtkPlot *plot, 
                     GtkPlotAxis *axis, 
                     GtkPlotVector tick_direction)
{
  GtkWidget *widget;
  GtkPlotText title, tick;
  gchar label[LABEL_MAX_LENGTH], new_label[LABEL_MAX_LENGTH];
  gdouble x_tick;
  gdouble xx, yy;
  gint text_height;
  gdouble xp, yp, width, height;
  gint ntick;
  gdouble m = plot->magnification;
  gboolean veto = FALSE;
  gdouble px, py;
  gdouble y;
  gint n = 0;

  widget = GTK_WIDGET(plot); 
  xp = plot->internal_allocation.x;
  yp = plot->internal_allocation.y;
  width = plot->internal_allocation.width;
  height = plot->internal_allocation.height;

  gtk_plot_pc_set_color (plot->pc, &axis->labels_attr.fg);

  text_height = roundint(axis->labels_attr.height*m);

  y = 0.0;
  switch(axis->labels_attr.angle){
    case 0:
           y += text_height / 2.;
           break;
    case 90:
           break;
    case 180:
           y -= text_height / 2.;
           break;
    case 270:
           break;
  }


  tick = axis->labels_attr;
  for(ntick = 0; ntick < axis->ticks.nticks; ntick++){
    if(axis->ticks.values[ntick].minor) continue;
    x_tick = axis->ticks.values[ntick].value;
    xx = -axis->direction.y * axis->origin.x + x_tick * axis->direction.x; 
    yy = axis->direction.x * axis->origin.y - x_tick * axis->direction.y;
    gtk_plot_get_pixel(plot, xx, yy, &px, &py);
    if(x_tick >= axis->ticks.min-1.e-9){
      if(!axis->custom_labels){
        gtk_plot_axis_parse_label(axis, x_tick, axis->label_precision, axis->label_style, label);
      }
      else
      {
        veto = FALSE;
        _gtkextra_signal_emit(GTK_OBJECT(axis), axis_signals[TICK_LABEL],
                        &x_tick, label, &veto);
        if(!veto) {
          if(axis->tick_labels){
            gchar **array;
            array = gtk_plot_array_get_string(axis->tick_labels);
            if(array && n < gtk_plot_array_get_size(axis->tick_labels) && array[n]) {
              g_snprintf(label, 100, "%s", array[n++]);
            } else {
              g_snprintf(label, 100, " ");
            }
          } else {
            gtk_plot_axis_parse_label(axis, x_tick, axis->label_precision, axis->label_style, label);
          }
        }
      }

      if(axis->labels_prefix){
        g_snprintf(new_label, 100, "%s%s", axis->labels_prefix, label);
        g_snprintf(label, 100, "%s", new_label);
      }
      if(axis->labels_suffix){
        g_snprintf(new_label, 100, "%s%s", label, axis->labels_suffix);
        g_snprintf(label, 100, "%s", new_label);
      }

      tick.text = label;
      tick.x = px;
      tick.y = py + y;

      if(axis->label_mask & GTK_PLOT_LABEL_IN){
         tick.x = tick.x + tick_direction.x*roundint(axis->labels_offset * m);
         tick.y = tick.y + tick_direction.y*roundint(axis->labels_offset * m);
         tick.x = (gdouble)tick.x / (gdouble)widget->allocation.width;
         tick.y = (gdouble)tick.y / (gdouble)widget->allocation.height;
         gtk_plot_draw_text(plot, tick);
      }
      if(axis->label_mask & GTK_PLOT_LABEL_OUT){
         tick.x = tick.x - tick_direction.x*roundint(axis->labels_offset * m);
         tick.y = tick.y - tick_direction.y*roundint(axis->labels_offset * m);
         tick.x = (gdouble)tick.x / (gdouble)widget->allocation.width;
         tick.y = (gdouble)tick.y / (gdouble)widget->allocation.height;
         gtk_plot_draw_text(plot, tick);
      }
    }
  }
  if(axis->title_visible && axis->title.text)
       {
         title = axis->title;
         gtk_plot_draw_text(plot, title); 
       }
}

void
gtk_plot_draw_line(GtkPlot *plot, 
                   GtkPlotLine line,
                   gdouble x1, gdouble y1, gdouble x2, gdouble y2)
{

  if(line.line_style == GTK_PLOT_LINE_NONE) return;


  gtk_plot_set_line_attributes(plot, line);

  gtk_plot_pc_draw_line(plot->pc, x1, y1, x2, y2);

}

void
gtk_plot_set_line_attributes(GtkPlot *plot, 
                             GtkPlotLine line)
{
  gdouble dot[] = {2., 3.};
  gdouble dash[] = {6., 4.};
  gdouble dot_dash[] = {6., 4., 2., 4.};
  gdouble dot_dot_dash[] = {6., 4., 2., 4., 2., 4.};
  gdouble dot_dash_dash[] = {6., 4., 6., 4., 2., 4.};

  gtk_plot_pc_set_color (plot->pc, &line.color);

  switch(line.line_style){
   case GTK_PLOT_LINE_SOLID:
        gtk_plot_pc_set_lineattr(plot->pc, line.line_width, 0, 
                                 line.cap_style, line.join_style);
        gtk_plot_pc_set_dash(plot->pc, 0, 0, 0);
        break;
   case GTK_PLOT_LINE_DOTTED:
        gtk_plot_pc_set_lineattr(plot->pc, line.line_width, 
                                 GDK_LINE_ON_OFF_DASH,
                                 line.cap_style, line.join_style);
        gtk_plot_pc_set_dash(plot->pc, 0, dot, 2);
        break;
   case GTK_PLOT_LINE_DASHED:
        gtk_plot_pc_set_lineattr(plot->pc, line.line_width, 
                                 GDK_LINE_ON_OFF_DASH, 
                                 line.cap_style, line.join_style);
        gtk_plot_pc_set_dash(plot->pc, 0, dash, 2);
        break;
   case GTK_PLOT_LINE_DOT_DASH:
        gtk_plot_pc_set_lineattr(plot->pc, line.line_width, 
                                 GDK_LINE_ON_OFF_DASH, 
                                 line.cap_style, line.join_style);
        gtk_plot_pc_set_dash(plot->pc, 0, dot_dash, 4);
        break;
   case GTK_PLOT_LINE_DOT_DOT_DASH:
        gtk_plot_pc_set_lineattr(plot->pc, line.line_width, 
                                 GDK_LINE_ON_OFF_DASH, 
                                 line.cap_style, line.join_style);
        gtk_plot_pc_set_dash(plot->pc, 0, dot_dot_dash, 6);
        break;
   case GTK_PLOT_LINE_DOT_DASH_DASH:
        gtk_plot_pc_set_lineattr(plot->pc, line.line_width, 
                                 GDK_LINE_ON_OFF_DASH,
                                 line.cap_style, line.join_style);
        gtk_plot_pc_set_dash(plot->pc, 0, dot_dash_dash, 6);
        break;
   case GTK_PLOT_LINE_NONE:
   default:
        break;
  }  
}

static void
gtk_plot_draw_legends (GtkWidget *widget)
{
  GtkPlot *plot;
  GList *datasets; 
  GtkPlotData *dataset;
  GtkAllocation legend_area;
  gint x, y;
  gint height;
  gint lwidth, lheight;
  gdouble m;

  plot = GTK_PLOT(widget);

  if(!plot->show_legends) return;

  m = plot->magnification;
  gtk_plot_pc_gsave(plot->pc);

/* first draw the white rectangle for the background */
  legend_area = gtk_plot_legends_get_allocation(plot);

  if(!plot->legends_attr.transparent){
     gtk_plot_pc_set_color(plot->pc, &plot->legends_attr.bg);
     gtk_plot_pc_draw_rectangle(plot->pc,
                               TRUE,
                               legend_area.x, legend_area.y,
                               legend_area.width, legend_area.height);
  }

  plot->legends_width = legend_area.width;
  plot->legends_height = legend_area.height;

/* now draw the legends */

  height = roundint(4 * m);
  y = legend_area.y + height;
  x = legend_area.x + roundint(4 * m);

  datasets = plot->data_sets;
  while(datasets)
   {
     dataset = GTK_PLOT_DATA(datasets->data);

     if(GTK_WIDGET_VISIBLE(GTK_WIDGET(dataset)) && dataset->show_legend)
       {
         GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(dataset)))->get_legend_size(dataset, &lwidth, &lheight);
         GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(dataset)))->draw_legend(dataset, x, y); 
         y += lheight;
         height += lheight;
       }
     datasets=datasets->next;
   }
 
   gtk_plot_pc_set_lineattr(plot->pc, plot->legends_border_width, 0, 0, 0);
   gtk_plot_pc_set_color(plot->pc, &plot->legends_attr.fg);
   gtk_plot_pc_set_dash(plot->pc, 0, 0, 0);

   if(plot->legends_border != GTK_PLOT_BORDER_NONE)
      {
        gtk_plot_pc_draw_rectangle(plot->pc,
                                  FALSE,
                                  legend_area.x, legend_area.y,
                                  legend_area.width, legend_area.height);
      }

   gtk_plot_pc_set_lineattr(plot->pc, 0, 0, 0, 0);
   if(plot->legends_border == GTK_PLOT_BORDER_SHADOW)
      {
        gtk_plot_pc_draw_rectangle(plot->pc,
                                  TRUE,
                                  legend_area.x + roundint(plot->legends_shadow_width * m), 
                                  legend_area.y + legend_area.height,
                                  legend_area.width, 
                                  roundint(plot->legends_shadow_width * m));
        gtk_plot_pc_draw_rectangle(plot->pc,
                                  TRUE,
                                  legend_area.x + legend_area.width, 
                                  legend_area.y + roundint(plot->legends_shadow_width * m),
                                  roundint(plot->legends_shadow_width * m), 
                                  legend_area.height);
      }

  datasets = plot->data_sets;
  while(datasets)
   {
     dataset = GTK_PLOT_DATA(datasets->data);

     if(GTK_WIDGET_VISIBLE(GTK_WIDGET(dataset)) && dataset->show_gradient)
       {
         GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(dataset)))->draw_gradient(dataset); 
       }
     datasets=datasets->next;
   }
 
  gtk_plot_pc_grestore(plot->pc);
}

void            
gtk_plot_axis_ticks_recalc      (GtkPlotAxis *axis)
{
  axis->ticks_recalc(axis);
}

void            
gtk_plot_axis_ticks_autoscale   (GtkPlotAxis *axis,
                                 gdouble xmin, gdouble xmax,
                                 gint *precision)
{
  axis->ticks_autoscale(axis, xmin, xmax, precision);
}

gdouble         
gtk_plot_axis_ticks_transform   (GtkPlotAxis *axis, gdouble y)
{
  return (axis->ticks_transform(axis, y));
}

gdouble         
gtk_plot_axis_ticks_inverse     (GtkPlotAxis *axis, gdouble x)
{
  return (axis->ticks_inverse(axis, x));
}

void            
gtk_plot_axis_parse_label       (GtkPlotAxis *axis,
				 gdouble val,
                                 gint precision,
                                 gint style,
                                 gchar *label)
{
  axis->parse_label(axis, val, precision, style, label);
}

gdouble
gtk_plot_ticks_transform(GtkPlotAxis *axis, gdouble x)
{
  gdouble position = 0;
  GtkPlotTicks *_ticks = &axis->ticks;
  GtkPlotTicks ticks = *_ticks;

  switch( ticks.scale ){
    case GTK_PLOT_SCALE_LOG10:
      if( x <= 0.0 || ticks.min <= 0.0 || ticks.max <= 0.0 )
        return 0;

      if(ticks.apply_break){
        if(x <= ticks.break_min) {
          position = log(x/ticks.min) / log(ticks.break_min / ticks.min);
          position *= ticks.break_position;
        } else {
          if(x <= ticks.break_max) return ticks.break_position;

          if(ticks.break_scale == GTK_PLOT_SCALE_LOG10)
            position = log(x/ticks.break_max) / log(ticks.max/ticks.break_max);
          else
            position = (x - ticks.break_max) / (ticks.max - ticks.break_max);
          position = ticks.break_position + (1 - ticks.break_position) * position;
        }
      } else {
        position = log(x/ticks.min) / log(ticks.max/ticks.min);
      }
      break;

    case GTK_PLOT_SCALE_LINEAR:
    default:
      if(ticks.apply_break){
        if(x <= ticks.break_min) {
          position = (x - ticks.min) / (ticks.break_min - ticks.min);
          position *= ticks.break_position;
        } else {
          if(x <= ticks.break_max) return ticks.break_position;

          if(ticks.break_scale == GTK_PLOT_SCALE_LOG10)
            position = log(x/ticks.break_max) / log(ticks.max/ticks.break_max);
          else
            position = (x - ticks.break_max) / (ticks.max - ticks.break_max);

          position = ticks.break_position + (1 - ticks.break_position) * position;
        }


      } else {
        position = (x - ticks.min) / (ticks.max - ticks.min);
      }
      break;
  }

  return(position);
}


gdouble
gtk_plot_ticks_inverse(GtkPlotAxis *axis, gdouble x)
{
    gdouble point = 0;
    GtkPlotTicks *_ticks = &axis->ticks;
    GtkPlotTicks ticks = *_ticks;

    switch(ticks.scale){
      case GTK_PLOT_SCALE_LINEAR:
        if(ticks.apply_break){
          if(x <= ticks.break_position){
            point = ticks.min + x*(ticks.break_min-ticks.min)/ticks.break_position;
          } else {
            point = ticks.break_max + (x - ticks.break_position)*(ticks.max-ticks.break_max)/(1-ticks.break_position);
          }
        } else { 
            point = ticks.min + x*(ticks.max-ticks.min);
        }
        break;
      case GTK_PLOT_SCALE_LOG10:
/* FIXME */
        if(ticks.apply_break){
          if(x <= ticks.break_position){
            point = ticks.min + x*(ticks.break_min-ticks.min)/ticks.break_position;
          } else {
            point = ticks.break_max + (x - ticks.break_position)*(ticks.max-ticks.break_max)/(1-ticks.break_position);
          }
        } else { 
            point = ticks.min + x*(ticks.max-ticks.min);
        }
        break;
    }
    return point;
}

inline gint
roundint (gdouble x)
{
 return (x+.50999999471);
}

void
gtk_plot_parse_label(GtkPlotAxis *axis, gdouble val, gint precision, gint style, gchar *label)
{
  gdouble auxval;
  gint intspace = 0;
  gint power;
  gfloat v;
  GtkPlotScale scale = axis->ticks.scale;

  auxval = fabs(val);

  power = 0.0;
  if(auxval != 0.0)
       power = (gint)log10(auxval);

  v = val / pow(10.0, power); 
  if(fabs(v) < 1.0 && v != 0.0){
     v *= 10.0;
     power -= 1;
  }
  if(fabs(v) >= 10.0){
     v /= 10.0;
     power += 1;
  }
/*
  if(power < -12){
     power = 0;
     v = 0.0f;
  }
*/

  if(auxval > 1)
    intspace = (gint)log10(auxval);


  switch(style){
    case GTK_PLOT_LABEL_EXP:    
      sprintf (label, "%*.*E", 1, precision, val);
      break;
    case GTK_PLOT_LABEL_POW:    
      if(scale == GTK_PLOT_SCALE_LOG10)
        sprintf (label, "10\\S%i", power);
      else
        sprintf (label, "%*.*f\\4x\\N10\\S%i", 1, precision, v, power);
      break;
    case GTK_PLOT_LABEL_FLOAT:
    default:
      if(fabs(val) < pow(10,-precision)) val = 0.0f;
      sprintf (label, "%*.*f", intspace, precision, val);
  }

}

void
gtk_plot_draw_text(GtkPlot *plot, 
                   GtkPlotText text) 
{
  gint x, y;

  if(!text.text) return;
  if(strlen(text.text) == 0) return;
  if(plot->drawable == NULL) return;

  x = text.x * GTK_WIDGET(plot)->allocation.width;
  y = text.y * GTK_WIDGET(plot)->allocation.height;

  gtk_plot_paint_text(plot, x, y, text);
}

static void
gtk_plot_paint_text(GtkPlot *plot, 
                    gint x, gint y,
                    GtkPlotText text)
{
  gdouble m = plot->magnification;

  if(!text.text) return;
  if(strlen(text.text) == 0) return;
  if(plot->drawable == NULL) return;

  gtk_plot_pc_draw_string(plot->pc,
			 x, y,
			 text.angle,
			 &text.fg,
			 &text.bg,
			 text.transparent,
			 text.border,
			 roundint(m * text.border_space),
			 roundint(m * text.border_width),
			 roundint(m * text.shadow_width),
			 text.font,
			 roundint(m * text.height),
			 text.justification,
			 text.text);

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_text_get_size(const gchar *text, gint angle, 
                       const gchar* text_font, 
                       gint text_height,   
                       gint *width, gint *height,
                       gint *ascent, gint *descent)
{
  PangoFontDescription *font, *latin_font;
  GtkPSFont *psfont, *base_psfont, *latin_psfont;
  gint old_width, old_height;
  gboolean italic, bold;
  gint fontsize;
  gint x, y, y0;
  GList *family;
  gint numf;
  gchar insert_char;
  gchar num[4];
  const gchar *aux = text;
  const gchar *lastchar = text;
  const gchar *wtext = text;
  const gchar *xaux = text;
  PangoFontMetrics *metrics = NULL;
  PangoLayout *layout = NULL;
  PangoRectangle rect;
  PangoContext *context;
  gint i = 0;

  if(!text || strlen(text) == 0) return;
  layout = pango_layout_new(context = gdk_pango_context_get_for_screen(gdk_screen_get_default()));
  g_object_unref(G_OBJECT(context));
  pango_layout_set_text(layout, text, -1); 

  gtk_psfont_get_families(&family, &numf);
  base_psfont = psfont = gtk_psfont_get_by_name(text_font);
  font = gtk_psfont_get_font_description(psfont, text_height);
  pango_layout_get_extents(layout, NULL, &rect);
  old_width = PANGO_PIXELS(rect.width);
  old_height = PANGO_PIXELS(rect.height);

  italic = psfont->italic;
  bold = psfont->bold;
  fontsize = text_height;
  
  if (psfont->i18n_latinfamily) {
    latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily, italic,
					     bold);
    latin_font = gtk_psfont_get_font_description(latin_psfont, text_height);
  } else {
    latin_font = NULL;
    latin_psfont = NULL;
  }

  metrics = pango_context_get_metrics(pango_layout_get_context(layout), font, gtk_get_default_language());
  y0 = y = PANGO_PIXELS(pango_font_metrics_get_ascent(metrics));
  y0 = y = 0;
  x = 0;
  old_width = 0;

  *ascent = PANGO_PIXELS(pango_font_metrics_get_ascent(metrics));
  *descent = PANGO_PIXELS(pango_font_metrics_get_descent(metrics));

  aux = wtext = text;
  while(aux && *aux != '\0' && *aux != '\n'){
   if(*aux == '\\'){
     aux = g_utf8_next_char(aux);
     switch(*aux){
       case '0': case '1': case '2': case '3':
       case '4': case '5': case '6': case '7': case '9':
           psfont = gtk_psfont_get_by_family((gchar *)g_list_nth_data(family, *aux-'0'), italic, bold);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
	   /*
	     The 0th-9th data of family is supposed to be a built-in Latin
	     font defined in font_data[], so this code is not needed unless
	     the font_data[] is modified.
	     
	   if (psfont->i18n_latinfamily) {
	     latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily,
						      italic, bold);
             gdk_font_unref(latin_font);
	     latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
	   }
	   */
	   
           aux = g_utf8_next_char(aux);
           break;
       case '8': case 'g':
           psfont = gtk_psfont_get_by_family("Symbol", italic, bold);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
	   /* The code commented out above might be needed here if the
	      font_data[] is modified.
	   */
           aux = g_utf8_next_char(aux);
           break;
       case 'B':
           bold = TRUE;
           psfont = gtk_psfont_get_by_family(psfont->family, italic, bold);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
	   if (psfont->i18n_latinfamily) {
	     latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily,
						      italic, bold);
             if(latin_font) pango_font_description_free(latin_font);
	     latin_font = gtk_psfont_get_font_description(latin_psfont,
						 fontsize);
	   }
           aux = g_utf8_next_char(aux);
           break;
       case 'i':
           italic = TRUE;
           psfont = gtk_psfont_get_by_family(psfont->family, italic, bold);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
	   if (psfont->i18n_latinfamily) {
	     latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily,
						      italic, bold);
             if(latin_font) pango_font_description_free(latin_font);
	     latin_font = gtk_psfont_get_font_description(latin_psfont,
						 fontsize);
	   }
           aux = g_utf8_next_char(aux);
           break;
       case 'S': case '^':
           fontsize = (int)((gdouble)fontsize * 0.6 + 0.5);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           pango_font_metrics_unref(metrics);
           metrics = pango_context_get_metrics(pango_layout_get_context(layout), font, gtk_get_default_language());
           y += PANGO_PIXELS(pango_font_metrics_get_ascent(metrics));
           if (psfont->i18n_latinfamily) {
             if(latin_font) pango_font_description_free(latin_font);
             latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
           }
           aux = g_utf8_next_char(aux);
           break;
       case 's': case '_':
           fontsize = (int)((gdouble)fontsize * 0.6 + 0.5);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           pango_font_metrics_unref(metrics);
           metrics = pango_context_get_metrics(pango_layout_get_context(layout), font, gtk_get_default_language());
           y -= PANGO_PIXELS(pango_font_metrics_get_descent(metrics));
           if (psfont->i18n_latinfamily) {
             if(latin_font) pango_font_description_free(latin_font);
             latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
           }
           aux = g_utf8_next_char(aux);
           break;
       case '+':
           fontsize += 3;
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
	   if (psfont->i18n_latinfamily){
             if(latin_font) pango_font_description_free(latin_font);
	     latin_font = gtk_psfont_get_font_description(latin_psfont,
						 fontsize);
           }
           aux = g_utf8_next_char(aux);
           break;
       case '-':
           fontsize -= 3;
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
	   if (psfont->i18n_latinfamily){
             if(latin_font) pango_font_description_free(latin_font);
	     latin_font = gtk_psfont_get_font_description(latin_psfont,
						 fontsize);
           }
           aux = g_utf8_next_char(aux);
           break;
       case 'N':
           psfont = base_psfont;
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, text_height);
	   y = y0;
           italic = psfont->italic;
           bold = psfont->bold;
           fontsize = text_height;
	   if (psfont->i18n_latinfamily) {
	     latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily,
						      italic, bold);
             if(latin_font) pango_font_description_free(latin_font);
	     latin_font = gtk_psfont_get_font_description(latin_psfont,
						 fontsize);
	   }
           aux = g_utf8_next_char(aux);
           break;
       case 'b':
	   if(lastchar){
             const gchar *aux2 = lastchar;
             gint i = g_utf8_next_char(lastchar) != ++aux2 ? 2 : 1;
             pango_layout_set_text(layout, lastchar, i);
             pango_layout_get_extents(layout, NULL, &rect);
             x -= PANGO_PIXELS(rect.width);
 
	     if (lastchar == wtext)
	       lastchar = NULL;
	     else
	       lastchar--;
	   } else {
             pango_layout_set_text(layout, "X", 1);
             pango_layout_get_extents(layout, NULL, &rect);
             x -= PANGO_PIXELS(rect.width);
           }
           aux = g_utf8_next_char(aux);
	   break;
       case 'x':
           xaux = aux + 1;
           for (i=0; i<3; i++){
            if (xaux[i] >= '0' && xaux[i] <= '9')
              num[i] = xaux[i];
            else
              break;
           }
           if (i < 3){
              aux++;
              break;
           }
           num[3] = '\0';
           insert_char = (gchar)atoi(num);
           /* \xNNN is always outputted with latin fonts. */
           pango_layout_set_font_description(layout, font);
           pango_layout_set_text(layout, aux, 1);
           pango_layout_get_extents(layout, NULL, &rect);
           x += PANGO_PIXELS(rect.width);
           aux += 4;
           lastchar = aux - 1;
           break;
       default:
           if(aux && *aux != '\0' && *aux !='\n'){
             const gchar *aux2 = aux;
             gint i = g_utf8_next_char(aux) != ++aux2 ? 2 : 1;
             pango_layout_set_font_description(layout, font);
             pango_layout_set_text(layout, aux, i);
             pango_layout_get_extents(layout, NULL, &rect);
	     x += PANGO_PIXELS(rect.width);
	     lastchar = aux;
             aux = g_utf8_next_char(aux);
           }
           break;
     }
   } else {
     if(aux && *aux != '\0' && *aux != '\n'){
       const gchar *aux2 = aux;
       gint i = g_utf8_next_char(aux) != ++aux2 ? 2 : 1;
       pango_layout_set_font_description(layout, font);
       pango_layout_set_text(layout, aux, i);
       pango_layout_get_extents(layout, NULL, &rect);
       x += PANGO_PIXELS(rect.width);
       lastchar = aux;
       aux = g_utf8_next_char(aux);
       if(x > old_width) old_width = x;
     }
   }
  }
  pango_font_description_free(font);
  if(latin_font) pango_font_description_free(latin_font);
  pango_font_metrics_unref(metrics);
  g_object_unref(G_OBJECT(layout));

  old_height = *ascent + *descent;
  *width = old_width;
  *height = old_height;
  if(angle == 90 || angle == 270)
    {
      *width = old_height;
      *height = old_width;
    }
}

void
gtk_plot_text_get_area(const gchar *text, gint angle, GtkJustification just, 
                       const gchar *font, gint font_height,
                       gint *x, gint *y,
                       gint *width, gint *height)
{
  gint ascent, descent;

  if(text == NULL) return;

  gtk_plot_text_get_size(text, angle, font, 
                         font_height, 
                         width, height, &ascent, &descent);

  *x = 0;
  *y = 0;

  switch(just){
    case GTK_JUSTIFY_LEFT:
      switch(angle){
        case 0:
            *y -= ascent;
            break;
        case 90:
            *y -= *height;
            *x -= ascent;
            break;
        case 180:
            *x -= *width;
            *y -= descent;
            break;
        case 270:
            *x -= descent;
            break;
      }
      break;
    case GTK_JUSTIFY_RIGHT:
      switch(angle){
        case 0:
            *x -= *width;
            *y -= ascent;
            break;
        case 90:
            *x -= ascent;
            break;
        case 180:
            *y -= descent; 
            break;
        case 270:
            *y -= *height;
            *x -= descent; 
            break;
      }
      break;
    case GTK_JUSTIFY_CENTER:
    default:
      switch(angle){
        case 0:
            *x -= *width / 2.;
            *y -= ascent;
	    break;
        case 90:
            *x -= ascent;
            *y -= *height / 2.;
	    break;
        case 180:
            *x -= *width / 2.;
            *y -= descent;
            break;
        case 270:
            *x -= descent;
            *y -= *height / 2.;
            break;
      }
  }

}

/******************************************
 *	gtk_plot_clip_data
 *	gtk_plot_get_position
 *	gtk_plot_get_size
 *      gtk_plot_get_internal_allocation 
 *	gtk_plot_set_background
 *	gtk_plot_move
 *	gtk_plot_resize
 *	gtk_plot_move_resize
 *	gtk_plot_get_pixel
 *	gtk_plot_get_point
 *	gtk_plot_real_get_pixel
 *      gtk_plot_set_xscale
 *      gtk_plot_set_yscale
 *      gtk_plot_draw_text
 ******************************************/
void
gtk_plot_clip_data (GtkPlot *plot, gboolean clip)
{
  plot->clip_data = clip;
}

void
gtk_plot_get_position (GtkPlot *plot, gdouble *x, gdouble *y)
{
  *x = plot->x;
  *y = plot->y;
}

void
gtk_plot_get_size (GtkPlot *plot, gdouble *width, gdouble *height)
{
  *width = plot->width;
  *height = plot->height;
}

GtkAllocation 
gtk_plot_get_internal_allocation (GtkPlot *plot)
{
  return(plot->internal_allocation);
}

void
gtk_plot_set_background(GtkPlot *plot, const GdkColor *color)
{
  plot->background = *color;

  gtk_plot_paint(plot);

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_move (GtkPlot *plot, gdouble x, gdouble y)
{
  gboolean veto = TRUE;

  _gtkextra_signal_emit (GTK_OBJECT(plot), plot_signals[MOVED],
                   &x, &y, &veto);

  if(!veto) return;

  plot->left->title.x += (x - plot->x);  
  plot->left->title.y += (y - plot->y);
  plot->right->title.x += (x - plot->x);  
  plot->right->title.y += (y - plot->y);
  plot->top->title.x += (x - plot->x);  
  plot->top->title.y += (y - plot->y);
  plot->bottom->title.x += (x - plot->x);  
  plot->bottom->title.y += (y - plot->y);

  plot->x = x;
  plot->y = y;

  plot->internal_allocation.x = GTK_WIDGET(plot)->allocation.x + roundint(plot->x * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.y = GTK_WIDGET(plot)->allocation.y + roundint(plot->y * GTK_WIDGET(plot)->allocation.height);
  plot->internal_allocation.width = roundint(plot->width * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.height = roundint(plot->height * GTK_WIDGET(plot)->allocation.height);

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_resize (GtkPlot *plot, gdouble width, gdouble height)
{
  gboolean veto = TRUE;

  _gtkextra_signal_emit (GTK_OBJECT(plot), plot_signals[RESIZED],
                   &width, &height, &veto);

  if(!veto) return;

  plot->left->title.y += (height - plot->height) / 2.;
  plot->right->title.x += (width - plot->width);  
  plot->right->title.y += (height - plot->height) / 2.;
  plot->top->title.x += (width - plot->width) / 2.;  
  plot->bottom->title.x += (width - plot->width) / 2.;  
  plot->bottom->title.y += (height - plot->height);

  plot->width = width;
  plot->height = height;

  plot->internal_allocation.x = GTK_WIDGET(plot)->allocation.x + roundint(plot->x * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.y = GTK_WIDGET(plot)->allocation.y + roundint(plot->y * GTK_WIDGET(plot)->allocation.height);
  plot->internal_allocation.width = roundint(plot->width * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.height = roundint(plot->height * GTK_WIDGET(plot)->allocation.height);

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], FALSE);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_set_magnification (GtkPlot *plot, gdouble magnification)
{
  GtkWidget *widget;
 
  widget = GTK_WIDGET(plot); 
  plot->magnification = magnification;

  plot->internal_allocation.x = GTK_WIDGET(plot)->allocation.x + roundint(plot->x * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.y = GTK_WIDGET(plot)->allocation.y + roundint(plot->y * GTK_WIDGET(plot)->allocation.height);
  plot->internal_allocation.width = roundint(plot->width * GTK_WIDGET(plot)->allocation.width);
  plot->internal_allocation.height = roundint(plot->height * GTK_WIDGET(plot)->allocation.height);

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], FALSE);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}
 
void
gtk_plot_move_resize (GtkPlot *plot, 
	              gdouble x, gdouble y,
                      gdouble width, gdouble height)
{
  gtk_plot_move(plot, x, y);
  gtk_plot_resize(plot, width, height);

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_get_pixel(GtkPlot *plot, gdouble xx, gdouble yy, gdouble *x, gdouble *y)
{
    GTK_PLOT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(plot)))->get_pixel (GTK_WIDGET(plot), xx, yy, x, y);
}

void 
gtk_plot_get_point(GtkPlot *plot, gint x, gint y, gdouble *xx, gdouble *yy)
{
    GTK_PLOT_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(plot)))->get_point (GTK_WIDGET(plot), x, y, xx, yy);
}


static void
gtk_plot_real_get_pixel(GtkWidget *widget, 
                        gdouble xx, gdouble yy , 
			gdouble *x, gdouble *y)
{
    GtkPlot *plot;
    gint xp, yp, width, height;

    plot = GTK_PLOT(widget); 
    xp = plot->internal_allocation.x;
    yp = plot->internal_allocation.y;
    width = plot->internal_allocation.width;
    height = plot->internal_allocation.height;

    *y = gtk_plot_ticks_transform(plot->left, yy)*height;
    *x = gtk_plot_ticks_transform(plot->bottom, xx)*width;

    if(!plot->reflect_x)
      *x = widget->allocation.x + xp + *x;
    else
      *x = widget->allocation.x + xp + width - *x;

    if(!plot->reflect_y)
      *y = widget->allocation.y + yp + height - *y;
    else
      *y = widget->allocation.y + yp + *y;
}

static void 
gtk_plot_real_get_point(GtkWidget *widget, 
                        gint x, gint y, 
                        gdouble *px, gdouble *py)
{
    GtkPlot *plot;
    gdouble xx, yy;
    gdouble xp, yp, width, height;
    gdouble rx, ry;

    plot = GTK_PLOT(widget); 
    xp = plot->internal_allocation.x;
    yp = plot->internal_allocation.y;
    width = plot->internal_allocation.width;
    height = plot->internal_allocation.height;

    if(!plot->reflect_x)
      xx = x - widget->allocation.x - xp;
    else
      xx = width - (x - widget->allocation.x - xp);

    if(!plot->reflect_y)
      yy = widget->allocation.y + yp + height - y;
    else
      yy = y - widget->allocation.y - yp;

    rx = plot->bottom->ticks.max - plot->bottom->ticks.min;
    ry = plot->left->ticks.max - plot->left->ticks.min;

    *px = gtk_plot_axis_ticks_inverse(plot->bottom, xx / width);
    *py = gtk_plot_axis_ticks_inverse(plot->left, yy / height);
}

void
gtk_plot_set_xrange (GtkPlot *plot, 
                     gdouble xmin, gdouble xmax)
{
  if(xmin > xmax) return;

  plot->xmin = xmin;
  plot->xmax = xmax;

  plot->bottom->ticks.min = xmin;
  plot->bottom->ticks.max = xmax;
  plot->top->ticks.min = xmin;
  plot->top->ticks.max = xmax;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], TRUE);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_set_yrange (GtkPlot *plot, 
                     gdouble ymin, gdouble ymax) 
{
  if(ymin > ymax) return;

  plot->ymin = ymin;
  plot->ymax = ymax;

  plot->left->ticks.min = ymin;
  plot->left->ticks.max = ymax;
  plot->right->ticks.min = ymin;
  plot->right->ticks.max = ymax;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], TRUE);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}


void
gtk_plot_set_range (GtkPlot *plot, 
                    gdouble xmin, gdouble xmax, 
                    gdouble ymin, gdouble ymax)
{
  if(xmin > xmax || ymin > ymax) return;

  plot->xmin = xmin;
  plot->xmax = xmax;
  plot->ymin = ymin;
  plot->ymax = ymax;

  plot->bottom->ticks.min = xmin;
  plot->bottom->ticks.max = xmax;
  plot->top->ticks.min = xmin;
  plot->top->ticks.max = xmax;
  plot->left->ticks.min = ymin;
  plot->left->ticks.max = ymax;
  plot->right->ticks.min = ymin;
  plot->right->ticks.max = ymax;

  gtk_plot_axis_ticks_recalc(plot->bottom);
  gtk_plot_axis_ticks_recalc(plot->top);
  gtk_plot_axis_ticks_recalc(plot->left);
  gtk_plot_axis_ticks_recalc(plot->right);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], TRUE);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void 
gtk_plot_ticks_autoscale(GtkPlotAxis *axis, gdouble xmin, gdouble xmax, gint *precision)
{
  GtkPlotTicks *ticks = &axis->ticks;

  if(xmin > xmax) return;

  if(ticks->scale == GTK_PLOT_SCALE_LOG10) {
          ticks->step = 1;
          ticks->nminor = 8;
          xmin = floor(log10(fabs(xmin))) - 1.;
          *precision = MAX(xmin + 1, 1);
          xmin = pow(10., xmin);
          xmax = ceil(log10(fabs(xmax)));
          xmax = pow(10., xmax);
          if(xmin == 0.0) xmin = xmax/1000.;
  } else {
          gdouble amin, amax;
          gdouble pmin, pmax;
          gdouble dx;
          gdouble pstep;

          amin = xmin;
          amax = xmax;
          if(xmin == xmax){
            if(xmin == 0.0){
              amax = xmax = 0.1;
            }else{
              pstep = floor(log10(fabs(xmin)));
              dx = xmin/pow(10., pstep) * pow(10., pstep);
              amax = xmax = xmin + 2 * dx;
              amin = xmin = xmin - 2 * dx;
            }
          }
          dx = (xmax - xmin)/8.;
          xmin -= dx;
          xmax += dx;
          if(xmin == 0.0) xmin -= dx;
          if(xmax == 0.0) xmax += dx;
          pmin = floor(log10(fabs(xmin))) - 1.;
          pmax = floor(log10(fabs(xmax))) - 1.;
          xmin = floor(xmin/pow(10., pmin)) * pow(10., pmin);
          xmax = floor(xmax/pow(10., pmax)) * pow(10., pmax);
          pstep = floor(log10(fabs(dx)));
          dx = ticks->step = floor(dx/pow(10., pstep)) * pow(10., pstep);
          while(xmin >= amin) xmin -= dx;
          while(xmax <= amax) xmax += dx;
          dx = floor((xmax - xmin)/ticks->step);
          while(dx > 10.){
            ticks->step *= 2;
            dx = floor((xmax - xmin)/ticks->step);
          }
          xmin = floor(xmin/ticks->step) * ticks->step;
          xmax = ceil(xmax/ticks->step) * ticks->step;
          ticks->step = ticks->step;
          *precision = MAX(floor(fabs(pstep)), 0);
  }

  ticks->min = xmin;
  ticks->max = xmax;

  gtk_plot_axis_ticks_recalc(axis);
  axis->label_precision = *precision;
}

void
gtk_plot_autoscale(GtkPlot *plot)
{
  GtkPlotData *dataset;
  GList *list;
  gdouble xmin, xmax;
  gdouble ymin, ymax;
  gint n;
  gboolean change = FALSE;
  gint labels_precision;

  if(!plot->data_sets) return;

  xmin = plot->xmax;
  xmax = plot->xmin;
  ymin = plot->ymax;
  ymax = plot->ymin;

  list = plot->data_sets;
  while(list){
     dataset = GTK_PLOT_DATA(list->data);
     if(!dataset->is_function){
       for(n = 0; n < dataset->num_points; n++){
           gdouble fx, fy, fz, fa;
           gdouble fdx, fdy, fdz, fda;
           gchar *label;
           gboolean error;
           gtk_plot_data_get_point(dataset, n, 
                                   &fx, &fy, &fz, &fa, 
                                   &fdx, &fdy, &fdz, &fda, 
                                   &label, &error);
           if(fx < xmin) xmin = fx;
           if(fy < ymin) ymin = fy;
           if(fx > xmax) xmax = fx;
           if(fy > ymax) ymax = fy;
           change = TRUE;
       }
     }

     list = list->next;
  } 

  if(!change) return;

  gtk_plot_axis_ticks_autoscale(plot->bottom, xmin, xmax, &labels_precision);
  gtk_plot_axis_ticks_autoscale(plot->left, ymin, ymax, &labels_precision);
  gtk_plot_axis_ticks_autoscale(plot->top, xmin, xmax, &labels_precision);
  gtk_plot_axis_ticks_autoscale(plot->right, ymin, ymax, &labels_precision);

  plot->xmin = plot->bottom->ticks.min;
  plot->xmax = plot->bottom->ticks.max;
  plot->ymin = plot->left->ticks.min;
  plot->ymax = plot->left->ticks.max;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], TRUE);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_get_xrange (GtkPlot *plot, 
                     gdouble *xmin, gdouble *xmax)
{
  *xmax = plot->xmax;
  *xmin = plot->xmin;
}

void
gtk_plot_get_yrange (GtkPlot *plot, 
                     gdouble *ymin, gdouble *ymax)
{
  *ymax = plot->ymax;
  *ymin = plot->ymin;
}

void
gtk_plot_set_xscale (GtkPlot *plot, GtkPlotScale scale_type)
{
  plot->xscale = scale_type;
  plot->bottom->ticks.scale = scale_type;
  plot->top->ticks.scale = scale_type;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], FALSE);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_set_yscale (GtkPlot *plot, GtkPlotScale scale_type)
{
  plot->yscale = scale_type;
  plot->left->ticks.scale = scale_type;
  plot->right->ticks.scale = scale_type;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[UPDATE], FALSE);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

GtkPlotScale
gtk_plot_get_xscale (GtkPlot *plot)
{
  return plot->bottom->ticks.scale;
}

GtkPlotScale
gtk_plot_get_yscale (GtkPlot *plot)
{
  return plot->left->ticks.scale;
}

void
gtk_plot_reflect_x (GtkPlot *plot, gboolean reflect)
{
  plot->reflect_x = reflect;
}

void
gtk_plot_reflect_y (GtkPlot *plot, gboolean reflect)
{
  plot->reflect_y = reflect;
}

gboolean
gtk_plot_is_x_reflected (GtkPlot *plot)
{
  return plot->reflect_x;
}

gboolean
gtk_plot_is_y_reflected (GtkPlot *plot)
{
  return plot->reflect_y;
}

GtkPlotText *
gtk_plot_put_text (GtkPlot *plot, gdouble x, gdouble y, 
                   const gchar *font, gint height, gint angle,
                   const GdkColor *fg, const GdkColor *bg, 
		   gboolean transparent,
		   GtkJustification justification,
	           const gchar *text)
{
  GtkWidget *widget;
  GtkPlotText *text_attr;
  GdkRectangle area;

  widget = GTK_WIDGET(plot);

  text_attr = g_new0(GtkPlotText, 1);

  area.x = widget->allocation.x;
  area.y = widget->allocation.y;

  text_attr->x = x;
  text_attr->y = y;
  text_attr->angle = angle;
  text_attr->justification = justification;
  text_attr->fg = widget->style->black;
  text_attr->bg = widget->style->white;
  text_attr->transparent = transparent;
  text_attr->border = 0;
  text_attr->border_space = 2;
  text_attr->border_width = 0;
  text_attr->shadow_width = 3;

  if(!font) {
    text_attr->font = g_strdup(DEFAULT_FONT);
    text_attr->height = DEFAULT_FONT_HEIGHT;
  } else {
    text_attr->font = g_strdup(font);
    text_attr->height = height;
  }

  text_attr->text = NULL;
  if(text) text_attr->text = g_strdup(text);

  if(fg != NULL)
    text_attr->fg = *fg;

  if(bg != NULL)
    text_attr->bg = *bg;

  plot->text = g_list_append(plot->text, text_attr);
  gtk_plot_draw_text(plot, *text_attr);

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
  return text_attr;
}

void
gtk_plot_text_set_attributes (GtkPlotText *text_attr,
                              const gchar *font,
                              gint height,
                              gint angle,
                              const GdkColor *fg,
                              const GdkColor *bg,
			      gboolean transparent,
		              GtkJustification justification,
	                      const gchar *text)
{
  text_attr->angle = angle;
  gdk_color_black(gdk_colormap_get_system(), &text_attr->fg);
  gdk_color_white(gdk_colormap_get_system(), &text_attr->bg);
  text_attr->justification = justification;
  text_attr->transparent = transparent;

  if(!font) {
    text_attr->font = g_strdup(DEFAULT_FONT);
    text_attr->height = DEFAULT_FONT_HEIGHT;
  } else {
    text_attr->font = g_strdup(font);
    text_attr->height = height;
  }

  if(text_attr->text) g_free(text_attr->text);
  text_attr->text = NULL;
  if(text) text_attr->text = g_strdup(text);

  if(bg != NULL)
    text_attr->bg = *bg;
  
  if(fg != NULL)
    text_attr->fg = *fg;
  
}

void
gtk_plot_text_set_border (GtkPlotText *text,
                          GtkPlotBorderStyle border,
                          gint border_space,
	                  gint border_width,
	                  gint shadow_width)
{
  if(!text) return;

  text->border = border;
  text->border_width = border_width;
  text->border_space = border_space;
  text->shadow_width = shadow_width;
}

/******************************************/
void            gtk_plot_set_ticks         	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation,
                                                 gdouble major_step,
                                                 gint nminor)
{
  if(orientation == GTK_PLOT_AXIS_X){
    gtk_plot_axis_set_ticks(plot->top, major_step, nminor);
    gtk_plot_axis_set_ticks(plot->bottom, major_step, nminor);
  } else {
    gtk_plot_axis_set_ticks(plot->left, major_step, nminor);
    gtk_plot_axis_set_ticks(plot->right, major_step, nminor);
  }
}

void            gtk_plot_set_major_ticks   	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation,
                                                 gdouble major_step)
{
  if(orientation == GTK_PLOT_AXIS_X){
    gtk_plot_axis_set_major_ticks(plot->top, major_step);
    gtk_plot_axis_set_major_ticks(plot->bottom, major_step);
  } else {
    gtk_plot_axis_set_major_ticks(plot->left, major_step);
    gtk_plot_axis_set_major_ticks(plot->right, major_step);
  }
}

void            gtk_plot_set_minor_ticks   	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation,
                                                 gint nminor)
{
  if(orientation == GTK_PLOT_AXIS_X){
    gtk_plot_axis_set_minor_ticks(plot->top, nminor);
    gtk_plot_axis_set_minor_ticks(plot->bottom, nminor);
  } else {
    gtk_plot_axis_set_minor_ticks(plot->left, nminor);
    gtk_plot_axis_set_minor_ticks(plot->right, nminor);
  }
}

void            gtk_plot_set_ticks_limits  	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation,
                                                 gdouble begin, gdouble end)
{
  if(orientation == GTK_PLOT_AXIS_X){
    gtk_plot_axis_set_ticks_limits(plot->top, begin, end);
    gtk_plot_axis_set_ticks_limits(plot->bottom, begin, end);
  } else {
    gtk_plot_axis_set_ticks_limits(plot->left, begin, end);
    gtk_plot_axis_set_ticks_limits(plot->right, begin, end);
  }
}

void            gtk_plot_unset_ticks_limits	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation)
{
  if(orientation == GTK_PLOT_AXIS_X){
    gtk_plot_axis_unset_ticks_limits(plot->top);
    gtk_plot_axis_unset_ticks_limits(plot->bottom);
  } else {
    gtk_plot_axis_unset_ticks_limits(plot->left);
    gtk_plot_axis_unset_ticks_limits(plot->right);
  }
}

void            gtk_plot_set_break         	(GtkPlot *plot,
                                                 GtkPlotOrientation orient,
                                                 gdouble min, gdouble max,
                                                 gdouble step_after,
                                                 gint nminor_after,
                                                 GtkPlotScale scale_after,
                                                 gdouble pos)
{
  if(orient == GTK_PLOT_AXIS_X){
    gtk_plot_axis_set_break(plot->top, min, max, step_after, nminor_after, scale_after, pos);
    gtk_plot_axis_set_break(plot->bottom, min, max, step_after, nminor_after, scale_after, pos);
  } else {
    gtk_plot_axis_set_break(plot->left, min, max, step_after, nminor_after, scale_after, pos);
    gtk_plot_axis_set_break(plot->right, min, max, step_after, nminor_after, scale_after, pos);
  }
}

void            gtk_plot_remove_break      	(GtkPlot *plot,
                                                 GtkPlotOrientation orient)
{
  if(orient == GTK_PLOT_AXIS_X){
    gtk_plot_axis_remove_break(plot->top);
    gtk_plot_axis_remove_break(plot->bottom);
  } else {
    gtk_plot_axis_remove_break(plot->left);
    gtk_plot_axis_remove_break(plot->right);
  }
}



/******************************************
 *      gtk_plot_get_axis
 *	gtk_plot_axis_set_visible
 *	gtk_plot_axis_visible
 *	gtk_plot_axis_set_title
 *	gtk_plot_axis_show_title
 *	gtk_plot_axis_hide_title
 *	gtk_plot_axis_move_title
 *	gtk_plot_axis_justify_title
 *	gtk_plot_axis_show_labels
 *	gtk_plot_axis_set_attributes
 *	gtk_plot_axis_get_attributes
 *	gtk_plot_axis_set_ticks
 *	gtk_plot_axis_set_major_ticks
 *	gtk_plot_axis_set_minor_ticks
 *	gtk_plot_axis_set_ticks_length
 *	gtk_plot_axis_show_ticks
 *	gtk_plot_axis_set_ticks_limits
 *	gtk_plot_axis_unset_ticks_limits
 *	gtk_plot_axis_labels_set_attributes
 *	gtk_plot_axis_labels_set_numbers
 ******************************************/

GtkPlotAxis *
gtk_plot_get_axis (GtkPlot *plot, GtkPlotAxisPos axis)
{
  GtkPlotAxis *aux = NULL;

  switch(axis){
    case GTK_PLOT_AXIS_LEFT:
         aux = plot->left;
         break;
    case GTK_PLOT_AXIS_RIGHT:
         aux = plot->right;
         break;
    case GTK_PLOT_AXIS_TOP:
         aux = plot->top;
         break;
    case GTK_PLOT_AXIS_BOTTOM:
         aux = plot->bottom;
         break;
  }
  return aux; 
}

void
gtk_plot_axis_set_visible (GtkPlotAxis *axis, gboolean visible)
{
  GtkPlotAxis *aux = axis;

  aux->is_visible = visible;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

gboolean
gtk_plot_axis_visible (GtkPlotAxis *axis)
{
  GtkPlotAxis *aux = axis;

  return aux->is_visible;
}


void
gtk_plot_axis_set_title (GtkPlotAxis *axis, const gchar *title)
{
  GtkPlotAxis *aux = axis;

  if(aux->title.text)
     g_free(aux->title.text);

  aux->title.text = g_strdup(title);

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_show_title (GtkPlotAxis *axis)
{
  GtkPlotAxis *aux = axis;

  aux->title_visible = TRUE;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_hide_title (GtkPlotAxis *axis)
{
  GtkPlotAxis *aux = axis;

  aux->title_visible = FALSE;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_move_title (GtkPlotAxis *axis, gint angle, gdouble x, gdouble y)
{
  GtkPlotAxis *aux = axis;

  aux->title.angle = angle;
  aux->title.x = x;
  aux->title.y = y;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_justify_title (GtkPlotAxis *axis, GtkJustification justification)
{
  GtkPlotAxis *aux = axis;

  aux->title.justification = justification;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void 
gtk_plot_axis_set_attributes (GtkPlotAxis *axis, 
			      gfloat width, const GdkColor *color)
{
  GtkPlotAxis *aux = axis;

  aux->line.line_width = width;

  aux->line.color = *color;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void 
gtk_plot_axis_get_attributes (GtkPlotAxis *axis, 
			      gfloat *width, GdkColor *color)
{
  GtkPlotAxis *aux = axis;

  *width = aux->line.line_width;
  *color = aux->line.color;
}

void
gtk_plot_axis_set_ticks (GtkPlotAxis *axis,
		         gdouble major_step,
		         gint nminor)
{
  axis->ticks.step = major_step;
  axis->ticks.nminor = nminor;
  gtk_plot_axis_ticks_recalc(axis);
  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_major_ticks (GtkPlotAxis *axis,
		               gdouble major_step)
{
  axis->ticks.step = major_step;
  gtk_plot_axis_ticks_recalc(axis);
  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_minor_ticks (GtkPlotAxis *axis,
		               gint nminor)
{
  axis->ticks.nminor = nminor;
  gtk_plot_axis_ticks_recalc(axis);
  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_ticks_length (GtkPlotAxis *axis, gint length)
{
  GtkPlotAxis *aux = axis;
  aux->ticks_length = length;
  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_ticks_width (GtkPlotAxis *axis, gfloat width)
{
  GtkPlotAxis *aux = axis;

  aux->ticks_width = width;
  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_show_ticks (GtkPlotAxis *axis,	
			  gint major_mask,
                          gint minor_mask)
{
  GtkPlotAxis *aux = axis;

  aux->major_mask = major_mask;
  aux->minor_mask = minor_mask;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_ticks_limits (GtkPlotAxis *axis,	
                          	gdouble begin, gdouble end)
{
  if(end < begin) return;

  axis->ticks.begin = begin;
  axis->ticks.end = end;
  axis->ticks.set_limits = TRUE;
  gtk_plot_axis_ticks_recalc(axis);

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_unset_ticks_limits (GtkPlotAxis *axis)
{
  axis->ticks.set_limits = FALSE;
  gtk_plot_axis_ticks_recalc(axis);

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_tick_labels  (GtkPlotAxis *axis, GtkPlotArray *labels)
{
  if(axis->tick_labels) g_object_unref(G_OBJECT(axis->tick_labels));
  axis->tick_labels = labels;

  if(labels){
    if(labels->name) g_free(labels->name);
    labels->name = g_strdup("tick_labels");
    g_object_ref(G_OBJECT(labels));
  }
}

void            
gtk_plot_axis_set_break         (GtkPlotAxis *axis,
                                 gdouble min, gdouble max,
                                 gdouble step_after, 
                                 gint nminor_after,
                                 GtkPlotScale scale_after,
                                 gdouble pos)
{
  axis->ticks.break_min = min;
  axis->ticks.break_max = max;
  axis->ticks.apply_break = TRUE;
  axis->ticks.break_step = step_after;
  axis->ticks.break_nminor = nminor_after;
  axis->ticks.break_position = pos;
  axis->ticks.break_scale = scale_after;
     
  gtk_plot_axis_ticks_recalc(axis);
  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void            
gtk_plot_axis_remove_break      (GtkPlotAxis *axis)
{
  axis->ticks.apply_break = FALSE;
  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_show_labels (GtkPlotAxis *axis, gint mask)
{
  GtkPlotAxis *aux = axis;

  aux->label_mask = mask;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}


void
gtk_plot_axis_title_set_attributes (GtkPlotAxis *axis,	
				    const gchar *font,
                                    gint height,
                                    gint angle,
			            const GdkColor *fg,
			            const GdkColor *bg,
				    gboolean transparent,
				    GtkJustification justification)
{
  GtkPlotAxis *aux = axis;

  if(!font){
   /* Use previous font */
/*    aux->title.font = g_strdup(DEFAULT_FONT);
    aux->title.height = DEFAULT_FONT_HEIGHT;
*/
  } else {
    if(aux->title.font) g_free(aux->title.font);
    aux->title.font = g_strdup(font);
    aux->title.height = height;
  }

  gdk_color_black(gdk_colormap_get_system(), &axis->title.fg);
  gdk_color_white(gdk_colormap_get_system(), &axis->title.bg);

  if(fg) aux->title.fg = *fg;
  if(bg) aux->title.bg = *bg;

  aux->title.angle = angle;
  aux->title.transparent = transparent;
  aux->title.justification = justification;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_labels_offset	    (GtkPlotAxis *axis,	
				     gint offset)
{
  GtkPlotAxis *aux = axis;

  aux->labels_offset = offset;
  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

gint
gtk_plot_axis_get_labels_offset	    (GtkPlotAxis *axis)
{
  GtkPlotAxis *aux = axis;

  return(aux->labels_offset);
}

void
gtk_plot_axis_set_labels_attributes (GtkPlotAxis *axis,	
				     const gchar *font,
                                     gint height,
                                     gint angle,
			             const GdkColor *fg,
			             const GdkColor *bg,
				     gboolean transparent,
				     GtkJustification justification)
{
  GtkPlotAxis *aux = axis;

  if(!font){
   /* Use previous font */
/*    aux->labels_attr.font = g_strdup(DEFAULT_FONT);
    aux->labels_attr.height = DEFAULT_FONT_HEIGHT;
*/
  } else {
    if(aux->labels_attr.font) g_free(aux->labels_attr.font);
    aux->labels_attr.font = g_strdup(font);
    aux->labels_attr.height = height;
  }

  aux->labels_attr.angle = angle;

  gdk_color_black(gdk_colormap_get_system(), &axis->labels_attr.fg);
  gdk_color_white(gdk_colormap_get_system(), &axis->labels_attr.bg);

  if(fg) aux->labels_attr.fg = *fg;
  if(bg) aux->labels_attr.bg = *bg;

  aux->labels_attr.transparent = transparent;
  aux->labels_attr.justification = justification;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_labels_style   (GtkPlotAxis *axis,	
              		          GtkPlotLabelStyle style,
              		          gint precision)
{
  GtkPlotAxis *aux = axis;

  aux->label_style = style;
  aux->label_precision = precision;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_use_custom_tick_labels (GtkPlotAxis *axis,
                                      gboolean use)
{
  axis->custom_labels = use;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_labels_prefix (GtkPlotAxis *axis,
				 const gchar *text)
{
  if(axis->labels_prefix) g_free(axis->labels_prefix);

  if(text)
    axis->labels_prefix = g_strdup(text);
  else 
    axis->labels_prefix = NULL;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

void
gtk_plot_axis_set_labels_suffix (GtkPlotAxis *axis,
				 const gchar *text)
{
  if(axis->labels_suffix) g_free(axis->labels_suffix);

  if(text)
    axis->labels_suffix = g_strdup(text);
  else 
    axis->labels_suffix = NULL;

  gtk_signal_emit (GTK_OBJECT(axis), axis_signals[AXIS_CHANGED]);
}

gchar *
gtk_plot_axis_get_labels_prefix (GtkPlotAxis *axis)
{
  return (axis->labels_prefix);
}

gchar *
gtk_plot_axis_get_labels_suffix (GtkPlotAxis *axis)
{
  return (axis->labels_suffix);
}

/******************************************
 *      gtk_plot_x0line_set_attributes
 *      gtk_plot_y0line_set_attributes
 *      gtk_plot_major_vgrid_set_attributes
 *      gtk_plot_minor_vgrid_set_attributes
 *      gtk_plot_major_hgrid_set_attributes
 *      gtk_plot_minor_hgrid_set_attributes
 ******************************************/

void 
gtk_plot_x0_set_visible(GtkPlot *plot, gboolean visible)
{
  plot->show_x0 = visible;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

gboolean 
gtk_plot_x0_visible(GtkPlot *plot)
{
  return plot->show_x0;
}

void 
gtk_plot_y0_set_visible(GtkPlot *plot, gboolean visible)
{
  plot->show_y0 = visible;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

gboolean 
gtk_plot_y0_visible(GtkPlot *plot)
{
  return plot->show_y0;
}

void 
gtk_plot_grids_set_on_top(GtkPlot *plot, gboolean on_top)
{
  plot->grids_on_top = on_top;
}

gboolean 
gtk_plot_grids_on_top(GtkPlot *plot)
{
  return(plot->grids_on_top);
}

void 
gtk_plot_grids_set_visible(GtkPlot *plot, 
                           gboolean vmajor, gboolean vminor,
                           gboolean hmajor, gboolean hminor)
{
  plot->bottom->show_major_grid = vmajor;
  plot->bottom->show_minor_grid = vminor;
  plot->left->show_major_grid = hmajor;
  plot->left->show_minor_grid = hminor;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}


void 
gtk_plot_grids_visible(GtkPlot *plot, 
                           gboolean *vmajor, gboolean *vminor,
                           gboolean *hmajor, gboolean *hminor)
{
  *vmajor = plot->bottom->show_major_grid;
  *vminor = plot->bottom->show_minor_grid;
  *hmajor = plot->left->show_major_grid;
  *hminor = plot->left->show_minor_grid;
}

void 
gtk_plot_x0line_set_attributes(GtkPlot *plot, 
                               GtkPlotLineStyle line_style,
                               gfloat width,
                               const GdkColor *color)
{
  plot->x0_line.line_style = line_style;
  plot->x0_line.line_width = width;
  if(color) plot->x0_line.color = *color;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void 
gtk_plot_y0line_set_attributes(GtkPlot *plot, 
                               GtkPlotLineStyle line_style,
                               gfloat width,
                               const GdkColor *color)
{
  plot->y0_line.line_style = line_style;
  plot->y0_line.line_width = width;
  if(color) plot->y0_line.color = *color;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void 
gtk_plot_major_vgrid_set_attributes(GtkPlot *plot, 
                                    GtkPlotLineStyle line_style,
                                    gfloat width,
                                    const GdkColor *color)
{
  plot->bottom->major_grid.line_style = line_style;
  plot->bottom->major_grid.line_width = width;
  if(color) plot->bottom->major_grid.color = *color;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void 
gtk_plot_minor_vgrid_set_attributes(GtkPlot *plot, 
                                    GtkPlotLineStyle line_style,
                                    gfloat width,
                                    const GdkColor *color)
{
  plot->bottom->minor_grid.line_style = line_style;
  plot->bottom->minor_grid.line_width = width;
  if(color) plot->bottom->minor_grid.color = *color;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void 
gtk_plot_major_hgrid_set_attributes(GtkPlot *plot, 
                                    GtkPlotLineStyle line_style,
                                    gfloat width,
                                    const GdkColor *color)
{
  plot->left->major_grid.line_style = line_style;
  plot->left->major_grid.line_width = width;
  if(color) plot->left->major_grid.color = *color;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void 
gtk_plot_minor_hgrid_set_attributes(GtkPlot *plot, 
                                    GtkPlotLineStyle line_style,
                                    gfloat width,
                                    const GdkColor *color)
{
  plot->left->minor_grid.line_style = line_style;
  plot->left->minor_grid.line_width = width;
  if(color) plot->left->minor_grid.color = *color;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

/******************************************
 * gtk_plot_show_legends
 * gtk_plot_hide_legends
 * gtk_plot_show_legends_border
 * gtk_plot_hide_legends_border
 * gtk_plot_legends_move
 * gtk_plot_legends_get_position
 * gtk_plot_legends_get_allocation
 * gtk_plot_set_legends_attributes
 ******************************************/

void
gtk_plot_show_legends(GtkPlot *plot)
{
  plot->show_legends = TRUE;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}


void
gtk_plot_hide_legends(GtkPlot *plot)
{
  plot->show_legends = FALSE;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_set_legends_border(GtkPlot *plot, 
                            GtkPlotBorderStyle legends_border, 
                            gint shadow_width)
{
  plot->legends_border = legends_border;
  plot->legends_shadow_width = shadow_width;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_legends_move(GtkPlot *plot, gdouble x, gdouble y)
{
  plot->legends_x = x;
  plot->legends_y = y;

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_legends_get_position(GtkPlot *plot, gdouble *x, gdouble *y)
{
  *x = plot->legends_x;
  *y = plot->legends_y;
}

GtkAllocation
gtk_plot_legends_get_allocation(GtkPlot *plot)
{
  GtkAllocation allocation;
  GtkWidget *widget;
  GList *datasets;
  gdouble x, y, width, height;
  gdouble m;

  widget = GTK_WIDGET(plot);
  m = plot->magnification;

  x = widget->allocation.x + plot->x * widget->allocation.width +
      plot->legends_x * plot->width * widget->allocation.width; 
  y = widget->allocation.y + plot->y * widget->allocation.height +
      plot->legends_y * plot->height * widget->allocation.height; 

  width = 24 * m;
  height = 8 * m;

  datasets = g_list_first(plot->data_sets);
  while(datasets)
   {
     GtkPlotData *dataset;
     gint lwidth, lheight;

     dataset = GTK_PLOT_DATA(datasets->data);
 
     if(GTK_WIDGET_VISIBLE(GTK_WIDGET(dataset)) && dataset->show_legend)
       { 
         GTK_PLOT_DATA_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(dataset)))->get_legend_size(dataset, &lwidth, &lheight);
         width = MAX(width, lwidth); 
         height += lheight;
       }

     datasets = datasets->next;
   }

  allocation.x = roundint(x);
  allocation.y = roundint(y);
  allocation.width = roundint(width);
  allocation.height = roundint(height);

  return(allocation);
}

void
gtk_plot_legends_set_attributes(GtkPlot *plot, const gchar *font, gint height, 
			        const GdkColor *foreground, const GdkColor *background)
{

  g_free(plot->legends_attr.font);

  if(!font) {
    plot->legends_attr.font = g_strdup(DEFAULT_FONT);
    plot->legends_attr.height = DEFAULT_FONT_HEIGHT;
  } else {
    plot->legends_attr.font = g_strdup(font);
    plot->legends_attr.height = height;
  }
  plot->legends_attr.fg = GTK_WIDGET(plot)->style->black;
  plot->legends_attr.bg = GTK_WIDGET(plot)->style->white;

  if(foreground != NULL)
    plot->legends_attr.fg = *foreground;

  plot->legends_attr.transparent = TRUE;
  if(background != NULL){
    plot->legends_attr.bg = *background;
    plot->legends_attr.transparent = FALSE;
  }

  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

void
gtk_plot_add_data(GtkPlot *plot,
                  GtkPlotData *dataset)
{
  gboolean veto = TRUE;

  _gtkextra_signal_emit (GTK_OBJECT(plot), plot_signals[ADD_DATA], dataset, &veto);
  plot->data_sets = g_list_append(plot->data_sets, dataset);
  gtk_widget_ref(GTK_WIDGET(dataset));
  gtk_object_sink(GTK_OBJECT(dataset));

  dataset->plot = plot;

  gtk_signal_emit_by_name (GTK_OBJECT(dataset), "add_to_plot", plot, &veto);
  gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
}

GtkPlotData *
gtk_plot_add_function(GtkPlot *plot, GtkPlotFunc function)
{
  GtkPlotData *dataset;

  dataset = GTK_PLOT_DATA(gtk_plot_data_new_function(function));

  gtk_plot_add_data(plot, dataset);

  return (dataset);
}

gint
gtk_plot_remove_data(GtkPlot *plot, GtkPlotData *dataset)
{
  GList *datasets;
  gpointer data;

  datasets = plot->data_sets;

  while(datasets)
   {
     data = datasets->data;
     
     if(GTK_PLOT_DATA(data) == dataset){
          gtk_widget_unref(GTK_WIDGET(dataset));
          plot->data_sets = g_list_remove_link(plot->data_sets, datasets);
          g_list_free_1(datasets);
          gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
	  return TRUE;
     }
     datasets = datasets->next;
   }

  return FALSE;
}

gint
gtk_plot_remove_text(GtkPlot *plot, GtkPlotText *text)
{
  GList *list;
  gpointer data;

  list = plot->text;

  while(list)
   {
     data = list->data;
     
     if((GtkPlotText *)data == text){
              plot->text = g_list_remove_link(plot->text, list);
              g_list_free_1(list);
              gtk_signal_emit (GTK_OBJECT(plot), plot_signals[CHANGED]);
	      return TRUE;
     }
     list = list->next;
   }

   return FALSE;
}


static void
gtk_plot_real_ticks_recalc(GtkPlotTicks *ticks)
{
  GtkPlotScale scale;
  gdouble min = 0., max = 0.;
  gdouble tick = 0.0;
  gdouble tick_step = 0.0;
  gdouble major_step;
  gdouble minor_step;
  gint nmajor = 0;
  gint n = 0;
  gdouble absmin, absmax;
  GtkPlotTick *major = NULL;
  gboolean changed = TRUE;
 
  scale = ticks->scale;

  max = ticks->max;
  min = ticks->min;

  absmin = min;
  absmax = max;

  if(ticks->set_limits){
       max = MIN(max, ticks->end);
       min = MAX(min, ticks->begin);
       absmin = min;
       absmax = max;
  } else {
/*
printf("%f\n",ticks->step);
printf("%f %f\n",min/ticks->step,floor(min/ticks->step));
printf("%f %f\n",max/ticks->step,ceil(max/ticks->step));
*/
       max = ceil(max/ticks->step) * ticks->step;
       min = floor(min/ticks->step) * ticks->step;
  }

  if(scale == GTK_PLOT_SCALE_LOG10){
    min = ticks->min;
    max = ticks->max;
    if(max <= 0.0f) ticks->max = max = 1.E-11;
    if(max <= 0) ticks->max = max = fabs(max);
    if(min <= 0) ticks->min = min = max / 100.;
    min = floor(log10(min));
    min = pow(10., min);
    absmin = ticks->min;
    absmax = ticks->max;
  }

  if(ticks->values){
     g_free(ticks->values);
     ticks->values = NULL;
  }

  ticks->nmajorticks = 0; 
  ticks->nminorticks = 0; 
  major_step = ticks->step;
  minor_step = major_step / ((gdouble)ticks->nminor + 1.0);

  if(scale == GTK_PLOT_SCALE_LOG10){
     if(major_step != 0.) 
         major_step = floor(major_step);

     if(major_step == 0.)
         major_step = 1.0;

     ticks->nminor = 8;
  }

  if(ticks->step > 0.){
   major = g_new0(GtkPlotTick,1);
   tick_step = min;
   tick = min;
   n = 0;
   while(tick <= max + 2*fabs(major_step)){
     if(tick >= absmin-major_step*1.E-10 && tick <= absmax+major_step*1.E-10){
        nmajor ++;
        major = g_realloc(major, nmajor*sizeof(GtkPlotTick)); 
        major[nmajor-1].value = tick;
        major[nmajor-1].minor = FALSE;
     }
     switch(scale){
        case GTK_PLOT_SCALE_LINEAR:
            tick += major_step; 
            break;
        case GTK_PLOT_SCALE_LOG10:
            n++;
            tick = tick_step * pow(10., n*major_step); 
            break;
     }
   }
  }

  /* jc: commented out

	if(nmajor == 0) return;

     can leave ticks->values NULL with this

   */

  ticks->nticks = 0;
  ticks->nmajorticks = nmajor;
  ticks->values = g_new0(GtkPlotTick, 1);

  if(ticks->step >0.){
   gint i;
   n = 0;
   for(nmajor = 0; nmajor <= ticks->nmajorticks; nmajor++){
    if(nmajor < ticks->nmajorticks){
      n++;
      ticks->values = g_realloc(ticks->values, n*sizeof(GtkPlotTick));
      ticks->values[n-1] = major[nmajor];
    }
    switch(scale){
      case GTK_PLOT_SCALE_LINEAR:
        if(nmajor < ticks->nmajorticks)
          tick_step = major[nmajor].value - major_step;
        else
          tick_step = major[nmajor-1].value;
        tick = tick_step;
        break;
      case GTK_PLOT_SCALE_LOG10:
        if(nmajor < ticks->nmajorticks)
          tick_step = major[nmajor].value/10.0;
        else
          tick_step = major[nmajor-1].value;
        tick = tick_step;
        break;
    }
    for(i = 0; i < ticks->nminor; i++){
     switch(scale){
        case GTK_PLOT_SCALE_LINEAR:
            tick += minor_step; 
            break;
        case GTK_PLOT_SCALE_LOG10:
            tick += tick_step; 
            break;
     }   
     if(tick >= absmin-major_step*1.E-10 && tick <= absmax+major_step*1.E-10){
        n++;
        ticks->values = g_realloc(ticks->values, n*sizeof(GtkPlotTick));
        ticks->values[n-1].value = tick;
        ticks->values[n-1].minor = TRUE;
        ticks->nminorticks++;
     }
    }
   }
  }

  ticks->nticks = n;

  if(major) g_free(major);

  /* sorting ticks */

  while(changed){
    gint i;
    changed = FALSE;
    for(i = 0; i < ticks->nticks - 1; i++){
      if(ticks->values[i].value > ticks->values[i+1].value) { 
          GtkPlotTick aux = ticks->values[i]; 
          ticks->values[i] = ticks->values[i+1]; 
          ticks->values[i+1] = aux; 
          changed = TRUE; 
      }
    }
  }

/*
  ticks->values[0].value = absmin;
  ticks->values[n-1].value = absmax;
*/
}

void
gtk_plot_ticks_recalc(GtkPlotAxis *axis)
{
  GtkPlotTicks *ticks = &axis->ticks;

  if(ticks->apply_break){
    GtkPlotTicks a1, a2;
    gint i;

    a1.scale = ticks->scale;
    a1.step = ticks->step;
    a1.begin = ticks->begin;
    a1.end = ticks->end;
    a1.nminor = ticks->nminor;
    a1.set_limits = ticks->set_limits;
    a1.min = ticks->min;
    a1.max = ticks->break_min;
    a1.values = NULL;
    gtk_plot_real_ticks_recalc(&a1);

    a2.scale = ticks->break_scale;
    a2.step = ticks->break_step;
    a2.begin = ticks->begin;
    a2.end = ticks->end;
    a2.nminor = ticks->break_nminor;
    a2.set_limits = ticks->set_limits;
    a2.min = ticks->break_max;
    a2.max = ticks->max;
    a2.values = NULL;
    gtk_plot_real_ticks_recalc(&a2);

    if(ticks->values){
      g_free(ticks->values);
      ticks->values = NULL;
    }
    ticks->nmajorticks = a1.nmajorticks+a2.nmajorticks;
    ticks->nminorticks = a1.nminorticks+a2.nminorticks;
    ticks->nticks = a1.nticks+a2.nticks;
    ticks->values = g_new0(GtkPlotTick, ticks->nticks);
    for(i = 0; i < a1.nticks; i++)
      ticks->values[i] = a1.values[i];
    for(i = 0; i < a2.nticks; i++)
      ticks->values[a1.nticks+i] = a2.values[i];

    if(a1.values) g_free(a1.values);
    if(a2.values) g_free(a2.values);
  } else {
    gtk_plot_real_ticks_recalc(ticks);
  }
}

static void
update_datasets(GtkPlot *plot, gboolean new_range)
{
  GList *list = NULL;

  list = plot->data_sets;
  while(list) {
    gtk_signal_emit_by_name(GTK_OBJECT(list->data), "update", new_range);

    list = list->next;
  }
}

