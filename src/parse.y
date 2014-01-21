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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

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

/* The lexer from lex.l.
 */
int yylex( void );
void yyrestart( FILE *input_file );

/* Declare file-private stuff, shared with the lexer. Bison will put this
 * stuff into parse.h, so just declare, don't define. Sadly we can't have
 * these things static :(
 */

/* Global .. the symbol whose definition we are currently parsing, the symbol
 * which all defs in this parse action should be made local to.
 */
extern Symbol *current_symbol;
extern Symbol *root_symbol;

/* The current parse context.
 */
extern Compile *current_compile;
extern ParseNode *current_parsenode;

/* The kit we are adding new symbols to.
 */
extern Toolkit *current_kit;

/* Where it should go in the kit.
 */
extern int tool_position;

/* Lineno of start of last top-level def.
 */
extern int last_top_lineno;

/* Text we've gathered in this lex.
 */
extern char lex_text_buffer[MAX_STRSIZE];

/* Stack of symbols for parser - each represents a new scope level.
 */
extern Symbol *scope_stack_symbol[MAX_SSTACK];
extern Compile *scope_stack_compile[MAX_SSTACK];
extern int scope_sp;

/* Use to generate unique ids for anonymouse parse objects (eg. lambdas etc).
 */
extern int parse_object_id;

/* Get text for parsed objects.
 */
char *input_text( char *out );
void input_reset( void );
void input_push( int n );
void input_backtoch( char ch );
void input_back1( void );
void input_pop( void );

/* Nest and unnest scopes.
 */
void scope_push( void );
void scope_pop( void );
void scope_pop_all( void );
void scope_reset( void );

/* Helper functions.
 */
void *parse_toplevel_end( Symbol *sym );
void *parse_access_end( Symbol *sym, Symbol *main );

%}

%union {
	struct sym_table *yy_symtab;
	ParseNode *yy_node;
	char *yy_name;
	ParseConst yy_const;
	UnOp yy_uop;
	BinOp yy_binop;
}

%token TK_TAG TK_IDENT TK_CONST TK_DOTDOTDOT TK_LAMBDA TK_FROM TK_TO TK_SUCHTHAT
%token TK_UMINUS TK_UPLUS TK_POW 
%token TK_LESS TK_LESSEQ TK_MORE TK_MOREEQ TK_NOTEQ
%token TK_LAND TK_LOR TK_BAND TK_BOR TK_JOIN TK_DIFF
%token TK_IF TK_THEN TK_ELSE 
%token TK_CHAR TK_SHORT TK_CLASS TK_SCOPE
%token TK_INT TK_FLOAT TK_DOUBLE TK_SIGNED TK_UNSIGNED TK_COMPLEX
%token TK_SEPARATOR TK_DIALOG TK_LSHIFT TK_RSHIFT

%type <yy_node> expr binop uop rhs list_expression comma_list body 
%type <yy_node> simple_pattern complex_pattern list_pattern
%type <yy_node> leaf_pattern
%type <yy_node> crhs cexprlist prhs lambda
%type <yy_const> TK_CONST 
%type <yy_name> TK_IDENT TK_TAG

%left TK_SUCHTHAT 
%left TK_LAMBDA 
%nonassoc TK_IF 
%left ',' 
%left TK_TO 
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
%left '!' '~' TK_JOIN TK_DIFF TK_UMINUS TK_UPLUS 
%right TK_POW ':'
%right TK_CONST '(' 
%right TK_IDENT TK_TAG TK_SCOPE '['
%right TK_APPLICATION
%left '?' '.' 

%start select

/* 

  Our syntax for list comprehensions is not LALR(1). We have:

  	simple_pattern '<-' expr ';' |
	expr ';'

  simple_pattern can be something like

	a:x

  which is also an expr. We don't know which branch to take until we see a
  '<' or a ';'.

  Use bison's GLR system to parse this, and ignore the first 13 reduce/reduce
  conflicts caused by this ambiguity.

  FIXME ... we now depend on bison, but we still have some yacc compatibility
  stuff in here, and we don't use all of bison's nice features (eg. for
  tracking line numbers in the source file). Fix this up at some stage.

 */

%glr-parser
%expect-rr 13

%error-verbose

%%

select: 
      	',' main | 
	'^' single_definition | 
	'*' params_plus_rhs optsemi {
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
	main single_definition
	;

single_definition: 
      	directive {
		tool_position += 1;
	} | 
	toplevel_definition optsemi {
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

		/* Cast away const here.
		 */
		tree_const_destroy( (ParseConst *) &$2 );
		tree_const_destroy( (ParseConst *) &$3 );

		input_reset();
	}
	;

toplevel_definition: 
	{
		last_top_lineno = input_state.lineno;
		scope_reset();
		current_compile = root_symbol->expr->compile;
	}
	definition {
		input_reset();
	}
	;

/* Parse a new defining occurence. This can be a local or a top-level.
 */
definition: 
   	simple_pattern {	
		Symbol *sym;

		/* Two forms: <name pattern-list rhs>, or <pattern rhs>.
		 * Enforce the no-args-to-pattern-assignment rule in the arg
		 * pattern parser.
		 */
		if( $1->type == NODE_LEAF ) {
			const char *name = IOBJECT( $1->leaf )->name;

			/* Make a new defining occurence.
			 */
			sym = symbol_new_defining( current_compile, name );

			(void) symbol_user_init( sym );
			(void) compile_new_local( sym->expr );
		}
		else {
			char name[256];

			/* We have <pattern rhs>. Make an anon symbol for this
			 * value, then the variables in the pattern become
			 * toplevels which access that.
			 */
			if( !compile_pattern_has_leaf( $1 ) )
				yyerror( _( "left-hand-side pattern "
					"contains no identifiers" ) );
			im_snprintf( name, 256, "$$pattern_lhs%d",
				parse_object_id++ );
			sym = symbol_new_defining( current_compile, name );
			sym->generated = TRUE;
			(void) symbol_user_init( sym );
			(void) compile_new_local( sym->expr );
		}

		/* Note on the enclosing last_sym. Things like the program
		 * window use this to work out what sym to display after a
		 * parse. symbol_dispose() is careful to NULL this out.
		 */
		current_compile->last_sym = sym;

		/* Initialise symbol parsing variables. Save old current symbol,
		 * add new one.
		 */
		scope_push();
		current_symbol = sym;
		current_compile = sym->expr->compile;

		g_assert( !current_compile->param );
		g_assert( current_compile->nparam == 0 );

		/* Junk any old def text.
		 */
		IM_FREE( current_compile->text );
		IM_FREE( current_compile->prhstext );
		IM_FREE( current_compile->rhstext );
	}
	params_plus_rhs {
		compile_check( current_compile );

		/* Link unresolved names into the outer scope.
		 */
		compile_resolve_names( current_compile, 
			compile_get_parent( current_compile ) );

		/* Is this the end of a top-level? Needs extra work to add to
		 * the enclosing toolkit etc.
		 */
		if( is_scope( symbol_get_parent( current_symbol ) ) ) 
			parse_toplevel_end( current_symbol );

		/* Is this a pattern definition? Expand the pattern to a
		 * set of access defs.
		 */
		if( $1->type != NODE_LEAF ) {
			Compile *parent = compile_get_parent( current_compile );
			GSList *built_syms;

			built_syms = compile_pattern_lhs( parent, 
				current_symbol, $1 );

			if( is_scope( symbol_get_parent( current_symbol ) ) )
				slist_map( built_syms,
					(SListMapFn) parse_toplevel_end, NULL );
			slist_map( built_syms,
				(SListMapFn) parse_access_end, 
				current_symbol );

			g_slist_free( built_syms );
		}

		scope_pop();
	}
	;

/* Parse params/body/locals into current_expr
 */
params_plus_rhs: 
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
		g_assert( current_compile->tree );
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
	params simple_pattern {
		Symbol *sym;

		/* If the pattern is just an identifier, make it a direct
		 * parameter. Otherwise make an anon param and put the pattern
		 * in as a local with the same id.
		 *
		 *	fred [a] = 12;
		 *
		 * parses to:
		 *
		 *	fred $$arg42 = 12 { $$patt42 = [a]; }
		 * 
		 * A later pass creates the "a = $$arg42?0" definition.
		 */
		if( $2->type == NODE_LEAF ) {
			const char *name = IOBJECT( $2->leaf )->name;

			/* Make defining occurence.
			 */
			sym = symbol_new_defining( current_compile, name );
			(void) symbol_parameter_init( sym );
		}
		else {
			char name[256];

			im_snprintf( name, 256, "$$arg%d", parse_object_id );
			sym = symbol_new_defining( current_compile, name );
			sym->generated = TRUE;
			(void) symbol_parameter_init( sym );

			im_snprintf( name, 256, "$$patt%d", parse_object_id++ );
			sym = symbol_new_defining( current_compile, name );
			sym->generated = TRUE;
			(void) symbol_user_init( sym );
			(void) compile_new_local( sym->expr );
			sym->expr->compile->tree = $2;
		}
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
       	definition {
		input_pop();
		input_push( 5 );
	}
	optsemi | 
	deflist definition {
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
		im_free( $1 );
	} | 
	TK_TAG {
		$$ = tree_tag_new( current_compile, $1 );
		im_free( $1 );
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
	list_expression {
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
		char name[256];
		Symbol *sym;

		/* Make an anonymous symbol local to the current sym, compile
		 * the expr inside that.
		 */
		im_snprintf( name, 256, "$$lambda%d", parse_object_id++ );
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
		im_free( $2 );
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

list_expression: 
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
	'[' expr TK_SUCHTHAT {
		char name[256];
		Symbol *sym;
		Compile *enclosing = current_compile;

		/* Make an anonymous symbol local to the current sym, copy
		 * the map expr inside that. 
		 */
		im_snprintf( name, 256, "$$lcomp%d", parse_object_id++ );
		sym = symbol_new_defining( current_compile, name );
		(void) symbol_user_init( sym );
		sym->generated = TRUE;
		(void) compile_new_local( sym->expr );

		/* Push a new scope.
		 */
		scope_push();
		current_symbol = sym;
		current_compile = sym->expr->compile;

		/* Somewhere to save the result expr. We have to copy the
		 * expr, as we want it to be bound in $$lcomp's context so
		 * that it can see the generators.
		 */
		sym = symbol_new_defining( current_compile, "$$result" );
		sym->generated = TRUE;
		sym->placeholder = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = compile_copy_tree( enclosing, $2, 
			sym->expr->compile );
	}
	generator frompred_list ']' {
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

generator:
       simple_pattern TK_FROM expr {
		char name[256];
		Symbol *sym;

		im_snprintf( name, 256, "$$pattern%d", parse_object_id );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		sym->placeholder = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = $1;

		im_snprintf( name, 256, "$$generator%d", parse_object_id++ );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		sym->placeholder = TRUE;
		(void) symbol_user_init( sym );
		(void) compile_new_local( sym->expr );
		sym->expr->compile->tree = $3;
       }
       ;

frompred:
       generator |
       expr {
		char name[256];
		Symbol *sym;

		im_snprintf( name, 256, "$$filter%d", parse_object_id++ );
		sym = symbol_new_defining( current_compile, name );
		sym->generated = TRUE;
		sym->placeholder = TRUE;
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

/* How odd, break the "'+' { BI_ADD } | ..." into a separate production and we
 * get reduce/reduce conflits. Copypaste a lot instead.
 */
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
	} |
	expr TK_DIFF expr {	
		ParseNode *pn1, *pn2;

		pn1 = tree_leaf_new( current_compile, "difference" );
		pn2 = tree_leaf_new( current_compile, "equal" );
		pn1 = tree_appl_new( current_compile, pn1, pn2 );
		pn1 = tree_appl_new( current_compile, pn1, $1 );
		pn1 = tree_appl_new( current_compile, pn1, $3 );

		$$ = pn1;
	} | 
	expr TK_TO expr {
		ParseNode *pn;

		pn = tree_leaf_new( current_compile, "mknvpair" );
		pn = tree_appl_new( current_compile, pn, $1 );
		pn = tree_appl_new( current_compile, pn, $3 );

		$$ = pn;
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
		im_free( $1 );
	} | 
	TK_CONST {
		$$ = tree_const_new( current_compile, $1 );
	} 
	;

/* What can appear in round brackets or a comma list.
 */
complex_pattern:
	TK_IDENT TK_IDENT {
		$$ = tree_pattern_class_new( current_compile, $1,
			tree_leaf_new( current_compile, $2 ) );
		im_free( $1 );
		im_free( $2 );
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

/* Return point on syntax error.
 */
jmp_buf parse_error_point;

/* Text we've lexed.
 */
char lex_text_buffer[MAX_STRSIZE];
VipsBuf lex_text = VIPS_BUF_STATIC( lex_text_buffer );

/* State of input system.
 */
InputState input_state;

/* Defintions for the static decls at the top. We have to put the defs down
 * here to mkake sure they don't creep in to the generated parser.h.
 */

/* Actually, we can't make these static :-( since they are declared extern at
 * the top of the file.
 */
Symbol *current_symbol;
Symbol *root_symbol;
Compile *current_compile = NULL;
ParseNode *current_parsenode = NULL;
Toolkit *current_kit;
int tool_position;
int last_top_lineno;
Symbol *scope_stack_symbol[MAX_SSTACK];
Compile *scope_stack_compile[MAX_SSTACK];
int scope_sp = 0;
int parse_object_id = 0;

/* Here for errors in parse. 
 *
 * Bison calls yyerror with only a char* arg. This printf() version is called
 * from nip2 in a few places during parse.
 */
void
nip2yyerror( const char *sub, ... )
{
	va_list ap;
 	char buf[4096];

        va_start( ap, sub );
        (void) im_vsnprintf( buf, 4096, sub, ap );
        va_end( ap );

	error_top( _( "Parse error." ) );

	if( current_compile && current_compile->last_sym ) 
		error_sub( _( "Error in %s: %s" ), 
			IOBJECT(  current_compile->last_sym )->name, buf );
	else
		error_sub( _( "Error: %s" ), buf );

	longjmp( parse_error_point, -1 );
}

/* Bison calls this.
 */
void
yyerror( const char *msg )
{
	nip2yyerror( "%s", msg );
}

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
	vips_buf_rewind( &lex_text );
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
	vips_buf_rewind( &lex_text );
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
		yyerror( _( "definition is too long" ) );
	if( is->bwp >= 0 )
		is->buf[is->bwp] = ch;
	is->bwp++;

	/* Add to lex text buffer.
	 */
	if( is->charno > 0 )
		vips_buf_appendc( &lex_text, ch );

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
		vips_buf_removec( &lex_text, ch );
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

/* Return the text we have accumulated for the current definition. Remove
 * leading and trailing whitespace and spare semicolons. out needs to be
 * MAX_STRSIZE.
 */
char *
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

	g_assert( len < MAX_STRSIZE - 1 );
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
void
input_reset( void )
{
	InputState *is = &input_state;

#ifdef DEBUG_CHARACTER
	printf( "input_reset:\n" );
#endif /*DEBUG_CHARACTER*/

	is->bwp = 0;
	is->bspsp = 0;
	is->bsp[0] = 0;
	vips_buf_rewind( &lex_text );
}

void
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
void
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
void
input_back1( void )
{
	InputState *is = &input_state;

	if( is->bsp[is->bspsp] > 0 )
		is->bsp[is->bspsp] -= 1;
}

void
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

void
scope_push( void )
{
	if( scope_sp == MAX_SSTACK )
		error( "sstack overflow" );

	scope_stack_symbol[scope_sp] = current_symbol;
	scope_stack_compile[scope_sp] = current_compile;
	scope_sp += 1;
}

void
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
void
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
void
scope_reset( void )
{
	scope_sp = 0;
}

/* End of top level parse. Fix up the symbol.
 */
void *
parse_toplevel_end( Symbol *sym )
{
	Tool *tool;

	tool = tool_new_sym( current_kit, tool_position, sym );
	tool->lineno = last_top_lineno;
	symbol_made( sym );

	return( NULL );
}

/* Built a pattern access definition. Set the various text fragments from the
 * def we are drived from.
 */
void *
parse_access_end( Symbol *sym, Symbol *main )
{
	IM_SETSTR( sym->expr->compile->rhstext, 
		main->expr->compile->rhstext ); 
	IM_SETSTR( sym->expr->compile->prhstext, 
		main->expr->compile->prhstext ); 
	IM_SETSTR( sym->expr->compile->text, 
		main->expr->compile->text ); 

	return( NULL );
}

/* Interface to parser. 
 */
static gboolean
parse_input( int ch, Symbol *sym, Toolkit *kit, int pos )
{
	current_kit = kit;
	current_symbol = sym;
	root_symbol = sym;
	tool_position = pos;

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

		return( FALSE );
	}
	yyparse();

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

	/* Compile.
	 */
	if( compile_object( compile ) )
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
 * the string checking components, return the IDENT if we do, NULL otherwise.  
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

		return( NULL ); 
	}
	ident = yylval.yy_name;

	if( (yychar = yylex()) != '=' ) {
		free_lex( yychar );
		IM_FREE( ident ); 

		return( NULL ); 
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
				nip2yyerror( _( "'%s' does not exist" ), 
					ident );
			if( !sym->expr || 
				!sym->expr->compile )
				nip2yyerror( _( "'%s' has no members" ), 
					ident );
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
