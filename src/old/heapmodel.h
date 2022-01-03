/* like a model, but something that represents a part of the heap (eg.
 * toggle/slider/text etc.)
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

#define TYPE_HEAPMODEL (heapmodel_get_type())
#define HEAPMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_HEAPMODEL, Heapmodel ))
#define HEAPMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_HEAPMODEL, HeapmodelClass))
#define IS_HEAPMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_HEAPMODEL ))
#define IS_HEAPMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_HEAPMODEL ))
#define HEAPMODEL_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_HEAPMODEL, HeapmodelClass ))

struct _Heapmodel {
	Model parent_class;

	/* Context.
	 */
	Row *row;		/* Enclosing row */
	Rhs *rhs;		/* Enclosing rhs */

	/* Set if model has changes which have not yet been applied to the
	 * heap ... update_model() blocks, update_heap() clears.
	 */
	gboolean modified;
};

typedef struct _HeapmodelClass {
	ModelClass parent_class;

	/* Building heaps from models, building models from heaps.

		new_heap	the heap has changed ... recurse down adding,
				updating (with a recursive new_heap()) and 
				removing children

		update_model	read the heap into the model ... eg. update
				text representation

		update_heap	if the heapmodel has any unapplied user edits,
				use them to update the heap ... update the
				heap area pointed to by the last
				update_model

		clear_edited	set back to default values

	 */
	void *(*new_heap)( Heapmodel *, PElement * );	
	void *(*update_model)( Heapmodel * );
	void *(*update_heap)( Heapmodel * );
	void *(*clear_edited)( Heapmodel * );
} HeapmodelClass;

void *heapmodel_new_heap( Heapmodel *heapmodel, PElement *root );
void *heapmodel_update_model( Heapmodel *heapmodel );
void *heapmodel_update_heap( Heapmodel *heapmodel );
void *heapmodel_clear_edited( Heapmodel *heapmodel );

GType heapmodel_get_type( void );

void heapmodel_set_modified( Heapmodel *heapmodel, gboolean modified );
gboolean heapmodel_name( Heapmodel *heapmodel, VipsBuf *buf );
gboolean heapmodel_value( Heapmodel *heapmodel, VipsBuf *buf );
