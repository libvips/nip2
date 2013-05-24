/* display a caption/value label pair, on a click display the formula in an
 * entry widget
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

/* Our signals. 
 */
enum {
	EDIT,		
	CHANGED,		
	ACTIVATE,	
	ENTER,	
	LEAVE,	
	LAST_SIGNAL
};

static GtkEventBoxClass *parent_class = NULL;

static guint formula_signals[LAST_SIGNAL] = { 0 };

/* Formula needing a refresh.
 */
static GSList *formula_refresh_all = NULL;

/* The idle we add if there are any formula needing a refresh.
 */
static gint formula_refresh_idle = 0;

/* Unqueue a refresh.
 */
static void
formula_refresh_unqueue( Formula *formula )
{
	if( formula->refresh_queued ) {
		formula_refresh_all = 
			g_slist_remove( formula_refresh_all, formula );
		formula->refresh_queued = FALSE;
		
		if( !formula_refresh_all )
			IM_FREEF( g_source_remove, formula_refresh_idle );
	}
}

/* Detect cancel in a text field.
 */
static gboolean
formula_key_press_event_cb( GtkWidget *widget, GdkEventKey *ev, 
	Formula *formula )
{
	gboolean handled;

	handled = FALSE;

        if( ev->keyval == GDK_Escape ) {
		set_gentry( formula->entry, "%s", formula->expr );

		/*

			FIXME ... really we want to go back to the edit mode
			set by our environment (eg. if we're in a show_formula
			workspace, should stay in show formula).

		 */
		formula_set_edit( formula, FALSE );

		handled = TRUE;
	}

        return( handled );
}

/* Activated!
 */
static void
formula_activate( Formula *formula )
{
	g_signal_emit( G_OBJECT( formula ), formula_signals[ACTIVATE], 0 );
}

static void
formula_activate_cb( GtkWidget *wid, Formula *formula )
{
	formula_activate( formula );
}

/* A char has changed in the entry (we will need scanning on activate).
 */
static void
formula_changed( Formula *formula )
{
	g_signal_emit( G_OBJECT( formula ), formula_signals[CHANGED], 0 );
}

/* Add an edit box.
 */
static void
formula_add_edit( Formula *formula )
{
        if( formula->entry_frame )
                return;

	/* We need to use an alignment since if the left label is hidden we'll
 	 * have nothing to hold us to the right height.
	 */
        formula->entry_frame = gtk_alignment_new( 0.5, 0.5, 1, 1 );
	gtk_alignment_set_padding( GTK_ALIGNMENT( formula->entry_frame ),
		3, 3, 2, 2 );
	gtk_box_pack_start( GTK_BOX( formula->hbox ), 
		formula->entry_frame, TRUE, TRUE, 0 );

        formula->entry = gtk_entry_new();
        set_tooltip( formula->entry, _( "Press Escape to cancel edit, "
                "press Return to accept edit and recalculate" ) );
        gtk_signal_connect( GTK_OBJECT( formula->entry ), "key_press_event", 
		GTK_SIGNAL_FUNC( formula_key_press_event_cb ), 
		GTK_OBJECT( formula ) );
        gtk_signal_connect_object( GTK_OBJECT( formula->entry ), "changed", 
		GTK_SIGNAL_FUNC( formula_changed ), GTK_OBJECT( formula ) );
        gtk_signal_connect( GTK_OBJECT( formula->entry ), "activate",
                GTK_SIGNAL_FUNC( formula_activate_cb ), formula );
	gtk_container_add( GTK_CONTAINER( formula->entry_frame ), 
		formula->entry );
	gtk_widget_show( formula->entry );

	/* Tell everyone we are in edit mode ... used to add to resettable,
	 * for example.
	 */
	g_signal_emit( G_OBJECT( formula ), formula_signals[EDIT], 0 );
}

static void 
formula_refresh( Formula *formula )
{
#ifdef DEBUG
	printf( "formula_refresh\n" );
#endif /*DEBUG*/

	/* Set edit mode.
	 */
	if( formula->edit ) {
		formula_add_edit( formula );
                gtk_widget_show( formula->entry_frame );
                gtk_widget_hide( formula->right_label );
		formula->changed = FALSE;
	}
	else {
                gtk_widget_show( formula->right_label );
                IM_FREEF( gtk_widget_destroy, formula->entry );
                IM_FREEF( gtk_widget_destroy, formula->entry_frame );
	}

	/* Don't update the formula display if the user has edited the text ...
	 * we shouldn't destroy their work.
	 */
	if( formula->entry && formula->expr && !formula->changed ) {
		/* Make sure we don't trigger "changed" when we zap in new
		 * text.
		 */
		gtk_signal_handler_block_by_data( 
			GTK_OBJECT( formula->entry ), formula );
		set_gentry( formula->entry, "%s", formula->expr );
		gtk_signal_handler_unblock_by_data( 
			GTK_OBJECT( formula->entry ), formula );
	}

	if( formula->caption ) {
		set_glabel( formula->left_label, _( "%s:" ), formula->caption );
                gtk_widget_show( formula->left_label );
	}
	else
                gtk_widget_hide( formula->left_label );
	if( formula->value ) 
		/* Just display the first line of the formula ... it can be
		 * mutiline for class members, for example.
		 */
		set_glabel1( formula->right_label, "%s", formula->value );

	if( formula->edit && formula->needs_focus ) {
		if( formula->expr )
			gtk_editable_select_region( 
				GTK_EDITABLE( formula->entry ), 0, -1 );
		gtk_widget_grab_focus( formula->entry );
		formula->needs_focus = FALSE;
	}
}

static gboolean
formula_refresh_idle_cb( void )
{
	formula_refresh_idle = 0;

	while( formula_refresh_all ) {
		Formula *formula = FORMULA( formula_refresh_all->data );

		formula_refresh_unqueue( formula );
		formula_refresh( formula );
	}

	return( FALSE );
}

static void
formula_refresh_queue( Formula *formula )
{
	if( !formula->refresh_queued ) {
		formula_refresh_all = 
			g_slist_prepend( formula_refresh_all, formula );
		formula->refresh_queued = TRUE;
		
		if( !formula_refresh_idle )
			formula_refresh_idle = g_idle_add( 
				(GSourceFunc) formula_refresh_idle_cb, NULL );
	}
}

static void
formula_destroy( GtkObject *object )
{
	Formula *formula;

#ifdef DEBUG
	printf( "formula_destroy\n" );
#endif /*DEBUG*/

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_FORMULA( object ) );

	/* My instance destroy stuff.
	 */
	formula = FORMULA( object );

	formula_refresh_unqueue( formula );
	IM_FREE( formula->caption );
	IM_FREE( formula->value );
	IM_FREE( formula->expr );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

/* Change edit mode.
 */
void 
formula_set_edit( Formula *formula, gboolean edit )
{
#ifdef DEBUG
	printf( "formula_set_edit: %d\n", edit );
#endif /*DEBUG*/

	if( formula->edit != edit ) {
		formula->edit = edit;
		formula_refresh_queue( formula );
	}

	/* Can't have been edited yet, whichever way we're turning edit.
	 */
	formula->changed = FALSE;
}

/* Grab focus on next refresh.
 */
void 
formula_set_needs_focus( Formula *formula, gboolean needs_focus )
{
#ifdef DEBUG
	printf( "formula_set_needs_focus: %d\n", needs_focus );
#endif /*DEBUG*/

	if( formula->needs_focus != needs_focus ) {
		formula->needs_focus = needs_focus;
		formula_refresh_queue( formula );
	}
}

/* Change sensitive mode.
 */
void 
formula_set_sensitive( Formula *formula, gboolean sensitive )
{
#ifdef DEBUG
	printf( "formula_set_sensitive: %d\n", sensitive );
#endif /*DEBUG*/

	if( formula->sensitive != sensitive ) {
		formula->sensitive = sensitive;

		if( !formula->sensitive )
			formula_set_edit( formula, FALSE );

		formula_refresh_queue( formula );
	}
}

/* Re-read the text. TRUE if we saw a change.
 */
gboolean
formula_scan( Formula *formula )
{
	gboolean changed;

#ifdef DEBUG
	printf( "formula_scan\n" );
#endif /*DEBUG*/

	changed = FALSE;

	/* Should be in edit mode.
	 */
	if( formula->edit && 
		formula->entry && 
		GTK_WIDGET_VISIBLE( formula->entry ) ) {
		const char *expr;

		/* There should be some edited text.
		 */
		expr = gtk_entry_get_text( GTK_ENTRY( formula->entry ) );
		if( expr && 
			strspn( expr, WHITESPACE ) != strlen( expr ) ) {
			IM_SETSTR( formula->expr, expr );
			changed = TRUE;
		}

                formula_set_edit( formula, FALSE );
	}

	return( changed );
}

static gboolean
formula_enter_notify_event( GtkWidget *widget, GdkEventCrossing *event )
{
	GtkWidget *event_widget;

	event_widget = gtk_get_event_widget( (GdkEvent *) event );

	if( event_widget == widget && event->detail != GDK_NOTIFY_INFERIOR ) {
		gtk_widget_set_state( widget, GTK_STATE_PRELIGHT );

		/* Tell people about our highlight change ... used to (eg.) set 
		 * flash help.
		 */
		g_signal_emit( G_OBJECT( widget ), formula_signals[ENTER], 0 );
	}

	return( FALSE );
}

static gboolean
formula_leave_notify_event( GtkWidget *widget, GdkEventCrossing *event )
{
	GtkWidget *event_widget;

	event_widget = gtk_get_event_widget( (GdkEvent *) event );

	if( event_widget == widget && event->detail != GDK_NOTIFY_INFERIOR ) {
		gtk_widget_set_state( widget, GTK_STATE_NORMAL );

		/* Tell people about our highlight change ... used to (eg.) set 
		 * flash help.
		 */
		g_signal_emit( G_OBJECT( widget ), formula_signals[LEAVE], 0 );
	}

	return( FALSE );
}

/* Event in us somewhere.
 */
static gboolean
formula_button_press_event( GtkWidget *widget, GdkEventButton *event )
{
	gboolean handled = FALSE;

	if( event->type == GDK_BUTTON_PRESS ) {
		Formula *formula = FORMULA( widget );

		if( event->button == 1 && formula->sensitive ) {
			if( !formula->edit ) {
				formula_set_edit( formula, TRUE );
				formula_set_needs_focus( formula, TRUE );
			}

			handled = TRUE;
		}
	}

	return( handled );
}       

static void
formula_real_changed( Formula *formula )
{
#ifdef DEBUG
	printf( "formula_real_changed\n" );
#endif /*DEBUG*/

	formula->changed = TRUE;
}

static void
formula_class_init( FormulaClass *class )
{
	GtkObjectClass *gobject_class = (GtkObjectClass *) class;
	GtkWidgetClass *widget_class = (GtkWidgetClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->destroy = formula_destroy;

	widget_class->enter_notify_event = formula_enter_notify_event;
	widget_class->leave_notify_event = formula_leave_notify_event;
	widget_class->button_press_event = formula_button_press_event;

	/* Create signals.
	 */
	formula_signals[EDIT] = g_signal_new( "edit",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( FormulaClass, changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	formula_signals[CHANGED] = g_signal_new( "changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( FormulaClass, changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	formula_signals[ACTIVATE] = g_signal_new( "activate",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( FormulaClass, activate ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	formula_signals[ENTER] = g_signal_new( "enter",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( FormulaClass, enter ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	formula_signals[LEAVE] = g_signal_new( "leave",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( FormulaClass, leave ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );

	/* Init methods.
	 */
	class->changed = formula_real_changed;
}

static void
formula_init( Formula *formula )
{
	/* How annoying! To avoid vertical resizes on edit/view toggles we
	 * need to add differing amounts of padding to the label depending on
	 * the theme.

	  	FIXME ... get this from the style somehow

	 */
#ifdef OS_WIN32
	/* with either wimp theme or gtk default.
	 */
	const int vpadding = 7;
#else /*!OS_WIN32*/
	/* clearlooks
	 */
	const int vpadding = 8;
#endif /*OS_WIN32*/

	formula->caption = NULL;
	formula->value = NULL;
	formula->expr = NULL;
	formula->edit = FALSE;
	formula->sensitive = TRUE;
	formula->changed = FALSE;
	formula->refresh_queued = FALSE;
	formula->needs_focus = FALSE;

	formula->entry_frame = NULL;

	gtk_widget_add_events( GTK_WIDGET( formula ), 
		GDK_POINTER_MOTION_HINT_MASK ); 

	formula->hbox = gtk_hbox_new( FALSE, 12 );
	gtk_container_add( GTK_CONTAINER( formula ), formula->hbox );
        gtk_widget_show( formula->hbox );

        formula->left_label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( formula->left_label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( formula->left_label ), 2, vpadding );
	gtk_box_pack_start( GTK_BOX( formula->hbox ), 
		formula->left_label, FALSE, FALSE, 2 );
        gtk_widget_show( formula->left_label );

        formula->right_label = gtk_label_new( "" );
        gtk_misc_set_alignment( GTK_MISC( formula->right_label ), 0, 0.5 );
        gtk_misc_set_padding( GTK_MISC( formula->right_label ), 7, vpadding );
	gtk_box_pack_start( GTK_BOX( formula->hbox ), 
		formula->right_label, TRUE, TRUE, 0 );
        gtk_widget_show( formula->right_label );
}

GtkType
formula_get_type( void )
{
	static GtkType formula_type = 0;

	if( !formula_type ) {
		static const GtkTypeInfo formula_info = {
			"Formula",
			sizeof( Formula ),
			sizeof( FormulaClass ),
			(GtkClassInitFunc) formula_class_init,
			(GtkObjectInitFunc) formula_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		formula_type = gtk_type_unique( GTK_TYPE_EVENT_BOX, 
			&formula_info );
	}

	return( formula_type );
}

Formula *
formula_new( void )
{
	Formula *formula = gtk_type_new( TYPE_FORMULA );

	return( formula );
}

void
formula_set_caption( Formula *formula, const char *caption )
{
	if( !caption && formula->caption ) {
		IM_FREE( formula->caption );
		formula_refresh_queue( formula );
	}
	else if( caption && (!formula->caption || 
		strcmp( caption, formula->caption ) != 0) ) {
		IM_SETSTR( formula->caption, caption );
		formula_refresh_queue( formula );
	}
}

void
formula_set_value_expr( Formula *formula, const char *value, const char *expr )
{
#ifdef DEBUG
	printf( "formula_set_value_expr: value=\"%s\", expr=\"%s\"\n",
		value, expr );
#endif /*DEBUG*/

	if( value && (!formula->value || 
		strcmp( value, formula->value ) != 0) ) {
		IM_SETSTR( formula->value, value );
		formula_refresh_queue( formula );
	}

	if( expr && (!formula->expr || 
		strcmp( expr, formula->expr ) != 0) ) {
		IM_SETSTR( formula->expr, expr );
		formula_refresh_queue( formula );
	}
}
