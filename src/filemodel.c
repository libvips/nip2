/* abstract base class for things which form the filemodel half of a 
 * filemodel/view pair
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

/* Don't compress save files.

 	FIXME ... some prebuilt libxml2s on win32 don't support libz
	compression, so don't turn this off

 */
#define DEBUG_SAVEFILE

#include "ip.h"

static ModelClass *parent_class = NULL;

static GSList *filemodel_registered = NULL;

/* Register a file model. Registered models are part of the "xxx has been
 * modified, save before quit?" check.
 */
void 
filemodel_register( Filemodel *filemodel )
{
	if( !filemodel->registered ) {
		filemodel->registered = TRUE;
		filemodel_registered = g_slist_prepend( filemodel_registered, 
			filemodel );

#ifdef DEBUG
		printf( "filemodel_register: %s \"%s\" (%p)\n",
			G_OBJECT_TYPE_NAME( filemodel ),
			IOBJECT( filemodel )->name,
			filemodel );
#endif /*DEBUG*/
	}
}

void 
filemodel_unregister( Filemodel *filemodel )
{
	if( filemodel->registered ) {
		filemodel->registered = FALSE;
		filemodel_registered = g_slist_remove( filemodel_registered, 
			filemodel );

#ifdef DEBUG
		printf( "filemodel_unregister: %s \"%s\" (%p)\n",
			G_OBJECT_TYPE_NAME( filemodel ),
			IOBJECT( filemodel )->name,
			filemodel );
#endif /*DEBUG*/
	}
}

/* Trigger the top_load method for a filemodel.
 */
void *
filemodel_top_load( Filemodel *filemodel, 
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	FilemodelClass *filemodel_class = FILEMODEL_GET_CLASS( filemodel );

	if( filemodel_class->top_load ) {
		if( !filemodel_class->top_load( filemodel, state, 
			parent, xnode ) )
			return( filemodel );
	}
	else {
		error_top( _( "Not implemented." ) );
		error_sub( _( "_%s() not implemented for class \"%s\"." ), 
			"top_load",
			G_OBJECT_CLASS_NAME( filemodel_class ) );

		return( filemodel );
	}

	return( NULL );
}

/* Trigger the set_modified method for a filemodel.
 */
void 
filemodel_set_modified( Filemodel *filemodel, gboolean modified )
{
	FilemodelClass *filemodel_class = FILEMODEL_GET_CLASS( filemodel );

	if( filemodel_class->set_modified ) 
		filemodel_class->set_modified( filemodel, modified );
}

gboolean
filemodel_save_all( Filemodel *filemodel, const char *filename )
{
	FilemodelClass *filemodel_class = FILEMODEL_GET_CLASS( filemodel );

	if( filemodel_class->save_all ) 
		return( filemodel_class->save_all( filemodel, filename ) );
	else {
		error_top( _( "Not implemented." ) );
		error_sub( _( "_%s() not implemented for class \"%s\"." ), 
			"save_all",
			G_OBJECT_CLASS_NAME( filemodel_class ) );

		return( FALSE );
	}
}

static void
filemodel_info( iObject *iobject, BufInfo *buf )
{
	Filemodel *filemodel = FILEMODEL( iobject );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );

	buf_appendf( buf, "filename = \"%s\"\n", NN( filemodel->filename ) );
	buf_appendf( buf, "modified = \"%s\"\n", 
		bool_to_char( filemodel->modified ) );
	buf_appendf( buf, "registered = \"%s\"\n", 
		bool_to_char( filemodel->registered ) );
	buf_appendf( buf, "auto_load = \"%s\"\n", 
		bool_to_char( filemodel->auto_load ) );
}

/* filename can be NULL for unset.
 */
void
filemodel_set_filename( Filemodel *filemodel, const char *filename )
{
	if( filemodel->filename != filename ) {
		IM_SETSTR( filemodel->filename, filename );
		iobject_changed( IOBJECT( filemodel ) );
	}
}

static void
filemodel_finalize( GObject *gobject )
{
	Filemodel *filemodel;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_FILEMODEL( gobject ) );

	filemodel = FILEMODEL( gobject );

#ifdef DEBUG
	printf( "filemodel_finalize: %s \"%s\" (%s)\n", 
		G_OBJECT_TYPE_NAME( filemodel ), 
		NN( IOBJECT( filemodel )->name ),
		NN( filemodel->filename ) );
#endif /*DEBUG*/

	IM_FREE( filemodel->filename );
	filemodel_unregister( filemodel );

	G_OBJECT_CLASS( parent_class )->finalize( gobject );
}

static xmlNode *
filemodel_save( Model *model, xmlNode *xnode )
{
	Filemodel *filemodel = FILEMODEL( model );
	xmlNode *xthis;

	if( !(xthis = MODEL_CLASS( parent_class )->save( model, xnode )) )
		return( NULL );

	if( !set_sprop( xthis, "filename", filemodel->filename ) )
		return( NULL );

	return( xthis );
}

static gboolean 
filemodel_load( Model *model,
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	Filemodel *filemodel = FILEMODEL( model );

	char buf[MAX_STRSIZE];

	if( get_sprop( xnode, "filename", buf, MAX_STRSIZE ) )
		filemodel_set_filename( filemodel, buf );

	if( !MODEL_CLASS( parent_class )->load( model, state, parent, xnode ) )
		return( FALSE );

	return( TRUE );
}

static gboolean 
filemodel_real_top_load( Filemodel *filemodel,
	ModelLoadState *state, Model *parent, xmlNode *xnode )
{
	return( TRUE );
}

static void 
filemodel_real_set_modified( Filemodel *filemodel, gboolean modified )
{
	if( filemodel->modified != modified ) {
#ifdef DEBUG
		printf( "filemodel_real_set_modified: %s \"%s\" (%s) %s\n", 
			G_OBJECT_TYPE_NAME( filemodel ), 
			NN( IOBJECT( filemodel )->name ),
			NN( filemodel->filename ),
			bool_to_char( modified ) );
#endif /*DEBUG*/

		filemodel->modified = modified;

		iobject_changed( IOBJECT( filemodel ) );
	}
}

/* Save to filemodel->filename.
 */
static gboolean
filemodel_save_all_xml( Filemodel *filemodel, const char *filename )
{
	xmlDoc *xdoc;
	char namespace[256];

	if( !(xdoc = xmlNewDoc( (xmlChar *) "1.0" )) ) {
		error_top( _( "XML library error." ) );
		error_sub( _( "model_save_filename: xmlNewDoc() failed" ) );
		return( FALSE );
	}

#ifndef DEBUG_SAVEFILE
	xmlSetDocCompressMode( xdoc, 1 );
#endif /*!DEBUG_SAVEFILE*/

	im_snprintf( namespace, 256, "%s/%d.%d.%d",
		NAMESPACE, 
		filemodel->major, filemodel->minor, filemodel->micro );
	if( !(xdoc->children = xmlNewDocNode( xdoc, 
		NULL, (xmlChar *) "root", NULL )) ||
		!set_sprop( xdoc->children, "xmlns", namespace ) ) {
		error_top( _( "XML library error." ) );
		error_sub( _( "model_save_filename: xmlNewDocNode() failed" ) );
		xmlFreeDoc( xdoc );
		return( FALSE );
	}

	column_set_offset( filemodel->x_off, filemodel->y_off );
	if( model_save( MODEL( filemodel ), xdoc->children ) ) {
		xmlFreeDoc( xdoc );
		return( FALSE );
	}

	prettify_tree( xdoc );

	if( calli_string_filename( 
		(calli_string_fn) xmlSaveFile, 
			filename, xdoc, NULL, NULL ) == -1 ) {
		error_top( _( "Save failed." ) );
		error_sub( _( "Save of %s \"%s\" to file \"%s\" failed.\n%s" ),
			G_OBJECT_TYPE_NAME( filemodel ), 
			NN( IOBJECT( filemodel )->name ),
			NN( filename ),
			g_strerror( errno ) );

		xmlFreeDoc( xdoc );
		return( FALSE );
	}

	xmlFreeDoc( xdoc );

	return( TRUE );
}

static gboolean
filemodel_save_all_text( Filemodel *filemodel, const char *filename )
{
	iOpenFile *of;

	if( !(of = file_open_write( "%s", filename )) ) 
		return( FALSE );

	column_set_offset( filemodel->x_off, filemodel->y_off );
	if( model_save_text( MODEL( filemodel ), of ) ) {
		file_close( of );
		return( FALSE );
	}
	file_close( of );

	return( TRUE );
}

static gboolean
filemodel_real_save_all( Filemodel *filemodel, const char *filename )
{
	ModelClass *model_class = MODEL_GET_CLASS( filemodel );

#ifdef DEBUG
	printf( "filemodel_real_save_all: save %s \"%s\" to file \"%s\"\n", 
		G_OBJECT_TYPE_NAME( filemodel ), 
		NN( IOBJECT( filemodel )->name ),
		filename );
#endif /*DEBUG*/

	if( model_class->save_text ) {
		if( !filemodel_save_all_text( filemodel, filename ) )
			return( FALSE );
	}
	else if( model_class->save ) {
		if( !filemodel_save_all_xml( filemodel, filename ) )
			return( FALSE );
	}
	else {
		error_top( _( "Not implemented." ) );
		error_sub( _( "filemodel_real_save_all: no save method" ) );
		return( FALSE );
	}

	path_add_file( filename );

	return( TRUE );
}

static void
filemodel_class_init( FilemodelClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	iObjectClass *iobject_class = IOBJECT_CLASS( class );
	ModelClass *model_class = (ModelClass*) class;

	parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = filemodel_finalize;

	iobject_class->info = filemodel_info;

	model_class->save = filemodel_save;
	model_class->load = filemodel_load;
	
	class->top_load = filemodel_real_top_load;
	class->set_modified = filemodel_real_set_modified;
	class->save_all = filemodel_real_save_all;

	/* NULL isn't an allowed value -- this gets overridden by our
	 * subclasses.
	 */
	class->filetype = NULL;
	class->filetype_pref = NULL;
}

static void
filemodel_init( Filemodel *filemodel )
{
	/* Init our instance fields.
	 */
	filemodel->filename = NULL;
	filemodel->modified = FALSE;
	filemodel->registered = FALSE;
	filemodel->auto_load = FALSE;
	filemodel->x_off = 0;
	filemodel->y_off = 0;

	/* Default version.
	 */
	filemodel->versioned = FALSE;
	filemodel->major = MAJOR_VERSION;
	filemodel->minor = MINOR_VERSION;
	filemodel->micro = MICRO_VERSION;
}

GtkType
filemodel_get_type( void )
{
	static GtkType filemodel_type = 0;

	if( !filemodel_type ) {
		static const GTypeInfo info = {
			sizeof( FilemodelClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) filemodel_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Filemodel ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) filemodel_init,
		};

		filemodel_type = g_type_register_static( TYPE_MODEL, 
			"Filemodel", &info, 0 );
	}

	return( filemodel_type );
}

void
filemodel_set_offset( Filemodel *filemodel, int x_off, int y_off )
{
#ifdef DEBUG
	printf( "filemodel_set_offset: %s \"%s\" %d x %d\n", 
		G_OBJECT_TYPE_NAME( filemodel ), 
		NN( IOBJECT( filemodel )->name ),
		x_off, y_off );
#endif /*DEBUG*/

	filemodel->x_off = x_off;
	filemodel->y_off = y_off;
}

static gboolean
filemodel_load_all_xml( Filemodel *filemodel, 
	Model *parent, ModelLoadState *state )
{
	const char *tname = G_OBJECT_TYPE_NAME( filemodel );

	xmlNode *xnode;
	xmlNode *xstart;

	/* Check the root element for type/version compatibility.
	 */
	if( !(xnode = xmlDocGetRootElement( state->xdoc )) ||
		!xnode->nsDef ||
		!is_prefix( NAMESPACE, (char *) xnode->nsDef->href ) ) {
		error_top( _( "Load failed." ) );
		error_sub( _( "Can't load XML file \"%s\", "
			"it's not a %s save file." ), 
			state->filename, PACKAGE );
		return( FALSE );
	}
	if( sscanf( (char *) xnode->nsDef->href + strlen( NAMESPACE ) + 1, 
		"%d.%d.%d",
		&state->major, &state->minor, &state->micro ) != 3 ) {
		error_top( _( "Load failed." ) );
		error_sub( _( "Can't load XML file \"%s\", "
			"unable to extract version information from "
			"namespace." ), state->filename );
		return( FALSE );
	}

#ifdef DEBUG
	printf( "filemodel_load_all_xml: major = %d, minor = %d, micro = %d\n",
		state->major, state->minor, state->micro );
#endif /*DEBUG*/

	if( !(xstart = get_node( xnode, tname )) ) {
		error_top( _( "Load failed." ) );
		error_sub( _( "Can't load XML file \"%s\", "
			"the file does not contain a %s." ), 
			state->filename, tname );
		return( FALSE );
	}

	/* Set the global loadstate so the lexer can see it.
	 */
	if( filemodel_top_load( filemodel, state, parent, xstart ) ) 
		return( FALSE );

	return( TRUE );
}

static gboolean
filemodel_load_all_xml_file( Filemodel *filemodel, 
	Model *parent, const char *filename )
{
	ModelLoadState *state;

	if( !(state = model_loadstate_new( filename )) )
		return( FALSE );
	if( !filemodel_load_all_xml( filemodel, parent, state ) ) {
		model_loadstate_destroy( state );
		return( FALSE );
	}
	model_loadstate_destroy( state );

	return( TRUE );
}

static gboolean
filemodel_load_all_xml_openfile( Filemodel *filemodel, 
	Model *parent, iOpenFile *of )
{
	ModelLoadState *state;

	if( !(state = model_loadstate_new_openfile( of )) )
		return( FALSE );
	if( !filemodel_load_all_xml( filemodel, parent, state ) ) {
		model_loadstate_destroy( state );
		return( FALSE );
	}
	model_loadstate_destroy( state );

	return( TRUE );
}

static gboolean
filemodel_load_all_text( Filemodel *filemodel, 
	Model *parent, const char *filename )
{
	iOpenFile *of;

	if( !(of = file_open_read( "%s", filename )) ) 
		return( FALSE );

	if( model_load_text( MODEL( filemodel ), parent, of ) ) {
		file_close( of );
		return( FALSE );
	}
	file_close( of );

	return( TRUE );
}

/* Load filename into filemodel ... can mean merge as well as init.
 */
gboolean
filemodel_load_all( Filemodel *filemodel, Model *parent, const char *filename )
{
	ModelClass *model_class = MODEL_GET_CLASS( filemodel );
	const char *tname = G_OBJECT_CLASS_NAME( model_class );

#ifdef DEBUG
	printf( "filemodel_load_all: load file \"%s\" into parent %s \"%s\"\n", 
		filename,
		G_OBJECT_TYPE_NAME( parent ), 
		NN( IOBJECT( parent )->name ) );
#endif /*DEBUG*/

	if( model_class->load_text ) {
		if( !filemodel_load_all_text( filemodel, parent, filename ) ) 
			return( FALSE );
	}
	else if( model_class->load ) {
		if( !filemodel_load_all_xml_file( filemodel, 
			parent, filename ) )
			return( FALSE );
	}
	else {
		error_top( _( "Not implemented." ) );
		error_sub( _( "_%s() not implemented for class \"%s\"." ), 
			"load", tname );
		return( FALSE );
	}

	path_add_file( filename );

	/* Don't recomp here, we may be loading a bunch of interdependent
	 * files.
	 */

	return( TRUE );
}

/* Load iOpenFile into filemodel ... can mean merge as well as init.
 */
gboolean
filemodel_load_all_openfile( Filemodel *filemodel, Model *parent, 
	iOpenFile *of )
{
	ModelClass *model_class = MODEL_GET_CLASS( filemodel );
	const char *tname = G_OBJECT_CLASS_NAME( model_class );

#ifdef DEBUG
	printf( "filemodel_load_all_openfile: load \"%s\" "
		"into parent %s \"%s\"\n", 
		of->fname,
		G_OBJECT_TYPE_NAME( parent ), 
		NN( IOBJECT( parent )->name ) );
#endif /*DEBUG*/

	if( model_class->load_text ) {
		if( model_load_text( MODEL( filemodel ), parent, of ) ) 
			return( FALSE );
	}
	else if( model_class->load ) {
		if( !filemodel_load_all_xml_openfile( filemodel, parent, of ) )
			return( FALSE );
	}
	else {
		error_top( _( "Not implemented." ) );
		error_sub( _( "_%s() not implemented for class \"%s\"." ), 
			"load", tname );
		return( FALSE );
	}

	path_add_file( of->fname );

	/* Don't recomp here, we may be loading a bunch of interdependent
	 * files.
	 */

	return( TRUE );
}

/* Interactive stuff ... save first.
 */

static void
filemodel_inter_save_cb( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Filemodel *filemodel = FILEMODEL( client );
	char *filename;

	if( (filename = filesel_get_filename( filesel )) ) {
		filemodel_set_filename( filemodel, filename );

		if( filemodel_save_all( filemodel, filename ) ) {
			filemodel_set_modified( filemodel, FALSE );
			nfn( sys, IWINDOW_YES );
		}
		else
			nfn( sys, IWINDOW_ERROR );

		g_free( filename );
	}
	else
		nfn( sys, IWINDOW_ERROR );
}

static void
filemodel_inter_saveas_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Filemodel *filemodel = FILEMODEL( client );
	FilemodelClass *class = FILEMODEL_GET_CLASS( filemodel );

	Filesel *filesel = FILESEL( filesel_new() );

	/* Expands to (eg.) "Save Column A2".
	 */
	iwindow_set_title( IWINDOW( filesel ), _( "Save %s %s" ),
		G_OBJECT_TYPE_NAME( filemodel ), 
		NN( IOBJECT( filemodel )->name ) );
	filesel_set_flags( filesel, FALSE, TRUE );
	filesel_set_filetype( filesel, 
		class->filetype, 
		watch_int_get( main_watchgroup, class->filetype_pref, 0 ) );
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( iwnd ) );
	filesel_set_done( filesel, filemodel_inter_save_cb, filemodel );
	idialog_set_notify( IDIALOG( filesel ), nfn, sys );
	iwindow_build( IWINDOW( filesel ) );
	if( filemodel->filename )
		filesel_set_filename( filesel, filemodel->filename );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

void
filemodel_inter_saveas( iWindow *parent, Filemodel *filemodel )
{
	filemodel_inter_saveas_cb( parent, filemodel, 
		iwindow_notify_null, NULL );
}

void
filemodel_inter_save( iWindow *parent, Filemodel *filemodel )
{
	if( filemodel->filename ) {
		if( !filemodel_save_all( filemodel, filemodel->filename ) ) 
			box_alert( GTK_WIDGET( parent ) );
		else 
			filemodel_set_modified( filemodel, FALSE );
	}
	else 
		filemodel_inter_saveas( parent, filemodel );
}

/* Now "empty" ... do an 'are you sure' check if modified has been set.
 */

static void
filemodel_inter_empty_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Filemodel *filemodel = FILEMODEL( client );

	(void) model_empty( MODEL( filemodel ) );
	filemodel_set_modified( filemodel, FALSE );

	nfn( sys, IWINDOW_YES );
}

static void
filemodel_inter_savenempty_ok_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	iWindowSusp *susp = iwindow_susp_new( filemodel_inter_empty_cb, 
		iwnd, client, nfn, sys );

	filemodel_inter_saveas_cb( iwnd, client, iwindow_susp_comp, susp );
}

void
filemodel_inter_savenempty_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Filemodel *filemodel = FILEMODEL( client );
	const char *tname = G_OBJECT_TYPE_NAME( filemodel );

	if( filemodel->modified ) {
		if( filemodel->filename )
			box_savenosave( GTK_WIDGET( iwnd ), 
				filemodel_inter_savenempty_ok_cb, 
				filemodel_inter_empty_cb, filemodel, 
				nfn, sys, 
				_( "Object has been modified." ),
				_( "%s \"%s\" has been modified since you "
				"loaded it from file \"%s\".\n\n"
				"Do you want to save your changes?" ),
				tname, 
				NN( IOBJECT( filemodel )->name ),
				NN( filemodel->filename ) );
		else
			box_savenosave( GTK_WIDGET( iwnd ), 
				filemodel_inter_savenempty_ok_cb, 
				filemodel_inter_empty_cb, filemodel, 
				nfn, sys, 
				_( "Object has been modified." ),
				_( "%s \"%s\" has been modified. "
				"Do you want to save your changes?" ),
				tname, 
				NN( IOBJECT( filemodel )->name ) );
	}
	else
		filemodel_inter_empty_cb( NULL, filemodel, nfn, sys );
}

void
filemodel_inter_savenempty( iWindow *parent, Filemodel *filemodel )
{
	filemodel_inter_savenempty_cb( parent, filemodel, 
		iwindow_notify_null, NULL );
}

/* Now "close" ... easy: just savenempty, then destroy.
 */

static void
filemodel_inter_close_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Filemodel *filemodel = FILEMODEL( client );

	IDESTROY( filemodel );

	nfn( sys, IWINDOW_YES );
}

void
filemodel_inter_savenclose_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	iWindowSusp *susp = iwindow_susp_new( filemodel_inter_close_cb, 
		iwnd, client, nfn, sys );

	filemodel_inter_savenempty_cb( iwnd, client, iwindow_susp_comp, susp );
}

void
filemodel_inter_savenclose( iWindow *parent, Filemodel *filemodel )
{
	filemodel_inter_savenclose_cb( parent, filemodel, 
		iwindow_notify_null, NULL );
}

/* Now "load" ... add stuff to a model from a file.
 */

static void
filemodel_inter_load_cb( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	Filesel *filesel = FILESEL( iwnd );
	Filemodel *filemodel = FILEMODEL( client );
	iContainer *parent = ICONTAINER( filemodel )->parent;
	char *filename;

	if( (filename = filesel_get_filename( filesel )) ) {
		filemodel_set_filename( filemodel, filename );

		if( filemodel_load_all( filemodel, 
			MODEL( parent ), filename ) ) 
			nfn( sys, IWINDOW_YES );
		else
			nfn( sys, IWINDOW_ERROR );

		g_free( filename );
	}
	else
		nfn( sys, IWINDOW_ERROR );
}

static void
filemodel_inter_loadas_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Filemodel *filemodel = FILEMODEL( client );
	FilemodelClass *class = FILEMODEL_GET_CLASS( filemodel );

	Filesel *filesel = FILESEL( filesel_new() );

	iwindow_set_title( IWINDOW( filesel ), "Load %s",
		G_OBJECT_TYPE_NAME( filemodel ) );
	filesel_set_flags( filesel, FALSE, TRUE );
	filesel_set_filetype( filesel, 
		class->filetype, 
		watch_int_get( main_watchgroup, class->filetype_pref, 0 ) );
	iwindow_set_parent( IWINDOW( filesel ), GTK_WIDGET( iwnd ) );
	filesel_set_done( filesel, filemodel_inter_load_cb, filemodel );
	idialog_set_notify( IDIALOG( filesel ), nfn, sys );
	iwindow_build( IWINDOW( filesel ) );
	if( filemodel->filename )
		filesel_set_filename( filesel, filemodel->filename );

	gtk_widget_show( GTK_WIDGET( filesel ) );
}

void
filemodel_inter_loadas( iWindow *parent, Filemodel *filemodel )
{
	filemodel_inter_loadas_cb( parent, filemodel, 
		iwindow_notify_null, NULL );
}

/* Finally "replace" ... empty, then load.
 */

static void
filemodel_inter_replace_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	iWindowSusp *susp = iwindow_susp_new( filemodel_inter_loadas_cb, 
		iwnd, client, nfn, sys );

	filemodel_inter_savenempty_cb( iwnd, client, iwindow_susp_comp, susp );
}

void
filemodel_inter_replace( iWindow *parent, Filemodel *filemodel )
{
	filemodel_inter_replace_cb( parent, filemodel, 
		iwindow_notify_null, NULL );
}

/* Close all registered filemodels.
 */

void
filemodel_inter_close_registered_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	if( filemodel_registered ) {
		Filemodel *filemodel = FILEMODEL( filemodel_registered->data );
		iWindowSusp *susp = iwindow_susp_new( 
			filemodel_inter_close_registered_cb, 
			iwnd, client, nfn, sys );

		filemodel_inter_savenclose_cb( iwnd, filemodel, 
			iwindow_susp_comp, susp );
	}
	else
		nfn( sys, IWINDOW_YES );
}

/* Mark something as having been loaded (or made) during startup. If we loaded
 * from one of the system areas, zap the filename so that we will save to the
 * user's area on changes.
 */
void
filemodel_set_auto_load( Filemodel *filemodel )
{
	filemodel->auto_load = TRUE;

	/* 

		FIXME ... not very futureproof

	 */
	if( filemodel->filename && 
		strstr( filemodel->filename, 
			"share" G_DIR_SEPARATOR_S PACKAGE ) ) {
		char *p = strrchr( filemodel->filename, G_DIR_SEPARATOR );
		char buf[FILENAME_MAX];

		assert( p );

		im_snprintf( buf, FILENAME_MAX, "$SAVEDIR" G_DIR_SEPARATOR_S
			"start" G_DIR_SEPARATOR_S "%s", p + 1 );
		filemodel_set_filename( filemodel, buf );
	}
}
