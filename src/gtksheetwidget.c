/* GtkSheet widget for Gtk+.
 * Copyright (C) 1999-2001 Adrian E. Feiguin <adrian@ifir.ifir.edu.ar>
 *
 * Based on GtkClist widget by Jay Painter, but major changes.
 * Memory allocation routines inspired on SC (Spreadsheet Calculator)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.gd
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION: gtksheet
 * @short_description: A spreadsheet widget for Gtk2
 *
 * #GtkSheet is a matrix widget for GTK+. It consists of an scrollable grid of cells where you can allocate text.
 * Cell contents can be edited interactively through a specially designed entry, GtkItemEntry.
 * It is also a container subclass, allowing you to display buttons, curves, pixmaps and any other widget in it.
 * You can also set many attributes as: border, foreground and background color, text justification, and more.
 * The testgtksheet program shows how easy is to create a spreadsheet-like GUI using this widget set. 
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <pango/pango.h>
#include "gtkitementry.h"
#include "gtksheetwidget.h"
#include "gtksheet-marshal.h"
#include "gtksheettypebuiltins.h"

/* sheet flags */
enum
{ 
  GTK_SHEET_IS_LOCKED       = 1 << 0,
  GTK_SHEET_IS_FROZEN       = 1 << 1,
  GTK_SHEET_IN_XDRAG        = 1 << 2,
  GTK_SHEET_IN_YDRAG        = 1 << 3,
  GTK_SHEET_IN_DRAG         = 1 << 4,
  GTK_SHEET_IN_SELECTION    = 1 << 5,
  GTK_SHEET_IN_RESIZE       = 1 << 6,
  GTK_SHEET_IN_CLIP         = 1 << 7,
  GTK_SHEET_REDRAW_PENDING  = 1 << 8,
};

#define GTK_SHEET_FLAGS(sheet)             (GTK_SHEET (sheet)->flags)
#define GTK_SHEET_SET_FLAGS(sheet,flag)    (GTK_SHEET_FLAGS (sheet) |= (flag))
#define GTK_SHEET_UNSET_FLAGS(sheet,flag)  (GTK_SHEET_FLAGS (sheet) &= ~(flag))

#define GTK_SHEET_IS_FROZEN(sheet)   (GTK_SHEET_FLAGS (sheet) & GTK_SHEET_IS_FROZEN)
#define GTK_SHEET_IN_XDRAG(sheet)    (GTK_SHEET_FLAGS (sheet) & GTK_SHEET_IN_XDRAG)
#define GTK_SHEET_IN_YDRAG(sheet)    (GTK_SHEET_FLAGS (sheet) & GTK_SHEET_IN_YDRAG)
#define GTK_SHEET_IN_DRAG(sheet)     (GTK_SHEET_FLAGS (sheet) & GTK_SHEET_IN_DRAG)
#define GTK_SHEET_IN_SELECTION(sheet) (GTK_SHEET_FLAGS (sheet) & GTK_SHEET_IN_SELECTION)
#define GTK_SHEET_IN_RESIZE(sheet) (GTK_SHEET_FLAGS (sheet) & GTK_SHEET_IN_RESIZE)
#define GTK_SHEET_IN_CLIP(sheet) (GTK_SHEET_FLAGS (sheet) & GTK_SHEET_IN_CLIP)
#define GTK_SHEET_REDRAW_PENDING(sheet)   (GTK_SHEET_FLAGS (sheet) & GTK_SHEET_REDRAW_PENDING)
 
#define CELL_SPACING 1
#define DRAG_WIDTH 6
#define TIMEOUT_SCROLL 20
#define TIMEOUT_FLASH 200
#define TIME_INTERVAL 8
#define COLUMN_MIN_WIDTH 10
#define MINROWS 1
#define MINCOLS 1
#define MAXLENGTH 30
#define CELLOFFSET 4
#define DEFAULT_COLUMN_WIDTH 80
#define DEFAULT_SHEET_TITLE "untitled"

static inline gint
gtk_sheet_row_height(GtkSheet *sheet, gint row)
{
  return sheet->row[row].height;
}

static inline gint
gtk_sheet_column_width(GtkSheet *sheet, gint col)
{
  return sheet->column[col].width;
}

static inline guint DEFAULT_ROW_HEIGHT(GtkWidget *widget) 
{ 
  if(!widget->style->font_desc) return 24;
  else {
    PangoContext *context = gtk_widget_get_pango_context(widget); 
    PangoFontMetrics *metrics = pango_context_get_metrics(context,
                                  widget->style->font_desc,
                                  pango_context_get_language(context)); 
    guint val = pango_font_metrics_get_descent(metrics) + 
                pango_font_metrics_get_ascent(metrics);
    pango_font_metrics_unref(metrics);
    return PANGO_PIXELS(val)+2*CELLOFFSET;
  }
}
static inline guint DEFAULT_FONT_ASCENT(GtkWidget *widget) 
{ 
  if(!widget->style->font_desc) return 12;
  else {
    PangoContext *context = gtk_widget_get_pango_context(widget); 
    PangoFontMetrics *metrics = pango_context_get_metrics(context,
                                  widget->style->font_desc,
                                  pango_context_get_language(context)); 
    guint val = pango_font_metrics_get_ascent(metrics);
    pango_font_metrics_unref(metrics);
    return PANGO_PIXELS(val);
  }
}
static inline guint STRING_WIDTH(GtkWidget *widget,
                                 PangoFontDescription *font, const gchar *text)
{
  PangoRectangle rect;
  PangoLayout *layout;

  layout = gtk_widget_create_pango_layout (widget, text);
  pango_layout_set_font_description (layout, font);

  pango_layout_get_extents (layout, NULL, &rect);

  g_object_unref(G_OBJECT(layout));
  return PANGO_PIXELS(rect.width);
}

static inline guint DEFAULT_FONT_DESCENT(GtkWidget *widget) 
{ 
  if(!widget->style->font_desc) return 12;
  else {
    PangoContext *context = gtk_widget_get_pango_context(widget); 
    PangoFontMetrics *metrics = pango_context_get_metrics(context,
                                  widget->style->font_desc,
                                  pango_context_get_language(context)); 
    guint val =  pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);
    return PANGO_PIXELS(val);
  }
}

/* gives the top pixel of the given row in context of
 * the sheet's voffset */
static inline gint
ROW_TOP_YPIXEL(GtkSheet *sheet, gint nrow)
{
   return (sheet->voffset + sheet->row[nrow].top_ypixel);
}


/* returns the row index from a y pixel location in the 
 * context of the sheet's voffset */
static inline gint 
ROW_FROM_YPIXEL(GtkSheet *sheet, gint y)
{
  gint i, cy;

  cy = sheet->voffset;
  if(sheet->column_titles_visible) cy += sheet->column_title_area.height;
  if(y < cy) return 0;
  for (i = 0; i <= sheet->maxrow; i++)
    {
      if (y >= cy  && y <= (cy + sheet->row[i].height) && sheet->row[i].is_visible)
	return i;
      if(sheet->row[i].is_visible) cy += sheet->row[i].height;

    }

  /* no match */
  return sheet->maxrow;
}


/* gives the left pixel of the given column in context of
 * the sheet's hoffset */
static inline gint
COLUMN_LEFT_XPIXEL(GtkSheet *sheet, gint ncol)
{
   return (sheet->hoffset + sheet->column[ncol].left_xpixel);
}


static inline gint
COLUMN_FROM_XPIXEL (GtkSheet * sheet,
		    gint x)
{
  gint i, cx;

  cx = sheet->hoffset;
  if(sheet->row_titles_visible) cx += sheet->row_title_area.width;
  if(x < cx) return 0;
  for (i = 0; i <= sheet->maxcol; i++)
    {
      if (x >= cx  && x <= (cx + sheet->column[i].width) && sheet->column[i].is_visible)
	return i;
      if(sheet->column[i].is_visible) cx += sheet->column[i].width;

    }

  /* no match */
  return sheet->maxcol;
}


/* returns the total height of the sheet */
static inline gint SHEET_HEIGHT(GtkSheet *sheet)
{
  gint i,cx;

  cx = ( sheet->column_titles_visible ? sheet->column_title_area.height : 0);
 
  for (i=0;i<=sheet->maxrow; i++)
   if(sheet->row[i].is_visible) cx += sheet->row[i].height;
  
  return cx;
}


/* returns the total width of the sheet */
static inline gint SHEET_WIDTH(GtkSheet *sheet)
{
  gint i,cx;

  cx = ( sheet->row_titles_visible ? sheet->row_title_area.width : 0);
 
  for (i=0;i<=sheet->maxcol; i++)
   if(sheet->column[i].is_visible) cx += sheet->column[i].width;
  
  return cx;
}

#define MIN_VISIBLE_ROW(sheet) sheet->view.row0
#define MAX_VISIBLE_ROW(sheet) sheet->view.rowi
#define MIN_VISIBLE_COLUMN(sheet) sheet->view.col0
#define MAX_VISIBLE_COLUMN(sheet) sheet->view.coli


static inline gint
POSSIBLE_XDRAG(GtkSheet *sheet, gint x, gint *drag_column)
{
 gint column, xdrag;

 column=COLUMN_FROM_XPIXEL(sheet, x);
 *drag_column=column;

 xdrag=COLUMN_LEFT_XPIXEL(sheet,column)+CELL_SPACING;
 if(x <= xdrag+DRAG_WIDTH/2 && column != 0){
   while(!sheet->column[column-1].is_visible && column>0) column--;
   *drag_column=column-1;
   return sheet->column[column-1].is_sensitive;
 }

 xdrag+=sheet->column[column].width;
 if(x >= xdrag-DRAG_WIDTH/2 && x <= xdrag+DRAG_WIDTH/2)
   return sheet->column[column].is_sensitive;

 return FALSE;
} 

static inline gint
POSSIBLE_YDRAG(GtkSheet *sheet, gint y, gint *drag_row)
{
 gint row, ydrag;

 row=ROW_FROM_YPIXEL(sheet, y);
 *drag_row=row;

 ydrag=ROW_TOP_YPIXEL(sheet,row)+CELL_SPACING;
 if(y <= ydrag+DRAG_WIDTH/2 && row != 0){
   while(!sheet->row[row-1].is_visible && row>0) row--;
   *drag_row=row-1;
   return sheet->row[row-1].is_sensitive;
 }

 ydrag+=sheet->row[row].height;

 if(y >= ydrag-DRAG_WIDTH/2 && y <= ydrag+DRAG_WIDTH/2)
   return sheet->row[row].is_sensitive;
 
 
 return FALSE;
}        

static inline gint POSSIBLE_DRAG(GtkSheet *sheet, gint x, gint y,
                            gint *drag_row, gint *drag_column)
{
  gint ydrag, xdrag;

  *drag_column=COLUMN_FROM_XPIXEL(sheet,x);
  *drag_row=ROW_FROM_YPIXEL(sheet,y);

  if(x>=COLUMN_LEFT_XPIXEL(sheet,sheet->range.col0)-DRAG_WIDTH/2 &&
     x<=COLUMN_LEFT_XPIXEL(sheet,sheet->range.coli)+
        sheet->column[sheet->range.coli].width+DRAG_WIDTH/2){
     ydrag=ROW_TOP_YPIXEL(sheet,sheet->range.row0);
     if(y>=ydrag-DRAG_WIDTH/2 && y<=ydrag+DRAG_WIDTH/2){
        *drag_row=sheet->range.row0;
        return TRUE;
     }
     ydrag=ROW_TOP_YPIXEL(sheet,sheet->range.rowi)+
           sheet->row[sheet->range.rowi].height;
     if(y>=ydrag-DRAG_WIDTH/2 && y<=ydrag+DRAG_WIDTH/2){
        *drag_row=sheet->range.rowi;
        return TRUE;
     }
  }

  if(y>=ROW_TOP_YPIXEL(sheet,sheet->range.row0)-DRAG_WIDTH/2 &&
     y<=ROW_TOP_YPIXEL(sheet,sheet->range.rowi)+
        sheet->row[sheet->range.rowi].height+DRAG_WIDTH/2){
     xdrag=COLUMN_LEFT_XPIXEL(sheet,sheet->range.col0);
     if(x>=xdrag-DRAG_WIDTH/2 && x<=xdrag+DRAG_WIDTH/2){
        *drag_column=sheet->range.col0;
        return TRUE;
     }
     xdrag=COLUMN_LEFT_XPIXEL(sheet,sheet->range.coli)+
           sheet->column[sheet->range.coli].width;
     if(x>=xdrag-DRAG_WIDTH/2 && x<=xdrag+DRAG_WIDTH/2){
        *drag_column=sheet->range.coli;
        return TRUE;
     }
  }
  return FALSE;
}

static inline gint POSSIBLE_RESIZE(GtkSheet *sheet, gint x, gint y,
                            gint *drag_row, gint *drag_column)
{
  gint xdrag, ydrag;
  
  xdrag=COLUMN_LEFT_XPIXEL(sheet,sheet->range.coli)+
           sheet->column[sheet->range.coli].width;

  ydrag=ROW_TOP_YPIXEL(sheet,sheet->range.rowi)+
           sheet->row[sheet->range.rowi].height;

  if(sheet->state == GTK_SHEET_COLUMN_SELECTED) 
        ydrag = ROW_TOP_YPIXEL(sheet, sheet->view.row0);

  if(sheet->state == GTK_SHEET_ROW_SELECTED)
        xdrag = COLUMN_LEFT_XPIXEL(sheet, sheet->view.col0);

  *drag_column=COLUMN_FROM_XPIXEL(sheet,x);
  *drag_row=ROW_FROM_YPIXEL(sheet,y);

  if(x>=xdrag-DRAG_WIDTH/2 && x<=xdrag+DRAG_WIDTH/2 &&
     y>=ydrag-DRAG_WIDTH/2 && y<=ydrag+DRAG_WIDTH/2) return TRUE;

  return FALSE;  
}

static void gtk_sheet_destroy 			(GtkObject * object);
static void gtk_sheet_finalize 			(GObject * object);
static void gtk_sheet_style_set 		(GtkWidget *widget,
		                 		 GtkStyle  *previous_style);
static void gtk_sheet_realize 			(GtkWidget * widget);
static void gtk_sheet_unrealize 		(GtkWidget * widget);
static void gtk_sheet_map 			(GtkWidget * widget);
static void gtk_sheet_unmap 			(GtkWidget * widget);
static gint gtk_sheet_expose 			(GtkWidget * widget,
		  				 GdkEventExpose * event);
static void gtk_sheet_forall 			(GtkContainer *container,
                              			 gboolean include_internals,
                              			 GtkCallback  callback, 
                              			 gpointer  callback_data); 

static void gtk_sheet_set_scroll_adjustments	(GtkSheet *sheet,
						 GtkAdjustment *hadjustment,
						 GtkAdjustment *vadjustment);

static gint gtk_sheet_button_press 		(GtkWidget * widget,
						 GdkEventButton * event);
static gint gtk_sheet_button_release 		(GtkWidget * widget,
						 GdkEventButton * event);
static gint gtk_sheet_motion 			(GtkWidget * widget,
		  				 GdkEventMotion * event);
static gint gtk_sheet_entry_key_press		(GtkWidget *widget,
		                		 GdkEventKey *key);
static gint gtk_sheet_key_press			(GtkWidget *widget,
		                		 GdkEventKey *key);
static void gtk_sheet_size_request 		(GtkWidget * widget,
			            	 	 GtkRequisition * requisition);
static void gtk_sheet_size_allocate 		(GtkWidget * widget,
			             		 GtkAllocation * allocation);

/* Sheet queries */

static gint gtk_sheet_range_isvisible 		(GtkSheet * sheet,
			 			 GtkSheetRange range);
static gint gtk_sheet_cell_isvisible 		(GtkSheet * sheet,
			  			 gint row, gint column);
/* Clipped Range */

static gint gtk_sheet_scroll			(gpointer data);
static gint gtk_sheet_flash			(gpointer data);

/* Drawing Routines */

/* draw cell background and frame */
static void gtk_sheet_cell_draw_default 	(GtkSheet *sheet, 
						 gint row, gint column);

/* draw cell border */
static void gtk_sheet_cell_draw_border 		(GtkSheet *sheet, 
						 gint row, gint column, 
						 gint mask);

/* draw cell contents */
static void gtk_sheet_cell_draw_label 		(GtkSheet *sheet, 
						 gint row, gint column);

/* draw visible part of range. If range==NULL then draw the whole screen */
static void gtk_sheet_range_draw		(GtkSheet *sheet, 
						 const GtkSheetRange *range);

/* highlight the visible part of the selected range */
static void gtk_sheet_range_draw_selection	(GtkSheet *sheet, 
						 GtkSheetRange range);

/* Selection */

static gint gtk_sheet_move_query		(GtkSheet *sheet, 
						 gint row, gint column);
static void gtk_sheet_real_select_range 	(GtkSheet * sheet,
			                 	 GtkSheetRange * range);
static void gtk_sheet_real_unselect_range 	(GtkSheet * sheet,
			                 	 const GtkSheetRange * range);
static void gtk_sheet_extend_selection		(GtkSheet *sheet, 
						 gint row, gint column);
static void gtk_sheet_new_selection		(GtkSheet *sheet, 
						 GtkSheetRange *range);
static void gtk_sheet_draw_border 		(GtkSheet *sheet, 
						 GtkSheetRange range);
static void gtk_sheet_draw_corners		(GtkSheet *sheet,
						 GtkSheetRange range);


/* Active Cell handling */

static void gtk_sheet_entry_changed		(GtkWidget *widget, 
						 gpointer data);
static gboolean gtk_sheet_deactivate_cell	(GtkSheet *sheet);
static void gtk_sheet_hide_active_cell		(GtkSheet *sheet);
static gboolean gtk_sheet_activate_cell		(GtkSheet *sheet, 
						 gint row, gint col);
static void gtk_sheet_draw_active_cell		(GtkSheet *sheet);
static void gtk_sheet_show_active_cell		(GtkSheet *sheet);
static void gtk_sheet_click_cell		(GtkSheet *sheet, 
                                 		 gint row, 
                                		 gint column,
                                 		 gboolean *veto);

/* Backing Pixmap */

static void gtk_sheet_make_backing_pixmap 	(GtkSheet *sheet, 
						 guint width, guint height);
static void gtk_sheet_draw_backing_pixmap	(GtkSheet *sheet, 
						 GtkSheetRange range);
/* Scrollbars */

static void adjust_scrollbars 			(GtkSheet * sheet);
static void vadjustment_changed		 	(GtkAdjustment * adjustment,
			       			 gpointer data);
static void hadjustment_changed 		(GtkAdjustment * adjustment,
			       			 gpointer data);
static void vadjustment_value_changed 		(GtkAdjustment * adjustment,
				     		 gpointer data);
static void hadjustment_value_changed 		(GtkAdjustment * adjustment,
				     		 gpointer data);


static void draw_xor_vline 			(GtkSheet * sheet);
static void draw_xor_hline 			(GtkSheet * sheet);
static void draw_xor_rectangle			(GtkSheet *sheet, 
						 GtkSheetRange range);
static void gtk_sheet_draw_flashing_range	(GtkSheet *sheet, 
						 GtkSheetRange range);
static guint new_column_width 			(GtkSheet * sheet,
		  				 gint column,
		  				 gint * x);
static guint new_row_height 			(GtkSheet * sheet,
		  				 gint row,
		  				 gint * y);
/* Sheet Button */

static void create_global_button		(GtkSheet *sheet);
static void global_button_clicked		(GtkWidget *widget, 
						 gpointer data);
/* Sheet Entry */

static void create_sheet_entry			(GtkSheet *sheet);
static void gtk_sheet_size_allocate_entry	(GtkSheet *sheet);
static void gtk_sheet_entry_set_max_size	(GtkSheet *sheet);

/* Sheet button gadgets */

static void size_allocate_column_title_buttons 	(GtkSheet * sheet);
static void size_allocate_row_title_buttons 	(GtkSheet * sheet);
static void gtk_sheet_recalc_top_ypixels	(GtkSheet *sheet, 
						 gint row);
static void gtk_sheet_recalc_left_xpixels	(GtkSheet *sheet, 
						 gint column);
static void row_button_set 			(GtkSheet *sheet, 
						 gint row);
static void column_button_set 			(GtkSheet *sheet, 
						 gint column);
static void row_button_release 			(GtkSheet *sheet, 
						 gint row);
static void column_button_release 		(GtkSheet *sheet, 
						 gint column);
static void gtk_sheet_button_draw		(GtkSheet *sheet, 
						 gint row, gint column);
static void size_allocate_global_button 	(GtkSheet *sheet);
static void gtk_sheet_button_size_request	(GtkSheet *sheet,
                                 		 GtkSheetButton *button, 
                                 		 GtkRequisition *requisition);

/* Attributes routines */

static void gtk_sheet_set_cell_attributes	(GtkSheet *sheet, 
						 gint row, gint col,
						 GtkSheetCellAttr attributes);

static void init_attributes			(GtkSheet *sheet, gint col,  
						 GtkSheetCellAttr *attributes);
/* Memory allocation routines */
static void gtk_sheet_real_range_clear 		(GtkSheet *sheet, 
						 const GtkSheetRange *range, 
                            			 gboolean delete);
static void gtk_sheet_real_cell_clear 		(GtkSheet *sheet, 
						 gint row,
						 gint column,
						 gboolean delete);
static GtkSheetCell * gtk_sheet_cell_new 	(void);
static gint AddRow				(GtkSheet *sheet, gint nrows);
static gint AddColumn				(GtkSheet *sheet, gint ncols);
static gint InsertRow				(GtkSheet *sheet, gint row, gint nrows);
static gint InsertColumn			(GtkSheet *sheet, gint col, gint ncols);
static gint DeleteRow				(GtkSheet *sheet, gint row, gint nrows);
static gint DeleteColumn			(GtkSheet *sheet, gint col, gint ncols);
static gint GrowSheet				(GtkSheet *sheet, 
						 gint newrows, gint newcols);
static gint CheckBounds				(GtkSheet *sheet, 
						 gint row, gint col);

/* Container Functions */
static void gtk_sheet_remove			(GtkContainer *container,
						 GtkWidget *widget);
static void gtk_sheet_realize_child		(GtkSheet *sheet,
						 GtkSheetChild *child);
static void gtk_sheet_position_child		(GtkSheet *sheet,
						 GtkSheetChild *child);
static void gtk_sheet_position_children		(GtkSheet *sheet);
static void gtk_sheet_child_show		(GtkSheetChild *child); 
static void gtk_sheet_child_hide		(GtkSheetChild *child); 
static void gtk_sheet_column_size_request       (GtkSheet *sheet,
                                                 gint col,
                                                 guint *requisition);
static void gtk_sheet_row_size_request          (GtkSheet *sheet,
                                                 gint row,
                                                 guint *requisition);


/* Signals */

extern void 
_gtksheet_signal_emit(GtkObject *object, guint signal_id, ...);

enum {
      SELECT_ROW, 
      SELECT_COLUMN, 
      SELECT_RANGE,
      CLIP_RANGE,
      RESIZE_RANGE,
      MOVE_RANGE,
      TRAVERSE, 
      DEACTIVATE, 
      ACTIVATE,
      SET_CELL,
      CLEAR_CELL,
      CHANGED,
      NEW_COL_WIDTH,
      NEW_ROW_HEIGHT,
      LAST_SIGNAL
};

static guint sheet_signals[LAST_SIGNAL] = {0};

/* SheetRange type */
static GtkSheetRange*
gtk_sheet_range_copy (const GtkSheetRange *range)
{
  GtkSheetRange *new_range;

  g_return_val_if_fail (range != NULL, NULL);

  new_range = g_new (GtkSheetRange, 1);

  *new_range = *range;

  return new_range;
}

static void
gtk_sheet_range_free (GtkSheetRange *range)
{
  g_return_if_fail (range != NULL);

  g_free (range);
}

GType
gtk_sheet_range_get_type (void)
{
  static GType sheet_range_type=0;

  if(sheet_range_type == 0)
  {
    sheet_range_type = g_boxed_type_register_static("GtkSheetRange", 
                                        (GBoxedCopyFunc)gtk_sheet_range_copy, 
                                        (GBoxedFreeFunc)gtk_sheet_range_free);
  }
  return sheet_range_type;

}

/* SheetCellBorder type */
void
gtk_sheet_cell_border_free(GtkSheetCellBorder *border)
{
    g_free(border);
}

GtkSheetCellBorder *
gtk_sheet_cell_border_copy(const GtkSheetCellBorder *other)
{
    GtkSheetCellBorder *border = g_new(GtkSheetCellBorder, 1);
    *border = *other;
    return border;
}

GType
gtk_sheet_cell_border_get_type (void)
{
  static GType our_type = 0;

  if (our_type == 0)
    our_type = g_boxed_type_register_static ("GtkSheetCellBorder",
					     (GBoxedCopyFunc) gtk_sheet_cell_border_copy,
					     (GBoxedFreeFunc) gtk_sheet_cell_border_free);
  return our_type;
}

/* SheetCellAttr type */
void
gtk_sheet_cell_attr_free(GtkSheetCellAttr *attrs)
{
    if (attrs->font_desc != NULL)
        pango_font_description_free(attrs->font_desc);

    g_free(attrs);
}

GtkSheetCellAttr *
gtk_sheet_cell_attr_copy(const GtkSheetCellAttr *other)
{
    GtkSheetCellAttr *attrs = g_new(GtkSheetCellAttr, 1);
    *attrs = *other;
    if (other->font_desc != NULL)
        attrs->font_desc = pango_font_description_copy(other->font_desc);
        
    return attrs;
}

GType
gtk_sheet_cell_attr_get_type (void)
{
  static GType our_type = 0;

  if (our_type == 0)
    our_type = g_boxed_type_register_static ("GtkSheetCellAttr",
					     (GBoxedCopyFunc) gtk_sheet_cell_attr_copy,
					     (GBoxedFreeFunc) gtk_sheet_cell_attr_free);
  return our_type;
}

/* Child properties */
enum
{
  CHILD_PROP_0,
  CHILD_PROP_X,
  CHILD_PROP_Y,
  CHILD_PROP_ATTACHED_TO_CELL,
  CHILD_PROP_FLOATING,
  CHILD_PROP_ROW,
  CHILD_PROP_COLUMN,
  CHILD_PROP_X_OPTIONS,
  CHILD_PROP_Y_OPTIONS,
  CHILD_PROP_X_PADDING,
  CHILD_PROP_Y_PADDING  
};

static void
gtk_sheet_set_child_property (GtkContainer    *container,
			      GtkWidget       *child,
			      guint            property_id,
			      const GValue    *value,
			      GParamSpec      *pspec)
{
  GtkSheet *sheet = GTK_SHEET (container);
  GtkSheetChild *sheet_child;
  GList *list;
  guint row, column;

  sheet_child = NULL;
  for (list = sheet->children; list; list = list->next) {
    sheet_child = list->data;
    if (sheet_child->widget == child)
      break;
  }
  if (!list) {
    GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
    return;
  }

  switch (property_id)
  {
    case CHILD_PROP_X:
      sheet_child->x = g_value_get_int(value);
      break;
    case CHILD_PROP_Y:
      sheet_child->y = g_value_get_int(value);
      break;
    case CHILD_PROP_ROW:
      row = g_value_get_int(value);
      if (row >= 0 && row <= sheet->maxrow)
          sheet_child->row = row;
      else {
          /* Warn about invalid property value */
          g_warning("Invalid row number.");
          return;
      }
      break;
    case CHILD_PROP_COLUMN:
      column = g_value_get_int(value);
      if (column >= 0 && column <= sheet->maxcol)
          sheet_child->col = column;
      else {
          /* Warn about invalid property value */
          g_warning("Invalid column number.");
          return;
      }
      break;
    case CHILD_PROP_ATTACHED_TO_CELL:
      sheet_child->attached_to_cell = g_value_get_boolean(value);
      break;
    case CHILD_PROP_FLOATING:
      sheet_child->floating = g_value_get_boolean(value);
      break;
    case CHILD_PROP_X_OPTIONS:
      sheet_child->xexpand = (g_value_get_flags (value) & GTK_EXPAND) != 0;
      sheet_child->xshrink = (g_value_get_flags (value) & GTK_SHRINK) != 0;
      sheet_child->xfill = (g_value_get_flags (value) & GTK_FILL) != 0;
      break;
    case CHILD_PROP_Y_OPTIONS:
      sheet_child->yexpand = (g_value_get_flags (value) & GTK_EXPAND) != 0;
      sheet_child->yshrink = (g_value_get_flags (value) & GTK_SHRINK) != 0;
      sheet_child->yfill = (g_value_get_flags (value) & GTK_FILL) != 0;
      break;
    case CHILD_PROP_X_PADDING:
      sheet_child->xpadding = g_value_get_uint (value);
      break;
    case CHILD_PROP_Y_PADDING:
      sheet_child->ypadding = g_value_get_uint (value);
      break;
    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      break;
  }
  if (GTK_WIDGET_VISIBLE (child) && GTK_WIDGET_VISIBLE (sheet))
    gtk_widget_queue_resize (child);
}

static void
gtk_sheet_get_child_property (GtkContainer    *container,
			      GtkWidget       *child,
			      guint            property_id,
			      GValue          *value,
			      GParamSpec      *pspec)
{
  GtkSheet *sheet = GTK_SHEET (container);
  GtkSheetChild *sheet_child;
  GList *list;

  sheet_child = NULL;
  for (list = sheet->children; list; list = list->next) {
    sheet_child = list->data;
    if (sheet_child->widget == child)
      break;
  }
  if (!list) {
    GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
    return;
  }

  switch (property_id)
  {
    case CHILD_PROP_X:
      g_value_set_int(value, sheet_child->x);
      break;
    case CHILD_PROP_Y:
      g_value_set_int(value, sheet_child->y);
      break;
    case CHILD_PROP_ROW:
      g_value_set_int(value, sheet_child->row);
      break;
    case CHILD_PROP_COLUMN:
      g_value_set_int(value, sheet_child->col);
      break;
    case CHILD_PROP_ATTACHED_TO_CELL:
      g_value_set_boolean(value, sheet_child->attached_to_cell);
      break;
    case CHILD_PROP_FLOATING:
      g_value_set_boolean(value, sheet_child->floating);
      break;
    case CHILD_PROP_X_OPTIONS:
      g_value_set_flags (value, (sheet_child->xexpand * GTK_EXPAND |
				                 sheet_child->xshrink * GTK_SHRINK |
				                 sheet_child->xfill * GTK_FILL));
      break;
    case CHILD_PROP_Y_OPTIONS:
      g_value_set_flags (value, (sheet_child->yexpand * GTK_EXPAND |
				                 sheet_child->yshrink * GTK_SHRINK |
				                 sheet_child->yfill * GTK_FILL));
      break;
    case CHILD_PROP_X_PADDING:
      g_value_set_uint (value, sheet_child->xpadding);
      break;
    case CHILD_PROP_Y_PADDING:
      g_value_set_uint (value, sheet_child->ypadding);
      break;
    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      break;
  }
}

/* Properties */
enum {
    PROP_0,

    PROP_AUTORESIZE,
    PROP_AUTOSCROLL,
    PROP_CLIP_TEXT,
    PROP_GRID_VISIBLE,
    PROP_JUSTIFY_ENTRY,
    PROP_LOCKED,
    PROP_STATE,
    PROP_COLUMNS_RESIZABLE,
    PROP_COLUMN_TITLES_VISIBLE,
    PROP_ROWS_RESIZABLE,
    PROP_ROW_TITLES_VISIBLE,
    PROP_TITLE,
    PROP_ACTIVE_CELL,
    PROP_SELECTED_RANGE,
    PROP_N_ROWS,
    PROP_N_COLUMNS,
    PROP_SELECTION_MODE,
};

static void
gtk_sheet_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
  GtkSheet *self = GTK_SHEET (object);
  gchar *mytitle;

  switch (property_id)
    {
    case PROP_AUTORESIZE:
      gtk_sheet_set_autoresize(self, g_value_get_boolean(value));
      break;

    case PROP_AUTOSCROLL:
      gtk_sheet_set_autoscroll(self, g_value_get_boolean(value));
      break;

    case PROP_CLIP_TEXT:
      gtk_sheet_set_clip_text(self, g_value_get_boolean(value));
      break;

    case PROP_GRID_VISIBLE:
      gtk_sheet_show_grid(self, g_value_get_boolean(value));

    case PROP_JUSTIFY_ENTRY:
      gtk_sheet_set_justify_entry(self, g_value_get_boolean(value));
      break;

    case PROP_LOCKED:
      gtk_sheet_set_locked(self, g_value_get_boolean(value));
      break;

    case PROP_COLUMNS_RESIZABLE:
      gtk_sheet_columns_set_resizable(self, g_value_get_boolean(value));
      break;

    case PROP_COLUMN_TITLES_VISIBLE:
      if (g_value_get_boolean(value))
        gtk_sheet_show_column_titles(self);
      else
        gtk_sheet_hide_column_titles(self);      
      break;

    case PROP_ROWS_RESIZABLE:
      gtk_sheet_rows_set_resizable(self, g_value_get_boolean(value));
      break;

    case PROP_ROW_TITLES_VISIBLE:
      if (g_value_get_boolean(value))
          gtk_sheet_show_row_titles(self);
      else
          gtk_sheet_hide_row_titles(self);
      break;

    case PROP_TITLE:      
      mytitle = g_value_dup_string(value);
      gtk_sheet_set_title(self, mytitle);
      g_free(mytitle);      
      break;

    case PROP_SELECTION_MODE:
      /* Currently only selection single and selection browse are supported */
      if (g_value_get_enum(value) == GTK_SELECTION_SINGLE)
        gtk_sheet_set_selection_mode(self, GTK_SELECTION_SINGLE);
      else
        gtk_sheet_set_selection_mode(self, GTK_SELECTION_BROWSE);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_sheet_get_property (GObject    *object,
                        guint       property_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
  GtkSheet *self = GTK_SHEET (object);
  GValue auxval = {0,};
  GValueArray *valarray;

  switch (property_id)
  {
    case PROP_AUTORESIZE:
      g_value_set_boolean (value, gtk_sheet_autoresize(self));
      break;

    case PROP_AUTOSCROLL:
      g_value_set_boolean (value, gtk_sheet_autoscroll(self));
      break;

    case PROP_CLIP_TEXT:
      g_value_set_boolean (value, gtk_sheet_clip_text(self));
      break;

    case PROP_GRID_VISIBLE:
      g_value_set_boolean (value, gtk_sheet_grid_visible(self));
      break;

    case PROP_JUSTIFY_ENTRY:
      g_value_set_boolean (value, gtk_sheet_justify_entry(self));
      break;

    case PROP_LOCKED:
      g_value_set_boolean (value, gtk_sheet_locked(self));
      break;

    case PROP_STATE:
      g_value_set_enum (value, gtk_sheet_get_state(self));
      break;

    case PROP_COLUMNS_RESIZABLE:
      g_value_set_boolean(value, gtk_sheet_columns_resizable(self));
      break;

    case PROP_COLUMN_TITLES_VISIBLE:
      g_value_set_boolean(value, gtk_sheet_column_titles_visible(self));
      break;

    case PROP_ROWS_RESIZABLE:
      g_value_set_boolean(value, gtk_sheet_rows_resizable(self));
      break;

    case PROP_ROW_TITLES_VISIBLE:
      g_value_set_boolean(value, gtk_sheet_row_titles_visible(self));
      break;

    case PROP_TITLE:
      g_value_set_string(value, self->name);
      break;

    case PROP_ACTIVE_CELL:
      /* Create a GValueArray of gint-containing GValues with the active_cells
         row and col. */
      valarray = g_value_array_new(2);
      g_value_init(&auxval, G_TYPE_INT);
      g_value_set_int(&auxval, self->active_cell.row);      
      g_value_array_append(valarray, &auxval); 
      g_value_set_int(&auxval, self->active_cell.col);
      g_value_array_append(valarray, &auxval); 
      /* Put the GValueArray in the provided GValue, which should be 
         initialized with g_value_init(value, G_TYPE_VALUE_ARRAY) */
      g_value_set_boxed(value, valarray);
      /* Free the auxiliary GValueArray */ 
      g_value_array_free(valarray);
      break;

    case PROP_SELECTED_RANGE:
      g_value_set_boxed(value, &(self->range));
      break;

    case PROP_N_COLUMNS:
      g_value_set_uint(value, gtk_sheet_get_columns_count(self));
      break;

    case PROP_N_ROWS:
      g_value_set_uint(value, gtk_sheet_get_rows_count(self));
      break;

    case PROP_SELECTION_MODE:
      g_value_set_enum(value, self->selection_mode);
      break;

    default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }

}

/*GtkSheet type*/

G_DEFINE_TYPE(GtkSheet, gtk_sheet, GTK_TYPE_CONTAINER);

static void
gtk_sheet_class_init (GtkSheetClass * klass)
{
  GParamSpec *pspec;
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;
  container_class = (GtkContainerClass *) klass;


  container_class->add = NULL;
  container_class->remove = gtk_sheet_remove;
  container_class->forall = gtk_sheet_forall;
  container_class->set_child_property = gtk_sheet_set_child_property;
  container_class->get_child_property = gtk_sheet_get_child_property;

  object_class->destroy = gtk_sheet_destroy;
  gobject_class->finalize = gtk_sheet_finalize;
  gobject_class->set_property = gtk_sheet_set_property;
  gobject_class->get_property = gtk_sheet_get_property;

  widget_class->realize = gtk_sheet_realize;
  widget_class->unrealize = gtk_sheet_unrealize;
  widget_class->map = gtk_sheet_map;
  widget_class->unmap = gtk_sheet_unmap;
  widget_class->style_set = gtk_sheet_style_set;
  widget_class->button_press_event = gtk_sheet_button_press;
  widget_class->button_release_event = gtk_sheet_button_release;
  widget_class->motion_notify_event = gtk_sheet_motion;
  widget_class->key_press_event = gtk_sheet_key_press;
  widget_class->expose_event = gtk_sheet_expose;
  widget_class->size_request = gtk_sheet_size_request;
  widget_class->size_allocate = gtk_sheet_size_allocate;
  widget_class->focus_in_event = NULL;
  widget_class->focus_out_event = NULL;

  klass->set_scroll_adjustments = gtk_sheet_set_scroll_adjustments;
  klass->select_row = NULL;
  klass->select_column = NULL;
  klass->select_range = NULL;
  klass->clip_range = NULL;
  klass->resize_range = NULL;
  klass->move_range = NULL;
  klass->traverse = NULL;
  klass->deactivate = NULL;
  klass->activate = NULL;
  klass->set_cell = NULL;
  klass->clear_cell = NULL;
  klass->changed = NULL;

  /**
   * GtkSheet::select-row:
   * @sheet: the sheet widget that emitted the signal
   * @row: the newly selected row index
   *
   * Emmited when a row has been selected.
   */
  sheet_signals[SELECT_ROW] =
    g_signal_new ("select-row",
            G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,		    
		    G_STRUCT_OFFSET (GtkSheetClass, select_row),
            NULL,
            NULL,
            gtksheet_VOID__INT,
            G_TYPE_NONE, 
            1, G_TYPE_INT);

  /**
   * GtkSheet::select-column:
   * @sheet: the sheet widget that emitted the signal
   * @select_column: the newly selected column index
   *
   * Emmited when a column has been selected.
   */
  sheet_signals[SELECT_COLUMN] =
    g_signal_new ("select-column",
            G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,		    
		    G_STRUCT_OFFSET (GtkSheetClass, select_column),
            NULL, NULL,
		    gtksheet_VOID__INT,
            G_TYPE_NONE, 1, G_TYPE_INT);

  /**
   * GtkSheet::select-range:
   * @sheet: the sheet widget that emitted the signal
   * @select_range: the newly selected #GtkSheetRange
   *
   * Emmited when a block of cells has been selected.
   */
   sheet_signals[SELECT_RANGE] =
    g_signal_new ("select-range",
            G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,		    
		    G_STRUCT_OFFSET (GtkSheetClass, select_range),
            NULL, NULL,
            gtksheet_VOID__BOXED,
	        G_TYPE_NONE, 1, GTK_TYPE_SHEET_RANGE);
 
  /**
   * GtkSheet::clip-range:
   * @sheet: the sheet widget that emitted the signal
   * @range: the range of cells that have been marked to be copied to the clipboard
   *
   * Emitted when gtk_sheet_clip_range() method is called. The @range parameter 
   * of the callback indicates the block of cells have been marked to be copied 
   * to the clipboard. Note that your program is reposible for actually copying 
   * the cell contents to the clipboard. The clip_range() method just marks the 
   * cells to indicate that their respective contents are on the clipboard. 
   */
  sheet_signals[CLIP_RANGE] =
    g_signal_new ("clip-range",
            G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,		    
		    G_STRUCT_OFFSET (GtkSheetClass, clip_range),
            NULL, NULL,
            gtksheet_VOID__BOXED,
	        G_TYPE_NONE, 1, GTK_TYPE_SHEET_RANGE);

   /**
   * GtkSheet::resize-range:
   * @sheet: the sheet widget that emitted the signal
   * @original_range:the orignal #GtkSheetRange range of selected cells.
   * @new_range: the new resized #GtkSheetRange of selected cells 
   *
   * Emmited when a block of selected cells is resized by the user by clicking 
   * in the border of the selected cells and dragging and dropping it. 
   */
  sheet_signals[RESIZE_RANGE] =
    g_signal_new ("resize-range",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, resize_range),
            NULL, NULL,
		    gtksheet_VOID__BOXED_BOXED,
	        G_TYPE_NONE, 2, GTK_TYPE_SHEET_RANGE, GTK_TYPE_SHEET_RANGE);
  /**
   * GtkSheet::move-range:
   * @sheet: the sheet widget that emitted the signal.
   * @origin_range: a #GtkSheetRange specifying the block of cells dragged by the 
   *                user
   * @destiny_range: a #GtkSheetRange specifying the new positions for the dragged 
   *                 cells according to where the user dropped them
   *
   * Emitted when the user drags a block of selected cells and drops them in a 
   * different position in the sheet. Your program is responsible of actually 
   * replacing the contents of the @destiny_range cells with the contents of the
   * @origin_range cells. 
   */
  sheet_signals[MOVE_RANGE] =
    g_signal_new ("move-range",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, move_range),
            NULL, NULL,
		    gtksheet_VOID__BOXED_BOXED,
            G_TYPE_NONE, 2, GTK_TYPE_SHEET_RANGE, GTK_TYPE_SHEET_RANGE);

  /**
   * GtkSheet::traverse:
   * @sheet: the sheet widget that emitted the signal.
   * @row: the previously active cell row number.
   * @column: the previously active cell column number.
   * @*new_row: a pointer to the new active cell row number 
   * @*new_column: a pointer to the new active cell column number
   *
   * The "traverse" is emited before "deactivate_cell" and allows to veto the movement.
   * In such case, the entry will remain in the site and the other signals will not be emited.
   * FIXME:: Should the user modify @new_row and @new_col ?
   */
  sheet_signals[TRAVERSE] =
    g_signal_new ("traverse",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, traverse),
            NULL, NULL,
            gtksheet_BOOLEAN__INT_INT_POINTER_POINTER,
	        G_TYPE_BOOLEAN, 4, G_TYPE_INT, G_TYPE_INT,
                               G_TYPE_POINTER, G_TYPE_POINTER);

  /**
   * GtkSheet::deactivate:
   * @sheet: the sheet widget that emitted the signal
   * @row: row number of deactivated cell.
   * @column: column number of deactivated cell.
   *
   * Emmited whenever a cell is deactivated(you click on other cell or start a new selection)
   */
  sheet_signals[DEACTIVATE] =
    g_signal_new ("deactivate",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, deactivate),
            NULL, NULL,
            gtksheet_BOOLEAN__INT_INT,
	        G_TYPE_BOOLEAN, 2, G_TYPE_INT, G_TYPE_INT);

  /**
   * GtkSheet::activate:
   * @sheet: the sheet widget that emitted the signal
   * @row: row number of activated cell.
   * @column: column number of activated cell.
   *
   * Emmited whenever a cell is activated(you click on it),
   */
  sheet_signals[ACTIVATE] =
    g_signal_new ("activate",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, activate),
            NULL, NULL,
            gtksheet_BOOLEAN__INT_INT,
            G_TYPE_BOOLEAN, 2, G_TYPE_INT, G_TYPE_INT);

  /**
   * GtkSheet::set-cell:
   * @sheet: the sheet widget that emitted the signal
   * @row: row number of activated cell.
   * @column: column number of activated cell.
   *
   * Emited when clicking on a non-empty cell.
   */
  sheet_signals[SET_CELL] =
    g_signal_new ("set-cell",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, set_cell),
            NULL, NULL,
            gtksheet_VOID__INT_INT,
	        G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

  /**
   * GtkSheet::clear-cell:
   * @sheet: the sheet widget that emitted the signal
   * @row: row number of cleared cell.
   * @column: column number of cleared cell.
   *
   * Emited when when the content of the cell is erased.
   */
  sheet_signals[CLEAR_CELL] =
    g_signal_new ("clear-cell",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, clear_cell),
            NULL, NULL,
            gtksheet_VOID__INT_INT,
	        G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

  /**
   * GtkSheet::changed:
   * @sheet: the sheet widget that emitted the signal
   * @row: row number of changed cell.
   * @column: column number of changed cell.
   *
   * "Emited when typing into the active cell, changing its content.
   * It is emitted after each key press in cell and after deactivating cell.
   */
  sheet_signals[CHANGED] =
    g_signal_new ("changed",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, changed),
            NULL, NULL,
            gtksheet_VOID__INT_INT,
	        G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

  /**
   * GtkSheet::new-column-width:
   * @sheet: the sheet widget that emitted the signal
   * @row: modified row number.
   * @width: new column width
   *
   * Emited when the width of a column is modified.
   */
  sheet_signals[NEW_COL_WIDTH] =
    g_signal_new ("new-column-width",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, new_column_width),
            NULL, NULL,
            gtksheet_VOID__INT_INT,
	        G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

  /**
   * GtkSheet::new-row-height:
   * @sheet: the sheet widget that emitted the signal
   * @col: modified dolumn number.
   * @height: new row height.
   *
   * Emited when the height of a row is modified.
   */
  sheet_signals[NEW_ROW_HEIGHT] =
    g_signal_new ("new-row-height",
		    G_TYPE_FROM_CLASS(object_class),
		    G_SIGNAL_RUN_LAST,
		    G_STRUCT_OFFSET (GtkSheetClass, new_row_height),
            NULL, NULL,
            gtksheet_VOID__INT_INT,
	        G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

  /**
   * GtkSheet::set-scroll-adjustments:
   * @sheet: the sheet widget that emitted the signal
   * @hadjustment: horizontal #GtkAdjustment.
   * @vadjustment: vertical #GtkAdkjustment.
   *
   * Emited when scroll adjustments are set.
   */
  widget_class->set_scroll_adjustments_signal =
    g_signal_new ("set-scroll-adjustments",
                    G_TYPE_FROM_CLASS(object_class),
                    G_SIGNAL_RUN_LAST,
                    G_STRUCT_OFFSET (GtkSheetClass, set_scroll_adjustments),
                    NULL, NULL,
                    gtksheet_VOID__OBJECT_OBJECT,
                    G_TYPE_NONE, 2, GTK_TYPE_ADJUSTMENT, GTK_TYPE_ADJUSTMENT);

  pspec = g_param_spec_boolean("autoresize",
                               "Autoresize",
                               "Set autoresize",
                               TRUE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_AUTORESIZE,
                                   pspec);

  pspec = g_param_spec_boolean("autoscroll",
                               "Autoscroll",
                               "Set autoscroll",
                               TRUE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_AUTOSCROLL,
                                   pspec);

  pspec = g_param_spec_boolean("clip-text",
                               "Clip text",
                               "Set clip text",
                               FALSE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_CLIP_TEXT,
                                   pspec);

  pspec = g_param_spec_boolean("grid-visible",
                               "Grid visible",
                               "Set whether the grid lines are visible",
                               TRUE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_GRID_VISIBLE,
                                   pspec);

  pspec = g_param_spec_boolean("locked",
                               "Locked",
                               "Set sheet locked",
                               FALSE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_LOCKED,
                                   pspec);

  pspec = g_param_spec_boolean("columns-resizable",
                               "Columns resizable",
                               "Set whether columns can be resized",
                               TRUE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_COLUMNS_RESIZABLE,
                                   pspec);

  pspec = g_param_spec_boolean("column-titles-visible",
                               "Column titles visible",
                               "Set whether column titles are visible",
                               TRUE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_COLUMN_TITLES_VISIBLE,
                                   pspec);

  pspec = g_param_spec_boolean("rows-resizable",
                               "Rows resizable",
                               "Set whether rows can be resized",
                               TRUE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_ROWS_RESIZABLE,
                                   pspec);

  pspec = g_param_spec_boolean("row-titles-visible",
                               "Row titles visible",
                               "Set whether row titles are visible",
                               TRUE,
                               G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_ROW_TITLES_VISIBLE,
                                   pspec);

  pspec = g_param_spec_enum("state",
                            "State",
                            "Sheet state",
                            GTK_TYPE_SHEET_STATE,
                            GTK_SHEET_NORMAL,
                            G_PARAM_READABLE);
  g_object_class_install_property (gobject_class,
                                   PROP_STATE,
                                   pspec);

  pspec = g_param_spec_string("title",
                              "Sheet title",
                              "Set sheet title",
                              DEFAULT_SHEET_TITLE,                              
                              G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_TITLE,
                                   pspec);

  pspec = g_param_spec_boxed("active-cell",
                             "Active cell (row, column)",
                             "An array (row, column) indicating the active cell",
                             G_TYPE_VALUE_ARRAY,
                             G_PARAM_READABLE);
  g_object_class_install_property (gobject_class,
                                   PROP_ACTIVE_CELL,
                                   pspec);

  pspec = g_param_spec_boxed("selected-range",
                             "Selected cells range",
                             "A SheetRange with the currently selected cells",
                             GTK_TYPE_SHEET_RANGE,
                             G_PARAM_READABLE);
  g_object_class_install_property (gobject_class,
                                   PROP_SELECTED_RANGE,
                                   pspec);

  pspec = g_param_spec_uint("n-rows",
                             "Number of rows",
                             "Number of rows in the sheet.",
                             MINROWS, G_MAXUINT, MINROWS,
                             G_PARAM_READABLE);
  g_object_class_install_property (gobject_class,
                                   PROP_N_ROWS,
                                   pspec);

  pspec = g_param_spec_uint("n-columns",
                             "Number of columns",
                             "Number of columns in the sheet.",
                             MINCOLS, G_MAXUINT, MINCOLS,
                             G_PARAM_READABLE);
  g_object_class_install_property (gobject_class,
                                   PROP_N_COLUMNS,
                                   pspec);

  pspec = g_param_spec_enum("selection-mode",
                            "Selection mode",
                            "Selection mode for the sheet",
                            GTK_TYPE_SELECTION_MODE,
                            GTK_SELECTION_BROWSE,
                            G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_SELECTION_MODE,
                                   pspec);

  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_X,
					      g_param_spec_int ("x", 
								 "Horizontal pixel position", 
								 "Horizontal position of the child in the sheet, in pixels",
								 G_MININT, G_MAXINT, 0,
								 G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_Y,
					      g_param_spec_int ("y", 
								 "Vertical pixel position", 
								 "Vertical position of the child in the sheet, in pixels",
								 G_MININT, G_MAXINT, 0,
								 G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_ROW,
					      g_param_spec_int ("row", 
								 "Row which the child is attached to", 
								 "The sheet row which the child is attached to",
								 0, G_MAXINT, 0,
								 G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_COLUMN,
					      g_param_spec_int ("column", 
								 "Column which the child is attached to", 
								 "The sheet column which the child is attached to",
								 0, G_MAXINT, 0,
								 G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_ATTACHED_TO_CELL,
					      g_param_spec_boolean ("attached-to-cell", 
								 "Whether child is attached", 
								 "Whether the child's position is kept attached to the cell indicated by 'row' and 'column'",
								 TRUE,
								 G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_FLOATING,
					      g_param_spec_boolean ("floating", 
								 "Resize child with the cell", 
								 "Whether the child size is kept within the area of the cell indicated by 'row' and 'column'",
								 FALSE,
								 G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_X_OPTIONS,
					      g_param_spec_flags ("x-options", 
								  "Horizontal options", 
								  "Options specifying the horizontal behaviour of the child",
								  GTK_TYPE_ATTACH_OPTIONS, GTK_EXPAND | GTK_FILL,
								  G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_Y_OPTIONS,
					      g_param_spec_flags ("y-options", 
								  "Vertical options", 
								  "Options specifying the vertical behaviour of the child",
								  GTK_TYPE_ATTACH_OPTIONS, GTK_EXPAND | GTK_FILL,
								  G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_X_PADDING,
					      g_param_spec_uint ("x-padding", 
								 "Horizontal padding", 
								 "Extra space to put between the child and the left and right cell borders, in pixels",
								 0, 65535, 0,
								 G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
					      CHILD_PROP_Y_PADDING,
					      g_param_spec_uint ("y-padding", 
								 "Vertical padding", 
								 "Extra space to put between the child and the upper and lower cell borders, in pixels",
								 0, 65535, 0,
								 G_PARAM_READWRITE));
}

static void 
gtk_sheet_init (GtkSheet *sheet)
{
  sheet->children = NULL;

  sheet->flags = 0;
  sheet->selection_mode = GTK_SELECTION_BROWSE;
  sheet->freeze_count = 0;
  sheet->state = GTK_SHEET_NORMAL;

  GTK_WIDGET_UNSET_FLAGS (sheet, GTK_NO_WINDOW);
  GTK_WIDGET_SET_FLAGS (sheet, GTK_CAN_FOCUS);

  sheet->maxrow = 0;
  sheet->maxcol = 0;

  sheet->view.row0 = 0;
  sheet->view.col0 = 0;
  sheet->view.rowi = 0;
  sheet->view.coli = 0;

  sheet->maxallocrow = 0;
  sheet->maxalloccol = 0;

  sheet->column_title_window=NULL;
  sheet->column_title_area.x=0;
  sheet->column_title_area.y=0;
  sheet->column_title_area.width=0;
  sheet->column_title_area.height=DEFAULT_ROW_HEIGHT(GTK_WIDGET(sheet));
 
  sheet->row_title_window=NULL;
  sheet->row_title_area.x=0;
  sheet->row_title_area.y=0;
  sheet->row_title_area.width=DEFAULT_COLUMN_WIDTH;
  sheet->row_title_area.height=0;

  sheet->active_cell.row=0;
  sheet->active_cell.col=0;
  sheet->selection_cell.row=0;
  sheet->selection_cell.col=0;

  sheet->sheet_entry=NULL;
  sheet->pixmap=NULL;

  sheet->range.row0=0;
  sheet->range.rowi=0;
  sheet->range.col0=0;
  sheet->range.coli=0;

  sheet->state=GTK_SHEET_NORMAL;

  sheet->sheet_window = NULL;
  sheet->sheet_window_width = 0;
  sheet->sheet_window_height = 0;
  sheet->sheet_entry = NULL;
  sheet->button = NULL;

  sheet->hoffset = 0;
  sheet->voffset = 0;

  sheet->hadjustment = NULL;
  sheet->vadjustment = NULL;

  sheet->cursor_drag = gdk_cursor_new(GDK_PLUS);
  sheet->xor_gc = NULL;
  sheet->fg_gc = NULL;
  sheet->bg_gc = NULL;
  sheet->x_drag = 0;
  sheet->y_drag = 0;

  GdkColormap *colormap = gdk_colormap_get_system();
  gdk_color_parse("white", &sheet->bg_color);
  gdk_colormap_alloc_color(colormap, &sheet->bg_color, TRUE, TRUE);
  gdk_color_parse("gray", &sheet->grid_color);
  gdk_colormap_alloc_color(colormap, &sheet->grid_color, TRUE, TRUE);

  sheet->show_grid = TRUE;

  sheet->name = g_strdup(DEFAULT_SHEET_TITLE);
}

/**
 * gtk_sheet_new:
 * @rows: initial number of rows
 * @columns: initial number of columns
 * @title: sheet title
 *
 * Creates a new sheet widget with the given number of rows and columns.
 *
 * Returns: the new sheet #GtkSheet
 */
GtkWidget *
gtk_sheet_new (guint rows, guint columns, const gchar *title)
{
  GtkWidget *widget;

  /* sanity check */
  g_return_val_if_fail (columns >= MINCOLS, NULL);
  g_return_val_if_fail (rows >= MINROWS, NULL);

  /*gtk_sheet_get_type();*/
  widget = g_object_new (gtk_sheet_get_type (), NULL);

  gtk_sheet_construct(GTK_SHEET(widget), rows, columns, title);

  return widget;
}

/**
 * gtk_sheet_construct:
 * @sheet: a #GtkSheet
 * @rows: number of rows
 * @columns: number of columns
 * @title: sheet title
 *
 * Initializes an existent #GtkSheet with the given number of rows and columns.
 */
void
gtk_sheet_construct (GtkSheet *sheet, guint rows, guint columns, const gchar *title)
{
  sheet->row=(GtkSheetRow *)g_malloc(sizeof(GtkSheetRow));
  sheet->column=(GtkSheetColumn *)g_malloc(sizeof(GtkSheetColumn));
  sheet->data=(GtkSheetCell ***)g_malloc(sizeof(GtkSheetCell **));

  sheet->data[0] = (GtkSheetCell **)g_malloc(sizeof(GtkSheetCell *)+sizeof(gdouble));
  sheet->data[0][0] = NULL;

  sheet->columns_resizable = TRUE;
  sheet->rows_resizable = TRUE;
  sheet->row_titles_visible = TRUE;
  sheet->column_titles_visible = TRUE;
  sheet->autoscroll = TRUE;
  sheet->justify_entry = TRUE;
  sheet->locked = FALSE;

  /* set number of rows and columns */
  GrowSheet(sheet, MINROWS, MINCOLS);

  /* Init row an column zero */
  AddRow(sheet,-1);
  AddColumn(sheet,-1);

  /* Add rows and columns */
  AddRow(sheet,rows-1);
  AddColumn(sheet,columns-1);

  /* create sheet entry */
  sheet->entry_type = 0;
  create_sheet_entry (sheet);

  /* create global selection button */
  create_global_button(sheet);

  if(title)
     sheet->name = g_strdup(title);

}

/**
 * gtk_sheet_new_browser:
 * @rows: initial number of rows
 * @columns: initial number of columns
 * @title: sheet title
 *
 * Creates a new browser sheet. Its cells cannot be edited(read-only).
 *
 * Returns: the new read-only #GtkSheet
 */
GtkWidget *
gtk_sheet_new_browser(guint rows, guint columns, const gchar *title)
{
  GtkWidget *widget;
  
  /*gtk_sheet_get_type();*/
  widget = g_object_new (gtk_sheet_get_type (), NULL);

  gtk_sheet_construct_browser(GTK_SHEET(widget), rows, columns, title);
 
  return widget;
}

/**
 * gtk_sheet_construct_browser:
 * @sheet: a #GtkSheet
 * @rows: number of rows
 * @columns: number of columns
 * @title: sheet title
 *
 * Initializes an existent read-only #GtkSheet with the given number of rows and columns.
 */
void
gtk_sheet_construct_browser(GtkSheet *sheet, guint rows, guint columns, 
                           const gchar *title)
{
  gtk_sheet_construct(sheet, rows, columns, title);

  gtk_sheet_set_locked(sheet, TRUE);
  sheet->autoresize = TRUE;
}

/**
 * gtk_sheet_new_with_custom_entry:
 * @rows: initial number of rows
 * @columns: initial number of columns
 * @title: sheet title
 * @entry_type: a #GtkType
 *
 * Creates a new sheet widget with the given number of rows and columns and a custome entry type.
 *
 * Returns: the new sheet #GtkSheet
 */
GtkWidget *
gtk_sheet_new_with_custom_entry (guint rows, guint columns, const gchar *title,
                                 GtkType entry_type)
{
  GtkWidget *widget;
  
  /*gtk_sheet_get_type();*/
  widget = g_object_new (gtk_sheet_get_type (), NULL);

  gtk_sheet_construct_with_custom_entry(GTK_SHEET(widget), 
                                       rows, columns, title, entry_type);
 
  return widget;
}

/**
 * gtk_sheet_construct_with_custom_entry:
 * @sheet: a #GtkSheet
 * @rows: number of rows
 * @columns: number of columns
 * @title: sheet title
 * @entry_type: a #GtkType
 *
 * Initializes an existent read-only #GtkSheet with the given number of rows and columns and a custom entry.
 */
void
gtk_sheet_construct_with_custom_entry (GtkSheet *sheet, 
                                      guint rows, guint columns, 
                                      const gchar *title,
                                      GtkType entry_type)
{
  gtk_sheet_construct(sheet, rows, columns, title);

  sheet->entry_type = entry_type;
  create_sheet_entry(sheet);
}

/**
 * gtk_sheet_change_entry:
 * @sheet: a #GtkSheet
 * @entry_type: a #GtkType
 *
 * Changes the current entry of the cell in #GtkSheet.
 */
void
gtk_sheet_change_entry(GtkSheet *sheet, GtkType entry_type)
{
  gint state;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  state = sheet->state;

  if(sheet->state == GTK_SHEET_NORMAL)
      gtk_sheet_hide_active_cell(sheet);

  sheet->entry_type = entry_type;

  create_sheet_entry(sheet);

  if(state == GTK_SHEET_NORMAL)
    {
      gtk_sheet_show_active_cell(sheet); 
      g_signal_connect(G_OBJECT(gtk_sheet_get_entry(sheet)),
                       "changed",
                       (GCallback)gtk_sheet_entry_changed,
                       G_OBJECT(GTK_WIDGET(sheet)));
    }
 
}

/**
 * gtk_sheet_show_grid:
 * @sheet: a #GtkSheet
 * @show : TRUE(grid visible) or FALSE(grid invisible)
 *
 * Sets the visibility of grid in #GtkSheet.
 */
void
gtk_sheet_show_grid(GtkSheet *sheet, gboolean show)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(show == sheet->show_grid) return;
 
  sheet->show_grid = show;
  g_object_notify(G_OBJECT(sheet), "grid-visible");

  if(!GTK_SHEET_IS_FROZEN(sheet)) 
    gtk_sheet_range_draw(sheet, NULL);
}

/**
 * gtk_sheet_grid_visible:
 * @sheet: a #GtkSheet
 *
 * Gets the visibility of grid in #GtkSheet.
 *
 * Return value: TRUE(grid visible) or FALSE(grid invisible)
 */
gboolean
gtk_sheet_grid_visible(GtkSheet *sheet)
{
  g_return_val_if_fail (sheet, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);
  if (sheet)
     return sheet->show_grid;
  return FALSE;
}

/**
 * gtk_sheet_set_background:
 * @sheet: a #GtkSheet
 * @color: a #GdkColor structure
 *
 * Set the background color of all #GtkSheet.
 */
void
gtk_sheet_set_background(GtkSheet *sheet, GdkColor *color)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!color)
    gdk_color_parse("white", &sheet->bg_color);
  else
    sheet->bg_color = *color;

  gdk_colormap_alloc_color(gdk_colormap_get_system(), &sheet->bg_color, TRUE, TRUE);

  if(!GTK_SHEET_IS_FROZEN(sheet)) 
    gtk_sheet_range_draw(sheet, NULL);
}

/**
 * gtk_sheet_set_grid:
 * @sheet: a #GtkSheet
 * @color: a #GdkColor structure
 *
 * Set the grid color.
 */
void
gtk_sheet_set_grid(GtkSheet *sheet, GdkColor *color)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!color)
    gdk_color_parse("black", &sheet->grid_color);
  else
    sheet->grid_color = *color;

  gdk_colormap_alloc_color(gdk_colormap_get_system(), &sheet->grid_color, TRUE, TRUE);

  if(!GTK_SHEET_IS_FROZEN(sheet)) 
    gtk_sheet_range_draw(sheet, NULL);
}

/**
 * gtk_sheet_get_columns_count:
 * @sheet: a #GtkSheet
 *
 * Get the number of the columns of the #GtkSheet.
 *
 * Return value: number of columns.
 */
guint
gtk_sheet_get_columns_count(GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, 0);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), 0);

  return sheet->maxcol + 1;
}

/**
 * gtk_sheet_get_rows_count:
 * @sheet: a #GtkSheet
 *
 * Get the number of the rows of the #GtkSheet.
 *
 * Return value: number of rows.
 */
guint
gtk_sheet_get_rows_count(GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, 0);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), 0);

  return sheet->maxrow + 1;
}

/**
 * gtk_sheet_get_state:
 * @sheet: a #GtkSheet
 *
 * Get the #GtkSheet state.
 *
 * Return value: GTK_SHEET_NORMAL,GTK_SHEET_ROW_SELECTED,GTK_SHEET_COLUMN_SELECTED,GTK_SHEET_RANGE_SELECTED
 */
gint
gtk_sheet_get_state(GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, 0);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), 0);

  return (sheet->state);
}

/**
 * gtk_sheet_set_selection_mode:
 * @sheet: a #GtkSheet
 * @mode: GTK_SELECTION_SINGLE or GTK_SELECTION_BROWSE
 *
 * Sets the selection mode of the cells in a #GtkSheet. 
 */
void
gtk_sheet_set_selection_mode(GtkSheet *sheet, GtkSelectionMode mode)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(GTK_WIDGET_REALIZED(sheet))
   gtk_sheet_real_unselect_range(sheet, NULL);

  sheet->selection_mode = mode;
  g_object_notify(G_OBJECT(sheet), "selection-mode");
}

/**
 * gtk_sheet_set_autoresize:
 * @sheet: a #GtkSheet
 * @autoresize: TRUE or FALSE
 *
 * The cells will be autoresized as you type text if autoresize=TRUE.
 * If you want the cells to be autoresized when you pack widgets look at gtk_sheet_attach_*()
 */
void
gtk_sheet_set_autoresize (GtkSheet *sheet, gboolean autoresize)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (autoresize != sheet->autoresize) {
    sheet->autoresize = autoresize;
    g_object_notify(G_OBJECT(sheet), "autoresize");
  }
}

/**
 * gtk_sheet_autoresize:
 * @sheet: a #GtkSheet
 *
 * Gets the autoresize mode of #GtkSheet.
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_autoresize (GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

  return sheet->autoresize;
}

static void
gtk_sheet_autoresize_column (GtkSheet *sheet, gint column)
{
  gint text_width = 0;
  gint row;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));
  if (column > sheet->maxcol || column < 0) return;

  for (row = 0; row < sheet->maxrow; row++){
    GtkSheetCell      **cell = &sheet->data[row][column];
    if (*cell && (*cell)->text && strlen((*cell)->text) > 0){
      GtkSheetCellAttr attributes;

      gtk_sheet_get_attributes(sheet, row, column, &attributes);
      if(attributes.is_visible){
        PangoFontDescription *font_desc = (attributes.font_desc == NULL) ? 
                GTK_WIDGET(sheet)->style->font_desc: attributes.font_desc;

        gint width = STRING_WIDTH(GTK_WIDGET(sheet),
                                  font_desc,
                                  (*cell)->text)
                   + 2*CELLOFFSET + attributes.border.width;
        text_width = MAX (text_width, width);
      }
    }
  }

  if(text_width > (gint)sheet->column[column].width){
      gtk_sheet_set_column_width(sheet, column, text_width);
      GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_REDRAW_PENDING);
  }
}

/**
 * gtk_sheet_set_autoscroll:
 * @sheet: a #GtkSheet
 * @autoscroll: TRUE or FALSE
 *
 * The table will be automatically scrolled when you reach the last row/column in #GtkSheet  if autoscroll=TRUE.
 */
void
gtk_sheet_set_autoscroll (GtkSheet *sheet, gboolean autoscroll)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));
  if (sheet->autoscroll != autoscroll) {
    sheet->autoscroll = autoscroll;
    g_object_notify(G_OBJECT(sheet), "autoscroll");
  }
}

/**
 * gtk_sheet_autoscroll:
 * @sheet: a #GtkSheet
 *
 * Get the autoscroll mode of #GtkSheet.
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_autoscroll (GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

  return sheet->autoscroll;
}

/**
 * gtk_sheet_set_clip_text:
 * @sheet: a #GtkSheet
 * @clip_text: TRUE or FALSE
 *
 * Clip text in cell.
 */
void
gtk_sheet_set_clip_text  (GtkSheet *sheet, gboolean clip_text)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));
  if (clip_text != sheet->clip_text) {
    sheet->clip_text = clip_text;
    g_object_notify(G_OBJECT(sheet), "clip-text");
  }
}

/**
 * gtk_sheet_clip_text:
 * @sheet: a #GtkSheet
 *
 * Get clip text mode in #GtkSheet.
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_clip_text (GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

  return sheet->clip_text;
}

/**
 * gtk_sheet_set_justify_entry:
 * @sheet: a #GtkSheet
 * @justify: TRUE or FALSE
 *
 * Justify cell text in #GtkSheet.
 */
void
gtk_sheet_set_justify_entry (GtkSheet *sheet, gboolean justify)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (justify != sheet->justify_entry) {
    sheet->justify_entry = justify;
    g_object_notify(G_OBJECT(sheet), "justify-entry");
  }
}

/**
 * gtk_sheet_justify_entry:
 * @sheet: a #GtkSheet
 *
 * Get the cell text justification status in #GtkSheet.
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_justify_entry (GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

  return sheet->justify_entry;
}

/**
 * gtk_sheet_set_locked:
 * @sheet: a #GtkSheet
 * @locked: TRUE or FALSE
 *
 * Lock the #GtkSheet .
 */
void
gtk_sheet_set_locked (GtkSheet *sheet, gboolean locked)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (locked != sheet->locked) {
    sheet->locked = locked;
    g_object_notify(G_OBJECT(sheet), "locked");
  }
}

/**
 * gtk_sheet_locked:
 * @sheet: a #GtkSheet
 *
 * Get the lock status of #GtkSheet.
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_locked (GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

  return sheet->locked;
}

/* This routine has problems with gtk+-1.2 related with the
 * label/button drawing - I think it's a bug in gtk+-1.2 */

/**
 * gtk_sheet_set_title:
 * @sheet: a #GtkSheet
 * @title: #GtkSheet title
 *
 * Set  #GtkSheet title.
 */
void
gtk_sheet_set_title(GtkSheet *sheet, const gchar *title)
{
/*  GtkWidget *old_widget;
*/  GtkWidget *label;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (title != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (sheet->name)
    g_free (sheet->name);

  sheet->name = g_strdup (title);

  g_object_notify(G_OBJECT(sheet), "title");

  if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)) || !title) return;

  if(GTK_BIN(sheet->button)->child)
           label = GTK_BIN(sheet->button)->child;
/*
  gtk_label_set_text(GTK_LABEL(label), title);
*/
  size_allocate_global_button(sheet);

  /* remove and destroy the old widget */
/*
  old_widget = GTK_BIN (sheet->button)->child;
  if (old_widget)
    {
      gtk_container_remove (GTK_CONTAINER (sheet->button), old_widget);
    }

  label = gtk_label_new (title);
  gtk_misc_set_alignment(GTK_MISC(label), 0.5 , 0.5 );

  gtk_container_add (GTK_CONTAINER (sheet->button), label);
  gtk_widget_show (label);

  size_allocate_global_button(sheet);

  gtk_signal_emit(GTK_OBJECT(sheet),sheet_signals[CHANGED], -1, -1);

  if(old_widget)
      gtk_widget_destroy (old_widget);
*/
}

/**
 * gtk_sheet_freeze:
 * @sheet: a #GtkSheet
 *
 * Freeze all visual updates of the #GtkSheet.
 * The updates will occure in a more efficient way than if you made them on a unfrozen #GtkSheet .
 */
void
gtk_sheet_freeze (GtkSheet *sheet)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  sheet->freeze_count++;
  GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IS_FROZEN);
}

/**
 * gtk_sheet_thaw:
 * @sheet: a #GtkSheet
 *
 * Thaw the sheet after you have made a number of changes on a frozen sheet.
 * The updates will occure in a more efficient way than if you made them on a unfrozen sheet .
 */
void
gtk_sheet_thaw(GtkSheet *sheet)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(sheet->freeze_count == 0) return;

  sheet->freeze_count--;
  if(sheet->freeze_count > 0) return;

  adjust_scrollbars(sheet);

  GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IS_FROZEN);

  sheet->old_vadjustment = -1.;
  sheet->old_hadjustment = -1.;

  if(sheet->hadjustment)
      g_signal_emit_by_name (GTK_OBJECT (sheet->hadjustment), "value_changed");
  if(sheet->vadjustment)
      g_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), "value_changed");

  if(sheet->state == GTK_STATE_NORMAL)
     if(sheet->sheet_entry && GTK_WIDGET_MAPPED(sheet->sheet_entry)){
        gtk_sheet_activate_cell(sheet, sheet->active_cell.row, sheet->active_cell.col);
/*
        gtk_signal_connect(GTK_OBJECT(gtk_sheet_get_entry(sheet)),
           	           "changed",
                           (GtkSignalFunc)gtk_sheet_entry_changed,
                           GTK_OBJECT(GTK_WIDGET(sheet)));
        gtk_sheet_show_active_cell(sheet);
*/
     }

}

/**
 * gtk_sheet_set_row_titles_width:
 * @sheet: a #GtkSheet
 * @width: row titles width. 
 *
 * Resize row titles .
 */
void
gtk_sheet_set_row_titles_width(GtkSheet *sheet, guint width)
{
 if(width < COLUMN_MIN_WIDTH) return;

 sheet->row_title_area.width = width;
 sheet->view.col0=COLUMN_FROM_XPIXEL(sheet, sheet->row_title_area.width+1);
 sheet->view.coli=COLUMN_FROM_XPIXEL(sheet, sheet->sheet_window_width);
 gtk_sheet_recalc_top_ypixels(sheet, 0);
 gtk_sheet_recalc_left_xpixels(sheet, 0);
 adjust_scrollbars(sheet);

 sheet->old_hadjustment = -1.;
 if(sheet->hadjustment)
     g_signal_emit_by_name (GTK_OBJECT (sheet->hadjustment), "value_changed");
 size_allocate_global_button(sheet);
}

/**
 * gtk_sheet_set_column_titles_height:
 * @sheet: a #GtkSheet
 * @height: column title height. 
 *
 * Resize column titles height .
 */
void
gtk_sheet_set_column_titles_height(GtkSheet *sheet, guint height)
{
 if(height < DEFAULT_ROW_HEIGHT(GTK_WIDGET(sheet))) return;

 sheet->column_title_area.height = height;
 sheet->view.row0=ROW_FROM_YPIXEL(sheet, sheet->column_title_area.height+1);
 sheet->view.rowi=ROW_FROM_YPIXEL(sheet, sheet->sheet_window_height-1);
 gtk_sheet_recalc_top_ypixels(sheet, 0);
 gtk_sheet_recalc_left_xpixels(sheet, 0);
 adjust_scrollbars(sheet);

 sheet->old_vadjustment = -1.;
 if(sheet->vadjustment)
     g_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), "value_changed");
 size_allocate_global_button(sheet);
}

/**
 * gtk_sheet_show_column_titles:
 * @sheet: a #GtkSheet
 *
 * Show column titles .
 */
void
gtk_sheet_show_column_titles(GtkSheet *sheet)
{
    gint col;

    if(sheet->column_titles_visible) return;

    sheet->column_titles_visible = TRUE;
    g_object_notify(G_OBJECT(sheet), "column-titles-visible");

    gtk_sheet_recalc_top_ypixels(sheet, 0);
    gtk_sheet_recalc_left_xpixels(sheet, 0);
    if (GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) {
        gdk_window_show(sheet->column_title_window);
        gdk_window_move_resize (sheet->column_title_window,
                              sheet->column_title_area.x,
                              sheet->column_title_area.y,
                              sheet->column_title_area.width,
                              sheet->column_title_area.height);

        for (col = MIN_VISIBLE_COLUMN(sheet); col <= MAX_VISIBLE_COLUMN(sheet); col++) {
            GtkSheetChild *child;
            child = sheet->column[col].button.child;
            if (child) {
                gtk_sheet_child_show(child);
            }
        }
        adjust_scrollbars(sheet);
    } 

    sheet->old_vadjustment = -1.;
    if(sheet->vadjustment)
        g_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), "value_changed");
    size_allocate_global_button(sheet);
}

/**
 * gtk_sheet_show_row_titles:
 * @sheet: a #GtkSheet
 *
 * Show row titles .
 */
void
gtk_sheet_show_row_titles(GtkSheet *sheet)
{
    gint row;

    if(sheet->row_titles_visible) return;

    sheet->row_titles_visible = TRUE;
    g_object_notify(G_OBJECT(sheet), "row-titles-visible");

    gtk_sheet_recalc_top_ypixels(sheet, 0);
    gtk_sheet_recalc_left_xpixels(sheet, 0);
    if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) {
        gdk_window_show(sheet->row_title_window);
        gdk_window_move_resize (sheet->row_title_window,
                              sheet->row_title_area.x,
                              sheet->row_title_area.y,
                              sheet->row_title_area.width,
                              sheet->row_title_area.height);

        for(row = MIN_VISIBLE_ROW(sheet); row <= MAX_VISIBLE_ROW(sheet); row++) {
            GtkSheetChild *child;
            child = sheet->row[row].button.child;
            if(child) {
                gtk_sheet_child_show(child);
            }
        }
        adjust_scrollbars(sheet);
    }

    sheet->old_hadjustment = -1.;
    if(sheet->hadjustment)
        g_signal_emit_by_name (GTK_OBJECT(sheet->hadjustment), "value_changed");
    size_allocate_global_button(sheet);
}

/**
 * gtk_sheet_hide_column_titles:
 * @sheet: a #GtkSheet
 *
 * Hide column titles .
 */
void
gtk_sheet_hide_column_titles(GtkSheet *sheet)
{
 gint col;

 if(!sheet->column_titles_visible) return;

 sheet->column_titles_visible = FALSE;
 g_object_notify(G_OBJECT(sheet), "column-titles-visible");
 gtk_sheet_recalc_top_ypixels(sheet, 0);
 gtk_sheet_recalc_left_xpixels(sheet, 0);
 if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))){
  if(sheet->column_title_window) 
    gdk_window_hide(sheet->column_title_window);
  if(GTK_WIDGET_VISIBLE(sheet->button)) 
    gtk_widget_hide(sheet->button);

  for(col = MIN_VISIBLE_COLUMN(sheet); col <= MAX_VISIBLE_COLUMN(sheet); col++){
    GtkSheetChild *child;
    child = sheet->column[col].button.child;
    if(child){
        gtk_sheet_child_hide(child);
    }
  }
  adjust_scrollbars(sheet);
 }
 
 sheet->old_vadjustment = -1.;
 if(sheet->vadjustment)
     g_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), "value_changed");
}

/**
 * gtk_sheet_hide_row_titles:
 * @sheet: a #GtkSheet
 *
 * Hide row titles .
 */
void
gtk_sheet_hide_row_titles(GtkSheet *sheet)
{
    gint row;

    if(!sheet->row_titles_visible) return;

    sheet->row_titles_visible = FALSE;
    g_object_notify(G_OBJECT(sheet), "row-titles-visible");

    gtk_sheet_recalc_top_ypixels(sheet, 0);
    gtk_sheet_recalc_left_xpixels(sheet, 0);
    if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))){
        if (sheet->row_title_window) 
            gdk_window_hide(sheet->row_title_window);
        if (GTK_WIDGET_VISIBLE(sheet->button)) 
            gtk_widget_hide(sheet->button);
        for (row = MIN_VISIBLE_ROW(sheet); row <= MAX_VISIBLE_ROW(sheet); row++) {
            GtkSheetChild *child;
            child = sheet->row[row].button.child;
            if(child){
                gtk_sheet_child_hide(child);
            }
        }
        adjust_scrollbars(sheet);
    }

    sheet->old_hadjustment = -1.;
    if(sheet->hadjustment)
        g_signal_emit_by_name (GTK_OBJECT(sheet->hadjustment), "value_changed");
}

/**
 * gtk_sheet_column_titles_visible:
 * @sheet: a #GtkSheet
 *
 * Get the visibility of sheet column titles .
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_column_titles_visible(GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);
  return sheet->column_titles_visible;
}

/**
 * gtk_sheet_row_titles_visible:
 * @sheet: a #GtkSheet
 *
 * Get the visibility of row column titles .
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_row_titles_visible(GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);
  return sheet->row_titles_visible;
}

/**
 * gtk_sheet_set_column_title:
 * @sheet: a #GtkSheet
 * @column: column number
 * @title: column title
 *
 * Set column title.
 */
void
gtk_sheet_set_column_title (GtkSheet * sheet,
			    gint column,
			    const gchar * title)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (sheet->column[column].name)
    g_free (sheet->column[column].name);

  sheet->column[column].name = g_strdup(title);
}

/**
 * gtk_sheet_set_row_title:
 * @sheet: a #GtkSheet
 * @row: row number
 * @title: row title
 *
 * Set row title.
 */
void
gtk_sheet_set_row_title (GtkSheet * sheet,
			 gint row,
			 const gchar * title)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (sheet->row[row].name)
    g_free (sheet->row[row].name);

  sheet->row[row].name = g_strdup (title);
}

/**
 * gtk_sheet_get_row_title:
 * @sheet: a #GtkSheet
 * @row: row number
 *
 * Get row title.
 * Return value: row title
 */
const gchar *
gtk_sheet_get_row_title (GtkSheet * sheet,
			 gint row)
{
  g_return_val_if_fail (sheet != NULL, NULL);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);

  return(sheet->row[row].name);
}

/**
 * gtk_sheet_get_column_title:
 * @sheet: a #GtkSheet
 * @column: column number
 *
 * Get column title.
 * Return value: column title
 */
const gchar *
gtk_sheet_get_column_title (GtkSheet * sheet,
			    gint column)
{
  g_return_val_if_fail (sheet != NULL, NULL);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);

  return(sheet->column[column].name);
}

/**
 * gtk_sheet_row_button_add_label:
 * @sheet: a #GtkSheet
 * @row: row number
 * @label: text label
 *
 * Set button label.It is used to set a row title.
 */
void
gtk_sheet_row_button_add_label(GtkSheet *sheet, gint row, const gchar *label)
{
  GtkSheetButton *button;
  GtkRequisition req;
  gboolean aux;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(row < 0 || row > sheet->maxrow) return;

  button = &sheet->row[row].button;
  if (button->label) g_free (button->label);
  button->label = g_strdup (label);

  aux = gtk_sheet_autoresize(sheet);
  gtk_sheet_set_autoresize(sheet, TRUE);
  gtk_sheet_button_size_request(sheet, button, &req);
  gtk_sheet_set_autoresize(sheet, aux);

  if(req.height > sheet->row[row].height)
     gtk_sheet_set_row_height(sheet, row, req.height);

  if(req.width > sheet->row_title_area.width){
     gtk_sheet_set_row_titles_width(sheet, req.width);
  }

  if(!GTK_SHEET_IS_FROZEN(sheet)){
    gtk_sheet_button_draw(sheet, row, -1);
    g_signal_emit(sheet, sheet_signals[CHANGED], 0, row, -1);
  }
}  

/**
 * gtk_sheet_row_button_get_label:
 * @sheet: a #GtkSheet
 * @row: row number
 *
 * Get button label.
 * Return value: In case of succes , a pointer to label text.Otherwise NULL>
 */
const gchar *
gtk_sheet_row_button_get_label(GtkSheet *sheet, gint row)
{
  g_return_val_if_fail (sheet != NULL, NULL);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);

  if(row < 0 || row > sheet->maxrow) return NULL;

  return (sheet->row[row].button.label);
}

/**
 * gtk_sheet_row_label_set_visibility:
 * @sheet: a #GtkSheet
 * @row: row number
 * @visible: TRUE or FALSE
 *
 * Set row label visibility.
 */
void
gtk_sheet_row_label_set_visibility(GtkSheet *sheet, gint row, gboolean visible)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(row < 0 || row > sheet->maxrow) return;

  sheet->row[row].button.label_visible = visible;

  if(!GTK_SHEET_IS_FROZEN(sheet)){  
    gtk_sheet_button_draw(sheet, row, -1);
    g_signal_emit(sheet, sheet_signals[CHANGED], 0, row, -1);
  }
}

/**
 * gtk_sheet_rows_labels_set_visibility:
 * @sheet: a #GtkSheet
 * @visible: TRUE or FALSE
 *
 * Set all rows label visibility.
 */
void
gtk_sheet_rows_labels_set_visibility(GtkSheet *sheet, gboolean visible)
{
  gint i;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  for(i = 0; i <= sheet->maxrow; i++)
    gtk_sheet_row_label_set_visibility(sheet, i, visible);
}

/**
 * gtk_sheet_column_button_add_label:
 * @sheet: a #GtkSheet
 * @column: column number
 * @label: text label
 *
 * Set button label.It is used to set a column title.
 */
void
gtk_sheet_column_button_add_label(GtkSheet *sheet, gint column, const gchar *label)
{
  GtkSheetButton *button;
  GtkRequisition req;
  gboolean aux;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(column < 0 || column >sheet->maxcol) return;

  button = &sheet->column[column].button;
  if (button->label) g_free (button->label);
  button->label = g_strdup (label);

  aux = gtk_sheet_autoresize(sheet);
  gtk_sheet_set_autoresize(sheet, TRUE);
  gtk_sheet_button_size_request(sheet, button, &req);
  gtk_sheet_set_autoresize(sheet, aux);

  if(req.width > sheet->column[column].width)
     gtk_sheet_set_column_width(sheet, column, req.width);

  if(req.height > sheet->column_title_area.height)
     gtk_sheet_set_column_titles_height(sheet, req.height);

  if(!GTK_SHEET_IS_FROZEN(sheet)){
    gtk_sheet_button_draw(sheet, -1, column);
    g_signal_emit(sheet, sheet_signals[CHANGED], 0, -1, column);
  }
}  

/**
 * gtk_sheet_column_button_get_label:
 * @sheet: a #GtkSheet.
 * @column: column number.
 *
 * Get column button label.
 * Return value: Column button label.
 */
const gchar *
gtk_sheet_column_button_get_label(GtkSheet *sheet, gint column)
{
  g_return_val_if_fail (sheet != NULL, NULL);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);

  if(column < 0 || column >sheet->maxcol) return NULL;

  return(sheet->column[column].button.label);
}

/**
 * gtk_sheet_column_label_set_visibility:
 * @sheet: a #GtkSheet.
 * @col: column number.
 * @visible: TRUE or FALSE
 *
 * Set column label visibility. The default value is TRUE. If FALSE, the column label is hidden.
 */
void
gtk_sheet_column_label_set_visibility(GtkSheet *sheet, gint col, gboolean visible)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(col < 0 || col > sheet->maxcol) return;

  sheet->column[col].button.label_visible = visible;

  if(!GTK_SHEET_IS_FROZEN(sheet)){  
    gtk_sheet_button_draw(sheet, -1, col);
    g_signal_emit(sheet, sheet_signals[CHANGED], 0, -1, col);
  }
}

/**
 * gtk_sheet_columns_labels_set_visibility:
 * @sheet: a #GtkSheet.
 * @visible: TRUE or FALSE
 *
 * Set all columns labels visibility. The default value is TRUE. If FALSE, the columns labels are hidden.
 */
void
gtk_sheet_columns_labels_set_visibility(GtkSheet *sheet, gboolean visible)
{
  gint i;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  for(i = 0; i <= sheet->maxcol; i++)
    gtk_sheet_column_label_set_visibility(sheet, i, visible);
}

/**
 * gtk_sheet_row_button_justify:
 * @sheet: a #GtkSheet.
 * @row: row number
 * @justification : a #GtkJustification :GTK_JUSTIFY_LEFT, RIGHT, CENTER
 *
 * Set the justification(alignment) of the row buttons. 
 */
void
gtk_sheet_row_button_justify(GtkSheet *sheet, gint row, 
                             GtkJustification justification)
{
  GtkSheetButton *button;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(row < 0 || row > sheet->maxrow) return;

  button = &sheet->row[row].button;
  button->justification = justification;

  if(!GTK_SHEET_IS_FROZEN(sheet)){  
    gtk_sheet_button_draw(sheet, row, -1);
    g_signal_emit(sheet, sheet_signals[CHANGED], 0, row, -1);
  }
}  

/**
 * gtk_sheet_column_button_justify:
 * @sheet: a #GtkSheet.
 * @column: column number
 * @justification : a #GtkJustification :GTK_JUSTIFY_LEFT, RIGHT, CENTER
 *
 * Set the justification(alignment) of the column buttons. 
 */
void
gtk_sheet_column_button_justify(GtkSheet *sheet, gint column, 
                                GtkJustification justification)
{
  GtkSheetButton *button;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(column < 0 || column > sheet->maxcol) return;

  button = &sheet->column[column].button;
  button->justification = justification;

  if(!GTK_SHEET_IS_FROZEN(sheet)){  
    gtk_sheet_button_draw(sheet, -1, column);
    g_signal_emit(sheet, sheet_signals[CHANGED], 0, -1, column);
  }
}  

/**
 * gtk_sheet_moveto:
 * @sheet: a #GtkSheet.
 * @row: row number
 * @column: column number
 * @row_align: row alignment
 * @col_align: column alignment
 *
 * Scroll the viewing area of the sheet to the given column and row; 
 * row_align and col_align are between 0-1 representing the location the row should appear on the screnn, 0.0 being top or left,
 * 1.0 being bottom or right; if row or column is negative then there is no change
 */
void
gtk_sheet_moveto (GtkSheet * sheet,
		  gint row,
		  gint column,
	          gfloat row_align,
                  gfloat col_align)
{
  gint x, y;
  guint width, height;
  gint adjust;
  gint min_row, min_col;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));
  g_return_if_fail (sheet->hadjustment != NULL);
  g_return_if_fail (sheet->vadjustment != NULL);

  if (row < 0 || row > sheet->maxrow)
    return;
  if (column < 0 || column > sheet->maxcol)
    return;

  height = sheet->sheet_window_height;
  width = sheet->sheet_window_width;

  /* adjust vertical scrollbar */

  if (row >= 0 && row_align >=0.)
    {
/*
      y = ROW_TOP_YPIXEL(sheet, row) - sheet->voffset -
          row_align*height-
          (1.-row_align)*sheet->row[row].height;
*/
      y = ROW_TOP_YPIXEL (sheet, row) - sheet->voffset
        - (gint) ( row_align*height + (1. - row_align) * sheet->row[row].height);

      /* This forces the sheet to scroll when you don't see the entire cell */
      min_row = row;
      adjust = 0;
      if(row_align == 1.){
        while(min_row >= 0 && min_row > MIN_VISIBLE_ROW(sheet)){
         if(sheet->row[min_row].is_visible) 
                adjust += sheet->row[min_row].height;
         if(adjust >= height){
           break;
         }
         min_row--;
        }
        min_row = MAX(min_row, 0);
        y = ROW_TOP_YPIXEL(sheet, min_row) - sheet->voffset +
            sheet->row[min_row].height - 1;
      }

      if (y < 0)
	sheet->vadjustment->value = 0.0;
      else
	sheet->vadjustment->value = y;

      sheet->old_vadjustment = -1.;
      g_signal_emit_by_name (sheet->vadjustment, "value_changed");

    } 
     
  /* adjust horizontal scrollbar */
  if (column >= 0 && col_align >= 0.)
    {
/*
      x = COLUMN_LEFT_XPIXEL (sheet, column) - sheet->hoffset -
          col_align*width -
          (1.-col_align)*sheet->column[column].width;
*/
      x = COLUMN_LEFT_XPIXEL (sheet, column) - sheet->hoffset
        - (gint) ( col_align*width + (1.-col_align)*sheet->column[column].width);

      /* This forces the sheet to scroll when you don't see the entire cell */
      min_col = column;
      adjust = 0;
      if(col_align == 1.){
        while(min_col >= 0 && min_col > MIN_VISIBLE_COLUMN(sheet)){
         if(sheet->column[min_col].is_visible) 
                adjust += sheet->column[min_col].width;
         if(adjust >= width){
           break;
         }
         min_col--;
        }
        min_col = MAX(min_col, 0);
        x = COLUMN_LEFT_XPIXEL(sheet, min_col) - sheet->hoffset +
            sheet->column[min_col].width - 1;
      }

      if (x < 0)
	sheet->hadjustment->value = 0.0;
      else
	sheet->hadjustment->value = x;

      sheet->old_vadjustment = -1.;
      g_signal_emit_by_name (GTK_OBJECT (sheet->hadjustment), "value_changed");

    }
}

/**
 * gtk_sheet_column_set_sensitivity:
 * @sheet: a #GtkSheet.
 * @column: column number
 * @sensitive: TRUE or FALSE
 *
 * Set column button sensitivity. If sensitivity is TRUE it can be toggled, otherwise it acts as a title.
 */
void 
gtk_sheet_column_set_sensitivity(GtkSheet *sheet, gint column, gboolean sensitive)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(column < 0 || column > sheet->maxcol) return;

  sheet->column[column].is_sensitive=sensitive;
  if(!sensitive)
     sheet->column[column].button.state=GTK_STATE_INSENSITIVE;
  else
     sheet->column[column].button.state=GTK_STATE_NORMAL;

  if(GTK_WIDGET_REALIZED(sheet) && !GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_button_draw(sheet, -1, column);
}

/**
 * gtk_sheet_columns_set_sensitivity:
 * @sheet: a #GtkSheet.
 * @sensitive: TRUE or FALSE
 *
 * Set all columns buttons sensitivity. If sensitivity is TRUE button can be toggled, otherwise  act as titles.
 */
void
gtk_sheet_columns_set_sensitivity(GtkSheet *sheet, gboolean sensitive)
{
  gint i;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  for(i=0; i<=sheet->maxcol; i++)
     gtk_sheet_column_set_sensitivity(sheet, i, sensitive);
}

/**
 * gtk_sheet_columns_set_resizable:
 * @sheet: a #GtkSheet.
 * @resizable: TRUE or FALSE
 *
 * Set columns resizable status.
 */
void
gtk_sheet_columns_set_resizable (GtkSheet *sheet, gboolean resizable)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (resizable != sheet->columns_resizable) {
    sheet->columns_resizable = resizable;
    g_object_notify(G_OBJECT(sheet), "columns-resizable");
  }
}

/**
 * gtk_sheet_columns_resizable:
 * @sheet: a #GtkSheet.
 *
 * Get columns resizable status.
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_columns_resizable (GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

  return sheet->columns_resizable;
}

/**
 * gtk_sheet_row_set_sensitivity:
 * @sheet: a #GtkSheet.
 * @row: row number
 * @sensitive: TRUE or FALSE
 *
 * Set row button sensitivity. If sensitivity is TRUE can be toggled, otherwise it acts as a title . 
 */
void 
gtk_sheet_row_set_sensitivity(GtkSheet *sheet, gint row,  gboolean sensitive)
{

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(row < 0 || row > sheet->maxrow) return;

  sheet->row[row].is_sensitive=sensitive;
  if(!sensitive)
     sheet->row[row].button.state=GTK_STATE_INSENSITIVE;
  else
     sheet->row[row].button.state=GTK_STATE_NORMAL;

  if(GTK_WIDGET_REALIZED(sheet) && !GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_button_draw(sheet, row, -1);
}

/**
 * gtk_sheet_rows_set_sensitivity:
 * @sheet: a #GtkSheet.
 * @sensitive: TRUE or FALSE
 *
 * Set rows buttons sensitivity. If sensitivity is TRUE button can be toggled, otherwise act as titles. 
 */
void
gtk_sheet_rows_set_sensitivity(GtkSheet *sheet, gboolean sensitive)
{
  gint i;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  for(i=0; i<=sheet->maxrow; i++)
     gtk_sheet_row_set_sensitivity(sheet, i, sensitive);
}

/**
 * gtk_sheet_rows_set_resizable:
 * @sheet: a #GtkSheet.
 * @resizable: TRUE or FALSE
 *
 * Set rows resizable status.
 */
void
gtk_sheet_rows_set_resizable (GtkSheet *sheet, gboolean resizable)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (sheet->rows_resizable != resizable) {
    sheet->rows_resizable = resizable;
    g_object_notify(G_OBJECT(sheet), "rows-resizable");
  }

}

/**
 * gtk_sheet_rows_resizable:
 * @sheet: a #GtkSheet.
 *
 * Get rows resizable status.
 * Return value: TRUE or FALSE
 */
gboolean
gtk_sheet_rows_resizable (GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

  return sheet->rows_resizable;
}

/**
 * gtk_sheet_column_set_visibility:
 * @sheet: a #GtkSheet.
 * @column: column number
 * @visible: TRUE or FALSE
 *
 * Set column visibility. The default value is TRUE. If FALSE, the column is hidden.
 */
void
gtk_sheet_column_set_visibility(GtkSheet *sheet, gint column, gboolean visible)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(column < 0 || column > sheet->maxcol) return;
  if(sheet->column[column].is_visible == visible) return;

  sheet->column[column].is_visible = visible;

  gtk_sheet_recalc_left_xpixels(sheet, column);

  if(!GTK_SHEET_IS_FROZEN(sheet) && 
    gtk_sheet_cell_isvisible(sheet, MIN_VISIBLE_ROW(sheet), column)){
      gtk_sheet_range_draw(sheet, NULL);
      size_allocate_column_title_buttons(sheet);
  }
}

/**
 * gtk_sheet_row_set_visibility:
 * @sheet: a #GtkSheet.
 * @row: row number
 * @visible: TRUE or FALSE
 *
 * Set row visibility. The default value is TRUE. If FALSE, the row is hidden.
 */
void
gtk_sheet_row_set_visibility(GtkSheet *sheet, gint row, gboolean visible)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(row < 0 || row > sheet->maxrow) return;
  if(sheet->row[row].is_visible == visible) return;

  sheet->row[row].is_visible = visible;

  gtk_sheet_recalc_top_ypixels(sheet, row);

  if(!GTK_SHEET_IS_FROZEN(sheet) && 
    gtk_sheet_cell_isvisible(sheet, row, MIN_VISIBLE_COLUMN(sheet))){
      gtk_sheet_range_draw(sheet, NULL);
      size_allocate_row_title_buttons(sheet);
  }
}

/**
 * gtk_sheet_select_row:
 * @sheet: a #GtkSheet.
 * @row: row number
 *
 * Select the row. The range is then highlighted, and the bounds are stored in sheet->range. 
 */
void
gtk_sheet_select_row (GtkSheet * sheet,
		      gint row)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (row < 0 || row > sheet->maxrow)
    return;

  if(sheet->state != GTK_SHEET_NORMAL) 
     gtk_sheet_real_unselect_range(sheet, NULL);
  else
  {
     gboolean veto = TRUE;
     veto = gtk_sheet_deactivate_cell(sheet);
     if(!veto) return;
  }

  sheet->state=GTK_SHEET_ROW_SELECTED;                     
  sheet->range.row0=row;
  sheet->range.col0=0;
  sheet->range.rowi=row;
  sheet->range.coli=sheet->maxcol;
  g_object_notify(G_OBJECT(sheet), "selected-range");
  sheet->active_cell.row=row;
  sheet->active_cell.col=0;
  g_object_notify(G_OBJECT(sheet), "active-cell");

  g_object_notify(G_OBJECT(sheet), "state");

  g_signal_emit (sheet, sheet_signals[SELECT_ROW], 0, row);
  gtk_sheet_real_select_range(sheet, NULL);

}

/**
 * gtk_sheet_select_column:
 * @sheet: a #GtkSheet.
 * @column: column number
 *
 * Select the column. The range is then highlighted, and the bounds are stored in sheet->range. 
 */
void
gtk_sheet_select_column (GtkSheet * sheet,
		         gint column)
{
  
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (column < 0 || column > sheet->maxcol)
    return;

  if(sheet->state != GTK_SHEET_NORMAL) 
     gtk_sheet_real_unselect_range(sheet, NULL);
  else
  {
     gboolean veto = TRUE;
     veto = gtk_sheet_deactivate_cell(sheet);
     if(!veto) return;
  }

  sheet->state=GTK_SHEET_COLUMN_SELECTED;                     
  sheet->range.row0=0;
  sheet->range.col0=column;
  sheet->range.rowi=sheet->maxrow;
  sheet->range.coli=column;
  g_object_notify(G_OBJECT(sheet), "selected-range");
  sheet->active_cell.row=0;
  sheet->active_cell.col=column;
  g_object_notify(G_OBJECT(sheet), "active-cell");


  g_object_notify(G_OBJECT(sheet), "state");
  g_signal_emit (sheet, sheet_signals[SELECT_COLUMN], 0, column);
  gtk_sheet_real_select_range(sheet, NULL);

}

/**
 * gtk_sheet_clip_range:
 * @sheet: a #GtkSheet.
 * @range: #GtkSheetRange to be saved
 *
 * Marks a block of cells to be copied to the clipboard. A dashed line will be
 * drawn around the block of cells indicated by @range to indicate that the cell
 * contents are copied to the clipboard. Only one range of cells can be clipped 
 * at a time. This method emits the "clip-range" signal.
 */
void
gtk_sheet_clip_range (GtkSheet *sheet, const GtkSheetRange *range)
{

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(GTK_SHEET_IN_CLIP(sheet)) return;

  GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IN_CLIP);

  if(range == NULL)
    sheet->clip_range = sheet->range;
  else
    sheet->clip_range=*range;

  sheet->interval=0;
  sheet->clip_timer=gtk_timeout_add(TIMEOUT_FLASH, gtk_sheet_flash, sheet); 

  g_signal_emit(sheet, sheet_signals[CLIP_RANGE], 0, &sheet->clip_range);

}

/**
 * gtk_sheet_unclip_range:
 * @sheet: a #GtkSheet.
 *
 * Free clipboard. 
 */
void
gtk_sheet_unclip_range(GtkSheet *sheet)
{

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!GTK_SHEET_IN_CLIP(sheet)) return;

  GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IN_CLIP);
  gtk_timeout_remove(sheet->clip_timer);
  gtk_sheet_range_draw(sheet, &sheet->clip_range);

  if(gtk_sheet_range_isvisible(sheet, sheet->range))
    gtk_sheet_range_draw(sheet, &sheet->range);
}

/**
 * gtk_sheet_in_clip:
 * @sheet: a #GtkSheet.
 *
 * Get the clip status of #GtkSheet. 
 * Return value: TRUE or FALSE 
 */
gboolean
gtk_sheet_in_clip (GtkSheet *sheet)
{
  g_return_val_if_fail (sheet != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

  return GTK_SHEET_IN_CLIP(sheet);
}


static gint
gtk_sheet_flash(gpointer data)
{
  GtkSheet *sheet;
  gint x,y,width,height;
  GdkRectangle clip_area;

  sheet=GTK_SHEET(data);

  if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return TRUE;
  if(!GTK_WIDGET_DRAWABLE(GTK_WIDGET(sheet))) return TRUE;
  if(!gtk_sheet_range_isvisible(sheet, sheet->clip_range)) return TRUE;
  if(GTK_SHEET_IN_XDRAG(sheet)) return TRUE; 
  if(GTK_SHEET_IN_YDRAG(sheet)) return TRUE; 

  GDK_THREADS_ENTER();
 
  x=COLUMN_LEFT_XPIXEL(sheet,sheet->clip_range.col0)+1;
  y=ROW_TOP_YPIXEL(sheet,sheet->clip_range.row0)+1;
  width=COLUMN_LEFT_XPIXEL(sheet,sheet->clip_range.coli)-x+ 
             sheet->column[sheet->clip_range.coli].width-1;
  height=ROW_TOP_YPIXEL(sheet,sheet->clip_range.rowi)-y+
             sheet->row[sheet->clip_range.rowi].height-1;

  clip_area.x=COLUMN_LEFT_XPIXEL(sheet, MIN_VISIBLE_COLUMN(sheet));
  clip_area.y=ROW_TOP_YPIXEL(sheet, MIN_VISIBLE_ROW(sheet));
  clip_area.width=sheet->sheet_window_width;
  clip_area.height=sheet->sheet_window_height;

  if(x<0) {
     width=width+x+1;
     x=-1;
  }
  if(width>clip_area.width) width=clip_area.width+10;
  if(y<0) {
     height=height+y+1;
     y=-1;
  }
  if(height>clip_area.height) height=clip_area.height+10;

  gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  x, y,
                  x, y,
                  1, height);

  gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  x, y,
                  x, y,
                  width, 1);

  gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  x, y+height,
                  x, y+height,
                  width, 1);

  gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  x+width, y,
                  x+width, y,
                  1, height);


  sheet->interval=sheet->interval+1;
  if(sheet->interval==TIME_INTERVAL) sheet->interval=0;

  gdk_gc_set_dashes(sheet->xor_gc, sheet->interval, (gint8*)"\4\4", 2);
  gtk_sheet_draw_flashing_range(sheet,sheet->clip_range);
  gdk_gc_set_dashes(sheet->xor_gc, 0, (gint8*)"\4\4", 2);

  GDK_THREADS_LEAVE();

  return TRUE;

}

static void
gtk_sheet_draw_flashing_range(GtkSheet *sheet, GtkSheetRange range)
{
  GdkRectangle clip_area;
  gint x,y,width,height;

  if(!gtk_sheet_range_isvisible(sheet, sheet->clip_range)) return;
  
  clip_area.x=COLUMN_LEFT_XPIXEL(sheet, MIN_VISIBLE_COLUMN(sheet));
  clip_area.y=ROW_TOP_YPIXEL(sheet, MIN_VISIBLE_ROW(sheet));
  clip_area.width=sheet->sheet_window_width;
  clip_area.height=sheet->sheet_window_height;

  gdk_gc_set_clip_rectangle(sheet->xor_gc, &clip_area);  

  x=COLUMN_LEFT_XPIXEL(sheet,sheet->clip_range.col0)+1;
  y=ROW_TOP_YPIXEL(sheet,sheet->clip_range.row0)+1;
  width=COLUMN_LEFT_XPIXEL(sheet,sheet->clip_range.coli)-x+ 
             sheet->column[sheet->clip_range.coli].width-1;
  height=ROW_TOP_YPIXEL(sheet,sheet->clip_range.rowi)-y+
             sheet->row[sheet->clip_range.rowi].height-1;

  if(x<0) {
     width=width+x+1;
     x=-1;
  }
  if(width>clip_area.width) width=clip_area.width+10;
  if(y<0) {
     height=height+y+1;
     y=-1;
  }
  if(height>clip_area.height) height=clip_area.height+10;

  gdk_gc_set_line_attributes(sheet->xor_gc, 1, 1, 0 ,0 );

  gdk_draw_rectangle(sheet->sheet_window, sheet->xor_gc, FALSE, 
                     x, y,
                     width, height);

  gdk_gc_set_line_attributes (sheet->xor_gc, 1, 0, 0, 0);

  gdk_gc_set_clip_rectangle(sheet->xor_gc, NULL);

}

static gint
gtk_sheet_range_isvisible (GtkSheet * sheet,
			 GtkSheetRange range)
{
  g_return_val_if_fail (sheet != NULL, FALSE);

  if (range.row0 < 0 || range.row0 > sheet->maxrow)
    return FALSE;

  if (range.rowi < 0 || range.rowi > sheet->maxrow)
    return FALSE;

  if (range.col0 < 0 || range.col0 > sheet->maxcol)
    return FALSE;

  if (range.coli < 0 || range.coli > sheet->maxcol)
    return FALSE;

  if (range.rowi < MIN_VISIBLE_ROW (sheet))
    return FALSE;

  if (range.row0 > MAX_VISIBLE_ROW (sheet))
    return FALSE;

  if (range.coli < MIN_VISIBLE_COLUMN (sheet))
    return FALSE;

  if (range.col0 > MAX_VISIBLE_COLUMN (sheet))
    return FALSE;

  return TRUE;
}

static gint
gtk_sheet_cell_isvisible (GtkSheet * sheet,
			  gint row, gint column)
{
  GtkSheetRange range;

  range.row0 = row;
  range.col0 = column;
  range.rowi = row;
  range.coli = column;

  return gtk_sheet_range_isvisible(sheet, range);
}

/**
 * gtk_sheet_get_visible_range:
 * @sheet: a #GtkSheet.
 * @range : a selected #GtkSheetRange 
 * struct _GtkSheetRange { gint row0,col0; //  upper-left cell 
 * 			  gint rowi,coli;  // lower-right cell  };
 *
 * Get sheet's ranges in a #GkSheetRange structure. 
 */
void 
gtk_sheet_get_visible_range(GtkSheet *sheet, GtkSheetRange *range)
{

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet)) ;
  g_return_if_fail (range != NULL);

  range->row0 = MIN_VISIBLE_ROW(sheet);
  range->col0 = MIN_VISIBLE_COLUMN(sheet);
  range->rowi = MAX_VISIBLE_ROW(sheet);
  range->coli = MAX_VISIBLE_COLUMN(sheet);

}

/**
 * gtk_sheet_get_vadjustment:
 * @sheet: a #GtkSheet.
 *
 * Get vertical scroll adjustments.
 * Return value: a #GtkAdjustment
 */
GtkAdjustment *
gtk_sheet_get_vadjustment (GtkSheet * sheet)
{
  g_return_val_if_fail (sheet != NULL, NULL);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);

  return sheet->vadjustment;
}

/**
 * gtk_sheet_get_hadjustment:
 * @sheet: a #GtkSheet.
 *
 * Get horizontal scroll adjustments.
 * Return value: a #GtkAdjustment
 */
GtkAdjustment *
gtk_sheet_get_hadjustment (GtkSheet * sheet)
{
  g_return_val_if_fail (sheet != NULL, NULL);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);

  return sheet->hadjustment;
}

/**
 * gtk_sheet_set_vadjustment:
 * @sheet: a #GtkSheet.
 * @adjustment: a #GtkAdjustment
 *
 * Change vertical scroll adjustments.
 */
void
gtk_sheet_set_vadjustment (GtkSheet      *sheet,
			   GtkAdjustment *adjustment)
{
  GtkAdjustment *old_adjustment;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));
  if (adjustment)
    g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));
  
  if (sheet->vadjustment == adjustment)
    return;
  
  old_adjustment = sheet->vadjustment;

  if (sheet->vadjustment)
    {
      /*gtk_signal_disconnect_by_data (GTK_OBJECT (sheet->vadjustment), sheet);*/
      g_signal_handlers_disconnect_matched(sheet->vadjustment, 
                                           G_SIGNAL_MATCH_DATA,
                                           0, 0, NULL, NULL, sheet); 
      g_object_unref (G_OBJECT (sheet->vadjustment));
    }

  sheet->vadjustment = adjustment;

  if (sheet->vadjustment)
    {
      g_object_ref_sink ( sheet->vadjustment );

      g_signal_connect (GTK_OBJECT (sheet->vadjustment), "changed",
			  (GCallback) vadjustment_changed,
			  (gpointer) sheet);
      g_signal_connect (GTK_OBJECT (sheet->vadjustment), "value_changed",
			  (GCallback) vadjustment_value_changed,
			  (gpointer) sheet);
    }

  if (!sheet->vadjustment || !old_adjustment)
     {
       gtk_widget_queue_resize (GTK_WIDGET (sheet));
       return;
     }

  sheet->old_vadjustment = sheet->vadjustment->value;
}

/**
 * gtk_sheet_set_hadjustment:
 * @sheet: a #GtkSheet.
 * @adjustment: a #GtkAdjustment
 *
 * Change horizontal scroll adjustments.
 */
void
gtk_sheet_set_hadjustment (GtkSheet      *sheet,
			   GtkAdjustment *adjustment)
{
  GtkAdjustment *old_adjustment;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));
  if (adjustment)
    g_return_if_fail (GTK_IS_ADJUSTMENT (adjustment));
  
  if (sheet->hadjustment == adjustment)
    return;
  
  old_adjustment = sheet->hadjustment;

  if (sheet->hadjustment)
    {
      g_signal_handlers_disconnect_matched(sheet->hadjustment, 
                                           G_SIGNAL_MATCH_DATA,
                                           0, 0, NULL, NULL, sheet); 
      /*gtk_signal_disconnect_by_data (GTK_OBJECT (sheet->hadjustment), sheet);*/
      g_object_unref (G_OBJECT (sheet->hadjustment));
    }

  sheet->hadjustment = adjustment;

  if (sheet->hadjustment)
    {
      g_object_ref_sink ( sheet->hadjustment );

      g_signal_connect (GTK_OBJECT (sheet->hadjustment), "changed",
			  (GCallback) hadjustment_changed,
			  (gpointer) sheet);
      g_signal_connect (GTK_OBJECT (sheet->hadjustment), "value_changed",
			  (GCallback) hadjustment_value_changed,
			  (gpointer) sheet);
    }

  if (!sheet->hadjustment || !old_adjustment)
     {
       gtk_widget_queue_resize (GTK_WIDGET (sheet));
       return;
     }

  sheet->old_hadjustment = sheet->hadjustment->value;
}

/**
 * gtk_sheet_set_scroll_adjustments:
 * @sheet: a #GtkSheet.
 * @hadjustment: a #GtkAdjustment
 * @vadjustment: a #GtkAdjustment
 *
 * Change horizontal and vertical scroll adjustments.
 */
static void
gtk_sheet_set_scroll_adjustments (GtkSheet *sheet,
				  GtkAdjustment *hadjustment,
				  GtkAdjustment *vadjustment)
{
   if(sheet->hadjustment != hadjustment)
         gtk_sheet_set_hadjustment (sheet, hadjustment);
   if(sheet->vadjustment != vadjustment)
         gtk_sheet_set_vadjustment (sheet, vadjustment);
}

static void
gtk_sheet_finalize (GObject * object)
{
  GtkSheet *sheet;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GTK_IS_SHEET (object));

  sheet = GTK_SHEET (object);

  /* get rid of all the cells */
  gtk_sheet_range_clear (sheet, NULL);
  gtk_sheet_range_delete(sheet, NULL);

  gtk_sheet_delete_rows (sheet, 0, sheet->maxrow + 1);
  gtk_sheet_delete_columns (sheet, 0, sheet->maxcol + 1);

  DeleteRow (sheet, 0, sheet->maxrow + 1);
  DeleteColumn (sheet, 0, sheet->maxcol + 1);

  g_free(sheet->row);
  sheet->row = NULL;
  g_free(sheet->column);
  sheet->column = NULL;
  g_free(sheet->data);
  sheet->data = NULL;

  if(sheet->name){
      g_free(sheet->name);
      sheet->name = NULL;
  }

  if (G_OBJECT_CLASS (gtk_sheet_parent_class)->finalize)
    (*G_OBJECT_CLASS (gtk_sheet_parent_class)->finalize) (object);
}

static void
gtk_sheet_destroy (GtkObject * object)
{
  GtkSheet *sheet;
  GList *children;

  g_return_if_fail (object != NULL);
  g_return_if_fail (GTK_IS_SHEET (object));

  sheet = GTK_SHEET (object);

  /* destroy the entry */
  if(sheet->sheet_entry && GTK_IS_WIDGET(sheet->sheet_entry)){
    gtk_widget_destroy (sheet->sheet_entry);
    sheet->sheet_entry = NULL;
  }

  /* destroy the global selection button */
  if(sheet->button && GTK_IS_WIDGET(sheet->button)){
    gtk_widget_destroy (sheet->button);
    sheet->button = NULL;
  }

  if(sheet->timer){
     gtk_timeout_remove(sheet->timer);
     sheet->timer = 0;
  }

  if(sheet->clip_timer){
     gtk_timeout_remove(sheet->clip_timer);
     sheet->clip_timer = 0;
  }

  /* unref adjustments */
  if (sheet->hadjustment)
    {
      g_signal_handlers_disconnect_matched(sheet->hadjustment, 
                                           G_SIGNAL_MATCH_DATA,
                                           0, 0, NULL, NULL, sheet); 
      /*gtk_signal_disconnect_by_data (GTK_OBJECT (sheet->hadjustment), sheet);*/
      g_object_unref (G_OBJECT (sheet->hadjustment));
      sheet->hadjustment = NULL;
    }
  if (sheet->vadjustment)
    {
      g_signal_handlers_disconnect_matched(sheet->vadjustment, 
                                           G_SIGNAL_MATCH_DATA,
                                           0, 0, NULL, NULL, sheet); 
      /*gtk_signal_disconnect_by_data (GTK_OBJECT (sheet->vadjustment), sheet);*/
      g_object_unref (G_OBJECT (sheet->vadjustment));
      sheet->vadjustment = NULL;
    }

  children = sheet->children;
  while(children){
    GtkSheetChild *child = (GtkSheetChild *)children->data;
    if(child && child->widget) 
      gtk_sheet_remove(GTK_CONTAINER(sheet), child->widget);
    children = sheet->children;
  }  
  sheet->children = NULL;

  if (GTK_OBJECT_CLASS (gtk_sheet_parent_class)->destroy)
    (*GTK_OBJECT_CLASS (gtk_sheet_parent_class)->destroy) (object);
}

static void
gtk_sheet_style_set (GtkWidget *widget,
		     GtkStyle  *previous_style)
{
  GtkSheet *sheet;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_SHEET (widget));

  if (GTK_WIDGET_CLASS (gtk_sheet_parent_class)->style_set)
    (*GTK_WIDGET_CLASS (gtk_sheet_parent_class)->style_set) (widget, previous_style);

  sheet = GTK_SHEET (widget);

  if(GTK_WIDGET_REALIZED(widget))
     {
       gtk_style_set_background (widget->style, widget->window, widget->state);
     }

}

static void
gtk_sheet_realize (GtkWidget * widget)
{
  GtkSheet *sheet;
  GdkWindowAttr attributes;
  gint attributes_mask;
  GdkGCValues values, auxvalues;
  GdkColormap *colormap;
  gchar *name;
  GtkSheetChild *child;
  GList *children;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_SHEET (widget));

  sheet = GTK_SHEET (widget);

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;

  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);

  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
			    GDK_BUTTON_PRESS_MASK |
			    GDK_BUTTON_RELEASE_MASK |
			    GDK_KEY_PRESS_MASK |
			    GDK_POINTER_MOTION_MASK |
			    GDK_POINTER_MOTION_HINT_MASK);
  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP |
                    GDK_WA_CURSOR;

  attributes.cursor = gdk_cursor_new(GDK_TOP_LEFT_ARROW);

  /* main window */
  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, attributes_mask);

  gdk_window_set_user_data (widget->window, sheet);

  widget->style = gtk_style_attach (widget->style, widget->window);

  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);

  attributes.x = 0;
  if(sheet->row_titles_visible)
       attributes.x = sheet->row_title_area.width;
  attributes.y = 0;
  attributes.width = sheet->column_title_area.width;
  attributes.height = sheet->column_title_area.height;

  /* column-title window */
  sheet->column_title_window = gdk_window_new (widget->window, &attributes, attributes_mask);
  gdk_window_set_user_data (sheet->column_title_window, sheet);
  gtk_style_set_background (widget->style, sheet->column_title_window, GTK_STATE_NORMAL);

  attributes.x = 0;
  attributes.y = 0;
  if(sheet->column_titles_visible)
       attributes.y = sheet->column_title_area.height;
  attributes.width = sheet->row_title_area.width;
  attributes.height = sheet->row_title_area.height;

  /* row-title window */
  sheet->row_title_window = gdk_window_new (widget->window, &attributes, attributes_mask);
  gdk_window_set_user_data (sheet->row_title_window, sheet);
  gtk_style_set_background (widget->style, sheet->row_title_window, GTK_STATE_NORMAL);

  /* sheet-window */
  attributes.cursor = gdk_cursor_new(GDK_PLUS);

  attributes.x = 0;
  attributes.y = 0;
  attributes.width = sheet->sheet_window_width, 
  attributes.height = sheet->sheet_window_height;

  sheet->sheet_window = gdk_window_new (widget->window, &attributes, attributes_mask);
  gdk_window_set_user_data (sheet->sheet_window, sheet);

  gdk_window_set_background (sheet->sheet_window, &widget->style->white);
  gdk_window_show (sheet->sheet_window);

  /* backing_pixmap */
  gtk_sheet_make_backing_pixmap(sheet, 0, 0);  

  /* GCs */
  if(sheet->fg_gc) 
      g_object_unref(sheet->fg_gc);
  if(sheet->bg_gc) 
      g_object_unref(sheet->bg_gc);
  sheet->fg_gc = gdk_gc_new (widget->window);
  sheet->bg_gc = gdk_gc_new (widget->window);

  colormap = gtk_widget_get_colormap(widget);
  gdk_color_parse("white", &widget->style->white);
  gdk_colormap_alloc_color(colormap, &widget->style->white, TRUE, TRUE);
  gdk_color_parse("black", &widget->style->black);
  gdk_colormap_alloc_color(colormap, &widget->style->black, TRUE, TRUE);

  gdk_gc_get_values(sheet->fg_gc, &auxvalues);

  values.foreground = widget->style->white;
  values.function = GDK_INVERT;
  values.subwindow_mode = GDK_INCLUDE_INFERIORS;
  if(sheet->xor_gc)
    g_object_unref(sheet->xor_gc);
  sheet->xor_gc = gdk_gc_new_with_values (widget->window,
					  &values,
					  GDK_GC_FOREGROUND |
					  GDK_GC_FUNCTION |
					  GDK_GC_SUBWINDOW);

  if(sheet->sheet_entry->parent){
          g_object_ref(G_OBJECT(sheet->sheet_entry));
          gtk_widget_unparent(sheet->sheet_entry);
  }
  gtk_widget_set_parent_window (sheet->sheet_entry, sheet->sheet_window);
  gtk_widget_set_parent(sheet->sheet_entry, GTK_WIDGET(sheet));

  if(sheet->button && sheet->button->parent){
          g_object_ref(G_OBJECT(sheet->button));
          gtk_widget_unparent(sheet->button);
  }
  gtk_widget_set_parent_window(sheet->button, sheet->sheet_window);
  gtk_widget_set_parent(sheet->button, GTK_WIDGET(sheet));

/*
  gtk_sheet_activate_cell(sheet, sheet->active_cell.row, sheet->active_cell.col);
*/
  if(!sheet->cursor_drag)
       sheet->cursor_drag = gdk_cursor_new(GDK_PLUS);
 
  if(sheet->column_titles_visible)
     gdk_window_show(sheet->column_title_window);
  if(sheet->row_titles_visible)
     gdk_window_show(sheet->row_title_window);

  size_allocate_row_title_buttons(sheet);
  size_allocate_column_title_buttons(sheet);

  name = g_strdup(sheet->name);
  gtk_sheet_set_title(sheet, name);

  g_free(name);

  children = sheet->children;
  while(children)
    {
      child = children->data;
      children = children->next;
 
      gtk_sheet_realize_child(sheet, child);
    }
}

static void
create_global_button(GtkSheet *sheet)
{
   sheet->button = gtk_button_new_with_label(" ");

   g_signal_connect (GTK_OBJECT (sheet->button),
		      "pressed",
		      (GCallback) global_button_clicked,
		      (gpointer) sheet);
}

static void
size_allocate_global_button(GtkSheet *sheet)
{
  GtkAllocation allocation;

  if(!sheet->column_titles_visible) return;
  if(!sheet->row_titles_visible) return;

  gtk_widget_size_request(sheet->button, NULL);

  allocation.x=0;
  allocation.y=0;
  allocation.width=sheet->row_title_area.width;
  allocation.height=sheet->column_title_area.height;

  gtk_widget_size_allocate(sheet->button, &allocation);
  gtk_widget_show(sheet->button);
}

static void
global_button_clicked(GtkWidget *widget, gpointer data)
{
  gboolean veto;

  gtk_sheet_click_cell(GTK_SHEET(data), -1, -1, &veto);
  gtk_widget_grab_focus(GTK_WIDGET(data));
}


static void
gtk_sheet_unrealize (GtkWidget * widget)
{
  GtkSheet *sheet;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_SHEET (widget));

  sheet = GTK_SHEET (widget);

  gdk_cursor_unref (sheet->cursor_drag);

  g_object_unref (sheet->xor_gc);
  g_object_unref (sheet->fg_gc);
  g_object_unref (sheet->bg_gc);

  gdk_window_destroy (sheet->sheet_window);
  gdk_window_destroy (sheet->column_title_window);
  gdk_window_destroy (sheet->row_title_window);

  if (sheet->pixmap){
    g_object_unref (G_OBJECT(sheet->pixmap));
    sheet->pixmap = NULL;
  }

  sheet->column_title_window=NULL;
  sheet->sheet_window = NULL;
  sheet->cursor_drag = NULL;
  sheet->xor_gc = NULL;
  sheet->fg_gc = NULL;
  sheet->bg_gc = NULL;

  if (GTK_WIDGET_CLASS (gtk_sheet_parent_class)->unrealize)
    (* GTK_WIDGET_CLASS (gtk_sheet_parent_class)->unrealize) (widget);
}

static void
gtk_sheet_map (GtkWidget * widget)
{
  GtkSheet *sheet;
  GtkSheetChild *child;
  GList *children;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_SHEET (widget));

  sheet = GTK_SHEET (widget);

  if (!GTK_WIDGET_MAPPED (widget))
    {
      GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);

      if(!sheet->cursor_drag) sheet->cursor_drag=gdk_cursor_new(GDK_PLUS);

      gdk_window_show (widget->window);

      gdk_window_show (sheet->sheet_window);

      if(sheet->column_titles_visible){
           size_allocate_column_title_buttons(sheet);
           gdk_window_show (sheet->column_title_window);
      }
      if(sheet->row_titles_visible){
           size_allocate_row_title_buttons(sheet);
           gdk_window_show (sheet->row_title_window);
      }

      if(!GTK_WIDGET_MAPPED (sheet->sheet_entry)){
      	          gtk_widget_show (sheet->sheet_entry);
   	          gtk_widget_map (sheet->sheet_entry);
      }

      if (GTK_WIDGET_VISIBLE (sheet->button) &&
	  !GTK_WIDGET_MAPPED (sheet->button)){
                  gtk_widget_show(sheet->button);
	          gtk_widget_map (sheet->button);
      }

      if(GTK_BIN(sheet->button)->child)
        if (GTK_WIDGET_VISIBLE (GTK_BIN(sheet->button)->child) &&
  	   !GTK_WIDGET_MAPPED (GTK_BIN(sheet->button)->child))
  	          gtk_widget_map (GTK_BIN(sheet->button)->child);

      gtk_sheet_range_draw(sheet, NULL);
      gtk_sheet_activate_cell(sheet, 
                              sheet->active_cell.row, 
                              sheet->active_cell.col);

      children = sheet->children;
      while (children)
      {
        child = children->data;
        children = children->next;

        if (GTK_WIDGET_VISIBLE (child->widget) &&
    	    !GTK_WIDGET_MAPPED (child->widget)){
	  gtk_widget_map (child->widget);
          gtk_sheet_position_child(sheet, child);
        }
      }

    }
}

static void
gtk_sheet_unmap (GtkWidget * widget)
{
  GtkSheet *sheet;
  GtkSheetChild *child;
  GList *children;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_SHEET (widget));

  sheet = GTK_SHEET (widget);

  if (GTK_WIDGET_MAPPED (widget))
    {
      GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED);

      gdk_window_hide (sheet->sheet_window);
      if(sheet->column_titles_visible)
          gdk_window_hide (sheet->column_title_window);
      if(sheet->row_titles_visible)
          gdk_window_hide (sheet->row_title_window);
      gdk_window_hide (widget->window);

      if (GTK_WIDGET_MAPPED (sheet->sheet_entry))
	gtk_widget_unmap (sheet->sheet_entry);

      if (GTK_WIDGET_MAPPED (sheet->button))
	gtk_widget_unmap (sheet->button);

      children = sheet->children;
      while (children)
        {
          child = children->data;
          children = children->next;

          if (GTK_WIDGET_VISIBLE (child->widget) &&
	      GTK_WIDGET_MAPPED (child->widget))
                {
  	             gtk_widget_unmap (child->widget);
                }
        }

    }
}


static void
gtk_sheet_cell_draw_default (GtkSheet *sheet, gint row, gint col)
{
  GtkWidget *widget;
  GdkGC *fg_gc, *bg_gc;
  GtkSheetCellAttr attributes;
  GdkRectangle area;

  g_return_if_fail (sheet != NULL);

  /* bail now if we arn't drawable yet */
  if (!GTK_WIDGET_DRAWABLE (sheet)) return;

  if (row < 0 || row > sheet->maxrow) return;
  if (col < 0 || col > sheet->maxcol) return;
  if (!sheet->column[col].is_visible) return;
  if (!sheet->row[row].is_visible) return;

  widget = GTK_WIDGET (sheet);

  gtk_sheet_get_attributes(sheet, row, col, &attributes);
 
  /* select GC for background rectangle */
  gdk_gc_set_foreground (sheet->fg_gc, &attributes.foreground);
  gdk_gc_set_foreground (sheet->bg_gc, &attributes.background);

  fg_gc = sheet->fg_gc;
  bg_gc = sheet->bg_gc;

  area.x=COLUMN_LEFT_XPIXEL(sheet,col);
  area.y=ROW_TOP_YPIXEL(sheet,row);
  area.width=sheet->column[col].width;
  area.height=sheet->row[row].height;

  gdk_draw_rectangle (sheet->pixmap,
  	              bg_gc,
	              TRUE,
	              area.x,
                      area.y,
	              area.width,
                      area.height);

  gdk_gc_set_line_attributes (sheet->fg_gc, 1, 0, 0, 0);

  if(sheet->show_grid){
    gdk_gc_set_foreground (sheet->bg_gc, &sheet->grid_color);

    gdk_draw_rectangle (sheet->pixmap,
                        sheet->bg_gc,
    	                FALSE,
	                area.x, area.y,
	                area.width, area.height);
  }
}

static void
gtk_sheet_cell_draw_border (GtkSheet *sheet, gint row, gint col, gint mask)
{
  GtkWidget *widget;
  GdkGC *fg_gc, *bg_gc;
  GtkSheetCellAttr attributes;
  GdkRectangle area;
  guint width;

  g_return_if_fail (sheet != NULL);

  /* bail now if we arn't drawable yet */
  if (!GTK_WIDGET_DRAWABLE (sheet)) return;

  if (row < 0 || row > sheet->maxrow) return;
  if (col < 0 || col > sheet->maxcol) return;
  if (!sheet->column[col].is_visible) return;
  if (!sheet->row[row].is_visible) return;

  widget = GTK_WIDGET (sheet);

  gtk_sheet_get_attributes(sheet, row, col, &attributes);

  /* select GC for background rectangle */
  gdk_gc_set_foreground (sheet->fg_gc, &attributes.border.color);
  gdk_gc_set_foreground (sheet->bg_gc, &attributes.background);

  fg_gc = sheet->fg_gc;
  bg_gc = sheet->bg_gc;

  area.x=COLUMN_LEFT_XPIXEL(sheet,col);
  area.y=ROW_TOP_YPIXEL(sheet,row);
  area.width=sheet->column[col].width;
  area.height=sheet->row[row].height;

  width = attributes.border.width;
  gdk_gc_set_line_attributes(sheet->fg_gc, attributes.border.width,
                                           attributes.border.line_style,
                                           attributes.border.cap_style,
                                           attributes.border.join_style);
  if(width>0){

   if(attributes.border.mask & GTK_SHEET_LEFT_BORDER & mask)
      gdk_draw_line(sheet->pixmap, sheet->fg_gc,
                    area.x, area.y-width/2,
                    area.x, area.y+area.height+width/2+1);

   if(attributes.border.mask & GTK_SHEET_RIGHT_BORDER & mask)
      gdk_draw_line(sheet->pixmap, sheet->fg_gc,
                    area.x+area.width, area.y-width/2,
                    area.x+area.width, 
                    area.y+area.height+width/2+1);

   if(attributes.border.mask & GTK_SHEET_TOP_BORDER & mask)
      gdk_draw_line(sheet->pixmap, sheet->fg_gc,
                    area.x-width/2,area.y,
                    area.x+area.width+width/2+1, 
                    area.y);

   if(attributes.border.mask & GTK_SHEET_BOTTOM_BORDER & mask)
      gdk_draw_line(sheet->pixmap, sheet->fg_gc,
                    area.x-width/2, area.y+area.height,
                    area.x+area.width+width/2+1, 
                    area.y+area.height);
  }

}


static void
gtk_sheet_cell_draw_label (GtkSheet *sheet, gint row, gint col)
{
  GtkWidget *widget;
  GdkRectangle area, clip_area;
  gint i;
  gint text_width, text_height, y;
  gint xoffset=0;  
  gint size, sizel, sizer;
  GdkGC *fg_gc, *bg_gc;
  GtkSheetCellAttr attributes;
  PangoLayout *layout;
  PangoRectangle rect;
  PangoRectangle logical_rect;
  PangoLayoutLine *line;
  PangoFontMetrics *metrics;
  PangoFontDescription *font_desc;
  PangoContext *context = gtk_widget_get_pango_context(GTK_WIDGET(sheet)); 
  gint ascent, descent, y_pos;

  char *label;

  g_return_if_fail (sheet != NULL);

   /* bail now if we aren't drawable yet */
   if (!GTK_WIDGET_DRAWABLE (sheet))
    return;

  if (row > sheet->maxallocrow) return;
  if (col > sheet->maxalloccol) return;
  if (!sheet->data[row]) return;
  if (!sheet->data[row][col]) return;
  if (!sheet->data[row][col]->text || strlen(sheet->data[row][col]->text)==0)
      return;

  if (row < 0 || row > sheet->maxrow) return;
  if (col < 0 || col > sheet->maxcol) return;
  if (!sheet->column[col].is_visible) return;
  if (!sheet->row[row].is_visible) return;


  widget = GTK_WIDGET(sheet);

  label = sheet->data[row][col]->text;

  gtk_sheet_get_attributes(sheet, row, col, &attributes);

  /* select GC for background rectangle */
  gdk_gc_set_foreground (sheet->fg_gc, &attributes.foreground);
  gdk_gc_set_foreground (sheet->bg_gc, &attributes.background);

  fg_gc = sheet->fg_gc;
  bg_gc = sheet->bg_gc;

  area.x=COLUMN_LEFT_XPIXEL(sheet,col);
  area.y=ROW_TOP_YPIXEL(sheet,row);
  area.width=sheet->column[col].width;
  area.height=sheet->row[row].height;

  clip_area = area;

  layout = gtk_widget_create_pango_layout (GTK_WIDGET(sheet), label);
  font_desc = (attributes.font_desc == NULL) ? GTK_WIDGET(sheet)->style->font_desc : attributes.font_desc;
  pango_layout_set_font_description (layout, font_desc);

  pango_layout_get_pixel_extents (layout, NULL, &rect);

  line = pango_layout_get_lines (layout)->data;
  pango_layout_line_get_extents (line, NULL, &logical_rect);

  metrics = pango_context_get_metrics(context,
                                  font_desc,
                                  pango_context_get_language(context)); 

  ascent = pango_font_metrics_get_ascent(metrics) / PANGO_SCALE;
  descent = pango_font_metrics_get_descent(metrics) / PANGO_SCALE;

  pango_font_metrics_unref(metrics);

  /* Align primarily for locale's ascent/descent */

  logical_rect.height /= PANGO_SCALE;
  logical_rect.y /= PANGO_SCALE;
  y_pos =  area.height - logical_rect.height;

  if (logical_rect.height > area.height)
    y_pos = (logical_rect.height - area.height - 2*CELLOFFSET) / 2;
  else if (y_pos < 0)
    y_pos = 0;
  else if (y_pos + logical_rect.height > area.height)
    y_pos = area.height - logical_rect.height;

  text_width = rect.width;
  text_height = rect.height;
  y = area.y + y_pos - CELLOFFSET;

  switch(attributes.justification){
    case GTK_JUSTIFY_RIGHT:
          size=area.width;
          area.x+=area.width;
          if(!gtk_sheet_clip_text(sheet)){          
           for(i=col-1; i>=MIN_VISIBLE_COLUMN(sheet); i--){
             if(gtk_sheet_cell_get_text(sheet, row, i)) break;
             if(size>=text_width+CELLOFFSET) break;
             size+=sheet->column[i].width;
             sheet->column[i].right_text_column = MAX(col, sheet->column[i].right_text_column);
           }
           area.width=size;
          }
          area.x-=size;
          xoffset+=area.width-text_width - 2 * CELLOFFSET -
                   attributes.border.width/2;
          break;
     case GTK_JUSTIFY_CENTER:
          sizel=area.width/2;
          sizer=area.width/2;
	  area.x+=area.width/2;
          if(!gtk_sheet_clip_text(sheet)){          
           for(i=col+1; i<=MAX_VISIBLE_COLUMN(sheet); i++){
             if(gtk_sheet_cell_get_text(sheet, row, i)) break;
             if(sizer>=text_width/2) break;
             sizer+=sheet->column[i].width;
             sheet->column[i].left_text_column = MIN(col, sheet->column[i].left_text_column);
           }
           for(i=col-1; i>=MIN_VISIBLE_COLUMN(sheet); i--){
             if(gtk_sheet_cell_get_text(sheet, row, i)) break;
             if(sizel>=text_width/2) break;
             sizel+=sheet->column[i].width;
             sheet->column[i].right_text_column = MAX(col, sheet->column[i].right_text_column);
           }
           size=MIN(sizel, sizer);
          }
	  area.x-=sizel;
          xoffset+= sizel - text_width/2 - CELLOFFSET;
	  area.width=sizel+sizer;
          break;
      case GTK_JUSTIFY_LEFT:
      default:
          size=area.width;
          if(!gtk_sheet_clip_text(sheet)){          
           for(i=col+1; i<=MAX_VISIBLE_COLUMN(sheet); i++){
             if(gtk_sheet_cell_get_text(sheet, row, i)) break;
             if(size>=text_width+CELLOFFSET) break;
             size+=sheet->column[i].width;
             sheet->column[i].left_text_column = MIN(col, sheet->column[i].left_text_column);
           }
           area.width=size;
          }
          xoffset += attributes.border.width/2;
          break;
   }

  if(!gtk_sheet_clip_text(sheet)) clip_area = area;
  gdk_gc_set_clip_rectangle(fg_gc, &clip_area);


  gdk_draw_layout (sheet->pixmap, fg_gc,
                   area.x + xoffset + CELLOFFSET,
		   y,
                   layout);

  gdk_gc_set_clip_rectangle(fg_gc, NULL);
  g_object_unref(G_OBJECT(layout));

  gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  area.x,
                  area.y,
                  area.x,
                  area.y,
                  area.width,
                  area.height);      

}



static void
gtk_sheet_range_draw(GtkSheet *sheet, const GtkSheetRange *range)
{
 gint i,j;
 GtkSheetRange drawing_range;
 GdkRectangle area;

 g_return_if_fail(sheet != NULL);
 g_return_if_fail(GTK_SHEET(sheet));
 
 if(!GTK_WIDGET_DRAWABLE(GTK_WIDGET(sheet))) return;
 if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;
 if(!GTK_WIDGET_MAPPED(GTK_WIDGET(sheet))) return;

 if(range == NULL)
 {
   drawing_range.row0=MIN_VISIBLE_ROW(sheet);
   drawing_range.col0=MIN_VISIBLE_COLUMN(sheet);
   drawing_range.rowi=MAX_VISIBLE_ROW(sheet);
   drawing_range.coli=MAX_VISIBLE_COLUMN(sheet);
/*
   gdk_draw_rectangle (sheet->pixmap,
	               GTK_WIDGET(sheet)->style->white_gc,
	               TRUE,
	               0,0,
	               sheet->sheet_window_width,sheet->sheet_window_height);
*/
 }
 else
 {
   drawing_range.row0=MAX(range->row0, MIN_VISIBLE_ROW(sheet));
   drawing_range.col0=MAX(range->col0, MIN_VISIBLE_COLUMN(sheet));
   drawing_range.rowi=MIN(range->rowi, MAX_VISIBLE_ROW(sheet));
   drawing_range.coli=MIN(range->coli, MAX_VISIBLE_COLUMN(sheet));
 }

 if(drawing_range.coli == sheet->maxcol){
  area.x=COLUMN_LEFT_XPIXEL(sheet,sheet->maxcol)+
         sheet->column[sheet->maxcol].width+1;
  area.y=0;

  gdk_gc_set_foreground(sheet->fg_gc, &sheet->bg_color);

  gdk_draw_rectangle (sheet->pixmap,
	              sheet->fg_gc,
	              TRUE,
	              area.x,area.y,
	              sheet->sheet_window_width - area.x, 
                      sheet->sheet_window_height);

  gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  area.x,
                  area.y,
                  area.x,
                  area.y,
	          sheet->sheet_window_width - area.x, 
                  sheet->sheet_window_height);                  
 }
 if(drawing_range.rowi == sheet->maxrow){
  area.x=0;
  area.y=ROW_TOP_YPIXEL(sheet,sheet->maxrow)+sheet->row[sheet->maxrow].height+1;

  gdk_gc_set_foreground(sheet->fg_gc, &sheet->bg_color);

  gdk_draw_rectangle (sheet->pixmap,
	              sheet->fg_gc,
	              TRUE,
	              area.x,area.y,
	              sheet->sheet_window_width,
                      sheet->sheet_window_height - area.y);

  gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  area.x,
                  area.y,
                  area.x,
                  area.y,
                  sheet->sheet_window_width,
                  sheet->sheet_window_height - area.y);
 }

 for(i=drawing_range.row0; i<=drawing_range.rowi; i++)
  for(j=drawing_range.col0; j<=drawing_range.coli; j++){
     gtk_sheet_cell_draw_default(sheet, i, j);
  }

 for(i=drawing_range.row0; i<=drawing_range.rowi; i++)
  for(j=drawing_range.col0; j<=drawing_range.coli; j++){
     gtk_sheet_cell_draw_border(sheet, i-1, j, GTK_SHEET_BOTTOM_BORDER);
     gtk_sheet_cell_draw_border(sheet, i+1, j, GTK_SHEET_TOP_BORDER);
     gtk_sheet_cell_draw_border(sheet, i, j-1, GTK_SHEET_RIGHT_BORDER);
     gtk_sheet_cell_draw_border(sheet, i, j+1, GTK_SHEET_LEFT_BORDER);
     gtk_sheet_cell_draw_border(sheet, i, j, 15);
  }

 for(i=drawing_range.row0; i<=drawing_range.rowi; i++)
  for(j=drawing_range.col0; j<=drawing_range.coli; j++)
     if(i<=sheet->maxallocrow && j<=sheet->maxalloccol &&
        sheet->data[i] && sheet->data[i][j])
                  gtk_sheet_cell_draw_label (sheet, i, j);
     
 for(i=drawing_range.row0; i<=drawing_range.rowi; i++)
  for(j=sheet->column[drawing_range.col0].left_text_column; j<drawing_range.col0; j++)
     if(i<=sheet->maxallocrow && j<=sheet->maxalloccol && 
        sheet->data[i] && sheet->data[i][j])
                  gtk_sheet_cell_draw_label (sheet, i, j);
    
 for(i=drawing_range.row0; i<=drawing_range.rowi; i++)
  for(j=drawing_range.coli+1; j<=sheet->column[drawing_range.coli].right_text_column; j++)
     if(i<=sheet->maxallocrow && j<=sheet->maxalloccol && 
        sheet->data[i] && sheet->data[i][j])
                  gtk_sheet_cell_draw_label (sheet, i, j); 

  gtk_sheet_draw_backing_pixmap(sheet, drawing_range);

  if(sheet->state != GTK_SHEET_NORMAL && gtk_sheet_range_isvisible(sheet, sheet->range))
       gtk_sheet_range_draw_selection(sheet, drawing_range);
  
  if(sheet->state == GTK_STATE_NORMAL && 
     sheet->active_cell.row >= drawing_range.row0 &&
     sheet->active_cell.row <= drawing_range.rowi &&
     sheet->active_cell.col >= drawing_range.col0 &&
     sheet->active_cell.col <= drawing_range.coli)    
                            gtk_sheet_show_active_cell(sheet);

}

static void
gtk_sheet_range_draw_selection(GtkSheet *sheet, GtkSheetRange range)
{
  GdkRectangle area;
  gint i,j;
  GtkSheetRange aux;

  if(range.col0 > sheet->range.coli || range.coli < sheet->range.col0 ||
     range.row0 > sheet->range.rowi || range.rowi < sheet->range.row0)
     return;

  if(!gtk_sheet_range_isvisible(sheet, range)) return;
  if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;

  aux=range;

  range.col0=MAX(sheet->range.col0, range.col0);
  range.coli=MIN(sheet->range.coli, range.coli);
  range.row0=MAX(sheet->range.row0, range.row0);
  range.rowi=MIN(sheet->range.rowi, range.rowi);

  range.col0=MAX(range.col0, MIN_VISIBLE_COLUMN(sheet));
  range.coli=MIN(range.coli, MAX_VISIBLE_COLUMN(sheet));
  range.row0=MAX(range.row0, MIN_VISIBLE_ROW(sheet));
  range.rowi=MIN(range.rowi, MAX_VISIBLE_ROW(sheet));

  for(i=range.row0; i<=range.rowi; i++){
   for(j=range.col0; j<=range.coli; j++){

    if(gtk_sheet_cell_get_state(sheet, i, j)==GTK_STATE_SELECTED && 
       sheet->column[j].is_visible && sheet->row[i].is_visible){

      row_button_set(sheet, i);
      column_button_set(sheet, j);

      area.x=COLUMN_LEFT_XPIXEL(sheet,j);
      area.y=ROW_TOP_YPIXEL(sheet,i);
      area.width=sheet->column[j].width;
      area.height=sheet->row[i].height;

      if(i==sheet->range.row0){
            area.y=area.y+2;
            area.height=area.height-2;
      }
      if(i==sheet->range.rowi) area.height=area.height-3;
      if(j==sheet->range.col0){
            area.x=area.x+2;
            area.width=area.width-2;
      }
      if(j==sheet->range.coli) area.width=area.width-3;

      if(i!=sheet->active_cell.row || j!=sheet->active_cell.col){
       gdk_draw_rectangle (sheet->sheet_window,
  	                   sheet->xor_gc,
	   	           TRUE,
	                   area.x+1,area.y+1,
	                   area.width,area.height);
      }
    }

   }
  }

  gtk_sheet_draw_border(sheet, sheet->range);

}

static void
gtk_sheet_draw_backing_pixmap(GtkSheet *sheet, GtkSheetRange range)
{
  gint x,y,width,height;

  if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;
 
  x=COLUMN_LEFT_XPIXEL(sheet,range.col0);
  y=ROW_TOP_YPIXEL(sheet, range.row0);  
  width=COLUMN_LEFT_XPIXEL(sheet, range.coli)-x+sheet->column[range.coli].width;
  height=ROW_TOP_YPIXEL(sheet, range.rowi)-y+sheet->row[range.rowi].height;

  if(range.row0==sheet->range.row0){
          y=y-5;
          height=height+5;
  }
  if(range.rowi==sheet->range.rowi) height=height+5;
  if(range.col0==sheet->range.col0){
            x=x-5;
            width=width+5;
  }
  if(range.coli==sheet->range.coli) width=width+5;

  
  width=MIN(width, sheet->sheet_window_width-x);
  height=MIN(height, sheet->sheet_window_height-y);

  x--; 
  y--;
  width+=2;
  height+=2;

  x = (sheet->row_titles_visible)
       ? MAX(x, sheet->row_title_area.width) : MAX(x, 0);
  y = (sheet->column_titles_visible)
       ? MAX(y, sheet->column_title_area.height) : MAX(y, 0);

  if(range.coli==sheet->maxcol) width=sheet->sheet_window_width-x;
  if(range.rowi==sheet->maxrow) height=sheet->sheet_window_height-y;

  gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  x,
                  y,
                  x,
                  y,
                  width+1,
                  height+1);                  
}

static GtkSheetCell *
gtk_sheet_cell_new()
{
 GtkSheetCell *cell;
 cell = g_new(GtkSheetCell, 1);
 cell->text = NULL;
 cell->link = NULL;
 cell->attributes = NULL;
 return cell;
}

/**
 * gtk_sheet_set_cell_text:
 * @sheet: a #GtkSheet.
 * @row: row_number
 * @col: column number
 * @text: cell text
 *
 * Set cell contents and allocate memory if needed.No justifcation is made. 
 */
void 
gtk_sheet_set_cell_text(GtkSheet *sheet, gint row, gint col, const gchar *text)
{
 GtkSheetCellAttr attributes;

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));
 if (col > sheet->maxcol || row > sheet->maxrow) return;
 if (col < 0 || row < 0) return;

 gtk_sheet_get_attributes(sheet, row, col, &attributes);
 gtk_sheet_set_cell(sheet, row, col, attributes.justification, text);
}

/**
 * gtk_sheet_set_cell:
 * @sheet: a #GtkSheet.
 * @row: row_number
 * @col: column number
 * @justification: a #GtkJustification :GTK_JUSTIFY_LEFT, RIGHT, CENTER
 * @text: cell text
 *
 * Set cell contents and allocate memory if needed.
 */
void 
gtk_sheet_set_cell(GtkSheet *sheet, gint row, gint col, 
                   GtkJustification justification,
                   const gchar *text)
{
 GtkSheetCell **cell;
 GtkSheetRange range;
 gint text_width;
 GtkSheetCellAttr attributes;

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));
 if (col > sheet->maxcol || row > sheet->maxrow) return;
 if (col < 0 || row < 0) return;

 CheckBounds(sheet, row, col);

 cell=&sheet->data[row][col];

 if(*cell==NULL)
  (*cell) = gtk_sheet_cell_new();

 gtk_sheet_get_attributes(sheet, row, col, &attributes);

 (*cell)->row = row;
 (*cell)->col = col;

 attributes.justification = justification;
 gtk_sheet_set_cell_attributes(sheet, row, col, attributes);

 if((*cell)->text){
    g_free((*cell)->text);
    (*cell)->text = NULL;
 }

 if(text) 
      (*cell)->text=g_strdup(text);

 if(attributes.is_visible){

   text_width = 0;
   if((*cell)->text && strlen((*cell)->text) > 0) {
     PangoFontDescription *font_desc = (attributes.font_desc == NULL) ? 
                     GTK_WIDGET(sheet)->style->font_desc : attributes.font_desc;

     text_width = STRING_WIDTH(GTK_WIDGET(sheet), font_desc, (*cell)->text);
   }

   range.row0 = row;
   range.rowi = row;
   range.col0 = sheet->view.col0;
   range.coli = sheet->view.coli;

   if(gtk_sheet_autoresize(sheet) &&
      text_width > sheet->column[col].width-2*CELLOFFSET-attributes.border.width){
      gtk_sheet_set_column_width(sheet, col, text_width+2*CELLOFFSET+attributes.border.width);
      GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_REDRAW_PENDING);
   }
   else
     if(!GTK_SHEET_IS_FROZEN(sheet))
       gtk_sheet_range_draw(sheet, &range);
 }

 g_signal_emit(sheet, sheet_signals[CHANGED], 0, row, col);

}

/**
 * gtk_sheet_cell_clear:
 * @sheet: a #GtkSheet.
 * @row: row_number
 * @column: column number
 *
 * Clear cell contents. 
 */
void
gtk_sheet_cell_clear (GtkSheet *sheet, gint row, gint column)
{
  GtkSheetRange range;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));
  if (column > sheet->maxcol || row > sheet->maxrow) return;
  if (column > sheet->maxalloccol || row > sheet->maxallocrow) return;
  if (column < 0 || row < 0) return;

  range.row0 = row;
  range.rowi = row;
  range.col0 = sheet->view.col0;
  range.coli = sheet->view.coli;

  gtk_sheet_real_cell_clear(sheet, row, column, FALSE);

  if(!GTK_SHEET_IS_FROZEN(sheet)){
     gtk_sheet_range_draw(sheet, &range);
  }
}

/**
 * gtk_sheet_cell_delete:
 * @sheet: a #GtkSheet.
 * @row: row_number
 * @column: column number
 *
 * Clear cell contents and remove links.
 */
void
gtk_sheet_cell_delete (GtkSheet *sheet, gint row, gint column)
{
  GtkSheetRange range;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));
  if (column > sheet->maxcol || row > sheet->maxrow) return;
  if (column > sheet->maxalloccol || row > sheet->maxallocrow) return;
  if (column < 0 || row < 0) return;

  range.row0 = row;
  range.rowi = row;
  range.col0 = sheet->view.col0;
  range.coli = sheet->view.coli;

  gtk_sheet_real_cell_clear(sheet, row, column, TRUE);

  if(!GTK_SHEET_IS_FROZEN(sheet)){
     gtk_sheet_range_draw(sheet, &range);
  }
}

static void
gtk_sheet_real_cell_clear (GtkSheet *sheet, gint row, gint column, gboolean delete)
{
  gchar *text;
  gpointer link;

  if(row > sheet->maxallocrow || column > sheet->maxalloccol) return;
  if(!sheet->data[row]) return;
  if(!sheet->data[row][column]) return;

  text = gtk_sheet_cell_get_text(sheet, row, column); 
  link = gtk_sheet_get_link(sheet, row, column); 

  if(text){ 
    g_free(sheet->data[row][column]->text);
    sheet->data[row][column]->text = NULL;

    if(GTK_IS_OBJECT(sheet) && G_OBJECT(sheet)->ref_count > 0)
      g_signal_emit(sheet, sheet_signals[CLEAR_CELL], 0, row, column);
  }  

  if(delete){ 
     if(sheet->data[row][column]->attributes){
         g_free(sheet->data[row][column]->attributes);
         sheet->data[row][column]->attributes = NULL;
     }
     sheet->data[row][column]->link = NULL;

     if(sheet->data[row][column]) g_free(sheet->data[row][column]);

     sheet->data[row][column] = NULL;
  }

}
 
/**
 * gtk_sheet_range_clear:
 * @sheet: a #GtkSheet.
 * @range: a #GtkSheetRange
 *
 * Clear range contents. If range==NULL the whole sheet will be cleared.
 */ 
void
gtk_sheet_range_clear (GtkSheet *sheet, const GtkSheetRange *range)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  gtk_sheet_real_range_clear(sheet, range, FALSE);
}

/**
 * gtk_sheet_range_delete:
 * @sheet: a #GtkSheet.
 * @range: a #GtkSheetRange
 *
 * Clear range contents and remove links. If range==NULL the whole sheet 
 * will be deleted.
 */ 
void
gtk_sheet_range_delete (GtkSheet *sheet, const GtkSheetRange *range)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  gtk_sheet_real_range_clear(sheet, range, TRUE);
}
 
static void
gtk_sheet_real_range_clear (GtkSheet *sheet, const GtkSheetRange *range, 
                            gboolean delete)
{
  gint i, j;
  GtkSheetRange clear;

  if(!range){
    clear.row0=0;
    clear.rowi=sheet->maxallocrow;
    clear.col0=0;
    clear.coli=sheet->maxalloccol;
  }else
    clear=*range;  

  clear.row0=MAX(clear.row0, 0);
  clear.col0=MAX(clear.col0, 0);
  clear.rowi=MIN(clear.rowi, sheet->maxallocrow);
  clear.coli=MIN(clear.coli, sheet->maxalloccol);

  for(i=clear.row0; i<=clear.rowi; i++)
    for(j=clear.col0; j<=clear.coli; j++){
        gtk_sheet_real_cell_clear(sheet, i, j, delete);
    }

  gtk_sheet_range_draw(sheet, NULL);
}

/**
 * gtk_sheet_cell_get_text:
 * @sheet: a #GtkSheet
 * @row: row number
 * @col: column number
 *
 * Get cell text.
 * Return value: cell text
 */ 
gchar *     
gtk_sheet_cell_get_text (GtkSheet *sheet, gint row, gint col)
{
  g_return_val_if_fail (sheet != NULL, NULL);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);

  if(col > sheet->maxcol || row > sheet->maxrow) return NULL;
  if(col < 0 || row < 0) return NULL;
  if(row > sheet->maxallocrow || col > sheet->maxalloccol) return NULL;
  if(!sheet->data[row]) return NULL;
  if(!sheet->data[row][col]) return NULL;
  if(!sheet->data[row][col]->text) return NULL;
  if(strlen(sheet->data[row][col]->text) == 0) return NULL;

  return (sheet->data[row][col]->text);
}

/**
 * gtk_sheet_link_cell:
 * @sheet: a #GtkSheet
 * @row: row number
 * @col: column number
 * @link: pointer linked to the cell
 *
 * Link pointer to a cell.
 */ 
void 
gtk_sheet_link_cell(GtkSheet *sheet, gint row, gint col, gpointer link)
{
 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));
 if(col > sheet->maxcol || row > sheet->maxrow) return;
 if(col < 0 || row < 0) return;

 if(row > sheet->maxallocrow || col > sheet->maxalloccol ||
    !sheet->data[row] || !sheet->data[row][col])
       gtk_sheet_set_cell_text(sheet, row, col, "");

 sheet->data[row][col]->link = link;
}

/**
 * gtk_sheet_get_link:
 * @sheet: a #GtkSheet
 * @row: row number
 * @col: column number
 *
 * Get link pointer from a cell.
 * Return value: pointer linked to the cell
 */ 
gpointer 
gtk_sheet_get_link(GtkSheet *sheet, gint row, gint col)
{
 g_return_val_if_fail (sheet != NULL, NULL);
 g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);
 if(col > sheet->maxcol || row > sheet->maxrow) return NULL;
 if(col < 0 || row < 0) return NULL;

 if (row > sheet->maxallocrow || col > sheet->maxalloccol) return NULL; 
 if (!sheet->data[row]) return NULL; /* Added by Chris Howell */ 
 if (!sheet->data[row][col]) return NULL; /* Added by Bob Lissner */ 

 return(sheet->data[row][col]->link);
}

/**
 * gtk_sheet_remove_link:
 * @sheet: a #GtkSheet
 * @row: row number
 * @col: column number
 *
 * Remove link pointer from a cell.
 */ 
void
gtk_sheet_remove_link(GtkSheet *sheet, gint row, gint col)
{
 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));
 if(col > sheet->maxcol || row > sheet->maxrow) return;
 if(col < 0 || row < 0) return;
 
 /* Fixed by Andreas Voegele */
 if(row < sheet->maxallocrow && col < sheet->maxalloccol &&
    sheet->data[row] && sheet->data[row][col] &&
    sheet->data[row][col]->link)
                            sheet->data[row][col]->link = NULL;
}

/**
 * gtk_sheet_cell_get_state:
 * @sheet: a #GtkSheet
 * @row: row number
 * @col: column number
 *
 * Get the cell state (GTK_STATE_SELECTED if it is selected or GTK_STATE_NORMAL
 * if it is not).
 * Return value: a #GtkStateType: GTK_STATE_NORMAL or GTK_STATE_SELECTED
 */ 
GtkStateType
gtk_sheet_cell_get_state (GtkSheet *sheet, gint row, gint col)
{
 gint state;
 GtkSheetRange *range;

 g_return_val_if_fail (sheet != NULL, 0);
 g_return_val_if_fail (GTK_IS_SHEET (sheet), 0);
 if(col > sheet->maxcol || row > sheet->maxrow) return 0;
 if(col < 0 || row < 0) return 0;

 state = sheet->state;
 range = &sheet->range;

 switch (state){
                case GTK_SHEET_NORMAL:
                     return GTK_STATE_NORMAL;
		     break;
		case GTK_SHEET_ROW_SELECTED:
                     if(row>=range->row0 && row<=range->rowi) 
                                        return GTK_STATE_SELECTED;
		     break;
                case GTK_SHEET_COLUMN_SELECTED:
                     if(col>=range->col0 && col<=range->coli) 
                                        return GTK_STATE_SELECTED;
		     break;
		case GTK_SHEET_RANGE_SELECTED:
                     if(row >= range->row0 && row <= range->rowi && \
                        col >= range->col0 && col <= range->coli)
                                        return GTK_STATE_SELECTED;
		     break;
 }
 return GTK_STATE_NORMAL;
}

/**
 * gtk_sheet_get_pixel_info:
 * @sheet: a #GtkSheet
 * @x: x coordinate
 * @y: y coordinate
 * @row: cell row number
 * @column: cell column number
 *
 * Get row and column correspondig to the given position in the screen.
 * Return value: TRUE(success) or FALSE(failure)
 */ 
gboolean
gtk_sheet_get_pixel_info (GtkSheet * sheet,
			  gint x,
			  gint y,
			  gint * row,
			  gint * column)
{
  gint trow, tcol;

  g_return_val_if_fail (sheet != NULL, 0);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), 0);

  /* bounds checking, return false if the user clicked 
   * on a blank area */
  trow = ROW_FROM_YPIXEL (sheet, y);
  if (trow > sheet->maxrow)
    return FALSE;

  *row = trow;

  tcol = COLUMN_FROM_XPIXEL (sheet, x);
  if (tcol > sheet->maxcol)
    return FALSE;

 *column = tcol;

  return TRUE;
}

/**
 * gtk_sheet_get_cell_area:
 * @sheet: a #GtkSheet
 * @row: row number
 * @column: column number
 * @area: a #GdkRectangle area of the cell
 *
 * Get area of a given cell.
 * Return value: TRUE(success) or FALSE(failure)
 */ 
gboolean
gtk_sheet_get_cell_area  (GtkSheet * sheet,
			  gint row,
                          gint column,
			  GdkRectangle *area)
{
  g_return_val_if_fail (sheet != NULL, 0);
  g_return_val_if_fail (GTK_IS_SHEET (sheet), 0);

  if(row > sheet->maxrow || column > sheet->maxcol) return FALSE;

  area->x = (column == -1) ? 0 : (COLUMN_LEFT_XPIXEL(sheet, column) -
                                 (sheet->row_titles_visible
                                   ? sheet->row_title_area.width
                                   : 0));
  area->y = (row == -1) ? 0 : (ROW_TOP_YPIXEL(sheet, row) -
                              (sheet->column_titles_visible
                               ? sheet->column_title_area.height
                               : 0));
  area->width= (column == -1) ? sheet->row_title_area.width
                              : sheet->column[column].width;
  area->height= (row == -1) ? sheet->column_title_area.height
                            : sheet->row[row].height;

/*
  if(row < 0 || column < 0) return FALSE;

  area->x = COLUMN_LEFT_XPIXEL(sheet, column);
  area->y = ROW_TOP_YPIXEL(sheet, row);
  if(sheet->row_titles_visible)
           area->x -= sheet->row_title_area.width;
  if(sheet->column_titles_visible)
           area->y -= sheet->column_title_area.height;

  area->width=sheet->column[column].width;
  area->height=sheet->row[row].height;  
*/
  return TRUE;
}

/**
 * gtk_sheet_set_active_cell:
 * @sheet: a #GtkSheet
 * @row: row number
 * @column: column number
 *
 * Set active cell where the entry will be displayed .
 * Return value: FALSE if current cell can't be deactivated or requested cell can't be activated 
 */ 
gboolean 
gtk_sheet_set_active_cell (GtkSheet *sheet, gint row, gint column)
{
 g_return_val_if_fail (sheet != NULL, 0);
 g_return_val_if_fail (GTK_IS_SHEET (sheet), 0);

 if(row < 0 || column < 0) return FALSE;
 if(row > sheet->maxrow || column > sheet->maxcol) return FALSE;

 if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)))
   {
       if(!gtk_sheet_deactivate_cell(sheet)) return FALSE;
   }

 sheet->active_cell.row=row;
 sheet->active_cell.col=column;
 
 if(!gtk_sheet_activate_cell(sheet, row, column)) return FALSE;
 
 return TRUE;
}

/**
 * gtk_sheet_get_active_cell:
 * @sheet: a #GtkSheet
 * @row: row number
 * @column: column number
 *
 * Store in row and col the coordinates of the active cell. 
 */ 
void
gtk_sheet_get_active_cell (GtkSheet *sheet, gint *row, gint *column)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  *row = sheet->active_cell.row;
  *column = sheet->active_cell.col;
}

static void
gtk_sheet_entry_changed(GtkWidget *widget, gpointer data)
{
 GtkSheet *sheet;
 gint row,col;
 const char *text;
 GtkJustification justification;
 GtkSheetCellAttr attributes;

 g_return_if_fail (data != NULL);
 g_return_if_fail (GTK_IS_SHEET (data));

 sheet=GTK_SHEET(data);

 if(!GTK_WIDGET_VISIBLE(widget)) return;
 if(sheet->state != GTK_STATE_NORMAL) return;

 row=sheet->active_cell.row;
 col=sheet->active_cell.col;

 if(row<0 || col<0) return;

 sheet->active_cell.row=-1;
 sheet->active_cell.col=-1;

 text=gtk_entry_get_text(GTK_ENTRY(gtk_sheet_get_entry(sheet)));

 GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IS_FROZEN);

 if(text && strlen(text)!=0){
      gtk_sheet_get_attributes(sheet, row, col, &attributes); 
      justification=attributes.justification;
      gtk_sheet_set_cell(sheet, row, col, justification, text);
 }
 else
 {
 /* Added by Matias Mutchinick */
      if(row < sheet->maxallocrow && col < sheet->maxalloccol && sheet->data[row] && sheet->data[row][col] && sheet->data[row][col]->text) {
        g_free(sheet->data[row][col]->text);
        sheet->data[row][col]->text = NULL;
      }
 }

 if(sheet->freeze_count == 0)
        GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IS_FROZEN);
 
 sheet->active_cell.row=row;;
 sheet->active_cell.col=col;

}


static gboolean 
gtk_sheet_deactivate_cell(GtkSheet *sheet)
{
 gboolean veto = TRUE;

 g_return_val_if_fail (sheet != NULL, FALSE);
 g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

 if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return FALSE;
 if(sheet->state != GTK_SHEET_NORMAL) return FALSE;

 _gtksheet_signal_emit(GTK_OBJECT(sheet),sheet_signals[DEACTIVATE], 
                                   sheet->active_cell.row,
                                   sheet->active_cell.col, &veto);

 if(!veto) return FALSE;

 g_signal_handlers_disconnect_by_func(G_OBJECT(gtk_sheet_get_entry(sheet)),
                    	              (GCallback) gtk_sheet_entry_changed,
                	                  G_OBJECT(GTK_WIDGET(sheet)));

 gtk_sheet_hide_active_cell(sheet);
 sheet->active_cell.row=-1;
 sheet->active_cell.col=-1;
 
 if(GTK_SHEET_REDRAW_PENDING(sheet)){
   GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_REDRAW_PENDING);
   gtk_sheet_range_draw(sheet, NULL);
 }

 return TRUE;
}	

static void
gtk_sheet_hide_active_cell(GtkSheet *sheet)
{
 const char *text;
 gint row,col;
 GtkJustification justification;
 GtkSheetCellAttr attributes;

 if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;

 row=sheet->active_cell.row;
 col=sheet->active_cell.col;

 if(row < 0 || col < 0) return;

 if(sheet->freeze_count == 0)
     GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IS_FROZEN);

 text=gtk_entry_get_text(GTK_ENTRY(gtk_sheet_get_entry(sheet)));

 gtk_sheet_get_attributes(sheet, row, col, &attributes); 
 justification=attributes.justification;

 if(text && strlen(text)!=0){
      gtk_sheet_set_cell(sheet, row, col, justification, text);
      g_signal_emit(sheet, sheet_signals[SET_CELL], 0, row, col);
 }
 else
 {
      gtk_sheet_cell_clear(sheet, row, col);
 }

 row=sheet->active_cell.row;
 col=sheet->active_cell.col;

 column_button_release(sheet, col);
 row_button_release(sheet, row);

 gtk_widget_unmap(sheet->sheet_entry);

 if(row != -1 && col != -1)
   gdk_draw_drawable(sheet->sheet_window,
                   GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                   sheet->pixmap,
                   COLUMN_LEFT_XPIXEL(sheet,col)-1,
                   ROW_TOP_YPIXEL(sheet,row)-1,
                   COLUMN_LEFT_XPIXEL(sheet,col)-1,
                   ROW_TOP_YPIXEL(sheet,row)-1,
                   sheet->column[col].width+4,
                   sheet->row[row].height+4);   

 gtk_widget_grab_focus(GTK_WIDGET(sheet));

 GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(sheet->sheet_entry), GTK_VISIBLE);

}

static gboolean
gtk_sheet_activate_cell(GtkSheet *sheet, gint row, gint col)
{
    gboolean veto = TRUE;

    g_return_val_if_fail (sheet != NULL, FALSE);
    g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

    if(row < 0 || col < 0) return FALSE;
    if(row > sheet->maxrow || col > sheet->maxcol) return FALSE;

    /* _gtksheet_signal_emit(GTK_OBJECT(sheet),sheet_signals[ACTIVATE], row, col, &veto);
    if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return veto;
    */

    if(!veto) return FALSE;
    if(sheet->state != GTK_SHEET_NORMAL){
        sheet->state=GTK_SHEET_NORMAL;
        g_object_notify(G_OBJECT(sheet), "state");
        gtk_sheet_real_unselect_range(sheet, NULL);
    }

    sheet->range.row0=row;
    sheet->range.col0=col;
    sheet->range.rowi=row;
    sheet->range.coli=col;
    g_object_notify(G_OBJECT(sheet), "selected-range");
    sheet->active_cell.row=row;
    sheet->active_cell.col=col;
    g_object_notify(G_OBJECT(sheet), "active-cell");
    sheet->selection_cell.row=row;
    sheet->selection_cell.col=col;
    row_button_set(sheet, row);
    column_button_set(sheet, col); 

    GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);
    gtk_sheet_show_active_cell(sheet);

    g_signal_connect(G_OBJECT(gtk_sheet_get_entry(sheet)),
        	         "changed",
                     (GCallback)gtk_sheet_entry_changed,
                     G_OBJECT(GTK_WIDGET(sheet)));

    _gtksheet_signal_emit(GTK_OBJECT(sheet),sheet_signals[ACTIVATE], row, col, &veto);

    return TRUE;
}

static void
gtk_sheet_show_active_cell(GtkSheet *sheet)
{
 GtkSheetCell *cell;
 GtkEntry *sheet_entry;
 GtkSheetCellAttr attributes;
 gchar *text = NULL;
 const gchar *old_text;
 GtkJustification justification;
 gint row, col;

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));

 row = sheet->active_cell.row;
 col = sheet->active_cell.col;

 /* Don't show the active cell, if there is no active cell: */
 if(!(row >= 0 && col >= 0)) /* e.g row or coll == -1. */
   return;
  
 if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;
 if(sheet->state != GTK_SHEET_NORMAL) return;
 if(GTK_SHEET_IN_SELECTION(sheet)) return;

 GTK_WIDGET_SET_FLAGS(GTK_WIDGET(sheet->sheet_entry), GTK_VISIBLE);

 sheet_entry = GTK_ENTRY(gtk_sheet_get_entry(sheet));

 gtk_sheet_get_attributes(sheet, row, col, &attributes); 

 justification = GTK_JUSTIFY_LEFT;

 if(gtk_sheet_justify_entry(sheet))
      justification = attributes.justification;

 if(row <= sheet->maxallocrow && col <= sheet->maxalloccol) {
   if(sheet->data[row]) {
	   if(sheet->data[row][col]) {
	     cell = sheet->data[row][col];
	     if(cell->text)
	       text = g_strdup(cell->text);
	   }
	 }
 }

 if(!text) text = g_strdup("");

 gtk_entry_set_visibility(GTK_ENTRY(sheet_entry), attributes.is_visible);

 if(gtk_sheet_locked(sheet) || !attributes.is_editable){ 
    gtk_editable_set_editable(GTK_EDITABLE(sheet_entry), FALSE);
 }else{
    gtk_editable_set_editable(GTK_EDITABLE(sheet_entry), TRUE);
 }

/*** Added by John Gotts. Mar 25, 2005 *********/
 old_text = gtk_entry_get_text(GTK_ENTRY(sheet_entry));
 if (strcmp(old_text, text) != 0) {
  if(!GTK_IS_ITEM_ENTRY(sheet_entry))
     gtk_entry_set_text(GTK_ENTRY(sheet_entry), text);
  else
     gtk_item_entry_set_text(GTK_ITEM_ENTRY(sheet_entry), text, justification);
 }

 gtk_sheet_entry_set_max_size(sheet);
 gtk_sheet_size_allocate_entry(sheet);

 gtk_widget_map(sheet->sheet_entry);
 gtk_sheet_draw_active_cell(sheet);

 gtk_widget_grab_focus(GTK_WIDGET(sheet_entry));

 g_free(text);
}

static void
gtk_sheet_draw_active_cell(GtkSheet *sheet)
{
    gint row, col;

    if(!GTK_WIDGET_DRAWABLE(GTK_WIDGET(sheet))) return;
    if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;

    row = sheet->active_cell.row;
    col = sheet->active_cell.col;
 
    if(row<0 || col<0) return;

    if(!gtk_sheet_cell_isvisible(sheet, row, col)) return;
 
    row_button_set(sheet, row);
    column_button_set(sheet, col);

    gtk_sheet_draw_backing_pixmap(sheet, sheet->range);
    gtk_sheet_draw_border(sheet, sheet->range);

}


static void
gtk_sheet_make_backing_pixmap (GtkSheet *sheet, guint width, guint height)
{
    gint pixmap_width, pixmap_height;

    if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;

    if(width == 0 && height == 0){
        width=sheet->sheet_window_width+80;
        height=sheet->sheet_window_height+80;
    }

    if (!sheet->pixmap)
    {
        /* allocate */
        sheet->pixmap = gdk_pixmap_new (sheet->sheet_window,
                                        width, height,
                                        -1);
        if(!GTK_SHEET_IS_FROZEN(sheet)) gtk_sheet_range_draw(sheet, NULL);
    }
    else
    {
        /* reallocate if sizes don't match */
        gdk_drawable_get_size (GDK_DRAWABLE(sheet->pixmap), 
                               &pixmap_width, &pixmap_height);

        if ((pixmap_width != width) || (pixmap_height != height))
        {
            g_object_unref(G_OBJECT(sheet->pixmap));
            sheet->pixmap = gdk_pixmap_new (sheet->sheet_window,
                                            width, height,
                                            -1);
            if(!GTK_SHEET_IS_FROZEN(sheet)) gtk_sheet_range_draw(sheet, NULL);
        }
    }
}

static void
gtk_sheet_new_selection(GtkSheet *sheet, GtkSheetRange *range)
{
  gint i,j, mask1, mask2;
  gint state, selected;
  gint x,y,width,height;
  GtkSheetRange new_range, aux_range;

  g_return_if_fail (sheet != NULL);

  if(range==NULL) range=&sheet->range;

  new_range=*range;

  range->row0=MIN(range->row0, sheet->range.row0);
  range->rowi=MAX(range->rowi, sheet->range.rowi);
  range->col0=MIN(range->col0, sheet->range.col0);
  range->coli=MAX(range->coli, sheet->range.coli);

  range->row0=MAX(range->row0, MIN_VISIBLE_ROW(sheet));
  range->rowi=MIN(range->rowi, MAX_VISIBLE_ROW(sheet));
  range->col0=MAX(range->col0, MIN_VISIBLE_COLUMN(sheet));
  range->coli=MIN(range->coli, MAX_VISIBLE_COLUMN(sheet));

  aux_range.row0=MAX(new_range.row0, MIN_VISIBLE_ROW(sheet));
  aux_range.rowi=MIN(new_range.rowi, MAX_VISIBLE_ROW(sheet));
  aux_range.col0=MAX(new_range.col0, MIN_VISIBLE_COLUMN(sheet));
  aux_range.coli=MIN(new_range.coli, MAX_VISIBLE_COLUMN(sheet));

  for(i=range->row0; i<=range->rowi; i++){
   for(j=range->col0; j<=range->coli; j++){     

    state=gtk_sheet_cell_get_state(sheet, i, j);
    selected=(i<=new_range.rowi && i>=new_range.row0 && 
        j<=new_range.coli && j>=new_range.col0) ? TRUE : FALSE;

    if(state==GTK_STATE_SELECTED && selected &&
       sheet->column[j].is_visible && sheet->row[i].is_visible &&
       (i==sheet->range.row0 || i==sheet->range.rowi ||
        j==sheet->range.col0 || j==sheet->range.coli ||
        i==new_range.row0 || i==new_range.rowi ||
        j==new_range.col0 || j==new_range.coli)){

       mask1 = i==sheet->range.row0 ? 1 : 0;
       mask1 = i==sheet->range.rowi ? mask1+2 : mask1;
       mask1 = j==sheet->range.col0 ? mask1+4 : mask1;
       mask1 = j==sheet->range.coli ? mask1+8 : mask1;

       mask2 = i==new_range.row0 ? 1 : 0;
       mask2 = i==new_range.rowi ? mask2+2 : mask2;
       mask2 = j==new_range.col0 ? mask2+4 : mask2;
       mask2 = j==new_range.coli ? mask2+8 : mask2;     

       if(mask1 != mask2){
         x=COLUMN_LEFT_XPIXEL(sheet,j);
         y=ROW_TOP_YPIXEL(sheet, i);  
         width=COLUMN_LEFT_XPIXEL(sheet, j)-x+sheet->column[j].width;
         height=ROW_TOP_YPIXEL(sheet, i)-y+sheet->row[i].height;

         if(i==sheet->range.row0){
            y=y-3;
            height=height+3;
         }
         if(i==sheet->range.rowi) height=height+3;
         if(j==sheet->range.col0){
            x=x-3;
            width=width+3;
         }
         if(j==sheet->range.coli) width=width+3;

         gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  x+1,
                  y+1,
                  x+1,
                  y+1,
                  width,
                  height);           

         if(i != sheet->active_cell.row || j != sheet->active_cell.col){
           x=COLUMN_LEFT_XPIXEL(sheet,j);
           y=ROW_TOP_YPIXEL(sheet, i);  
           width=COLUMN_LEFT_XPIXEL(sheet, j)-x+sheet->column[j].width;
           height=ROW_TOP_YPIXEL(sheet, i)-y+sheet->row[i].height;

           if(i==new_range.row0){
               y=y+2;
               height=height-2;
            }
            if(i==new_range.rowi) height=height-3;
            if(j==new_range.col0){
               x=x+2;
               width=width-2;
            }
            if(j==new_range.coli) width=width-3;

            gdk_draw_rectangle (sheet->sheet_window,
  	                   sheet->xor_gc,
	   	           TRUE,
	                   x+1,y+1,
	                   width,height);
          }
       }
    }
   }
  }

  for(i=range->row0; i<=range->rowi; i++){
   for(j=range->col0; j<=range->coli; j++){     

    state=gtk_sheet_cell_get_state(sheet, i, j);
    selected=(i<=new_range.rowi && i>=new_range.row0 && 
        j<=new_range.coli && j>=new_range.col0) ? TRUE : FALSE;

    if(state==GTK_STATE_SELECTED && !selected &&   
       sheet->column[j].is_visible && sheet->row[i].is_visible){

      x=COLUMN_LEFT_XPIXEL(sheet,j);
      y=ROW_TOP_YPIXEL(sheet, i);  
      width=COLUMN_LEFT_XPIXEL(sheet, j)-x+sheet->column[j].width;
      height=ROW_TOP_YPIXEL(sheet, i)-y+sheet->row[i].height;

      if(i==sheet->range.row0){
            y=y-3;
            height=height+3;
      }
      if(i==sheet->range.rowi) height=height+3;
      if(j==sheet->range.col0){
            x=x-3;
            width=width+3;
      }
      if(j==sheet->range.coli) width=width+3;

      gdk_draw_drawable(sheet->sheet_window,
                  GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                  sheet->pixmap,
                  x+1,
                  y+1,
                  x+1,
                  y+1,
                  width,
                  height);           
    }
   }
  }

  for(i=range->row0; i<=range->rowi; i++){
   for(j=range->col0; j<=range->coli; j++){     

    state=gtk_sheet_cell_get_state(sheet, i, j);
    selected=(i<=new_range.rowi && i>=new_range.row0 && 
        j<=new_range.coli && j>=new_range.col0) ? TRUE : FALSE;

    if(state!=GTK_STATE_SELECTED && selected &&
       sheet->column[j].is_visible && sheet->row[i].is_visible &&
       (i != sheet->active_cell.row || j != sheet->active_cell.col)){

      x=COLUMN_LEFT_XPIXEL(sheet,j);
      y=ROW_TOP_YPIXEL(sheet, i);  
      width=COLUMN_LEFT_XPIXEL(sheet, j)-x+sheet->column[j].width;
      height=ROW_TOP_YPIXEL(sheet, i)-y+sheet->row[i].height;

      if(i==new_range.row0){
            y=y+2;
            height=height-2;
       }
       if(i==new_range.rowi) height=height-3;
       if(j==new_range.col0){
            x=x+2;
            width=width-2;
       }
       if(j==new_range.coli) width=width-3;

       gdk_draw_rectangle (sheet->sheet_window,
  	                   sheet->xor_gc,
	   	           TRUE,
	                   x+1,y+1,
	                   width,height);
    }   

   }
  }

  for(i=aux_range.row0; i<=aux_range.rowi; i++){
   for(j=aux_range.col0; j<=aux_range.coli; j++){     

    if(sheet->column[j].is_visible && sheet->row[i].is_visible){

       state=gtk_sheet_cell_get_state(sheet, i, j);

       mask1 = i==sheet->range.row0 ? 1 : 0;
       mask1 = i==sheet->range.rowi ? mask1+2 : mask1;
       mask1 = j==sheet->range.col0 ? mask1+4 : mask1;
       mask1 = j==sheet->range.coli ? mask1+8 : mask1;

       mask2 = i==new_range.row0 ? 1 : 0;
       mask2 = i==new_range.rowi ? mask2+2 : mask2;
       mask2 = j==new_range.col0 ? mask2+4 : mask2;
       mask2 = j==new_range.coli ? mask2+8 : mask2;    
       if(mask2!=mask1 || (mask2==mask1 && state!=GTK_STATE_SELECTED)){
         x=COLUMN_LEFT_XPIXEL(sheet,j);
         y=ROW_TOP_YPIXEL(sheet, i);  
         width=sheet->column[j].width;
         height=sheet->row[i].height;
         if(mask2 & 1)
               gdk_draw_rectangle (sheet->sheet_window,
  	                           sheet->xor_gc,
	   	                   TRUE,
	                           x+1,y-1,
	                           width,3);    
         if(mask2 & 2)
               gdk_draw_rectangle (sheet->sheet_window,
  	                           sheet->xor_gc,
	   	                   TRUE,
	                           x+1,y+height-1,
	                           width,3);
         if(mask2 & 4)
               gdk_draw_rectangle (sheet->sheet_window,
  	                           sheet->xor_gc,
	   	                   TRUE,
	                           x-1,y+1,
	                           3,height);
         if(mask2 & 8)
               gdk_draw_rectangle (sheet->sheet_window,
  	                           sheet->xor_gc,
	   	                   TRUE,
	                           x+width-1,y+1,
	                           3,height);
       }         

    } 

   }
  } 


  *range=new_range;
  gtk_sheet_draw_corners(sheet, new_range);

}

static void
gtk_sheet_draw_border (GtkSheet *sheet, GtkSheetRange new_range)
{
  GtkWidget *widget;
  GdkRectangle area;
  gint i;
  gint x,y,width,height;

  widget = GTK_WIDGET(sheet);

  x=COLUMN_LEFT_XPIXEL(sheet,new_range.col0);
  y=ROW_TOP_YPIXEL(sheet,new_range.row0);
  width=COLUMN_LEFT_XPIXEL(sheet,new_range.coli)-x+ 
             sheet->column[new_range.coli].width;
  height=ROW_TOP_YPIXEL(sheet,new_range.rowi)-y+
             sheet->row[new_range.rowi].height;

  area.x=COLUMN_LEFT_XPIXEL(sheet, MIN_VISIBLE_COLUMN(sheet));
  area.y=ROW_TOP_YPIXEL(sheet, MIN_VISIBLE_ROW(sheet));
  area.width=sheet->sheet_window_width;
  area.height=sheet->sheet_window_height;

  if(x<0) {
      width=width+x;
      x=0;
  }
  if(width>area.width) width=area.width+10;
  if(y<0) {
      height=height+y;
      y=0;
  }
  if(height>area.height) height=area.height+10;

  gdk_gc_set_clip_rectangle(sheet->xor_gc, &area);

  for(i=-1; i<=1; ++i)
     gdk_draw_rectangle (sheet->sheet_window,
  	                 sheet->xor_gc,
                         FALSE,
	                 x+i,y+i,
	                 width-2*i,height-2*i);

  gdk_gc_set_clip_rectangle(sheet->xor_gc, NULL);
  
  gtk_sheet_draw_corners(sheet, new_range);

}

static void
gtk_sheet_draw_corners(GtkSheet *sheet, GtkSheetRange range)
{
  gint x,y;
  guint width = 1;

  if(gtk_sheet_cell_isvisible(sheet, range.row0, range.col0)){
       x=COLUMN_LEFT_XPIXEL(sheet,range.col0);
       y=ROW_TOP_YPIXEL(sheet,range.row0);
       gdk_draw_drawable(sheet->sheet_window,
                       GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                       sheet->pixmap,
                       x-1,
                       y-1,
                       x-1,
                       y-1,
                       3,
                       3);         
       gdk_draw_rectangle (sheet->sheet_window,
  	                   sheet->xor_gc,
                           TRUE,
	                   x-1,y-1,
	                   3,3);
  }

  if(gtk_sheet_cell_isvisible(sheet, range.row0, range.coli) ||
     sheet->state == GTK_SHEET_COLUMN_SELECTED){
       x=COLUMN_LEFT_XPIXEL(sheet,range.coli)+
         sheet->column[range.coli].width;
       y=ROW_TOP_YPIXEL(sheet,range.row0);
       width = 1;
       if(sheet->state == GTK_SHEET_COLUMN_SELECTED)
         {
             y = ROW_TOP_YPIXEL(sheet, sheet->view.row0)+3;
             width = 3;
         }
       gdk_draw_drawable(sheet->sheet_window,
                       GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                       sheet->pixmap,
                       x-width,
                       y-width,
                       x-width,
                       y-width,
                       2*width+1,
                       2*width+1);         
       gdk_draw_rectangle (sheet->sheet_window,
  	                   sheet->xor_gc,
                           TRUE,
	                   x-width+width/2,y-width+width/2,
	                   2+width,2+width);
  }

  if(gtk_sheet_cell_isvisible(sheet, range.rowi, range.col0) ||
     sheet->state == GTK_SHEET_ROW_SELECTED){
       x=COLUMN_LEFT_XPIXEL(sheet,range.col0);
       y=ROW_TOP_YPIXEL(sheet,range.rowi)+
         sheet->row[range.rowi].height;
       width = 1;
       if(sheet->state == GTK_SHEET_ROW_SELECTED) 
         {
             x = COLUMN_LEFT_XPIXEL(sheet, sheet->view.col0)+3;
             width = 3;
         }
       gdk_draw_drawable(sheet->sheet_window,
                       GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                       sheet->pixmap,
                       x-width,
                       y-width,
                       x-width,
                       y-width,
                       2*width+1,
                       2*width+1);         
       gdk_draw_rectangle (sheet->sheet_window,
  	                   sheet->xor_gc,
                           TRUE,
	                   x-width+width/2,y-width+width/2,
	                   2+width,2+width);
  }

  if(gtk_sheet_cell_isvisible(sheet, range.rowi, range.coli)){
       x=COLUMN_LEFT_XPIXEL(sheet,range.coli)+
         sheet->column[range.coli].width;
       y=ROW_TOP_YPIXEL(sheet,range.rowi)+
         sheet->row[range.rowi].height;
       width = 1;
       if(sheet->state == GTK_SHEET_RANGE_SELECTED) width = 3;
       if(sheet->state == GTK_SHEET_NORMAL) width = 3;
       gdk_draw_drawable(sheet->sheet_window,
                       GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                       sheet->pixmap,
                       x-width,
                       y-width,
                       x-width,
                       y-width,
                       2*width+1,
                       2*width+1);         
       gdk_draw_rectangle (sheet->sheet_window,
  	                   sheet->xor_gc,
                           TRUE,
	                   x-width+width/2,y-width+width/2,
	                   2+width,2+width);

  }

}


static void
gtk_sheet_real_select_range (GtkSheet * sheet,
			     GtkSheetRange * range)
{
  gint i;
  gint state;

  g_return_if_fail (sheet != NULL);

  if(range==NULL) range=&sheet->range;

  if(range->row0 < 0 || range->rowi < 0) return;
  if(range->col0 < 0 || range->coli < 0) return;

  state=sheet->state;

  if(state==GTK_SHEET_COLUMN_SELECTED || state==GTK_SHEET_RANGE_SELECTED){
   for(i=sheet->range.col0; i< range->col0; i++)
    column_button_release(sheet, i);
   for(i=range->coli+1; i<= sheet->range.coli; i++)
    column_button_release(sheet, i);
   for(i=range->col0; i<=range->coli; i++){
    column_button_set(sheet, i);
   }
  }
 
  if(state==GTK_SHEET_ROW_SELECTED || state==GTK_SHEET_RANGE_SELECTED){
   for(i=sheet->range.row0; i< range->row0; i++)
    row_button_release(sheet, i);
   for(i=range->rowi+1; i<= sheet->range.rowi; i++)
    row_button_release(sheet, i);
   for(i=range->row0; i<=range->rowi; i++){
    row_button_set(sheet, i);
   }
  }

  if(range->coli != sheet->range.coli || range->col0 != sheet->range.col0 ||
     range->rowi != sheet->range.rowi || range->row0 != sheet->range.row0)
  {
    gtk_sheet_new_selection(sheet, range);

    sheet->range.col0=range->col0;
    sheet->range.coli=range->coli;
    sheet->range.row0=range->row0;
    sheet->range.rowi=range->rowi;
    g_object_notify(G_OBJECT(sheet), "selected-range");
  }
  else
  {
	   gtk_sheet_draw_backing_pixmap(sheet, sheet->range);
           gtk_sheet_range_draw_selection(sheet, sheet->range);
  }

  g_signal_emit(sheet, sheet_signals[SELECT_RANGE], 0, range);
}

/**
 * gtk_sheet_select_range:
 * @sheet: a #GtkSheet
 * @range: a #GtkSheetRange
 *
 * Highlight the selected range and store bounds in sheet->range
 */ 
void
gtk_sheet_select_range(GtkSheet * sheet, const GtkSheetRange *range)
{
  g_return_if_fail (sheet != NULL);

  if(range==NULL) range=&sheet->range;

  if(range->row0 < 0 || range->rowi < 0) return;
  if(range->col0 < 0 || range->coli < 0) return;

  if(sheet->state != GTK_SHEET_NORMAL) 
       gtk_sheet_real_unselect_range(sheet, NULL);
  else
  {
     gboolean veto = TRUE;
     veto = gtk_sheet_deactivate_cell(sheet);
     if(!veto) return;
  }

  sheet->range.row0=range->row0;
  sheet->range.rowi=range->rowi;
  sheet->range.col0=range->col0;
  sheet->range.coli=range->coli;
  g_object_notify(G_OBJECT(sheet), "selected-range");
  sheet->active_cell.row=range->row0;
  sheet->active_cell.col=range->col0;
  g_object_notify(G_OBJECT(sheet), "active-cell");

  sheet->selection_cell.row=range->rowi;
  sheet->selection_cell.col=range->coli;

  sheet->state = GTK_SHEET_RANGE_SELECTED;
  g_object_notify(G_OBJECT(sheet), "state");
  gtk_sheet_real_select_range(sheet, NULL);

}
/**
 * gtk_sheet_unselect_range:
 * @sheet: a #GtkSheet
 *
 * Unselect the current selected range and clears the bounds in sheet->range. 
 */ 
void
gtk_sheet_unselect_range (GtkSheet * sheet)
{
  gtk_sheet_real_unselect_range(sheet, NULL);
  sheet->state = GTK_STATE_NORMAL;
  g_object_notify(G_OBJECT(sheet), "state");
  gtk_sheet_activate_cell(sheet, sheet->active_cell.row, sheet->active_cell.col);
}


static void
gtk_sheet_real_unselect_range (GtkSheet * sheet,
			       const GtkSheetRange *range)
{
  gint i;
 
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)));

  if(range==NULL){
     range=&sheet->range;
  }

  if(range->row0 < 0 || range->rowi < 0) return;
  if(range->col0 < 0 || range->coli < 0) return;

  if (gtk_sheet_range_isvisible (sheet, *range)){
    gtk_sheet_draw_backing_pixmap(sheet, *range);
  }

  for(i=range->col0; i<=range->coli; i++){
     column_button_release(sheet, i);
  }

  for(i=range->row0; i<=range->rowi; i++){
     row_button_release(sheet, i);
  }

  gtk_sheet_position_children(sheet);
}


static gint
gtk_sheet_expose (GtkWidget * widget,
		  GdkEventExpose * event)
{
  GtkSheet *sheet;
  GtkSheetRange range;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  sheet = GTK_SHEET (widget);

  if (GTK_WIDGET_DRAWABLE (widget))
  {
      range.row0=ROW_FROM_YPIXEL(sheet,event->area.y);
      range.col0=COLUMN_FROM_XPIXEL(sheet,event->area.x);
      range.rowi=ROW_FROM_YPIXEL(sheet,event->area.y+event->area.height);
      range.coli=COLUMN_FROM_XPIXEL(sheet,event->area.x+event->area.width);

      /* exposure events on the sheet */
 
      if(event->window == sheet->row_title_window && sheet->row_titles_visible){
         gint i;
         for(i = MIN_VISIBLE_ROW(sheet); i <= MAX_VISIBLE_ROW(sheet); i++)
           gtk_sheet_button_draw(sheet,i,-1);
      }

      if(event->window == sheet->column_title_window && sheet->column_titles_visible){
         gint i;
         for(i = MIN_VISIBLE_COLUMN(sheet); i <= MAX_VISIBLE_COLUMN(sheet); i++)
           gtk_sheet_button_draw(sheet,-1,i);
      }

      if (event->window == sheet->sheet_window){
        gtk_sheet_draw_backing_pixmap(sheet, range);
              
        if(sheet->state != GTK_SHEET_NORMAL){
                if(gtk_sheet_range_isvisible(sheet, sheet->range))          
                   gtk_sheet_draw_backing_pixmap(sheet, sheet->range);
                if(GTK_SHEET_IN_RESIZE(sheet) || GTK_SHEET_IN_DRAG(sheet))
                   gtk_sheet_draw_backing_pixmap(sheet, sheet->drag_range);

                if(gtk_sheet_range_isvisible(sheet, sheet->range))          
                   gtk_sheet_range_draw_selection(sheet, sheet->range);
                if(GTK_SHEET_IN_RESIZE(sheet) || GTK_SHEET_IN_DRAG(sheet))
                   draw_xor_rectangle(sheet, sheet->drag_range);
        }

        if((!GTK_SHEET_IN_XDRAG(sheet)) && (!GTK_SHEET_IN_YDRAG(sheet))){
             if(sheet->state == GTK_SHEET_NORMAL){ 
                 gtk_sheet_draw_active_cell(sheet);
                 if(!GTK_SHEET_IN_SELECTION(sheet))
                         gtk_widget_queue_draw(sheet->sheet_entry);
             }
        }


      }

  }

  if(sheet->state != GTK_SHEET_NORMAL && GTK_SHEET_IN_SELECTION(sheet))
     gtk_widget_grab_focus(GTK_WIDGET(sheet));

  (* GTK_WIDGET_CLASS (gtk_sheet_parent_class)->expose_event) (widget, event);

  return FALSE;
}


static gint
gtk_sheet_button_press (GtkWidget * widget,
			GdkEventButton * event)
{
  GtkSheet *sheet;
  GdkModifierType mods;
  gint x, y, row, column;
  gboolean veto;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

/*
  if(event->type != GDK_BUTTON_PRESS) return TRUE;
*/
  gdk_window_get_pointer(widget->window, NULL, NULL, &mods);
  if(!(mods & GDK_BUTTON1_MASK)) return TRUE;

  sheet = GTK_SHEET (widget);

  /* press on resize windows */
  if (event->window == sheet->column_title_window &&
      gtk_sheet_columns_resizable(sheet))
      {
	gtk_widget_get_pointer (widget, &sheet->x_drag, NULL);
        if(POSSIBLE_XDRAG(sheet, sheet->x_drag, &sheet->drag_cell.col)){
          guint req;
          if (event->type == GDK_2BUTTON_PRESS){
            gtk_sheet_autoresize_column (sheet, sheet->drag_cell.col);
            GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IN_XDRAG);
            return TRUE;
          }
          gtk_sheet_column_size_request(sheet, sheet->drag_cell.col, &req);
	  GTK_SHEET_SET_FLAGS (sheet, GTK_SHEET_IN_XDRAG);
	  gdk_pointer_grab (sheet->column_title_window, FALSE,
			    GDK_POINTER_MOTION_HINT_MASK |
			    GDK_BUTTON1_MOTION_MASK |
			    GDK_BUTTON_RELEASE_MASK,
			    NULL, NULL, event->time);

	  draw_xor_vline (sheet);
	  return TRUE;
        }
      }

  if (event->window == sheet->row_title_window && gtk_sheet_rows_resizable(sheet))
      {
	gtk_widget_get_pointer (widget, NULL, &sheet->y_drag);

        if(POSSIBLE_YDRAG(sheet, sheet->y_drag, &sheet->drag_cell.row)){
          guint req;
          gtk_sheet_row_size_request(sheet, sheet->drag_cell.row, &req);
	  GTK_SHEET_SET_FLAGS (sheet, GTK_SHEET_IN_YDRAG);
	  gdk_pointer_grab (sheet->row_title_window, FALSE,
			    GDK_POINTER_MOTION_HINT_MASK |
			    GDK_BUTTON1_MOTION_MASK |
			    GDK_BUTTON_RELEASE_MASK,
			    NULL, NULL, event->time);

	  draw_xor_hline (sheet);
	  return TRUE;
        }
      }

  /* the sheet itself does not handle other than single click events */
  if(event->type != GDK_BUTTON_PRESS) return FALSE;

  /* selections on the sheet */
    if(event->window == sheet->sheet_window){
     gtk_widget_get_pointer (widget, &x, &y);
     gtk_sheet_get_pixel_info (sheet, x, y, &row, &column);
     gdk_pointer_grab (sheet->sheet_window, FALSE,
		       GDK_POINTER_MOTION_HINT_MASK |
		       GDK_BUTTON1_MOTION_MASK |
		       GDK_BUTTON_RELEASE_MASK,
		       NULL, NULL, event->time);
     gtk_grab_add(GTK_WIDGET(sheet));
     sheet->timer=gtk_timeout_add(TIMEOUT_SCROLL, gtk_sheet_scroll, sheet); 
     gtk_widget_grab_focus(GTK_WIDGET(sheet));

     if(sheet->selection_mode != GTK_SELECTION_SINGLE &&
        sheet->cursor_drag->type==GDK_SIZING &&
        !GTK_SHEET_IN_SELECTION(sheet) && !GTK_SHEET_IN_RESIZE(sheet)){
        if(sheet->state==GTK_STATE_NORMAL) {
          row=sheet->active_cell.row;
          column=sheet->active_cell.col;
          if(!gtk_sheet_deactivate_cell(sheet)) return FALSE;
          sheet->active_cell.row=row;
          sheet->active_cell.col=column;
          sheet->drag_range=sheet->range;
          sheet->state=GTK_SHEET_RANGE_SELECTED;
          g_object_notify(G_OBJECT(sheet), "state");
          gtk_sheet_select_range(sheet, &sheet->drag_range);
        }
        sheet->x_drag=x;
        sheet->y_drag=y;
        if(row > sheet->range.rowi) row--;
        if(column > sheet->range.coli) column--;
        sheet->drag_cell.row = row;
        sheet->drag_cell.col = column;
        sheet->drag_range=sheet->range;
        draw_xor_rectangle(sheet, sheet->drag_range);
        GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IN_RESIZE);
     }
     else if(sheet->cursor_drag->type==GDK_TOP_LEFT_ARROW &&
            !GTK_SHEET_IN_SELECTION(sheet) && !GTK_SHEET_IN_DRAG(sheet)) {
            if(sheet->state==GTK_STATE_NORMAL) {
              row=sheet->active_cell.row;
              column=sheet->active_cell.col;
              if(!gtk_sheet_deactivate_cell(sheet)) return FALSE;
              sheet->active_cell.row=row;
              sheet->active_cell.col=column;
              sheet->drag_range=sheet->range;
              sheet->state=GTK_SHEET_RANGE_SELECTED;
              g_object_notify(G_OBJECT(sheet), "state");
              gtk_sheet_select_range(sheet, &sheet->drag_range);
            }
            sheet->x_drag=x;
            sheet->y_drag=y;
            if(row < sheet->range.row0) row++;
            if(row > sheet->range.rowi) row--;
            if(column < sheet->range.col0) column++;
            if(column > sheet->range.coli) column--;
            sheet->drag_cell.row=row;
            sheet->drag_cell.col=column;
            sheet->drag_range=sheet->range;
            draw_xor_rectangle(sheet, sheet->drag_range);
            GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IN_DRAG);
          }
          else 
          {
           gtk_sheet_click_cell(sheet, row, column, &veto);
           if(veto) GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);
          }

    }

    if(event->window == sheet->column_title_window){
     gtk_widget_get_pointer (widget, &x, &y);
     column = COLUMN_FROM_XPIXEL(sheet, x);
     if(sheet->column[column].is_sensitive){;
       gtk_sheet_click_cell(sheet, -1, column, &veto);
       gtk_grab_add(GTK_WIDGET(sheet));
       sheet->timer=gtk_timeout_add(TIMEOUT_SCROLL, gtk_sheet_scroll, sheet); 
       gtk_widget_grab_focus(GTK_WIDGET(sheet));
       GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);
     }
    }

    if(event->window == sheet->row_title_window){
     gtk_widget_get_pointer (widget, &x, &y);
     row = ROW_FROM_YPIXEL(sheet, y);
     if(sheet->row[row].is_sensitive){
       gtk_sheet_click_cell(sheet, row, -1, &veto);
       gtk_grab_add(GTK_WIDGET(sheet));
       sheet->timer=gtk_timeout_add(TIMEOUT_SCROLL, gtk_sheet_scroll, sheet); 
       gtk_widget_grab_focus(GTK_WIDGET(sheet));
       GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);
     }
    }

    return TRUE;
}

static gint
gtk_sheet_scroll(gpointer data)
{
 GtkSheet *sheet;
 gint x,y,row,column;
 gint move;
  
 sheet=GTK_SHEET(data);

 GDK_THREADS_ENTER();

 gtk_widget_get_pointer (GTK_WIDGET(sheet), &x, &y);
 gtk_sheet_get_pixel_info (sheet, x, y, &row, &column);

 move=TRUE;

 if(GTK_SHEET_IN_SELECTION(sheet))
      gtk_sheet_extend_selection(sheet, row, column);

 if(GTK_SHEET_IN_DRAG(sheet) || GTK_SHEET_IN_RESIZE(sheet)){
       move=gtk_sheet_move_query(sheet, row, column);
       if(move) draw_xor_rectangle(sheet, sheet->drag_range);      
 }       

 GDK_THREADS_LEAVE();

 return TRUE;
      
}

static void
gtk_sheet_click_cell(GtkSheet *sheet, gint row, gint column, gboolean *veto)
{
      *veto = TRUE;

      if(row > sheet->maxrow || column > sheet->maxcol){
          *veto = FALSE;
          return;
      }

      if(column >= 0 && row >= 0)
       if(!sheet->column[column].is_visible || !sheet->row[row].is_visible) 
         {
           *veto = FALSE;
           return;
         }

      _gtksheet_signal_emit(GTK_OBJECT(sheet), sheet_signals[TRAVERSE],
                            sheet->active_cell.row, sheet->active_cell.col, 
                            &row, &column, veto);

      if(!*veto){
           if(sheet->state == GTK_STATE_NORMAL) return;

           row = sheet->active_cell.row;
           column = sheet->active_cell.col;
           gtk_sheet_activate_cell(sheet, row, column);
           return;
      }

      if(row == -1 && column >= 0){
          if(gtk_sheet_autoscroll(sheet))
            gtk_sheet_move_query(sheet, row, column);
	  gtk_sheet_select_column(sheet, column);
          return;
      }
      if(column == -1 && row >= 0){
          if(gtk_sheet_autoscroll(sheet))
            gtk_sheet_move_query(sheet, row, column);
          gtk_sheet_select_row(sheet, row);
          return;
      }

      if(row==-1 && column ==-1){
        sheet->range.row0=0; /*gtk_sheet_select_range will gobject_notify this*/
        sheet->range.col0=0;
        sheet->range.rowi=sheet->maxrow;
        sheet->range.coli=sheet->maxcol;
        sheet->active_cell.row=0; /*gtk_sheet_activate_cell will gobject_notify*/
        sheet->active_cell.col=0;
        gtk_sheet_select_range(sheet, NULL);
        return;
      }

      if (row!=-1 && column !=-1) {
          if(sheet->state != GTK_SHEET_NORMAL){
            sheet->state = GTK_SHEET_NORMAL;
            g_object_notify(G_OBJECT(sheet), "state");
            gtk_sheet_real_unselect_range(sheet, NULL);
          }
          else
          {
            if(!gtk_sheet_deactivate_cell(sheet)){
              *veto = FALSE;
              return;
            }
          }

          if(gtk_sheet_autoscroll(sheet))
            gtk_sheet_move_query(sheet, row, column);
          sheet->active_cell.row=row;
          sheet->active_cell.col=column;
	      sheet->selection_cell.row=row;
          sheet->selection_cell.col=column;
          sheet->range.row0=row;
          sheet->range.col0=column;
          sheet->range.rowi=row;
          sheet->range.coli=column;
          /*g_object_notify(G_OBJECT(sheet), "selected-range");*/

	      sheet->state=GTK_SHEET_NORMAL;
          g_object_notify(G_OBJECT(sheet), "state");
          GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);
	      gtk_sheet_draw_active_cell(sheet);
	      return;
      }

      gtk_sheet_activate_cell(sheet, sheet->active_cell.row,
                                     sheet->active_cell.col);
}

static gint
gtk_sheet_button_release (GtkWidget * widget,
			GdkEventButton * event)
{
  GtkSheet *sheet;
  gint x,y;
 
  sheet=GTK_SHEET(widget);

  /* release on resize windows */
  if (GTK_SHEET_IN_XDRAG (sheet)){
    GTK_SHEET_UNSET_FLAGS (sheet, GTK_SHEET_IN_XDRAG);
    GTK_SHEET_UNSET_FLAGS (sheet, GTK_SHEET_IN_SELECTION);
    gtk_widget_get_pointer (widget, &x, NULL);
    gdk_pointer_ungrab (event->time);
    draw_xor_vline (sheet);

    gtk_sheet_set_column_width (sheet, sheet->drag_cell.col, new_column_width (sheet, sheet->drag_cell.col, &x));
    sheet->old_hadjustment = -1.;
    g_signal_emit_by_name (GTK_OBJECT (sheet->hadjustment), "value_changed");
    return TRUE;
  }

  if (GTK_SHEET_IN_YDRAG (sheet)){
    GTK_SHEET_UNSET_FLAGS (sheet, GTK_SHEET_IN_YDRAG);
    GTK_SHEET_UNSET_FLAGS (sheet, GTK_SHEET_IN_SELECTION);
    gtk_widget_get_pointer (widget, NULL, &y);
    gdk_pointer_ungrab (event->time);
    draw_xor_hline (sheet);

    gtk_sheet_set_row_height (sheet, sheet->drag_cell.row, new_row_height (sheet, sheet->drag_cell.row, &y));
    sheet->old_vadjustment = -1.;
    g_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), "value_changed");
    return TRUE;
  }

  
  if (GTK_SHEET_IN_DRAG(sheet)){
      GtkSheetRange old_range;
      draw_xor_rectangle(sheet, sheet->drag_range);
      GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IN_DRAG);
      gdk_pointer_ungrab (event->time);

      gtk_sheet_real_unselect_range(sheet, NULL);
      
      sheet->active_cell.row = sheet->active_cell.row +
                               (sheet->drag_range.row0 - sheet->range.row0);
      sheet->active_cell.col = sheet->active_cell.col +
                               (sheet->drag_range.col0 - sheet->range.col0);
      sheet->selection_cell.row = sheet->selection_cell.row +
                                  (sheet->drag_range.row0 - sheet->range.row0);
      sheet->selection_cell.col = sheet->selection_cell.col +
                                  (sheet->drag_range.col0 - sheet->range.col0);
      old_range=sheet->range;
      sheet->range=sheet->drag_range; /*gobject_notify in gtk_sheet_select_range*/
      sheet->drag_range=old_range;
      g_signal_emit (sheet, sheet_signals[MOVE_RANGE], 0,
                     &sheet->drag_range, &sheet->range);
      gtk_sheet_select_range(sheet, &sheet->range);
  }

  if (GTK_SHEET_IN_RESIZE(sheet)){
      GtkSheetRange old_range;
      draw_xor_rectangle(sheet, sheet->drag_range);
      GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IN_RESIZE);
      gdk_pointer_ungrab (event->time);

      gtk_sheet_real_unselect_range(sheet, NULL);
      
      sheet->active_cell.row = sheet->active_cell.row +
                               (sheet->drag_range.row0 - sheet->range.row0);
      sheet->active_cell.col = sheet->active_cell.col +
                               (sheet->drag_range.col0 - sheet->range.col0);
      if(sheet->drag_range.row0 < sheet->range.row0)
                     sheet->selection_cell.row = sheet->drag_range.row0;
      if(sheet->drag_range.rowi >= sheet->range.rowi)
                     sheet->selection_cell.row = sheet->drag_range.rowi;
      if(sheet->drag_range.col0 < sheet->range.col0)
                     sheet->selection_cell.col = sheet->drag_range.col0;
      if(sheet->drag_range.coli >= sheet->range.coli)
                     sheet->selection_cell.col = sheet->drag_range.coli;
      old_range = sheet->range;
      sheet->range = sheet->drag_range;
      sheet->drag_range = old_range;

      if(sheet->state==GTK_STATE_NORMAL) {
          sheet->state=GTK_SHEET_RANGE_SELECTED;
          g_object_notify(G_OBJECT(sheet), "state");
      }
      g_signal_emit (sheet, sheet_signals[RESIZE_RANGE], 0,
                     &sheet->drag_range, &sheet->range);
      gtk_sheet_select_range(sheet, &sheet->range);
  }

  if(sheet->state == GTK_SHEET_NORMAL && GTK_SHEET_IN_SELECTION(sheet)){
      GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);
      gdk_pointer_ungrab (event->time);
      gtk_sheet_activate_cell(sheet, sheet->active_cell.row, 
                                     sheet->active_cell.col);
  }

  if(GTK_SHEET_IN_SELECTION)
         gdk_pointer_ungrab (event->time);
  if(sheet->timer)
         gtk_timeout_remove(sheet->timer);
  gtk_grab_remove(GTK_WIDGET(sheet));

  GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);

  return TRUE;
}

static gint
gtk_sheet_motion (GtkWidget * widget,
		  GdkEventMotion * event)
{
  GtkSheet *sheet;
  GdkModifierType mods;
  GdkCursorType new_cursor;
  gint x, y, row, column;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_SHEET (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);


  sheet = GTK_SHEET (widget);

  /* selections on the sheet */
  x = event->x;
  y = event->y;

  if(event->window == sheet->column_title_window && gtk_sheet_columns_resizable(sheet)){
    gtk_widget_get_pointer(widget, &x, &y);
    if(!GTK_SHEET_IN_SELECTION(sheet) && POSSIBLE_XDRAG(sheet, x, &column)){
      new_cursor=GDK_SB_H_DOUBLE_ARROW;
      if(new_cursor != sheet->cursor_drag->type){
        gdk_cursor_unref(sheet->cursor_drag);
        sheet->cursor_drag=gdk_cursor_new(GDK_SB_H_DOUBLE_ARROW);
        gdk_window_set_cursor(sheet->column_title_window,sheet->cursor_drag);
      }
    }else{
      new_cursor=GDK_TOP_LEFT_ARROW;
      if(!GTK_SHEET_IN_XDRAG(sheet) && new_cursor != sheet->cursor_drag->type){
        gdk_cursor_unref(sheet->cursor_drag);
        sheet->cursor_drag=gdk_cursor_new(GDK_TOP_LEFT_ARROW);
        gdk_window_set_cursor(sheet->column_title_window,sheet->cursor_drag);
      }
    }
  }      

  if(event->window == sheet->row_title_window && gtk_sheet_rows_resizable(sheet)){
    gtk_widget_get_pointer(widget, &x, &y);
    if(!GTK_SHEET_IN_SELECTION(sheet) && POSSIBLE_YDRAG(sheet,y, &column)){
      new_cursor=GDK_SB_V_DOUBLE_ARROW;
      if(new_cursor != sheet->cursor_drag->type){
        gdk_cursor_unref(sheet->cursor_drag);
        sheet->cursor_drag=gdk_cursor_new(GDK_SB_V_DOUBLE_ARROW);
        gdk_window_set_cursor(sheet->row_title_window,sheet->cursor_drag);
      }
    }else{
      new_cursor=GDK_TOP_LEFT_ARROW;
      if(!GTK_SHEET_IN_YDRAG(sheet) && new_cursor != sheet->cursor_drag->type){
        gdk_cursor_unref(sheet->cursor_drag);
        sheet->cursor_drag=gdk_cursor_new(GDK_TOP_LEFT_ARROW);
        gdk_window_set_cursor(sheet->row_title_window,sheet->cursor_drag);
      }
    }
  }      

  new_cursor=GDK_PLUS;
  if(!POSSIBLE_DRAG(sheet,x,y,&row,&column) && !GTK_SHEET_IN_DRAG(sheet) &&
     !POSSIBLE_RESIZE(sheet,x,y,&row,&column) && !GTK_SHEET_IN_RESIZE(sheet) &&
     event->window == sheet->sheet_window && 
     new_cursor != sheet->cursor_drag->type){
         gdk_cursor_unref(sheet->cursor_drag);
         sheet->cursor_drag=gdk_cursor_new(GDK_PLUS);
         gdk_window_set_cursor(sheet->sheet_window,sheet->cursor_drag);
  }

  new_cursor=GDK_TOP_LEFT_ARROW;
  if(!(POSSIBLE_RESIZE(sheet,x,y,&row,&column) || GTK_SHEET_IN_RESIZE(sheet)) &&
     (POSSIBLE_DRAG(sheet, x,y,&row,&column) || GTK_SHEET_IN_DRAG(sheet)) && 
     event->window == sheet->sheet_window && 
     new_cursor != sheet->cursor_drag->type){
         gdk_cursor_unref(sheet->cursor_drag);
         sheet->cursor_drag=gdk_cursor_new(GDK_TOP_LEFT_ARROW);
         gdk_window_set_cursor(sheet->sheet_window,sheet->cursor_drag);
  }

  new_cursor=GDK_SIZING;
  if(!GTK_SHEET_IN_DRAG(sheet) &&
     (POSSIBLE_RESIZE(sheet,x,y,&row,&column) || GTK_SHEET_IN_RESIZE(sheet)) &&
     event->window == sheet->sheet_window && 
     new_cursor != sheet->cursor_drag->type){
         gdk_cursor_unref(sheet->cursor_drag);
         sheet->cursor_drag=gdk_cursor_new(GDK_SIZING);
         gdk_window_set_cursor(sheet->sheet_window,sheet->cursor_drag);
  }

  gdk_window_get_pointer (widget->window, &x, &y, &mods);
  if(!(mods & GDK_BUTTON1_MASK)) return FALSE;

  if (GTK_SHEET_IN_XDRAG (sheet)){
	if (event->is_hint || event->window != widget->window)
	    gtk_widget_get_pointer (widget, &x, NULL);
	  else
	    x = event->x;

	  new_column_width (sheet, sheet->drag_cell.col, &x);
	  if (x != sheet->x_drag)
	    {
	      draw_xor_vline (sheet);
	      sheet->x_drag = x;
	      draw_xor_vline (sheet);
	    }
          return TRUE;
  }

  if (GTK_SHEET_IN_YDRAG (sheet)){
	  if (event->is_hint || event->window != widget->window)
	    gtk_widget_get_pointer (widget, NULL, &y);
	  else
	    y = event->y;

	  new_row_height (sheet, sheet->drag_cell.row, &y);
	  if (y != sheet->y_drag)
	    {
	      draw_xor_hline (sheet);
	      sheet->y_drag = y;
	      draw_xor_hline (sheet);
	    }
          return TRUE;
  }

  if (GTK_SHEET_IN_DRAG(sheet)){
       GtkSheetRange aux;
       column=COLUMN_FROM_XPIXEL(sheet,x)-sheet->drag_cell.col;
       row=ROW_FROM_YPIXEL(sheet,y)-sheet->drag_cell.row;
       if(sheet->state==GTK_SHEET_COLUMN_SELECTED) row=0;
       if(sheet->state==GTK_SHEET_ROW_SELECTED) column=0;
       sheet->x_drag=x;
       sheet->y_drag=y;
       aux=sheet->range;
       if(aux.row0+row >= 0 && aux.rowi+row <= sheet->maxrow &&
          aux.col0+column >= 0 && aux.coli+column <= sheet->maxcol){
             aux=sheet->drag_range;
             sheet->drag_range.row0=sheet->range.row0+row;
             sheet->drag_range.col0=sheet->range.col0+column;
             sheet->drag_range.rowi=sheet->range.rowi+row;
             sheet->drag_range.coli=sheet->range.coli+column;
             if(aux.row0 != sheet->drag_range.row0 ||
                aux.col0 != sheet->drag_range.col0){
                draw_xor_rectangle (sheet, aux);
                draw_xor_rectangle (sheet, sheet->drag_range);
             }
       }
       return TRUE;
  }

  if (GTK_SHEET_IN_RESIZE(sheet)){
       GtkSheetRange aux;
       gint v_h, current_col, current_row, col_threshold, row_threshold;
       v_h=1;

       if(abs(x-COLUMN_LEFT_XPIXEL(sheet,sheet->drag_cell.col)) >
          abs(y-ROW_TOP_YPIXEL(sheet,sheet->drag_cell.row))) v_h=2;
        
       current_col = COLUMN_FROM_XPIXEL(sheet,x);
       current_row = ROW_FROM_YPIXEL(sheet,y);
       column = current_col-sheet->drag_cell.col;
       row    = current_row-sheet->drag_cell.row;

       /*use half of column width resp. row height as threshold to expand selection*/
       col_threshold = COLUMN_LEFT_XPIXEL(sheet,current_col)+gtk_sheet_column_width (sheet,current_col)/2;
       if (column > 0){
         if (x < col_threshold)
           column-=1;
       }
       else if (column < 0){
         if (x > col_threshold) 
	   column+=1;
       }
       row_threshold = ROW_TOP_YPIXEL(sheet,current_row)+gtk_sheet_row_height (sheet, current_row)/2;
       if (row > 0){
         if(y < row_threshold)
           row-=1;
       }
       else if (row < 0){
         if(y > row_threshold)
           row+=1;       
       }

       if(sheet->state==GTK_SHEET_COLUMN_SELECTED) row=0;
       if(sheet->state==GTK_SHEET_ROW_SELECTED) column=0;
       sheet->x_drag=x;
       sheet->y_drag=y;
       aux=sheet->range;

       if(v_h==1) 
           column=0;
       else
           row=0;

       if(aux.row0+row >= 0 && aux.rowi+row <= sheet->maxrow &&
          aux.col0+column >= 0 && aux.coli+column <= sheet->maxcol){

             aux=sheet->drag_range;
             sheet->drag_range=sheet->range;

             if(row<0) sheet->drag_range.row0=sheet->range.row0+row;
             if(row>0) sheet->drag_range.rowi=sheet->range.rowi+row;
             if(column<0) sheet->drag_range.col0=sheet->range.col0+column;
             if(column>0) sheet->drag_range.coli=sheet->range.coli+column;
             
             if(aux.row0 != sheet->drag_range.row0 ||
                aux.rowi != sheet->drag_range.rowi ||
                aux.col0 != sheet->drag_range.col0 ||
                aux.coli != sheet->drag_range.coli){
                     draw_xor_rectangle (sheet, aux);
                     draw_xor_rectangle (sheet, sheet->drag_range);
             }
       }
       return TRUE;
  }

  

  gtk_sheet_get_pixel_info (sheet, x, y, &row, &column);

  if(sheet->state==GTK_SHEET_NORMAL && row==sheet->active_cell.row &&
     column==sheet->active_cell.col) return TRUE;

  if(GTK_SHEET_IN_SELECTION(sheet) && mods&GDK_BUTTON1_MASK)
                          gtk_sheet_extend_selection(sheet, row, column);

  return TRUE;
}

static gint
gtk_sheet_move_query(GtkSheet *sheet, gint row, gint column)
{
  gint row_move, column_move;
  gfloat row_align, col_align;
  guint height, width;
  gint new_row = row;
  gint new_col = column;

  row_move=FALSE;
  column_move=FALSE;
  row_align=-1.;
  col_align=-1.;

  height = sheet->sheet_window_height;
  width = sheet->sheet_window_width;

  if(row>=MAX_VISIBLE_ROW(sheet) && sheet->state!=GTK_SHEET_COLUMN_SELECTED) {
          row_align = 1.;
	  new_row = MIN(sheet->maxrow, row + 1);
          row_move = TRUE;
          if(MAX_VISIBLE_ROW(sheet) == sheet->maxrow &&
             ROW_TOP_YPIXEL(sheet, sheet->maxrow) + 
             sheet->row[sheet->maxrow].height < height){
                 row_move = FALSE;
		 row_align = -1.;
          }
  }
  if(row<MIN_VISIBLE_ROW(sheet) && sheet->state!=GTK_SHEET_COLUMN_SELECTED) {
          row_align= 0.;
          row_move = TRUE;
  }
  if(column>=MAX_VISIBLE_COLUMN(sheet) && sheet->state!=GTK_SHEET_ROW_SELECTED) {
          col_align = 1.;
          new_col = MIN(sheet->maxcol, column + 1);
          column_move = TRUE;
          if(MAX_VISIBLE_COLUMN(sheet) == sheet->maxcol &&
             COLUMN_LEFT_XPIXEL(sheet, sheet->maxcol) + 
             sheet->column[sheet->maxcol].width < width){
                 column_move = FALSE;
		 col_align = -1.;
          }
  } 
  if(column<MIN_VISIBLE_COLUMN(sheet) && sheet->state!=GTK_SHEET_ROW_SELECTED) {
	  col_align = 0.;
          column_move = TRUE;
  }

  if(row_move || column_move){
        gtk_sheet_moveto(sheet, new_row, new_col, row_align, col_align);
  }

  return(row_move || column_move);
}

static void
gtk_sheet_extend_selection(GtkSheet *sheet, gint row, gint column)
{
   GtkSheetRange range;
   gint state;
   gint r,c;

   if(row == sheet->selection_cell.row && column == sheet->selection_cell.col)
        return;

   if(sheet->selection_mode == GTK_SELECTION_SINGLE) return;

   gtk_sheet_move_query(sheet, row, column);
   gtk_widget_grab_focus(GTK_WIDGET(sheet));

   if(GTK_SHEET_IN_DRAG(sheet)) return;

   state=sheet->state;

   switch(sheet->state){
    case GTK_SHEET_ROW_SELECTED:
	     column = sheet->maxcol;
         break;
    case GTK_SHEET_COLUMN_SELECTED:
	     row = sheet->maxrow;
         break; 
    case GTK_SHEET_NORMAL:
	     sheet->state=GTK_SHEET_RANGE_SELECTED;
         g_object_notify(G_OBJECT(sheet), "state");
         r=sheet->active_cell.row;
         c=sheet->active_cell.col;
         sheet->range.col0=c;
         sheet->range.row0=r;
         sheet->range.coli=c;
         sheet->range.rowi=r;
         gdk_draw_drawable(sheet->sheet_window,
                   GTK_WIDGET(sheet)->style->fg_gc[GTK_STATE_NORMAL],
                   sheet->pixmap,
                   COLUMN_LEFT_XPIXEL(sheet,c)-1,
                   ROW_TOP_YPIXEL(sheet,r)-1,
                   COLUMN_LEFT_XPIXEL(sheet,c)-1,
                   ROW_TOP_YPIXEL(sheet,r)-1,
                   sheet->column[c].width+4,
                   sheet->row[r].height+4);   
         gtk_sheet_range_draw_selection(sheet, sheet->range);
    case GTK_SHEET_RANGE_SELECTED:
         sheet->state=GTK_SHEET_RANGE_SELECTED;
         g_object_notify(G_OBJECT(sheet), "state");
   }

   sheet->selection_cell.row = row;
   sheet->selection_cell.col = column;

   range.col0=MIN(column,sheet->active_cell.col);
   range.coli=MAX(column,sheet->active_cell.col);
   range.row0=MIN(row,sheet->active_cell.row);
   range.rowi=MAX(row,sheet->active_cell.row);

   if(range.row0 != sheet->range.row0 || range.rowi != sheet->range.rowi ||
      range.col0 != sheet->range.col0 || range.coli != sheet->range.coli ||
      state==GTK_SHEET_NORMAL)
               gtk_sheet_real_select_range(sheet, &range);

}

static gint
gtk_sheet_entry_key_press(GtkWidget *widget,
		          GdkEventKey *key)
{
  gboolean focus;
  g_signal_emit_by_name(GTK_OBJECT(widget), "key_press_event", key, &focus);
  return focus;
}

static gint
gtk_sheet_key_press(GtkWidget *widget,
		    GdkEventKey *key)
{
  GtkSheet *sheet;
  gint row, col;
  gint state;
  gboolean extend_selection = FALSE;
  gboolean force_move = FALSE;
  gboolean in_selection = FALSE;
  gboolean veto = TRUE;
  gint scroll = 1;

  sheet = GTK_SHEET(widget);

  if(key->state & GDK_CONTROL_MASK || key->keyval==GDK_Control_L ||
     key->keyval==GDK_Control_R) return FALSE;

/*
  {
    if(key->keyval=='c' || key->keyval == 'C' && sheet->state != GTK_STATE_NORMAL)
            gtk_sheet_clip_range(sheet, sheet->range);
    if(key->keyval=='x' || key->keyval == 'X')
            gtk_sheet_unclip_range(sheet);    
    return FALSE;
  }
*/

  extend_selection = (key->state & GDK_SHIFT_MASK) || key->keyval==GDK_Shift_L 
|| key->keyval==GDK_Shift_R;

  state=sheet->state;
  in_selection = GTK_SHEET_IN_SELECTION(sheet);
  GTK_SHEET_UNSET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);

  switch(key->keyval){
    case GDK_Return: case GDK_KP_Enter:
      if(sheet->state == GTK_SHEET_NORMAL && 
         !GTK_SHEET_IN_SELECTION(sheet))
         g_signal_stop_emission_by_name(GTK_OBJECT(gtk_sheet_get_entry(sheet)), 
                                        "key_press_event");
      row = sheet->active_cell.row;
      col = sheet->active_cell.col;
      if(sheet->state == GTK_SHEET_COLUMN_SELECTED)
           row = MIN_VISIBLE_ROW(sheet)-1;
      if(sheet->state == GTK_SHEET_ROW_SELECTED)
           col = MIN_VISIBLE_COLUMN(sheet);
      if(row < sheet->maxrow){
           row = row + scroll;
           while(!sheet->row[row].is_visible && row<sheet->maxrow) row++;
      }
      gtk_sheet_click_cell(sheet, row, col, &veto);
      extend_selection = FALSE;
      break;
   case GDK_ISO_Left_Tab:
      row = sheet->active_cell.row;
      col = sheet->active_cell.col;
      if(sheet->state == GTK_SHEET_ROW_SELECTED) 
           col = MIN_VISIBLE_COLUMN(sheet)-1;
      if(sheet->state == GTK_SHEET_COLUMN_SELECTED) 
           row = MIN_VISIBLE_ROW(sheet);
      if(col > 0){
           col = col - scroll; 
           while(!sheet->column[col].is_visible && col>0) col--;
	   col=MAX(0, col);
      }       
      gtk_sheet_click_cell(sheet, row, col, &veto);
      extend_selection = FALSE;
      break;
   case GDK_Tab:
      row = sheet->active_cell.row;
      col = sheet->active_cell.col;
      if(sheet->state == GTK_SHEET_ROW_SELECTED) 
           col = MIN_VISIBLE_COLUMN(sheet)-1;
      if(sheet->state == GTK_SHEET_COLUMN_SELECTED) 
           row = MIN_VISIBLE_ROW(sheet);
      if(col < sheet->maxcol){
           col = col + scroll; 
           while(!sheet->column[col].is_visible && col<sheet->maxcol) col++;
      }       
      gtk_sheet_click_cell(sheet, row, col, &veto);
      extend_selection = FALSE;
      break;
/*    case GDK_BackSpace:
      if(sheet->active_cell.row >= 0 && sheet->active_cell.col >= 0){
       if(sheet->active_cell.col > 0){
            col = sheet->active_cell.col - scroll; 
	    row = sheet->active_cell.row;
            while(!sheet->column[col].is_visible && col > 0) col--;
       }       
      }
      gtk_sheet_click_cell(sheet, row, col, &veto);
      extend_selection = FALSE;
      break;
*/
    case GDK_Page_Up:
      scroll=MAX_VISIBLE_ROW(sheet)-MIN_VISIBLE_ROW(sheet)+1;
    case GDK_Up:
      if(extend_selection){
        if(state==GTK_STATE_NORMAL){
           row=sheet->active_cell.row;
           col=sheet->active_cell.col;
           gtk_sheet_click_cell(sheet, row, col, &veto);
           if(!veto) break;
        }
        if(sheet->selection_cell.row > 0){
          row = sheet->selection_cell.row - scroll;
          while(!sheet->row[row].is_visible && row > 0) row--;
          row = MAX(0, row);
          gtk_sheet_extend_selection(sheet, row, sheet->selection_cell.col);
        }
        return TRUE;
      }
      col = sheet->active_cell.col;
      row = sheet->active_cell.row;
      if(state==GTK_SHEET_COLUMN_SELECTED) 
             row = MIN_VISIBLE_ROW(sheet);
      if(state==GTK_SHEET_ROW_SELECTED) 
             col = MIN_VISIBLE_COLUMN(sheet);
      row = row - scroll;
      while(!sheet->row[row].is_visible && row > 0) row--;
      row = MAX(0,row);
      gtk_sheet_click_cell(sheet, row, col, &veto);
      extend_selection = FALSE;
      break;
    case GDK_Page_Down:
      scroll=MAX_VISIBLE_ROW(sheet)-MIN_VISIBLE_ROW(sheet)+1;
    case GDK_Down:
      if(extend_selection){
        if(state==GTK_STATE_NORMAL){
           row=sheet->active_cell.row;
           col=sheet->active_cell.col;
           gtk_sheet_click_cell(sheet, row, col, &veto);
           if(!veto) break;
        }
        if(sheet->selection_cell.row < sheet->maxrow){
          row = sheet->selection_cell.row + scroll;
          while(!sheet->row[row].is_visible && row < sheet->maxrow) row++;
          row = MIN(sheet->maxrow, row);
          gtk_sheet_extend_selection(sheet, row, sheet->selection_cell.col);
        }
        return TRUE;
      }
      col = sheet->active_cell.col;
      row = sheet->active_cell.row;
      if(sheet->active_cell.row < sheet->maxrow){
	   if(state==GTK_SHEET_COLUMN_SELECTED) 
                row = MIN_VISIBLE_ROW(sheet)-1;
	   if(state==GTK_SHEET_ROW_SELECTED) 
                col = MIN_VISIBLE_COLUMN(sheet);
	   row = row + scroll;
           while(!sheet->row[row].is_visible && row < sheet->maxrow) row++;
           row = MIN(sheet->maxrow, row);
      }
      gtk_sheet_click_cell(sheet, row, col, &veto);
      extend_selection = FALSE;
      break;
    case GDK_Right:
      if(extend_selection){
        if(state==GTK_STATE_NORMAL){
           row=sheet->active_cell.row;
           col=sheet->active_cell.col;
           gtk_sheet_click_cell(sheet, row, col, &veto);
           if(!veto) break;
        }
        if(sheet->selection_cell.col < sheet->maxcol){
          col = sheet->selection_cell.col + 1;
          while(!sheet->column[col].is_visible && col < sheet->maxcol) col++;
          gtk_sheet_extend_selection(sheet, sheet->selection_cell.row, col);
        }
        return TRUE;
      }
      col = sheet->active_cell.col;
      row = sheet->active_cell.row;
      if(sheet->active_cell.col < sheet->maxcol){
           col ++;
	   if(state==GTK_SHEET_ROW_SELECTED) 
                col = MIN_VISIBLE_COLUMN(sheet)-1;
	   if(state==GTK_SHEET_COLUMN_SELECTED) 
                row = MIN_VISIBLE_ROW(sheet);
           while(!sheet->column[col].is_visible && col < sheet->maxcol) col++;
           if(strlen(gtk_entry_get_text(GTK_ENTRY(gtk_sheet_get_entry(sheet)))) == 0 
              || force_move) {
	        gtk_sheet_click_cell(sheet, row, col, &veto);
           }
           else
              return FALSE;
      }
      extend_selection = FALSE;
      break;
    case GDK_Left:
      if(extend_selection){
        if(state==GTK_STATE_NORMAL){
           row=sheet->active_cell.row;
           col=sheet->active_cell.col;
           gtk_sheet_click_cell(sheet, row, col, &veto);
           if(!veto) break;
        }
        if(sheet->selection_cell.col > 0){
          col = sheet->selection_cell.col - 1;
          while(!sheet->column[col].is_visible && col > 0) col--;          
          gtk_sheet_extend_selection(sheet, sheet->selection_cell.row, col);
        }
	return TRUE;
      }
      col = sheet->active_cell.col - 1;
      row = sheet->active_cell.row;
      if(state==GTK_SHEET_ROW_SELECTED) 
                col = MIN_VISIBLE_COLUMN(sheet)-1;
      if(state==GTK_SHEET_COLUMN_SELECTED) 
                row = MIN_VISIBLE_ROW(sheet);
      while(!sheet->column[col].is_visible && col > 0) col--;
      col = MAX(0, col);

      if(strlen(gtk_entry_get_text(GTK_ENTRY(gtk_sheet_get_entry(sheet)))) == 0
         || force_move){
                gtk_sheet_click_cell(sheet, row, col, &veto);
      }
      else
         return FALSE;
      extend_selection = FALSE;
      break;
    case GDK_Home:
      row=0;
      while(!sheet->row[row].is_visible && row < sheet->maxrow) row++;
      gtk_sheet_click_cell(sheet, row, sheet->active_cell.col, &veto);
      extend_selection = FALSE;
      break;
    case GDK_End:
      row=sheet->maxrow;
      while(!sheet->row[row].is_visible && row > 0) row--;
      gtk_sheet_click_cell(sheet, row, sheet->active_cell.col, &veto);
      extend_selection = FALSE;
      break;
    default:
      if(in_selection) {
        GTK_SHEET_SET_FLAGS(sheet, GTK_SHEET_IN_SELECTION);
        if(extend_selection) return TRUE;
      }
      if(state == GTK_SHEET_ROW_SELECTED) 
        sheet->active_cell.col=MIN_VISIBLE_COLUMN(sheet);
      if(state == GTK_SHEET_COLUMN_SELECTED)
        sheet->active_cell.row=MIN_VISIBLE_ROW(sheet);
      return FALSE;
  }

  if(extend_selection) return TRUE;

  gtk_sheet_activate_cell(sheet, sheet->active_cell.row,
                                 sheet->active_cell.col);

  return TRUE;
} 

static void
gtk_sheet_size_request (GtkWidget * widget,
			GtkRequisition * requisition)
{
  GtkSheet *sheet;
  GList *children;
  GtkSheetChild *child;
  GtkRequisition child_requisition;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_SHEET (widget));
  g_return_if_fail (requisition != NULL);

  sheet = GTK_SHEET (widget);

  requisition->width = 3*DEFAULT_COLUMN_WIDTH;
  requisition->height = 3*DEFAULT_ROW_HEIGHT(widget);

  /* compute the size of the column title area */
  if(sheet->column_titles_visible) 
     requisition->height += sheet->column_title_area.height;

  /* compute the size of the row title area */
  if(sheet->row_titles_visible) 
     requisition->width += sheet->row_title_area.width;

  sheet->view.row0=ROW_FROM_YPIXEL(sheet, sheet->column_title_area.height+1);
  sheet->view.rowi=ROW_FROM_YPIXEL(sheet, sheet->sheet_window_height-1);
  sheet->view.col0=COLUMN_FROM_XPIXEL(sheet, sheet->row_title_area.width+1);
  sheet->view.coli=COLUMN_FROM_XPIXEL(sheet, sheet->sheet_window_width);

  if(!sheet->column_titles_visible) 
     sheet->view.row0=ROW_FROM_YPIXEL(sheet, 1);

  if(!sheet->row_titles_visible) 
     sheet->view.col0=COLUMN_FROM_XPIXEL(sheet, 1);

  children = sheet->children;
  while (children)
  {
    child = children->data;
    children = children->next;

    gtk_widget_size_request(child->widget, &child_requisition);
  }
}

 
static void
gtk_sheet_size_allocate (GtkWidget * widget,
			 GtkAllocation * allocation)
{
  GtkSheet *sheet;
  GtkAllocation sheet_allocation;
  gint border_width;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_SHEET (widget));
  g_return_if_fail (allocation != NULL);

  sheet = GTK_SHEET (widget);
  widget->allocation = *allocation;
  border_width = GTK_CONTAINER(widget)->border_width;

  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (widget->window,
	      	  	    allocation->x + border_width,
	                    allocation->y + border_width,
                            allocation->width - 2*border_width,
	                    allocation->height - 2*border_width);

  /* use internal allocation structure for all the math
   * because it's easier than always subtracting the container
   * border width */
  sheet->internal_allocation.x = 0;
  sheet->internal_allocation.y = 0;
  sheet->internal_allocation.width = allocation->width - 2*border_width;
  sheet->internal_allocation.height = allocation->height - 2*border_width;
	
  sheet_allocation.x = 0;
  sheet_allocation.y = 0;
  sheet_allocation.width = allocation->width - 2*border_width;
  sheet_allocation.height = allocation->height - 2*border_width;

  sheet->sheet_window_width = sheet_allocation.width;
  sheet->sheet_window_height = sheet_allocation.height;

  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (sheet->sheet_window,
			    sheet_allocation.x,
			    sheet_allocation.y,
			    sheet_allocation.width,
			    sheet_allocation.height);

    /* position the window which holds the column title buttons */
  sheet->column_title_area.x = 0;
  sheet->column_title_area.y = 0;
  if(sheet->row_titles_visible)
       sheet->column_title_area.x = sheet->row_title_area.width;
  sheet->column_title_area.width = sheet_allocation.width - 
                                     sheet->column_title_area.x;
  if(GTK_WIDGET_REALIZED(widget) && sheet->column_titles_visible)
      gdk_window_move_resize (sheet->column_title_window,
			      sheet->column_title_area.x,
			      sheet->column_title_area.y,
			      sheet->column_title_area.width,
			      sheet->column_title_area.height);

  sheet->sheet_window_width = sheet_allocation.width;
  sheet->sheet_window_height = sheet_allocation.height;

  /* column button allocation */
  size_allocate_column_title_buttons (sheet);

  /* position the window which holds the row title buttons */
  sheet->row_title_area.x = 0;
  sheet->row_title_area.y = 0;
  if(sheet->column_titles_visible)
       sheet->row_title_area.y = sheet->column_title_area.height;
  sheet->row_title_area.height = sheet_allocation.height -
                                   sheet->row_title_area.y;

  if(GTK_WIDGET_REALIZED(widget) && sheet->row_titles_visible)
      gdk_window_move_resize (sheet->row_title_window,
			      sheet->row_title_area.x,
			      sheet->row_title_area.y,
			      sheet->row_title_area.width,
			      sheet->row_title_area.height);


  /* row button allocation */
  size_allocate_row_title_buttons (sheet);

  sheet->view.row0=ROW_FROM_YPIXEL(sheet, sheet->column_title_area.height+1);
  sheet->view.rowi=ROW_FROM_YPIXEL(sheet, sheet->sheet_window_height-1);
  sheet->view.col0=COLUMN_FROM_XPIXEL(sheet, sheet->row_title_area.width+1);
  sheet->view.coli=COLUMN_FROM_XPIXEL(sheet, sheet->sheet_window_width);

  if(!sheet->column_titles_visible)
       sheet->view.row0=ROW_FROM_YPIXEL(sheet, 1);
      
  if(!sheet->row_titles_visible)
       sheet->view.col0=COLUMN_FROM_XPIXEL(sheet, 1);

  size_allocate_column_title_buttons(sheet);
  size_allocate_row_title_buttons(sheet);

  /* re-scale backing pixmap */
  gtk_sheet_make_backing_pixmap(sheet, 0, 0); 
  gtk_sheet_position_children(sheet);

  /* set the scrollbars adjustments */
  adjust_scrollbars (sheet);
}

static void
size_allocate_column_title_buttons (GtkSheet * sheet)
{
  gint i;
  gint x,width;

  if (!sheet->column_titles_visible) return;
  if (!GTK_WIDGET_REALIZED (sheet))
    return;

  width = sheet->sheet_window_width;
  x = 0;

  if(sheet->row_titles_visible)
    {
      width -= sheet->row_title_area.width;
      x = sheet->row_title_area.width;
    }

  if(sheet->column_title_area.width != width || sheet->column_title_area.x != x)
  {
     sheet->column_title_area.width = width;
     sheet->column_title_area.x = x;
     gdk_window_move_resize (sheet->column_title_window,
  		  	     sheet->column_title_area.x,
			     sheet->column_title_area.y,
			     sheet->column_title_area.width,
			     sheet->column_title_area.height);
  }


  if(MAX_VISIBLE_COLUMN(sheet) == sheet->maxcol)
     gdk_window_clear_area (sheet->column_title_window,
	   		    0,0,
			    sheet->column_title_area.width, 
                            sheet->column_title_area.height);

  if(!GTK_WIDGET_DRAWABLE(sheet)) return;

  for (i = MIN_VISIBLE_COLUMN(sheet); i <= MAX_VISIBLE_COLUMN(sheet); i++)
      gtk_sheet_button_draw(sheet,-1,i);
}
	
static void
size_allocate_row_title_buttons (GtkSheet * sheet)
{
  gint i;
  gint y, height;

  if (!sheet->row_titles_visible) return;
  if (!GTK_WIDGET_REALIZED (sheet))
    return;

  height = sheet->sheet_window_height;
  y = 0;

  if(sheet->column_titles_visible)
    {
      height -= sheet->column_title_area.height;
      y = sheet->column_title_area.height;
    }
    
  if(sheet->row_title_area.height != height || sheet->row_title_area.y != y){
     sheet->row_title_area.y = y;
     sheet->row_title_area.height = height;
     gdk_window_move_resize (sheet->row_title_window,
  		  	     sheet->row_title_area.x,
			     sheet->row_title_area.y,
			     sheet->row_title_area.width,
			     sheet->row_title_area.height);
  }
  if(MAX_VISIBLE_ROW(sheet) == sheet->maxrow)
    gdk_window_clear_area (sheet->row_title_window,
			   0,0,
			   sheet->row_title_area.width, 
                           sheet->row_title_area.height);

  if(!GTK_WIDGET_DRAWABLE(sheet)) return;

  for(i = MIN_VISIBLE_ROW(sheet); i <= MAX_VISIBLE_ROW(sheet); i++)
      gtk_sheet_button_draw(sheet,i,-1);
}
	  
static void
gtk_sheet_recalc_top_ypixels(GtkSheet *sheet, gint row)
{
  gint i, cy;

  cy = sheet->column_title_area.height;
  if(!sheet->column_titles_visible) cy = 0;
  for(i=0; i<=sheet->maxrow; i++){
      sheet->row[i].top_ypixel=cy;
      if(sheet->row[i].is_visible) cy+=sheet->row[i].height;
  }
}

static void
gtk_sheet_recalc_left_xpixels(GtkSheet *sheet, gint column)
{
  gint i, cx;

  cx = sheet->row_title_area.width;
  if(!sheet->row_titles_visible) cx = 0;
  for(i=0; i<=sheet->maxcol; i++){
      sheet->column[i].left_xpixel=cx;
      if(sheet->column[i].is_visible) cx+=sheet->column[i].width;
  }

}



static void
gtk_sheet_size_allocate_entry(GtkSheet *sheet)
{
 GtkAllocation shentry_allocation;
 GtkSheetCellAttr attributes;
 GtkEntry *sheet_entry;
 GtkStyle *style = NULL, *previous_style = NULL;
 gint row, col;
 gint size, max_size, text_size, column_width;
 const gchar *text;
 PangoFontDescription *font_desc;

 if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;
 if(!GTK_WIDGET_MAPPED(GTK_WIDGET(sheet))) return;

 sheet_entry = GTK_ENTRY(gtk_sheet_get_entry(sheet));

 gtk_sheet_get_attributes(sheet, sheet->active_cell.row, sheet->active_cell.col, &attributes); 
 font_desc = (attributes.font_desc == NULL) ? 
             GTK_WIDGET(sheet)->style->font_desc : attributes.font_desc;

 if(GTK_WIDGET_REALIZED(sheet->sheet_entry)){

  if(!GTK_WIDGET(sheet_entry)->style) 
        gtk_widget_ensure_style(GTK_WIDGET(sheet_entry));

  previous_style = GTK_WIDGET(sheet_entry)->style;

  style = gtk_style_copy(previous_style);
  style->bg[GTK_STATE_NORMAL] = attributes.background;
  style->fg[GTK_STATE_NORMAL] = attributes.foreground;
  style->text[GTK_STATE_NORMAL] = attributes.foreground;
  style->bg[GTK_STATE_ACTIVE] = attributes.background;
  style->fg[GTK_STATE_ACTIVE] = attributes.foreground;
  style->text[GTK_STATE_ACTIVE] = attributes.foreground;

  pango_font_description_free(style->font_desc);

  style->font_desc = pango_font_description_copy(font_desc);

  GTK_WIDGET(sheet_entry)->style = style;
  gtk_widget_size_request(sheet->sheet_entry, NULL);
  GTK_WIDGET(sheet_entry)->style = previous_style;

  if(style != previous_style){
    if(!GTK_IS_ITEM_ENTRY(sheet->sheet_entry)){
      style->bg[GTK_STATE_NORMAL] = previous_style->bg[GTK_STATE_NORMAL];
      style->fg[GTK_STATE_NORMAL] = previous_style->fg[GTK_STATE_NORMAL];
      style->bg[GTK_STATE_ACTIVE] = previous_style->bg[GTK_STATE_ACTIVE];
      style->fg[GTK_STATE_ACTIVE] = previous_style->fg[GTK_STATE_ACTIVE];
    }
    gtk_widget_set_style(GTK_WIDGET(sheet_entry), style);
  }
 }

 if(GTK_IS_ITEM_ENTRY(sheet_entry))
    max_size = GTK_ITEM_ENTRY(sheet_entry)->text_max_size;
 else
    max_size = 0;

 text_size = 0;
 text = gtk_entry_get_text(GTK_ENTRY(sheet_entry));
 if(text && strlen(text) > 0){ 
     text_size = STRING_WIDTH(GTK_WIDGET(sheet), font_desc, text);
 }

 column_width=sheet->column[sheet->active_cell.col].width;

 size=MIN(text_size, max_size);
 size=MAX(size,column_width-2*CELLOFFSET);

 row=sheet->active_cell.row;
 col=sheet->active_cell.col;

 shentry_allocation.x = COLUMN_LEFT_XPIXEL(sheet,sheet->active_cell.col);
 shentry_allocation.y = ROW_TOP_YPIXEL(sheet,sheet->active_cell.row);
 shentry_allocation.width = column_width;
 shentry_allocation.height = sheet->row[sheet->active_cell.row].height;

 if(GTK_IS_ITEM_ENTRY(sheet->sheet_entry)){

   shentry_allocation.height -= 2*CELLOFFSET;
   shentry_allocation.y += CELLOFFSET;
   if(gtk_sheet_clip_text(sheet))
     shentry_allocation.width = column_width - 2*CELLOFFSET;
   else
     shentry_allocation.width = size;

   switch(GTK_ITEM_ENTRY(sheet_entry)->justification){
     case GTK_JUSTIFY_CENTER:
       shentry_allocation.x += (column_width)/2 - size/2;
       break;
     case GTK_JUSTIFY_RIGHT:
       shentry_allocation.x += column_width - size - CELLOFFSET;
       break;
     case GTK_JUSTIFY_LEFT:
     case GTK_JUSTIFY_FILL:
       shentry_allocation.x += CELLOFFSET;
       break;
    }

 }

 if(!GTK_IS_ITEM_ENTRY(sheet->sheet_entry)){
   shentry_allocation.x += 2;
   shentry_allocation.y += 2;
   shentry_allocation.width -= MIN(shentry_allocation.width, 3);
   shentry_allocation.height -= MIN(shentry_allocation.height, 3);
 }

 gtk_widget_size_allocate(sheet->sheet_entry, &shentry_allocation);

 if(previous_style == style) gtk_style_unref(previous_style);
}

static void
gtk_sheet_entry_set_max_size(GtkSheet *sheet)
{
 gint i;
 gint size=0;
 gint sizel=0, sizer=0;
 gint row,col;
 GtkJustification justification;

 row=sheet->active_cell.row;
 col=sheet->active_cell.col;

 if(!GTK_IS_ITEM_ENTRY(sheet->sheet_entry) || gtk_sheet_clip_text(sheet)) return;

 justification = GTK_ITEM_ENTRY(sheet->sheet_entry)->justification;

 switch(justification){
  case GTK_JUSTIFY_FILL:
  case GTK_JUSTIFY_LEFT:
    for(i=col+1; i<=MAX_VISIBLE_COLUMN(sheet); i++){
     if(gtk_sheet_cell_get_text(sheet, row, i)) break;
     size+=sheet->column[i].width;
    }
    size = MIN(size, sheet->sheet_window_width - COLUMN_LEFT_XPIXEL(sheet, col));
    break;
  case GTK_JUSTIFY_RIGHT:
    for(i=col-1; i>=MIN_VISIBLE_COLUMN(sheet); i--){
     if(gtk_sheet_cell_get_text(sheet, row, i)) break;
     size+=sheet->column[i].width;
    }
    break;
  case GTK_JUSTIFY_CENTER:
    for(i=col+1; i<=MAX_VISIBLE_COLUMN(sheet); i++){
/*     if(gtk_sheet_cell_get_text(sheet, row, i)) break;
*/
     sizer+=sheet->column[i].width;
    }
    for(i=col-1; i>=MIN_VISIBLE_COLUMN(sheet); i--){
     if(gtk_sheet_cell_get_text(sheet, row, i)) break;
     sizel+=sheet->column[i].width;
    }
    size=2*MIN(sizel, sizer);
    break;
 }

 if(size!=0) size+=sheet->column[col].width;
 GTK_ITEM_ENTRY(sheet->sheet_entry)->text_max_size=size;

}

static void
create_sheet_entry(GtkSheet *sheet)
{
    GtkWidget *widget;
    GtkWidget *parent;
    GtkWidget *entry;
    GtkStyle *style;
    gint found_entry = FALSE;

    widget = GTK_WIDGET(sheet);

    style = gtk_style_copy(GTK_WIDGET(sheet)->style); 

    if(sheet->sheet_entry){
        /* avoids warnings */
        g_object_ref(G_OBJECT(sheet->sheet_entry));
        gtk_widget_unparent(sheet->sheet_entry);
        gtk_widget_destroy(sheet->sheet_entry);
    }

    if (sheet->entry_type) {

        if (!g_type_is_a (sheet->entry_type, GTK_TYPE_ENTRY)) {
            parent = GTK_WIDGET(g_object_new(sheet->entry_type, NULL));

            sheet->sheet_entry = parent;

            entry = gtk_sheet_get_entry (sheet);
            if(GTK_IS_ENTRY(entry)) found_entry = TRUE;

        } else {
            /*parent = GTK_WIDGET(gtk_type_new(sheet->entry_type));*/
            parent = GTK_WIDGET(g_object_new(sheet->entry_type, NULL));
            entry = parent;
            found_entry = TRUE;
        }             
                                        
        if (!found_entry) {
            g_warning ("Entry type must be GtkEntry subclass, using default");
            entry = gtk_item_entry_new();
            sheet->sheet_entry = entry;
        } else {
            sheet->sheet_entry = parent;
        }

    } else {
        entry = gtk_item_entry_new();
        sheet->sheet_entry = entry;
    }

    gtk_widget_size_request(sheet->sheet_entry, NULL);

    if(GTK_WIDGET_REALIZED(sheet))
    {
        gtk_widget_set_parent_window (sheet->sheet_entry, sheet->sheet_window);
        gtk_widget_set_parent(sheet->sheet_entry, GTK_WIDGET(sheet));
        gtk_widget_realize(sheet->sheet_entry);
    }

    g_signal_connect_swapped(G_OBJECT(entry),"key_press_event",
                           (GCallback) gtk_sheet_entry_key_press,
                           G_OBJECT(sheet)); 

    gtk_widget_show (sheet->sheet_entry); 
}


/**
 * gtk_sheet_get_entry:
 * @sheet: a #GtkSheet
 *
 * Get sheet's entry widget.
 * Return value: a #GtkWidget
 */ 
GtkWidget * 
gtk_sheet_get_entry(GtkSheet *sheet)
{
 GtkWidget *parent;
 GtkWidget *entry = NULL;
 GtkTableChild *table_child;
 GtkBoxChild *box_child;
 GList *children = NULL;

 g_return_val_if_fail (sheet != NULL, NULL);
 g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);
 g_return_val_if_fail (sheet->sheet_entry != NULL, NULL);

 if(GTK_IS_ENTRY(sheet->sheet_entry)) return (sheet->sheet_entry);

 parent = GTK_WIDGET(sheet->sheet_entry);

 if(GTK_IS_TABLE(parent)) children = GTK_TABLE(parent)->children;
 if(GTK_IS_BOX(parent)) children = GTK_BOX(parent)->children;

 if(!children) return NULL;

 while(children){
      if(GTK_IS_TABLE(parent)) {
                 table_child = children->data;
                 entry = table_child->widget;
      }
      if(GTK_IS_BOX(parent)){
                 box_child = children->data; 
                 entry = box_child->widget;
      }

      if(GTK_IS_ENTRY(entry))  
                                break;
      children = children->next;                        
 } 


 if(!GTK_IS_ENTRY(entry))   return NULL;

 return (entry);

}

/**
 * gtk_sheet_get_entry_widget:
 * @sheet: a #GtkSheet
 *
 * Get sheet's entry widget.
 * Return value: a #GtkWidget
 */ 
GtkWidget * 
gtk_sheet_get_entry_widget(GtkSheet *sheet)
{
 g_return_val_if_fail (sheet != NULL, NULL);
 g_return_val_if_fail (GTK_IS_SHEET (sheet), NULL);
 g_return_val_if_fail (sheet->sheet_entry != NULL, NULL);

 return (sheet->sheet_entry);
}

/* BUTTONS */
static void
row_button_set (GtkSheet *sheet, gint row)
{
  if(sheet->row[row].button.state == GTK_STATE_ACTIVE) return;

  sheet->row[row].button.state = GTK_STATE_ACTIVE;
  gtk_sheet_button_draw(sheet, row, -1);
 
}

static void
column_button_set (GtkSheet *sheet, gint column)
{
  if(sheet->column[column].button.state == GTK_STATE_ACTIVE) return;

  sheet->column[column].button.state = GTK_STATE_ACTIVE;
  gtk_sheet_button_draw(sheet, -1, column);
 
}

static void
row_button_release (GtkSheet *sheet, gint row)
{
  if(sheet->row[row].button.state == GTK_STATE_NORMAL) return;

  sheet->row[row].button.state = GTK_STATE_NORMAL;
  gtk_sheet_button_draw(sheet, row, -1);
}

static void
column_button_release (GtkSheet *sheet, gint column)
{
  if(sheet->column[column].button.state == GTK_STATE_NORMAL) return;

  sheet->column[column].button.state = GTK_STATE_NORMAL;
  gtk_sheet_button_draw(sheet, -1, column);
}

static void
gtk_sheet_button_draw (GtkSheet *sheet, gint row, gint column)
{
  GdkWindow *window = NULL;
  GtkShadowType shadow_type;
  guint width = 0, height = 0;
  gint x = 0, y = 0;
  gint index = 0;
  gint text_width = 0, text_height = 0;
  GtkSheetButton *button = NULL;
  GtkSheetChild *child = NULL;
  GdkRectangle allocation;
  gboolean is_sensitive = FALSE;
  gint state = 0;
  gint len = 0;
  gchar *line = 0;
  gchar *words = 0;
  gchar label[10];
  PangoAlignment align = PANGO_ALIGN_LEFT; 
  gboolean rtl;

  rtl = gtk_widget_get_direction(GTK_WIDGET(sheet)) == GTK_TEXT_DIR_RTL;

  if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))) return;

  if(row >= 0 && !sheet->row[row].is_visible) return;
  if(column >= 0 && !sheet->column[column].is_visible) return;
  if(row >= 0 && !sheet->row_titles_visible) return;
  if(column >= 0 && !sheet->column_titles_visible) return;
  if(column>=0 && column <MIN_VISIBLE_COLUMN(sheet)) return;
  if(column>=0 && column >MAX_VISIBLE_COLUMN(sheet)) return;
  if(row>=0 && row <MIN_VISIBLE_ROW(sheet)) return;
  if(row>=0 && row >MAX_VISIBLE_ROW(sheet)) return;
  if( (row == -1) && (column == -1) ) return; 

  if(row==-1){
     window=sheet->column_title_window;
     button=&sheet->column[column].button;
     index=column;
     x = COLUMN_LEFT_XPIXEL(sheet, column)+CELL_SPACING;
     if(sheet->row_titles_visible) x -= sheet->row_title_area.width;
     y = 0;
     width = sheet->column[column].width;
     height = sheet->column_title_area.height;
     is_sensitive=sheet->column[column].is_sensitive;
  }
  else if(column==-1){
     window=sheet->row_title_window;
     button=&sheet->row[row].button;
     index=row;
     x = 0;
     y = ROW_TOP_YPIXEL(sheet, row)+CELL_SPACING;
     if(sheet->column_titles_visible) y-=sheet->column_title_area.height;
     width = sheet->row_title_area.width;
     height = sheet->row[row].height;
     is_sensitive=sheet->row[row].is_sensitive;
  }

  allocation.x = x;
  allocation.y = y;
  allocation.width = width;
  allocation.height = height;
 
  gdk_window_clear_area (window,
                         x, y,
	                 width, height);

  gtk_paint_box (sheet->button->style, window,
                 GTK_STATE_NORMAL, GTK_SHADOW_OUT, 
                 &allocation, GTK_WIDGET(sheet->button),
                 "buttondefault", x, y, width, height);

  state = button->state;
  if(!is_sensitive) state=GTK_STATE_INSENSITIVE;

  if (state == GTK_STATE_ACTIVE)
     shadow_type = GTK_SHADOW_IN;
  else
     shadow_type = GTK_SHADOW_OUT;

  if(state != GTK_STATE_NORMAL && state != GTK_STATE_INSENSITIVE)
  gtk_paint_box (sheet->button->style, window,
                 button->state, shadow_type, 
                 &allocation, GTK_WIDGET(sheet->button),
                 "button", x, y, width, height);

  if(button->label_visible){

    text_height=DEFAULT_ROW_HEIGHT(GTK_WIDGET(sheet))-2*CELLOFFSET;

    gdk_gc_set_clip_rectangle(GTK_WIDGET(sheet)->style->fg_gc[button->state], 
                              &allocation);
    gdk_gc_set_clip_rectangle(GTK_WIDGET(sheet)->style->white_gc, &allocation);

/*
    y += DEFAULT_ROW_HEIGHT(GTK_WIDGET(sheet))/2 + sheet->button->style->ythickness + DEFAULT_FONT_DESCENT(GTK_WIDGET(sheet));
*/
    y += 2*sheet->button->style->ythickness;

    if(button->label && strlen(button->label)>0){
           PangoLayout *layout = NULL;
           gint real_x = x, real_y = y;

           words=button->label;
           line = g_new(gchar, 1);
           line[0]='\0';

           while(words && *words != '\0'){
             if(*words != '\n'){
                len=strlen(line);
                line=g_realloc(line, len+2);
                line[len]=*words;
                line[len+1]='\0';
             }
             if(*words == '\n' || *(words+1) == '\0'){
               text_width = STRING_WIDTH(GTK_WIDGET(sheet), GTK_WIDGET(sheet)->style->font_desc, line);

               layout = gtk_widget_create_pango_layout (GTK_WIDGET(sheet), line);
               switch(button->justification){
                 case GTK_JUSTIFY_LEFT:
                   real_x = x + CELLOFFSET;
                   align = rtl ? PANGO_ALIGN_RIGHT : PANGO_ALIGN_LEFT;
                   break;
                 case GTK_JUSTIFY_RIGHT:
                   real_x = x + width - text_width - CELLOFFSET;
                   align = rtl ? PANGO_ALIGN_LEFT : PANGO_ALIGN_RIGHT;
                   break;
                 case GTK_JUSTIFY_CENTER:
                 default:
                   real_x = x + (width - text_width)/2;
                   align = rtl ? PANGO_ALIGN_RIGHT : PANGO_ALIGN_LEFT;
                   pango_layout_set_justify (layout, TRUE);
               }
               pango_layout_set_alignment (layout, align);
               gtk_paint_layout (GTK_WIDGET(sheet)->style,
                                 window,
                                 state,
                                 FALSE,
                                 &allocation,
                                 GTK_WIDGET(sheet),
                                 "label",
                                 real_x, real_y,
                                 layout);
               g_object_unref(G_OBJECT(layout));

               real_y += text_height + 2;

               g_free(line);
               line = g_new(gchar, 1);
               line[0]='\0';
             }
             words++;
           }
           g_free(line);
    }else{
           PangoLayout *layout = NULL;
           gint real_x = x, real_y = y;

           sprintf(label,"%d",index);
           text_width = STRING_WIDTH(GTK_WIDGET(sheet), GTK_WIDGET(sheet)->style->font_desc, label);

           layout = gtk_widget_create_pango_layout (GTK_WIDGET(sheet), label);
           switch(button->justification){
             case GTK_JUSTIFY_LEFT:
               real_x = x + CELLOFFSET;
               align = rtl ? PANGO_ALIGN_RIGHT : PANGO_ALIGN_LEFT;
               break;
             case GTK_JUSTIFY_RIGHT:
               real_x = x + width - text_width - CELLOFFSET;
               align = rtl ? PANGO_ALIGN_LEFT : PANGO_ALIGN_RIGHT;
               break;
             case GTK_JUSTIFY_CENTER:
             default:
               real_x = x + (width - text_width)/2;
               align = rtl ? PANGO_ALIGN_RIGHT : PANGO_ALIGN_LEFT;
               pango_layout_set_justify (layout, TRUE);
           }
           pango_layout_set_alignment (layout, align);
           gtk_paint_layout (GTK_WIDGET(sheet)->style,
                             window,
                             state,
                             FALSE,
                             &allocation,
                             GTK_WIDGET(sheet),
                             "label",
                             real_x, real_y,
                             layout);
           g_object_unref(G_OBJECT(layout));
    }

    gdk_gc_set_clip_rectangle(GTK_WIDGET(sheet)->style->fg_gc[button->state],
                            NULL);
    gdk_gc_set_clip_rectangle(GTK_WIDGET(sheet)->style->white_gc, NULL);

  }

  if((child = button->child) && (child->widget)){
      child->x = allocation.x;
      child->y = allocation.y;

      child->x += (width - child->widget->requisition.width) / 2; 
      child->y += (height - child->widget->requisition.height) / 2;
      allocation.x = child->x;
      allocation.y = child->y;
      allocation.width = child->widget->requisition.width;
      allocation.height = child->widget->requisition.height;

      x = child->x;
      y = child->y;

      gtk_widget_set_state(child->widget, button->state);

      if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)) &&
         GTK_WIDGET_MAPPED(child->widget))
            {
              gtk_widget_size_allocate(child->widget, 
                                       &allocation);
              gtk_widget_queue_draw(child->widget);
            }
  }
   
}


/* SCROLLBARS
 *
 * functions:
 *   adjust_scrollbars
 *   vadjustment_changed
 *   hadjustment_changed
 *   vadjustment_value_changed
 *   hadjustment_value_changed */

static void
adjust_scrollbars (GtkSheet * sheet)
{

 if(sheet->vadjustment){ 
  sheet->vadjustment->page_size = sheet->sheet_window_height;
  sheet->vadjustment->page_increment = sheet->sheet_window_height / 2;
  sheet->vadjustment->step_increment = DEFAULT_ROW_HEIGHT(GTK_WIDGET(sheet));
  sheet->vadjustment->lower = 0;
  sheet->vadjustment->upper = SHEET_HEIGHT (sheet) + 80;
/*
  if (sheet->sheet_window_height - sheet->voffset > SHEET_HEIGHT (sheet))
    {
      sheet->vadjustment->value = MAX(0, SHEET_HEIGHT (sheet) - 
	sheet->sheet_window_height);
      gtk_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), 
			       "value_changed");
    }
*/
    g_signal_emit_by_name (GTK_OBJECT(sheet->vadjustment), "changed");

 }

 if(sheet->hadjustment){
  sheet->hadjustment->page_size = sheet->sheet_window_width;
  sheet->hadjustment->page_increment = sheet->sheet_window_width / 2;
  sheet->hadjustment->step_increment = DEFAULT_COLUMN_WIDTH;
  sheet->hadjustment->lower = 0;
  sheet->hadjustment->upper = SHEET_WIDTH (sheet)+ 80;
/*
  if (sheet->sheet_window_width - sheet->hoffset > SHEET_WIDTH (sheet))
    {
      sheet->hadjustment->value = MAX(0, SHEET_WIDTH (sheet) - 
	sheet->sheet_window_width);
      gtk_signal_emit_by_name (GTK_OBJECT(sheet->hadjustment), 
			       "value_changed");
    }
*/
    g_signal_emit_by_name (GTK_OBJECT(sheet->hadjustment), "changed");

 }
/*
 if(GTK_WIDGET_REALIZED(sheet)) 
   {
     if(sheet->row_titles_visible){
                 size_allocate_row_title_buttons(sheet);
                 gdk_window_show(sheet->row_title_window);
     }

     if(sheet->column_titles_visible){
                 size_allocate_column_title_buttons(sheet);
                 gdk_window_show(sheet->column_title_window);
     }

     gtk_sheet_range_draw(sheet, NULL);
   }
*/
}


static void
vadjustment_changed (GtkAdjustment * adjustment,
			       gpointer data)
{
  GtkSheet *sheet;

  g_return_if_fail (adjustment != NULL);
  g_return_if_fail (data != NULL);

  sheet = GTK_SHEET (data);

}

static void
hadjustment_changed (GtkAdjustment * adjustment,
			       gpointer data)
{
  GtkSheet *sheet;

  g_return_if_fail (adjustment != NULL);
  g_return_if_fail (data != NULL);

  sheet = GTK_SHEET (data);

}


static void
vadjustment_value_changed (GtkAdjustment * adjustment,
				     gpointer data)
{
  GtkSheet *sheet;
  gint diff, value, old_value;
  gint i;
  gint row, new_row;
  gint y=0;

  g_return_if_fail (adjustment != NULL);
  g_return_if_fail (data != NULL);
  g_return_if_fail (GTK_IS_SHEET (data));

  sheet = GTK_SHEET (data);

  if(GTK_SHEET_IS_FROZEN(sheet)) return;

  row=ROW_FROM_YPIXEL(sheet,sheet->column_title_area.height + CELL_SPACING);
  if(!sheet->column_titles_visible)
     row=ROW_FROM_YPIXEL(sheet,CELL_SPACING);
    
  old_value = -sheet->voffset;

  for(i=0; i<= sheet->maxrow; i++){
   if(sheet->row[i].is_visible) y+=sheet->row[i].height;
   if(y > adjustment->value) break;
  }
  y-=sheet->row[i].height;
  new_row=i;

  if (adjustment->value > sheet->old_vadjustment && sheet->old_vadjustment > 0. &&
      sheet->row[i].height > sheet->vadjustment->step_increment){
/* This avoids embarrassing twitching */
          if(row == new_row && row != sheet->maxrow &&
             adjustment->value - sheet->old_vadjustment >= 
                          sheet->vadjustment->step_increment &&
             new_row + 1 != MIN_VISIBLE_ROW(sheet)){
                new_row+=1;
                y=y+sheet->row[row].height;
          }
  }

/* Negative old_adjustment enforces the redraw, otherwise avoid spureous redraw */
  if(sheet->old_vadjustment >= 0. && row == new_row){
      sheet->old_vadjustment = sheet->vadjustment->value;
      return;
  }

  sheet->old_vadjustment = sheet->vadjustment->value;
  adjustment->value=y;

 
  if(new_row == 0){
   sheet->vadjustment->step_increment=
   sheet->row[0].height;
  }else{
   sheet->vadjustment->step_increment=
   MIN(sheet->row[new_row].height, sheet->row[new_row-1].height);
  }

  sheet->vadjustment->value=adjustment->value;

  value = adjustment->value;

  if (value >= -sheet->voffset)
	{
	  /* scroll down */
	  diff = value + sheet->voffset;
	}
  else
	{
	  /* scroll up */
	  diff = -sheet->voffset - value;
	}

      sheet->voffset = -value;
 
  sheet->view.row0=ROW_FROM_YPIXEL(sheet, sheet->column_title_area.height+1);
  sheet->view.rowi=ROW_FROM_YPIXEL(sheet, sheet->sheet_window_height-1);
  if(!sheet->column_titles_visible)
     sheet->view.row0=ROW_FROM_YPIXEL(sheet, 1);

  if(GTK_WIDGET_REALIZED(sheet->sheet_entry) &&
     sheet->state == GTK_SHEET_NORMAL && 
     sheet->active_cell.row >= 0 && sheet->active_cell.col >= 0 &&
     !gtk_sheet_cell_isvisible(sheet, sheet->active_cell.row,
                                      sheet->active_cell.col))
    {
      const gchar *text;

      text = gtk_entry_get_text(GTK_ENTRY(gtk_sheet_get_entry(sheet)));

      if(!text || strlen(text)==0) 
             gtk_sheet_cell_clear(sheet,
                                  sheet->active_cell.row,
                                  sheet->active_cell.col);
       gtk_widget_unmap(sheet->sheet_entry);
    }

  gtk_sheet_position_children(sheet);

  gtk_sheet_range_draw(sheet, NULL);
  size_allocate_row_title_buttons(sheet);
  size_allocate_global_button(sheet);
}

static void
hadjustment_value_changed (GtkAdjustment * adjustment,
			   gpointer data)
{
  GtkSheet *sheet;
  gint i, diff, value, old_value;
  gint column, new_column;
  gint x=0;

  g_return_if_fail (adjustment != NULL);
  g_return_if_fail (data != NULL);
  g_return_if_fail (GTK_IS_SHEET (data));

  sheet = GTK_SHEET (data);

  if(GTK_SHEET_IS_FROZEN(sheet)) return;

  column=COLUMN_FROM_XPIXEL(sheet,sheet->row_title_area.width + CELL_SPACING);
  if(!sheet->row_titles_visible)
     column=COLUMN_FROM_XPIXEL(sheet, CELL_SPACING);

  old_value = -sheet->hoffset;

  for(i=0; i<= sheet->maxcol; i++){
   if(sheet->column[i].is_visible) x+=sheet->column[i].width;
   if(x > adjustment->value) break;
  }
  x-=sheet->column[i].width;
  new_column=i;

  if (adjustment->value > sheet->old_hadjustment && sheet->old_hadjustment > 0 &&
      sheet->column[i].width > sheet->hadjustment->step_increment){
/* This avoids embarrassing twitching */
          if(column == new_column && column != sheet->maxcol &&
             adjustment->value - sheet->old_hadjustment >= 
                          sheet->hadjustment->step_increment &&
             new_column + 1 != MIN_VISIBLE_COLUMN(sheet)){
             new_column+=1;
             x=x+sheet->column[column].width;
          }
  }

/* Negative old_adjustment enforces the redraw, otherwise avoid spureous redraw */
  if(sheet->old_hadjustment >= 0. && new_column == column){
     sheet->old_hadjustment = sheet->hadjustment->value;
     return;
  }

  sheet->old_hadjustment = sheet->hadjustment->value;
  adjustment->value=x;

  if(new_column == 0){
   sheet->hadjustment->step_increment=
   sheet->column[0].width;
  }else{
   sheet->hadjustment->step_increment=
   MIN(sheet->column[new_column].width, sheet->column[new_column-1].width);
  }


  sheet->hadjustment->value=adjustment->value;

  value = adjustment->value;

  if (value >= -sheet->hoffset)
        {
	  /* scroll right */
	  diff = value + sheet->hoffset;
	}
  else
	{
	  /* scroll left */
	  diff = -sheet->hoffset - value;
	}

  sheet->hoffset = -value;

  sheet->view.col0=COLUMN_FROM_XPIXEL(sheet, sheet->row_title_area.width+1);
  sheet->view.coli=COLUMN_FROM_XPIXEL(sheet, sheet->sheet_window_width);
  if(!sheet->row_titles_visible)
    sheet->view.col0=COLUMN_FROM_XPIXEL(sheet, 1);

  if(GTK_WIDGET_REALIZED(sheet->sheet_entry) &&
     sheet->state == GTK_SHEET_NORMAL && 
     sheet->active_cell.row >= 0 && sheet->active_cell.col >= 0 &&
     !gtk_sheet_cell_isvisible(sheet, sheet->active_cell.row,
                                      sheet->active_cell.col))
    {
      const gchar *text;

      text = gtk_entry_get_text(GTK_ENTRY(gtk_sheet_get_entry(sheet)));
      if(!text || strlen(text)==0) 
             gtk_sheet_cell_clear(sheet,
                                  sheet->active_cell.row,
                                  sheet->active_cell.col);

      gtk_widget_unmap(sheet->sheet_entry);
    }

  gtk_sheet_position_children(sheet);

  gtk_sheet_range_draw(sheet, NULL);
  size_allocate_column_title_buttons(sheet);
}
	

/* COLUMN RESIZING */
static void                          
draw_xor_vline (GtkSheet * sheet)
{
  GtkWidget *widget;
  
  g_return_if_fail (sheet != NULL);
  
  widget = GTK_WIDGET (sheet);

  gdk_draw_line (widget->window, sheet->xor_gc,  
                 sheet->x_drag,                                       
                 sheet->column_title_area.height,                               
                 sheet->x_drag,                                             
                 sheet->sheet_window_height + 1);
}

/* ROW RESIZING */
static void                          
draw_xor_hline (GtkSheet * sheet)
{
  GtkWidget *widget;
  
  g_return_if_fail (sheet != NULL);
  
  widget = GTK_WIDGET (sheet);

  gdk_draw_line (widget->window, sheet->xor_gc,  
		 sheet->row_title_area.width,
                 sheet->y_drag,                                       
                        
	         sheet->sheet_window_width + 1,                      
                 sheet->y_drag);                                             
}

/* SELECTED RANGE */
static void
draw_xor_rectangle(GtkSheet *sheet, GtkSheetRange range)
{
   gint i;
   GdkRectangle clip_area, area;
   GdkGCValues values;

   area.x=COLUMN_LEFT_XPIXEL(sheet, range.col0);
   area.y=ROW_TOP_YPIXEL(sheet, range.row0);
   area.width=COLUMN_LEFT_XPIXEL(sheet, range.coli)-area.x+
                                        sheet->column[range.coli].width;
   area.height=ROW_TOP_YPIXEL(sheet, range.rowi)-area.y+
                                        sheet->row[range.rowi].height;

   clip_area.x=sheet->row_title_area.width;
   clip_area.y=sheet->column_title_area.height;
   clip_area.width=sheet->sheet_window_width;
   clip_area.height=sheet->sheet_window_height;

   if(!sheet->row_titles_visible) clip_area.x = 0;
   if(!sheet->column_titles_visible) clip_area.y = 0;

   if(area.x<0) {
      area.width=area.width+area.x;
      area.x=0;
   }
   if(area.width>clip_area.width) area.width=clip_area.width+10;
   if(area.y<0) {
      area.height=area.height+area.y;
      area.y=0;
   }
   if(area.height>clip_area.height) area.height=clip_area.height+10;

   clip_area.x--;
   clip_area.y--;
   clip_area.width+=3;
   clip_area.height+=3;

   gdk_gc_get_values(sheet->xor_gc, &values);

   gdk_gc_set_clip_rectangle(sheet->xor_gc, &clip_area);

   for(i=-1;i<=1;++i)
     gdk_draw_rectangle(sheet->sheet_window,
                        sheet->xor_gc,
		        FALSE,
		        area.x+i, area.y+i,
                        area.width-2*i, area.height-2*i);


   gdk_gc_set_clip_rectangle(sheet->xor_gc, NULL);

   gdk_gc_set_foreground(sheet->xor_gc, &values.foreground);

}                      

  
/* this function returns the new width of the column being resized given
 * the column and x position of the cursor; the x cursor position is passed
 * in as a pointer and automaticaly corrected if it's beyond min/max limits */
static guint
new_column_width (GtkSheet * sheet,
		  gint column,
		  gint * x)
{
  gint cx, width;
  GtkRequisition requisition;

  cx = *x;

  requisition.width = sheet->column[column].requisition;

  /* you can't shrink a column to less than its minimum width */
  if (cx < COLUMN_LEFT_XPIXEL (sheet, column) + requisition.width)
    {
      *x = cx = COLUMN_LEFT_XPIXEL (sheet, column) + requisition.width;
    }

  /* don't grow past the end of the window */
  /*
  if (cx > sheet->sheet_window_width)
    {
      *x = cx = sheet->sheet_window_width;
    }
    */
  /* calculate new column width making sure it doesn't end up
   * less than the minimum width */
  width = cx - COLUMN_LEFT_XPIXEL (sheet, column);
  if (width < requisition.width)
    width = requisition.width;

  sheet->column[column].width = width;
  gtk_sheet_recalc_left_xpixels(sheet, column+1);
  sheet->view.coli=COLUMN_FROM_XPIXEL(sheet, sheet->sheet_window_width);
  size_allocate_column_title_buttons (sheet);
  
  return width;
}

/* this function returns the new height of the row being resized given
 * the row and y position of the cursor; the y cursor position is passed
 * in as a pointer and automaticaly corrected if it's beyond min/max limits */
static guint
new_row_height (GtkSheet * sheet,
		gint row,
		gint * y)
{
  GtkRequisition requisition;
  gint cy, height;

  cy = *y;

  requisition.height = sheet->row[row].requisition;

  /* you can't shrink a row to less than its minimum height */
  if (cy < ROW_TOP_YPIXEL (sheet, row) + requisition.height)

    {
      *y = cy = ROW_TOP_YPIXEL (sheet, row) + requisition.height;
    }

  /* don't grow past the end of the window */
  /*
  if (cy > sheet->sheet_window_height)
    {
      *y = cy = sheet->sheet_window_height;
    }
    */
  /* calculate new row height making sure it doesn't end up
   * less than the minimum height */
  height = (cy - ROW_TOP_YPIXEL (sheet, row));
  if (height < requisition.height)
    height = requisition.height;

  sheet->row[row].height = height;
  gtk_sheet_recalc_top_ypixels(sheet, row);
  sheet->view.rowi=ROW_FROM_YPIXEL(sheet, sheet->sheet_window_height-1);
  size_allocate_row_title_buttons (sheet);

  return height;
}

/**
 * gtk_sheet_set_column_width:
 * @sheet: a #GtkSheet.
 * @column: column number.
 * @width: the width of the column.
 *
 * Set column width.
 */ 
void
gtk_sheet_set_column_width (GtkSheet * sheet,
			    gint column,
			    guint width)
{
  guint min_width;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (column < 0 || column > sheet->maxcol)
    return;

  gtk_sheet_column_size_request(sheet, column, &min_width);
  if(width < min_width) return;

  sheet->column[column].width = width;

  gtk_sheet_recalc_left_xpixels(sheet, column+1);

  if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)) && !GTK_SHEET_IS_FROZEN(sheet)){
    size_allocate_column_title_buttons (sheet);
    adjust_scrollbars (sheet);
    gtk_sheet_size_allocate_entry(sheet);
    gtk_sheet_range_draw (sheet, NULL);
  } else

  g_signal_emit (sheet, sheet_signals[CHANGED], 0, -1, column);
  g_signal_emit (sheet, sheet_signals[NEW_COL_WIDTH], 0, column, width);

}

/**
 * gtk_sheet_set_row_height:
 * @sheet: a #GtkSheet.
 * @row: row number.
 * @height: row height(in pixels).
 *
 * Set row height.
 */ 
void
gtk_sheet_set_row_height (GtkSheet * sheet,
			    gint row,
			    guint height)
{
  guint min_height;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if (row < 0 || row > sheet->maxrow)
    return;

  gtk_sheet_row_size_request(sheet, row, &min_height);
  if(height < min_height) return;

  sheet->row[row].height = height;

  gtk_sheet_recalc_top_ypixels(sheet, row+1);

  if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)) && !GTK_SHEET_IS_FROZEN(sheet)){
    size_allocate_row_title_buttons (sheet);
    adjust_scrollbars (sheet);
    gtk_sheet_size_allocate_entry(sheet);
    gtk_sheet_range_draw (sheet, NULL);
  }

  g_signal_emit (sheet, sheet_signals[CHANGED], 0, row, -1);
  g_signal_emit (sheet, sheet_signals[NEW_ROW_HEIGHT], 0, row, height);

}

/**
 * gtk_sheet_add_column:
 * @sheet: a #GtkSheet.
 * @ncols: number of columns to be appended.
 *
 * Append @ncols columns to the end of the sheet. 
 */ 
void
gtk_sheet_add_column(GtkSheet *sheet, guint ncols)
{

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));

 AddColumn(sheet, ncols);
 g_object_notify(G_OBJECT(sheet), "n-columns");

 if(!GTK_WIDGET_REALIZED(sheet)) return;

 adjust_scrollbars(sheet);

 if(sheet->state==GTK_SHEET_ROW_SELECTED) {
    sheet->range.coli+=ncols;
    g_object_notify(G_OBJECT(sheet), "selected-range");
 }

 sheet->old_hadjustment = -1.;
 if(!GTK_SHEET_IS_FROZEN(sheet) && sheet->hadjustment)
      g_signal_emit_by_name (GTK_OBJECT (sheet->hadjustment), "value_changed");
}

/**
 * gtk_sheet_add_row:
 * @sheet: a #GtkSheet.
 * @nrows: number of rows to be appended.
 *
 * Append @nrows rows to the end of the sheet. 
 */ 
void
gtk_sheet_add_row(GtkSheet *sheet, guint nrows)
{

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));

 AddRow(sheet, nrows);
 g_object_notify(G_OBJECT(sheet), "n-rows");

 if(!GTK_WIDGET_REALIZED(sheet)) return;

 if(sheet->state==GTK_SHEET_COLUMN_SELECTED) {
   sheet->range.rowi+=nrows;
   g_object_notify(G_OBJECT(sheet), "selected-range");
 }

 adjust_scrollbars(sheet);

 sheet->old_vadjustment = -1.;
 if(!GTK_SHEET_IS_FROZEN(sheet) && sheet->vadjustment)
      g_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), "value_changed");
}

/**
 * gtk_sheet_insert_rows:
 * @sheet: a #GtkSheet.
 * @row: row number.
 * @nrows: number of rows to be inserted.
 *
 * Insert @nrows rows before the given row and pull right.
 */ 
void
gtk_sheet_insert_rows(GtkSheet *sheet, guint row, guint nrows)
{
 GList *children;
 GtkSheetChild *child;

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));

 if(GTK_WIDGET_REALIZED(sheet))
   gtk_sheet_real_unselect_range(sheet, NULL);

 InsertRow(sheet, row, nrows);
 g_object_notify(G_OBJECT(sheet), "n-rows");

 children = sheet->children;
 while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->attached_to_cell)
        if(child->row >= row) child->row += nrows; 

     children = children->next;
   }

 if(!GTK_WIDGET_REALIZED(sheet)) return;

 if(sheet->state==GTK_SHEET_COLUMN_SELECTED) {
   sheet->range.rowi+=nrows;
   g_object_notify(G_OBJECT(sheet), "selected-range");
 }
 adjust_scrollbars(sheet);
 
 sheet->old_vadjustment = -1.;
 if(!GTK_SHEET_IS_FROZEN(sheet) && sheet->vadjustment)
      g_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), "value_changed");

}

/**
 * gtk_sheet_insert_columns:
 * @sheet: a #GtkSheet.
 * @col: column number.
 * @ncols: number of columns to be inserted.
 *
 * Insert @ncols columns before the given row and pull right.
 */ 
void
gtk_sheet_insert_columns(GtkSheet *sheet, guint col, guint ncols)
{
 GList *children;
 GtkSheetChild *child;

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));

 if(GTK_WIDGET_REALIZED(sheet))
   gtk_sheet_real_unselect_range(sheet, NULL);

 InsertColumn(sheet, col, ncols);
 g_object_notify(G_OBJECT(sheet), "n-columns");

 children = sheet->children;
 while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->attached_to_cell)
        if(child->col >= col) child->col += ncols; 

     children = children->next;
   }

 if(!GTK_WIDGET_REALIZED(sheet)) return;

 if(sheet->state==GTK_SHEET_ROW_SELECTED) {
   sheet->range.coli+=ncols;
   g_object_notify(G_OBJECT(sheet), "selected-range");
 }
 adjust_scrollbars(sheet);

 sheet->old_hadjustment = -1.;
 if(!GTK_SHEET_IS_FROZEN(sheet) && sheet->hadjustment)
      g_signal_emit_by_name (GTK_OBJECT (sheet->hadjustment), "value_changed");

}

/**
 * gtk_sheet_delete_rows:
 * @sheet: a #GtkSheet.
 * @row: row number.
 * @nrows: number of rows to be deleted.
 *
 * Delete @nrows rows starting from @row.
 */ 
void
gtk_sheet_delete_rows(GtkSheet *sheet, guint row, guint nrows)
{
 GList *children;
 GtkSheetChild *child;
 gint irow, icol;
 gboolean veto;

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));

 nrows = MIN(nrows, sheet->maxrow-row+1);

 if(GTK_WIDGET_REALIZED(sheet))
   gtk_sheet_real_unselect_range(sheet, NULL);

 DeleteRow(sheet, row, nrows);
 g_object_notify(G_OBJECT(sheet), "n-rows");

 children = sheet->children;
 while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->attached_to_cell &&
        child->row >= row && child->row < row+nrows){  
              gtk_container_remove(GTK_CONTAINER(sheet), child->widget);
              children = sheet->children;
     } else
        children = children->next;
   }

 children = sheet->children;
 while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->attached_to_cell && child->row > row) child->row -= nrows; 
     children = children->next;
   }

 if(!GTK_WIDGET_REALIZED(sheet)) return;

 irow = sheet->active_cell.row;
 icol = sheet->active_cell.col;

 sheet->active_cell.row = -1;
 sheet->active_cell.col = -1;

/* if(sheet->state == GTK_SHEET_ROW_SELECTED)
*/

 irow = MIN(irow, sheet->maxrow);
 irow = MAX(irow, 0);
 gtk_sheet_click_cell(sheet, irow, icol, &veto);

 gtk_sheet_activate_cell(sheet, sheet->active_cell.row,
                                       sheet->active_cell.col);

 adjust_scrollbars(sheet);

 sheet->old_vadjustment = -1.;
 if(!GTK_SHEET_IS_FROZEN(sheet) && sheet->vadjustment)
      g_signal_emit_by_name (GTK_OBJECT (sheet->vadjustment), "value_changed");

}

/**
 * gtk_sheet_delete_columns:
 * @sheet: a #GtkSheet.
 * @col: column number.
 * @ncols: number of columns to be deleted.
 *
 * Delete @ncols columns starting from @col.
 */ 
void
gtk_sheet_delete_columns(GtkSheet *sheet, guint col, guint ncols)
{
 GList *children;
 GtkSheetChild *child;
 gint irow, icol;
 gboolean veto;

 g_return_if_fail (sheet != NULL);
 g_return_if_fail (GTK_IS_SHEET (sheet));

 ncols = MIN(ncols, sheet->maxcol-col+1);

 if(GTK_WIDGET_REALIZED(sheet))
   gtk_sheet_real_unselect_range(sheet, NULL);

 DeleteColumn(sheet, col, ncols);
 g_object_notify(G_OBJECT(sheet), "n-columns");

 children = sheet->children;
 while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->attached_to_cell &&
        child->col >= col && child->col < col+ncols){  
              gtk_container_remove(GTK_CONTAINER(sheet), child->widget);
              children = sheet->children;
     } else
        children = children->next;
   }

 children = sheet->children;
 while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->attached_to_cell && child->col > col) child->col -= ncols; 
     children = children->next;
   }

 if(!GTK_WIDGET_REALIZED(sheet)) return;

 irow = sheet->active_cell.row;
 icol = sheet->active_cell.col;

 sheet->active_cell.row = -1;
 sheet->active_cell.col = -1;

/* if(sheet->state == GTK_SHEET_COLUMN_SELECTED)
*/

 icol = MIN(icol, sheet->maxcol);
 icol = MAX(icol, 0);
 gtk_sheet_click_cell(sheet, irow, icol, &veto);

 gtk_sheet_activate_cell(sheet, sheet->active_cell.row,
                                       sheet->active_cell.col);

 adjust_scrollbars(sheet);

 sheet->old_hadjustment = -1.;
 if(!GTK_SHEET_IS_FROZEN(sheet) && sheet->hadjustment)
      g_signal_emit_by_name (GTK_OBJECT (sheet->hadjustment), "value_changed");

}

/**
 * gtk_sheet_range_set_background:
 * @sheet: a #GtkSheet.
 * @urange: a #GtkSheetRange.
 * @color: a #GdkColor.
 *
 * Set background color of the given range.
 */ 
void
gtk_sheet_range_set_background(GtkSheet *sheet, const GtkSheetRange *urange, const GdkColor *color)
{
  gint i, j;
  GtkSheetCellAttr attributes;
  GtkSheetRange range;
  GdkColor new_color;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!urange) 
     range = sheet->range;
  else
     range = *urange;

  /* Make sure the color is allocated, otherwise odd results can occur
     when drawing */
  new_color = *color;
  gdk_colormap_alloc_color(gdk_colormap_get_system(), &new_color, TRUE, TRUE);

  for (i=range.row0; i<=range.rowi; i++)
    for (j=range.col0; j<=range.coli; j++){
      gtk_sheet_get_attributes(sheet, i, j, &attributes);
      if(color != NULL)
        attributes.background = new_color;
      else
        attributes.background = sheet->bg_color;
 
      gtk_sheet_set_cell_attributes(sheet, i, j, attributes); 
    }

  range.row0--;
  range.col0--;
  range.rowi++;
  range.coli++;

  if(!GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_range_draw(sheet, &range);

}

/**
 * gtk_sheet_range_set_foreground:
 * @sheet: a #GtkSheet.
 * @urange: a #GtkSheetRange.
 * @color: a #GdkColor.
 *
 * Set foreground color of the given range.
 */ 
void
gtk_sheet_range_set_foreground(GtkSheet *sheet, const GtkSheetRange *urange, const GdkColor *color)
{
  gint i, j;
  GtkSheetCellAttr attributes;
  GtkSheetRange range;
  GdkColor new_color;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!urange) 
     range = sheet->range;
  else
     range = *urange;

  /* Make sure the color is allocated, otherwise odd results can occur
     when drawing */
  new_color = *color;
  gdk_colormap_alloc_color(gdk_colormap_get_system(), &new_color, TRUE, TRUE);

  for (i=range.row0; i<=range.rowi; i++)
    for (j=range.col0; j<=range.coli; j++){
      gtk_sheet_get_attributes(sheet, i, j, &attributes);

      if(color != NULL)
        attributes.foreground = new_color;
      else {
        gdk_color_parse("black", &attributes.foreground);
        gdk_colormap_alloc_color(gdk_colormap_get_system(), &attributes.foreground,
                                 TRUE, TRUE);
      }
      gtk_sheet_set_cell_attributes(sheet, i, j, attributes); 
    }

  if(!GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_range_draw(sheet, &range);

}

/**
 * gtk_sheet_range_set_justification:
 * @sheet: a #GtkSheet.
 * @urange: a #GtkSheetRange.
 * @just: a #GtkJustification : GTK_JUSTIFY_LEFT, RIGHT, CENTER.
 *
 * Set text justification (GTK_JUSTIFY_LEFT, RIGHT, CENTER) of the given range.
 * The default value is GTK_JUSTIFY_LEFT. If autoformat is on, the default justification for numbers is GTK_JUSTIFY_RIGHT.
 */ 
void
gtk_sheet_range_set_justification(GtkSheet *sheet, const GtkSheetRange *urange, 
                                  GtkJustification just)
{
  gint i, j;
  GtkSheetCellAttr attributes;
  GtkSheetRange range;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!urange) 
     range = sheet->range;
  else
     range = *urange;

  for (i=range.row0; i<=range.rowi; i++)
    for (j=range.col0; j<=range.coli; j++){
      gtk_sheet_get_attributes(sheet, i, j, &attributes);
      attributes.justification = just;
      gtk_sheet_set_cell_attributes(sheet, i, j, attributes); 
    }

  range.col0 = sheet->view.col0;    
  range.coli = sheet->view.coli;    

  if(!GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_range_draw(sheet, &range);

}

/**
 * gtk_sheet_column_set_justification:
 * @sheet: a #GtkSheet.
 * @col: column number
 * @just: a #GtkJustification : GTK_JUSTIFY_LEFT, RIGHT, CENTER
 *
 * Set column justification (GTK_JUSTIFY_LEFT, RIGHT, CENTER).
 * The default value is GTK_JUSTIFY_LEFT. If autoformat is on, the default justification for numbers is GTK_JUSTIFY_RIGHT.
 */ 
void
gtk_sheet_column_set_justification(GtkSheet *sheet, gint col, 
                                   GtkJustification justification)
{
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(col > sheet->maxcol) return;

  sheet->column[col].justification = justification;
  
  if(GTK_WIDGET_REALIZED(sheet) && !GTK_SHEET_IS_FROZEN(sheet) &&
     col >= MIN_VISIBLE_COLUMN(sheet) && col <= MAX_VISIBLE_COLUMN(sheet))
          gtk_sheet_range_draw(sheet, NULL);

}

/**
 * gtk_sheet_range_set_editable:
 * @sheet: a #GtkSheet.
 * @urange: a #GtkSheetRange
 * @editable: TRUE or FALSE
 *
 * Set if cell contents can be edited or not in the given range.
 */ 
void
gtk_sheet_range_set_editable(GtkSheet *sheet, const GtkSheetRange *urange, gboolean editable)
{
  gint i, j;
  GtkSheetCellAttr attributes;
  GtkSheetRange range;
 
  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!urange) 
     range = sheet->range;
  else
     range = *urange;

  for (i=range.row0; i<=range.rowi; i++)
    for (j=range.col0; j<=range.coli; j++){
      gtk_sheet_get_attributes(sheet, i, j, &attributes);
      attributes.is_editable = editable;
      gtk_sheet_set_cell_attributes(sheet, i, j, attributes); 
    }
 
  if(!GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_range_draw(sheet, &range);

}

/**
 * gtk_sheet_range_set_visible:
 * @sheet: a #GtkSheet.
 * @urange: a #GtkSheetRange.
 * @visible: TRUE or FALSE.
 *
 * Set if cell contents are visible or not in the given range: accepted values are TRUE or FALSE. 
 */ 
void
gtk_sheet_range_set_visible(GtkSheet *sheet, const GtkSheetRange *urange, gboolean visible)
{
  gint i, j;
  GtkSheetCellAttr attributes;
  GtkSheetRange range;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!urange) 
     range = sheet->range;
  else
     range = *urange;

  for (i=range.row0; i<=range.rowi; i++)
    for (j=range.col0; j<=range.coli; j++){
      gtk_sheet_get_attributes(sheet, i, j, &attributes);
      attributes.is_visible=visible;
      gtk_sheet_set_cell_attributes(sheet, i, j, attributes); 
    }
 
  if(!GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_range_draw(sheet, &range);

}

/**
 * gtk_sheet_range_set_border:
 * @sheet: a #GtkSheet.
 * @urange: a #GtkSheetRange where we set border style.
 * @mask: CELL_LEFT_BORDER, CELL_RIGHT_BORDER, CELL_TOP_BORDER,CELL_BOTTOM_BORDER
 * @width: width of the border line in pixels
 * @line_style: GdkLineStyle for the border line
 *
 * Set cell border style in the given range. 
 */ 
void
gtk_sheet_range_set_border(GtkSheet *sheet, const GtkSheetRange *urange, gint mask, 
guint width, gint line_style)
{
  gint i, j;
  GtkSheetCellAttr attributes;
  GtkSheetRange range;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!urange) 
     range = sheet->range;
  else
     range = *urange;

  for (i=range.row0; i<=range.rowi; i++)
    for (j=range.col0; j<=range.coli; j++){
      gtk_sheet_get_attributes(sheet, i, j, &attributes);
      attributes.border.mask = mask;
      attributes.border.width = width;
      attributes.border.line_style=line_style;
      attributes.border.cap_style=GDK_CAP_NOT_LAST;
      attributes.border.join_style=GDK_JOIN_MITER;
      gtk_sheet_set_cell_attributes(sheet, i, j, attributes);      
    }

  range.row0--; 
  range.col0--; 
  range.rowi++; 
  range.coli++; 

  if(!GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_range_draw(sheet, &range);

}

/**
 * gtk_sheet_range_set_border_color:
 * @sheet: a #GtkSheet.
 * @urange: a #GtkSheetRange where we set border color.
 * @color: a #GdkColor.
 *
 * Set border color for the given range.
 */ 
void
gtk_sheet_range_set_border_color(GtkSheet *sheet, const GtkSheetRange *urange, const GdkColor *color)
{
  gint i, j;
  GtkSheetCellAttr attributes;
  GtkSheetRange range;
  GdkColor new_color;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!urange) 
     range = sheet->range;
  else
     range = *urange;

  new_color = *color;
  gdk_colormap_alloc_color(gdk_colormap_get_system(), &new_color, TRUE, TRUE);
  
  for (i=range.row0; i<=range.rowi; i++)
    for (j=range.col0; j<=range.coli; j++){
      gtk_sheet_get_attributes(sheet, i, j, &attributes);
      attributes.border.color = new_color;
      gtk_sheet_set_cell_attributes(sheet, i, j, attributes); 
    }
 
  if(!GTK_SHEET_IS_FROZEN(sheet))
      gtk_sheet_range_draw(sheet, &range);

}

/**
 * gtk_sheet_range_set_font:
 * @sheet: a #GtkSheet.
 * @urange: a #GtkSheetRange where we set font.
 * @font: a #PangoFontDescription.
 *
 * Set font for the given range.
 */ 
void
gtk_sheet_range_set_font(GtkSheet *sheet, const GtkSheetRange *urange, PangoFontDescription *font)
{
  gint i, j;
  gint font_height;
  GtkSheetCellAttr attributes;
  GtkSheetRange range;
  PangoContext *context;
  PangoFontMetrics *metrics;

  g_return_if_fail (sheet != NULL);
  g_return_if_fail (GTK_IS_SHEET (sheet));

  if(!urange) 
     range = sheet->range;
  else
     range = *urange;

  gtk_sheet_freeze(sheet);

  context = gtk_widget_get_pango_context(GTK_WIDGET(sheet)); 
  metrics = pango_context_get_metrics(context,
                                font,
                                pango_context_get_language(context)); 
  font_height = pango_font_metrics_get_descent(metrics) + 
                pango_font_metrics_get_ascent(metrics);
  font_height = PANGO_PIXELS(font_height) + 2*CELLOFFSET;

  for (i=range.row0; i<=range.rowi; i++)
    for (j=range.col0; j<=range.coli; j++){
      gtk_sheet_get_attributes(sheet, i, j, &attributes);
      /* Free the previous font description */
      if (attributes.font_desc != NULL)
         pango_font_description_free(attributes.font_desc);

      attributes.font_desc = pango_font_description_copy(font);

      if(font_height > sheet->row[i].height){
          sheet->row[i].height = font_height;
          gtk_sheet_recalc_top_ypixels(sheet, i);
      }

      gtk_sheet_set_cell_attributes(sheet, i, j, attributes);  
    }

  gtk_sheet_thaw(sheet);
  pango_font_metrics_unref(metrics);
}


/* 
   The values in the attributes parameter will be copied to the cell. As the 
   attributes contain a pointer to a PangoFontDescription structure, you have
   to make sure that the previous pointer is freed before calling this function
   and that the structure pointed by the new attributes is maintained throught
   the attributes lifetime.
 */
static void
gtk_sheet_set_cell_attributes(GtkSheet *sheet, gint row, gint col, GtkSheetCellAttr attributes)
{
  GtkSheetCell **cell;

  if(row > sheet->maxrow || col >sheet->maxcol) return;

  CheckBounds(sheet, row, col);

  cell = &sheet->data[row][col];

  if(*cell==NULL){
   (*cell) = gtk_sheet_cell_new();
   (*cell)->row = row;
   (*cell)->col = col;
  }

  if((*cell)->attributes == NULL) 
      (*cell)->attributes = g_new(GtkSheetCellAttr, 1);

  *((*cell)->attributes) = attributes;
}

/**
 * gtk_sheet_get_attributes:
 * @sheet: a #GtkSheet.
 * @row: row number
 * @col: column number
 * @attributes: #GtkSheetCellAttr of the given range
 *
 * Gett cell attributes of the given cell. 
 * Return value: TRUE means that the cell is currently allocated.
 */ 
gboolean
gtk_sheet_get_attributes(GtkSheet *sheet, gint row, gint col, GtkSheetCellAttr *attributes)
{
 GtkSheetCell **cell = NULL;

 g_return_val_if_fail (sheet != NULL, FALSE);
 g_return_val_if_fail (GTK_IS_SHEET (sheet), FALSE);

 if(row < 0 || col < 0) return FALSE;

 if(row > sheet->maxallocrow || col > sheet->maxalloccol){
    init_attributes(sheet, col, attributes);
    return FALSE;
 }

 if(row <= sheet->maxallocrow && col <= sheet->maxalloccol){
    if(sheet->data[row] && sheet->data[row][col])
      cell = &sheet->data[row][col];
    if(cell == NULL || *cell == NULL){
      init_attributes(sheet, col, attributes);
      return FALSE;
    } else
      if((*cell)->attributes == NULL){
         init_attributes(sheet, col, attributes);
         return FALSE;
      }else{
         *attributes = *(sheet->data[row][col]->attributes);
         if(sheet->column[col].justification != GTK_JUSTIFY_FILL)
              attributes->justification = sheet->column[col].justification;
      }
 }   
 
 return TRUE;
}

static void
init_attributes(GtkSheet *sheet, gint col, GtkSheetCellAttr *attributes)
{
 /* DEFAULT VALUES */    
 attributes->foreground = GTK_WIDGET(sheet)->style->black;
 attributes->background = sheet->bg_color;
 if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sheet))){
   GdkColormap *colormap;
   colormap=gdk_colormap_get_system();
   gdk_color_parse("black", &attributes->foreground);
   gdk_colormap_alloc_color(colormap, &attributes->foreground, TRUE, TRUE);
   attributes->background = sheet->bg_color;
 }
 attributes->justification = sheet->column[col].justification;
 attributes->border.width = 0;
 attributes->border.line_style = GDK_LINE_SOLID;
 attributes->border.cap_style = GDK_CAP_NOT_LAST;
 attributes->border.join_style = GDK_JOIN_MITER;
 attributes->border.mask = 0;
 attributes->border.color = GTK_WIDGET(sheet)->style->black;
 attributes->is_editable = TRUE;
 attributes->is_visible = TRUE;
 attributes->font_desc = NULL;

}       
 
/**********************************************************************
 * Memory allocation routines: 
 * AddRow & AddColumn allocate memory for GtkSheetColumn & GtkSheetRow structs.
 * InsertRow 
 * InsertColumn
 * DeleteRow
 * DeleteColumn
 * GrowSheet allocates memory for the sheet cells contents using an array of 
 * pointers. Alternative to this could be a linked list or a hash table.
 * CheckBounds checks whether the given cell is currently allocated or not. 
 * If not, it calls to GrowSheet.
 **********************************************************************/

static gint
AddColumn(GtkSheet *tbl, gint ncols)
{
   gint i;

   if(ncols == -1 && tbl->maxcol == 0)
     {
       ncols = 1;
     }
   else
     {
       tbl->maxcol += ncols;
       tbl->column = (GtkSheetColumn *)g_realloc(tbl->column,(tbl->maxcol+1)*
                                                 sizeof(GtkSheetColumn));
     }

   for(i=tbl->maxcol-ncols+1; i<= tbl->maxcol; i++){
        tbl->column[i].width=DEFAULT_COLUMN_WIDTH;
	tbl->column[i].button.label=NULL;
	tbl->column[i].button.child=NULL;
        tbl->column[i].button.state=GTK_STATE_NORMAL;
        tbl->column[i].button.justification=GTK_JUSTIFY_CENTER;
        tbl->column[i].button.label_visible = TRUE;
        tbl->column[i].name=NULL;
        tbl->column[i].is_visible=TRUE;
        tbl->column[i].is_sensitive=TRUE;
        tbl->column[i].left_text_column=i;
        tbl->column[i].right_text_column=i;
        tbl->column[i].justification=GTK_JUSTIFY_FILL;
        tbl->column[i].requisition=DEFAULT_COLUMN_WIDTH;
        if(i>0)
        {
           tbl->column[i].left_text_column=tbl->column[i-1].left_text_column;
           tbl->column[i].left_xpixel=tbl->column[i-1].left_xpixel +
                                     tbl->column[i-1].width;
	}
        else
        {
	   tbl->column[i].left_xpixel=tbl->row_title_area.width;
	   if(!tbl->row_titles_visible) 
                        tbl->column[i].left_xpixel=0;
        }
   }
   return TRUE;
}

static gint
AddRow(GtkSheet *tbl, gint nrows)
{
   gint i;

   if(nrows == -1 && tbl->maxrow == 0)
     {
       nrows = 1;
     }
   else
     {
       tbl->maxrow += nrows;
       tbl->row = (GtkSheetRow *)g_realloc(tbl->row,(tbl->maxrow+1)*
                                            sizeof(GtkSheetRow));
     }

   for(i=tbl->maxrow-nrows+1; i<= tbl->maxrow; i++){
        tbl->row[i].requisition=tbl->row[i].height=DEFAULT_ROW_HEIGHT(GTK_WIDGET(tbl));
	tbl->row[i].button.label=NULL;
	tbl->row[i].button.child=NULL;
        tbl->row[i].button.state=GTK_STATE_NORMAL;
        tbl->row[i].button.justification=GTK_JUSTIFY_CENTER;
        tbl->row[i].button.label_visible = TRUE;
        tbl->row[i].name=NULL;
        tbl->row[i].is_visible=TRUE;
        tbl->row[i].is_sensitive=TRUE;
        if(i>0)
           tbl->row[i].top_ypixel=tbl->row[i-1].top_ypixel+tbl->row[i-1].height;
	else
        {
	   tbl->row[i].top_ypixel=tbl->column_title_area.height;
           if(!tbl->column_titles_visible) 
                        tbl->row[i].top_ypixel=0;
        } 
   }
   return TRUE;
}

static gint
InsertRow(GtkSheet *tbl, gint row, gint nrows)
{
  GtkSheetCell **pp;
  gint i,j;
  GtkSheetCell **auxdata;
  GtkSheetRow auxrow;

  AddRow(tbl,nrows);

  for(i=tbl->maxrow; i>=row+nrows; i--){
    auxrow = tbl->row[i];  
    tbl->row[i]=tbl->row[i-nrows];    
    tbl->row[i].is_visible=tbl->row[i-nrows].is_visible;
    tbl->row[i].is_sensitive=tbl->row[i-nrows].is_sensitive;
    if(auxrow.is_visible) 
               tbl->row[i].top_ypixel+=nrows*DEFAULT_ROW_HEIGHT(GTK_WIDGET(tbl));
    tbl->row[i-nrows]=auxrow;
  }

  if(row <= tbl->maxallocrow){
   
    GrowSheet(tbl,nrows,0);

    for(i=tbl->maxallocrow; i>=row+nrows; i--){
      auxdata = tbl->data[i];
      tbl->data[i]=tbl->data[i-nrows];

      pp= tbl->data[i];
      for(j=0; j<=tbl->maxalloccol; j++,pp++){
        if(*pp!=(GtkSheetCell *)NULL)
                                    (*pp)->row=i;
      
      }
      tbl->data[i-nrows]=auxdata;
    }
  }
  gtk_sheet_recalc_top_ypixels(tbl, 0);
  return TRUE;
}  

static gint 
InsertColumn(GtkSheet *tbl, gint col, gint ncols)
{
  gint i,j;
  GtkSheetColumn auxcol;

  AddColumn(tbl,ncols);

  for(i=tbl->maxcol; i>=col+ncols; i--){
    auxcol = tbl->column[i];
    tbl->column[i]=tbl->column[i-ncols];
    tbl->column[i].is_visible=tbl->column[i-ncols].is_visible;
    tbl->column[i].is_sensitive=tbl->column[i-ncols].is_sensitive;
    tbl->column[i].left_text_column=tbl->column[i-ncols].left_text_column;
    tbl->column[i].right_text_column=tbl->column[i-ncols].right_text_column;
    tbl->column[i].justification=tbl->column[i-ncols].justification;
    if(auxcol.is_visible) tbl->column[i].left_xpixel+=ncols*DEFAULT_COLUMN_WIDTH;
    tbl->column[i-ncols]=auxcol;
  }

  if(col <= tbl->maxalloccol){
   
    GrowSheet(tbl,0,ncols);

    for(i=0; i<=tbl->maxallocrow; i++){
      for(j=tbl->maxalloccol; j>=col+ncols; j--){
        gtk_sheet_real_cell_clear(tbl, i, j, TRUE);
        tbl->data[i][j]=tbl->data[i][j-ncols];
        if(tbl->data[i][j]) tbl->data[i][j]->col=j;
        tbl->data[i][j-ncols]=NULL;
      }
    }
  }
  gtk_sheet_recalc_left_xpixels(tbl, 0);
  return TRUE;
}

static gint
DeleteRow(GtkSheet *tbl, gint row, gint nrows)
{
  GtkSheetCell **auxdata = NULL;
  gint i,j;

  if(nrows <= 0 || row > tbl->maxrow) return TRUE;

  nrows=MIN(nrows,tbl->maxrow-row+1);

  for(i=row; i<row+nrows; i++){
    if(tbl->row[i].name){
            g_free(tbl->row[i].name);
            tbl->row[i].name = NULL;
    }
    if(tbl->row[i].button.label){
            g_free(tbl->row[i].button.label);
            tbl->row[i].button.label = NULL;
    }
  }           

  for(i=row; i<=tbl->maxrow-nrows; i++){
    if(i+nrows <= tbl->maxrow){ 
      tbl->row[i]=tbl->row[i+nrows];
    }
  }

  if(row <= tbl->maxallocrow){

    for(i=row; i<=tbl->maxrow-nrows; i++){
      if(i<=tbl->maxallocrow){
        auxdata=tbl->data[i];
        for(j=0; j<=tbl->maxalloccol; j++){
              gtk_sheet_real_cell_clear(tbl, i, j, TRUE);
        }
      }
      if(i+nrows<=tbl->maxallocrow){
        tbl->data[i]=tbl->data[i+nrows];
        tbl->data[i+nrows]=auxdata;
        for(j=0; j<=tbl->maxalloccol; j++){
            if(tbl->data[i][j]) tbl->data[i][j]->row=i;
        }
      }
    }

    for(i=tbl->maxrow-nrows+1; i<=tbl->maxallocrow; i++){
           if(i > 0 && tbl->data[i]){
                g_free(tbl->data[i]);
                tbl->data[i] = NULL;
           }
    }

    tbl->maxallocrow-=MIN(nrows,tbl->maxallocrow-row+1);
    tbl->maxallocrow = MIN(tbl->maxallocrow, tbl->maxrow);

  }

  tbl->maxrow-=nrows;
  gtk_sheet_recalc_top_ypixels(tbl, 0);
  return TRUE;
} 

static gint
DeleteColumn(GtkSheet *tbl, gint column, gint ncols)
{
  gint i,j;
  GtkSheetColumn auxcol;

  ncols = MIN(ncols,tbl->maxcol-column+1);

  if(ncols <= 0 || column > tbl->maxcol) return TRUE;

  for(i=column; i<column+ncols; i++){
    auxcol=tbl->column[i];
    if(tbl->column[i].name){
             g_free(tbl->column[i].name);
             tbl->column[i].name = NULL;
    }
    if(tbl->column[i].button.label){
             g_free(tbl->column[i].button.label);
             tbl->column[i].button.label = NULL;
    }
  }

  for(i=column; i<=tbl->maxcol-ncols; i++){
    if(i+ncols <= tbl->maxcol){
      tbl->column[i]=tbl->column[i+ncols];    
    } 
  }

  if(column <= tbl->maxalloccol){

    for(i=column; i<=tbl->maxcol-ncols; i++){
      if(i<=tbl->maxalloccol){
        for(j=0; j<=tbl->maxallocrow; j++){
              gtk_sheet_real_cell_clear(tbl, j, i, TRUE);
              if(i+ncols <= tbl->maxalloccol){
                  tbl->data[j][i] = tbl->data[j][i+ncols];
                  tbl->data[j][i+ncols] = NULL;
	          if(tbl->data[j][i]) tbl->data[j][i]->col=i;
              }
        }
      }

    }

    tbl->maxalloccol-=MIN(ncols,tbl->maxalloccol-column+1);
    tbl->maxalloccol = MIN(tbl->maxalloccol, tbl->maxcol);
  }
  tbl->maxcol-=ncols;
  gtk_sheet_recalc_left_xpixels(tbl, 0);
  return TRUE;
}  

static gint
GrowSheet(GtkSheet *tbl, gint newrows, gint newcols)
{
  gint i,j;
  gint inirow, inicol;

  inirow = tbl->maxallocrow + 1;  
  inicol = tbl->maxalloccol + 1;  

  tbl->maxalloccol = tbl->maxalloccol + newcols;
  tbl->maxallocrow = tbl->maxallocrow + newrows;

  if(newrows>0){
      tbl->data = (GtkSheetCell***)
                 g_realloc(tbl->data,(tbl->maxallocrow+1)*sizeof(GtkSheetCell **)+sizeof(double));

      for(i=inirow; i <= tbl->maxallocrow; i++){
        tbl->data[i] = (GtkSheetCell **) \
                       g_malloc((tbl->maxcol+1)*sizeof(GtkSheetCell *)+sizeof(double));
        for(j=0; j<inicol; j++) {
          tbl->data[i][j] = NULL;
        }
      }
          
  }

  if(newcols>0){
      for(i=0; i <= tbl->maxallocrow; i++) {
        tbl->data[i] = (GtkSheetCell **) \
                       g_realloc(tbl->data[i],(tbl->maxalloccol+1)*sizeof(GtkSheetCell *)+sizeof(double));
        for(j=inicol; j <= tbl->maxalloccol; j++) {
          tbl->data[i][j] = NULL;
	}
      }
  }

  return(0);
}	   

static gint
CheckBounds(GtkSheet *tbl, gint row, gint col)
{
  gint newrows=0,newcols=0;

  if(col>tbl->maxalloccol) newcols=col-tbl->maxalloccol;
  if(row>tbl->maxallocrow) newrows=row-tbl->maxallocrow;
  if(newrows>0 || newcols>0) GrowSheet(tbl, newrows, newcols);
  return(0);
} 

/********************************************************************
 * Container Functions:
 * gtk_sheet_add
 * gtk_sheet_put
 * gtk_sheet_attach
 * gtk_sheet_remove
 * gtk_sheet_move_child
 * gtk_sheet_position_child
 * gtk_sheet_position_children 
 * gtk_sheet_realize_child
 * gtk_sheet_get_child_at
 ********************************************************************/ 

/**
 * gtk_sheet_put:
 * @sheet: a #GtkSheet.
 * @child: GtkWidget to be put
 * @x: x coordinate where we put the widget
 * @y: y coordinate where we put the widget
 *
 * Add widgets to the sheet.
 * The widget is floating in one given position (x,y) regardless of the configurations of rows/columns. 
 * This means that cells do not resize depending on the widgets' size. 
 * You can resize it yourself or use gtk_sheet_attach_*()
 * You may remove it with gtk_container_remove(GTK_CONTAINER(sheet), GtkWidget *child); 
 *
 * Return value: TRUE means that the cell is currently allocated.
 */ 
GtkSheetChild *
gtk_sheet_put(GtkSheet *sheet, GtkWidget *child, gint x, gint y)
{
  GtkRequisition child_requisition;
  GtkSheetChild *child_info;

  g_return_val_if_fail(sheet != NULL, NULL);
  g_return_val_if_fail(GTK_IS_SHEET(sheet), NULL);
  g_return_val_if_fail(child != NULL, NULL);
  g_return_val_if_fail(child->parent == NULL, NULL);

  child_info = g_new (GtkSheetChild, 1);
  child_info->widget = child;
  child_info->x = x;  
  child_info->y = y;
  child_info->attached_to_cell = FALSE;
  child_info->floating = TRUE;
  child_info->xpadding = child_info->ypadding = 0;
  child_info->xexpand = child_info->yexpand = FALSE;
  child_info->xshrink = child_info->yshrink = FALSE;
  child_info->xfill = child_info->yfill = FALSE;

  sheet->children = g_list_append(sheet->children, child_info);

  gtk_widget_set_parent (child, GTK_WIDGET(sheet));

  gtk_widget_size_request(child, &child_requisition);

  if (GTK_WIDGET_VISIBLE(GTK_WIDGET(sheet)))
    {
       if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)) && 
          (!GTK_WIDGET_REALIZED(child) || GTK_WIDGET_NO_WINDOW(child)))
        gtk_sheet_realize_child(sheet, child_info);

       if(GTK_WIDGET_MAPPED(GTK_WIDGET(sheet)) && 
          !GTK_WIDGET_MAPPED(child))
        gtk_widget_map(child);
    }

  gtk_sheet_position_child(sheet, child_info);

/* This will avoid drawing on the titles */

  if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)))
   {
      if(sheet->row_titles_visible)
             gdk_window_show(sheet->row_title_window);
      if(sheet->column_titles_visible)
             gdk_window_show(sheet->column_title_window);
   }

  return (child_info);
}

/**
 * gtk_sheet_attach_floating:
 * @sheet: a #GtkSheet.
 * @widget: #GtkWidget to be put
 * @row: row number
 * @col: column number
 *
 * The widget is attached to the top-left corner of a cell (row,column) and moves with it when you change width,
 * height, or you delete of add row/columns
 */ 
void
gtk_sheet_attach_floating       (GtkSheet *sheet,
                                 GtkWidget *widget,
                                 gint row, gint col)
{
  GdkRectangle area;
  GtkSheetChild *child;

  if(row < 0 || col < 0){
    gtk_sheet_button_attach(sheet, widget, row, col);
    return;
  }

  gtk_sheet_get_cell_area(sheet, row, col, &area);
  child = gtk_sheet_put(sheet, widget, area.x, area.y);
  child->attached_to_cell = TRUE;
  child->row = row;
  child->col = col;
}

/**
 * gtk_sheet_attach_default:
 * @sheet: a #GtkSheet.
 * @widget: #GtkWidget to be put
 * @row: row number
 * @col: column number
 *
 * Attaches a child widget to the given cell with the 0,0 alignments.
 * Works basically like gtk_table_attach, with the same options, the widget is confined in the cell, and whether it fills the 
 * cell, expands with it, or shrinks with it, depending on the options. 
 * The child is reallocated each time the column or row changes, keeping attached to the same cell.
 * It's in fact gtk_sheet_attach() with GTK_EXPAND set.
 */ 
void
gtk_sheet_attach_default        (GtkSheet *sheet,
                                 GtkWidget *widget,
                                 gint row, gint col)
{
  if(row < 0 || col < 0){
    gtk_sheet_button_attach(sheet, widget, row, col);
    return;
  }

  gtk_sheet_attach(sheet, widget, row, col, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);
}

/**
 * gtk_sheet_attach:
 * @sheet: a #GtkSheet.
 * @widget: #GtkWidget to be put
 * @row: row number
 * @col: column number
 * @xoptions: if set GTK_EXPAND cell will expand/shrink on x direction
 * @yoptions: if set GTK_EXPAND cell will expand/shrink on y direction
 * @xpadding: x coordinate of the alignment
 * @ypadding: y coordinate of the alignment
 * 
 * Attaches a child widget to the given cell with the given alignments.
 * Works basically like gtk_table_attach, with the same options, the widget is confined in the cell, and whether it fills the
 * cell, expands with it, or shrinks with it, depending on the options , if GTK_EXPAND is set. 
 * The child is reallocated each time the column or row changes, keeping attached to the same cell.
 */ 
void
gtk_sheet_attach        (GtkSheet *sheet,
                         GtkWidget *widget,
                         gint row, gint col,
                         gint xoptions,
                         gint yoptions,
                         gint xpadding,
                         gint ypadding)
{
  GdkRectangle area;
  GtkSheetChild *child = NULL;

  if(row < 0 || col < 0){
    gtk_sheet_button_attach(sheet, widget, row, col);
    return;
  }

  child = g_new0(GtkSheetChild, 1);
  child->attached_to_cell = TRUE;
  child->floating = FALSE;
  child->widget = widget;
  child->row = row;
  child->col = col;
  child->xpadding = xpadding;
  child->ypadding = ypadding;
  child->xexpand = (xoptions & GTK_EXPAND) != 0;
  child->yexpand = (yoptions & GTK_EXPAND) != 0;
  child->xshrink = (xoptions & GTK_SHRINK) != 0;
  child->yshrink = (yoptions & GTK_SHRINK) != 0;
  child->xfill = (xoptions & GTK_FILL) != 0;
  child->yfill = (yoptions & GTK_FILL) != 0;

  sheet->children = g_list_append(sheet->children, child);

  gtk_sheet_get_cell_area(sheet, row, col, &area);

  child->x = area.x + child->xpadding;
  child->y = area.y + child->ypadding;

  if (GTK_WIDGET_VISIBLE(GTK_WIDGET(sheet)))
    {
       if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)) &&
          (!GTK_WIDGET_REALIZED(widget) || GTK_WIDGET_NO_WINDOW(widget)))
        gtk_sheet_realize_child(sheet, child);

       if(GTK_WIDGET_MAPPED(GTK_WIDGET(sheet)) &&
          !GTK_WIDGET_MAPPED(widget))
        gtk_widget_map(widget);
    }

  gtk_sheet_position_child(sheet, child);

/* This will avoid drawing on the titles */

  if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)))
   {
      if(GTK_SHEET_ROW_TITLES_VISIBLE(sheet))
             gdk_window_show(sheet->row_title_window);
      if(GTK_SHEET_COL_TITLES_VISIBLE(sheet))
             gdk_window_show(sheet->column_title_window);
   }

}

/**
 * gtk_sheet_button_attach:
 * @sheet: a #GtkSheet.
 * @widget: #GtkWidget to be put
 * @row: row number
 * @col: column number
 *
 * Button attach works like cell attach but for the buttons.
 */ 
void
gtk_sheet_button_attach		(GtkSheet *sheet, 
				 GtkWidget *widget, 
				 gint row, gint col)
{
  GtkSheetButton *button;
  GtkSheetChild *child;
  GtkRequisition button_requisition;

  if(row >= 0 && col >= 0) return;
  if(row < 0 && col < 0) return;

  child = g_new (GtkSheetChild, 1);
  child->widget = widget;
  child->x = 0;  
  child->y = 0;  
  child->attached_to_cell = TRUE;
  child->floating = FALSE;
  child->row = row;
  child->col = col;
  child->xpadding = child->ypadding = 0;
  child->xshrink = child->yshrink = FALSE;
  child->xfill = child->yfill = FALSE;

  if(row == -1){
     button = &sheet->column[col].button;
     button->child = child;
  }
  else
  {
     button = &sheet->row[row].button;
     button->child = child;
  }

  sheet->children = g_list_append(sheet->children, child);

  gtk_sheet_button_size_request(sheet, button, &button_requisition);

  if(row == -1){
       if(button_requisition.height > sheet->column_title_area.height)
             sheet->column_title_area.height = button_requisition.height; 
       if(button_requisition.width > sheet->column[col].width)
             sheet->column[col].width = button_requisition.width; 
  }

  if(col == -1){
       if(button_requisition.width > sheet->row_title_area.width)
             sheet->row_title_area.width = button_requisition.width; 
       if(button_requisition.height > sheet->row[row].height)
             sheet->row[row].height = button_requisition.height; 
  }

  if (GTK_WIDGET_VISIBLE(GTK_WIDGET(sheet)))
    {
       if(GTK_WIDGET_REALIZED(GTK_WIDGET(sheet)) && 
          (!GTK_WIDGET_REALIZED(widget) || GTK_WIDGET_NO_WINDOW(widget)))
        gtk_sheet_realize_child(sheet, child);

       if(GTK_WIDGET_MAPPED(GTK_WIDGET(sheet)) && 
          !GTK_WIDGET_MAPPED(widget))
        gtk_widget_map(widget);
    }

  if(row == -1) size_allocate_column_title_buttons(sheet);
  if(col == -1) size_allocate_row_title_buttons(sheet);

}

static void
label_size_request(GtkSheet *sheet, gchar *label, GtkRequisition *req)
{
  gchar *words;
  gchar word[1000];
  gint n = 0;
  gint row_height = DEFAULT_ROW_HEIGHT(GTK_WIDGET(sheet)) - 2*CELLOFFSET + 2;

  req->height = 0;
  req->width = 0;
  words=label;

  while(words && *words != '\0'){
    if(*words == '\n' || *(words+1) == '\0'){
      req->height += row_height;

      word[n] = '\0';
      req->width = MAX(req->width, STRING_WIDTH(GTK_WIDGET(sheet), GTK_WIDGET(sheet)->style->font_desc, word));
      n = 0;
    } else {
      word[n++] = *words;
    }
    words++;
  }
 
  if(n > 0) req->height -= 2;
}

static void
gtk_sheet_button_size_request	(GtkSheet *sheet,
                                 GtkSheetButton *button, 
                                 GtkRequisition *button_requisition)
{
  GtkRequisition requisition;
  GtkRequisition label_requisition;

  if(gtk_sheet_autoresize(sheet) && button->label && strlen(button->label) > 0){
     label_size_request(sheet, button->label, &label_requisition);
     label_requisition.width += 2*CELLOFFSET;
     label_requisition.height += 2*CELLOFFSET;
  } else {
     label_requisition.height = DEFAULT_ROW_HEIGHT(GTK_WIDGET(sheet));
     label_requisition.width = COLUMN_MIN_WIDTH;
  } 

  if(button->child)
  {
     gtk_widget_size_request(button->child->widget, &requisition);
     requisition.width += 2*button->child->xpadding;
     requisition.height += 2*button->child->ypadding;
     requisition.width += 2*sheet->button->style->xthickness;
     requisition.height += 2*sheet->button->style->ythickness;
  }
  else
  {
     requisition.height = DEFAULT_ROW_HEIGHT(GTK_WIDGET(sheet));
     requisition.width = COLUMN_MIN_WIDTH;
  }

  *button_requisition = requisition;
  button_requisition->width = MAX(requisition.width, label_requisition.width);
  button_requisition->height = MAX(requisition.height, label_requisition.height);
 
}

static void
gtk_sheet_row_size_request      (GtkSheet *sheet,
                                 gint row,
                                 guint *requisition)
{
  GtkRequisition button_requisition;
  GList *children;

  gtk_sheet_button_size_request(sheet, &sheet->row[row].button, &button_requisition);

  *requisition = button_requisition.height;

  children = sheet->children;
  while(children){
    GtkSheetChild *child = (GtkSheetChild *)children->data;
    GtkRequisition child_requisition;

    if(child->attached_to_cell && child->row == row && child->col != -1 && !child->floating && !child->yshrink){
      gtk_widget_get_child_requisition(child->widget, &child_requisition);

      if(child_requisition.height + 2 * child->ypadding > *requisition)
        *requisition = child_requisition.height + 2 * child->ypadding;
    }
    children = children->next;
  }

  sheet->row[row].requisition = *requisition;
}

static void
gtk_sheet_column_size_request   (GtkSheet *sheet,
                                 gint col,
                                 guint *requisition)
{
  GtkRequisition button_requisition;
  GList *children;

  gtk_sheet_button_size_request(sheet, &sheet->column[col].button, &button_requisition);

  *requisition = button_requisition.width;

  children = sheet->children;
  while(children){
    GtkSheetChild *child = (GtkSheetChild *)children->data;
    GtkRequisition child_requisition;

    if(child->attached_to_cell && child->col == col && child->row != -1 && !child->floating && !child->xshrink){
      gtk_widget_get_child_requisition(child->widget, &child_requisition);

      if(child_requisition.width + 2 * child->xpadding > *requisition)
        *requisition = child_requisition.width + 2 * child->xpadding;
    }
    children = children->next;
  }

  sheet->column[col].requisition = *requisition;
}

/**
 * gtk_sheet_move_child:
 * @sheet: a #GtkSheet.
 * @widget: #GtkWidget to be put.
 * @x: x coord at which we move the widget.
 * @y: y coord at which we move the widget.
 *
 * Move widgets added with gtk_sheet_put() in the sheet.
 */ 
void
gtk_sheet_move_child(GtkSheet *sheet, GtkWidget *widget, gint x, gint y)
{
  GtkSheetChild *child;
  GList *children;

  g_return_if_fail(sheet != NULL);
  g_return_if_fail(GTK_IS_SHEET(sheet));

  children = sheet->children;
  while(children)
    {
       child = children->data;

       if(child->widget == widget){
         child->x = x;
         child->y = y;
         child->row = ROW_FROM_YPIXEL(sheet, y);
         child->col = COLUMN_FROM_XPIXEL(sheet, x);
         gtk_sheet_position_child(sheet, child);
         return;
       }

       children = children->next;
    }

  g_warning("Widget must be a GtkSheet child"); 

}

static void
gtk_sheet_position_child(GtkSheet *sheet, GtkSheetChild *child)
{
   GtkRequisition child_requisition;
   GtkAllocation child_allocation;
   gint xoffset = 0; 
   gint yoffset = 0;
   gint x = 0, y = 0;
   GdkRectangle area;

   gtk_widget_get_child_requisition(child->widget, &child_requisition);

   if(sheet->column_titles_visible) 
             yoffset = sheet->column_title_area.height;

   if(sheet->row_titles_visible)
             xoffset = sheet->row_title_area.width;

   if(child->attached_to_cell){
/*
      child->x = COLUMN_LEFT_XPIXEL(sheet, child->col);
      child->y = ROW_TOP_YPIXEL(sheet, child->row);

      if(sheet->row_titles_visible) 
                                    child->x-=sheet->row_title_area.width;
      if(sheet->column_titles_visible) 
                                    child->y-=sheet->column_title_area.height;

      width = sheet->column[child->col].width;
      height = sheet->row[child->row].height;
*/
      
      gtk_sheet_get_cell_area(sheet, child->row, child->col, &area);
      child->x = area.x + child->xpadding;
      child->y = area.y + child->ypadding;

      if(!child->floating){
        if(child_requisition.width + 2*child->xpadding <= sheet->column[child->col].width){
          if(child->xfill){
            child_requisition.width = child_allocation.width = sheet->column[child->col].width - 2*child->xpadding;
          } else {
            if(child->xexpand){
              child->x = area.x + sheet->column[child->col].width / 2 -
                                  child_requisition.width / 2;
            }
            child_allocation.width = child_requisition.width;
          }
        } else {
          if(!child->xshrink){
            gtk_sheet_set_column_width(sheet, child->col, child_requisition.width + 2 * child->xpadding);
          }
          child_allocation.width = sheet->column[child->col].width - 2*child->xpadding;
        }

        if(child_requisition.height + 2*child->ypadding <= sheet->row[child->row].height){
          if(child->yfill){
            child_requisition.height = child_allocation.height = sheet->row[child->row].height - 2*child->ypadding;
          } else {
            if(child->yexpand){
              child->y = area.y + sheet->row[child->row].height / 2 -
                                  child_requisition.height / 2;
            }
            child_allocation.height = child_requisition.height;
          }
        } else {
          if(!child->yshrink){
            gtk_sheet_set_row_height(sheet, child->row, child_requisition.height + 2 * child->ypadding);
          }
          child_allocation.height = sheet->row[child->row].height - 2*child->ypadding;
        }
      } else {
        child_allocation.width = child_requisition.width;
        child_allocation.height = child_requisition.height;
      }

      x = child_allocation.x = child->x + xoffset;
      y = child_allocation.y = child->y + yoffset;
   }
   else
   {
      x = child_allocation.x = child->x + sheet->hoffset + xoffset;   
      x = child_allocation.x = child->x + xoffset;   
      y = child_allocation.y = child->y + sheet->voffset + yoffset;
      y = child_allocation.y = child->y + yoffset;
      child_allocation.width = child_requisition.width;
      child_allocation.height = child_requisition.height;
   }

   gtk_widget_size_allocate(child->widget, &child_allocation);
   gtk_widget_queue_draw(child->widget);
}

static void
gtk_sheet_forall (GtkContainer *container,
                  gboolean      include_internals,
                  GtkCallback   callback,
                  gpointer      callback_data)
{
  GtkSheet *sheet;
  GtkSheetChild *child;
  GList *children;

  g_return_if_fail (GTK_IS_SHEET (container));
  g_return_if_fail (callback != NULL);

  sheet = GTK_SHEET (container);
  children = sheet->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      (* callback) (child->widget, callback_data);
    }
  if(sheet->button)
     (* callback) (sheet->button, callback_data);
  if(sheet->sheet_entry)
     (* callback) (sheet->sheet_entry, callback_data);
}


static void
gtk_sheet_position_children(GtkSheet *sheet)
{
  GList *children;
  GtkSheetChild *child;

  children = sheet->children;

  while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->col !=-1 && child->row != -1)
           gtk_sheet_position_child(sheet, child);

     if (child->row == -1) {
        if(child->col < MIN_VISIBLE_COLUMN(sheet) || 
          child->col > MAX_VISIBLE_COLUMN(sheet))
          gtk_sheet_child_hide(child);
        else
          gtk_sheet_child_show(child);
     }
     if (child->col == -1) {
        if(child->row < MIN_VISIBLE_ROW(sheet) ||
          child->row > MAX_VISIBLE_ROW(sheet))
          gtk_sheet_child_hide(child);
        else
          gtk_sheet_child_show(child);
     }
 
     children = children->next;
   }
    
}

static void
gtk_sheet_remove (GtkContainer *container, GtkWidget *widget)
{
  GtkSheet *sheet;
  GList *children;
  GtkSheetChild *child = 0;

  g_return_if_fail(container != NULL);
  g_return_if_fail(GTK_IS_SHEET(container));

  sheet = GTK_SHEET(container);

  children = sheet->children;

  while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->widget == widget) break;

     children = children->next;
   }

  if (children)
   {
     if(child->row == -1)
        sheet->row[child->col].button.child = NULL;

     if(child->col == -1)
        sheet->column[child->row].button.child = NULL;

     gtk_widget_unparent (widget);
     child->widget = NULL;

     sheet->children = g_list_remove_link (sheet->children, children);
     g_list_free_1 (children);
     g_free(child);
   }

}

static void
gtk_sheet_realize_child(GtkSheet *sheet, GtkSheetChild *child)
{
  GtkWidget *widget;

  widget = GTK_WIDGET(sheet);

  if(GTK_WIDGET_REALIZED(widget)){
    if(child->row == -1)
      gtk_widget_set_parent_window(child->widget, sheet->column_title_window);
    else if(child->col == -1)
      gtk_widget_set_parent_window(child->widget, sheet->row_title_window);
    else
      gtk_widget_set_parent_window(child->widget, sheet->sheet_window);
  }

  gtk_widget_set_parent(child->widget, widget);
}


/**
 * gtk_sheet_get_child_at:
 * @sheet: a #GtkSheet.
 * @row: row number
 * @col: column number
 *
 * Get the child attached at @row,@col.
 * Return value: a #GtkSheetChild structure.
 */ 
GtkSheetChild *
gtk_sheet_get_child_at(GtkSheet *sheet, gint row, gint col)
{
  GList *children;
  GtkSheetChild *child = 0;

  g_return_val_if_fail(sheet != NULL, NULL);
  g_return_val_if_fail(GTK_IS_SHEET(sheet), NULL);

  children = sheet->children;

  while(children)
   {
     child = (GtkSheetChild *)children->data;

     if(child->attached_to_cell)
        if(child->row == row && child->col == col) break; 
     
     children = children->next;
   }

  if(children) return child; 

  return NULL;
}

static void
gtk_sheet_child_hide(GtkSheetChild *child) 
{
  g_return_if_fail(child != NULL);
  gtk_widget_hide(child->widget);
}

static void
gtk_sheet_child_show(GtkSheetChild *child) 
{
  g_return_if_fail(child != NULL);

  gtk_widget_show(child->widget);
}
