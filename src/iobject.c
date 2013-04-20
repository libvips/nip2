/* abstract base class for all nip objects
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
	SIG_DESTROY,	/* End lifetime */
	SIG_CHANGED,	/* iObject has changed somehow */
	SIG_LAST
};

static GObjectClass *parent_class = NULL;

static guint iobject_signals[SIG_LAST] = { 0 };

/* Don't emit "destroy" immediately, do it from the _dispose handler.
 */
void *
iobject_destroy( iObject *iobject )
{
#ifdef DEBUG
	printf( "iobject_destroy: " );
	iobject_print( iobject );
#endif /*DEBUG*/

	if( !iobject->in_destruction )
		g_object_run_dispose( G_OBJECT( iobject ) );

	return( NULL );
}

void *
iobject_changed( iObject *iobject )
{
	g_return_val_if_fail( iobject != NULL, NULL );
	g_return_val_if_fail( IS_IOBJECT( iobject ), NULL );

#ifdef DEBUG
	printf( "iobject_changed: " );
	iobject_print( iobject );
#endif /*DEBUG*/

	g_signal_emit( G_OBJECT( iobject ), iobject_signals[SIG_CHANGED], 0 );

	return( NULL );
}

void *
iobject_info( iObject *iobject, VipsBuf *buf )
{
	iObjectClass *iobject_class = IOBJECT_GET_CLASS( iobject );

	g_return_val_if_fail( iobject != NULL, NULL );
	g_return_val_if_fail( IS_IOBJECT( iobject ), NULL );

	if( iobject_class->info )
		iobject_class->info( iobject, buf );

	return( NULL );
}

static void
iobject_dispose( GObject *gobject )
{
	iObject *iobject = IOBJECT( gobject );

#ifdef DEBUG
	printf( "iobject_dispose: " );
	iobject_print( iobject );
#endif /*DEBUG*/

	if( !iobject->in_destruction ) {
		iobject->in_destruction = TRUE;
		g_signal_emit( G_OBJECT( iobject ), 
			iobject_signals[SIG_DESTROY], 0 );
		iobject->in_destruction = FALSE;
	}

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
iobject_finalize( GObject *gobject )
{
	iObject *iobject = IOBJECT( gobject );

#ifdef DEBUG
	printf( "iobject_finalize: " );
	iobject_print( iobject );
#endif /*DEBUG*/

	/* Unlike GTK, we allow floating objects to be finalized. Handy if a
	 * _new() fails. So don't assert( !iobject->floating );
	 */

	IM_FREE( iobject->name );
	IM_FREE( iobject->caption );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
iobject_real_destroy( iObject *iobject )
{
}

static void
iobject_real_changed( iObject *iobject )
{
	iObjectClass *iobject_class = IOBJECT_GET_CLASS( iobject );

	if( iobject_class->generate_caption )
		IM_SETSTR( iobject->caption, 
			iobject_class->generate_caption( iobject ) );
}

static void
iobject_real_info( iObject *iobject, VipsBuf *buf )
{
	if( iobject->name )
		vips_buf_appendf( buf, "name = \"%s\"\n", iobject->name );
	if( iobject->caption )
		vips_buf_appendf( buf, "caption = \"%s\"\n", iobject->caption );
	vips_buf_appendf( buf, "iObject :: \"%s\"\n", 
		G_OBJECT_TYPE_NAME( iobject ) );
}

static void
iobject_class_init( iObjectClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = iobject_dispose;
	gobject_class->finalize = iobject_finalize;

	class->destroy = iobject_real_destroy;
	class->changed = iobject_real_changed;
	class->info = iobject_real_info;
	class->generate_caption = NULL;

	class->user_name = _( "Object" );

	/* Create signals.
	 */
	iobject_signals[SIG_DESTROY] = g_signal_new( "destroy",
		G_TYPE_FROM_CLASS( gobject_class ),
		G_SIGNAL_RUN_CLEANUP | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
		G_STRUCT_OFFSET( iObjectClass, destroy ), 
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
	iobject_signals[SIG_CHANGED] = g_signal_new( "changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( iObjectClass, changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
}

static void
iobject_init( iObject *iobject )
{
#ifdef DEBUG
	printf( "iobject_init: " );
	iobject_print( iobject );
#endif /*DEBUG*/

	/* Init our instance fields.
	 */
	iobject->name = NULL;
	iobject->caption = NULL;
	iobject->floating = TRUE;
	iobject->in_destruction = FALSE;
}

GType
iobject_get_type( void )
{
	static GType iobject_type = 0;

	if( !iobject_type ) {
		static const GTypeInfo info = {
			sizeof( iObjectClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) iobject_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( iObject ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) iobject_init,
		};

		iobject_type = g_type_register_static( G_TYPE_OBJECT, 
			"iObject", &info, 0 );
	}

	return( iobject_type );
}

/* Test the name field ... handy with map.
 */
void *
iobject_test_name( iObject *iobject, const char *name )
{
	g_return_val_if_fail( iobject != NULL, NULL );
	g_return_val_if_fail( IS_IOBJECT( iobject ), NULL );

	if( iobject->name && strcmp( iobject->name, name ) == 0 )
		return( iobject );

	return( NULL );
}

void *
iobject_print( iObject *iobject )
{
	g_print( "%s \"%s\" (%p)\n", 
		G_OBJECT_TYPE_NAME( iobject ),
		NN( iobject->name ), 
		iobject ); 

	return( NULL );
}

void
iobject_set( iObject *iobject, const char *name, const char *caption )
{
	gboolean changed = FALSE;

	g_return_if_fail( iobject != NULL );
	g_return_if_fail( IS_IOBJECT( iobject ) );

	if( name && name != iobject->name ) {
		IM_SETSTR( iobject->name, name );
		changed = TRUE;
	}
	if( caption && caption != iobject->caption ) {
		IM_SETSTR( iobject->caption, caption );
		changed = TRUE;
	}

	if( changed )
		iobject_changed( iobject );

#ifdef DEBUG
	printf( "iobject_set: " );
	iobject_print( iobject );
#endif /*DEBUG*/
}

void
iobject_sink( iObject *iobject )
{
	g_assert( IS_IOBJECT( iobject ) ); 

	if( iobject->floating ) {
		iobject->floating = FALSE;
		g_object_unref( G_OBJECT( iobject ) );
	}
}

void
iobject_dump( iObject *iobject )
{
	char txt[1000];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	iobject_info( iobject, &buf );
	printf( "%s", vips_buf_all( &buf ) );
}
