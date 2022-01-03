/* Symbol classifiers.
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

/* Is it one of the system members? Hidden in menus and class display. We
 * can't safely use is_super()/is_this() (they are fast), because we can get
 * called during build (before they are working). Use strcmp() instead.
 */
gboolean
is_system( Symbol *sym )
{
	Symbol *parent = symbol_get_parent( sym );

	/* Something like $$lambda1 and friends.
	 */
	if( sym->generated )
		return( TRUE );

	if( strcmp( IOBJECT( sym )->name, MEMBER_CHECK ) == 0 ||
		strcmp( IOBJECT( sym )->name, MEMBER_NAME ) == 0 ||
		strcmp( IOBJECT( sym )->name, MEMBER_THIS ) == 0 ||
		IOBJECT( sym )->name[0] == '_' )
		return( TRUE );

	if( parent &&
		!is_scope( parent ) &&
		strcmp( IOBJECT( sym )->name, IOBJECT( parent )->name ) == 0 )
		return( TRUE );

	return( FALSE );
}

/* Something like "a = Separator;"
 */
gboolean
is_separator( Symbol *sym )
{
	if( sym->expr && 
		sym->expr->compile && 
		sym->expr->compile->tree && 
		sym->expr->compile->tree->type == NODE_LEAF ) {
		Symbol *leaf = sym->expr->compile->tree->leaf;

		return( strcmp( IOBJECT( leaf )->name, CLASS_SEPARATOR ) == 0 );
	}

	return( FALSE );
}

/* Is a symbol a class.
 */
gboolean
is_class( Compile *compile )
{
	return( compile->is_klass );
}

/* Is a sym the super member of some class.
 */
gboolean
is_super( Symbol *sym )
{
	Symbol *parent = symbol_get_parent( sym );
	Compile *parent_compile = parent->expr->compile;

	return( parent_compile && 
		is_class( parent_compile ) && 
		sym == parent_compile->super );
}

/* Is a sym the this member of some class.
 */
gboolean
is_this( Symbol *sym )
{
	Symbol *parent = symbol_get_parent( sym );
	Compile *parent_compile = parent->expr->compile;

	return( is_class( parent_compile ) && sym == parent_compile->this );
}

/* Is sym a member of an enclosing class of compile.
 */
gboolean
is_member_enclosing( Compile *compile, Symbol *sym )
{
	for( compile = compile_get_parent( compile ); compile; 
		compile = compile_get_parent( compile ) ) 
		if( is_class( compile ) && compile->sym != sym &&
			ICONTAINER( sym )->parent == ICONTAINER( compile ) ) 
			return( TRUE );

	return( FALSE );
}

/* Is a symbol a compile-time scope (eg. workspace)
 */
gboolean
is_scope( Symbol *sym )
{
	return( sym->type == SYM_ROOT || 
		sym->type == SYM_WORKSPACE || 
		sym->type == SYM_WORKSPACEROOT || 
		!symbol_get_parent( sym ) );
}

/* Is a symbol a top-level definition. Tops are symbols whose parents are 
 * SYM_ROOT, SYM_WORKSPACE and friends.
 */
gboolean
is_top( Symbol *sym )
{
	if( is_scope( sym ) || is_scope( symbol_get_parent( sym ) ) )
		return( TRUE );

	return( FALSE );
}

/* Is a symbol a member of a class? Params don't count.
 */
gboolean
is_member( Symbol *sym )
{
 	return( is_value( sym ) && 
		is_class( COMPILE( ICONTAINER( sym )->parent ) ) ); 
}

/* Is a compile a member function (not a sub-class)?
 */
gboolean
is_memberfunc( Compile *compile )
{
 	return( is_class( compile_get_parent( compile ) ) && 
		!is_class( compile ) );
}

/* Something that ought to have a value.
 */
gboolean
is_value( Symbol *sym )
{
 	return( sym->type == SYM_VALUE && sym->expr );
}

/* Is sym an ancestor of context?
 */
gboolean
is_ancestor( Symbol *context, Symbol *sym )
{
	if( context == sym )
		return( TRUE );

	if( context == symbol_root ) 
		return( FALSE );

	return( is_ancestor( symbol_get_parent( context ), sym ) );
}

gboolean 
is_menuable( Symbol *sym )
{
	/* In a hidden kit?
	 */
	if( sym->tool && IOBJECT( sym->tool->kit )->name[0] == '_' )
		return( FALSE );

	/* A hidden item?
	 */
	if( IOBJECT( sym )->name[0] == '_' )
		return( FALSE );

	/* We also hide all supers, system things 
	 */
	if( !is_value( sym ) || !sym->expr->compile || is_system( sym ) ||
		strcmp( IOBJECT( sym )->name, MEMBER_SUPER ) == 0 )
		return( FALSE );

	return( TRUE );
}
