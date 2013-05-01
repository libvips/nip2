/* a column in a workspace
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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#define TYPE_COLUMN (column_get_type())
#define COLUMN( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_COLUMN, Column ))
#define COLUMN_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_COLUMN, ColumnClass))
#define IS_COLUMN( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_COLUMN ))
#define IS_COLUMN_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_COLUMN ))
#define COLUMN_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_COLUMN, ColumnClass ))

struct _Column {
	Filemodel parent_object;

	/* Our context.
	 */
	Subcolumn *scol;	/* Subcolumn we enclose */
	Workspace *ws;		/* Enclosing workspace */

        /* Appearance state info.
         */
        int x, y;		/* Position */
        gboolean open;		/* Currently popped down */
        gboolean selected;

	/* Other state.
	 */
        int next;		/* Index of next symbol we make */
        Row *last_select;	/* Last row clicked ... for x sel */
};

typedef struct _ColumnClass {
	FilemodelClass parent_class;

	/* My methods.
	 */
} ColumnClass;

void *column_map( Column *col, row_map_fn fn, void *a, void *b );
void *column_map_symbol( Column *col, symbol_map_fn fn, void *a );

void *column_select_symbols( Column *col );

GtkType column_get_type( void );

Column *column_new( Workspace *ws, const char *name );

Column *column_get_last_new( void );
void column_clear_last_new( void );

Row *column_get_bottom( Column *col );
gboolean column_add_n_names( Column *col, 
	const char *name, VipsBuf *buf, int nparam );
gboolean column_is_empty( Column *col );

void column_set_offset( int x_off, int y_off );

char *column_name_new( Column *col );

void column_set_open( Column *col, gboolean open );
