/* a list widget ... which you can reorder with dragndrop.
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

#ifndef ORDERLIST_H
#define ORDERLIST_H

#include <gdk/gdk.h>
#include <gtk/gtkwindow.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TYPE_ORDERLIST (orderlist_get_type())
#define ORDERLIST( obj ) (GTK_CHECK_CAST( (obj), TYPE_ORDERLIST, Orderlist ))
#define ORDERLIST_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_ORDERLIST, OrderlistClass ))
#define IS_ORDERLIST( obj ) (GTK_CHECK_TYPE( (obj), TYPE_ORDERLIST ))
#define IS_ORDERLIST_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_ORDERLIST ))

typedef struct _Orderlist {
	GtkList list;

	/* My instance vars.
	 */
	GtkWidget *cname;		/* New option entry */
} Orderlist;

typedef struct _OrderlistClass {
	GtkListClass parent_class;

	/* My methods.
	 */
} OrderlistClass;

void orderlist_add_label( Orderlist *ol, const char *name, int pos );
void orderlist_scan( Orderlist *ol );
GtkType orderlist_get_type( void );
GtkWidget *orderlist_new( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ORDERLIST_H */
