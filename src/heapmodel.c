/* base class for models of heap classes
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

/*
#define DEBUG
 */

#include "ip.h"

static ModelClass *parent_class = NULL;

void *
heapmodel_new_heap( Heapmodel *heapmodel, PElement *root )
{
	HeapmodelClass *heapmodel_class = HEAPMODEL_GET_CLASS( heapmodel );

	if( heapmodel_class->new_heap ) {
		void *res;
		
		res = heapmodel_class->new_heap( heapmodel, root );

		return( res );
	}

	return( NULL );
}

void *
heapmodel_update_model( Heapmodel *heapmodel )
{
	HeapmodelClass *heapmodel_class = HEAPMODEL_GET_CLASS( heapmodel );

#ifdef DEBUG
	printf( "heapmodel_update_model: %s ",
		G_OBJECT_TYPE_NAME( heapmodel ) );
	row_name_print( heapmodel->row );
	printf( " modified = %d\n", heapmodel->modified );
#endif /*DEBUG*/

	if( heapmodel_class->update_model && !heapmodel->modified ) {
		void *res;

		res = heapmodel_class->update_model( heapmodel );

		return( res );
	}

	return( NULL );
}

void *
heapmodel_update_heap( Heapmodel *heapmodel )
{
	HeapmodelClass *heapmodel_class = HEAPMODEL_GET_CLASS( heapmodel );

	if( heapmodel_class->update_heap && heapmodel->modified ) {
		void *res;

		res = heapmodel_class->update_heap( heapmodel );

		return( res );
	}

	return( NULL );
}

void *
heapmodel_clear_edited( Heapmodel *heapmodel )
{
	HeapmodelClass *heapmodel_class = HEAPMODEL_GET_CLASS( heapmodel );

	if( heapmodel_class->clear_edited )
		return( heapmodel_class->clear_edited( heapmodel ) );

	return( NULL );
}

static void
heapmodel_finalize( GObject *gobject )
{
	Heapmodel *heapmodel;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_HEAPMODEL( gobject ) );

	heapmodel = HEAPMODEL( gobject );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static Rhs *
heapmodel_get_rhs( Heapmodel *heapmodel )
{
	iContainer *p;

	/* Search for the enclosing RHS ... may not be one if (eg.) this is a
	 * top-level row.
	 */
	for( p = ICONTAINER( heapmodel )->parent; p; p = p->parent )
		if( IS_RHS( p ) )
			return( RHS( p ) );

	return( NULL );
}

static Row *
heapmodel_get_row( Heapmodel *heapmodel )
{
	Rhs *rhs;

	if( IS_RHS( heapmodel ) )
		return( ROW( ICONTAINER( heapmodel )->parent ) );
	else if( (rhs = heapmodel_get_rhs( heapmodel )) )
		return( HEAPMODEL( rhs )->row );
	else
		return( NULL );
}

static void
heapmodel_parent_add( iContainer *child )
{
	Heapmodel *heapmodel = HEAPMODEL( child );

	g_assert( IS_HEAPMODEL( child->parent ) || 
		IS_FILEMODEL( child->parent ) ); 

	ICONTAINER_CLASS( parent_class )->parent_add( child );

	/* Update our context.
	 */
	heapmodel->rhs = heapmodel_get_rhs( heapmodel );
	heapmodel->row = heapmodel_get_row( heapmodel );
}

static void *
heapmodel_real_new_heap( Heapmodel *heapmodel, PElement *root )
{
	iobject_changed( IOBJECT( heapmodel ) );

	return( NULL );
}

static void *
heapmodel_real_update_model( Heapmodel *heapmodel )
{
	iobject_changed( IOBJECT( heapmodel ) );

	return( NULL );
}

static void *
heapmodel_real_update_heap( Heapmodel *heapmodel )
{
	g_assert( heapmodel->modified );

	heapmodel_set_modified( heapmodel, FALSE );

	return( NULL );
}

static void *
heapmodel_real_clear_edited( Heapmodel *heapmodel )
{
	return( NULL );
}

static void
heapmodel_class_init( HeapmodelClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Init methods.
	 */
	gobject_class->finalize = heapmodel_finalize;

	icontainer_class->parent_add = heapmodel_parent_add;

	heapmodel_class->new_heap = heapmodel_real_new_heap;
	heapmodel_class->update_heap = heapmodel_real_update_heap;
	heapmodel_class->update_model = heapmodel_real_update_model;
	heapmodel_class->clear_edited = heapmodel_real_clear_edited;
}

static void
heapmodel_init( Heapmodel *heapmodel )
{
        heapmodel->row = NULL;
        heapmodel->rhs = NULL;

	heapmodel->modified = FALSE;
}

GType
heapmodel_get_type( void )
{
	static GType heapmodel_type = 0;

	if( !heapmodel_type ) {
		static const GTypeInfo info = {
			sizeof( HeapmodelClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) heapmodel_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Heapmodel ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) heapmodel_init,
		};

		heapmodel_type = g_type_register_static( TYPE_MODEL, 
			"Heapmodel", &info, 0 );
	}

	return( heapmodel_type );
}

void
heapmodel_set_modified( Heapmodel *heapmodel, gboolean modified )
{
	if( heapmodel->modified != modified ) {
#ifdef DEBUG
{
		HeapmodelClass *heapmodel_class = 
			HEAPMODEL_GET_CLASS( heapmodel );

		printf( "heapmodel_set_modified: %s::", 
			G_OBJECT_CLASS_NAME( heapmodel_class ) );
		row_name_print( heapmodel->row );
		printf( " %s\n", bool_to_char( modified ) );
}
#endif /*DEBUG*/

		heapmodel->modified = modified;
		iobject_changed( IOBJECT( heapmodel ) );
	}
}

/* Generate a descriptive name for a heapmodel. Used for captions.
 */
gboolean
heapmodel_name( Heapmodel *heapmodel, VipsBuf *buf )
{
	Row *row = heapmodel->row;
	Expr *expr;
	Symbol *sym;
	Toolitem *toolitem;

	if( !row || !(expr = row->expr) || !PEISCLASS( &expr->root ) ) 
		return( FALSE );
	sym = PEGETCLASSCOMPILE( &expr->root )->sym;

	/* If this is an action member we should be able to look up
	 * it's sym and get a descriptive label.
	 */
	if( (toolitem = toolitem_lookup( row->ws->kitg, sym )) )
		vips_buf_appends( buf, toolitem->name );
	else 
		symbol_qualified_name_relative( row->ws->sym, sym, buf );

	return( TRUE );
}

/* Print the value member to a buf.
 */
gboolean
heapmodel_value( Heapmodel *heapmodel, VipsBuf *buf )
{
	Expr *expr;
	PElement value;

	if( !heapmodel->row ||
		!(expr = heapmodel->row->expr) ||
		expr->err || 
		expr->sym->dirty ||
		!class_get_member( &expr->root, MEMBER_VALUE, NULL, &value ) ) 
		return( FALSE );

	itext_value( reduce_context, buf, &value );

	return( TRUE );
}

