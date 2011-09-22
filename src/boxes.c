/* Make various little popup dialogs ... error, info, question.
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

/* Max amount of text in a info/error/question dialog.
 */
#define MAX_DIALOG_TEXT (1000)

/* Find a window to use as dialog parent.
 */
static GtkWidget *
box_pick_parent( GtkWidget *par )
{
	if( !par ) {
		Mainw *mainw;

		if( (mainw = mainw_pick_one()) )
			return( GTK_WIDGET( mainw ) );
		else
			return( main_window_top );
	}
	else 
		return( par );
}

/* Make the insides of a error, info or question dialog.
 */
static void
box_build( iDialog *idlg, 
	GtkWidget *work, char *s, const char *stock_id )
{	
	GtkWidget *icon;
	GtkWidget *hb;
	GtkWidget *lab;

	hb = gtk_hbox_new( FALSE, 12 );
	gtk_container_border_width( GTK_CONTAINER( hb ), 0 );
	gtk_container_add( GTK_CONTAINER( work ), hb );
	gtk_widget_show( hb );

	icon = gtk_image_new_from_stock( stock_id, GTK_ICON_SIZE_DIALOG );
	gtk_misc_set_alignment( GTK_MISC( icon ), 0.0, 0.0 );
        gtk_box_pack_start( GTK_BOX( hb ), icon, FALSE, FALSE, 0 );
	gtk_widget_show( icon );

	lab = gtk_label_new( NULL );
	gtk_label_set_markup( GTK_LABEL( lab ), s );
        gtk_label_set_justify( GTK_LABEL( lab ), GTK_JUSTIFY_LEFT );
        gtk_label_set_selectable( GTK_LABEL( lab ), TRUE );
	gtk_label_set_line_wrap( GTK_LABEL( lab ), TRUE );
        gtk_box_pack_start( GTK_BOX( hb ), lab, FALSE, FALSE, 0 );
	gtk_widget_show( lab );
}

/* Make an error dialog.
 */
/*VARARGS2*/
static void
box_error( GtkWidget *par, const char *fmt, ... )
{
	va_list ap;
	char buf[MAX_DIALOG_TEXT];
	GtkWidget *idlg;

        va_start( ap, fmt );
	(void) im_vsnprintf( buf, MAX_DIALOG_TEXT, fmt, ap );
        va_end( ap );

	idlg = idialog_new();
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) box_build, buf, GTK_STOCK_DIALOG_ERROR, NULL );
	idialog_set_callbacks( IDIALOG( idlg ), NULL, NULL, NULL, NULL );
	idialog_add_ok( IDIALOG( idlg ), iwindow_true_cb, GTK_STOCK_OK );
	iwindow_set_parent( IWINDOW( idlg ), box_pick_parent( par ) );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );
}

/* Mark up a top/sub pair for a dialog box.
 */
static void
box_vmarkup( char *out, const char *top, const char *sub, va_list ap )
{
	char buf1[MAX_DIALOG_TEXT];
	char buf2[MAX_DIALOG_TEXT];
	char buf3[MAX_DIALOG_TEXT];

	escape_markup( top, buf1, MAX_DIALOG_TEXT );
	(void) im_vsnprintf( buf2, MAX_DIALOG_TEXT, sub, ap );
	escape_markup( buf2, buf3, MAX_DIALOG_TEXT );

	(void) im_snprintf( out, MAX_DIALOG_TEXT, 
		"<b><big>%s</big></b>", buf1 );
	if( strcmp( buf3, "" ) != 0 ) {
		int len = strlen( out );

		(void) im_snprintf( out + len, MAX_DIALOG_TEXT - len, 
			"\n\n%s", buf3 );
	}
}

static void
box_markup( char *out, const char *top, const char *sub, ... )
{
	va_list ap;

        va_start( ap, sub );
	box_vmarkup( out, top, sub, ap );
        va_end( ap );
}

/* Display buffered errors in an error dialog. 
 */
void
box_alert( GtkWidget *par )
{
	char buf[MAX_DIALOG_TEXT];

	if( !main_window_gdk ) {
		/* No X, just print.
		 */
		fprintf( stderr, "%s\n", error_get_top() );
		fprintf( stderr, "%s\n", error_get_sub() );
		return;
	}

	box_markup( buf, error_get_top(), "%s", error_get_sub() );
	box_error( par, "%s", buf );
}

/* Make an information dialog.
 */
void
box_vinfo( GtkWidget *par, const char *top, const char *sub, va_list ap )
{
	char buf[MAX_DIALOG_TEXT];
	GtkWidget *idlg;

	box_vmarkup( buf, top, sub, ap );

	idlg = idialog_new();
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) box_build, 
			buf, GTK_STOCK_DIALOG_INFO, NULL );
	idialog_set_callbacks( IDIALOG( idlg ), NULL, NULL, NULL, NULL );
	idialog_add_ok( IDIALOG( idlg ), iwindow_true_cb, GTK_STOCK_OK );
	iwindow_set_parent( IWINDOW( idlg ), box_pick_parent( par ) );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );
}

/* Make an information dialog.
 */
void
box_info( GtkWidget *par, const char *top, const char *sub, ... )
{
	va_list ap;

        va_start( ap, sub );
	box_vinfo( par, top, sub, ap );
        va_end( ap );
}

/* Pop up an 'Are you sure?' window. 
 */
iDialog *
box_yesno( GtkWidget *par, 
	iWindowFn okcb, iWindowFn cancelcb, void *client, /* Call client */
	iWindowNotifyFn nfn, void *sys,			  /* Call parent */
	const char *yes_label, 
	const char *top, const char *sub, ... )
{
	va_list ap;
	char buf[MAX_DIALOG_TEXT];
	GtkWidget *idlg;

	va_start( ap, sub );
	box_vmarkup( buf, top, sub, ap );
	va_end( ap );

	idlg = idialog_new();
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) box_build, 
			buf, GTK_STOCK_DIALOG_QUESTION, NULL );
	idialog_set_callbacks( IDIALOG( idlg ), cancelcb, NULL, NULL, client );
	idialog_add_ok( IDIALOG( idlg ), okcb, "%s", yes_label );
	idialog_set_notify( IDIALOG( idlg ), nfn, sys );
	iwindow_set_parent( IWINDOW( idlg ), box_pick_parent( par ) );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );

	return( IDIALOG( idlg ) );
}

/* Pop up a `save'/`don't save'/`cancel' dialog.
 */
void
box_savenosave( GtkWidget *par, 
	iWindowFn save, iWindowFn nosave, void *client,   /* Call client */
	iWindowNotifyFn nfn, void *sys,			  /* Call parent */
	const char *top, const char *sub, ... )
{
	va_list ap;
	char buf[MAX_DIALOG_TEXT];
	GtkWidget *idlg;

	va_start( ap, sub );
	box_vmarkup( buf, top, sub, ap );
	va_end( ap );

	idlg = idialog_new();
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) box_build, 
			buf, GTK_STOCK_DIALOG_QUESTION, NULL );
	idialog_set_callbacks( IDIALOG( idlg ), 
		iwindow_true_cb, NULL, NULL, client );
	idialog_add_ok( IDIALOG( idlg ), nosave, _( "Close _without Saving" ) );
	idialog_add_ok( IDIALOG( idlg ), save, GTK_STOCK_SAVE );
	idialog_set_notify( IDIALOG( idlg ), nfn, sys );
	iwindow_set_parent( IWINDOW( idlg ), box_pick_parent( par ) );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );
}

#define ABOUT(A) ((About *) (A))

/* Make the insides of an about box.
 */
static void
about_build( iDialog *idlg, GtkWidget *work )
{	
	/* Translators: translate this to a credit for you, and it'll appear in
	 * the About box.
	 */
	char *translator_credits = _( "translator_credits" );

	GtkWidget *hb;
	GtkWidget *lab;
	char txt[MAX_DIALOG_TEXT];
	char txt2[MAX_DIALOG_TEXT];
	VipsBuf buf = VIPS_BUF_STATIC( txt );
	GtkWidget *image;

	im_snprintf( txt2, MAX_DIALOG_TEXT, _( "About %s." ), PACKAGE );
	vips_buf_appendf( &buf, "<b><big>%s</big></b>\n\n", txt2 );
	im_snprintf( txt2, MAX_DIALOG_TEXT, 
		_( "%s is an image processing package." ), PACKAGE );
	vips_buf_appendf( &buf, "%s\n\n", txt2 );

	im_snprintf( txt2, MAX_DIALOG_TEXT, 
		_( "%s comes with ABSOLUTELY NO WARRANTY. This is "
		"free software and you are welcome to redistribute "
		"it under certain conditions, see http://www.gnu.org." ), 
		PACKAGE );
	vips_buf_appendf( &buf, "%s\n\n", txt2 );

	im_snprintf( txt2, MAX_DIALOG_TEXT, _( NIP_COPYRIGHT ), PACKAGE );
	vips_buf_appendf( &buf, "%s\n\n", txt2 );

{
	char buf1[FILENAME_MAX];
	char buf2[FILENAME_MAX];

	im_snprintf( buf1, FILENAME_MAX, "%s" G_DIR_SEPARATOR_S "start",
		get_savedir() );
	expand_variables( buf1, buf2 );
        nativeize_path( buf2 );
	escape_markup( buf2, buf1, FILENAME_MAX );
	vips_buf_appendf( &buf, "<b>%s:</b> %s\n", 
		_( "Personal start folder" ), buf1 );
}

	vips_buf_appendf( &buf, "<b>%s:</b> %s\n", 
		_( "Homepage" ), VIPS_HOMEPAGE );
	escape_markup( im_version_string(), txt2, MAX_DIALOG_TEXT );
	vips_buf_appendf( &buf, "<b>%s:</b> %s\n", 
		_( "Linked to VIPS" ), txt2 );
	escape_markup( IM_VERSION_STRING, txt2, MAX_DIALOG_TEXT );
	vips_buf_appendf( &buf, "<b>%s:</b> %s\n", 
		_( "Built against VIPS" ), txt2 );
	escape_markup( PACKAGE, txt2, MAX_DIALOG_TEXT );
	vips_buf_appendf( &buf, "<b>$PACKAGE:</b> %s\n", txt2 );
	escape_markup( VERSION, txt2, MAX_DIALOG_TEXT );
	vips_buf_appendf( &buf, "<b>$VERSION:</b> %s\n", txt2 );
	escape_markup( NN( g_getenv( "VIPSHOME" ) ), txt2, MAX_DIALOG_TEXT );
	vips_buf_appendf( &buf, "<b>$VIPSHOME:</b> %s\n", txt2 );
	escape_markup( NN( g_getenv( "HOME" ) ), txt2, MAX_DIALOG_TEXT );
	vips_buf_appendf( &buf, "<b>$HOME:</b> %s\n", txt2 );
	escape_markup( NN( g_getenv( "SAVEDIR" ) ), txt2, MAX_DIALOG_TEXT );
	vips_buf_appendf( &buf, "<b>$SAVEDIR:</b> %s\n", txt2 );
	escape_markup( PATH_TMP, txt2, MAX_DIALOG_TEXT );
	vips_buf_appendf( &buf, "<b>%s:</b> %s\n", 
		_( "Temp files in" ), txt2 );
	if( strcmp( translator_credits, "translator_credits" ) != 0 ) {
		vips_buf_appendf( &buf, "\n" ); 
		vips_buf_appends( &buf, translator_credits );
	}

	vips_buf_appendf( &buf, "\n" ); 

	mainw_find_disc( &buf );
	/* Expands to (eg.) "14GB free in /pics/tmp" */
        vips_buf_appendf( &buf, _( " in \"%s\"" ), PATH_TMP );
        vips_buf_appends( &buf, "\n" );

        vips_buf_appendf( &buf, 
		_( "%d cells in heap, %d cells free, %d cells maximum" ),
                reduce_context->heap->ncells, 
		reduce_context->heap->nfree, 
		reduce_context->heap->max_fn( reduce_context->heap ) );
        vips_buf_appends( &buf, "\n" );

        vips_buf_appendf( &buf, _( "%d vips calls cached by nip2" ), 
		cache_history_size );
        vips_buf_appends( &buf, "\n" );

        vips_buf_appendf( &buf, _( "%d vips operations cached by libvips" ), 
		vips_cache_get_size() );
        vips_buf_appends( &buf, "\n" );

        vips_buf_appendf( &buf, _( "using %d threads" ), im_concurrency_get() );
        vips_buf_appends( &buf, "\n" );

        vips_buf_appendf( &buf, _( "%d pixel buffers in vips" ), 
		vips_tracked_get_allocs() );
        vips_buf_appends( &buf, "\n" );

	to_size( &buf, vips_tracked_get_mem() );
        vips_buf_appendf( &buf, _( " of ram in pixel buffers" ) ); 
        vips_buf_appends( &buf, "\n" );

	to_size( &buf, vips_tracked_get_mem_highwater() );
        vips_buf_appendf( &buf, _( " of ram highwater mark" ) ); 
        vips_buf_appends( &buf, "\n" );

	hb = gtk_hbox_new( FALSE, 0 );
	gtk_container_border_width( GTK_CONTAINER( hb ), 10 );
	gtk_container_add( GTK_CONTAINER( work ), hb );
	gtk_widget_show( hb );

	image = image_new_from_file( 
		"$VIPSHOME/share/$PACKAGE/data/vips-128.png" );
        gtk_box_pack_start( GTK_BOX( hb ), image, FALSE, FALSE, 2 );
	gtk_widget_show( image );

	lab = gtk_label_new( "" );
	gtk_label_set_markup( GTK_LABEL( lab ), vips_buf_all( &buf ) );
        gtk_label_set_justify( GTK_LABEL( lab ), GTK_JUSTIFY_LEFT );
        gtk_label_set_selectable( GTK_LABEL( lab ), TRUE );
	gtk_label_set_line_wrap( GTK_LABEL( lab ), TRUE );
        gtk_box_pack_start( GTK_BOX( hb ), lab, FALSE, FALSE, 2 );
	gtk_widget_show( lab );
}

/* Pop up an "about" window.
 */
void
box_about( GtkWidget *par )
{
	GtkWidget *idlg;

	idlg = idialog_new();
	idialog_set_build( IDIALOG( idlg ), 
		(iWindowBuildFn) about_build, NULL, NULL, NULL );
	idialog_add_ok( IDIALOG( idlg ), iwindow_true_cb, GTK_STOCK_OK );
	iwindow_set_parent( IWINDOW( idlg ), box_pick_parent( par ) );
	iwindow_build( IWINDOW( idlg ) );

	gtk_widget_show( GTK_WIDGET( idlg ) );
}

/* A big list of all the help tags, plus the file and anchor they are defined
 * in. See makehelpindex.pl.
 */
static const char *box_helpindex[][2] = {
#include "helpindex.h"
};

/* Pop up a help window for a tag.
 */
void
box_help( GtkWidget *par, const char *name )
{
	int i;

	for( i = 0; i < IM_NUMBER( box_helpindex ); i++ )
		if( strcmp( name, box_helpindex[i][0] ) == 0 ) {
			char url[512];

			im_snprintf( url, 512, "file://%s/%s", 
				NIP_DOCPATH, box_helpindex[i][1] );
			box_url( par, url );
			return;
		}

	error_top( _( "Help page not found." ) );
	error_sub( _( "No indexed help page found for tag \"%s\"" ), name );
	iwindow_alert( par, GTK_MESSAGE_ERROR );
}

/* Name + caption dialog ... for new workspace / new column.
 */

static iDialogClass *stringset_parent_class = NULL;

void *
stringset_child_destroy( StringsetChild *ssc )
{
	ssc->ss->children = g_slist_remove( ssc->ss->children, ssc );

	IM_FREE( ssc->label );
	IM_FREE( ssc->text );
	IM_FREE( ssc->tooltip );
	IM_FREE( ssc );

	return( NULL );
}

StringsetChild *
stringset_child_new( Stringset *ss,
	const char *label, const char *text, const char *tooltip )
{
	StringsetChild *ssc = INEW( NULL, StringsetChild );

	ssc->ss = ss;
	ssc->label = im_strdup( NULL, label );
	ssc->text = im_strdup( NULL, text );
	ssc->tooltip = im_strdup( NULL, tooltip );

	ss->children = g_slist_append( ss->children, ssc );

	return( ssc );
}

static void
stringset_destroy( GtkObject *object )
{
	Stringset *ss;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_STRINGSET( object ) );

	ss = STRINGSET( object );

	slist_map( ss->children,
		(SListMapFn) stringset_child_destroy, NULL );
	UNREF( ss->group );

	if( GTK_OBJECT_CLASS( stringset_parent_class )->destroy )
		GTK_OBJECT_CLASS( stringset_parent_class )->destroy( object );
}

static void *
stringset_build_set_default( StringsetChild *ssc, iDialog *idlg )
{
	idialog_set_default_entry( idlg, GTK_ENTRY( ssc->entry ) );

	return( NULL );
}

static void
stringset_build( GtkWidget *widget )
{
	Stringset *ss = STRINGSET( widget );
	iDialog *idlg = IDIALOG( widget );
	GSList *p;

#ifdef DEBUG
	printf( "stringset_build: %s\n", IWINDOW( ss )->title );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( stringset_parent_class )->build )
		IWINDOW_CLASS( stringset_parent_class )->build( widget );

	ss->group = gtk_size_group_new( GTK_SIZE_GROUP_HORIZONTAL );

	for( p = ss->children; p; p = p->next ) {
		StringsetChild *ssc = (StringsetChild *) p->data;

		ssc->entry = 
			build_glabeltext4( idlg->work, ss->group, ssc->label );
		if( ssc->text )
			set_gentry( ssc->entry, "%s", ssc->text );
		if( ssc->tooltip )
			set_tooltip( ssc->entry, "%s", ssc->tooltip );
	}

	/* Set defaults in reverse, so we get top item with focus.
	 */
	(void) slist_map_rev( ss->children,
		(SListMapFn) stringset_build_set_default, idlg );

	gtk_widget_show_all( idlg->work );
}

static void
stringset_class_init( StringsetClass *class )
{
	GtkObjectClass *object_class;
	iWindowClass *iwindow_class;

	object_class = (GtkObjectClass *) class;
	iwindow_class = (iWindowClass *) class;

	object_class->destroy = stringset_destroy;
	iwindow_class->build = stringset_build;

	stringset_parent_class = g_type_class_peek_parent( class );
}

static void
stringset_init( Stringset *ss )
{
#ifdef DEBUG
	printf( "stringset_init: %s\n", IWINDOW( ss )->title );
#endif /*DEBUG*/

	ss->children = NULL;
}

GtkType
stringset_get_type( void )
{
	static GtkType stringset_type = 0;

	if( !stringset_type ) {
		static const GtkTypeInfo info = {
			"Stringset",
			sizeof( Stringset ),
			sizeof( StringsetClass ),
			(GtkClassInitFunc) stringset_class_init,
			(GtkObjectInitFunc) stringset_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		stringset_type = gtk_type_unique( TYPE_IDIALOG, &info );
	}

	return( stringset_type );
}

GtkWidget *
stringset_new( void )
{
	Stringset *ss = gtk_type_new( TYPE_STRINGSET );

	return( GTK_WIDGET( ss ) );
}

StringsetChild *
stringset_child_get( Stringset *ss, const char *label )
{
	GSList *p;

	for( p = ss->children; p; p = p->next ) {
		StringsetChild *ssc = (StringsetChild *) p->data;

		if( strcmp( label, ssc->label ) == 0 )
			return( ssc );
	}

	return( NULL );
}

/* Find dialog.
 */

static iDialogClass *find_parent_class = NULL;

static void
find_destroy( GtkObject *object )
{
	Find *find;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_FIND( object ) );

	find = FIND( object );

	/* My instance destroy stuff.
	 */

	if( GTK_OBJECT_CLASS( find_parent_class )->destroy )
		(*GTK_OBJECT_CLASS( find_parent_class )->destroy)( object );
}

static void
find_build( GtkWidget *widget )
{
	Find *find = FIND( widget );
	iDialog *idlg = IDIALOG( widget );

#ifdef DEBUG
	printf( "find_build: %s\n", IWINDOW( find )->title );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( find_parent_class )->build )
		(*IWINDOW_CLASS( find_parent_class )->build)( widget );

	find->search = build_glabeltext4( idlg->work, NULL, _( "Search for" ) );
	find->csens = build_gtoggle( idlg->work, _( "Case sensitive" ) );
#ifdef HAVE_GREGEX
	find->regexp = build_gtoggle( idlg->work, _( "Regular expression" ) );
#endif /*HAVE_GREGEX*/
	find->fromtop = build_gtoggle( idlg->work, _( "Search from start" ) );
	idialog_set_default_entry( idlg, GTK_ENTRY( find->search ) );
	gtk_widget_show_all( idlg->work );
}

static void
find_class_init( FindClass *class )
{
	GtkObjectClass *object_class;
	iWindowClass *iwindow_class;

	object_class = (GtkObjectClass *) class;
	iwindow_class = (iWindowClass *) class;

	object_class->destroy = find_destroy;
	iwindow_class->build = find_build;

	find_parent_class = g_type_class_peek_parent( class );
}

static void
find_init( Find *find )
{
#ifdef DEBUG
	printf( "find_init: %s\n", IWINDOW( find )->title );
#endif /*DEBUG*/

	idialog_set_pinup( IDIALOG( find ), TRUE );
}

GtkType
find_get_type( void )
{
	static GtkType find_type = 0;

	if( !find_type ) {
		static const GtkTypeInfo info = {
			"Find",
			sizeof( Find ),
			sizeof( FindClass ),
			(GtkClassInitFunc) find_class_init,
			(GtkObjectInitFunc) find_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		find_type = gtk_type_unique( TYPE_IDIALOG, &info );
	}

	return( find_type );
}

GtkWidget *
find_new( void )
{
	Find *find = gtk_type_new( TYPE_FIND );

	return( GTK_WIDGET( find ) );
}

/* Imageheader dialog.
 */

static iDialogClass *imageheader_parent_class = NULL;

static void
imageheader_destroy( GtkObject *object )
{
	Imageheader *imageheader;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_IMAGEHEADER( object ) );

	imageheader = IMAGEHEADER( object );

	/* My instance destroy stuff.
	 */
	UNREF( imageheader->conv );

	if( GTK_OBJECT_CLASS( imageheader_parent_class )->destroy )
		GTK_OBJECT_CLASS( imageheader_parent_class )->destroy( object );
}

/* Make a row for the header display. Align the names with "group".
 */
static GtkWidget *
imageheader_row_new( GtkSizeGroup *group, const char *field, GValue *value )
{
	GtkWidget *hbox;
	GtkWidget *label;
	const char *extra;
        char *str_value;

	hbox = gtk_hbox_new( FALSE, 12 );

	label = gtk_label_new( field );
	gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 2 );
	gtk_size_group_add_widget( group, label );

	label = gtk_label_new( "=" );
        gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 2 );

	label = gtk_label_new( "" );
        gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 2 );

	/* Look for enums and decode them.
	 */
	extra = NULL;
	if( strcmp( field, "Coding" ) == 0 )
		extra = NN( im_Coding2char( g_value_get_int( value ) ) );
	else if( strcmp( field, "BandFmt" ) == 0 )
		extra = NN( im_BandFmt2char( g_value_get_int( value ) ) );
	else if( strcmp( field, "Type" ) == 0 )
		extra = NN( im_Type2char( g_value_get_int( value ) ) );

	str_value = g_strdup_value_contents( value );
	if( extra )
		set_glabel( label, "%s - %s", str_value, extra );
	else
		set_glabel( label, "%s", str_value );
	g_free( str_value );

	return( hbox );
}

/* Mapped over the header fields to make rows.
 */
static void *
imageheader_row_map_fn( IMAGE *im, 
	const char *field, GValue *value, Imageheader *imageheader )
{
	GtkWidget *hbox;

	/* This is just there for back compt, hide it.
	 */
	if( strcmp( field, "Bbits" ) == 0 )
		return( NULL );

	hbox = imageheader_row_new( imageheader->group, field, value );
	gtk_box_pack_start( GTK_BOX( imageheader->fields ), 
		hbox, FALSE, FALSE, 0 );

	return( NULL );
}

static void
imageheader_refresh( Imageheader *imageheader )
{
	DESTROY_GTK( imageheader->fields );

	if( imageheader->conv && imageheader->conv->ii ) {
		IMAGE *im = imageinfo_get( FALSE, imageheader->conv->ii );

		imageheader->group = 
			gtk_size_group_new( GTK_SIZE_GROUP_HORIZONTAL );
		imageheader->fields = gtk_vbox_new( FALSE, 2 );
		gtk_scrolled_window_add_with_viewport( 
			GTK_SCROLLED_WINDOW( imageheader->swin_fields ), 
			imageheader->fields );
		im_header_map( im, 
			(im_header_map_fn) imageheader_row_map_fn,
			imageheader );
		gtk_widget_show_all( imageheader->fields );

		gtk_text_buffer_set_text( 
			gtk_text_view_get_buffer( 
				GTK_TEXT_VIEW( imageheader->history ) ),
			im_history_get( im ), -1 );
	}
	else {
		gtk_editable_delete_text( GTK_EDITABLE( imageheader->history ),
			0, -1 );
	}
}

static void
imageheader_build( GtkWidget *widget )
{
	Imageheader *imageheader = IMAGEHEADER( widget );
	iDialog *idlg = IDIALOG( widget );

	GtkWidget *label;
	GtkWidget *swin;
	GtkWidget *pane;
	GtkWidget *vbox;
	PangoFontDescription *font_desc;

#ifdef DEBUG
	printf( "imageheader_build: %s\n", IWINDOW( imageheader )->title );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( imageheader_parent_class )->build )
		(*IWINDOW_CLASS( imageheader_parent_class )->build)( widget );

	pane = gtk_vpaned_new();
        gtk_box_pack_start( GTK_BOX( idlg->work ), pane, TRUE, TRUE, 2 );

	/* Scrolled area to hold fields.
	 */
	vbox = gtk_vbox_new( FALSE, 2 );
	gtk_paned_pack1( GTK_PANED( pane ), vbox, TRUE, FALSE );
	label = gtk_label_new( _( "Image header fields" ) );
	gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( vbox ), label, FALSE, FALSE, 2 );
	imageheader->swin_fields = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( 
		GTK_SCROLLED_WINDOW( imageheader->swin_fields ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        gtk_box_pack_start( GTK_BOX( vbox ), 
		imageheader->swin_fields, TRUE, TRUE, 2 );

	/* Created in _refresh()
	 */
	imageheader->fields = NULL;

	vbox = gtk_vbox_new( FALSE, 2 );
	gtk_paned_pack2( GTK_PANED( pane ), vbox, TRUE, FALSE );
	label = gtk_label_new( _( "Image history" ) );
	gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( vbox ), label, FALSE, FALSE, 2 );
	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        gtk_box_pack_end( GTK_BOX( vbox ), swin, TRUE, TRUE, 2 );
	imageheader->history = gtk_text_view_new();
	gtk_text_view_set_editable( GTK_TEXT_VIEW( imageheader->history ), 
		FALSE );
	gtk_text_view_set_cursor_visible( GTK_TEXT_VIEW( imageheader->history ),
		FALSE );
	font_desc = pango_font_description_from_string( "Mono" );
	gtk_widget_modify_font( imageheader->history, font_desc );
	pango_font_description_free( font_desc );
	gtk_container_add( GTK_CONTAINER( swin ), imageheader->history );

	imageheader_refresh( imageheader );

        gtk_window_set_default_size( GTK_WINDOW( imageheader ), 550, 550 );

	gtk_widget_show_all( idlg->work );
}

static void
imageheader_class_init( ImageheaderClass *class )
{
	GtkObjectClass *object_class;
	iWindowClass *iwindow_class;

	object_class = (GtkObjectClass *) class;
	iwindow_class = (iWindowClass *) class;

	object_class->destroy = imageheader_destroy;
	iwindow_class->build = imageheader_build;

	imageheader_parent_class = g_type_class_peek_parent( class );
}

static void
imageheader_init( Imageheader *imageheader )
{
#ifdef DEBUG
	printf( "imageheader_init: %s\n", IWINDOW( imageheader )->title );
#endif /*DEBUG*/

	imageheader->conv = NULL;
}

GtkType
imageheader_get_type( void )
{
	static GtkType imageheader_type = 0;

	if( !imageheader_type ) {
		static const GtkTypeInfo info = {
			"Imageheader",
			sizeof( Imageheader ),
			sizeof( ImageheaderClass ),
			(GtkClassInitFunc) imageheader_class_init,
			(GtkObjectInitFunc) imageheader_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		imageheader_type = gtk_type_unique( TYPE_IDIALOG, &info );
	}

	return( imageheader_type );
}

/* Conversion has changed signal.
 */
static void
imageheader_conv_changed( Model *model, Imageheader *imageheader )
{
	g_assert( IS_MODEL( model ) );
	g_assert( IS_IMAGEHEADER( imageheader ) );

	imageheader_refresh( imageheader );
}

static void
imageheader_link( Imageheader *imageheader, Conversion *conv )
{
	imageheader->conv = conv;
	g_object_ref( G_OBJECT( conv ) );
	iobject_sink( IOBJECT( conv ) );

	listen_add( G_OBJECT( imageheader ), (GObject **) &imageheader->conv,
		"changed", G_CALLBACK( imageheader_conv_changed ) );
}

GtkWidget *
imageheader_new( Conversion *conv )
{
	Imageheader *imageheader = gtk_type_new( TYPE_IMAGEHEADER );

	imageheader_link( imageheader, conv );

	return( GTK_WIDGET( imageheader ) );
}

/* Launch a viewer on a URL.
 */
void
box_url( GtkWidget *par, const char *url )
{
#ifdef OS_WIN32
	char url2[FILENAME_MAX];
	int v;
	
	expand_variables( url, url2 );
	v = (int) ShellExecute( NULL, "open", url2, NULL, NULL, SW_SHOWNORMAL );
	if( v <= 32 ) {
		error_top( _( "Unable to view help file." ) );
		error_sub( _( "Unable to open URL \"%s\", "
			"windows error code = %d." ), url, v );
		iwindow_alert( par, GTK_MESSAGE_ERROR );
	}
#elif defined OS_DARWIN
	(void) systemf( "open %s", url );
#elif defined HAVE_XDG_OPEN
	static gboolean shown = FALSE;

	if( systemf( "%s %s", XDG_OPEN, url ) ) {
		error_top( _( "Unable to view help file." ) );
		error_sub( _( "Attempt to view URL with xdg-open failed\n%s" ),
			url );
		iwindow_alert( par, GTK_MESSAGE_ERROR );
	}
	else if( !shown ) {
		error_top( _( "Browser window opened." ) );
		error_sub( "%s", 
			_( "You may need to switch desktops to see the "
			"new window." ) );
		iwindow_alert( par, GTK_MESSAGE_INFO );
		shown = TRUE;
	}
#else /*default unix-y*/
	static gboolean shown = FALSE;

	char txt[512];
	VipsBuf buf = VIPS_BUF_STATIC( txt );
	char txt2[512];
	VipsBuf buf2 = VIPS_BUF_STATIC( txt2 );

	char url2[FILENAME_MAX];

	expand_variables( url, url2 );

	vips_buf_appendf( &buf, "%s %s", BOX_BROWSER, BOX_BROWSER_REMOTE );
	vips_buf_appendf( &buf2, vips_buf_all( &buf ), url2 );

	if( systemf( "%s", vips_buf_all( &buf2 ) ) ) {
		error_top( _( "Unable to view help file." ) );
		error_sub( _( 
			"Attempted to launch browser with command:\n"
			"  %s\n"
			"You can change this command in Preferences." ),
			vips_buf_all( &buf2 ) );
		iwindow_alert( par, GTK_MESSAGE_ERROR );
	}
	else if( !shown ) {
		error_top( _( "Browser window opened." ) );
		error_sub( "%s",
			_( "You may need to switch desktops to see the "
			"new window." ) );
		iwindow_alert( par, GTK_MESSAGE_INFO );
		shown = TRUE;
	}
#endif /*lots*/
}

/* Fontchooser dialog.
 */

static iDialogClass *fontchooser_parent_class = NULL;

static void
fontchooser_build( GtkWidget *widget )
{
	Fontchooser *fontchooser = FONTCHOOSER( widget );
	iDialog *idlg = IDIALOG( widget );

#ifdef DEBUG
	printf( "fontchooser_build: %s\n", IWINDOW( fontchooser )->title );
#endif /*DEBUG*/

	/* Call all builds in superclasses.
	 */
	if( IWINDOW_CLASS( fontchooser_parent_class )->build )
		(*IWINDOW_CLASS( fontchooser_parent_class )->build)( widget );

	fontchooser->fontchooser = gtk_font_selection_new();
        gtk_box_pack_start( GTK_BOX( idlg->work ), 
		fontchooser->fontchooser, TRUE, TRUE, 2 );

	iwindow_set_title( IWINDOW( idlg ), _( "Select Font" ) );

	gtk_widget_show_all( idlg->work );
}

static void
fontchooser_class_init( FontchooserClass *class )
{
	iWindowClass *iwindow_class;

	fontchooser_parent_class = g_type_class_peek_parent( class );

	iwindow_class = (iWindowClass *) class;

	iwindow_class->build = fontchooser_build;
}

static void
fontchooser_init( Fontchooser *fontchooser )
{
}

GtkType
fontchooser_get_type( void )
{
	static GtkType fontchooser_type = 0;

	if( !fontchooser_type ) {
		static const GtkTypeInfo info = {
			"Fontchooser",
			sizeof( Fontchooser ),
			sizeof( FontchooserClass ),
			(GtkClassInitFunc) fontchooser_class_init,
			(GtkObjectInitFunc) fontchooser_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		fontchooser_type = gtk_type_unique( TYPE_IDIALOG, &info );
	}

	return( fontchooser_type );
}

Fontchooser *
fontchooser_new( void )
{
	Fontchooser *fontchooser = gtk_type_new( TYPE_FONTCHOOSER );

	return( fontchooser );
}

gboolean 
fontchooser_set_font_name( Fontchooser *fontchooser, const char *font_name )
{
	if( !gtk_font_selection_set_font_name( 
		GTK_FONT_SELECTION( fontchooser->fontchooser ), font_name ) ) {
		error_top( _( "Font not found." ) );
		error_sub( _( "Font \"%s\" not found on system." ),
			font_name );
		return( FALSE );
	}

	return( TRUE );
}

char *
fontchooser_get_font_name( Fontchooser *fontchooser )
{
	return( gtk_font_selection_get_font_name( 
		GTK_FONT_SELECTION( fontchooser->fontchooser ) ) );
}

/* Fontbutton.
 */

/* Our signals. 
 */
enum {
	SIG_CHANGED,	/* New font selected */
	SIG_LAST
};

static GtkButtonClass *fontbutton_parent_class = NULL;

static guint fontbutton_signals[SIG_LAST] = { 0 };

static void
fontbutton_finalize( GObject *gobject )
{
	Fontbutton *fontbutton;

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_FONTBUTTON( gobject ) );

	fontbutton = FONTBUTTON( gobject );

	IM_FREE( fontbutton->font_name );

	G_OBJECT_CLASS( fontbutton_parent_class )->finalize( gobject );
}

static void
fontbutton_ok_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Fontchooser *fontchooser = FONTCHOOSER( iwnd );
	Fontbutton *fontbutton = FONTBUTTON( client );
	char *font_name;

	font_name = fontchooser_get_font_name( fontchooser ); 
	fontbutton_set_font_name( fontbutton, font_name );
	g_free( font_name );

	nfn( sys, IWINDOW_YES );
}

static void 
fontbutton_popdown_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys ) 
{
	Fontbutton *fontbutton = FONTBUTTON( client );

	fontbutton->fontchooser = NULL;

	nfn( sys, IWINDOW_YES );
}

static void
fontbutton_clicked( GtkButton *button )
{
	Fontbutton *fontbutton = FONTBUTTON( button );

	if( fontbutton->fontchooser ) 
		gtk_window_present( GTK_WINDOW( fontbutton->fontchooser ) );
	else {
		fontbutton->fontchooser = fontchooser_new();
		iwindow_set_title( IWINDOW( fontbutton->fontchooser ), 
			_( "Pick a font" ) );
		idialog_set_callbacks( IDIALOG( fontbutton->fontchooser ), 
			iwindow_true_cb, fontbutton_popdown_cb, NULL,
			fontbutton );
		idialog_add_ok( IDIALOG( fontbutton->fontchooser ), 
			fontbutton_ok_cb, _( "Set Font" ) );
		iwindow_set_parent( IWINDOW( fontbutton->fontchooser ), 
			GTK_WIDGET( button ) );
		idialog_set_pinup( IDIALOG( fontbutton->fontchooser ), TRUE );
		iwindow_build( IWINDOW( fontbutton->fontchooser ) );
		fontchooser_set_font_name( fontbutton->fontchooser, 
			fontbutton->font_name ); 

		gtk_widget_show( GTK_WIDGET( fontbutton->fontchooser ) );
	}
}

static void
fontbutton_real_changed( Fontbutton *fontbutton )
{
}

static void
fontbutton_class_init( FontbuttonClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	GtkButtonClass *bobject_class = (GtkButtonClass *) class;

	fontbutton_parent_class = g_type_class_peek_parent( class );

	gobject_class->finalize = fontbutton_finalize;

	bobject_class->clicked = fontbutton_clicked;

	class->changed = fontbutton_real_changed;

	fontbutton_signals[SIG_CHANGED] = g_signal_new( "changed",
		G_OBJECT_CLASS_TYPE( gobject_class ),
		G_SIGNAL_RUN_FIRST,
		G_STRUCT_OFFSET( FontbuttonClass, changed ),
		NULL, NULL,
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE, 0 );
}

static void
fontbutton_init( Fontbutton *fontbutton )
{
	fontbutton->font_name = NULL;
	fontbutton->fontchooser = NULL;

	set_tooltip( GTK_WIDGET( fontbutton ), _( "Click to select font" ) );
}

GtkType
fontbutton_get_type( void )
{
	static GtkType fontbutton_type = 0;

	if( !fontbutton_type ) {
		static const GtkTypeInfo info = {
			"Fontbutton",
			sizeof( Fontbutton ),
			sizeof( FontbuttonClass ),
			(GtkClassInitFunc) fontbutton_class_init,
			(GtkObjectInitFunc) fontbutton_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		fontbutton_type = gtk_type_unique( GTK_TYPE_BUTTON, &info );
	}

	return( fontbutton_type );
}

Fontbutton *
fontbutton_new( void )
{
	Fontbutton *fontbutton = g_object_new( TYPE_FONTBUTTON, 
		"label", "Sans 12", NULL );

	return( fontbutton );
}

void
fontbutton_set_font_name( Fontbutton *fontbutton, const char *font_name )
{
	char font[256];
	char button_text[256];
	int i;

	if( !fontbutton->font_name ||
		strcmp( fontbutton->font_name, font_name ) != 0 ) {
		IM_SETSTR( fontbutton->font_name, font_name );

		im_strncpy( font, font_name, 256 );
		for( i = strlen( font ) - 1; i > 0 && isdigit( font[i] ); i-- )
			font[i] = '\0';
		im_snprintf( button_text, 256, 
			"<span font_desc=\"%s\" size=\"medium\">%s</span>",
			font, font_name );
		gtk_label_set_markup( 
			GTK_LABEL( gtk_bin_get_child( GTK_BIN( fontbutton ) ) ),
			button_text );

		if( fontbutton->fontchooser )
			fontchooser_set_font_name( fontbutton->fontchooser,
				font_name );

		g_signal_emit( G_OBJECT( fontbutton ), 
			fontbutton_signals[SIG_CHANGED], 0 );
	}
}

const char *
fontbutton_get_font_name( Fontbutton *fontbutton )
{
	return( fontbutton->font_name );
}

/* Infobar. Optional: it's only in quite recent gtk.
 */
#ifdef USE_INFOBAR

static GtkInfoBarClass *infobar_parent_class = NULL;

static void
infobar_destroy( GtkObject *object )
{
	Infobar *infobar;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_INFOBAR( object ) );

	infobar = INFOBAR( object );

	IM_FREEF( g_source_remove, infobar->close_timeout );
	IM_FREEF( g_source_remove, infobar->close_animation_timeout );

	GTK_OBJECT_CLASS( infobar_parent_class )->destroy( object );
}

static void
infobar_class_init( InfobarClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;

	infobar_parent_class = g_type_class_peek_parent( class );

	object_class->destroy = infobar_destroy;
}

static void
infobar_init( Infobar *infobar )
{
	infobar->top = NULL;
	infobar->sub = NULL;
	infobar->close_timeout = 0;
	infobar->close_animation_timeout = 0;
	infobar->height = 0;
}

GType
infobar_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( InfobarClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) infobar_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Infobar ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) infobar_init,
		};

		type = g_type_register_static( GTK_TYPE_INFO_BAR, 
			"Infobar", &info, 0 );
	}

	return( type );
}

static void
infobar_cancel_close( Infobar *infobar )
{
	IM_FREEF( g_source_remove, infobar->close_timeout );
	IM_FREEF( g_source_remove, infobar->close_animation_timeout );
	gtk_widget_set_size_request( GTK_WIDGET( infobar ), -1, -1 );
}

static void
infobar_hide( Infobar *infobar )
{
	infobar_cancel_close( infobar );
	gtk_widget_hide( GTK_WIDGET( infobar ) );
	gtk_widget_hide( GTK_WIDGET( infobar->sub ) );
	gtk_widget_set_sensitive( GTK_WIDGET( infobar->info ), TRUE );
}

static gboolean
infobar_close_animation_timeout( Infobar *infobar )
{
	infobar->height -= 20;
	if( infobar->height <= 0 ) {
		infobar_hide( infobar );
		return( FALSE );
	}
	gtk_widget_set_size_request( GTK_WIDGET( infobar ), 
		-1, infobar->height );

	return( TRUE );
}

static void
infobar_start_close( Infobar *infobar )
{
	infobar_cancel_close( infobar );

	infobar->height = GTK_WIDGET( infobar )->allocation.height;
	infobar->close_animation_timeout = g_timeout_add( 50, 
		(GSourceFunc) infobar_close_animation_timeout, infobar );
}

static gboolean
infobar_close_timeout( Infobar *infobar )
{
	infobar_start_close( infobar );

	return( FALSE );
}

static void
infobar_show( Infobar *infobar )
{
	infobar_cancel_close( infobar );

	infobar->close_timeout = g_timeout_add( 5000, 
		(GSourceFunc) infobar_close_timeout, infobar );

	gtk_widget_show( GTK_WIDGET( infobar ) );
}

static void                
infobar_info_cb( GtkWidget *button, Infobar *infobar )
{
	infobar_cancel_close( infobar );
	gtk_widget_show( GTK_WIDGET( infobar->sub ) );
	gtk_widget_set_sensitive( GTK_WIDGET( infobar->info ), FALSE );
}

static void                
infobar_close_cb( GtkWidget *button, Infobar *infobar )
{
	infobar_start_close( infobar );
}

Infobar *
infobar_new( void )
{
	Infobar *infobar;
	GtkWidget *vbox;
	GtkWidget *content_area;
	GtkWidget *hbox;
	GtkWidget *action_area;
	GtkWidget *button;

	infobar = g_object_new( TYPE_INFOBAR, NULL );

	vbox = gtk_vbox_new( FALSE, 10 );
	content_area = gtk_info_bar_get_content_area( GTK_INFO_BAR( infobar ) );
	gtk_container_add( GTK_CONTAINER( content_area ), vbox );
	gtk_widget_show( vbox );

	infobar->top = gtk_label_new( "" );
        gtk_label_set_justify( GTK_LABEL( infobar->top ), GTK_JUSTIFY_LEFT );
        gtk_label_set_selectable( GTK_LABEL( infobar->top ), TRUE );
	gtk_label_set_line_wrap( GTK_LABEL( infobar->top ), TRUE );
	gtk_container_add( GTK_CONTAINER( vbox ), infobar->top );
	gtk_widget_show( infobar->top );

	infobar->sub = gtk_label_new( "" );
        gtk_label_set_justify( GTK_LABEL( infobar->sub ), GTK_JUSTIFY_LEFT );
        gtk_label_set_selectable( GTK_LABEL( infobar->sub ), TRUE );
	gtk_label_set_line_wrap( GTK_LABEL( infobar->sub ), TRUE );
	gtk_container_add( GTK_CONTAINER( vbox ), infobar->sub );

	/* We can't use gtk_info_bar_add_button(), we need the buttons
	 * horizontally.
	 */

	hbox = gtk_hbox_new( FALSE, 2 );
	action_area = gtk_info_bar_get_action_area( GTK_INFO_BAR( infobar ) );
	gtk_container_add( GTK_CONTAINER( action_area ), hbox );
	gtk_widget_show( hbox );

	button = gtk_button_new_from_stock( GTK_STOCK_CLOSE );
        gtk_box_pack_end( GTK_BOX( hbox ), button, TRUE, TRUE, 2 );
	g_signal_connect( button, "clicked",
		G_CALLBACK( infobar_close_cb ), infobar );
	gtk_widget_show( button );

	infobar->info = gtk_button_new_from_stock( GTK_STOCK_INFO );
        gtk_box_pack_end( GTK_BOX( hbox ), infobar->info, TRUE, TRUE, 2 );
	g_signal_connect( infobar->info, "clicked",
		G_CALLBACK( infobar_info_cb ), infobar );
	gtk_widget_show( infobar->info );

	return( infobar );
}

#else /*!USE_INFOBAR*/

Infobar *
infobar_new( void )
{
	return( NULL );
}

#endif /*USE_INFOBAR*/

/* Set the label on an infobar to some marked-up text.
 */
void
infobar_vset( Infobar *infobar, GtkMessageType type, 
	const char *top, const char *sub, va_list ap )
{
#ifdef USE_INFOBAR
	char buf1[MAX_DIALOG_TEXT];
	char buf2[MAX_DIALOG_TEXT];
	char *p;

	escape_markup( top, buf1, MAX_DIALOG_TEXT );
	im_snprintf( buf2, MAX_DIALOG_TEXT, "<b>%s</b>", buf1 );
	gtk_label_set_markup( GTK_LABEL( infobar->top ), buf2 );

	(void) im_vsnprintf( buf1, MAX_DIALOG_TEXT, sub, ap );
	escape_markup( buf1, buf2, MAX_DIALOG_TEXT );

	/* Remove any trailing newlines, they make infobars rather large.
	 */
	while( (p = buf2 + strlen( buf2 )) > buf2 && p[-1] == '\n' )
		p[-1] = '\0';

	gtk_label_set_markup( GTK_LABEL( infobar->sub ), buf2 );

	gtk_info_bar_set_message_type( GTK_INFO_BAR( infobar ), type );

	infobar_show( infobar );
#endif /*USE_INFOBAR*/
}

/* Set the label on an infobar to some marked-up text.
 */
void
infobar_set( Infobar *infobar, GtkMessageType type, 
	const char *top, const char *sub, ... )
{
	va_list ap;

        va_start( ap, sub );
	infobar_vset( infobar, type, top, sub, ap );
        va_end( ap );
}
