/* the rhs of a tallyrow ... group together everything to the right of the
 * button
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
#define DEBUG
 */

static HeapmodelClass *parent_class = NULL;

/* child is about to be added ... update our graphic/scol/text shortcuts.
 */
static void
rhs_child_add( iContainer *parent, iContainer *child, int pos )
{
	Rhs *rhs = RHS( parent );

	if( IS_SUBCOLUMN( child ) ) {
		IDESTROY( rhs->scol );
		rhs->scol = MODEL( child );
	}
	else if( IS_ITEXT( child ) ) {
		IDESTROY( rhs->itext );
		rhs->itext = MODEL( child );
	}
	else {
		IDESTROY( rhs->graphic );
		rhs->graphic = MODEL( child );
	}

	ICONTAINER_CLASS( parent_class )->child_add( parent, child, pos );
}

static void
rhs_child_remove( iContainer *parent, iContainer *child )
{
	Rhs *rhs = RHS( parent );

	if( (void *) child == (void *) rhs->graphic )
		rhs->graphic = NULL;
	else if( (void *) child == (void *) rhs->scol )
		rhs->scol = NULL;
	else if( (void *) child == (void *) rhs->itext )
		rhs->itext = NULL;

	ICONTAINER_CLASS( parent_class )->child_remove( parent, child );
}

static void
rhs_parent_add( iContainer *child )
{
	g_assert( IS_ROW( child->parent ) );

	ICONTAINER_CLASS( parent_class )->parent_add( child );
}

static View *
rhs_view_new( Model *model, View *parent )
{
	return( rhsview_new() );
}

static gboolean
rhs_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	Rhs *rhs = RHS( model );

	g_assert( IS_ROW( parent ) );

	/* Hmm. Is this guaranteed?
	 */
	g_assert( sizeof( RhsFlags ) == sizeof( int ) );

	if( !get_iprop( xnode, "vislevel", &rhs->vislevel ) ||
		!get_iprop( xnode, "flags", (int *) &rhs->flags ) )
		return( FALSE );

	if( !MODEL_CLASS( parent_class )->load( model, state, parent, xnode ) )
		return( FALSE );

	return( TRUE );
}

static xmlNode *
rhs_save( Model *model, xmlNode *xnode )
{
	Rhs *rhs = RHS( model );

	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( !set_prop( xthis, "vislevel", "%d", rhs->vislevel ) ||
		!set_prop( xthis, "flags", "%d", rhs->flags ) )
		return( NULL );

	return( xthis );
}

/* How to spot and make a graphic display.
 */
typedef struct {
	const char *name;
	GType (*type)( void );
} RhsGraphic;

/* All our graphicdisplay widgets. Order is important! Most-derived classes
 * first.
 */
static RhsGraphic rhs_graphic[] = {
	{ CLASS_CLOCK, clock_get_type },
	{ CLASS_EXPRESSION, expression_get_type },
	{ CLASS_GROUP, group_get_type },
	{ CLASS_LIST, group_get_type },
	{ CLASS_PATHNAME, pathname_get_type },
	{ CLASS_FONTNAME, fontname_get_type },
	{ CLASS_TOGGLE, toggle_get_type },
	{ CLASS_SLIDER, slider_get_type },
	{ CLASS_COLOUR, colour_get_type },
	{ CLASS_OPTION, option_get_type },
	{ CLASS_MATRIX, matrix_get_type },
	{ CLASS_ARROW, iarrow_get_type },
	{ CLASS_REGION, iregion_get_type },
	{ CLASS_PLOT, plot_get_type },
	{ CLASS_IMAGE, iimage_get_type },
	{ CLASS_NUMBER, number_get_type },
	{ CLASS_REAL, real_get_type },
	{ CLASS_VECTOR, vector_get_type },
	{ CLASS_STRING, string_get_type }
};

/* Create/destroy the graphic display.
 */
static gboolean
rhs_refresh_graphic( Rhs *rhs, PElement *root )
{
	gboolean result;
	Row *row = HEAPMODEL( rhs )->row;
	int i;

	if( !heap_is_class( root, &result ) ) 
		return( FALSE );

	/* Only for non-parameter class objects.
	 */
	if( result && row->sym->type != SYM_PARAM ) {
		for( i = 0; i < IM_NUMBER( rhs_graphic ); i++ ) {
			const char *name = rhs_graphic[i].name;

			if( !heap_is_instanceof( name, root, &result ) ) 
				return( FALSE );
			if( result ) 
				break;
		}

		if( i != IM_NUMBER( rhs_graphic ) ) {
			GType type = rhs_graphic[i].type();

			if( !rhs->graphic || !TYPE_EXACT( rhs->graphic, type ) )
				classmodel_new_classmodel( type, rhs );
		}
		else
			/* Not a class we know about.
			 */
			IDESTROY( rhs->graphic );
	}
	else
		/* Should be no graphic display.
		 */
		IDESTROY( rhs->graphic );

	return( TRUE );
}

static void *
rhs_new_heap( Heapmodel *heapmodel, PElement *root )
{
	gboolean result;
	Rhs *rhs = RHS( heapmodel );
	Row *row = HEAPMODEL( rhs )->row;

#ifdef DEBUG
	printf( "rhs_new_heap: " );
	row_name_print( HEAPMODEL( rhs )->row );
	printf( "\n" );
#endif /*DEBUG*/

	/* Create/reuse/destroy the graphic display.
	 */
	if( !rhs_refresh_graphic( rhs, root ) )
		return( rhs );

	/* Create/reuse/destroy class display. Only for non-param symbols.
	 */
	if( !heap_is_class( root, &result ) ) 
		return( rhs );
	if( result && row->sym->type != SYM_PARAM ) {
		if( !rhs->scol || !IS_SUBCOLUMN( rhs->scol ) ) 
			subcolumn_new( rhs, NULL );
	}
	else 
		/* Should be no klass display.
		 */
		IDESTROY( rhs->scol );

	/* Create/reuse/destroy text display.
	 */
	if( !rhs->itext )
		itext_new( rhs );

	/* Recurse for children.
	 */
	if( rhs->graphic )
		if( heapmodel_new_heap( HEAPMODEL( rhs->graphic ), root ) )
			return( rhs );

	if( rhs->scol )
		if( heapmodel_new_heap( HEAPMODEL( rhs->scol ), root ) )
			return( rhs );

	if( rhs->itext )
		if( heapmodel_new_heap( HEAPMODEL( rhs->itext ), root ) )
			return( rhs );

	return( HEAPMODEL_CLASS( parent_class )->new_heap( heapmodel, root ) );
}

/* Rethink child visibility.
 */
void
rhs_set_vislevel( Rhs *rhs, int vislevel )
{
	vislevel = IM_MAX( 0, vislevel );

#ifdef DEBUG
	printf( "rhs_set_vislevel: %d ...\n", vislevel );
#endif /*DEBUG*/

	if( rhs->scol ) {
		Subcolumn *scol = SUBCOLUMN( rhs->scol );

		if( rhs->graphic ) {
			switch( vislevel ) {
			case 0:
				rhs->flags = RHS_ITEXT;
				break;

			case 1:
				rhs->flags = RHS_GRAPHIC;
				break;

			case 2:
				rhs->flags = RHS_ITEXT | RHS_GRAPHIC;
				break;

			default:
				rhs->flags = RHS_ITEXT | RHS_GRAPHIC | RHS_SCOL;
			}

			subcolumn_set_vislevel( scol, vislevel - 2 );
			if( vislevel < 3 )
				rhs->vislevel = vislevel;
			else
				rhs->vislevel = scol->vislevel + 2;
		}
		else {
			vislevel = IM_MAX( 1, vislevel );

			if( vislevel == 1 )
				rhs->flags = RHS_ITEXT;
			else
				rhs->flags = RHS_ITEXT | RHS_SCOL;

			subcolumn_set_vislevel( scol, vislevel - 1 );
			rhs->vislevel = scol->vislevel + 1;
		}
	}
	else {
		rhs->flags = RHS_ITEXT;
		rhs->vislevel = vislevel;
	}

#ifdef DEBUG
	printf( "... set to: %d\n", rhs->vislevel );
#endif /*DEBUG*/

	iobject_changed( IOBJECT( rhs ) );
}

void
rhs_vislevel_up( Rhs *rhs )
{
	rhs_set_vislevel( rhs, rhs->vislevel + 1 );
}

void
rhs_vislevel_down( Rhs *rhs )
{
	rhs_set_vislevel( rhs, rhs->vislevel - 1 );
}

static void *
rhs_update_model( Heapmodel *heapmodel )
{
	Rhs *rhs = RHS( heapmodel );

	/* Update visibility.
	 */
	rhs_set_vislevel( rhs, rhs->vislevel );

	return( HEAPMODEL_CLASS( parent_class )->update_model( heapmodel ) );
}

static void
rhs_class_init( RhsClass *class )
{
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	icontainer_class->child_add = rhs_child_add;
	icontainer_class->child_remove = rhs_child_remove;
	icontainer_class->parent_add = rhs_parent_add;

	model_class->view_new = rhs_view_new;
	model_class->load = rhs_load;
	model_class->save = rhs_save;

	heapmodel_class->new_heap = rhs_new_heap;
	heapmodel_class->update_model = rhs_update_model;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
rhs_init( Rhs *rhs )
{
#ifdef DEBUG
	printf( "rhs_init\n" );
#endif /*DEBUG*/

	/* -1 means not set yet ... default vislevel set by row_new_heap()
	 * when the class members become available.
	 */
        rhs->vislevel = -1;

        rhs->graphic = NULL;
        rhs->scol = NULL;
        rhs->itext = NULL;
}

GType
rhs_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( RhsClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) rhs_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Rhs ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) rhs_init,
		};

		type = g_type_register_static( TYPE_HEAPMODEL, 
			"Rhs", &info, 0 );
	}

	return( type );
}

Rhs *
rhs_new( Row *row )
{
	Rhs *rhs = RHS( g_object_new( TYPE_RHS, NULL ) );

	icontainer_child_add( ICONTAINER( row ), ICONTAINER( rhs ), -1 );

#ifdef DEBUG
	printf( "rhs_new: " );
	row_name_print( HEAPMODEL( rhs )->row );
	printf( " (%p)\n", rhs );
#endif /*DEBUG*/

	return( rhs );
}

static void *
rhs_child_edited_sub( Model *model )
{
	Row *row = ROW( model );

	if( row->child_rhs && rhs_child_edited( row->child_rhs ) )
		return( row );

	return( NULL );
}

/* Does this RHS have any edited children? text, graphic, or recursive.
 */
gboolean
rhs_child_edited( Rhs *rhs )
{
	if( rhs->itext && ITEXT( rhs->itext )->edited )
		return( TRUE );
	else if( rhs->graphic && CLASSMODEL( rhs->graphic )->edited )
		return( TRUE );
	else if( rhs->scol ) 
		return( icontainer_map( ICONTAINER( rhs->scol ),
			(icontainer_map_fn) rhs_child_edited_sub, 
			NULL, NULL ) != NULL );
	else
		return( FALSE );
}
