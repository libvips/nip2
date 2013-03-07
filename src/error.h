/* Decls for ierror.c ... show all ierrors
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

#define TYPE_IERROR (ierror_get_type())
#define IERROR( obj ) (GTK_CHECK_CAST( (obj), TYPE_IERROR, iError ))
#define IERROR_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_IERROR, iErrorClass ))
#define IS_IERROR( obj ) (GTK_CHECK_TYPE( (obj), TYPE_IERROR ))
#define IS_IERROR_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_IERROR ))

struct _iError {
	Log parent_class;

	Toolkitgroup *kitg;	/* Where we search for link ierrors */
};

typedef struct _iErrorClass {
	LogClass parent_class;

	/* My methods.
	 */
} iErrorClass;

GtkType ierror_get_type( void );
iError *ierror_new( Toolkitgroup *kitg );

