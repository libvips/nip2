/* image management ... a layer over the VIPS IMAGE type
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

jobs:

- reference counting layer ... in Managed base class, plus links to heap 
  garbage collection

- filesystem tracking: we stat open files and signal file_changed if we see a 
  change

- cache: several open( "fred.v" )s share a single Imageinfo, provided their 
  mtimes are all the same

- lookup table management ... if an operation can work with pixel lookup 
  tables (found by examining a flag in the VIPS function descriptor), then
  instead of operating on the image, the operation runs on the LUT associated
  with that image ... Imageinfo tracks the LUTs representing delayed eval

- dependency tracking ... an imageinfo can require several other imageinfos
  to be open for it to work properly; we follow these dependencies, and
  delay destroying an imageinfo until it's not required by any others

- temp file management ... we can make temp images on disc; we unlink() these
  temps when they're no longer needed

- imageinfo/expr association tracking ... we track when an expr 
  receives an imageinfo as its value; the info is used to get region views
  to display in the right image ... see expr_real_new_value()

- paint stuff: also undo/redo buffers, each with a "*_changed" signal

 */

/* 

more stuff:

while we transition to vips8, also use imageinfo to wrap VipsImage

most of the jobs above are pushed down into vips8 now ... except for

- reference counting layer ... in Managed base class

- filesystem tracking: we stat open files and signal file_changed if we see a
  change

- cache: several open( "fred.v" )s share a single Imageinfo, provided their 
  mtimes are all the same

 */


#include "ip.h"

/*
#define DEBUG
#define DEBUG_MAKE
#define DEBUG_OPEN
#define DEBUG_RGB
#define DEBUG_CHECK
 */

static iContainerClass *imageinfogroup_parent_class = NULL;

static void
imageinfogroup_finalize( GObject *gobject )
{
	Imageinfogroup *imageinfogroup = IMAGEINFOGROUP( gobject );

	IM_FREEF( g_hash_table_destroy, imageinfogroup->filename_hash );

	G_OBJECT_CLASS( imageinfogroup_parent_class )->finalize( gobject );
}

static void
imageinfogroup_child_add( iContainer *parent, iContainer *child, int pos )
{
	Imageinfogroup *imageinfogroup = IMAGEINFOGROUP( parent );
	Imageinfo *imageinfo = IMAGEINFO( child );
	const char *name = IOBJECT( imageinfo )->name;
	GSList *hits;

	hits = (GSList *) g_hash_table_lookup( imageinfogroup->filename_hash,
		name );
	hits = g_slist_prepend( hits, imageinfo );
	g_hash_table_insert( imageinfogroup->filename_hash, 
		(gpointer) name, (gpointer) hits );

	ICONTAINER_CLASS( imageinfogroup_parent_class )->
		child_add( parent, child, pos );
}

static void 
imageinfogroup_child_remove( iContainer *parent, iContainer *child )
{
	Imageinfogroup *imageinfogroup = IMAGEINFOGROUP( parent );
	Imageinfo *imageinfo = IMAGEINFO( child );
	const char *name = IOBJECT( imageinfo )->name;
	GSList *hits;

	hits = (GSList *) g_hash_table_lookup( imageinfogroup->filename_hash,
		name );
	g_assert( hits );
	hits = g_slist_remove( hits, imageinfo );

	/* child is going away (probably), so we don't want to link hits back
	 * on again with child->name as the key ... if possible, look down
	 * hits for another name we can use instead.
	 */
	if( hits ) {
		const char *new_name = IOBJECT( hits->data )->name;

		g_hash_table_replace( imageinfogroup->filename_hash, 
			(gpointer) new_name, (gpointer) hits );
	}
	else
		g_hash_table_remove( imageinfogroup->filename_hash,
			(gpointer) name );

	ICONTAINER_CLASS( imageinfogroup_parent_class )->
		child_remove( parent, child );
}

static void
imageinfogroup_class_init( ImageinfogroupClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iContainerClass *icontainer_class = ICONTAINER_CLASS( class );

	imageinfogroup_parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = imageinfogroup_finalize;

	icontainer_class->child_add = imageinfogroup_child_add;
	icontainer_class->child_remove = imageinfogroup_child_remove;
}

static void
imageinfogroup_init( Imageinfogroup *imageinfogroup )
{
#ifdef DEBUG
	printf( "imageinfogroup_init\n" );
#endif /*DEBUG*/

	imageinfogroup->filename_hash = 
		g_hash_table_new( g_str_hash, g_str_equal );
}

GType
imageinfogroup_get_type( void )
{
	static GType imageinfogroup_type = 0;

	if( !imageinfogroup_type ) {
		static const GTypeInfo info = {
			sizeof( ImageinfogroupClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) imageinfogroup_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Imageinfogroup ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) imageinfogroup_init,
		};

		imageinfogroup_type = g_type_register_static( TYPE_ICONTAINER, 
			"Imageinfogroup", &info, 0 );
	}

	return( imageinfogroup_type );
}

Imageinfogroup *
imageinfogroup_new( void )
{
	Imageinfogroup *imageinfogroup = IMAGEINFOGROUP( 
		g_object_new( TYPE_IMAGEINFOGROUP, NULL ) );

	return( imageinfogroup );
}

static void *
imageinfogroup_lookup_test( Imageinfo *imageinfo, struct stat *buf )
{
	const char *name = IOBJECT( imageinfo )->name;

	if( name && buf->st_mtime == imageinfo->mtime )
		return( imageinfo );

	return( NULL );
}

/* Look up by filename ... mtimes have to match too.
 */
static Imageinfo *
imageinfogroup_lookup( Imageinfogroup *imageinfogroup, const char *filename )
{
	GSList *hits;
	Imageinfo *imageinfo;
	struct stat buf;

	if( stat( filename, &buf ) == 0 && 
		(hits = (GSList *) g_hash_table_lookup( 
			imageinfogroup->filename_hash, filename )) &&
		(imageinfo = IMAGEINFO( slist_map( hits,
			(SListMapFn) imageinfogroup_lookup_test, &buf ) )) ) 
		return( imageinfo );

	return( NULL );
}

/* Our signals. 
 */
enum {
	SIG_AREA_CHANGED,	/* Area of image has changed: update screen */
	SIG_AREA_PAINTED,	/* Area of image has been painted */
	SIG_UNDO_CHANGED,	/* Undo/redo state has changed */
	SIG_FILE_CHANGED,	/* Underlying file seems to have changed */
	SIG_INVALIDATE,		/* IMAGE* has been invalidated */
	SIG_LAST
};

static ManagedClass *parent_class = NULL;

static guint imageinfo_signals[SIG_LAST] = { 0 };

#if defined(DEBUG) || defined(DEBUG_OPEN) || defined(DEBUG_RGB) || \
	defined(DEBUG_CHECK) || defined(DEBUG_MAKE) 
static void
imageinfo_print( Imageinfo *imageinfo )
{
	printf( " \"%s\" mtime = %d (%p)\n",
		IOBJECT( imageinfo )->name,
		(int) imageinfo->mtime,
		imageinfo );
}
#endif

void *
imageinfo_area_changed( Imageinfo *imageinfo, Rect *dirty )
{
	g_assert( IS_IMAGEINFO( imageinfo ) );

#ifdef DEBUG
	printf( "imageinfo_area_changed: left = %d, top = %d, "
		"width = %d, height = %d\n",
		dirty->left, dirty->top, dirty->width, dirty->height );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( imageinfo ), 
		imageinfo_signals[SIG_AREA_CHANGED], 0, dirty );

	return( NULL );
}

void *
imageinfo_area_painted( Imageinfo *imageinfo, Rect *dirty )
{
	g_assert( IS_IMAGEINFO( imageinfo ) );

#ifdef DEBUG
	printf( "imageinfo_area_painted: left = %d, top = %d, "
		"width = %d, height = %d\n",
		dirty->left, dirty->top, dirty->width, dirty->height );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( imageinfo ), 
		imageinfo_signals[SIG_AREA_PAINTED], 0, dirty );

	return( NULL );
}

static void *
imageinfo_undo_changed( Imageinfo *imageinfo )
{
	g_assert( IS_IMAGEINFO( imageinfo ) );

	g_signal_emit( G_OBJECT( imageinfo ), 
		imageinfo_signals[SIG_UNDO_CHANGED], 0 );

	return( NULL );
}

static void *
imageinfo_file_changed( Imageinfo *imageinfo )
{
	g_assert( IS_IMAGEINFO( imageinfo ) );

#ifdef DEBUG_CHECK
	printf( "imageinfo_file_changed:" );
	imageinfo_print( imageinfo );
#endif /*DEBUG_CHECK*/

	g_signal_emit( G_OBJECT( imageinfo ), 
		imageinfo_signals[SIG_FILE_CHANGED], 0 );

	return( NULL );
}

static void *
imageinfo_invalidate( Imageinfo *imageinfo )
{
	g_assert( IS_IMAGEINFO( imageinfo ) );

#ifdef DEBUG_CHECK
	printf( "imageinfo_invalidate:" );
	imageinfo_print( imageinfo );
#endif /*DEBUG_CHECK*/

	g_signal_emit( G_OBJECT( imageinfo ), 
		imageinfo_signals[SIG_INVALIDATE], 0 );

	return( NULL );
}

void
imageinfo_expr_add( Imageinfo *imageinfo, Expr *expr )
{
#ifdef DEBUG
	printf( "imageinfo_expr_add: " );
	expr_name_print( expr );
	printf( "has imageinfo \"%s\" as value\n", imageinfo->im->filename );
#endif /*DEBUG*/

	g_assert( !g_slist_find( imageinfo->exprs, expr ) );
	g_assert( !expr->imageinfo );

	expr->imageinfo = imageinfo;
	imageinfo->exprs = g_slist_prepend( imageinfo->exprs, expr );
}

void *
imageinfo_expr_remove( Expr *expr, Imageinfo *imageinfo )
{
#ifdef DEBUG
	printf( "imageinfo_expr_remove: " );
	expr_name_print( expr );
	printf( "has lost imageinfo \"%s\" as value\n", 
		imageinfo->im->filename );
#endif /*DEBUG*/

	g_assert( expr->imageinfo );
	g_assert( g_slist_find( imageinfo->exprs, expr ) );
	g_assert( expr->imageinfo == imageinfo );

	expr->imageinfo = NULL;
	imageinfo->exprs = g_slist_remove( imageinfo->exprs, expr );

	return( NULL );
}

GSList *
imageinfo_expr_which( Imageinfo *imageinfo )
{
	return( imageinfo->exprs );
}

/* Find the underlying image in an imageinfo.
 */
IMAGE *
imageinfo_get_underlying( Imageinfo *imageinfo )
{
	if( imageinfo->underlying )
		return( imageinfo_get_underlying( imageinfo->underlying ) );
	else
		return(  imageinfo->im );
}

/* Free up an undo fragment. 
 */
static void
imageinfo_undofragment_free( Undofragment *frag )
{
	IM_FREEF( im_close, frag->im );
	IM_FREE( frag );
}

/* Free an undo buffer.
 */
static void
imageinfo_undobuffer_free( Undobuffer *undo )
{
	slist_map( undo->frags, 
		(SListMapFn) imageinfo_undofragment_free, NULL );
	IM_FREEF( g_slist_free, undo->frags );
	IM_FREE( undo );
}

/* Free all undo information attached to an imageinfo.
 */
static void
imageinfo_undo_free( Imageinfo *imageinfo )
{	
	slist_map( imageinfo->redo, 
		(SListMapFn) imageinfo_undobuffer_free, NULL );
	IM_FREEF( g_slist_free, imageinfo->redo );
	slist_map( imageinfo->undo, 
		(SListMapFn) imageinfo_undobuffer_free, NULL );
	IM_FREEF( g_slist_free, imageinfo->undo );
	IM_FREEF( imageinfo_undobuffer_free, imageinfo->cundo );
}

static void
imageinfo_dispose_eval( Imageinfo *imageinfo )
{
	imageinfo->monitored = FALSE;

	/* Make sure any callbacks from the IMAGE stop working.
	 */
	if( imageinfo->proxy ) {
		imageinfo->proxy->imageinfo = NULL;
		imageinfo->proxy = NULL;
	}
}

static void
imageinfo_dispose( GObject *gobject )
{
	Imageinfo *imageinfo = IMAGEINFO( gobject );

#ifdef DEBUG_OPEN
	printf( "imageinfo_dispose:" );
	imageinfo_print( imageinfo );
#endif /*DEBUG_OPEN*/

	slist_map( imageinfo->exprs, 
		(SListMapFn) imageinfo_expr_remove, imageinfo );
	g_assert( !imageinfo->exprs );

	imageinfo_dispose_eval( imageinfo );

	IM_FREEF( g_source_remove, imageinfo->check_tid );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

/* Final death!
 */
static void 
imageinfo_finalize( GObject *gobject )
{
	Imageinfo *imageinfo = IMAGEINFO( gobject );
	IMAGE *im = imageinfo_get_underlying( imageinfo );
	gboolean isfile = im ? im_isfile( im ) : FALSE;
	char name[FILENAME_MAX];

#ifdef DEBUG_MAKE
	printf( "imageinfo_finalize:" ); 
	imageinfo_print( imageinfo );
#endif /*DEBUG_MAKE*/

	if( imageinfo->dfile && isfile ) 
		/* We must close before we delete to make sure we
		 * get the desc file too ... save the filename.
		 */
		im_strncpy( name, im->filename, FILENAME_MAX - 5 );

	IM_FREEF( im_close, imageinfo->im );
	IM_FREEF( im_close, imageinfo->mapped_im );
	IM_FREEF( im_close, imageinfo->identity_lut );

	if( imageinfo->dfile && isfile ) {
#ifdef DEBUG_OPEN
		printf( "imageinfo_destroy: unlinking \"%s\"\n", name );
#endif /*DEBUG_OPEN*/

		unlinkf( "%s", name );
		strcpy( name + strlen( name ) - 1, "desc" );
		unlinkf( "%s", name );
		iobject_changed( IOBJECT( main_imageinfogroup ) );
	}

	MANAGED_UNREF( imageinfo->underlying );

	imageinfo_undo_free( imageinfo );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

/* Make an info string about an imageinfo.
 */
static void
imageinfo_info( iObject *iobject, VipsBuf *buf )
{
	Imageinfo *imageinfo = IMAGEINFO( iobject );

	vips_buf_appendi( buf, imageinfo_get( FALSE, imageinfo ) );

	/* Don't chain up to parent->info(), we don't want all the other
	 * stuff, this is going to be used for a caption.
	 */
}

static void
imageinfo_real_area_changed( Imageinfo *imageinfo, Rect *dirty )
{
}

static void
imageinfo_real_area_painted( Imageinfo *imageinfo, Rect *dirty )
{
	/* Cache attaches to this signal and invalidates on paint. Trigger a
	 * repaint in turn.
	 */
	imageinfo_area_changed( imageinfo, dirty );
}

static void
imageinfo_real_undo_changed( Imageinfo *imageinfo )
{
}

static void
imageinfo_real_file_changed( Imageinfo *imageinfo )
{
}

static void
imageinfo_real_invalidate( Imageinfo *imageinfo )
{
}

static void
imageinfo_class_init( ImageinfoClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );
	ManagedClass *managed_class = MANAGED_CLASS( class );

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = imageinfo_dispose;
	gobject_class->finalize = imageinfo_finalize;

	iobject_class->info = imageinfo_info;

	/* Timeout on unreffed images.
	 */
	managed_class->keepalive = 60.0;

	class->area_changed = imageinfo_real_area_changed;
	class->area_painted = imageinfo_real_area_painted;
	class->undo_changed = imageinfo_real_undo_changed;
	class->file_changed = imageinfo_real_file_changed;
	class->invalidate = imageinfo_real_invalidate;

	/* Create signals.
	 */
	imageinfo_signals[SIG_AREA_CHANGED] = g_signal_new( "area_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ImageinfoClass, area_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		G_TYPE_POINTER );
	imageinfo_signals[SIG_AREA_PAINTED] = g_signal_new( "area_painted",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ImageinfoClass, area_painted ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		G_TYPE_POINTER );
	imageinfo_signals[SIG_UNDO_CHANGED] = g_signal_new( "undo_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ImageinfoClass, undo_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	imageinfo_signals[SIG_FILE_CHANGED] = g_signal_new( "file_changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ImageinfoClass, file_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	imageinfo_signals[SIG_INVALIDATE] = g_signal_new( "invalidate",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( ImageinfoClass, invalidate ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
}

static void
imageinfo_init( Imageinfo *imageinfo )
{
#ifdef DEBUG_MAKE
	printf( "imageinfo_init: %p\n", imageinfo );
#endif /*DEBUG_MAKE*/

	imageinfo->im = NULL;
	imageinfo->mapped_im = NULL;
	imageinfo->identity_lut = NULL;
	imageinfo->underlying = NULL;
	imageinfo->proxy = NULL;

	imageinfo->dfile = FALSE;
	imageinfo->from_file = FALSE;
	imageinfo->mtime = 0;
	imageinfo->exprs = NULL;
	imageinfo->ok_to_paint = FALSE;
	imageinfo->undo = NULL;
	imageinfo->redo = NULL;
	imageinfo->cundo = NULL;

	imageinfo->monitored = FALSE;

	imageinfo->check_mtime = 0;
	imageinfo->check_tid = 0;
}

GType
imageinfo_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ImageinfoClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) imageinfo_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Imageinfo ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) imageinfo_init,
		};

		type = g_type_register_static( TYPE_MANAGED, 
			"Imageinfo", &info, 0 );
	}

	return( type );
}

static int
imageinfo_proxy_eval( Imageinfoproxy *proxy )
{
	Imageinfo *imageinfo = proxy->imageinfo;

	if( imageinfo ) 
		if( progress_update_percent( imageinfo->im->time->percent,
			imageinfo->im->time->eta ) ) {
			im_error( "nip2", _( "User cancelled operation" ) );
			return( -1 );
		}

	return( 0 );
}

static int
imageinfo_proxy_invalidate( Imageinfoproxy *proxy )
{
	Imageinfo *imageinfo = proxy->imageinfo;

	if( imageinfo ) 
		imageinfo_invalidate( imageinfo );

	return( 0 );
}

static int
imageinfo_proxy_preclose( Imageinfoproxy *proxy )
{
	Imageinfo *imageinfo = proxy->imageinfo;

	/* Remove everything related to progress.
	 */
	if( imageinfo ) 
		imageinfo_dispose_eval( imageinfo );

	return( 0 );
}

/* Add a proxy to track IMAGE events.
 */
static void
imageinfo_proxy_add( Imageinfo *imageinfo )
{
	/* Only if we're running interactively.
	 */
	if( !main_window_top )
		return;

	/* Already being monitored?
	 */
	if( imageinfo->monitored ) 
		return;
	imageinfo->monitored = TRUE;

        /* Need a proxy on IMAGE.
         */ 
	g_assert( !imageinfo->proxy );
	if( !(imageinfo->proxy = IM_NEW( imageinfo->im, Imageinfoproxy )) )
	if( !(imageinfo->proxy = IM_NEW( NULL, Imageinfoproxy )) )
		return;
	imageinfo->proxy->im = imageinfo->im;
	imageinfo->proxy->imageinfo = imageinfo;

	(void) im_add_eval_callback( imageinfo->im, 
		(im_callback_fn) imageinfo_proxy_eval, 
		imageinfo->proxy, NULL );

	(void) im_add_invalidate_callback( imageinfo->im, 
		(im_callback_fn) imageinfo_proxy_invalidate, 
		imageinfo->proxy, NULL );

	/* Has to be preclose, because we want to be sure we disconnect before 
	 * the proxy is freed on a close callback.
	 */
	(void) im_add_preclose_callback( imageinfo->im, 
		(im_callback_fn) imageinfo_proxy_preclose, 
		imageinfo->proxy, NULL );
}

/* Make a basic imageinfo. No refs, will be destroyed on next GC. If name is
 * NULL, make a temp name up; otherwise name needs to be unique.
 */
Imageinfo *
imageinfo_new( Imageinfogroup *imageinfogroup, 
	Heap *heap, IMAGE *im, const char *name )
{
	Imageinfo *imageinfo = 
		IMAGEINFO( g_object_new( TYPE_IMAGEINFO, NULL ) );
	char buf[FILENAME_MAX];

#ifdef DEBUG_OPEN
	printf( "imageinfo_new: %p \"%s\"\n", imageinfo, im->filename );
#endif /*DEBUG_OPEN*/

	managed_link_heap( MANAGED( imageinfo ), heap );

	if( !name ) {
		if( !temp_name( buf, "v" ) ) 
			/* Will be freed on next GC.
			 */
			return( NULL );

		name = buf;
	}
	iobject_set( IOBJECT( imageinfo ), name, NULL );

	/* Only record the pointer when we know we will make the imageinfo
	 * successfully.
	 */
	imageinfo->im = im;

	icontainer_child_add( ICONTAINER( imageinfogroup ),
		ICONTAINER( imageinfo ), -1 );
	imageinfo_proxy_add( imageinfo );

	return( imageinfo );
}

/* An image is a result of a LUT operation on an earlier imageinfo.
 */
void
imageinfo_set_underlying( Imageinfo *top_imageinfo, Imageinfo *imageinfo )
{
	g_assert( !top_imageinfo->underlying );

	top_imageinfo->underlying = imageinfo;
	MANAGED_REF( top_imageinfo->underlying );
}

/* Make a temp image. Deleted on close. No refs: closed on next GC. If you
 * want it to stick around, ref it!
 */
Imageinfo *
imageinfo_new_temp( Imageinfogroup *imageinfogroup, 
	Heap *heap, const char *name, const char *mode )
{
	IMAGE *im;
	char tname[FILENAME_MAX];
	Imageinfo *imageinfo;

	if( !temp_name( tname, "v" ) || !(im = im_open( tname, mode )) )
		return( NULL );
	if( !(imageinfo = imageinfo_new( imageinfogroup, heap, im, name )) ) {
		im_close( im );
		return( NULL );
	}
	imageinfo->dfile = TRUE;

	return( imageinfo );
}

/* Need this context during imageinfo_open_image_input().
 */
typedef struct _ImageinfoOpen {
	Imageinfogroup *imageinfogroup;
	Heap *heap;
	const char *filename;
	GtkWidget *parent;
} ImageinfoOpen;

/* Open for read ... returns a non-heap pointer, destroy if it goes in the
 * heap.
 */
static Imageinfo *
imageinfo_open_image_input( const char *filename, ImageinfoOpen *open )
{
	Imageinfo *imageinfo;
	VipsFormatClass *format;

	if( !(format = vips_format_for_file( filename )) ) 
		return( NULL );

	if( strcmp( VIPS_OBJECT_CLASS( format )->nickname, "vips" ) == 0 ) {
		IMAGE *im;

		if( !(im = im_open( filename, "r" )) ) 
			return( NULL );

		if( !(imageinfo = imageinfo_new( open->imageinfogroup, 
			open->heap, im, open->filename )) ) {
			im_close( im );
			return( NULL );
		}
		MANAGED_REF( imageinfo );

#ifdef DEBUG_OPEN
		printf( "imageinfo_open_image_input: opened VIPS \"%s\"\n", 
			filename );
#endif /*DEBUG_OPEN*/
	}
	else {
		VipsFormatFlags flags = 
			vips_format_get_flags( format, filename );
		const char *mode = flags & VIPS_FORMAT_PARTIAL ? "p" : "w";

		if( !(imageinfo = imageinfo_new_temp( open->imageinfogroup, 
			open->heap, open->filename, mode )) )
			return( NULL );
		MANAGED_REF( imageinfo );
		if( format->load( filename, imageinfo->im ) ||
			im_histlin( imageinfo->im, "im_copy %s %s",
				filename, imageinfo->im->filename ) ) {
			MANAGED_UNREF( imageinfo );
			return( NULL );
		}

#ifdef DEBUG_OPEN
		printf( "imageinfo_open_image_input: "
			"opened %s \"%s\"\n", format->name, filename );
#endif /*DEBUG_OPEN*/
	}

	/* Get ready for input.
 	 */
	if( im_pincheck( imageinfo->im ) ) 
		return( NULL );

	/* The rewind will have removed everything from the IMAGE. Reattach
	 * progress.
	 */
	imageinfo_proxy_add( imageinfo );

	/* Attach the original filename ... pick this up again later as a
	 * save default.
	 */
	if( im_meta_set_string( imageinfo->im, ORIGINAL_FILENAME, filename ) )
		return( NULL );

	return( imageinfo );
}

/* Was this ii loaded from a file (ie. ->name contains a filename the user
 * might recognise).
 */
gboolean 
imageinfo_is_from_file( Imageinfo *imageinfo )
{
	return( IOBJECT( imageinfo )->name && imageinfo->from_file );
}

static gint
imageinfo_attach_check_cb( Imageinfo *imageinfo )
{
	if( imageinfo_is_from_file( imageinfo ) && imageinfo->check_tid ) {
		struct stat buf;

		if( !stat( IOBJECT( imageinfo )->name, &buf ) &&
			buf.st_mtime != imageinfo->check_mtime ) {
			imageinfo->check_mtime = buf.st_mtime;
			imageinfo_file_changed( imageinfo );
		}
	}

	return( TRUE );
}

/* Start checking this file for updates, signal reload if there is one.
 */
static void
imageinfo_attach_check( Imageinfo *imageinfo )
{
	if( imageinfo_is_from_file( imageinfo ) && !imageinfo->check_tid ) {
		struct stat buf;

		/* Need to be able to state to be able to track a file.
		 */
		if( stat( IOBJECT( imageinfo )->name, &buf ) )
			return;

		imageinfo->mtime = buf.st_mtime;
		imageinfo->check_mtime = imageinfo->mtime;
		imageinfo->check_tid = g_timeout_add( 1000, 
			(GSourceFunc) imageinfo_attach_check_cb, imageinfo );

#ifdef DEBUG_CHECK
		printf( "imageinfo_attach_check: starting to check" );
		imageinfo_print( imageinfo );
#endif /*DEBUG_CHECK*/
	}
	else
		IM_FREEF( g_source_remove, imageinfo->check_tid );
}

/* Open a filename for input.
 */
Imageinfo *
imageinfo_new_input( Imageinfogroup *imageinfogroup, GtkWidget *parent,
	Heap *heap, const char *filename )
{
	Imageinfo *imageinfo;
	ImageinfoOpen open;

	if( (imageinfo = imageinfogroup_lookup( imageinfogroup, filename )) ) {
		/* We always make a new non-heap pointer.
		 */
		MANAGED_REF( imageinfo );
		return( imageinfo );
	}

	open.imageinfogroup = imageinfogroup;
	open.heap = heap;
	open.filename = filename;
	open.parent = parent;

        if( !(imageinfo = (Imageinfo *) callv_string_filename( 
		(callv_string_fn) imageinfo_open_image_input, 
		filename, &open, NULL, NULL )) ) {
		error_top( _( "Unable to open image." ) );
		error_sub( _( "Unable to open file \"%s\" as image." ), 
			filename );
		error_vips();
                return( NULL );
        }

	imageinfo->from_file = TRUE;
	imageinfo_attach_check( imageinfo );

	return( imageinfo );
}

/* Add an identity lut, if this is a LUTtable image.
 */
static IMAGE *
imageinfo_get_identity_lut( Imageinfo *imageinfo )
{
	if( imageinfo->im->Coding == IM_CODING_NONE && 
		imageinfo->im->BandFmt == IM_BANDFMT_UCHAR ) {
		if( !imageinfo->identity_lut ) {
			char tname[FILENAME_MAX];
			IMAGE *im;

			if( !temp_name( tname, "v" ) || 
				!(im = im_open( tname, "p" )) )
				return( NULL );
			imageinfo->identity_lut = im;

			if( im_identity( imageinfo->identity_lut, 
				imageinfo->im->Bands ) || 
				im_histlin( imageinfo->identity_lut, 
					"im_identity %s %d",
					imageinfo->identity_lut->filename,
					imageinfo->im->Bands ) ) 
				return( NULL );
		}

		return( imageinfo->identity_lut );
	}
	else
		return( NULL );
}

static IMAGE *
imageinfo_get_mapped( Imageinfo *imageinfo )
{
	if( !imageinfo->mapped_im ) {
		IMAGE *im = imageinfo_get_underlying( imageinfo );
		IMAGE *mapped_im;
		char name[FILENAME_MAX];
		char *argv[4];

		if( !temp_name( name, "v" ) || 
			!(mapped_im = im_open( name, "p" )) ) 
			return( NULL );
		argv[0] = im->filename;
		argv[1] = mapped_im->filename;
		argv[2] = imageinfo->im->filename;
		argv[3] = NULL;
		if( im_maplut( im, mapped_im, imageinfo->im ) ||
			im_updatehist( mapped_im, "im_maplut", 3, argv ) ) {
			im_close( mapped_im );
			error_vips_all();
			return( NULL );
		}
		imageinfo->mapped_im = mapped_im;
	}

	return( imageinfo->mapped_im );
}

/* Get a lut ... or not!
 */
IMAGE *
imageinfo_get( gboolean use_lut, Imageinfo *imageinfo )
{
	if( !imageinfo ) 
		return( NULL );

	if( use_lut && imageinfo->underlying ) 
		return( imageinfo->im );
	if( use_lut && !imageinfo->underlying ) {
		IMAGE *lut;

		if( (lut = imageinfo_get_identity_lut( imageinfo )) )
			return( lut );
		else
			return( imageinfo->im );
	}
	else if( !use_lut && imageinfo->underlying )
		return( imageinfo_get_mapped( imageinfo ) );
	else
		return( imageinfo->im );
}

/* Do a set of II all refer to the same underlying image? Used to spot
 * LUTable optimisations.
 */
gboolean
imageinfo_same_underlying( Imageinfo *imageinfo[], int n )
{
	int i;

	if( n < 2 )
		return( TRUE );
	else {
		IMAGE *first = imageinfo_get_underlying( imageinfo[0] );

		for( i = 1; i < n; i++ )
			if( imageinfo_get_underlying( imageinfo[i] ) != first )
				return( FALSE );

		return( TRUE );
	}
}

/* Write to a filename.
 */
gboolean
imageinfo_write( Imageinfo *imageinfo, const char *name )
{
	Imageinfogroup *imageinfogroup = 
		IMAGEINFOGROUP( ICONTAINER( imageinfo )->parent );
	IMAGE *im = imageinfo_get( FALSE, imageinfo );
	char filename[FILENAME_MAX];
	char filemode[FILENAME_MAX];

	im_filename_split( name, filename, filemode );

	if( (imageinfo = imageinfogroup_lookup( imageinfogroup, filename )) ) {
                error_top( _( "Unable to write to file." ) );
		error_sub( _( "File \"%s\" is already open for read." ), 
			filename );

                return( FALSE );
	}

	if( vips_format_write( im, name ) ) {
		error_top( _( "Unable to write to file." ) );
		error_sub( _( "Error writing image to file \"%s\"." ), 
			filename );
		error_vips();

		return( FALSE );
	}

	return( TRUE );
}

static gboolean
imageinfo_make_paintable( Imageinfo *imageinfo )
{
	if( im_rwcheck( imageinfo->im ) ) {
		error_top( _( "Unable to paint on image." ) );
		error_sub( _( "Unable to get write permission for "
			"file \"%s\".\nCheck permission settings." ), 
			imageinfo->im->filename );
		error_vips();
		return( FALSE );
	}

	imageinfo->ok_to_paint = TRUE;

	return( TRUE );
}

static void
imageinfo_check_paintable_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Imageinfo *imageinfo = IMAGEINFO( client );

	if( !imageinfo_make_paintable( imageinfo ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	nfn( sys, IWINDOW_YES );
}

/* Check painting is OK. nfn() called on "ok!". Returns FALSE if it's
 * not immediately obvious that we can paint.
 */
gboolean
imageinfo_check_paintable( Imageinfo *imageinfo, GtkWidget *parent,
	iWindowNotifyFn nfn, void *sys )
{
	IMAGE *im = imageinfo_get( FALSE, imageinfo );

	if( im_isfile( im ) && 
		!imageinfo->dfile && !imageinfo->ok_to_paint ) {
		iDialog *idlg;

		idlg = box_yesno( parent,
			imageinfo_check_paintable_cb, 
				iwindow_true_cb, imageinfo,
			nfn, sys,
			_( "Modify" ),
			_( "Modify disc file?" ),
			_( "This image is being shown directly from the "
			"disc file:\n\n"
			"   %s\n\n"
			"If you paint on this file, it will be permanently "
			"changed. If something goes wrong, you may lose work. "
			"Are you sure you want to modify this file?" ),
			IOBJECT( imageinfo )->name );
		idialog_set_iobject( idlg, IOBJECT( imageinfo ) );

		return( FALSE );
	}
	else if( !im_isfile( im ) && !imageinfo->ok_to_paint ) {
		if( !imageinfo_make_paintable( imageinfo ) ) {
			nfn( sys, IWINDOW_ERROR );
			return( FALSE );
		}
	}

	nfn( sys, IWINDOW_YES );

	return( TRUE );
}

/* Try to get an Imageinfo from a symbol.
 */
Imageinfo *
imageinfo_sym_image( Symbol *sym )
{
        PElement *root = &sym->expr->root;

        if( sym->type == SYM_VALUE && PEISIMAGE( root ) )
                return( PEGETII( root ) );
        else
                return( NULL );
}

/* Brush definitions.
 */
static PEL imageinfo_brush1[] = {
	0xff
};
static PEL imageinfo_brush2[] = {
	0xff, 0xff,
	0xff, 0xff
};
static PEL imageinfo_brush3[] = {
	0x00, 0xff, 0x00,
	0xff, 0xff, 0xff,
	0x00, 0xff, 0x00
};
static PEL imageinfo_brush4[] = {
	0x00, 0xff, 0xff, 0x00,
	0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff,
	0x00, 0xff, 0xff, 0x00
};
static PEL imageinfo_brush5[] = {
	0x00, 0xff, 0xff, 0xff, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0xff, 0xff, 0xff, 0x00
};
static PEL imageinfo_brush6[] = {
	0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff, 0xff, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0xff, 0xff, 0xff, 0xff, 0x00,
	0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
};
static PEL imageinfo_brush7[] = {
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
};
static PEL imageinfo_brush8[] = {
	0x00, 0xff,
	0xff, 0xff
};
static PEL imageinfo_brush9[] = {
	0x00, 0x00, 0xff,
	0x00, 0xff, 0xff,
	0xff, 0xff, 0x00
};
static PEL imageinfo_brush10[] = {
	0x00, 0x00, 0x00, 0xff,
	0x00, 0x00, 0xff, 0xff,
	0x00, 0xff, 0xff, 0x00,
	0xff, 0xff, 0x00, 0x00
};
static PEL imageinfo_brush11[] = {
	0x00, 0x00, 0x00, 0x00, 0xff,
	0x00, 0x00, 0x00, 0xff, 0xff,
	0x00, 0x00, 0xff, 0xff, 0x00,
	0x00, 0xff, 0xff, 0x00, 0x00,
	0xff, 0xff, 0x00, 0x00, 0x00
};
static PEL imageinfo_brush12[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
	0x00, 0x00, 0x00, 0xff, 0xff, 0x00,
	0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
	0x00, 0xff, 0xff, 0x00, 0x00, 0x00,
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00
};
static PEL imageinfo_brush13[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static PEL *imageinfo_brush_masks[] = {
	imageinfo_brush1, imageinfo_brush2, imageinfo_brush3, 
	imageinfo_brush4, imageinfo_brush5, imageinfo_brush6, 
	imageinfo_brush7, imageinfo_brush8, imageinfo_brush9, 
	imageinfo_brush10, imageinfo_brush11, imageinfo_brush12, 
	imageinfo_brush13
};

static Rect imageinfo_brush_shapes[] = {
	{ 0, 0, 1, 1 },			/* PAINTBOX_1ROUND */
	{ -1, -1, 2, 2 },		/* PAINTBOX_2ROUND */
	{ -1, -1, 3, 3 },		/* PAINTBOX_3ROUND */
	{ -2, -2, 4, 4 },		/* PAINTBOX_4ROUND */
	{ -2, -2, 5, 5 },		/* PAINTBOX_5ROUND */
	{ -3, -3, 6, 6 },		/* PAINTBOX_6ROUND */
	{ -5, -5, 10, 10 },		/* PAINTBOX_10ROUND */
	{ -1, -1, 2, 2 },		/* PAINTBOX_2ITALIC */
	{ -1, -1, 3, 3 },		/* PAINTBOX_3ITALIC */
	{ -2, -2, 4, 4 },		/* PAINTBOX_4ITALIC */
	{ -2, -2, 5, 5 },		/* PAINTBOX_5ITALIC */
	{ -3, -3, 6, 6 },		/* PAINTBOX_6ITALIC */
	{ -5, -5, 10, 10 }		/* PAINTBOX_10ITALIC */
};

static Undofragment *
imageinfo_undofragment_new( Undobuffer *undo )
{
	Undofragment *frag = INEW( NULL, Undofragment );

	frag->undo = undo;
	frag->im = NULL;

	return( frag );
}

static Undobuffer *
imageinfo_undobuffer_new( Imageinfo *imageinfo )
{	
	Undobuffer *undo = INEW( NULL, Undobuffer );

	undo->imageinfo = imageinfo;
	undo->frags = NULL;

	/* No pixels in bounding box at the moment.
	 */
	undo->bbox.left = 0;
	undo->bbox.top = 0;
	undo->bbox.width = 0;
	undo->bbox.height = 0;

	return( undo );
}

/* Grab from the image into an IMAGE buffer. Always grab to memory.
 */
static IMAGE *
imageinfo_undo_grab_area( IMAGE *im, Rect *dirty )
{
	IMAGE *save;

	/* Make new image to extract to. 
	 */
	if( !(save = im_open( "undo buffer", "t" )) )
		return( NULL );

	/* Try to extract from im.
	 */
	if( im_extract_area( im, save, 
		dirty->left, dirty->top, dirty->width, dirty->height ) ) {
		im_close( save );
		error_vips_all();
		return( NULL );
	}

	return( save );
}

/* Grab into an undo fragment. Add frag to frag list on undo buffer, expand
 * bounding box.
 */
static Undofragment *
imageinfo_undo_grab( Undobuffer *undo, Rect *dirty )
{
	Imageinfo *imageinfo = undo->imageinfo;
	Undofragment *frag = imageinfo_undofragment_new( undo );
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 
	Rect bbox;

	/* Try to extract from im. Memory allocation happens at this
	 * point, so we must be careful!
	 */
	if( !(frag->im = imageinfo_undo_grab_area( im, dirty )) ) {
		imageinfo_undofragment_free( frag );
		error_vips_all();
		return( NULL );
	}

	/* Note position of this frag.
	 */
	frag->pos = *dirty;

	/* Add frag to frag list on undo buffer.
	 */
	undo->frags = g_slist_prepend( undo->frags, frag );

	/* Find bounding box for saved pixels.
	 */
	im_rect_unionrect( dirty, &undo->bbox, &bbox );
	undo->bbox = bbox;

	/* Return new frag.
	 */
	return( frag );
}

/* Trim the undo buffer if we have more than x items on it.
 */
static void
imageinfo_undo_trim( Imageinfo *imageinfo )
{
	int max = PAINTBOX_MAX_UNDO;
	int len = g_slist_length( imageinfo->undo );

	if( max >= 0 && len > max ) {
		GSList *l;
		int i;

		l = g_slist_reverse( imageinfo->undo );

		for( i = 0; i < len - max; i++ ) {
			Undobuffer *undo = (Undobuffer *) l->data;

			imageinfo_undobuffer_free( undo );
			l = g_slist_remove( l, undo );
		}

		imageinfo->undo = g_slist_reverse( l );
	}

#ifdef DEBUG
	printf( "imageinfo_undo_trim: %d items in undo buffer\n", 
		g_slist_length( imageinfo->undo ) );
#endif /*DEBUG*/
}

/* Mark the start or end of an undo session. Copy current undo information 
 * to the undo buffers and NULL out the current undo pointer. Junk all redo
 * information: this new undo action makes all that out of date.
 */
void
imageinfo_undo_mark( Imageinfo *imageinfo )
{
	/* Is there an existing undo save area?
	 */
	if( imageinfo->cundo ) {
		/* Left over from the last undo save. Copy to undo save list
		 * and get ready for new undo buffer.
		 */
		imageinfo->undo = 
			g_slist_prepend( imageinfo->undo, imageinfo->cundo );
		imageinfo->cundo = NULL;
	}

	/* Junk all redo information. 
	 */
	slist_map( imageinfo->redo, 
		(SListMapFn) imageinfo_undobuffer_free, NULL );
	IM_FREEF( g_slist_free, imageinfo->redo );

	/* Trim undo buffer.
	 */
	imageinfo_undo_trim( imageinfo );

	/* Update menus.
	 */
	imageinfo_undo_changed( imageinfo );
}

/* Add to the undo buffer. If there is no undo buffer currently under
 * construction, make a new one. If there is an existing undo buffer, try to
 * grow it left/right/up/down so as to just enclose the new bounding box. We
 * assume that our dirty areas are not going to be disconnected. Is this
 * always true? No - if you move smudge or smear quickly, you can get
 * non-overlapping areas. However: if you do lots of little operations in more
 * or less the same place (surely the usual case), then this technique will be
 * far better.
 */
static gboolean
imageinfo_undo_add( Imageinfo *imageinfo, Rect *dirty )
{
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 
	Undobuffer *undo = imageinfo->cundo;
	Rect over, image, clipped;

	/* Undo disabled? Do nothing.
	 */
	if( PAINTBOX_MAX_UNDO == 0 )
		return( TRUE );

	/* Clip dirty against image size. 
	 */
	image.left = 0;
	image.top = 0;
	image.width = im->Xsize;
	image.height = im->Ysize;
	im_rect_intersectrect( &image, dirty, &clipped );

	/* Is there anything left? If not, can return immediately.
	 */
	if( im_rect_isempty( &clipped ) )
		return( TRUE );

	if( !undo ) {
		/* No current undo buffer ... start a new one for this action.
		 */
		if( !(imageinfo->cundo = undo = 
			imageinfo_undobuffer_new( imageinfo )) )
			return( FALSE );

		return( imageinfo_undo_grab( undo, &clipped ) != NULL );
	}

	/* Existing stuff we are to add to. Try to expand our undo
	 * area to just enclose the new bounding box. We assume that
	 * there is an overlap between the new and old stuff.
	 */

	/* Do we need to expand our saved area to the right?
	 */
	if( IM_RECT_RIGHT( &clipped ) > IM_RECT_RIGHT( &undo->bbox ) ) {
		/* Expand to the right. Calculate the section we need
		 * to add to our bounding box.
		 */
		over.left = IM_RECT_RIGHT( &undo->bbox );
		over.top = undo->bbox.top;
		over.width = IM_RECT_RIGHT( &clipped ) - 
			IM_RECT_RIGHT( &undo->bbox );
		over.height = undo->bbox.height;

		/* Grab new fragment.
		 */
		if( !imageinfo_undo_grab( undo, &over ) )
			return( FALSE );
	}

	/* Do we need to expand our saved area to the left?
	 */
	if( undo->bbox.left > clipped.left ) {
		over.left = clipped.left;
		over.top = undo->bbox.top;
		over.width = undo->bbox.left - clipped.left;
		over.height = undo->bbox.height;

		if( !imageinfo_undo_grab( undo, &over ) )
			return( FALSE );
	}

	/* Do we need to expand our saved area upwards?
	 */
	if( undo->bbox.top > clipped.top ) {
		over.left = undo->bbox.left;
		over.top = clipped.top;
		over.width = undo->bbox.width;
		over.height = undo->bbox.top - clipped.top;

		if( !imageinfo_undo_grab( undo, &over ) )
			return( FALSE );
	}

	/* Do we need to expand our saved area downwards?
	 */
	if( IM_RECT_BOTTOM( &clipped ) > IM_RECT_BOTTOM( &undo->bbox ) ) {
		over.left = undo->bbox.left;
		over.top = IM_RECT_BOTTOM( &undo->bbox );
		over.width = undo->bbox.width;
		over.height = IM_RECT_BOTTOM( &clipped ) - 
			IM_RECT_BOTTOM( &undo->bbox );

		if( !imageinfo_undo_grab( undo, &over ) )
			return( FALSE );
	}

	return( TRUE );
}

/* Paste an undo fragment back into the image.
 */
static void *
imageinfo_undofragment_paste( Undofragment *frag )
{
	Undobuffer *undo = frag->undo;
	Imageinfo *imageinfo = undo->imageinfo;
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 

	im_insertplace( im, frag->im, frag->pos.left, frag->pos.top );
	imageinfo_area_painted( imageinfo, &frag->pos );

	return( NULL );
}

/* Paste a whole undo buffer back into the image.
 */
static void
imageinfo_undobuffer_paste( Undobuffer *undo )
{
	slist_map( undo->frags, 
		(SListMapFn) imageinfo_undofragment_paste, NULL );
}

/* Undo a paint action.
 */
gboolean
imageinfo_undo( Imageinfo *imageinfo )
{
	Undobuffer *undo;

	/* Find the undo action we are to perform.
	 */
	if( !imageinfo->undo )
		return( TRUE );
	undo = (Undobuffer *) imageinfo->undo->data;

	/* We are going to undo the first action on the undo list. We must
	 * save the area under the first undo action to the redo list. Do
	 * the save, even if undo is disabled.
	 */
	if( !imageinfo_undo_add( imageinfo, &undo->bbox ) ) 
		return( FALSE );

	/* Add new undo area.
	 */
	imageinfo->redo = g_slist_prepend( imageinfo->redo, imageinfo->cundo );
	imageinfo->cundo = NULL;

	/* Paint undo back.
	 */
	imageinfo_undobuffer_paste( undo );

	/* Junk the undo action we have performed.
	 */
	imageinfo->undo = g_slist_remove( imageinfo->undo, undo );
	imageinfo_undobuffer_free( undo );

	/* Trim undo buffer.
	 */
	imageinfo_undo_trim( imageinfo );

	/* Update menus.
	 */
	imageinfo_undo_changed( imageinfo );

	return( TRUE );
}

/* Redo a paint action, if possible.
 */
gboolean
imageinfo_redo( Imageinfo *imageinfo )
{
	Undobuffer *undo;

	/* Find the redo action we are to perform.
	 */
	if( !imageinfo->redo )
		return( TRUE );
	undo = (Undobuffer *) imageinfo->redo->data;

	/* We are going to redo the first action on the redo list. We must
	 * save the area under the first redo action to the undo list. Save
	 * even if undo is disabled.
	 */
	if( !imageinfo_undo_add( imageinfo, &undo->bbox ) ) 
		return( FALSE );

	/* Add this new buffer to the undo list.
	 */
	imageinfo->undo = g_slist_prepend( imageinfo->undo, imageinfo->cundo );
	imageinfo->cundo = NULL;

	/* Paint redo back.
	 */
	imageinfo_undobuffer_paste( undo );

	/* We can junk the head of the undo list now.
	 */
	imageinfo->redo = g_slist_remove( imageinfo->redo, undo );
	imageinfo_undobuffer_free( undo );

	/* Trim undo buffer.
	 */
	imageinfo_undo_trim( imageinfo );

	/* Update menus.
	 */
	imageinfo_undo_changed( imageinfo );

	return( TRUE );
}

void
imageinfo_undo_clear( Imageinfo *imageinfo )
{
	imageinfo_undo_free( imageinfo );
	imageinfo_undo_changed( imageinfo );
}

/* Draw a line.
 */
gboolean
imageinfo_paint_line( Imageinfo *imageinfo, Imageinfo *ink, 
	int nib, int x1, int y1, int x2, int y2 )
{
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 
	IMAGE *ink_im = imageinfo_get( FALSE, ink );
	PEL *data = (PEL *) ink_im->data;
	Rect dirty, p1, p2, image, clipped;

	p1 = imageinfo_brush_shapes[nib];
	p1.left += x1;
	p1.top += y1;
	p2 = imageinfo_brush_shapes[nib];
	p2.left += x2;
	p2.top += y2;
	im_rect_unionrect( &p1, &p2, &dirty );

	image.left = 0;
	image.top = 0;
	image.width = im->Xsize;
	image.height = im->Ysize;
	im_rect_intersectrect( &dirty, &image, &clipped );

	if( im_rect_isempty( &clipped ) )
		return( TRUE );

	if( !imageinfo_undo_add( imageinfo, &clipped ) ) 
		return( FALSE );

	if( im_fastlineuser( im, x1, y1, x2, y2, im_plotmask, data, 
		imageinfo_brush_masks[nib], &imageinfo_brush_shapes[nib] ) ) {
		error_vips_all();
		return( FALSE );
	}

	imageinfo_area_painted( imageinfo, &dirty );

	return( TRUE );
}

/* Smudge a line.
 */
gboolean
imageinfo_paint_smudge( Imageinfo *imageinfo, 
	Rect *oper, int x1, int y1, int x2, int y2 )
{	
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 
	Rect p1, p2, dirty;

	/* Calculate bounding box for smudge.
	 */
	p1 = *oper;
	p1.left += x1;
	p1.top += y1;
	p2 = *oper;
	p2.left += x2;
	p2.top += y2;
	im_rect_unionrect( &p1, &p2, &dirty );
	if( !imageinfo_undo_add( imageinfo, &dirty ) )
		return( FALSE );

	/* Smudge line connecting old and new points. 
	 */
	if( im_fastlineuser( im, x1, y1, x2, y2, im_smudge, 
		oper, NULL, NULL ) ) {
		error_vips_all();
		return( FALSE );
	}

	imageinfo_area_painted( imageinfo, &dirty );

	return( TRUE );
}

/* Flood an area.
 */
gboolean
imageinfo_paint_flood( Imageinfo *imageinfo, Imageinfo *ink, 
	int x, int y, gboolean blob )
{
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 
	IMAGE *ink_im = imageinfo_get( FALSE, ink );
	PEL *data = (PEL *) ink_im->data;
	Rect dirty;
	int result;

	/* Save undo area. We have to save the entire image, as we don't know
	 * how much the flood will change :(
	 */
	dirty.left = 0;
	dirty.top = 0;
	dirty.width = im->Xsize;
	dirty.height = im->Ysize;
	if( !imageinfo_undo_add( imageinfo, &dirty ) ) 
		return( FALSE );

	/* Flood!
	 */
	if( blob )
		result = im_flood_blob( im, x, y, data, &dirty );
	else
		result = im_flood( im, x, y, data, &dirty );
	if( result ) {
		error_vips_all();
		return( FALSE );
	}

	imageinfo_area_painted( imageinfo, &dirty );

	return( TRUE );
}

gboolean
imageinfo_paint_dropper( Imageinfo *imageinfo, Imageinfo *ink, int x, int y )
{
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 
	IMAGE *ink_im = imageinfo_get( FALSE, ink );
	PEL *data = (PEL *) ink_im->data;
	Rect dirty;

	if( im_readpoint( im, x, y, data ) ) {
		error_vips_all();
		return( FALSE );
	}
	im_invalidate( ink_im );

	dirty.left = 0;
	dirty.top = 0;
	dirty.width = ink_im->Xsize;
	dirty.height = ink_im->Ysize;

	imageinfo_area_painted( ink, &dirty );

	return( TRUE );
}

/* Fill a rect.
 */
gboolean
imageinfo_paint_rect( Imageinfo *imageinfo, Imageinfo *ink, Rect *area )
{	
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 
	IMAGE *ink_im = imageinfo_get( FALSE, ink );
	PEL *data = (PEL *) ink_im->data;

	if( !imageinfo_undo_add( imageinfo, area ) )
		return( FALSE );

	if( im_paintrect( im, area, data ) ) {
		error_vips_all();
		return( FALSE );
	}

	imageinfo_area_painted( imageinfo, area );

	return( TRUE );
}

/* Paint text into imageinfo, return width/height in tarea.
 */
gboolean
imageinfo_paint_text( Imageinfo *imageinfo, 
	const char *font_name, const char *text, Rect *tarea )
{
	IMAGE *im = imageinfo_get( FALSE, imageinfo );

	if( im_text( im, text, font_name, 0, 0, get_dpi() ) ) {
		error_top( _( "Unable to paint text." ) );
		error_sub( _( "Unable to paint text \"%s\" in font \"%s\"." ), 
			text, font_name );
		error_vips();

		return( FALSE );
	}

	tarea->left = 0;
	tarea->top = 0;
	tarea->width = im->Xsize;
	tarea->height = im->Ysize;

	return( TRUE );
}

/* Paint a mask.
 */
gboolean
imageinfo_paint_mask( Imageinfo *imageinfo, 
	Imageinfo *ink, Imageinfo *mask, int x, int y )
{
	IMAGE *im = imageinfo_get( FALSE, imageinfo ); 
	IMAGE *ink_im = imageinfo_get( FALSE, ink );
	IMAGE *mask_im = imageinfo_get( FALSE, mask );
	Rect dirty, image, clipped;

	dirty.left = x;
	dirty.top = y;
	dirty.width = mask_im->Xsize;
	dirty.height = mask_im->Ysize;
	image.left = 0;
	image.top = 0;
	image.width = im->Xsize;
	image.height = im->Ysize;
	im_rect_intersectrect( &dirty, &image, &clipped );

	if( im_rect_isempty( &clipped ) )
		return( TRUE );

	if( !imageinfo_undo_add( imageinfo, &clipped ) ) 
		return( FALSE );

	if( im_plotmask( im, 0, 0, 
		(PEL *) ink_im->data, (PEL *) mask_im->data, &dirty ) ) {
		error_vips_all();
		return( FALSE );
	}

	imageinfo_area_painted( imageinfo, &dirty );

	return( TRUE );
}

/* Print a pixel. Output has to be parseable by imageinfo_from_text().
 */
void 
imageinfo_to_text( Imageinfo *imageinfo, VipsBuf *buf )
{
	IMAGE *im = imageinfo_get( FALSE, imageinfo );
	PEL *p = (PEL *) im->data;
	int i;

#define PRINT_INT( T, I ) vips_buf_appendf( buf, "%d", ((T *)p)[I] );
#define PRINT_FLOAT( T, I ) vips_buf_appendg( buf, ((T *)p)[I] );

	for( i = 0; i < im->Bands; i++ ) {
		if( i )
			vips_buf_appends( buf, ", " );

		switch( im->BandFmt ) {
		case IM_BANDFMT_UCHAR:
			PRINT_INT( unsigned char, i );
			break;
			
		case IM_BANDFMT_CHAR:
			PRINT_INT( char, i );
			break;
			
		case IM_BANDFMT_USHORT:
			PRINT_INT( unsigned short, i );
			break;
			
		case IM_BANDFMT_SHORT:
			PRINT_INT( short, i );
			break;
			
		case IM_BANDFMT_UINT:
			PRINT_INT( unsigned int, i );
			break;
			
		case IM_BANDFMT_INT:
			PRINT_INT( int, i );
			break;
			
		case IM_BANDFMT_FLOAT:
			PRINT_FLOAT( float, i );
			break;
			
		case IM_BANDFMT_COMPLEX:
			vips_buf_appends( buf, "(" );
			PRINT_FLOAT( float, (i << 1) );
			vips_buf_appends( buf, ", " );
			PRINT_FLOAT( float, (i << 1) + 1 );
			vips_buf_appends( buf, ")" );
			break;
			
		case IM_BANDFMT_DOUBLE:
			PRINT_FLOAT( double, i );
			break;
			
		case IM_BANDFMT_DPCOMPLEX:
			vips_buf_appends( buf, "(" );
			PRINT_FLOAT( double, i << 1 );
			vips_buf_appends( buf, ", " );
			PRINT_FLOAT( double, (i << 1) + 1 );
			vips_buf_appends( buf, ")" );
			break;

		default:
			vips_buf_appends( buf, "???" );
			break;
		}
	}
}

/* Set band i to value.
 */
static void
imageinfo_from_text_band( Imageinfo *imageinfo, int i, double re, double im )
{
	IMAGE *image = imageinfo_get( FALSE, imageinfo );
	PEL *p = (PEL *) image->data;
	double mod = sqrt( re*re + im*im );

	if( i < 0 || i >= image->Bands )
		return;

#define SET_INT( T, I, X ) (((T *)p)[I] = (T) IM_RINT(X)) 
#define SET_FLOAT( T, I, X ) (((T *)p)[I] = (T) (X)) 

	switch( image->BandFmt ) {
	case IM_BANDFMT_UCHAR:
		SET_INT( unsigned char, i, mod );
		break;
		
	case IM_BANDFMT_CHAR:
		SET_INT( char, i, mod );
		break;
		
	case IM_BANDFMT_USHORT:
		SET_INT( unsigned short, i, mod );
		break;
		
	case IM_BANDFMT_SHORT:
		SET_INT( short, i, mod );
		break;
		
	case IM_BANDFMT_UINT:
		SET_INT( unsigned int, i, mod );
		break;
		
	case IM_BANDFMT_INT:
		SET_INT( int, i, mod );
		break;
		
	case IM_BANDFMT_FLOAT:
		SET_FLOAT( float, i, mod );
		break;
		
	case IM_BANDFMT_COMPLEX:
		SET_FLOAT( float, (i << 1), re );
		SET_FLOAT( float, (i << 1) + 1, im );
		break;
		
	case IM_BANDFMT_DOUBLE:
		SET_FLOAT( double, i, mod );
		break;
		
	case IM_BANDFMT_DPCOMPLEX:
		SET_FLOAT( double, i << 1, re );
		SET_FLOAT( double, (i << 1) + 1, im );
		break;

	default:
		break;
	}
}

/* Parse a string to an imageinfo.
 * Strings are from imageinfo_to_text(), ie. of the form:
 *
 *	50, 0, 0
 *	(12,13), (14,15)
 *
 */
gboolean
imageinfo_from_text( Imageinfo *imageinfo, const char *text )
{
	char buf[MAX_LINELENGTH];
	char *p;
	int i;
	Rect dirty;

#ifdef DEBUG_RGB
	printf( "imageinfo_from_text: in: \"\%s\"\n", text );
#endif /*DEBUG_RGB*/

	im_strncpy( buf, text, MAX_LINELENGTH );

	for( i = 0, p = buf; p += strspn( p, WHITESPACE ), *p; i++ ) {
		double re, im;

		if( p[0] == '(' ) {
			/* Complex constant.
			 */
			re = g_ascii_strtod( p + 1, NULL );
			p = break_token( p, "," );
			im = g_ascii_strtod( p, NULL );
			p = break_token( p, ")" );
		}
		else {
			/* Real constant.
			 */
			re = g_ascii_strtod( p, NULL );
			im = 0;
		}

		p = break_token( p, "," );

		imageinfo_from_text_band( imageinfo, i, re, im );
	}

#ifdef DEBUG_RGB
{
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	printf( "imageinfo_from_text: out: " );
	imageinfo_to_text( imageinfo, &buf );
	printf( "%s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG_RGB*/

	dirty.left = 0;
	dirty.top = 0;
	dirty.width = 1;
	dirty.height = 1;
	imageinfo_area_painted( imageinfo, &dirty );

	return( TRUE );
}

/* Get the image as display RGB in rgb[0-2].
 */
void
imageinfo_to_rgb( Imageinfo *imageinfo, double *rgb )
{
	Conversion *conv;
	Rect area;
	PEL *p;
	int i;

#ifdef DEBUG_RGB
{
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	printf( "imageinfo_to_rgb: in: " );
	imageinfo_to_text( imageinfo, &buf );
	printf( "%s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG_RGB*/

	/* Make a temporary conv ... we hold the ref.
	 */
	conv = conversion_new( NULL );
	conversion_set_synchronous( conv, TRUE );
	conversion_set_image( conv, imageinfo );
	g_object_ref( G_OBJECT( conv ) );
	iobject_sink( IOBJECT( conv ) );

	area.left = 0;
	area.top = 0;
	area.width = 1;
	area.height = 1;

	if( im_prepare( conv->ireg, &area ) ) {
		UNREF( conv );
		return;
	}
        p = (PEL *) IM_REGION_ADDR( conv->ireg, area.left, area.top );

	if( imageinfo->im->Bands < 3 ) 
		for( i = 0; i < 3; i++ )
			rgb[i] = p[0] / 255.0;
	else 
		for( i = 0; i < 3; i++ )
			rgb[i] = p[i] / 255.0;

#ifdef DEBUG_RGB
	printf( "imageinfo_to_rgb: out: r = %g, g = %g, b = %g\n", 
		rgb[0], rgb[1], rgb[2] );
#endif /*DEBUG_RGB*/

	UNREF( conv );
}

/* Try to overwrite an imageinfo with a display RGB colour.
 */
void
imageinfo_from_rgb( Imageinfo *imageinfo, double *rgb )
{
	Imageinfogroup *imageinfogroup = 
		IMAGEINFOGROUP( ICONTAINER( imageinfo )->parent );
	IMAGE *im = imageinfo_get( FALSE, imageinfo );
	Imageinfo *in, *out;
	IMAGE *t1, *t2;
	int i;
	Rect dirty;

	/* Interchange format is sRGB.

		FIXME ... should let other displays be used here, see
		../scraps/calibrate.[hc]

	 */
	struct im_col_display *display = im_col_displays( 7 );

#ifdef DEBUG_RGB
	printf( "imageinfo_from_rgb: in: r = %g, g = %g, b = %g\n", 
		rgb[0], rgb[1], rgb[2] );
#endif /*DEBUG_RGB*/

	/* Make 1 pixel images for conversion.
	 */
	in = imageinfo_new_temp( imageinfogroup, 
		reduce_context->heap, NULL, "t" );
	out = imageinfo_new_temp( imageinfogroup, 
		reduce_context->heap, NULL, "t" );
	if( !in || !out )
		return;
	if( !(t1 = im_open_local( out->im, "imageinfo_from_rgb:1", "t" )) ||
		!(t2 = im_open_local( out->im, "imageinfo_from_rgb:1", "t" )) )
		return;

	/* Fill in with rgb.
	 */
	im_initdesc( in->im, 1, 1, 3, 
		IM_BBITS_BYTE, IM_BANDFMT_UCHAR, IM_CODING_NONE, 
		IM_TYPE_sRGB, 1.0, 1.0, 0, 0 );
	if( im_setupout( in->im ) ) 
		return;
	for( i = 0; i < 3; i++ )
		((PEL *) in->im->data)[i] = IM_RINT( rgb[i] * 255.0 );

	/* To imageinfo->type. Make sure we get a float ... except for LABQ
	 * and RAD.
	 */
	if( im->Coding == IM_CODING_LABQ ) {
		if( im_disp2Lab( in->im, t1, display ) ||
			im_Lab2LabQ( t1, out->im ) )
			return;
	}
	else if( im->Coding == IM_CODING_RAD ) {
		if( im_disp2XYZ( in->im, t1, display ) ||
			im_float2rad( t1, out->im ) )
			return;
	}
	else if( im->Coding == IM_CODING_NONE ) {
		switch( im->Type ) {
		case IM_TYPE_XYZ:
			if( im_disp2XYZ( in->im, out->im, display ) )
				return;
			break;

		case IM_TYPE_YXY:
			if( im_disp2XYZ( in->im, t1, display ) ||
				im_XYZ2Yxy( t1, out->im ) )
				return;
			break;

		case IM_TYPE_LAB:
			if( im_disp2Lab( in->im, out->im, display ) )
				return;
			break;

		case IM_TYPE_LCH:
			if( im_disp2Lab( in->im, t1, display ) ||
				im_Lab2LCh( t1, out->im ) )
				return;
			break;

		case IM_TYPE_UCS:
			if( im_disp2Lab( in->im, t1, display ) ||
				im_Lab2LCh( t1, t2 ) ||
				im_LCh2UCS( t2, out->im ) )
				return;
			break;

		case IM_TYPE_RGB16:
		case IM_TYPE_GREY16:
			if( im_lintra( 1.0 / 256.0, in->im, 0.0, out->im ) )
				return;
			break;

		case IM_TYPE_RGB:
		case IM_TYPE_sRGB:
		default:
			if( im_clip2fmt( in->im, out->im, IM_BANDFMT_FLOAT ) )
				return;
			break;
		}
	}

#define SET( TYPE, i ) ((TYPE *) im->data)[i] = ((float *) out->im->data)[i];

	/* Now ... overwrite imageinfo.
	 */
	if( im->Coding == IM_CODING_LABQ ||
		im->Coding == IM_CODING_RAD ) {
		for( i = 0; i < im->Bands; i++ ) 
			((PEL *) im->data)[i] = ((PEL *) out->im->data)[i];
	}
	else {
		for( i = 0; i < im->Bands; i++ )
			switch( im->BandFmt ) {
			case IM_BANDFMT_UCHAR:          
				SET( unsigned char, i ); 
				break;

			case IM_BANDFMT_CHAR:           
				SET( signed char, i ); 
				break;

			case IM_BANDFMT_USHORT:         
				SET( unsigned short, i ); 
				break;

			case IM_BANDFMT_SHORT:          
				SET( signed short, i ); 
				break;

			case IM_BANDFMT_UINT:           
				SET( unsigned int, i ); 
				break;

			case IM_BANDFMT_INT:            
				SET( signed int, i );  
				break;

			case IM_BANDFMT_FLOAT:          
				SET( float, i ); 
				break;

			case IM_BANDFMT_DOUBLE:         
				SET( double, i ); 
				break;

			case IM_BANDFMT_COMPLEX:        
				SET( float, i * 2 ); 
				SET( float, i * 2 + 1 ); 
				break;

			case IM_BANDFMT_DPCOMPLEX:      
				SET( double, i * 2 ); 
				SET( double, i * 2 + 1 ); 
				break;

			default:
				g_assert( FALSE );
			}
	}
	im_invalidate( im );

#ifdef DEBUG_RGB
{
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	printf( "imageinfo_from_rgb: out: " );
	imageinfo_to_text( imageinfo, &buf );
	printf( "%s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG_RGB*/

	dirty.left = 0;
	dirty.top = 0;
	dirty.width = 1;
	dirty.height = 1;
	imageinfo_area_painted( imageinfo, &dirty );
}

/* Widgets for colour edit.
 */
typedef struct _ColourEdit {
	iDialog *idlg;

	Imageinfo *imageinfo;
	GtkWidget *colour_widget;
} ColourEdit;

/* Done button hit.
 */
static void
imageinfo_colour_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	ColourEdit *eds = (ColourEdit *) client;
	Imageinfo *imageinfo = eds->imageinfo;
	double rgb[4];

	gtk_color_selection_get_color( 
		GTK_COLOR_SELECTION( eds->colour_widget ), rgb );

	/* This will emit "area_painted" on our imageinfo.
	 */
	imageinfo_from_rgb( imageinfo, rgb );

	nfn( sys, IWINDOW_YES );
}

/* Build the insides of colour edit.
 */
static void
imageinfo_colour_buildedit( iDialog *idlg, GtkWidget *work, ColourEdit *eds )
{
	Imageinfo *imageinfo = eds->imageinfo;
	double rgb[4];

	eds->colour_widget = gtk_color_selection_new();
	gtk_color_selection_set_has_opacity_control( 
		GTK_COLOR_SELECTION( eds->colour_widget ), FALSE );
	imageinfo_to_rgb( imageinfo, rgb );
	gtk_color_selection_set_color( 
		GTK_COLOR_SELECTION( eds->colour_widget ), rgb );
        gtk_box_pack_start( GTK_BOX( work ), 
		eds->colour_widget, TRUE, TRUE, 2 );

        gtk_widget_show_all( work );
}

void
imageinfo_colour_edit( GtkWidget *parent, Imageinfo *imageinfo )
{
	ColourEdit *eds = INEW( NULL, ColourEdit );
	GtkWidget *idlg;

	eds->imageinfo = imageinfo;

	idlg = idialog_new();
	iwindow_set_title( IWINDOW( idlg ), "Edit Colour" );
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) imageinfo_colour_buildedit, eds, NULL, NULL );
	idialog_set_callbacks( IDIALOG( idlg ), 
		iwindow_true_cb, NULL, idialog_free_client, eds );
	idialog_add_ok( IDIALOG( idlg ), 
		imageinfo_colour_done_cb, "Set Colour" );
	iwindow_set_parent( IWINDOW( idlg ), parent );
	idialog_set_iobject( IDIALOG( idlg ), IOBJECT( imageinfo ) );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );
}
