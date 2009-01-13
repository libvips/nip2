/* abstract base class for all nip objects
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

#define TYPE_IOBJECT (iobject_get_type())
#define IOBJECT( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_IOBJECT, iObject ))
#define IOBJECT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_IOBJECT, iObjectClass))
#define IS_IOBJECT( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IOBJECT ))
#define IS_IOBJECT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IOBJECT ))
#define IOBJECT_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_IOBJECT, iObjectClass ))

/* Handy iobject_destroy() shortcut.
 */
#define IDESTROY( O ) { \
	if( O ) { \
		(void) iobject_destroy( IOBJECT( O ) ); \
		( O ) = NULL; \
	} \
}

struct _iObject {
	GObject parent_object;

	/* My instance vars.
	 */
        char *name;             /* iObject name */
        char *caption;          /* Comment of some sort */

	/* True when created ... the 1 reference that gobject makes is
	 * 'floating' and not owned by anyone. Do _sink() after every _ref()
	 * to transfer ownership to the parent container. Upshot: no need to
	 * _unref() after _add() in _new().
	 */
	gboolean floating;

	/* Stop destroy loops with this.
	 */
	gboolean in_destruction;
};

typedef struct _iObjectClass {
	GObjectClass parent_class;

	/* End object's lifetime, just like gtk_object_destroy.
	 */
	void (*destroy)( iObject * );

	/* Something about the object has changed. Should use glib's properties
	 * but fix this later.
	 */
	void (*changed)( iObject * );

	/* Try and say something useful about us.
	 */
	void (*info)( iObject *, VipsBuf * );

	/* Called on _changed() to update the caption. Define this if you want
	 * the caption to be an explanatory note about the object. 
	 */
	const char *(*generate_caption)( iObject * );
} iObjectClass;

void *iobject_destroy( iObject *iobject );
void *iobject_changed( iObject *iobject );
void *iobject_info( iObject *iobject, VipsBuf * );

GType iobject_get_type( void );

void *iobject_test_name( iObject *iobject, const char *name );
void *iobject_print( iObject *iobject );
void iobject_set( iObject *iobject, const char *name, const char *caption );
void iobject_sink( iObject *iobject );
void iobject_dump( iObject *iobject );
