/* a ip region class in a workspace
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

#define TYPE_IREGION (iregion_get_type())
#define IREGION( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_IREGION, iRegion ))
#define IREGION_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_IREGION, iRegionClass))
#define IS_IREGION( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IREGION ))
#define IS_IREGION_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IREGION ))
#define IREGION_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_IREGION, iRegionClass ))

/* Handy for indexing arrays.
 */
typedef enum iRegionType {
	IREGION_MARK = 0,
	IREGION_HGUIDE,
	IREGION_VGUIDE,
	IREGION_ARROW,
	IREGION_REGION,
	IREGION_AREA
} iRegionType;

/* Our instance vars ... packaged up for code sharing.
 */
typedef struct {
	/* Stuff from the heap.
	 */
	Element image_class;		/* Child image class */
	Imageinfo *ii;
	Rect area;

	/* Client display stuff.
	 */
	Classmodel *classmodel;
	iRegiongroup *iregiongroup;
} iRegionInstance;

struct _iRegion {
	iImage parent_class;

	/* Class fields shared with iarrow.c.
	 */
	iRegionInstance instance;
};

typedef struct _iRegionClass {
	iImageClass parent_class;

	/* My methods.
	 */
} iRegionClass;

void iregion_instance_destroy( iRegionInstance *instance );
void iregion_instance_init( iRegionInstance *instance, Classmodel *classmodel );
gboolean iregion_instance_update( iRegionInstance *instance, PElement *root );

void iregion_edit( GtkWidget *parent, Model *model );
void iregion_parent_add( iContainer *child );
xmlNode *iregion_save( Model *model, xmlNode *xnode );
gboolean iregion_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode );

void *iregion_update_heap( Heapmodel *heapmodel );

gboolean iregion_class_get( Classmodel *classmodel, PElement *root );
gboolean iregion_class_new( Classmodel *classmodel, 
	PElement *fn, PElement *out );

GType iregion_get_type( void );
