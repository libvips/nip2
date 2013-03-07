/* Watch stuff in the prefs workspace.
 */

/*

    Copyright (C) 1991-2003 The National Gallery

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your watch) any later version.

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

static iContainerClass *watchgroup_parent_class = NULL;

/* Our signals. 
 */
enum {
	SIG_WATCH_CHANGED,	/* "changed" on one of our watches */
	SIG_LAST
};

static guint watchgroup_signals[SIG_LAST] = { 0 };

static void 
watchgroup_changed( Watchgroup *watchgroup, Watch *watch )
{
	g_signal_emit( G_OBJECT( watchgroup ), 
		watchgroup_signals[SIG_WATCH_CHANGED], 0, watch );
}

static void
watchgroup_class_init( WatchgroupClass *class )
{
	watchgroup_parent_class = g_type_class_peek_parent( class );

	watchgroup_signals[SIG_WATCH_CHANGED] = g_signal_new( "watch_changed",
		G_OBJECT_CLASS_TYPE( class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( WatchgroupClass, watch_changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__POINTER,
		G_TYPE_NONE, 1,
		G_TYPE_POINTER );
}

static void
watchgroup_init( Watchgroup *watchgroup )
{
#ifdef DEBUG
	printf( "watchgroup_init\n" );
#endif /*DEBUG*/

	watchgroup->auto_save_timeout = 0;
}

GType
watchgroup_get_type( void )
{
	static GType watchgroup_type = 0;

	if( !watchgroup_type ) {
		static const GTypeInfo info = {
			sizeof( WatchgroupClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) watchgroup_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Watchgroup ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) watchgroup_init,
		};

		watchgroup_type = g_type_register_static( TYPE_ICONTAINER, 
			"Watchgroup", &info, 0 );
	}

	return( watchgroup_type );
}

Watchgroup *
watchgroup_new( Workspacegroup *workspacegroup, const char *name )
{
	Watchgroup *watchgroup = WATCHGROUP( 
		g_object_new( TYPE_WATCHGROUP, NULL ) );

	/* Assume it's a static string.
	 */
	watchgroup->name = name;

	watchgroup->workspacegroup = workspacegroup;
	icontainer_set_hash( ICONTAINER( watchgroup ) );

	return( watchgroup );
}

/* Get the ws we are storing prefs in, and check it looks OK.
 */
static Workspace *
watchgroup_get_workspace( Watchgroup *watchgroup )
{
	Compile *compile;
	Symbol *sym;

	if( !watchgroup->workspacegroup->sym )
		return( NULL );

	compile = watchgroup->workspacegroup->sym->expr->compile;

	if( !(sym = compile_lookup( compile, watchgroup->name )) ||
		!sym->expr->compile ||
		sym->type != SYM_WORKSPACE ||
		!sym->ws )
		return( NULL );

	return( sym->ws );
}

static void
watchgroup_save( Watchgroup *watchgroup )
{
	Workspace *ws;

	if( (ws = watchgroup_get_workspace( watchgroup )) ) {
		Filemodel *filemodel = FILEMODEL( ws );

		if( filemodel->modified ) {
			symbol_recalculate_all();

			/* Ignore error returns ... hmm! Tricky: we can come
			 * here during shutdown.
			 */
			(void) filemodel_save_all( filemodel, 
				filemodel->filename );

			filemodel_set_modified( filemodel, FALSE );
		}
	}
}

static gboolean
watchgroup_dirty_timeout_cb( Watchgroup *watchgroup )
{
	watchgroup->auto_save_timeout = 0;

	watchgroup_save( watchgroup );

	return( FALSE );
}

void
watchgroup_dirty( Watchgroup *watchgroup )
{
	Workspace *ws;

	/* Find the preferences workspace.
	 */
	if( (ws = watchgroup_get_workspace( watchgroup )) ) {
		Filemodel *filemodel = FILEMODEL( ws );

		/* Mark ws dirty, start save timer.
		 */
		filemodel_set_modified( filemodel, TRUE );

		IM_FREEF( g_source_remove, watchgroup->auto_save_timeout );
		watchgroup->auto_save_timeout = g_timeout_add( 1000, 
			(GSourceFunc) watchgroup_dirty_timeout_cb, watchgroup );
	}
}

void
watchgroup_flush( Watchgroup *watchgroup )
{
	/* Do we have a pending save?
	 */
	if( watchgroup->auto_save_timeout ) {
		watchgroup_save( watchgroup );

		IM_FREEF( g_source_remove, watchgroup->auto_save_timeout );
	}
}

static iContainerClass *watch_parent_class = NULL;
static GSList *watch_all = NULL;

static void
watch_finalize( GObject *gobject )
{
	Watch *watch;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WATCH( gobject ) );

	watch = WATCH( gobject );

#ifdef DEBUG
	printf( "watch_finalize: %s\n", NN( IOBJECT( watch )->name ) );
#endif /*DEBUG*/

	watch_all = g_slist_remove( watch_all, watch );

	G_OBJECT_CLASS( watch_parent_class )->finalize( gobject );
}

static void
watch_dispose( GObject *gobject )
{
	Watch *watch;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WATCH( gobject ) );

	watch = WATCH( gobject );

#ifdef DEBUG
	printf( "watch_dispose: %s\n", NN( IOBJECT( watch )->name ) );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	FREESID( watch->destroy_sid, watch->row );
	FREESID( watch->changed_sid, watch->row );
	watch->row = NULL;

	G_OBJECT_CLASS( watch_parent_class )->dispose( gobject );
}

static void
watch_changed( iObject *iobject )
{
	Watch *watch = WATCH( iobject );
	Watchgroup *watchgroup = WATCHGROUP( ICONTAINER( watch )->parent );

	/* Emit on our group too. Can get here before our parent is linked on,
	 * careful.
	 */
	if( watchgroup )
		watchgroup_changed( WATCHGROUP( ICONTAINER( watch )->parent ), 
			watch );

	IOBJECT_CLASS( watch_parent_class )->changed( iobject );
}

static void
watch_class_init( WatchClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	WatchClass *watch_class = (WatchClass *) class;

	watch_parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = watch_finalize;
	gobject_class->dispose = watch_dispose;

	iobject_class->changed = watch_changed;

	watch_class->update = NULL;
	watch_class->get_value = NULL;
}

static void
watch_init( Watch *watch )
{
	watch->row = NULL;
	watch->ok = FALSE;
	watch->destroy_sid = 0;
	watch->changed_sid = 0;

	watch_all = g_slist_prepend( watch_all, watch );
}

GType
watch_get_type( void )
{
	static GType watch_type = 0;

	if( !watch_type ) {
		static const GTypeInfo info = {
			sizeof( WatchClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) watch_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Watch ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) watch_init,
		};

		watch_type = g_type_register_static( TYPE_ICONTAINER, 
			"Watch", &info, 0 );
	}

	return( watch_type );
}

static void
watch_link( Watch *watch, Watchgroup *watchgroup, const char *name )
{
	iobject_set( IOBJECT( watch ), name, NULL );
	icontainer_child_add( ICONTAINER( watchgroup ), 
		ICONTAINER( watch ), -1 );
}

static void
watch_destroy_cb( Row *row, Watch *watch )
{
#ifdef DEBUG
	printf( "watch_destroy_cb\n" );
#endif /*DEBUG*/

	watch->row = NULL;
	watch->ok = FALSE;
	watch->destroy_sid = 0;
	watch->changed_sid = 0;
}

/* The row we are watching has changed.
 */
static void
watch_changed_cb( Row *row, Watch *watch )
{
#ifdef DEBUG
	printf( "watch_changed_cb: %s\n", NN( IOBJECT( watch )->name ) );
#endif /*DEBUG*/

	if( row->expr )
		watch->ok = WATCH_GET_CLASS( watch )->update( watch );

	iobject_changed( IOBJECT( watch ) );
}

/* Make sure we're linked to the thing we watch.
 */
static void
watch_attach( Watch *watch )
{
	Watchgroup *watchgroup = WATCHGROUP( ICONTAINER( watch )->parent );
	const char *name = IOBJECT( watch )->name;
	Workspace *ws;
	Symbol *sym;

	if( watch->row )
		return;

	if( (ws = watchgroup_get_workspace( watchgroup )) &&
		ws->sym->expr &&
		ws->sym->expr->compile &&
		(sym = compile_lookup( ws->sym->expr->compile, name )) &&
		sym->expr->row ) {
		watch->row = sym->expr->row;
		watch->destroy_sid = 
			g_signal_connect( G_OBJECT( watch->row ), "destroy",
				G_CALLBACK( watch_destroy_cb ), watch );
		watch->changed_sid = 
			g_signal_connect( G_OBJECT( watch->row ), "changed",
				G_CALLBACK( watch_changed_cb ), watch );
	}
}

Watch *
watch_find( Watchgroup *watchgroup, const char *name )
{
	return( (Watch *)
		(icontainer_child_lookup( ICONTAINER( watchgroup ), name )) );
}

static gboolean
watch_get( Watch *watch, void **out )
{
#ifdef DEBUG
	printf( "watch_get: %s\n", NN( IOBJECT( watch )->name ) );
#endif /*DEBUG*/

	watch_attach( watch );

	if( !watch->row )
		return( FALSE );

	if( !watch->ok )
		watch->ok = WATCH_GET_CLASS( watch )->update( watch );

	if( !watch->ok )
		return( FALSE );

	*out = WATCH_GET_CLASS( watch )->get_value( watch );

	return( TRUE );
}

static void *
watch_relink( Watch *watch )
{
	if( !watch->row ) {
		watch_attach( watch );
		if( watch->row ) 
			iobject_changed( IOBJECT( watch ) );
	}

	return( NULL );
}

void
watch_relink_all( void )
{
	slist_map( watch_all, (SListMapFn) watch_relink, NULL );
}

void
watch_vset( Watch *watch, const char *fmt, va_list args )
{
	if( watch->row && watch->row->child_rhs && 
		watch->row->child_rhs->itext ) {
		iText *itext = ITEXT( watch->row->child_rhs->itext );
		char buf[256];

		(void) im_vsnprintf( buf, 256, fmt, args );

		if( itext_set_formula( itext, buf ) ) {
#ifdef DEBUG
			printf( "watch_vset: %s = %s\n", 
				IOBJECT( watch )->name, buf );
#endif /*DEBUG*/

			itext_set_edited( itext, TRUE );
			expr_dirty( watch->row->sym->expr, link_serial_new() );
			watchgroup_dirty( 
				WATCHGROUP( ICONTAINER( watch )->parent ) );
		}
	}
}

void
watch_set( Watch *watch, const char *fmt, ... )
{
	va_list args;

	va_start( args, fmt );
	watch_vset( watch, fmt, args );
	va_end( args );
}

static WatchClass *watch_double_parent_class = NULL;

static gboolean
watch_double_update( Watch *watch )
{
	WatchDouble *watch_double = WATCH_DOUBLE( watch );
	PElement *root = &watch->row->expr->root;

#ifdef DEBUG
	printf( "watch_double_update\n" );
#endif /*DEBUG*/

	if( PEISNOVAL( root ) )
		return( FALSE );
	if( !PEISREAL( root ) ) {
		heap_error_typecheck( root, IOBJECT( watch )->name, "real" );
		return( FALSE );
	}

	watch_double->value = PEGETREAL( root );

	return( TRUE );
}

static void *
watch_double_get_value( Watch *watch )
{
	WatchDouble *watch_double = WATCH_DOUBLE( watch );

	return( (void *) &watch_double->value );
}

static void
watch_double_class_init( WatchDoubleClass *class )
{
	WatchClass *watch_class = (WatchClass *) class;

	watch_double_parent_class = g_type_class_peek_parent( class );

	watch_class->update = watch_double_update;
	watch_class->get_value = watch_double_get_value;
}

static void
watch_double_init( WatchDouble *watch_double )
{
#ifdef DEBUG
	printf( "watch_double_init\n" );
#endif /*DEBUG*/

	watch_double->value = -1.0;
}

GType
watch_double_get_type( void )
{
	static GType watch_double_type = 0;

	if( !watch_double_type ) {
		static const GTypeInfo info = {
			sizeof( WatchDoubleClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) watch_double_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( WatchDouble ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) watch_double_init,
		};

		watch_double_type = g_type_register_static( TYPE_WATCH, 
			"WatchDouble", &info, 0 );
	}

	return( watch_double_type );
}

static Watch *
watch_double_new( Watchgroup *watchgroup, const char *name )
{
	WatchDouble *watch_double = WATCH_DOUBLE( 
		g_object_new( TYPE_WATCH_DOUBLE, NULL ) );

	watch_link( WATCH( watch_double ), watchgroup, name );

	return( WATCH( watch_double ) );
}

double 
watch_double_get( Watchgroup *watchgroup, const char *name, double fallback )
{
	Watch *watch;
	void *value;

	if( !watchgroup )
		return( fallback );

	if( !(watch = watch_find( watchgroup, name )) )
		watch = watch_double_new( watchgroup, name );

	g_assert( IS_WATCH_DOUBLE( watch ) );

	if( !watch_get( watch, &value ) ) 
		return( fallback );

	return( *((double *) value) );
}

static WatchClass *watch_int_parent_class = NULL;

static gboolean
watch_int_update( Watch *watch )
{
	WatchInt *watch_int = WATCH_INT( watch );
	Expr *expr = watch->row->expr;
	PElement *root = &watch->row->expr->root;

#ifdef DEBUG
	printf( "watch_int_update: %s\n", NN( IOBJECT( watch )->name ) );
#endif /*DEBUG*/

	/* Can get called during shutdown :-( main_watchgroup_changed_cb() can
	 * call us before destroying the row, but after killing the expr.
	 */
	if( !expr )
		return( FALSE );
	root = &expr->root;
	if( PEISNOVAL( root ) )
		return( FALSE );
	if( !PEISREAL( root ) ) {
		heap_error_typecheck( root, IOBJECT( watch )->name, "real" );
		return( FALSE );
	}

	watch_int->value = IM_RINT( PEGETREAL( root ) );

	return( TRUE );
}

static void *
watch_int_get_value( Watch *watch )
{
	WatchInt *watch_int = WATCH_INT( watch );

	return( (void *) &watch_int->value );
}

static void
watch_int_class_init( WatchIntClass *class )
{
	WatchClass *watch_class = (WatchClass *) class;

	watch_int_parent_class = g_type_class_peek_parent( class );

	watch_class->update = watch_int_update;
	watch_class->get_value = watch_int_get_value;
}

static void
watch_int_init( WatchInt *watch_int )
{
#ifdef DEBUG
	printf( "watch_int_init\n" );
#endif /*DEBUG*/

	watch_int->value = -1;
}

GType
watch_int_get_type( void )
{
	static GType watch_int_type = 0;

	if( !watch_int_type ) {
		static const GTypeInfo info = {
			sizeof( WatchIntClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) watch_int_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( WatchInt ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) watch_int_init,
		};

		watch_int_type = g_type_register_static( TYPE_WATCH, 
			"WatchInt", &info, 0 );
	}

	return( watch_int_type );
}

static Watch *
watch_int_new( Watchgroup *watchgroup, const char *name )
{
	WatchInt *watch_int = WATCH_INT( g_object_new( TYPE_WATCH_INT, NULL ) );

	watch_link( WATCH( watch_int ), watchgroup, name );

	return( WATCH( watch_int ) );
}

int
watch_int_get( Watchgroup *watchgroup, const char *name, int fallback )
{
	Watch *watch;
	void *value;

	if( !watchgroup || !name )
		return( fallback );

	if( !(watch = watch_find( watchgroup, name )) )
		watch = watch_int_new( watchgroup, name );

	g_assert( IS_WATCH_INT( watch ) );

	if( !watch_get( watch, &value ) ) 
		return( fallback );

	return( *((int *) value) );
}

static WatchClass *watch_path_parent_class = NULL;

static void
watch_path_finalize( GObject *gobject )
{
	WatchPath *watch_path;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WATCH_PATH( gobject ) );

#ifdef DEBUG
	printf( "watch_path_finalize\n" );
#endif /*DEBUG*/

	watch_path = WATCH_PATH( gobject );

	/* My instance destroy stuff.
	 */
	IM_FREEF( slist_free_all, watch_path->value );

	G_OBJECT_CLASS( watch_path_parent_class )->finalize( gobject );
}

static gboolean
watch_path_update( Watch *watch )
{
	WatchPath *watch_path = WATCH_PATH( watch );
	PElement *root = &watch->row->expr->root;
	GSList *value;

#ifdef DEBUG
	printf( "watch_path_update\n" );
#endif /*DEBUG*/

	if( !heap_get_lstring( root, &value ) )
		return( FALSE );

	IM_FREEF( slist_free_all, watch_path->value );
	watch_path->value = value;

	return( TRUE );
}

static void *
watch_path_get_value( Watch *watch )
{
	WatchPath *watch_path = WATCH_PATH( watch );

	return( (void *) &watch_path->value );
}

static void
watch_path_class_init( WatchPathClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	WatchClass *watch_class = (WatchClass *) class;

	watch_path_parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = watch_path_finalize;

	watch_class->update = watch_path_update;
	watch_class->get_value = watch_path_get_value;
}

static void
watch_path_init( WatchPath *watch_path )
{
#ifdef DEBUG
	printf( "watch_path_init\n" );
#endif /*DEBUG*/

	watch_path->value = NULL;
}

GType
watch_path_get_type( void )
{
	static GType watch_path_type = 0;

	if( !watch_path_type ) {
		static const GTypeInfo info = {
			sizeof( WatchPathClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) watch_path_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( WatchPath ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) watch_path_init,
		};

		watch_path_type = g_type_register_static( TYPE_WATCH, 
			"WatchPath", &info, 0 );
	}

	return( watch_path_type );
}

static Watch *
watch_path_new( Watchgroup *watchgroup, const char *name )
{
	WatchPath *watch_path = WATCH_PATH( 
		g_object_new( TYPE_WATCH_PATH, NULL ) );

	watch_link( WATCH( watch_path ), watchgroup, name );

	return( WATCH( watch_path ) );
}

GSList *
watch_path_get( Watchgroup *watchgroup, const char *name, GSList *fallback )
{
	Watch *watch;
	void *value;

	if( !watchgroup )
		return( fallback );

	if( !(watch = watch_find( watchgroup, name )) )
		watch = watch_path_new( watchgroup, name );

	g_assert( IS_WATCH_PATH( watch ) );

	if( !watch_get( watch, &value ) ) 
		return( fallback );
	
	return( *((GSList **) value) );
}

static WatchClass *watch_bool_parent_class = NULL;

static gboolean
watch_bool_update( Watch *watch )
{
	WatchBool *watch_bool = WATCH_BOOL( watch );
	PElement *root = &watch->row->expr->root;

#ifdef DEBUG
	printf( "watch_bool_update: %s\n", NN( IOBJECT( watch )->name ) );
#endif /*DEBUG*/

	if( PEISNOVAL( root ) )
		return( FALSE );
	if( !PEISBOOL( root ) ) {
		heap_error_typecheck( root, IOBJECT( watch )->name, "bool" );
		return( FALSE );
	}

	watch_bool->value = PEGETBOOL( root );

	return( TRUE );
}

static void *
watch_bool_get_value( Watch *watch )
{
	WatchBool *watch_bool = WATCH_BOOL( watch );

	return( (void *) &watch_bool->value );
}

static void
watch_bool_class_init( WatchBoolClass *class )
{
	WatchClass *watch_class = (WatchClass *) class;

	watch_bool_parent_class = g_type_class_peek_parent( class );

	watch_class->update = watch_bool_update;
	watch_class->get_value = watch_bool_get_value;
}

static void
watch_bool_init( WatchBool *watch_bool )
{
#ifdef DEBUG
	printf( "watch_bool_init\n" );
#endif /*DEBUG*/

	watch_bool->value = FALSE;
}

GType
watch_bool_get_type( void )
{
	static GType watch_bool_type = 0;

	if( !watch_bool_type ) {
		static const GTypeInfo info = {
			sizeof( WatchBoolClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) watch_bool_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( WatchBool ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) watch_bool_init,
		};

		watch_bool_type = g_type_register_static( TYPE_WATCH, 
			"WatchBool", &info, 0 );
	}

	return( watch_bool_type );
}

static Watch *
watch_bool_new( Watchgroup *watchgroup, const char *name )
{
	WatchBool *watch_bool = WATCH_BOOL( 
		g_object_new( TYPE_WATCH_BOOL, NULL ) );

	watch_link( WATCH( watch_bool ), watchgroup, name );

	return( WATCH( watch_bool ) );
}

gboolean 
watch_bool_get( Watchgroup *watchgroup, const char *name, gboolean fallback )
{
	Watch *watch;
	void *value;

	if( !watchgroup )
		return( fallback );

	if( !(watch = watch_find( watchgroup, name )) )
		watch = watch_bool_new( watchgroup, name );

	g_assert( IS_WATCH_BOOL( watch ) );

	if( !watch_get( watch, &value ) ) 
		return( fallback );

	return( *((gboolean *) value) );
}

static WatchClass *watch_string_parent_class = NULL;

static void
watch_string_finalize( GObject *gobject )
{
	WatchString *watch_string;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WATCH_STRING( gobject ) );

#ifdef DEBUG
	printf( "watch_string_finalize\n" );
#endif /*DEBUG*/

	watch_string = WATCH_STRING( gobject );

	/* My instance destroy stuff.
	 */
	IM_FREE( watch_string->value );

	G_OBJECT_CLASS( watch_string_parent_class )->finalize( gobject );
}

static gboolean
watch_string_update( Watch *watch )
{
	WatchString *watch_string = WATCH_STRING( watch );
	PElement *root = &watch->row->expr->root;
	char value[1024];

#ifdef DEBUG
	printf( "watch_string_update\n" );
#endif /*DEBUG*/

	if( !heap_get_string( root, value, 1024 ) )
		return( FALSE );

	IM_SETSTR( watch_string->value, value );

	return( TRUE );
}

static void *
watch_string_get_value( Watch *watch )
{
	WatchString *watch_string = WATCH_STRING( watch );

	return( (void *) &watch_string->value );
}

static void
watch_string_class_init( WatchStringClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	WatchClass *watch_class = (WatchClass *) class;

	watch_string_parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = watch_string_finalize;

	watch_class->update = watch_string_update;
	watch_class->get_value = watch_string_get_value;
}

static void
watch_string_init( WatchString *watch_string )
{
#ifdef DEBUG
	printf( "watch_string_init\n" );
#endif /*DEBUG*/

	watch_string->value = NULL;
}

GType
watch_string_get_type( void )
{
	static GType watch_string_type = 0;

	if( !watch_string_type ) {
		static const GTypeInfo info = {
			sizeof( WatchStringClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) watch_string_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( WatchString ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) watch_string_init,
		};

		watch_string_type = g_type_register_static( TYPE_WATCH, 
			"WatchString", &info, 0 );
	}

	return( watch_string_type );
}

static Watch *
watch_string_new( Watchgroup *watchgroup, const char *name )
{
	WatchString *watch_string = WATCH_STRING( 
		g_object_new( TYPE_WATCH_STRING, NULL ) );

	watch_link( WATCH( watch_string ), watchgroup, name );

	return( WATCH( watch_string ) );
}

const char * 
watch_string_get( Watchgroup *watchgroup, 
	const char *name, const char *fallback )
{
	Watch *watch;
	void *value;

	if( !watchgroup )
		return( fallback );

	if( !(watch = watch_find( watchgroup, name )) )
		watch = watch_string_new( watchgroup, name );

	g_assert( IS_WATCH_STRING( watch ) );

	if( !watch_get( watch, &value ) ) 
		return( fallback );

	return( *((const char **) value) );
}

