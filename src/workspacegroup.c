/* A set of workspaces loaded and saved from a ws file.
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

static FilemodelClass *parent_class = NULL;

Workspace *
workspacegroup_map( Workspacegroup *wsg, workspace_map_fn fn, void *a, void *b )
{
	return( (Workspace *) icontainer_map( ICONTAINER( wsg ), 
		(icontainer_map_fn) fn, a, b ) );
}

static void *
workspacegroup_is_empty_sub( Workspace *ws, gboolean *empty )
{
	if( !workspace_is_empty( ws ) ) {
		*empty = FALSE;
		return( ws );
	}

	return( NULL );
}

gboolean 
workspacegroup_is_empty( Workspacegroup *wsg )
{
	gboolean empty;

	empty = TRUE;
	(void) workspacegroup_map( wsg, 
		(workspace_map_fn) workspacegroup_is_empty_sub, &empty, NULL );

	return( empty );
}

static void 
workspacegroup_dispose( GObject *gobject )
{
	Workspacegroup *wsg;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WORKSPACEGROUP( gobject ) );

	wsg = WORKSPACEGROUP( gobject );

#ifdef DEBUG
	printf( "workspacegroup_dispose %s\n", IOBJECT( wsg )->name );
#endif /*DEBUG*/

	icontainer_map( ICONTAINER( wsg ),
		(icontainer_map_fn) icontainer_child_remove, NULL, NULL );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
workspacegroup_class_init( WorkspacegroupClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	FilemodelClass *filemodel_class = (FilemodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = workspacegroup_dispose;

	filemodel_class->filetype = filesel_type_workspace;
}

static void
workspacegroup_init( Workspacegroup *wsg )
{
}

GType
workspacegroup_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( WorkspacegroupClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) workspacegroup_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Workspacegroup ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) workspacegroup_init,
		};

		type = g_type_register_static( TYPE_FILEMODEL, 
			"Workspacegroup", &info, 0 );
	}

	return( type );
}

static void
workspacegroup_link( Workspacegroup *wsg, Workspaceroot *wsr, const char *name )
{
	iobject_set( IOBJECT( wsg ), name, NULL );
	icontainer_child_add( ICONTAINER( wsr ), ICONTAINER( wsg ), -1 );
	wsg->wsr = wsr;

	filemodel_register( FILEMODEL( wsg ) );
}

Workspacegroup *
workspacegroup_new( Workspaceroot *wsr, const char *name )
{
	Workspacegroup *wsg;

#ifdef DEBUG
	printf( "workspacegroup_new: %s\n", name );
#endif /*DEBUG*/

	wsg = WORKSPACEGROUP( g_object_new( TYPE_WORKSPACEGROUP, NULL ) );
	workspacegroup_link( wsg, wsr, name );

	return( wsg );
}

/* Make the blank workspacegroup we present the user with (in the absence of
 * anything else).
 */
Workspacegroup *
workspacegroup_new_blank( Workspaceroot *wsr, const char *name )
{
	Workspacegroup *wsg;

	if( !(wsg = workspacegroup_new( wsr, name )) )
		return( NULL );
	iobject_set( IOBJECT( wsg ), NULL, _( "Default empty workspace" ) );

	return( wsg );
}

Workspacegroup *
workspacegroup_new_filename( Workspaceroot *wsr, const char *filename )
{
	char name[FILENAME_MAX];
	Workspacegroup *wsg;

	name_from_filename( filename, name );
	wsg = workspacegroup_new( wsr, name );
	filemodel_set_filename( FILEMODEL( wsg ), filename );

	return( wsg );
}

static gboolean
workspacegroup_load_empty( Workspacegroup *wsg, Workspaceroot *wsr, 
	const char *filename, const char *filename_user )
{
	printf( "workspacegroup_load_empty:\n" ); 

	return( TRUE );
}

/* Load a file as a workspacegroup.
 */
Workspacegroup *
workspacegroup_new_from_file( Workspaceroot *wsr, 
	const char *filename, const char *filename_user )
{
	Workspacegroup *wsg;

	wsg = WORKSPACEGROUP( g_object_new( TYPE_WORKSPACEGROUP, NULL ) );
	if( !workspacegroup_load_empty( wsg, wsr, filename, filename_user ) ) {
		g_object_unref( G_OBJECT( wsg ) );
		return( NULL );
	}

	return( wsg );
}

/* New workspacegroup from a file.
 */
Workspacegroup *
workspacegroup_new_from_openfile( Workspaceroot *wsr, iOpenFile *of )
{
	Workspacegroup *wsg;

#ifdef DEBUG
	printf( "workspacegroup_new_from_openfile: %s\n", of->fname );
#endif /*DEBUG*/

	wsg = WORKSPACEGROUP( g_object_new( TYPE_WORKSPACEGROUP, NULL ) );
	wsg->load_type = WORKSPACEGROUP_LOAD_TOP;
	if( !filemodel_load_all_openfile( FILEMODEL( wsg ), 
		MODEL( wsr ), of ) ) {
		g_object_unref( G_OBJECT( wsg ) );
		return( NULL );
	}

	filemodel_set_modified( FILEMODEL( wsg ), FALSE );
	filemodel_set_filename( FILEMODEL( wsg ), of->fname );

#ifdef DEBUG
	printf( "(set name = %s)\n", IOBJECT( wsg )->name );
#endif /*DEBUG*/

	return( wsg );
}

