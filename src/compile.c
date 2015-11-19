/* Stuff to parse and compile text.
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
#define DEBUG_RESOLVE
 */

/* regular (and very slow) sanity checks on symbols ... needs DEBUG in
 * symbol.c as well
#define DEBUG_SANITY
 */

/* count how many nodes we find with common sub-expression removal.
#define DEBUG_COMMON
 */

/* show what everything compiled to
#define DEBUG_RESULT
 */

/* trace list comp compile
#define DEBUG_LCOMP
 */

/* trace pattern LHS generation
#define DEBUG_PATTERN
 */

/* 
#define DEBUG
 */

#include "ip.h"

static iContainerClass *parent_class = NULL;

Compile *
compile_get_parent( Compile *compile )
{
	if( !ICONTAINER( compile->sym )->parent )
		return( NULL );

	return( COMPILE( ICONTAINER( compile->sym )->parent ) );
}

void *
compile_name_print( Compile *compile )
{
	printf( "compile(%p) ", compile );
	symbol_name_print( compile->sym );

	return( NULL );
}

static void *
compile_name_sub( Expr *expr, VipsBuf *buf )
{
	if( expr->row ) {
		if( !vips_buf_is_empty( buf ) )
			vips_buf_appends( buf, ", " );
		row_qualified_name( expr->row, buf );
	}

	return( NULL );
}

void
compile_name( Compile *compile, VipsBuf *buf )
{
	char txt[256];
	VipsBuf buf2 = VIPS_BUF_STATIC( txt );

	vips_buf_appends( buf, "\"" );
	symbol_qualified_name( compile->sym, buf );
	vips_buf_appends( buf, "\"" );

	slist_map( compile->exprs,
		(SListMapFn) compile_name_sub, &buf2 );
	if( !vips_buf_is_empty( &buf2 ) ) 
		vips_buf_appendf( buf, " (%s)", vips_buf_all( &buf2 ) );
}

static Compile *
compile_map_all_sub( Symbol *sym, map_compile_fn fn, void *a )
{
	if( !sym->expr || !sym->expr->compile )
		return( NULL );
	else 
		return( compile_map_all( sym->expr->compile, fn, a ) );
}

/* Apply a function to a compile ... and any local compiles. Do top-down.
 */
Compile *
compile_map_all( Compile *compile, map_compile_fn fn, void *a )
{
	Compile *res;

	/* Us first.
	 */
	if( (res = fn( compile, a )) )
		return( res );

	/* Then any children.
	 */
	if( (res = (Compile *) icontainer_map( ICONTAINER( compile ),
		(icontainer_map_fn) compile_map_all_sub, (void *) fn, a )) )
		return( res );

	return( NULL );
}

/* Look up by name.
 */
Symbol *
compile_lookup( Compile *compile, const char *name )
{
	return( SYMBOL( 
		icontainer_child_lookup( ICONTAINER( compile ), name ) ) );
}

/* Make a dependency. Text in compile refers to sym.
 */
void
compile_link_make( Compile *compile, Symbol *child )
{
	/* Already a dependency? Don't make a second link.
	 */
	if( !g_slist_find( compile->children, child ) ) {
		/* New link, each direction.
		 */
		compile->children = g_slist_prepend( compile->children, child );
		child->parents = g_slist_prepend( child->parents, compile );

		/* If the child is a forward reference, we may have to patch 
		 * this later. Save the pointer-to-child pointer on child.
		 */
		if( child->type == SYM_ZOMBIE )
			(void) symbol_patch_add( 
				&compile->children->data, child );
	}

#ifdef DEBUG_SANITY
	/* Sanity check.
	 */
	symbol_sanity( child );
	symbol_leaf_set_sanity();
#endif /*DEBUG_SANITY*/
}

/* Break a dependency. Text in compile referred to child.
 */
void *
compile_link_break( Compile *compile, Symbol *child )
{
	/* Sanity check.
	 */
#ifdef DEBUG_SANITY
	symbol_sanity( child );
	symbol_leaf_set_sanity();
#endif /*DEBUG_SANITY*/

	/* Must be there.
	 */
	g_assert( g_slist_find( compile->children, child ) &&
		g_slist_find( child->parents, compile ) );

	compile->children = g_slist_remove( compile->children, child );
	child->parents = g_slist_remove( child->parents, compile );

	/* Sanity check.
	 */
#ifdef DEBUG_SANITY
	symbol_sanity( child );
	symbol_leaf_set_sanity();
#endif /*DEBUG_SANITY*/

	return( NULL );
}

void *
compile_expr_link_break( Compile *compile, Expr *expr )
{
	g_assert( expr->compile == compile );
	g_assert( g_slist_find( compile->exprs, expr ) );

	expr->compile = NULL;
	compile->exprs = g_slist_remove( compile->exprs, expr );

	g_object_unref( G_OBJECT( compile ) );

	return( NULL );
}

void *
compile_expr_link_break_rev( Expr *expr, Compile *compile )
{
	return( compile_expr_link_break( compile, expr ) );
}

void 
compile_expr_link_make( Compile *compile, Expr *expr )
{
	g_assert( !expr->compile );
	g_assert( !g_slist_find( compile->exprs, expr ) );
	g_assert( compile->sym == expr->sym );

	expr->compile = compile;
	compile->exprs = g_slist_prepend( compile->exprs, expr );

        g_object_ref( G_OBJECT( compile ) );
	iobject_sink( IOBJECT( compile ) );
}

static void
compile_finalize( GObject *gobject )
{
	Compile *compile;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_COMPILE( gobject ) );

	compile = COMPILE( gobject );

#ifdef DEBUG
	printf( "compile_finalize: " );
	compile_name_print( compile );
	printf( "\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */

	/* Junk parse tree.
	 */
	slist_map( compile->treefrag, (SListMapFn) tree_node_destroy, NULL );
	IM_FREEF( g_slist_free, compile->treefrag );
	compile->tree = NULL;

	/* Break links to all locals.
	 */
	IM_FREEF( g_slist_free, compile->param );
	compile->nparam = 0;
	IM_FREEF( g_slist_free, compile->secret );
	compile->nsecret = 0;
	compile->this = NULL;
	compile->super = NULL;
	(void) slist_map( compile->children, 
		(SListMapFn) symbol_link_break, compile );
	IM_FREEF( g_slist_free, compile->children );

	/* Remove static strings we created.
	 */
	slist_map( compile->statics, 
		(SListMapFn) managed_destroy_nonheap, NULL );
	IM_FREEF( g_slist_free, compile->statics );

	/* Junk heap.
	 */
	if( compile->heap ) {
		compile->base.type = ELEMENT_NOVAL;
		compile->base.ele = (void *) 1;
		heap_unregister_element( compile->heap, &compile->base ); 
		UNREF( compile->heap );
	}

	/* Junk text.
	 */
	IM_FREE( compile->text );
	IM_FREE( compile->prhstext );
	IM_FREE( compile->rhstext );

	compile->sym = NULL;

	/* If we're being finalized, we must have a ref count of zero, so
	 * there shouldn't be any exprs looking at us.
	 */
	g_assert( !compile->exprs );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
compile_class_init( CompileClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = compile_finalize;

	/* Create signals.
	 */

	/* Init default methods.
	 */
}

static void
compile_init( Compile *compile )
{
	/* Init our instance fields.
	 */
	compile->sym = NULL;

	compile->exprs = NULL;

	compile->is_klass = FALSE;
	compile->has_super = FALSE;

	compile->text = NULL;
	compile->prhstext = NULL;
	compile->rhstext = NULL;

	compile->tree = NULL;
	compile->treefrag = NULL;
	compile->last_sym = NULL;

	compile->nparam = 0;
	compile->param = NULL;
	compile->nsecret = 0;
	compile->secret = NULL;
	compile->this = NULL;
	compile->super = NULL;
	compile->children = NULL;

	compile->base.type = ELEMENT_NOVAL;
	compile->heap = NULL;
	compile->statics = NULL;
}

GType
compile_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( CompileClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) compile_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Compile ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) compile_init,
		};

		type = g_type_register_static( TYPE_ICONTAINER, 
			"Compile", &info, 0 );
	}

	return( type );
}

/* Make a compile linked to an expr.
 */
Compile *
compile_new( Expr *expr )
{
	Compile *compile = COMPILE( g_object_new( TYPE_COMPILE, NULL ) );

	compile->sym = expr->sym;

	/* Junk any old compile.
	 */
	if( expr->compile )
		compile_expr_link_break( expr->compile, expr );

	compile_expr_link_make( compile, expr );

	/* We'll want to be able to do name lookups.
	 */
	icontainer_set_hash( ICONTAINER( compile ) );

#ifdef DEBUG
	printf( "compile_new: " );
	compile_name_print( compile );
	printf( "\n" );
#endif /*DEBUG*/

	return( compile );
}

/* Max cells function for symbols. Enough to compile something big.
 */
static int
compile_heap_max_fn( Heap *heap )
{
	return( 10000 );
}

/* Make a exprinfo suitable for a top-level symbol.
 */
Compile *
compile_new_toplevel( Expr *expr )
{
	Compile *compile = compile_new( expr );

	compile->heap = heap_new( compile, compile_heap_max_fn, 100, 1000 );
	g_object_ref( G_OBJECT( compile->heap ) );
	iobject_sink( IOBJECT( compile->heap ) );

	heap_register_element( compile->heap, &compile->base ); 

	return( compile );
}

/* Make a exprinfo suitable for a local.
 */
Compile *
compile_new_local( Expr *expr )
{
	Compile *compile = compile_new( expr );

	compile->heap = heap_new( compile, compile_heap_max_fn, 100, 100 );
	g_object_ref( G_OBJECT( compile->heap ) );
	iobject_sink( IOBJECT( compile->heap ) );

	heap_register_element( compile->heap, &compile->base ); 

	return( compile );
}

/* Code generation.
 */

/* Generate a binop. Point arg1 and arg2 at the elements to be filled in:
 * caller sets them later. First arg is the compile that this operator came 
 * from. 
 */
static gboolean
compile_binop( Compile *compile, 
	BinOp bop, PElement *arg1, PElement *arg2, PElement *out )
{
	Heap *heap = compile->heap;

	HeapNode *hn1, *hn2, *hn3;
	PElement e1, e2;

	if( NEWNODE( heap, hn1 ) )
		return( FALSE );
	hn1->type = TAG_APPL;
	PPUT( hn1, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
	PEPUTP( out, ELEMENT_NODE, hn1 );
	PEPOINTLEFT( hn1, &e1 );
	PEPOINTRIGHT( hn1, arg2 );

	if( NEWNODE( heap, hn2 ) )
		return( FALSE );
	hn2->type = TAG_APPL;
	PPUT( hn2, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
	PEPUTP( &e1, ELEMENT_NODE, hn2 );
	PEPOINTRIGHT( hn2, arg1 );
	PEPOINTLEFT( hn2, &e2 );

	if( NEWNODE( heap, hn3 ) )
		return( FALSE );
	hn3->type = TAG_APPL;
	PPUT( hn3, ELEMENT_BINOP, bop, ELEMENT_COMPILEREF, compile );
	PEPUTP( &e2, ELEMENT_NODE, hn3 );

	return( TRUE );
}

/* Generate "x.sym". Set x to be NULL and point rhs at it .. caller
 * fills in later.
 */
static gboolean
compile_dotsym( Compile *compile, Symbol *sym, PElement *rhs, PElement *out )
{
	PElement e;

	if( !compile_binop( compile, BI_DOT, rhs, &e, out ) )
		return( FALSE );
	PEPUTP( &e, ELEMENT_SYMREF, sym );

	return( TRUE );
}

/* Compile a reference to sym from expr.
 */
static gboolean
compile_reference( Compile *compile, Symbol *sym, PElement *out )
{
	Heap *heap = compile->heap;
	Compile *parent = compile_get_parent( compile );

#ifdef DEBUG
	printf( "generate_reference: ref to " );
	symbol_name_print( sym );
	printf( "inside " );
	compile_name_print( compile );
	printf( "\n" );
#endif /*DEBUG*/

	if( g_slist_find( compile->param, sym ) || 
		g_slist_find( compile->secret, sym ) ) {
		/* sym is a simple parameter, easy!
		 */
		PEPUTP( out, ELEMENT_SYMBOL, sym );
	}
	else if( is_class( parent ) && 
		(symbol_get_parent( sym ) == parent->sym ||
		g_slist_find( parent->secret, sym )) ) {
		Symbol *ths = parent->this;

		/* sym is a member of the same class as expr, or sym is a
		 * secret to our constructor (in which case it'll be in this
		 * as well) ... generate (.sym this)
		 *
		 * Optimisation: don't generate (.this this)
		 */
		if( sym == ths ) {
			PEPUTP( out, ELEMENT_SYMBOL, ths );
		}
		else {
			PElement rhs;

			if( !compile_dotsym( compile, sym, &rhs, out ) )
				return( FALSE );
			PEPUTP( &rhs, ELEMENT_SYMBOL, ths );
		}
	}
	else if( is_member_enclosing( compile, sym ) ) {
		Symbol *sths = symbol_get_parent( sym )->expr->compile->this;
		PElement rhs;

		/* Sym is a member of an enclosing class ...
		 * generate (.sym ref-to-this-for-that-class)
		 */
		if( !compile_dotsym( compile, sym, &rhs, out ) ||
			!compile_reference( compile, sths, &rhs ) )
			return( FALSE );
	}
	else {
		/* some other reference ... generate (sym secret1 .. secretn)
		 * recurse for secrets, since we may have to fetch them from 
		 * "this"
		 */
		PElement e = *out;
		PElement f;
		GSList *l;

		PEPUTP( &e, ELEMENT_SYMBOL, sym );

		/* Build secret args to this sym.
		 */
		if( sym->expr && sym->expr->compile )
			for( l = sym->expr->compile->secret; l; l = l->next ) {
				Symbol *arg = SYMBOL( l->data );
				HeapNode *hn1;

				if( NEWNODE( heap, hn1 ) )
					return( FALSE );
				hn1->type = TAG_APPL;
				PEPUTLEFT( hn1, &e );
				PPUTRIGHT( hn1, ELEMENT_ELIST, NULL ); 
				PEPUTP( &e, ELEMENT_NODE, hn1 );

				PEPOINTRIGHT( hn1, &f );
				if( !compile_reference( compile, arg, &f ) )
					return( FALSE );
			}
	}

	return( TRUE );
}

/* Build a graph with vars still in it. Write result to *out.
 */
static gboolean
compile_graph( Compile *compile, ParseNode *pn, PElement *out )
{
	Heap *heap = compile->heap;
	HeapNode *hn1, *hn2, *hn3;
	PElement e1, e2, e3;
	GSList *l;

	switch( pn->type ) {
	case NODE_APPLY:
		/* Build apply node.
		 */
		if( NEWNODE( heap, hn1 ) )
			return( FALSE );
		hn1->type = TAG_APPL;
		PPUT( hn1, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
		PEPUTP( out, ELEMENT_NODE, hn1 );

		/* Make sides.
		 */
		PEPOINTLEFT( hn1, &e1 );
		PEPOINTRIGHT( hn1, &e2 );
		if( !compile_graph( compile, pn->arg1, &e1 ) ||
			!compile_graph( compile, pn->arg2, &e2 ) )
			return( FALSE );

		break;

	case NODE_UOP:
		/* Build apply node.
		 */
		if( NEWNODE( heap, hn1 ) )
			return( FALSE );
		hn1->type = TAG_APPL;
		PPUT( hn1, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
		PEPUTP( out, ELEMENT_NODE, hn1 );
		PEPOINTLEFT( hn1, &e1 );

		if( NEWNODE( heap, hn2 ) )
			return( FALSE );
		hn2->type = TAG_APPL;
		PPUT( hn2, ELEMENT_UNOP, pn->uop, ELEMENT_COMPILEREF, compile );
		PEPUTP( &e1, ELEMENT_NODE, hn2 );

		/* Build arg.
		 */
		PEPOINTRIGHT( hn1, &e2 );
		if( !compile_graph( compile, pn->arg1, &e2 ) )
			return( FALSE );

		break;

	case NODE_BINOP:
		if( !compile_binop( compile, pn->biop, &e1, &e2, out ) ||
			!compile_graph( compile, pn->arg1, &e1 ) ||
			!compile_graph( compile, pn->arg2, &e2 ) )
			return( FALSE );

		break;

	case NODE_COMPOSE:
		if( NEWNODE( heap, hn1 ) )
			return( FALSE );
		hn1->type = TAG_APPL;
		PPUT( hn1, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
		PEPUTP( out, ELEMENT_NODE, hn1 );
		PEPOINTLEFT( hn1, &e1 );

		if( NEWNODE( heap, hn2 ) )
			return( FALSE );
		hn2->type = TAG_APPL;
		PPUT( hn2, ELEMENT_COMB, COMB_SR, 
			ELEMENT_ELIST, NULL );
		PEPUTP( &e1, ELEMENT_NODE, hn2 );

		/* Build args.
		 */
		PEPOINTRIGHT( hn1, &e2 );
		PEPOINTRIGHT( hn2, &e3 );
		if( !compile_graph( compile, pn->arg1, &e3 ) ||
			!compile_graph( compile, pn->arg2, &e2 ) )
			return( FALSE );

		break;

	case NODE_LEAF:
		/* A reference to a symbol. 
		 */
		if( !compile_reference( compile, pn->leaf, out ) )
			return( FALSE );

		break;

	case NODE_CLASS:
		/* Output constructor.
		 */
		PEPUTP( out, ELEMENT_CONSTRUCTOR, pn->klass );
		break;

	case NODE_TAG:
		/* RHS of projection. 
		 */
		PEPUTP( out, ELEMENT_TAG, pn->tag );
		break;

	case NODE_GENERATOR:
		/* Build apply nodes.
		 */
		if( NEWNODE( heap, hn1 ) )
			return( FALSE );
		hn1->type = TAG_APPL;
		PPUT( hn1, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
		PEPUTP( out, ELEMENT_NODE, hn1 );
		PEPOINTLEFT( hn1, &e1 );

		if( NEWNODE( heap, hn2 ) )
			return( FALSE );
		hn2->type = TAG_APPL;
		PPUT( hn2, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
		PEPUTP( &e1, ELEMENT_NODE, hn2 );
		PEPOINTLEFT( hn2, &e2 );

		if( NEWNODE( heap, hn3 ) )
			return( FALSE );
		hn3->type = TAG_APPL;
		PPUT( hn3, ELEMENT_COMB, COMB_GEN, ELEMENT_ELIST, NULL );
		PEPUTP( &e2, ELEMENT_NODE, hn3 );

		/* Build args.
		 */
		PEPOINTRIGHT( hn1, &e3 );
		PEPOINTRIGHT( hn2, &e2 );
		PEPOINTRIGHT( hn3, &e1 );
		if( !compile_graph( compile, pn->arg1, &e1 ) )
			return( FALSE );
		if( pn->arg2 )
			if( !compile_graph( compile, pn->arg2, &e2 ) )
				return( FALSE );
		if( pn->arg3 )
			if( !compile_graph( compile, pn->arg3, &e3 ) )
				return( FALSE );

		break;

	case NODE_LISTCONST:
	case NODE_SUPER:
		/* List of expressions.
		 */

		/* Make first RHS ... the end of the list. 
		 */
		e1 = *out;
		PEPUTP( &e1, ELEMENT_ELIST, NULL );

		/* Build @':' for each element.
		 */
		for( l = pn->elist; l; l = l->next ) {
			ParseNode *arg = (ParseNode *) l->data;

			/* Build apply nodes.
			 */
			if( NEWNODE( heap, hn1 ) )
				return( FALSE );
			hn1->type = TAG_APPL;
			PPUT( hn1, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
			PEPUTP( &e1, ELEMENT_NODE, hn1 );
			PEPOINTLEFT( hn1, &e2 );

			if( NEWNODE( heap, hn2 ) )
				return( FALSE );
			hn2->type = TAG_APPL;
			PPUT( hn2, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL );
			PEPUTP( &e2, ELEMENT_NODE, hn2 );
			PEPOINTLEFT( hn2, &e2 );

			if( NEWNODE( heap, hn3 ) )
				return( FALSE );
			hn3->type = TAG_APPL;
			PPUT( hn3, ELEMENT_BINOP, BI_CONS, 
				ELEMENT_COMPILEREF, compile );
			PEPUTP( &e2, ELEMENT_NODE, hn3 );

			/* Build arg.
			 */
			PEPOINTRIGHT( hn2, &e3 );
			if( !compile_graph( compile, arg, &e3 ) )
				return( FALSE );

			/* APPL is now our LHS.
			 */
			PEPOINTRIGHT( hn1, &e1 );
		}

		break;

	case NODE_CONST:
		/* Constant.
		 */
		switch( pn->con.type ) {
		case PARSE_CONST_STR:
		{
			Managedstring *managedstring;
				
			if( !(managedstring = managedstring_find( 
				reduce_context->heap, 
				pn->con.val.str )) )
				return( FALSE );
			MANAGED_REF( managedstring );
			compile->statics = g_slist_prepend( compile->statics,
				managedstring );
			PEPUTP( out, ELEMENT_MANAGED, managedstring );
		}
			break;

		case PARSE_CONST_CHAR:
			PEPUTP( out, ELEMENT_CHAR, pn->con.val.ch );
			break;

		case PARSE_CONST_BOOL:
			PEPUTP( out, ELEMENT_BOOL, pn->con.val.bool );
			break;

		case PARSE_CONST_ELIST:
			PEPUTP( out, ELEMENT_ELIST, NULL );
			break;

		case PARSE_CONST_NUM:
			if( !heap_real_new( heap, pn->con.val.num, out ) )
				return( FALSE );
			break;

		case PARSE_CONST_COMPLEX:
			if( !heap_complex_new( heap, 0, pn->con.val.num, out ) )
				return( FALSE );
			break;

		default:
			g_assert( FALSE );
		}

		break;

	case NODE_NONE:
	default:
		g_assert( FALSE );
	}

	return( TRUE );
}

/* Parameter abstraction.
 */

/* Abstract a symbol from the body of a piece of graph. Set *used if we found 
 * the symbol in this piece of graph ... ie. if our caller should add an
 * Sx-combinator for us.  Update *root with the new piece of graph.
 */
static int
compile_abstract_body( Compile *compile, 
	PElement *root, Symbol *sym, gboolean *used )
{
	Heap *heap = compile->heap;
	HeapNode *hn;
	HeapNode *hn1;
	PElement e1, e2;
	gboolean b1, b2;
	CombinatorType comb;

	switch( PEGETTYPE( root ) ) {
	case ELEMENT_NODE:
		hn = PEGETVAL( root );
		switch( hn->type ) {
		case TAG_APPL:		
		case TAG_CONS:
			b1 = FALSE; b2 = FALSE;
			PEPOINTLEFT( hn, &e1 );
			PEPOINTRIGHT( hn, &e2 );
			if( compile_abstract_body( compile, &e1, sym, &b1 ) ||
				compile_abstract_body( compile, 
					&e2, sym, &b2 ) )
				return( -1 );

			if( PEISCOMB( &e2 ) && 
				PEGETCOMB( &e2 ) == COMB_I && !b1 && b2 &&
				hn->type == TAG_APPL ) {
				PEPUTPE( root, &e1 );
				*used = TRUE;
			}
			else if( b1 || b2 ) {
				if( b1 && !b2 ) 
					comb = COMB_SL;
				else if( !b1 && b2 ) 
					comb = COMB_SR;
				else 
					comb = COMB_S;

				/* Generate Sx combinator.
				 */
				if( NEWNODE( heap, hn1 ) )
					return( -1 );
				hn1->type = TAG_APPL;
				PPUTLEFT( hn1, ELEMENT_COMB, comb );
				PEPUTRIGHT( hn1, &e1 );
				PEPUTP( &e1, ELEMENT_NODE, hn1 );

				/* We've used the var too!
				 */
				*used = TRUE;
			}

			break;

		case TAG_DOUBLE:
		case TAG_COMPLEX:
		case TAG_CLASS:
		case TAG_GEN:
			break;

		case TAG_FILE:
		case TAG_FREE:	
		default:
			g_assert( FALSE );
		}

		break;

	case ELEMENT_SYMBOL:
		if( SYMBOL( PEGETVAL( root ) ) == sym ) {
			/* Found an instance! Make an I combinator.
			 */
			*used = TRUE;
			PEPUTP( root, ELEMENT_COMB, COMB_I );
		}
		break;

	case ELEMENT_CONSTRUCTOR:
		/* set used .. to stop K being generated for this
		 * class parameter.
		 */
		*used = TRUE;
		break;

	case ELEMENT_MANAGED:
	case ELEMENT_CHAR:
	case ELEMENT_BOOL:
	case ELEMENT_BINOP:
	case ELEMENT_UNOP:
	case ELEMENT_COMB:
	case ELEMENT_ELIST:
	case ELEMENT_SYMREF:
	case ELEMENT_COMPILEREF:
	case ELEMENT_NOVAL:
	case ELEMENT_TAG:
		/* Leave alone.
		 */
		break;

	default:
		g_assert( FALSE );
	}

	return( 0 );
}

/* Abstract a symbol from a graph. As above, but make a K if the symbol is
 * entirely unused. 
 */
static void *
compile_abstract_symbol( Symbol *sym, Compile *compile, PElement *root )
{
	Heap *heap = compile->heap;
	gboolean b;

#ifdef DEBUG
	printf( "abstracting " );
	symbol_name_print( sym );
	printf( "\n" );
#endif /*DEBUG*/

	b = FALSE;
	if( compile_abstract_body( compile, root, sym, &b ) )
		return( sym );

	if( !b ) {
		HeapNode *hn1;

		/* Parameter not used! Need a K.
		 */
		if( NEWNODE( heap, hn1 ) )
			return( sym );
		hn1->type = TAG_APPL;
		PPUTLEFT( hn1, ELEMENT_COMB, COMB_K );
		PEPUTRIGHT( hn1, root );

		/* Update root.
		 */
		PEPUTP( root, ELEMENT_NODE, hn1 );
	}

	return( NULL );
}

/* Common sub-expression elimination.
 */

#ifdef DEBUG_COMMON
static void *
compile_node_count_sub( HeapNode *hn, int *n )
{
	*n += 1;

	return( NULL );
}

static int
compile_node_count( HeapNode *hn )
{
	int n;

	n = 0;
	heap_map( hn, (heap_map_fn) compile_node_count_sub, &n, NULL );

	return( n );
}

/* Accumulate total saved here during walk of this tree.
 */
static int compile_node_sum;
#endif /*DEBUG_COMMON*/

/* A hash code we calculate from a bit of heap.
 */
typedef gpointer CompileHash;

/* Combine two hashes.
 */
#define COMPILEHASH_ADD( A, B ) \
	GUINT_TO_POINTER( GPOINTER_TO_UINT( A ) +  GPOINTER_TO_UINT( B ) )

/* An int to a hash.
 */
#define INT_TO_HASH GUINT_TO_POINTER

/* Build one of these during sharing analysis. From node pointers to
 * hash codes, and from hash codes to a list of matching node pointers.
 */
typedef struct _CompileShare {
	Compile *compile;

	GHashTable *node2hash;
	GHashTable *hash2nodel;
} CompileShare;

static gboolean
compile_share_destroy_sub( gpointer key, gpointer value, gpointer user_data )
{
	if( value )
		g_slist_free( (GSList *) value );

	return( TRUE );
}

static void
compile_share_destroy( CompileShare *share )
{
	share->compile = NULL;
	if( share->node2hash ) {
		g_hash_table_destroy( share->node2hash );
		share->node2hash = NULL;
	}
	if( share->hash2nodel ) {
		g_hash_table_foreach_remove( share->hash2nodel,
			compile_share_destroy_sub, NULL );
		g_hash_table_destroy( share->hash2nodel );
		share->hash2nodel = NULL;
	}
}

static void
compile_share_init( CompileShare *share, Compile *compile )
{
	share->compile = compile;
	share->node2hash = g_hash_table_new( NULL, g_direct_equal );
	share->hash2nodel = g_hash_table_new( NULL, g_direct_equal );
}

/* Remove a heapnode from the share.
 */
static void *
compile_share_remove( HeapNode *hn, CompileShare *share )
{
	CompileHash hash;

	if( (hash = g_hash_table_lookup( share->node2hash, hn )) ) {
		GSList *nodel;

		if( (nodel = g_hash_table_lookup( share->hash2nodel, 
			hash )) ) {
			nodel = slist_remove_all( nodel, hn );
			g_hash_table_replace( share->hash2nodel, 
				hash, nodel );
		}

		g_hash_table_remove( share->node2hash, hn );
	}

	return( NULL );
}

/* Add a new heapnode.
 */
static void
compile_share_add( CompileShare *share, HeapNode *hn, CompileHash hash )
{
	/* Make sure hash is non-zero (very unlikely).
	 */
	if( !hash )
		hash = INT_TO_HASH( 1 );

	if( !g_hash_table_lookup( share->node2hash, hn ) ) {
		GSList *nodel;

		g_hash_table_insert( share->node2hash, hn, hash ); 

		if( (nodel = g_hash_table_lookup( share->hash2nodel, hash )) ) {
			nodel = g_slist_prepend( nodel, hn );
			g_hash_table_replace( share->hash2nodel, hash, nodel );
		}
		else {
			nodel = g_slist_prepend( NULL, hn );
			g_hash_table_insert( share->hash2nodel, hash, nodel );
		}
	}
}

/* From a HeapNode, find a list of the other heapnodes which hashed to the same
 * value.
 */
static GSList *
compile_share_lookup( CompileShare *share, HeapNode *hn )
{
	CompileHash hash;

	if( (hash = (CompileHash) 
		g_hash_table_lookup( share->node2hash, hn )) ) 
		return( g_hash_table_lookup( share->hash2nodel, 
			(gpointer) hash ) );

	return( NULL );
}

static CompileHash compile_share_scan_node( CompileShare *share, 
	HeapNode *hn );

static CompileHash
compile_share_scan_element( CompileShare *share, PElement *e )
{
	CompileHash hash;

	switch( PEGETTYPE( e ) ) {
	case ELEMENT_NODE:
		hash = compile_share_scan_node( share, PEGETVAL( e ) );
		break;

	case ELEMENT_SYMBOL:
	case ELEMENT_SYMREF:
	case ELEMENT_COMPILEREF:
	case ELEMENT_CHAR:
	case ELEMENT_BOOL:
	case ELEMENT_BINOP:
	case ELEMENT_UNOP:
	case ELEMENT_COMB:
	case ELEMENT_CONSTRUCTOR:
		hash = INT_TO_HASH( PEGETTYPE( e ) + PEGETVAL( e ) );
		break;

	case ELEMENT_ELIST:
		hash = INT_TO_HASH( ELEMENT_ELIST );
		break;

	case ELEMENT_TAG:
		hash = INT_TO_HASH( g_str_hash( PEGETTAG( e ) ) );
		break;

	case ELEMENT_MANAGED:
		hash = INT_TO_HASH( PEGETMANAGED( e )->hash );
		break;

	case ELEMENT_NOVAL:
	default:
		g_assert( 0 );
	}

	return( hash );
}

/* Calculate a hash for every node in a tree. We can just recurse and
 * calculate bottom-up, since we'll never get very deep. If we were scanning
 * run-time code, we'd need a better scheme.
 */
static CompileHash
compile_share_scan_node( CompileShare *share, HeapNode *hn )
{
	CompileHash hash;
	PElement a;

	hash = INT_TO_HASH( 0 );
	switch( hn->type ) {
	case TAG_CONS:
	case TAG_GEN:
	case TAG_CLASS:
	case TAG_COMPLEX:
	case TAG_APPL:
		PEPOINTLEFT( hn, &a );
		hash = COMPILEHASH_ADD( hash, 
			compile_share_scan_element( share, &a ) );
		PEPOINTRIGHT( hn, &a );
		hash = COMPILEHASH_ADD( hash, 
			compile_share_scan_element( share, &a ) );
		hash = COMPILEHASH_ADD( hash, 
			INT_TO_HASH( (int) hn->type ) );
		break;

	case TAG_DOUBLE:
		hash = COMPILEHASH_ADD( hash, 
			INT_TO_HASH( (int) hn->body.num ) );
		hash = COMPILEHASH_ADD( hash, 
			INT_TO_HASH( (int) hn->type ) );
		break;

	case TAG_FILE:
	case TAG_REFERENCE:
	case TAG_SHARED:
	case TAG_FREE:
	default:
		g_assert( FALSE );
	}

	/* Add to accumulated table.
	 */
	compile_share_add( share, hn, hash );

	return( hash );
}

/* Test two sub-trees for equality.
 */
static gboolean
compile_equal_node( HeapNode *hn1, HeapNode *hn2 )
{
	/* Test for pointer equality.
	 */
	if( hn1 == hn2 )
		return( TRUE );

	/* Test type tags for equality.
	 */
	if( hn1->type != hn2->type )
		return( FALSE );

	/* If double, test immediately.
	 */
	if( hn1->type == TAG_DOUBLE ) {
		if( hn1->body.num == hn2->body.num )
			return( TRUE );
		else
			return( FALSE );
	}

	/* If complex, test immediately.
	 */
	if( hn1->type == TAG_COMPLEX ) {
		if( GETLEFT( hn1 )->body.num == GETLEFT( hn2 )->body.num &&
			GETRIGHT( hn1 )->body.num == GETRIGHT( hn2 )->body.num )
			return( TRUE );
		else
			return( FALSE );
	}

	/* If compound type, something is wrong! Only built by reduce.
	 */
	g_assert( hn1->type != TAG_CLASS );

	/* In two parts, test tags.
	 */
	if( GETLT( hn1 ) != GETLT( hn2 ) )
		return( FALSE );
	if( GETRT( hn1 ) != GETRT( hn2 ) )
		return( FALSE );

	/* Test non-subtree parts.
	 */
	if( GETLT( hn1 ) != ELEMENT_NODE ) 
		if( GETLEFT( hn1 ) != GETLEFT( hn2 ) )
			return( FALSE );
	if( GETRT( hn1 ) != ELEMENT_NODE ) 
		if( GETRIGHT( hn1 ) != GETRIGHT( hn2 ) )
			return( FALSE );

	/* If sub-trees, test them.
	 */
	if( GETLT( hn1 ) == ELEMENT_NODE ) 
		if( !compile_equal_node( GETLEFT( hn1 ), GETLEFT( hn2 ) ) )
			return( FALSE );
	if( GETRT( hn1 ) == ELEMENT_NODE ) 
		if( !compile_equal_node( GETRIGHT( hn1 ), GETRIGHT( hn2 ) ) )
			return( FALSE );

	return( TRUE );
}

/* Found two equal sub-expressions. We can change hn1 to just be a reference
 * to hn2.
 */
static int
compile_transform_reference( Compile *compile, HeapNode *hn1, HeapNode *hn2 )
{
#ifdef DEBUG
{
	Heap *heap = compile->heap;
	char txt[100];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_node( heap, &buf, hn1, TRUE );
	printf( "Found common subexpression: %s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG*/

#ifdef DEBUG_COMMON
	compile_node_sum += compile_node_count( hn1 );
#endif /*DEBUG_COMMON*/

	/* Zap nodes to indicate sharing. 
	 */
	hn1->type = TAG_REFERENCE;
	PPUTLEFT( hn1, ELEMENT_NODE, hn2 );
	PPUTRIGHT( hn1, ELEMENT_NODE, NULL );

	return( 0 );
}

/* Node other hashes to the same value as our node. Test for equality, and if
 * they match, turn us into a share point and turn the other node into a ref.
 */
static void *
compile_share_test( HeapNode *other, CompileShare *share, HeapNode *hn )
{
	if( hn != other && compile_equal_node( hn, other ) ) {
		heap_map( other,
			(heap_map_fn) compile_share_remove, share, NULL );
		compile_transform_reference( share->compile, other, hn );
	}

	return( NULL );
}

/* Scan a chunk of tree top-down, looking for and eliminating common nodes.
 */
static void
compile_share_trim( CompileShare *share, HeapNode *hn )
{
	PElement a;
	GSList *nodel;

	if( (nodel = compile_share_lookup( share, hn )) ) 
		slist_map2( nodel,
			(SListMap2Fn) compile_share_test, share, hn );

	switch( hn->type ) {
	case TAG_CONS:
	case TAG_GEN:
	case TAG_CLASS:
	case TAG_COMPLEX:
	case TAG_APPL:
		PEPOINTLEFT( hn, &a );
		if( PEISNODE( &a ) )
			compile_share_trim( share, PEGETVAL( &a ) );
		PEPOINTRIGHT( hn, &a );
		if( PEISNODE( &a ) )
			compile_share_trim( share, PEGETVAL( &a ) );
		break;

	case TAG_DOUBLE:
	case TAG_REFERENCE:
		break;

	case TAG_SHARED:
	case TAG_FREE:
	case TAG_FILE:
	default:
		g_assert( FALSE );
	}
}

static void
compile_share_scan( Compile *compile, PElement *a )
{
	if( PEISNODE( a ) ) {
		HeapNode *hn = PEGETVAL( a );
		CompileShare share;

		compile_share_init( &share, compile );
		compile_share_scan_node( &share, hn );
		compile_share_trim( &share, hn );
		compile_share_destroy( &share );
	}
}

/* Use this to generate an id for each SHARE node.
 */
static int compile_share_number = 0;

/* If this is a REF node, make sure dest is a SHARE node.
 */
static void *
compile_transform_share( HeapNode *hn, Compile *compile )
{
	Heap *heap = compile->heap;

	if( hn->type == TAG_REFERENCE ) {
		HeapNode *hn1 = GETLEFT( hn );

		if( hn1->type != TAG_SHARED ) {
			HeapNode *hn2;

#ifdef DEBUG
{
			char txt[100];
			VipsBuf buf = VIPS_BUF_STATIC( txt );

			graph_node( heap, &buf, hn1, TRUE );
			printf( "Found shared code: %s\n", 
				vips_buf_all( &buf ) );
}
#endif /*DEBUG*/

			if( NEWNODE( heap, hn2 ) )
				return( hn );
			*hn2 = *hn1;
			hn1->type = TAG_SHARED;
			PPUT( hn1, 
				ELEMENT_NODE, hn2, 
				ELEMENT_CHAR, GUINT_TO_POINTER( 
					compile_share_number ) );

			compile_share_number++;
			if( compile_share_number == MAX_RELOC ) {
				error_top( _( "Too many shared nodes in "
					"graph." ) );
				error_sub( _( "Raise MAX_RELOC" ) );
				return( hn );
			}
		}
	}

	return( NULL );
}

/* Do common-subexpression elimination. 
 */
static gboolean
compile_remove_subexpr( Compile *compile, PElement *root )
{
	HeapNode *rootn = PEGETVAL( root );
#ifdef DEBUG_COMMON
	static int compile_node_total = 0;
#endif /*DEBUG_COMMON*/

	if( PEGETTYPE( root ) != ELEMENT_NODE )
		/* Nowt to do.
		 */
		return( TRUE );

#ifdef DEBUG_COMMON
	compile_node_sum = 0;
#endif /*DEBUG_COMMON*/

	/* Scan for common nodes, replace stuff we remove with REFERENCE
  	 * nodes.
	 */
	compile_share_scan( compile, root );

	/* Now search for destinations of reference nodes and mark all shared
	 * sections. Each shared section is given a number ... saves a lookup
	 * during copy.
	 */
	compile_share_number = 0;
	if( heap_map( rootn, 
		(heap_map_fn) compile_transform_share, compile, NULL ) ) {
		/* We can't leave the graph half-done, it'll confuse the copier
		 * later. Zap the graph.
		 */
		PEPUTP( root, ELEMENT_NOVAL, NULL );
		return( FALSE );
	}

#ifdef DEBUG_COMMON
	if( compile_node_sum ) {
		compile_node_total += compile_node_sum;
		printf( "compile_remove_subexpr: " );
		symbol_name_print( compile->sym );
		printf( "saved %d nodes (total %d)\n", 
			compile_node_sum, compile_node_total );
	}
#endif /*DEBUG_COMMON*/

	return( TRUE );
}

/* Top-level compiler driver.
 */

/* Compile a symbol into a heap. 
 */
static void *
compile_heap( Compile *compile )
{
	PElement base;

	/* Don't generate code for parser temps.
	 */
	if( compile->sym->placeholder )
		return( NULL );

	PEPOINTE( &base, &compile->base );

	/* Is there an existing function base? GC it away.
	 */
	if( PEGETTYPE( &base ) != ELEMENT_NOVAL ) {
		PEPUTP( &base, ELEMENT_NOVAL, (void *) 2 );
		if( !heap_gc( compile->heap ) )
			return( compile->sym );

		return( NULL );
	}

#ifdef DEBUG
	printf( "*** compile_expr: about to compile " );
	symbol_name_print( compile->sym );
	printf( "\n" );
	if( compile->tree )
		dump_tree( compile->tree );
#endif /*DEBUG*/

	/* Compile function body. Tree can be NULL for classes.
	 */
	if( compile->tree ) {
		if( !compile_graph( compile, compile->tree, &base ) )
			return( compile->sym );
	}
	else {
		PEPUTP( &base, ELEMENT_NOVAL, (void *) 3 );
	}

#ifdef DEBUG
{
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_pelement( compile->heap, &buf, &base, TRUE );
	printf( "before var abstraction, compiled \"%s\" to: %s\n", 
		IOBJECT( compile->sym )->name, vips_buf_all( &buf ) );
}
#endif /*DEBUG*/

	/* Abstract real parameters.
	 */
#ifdef DEBUG
	printf( "abstracting real params ...\n" );
#endif /*DEBUG*/
	if( slist_map2_rev( compile->param,
		(SListMap2Fn) compile_abstract_symbol, compile, &base ) )
			return( compile->sym );

	/* Abstract secret parameters. 
	 */
#ifdef DEBUG
	printf( "abstracting secret params ...\n" );
#endif /*DEBUG*/
	if( slist_map2_rev( compile->secret,
		(SListMap2Fn) compile_abstract_symbol, compile, &base ) )
			return( compile->sym );

	/* Remove common sub-expressions.
	 */
	if( !compile_remove_subexpr( compile, &base ) )
		return( compile->sym );

#ifdef DEBUG_RESULT
{
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	printf( "compiled " );
	symbol_name_print( compile->sym );
	printf( "to: " );
	graph_pelement( compile->heap, &buf, &base, TRUE );
	printf( "%s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG_RESULT*/

	return( NULL );
}

static void *compile_object_sub( Compile *compile );

static void *
compile_symbol_sub( Symbol *sym )
{
	Compile *compile;

	if( sym->expr && (compile = sym->expr->compile) )
		if( compile_object_sub( compile ) )
			return( sym );

	return( NULL );
}

static void *
compile_object_sub( Compile *compile )
{
	if( icontainer_map( ICONTAINER( compile ),
		(icontainer_map_fn) compile_symbol_sub, NULL, NULL ) )
		return( compile );

	if( compile_heap( compile ) )
		return( compile );

	return( NULL );
}

/* Top-level compile a thing entry point.
 */
void *
compile_object( Compile *compile )
{
	/* Walk this tree of symbols computing the secret lists.
	 */
	secret_build( compile );

	/* Compile all definitions from the inside out.
	 */
	if( compile_object_sub( compile ) )
		return( compile );

	return( NULL );
}

static void *
compile_toolkit_sub( Tool *tool )
{
	Compile *compile;

	if( tool->sym && tool->sym->expr && 
		(compile = tool->sym->expr->compile )) 
		/* Only if we have no code.
		 */
		if( compile->base.type == ELEMENT_NOVAL ) 
			if( compile_object( compile ) )
				return( tool );

	return( NULL );
}

/* Scan a toolkit and make sure all the symbols have been compiled.
 */
void *
compile_toolkit( Toolkit *kit )
{
	return( toolkit_map( kit,
		(tool_map_fn) compile_toolkit_sub, NULL, NULL ) );
}

/* Parse support.
 */

static ParseNode *
compile_check_i18n( Compile *compile, ParseNode *pn )
{
	switch( pn->type ) {
	case NODE_APPLY:
		if( pn->arg1->type == NODE_LEAF && 
			strcmp( IOBJECT( pn->arg1->leaf )->name, "_" ) == 0 &&
			pn->arg2->type == NODE_CONST &&
			pn->arg2->con.type == PARSE_CONST_STR ) {
			const char *text = pn->arg2->con.val.str;

			if( main_option_i18n ) {
				/* Remove msgid duplicates with this.
				 */
				static GHashTable *msgid = NULL;

				if( !msgid ) 
					msgid = g_hash_table_new( 
						g_str_hash, g_str_equal );

				if( !g_hash_table_lookup( msgid, text ) ) {
					char buf[MAX_STRSIZE];

					g_hash_table_insert( msgid, 
						(void *) text, NULL ); 
					my_strecpy( buf, text, TRUE );
					printf( "msgid \"%s\"\n", buf );
					printf( "msgstr \"\"\n\n" );
				}
			}

			/* We can gettext these at compile time. Replace the
			 * APPLY node with a fixed-up text string.
			 */
			pn->type = NODE_CONST;
			pn->con.type = PARSE_CONST_STR;
			pn->con.val.str = im_strdupn( _( text ) );
		}
		break;

	default:
		break;
	}

	return( NULL );
}

static ParseNode *
compile_check_more( Compile *compile, ParseNode *pn )
{
	switch( pn->type ) {
	case NODE_BINOP:
		switch( pn->biop ) {
		case BI_MORE:
			pn->biop = BI_LESS;
			SWAPP( pn->arg1, pn->arg2 );
			break;

		case BI_MOREEQ:
			pn->biop = BI_LESSEQ;
			SWAPP( pn->arg1, pn->arg2 );
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return( NULL );
}

/* Do end-of-parse checks. Called after every 'A = ...' style definition (not 
 * just top-level syms). Used to do lots of checks, not much left now.
 */
gboolean
compile_check( Compile *compile )
{
	Symbol *sym = compile->sym;
	Symbol *parent = symbol_get_parent( sym );

	/* Check "check" member. 
	 */ 
	if( is_member( sym ) &&
		strcmp( IOBJECT( sym )->name, MEMBER_CHECK ) == 0 ) {
		if( compile->nparam != 0 ) {
			error_top( _( "Too many arguments." ) );
			error_sub( _( "Member \"%s\" of class "
				"\"%s\" should have no arguments." ),
				MEMBER_CHECK, symbol_name( parent ) );

			return( FALSE );
		}
	}

	/* Look for (_ "string constant") and pump it through gettext. We can
	 * do a lot of i18n at compile-time.
	 */
#ifdef DEBUG
	printf( "compile_check_i18n: " );
	compile_name_print( compile );
	printf( "\n" );
#endif /*DEBUG*/
	(void) tree_map( compile, 
		(tree_map_fn) compile_check_i18n, compile->tree, NULL, NULL );

	/* Swap MORE and MOREEQ for LESS and LESSEQ. Reduces the number of
	 * cases for the compiler. 
	 */
	(void) tree_map( compile, 
		(tree_map_fn) compile_check_more, compile->tree, NULL, NULL );

	return( TRUE );
}

/* Mark error on all exprs using this compile.
 */
void
compile_error_set( Compile *compile )
{
	(void) slist_map( compile->exprs, (SListMapFn) expr_error_set, NULL );
}

/* Patch a pointer on a patch list. 
 */
static void *
compile_patch_pointers_sub( void **pnt, void *nsym, void *osym )
{
	g_assert( *pnt == osym );

	*pnt = nsym;

	return( NULL );
}

/* Patch pointers to old to point to new instead.
 */
static void
compile_patch_pointers( Symbol *nsym, Symbol *osym )
{
	(void) slist_map2( osym->patch, 
		(SListMap2Fn) compile_patch_pointers_sub, nsym, osym );
}

/* Sub fn of below.
 */
static void *
compile_resolve_sub( Compile *pnt, Symbol *sym )
{
	if( !g_slist_find( sym->parents, pnt ) ) 
		sym->parents = g_slist_prepend( sym->parents, pnt );

	return( NULL );
}

/* Sub fn 2 of below.
 */
static void *
compile_resolve_sub1( Compile *compile )
{
	return( symbol_fix_counts( compile->sym ) );
}

/* We've found a symbol which is the true definition of an unresolved symbol.
 * We fiddle references to zombie to refer to sym instead.
 */
static void
compile_resolve( Symbol *sym, Symbol *zombie )
{
#ifdef DEBUG_RESOLVE
	printf( "compile_resolve: resolving zombie " );
	symbol_name_print( zombie );
	printf( "to symbol " );
	symbol_name_print( sym );
	printf( "\n" );
#endif /*DEBUG_RESOLVE*/

	/* Symbol on outer table. Patch pointers to zombie to point to
	 * sym instead.
	 */
	compile_patch_pointers( sym, zombie );

	/* Also unresolved in outer scope?
	 */
	if( sym->type == SYM_ZOMBIE )
		/* We may need to move it again - so add the patch
		 * pointers we have just used to the patch list on
		 * sym.
		 */
		(void) slist_map( zombie->patch, 
			(SListMapFn) symbol_patch_add, sym );

	/* Add other information the ZOMBIE has picked up. We only
	 * need to make the link one way: the patching will make the
	 * other half for us.
	 */
	(void) slist_map( zombie->parents, 
		(SListMapFn) compile_resolve_sub, sym );

	/* Make sure the dirty counts are set correctly. We have
	 * changed dep (maybe), so need a fiddle.
	 */
	(void) slist_map( zombie->parents, 
		(SListMapFn) compile_resolve_sub1, NULL );

	/* No one refers to the zombie now.
	 */
	IM_FREEF( g_slist_free, zombie->parents );

	IDESTROY( zombie );
}

/* Sub-function of below.
 */
static void *
compile_resolve_names_sub( Symbol *sym, Compile *outer )
{
	const char *name = IOBJECT( sym )->name;
	Symbol *old;

	/* Is it the sort of thing we are looking for? ZOMBIEs only, please.
	 */
	if( sym->type != SYM_ZOMBIE )
		return( NULL );

	if( (old = compile_lookup( outer, name )) ) 
		compile_resolve( old, sym );
	else {
		/* Nothing on the outer table of that name. Can just move the
		 * symbol across.
		 */
		g_object_ref( G_OBJECT( sym ) );
		icontainer_child_remove( ICONTAINER( sym ) );
		icontainer_child_add( ICONTAINER( outer ), 
			ICONTAINER( sym ), -1 );
		g_object_unref( G_OBJECT( sym ) );
	}

	return( NULL );
}

/* End of definition parse: we search the symbol table we have built for this
 * definition, looking for unresolved names (ZOMBIEs). If we find any, we move
 * the zombie to the enclosing symbol table, since the name may be
 * resolved one level up. If we find a symbol on the enclosing table of the
 * same name, we have to patch pointers to our inner ZOMBIE to point to this
 * new symbol. Nasty!
 */
void
compile_resolve_names( Compile *inner, Compile *outer )
{
	(void) icontainer_map( ICONTAINER( inner ), 
		(icontainer_map_fn) compile_resolve_names_sub, outer, NULL );
}

/* Hit a top-level zombie during reduction. Search outwards to root looking on
 * enclosing tables for a match.
 */
Symbol *
compile_resolve_top( Symbol *sym )
{
	Compile *enclosing;

	for( enclosing = COMPILE( ICONTAINER( sym )->parent ); enclosing;
		enclosing = compile_get_parent( enclosing ) ) {
		Symbol *outer_sym;

		if( (outer_sym = compile_lookup( enclosing, 
			IOBJECT( sym )->name )) &&
			outer_sym->type != SYM_ZOMBIE )
			return( outer_sym );
	}

	return( NULL );
}

/* Search outwards for this sym.
 */
static void *
compile_resolve_dynamic_sub( Symbol *sym, Compile *context )
{
	Compile *tab;

	if( sym->type != SYM_ZOMBIE )
		return( NULL );

	for( tab = context; tab; tab = compile_get_parent( tab ) ) {
		Symbol *def = compile_lookup( tab, IOBJECT( sym )->name );

		if( def && def->type != SYM_ZOMBIE ) {
			/* We've found a non-zombie! Bind and we're done.
			 */
			compile_resolve( def, sym );
			break;
		}
	}

	return( NULL );
}

/* Resolve ZOMBIEs in tab by searching outwards from context. We only move 
 * and patch if we find a match ... otherwise we leave the zombie where is is.
 *
 * This is used for dynamic exprs in the tally display: we don't care about
 * fwd refs, but we want to be able to handle multiple scope contexts.
 */
void 
compile_resolve_dynamic( Compile *tab, Compile *context )
{
	(void) icontainer_map( ICONTAINER( tab ), 
		(icontainer_map_fn) compile_resolve_dynamic_sub, 
		context, NULL );
}

Symbol *
compile_get_member( Compile *compile, const char *name )
{
	iContainer *child;

	if( is_class( compile ) && 
		(child = icontainer_child_lookup( ICONTAINER( compile ), 
			name )) )
		return( SYMBOL( child ) );

	return( NULL );
}

const char *
compile_get_member_string( Compile *compile, const char *name )
{
	Symbol *member;
	Compile *member_compile;

	if( (member = compile_get_member( compile, name )) &&
		is_value( member ) && 
		(member_compile = member->expr->compile) &&
		member_compile->tree && 
		member_compile->tree->type == NODE_CONST &&
		member_compile->tree->con.type == PARSE_CONST_STR ) 
		return( member_compile->tree->con.val.str );

	return( NULL );
}

static void *
compile_find_generated_node( Compile *compile, ParseNode *node,
	GSList **list )
{
	Symbol *sym = node->leaf;

        if( node->type == NODE_LEAF &&
		sym->generated &&
		symbol_get_parent( sym ) &&
		symbol_get_parent( sym )->expr->compile == compile ) 
		*list = g_slist_prepend( *list, sym );

	return( NULL );
}

/* Search a scrap of tree and build a list of all the lambdas/lcomps/etc. it
 * generated.
 */
static GSList *
compile_find_generated( Compile *compile, ParseNode *tree )
{
	GSList *list;

	list = NULL;
	tree_map( compile,
		(tree_map_fn) compile_find_generated_node, tree, &list, NULL );

	return( list );
}

/* Make a copy of sym (and all it's children and trees) in the destination 
 * scope. This only works for stuff from the parse stage. Symbols which have
 * values and stuff attached are too complicated to copy easily.
 */
static void *
compile_copy_sym( Symbol *sym, Compile *dest )
{
	const char *name = IOBJECT( sym )->name;
	Symbol *copy_sym;

#ifdef DEBUG
	printf( "compile_copy_sym: copying " );
	symbol_name_print( sym );
	printf( "to scope of " );
	compile_name_print( dest );
	printf( "\n" );
#endif /*DEBUG*/

	/* Must be a different place.
	 */
	g_assert( symbol_get_parent( sym )->expr->compile != dest );

	/* Must not be an existing sym of that name. Or if there is, it has to
	 * be a zombie.
	 */
	g_assert( !compile_lookup( dest, name ) ||
		compile_lookup( dest, name )->type == SYM_ZOMBIE );

	switch( sym->type ) {
	case SYM_VALUE:
		copy_sym = symbol_new_defining( dest, name );
		copy_sym->generated = sym->generated;
		(void) symbol_user_init( copy_sym );
		(void) compile_new_local( copy_sym->expr );

		/* Copy any locals over. We have to do this before we copy the
		 * tree so that the new tree links to the new syms.
		 */
		icontainer_map( ICONTAINER( sym->expr->compile ),
			(icontainer_map_fn) compile_copy_sym, 
			copy_sym->expr->compile, NULL );

		copy_sym->expr->compile->tree = tree_copy( 
			copy_sym->expr->compile, sym->expr->compile->tree );

		/* Copying the tree may have made some zombies. Resolve 
		 * outwards.
		 */
		compile_resolve_names( copy_sym->expr->compile, dest );

		break;

	case SYM_PARAM:
		copy_sym = symbol_new_defining( dest, name );
		copy_sym->generated = sym->generated;
		symbol_parameter_init( copy_sym );
		break;

	case SYM_ZOMBIE:
		break;

	case SYM_WORKSPACE:
	case SYM_WORKSPACEROOT:
	case SYM_ROOT:
	case SYM_EXTERNAL:
	case SYM_BUILTIN:
	default:
		g_assert( 0 );
	}

	return( NULL );
}

/* tree is a scrap of graph in fromscope's context. It may have caused the
 * generation of a number of lambdas, lcomps etc. in fromscope. Make a copy
 * of the tree in toscope and copy over any generated syms too. fromscope and
 * toscope can be the same, in which case we can just copy the tree.
 */
ParseNode *
compile_copy_tree( Compile *fromscope, ParseNode *tree, Compile *toscope )
{
	ParseNode *copy_tree;

#ifdef DEBUG
	printf( "compile_copy_tree: copying tree from " );
	compile_name_print( fromscope );
	printf( " to " );
	compile_name_print( toscope );
	printf( "\n" );
#endif /*DEBUG*/

	/* A new context? Copy generated syms over.
	 */
	if( fromscope != toscope ) {
		GSList *generated;

		generated = compile_find_generated( fromscope, tree );

#ifdef DEBUG
		printf( "with generated children: " ); 
		(void) slist_map( generated, (SListMapFn) dump_tiny, NULL );
		printf( "\n" ); 
#endif /*DEBUG*/

		slist_map( generated, 
			(SListMapFn) compile_copy_sym, toscope );

		g_slist_free( generated );
	}

	copy_tree = tree_copy( toscope, tree );

	/* Copying the tree may have made some zombies. Resolve 
	 * outwards.
	 */
	compile_resolve_names( toscope, compile_get_parent( toscope ) );

	return( copy_tree );
}

/* Generate the parse tree for this list comprehension. 

  Example: after parse we have:

	[(x, y) :: x <- [1..3]; y <- [x..3]; x + y > 3];

	... $$lcomp1 ...
	{
		$$lcomp1 = NULL
		{
			$$result = (x, y);
			// elements in left-to-right order
			// in compile->children
			x = [1..3]
			y = [x..3]
			$$filter1 = x + y > 3
		}
	}

  and we generate this code:

  	z = $$lcomp1
	{
		$$lcomp1 = foldr $f1 [] [1..3]
		{
			$f1 x $sofar = foldr $f2 $sofar [x..3]
			{
				$f2 y $sofar = if x + y > 3 then $f3 else $sofar
				{
					$f3 = (x, y) : $sofar;
				}
			}
		}
	}

 */

/* Find the placeholders generated by the parser. Filters, generators,
 * patterns and $$result. 
 */
static void *
compile_lcomp_find( Symbol *sym, GSList **children )
{
	if( sym->placeholder )
		*children = g_slist_append( *children, sym );

	return( NULL );
}

static Symbol *
compile_lcomp_find_pattern( GSList *children, const char *generator )
{
	int n;
	char pattern[256];
	GSList *p;

	if( sscanf( generator, "$$generator%d", &n ) != 1 )
		return( NULL );
	im_snprintf( pattern, 256, "$$pattern%d", n );

	for( p = children; p; p = p->next ) {
		Symbol *sym = (Symbol *) p->data;

		if( strcmp( IOBJECT( sym )->name, pattern ) == 0 )
			return( sym );
	}

	return( NULL );
}

void 
compile_lcomp( Compile *compile )
{
	/* Number nested locals with this. Keep numbering global so debugging
	 * nested lcomps is easier.
	 */
	static int count = 1;

	GSList *children;
	gboolean sofar;
	Compile *scope;
	Symbol *result;
	GSList *p;
	Symbol *child;
	char name[256];
	ParseNode *n1, *n2, *n3;

#ifdef DEBUG_LCOMP
	printf( "before compile_lcomp:\n" );
	dump_compile( compile );
#endif /*DEBUG_LCOMP*/

	/* Find all the elements of the lcomp: generators, filters, patterns 
	 * and $$result.
	 */
	children = NULL;
	(void) icontainer_map( ICONTAINER( compile ), 
		(icontainer_map_fn) compile_lcomp_find, &children, NULL );

#ifdef DEBUG_LCOMP
	printf( "list comp " );
	compile_name_print( compile );
	printf( " has children: " ); 
	(void) slist_map( children, (SListMapFn) dump_tiny, NULL );
	printf( "\n" ); 
#endif /*DEBUG_LCOMP*/

	/* As yet no list to build on.
	 */
	sofar = FALSE;

	/* Start by building a tree in this scope.
	 */
	scope = compile;

	/* Not seen the result element yet, but we should.
	 */
	result = NULL;

	/* Now generate code for each element, either a filter or a generator.
	 * If we do a generator, we need to search for the associated pattern
	 * and expand it.
	 */
	for( p = children; p; p = p->next ) {
		Symbol *element = (Symbol *) p->data;

		/* Just note the result element ... we use it right at the end.
		 */
		if( strcmp( "$$result", IOBJECT( element )->name ) == 0 ) {
			result = element;
			continue;
		}

		/* And only process filter/gen.
		 */
		if( !is_prefix( "$$filter", IOBJECT( element )->name ) &&
			!is_prefix( "$$gen", IOBJECT( element )->name ) ) 
			continue;

		/* Start the next nest in. child is the local we will make for
		 * this scope.
		 */
		im_snprintf( name, 256, "$$fn%d", count++ );
		child = symbol_new_defining( scope, name );
		child->generated = TRUE;
		(void) symbol_user_init( child );
		(void) compile_new_local( child->expr );

		if( is_prefix( "$$filter", IOBJECT( element )->name ) ) {
			/* A filter.
			 */
			n1 = compile_copy_tree( compile, 
				element->expr->compile->tree,
				scope );
			n2 = tree_leafsym_new( scope, child );
			n3 = tree_leaf_new( scope, "$$sofar" );
			n1 = tree_ifelse_new( scope, n1, n2, n3 );
			scope->tree = n1;
		}
		else if( is_prefix( "$$gen", IOBJECT( element )->name ) ) {
			Symbol *param1;
			Symbol *param2;
			Symbol *pattern;
			GSList *built_syms;

			/* A generator. 
			 */
			param1 = symbol_new_defining( child->expr->compile, 
				IOBJECT( element )->name );
			param1->generated = TRUE;
			symbol_parameter_init( param1 );
			param2 = symbol_new_defining( child->expr->compile, 
				"$$sofar" );
			param2->generated = TRUE;
			symbol_parameter_init( param2 );

			/* Now expand the pattern: it will access parts of the
			 * $$generator argument.
			 */
			pattern = compile_lcomp_find_pattern( children, 
				IOBJECT( element )->name );
			g_assert( pattern );
			built_syms = compile_pattern_lhs( child->expr->compile, 
				param1, pattern->expr->compile->tree );
			g_slist_free( built_syms );

			/* Make the "foldr $$fn $sofar expr" tree.
			 */
			n1 = tree_leaf_new( scope, "foldr" );
			n2 = tree_leafsym_new( scope, child );
			n3 = tree_appl_new( scope, n1, n2 );
			if( sofar )
				n2 = tree_leaf_new( scope, "$$sofar" );
			else {
				ParseConst con;

				con.type = PARSE_CONST_ELIST;
				n2 = tree_const_new( scope, con );
			}
			n3 = tree_appl_new( scope, n3, n2 );
			n2 = compile_copy_tree( compile,
				element->expr->compile->tree, 
				scope );
			n3 = tree_appl_new( scope, n3, n2 );
			scope->tree = n3;

			/* There's now an enclosing sofar we can use.
			 */
			sofar = TRUE;
		}

		/* Nest in again.
		 */
		scope = child->expr->compile;
	}

	/* Copy the code for the final result.
	 */
	g_assert( result );

	n1 = compile_copy_tree( result->expr->compile, 
		result->expr->compile->tree, scope );
	n2 = tree_leaf_new( scope, "$$sofar" );
	n3 = tree_binop_new( compile, BI_CONS, n1, n2 );
	scope->tree = n3;

	/* Loop outwards again, closing the scopes we made.
	 */
	while( scope != compile ) {
		/* We know check can't fail on generated code.

		   	FIXME ... yuk, maybe compile_lcomp should be 
			failable too

		 */
		(void) compile_check( scope );
		compile_resolve_names( scope, compile_get_parent( scope ) );

		scope = compile_get_parent( scope );
	}

#ifdef DEBUG_LCOMP
	printf( "after compile_lcomp:\n" );
	dump_compile( compile );
#endif /*DEBUG_LCOMP*/

	g_slist_free( children );
}

/* Compile a pattern LHS. Generate a sym for each pattern variable, each of
 * which checks and accesses sym. For example:
 *
 *	[a] = x;
 *
 * compiles to:
 *
 * 	sym = x;
 * 	a = if is_list sym && len sym == 1 then sym?0 else error "..";
 */

/* Generate code to access element n of a pattern trail. Eg, pattern is
 * 	[[[a]]]
 * the trail will be 
 * 	0) LISTCONST 1) LISTCONST 2) LISTCONST 3) LEAF
 * then access(0) will be
 * 	leaf
 * and access(1) will be
 * 	leaf?0
 * and access(3) (to get the value for a) will be
 * 	leaf?0?0?0
 */
static ParseNode *
compile_pattern_access( Compile *compile, 
	Symbol *leaf, ParseNode **trail, int n )
{
	ParseNode *node;
	ParseNode *left;
	ParseNode *right;
	ParseConst c;
	int i;

	/* The initial leaf ref we access from.
	 */
	node = tree_leafsym_new( compile, leaf );

	for( i = 0; i < n; i++ )
		switch( trail[i]->type ) {
		case NODE_CONST:
		case NODE_PATTERN_CLASS:
		case NODE_LEAF:
			break;

		case NODE_BINOP:
			switch( trail[i]->biop ) {
			case BI_COMMA:
				/* Generate re or im?
				 */
				if( trail[i]->arg1 == trail[i + 1] )
					left = tree_leaf_new( compile, "re" );
				else
					left = tree_leaf_new( compile, "im" );
				node = tree_appl_new( compile, left, node );
				break;

			case BI_CONS:
				/* Generate hd or tl?
				 */
				if( trail[i]->arg1 == trail[i + 1] )
					left = tree_leaf_new( compile, "hd" );
				else
					left = tree_leaf_new( compile, "tl" );
				node = tree_appl_new( compile, left, node );
				break;

			default:
				g_assert( 0 );
			}
			break;

		case NODE_LISTCONST:
			/* Which list element do we need? Look for the next
			 * item in the trail in the list of elements.
			 */
			c.type = PARSE_CONST_NUM;
			c.val.num = g_slist_index( trail[i]->elist, 
				trail[i + 1] );
			right = tree_const_new( compile, c );
			node = tree_binop_new( compile, 
				BI_SELECT, node, right );
			break;

		default:
			g_assert( 0 );
		}

	return( node );
}

/* Generate a parsetree for the condition test. The array of nodes represents
 * the set of conditions we have to test, left to right. 
 */
static ParseNode *
compile_pattern_condition( Compile *compile, 
	Symbol *leaf, ParseNode **trail, int depth )
{
	ParseConst n;
	ParseNode *node;
	ParseNode *node2;
	ParseNode *left;
	ParseNode *right;
	int i;

	n.type = PARSE_CONST_BOOL;
	n.val.bool = TRUE;
	node = tree_const_new( compile, n );

	for( i = depth - 1; i >= 0; i-- ) {
		switch( trail[i]->type ) {
		case NODE_LEAF:
			break;

		case NODE_BINOP:
			switch( trail[i]->biop ) {
			case BI_COMMA:
				/* Generate is_complex x.
				 */
				left = tree_leaf_new( compile, "is_complex" );
				right = compile_pattern_access( compile, 
					leaf, trail, i );
				node2 = tree_appl_new( compile, left, right );

				node = tree_binop_new( compile, 
					BI_LAND, node2, node );
				break;

			case BI_CONS:
				/* Generate is_list x && x != [].
				 */
				left = tree_leaf_new( compile, "is_list" );
				right = compile_pattern_access( compile, 
					leaf, trail, i );
				node2 = tree_appl_new( compile, left, right );

				node = tree_binop_new( compile, 
					BI_LAND, node2, node );

				left = compile_pattern_access( compile, 
					leaf, trail, i );
				n.type = PARSE_CONST_ELIST;
				right = tree_const_new( compile, n );
				node2 = tree_binop_new( compile, 
					BI_NOTEQ, left, right );

				node = tree_binop_new( compile, 
					BI_LAND, node, node2 );
				break;

			default:
				g_assert( 0 );
			}
			break;

		case NODE_LISTCONST:
			/* Generate is_list x && is_list_len n x.
			 */
			left = tree_leaf_new( compile, "is_list" );
			right = compile_pattern_access( compile, 
				leaf, trail, i );
			node2 = tree_appl_new( compile, left, right );

			node = tree_binop_new( compile, BI_LAND, node2, node );

			left = tree_leaf_new( compile, "is_list_len" );
			n.type = PARSE_CONST_NUM;
			n.val.num = g_slist_length( trail[i]->elist );
			right = tree_const_new( compile, n );
			left = tree_appl_new( compile, left, right );
			right = compile_pattern_access( compile, 
				leaf, trail, i );
			node2 = tree_appl_new( compile, left, right );

			node = tree_binop_new( compile, BI_LAND, node, node2 );
			break;

		case NODE_CONST:
			/* Generate x == n.
			 */
			left = compile_pattern_access( compile, 
				leaf, trail, i );
			right = tree_const_new( compile, trail[i]->con );
			node2 = tree_binop_new( compile, BI_EQ, left, right );

			node = tree_binop_new( compile, BI_LAND, node2, node );
			break;

		case NODE_PATTERN_CLASS:
			/* Generate is_instanceof "class-name" x.
			 */
			left = tree_leaf_new( compile, "is_instanceof" );
			n.type = PARSE_CONST_STR;
			n.val.str = im_strdupn( trail[i]->tag );
			right = tree_const_new( compile, n );
			node2 = tree_appl_new( compile, left, right );
			right = compile_pattern_access( compile, 
				leaf, trail, i );
			node2 = tree_appl_new( compile, node2, right );

			node = tree_binop_new( compile, BI_LAND, node2, node );
			break;

		default:
			g_assert( 0 );
		}
	}

	return( node );
}

/* Generate a parsetree for a "pattern match failed" error.
 */
static ParseNode *
compile_pattern_error( Compile *compile, Symbol *leaf )
{
	ParseNode *left;
	ParseConst n;
	ParseNode *right;
	ParseNode *node;

	left = tree_leaf_new( compile, "error" );
	n.type = PARSE_CONST_STR;
	n.val.str = im_strdupn( _( "pattern match failed" ) );
	right = tree_const_new( compile, n );
	node = tree_appl_new( compile, left, right );

	return( node );
}

/* Depth of trail we keep as we walk the pattern.
 */
#define MAX_TRAIL (10)

typedef struct _PatternLhs {
	Compile *compile;	/* Scope in which we generate new symbols */
	Symbol *sym;		/* Thing we access */

	/* The trail of nodes representing this slice of the pattern.
	 */
	ParseNode *trail[MAX_TRAIL];
	int depth;
	GSList *built_syms;
} PatternLhs;

/* Generate one reference. leaf is the new sym we generate.
 */
static void
compile_pattern_lhs_leaf( PatternLhs *lhs, Symbol *leaf )
{
	Symbol *sym;
	Compile *compile;

	sym = symbol_new_defining( lhs->compile, IOBJECT( leaf )->name );
	sym->generated = TRUE;
	(void) symbol_user_init( sym );
	(void) compile_new_local( sym->expr );
	lhs->built_syms = g_slist_prepend( lhs->built_syms, sym );
	compile = sym->expr->compile;

	compile->tree = tree_ifelse_new( compile, 
		compile_pattern_condition( compile, 
			lhs->sym, lhs->trail, lhs->depth ),
		compile_pattern_access( compile, 
			lhs->sym, lhs->trail, lhs->depth ),
		compile_pattern_error( compile, leaf ) );

#ifdef DEBUG_PATTERN
	printf( "compile_pattern_lhs_leaf: generated\n" );
	dump_compile( compile );
#endif /*DEBUG_PATTERN*/
}

/* Recurse over the pattern generating references.
 */
static void *
compile_pattern_lhs_sub( ParseNode *node, PatternLhs *lhs )
{
	lhs->trail[lhs->depth++] = node;

	switch( node->type ) {
	case NODE_LEAF:
		compile_pattern_lhs_leaf( lhs, node->leaf );
		break;

	case NODE_PATTERN_CLASS:
		compile_pattern_lhs_sub( node->arg1, lhs );
		break;

	case NODE_BINOP:
		compile_pattern_lhs_sub( node->arg1, lhs );
		compile_pattern_lhs_sub( node->arg2, lhs );
		break;

	case NODE_LISTCONST:
		slist_map( node->elist,
			(SListMapFn) compile_pattern_lhs_sub, lhs );
		break;

	case NODE_CONST:
		break;

	default:
		g_assert( 0 );
	}

	lhs->depth--;

	return( NULL );
}

/* Something like "[a] = [1];". sym is the $$pattern we are generating access 
 * syms for, node is the pattern tree, compile is the scope in which we
 * generate the new defining symbols. Return a list of the syms we built: they
 * will need any final finishing up and then having symbol_made() called on 
 * them. You need to free the list, too.
 */
GSList *
compile_pattern_lhs( Compile *compile, Symbol *sym, ParseNode *node )
{
	PatternLhs lhs;

#ifdef DEBUG_PATTERN
	printf( "compile_pattern_lhs: building access fns for %s\n", 
		symbol_name( sym ) );
#endif /*DEBUG_PATTERN*/

	lhs.compile = compile;
	lhs.sym = sym;
	lhs.depth = 0;
	lhs.built_syms = NULL;

	compile_pattern_lhs_sub( node, &lhs );

	g_assert( lhs.depth == 0 );

	return( lhs.built_syms );
}

static ParseNode *
compile_pattern_has_leaf_sub( Compile *compile, 
	ParseNode *node, void *a, void *b )
{
	if( node->type == NODE_LEAF )
		return( node );

	return( NULL );
}

/* Does a pattern contain a leaf? We don't allow const-only patterns in
 * definitions.
 */
gboolean
compile_pattern_has_leaf( ParseNode *node )
{
	return( tree_map( NULL, 
		(tree_map_fn) compile_pattern_has_leaf_sub, node, 
		NULL, NULL ) != NULL );
}
