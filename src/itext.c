/* a text item in a workspace
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

static HeapmodelClass *parent_class = NULL;

static void
itext_finalize( GObject *gobject )
{
	iText *itext;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_ITEXT( gobject ) );

	itext = ITEXT( gobject );

#ifdef DEBUG
	printf( "itext_destroy\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */
	IM_FREE( itext->formula );
	IM_FREE( itext->formula_default );
	buf_destroy( &itext->value );
	buf_destroy( &itext->decompile );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static void
itext_info( iObject *iobject, BufInfo *buf )
{
	iText *itext = ITEXT( iobject );

	buf_appends( buf, _( "Formula" ) );
	buf_appendf( buf, ": %s\n", NN( itext->formula ) );
}

/* Fwd ref this.
 */
static gboolean itext_add_element( BufInfo *buf, 
	PElement *base, gboolean top, gboolean bracket );

/* Sub-fn of below, callback for list print. Eval and print the item into 
 * the buffer, separating with commas as required. 
 */
static void *
itext_add_list( PElement *base, BufInfo *buf, gboolean *first )
{
	Reduce *rc = reduce_context;

	if( *first )
		*first = FALSE;
	else 
		buf_appends( buf, ", " );

	/* Reduce the head, and print.
	 */
	if( !reduce_pelement( rc, reduce_spine, base ) ) 
		return( base );
	if( !itext_add_element( buf, base, FALSE, FALSE ) )
		return( base );

	/* Buffer full? Abort list print.
	 */
	if( buf->full )
		return( base );

	return( NULL );
}

/* Sub-fn of below, callback for string print. Print the chars into the 
 * buffer.
 */
static void *
itext_add_string( PElement *base, BufInfo *buf )
{
	Reduce *rc = reduce_context;

	/* Reduce the head, and add the char.
	 */
	if( !reduce_pelement( rc, reduce_spine, base ) ) 
		return( base );
	if( PEISCHAR( base ) )
		/* Don't escape chars in string mode.
		 */
		buf_appendf( buf, "%c", PEGETCHAR( base ) );
	else {
		/* Help! Fall back to ordinary item print.
		 */
		buf_appends( buf, ", " );
		if( !itext_add_element( buf, base, FALSE, FALSE ) )
			return( base );
	}

	/* Buffer full? Abort string print.
	 */
	if( buf->full )
		return( base );

	return( NULL );
}

/* Print a char ... we need to escape \n etc.
 */
static void
itext_add_char( int ch, BufInfo *buf )
{
	char in[2];
	char out[3];

	in[0] = ch;
	in[1] = '\0';
	my_strecpy( out, in, FALSE );

	buf_appends( buf, out );
}

/* Print a complex. 
 */
static void
itext_add_complex( double rp, double ip, BufInfo *buf )
{
	if( PRINT_CARTESIAN ) 
		buf_appendf( buf, "(%.12g, %.12g)", rp, ip );
	else {
		if( rp == 0 ) {
			if( ip == 0 ) 
				buf_appendf( buf, "0" );
			else 
				buf_appendf( buf, "%.12gj", ip );
		}
		else if( ip == 0 ) 
			buf_appendf( buf, "%.12g", rp );
		else
			buf_appendf( buf, "%.12g + %.12gj", rp, ip );

	}
}

/* Try to decompile.
 */
static gboolean
itext_decompile_element( BufInfo *buf, PElement *base, gboolean top )
{
	Reduce *rc = reduce_context;
	gboolean result;

	/* Set the value label for a tally entry.
	 */
	if( PEISNOVAL( base ) ) 
		buf_appends( buf, _( "no value" ) );
	else if( PEISREAL( base ) ) 
		buf_appendf( buf, "%g", PEGETREAL( base ) );
	else if( PEISBOOL( base ) ) 
		buf_appends( buf, bool_to_char( PEGETBOOL( base ) ) );
	else if( PEISCHAR( base ) ) {
		buf_appends( buf, "'" );
		itext_add_char( (int) PEGETCHAR( base ), buf );
		buf_appends( buf, "'" );
	}
	else if( PEISCOMPLEX( base ) ) 
		itext_add_complex( PEGETREALPART( base ), PEGETIMAGPART( base ),
			buf );
	else if( PEISELIST( base ) ) {
		buf_appends( buf, "[ ]" );
	}
	else if( !heap_is_string( base, &result ) ) 
		/* Eval error.
		 */
		return( FALSE );
	else if( result ) {
		buf_appends( buf, "\"" );
		if( heap_map_list( base,
			(heap_map_list_fn) itext_add_string, buf, NULL ) )
			return( FALSE );
		buf_appends( buf, "\"" );
	}
	else if( PEISLIST( base ) ) {
		gboolean first = TRUE;

		buf_appends( buf, "[" );
		if( heap_map_list( base,
			(heap_map_list_fn) itext_add_list, buf, &first ) )
			return( FALSE );
		buf_appends( buf, "]" );
	}
	else if( PEISIMAGE( base ) ) {
		Imageinfo *ii = PEGETII( base );

		if( !top )
			buf_appends( buf, "(" );

		if( ii && IOBJECT( ii )->name ) 
			buf_appendf( buf, "vips_image \"%s\"", 
				IOBJECT( ii )->name );
		else 
			buf_appendf( buf, "vips_image <unknown>" );

		if( !top )
			buf_appends( buf, ")" );
	}
#ifdef HAVE_VIPS8
	else if( PEISVIPSOBJECT( base ) ) {
		VipsObject *object = PEGETVIPSOBJECT( base );

		buf_appendf( buf, "<%s: %s>", object->name, object->caption );
	}
#endif /*HAVE_VIPS8*/
	if( PEISMANAGEDSTRING( base ) ) {
		Managedstring *managedstring = PEGETMANAGEDSTRING( base );

		buf_appendf( buf, "\"%s\"", managedstring->string );
	}
	else if( PEISMANAGED( base ) ) {
		Managed *managed = PEGETMANAGED( base );

		buf_appends( buf, "<" );
		iobject_info( IOBJECT( managed ), buf );
		buf_appends( buf, ">" );
	}
	else if( PEISCLASS( base ) ) {
		Compile *compile = PEGETCLASSCOMPILE( base );
		PElement params;
		int i;

		if( !top )
			buf_appends( buf, "(" );

		symbol_qualified_name( compile->sym, buf );

		/* Skip over the secrets, then decompile all the args.
		 */
		PEGETCLASSSECRET( &params, base );
		for( i = 0; i < compile->nsecret; i++ ) {
			HeapNode *hn = PEGETVAL( &params );

			PEPOINTRIGHT( hn, &params );
		}

		for( i = 0; i < compile->nparam; i++ ) {
			HeapNode *hn = PEGETVAL( &params );
			HeapNode *sv = GETLEFT( hn );
			PElement value;

			PEPOINTRIGHT( sv, &value );
			buf_appends( buf, " " );
			if( !itext_decompile_element( buf, &value, FALSE ) )
				return( FALSE );

			PEPOINTRIGHT( hn, &params );
		}

		if( !top )
			buf_appends( buf, ")" );

	}
	else if( PEISSYMREF( base ) ) 
		buf_appends( buf, IOBJECT( PEGETSYMREF( base ) )->name );
	else if( PEISTAG( base ) ) 
		buf_appends( buf, PEGETTAG( base ) );
	else 
		graph_pelement( rc->heap, buf, base, TRACE_FUNCTIONS );

	return( TRUE );
}

/* Little wrapper ... used for formatting error messages, etc. FALSE for eval
 * error.
 */
static gboolean 
itext_decompile( Reduce *rc, BufInfo *buf, PElement *root )
{
	/* Evaluate and print off values.
	 */
	if( !reduce_pelement( rc, reduce_spine, root ) ) 
		return( FALSE );

	if( !itext_decompile_element( buf, root, TRUE ) && !buf->full ) 
		/* Tally eval failed, and buffer is not full ... must
		 * have been an eval error.
		 */
		return( FALSE );

	return( TRUE );
}

/* Print function for computed values. top is TRUE only for the very top level
 * output. bracket means we should bracket compound expressions.
 */
static gboolean
itext_add_element( BufInfo *buf, PElement *base, 
	gboolean top, gboolean bracket )
{
	gboolean result;

	/* Set the value label for a tally entry.
	 */
	if( PEISNOVAL( base ) ) 
		buf_appends( buf, _( "no value" ) );
	else if( PEISREAL( base ) ) 
		buf_appendf( buf, "%.7g", PEGETREAL( base ) );
	else if( PEISBOOL( base ) ) 
		buf_appends( buf, bool_to_char( PEGETBOOL( base ) ) );
	else if( PEISCHAR( base ) ) {
		buf_appends( buf, "'" );
		itext_add_char( (int) PEGETCHAR( base ), buf );
		buf_appends( buf, "'" );
	}
	else if( PEISCOMPLEX( base ) ) {
		itext_add_complex( PEGETREALPART( base ), PEGETIMAGPART( base ),
			buf );
	}
	else if( PEISELIST( base ) ) {
		buf_appends( buf, "[ ]" );
	}
	else if( !heap_is_string( base, &result ) ) 
		/* Eval error.
		 */
		return( FALSE );
	else if( result ) {
		/* Only generate quotes for non-top-level string objects.
		 */
		if( !top ) 
			buf_appends( buf, "\"" );

		/* Print string contents.
		 */
		if( heap_map_list( base,
			(heap_map_list_fn) itext_add_string, buf, NULL ) )
			return( FALSE );

		if( !top ) 
			buf_appends( buf, "\"" );
	}
	else if( PEISLIST( base ) ) {
		gboolean first = TRUE;

		buf_appends( buf, "[" );
		if( heap_map_list( base,
			(heap_map_list_fn) itext_add_list, buf, &first ) )
			return( FALSE );
		buf_appends( buf, "]" );
	}
#ifdef HAVE_VIPS8
	else if( PEISVIPSOBJECT( base ) ) {
		VipsObject *object = PEGETVIPSOBJECT( base );

		buf_appendf( buf, "<%s %s, %s>", 
			G_OBJECT_TYPE_NAME( object ),
			object->name, object->caption );
	}
#endif /*HAVE_VIPS8*/
	else if( PEISIMAGE( base ) ) {
		buf_appendf( buf, "<" );
		buf_appendi( buf, imageinfo_get( FALSE, PEGETII( base ) ) );
		buf_appendf( buf, ">" );
	}
	if( PEISMANAGEDSTRING( base ) ) {
		Managedstring *managedstring = PEGETMANAGEDSTRING( base );

		if( !top ) 
			buf_appends( buf, "\"" );
		buf_appends( buf, managedstring->string );
		if( !top ) 
			buf_appends( buf, "\"" );
	}
	else if( PEISMANAGED( base ) ) {
		Managed *managed = PEGETMANAGED( base );

		buf_appends( buf, "<" );
		iobject_info( IOBJECT( managed ), buf );
		buf_appends( buf, ">" );
	}
	else if( PEISCLASS( base ) ) {
		Compile *compile = PEGETCLASSCOMPILE( base );
		PElement params;
		int i;

		if( bracket && compile->nparam )
			buf_appends( buf, "(" );

		/* Name.
		 */
		symbol_qualified_name( compile->sym, buf );

		/* Skip over the secrets, then value-ize all the args.
		 */
		PEGETCLASSSECRET( &params, base );
		for( i = 0; i < compile->nsecret; i++ ) {
			HeapNode *hn = PEGETVAL( &params );

			PEPOINTRIGHT( hn, &params );
		}

		for( i = 0; i < compile->nparam; i++ ) {
			HeapNode *hn = PEGETVAL( &params );
			HeapNode *sv = GETLEFT( hn );
			PElement value;

			PEPOINTRIGHT( sv, &value );
			buf_appends( buf, " " );
			if( !itext_add_element( buf, &value, FALSE, TRUE ) )
				return( FALSE );

			PEPOINTRIGHT( hn, &params );
		}

		if( bracket && compile->nparam )
			buf_appends( buf, ")" );
	}
	else if( PEISSYMREF( base ) ) {
		Symbol *sym = PEGETSYMREF( base );

		if( is_scope( sym ) ) {
			buf_appendf( buf, "<scope '" );
			symbol_qualified_name( sym, buf );
			buf_appendf( buf, "'>" ); 
		}
		else {
			buf_appendf( buf, "<reference to symbol '" ), 
			symbol_qualified_name( sym, buf );
			buf_appendf( buf, "'>" ); 
		}
	}
	else if( PEISTAG( base ) ) 
		buf_appendf( buf, ".%s", PEGETTAG( base ) );
	else {
		buf_appendf( buf, "<" ); 
		buf_appends( buf, _( "function" ) );
		buf_appendf( buf, ">" ); 
	}

	return( TRUE );
}

/* Little wrapper ... used for formatting error messages, etc. FALSE for eval
 * error.
 */
gboolean 
itext_value( Reduce *rc, BufInfo *buf, PElement *root )
{
	/* Evaluate and print off values.
	 */
	if( !reduce_pelement( rc, reduce_spine, root ) ) 
		return( FALSE );

	if( !itext_add_element( buf, root, TRUE, FALSE ) && !buf->full ) 
		/* Tally eval failed, and buffer is not full ... must
		 * have been an eval error.
		 */
		return( FALSE );

	return( TRUE );
}

/* Same, but everror on eval fail.
 */
void
itext_value_ev( Reduce *rc, BufInfo *buf, PElement *root )
{
	if( !itext_value( rc, buf, root ) )
		reduce_throw( rc );
}

/* Decompile an Expr.
 */
static gboolean
itext_make_decompiled_string( Expr *expr, BufInfo *buf )
{
	/* Old error on this expression?
	 */
	if( expr->err ) {
		expr_error_get( expr );
		return( FALSE );
	}

	/* Dirty? We can't print dirty values, since we might have pointers 
	 * to deleted symbols in the heap (if we are dirty because one of our 
	 * parents has been deleted).

	 	FIXME ... this seem a bit restrictive :-( ... could just
		block reads of symbol pointers instead?

	 */
	if( expr->sym->dirty ) {
		buf_appendf( buf, _( "Dirty value" ) );
		return( TRUE );
	}

	/* Evaluate and print off values.
	 */
	if( !itext_decompile( reduce_context, buf, &expr->root ) )
		return( FALSE );

	return( TRUE );
}

/* Make a value string from an Expr.
 */
gboolean
itext_make_value_string( Expr *expr, BufInfo *buf )
{
	/* Old error on this expression?
	 */
	if( expr->err ) {
		expr_error_get( expr );
		return( FALSE );
	}

	/* Dirty? We can't print dirty values, since we might have pointers 
	 * to deleted symbols in the heap (if we are dirty because one of our 
	 * parents has been deleted).

	 	FIXME ... this seem a bit restrictive :-( ... could just
		block reads of symbol pointers instead?

	 */
	if( expr->sym->dirty ) {
		buf_appendf( buf, _( "Dirty value" ) );
		return( TRUE );
	}

	/* Evaluate and print off values.
	 */
	if( !itext_value( reduce_context, buf, &expr->root ) )
		return( FALSE );

	return( TRUE );
}

static void *
itext_update_model( Heapmodel *heapmodel )
{
	iText *itext = ITEXT( heapmodel );
        Row *row = HEAPMODEL( itext )->row;
	Expr *expr = row->expr;

#ifdef DEBUG
	printf( "itext_update_model: " );
	row_name_print( row );
	if( row->sym && row->sym->dirty )
		printf( " (dirty)" );
	printf( "\n" );
#endif /*DEBUG*/

	buf_set_dynamic( &itext->value, LINELENGTH );
	buf_set_dynamic( &itext->decompile, LINELENGTH );
	if( expr ) {
		if( !itext_make_value_string( expr, &itext->value ) ||
			!itext_make_decompiled_string( expr, 
				&itext->decompile ) ) 
			expr_error_set( expr );
	}

#ifdef DEBUG
	printf( "itext_update_model: " );
	row_name_print( row );
	printf( " has value: %s\n", buf_all( &itext->value ) );
#endif /*DEBUG*/

	/* If this is a non-edited row, update the source.
	 */
	if( !itext->edited || row == row->top_row ) {
		const char *new_formula;

		if( expr && expr->compile && expr->compile->rhstext ) 
			new_formula = expr->compile->rhstext;
		else 
			new_formula = buf_all( &itext->decompile );

		IM_SETSTR( itext->formula_default, new_formula );

		/* Don't use itext_set_formula(), as we don't want to set
		 * _modified or recomp.
		 */
		IM_SETSTR( itext->formula, itext->formula_default );
	}

	return( HEAPMODEL_CLASS( parent_class )->update_model( heapmodel ) );
}

/* Build param lists.
 */
static void *
itext_update_heap_sub( Symbol *sym, BufInfo *buf )
{
	buf_appendf( buf, "%s ", IOBJECT( sym )->name );

	return( NULL );
}

/* heapmodel->modified is set ... parse, compile, and mark for recomp.
 */
static void *
itext_update_heap( Heapmodel *heapmodel )
{
	iText *itext = ITEXT( heapmodel );
        Row *row = heapmodel->row;
	Expr *expr = row->expr;

#ifdef DEBUG
	printf( "itext_update_heap: " );
	row_name_print( HEAPMODEL( itext )->row );
	printf( "\n" );
#endif /*DEBUG*/

	/* We can have no modified text, but come here anyway. For example, we
	 * could try eval, find an error due to an undefined symbol, and have
	 * to retry later. Clearing the row error later will mark us modified, 
	 * even though we have no text of our own.
	 */
	if( itext->formula ) {
		char txt[MAX_STRSIZE];
		BufInfo buf;
		ParseRhsSyntax syntax;

		buf_init_static( &buf, txt, MAX_STRSIZE );
		if( is_super( row->sym ) ) {
			/* A super member ... special syntax.
			 */
			buf_appendf( &buf, "%s", itext->formula );

			syntax = PARSE_SUPER;
		}
		else {
			/* Build a new params + '=' + rhs string.
			 */
			if( expr->compile ) 
				(void) slist_map( expr->compile->param, 
					(SListMapFn) itext_update_heap_sub, 
					&buf );
			buf_appendf( &buf, "= %s;", itext->formula );

			syntax = PARSE_PARAMS;
		}

		/* Parse and compile.
		 */
		expr_error_clear( expr );
		attach_input_string( buf_all( &buf ) );
		if( !parse_rhs( expr, syntax ) ) {
			expr_error_set( expr );
			return( heapmodel );
		}
	}

	/* Mark for recomp.
	 */
	(void) expr_dirty( expr, link_serial_new() );

	return( HEAPMODEL_CLASS( parent_class )->update_heap( heapmodel ) );
}

static void *
itext_clear_edited( Heapmodel *heapmodel )
{
	iText *itext = ITEXT( heapmodel );

#ifdef DEBUG
	printf( "itext_clear_edited: " );
	row_name_print( HEAPMODEL( itext )->row );
	printf( "\n" );
#endif /*DEBUG*/

	if( itext->edited ) {
		itext_set_edited( itext, FALSE );

		/*

			FIXME ... formula_default is not always set for cloned
			edited rows! fix this properly

		 */
		if( itext->formula_default )
			itext_set_formula( itext, itext->formula_default );
		else 
			printf( "itext_clear_edited: FIXME!\n" );

		if( heapmodel->row->expr )
			expr_dirty( heapmodel->row->expr, link_serial_new() );

		/* Don't clear HEAPMODEL( itext )->modified, we want to make
		 * sure we re-parse and compile the default value to break any
		 * old links we might have.
		 */
	}

	return( HEAPMODEL_CLASS( parent_class )->clear_edited( heapmodel ) );
}

static void
itext_parent_add( iContainer *child )
{
	iText *itext = ITEXT( child );
	Row *row;

	g_assert( IS_RHS( child->parent ) );

	ICONTAINER_CLASS( parent_class )->parent_add( child );

	row = HEAPMODEL( itext )->row;

#ifdef DEBUG
	printf( "itext_new: " );
	row_name_print( row );
	printf( "\n" );
#endif /*DEBUG*/

	/* Top rows default to edited.
	 */
	if( row == row->top_row )
		itext->edited = TRUE;
}

static gboolean
itext_load( Model *model, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	iText *itext = ITEXT( model );

	char formula[MAX_STRSIZE];
	char formula2[MAX_STRSIZE];

	g_assert( IS_RHS( parent ) );

	if( get_sprop( xnode, "formula", formula, MAX_STRSIZE ) ) {
		model_loadstate_rewrite( state, formula, formula2 );
		itext_set_formula( itext, formula2 ); 
		itext_set_edited( itext, TRUE );
	}

	return( MODEL_CLASS( parent_class )->load( model, 
		state, parent, xnode ) );
}

static View *
itext_view_new( Model *model, View *parent )
{
	return( itextview_new() );
}

static xmlNode *
itext_save( Model *model, xmlNode *xnode )
{
	iText *itext = ITEXT( model );
	Row *row = HEAPMODEL( model )->row;

	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( itext->edited || row->top_row == row )
		if( !set_sprop( xthis, "formula", itext->formula ) )
			return( NULL );

	return( xthis );
}

static void
itext_class_init( iTextClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iObjectClass *iobject_class = (iObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->finalize = itext_finalize;

	iobject_class->info = itext_info;

	icontainer_class->parent_add = itext_parent_add;

	model_class->view_new = itext_view_new;
	model_class->save = itext_save;
	model_class->load = itext_load;

	heapmodel_class->update_model = itext_update_model;
	heapmodel_class->update_heap = itext_update_heap;
	heapmodel_class->clear_edited = itext_clear_edited;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );
}

static void
itext_init( iText *itext )
{
	Model *model = MODEL( itext );

	model->display = FALSE;

	itext->formula = NULL;
	itext->formula_default = NULL;
	buf_init( &itext->value );
	buf_init( &itext->decompile );
	buf_set_dynamic( &itext->value, LINELENGTH );
	buf_set_dynamic( &itext->decompile, LINELENGTH );
	itext->edited = FALSE;

	/* Some defaults changed in _parent_add() above.
	 */
}

GType
itext_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( iTextClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) itext_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( iText ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) itext_init,
		};

		type = g_type_register_static( TYPE_HEAPMODEL, 
			"iText", &info, 0 );
	}

	return( type );
}

iText *
itext_new( Rhs *rhs )
{
	iText *itext;

	itext = ITEXT( g_object_new( TYPE_ITEXT, NULL ) );
	icontainer_child_add( ICONTAINER( rhs ), ICONTAINER( itext ), -1 );

	return( itext );
}

void
itext_set_edited( iText *itext, gboolean edited )
{
	Heapmodel *heapmodel = HEAPMODEL( itext );

	if( itext->edited != edited ) {
#ifdef DEBUG
		printf( "itext_set_edited: " );
		row_name_print( heapmodel->row );
		printf( " %s\n", bool_to_char( edited ) );
#endif /*DEBUG*/

		itext->edited = edited;
		iobject_changed( IOBJECT( itext ) );
	}

	if( edited ) 
		heapmodel_set_modified( heapmodel, TRUE );
}

gboolean
itext_set_formula( iText *itext, const char *formula )
{
	if( !itext->formula || strcmp( itext->formula, formula ) != 0 ) {
#ifdef DEBUG
		printf( "itext_set_formula: " );
		row_name_print( HEAPMODEL( itext )->row );
		printf( " \"%s\"\n", formula );
#endif /*DEBUG*/

		IM_SETSTR( itext->formula, formula );

		heapmodel_set_modified( HEAPMODEL( itext ), TRUE );

		iobject_changed( IOBJECT( itext ) );

		return( TRUE );
	}

	return( FALSE );
}
