/* Links between top-level syms and the exprs which reference them
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

/* A sub-link ... the expr in parent that actually references child, plus
 * the number of times it makes the reference.
 */
struct _LinkExpr {
	Link *link;		/* Link we are part of */

	Expr *expr;		/* Expr that references child */
	int count;		/* Number of times expr references child */
	gboolean dynamic;	/* True for dynamic link */
};

/* A link object!
 */
struct _Link {
	Symbol *parent;		/* This top-level symbol contains exprs ... */
	Symbol *child;		/* ... which reference this symbol */

	/* Link serial number ... when we walk the symbol graph marking
	 * stuff dirty, use this to stop repeat trips along links, and
	 * avoid getting stuck in cycles.
	 */
	int serial;

	/* The expressions inside parent which contain direct references to
	 * child. If parent is in the tally, there can be lots of these.
	 *
	 * Two sort of links: static links, which we can deduce from
	 * compile-time analysis of the expr and which only change when
	 * the user edits and we recompile, and dynamic links which we
	 * clear when we regenerate the heap image of the function, and add
	 * to during evaluation.
	 */
	GSList *static_links;
	GSList *dynamic_links;
};

void *link_expr_destroy( LinkExpr *le );
void *link_destroy( Link *link );
void *link_add( Symbol *child, Expr *expr, gboolean dynamic );
void *link_remove( Symbol *child, Expr *expr, gboolean dynamic );

int symbol_ndirty( Symbol *sym );
void *symbol_fix_counts( Symbol *sym );
void symbol_link_destroy( Symbol *sym );
void symbol_link_build( Symbol *sym );

int link_serial_new( void );
void *symbol_dirty_intrans( Symbol *sym, int serial );
void *symbol_dirty( Symbol *sym, int serial );
void *symbol_dirty_total( Symbol *sym, int serial );

void *symbol_dirty_clear( Symbol *sym );
