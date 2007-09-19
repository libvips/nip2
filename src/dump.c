/* Prettyprint various things for debugging.
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

/* A lot of this file is just for debugging.
 */
#define DEBUG

/* Dump a binary operator.
 */
char *
decode_BinOp( BinOp op )
{
	switch( op ) {
	case BI_NONE:		return( "(none)" );
	case BI_ADD:		return( "+" );
	case BI_SUB:		return( "-" );
	case BI_POW:		return( "**" );
	case BI_REM:		return( "%" );
	case BI_LSHIFT:		return( "<<" );
	case BI_RSHIFT:		return( ">>" );
	case BI_SELECT:		return( "?" );
	case BI_DIV:		return( "/" );
	case BI_JOIN:		return( "++" );
	case BI_COMMA:		return( "," );
	case BI_DOT:		return( "." );
	case BI_MUL:		return( "*" );
	case BI_LAND:		return( "&&" );
	case BI_LOR:		return( "||" );
	case BI_BAND:		return( "&" );
	case BI_BOR:		return( "|" );
	case BI_EOR:		return( "^" );
	case BI_EQ:		return( "==" );
	case BI_NOTEQ:		return( "!=" );
	case BI_PEQ:		return( "===" );
	case BI_PNOTEQ:		return( "!==" );
	case BI_LESS:		return( "<" );
	case BI_LESSEQ:		return( "<=" );
	case BI_MORE:		return( ">" );
	case BI_MOREEQ:		return( ">=" );
	case BI_IF:		return( "if_then_else" );
	case BI_CONS:		return( ":" );

	default:
		assert( FALSE );

		/* Keep gcc happy.
		 */
		return( NULL );
	}
}

/* Dump a unary operator.
 */
char *
decode_UnOp( UnOp op )
{
	switch( op ) {
	case UN_NONE:		return( "(none)" );
	case UN_CSCHAR:		return( "(signed char)" );
	case UN_CUCHAR:		return( "(unsigned char)" );
	case UN_CSSHORT:	return( "(signed short)" );
	case UN_CUSHORT:	return( "(unsigned short)" );
	case UN_CSINT:		return( "(signed int)" );
	case UN_CUINT:		return( "(unsigned int)" );
	case UN_CFLOAT:		return( "(float)" );
	case UN_CDOUBLE:	return( "(double)" );
	case UN_CCOMPLEX:	return( "(complex)" );
	case UN_CDCOMPLEX:	return( "(double complex)" );
	case UN_MINUS:		return( "-" );
	case UN_NEG:		return( "!" );
	case UN_COMPLEMENT:	return( "~" );
	case UN_PLUS:		return( "+" );

	default:
		assert( FALSE );

		/* Keep gcc happy.
		 */
		return( NULL );
	}
}

/* Decode a node tag.
 */
char *
decode_NodeType( NodeType tag )
{
	switch( tag ) {
	case TAG_APPL:		return( "TAG_APPL" );
	case TAG_CONS:		return( "TAG_CONS" );
	case TAG_FREE:		return( "TAG_FREE" );
	case TAG_DOUBLE:	return( "TAG_DOUBLE" );
	case TAG_COMPLEX:	return( "TAG_COMPLEX" );
	case TAG_CLASS:		return( "TAG_CLASS" );
	case TAG_GEN:		return( "TAG_GEN" );
	case TAG_FILE:		return( "TAG_FILE" );
	case TAG_SHARED:	return( "TAG_SHARED" );
	case TAG_REFERENCE:	return( "TAG_REFERENCE" );

	default:
		assert( FALSE );

		/* Keep gcc happy.
		 */
		return( NULL );
	}
}

/* Decode a CombinatorType.
 */
char *
decode_CombinatorType( CombinatorType comb ) 
{
	switch( comb ) {
        case COMB_S: 		return( "S" );
        case COMB_SL: 		return( "Sl" );
        case COMB_SR: 		return( "Sr" );
	case COMB_I:		return( "I" );
	case COMB_K:		return( "K" );
	case COMB_GEN:		return( "G" );
	default:
		assert( FALSE );

		/* Keep gcc happy.
		 */
		return( NULL );
	}
}

/* Decode a symbol type.
 */
char *
decode_SymbolType( SymbolType t )
{	
	switch( t ) {
	case SYM_VALUE:		return( "SYM_VALUE" );
	case SYM_PARAM:		return( "SYM_PARAM" );
	case SYM_ZOMBIE:	return( "SYM_ZOMBIE" );
	case SYM_WORKSPACE:	return( "SYM_WORKSPACE" );
	case SYM_WORKSPACEGROUP:return( "SYM_WORKSPACEGROUP" );
	case SYM_ROOT:		return( "SYM_ROOT" );
	case SYM_EXTERNAL:	return( "SYM_EXTERNAL" );
	case SYM_BUILTIN:	return( "SYM_BUILTIN" );

	default:
		assert( FALSE );
		return( NULL );
	}
}

/* Decode a symbol type into something users might like to see.
 */
char *
decode_SymbolType_user( SymbolType t )
{	
	switch( t ) {
	case SYM_VALUE:		return( _( "value" ) );
	case SYM_PARAM:		return( _( "parameter" ) );
	case SYM_ZOMBIE:	return( _( "zombie" ) );
	case SYM_WORKSPACE:	return( _( "workspace" ) );
	case SYM_WORKSPACEGROUP:return( _( "workspace group" ) );
	case SYM_ROOT:		return( _( "root symbol" ) );
	case SYM_EXTERNAL:	return( _( "external symbol" ) );
	case SYM_BUILTIN:	return( _( "built-in symbol" ) );

	default:
		assert( FALSE );
		return( NULL );
	}
}

/* From here on, just used for debugging.
 */
#ifdef DEBUG

#warning "DEBUG on in dump.c"

/* Do a tiny dump of a symbol. Just a few characters.
 */
void *
dump_tiny( Symbol *sym )
{
	BufInfo buf;
	char str[100];

	buf_init_static( &buf, str, 100 );
	symbol_qualified_name( sym, &buf );
	if( sym->dirty )
		printf( "*" );
	printf( "%s %s; ", decode_SymbolType( sym->type ), buf_all( &buf ) );

	return( NULL );
}

/* Dump a expr, tiny.
 */
static void *
dump_expr_tiny( Expr *expr )
{
	printf( "(expr->sym->name = " );
	symbol_name_print( expr->sym );
	printf( ") " );

	return( NULL );
}

/* Dump a expr info.
 */
void
dump_expr( Expr *expr )
{
	Symbol *sym = expr->sym;

	printf( "expr (%p)->sym->name = \"%s\"\n", 
		expr, IOBJECT( sym )->name );
	printf( "%s->row = %s\n", IOBJECT( sym )->name, 
		expr->row?"(set)":"(not set)" );

	if( expr->compile ) {
		printf( "%s->compile:\n", IOBJECT( sym )->name );
		dump_compile( expr->compile );
	}

	printf( "%s->expr->root = ", IOBJECT( sym )->name );
	if( !sym->dirty )
		pgraph( &expr->root );
	else
		printf( "<symbol is dirty ... can't print root>" );
	printf( "\n" );

	printf( "%s->expr->err = %s\n", 
		IOBJECT( sym )->name, bool_to_char( expr->err ) );
	printf( "%s->expr->error_top = \"%s\"\n", 
		IOBJECT( sym )->name, NN( expr->error_top ) );
	printf( "%s->expr->error_sub = \"%s\"\n", 
		IOBJECT( sym )->name, NN( expr->error_sub ) );
}

/* Dump a compile, tiny.
 */
static void *
dump_compile_tiny( Compile *compile )
{
	printf( "(compile->sym->name = " );
	symbol_name_print( compile->sym );
	printf( ") " );

	return( NULL );
}

/* Dump a compile.
 */
void
dump_compile( Compile *compile )
{
	Symbol *sym = compile->sym;
	BufInfo buf;
	char str[100];

	printf( "compile (%p)->sym->name = \"%s\"\n", 
		compile, IOBJECT( sym )->name );

	printf( "%s->class = %s\n", 
		IOBJECT( sym )->name, bool_to_char( compile->is_klass ) );
	printf( "%s->super = %s\n", 
		IOBJECT( sym )->name, bool_to_char( compile->has_super ) );

	printf( "%s->compile->text = \"%s\"\n", 
		IOBJECT( sym )->name, NN( compile->text ) );
	printf( "%s->compile->prhstext = \"%s\"\n", 
		IOBJECT( sym )->name, NN( compile->prhstext ) );
	printf( "%s->compile->rhstext = \"%s\"\n", 
		IOBJECT( sym )->name, NN( compile->rhstext ) );

	printf( "%s->compile->tree = \n", IOBJECT( sym )->name ); 
	if( compile->tree ) 
		(void) dump_tree( compile->tree ); 
	else
		printf( "   (no compile tree)\n" );
	printf( "%s->compile->treefrag = %d pointers\n", IOBJECT( sym )->name,
		g_slist_length( compile->treefrag ) );

	printf( "%s->compile->children =\n", 
		IOBJECT( sym )->name );
	(void) icontainer_map( ICONTAINER( compile ), 
		(icontainer_map_fn) dump_symbol, NULL, NULL );
	printf( "%s->compile->nparam = %d\n", 
		IOBJECT( sym )->name, compile->nparam );
	printf( "%s->compile->param = ", IOBJECT( sym )->name );
	(void) slist_map( compile->param, (SListMapFn) dump_tiny, NULL );
	printf( "\n" );
	printf( "%s->compile->nsecret = %d\n", 
		IOBJECT( sym )->name, compile->nsecret );
	printf( "%s->compile->secret = ", IOBJECT( sym )->name );
	(void) slist_map( compile->secret, (SListMapFn) dump_tiny, NULL );
	printf( "\n" );
	printf( "%s->compile->this = ", IOBJECT( sym )->name );
	if( compile->this )
		dump_tiny( compile->this );
	else
		printf( "(null)" );
	printf( "\n" );
	printf( "%s->compile->super = ", IOBJECT( sym )->name );
	if( compile->super )
		dump_tiny( compile->super );
	else
		printf( "(null)" );
	printf( "\n" );
	printf( "%s->compile->children = ", IOBJECT( sym )->name );
	(void) slist_map( compile->children, (SListMapFn) dump_tiny, NULL );
	printf( "\n" );

	buf_init_static( &buf, str, 80 );
	graph_element( compile->heap, &buf, &compile->base, FALSE );
	printf( "%s->compile->base = %s\n", 
		IOBJECT( sym )->name, buf_all( &buf ) );
	if( compile->heap )
		iobject_dump( IOBJECT( compile->heap ) );
}

/* Print a full symbol and all it's children. 
 */
void *
dump_symbol( Symbol *sym )
{	
	printf( "\n\nsym->name = " );
	(void) dump_tiny( sym );
	printf( "\n" );

	printf( "%s->patch = %d pointers\n", IOBJECT( sym )->name,
		g_slist_length( sym->patch ) );

	if( sym->expr ) 
		dump_expr( sym->expr );
	else
		printf( "%s->expr = <no expr info>\n", IOBJECT( sym )->name );

	printf( "%s->base = ", IOBJECT( sym )->name );
	if( !sym->dirty ) {
		PElement root;

		PEPOINTE( &root, &sym->base );
		pgraph( &root );
	}
	else
		printf( "<sym is dirty ... can't print>" );
	printf( "\n" );

	printf( "%s->dirty = %s\n", 
		IOBJECT( sym )->name, bool_to_char( sym->dirty ) );
	printf( "%s->parents = ", IOBJECT( sym )->name );
	(void) slist_map( sym->parents, (SListMapFn) dump_compile_tiny, NULL );
	printf( "\n" );

	/* Prints topchildren and topparents.
	 */
	dump_links( sym );
	printf( "%s->ndirtychildren = %d\n", 
		IOBJECT( sym )->name, sym->ndirtychildren );
	printf( "%s->leaf = %s\n", 
		IOBJECT( sym )->name, bool_to_char( sym->leaf ) );

	printf( "%s->tool = kit ", IOBJECT( sym )->name );
	if( sym->tool )
		dump_kit( sym->tool->kit );
	else
		printf( "<NULL>\n" );

	return( NULL );
}

/* Pretty print the whole of the symbol table.
 */
void
dump_symbol_table( void )
{	
	(void) icontainer_map( ICONTAINER( symbol_root->expr->compile ), 
		(icontainer_map_fn) dump_symbol, NULL, NULL );
}

/* Tiny dump a tool.
 */
static void *
dump_tiny_tool( Tool *tool )
{
	switch( tool->type ) {
	case TOOL_SEP:
		printf( "<separator> " );
		break;

	case TOOL_DIA:
		printf( "<dialog \"%s\"> ", FILEMODEL( tool )->filename );
		break;
	
	case TOOL_SYM:
		dump_tiny( tool->sym );
		break;

	default:
		assert( FALSE );
	}

	return( NULL );
}

/* Print out the syms in a kit.
 */
void *
dump_kit( Toolkit *kit )
{
	printf( "kit->name = %s; ", IOBJECT( kit )->name );
	printf( "%s->tools = ", IOBJECT( kit )->name );
	icontainer_map( ICONTAINER( kit ), 
		(icontainer_map_fn) dump_tiny_tool, NULL, NULL );
	printf( "\n" );

	return( NULL );
}

/* Easy find-a-symbol for gdb.
 */
Symbol *
sym( char *name )
{	
	return( compile_lookup( symbol_root->expr->compile, name ) );
}

/* Print from a name.
 */
void 
psym( char *name )
{	
	Symbol *s;

	if( (s = sym( name )) )
		(void) dump_symbol( s );
	else
		printf( "Symbol \"%s\" not found\n", name );
}

/* Print scrap of graph.
 */
void 
pgraph( PElement *graph )
{	
	BufInfo buf;
	char txt[10240];

	buf_init_static( &buf, txt, 10240 );
	graph_pelement( reduce_context->heap, &buf, graph, TRUE );
	printf( "%s\n", buf_all( &buf ) );
}

/* Print symbol value from name.
 */
void 
psymv( char *name )
{	
	BufInfo buf;
	char txt[1024];
	Symbol *s = sym( name );

	if( s ) {
		buf_init_static( &buf, txt, 1024 );
		graph_pelement( reduce_context->heap, 
			&buf, &s->expr->root, TRUE );
		printf( "%s = %s\n", name, buf_all( &buf ) );
	}
}

/* Pretty-print an element.
 */
static void
print_element( int nsp, EType type, void *arg )
{
	switch( type ) {
	case ELEMENT_NOVAL:
		printf( "no value (%d)\n", GPOINTER_TO_INT( arg ) );
		break;

	case ELEMENT_NODE:
		printf( "node ->\n" );
		graph_heap( nsp + 1, arg );
		break;

	case ELEMENT_SYMBOL:
		printf( "symbol \"%s\"", IOBJECT( arg )->name );
		break;

	case ELEMENT_SYMREF:
		printf( "symref \"%s\"", IOBJECT( arg )->name );
		break;

	case ELEMENT_COMPILEREF:
		printf( "compileref " );
		compile_name_print( COMPILE( arg ) );
		break;

	case ELEMENT_CONSTRUCTOR:
		printf( "constructor \"%s\"", IOBJECT( arg )->name );
		break;

	case ELEMENT_CHAR:
		printf( "char \"%c\"", GPOINTER_TO_UINT( arg ) );
		break;

	case ELEMENT_BOOL:
		printf( "bool \"%s\"", bool_to_char(
			(gboolean) GPOINTER_TO_UINT( arg ) ) );
		break;

	case ELEMENT_BINOP:
		printf( "binop \"%s\"", decode_BinOp( (BinOp)arg ) );
		break;

	case ELEMENT_UNOP:
		printf( "unop \"%s\"", decode_UnOp( (UnOp)arg ) );
		break;

	case ELEMENT_COMB:
		printf( "combinator \"%s\"", 
			decode_CombinatorType( (CombinatorType)arg ) );
		break;

	case ELEMENT_TAG:
		printf( "tag \"%s\"", (char*)arg ); 
		break;

	case ELEMENT_STATIC:
		printf( "static string \"%s\"", 
			((HeapStaticString*)arg)->text ); 
		break;

	case ELEMENT_MANAGED:
		printf( "Managed* %p", arg ); 
		break;

	case ELEMENT_ELIST:
		printf( "empty-list []" ); 
		break;

	default:
		assert( FALSE );
	}
}

/* Pretty-print a heap graph.
 */
void 
graph_heap( int nsp, HeapNode *hn )
{
	if( !hn )
		return;

	printf( "%s", spc( nsp ) );
	printf( "Node: " );

	printf( "serial = %d, ", hn->flgs & FLAG_SERIAL );

	if( hn->flgs & FLAG_PRINT )
		printf( "print, " );
	else
		printf( "noprint, " );

	if( hn->flgs & FLAG_DEBUG )
		printf( "debug, " );
	else
		printf( "nodebug, " );

	if( hn->flgs & FLAG_MARK )
		printf( "marked, " );
	else
		printf( "notmarked, " );

	printf( "%s ", decode_NodeType( hn->type ) );

	switch( hn->type ) {
	case TAG_APPL:
	case TAG_CONS:
		printf( "\n" );
		printf( "%s", spc( nsp ) );
		printf( "left:  " );
		print_element( nsp, hn->ltype, hn->body.ptrs.left );

		printf( "\n" );
		printf( "%s", spc( nsp ) );
		printf( "right: " );
		print_element( nsp, hn->rtype, hn->body.ptrs.right );

		printf( "\n" );

		break;

	case TAG_DOUBLE:
		printf( "real \"%g\"\n", hn->body.num );
		break;

	case TAG_CLASS:
		printf( "instance-of-class \"%s\"\n",
			IOBJECT( hn->body.ptrs.left )->name );
		printf( " secrets=(" );
		print_element( nsp, 
			GETRIGHT( hn )->ltype, 
			GETRIGHT( hn )->body.ptrs.left );
		printf( ") members=(" );
		print_element( nsp, 
			GETRIGHT( hn )->rtype, 
			GETRIGHT( hn )->body.ptrs.right );
		printf( ")\n" );
		break;

	case TAG_COMPLEX:
		printf( "complex \"(%g,%g)\"\n", 
			GETLEFT( hn )->body.num, GETRIGHT( hn )->body.num );
		break;

	case TAG_GEN:
		printf( "list generator start=%g next=%g final=%g\n", 
			GETLEFT( hn )->body.num, 
			GETLEFT( GETRIGHT( hn ) )->body.num, 
			GETRIGHT( GETRIGHT( hn ) )->body.num );
		break;

	case TAG_FILE:
		printf( "list generator file=%s\n", 
			MANAGEDFILE( GETLEFT( hn ) )->file->fname );
		break;

	case TAG_FREE:
	default:
		assert( FALSE );
	}
}

/* Pretty-print a const.
 */
static void
dump_parseconst( ParseConst *pc )
{
	switch( pc->type ) {
	case PARSE_CONST_NUM:
		printf( "%G", pc->val.num );
		break;

	case PARSE_CONST_COMPLEX:
		printf( "%Gj", pc->val.num );

	case PARSE_CONST_STR:
		printf( "\"%s\"", pc->val.str );
		break;

	case PARSE_CONST_BOOL:
		printf( "%s", bool_to_char( pc->val.bool ) );
		break;

	case PARSE_CONST_CHAR:
		printf( "'%c'", pc->val.ch );
		break;

	case PARSE_CONST_ELIST:
		printf( "[]" );
		break;

	default:
		assert( FALSE );
	}
}

/* Dump a parse tree. 
 */
void *
dump_tree( ParseNode *n )
{
	switch( n->type ) {
	case NODE_NONE:
		printf( "node->type == NODE_NONE\n" );
		break;

	case NODE_APPLY:
		printf( "Function application\n" );
		printf( "LHS = " );
		(void) dump_tree( n->arg1 );
		printf( "RHS = " );
		(void) dump_tree( n->arg2 );
		break;

	case NODE_CLASS:
		printf( "Class: " );
		(void) dump_compile_tiny( n->klass );
		printf( "\n" );
		break;

	case NODE_LEAF:
		printf( "Leaf symbol: " );
		(void) dump_tiny( n->leaf );
		printf( "\n" );
		break;

	case NODE_TAG:
		printf( "Tag: %s\n", n->tag );
		break;

	case NODE_BINOP:
		printf( "Binary operator %s\n", decode_BinOp( n->biop ) );
		printf( "Left expression:\n" );
		(void) dump_tree( n->arg1 );
		printf( "Right expression:\n" );
		(void) dump_tree( n->arg2 );
		break;

	case NODE_UOP:
		printf( "Unary operator %s\n", decode_UnOp( n->uop ) );
		printf( "Arg expression:\n" );
		(void) dump_tree( n->arg1 );
		break;

	case NODE_CONST:
		printf( "Constant " );
		dump_parseconst( &n->con );
		printf( "\n" );
		break;

	case NODE_GENERATOR:
		printf( "List generator\n" );
		printf( "Start:\n" );
		(void) dump_tree( n->arg1 );
		if( n->arg2 ) {
			printf( "Next:\n" );
			(void) dump_tree( n->arg2 );
		}
		if( n->arg3 ) {
			printf( "End:\n" );
			(void) dump_tree( n->arg3 );
		}
		break;

	case NODE_COMPOSE:
		printf( "Function compose\n" );
		printf( "Left:\n" );
		(void) dump_tree( n->arg1 );
		printf( "Right:\n" );
		(void) dump_tree( n->arg2 );
		break;

	case NODE_LISTCONST:
	case NODE_SUPER:
		if( n->type == NODE_LISTCONST )
			printf( "List constant\n" );
		else
			printf( "Superclass construct\n" );

		printf( "***[\n" );
		slist_map_rev( n->elist, (SListMapFn) dump_tree, NULL );
		printf( "***]\n" );
		break;

	default:
		assert( FALSE );
	}

	return( NULL );
}

static void *
dump_link_expr( LinkExpr *le )
{
	dump_expr_tiny( le->expr );
	printf( " count = %d ; ", le->count );

	return( NULL );
}

void *
dump_link( Link *link )
{
	printf( "link->parent = " );
	symbol_name_print( link->parent );
	printf( "\n" );

	printf( "link->child = " );
	symbol_name_print( link->child );
	printf( "\n" );

	printf( "link->serial = %d\n", link->serial );

	printf( "link->static_links = " );
	slist_map( link->static_links, (SListMapFn) dump_link_expr, NULL );
	printf( "\n" );
	printf( "link->dynamic_links = " );
	slist_map( link->dynamic_links, (SListMapFn) dump_link_expr, NULL );
	printf( "\n" );

	return( NULL );
}

void
dump_links( Symbol *sym )
{
	symbol_name_print( sym );
	printf( "->topchildren = \n" );
	slist_map( sym->topchildren, (SListMapFn) dump_link, NULL );

	symbol_name_print( sym );
	printf( "->topparents = \n" );
	slist_map( sym->topparents, (SListMapFn) dump_link, NULL );
}

void 
dump_symbol_heap( Symbol *sym )
{
	printf( "symbol " );
	symbol_name_print( sym );
	printf( " has graph:\n" );
	if( sym->expr ) 
		pgraph( &sym->expr->root );
	printf( "\n" );
}

#endif /*DEBUG*/
