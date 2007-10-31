/* display a minimal graphic for an object (just the caption)
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

/*
#define DEBUG
 */

#include "ip.h"

static GraphicviewClass *parent_class = NULL;

static void 
valueview_refresh( vObject *vobject )
{
	Valueview *valueview = VALUEVIEW( vobject );
	Model *model = MODEL( vobject->iobject );

#ifdef DEBUG
	printf( "valueview_refresh: " );
	row_name_print( HEAPMODEL( model )->row );
	printf( "\n" );
#endif /*DEBUG*/

	set_gcaption( valueview->label, "%s", NN( IOBJECT( model )->caption ) );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
valueview_link( View *view, Model *model, View *parent )
{
	Valueview *valueview = VALUEVIEW( view );
	Rowview *rview = ROWVIEW( parent->parent );

	VIEW_CLASS( parent_class )->link( view, model, parent );

	(void) rowview_menu_attach( rview, valueview->eb );
}

static void
valueview_class_init( ValueviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = valueview_refresh;

	view_class->link = valueview_link;
}

static gboolean
valueview_event_cb( GtkWidget *widget, GdkEvent *ev, 
	Valueview *valueview )
{
	Model *model = MODEL( VOBJECT( valueview )->iobject );
	Row *row = HEAPMODEL( model )->row;

	gboolean handled = FALSE;

        switch( ev->type ) {
        case GDK_BUTTON_PRESS:
		if( ev->button.button == 1 ) {
			row_select_modifier( row, ev->button.state );
			handled = TRUE;
		}
		break;

        case GDK_2BUTTON_PRESS:
		if( ev->button.button == 1 ) {
			model_edit( widget, MODEL( model ) );

			handled = TRUE;
		}
		break;

	default:
		break;
	}

	return( handled );
}

static void
valueview_init( Valueview *valueview )
{
#ifdef DEBUG
	printf( "valueview_init\n" );
#endif /*DEBUG*/

        valueview->eb = gtk_event_box_new();
        gtk_box_pack_start( GTK_BOX( valueview ), 
		valueview->eb, FALSE, FALSE, 0 );
	valueview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( valueview->label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( valueview->label ), 2, 0 );
        gtk_container_add( GTK_CONTAINER( valueview->eb ), valueview->label );
	gtk_widget_set_name( valueview->eb, "caption_widget" );
        gtk_signal_connect( GTK_OBJECT( valueview->eb ), "event",
                GTK_SIGNAL_FUNC( valueview_event_cb ), valueview );

        gtk_widget_show_all( GTK_WIDGET( valueview->eb ) );
}

GtkType
valueview_get_type( void )
{
	static GtkType valueview_type = 0;

	if( !valueview_type ) {
		static const GtkTypeInfo info = {
			"Valueview",
			sizeof( Valueview ),
			sizeof( ValueviewClass ),
			(GtkClassInitFunc) valueview_class_init,
			(GtkObjectInitFunc) valueview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		valueview_type = gtk_type_unique( TYPE_GRAPHICVIEW, &info );
	}

	return( valueview_type );
}

View *
valueview_new( void )
{
	Valueview *valueview = gtk_type_new( TYPE_VALUEVIEW );

	return( VIEW( valueview ) );
}
