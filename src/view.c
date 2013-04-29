/* abstract base class for items which can form a row in a tally
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
#define DEBUG_VIEWCHILD
 */

/* Time each refresh
#define DEBUG_TIME
 */

#include "ip.h"

static vObjectClass *parent_class = NULL;

static GSList *view_scannable = NULL;

static GSList *view_resettable = NULL;

void
view_scannable_register( View *view )
{
	/* Must have a scan method.
	 */
	g_assert( VIEW_GET_CLASS( view )->scan );

	if( !view->scannable ) {
		view_scannable = g_slist_prepend( view_scannable, view );
		view->scannable = TRUE;
	}
}

void
view_scannable_unregister( View *view )
{
	if( view->scannable ) {
		view_scannable = g_slist_remove( view_scannable, view );
		view->scannable = FALSE;
	}
}

gboolean
view_scan_all( void )
{
	if( slist_map( view_scannable, (SListMapFn) view_scan, NULL ) )
		return( FALSE );

	view_reset_all();

	return( TRUE );
}

void
view_resettable_register( View *view )
{
	/* Must have a reset method.
	 */
	g_assert( VIEW_GET_CLASS( view )->reset );

	if( !view->resettable ) {
		view_resettable = g_slist_prepend( view_resettable, view );
		view->resettable = TRUE;
	}
}

void
view_resettable_unregister( View *view )
{
	if( view->resettable ) {
		view_resettable = g_slist_remove( view_resettable, view );
		view->resettable = FALSE;
	}
}

void
view_reset_all( void )
{
	(void) slist_map( view_resettable, (SListMapFn) view_reset, NULL );
}

/* Should a viewchild be displayed? If model->display is true, also give the
 * enclosing view a chance to filter.
 */
static gboolean
view_viewchild_display( ViewChild *viewchild )
{
	View *parent_view = viewchild->parent_view;
	Model *child_model = viewchild->child_model;
	ViewClass *parent_view_class = VIEW_GET_CLASS( parent_view );

	if( child_model->display && parent_view_class->display )
		return( parent_view_class->display( parent_view, 
			child_model ) );

	return( child_model->display );
}

/* One of the children of the model we watch has changed ... create or destroy
 * the child view as required.
 */
static void
view_viewchild_changed( Model *model, ViewChild *viewchild )
{
	gboolean display = view_viewchild_display( viewchild );
	View *child = viewchild->child_view;

	if( !display && child ) {
#ifdef DEBUG_VIEWCHILD
		printf( "view_viewchild_changed: %s \"%s\", removing view\n", 
			G_OBJECT_TYPE_NAME( model ), 
			NN( IOBJECT( model )->name ) );

		printf( "view_viewchild_changed: %s\n", 
			G_OBJECT_TYPE_NAME( child ) ); 
#endif /*DEBUG_VIEWCHILD*/

		DESTROY_GTK( child );
	}
	else if( display && !child ) {
#ifdef DEBUG_VIEWCHILD
		printf( "view_viewchild_changed: %s \"%s\", adding view\n", 
			G_OBJECT_TYPE_NAME( model ), 
			NN( IOBJECT( model )->name ) );
#endif /*DEBUG_VIEWCHILD*/

		model_view_new( model, viewchild->parent_view );
	}
}

static ViewChild *
view_viewchild_new( View *parent_view, Model *child_model )
{
	ViewChild *viewchild;

#ifdef DEBUG_VIEWCHILD
	printf( "view_viewchild_new: view \"%s\" watching %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( parent_view ), 
		G_OBJECT_TYPE_NAME( child_model ), 
		NN( IOBJECT( child_model )->name ) );
#endif /*DEBUG_VIEWCHILD*/

	if( !(viewchild = INEW( NULL, ViewChild )) )
		return( NULL );

	viewchild->parent_view = parent_view;
	viewchild->child_model = child_model;
	viewchild->child_model_changed_sid = 
		g_signal_connect( child_model, "changed", 
			G_CALLBACK( view_viewchild_changed ), viewchild );
	viewchild->child_view = NULL;

	parent_view->managed = 
		g_slist_append( parent_view->managed, viewchild );

	return( viewchild );
}

static void *
view_viewchild_destroy( ViewChild *viewchild )
{
	View *parent_view = viewchild->parent_view;
	View *child_view = viewchild->child_view;

#ifdef DEBUG_VIEWCHILD
	printf( "view_viewchild_destroy: view %s watching model %s\n",
		G_OBJECT_TYPE_NAME( viewchild->parent_view ), 
		G_OBJECT_TYPE_NAME( viewchild->child_model ) );
#endif /*DEBUG_VIEWCHILD*/

	if( child_view ) { 
		g_assert( child_view->parent == parent_view );
		child_view->parent = NULL;
	}
	FREESID( viewchild->child_model_changed_sid, viewchild->child_model );
	parent_view->managed = 
		g_slist_remove( parent_view->managed, viewchild );

	im_free( viewchild );

	return( NULL );
}

static void *
view_viewchild_test_child_model( ViewChild *viewchild, Model *child_model )
{
#ifdef DEBUG
	printf( "view_viewchild_test_child_model: model %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( child_model ), 
		NN( IOBJECT( child_model )->name ) );
#endif /*DEBUG*/

	if( viewchild->child_model == child_model )
		return( viewchild );

	return( NULL );
}

/* Do we have a model?
 */
gboolean 
view_hasmodel( View *view ) 
{ 
	return( VOBJECT( view )->iobject != NULL ); 
}

void *
view_model_test( View *view, Model *model )
{
	if( VOBJECT( view )->iobject == IOBJECT( model ) )
		return( view );

	return( NULL );
}

/* Link to enclosing model and view. 
 */
void 
view_link( View *view, Model *model, View *parent )
{
	VIEW_GET_CLASS( view )->link( view, model, parent );
}

/* Add a child.
 */
void
view_child_add( View *parent, View *child )
{
	VIEW_GET_CLASS( parent )->child_add( parent, child );
}

/* Remove a child.
 */
void
view_child_remove( View *child )
{
	View *parent = child->parent;

	VIEW_GET_CLASS( parent )->child_remove( parent, child );
}

/* Child needs repositioning.
 */
void
view_child_position( View *child )
{
	View *parent = child->parent;

	VIEW_GET_CLASS( parent )->child_position( parent, child );
}

/* Pop child to front of stacking order.
 */
void
view_child_front( View *child )
{
	View *parent = child->parent;

	if( parent )
		VIEW_GET_CLASS( parent )->child_front( parent, child );
}

/* Break link to model. 
 */
void 
view_unlink( View *view )
{
	g_assert( view != NULL );
	g_assert( VOBJECT( view )->iobject != NULL );
	g_assert( IS_VIEW( view ) && IS_MODEL( VOBJECT( view )->iobject ) );

	FREESID( view->pos_changed_sid, VOBJECT( view )->iobject );
	FREESID( view->scrollto_sid, VOBJECT( view )->iobject );
	FREESID( view->layout_sid, VOBJECT( view )->iobject );
	FREESID( view->reset_sid, VOBJECT( view )->iobject );
	FREESID( view->front_sid, VOBJECT( view )->iobject );
	FREESID( view->child_add_sid, VOBJECT( view )->iobject );
	FREESID( view->child_remove_sid, VOBJECT( view )->iobject );
	FREESID( view->child_detach_sid, VOBJECT( view )->iobject );
	FREESID( view->child_attach_sid, VOBJECT( view )->iobject );
}

static void
view_destroy( GtkObject *object )
{
	View *view;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_VIEW( object ) );

	view = VIEW( object );

#ifdef DEBUG
	printf( "view_destroy: \"%s\"\n", G_OBJECT_TYPE_NAME( object ) );
#endif /*DEBUG*/

	/* We're probably changing the size of our enclosing column.
	 */
	view_resize( view );

	if( view->scannable )
		view_scannable_unregister( view );
	if( view->resettable )
		view_resettable_unregister( view );

	if( VOBJECT( view )->iobject )
		view_unlink( view );

	if( view->parent )
		view_child_remove( view );

	slist_map( view->managed,
		(SListMapFn) view_viewchild_destroy, NULL );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void 
view_finalize( GObject *gobject )
{
#ifdef DEBUG
	printf( "view_finalize: \"%s\"\n", G_OBJECT_TYPE_NAME( gobject ) );
#endif /*DEBUG*/

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

/* Called for model pos_changed signal ... queue a refresh. 
 */
static void
view_model_pos_changed( Model *model, View *view )
{
        g_assert( IS_MODEL( model ) );
        g_assert( IS_VIEW( view ) );

#ifdef DEBUG
        printf( "view_model_pos_changed: %s %s \"%s\"\n",
                G_OBJECT_TYPE_NAME( view ),
                G_OBJECT_TYPE_NAME( model ),
                NN( IOBJECT( model )->name ) );
#endif /*DEBUG*/

        vobject_refresh_queue( VOBJECT( view ) );
}

/* Called for model scrollto signal ... try scrolling.
 */
static void
view_model_scrollto( Model *model, ModelScrollPosition position, View *view )
{
	g_assert( IS_MODEL( model ) );
	g_assert( IS_VIEW( view ) );

#ifdef DEBUG
	printf( "view_model_scrollto: %s\n", IOBJECT( model )->name );
#endif /*DEBUG*/

	view_scrollto( view, position );
}

/* Called for model layout signal ... try to lay out children.
 */
static void
view_model_layout( Model *model, View *view )
{
	g_assert( IS_MODEL( model ) );
	g_assert( IS_VIEW( view ) );

#ifdef DEBUG
	printf( "view_model_layout: %s\n", IOBJECT( model )->name );
#endif /*DEBUG*/

	view_layout( view );
}

/* Called for model reset signal ... try resetting.
 */
static void
view_model_reset( Model *model, View *view )
{
	g_assert( IS_MODEL( model ) );
	g_assert( IS_VIEW( view ) );

#ifdef DEBUG
	printf( "view_model_reset: %s\n", IOBJECT( model )->name );
#endif /*DEBUG*/

	view_reset( view );
}

/* Called for model front signal ... bring view to front.
 */
static void
view_model_front( Model *model, View *view )
{
	g_assert( IS_MODEL( model ) );
	g_assert( IS_VIEW( view ) );

#ifdef DEBUG
	printf( "view_model_front: model %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( model ), NN( IOBJECT( model )->name ) );
	printf( "\tview %s\n", G_OBJECT_TYPE_NAME( view ) );
#endif /*DEBUG*/

	view_child_front( view );
}

/* Called for model child_add signal ... start watching that child.
 */
static void
view_model_child_add( Model *parent, Model *child, int pos, View *parent_view )
{
	ViewChild *viewchild;

#ifdef DEBUG
	printf( "view_model_child_add: parent %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( parent ), NN( IOBJECT( parent )->name ) );
#endif /*DEBUG*/

	g_assert( IS_MODEL( parent ) );
	g_assert( IS_MODEL( child ) );
	g_assert( IS_VIEW( parent_view ) );

#ifdef DEBUG
	viewchild = slist_map( parent_view->managed,
		(SListMapFn) view_viewchild_test_child_model, child );
	g_assert( !viewchild );
#endif /*DEBUG*/

	viewchild = view_viewchild_new( parent_view, child ); 
	view_viewchild_changed( child, viewchild );
}

/* Called for model child_remove signal ... stop watching that child. child
 * may have been finalized already.
 */
static void
view_model_child_remove( iContainer *parent, iContainer *child, 
	View *parent_view )
{
	ViewChild *viewchild;

#ifdef DEBUG
{
	printf( "view_model_child_remove: child %s \"%s\"; "
		"parent %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( child ), NN( IOBJECT( child )->name ),
		G_OBJECT_TYPE_NAME( parent ), NN( IOBJECT( parent )->name ) );

	printf( "view_model_child_remove: parent_view = view of %s \"%s\"\n",
		G_OBJECT_TYPE_NAME( VOBJECT( parent_view )->iobject ), 
		NN( IOBJECT( VOBJECT( parent_view )->iobject )->name ) );
}
#endif /*DEBUG*/

	viewchild = slist_map( parent_view->managed,
		(SListMapFn) view_viewchild_test_child_model, child );

	g_assert( viewchild );

	(void) view_viewchild_destroy( viewchild ); 
}

/* Called for model parent_detach signal ... remove the viewchild for this
 * child. child_attach will build a new one. 
 */
static void
view_model_child_detach( iContainer *old_parent, iContainer *child, 
	View *old_parent_view )
{
	ViewChild *viewchild;

#ifdef DEBUG
{
	printf( "view_model_child_detach: child %s \"%s\"; "
		"old_parent %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( child ), 
		NN( IOBJECT( child )->name ),
		G_OBJECT_TYPE_NAME( old_parent ), 
		NN( IOBJECT( old_parent )->name ) );

	printf( "view_model_child_detach: old_parent_view = "
			"view of %s \"%s\"\n",
		G_OBJECT_TYPE_NAME( VOBJECT( old_parent_view )->iobject ), 
		NN( IOBJECT( VOBJECT( old_parent_view )->iobject )->name ) );
}
#endif /*DEBUG*/

	viewchild = slist_map( old_parent_view->managed,
		(SListMapFn) view_viewchild_test_child_model, child );

	g_assert( viewchild );
	g_assert( !child->temp_view );

	child->temp_view = viewchild->child_view;

	(void) view_viewchild_destroy( viewchild ); 
}

/* Called for model child_attach signal ... make a new viewchild on the new
 * parent view.
 */
static void
view_model_child_attach( iContainer *new_parent, iContainer *child, int pos,
	View *new_parent_view )
{
	ViewChild *viewchild;

	g_assert( !slist_map( new_parent_view->managed,
		(SListMapFn) view_viewchild_test_child_model, child ) ); 

	viewchild = view_viewchild_new( new_parent_view, MODEL( child ) ); 

	g_assert( child->temp_view && IS_VIEW( child->temp_view ) );
	viewchild->child_view = child->temp_view;
	child->temp_view = NULL;

	viewchild->child_view->parent = new_parent_view;
}

static void *
view_real_link_sub( Model *child_model, View *parent_view )
{
	ViewChild *viewchild;

	viewchild = view_viewchild_new( parent_view, child_model ); 
	view_viewchild_changed( child_model, viewchild );

	return( NULL );
}

/* Link to model and to enclosing view. 
 */
static void 
view_real_link( View *view, Model *model, View *parent_view )
{
	g_assert( view != NULL );
	g_assert( IS_VIEW( view ) && IS_MODEL( model ) );
	g_assert( !VOBJECT( view )->iobject );

#ifdef DEBUG
	printf( "view_real_link: linking %s to model %s \"%s\"\n", 
		G_OBJECT_TYPE_NAME( view ), 
		G_OBJECT_TYPE_NAME( model ), NN( IOBJECT( model )->name ) );
#endif /*DEBUG*/

	vobject_link( VOBJECT( view ), IOBJECT( model ) );
	if( parent_view )
		view_child_add( parent_view, view );

	view->pos_changed_sid = g_signal_connect( model, "pos_changed", 
		G_CALLBACK( view_model_pos_changed ), view );
	view->scrollto_sid = g_signal_connect( model, "scrollto", 
		G_CALLBACK( view_model_scrollto ), view );
	view->layout_sid = g_signal_connect( model, "layout", 
		G_CALLBACK( view_model_layout ), view );
	view->reset_sid = g_signal_connect( model, "reset", 
		G_CALLBACK( view_model_reset ), view );
	view->front_sid = g_signal_connect( model, "front", 
		G_CALLBACK( view_model_front ), view );
	view->child_add_sid = g_signal_connect( model, "child_add", 
		G_CALLBACK( view_model_child_add ), view );
	view->child_remove_sid = g_signal_connect( model, "child_remove", 
		G_CALLBACK( view_model_child_remove ), view );
	view->child_detach_sid = g_signal_connect( model, "child_detach", 
		G_CALLBACK( view_model_child_detach ), view );
	view->child_attach_sid = g_signal_connect( model, "child_attach", 
		G_CALLBACK( view_model_child_attach ), view );

	icontainer_map( ICONTAINER( model ),
		(icontainer_map_fn) view_real_link_sub, view, NULL );

	gtk_widget_show( GTK_WIDGET( view ) );
}

static void
view_real_child_add( View *parent, View *child )
{
	ViewChild *viewchild;

	g_assert( IS_VIEW( parent ) && IS_VIEW( child ) );
	g_assert( child->parent == NULL );

#ifdef DEBUG
	printf( "view_real_child_add: parent %p %s, child %p %s\n", 
		parent,
		G_OBJECT_TYPE_NAME( parent ), 
		child,
		G_OBJECT_TYPE_NAME( child ) );
#endif /*DEBUG*/

	viewchild = slist_map( parent->managed,
		(SListMapFn) view_viewchild_test_child_model, 
		VOBJECT( child)->iobject );

	g_assert( viewchild );
	g_assert( viewchild->child_view == NULL );

	/* Not all views are true widgets (ie. get _ref()'s and _sink()'d by a
	 * parent in gtk_container()). Ref and sink ourselves to ensure that
	 * even these odd views get unfloated. See also
	 * view_real_child_remove(). Affects the tool/toolkit views, and
	 * rowview at least.
	 */
	child->parent = parent;
	viewchild->child_view = child;
	g_object_ref( GTK_OBJECT( child ) );
	gtk_object_sink( GTK_OBJECT( child ) );
}

static void
view_real_child_remove( View *parent, View *child )
{
	ViewChild *viewchild;

#ifdef DEBUG
	printf( "view_real_child_remove: parent %s, child %s\n", 
		G_OBJECT_TYPE_NAME( parent ), G_OBJECT_TYPE_NAME( child ) );
#endif /*DEBUG*/

	viewchild = slist_map( parent->managed,
		(SListMapFn) view_viewchild_test_child_model, 
		VOBJECT( child )->iobject );

	/* Can have floating views which are not part of the viewchild system.
	 */
	if( viewchild &&
		viewchild->child_view == child ) { 
		viewchild->child_view = NULL;
		g_object_unref( G_OBJECT( child ) );
	}

	child->parent = NULL;
}

static void
view_real_child_position( View *parent, View *child )
{
}

static void
view_real_child_front( View *parent, View *child )
{
}

static void 
view_real_reset( View *view )
{
	view_resettable_unregister( view );
}

static void *
view_real_scan( View *view )
{
	Model *model = MODEL( VOBJECT( view )->iobject );
	Heapmodel *heapmodel;

	view_scannable_unregister( view );

	/* If we've changed something in this model, mark it for recomp.
	 */
	if( model && IS_HEAPMODEL( model ) && 
		(heapmodel = HEAPMODEL( model ))->modified && 
		heapmodel->row ) {
		Expr *expr = heapmodel->row->expr;

		if( expr )
			(void) expr_dirty( expr, link_serial_new() );
	}

	return( NULL );
}

static void
view_class_init( ViewClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	GtkObjectClass *object_class = (GtkObjectClass*) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = view_finalize;

	object_class->destroy = view_destroy;

	/* Create signals.
	 */

	/* Init default methods.
	 */
	class->link = view_real_link;
	class->child_add = view_real_child_add;
	class->child_remove = view_real_child_remove;
	class->child_position = view_real_child_position;
	class->child_front = view_real_child_front;
	class->display = NULL;

	class->reset = view_real_reset;
	class->scan = view_real_scan;
	class->scrollto = NULL;
	class->layout = NULL;
}

static void
view_init( View *view )
{
	/* Init our instance fields.
	 */
	view->pos_changed_sid = 0;
	view->scrollto_sid = 0;
	view->layout_sid = 0;
	view->reset_sid = 0;
	view->front_sid = 0;
	view->child_add_sid = 0;
	view->child_remove_sid = 0;
	view->child_detach_sid = 0;
	view->child_attach_sid = 0;

	view->parent = NULL;

	view->scannable = FALSE;
	view->resettable = FALSE;
}

GtkType
view_get_type( void )
{
	static GtkType view_type = 0;

	if( !view_type ) {
		static const GtkTypeInfo view_info = {
			"View",
			sizeof( View ),
			sizeof( ViewClass ),
			(GtkClassInitFunc) view_class_init,
			(GtkObjectInitFunc) view_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		view_type = gtk_type_unique( TYPE_VOBJECT, &view_info );
	}

	return( view_type );
}

/* Trigger the reset method for a view.
 */
void *
view_reset( View *view )
{
	ViewClass *view_class = VIEW_GET_CLASS( view );

	if( view_class->reset )
		view_class->reset( view );

	return( NULL );
}

/* Trigger the scan method for a view.
 */
void *
view_scan( View *view )
{
	ViewClass *view_class = VIEW_GET_CLASS( view );

	if( view_class->scan )
		return( view_class->scan( view ) );

	return( NULL );
}

/* Trigger the scrollto method for a view.
 */
void *
view_scrollto( View *view, ModelScrollPosition position )
{
	ViewClass *view_class = VIEW_GET_CLASS( view );

	if( view_class->scrollto )
		view_class->scrollto( view, position );

	return( NULL );
}

/* Trigger the layout method for a view.
 */
void *
view_layout( View *view )
{
	ViewClass *view_class = VIEW_GET_CLASS( view );

	if( view_class->layout )
		view_class->layout( view );

	return( NULL );
}

static void *
view_map_sub( ViewChild *viewchild, view_map_fn fn, void *a, void *b )
{
	if( viewchild->child_view )
		return( fn( viewchild->child_view, a, b ) );
	
	return( NULL );
}

/* Map over a view's children.
 */
void *
view_map( View *view, view_map_fn fn, void *a, void *b )
{
	return( slist_map3( view->managed, 
		(SListMap3Fn) view_map_sub, (void *) fn, a, b ) );
}

/* Apply a function to view, and to all it's children.
 */
void *
view_map_all( View *view, view_map_fn fn, void *a )
{
	View *result;

	if( (result = fn( view, a, NULL )) )
		return( result );

	return( view_map( view, 
		(view_map_fn) view_map_all, (void *) fn, a ) );
}

void
view_save_as_cb( GtkWidget *menu, GtkWidget *host, View *view )
{
	Model *model = MODEL( VOBJECT( view )->iobject );

	if( IS_FILEMODEL( model ) ) {
		iWindow *iwnd = IWINDOW( view_get_toplevel( view ) );

		filemodel_inter_saveas( iwnd, FILEMODEL( model ) );
	}
}

void
view_save_cb( GtkWidget *menu, GtkWidget *host, View *view )
{
	Model *model = MODEL( VOBJECT( view )->iobject );

	if( IS_FILEMODEL( model ) ) {
		iWindow *iwnd = IWINDOW( view_get_toplevel( view ) );

		filemodel_inter_save( iwnd, FILEMODEL( model ) );
	}
}

void
view_close_cb( GtkWidget *menu, GtkWidget *host, View *view )
{
	Model *model = MODEL( VOBJECT( view )->iobject );

	if( IS_FILEMODEL( model ) ) {
		iWindow *iwnd = IWINDOW( view_get_toplevel( view ) );

		filemodel_inter_savenclose( iwnd, FILEMODEL( model ) );
	}
}

/* Callback for "activate" on a view.
 */
void
view_activate_cb( View *view )
{
	view_scannable_register( view );
	symbol_recalculate_all();
}

/* Callback for "changed" on a view.
 */
void 
view_changed_cb( View *view )
{
	/* Make sure it's on the scannable list.
	 */
	view_scannable_register( view );
}

void
view_not_implemented_cb( GtkWidget *menu, GtkWidget *host, View *view )
{
	error_top( _( "Not implemented." ) );
	iwindow_alert( GTK_WIDGET( view ), GTK_MESSAGE_ERROR );
}

GtkWidget *
view_get_toplevel( View *view )
{
	while( IS_VIEW( view ) && view->parent )
		view = view->parent;

	return( gtk_widget_get_toplevel( GTK_WIDGET( view ) ) );
}

Columnview *
view_get_columnview( View *child )
{
	View *view;

	for( view = child; view && !IS_COLUMNVIEW( view ); view = view->parent )
		;

	if( !view )
		return( NULL );

	return( COLUMNVIEW( view ) );
}

/* A view has changed size ... rethink the enclosing column geo. Helps table
 * to not break.
 */
void *
view_resize( View *view )
{
	Columnview *cview = view_get_columnview( view );

	if( cview )
		gtk_widget_queue_resize( GTK_WIDGET( cview ) );

	return( NULL );
}
