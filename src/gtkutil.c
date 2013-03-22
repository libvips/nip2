/* gtkutil functions.
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

/*
#define DEBUG
 */

/* All our tooltips.
 */
static GtkTooltips *our_tooltips = NULL;

/* Set two adjustments together.
 */
void
adjustments_set_value( GtkAdjustment *hadj, GtkAdjustment *vadj,
        float hval, float vval )
{
        gboolean hchanged = FALSE;
        gboolean vchanged = FALSE;

        if( hval != hadj->value ) {
                hadj->value = hval;
                hchanged = TRUE;
        }
        if( vval != vadj->value ) {
                vadj->value = vval;
                vchanged = TRUE;
        }

#ifdef DEBUG
        if( hchanged )
                printf( "adjustments_set_value: hadj = %g\n", hval );
        if( vchanged )
                printf( "adjustments_set_value: vadj = %g\n", vval );
#endif /*DEBUG*/

        if( hchanged )
                gtk_adjustment_value_changed( hadj );
        if( vchanged )
                gtk_adjustment_value_changed( vadj );
}

void *
object_destroy( void *obj )
{
	gtk_object_destroy( GTK_OBJECT( obj ) );

	return( NULL );
}

/* Like g_free, but return NULL for list maps.
 */
void *
null_g_free( void *obj )
{
	g_free( obj );

	return( NULL );
}

/* Set visible/not.
 */
void
widget_visible( GtkWidget *widget, gboolean visible )
{
	if( widget && visible )
		gtk_widget_show( widget );
	else if( widget && !visible )
		gtk_widget_hide( widget );
}

/* Make a button widget.
 */
GtkWidget *
build_button( const char *stock_id, GtkSignalFunc cb, gpointer user )
{
	GtkWidget *but;

	but = gtk_button_new_from_stock( stock_id );
	GTK_WIDGET_SET_FLAGS( but, GTK_CAN_DEFAULT );
	gtk_signal_connect( GTK_OBJECT( but ), "clicked", cb, user );

	return( but );
}

/* Calculate the bounding box for a string rendered with a widget's default
 * font. Set geo to a rect with 0,0 positioned on the left-hand baseline.
 */
void
get_geo( GtkWidget *widget, const char *text, Rect *geo )
{
	PangoLayout *layout;
	int width, height;

	layout = gtk_widget_create_pango_layout( widget, text );
	pango_layout_get_pixel_size( layout, &width, &height );
	g_object_unref( layout );

	/* FIXME ... we left/top to 0 for now.
	 */
	geo->width = width;
	geo->height = height;
	geo->left = 0;
	geo->top = 0;
}

/* Set a widget to a fixed size ... width in characters.
 */
void
set_fixed( GtkWidget *widget, int nchars )
{
	Rect geo;

	get_geo( widget, "8", &geo );
	gtk_widget_set_size_request( widget, geo.width * nchars, geo.height );
}

/* Build a GtkEntry, with a widget width specified in characters.
 */
GtkWidget *
build_entry( int nchars )
{
	GtkWidget *entry;

	entry = gtk_entry_new();
	gtk_entry_set_width_chars( GTK_ENTRY( entry ), nchars );

	return( entry );
}

/* Build a new menu.
 */
GtkWidget *
menu_build( const char *name )
{
	GtkWidget *menu;

	menu = gtk_menu_new();
	gtk_menu_set_title( GTK_MENU( menu ), name );

	return( menu );
}

/* Add a menu item.
 */
GtkWidget *
menu_add_but( GtkWidget *menu, 
	const char *stock_id, GtkSignalFunc cb, void *user )
{
	GtkWidget *but;

	/* We don't provide an accel group for popup menus.
	 */
	but = gtk_image_menu_item_new_from_stock( stock_id, NULL );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu ), but );
	gtk_widget_show( but );
	gtk_signal_connect( GTK_OBJECT( but ), "activate", cb, user );

	return( but );
}

/* Add a toggle item.
 */
GtkWidget *
menu_add_tog( GtkWidget *menu, const char *name, GtkSignalFunc cb, void *user )
{
	GtkWidget *tog;

	tog = gtk_check_menu_item_new_with_mnemonic( name );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu ), tog );
	gtk_widget_show( tog );
	gtk_signal_connect( GTK_OBJECT( tog ), "toggled", cb, user );

	return( tog );
}

/* Add a separator.
 */
GtkWidget *
menu_add_sep( GtkWidget *menu )
{
	GtkWidget *sep;

	sep = gtk_menu_item_new();
	gtk_widget_set_sensitive( GTK_WIDGET( sep ), FALSE );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu ), sep );
	gtk_widget_show( sep );

	return( sep );
}

/* Add a pullright.
 */
GtkWidget *
menu_add_pullright( GtkWidget *menu, const char *stock_id )
{
	GtkWidget *pullright;
	GtkWidget *subpane;

	subpane = gtk_menu_new();
	pullright = gtk_image_menu_item_new_from_stock( stock_id, NULL );
	gtk_menu_item_set_submenu( GTK_MENU_ITEM( pullright ), subpane );
	gtk_menu_shell_append( GTK_MENU_SHELL( menu ), pullright );
	gtk_widget_show( pullright );

	return( subpane );
}

/* Four quarks: each menu item has a quark linking back to the main pane, 
 * plus a quark for the user signal. The main pane has a quark linking to the
 * widget the menu was popped from, and that has the userdata for this context.
 * One more quark holds the popup in a host.
 */
static GQuark quark_main = 0;
static GQuark quark_host = 0;
static GQuark quark_data = 0;
static GQuark quark_popup = 0;

/* Build a new popup menu.
 */
GtkWidget *
popup_build( const char *name )
{
	/* Build our quarks.
	 */
	if( !quark_main ) {
		quark_main = g_quark_from_static_string( "quark_main" );
		quark_host = g_quark_from_static_string( "quark_host" );
		quark_data = g_quark_from_static_string( "quark_data" );
		quark_popup = g_quark_from_static_string( "quark_popup" );
	}

	return( menu_build( name ) );
}

/* Activate function for a popup menu item.
 */
static void
popup_activate_cb( GtkWidget *item, PopupFunc cb )
{
	GtkWidget *qmain = gtk_object_get_data_by_id( 
		GTK_OBJECT( item ), quark_main );
	GtkWidget *qhost = gtk_object_get_data_by_id( 
		GTK_OBJECT( qmain ), quark_host );
	void *qdata = gtk_object_get_data_by_id( 
		GTK_OBJECT( qhost ), quark_data );

	(*cb)( item, qhost, qdata );
}

/* Add a menu item to a popup.
 */
GtkWidget *
popup_add_but( GtkWidget *popup, const char *name, PopupFunc cb )
{
	GtkWidget *but = menu_add_but( popup, name, 
		GTK_SIGNAL_FUNC( popup_activate_cb ), (void *) cb );

	gtk_object_set_data_by_id( GTK_OBJECT( but ), quark_main, popup );

	return( but );
}

/* Add a toggle item to a popup.
 */
GtkWidget *
popup_add_tog( GtkWidget *popup, const char *name, PopupFunc cb )
{
	GtkWidget *tog = menu_add_tog( popup, name, 
		GTK_SIGNAL_FUNC( popup_activate_cb ), (void *) cb );

	gtk_object_set_data_by_id( GTK_OBJECT( tog ), quark_main, popup );

	return( tog );
}

/* Add a pullright item to a popup. Return the empty sub-pane.
 */
GtkWidget *
popup_add_pullright( GtkWidget *popup, const char *name )
{
	GtkWidget *pullright = menu_add_pullright( popup, name );

	gtk_object_set_data_by_id( GTK_OBJECT( pullright ), quark_main, popup );

	return( pullright );
}

/* Show the popup.
 */
void
popup_show( GtkWidget *host, GdkEvent *ev )
{
	GtkWidget *popup = gtk_object_get_data_by_id( 
		GTK_OBJECT( host ), quark_popup );

	gtk_object_set_data_by_id( GTK_OBJECT( popup ), quark_host, host );
	gtk_menu_popup( GTK_MENU( popup ), NULL, NULL,
		(GtkMenuPositionFunc) NULL, NULL, 3, ev->button.time );
}

/* Event handler for popupshow.
 */
static gboolean
popup_handle_event( GtkWidget *host, GdkEvent *ev, gpointer dummy )
{
	gboolean handled = FALSE;

        if( ev->type == GDK_BUTTON_PRESS && ev->button.button == 3 ) {
		popup_show( host, ev );
		handled = TRUE;
	}
	else if( ev->type == GDK_KEY_PRESS && ev->key.keyval == GDK_F10 && 
		ev->key.state & GDK_SHIFT_MASK ) {
		popup_show( host, ev );
		handled = TRUE;
	}

	return( handled );
}

/* Link a host to a popup.
 */
void
popup_link( GtkWidget *host, GtkWidget *popup, void *data )
{
	gtk_object_set_data_by_id( GTK_OBJECT( host ), quark_popup, popup );
	gtk_object_set_data_by_id( GTK_OBJECT( host ), quark_data, data );
}

/* Add a callback to show a popup.
 */
guint
popup_attach( GtkWidget *host, GtkWidget *popup, void *data )
{
	guint sid;

	popup_link( host, popup, data );

	/* We can't just use gtk_menu_attach_to_widget(), since that can only
	 * attach a menu to a single widget. We want to be able to attach a
	 * single menu to meny widgets.
	 */
        sid = gtk_signal_connect( GTK_OBJECT( host ), "event",
                GTK_SIGNAL_FUNC( popup_handle_event ), NULL );

	return( sid );
}

void
popup_detach( GtkWidget *host, guint sid )
{
	gtk_signal_disconnect( GTK_OBJECT( host ), sid );
}

static void
set_tooltip_events( GtkWidget *wid )
{
	gtk_widget_add_events( wid, 
		GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK );
}

/* Set the tooltip on a widget.
 */
void
set_tooltip( GtkWidget *wid, const char *fmt, ... )
{
	va_list ap;
	char *txt;

	if( !wid )
		return;

	if( !fmt )
		fmt = "";

	va_start( ap, fmt );
	txt = g_strdup_vprintf( fmt, ap );
	va_end( ap );

	if( !our_tooltips ) 
		our_tooltips = gtk_tooltips_new();

	gtk_tooltips_set_tip( our_tooltips, wid, txt, NULL );

	if( !GTK_WIDGET_REALIZED( wid ) )
		gtk_signal_connect( GTK_OBJECT( wid ), "realize",
			GTK_SIGNAL_FUNC( set_tooltip_events ), NULL );
	else
		set_tooltip_events( wid );

	g_free( txt );
}

/* Track tooltips we generate with one of these.
 */
typedef struct _TooltipGenerate {
	GtkWidget *widget;

	TooltipGenerateFn generate;
	void *a;
	void *b;

	VipsBuf buf;
	char txt[256];
} TooltipGenerate;

static void
tooltip_generate_free( GtkWidget *widget, TooltipGenerate *gen )
{
	gen->widget = NULL;
	gen->generate = NULL;
	gen->a = NULL;
	gen->b = NULL;

	IM_FREE( gen );
}

static gboolean
tooltip_generate_rebuild( GtkWidget *widget, 
	GdkEventCrossing *event, TooltipGenerate *gen )
{
	gboolean handled = FALSE;

	if( gen->widget ) {
		vips_buf_rewind( &gen->buf );
		gen->generate( widget, &gen->buf, gen->a, gen->b );
		set_tooltip( gen->widget, "%s", vips_buf_all( &gen->buf ) );
	}

	return( handled );
}

static void
tooltip_generate_attach( GtkWidget *widget, TooltipGenerate *gen )
{
	/* Must have enter/leave.
	 */
	gtk_widget_add_events( widget, 
		GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK );

	/* On enter, regenerate the tooltip.
	 */
	g_signal_connect( widget, "enter_notify_event",
		G_CALLBACK( tooltip_generate_rebuild ), gen );
}

/* Set a callback to be used to generate the tooltip.
 */
void
set_tooltip_generate( GtkWidget *widget, 
	TooltipGenerateFn generate, void *a, void *b )
{
	TooltipGenerate *gen;

	if( !(gen = INEW( NULL, TooltipGenerate )) )
		return;

	gen->widget = widget;
	gen->generate = generate;
	gen->a = a;
	gen->b = b;
	vips_buf_init_static( &gen->buf, gen->txt, 256 );
	g_signal_connect( widget, "destroy", 
		G_CALLBACK( tooltip_generate_free ), gen );

	if( !GTK_WIDGET_REALIZED( widget ) )
		g_signal_connect( widget, "realize",
			G_CALLBACK( tooltip_generate_attach ), gen );
	else
		tooltip_generate_attach( widget, gen );
}

/* Junk all tooltips, helps trim valgrind noise.
 */
void
junk_tooltips( void )
{
	if( our_tooltips )
		g_object_ref_sink( GTK_OBJECT( our_tooltips ) );
}

/* Set a GtkEditable.
 */
void
set_gentryv( GtkWidget *edit, const char *fmt, va_list ap )
{
	char buf[1000];
	gint position;
	int i;
	int len;

	if( !edit )
		return;

	if( !fmt )
		fmt = "";

	(void) im_vsnprintf( buf, 1000, fmt, ap );

	/* Filter out /n and /t ... they confuse gtkentry terribly
	 */
	len = strlen( buf );
	for( i = 0; i < len; i++ )
		if( buf[i] == '\n' || buf[i] == '\t' )
			buf[i] = ' ';

	gtk_editable_delete_text( GTK_EDITABLE( edit ), 0, -1 );
	position = 0;
	gtk_editable_insert_text( GTK_EDITABLE( edit ), 
		buf, strlen( buf ), &position );
}

/* Set a GtkEditable.
 */
void
set_gentry( GtkWidget *edit, const char *fmt, ... )
{
	va_list ap;

	va_start( ap, fmt );
	set_gentryv( edit, fmt, ap );
	va_end( ap );
}

void
set_glabel( GtkWidget *label, const char *fmt, ... )
{
	va_list ap;
	char buf[1000];

	va_start( ap, fmt );
	(void) im_vsnprintf( buf, 1000, fmt, ap );
	va_end( ap );

	gtk_label_set_text( GTK_LABEL( label ), buf );
}

/* Like set_glabel(), but don't display multi-line strings (just display the
 * first line).
 */
void
set_glabel1( GtkWidget *label, const char *fmt, ... )
{
	va_list ap;
	char txt[1000];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	va_start( ap, fmt );
	vips_buf_vappendf( &buf, fmt, ap );
	va_end( ap );

	gtk_label_set_text( GTK_LABEL( label ), vips_buf_firstline( &buf ) );
}

/* Like set_glabel, but do it caption-style.
 */
void 
set_gcaption( GtkWidget *label, const char *fmt, ... )
{
	va_list ap;
	char buf1[1000];
	char buf2[1000];

	va_start( ap, fmt );
	(void) im_vsnprintf( buf1, 1000, fmt, ap );
	va_end( ap );

	escape_markup( buf1, buf2, 1000 );
	(void) im_snprintf( buf1, 1000, 
		"<span size=\"smaller\">%s</span>", buf2 );
	gtk_label_set_markup( GTK_LABEL( label ), buf1 );
}

gboolean
get_geditable_name( GtkWidget *text, char *out, int sz )
{
	char *name;
	char *tname;

	name = gtk_editable_get_chars( GTK_EDITABLE( text ), 0, -1 );
	tname = trim_nonalpha( name );
	if( !tname ) {
		IM_FREEF( g_free, name );
		error_top( _( "Bad identifier." ) );
		error_sub( 
			_( "Enter an identifier. Identifiers start with "
			"a letter, and then contain only letters, numbers, "
			"apostrophy and underscore." ) );
		return( FALSE );
	}
	im_strncpy( out, tname, sz );
	g_free( name );

	return( TRUE );
}

gboolean
get_geditable_string( GtkWidget *text, char *out, int sz )
{
	char *str;

	str = gtk_editable_get_chars( GTK_EDITABLE( text ), 0, -1 );
	im_strncpy( out, str, sz );
	g_free( str );

	return( TRUE );
}

gboolean
get_geditable_filename( GtkWidget *text, char *out, int sz )
{
	char *filename;
	char *tfilename;

	filename = gtk_editable_get_chars( GTK_EDITABLE( text ), 0, -1 );
	tfilename = trim_white( filename );
	if( !is_valid_filename( tfilename ) ) {
		g_free( filename );
		return( FALSE );
	}
	im_strncpy( out, tfilename, sz );
	g_free( filename );

	return( TRUE );
}

/* Get a geditable as a double.
 */
gboolean
get_geditable_double( GtkWidget *text, double *out )
{
	char *txt;
	char *end;
	double t;

	txt = gtk_editable_get_chars( GTK_EDITABLE( text ), 0, -1 );
	t = strtod( txt, &end );
	if( end == txt ) {
		error_top( _( "Bad floating point number." ) );
		error_sub( _( "\"%s\" is not a floating point number." ), txt );
		g_free( txt );

		return( FALSE );
	}

	if( strspn( end, WHITESPACE ) != strlen( end ) ) {
		error_top( _( "Bad floating point number." ) );
		error_sub( _( "Extra characters \"%s\" after number." ), end );
		g_free( txt );

		return( FALSE );
	}
	g_free( txt );

	*out = t;

	return( TRUE );
}

/* Get as int.
 */
gboolean
get_geditable_int( GtkWidget *text, int *n )
{
	int i;
	char *txt;

	/* Parse values.
	 */
	txt = gtk_editable_get_chars( GTK_EDITABLE( text ), 0, -1 );
	if( sscanf( txt, "%i", &i ) != 1 ) {
		error_top( _( "Bad integer." ) );
		error_sub( _( "\"%s\" is not an integer." ), txt );
		g_free( txt );
		return( FALSE );
	}
	g_free( txt );
	*n = i;

	return( TRUE );
}

/* Get as unsigned int.
 */
gboolean
get_geditable_uint( GtkWidget *text, int *n )
{
	int i;

	if( !get_geditable_int( text, &i ) || i < 0 ) {
		error_top( _( "Bad unsigned integer." ) );
		return( FALSE );
	}
	*n = i;

	return( TRUE );
}

/* Get as positive int.
 */
gboolean
get_geditable_pint( GtkWidget *text, int *n )
{
	int i;

	if( !get_geditable_int( text, &i ) || i <= 0 ) {
		error_top( _( "Bad positive integer." ) );
		return( FALSE );
	}
	*n = i;

	return( TRUE );
}

/* Indent widget, label above.
 */
GtkWidget *
build_glabelframe2( GtkWidget *widget, const char *name )
{
	GtkWidget *lab;
	GtkWidget *vb;
	GtkWidget *hb;
	GtkWidget *inv;
	char buf[1000];

        hb = gtk_hbox_new( FALSE, 2 );
        inv = gtk_label_new( "" );
        gtk_box_pack_start( GTK_BOX( hb ), inv, FALSE, FALSE, 15 );
        gtk_box_pack_start( GTK_BOX( hb ), widget, TRUE, TRUE, 0 );

        vb = gtk_vbox_new( FALSE, 2 );
	im_snprintf( buf, 1000, _( "%s:" ), name );
	lab = gtk_label_new( buf );
	gtk_misc_set_alignment( GTK_MISC( lab ), 0.0, 0.5 );
        gtk_box_pack_start( GTK_BOX( vb ), lab, FALSE, FALSE, 0 );
        gtk_box_pack_start( GTK_BOX( vb ), hb, TRUE, TRUE, 0 );

	return( vb );
}

/* Make a text field + label. Indent the text on a new line.
 */
GtkWidget *
build_glabeltext3( GtkWidget *box, const char *label )
{	
	GtkWidget *txt;
	GtkWidget *vb;

        txt = gtk_entry_new();
        vb = build_glabelframe2( txt, label );
        gtk_box_pack_start( GTK_BOX( box ), vb, FALSE, FALSE, 0 );

	return( txt );
}

/* Make text field plus label .. use a sizegroup for alignment.
 */
GtkWidget *
build_glabeltext4( GtkWidget *box, GtkSizeGroup *group, const char *text )
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *entry;
	char buf[256];

	hbox = gtk_hbox_new( FALSE, 12 );
	im_snprintf( buf, 256, _( "%s:" ), text );
	label = gtk_label_new( buf );
	gtk_misc_set_alignment( GTK_MISC( label ), 0.0, 0.5 );
	if( group )
		gtk_size_group_add_widget( group, label );  
	gtk_box_pack_start( GTK_BOX( hbox ), label, FALSE, FALSE, 0 );

	entry = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( hbox ), entry, TRUE, TRUE, 0 );

	gtk_box_pack_start( GTK_BOX( box ), hbox, FALSE, FALSE, 0 );

	gtk_widget_show_all( hbox );

	return( entry );
}

/* Make a labeled toggle.
 */
GtkWidget *
build_gtoggle( GtkWidget *box, const char *caption )
{
        GtkWidget *hb;
        GtkWidget *inv;
	GtkWidget *toggle;

	/* Indent left a bit.
	 */
        inv = gtk_label_new( "" );
        hb = gtk_hbox_new( FALSE, 0 );
        gtk_box_pack_start( GTK_BOX( hb ), inv, FALSE, FALSE, 2 );
        toggle = gtk_check_button_new_with_label( caption );
	gtk_container_set_border_width( GTK_CONTAINER( toggle ), 4 );
        gtk_box_pack_start( GTK_BOX( hb ), toggle, TRUE, TRUE, 0 );

        gtk_box_pack_start( GTK_BOX( box ), hb, FALSE, FALSE, 0 );

	return( toggle );
}

/* Make a label plus option menu.
 */
GtkWidget *
build_goption( GtkWidget *box, GtkSizeGroup *group, 
	const char *name, const char *item_names[], int nitem,
	GtkSignalFunc fn, void *value )
{
	GtkWidget *hb;
	GtkWidget *label;
	GtkWidget *om;
	int i;
	char buf[1000];

        hb = gtk_hbox_new( FALSE, 12 );
	im_snprintf( buf, 1000, _( "%s:" ), name );
        label = gtk_label_new( buf );
	if( group )
		gtk_size_group_add_widget( group, label );  
        gtk_box_pack_start( GTK_BOX( hb ), label, FALSE, TRUE, 0 );

	om = gtk_combo_box_new_text();
        gtk_box_pack_start( GTK_BOX( hb ), om, FALSE, TRUE, 0 );
        set_tooltip( om, _( "Left-click to change value" ) );

	for( i = 0; i < nitem; i++ ) 
		gtk_combo_box_append_text( GTK_COMBO_BOX( om ),
			 _( item_names[i] ) );
	if( fn ) 
		gtk_signal_connect( GTK_OBJECT( om ), "changed", fn, value );
        gtk_box_pack_start( GTK_BOX( box ), hb, FALSE, TRUE, 0 );
        gtk_widget_show_all( hb );

	return( om );
}

/* Register a widget as a filename drag receiver.
 */
typedef struct {
	GtkWidget *widget;
	FiledropFunc fn;
	void *client;
} FiledropInfo;

static gboolean
filedrop_trigger( FiledropInfo *fdi, const char *path )
{
	char buf[FILENAME_MAX];
	gboolean result;

	im_strncpy( buf, path, FILENAME_MAX );
	path_compact( buf );
	result = fdi->fn( fdi->client, buf );

	return( result );
}

static void
filedrop_drag_data_received( GtkWidget *widget, 
	GdkDragContext *context,
	gint x, gint y, GtkSelectionData *data, guint info, guint time,
	FiledropInfo *fdi )
{
	gchar *sPath = NULL;
	gchar *pFrom, *pTo; 
	gboolean result;

        pFrom = strstr( (char *) data->data, "file:" );

        while( pFrom ) {
#if !GLIB_CHECK_VERSION (2,0,0)
		pFrom += 5; /* remove 'file:' */
#else
		GError *error = NULL;
#endif

		pTo = pFrom;
		while( *pTo != 0 && *pTo != 0xd && *pTo != 0xa ) 
			pTo += 1;

		sPath = g_strndup( pFrom, pTo - pFrom );

#if !GLIB_CHECK_VERSION (2,0,0)
		result = filedrop_trigger( fdi, sPath );
#else
		/* format changed with Gtk+1.3, use conversion 
		 */
		pFrom = g_filename_from_uri( sPath, NULL, &error );
		result = filedrop_trigger( fdi, pFrom );
		g_free( pFrom );
#endif

		g_free( sPath );

		if( !result )
			iwindow_alert( fdi->widget, GTK_MESSAGE_ERROR );

		pFrom = strstr( pTo, "file:" );
        } 

        gtk_drag_finish( context, TRUE, FALSE, time );
}

/* HB: file dnd stuff lent by The Gimp via Dia, not fully understood 
 * but working ...
 */
enum {
	TARGET_URI_LIST,
	TARGET_TEXT_PLAIN
};

static void
filedrop_destroy( GtkWidget *widget, FiledropInfo *fdi )
{
	im_free( fdi );
}

void
filedrop_register( GtkWidget *widget, FiledropFunc fn, void *client )
{
	static GtkTargetEntry target_table[] = {
		{ "text/uri-list", 0, TARGET_URI_LIST },
		{ "text/plain", 0, TARGET_TEXT_PLAIN }
	};

	FiledropInfo *fdi = INEW( NULL, FiledropInfo );

	fdi->widget = widget;
	fdi->fn = fn;
	fdi->client = client;
	gtk_signal_connect( GTK_OBJECT( widget ), "destroy",
                GTK_SIGNAL_FUNC( filedrop_destroy ), fdi );

	gtk_drag_dest_set( GTK_WIDGET( widget ),
		GTK_DEST_DEFAULT_ALL,
		target_table, IM_NUMBER( target_table ),
		GDK_ACTION_COPY |
		/* That's all you need to get draggable URIs in GNOME and
		 * win32, but KDE needs these other flags too, apparently.
		 */
		GDK_ACTION_MOVE | GDK_ACTION_LINK | GDK_ACTION_ASK );
	gtk_signal_connect( GTK_OBJECT( widget ), "drag_data_received",
		GTK_SIGNAL_FUNC( filedrop_drag_data_received ), fdi );
}

/* Add symbol drag to the target list.
 */
void
set_symbol_drag_type( GtkWidget *widget )
{
	static const GtkTargetEntry targets[] = {
		{ "text/symbol", 0, TARGET_SYMBOL }
	};

	GtkTargetList *target_list;

	if( !GTK_WIDGET_REALIZED( widget ) ) 
		return;

	/* We can't always set the dest types, since we're probably already a
	 * filedrop. Just add to the target list.
	 */
	if( (target_list = 
		gtk_drag_dest_get_target_list( widget )) )
		gtk_target_list_add_table( target_list, 
			targets, IM_NUMBER( targets ) );
	else
		gtk_drag_dest_set( widget,
			GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_MOTION | 
				GTK_DEST_DEFAULT_DROP,
			targets, IM_NUMBER( targets ),
			GDK_ACTION_COPY );

	gtk_drag_source_set( widget,
		GDK_BUTTON1_MASK | GDK_BUTTON3_MASK,
		targets, IM_NUMBER( targets ),
		GDK_ACTION_COPY | GDK_ACTION_MOVE );
}

typedef struct _Listen {
	GObject *gobject;		/* This object */
	GObject *source;		/* Listens for signals from this */
	GObject **zap;			/* NULL this on destroy */
	const char *name;		/* Signal name */
	GCallback gcallback;		/* Call this handler */

	guint name_sid;
	guint gobject_destroy_sid;
	guint source_destroy_sid;
} Listen;

static void
listen_gobject_destroy_cb( GObject *gobject, Listen *listen )
{
	/* gobject has gone ... source should no longer send us signals.
	 */
	FREESID( listen->name_sid, listen->source );
	FREESID( listen->source_destroy_sid, listen->source );

	g_free( listen );
}

static void
listen_source_destroy_cb( GObject *gobject, Listen *listen )
{
	/* Source has gone, these signals have been destroyed.
	 */
	listen->name_sid = 0;
	listen->source_destroy_sid = 0;

	/* Link broken, no need to auto-free us on gobject destroy.
	 */
	FREESID( listen->gobject_destroy_sid, listen->gobject );

	/* Zap gobject member pointer to source.
	 */
	if( listen->zap ) {
		g_assert( !*(listen->zap) || 
			*(listen->zap) == listen->source );

		*(listen->zap) = NULL;
	}

	g_free( listen );
}

void
listen_add( GObject *gobject, GObject **zap, 
	const char *name, GCallback gcallback )
{
	Listen *listen = g_new( Listen, 1 );

	listen->gobject = gobject;
	listen->source = *zap;
	listen->zap = zap;
	listen->name = name;
	listen->gcallback = gcallback;

	listen->name_sid = g_signal_connect( listen->source, 
		listen->name, listen->gcallback, listen->gobject );
	listen->source_destroy_sid = g_signal_connect( listen->source, 
		"destroy",
		G_CALLBACK( listen_source_destroy_cb ), listen );
	listen->gobject_destroy_sid = g_signal_connect( gobject, "destroy",
		G_CALLBACK( listen_gobject_destroy_cb ), listen );
}

void
widget_update_pointer( GtkWidget *widget, GdkEvent *ev )
{
	if( ev->type == GDK_MOTION_NOTIFY && ev->motion.is_hint ) {
		GdkDisplay *display = gtk_widget_get_display( widget );
		GdkScreen *screen;
		int x_root, y_root;

		gdk_display_get_pointer( display, 
			&screen, &x_root, &y_root, NULL );
		ev->motion.x_root = x_root;
		ev->motion.y_root = y_root; 
	}
}

void *
gobject_print( GObject *gobject )
{
	printf( "%s (%p)\n", G_OBJECT_TYPE_NAME( gobject ), gobject );

	return( NULL );
}

/* Get the default DPI.
 */
int
get_dpi( void )
{
	GdkScreen *screen = gdk_screen_get_default();

	if( screen ) {
		int width_pixels = gdk_screen_get_width( screen );
		int width_mm = gdk_screen_get_width_mm( screen );

		return( width_pixels / (width_mm / 25.4) );
	}
	else
		return( 72 );
}

GtkWidget *
image_new_from_file( const char *name )
{
	GtkWidget *image;
	char *file;

	if( (file = path_find_file( name )) ) {
		image = (GtkWidget *) callv_string_filename( 
			(callv_string_fn) gtk_image_new_from_file, 
			file, NULL, NULL, NULL );
		im_free( file );
	}
	else
		/* We get a broken image icon if this fails.
		 */
		image = gtk_image_new_from_file( name );

	return( image );
}

void
vfatal( GError **error )
{
	fprintf( stderr, PACKAGE ": fatal error\n" );

	if( *error ) {
		fprintf( stderr, "%s\n", (*error)->message );
		IM_FREEF( g_error_free, *error );
	}

	exit( -1 );
} 

char *
text_view_get_text( GtkTextView *text_view )
{ 
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	GtkTextIter start_iter;
	GtkTextIter end_iter;
	char *text;

	gtk_text_buffer_get_start_iter( text_buffer, &start_iter );
	gtk_text_buffer_get_end_iter( text_buffer, &end_iter );
	text = gtk_text_buffer_get_text( text_buffer, 
		&start_iter, &end_iter, FALSE ); 

	return( text );
}

void
text_view_set_text( GtkTextView *text_view, 
	const char *text, gboolean editable )
{
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );

	gtk_text_buffer_set_text( text_buffer, text ? text : "", -1 );

	gtk_text_view_set_editable( text_view, editable );
	gtk_text_view_set_cursor_visible( text_view, editable );
}

void
text_view_select_text( GtkTextView *text_view, int start, int end )
{
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	GtkTextMark *mark = gtk_text_buffer_get_insert( text_buffer );
	GtkTextIter start_iter;
	GtkTextIter end_iter;

	gtk_text_buffer_get_iter_at_offset( text_buffer, &start_iter, start );
	gtk_text_buffer_get_iter_at_offset( text_buffer, &end_iter, end );
	gtk_text_buffer_select_range( text_buffer, &start_iter, &end_iter );
	gtk_text_view_scroll_mark_onscreen( text_view, mark );
}

/* If parent dies, kill us too. Parent can be anything, but child must be an
 * iobject.
 */
typedef struct _DestroyIfDestroyed {
	GObject *child;
	GObject *parent;
	DestroyFn destroy_fn;
} DestroyIfDestroyed;

static void destroy_if_destroyed_parent_cb( DestroyIfDestroyed *difd, 
	GObject *parent );
static void destroy_if_destroyed_child_cb( DestroyIfDestroyed *difd, 
	GObject *child );

static void
destroy_if_destroyed_parent_cb( DestroyIfDestroyed *difd, GObject *parent )
{
	GObject *child;
	DestroyFn destroy_fn;

#ifdef DEBUG
	printf( "destroy_if_destroyed_parent_cb: %p\n", difd );
#endif /*DEBUG*/

	/* Destroying the child will trigger the other half of difd, make sure
	 * we remove the link first.
	 */
	child = difd->child;
	destroy_fn = difd->destroy_fn;
	g_object_weak_unref( difd->child, 
		(GWeakNotify) destroy_if_destroyed_child_cb, difd );
	destroy_fn( child );

	difd->child = NULL;
	difd->parent = NULL;
	difd->destroy_fn = NULL;
	g_free( difd );
}

static void
destroy_if_destroyed_child_cb( DestroyIfDestroyed *difd, GObject *child )
{
#ifdef DEBUG
	printf( "destroy_if_destroyed_child_cb: %p\n", difd );
#endif /*DEBUG*/

	g_object_weak_unref( difd->parent, 
		(GWeakNotify) destroy_if_destroyed_parent_cb, difd );

	difd->child = NULL;
	difd->parent = NULL;
	difd->destroy_fn = NULL;
	g_free( difd );
}

void
destroy_if_destroyed( GObject *child, GObject *parent, DestroyFn destroy_fn )
{
	DestroyIfDestroyed *difd = g_new( DestroyIfDestroyed, 1 );

#ifdef DEBUG
	printf( "destroy_if_destroyed %p: parent=%p, child=%p\n", 
		difd, parent, child );
#endif /*DEBUG*/

	difd->child = child;
	difd->parent = parent;
	difd->destroy_fn = destroy_fn;

	g_object_weak_ref( parent, 
		(GWeakNotify) destroy_if_destroyed_parent_cb, difd );
	g_object_weak_ref( child, 
		(GWeakNotify) destroy_if_destroyed_child_cb, difd );
}

/* A 'safe' way to run a few events.
 */
void
process_events( void )
{
	/* Max events we process before signalling a timeout. Without this we
	 * can get stuck in event loops in some circumstances.
	 */
	static const int max_events = 100;

	/* Block too much recursion. 0 is from the top-level, 1 is from a
	 * callback, we don't want any more than that.
	 */
	if( g_main_depth() < 2 ) {
		int n;

#ifdef DEBUG
		printf( "progress_update: starting event dispatch\n" );
#endif /*DEBUG*/

		for( n = 0; n < max_events && 
			g_main_context_iteration( NULL, FALSE ); n++ )
			;

#ifdef DEBUG
		printf( "progress_update: event dispatch done\n" );
		if( n == max_events )
			printf( "progress_update: event dispatch timeout\n" );
#endif /*DEBUG*/
	}
}
