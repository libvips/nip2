/* Global variables from parse.y.
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

/* Our input stream can be attached to either a string or a FILE. 
 * Keep track of the state of play here.
 */
typedef struct {
	iOpenFile *of;		/* Non-NULL if we read from a file */
	char *str;		/* Non-NULL if we read from a string */
	char *strpos;		/* Position in string */

	char buf[MAX_STRSIZE];	/* Accumulate text of each definition here */
	int bwp;		/* Write point in the above */
	int bsp[MAX_SSTACK];	/* Start point stack */
	int bspsp;		/* Stack pointer */

	int lineno;		/* Current line number */
	int charno;		/* Character in line */
	int pcharno;		/* Characters in previous line */
	int charpos;		/* Characters read by lex so far */

	int oldchar;            /* unget buffer, -1 for no unget */
} InputState;

extern InputState input_state;

/* Function declarations for parse.y.
 */
void yyerror( const char *sub, ... )
	__attribute__((format(printf, 1, 2)));
extern int yyleng;			/* lex stuff */

/* Lex gathers tokens here for workspace.c
 */
extern BufInfo lex_text;

/* Attach input for lex.
 */
void attach_input_file( iOpenFile *of );
void attach_input_string( const char *str );
int ip_input( void );
void ip_unput( int ch );
void ip_unget( void );
gboolean is_EOF( void );

/* Parse stuff.
 */

/* Order and number important ... see table in parse_rhs()
 */
typedef enum {
	PARSE_RHS = 0,		/* eg. "a + b" */
	PARSE_PARAMS,		/* eg. "a b = a + b" */
	PARSE_SUPER 		/* eg. "fred c d" */
} ParseRhsSyntax;

extern Symbol *last_top_sym;

gboolean parse_toplevel( Toolkit *kit, int pos );
gboolean parse_onedef( Toolkit *kit, int pos );
gboolean parse_rhs( Expr *expr, ParseRhsSyntax syntax );

void free_lex( int yychar );
char *parse_test_define( void );
Symbol *parse_set_symbol( void );
