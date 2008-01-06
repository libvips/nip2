/* Manage workspace objects.
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
 */

#include "ip.h"

static FilemodelClass *parent_class = NULL;

static GSList *workspace_all = NULL;

Workspacegroup *
workspace_get_workspacegroup( Workspace *ws )
{
	return( WORKSPACEGROUP( ICONTAINER( ws )->parent ) );
}

/* Over all workspaces.
 */
void *
workspace_map( workspace_map_fn fn, void *a, void *b )
{
	return( icontainer_map( ICONTAINER( main_workspacegroup ), 
		(icontainer_map_fn) fn, a, b ) );
}

/* Map across the columns in a workspace.
 */
void *
workspace_map_column( Workspace *ws, column_map_fn fn, void *a )
{
	return( icontainer_map( ICONTAINER( ws ), 
		(icontainer_map_fn) fn, a, NULL ) );
}

/* Map across a Workspace, applying to the symbols of the top-level rows.
 */
void *
workspace_map_symbol( Workspace *ws, symbol_map_fn fn, void *a )
{
	return( icontainer_map( ICONTAINER( ws ), 
		(icontainer_map_fn) column_map_symbol, (void *) fn, a ) );
}

static void *
workspace_is_empty_sub( Symbol *sym )
{
	return( sym );
}

/* Does a workspace contain no rows?
 */
gboolean
workspace_is_empty( Workspace *ws )
{
	return( workspace_map_symbol( ws,
		(symbol_map_fn) workspace_is_empty_sub, NULL ) == NULL );
}

/* Map a function over all selected rows in a workspace.
 */
void *
workspace_selected_map( Workspace *ws, row_map_fn fn, void *a, void *b )
{
	return( slist_map2( ws->selected, (SListMap2Fn) fn, a, b ) );
}

static void *
workspace_selected_map_sym_sub( Row *row, symbol_map_fn fn, void *a )
{
	return( fn( row->sym, a, NULL, NULL ) );
}

/* Map a function over all selected symbols in a workspace.
 */
void *
workspace_selected_map_sym( Workspace *ws, 
	symbol_map_fn fn, void *a, void *b )
{
	return( workspace_selected_map( ws,
		(row_map_fn) workspace_selected_map_sym_sub, (void *) fn, a ) );
}

/* Are there any selected rows?
 */
gboolean
workspace_selected_any( Workspace *ws )
{
	return( ws->selected != NULL );
}

/* Number of selected rows.
 */
int
workspace_selected_num( Workspace *ws )
{
	return( g_slist_length( ws->selected ) );
}

static void *
workspace_selected_sym_sub( Row *row, Symbol *sym )
{
	if( row->sym == sym )
		return( row );

	return( NULL );
}

/* Is sym selected?
 */
gboolean
workspace_selected_sym( Workspace *ws, Symbol *sym )
{
	return( workspace_selected_map( ws,
		(row_map_fn) workspace_selected_sym_sub, sym, NULL ) != NULL  );
}

/* Is just one row selected? If yes, return it.
 */
Row *
workspace_selected_one( Workspace *ws )
{
	int len = g_slist_length( ws->selected );

	if( len == 1 )
		return( (Row *)(ws->selected->data) );
	else if( len == 0 ) {
		error_top( _( "No objects selected." ) );
		error_sub( _( "Select exactly one object and try again." ) );
		return( NULL );
	}
	else {
		error_top( _( "More than one object selected." ) );
		error_sub( _( "Select exactly one object and try again." ) );
		return( NULL );
	}
}

static void *
workspace_deselect_all_sub( Column *col )
{
	col->last_select = NULL;

	return( NULL );
}

/* Deselect all rows.
 */
void
workspace_deselect_all( Workspace *ws )
{
	(void) workspace_selected_map( ws, 
		(row_map_fn) row_deselect, NULL, NULL );
	(void) workspace_map_column( ws, 
		(column_map_fn) workspace_deselect_all_sub, NULL );
}

/* Track this while we build a names list.
 */
typedef struct {
	BufInfo *buf;
	const char *separator;
	gboolean first;
} NamesInfo;

/* Add a name to a string for a symbol.
 */
static void *
workspace_selected_names_sub( Row *row, NamesInfo *names )
{
	if( !names->first )
		buf_appends( names->buf, names->separator );

	/* Hack: if this is a matrix with selected cells, use an extract to
	 * get those cells out. We should really have a row method for this I
	 * guess :-(
	 */
	if( row->child_rhs && row->child_rhs->graphic &&
		IS_MATRIX( row->child_rhs->graphic ) &&
		MATRIX( row->child_rhs->graphic )->selected ) {
		Matrix *matrix = MATRIX( row->child_rhs->graphic );

		buf_appends( names->buf, "(" );
		row_qualified_name( row, names->buf );
		buf_appendf( names->buf, ".extract %d %d %d %d)",
			matrix->range.left, 
			matrix->range.top, 
			matrix->range.width, 
			matrix->range.height );
	}
	else 
		row_qualified_name( row, names->buf );

	names->first = FALSE;

	return( NULL );
}

/* Add a list of selected symbol names to a string. 
 */
void
workspace_selected_names( Workspace *ws, BufInfo *buf, const char *separator )
{
	NamesInfo names;

        names.buf = buf;
        names.separator = separator;
        names.first = TRUE;

	(void) workspace_selected_map( ws,
		(row_map_fn) workspace_selected_names_sub, &names, NULL );
}

void
workspace_column_names( Column *col, BufInfo *buf, const char *separator )
{
	NamesInfo names;

        names.buf = buf;
        names.separator = separator;
        names.first = TRUE;

	(void) column_map( col,
		(row_map_fn) workspace_selected_names_sub, &names, NULL );
}

/* Select all objects in all columns.
 */
void
workspace_select_all( Workspace *ws )
{
	(void) icontainer_map( ICONTAINER( ws ), 
		(icontainer_map_fn) column_select_symbols, NULL, NULL );
}

/* Is there just one column, and is it empty? 
 */
Column *
workspace_is_one_empty( Workspace *ws )
{
	GSList *children = ICONTAINER( ws )->children;
	Column *col;

	if( g_slist_length( children ) != 1 ) 
		return( NULL );

	col = COLUMN( children->data );
	if( !column_is_empty( col ) )
		return( NULL );

	return( col );
}

/* Search for a column by name.
 */
Column *
workspace_column_find( Workspace *ws, const char *name )
{
	Model *model;

	if( !(model = icontainer_map( ICONTAINER( ws ),
		(icontainer_map_fn) iobject_test_name, (void *) name, NULL )) ) 
		return( NULL );

	return( COLUMN( model ) );
}

/* Return the column for a name ... an existing column, or a new one.
 */
Column *
workspace_column_get( Workspace *ws, const char *name )
{
	Column *col;

	/* Exists?
	 */
	if( (col = workspace_column_find( ws, name )) ) 
		return( col );

	/* No - build new column and return a pointer to that.
	 */
	return( column_new( ws, name ) );
}

/* Make up a new column name. Check for not already in workspace, and not in 
 * xml file (if columns non-NULL).
 */
char *
workspace_column_name_new( Workspace *ws, xmlNode *columns )
{
	char buf[256];

	/* Search for one not in use.
	 */
	for(;;) {
		number_to_string( ws->next++, buf );

		if( workspace_column_find( ws, buf ) ) 
			continue;
		if( columns ) {
			xmlNode *i;

			for( i = columns; i; i = i->next ) {
				char name[MAX_STRSIZE];

				if( strcmp( (char *) i->name, "Column" ) == 0 &&
					get_sprop( i, "name", 
						name, MAX_STRSIZE ) )
					if( strcmp( name, buf ) == 0 )
						break;
			}

			if( i )
				continue;
		}

		return( im_strdup( NULL, buf ) );
	}
}

/* Make sure we have a column selected ... pick one of the existing columns; if 
 * there are none, make a column.
 */
Column *
workspace_column_pick( Workspace *ws )
{
	Column *col;

	if( ws->current )
		return( ws->current );

	if( ICONTAINER( ws )->children ) {
		col = COLUMN( ICONTAINER( ws )->children->data );
		workspace_column_select( ws, col );

		return( col );
	}

	/* Make an empty column ... always at the top left.
	 */
	col = column_new( ws, "A" );
	col->x = WORKSPACEVIEW_MARGIN_LEFT;
	col->y = WORKSPACEVIEW_MARGIN_TOP;
	workspace_column_select( ws, col );

	return( col );
}

/* Select a column. Can select NULL for no current col in this ws.
 */
void
workspace_column_select( Workspace *ws, Column *col )
{
	assert( !col || ICONTAINER_IS_CHILD( ws, col ) ); 

	if( col && col == ws->current )
		return;

	if( ws->current ) {
		ws->current->selected = FALSE;
		iobject_changed( IOBJECT( ws->current ) );
	}

	ws->current = col;

	if( col ) {
		col->selected = TRUE;
		iobject_changed( IOBJECT( col ) );
	}
}

/* Make a new symbol, part of the current column.
 */
static Symbol *
workspace_add_symbol( Workspace *ws )
{
	Column *col = workspace_column_pick( ws );
	Symbol *sym;
	char *name;

	name = column_name_new( col );
	sym = symbol_new( ws->sym->expr->compile, name );
	IM_FREE( name );

	return( sym );
}

/* Make up a new definition.
 */
Symbol *
workspace_add_def( Workspace *ws, const char *str )
{
	Column *col = workspace_column_pick( ws );
	Symbol *sym;
	char *name;

#ifdef DEBUG
	printf( "workspace_add_def: %s\n", str );
#endif /*DEBUG*/

        if( !str || strspn( str, WHITESPACE ) == strlen( str ) )
		return( NULL );

	/* Try parsing as a "fred = 12" style def. 
	 */
	attach_input_string( str );
	if( (name = parse_test_define()) ) {
		sym = symbol_new( ws->sym->expr->compile, name );
		IM_FREE( name );
		attach_input_string( str + 
			IM_CLIP( 0, input_state.charpos - 1, strlen( str ) ) );
	}
	else {
		/* That didn't work. Make a sym from the col name.
		 */
		sym = workspace_add_symbol( ws );
		attach_input_string( str );
	}

	if( !symbol_user_init( sym ) || 
		!parse_rhs( sym->expr, PARSE_RHS ) ) {
		/* Another parse error.
		 */
		expr_error_get( sym->expr );

		/* Block changes to error_string ... symbol_destroy() 
		 * can set this for compound objects.
		 */
		error_block();
		IDESTROY( sym );
		error_unblock();

		return( NULL );
	}

	/* If we're redefining a sym, it might have a row already.
	 */
	if( !sym->expr->row )
		(void) row_new( col->scol, sym, &sym->expr->root );
	symbol_made( sym );
	filemodel_set_modified( FILEMODEL( ws ), TRUE );

	if( symbol_recalculate_check( sym ) ) {
		/* Eval error.
		 */
		expr_error_get( sym->expr );
		error_block();
		IDESTROY( sym );
		error_unblock();

		return( NULL );
	}

	/* Jump to column containing object.
	 */
	model_scrollto( MODEL( col ), MODEL_SCROLL_BOTTOM );

	return( sym );
}

Symbol *
workspace_load_file( Workspace *ws, const char *filename )
{
	char txt[MAX_STRSIZE];
	BufInfo buf;
	Symbol *sym;

	buf_init_static( &buf, txt, MAX_STRSIZE );
	buf_appends( &buf, "Image_file \"" );
	buf_appendsc( &buf, TRUE, filename );
	buf_appends( &buf, "\"" );
	if( (sym = workspace_add_def( ws, buf_all( &buf ) )) ) {
		mainw_recent_add( &mainw_recent_image, filename );
		return( sym );
	}
	error_clear();

	buf_init_static( &buf, txt, MAX_STRSIZE );
	buf_appends( &buf, "Matrix_file \"" );
	buf_appendsc( &buf, TRUE, filename );
	buf_appends( &buf, "\"" );
	if( (sym = workspace_add_def( ws, buf_all( &buf ) )) ) {
		mainw_recent_add( &mainw_recent_matrix, filename );
		return( sym );
	}
	error_clear();

	return( NULL );
}

/* Bounding box of columns to be saved. Though we only really set top/left.
 */
static void *
workspace_selected_save_box( Column *col, Rect *box )
{
	if( model_save_test( MODEL( col ) ) ) {
		if( im_rect_isempty( box ) ) {
			box->left = col->x;
			box->top = col->y;
			box->width = 100;
			box->height = 100;
		}
		else {
			box->left = IM_MIN( box->left, col->x );
			box->top = IM_MIN( box->top, col->y );
		}
	}

	return( NULL );
}

/* Save just the selected objects.
 */
gboolean
workspace_selected_save( Workspace *ws, const char *filename )
{
	WorkspaceSaveType save = ws->save_type;
	Rect box = { 0 };

	ws->save_type = WORKSPACE_SAVE_SELECTED;

	workspace_map_column( ws, 
		(column_map_fn) workspace_selected_save_box, &box );
	filemodel_set_offset( FILEMODEL( ws ), box.left, box.top );

	if( !filemodel_save_all( FILEMODEL( ws ), filename ) ) {
		ws->save_type = save;
		unlinkf( "%s", filename );

		return( FALSE );
	}
	ws->save_type = save;

	return( TRUE );
}

/* Clone all selected symbols.
 */
gboolean 
workspace_clone_selected( Workspace *ws )
{
	char filename[FILENAME_MAX];

	/* Make a name for our clone file.
	 */
	if( !temp_name( filename, "ws" ) )
		return( FALSE );

	/* Save selected objects.
	 */
	if( !workspace_selected_save( ws, filename ) ) 
		return( FALSE );

	/* Try to load the clone file back again.
	 */
        set_hourglass();
	if( !workspace_merge_column_file( ws, filename ) ) {
		set_pointer();
		unlinkf( "%s", filename );

		return( FALSE );
	}
	set_pointer();
	unlinkf( "%s", filename );

	return( TRUE );
}

/* Keep the last WS_RETAIN workspaces as ipfl*.ws files.
 */
#define WS_RETAIN (10)

/* Array of names of workspace files we are keeping.
 */
static char *retain_files[WS_RETAIN] = { NULL };

/* The next one we allocate.
 */
static int retain_next = 0;

/* Save the workspace to one of our temp files.
 */
static gboolean
workspace_checkmark_timeout( Workspace *ws )
{
	ws->auto_save_timeout = 0;

	if( !AUTO_WS_SAVE )
		return( FALSE );

	/* Don't backup auto loaded workspace (eg. preferences). These are
	 * system things and don't need it.
	 */
	if( FILEMODEL( ws )->auto_load )
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
 
	if( !filemodel_save_all( FILEMODEL( ws ), retain_files[retain_next] ) )
		return( FALSE );

	retain_next = (retain_next + 1) % WS_RETAIN;

	return( FALSE );
}

/* Save the workspace to one of our temp files. Don't save directly (pretty
 * slow), instead set a timeout and save when we're quiet for >1s.
 */
static void
workspace_checkmark( Workspace *ws )
{
	if( !AUTO_WS_SAVE )
		return;
	if( FILEMODEL( ws )->auto_load )
		return;

	IM_FREEF( g_source_remove, ws->auto_save_timeout );
	ws->auto_save_timeout = g_timeout_add( 1000, 
		(GSourceFunc) workspace_checkmark_timeout, ws );
}

/* On safe exit, remove all ws checkmarks.
 */
void
workspace_retain_clean( void )
{
	int i;

	for( i = 0; i < WS_RETAIN; i++ ) {
		if( retain_files[i] ) {
			unlinkf( "%s", retain_files[i] );
			IM_FREE( retain_files[i] );
		}
	}
}

/* Track best-so-far file date here during search.
 */
static time_t date_sofar;

/* This file any better than the previous best candidate? Subfn of below.
 */
static char *
workspace_test_file( char *name, char *name_sofar )
{
	char buf[FILENAME_MAX];
	struct stat st;
	int i;

	(void) expand_variables( name, buf );
	for( i = 0; i < WS_RETAIN; i++ )
		if( retain_files[i] && 
			strcmp( buf, retain_files[i] ) == 0 )
			return( NULL );
	if( stat( buf, &st ) == -1 )
		return( NULL );
#ifdef HAVE_GETEUID
	if( st.st_uid != geteuid() )
		return( NULL );
#endif /*HAVE_GETEUID*/
	if( st.st_size == 0 )
		return( NULL );
	if( date_sofar > 0 && st.st_mtime < date_sofar )
		return( NULL );
	
	strcpy( name_sofar, name );
	date_sofar = st.st_mtime;

	return( NULL );
}

/* Load a workspace, called from a yesno dialog.
 */
static void
workspace_auto_recover_load( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	char *filename = (char *) client;
	Mainw *mainw = MAINW( iwindow_get_root_noparent( GTK_WIDGET( iwnd ) ) );
	Filemodel *filemodel;

	/* Load ws file.
	 */
        set_hourglass();
	filemodel = mainw_open_file( mainw, filename );
	set_pointer();

	if( filemodel ) {
		/* The filename will be something like
		 * "~/.nip2-7.9.6/tmp/untitled-nip2-0-3904875.ws", very
		 * unhelpful.
		 */
		IM_FREE( filemodel->filename );
		iobject_changed( IOBJECT( filemodel ) );

		nfn( sys, IWINDOW_TRUE );
	}
	else
		nfn( sys, IWINDOW_ERROR );
}

/* Do an auto-recover ... search for and load the most recent "ipfl*.ws" file 
 * in the tmp area owned by us, with a size > 0, that's not in our
 * retain_files[] set.
 */
void
workspace_auto_recover( GtkWidget *parent )
{
	char *p;
	char *name;
	char buf[FILENAME_MAX];
	char buf2[FILENAME_MAX];

	/* Find the dir we are saving temp files to.
	 */
	if( !temp_name( buf, "ws" ) ) {
		box_alert( parent );
		return;
	}

	if( (p = strrchr( buf, G_DIR_SEPARATOR )) )
		*p = '\0';

	date_sofar = -1;
	(void) path_map_dir( buf, "*.ws", 
		(path_map_fn) workspace_test_file, buf2 );
	if( date_sofar == -1 ) {
		if( !AUTO_WS_SAVE )
			box_info( parent, 
				_( "No backup workspaces found." ),
				_( "You need to enable \"Auto workspace "
				"save\" in Preferences "
				"before automatic recovery works." ) );
		else
			box_info( parent, 
				_( "No backup workspaces found." ),
				_( "No suitable workspace save files found "
				"in \"%s\"" ), buf );

		return;
	}

	/* Tricksy ... free str with notify callack from yesno.
	 */
	name = im_strdupn( buf2 );

	box_yesno( parent, 
		workspace_auto_recover_load, iwindow_true_cb, name, 
		(iWindowNotifyFn) im_free, name,
		GTK_STOCK_OPEN, 
		_( "Open workspace backup?" ),
		_( "Found workspace \"%s\", dated %s. "
		"Do you want to recover this workspace?" ),
		name, ctime( &date_sofar ) );
}

static void
workspace_dispose( GObject *gobject )
{
	Workspace *ws;

#ifdef DEBUG
	printf( "workspace_dispose: %s\n", NN( IOBJECT( gobject )->name ) );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WORKSPACE( gobject ) );

	ws = WORKSPACE( gobject );

	IM_FREEF( g_source_remove, ws->auto_save_timeout );
	UNREF( ws->kitg );
	UNREF( ws->local_kitg );
	IDESTROY( ws->sym );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
workspace_finalize( GObject *gobject )
{
	Workspace *ws;

#ifdef DEBUG
	printf( "workspace_finalize: %s\n", NN( IOBJECT( gobject )->name ) );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WORKSPACE( gobject ) );

	ws = WORKSPACE( gobject );

	IM_FREE( ws->status );
	IM_FREE( ws->local_defs );

	workspace_all = g_slist_remove( workspace_all, ws );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
workspace_child_add( iContainer *parent, iContainer *child, int pos )
{
	Workspace *ws = WORKSPACE( parent );
	Column *col = COLUMN( child );

	ICONTAINER_CLASS( parent_class )->child_add( parent, child, pos );

	if( col->selected )
		workspace_column_select( ws, col );
}

static void
workspace_child_remove( iContainer *parent, iContainer *child )
{
	Workspace *ws = WORKSPACE( parent );
	Column *col = COLUMN( child );

	/* Will we remove the current column? If yes, make sure
	 * current_column is NULL.
	 */
	if( ws->current == col )
		workspace_column_select( ws, NULL );

	filemodel_set_modified( FILEMODEL( ws ), TRUE );

	ICONTAINER_CLASS( parent_class )->child_remove( parent, child );
}

static void
workspace_link( Workspace *ws, Workspacegroup *wsg, const char *name )
{
	Symbol *sym;

#ifdef DEBUG
	printf( "workspace_link: naming ws as %s\n", name );
#endif /*DEBUG*/

	sym = symbol_new_defining( wsg->sym->expr->compile, name );

	ws->sym = sym;
	sym->type = SYM_WORKSPACE;
	sym->ws = ws;
	sym->expr = expr_new( sym );
	(void) compile_new( sym->expr );
	symbol_made( sym );
	iobject_set( IOBJECT( ws ), name, NULL );

	ws->local_kitg = toolkitgroup_new( ws->sym );
	g_object_ref( G_OBJECT( ws->local_kitg ) );
	iobject_sink( IOBJECT( ws->local_kitg ) );
}

static const char *
workspacemode_to_char( WorkspaceMode mode )
{
	switch( mode ) {
	case WORKSPACE_MODE_REGULAR:
		return( "WORKSPACE_MODE_REGULAR" );

	case WORKSPACE_MODE_FORMULA:
		return( "WORKSPACE_MODE_FORMULA" );

	case WORKSPACE_MODE_NOEDIT:
		return( "WORKSPACE_MODE_NOEDIT" );

	default:
		return( NULL );
	}
}

static WorkspaceMode
char_to_workspacemode( const char *mode )
{
	if( strcasecmp( mode, "WORKSPACE_MODE_REGULAR" ) == 0 )
		return( WORKSPACE_MODE_REGULAR );
	else if( strcasecmp( mode, "WORKSPACE_MODE_FORMULA" ) == 0 )
		return( WORKSPACE_MODE_FORMULA );
	else if( strcasecmp( mode, "WORKSPACE_MODE_NOEDIT" ) == 0 )
		return( WORKSPACE_MODE_NOEDIT );
	else
		return( (WorkspaceMode) -1 );
}

static View *
workspace_view_new( Model *model, View *parent )
{
	return( workspaceview_new() );
}

static gboolean
workspace_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	Workspace *ws = WORKSPACE( model );
	char buf[256];
	char *txt;

	assert( IS_WORKSPACEGROUP( parent ) );

	/* "view" is optional, for backwards compatibility.
	 */
	if( get_sprop( xnode, "view", buf, 256 ) ) {
		WorkspaceMode mode = char_to_workspacemode( buf );

		if( mode >= 0 )
			/* Could call workspace_set_mode(), but this is only a
			 * load, so so what.
			 */
			ws->mode = mode;
	}

	/* Also optional.
	 */
	(void) get_dprop( xnode, "scale", &ws->scale );
	(void) get_dprop( xnode, "offset", &ws->offset );

	(void) get_iprop( xnode, "window_width", &ws->window_width );
	(void) get_iprop( xnode, "window_height", &ws->window_height );

	(void) get_bprop( xnode, "lpane_open", &ws->lpane_open );
	(void) get_iprop( xnode, "lpane_position", &ws->lpane_position );
	(void) get_bprop( xnode, "rpane_open", &ws->rpane_open );
	(void) get_iprop( xnode, "rpane_position", &ws->rpane_position );

	if( get_sprop( xnode, "name", buf, 256 ) ) {
		IM_SETSTR( IOBJECT( ws )->name, buf );
	}
	if( get_sprop( xnode, "caption", buf, 256 ) ) {
		IM_SETSTR( IOBJECT( ws )->caption, buf );
	}

	/* Don't use get_sprop() and avoid a limit on def size.
	 */
	if( (txt = (char *) xmlGetProp( xnode, (xmlChar *) "local_defs" )) ) {
		(void) workspace_local_set( ws, txt );
		IM_FREEF( xmlFree, txt );
	}

	if( !MODEL_CLASS( parent_class )->load( model, state, parent, xnode ) )
		return( FALSE );

	return( TRUE );
}

static xmlNode *
workspace_save( Model *model, xmlNode *xnode )
{
	Workspace *ws = WORKSPACE( model );
	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( !set_prop( xthis, "view", workspacemode_to_char( ws->mode ) ) ||
		!set_dprop( xthis, "scale", ws->scale ) ||
		!set_dprop( xthis, "offset", ws->offset ) ||
		!set_prop( xthis, "window_width", "%d", ws->window_width ) ||
		!set_prop( xthis, "window_height", "%d", ws->window_height ) ||
		!set_prop( xthis, "lpane_position", "%d", 
			ws->lpane_position ) ||
		!set_prop( xthis, "lpane_open", 
			bool_to_char( ws->lpane_open ) ) ||
		!set_prop( xthis, "rpane_position", "%d", 
			ws->rpane_position ) ||
		!set_prop( xthis, "rpane_open", 
			bool_to_char( ws->rpane_open ) ) ||
		!set_sprop( xthis, "local_defs", ws->local_defs ) ||
		!set_sprop( xthis, "name", IOBJECT( ws )->name ) ||
		!set_sprop( xthis, "caption", IOBJECT( ws )->caption ) ) 
		return( NULL );

	return( xthis );
}

void
workspace_empty( Model *model )
{
	Workspace *ws = WORKSPACE( model );

	/* Make sure this gets reset.
	 */
	ws->area.left = 0;
	ws->area.top = 0;
	ws->area.width = 0;
	ws->area.height = 0;

	MODEL_CLASS( parent_class )->empty( model );
}

static void
workspace_rename_row_node( ModelLoadState *state, Column *col, xmlNode *xnode )
{
	char name[MAX_STRSIZE];

	if( strcmp( (char *) xnode->name, "Row" ) == 0 &&
		get_sprop( xnode, "name", name, MAX_STRSIZE ) ) {
		char *new_name;

		new_name = column_name_new( col );
		(void) set_prop( xnode, "name", new_name );
		(void) model_loadstate_rename_new( state, name, new_name );
		IM_FREE( new_name );
	}
}

/* Rename column if there's one of that name in workspace. 
 */
static void
workspace_rename_column_node( Workspace *ws, 
	ModelLoadState *state, xmlNode *xnode, xmlNode *columns )
{
	char name[MAX_STRSIZE];

	if( strcmp( (char *) xnode->name, "Column" ) == 0 &&
		get_sprop( xnode, "name", name, MAX_STRSIZE ) &&
		icontainer_map( ICONTAINER( ws ), 
			(icontainer_map_fn) iobject_test_name, name, NULL ) ) {
		char *new_name;
		Column *col;
		xmlNode *i;

		/* Exists already ... rename this column.
		 */
		new_name = workspace_column_name_new( ws, columns );
		col = column_new( ws, new_name );

#ifdef DEBUG
		printf( "workspace_rename_column_node: renaming column "
			"%s to %s\n", 
			name, new_name );
#endif /*DEBUG*/

		(void) set_prop( xnode, "name", new_name );
		IM_FREE( new_name );

		/* And allocate new names for all rows in the subcolumn.
		 */
		for( i = get_node( xnode, "Subcolumn" )->children; 
			i; i = i->next ) 
			workspace_rename_row_node( state, col, i );

		IDESTROY( col );
	}
}

static void *
workspace_load_toolkit( const char *filename, Toolkitgroup *toolkitgroup )
{
	if( !toolkit_new_from_file( toolkitgroup, filename ) ) 
		box_alert( NULL );

	return( NULL );
}

static gboolean
workspace_load_compat( Workspace *ws, int major, int minor )
{
	char pathname[FILENAME_MAX];
	GSList *path;

#ifdef DEBUG
	printf( "workspace_load_compat: loading compat for %d.%d\n", 
		major, minor );
#endif /*DEBUG*/

	/* Make a private toolkitgroup local to this workspace to hold the
	 * compatibility defs we are planning to load.
	 */
	UNREF( ws->kitg );
	ws->kitg = toolkitgroup_new( ws->sym );
	g_object_ref( G_OBJECT( ws->kitg ) );
	iobject_sink( IOBJECT( ws->kitg ) );

	im_snprintf( pathname, FILENAME_MAX, 
		"$VIPSHOME/share/" PACKAGE "/compat/%d.%d", major, minor );
	path = path_parse( pathname );
	if( path_map_exact( path, "*.def", 
		(path_map_fn) workspace_load_toolkit, ws->kitg ) ) {
		path_free( path );
		return( FALSE );
	}
	path_free( path );

	ws->compat_major = major;
	ws->compat_minor = minor;

	return( TRUE );
}

static gboolean
workspace_top_load( Filemodel *filemodel,
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	Workspace *ws = WORKSPACE( filemodel );
	Workspacegroup *wsg = WORKSPACEGROUP( parent );
	Column *current_col;
	xmlNode *i, *j, *k;
	char name[FILENAME_MAX];

#ifdef DEBUG
	printf( "workspace_top_load: from %s\n", state->filename );
#endif /*DEBUG*/

	switch( ws->load_type ) {
	case WORKSPACE_LOAD_TOP:
		/* Easy ... ws is a blank Workspace we are loading into. No
		 * renaming needed, except for the ws.
		 */

		/* Set the workspace name from the filename, ignoring the name
		 * saved in the file.
		 */
		name_from_filename( state->filename, name );
		while( compile_lookup( wsg->sym->expr->compile, name ) )
			increment_name( name );
		workspace_link( ws, wsg, name );

		filemodel->major = state->major;
		filemodel->minor = state->minor;
		filemodel->micro = state->micro;
		filemodel->versioned = TRUE;

		/* If necessary, load up compatibility definitions.
		 */
		if( filemodel->major != MAJOR_VERSION || 
			filemodel->minor != MINOR_VERSION ) {
			if( existsf( "$VIPSHOME/share/" PACKAGE "/compat/%d.%d",
				filemodel->major, filemodel->minor ) ) 
				if( !workspace_load_compat( ws, 
					filemodel->major, filemodel->minor ) ) 
					return( FALSE );

			/* Do we need broken region handling?
			 */
			if( filemodel->major == 7 && filemodel->minor == 8 ) 
				ws->compat_78 = TRUE;
		}

		if( model_load( MODEL( ws ), state, parent, xnode ) )
			return( FALSE );

		/* The model_load() will set the name from the name saved in
		 * the XML. We want to override that with the name from the
		 * filename.
		 */
		iobject_set( IOBJECT( ws ), name, NULL );

		break;

	case WORKSPACE_LOAD_COLUMNS:
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
		 */
		if( state->major != filemodel->major ||
			state->minor != filemodel->minor ) {
			box_info( NULL,
				_( "Version mismatch." ),
				_( "File \"%s\" was saved from %s-%d.%d.%d. "
				"You may see compatibility problems." ),
				state->filename, PACKAGE,
				state->major, state->minor, state->micro );
		}

		break;

	case WORKSPACE_LOAD_ROWS:
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
		assert( FALSE );
	}

	return( FILEMODEL_CLASS( parent_class )->top_load( filemodel, 
		state, parent, xnode ) );
}

static void 
workspace_set_modified( Filemodel *filemodel, gboolean modified )
{
	Workspace *ws = WORKSPACE( filemodel );

	workspace_checkmark( ws );

	FILEMODEL_CLASS( parent_class )->set_modified( filemodel, modified );
}

static gboolean
workspace_save_all( Filemodel *filemodel, const char *filename )
{
	gboolean result;

#ifdef DEBUG
	printf( "workspace_save_all: %s to %s\n",
		NN( IOBJECT( filemodel )->name ), filename );
#endif /*DEBUG*/

	if( (result = FILEMODEL_CLASS( parent_class )->save_all( filemodel,
		filename )) )
		/* This will add save-as files to recent too. Don't note
		 * auto_load on recent, since it won't have been loaded by the
		 * user.
		 */
		if( !filemodel->auto_load )
			mainw_recent_add( &mainw_recent_workspace, filename );

	return( result );
}

static void
workspace_class_init( WorkspaceClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	FilemodelClass *filemodel_class = (FilemodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = workspace_dispose;
	gobject_class->finalize = workspace_finalize;

	icontainer_class->child_add = workspace_child_add;
	icontainer_class->child_remove = workspace_child_remove;

	model_class->view_new = workspace_view_new;
	model_class->load = workspace_load;
	model_class->save = workspace_save;
	model_class->empty = workspace_empty;

	filemodel_class->top_load = workspace_top_load;
	filemodel_class->set_modified = workspace_set_modified;
	filemodel_class->save_all = workspace_save_all;
	filemodel_class->filetype = filesel_type_workspace;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
workspace_init( Workspace *ws )
{
	char txt[256];

	ws->sym = NULL;

	/* We default to using the main toolkitgroup for our definitions.
	 * Unref and load private defs if we need compatibility.
	 */
	ws->kitg = main_toolkitgroup;
	g_object_ref( G_OBJECT( ws->kitg ) );

	ws->next = 0;
	ws->current = NULL;
	ws->selected = NULL;
	ws->errors = NULL;
        ws->mode = WORKSPACE_MODE_REGULAR;

	ws->compat_78 = FALSE;
	ws->compat_major = 0;
	ws->compat_minor = 0;

	ws->load_type = WORKSPACE_LOAD_TOP;
	ws->save_type = WORKSPACE_SAVE_ALL;

	ws->area.left = 0;
	ws->area.top = 0;
	ws->area.width = 0;
	ws->area.height = 0;
	ws->vp = ws->area;
	ws->window_width = 0;
	ws->window_height = 0;

	/* Overwritten by mainw.
	 */
	ws->lpane_open = WORKSPACE_RPANE_OPEN;
	ws->lpane_position = WORKSPACE_RPANE_POSITION;
	ws->rpane_open = WORKSPACE_LPANE_OPEN;
	ws->rpane_position = WORKSPACE_LPANE_POSITION;

	ws->auto_save_timeout = 0;

	ws->status = NULL;
	im_snprintf( txt, 256, _( NIP_COPYRIGHT ), PACKAGE );
	IM_SETSTR( ws->status, txt );

	ws->scale = 1.0;
	ws->offset = 0.0;

	ws->local_defs = im_strdupn( _( 
		"// private definitions for this workspace\n" ) );
	ws->local_kitg = NULL;
	ws->local_kit = NULL;

	filemodel_register( FILEMODEL( ws ) );

	workspace_all = g_slist_prepend( workspace_all, ws );
}

GType
workspace_get_type( void )
{
	static GType workspace_type = 0;

	if( !workspace_type ) {
		static const GTypeInfo info = {
			sizeof( WorkspaceClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) workspace_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Workspace ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) workspace_init,
		};

		workspace_type = g_type_register_static( TYPE_FILEMODEL, 
			"Workspace", &info, 0 );
	}

	return( workspace_type );
}

Workspace *
workspace_new( Workspacegroup *wsg, const char *name )
{
	Workspace *ws;

#ifdef DEBUG
	printf( "workspace_new: %s\n", name );
#endif /*DEBUG*/

	if( compile_lookup( wsg->sym->expr->compile, name ) ) {
		error_top( _( "Name clash." ) );
		error_sub( _( "Can't create workspace \"%s\". "
			"A symbol with that name already exists." ), name );
		return( NULL );
	}

	ws = WORKSPACE( g_object_new( TYPE_WORKSPACE, NULL ) );
	icontainer_child_add( ICONTAINER( wsg ), ICONTAINER( ws ), -1 );
	workspace_link( ws, wsg, name );
	(void) workspace_column_pick( ws );

	return( ws );
}

/* Load into an empty workspace.
 */
static gboolean
workspace_load_empty( Workspace *ws, Workspacegroup *wsg, const char *filename )
{
	g_assert( workspace_is_empty( ws ) );

	ws->load_type = WORKSPACE_LOAD_TOP;
	column_set_offset( WORKSPACEVIEW_MARGIN_LEFT, 
		WORKSPACEVIEW_MARGIN_TOP );
	if( !filemodel_load_all( FILEMODEL( ws ), MODEL( wsg ), filename ) ) 
		return( FALSE );
	filemodel_set_modified( FILEMODEL( ws ), FALSE );
	filemodel_set_filename( FILEMODEL( ws ), filename );

	return( TRUE );
}

/* New workspace from a file.
 */
Workspace *
workspace_new_from_file( Workspacegroup *wsg, const char *filename )
{
	Workspace *ws;

#ifdef DEBUG
	printf( "workspace_new_from_file: %s\n", filename );
#endif /*DEBUG*/

	ws = WORKSPACE( g_object_new( TYPE_WORKSPACE, NULL ) );
	if( !workspace_load_empty( ws, wsg, filename ) ) {
		g_object_unref( G_OBJECT( ws ) );
		return( NULL );
	}

	return( ws );
}

/* New workspace from a file.
 */
Workspace *
workspace_new_from_openfile( Workspacegroup *wsg, iOpenFile *of )
{
	Workspace *ws;

#ifdef DEBUG
	printf( "workspace_new_from_openfile: %s\n", of->fname );
#endif /*DEBUG*/

	ws = WORKSPACE( g_object_new( TYPE_WORKSPACE, NULL ) );
	ws->load_type = WORKSPACE_LOAD_TOP;
	if( !filemodel_load_all_openfile( FILEMODEL( ws ), 
		MODEL( wsg ), of ) ) {
		g_object_unref( G_OBJECT( ws ) );
		return( NULL );
	}

	filemodel_set_modified( FILEMODEL( ws ), FALSE );
	filemodel_set_filename( FILEMODEL( ws ), of->fname );

#ifdef DEBUG
	printf( "(set name = %s)\n", IOBJECT( ws )->name );
#endif /*DEBUG*/

	return( ws );
}

/* Make the blank workspace we present the user with (in the absence of
 * anything else).
 */
Workspace *
workspace_new_blank( Workspacegroup *wsg, const char *name )
{
	Workspace *ws;

	if( !(ws = workspace_new( wsg, name )) )
		return( NULL );

	iobject_set( IOBJECT( ws ), NULL, _( "Default empty workspace" ) );

	return( ws );
}

/* Merge file into this workspace. If this workspace is blank, then behave
 * like workspace_new_from_file() instead.
 */
gboolean
workspace_merge_file( Workspace *ws, const char *filename )
{
	if( workspace_is_empty( ws ) ) {
		model_empty( MODEL( ws ) );

		if( !workspace_load_empty( ws, 
			WORKSPACEGROUP( ICONTAINER( ws )->parent ), 
			filename ) ) 
			return( FALSE );
	}
	else {
		ws->load_type = WORKSPACE_LOAD_COLUMNS;
		column_set_offset( 
			IM_RECT_RIGHT( &ws->area ) + WORKSPACEVIEW_MARGIN_LEFT,
			WORKSPACEVIEW_MARGIN_TOP );
		if( !filemodel_load_all( FILEMODEL( ws ), 
			MODEL( ICONTAINER( ws )->parent ), filename ) ) 
			return( FALSE );

		filemodel_set_modified( FILEMODEL( ws ), TRUE );
	}

	return( TRUE );
}

/* Merge file into the current column of this workspace.
 */
gboolean
workspace_merge_column_file( Workspace *ws, const char *filename )
{
	ws->load_type = WORKSPACE_LOAD_ROWS;
	column_set_offset( IM_RECT_RIGHT( &ws->area ), 
		IM_RECT_BOTTOM( &ws->area ) );
	if( !filemodel_load_all( FILEMODEL( ws ), 
		MODEL( ICONTAINER( ws )->parent ), filename ) ) 
		return( FALSE );

	filemodel_set_modified( FILEMODEL( ws ), TRUE );

	return( TRUE );
}

/* Get the bottom row from the current column.
 */
Row *
workspace_get_bottom( Workspace *ws )
{
	return( column_get_bottom( workspace_column_pick( ws ) ) );
}

gboolean
workspace_add_action( Workspace *ws, 
	const char *name, const char *action, int nparam )
{
	Column *col = workspace_column_pick( ws );
	char str[1024];
	BufInfo buf;

	/* Are there any selected symbols?
	 */
	buf_init_static( &buf, str, 1024 );
	buf_appends( &buf, action );
	if( nparam > 0 && workspace_selected_any( ws ) ) {
		if( nparam != workspace_selected_num( ws ) ) {
			error_top( _( "Wrong number of arguments." ) );
			error_sub( _( "%s needs %d arguments, "
				"there are %d selected." ), 
				name, nparam,
				workspace_selected_num( ws ) );
			return( FALSE );
		}

		buf_appends( &buf, " " );
		workspace_selected_names( ws, &buf, " " );
		if( buf_is_full( &buf ) ) {
			error_top( _( "Overflow error." ) );
			error_sub( _( "Too many names selected." ) );
			return( FALSE );
		}

		if( !workspace_add_def( ws, buf_all( &buf ) ) ) 
			return( FALSE );
		workspace_deselect_all( ws );
	}
	else {
		/* Try to use the previous n items in this column as the
		 * arguments. 
		 */
		if( !column_add_n_names( col, name, &buf, nparam ) || 
			!workspace_add_def( ws, buf_all( &buf ) ) ) 
			return( FALSE );
	}

	return( TRUE );
}

int
workspace_number( void )
{
	return( g_slist_length( workspace_all ) );
}

Workspace *
workspace_clone( Workspace *ws )
{
	Workspacegroup *wsg = WORKSPACEGROUP( ICONTAINER( ws )->parent );
	Workspace *nws;
	char filename[4096];

	/* Make a name for our clone file.
	 */
	if( !temp_name( filename, "ws" ) ||
		!filemodel_save_all( FILEMODEL( ws ), filename ) ) 
		return( NULL );

	/* Try to load the clone file back again.
	 */
	if( !(nws = workspace_new_from_file( wsg, filename )) ) {
		unlinkf( "%s", filename );
		return( NULL );
	}
	unlinkf( "%s", filename );

	/* Get rid of the crazy "nip-x-xxxx.ws" filename.
	 */
	filemodel_set_filename( FILEMODEL( nws ), NULL );

	return( nws );
}

static void *
workspace_row_dirty( Row *row, int serial )
{
	return( expr_dirty( row->expr, serial ) );
}

/* Recalculate selected items.
 */
gboolean
workspace_selected_recalc( Workspace *ws )
{
	if( workspace_selected_map( ws,
		(row_map_fn) workspace_row_dirty, 
		 GINT_TO_POINTER( link_serial_new() ), NULL ) ) 
		return( FALSE );

	/* Recalc even if autorecomp is off.
	 */
	symbol_recalculate_all_force( FALSE );

	workspace_deselect_all( ws );

	return( TRUE );
}

static void *
workspace_selected_remove2( Row *row )
{
	if( row != row->top_row )
		return( row );

	return( NULL );
}

static void *
workspace_selected_remove3( Row *row, int *nsel )
{
	if( row->selected )
		*nsel += 1;

	return( NULL );
}

static void *
workspace_selected_remove4( Column *col, GSList **cs )
{
	int nsel = 0;

	(void) column_map( col, 
		(row_map_fn) workspace_selected_remove3, &nsel, NULL );
	if( nsel > 0 )
		*cs = g_slist_prepend( *cs, col );

	return( NULL );
}

static void *
workspace_selected_remove5( Column *col )
{
	Subcolumn *scol = col->scol;
	int nmembers = g_slist_length( ICONTAINER( scol )->children );

	if( nmembers > 0 ) 
		icontainer_pos_renumber( ICONTAINER( scol ) );
	else
		IDESTROY( col );

	return( NULL );
}

/* Remove selected items.
 *
 * 0. check all objects to be destroyed are top level rows
 * 1. look for and note all columns containing items we are going to delete
 * 2. loop over selected items, and delete them one-by-one.
 * 3. loop over the columns we noted in 1 and delete empty ones
 * 4. renumber affected columns
 */
static gboolean
workspace_selected_remove( Workspace *ws )
{
	Row *row;
	GSList *cs = NULL;

	if( (row = (Row *) workspace_selected_map( ws,
		(row_map_fn) workspace_selected_remove2, NULL, NULL )) ) {
		error_top( _( "You can only remove top level rows." ) );
		error_sub( _( "Not all selected objects are top level "
			"rows." ) );
		return( FALSE );
	}

	(void) workspace_map_column( ws, 
		(column_map_fn) workspace_selected_remove4, &cs );
	(void) workspace_selected_map_sym( ws,
		(symbol_map_fn) iobject_destroy, NULL, NULL );
	(void) slist_map( cs, 
		(SListMapFn) workspace_selected_remove5, NULL );

	IM_FREEF( g_slist_free, cs );
	symbol_recalculate_all();
	filemodel_set_modified( FILEMODEL( ws ), TRUE );

	return( TRUE );
}

/* Callback for workspace_selected_remove_yesno. Remove selected items.
 */
static void
workspace_selected_remove_yesno_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Workspace *ws = WORKSPACE( client );

	if( workspace_selected_remove( ws ) )
		nfn( sys, IWINDOW_TRUE );
	else
		nfn( sys, IWINDOW_ERROR );
}

/* Ask before removing selected.
 */
void
workspace_selected_remove_yesno( Workspace *ws, GtkWidget *parent )
{
	BufInfo buf;
	char str[30];

        if( !workspace_selected_any( ws ) ) 
		return;

	buf_init_static( &buf, str, 30 );
	workspace_selected_names( ws, &buf, ", " );

	box_yesno( parent, 
		workspace_selected_remove_yesno_cb, iwindow_true_cb, ws, 
		iwindow_notify_null, NULL,
		GTK_STOCK_DELETE, 
		_( "Delete selected objects?" ),
		_( "Are you sure you want to delete %s?" ), buf_all( &buf ) );
}

/* Sub fn of below ... add a new index expression.
 */
static gboolean
workspace_ungroup_add_index( Row *row, const char *fmt, int i )
{
	static BufInfo buf;
	static char txt[200];

	buf_init_static( &buf, txt, 200 );
	row_qualified_name( row, &buf );
	buf_appendf( &buf, fmt, i );
	if( !workspace_add_def( row->ws, buf_all( &buf ) ) )
		return( FALSE );

	return( TRUE );
}

static void *
workspace_ungroup_row( Row *row )
{
	PElement *root = &row->expr->root;
	gboolean result;
	PElement value;
	int length;
	int i;

	if( !heap_is_instanceof( CLASS_GROUP, root, &result ) )
		return( row );
	if( result ) {
		if( !class_get_member( root, MEMBER_VALUE, NULL, &value ) || 
			(length = heap_list_length_max( &value, 100 )) < 0 ) 
			return( row );

		for( i = 0; i < length; i++ )
			if( !workspace_ungroup_add_index( row, 
				".value?%d", i ) )
				return( row );
	}
	else {
		if( !heap_is_list( root, &result ) )
			return( row );
		if( result ) {
			if( (length = heap_list_length_max( root, 100 )) < 0 ) 
				return( row );

			for( i = 0; i < length; i++ )
				if( !workspace_ungroup_add_index( row, 
					"?%d", i ) )
					return( row );
		}
		else {
			BufInfo buf;
			char txt[100];

			buf_init_static( &buf, txt, 100 );
			row_qualified_name( row, &buf );
			error_top( _( "Unable to ungroup." ) );
			error_sub( _( "Row \"%s\" is not a Group or a list." ), 
				buf_all( &buf ) );  

			return( row );
		}
	}

	return( NULL );
}

/* Ungroup the selected object(s), or the bottom object.
 */
gboolean
workspace_selected_ungroup( Workspace *ws )
{
	if( !workspace_selected_any( ws ) ) {
		/* Nothing selected -- ungroup bottom object.
		 */
		Symbol *sym = workspace_get_bottom( ws )->sym;

		if( !sym || workspace_ungroup_row( sym->expr->row ) ) 
			return( FALSE );
		symbol_recalculate_all();
	}
	else {
		/* Ungroup selected symbols.
		 */
		if( workspace_selected_map( ws,
			(row_map_fn) workspace_ungroup_row, NULL, NULL ) ) {
			symbol_recalculate_all();
			return( FALSE );
		}
		symbol_recalculate_all();
		workspace_deselect_all( ws );
	}

	return( TRUE );
}

void
workspace_set_status( Workspace *ws, const char *fmt, ... )
{
	va_list ap;
	char buf[256];

	va_start( ap, fmt );
	(void) im_vsnprintf( buf, 256, fmt, ap );
	va_end( ap );

	IM_SETSTR( ws->status, buf );
	iobject_changed( IOBJECT( ws ) );
}

void
workspace_set_mode( Workspace *ws, WorkspaceMode mode )
{
	if( ws->mode != mode ) {
		ws->mode = mode;

		/* Rebuild all the views. Yuk! It would be better to get the
		 * views that change with workspace mode to watch the
		 * enclosing workspace and update on that. But we'd have
		 * connections from almost every object in the ws. We don't
		 * change mode very often, so just loop over them all.
		 */
		icontainer_map_all( ICONTAINER( ws ),
			(icontainer_map_fn) iobject_changed, NULL );
	}
}

/* New ws private defs.
 */
gboolean
workspace_local_set( Workspace *ws, const char *txt )
{
	/* New kit for defs ... will destroy any old defs, since we can't have
	 * two kits with the same name. Don't register it, we don't want it 
	 * to be autosaved on quit.
	 */
	ws->local_kit = toolkit_new( ws->local_kitg, "Workspace Locals" );
	filemodel_unregister( FILEMODEL( ws->local_kit ) );
	IM_SETSTR( ws->local_defs, txt );
	iobject_changed( IOBJECT( ws ) );

	filemodel_set_modified( FILEMODEL( ws ), TRUE );
	attach_input_string( txt );
	if( !parse_toplevel( ws->local_kit, 0 ) ) 
		return( FALSE );

	return( TRUE );
}

gboolean
workspace_local_set_from_file( Workspace *ws, const char *fname )
{
	iOpenFile *of;
	char *txt;

	if( !(of = file_open_read( "%s", fname )) ) 
		return( FALSE );
	if( !(txt = file_read( of )) ) {
		file_close( of );
		return( FALSE );
	}
	if( !workspace_local_set( ws, txt ) ) {
		g_free( txt );
		file_close( of );
		return( FALSE );
	}

	filemodel_set_filename( FILEMODEL( ws->local_kit ), fname );

	g_free( txt );
	file_close( of );

	return( TRUE );
}
