/* Class functions ... really part of heap.c, but split out here to make it
 * more manageable.
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
#define DEBUG_MEMBER
#define DEBUG_VERBOSE
#define DEBUG
#define DEBUG_BUILD
 */

static gboolean
class_is_class( PElement *instance )
{
	if( !PEISCLASS( instance ) ) {
		char txt[50];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		if( !itext_value( reduce_context, &buf, instance ) )
			return( FALSE );
		error_top( _( "Bad argument." ) );
		error_sub( _( "Object %s is not a class." ), 
			vips_buf_all( &buf ) );

		return( FALSE );
	}

	return( TRUE );
}

Compile *
class_get_compile( PElement *instance )
{
	if( !class_is_class( instance ) )
		return( NULL );

	return( PEGETCLASSCOMPILE( instance ) );
}

/* Look up "super" in a class ... try to do it quickly.
 */
gboolean
class_get_super( PElement *instance, PElement *out )
{
	Compile *compile;

	if( !(compile = class_get_compile( instance )) )
		return( FALSE );
	g_assert( compile->super );

	return( class_get_symbol( instance, compile->super, out ) );
}

void *
class_map( PElement *instance, class_map_fn fn, void *a, void *b )
{
	PElement member;
	HeapNode *p;

	if( !PEISCLASS( instance ) ) 
		return( NULL );

	/* Loop over the instance member list.
	 */
	PEGETCLASSMEMBER( &member, instance );
	if( !PEISELIST( &member ) )
		for( p = PEGETVAL( &member ); p; p = GETRIGHT( p ) ) {
			HeapNode *hn;
			PElement s, v;
			Symbol *sym;
			void *result;

			/* Get the sym/value pair, get the sym.
			 */
			hn = GETLEFT( p );
			PEPOINTLEFT( hn, &s );
			PEPOINTRIGHT( hn, &v );
			sym = PEGETSYMREF( &s );

			if( (result = fn( sym, &v, a, b )) )
				return( result );

		}

	return( NULL );
}

/* Look up a member in a class instance by name. If lookup fails in this 
 * instance, try the superclass. Don't search secrets. Point sym and value 
 * at the symbol we found and its value. sym can be NULL for no result
 * required.
 */
gboolean
class_get_member( PElement *instance, const char *name, 
	Symbol **sym_out, PElement *out )
{
	PElement member;
	PElement super;
	HeapNode *p;

#ifdef DEBUG_MEMBER
	printf( "class_get_member: looking up \"%s\" in class ", name );
	pgraph( instance );
#endif /*DEBUG_MEMBER*/

	if( !class_is_class( instance ) )
		return( FALSE );

	/* Search this instance member list.
	 */
	PEGETCLASSMEMBER( &member, instance );
	if( !PEISELIST( &member ) )
		for( p = PEGETVAL( &member ); p; p = GETRIGHT( p ) ) {
			HeapNode *hn;
			PElement s;
			Symbol *sym;

			/* Get the sym/value pair, get the sym.
			 */
			hn = GETLEFT( p );
			PEPOINTLEFT( hn, &s );

			/* Match? 
			 */
			sym = PEGETSYMREF( &s );
			if( strcmp( IOBJECT( sym )->name, name ) == 0 ) {
				/* Found!
				 */
				PEPOINTRIGHT( hn, out );
				if( sym_out )
					*sym_out = sym;

#ifdef DEBUG_MEMBER
				printf( "class_get_member: found: " );
				pgraph( out );
#endif /*DEBUG_MEMBER*/

				return( TRUE );
			}
		}

	/* Nope ... try the superclass.
	 */
	if( !class_get_super( instance, &super ) || !PEISELIST( &super ) ) {
		/*

			FIXME ... gcc 2.95.2 gets this wrong, tries to
				  eliminate the tail recursion with -O2
				  and makes bad code
			      ... guess how long that took to find
			      ... put this back at some point

		return( class_get_member( &super, name, sym_out, value ) ); 
		 */
		gboolean result = class_get_member( &super, name, 
			sym_out, out ); 

		return( result );
	}

	error_top( _( "Member not found." ) );
	error_sub( _( "Member \"%s\" not found in class \"%s\"." ),
		name, IOBJECT( PEGETCLASSCOMPILE( instance )->sym )->name );

	return( FALSE );
}

/* Look up a symbol in a class. Write to out, or FALSE for not found. Look up 
 * by symbol pointer. Search secrets as well. Try the superclass if lookup 
 * fails.
 */
gboolean
class_get_symbol( PElement *instance, Symbol *sym, PElement *out )
{
	HeapNode *p;
	PElement secret;
	PElement super;

#ifdef DEBUG_MEMBER
	printf( "class_get_symbol: looking up " );
	symbol_name_print( sym );
	printf( "in class " );
	pgraph( instance );
#endif /*DEBUG_MEMBER*/

	if( !class_is_class( instance ) )
		return( FALSE );

	PEGETCLASSSECRET( &secret, instance );
	if( PEISNODE( &secret ) )
		for( p = PEGETVAL( &secret ); p; p = GETRIGHT( p ) ) {
			PElement s;
			HeapNode *hn;

			/* Get the sym/value pair, get the sym.
			 */
			hn = GETLEFT( p );
			PEPOINTLEFT( hn, &s );

			/* Match?
			 */
			if( PEGETSYMREF( &s ) == sym ) {
				/* Found!
				 */
				PEPOINTRIGHT( hn, out );

#ifdef DEBUG_MEMBER
				printf( "class_get_symbol: found: " );
				pgraph( out );
#endif /*DEBUG_MEMBER*/

				return( TRUE );
			}
		}

	/* Nope ... try the superclass.
	 */
	if( !class_get_super( instance, &super ) || !PEISELIST( &super ) ) {
		/*

			FIXME ... gcc 2.95.2 gets this wrong, tries to
				  eliminate the tail recursion with -O2
				  and makes bad code
			      ... guess how long that took to find
			      ... put this back at some point

		return( class_get_member( &super, name, out ) ); 
		 */
		gboolean result = class_get_symbol( &super, sym, out ); 

		return( result );
	}

	return( FALSE );
}

/* Search back up the inheritance tree for an exact instance of this
 * class.
 */
gboolean
class_get_exact( PElement *instance, const char *name, PElement *out )
{
	PElement pe;

	pe = *instance;
	while( !reduce_is_instanceof_exact( reduce_context, name, &pe ) ) {
		if( !class_get_super( &pe, &pe ) || PEISELIST( &pe ) ) 
			return( FALSE );
	}

	*out = pe;

	return( TRUE );
}

/* Stuff we need for class build.
 */
typedef struct {
	Heap *heap;		/* Heap to build on */
	Symbol *sym;		/* Sym we are local to */
	PElement *arg;		/* Args to constructor */
	PElement *this;		/* Base of instance we are building */
	int i;			/* Index in arg list */
	Compile *compile;	/* Compile for our class */
} ClassBuildInfo;

/* Member sym of class pbi->sym needs secret as an argument ... add it!
 */
static gboolean
class_member_secret( ClassBuildInfo *pbi, 
	Symbol *sym, GSList *secret, PElement *out )
{
	Symbol *ssym;
	Heap *heap = pbi->heap;
	HeapNode *apl;

	if( !secret )
		return( TRUE );
	ssym = SYMBOL( secret->data );

	/* Make function application for this member.
	 */
	if( NEWNODE( heap, apl ) )
		return( FALSE );
	apl->type = TAG_APPL;
	PEPUTLEFT( apl, out );

	/* Is the secret "this"? Easy.
	 */
	if( ssym == pbi->sym->expr->compile->this ) {
		PEPUTRIGHT( apl, pbi->this );
	}
	else {
		/* Look up ssym in pbi->sym's secrets ... should be there 
		 * somewhere. Use it's index to find the pbi->arg[] we need.
		 */
		int pos = g_slist_index( 
			pbi->sym->expr->compile->secret, ssym );

		/* FIXME ... may not be if we've regenerated one of these
		 * stupid things :-( change this so we always go through 
		 * 'this'.
		 */
		if( pos < 0 || pos >= pbi->sym->expr->compile->nsecret ) {
			error_top( _( "No such secret." ) );
			error_sub( _( "Editing local classes which reference "
				"non-local objects is a bit broken at the "
				"moment :-(" ) );
			return( FALSE );
		}

		PEPUTRIGHT( apl, 
			&pbi->arg[pbi->sym->expr->compile->nsecret - pos - 1] );
	}

	PEPUTP( out, ELEMENT_NODE, apl );

#ifdef DEBUG_VERBOSE
{
	PElement p1;
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	PEPOINTRIGHT( apl, &p1 );
	graph_pelement( pbi->heap, &buf, &p1, TRUE );
	printf( "class_member_secret: secret arg " );
	symbol_name_print( ssym );
	printf( "to member " );
	symbol_name_print( sym );
	printf( "= %s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG_VERBOSE*/

	return( class_member_secret( pbi, sym, secret->next, out ) );
}

/* Add a member to a class.
 */
static void *
add_class_member( Symbol *sym, ClassBuildInfo *pbi, PElement *out )
{
	Heap *heap = pbi->heap;
	HeapNode *base, *sv;
	PElement v;

	/* Is this something that should be part of a class.
	 */
	if( sym->type != SYM_VALUE )
		return( NULL );

	/* Make new class-local-list element for this local.
	 */
	if( NEWNODE( heap, base ) )
		return( sym );
	base->type = TAG_CONS;
	PPUTLEFT( base, ELEMENT_ELIST, NULL );
	PEPUTRIGHT( base, out );
	PEPUTP( out, ELEMENT_NODE, base );

	/* Make sym/value pair for this local.
	 */
	if( NEWNODE( heap, sv ) )
		return( sym );
	sv->type = TAG_CONS;
	PPUT( sv, ELEMENT_SYMREF, sym, ELEMENT_SYMBOL, sym ); 
	PPUTLEFT( base, ELEMENT_NODE, sv );

	/* Build value ... apply args to the symbol.
	 */
	PEPOINTRIGHT( sv, &v );
	if( !class_member_secret( pbi, sym, sym->expr->compile->secret, &v ) )
		return( sym );

#ifdef DEBUG_VERBOSE
{
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_pelement( heap, &buf, &v, TRUE );
	printf( "add_class_member: member \"%s\" of class \"%s\" = %s\n",
		IOBJECT( sym )->name, IOBJECT( pbi->sym )->name, 
		vips_buf_all( &buf ) );
}
#endif /*DEBUG_VERBOSE*/

	return( NULL );
}

/* Add a symbol/value pair to a class.
 */
static gboolean
add_class_svpair( ClassBuildInfo *pbi, 
	Symbol *sym, PElement *val, PElement *out )
{
	Heap *heap = pbi->heap;
	HeapNode *base, *sv;

#ifdef DEBUG_VERBOSE
{
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_pelement( heap, &buf, val, TRUE );
	printf( "add_class_svpair: adding parameter \"%s\" to class "
		"\"%s\" = %s\n", 
		IOBJECT( sym )->name, IOBJECT( pbi->sym )->name,
		vips_buf_all( &buf ) );
}
#endif /*DEBUG_VERBOSE*/

	/* Make new class-local-list element for this parameter.
	 */
	if( NEWNODE( heap, base ) )
		return( FALSE );
	base->type = TAG_CONS;
	PPUTLEFT( base, ELEMENT_ELIST, NULL );
	PEPUTRIGHT( base, out );
	PEPUTP( out, ELEMENT_NODE, base );

	/* Make sym/value pair for this parameter.
	 */
	if( NEWNODE( heap, sv ) )
		return( FALSE );
	sv->type = TAG_CONS;
	PPUTLEFT( sv, ELEMENT_SYMREF, sym )
	PEPUTRIGHT( sv, val );
	PPUTLEFT( base, ELEMENT_NODE, sv );

	return( TRUE );
}

/* Add a parameter (secret or real) to a class.
 */
static void *
add_class_parameter( Symbol *sym, ClassBuildInfo *pbi, PElement *out )
{
	/* Add this symbol/value pair.
	 */
	if( !add_class_svpair( pbi, sym, &pbi->arg[pbi->i], out ) )
		return( sym );

	/* Move arg index on.
	 */
	pbi->i += 1;

	return( NULL );
}

/* Add the name member ... build the name string carefully.
 */
static void *
class_new_single_name( Heap *heap, PElement *pe,
	ClassBuildInfo *pbi, PElement *instance )
{
	Symbol *snm = compile_lookup( pbi->compile, MEMBER_NAME );
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	/* Make class name string.
	 */
	symbol_qualified_name( pbi->sym, &buf );
	PEPUTP( pe, ELEMENT_ELIST, NULL );
	if( !heap_managedstring_new( heap, vips_buf_all( &buf ), pe ) ) 
		return( heap );

	/* Add as a member.
	 */
	if( !add_class_svpair( pbi, snm, pe, instance ) ) 
		return( heap );

	return( NULL );
}

/* Make a single level class instance ... fn below then loops over a class
 * hierarchy with this.
 */
static gboolean
class_new_single( Heap *heap, 
	Compile *compile, PElement *arg, PElement *this, PElement *out )
{
	Symbol *sym = compile->sym;
	Symbol *sths = compile->this;

	HeapNode *base, *sm;
	PElement p1;
	ClassBuildInfo pbi;

#ifdef DEBUG
{
	int i;

	printf( "class_new_single: starting for " );
	symbol_name_print( sym );
	printf( "%d secrets, %d params\n", 
		compile->nsecret, compile->nparam );

	for( i = 0; i < compile->nsecret; i++ ) {
		char txt[256];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		graph_pelement( heap, &buf, &arg[i], TRUE );
		printf( "\tsecret %2d = %s\n", i, vips_buf_all( &buf ) );
	}

	for( i = 0; i < compile->nparam; i++ ) {
		char txt[256];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		graph_pelement( heap, &buf, &arg[i + compile->nsecret], TRUE );
		printf( "\targ %2d = %s\n", i, vips_buf_all( &buf ) );
	}
}
#endif /*DEBUG*/

	/* Make class base.
	 */
	if( NEWNODE( heap, base ) )
		return( FALSE );
	base->type = TAG_CLASS;
	PPUT( base, ELEMENT_COMPILEREF, compile, ELEMENT_ELIST, NULL ); 
	PEPUTP( out, ELEMENT_NODE, base );

	/* Make node for holding secrets and members.
	 */
	if( NEWNODE( heap, sm ) )
		return( FALSE );
	sm->type = TAG_CONS;
	PPUT( sm, ELEMENT_ELIST, NULL, ELEMENT_ELIST, NULL ); 
	PPUTRIGHT( base, ELEMENT_NODE, sm );

	/* Build list of members.
	 */
	pbi.heap = heap;
	pbi.sym = sym;
	pbi.arg = arg;
	pbi.this = this;
	pbi.compile = compile;
	PEPOINTRIGHT( sm, &p1 );
	if( icontainer_map_rev( ICONTAINER( compile ), 
		(icontainer_map_fn) add_class_member, &pbi, &p1 ) ) 
		return( FALSE );

	/* Add name member.
	 */
	if( heap_safe_pointer( heap,
		(heap_safe_pointer_fn) class_new_single_name, 
		&pbi, &p1, NULL, NULL ) )
		return( FALSE );

	/* Add this member.
	 */
	if( !add_class_svpair( &pbi, sths, this, &p1 ) )
		return( FALSE );

	/* Add class parameters to member list.
	 */
	pbi.i = 0;
	if( slist_map2_rev( compile->param, 
		(SListMap2Fn) add_class_parameter, &pbi, &p1 ) )
		return( FALSE );

	/* Now ... secret list starts off pointing to head of member list.
	 */
	PEPUTLEFT( sm, &p1 );

	/* Add all secret parameters to secret list.
	 */
	PEPOINTLEFT( sm, &p1 );
	if( slist_map2_rev( compile->secret, 
		(SListMap2Fn) add_class_parameter, &pbi, &p1 ) )
		return( FALSE );

#ifdef DEBUG
{
	char txt[256];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_pelement( heap, &buf, out, TRUE );
	printf( "class_new_single: built instance of " );
	symbol_name_print( sym );
	printf( ":\n%s\n", vips_buf_all( &buf ) );
}
#endif /*DEBUG*/

	return( TRUE );
}

/* Look at a scrap of graph and try to find a constructor it might be using.
 * This will only work for really basic functions :-( but it's enough to allow
 * us to pass extra secrets through the superclass. Used by (eg.) Colour when
 * it overrides Value and adds the colourspace arg.
 */
static Compile *
class_guess_constructor( PElement *fn )
{
	if( PEISCONSTRUCTOR( fn ) ) 
		return( PEGETCOMPILE( fn ) );
	else if( PEISNODE( fn ) ) {
		HeapNode *hn = PEGETVAL( fn );

		if( hn->type == TAG_APPL ) {
			PElement left;

			PEPOINTLEFT( hn, &left );

			return( class_guess_constructor( &left ) );
		}
	}

	return( NULL );
}

/* Look at arg0 and try to extract the arguments (all the RHS of the @ nodes).
 * Return the number of args we found, or -1 if we find crazy stuff.
 */
static int
class_guess_args( PElement arg[], PElement *fn )
{
	if( PEISCONSTRUCTOR( fn ) )
		return( 0 );
	else if( PEISNODE( fn ) ) {
		PElement left;
		int i;

		PEPOINTLEFT( PEGETVAL( fn ), &left );
		if( (i = class_guess_args( arg, &left )) == -1 )
			return( -1 );
		if( i >= MAX_SYSTEM ) {
			error_top( _( "Too many arguments." ) );
			error_sub( _( "You can't have more than %d "
				"arguments to a superclass constructor." ),
				MAX_SYSTEM );

			return( -1 );
		}

		PEPOINTRIGHT( PEGETVAL( fn ), &arg[i] );

		return( i + 1 );
	}
	else
		return( -1 );
}

static void *
class_new_super_sub( Heap *heap, PElement *p1,
	Compile *compile, PElement *arg, PElement *this, PElement *super )
{
	/* Build the superclass ... we overwrite the super 
	 * list with the constructed class, so make a copy of 
	 * the pointer to stop it being GCed.
	 */
	PEPUTPE( p1, super );

	if( !class_new_single( heap, compile, arg, this, super ) ) 
		return( heap );

	return( NULL );
}

/* Clone a class instance. Copy pointers to the the args, secrets and super; 
 * rebuild with the specified "this". Instance and out can be equal.
 */
static gboolean
class_clone_super( Heap *heap, Compile *compile,
	PElement *instance, PElement *this, PElement *out )
{
	PElement arg[MAX_SYSTEM];
	const int nargs = compile->nsecret + compile->nparam;
	PElement secret;
	int i;

	g_assert( nargs <= MAX_SYSTEM );

#ifdef DEBUG_VERBOSE
{
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_pelement( heap, &buf, instance, TRUE );
	printf( "class_new_clone: about to clone \"%s\": %s\n", 
		IOBJECT( compile->sym )->name, vips_buf_all( &buf ) );
}
#endif /*DEBUG_VERBOSE*/

	/* Pull out values of secrets and class args into arg[].
	 */
	PEGETCLASSSECRET( &secret, instance );
	for( i = 0; i < nargs; i++ ) {
		HeapNode *hn = PEGETVAL( &secret );
		HeapNode *sv = GETLEFT( hn );
		int index = nargs - i - 1;

		PEPOINTRIGHT( sv, &arg[index] );
		PEPOINTRIGHT( hn, &secret );
	}

	/* Build class again.
	 */
	return( class_new_single( heap, compile, arg, this, out ) );
}

static void *
class_clone_super_sub( Heap *heap, PElement *p1,
	Compile *compile, PElement *instance, PElement *this, PElement *out )
{
	/* instance and out can point to the same node, so save a pointer to  
	 * instance to stop it being GCed.
	 */
	PEPUTPE( p1, instance );

	if( !class_clone_super( heap, compile, instance, this, out ) ) 
		return( heap );

	return( NULL );
}

/* Does this class have a "super"? Build it and recurse.
 */
gboolean
class_new_super( Heap *heap, 
	Compile *compile, PElement *this, PElement *instance )
{
	PElement super;

	if( compile->has_super && class_get_super( instance, &super ) ) {
		Compile *super_compile;
		int len, fn_len;
		PElement arg0;

		/* It must be a list whose first element is the superclass
		 * constructor, or a partially parameterised constructor, or 
		 * the superclass itself (if it has already
		 * been constructed, or has no args). Other elements in the
		 * list are the remaining args.
		 *
		 * We keep the list form, since we want to not build the
		 * superclass until now if we can help it ... otherwise we
		 * have to construct once, then construct again when we clone.
		 */
		if( (len = heap_list_length( &super )) < 1 ||
			!heap_list_index( &super, 0, &arg0 ) ||
			!heap_reduce_strict( &arg0 ) )
			return( FALSE );

		if( (super_compile = class_guess_constructor( &arg0 )) ) {
			PElement fn_arg[MAX_SYSTEM];
			PElement arg[MAX_SYSTEM];
			int i;

			/* How many function args are there? 
			 */
			if( (fn_len = class_guess_args( fn_arg, &arg0 )) < 0 )
				return( FALSE );

			/* Check total arg count.
			 */
			if( super_compile->nsecret != 0 ) {
				char txt[1024];
				VipsBuf buf = VIPS_BUF_STATIC( txt );

				slist_map2( super_compile->secret, 
					(SListMap2Fn) symbol_name_error, 
					&buf, NULL );

				error_top( _( "Bad superclass." ) );
				error_sub( _( "Superclass constructor \"%s\" "
					"refers to non-local symbols %s" ),
					symbol_name( super_compile->sym ),
					vips_buf_all( &buf ) );

				return( FALSE );
			}
			if( len - 1 + fn_len != super_compile->nparam ) {
				error_top( _( "Wrong number of arguments." ) );
				error_sub( _( "Superclass constructor \"%s\" "
					"expects %d arguments, not %d." ),
					symbol_name( super_compile->sym ),
					super_compile->nparam,
					len - 1 + fn_len );

				return( FALSE );
			}

			/* Grab the explicit args from the super list.
			 */
			for( i = 0; i < len - 1; i++ ) {
				if( !heap_list_index( &super, len - 1 - i, 
					&arg[i] ) ) 
					return( FALSE );
			}

			/* Append the function args, but reverse them as we
			 * go so we get most-nested arg last.
			 */
			for( i = 0; i < fn_len; i++ )
				arg[i + len - 1] = fn_arg[fn_len - 1 - i];

			/* Build the superclass ... we overwrite the super 
			 * list with the constructed class, so make a copy of 
			 * the pointer to stop it being GCed.
			 */
			if( heap_safe_pointer( heap, 
				(heap_safe_pointer_fn) class_new_super_sub,
				super_compile, arg, this, &super ) )
				return( FALSE );

		}
		else if( PEISCLASS( &arg0 ) ) {
			/* Super is a constructed class ... clone it, but with
			 * our "this" in there. Slow, but useful.
			 */
			super_compile = PEGETCLASSCOMPILE( &arg0 );

			if( heap_safe_pointer( heap, 
				(heap_safe_pointer_fn) class_clone_super_sub,
				super_compile, &arg0, this, &super ) )
				return( FALSE );
		}
		else {
			char txt1[300];
			VipsBuf buf1 = VIPS_BUF_STATIC( txt1 );
			char txt2[300];
			VipsBuf buf2 = VIPS_BUF_STATIC( txt2 );

			error_top( _( "Bad superclass." ) );

			itext_value( reduce_context, &buf1, &arg0 );
			vips_buf_appendf( &buf2,
				_( "First element in superclass of \"%s\" "
				"must be class or constructor." ),
				symbol_name( compile->sym ) );
			vips_buf_appendf( &buf2, "\n" );
			vips_buf_appendf( &buf2, _( "You passed:" ) );
			error_sub( "%s\n  %s", 
				vips_buf_all( &buf2 ), vips_buf_all( &buf1 ) );

			return( FALSE );
		}

		/* And recursively build any superclasses.
		 */
		if( !class_new_super( heap, super_compile, this, &super ) )
			return( FALSE );
	}

	return( TRUE );
}

/* Make a class instance. 
 */
gboolean
class_new( Heap *heap, Compile *compile, HeapNode **arg, PElement *out )
{
	int i;
	PElement pe_arg[MAX_SYSTEM];

	/* Make a set of arg pointers.
	 */
	if( compile->nparam + compile->nsecret >= MAX_SYSTEM ) {
		error_top( _( "Too many arguments." ) );
		error_sub( _( "Too many arguments to class constructor \"%s\". "
			"No more than %d arguments are supported." ),
			symbol_name( compile->sym ), MAX_SYSTEM );
		return( FALSE );
	}
	for( i = 0; i < compile->nparam + compile->nsecret; i++ ) {
		PEPOINTRIGHT( arg[i], &pe_arg[i] );
	}

	/* Build the base instance.
	 */
	if( !class_new_single( heap, compile, pe_arg, out, out ) )
		return( FALSE );

	/* And recursively build any superclasses.
	 */
	if( !class_new_super( heap, compile, out, out ) )
		return( FALSE );

#ifdef DEBUG_BUILD
{
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_pelement( heap, &buf, out, TRUE );
	printf( "class_new: built instance of \"%s\": %s\n", 
		IOBJECT( compile->sym )->name, vips_buf_all( &buf ) );
}
#endif /*DEBUG_BUILD*/

	return( TRUE );
}

/* Clone a class instance. Copy pointers to the the args, secrets and super; 
 * regenerate all the members. instance and out can be equal.
 */
gboolean
class_clone_args( Heap *heap, PElement *instance, PElement *out )
{
	HeapNode *arg[MAX_SYSTEM];
	Compile *compile = PEGETCLASSCOMPILE( instance );
	const int nargs = compile->nsecret + compile->nparam;
	PElement secret;
	int i;

	g_assert( nargs <= MAX_SYSTEM );

#ifdef DEBUG_VERBOSE
{
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_pelement( heap, &buf, instance, TRUE );
	printf( "class_clone_args: about to clone \"%s\": %s\n", 
		IOBJECT( compile->sym )->name, vips_buf_all( &buf ) );
}
#endif /*DEBUG_VERBOSE*/

	/* Pull out values of secrets and class args into RHS of arg[].
	 */
	PEGETCLASSSECRET( &secret, instance );
	for( i = 0; i < nargs; i++ ) {
		HeapNode *hn = PEGETVAL( &secret );
		HeapNode *sv = GETLEFT( hn );
		int index = nargs - i - 1;

		arg[index] = sv;
		PEPOINTRIGHT( hn, &secret );
	}

	/* Build class again.
	 */
	return( class_new( heap, compile, &arg[0], out ) );
}

/* Build a class instance picking parameters from C args ... handy for
 * making a new toggle instance on a click, for example.
 */
gboolean
class_newv( Heap *heap, const char *name, PElement *out, ... )
{
	va_list ap;
	Symbol *sym;
	Compile *compile;
	HeapNode args[MAX_SYSTEM];
	HeapNode *pargs[MAX_SYSTEM];
	int i;

	if( !(sym = compile_lookup( symbol_root->expr->compile, name )) ||
		!is_value( sym ) || !is_class( sym->expr->compile ) ) {
		error_top( _( "Class not found." ) );
		error_sub( _( "Class \"%s\" not found." ), name );
		return( FALSE );
	}
	compile = sym->expr->compile;
	if( compile->nparam >= MAX_SYSTEM ) {
		error_top( _( "Too many arguments." ) );
		error_sub( _( "Too many arguments to class constructor \"%s\". "
			"No more than %d arguments are supported." ),
			symbol_name( compile->sym ), MAX_SYSTEM );
		return( FALSE );
	}

        va_start( ap, out );
	for( i = 0; i < compile->nparam; i++ ) {
		PElement *arg = va_arg( ap, PElement * );
		PElement rhs;

		pargs[i] = &args[i];
		PEPOINTRIGHT( pargs[i], &rhs );
		PEPUTPE( &rhs, arg );
	}
        va_end( ap );

	return( class_new( heap, compile, &pargs[0], out ) );
}

static void
class_typecheck_error( PElement *instance, const char *name, const char *type )
{
	PElement val;
	gboolean res;
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	res = class_get_member( instance, name, NULL, &val );
	g_assert( res );

	vips_buf_appendf( &buf, _( "Member \"%s\" of class \"%s\" "
		"should be of type \"%s\", instead it's:" ), 
		name, 
		IOBJECT( PEGETCLASSCOMPILE( instance )->sym )->name, 
		type );
	vips_buf_appends( &buf, "\n   " );
	if( !itext_value( reduce_context, &buf, &val ) )
		return;

	error_top( _( "Bad argument." ) );
	error_sub( "%s", vips_buf_all( &buf ) );
}

/* A function that gets a type from a class.
 */
typedef gboolean (*ClassGetFn)( PElement *, void * );

static gboolean
class_get_member_check( PElement *instance, const char *name, const char *type,
	ClassGetFn fn, void *a )
{
	PElement val;

	if( !class_get_member( instance, name, NULL, &val ) )
		return( FALSE );

	if( !fn( &val, a ) ) {
		class_typecheck_error( instance, name, type );
		return( FALSE );
	}

	return( TRUE );
}

gboolean
class_get_member_bool( PElement *instance, const char *name, gboolean *out )
{
	return( class_get_member_check( instance, name, "bool",
		(ClassGetFn) heap_get_bool, out ) );
}

gboolean
class_get_member_real( PElement *instance, const char *name, double *out )
{
	return( class_get_member_check( instance, name, "real",
		(ClassGetFn) heap_get_real, out ) );
}

gboolean
class_get_member_int( PElement *instance, const char *name, int *out )
{
	double d;

	if( !class_get_member_check( instance, name, "real",
		(ClassGetFn) heap_get_real, &d ) )
		return( FALSE );
	*out = IM_RINT( d );

	return( TRUE );
}

gboolean
class_get_member_class( PElement *instance, const char *name, 
	const char *type, PElement *out )
{
	gboolean result;

	if( !class_get_member_check( instance, name, type,
		(ClassGetFn) heap_get_class, out ) )
		return( FALSE );

	if( !heap_is_instanceof( type, out, &result ) ) 
		return( FALSE );
	if( !result ) {
		class_typecheck_error( instance, name, type );
		return( FALSE );
	}

	return( TRUE );
}

gboolean
class_get_member_image( PElement *instance, const char *name, Imageinfo **out )
{
	return( class_get_member_check( instance, name, "image",
		(ClassGetFn) heap_get_image, out ) );
}

gboolean
class_get_member_lstring( PElement *instance, const char *name, 
	GSList **labels )
{
	return( class_get_member_check( instance, name, "finite [[char]]",
		(ClassGetFn) heap_get_lstring, labels ) );
}

gboolean
class_get_member_string( PElement *instance, const char *name, 
	char *buf, int sz )
{
	PElement val;

	if( !class_get_member( instance, name, NULL, &val ) )
		return( FALSE );

	if( !heap_get_string( &val, buf, sz ) ) {
		class_typecheck_error( instance, name, "finite [char]" );
		return( FALSE );
	}

	return( TRUE );
}

gboolean
class_get_member_instance( PElement *instance, 
	const char *name, const char *klass, PElement *out )
{
	gboolean result;

	return( class_get_member( instance, name, NULL, out ) &&
		heap_is_instanceof( klass, out, &result ) &&
		result );
}

gboolean
class_get_member_matrix_size( PElement *instance, const char *name, 
	int *xsize, int *ysize )
{
	PElement val;

	if( !class_get_member( instance, name, NULL, &val ) )
		return( FALSE );

	if( !heap_get_matrix_size( &val, xsize, ysize ) ) {
		class_typecheck_error( instance, name, 
			"finite rectangular [[real]]" );
		return( FALSE );
	}

	return( TRUE );
}

gboolean
class_get_member_matrix( PElement *instance, const char *name, 
	double *buf, int n, int *xsize, int *ysize )
{
	PElement val;

	if( !class_get_member( instance, name, NULL, &val ) )
		return( FALSE );

	if( !heap_get_matrix( &val, buf, n, xsize, ysize ) ) {
		class_typecheck_error( instance, name, 
			"finite rectangular [[real]]" );
		return( FALSE );
	}

	return( TRUE );
}

gboolean
class_get_member_realvec( PElement *instance, const char *name, 
	double *buf, int n, int *length )
{
	PElement val;
	int l;

	if( !class_get_member( instance, name, NULL, &val ) )
		return( FALSE );

	if( (l = heap_get_realvec( &val, buf, n )) < 0 ) {
		class_typecheck_error( instance, name, "finite [real]" );
		return( FALSE );
	}

	*length = l;

	return( TRUE );
}
