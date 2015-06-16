/* an ip region class object in a workspace
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

G_DEFINE_TYPE( iRegion, iregion, TYPE_IIMAGE ); 

void
iregion_instance_destroy( iRegionInstance *instance )
{
	instance->image_class.type = ELEMENT_NOVAL;
	instance->image_class.ele = (void *) 8;
	MANAGED_UNREF( instance->ii );
	instance->classmodel = NULL;
	instance->iregiongroup = NULL;
	heap_unregister_element( reduce_context->heap, &instance->image_class );
}

void
iregion_instance_init( iRegionInstance *instance, Classmodel *classmodel )
{
	instance->image_class.type = ELEMENT_NOVAL;
	instance->image_class.ele = (void *) 9;
	instance->ii = NULL;
	instance->area.left = 0;
	instance->area.top = 0;
	instance->area.width = 0;
	instance->area.height = 0;
	instance->classmodel = classmodel;
	instance->iregiongroup = NULL;

	heap_register_element( reduce_context->heap, &instance->image_class );
}

gboolean
iregion_instance_update( iRegionInstance *instance, PElement *root )
{
	PElement image;
	PElement image_class;
	Imageinfo *value;
	int left, top, width, height;

	if( !class_get_member_class( root, MEMBER_IMAGE, "Image", &image ) ||
		!class_get_member_image( &image, MEMBER_VALUE, &value ) ||
		!class_get_member_int( root, MEMBER_LEFT, &left ) ||
		!class_get_member_int( root, MEMBER_TOP, &top ) ||
		!class_get_member_int( root, MEMBER_WIDTH, &width ) ||
		!class_get_member_int( root, MEMBER_HEIGHT, &height ) )
		return( FALSE );

	instance->area.left = left;
	instance->area.top = top;
	instance->area.width = width;
	instance->area.height = height;

	MANAGED_UNREF( instance->ii );
	instance->ii = value;
	MANAGED_REF( value );

	PEPOINTE( &image_class, &instance->image_class );
	PEPUTPE( &image_class, &image );

	return( TRUE );
}

static void
iregion_finalize( GObject *gobject )
{
	iRegion *iregion;

#ifdef DEBUG
	printf( "iregion_finalize\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_IREGION( gobject ) );

	iregion = IREGION( gobject );

	/* My instance finalize stuff.
	 */
	iregion_instance_destroy( &iregion->instance );

	G_OBJECT_CLASS( iregion_parent_class )->finalize( gobject );
}

static void *
iregion_generate_caption_sub( iImage *iimage, 
	iRegion *iregion, gboolean *first )
{
	iImage *our_iimage = IIMAGE( iregion );
	Workspace *ws = HEAPMODEL( iregion )->row->ws;
	Row *row = HEAPMODEL( iimage )->row;

	/* Supress this name in the caption if it's a superclass. If this
	 * thing is on a super, it's on the subclass too ... not helpful to
	 * have it twice.
	 */
	if( row->sym &&
		!is_super( row->sym ) ) {
		if( *first )
			*first = FALSE;
		else 
			vips_buf_appends( &our_iimage->caption_buffer, ", " );

		row_qualified_name_relative( ws->sym, 
			row, &our_iimage->caption_buffer );
	}

	return( NULL );
}

static const char *
iregion_generate_caption( iObject *iobject ) 
{
	iRegion *iregion = IREGION( iobject );
	iImage *iimage = IIMAGE( iregion );
	const int nimages = g_slist_length( CLASSMODEL( iregion )->iimages );
	VipsBuf *buf = &iimage->caption_buffer;
	gboolean first;

	vips_buf_rewind( buf );
	heapmodel_name( HEAPMODEL( iregion ), buf );
	vips_buf_appendf( buf, " " );
	/* Expands to (eg.) "Region on A1 at (10, 10), size (50, 50)"
	 */
	vips_buf_appendf( buf, _( "on" ) );
	vips_buf_appendf( buf, " " );
	if( nimages > 1 )
		vips_buf_appendf( buf, "[" );
	first = TRUE;
	slist_map2( CLASSMODEL( iregion )->iimages,
		(SListMap2Fn) iregion_generate_caption_sub, iregion, &first );
	if( nimages > 1 )
		vips_buf_appendf( buf, "]" );
	vips_buf_appendf( buf, " " );
	vips_buf_appendf( buf, _( "at (%d, %d), size (%d, %d)" ),
		iregion->instance.area.left, iregion->instance.area.top,
		iregion->instance.area.width, iregion->instance.area.height );

	return( vips_buf_all( buf ) );
}

static void
iregion_done_cb( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys )
{
	Classmodel *classmodel = CLASSMODEL( client );
	iRegionInstance *instance = classmodel_get_instance( classmodel );
	Stringset *ss = STRINGSET( iwnd );
	Rect area;

	StringsetChild *left = stringset_child_get( ss, _( "Left" ) );
	StringsetChild *top = stringset_child_get( ss, _( "Top" ) );
	StringsetChild *width = stringset_child_get( ss, _( "Width" ) );
	StringsetChild *height = stringset_child_get( ss, _( "Height" ) );

	if( !get_geditable_int( left->entry, &area.left ) ||
		!get_geditable_int( top->entry, &area.top ) ||
		!get_geditable_int( width->entry, &area.width ) ||
		!get_geditable_int( height->entry, &area.height ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( instance ) {
		instance->area = area;
		classmodel_update( classmodel );
		symbol_recalculate_all();
	}

	nfn( sys, IWINDOW_YES );
}

static View *
iregion_view_new( Model *model, View *parent )
{
	return( iregionview_new() );
}

/* Pop up a iregion edit box. Shared with iarrow.c.
 */
void 
iregion_edit( GtkWidget *parent, Model *model )
{
	Classmodel *classmodel = CLASSMODEL( model );
	iRegionInstance *instance = classmodel_get_instance( classmodel );
	GtkWidget *ss = stringset_new();

	if( instance ) {
		char txt[256];

		im_snprintf( txt, 256, "%d", instance->area.left );
		stringset_child_new( STRINGSET( ss ), 
			_( "Left" ), txt, _( "Left edge of region" ) );
		im_snprintf( txt, 256, "%d", instance->area.top );
		stringset_child_new( STRINGSET( ss ), 
			_( "Top" ), txt, _( "Top edge of region" ) );
		im_snprintf( txt, 256, "%d", instance->area.width );
		stringset_child_new( STRINGSET( ss ), 
			_( "Width" ), txt, _( "Width of region" ) );
		im_snprintf( txt, 256, "%d", instance->area.height );
		stringset_child_new( STRINGSET( ss ), 
			_( "Height" ), txt, _( "Height of region" ) );
	}

	iwindow_set_title( IWINDOW( ss ), _( "Edit %s %s" ),
		IOBJECT_GET_CLASS_NAME( model ),
		IOBJECT( HEAPMODEL( model )->row )->name );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, classmodel );
	idialog_add_ok( IDIALOG( ss ), 
		iregion_done_cb, _( "Set %s" ), 
		IOBJECT_GET_CLASS_NAME( model ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( parent ) );
	idialog_set_iobject( IDIALOG( ss ), IOBJECT( model ) );
	idialog_set_pinup( IDIALOG( ss ), TRUE );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );
}

/* Shared with iarrow.c.
 */
void
iregion_parent_add( iContainer *child )
{
	/* Get our parent class. We can't just use the global parent_class, 
	 * since due to our lame MI scheme, this method may be called for 
	 * iarrow/ipoint etc. as well as iregion ... look up dynamically.
	 */
	gpointer parent_class = PARENT_CLASS_DYNAMIC( child );

	ICONTAINER_CLASS( parent_class )->parent_add( child );

	/* Now we're all linked up, make a child model to handle client 
	 * displays on imageviews.
	 */
	(void) iregiongroup_new( CLASSMODEL( child ) );
}

/* Shared with iarrow.c.
 */
xmlNode *
iregion_save( Model *model, xmlNode *xnode )
{
	/* Get our parent class. We can't just use the global parent_class, 
	 * since due to our lame MI scheme, this method may be called for 
	 * iarrow/ipoint etc. as well as iregion ... look up dynamically.
	 */
	gpointer parent_class = PARENT_CLASS_DYNAMIC( model );

	iRegionInstance *instance = 
		classmodel_get_instance( CLASSMODEL( model ) );

	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( instance && CLASSMODEL( model )->edited ) {
		Rect *area = &instance->area;

		if( !set_iprop( xthis, "left", area->left ) ||
			!set_iprop( xthis, "top", area->top ) ||
			!set_iprop( xthis, "width", area->width ) ||
			!set_iprop( xthis, "height", area->height ) )
			return( NULL );
	}

	return( xthis );
}

/* Shared with iarrow.c.
 */
gboolean
iregion_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	gpointer parent_class = PARENT_CLASS_DYNAMIC( model );
	iRegionInstance *instance = 
		classmodel_get_instance( CLASSMODEL( model ) );

	g_assert( IS_RHS( parent ) );

	if( instance ) {
		Rect *area = &instance->area;

		if( get_iprop( xnode, "left", &area->left ) &&
			get_iprop( xnode, "top", &area->top ) &&
			get_iprop( xnode, "width", &area->width ) &&
			get_iprop( xnode, "height", &area->height ) )
			classmodel_set_edited( CLASSMODEL( model ), TRUE );
	}

	return( MODEL_CLASS( parent_class )->load( model, 
		state, parent, xnode ) );
}

/* Need to implement _update_heap(), as not all model fields are directly
 * editable ... some are set only from expr. See also iimage.c. Shared with
 * iarrow.c.
 */
void *
iregion_update_heap( Heapmodel *heapmodel )
{
	gpointer parent_class = PARENT_CLASS_DYNAMIC( heapmodel );
	iRegionInstance *instance = 
		classmodel_get_instance( CLASSMODEL( heapmodel ) );
	Expr *expr = heapmodel->row->expr;

	Rect area;
	PElement pe;

	if( instance ) {
		/* Save any model fields that may have been set by _load() and 
		 * which might be zapped by _get_instance().
		 */
		area = instance->area;

		/* Look for the base instance and update from that.
		 */
		if( !class_get_exact( &expr->root, 
			IOBJECT( heapmodel )->name, &pe ) )
			return( FALSE );
		if( !iregion_instance_update( instance, &pe ) )
			return( heapmodel );

		/* Restore model fields from _load().
		 */
		instance->area = area;
	}

	/* Classmodel _update_heap() will do _instance_new() from the fixed up
	 * model.
	 */
	return( HEAPMODEL_CLASS( parent_class )->update_heap( heapmodel ) );
}

static void *
iregion_update_model( Heapmodel *heapmodel )
{
	iRegion *iregion = IREGION( heapmodel );

	if( HEAPMODEL_CLASS( iregion_parent_class )->update_model( heapmodel ) )
		return( heapmodel );

	/* Update who-has-displays-on-what stuff.
	 */
	classmodel_iimage_update( CLASSMODEL( iregion ), iregion->instance.ii );

	/* Make sure the caption is regenerated.
	 */
	iobject_changed( IOBJECT( heapmodel ) );

	return( NULL );
}

/* Update iRegion from heap. Shared with iarrow.c.
 */
gboolean
iregion_class_get( Classmodel *classmodel, PElement *root )
{
	gpointer parent_class = PARENT_CLASS_DYNAMIC( classmodel );
	iRegionInstance *instance = classmodel_get_instance( classmodel );

#ifdef DEBUG
	printf( "iregion_class_get: " );
	row_name_print( HEAPMODEL( classmodel )->row );
	printf( "\n" );
#endif /*DEBUG*/

	if( instance && !iregion_instance_update( instance, root ) )
		return( FALSE );

	return( CLASSMODEL_CLASS( parent_class )->class_get( 
		classmodel, root ) );
}

/* Make a new "fn value" application. Shared with iarrow.c.
 */
gboolean
iregion_class_new( Classmodel *classmodel, PElement *fn, PElement *out )
{
	Heap *heap = reduce_context->heap;
	iRegionInstance *instance = classmodel_get_instance( classmodel );

	PElement rhs;

#ifdef DEBUG
	printf( "iregion_class_new\n" );
#endif /*DEBUG*/

	/* Make application nodes.
	 */
	if( instance ) {
		heap_appl_init( out, fn );
		if( !heap_appl_add( heap, out, &rhs ) ||
			!heap_element_new( heap, 
				&instance->image_class, &rhs ) ||
			!heap_appl_add( heap, out, &rhs ) ||
			!heap_real_new( heap, instance->area.left, &rhs ) ||
			!heap_appl_add( heap, out, &rhs ) ||
			!heap_real_new( heap, instance->area.top, &rhs ) ||
			!heap_appl_add( heap, out, &rhs ) ||
			!heap_real_new( heap, instance->area.width, &rhs ) ||
			!heap_appl_add( heap, out, &rhs ) ||
			!heap_real_new( heap, instance->area.height, &rhs ) )
			return( FALSE );
	}

	return( TRUE );
}

static void *
iregion_get_instance( Classmodel *classmodel )
{
	iRegion *iregion = IREGION( classmodel );

	return( &iregion->instance );
}

static void
iregion_class_init( iRegionClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->finalize = iregion_finalize;

	iobject_class->user_name = _( "Region" );
	iobject_class->generate_caption = iregion_generate_caption;

	icontainer_class->parent_add = iregion_parent_add;

	model_class->view_new = iregion_view_new;
	model_class->edit = iregion_edit;
	model_class->save = iregion_save;
	model_class->load = iregion_load;

	heapmodel_class->update_heap = iregion_update_heap;
	heapmodel_class->update_model = iregion_update_model;

	classmodel_class->class_get = iregion_class_get;
	classmodel_class->class_new = iregion_class_new;
	classmodel_class->get_instance = iregion_get_instance;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
iregion_init( iRegion *iregion )
{
	iregion_instance_init( &iregion->instance, CLASSMODEL( iregion ) );

	iobject_set( IOBJECT( iregion ), CLASS_REGION, NULL );
}

