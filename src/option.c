/* an input option ... put/get methods
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

static ClassmodelClass *parent_class = NULL;

static void
option_finalize( GObject *gobject )
{
	Option *option;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_OPTION( gobject ) );

	option = OPTION( gobject );

	/* My instance finalize stuff.
	 */
	IM_FREEF( slist_free_all, option->labels );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static View *
option_view_new( Model *model, View *parent )
{
	return( optionview_new() );
}

/* Widgets for option edit.
 */
typedef struct _OptionEdit {
	iDialog *idlg;

	Option *option;

	GtkWidget *caption;
	GtkWidget *labels;		/* List for current options */
	GtkWidget *value;		/* Currently selected */

	GtkWidget *swin;		/* ScrolledWindow for list */
} OptionEdit;

/* Sub fn of below.
 */
static void
option_add_item( GtkWidget *item, GSList **labels )
{
	if( GTK_IS_LABEL( GTK_BIN( item )->child ) )
		*labels = g_slist_append( *labels, 
			g_strdup( orderitem_get_label( item ) ) );
}

/* Done button hit.
 */
/*ARGSUSED*/
static void
option_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	OptionEdit *eds = (OptionEdit *) client;
	Classmodel *classmodel = CLASSMODEL( eds->option );

	char *caption;
	GSList *labels;
	int value;

	caption = gtk_editable_get_chars( 
		GTK_EDITABLE( eds->caption ), 0, -1 );

	/* Extract the list of items.
	 */
	orderlist_scan( ORDERLIST( eds->labels ) );
	labels = NULL;
	gtk_container_foreach( GTK_CONTAINER( eds->labels ), 
		(GtkCallback) option_add_item, &labels );
	if( g_slist_length( labels ) == 0 ) {
		error_top( _( "No options." ) );
		error_sub( _( "You need at least one option in your "
			"option list" ) );
		IM_FREEF( slist_free_all, labels );
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	/* Current value.
	 */
	if( !get_geditable_int( eds->value, &value ) ) {
		IM_FREEF( slist_free_all, labels );
		IM_FREE( caption );
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	/* Update model.
	 */
	IM_FREEF( slist_free_all, eds->option->labels );
	eds->option->labels = labels;
	eds->option->value = value;
	iobject_set( IOBJECT( eds->option ), NULL, caption );
	IM_FREE( caption );

	/* Rebuild object.
	 */
	classmodel_update( classmodel );
	symbol_recalculate_all();

	nfn( sys, IWINDOW_YES );
}

/* Build the insides of option edit.
 */
static void
option_buildedit( iDialog *idlg, GtkWidget *work, OptionEdit *eds )
{
	Option *option = eds->option;
	GtkWidget *vb;
	GSList *p;
	int n;

	eds->caption = build_glabeltext3( work, _( "Caption" ) );
	idialog_init_entry( idlg, eds->caption, _( "Set option caption here" ),
		"%s", IOBJECT( option )->caption );

	eds->swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_container_set_border_width( GTK_CONTAINER( eds->swin ), 2 );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( eds->swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request( GTK_WIDGET( eds->swin ), 180, 180 );

	eds->labels = orderlist_new();
	gtk_list_set_selection_mode( GTK_LIST( eds->labels ),
		GTK_SELECTION_EXTENDED );
	gtk_scrolled_window_add_with_viewport( 
		GTK_SCROLLED_WINDOW( eds->swin ), eds->labels );
	gtk_container_set_focus_vadjustment( GTK_CONTAINER( eds->labels ),
		gtk_scrolled_window_get_vadjustment( 
			GTK_SCROLLED_WINDOW( eds->swin ) ) );
	gtk_container_set_focus_hadjustment( GTK_CONTAINER( eds->labels ),
		gtk_scrolled_window_get_hadjustment(
			GTK_SCROLLED_WINDOW( eds->swin ) ) );

	vb = build_glabelframe2( eds->swin, _( "Options" ) );
	gtk_box_pack_start( GTK_BOX( work ), vb, TRUE, TRUE, 0 );

	for( n = 0, p = option->labels; p; p = p->next, n++ ) {
		const char *text = (const char *) p->data;

		orderlist_add_label( ORDERLIST( eds->labels ), text, -1 );
	}

        eds->value = build_glabeltext3( work, _( "Value" ) );
	idialog_init_entry( idlg, eds->value,
		_( "Set option default value here" ), "%d", option->value );

        gtk_widget_show_all( work );
}

/* Pop up a option edit box.
 */
static void 
option_edit( GtkWidget *parent, Model *model )
{
	Option *option = OPTION( model );
	OptionEdit *eds = INEW( NULL, OptionEdit );
	GtkWidget *idlg;

	eds->option = option;

	idlg = idialog_new();
	iwindow_set_title( IWINDOW( idlg ), _( "Edit Option" ) );
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) option_buildedit, eds, NULL, NULL );
	idialog_set_callbacks( IDIALOG( idlg ), 
		iwindow_true_cb, NULL, idialog_free_client, eds );
	idialog_add_ok( IDIALOG( idlg ), option_done_cb, _( "Set Option" ) );
	iwindow_set_parent( IWINDOW( idlg ), parent );
	idialog_set_iobject( IDIALOG( idlg ), IOBJECT( model ) );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );
}

/* Members of option we automate.
 */
static ClassmodelMember option_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_CAPTION, "caption", N_( "Caption" ),
		G_STRUCT_OFFSET( iObject, caption ) },
	{ CLASSMODEL_MEMBER_STRING_LIST, NULL, 0,
		MEMBER_LABELS, "labels", N_( "Labels" ),
		G_STRUCT_OFFSET( Option, labels ) },
	{ CLASSMODEL_MEMBER_INT, NULL, 0,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Option, value ) }
};

static void
option_class_init( OptionClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->finalize = option_finalize;

	model_class->view_new = option_view_new;
	model_class->edit = option_edit;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = option_members;
	classmodel_class->n_members = IM_NUMBER( option_members );
}

static void
option_init( Option *option )
{
        option->labels = NULL;
	option->value = 0;

	iobject_set( IOBJECT( option ), CLASS_OPTION, NULL );
}

GType
option_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( OptionClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) option_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Option ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) option_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Option", &info, 0 );
	}

	return( type );
}
