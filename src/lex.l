%{
/* Lexer for image processing expressions.
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

#ifdef HAVE_FLEX

/* Flex has a different input mechanism :(
 */

#define YY_INPUT(buf,result,max_size) { \
	extern int ip_input( void ); \
	int c = ip_input(); \
	result = (c == 0) ? YY_NULL : (buf[0] = c, 1); \
}

#undef unput
#define unput ip_unput

#else /*HAVE_FLEX*/

/* Assume this is plain lex.
 */

/* Redefine input, output, unput and yywrap.
 */
#undef input
#undef output
#undef unput
#undef YYLMAX

/* See parse.y for input and unput.
 */
#define output(A) (error( "output called by lex" ))

#define YYLMAX MAX_STRSIZE

#define unput ip_unput
#define input ip_input
#endif /*HAVE_FLEX*/

/* Stuff from bison.
 */
#include "parse.h"

/* Read a string into a buffer. Read up to the " character, " can be
 * escaped with '\'.
 */
static void
read_string( char *buf )
{
	int ch;
	int i;

	/* Read up to \n, ", EOF, ignoring \"
	 * Don't forget about "\\" though.
	 */
	for( i = 0; (ch = ip_input()); i++ ) {
		if( ch == EOF || ch == '\n' || ch == '"' || ch == '\0' )
			break;
		if( i >= MAX_STRSIZE )
			yyerror( _( "line too long" ) );

		buf[i] = ch;

		if( ch == '\\' ) {
			ch = ip_input();

			if( ch == EOF || ch == '\n' || ch == '\0' )
				break;
			if( i >= MAX_STRSIZE )
				yyerror( _( "line too long" ) );

			buf[++i] = ch;
		}
	}
	buf[i] = '\0';

	if( ch == '\n' )
		yyerror( _( "end of line inside string" ) );
	if( ch == EOF || ch == '\0' )
		yyerror( _( "no end of string" ) );
}

/* Read a char constant. The leading ' has already been seen. Cases to consider:
 * 	'\n'
 * 	'\\'
 * 	'''	(illegal in C, but I think we allow it)
 * 	'\''
 */
static int
read_char( void )
{
	int ch;

	ch = ip_input();

	if( ch == EOF || ch == '\n' || ch == '\0' )
		yyerror( _( "bad char constant" ) );
	if( ch == '\\' ) {
		char buf[3];
		char buf2[3];

		buf[0] = ch;
		buf[1] = ch = ip_input();
		buf[2] = '\0';

		if( ch == EOF || ch == '\n' || ch == '\0' )
			yyerror( _( "bad char constant" ) );

		my_strccpy( buf2, buf );

		ch = buf2[0];
	}

	if( '\'' != ip_input() )
		yyerror( _( "bad char constant" ) );

	return( ch );
}


%}

%Start DOT
%Start BINARY 

%option noyywrap
%%
\/\* { 
	int ch;

	while( (ch = input()) != EOF )
		if( ch == '*' ) {
			if( (ch = input()) == '/' )
				break;
			else
				unput( ch );
		}
		else if( ch == '/' ) {
			if( (ch = input()) == '*' ) 
				yyerror( _( "nested comment" ) );
			else
				unput( ch );
		}

	if( ch == EOF )
		yyerror( _( "no end of comment" ) );
}
# |
(\/\/) { 
	int ch;

	/* Read string up to \n, EOF.
	 */
	while( (ch = input()) != EOF && ch != '\n' && ch != '\0')
		;
}

\#separator { BEGIN 0; return( TK_SEPARATOR ); }
\#dialog { BEGIN 0; return( TK_DIALOG ); }
class { BEGIN 0; return( TK_CLASS ); }
scope { BEGIN 0; return( TK_SCOPE ); }
char { BEGIN 0; return( TK_CHAR ); }
short { BEGIN 0; return( TK_SHORT ); }
int { BEGIN 0; return( TK_INT ); }
float { BEGIN 0; return( TK_FLOAT ); }
double { BEGIN 0; return( TK_DOUBLE ); }
signed { BEGIN 0; return( TK_SIGNED ); }
unsigned { BEGIN 0; return( TK_UNSIGNED ); }
complex	{ BEGIN 0; return( TK_COMPLEX ); }
if { BEGIN 0; return( TK_IF ); }
then { BEGIN 0; return( TK_THEN ); }
else { BEGIN 0; return( TK_ELSE ); }
\.\.\. { BEGIN 0; return( TK_DOTDOTDOT ); }
\.\. { BEGIN 0; return( TK_DOTDOTDOT ); }

true |
TRUE { 	
	BEGIN BINARY;

	yylval.yy_const.type = PARSE_CONST_BOOL;
	yylval.yy_const.val.bol = TRUE;

	return( TK_CONST );
}
false |
FALSE { 	
	BEGIN BINARY;

	yylval.yy_const.type = PARSE_CONST_BOOL;
	yylval.yy_const.val.bol = FALSE;

	return( TK_CONST );
}

<DOT>[a-zA-Z_][a-zA-Z0-9_']* {
	BEGIN BINARY;

	yylval.yy_name = im_strdupn( yytext );

	return( TK_TAG );
}
[a-zA-Z_][a-zA-Z0-9_']* {
	char *name = model_loadstate_rewrite_name( yytext );

	BEGIN BINARY;

	if( name ) {
		yylval.yy_name = im_strdupn( name );
		vips_buf_change( &lex_text, yytext, name );
	}
	else 
		yylval.yy_name = im_strdupn( yytext );

	return( TK_IDENT );
}
\$[a-zA-Z_][a-zA-Z0-9_']* {
	BEGIN BINARY;

	yylval.yy_const.type = PARSE_CONST_STR;
	yylval.yy_const.val.str = im_strdupn( yytext + 1 );

	return( TK_CONST );
}

\( { BEGIN 0; return( '(' ); }
\) { BEGIN BINARY; return( ')' ); }
\+\+ { BEGIN 0; return( TK_JOIN ); }
\-\- { BEGIN 0; return( TK_DIFF ); }
<BINARY>\+ |
<BINARY>\- { BEGIN 0; return( *yytext ); }
\- { BEGIN 0; return( TK_UMINUS ); }
\+ { BEGIN 0; return( TK_UPLUS ); }
\< { BEGIN 0; return( TK_LESS ); }
\<\= { BEGIN 0; return( TK_LESSEQ ); }
\> { BEGIN 0; return( TK_MORE ); }
\>\= { BEGIN 0; return( TK_MOREEQ ); }
\=\> { BEGIN 0; return( TK_TO ); }
\& { BEGIN 0; return( TK_BAND ); }
\&\& { BEGIN 0; return( TK_LAND ); }
\:\: { BEGIN 0; return( TK_SUCHTHAT ); }
\*\* { BEGIN 0; return( TK_POW ); }
\>\> { BEGIN 0; return( TK_RSHIFT ); }
\<\< { BEGIN 0; return( TK_LSHIFT ); }
\<\- { BEGIN 0; return( TK_FROM ); }
\| { BEGIN 0; return( TK_BOR ); }
\|\| { BEGIN 0; return( TK_LOR ); }
\=\= { BEGIN 0; return( TK_EQ ); }
\=\=\= { BEGIN 0; return( TK_PEQ ); }
\!\= { BEGIN 0; return( TK_NOTEQ ); }
\!\=\= { BEGIN 0; return( TK_PNOTEQ ); }
\\ { BEGIN 0; return( TK_LAMBDA ); }
\^ |
\? |
\* |
\/ |
\% |
\, |
\! |
\; |
\[ |
\: |
\= |
\~ |
\@ |
\{ |
\} { BEGIN 0; return( *yytext ); }
\. { BEGIN DOT; return( *yytext ); }
\] { BEGIN BINARY; return( *yytext ); }

0x[0-9a-fA-F]+ {
	unsigned int i;

	BEGIN BINARY;

	if( sscanf( yytext, "0x%x", &i ) != 1 )
		nip2yyerror( _( "bad number %s" ), yytext );

	yylval.yy_const.type = PARSE_CONST_NUM;
	yylval.yy_const.val.num = i;

	return( TK_CONST );
}
[0-9]*(\.[0-9]+)?([eE][+-]?[0-9]+)?[ij]? {
	double d;
	int ch;

	BEGIN BINARY;

	d = g_ascii_strtod( yytext, NULL );

	yylval.yy_const.type = PARSE_CONST_NUM;
	yylval.yy_const.val.num = d;

	ch = yytext[strlen( yytext ) - 1];
	if( ch == 'i' || ch == 'j' )
		yylval.yy_const.type = PARSE_CONST_COMPLEX;

	return( TK_CONST );
}

\' {
	BEGIN BINARY;

	yylval.yy_const.type = PARSE_CONST_CHAR;
	yylval.yy_const.val.ch = read_char();

	return( TK_CONST );
}
\" { 
	ModelLoadState *state = model_loadstate;

	char buf[MAX_STRSIZE];
	char buf2[MAX_STRSIZE];

	BEGIN BINARY;

	read_string( buf );

	/* We need to keep buf as exactly the string in the source, 
	 * including before interpretation of \ escapes, for the
	 * vips_buf_change() to work.
	 */
	my_strccpy( buf2, buf );

	if( state &&
		state->rewrite_path ) {
		char buf3[FILENAME_MAX];

		path_compact( buf2 );

		/* We've interpreted \n etc. in my_strccpy() above, plus we
		 * have nativised paths from / to \ and therefore introduced
		 * backslashes that weren't there before.
		 *
		 * Before we write source code out again, we must reescape
		 * everything.
		 */
		my_strecpy( buf3, buf2, TRUE );

		vips_buf_change( &lex_text, buf, buf3 );
	}

	if( strcmp( buf2, "" ) == 0 ) 
		yylval.yy_const.type = PARSE_CONST_ELIST;
	else {
		yylval.yy_const.type = PARSE_CONST_STR;
		yylval.yy_const.val.str = im_strdupn( buf2 );
	}

	return( TK_CONST );
}

[ \t\n\r\m\01] ;

. {
	nip2yyerror( _( "illegal character \"%c\"" ), *yytext );
}
