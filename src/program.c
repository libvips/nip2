/* program window
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
#define DEBUG
#define DEBUG_TREE
 */

#include "ip.h"

/* Keep tools/kits in a treestore. Also pointers to managed objects.
 */
enum {
	NAME_COLUMN,			/* Kit or tool name */
	NAME_I18N_COLUMN,		/* Localised tool name */
	TOOL_POINTER_COLUMN,		/* Pointer to tool */
	KIT_POINTER_COLUMN,		/* Pointer to kit (if no tool) */
	N_COLUMNS
};

static iWindowClass *parent_class = NULL;

static GSList *program_all = NULL;

static GtkWidget *program_menu = NULL;

static Model *
program_get_selected( Program *program )
{
	Model *model;

	if( program->tool )
		model = MODEL( program->tool );
	else if( program->kit )
		model = MODEL( program->kit );
	else 
		model = NULL;

	return( model );
}

static void
program_info( Program *program, BufInfo *buf )
{
	Model *model = program_get_selected( program );

	buf_appendf( buf, _( "Edit window" ) );
	buf_appendf( buf, "\n" );
	buf_appendf( buf, "dirty = \"%s\"\n", bool_to_char( program->dirty ) );
	buf_appendf( buf, "\n" );

	if( model ) {
		iobject_info( IOBJECT( model ), buf );
		buf_appendf( buf, "\n" );
	}
}

gboolean
my_strcmp( const char *a, const char *b )
{
	if( a == b )
		return( 0 );
	if( !a )
		return( -1 );
	if( !b )
		return( -1 );

	return( strcmp( a, b ) );
}

/* Remove this and any subsequent nodes at this level.
 */
static void
program_refresh_trim( Program *program, GtkTreePath *path )
{
	GtkTreeIter iter;

	while( gtk_tree_model_get_iter( GTK_TREE_MODEL( program->store ), 
		&iter, path ) ) {
#ifdef DEBUG_TREE
		printf( "program_refresh_trim: removing %s\n",
			gtk_tree_path_to_string ( path ) );
#endif /*DEBUG_TREE*/

		gtk_tree_store_remove( program->store, &iter );
	}
}

static void
program_refresh_update( Program *program, GtkTreePath *path,
	const char *name, const char *name_i18n, Tool *tool, Toolkit *kit )
{
	GtkTreeIter iter;
	
	/* Update, or append if there's nothing to update.
	 */
	if( gtk_tree_model_get_iter( GTK_TREE_MODEL( program->store ), 
		&iter, path ) ) {
		/* Node exists.
 		 */
		char *store_name;
		char *store_name_i18n;
		Tool *store_tool;
		Toolkit *store_kit;

		gtk_tree_model_get( GTK_TREE_MODEL( program->store ), &iter,
			NAME_COLUMN, &store_name,
			NAME_I18N_COLUMN, &store_name_i18n,
			TOOL_POINTER_COLUMN, &store_tool,
			KIT_POINTER_COLUMN, &store_kit,
			-1 );

		if( tool != store_tool ||
			kit != store_kit ||
			my_strcmp( name, store_name ) != 0 ||
			my_strcmp( name_i18n, store_name_i18n ) != 0 ) {
#ifdef DEBUG_TREE
			printf( "program_refresh_update: updating \"%s\"\n",
				name );
#endif /*DEBUG_TREE*/
			gtk_tree_store_set( program->store, 
				&iter,
				NAME_COLUMN, name,
				NAME_I18N_COLUMN, name_i18n,
				TOOL_POINTER_COLUMN, tool,
				KIT_POINTER_COLUMN, kit,
				-1 );
		}

		g_free( store_name );
		g_free( store_name_i18n );

		/* Make sure tool nodes have no children ... this can happen 
		 * after some drags.
		 */
		if( tool && 
			gtk_tree_model_iter_has_child( 
				GTK_TREE_MODEL( program->store ), &iter ) ) {
			GtkTreePath *child_path;

			child_path = gtk_tree_path_copy( path );
			gtk_tree_path_down( child_path );
			program_refresh_trim( program, child_path );
			gtk_tree_path_free( child_path );
		}
	}
	else {
		GtkTreeIter parent_iter;
		GtkTreeIter *piter;

#ifdef DEBUG_TREE
		printf( "program_refresh_update: creating \"%s\"\n", name );
#endif /*DEBUG_TREE*/
 
		/* Get an iter for the parent node, if it exists.
		 */
		if( gtk_tree_path_get_depth( path ) > 1 ) {
			GtkTreePath *parent_path;
	
			parent_path = gtk_tree_path_copy( path );
			gtk_tree_path_up( parent_path );
			gtk_tree_model_get_iter( 
				GTK_TREE_MODEL( program->store ),
				&parent_iter, parent_path );
			gtk_tree_path_free( parent_path );
			piter = &parent_iter;
		}
		else 
			piter = NULL;

		gtk_tree_store_append( program->store, &iter, piter );
		gtk_tree_store_set( program->store, &iter,
			NAME_COLUMN, name,
			NAME_I18N_COLUMN, name_i18n,
			TOOL_POINTER_COLUMN, tool,
			KIT_POINTER_COLUMN, kit,
			-1 );
	}
}

static void *
program_refresh_tool( Tool *tool, Program *program, GtkTreePath *path )
{
	if( tool->toolitem )
		program_refresh_update( program, path,
			IOBJECT( tool )->name, tool->toolitem->name,
			tool, tool->kit );
	else
		program_refresh_update( program, path,
			IOBJECT( tool )->name, NULL,
			tool, tool->kit );

	gtk_tree_path_next( path );

	return( NULL );
}

static void *
program_refresh_kit( Toolkit *kit, Program *program, GtkTreePath *path )
{
	program_refresh_update( program, path,
		IOBJECT( kit )->name, NULL, NULL, kit );

	gtk_tree_path_down( path );
	toolkit_map( kit, 
		(tool_map_fn) program_refresh_tool, program, path );

	/* Remove any unused tool nodes.
	 */
	program_refresh_trim( program, path );

	gtk_tree_path_up( path );

	gtk_tree_path_next( path );

	return( NULL );
}

/* Update the title.
 */
static void
program_title( Program *program )
{
	BufInfo buf;
	char txt[512];

	buf_init_static( &buf, txt, 512 );
	buf_appendf( &buf, IOBJECT( program->kitg )->name );
	if( program->kit ) {
		buf_appendf( &buf, " - %s", IOBJECT( program->kit )->name );

		if( FILEMODEL( program->kit )->modified ) {
			buf_appendf( &buf, " [" );
			buf_appendf( &buf, _( "modified" ) );
			buf_appendf( &buf, "]" );
		}
	}
	if( program->tool ) {
		buf_appendf( &buf, " - %s", IOBJECT( program->tool )->name );

		if( program->dirty ) {
			buf_appendf( &buf, " [" );
			buf_appendf( &buf, _( "modified" ) );
			buf_appendf( &buf, "]" );
		}
	}

	iwindow_set_title( IWINDOW( program ), buf_all( &buf ) );
}

typedef struct _ProgramRowLookupInfo {
	Program *program;

	Model *model;
	GtkTreeIter *return_iter;
	gboolean found;
} ProgramRowLookupInfo;

static gboolean
program_row_lookup_sub( GtkTreeModel *model,
	GtkTreePath *path, GtkTreeIter *iter, ProgramRowLookupInfo *info )
{
	Tool *tool;
	Toolkit *kit;

	gtk_tree_model_get( model, iter,
		TOOL_POINTER_COLUMN, &tool,
		KIT_POINTER_COLUMN, &kit,
		-1 );

	if( (void *) tool == (void *) info->model || 
		(void *) kit == (void *) info->model ) {
		*info->return_iter = *iter;
		info->found = TRUE;
		return( TRUE );
	}

	return( FALSE );
}

/* Point return_iter at the row containing a pointer to the Model.
 */
static gboolean
program_row_lookup( Program *program, Model *model, GtkTreeIter *return_iter )
{
	ProgramRowLookupInfo info;

	info.program = program;
	info.model = model;
	info.return_iter = return_iter;
	info.found = FALSE;

	gtk_tree_model_foreach( GTK_TREE_MODEL( program->store ),
		(GtkTreeModelForeachFunc) program_row_lookup_sub, &info );

	return( info.found );
}

static gboolean
program_refresh_timeout( Program *program )
{
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkTreeSelection *select = 
		gtk_tree_view_get_selection( GTK_TREE_VIEW( program->tree ) );
	Model *model = program_get_selected( program );

	program->refresh_timeout = 0;

#ifdef DEBUG
	printf( "program_refresh_timeout\n" );
#endif /*DEBUG*/

	/* Block insert/delete/select signals.
	 */
	g_signal_handler_block( G_OBJECT( program->store ), 
		program->row_deleted_sid );
	g_signal_handler_block( G_OBJECT( program->store ), 
		program->row_inserted_sid );
	g_signal_handler_block( G_OBJECT( select ), 
		program->select_changed_sid );

	/* Rebuild the tree widget.
	 */
	path = gtk_tree_path_new();
	gtk_tree_path_down( path );
	toolkitgroup_map( program->kitg,
		(toolkit_map_fn) program_refresh_kit, program, path );

	/* Remove any unused kit nodes.
	 */
	program_refresh_trim( program, path );

	gtk_tree_path_free( path );

	g_signal_handler_unblock( G_OBJECT( program->store ), 
		program->row_inserted_sid );
	g_signal_handler_unblock( G_OBJECT( program->store ), 
		program->row_deleted_sid );

	/* Update title bar.
	 */
	program_title( program );

	/* Scroll to current kit or tool.
	 */
	if( model &&
		program_row_lookup( program, model, &iter ) ) {

		path = gtk_tree_model_get_path( 
			GTK_TREE_MODEL( program->store ), &iter );

		/* Only expand tools ... we want to be able to select kits
		 * without expansion.
		 */
		if( IS_TOOL( model ) )
			gtk_tree_view_expand_to_path( 
				GTK_TREE_VIEW( program->tree ),
				path );

		gtk_tree_view_set_cursor( GTK_TREE_VIEW( program->tree ),
			path, NULL,
			FALSE );

		gtk_tree_path_free( path );
	}
	else 
		gtk_tree_selection_unselect_all( select );

	g_signal_handler_unblock( G_OBJECT( select ), 
		program->select_changed_sid );

	return( FALSE );
}

/* Schedule an update for all our widgets.
 */
static void
program_refresh( Program *program )
{
	IM_FREEF( g_source_remove, program->refresh_timeout );
	
	/* 1ms to make sure we run after idle (is this right?)
	 */
	program->refresh_timeout = g_timeout_add( 1, 
		(GSourceFunc) program_refresh_timeout, program );
}

/* Break the tool & kit links.
 */
static void
program_detach( Program *program )
{
	if( program->tool ) {
		program->pos = -1;
		FREESID( program->tool_destroy_sid, program->tool );
		program->tool = NULL;
	}

	if( program->kit ) {
		FREESID( program->kit_destroy_sid, program->kit );
		program->kit = NULL;
	}

	program_refresh( program );
}

static void
program_find_reset( Program *program )
{
	FREESID( program->find_sym_destroy_sid, program->find_sym );
	program->find_sym = NULL;
	program->find_start = 0;
	program->find_end = 0;
}

static void
program_find_destroy_cb( Symbol *sym, Program *program )
{
	program_find_reset( program );
}

static void
program_find_note( Program *program, Symbol *sym, int start, int end )
{
	program_find_reset( program );

	program->find_sym = sym;
	program->find_sym_destroy_sid = 
		g_signal_connect( G_OBJECT( sym ), "destroy",
			G_CALLBACK( program_find_destroy_cb ), program );
	program->find_start = start;
	program->find_end = end;
}

static gboolean
program_find_pos( Program *program, const char *text, int *start, int *end )
{
#ifdef HAVE_REGEXEC
	if( program->regexp ) {
		regmatch_t matches[1];

		if( !regexec( program->comp, text, 1, matches, 0 ) ) {
			*start = matches[0].rm_so;
			*end = matches[0].rm_eo;

			return( TRUE );
		}
	}
	else 
#endif /*HAVE_REGEXEC*/
	if( program->csens ) {
		char *p;

		if( (p = strstr( text, program->search )) ) {
			*start = p - text;
			*end = *start + strlen( program->search );

			return( TRUE );
		}
	}
	else {
		char *p;

		if( (p = my_strcasestr( text, program->search )) ) {
			*start = p - text;
			*end = *start + strlen( program->search );

			return( TRUE );
		}
	}

	return( FALSE );
}

static void *
program_find_tool( Tool *tool, Program *program, gboolean *skipping )
{
	Symbol *sym;

	if( tool->type != TOOL_SYM )
		return( NULL );
	sym = tool->sym;

	/* In search mode? Check if we've found the start point.
	 */
	if( *skipping ) {
		if( sym == program->find_sym || !program->find_sym )
			*skipping = FALSE;
	}

	/* Reached start point? Check from start onwards.
	 */
	if( !*skipping ) {
		if( sym->expr && sym->expr->compile && 
			program->find_start < 
				strlen( sym->expr->compile->text ) ) {
			int start, end;

			if( program_find_pos( program, 
				sym->expr->compile->text + program->find_start, 
				&start, &end ) ) {
				program_find_note( program, sym, 
					start + program->find_start, 
					end + program->find_start );
				return( tool );
			}
		}

		program_find_reset( program );
	}

	return( NULL );
}

static void *
program_find_toolkit( Toolkit *kit, Program *program, gboolean *skipping )
{
	return( icontainer_map( ICONTAINER( kit ), 
		(icontainer_map_fn) program_find_tool, program, &skipping ) );
}

static gboolean
program_find( Program *program )
{
	gboolean skipping = TRUE;

	if( toolkitgroup_map( program->kitg,
		(toolkit_map_fn) program_find_toolkit, program, &skipping ) )
		return( TRUE );

	return( FALSE );
}

static void
program_destroy( GtkObject *object )
{
	Program *program;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_PROGRAM( object ) );

	program = PROGRAM( object );

#ifdef DEBUG
	printf( "program_destroy\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	program_detach( program );
	UNREF( program->store );
	FREESID( program->kitgroup_changed_sid, program->kitg );
	FREESID( program->kitgroup_destroy_sid, program->kitg );

	IM_FREEF( g_free, program->search );
#ifdef HAVE_REGEXEC
	IM_FREEF( regfree, program->comp );
#endif /*HAVE_REGEXEC*/

	program_find_reset( program );

	IM_FREEF( g_source_remove, program->refresh_timeout );

	program_all = g_slist_remove( program_all, program );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
program_edit_dia_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Tool *tool = TOOL( client );
	Stringset *ss = STRINGSET( iwnd );
	StringsetChild *name = stringset_child_get( ss, _( "Name" ) );
	StringsetChild *file = stringset_child_get( ss, _( "Filename" ) );

	char name_text[1024];
	char file_text[1024];

	if( !get_geditable_string( name->entry, name_text, 1024 ) ||
		!get_geditable_filename( file->entry, file_text, 1024 ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( !tool_new_dia( tool->kit, 
		ICONTAINER( tool )->pos, name_text, file_text ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	nfn( sys, IWINDOW_TRUE );
}

static void
program_edit_dia( Program *program, Tool *tool )
{
	GtkWidget *ss = stringset_new();

	assert( tool->type == TOOL_DIA );

	stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), IOBJECT( tool )->name, _( "Menu item text" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Filename" ), FILEMODEL( tool )->filename, 
		_( "Load column from this file" ) );

	iwindow_set_title( IWINDOW( ss ), _( "Edit Column Item \"%s\"" ), 
		IOBJECT( tool )->name );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, tool );
	idialog_add_ok( IDIALOG( ss ), 
		program_edit_dia_done_cb, _( "Set column item" ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( program ) );
	idialog_set_iobject( IDIALOG( ss ), IOBJECT( tool ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );
}

static void
program_edit_object_cb( GtkWidget *menu, Program *program )
{
	Model *model = program_get_selected( program );

	if( model && IS_TOOL( model ) && TOOL( model )->type == TOOL_DIA ) 
		program_edit_dia( program, program->tool );
}

static gboolean
program_is_saveable( Model *model )
{
	if( !IS_TOOLKIT( model ) ) {
		error_top( _( "Unable to save." ) );
		error_sub( _( "You can only save toolkits, not tools." ) );
		return( FALSE );
	}

	if( IS_TOOLKIT( model ) && TOOLKIT( model )->pseudo ) {
		error_top( _( "Unable to save." ) );
		error_sub( _( "You can't save auto-generated toolkits." ) );
		return( FALSE );
	}

	return( TRUE );
}

static void
program_save_object_cb( GtkWidget *menu, Program *program )
{
	Model *model = program_get_selected( program );

	if( model ) {
		if( program_is_saveable( model ) ) 
			filemodel_inter_save( IWINDOW( program ), 
				FILEMODEL( model ) );
		else 
			box_alert( GTK_WIDGET( program ) );
	}
}

static void
program_saveas_object_cb( GtkWidget *menu, Program *program ) 
{
	Model *model = program_get_selected( program );

	if( model ) {
		if( program_is_saveable( model ) ) 
			filemodel_inter_saveas( IWINDOW( program ), 
				FILEMODEL( model ) );
		else
			box_alert( GTK_WIDGET( program ) );
	}
}

static void
program_remove_object_cb( GtkWidget *menu, Program *program ) 
{
	Model *model = program_get_selected( program );

	if( model )
		model_check_destroy( GTK_WIDGET( program ), model );
}

static void
program_class_init( ProgramClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	GtkWidget *pane;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = program_destroy;

	/* Create signals.
	 */

	/* Init methods.
	 */

	pane = program_menu = popup_build( _( "Toolkit menu" ) );
	popup_add_but( pane, _( "_Edit" ), 
		POPUP_FUNC( program_edit_object_cb ) );
	popup_add_but( pane, GTK_STOCK_SAVE,
		POPUP_FUNC( program_save_object_cb ) );
	popup_add_but( pane, GTK_STOCK_SAVE_AS,
		POPUP_FUNC( program_saveas_object_cb ) );
	menu_add_sep( pane );
	popup_add_but( pane, GTK_STOCK_DELETE,
		POPUP_FUNC( program_remove_object_cb ) );
}

/* Some kit/tool has changed ... update everything.
 */
static void
program_kitgroup_changed( Model *model, Program *program )
{
#ifdef DEBUG
	printf( "program_kitgroup_changed:\n" );
#endif /*DEBUG*/

	program_refresh( program );
}

static void
program_kitgroup_destroy( Model *model, Program *program )
{
#ifdef DEBUG
	printf( "program_kitgroup_destroy:\n" );
#endif /*DEBUG*/

	/* Our toolkitgroup has gone! Give up on the world.
	 */
	program->kitgroup_changed_sid = 0;
	program->kitgroup_destroy_sid = 0;

	iwindow_kill( IWINDOW( program ) );
}

static void
program_init( Program *program )
{
	program->kitg = NULL;

	program->text = NULL;
	program->dirty = FALSE;
	program->text_hash = 0;
	program->tree = NULL;
	program->store = NULL;
	program->pane_position = PROGRAM_PANE_POSITION;
	program->refresh_timeout = 0;

	program->kitgroup_changed_sid = 0;
	program->kitgroup_destroy_sid = 0;

	program->kit = NULL;
	program->kit_destroy_sid = 0;

	program->tool = NULL;
	program->pos = -1;
	program->tool_destroy_sid = 0;

	program->search = NULL;
	program->csens = FALSE;
	program->regexp = FALSE;
	program->fromtop = TRUE;
#ifdef HAVE_REGEXEC
	program->comp = NULL;
#endif /*HAVE_REGEXEC*/
}

GtkType
program_get_type( void )
{
	static GtkType program_type = 0;

	if( !program_type ) {
		static const GtkTypeInfo info = {
			"Program",
			sizeof( Program ),
			sizeof( ProgramClass ),
			(GtkClassInitFunc) program_class_init,
			(GtkObjectInitFunc) program_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		program_type = gtk_type_unique( TYPE_IWINDOW, &info );
	}

	return( program_type );
}

/* The kit we have selected has been destroyed.
 */
static void
program_kit_destroy( Toolkit *kit, Program *program )
{
#ifdef DEBUG
	printf( "program_kit_destroy:\n" );
#endif /*DEBUG*/

	assert( program->kit == kit );

	program_detach( program );
	program_refresh( program );
}

static void
program_set_text( Program *program, const char *text, gboolean editable )
{
	guint text_hash = g_str_hash( text );

	if( text_hash != program->text_hash ) {
		text_view_set_text( GTK_TEXT_VIEW( program->text ), 
			text, editable );
		program->text_hash = text_hash;
	}

	program->dirty = FALSE;
}

/* Swap text for text for tool.
 */
static void
program_set_text_tool( Program *program, Tool *tool )
{
	char str[MAX_STRSIZE];
	BufInfo buf;

	switch( tool->type ) {
	case TOOL_DIA:
	case TOOL_SEP:
		program_set_text( program, "", FALSE );
		break;

	case TOOL_SYM:
		switch( tool->sym->type ) {
		case SYM_EXTERNAL:
			buf_init_static( &buf, str, MAX_STRSIZE );
			vips_usage( &buf, tool->sym->function );
			program_set_text( program, buf_all( &buf ), FALSE );
			break;

		case SYM_BUILTIN:
			buf_init_static( &buf, str, MAX_STRSIZE );
			builtin_usage( &buf, tool->sym->builtin );
			program_set_text( program, buf_all( &buf ), FALSE );
			break;

		case SYM_VALUE:
			program_set_text( program, 
				tool->sym->expr->compile->text, TRUE );
			break;
		
		default:
			assert( FALSE );
		}
		break;

	default:
		assert( FALSE );
	}
}

/* The sym we are editing has been destroyed.
 */
static void
program_tool_destroy( Tool *tool, Program *program )
{
#ifdef DEBUG
	printf( "program_tool_destroy:\n" );
#endif /*DEBUG*/

	assert( program->tool == tool );

	program_detach( program );
	program_set_text( program, "", TRUE );
	program_refresh( program );
}

/* Pick a kit ... but don't touch the text yet. 
 */
static void
program_select_kit_sub( Program *program, Toolkit *kit )
{
	/* None? Pick "untitled".
	 */
	if( !kit )
		kit = toolkit_by_name( program->kitg, "untitled" );

	program_detach( program );

	if( kit ) {
		program->kit = kit;
		program->kit_destroy_sid = g_signal_connect( G_OBJECT( kit ), 
			"destroy", G_CALLBACK( program_kit_destroy ), program );
	}

	program_refresh( program );
}

/* Select a new kit in the tree. 
 */
static void
program_select_kit( Program *program, Toolkit *kit )
{
	program_select_kit_sub( program, kit );
	program_set_text( program, "", TRUE );
	program_refresh( program );
}

/* Select a tool in the tree. 
 */
static void
program_select_tool( Program *program, Tool *tool )
{
	program_detach( program );

	if( tool ) {
		program_select_kit_sub( program, tool->kit );

		program->tool = tool;
		program->pos = ICONTAINER( tool )->pos;
		program->tool_destroy_sid = g_signal_connect( G_OBJECT( tool ), 
			"destroy", 
			G_CALLBACK( program_tool_destroy ), program );

		program_set_text_tool( program, tool );
	}

	program_refresh( program );
}

static char *
program_get_text( Program *program )
{
	GtkTextView *text_view = GTK_TEXT_VIEW( program->text );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	GtkTextIter start_iter;
	GtkTextIter end_iter;
	char *text;

	gtk_text_buffer_get_start_iter( text_buffer, &start_iter );
	gtk_text_buffer_get_end_iter( text_buffer, &end_iter );
	text = gtk_text_buffer_get_text( text_buffer, 
		&start_iter, &end_iter, FALSE ); 

	return( text );
}

/* Read and parse the text.
 */
static gboolean
program_parse( Program *program )
{
	char *txt;
	char buffer[MAX_STRSIZE];
	Compile *compile;

	if( !program->dirty )
		return( TRUE );

	/* Irritatingly, we need to append a ';'. Also, update the hash, so we
	 * don't set the same text back again if we can help it.
	 */
	txt = program_get_text( program );
	program->text_hash = g_str_hash( txt );
	im_snprintf( buffer, MAX_STRSIZE, "%s;", txt );
	IM_FREEF( g_free, txt );

	if( strspn( buffer, WHITESPACE ";" ) == strlen( buffer ) ) 
		return( TRUE );

	/* Make sure we've got a kit.
	 */
	if( !program->kit )
		program_select_kit_sub( program, program->kit );
	compile = program->kit->kitg->root->expr->compile;

#ifdef DEBUG
	printf( "program_parse: parsing to kit \"%s\", pos %d\n",
		IOBJECT( program->kit )->name, program->pos  );
#endif /*DEBUG*/

	/* ... and parse the new text into it.
	 */
	attach_input_string( buffer );
	if( !parse_onedef( program->kit, program->pos ) ) {
		text_view_select_text( GTK_TEXT_VIEW( program->text ), 
			input_state.charpos - yyleng, input_state.charpos );
		return( FALSE );
	}

	program->dirty = FALSE;
	filemodel_set_modified( FILEMODEL( program->kit ), TRUE );

	/* Reselect last_sym, the last thing the parser saw. 
	 */
	if( compile->last_sym && compile->last_sym->tool ) 
		program_select_tool( program, compile->last_sym->tool );

	symbol_recalculate_all();

	return( TRUE );
}

static void
program_tool_new_action_cb( GtkAction *action, Program *program )
{
	/* Existing text changed? Parse it.
	 */
	if( program->dirty && !program_parse( program ) ) {
		box_alert( GTK_WIDGET( program ) );
		return;
	}

	program_select_kit( program, program->kit );
}

static void
program_toolkit_new_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Stringset *ss = STRINGSET( iwnd );
	StringsetChild *name = stringset_child_get( ss, _( "Name" ) );
	StringsetChild *caption = stringset_child_get( ss, _( "Caption" ) );
	Program *program = PROGRAM( client );

	Toolkit *kit;
	BufInfo buf;
	char str[1024];
	char name_text[1024];
	char caption_text[1024];

	if( !get_geditable_name( name->entry, name_text, 1024 ) ||
		!get_geditable_string( caption->entry, caption_text, 1024 ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	/* Make a filename from the name ... user start directory.
	 */
	buf_init_static( &buf, str, 1024 );
	buf_appendf( &buf, "$SAVEDIR" G_DIR_SEPARATOR_S 
		"start" G_DIR_SEPARATOR_S "%s.def", 
		name_text );
	kit = toolkit_new_filename( main_toolkitgroup, buf_all( &buf ) );

	/* Set caption.
	 */
	if( strspn( caption_text, WHITESPACE ) != strlen( caption_text ) )
		iobject_set( IOBJECT( kit ), NULL, caption_text );
	else
		iobject_set( IOBJECT( kit ), NULL, "untitled" );

	program_select_kit( program, kit );

	nfn( sys, IWINDOW_TRUE );
}

static void
program_toolkit_new_action_cb( GtkAction *action, Program *program )
{
	GtkWidget *ss = stringset_new();

	stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), "", _( "Set toolkit name here" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Caption" ), "", _( "Set toolkit caption here" ) );
	iwindow_set_title( IWINDOW( ss ), _( "New Toolkit" ) );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, program );
	idialog_add_ok( IDIALOG( ss ), 
		program_toolkit_new_done_cb, _( "Create" ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( program ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );
}

static gboolean
program_check_kit( Program *program )
{
	if( !program->kit ) {
		box_info( GTK_WIDGET( program ), 
			_( "Nothing selected." ),
			_( "No toolkit selected." ) );
		return( FALSE );
	}

	return( TRUE );
}

static void
program_separator_new_action_cb( GtkAction *action, Program *program )
{
	Tool *tool;
	int pos;

	if( !program_check_kit( program ) )
		return;

	pos = icontainer_pos_last( ICONTAINER( program->kit ) );
	tool = tool_new_sep( program->kit, pos + 1 );
	program_select_tool( program, tool );
}

static void
program_column_item_new_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Stringset *ss = STRINGSET( iwnd );
	StringsetChild *name = stringset_child_get( ss, _( "Name" ) );
	StringsetChild *file = stringset_child_get( ss, _( "Filename" ) );
	Program *program = PROGRAM( client );
	Tool *tool;

	int pos;
	char name_text[1024];
	char file_text[1024];

	if( !get_geditable_name( name->entry, name_text, 1024 ) ||
		!get_geditable_filename( file->entry, file_text, 1024 ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	pos = icontainer_pos_last( ICONTAINER( program->kit ) );
	tool = tool_new_dia( program->kit, pos + 1, name_text, file_text );
	program_select_tool( program, tool );

	nfn( sys, IWINDOW_TRUE );
}

static void
program_column_item_new_action_cb( GtkAction *action, Program *program )
{
	GtkWidget *ss;

	if( !program_check_kit( program ) )
		return;

	ss = stringset_new();
	stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), "", _( "Display this name" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Filename" ), "", _( "Load this file" ) );
	iwindow_set_title( IWINDOW( ss ), "New Column Item" );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, program );
	idialog_add_ok( IDIALOG( ss ), 
		program_column_item_new_done_cb, _( "Create" ) );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( program ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );
}

static void
program_program_new_action_cb( GtkAction *action, Program *program )
{
	Program *program2;

	program2 = program_new( program->kitg );

	gtk_widget_show( GTK_WIDGET( program2 ) ); 
}

static void
program_workspace_new_action_cb( GtkAction *action, Program *program )
{
	workspacegroup_workspace_new( main_workspacegroup, 
		GTK_WIDGET( program ) );
}

static void *
program_load_file_fn( Filesel *filesel, 
	const char *filename, Program *program, void *b )
{
	Toolkit *kit;

	if( !(kit = toolkit_new_from_file( main_toolkitgroup, filename )) ) 
		return( filesel );

	program_select_kit( program, kit );

	return( NULL );
}

/* Callback from load browser.
 */
static void
program_load_file_cb( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Program *program = PROGRAM( client );

	if( filesel_map_filename_multi( filesel,
		(FileselMapFn) program_load_file_fn, program, NULL ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	symbol_recalculate_all();

	nfn( sys, IWINDOW_TRUE );
}

static void
program_open_action_cb( GtkAction *action, Program *program )
{
	GtkWidget *filesel = filesel_new();

	iwindow_set_title( IWINDOW( filesel ), _( "Load Definition" ) );
	filesel_set_flags( FILESEL( filesel ), FALSE, FALSE );
	filesel_set_filetype( FILESEL( filesel ), filesel_type_definition, 0 ); 
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( program ) );
	filesel_set_done( FILESEL( filesel ), program_load_file_cb, program );
	filesel_set_multi( FILESEL( filesel ), TRUE );
	iwindow_build( IWINDOW( filesel ) );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

static void
program_save_action_cb( GtkAction *action, Program *program )
{
	if( !program_check_kit( program ) )
		return;

	filemodel_inter_save( IWINDOW( program ), FILEMODEL( program->kit ) );
}

static void
program_save_as_action_cb( GtkAction *action, Program *program )
{
	if( !program_check_kit( program ) )
		return;

	filemodel_inter_saveas( IWINDOW( program ), FILEMODEL( program->kit ) );
}

static void
program_process_action_cb( GtkAction *action, Program *program )
{
	if( !program_parse( program ) )
		box_alert( GTK_WIDGET( program ) );
}

static void
program_reload_menus_cb( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	main_reload();
	symbol_recalculate_all();

	nfn( sys, IWINDOW_TRUE );
}

/* Reload all menus.
 */
static void
program_reload_action_cb( GtkAction *action, Program *program )
{
	box_yesno( GTK_WIDGET( program ),
		program_reload_menus_cb, iwindow_true_cb, NULL,
		iwindow_notify_null, NULL,
		_( "Reload" ), 
		_( "Reload startup objects?" ),
		_( "Would you like to reload all startup menus, workspaces "
		"and plugins now? This may take a few seconds." ) );
}

static void
program_cut_action_cb( GtkAction *action, Program *program )
{
	GtkTextView *text_view = GTK_TEXT_VIEW( program->text );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	GtkClipboard *clipboard = gtk_widget_get_clipboard( 
		GTK_WIDGET( text_view ), GDK_SELECTION_CLIPBOARD );
	gboolean editable = !program->kit || !program->kit->pseudo;

	gtk_text_buffer_cut_clipboard( text_buffer, clipboard, editable );
}

static void
program_copy( Program *program )
{
	GtkTextView *text_view = GTK_TEXT_VIEW( program->text );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	GtkClipboard *clipboard = gtk_widget_get_clipboard( 
		GTK_WIDGET( text_view ), GDK_SELECTION_CLIPBOARD );

	gtk_text_buffer_copy_clipboard( text_buffer, clipboard );
}

static void
program_copy_action_cb( GtkAction *action, Program *program )
{
	program_copy( program );
}

static void
program_paste_action_cb( GtkAction *action, Program *program )
{
	GtkTextView *text_view = GTK_TEXT_VIEW( program->text );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	GtkClipboard *clipboard = gtk_widget_get_clipboard( 
		GTK_WIDGET( text_view ), GDK_SELECTION_CLIPBOARD );
	gboolean editable = !program->kit || !program->kit->pseudo;

	gtk_text_buffer_paste_clipboard( text_buffer, clipboard, NULL,
		editable );
}

static void
program_delete_action_cb( GtkAction *action, Program *program )
{
	GtkTextView *text_view = GTK_TEXT_VIEW( program->text );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	gboolean editable = !program->kit || !program->kit->pseudo;

	gtk_text_buffer_delete_selection( text_buffer, TRUE, editable );
}

static void
program_select_all_action_cb( GtkAction *action, Program *program )
{
	text_view_select_text( GTK_TEXT_VIEW( program->text ), 0, -1 );
}

static void
program_deselect_all_action_cb( GtkAction *action, Program *program )
{
	GtkTextView *text_view = GTK_TEXT_VIEW( program->text );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	GtkTextMark *mark = gtk_text_buffer_get_insert( text_buffer );
	GtkTextIter iter;

	gtk_text_buffer_get_iter_at_mark( text_buffer, &iter, mark );
	gtk_text_buffer_select_range( text_buffer, &iter, &iter );
}

static void
program_remove_tool_action_cb( GtkAction *action, Program *program )
{
	Model *model = program_get_selected( program );

	if( model && IS_TOOL( model ) )
		model_check_destroy( GTK_WIDGET( program ), model );
	else 
		box_info( GTK_WIDGET( program ), 
			_( "No tool selected" ), "%s", "" );
}

static void
program_remove_toolkit_action_cb( GtkAction *action, Program *program )
{
	if( !program_check_kit( program ) )
		return;

	model_check_destroy( GTK_WIDGET( program ), MODEL( program->kit ) );
}

static void
program_find_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Find *find = FIND( iwnd );
	Program *program = PROGRAM( client );

	IM_FREEF( g_free, program->search );

	program->search = 
		gtk_editable_get_chars( GTK_EDITABLE( find->search ), 0, -1 );
	program->csens = GTK_TOGGLE_BUTTON( find->csens )->active;
#ifdef HAVE_REGEXEC
	program->regexp = GTK_TOGGLE_BUTTON( find->regexp )->active;
#endif /*HAVE_REGEXEC*/
	program->fromtop = GTK_TOGGLE_BUTTON( find->fromtop )->active;

#ifdef HAVE_REGEXEC
	if( program->regexp ) {
		int flags = 0;

		if( !program->comp )
			program->comp = INEW( NULL, regex_t );

		if( !program->csens )
			flags |= REG_ICASE;

		if( regcomp( program->comp, program->search, flags ) != 0 ) {
			error_top( _( "Parse error." ) );
			error_sub( _( "Bad regular expression." ) );
			nfn( sys, IWINDOW_ERROR );
			return;
		}
	}
#endif /*HAVE_REGEXEC*/

	if( program->fromtop )
		program_find_reset( program );
	else
		program->find_start += 1;

	if( program_find( program ) ) {
		program_select_tool( program, program->find_sym->tool );
		text_view_select_text( GTK_TEXT_VIEW( program->text ), 
			program->find_start, program->find_end );
	}
	else {
		error_top( _( "Not found." ) );
		error_sub( _( "No match found for \"%s\"." ), program->search );
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	nfn( sys, IWINDOW_TRUE );
}

static void
program_find_action_cb( GtkAction *action, Program *program )
{
	GtkWidget *find = find_new();

	iwindow_set_title( IWINDOW( find ), _( "Find in all Toolkits" ) );
	idialog_set_callbacks( IDIALOG( find ), 
		iwindow_true_cb, NULL, NULL, program );
	idialog_add_ok( IDIALOG( find ), program_find_done_cb, GTK_STOCK_FIND );
	iwindow_set_parent( IWINDOW( find ), GTK_WIDGET( program ) );
	idialog_set_cancel_text( IDIALOG( find ), GTK_STOCK_CLOSE );
	iwindow_build( IWINDOW( find ) );

	if( program->search )
		set_gentry( FIND( find )->search, "%s", program->search );
	set_tooltip( FIND( find )->search, _( "Enter search string here" ) );
        gtk_toggle_button_set_active( 
		GTK_TOGGLE_BUTTON( FIND( find )->csens ), program->csens );
#ifdef HAVE_REGEXEC
        gtk_toggle_button_set_active( 
		GTK_TOGGLE_BUTTON( FIND( find )->regexp ), program->regexp );
#endif /*HAVE_REGEXEC*/
        gtk_toggle_button_set_active( 
		GTK_TOGGLE_BUTTON( FIND( find )->fromtop ), program->fromtop );

	gtk_widget_show( find );
}

static void
program_find_again_action_cb( GtkAction *action, Program *program )
{
	if( program->find_sym ) 
		program->find_start += 1;

	if( program_find( program ) ) {
		program_select_tool( program, program->find_sym->tool );
		text_view_select_text( GTK_TEXT_VIEW( program->text ), 
			program->find_start, program->find_end );
	}
	else
		box_info( GTK_WIDGET( program ), 
			_( "Not found." ),
			_( "No match found." ) );
}

static void
program_goto_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Program *program = PROGRAM( client );
	Stringset *ss = STRINGSET( iwnd );
	StringsetChild *name = stringset_child_get( ss, _( "Name" ) );
	Symbol *sym;
	char name_text[1024];

	if( !get_geditable_string( name->entry, name_text, 1024 ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( !(sym = compile_lookup( program->kitg->root->expr->compile, 
		name_text )) ) {
		error_top( _( "Not found." ) );
		error_sub( _( "No top-level symbol called \"%s\"." ), 
			name_text );
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( !sym->tool ) {
		error_top( _( "Not found." ) );
		error_sub( _( "Symbol \"%s\" has no tool inforation." ), 
			name_text );
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( !program_select( program, MODEL( sym->tool ) ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	nfn( sys, IWINDOW_TRUE );
}

static void
program_goto_action_cb( GtkAction *action, Program *program )
{
	GtkWidget *ss = stringset_new();
	StringsetChild *name;

	name = stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), "", _( "Go to definition of this symbol" ) );

	iwindow_set_title( IWINDOW( ss ), _( "Go to Definition" ) );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, program );
	idialog_add_ok( IDIALOG( ss ), 
		program_goto_done_cb, GTK_STOCK_JUMP_TO );
	idialog_set_pinup( IDIALOG( ss ), TRUE );
	iwindow_set_parent( IWINDOW( ss ), GTK_WIDGET( program ) );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );

	/* Now try to paste the selection into the name widget.
		
		FIXME ... get rid of this, have a right-button menu on the 
		text widget which includes a 'go to def' item.

		or could make sym names into hyperlinks? see text demo example

	 */
	program_copy( program );
	gtk_editable_paste_clipboard( GTK_EDITABLE( name->entry ) );
}

static void
program_info_action_cb( GtkAction *action, Program *program )
{
	BufInfo buf;
	char txt[MAX_STRSIZE];

	buf_init_static( &buf, txt, MAX_STRSIZE );
	program_info( program, &buf );
	box_info( GTK_WIDGET( program ), _( "Object information." ), 
		"%s", buf_all( &buf ) );
}

static void
program_trace_action_cb( GtkAction *action, Program *program )
{
	(void) trace_new();
}

static void
program_errorreport_action_cb( GtkAction *action, Program *program )
{
	iError *ierror;

	ierror = ierror_new( program->kitg );
	gtk_widget_show( GTK_WIDGET( ierror ) ); 

#ifdef DEBUG
	/* Dump VIPS memory usage info for debugging.
	 */
	im__print_all();
#endif /*DEBUG*/
}

static void
program_tool_help_action_cb( GtkAction *action, Program *program )
{
	BufInfo buf;
	char txt[512];

	buf_init_static( &buf, txt, 512 );

	if( program->tool && program->tool->type == TOOL_SYM && 
		program->kit && program->kit->pseudo ) {
		switch( program->tool->sym->type ) {
		case SYM_EXTERNAL:
			buf_appendf( &buf, "file://"
				VIPS_DOCPATH "/man/%s.3.html", 
				IOBJECT( program->tool->sym )->name );
			box_url( GTK_WIDGET( program ), buf_all( &buf ) );
			break;

		case SYM_BUILTIN:
			box_help( GTK_WIDGET( program ), "tb:builtin" );
			break;

		default:
			break;
		}
	}
	else
		box_info( GTK_WIDGET( program ), 
			_( "No documentation available." ),
			_( "On-line documentation is only currently "
			"available for VIPS functions and nip builtins." ) );
}

/* Our actions.
 */
static GtkActionEntry program_actions[] = {
	/* Menu items.
	 */
	{ "FileMenu", NULL, "_File" },
	{ "FileNewMenu", NULL, "_New" },
	{ "EditMenu", NULL, "_Edit" },
	{ "DebugMenu", NULL, "_Debug" },
	{ "HelpMenu", NULL, "_Help" },

	/* Actions.
	 */
	{ "NewTool", 
		GTK_STOCK_NEW, N_( "New _Tool" ), NULL, 
		N_( "Make a new tool" ), 
		G_CALLBACK( program_tool_new_action_cb ) },

	{ "NewToolkit", 
		GTK_STOCK_NEW, N_( "New Tool_kit" ), NULL, 
		N_( "Make a new toolkit" ), 
		G_CALLBACK( program_toolkit_new_action_cb ) },

	{ "NewSeparator", 
		GTK_STOCK_NEW, N_( "New _Separator" ), NULL, 
		N_( "Make a new separator" ), 
		G_CALLBACK( program_separator_new_action_cb ) },

	{ "NewColumnItem", 
		GTK_STOCK_NEW, N_( "New _Column Item" ), NULL, 
		N_( "Make a new column item" ), 
		G_CALLBACK( program_column_item_new_action_cb ) },

	{ "NewProgram", 
		GTK_STOCK_NEW, N_( "New _Program Window" ), NULL, 
		N_( "Make a new program window" ), 
		G_CALLBACK( program_program_new_action_cb ) },

	{ "NewWorkspace", 
		GTK_STOCK_NEW, N_( "New _Workspace" ), NULL, 
		N_( "Make a new workspace" ), 
		G_CALLBACK( program_workspace_new_action_cb ) },

	{ "Open", 
		GTK_STOCK_OPEN, N_( "_Open Toolkit" ), NULL,
		N_( "_Open toolkit" ), 
		G_CALLBACK( program_open_action_cb ) },

	{ "Save", 
		GTK_STOCK_SAVE, N_( "Save Toolkit" ), NULL,
		N_( "_Save toolkit" ), 
		G_CALLBACK( program_save_action_cb ) },

	{ "SaveAs", 
		GTK_STOCK_SAVE_AS, N_( "Save Toolkit _As" ), NULL,
		N_( "Save toolkit as" ), 
		G_CALLBACK( program_save_as_action_cb ) },

	{ "Process", 
		NULL, N_( "_Process" ), NULL,
		N_( "Process text" ), 
		G_CALLBACK( program_process_action_cb ) },

	{ "Reload", 
		NULL, N_( "_Reload Start Stuff" ), NULL,
		N_( "Remove and reload all startup data" ), 
		G_CALLBACK( program_reload_action_cb ) },

	{ "Close", 
		GTK_STOCK_CLOSE, N_( "_Close" ), NULL,
		N_( "Close" ), 
		G_CALLBACK( iwindow_kill_action_cb ) },

	{ "Cut", 
		GTK_STOCK_CUT, N_( "C_ut" ), NULL,
		N_( "Cut selected text" ), 
		G_CALLBACK( program_cut_action_cb ) },

	{ "Copy", 
		GTK_STOCK_COPY, N_( "_Copy" ), NULL,
		N_( "Copy selected text" ), 
		G_CALLBACK( program_copy_action_cb ) },

	{ "Paste", 
		GTK_STOCK_PASTE, N_( "_Paste" ), NULL,
		N_( "Paste selected text" ), 
		G_CALLBACK( program_paste_action_cb ) },

	{ "Delete", 
		GTK_STOCK_DELETE, N_( "_Delete" ), NULL,
		N_( "Delete selected text" ), 
		G_CALLBACK( program_delete_action_cb ) },

	{ "SelectAll", 
		NULL, N_( "Select _All" ), NULL,
		N_( "Select all text" ), 
		G_CALLBACK( program_select_all_action_cb ) },

	{ "DeselectAll", 
		NULL, N_( "Dese_lect All" ), NULL,
		N_( "Deselect all text" ), 
		G_CALLBACK( program_deselect_all_action_cb ) },

	{ "DeleteTool", 
		NULL, N_( "Delete _Tool" ), NULL,
		N_( "Delete current tool" ), 
		G_CALLBACK( program_remove_tool_action_cb ) },

	{ "DeleteToolkit", 
		NULL, N_( "Delete Tool_kit" ), NULL,
		N_( "Delete current toolkit" ), 
		G_CALLBACK( program_remove_toolkit_action_cb ) },

	{ "Find", 
		GTK_STOCK_FIND, N_( "_Find" ), NULL,
		N_( "Find text in toolkits" ), 
		G_CALLBACK( program_find_action_cb ) },

	{ "FindNext", 
		NULL, N_( "Find _Next" ), "<control>G",
		N_( "Find text again" ), 
		G_CALLBACK( program_find_again_action_cb ) },

	{ "JumpTo", 
		GTK_STOCK_JUMP_TO, N_( "_Jump To Definition" ), NULL,
		N_( "Jump to definition" ), 
		G_CALLBACK( program_goto_action_cb ) },

	{ "Info", 
		NULL, N_( "_Info" ), NULL,
		N_( "Info on selected object" ), 
		G_CALLBACK( program_info_action_cb ) },

	{ "Trace", 
		NULL, N_( "_Trace" ), NULL,
		N_( "Make a new trace window" ), 
		G_CALLBACK( program_trace_action_cb ) },

	{ "Errors", 
		NULL, N_( "_Errors" ), NULL,
		N_( "Show all errors" ), 
		G_CALLBACK( program_errorreport_action_cb ) },

	{ "About", 
		NULL, N_( "_About" ), NULL,
		N_( "About this program" ), 
		G_CALLBACK( mainw_about_action_cb ) },

	{ "Guide", 
		GTK_STOCK_HELP, N_( "_Contents" ), "F1",
		N_( "Open the users guide" ), 
		G_CALLBACK( mainw_guide_action_cb ) },

	{ "HelpTool", 
		NULL, N_( "Help on _Tool" ), NULL,
		N_( "View docs for this tool" ), 
		G_CALLBACK( program_tool_help_action_cb ) }
};

static const char *program_menubar_ui_description =
"<ui>"
"  <menubar name='ProgramMenubar'>"
"    <menu action='FileMenu'>"
"      <menu action='FileNewMenu'>"
"        <menuitem action='NewTool'/>"
"        <menuitem action='NewToolkit'/>"
"        <menuitem action='NewSeparator'/>"
"        <menuitem action='NewColumnItem'/>"
"        <menuitem action='NewProgram'/>"
"        <menuitem action='NewWorkspace'/>"
"      </menu>"
"      <menuitem action='Open'/>"
"      <separator/>"
"      <menuitem action='Save'/>"
"      <menuitem action='SaveAs'/>"
"      <separator/>"
"      <menuitem action='Process'/>"
"      <menuitem action='Reload'/>"
"      <separator/>"
"      <menuitem action='Close'/>"
"    </menu>"
"    <menu action='EditMenu'>"
"      <menuitem action='Cut'/>"
"      <menuitem action='Copy'/>"
"      <menuitem action='Paste'/>"
"      <menuitem action='Delete'/>"
"      <separator/>"
"      <menuitem action='SelectAll'/>"
"      <menuitem action='DeselectAll'/>"
"      <separator/>"
"      <menuitem action='DeleteTool'/>"
"      <menuitem action='DeleteToolkit'/>"
"      <separator/>"
"      <menuitem action='Find'/>"
"      <menuitem action='FindNext'/>"
"      <menuitem action='JumpTo'/>"
"      <separator/>"
"      <menuitem action='Info'/>"
"    </menu>"
"    <menu action='DebugMenu'>"
"      <menuitem action='Trace'/>"
"      <menuitem action='Errors'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"      <menuitem action='HelpTool'/>"
"    </menu>"
"  </menubar>"
"</ui>";

gboolean
program_select( Program *program, Model *model )
{
	/* Existing text changed? Parse it.
	 */
	if( program->dirty && !program_parse( program ) ) 
		return( FALSE );

	if( model ) {
		if( IS_TOOL( model ) ) 
			program_select_tool( program, TOOL( model ) );
		else if( IS_TOOLKIT( model ) ) 
			program_select_kit( program, TOOLKIT( model ) );
	}

	return( TRUE );
}

static void
program_text_changed( GtkTextBuffer *buffer, Program *program )
{
	if( !program->dirty ) {
		program->dirty = TRUE;
		program_refresh( program );
	}
}

/* Select a row from an iter.
 */
static void
program_select_row( Program *program, GtkTreeIter *iter )
{
	Tool *tool;
	Toolkit *kit;
	Model *model;

	gtk_tree_model_get( GTK_TREE_MODEL( program->store ), iter,
		TOOL_POINTER_COLUMN, &tool,
		KIT_POINTER_COLUMN, &kit,
		-1 );
	if( tool )
		model = MODEL( tool );
	else
		model = MODEL( kit );

	if( !program_select( program, model ) ) 
		box_alert( GTK_WIDGET( program ) );
}

static void
program_row_collapsed_cb( GtkTreeView *tree, 
	GtkTreeIter *iter, GtkTreePath *path, Program *program )
{
	Toolkit *kit;

#ifdef DEBUG
	printf( "program_row_collapsed_cb:\n" );
	printf( "  path = %s\n", gtk_tree_path_to_string( path ) );
#endif /*DEBUG*/

	gtk_tree_model_get( GTK_TREE_MODEL( program->store ), iter,
		KIT_POINTER_COLUMN, &kit,
		-1 );

	/* If we have collapsed the kit containing the currently selected
	 * tool, the kit will just bounce open again when we refresh the tree.
	 * Unselect the tool.
	 */
	if( program->kit == kit )
		program_select_kit( program, kit );
}

static void
program_selection_changed_cb( GtkTreeSelection *select, Program *program )
{
        GtkTreeIter iter;
        GtkTreeModel *model;

#ifdef DEBUG
	printf( "program_selection_changed_cb:\n" );
#endif /*DEBUG*/

        if( gtk_tree_selection_get_selected( select, &model, &iter ) ) {
#ifdef DEBUG
		printf( "  selection = %s\n",
			gtk_tree_path_to_string (
				gtk_tree_model_get_path( model, &iter ) ) );
#endif /*DEBUG*/

                program_select_row( program, &iter );
	}

	program_refresh( program );
}

static gboolean
program_tree_event_cb( GtkTreeView *tree, GdkEvent *ev, Program *program )
{
	GtkTreePath *path;
	gboolean handled = FALSE;

        if( ev->type == GDK_BUTTON_PRESS && ev->button.button == 3 &&
		gtk_tree_view_get_path_at_pos( tree,
			ev->button.x, ev->button.y, 
			&path, NULL, NULL, NULL ) ) {
		GtkTreeIter iter;

		gtk_tree_model_get_iter( GTK_TREE_MODEL( program->store ),
			&iter, path );
                program_select_row( program, &iter );
		gtk_tree_path_free( path );
		popup_link( GTK_WIDGET( program ), program_menu, NULL );
		popup_show( GTK_WIDGET( program ), ev );

		handled = TRUE;
	}

	return( handled );
}

static void
program_row_inserted_cb( GtkTreeModel *treemodel, 
	GtkTreePath *path, GtkTreeIter *iter, Program *program )
{
	GtkTreeIter iter2;
	GtkTreeIter iter3;

#ifdef DEBUG
	printf( "program_row_inserted_cb:\n" );
	printf( "  path = %s\n", gtk_tree_path_to_string( path ) );
#endif /*DEBUG*/

	program->to_pos = -1;
	program->to_kit = NULL;

	switch( gtk_tree_path_get_depth( path ) ) {
	case 3:
		program->to_pos = gtk_tree_path_get_indices( path )[1];
		gtk_tree_model_iter_parent( GTK_TREE_MODEL( program->store ),
			&iter2, iter );
		gtk_tree_model_iter_parent( GTK_TREE_MODEL( program->store ),
			&iter3, &iter2 );
		gtk_tree_model_get( GTK_TREE_MODEL( program->store ), &iter3,
			KIT_POINTER_COLUMN, &program->to_kit,
			-1 );
		break;

	case 2:
		program->to_pos = gtk_tree_path_get_indices( path )[1];
		gtk_tree_model_iter_parent( GTK_TREE_MODEL( program->store ),
			&iter2, iter );
		gtk_tree_model_get( GTK_TREE_MODEL( program->store ), &iter2,
			KIT_POINTER_COLUMN, &program->to_kit,
			-1 );
		break;

	case 1:
		program->to_pos = -1;
		gtk_tree_model_get( GTK_TREE_MODEL( program->store ), iter,
			KIT_POINTER_COLUMN, &program->to_kit,
			-1 );
		break;

	}

#ifdef DEBUG
	if( program->to_kit ) {
		printf( "  to_kit = " );
		iobject_print( IOBJECT( program->to_kit ) );
	}
	else
		printf( "  to_kit = NULL\n" );
	printf( "  to_pos = %d\n", program->to_pos );
#endif /*DEBUG*/
}

static void
program_row_deleted_cb( GtkTreeModel *treemodel, 
	GtkTreePath *path, Program *program )
{
#ifdef DEBUG
	printf( "program_row_deleted_cb:\n" );
	printf( "  delete path = %s\n", gtk_tree_path_to_string( path ) );
#endif /*DEBUG*/

	if( !program->to_kit || !program->tool ) {
		box_info( GTK_WIDGET( program ), 
			_( "Bad drag." ),
			_( "Sorry, you can only drag tools between toolkits. "
			"You can't reorder toolkits, you can't nest toolkits "
			"and you can't drag tools to the top level." ) );
		return;
	}

#ifdef DEBUG
	printf( "  to_kit = " );
	iobject_print( IOBJECT( program->to_kit ) );
	printf( "  to_pos = %d\n", program->to_pos );
	printf( "  selected tool = " );
	iobject_print( IOBJECT( program->tool ) );
#endif /*DEBUG*/

	/* Move tool.
	 */
	g_object_ref( G_OBJECT( program->tool ) );
	icontainer_child_remove( ICONTAINER( program->tool ) );
	icontainer_child_add( ICONTAINER( program->to_kit ), 
		ICONTAINER( program->tool ), program->to_pos );
	g_object_unref( G_OBJECT( program->tool ) );
	filemodel_set_modified( FILEMODEL( program->to_kit ), TRUE );
	iobject_changed( IOBJECT( program->tool ) );
}

static void
program_edit_map_cb( GtkWidget *widget, Program *program )
{
	GtkClipboard *clipboard = gtk_widget_get_clipboard( 
		GTK_WIDGET( program ), GDK_SELECTION_CLIPBOARD );
	GtkTextView *text_view = GTK_TEXT_VIEW( program->text );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	gboolean editable = !program->kit || !program->kit->pseudo;
	gboolean available = gtk_clipboard_wait_is_text_available( clipboard );
	gboolean selected = gtk_text_buffer_get_selection_bounds( text_buffer,
		NULL, NULL );
	GtkAction *action;

	action = gtk_action_group_get_action( program->action_group, 
		"Paste" );
	g_object_set( G_OBJECT( action ), 
		"sensitive", available && editable,
		NULL );

	action = gtk_action_group_get_action( program->action_group, 
		"Copy" );
	g_object_set( G_OBJECT( action ), 
		"sensitive", selected,
		NULL );

	action = gtk_action_group_get_action( program->action_group, 
		"Cut" );
	g_object_set( G_OBJECT( action ), 
		"sensitive", selected && editable,
		NULL );

	action = gtk_action_group_get_action( program->action_group, 
		"Delete" );
	g_object_set( G_OBJECT( action ), 
		"sensitive", selected && editable,
		NULL );

	action = gtk_action_group_get_action( program->action_group, 
		"DeselectAll" );
	g_object_set( G_OBJECT( action ), 
		"sensitive", selected,
		NULL );
}

static PangoTabArray *
program_tabs_new( void )
{
	const int ntabs = 20;
	const int tab_width = 30;	/* in pixels, about 4 chars */

	PangoTabArray *tabs = pango_tab_array_new( ntabs, TRUE );
	int i;

	for( i = 0; i < ntabs; i++ )
		pango_tab_array_set_tab( tabs, 
			i, PANGO_TAB_LEFT, i * tab_width );

	return( tabs );
}

GtkWidget *
program_text_new( void )
{
	PangoFontDescription *font_desc;
	PangoTabArray *tabs;
	GtkWidget *text;

	text = gtk_text_view_new();
	font_desc = pango_font_description_from_string( "Mono" );
	gtk_widget_modify_font( text, font_desc );
	pango_font_description_free( font_desc );
	tabs = program_tabs_new();
	gtk_text_view_set_tabs( GTK_TEXT_VIEW( text ), tabs );
	pango_tab_array_free( tabs );

	return( text );
}

static void
program_build( Program *program, GtkWidget *vbox )
{
	GtkAccelGroup *accel_group;
	GError *error;

	GtkWidget *mbar;
	GtkWidget *item;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;
	GtkWidget *swin;

        /* Make main menu bar
         */
	program->action_group = gtk_action_group_new( "ProgramActions" );
	gtk_action_group_set_translation_domain( program->action_group, 
		GETTEXT_PACKAGE );
	gtk_action_group_add_actions( program->action_group, 
		program_actions, G_N_ELEMENTS( program_actions ), 
		GTK_WINDOW( program ) );

	program->ui_manager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group( program->ui_manager, 
		program->action_group, 0 );

	accel_group = gtk_ui_manager_get_accel_group( program->ui_manager );
	gtk_window_add_accel_group( GTK_WINDOW( program ), accel_group );

	error = NULL;
	if( !gtk_ui_manager_add_ui_from_string( program->ui_manager,
			program_menubar_ui_description, -1, &error ) ) {
		g_message( "building menus failed: %s", error->message );
		g_error_free( error );
		exit( EXIT_FAILURE );
	}

	mbar = gtk_ui_manager_get_widget( program->ui_manager, 
		"/ProgramMenubar" );
	gtk_box_pack_start( GTK_BOX( vbox ), mbar, FALSE, FALSE, 0 );
        gtk_widget_show( mbar );

	/* On map of the edit menu, rethink cut/copy/paste sensitivity.
	 */
        item = gtk_ui_manager_get_widget( program->ui_manager,
		"/ProgramMenubar/EditMenu/Cut" );
	item = gtk_widget_get_parent( GTK_WIDGET( item ) );
        gtk_signal_connect( GTK_OBJECT( item ), "map",
                GTK_SIGNAL_FUNC( program_edit_map_cb ), program );

	program->pane = gtk_hpaned_new();
	gtk_paned_set_position( GTK_PANED( program->pane ), 
		program->pane_position );
	gtk_box_pack_start( GTK_BOX( vbox ), program->pane, TRUE, TRUE, 0 );
	gtk_widget_show( program->pane );

	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_paned_pack1( GTK_PANED( program->pane ), swin, FALSE, FALSE );
	gtk_widget_show( swin );

	program->store = gtk_tree_store_new( N_COLUMNS, 
		G_TYPE_STRING, 
		G_TYPE_STRING,
		G_TYPE_POINTER,
		G_TYPE_POINTER );
	program->row_inserted_sid = g_signal_connect( 
		G_OBJECT( program->store ), "row_inserted",
		G_CALLBACK( program_row_inserted_cb ), program );
	program->row_deleted_sid = g_signal_connect( 
		G_OBJECT( program->store ), "row_deleted",
		G_CALLBACK( program_row_deleted_cb ), program );

	program->tree = gtk_tree_view_new_with_model( 
		GTK_TREE_MODEL( program->store ) );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Tool" ),
		renderer, "text", NAME_COLUMN, NULL );
	gtk_tree_view_append_column( GTK_TREE_VIEW( program->tree ), column );

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes( _( "Name" ),
		renderer, "text", NAME_I18N_COLUMN, NULL );
	gtk_tree_view_append_column( GTK_TREE_VIEW( program->tree ), column );

	g_signal_connect( G_OBJECT( program->tree ), "row_collapsed",
		G_CALLBACK( program_row_collapsed_cb ), program );

	gtk_container_add( GTK_CONTAINER( swin ), program->tree );
	gtk_tree_view_set_enable_search( GTK_TREE_VIEW( program->tree ), TRUE );
	gtk_tree_view_set_reorderable( GTK_TREE_VIEW( program->tree ), TRUE );
	select = gtk_tree_view_get_selection( GTK_TREE_VIEW( program->tree ) );
	gtk_tree_selection_set_mode( select, GTK_SELECTION_SINGLE );
	program->select_changed_sid = g_signal_connect( 
		G_OBJECT( select ), "changed",
		G_CALLBACK( program_selection_changed_cb ), program );
	gtk_signal_connect( GTK_OBJECT( program->tree ), "event",
		GTK_SIGNAL_FUNC( program_tree_event_cb ), program );
	gtk_widget_show( program->tree );

	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_paned_pack2( GTK_PANED( program->pane ), swin, TRUE, TRUE );
	gtk_widget_show( swin );

	program->text = program_text_new();
        g_signal_connect( 
		gtk_text_view_get_buffer( GTK_TEXT_VIEW( program->text ) ),
		"changed",
                G_CALLBACK( program_text_changed ), program );
	gtk_container_add( GTK_CONTAINER( swin ), program->text );
	gtk_widget_show( program->text );

	gtk_widget_grab_focus( program->text );
}

static void
program_popdown( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys )
{
	Program *program = PROGRAM( iwnd );

	prefs_set( "PROGRAM_PANE_POSITION", "%d", 
		gtk_paned_get_position( GTK_PANED( program->pane ) ) );

        if( program->dirty && !program_parse( program ) )
                nfn( sys, IWINDOW_ERROR );
        else
                nfn( sys, IWINDOW_TRUE );
}

static void
program_link( Program *program, Toolkitgroup *kitg )
{
	program->kitg = kitg;
	program_title( program );
	iwindow_set_size_prefs( IWINDOW( program ), 
		"PROGRAM_WIDTH", "PROGRAM_HEIGHT" );
	iwindow_set_build( IWINDOW( program ), 
		(iWindowBuildFn) program_build, NULL, NULL, NULL );
	iwindow_set_popdown( IWINDOW( program ), program_popdown, NULL );
	iwindow_build( IWINDOW( program ) );
	program_all = g_slist_prepend( program_all, program );
	program_refresh( program );

	program->kitgroup_changed_sid = 
		g_signal_connect( G_OBJECT( program->kitg ), "changed",
			G_CALLBACK( program_kitgroup_changed ), program );
	program->kitgroup_destroy_sid = 
		g_signal_connect( G_OBJECT( program->kitg ), "destroy",
			G_CALLBACK( program_kitgroup_destroy ), program );
}

Program *
program_new( Toolkitgroup *kitg )
{
	Program *program = gtk_type_new( TYPE_PROGRAM );

	program_link( program, kitg );

	return( program );
}
