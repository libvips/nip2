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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "gtkplotpc.h"
#include "gtkplotgdk.h"
#include "gtkplot.h"
#include "gtkpsfont.h"
#include "gtkplotcanvas.h"
#include <pango/pango.h>

static void gtk_plot_gdk_init                       (GtkPlotGdk *pc);
static void gtk_plot_gdk_class_init                 (GtkPlotGdkClass *klass);
static void gtk_plot_gdk_finalize                   (GObject *object);
static void gtk_plot_gdk_real_set_drawable          (GtkPlotGdk *gdk,
						     GdkDrawable *drawable);
static gboolean gtk_plot_gdk_real_init              (GtkPlotPC *pc);
static void gtk_plot_gdk_set_viewport               (GtkPlotPC *pc,
						     gdouble w, gdouble h);
static void gtk_plot_gdk_leave                      (GtkPlotPC *pc);
static void gtk_plot_gdk_gsave                      (GtkPlotPC *pc);
static void gtk_plot_gdk_grestore                   (GtkPlotPC *pc);
static void gtk_plot_gdk_clip                       (GtkPlotPC *pc,
                                                     const GdkRectangle *area);
static void gtk_plot_gdk_clip_mask                  (GtkPlotPC *pc,
						     gdouble x,
						     gdouble y,
                                                     const GdkBitmap *mask);
static void gtk_plot_gdk_set_color                   (GtkPlotPC *pc,
                                                     const GdkColor *color);
static void gtk_plot_gdk_set_lineattr           (GtkPlotPC *pc,
                                                 gfloat line_width,
                                                 GdkLineStyle line_style,
                                                 GdkCapStyle cap_style,
                                                 GdkJoinStyle join_style);
static void gtk_plot_gdk_set_dash                    (GtkPlotPC *pc,
                                                     gdouble offset_,
                                                     gdouble *values,
                                                     gint num_values);
static void gtk_plot_gdk_draw_point                  (GtkPlotPC *pc,
                                                     gdouble x, gdouble y);
static void gtk_plot_gdk_draw_line                   (GtkPlotPC *pc,
                                                     gdouble x1, gdouble y1,
                                                     gdouble x2, gdouble y2);
static void gtk_plot_gdk_draw_lines                  (GtkPlotPC *pc,
                                                     GtkPlotPoint *points,
                                                     gint numpoints);
static void gtk_plot_gdk_draw_rectangle              (GtkPlotPC *pc,
                                                     gint filled,
                                                     gdouble x, gdouble y,
                                                     gdouble width, 
                                                     gdouble height);
static void gtk_plot_gdk_draw_polygon                (GtkPlotPC *pc,
                                                     gint filled,
                                                     GtkPlotPoint *points,
                                                     gint numpoints);
static void gtk_plot_gdk_draw_circle                 (GtkPlotPC *pc,
                                                     gint filled,
                                                     gdouble x, gdouble y,
                                                     gdouble size);
static void gtk_plot_gdk_draw_ellipse                (GtkPlotPC *pc,
                                                     gint filled,
                                                     gdouble x, gdouble y,
                                                     gdouble width, 
                                                     gdouble height);
static void gtk_plot_gdk_set_font                    (GtkPlotPC *pc,
						     GtkPSFont *psfont,
                                                     gint height);
static void gtk_plot_gdk_draw_string                (GtkPlotPC *pc,
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
static void gtk_plot_gdk_draw_pixmap                (GtkPlotPC *pc,
                                                     GdkPixmap *pixmap,
                                                     GdkBitmap *mask,
                                                     gint xsrc, gint ysrc,
                                                     gint xdest, gint ydest,
                                                     gint width, gint height,
                                                     gdouble scale_x, 
                                                     gdouble scale_y);

static GdkPixmap * scale_pixmap 		    (GdkWindow *window, 
						     GdkPixmap *pixmap, 
						     gdouble scale_x, 
						     gdouble scale_y);
static GdkBitmap * scale_bitmap 		    (GdkWindow *window, 
						     GdkBitmap *bitmap, 
						     gdouble scale_x, 
						     gdouble scale_y);

extern inline gint roundint                         (gdouble x);

static GtkPlotPCClass *parent_class = NULL;

GtkType
gtk_plot_gdk_get_type (void)
{
  static GtkType pc_type = 0;

  if (!pc_type)
    {
      GtkTypeInfo pc_info =
      {
        "GtkPlotGdk",
        sizeof (GtkPlotGdk),
        sizeof (GtkPlotGdkClass),
        (GtkClassInitFunc) gtk_plot_gdk_class_init,
        (GtkObjectInitFunc) gtk_plot_gdk_init,
        /* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      pc_type = gtk_type_unique (GTK_TYPE_PLOT_PC, &pc_info);
    }
  return pc_type;
}

static void
gtk_plot_gdk_init (GtkPlotGdk *pc)
{
/*
  GdkWindowAttr attributes;
  gint attributes_mask;
  GdkScreen *screen;

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.title = NULL;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gdk_visual_get_system ();
  attributes.colormap = gdk_colormap_get_system ();
  attributes.event_mask = 0;
  attributes_mask = GDK_WA_VISUAL | GDK_WA_COLORMAP;
*/
  pc->gc = NULL;
  pc->drawable = NULL;
  pc->ref_count = 0;
  pc->text_direction = GTK_TEXT_DIR_LTR;

/*
  pc->window = gdk_window_new (NULL, &attributes, attributes_mask);

  screen = gdk_screen_get_default ();
  pc->context = gdk_pango_context_get ();
*/
/*
  pango_context_set_base_dir (pc->context,
                              pc->text_direction == GTK_TEXT_DIR_LTR ?
                              PANGO_DIRECTION_LTR : PANGO_DIRECTION_RTL);
*/
/*
  pango_context_set_language (pc->context, gtk_get_default_language ());
  
  pc->layout = pango_layout_new(pc->context);
*/
}


static void
gtk_plot_gdk_class_init (GtkPlotGdkClass *klass)
{
  GtkObjectClass *object_class;
  GObjectClass *gobject_class;
  GtkPlotPCClass *pc_class;
  GtkPlotGdkClass *gdk_class;

  parent_class = gtk_type_class (gtk_plot_pc_get_type ());

  object_class = (GtkObjectClass *) klass;
  gobject_class = (GObjectClass *) klass;

  pc_class = (GtkPlotPCClass *) klass;
  gdk_class = (GtkPlotGdkClass *) klass;

  gobject_class->finalize = gtk_plot_gdk_finalize;

  gdk_class->set_drawable = gtk_plot_gdk_real_set_drawable;

  pc_class->init = gtk_plot_gdk_real_init;
  pc_class->leave = gtk_plot_gdk_leave;
  pc_class->set_viewport = gtk_plot_gdk_set_viewport;
  pc_class->gsave = gtk_plot_gdk_gsave;
  pc_class->grestore = gtk_plot_gdk_grestore;
  pc_class->clip = gtk_plot_gdk_clip;
  pc_class->clip_mask = gtk_plot_gdk_clip_mask;
  pc_class->set_color = gtk_plot_gdk_set_color;
  pc_class->set_dash = gtk_plot_gdk_set_dash;
  pc_class->set_lineattr = gtk_plot_gdk_set_lineattr;
  pc_class->draw_point = gtk_plot_gdk_draw_point;
  pc_class->draw_line = gtk_plot_gdk_draw_line;
  pc_class->draw_lines = gtk_plot_gdk_draw_lines;
  pc_class->draw_rectangle = gtk_plot_gdk_draw_rectangle;
  pc_class->draw_polygon = gtk_plot_gdk_draw_polygon;
  pc_class->draw_circle = gtk_plot_gdk_draw_circle;
  pc_class->draw_ellipse = gtk_plot_gdk_draw_ellipse;
  pc_class->set_font = gtk_plot_gdk_set_font;
  pc_class->draw_string = gtk_plot_gdk_draw_string;
  pc_class->draw_pixmap = gtk_plot_gdk_draw_pixmap;
}


GtkObject *
gtk_plot_gdk_new                                (GtkWidget *widget)
{
  GtkObject *object;

  object = gtk_type_new(gtk_plot_gdk_get_type());

  gtk_plot_gdk_construct(GTK_PLOT_GDK(object), widget);

  return (object);
}

void
gtk_plot_gdk_construct(GtkPlotGdk *pc, GtkWidget *widget)
{
  pc->window = widget->window;
  pc->context = gtk_widget_get_pango_context (widget);
  g_object_ref(G_OBJECT(pc->context));
  pc->layout = pango_layout_new(pc->context);
}


static void
gtk_plot_gdk_finalize (GObject *object)
{
  GtkPlotGdk *pc = GTK_PLOT_GDK(object);

  GTK_PLOT_GDK(object)->window = NULL;

  if(GTK_PLOT_GDK(object)->ref_count > 0 && GTK_PLOT_GDK(object)->gc){
          gdk_gc_destroy(GTK_PLOT_GDK(object)->gc);
          GTK_PLOT_GDK(object)->gc = NULL;
  }

  if(pc->layout)
    g_object_unref(G_OBJECT(pc->layout));
  pc->layout = NULL;

  if(pc->context)
    g_object_unref(G_OBJECT(pc->context));
  pc->context = NULL;
}

static void
gtk_plot_gdk_real_set_drawable(GtkPlotGdk *pc, GdkDrawable *drawable)
{
  pc->drawable = drawable;
}

static gboolean 
gtk_plot_gdk_real_init (GtkPlotPC *pc)
{
  return TRUE;
}

static void
gtk_plot_gdk_leave (GtkPlotPC *pc)
{
}

void 
gtk_plot_gdk_set_drawable               (GtkPlotGdk *gdk, GdkDrawable *drawable)
{
  GTK_PLOT_GDK_CLASS(GTK_OBJECT_GET_CLASS(GTK_OBJECT(gdk)))->set_drawable(gdk, drawable);
}

static void 
gtk_plot_gdk_set_viewport               (GtkPlotPC *pc, gdouble w, gdouble h)
{
}

static void 
gtk_plot_gdk_gsave                                  (GtkPlotPC *pc)
{
  if(GTK_PLOT_GDK(pc)->gc) 
    gdk_gc_ref(GTK_PLOT_GDK(pc)->gc);
  else{
    if(GTK_PLOT_GDK(pc)->drawable) GTK_PLOT_GDK(pc)->gc = gdk_gc_new(GTK_PLOT_GDK(pc)->drawable);
  }

  GTK_PLOT_GDK(pc)->ref_count++;
}

static void 
gtk_plot_gdk_grestore                                  (GtkPlotPC *pc)
{
  if(GTK_PLOT_GDK(pc)->gc) gdk_gc_unref(GTK_PLOT_GDK(pc)->gc);

  GTK_PLOT_GDK(pc)->ref_count--;
  if(GTK_PLOT_GDK(pc)->ref_count == 0) GTK_PLOT_GDK(pc)->gc = NULL;
}

static void 
gtk_plot_gdk_clip                                   (GtkPlotPC *pc,
                                                     const GdkRectangle *area)
{
  if(!GTK_PLOT_GDK(pc)->gc) return;

  /* discard GdkRectangle* const: 
   * gdk_gc_set_clip_rectangle should have a const arg.
   * I've checked the code and it doesn't change it or keep it. murrayc.
   */

  gdk_gc_set_clip_rectangle(GTK_PLOT_GDK(pc)->gc, (GdkRectangle*)area);  
}

static void 
gtk_plot_gdk_clip_mask                              (GtkPlotPC *pc,
						     gdouble x,
						     gdouble y,
                                                     const GdkBitmap *mask)
{
  if(!GTK_PLOT_GDK(pc)->gc) return;

  if(x >= 0 && y >= 0)
    gdk_gc_set_clip_origin(GTK_PLOT_GDK(pc)->gc, x, y);

  gdk_gc_set_clip_mask(GTK_PLOT_GDK(pc)->gc, (GdkBitmap*)mask);  
}

static void 
gtk_plot_gdk_set_color                               (GtkPlotPC *pc,
                                                     const GdkColor *color)
{
  GdkColor new_color;

  if(!GTK_PLOT_GDK(pc)->gc) return;

  new_color = *color;
  gdk_color_alloc(gdk_colormap_get_system(), &new_color);
  gdk_gc_set_foreground(GTK_PLOT_GDK(pc)->gc, &new_color);
}

static void 
gtk_plot_gdk_set_dash                               (GtkPlotPC *pc,
                                                    gdouble offset,
                                                    gdouble *values,
                                                    gint num_values)
{
  gint8 dash[1000];
  int i;

  if(!GTK_PLOT_GDK(pc)->gc) return;

  if(num_values == 0){
    return;
  }

  g_assert( num_values < 1000 );

  for(i = 0; i < num_values; i++)
	  dash[i] = values[i];

  gdk_gc_set_dashes(GTK_PLOT_GDK(pc)->gc, 0, dash, num_values);
}

static void gtk_plot_gdk_set_lineattr           (GtkPlotPC *pc,
                                                 gfloat line_width,
                                                 GdkLineStyle line_style,
                                                 GdkCapStyle cap_style,
                                                 GdkJoinStyle join_style)
{
  if(!GTK_PLOT_GDK(pc)->gc) return;

  gdk_gc_set_line_attributes(GTK_PLOT_GDK(pc)->gc,  
                             roundint(line_width),
                             line_style,
                             cap_style,
                             join_style);
}

static void 
gtk_plot_gdk_draw_point                              (GtkPlotPC *pc,
                                                     gdouble x, gdouble y)
{
  if(!GTK_PLOT_GDK(pc)->gc) return;
  if(!GTK_PLOT_GDK(pc)->drawable) return;

  gdk_draw_point(GTK_PLOT_GDK(pc)->drawable, GTK_PLOT_GDK(pc)->gc, 
                 roundint(x), roundint(y));
}

static void 
gtk_plot_gdk_draw_line                               (GtkPlotPC *pc,
                                                     gdouble x1, gdouble y1,
                                                     gdouble x2, gdouble y2)
{
  if(!GTK_PLOT_GDK(pc)->gc) return;
  if(!GTK_PLOT_GDK(pc)->drawable) return;

  gdk_draw_line(GTK_PLOT_GDK(pc)->drawable, GTK_PLOT_GDK(pc)->gc, 
                roundint(x1), roundint(y1), roundint(x2), roundint(y2));
}

static void 
gtk_plot_gdk_draw_lines                              (GtkPlotPC *pc,
                                                     GtkPlotPoint *points,
                                                     gint numpoints)
{
  GdkPoint *p = NULL;
  gint i;

  if(!GTK_PLOT_GDK(pc)->gc) return;
  if(!GTK_PLOT_GDK(pc)->drawable) return;

  p = (GdkPoint *)g_malloc(numpoints * sizeof(GdkPoint));
  for(i = 0; i < numpoints; i++){
    p[i].x = roundint(points[i].x);
    p[i].y = roundint(points[i].y);
  }

  gdk_draw_lines(GTK_PLOT_GDK(pc)->drawable, GTK_PLOT_GDK(pc)->gc, p, numpoints);

  g_free(p);
}

static void 
gtk_plot_gdk_draw_rectangle                          (GtkPlotPC *pc,
                                                     gint filled,
                                                     gdouble x, gdouble y,
                                                     gdouble width, gdouble height)
{
  if(!GTK_PLOT_GDK(pc)->gc) return;
  if(!GTK_PLOT_GDK(pc)->drawable) return;

  gdk_draw_rectangle (GTK_PLOT_GDK(pc)->drawable, GTK_PLOT_GDK(pc)->gc,
                      filled,
                      roundint(x), roundint(y), 
                      roundint(width), roundint(height));

}

static void 
gtk_plot_gdk_draw_polygon                            (GtkPlotPC *pc,
                                                     gint filled,
                                                     GtkPlotPoint *points,
                                                     gint numpoints)
{
  GdkPoint *p = NULL;
  gint i;

  if(!GTK_PLOT_GDK(pc)->gc) return;
  if(!GTK_PLOT_GDK(pc)->drawable) return;

  p = (GdkPoint *)g_malloc(numpoints * sizeof(GdkPoint));
  for(i = 0; i < numpoints; i++){
    p[i].x = roundint(points[i].x);
    p[i].y = roundint(points[i].y);
  }

  gdk_draw_polygon(GTK_PLOT_GDK(pc)->drawable, GTK_PLOT_GDK(pc)->gc, 
                   filled, p, numpoints);

  g_free(p);
}

static void 
gtk_plot_gdk_draw_circle                             (GtkPlotPC *pc,
                                                     gint filled,
                                                     gdouble x, gdouble y,
                                                     gdouble size)
{
  if(!GTK_PLOT_GDK(pc)->gc) return;
  if(!GTK_PLOT_GDK(pc)->drawable) return;

  gdk_draw_arc (GTK_PLOT_GDK(pc)->drawable, GTK_PLOT_GDK(pc)->gc,
                filled,
                roundint(x-size/2.0), roundint(y-size/2.0),
                roundint(size), roundint(size), 0, 25000);

}

static void 
gtk_plot_gdk_draw_ellipse                            (GtkPlotPC *pc,
                                                     gint filled,
                                                     gdouble x, gdouble y,
                                                     gdouble width, gdouble height)
{
  if(!GTK_PLOT_GDK(pc)->gc) return;
  if(!GTK_PLOT_GDK(pc)->drawable) return;

  gdk_draw_arc (GTK_PLOT_GDK(pc)->drawable, GTK_PLOT_GDK(pc)->gc,
                filled,
                roundint(x), roundint(y),
                roundint(width), roundint(height), 0, 25000);
}

static void 
gtk_plot_gdk_set_font                                (GtkPlotPC *pc,
						     GtkPSFont *psfont,
                                                     gint height)
{

}

/* subfunction of gtk_plot_gdk_draw_string(). */
static gint
drawstring(GtkPlotPC *pc,
	   GdkDrawable *drawable,
	   GdkGC *gc,
           gint angle,
	   gint dx, gint dy,
	   GtkPSFont *psfont, gint height,
	   const gchar *text)
{
  PangoFontDescription *font;
  PangoRectangle rect;
  gint ret_value;

  if(!text || strlen(text) == 0) return 0;
  font = gtk_psfont_get_font_description(psfont, height);
  pango_layout_set_font_description(GTK_PLOT_GDK(pc)->layout, font);

  pango_layout_set_text(GTK_PLOT_GDK(pc)->layout, text, strlen(text));
  pango_layout_get_extents(GTK_PLOT_GDK(pc)->layout, NULL, &rect);

  if (psfont->i18n_latinfamily && psfont->vertical) {
    /* vertical-writing CJK postscript fonts. */
    return rect.height;
  } 
  else 
  {
    /* horizontal writing */
    if(angle == 90)
      gdk_draw_layout (drawable, gc, dx, dy-PANGO_PIXELS(rect.width), GTK_PLOT_GDK(pc)->layout);
    else if(angle == 180)
      gdk_draw_layout (drawable, gc, dx-PANGO_PIXELS(rect.width), dy, GTK_PLOT_GDK(pc)->layout);
    else
      gdk_draw_layout (drawable, gc, dx, dy, GTK_PLOT_GDK(pc)->layout);

/*
    gdk_draw_layout (GTK_PLOT_GDK(pc)->drawable, gc, dx, dy, GTK_PLOT_GDK(pc)->layout);
*/
    
  }
  pango_font_description_free(font);
  ret_value = (angle == 0 || angle == 180) ? rect.width : rect.height;
  return PANGO_PIXELS(rect.width);
}

static void 
gtk_plot_gdk_draw_string                        (GtkPlotPC *pc,
                                                gint tx, gint ty,
                                                gint angle,
                                                const GdkColor *fg,
                                                const GdkColor *bg,
                                                gboolean transparent,
                                                gint border,
                                                gint border_space,
                                                gint border_width,
                                                gint shadow_width,
                                                const gchar *font_name,
                                                gint font_height,
                                                GtkJustification just,
                                                const gchar *text)
{
  GdkGC *gc;
  GList *family = NULL;
  gint x0, y0;
  gint old_width, old_height;
  gboolean bold, italic;
  gint fontsize;
  gint ascent, descent;
  gint numf;
  gint width, height;
  gint x, y;
  gint i;
  PangoFontDescription *font = NULL, *latin_font = NULL;
  GtkPSFont *psfont, *base_psfont, *latin_psfont;
  gchar subs[2], insert_char;
  const gchar *aux = text;
  const gchar *lastchar = text;
  const gchar *wtext = text;
  const gchar *xaux = text;
  gchar new_text[strlen(text)+1];
  gchar num[4];
  PangoRectangle rect;
  PangoFontMetrics *metrics = NULL;
  PangoLayout *layout = NULL;
  gint real_x, real_y, real_width, real_height;
  GdkColor real_fg = *fg;  
  GdkColor real_bg = *bg;  
  PangoMatrix matrix = PANGO_MATRIX_INIT;
  PangoContext *context = GTK_PLOT_GDK(pc)->context;
  GdkDrawable *drawable = GTK_PLOT_GDK(pc)->drawable;
  gint sign_x = 1, sign_y = 0;

  if(!GTK_PLOT_GDK(pc)->drawable) return;
  if(!GTK_PLOT_GDK(pc)->gc) return;
  if(!text || strlen(text) == 0) return;

  gc = GTK_PLOT_GDK(pc)->gc;
  layout = GTK_PLOT_GDK(pc)->layout;

  if(!gc) return;

  gtk_plot_text_get_size(text, angle, font_name, font_height, &width, &height, &ascent, &descent);

  if(height == 0 || width == 0) return;

  old_width = width;
  old_height = height;
  if(angle == 90 || angle == 270)
    {
      old_width = height;
      old_height = width;
    }
  switch(angle){
    case 90:
      sign_x = 0;
      sign_y = -1;
      break;
    case 180:
      sign_x = -1;
      sign_y = 0;
      break;
    case 270:
      sign_x = 0;
      sign_y = 1;
      break;
    case 0:
    default:
      sign_x = 1;
      sign_y = 0;
      break;
  }

  switch(just){
    case GTK_JUSTIFY_LEFT:
      switch(angle){
        case 0:
            ty -= ascent;
            break;
        case 90:
            ty -= height;
            tx -= ascent;
            break;
        case 180:
            tx -= width;
            ty -= descent;
            break;
        case 270:
            tx -= descent;
            break;
      }
      break;
    case GTK_JUSTIFY_RIGHT:
      switch(angle){
        case 0:
            tx -= width;
            ty -= ascent;
            break;
        case 90:
            tx -= ascent;
            ty += height;
            break;
        case 180:
            tx += width;
            ty -= descent;
            break;
        case 270:
            tx -= descent;
            break;
      }
      break;
    case GTK_JUSTIFY_CENTER:
    default:
      switch(angle){
        case 0:
            tx -= width / 2.;
            ty -= ascent;
            break;
        case 90:
            tx -= ascent;
            ty += height / 2.;
            break;
        case 180:
            tx += width / 2.;
            ty -= descent;
            break;
        case 270:
            tx -= descent;
            ty -= height / 2.;
            break;
      }
  }

  real_x = tx;
  real_y = ty;
  real_width = width;
  real_height = height;

  pango_matrix_rotate (&matrix, angle);
  pango_context_set_matrix (context, &matrix);
  pango_layout_context_changed (layout);

  if(!transparent){
    gdk_gc_set_foreground(gc, &real_bg);
    gdk_draw_rectangle(drawable, gc, TRUE, tx, ty, old_width, old_height);
  }

/* TEST */
/*
  gdk_draw_rectangle(text_pixmap, gc, FALSE, 0, 0, old_width-1, old_height-1);
*/

  gtk_psfont_get_families(&family, &numf);
  base_psfont = psfont = gtk_psfont_get_by_name(font_name);
  font = gtk_psfont_get_font_description(psfont, font_height);
  italic = psfont->italic;
  bold = psfont->bold;
  fontsize = font_height;
  x0 = x = 0;
  y0 = y = 0;

  if (psfont->i18n_latinfamily) {
    latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily, italic,
                                             bold);
    if(latin_font) pango_font_description_free(latin_font);
    latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
  } else {
    latin_psfont = NULL; 
    latin_font = NULL;
  }

  gdk_gc_set_foreground(gc, &real_fg);
  aux = text;
  while(aux && *aux != '\0' && *aux != '\n'){
   if(*aux == '\\'){
     aux = g_utf8_next_char(aux);
     switch(*aux){
       case '0': case '1': case '2': case '3':
       case '4': case '5': case '6': case '7': case '9':
           psfont = gtk_psfont_get_by_family((gchar *)g_list_nth_data(family, *aux-'0'), italic, bold);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           aux = g_utf8_next_char(aux);
           break;
       case '8': case 'g':
           psfont = gtk_psfont_get_by_family("Symbol", italic, bold);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           aux = g_utf8_next_char(aux);
           break;
       case 'B':
           bold = TRUE;
           psfont = gtk_psfont_get_by_family(psfont->family, italic, bold);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           if(latin_font){
             latin_font = NULL;
           }
           if (psfont->i18n_latinfamily) {
             latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily,
                                                      italic, bold);
             if(latin_font) pango_font_description_free(latin_font);
             latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
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
              aux = g_utf8_next_char(aux);
              break;
           }
           num[3] = '\0';
           insert_char = (gchar)atoi(num);
           subs[0] = insert_char;
           subs[1] = '\0';
           pango_layout_set_font_description(layout, font);
           pango_layout_set_text(layout, subs, 1);
           pango_layout_get_extents(layout, NULL, &rect);
           x += sign_x*PANGO_PIXELS(rect.width);
           y += sign_y*PANGO_PIXELS(rect.width);
           aux += 4;
           lastchar = aux - 1;
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
             latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
           }
           aux = g_utf8_next_char(aux);
           break;
       case 'S': case '^':
           fontsize = (int)((gdouble)fontsize * 0.6 + 0.5);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           if(metrics) pango_font_metrics_unref(metrics);
           metrics = pango_context_get_metrics(pango_layout_get_context(layout), font, pango_context_get_language(pango_layout_get_context(layout)));
           if (psfont->i18n_latinfamily) {
             latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
           }
           if(angle == 180)
             y = y0 + fontsize;
           else if(angle == 270)
             x = x0 + sign_y*fontsize;
           aux = g_utf8_next_char(aux);
           break;
       case 's': case '_':
           fontsize = (int)((gdouble)fontsize * 0.6 + 0.5);
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           if(metrics) pango_font_metrics_unref(metrics);
           metrics = pango_context_get_metrics(pango_layout_get_context(layout), font, pango_context_get_language(pango_layout_get_context(layout)));
           if(angle == 0)
             y = y0 + fontsize;
           else if(angle == 90)
             x = x0 - sign_y*fontsize;
           if (psfont->i18n_latinfamily) {
             latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
           }
           aux = g_utf8_next_char(aux);
           break;
       case '+':
           fontsize += 3;
           y -= sign_x*3;
           x += sign_y*3;
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           if (psfont->i18n_latinfamily) {
             latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
           }
           aux = g_utf8_next_char(aux);
           break;
       case '-':
           fontsize -= 3;
           y += sign_x*3;
           x -= sign_y*3;
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           if (psfont->i18n_latinfamily) {
             latin_font = gtk_psfont_get_font_description(latin_psfont, fontsize);
           }
           aux = g_utf8_next_char(aux);
           break;
       case 'N':
	   psfont = base_psfont;
           fontsize = font_height;
           pango_font_description_free(font);
           font = gtk_psfont_get_font_description(psfont, fontsize);
           if(angle == 0 || angle == 180)
             y = y0;
           else
             x = x0;
           italic = psfont->italic;
           bold = psfont->bold;
           aux = g_utf8_next_char(aux);
           break;
       case 'b':
	   if (lastchar) {
             const gchar *aux2 = lastchar;
             gint i = g_utf8_prev_char(lastchar) != --aux2 ? 2 : 1;
             pango_layout_set_text(layout, lastchar, i);
             pango_layout_get_extents(layout, NULL, &rect);
	     x -= sign_x*PANGO_PIXELS(rect.width);
	     y -= sign_y*PANGO_PIXELS(rect.width);

	     if (lastchar == wtext)
	       lastchar = NULL;
	     else
	       lastchar = g_utf8_prev_char(lastchar);
	   } else {
             pango_layout_set_text(layout, "X", 1);
             pango_layout_get_extents(layout, NULL, &rect);
	     x -= sign_x*PANGO_PIXELS(rect.width);
	     y -= sign_y*PANGO_PIXELS(rect.width);
	   }
           aux = g_utf8_next_char(aux);
           break;
       default:
           if(aux && *aux != '\0' && *aux !='\n'){
             gint new_width = 0;
	     new_width = drawstring(pc, drawable, gc, angle, tx+x, ty+y,
			     psfont, fontsize, aux);
             x += sign_x * new_width;
             y += sign_y * new_width;
	     lastchar = aux;
	     aux = g_utf8_next_char(aux);
	   }
	   break;
     }
   } else {
     gint new_len = 0;
     gint new_width = 0;
     lastchar = aux;
     while(aux && *aux != '\0' && *aux !='\n' && *aux != '\\'){
       xaux = aux;
       new_len += g_utf8_next_char(aux) != ++xaux ? 2 : 1;
       aux = g_utf8_next_char(aux);
     }
     xaux = lastchar;
     for(i = 0; i < new_len; i++) new_text[i] = *xaux++;
     new_text[new_len] = '\0';
     new_width = drawstring(pc, drawable, gc, angle, tx+x, ty+y,
	         psfont, fontsize, new_text);
     x += sign_x * new_width;
     y += sign_y * new_width;
     lastchar = aux;
   }
  }

  pango_font_description_free(font);
  if(latin_font) pango_font_description_free(latin_font);
  if(metrics) pango_font_metrics_unref(metrics);

/* border */

  gdk_gc_set_foreground(gc, &real_fg);
  gtk_plot_pc_set_dash(pc, 0, NULL, 0);
  gtk_plot_pc_set_lineattr(pc, border_width, 0, 0, 0);
  switch(border){
    case GTK_PLOT_BORDER_SHADOW: 
      gtk_plot_pc_draw_rectangle(pc,
   		         TRUE, 
                         tx - border_space + shadow_width, 
                         ty + height + border_space, 
                         width + 2 * border_space, shadow_width);
      gtk_plot_pc_draw_rectangle(pc,
   		         TRUE, 
                         tx + width + border_space, 
                         ty - border_space + shadow_width, 
                         shadow_width, height + 2 * border_space);
    case GTK_PLOT_BORDER_LINE: 
      gtk_plot_pc_draw_rectangle(pc,
   		         FALSE, 
                         tx - border_space, ty - border_space, 
                         width + 2*border_space, height + 2*border_space);
    case GTK_PLOT_BORDER_NONE: 
    default:
	break; 
  }

  return;
}

static void gtk_plot_gdk_draw_pixmap                (GtkPlotPC *pc,
                                                     GdkPixmap *pixmap,
                                                     GdkBitmap *mask,
                                                     gint xsrc, gint ysrc,
                                                     gint xdest, gint ydest,
                                                     gint width,
                                                     gint height,
                                                     gdouble scale_x, 
                                                     gdouble scale_y)
{
  GdkGC *gc;
  GdkPixmap *new_pixmap;
  GdkBitmap *new_mask = NULL;

  if(!GTK_PLOT_GDK(pc)->drawable) return;
  if(!GTK_PLOT_GDK(pc)->gc) return;

  gc = GTK_PLOT_GDK(pc)->gc;

  if(!gc) return;

  new_pixmap = scale_pixmap(GTK_PLOT_GDK(pc)->drawable, pixmap, scale_x, scale_y);

  if(mask)
     new_mask = scale_bitmap(GTK_PLOT_GDK(pc)->drawable, mask, scale_x, scale_y);

/* TEST
  new_pixmap = pixmap; gdk_pixmap_ref(pixmap);
  if(mask) new_mask = mask; gdk_bitmap_ref(mask);
*/

  gtk_plot_pc_clip_mask(pc, xdest, ydest, new_mask);
  gdk_draw_pixmap(GTK_PLOT_GDK(pc)->drawable, gc, new_pixmap,
                  xsrc, ysrc, xdest, ydest, width*scale_x, height*scale_y);
  gtk_plot_pc_clip_mask(pc, xdest, ydest, NULL);

  if(new_mask) gdk_bitmap_unref(new_mask);
  gdk_pixmap_unref(new_pixmap);
}

static GdkPixmap *
scale_pixmap (GdkWindow *window, GdkPixmap *pixmap, gdouble scale_x, gdouble scale_y)
{
  GdkGC *gc = NULL;
  GdkPixmap *new_pixmap;
  gint width, height, new_width, new_height;
  GdkPixbuf *pixbuf = NULL;
  GdkPixbuf *aux_pixbuf = NULL;
                                                                                
  if(!pixmap) return NULL;
  if(!window) return NULL;
                                                                                
  gdk_window_get_size(pixmap, &width, &height);
                                                                                
  gc = gdk_gc_new(window);
                                                                                
  if(scale_x == 1.0 && scale_y == 1.0){
    new_pixmap = gdk_pixmap_new(window, width, height, -1);
    gdk_draw_pixmap(new_pixmap,
                    gc,
                    pixmap,
                    0, 0,
                    0, 0,
                    width, height);
    return new_pixmap;
  }
                                                                                
  new_width = roundint(width * scale_x);
  new_height = roundint(height * scale_y);
                                                                                
  pixbuf = gdk_pixbuf_get_from_drawable(NULL, pixmap, gdk_drawable_get_colormap(window), 0, 0, 0, 0, width, height);
  aux_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
  new_pixmap = gdk_pixmap_new(pixmap, new_width, new_height, -1);
  gdk_draw_pixbuf(new_pixmap, gc, aux_pixbuf, 0, 0, 0, 0, new_width, new_height, GDK_RGB_DITHER_MAX, 0, 0);
                                                                                
  gdk_pixbuf_unref(pixbuf);
  gdk_pixbuf_unref(aux_pixbuf);
                                                                                
  gdk_gc_unref(gc);
                                                                                
  return new_pixmap;
}

static GdkBitmap *
scale_bitmap (GdkWindow *window, GdkBitmap *bitmap, gdouble scale_x, gdouble scale_y)
{
  GdkGC *gc;
  GdkVisual *visual = NULL;
  GdkImage *image = NULL, *new_image = NULL;
  GdkBitmap *new_bitmap = NULL;
  gint x, y, width, height, new_width, new_height;
  GdkColor color;

  if(!bitmap) return NULL;
  if(!window) return NULL;

  gc = gdk_gc_new(bitmap);

  gdk_window_get_size(bitmap, &width, &height);

  if(scale_x == 1.0 && scale_y == 1.0){
    new_bitmap = gdk_pixmap_new(window, width, height, 1);
    color.pixel = 0;
    gdk_gc_set_foreground(gc, &color);
    gdk_draw_rectangle(new_bitmap, gc, TRUE, 0, 0, width, height); 
    color.pixel = 1;
    gdk_gc_set_foreground(gc, &color);

    gdk_draw_pixmap(new_bitmap,
                    gc,
                    bitmap,
                    0, 0,
                    0, 0,
                    width, height);
    return new_bitmap;
  }

  new_width = roundint(width * scale_x);
  new_height = roundint(height * scale_y);

  /* make a client side image of the bitmap, and
   * scale the data into a another client side image */
  visual = gdk_window_get_visual (bitmap);
  if(!visual) return NULL;
  new_image = gdk_image_new(GDK_IMAGE_FASTEST,visual,new_width,new_height);
  if(!new_image) return NULL;
  new_bitmap = gdk_pixmap_new(window, new_width, new_height, 1);

  image = gdk_drawable_get_image(bitmap,
                        0, 0,
                        width, height);

  color.pixel = 0;
  gdk_gc_set_foreground(gc, &color);
  gdk_draw_rectangle(new_bitmap, gc, TRUE, 0, 0, width, height); 
  color.pixel = 1;
  gdk_gc_set_foreground(gc, &color);

  for(x = 0; x < new_width; x++){
    for(y = 0; y < new_height; y++){
      gint px, py;
      gulong pixel;

      px = MIN(roundint(x / scale_x), width - 1);
      py = MIN(roundint(y / scale_y), height - 1);

      pixel = gdk_image_get_pixel(image, px, py);
      gdk_image_put_pixel(new_image, x, y, pixel);
    }
  }

  /* draw the image into a new pixmap */
  gdk_draw_image(new_bitmap,gc,new_image,0,0,0,0,new_width,new_height);

  gdk_image_destroy(image);
  gdk_image_destroy(new_image);

  return new_bitmap;
}

