/* orderlist ... a gtklist with an entry box, reorderable elements and a
 * right-button menu.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /*HAVE_UNISTD_H*/
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <vips/vips.h>
#include <vips/rect.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

#include "util.h"
#include "gtkutil.h"
#include "orderitem.h"
#include "orderlist.h"

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) gettext(String)
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) (String)
#endif
#else /* NLS is disabled */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(String) (String)
#define gettext(String) (String)
#define dgettext(Domain,String) (String)
#define dcgettext(Domain,String,Type) (String)
#define bindtextdomain(Domain,Directory) (Domain) 
#define bind_textdomain_codeset(Domain,Codeset) (Codeset) 
#define ngettext(S, P, N) ((N) == 1 ? (S) : (P))
#endif /* ENABLE_NLS */

static GtkListClass *parent_class = NULL;

static GtkWidget *orderlist_menu = NULL;

static void
orderlist_destroy( GtkObject *object )
{
	Orderlist *ol;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_ORDERLIST( object ) );

	ol = ORDERLIST( object );

	/* My instance destroy stuff.
	 */

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

/* Delete the selected items in a list.
 */
static void
list_delete_selected( GtkList *list )
{	
	GList *clear_list = NULL;
	GList *p;

	for( p = list->selection; p; p = p->next )
		clear_list = g_list_prepend( clear_list, p->data );
	clear_list = g_list_reverse( clear_list );

	gtk_list_remove_items( list, clear_list );

	g_list_free( clear_list );
}

/* Delete this item.
 */
/*ARGSUSED*/
static void
orderlist_delete_cb( GtkWidget *menu, GtkWidget *host, GtkWidget *item )
{
	GtkWidget *list = item->parent;

	gtk_list_select_child( GTK_LIST( list ), item );
	list_delete_selected( GTK_LIST( list ) );
}

/* Delete selected items.
 */
/*ARGSUSED*/
static void
orderlist_delete_selected_cb( GtkWidget *menu, 
	GtkWidget *host, GtkWidget *item )
{
	GtkWidget *list = item->parent;

	list_delete_selected( GTK_LIST( list ) );
}

/* Delete all items.
 */
/*ARGSUSED*/
static void
orderlist_delete_all_cb( GtkWidget *menu, GtkWidget *host, GtkWidget *item )
{
	GtkWidget *list = item->parent;
	GList *children = GTK_LIST( list )->children;
	int len = g_list_length( children );

	if( len > 1 ) {
		GtkWidget *last_item;

		/* :-( gtk hates deleting the object with focus from a popup.
		 * Warp focus to the last widget.
		 */
		last_item = g_list_nth_data( children, len - 1 );
		gtk_widget_grab_focus( GTK_BIN( last_item )->child );

		gtk_list_clear_items( GTK_LIST( list ), 0, len - 1 );
	}
}

static void
orderlist_class_init( OrderlistClass *class )
{
	GtkObjectClass *object_class;
	GtkWidget *pane;

	object_class = (GtkObjectClass*) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = orderlist_destroy;

	/* Create signals.
	 */

	/* Init default methods.
	 */

	pane = orderlist_menu = popup_build( _( "Orderlist menu" ) );
	popup_add_but( pane, _( "_Delete" ), 
		POPUP_FUNC( orderlist_delete_cb ) );
	popup_add_but( pane, _( "Delete _Selected" ), 
		POPUP_FUNC( orderlist_delete_selected_cb ) );
	popup_add_but( pane, _( "Delete _All" ), 
		POPUP_FUNC( orderlist_delete_all_cb ) );
}

/* Add a label to a list.
 */
void
orderlist_add_label( Orderlist *ol, const char *name, int pos )
{
	int len = g_list_length( GTK_LIST( ol )->children );
	GtkWidget *item;

	/* Don't append after entry.
	 */
	if( pos < 0 || pos == len )
		pos = len - 1;

	item = orderitem_new_with_label( name );
	gtk_list_insert_items( GTK_LIST( ol ), 
		g_list_prepend( NULL, item ), pos );
	gtk_widget_show( item );
        popup_attach( item, orderlist_menu, item );
	set_tooltip( item, _( "Current options - right button for menu" ) );
}

/* Scan the entry widget.
 */
void
orderlist_scan( Orderlist *ol )
{
	char *cname = gtk_editable_get_chars( 
		GTK_EDITABLE( ol->cname ), 0, -1 );
	int len = g_list_length( GTK_LIST( ol )->children );

	/* Empty? Do nothing.
	 */
	if( strcmp( cname, "" ) != 0 ) {
		GtkAdjustment *adj;
		GtkWidget *par = GTK_WIDGET( ol )->parent->parent;

		orderlist_add_label( ORDERLIST( ol ), cname, len - 1 );
		set_gentry( ol->cname, "%s", "" );

		/* Scroll to bottom.
		 */
		par = GTK_WIDGET( ol )->parent->parent;
		if( GTK_IS_SCROLLED_WINDOW( par ) ) {
			adj = gtk_scrolled_window_get_vadjustment( 
				GTK_SCROLLED_WINDOW( par ) );
			adj->value = adj->upper;
			gtk_adjustment_value_changed( adj );
		}
	}

	g_free( cname );
}

/* Activate in the new item entry widget.
 */
static void
orderlist_add_cb( GtkWidget *widget, Orderlist *ol )
{
	orderlist_scan( ol );
}

/*ARGSUSED*/
static void
orderlist_deselect_cb( GtkWidget *widget, gpointer *dummy )
{
	gtk_item_deselect( GTK_ITEM( widget ) );
}

/*ARGSUSED*/
static void
orderlist_warpfocus_cb( GtkWidget *widget, GdkEventFocus *event, GtkWidget *to )
{
	gtk_widget_grab_focus( to );
}

static void
orderlist_init( Orderlist *ol )
{
	GtkWidget *oi;

	/* Make the append.
	 */
	ol->cname = gtk_entry_new();
	gtk_signal_connect( GTK_OBJECT( ol->cname ), "activate",
		GTK_SIGNAL_FUNC( orderlist_add_cb ), ol );
	oi = orderitem_new();
	gtk_signal_connect( GTK_OBJECT( oi ), "select",
		GTK_SIGNAL_FUNC( orderlist_deselect_cb ), NULL );
	gtk_signal_connect( GTK_OBJECT( oi ), "focus-in-event",
		GTK_SIGNAL_FUNC( orderlist_warpfocus_cb ), ol->cname );
	gtk_container_add( GTK_CONTAINER( oi ), ol->cname );
	gtk_list_insert_items( GTK_LIST( ol ), g_list_prepend( NULL, oi ), -1 );
	set_tooltip( ol->cname, _( "Enter new option fields here" ) );
}

GtkType
orderlist_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Orderlist",
			sizeof( Orderlist ),
			sizeof( OrderlistClass ),
			(GtkClassInitFunc) orderlist_class_init,
			(GtkObjectInitFunc) orderlist_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( GTK_TYPE_LIST, &info );
	}

	return( type );
}

GtkWidget *
orderlist_new( void )
{
	Orderlist *ol = gtk_type_new( TYPE_ORDERLIST );

	/* Do _new(args) init.
	 */

	return( GTK_WIDGET( ol ) );
}
