/* coordinate the display of regionviews on imageviews
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

#define TYPE_IREGIONGROUPVIEW (iregiongroupview_get_type())
#define IREGIONGROUPVIEW( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_IREGIONGROUPVIEW, iRegiongroupview ))
#define IREGIONGROUPVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_IREGIONGROUPVIEW, \
		iRegiongroupviewClass ))
#define IS_IREGIONGROUPVIEW( obj ) \
	(GTK_CHECK_TYPE( (obj), TYPE_IREGIONGROUPVIEW ))
#define IS_IREGIONGROUPVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_IREGIONGROUPVIEW ))

typedef struct _iRegiongroupview {
	View parent_class;

	/* Keep our classmodel here, we need it during destroy.
	 */
	Classmodel *classmodel;

} iRegiongroupview;

typedef struct _iRegiongroupviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} iRegiongroupviewClass;

GType iregiongroupview_get_type( void );
View *iregiongroupview_new( void );
