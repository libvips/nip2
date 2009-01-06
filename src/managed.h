/* managed objects ... things like Imageinfo which are lifetime managed by
 * both the GC and by pointers from C: we need to both mark/sweep and refcount
 * 
 * abstract class: Managedgvalue, Imageinfo, etc. build off this
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

#define TYPE_MANAGED (managed_get_type())
#define MANAGED( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_MANAGED, Managed ))
#define MANAGED_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_MANAGED, ManagedClass))
#define IS_MANAGED( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_MANAGED ))
#define IS_MANAGED_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_MANAGED ))
#define MANAGED_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_MANAGED, ManagedClass ))

#define MANAGED_UNREF( X ) { \
	if( X ) { \
		managed_destroy_nonheap( MANAGED( X ) ); \
		X = NULL; \
	} \
}
#define MANAGED_REF( X ) managed_dup_nonheap( MANAGED( X ) )

struct _Managed {
	iContainer parent_object;

	/* Can't just set ->heap = NULL to mean unattached, our subclasses
	 * rely on ->heap being valid even during dispose.
	 */

	Heap *heap;		/* Heap we are attached to */
	gboolean attached;	/* If we are attached to the heap */

	gboolean marked;	/* For mark-sweep */
	int count;		/* Number of non-heap pointers to us */
	gboolean zombie;	/* Unreffed, but being kept alive */
	double time;		/* When we became a zombie */

	/* 

	   	FIXME ... This should go with vips8: it does dependency 
		tracking for us.

	 */
	GSList *sub;		/* Sub-objects ... mark these if we mark this */
};

typedef struct _ManagedClass {
	iContainerClass parent_class;

	/* How long after zombiefying before we unref.
	 */
	double keepalive;
} ManagedClass;

void managed_check_all_destroyed( void );

void managed_link_heap( Managed *managed, Heap *heap );

void managed_destroy_heap( Managed *managed );
void managed_destroy_nonheap( Managed *managed );
void managed_dup_nonheap( Managed *managed );

void *managed_sub_remove( Managed *in, Managed *managed );
void managed_sub_add( Managed *managed, Managed *in );
void managed_sub_add_all( Managed *out, int nin, Managed **in );

GType managed_get_type( void );

void managed_clear( Heap *heap );
void managed_mark( Managed *managed );
gboolean managed_free_unused( Heap *heap );
