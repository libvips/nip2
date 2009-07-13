/* Prettyprint heap structures for debugging. Also printed by trace.c
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

/* Indent step.
 */
#define TAB (2)

/* Fwd ref.
 */
static void lisp_pelement( VipsBuf *buf, PElement *base, 
	GSList **back, gboolean fn, int indent );

/* Print a sym-value list.
 */
static void
lisp_symval( VipsBuf *buf, PElement *base, 
	GSList **back, gboolean fn, int indent, PElement *stop )
{
	gboolean error = FALSE;

	/* Reached the "stop" element?
	 */
	if( stop && *base->type == *stop->type && *base->ele == *stop->ele )
		return;

	if( PEISNODE( base ) ) {
		HeapNode *hn = PEGETVAL( base );
		PElement pe;

		if( hn->type != TAG_CONS )
			error = TRUE;

		PEPOINTLEFT( hn, &pe );
		if( !error && PEISNODE( &pe ) ) {
			HeapNode *hn2 = PEGETVAL( &pe );

			if( hn2->type != TAG_CONS )
				error = TRUE;

			PEPOINTLEFT( hn2, &pe );
			if( !error && PEISSYMREF( &pe ) ) {
				vips_buf_appendf( buf, "\n%s", spc( indent ) );
				symbol_qualified_name( 
					PEGETSYMREF( &pe ), buf );
				vips_buf_appendf( buf, " = " );

				PEPOINTRIGHT( hn2, &pe );
				lisp_pelement( buf, &pe, 
					back, fn, indent + TAB );

				PEPOINTRIGHT( hn, &pe );
				lisp_symval( buf, &pe, back, fn, indent, stop );
			}
			else
				error = TRUE;
		}
		else
			error = TRUE;
	}
	else if( !PEISELIST( base ) ) 
		error = TRUE;

	if( error )
		vips_buf_appendf( buf, "\n%s<*** malformed symval list>", 
			spc( indent ) );
}

/* Print a [*] ... our caller has printed the enclosing [ ] and the first
 * element, so we print a ", " followed by us.
 */
static void
lisp_list( VipsBuf *buf, PElement *base, 
	GSList **back, gboolean fn, int indent )
{
	if( PEISNODE( base ) ) {
		HeapNode *hn = PEGETVAL( base );
		PElement pe;

		vips_buf_appends( buf, ", " );

		if( hn->type == TAG_CONS ) {
			PEPOINTLEFT( hn, &pe );
			lisp_pelement( buf, &pe, back, fn, indent );

			PEPOINTRIGHT( hn, &pe );
			lisp_list( buf, &pe, back, fn, indent );
		}
		else
			lisp_pelement( buf, base, back, fn, indent );
	}
	else if( !PEISELIST( base ) ) 
		lisp_pelement( buf, base, back, fn, indent );
}

/* Print a [char] ... fall back to lisp_list() if we hit a non-char
 * element.
 */
static void
lisp_string( VipsBuf *buf, PElement *base, 
	GSList **back, gboolean fn, int indent )
{
	gboolean error = FALSE;

	if( PEISNODE( base ) ) {
		HeapNode *hn = PEGETVAL( base );
		PElement pe;

		if( hn->type != TAG_CONS )
			error = TRUE;

		PEPOINTLEFT( hn, &pe );
		if( !error ) {
			if( PEISCHAR( &pe ) ) {
				vips_buf_appendf( buf, "%c", PEGETCHAR( &pe ) );

				PEPOINTRIGHT( hn, &pe );
				lisp_string( buf, &pe, back, fn, indent );
			}
			else {
				vips_buf_appends( buf, "\":[" );
				lisp_pelement( buf, &pe, back, fn, indent );

				PEPOINTRIGHT( hn, &pe );
				lisp_list( buf, &pe, back, fn, indent );
				vips_buf_appends( buf, "]" );

				error = TRUE;
			}
		}
		else
			error = TRUE;
	}
	else if( !PEISELIST( base ) ) 
		error = TRUE;
}

/* Print a graph LISP-style.
 */
static void
lisp_node( VipsBuf *buf, HeapNode *hn, GSList **back, gboolean fn, int indent )
{
	int i;
	PElement p1, p2;

	/* Have we printed this node before?
	 */
	if( hn->flgs & FLAG_PRINT ) {
		if( (i = g_slist_index( *back, hn )) == -1 ) {
			*back = g_slist_prepend( *back, hn );
			vips_buf_appendf( buf, "<" ); 
			vips_buf_appendf( buf, _( "circular" ) ); 
			vips_buf_appendf( buf, " (%p)>", hn );
		}
		else {
			vips_buf_appendf( buf, "<" );
			vips_buf_appendf( buf, _( "circular to label %d" ), i );
			vips_buf_appendf( buf, ">" );
		}

		return;
	}
	hn->flgs |= FLAG_PRINT;

	if( (i = g_slist_index( *back, hn )) != -1 ) {
		vips_buf_appendf( buf, "*" );
		vips_buf_appendf( buf, _( "label %d" ), i );
		vips_buf_appendf( buf, ": " );
	}

	switch( hn->type ) {
	case TAG_APPL:
		if( fn ) {
			PEPOINTLEFT( hn, &p1 );
			PEPOINTRIGHT( hn, &p2 );
			vips_buf_appends( buf, "(" );
			lisp_pelement( buf, &p1, back, fn, indent );
			vips_buf_appends( buf, " " );
			lisp_pelement( buf, &p2, back, fn, indent );
			vips_buf_appends( buf, ")" );
		}
		else {
			vips_buf_appends( buf, "<" );
			vips_buf_appends( buf, _( "unevaluated" ) );
			vips_buf_appends( buf, ">" );
		}

		break;

	case TAG_CONS:
		PEPOINTLEFT( hn, &p1 );
		if( PEISCHAR( &p1 ) ) {
			vips_buf_appendf( buf, "\"%c", PEGETCHAR( &p1 ) );
			PEPOINTRIGHT( hn, &p2 );
			lisp_string( buf, &p2, back, fn, indent );
			vips_buf_appends( buf, "\"" );
		}
		else {
			vips_buf_appends( buf, "[" );
			lisp_pelement( buf, &p1, back, fn, indent );
			PEPOINTRIGHT( hn, &p2 );
			lisp_list( buf, &p2, back, fn, indent );
			vips_buf_appends( buf, "]" );
		}
		break;

	case TAG_DOUBLE:
		vips_buf_appendf( buf, "%g", hn->body.num );
		break;

	case TAG_COMPLEX:
		vips_buf_appendf( buf, "(%g,%g)", 
			GETLEFT( hn )->body.num, GETRIGHT( hn )->body.num );
		break;

	case TAG_CLASS:
		if( fn ) {
			vips_buf_appendf( buf, "\n%s", spc( indent ) );
			vips_buf_appendf( buf, _( "class (%p)" ), hn );
			vips_buf_appendf( buf, " " );
		}

		PEPOINTLEFT( hn, &p1 );
		lisp_pelement( buf, &p1, back, fn, indent );

		if( fn ) {
			hn = GETRIGHT( hn );

			vips_buf_appendf( buf, "\n%s", spc( indent + TAB ) ); 
			vips_buf_appendf( buf, _( "members" ) );
			vips_buf_appendf( buf, " = { " );
			PEPOINTRIGHT( hn, &p1 );
			lisp_symval( buf, &p1, 
				back, fn, indent + TAB * 2, NULL );
			vips_buf_appendf( buf, "\n%s}", spc( indent + TAB ) ); 

			PEPOINTLEFT( hn, &p2 );
			if( *p1.type != *p2.type || *p1.ele != *p2.ele ) { 
				vips_buf_appendf( buf, "\n%s", spc( indent + TAB ) );
				vips_buf_appendf( buf, _( "secret" ) );
				vips_buf_appendf( buf, " = { " );
				lisp_symval( buf, &p2, 
					back, fn, indent + TAB * 2, &p1 );
				vips_buf_appendf( buf, 
					"\n%s} ", spc( indent + TAB ) ); 
			}
		}

		break;

	case TAG_GEN:
		vips_buf_appendf( buf, "[%g,%g...", 
			GETLEFT( hn )->body.num, 
			GETLEFT( GETRIGHT( hn ) )->body.num ); 
		if( GETRT( GETRIGHT( hn ) ) == ELEMENT_ELIST )
			vips_buf_appends( buf, "[ ]]" );
		else
			vips_buf_appendf( buf, "%g]",
				GETRIGHT( GETRIGHT( hn ) )->body.num ); 
		break;

	case TAG_SHARED:
		PEPOINTLEFT( hn, &p1 );
		i = GPOINTER_TO_INT( GETRIGHT( hn ) );
		vips_buf_appendf( buf, "SHARE%d[", i );
		lisp_pelement( buf, &p1, back, fn, indent );
		vips_buf_appends( buf, "]" );
		break;

	case TAG_REFERENCE:
		i = GPOINTER_TO_INT( GETRIGHT( GETLEFT( hn ) ) );
		vips_buf_appendf( buf, "REF%d", i );
		break;

	case TAG_FREE:
	default:
		g_assert( FALSE );
	}
}

/* Print a pelement LISP-style.
 */
static void
lisp_pelement( VipsBuf *buf, PElement *base, 
	GSList **back, gboolean fn, int indent )
{
	HeapNode *hn;
	EType type = PEGETTYPE( base );

	switch( type ) {
	case ELEMENT_NOVAL:
		vips_buf_appends( buf, "<" );
		vips_buf_appendf( buf, _( "no value (type %d)" ), 
			GPOINTER_TO_INT( PEGETVAL( base ) ) );
		vips_buf_appends( buf, ">" );
		break;

	case ELEMENT_NODE:
		if( !(hn = PEGETVAL( base )) ) {
			vips_buf_appends( buf, "<" );
			vips_buf_appends( buf, _( "NULL pointer" ) );
			vips_buf_appends( buf, ">" );
		}
		else
			lisp_node( buf, hn, back, fn, indent );
		break;

	case ELEMENT_SYMBOL:
		vips_buf_appends( buf, "<" ); 
		vips_buf_appends( buf, _( "symbol" ) ); 
		vips_buf_appends( buf, " \"" ); 
		symbol_qualified_name( PEGETSYMBOL( base ), buf );
		vips_buf_appends( buf, "\">" ); 
		break;

	case ELEMENT_CONSTRUCTOR:
		vips_buf_appends( buf, "<" ); 
		vips_buf_appends( buf, _( "constructor" ) ); 
		vips_buf_appends( buf, " \"" ); 
		symbol_qualified_name( PEGETCOMPILE( base )->sym, buf );
		vips_buf_appends( buf, "\">" ); 
		break;

	case ELEMENT_SYMREF:
		vips_buf_appends( buf, "<" ); 
		vips_buf_appends( buf, _( "symref" ) ); 
		vips_buf_appends( buf, " \"" ); 
		symbol_qualified_name( PEGETSYMBOL( base ), buf );
		vips_buf_appends( buf, "\">" ); 
		break;

	case ELEMENT_COMPILEREF:
		vips_buf_appends( buf, "<" ); 
		vips_buf_appends( buf, _( "compileref" ) ); 
		vips_buf_appends( buf, " \"" ); 
		symbol_qualified_name( PEGETCOMPILE( base )->sym, buf );
		vips_buf_appends( buf, "\">" ); 
		break;

	case ELEMENT_CHAR:
		vips_buf_appendf( buf, "'%c'", (int) PEGETCHAR( base ) );
		break;

	case ELEMENT_BOOL:
		vips_buf_appends( buf, bool_to_char( PEGETBOOL( base ) ) );
		break;

	case ELEMENT_BINOP:
		vips_buf_appends( buf, decode_BinOp( PEGETBINOP( base ) ) );
		break;

	case ELEMENT_UNOP:
		vips_buf_appends( buf, decode_UnOp( PEGETUNOP( base ) ) );
		break;

	case ELEMENT_ELIST:
		vips_buf_appends( buf, "[ ]" ); 
		break;

	case ELEMENT_TAG:
		vips_buf_appendf( buf, "<" );
		vips_buf_appendf( buf, _( "tag \"%s\"" ), PEGETTAG( base ) );
		vips_buf_appendf( buf, ">" );
		break;

	case ELEMENT_MANAGED:
		vips_buf_appendf( buf, "<Managed* %p>", PEGETVAL( base ) );
		break;

	case ELEMENT_COMB:
		vips_buf_appends( buf, decode_CombinatorType( PEGETCOMB( base ) ) );
		break;

	default:
		vips_buf_appendf( buf, "<" );
		vips_buf_appendf( buf, _( "unknown element tag %d" ), type );
		vips_buf_appendf( buf, ">" );
		break;
	}
}

/* Print a node to a buffer. If fn is true, trace into functions.
 */
void
graph_node( Heap *heap, VipsBuf *buf, HeapNode *root, gboolean fn )
{
	GSList *back;
	char txt[4];
	VipsBuf buf2 = VIPS_BUF_STATIC( txt );

	/* May be called before heap is built.
	 */
	if( !heap )
		return;

	back = NULL;
	heap_clear( heap, FLAG_PRINT );
	lisp_node( &buf2, root, &back, fn, 0 );
	heap_clear( heap, FLAG_PRINT );
	lisp_node( buf, root, &back, fn, 0 );
	IM_FREEF( g_slist_free, back );
}

/* As above, but start from a pelement.
 */
void
graph_pelement( Heap *heap, VipsBuf *buf, PElement *root, gboolean fn )
{
	GSList *back;
	char txt[4];
	VipsBuf buf2 = VIPS_BUF_STATIC( txt );

	/* May be called before heap is built.
	 */
	if( !heap )
		return;

	/* We print twice ... the first time through we build the list of back
	 * pointers so we can label the graph correctly.
	 */
	back = NULL;

	heap_clear( heap, FLAG_PRINT );
	lisp_pelement( &buf2, root, &back, fn, 0 );

	heap_clear( heap, FLAG_PRINT );
	lisp_pelement( buf, root, &back, fn, 0 );

	IM_FREEF( g_slist_free, back );
}

/* As above, but start from an element.
 */
void
graph_element( Heap *heap, VipsBuf *buf, Element *root, gboolean fn )
{
	PElement base;

	PEPOINTE( &base, root );
	graph_pelement( heap, buf, &base, fn );
}

void
graph_pointer( PElement *root )
{
	char txt[1000];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_pelement( reduce_context->heap, &buf, root, TRUE );
	printf( "%s\n", vips_buf_all( &buf ) );
}

/* Fwd ref.
 */
static void shell_pelement( PElement *base );

/* Print a graph shell-style.
 */
static void
shell_node( HeapNode *hn )
{
	PElement p1, p2;

	/* Have we printed this node before?
	 */
	if( hn->flgs & FLAG_PRINT ) {
		printf( "<*circular*>" );
		return;
	}
	hn->flgs |= FLAG_PRINT;

	switch( hn->type ) {
	case TAG_CLASS:
	case TAG_APPL:
	case TAG_REFERENCE:
	case TAG_SHARED:
	case TAG_GEN:
		break;
		
	case TAG_CONS:
{
		gboolean string_mode;

		PEPOINTLEFT( hn, &p1 );
		string_mode = PEISCHAR( &p1 );

		for(;;) {
			if( string_mode )
				printf( "%c", PEGETCHAR( &p1 ) );
			else
				shell_pelement( &p1 );

			PEPOINTRIGHT( hn, &p2 );
			if( PEISELIST( &p2 ) ) 
				break;

			if( !string_mode )
				printf( "\n" );
			hn = PEGETVAL( &p2 );
			PEPOINTLEFT( hn, &p1 );
			if( string_mode && !PEISCHAR( &p1 ) )
				string_mode = FALSE;
		}
}
		break;

	case TAG_DOUBLE:
		printf( "%g", hn->body.num );
		break;

	case TAG_COMPLEX:
		printf( "%g %g", 
			GETLEFT( hn )->body.num, GETRIGHT( hn )->body.num );
		break;

	case TAG_FREE:
	default:
		g_assert( FALSE );
	}
}

/* Print a pelement shell-style.
 */
static void
shell_pelement( PElement *base )
{
	switch( PEGETTYPE( base ) ) {
	/* Only allow concrete base types.
	 */
	case ELEMENT_SYMREF:
	case ELEMENT_COMPILEREF:
	case ELEMENT_CONSTRUCTOR:
	case ELEMENT_BINOP:
	case ELEMENT_UNOP:
	case ELEMENT_COMB:
	case ELEMENT_TAG:
	case ELEMENT_SYMBOL:
	case ELEMENT_NOVAL:
		printf( "no-value" );
		break;

	case ELEMENT_NODE:
		shell_node( PEGETVAL( base ) );
		break;

	case ELEMENT_CHAR:
		printf( "%c", (int)PEGETCHAR( base ) );
		break;

	case ELEMENT_BOOL:
		printf( "%s", bool_to_char( PEGETBOOL( base ) ) );
		break;

	case ELEMENT_ELIST:
		printf( "[ ]" ); 
		break;

	case ELEMENT_MANAGED:
		if( PEISIMAGE( base ) )
			printf( "%s", PEGETIMAGE( base )->filename );
		break;

	default:
		g_assert( FALSE );
	}
}

/* Print a pelement shell-style.
 */
void
graph_value( PElement *root )
{
	Reduce *rc = reduce_context;

	if( !reduce_pelement( rc, reduce_spine_strict, root ) ) {
		box_alert( NULL );
		return;
	}

	heap_clear( reduce_context->heap, FLAG_PRINT );
	shell_pelement( root );
	printf( "\n" );
}


