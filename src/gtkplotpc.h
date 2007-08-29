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

#ifndef __GTK_PLOT_PC_H__
#define __GTK_PLOT_PC_H__

#include <stdio.h>
#include "gtkpsfont.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GTK_PLOT_PC(obj)        GTK_CHECK_CAST (obj, gtk_plot_pc_get_type (), GtkPlotPC)
#define GTK_TYPE_PLOT_PC   (gtk_plot_pc_get_type ())

#define GTK_PLOT_PC_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_pc_get_type(), GtkPlotPCClass)
#define GTK_IS_PLOT_PC(obj)     GTK_CHECK_TYPE (obj, gtk_plot_pc_get_type ())


/* Page size */

typedef enum{
     GTK_PLOT_LETTER	,
     GTK_PLOT_LEGAL	,
     GTK_PLOT_A4	,
     GTK_PLOT_EXECUTIVE	,
     GTK_PLOT_CUSTOM	
} GtkPlotPageSize;

#define GTK_PLOT_LETTER_W 	612   /* Width and Height in ps points */
#define GTK_PLOT_LETTER_H 	792

#define GTK_PLOT_LEGAL_W	612
#define GTK_PLOT_LEGAL_H	1008

#define GTK_PLOT_A4_W		595
#define GTK_PLOT_A4_H		842

#define GTK_PLOT_EXECUTIVE_W	540
#define GTK_PLOT_EXECUTIVE_H	720


/* Page orientation */
typedef enum{
     GTK_PLOT_PORTRAIT	,
     GTK_PLOT_LANDSCAPE	
} GtkPlotPageOrientation;

/* Size units */
typedef enum{
     GTK_PLOT_PSPOINTS	,
     GTK_PLOT_MM	,
     GTK_PLOT_CM	,
     GTK_PLOT_INCHES	
} GtkPlotUnits;


typedef struct _GtkPlotPC GtkPlotPC;
typedef struct _GtkPlotPCClass GtkPlotPCClass;
typedef struct _GtkPlotPoint         GtkPlotPoint;

struct _GtkPlotPoint
{
  gdouble x, y;
};

struct _GtkPlotPC
{
   GtkObject object;

   gdouble width, height; /* viewport */
   GdkColor color;

   gint init_count;
   gboolean use_pixmap;
};


struct _GtkPlotPCClass
{
   GtkObjectClass parent_class;

   gboolean  (* init)					(GtkPlotPC *pc);

   void  (* leave)					(GtkPlotPC *pc);

   void  (* set_viewport)				(GtkPlotPC *pc,
							 gdouble w, gdouble h);

   void  (* gsave)					(GtkPlotPC *pc);

   void  (* grestore)					(GtkPlotPC *pc);

   void  (* clip)					(GtkPlotPC *pc,
							 const GdkRectangle *area);
   void  (* clip_mask)					(GtkPlotPC *pc,
							 gdouble x,
							 gdouble y,
							 const GdkBitmap *mask);

   void  (* set_color)                     		(GtkPlotPC *pc,
                                                 	const GdkColor *color);

   void  (* set_lineattr)			(GtkPlotPC *pc,
						 gfloat line_width,
                                                 GdkLineStyle line_style,
                                                 GdkCapStyle cap_style,
                                                 GdkJoinStyle join_style);

   void  (* set_dash)					(GtkPlotPC *pc,
							 gdouble offset_,	
							 gdouble *values,
							 gint num_values);

   void  (* draw_point)					(GtkPlotPC *pc,
							 gdouble x, gdouble y);

   void  (* draw_line)					(GtkPlotPC *pc,
							 gdouble x1, gdouble y1,
							 gdouble x2, gdouble y2);

   void  (* draw_lines)					(GtkPlotPC *pc,
							 GtkPlotPoint *points,
							 gint numpoints);

   void  (* draw_rectangle)	                        (GtkPlotPC *pc,
							 gboolean filled,
							 gdouble x, gdouble y,
							 gdouble width,
							 gdouble height);

   void  (* draw_polygon)	                        (GtkPlotPC *pc,
							 gboolean filled,
                                	                 GtkPlotPoint *points,
                                        	         gint numpoints);

   void  (* draw_circle) 	                        (GtkPlotPC *pc,
							 gboolean filled,
                                                 	 gdouble x, gdouble y,
                                                 	 gdouble size); 

   void  (* draw_ellipse) 	                        (GtkPlotPC *pc,
							 gboolean filled,
                                                 	 gdouble x, gdouble y,
                                                 	 gdouble width, 
                                                 	 gdouble height); 

   void  (* set_font)					(GtkPlotPC *pc,
							 GtkPSFont *psfont,
							 gint height);

   void  (* draw_string)   	                        (GtkPlotPC *pc,
                                   	             	 gint x, gint y,
                                               		 gint angle,
							 const GdkColor *fg,
							 const GdkColor *bg,
							 gboolean transparent,
							 gint border,
							 gint border_space,
							 gint border_width,
							 gint shadow_width,
							 const gchar *font,
							 gint height,
							 GtkJustification just,
							 const gchar *text);

   void  (* draw_pixmap)   	                        (GtkPlotPC *pc,
							 GdkPixmap *pixmap,
							 GdkBitmap *mask,
                                   	             	 gint xsrc, gint ysrc,
                                   	             	 gint xdest, gint ydest,
                                   	             	 gint width, 
                                                         gint height, 
                                   	             	 gdouble scale_x, 
                                                         gdouble scale_y); 
};

GtkType    gtk_plot_pc_get_type				(void);
GtkObject *gtk_plot_pc_new				(void);
							 
gboolean gtk_plot_pc_init				(GtkPlotPC *pc);

void gtk_plot_pc_leave					(GtkPlotPC *pc);
void gtk_plot_pc_set_viewport				(GtkPlotPC *pc,
							 gdouble w, gdouble h);

void gtk_plot_pc_gsave					(GtkPlotPC *pc);

void gtk_plot_pc_grestore				(GtkPlotPC *pc);

void gtk_plot_pc_clip					(GtkPlotPC *pc,
							 GdkRectangle *area);
void gtk_plot_pc_clip_mask				(GtkPlotPC *pc,
							 gdouble x, 
							 gdouble y, 
							 GdkBitmap *mask);

void gtk_plot_pc_set_color                     		(GtkPlotPC *pc,
                                                   	 GdkColor *color);

void gtk_plot_pc_set_lineattr			  (GtkPlotPC *pc,
		                                   gfloat line_width,
                                                   GdkLineStyle line_style,
                                                   GdkCapStyle cap_style,
                                                   GdkJoinStyle join_style);

void gtk_plot_pc_set_dash				(GtkPlotPC *pc,
							 gdouble offset_,
							 gdouble *values,
							 gint num_values);

void gtk_plot_pc_draw_point				(GtkPlotPC *pc,
							 gdouble x, gdouble y);

void gtk_plot_pc_draw_line				(GtkPlotPC *pc,
							 gdouble x1, gdouble y1,
							 gdouble x2, gdouble y2);

void gtk_plot_pc_draw_lines				(GtkPlotPC *pc,
							 GtkPlotPoint *points,
							 gint numpoints);

void gtk_plot_pc_draw_rectangle	                        (GtkPlotPC *pc,
							 gboolean filled,
							 gdouble x, gdouble y,
							 gdouble width,
							 gdouble height);

void gtk_plot_pc_draw_polygon	                        (GtkPlotPC *pc,
                                                	 gint filled,
                                	                 GtkPlotPoint *points,
                                        	         gint numpoints);

void gtk_plot_pc_draw_ellipse	                        (GtkPlotPC *pc,
							 gboolean filled,
							 gdouble x, gdouble y,
							 gdouble width,
							 gdouble height);

void gtk_plot_pc_draw_circle 	                        (GtkPlotPC *pc,
							 gint filled,
                                                 	 gdouble x, gdouble y,
                                                 	 gdouble size); 

void gtk_plot_pc_set_font				(GtkPlotPC *pc,
							 GtkPSFont *psfont,
							 gint height);

void gtk_plot_pc_draw_string   	                	(GtkPlotPC *pc,
                                   	         	 gint x, gint y,
                                               		 gint angle,
							 const GdkColor *fg,
							 const GdkColor *bg,
							 gboolean transparent,
							 gint border,
							 gint border_space,
							 gint border_width,
							 gint shadow_width,
							 const gchar *font,
							 gint height,
							 GtkJustification just,
							 const gchar *text);

void  gtk_plot_pc_draw_pixmap   	                (GtkPlotPC *pc,
							 GdkPixmap *pixmap,
							 GdkBitmap *mask,
                                   	             	 gint xsrc, gint ysrc,
                                   	             	 gint xdest, gint ydest,
                                   	             	 gint width,
                                   	             	 gint height, 
                                   	             	 gdouble scale_x,
                                   	             	 gdouble scale_y); 

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_PC_H__ */

