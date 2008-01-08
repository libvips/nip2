/* Build parse trees.
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

/* Free any stuff attached to a ParseConst.
 */
void
tree_const_destroy( ParseConst *pc )
{
	if( pc->type == PARSE_CONST_STR )
		IM_FREE( pc->val.str );
	pc->type = PARSE_CONST_NONE;
}

void
tree_const_copy( ParseConst *from, ParseConst *to )
{
	*to = *from;
	if( to->type == PARSE_CONST_STR && to->val.str )
		to->val.str = im_strdupn( to->val.str );
}

/* Free a parse node.
 */
void *
tree_node_destroy( ParseNode *n )
{
	switch( n->type ) {
	case NODE_PATTERN_CLASS:
	case NODE_TAG:
		IM_FREE( n->tag );
		break;

	case NODE_CONST:
		tree_const_destroy( &n->con );
		break;

	case NODE_LISTCONST:
	case NODE_SUPER:
		IM_FREEF( g_slist_free, n->elist );
		break;

	case NODE_APPLY:
	case NODE_CLASS:
	case NODE_BINOP:
	case NODE_UOP:
	case NODE_LEAF:
	case NODE_GENERATOR:
	case NODE_NONE:
	case NODE_COMPOSE:
		break;

	default:
		assert( FALSE );
	}

	IM_FREE( n );

	return( NULL );
}

/* Make an empty parse node.
 */
static ParseNode *
tree_new( Compile *compile )
{
	ParseNode *no = INEW( NULL, ParseNode );

	no->compile = compile;
	no->type = NODE_NONE;
	no->biop = BI_NONE;
	no->uop = UN_NONE;
	no->arg1 = NULL;
	no->arg2 = NULL;
	no->arg3 = NULL;
	no->leaf = NULL;
	no->klass = NULL;
	no->elist = NULL;
	no->tag = NULL;
	no->con.type = PARSE_CONST_NONE;
	no->con.val.str = NULL;

	compile->treefrag = g_slist_prepend( compile->treefrag, no );

	return( no );
}

/* Make a binary operator node.
 */
ParseNode *
tree_binop_new( Compile *compile, BinOp op, ParseNode *l, ParseNode *r )
{
	ParseNode *no = tree_new( compile );

	no->type = NODE_BINOP;
	no->biop = op;
	no->arg1 = l;
	no->arg2 = r;

	return( no );
}

/* Make a function compose node.
 */
ParseNode *
tree_compose_new( Compile *compile, ParseNode *f, ParseNode *g )
{
	ParseNode *no = tree_new( compile );

	no->type = NODE_COMPOSE;
	no->arg1 = f;
	no->arg2 = g;

	return( no );
}

/* Make a generator node.
 */
ParseNode *
tree_generator_new( Compile *compile, ParseNode *s, ParseNode *n, ParseNode *f )
{
	ParseNode *no = tree_new( compile );

	no->type = NODE_GENERATOR;
	no->arg1 = s;
	no->arg2 = n;
	no->arg3 = f;

	return( no );
}

/* Make an IF node.
 */
ParseNode *
tree_ifelse_new( Compile *compile, ParseNode *c, ParseNode *t, ParseNode *e )
{
	ParseNode *else_node = tree_lconst_new( compile, e );
	ParseNode *then_node = tree_lconst_extend( compile, else_node, t );
	ParseNode *if_node = tree_binop_new( compile, BI_IF, c, then_node );

	return( if_node );
}

/* Make a class node.
 */
ParseNode *
tree_class_new( Compile *compile )
{
	ParseNode *no = tree_new( compile );
	Symbol *this, *super, *name, *cons;

	assert( !compile->is_klass );
	assert( !compile->this );
	assert( !compile->super );

	no->type = NODE_CLASS;
	no->klass = compile;

	/* Make enclosing into a class.
	 */
	compile->is_klass = TRUE;

	/* Add builtin syms. 
	 */
	this = symbol_new_defining( compile, MEMBER_THIS );
	(void) symbol_parameter_builtin_init( this );
	compile->this = this;

	super = symbol_new_defining( compile, MEMBER_SUPER );
	(void) symbol_user_init( super );
	(void) compile_new_local( super->expr );
	symbol_made( super );
	compile->super = super;

	name = symbol_new_defining( compile, MEMBER_NAME );
	(void) symbol_parameter_builtin_init( name );

	cons = symbol_new_defining( compile, IOBJECT( compile->sym )->name );
	(void) symbol_user_init( cons );
	(void) compile_new_local( cons->expr );
	cons->expr->compile->tree = tree_leafsym_new( compile, compile->sym );
	symbol_made( cons );

	return( no );
}

/* Make a tag node.
 */
ParseNode *
tree_tag_new( Compile *compile, const char *r )
{	
	ParseNode *no = tree_new( compile );

	no->type = NODE_TAG;
	no->tag = im_strdupn( r );

	return( no );
}

/* Make a unary operator node.
 */
ParseNode *
tree_unop_new( Compile *compile, UnOp op, ParseNode *a )
{
	ParseNode *no = tree_new( compile );

	no->type = NODE_UOP;
	no->uop = op;
	no->arg1 = a;

	return( no );
}

ParseNode *
tree_leaf_new( Compile *compile, const char *name )
{
	ParseNode *no = tree_new( compile );

	no->type = NODE_LEAF;
	no->leaf = symbol_new_reference( compile, name );

	/* Have we a reference to a ZOMBIE? If yes, we may need to patch this
	 * leaf to point to a new symbol. Add the leaf's pointer to the
	 * refedat list on the ZOMBIE.
	 */
	if( no->leaf->type == SYM_ZOMBIE )
		(void) symbol_patch_add( (void **) &no->leaf, no->leaf );

	return( no );
}

/* Make a new leaf node ... except we know the final symbol now. 
 */
ParseNode *
tree_leafsym_new( Compile *compile, Symbol *sym )
{
	ParseNode *no = tree_new( compile );

	/* Fill fields.
	 */
	no->type = NODE_LEAF;
	no->leaf = sym;

	/* Note that this compile refs this sym.
	 */
	compile_link_make( compile, sym );

	/* Have we a reference to a ZOMBIE? If yes, we may need to patch this
	 * leaf to point to a new symbol. Add the leaf's pointer to the
	 * refedat list on the ZOMBIE.
	 */
	if( sym->type == SYM_ZOMBIE )
		(void) symbol_patch_add( (void **) &no->leaf, sym );

	return( no );
}

/* Init a clist.
 */
ParseNode *
tree_lconst_new( Compile *compile, ParseNode *a )
{
	ParseNode *no = tree_new( compile );

	/* Fill fields.
	 */
	no->type = NODE_LISTCONST;
	no->elist = NULL;

	no->elist = g_slist_prepend( no->elist, a );

	return( no );
}

/* Extend a clist.
 */
ParseNode *
tree_lconst_extend( Compile *compile, ParseNode *base, ParseNode *new )
{
	assert( base->type == NODE_LISTCONST );

	base->elist = g_slist_prepend( base->elist, new );

	return( base );
}

/* Init a super.
 */
ParseNode *
tree_super_new( Compile *compile )
{
	ParseNode *no = tree_new( compile );

	no->type = NODE_SUPER;

	return( no );
}

/* Extend a super.
 */
ParseNode *
tree_super_extend( Compile *compile, ParseNode *base, ParseNode *new )
{
	assert( base->type == NODE_SUPER );

	base->elist = g_slist_append( base->elist, new );

	return( base );
}

/* Make a new constant node. 
 */
ParseNode *
tree_const_new( Compile *compile, ParseConst n )
{
	ParseNode *no = tree_new( compile );

	no->type = NODE_CONST;
	no->con = n;

	return( no );
}

/* Make a new apply node. 
 */
ParseNode *
tree_appl_new( Compile *compile, ParseNode *l, ParseNode *r )
{	
	ParseNode *no = tree_new( compile );

	no->type = NODE_APPLY;
	no->arg1 = l;
	no->arg2 = r;

	return( no );
}

ParseNode *
tree_pattern_class_new( Compile *compile, const char *class_name, ParseNode *l )
{
	ParseNode *no = tree_new( compile );

	no->type = NODE_PATTERN_CLASS;
	no->arg1 = l;
	no->tag = im_strdupn( class_name );

	return( no );
}

ParseNode *
tree_map( Compile *compile, tree_map_fn fn, ParseNode *node, void *a, void *b ) 
{
	ParseNode *result;
	GSList *l;

	g_assert( node );

	if( (result = fn( compile, node, a, b )) )
		return( result );

	switch( node->type ) {
	case NODE_GENERATOR:
		if( (result = tree_map( compile, fn, node->arg1, a, b )) )
			return( result );
		if( node->arg2 &&
			(result = tree_map( compile, fn, node->arg2, a, b )) )
			return( result );
		if( node->arg3 &&
			(result = tree_map( compile, fn, node->arg3, a, b )) )
			return( result );
		break;

	case NODE_APPLY:
	case NODE_BINOP:
	case NODE_COMPOSE:
		if( (result = tree_map( compile, fn, node->arg1, a, b )) ||
			(result = tree_map( compile, fn, node->arg2, a, b )) )
			return( result );
		break;

	case NODE_UOP:
		if( (result = fn( compile, node->arg1, a, b )) )
			return( result );
		break;

	case NODE_SUPER:
	case NODE_LISTCONST:
		for( l = node->elist; l; l = l->next ) {
			ParseNode *arg = (ParseNode *) l->data;

			if( (result = tree_map( compile, fn, arg, a, b )) )
				return( result );
		}
		break;

	case NODE_LEAF:
	case NODE_CLASS:
	case NODE_TAG:
	case NODE_CONST:
		break;

	case NODE_NONE:
	default:
		assert( FALSE );
	}

	return( NULL );
}

/* Copy a tree to a new context. Make all symbols afresh ... you need to link
 * after calling this. 
 */
ParseNode *
tree_copy( Compile *compile, ParseNode *node )
{
	ParseNode *copy;
	GSList *l;

	assert( node );

	switch( node->type ) {
	case NODE_GENERATOR:
	case NODE_APPLY:
	case NODE_BINOP:
	case NODE_COMPOSE:
	case NODE_UOP:
	case NODE_TAG:
	case NODE_CONST:
	case NODE_PATTERN_CLASS:
		copy = tree_new( compile );
		copy->type = node->type;
		copy->uop = node->uop;
		copy->biop = node->biop;
		if( node->tag )
			copy->tag = im_strdupn( node->tag );
		tree_const_copy( &node->con, &copy->con );
		if( node->arg1 )
			copy->arg1 = tree_copy( compile, node->arg1 );
		if( node->arg2 )
			copy->arg2 = tree_copy( compile, node->arg2 );
		if( node->arg3 )
			copy->arg3 = tree_copy( compile, node->arg3 );
		break;

	case NODE_SUPER:
	case NODE_LISTCONST:
		copy = tree_new( compile );
		for( l = node->elist; l; l = l->next ) {
			ParseNode *arg = (ParseNode *) l->data;

			copy->elist = g_slist_append( copy->elist, 
				tree_copy( compile, arg ) );
		}
		copy->type = node->type;
		break;

	case NODE_CLASS:
		copy = tree_class_new( compile );
		break;

	case NODE_LEAF:
		copy = tree_leaf_new( compile, IOBJECT( node->leaf )->name );
		break;

	case NODE_NONE:
	default:
		assert( FALSE );
	}

	return( copy );
}

