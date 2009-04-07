/* a list item widget ... which you can move with dragndrop.
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

#ifndef ORDERITEM_H
#define ORDERITEM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TYPE_ORDERITEM (orderitem_get_type())
#define ORDERITEM( obj ) (GTK_CHECK_CAST( (obj), TYPE_ORDERITEM, Orderitem ))
#define ORDERITEM_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_ORDERITEM, OrderitemClass ))
#define IS_ORDERITEM( obj ) (GTK_CHECK_TYPE( (obj), TYPE_ORDERITEM ))
#define IS_ORDERITEM_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_ORDERITEM ))

typedef enum {
	DRAG_NONE,
	DRAG_BEGIN,
	DRAG_INPROGRESS
} OrderitemDragState;

typedef struct _Orderitem {
	GtkListItem item;

	/* My instance vars.
	 */
	gint delete_me;		/* This item to be deleted on end of drag */
} Orderitem;

typedef struct _OrderitemClass {
	GtkListItemClass parent_class;

	/* My methods.
	 */
} OrderitemClass;

extern GtkType orderitem_get_type( void );
extern GtkWidget *orderitem_new( void );
extern GtkWidget *orderitem_new_with_label( const gchar *label );
extern gchar *orderitem_get_label( GtkWidget *widget );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ORDERITEM_H */

