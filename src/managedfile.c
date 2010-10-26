/* a managed FILE* ... for lazy file read
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

#include "ip.h"

/* 
#define DEBUG
 */

static ManagedClass *parent_class = NULL;

static void
managedfile_dispose( GObject *gobject )
{
	Managedfile *managedfile = MANAGEDFILE( gobject );

#ifdef DEBUG
	printf( "managedfile_dispose: " );
	iobject_print( IOBJECT( managedfile ) );
#endif /*DEBUG*/

	IM_FREEF( ifile_close, managedfile->file );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
managedfile_info( iObject *iobject, VipsBuf *buf )
{
	Managedfile *managedfile = MANAGEDFILE( iobject );

	vips_buf_appendf( buf, "managedfile->fp = %p\n", managedfile->file->fp );
	vips_buf_appendf( buf, "managedfile->file->filename = %s\n", 
		managedfile->file->fname );
	vips_buf_appendf( buf, "managedfile->file->last_errno = %d\n", 
		managedfile->file->last_errno );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );
}

static void
managedfile_class_init( ManagedfileClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = managedfile_dispose;

	iobject_class->info = managedfile_info;
}

static void
managedfile_init( Managedfile *managedfile )
{
#ifdef DEBUG
	printf( "managedfile_init: %p\n", managedfile );
#endif /*DEBUG*/

	managedfile->file = NULL;
}

GType
managedfile_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ManagedfileClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) managedfile_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Managedfile ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) managedfile_init,
		};

		type = g_type_register_static( TYPE_MANAGED, 
			"Managedfile", &info, 0 );
	}

	return( type );
}

Managedfile *
managedfile_new( Heap *heap, const char *filename )
{
	Managedfile *managedfile;
	iOpenFile *file;

#ifdef DEBUG
	printf( "managedfile_new: %p: %s\n", managedfile, filename );
#endif /*DEBUG*/

	if( !(file = ifile_open_read( "%s", filename )) )
		return( NULL );

	managedfile = g_object_new( TYPE_MANAGEDFILE, NULL );
	managed_link_heap( MANAGED( managedfile ), heap );
	managedfile->file = file;

	return( managedfile );
}

int
managedfile_getc( Managedfile *managedfile )
{
	int ch = ifile_getc( managedfile->file );

#ifdef DEBUG
{
	char in[2];
	char out[3];

	in[0] = ch;
	in[1] = '\0';
	my_strecpy( out, in, FALSE );
	printf( "managedfile_getc: '%s' (%d)\n", out, ch );
}
#endif /*DEBUG*/

	return( ch );
}
