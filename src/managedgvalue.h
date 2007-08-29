/* a managed gvalue 
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

#define TYPE_MANAGEDGVALUE (managedgvalue_get_type())
#define MANAGEDGVALUE( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_MANAGEDGVALUE, Managedgvalue ))
#define MANAGEDGVALUE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_MANAGEDGVALUE, ManagedgvalueClass))
#define IS_MANAGEDGVALUE( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_MANAGEDGVALUE ))
#define IS_MANAGEDGVALUE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_MANAGEDGVALUE ))
#define MANAGEDGVALUE_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), \
		TYPE_MANAGEDGVALUE, ManagedgvalueClass ))

struct _Managedgvalue {
	Managed parent_object;

	GValue value;
};

typedef struct _ManagedgvalueClass {
	ManagedClass parent_class;

} ManagedgvalueClass;

GType managedgvalue_get_type( void );

void managedgvalue_set_value( Managedgvalue *managedgvalue, GValue *value );
Managedgvalue *managedgvalue_new( Heap *heap, GValue *value );
