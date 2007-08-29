/* the rhs of a row ... group together everything to the right of the
 * button
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

#define TYPE_RHS (rhs_get_type())
#define RHS( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_RHS, Rhs ))
#define RHS_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_RHS, RhsClass))
#define IS_RHS( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_RHS ))
#define IS_RHS_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_RHS ))
#define RHS_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_RHS, RhsClass ))

/* Which children are visible.
 */
typedef enum {
	RHS_GRAPHIC = 1,		/* Graphical display */
	RHS_SCOL = 2,			/* Class browser display */
	RHS_ITEXT = 4			/* Textual display */
} RhsFlags;

struct _Rhs {
	Heapmodel parent_class;

	int vislevel;		/* Visibility level */
	RhsFlags flags;		/* Which children we want visible */

        Model *graphic;		/* Graphic display ... toggle/slider/etc */
        Model *scol;		/* Class display */
        Model *itext;		/* Text display */
};

typedef struct _RhsClass {
	HeapmodelClass parent_class;

	/* My methods.
	 */
} RhsClass;

GType rhs_get_type( void );
Rhs *rhs_new( Row *row );

void rhs_set_vislevel( Rhs *rhs, int vislevel );
void rhs_vislevel_up( Rhs *rhs );
void rhs_vislevel_down( Rhs *rhs );

gboolean rhs_child_edited( Rhs *rhs );
