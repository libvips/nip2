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

	IM_FREEF( g_source_remove, wsg->autosave_timeout );
	icontainer_map( ICONTAINER( wsg ),
		(icontainer_map_fn) icontainer_child_remove, NULL, NULL );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static View *
workspacegroup_view_new( Model *model, View *parent )
{
	return( workspacegroupview_new() );
}

void *
workspacegroup_save_workspace( iContainer *icontainer, void *a, void *b )
{
	Workspace *ws = WORKSPACE( icontainer );
	xmlNode *xnode = (xmlNode *) a;
	Workspacegroup *wsg = WORKSPACEGROUP( b );

	in save-selected mode, save just the current WS

	return( model_save( ws, xnode ) );
}

static xmlNode *
workspacegroup_save( Model *model, xmlNode *xnode )
{
	/* We normally chain up like this:
	 *
	 * 	xthis = MODEL_CLASS( parent_class )->save( model, xnode )
	 *
	 * but that will make a workspacegroup holding our workspaces. Instead 
	 * we want to save all our workspaces directly to xnode with nothing 
	 * about us in there.
	 *
	 * See model_real_save().
	 */

	if( icontainer_map( ICONTAINER( model ), 
		workspacegroup_save_workspace, xnode, model ) )
		return( NULL );

	return( xnode );
}

static gboolean
workspacegroup_top_load( Filemodel *filemodel,
	ModelLoadState *state, Model *parent, xmlNode *xroot )
{
	Workspacegroup *wsg = WORKSPACEGROUP( filemodel );
	Workspaceroot *wsr = WORKSPACEROOT( parent );

	char *new_dir;
	Workspace *ws;
	Column *current_col;
	xmlNode *i, *j, *k, *xnode;
	char name[FILENAME_MAX];
	int best_major;
	int best_minor;

#ifdef DEBUG
#endif /*DEBUG*/
	printf( "workspacegroup_top_load: from %s\n", state->filename );

	/* The top node should be the first workspace. Get the filename this
	 * workspace was saved as so we can work out how to rewrite embedded
	 * filenames.
	 *
	 * The filename field can be missing. 
	 */
	if( (xnode = get_node( xroot, "Workspace" )) &&
		get_sprop( xnode, "filename", name, FILENAME_MAX ) ) {
		/* The old filename could be non-native, so we must rewrite 
		 * to native form first so g_path_get_dirname() can work.
		 */
		path_compact( name );

		state->old_dir = g_path_get_dirname( name ); 

		new_dir = g_path_get_dirname( state->filename_user );
		path_rewrite_add( state->old_dir, new_dir, FALSE );
		g_free( new_dir );
	}

	/* See commented out ode in COLUMN load below.
	 */
	printf( "workspace_top_load: compat check on merge is broken!\n" );

	switch( wsg->load_type ) {
	case WORKSPACEGROUP_LOAD_TOP:
		/* Load all workspaces into this wsg.
		 */
		for( xnode = xroot->children; xnode; xnode = xnode->next ) {
			if( strcmp( (char *) xnode->name, "Workspace" ) != 0 )
				continue;

			column_set_offset( WORKSPACEVIEW_MARGIN_LEFT, 
				WORKSPACEVIEW_MARGIN_TOP );

			/* Rename to avoid clashes. 
			 */
			if( !get_sprop( xnode, "name", name, FILENAME_MAX ) ) 
				return( FALSE );
			while( compile_lookup( wsr->sym->expr->compile, name ) )
				increment_name( name );
			ws = workspace_new( wsg, name );

			if( get_iprop( xnode, "major", 
					&FILEMODEL( ws )->major ) &&
				get_iprop( xnode, "minor", 
					&FILEMODEL( ws )->minor ) &&
				get_iprop( xnode, "micro", 
					&FILEMODEL( ws )->micro ) )
				FILEMODEL( ws )->versioned = TRUE;

			/* If necessary, load up compatibility definitions.
			 */
			if( !workspace_load_compat( ws, 
				FILEMODEL( ws )->major, 
				FILEMODEL( ws )->minor ) ) 
				return( FALSE );

			if( model_load( MODEL( ws ), state, parent, xnode ) )
				return( FALSE );
		}

		break;

	case WORKSPACE_LOAD_COLUMNS:
		printf( "workspacegroup_top_load: column load not done\n" ); 

		/* Load at column level ... rename columns which clash with 
		 * columns in the current workspace. Also look out for clashes
		 * with columns we will load.
		 */
		for( i = xnode->children; i; i = i->next ) 
			workspace_rename_column_node( ws, 
				state, i, xnode->children );

		/* Load those columns.
		 */
		for( i = xnode->children; i; i = i->next ) 
			if( !model_new_xml( state, MODEL( ws ), i ) )
				return( FALSE );

		/* Is there a version mismatch? Issue a warning.
		if( workspace_have_compat( state->major, state->minor, 
			&best_major, &best_minor ) &&
			(best_major != filemodel->major ||
			best_minor != filemodel->minor) ) {
			error_top( _( "Version mismatch." ) );
			error_sub( _( "File \"%s\" was saved from %s-%d.%d.%d. "
				"You may see compatibility problems." ),
				state->filename, PACKAGE,
				state->major, state->minor, state->micro );
			iwindow_alert( GTK_WIDGET( ws->iwnd ), 
				GTK_MESSAGE_INFO );
		}
		 */

		break;

	case WORKSPACE_LOAD_ROWS:
		printf( "workspacegroup_top_load: row load not done\n" ); 

		current_col = workspace_column_pick( ws );

		/* Rename all rows into current column ... loop over column,
		 * subcolumns, rows.
		 */
		for( i = xnode->children; i; i = i->next ) 
			for( j = i->children; j; j = j->next ) 
				for( k = j->children; k; k = k->next ) 
					workspace_rename_row_node( state, 
						current_col, k );

		/* And load rows.
		 */
		for( i = xnode->children; i; i = i->next ) 
			for( j = i->children; j; j = j->next ) 
				for( k = j->children; k; k = k->next ) 
					if( !model_new_xml( state, 
						MODEL( current_col->scol ), 
						k ) )
						return( FALSE );

		break;

	default:
		g_assert( FALSE );
	}

	return( FILEMODEL_CLASS( parent_class )->top_load( filemodel, 
		state, parent, xnode ) );
}

/* Backup the last WS_RETAIN workspaces.
 */
#define WS_RETAIN (10)

/* Array of names of workspace files we are keeping.
 */
static char *retain_files[WS_RETAIN] = { NULL };

/* On safe exit, remove all ws checkmarks.
 */
void
workspacegroup_autosave_clean( void )
{
	int i;

	for( i = 0; i < WS_RETAIN; i++ ) {
		if( retain_files[i] ) {
			unlinkf( "%s", retain_files[i] );
			IM_FREE( retain_files[i] );
		}
	}
}

/* Save the workspace to one of our temp files.
 */
static gboolean
workspacegroup_checkmark_timeout( Workspacegroup *wsg )
{
	/* The next one we allocate.
	 */
	static int retain_next = 0;

	wsg->autosave_timeout = 0;

	if( !AUTO_WS_SAVE )
		return( FALSE );

	/* Don't backup auto loaded workspace (eg. preferences). These are
	 * system things and don't need it.
	 */
	if( FILEMODEL( wsg )->auto_load )
		return( FALSE );

	/* Do we have a name for this retain file?
	 */
	if( !retain_files[retain_next] ) {
		char filename[FILENAME_MAX];

		/* No name yet - make one up.
		 */
		if( !temp_name( filename, "ws" ) )
			return( FALSE );
		retain_files[retain_next] = im_strdup( NULL, filename );
	}
 
	if( !filemodel_save_all( FILEMODEL( wsg ), retain_files[retain_next] ) )
		return( FALSE );

	retain_next = (retain_next + 1) % WS_RETAIN;

	return( FALSE );
}

/* Save the workspace to one of our temp files. Don't save directly (pretty
 * slow), instead set a timeout and save when we're quiet for >1s.
 */
static void
workspacegroup_checkmark( Workspacegroup *wsg )
{
	if( !AUTO_WS_SAVE )
		return;
	if( FILEMODEL( wsg )->auto_load )
		return;

	IM_FREEF( g_source_remove, wsg->autosave_timeout );
	wsg->autosave_timeout = g_timeout_add( 1000, 
		(GSourceFunc) workspacegroup_checkmark_timeout, wsg );
}

typedef struct {
	/* Best so far filename.
	 */
	char filename[FILENAME_MAX];

	/* Best-so-far file date.
	 */
	time_t time;
} AutoRecover;

/* This file any better than the previous best candidate? Subfn of below.
 */
static void *
workspacegroup_test_file( const char *name, void *a, void *b, void *c )
{
	AutoRecover *recover = (AutoRecover *) a;

	char buf[FILENAME_MAX];
	time_t time;
	int i;

	im_strncpy( buf, name, FILENAME_MAX );
	path_expand( buf );
	for( i = 0; i < WS_RETAIN; i++ )
		if( retain_files[i] && 
			strcmp( buf, retain_files[i] ) == 0 )
			return( NULL );
	if( !(time = mtime( "%s", buf )) )
		return( NULL );
	if( recover->time > 0 && time < recover->time )
		return( NULL );
	
	strcpy( recover->filename, buf );
	recover->time = time;

	return( NULL );
}

/* Search for the most recent "*.ws" file 
 * in the tmp area owned by us, with a size > 0, that's not in our
 * retain_files[] set.
 */
char *
workspacegroup_autosave_recover( void )
{
	AutoRecover recover;

	strcpy( recover.filename, "" ); 
	recover.time = 0;  
	(void) path_map_dir( PATH_TMP, "*.ws", 
		(path_map_fn) workspacegroup_test_file, &recover );

	if( !recover.time )
		return( NULL );

	return( g_strdup( recover.filename ) ); 
}

static void 
workspacegroup_set_modified( Filemodel *filemodel, gboolean modified )
{
	Workspacegroup *wsg = WORKSPACEGROUP( filemodel );

	workspacegroup_checkmark( wsg );

	FILEMODEL_CLASS( parent_class )->set_modified( filemodel, modified );
}

static gboolean
workspacegroup_save_all( Filemodel *filemodel, const char *filename )
{
	gboolean result;

#ifdef DEBUG
	printf( "workspacegroup_save_all: %s to %s\n",
		NN( IOBJECT( filemodel )->name ), filename );
#endif /*DEBUG*/

	if( (result = FILEMODEL_CLASS( parent_class )->
		save_all( filemodel, filename )) )
		/* This will add save-as files to recent too. Don't note
		 * auto_load on recent, since it won't have been loaded by the
		 * user.
		 */
		if( !filemodel->auto_load )
			mainw_recent_add( &mainw_recent_workspace, filename );

	return( result );
}

static void
workspacegroup_class_init( WorkspacegroupClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	FilemodelClass *filemodel_class = (FilemodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = workspacegroup_dispose;

	/* ->load() is done by workspace_top_load().
	 */
	model_class->view_new = workspacegroup_view_new;
	model_class->save = workspacegroup_save;

	filemodel_class->filetype = filesel_type_workspace;
	filemodel_class->top_load = workspacegroup_top_load;
	filemodel_class->save_all = workspacegroup_save_all;
	filemodel_class->set_modified = workspacegroup_set_modified;
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
workspacegroup_link( Workspacegroup *wsg, Workspaceroot *wsr )
{
	icontainer_child_add( ICONTAINER( wsr ), ICONTAINER( wsg ), -1 );
	wsg->wsr = wsr;
	filemodel_register( FILEMODEL( wsg ) );
}

Workspacegroup *
workspacegroup_new( Workspaceroot *wsr )
{
	Workspacegroup *wsg;

#ifdef DEBUG
#endif /*DEBUG*/
	printf( "workspacegroup_new:\n" ); 

	wsg = WORKSPACEGROUP( g_object_new( TYPE_WORKSPACEGROUP, NULL ) );
	workspacegroup_link( wsg, wsr );
	filemodel_set_modified( FILEMODEL( wsg ), FALSE );

	return( wsg );
}

/* Make the blank workspacegroup we present the user with (in the absence of
 * anything else).
 */
Workspacegroup *
workspacegroup_new_blank( Workspaceroot *wsr, const char *name )
{
	Workspacegroup *wsg;
	Workspace *ws;

	if( !(wsg = workspacegroup_new( wsr )) )
		return( NULL );
	iobject_set( IOBJECT( wsg ), name, _( "Default empty workspace" ) );
	workspace_new_blank( wsg, "tab1" );
	filemodel_set_modified( FILEMODEL( wsg ), FALSE );

	return( wsg );
}

Workspacegroup *
workspacegroup_new_filename( Workspaceroot *wsr, const char *filename )
{
	Workspacegroup *wsg;
	char name[FILENAME_MAX];

	if( !(wsg = workspacegroup_new( wsr )) )
		return( NULL ); 
	name_from_filename( filename, name );
	iobject_set( IOBJECT( wsg ), name, _( "Default empty workspace" ) );
	filemodel_set_filename( FILEMODEL( wsg ), filename );
	filemodel_set_modified( FILEMODEL( wsg ), FALSE );

	return( wsg );
}

/* Load a file as a workspacegroup.
 */
Workspacegroup *
workspacegroup_new_from_file( Workspaceroot *wsr, 
	const char *filename, const char *filename_user )
{
	Workspacegroup *wsg;

	if( !(wsg = workspacegroup_new( wsr )) )
		return( NULL );
	wsg->load_type = WORKSPACEGROUP_LOAD_TOP;

	if( !filemodel_load_all( FILEMODEL( wsg ), 
		MODEL( wsr ), filename, filename_user ) ) {
		g_object_unref( G_OBJECT( wsg ) );
		return( NULL );
	}

	filemodel_set_filename( FILEMODEL( wsg ), 
		filename_user ? filename_user : filename );
	filemodel_set_modified( FILEMODEL( wsg ), FALSE );

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

	if( !(wsg = workspacegroup_new( wsr )) )
		return( NULL );
	if( !filemodel_load_all_openfile( FILEMODEL( wsg ), 
		MODEL( wsr ), of ) ) {
		g_object_unref( G_OBJECT( wsg ) );
		return( NULL );
	}

	filemodel_set_filename( FILEMODEL( wsg ), of->fname );
	filemodel_set_modified( FILEMODEL( wsg ), FALSE );

#ifdef DEBUG
	printf( "(set name = %s)\n", IOBJECT( wsg )->name );
#endif /*DEBUG*/

	return( wsg );
}

Workspacegroup *
workspacegroup_duplicate( Workspacegroup *wsg )
{
	Workspacegroup *new_wsg;

	new_wsg = workspacegroup_new_blank( wsg->wsr, NULL );

	/* Save to file, load back again, see workspace_clone().
	 */
	printf( "workspacegroup_duplicate:\n" );

	return( new_wsg );
}

