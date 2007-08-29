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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gtk/gtk.h>
#include "gtkplot.h"
#include "gtkplotcanvas.h"
#include "gtkplotcanvastext.h"
#include "gtkplotgdk.h"
#include "gtkplotps.h"

static gchar DEFAULT_FONT[] = "Helvetica";
#define DEFAULT_FONT_HEIGHT 12
#define P_(string) string

enum {
  ARG_0,
  ARG_TEXT,
};

static void gtk_plot_canvas_text_init		(GtkPlotCanvasText *text);
static void gtk_plot_canvas_text_destroy	(GtkObject *object);
static void gtk_plot_canvas_text_class_init(GtkPlotCanvasChildClass *klass);
static void gtk_plot_canvas_text_draw 		(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_text_size_allocate	(GtkPlotCanvas *canvas,
						 GtkPlotCanvasChild *child);
static void gtk_plot_canvas_text_get_property(GObject      *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void gtk_plot_canvas_text_set_property(GObject      *object,
                                                 guint            prop_id,
                                                 const GValue          *value,
                                                 GParamSpec      *pspec);

extern inline gint roundint                     (gdouble x);
static GtkPlotCanvasChildClass *parent_class = NULL;

GtkType
gtk_plot_canvas_text_get_type (void)
{
  static GtkType plot_canvas_text_type = 0;

  if (!plot_canvas_text_type)
    {
      GtkTypeInfo plot_canvas_text_info =
      {
	"GtkPlotCanvasText",
	sizeof (GtkPlotCanvasText),
	sizeof (GtkPlotCanvasTextClass),
	(GtkClassInitFunc) gtk_plot_canvas_text_class_init,
	(GtkObjectInitFunc) gtk_plot_canvas_text_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      plot_canvas_text_type = gtk_type_unique (gtk_plot_canvas_child_get_type(), &plot_canvas_text_info);
    }
  return plot_canvas_text_type;
}

GtkPlotCanvasChild*
gtk_plot_canvas_text_new (const gchar *font, gint height, gint angle,
                          const GdkColor *fg, const GdkColor *bg,
                          gboolean transparent,
                          GtkJustification justification,
                          const gchar *real_text)
{
  GtkPlotCanvasText *text;
  GtkPlotText *text_attr;
                                                                                
  text = gtk_type_new (gtk_plot_canvas_text_get_type ());

  text_attr = &text->text;

  text_attr->angle = angle;
  text_attr->justification = justification;
  text_attr->transparent = transparent;
  text_attr->border = 0;
  text_attr->border_width = 0;
  text_attr->shadow_width = 0;
                                                                                
  if(!font) {
    text_attr->font = g_strdup(DEFAULT_FONT);
    text_attr->height = DEFAULT_FONT_HEIGHT;
  } else {
    text_attr->font = g_strdup(font);
    text_attr->height = height;
  }
                                                                                
  text_attr->text = NULL;
  if(text) text_attr->text = g_strdup(real_text);
                                                                                
  if(fg != NULL)
    text_attr->fg = *fg;
                                                                                
  if(bg != NULL)
    text_attr->bg = *bg;

  return GTK_PLOT_CANVAS_CHILD (text);
}

static void
gtk_plot_canvas_text_init (GtkPlotCanvasText *text)
{
  GtkPlotText *text_attr;

  text_attr = &text->text;

  text_attr->angle = 0;
  gdk_color_black(gdk_colormap_get_system(), &text_attr->fg);
  gdk_color_white(gdk_colormap_get_system(), &text_attr->bg);
  text_attr->justification = GTK_JUSTIFY_LEFT;
  text_attr->transparent = TRUE;
  text_attr->border = 0;
  text_attr->border_width = 0;
  text_attr->shadow_width = 0;
                                                                                
  text_attr->text = NULL;

  GTK_PLOT_CANVAS_CHILD(text)->flags = GTK_PLOT_CANVAS_CAN_MOVE;
}

static void
gtk_plot_canvas_text_class_init (GtkPlotCanvasChildClass *klass)
{
  GtkObjectClass *object_class = (GtkObjectClass *)klass;
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  parent_class = gtk_type_class (gtk_plot_canvas_child_get_type ());

  klass->draw = gtk_plot_canvas_text_draw; 
  klass->move = NULL; 
  klass->move_resize = NULL; 
  klass->size_allocate = gtk_plot_canvas_text_size_allocate;

  object_class->destroy = gtk_plot_canvas_text_destroy;

  gobject_class->get_property = gtk_plot_canvas_text_get_property;
  gobject_class->set_property = gtk_plot_canvas_text_set_property;
   
  g_object_class_install_property (gobject_class,
                           ARG_TEXT,
  g_param_spec_pointer ("text",
                           P_("Text Attributes"),
                           P_("Text Attributes"),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
}

static void
gtk_plot_canvas_text_get_property (GObject      *object,
                                    guint            prop_id,
                                    GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasText *text = GTK_PLOT_CANVAS_TEXT (object);
                                                                                
  switch(prop_id){
    case ARG_TEXT:
      g_value_set_pointer(value, &text->text);
      break;
  }
}

static void
gtk_plot_canvas_text_set_property (GObject      *object,
                                    guint            prop_id,
                                    const GValue          *value,
                                    GParamSpec      *pspec)
{
  GtkPlotCanvasText *text = GTK_PLOT_CANVAS_TEXT (object);
  GtkPlotText *aux_text = NULL;
                                                                                
  switch(prop_id){
    case ARG_TEXT:
      aux_text = (GtkPlotText *)g_value_get_pointer(value);
      if(text->text.text) g_free(text->text.text);
      if(text->text.font) g_free(text->text.font);
      text->text.text = NULL;
      text->text.font = NULL;
      text->text = *aux_text;
      if(aux_text->text) text->text.text = g_strdup(aux_text->text);
      if(aux_text->font) text->text.font = g_strdup(aux_text->font);
      break;
  }
}


static void
gtk_plot_canvas_text_destroy (GtkObject *object)
{
  GtkPlotCanvasText *text = GTK_PLOT_CANVAS_TEXT(object);

  if(text->text.font) g_free(text->text.font);
  text->text.font = NULL;

  if(text->text.text) g_free(text->text.text);
  text->text.text = NULL;
}

static void 
gtk_plot_canvas_text_draw 		(GtkPlotCanvas *canvas,
					 GtkPlotCanvasChild *child)
{
  GtkPlotCanvasText *text_child = GTK_PLOT_CANVAS_TEXT(child);
  GtkPlotText *text = &text_child->text;
  gdouble m = canvas->magnification;
  gint x, y;

  x = text->x * canvas->pixmap_width;
  y = text->y * canvas->pixmap_height;

  gtk_plot_pc_draw_string(canvas->pc,
                         x, y,
                         text->angle,
                         &text->fg,
                         &text->bg,
                         text->transparent,
                         text->border,
                         roundint(m * text->border_space),
                         roundint(m * text->border_width),
                         roundint(m * text->shadow_width),
                         text->font,
                         roundint(m * text->height),
                         text->justification,
                         text->text);
}

static void
gtk_plot_canvas_text_size_allocate(GtkPlotCanvas *canvas, GtkPlotCanvasChild *child)
{
  GtkPlotCanvasText *text = GTK_PLOT_CANVAS_TEXT(child);
  gint tx, ty, x, y, width, height;
  gdouble m = canvas->magnification;

  text->text.x = child->rx1;
  text->text.y = child->ry1;

  x = roundint(text->text.x * canvas->pixmap_width);
  y = roundint(text->text.y * canvas->pixmap_height);

  gtk_plot_text_get_area(text->text.text, text->text.angle, 
                         text->text.justification,
                         text->text.font, roundint(m * text->text.height),
                         &tx, &ty, &width, &height);

  if(text->text.border != GTK_PLOT_BORDER_NONE){
     tx -= text->text.border_space;
     ty -= text->text.border_space;
     width += 2 * text->text.border_space;
     height += 2 * text->text.border_space;
  }
                                                                                
  tx += x;
  ty += y;
  child->allocation.x = tx;
  child->allocation.y = ty;
  child->allocation.width = width;
  child->allocation.height = height;

  gtk_plot_canvas_get_position(canvas, tx + width, ty + height,
                               &child->rx2, &child->ry2);

}

void
gtk_plot_canvas_text_set_attributes(GtkPlotCanvasText *text,
			  const gchar *font, gint height, gint angle,
                          const GdkColor *fg, const GdkColor *bg,
                          gboolean transparent,
                          GtkJustification justification,
                          const gchar *real_text)
{
  if(font){ g_free(text->text.font); text->text.font = g_strdup(font); }
  if(real_text){ g_free(text->text.text); text->text.text = g_strdup(real_text); }
  text->text.justification = justification;
  text->text.height = height;
  text->text.angle = angle;
  text->text.transparent = transparent;
  if(fg) text->text.fg = *fg;
  if(bg) text->text.bg = *bg;
}


