/* a row in a workspace ... part of a subcolumn
 */

/*

    Copyright (C) 1991-2003 The National Gallery

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#define TYPE_ROW (row_get_type())
#define ROW( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_ROW, Row ))
#define ROW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_ROW, RowClass))
#define IS_ROW( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_ROW ))
#define IS_ROW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_ROW ))
#define ROW_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_ROW, RowClass ))

/* For when we're flashing the showstate up.
 */
typedef enum {
	ROW_SHOW_NONE,
	ROW_SHOW_PARENT,
	ROW_SHOW_CHILD
} RowShowState;

struct _Row {
	Heapmodel parent_class;

	/* Our context.
	 */
	Subcolumn *scol;	/* Enclosing subcolumn */
	Rhs *child_rhs;		/* Child RHS */
	Column *top_col;	/* Enclosing top level column */
	Workspace *ws;		/* Enclosing workspace */
	Row *top_row;		/* Enclosing root row */

	Symbol *sym;		/* Symbol we represent */

	Expr *expr;		/* The expr we edit */
	gboolean err;		/* Set if this row is on the error list */

	gboolean selected;	/* Selected or not */
	gboolean is_class;	/* Display spin buttons */
	gboolean popup;		/* Set to pop up view on 1st display */
	gboolean to_save;	/* Should be saved (part of only-save-modded) */

	GSList *parents;	/* rows which depend on us */
	GSList *children;	/* rows we depend on */
	gboolean dirty;		/* If we're marked for recomp */
	GSList *recomp;		/* If root of class display, subs to recomp */
	GSList *recomp_save;	/* Previous recomp list */

	gboolean depend;	/* For spotting dependency loops */

	RowShowState show;	/* For showing parent/child stuff */
};

typedef struct _RowClass {
	HeapmodelClass parent_class;

	/* My methods.
	 */
} RowClass;

const char *row_name( Row *row );
void row_qualified_name_relative( Symbol *context, Row *row, BufInfo *buf );
void row_qualified_name( Row *row, BufInfo *buf );
void *row_name_print( Row *row );

void row_error_set( Row *row );
void row_error_clear( Row *row );

Workspace *row_get_workspace( Row *row );

GType row_get_type( void );
void row_link_symbol( Row *row, Symbol *sym, PElement *root );
Row *row_new( Subcolumn *scol, Symbol *sym, PElement *root );

void *row_dirty_set( Row *row, gboolean clear_dirty );
void *row_dirty( Row *row, gboolean clear_dirty );
void *row_dirty_intrans( Row *row, gboolean clear_dirty );

void row_recomp( Row *row );

void *row_is_selected( Row *row );
void *row_deselect( Row *row );
void *row_select_ensure( Row *row );
void *row_select( Row *row );
void *row_select_extend( Row *row );
void *row_select_toggle( Row *row );
void row_select_modifier( Row *row, guint state );

void row_show_dependants( Row *row );
void row_hide_dependants( Row *row );
void row_set_status( Row *row );

Row *row_parse_name( Symbol *context, const char *path );

