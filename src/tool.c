/* Manage toolkits and their display.
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
#define DEBUG_VERBOSE
#define DEBUG_MENUS
#define DEBUG
#define DEBUG_TOOLITEM
 */

#include "ip.h"

static FilemodelClass *parent_class = NULL;

/* Largest string we let the user set for name/tip/etc.
 */
#define MAX_NAME (256)

/* Remove a tool. Also strip the sym, if any.
 */
static void 
tool_dispose( GObject *gobject )
{	
	Tool *tool = TOOL( gobject );

#ifdef DEBUG
	printf( "tool_dispose: destroying tool for " );
	if( tool->sym )
		symbol_name_print( tool->sym );
	else
		printf( "anonymous-tool" );
	printf( " at addr %p\n", tool );
#endif /*DEBUG*/

	FREESID( tool->new_value_sid, tool->link_sym );

	/* Unlink from symbol and toolkit. This changes the kit - mark it as
	 * dirty.
	 */
	if( tool->sym ) {
		Symbol *sym = tool->sym;

		sym->tool = NULL;
		tool->sym = NULL;

		symbol_strip( sym );
	}

	filemodel_set_modified( FILEMODEL( tool->kit ), TRUE );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static View *
tool_view_new( Model *model, View *parent )
{
	return( toolview_new() );
}

/* Save a tool's definition to a file. 
 */
static gboolean
tool_save_text( Model *model, iOpenFile *of )
{
	Tool *tool = TOOL( model );
	Symbol *sym = tool->sym;

	switch( tool->type ) {
	case TOOL_SYM:
		if( sym->expr )
			if( !file_write( of, 
				"%s;\n\n", sym->expr->compile->text ) )
				return( FALSE );
		break;

	case TOOL_SEP:
		if( !file_write( of, "#separator\n\n" ) )
			return( FALSE );
		break;

	case TOOL_DIA:
		if( !file_write( of, "#dialog \"%s\" \"%s\"\n\n",
			IOBJECT( tool )->name, FILEMODEL( tool )->filename ) )
			return( FALSE );
		break;

	default:
		assert( FALSE );
	}

	return( TRUE );
}

static char *
tool_type_to_char( Tooltype type )
{
	switch( type ) {
	case TOOL_SYM:	return( "symbol" );
	case TOOL_DIA:	return( "dialog" );
	case TOOL_SEP:	return( "separator" );

	default:
		assert( FALSE );
	}
}

static void
tool_info( iObject *iobject, BufInfo *buf )
{
	Tool *tool = TOOL( iobject );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );

	buf_appendf( buf, "type = \"%s\"\n", tool_type_to_char( tool->type ) );
	if( tool->type == TOOL_SYM )
		buf_appendf( buf, "symbol = \"%s\"\n", 
			IOBJECT( tool->sym )->name );
	if( tool->lineno != -1 )
		buf_appendf( buf, "lineno = %d\n", tool->lineno );
	if( tool->kit )
		buf_appendf( buf, "toolkit = \"%s\"\n", 
			IOBJECT( tool->kit )->name );
}

static void
tool_parent_add( iContainer *child )
{
        Tool *tool = TOOL( child );
        Toolkit *kit = TOOLKIT( child->parent );

        tool->kit = kit;

        ICONTAINER_CLASS( parent_class )->parent_add( child );
}

static void
tool_class_init( ToolClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = tool_dispose;

	iobject_class->info = tool_info;

	icontainer_class->parent_add = tool_parent_add;

	model_class->view_new = tool_view_new;
	model_class->save_text = tool_save_text;
}

static void
tool_init( Tool *tool )
{
        tool->type = TOOL_SEP;
        tool->sym = NULL;
        tool->kit = NULL;
        tool->lineno = -1;
}

GType
tool_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ToolClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) tool_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Tool ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) tool_init,
		};

		type = g_type_register_static( TYPE_FILEMODEL, 
			"Tool", &info, 0 );
	}

	return( type );
}

/* Add a tool to a toolkit.
 */
static void
tool_link( Tool *tool, Toolkit *kit, int pos, const char *name )
{
	filemodel_set_modified( FILEMODEL( kit ), TRUE );
	iobject_set( IOBJECT( tool ), name, NULL );
	icontainer_child_add( ICONTAINER( kit ), ICONTAINER( tool ), pos );
}

static void *
toolitem_free( Toolitem *toolitem )
{
	Toolitem *parent = toolitem->parent;

#ifdef DEBUG_TOOLITEM
	printf( "toolitem_free: %s\n", toolitem->name );
#endif /*DEBUG_TOOLITEM*/

	slist_map( toolitem->children, (SListMapFn) toolitem_free, NULL );

	assert( !toolitem->children );

	if( parent ) {
		parent->children = g_slist_remove( parent->children, toolitem );
		toolitem->parent = NULL;
	}

	IM_FREE( toolitem->label );
	IM_FREE( toolitem->name );
	IM_FREE( toolitem->icon );
	IM_FREE( toolitem->tooltip );
	IM_FREE( toolitem->help );
	IM_FREE( toolitem->action );
	IM_FREE( toolitem->path );
	IM_FREE( toolitem->user_path );
	IM_FREE( toolitem );

	return( NULL );
}

static Toolitem *
toolitem_new( Toolitem *parent, Compile *compile, Tool *tool )
{
	Toolitem *toolitem;

	if( !(toolitem = INEW( NULL, Toolitem )) )
		return( NULL );
	toolitem->compile = compile;
	toolitem->tool = tool;
	toolitem->action_sym = NULL;

	toolitem->is_separator = FALSE;
	toolitem->is_pullright = FALSE;
	toolitem->children = NULL;
	toolitem->parent = parent;
	toolitem->is_action = FALSE;

	toolitem->label = NULL;
	toolitem->name = NULL;
	toolitem->icon = NULL;
	toolitem->tooltip = NULL;
	toolitem->help = NULL;
	toolitem->action = NULL;
	toolitem->path = NULL;
	toolitem->user_path = NULL;

	if( parent ) 
		parent->children = 
			g_slist_append( parent->children, toolitem );

	return( toolitem );
}

/* Set label & name & icon.

	FIXME ... we will do repeated heap_is_instanceof() during item build,
	do it once and set a flag instead

 */
static void
toolitem_set_name( Toolitem *toolitem, PElement *root )
{
	gboolean result;
	char value[MAX_NAME];
	int i;

	if( root && 
		heap_is_instanceof( CLASS_MENUITEM, root, &result ) && 
		result ) {
		if( class_get_member_string( root, 
			MEMBER_LABEL, value, MAX_NAME ) ) {
			/* Save the i18n-ed version.
			 */
			IM_SETSTR( toolitem->label, _( value ) );

			/* Strip underscores (they mark mnemonics). Can't use
			 * strrcpy(), we have overlapping blocks.
			 */
			im_strncpy( value, toolitem->label, MAX_NAME );
			for( i = 0; value[i]; i++ )
				if( value[i] == '_' )
					memccpy( value + i, value + i + 1,
						0, MAX_NAME - i );
			IM_SETSTR( toolitem->name, value );
		}

		if( class_get_member_string( root, 
			MEMBER_ICON, value, MAX_NAME ) ) 
			IM_SETSTR( toolitem->icon, value );
	}
	else {
		/* Remove underscores from the object name ... we don't want
		 * them to be mnemonics.
		 */
		im_strncpy( value, 
			IOBJECT( toolitem->compile->sym )->name, MAX_NAME );
		for( i = 0; value[i]; i++ )
			if( value[i] == '_' )
				value[i] = ' ';

		IM_SETSTR( toolitem->label, value );
		IM_SETSTR( toolitem->name, toolitem->label );
	}

	if( root && 
		heap_is_instanceof( CLASS_MENUSEPARATOR, root, &result ) && 
		result ) 
		toolitem->is_separator = TRUE;
}

static void
toolitem_set_tooltip( Toolitem *toolitem, PElement *root )
{
	gboolean result;
	char value[MAX_NAME];

	if( root && 
		heap_is_instanceof( CLASS_MENUITEM, root, &result ) &&
		result &&
		class_get_member_string( root, 
			MEMBER_TOOLTIP, value, MAX_NAME ) ) {
		IM_SETSTR( toolitem->tooltip, _( value ) );
	}
	else {
		char *p;

		if( (p = toolitem->compile->text) ) {
			/* Skip leading whitespace.
			 */
			while( isspace( (int)(*p) ) )
				p++;

			/* Skip leading comment, if any.
			 */
			if( p[0] == '/' && p[1] == '*' )
				p += 2;
			else if( p[0] == '/' && p[1] == '/' )
				p += 2;

			/* Skip more whitespace.
			 */
			while( isspace( (int)(*p) ) )
				p++;

			/* Limit to MAX_NAME chars or 1st line. Strip trailing
			 * whitespace.
			 */
			im_strncpy( value, p, MAX_NAME );
			if( (p = strchr( value, '\n' )) )
				*p = '\0';
			*((char *) my_strrspn( value, WHITESPACE )) = '\0';
			IM_SETSTR( toolitem->tooltip, value );
		}
	}
}

static void
toolitem_set_pullright( Toolitem *toolitem, PElement *root )
{
	gboolean result;

	/* New-style pullright?
	 */
	if( root && 
		heap_is_instanceof( CLASS_MENUPULLRIGHT, root, &result ) &&
		result ) 
		toolitem->is_pullright = TRUE;
	/* Old-style pullright?
	 */
	else if( is_value( toolitem->compile->sym ) && 
		is_class( toolitem->compile ) && 
		!toolitem->compile->has_super && 
		toolitem->compile->nparam == 0 ) 
		toolitem->is_pullright = TRUE;
}

static void
toolitem_set_action( Toolitem *toolitem, PElement *root )
{
	gboolean result;
	char txt[256];
	BufInfo buf;

	buf_init_static( &buf, txt, 256 );

	if( toolitem->parent )
		buf_appendf( &buf, "%s.", toolitem->parent->action );
	buf_appendf( &buf, "%s", IOBJECT( toolitem->compile->sym )->name );

	/* If this is a Menuaction, we need the action member.
	 */
	if( root && 
		heap_is_instanceof( CLASS_MENUACTION, root, &result ) &&
		result ) {
		PElement out;

		toolitem->is_action = TRUE;
		(void) class_get_member( root, 
			MEMBER_ACTION, &toolitem->action_sym, &out );
	}

	/* If there's an action member, use that. 
	 */
	if( toolitem->is_action )
		buf_appends( &buf, "." MEMBER_ACTION ); 

	IM_SETSTR( toolitem->action, buf_all( &buf ) );

	/* No action member found and this is an item (ie. not a pullright)?
	 * Default to the sym itself.
	 */
	if( !toolitem->action_sym && !toolitem->is_pullright )
		toolitem->action_sym = toolitem->compile->sym;
}

static void
toolitem_set_path( Toolitem *toolitem )
{
	char txt[256];
	BufInfo buf;

	buf_init_static( &buf, txt, 256 );
	if( toolitem->parent )
		buf_appendf( &buf, "%s", toolitem->parent->path );
	else 
		buf_appendf( &buf, "<mainw>/Toolkits/%s", 
			IOBJECT( toolitem->tool->kit )->name );
	buf_appendf( &buf, "/%s", toolitem->name );
	IM_SETSTR( toolitem->path, buf_all( &buf ) );
}

static void
toolitem_set_user_path( Toolitem *toolitem )
{
	char txt[256];
	BufInfo buf;

	buf_init_static( &buf, txt, 256 );
	if( toolitem->parent )
		buf_appends( &buf, toolitem->parent->user_path );
	else
		buf_appends( &buf, IOBJECT( toolitem->tool->kit )->name );
	buf_appendf( &buf, " / %s", toolitem->name );
	IM_SETSTR( toolitem->user_path, buf_all( &buf ) );
}

static void *
toolitem_set_help_sub( Symbol *param, BufInfo *buf )
{
	buf_appends( buf, " " );
	buf_appends( buf, IOBJECT( param )->name );

	return( NULL );
}

static void
toolitem_set_help( Toolitem *toolitem )
{
	char txt[256];
	BufInfo buf;

	buf_init_static( &buf, txt, 256 );
	buf_appends( &buf, toolitem->name );

	/* Get the params from the action member if we can.
	 */
	if( toolitem->action_sym && 
		toolitem->action_sym->expr &&
		toolitem->action_sym->expr->compile->param ) 
		slist_map( toolitem->action_sym->expr->compile->param,
			(SListMapFn) toolitem_set_help_sub, &buf );

	buf_appends( &buf, ": " );
	if( toolitem->tooltip )
		buf_appends( &buf, toolitem->tooltip );
	IM_SETSTR( toolitem->help, buf_firstline( &buf ) );
}

static Toolitem *
toolitem_build( Tool *tool, 
	Compile *compile, PElement *root, Toolitem *parent )
{
	Toolitem *toolitem;

	if( !(toolitem = toolitem_new( parent, compile, tool )) )
		return( NULL );

	toolitem_set_name( toolitem, root );
	toolitem_set_tooltip( toolitem, root );
	toolitem_set_pullright( toolitem, root );
	toolitem_set_action( toolitem, root );
	toolitem_set_path( toolitem );
	toolitem_set_user_path( toolitem );
	toolitem_set_help( toolitem );

#ifdef DEBUG_TOOLITEM
	printf( "toolitem_build: %s\n", toolitem->name );
#endif /*DEBUG_TOOLITEM*/

#ifdef DEBUG_VERBOSE
	printf( "toolitem_build:\n" );
	printf( "\tpullright = %d\n", toolitem->is_pullright );
	printf( "\tlabel = \"%s\"\n", toolitem->label );
	printf( "\tname = \"%s\"\n", toolitem->name );
	printf( "\ticon = \"%s\"\n", toolitem->icon );
	printf( "\ttooltip = \"%s\"\n", toolitem->tooltip );
	printf( "\thelp = \"%s\"\n", toolitem->help );
	printf( "\taction = \"%s\"\n", toolitem->action );
	printf( "\tpath = \"%s\"\n", toolitem->path );
	printf( "\tuser_path = \"%s\"\n", toolitem->user_path );
#endif /*DEBUG_VERBOSE*/

	return( toolitem );
}

static Toolitem *
toolitem_build_all( Tool *tool, Compile *compile, PElement *root,
	Toolitem *parent );

static void *
toolitem_build_all_sub( Symbol *sym, Toolitem *parent )
{
	if( is_menuable( sym ) )
		(void) toolitem_build_all( parent->tool, sym->expr->compile, 
			NULL, parent );

	return( NULL );
}

static Toolitem *
toolitem_build_all( Tool *tool, Compile *compile, PElement *root,
	Toolitem *parent )
{
	Toolitem *toolitem;
	gboolean result;

	if( !(toolitem = toolitem_build( tool, compile, root, parent )) )
		return( NULL );

	/* If this is a dynamic pullright, walk the heap to find the members.
	 */
	if( toolitem->is_pullright && root && 
		heap_is_instanceof( CLASS_MENUPULLRIGHT, root, &result ) &&
		result ) {
		PElement member;
		HeapNode *p;

		PEGETCLASSMEMBER( &member, root );

		if( PEISNODE( &member ) )
			for( p = PEGETVAL( &member ); p; p = GETRIGHT( p ) ) {
				PElement s, v;
				HeapNode *hn;
				Symbol *sym;

				/* Get the sym/value pair.
				 */
				hn = GETLEFT( p );
				PEPOINTLEFT( hn, &s );
				PEPOINTRIGHT( hn, &v );
				sym = SYMBOL( PEGETSYMREF( &s ) );

				/* Ignore this/super/check etc.
				 */
				if( !is_menuable( sym ) )
					continue;

				/* For dynamic menus, only make items for
				 * things which are subclasses of menu.
				 */
				if( !heap_is_instanceof( CLASS_MENU, 
					&v, &result ) || !result ) 
					continue;

				(void) toolitem_build_all( tool, 
					sym->expr->compile, &v, toolitem );
			}
	}
	else if( toolitem->is_pullright ) {
		/* A static pullright... just walk the container.
		 */
		(void) icontainer_map( ICONTAINER( compile ),
			(icontainer_map_fn) toolitem_build_all_sub,
			toolitem, NULL );
	}

	return( toolitem );
}

#ifdef DEBUG_MENUS
static void 
toolitem_print( Toolitem *toolitem ) 
{
	if( toolitem->is_separator )
		printf( "-----------\n" );
	else 
		printf( "%s --- %s\n", 
			NN( toolitem->user_path ), NN( toolitem->help ) );
}

static void *
toolitem_print_all( Toolitem *toolitem )
{
	if( toolitem->is_pullright )
		slist_map( toolitem->children, 
			(SListMapFn) toolitem_print_all, NULL );
	else
		toolitem_print( toolitem );

	return( NULL );
}
#endif /*DEBUG_MENUS*/

/* Rebuild the toolitem tree.
 */
static void
tool_toolitem_rebuild( Tool *tool )
{
	IM_FREEF( toolitem_free, tool->toolitem );

	switch( tool->type ) {
	case TOOL_SYM:
		if( is_menuable( tool->sym ) )
			tool->toolitem = toolitem_build_all( tool, 
				tool->sym->expr->compile, 
				&tool->sym->expr->root, 
				NULL ); 
		break;

	case TOOL_DIA:
		if( (tool->toolitem = toolitem_new( NULL, NULL, tool )) ) 
			IM_SETSTR( tool->toolitem->label, 
				IOBJECT( tool )->name );
		break;

	case TOOL_SEP:
		if( (tool->toolitem = toolitem_new( NULL, NULL, tool )) )
			tool->toolitem->is_separator = TRUE;
		break;

	default:
		assert( 0 );
	}

	iobject_changed( IOBJECT( tool ) );

#ifdef DEBUG_MENUS
	if( tool->toolitem )
		toolitem_print_all( tool->toolitem );
#endif /*DEBUG_MENUS*/
}

/* The expr has a new value.
 */
static void
tool_new_value_cb( Symbol *sym, Tool *tool )
{
#ifdef DEBUG
	printf( "tool_new_value_cb: new value for " );
	symbol_name_print( sym );
	printf( "\n" );
#endif /*DEBUG*/

	tool_toolitem_rebuild( tool );
}

/* Add a symbol to a toolkit. 
 */
Tool *
tool_new_sym( Toolkit *kit, int pos, Symbol *sym )
{
	Tool *tool;

	assert( kit && sym );

	/* Is there a tool we can reuse? Don't update pos .. assume we want to
	 * keep the old one.
	 */
	if( sym->tool && sym->tool->kit == kit ) {
		sym->tool->lineno = -1;
		return( sym->tool );
	}

	/* Junk any existing tool for this sym.
	 */
	if( (tool = sym->tool) ) {
		sym->tool = NULL;
		tool->sym = NULL;

		iobject_destroy( IOBJECT( tool ) );
	}

	tool = TOOL( g_object_new( TYPE_TOOL, NULL ) );
	tool->type = TOOL_SYM;
	tool->sym = sym;
	sym->tool = tool;
	tool->new_value_sid = g_signal_connect( sym, "new_value", 
		G_CALLBACK( tool_new_value_cb ), tool );
	tool->link_sym = sym;
	tool_link( tool, kit, pos, IOBJECT( sym )->name );

#ifdef DEBUG
	printf( "tool_new_sym: new tool for " );
	symbol_name_print( sym );
	printf( " at %p\n", tool );
#endif /*DEBUG*/

	return( tool );
}

/* Add a separator to a toolkit. 
 */
Tool *
tool_new_sep( Toolkit *kit, int pos )
{
	Tool *tool;

	assert( kit );

	tool = TOOL( g_object_new( TYPE_TOOL, NULL ) );
	tool->type = TOOL_SEP;
	iobject_set( IOBJECT( tool ), "separator", NULL );
	tool_link( tool, kit, pos, NULL );
	tool_toolitem_rebuild( tool );

	return( tool );
}

/* Search a kit for a tool by tool name. Used for searching for dialogs ... we
 * can't use the symtable stuff, as they're not syms.
 */
static Tool *
tool_find( Toolkit *kit, const char *name )
{
	return( (Tool *) icontainer_map( ICONTAINER( kit ), 
		(icontainer_map_fn) iobject_test_name, (char *) name, NULL ) );
}

/* Add a dialog entry to a toolkit. 
 */
Tool *
tool_new_dia( Toolkit *kit, int pos, 
	const char *name, const char *filename )
{
	Tool *tool;

	assert( kit && name && filename );

	if( (tool = tool_find( kit, name )) ) {
		if( tool->type != TOOL_DIA ) {
			error_top( _( "Name clash." ) );
			error_sub( _( "Can't create dialog with name \"%s\", "
				"an object with that name already exists in "
				"kit \"%s\"." ), 
				name, IOBJECT( kit )->name );
			return( NULL );
		}

		/* Just update the filename.
		 */
		filemodel_set_filename( FILEMODEL( tool ), filename );
		tool->lineno = -1;
	}
	else {
		tool = TOOL( g_object_new( TYPE_TOOL, NULL ) );
		tool->type = TOOL_DIA;
		filemodel_set_filename( FILEMODEL( tool ), filename );
		iobject_set( IOBJECT( tool ), name, NULL );
		tool_link( tool, kit, pos, NULL );
	}

	tool_toolitem_rebuild( tool );

	return( tool );
}

static Toolitem *
toolitem_lookup_toolitem( Toolitem *toolitem, Symbol *action )
{
	if( toolitem->action_sym == action )
		return( toolitem );
	else
		return( (Toolitem *) slist_map( toolitem->children,
			(SListMapFn) toolitem_lookup_toolitem, action ) );
}

static Toolitem *
toolitem_lookup_tool( Tool *tool, Symbol *action )
{
	if( tool->toolitem )
		return( toolitem_lookup_toolitem( tool->toolitem, action ) );
	else
		return( NULL );
}

static Toolitem *
toolitem_lookup_toolkit( Toolkit *kit, Symbol *action )
{
	return( (Toolitem *) toolkit_map( kit, 
		(tool_map_fn) toolitem_lookup_tool,
		action, NULL ) );
}

/* Just walk the whole kit. Could use a hash in kitg, but we don't call this
 * so often.
 */
Toolitem *
toolitem_lookup( Toolkitgroup *kitg, Symbol *action )
{
	return( (Toolitem *) toolkitgroup_map( kitg,
		(toolkit_map_fn) toolitem_lookup_toolkit, 
		action, NULL ) );
}
