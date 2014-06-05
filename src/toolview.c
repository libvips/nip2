/* Manage toolviewkits and their display.
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

/*
#define DEBUG
 */

#include "ip.h"

static ViewClass *parent_class = NULL;

/* Link menu items to toolview with this.
 */
static GQuark toolview_quark = 0;

static Mainw *
toolview_get_mainw( Toolview *tview )
{
	if( !tview->kview ||
		!tview->kview->kitgview )
		return( NULL );

	return( tview->kview->kitgview->mainw );
}

static Workspace *
toolview_get_workspace( Toolview *tview )
{
	Mainw *mainw;

	if( !(mainw = toolview_get_mainw( tview )) )
		return( NULL );

	return( mainw_get_workspace( mainw ) ); 
}

static Workspace *
item_get_workspace( GtkWidget *item )
{
	Toolview *tview;

	if( !(tview = gtk_object_get_data_by_id( GTK_OBJECT( item ), 
		toolview_quark )) )
		return( NULL );

	return( toolview_get_workspace( tview ) );
}

static void 
toolview_destroy( GtkObject *object )
{	
	Toolview *tview;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_TOOLVIEW( object ) );

	tview = TOOLVIEW( object );

#ifdef DEBUG
	printf( "toolview_destroy: %p\n", object );
#endif /*DEBUG*/

	DESTROY_GTK( tview->item );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
toolview_finalize( GObject *gobject )
{
#ifdef DEBUG
	printf( "toolview_finalize: %p\n", gobject );
#endif /*DEBUG*/

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
toolview_activate_cb( GtkWidget *widget, Toolitem *toolitem )
{
	Workspace *ws = item_get_workspace( widget );

	switch( toolitem->tool->type ) {
	case TOOL_DIA:
		if( !workspace_merge_file( ws, 
			FILEMODEL( toolitem->tool )->filename ) )
			iwindow_alert( widget, GTK_MESSAGE_ERROR );
		symbol_recalculate_all();
		break;

	case TOOL_SYM:
		if( !workspace_add_action( ws, 
			toolitem->name, toolitem->action, 
			toolitem->action_sym->expr->compile->nparam ) )
			iwindow_alert( widget, GTK_MESSAGE_ERROR );
		break;

	default:
		g_assert( FALSE );
	}
}

/* Flash help for a toolview.
 */
static void 
toolview_select_cb( GtkWidget *widget, Toolitem *toolitem )
{
	Workspace *ws = item_get_workspace( widget );

	if( ws && 
		toolitem->help )
		workspace_set_status( ws, "%s", toolitem->help );
}

/* Sub fn of below ... build a menu item for a TOOL_SYM. 
 */
static GtkWidget *
toolview_refresh_sub( Toolview *tview, 
	Toolitem *toolitem, Workspace *ws, GtkWidget *menu )
{
	Mainw *mainw = toolview_get_mainw( tview );
	GtkWidget *item;

	if( toolitem->is_separator ) 
		item = gtk_menu_item_new();
	else {
		item = gtk_image_menu_item_new_with_mnemonic( toolitem->label );

		gtk_object_set_data_by_id( GTK_OBJECT( item ),
			toolview_quark, tview );

		if( toolitem->icon )
			gtk_image_menu_item_set_image( 
				GTK_IMAGE_MENU_ITEM( item ),
				image_new_from_file( toolitem->icon ) );

		if( !toolitem->is_pullright ) 
			gtk_signal_connect( GTK_OBJECT( item ), "activate", 
				GTK_SIGNAL_FUNC( toolview_activate_cb ), 
				toolitem );

		if( toolitem->help && 
			!toolitem->is_pullright )
			set_tooltip( item, "%s", toolitem->help );

		gtk_signal_connect( GTK_OBJECT( item ), "select", 
			GTK_SIGNAL_FUNC( toolview_select_cb ), toolitem );

		/* Make a pullright and recurse if necessary
		 */
		if( toolitem->is_pullright ) {
			GtkWidget *submenu = gtk_menu_new();
			GSList *p;

			gtk_menu_set_accel_group( GTK_MENU( submenu ), 
				IWINDOW( mainw )->accel_group );
			gtk_menu_set_accel_path( GTK_MENU( submenu ), 
				toolitem->path );

			for( p = toolitem->children; p; p = p->next ) {
				Toolitem *child = p->data;

				toolview_refresh_sub( tview, 
					child, ws, submenu );
			}

			gtk_menu_item_set_submenu( GTK_MENU_ITEM( item ), 
				submenu );
		}
	}

	/* Is a top-level toolitem?
 	 */
	if( toolitem == toolitem->tool->toolitem ) 
		gtk_menu_shell_insert( GTK_MENU_SHELL( menu ), item, 
			ICONTAINER( toolitem->tool )->pos + 1 );
	else
		/* Submenus are always completely rebuilt, so we can just 
		 * append.
		 */
		gtk_menu_shell_append( GTK_MENU_SHELL( menu ), item );

	gtk_widget_show( item );

	return( item );
}

/* Our widget has been destroyed. NULL out or pointer to it, to stop us
 * destroying it again later.
 */
void
toolview_destroy_cb( GtkWidget *widget, Toolview *tview )
{
	g_assert( tview->item == widget );

	tview->item = NULL;
}

/* Update toolview display.
 */
static void
toolview_refresh( vObject *vobject )
{
	Toolview *tview = TOOLVIEW( vobject );
	Workspace *ws = toolview_get_workspace( tview );
	Tool *tool = TOOL( VOBJECT( tview )->iobject );
	Toolkitview *kview = tview->kview;

#ifdef DEBUG
	printf( "toolview_refresh: " );
	iobject_print( VOBJECT( tview )->iobject );
#endif /*DEBUG*/

	if( !toolview_quark ) 
		toolview_quark = 
			g_quark_from_static_string( "toolview_quark" );

	DESTROY_GTK( tview->item );

	if( tool->toolitem ) 
		tview->item = toolview_refresh_sub( tview, tool->toolitem, 
			ws, kview->menu );

	if( tview->item )
		gtk_signal_connect( GTK_OBJECT( tview->item ), "destroy",
			GTK_SIGNAL_FUNC( toolview_destroy_cb ), tview );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
toolview_link( View *view, Model *model, View *parent )
{
	Toolview *tview = TOOLVIEW( view );
	Toolkitview *kview = TOOLKITVIEW( parent );

	VIEW_CLASS( parent_class )->link( view, model, parent );

#ifdef DEBUG
	printf( "toolview_link: " );
	iobject_print( VOBJECT( tview )->iobject );
#endif /*DEBUG*/

	tview->kview = kview;
}

static void
toolview_class_init( ToolviewClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass*) class;
	GObjectClass *gobject_class = (GObjectClass*) class;
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = toolview_destroy;
	gobject_class->finalize = toolview_finalize;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = toolview_refresh;

	view_class->link = toolview_link;
}

static void
toolview_init( Toolview *toolview )
{
        toolview->item = NULL;
}

GtkType
toolview_get_type( void )
{
	static GtkType toolview_type = 0;

	if( !toolview_type ) {
		static const GtkTypeInfo toolview_info = {
			"Toolview",
			sizeof( Toolview ),
			sizeof( ToolviewClass ),
			(GtkClassInitFunc) toolview_class_init,
			(GtkObjectInitFunc) toolview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		toolview_type = gtk_type_unique( TYPE_VIEW, &toolview_info );
	}

	return( toolview_type );
}

View *
toolview_new( void )
{
	Toolview *tview = gtk_type_new( TYPE_TOOLVIEW );

#ifdef DEBUG
	printf( "toolview_new: %p\n", tview );
#endif /*DEBUG*/

	return( VIEW( tview ) );
}
