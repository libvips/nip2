/* main() ... start everything up. See mainw.c for main window stuff.
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

/* Show all paint actions with flashing stuff.
#define DEBUG_UPDATES
 */

/* Stop startup creation of externs for all VIPS functions etc.
#define DEBUG_NOAUTO
 */

/* Stop on any gtk error/warning/whatever. Usually set by configure for dev
 * builds. 
#define DEBUG_FATAL
 */

/* But some themes can trigger warnings, argh, so sometimes we need to
 * undef it. VipsObject sets can trigger warnings. libgoffice will warn about
 * precision issues if run under valgrind.
 */
#undef DEBUG_FATAL

/* Time startup.
#define DEBUG_TIME
 */

/* On quit, make sure we free stuff we can free. Define PROFILE too to get
 * glib's mem use profiler. But it's broken on windows and will cause strange
 * problems, careful.
#define DEBUG_LEAK
#define PROFILE
 */

/* Need im__print_all() for leak testing.
 */
#ifdef DEBUG_LEAK
extern void im__print_all( void );
#endif /*DEBUG_LEAK*/

/* General stuff. 
 */
GdkWindow *main_window_gdk;			/* Top GdkWindow */
GtkWidget *main_window_top = NULL;		/* Secret top window */

Workspacegroup *main_workspacegroup = NULL;	/* All the workspaces */
Toolkitgroup *main_toolkitgroup = NULL;		/* All the toolkits */
Symbol *main_symbol_root = NULL;		/* Root of symtable */
Watchgroup *main_watchgroup = NULL;		/* All of the watches */
Imageinfogroup *main_imageinfogroup = NULL;	/* All of the images */

void *main_c_stack_base = NULL;			/* Base of C stack */

gboolean main_starting = TRUE;			/* In startup */

static const char *main_argv0 = NULL;		/* argv[0] */
static iOpenFile *main_stdin = NULL;		/* stdin as an iOpenFile */
static GtkIconFactory *main_icon_factory = NULL;/* Add stocks to this */
static Splash *main_splash = NULL;

static char *main_option_script = NULL;
static char *main_option_expression = NULL;
gboolean main_option_batch = FALSE;
static gboolean main_option_no_load_menus = FALSE;
static gboolean main_option_no_load_args = FALSE;
static gboolean main_option_stdin_ws = FALSE;
static gboolean main_option_stdin_def = FALSE;
static char *main_option_output = NULL;
static char **main_option_set = NULL;
static gboolean main_option_benchmark = FALSE;
gboolean main_option_time_save = FALSE;
gboolean main_option_i18n = FALSE;
static gboolean main_option_print_main = FALSE;
static gboolean main_option_version = FALSE;
static gboolean main_option_verbose = FALSE;
static gboolean main_option_test = FALSE;
static char *main_option_prefix = NULL;

static GOptionEntry main_option[] = {
	{ "expression", 'e', 0, G_OPTION_ARG_STRING, &main_option_expression, 
		N_( "evaluate and print EXPRESSION" ), 
		"EXPRESSION" },
	{ "script", 's', 0, G_OPTION_ARG_FILENAME, &main_option_script, 
		N_( "load FILE as a set of definitions" ), 
		"FILE" },
	{ "output", 'o', 0, G_OPTION_ARG_FILENAME, &main_option_output, 
		N_( "write value of 'main' to FILE" ), "FILE" },
	{ "batch", 'b', 0, G_OPTION_ARG_NONE, &main_option_batch, 
		N_( "run in batch mode" ), NULL },
	{ "set", '=', 0, G_OPTION_ARG_STRING_ARRAY, &main_option_set, 
		N_( "set values" ), NULL },
	{ "verbose", 'V', 0, G_OPTION_ARG_NONE, &main_option_verbose, 
		N_( "verbose error output" ), NULL },
	{ "no-load-menus", 'm', 0, G_OPTION_ARG_NONE, 
		&main_option_no_load_menus, 
		N_( "don't load menu definitions" ), NULL },
	{ "no-load-args", 'a', 0, G_OPTION_ARG_NONE, &main_option_no_load_args, 
		N_( "don't try to load command-line arguments" ), NULL },
	{ "stdin-ws", 'w', 0, G_OPTION_ARG_NONE, &main_option_stdin_ws, 
		N_( "load stdin as a workspace" ), NULL },
	{ "stdin-def", 'd', 0, G_OPTION_ARG_NONE, &main_option_stdin_def, 
		N_( "load stdin as a set of definitions" ), NULL },
	{ "print-main", 'p', 0, G_OPTION_ARG_NONE, &main_option_print_main, 
		N_( "print value of 'main' to stdout" ), 
		NULL },
	{ "benchmark", 'c', 0, G_OPTION_ARG_NONE, &main_option_benchmark, 
		N_( "start up and shut down" ), 
		NULL },
	{ "time-save", 't', 0, G_OPTION_ARG_NONE, &main_option_time_save, 
		N_( "time image save operations" ), 
		NULL },
	{ "prefix", 'x', 0, G_OPTION_ARG_FILENAME, &main_option_prefix, 
		N_( "start as if installed to PREFIX" ), "PREFIX" },
	{ "i18n", 'i', 0, G_OPTION_ARG_NONE, &main_option_i18n, 
		N_( "output strings for internationalisation" ), 
		NULL },
	{ "version", 'v', 0, G_OPTION_ARG_NONE, &main_option_version, 
		N_( "print version number" ), 
		NULL },
	{ "test", 'T', 0, G_OPTION_ARG_NONE, &main_option_test, 
		N_( "test for errors and quit" ), NULL },
	{ NULL }
};

/* Accumulate startup errors here.
 */
static char main_start_error_txt[MAX_STRSIZE];
static VipsBuf main_start_error = VIPS_BUF_STATIC( main_start_error_txt );

static void
main_log_add( const char *fmt, ... )
{
	va_list ap;

        va_start( ap, fmt );
	vips_buf_vappendf( &main_start_error, fmt, ap );
        va_end( ap );
}

static const char *
main_log_get( void )
{
	return( vips_buf_all( &main_start_error ) );
}

static gboolean
main_log_is_empty( void )
{
	return( vips_buf_is_empty( &main_start_error ) );
}

/* NULL log handler. Used to suppress output on win32 without DEBUG_FATAL.
 */
#ifndef DEBUG_FATAL
#ifdef OS_WIN32 
static void
main_log_null( const char *log_domain, GLogLevelFlags log_level,
	const char *message, void *user_data )
{
}
#endif /*OS_WIN32*/ 
#endif /*!DEBUG_FATAL*/

/* Print all errors and quit. Batch mode only.
 */
static void
main_error_exit( const char *fmt, ... )
{
	va_list args;

        va_start( args, fmt );
        (void) vfprintf( stderr, fmt, args );
        va_end( args );

	fprintf( stderr, "\n%s %s\n", 
		error_get_top(), error_get_sub() );

	if( main_option_verbose ) {
		char txt[MAX_STRSIZE];
		VipsBuf buf = VIPS_BUF_STATIC( txt );

		slist_map( expr_error_all,
			(SListMapFn) expr_error_print, &buf );
		fprintf( stderr, "%s\n", vips_buf_all( &buf ) );
	}

	exit( 1 );
}

/* Output a single main.
 */
static void
main_print_main( Symbol *sym )
{
	PElement *root;

	root = &sym->expr->root;
	if( symbol_recalculate_check( sym ) || 
		!reduce_pelement( reduce_context, reduce_spine_strict, root ) ) 
		main_error_exit( _( "error calculating \"%s\"" ), 
			symbol_name( sym ) );

	if( main_option_output ) {
		char filename[FILENAME_MAX];

		im_strncpy( filename, main_option_output, FILENAME_MAX );
		if( !group_save_item( root, filename ) )
			main_error_exit( _( "error saving \"%s\"" ), 
				symbol_name( sym ) );
	}

	if( main_option_print_main )
		graph_value( root );
}

static void *
main_print_ws( Workspace *ws, gboolean *found )
{
	Symbol *sym;

	if( (sym = compile_lookup( 
		ws->sym->expr->compile, "main" )) ) {
		main_print_main( sym );
		*found = TRUE;
	}

	return( NULL );
}

/* Clean up our application and quit. Not interactive! Do any "has been
 * modified, OK to quit?" stuff before this.
 */
static void
main_quit( void )
{
#if HAVE_FFTW || HAVE_FFTW3
	iOpenFile *of;
#endif /*HAVE_FFTW || HAVE_FFTW3*/

#ifdef DEBUG
	printf( "main_quit: cleaning up ...\n" );
#endif/*DEBUG*/

	if( main_option_print_main || main_option_output ) {
		Symbol *sym;
		gboolean found;

		symbol_recalculate_all();

		/* Process all the mains we can find: one at the top level,
		 * one in each workspace.
		 */
		found = FALSE;
		if( (sym = compile_lookup( 
			symbol_root->expr->compile, "main" )) ) {
			main_print_main( sym );
			found = TRUE;
		}
		workspace_map( (workspace_map_fn) main_print_ws, &found, NULL );

		if( !found )
			main_error_exit( "%s", _( "no \"main\" found" ) );
	}

	/* Force all windows down. 

		FIXME 

		this can cause a recursive call to us from mainw_destroy()

	 */
	iwindow_map_all( (iWindowMapFn) iwindow_kill, NULL );
	mainw_shutdown();

	/* Dump wisdom back again.
	 */
#if HAVE_FFTW || HAVE_FFTW3
	if( (of = file_open_write( "%s" G_DIR_SEPARATOR_S "wisdom", 
		get_savedir() )) ) {
		fftw_export_wisdom_to_file( of->fp );
		file_close( of );
	}
#endif /*HAVE_FFTW*/

	/* Remove any ws retain files.
	 */
	workspace_retain_clean();

	/* Junk all symbols. This may remove a bunch of intermediate images
	 * too.
	 */
	UNREF( main_watchgroup );
	UNREF( main_symbol_root );
	UNREF( main_toolkitgroup );
	UNREF( main_workspacegroup );

	/* Junk reduction machine ... this should remove all image temps.
	 */
	reduce_destroy( reduce_context );

#ifdef DEBUG_LEAK
	fprintf( stderr, "DEBUG_LEAK: testing for leaks ...\n" );

	/* Free other GTK stuff.
	 */
	if( main_icon_factory )
		gtk_icon_factory_remove_default( main_icon_factory );

#ifdef HAVE_LIBGOFFICE
	/* Not quite sure what this does, but don't do it in batch mode.
 	 */
	if( !main_option_batch )
		libgoffice_shutdown ();
#endif /*HAVE_LIBGOFFICE*/

	/* Should have freed everything now.
	 */

	/* Make sure!

		FIXME ... #ifdef this lot out at some point

	 */
	UNREF( main_imageinfogroup );
	heap_check_all_destroyed();
	im__print_all();
	managed_check_all_destroyed();
	util_check_all_destroyed();
	vips_check_all_destroyed();

#ifdef PROFILE
	g_mem_profile();
#endif /*PROFILE*/
#endif /*DEBUG_LEAK*/

#ifdef DEBUG
	printf( "main_quit: exit( 0 )\n" );
#endif/*DEBUG*/

	/* And exit.
	 */
	exit( 0 );
}

static void
main_quit_test_cb( void *sys, iWindowResult result )
{
	if( result == IWINDOW_YES )
		/* No return from this.
		 */
		main_quit();
}

/* Check before quitting.
 */
void
main_quit_test( void )
{
	/* Flush any pending preference saves before we look for dirty
	 * objects.
	 */
	watchgroup_flush( main_watchgroup );

	/* Close registered models.
	 */
	filemodel_inter_close_registered_cb( IWINDOW( main_window_top ), NULL,
		main_quit_test_cb, NULL );
}

gboolean
main_splash_enabled( void )
{
	return( !existsf( "%s" G_DIR_SEPARATOR_S "%s", 
		get_savedir(), NO_SPLASH ) );
}

static void
main_watchgroup_changed_cb( void )
{
	/* Only set this in GUI mode. Otherwise, let the user control CPUs 
	 * with the env variable and --vips-concurrency args.
	 */
	if( !main_option_batch )
		im_concurrency_set( VIPS_CPUS );
}

/* Try to load a thing, anything at all. Actually, we don't load plugins
 * experimentally, win32 pops up an annoying error dialog if you try that.
 */
gboolean
main_load( Workspace *ws, const char *filename )
{
	Workspace *new_ws;

	if( (new_ws = 
		workspace_new_from_file( main_workspacegroup, filename )) ) {
		Mainw *new_mainw;

		if( !main_option_batch ) {
			new_mainw = mainw_new( new_ws );
			gtk_widget_show( GTK_WIDGET( new_mainw ) );
		}
		mainw_recent_add( &mainw_recent_workspace, filename );

		return( TRUE );
	}
	error_clear();

	/* workspace_load_file() needs to recalc to work, try to avoid that by
	 * doing .defs first.
	 */
	if( is_file_type( &filesel_dfile_type, filename ) ) {
		if( toolkit_new_from_file( main_toolkitgroup, filename ) )
			return( TRUE );
	}

	/* Try as matrix or image. Have to do these via definitions.
	 */
	if( workspace_load_file( ws, filename ) )
		return( TRUE );

	error_clear();

	error_top( _( "Unknown file type." ) );
	error_sub( _( "Unable to load \"%s\"." ), filename );

	return( FALSE );
}

#ifndef DEBUG_NOAUTO
static void *
main_load_plug( char *name )
{
	if( !calli_string_filename( (calli_string_fn) im_load_plugin,
		name, NULL, NULL, NULL ) ) {
		error_top( _( "Unable to load." ) );
		error_sub( _( "Error loading plug-in \"%s\"." ), name );
		error_vips();
		box_alert( NULL );
	}

	return( NULL );
}
#endif /*!DEBUG_NOAUTO*/

static void *
main_load_def( const char *filename )
{	
	Toolkit *kit;

	if( !main_option_no_load_menus || im_skip_dir( filename )[0] == '_' ) {
		progress_update_loading( 0, im_skip_dir( filename ) );

		if( !(kit = toolkit_new_from_file( main_toolkitgroup, 
			filename )) )
			box_alert( NULL );
		else 
			filemodel_set_auto_load( FILEMODEL( kit ) );
	}

	return( NULL );
}

static void *
main_load_ws( const char *filename )
{
	Workspace *ws;

#ifdef DEBUG
	printf( "main_load_ws: %s\n", filename );
#endif/*DEBUG*/

	progress_update_loading( 0, im_skip_dir( filename ) );

	if( !(ws = workspace_new_from_file( main_workspacegroup, filename )) ) 
		box_alert( NULL );
	else {
		filemodel_set_auto_load( FILEMODEL( ws ) );
	}

	return( NULL );
}

#ifndef DEBUG_NOAUTO
/* Link all the packages in a function.
 */
static void *
main_link_package( im_package *pack)
{
	char name[MAX_STRSIZE];
	Toolkit *kit;
        int i;

	im_snprintf( name, MAX_STRSIZE, "_%s", pack->name );
	kit = toolkit_new( main_toolkitgroup, name );

        for( i = 0; i < pack->nfuncs; i++ ) 
		if( vips_is_callable( pack->table[i] ) ) {
			Symbol *sym;

			sym = symbol_new( symbol_root->expr->compile,
				pack->table[i]->name );
			g_assert( sym->type == SYM_ZOMBIE );
			sym->type = SYM_EXTERNAL;
			sym->function = pack->table[i];
			sym->fn_nargs = vips_n_args( pack->table[i] );
			(void) tool_new_sym( kit, -1, sym );
			symbol_made( sym );
		}

	filemodel_set_auto_load( FILEMODEL( kit ) );
	filemodel_set_modified( FILEMODEL( kit ), FALSE );
	kit->pseudo = TRUE;

        return( NULL );
}
#endif /*!DEBUG_NOAUTO*/

/* Load all plugins and defs.
 */
static void
main_load_startup( void )
{
	mainw_recent_freeze();

/* Stop load of builtins, plugs and vips ... handy for debugging if you're
 * tracing symbol.c
 */
#ifdef DEBUG_NOAUTO
	printf( "*** DEBUG_NOAUTO set, not loading builtin, plugs and vips\n" );
#else /*!DEBUG_NOAUTO*/

#ifdef DEBUG
	printf( "built-ins init\n" );
#endif/*DEBUG*/

	/* Add builtin toolkit.
	 */
	builtin_init();

#ifdef DEBUG
	printf( "plug-ins init\n" );
#endif/*DEBUG*/

	/* Load any plug-ins on PATH_START. 
	 */
	(void) path_map_exact( PATH_START, "*.plg", 
		(path_map_fn) main_load_plug, NULL );

	/* Link all VIPS functions as SYM_EXTERNAL.
	 */
        (void) im_map_packages( (VSListMap2Fn) main_link_package, NULL );
#endif /*!DEBUG_NOAUTO*/

	/* Load up all defs and wses.
	 */
#ifdef DEBUG
	printf( "definitions init\n" );
#endif/*DEBUG*/
	(void) path_map_exact( PATH_START, "*.def", 
		(path_map_fn) main_load_def, NULL );

#ifdef DEBUG
	printf( "ws init\n" );
#endif/*DEBUG*/
	(void) path_map_exact( PATH_START, "*.ws", 
		(path_map_fn) main_load_ws, NULL );

	mainw_recent_thaw();
}

static void *
main_junk_auto_load( Filemodel *filemodel )
{
	g_assert( IS_FILEMODEL( filemodel ) );

	if( filemodel->auto_load )
		IDESTROY( filemodel );

	return( NULL );
}

/* Remove and reload all menus/plugins/workspaces.
 */
void
main_reload( void )
{
	progress_begin();

	/* Remove.
	 */
	toolkitgroup_map( main_toolkitgroup, 
		(toolkit_map_fn) main_junk_auto_load, NULL, NULL );
	workspace_map( (workspace_map_fn) main_junk_auto_load, NULL, NULL );
	im_close_plugins();

	/* Reload.
	 */
	main_load_startup();

	/* We may have changed our prefs ... link the watches to the
	 * new prefs workspace.
	 */
	watch_relink_all();

	progress_end();
}

/* Use a file to paint a named stock item.
 */
static void
main_file_for_stock( GtkIconFactory *icon_factory,
	const char *stock, const char *file )
{
	GtkIconSource *icon_source;
	GtkIconSet *icon_set;
	char buf[FILENAME_MAX];
	char buf2[FILENAME_MAX];

	im_snprintf( buf2, FILENAME_MAX, 
		"$VIPSHOME/share/$PACKAGE/data/%s", file );
	expand_variables( buf2, buf );
        nativeize_path( buf );
	icon_source = gtk_icon_source_new(); 
	gtk_icon_source_set_filename( icon_source, buf );
	icon_set = gtk_icon_set_new();
	gtk_icon_set_add_source( icon_set, icon_source );
	gtk_icon_source_free( icon_source );
	gtk_icon_factory_add( icon_factory, stock, icon_set );
	gtk_icon_set_unref( icon_set );
}

/* Make our custom icon sets.
 */
static void
main_register_icons( void )
{
	static const GtkStockItem stock_item[] = {
/* Can be (eg.) 
 *
 *   { GTK_STOCK_COPY, N_("_Copy"), GDK_CONTROL_MASK, 'c', GETTEXT_PACKAGE },
 *
 */
		{ STOCK_NEXT_ERROR, 
			N_( "Next _Error" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_DROPPER, N_( "Ink dropper" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_DUPLICATE, N_( "D_uplicate" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_PAINTBRUSH, N_( "Pen" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_LINE, N_( "Line" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_TEXT, N_( "Text" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_SMUDGE, N_( "Smudge" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_FLOOD, N_( "Flood" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_FLOOD_BLOB, N_( "Flood Blob" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_RECT, N_( "Fill Rectangle" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_MOVE, N_( "Pan" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_SELECT, N_( "Select" ), 0, 0, GETTEXT_PACKAGE },

		/* And the LEDs we use.
		 */
		{ STOCK_LED_RED, N_( "Red LED" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_LED_GREEN, N_( "Green LED" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_LED_BLUE, N_( "Blue LED" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_LED_YELLOW, N_( "Yellow LED" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_LED_CYAN, N_( "Cyan LED" ), 0, 0, GETTEXT_PACKAGE },
		{ STOCK_LED_OFF, N_( "Off LED" ), 0, 0, GETTEXT_PACKAGE }
	};

	GtkIconSet *icon_set;

	gtk_stock_add_static( stock_item, IM_NUMBER( stock_item ) );
	main_icon_factory = gtk_icon_factory_new();

	/* Make a colour picker stock ... take the stock icon and add our own
	 * text (gtk defines no text for the standard version of this stock
	 * icon).
	 */
	icon_set = gtk_icon_factory_lookup_default( GTK_STOCK_COLOR_PICKER );
	gtk_icon_factory_add( main_icon_factory, STOCK_DROPPER, icon_set );

	/* For Next Error, use JUMP_TO.
	 */
	icon_set = gtk_icon_factory_lookup_default( GTK_STOCK_JUMP_TO );
	gtk_icon_factory_add( main_icon_factory, STOCK_NEXT_ERROR, icon_set );

	/* For clone, use the DND_MULTIPLE icon (close enough).
	 */
	icon_set = gtk_icon_factory_lookup_default( GTK_STOCK_DND_MULTIPLE );
	gtk_icon_factory_add( main_icon_factory, STOCK_DUPLICATE, icon_set );

	/* Link to our stock .pngs.
	 */
	main_file_for_stock( main_icon_factory, 
		STOCK_PAINTBRUSH, "stock-tool-ink-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_LINE, "stock-tool-path-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_TEXT, "stock-tool-text-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_SMUDGE, "stock-tool-smudge-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_FLOOD, "stock-tool-bucket-fill-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_FLOOD_BLOB, "stock-tool-bucket-fill-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_RECT, "stock-tool-rect-select-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_MOVE, "stock-tool-move-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_SELECT, "stock-tool-select-22.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_LED_RED, "stock-led-red-18.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_LED_GREEN, "stock-led-green-18.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_LED_BLUE, "stock-led-blue-18.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_LED_YELLOW, "stock-led-yellow-18.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_LED_CYAN, "stock-led-cyan-18.png" );
	main_file_for_stock( main_icon_factory, 
		STOCK_LED_OFF, "stock-led-off-18.png" );

	gtk_icon_factory_add_default( main_icon_factory );
	g_object_unref( main_icon_factory );
}

/* Init the display connection stuff.
 */
static void
main_x_init( int *argc, char ***argv )
{
	char buf[FILENAME_MAX];

	if( main_window_gdk )
		return;

#ifdef DEBUG
	printf( "X11 init\n" );
#endif/*DEBUG*/

	(void) calli_string_filename( 
		(calli_string_fn) gtk_rc_add_default_file, 
			"$VIPSHOME" G_DIR_SEPARATOR_S "share" G_DIR_SEPARATOR_S 
			PACKAGE G_DIR_SEPARATOR_S "rc" G_DIR_SEPARATOR_S 
			"ipgtkrc", NULL, NULL, NULL );
	gtk_init( argc, argv );

#ifdef HAVE_LIBGOFFICE
        libgoffice_init();
	go_plugins_init( NULL, NULL, NULL, NULL, TRUE, 
		GO_TYPE_PLUGIN_LOADER_MODULE );
#endif /*HAVE_LIBGOFFICE*/

	/* Set the default icon. 
	 */
	expand_variables( "$VIPSHOME/share/$PACKAGE/data/vips-128.png", buf );
        nativeize_path( buf );
	gtk_window_set_default_icon_from_file( buf, NULL );

	/* Turn off startup notification. Startup is done when we pop our
	 * first window, not when we make this secret window or display the
	 * splash screen.
	 */
	gtk_window_set_auto_startup_notification( FALSE );

        /* Make invisible top level window ... used to get stuff for
         * build. Realize it, but don't display it.
         */
        main_window_top = iwindow_new( GTK_WINDOW_TOPLEVEL );
        gtk_widget_realize( main_window_top );
	main_window_gdk = main_window_top->window;

#ifdef DEBUG_UPDATES
	printf( "*** debug updates is on\n" );
	gdk_window_set_debug_updates( TRUE );
#endif /*DEBUG_UPDATES*/

	main_register_icons();

	if( main_splash_enabled() ) {
		main_splash = splash_new();
		gtk_widget_show( GTK_WIDGET( main_splash ) );

		/* Wait for the splashscreen to pop up. We want this to appear
		 * as quickly as possible.
		 */
		while( g_main_context_iteration( NULL, FALSE ) )
			;
	}

	/* Next window we make is end of startup.
	 */
	gtk_window_set_auto_startup_notification( TRUE );

	/* Load up any saved accelerators.
	 */
	calli_string_filenamef( (calli_string_fn) gtk_accel_map_load,
		"%s" G_DIR_SEPARATOR_S "accel_map", get_savedir() );
}

static void *
main_toobig_done_sub( const char *filename )
{
	unlinkf( "%s", filename );

	return( NULL );
}

/* OK in "flush temps" yesno.
 */
static void
main_toobig_done( iWindow *iwnd, 
	void *client, iWindowNotifyFn nfn, void *sys )
{
	/* Don't "rm *", too dangerous. There can (I think?) only be ws and
	 * VIPS files.
	 */
	path_map_dir( PATH_TMP, "*.v",
		(path_map_fn) main_toobig_done_sub, NULL );
	path_map_dir( PATH_TMP, "*.ws",
		(path_map_fn) main_toobig_done_sub, NULL );

	/* Tell space-free indicators to update.
	 */
	if( main_imageinfogroup )
		iobject_changed( IOBJECT( main_imageinfogroup ) );

	nfn( sys, IWINDOW_YES );
}

/* Test for a bunch of stuff in the TMP area. Need to do this before
 * we load args in case there are large JPEGs there. Only bother in
 * interactive mode: we won't be able to question the user without an
 * X connection.
 */
static void
main_check_temp( double total )
{
	if( total > 10 * 1024 * 1024 ) {
		char txt[256];
		VipsBuf buf = VIPS_BUF_STATIC( txt );
		char tmp[FILENAME_MAX];

		expand_variables( PATH_TMP, tmp );
		nativeize_path( tmp );
		absoluteize_path( tmp );
		to_size( &buf, total );

		box_yesno( NULL,
			main_toobig_done, iwindow_true_cb, NULL,
			NULL, NULL,
			_( "Empty temp area" ),
			_( "Many files in temp area." ),
			_( "The temp area \"%s\" contains %s of files. "
			"Would you like to empty the temp area? "
			"This will delete any workspace backups and "
			"cannot be undone." ),
			tmp, vips_buf_all( &buf ) );
	}
}

/* Make sure a savedir exists. Used to build the "~/.nip2-xx/tmp" etc.
 * directory tree.
 */
static void
main_mkdir( const char *dir )
{
	if( !existsf( "%s" G_DIR_SEPARATOR_S "%s", get_savedir(), dir ) )
		if( !mkdirf( "%s" G_DIR_SEPARATOR_S "%s", get_savedir(), dir ) )
			error_exit( _( "unable to make %s %s: %s" ),
				get_savedir(), dir, g_strerror( errno ) );
}

static gboolean
main_set( const char *str )
{
	Symbol *sym;

#ifdef DEBUG
	printf( "main_set: %s\n", str );
#endif /*DEBUG*/

	attach_input_string( str );
	if( !(sym = parse_set_symbol()) ) 
		return( FALSE );

	/* Put the input just after the '=', ready to parse a RHS into the
	 * symbol.
	 */
	attach_input_string( str + 
		IM_CLIP( 0, input_state.charpos - 1, strlen( str ) ) );

	if( !symbol_user_init( sym ) || 
		!parse_rhs( sym->expr, PARSE_RHS ) ) {
		/* Another parse error.
		 */
		expr_error_get( sym->expr );

		/* Block changes to error_string ... symbol_destroy() 
		 * can set this for compound objects.
		 */
		error_block();
		IDESTROY( sym );
		error_unblock();

		return( FALSE );
	}

	symbol_made( sym );

	/* Is there a row? Make sure any modified text there can't zap our new
	 * text.
	 */
	if( sym->expr->row ) {
		Row *row = sym->expr->row;

		heapmodel_set_modified( 
			HEAPMODEL( row->child_rhs->itext ), FALSE );
	}

	return( TRUE );
}

/* Start here!
 */
int
main( int argc, char *argv[] )
{
	gboolean welcome_message = FALSE;
	Workspace *ws;
	GError *error = NULL;
	GOptionContext *context;
	const char *prefix;
	int i;
	double total = 0.0;
#ifdef HAVE_GETRLIMIT
	struct rlimit rlp;
#endif /*HAVE_GETRLIMIT*/
	char name[256];
#if HAVE_FFTW || HAVE_FFTW3
	iOpenFile *of;
#endif /*HAVE_FFTW*/
	Toolkit *kit;
	char txt[MAX_STRSIZE];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

#ifdef DEBUG_LEAK
#ifdef PROFILE
	g_mem_set_vtable( glib_mem_profiler_table );
#endif /*PROFILE*/
#endif /*DEBUG_LEAK*/

#ifdef DEBUG_TIME
	GTimer *startup_timer = g_timer_new();
	printf( "DEBUG_TIME: startup timer zeroed ...\n" );
#endif /*DEBUG_TIME*/

	/* In startup phase.
	 */
	main_starting = TRUE;

	/* Want numeric locale to be "C", so we have C rules for doing 
	 * double <-> string (ie. no "," for decimal point).
	 */
	setlocale( LC_ALL, "" );
	setlocale( LC_NUMERIC, "C" );

	/* Make sure our LC_NUMERIC setting is not trashed.
 	 */
	gtk_disable_setlocale();

	/* Set localised application name.
	 */
	g_set_application_name( _( PACKAGE ) );

#ifdef DEBUG
	printf( "main: sizeof( HeapNode ) == %zd\n", sizeof( HeapNode ) );

	/* Should be 3 pointers, hopefully.
	 */
	if( sizeof( HeapNode ) != 3 * sizeof( void * ) )
		printf( "*** struct packing problem!\n" );
#endif/*DEBUG*/

	/* Yuk .. shouldn't really write to argv0. This can't change the
	 * string length.
	 *
	 * On win32 we will sometimes get paths with mixed '/' and '\' which 
	 * confuses vips's prefix guessing. Make sure we have one or the other.
	 */
	nativeize_path( argv[0] );

	main_argv0 = argv[0];
	main_c_stack_base = &argc;

	/* Pass config.h stuff down to .ws files.
	 */
	setenvf( "PACKAGE", "%s", PACKAGE );
	setenvf( "VERSION", "%s", VERSION );

#ifdef OS_WIN32
{
        /* No HOME on windows ... make one from HOMEDRIVE and HOMEDIR (via
         * glib).
         */
	const char *home;
	char buf[FILENAME_MAX];

	if( !(home = g_getenv( "HOME" )) ) 
		home = g_get_home_dir();

	/* We need native paths.
	 */
	strncpy( buf, home, FILENAME_MAX );
	nativeize_path( buf );
	setenvf( "HOME", "%s", buf );
}
#endif /*OS_WIN32*/

	/* Name of the dir we store our config stuff in. This can get used by
	 * Preferences.ws.
	 */
	setenvf( "SAVEDIR", "%s", get_savedir() );

	/* Path separator on this platform.
	 */
	setenvf( "SEP", "%s", G_DIR_SEPARATOR_S );

	/* Start up vips.
	 */
	if( im_init_world( main_argv0 ) )
		error_exit( "unable to start VIPS" );

	/* Init i18n ... get catalogues from $VIPSHOME/share/locale so we're
	 * relocatable.
	 */
	prefix = im_guess_prefix( main_argv0, "VIPSHOME" );
	im_snprintf( name, 256, 
		"%s" G_DIR_SEPARATOR_S "share" G_DIR_SEPARATOR_S "locale", 
		prefix );
#ifdef DEBUG
	printf( "bindtextdomain: %s\n", name );
#endif /*DEBUG*/
	textdomain( GETTEXT_PACKAGE );
	bindtextdomain( GETTEXT_PACKAGE, name );
	bind_textdomain_codeset( GETTEXT_PACKAGE, "UTF-8" );

	context = g_option_context_new( _( "- image processing spreadsheet" ) );
	g_option_context_add_main_entries( context, 
		main_option, GETTEXT_PACKAGE );

	/* Don't start X here! We may be in batch mode.
	 */
	g_option_context_add_group( context, gtk_get_option_group( FALSE ) );
	g_option_context_add_group( context, im_get_option_group() );

	if( !g_option_context_parse( context, &argc, &argv, &error ) ) 
		vfatal( &error );

	g_option_context_free( context );

	/* Override the install guess from vips. This won't pick up msg
	 * cats sadly :( since we have to init i18n before arg parsing. Handy
	 * for testing without installing.
	 */
	if( main_option_prefix ) {
		char tmp[FILENAME_MAX];

		im_strncpy( tmp, main_option_prefix, FILENAME_MAX );
		nativeize_path( tmp );
		absoluteize_path( tmp );
		prefix = im_strdupn( tmp );
		setenvf( "VIPSHOME", "%s", prefix );
	}

	if( main_option_version ) {
		printf( "%s-%s", PACKAGE, VERSION );
		printf( "\n" );

		printf( _( "linked to vips-%s" ), im_version_string() );
		printf( "\n" );

		exit( 0 );
	}

#ifdef DEBUG_LEAK
	fprintf( stderr, 
		"*** DEBUG_LEAK is on ... will leaktest on exit\n" );
#endif /*DEBUG_LEAK*/

#ifdef DEBUG_FATAL
	/* Set masks for debugging ... stop on any problem. 
	 */
	g_log_set_always_fatal( 
		G_LOG_FLAG_RECURSION |
		G_LOG_FLAG_FATAL |
		G_LOG_LEVEL_ERROR |
		G_LOG_LEVEL_CRITICAL |
		G_LOG_LEVEL_WARNING );

	fprintf( stderr, 
		"*** DEBUG_FATAL is on ... will abort() on first warning\n" );
#else /*!DEBUG_FATAL*/
#ifdef OS_WIN32 
	/* No logging output ... on win32, log output pops up a very annoying
 	 * console text box.
	 */
	g_log_set_handler( "GLib", 
		G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, 
		main_log_null, NULL );
	g_log_set_handler( "Gtk", 
		G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, 
		main_log_null, NULL );
	g_log_set_handler( NULL,
		G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, 
		main_log_null, NULL );
#endif /*OS_WIN32*/ 
#endif /*DEBUG_FATAL*/

	main_stdin = file_open_read_stdin();

#ifdef HAVE_GETRLIMIT
	/* Make sure we have lots of file descriptors. Some platforms have cur
	 * as 256 and max at 1024 to keep stdio happy.
	 */
	if( getrlimit( RLIMIT_NOFILE, &rlp ) == 0 ) {
		rlim_t old_limit = rlp.rlim_cur;

		rlp.rlim_cur = rlp.rlim_max;
		if( setrlimit( RLIMIT_NOFILE, &rlp ) == 0 ) {
#ifdef DEBUG
			printf( "set max file descriptors to %d\n", 
				(int) rlp.rlim_max );
#endif /*DEBUG*/
		}
		else if( (int) rlp.rlim_max != -1 ) {
			/* -1 means can't-be-set, at least on os x, so don't
			 * warn.
			 */
			g_warning( _( "unable to change max file descriptors\n"
				"max file descriptors still set to %d" ),
				(int) old_limit );
		}
	}
	else {
		g_warning( _( "unable to read max file descriptors" ) );
	}
#endif /*HAVE_GETRLIMIT*/

	/* Make our file types.
	 */
	filesel_startup();

	/* Set default values for paths.
	 */
	path_init();

	/* First time we've been run? Welcome message.
	 */
	if( !existsf( "%s", get_savedir() ) ) 
		welcome_message = TRUE;

	/* Always make these in case some got deleted.
	 */
	main_mkdir( "" );
	main_mkdir( "tmp" );
	main_mkdir( "start" );
	main_mkdir( "data" );

	/* Init other stuff.
	 */
#ifdef HAVE_FFTW3
	fftw_import_system_wisdom();
#endif /*HAVE_FFTW3*/
#if HAVE_FFTW || HAVE_FFTW3
	if( (of = file_open_read( "%s" G_DIR_SEPARATOR_S "wisdom", 
		get_savedir() )) ) {
		fftw_import_wisdom_from_file( of->fp );
		file_close( of );
	}
#endif /*HAVE_FFTW*/

	mainw_startup();
	reduce_context = reduce_new();
	main_symbol_root = symbol_root_init();
	g_object_ref( G_OBJECT( main_symbol_root ) );
	iobject_sink( IOBJECT( main_symbol_root ) );
	model_base_init();
	main_workspacegroup = workspacegroup_new( "Workspaces" );
	g_object_ref( G_OBJECT( main_workspacegroup ) );
	iobject_sink( IOBJECT( main_workspacegroup ) );
	main_watchgroup = watchgroup_new( main_workspacegroup, "Preferences" );
	g_object_ref( G_OBJECT( main_watchgroup ) );
	iobject_sink( IOBJECT( main_watchgroup ) );
	main_toolkitgroup = toolkitgroup_new( symbol_root );
	g_object_ref( G_OBJECT( main_toolkitgroup ) );
	iobject_sink( IOBJECT( main_toolkitgroup ) );
	main_imageinfogroup = imageinfogroup_new();
	g_object_ref( G_OBJECT( main_imageinfogroup ) );
	iobject_sink( IOBJECT( main_imageinfogroup ) );

	/* First pass at command-line options. Just look at the flags that
	 * imply other flags, don't do any processing yet.
	 */
	if( main_option_script ) {
		main_option_batch = TRUE;
		main_option_no_load_menus = TRUE;
		main_option_no_load_args = TRUE;
		main_option_print_main = TRUE;
	}

	if( main_option_test ) {
		main_option_batch = TRUE;
		main_option_verbose = TRUE;
	}

	if( main_option_expression ) {
		main_option_batch = TRUE;
		main_option_no_load_menus = TRUE;
		main_option_no_load_args = TRUE;
		main_option_print_main = TRUE;
	}

	if( main_option_benchmark ) {
		main_option_batch = TRUE;
		main_option_no_load_menus = FALSE;
	}

	if( main_option_i18n ) {
		/* Just start up and shutdown, no X. Output constant
		 * i18n strings.
		 */
		main_option_batch = TRUE;
		main_option_no_load_menus = FALSE;
	}

#ifdef DEBUG
	if( main_option_batch ) 
		printf( "non-interactive mode\n" );
#endif /*DEBUG*/

	/* Start the X connection. We need this before _load_all(), so that
	 * we can pop up error dialogs.
	 */
	if( !main_option_batch )
		main_x_init( &argc, &argv );

	/* Load start-up stuff. Builtins, plugins, externals etc. We need to
	 * do this before we load any user code so we can prevent redefinition
	 * of builtins.
	 */
	main_load_startup();

#ifdef DEBUG
	printf( "arg processing\n" );
#endif/*DEBUG*/

	/* Might make this from stdin/whatever if we have a special
	 * command-line flag.
	 */
	ws = NULL;

	/* Second command-line pass. This time we do any actions.
	 */
	if( main_option_script ) {
		if( !toolkit_new_from_file( main_toolkitgroup,
			main_option_script ) )
			main_log_add( "%s\n", error_get_sub() );
	}

	if( main_option_expression ) {
		kit = toolkit_new( main_toolkitgroup, "_expression" );

		vips_buf_appendf( &buf, "main = %s;", main_option_expression );
		attach_input_string( vips_buf_all( &buf ) );
		(void) parse_onedef( kit, -1 );

		filemodel_set_modified( FILEMODEL( kit ), FALSE );
	}

	if( main_option_stdin_def ) {
		if( !(kit = toolkit_new_from_openfile( 
			main_toolkitgroup, main_stdin )) )
			main_log_add( "%s\n", error_get_sub() );
	}

	if( main_option_stdin_ws ) {
		if( !(ws = workspace_new_from_openfile( 
			main_workspacegroup, main_stdin )) ) 
			main_log_add( "%s\n", error_get_sub() );
		else 
			/* Don't want to have "stdin" as the filename.
			 */
			filemodel_set_filename( FILEMODEL( ws ), NULL );
	}

	/* Make sure we have a start workspace.
	 */
	if( !ws ) {
		workspacegroup_name_new( main_workspacegroup, name );
		ws = workspace_new_blank( main_workspacegroup, name );
	}

	/* Reset IM_CONCURRENCY if a watch changes. Need to do this after
	 * parsing options so we skip in batch mode.
	 */
	g_signal_connect( main_watchgroup, "watch_changed", 
		G_CALLBACK( main_watchgroup_changed_cb ), NULL );

	/* Recalc to build all classes. We have to do this in batch
	 * mode since we can find dirties through dynamic lookups. Even though
	 * you might think we could just follow recomps.
	 */
	symbol_recalculate_all_force( TRUE );

	/* Pass PATH_TMP down to vips via TMPDIR. See im_system(), for
	 * example. We need to do this after the first recomp so that prefs
	 * are loaded.
	 */
{
	char buf[FILENAME_MAX];

	expand_variables( PATH_TMP, buf );
	nativeize_path( buf );
	setenvf( "TMPDIR", "%s", buf );
}

	/* Measure amount of stuff in temp area ... need this for checking
	 * temps later. We pop a dialog if there are too many, so only useful
	 * in interactive mode.
	 */
	if( !main_option_batch )
		total = directory_size( PATH_TMP );

	/* Make nip's argc/argv[].
	 */
	kit = toolkit_new( main_toolkitgroup, "_args" );
	vips_buf_rewind( &buf );
	vips_buf_appendf( &buf, "argc = %d;", argc );
	attach_input_string( vips_buf_all( &buf ) );
	(void) parse_onedef( kit, -1 );

	vips_buf_rewind( &buf );
	vips_buf_appendf( &buf, "argv = [" );
	for( i = 0; i < argc; i++ ) {
		if( i > 0 )
			vips_buf_appendf( &buf, ", " );
		vips_buf_appendf( &buf, "\"%s\"", argv[i] );
	}
	vips_buf_appendf( &buf, "];" );

	attach_input_string( vips_buf_all( &buf ) );
	if( !parse_onedef( kit, -1 ) ) 
		main_log_add( "%s\n", error_get_sub() );

	filemodel_set_modified( FILEMODEL( kit ), FALSE );

	if( !main_option_no_load_args ) {
		/* Load args as files, if we can. 
		 */
		for( i = 1; i < argc; i++ )
			if( !main_load( ws, argv[i] ) ) 
				main_log_add( "%s\n", error_get_sub() );
	}

	/* Abandon batch mode if there are startup errors.
	 */
	if( main_option_batch ) {
		if( !main_log_is_empty() ) {
			fprintf( stderr, _( "Startup error log:\n%s" ), 
				main_log_get() );
			exit( 1 );
		}
	}

	if( main_option_set ) {
		int i;

		for( i = 0; main_option_set[i]; i++ ) 
			if( !main_set( main_option_set[i] ) )
				main_log_add( "%s\n", error_get_sub() );
	}

	symbol_recalculate_all_force( TRUE );

	/* Make sure our start ws doesn't have modified set. We may have
	 * loaded some images or whatever into it.
	 */
	filemodel_set_modified( FILEMODEL( ws ), FALSE );

#ifdef DEBUG_TIME
	printf( "DEBUG_TIME: main init in %gs\n",  
		g_timer_elapsed( startup_timer, NULL ) );
#endif /*DEBUG_TIME*/

	/* Are we running interactively? Start the main window and loop.
	 */
	if( !main_option_batch ) {
		/* Only display our initial ws if it's not blank, or if it is
		 * blank, if there are no other windows up.
		 */
		if( !workspace_is_empty( ws ) || mainw_number() == 0 ) {
			Mainw *mainw;
			
			mainw = mainw_new( ws );
			gtk_widget_show( GTK_WIDGET( mainw ) );
		}

		/* Process a few events ... we want the window to be mapped so
		 * that log/welcome/clean? messages we pop appear in the right
		 * place on the screen.
		 */
		while( g_main_context_iteration( NULL, FALSE ) )
			;

		if( main_splash ) {
			gtk_widget_destroy( GTK_WIDGET( main_splash ) );
			main_splash = NULL;
		}

		if( !main_log_is_empty() ) {
			error_top( _( "Startup error." ) );
			error_sub( _( "Startup error log:\n%s" ), 
				main_log_get() );
			box_alert( NULL );
		}

		if( welcome_message ) {
			char save_dir[FILENAME_MAX];
			char buf[256];

			im_snprintf( buf, 256, 
				_( "Welcome to %s-%s!" ), PACKAGE, VERSION );
			expand_variables( get_savedir(), save_dir );
			nativeize_path( save_dir );
			box_info( NULL, 
				buf,
_( "A new directory has been created in your home directory to hold startup, "
"data and temporary files:\n\n"
"     %s\n\n"
"If you've used previous versions of %s, you will probably want "
"to move any files over from your old work area and remove any old temps." ),
				save_dir, PACKAGE );
		}

		/* Offer to junk temps.
		 */
		main_check_temp( total );

#ifdef DEBUG
		printf( "starting event dispatch loop\n" );
#endif/*DEBUG*/

		/* Through startup.
		 */
		main_starting = FALSE;

		gtk_main();
	}

	if( main_option_test && expr_error_all )
		main_error_exit( "--test: errors found" );

	/* No return from this.
	 */
	main_quit();

	return( 0 );
}

#ifdef OS_WIN32 
/* Get non-cmd line args on win32.
 */
static int 
breakargs( char *program, char *line, char **argv ) 
{ 
	int argc = 1; 

	argv[0] = program; 

	while( *line && argc < MAX_SYSTEM - 1 ) { 
		while( *line && isspace( *line ) ) 
			line++; 

		if( *line == '"' ) {
			/* Windows-95 quoted arguments 
			 */ 
			char *start = line + 1; 
			char *end = start; 

			while( *end && *end != '"' ) 
				end++; 

			if( *end == '"' ) { 
				*end = '\0'; 
				argv[argc++] = start; 
				line = end + 1; 
				continue; 
			} 
		} 

		if( *line ) { 
			argv[argc++] = line; 
			while( *line && !isspace( *line ) ) 
				line++; 

			if( *line ) 
				*line++ = '\0'; 
		} 
	} 

	/* add trailing NULL pointer to argv 
	 */ 
	argv[argc] = NULL; 

	return( argc ); 
} 

int WINAPI 
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpszCmdLine, int nShowCmd ) 
{ 
	char *argv[MAX_SYSTEM];
	int  argc;                                               
	TCHAR program[MAXPATHLEN];                               

	GetModuleFileName( hInstance, program, sizeof(program) );  
	argc = breakargs( (char *) program, lpszCmdLine, argv );    

	return( main( argc, argv ) );
} 
#endif /*OS_WIN32*/ 
