/* an editable string in a workspace
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

#define TYPE_STRING (string_get_type())
#define STRING( obj ) (GTK_CHECK_CAST( (obj), TYPE_STRING, String ))
#define STRING_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_STRING, StringClass ))
#define IS_STRING( obj ) (GTK_CHECK_TYPE( (obj), TYPE_STRING ))
#define IS_STRING_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_STRING ))

struct _String {
	Classmodel parent_class;

	/* Class fields.
	 */
	char *value;
};

typedef struct _StringClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} StringClass;

GType string_get_type( void );
