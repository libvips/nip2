/* a managed STRING* ... for lazy string read
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

    These strings are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#define TYPE_MANAGEDSTRING (managedstring_get_type())
#define MANAGEDSTRING( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_MANAGEDSTRING, Managedstring ))
#define MANAGEDSTRING_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_MANAGEDSTRING, ManagedstringClass))
#define IS_MANAGEDSTRING( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_MANAGEDSTRING ))
#define IS_MANAGEDSTRING_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_MANAGEDSTRING ))
#define MANAGEDSTRING_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), \
		TYPE_MANAGEDSTRING, ManagedstringClass ))

struct _Managedstring {
	Managed parent_object;

	const char *string;
	Element e;		/* Points to compiled string */
};

typedef struct _ManagedstringClass {
	ManagedClass parent_class;

} ManagedstringClass;

GType managedstring_get_type( void );

Managedstring *managedstring_find( Heap *heap, const char *string );
