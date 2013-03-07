/* abstract base class for containers
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

#define TYPE_ICONTAINER (icontainer_get_type())
#define ICONTAINER( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_ICONTAINER, iContainer ))
#define ICONTAINER_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_ICONTAINER, iContainerClass))
#define IS_ICONTAINER( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_ICONTAINER ))
#define IS_ICONTAINER_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_ICONTAINER ))
#define ICONTAINER_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_ICONTAINER, iContainerClass ))

/* Test for is C a child of P.
 */
#define ICONTAINER_IS_CHILD( P, C ) \
	(g_slist_find( ICONTAINER( P )->children, ICONTAINER( C ) ) && \
	ICONTAINER( C )->parent == ICONTAINER( P ))

struct _iContainer {
	iObject parent_object;

	/* My instance vars.
	 */
	GSList *children;	/* iContainers which are inside this one */
	int pos;		/* Position in parent */
	iContainer *parent;	/* iContainer we are inside */
	guint destroy_sid;	/* Parent listens for our "destroy" here */
	GHashTable *child_hash;	/* Optional: hash of children by their name */
};

typedef struct _iContainerClass {
	iObjectClass parent_class;

	/* 

		pos_changed	our pos has changed

		child_add	a child has been added to us

		child_remove	a child is about be removed from us

		parent_add	parent has been attached

		parent_remove	parent is about to be removed

	 */

	void (*pos_changed)( iContainer *icontainer );
	void (*child_add)( iContainer *parent, iContainer *child, int );
	void (*child_remove)( iContainer *parent, iContainer *child );
	void (*parent_add)( iContainer *child );
	void (*parent_remove)( iContainer *child );
} iContainerClass;

typedef void *(*icontainer_map_fn)( iContainer *, 
	void *, void * );
typedef void *(*icontainer_map3_fn)( iContainer *, 
	void *, void *, void * );
typedef void *(*icontainer_map4_fn)( iContainer *, 
	void *, void *, void *, void * );
typedef void *(*icontainer_map5_fn)( iContainer *, 
	void *, void *, void *, void *, void * );

typedef gint (*icontainer_sort_fn)( iContainer *a, iContainer *b );

int icontainer_get_n_children( iContainer *icontainer );
GSList *icontainer_get_children( iContainer *icontainer );
void *icontainer_map( iContainer *icontainer, 
	icontainer_map_fn fn, void *a, void *b );
void *icontainer_map3( iContainer *icontainer, 
	icontainer_map3_fn fn, void *a, void *b, void *c );
void *icontainer_map4( iContainer *icontainer, 
	icontainer_map4_fn fn, void *a, void *b, void *c, void *d );
void *icontainer_map5( iContainer *icontainer, 
	icontainer_map5_fn fn, void *a, void *b, void *c, void *d, void *e ); 
void *icontainer_map_rev( iContainer *icontainer, 
	icontainer_map_fn fn, void *a, void *b );
void *icontainer_map_all( iContainer *icontainer, 
	icontainer_map_fn fn, void *a );
void *icontainer_map2_all( iContainer *icontainer, 
	icontainer_map_fn fn, void *a, void *b );
void *icontainer_map3_all( iContainer *icontainer, 
	icontainer_map3_fn fn, void *a, void *b, void *c );
void *icontainer_map4_all( iContainer *icontainer, 
	icontainer_map4_fn fn, void *a, void *b, void *c, void *d );
void *icontainer_map_all_intrans( iContainer *icontainer, 
	icontainer_map_fn fn, void *a );

void icontainer_sanity( iContainer *icontainer );

void icontainer_pos_sort( iContainer *icontainer );
int icontainer_pos_last( iContainer *icontainer );
void icontainer_pos_renumber( iContainer *icontainer );
void icontainer_custom_sort( iContainer *icontainer, GCompareFunc fn );
gint icontainer_name_compare( iContainer *a, iContainer *b );

void icontainer_child_add( iContainer *icontainer, iContainer *child, int pos );
void icontainer_child_add_before( iContainer *parent, 
	iContainer *child, iContainer *before );
void icontainer_child_move( iContainer *child, int pos );
void *icontainer_child_remove( iContainer *child );

GType icontainer_get_type( void );

void icontainer_set_hash( iContainer *icontainer );
iContainer *icontainer_child_lookup( iContainer *parent, const char *name );
