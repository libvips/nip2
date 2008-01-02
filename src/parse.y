%{

/* Parse ip's macro language.
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

/*
#define DEBUG
 */

/* trace text read system
#define DEBUG_CHARACTER
 */

#define YYERROR_VERBOSE

/* Last top-level symbol we made.
 */
Symbol *last_top_sym;

/* Global .. the symbol whose definition we are currently parsing, the symbol
 * which all top defs in this parse action should be made local to,
 * and the return point if we find a syntax error.
 */
static Symbol *current_symbol;
static Symbol *root_symbol;
jmp_buf parse_error_point;

/* The current parse context.
 */
static Compile *current_compile = NULL;
static ParseNode *current_parsenode = NULL;

/* A list of the top-level symbols we've defined so far in this parse action. 
 * We don't allow a single parse action to define a symbol more than once.
 */
static GSList *parse_sofar = NULL;

/* The kit we are adding new symbols to.
 */
static Toolkit *current_kit;

/* Where it should go in the kit.
 */
static int tool_position;

/* Lineno of start of last top-level def.
 */
static int last_top_lineno;

/* Text we've gathered in this lex.
 */
BufInfo lex_text;
static char lex_text_buffer[MAX_STRSIZE];

/* Stack of symbols for parser - each represents a new scope level.
 */
static Symbol *scope_stack_symbol[MAX_SSTACK];
static Compile *scope_stack_compile[MAX_SSTACK];
static int scope_sp = 0;

/* Here for errors in parse. Can be called by some of the tree builders.
 */
/*VARARGS1*/
void
yyerror( const char *sub, ... )
{
	va_list ap;
 	char buf[4096];

        va_start( ap, sub );
        (void) im_vsnprintf( buf, 4096, sub, ap );
        va_end( ap );

	error_top( _( "Parse error." ) );

	if( last_top_sym ) 
		error_sub( _( "Error in %s: %s" ), 
			IOBJECT( last_top_sym )->name, buf );
	else
		error_sub( _( "Error: %s" ), buf );

	longjmp( parse_error_point, -1 );
}

InputState input_state;

/* Attach yyinput to a file.
 */
void
attach_input_file( iOpenFile *of )
{
	InputState *is = &input_state;

#ifdef DEBUG
	printf( "attach_input_file: \"%s\"\n", of->fname );
#endif /*DEBUG*/

	/* Need to clear flex/bison's buffers in case we abandoned the
	 * previous parse. 
	 */
	yyrestart( NULL );

	is->of = of;
	is->str = NULL;
	is->strpos = NULL;
	is->bwp = 0;
	is->bspsp = 0;
	is->bsp[is->bspsp] = 0;
	is->lineno = 1;
	is->charno = 0;
	is->pcharno = 0;
	is->charpos = 0;
	is->oldchar = -1;

	/* Init text gatherer.
	 */
	buf_init_static( &lex_text, lex_text_buffer, MAX_STRSIZE );
}

/* Attach yyinput to a string.
 */
void
attach_input_string( const char *str )
{
	InputState *is = &input_state;

#ifdef DEBUG
	printf( "attach_input_string: \"%s\"\n", str );
#endif /*DEBUG*/

	yyrestart( NULL );

	is->of = NULL;
	is->str = (char *) str;
	is->strpos = (char *) str;
	is->bwp = 0;
	is->bspsp = 0;
	is->bsp[is->bspsp] = 0;
	is->lineno = 1;
	is->charno = 0;
	is->pcharno = 0;
	is->charpos = 0;
	is->oldchar = -1;

	/* Init text gatherer.
	 */
	buf_init_static( &lex_text, lex_text_buffer, MAX_STRSIZE );
}

/* Read a character from the input.
 */
int
ip_input( void ) 
{
	InputState *is = &input_state;
	int ch;

	if( is->oldchar >= 0 ) {
		/* From unget buffer.
		 */
		ch = is->oldchar;
		is->oldchar = -1;
	}
	else if( is->of ) {
		/* Input from file. 
		 */
		if( (ch = getc( is->of->fp )) == EOF )
			return( 0 );
	}
	else {
		/* Input from string. 
		 */
		if( (ch = *is->strpos) )
			is->strpos++;
		else
			/* No counts to update!
			 */
			return( 0 );
	}

	/* Update counts.
	 */
	if( ch == '\n' ) {
		is->lineno++;
		is->pcharno = is->charno + 1;
		is->charno = 0;
	}
	is->charno++;
	is->charpos++;

	/* Add this character to the characters we have accumulated for this
	 * definition.
	 */
	if( is->bwp >= MAX_STRSIZE )
		yyerror( _( "definition is too long!" ) );
	if( is->bwp >= 0 )
		is->buf[is->bwp] = ch;
	is->bwp++;

	/* Add to lex text buffer.
	 */
	if( is->charno > 0 )
		buf_appendc( &lex_text, ch );

#ifdef DEBUG_CHARACTER
	printf( "ip_input: returning '%c'\n", ch ); 
#endif /*DEBUG_CHARACTER*/

	return( ch );
}

/* Unget an input character.
 */
void
ip_unput( int ch )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "ip_unput: ungetting '%c'\n", ch ); 
#endif /*DEBUG_CHARACTER*/

	/* Is lex trying to unget the end-of-file marker? Do nothing if it is.
	 */
	if( !ch )
		return;

	if( is->of ) {
		if( ungetc( ch, is->of->fp ) == EOF )
			error( "unget buffer overflow" );
	}
	else 
		/* Save extra char here.
		 */
		is->oldchar = ch;

	/* Redo counts.
	 */
	if( ch == '\n' ) {
		is->lineno--;

		/* Restore previous charno.
		 */
		is->charno = is->pcharno;
		is->pcharno = 0;
	}
	is->charno--;
	is->charpos--;
	is->bwp--;

	/* Unget from lex text buffer.
	 */
	if( is->charno > 0 )
		buf_removec( &lex_text, ch );
}

/* Test for end-of-input.
 */
gboolean
is_EOF( void )
{
	InputState *is = &input_state;

	if( is->of )
		return( feof( is->of->fp ) );
	else
		return( *is->str == '\0' );
}

static void
scope_push( void )
{
	if( scope_sp == MAX_SSTACK )
		error( "sstack overflow" );

	scope_stack_symbol[scope_sp] = current_symbol;
	scope_stack_compile[scope_sp] = current_compile;
	scope_sp += 1;
}

static void
scope_pop( void )
{
	if( scope_sp <= 0 )
		error( "sstack underflow" );

	scope_sp -= 1;
	current_symbol = scope_stack_symbol[scope_sp];
	current_compile = scope_stack_compile[scope_sp];
}

/* Back to the outermost scope.
 */
static void
scope_pop_all( void )
{
	if( scope_sp > 0 ) {
		scope_sp = 0;
		current_symbol = scope_stack_symbol[scope_sp];
		current_compile = scope_stack_compile[scope_sp];
	}
}

/* Reset/push/pop parser stacks. 
 */
static void
scope_reset( void )
{
	scope_sp = 0;
}

/* Return the text we have accumulated for the current definition. Remove
 * leading and trailing whitespace and spare semicolons. out needs to be
 * MAX_STRSIZE.
 */
static char *
input_text( char *out )
{
	InputState *is = &input_state;
	const char *buf = is->buf;

	int start = is->bsp[is->bspsp];
	int end = is->bwp;
	int len;
	int i;

	for( i = start; i < end && 
		(isspace( buf[i] ) || buf[i] == ';'); i++ )
		;
	start = i;
	for( i = end - 1; i > start && 
		(isspace( buf[i] ) || buf[i] == ';'); i-- )
		;
	end = i + 1;

	len = end - start;

	assert( len < MAX_STRSIZE - 1 );
	im_strncpy( out, buf + start, len + 1 );
	out[len] = '\0';

#ifdef DEBUG_CHARACTER
	printf( "input_text: level %d, returning \"%s\"\n", 
		is->bspsp, out );
#endif /*DEBUG_CHARACTER*/

	return( out );
}

/* Reset/push/pop input stacks.
 */
static void
input_reset( void )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "input_reset:\n" );
#endif /*DEBUG_CHARACTER*/

	is->bwp = 0;
	is->bspsp = 0;
	is->bsp[0] = 0;
	buf_init_static( &lex_text, lex_text_buffer, MAX_STRSIZE );
}

static void
input_push( int n )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "input_push(%d): going to level %d, %d bytes into buffer\n", 
		n, is->bspsp + 1, is->bwp );

	{
		const int len = IM_MIN( is->bwp, 20 );
		int i;

		for( i = is->bwp - len; i < is->bwp; i++ )
			if( is->buf[i] == '\n' )
				printf( "@" );
			else if( is->buf[i] == ' ' || is->buf[i] == '\t' )
				printf( "_" );
			else
				printf( "%c", is->buf[i] );
		printf( "\n" );
		for( i = 0; i < len; i++ )
			printf( "-" );
		printf( "^\n" );
	}
#endif /*DEBUG_CHARACTER*/

	is->bspsp += 1;
	if( is->bspsp >= MAX_SSTACK )
		error( "bstack overflow" );

	is->bsp[is->bspsp] = is->bwp;
}

/* Yuk! We've just done an input_push() to try to grab the RHS of a 
 * definition ... unfortunately, due to token readahead, we've probably 
 * already read the start of the RHS.
 *
 * Back up the start point to just after the last ch character.
 */
static void
input_backtoch( char ch )
{
	InputState *is = &input_state;
	int i;

	for( i = is->bsp[is->bspsp] - 1; i > 0 && is->buf[i] != ch; i-- )
		;

	if( is->buf[i] == ch )
		is->bsp[is->bspsp] = i + 1;
}

/* Move the last input_push() point back 1 character.
 */
static void
input_back1( void )
{
	InputState *is = &input_state;

	if( is->bsp[is->bspsp] > 0 )
		is->bsp[is->bspsp] -= 1;
}

static void
input_pop( void )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "input_pop: %d bytes into buffer\n", input_state.bwp );
#endif /*DEBUG_CHARACTER*/

	if( is->bspsp <= 0 )
		error( "bstack underflow" );

	is->bspsp--;
}

%}

%union {
	struct sym_table *yy_symtab;
	ParseNode *yy_node;
	Symbol *yy_sym;
	char *yy_name;
	ParseConst yy_const;
	UnOp yy_uop;
	BinOp yy_binop;
}

%token TK_TAG TK_IDENT TK_CONST TK_DOTDOTDOT TK_LAMBDA TK_FROM
%token TK_UMINUS TK_UPLUS TK_POW 
%token TK_LESS TK_LESSEQ TK_MORE TK_MOREEQ TK_NOTEQ
%token TK_LAND TK_LOR TK_BAND TK_BOR TK_JOIN
%token TK_IF TK_THEN TK_ELSE 
%token TK_CHAR TK_SHORT TK_CLASS TK_SCOPE
%token TK_INT TK_FLOAT TK_DOUBLE TK_SIGNED TK_UNSIGNED TK_COMPLEX
%token TK_SEPARATOR TK_DIALOG TK_LSHIFT TK_RSHIFT

%type <yy_node> expr binop uop rhs listex comma_list body 
%type <yy_node> pattern simple_pattern complex_pattern list_pattern
%type <yy_node> leaf_pattern
%type <yy_node> crhs cexprlist prhs lambda
%type <yy_const> TK_CONST 
%type <yy_name> TK_IDENT TK_TAG
%type <yy_sym> topdef

%left TK_LAMBDA 
%nonassoc TK_IF 
%left ',' 
%left TK_LOR 
%left TK_LAND '@'
%left TK_BOR
%left '^' 
%left TK_BAND
%nonassoc TK_EQ TK_NOTEQ TK_PEQ TK_PNOTEQ
%nonassoc TK_LESS TK_LESSEQ TK_MORE TK_MOREEQ 
%left TK_LSHIFT TK_RSHIFT
%left '+' '-'
%left '*' '/' '%' 
%left '!' '~' TK_JOIN TK_UMINUS TK_UPLUS 
%right TK_POW ':'
%right TK_CONST '(' 
%right TK_IDENT TK_TAG TK_SCOPE '['
%right TK_APPLICATION
%left '?' '.' 

%start select

%%

select: 
      	',' main | 
	'^' onedef | 
	'*' sdef optsemi {
		compile_check( current_compile );
	} | 
	prhs {
		char buf[MAX_STRSIZE];

		current_compile->tree = $1;

		/* Junk any old text.
		 */
		IM_FREE( current_compile->text );
		IM_FREE( current_compile->prhstext );
		IM_FREE( current_compile->rhstext );

		/* Set new text.
		 */
		IM_SETSTR( current_compile->rhstext, input_text( buf ) );

		compile_check( current_compile );
	}
	;

prhs: 
    	TK_BAND expr {
		$$ = $2;
	} | 
	'@' cexprlist {
		$$ = $2;
	}
	;

main: 
    	/* Empty */ | 
	main onedef
	;

onedef: 
      	directive {
		tool_position += 1;
	} | 
	topdef optsemi {
		tool_position += 1;
	}
	;

directive: 
	TK_SEPARATOR {
		Tool *tool;

		if( !is_top( current_symbol ) )
			yyerror( _( "not top level" ) );

		tool = tool_new_sep( current_kit, tool_position );
		tool->lineno = input_state.lineno;

		input_reset();
	} | 
	TK_DIALOG TK_CONST TK_CONST {
		Tool *tool;

		if( !is_top( current_symbol ) )
			yyerror( _( "not top level" ) );

		/* Should have two strings.
		 */
		if( $2.type != PARSE_CONST_STR || $3.type != PARSE_CONST_STR )
			yyerror( _( "not strings" ) );

		/* Add tool.
		 */
		tool = tool_new_dia( current_kit, tool_position, 
			$2.val.str, $3.val.str );
		if( !tool )
			yyerror( error_get_sub() );
		tool->lineno = input_state.lineno;

		tree_const_destroy( &$2 );
		tree_const_destroy( &$3 );

		input_reset();
	}
	;

topdef: 
	{
		last_top_sym = NULL;
		last_top_lineno = input_state.lineno;
		scope_reset();
		current_compile = root_symbol->expr->compile;
	}
	def {
		Symbol *sym = last_top_sym;
		Tool *tool;

		assert( sym );

		/* Work out extra secret parameters for locals.
		 */
		secret_build( sym->expr->compile );

		/* Add to the current kit.
		 */
		if( current_kit ) {
			tool = tool_new_sym( current_kit, tool_position, sym );
			tool->lineno = last_top_lineno;
		}

		/* Finished with this symbol.
		 */
		symbol_made( sym );

		/* Compile.
		 */
		if( compile_heap( sym->expr->compile ) )
			yyerror( _( "Unable to compile \"%s\"\n%s." ),
				IOBJECT( sym )->name, error_get_sub() );

		input_reset();

		$$ = sym;
	}
	;

/* Parse a new defining occurence.
 */
def: 
   	pattern {	
		Symbol *sym;

		g_assert( $1->type == NODE_PATTERN );

		/* Two forms: <name pattern-list rhs>, or <pattern rhs>.
		 * Enforce the no-args-to-pattern-assignment rule in the arg
		 * pattern parser.
		 */
		if( $1->arg1->type == NODE_LEAF ) {
			const char *name = IOBJECT( $1->arg1->leaf )->name;

			/* Is this a top-level definition? Check we've not 
			 * defined any other top-level syms with this name 
			 * in this parse action.
			 */
			if( current_symbol == root_symbol ) 
				if( (sym = slist_map( parse_sofar,
					(SListMapFn) iobject_test_name, 
					(void *) name )) )
					symbol_error_redefine( sym );

			/* Make defining occurence.
			 */
			sym = symbol_new_defining( current_compile, name );
			(void) symbol_user_init( sym );
			(void) compile_new_local( sym->expr );

			/* If this is to be a top-level-definition, make a 
			 * note of it.
			 */
			if( current_symbol == root_symbol ) {
				last_top_sym = sym;

				/* Also note on list of syms for this action.
				 */
				parse_sofar = g_slist_prepend( parse_sofar, 
					sym );
			}
		}
		else {
			static int count = 0;
			char name[256];

			/* We have <pattern rhs>. Make an anon symbol for this
			 * value, then the variables in the pattern become
			 * toplevels which index that.
			 */
			im_snprintf( name, 256, "$$pattern_lhs%d", count++ );
			sym = symbol_new_defining( current_compile, name );
			sym->generated = TRUE;
			(void) symbol_user_init( sym );
			(void) compile_new_local( sym->expr );

			/* Generate the code for the pattern elements.
			 */
			compile_pattern_lhs( current_compile, sym, $1->arg1 );
		}

		/* Initialise symbol parsing variables. Save old current symbol,
		 * add new one.
		 */
		scope_push();
		current_symbol = sym;
		current_compile = sym->expr->compile;

		assert( !current_compile->param );
		assert( current_compile->nparam == 0 );

		/* Junk any old def text.
		 */
		IM_FREE( current_compile->text );
		IM_FREE( current_compile->prhstext );
		IM_FREE( current_compile->rhstext );
	}
	sdef {
		compile_check( current_compile );

		/* Link unresolved names in to the outer scope.
		 */
		compile_resolve_names( current_compile, 
			compile_get_parent( current_compile ) );

		scope_pop();
	}
	;

/* Parse params/body/locals into current_expr
 */
sdef: 
	{	
		input_push( 1 );

		/* We've already read the character past the end of the 
		 * identifier (that's why we know the identifier is over).
		 */
		input_back1();
	}
	params {	
		input_push( 2 );
		input_backtoch( '=' );
	}
	body {	
		current_compile->tree = $4;
		assert( current_compile->tree );
		input_push( 4 );
	}
	locals {
		char buf[MAX_STRSIZE];

		input_pop();

		/* Save body text as rhstext.
		 */
		IM_SETSTR( current_compile->rhstext, input_text( buf ) );
		input_pop();

		/* Save params '=' body as prhstext.
		 */
		IM_SETSTR( current_compile->prhstext, input_text( buf ) );
		input_pop();

		/* Save full text of definition.
		 */
		IM_SETSTR( current_compile->text, input_text( buf ) );

#ifdef DEBUG
		printf( "%s->compile->text = \"%s\"\n",
			IOBJECT( current_compile->sym )->name, 
			current_compile->text );
		printf( "%s->compile->prhstext = \"%s\"\n",
			IOBJECT( current_compile->sym )->name, 
			current_compile->prhstext );
		printf( "%s->compile->rhstext = \"%s\"\n",
			IOBJECT( current_compile->sym )->name, 
			current_compile->rhstext );
#endif /*DEBUG*/
	}
	;
	
params: 
      	/* Empty */ | 
	params TK_IDENT {
		Symbol *sym;

		sym = symbol_new_defining( current_compile, $2 );
		symbol_parameter_init( sym );

		IM_FREE( $2 );
	}
	;

body : 
     	'=' TK_CLASS crhs {
		$$ = $3;
	} | 
	rhs {
		$$ = $1;
	}
	;

crhs: 
    	{
		ParseNode *pn = tree_class_new( current_compile );

		input_push( 3 );
		scope_push();
		current_symbol = current_compile->super;
		current_compile = current_symbol->expr->compile;

		current_parsenode = pn;
	}
	cexprlist {
		Compile *parent = compile_get_parent( current_compile );
		char buf[MAX_STRSIZE];
		int len;

		(void) input_text( buf );

		/* Always read 1 char too many.
		 */
		if( (len = strlen( buf )) > 0 ) 
			buf[len - 1] = '\0';

		IM_SETSTR( current_compile->rhstext, buf );
		input_pop();
		current_compile->tree = $2;

		if( current_compile->tree->elist )
			parent->has_super = TRUE;

		/* Do some checking.
		 */
		compile_check( current_compile );

		/* Link unresolved names.
		 */
		compile_resolve_names( current_compile, parent );

		scope_pop();

		$$ = current_parsenode;
		current_parsenode = NULL;
	}
	;

rhs: 
   	'=' expr { 	
		$$ = $2;
	} | 
	'=' expr ',' expr optsemi rhs { 	
		$$ = tree_ifelse_new( current_compile, $4, $2, $6 );
	}
	;

locals:	
      	';' | 
	'{' deflist '}' | 
	'{' '}' 
	;	

optsemi: 
       	/* Empty */ | 
	';' optsemi
	;

deflist: 
       	def {
		input_pop();
		input_push( 5 );
	}
	optsemi | 
	deflist def {
		input_pop();
		input_push( 6 );
	}
	optsemi
	;

cexprlist: 
	/* Empty */ {
		$$ = tree_super_new( current_compile );
	} | 
	cexprlist expr %prec TK_APPLICATION {
		$$ = tree_super_extend( current_compile, $1, $2 );
	}
	;

expr: 
    	'(' expr ')' { 
		$$ = $2;
	} | 
	TK_CONST {
		$$ = tree_const_new( current_compile, $1 );
	} | 
	TK_IDENT {
		$$ = tree_leaf_new( current_compile, $1 );
		IM_FREE( $1 );
	} | 
	TK_TAG {
		$$ = tree_tag_new( current_compile, $1 );
		IM_FREE( $1 );
	} | 
	TK_SCOPE {
		$$ = tree_leaf_new( current_compile, 
			IOBJECT( symbol_get_scope( current_symbol ) )->name );
	} | 
	TK_IF expr TK_THEN expr TK_ELSE expr %prec TK_IF {
		$$ = tree_ifelse_new( current_compile, $2, $4, $6 );
	} | 
	expr expr %prec TK_APPLICATION {
		$$ = tree_appl_new( current_compile, $1, $2 );
	} | 
	lambda | 
	listex {
		$$ = $1;
	} | 
	'(' expr ',' expr ')' {	
		$$ = tree_binop_new( current_compile, BI_COMMA, $2, $4 );
	} | 
	binop | 
	uop
	;

lambda:
	TK_LAMBDA TK_IDENT %prec TK_LAMBDA {
		static int count = 0;
		char name[256];
		Symbol *sym;

		/* Make an anonymous symbol local to the current sym, compile
		 * the expr inside that.
		 */
		im_snprintf( name, 256, "$$lambda%d", count++ );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );

		/* Initialise symbol parsing variables. Save old current symbol,
		 * add new one.
		 */
		scope_push();
		current_symbol = sym;
		current_compile = sym->expr->compile;

		/* Make the parameter.
		 */
		sym = symbol_new_defining( current_compile, $2 );
		symbol_parameter_init( sym );
		IM_FREE( $2 );
	}
	expr {
		Symbol *sym;

		current_compile->tree = $4;

		if( !compile_check( current_compile ) )
			yyerror( error_get_sub() );

		/* Link unresolved names in to the outer scope.
		 */
		compile_resolve_names( current_compile, 
			compile_get_parent( current_compile ) );

		/* The value of the expr is the anon we defined.
		 */
		sym = current_symbol;
		scope_pop();
		$$ = tree_leafsym_new( current_compile, sym );
	}
	;

listex: 
      	'[' expr TK_DOTDOTDOT ']' {
		$$ = tree_generator_new( current_compile, $2, NULL, NULL );
	} | 
	'[' expr TK_DOTDOTDOT expr ']' {
		$$ = tree_generator_new( current_compile, $2, NULL, $4 );
	} | 
	'[' expr ',' expr TK_DOTDOTDOT ']' {
		$$ = tree_generator_new( current_compile, $2, $4, NULL );
	} | 
	'[' expr ',' expr TK_DOTDOTDOT expr ']' {
		$$ = tree_generator_new( current_compile, $2, $4, $6 );
	} | 
	'[' expr TK_BOR TK_IDENT TK_FROM expr {
		static int count = 0;
		char name[256];
		Symbol *sym;
		Compile *enclosing = current_compile;

		/* Make an anonymous symbol local to the current sym, save
		 * the map expr inside that. 
		 */
		im_snprintf( name, 256, "$$lcomp%d", count++ );
		sym = symbol_new_defining( current_compile, name );
		(void) symbol_user_init( sym );
		sym->generated = TRUE;
		(void) compile_new_local( sym->expr );

		/* Initialise symbol parsing variables. Save old current symbol,
		 * add new one.
		 */
		scope_push();
		current_symbol = sym;
		current_compile = sym->expr->compile;

		/* Somewhere to save the result expr
		 */
		sym = symbol_new_defining( current_compile, "$$result" );
		sym->generated = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = compile_copy_tree( enclosing, $2, 
			sym->expr->compile );

		/* Make the first "x <- expr" generator.
		 */
		sym = symbol_new_defining( current_compile, $4 );
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = compile_copy_tree( enclosing, $6, 
			sym->expr->compile );

		IM_FREE( $4 );
	}
	frompred_list ']' {
		Symbol *sym;

		/* The map expr can refer to generator names. Resolve inwards
		 * so it links to the generators.
		 */
		compile_resolve_names( compile_get_parent( current_compile ),
			current_compile ); 

		/* Generate the code for the list comp.
		 */
		compile_lcomp( current_compile );

		compile_check( current_compile );

		/* Link unresolved names outwards.
		 */
		compile_resolve_names( current_compile, 
			compile_get_parent( current_compile ) );

		/* The value of the expr is the anon we defined.
		 */
		sym = current_symbol;
		scope_pop();
		$$ = tree_leafsym_new( current_compile, sym );
	} |
	'[' comma_list ']' {
		$$ = $2;
	} | 
	'[' ']' {
		ParseConst elist;

		elist.type = PARSE_CONST_ELIST;
		$$ = tree_const_new( current_compile, elist );
	}
	;

frompred_list: 
	/* Empty */ {
	} |
     	frompred_list ';' frompred {
	}
	;

frompred:
       TK_IDENT TK_FROM expr {
		Symbol *sym;

		sym = symbol_new_defining( current_compile, $1 );
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = $3;
		IM_FREE( $1 );
       } |
       expr {
		static int count = 0;
		char name[256];
		Symbol *sym;

		im_snprintf( name, 256, "$$filter%d", count++ );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = $1;
       }
       ;

comma_list: 
     	expr ',' comma_list {
		$$ = tree_lconst_extend( current_compile, $3, $1 );
	} | 
	expr {
		$$ = tree_lconst_new( current_compile, $1 );
	}
	;

binop: 
     	expr '+' expr {	
		$$ = tree_binop_new( current_compile, BI_ADD, $1, $3 );
	} | 
	expr ':' expr {	
		$$ = tree_binop_new( current_compile, BI_CONS, $1, $3 );
	} | 
	expr '-' expr {	
		$$ = tree_binop_new( current_compile, BI_SUB, $1, $3 );
	} | 
	expr '?' expr {	
		$$ = tree_binop_new( current_compile, BI_SELECT, $1, $3 );
	} | 
	expr '/' expr {	
		$$ = tree_binop_new( current_compile, BI_DIV, $1, $3 );
	} | 
	expr '*' expr {	
		$$ = tree_binop_new( current_compile, BI_MUL, $1, $3 );
	} | 
	expr '%' expr {
		$$ = tree_binop_new( current_compile, BI_REM, $1, $3 );
	} | 
	expr TK_JOIN expr {	
		$$ = tree_binop_new( current_compile, BI_JOIN, $1, $3 );
	} | 
	expr TK_POW expr {	
		$$ = tree_binop_new( current_compile, BI_POW, $1, $3 );
	} | 
	expr TK_LSHIFT expr {	
		$$ = tree_binop_new( current_compile, BI_LSHIFT, $1, $3 );
	} | 
	expr TK_RSHIFT expr {	
		$$ = tree_binop_new( current_compile, BI_RSHIFT, $1, $3 );
	} | 
	expr '^' expr {	
		$$ = tree_binop_new( current_compile, BI_EOR, $1, $3 );
	} | 
	expr TK_LAND expr {	
		$$ = tree_binop_new( current_compile, BI_LAND, $1, $3 );
	} | 
	expr TK_BAND expr {	
		$$ = tree_binop_new( current_compile, BI_BAND, $1, $3 );
	} | 
	expr '@' expr {	
		$$ = tree_compose_new( current_compile, $1, $3 );
	} | 
	expr TK_LOR expr {	
		$$ = tree_binop_new( current_compile, BI_LOR, $1, $3 );
	} | 
	expr TK_BOR expr {	
		$$ = tree_binop_new( current_compile, BI_BOR, $1, $3 );
	} | 
	expr TK_LESS expr {	
		$$ = tree_binop_new( current_compile, BI_LESS, $1, $3 );
	} | 
	expr TK_LESSEQ expr {	
		$$ = tree_binop_new( current_compile, BI_LESSEQ, $1, $3 );
	} | 
	expr TK_MORE expr {	
		$$ = tree_binop_new( current_compile, BI_MORE, $1, $3 );
	} | 
	expr TK_MOREEQ expr {	
		$$ = tree_binop_new( current_compile, BI_MOREEQ, $1, $3 );
	} | 
	expr TK_EQ expr {	
		$$ = tree_binop_new( current_compile, BI_EQ, $1, $3 );
	} | 
	expr TK_NOTEQ expr {	
		$$ = tree_binop_new( current_compile, BI_NOTEQ, $1, $3 );
	} | 
	expr TK_PEQ expr {	
		$$ = tree_binop_new( current_compile, BI_PEQ, $1, $3 );
	} | 
	expr TK_PNOTEQ expr {	
		$$ = tree_binop_new( current_compile, BI_PNOTEQ, $1, $3 );
	} | 
	expr '.' expr {
		$$ = tree_binop_new( current_compile, BI_DOT, $1, $3 );
	}
	;

signed: 
      	/* Nothing */ | 
	TK_SIGNED
	;

unsigned: 
	/* Nothing */ | 
	TK_UNSIGNED
	;

uop: 
   	'(' unsigned TK_CHAR ')' expr %prec TK_UMINUS {	
		$$ = tree_unop_new( current_compile, UN_CUCHAR, $5 );
	} | 
	'(' TK_SIGNED TK_CHAR ')' expr %prec TK_UMINUS {	
		$$ = tree_unop_new( current_compile, UN_CSCHAR, $5 );
	} | 
	'(' signed TK_SHORT ')' expr %prec TK_UMINUS {	
		$$ = tree_unop_new( current_compile, UN_CSSHORT, $5 );
	} | 
	'(' TK_UNSIGNED TK_SHORT ')' expr %prec TK_UMINUS {
		$$ = tree_unop_new( current_compile, UN_CUSHORT, $5 );
	} | 
	'(' signed TK_INT ')' expr %prec TK_UMINUS {	
		$$ = tree_unop_new( current_compile, UN_CSINT, $5 );
	} | 
	'(' TK_UNSIGNED TK_INT ')' expr %prec TK_UMINUS {
		$$ = tree_unop_new( current_compile, UN_CUINT, $5 );
	} | 
	'(' TK_FLOAT ')' expr %prec TK_UMINUS {
		$$ = tree_unop_new( current_compile, UN_CFLOAT, $4 );
	} | 
	'(' TK_DOUBLE ')' expr %prec TK_UMINUS {
		$$ = tree_unop_new( current_compile, UN_CDOUBLE, $4 );
	} | 
	'(' TK_COMPLEX ')' expr %prec TK_UMINUS {
		$$ = tree_unop_new( current_compile, UN_CCOMPLEX, $4 );
	} | 
	'(' TK_DOUBLE TK_COMPLEX ')' expr %prec TK_UMINUS {
		$$ = tree_unop_new( current_compile, UN_CDCOMPLEX, $5 );
	} | 
	TK_UMINUS expr {
		$$ = tree_unop_new( current_compile, UN_MINUS, $2 );
	} | 
	'!' expr {
		$$ = tree_unop_new( current_compile, UN_NEG, $2 );
	} | 
	'~' expr {
		$$ = tree_unop_new( current_compile, UN_COMPLEMENT, $2 );
	} | 
	TK_UPLUS expr {
		$$ = tree_unop_new( current_compile, UN_PLUS, $2 );
	}
	;

pattern:
	simple_pattern {
		$$ = tree_pattern_new( current_compile );
		$$->arg1 = $1;
	}
	;

/* Stuff that can appear on the LHS of an equals, or as a parameter pattern.
 */
simple_pattern:
	leaf_pattern | 
	'(' leaf_pattern ',' leaf_pattern ')' {
		$$ = tree_binop_new( current_compile, BI_COMMA, $2, $4 );
	} |
	simple_pattern ':' simple_pattern { 
		$$ = tree_binop_new( current_compile, BI_CONS, $1, $3 );
	} |
	'(' complex_pattern ')' {
		$$ = $2;
	} | 
	'[' list_pattern ']' {
		$$ = $2;
	} |
	'[' ']' {
		ParseConst elist;

		elist.type = PARSE_CONST_ELIST;
		$$ = tree_const_new( current_compile, elist );
	}  
	;

/* Stuff that can appear in a complex (a, b) pattern.
 */
leaf_pattern:
	TK_IDENT {
		$$ = tree_leaf_new( current_compile, $1 );
		IM_FREE( $1 );
	} | 
	TK_CONST {
		$$ = tree_const_new( current_compile, $1 );
	} 
	;

/* What can appear in round brackets or a comma list.
 */
complex_pattern:
	TK_IDENT TK_IDENT {
		$$ = tree_pattern_class_new( current_compile, $1, $2 );
		IM_FREE( $1 );
		IM_FREE( $2 );
	} |
	simple_pattern
	;

list_pattern:
     	complex_pattern ',' list_pattern {
		$$ = tree_lconst_extend( current_compile, $3, $1 );
	} | 
	complex_pattern {
		$$ = tree_lconst_new( current_compile, $1 );
	}
	;

%%

/* Interface to parser. 
 */
static gboolean
parse_input( int ch, Symbol *sym, Toolkit *kit, int pos )
{
	current_kit = kit;
	current_symbol = sym;
	root_symbol = sym;
	tool_position = pos;
	last_top_sym = NULL;

	scope_reset();
	input_reset();

	/* Signal start nonterminal to parser.
	 */
	ip_unput( ch );

	if( setjmp( parse_error_point ) ) {
		/* Restore current_compile.
		 */
		scope_pop_all();

		if( current_compile ) 
			compile_error_set( current_compile );

		IM_FREEF( g_slist_free, parse_sofar );

		return( FALSE );
	}
	yyparse();
	IM_FREEF( g_slist_free, parse_sofar );

	/* All ok.
	 */
	return( TRUE );
}

/* Parse the input into a set of symbols at a position in a kit. 
 * kit may be NULL for no kit. 
 */
gboolean
parse_toplevel( Toolkit *kit, int pos )
{
	gboolean result;

	current_compile = NULL;

	result = parse_input( ',', kit->kitg->root, kit, pos );

	if( kit )
		iobject_changed( IOBJECT( kit ) );

	return( result );
}

/* Parse a single top-level definition.
 */
gboolean
parse_onedef( Toolkit *kit, int pos )
{
	gboolean result;

	current_compile = NULL;

	result = parse_input( '^', kit->kitg->root, kit, pos );

	if( kit )
		iobject_changed( IOBJECT( kit ) );

	return( result );
}

/* Parse new text into "expr". If params is set, str should be "a b = a+b"
 * (ie. include params), if not, then just rhs (eg. "a+b").
 */
gboolean
parse_rhs( Expr *expr, ParseRhsSyntax syntax )
{
	static const char start_ch_table[] = {
		'&',		/* PARSE_RHS */
		'*',		/* PARSE_PARAMS */		
		'@'		/* PARSE_SUPER */
	};

	char start_ch = start_ch_table[(int) syntax];
	Compile *compile = compile_new_local( expr );

	current_compile = compile;
	if( !parse_input( start_ch, expr->sym, NULL, -1 ) ) {
		current_compile = NULL;
		return( FALSE );
	}
	current_compile = NULL;

#ifdef DEBUG
	printf( "parse_rhs:\n" );
	dump_tree( compile->tree );
#endif /*DEBUG*/

	/* Resolve any dynamic refs.
	 */
	expr_resolve( expr );

	/* Work out extra secret parameters for locals.
	 */
	secret_build( compile );

	/* Compile.
	 */
	if( compile_heap( compile ) )
		return( FALSE );

	return( TRUE );
}

/* Free any stuff the lexer might have allocated. 
 */
void
free_lex( int yychar )
{
	switch( yychar ) {
	case TK_CONST:
		tree_const_destroy( &yylval.yy_const );
		break;

	case TK_IDENT:
	case TK_TAG:
		IM_FREE( yylval.yy_name );
		break;

	default:
		break;
	}
}

/* Do we have a string of the form "IDENT = .."? Use the lexer to look along
 * the string checking components, return the IDENT if we do. 
 */
char *
parse_test_define( void )
{
	extern int yylex( void );
	int yychar;
	char *ident;

	ident = NULL;

	if( setjmp( parse_error_point ) ) {
		/* Here for yyerror in lex. 
		 */
		IM_FREE( ident );
		return( NULL ); 
	}

	if( (yychar = yylex()) != TK_IDENT ) {
		free_lex( yychar );
		yyerror( _( "no leading identifier" ) );
	}
	ident = yylval.yy_name;

	if( (yychar = yylex()) != '=' ) {
		free_lex( yychar );
		yyerror( _( "'=' missing" ) );
	}

	return( ident );
}

/* Do we have a string like "Workspaces.untitled.A1 = .."? Check for the
 * symbols as we see them, make the last one and return it. Used by --set.
 */
Symbol *
parse_set_symbol( void )
{
	int yychar;
	extern int yylex( void );
	Compile *compile = symbol_root->expr->compile;
	char *ident;
	Symbol *sym;

	ident = NULL;

	if( setjmp( parse_error_point ) ) {
		/* Here for yyerror in lex. 
		 */
		IM_FREE( ident );
		return( NULL ); 
	}

	do {
		if( (yychar = yylex()) != TK_IDENT && yychar != TK_TAG ) {
			free_lex( yychar );
			yyerror( _( "identifier expected" ) );
		}
		ident = yylval.yy_name;

		switch( (yychar = yylex()) ) {
		case '.':
			/* There's a dot, so we expect another identifier to 
			 * come. Look up this one and move to that context.
			 */
			if( !(sym = compile_lookup( compile, ident )) ) 
				yyerror( _( "'%s' does not exist" ), ident );
			if( !sym->expr || !sym->expr->compile )
				yyerror( _( "'%s' has no members" ), ident );
			compile = sym->expr->compile;
			IM_FREE( ident );
			break;

		case '=':
			/* This is the final identifier: create the symbol in
			 * this context.
			 */
			sym = symbol_new_defining( compile, ident );
			IM_FREE( ident );
			break;

		default:
			free_lex( yychar );
			yyerror( _( "'.' or '=' expected" ) ); 
		}
	} while( yychar != '=' );

	return( sym );
}
