/* Basic ops on symbols.
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

/* All debug
#define DEBUG
 */

/* Just trace create/destroy.
#define DEBUG_MAKE
 */

/* Time recomputes.
#define DEBUG_TIME
 */

/* If DEBUG is on, make sure other debugs are on too.
 */
#ifdef DEBUG
#ifndef DEBUG_MAKE
#define DEBUG_MAKE
#endif
#ifndef DEBUG_TIME
#define DEBUG_TIME
#endif
#endif

/* Our signals. 
 */
enum {
	SIG_NEW_VALUE,		/* new value for sym->expr */
	SIG_LAST
};

static guint symbol_signals[SIG_LAST] = { 0 };

/* Global symbol - top-level definitions are locals to this symbol.
 */
Symbol *symbol_root = NULL;

/* Set of dirty top-level symbols with no dirty children which do not contain
 * errors. Used to generate next-to-recalc.
 */
static GSList *symbol_leaf_set = NULL;

static FilemodelClass *parent_class = NULL;

/* Apply a function to a symbol ... and any locals.
 */
Symbol *
symbol_map_all( Symbol *sym, symbol_map_fn fn, void *a, void *b )
{
	Symbol *res;

	/* Apply to this sym.
	 */
	if( (res = fn( sym, a, b, NULL )) )
		return( res );

	/* And over any locals of those locals.
	 */
	if( sym->expr && sym->expr->compile && 
		(res = icontainer_map3( ICONTAINER( sym->expr->compile ), 
			(icontainer_map3_fn) symbol_map_all, 
			(void *) fn, a, b )) )
		return( res );

	return( NULL );
}

/* Find a symbol's enclosing sym.
 */
Symbol *
symbol_get_parent( Symbol *sym )
{
	if( !ICONTAINER( sym )->parent )
		return( NULL );

	return( COMPILE( ICONTAINER( sym )->parent )->sym );
}

/* Find the enclosing workspace, if any. 
 */
Workspace *
symbol_get_workspace( Symbol *sym )
{
	if( !sym->expr || !sym->expr->row )
		return( NULL );

	return( row_get_workspace( sym->expr->row ) );
}

/* Find the enclosing tool, if any.
 */
Tool *
symbol_get_tool( Symbol *sym )
{
	Symbol *i;

	for( i = sym; i && !i->tool; i = symbol_get_parent( i ) )
		;
	if( i )
		return( i->tool );

	return( NULL );
}

/* Get the enclosing scope for a sym.
 */
Symbol *
symbol_get_scope( Symbol *sym )
{
	Symbol *i;

	for( i = sym; i && !is_scope( i ); i = symbol_get_parent( i ) )
		;

	return( i );
}

/* Make a fully-qualified symbol name .. eg fred.jim, given jim. Don't print
 * static scopes.
 */
void
symbol_qualified_name( Symbol *sym, BufInfo *buf )
{
	Symbol *parent = symbol_get_parent( sym );

	if( parent && !is_scope( parent ) ) {
		symbol_qualified_name( parent, buf );
		buf_appends( buf, "." );
	}

	buf_appends( buf, NN( IOBJECT( sym )->name ) );
}

/* Make a symbol name relative to a scope context ... ie. from the point of
 * view of a local of context, what name will find sym.
 */
void
symbol_qualified_name_relative( Symbol *context, Symbol *sym, BufInfo *buf )
{
	Symbol *parent = symbol_get_parent( sym );

	if( parent && !is_ancestor( context, parent ) ) {
		symbol_qualified_name_relative( context, parent, buf );
		buf_appends( buf, "." );
	}

	buf_appends( buf, NN( IOBJECT( sym )->name ) );
}

/* Handy for error messages ... but nowt else. Return string overwritten on
 * next call.
 */
const char *
symbol_name( Symbol *sym )
{
	static BufInfo buf;
	static char txt[200];

	buf_init_static( &buf, txt, 200 );
	symbol_qualified_name( sym, &buf );

	return( buf_all( &buf ) );
}

/* Convenience ... print a qual name to stdout.
 */
void
symbol_name_print( Symbol *sym )
{
	printf( "%s", symbol_name( sym ) );
}

/* Print a symbol's name, including the enclosing static scope. Return value
 * is a pointer to a static buffer :(
 */
const char *
symbol_name_scope( Symbol *sym )
{
	Symbol *scope = symbol_get_scope( sym );

	static BufInfo buf;
	static char txt[200];

	buf_init_static( &buf, txt, 200 );
	buf_appends( &buf, NN( IOBJECT( scope )->name ) );
	buf_appends( &buf, "." );
	symbol_qualified_name_relative( scope, sym, &buf );

	return( buf_all( &buf ) );
}

/* Convenience ... print a qual name to stdout.
 */
void
symbol_name_scope_print( Symbol *sym )
{
	printf( "%s", symbol_name_scope( sym ) );
}

void 
symbol_new_value( Symbol *sym )
{
	g_signal_emit( G_OBJECT( sym ), symbol_signals[SIG_NEW_VALUE], 0 );
}

/* Add a pointer to a patch list.
 */
void *
symbol_patch_add( void **pnt, Symbol *sym )
{
	assert( sym->type == SYM_ZOMBIE );

	sym->patch = g_slist_prepend( sym->patch, pnt );

	return( NULL );
}

static void
symbol_clear( Symbol *sym )
{
	sym->type = SYM_ZOMBIE;

	sym->patch = NULL;

	sym->expr = NULL;

	sym->base.type = ELEMENT_NOVAL;
	sym->base.ele = (void *) 15;	/* handy for debugging */

	sym->dirty = FALSE;
	sym->parents = NULL;

	sym->topchildren = NULL;
	sym->topparents = NULL;
	sym->ndirtychildren = 0;
	sym->leaf = FALSE;

	sym->generated = FALSE;
	sym->placeholder = FALSE;

	sym->tool = NULL;

	sym->function = NULL;

	sym->builtin = NULL;

	sym->wsg = NULL;

	sym->ws = NULL;
}

/* Initialise root symbol.
 */
Symbol *
symbol_root_init( void )
{
	Symbol *root = SYMBOL( g_object_new( TYPE_SYMBOL, NULL ) );

	symbol_clear( root );
	iobject_set( IOBJECT( root ), "$$ROOT", NULL );
	root->type = SYM_ROOT;
	root->expr = expr_new( root );
	(void) compile_new_local( root->expr );

	symbol_root = symbol_new( root->expr->compile, "root" );
	symbol_root->type = SYM_ROOT;
	symbol_root->expr = expr_new( symbol_root );
	(void) compile_new( symbol_root->expr );

	return( root );
}

/* Should a symbol be in the leaf set?
 */
static gboolean
symbol_is_leafable( Symbol *sym )
{
	if( is_top( sym ) && 
		sym->dirty && 
		sym->expr && 
		!sym->expr->err && 
		sym->ndirtychildren == 0 )
		return( TRUE );

	return( FALSE );
}

#ifdef DEBUG
/* Do a sanity check on a symbol.
 */
void *
symbol_sanity( Symbol *sym ) 
{
	if( is_top( sym ) ) {
		if( symbol_ndirty( sym ) != sym->ndirtychildren )
			error( "sanity failure #1 for sym \"%s\"", 
				symbol_name( sym ) );
	}

	if( symbol_is_leafable( sym ) && !sym->leaf )
		error( "sanity failure #2 for sym \"%s\"", symbol_name( sym ) );
	if( !symbol_is_leafable( sym ) && sym->leaf )
		error( "sanity failure #3 for sym \"%s\"", symbol_name( sym ) );
	if( sym->leaf && !g_slist_find( symbol_leaf_set, sym ) )
		error( "sanity failure #6 for sym \"%s\"", symbol_name( sym ) );
	if( !sym->leaf && g_slist_find( symbol_leaf_set, sym ) )
		error( "sanity failure #7 for sym \"%s\"", symbol_name( sym ) );

	return( NULL );
}
#endif/*DEBUG*/

#ifdef DEBUG
/* Test the leaf set for sanity.
 */
void
symbol_leaf_set_sanity( void )
{
	slist_map( symbol_leaf_set, (SListMapFn) symbol_sanity, NULL );
	icontainer_map( ICONTAINER( symbol_root->expr->compile ),
		(icontainer_map_fn) symbol_sanity, NULL, NULL );

	/* Commented out to reduce spam
	 *
	printf( "Leaf set: " );
	slist_map( symbol_leaf_set, (SListMapFn) dump_tiny, NULL );
	printf( "\n" );
	 */
}
#endif /*DEBUG*/

/* Strip a symbol down, ready for redefinition. 
 */
void *
symbol_strip( Symbol *sym )
{
#ifdef DEBUG_MAKE
	printf( "symbol_strip: " );
	symbol_name_print( sym );
	printf( "\n" );
#endif /*DEBUG_MAKE*/

	/* Anything that refers to us will need a recomp.
	 */
	if( is_top( sym ) ) 
		symbol_dirty_intrans( sym, link_serial_new() );

	/* Clean out old exprinfo.
	 */
	icontainer_map( ICONTAINER( sym ),
		(icontainer_map_fn) expr_strip, NULL, NULL );

	/* Free any top-links we made.
	 */
	(void) slist_map( sym->topchildren, (SListMapFn) link_destroy, NULL );

	/* Can free the patch list. We should not have to resolve off this
	 * name again.
	 */
	IM_FREEF( g_slist_free, sym->patch );

	/* Workspacegroup? Unlink from wsg.
	 */
	if( sym->wsg ) {
		sym->wsg->sym = NULL;
		sym->wsg = NULL;
	}

	/* Workspace? Unlink from ws.
	 */
	if( sym->ws ) {
		sym->ws->sym = NULL;
		sym->ws = NULL;
	}

	/* It's a ZOMBIE now.
	 */
	sym->type = SYM_ZOMBIE;

#ifdef DEBUG
	symbol_sanity( sym );
#endif /*DEBUG*/

	return( NULL );
}

static void *
symbol_made_error_clear( Link *link )
{
	expr_error_clear( link->parent->expr );

	return( NULL );
}

/* Finish creating a symbol. Sequence is: symbol_new(), specialise ZOMBIE
 * into a particular symbol type, symbol_made(). Do any final tidying up.
 */
void
symbol_made( Symbol *sym )
{
#ifdef DEBUG_MAKE
	printf( "symbol_made: " );
	symbol_name_print( sym );
	printf( "\n" );
#endif /*DEBUG_MAKE*/

	if( is_top( sym ) ) {
		/* Remake all top-level dependencies.
		 */
		(void) symbol_link_build( sym );

		/* Clear error on every symbol that refs us, then mark dirty.
		 * This lets us replace refed-to syms cleanly.
		 */
		slist_map( sym->topparents,
			(SListMapFn) symbol_made_error_clear, NULL );

		/* Real dirrrrty.
		 */
		if( sym->expr )
			expr_dirty( sym->expr, link_serial_new() );
	}

#ifdef DEBUG
	dump_symbol( sym );
#endif /*DEBUG*/
}

static void *
symbol_not_defined_sub( Link *link, BufInfo *buf )
{
	symbol_qualified_name( link->parent, buf );

	if( link->parent->tool ) 
		tool_error( link->parent->tool, buf );

	return( NULL );
}

/* Make a "not defined" error message. Can be called before symbol is removed,
 * so don't assume it's a ZOMBIE.
 */
void
symbol_not_defined( Symbol *sym )
{
	char txt[256];
	BufInfo buf;

	error_top( _( "Not found." ) );
	buf_init_static( &buf, txt, 256 );
	buf_appendf( &buf, _( "Symbol %s is not defined." ), 
		symbol_name( sym ) );
	buf_appends( &buf, "\n" );
	if( sym->topparents ) {
		buf_appendf( &buf, _( "%s is referred to by" ),
			symbol_name( sym ) );
		buf_appends( &buf, ": " );
		slist_map2( sym->topparents,
			(SListMap2Fn) symbol_not_defined_sub, &buf, NULL );
		buf_appends( &buf, "\n" );
	}
	error_sub( buf_all( &buf ) );
}

/* Compile refers to sym, which is going ... mark compile as containing an 
 * error.
 */
static void *
symbol_destroy_error( Compile *compile, Symbol *sym )
{
	symbol_not_defined( sym );
	compile_error_set( compile );

	return( NULL );
}

static void 
symbol_dispose( GObject *gobject )
{
	Symbol *sym;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_SYMBOL( gobject ) );

	sym = SYMBOL( gobject );

#ifdef DEBUG_MAKE
	printf( "symbol_dispose: " );
	symbol_name_print( sym );
	printf( " (%p)\n", sym );
#endif /*DEBUG_MAKE*/

	/* Note the impending destruction of this symbol. It's a state
	 * change, take off the leaf set if it's there.
	 */
	symbol_state_change( sym );

	/* Clear state.
	 */
	if( is_top( sym ) ) {
		/* All stuff that depends on this sym is now dirty.
		 */
		symbol_dirty_intrans( sym, link_serial_new() );
		symbol_dirty_clear( sym );
	}

	/* Strip it down.
	 */
	(void) symbol_strip( sym );
	IDESTROY( sym->tool );

	/* Any exprs which refer to us must have errors.
	 */
	(void) slist_map( sym->parents, 
		(SListMapFn) symbol_destroy_error, sym );

	/* Remove links from any expr which refer to us.
	 */
	(void) slist_map( sym->parents, (SListMapFn) compile_link_break, sym );

	/* No other syms should have toplinks to us.
	 */
	(void) slist_map( sym->topparents, (SListMapFn) link_destroy, NULL );

	/* Unregister value with GC.
	 */
	reduce_unregister( sym );

	/* Free other stuff. 
	 */
	sym->type = SYM_ZOMBIE; 

	assert( !sym->tool );
	assert( !sym->parents );
	assert( !sym->topparents );
	assert( !sym->topchildren );

	IM_FREEF( g_slist_free, sym->patch );
	IM_FREEF( g_slist_free, sym->parents );

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
symbol_changed( iObject *iobject )
{
	Symbol *sym = SYMBOL( iobject );

	/* If we have a tool, signal changed on that as well.
	 */
	if( sym->tool )
		iobject_changed( IOBJECT( sym->tool ) );

	IOBJECT_CLASS( parent_class )->changed( iobject );
}

static void
symbol_real_new_value( Symbol *symbol )
{
}

static void
symbol_class_init( SymbolClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = (iObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->dispose = symbol_dispose;

	iobject_class->changed = symbol_changed;

	symbol_signals[SIG_NEW_VALUE] = g_signal_new( "new_value",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( SymbolClass, new_value ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );

	class->new_value = symbol_real_new_value;
}

static void
symbol_init( Symbol *sym )
{
	symbol_clear( sym );

#ifdef DEBUG_MAKE
	printf( "symbol_init: (%p)\n", sym );
#endif /*DEBUG_MAKE*/
}

GtkType
symbol_get_type( void )
{
	static GtkType symbol_type = 0;

	if( !symbol_type ) {
		static const GTypeInfo info = {
			sizeof( SymbolClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) symbol_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Symbol ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) symbol_init,
		};

		symbol_type = g_type_register_static( TYPE_FILEMODEL, 
			"Symbol", &info, 0 );
	}

	return( symbol_type );
}

/* Make a new symbol on an expr. If it's already there and a ZOMBIE, just 
 * return it. If it's not a ZOMBIE, turn it into one. Otherwise make and 
 * link on a new symbol.
 */
Symbol *
symbol_new( Compile *compile, const char *name )
{
	Symbol *sym;

	if( (sym = compile_lookup( compile, name )) ) {
		if( sym->type != SYM_ZOMBIE ) 
			/* Already exists: strip it down.
			 */
			(void) symbol_strip( sym );

#ifdef DEBUG_MAKE
		printf( "symbol_new: redefining " );
		symbol_name_print( sym );
		printf( " (%p)\n", sym );
#endif /*DEBUG_MAKE*/
	}
	else {
		sym = SYMBOL( g_object_new( TYPE_SYMBOL, NULL ) );
		iobject_set( IOBJECT( sym ), name, NULL );
		icontainer_child_add( ICONTAINER( compile ), 
			ICONTAINER( sym ), -1 );

#ifdef DEBUG_MAKE
		printf( "symbol_new: creating " );
		symbol_name_print( sym );
		printf( " (%p)\n", sym );
#endif /*DEBUG_MAKE*/
	}

	return( sym );
}

void
symbol_error_redefine( Symbol *sym )
{
	static BufInfo buf;
	static char txt[200];

	buf_init_static( &buf, txt, 200 );
	buf_appendf( &buf, _( "Redefinition of \"%s\"." ), 
		IOBJECT( sym )->name );
	if( sym->tool && sym->tool->lineno != -1 ) {
		buf_appendf( &buf, "\n" );
		buf_appendf( &buf, _( "Previously defined at line %d." ),
			sym->tool->lineno );
	}

	yyerror( "%s", buf_all( &buf ) );
}

/* Name in defining occurence. If this is a top-level definition, clean the
 * old symbol and get ready to attach a user function to it. If its not a top-
 * level definition, we flag an error. Consider repeated parameter names, 
 * repeated occurence of names in locals, local name clashes with parameter
 * name etc.
 * We make a ZOMBIE: our caller should turn it into a blank user definition, a
 * parameter etc.
 */
Symbol *
symbol_new_defining( Compile *compile, const char *name )
{
	Symbol *sym;

	/* Block definition of "root" anywhere ... too confusing.
	 */
	if( strcmp( name, IOBJECT( symbol_root )->name ) == 0 )
		yyerror( _( "Attempt to redefine root symbol \"%s\"." ), name );

	/* Is this a redefinition of an existing symbol?
	 */
	if( (sym = compile_lookup( compile, name )) ) {
		/* Yes. Check that this redefinition is legal.
		 */
		switch( sym->type ) {
		case SYM_VALUE:
			/* Redef of existing symbol? Only allowed at top
			 * level.
			 */
			if( !is_scope( compile->sym ) )
				symbol_error_redefine( sym );
			break;

		case SYM_ZOMBIE:
			/* This is the definition for a previously referenced
			 * symbol. Just return the ZOMBIE we made.
			 */
			break;

		default:
			/* Parameter, workspace, etc.
			 */
			yyerror( _( "Can't redefine %s \"%s\"." ), 
				decode_SymbolType_user( sym->type ), name );
			/*NOTREACHED*/
		}

		/* This is the defining occurence ... move to the end of the
		 * traverse order.
		 */
		icontainer_child_move( ICONTAINER( sym ), -1 );
	}

	/* Get it ready.
	 */
	sym = symbol_new( compile, name );

	return( sym );
}

/* Make a reference to a symbol. Look on the local table for the name - if
 * it's not there, make a ZOMBIE. Note that ZOMBIEs etc. need patch lists
 * attached to them for all pointers to them we make. Responsibility of
 * caller!
 */
Symbol *
symbol_new_reference( Compile *compile, const char *name )
{
	Symbol *sym = compile_lookup( compile, name );

	if( !sym )
		sym = symbol_new( compile, name );

	/* Note the new dependency.
	 */
	compile_link_make( compile, sym );

	return( sym );
}

/* Compile refers to child ... break link.
 */
void *
symbol_link_break( Symbol *child, Compile *compile )
{
	compile_link_break( compile, child );

	return( NULL );
}

/* Specialise into a user definition.
 */
gboolean
symbol_user_init( Symbol *sym )
{
	assert( sym->type == SYM_ZOMBIE );

	sym->type = SYM_VALUE;
	reduce_register( sym );
	if( !sym->expr ) 
		sym->expr = expr_new( sym );

	/* We don't symbol_made() yet, wait until we have finished building
	 * sym->expr.
	 */

	return( TRUE );
}

/* Specialise into a parameter on an expression.
 */
gboolean
symbol_parameter_init( Symbol *sym )
{
	Compile *parent = COMPILE( ICONTAINER( sym )->parent );

	assert( sym->type == SYM_ZOMBIE );

	sym->type = SYM_PARAM;
	parent->param = g_slist_append( parent->param, sym );
	parent->nparam = g_slist_length( parent->param );
	symbol_made( sym );

	return( TRUE );
}

/* Specialise into a builtin parameter (eg. "this").
 */
gboolean
symbol_parameter_builtin_init( Symbol *sym )
{
	assert( sym->type == SYM_ZOMBIE );

	sym->type = SYM_PARAM;
	symbol_made( sym );

	return( TRUE );
}

/* Get the next dirty leaf symbol.
 */
static Symbol *
symbol_leaf_next( void ) 
{
	if( symbol_leaf_set )
		return( (Symbol *) symbol_leaf_set->data );
	else
		return( NULL ); 
}

/* Are there symbols we can recalculate? Used to display "Calculating ..."
 * status.
 */
gboolean 
symbol_busy( void )
{
	return( symbol_leaf_set != NULL );
}

/* Set leaf state.
 */
static void
symbol_set_leaf( Symbol *sym, gboolean leaf )
{
	if( sym->leaf != leaf ) {
		gboolean changed;

		sym->leaf = leaf;

		changed = FALSE;
		if( leaf ) {
			if( !symbol_leaf_set )
				changed = TRUE;

			symbol_leaf_set = 
				g_slist_prepend( symbol_leaf_set, sym );
		}
		else {
			assert( symbol_leaf_set );

			symbol_leaf_set = 
				g_slist_remove( symbol_leaf_set, sym );

			if( !symbol_leaf_set )
				changed = TRUE;
		}

		if( changed )
			iobject_changed( IOBJECT( reduce_context->heap ) );

		if( sym->expr && sym->expr->row )
			iobject_changed( IOBJECT( sym->expr->row ) );
	}
}

/* State of a symbol has changed ... update! 
 */
void
symbol_state_change( Symbol *sym )
{
	assert( sym->ndirtychildren >= 0 );

	/* Used to do more ... now we just set leaf.
	 */
	symbol_set_leaf( sym, symbol_is_leafable( sym ) );
}

/* Recalculate a symbol. We know we are dirty and have no dirty ancestors.
 */
static gboolean
symbol_recalculate_sub( Symbol *sym )
{
	gboolean result = TRUE;

#ifdef DEBUG_TIME
	static GTimer *timer = NULL;

	if( !timer )
		timer = g_timer_new();
	g_timer_reset( timer );
#endif /*DEBUG_TIME*/

	if( is_value( sym ) ) {
		if( sym->expr->row ) {
			/* This is the root of a display ... use that recomp
			 * mechanism.
			 */
			row_recomp( sym->expr->row );
			if( sym->expr->row->err )
				result = FALSE;
		}
		else if( sym->expr->compile->nparam > 0 ) {
			/* This is a function ... just mark it clean and wait
			 * until link time before we try to copy the code into
			 * the heap.
			 */
			symbol_dirty_clear( sym );
			expr_new_value( sym->expr );
		}
		else {
			if( reduce_regenerate( sym->expr, &sym->expr->root ) ) {
				symbol_dirty_clear( sym );
				expr_new_value( sym->expr );
			}
			else
				result = FALSE;

		}
	}
	else
		symbol_dirty_clear( sym );

#ifdef DEBUG_TIME
	printf( "symbol_recalculate_sub: " );
	symbol_name_scope_print( sym );
	printf( " %g\n", g_timer_elapsed( timer, NULL ) );
#endif /*DEBUG_TIME*/

	return( result );
}

/* We can get called recursively .. eg. we do an im_tiff2vips(), that
 * pops a progress box, that triggers idle, that tries to recalc a
 * leaf again.
 */
static gboolean symbol_running = FALSE;

/* Recalc a symbol ... with error checks.
 */
void *
symbol_recalculate_check( Symbol *sym )
{
#ifdef DEBUG
	printf( "symbol_recalculate_check: %s\n", symbol_name( sym ) );
#endif /*DEBUG*/

	error_clear();
	if( sym->expr->err ) {
		expr_error_get( sym->expr );
		return( sym );
	}
	if( !sym->dirty ) 
		return( NULL );
	if( !is_value( sym ) ) {
		symbol_dirty_clear( sym );
		return( NULL );
	}
	if( symbol_running )
		return( NULL );

	reduce_context->heap->filled = FALSE;
	symbol_running = TRUE;
	if( !symbol_recalculate_sub( sym ) || 
		reduce_context->heap->filled ) {
		expr_error_set( sym->expr );
		symbol_running = FALSE;
		return( sym );
	}
	symbol_running = FALSE;

	return( NULL );
}

/* Recalculate a symbol. FALSE if no symbols can be recalced.
 */
static gboolean
symbol_recalculate_leaf( void )
{
	gboolean recalculated;
	Symbol *sym;

	recalculated = FALSE;

#ifdef DEBUG
	printf( "symbol_recalculate_leaves: Leaf set: " );
	slist_map( symbol_leaf_set, (SListMapFn) dump_tiny, NULL );
	printf( "\n" );
#endif /*DEBUG*/

	/* Grab stuff off the leaf set.
	 */
	if( (sym = symbol_leaf_next()) ) {
		/* Should be dirty with no dirty children. Unless it's a
		 * function, in which case dirty kids are OK.
		 */
		assert( sym->dirty );
		assert( !sym->expr->err );
		assert( is_top( sym ) );
		assert( symbol_ndirty( sym ) == 0 || is_value( sym ) );

		/* Found a symbol!
		 */
		(void) symbol_recalculate_check( sym );

		/* Note a pending GC.
		 */
		(void) heap_gc_request( reduce_context->heap );

		/* We have recalculated a symbol.
		 */
		recalculated = TRUE;
	}

	return( recalculated );
}

/* Our idle recomp callback. 
 */
static gint symbol_idle_id = 0;

static gboolean
symbol_recalculate_idle_cb( void )
{
	gboolean run_again;

	run_again = TRUE;

	if( !symbol_running ) {
		set_hourglass();

		if( !symbol_recalculate_leaf() ) {
			/* Nothing more to do: shut down idle recomp.
			 */
			symbol_idle_id = 0;

			run_again = FALSE;
		}

		set_pointer();
	}

	return( run_again );
}

/* Recalculate ... either nudge the idle recomp, or in batch mode, do a recomp
 * right now.
 */
void
symbol_recalculate_all_force( gboolean now )
{
#ifdef DEBUG
	icontainer_map( ICONTAINER( symbol_root->expr->compile ), 
		(icontainer_map_fn) symbol_sanity, NULL, NULL );
#endif /*DEBUG*/

	/* In case we're called directly.
	 */
	(void) view_scan_all();

	if( symbol_running )
		/* Do nothing.
		 */
		;
	else if( main_option_batch || now ) {
		set_hourglass();

		while( symbol_recalculate_leaf() )
			;

		set_pointer();
	}
	else if( !symbol_idle_id ) 
		symbol_idle_id = g_idle_add( 
			(GSourceFunc) symbol_recalculate_idle_cb, NULL );
}

/* Recalculate the symbol table.
 */
void
symbol_recalculate_all( void )
{
	/* Do a scan, even if we don't recomp. We need to pick up edits before
	 * views get refreshed.
	 */
	(void) view_scan_all();

	if( mainw_auto_recalc )
		symbol_recalculate_all_force( FALSE );
}
