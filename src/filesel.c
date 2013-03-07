/* ip's file selectors.
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

/* Define for debugging output.
#define DEBUG
 */

/* TIFF save possibilities. Needs to be kept in sync with the Option in
 * preferences.
 */
typedef enum {
	TIFF_COMPRESSION_NONE = 0,	/* No compression */
	TIFF_COMPRESSION_LZW,		/* Lempel-Ziv compression */
	TIFF_COMPRESSION_DEFLATE,	/* Zip (deflate) compression */
	TIFF_COMPRESSION_PACKBITS,	/* Packbits compression */
	TIFF_COMPRESSION_JPEG,		/* JPEG compression */
	TIFF_COMPRESSION_CCITTFAX4	/* Fax compression */
} TiffCompression;

typedef enum {
	TIFF_LAYOUT_STRIP = 0,		/* Strip TIFF */
	TIFF_LAYOUT_TILE		/* Tiled TIFF */
} TiffLayout;

typedef enum {
	TIFF_MULTIRES_FLAT = 0,		/* Flat file */
	TIFF_MULTIRES_PYRAMID		/* Pyramidal TIFF */
} TiffMultires;

typedef enum {
	TIFF_FORMAT_MANYBIT = 0,	/* No bit reduction */
	TIFF_FORMAT_ONEBIT		/* Reduce to 1 bit, where poss */
} TiffFormat;

/* Keep a list of all filesels currently active ... we use this for refresh on
 * new file.
 */
static GSList *filesel_all = NULL;

static iDialogClass *parent_class = NULL;

/* For filesels which don't have a suggested filename, track the last dir we
 * went to and use that as the start dir next time.
 */
static char *filesel_last_dir = NULL;

static const char *icc_suffs[] = { ".icc", ".icm", NULL };
static const char *workspace_suffs[] = { ".ws", NULL };
static const char *rec_suffs[] = { ".rec", NULL };
static const char *mor_suffs[] = { ".mor", NULL };
static const char *con_suffs[] = { ".con", NULL };
static const char *mat_suffs[] = { ".mat", NULL };
static const char *def_suffs[] = { ".def", NULL };
static const char *all_suffs[] = { "", NULL };

FileselFileType
        filesel_wfile_type = 
		{ N_( "Workspace files (*.ws)" ), workspace_suffs },
        filesel_rfile_type = 
		{ N_( "Recombination matrix files (*.rec)" ), rec_suffs },
        filesel_mfile_type = 
		{ N_( "Morphology matrix files (*.mor)" ), mor_suffs },
        filesel_cfile_type = 
		{ N_( "Convolution matrix files (*.con)" ), con_suffs },
        filesel_xfile_type = 
		{ N_( "Matrix files (*.mat)" ), mat_suffs },
        filesel_dfile_type = 
		{ N_( "Definition files (*.def)" ), def_suffs },
        filesel_ifile_type = 
		{ N_( "ICC profiles (*.icc, *.icm)" ), icc_suffs },
        filesel_allfile_type = 
		{ N_( "All files (*)" ), all_suffs };

FileselFileType
        *filesel_type_definition[] = { 
		&filesel_dfile_type, NULL 
	},
        *filesel_type_workspace[] = { 
		&filesel_wfile_type, NULL 
	},

        *filesel_type_matrix[] = { 
		&filesel_xfile_type, &filesel_cfile_type, &filesel_rfile_type, 
		&filesel_mfile_type, NULL 
	},

	/* Set during startup.
	 */
        **filesel_type_image = NULL,
        **filesel_type_mainw = NULL,
        **filesel_type_any = NULL;

static void *
build_vips_formats_sub( VipsFormatClass *format, GSList **types )
{
	FileselFileType *type = g_new( FileselFileType, 1 );
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );
	const char **i;

	vips_buf_appendf( &buf, 
		"%s ", VIPS_OBJECT_CLASS( format )->description );
	/* Used as eg. "VIPS image files (*.v)"
	 */
	vips_buf_appends( &buf, _( "image files" ) );
	vips_buf_appends( &buf, " (" );
	if( *format->suffs )
		for( i = format->suffs; *i; i++ ) {
			vips_buf_appendf( &buf, "*%s", *i );
			if( i[1] )
				vips_buf_appends( &buf, "; " );
		}
	else
		/* No suffix means any allowed.
		 */
		vips_buf_appendf( &buf, "*" );

	vips_buf_appends( &buf, ")" );

	type->name = g_strdup( vips_buf_all( &buf ) );
	type->suffixes = format->suffs;

	*types = g_slist_append( *types, type );

	return( NULL );
}

/* Look at the registered VIPS formats, build a file type list. Call from
 * filesel class init.
 */
static FileselFileType **
build_image_file_type( void )
{
	GSList *types;
	FileselFileType **type_array;

	types = NULL;
	vips_format_map( (VSListMap2Fn) build_vips_formats_sub, &types, NULL );

	type_array = (FileselFileType **) slist_to_array( types );
	g_slist_free( types );

	return( type_array );
}

/* Combine a NULL-terminated list of FileselFileType arrays into one.
 */
static FileselFileType **
build_file_type_va( FileselFileType **first, ... )
{
	va_list args;
	int len;
	FileselFileType **i;
	FileselFileType **array;
	int j;
	int k;

	/* Count total number of items.
	 */
	len = 0;
	va_start( args, first );
	for( i = first; i; i = va_arg( args, FileselFileType ** ) )
		len += array_len( (void **) i );
	va_end( args );

	/* Copy and NULL-terminate.
	 */
	array = g_new( FileselFileType *, len + 1 );
	va_start( args, first );
	j = 0;
	for( i = first; i; i = va_arg( args, FileselFileType ** ) ) 
		for( k = 0; i[k]; k++ )
			array[j++] = i[k];
	va_end( args );
	array[j] = NULL;

	return( array );
}

/* Here from main() during startup. We can't just put this in class_init,
 * because we want to be sure this happens early on.
 */
void
filesel_startup( void )
{
	filesel_type_image = build_image_file_type();
	filesel_type_mainw = build_file_type_va( filesel_type_image, 
		filesel_type_matrix, filesel_type_workspace, NULL );
	filesel_type_any = build_file_type_va( filesel_type_mainw,
		filesel_type_definition, NULL );
}

/* Is a file of type ... just look at the suffix.
 */
gboolean
is_file_type( const FileselFileType *type, const char *filename )
{
	const char **p;
	const char *suf;

	if( (suf = strrchr( filename, '.' )) ) {
		for( p = type->suffixes; *p; p++ )
			if( strcasecmp( suf, *p ) == 0 )
				return( TRUE );
	}

	return( FALSE );
}

/* Map TIFF formats to char* for VIPS.
 */
static char *
decode_tiff_compression( TiffCompression tc )
{
	switch( tc ) {
	case TIFF_COMPRESSION_LZW:	return( "lzw" );
	case TIFF_COMPRESSION_DEFLATE:	return( "deflate" );
	case TIFF_COMPRESSION_PACKBITS:	return( "packbits" );
	case TIFF_COMPRESSION_JPEG:	return( "jpeg" );
	case TIFF_COMPRESSION_CCITTFAX4:return( "ccittfax4" );

	case TIFF_COMPRESSION_NONE:
	default:
		return( "none" );
	}
}

static char *
decode_tiff_layout( TiffLayout tf )
{
	switch( tf ) {
	case TIFF_LAYOUT_TILE:		return( "tile" );

	case TIFF_LAYOUT_STRIP:
	default:
		return( "strip" );
	}
}

static char *
decode_tiff_multires( TiffMultires tm )
{
	switch( tm ) {
	case TIFF_MULTIRES_PYRAMID:	return( "pyramid" );

	case TIFF_MULTIRES_FLAT:
	default:
		return( "flat" );
	}
}

static char *
decode_tiff_format( TiffFormat tm )
{
	switch( tm ) {
	case TIFF_FORMAT_ONEBIT:	return( "onebit" );

	case TIFF_FORMAT_MANYBIT:
	default:
		return( "manybit" );
	}
}

/* Make a TIFF save format string.
 */
static void
filesel_tiff_mode( char *out )
{
	char ctype[FILENAME_MAX];
	char ltype[FILENAME_MAX];
	char buf[FILENAME_MAX];

	strcpy( ctype, decode_tiff_compression( IP_TIFF_COMPRESSION ) );
	if( IP_TIFF_COMPRESSION == TIFF_COMPRESSION_JPEG ) {
		im_snprintf( buf, FILENAME_MAX, ":%d", IP_TIFF_JPEG_Q );
		strcat( ctype, buf );
	}
	if( IP_TIFF_COMPRESSION == TIFF_COMPRESSION_DEFLATE ||
		IP_TIFF_COMPRESSION == TIFF_COMPRESSION_LZW ) {
		im_snprintf( buf, FILENAME_MAX, ":%d", IP_TIFF_PREDICTOR + 1 );
		strcat( ctype, buf );
	}

	strcpy( ltype, decode_tiff_layout( IP_TIFF_LAYOUT ) );
	if( IP_TIFF_LAYOUT == TIFF_LAYOUT_TILE ) {
		im_snprintf( buf, FILENAME_MAX, ":%dx%d", 
			IP_TIFF_TILE_WIDTH,
			IP_TIFF_TILE_WIDTH );
		strcat( ltype, buf );
	}

	im_snprintf( out, 256, "%s,%s,%s,%s,,,%s", 
		ctype, ltype, 
		decode_tiff_multires( IP_TIFF_MULTI_RES ),
		decode_tiff_format( IP_TIFF_FORMAT ),
		IP_TIFF_BIGTIFF ? "8" : "" );
}

/* Make a JPEG save format string.
 */
static void
filesel_jpeg_mode( char *out )
{
	char profile[FILENAME_MAX];

	switch( IP_JPEG_ICC_PROFILE ) {
	case 0:
		/* Use embedded profile ... do nothing.
		 */
		strcpy( profile, "" );

		break;

	case 1:
	{
		/* Embed from file.
		 */
		char buf[FILENAME_MAX];
		char buf2[FILENAME_MAX];

		im_strncpy( buf, IP_JPEG_ICC_PROFILE_FILE, FILENAME_MAX );
		expand_variables( buf, buf2 );
		nativeize_path( buf2 );
		im_snprintf( profile, FILENAME_MAX, ",%s", buf2 );

		break;
	}

	case 2:
		/* Don't attach a profile.
		 */
		im_snprintf( profile, FILENAME_MAX, ",none" );
		break;

	default:
		/* Again, do nothing.
		 */
		strcpy( profile, "" );

		break;
	}

	im_snprintf( out, 256, "%d%s", IP_JPEG_Q, profile );
}

/* Make a PNG save format string.
 */
static void
filesel_png_mode( char *out )
{
	im_snprintf( out, 256, "%d,%d", IP_PNG_COMPRESSION, IP_PNG_INTERLACE );
}

/* Make a PPM save format string.
 */
static void
filesel_ppm_mode( char *out )
{
	switch( IP_PPM_MODE ) {
	case 0:
		im_snprintf( out, 256, "binary" );
		break;

	default:
		im_snprintf( out, 256, "ascii" );
		break;
	}
}

/* Make a CSV save format string.
 */
static void
filesel_csv_mode( char *out )
{
	/* We have to escape ":" and "," characters in the separator string.
	 */
	char separator[256];

	escape_mode( IP_CSV_SEPARATOR, separator, 256 );

	im_snprintf( out, 256, "sep:%s", separator );
}

typedef void (*make_mode_fn)( char *buf );

typedef struct {
	const char *caption_filter;	/* nip2 column name for the format */
	const char *name;		/* vips nickname for the format */
	make_mode_fn mode_fn;		/* Build a mode string */
} FileselMode;

static FileselMode filesel_mode_table[] = {
	{ "JPEG", "jpeg", filesel_jpeg_mode },
	{ "PNG", "png", filesel_png_mode },
	{ "TIFF", "tiff", filesel_tiff_mode },
	{ "CSV", "csv", filesel_csv_mode },
	{ "PPM", "ppm", filesel_ppm_mode }
};

static FileselMode *
filesel_get_mode( const char *filename )
{
	int i;
	VipsFormatClass *format;

	if( (format = vips_format_for_name( filename )) ) {
		VipsObjectClass *object_class = VIPS_OBJECT_CLASS( format );

		for( i = 0; i < IM_NUMBER( filesel_mode_table ); i++ )
			if( strcmp( filesel_mode_table[i].name, 
				object_class->nickname ) == 0 ) 
				return( &filesel_mode_table[i] );
	}
	else
		im_error_clear();

	return( NULL );
}

/* Add our image save settings to the end of a filename. filename must be
 * at least FILENAME_MAX characters in size.
 */
void
filesel_add_mode( char *filename )
{
	FileselMode *mode;

	if( (mode = filesel_get_mode( filename )) ) {
		char ext[256];
		int l = strlen( filename );

		mode->mode_fn( ext );
		im_snprintf( filename + l, FILENAME_MAX - l, ":%s", ext );
	}
}

static const char *
filesel_get_filter( const char *filename )
{
	FileselMode *mode;

	if( (mode = filesel_get_mode( filename )) ) 
		return( mode->caption_filter );

	return( NULL );
}

static void
filesel_destroy( GtkObject *object )
{
	Filesel *filesel;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_FILESEL( object ) );

	filesel = FILESEL( object );

	filesel_all = g_slist_remove( filesel_all, filesel );
	IM_FREEF( g_free, filesel->current_dir );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

/* Update `space free' label.
 */
static void
filesel_space_update( Filesel *filesel, const char *dirname )
{
	double sz = find_space( dirname );

	if( filesel->space ) {
		if( sz < 0 )
			set_glabel( filesel->space, 
				_( "Unable to determine "
					"space free in \"%s\"." ), 
				dirname );
		else {
			char txt[MAX_STRSIZE];
			VipsBuf buf = VIPS_BUF_STATIC( txt );

			vips_buf_append_size( &buf, sz );
			vips_buf_appendf( &buf, " " );
			/* Expands to (eg.) '6GB free in "/pics/tmp"'
			 */
			vips_buf_appendf( &buf, _( "free in \"%s\"" ), dirname );
			set_glabel( filesel->space, "%s", vips_buf_all( &buf ) );
		}
	}
}

static void *
filesel_add_volume( const char *dir, Filesel *filesel )
{
	char buf[FILENAME_MAX];

	im_strncpy( buf, dir, FILENAME_MAX );
	path_expand( buf );

	gtk_file_chooser_add_shortcut_folder( 
		GTK_FILE_CHOOSER( filesel->chooser ), buf, NULL );

	return( NULL );
}

static void
filesel_suffix_to_glob( const char *suffix, VipsBuf *patt )
{
	int i;
	char ch;

	vips_buf_appends( patt, "*" );

	for( i = 0; (ch = suffix[i]); i++ ) {
		if( isalpha( ch ) ) {
			vips_buf_appends( patt, "[" );
			vips_buf_appendf( patt, "%c", toupper( ch ) );
			vips_buf_appendf( patt, "%c", tolower( ch ) );
			vips_buf_appends( patt, "]" );
		}
		else
			vips_buf_appendf( patt, "%c", ch );
	}
}

/* Make a shell glob from a filetype.
 */
void
filesel_make_patt( FileselFileType *type, VipsBuf *patt )
{
	int i;

	/* Only use {} braces if there's more than one suffix to match.
	 */
	if( type->suffixes[1] )
		vips_buf_appends( patt, "{" );

	for( i = 0; type->suffixes[i]; i++ ) {
		if( i > 0 )
			vips_buf_appends( patt, "," );

		filesel_suffix_to_glob( type->suffixes[i], patt );
	}

	if( type->suffixes[1] )
		vips_buf_appends( patt, "}" );
}

static char *
filesel_get_dir( Filesel *filesel )
{
	return( gtk_file_chooser_get_current_folder(
		GTK_FILE_CHOOSER( filesel->chooser ) ) );
}

static void
filesel_dir_enter( Filesel *filesel )
{
	char *dir = filesel_get_dir( filesel );

	if( !filesel->current_dir || 
		(dir && strcmp( filesel->current_dir, dir ) != 0) ) {
		filesel_space_update( filesel, dir );
		if( !filesel->start_name )
			IM_SETSTR( filesel_last_dir, dir );

		filesel->current_dir = dir;
		dir = NULL;
	}

	g_free( dir );
}

/* New dir entered signal.
 */
static void
filesel_current_folder_changed_cb( GtkWidget *widget, gpointer data )
{
	filesel_dir_enter( FILESEL( data ) );
}

/* Update file info display.
 */
static void
filesel_info_update( Filesel *filesel, const char *name )
{
	if( filesel->info ) {
		char txt[MAX_STRSIZE];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		get_image_info( &buf, name );
		set_glabel( filesel->info, "%s", vips_buf_firstline( &buf ) );
	}
}

int
filesel_get_filetype( Filesel *filesel )
{
	int type;
	GtkFileFilter *filter;

	type = filesel->default_type;

	if( filesel->chooser &&
		(filter = gtk_file_chooser_get_filter( 
			GTK_FILE_CHOOSER( filesel->chooser ) )) )  {
		int i;

		for( i = 0; filesel->filter[i]; i++ ) 
			if( filter == filesel->filter[i] )
				break;
		g_assert( filesel->filter[i] );

		type = i;
	}

#ifdef DEBUG
	printf( "filesel_get_filetype: %d\n", type ); 
#endif /*DEBUG*/

	return( type );
}

/* Find the index of the type which matches this filename.
 */
static int
filesel_find_file_type( FileselFileType **type, const char *filename )
{
	int i, j;

	for( i = 0; type[i]; i++ ) 
		for( j = 0; type[i]->suffixes[j]; j++ ) 
			if( is_casepostfix( type[i]->suffixes[j], filename ) )
				return( i );

	return( -1 );
}

static void
filesel_set_filter( Filesel *filesel, GtkFileFilter *filter )
{
#ifdef DEBUG
	printf( "filesel_set_filter: %p\n", filter ); 
#endif /*DEBUG*/

	g_assert( filter );

	gtk_file_chooser_set_filter( GTK_FILE_CHOOSER( filesel->chooser ),
		filter );
}

static void
filesel_set_filetype_from_filename( Filesel *filesel, const char *name )
{
	int type;
	int i;
	char *p;

	/* If we're showing "all", any filename is OK, so don't change the file
	 * type.
	 */
	type = filesel_get_filetype( filesel );
	if( type == filesel->ntypes - 1 )
		return;

	/* If we've not got a sensible filename, don't bother.
	 */
	if( (p = strrchr( name, G_DIR_SEPARATOR )) &&
		strspn( p + 1, " \n\t" ) == strlen( p + 1 ) ) 
		return;

	if( (i = filesel_find_file_type( filesel->type, name )) >= 0 )
		filesel_set_filter( filesel, filesel->filter[i] );
	else
		/* No match, or no suffix. Set the last type (should be "All").
		 */
		filesel_set_filter( filesel, 
			filesel->filter[filesel->ntypes - 1] );
}

gboolean 
filesel_set_filename( Filesel *filesel, const char *name )
{
	char buf[FILENAME_MAX];

	if( !is_valid_filename( name ) ) 
		return( FALSE );

	im_strncpy( buf, name, FILENAME_MAX );
	path_expand( buf );

#ifdef DEBUG
	printf( "filesel_set_filename: %s\n", buf ); 
#endif /*DEBUG*/

	/* set_filename() will only select existing files, we need to be able
	 * to set any filename (eg. for increment filename), so we have to
	 * set_current_name() as well.
	 */
	gtk_file_chooser_set_filename( 
		GTK_FILE_CHOOSER( filesel->chooser ), buf );

	if( filesel->save )
		gtk_file_chooser_set_current_name(
			GTK_FILE_CHOOSER( filesel->chooser ), 
			im_skip_dir( buf ) );

	filesel->start_name = TRUE;

	/* We have to set this after setting the filename.
	 */
	filesel_set_filetype_from_filename( filesel, buf );

	return( TRUE );
}

/* Read the filename out ... test for sanity.
 */
char *
filesel_get_filename( Filesel *filesel )
{
	char *name;
	char tmp[FILENAME_MAX];

	name = gtk_file_chooser_get_filename(
		GTK_FILE_CHOOSER( filesel->chooser ) );

#ifdef DEBUG
	printf( "filesel_get_filename: %s\n", name ); 
#endif /*DEBUG*/

	if( !name ) {
		error_top( _( "Bad filename." ) );
		error_sub( _( "No file selected." ) );
		return( NULL );
	}
	if( !is_valid_filename( name ) ) {
		g_free( name );
		return( NULL );
	}

	/* Rewrite to compact form, eg. "$HOME/fred".
	 */
	im_strncpy( tmp, name, FILENAME_MAX );
	path_compact( tmp );
	g_free( name );

	return( g_strdup( tmp ) );
}

/* Get filename multi ... map over the selected filenames.
 */
void *
filesel_map_filename_multi( Filesel *filesel,
	FileselMapFn fn, void *a, void *b )
{
	GSList *names = gtk_file_chooser_get_filenames( 
		GTK_FILE_CHOOSER( filesel->chooser ) );
	GSList *p;

	for( p = names; p; p = p->next ) {
		char *filename = (char *) p->data;

		char tmp[FILENAME_MAX];
		void *res;

		im_strncpy( tmp, filename, FILENAME_MAX );
		path_compact( tmp );

		if( (res = fn( filesel, tmp, a, b )) ) {
			IM_FREEF( slist_free_all, names );
			return( res );
		}
	}
	IM_FREEF( slist_free_all, names );

	return( NULL );
}

/* New file selected signal.
 */
static void
filesel_selection_changed_cb( GtkWidget *widget, gpointer data )
{
	Filesel *filesel = FILESEL( data );
        char *filename;

#ifdef DEBUG
	printf( "filesel_selection_changed_cb: %s\n", 
		NN( IWINDOW( filesel )->title ) );
#endif /*DEBUG*/

	if( (filename = filesel_get_filename( filesel )) ) {
#ifdef DEBUG
		printf( "filesel_selection_changed_cb: %s - \"%s\"\n", 
			NN( IWINDOW( filesel )->title ), filename );
#endif /*DEBUG*/

		filesel_info_update( filesel, filename );
		g_free( filename );
	}
}

static void
filesel_file_activated_cb( GtkWidget *widget, gpointer data )
{
	idialog_done_trigger( IDIALOG( data ), 0 );
}

/* Increment filename on OK.
 */
static void
filesel_auto_incr_cb( GtkWidget *tog, Filesel *filesel )
{
        filesel->incr = GTK_TOGGLE_BUTTON( tog )->active;

	if( filesel->incr )
		idialog_set_pinup( IDIALOG( filesel ), TRUE );
}

static void
filesel_update_preview_cb( GtkFileChooser *chooser, Filesel *filesel )
{
        char *filename;

	if( (filename = gtk_file_chooser_get_preview_filename(
		GTK_FILE_CHOOSER( filesel->chooser ) )) ) {
		preview_set_filename( filesel->preview, filename );
		g_free( filename );
	}
}

static GtkFileFilter *
file_filter_from_file_type( FileselFileType *type )
{
	GtkFileFilter *filter;
	int j;
	
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name( filter, _( type->name ) );

	if( type->suffixes[0] )
		for( j = 0; type->suffixes[j]; j++ ) {
			char txt[FILENAME_MAX];
			VipsBuf buf = VIPS_BUF_STATIC( txt );

			filesel_suffix_to_glob( type->suffixes[j], &buf );
			gtk_file_filter_add_pattern( filter, 
				vips_buf_all( &buf ) );
		}
	else
		/* No suffix list means any suffix allowed.
		 */
		gtk_file_filter_add_pattern( filter, "*" );

	return( filter );
}

static void
filesel_add_filter( Filesel *filesel, FileselFileType *type, int i )
{
	filesel->filter[i] = file_filter_from_file_type( type );

#ifdef DEBUG
	printf( "filesel_add_filter: %p (%d)\n", filesel->filter[i], i ); 
#endif /*DEBUG*/

	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER( filesel->chooser ),
		filesel->filter[i] );

	if( i == filesel->default_type )
		filesel_set_filter( filesel, filesel->filter[i] );
}

static void
filesel_build( GtkWidget *widget )
{
	Filesel *filesel = FILESEL( widget );
	iDialog *idlg = IDIALOG( widget );

	int i;
	FileselFileType *type;
	GtkWidget *vb;
	GtkWidget *tog;

#ifdef DEBUG
	printf( "filesel_build: %s\n", NN( IWINDOW( filesel )->title ) );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( parent_class )->build )
		IWINDOW_CLASS( parent_class )->build( widget );

	filesel->chooser = gtk_file_chooser_widget_new( filesel->save ? 
			GTK_FILE_CHOOSER_ACTION_SAVE :
			GTK_FILE_CHOOSER_ACTION_OPEN );
	gtk_file_chooser_set_select_multiple( 
		GTK_FILE_CHOOSER( filesel->chooser ), filesel->multi );
        gtk_box_pack_start( GTK_BOX( idlg->work ), 
		filesel->chooser, TRUE, TRUE, 0 );
	gtk_widget_show( filesel->chooser );

	/* Add data path to volumes.
	 */
        slist_map( PATH_SEARCH,
		(SListMapFn) filesel_add_volume, filesel );

	/* Add all the supported file types. Add "all" to the end.
	 */
	for( i = 0; (type = filesel->type[i]); i++ ) 
		filesel_add_filter( filesel, type, i );
	filesel_add_filter( filesel, &filesel_allfile_type, i );

        /* Spot changes.
         */
        gtk_signal_connect( GTK_OBJECT( filesel->chooser ), 
		"current-folder-changed",
                GTK_SIGNAL_FUNC( filesel_current_folder_changed_cb ), filesel );
        gtk_signal_connect( GTK_OBJECT( filesel->chooser ), 
		"selection-changed",
                GTK_SIGNAL_FUNC( filesel_selection_changed_cb ), filesel );
        gtk_signal_connect( GTK_OBJECT( filesel->chooser ), 
		"file-activated",
                GTK_SIGNAL_FUNC( filesel_file_activated_cb ), filesel );

	/* Pack extra widgets.
	 */
        vb = gtk_vbox_new( FALSE, 6 );
	gtk_file_chooser_set_extra_widget(
		GTK_FILE_CHOOSER( filesel->chooser ), vb );
	gtk_widget_show( vb );

        /* Space free label. 
         */
	if( filesel->save ) {
		filesel->space = gtk_label_new( "" );
		gtk_misc_set_alignment( GTK_MISC( filesel->space ), 0, 0.5 );
		gtk_box_pack_start( GTK_BOX( vb ), 
			filesel->space, FALSE, FALSE, 0 );
		gtk_widget_show( filesel->space );
	}

        /* File info label.
         */
	if( !filesel->save ) {
		filesel->info = gtk_label_new( "" );
		gtk_misc_set_alignment( GTK_MISC( filesel->info ), 0, 0.5 );
		gtk_box_pack_start( GTK_BOX( vb ), 
			filesel->info, FALSE, FALSE, 0 );
		gtk_widget_show( filesel->info );
	}

        /* Auto-increment toggle.
         */
	if( filesel->save ) {
		tog = gtk_check_button_new_with_label( 
			_( "Increment filename" ) );
		gtk_signal_connect( GTK_OBJECT( tog ), "toggled",
			GTK_SIGNAL_FUNC( filesel_auto_incr_cb ), filesel );
		gtk_box_pack_start( GTK_BOX( vb ), tog, FALSE, FALSE, 0 );
		gtk_widget_show( tog );
		set_tooltip( tog, 
			_( "After Save, add 1 to the last number in the "
			"file name" ) );
	}

        if( filesel->imls ) {
		filesel->preview = preview_new();
		gtk_file_chooser_set_preview_widget(
			GTK_FILE_CHOOSER( filesel->chooser ), 
			GTK_WIDGET( filesel->preview ) );
		gtk_signal_connect( GTK_OBJECT( filesel->chooser ), 
			"update-preview",
			GTK_SIGNAL_FUNC( filesel_update_preview_cb ), filesel );
		gtk_widget_show( GTK_WIDGET( filesel->preview ) );
		gtk_file_chooser_set_preview_widget_active(
			GTK_FILE_CHOOSER( filesel->chooser ), TRUE );
	}

	if( filesel_last_dir ) 
		gtk_file_chooser_set_current_folder(
			GTK_FILE_CHOOSER( filesel->chooser ), 
			filesel_last_dir );

	/* Save boxes can be much smaller.
	 */
	if( !filesel->save )
		gtk_window_set_default_size( GTK_WINDOW( filesel ), 600, 500 );
}

static void
filesel_class_init( FileselClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	iWindowClass *iwindow_class = (iWindowClass *) class;

	object_class->destroy = filesel_destroy;

	iwindow_class->build = filesel_build;

	parent_class = g_type_class_peek_parent( class );
}

/* Increment filename. If there's no number there now, assume zero.
 */
static void
filesel_increment_filename( Filesel *filesel )
{
        char *filename;

	if( (filename = filesel_get_filename( filesel )) ) {
		char name[FILENAME_MAX];

		im_strncpy( name, filename, FILENAME_MAX );
		g_free( filename );
		increment_filename( name );

		(void) filesel_set_filename( filesel, name );
	}
}

static void *
filesel_refresh( Filesel *filesel )
{
	char *dir;

	dir = gtk_file_chooser_get_current_folder( 
		GTK_FILE_CHOOSER( filesel->chooser ) );
	gtk_file_chooser_set_current_folder(
		GTK_FILE_CHOOSER( filesel->chooser ), dir );
	g_free( dir );

        return( NULL );
}

/* There may be a new file ... ask all fsb's to refresh.
 */
void
filesel_refresh_all( void )
{
        (void) slist_map( filesel_all, (SListMapFn) filesel_refresh, NULL );
}

static void
filesel_init( Filesel *filesel )
{
	int i;

#ifdef DEBUG
	printf( "filesel_init: %s\n", NN( IWINDOW( filesel )->title ) );
#endif /*DEBUG*/

	filesel->chooser = NULL;
	filesel->space = NULL;
	filesel->info = NULL;
	filesel->preview = NULL;
	for( i = 0; i < FILESEL_MAX_FILTERS; i++ )
		filesel->filter[i] = NULL;
	filesel->incr = FALSE;
	filesel->imls = FALSE;
	filesel->save = FALSE;
	filesel->multi = FALSE;
	filesel->start_name = FALSE;
	filesel->type = NULL;
	filesel->default_type = 0;
	filesel->type_pref = NULL;
	filesel->current_dir = NULL;
	filesel->done_cb = NULL;
	filesel->client = NULL;

	idialog_set_callbacks( IDIALOG( filesel ), 
		iwindow_true_cb, NULL, NULL, NULL );
	idialog_set_pinup( IDIALOG( filesel ), TRUE );
	idialog_set_nosep( IDIALOG( filesel ), TRUE );
	idialog_set_button_focus( IDIALOG( filesel ), FALSE );
	idialog_set_help_tag( IDIALOG( filesel ), "sec:loadsave" );

	filesel_all = g_slist_prepend( filesel_all, filesel );
}

GtkType
filesel_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Filesel",
			sizeof( Filesel ),
			sizeof( FileselClass ),
			(GtkClassInitFunc) filesel_class_init,
			(GtkObjectInitFunc) filesel_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_IDIALOG, &info );
	}

	return( type );
}

GtkWidget *
filesel_new( void )
{
	Filesel *filesel = (Filesel *) gtk_type_new( TYPE_FILESEL );

	iwindow_set_size_prefs( IWINDOW( filesel ), 
		"FILESEL_WINDOW_WIDTH", "FILESEL_WINDOW_HEIGHT" );

	return( GTK_WIDGET( filesel ) );
}

void 
filesel_set_done( Filesel *filesel, iWindowFn done_cb, void *client )
{
	filesel->done_cb = done_cb;
	filesel->client = client;
}

/* Back from the user function ... unset the hourglass, and update.
 */
static void
filesel_trigger2( void *sys, iWindowResult result )
{
	iWindowSusp *susp = (iWindowSusp *) sys;
	Filesel *filesel = FILESEL( susp->client );

	progress_end();

	/* If this is a save, assume that there is now a new file, 
	 * and ask all fsb's to update.
	 */
	if( filesel->save && result != IWINDOW_ERROR )
		filesel_refresh_all();

	if( result != IWINDOW_YES ) {
		/* Failure ... bomb out.
		 */
		iwindow_susp_return( susp, result );
		return;
	}

	/* Increment the filename, if required.
	 */
	if( filesel->incr ) {
		filesel_increment_filename( filesel );
		filesel_refresh( filesel );
	}

	/* Success!
	 */
	iwindow_susp_return( susp, result );
}

/* Start of user done ... shut down our suspension, and set the hglass.
 */
static void
filesel_trigger( Filesel *filesel, iWindow *iwnd, 
	iWindowNotifyFn nfn, void *sys )
{
	/* Suspend the callback for a bit.
	 */
	iWindowSusp *susp = iwindow_susp_new( NULL, iwnd, filesel, nfn, sys );

	/* If there's a filetype pref, update it.
	 */
	if( filesel->type_pref )
		prefs_set( filesel->type_pref, 
			"%d", filesel_get_filetype( filesel ) );

	progress_begin();
	filesel->done_cb( IWINDOW( filesel ), 
		filesel->client, filesel_trigger2, susp );
}

static void
filesel_prefs_ok_cb( iWindow *iwnd, void *client,
	iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( client );

	/* Force a recalc, in case we've changed the autorecalc 
	 * settings. Also does a scan on any widgets.
	 */
	symbol_recalculate_all_force( TRUE );

	filesel_trigger( filesel, iwnd, nfn, sys );
}

static void
filesel_prefs( Filesel *filesel, iWindow *iwnd, 
	const char *caption_filter, 
	iWindowNotifyFn nfn, void *sys )
{
	Prefs *prefs;

	if( !(prefs = prefs_new( caption_filter )) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	/* Expands to (eg.) "TIFF Save Preferences".
	 */
	iwindow_set_title( IWINDOW( prefs ), 
		_( "%s Save Preferences" ), caption_filter );
	iwindow_set_parent( IWINDOW( prefs ), GTK_WIDGET( iwnd ) );
	idialog_set_callbacks( IDIALOG( prefs ), 
		iwindow_true_cb, NULL, NULL, filesel );
	idialog_add_ok( IDIALOG( prefs ), filesel_prefs_ok_cb, GTK_STOCK_SAVE );
	idialog_set_notify( IDIALOG( prefs ), nfn, sys );
	iwindow_build( IWINDOW( prefs ) );

	gtk_widget_show( GTK_WIDGET( prefs ) );
}

/* We have a filename and it's OK to overwrite. Is it a type for which we have
 * to offer preferences?
 */
static void
filesel_yesno_cb( iWindow *iwnd, void *client,
	iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( client );
	char *filename;
	const char *caption_filter;

	if( filesel->save ) {
		if( !(filename = filesel_get_filename( filesel )) ) 
			nfn( sys, IWINDOW_ERROR );
		else {
			if( (caption_filter = filesel_get_filter( filename )) ) 
				filesel_prefs( filesel, iwnd, caption_filter,
					nfn, sys );
			else
				filesel_trigger( filesel, iwnd, nfn, sys );

			g_free( filename );
		}
	}
	else
		filesel_trigger( filesel, iwnd, nfn, sys );
}

static void
filesel_done_cb( iWindow *iwnd, void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
        char *filename;

#ifdef DEBUG
	printf( "filesel_done\n" );
#endif /*DEBUG*/

	if( !(filename = filesel_get_filename( filesel )) ) 
		nfn( sys, IWINDOW_ERROR );
	else if( isdir( "%s", filename ) ) {
		nfn( sys, IWINDOW_NO );
	}
	else {
		/* File exists and we are saving? Do a yesno before we carry on.
		 */
		if( filesel->save && existsf( "%s", filename ) ) {
			box_yesno( GTK_WIDGET( filesel ),
				filesel_yesno_cb, iwindow_true_cb, filesel,
				nfn, sys,
				_( "Overwrite" ),
				_( "Overwrite file?" ),
				_( "File \"%s\" exists. "
				"OK to overwrite?" ), filename );
		}
		else
			/* Just call the user function directly.
			 */
			filesel_yesno_cb( iwnd, filesel, nfn, sys );

		g_free( filename );
	}
}

void 
filesel_set_flags( Filesel *filesel, gboolean imls, gboolean save )
{
	filesel->imls = imls;
	filesel->save = save;

	idialog_add_ok( IDIALOG( filesel ), filesel_done_cb, 
		save ? GTK_STOCK_SAVE : GTK_STOCK_OPEN );
}

void 
filesel_set_filetype( Filesel *filesel, 
	FileselFileType **type, int default_type )
{
	/* Reset the widget, if it's there.
	 */
	if( filesel->chooser )
		filesel_set_filter( filesel, filesel->filter[default_type] );

	filesel->type = type;
	filesel->ntypes = array_len( (void **) type );
	filesel->default_type = default_type;
}

void 
filesel_set_filetype_pref( Filesel *filesel, 
	const char *type_pref )
{
	filesel->type_pref = type_pref;
}

void
filesel_set_multi( Filesel *filesel, gboolean multi )
{
	filesel->multi = multi;
}
