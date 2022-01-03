/* a pathname in a workspace
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

#define TYPE_PATHNAME (pathname_get_type())
#define PATHNAME( obj ) (G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_PATHNAME, Pathname ))
#define PATHNAME_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_PATHNAME, PathnameClass ))
#define IS_PATHNAME( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_PATHNAME ))
#define IS_PATHNAME_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_PATHNAME ))

typedef struct _Pathname {
	Classmodel model;

	char *value;
} Pathname;

typedef struct _PathnameClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} PathnameClass;

GType pathname_get_type( void );
