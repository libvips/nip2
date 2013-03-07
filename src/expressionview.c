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

static GraphicviewClass *parent_class = NULL;

/* Re-read the text in a tally entry. 
 */
static void *
expressionview_scan( View *view )
{
	Expressionview *expressionview = EXPRESSIONVIEW( view );
	Expression *expression = EXPRESSION( 
		VOBJECT( expressionview )->iobject );
	iText *itext = expression_get_itext( expression );

#ifdef DEBUG
{
	Row *row = HEAPMODEL( expression )->row;

	printf( "expressionview_scan: " );
	row_name_print( row );
	printf( "\n" );
}
#endif /*DEBUG*/

	if( itext &&
		formula_scan( expressionview->formula ) &&
		itext_set_formula( itext, expressionview->formula->expr ) ) {
		itext_set_edited( itext, TRUE );

		/* ... make sure MEMBER_VALUE gets marked dirty too.
		 */
		expr_dirty( HEAPMODEL( itext )->row->expr, 
			link_serial_new() );
	}

	return( VIEW_CLASS( parent_class )->scan( view ) );
}

void
expressionview_activate_cb( GtkWidget *wid, Expressionview *expressionview )
{
	Expression *expression = 
		EXPRESSION( VOBJECT( expressionview )->iobject );
	Row *row = HEAPMODEL( expression )->row;

	/* Reset edits on this row and all children.
	 */
	(void) icontainer_map_all( ICONTAINER( row ),
		(icontainer_map_fn) heapmodel_clear_edited, NULL );

	/* Make sure we scan this text, even if it's not been edited.
	 */
	view_scannable_register( VIEW( expressionview ) );

	filemodel_set_modified( FILEMODEL( row->ws ), TRUE );

	symbol_recalculate_all();
}

static void 
expressionview_refresh( vObject *vobject )
{
	Expressionview *expressionview = EXPRESSIONVIEW( vobject );
	Expression *expression = 
		EXPRESSION( VOBJECT( expressionview )->iobject );
	iText *itext = expression_get_itext( expression );
	Row *row = HEAPMODEL( expression )->row;

#ifdef DEBUG
	printf( "expressionview_refresh: " );
	row_name_print( row );
	printf( " (%p)\n", vobject );
#endif /*DEBUG*/

	formula_set_edit( expressionview->formula, 
		row->ws->mode == WORKSPACE_MODE_FORMULA );
	if( itext ) 
		formula_set_value_expr( expressionview->formula,
			vips_buf_all( &itext->value ), itext->formula );
	if( vobject->iobject->caption )
		formula_set_caption( expressionview->formula,
			vobject->iobject->caption );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void 
expressionview_set_edit( Expressionview *expressionview, gboolean edit )
{
	formula_set_edit( expressionview->formula, edit );

	if( edit ) 
		view_resettable_register( VIEW( expressionview ) );
}

static void
expressionview_link( View *view, Model *model, View *parent )
{
	Expressionview *expressionview = EXPRESSIONVIEW( view );
	Expression *expression = EXPRESSION( model );
	Row *row = HEAPMODEL( expression )->row;

#ifdef DEBUG
	printf( "expressionview_link: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	VIEW_CLASS( parent_class )->link( view, model, parent );

	if( GRAPHICVIEW( view )->sview )
		gtk_size_group_add_widget( GRAPHICVIEW( view )->sview->group,   
			expressionview->formula->left_label );

	/* Edit mode defaults to edit mode for workspace.
	 */
        expressionview_set_edit( expressionview, 
		row->ws->mode == WORKSPACE_MODE_FORMULA );
}

/* Reset edit mode ... go back to whatever is set for this ws.
 */
static void 
expressionview_reset( View *view )
{
	Expressionview *expressionview = EXPRESSIONVIEW( view );
	Expression *expression = 
		EXPRESSION( VOBJECT( expressionview )->iobject );
	Row *row = HEAPMODEL( expression )->row;

	expressionview_set_edit( expressionview, 
		row->ws->mode == WORKSPACE_MODE_FORMULA );
}

static void
expressionview_class_init( ExpressionviewClass *class )
{
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = expressionview_refresh;

	view_class->link = expressionview_link;
	view_class->reset = expressionview_reset;
	view_class->scan = expressionview_scan;
}

static void
expressionview_init( Expressionview *expressionview )
{
	expressionview->formula = formula_new();
        gtk_signal_connect_object( GTK_OBJECT( expressionview->formula ), 
		"changed", 
		GTK_SIGNAL_FUNC( view_changed_cb ), 
		GTK_OBJECT( expressionview ) );
        gtk_signal_connect( GTK_OBJECT( expressionview->formula ), "activate",
                GTK_SIGNAL_FUNC( expressionview_activate_cb ), expressionview );
        gtk_box_pack_start( GTK_BOX( expressionview ), 
		GTK_WIDGET( expressionview->formula ), TRUE, FALSE, 0 );
        gtk_widget_show( GTK_WIDGET( expressionview->formula ) );
}

GtkType
expressionview_get_type( void )
{
	static GtkType expressionview_type = 0;

	if( !expressionview_type ) {
		static const GtkTypeInfo expressionview_info = {
			"Expressionview",
			sizeof( Expressionview ),
			sizeof( ExpressionviewClass ),
			(GtkClassInitFunc) expressionview_class_init,
			(GtkObjectInitFunc) expressionview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		expressionview_type = gtk_type_unique( TYPE_GRAPHICVIEW, 
			&expressionview_info );
	}

	return( expressionview_type );
}

View *
expressionview_new( void )
{
	Expressionview *expressionview = gtk_type_new( TYPE_EXPRESSIONVIEW );

	return( VIEW( expressionview ) );
}
