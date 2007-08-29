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
#ifndef __GTK_PLOT_H__
#define __GTK_PLOT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gtkplotpc.h"
#include "gtkplotarray.h"

#define GTK_PLOT(obj)        GTK_CHECK_CAST (obj, gtk_plot_get_type (), GtkPlot)

#define GTK_TYPE_PLOT   (gtk_plot_get_type ())

#define GTK_PLOT_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_get_type(), GtkPlotClass)
#define GTK_IS_PLOT(obj)     GTK_CHECK_TYPE (obj, gtk_plot_get_type ())
#define GTK_PLOT_AXIS(obj)        GTK_CHECK_CAST (obj, gtk_plot_axis_get_type (), GtkPlotAxis)

#define GTK_TYPE_PLOT_AXIS   (gtk_plot_axis_get_type ())
#define GTK_IS_PLOT_AXIS(obj)     GTK_CHECK_TYPE (obj, gtk_plot_axis_get_type ())

typedef struct _GtkPlot		GtkPlot;
typedef struct _GtkPlotClass	GtkPlotClass;
typedef struct _GtkPlotData		GtkPlotData;
typedef struct _GtkPlotDataClass	GtkPlotDataClass;
typedef struct _GtkPlotAxis 	GtkPlotAxis;
typedef struct _GtkPlotAxisClass 	GtkPlotAxisClass;
typedef struct _GtkPlotText 	GtkPlotText;
typedef struct _GtkPlotLine 	GtkPlotLine;
typedef struct _GtkPlotSymbol 	GtkPlotSymbol;
typedef struct _GtkPlotTick	GtkPlotTick;
typedef struct _GtkPlotTicks	GtkPlotTicks;
typedef struct _GtkPlotVector   GtkPlotVector;
typedef struct _GtkPlotMarker   GtkPlotMarker;


typedef gdouble (*GtkPlotFunc)		(GtkPlot *plot, 
					 GtkPlotData *data, 
					 gdouble x, 
					 gboolean *error);

typedef gdouble (*GtkPlotFunc3D)	(GtkPlot *plot, 
					 GtkPlotData *data, 
					 gdouble x, gdouble y, 
					 gboolean *error);

typedef void   	(*GtkPlotIterator)	(GtkPlot *plot,
					 GtkPlotData *data, 
					 gint iter, 
                                         gdouble *x, 
					 gdouble *y, 
					 gdouble *z,
					 gdouble *a,
                                         gdouble *dx, 
					 gdouble *dy, 
					 gdouble *dz,
					 gdouble *da,
					 gchar **label,
					 gboolean *error);

enum
{
  GTK_PLOT_DATA_X 	= 1 << 0,
  GTK_PLOT_DATA_Y 	= 1 << 1,
  GTK_PLOT_DATA_Z 	= 1 << 2,
  GTK_PLOT_DATA_A 	= 1 << 3,
  GTK_PLOT_DATA_DX 	= 1 << 4,
  GTK_PLOT_DATA_DY 	= 1 << 5,
  GTK_PLOT_DATA_DZ 	= 1 << 6,
  GTK_PLOT_DATA_DA 	= 1 << 7,
  GTK_PLOT_DATA_LABELS 	= 1 << 8,
};

typedef enum
{
  GTK_PLOT_SCALE_LINEAR	,
  GTK_PLOT_SCALE_LOG10		
} GtkPlotScale;

typedef enum
{
  GTK_PLOT_SYMBOL_NONE		,
  GTK_PLOT_SYMBOL_SQUARE	,
  GTK_PLOT_SYMBOL_CIRCLE	,
  GTK_PLOT_SYMBOL_UP_TRIANGLE	,
  GTK_PLOT_SYMBOL_DOWN_TRIANGLE	,
  GTK_PLOT_SYMBOL_RIGHT_TRIANGLE	,
  GTK_PLOT_SYMBOL_LEFT_TRIANGLE	,
  GTK_PLOT_SYMBOL_DIAMOND	,
  GTK_PLOT_SYMBOL_PLUS		,
  GTK_PLOT_SYMBOL_CROSS		,
  GTK_PLOT_SYMBOL_STAR		,
  GTK_PLOT_SYMBOL_DOT	 	,
  GTK_PLOT_SYMBOL_IMPULSE 	,
} GtkPlotSymbolType;

typedef enum
{
  GTK_PLOT_SYMBOL_EMPTY		,
  GTK_PLOT_SYMBOL_FILLED	,
  GTK_PLOT_SYMBOL_OPAQUE	
} GtkPlotSymbolStyle;

typedef enum
{
  GTK_PLOT_BORDER_NONE		,
  GTK_PLOT_BORDER_LINE		,
  GTK_PLOT_BORDER_SHADOW	,
} GtkPlotBorderStyle;

typedef enum
{
  GTK_PLOT_LINE_NONE		,
  GTK_PLOT_LINE_SOLID		,
  GTK_PLOT_LINE_DOTTED		,
  GTK_PLOT_LINE_DASHED		,
  GTK_PLOT_LINE_DOT_DASH	,
  GTK_PLOT_LINE_DOT_DOT_DASH	,
  GTK_PLOT_LINE_DOT_DASH_DASH	
} GtkPlotLineStyle;

typedef enum
{
  GTK_PLOT_CONNECT_NONE		,
  GTK_PLOT_CONNECT_STRAIGHT	,
  GTK_PLOT_CONNECT_SPLINE	,
  GTK_PLOT_CONNECT_HV_STEP	,
  GTK_PLOT_CONNECT_VH_STEP	,
  GTK_PLOT_CONNECT_MIDDLE_STEP	
} GtkPlotConnector;

typedef enum
{
  GTK_PLOT_LABEL_NONE    	= 0,
  GTK_PLOT_LABEL_IN 	   	= 1 << 0,
  GTK_PLOT_LABEL_OUT    	= 1 << 1,
}GtkPlotLabelPos;

typedef enum
{
  GTK_PLOT_ERROR_DIV_ZERO,
  GTK_PLOT_ERROR_LOG_NEG
} GtkPlotError;

typedef enum
{
  GTK_PLOT_AXIS_X	,
  GTK_PLOT_AXIS_Y	,
  GTK_PLOT_AXIS_Z	,
} GtkPlotOrientation;

typedef enum
{
  GTK_PLOT_AXIS_LEFT	,
  GTK_PLOT_AXIS_RIGHT	,
  GTK_PLOT_AXIS_TOP	,
  GTK_PLOT_AXIS_BOTTOM	
} GtkPlotAxisPos;

typedef enum
{
  GTK_PLOT_LABEL_FLOAT	,
  GTK_PLOT_LABEL_EXP	,
  GTK_PLOT_LABEL_POW	
} GtkPlotLabelStyle;

typedef enum
{
  GTK_PLOT_TICKS_NONE		= 0,
  GTK_PLOT_TICKS_IN		= 1 << 0,
  GTK_PLOT_TICKS_OUT		= 1 << 1
} GtkPlotTicksPos;

enum
{
  GTK_PLOT_GRADIENT_H		= 1 << 0,
  GTK_PLOT_GRADIENT_V		= 1 << 1,
  GTK_PLOT_GRADIENT_S		= 1 << 2,
};

struct _GtkPlotMarker
{
  GtkPlotData *data;
  gint point;
};

struct _GtkPlotText
{
  gdouble x, y;
  gint angle; /* 0, 90, 180, 270 */
  GdkColor fg;
  GdkColor bg;
 
  gboolean transparent;
 
  GtkPlotBorderStyle border;
  gint border_width;
  gint shadow_width;
  gint border_space;

  gchar *font;
  gint height;

  gchar *text;

  GtkJustification justification;
};

struct _GtkPlotLine
{
  GtkPlotLineStyle line_style;
  GdkCapStyle cap_style;
  GdkJoinStyle join_style;

  gfloat line_width;
  GdkColor color;
};

struct _GtkPlotSymbol
{
  GtkPlotSymbolType symbol_type;
  GtkPlotSymbolStyle symbol_style;

  gint size;

  GdkColor color;
  GtkPlotLine border;
};

struct _GtkPlotVector
{
  gdouble x, y, z;
};

struct _GtkPlotTick
{
  gdouble value;
  gboolean minor;
};

struct _GtkPlotTicks
{
  gdouble min;
  gdouble max;

  GtkPlotScale scale;

  gint nmajorticks;             /* Number of major ticks drawn */
  gint nminorticks;             /* Number of minor ticks drawn */
  gint nticks;			/* total number of ticks */

  gdouble step;		   	/* major ticks step */
  gint nminor;		  	/* number of minor ticks between major ticks */

  gboolean apply_break;
  GtkPlotScale break_scale;
  gdouble break_step;
  gint break_nminor;
  gdouble break_min;
  gdouble break_max;
  gdouble break_position;

  GtkPlotTick *values;

  gboolean set_limits;
  gdouble begin, end; 
};

struct _GtkPlotAxis
{
  GtkObject object;

  gboolean is_visible;

  GtkPlotVector origin;
  GtkPlotVector direction;

  GtkPlotText title;
  gboolean title_visible;

  GtkPlotTicks ticks;

  GtkPlotOrientation orientation;
  
  GtkPlotLine line;
  GtkPlotLine major_grid;
  GtkPlotLine minor_grid;

  gint major_mask;
  gint minor_mask;
  gint ticks_length;
  gfloat ticks_width;

  gboolean custom_labels;

  gint labels_offset;

  gchar *labels_prefix;
  gchar *labels_suffix;

  gboolean show_major_grid;
  gboolean show_minor_grid;

  GtkPlotText labels_attr;

  gint label_precision;
  gint label_style;
  gint label_mask;

  GtkPlotArray *tick_labels;

  void (*ticks_recalc)   		(GtkPlotAxis *axis);
  void (*ticks_autoscale)   		(GtkPlotAxis *axis, 
					 gdouble xmin, gdouble xmax,
			     		 gint *precision);
  gdouble (*ticks_transform)   		(GtkPlotAxis *axis, gdouble y);
  gdouble (*ticks_inverse)   		(GtkPlotAxis *axis, gdouble y);
  void (*parse_label)   		(GtkPlotAxis *axis, 
					 gdouble val,
					 gint precision,
					 gint style,
					 gchar *label);
};

struct _GtkPlotData
{
  GtkWidget widget;

  gboolean is_function;
  gboolean is_iterator;

  guint16 iterator_mask;

  gboolean show_legend;
  gboolean show_labels;
  gboolean fill_area;

  gchar *name;
  gchar *legend;

  GtkPlot *plot;

  GtkPlotSymbol symbol;
  GtkPlotLine line;
  GtkPlotConnector line_connector;

  GtkPlotLine x_line;
  GtkPlotLine y_line;
  GtkPlotLine z_line;

  gboolean show_xerrbars;
  gint xerrbar_width;
  gint xerrbar_caps;

  gboolean show_yerrbars;
  gint yerrbar_width;
  gint yerrbar_caps;

  gboolean show_zerrbars;
  gint zerrbar_width;
  gint zerrbar_caps;

  gint num_points;

  GtkPlotArrayList *data;

  gint labels_offset;
  GtkPlotText labels_attr;

  GtkPlotFunc function;
  GtkPlotFunc3D function3d;
  GtkPlotIterator iterator;

  gdouble x_step;
  gdouble y_step;
  gdouble z_step;

  GdkColor color_min, color_max;
  GdkColor color_lt_min, color_gt_max;
  gint gradient_mask;
  GtkPlotAxis * gradient;
  gboolean gradient_custom;
  GdkColor *gradient_colors;

  gboolean show_gradient;
  gint legends_precision;
  GtkPlotLabelStyle legends_style;

  gpointer link;

  GList *markers;
  gboolean show_markers;

  gboolean redraw_pending;

  GtkPlotAxisPos gradient_title_pos;
  gint gradient_line_width;
  gint gradient_line_height;
  gdouble gradient_x, gradient_y;

  GtkPlotBorderStyle gradient_border;
  gint gradient_border_offset;
  gint gradient_border_width;
  gint gradient_shadow_width;
};

struct _GtkPlot
{
  GtkWidget widget;

  GtkAllocation internal_allocation;

  GdkDrawable *drawable;

  GdkPixmap *bg_pixmap;
  gboolean use_pixmap;

  gboolean transparent;

  gdouble magnification;

  gboolean clip_data;

  GdkColor background;

  gboolean grids_on_top;
  gboolean show_x0;
  gboolean show_y0;

 /* location and size in percentage of the widget's size */
  gdouble x, y, width, height;

  gdouble xmin, xmax;
  gdouble ymin, ymax;

  GtkPlotScale xscale, yscale;

  GtkPlotAxis *bottom; 
  GtkPlotAxis *top; 
  GtkPlotAxis *left; 
  GtkPlotAxis *right; 

  gboolean reflect_x;
  gboolean reflect_y;

  gfloat bottom_align;
  gfloat top_align;
  gfloat left_align;
  gfloat right_align;

  GtkPlotLine x0_line;
  GtkPlotLine y0_line;

  gdouble legends_x, legends_y; /* position in % */
  gint legends_width, legends_height; /* absolute size */

  GtkPlotBorderStyle legends_border;
  gint legends_line_width;  
  gint legends_border_width;
  gint legends_shadow_width;
  gboolean show_legends;
  GtkPlotText legends_attr;

  GList *data_sets;
  GList *text;

  GtkPlotPC *pc;
};

struct _GtkPlotClass
{
  GtkWidgetClass parent_class;

  void		(* set_pc)		(GtkPlot *plot, GtkPlotPC *pc);
  void		(* set_drawable)	(GtkPlot *plot, GdkDrawable *drawable);
  gboolean	(* add_data)   		(GtkPlot *plot, GtkPlotData *data);
  void		(* update)   		(GtkPlot *plot, gboolean new_range);
  void 		(* changed) 		(GtkPlot *plot);

  gboolean 	(* moved)   		(GtkPlot *plot, 
					 gdouble x, gdouble y);

  gboolean 	(* resized) 		(GtkPlot *plot,
                                         gdouble width, gdouble height);

  void 		(* plot_paint)   	(GtkWidget *plot); 

  void 		(* draw_legends)   	(GtkWidget *plot); 

  void 		(* get_pixel)   	(GtkWidget *plot, 
                	                 gdouble x, gdouble y,
                         	         gdouble *px, gdouble *py);

  void 		(* get_point)   	(GtkWidget *plot, 
                	                 gint px, gint py,
                               		 gdouble *x, gdouble *y);

/*
  void 		(* error) 		(GtkPlot *plot, gint errno);
*/
};

struct _GtkPlotDataClass
{
  GtkWidgetClass parent_class;

  gboolean	(* add_to_plot) 	(GtkPlotData *data, GtkPlot *plot);
  void		(* update) 		(GtkPlotData *data, gboolean new_range);
  void		(* gradient_changed) 	(GtkPlotData *data);
  void		(* gradient_colors_changed) 	(GtkPlotData *data);
  void 		(* draw_data)   	(GtkPlotData *data);
  void 		(* draw_symbol)   	(GtkPlotData *data, 
                                         gdouble x, 
					 gdouble y, 
					 gdouble z,
					 gdouble a,
                                         gdouble dx, 
					 gdouble dy, 
					 gdouble dz,
					 gdouble da);
  void 		(* draw_legend)   	(GtkPlotData *data, gint x, gint y);
  void 		(* get_legend_size)   	(GtkPlotData *data, 
                                         gint *width, gint *height);
  void 		(* draw_gradient)   	(GtkPlotData *data);
  void 		(* get_gradient_size)  	(GtkPlotData *data, 
                                         gint *width, gint *height);

  void		(* clone)		(GtkPlotData *data, GtkPlotData *copy);
/*
  void 		(* error) 		(GtkPlotData *data, gint errno);
*/
};

struct _GtkPlotAxisClass
{
  GtkObjectClass parent_class;

  gboolean 	(* tick_label)   	(GtkPlotAxis *axis, 
                                         gdouble *tick,
					 gchar *label);
  gboolean 	(* changed)   		(GtkPlotAxis *axis);
};


/* Plot */

GtkType		gtk_plot_get_type		(void);
GtkWidget*	gtk_plot_new			(GdkDrawable *drawable);
GtkWidget*	gtk_plot_new_with_size		(GdkDrawable *drawable,
                                                 gdouble width, gdouble height);
void		gtk_plot_construct		(GtkPlot *plot, 
     						 GdkDrawable *drawable);
void		gtk_plot_construct_with_size	(GtkPlot *plot,
						 GdkDrawable *drawable,
                                                 gdouble width, gdouble height);
void		gtk_plot_set_drawable		(GtkPlot *plot,
						 GdkDrawable *drawable);
GdkDrawable *	gtk_plot_get_drawable		(GtkPlot *plot);
void		gtk_plot_set_pc			(GtkPlot *plot,
						 GtkPlotPC *pc);
void		gtk_plot_set_background_pixmap	(GtkPlot *plot,
						 GdkPixmap *pixmap);
void		gtk_plot_set_transparent	(GtkPlot *plot, 
						 gboolean transparent);
gboolean	gtk_plot_is_transparent		(GtkPlot *plot); 
void		gtk_plot_get_position		(GtkPlot *plot,
						 gdouble *x, gdouble *y);
void		gtk_plot_get_size		(GtkPlot *plot,
						 gdouble *width, 
					  	 gdouble *height);
GtkAllocation 	gtk_plot_get_internal_allocation(GtkPlot *plot);
void		gtk_plot_set_background		(GtkPlot *plot, 
						 const GdkColor *background);
void 		gtk_plot_paint                  (GtkPlot *plot);
void		gtk_plot_refresh		(GtkPlot *plot,
						 GdkRectangle *area);
void		gtk_plot_move		        (GtkPlot *plot,
						 gdouble x, gdouble y);
void		gtk_plot_resize		        (GtkPlot *plot,
						 gdouble width, gdouble height);
void		gtk_plot_set_magnification      (GtkPlot *plot,
						 gdouble magnification);
void		gtk_plot_move_resize		(GtkPlot *plot,
						 gdouble x, gdouble y,
						 gdouble width, gdouble height);
void		gtk_plot_get_pixel		(GtkPlot *plot,
                                                 gdouble xx, gdouble yy,
                                                 gdouble *x, gdouble *y);
void		gtk_plot_get_point		(GtkPlot *plot,
                                                 gint x, gint y,
                                                 gdouble *xx, gdouble *yy);
void		gtk_plot_clip_data		(GtkPlot *plot, gboolean clip);
void		gtk_plot_set_xrange		(GtkPlot *plot,
						 gdouble xmin, gdouble xmax);
void		gtk_plot_set_yrange		(GtkPlot *plot,
						 gdouble ymin, gdouble ymax);
void		gtk_plot_set_range		(GtkPlot *plot,
						 gdouble xmin, gdouble xmax,
						 gdouble ymin, gdouble ymax);
void		gtk_plot_autoscale		(GtkPlot *plot);
void		gtk_plot_get_xrange		(GtkPlot *plot,
						 gdouble *xmin, gdouble *xmax);
void		gtk_plot_get_yrange		(GtkPlot *plot,
						 gdouble *ymin, gdouble *ymax);
void 		gtk_plot_set_xscale		(GtkPlot *plot,
						 GtkPlotScale scale_type);
void 		gtk_plot_set_yscale		(GtkPlot *plot,
						 GtkPlotScale scale_type);
GtkPlotScale 	gtk_plot_get_xscale		(GtkPlot *plot);
GtkPlotScale 	gtk_plot_get_yscale		(GtkPlot *plot);
void	 	gtk_plot_reflect_x		(GtkPlot *plot, 
						 gboolean reflect);
void	 	gtk_plot_reflect_y		(GtkPlot *plot, 
						 gboolean reflect);
gboolean 	gtk_plot_is_x_reflected		(GtkPlot *plot);
gboolean 	gtk_plot_is_y_reflected		(GtkPlot *plot);
GtkPlotText *	gtk_plot_put_text		(GtkPlot *plot,
						 gdouble x, gdouble y, 
						 const gchar *font,	
             					 gint height,
             					 gint angle,
						 const GdkColor *foreground,
						 const GdkColor *background,
						 gboolean transparent,
						 GtkJustification justification,
                                                 const gchar *text); 
gint 		gtk_plot_remove_text		(GtkPlot *plot,
						 GtkPlotText *text);

void		gtk_plot_text_get_size		(const gchar *text, 
						 gint angle,
						 const gchar *font_name, 
						 gint font_size, 
						 gint *width, gint *height,
						 gint *ascent, gint *descent);
void		gtk_plot_text_get_area		(const gchar *text,
						 gint angle,
						 GtkJustification just,
						 const gchar *font_name,
						 gint font_size,
						 gint *x, gint *y,
						 gint *width, gint *height);
void 		gtk_plot_text_set_attributes 	(GtkPlotText *text_attr,
                              			 const gchar *font,
                             			 gint height,
                              			 gint angle,
                              			 const GdkColor *fg,
                              			 const GdkColor *bg,
                              			 gboolean transparent,
                              			 GtkJustification justification,
                              			 const gchar *text);
void 		gtk_plot_text_set_border 	(GtkPlotText *text,
                        			 GtkPlotBorderStyle border,
                          			 gint border_space,
                          			 gint border_width,
                          			 gint shadow_width);
void            gtk_plot_draw_line		(GtkPlot *plot,
                			         GtkPlotLine line,
                   				 gdouble x1, gdouble y1, 
						 gdouble x2, gdouble y2);
void 		gtk_plot_draw_text		(GtkPlot *plot,
                   				 GtkPlotText text);
void            gtk_plot_set_ticks         	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation,                                                 gdouble major_step,
                                                 gint nminor);
void            gtk_plot_set_major_ticks   	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation,                                                 gdouble major_step);
void            gtk_plot_set_minor_ticks   	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation,                                                 gint nminor);
void            gtk_plot_set_ticks_limits  	(GtkPlot *plot,
                                                 GtkPlotOrientation orientation,                                                 gdouble begin, gdouble end);
void            gtk_plot_unset_ticks_limits(GtkPlot *plot,
                                                 GtkPlotOrientation orientation);
void            gtk_plot_set_break         	(GtkPlot *plot,
                                                 GtkPlotOrientation orient,
                                                 gdouble min, gdouble max,
                                                 gdouble step_after,
                                                 gint nminor_after,
                                                 GtkPlotScale scale_after,
                                                 gdouble pos);
void            gtk_plot_remove_break      	(GtkPlot *plot,
                                                 GtkPlotOrientation orient);

/* Axis */

GtkType		gtk_plot_axis_get_type		(void);
GtkObject*	gtk_plot_axis_new		(GtkPlotOrientation orientation);
void		gtk_plot_axis_construct		(GtkPlotAxis *axis, 
					         GtkPlotOrientation orientation);
GtkPlotAxis *   gtk_plot_get_axis               (GtkPlot *plot, 
                                                 GtkPlotAxisPos axis);
void		gtk_plot_axis_set_visible	(GtkPlotAxis *axis, 
                                                 gboolean visible);
gboolean	gtk_plot_axis_visible		(GtkPlotAxis *axis);
void		gtk_plot_axis_set_title		(GtkPlotAxis *axis,
						 const gchar *title);
void		gtk_plot_axis_show_title	(GtkPlotAxis *axis); 
void		gtk_plot_axis_hide_title	(GtkPlotAxis *axis);
void		gtk_plot_axis_move_title	(GtkPlotAxis *axis, 
						 gint angle,
						 gdouble x, gdouble y);
void		gtk_plot_axis_justify_title	(GtkPlotAxis *axis,
						 GtkJustification justification);
void		gtk_plot_axis_set_attributes 	(GtkPlotAxis *axis,
						 gfloat width,
						 const GdkColor *color);
void		gtk_plot_axis_get_attributes 	(GtkPlotAxis *axis,
						 gfloat *width,
						 GdkColor *color);
void		gtk_plot_axis_set_ticks		(GtkPlotAxis *axis,
						 gdouble major_step,
						 gint nminor);
void		gtk_plot_axis_set_major_ticks	(GtkPlotAxis *axis,
						 gdouble major_step);
void		gtk_plot_axis_set_minor_ticks	(GtkPlotAxis *axis,
						 gint nminor);
void		gtk_plot_axis_set_ticks_length	(GtkPlotAxis *axis,
						 gint length);
void		gtk_plot_axis_set_ticks_width	(GtkPlotAxis *axis,
						 gfloat width);
void		gtk_plot_axis_show_ticks	(GtkPlotAxis *axis,
                                                 gint major_mask,
						 gint minor_mask);
void		gtk_plot_axis_set_ticks_limits	(GtkPlotAxis *axis,
						 gdouble begin, gdouble end);
void		gtk_plot_axis_unset_ticks_limits(GtkPlotAxis *axis);
void		gtk_plot_axis_set_tick_labels	(GtkPlotAxis *axis,
						 GtkPlotArray *array);
void		gtk_plot_axis_set_break		(GtkPlotAxis *axis,
						 gdouble min, gdouble max,
						 gdouble step_after, 
						 gint nminor_after,
                                                 GtkPlotScale scale_after,
						 gdouble pos);
void		gtk_plot_axis_remove_break	(GtkPlotAxis *axis);
void		gtk_plot_axis_show_labels	(GtkPlotAxis *axis,
						 gint labels_mask);
void		gtk_plot_axis_title_set_attributes	
						(GtkPlotAxis *axis,
					 	 const gchar *font,
					 	 gint height,
					 	 gint angle,
					 	 const GdkColor *foreground,
						 const GdkColor *background,
                                        	 gboolean transparent,
                                        	 GtkJustification justification);
void		gtk_plot_axis_set_labels_attributes	
						(GtkPlotAxis *axis,
					 	 const gchar *font,
						 gint height,
						 gint angle,
						 const GdkColor *foreground,
						 const GdkColor *background,
                                        	 gboolean transparent,
                                        	 GtkJustification justification);
#define gtk_plot_axis_set_labels_numbers gtk_plot_axis_set_labels_style
void		gtk_plot_axis_set_labels_style	(GtkPlotAxis *axis,
						 GtkPlotLabelStyle style,
						 gint precision); 
void		gtk_plot_axis_set_labels_offset	(GtkPlotAxis *axis,
						 gint offset); 
gint		gtk_plot_axis_get_labels_offset	(GtkPlotAxis *axis);
void 		gtk_plot_axis_use_custom_tick_labels 	
						(GtkPlotAxis *axis,
                                      		 gboolean use);
void		gtk_plot_axis_set_labels_suffix (GtkPlotAxis *axis,
						 const gchar *text);
void		gtk_plot_axis_set_labels_prefix (GtkPlotAxis *axis,
						 const gchar *text);
gchar *		gtk_plot_axis_get_labels_suffix (GtkPlotAxis *axis);
gchar *		gtk_plot_axis_get_labels_prefix (GtkPlotAxis *axis);

void 		gtk_plot_axis_ticks_recalc      (GtkPlotAxis *axis);
void 		gtk_plot_axis_ticks_autoscale   (GtkPlotAxis *axis,
                  	                         gdouble xmin, gdouble xmax,
                        	                 gint *precision);
gdouble 	gtk_plot_axis_ticks_transform   (GtkPlotAxis *axis, 
						 gdouble y);
gdouble 	gtk_plot_axis_ticks_inverse     (GtkPlotAxis *axis, 
						 gdouble x);
void 		gtk_plot_axis_parse_label       (GtkPlotAxis *axis, 
						 gdouble val,
                                         	 gint precision,
                                         	 gint style,
                                         	 gchar *label);
/* Grids */
void		gtk_plot_x0_set_visible			(GtkPlot *plot, 
							 gboolean visible);
gboolean 	gtk_plot_x0_visible			(GtkPlot *plot);
void 		gtk_plot_y0_set_visible			(GtkPlot *plot, 
							 gboolean visible);
gboolean 	gtk_plot_y0_visible			(GtkPlot *plot);
void		gtk_plot_grids_set_on_top		(GtkPlot *plot,
							 gboolean on_top);
gboolean	gtk_plot_grids_on_top			(GtkPlot *plot);
void		gtk_plot_grids_set_visible		(GtkPlot *plot,
                				         gboolean vmajor, 
						 	 gboolean vminor,
                        				 gboolean hmajor,
							 gboolean hminor);
void		gtk_plot_grids_visible		        (GtkPlot *plot,
                         				 gboolean *vmajor, 
							 gboolean *vminor,
                         				 gboolean *hmajor, 
							 gboolean *hminor);
void		gtk_plot_y0line_set_attributes 	(GtkPlot *plot,
						 GtkPlotLineStyle style,
						 gfloat width,
						 const GdkColor *color);
void		gtk_plot_x0line_set_attributes 	(GtkPlot *plot,
						 GtkPlotLineStyle style,
						 gfloat width,
						 const GdkColor *color);
void		gtk_plot_major_vgrid_set_attributes 	(GtkPlot *plot,
						 	 GtkPlotLineStyle style,
						 	 gfloat width,
						 	 const GdkColor *color);
void		gtk_plot_minor_vgrid_set_attributes 	(GtkPlot *plot,
						 	 GtkPlotLineStyle style,
						 	 gfloat width,
						 	 const GdkColor *color);
void		gtk_plot_major_hgrid_set_attributes 	(GtkPlot *plot,
						 	 GtkPlotLineStyle style,
						 	 gfloat width,
						 	 const GdkColor *color);
void		gtk_plot_minor_hgrid_set_attributes 	(GtkPlot *plot,
						 	 GtkPlotLineStyle style,
						 	 gfloat width,
						 	 const GdkColor *color);

/* Legends */

void 		gtk_plot_show_legends 		(GtkPlot *plot);
void 		gtk_plot_hide_legends 		(GtkPlot *plot);
void 		gtk_plot_set_legends_border 	(GtkPlot *plot,
                                                 GtkPlotBorderStyle border,
                                                 gint shadow_width);
void		gtk_plot_legends_move		(GtkPlot *plot,
						 gdouble x, gdouble y);
void		gtk_plot_legends_get_position	(GtkPlot *plot,
						 gdouble *x, gdouble *y);
GtkAllocation	gtk_plot_legends_get_allocation	(GtkPlot *plot);
void		gtk_plot_legends_set_attributes (GtkPlot *plot,
						 const gchar *font,
						 gint height,
						 const GdkColor *foreground,
						 const GdkColor *background);
void 		gtk_plot_set_line_attributes    (GtkPlot *plot,
                                                 GtkPlotLine line);
/* Datasets */

void 		gtk_plot_add_data		(GtkPlot *plot,
						 GtkPlotData *data);
gint 		gtk_plot_remove_data		(GtkPlot *plot,
						 GtkPlotData *data);
GtkPlotData * 	gtk_plot_add_function		(GtkPlot *plot,
						 GtkPlotFunc function);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_H__ */
