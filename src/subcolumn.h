/* a column of rows in a workspace
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

#define TYPE_SUBCOLUMN (subcolumn_get_type())
#define SUBCOLUMN( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_SUBCOLUMN, Subcolumn ))
#define SUBCOLUMN_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_SUBCOLUMN, SubcolumnClass))
#define IS_SUBCOLUMN( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_SUBCOLUMN ))
#define IS_SUBCOLUMN_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_SUBCOLUMN ))
#define SUBCOLUMN_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_SUBCOLUMN, SubcolumnClass ))

/* Predicate on a row.
 */
typedef gboolean (*RowPred)( Row * );

/* Control class member visibility with these.
 */
typedef struct {
	const char *name;
	RowPred pred;
} SubcolumnVisibility;

struct _Subcolumn {
	Heapmodel parent_class;

	/* Our context.
	 */
	Column *col;		/* Enclosing column (or NULL) */
	Subcolumn *scol;	/* Enclosing subcolumn (or NULL) */
	Column *top_col;	/* Topmost enclosing column */
	Subcolumn *top_scol;	/* Topmost enclosing subcolumn */

	int vislevel;		/* Visibility level */
	gboolean is_top;	/* TRUE if parent is a column */

	Element base;		/* "this" for our members */
	gboolean known_private;	/* TRUE after top-level clone .. can write! */

	/* For subcolumns representing a class instance, the rows for the
	 * "this" and "super" members.
	 */
	Row *this;
	Row *super;
};

typedef struct _SubcolumnClass {
	HeapmodelClass parent_class;

	/* My methods.
	 */
} SubcolumnClass;

extern const SubcolumnVisibility subcolumn_visibility[];
extern const int subcolumn_nvisibility;

void *subcolumn_map( Subcolumn *scol, row_map_fn fn, void *a, void *b );

GType subcolumn_get_type( void );
void *subcolumn_new_view( Subcolumn *scol );
Subcolumn *subcolumn_new( Rhs *rhs, Column *col );

void subcolumn_set_vislevel( Subcolumn *scol, int vislevel );

gboolean subcolumn_make_private( Subcolumn *scol );
