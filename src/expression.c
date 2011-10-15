/* an editable expression
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

static ClassmodelClass *parent_class = NULL;

/* Sub fn. of below.
 */
static void *
expression_get_itext_sub( Row *row )
{
	Model *itext;

	/*

		FIXME ... yuk, map + strcmp

		could make subcolumn indexed by symbol name? probably not
		worth it

	 */
	if( row->sym && 
		strcmp( IOBJECT( row->sym )->name, MEMBER_EXPR ) == 0 &&
		row->child_rhs && 
		(itext = row->child_rhs->itext) )
			return( itext );

	return( NULL );
}

/* Look down our RHS and try to grab the itext for our MEMBER_EXPR.
 * Expressionview presents this as the editable formula.
 *
 * We can't call the editable member "value", since this imples (elsewhere in
 * nip anway) an unboxed value. Our editable member could also be boxed .. so
 * have a different name of reduce confusion a little. Also means we can
 * define an Expression which inherits from expr.
 */
iText *
expression_get_itext( Expression *expression )
{
	Row *row = HEAPMODEL( expression )->row;

	if( row->child_rhs && row->child_rhs->scol ) 
		return( (iText *) subcolumn_map( 
			SUBCOLUMN( row->child_rhs->scol ),
			(row_map_fn) expression_get_itext_sub, 
			NULL, NULL ) );

	return( NULL );
}

static View *
expression_view_new( Model *model, View *parent )
{
	return( expressionview_new() );
}

static xmlNode *
expression_save( Model *model, xmlNode *xnode )
{
	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( !set_sprop( xthis, "caption", IOBJECT( model )->caption ) )
		return( NULL );

	return( xthis );
}

static gboolean
expression_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	char caption[MAX_STRSIZE];

	g_assert( IS_RHS( parent ) );

	if( get_sprop( xnode, "caption", caption, MAX_STRSIZE ) ) 
		iobject_set( IOBJECT( model ), NULL, caption );

	return( MODEL_CLASS( parent_class )->load( model, 
		state, parent, xnode ) );
}

/* Update Expression from heap.
 */
static gboolean
expression_class_get( Classmodel *classmodel, PElement *root )
{
	char caption[MAX_STRSIZE];

#ifdef DEBUG
	printf( "expression_class_get: " );
	row_name_print( HEAPMODEL( classmodel )->row );
	printf( "\n" );
#endif /*DEBUG*/

	if( !class_get_member_string( root, MEMBER_CAPTION, 
		caption, MAX_STRSIZE ) )
		return( FALSE );
	iobject_set( IOBJECT( classmodel ), NULL, caption );

	return( TRUE );
}

static void
expression_class_init( ExpressionClass *class )
{
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	model_class->view_new = expression_view_new;
	model_class->save = expression_save;
	model_class->load = expression_load;

	classmodel_class->class_get = expression_class_get;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
expression_init( Expression *expression )
{
	iobject_set( IOBJECT( expression ), CLASS_EXPRESSION, NULL );
}

GType
expression_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ExpressionClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) expression_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Expression ),
			32,             /* n_pexpressionlocs */
			(GInstanceInitFunc) expression_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Expression", &info, 0 );
	}

	return( type );
}
