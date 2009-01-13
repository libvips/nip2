/* Types for the symbol table.
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

#define TYPE_SYMBOL (symbol_get_type())
#define SYMBOL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_SYMBOL, Symbol ))
#define SYMBOL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_SYMBOL, SymbolClass))
#define IS_SYMBOL( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_SYMBOL ))
#define IS_SYMBOL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_SYMBOL ))
#define SYMBOL_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_SYMBOL, SymbolClass ))

/* The types of symbol we can have.
 */
typedef enum {
	SYM_VALUE,		/* Symbol with a value attached */
	SYM_PARAM,		/* A parameter to a user function */
	SYM_ZOMBIE,		/* A referred to but not defined */
	SYM_WORKSPACE,		/* A loaded workspace */
	SYM_WORKSPACEGROUP,	/* A collection of workspaces */
	SYM_ROOT,		/* The root symbol */
	SYM_EXTERNAL,		/* A reference to an external function */
	SYM_BUILTIN		/* A reference to a built-in function */
} SymbolType;

/* A symbol.
 */
struct _Symbol {
	Filemodel parent_class;

	/* The type of this symbol.
	 */
	SymbolType type;

	/* Track during parse. A list of pointers to pointers to this
	 * symbol which we need to patch if we resolve to an outer scope.
	 */
	GSList *patch;

	/* Main expression for this sym. All expressions are icontainer
	 * children of us.
	 */
	Expr *expr;

	/* Base of graph for value of this symbol. Use sym->expr->root to get
	 * value though .. we just hold pointer for GC here. Expressions on
	 * ext_expr have their GC handled by their enclosing Subcolumn.
	 */
	Element base;		/* Value for this expr */

	/* Recomputation links. Use these to work out what to build next.
	 */
	gboolean dirty;		/* True if this sym needs recalc */
	GSList *parents;	/* Compiles which refer to this sym */

	GSList *topchildren;	/* For top syms, all top-level children */
	GSList *topparents;	/* For top syms, all top-level parents */
	int ndirtychildren;	/* Number of dirty top syms we refer to */
	gboolean leaf;		/* True for in recomp set */

	/* This is a generated symbol, like $$result, $$fn1, whatever.
	 */
	gboolean generated;

	/* A temporary intermediate symbol generated during parse to hold 
	 * stuff until we need it. Don't generate code for these.
	 */
	gboolean placeholder;

	/* X-tras for definitions.
	 */
	Tool *tool;		/* Tool and toolkit defined in */

	/* X-tras for SYM_EXTERNAL ... our im_function.
	 */
	im_function *function;	/* Function we run */
	int fn_nargs;		/* Number of args fn needs from nip */

	/* X-tras for SYM_BUILTIN ... our function.
	 */
	BuiltinInfo *builtin;

	/* For WORKSPACEGROUP ... the wsg we represent.
	 */
	Workspacegroup *wsg;

	/* For WORKSPACE ... the ws we represent.
	 */
	Workspace *ws;
};

typedef struct _SymbolClass {
	FilemodelClass parent_class;

	/* 

		new_value	sym->expr has a new value (this signal is
				fwd'd from sym->expr)

	 */

	void (*new_value)( Symbol *sym );
} SymbolClass;

GType symbol_get_type( void );

/* All symbols come off this.
 */
extern Symbol *symbol_root;

Symbol *symbol_map_all( Symbol *sym, symbol_map_fn fn, void *a, void *b );

Symbol *symbol_get_parent( Symbol *sym );
Workspace *symbol_get_workspace( Symbol *sym );
Tool *symbol_get_tool( Symbol *sym );
Symbol *symbol_get_scope( Symbol *sym );

void symbol_qualified_name( Symbol *sym, VipsBuf *buf );
void symbol_qualified_name_relative( Symbol *context, 
	Symbol *sym, VipsBuf *buf );
const char *symbol_name( Symbol *sym );
void symbol_name_print( Symbol *sym );
const char *symbol_name_scope( Symbol *sym );
void symbol_name_scope_print( Symbol *sym );

void symbol_new_value( Symbol *sym );

void *symbol_patch_add( void **pnt, Symbol *sym );

Symbol *symbol_root_init( void );

Symbol *symbol_new( Compile *compile, const char *name );
void symbol_error_redefine( Symbol *sym );
Symbol *symbol_new_defining( Compile *compile, const char *name );
Symbol *symbol_new_reference( Compile *compile, const char *name );
void symbol_made( Symbol *sym );

void symbol_not_defined( Symbol *sym );

void *symbol_link_break( Symbol *child, Compile *compile );

gboolean symbol_user_init( Symbol *sym );
gboolean symbol_parameter_init( Symbol *sym );
gboolean symbol_parameter_builtin_init( Symbol *sym );

gboolean symbol_busy( void );

void *symbol_sanity( Symbol *sym );
void symbol_leaf_set_sanity( void );
void *symbol_strip( Symbol *sym );

void symbol_state_change( Symbol *sym );

void *symbol_recalculate_check( Symbol *sym );
void symbol_recalculate_all_force( gboolean now );
void symbol_recalculate_all( void );
