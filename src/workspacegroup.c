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

void
workspacegroup_set_load_type( Workspacegroup *wsg, 
	WorkspacegroupLoadType load_type )
{
	wsg->load_type = load_type;
}

void
workspacegroup_set_save_type( Workspacegroup *wsg, 
	WorkspacegroupSaveType save_type )
{
	wsg->save_type = save_type;
}

static Workspace *
workspacegroup_get_workspace( Workspacegroup *wsg )
{
	if( !ICONTAINER( wsg )->current )
		return( NULL );

	return( WORKSPACE( ICONTAINER( wsg )->current ) ); 
}

static Workspace *
workspacegroup_workspace_pick( Workspacegroup *wsg )
{
	Workspace *ws;

	if( (ws = workspacegroup_get_workspace( wsg )) )
		return( ws );

	if( ICONTAINER( wsg )->children ) {
		ws = WORKSPACE( ICONTAINER( wsg )->children->data );
		icontainer_current( ICONTAINER( wsg ), ICONTAINER( ws ) );

		return( ws );
	}

	ws = workspace_new_blank( wsg );

	(void) workspace_column_pick( ws ); 

	return( ws );
}

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
#endif /*DEBUG*/
	printf( "workspacegroup_dispose %s\n", IOBJECT( wsg )->name );

	IM_FREEF( g_source_remove, wsg->autosave_timeout );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void 
workspacegroup_finalize( GObject *gobject )
{
	Workspacegroup *wsg;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WORKSPACEGROUP( gobject ) );

	wsg = WORKSPACEGROUP( gobject );

#ifdef DEBUG
#endif /*DEBUG*/
	printf( "workspacegroup_finalize %s\n", IOBJECT( wsg )->name );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static View *
workspacegroup_view_new( Model *model, View *parent )
{
	return( workspacegroupview_new() );
}

static void *
workspacegroup_save_sub( iContainer *icontainer, void *a, void *b )
{
	Workspace *ws = WORKSPACE( icontainer );
	xmlNode *xnode = (xmlNode *) a;
	Workspacegroup *wsg = WORKSPACEGROUP( b );

	/* Only save all workspaces in save-all mode. 
	 */
	if( wsg->save_type != WORKSPACEGROUP_SAVE_ALL &&
		WORKSPACE( ICONTAINER( wsg )->current ) != ws )
		return( NULL );

	return( model_save( MODEL( ws ), xnode ) );
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
		workspacegroup_save_sub, xnode, model ) )
		return( NULL );

	return( xnode );
}

/* Loops over xml trees follow this pattern.
 */
#define FOR_ALL_XML( ROOT, CHILD, CHILD_NAME ) { \
	xmlNode *CHILD; \
	\
	for( CHILD = ROOT->children; CHILD; CHILD = CHILD->next ) { \
		if( strcmp( (char *) CHILD->name, CHILD_NAME ) != 0 ) \
			continue;

#define FOR_ALL_XML_END } }

static void
workspacegroup_rename_workspace_node( Workspacegroup *wsg, 
	ModelLoadState *state, xmlNode *xws )
{
	Workspaceroot *wsr = wsg->wsr;

	char name[MAX_STRSIZE];
	char new_name[MAX_STRSIZE];

	if( !get_sprop( xws, "name", name, MAX_STRSIZE ) )
		return;

	strcpy( new_name, name );
	while( compile_lookup( wsr->sym->expr->compile, new_name ) ||
		model_loadstate_taken( state, new_name ) )
		increment_name( new_name );

	(void) set_sprop( xws, "name", new_name );
	(void) model_loadstate_rename_new( state, name, new_name );
}

/* Does a scrap of XML need compat defs?
 */
static gboolean
workspacegroup_xml_needs_compat( ModelLoadState *state, xmlNode *xws, 
	int *best_major, int *best_minor )
{
	int major;
	int minor;

	/* What version is the XML expecting? A combination of the version of
	 * nip that saved the file, and any compat notes on the workspace XML
	 */
	if( !get_iprop( xws, "major", &major ) ||
		!get_iprop( xws, "minor", &minor ) ) {
		/* Fall back to the version number in the xml header.
		 */
		major = state->major;
		minor = state->minor;
	}

	/* Find the best set of compat we have.
	 */
	return( workspace_have_compat( major, minor, best_major, best_minor ) );
}

/* Load all workspaces into this wsg.
 */
static gboolean
workspacegroup_load_new( Workspacegroup *wsg, 
	ModelLoadState *state, xmlNode *xroot )
{
	Workspace *first_ws;

	/* Rename ... new names for any workspaces which clash. 
	 */
	FOR_ALL_XML( xroot, xws, "Workspace" ) {
		workspacegroup_rename_workspace_node( wsg, state, xws );
	} FOR_ALL_XML_END

	/* _front() the first ws we load. Needed for things like duplicate ws
	 * and merge wses.
	 */
	first_ws = NULL;

	FOR_ALL_XML( xroot, xws, "Workspace" ) {
		char name[MAX_STRSIZE];
		Workspace *ws;
		int major;
		int minor;

		column_set_offset( WORKSPACEVIEW_MARGIN_LEFT, 
			WORKSPACEVIEW_MARGIN_TOP );

		if( !get_sprop( xws, "name", name, FILENAME_MAX ) || 
			!(ws = workspace_new( wsg, name )) )
			return( FALSE );

		if( workspacegroup_xml_needs_compat( state, xws, 
			&major, &minor ) &&
			!workspace_load_compat( ws, major, minor ) ) 
			return( FALSE );

		if( model_load( MODEL( ws ), state, MODEL( wsg ), xws ) )
			return( FALSE );

		if( !first_ws )
			first_ws = ws;
	} FOR_ALL_XML_END

	if( first_ws ) 
		icontainer_current( ICONTAINER( wsg ), ICONTAINER( first_ws ) );

	return( TRUE );
}

static void
workspacegroup_rename_row_node( Workspace *ws, 
	ModelLoadState *state, const char *col_name, xmlNode *xrow )
{
	char old_name[MAX_STRSIZE];
	char new_name[MAX_STRSIZE];

	if( !get_sprop( xrow, "name", old_name, MAX_STRSIZE ) )
		return;

	im_snprintf( new_name, MAX_STRSIZE, "%s1", col_name );
	while( compile_lookup( ws->sym->expr->compile, new_name ) ||
		model_loadstate_taken( state, new_name ) )
		increment_name( new_name );

	(void) set_sprop( xrow, "name", new_name );
	(void) model_loadstate_rename_new( state, old_name, new_name );

	printf( "workspacegroup_rename_row_node: renamed %s as %s\n",
		old_name, new_name );
}

/* Rename column if there's one of that name in workspace. 
 */
static void
workspacegroup_rename_column_node( Workspacegroup *wsg, 
	Workspace *ws, ModelLoadState *state, xmlNode *xcol )
{
	char name[MAX_STRSIZE];
	char new_name[256];

	if( !get_sprop( xcol, "name", name, MAX_STRSIZE ) )
		return;

	im_strncpy( new_name, name, 256 );
	while( workspace_column_find( ws, new_name ) ||
		model_loadstate_column_taken( state, new_name ) ) 
		workspace_column_name_new( ws, new_name );

	if( strcmp( name, new_name ) != 0 ) { 
#ifdef DEBUG
#endif /*DEBUG*/
		printf( "workspace_rename_column_node: renaming column "
			"%s to %s\n", name, new_name );

		(void) set_sprop( xcol, "name", new_name );
		(void) model_loadstate_column_rename_new( state, 
			name, new_name ); 

		/* And allocate new names for all rows in the subcolumn.
		 */
		FOR_ALL_XML( xcol, xsub, "Subcolumn" ) {
			FOR_ALL_XML( xsub, xrow, "Row" ) {
				workspacegroup_rename_row_node( ws, state, 
					new_name, xrow );
			} FOR_ALL_XML_END
		} FOR_ALL_XML_END
	}
}

/* Load at column level ... rename columns which clash with 
 * columns in the current workspace. Also look out for clashes
 * with columns we will load.
 */
static gboolean
workspacegroup_load_columns( Workspacegroup *wsg, 
	ModelLoadState *state, xmlNode *xroot )
{
	Workspace *ws = workspacegroup_workspace_pick( wsg );

	int xml_major;
	int xml_minor;
	gboolean found;
	int ws_major;
	int ws_minor;

	/* Look for any compat problems.
	 */
	found = FALSE;
	FOR_ALL_XML( xroot, xws, "Workspace" ) { 
		if( workspacegroup_xml_needs_compat( state, xws, 
			&xml_major, &xml_minor ) ) {
			found = TRUE;
			break;
		}
	} FOR_ALL_XML_END

	workspace_get_version( ws, &ws_major, &ws_minor );
	if( found &&
		(xml_major != ws_major ||
		xml_minor != ws_minor) ) {
		error_top( _( "Version mismatch." ) );
		error_sub( _( "File \"%s\" needs version %d.%d. Merging "
			"into this tab may cause compatibility problems." ),
			state->filename, xml_major, xml_minor );
		iwindow_alert( GTK_WIDGET( wsg->iwnd ), GTK_MESSAGE_INFO );
	}

	/* Search all the columns we will load for their names and add rename
	 * rules.
	 */
	FOR_ALL_XML( xroot, xws, "Workspace" ) { 
		FOR_ALL_XML( xws, xcol, "Column" ) { 
			workspacegroup_rename_column_node( wsg, ws, 
				state, xcol );
		} FOR_ALL_XML_END
	} FOR_ALL_XML_END

	/* Load those columns.
	 */
	FOR_ALL_XML( xroot, xws, "Workspace" ) { 
		FOR_ALL_XML( xws, xcol, "Column" ) { 
			if( !model_new_xml( state, MODEL( ws ), xcol ) )
				return( FALSE );
		} FOR_ALL_XML_END
	} FOR_ALL_XML_END

	return( TRUE ); 
}

/* Load at row level ... merge into the current column.
 */
static gboolean
workspacegroup_load_rows( Workspacegroup *wsg, 
	ModelLoadState *state, xmlNode *xroot )
{
	Workspace *ws = workspacegroup_workspace_pick( wsg );
	Column *col = workspace_column_pick( ws );

	int xml_major;
	int xml_minor;
	gboolean found;
	int ws_major;
	int ws_minor;

	/* Look for any compat problems.
	 */
	found = FALSE;
	FOR_ALL_XML( xroot, xws, "Workspace" ) { 
		if( workspacegroup_xml_needs_compat( state, xws, 
			&xml_major, &xml_minor ) ) {
			found = TRUE;
			break;
		}
	} FOR_ALL_XML_END

	workspace_get_version( ws, &ws_major, &ws_minor );
	if( found &&
		(xml_major != ws_major ||
		xml_minor != ws_minor) ) {
		error_top( _( "Version mismatch." ) );
		error_sub( _( "File \"%s\" needs version %d.%d. Merging "
			"into this tab may cause compatibility problems." ),
			state->filename, xml_major, xml_minor );
		iwindow_alert( GTK_WIDGET( wsg->iwnd ), GTK_MESSAGE_INFO );
	}

	FOR_ALL_XML( xroot, xws, "Workspace" ) {
		FOR_ALL_XML( xws, xcol, "Column" ) {
			FOR_ALL_XML( xcol, xsub, "Subcolumn" ) {
				FOR_ALL_XML( xsub, xrow, "Row" ) {
					workspacegroup_rename_row_node( ws, 
						state, IOBJECT( col )->name, 
						xrow );
				} FOR_ALL_XML_END
			} FOR_ALL_XML_END
		} FOR_ALL_XML_END
	} FOR_ALL_XML_END

	FOR_ALL_XML( xroot, xws, "Workspace" ) {
		FOR_ALL_XML( xws, xcol, "Column" ) {
			FOR_ALL_XML( xcol, xsub, "Subcolumn" ) {
				FOR_ALL_XML( xsub, xrow, "Row" ) {
					if( !model_new_xml( state,
						MODEL( col->scol ),
						xrow ) )
						return( FALSE );
				} FOR_ALL_XML_END
			} FOR_ALL_XML_END
		} FOR_ALL_XML_END
	} FOR_ALL_XML_END

	return( TRUE ); 
}

static gboolean
workspacegroup_top_load( Filemodel *filemodel,
	ModelLoadState *state, Model *parent, xmlNode *xroot )
{
	Workspacegroup *wsg = WORKSPACEGROUP( filemodel );

	xmlNode *xnode;
	char name[FILENAME_MAX];

#ifdef DEBUG
	printf( "workspacegroup_top_load: from %s\n", state->filename );
#endif /*DEBUG*/

	/* The top node should be the first workspace. Get the filename this
	 * workspace was saved as so we can work out how to rewrite embedded
	 * filenames.
	 *
	 * The filename field can be missing. 
	 */
	if( (xnode = get_node( xroot, "Workspace" )) &&
		get_sprop( xnode, "filename", name, FILENAME_MAX ) ) {
		char *new_dir;

		/* The old filename could be non-native, so we must rewrite 
		 * to native form first so g_path_get_dirname() can work.
		 */
		path_compact( name );

		state->old_dir = g_path_get_dirname( name ); 

		new_dir = g_path_get_dirname( state->filename_user );
		path_rewrite_add( state->old_dir, new_dir, FALSE );
		g_free( new_dir );
	}

	switch( wsg->load_type ) {
	case WORKSPACEGROUP_LOAD_NEW:
		if( !workspacegroup_load_new( wsg, state, xroot ) )
			return( FALSE );
		break;

	case WORKSPACEGROUP_LOAD_COLUMNS:
		if( !workspacegroup_load_columns( wsg, state, xroot ) )
			return( FALSE );
		break;

	case WORKSPACEGROUP_LOAD_ROWS:
		if( !workspacegroup_load_rows( wsg, state, xroot ) )
			return( FALSE );
		break;

	default:
		g_assert( FALSE );
	}

	return( FILEMODEL_CLASS( parent_class )->top_load( filemodel, 
		state, parent, xnode ) );
}

static gboolean
workspacegroup_top_save( Filemodel *filemodel, const char *filename )
{
	gboolean result;

#ifdef DEBUG
	printf( "workspacegroup_top_save: %s to %s\n",
		NN( IOBJECT( filemodel )->name ), filename );
#endif /*DEBUG*/

	if( (result = FILEMODEL_CLASS( parent_class )->
		top_save( filemodel, filename )) )
		/* This will add save-as files to recent too. Don't note
		 * auto_load on recent, since it won't have been loaded by the
		 * user.
		 */
		if( !filemodel->auto_load )
			mainw_recent_add( &mainw_recent_workspace, filename );

	return( result );
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
 
	if( !filemodel_top_save( FILEMODEL( wsg ), retain_files[retain_next] ) )
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

static void
workspacegroup_class_init( WorkspacegroupClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	FilemodelClass *filemodel_class = (FilemodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = workspacegroup_dispose;
	gobject_class->finalize = workspacegroup_finalize;

	iobject_class->user_name = _( "Workspace" );

	/* ->load() is done by workspace_top_load().
	 */
	model_class->view_new = workspacegroup_view_new;
	model_class->save = workspacegroup_save;

	filemodel_class->filetype = filesel_type_workspace;
	filemodel_class->top_load = workspacegroup_top_load;
	filemodel_class->top_save = workspacegroup_top_save;
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
	printf( "workspacegroup_new:\n" ); 
#endif /*DEBUG*/

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

	if( !(wsg = workspacegroup_new( wsr )) )
		return( NULL );
	iobject_set( IOBJECT( wsg ), name, _( "Default empty workspace" ) );
	(void) workspacegroup_workspace_pick( wsg ); 
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
	char name[FILENAME_MAX];

	if( !(wsg = workspacegroup_new( wsr )) )
		return( NULL );

	workspacegroup_set_load_type( wsg, WORKSPACEGROUP_LOAD_NEW );
	if( !filemodel_load_all( FILEMODEL( wsg ), 
		MODEL( wsr ), filename, filename_user ) ) 
		return( NULL );

	filemodel_set_filename( FILEMODEL( wsg ), filename_user );
	filemodel_set_modified( FILEMODEL( wsg ), FALSE );

	name_from_filename( filename_user, name );
	iobject_set( IOBJECT( wsg ), name, NULL );

	return( wsg );
}

/* New workspacegroup from a file.
 */
Workspacegroup *
workspacegroup_new_from_openfile( Workspaceroot *wsr, iOpenFile *of )
{
	Workspacegroup *wsg;
	char name[FILENAME_MAX];

#ifdef DEBUG
	printf( "workspacegroup_new_from_openfile: %s\n", of->fname );
#endif /*DEBUG*/

	if( !(wsg = workspacegroup_new( wsr )) )
		return( NULL );

	workspacegroup_set_load_type( wsg, WORKSPACEGROUP_LOAD_NEW );
	if( !filemodel_load_all_openfile( FILEMODEL( wsg ), 
		MODEL( wsr ), of ) ) {
		g_object_unref( G_OBJECT( wsg ) );
		return( NULL );
	}

	filemodel_set_filename( FILEMODEL( wsg ), of->fname );
	filemodel_set_modified( FILEMODEL( wsg ), FALSE );

	name_from_filename( of->fname, name );
	iobject_set( IOBJECT( wsg ), name, NULL );

	return( wsg );
}

/* Merge into workspacegroup as a set of new workspaces.
 */
gboolean
workspacegroup_merge_workspaces( Workspacegroup *wsg, const char *filename )
{
	workspacegroup_set_load_type( wsg, WORKSPACEGROUP_LOAD_NEW );
	if( !filemodel_load_all( FILEMODEL( wsg ), MODEL( wsg->wsr ), 
		filename, NULL ) ) 
		return( FALSE );

	filemodel_set_modified( FILEMODEL( wsg ), TRUE );

	return( TRUE );
}

/* Merge into the current workspace as a set of columns. 
 */
gboolean
workspacegroup_merge_columns( Workspacegroup *wsg, const char *filename )
{
	Workspace *ws;

	if( (ws = workspacegroup_get_workspace( wsg )) ) 
		column_set_offset( 
			IM_RECT_RIGHT( &ws->area ) + WORKSPACEVIEW_MARGIN_LEFT,
			WORKSPACEVIEW_MARGIN_TOP );

	workspacegroup_set_load_type( wsg, WORKSPACEGROUP_LOAD_COLUMNS );
	if( !filemodel_load_all( FILEMODEL( wsg ), MODEL( wsg->wsr ), 
		filename, NULL ) ) 
		return( FALSE );

	filemodel_set_modified( FILEMODEL( wsg ), TRUE );

	return( TRUE );
}

/* Merge into the current workspace as a set of rows.  
 */
gboolean
workspacegroup_merge_rows( Workspacegroup *wsg, const char *filename )
{
	workspacegroup_set_load_type( wsg, WORKSPACEGROUP_LOAD_ROWS );
	if( !filemodel_load_all( FILEMODEL( wsg ), MODEL( wsg->wsr ), 
		filename, NULL ) ) 
		return( FALSE );

	filemodel_set_modified( FILEMODEL( wsg ), TRUE );

	return( TRUE );
}

/* Save just the selected objects in the current workspace. 
 */
gboolean
workspacegroup_save_selected( Workspacegroup *wsg, const char *filename )
{
	workspacegroup_set_save_type( wsg, WORKSPACEGROUP_SAVE_SELECTED );
	if( !filemodel_top_save( FILEMODEL( wsg ), filename ) ) {
		unlinkf( "%s", filename );

		return( FALSE );
	}

	return( TRUE );
}

/* Save just the current workspace. 
 */
gboolean
workspacegroup_save_current( Workspacegroup *wsg, const char *filename )
{
	workspacegroup_set_save_type( wsg, WORKSPACEGROUP_SAVE_WORKSPACE );
	if( !filemodel_top_save( FILEMODEL( wsg ), filename ) ) {
		unlinkf( "%s", filename );

		return( FALSE );
	}

	return( TRUE );
}

/* Save an entire workspacegroup.
 */
gboolean
workspacegroup_save_all( Workspacegroup *wsg, const char *filename )
{
	workspacegroup_set_save_type( wsg, WORKSPACEGROUP_SAVE_ALL );
	if( !filemodel_top_save( FILEMODEL( wsg ), filename ) ) {
		unlinkf( "%s", filename );

		return( FALSE );
	}

	return( TRUE );
}

Workspacegroup *
workspacegroup_duplicate( Workspacegroup *wsg )
{
	Workspaceroot *wsr = wsg->wsr;

	Workspacegroup *new_wsg;
	char filename[FILENAME_MAX];

	if( !temp_name( filename, "ws" ) ||
		!workspacegroup_save_all( wsg, filename ) ) 
		return( NULL );

	if( !(new_wsg = workspacegroup_new_from_file( wsr, 
		filename, FILEMODEL( wsg )->filename )) ) {
		unlinkf( "%s", filename );
		return( NULL );
	}
	unlinkf( "%s", filename );

	return( new_wsg );
}
