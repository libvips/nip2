/* a group in a workspace
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

#define TYPE_GROUP (group_get_type())
#define GROUP( obj ) (GTK_CHECK_CAST( (obj), TYPE_GROUP, Group ))
#define GROUP_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_GROUP, GroupClass ))
#define IS_GROUP( obj ) (GTK_CHECK_TYPE( (obj), TYPE_GROUP ))
#define IS_GROUP_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_GROUP ))

typedef struct _Group {
	Value parent_object;

} Group;

typedef struct _GroupClass {
	ValueClass parent_class;

	/* My methods.
	 */
} GroupClass;

GType group_get_type( void );
gboolean group_save_item( PElement *item, char *filename );
