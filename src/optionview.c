/* run the display for a option in a workspace 
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

/* Copy a gslist of strings.
 */
static GSList *
lstring_copy( GSList *lstring )
{
	GSList *new;
	GSList *p;

	new = NULL;
	for( p = lstring; p; p = p->next )
		new = g_slist_prepend( new, 
			g_strdup( (const char *) p->data ) );

	new = g_slist_reverse( new );

	return( new );
}

/* Are two lstrings equal?
 */
static gboolean
lstring_equal( GSList *a, GSList *b )
{
	for( ; a && b; a = a->next, b = b->next )
		if( strcmp( (const char *) a->data, 
			(const char *) b->data ) != 0 )
			return( FALSE );

	if( a || b )
		return( FALSE );

	return( TRUE );
}

static void
optionview_destroy( GtkObject *object )
{
	Optionview *optionview;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_OPTIONVIEW( object ) );

	optionview = OPTIONVIEW( object );

	/* My instance destroy stuff.
	 */
	IM_FREEF( slist_free_all, optionview->labels );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
optionview_link( View *view, Model *model, View *parent )
{
	Optionview *optionview = OPTIONVIEW( view );

	VIEW_CLASS( parent_class )->link( view, model, parent );

	if( GRAPHICVIEW( view )->sview )
		gtk_size_group_add_widget( GRAPHICVIEW( view )->sview->group,   
			optionview->label );
}

/* Change to a optionview widget ... update the model. 
 */
static void
optionview_change_cb( GtkWidget *wid, Optionview *optionview )
{
	Option *option = OPTION( VOBJECT( optionview )->iobject );
	Classmodel *classmodel = CLASSMODEL( option );

	const int nvalue = gtk_combo_box_get_active( 
		GTK_COMBO_BOX( optionview->options ) );

	if( option->value != nvalue ) {
		option->value = nvalue;

		classmodel_update( classmodel );
		symbol_recalculate_all();
	}
}

static void 
optionview_refresh( vObject *vobject )
{
	Optionview *optionview = OPTIONVIEW( vobject );
	Option *option = OPTION( VOBJECT( optionview )->iobject );

	GSList *p;
	int i;

#ifdef DEBUG
	printf( "optionview_refresh: " );
	row_name_print( HEAPMODEL( option )->row );
	printf( "\n" );
#endif /*DEBUG*/

	/* Only rebuild the menu if there's been a change.
	 */
	if( !lstring_equal( optionview->labels, option->labels ) ) {
		/* If the menu is currently up, we can get strange things 
		 * happening if we destroy it.
		 */
		if( optionview->options )
			gtk_combo_box_popdown( 
				GTK_COMBO_BOX( optionview->options ) );
		IM_FREEF( gtk_widget_destroy, optionview->options );

		optionview->options = gtk_combo_box_new_text();
		for( p = option->labels, i = 0; p; p = p->next, i++ ) 
			gtk_combo_box_append_text( 
				GTK_COMBO_BOX( optionview->options ),
				(const char *) p->data );
		gtk_box_pack_start( GTK_BOX( optionview->hbox ), 
			optionview->options, TRUE, TRUE, 0 );

		gtk_signal_connect( GTK_OBJECT( optionview->options ), 
			"changed", 
			GTK_SIGNAL_FUNC( optionview_change_cb ), optionview );
		gtk_widget_show( optionview->options );

		IM_FREEF( slist_free_all, optionview->labels );
		optionview->labels = lstring_copy( option->labels );
	}

	if( optionview->options ) {
		gtk_signal_handler_block_by_data( 
			GTK_OBJECT( optionview->options ), optionview );
		gtk_combo_box_set_active( GTK_COMBO_BOX( optionview->options ), 
			option->value );
		gtk_signal_handler_unblock_by_data( 
			GTK_OBJECT( optionview->options ), optionview );
	}

	set_glabel( optionview->label, _( "%s:" ), IOBJECT( option )->caption );

	VOBJECT_CLASS( parent_class )->refresh( vobject );
}

static void
optionview_class_init( OptionviewClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	vObjectClass *vobject_class = (vObjectClass *) class;
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = optionview_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */
	vobject_class->refresh = optionview_refresh;

	view_class->link = optionview_link;
}

static void
optionview_init( Optionview *optionview )
{
        optionview->hbox = gtk_hbox_new( FALSE, 12 );
        gtk_box_pack_start( GTK_BOX( optionview ), 
		optionview->hbox, TRUE, FALSE, 0 );

        optionview->label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( optionview->label ), 0, 0.5 );
	gtk_box_pack_start( GTK_BOX( optionview->hbox ), 
		optionview->label, FALSE, FALSE, 2 );

	optionview->options = NULL;

	optionview->labels = NULL;

        gtk_widget_show_all( optionview->hbox );
}

GtkType
optionview_get_type( void )
{
	static GtkType optionview_type = 0;

	if( !optionview_type ) {
		static const GtkTypeInfo sinfo = {
			"Optionview",
			sizeof( Optionview ),
			sizeof( OptionviewClass ),
			(GtkClassInitFunc) optionview_class_init,
			(GtkObjectInitFunc) optionview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		optionview_type = gtk_type_unique( TYPE_GRAPHICVIEW, &sinfo );
	}

	return( optionview_type );
}

View *
optionview_new( void )
{
	Optionview *optionview = gtk_type_new( TYPE_OPTIONVIEW );

	return( VIEW( optionview ) );
}
