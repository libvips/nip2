/* an input matrix 
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

static void
matrix_finalize( GObject *gobject )
{
	Matrix *matrix;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_MATRIX( gobject ) );

	matrix = MATRIX( gobject );

#ifdef DEBUG
	printf( "matrix_finalize\n" );
#endif /*DEBUG*/

	/* My instance finalize stuff.
	 */
	IM_FREE( matrix->value.coeff );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

/* Rearrange our model for a new width/height.
 */
gboolean
matrix_value_resize( MatrixValue *value, int width, int height )
{
	double *coeff;
	int x, y, i;

	if( width == value->width && height == value->height )
		return( TRUE );

	if( !(coeff = IARRAY( NULL, width * height, double )) )
		return( FALSE );

	/* Set what we can with values from the old matrix.
	 */
	for( i = 0, y = 0; y < height; y++ )
		for( x = 0; x < width; x++, i++ ) 
			if( y < value->height && x < value->width ) 
				coeff[i] = value->coeff[x + 
					y * value->width];
			else 
				coeff[i] = 0.0;

	/* Install new values.
	 */
	IM_FREE( value->coeff );
	value->coeff = coeff;
	value->width = width;
	value->height = height;

	return( TRUE );
}

/* Widgets for matrix edit.
 */
typedef struct _MatrixEdit {
	iDialog *idlg;

	Matrix *matrix;

	GtkWidget *width;
	GtkWidget *height;
	GtkWidget *display;
} MatrixEdit;

/* Done button hit.
 */
/*ARGSUSED*/
static void
matrix_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	MatrixEdit *eds = (MatrixEdit *) client;

	int width, height;

	/* Parse values. We have to scan before we resize in case we are
	 * sizing smaller and we have unscanned changes at the edges.
	 */
	view_scan_all();
	eds->matrix->display = (MatrixDisplayType)
		gtk_combo_box_get_active( GTK_COMBO_BOX( eds->display ) );
	if( !get_geditable_pint( eds->width, &width ) ||
		!get_geditable_pint( eds->height, &height ) ||
		!matrix_value_resize( &eds->matrix->value, width, height ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	/* Rebuild object.
	 */
	classmodel_update( CLASSMODEL( eds->matrix ) );
	symbol_recalculate_all();

	nfn( sys, IWINDOW_YES );
}

/* Build the insides of matrix edit.
 */
static void
matrix_buildedit( iDialog *idlg, GtkWidget *work, MatrixEdit *eds )
{
	Matrix *matrix = eds->matrix;

	GtkSizeGroup *group;

        /* Index with MatrixType.
         */
        static const char *display_names[] = {
                N_( "Text" ),
                N_( "Sliders" ),
                N_( "Toggle buttons" ),
                N_( "Text, plus scale and offset" )
        };

	group = gtk_size_group_new( GTK_SIZE_GROUP_HORIZONTAL );

        eds->width = build_glabeltext4( work, group, "Width" );
	idialog_init_entry( idlg, eds->width, 
		"Width of matrix", "%d", matrix->value.width );
        eds->height = build_glabeltext4( work, group, "Height" );
	idialog_init_entry( idlg, eds->height, 
		"Height of matrix", "%d", matrix->value.height );
        eds->display = build_goption( work, group, _( "Display as" ),
                display_names, IM_NUMBER( display_names ), NULL, NULL );
	gtk_combo_box_set_active( GTK_COMBO_BOX( eds->display ), 
		matrix->display );

	UNREF( group );

        gtk_widget_show_all( work );
}

static View *
matrix_view_new( Model *model, View *parent )
{
	return( matrixview_new() );
}

/* Pop up a matrix edit box.
 */
static void 
matrix_edit( GtkWidget *parent, Model *model )
{
	Matrix *matrix = MATRIX( model );
	MatrixEdit *eds = INEW( NULL, MatrixEdit );
	GtkWidget *idlg;

	eds->matrix = matrix;

	idlg = idialog_new();
	iwindow_set_title( IWINDOW( idlg ), _( "Edit %s %s" ),
		G_OBJECT_TYPE_NAME( model ),
		IOBJECT( HEAPMODEL( model )->row )->name );
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) matrix_buildedit, eds, NULL, NULL );
	idialog_set_callbacks( IDIALOG( idlg ), 
		iwindow_true_cb, NULL, idialog_free_client, eds );
	idialog_add_ok( IDIALOG( idlg ), 
		matrix_done_cb, _( "Set %s" ), G_OBJECT_TYPE_NAME( model ) );
	iwindow_set_parent( IWINDOW( idlg ), parent );
	idialog_set_iobject( IDIALOG( idlg ), IOBJECT( model ) );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );
}

static gboolean
matrix_graphic_save( Classmodel *classmodel, 
	GtkWidget *parent, const char *filename )
{
	Matrix *matrix = MATRIX( classmodel );
	DOUBLEMASK *dmask;

	if( !(dmask = matrix_model_to_dmask( matrix )) ) 
		return( FALSE );

	if( im_write_dmask_name( dmask, filename ) ) {
		error_vips_all();
		IM_FREEF( im_free_dmask, dmask );
		return( FALSE );
	}
	IM_FREEF( im_free_dmask, dmask );

	mainw_recent_add( &mainw_recent_matrix, filename );

	return( TRUE );
}

static gboolean
matrix_graphic_replace( Classmodel *classmodel, 
	GtkWidget *parent, const char *filename )
{
	Matrix *matrix = MATRIX( classmodel );
	Row *row = HEAPMODEL( matrix )->row;
	iText *itext = ITEXT( HEAPMODEL( matrix )->rhs->itext );
	DOUBLEMASK *dmask;
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	if( !(dmask = im_read_dmask( filename )) ) {
		error_vips_all();
		return( FALSE );
	}

	matrix_dmask_to_ip( dmask, &buf );
	im_free_dmask( dmask );

	if( itext_set_formula( itext, vips_buf_all( &buf ) ) ) {
		itext_set_edited( itext, TRUE );
		(void) expr_dirty( row->expr, link_serial_new() );
	}

	mainw_recent_add( &mainw_recent_matrix, filename );

	return( TRUE );
}

/* Members of matrix we automate.
 */
static ClassmodelMember matrix_members[] = {
	{ CLASSMODEL_MEMBER_MATRIX, NULL, 0, 
		MEMBER_VALUE, NULL, N_( "Value" ),
		G_STRUCT_OFFSET( Matrix, value ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		MEMBER_SCALE, "scale", N_( "Scale" ),
		G_STRUCT_OFFSET( Matrix, scale ) },
	{ CLASSMODEL_MEMBER_DOUBLE, NULL, 0,
		MEMBER_OFFSET, "offset", N_( "Offset" ),
		G_STRUCT_OFFSET( Matrix, offset ) },
	{ CLASSMODEL_MEMBER_STRING, NULL, 0,
		MEMBER_FILENAME, "filename", N_( "Filename" ),
		G_STRUCT_OFFSET( Classmodel, filename ) },
	{ CLASSMODEL_MEMBER_ENUM, NULL, MATRIX_DISPLAY_LAST - 1,
		MEMBER_DISPLAY, "display", N_( "Display" ),
		G_STRUCT_OFFSET( Matrix, display ) }
};

static void
matrix_class_init( MatrixClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->finalize = matrix_finalize;

	model_class->view_new = matrix_view_new;
	model_class->edit = matrix_edit;

	classmodel_class->graphic_save = matrix_graphic_save;
	classmodel_class->graphic_replace = matrix_graphic_replace;

	classmodel_class->filetype = filesel_type_matrix;
	classmodel_class->filetype_pref = "MATRIX_FILE_TYPE";

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = matrix_members;
	classmodel_class->n_members = IM_NUMBER( matrix_members );
}

static void
matrix_init( Matrix *matrix )
{
#ifdef DEBUG
	printf( "matrix_init\n" );
#endif /*DEBUG*/

	matrix->value.coeff = NULL;
        matrix->value.width = 0;
	matrix->value.height = 0;
	matrix->display = MATRIX_DISPLAY_TEXT;
	matrix->scale = 1.0;
	matrix->offset = 0.0;
	matrix->selected = FALSE;

	iobject_set( IOBJECT( matrix ), CLASS_MATRIX, NULL );
}

GtkType
matrix_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( MatrixClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) matrix_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Matrix ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) matrix_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Matrix", &info, 0 );
	}

	return( type );
}

void
matrix_select( Matrix *matrix, int left, int top, int width, int height )
{
	if( !matrix->selected ||
		matrix->range.left != left ||
		matrix->range.top != top ||
		matrix->range.width != width ||
		matrix->range.height != height ) {
		Row *row = HEAPMODEL( matrix )->row;

#ifdef DEBUG
		printf( "matrix_select: "
			"left=%d, top = %d, width = %d, height = %d\n",
			left, top, width, height );
#endif /*DEBUG*/

		matrix->selected = TRUE;
		matrix->range.left = left;
		matrix->range.top = top;
		matrix->range.width = width;
		matrix->range.height = height;
		iobject_changed( IOBJECT( matrix ) );

		/* Also make sure this row is selected.
		 */
		row_select_ensure( row );

		/* The range of cells selected has changed, so the workspace
		 * must update the status line too. row_select_ensure() only
		 * spots row on/off selects. Yuk!
		 */
		iobject_changed( IOBJECT( row->ws ) );
	}
}

void 
matrix_deselect( Matrix *matrix )
{
	if( matrix->selected ) {
		Row *row = HEAPMODEL( matrix )->row;

#ifdef DEBUG
		printf( "matrix_deselect\n" );
#endif /*DEBUG*/

		matrix->selected = FALSE;
		iobject_changed( IOBJECT( matrix ) );

		/* Also make sure this row is not selected.
		 */
		row_deselect( row );
	}
}

/* Guess a display type from a filename.
 */
static int
matrix_guess_display( const char *fname )
{
	/* Choose display type based on filename suffix ... rec 
	 * displays as 1, mor displays as 2, .con displays as 3, all others 
	 * display as 0. Keep in sync with MatrixDisplayType.
	 */
	static const FileselFileType *types[] = {
		&filesel_xfile_type,	// matrix
		&filesel_rfile_type,	// recombination
		&filesel_mfile_type,	// morphology
		&filesel_cfile_type	// convolution
	};

	int i;

	if( !fname )
		return( 0 );

	for( i = 0; i < IM_NUMBER( types ); i++ )
		if( is_file_type( types[i], fname ) )
			return( i );

	return( 0 );
}

/* Make an ip definition out of a DOUBLEMASK.
 */
void
matrix_dmask_to_ip( DOUBLEMASK *dmask, VipsBuf *buf )
{
	int x, y;

	/* Build matrix expression.
	 */
	vips_buf_appends( buf, CLASS_MATRIX " " );

	vips_buf_appends( buf, "[" );
	for( y = 0; y < dmask->ysize; y++ ) {
		vips_buf_appends( buf, "[" );
		for( x = 0; x < dmask->xsize; x++ ) {
			vips_buf_appendf( buf, "%g", 
				dmask->coeff[x + y*dmask->xsize] );
			if( x != dmask->xsize - 1 )
				vips_buf_appends( buf, "," );
		}
		vips_buf_appends( buf, "]" );
		if( y != dmask->ysize - 1 )
			vips_buf_appends( buf, "," );
	}
	vips_buf_appends( buf, "]" );

	vips_buf_appendf( buf, "(%g) (%g) \"%s\" %d", 
		dmask->scale, dmask->offset, dmask->filename,
		matrix_guess_display( dmask->filename ) );
}

/* Make a heap object out of a DOUBLEMASK.
 */
gboolean
matrix_dmask_to_heap( Heap *heap, DOUBLEMASK *dmask, PElement *out )
{
	Symbol *sym = compile_lookup( symbol_root->expr->compile, 
		CLASS_MATRIX );

	PElement rhs;

	if( !sym || !sym->expr || !sym->expr->compile ||
		!heap_copy( heap, sym->expr->compile, out ) )
		return( FALSE );

	if( !heap_appl_add( heap, out, &rhs ) || 
		!heap_matrix_new( heap, 
			dmask->xsize, dmask->ysize, dmask->coeff, &rhs ) ||
		!heap_appl_add( heap, out, &rhs ) ||
		!heap_real_new( heap, dmask->scale, &rhs ) ||
		!heap_appl_add( heap, out, &rhs ) ||
		!heap_real_new( heap, dmask->offset, &rhs ) ||
		!heap_appl_add( heap, out, &rhs ) ||
		!heap_managedstring_new( heap, dmask->filename, &rhs ) ||
		!heap_appl_add( heap, out, &rhs ) ||
		!heap_real_new( heap, 
			matrix_guess_display( dmask->filename ), &rhs ) )
		return( FALSE );

	return( TRUE );
}

/* Cast an IMASK to a DMASK.
 */
DOUBLEMASK *
matrix_imask_to_dmask( INTMASK *imask )
{
	DOUBLEMASK *dmask;
	int i;

	if( !(dmask = im_create_dmask( imask->filename, 
		imask->xsize, imask->ysize )) ) {
		error_vips_all();
		return( NULL );
	}

	dmask->scale = imask->scale;
	dmask->offset = imask->offset;
	for( i = 0; i < imask->xsize * imask->ysize; i++ )
		dmask->coeff[i] = imask->coeff[i];

	return( dmask );
}

/* Cast a DMASK to an IMASK.
 */
INTMASK *
matrix_dmask_to_imask( DOUBLEMASK *dmask )
{
	INTMASK *imask;
	int i;

	if( !(imask = im_create_imask( dmask->filename, 
		dmask->xsize, dmask->ysize )) ) {
		error_vips_all();
		return( NULL );
	}

	imask->scale = dmask->scale;
	imask->offset = dmask->offset;
	for( i = 0; i < dmask->xsize * dmask->ysize; i++ )
		imask->coeff[i] = dmask->coeff[i];

	return( imask );
}

/* Make a heap object out of an INTMASK.
 */
gboolean
matrix_imask_to_heap( Heap *heap, INTMASK *imask, PElement *out )
{
	DOUBLEMASK *dmask;

	if( !(dmask = matrix_imask_to_dmask( imask )) )
		return( FALSE );
	if( !matrix_dmask_to_heap( heap, dmask, out ) ) {
		im_free_dmask( dmask );
		return( FALSE );
	}
	im_free_dmask( dmask );

	return( TRUE );
}

/* Make a DOUBLEMASK out of an ip value.
 */
DOUBLEMASK *
matrix_ip_to_dmask( PElement *root )
{
	char buf[MAX_STRSIZE];
	char name[FILENAME_MAX];
	DOUBLEMASK *dmask;
	double scale, offset;
	char *filename;
	int width, height;

	if( !class_get_member_matrix_size( root, 
		MEMBER_VALUE, &width, &height ) )
		return( NULL );

	if( class_get_member_string( root, MEMBER_FILENAME, buf, MAX_STRSIZE ) )
		filename = buf;
	else {
		if( !temp_name( name, "mat" ) )
			return( NULL );

		filename = name;
	}

	if( !(dmask = im_create_dmask( filename, width, height )) ) {
		error_vips_all();
		return( NULL );
	}

	if( !class_get_member_matrix( root, MEMBER_VALUE, 
		dmask->coeff, width * height, &width, &height ) ) {
		IM_FREEF( im_free_dmask, dmask );
		return( FALSE );
	}

	if( !class_get_member_real( root, MEMBER_SCALE, &scale ) )
		scale = 1.0;
	if( !class_get_member_real( root, MEMBER_OFFSET, &offset ) )
		offset = 0.0;
	dmask->scale = scale;
	dmask->offset = offset;

	return( dmask );
}

/* Make an INTMASK out of an ip value.
 */
INTMASK *
matrix_ip_to_imask( PElement *root )
{
	DOUBLEMASK *dmask;
	INTMASK *imask;

	if( !(dmask = matrix_ip_to_dmask( root )) )
		return( NULL );

	if( !(imask = matrix_dmask_to_imask( dmask )) ) {
		IM_FREEF( im_free_dmask, dmask );
		return( NULL );
	}

	return( imask );
}

DOUBLEMASK *
matrix_model_to_dmask( Matrix *matrix )
{
	DOUBLEMASK *dmask;
	int i;

	if( !(dmask = im_create_dmask( CLASSMODEL( matrix )->filename, 
		matrix->value.width, matrix->value.height )) ) {
		error_vips_all();
		return( NULL );
	}

	dmask->scale = matrix->scale;
	dmask->offset = matrix->offset;
	for( i = 0; i < matrix->value.width * matrix->value.height; i++ )
		dmask->coeff[i] = matrix->value.coeff[i];

	return( dmask );
}

gboolean
matrix_dmask_to_model( Matrix *matrix, DOUBLEMASK *dmask )
{
	int i;

	if( !matrix_value_resize( &matrix->value, 
		dmask->xsize, dmask->ysize ) ) 
		return( FALSE );

	matrix->scale = dmask->scale;
	matrix->offset = dmask->offset;
	for( i = 0; i < matrix->value.width * matrix->value.height; i++ )
		matrix->value.coeff[i] = dmask->coeff[i];
	matrix->display = 
		(MatrixDisplayType) matrix_guess_display( dmask->filename );
	IM_SETSTR( CLASSMODEL( matrix )->filename, dmask->filename );

	return( TRUE );
}

