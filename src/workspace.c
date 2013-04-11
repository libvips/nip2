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

static ModelClass *parent_class = NULL;

static GSList *workspace_all = NULL;

Workspacegroup *
workspace_get_workspacegroup( Workspace *ws )
{
	iContainer *parent; 

	if( (parent = ICONTAINER( ws )->parent) )
		return( WORKSPACEGROUP( parent ) );

	return( NULL );
}

Workspaceroot *
workspace_get_workspaceroot( Workspace *ws )
{
	return( workspace_get_workspacegroup( ws )->wsr );
}

void
workspace_set_modified( Workspace *ws, gboolean modified )
{
	Workspacegroup *wsg;

	if( (wsg = workspace_get_workspacegroup( ws )) )
		filemodel_set_modified( FILEMODEL( wsg ), modified );
}

/* Over all workspaces.
 */
void *
workspace_map( workspace_map_fn fn, void *a, void *b )
{
	return( icontainer_map( ICONTAINER( main_workspaceroot ), 
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
	VipsBuf *buf;
	const char *separator;
	gboolean first;
} NamesInfo;

/* Add a name to a string for a symbol.
 */
static void *
workspace_selected_names_sub( Row *row, NamesInfo *names )
{
	if( !names->first )
		vips_buf_appends( names->buf, names->separator );

	/* Hack: if this is a matrix with selected cells, use an extract to
	 * get those cells out. We should really have a row method for this I
	 * guess :-(
	 */
	if( row->child_rhs && row->child_rhs->graphic &&
		IS_MATRIX( row->child_rhs->graphic ) &&
		MATRIX( row->child_rhs->graphic )->selected ) {
		Matrix *matrix = MATRIX( row->child_rhs->graphic );

		vips_buf_appends( names->buf, "(" );
		row_qualified_name( row, names->buf );
		vips_buf_appendf( names->buf, ".extract %d %d %d %d)",
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
workspace_selected_names( Workspace *ws, VipsBuf *buf, const char *separator )
{
	NamesInfo names;

        names.buf = buf;
        names.separator = separator;
        names.first = TRUE;

	(void) workspace_selected_map( ws,
		(row_map_fn) workspace_selected_names_sub, &names, NULL );
}

void
workspace_column_names( Column *col, VipsBuf *buf, const char *separator )
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

/* Make up a new column name. Check for not already in workspace.
 */
void
workspace_column_name_new( Workspace *ws, char *name )
{
	do {
		number_to_string( ws->next++, name );
	} while( workspace_column_find( ws, name ) );
}

Column *
workspace_get_column( Workspace *ws )
{
	if( ICONTAINER( ws )->current )
		return( COLUMN( ICONTAINER( ws )->current ) );

	return( NULL );
}

/* Select a column. Can select NULL for no current col in this ws.
 */
void
workspace_column_select( Workspace *ws, Column *col )
{
	icontainer_child_current( ICONTAINER( ws ), ICONTAINER( col ) ); 
}

/* Make sure we have a column selected ... pick one of the existing columns; if 
 * there are none, make a column.
 */
Column *
workspace_column_pick( Workspace *ws )
{
	Column *col;

	if( (col = workspace_get_column( ws )) )
		return( col );
	if( (col = COLUMN( icontainer_get_nth_child( 
		ICONTAINER( ws ), 0 ) )) ) {
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

/* Make and select a column.
 */
gboolean
workspace_column_new( Workspace *ws )
{
	char new_name[MAX_STRSIZE];
	Column *col;

	workspace_column_name_new( ws, new_name );
	if( !(col = column_new( ws, new_name )) ) 
		return( FALSE );
	workspace_column_select( ws, col );

	return( TRUE );
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
	workspace_set_modified( ws, TRUE );

	return( sym );
}

/* Make up a new definition, recalc and scroll to make it visible. 
 */
Symbol *
workspace_add_def_recalc( Workspace *ws, const char *str )
{
	Column *col = workspace_column_pick( ws );

	Symbol *sym;

#ifdef DEBUG
	printf( "workspace_add_def_recalc: %s\n", str );
#endif /*DEBUG*/

	if( !(sym = workspace_add_def( ws, str )) )
		return( NULL );

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

gboolean
workspace_load_file_buf( VipsBuf *buf, const char *filename )
{
	if( callv_string_filenamef( 
		(callv_string_fn) vips_format_for_file,
		"%s", filename ) ) 
		vips_buf_appends( buf, "Image_file" );
	else
		vips_buf_appends( buf, "Matrix_file" );

	vips_buf_appends( buf, " \"" );
	vips_buf_appendsc( buf, TRUE, filename );
	vips_buf_appends( buf, "\"" );

	return( TRUE );
}

/* Load a matrix or image. Don't recalc: you need to recalc later to test for
 * success/fail. See eg. workspace_add_def_recalc()
 */
Symbol *
workspace_load_file( Workspace *ws, const char *filename )
{
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );
	Symbol *sym;

	if( !workspace_load_file_buf( &buf, filename ) )
		return( NULL );
	if( !(sym = workspace_add_def( ws, vips_buf_all( &buf ) )) ) 
		return( NULL );
	mainw_recent_add( &mainw_recent_image, filename );

	return( sym );
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
workspace_changed( iObject *iobject )
{
	Workspace *ws;
	Workspacegroup *wsg;

#ifdef DEBUG
	printf( "workspace_changed: %s\n", NN( iobject->name ) );
#endif /*DEBUG*/

	g_return_if_fail( iobject != NULL );
	g_return_if_fail( IS_WORKSPACE( iobject ) );

	ws = WORKSPACE( iobject );
	wsg = workspace_get_workspacegroup( ws );

	/* Signal changed on our workspacegroup, if we're the current object.
	 */
	if( wsg &&
		ICONTAINER( wsg )->current == ICONTAINER( iobject ) )
		iobject_changed( IOBJECT( wsg ) );

	IOBJECT_CLASS( parent_class )->changed( iobject );
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
	Column *current = workspace_get_column( ws );

	/* Will we remove the current column? If yes, make sure
	 * current_column is NULL.
	 */
	if( current == col )
		workspace_column_select( ws, NULL );

	workspace_set_modified( ws, TRUE );

	ICONTAINER_CLASS( parent_class )->child_remove( parent, child );
}

static void
workspace_child_current( iContainer *parent, iContainer *child )
{
	Workspace *ws = WORKSPACE( parent );
	Column *col = COLUMN( child );
	Column *current = workspace_get_column( ws );

	if( current )
		current->selected = FALSE;
	if( col )
		col->selected = TRUE;

	ICONTAINER_CLASS( parent_class )->child_current( parent, child );
}

static void
workspace_link( Workspace *ws, Workspacegroup *wsg, const char *name )
{
	Workspaceroot *wsr = wsg->wsr;

	Symbol *sym;

#ifdef DEBUG
	printf( "workspace_link: naming ws %p as %s\n", ws, name );
#endif /*DEBUG*/

	sym = symbol_new_defining( wsr->sym->expr->compile, name );

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
	char buf[FILENAME_MAX];
	char *txt;

	g_assert( IS_WORKSPACEGROUP( parent ) );

	/* "view" is optional, for backwards compatibility.
	 */
	if( get_sprop( xnode, "view", buf, FILENAME_MAX ) ) {
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

	(void) get_bprop( xnode, "lpane_open", &ws->lpane_open );
	(void) get_iprop( xnode, "lpane_position", &ws->lpane_position );
	(void) get_bprop( xnode, "rpane_open", &ws->rpane_open );
	(void) get_iprop( xnode, "rpane_position", &ws->rpane_position );

	if( get_sprop( xnode, "name", buf, FILENAME_MAX ) ) {
		IM_SETSTR( IOBJECT( ws )->name, buf );
	}
	if( get_sprop( xnode, "caption", buf, FILENAME_MAX ) ) {
		IM_SETSTR( IOBJECT( ws )->caption, buf );
	}

	/* Don't use get_sprop() and avoid a limit on def size.
	 */
	if( (txt = (char *) xmlGetProp( xnode, (xmlChar *) "local_defs" )) ) {
		(void) workspace_local_set( ws, txt );
		IM_FREEF( xmlFree, txt );
	}

	(void) get_iprop( xnode, "major", &ws->compat_major );
	(void) get_iprop( xnode, "minor", &ws->compat_minor );

	if( !MODEL_CLASS( parent_class )->load( model, state, parent, xnode ) )
		return( FALSE );

	return( TRUE );
}

static xmlNode *
workspace_save( Model *model, xmlNode *xnode )
{
	Workspace *ws = WORKSPACE( model );
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );
	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( !set_sprop( xthis, "view", workspacemode_to_char( ws->mode ) ) ||
		!set_dprop( xthis, "scale", ws->scale ) ||
		!set_dprop( xthis, "offset", ws->offset ) ||
		!set_iprop( xthis, "lpane_position", ws->lpane_position ) ||
		!set_sprop( xthis, "lpane_open", 
			bool_to_char( ws->lpane_open ) ) ||
		!set_iprop( xthis, "rpane_position", ws->rpane_position ) ||
		!set_sprop( xthis, "rpane_open", 
			bool_to_char( ws->rpane_open ) ) ||
		!set_sprop( xthis, "local_defs", ws->local_defs ) ||
		!set_sprop( xthis, "name", IOBJECT( ws )->name ) ||
		!set_sprop( xthis, "caption", IOBJECT( ws )->caption ) ) 
		return( NULL );

	/* We have to save our workspacegroup's filename here for compt with
	 * older nip2.
	 */
	if( !set_sprop( xthis, "filename", FILEMODEL( wsg )->filename ) )
		return( NULL );

	if( ws->compat_major ) {
		if( !set_iprop( xthis, "major", ws->compat_major ) ||
			!set_iprop( xthis, "minor", ws->compat_minor ) )
			return( NULL );
	}

	return( xthis );
}

static void
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

static void *
workspace_load_toolkit( const char *filename, Toolkitgroup *toolkitgroup )
{
	if( !toolkit_new_from_file( toolkitgroup, filename ) ) 
		iwindow_alert( NULL, GTK_MESSAGE_ERROR );

	return( NULL );
}

/* The compat modes this version of nip2 has. Search the compat dir and make a
 * list of these things.
 */
#define MAX_COMPAT (100)
static int compat_major[MAX_COMPAT];
static int compat_minor[MAX_COMPAT];
static int n_compat = 0;

static void *
workspace_build_compat_fn( const char *filename )
{
	char *basename;
	int major;
	int minor;

	basename = g_path_get_basename( filename );

	if( sscanf( basename, "%d.%d", &major, &minor ) != 2 ) {
		g_free( basename );
		return( NULL );
	}
	g_free( basename );

	compat_major[n_compat] = major;
	compat_minor[n_compat] = minor;
	n_compat += 1;

#ifdef DEBUG
	printf( "workspace_build_compat_fn: found major = %d, minor = %d\n", 
		major, minor ); 
#endif /*DEBUG*/

	return( NULL );
}

/* Build the list of ws compatibility defs we have.
 */
static void
workspace_build_compat( void )
{
	if( n_compat > 0 )
		return;

	path_map_dir( "$VIPSHOME/share/" PACKAGE "/compat", "*.*", 
		(path_map_fn) workspace_build_compat_fn, NULL );
}

/* Given a major/minor (eg. read from a ws header), return non-zero if we have 
 * a set of compat defs.
 */
int
workspace_have_compat( int major, int minor, int *best_major, int *best_minor )
{
	int i;
	int best;

#ifdef DEBUG
	printf( "workspace_have_compat: searching for %d.%d\n", major, minor );
#endif /*DEBUG*/

	/* Sets of ws compatibility defs cover themselves and any earlier
	 * releases, as far back as the next set of compat defs. We need to
	 * search for the smallest compat version that's greater than the
	 * version number in the file.
	 */
	workspace_build_compat();
	best = -1;
	for( i = 0; i < n_compat; i++ ) 
		if( major <= compat_major[i] && minor <= compat_minor[i] ) 
			/* Found a possible compat set, is it better than the
			 * best we've seen so far?
			 */
			if( best == -1 ||
				compat_major[i] < compat_major[best] ||
				compat_minor[i] < compat_minor[best] )
				best = i;
	if( best == -1 )
		return( 0 );

#ifdef DEBUG
	printf( "\tfound %d.%d\n", compat_major[best], compat_minor[best] );
#endif /*DEBUG*/

	if( best_major )
		*best_major = compat_major[best];
	if( best_minor )
		*best_minor = compat_minor[best];

	return( 1 );
}

void
workspace_get_version( Workspace *ws, int *major, int *minor )
{
	if( ws->compat_major ) {
		*major = ws->compat_major;
		*minor = ws->compat_minor;
	}
	else {
		*major = MAJOR_VERSION;
		*minor = MINOR_VERSION;
	}
}

gboolean
workspace_load_compat( Workspace *ws, int major, int minor )
{
	char pathname[FILENAME_MAX];
	GSList *path;
	int best_major;
	int best_minor;

	if( workspace_have_compat( major, minor, &best_major, &best_minor ) ) {
		/* Make a private toolkitgroup local to this workspace to 
		 * hold the compatibility defs we are planning to load.
		 */
		UNREF( ws->kitg );
		ws->kitg = toolkitgroup_new( ws->sym );
		g_object_ref( G_OBJECT( ws->kitg ) );
		iobject_sink( IOBJECT( ws->kitg ) );

		im_snprintf( pathname, FILENAME_MAX, 
			"$VIPSHOME/share/" PACKAGE "/compat/%d.%d", 
			best_major, best_minor );
		path = path_parse( pathname );
		if( path_map( path, "*.def", 
			(path_map_fn) workspace_load_toolkit, ws->kitg ) ) {
			path_free2( path );
			return( FALSE );
		}
		path_free2( path );

		ws->compat_major = best_major;
		ws->compat_minor = best_minor;
	}
	else {
		/* No compat defs necessary for this ws. 
		 */
		ws->compat_major = 0;
		ws->compat_minor = 0;
	}

	return( TRUE );
}

static void
workspace_class_init( WorkspaceClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = workspace_dispose;
	gobject_class->finalize = workspace_finalize;

	iobject_class->changed = workspace_changed;
	iobject_class->user_name = _( "Tab" );

	icontainer_class->child_add = workspace_child_add;
	icontainer_class->child_remove = workspace_child_remove;
	icontainer_class->child_current = workspace_child_current;

	model_class->view_new = workspace_view_new;
	model_class->load = workspace_load;
	model_class->save = workspace_save;
	model_class->empty = workspace_empty;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
workspace_init( Workspace *ws )
{
	ws->sym = NULL;

	/* We default to using the main toolkitgroup for our definitions.
	 * Unref and load private defs if we need compatibility.
	 */
	ws->kitg = main_toolkitgroup;
	g_object_ref( G_OBJECT( ws->kitg ) );

	ws->next = 0;
	ws->selected = NULL;
	ws->errors = NULL;
        ws->mode = WORKSPACE_MODE_REGULAR;

	ws->compat_major = 0;
	ws->compat_minor = 0;

	ws->area.left = 0;
	ws->area.top = 0;
	ws->area.width = 0;
	ws->area.height = 0;
	ws->vp = ws->area;

	/* Overwritten by mainw.
	 */
	ws->lpane_open = WORKSPACE_RPANE_OPEN;
	ws->lpane_position = WORKSPACE_RPANE_POSITION;
	ws->rpane_open = WORKSPACE_LPANE_OPEN;
	ws->rpane_position = WORKSPACE_LPANE_POSITION;

	ws->status = NULL;

	ws->scale = 1.0;
	ws->offset = 0.0;

	ws->local_defs = im_strdupn( _( 
		"// private definitions for this workspace\n" ) );
	ws->local_kitg = NULL;
	ws->local_kit = NULL;

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

		workspace_type = g_type_register_static( TYPE_MODEL, 
			"Workspace", &info, 0 );
	}

	return( workspace_type );
}

Workspace *
workspace_new( Workspacegroup *wsg, const char *name )
{
	Workspaceroot *wsr = wsg->wsr;

	Workspace *ws;

#ifdef DEBUG
	printf( "workspace_new: %s\n", name );
#endif /*DEBUG*/

	if( compile_lookup( wsr->sym->expr->compile, name ) ) {
		error_top( _( "Name clash." ) );
		error_sub( _( "Can't create workspace \"%s\". "
			"A symbol with that name already exists." ), name );
		return( NULL );
	}

	ws = WORKSPACE( g_object_new( TYPE_WORKSPACE, NULL ) );
	workspace_link( ws, wsg, name );
	icontainer_child_add( ICONTAINER( wsg ), ICONTAINER( ws ), -1 );

	return( ws );
}

/* Make the blank workspace we present the user with (in the absence of
 * anything else).
 */
Workspace *
workspace_new_blank( Workspacegroup *wsg )
{
	char name[256];
	Workspace *ws;

	workspaceroot_name_new( wsg->wsr, name );
	if( !(ws = workspace_new( wsg, name )) )
		return( NULL );

	/* Make an empty column.
	 */
	(void) workspace_column_pick( ws );

	icontainer_child_current( ICONTAINER( wsg ), ICONTAINER( ws ) );

	iobject_set( IOBJECT( ws ), NULL, _( "Default empty tab" ) );

	return( ws );
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
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	/* Are there any selected symbols?
	 */
	vips_buf_appends( &buf, action );
	if( nparam > 0 && workspace_selected_any( ws ) ) {
		if( nparam != workspace_selected_num( ws ) ) {
			error_top( _( "Wrong number of arguments." ) );
			error_sub( _( "%s needs %d arguments, "
				"there are %d selected." ), 
				name, nparam,
				workspace_selected_num( ws ) );
			return( FALSE );
		}

		vips_buf_appends( &buf, " " );
		workspace_selected_names( ws, &buf, " " );
		if( vips_buf_is_full( &buf ) ) {
			error_top( _( "Overflow error." ) );
			error_sub( _( "Too many names selected." ) );
			return( FALSE );
		}

		if( !workspace_add_def_recalc( ws, vips_buf_all( &buf ) ) ) 
			return( FALSE );
		workspace_deselect_all( ws );
	}
	else {
		/* Try to use the previous n items in this column as the
		 * arguments. 
		 */
		if( !column_add_n_names( col, name, &buf, nparam ) || 
			!workspace_add_def_recalc( ws, vips_buf_all( &buf ) ) ) 
			return( FALSE );
	}

	return( TRUE );
}

int
workspace_number( void )
{
	return( g_slist_length( workspace_all ) );
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
	workspace_set_modified( ws, TRUE );

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
		nfn( sys, IWINDOW_YES );
	else
		nfn( sys, IWINDOW_ERROR );
}

/* Ask before removing selected.
 */
void
workspace_selected_remove_yesno( Workspace *ws, GtkWidget *parent )
{
	char txt[30];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

        if( !workspace_selected_any( ws ) ) 
		return;

	workspace_selected_names( ws, &buf, ", " );

	box_yesno( parent, 
		workspace_selected_remove_yesno_cb, iwindow_true_cb, ws, 
		iwindow_notify_null, NULL,
		GTK_STOCK_DELETE, 
		_( "Delete selected objects?" ),
		_( "Are you sure you want to delete %s?" ), vips_buf_all( &buf ) );
}

/* Sub fn of below ... add a new index expression.
 */
static gboolean
workspace_ungroup_add_index( Row *row, const char *fmt, int i )
{
	static char txt[200];
	static VipsBuf buf = VIPS_BUF_STATIC( txt );

	vips_buf_rewind( &buf );
	row_qualified_name( row, &buf );
	vips_buf_appendf( &buf, fmt, i );
	if( !workspace_add_def_recalc( row->ws, vips_buf_all( &buf ) ) )
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
			char txt[100];
			VipsBuf buf = VIPS_BUF_STATIC( txt );

			row_qualified_name( row, &buf );
			error_top( _( "Unable to ungroup." ) );
			error_sub( _( "Row \"%s\" is not a Group or a list." ), 
				vips_buf_all( &buf ) );  

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

/* Group the selected object(s).
 */
gboolean
workspace_selected_group( Workspace *ws )
{
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	if( !workspace_selected_any( ws ) ) {
		Row *row;

		if( !(row = workspace_get_bottom( ws )) )
			return( FALSE );
		row_select( row );
	}

	vips_buf_appends( &buf, "Group [" );
	workspace_selected_names( ws, &buf, "," );
	vips_buf_appends( &buf, "]" );
	if( !workspace_add_def_recalc( ws, vips_buf_all( &buf ) ) ) 
		return( FALSE );
	workspace_deselect_all( ws );

	return( TRUE );
}

static Row *
workspace_test_error( Row *row, Workspace *ws, int *found )
{
	g_assert( row->err );

	/* Found next?
	 */
	if( *found )
		return( row );

	if( row == ws->last_error ) {
		/* Found the last one ... return the next one.
		 */
		*found = 1;
		return( NULL );
	}

	return( NULL );
}

/* FALSE for no errors.
 */
gboolean
workspace_next_error( Workspace *ws )
{
	char txt[MAX_LINELENGTH];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	int found;

	if( !ws->errors ) 
		return( FALSE ); 

	/* Search for the one after the last one.
	 */
	found = 0;
	ws->last_error = (Row *) slist_map2( ws->errors, 
		(SListMap2Fn) workspace_test_error, ws, &found );

	/* NULL? We've hit end of table, start again.
	 */
	if( !ws->last_error ) {
		found = 1;
		ws->last_error = (Row *) slist_map2( ws->errors, 
			(SListMap2Fn) workspace_test_error, ws, &found );
	}

	/* *must* have one now.
	 */
	g_assert( ws->last_error && ws->last_error->err );

	model_scrollto( MODEL( ws->last_error ), MODEL_SCROLL_TOP );

	row_qualified_name( ws->last_error->expr->row, &buf );
	vips_buf_appends( &buf, ": " );
	vips_buf_appends( &buf, ws->last_error->expr->error_top );
	workspace_set_status( ws, "%s", vips_buf_firstline( &buf ) );

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

	workspace_set_modified( ws, TRUE );
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

	if( !(of = ifile_open_read( "%s", fname )) ) 
		return( FALSE );
	if( !(txt = ifile_read( of )) ) {
		ifile_close( of );
		return( FALSE );
	}
	if( !workspace_local_set( ws, txt ) ) {
		g_free( txt );
		ifile_close( of );
		return( FALSE );
	}

	filemodel_set_filename( FILEMODEL( ws->local_kit ), fname );

	g_free( txt );
	ifile_close( of );

	return( TRUE );
}

static gint
workspace_jump_name_compare( iContainer *a, iContainer *b )
{
	int la = strlen( IOBJECT( a )->name );
	int lb = strlen( IOBJECT( b )->name );

	/* Smaller names first.
	 */
	if( la == lb )
		return( strcmp( IOBJECT( a )->name, IOBJECT( b )->name ) );
	else
		return( la - lb );
}

static void
workspace_jump_column_cb( GtkWidget *item, Column *column )
{
	model_scrollto( MODEL( column ), MODEL_SCROLL_TOP );
}

static void *
workspace_jump_build( Column *column, GtkWidget *menu )
{
	GtkWidget *item;
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	vips_buf_appendf( &buf, "%s - %s", 
		IOBJECT( column )->name, IOBJECT( column )->caption );
	item = gtk_menu_item_new_with_label( vips_buf_all( &buf ) );
	g_signal_connect( item, "activate",
		G_CALLBACK( workspace_jump_column_cb ), column );
	gtk_menu_append( GTK_MENU( menu ), item );
	gtk_widget_show( item );

	return( NULL );
}

/* Update a menu with the set of current columns.
 */
void
workspace_jump_update( Workspace *ws, GtkWidget *menu )
{
	GtkWidget *item;
	GSList *columns;

	gtk_container_foreach( GTK_CONTAINER( menu ),
		(GtkCallback) gtk_widget_destroy, NULL );

	item = gtk_tearoff_menu_item_new();
	gtk_menu_append( GTK_MENU( menu ), item );
	gtk_widget_show( item );

	columns = icontainer_get_children( ICONTAINER( ws ) );

        columns = g_slist_sort( columns, 
		(GCompareFunc) workspace_jump_name_compare );
	slist_map( columns, (SListMapFn) workspace_jump_build, menu );

	g_slist_free( columns );
}

/* Merge file into this workspace. 
 */
gboolean
workspace_merge_file( Workspace *ws, const char *filename )
{
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	icontainer_child_current( ICONTAINER( wsg ), ICONTAINER( ws ) );

	return( workspacegroup_merge_columns( wsg, filename ) );
}

/* Duplicate selected rows in this workspace.
 */
gboolean 
workspace_selected_duplicate( Workspace *ws )
{
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	char filename[FILENAME_MAX];

	if( !workspace_selected_any( ws ) ) {
		Row *row;

		if( !(row = workspace_get_bottom( ws )) )
			return( FALSE );

		row_select( row );
	}

	if( !temp_name( filename, "ws" ) )
		return( FALSE );
	if( !workspace_selected_save( ws, filename ) ) 
		return( FALSE );

        progress_begin();

	if( !workspacegroup_merge_rows( wsg, filename ) ) {
		progress_end();
		unlinkf( "%s", filename );

		return( FALSE );
	}
	unlinkf( "%s", filename );

	symbol_recalculate_all();
	workspace_deselect_all( ws );
	model_scrollto( MODEL( workspace_get_column( ws ) ), MODEL_SCROLL_TOP );

	progress_end();

	return( TRUE );
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
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	Rect box = { 0 };

	icontainer_child_current( ICONTAINER( wsg ), ICONTAINER( ws ) );

	workspace_map_column( ws, 
		(column_map_fn) workspace_selected_save_box, 
		&box );

	filemodel_set_offset( FILEMODEL( wsg ), box.left, box.top );

	if( !workspacegroup_save_selected( wsg, filename ) ) 
		return( FALSE );

	return( TRUE );
}

gboolean
workspace_rename( Workspace *ws, const char *name, const char *caption )
{
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	if( !symbol_rename( ws->sym, name ) )
		return( FALSE );

	g_object_ref( ws );

	icontainer_child_remove( ICONTAINER( ws ) );
	iobject_set( IOBJECT( ws ), IOBJECT( ws->sym )->name, caption );
	icontainer_child_add( ICONTAINER( wsg ), ICONTAINER( ws ),
		ICONTAINER( ws )->pos );

	g_object_unref( ws );

	// do we need this? unclear
	//iobject_changed( IOBJECT( wsg ) );

	return( TRUE );
}

gboolean
workspace_duplicate( Workspace *ws )
{
	Workspacegroup *wsg = workspace_get_workspacegroup( ws );

	char filename[FILENAME_MAX];

	if( !temp_name( filename, "ws" ) )
		return( FALSE );
	icontainer_child_current( ICONTAINER( wsg ), ICONTAINER( ws ) );
	if( !workspacegroup_save_current( wsg, filename ) ) 
		return( FALSE );

        progress_begin();

	if( !workspacegroup_merge_workspaces( wsg, filename ) ) {
		progress_end();
		unlinkf( "%s", filename );

		return( FALSE );
	}
	unlinkf( "%s", filename );

	symbol_recalculate_all();

	progress_end();

	return( TRUE );
}


