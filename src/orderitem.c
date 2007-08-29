/* orderitem ... a gtklistitem which can be moved with dragndrop.
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

#include <vips/vips.h>
#include <vips/util.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "orderitem.h"

/* Debug msg.
#define DEBUG
 */

static GtkListItemClass *parent_class = NULL;

typedef enum {
	TARGET_STRING,
	TARGET_ROOTWIN
} OrderitemTarget;

static GtkTargetEntry target_table[] = {
	{ "STRING",     0, TARGET_STRING },
	{ "text/plain", 0, TARGET_STRING },
	{ "application/x-rootwin-drop", 0, TARGET_ROOTWIN }
};

static void
orderitem_destroy( GtkObject *object )
{
	Orderitem *oi;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_ORDERITEM( object ) );

	oi = ORDERITEM( object );

	/* My instance destroy stuff.
	 */

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
orderitem_class_init( OrderitemClass *class )
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass*) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = orderitem_destroy;

	/* Create signals.
	 */

	/* Init default methods.
	 */
}

gchar *
orderitem_get_label( GtkWidget *widget )
{
	gchar *lab;

	if( widget && GTK_IS_LABEL( GTK_BIN( widget )->child ) )
		gtk_label_get( GTK_LABEL( GTK_BIN( widget )->child ), &lab );
	else
		lab = "untitled";

	return( lab );
}

static void
orderitem_insert( GtkWidget *item, GtkWidget *source_widget, gchar *name )
{
	GtkWidget *list = item->parent;
	GtkWidget *new_item;
	gint pos = gtk_list_child_position( GTK_LIST( list ), item );

	if( source_widget ) {
		gint old_pos = gtk_list_child_position( GTK_LIST( list ), 
			source_widget );

		if( pos > old_pos )
			pos += 1;
	}

	new_item = orderitem_new_with_label( name );
	gtk_widget_show( new_item );
	gtk_list_insert_items( GTK_LIST( list ), 
		g_list_prepend( NULL, new_item ), pos );
}

static void
orderitem_delete( GtkWidget *item )
{
	gtk_list_remove_items( GTK_LIST( item->parent ), 
		g_list_prepend( NULL, item ) );
}

static void
orderitem_drag_data_received( GtkWidget *widget, GdkDragContext *context, 
	gint x, gint y,
	GtkSelectionData *data,
	guint info, guint time )
{
	GtkWidget *source_widget;

#ifdef DEBUG
	g_message( "orderitem_drag_data_received\n" );
#endif /*DEBUG*/

	source_widget = gtk_drag_get_source_widget( context );
	if( source_widget ) 
		orderitem_insert( widget, source_widget, 
			orderitem_get_label( source_widget ) );
	else if( data->length >= 0 && data->format == 8 ) {
		orderitem_insert( widget, NULL, (gchar *) data->data );
		gtk_drag_finish( context, TRUE, FALSE, time );
		return;
	}

	gtk_drag_finish( context, FALSE, FALSE, time );
}

static void
orderitem_drag_data_get( GtkWidget *widget, GdkDragContext *context,
	GtkSelectionData *selection_data, guint info,
	guint time,
	gpointer data )
{
	Orderitem *oi;
	gchar *lab;

#ifdef DEBUG
	g_message( "orderitem_drag_data_get\n" );
#endif /*DEBUG*/

	oi = ORDERITEM( widget );

	switch( info ) {
	case TARGET_ROOTWIN:
		oi->delete_me = TRUE;
		break;

	case TARGET_STRING:
		lab = orderitem_get_label( widget );
		gtk_selection_data_set( selection_data,
			selection_data->target, 8, 
			(guchar *) lab, strlen( lab ) );
		break;
	}
}

static void
orderitem_drag_data_delete( GtkWidget *widget, GdkDragContext *context,
	gpointer data )
{
	Orderitem *oi;

#ifdef DEBUG
	g_message( "orderitem_drag_data_delete\n" );
#endif /*DEBUG*/

	oi = ORDERITEM( widget );
	oi->delete_me = TRUE;
}

static void
orderitem_drag_end( GtkWidget *widget, GdkDragContext *context,
	gpointer data )
{
	Orderitem *oi;

#ifdef DEBUG
	g_message( "orderitem_drag_end\n" );
#endif /*DEBUG*/

	oi = ORDERITEM( widget );
	if( oi->delete_me )
		orderitem_delete( widget );
}

static void
orderitem_drag_begin( GtkWidget *widget, GdkDragContext *context,
	gpointer data )
{
	Orderitem *oi;

#ifdef DEBUG
	g_message( "orderitem_drag_begin\n" );
#endif /*DEBUG*/

	oi = ORDERITEM( widget );
	oi->delete_me = FALSE;
}

static void
orderitem_init( Orderitem *oi )
{
	/* Init our instance fields.
	 */

	/* Can be source and destination.
	 */
	gtk_drag_dest_set( GTK_WIDGET( oi ), GTK_DEST_DEFAULT_ALL, 
		target_table, IM_NUMBER( target_table ),
		GDK_ACTION_MOVE );
	gtk_drag_source_set( GTK_WIDGET( oi ), GDK_BUTTON1_MASK,
		target_table, IM_NUMBER( target_table ),
		GDK_ACTION_MOVE );

	/* Attach signals.
	 */
	gtk_signal_connect( GTK_OBJECT( oi ), "drag_data_received",
		GTK_SIGNAL_FUNC( orderitem_drag_data_received ), NULL );
	gtk_signal_connect( GTK_OBJECT( oi ), "drag_data_get",
		GTK_SIGNAL_FUNC( orderitem_drag_data_get ), NULL);
	gtk_signal_connect( GTK_OBJECT( oi ), "drag_data_delete",
		GTK_SIGNAL_FUNC( orderitem_drag_data_delete ), NULL);
	gtk_signal_connect( GTK_OBJECT( oi ), "drag_end",
		GTK_SIGNAL_FUNC( orderitem_drag_end ), NULL);
	gtk_signal_connect( GTK_OBJECT( oi ), "drag_begin",
		GTK_SIGNAL_FUNC( orderitem_drag_begin ), NULL);
}

GtkType
orderitem_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Orderitem",
			sizeof( Orderitem ),
			sizeof( OrderitemClass ),
			(GtkClassInitFunc) orderitem_class_init,
			(GtkObjectInitFunc) orderitem_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( GTK_TYPE_LIST_ITEM, &info );
	}

	return( type );
}

GtkWidget *
orderitem_new( void )
{
	Orderitem *oi = gtk_type_new( TYPE_ORDERITEM );

	/* Do _new(args) init.
	 */

	return( GTK_WIDGET( oi ) );
}

GtkWidget *
orderitem_new_with_label( const gchar *label )
{
	GtkWidget *list_item;
	GtkWidget *label_widget;

	list_item = orderitem_new();
	label_widget = gtk_label_new( label );
	gtk_misc_set_alignment( GTK_MISC( label_widget ), 0.0, 0.5 );

	gtk_container_add( GTK_CONTAINER( list_item ), label_widget );
	gtk_widget_show( label_widget );

	return( list_item );
}


