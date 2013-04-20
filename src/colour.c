/* an image class object in a workspace
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

/*
#define DEBUG
 */

#include "ip.h"

/* Set of allowed colour_space strings. Do a case-insensitive match.
 */
static const char *colour_colour_space[] = {
	"xyz", 		/* index 0 */
	"yxy", 		/* index 1 */
	"lab", 		/* index 2 */
	"lch", 		/* index 3 */
	"ucs", 		/* index 4 */
	"rgb", 		/* index 5 */
	"srgb",		/* index 6 */
	"rgb16",	/* index 7 */
	"grey16"	/* index 8 */
};

/* For each allowed colourspace, the corresponding VIPS Type value.
 */
static const int colour_type[] = {
	IM_TYPE_XYZ,
	IM_TYPE_YXY,
	IM_TYPE_LAB,
	IM_TYPE_LCH,
	IM_TYPE_UCS,
	IM_TYPE_RGB,
	IM_TYPE_sRGB,
	IM_TYPE_RGB16,
	IM_TYPE_GREY16
};

static ClassmodelClass *parent_class = NULL;

static void
colour_finalize( GObject *gobject )
{
	Colour *colour = COLOUR( gobject );

	IM_FREE( colour->colour_space );
	vips_buf_destroy( &colour->caption );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

/* Widgets for colour edit.
 */
typedef struct _ColourEdit {
	iDialog *idlg;

	Colour *colour;
	GtkWidget *colour_widget;
} ColourEdit;

/* Find the VIPS type for a colour space string.
 */
static int
colour_get_vips_type( Colour *colour )
{
	int type;
	int i;

	/* Default to something harmless.
	 */
	type = IM_TYPE_MULTIBAND;

	if( colour->colour_space ) 
		for( i = 0; i < IM_NUMBER( colour_colour_space ); i++ )
			if( strcasecmp( colour->colour_space, 
				colour_colour_space[i] ) == 0 ) {
				type = colour_type[i];
				break;
			}

	return( type );
}

/* Are two doubles more or less equal. We need this when we check 
 * for update to stop loops. The 0.0001 is a bit of a fudge :-(
 */
#define DEQ( A, B ) (ABS((A) - (B)) < 0.0001)

/* Update non-model stuff in object from the model params.
 */
static void
colour_refresh( Colour *colour )
{
	vips_buf_rewind( &colour->caption );
	vips_buf_appendf( &colour->caption, CLASS_COLOUR " %s [%g, %g, %g]",
		NN( colour->colour_space ),
		colour->value[0], colour->value[1], colour->value[2] );
}

void
colour_set_colour( Colour *colour, 
	const char *colour_space, double value[3] )
{
	int i;

	/* No change?
	 */
	for( i = 0; i < 3; i++ )
		if( !DEQ( value[i], colour->value[i] ) )
			break;
	if( i == 3 && strcmp( colour_space, colour->colour_space ) == 0 )
		return;

	for( i = 0; i < 3; i++ )
		colour->value[i] = value[i];
	IM_SETSTR( colour->colour_space, colour_space );

	colour_refresh( colour );
        classmodel_update( CLASSMODEL( colour ) );
        symbol_recalculate_all();
}

/* Code up a colour as an ii. Refcount zero! Will go on next GC.
 */
Imageinfo *
colour_ii_new( Colour *colour )
{
	Imageinfo *imageinfo;
	int i;

	if( !(imageinfo = imageinfo_new_temp( main_imageinfogroup, 
		reduce_context->heap, NULL, "t" )) ) 
		return( NULL );

	/* Make a 3 band 32-bit FLOAT memory image.
	 */
	im_initdesc( imageinfo->im, 1, 1, 3, 
		IM_BBITS_FLOAT, IM_BANDFMT_FLOAT, 
		IM_CODING_NONE, colour_get_vips_type( colour ),
		1.0, 1.0, 0, 0 );
	if( im_setupout( imageinfo->im ) ) 
		return( NULL );
	for( i = 0; i < 3; i++ )
		((float *) imageinfo->im->data)[i] = colour->value[i];

	return( imageinfo );
}

/* Convert our colour to rgb. Slow!
 */
static void
colour_get_rgb( Colour *colour, double rgb[4] )
{
	Imageinfo *imageinfo;

	if( (imageinfo = colour_ii_new( colour )) )
		imageinfo_to_rgb( imageinfo, rgb );
}

void
colour_set_rgb( Colour *colour, double rgb[4] )
{
	Imageinfo *imageinfo;

	if( (imageinfo = colour_ii_new( colour )) ) {
		double old_rgb[4];
		double value[3];
		int i;

		/* Setting as RGB can't express small differences since we're
		 * going via 8 bit RGB. So only accept the new value if it's 
		 * sufficiently different from
		 * what we have now.
		 */
		colour_get_rgb( colour, old_rgb );
		if( fabs( rgb[0] - old_rgb[0] ) > (0.5 / 255) ||
			fabs( rgb[1] - old_rgb[1] ) > (0.5 / 255) ||
			fabs( rgb[2] - old_rgb[2] ) > (0.5 / 255) ) {
			imageinfo_from_rgb( imageinfo, rgb );
			for( i = 0; i < 3; i++ )
				value[i] = ((float *) imageinfo->im->data)[i];
			colour_set_colour( colour, 
				colour->colour_space, value );
		}
	}
}

/* Done button hit.
 */
static void
colour_done_cb( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys )
{
	ColourEdit *eds = (ColourEdit *) client;
	Colour *colour = eds->colour;
	double rgb[4];

	gtk_color_selection_get_color( 
		GTK_COLOR_SELECTION( eds->colour_widget ), rgb );

	colour_set_rgb( colour, rgb );

	nfn( sys, IWINDOW_YES );
}

/* Build the insides of colour edit.
 */
static void
colour_buildedit( iDialog *idlg, GtkWidget *work, ColourEdit *eds )
{
	Colour *colour = eds->colour;
	double rgb[4];

	eds->colour_widget = gtk_color_selection_new();
	gtk_color_selection_set_has_opacity_control( 
		GTK_COLOR_SELECTION( eds->colour_widget ), FALSE );
	colour_get_rgb( colour, rgb );
	gtk_color_selection_set_color( 
		GTK_COLOR_SELECTION( eds->colour_widget ), rgb );
        gtk_box_pack_start( GTK_BOX( work ), 
		eds->colour_widget, TRUE, TRUE, 2 );

        gtk_widget_show_all( work );
}

static void
colour_edit( GtkWidget *parent, Model *model )
{
	Colour *colour = COLOUR( model );
	ColourEdit *eds = INEW( NULL, ColourEdit );
	GtkWidget *idlg;

	eds->colour = colour;

	idlg = idialog_new();
	iwindow_set_title( IWINDOW( idlg ), _( "Edit %s %s" ),
		IOBJECT_GET_CLASS_NAME( model ),
		IOBJECT( HEAPMODEL( model )->row )->name );
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) colour_buildedit, eds, NULL, NULL );
	idialog_set_callbacks( IDIALOG( idlg ), 
		iwindow_true_cb, NULL, idialog_free_client, eds );
	idialog_add_ok( IDIALOG( idlg ), 
		colour_done_cb, _( "Set %s" ), 
		IOBJECT_GET_CLASS_NAME( model ) );
	iwindow_set_parent( IWINDOW( idlg ), parent );
	idialog_set_iobject( IDIALOG( idlg ), IOBJECT( model ) );
	idialog_set_pinup( IDIALOG( idlg ), TRUE );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );
}

static View *
colour_view_new( Model *model, View *parent )
{
	return( colourview_new() );
}

static void *
colour_update_model( Heapmodel *heapmodel )
{
	Colour *colour = COLOUR( heapmodel );

	if( HEAPMODEL_CLASS( parent_class )->update_model( heapmodel ) )
		return( heapmodel );

	colour_refresh( colour );

	return( NULL );
}

/* Members of colour we automate.
 */
static ClassmodelMember colour_members[] = {
	{ CLASSMODEL_MEMBER_STRING, NULL, 0, 
		MEMBER_COLOUR_SPACE, "colour_space", N_( "Color Space" ),
		G_STRUCT_OFFSET( Colour, colour_space ) },
	{ CLASSMODEL_MEMBER_REALVEC_FIXED, NULL, 3,
		MEMBER_VALUE, "value", N_( "Value" ),
		G_STRUCT_OFFSET( Colour, value ) }
};

static void
colour_class_init( ColourClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	HeapmodelClass *heapmodel_class = (HeapmodelClass *) class;
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->finalize = colour_finalize;

	model_class->view_new = colour_view_new;
	model_class->edit = colour_edit;

	heapmodel_class->update_model = colour_update_model;

	/* Static init.
	 */
	model_register_loadable( MODEL_CLASS( class ) );

	classmodel_class->members = colour_members;
	classmodel_class->n_members = IM_NUMBER( colour_members );
}

static void
colour_init( Colour *colour )
{
	colour->value[0] = 0.0;
	colour->value[1] = 0.0;
	colour->value[2] = 0.0;
	colour->colour_space = NULL;
	vips_buf_init_dynamic( &colour->caption, MAX_LINELENGTH );

	iobject_set( IOBJECT( colour ), CLASS_COLOUR, NULL );
}

GType
colour_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ColourClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) colour_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Colour ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) colour_init,
		};

		type = g_type_register_static( TYPE_CLASSMODEL, 
			"Colour", &info, 0 );
	}

	return( type );
}
