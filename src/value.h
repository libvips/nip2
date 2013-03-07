/* abstract base class for real/group/vector
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

#define TYPE_VALUE (value_get_type())
#define VALUE( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_VALUE, Value ))
#define VALUE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_VALUE, ValueClass))
#define IS_VALUE( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_VALUE ))
#define IS_VALUE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_VALUE ))
#define VALUE_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_VALUE, ValueClass ))

typedef struct _Value {
	Classmodel model;

	/* Build caption buffer here.
	 */
	VipsBuf caption_buffer;
} Value;

typedef struct _ValueClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} ValueClass;

GType value_get_type( void );
