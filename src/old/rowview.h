/* a rowview in a workspace ... part of a tallycolumn, not a separate widget
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

#define TYPE_ROWVIEW (rowview_get_type())
#define ROWVIEW( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_ROWVIEW, Rowview ))
#define ROWVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_ROWVIEW, RowviewClass ))
#define IS_ROWVIEW( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_ROWVIEW ))
#define IS_ROWVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_ROWVIEW ))

struct _Rowview {
	View view;

	Subcolumnview *sview;	/* Enclosing subcolumnview */

	Rhsview *rhsview;	/* Our rhs */

	gboolean visible;	/* Currently visible */
        int rnum;		/* Row of tallycolumn we are in */

        GtkWidget *spin;   	/* Class display open/close widgets */
        GtkWidget *but;   	/* Name button */
        GtkWidget *led;      	/* Indicators */
        GtkWidget *label;   	/* Name label */

	char *last_tooltip;	/* Last tooltip we set */
};

typedef struct _RowviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} RowviewClass;

guint rowview_menu_attach( Rowview *rview, GtkWidget *widget );

GType rowview_get_type( void );
View *rowview_new( void );

void rowview_get_position( Rowview *rview, int *x, int *y, int *w, int *h );
void rowview_set_visible( Rowview *rview, gboolean visible );
gboolean rowview_get_visible( Rowview *rview );
