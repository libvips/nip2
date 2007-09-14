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

/* 
#define DEBUG
 */

/* Just show secrets we added
#define DEBUG_ADD
 */

#include "ip.h"

/* build secret sets for exprs

cases:

	fred a 
		= jim 12
	{
		jim b = a + b;
	}

jim refers to a parameter in an enclosing scope ... we add extra secret
parameters to jim like this:

	fred a 
		= jim [a] 12
	{
		jim [a] b = a + b;
	}

across class boundaries:

	fred a
		= jim
	{
		jim = class {
			b = a;
		}
	}

now fred.jim.b refers to fred.a ... a needs to be added to the secrets on
jim's constructor like this:

	fred a
		= jim [a]
	{
		jim [a] = class {
			b = a;
		}
	}

if the secret is a class member, pass "this" instead and the inner thing then
gets from that

	fred a = class
	{
		jim [fred.this] b = fred.this.a + b;
	}

if the inner thing is also a class, need to get in two stages ... first get
the right this, then get from that

	fred a = class
	{
		jim [fred.this] = class {
			b = jim.this.fred.this.a;
		}
	}

need to work for any sort of nesting of functions and classes

	fred = class {
		b = c
		{
			c = this;
		}
	}

not just params ... can involve locals of parents

 */

/* Add a secret. Set changed if we make a change.
 */
static void *
secret_add( Symbol *secret, Compile *compile, gboolean *changed )
{
	Compile *parent = compile_get_parent( compile );

#ifdef DEBUG
	printf( "secret_add: considering secret " );
	symbol_name_print( secret );
	printf( " for " );
	compile_name_print( compile );
	printf( " ...\n" );
#endif /*DEBUG*/

	/* If expr is a class, don't add our own this.
	 */
	if( is_class( compile ) && secret == compile->this )
		return( NULL );

	/* If expr already has secret as a param or secret, don't add again.
	 */
	if( g_slist_find( compile->secret, secret ) ||
		g_slist_find( compile->param, secret ) )
		return( NULL );

	/* If secret is a member (param, func, whatever), add secret's
	 * enclosing "this" instead ... expr can then get secret from there.
	 * Unless the secret is already a "this", of course.
	 */
	if( is_class( COMPILE( ICONTAINER( secret )->parent ) ) &&
		!is_this( secret ) )
		secret = COMPILE( ICONTAINER( secret )->parent )->this;

	/* If compile is a member (and not a class itself), add the secret to 
	 * compile's constructor instead ... compile can get from "this".
	 */
	if( is_class( parent ) && secret != parent->this && 
		!is_class( compile ) ) 
		compile = parent;

	/* We may have moved stuff about ... check for dupes again.
	 */
	if( g_slist_find( compile->secret, secret ) ||
		g_slist_find( compile->param, secret ) )
		return( NULL );

#ifdef DEBUG_ADD
	printf( "secret_add: adding secret " );
	symbol_name_print( secret );
	printf( " to " );
	compile_name_print( compile );
	printf( "\n" );
#endif /*DEBUG_ADD*/

	compile->secret = g_slist_append( compile->secret, secret );
	compile->nsecret += 1;
	*changed = TRUE;

	return( NULL );
}

/* If compile is a member, then secret lists are easy ... just use "this".
 */
static void *
secret_set_class( Compile *compile ) 
{
	if( is_class( compile_get_parent( compile ) ) ) {
		Compile *parent = compile_get_parent( compile );
		Symbol *ths = parent->this;
		gboolean changed;

		if( secret_add( ths, compile, &changed ) )
			return( (void *) ths );
	}

	return( NULL );
}

/* child is one of compile's children ...  is it reference to a parameter 
 * in an enclosing scope? If yes, we've found a secret!
 */
static void *
secret_is_nonlocal( Symbol *child, Compile *compile )
{
	gboolean changed;

	if( child->type == SYM_PARAM && 
		COMPILE( ICONTAINER( child )->parent ) != compile ) {
		if( secret_add( child, compile, &changed ) )
			return( child );
	}

	return( NULL );
}

/* Make initial secret list ... if this is a member/function, search for 
 * references to symbols in an enclosing scope.
 */
static void *
secret_find_nonlocal( Compile *compile )
{
	/* Look for any secrets.
	 */
	if( slist_map( compile->children,
		(SListMapFn) secret_is_nonlocal, compile ) )
		return( compile );

	return( NULL );
}

/* Does child have any secrets that compile does not?
 */
static void *
secret_test( Symbol *child, Compile *compile, gboolean *changed )
{
	/* If this is a parameter or a zombie, nothing to do.
	 */
	if( !is_value( child ) )
		return( NULL );

	if( child->expr->compile )
		if( slist_map2( child->expr->compile->secret, 
			(SListMap2Fn) secret_add, compile, changed ) )
			return( child );

	return( NULL );
}

/* Close secret list ... if sym has a child with a secret sym does not have,
 * sym needs child's secret too.
 */
static void *
secret_close( Compile *compile, gboolean *changed )
{
	if( is_class( compile ) ) {
		/* For classes, need to consider all of their locals. Any
		 * secrets our locals have, we need too.
		 */
		if( icontainer_map( ICONTAINER( compile ),
			(icontainer_map_fn) secret_test, compile, changed ) )
			return( compile );
	}
	else {
		/* Look at our immediate children, any of them have secrets 
		 * we don't?
		 */
		if( slist_map2( compile->children,
			(SListMap2Fn) secret_test, compile, changed ) )
			return( compile );
	}

	return( NULL );
}

#ifdef DEBUG
/* Sub-fn of below ... add param as a secret to sym.
 */
static void *
secret_all_add( Compile *compile, Symbol *param )
{
	gboolean changed;

	return( secret_add( param, compile, &changed ) );
}

/* Sub-fn of below ... add param as a secret for all of compile's locals.
 */
static void *
secret_all_sym( Symbol *param, Compile *compile )
{
	return( compile_map_all( compile,
		(map_compile_fn) secret_all_add, param ) );
}

/* Make syms params and secrets secrets for all sub-syms. Only handy for
 * debugging.
 */
static void *
secret_all( Compile *compile ) 
{
	if( slist_map( compile->param,
		(SListMapFn) secret_all_sym, compile ) ||
		slist_map( compile->secret, 
			(SListMapFn) secret_all_sym, compile ) )
		return( compile );

	return( NULL );
}
#endif /*DEBUG*/

/* Make secret param lists for compile and all of it's sub-defs.
 */
void
secret_build( Compile *compile )
{
	gboolean changed;

#ifdef DEBUG_ADD
	printf( "secret_build: " );
	symbol_name_print( compile->sym );
	printf( "\n" );
#endif /*DEBUG_ADD*/

	/* Look for class definitions ... all members of a
	 * class should take a single secret, their "this" parameter. 
	 * When they in turn call their locals, they can get the 
	 * secrets they need from "this".
	 */
	(void) compile_map_all( compile, 
		(map_compile_fn) secret_set_class, NULL );

	/* Now look for non-member functions ... if they reference 
	 * parameters in an enclosing scope, add that parameter to 
	 * the secret list.
	 */
	(void) compile_map_all( compile, 
		(map_compile_fn) secret_find_nonlocal, NULL );

	/* Now take the closure of the secret lists ... have to 
	 * fix() this to get limit of secret_close().
	 */
	do {
		changed = FALSE;
		(void) compile_map_all( compile,
			(map_compile_fn) secret_close, &changed );
	} while( changed ); 
}

