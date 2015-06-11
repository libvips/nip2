/* a view of a text thingy
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

G_DEFINE_TYPE( editview, Editview, TYPE_GRAPHICVIEW ); 

static void
editview_link( View *view, Model *model, View *parent )
{
	Editview *editview = EDITVIEW( view );

	VIEW_CLASS( editview_parent_class )->link( view, model, parent );

	if( GRAPHICVIEW( view )->sview )
		gtk_size_group_add_widget( GRAPHICVIEW( view )->sview->group,   
			editview->label );
}

static void 
editview_refresh( vObject *vobject )
{
	Editview *editview = EDITVIEW( vobject );

#ifdef DEBUG
	printf( "editview_refresh:\n" );
#endif /*DEBUG*/

	if( vobject->iobject->caption )
		set_glabel( editview->label, _( "%s:" ), 
			vobject->iobject->caption );

	VOBJECT_CLASS( editview_parent_class )->refresh( vobject );
}

static void
editview_class_init( EditviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = editview_refresh;

	view_class->link = editview_link;
}

/* Detect cancel in a text field.
 */
static gboolean
editview_event_cb( GtkWidget *widget, GdkEvent *ev, Editview *editview )
{
	gboolean handled;

	handled = FALSE;

        if( ev->key.keyval == GDK_Escape ) {
		handled = TRUE;

		/* Zap model value back into edit box.
		 */
		vobject_refresh_queue( VOBJECT( editview ) );
	}

        return( handled );
}

static void
editview_activate_cb( GtkWidget *wid, Editview *editview )
{
    	Expr *expr = HEAPMODEL( VOBJECT( editview )->iobject )->row->expr;

	/* If we've been changed, we'll be on the scannable list ... just
	 * recomp.
	 */
	symbol_recalculate_all();

	if( expr->err ) {
		expr_error_get( expr );
		iwindow_alert( wid, GTK_MESSAGE_ERROR );
	}
}

static void
editview_init( Editview *editview )
{
	GtkWidget *hbox;

	gtk_container_set_border_width( GTK_CONTAINER( editview ), 2 );

	hbox = gtk_hbox_new( FALSE, 12 );
        gtk_box_pack_start( GTK_BOX( editview ), hbox, TRUE, FALSE, 0 );

        editview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( editview->label ), 0, 0.5 );
	gtk_box_pack_start( GTK_BOX( hbox ), editview->label, FALSE, FALSE, 2 );

        editview->text = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( hbox ), editview->text, TRUE, TRUE, 0 );
        set_tooltip( editview->text, _( "Escape to cancel edit, "
                "press Return to accept edit and recalculate" ) );
        g_signal_connect_object( editview->text, "changed",
                G_CALLBACK( view_changed_cb ), GTK_OBJECT( editview ) );
        g_signal_connect( editview->text ), "activate",
                G_CALLBACK( editview_activate_cb ), editview );
        g_signal_connect( editview->text ), "event",
                G_CALLBACK( editview_event_cb ), editview );

        gtk_widget_show_all( hbox );
}

void
editview_set_entry( Editview *editview, const char *fmt, ... )
{
	va_list ap;
	char buf[1000];

	va_start( ap, fmt );
	(void) im_vsnprintf( buf, 1000, fmt, ap );
	va_end( ap );

	/* Make sure we don't trigger "changed" when we zap in the
	 * text.
	 */
	gtk_signal_handler_block_by_data( 
		GTK_OBJECT( editview->text ), editview );
	set_gentry( editview->text, "%s", buf );
	gtk_signal_handler_unblock_by_data( 
		GTK_OBJECT( editview->text ), editview );
}
