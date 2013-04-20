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

static ViewClass *parent_class = NULL;

static void 
itextview_refresh( vObject *vobject )
{
	iTextview *itextview = ITEXTVIEW( vobject );
	iText *itext = ITEXT( VOBJECT( itextview )->iobject );
	Row *row = HEAPMODEL( itext )->row;

	const char *display;

#ifdef DEBUG
	printf( "itextview_refresh: " );
	row_name_print( row );
	printf( " (%p)\n", vobject );
#endif /*DEBUG*/

	/* Only reset edit mode if the text hasn't been changed. We
	 * don't want the user to lose work.
	 */
	if( !itextview->formula->changed ) 
		switch( row->ws->mode ) {
		case WORKSPACE_MODE_REGULAR:
			formula_set_edit( itextview->formula, FALSE );
			formula_set_sensitive( itextview->formula, TRUE );
			break;

		case WORKSPACE_MODE_FORMULA:
			formula_set_edit( itextview->formula, TRUE );
			formula_set_sensitive( itextview->formula, TRUE );
			break;

		case WORKSPACE_MODE_NOEDIT:
			formula_set_edit( itextview->formula, FALSE );
			formula_set_sensitive( itextview->formula, FALSE );
			break;

		default:
			g_assert_not_reached();
		}

	/* We display the formula if this is a class ... we assume the members
	 * and/or the graphic will represent the value.
	 */
	if( row->is_class )
		display = itext->formula;
	else
		display = vips_buf_all( &itext->value );

	if( itextview->formula && itext->value.base )
		formula_set_value_expr( itextview->formula,
			display, itext->formula );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
itextview_link( View *view, Model *model, View *parent )
{
	iTextview *itextview = ITEXTVIEW( view );
	iText *itext = ITEXT( model );
	Row *row = HEAPMODEL( itext )->row;

#ifdef DEBUG
	printf( "itextview_link: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	VIEW_CLASS( parent_class )->link( view, model, parent );

	/* Edit mode defaults to edit mode for workspace.
	 */
        formula_set_edit( itextview->formula, 
		row->ws->mode == WORKSPACE_MODE_FORMULA );
}

/* Reset edit mode ... go back to whatever is set for this ws.
 */
static void 
itextview_reset( View *view )
{
	iTextview *itextview = ITEXTVIEW( view );
	iText *itext = ITEXT( VOBJECT( itextview )->iobject );
	Row *row = HEAPMODEL( itext )->row;

#ifdef DEBUG
	printf( "itextview_reset: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	formula_set_edit( ITEXTVIEW( view )->formula, 
		row->ws->mode == WORKSPACE_MODE_FORMULA );

	VIEW_CLASS( parent_class )->reset( view );
}

/* Re-read the text in a tally entry. 
 */
static void *
itextview_scan( View *view )
{
	iTextview *itextview = ITEXTVIEW( view );
	iText *itext = ITEXT( VOBJECT( itextview )->iobject );

#ifdef DEBUG
	Row *row = HEAPMODEL( itext )->row;

	printf( "itextview_scan: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	if( formula_scan( itextview->formula ) &&
		itext_set_formula( itext, itextview->formula->expr ) )
		itext_set_edited( itext, TRUE );

	return( VIEW_CLASS( parent_class )->scan( view ) );
}

static void
itextview_class_init( iTextviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = itextview_refresh;

	view_class->link = itextview_link;
	view_class->reset = itextview_reset;
	view_class->scan = itextview_scan;
}

void
itextview_edit_cb( Formula *formula, iTextview *itextview )
{
	view_resettable_register( VIEW( itextview ) );
}

void
itextview_activate_cb( Formula *formula, iTextview *itextview )
{
	iText *itext = ITEXT( VOBJECT( itextview )->iobject );
	Row *row = HEAPMODEL( itext )->row;

	/* Reset edits on this row and all children. Our (potentially) next
	 * text will invlidate all of them.
	 */
	(void) icontainer_map_all( ICONTAINER( row ),
		(icontainer_map_fn) heapmodel_clear_edited, NULL );

	/* Make sure we scan this text, even if it's not been edited.
	 */
	view_scannable_register( VIEW( itextview ) );

	workspace_set_modified( row->ws, TRUE );

	symbol_recalculate_all();
}

static void
itextview_enter_cb( Formula *formula, iTextview *itextview )
{
	iText *itext = ITEXT( VOBJECT( itextview )->iobject );
	Row *row = HEAPMODEL( itext )->row;

	row_set_status( row );
	row_show_dependents( row );
}

static void
itextview_leave_cb( Formula *formula, iTextview *itextview )
{
	iText *itext = ITEXT( VOBJECT( itextview )->iobject );
	Row *row = HEAPMODEL( itext )->row;

	row_hide_dependents( row );
}

static void
itextview_init( iTextview *itextview )
{
	itextview->formula = formula_new();

        gtk_signal_connect( GTK_OBJECT( itextview->formula ), "edit", 
		GTK_SIGNAL_FUNC( itextview_edit_cb ), itextview );
        gtk_signal_connect_object( GTK_OBJECT( itextview->formula ), "changed", 
		GTK_SIGNAL_FUNC( view_changed_cb ), itextview );
        gtk_signal_connect( GTK_OBJECT( itextview->formula ), "activate",
                GTK_SIGNAL_FUNC( itextview_activate_cb ), itextview );
        gtk_signal_connect( GTK_OBJECT( itextview->formula ), "enter", 
		GTK_SIGNAL_FUNC( itextview_enter_cb ), itextview );
        gtk_signal_connect( GTK_OBJECT( itextview->formula ), "leave", 
		GTK_SIGNAL_FUNC( itextview_leave_cb ), itextview );

        gtk_box_pack_start( GTK_BOX( itextview ), 
		GTK_WIDGET( itextview->formula ), TRUE, FALSE, 0 );
        gtk_widget_show( GTK_WIDGET( itextview->formula ) );
}

GtkType
itextview_get_type( void )
{
	static GtkType itextview_type = 0;

	if( !itextview_type ) {
		static const GtkTypeInfo itextview_info = {
			"iTextview",
			sizeof( iTextview ),
			sizeof( iTextviewClass ),
			(GtkClassInitFunc) itextview_class_init,
			(GtkObjectInitFunc) itextview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		itextview_type = gtk_type_unique( TYPE_VIEW, &itextview_info );
	}

	return( itextview_type );
}

View *
itextview_new( void )
{
	iTextview *itextview = gtk_type_new( TYPE_ITEXTVIEW );

	return( VIEW( itextview ) );
}
