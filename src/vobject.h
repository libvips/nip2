/* abstract base class for a view ... watch an iobject and call _refresh in 
 * idle if it changes.
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

#define TYPE_VOBJECT (vobject_get_type())
#define VOBJECT( obj ) (GTK_CHECK_CAST( (obj), TYPE_VOBJECT, vObject ))
#define VOBJECT_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_VOBJECT, vObjectClass ))
#define IS_VOBJECT( obj ) (GTK_CHECK_TYPE( (obj), TYPE_VOBJECT ))
#define IS_VOBJECT_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_VOBJECT ))
#define VOBJECT_GET_CLASS( obj ) \
	(GTK_CHECK_GET_CLASS( (obj), TYPE_VOBJECT, vObjectClass ))

struct _vObject {
	GtkVBox vbox;

	/* My instance vars.
	 */
	iObject *iobject;		/* iObject we are watching */
	guint changed_sid;		/* Signals we use to watch model */
	guint destroy_sid;

	gboolean dirty;			/* In need of refreshment */
};

typedef struct _vObjectClass {
	GtkVBoxClass parent_class;

	/* State change

		refresh		refresh widgets (don't look at heap value,
				look at model)

		link		this vobject has been linked to an iobject

				we also have View::link() -- vObject::link is
				a lower-level link which is handy for views 
				which are not Views, eg. toolkitbrowser

	 */
	void (*refresh)( vObject * );
	void (*link)( vObject *, iObject * );
} vObjectClass;

void *vobject_refresh_queue( vObject *vobject );

GType vobject_get_type( void );
void vobject_base_init( void );

void vobject_link( vObject *vobject, iObject *iobject );

void *vobject_refresh( vObject *vobject );
