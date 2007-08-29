/* a real in a workspace
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

#define TYPE_REAL (real_get_type())
#define REAL( obj ) (GTK_CHECK_CAST( (obj), TYPE_REAL, Real ))
#define REAL_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_REAL, RealClass ))
#define IS_REAL( obj ) (GTK_CHECK_TYPE( (obj), TYPE_REAL ))
#define IS_REAL_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_REAL ))

typedef struct _Real {
	Value parent_object;

	/* Private ... build iobject caption here.
	 */
} Real;

typedef struct _RealClass {
	ValueClass parent_class;

	/* My methods.
	 */
} RealClass;

GType real_get_type( void );
