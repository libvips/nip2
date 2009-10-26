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
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GTK_SHEETWIDGET_H__
#define __GTK_SHEETWIDGET_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

G_BEGIN_DECLS


typedef enum
{
  GTK_SHEET_FOREGROUND,
  GTK_SHEET_BACKGROUND,
  GTK_SHEET_FONT,
  GTK_SHEET_JUSTIFICATION,
  GTK_SHEET_BORDER,
  GTK_SHEET_BORDER_COLOR,
  GTK_SHEET_IS_EDITABLE,
  GTK_SHEET_IS_VISIBLE
} GtkSheetAttrType;

/**
 * GtkSheetState:
 */
typedef enum 
{
  GTK_SHEET_NORMAL,             /* Nothing selected */
  GTK_SHEET_ROW_SELECTED,       /* A single row is selected */
  GTK_SHEET_COLUMN_SELECTED,    /* A single column is selected */
  GTK_SHEET_RANGE_SELECTED      /* A range of cells is selected */
} GtkSheetState;


/**
 * GtkSheetBorderFlags:
 * 
 * Used to build a border mask when setting border properties with gtk_sheet_range_set_border().
 */     
typedef enum
{
  GTK_SHEET_LEFT_BORDER     = 1 << 0, 
  GTK_SHEET_RIGHT_BORDER    = 1 << 1, 
  GTK_SHEET_TOP_BORDER      = 1 << 2, 
  GTK_SHEET_BOTTOM_BORDER   = 1 << 3 
} GtkSheetBorderFlags; 

#define GTK_TYPE_SHEET_CELL_BORDER (gtk_sheet_cell_border_get_type ())
#define GTK_TYPE_SHEET_CELL_ATTR (gtk_sheet_cell_attr_get_type ())
#define GTK_TYPE_SHEET_RANGE (gtk_sheet_range_get_type ())

#define GTK_TYPE_SHEET (gtk_sheet_get_type ())
#define GTK_SHEET(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj),GTK_TYPE_SHEET, GtkSheet))
#define GTK_SHEET_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SHEET, GtkSheetClass))
#define GTK_IS_SHEET(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SHEET))
#define GTK_IS_SHEET_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SHEET))
#define GTK_SHEET_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SHEET, GtkSheetClass))

/* Public flags, for compatibility */

#define GTK_SHEET_IS_LOCKED(sheet)       gtk_sheet_locked(sheet)
#define GTK_SHEET_ROW_FROZEN(sheet)      !gtk_sheet_rows_resizable(sheet)
#define GTK_SHEET_COLUMN_FROZEN(sheet)   !gtk_sheet_columns_resizable(sheet)
#define GTK_SHEET_AUTORESIZE(sheet)      gtk_sheet_autoresize(sheet)
#define GTK_SHEET_CLIP_TEXT(sheet)       gtk_sheet_clip_text(sheet)
#define GTK_SHEET_ROW_TITLES_VISIBLE(sheet)   gtk_sheet_row_titles_visible(sheet)
#define GTK_SHEET_COL_TITLES_VISIBLE(sheet)   gtk_sheet_column_titles_visible(sheet) 
#define GTK_SHEET_AUTO_SCROLL(sheet)     gtk_sheet_autoscroll(sheet)
#define GTK_SHEET_JUSTIFY_ENTRY(sheet)   gtk_sheet_justify_entry(sheet)



typedef struct _GtkSheet GtkSheet;
typedef struct _GtkSheetClass GtkSheetClass;
typedef struct _GtkSheetChild GtkSheetChild;
typedef struct _GtkSheetRow GtkSheetRow;
typedef struct _GtkSheetColumn GtkSheetColumn;
typedef struct _GtkSheetCell GtkSheetCell;
typedef struct _GtkSheetRange GtkSheetRange;
typedef struct _GtkSheetButton       GtkSheetButton;
typedef struct _GtkSheetCellAttr     GtkSheetCellAttr;
typedef struct _GtkSheetCellBorder     GtkSheetCellBorder;


/**
 * GtkSheetChild:
 * @widget: the child's widget
 * @x: the horizontal pixel position of the widget in the gtksheet
 * @y: the vertical pixel position of the widget in the gtksheet
 * @attached_to_cell: whether the child's position is kept attached to the cell indicated by @row and @col
 * @floating: whether the child size is kept within the area of the cell indicated by @row and @col
 * @row: the sheet row which the child is attached to.
 * @col: the sheet column which the child is attached to.
 * @xpadding: extra horizontal padding added around the widget 
 * @ypadding: extra vertical padding added around the widget 
 * @xexpand: whether the sheet cell should expand to take up any extra horizontal space that has been allocated to the sheet.
 * @yexpand: whether the sheet cell should expand to take up any extra vertical space that has been allocated to the sheet.
 * @xshrink: whether the widget should shrink when the sheet cell shrinks horizontally.
 * @yshrink: whether the widget should shrink when the sheet cell shrinks vertically.
 * @xfill: whether the widget should fill the horizontal space allocated to it in the sheet cell.
 * @yfill: whether the widget should fill the vertical space allocated to it in the sheet cell
 *
 * The GtkSheetChild struct is returned by the gtk_sheet_get_child_at() and 
 * gtk_sheet_put() functions and contains information about the child's packing
 * inside a GtkSheet.
 * Previously, modifying this structure was the only way to modify child's 
 * packing. However, now that child properties have been implemented, you 
 * shouldn't use this method. To change child packing options
 * set child properties with the gtk_container_child_set_property() or similar
 * functions.
 *
 * Deprecated: 0.1.0: Use child properties instead.
 */
struct _GtkSheetChild
{ 
  GtkWidget *widget;
  gint x,y ;
  gboolean attached_to_cell;
  gboolean floating;
  gint row, col;
  guint16 xpadding;
  guint16 ypadding;
  gboolean xexpand;
  gboolean yexpand;
  gboolean xshrink;
  gboolean yshrink;
  gboolean xfill;
  gboolean yfill;
};


/**
 * GtkSheetButton:
 *
 * The GtkSheetButton struct contains only private data.
 * It should only be accessed through the functions described below.
 */
struct _GtkSheetButton
{
  /*< private >*/
  GtkStateType state;
  gchar *label;

  gboolean label_visible;
  GtkSheetChild *child;

  GtkJustification justification;
};


/**
 * GtkSheetCellBorder:
 *
 * The GtkSheetCellBorder struct contains only private data.
 * It should only be accessed through the functions described below.
 */
struct _GtkSheetCellBorder
{
  /*< private >*/
  gint8 mask;
  guint width;
  GdkLineStyle line_style;
  GdkCapStyle cap_style;
  GdkJoinStyle join_style;
  GdkColor color;
};

/**
 * GtkSheetCellAttr:
 *
 * The GtkSheetCellAttr struct contains only private data.
 * It should only be accessed through the functions described below.
 */
struct _GtkSheetCellAttr
{
  /*< private >*/
  GtkJustification justification;
  PangoFontDescription *font_desc;
  GdkColor foreground;
  GdkColor background;
  GtkSheetCellBorder border;
  gboolean is_editable;
  gboolean is_visible;
};

/**
 * GtkSheetCell:
 *
 * The GtkSheetCell struct contains only private data.
 * It should only be accessed through the functions described below.
 */
struct _GtkSheetCell
{
   /*< private >*/
  GdkRectangle area;
  gint row;
  gint col;

  GtkSheetCellAttr *attributes;

  gchar *text;
  gpointer link;
};

/**
 * GtkSheetRange:
 *
 * The #GtkSheetCellRange struct is used in many GtkSheet functions to indicate 
 * a range of cells of the sheet where some properties or changes must be made. 
 * The range goes from the the upper-left cell determined by @row0 and @col0, to
 * the bottom-right cell indicated by @rowi and @coli.
 */
struct _GtkSheetRange
{
  gint row0,col0; /* upper-left cell */
  gint rowi,coli; /* bottom-right cell */
};


/**
 * GtkSheetRow:
 *
 * The GtkSheetRow struct contains only private data.
 * It should only be accessed through the functions described below.
 */
struct _GtkSheetRow
{
 /*< private >*/
 gchar *name;
 gint height;
 gint top_ypixel;
 guint16 requisition;

 GtkSheetButton button;
 gboolean is_sensitive;
 gboolean is_visible;
};

/**
 * GtkSheetColumn:
 *
 * The GtkSheetColumn struct contains only private data.
 * It should only be accessed through the functions described below.
 */
struct _GtkSheetColumn
{
 /*< private >*/
 gchar *name;
 gint width;
 gint left_xpixel;
 guint16 requisition;

 GtkSheetButton button;

 gint left_text_column; /* min left column displaying text on this column */
 gint right_text_column; /* max right column displaying text on this column */

 GtkJustification justification;
 gboolean is_sensitive;
 gboolean is_visible;
};


/**
 * GtkSheet:
 *
 * The GtkSheet struct contains only private data.
 * It should only be accessed through the functions described below.
 */
struct _GtkSheet{
  GtkContainer container;

  guint16 flags;

  GtkSelectionMode selection_mode;
  gboolean autoresize;
  gboolean autoscroll;
  gboolean clip_text;
  gboolean justify_entry;
  gboolean locked;

  guint freeze_count;

  /* Background colors */
  GdkColor bg_color;
  GdkColor grid_color;
  gboolean show_grid;

  /* sheet children */
  GList *children;

  /* allocation rectangle after the container_border_width
     and the width of the shadow border */
  GdkRectangle internal_allocation;

  gchar *name;

  GtkSheetRow *row;
  GtkSheetColumn *column;

  gboolean rows_resizable;
  gboolean columns_resizable;

  /* max number of diplayed cells */
  gint maxrow;
  gint maxcol;

  /* Displayed range */

  GtkSheetRange view; 

  /* sheet data: dynamically allocated array of cell pointers */
  GtkSheetCell ***data;

  /* max number of allocated cells */
  gint maxallocrow;
  gint maxalloccol;

  /* active cell */
  GtkSheetCell active_cell;
  GtkWidget *sheet_entry;

  GtkType entry_type;

  /* expanding selection */
  GtkSheetCell selection_cell;

  /* timer for automatic scroll during selection */  
  gint32 timer;
  /* timer for flashing clipped range */
  gint32 clip_timer;
  gint interval;

  /* global selection button */
  GtkWidget *button;

  /* sheet state */
  gint state;

  /* selected range */
  GtkSheetRange range;

  /*the scrolling window and it's height and width to
   * make things a little speedier */
  GdkWindow *sheet_window;
  guint sheet_window_width;
  guint sheet_window_height;

  /* sheet backing pixmap */  
  GdkWindow *pixmap;    

  /* offsets for scrolling */
  gint hoffset;
  gint voffset;
  gfloat old_hadjustment;
  gfloat old_vadjustment;
  
  /* border shadow style */
  GtkShadowType shadow_type;
  
  /* Column Titles */
  GdkRectangle column_title_area;
  GdkWindow *column_title_window;
  gboolean column_titles_visible;

  /* Row Titles */
  GdkRectangle row_title_area;
  GdkWindow *row_title_window;
  gboolean row_titles_visible;

  /*scrollbars*/
  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;

  /* xor GC for the verticle drag line */
  GdkGC *xor_gc;

  /* gc for drawing unselected cells */
  GdkGC *fg_gc;
  GdkGC *bg_gc;

  /* cursor used to indicate dragging */
  GdkCursor *cursor_drag;

  /* the current x-pixel location of the xor-drag vline */
  gint x_drag;

  /* the current y-pixel location of the xor-drag hline */
  gint y_drag;

  /* current cell being dragged */
  GtkSheetCell drag_cell;
  /* current range being dragged */
  GtkSheetRange drag_range;

  /* clipped range */
  GtkSheetRange clip_range;
};

struct _GtkSheetClass
{
 GtkContainerClass parent_class;
 
 void (*set_scroll_adjustments) (GtkSheet *sheet,
				 GtkAdjustment *hadjustment,
				 GtkAdjustment *vadjustment);

 void (*select_row) 		(GtkSheet *sheet, gint row);

 void (*select_column) 		(GtkSheet *sheet, gint column);

 void (*select_range) 		(GtkSheet *sheet, GtkSheetRange *range);

 void (*clip_range) 		(GtkSheet *sheet, GtkSheetRange *clip_range);

 void (*resize_range)		(GtkSheet *sheet,
	                	GtkSheetRange *old_range,
                        	GtkSheetRange *new_range);

 void (*move_range)    		(GtkSheet *sheet,
	                	GtkSheetRange *old_range,
                        	GtkSheetRange *new_range);

 gboolean (*traverse)       	(GtkSheet *sheet,
                         	gint row, gint column,
                         	gint *new_row, gint *new_column);

 gboolean (*deactivate)	 	(GtkSheet *sheet,
	                  	gint row, gint column);

 gboolean (*activate) 		(GtkSheet *sheet,
	                	gint row, gint column);

 void (*set_cell) 		(GtkSheet *sheet,
	           		gint row, gint column);

 void (*clear_cell) 		(GtkSheet *sheet,
	           		gint row, gint column);

 void (*changed) 		(GtkSheet *sheet,
	          		gint row, gint column);

 void (*new_column_width)       (GtkSheet *sheet,
                                 gint col,
                                 guint width);

 void (*new_row_height)       	(GtkSheet *sheet,
                                 gint row,
                                 guint height);

};
  
GType gtk_sheet_get_type (void);
GType gtk_sheet_range_get_type (void);
GType gtk_sheet_cell_border_get_type (void);
GType gtk_sheet_cell_attr_get_type (void);

/* create a new sheet */
GtkWidget *
gtk_sheet_new 				(guint rows, guint columns, const gchar *title);

void
gtk_sheet_construct			(GtkSheet *sheet,
       					 guint rows, guint columns, const gchar *title);

/* create a new browser sheet. It cells can not be edited */
GtkWidget *
gtk_sheet_new_browser 			(guint rows, guint columns, const gchar *title);

void
gtk_sheet_construct_browser		(GtkSheet *sheet,
       					 guint rows, guint columns, const gchar *title);

/* create a new sheet with custom entry */
GtkWidget *
gtk_sheet_new_with_custom_entry 	(guint rows, guint columns, 
                                         const gchar *title,
                                 	 GtkType entry_type);
void
gtk_sheet_construct_with_custom_entry	(GtkSheet *sheet,
       					 guint rows, guint columns, 
                                         const gchar *title,
					 GtkType entry_type);
/* change scroll adjustments */
void
gtk_sheet_set_hadjustment		(GtkSheet *sheet,
					 GtkAdjustment *adjustment); 
void
gtk_sheet_set_vadjustment		(GtkSheet *sheet,
					 GtkAdjustment *adjustment); 
/* Change entry */
void
gtk_sheet_change_entry			(GtkSheet *sheet, GtkType entry_type);

/* Returns sheet's entry widget */
GtkWidget *
gtk_sheet_get_entry			(GtkSheet *sheet);
GtkWidget *
gtk_sheet_get_entry_widget		(GtkSheet *sheet);

/* Returns sheet->state 
 * Added by Steven Rostedt <steven.rostedt@lmco.com> */
gint
gtk_sheet_get_state 			(GtkSheet *sheet);

/* Returns sheet's ranges 
 * Added by Murray Cumming */
guint
gtk_sheet_get_columns_count 		(GtkSheet *sheet);

guint
gtk_sheet_get_rows_count 		(GtkSheet *sheet);

void
gtk_sheet_get_visible_range		(GtkSheet *sheet,
					 GtkSheetRange *range);
void
gtk_sheet_set_selection_mode		(GtkSheet *sheet, GtkSelectionMode mode);

void
gtk_sheet_set_autoresize		(GtkSheet *sheet, gboolean autoresize);

gboolean
gtk_sheet_autoresize			(GtkSheet *sheet);

void
gtk_sheet_set_autoscroll		(GtkSheet *sheet, gboolean autoscroll);

gboolean
gtk_sheet_autoscroll			(GtkSheet *sheet);

void
gtk_sheet_set_clip_text			(GtkSheet *sheet, gboolean clip_text);

gboolean
gtk_sheet_clip_text			(GtkSheet *sheet);

void
gtk_sheet_set_justify_entry		(GtkSheet *sheet, gboolean justify);

gboolean
gtk_sheet_justify_entry			(GtkSheet *sheet);

void
gtk_sheet_set_locked			(GtkSheet *sheet, gboolean locked);

gboolean
gtk_sheet_locked			(GtkSheet *sheet);

/* set sheet title */
void
gtk_sheet_set_title 			(GtkSheet *sheet, const gchar *title);

/* freeze all visual updates of the sheet.
 * Then thaw the sheet after you have made a number of changes.
 * The updates will occure in a more efficent way than if
 * you made them on a unfrozen sheet */
void
gtk_sheet_freeze			(GtkSheet *sheet);
void
gtk_sheet_thaw				(GtkSheet *sheet);
/* Background colors */
void
gtk_sheet_set_background		(GtkSheet *sheet,
					 GdkColor *color);
void
gtk_sheet_set_grid			(GtkSheet *sheet,
					 GdkColor *color);
void
gtk_sheet_show_grid			(GtkSheet *sheet,
					 gboolean show);
gboolean
gtk_sheet_grid_visible			(GtkSheet *sheet);

/* set/get column title */ 
void
gtk_sheet_set_column_title 		(GtkSheet * sheet,
			    		gint column,
			    		const gchar * title);

const gchar *
gtk_sheet_get_column_title 		(GtkSheet * sheet,
			    		gint column);

/* set/get row title */
void
gtk_sheet_set_row_title 		(GtkSheet * sheet,
			    		gint row,
			    		const gchar * title);
const gchar *
gtk_sheet_get_row_title 		(GtkSheet * sheet,
			    		gint row);

/* set/get button label */
void
gtk_sheet_row_button_add_label		(GtkSheet *sheet, 
					gint row, const gchar *label);
void
gtk_sheet_column_button_add_label	(GtkSheet *sheet, 
					gint column, const gchar *label);
const gchar *
gtk_sheet_row_button_get_label		(GtkSheet *sheet, 
					gint row);
const gchar *
gtk_sheet_column_button_get_label	(GtkSheet *sheet, 
					gint column);
void
gtk_sheet_row_button_justify		(GtkSheet *sheet, 
					gint row, GtkJustification justification);
void
gtk_sheet_column_button_justify		(GtkSheet *sheet, 
					gint column, GtkJustification justification);

/* scroll the viewing area of the sheet to the given column
 * and row; row_align and col_align are between 0-1 representing the
 * location the row should appear on the screnn, 0.0 being top or left,
 * 1.0 being bottom or right; if row or column is negative then there
 * is no change */
void
gtk_sheet_moveto (GtkSheet * sheet,
		  gint row,
		  gint column,
	          gfloat row_align,
                  gfloat col_align);

/* resize column/row titles window */
void 
gtk_sheet_set_row_titles_width(GtkSheet *sheet, guint width);
void 
gtk_sheet_set_column_titles_height(GtkSheet *sheet, guint height);

/* show/hide column/row titles window */
void
gtk_sheet_show_column_titles		(GtkSheet *sheet);
void
gtk_sheet_show_row_titles		(GtkSheet *sheet);
void
gtk_sheet_hide_column_titles		(GtkSheet *sheet);
void
gtk_sheet_hide_row_titles		(GtkSheet *sheet);
gboolean
gtk_sheet_column_titles_visible		(GtkSheet *sheet);
gboolean
gtk_sheet_row_titles_visible		(GtkSheet *sheet);

/* set column button sensitivity. If sensitivity is TRUE it can be toggled,  
 *  otherwise it acts as a title */
void 
gtk_sheet_column_set_sensitivity	(GtkSheet *sheet, 
					gint column, gboolean sensitive);

/* set sensitivity for all column buttons */
void
gtk_sheet_columns_set_sensitivity	(GtkSheet *sheet, gboolean sensitive);
void
gtk_sheet_columns_set_resizable	 	(GtkSheet *sheet, gboolean resizable);
gboolean
gtk_sheet_columns_resizable	 	(GtkSheet *sheet);

/* set row button sensitivity. If sensitivity is TRUE can be toggled, 
 * otherwise it acts as a title */
void 
gtk_sheet_row_set_sensitivity		(GtkSheet *sheet, 
					gint row,  gboolean sensitive);

/* set sensitivity for all row buttons */
void
gtk_sheet_rows_set_sensitivity		(GtkSheet *sheet, gboolean sensitive);
void
gtk_sheet_rows_set_resizable	 	(GtkSheet *sheet, gboolean resizable);
gboolean
gtk_sheet_rows_resizable		(GtkSheet *sheet);

/* set column visibility. The default value is TRUE. If FALSE, the 
 * column is hidden */
void
gtk_sheet_column_set_visibility		(GtkSheet *sheet, 
					gint column, gboolean visible);
void
gtk_sheet_column_label_set_visibility	(GtkSheet *sheet, 
					gint col, gboolean visible);
void
gtk_sheet_columns_labels_set_visibility	(GtkSheet *sheet, gboolean visible);

/* set row visibility. The default value is TRUE. If FALSE, the 
 * row is hidden */
void
gtk_sheet_row_set_visibility		(GtkSheet *sheet, 
					 gint row, gboolean visible);
void
gtk_sheet_row_label_set_visibility	(GtkSheet *sheet, 
					 gint row, gboolean visible);
void
gtk_sheet_rows_labels_set_visibility	(GtkSheet *sheet, gboolean visible);


/* select the row. The range is then highlighted, and the bounds are stored
 * in sheet->range  */
void
gtk_sheet_select_row 			(GtkSheet * sheet,
		      			gint row);

/* select the column. The range is then highlighted, and the bounds are stored
 * in sheet->range  */
void
gtk_sheet_select_column 		(GtkSheet * sheet,
		         		gint column);

/* save selected range to "clipboard" */
void
gtk_sheet_clip_range 			(GtkSheet *sheet, const GtkSheetRange *range);
/* free clipboard */
void
gtk_sheet_unclip_range			(GtkSheet *sheet);

gboolean
gtk_sheet_in_clip			(GtkSheet *sheet);

/* get scrollbars adjustment */
GtkAdjustment *
gtk_sheet_get_vadjustment 		(GtkSheet * sheet);
GtkAdjustment *
gtk_sheet_get_hadjustment 		(GtkSheet * sheet);

/* highlight the selected range and store bounds in sheet->range */
void gtk_sheet_select_range		(GtkSheet *sheet, 
					 const GtkSheetRange *range); 

/* obvious */
void gtk_sheet_unselect_range		(GtkSheet *sheet); 

/* set active cell where the entry will be displayed 
 * returns FALSE if current cell can't be deactivated or
 * requested cell can't be activated */
gboolean
gtk_sheet_set_active_cell 		(GtkSheet *sheet, 
					gint row, gint column);
void
gtk_sheet_get_active_cell 		(GtkSheet *sheet, 
					gint *row, gint *column);

/* set cell contents and allocate memory if needed */
void 
gtk_sheet_set_cell			(GtkSheet *sheet, 
					gint row, gint col, 
                                        GtkJustification justification,
                   			const gchar *text);
void 
gtk_sheet_set_cell_text			(GtkSheet *sheet, 
					gint row, gint col,
                   			const gchar *text);

/* get cell contents */
gchar *     
gtk_sheet_cell_get_text 		(GtkSheet *sheet, gint row, gint col);


/* clear cell contents */
void 
gtk_sheet_cell_clear			(GtkSheet *sheet, gint row, gint column);
/* clear cell contents and remove links */
void 
gtk_sheet_cell_delete			(GtkSheet *sheet, gint row, gint column);

/* clear range contents. If range==NULL the whole sheet will be cleared */
void 
gtk_sheet_range_clear			(GtkSheet *sheet, 
					 const GtkSheetRange *range);
/* clear range contents and remove links */
void 
gtk_sheet_range_delete			(GtkSheet *sheet, 
					 const GtkSheetRange *range);

/* get cell state: GTK_STATE_NORMAL, GTK_STATE_SELECTED */
GtkStateType
gtk_sheet_cell_get_state 		(GtkSheet *sheet, gint row, gint col);

/* Handles cell links */
void
gtk_sheet_link_cell			(GtkSheet *sheet, gint row, gint col,
 					 gpointer link);

gpointer
gtk_sheet_get_link			(GtkSheet *sheet, gint row, gint col);
void
gtk_sheet_remove_link			(GtkSheet *sheet, gint row, gint col);

/* get row and column correspondig to the given position in the screen */
gboolean
gtk_sheet_get_pixel_info (GtkSheet * sheet,
			  gint x,
			  gint y,
			  gint * row,
			  gint * column);

/* get area of a given cell */
gboolean
gtk_sheet_get_cell_area (GtkSheet *sheet,
                         gint row,
                         gint column,
                         GdkRectangle *area);

/* set column width */
void
gtk_sheet_set_column_width (GtkSheet * sheet,
			    gint column,
			    guint width);

/* set row height */
void
gtk_sheet_set_row_height (GtkSheet * sheet,
			  gint row,
			  guint height);

/* append ncols columns to the end of the sheet */
void
gtk_sheet_add_column			(GtkSheet *sheet, guint ncols);

/* append nrows row to the end of the sheet */
void
gtk_sheet_add_row			(GtkSheet *sheet, guint nrows);

/* insert nrows rows before the given row and pull right */
void
gtk_sheet_insert_rows			(GtkSheet *sheet, guint row, guint nrows);

/* insert ncols columns before the given col and pull down */
 void
gtk_sheet_insert_columns		(GtkSheet *sheet, guint col, guint ncols);

/* delete nrows rows starting in row */
void
gtk_sheet_delete_rows			(GtkSheet *sheet, guint row, guint nrows);

/* delete ncols columns starting in col */
void
gtk_sheet_delete_columns		(GtkSheet *sheet, guint col, guint ncols);

/* set abckground color of the given range */
void
gtk_sheet_range_set_background		(GtkSheet *sheet, 
					const GtkSheetRange *urange, 
					const GdkColor *color);

/* set foreground color (text color) of the given range */
void
gtk_sheet_range_set_foreground		(GtkSheet *sheet, 
					const GtkSheetRange *urange, 
					const GdkColor *color);

/* set text justification (GTK_JUSTIFY_LEFT, RIGHT, CENTER) of the given range.
 * The default value is GTK_JUSTIFY_LEFT. If autoformat is on, the
 * default justification for numbers is GTK_JUSTIFY_RIGHT */
void
gtk_sheet_range_set_justification	(GtkSheet *sheet, 
					const GtkSheetRange *urange, 
					GtkJustification just);
void
gtk_sheet_column_set_justification      (GtkSheet *sheet,
                                        gint col,
                                        GtkJustification just);
/* set if cell contents can be edited or not in the given range:
 * accepted values are TRUE or FALSE. */
void
gtk_sheet_range_set_editable		(GtkSheet *sheet, 
					const GtkSheetRange *urange, 
					gint editable);

/* set if cell contents are visible or not in the given range:
 * accepted values are TRUE or FALSE.*/
void
gtk_sheet_range_set_visible		(GtkSheet *sheet, 
					const GtkSheetRange *urange, 
					gboolean visible);

/* set cell border style in the given range.
 * mask values are CELL_LEFT_BORDER, CELL_RIGHT_BORDER, CELL_TOP_BORDER,
 * CELL_BOTTOM_BORDER
 * width is the width of the border line in pixels 
 * line_style is the line_style for the border line */
void
gtk_sheet_range_set_border		(GtkSheet *sheet, 
					const GtkSheetRange *urange, 
					gint mask, 
					guint width, 
					gint line_style);

/* set border color for the given range */
void
gtk_sheet_range_set_border_color	(GtkSheet *sheet, 
					const GtkSheetRange *urange, 
					const GdkColor *color);

/* set font for the given range */
void
gtk_sheet_range_set_font		(GtkSheet *sheet, 
					const GtkSheetRange *urange, 
					PangoFontDescription *font);

/* get cell attributes of the given cell */
/* TRUE means that the cell is currently allocated */
gboolean
gtk_sheet_get_attributes		(GtkSheet *sheet, 
					gint row, gint col, 
					GtkSheetCellAttr *attributes);


GtkSheetChild *
gtk_sheet_put 				(GtkSheet *sheet, 
					 GtkWidget *child, 
					 gint x, gint y);
void
gtk_sheet_attach_floating               (GtkSheet *sheet,
                                         GtkWidget *widget,
                                         gint row, gint col);
void
gtk_sheet_attach_default                (GtkSheet *sheet,
                                         GtkWidget *widget,
                                         gint row, gint col);
void
gtk_sheet_attach                        (GtkSheet *sheet,
                                         GtkWidget *widget,
                                         gint row, gint col,
                                         gint xoptions,
                                         gint yoptions,
                                         gint xpadding,
                                         gint ypadding);


void
gtk_sheet_move_child 			(GtkSheet *sheet, 
					 GtkWidget *widget, 
					 gint x, gint y);

GtkSheetChild *
gtk_sheet_get_child_at			(GtkSheet *sheet, 
					 gint row, gint col);

void
gtk_sheet_button_attach			(GtkSheet *sheet,
					 GtkWidget *widget,
					 gint row, gint col);
                       

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_SHEETWIDGET_H__ */


