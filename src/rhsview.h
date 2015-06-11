/* the rhs of a tallyrow ... group together everything to the right of the
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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#define TYPE_RHSVIEW (rhsview_get_type())
#define RHSVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_RHSVIEW, Rhsview ))
#define RHSVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_RHSVIEW, RhsviewClass ))
#define IS_RHSVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_RHSVIEW ))
#define IS_RHSVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_RHSVIEW ))

struct _Rhsview {
	View item;

	Rowview *rview;

	View *graphic;			/* Our three elements */
	View *scol;
	View *itext;

        GtkWidget *table;		/* Lay out elements in this */
	RhsFlags flags;			/* Last vis set we set */
};

typedef struct _RhsviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} RhsviewClass;

GType rhsview_get_type( void );
View *rhsview_new( void );
