/* Declarations supporting gtkutil.c
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

/* Look up an object's parent class dynamically.
 */
#define PARENT_CLASS_DYNAMIC( OBJECT ) \
	(g_type_class_peek( \
		g_type_parent( \
			G_TYPE_FROM_INSTANCE( OBJECT ) ) ))

/* Like G_CHECK_TYPE, but insist on an exact match.
 */
#define TYPE_EXACT( OBJECT, TYPE ) \
	(G_TYPE_FROM_INSTANCE( OBJECT ) == (TYPE))

#define DESTROY_GTK( X ) { \
	if( X ) { \
		gtk_object_destroy( GTK_OBJECT( X ) ); \
		(X) = NULL; \
	} \
}

void adjustments_set_value( GtkAdjustment *hadj, GtkAdjustment *vadj,
        float hval, float vval );

void *object_destroy( void *obj );
void *null_g_free( void *obj );
const char *object_type_name( GtkObject *obj );

void widget_visible( GtkWidget *widget, gboolean visible );

/* Make widgets.
 */
GtkWidget *build_button( const char *name, GtkSignalFunc cb, gpointer user );
void get_geo( GtkWidget *widget, const char *text, Rect *geo );
void set_fixed( GtkWidget *widget, int nchars );
GtkWidget *build_entry( int nchars );

GtkWidget *menu_build( const char *name );
GtkWidget *menu_add_but( GtkWidget *menu, 
	const char *name, GtkSignalFunc cb, void *user );
GtkWidget *menu_add_tog( GtkWidget *menu, 
	const char *name, GtkSignalFunc cb, void *user );
GtkWidget *menu_add_sep( GtkWidget *menu );
GtkWidget *menu_add_pullright( GtkWidget *popup, const char *name );

/* Popup menu handling.
 */
typedef void (*PopupFunc)( GtkWidget *, GtkWidget *, void * );
#define POPUP_FUNC( fn ) ((PopupFunc) (fn))

GtkWidget *popup_build( const char *name );
GtkWidget *popup_add_but( GtkWidget *, const char *, PopupFunc );
GtkWidget *popup_add_tog( GtkWidget *, const char *, PopupFunc );
GtkWidget *popup_add_pullright( GtkWidget *popup, const char *name );
void popup_show( GtkWidget *host, GdkEvent *ev );
void popup_link( GtkWidget *host, GtkWidget *popup, void *data );
guint popup_attach( GtkWidget *host, GtkWidget *popup, void *data );
void popup_detach( GtkWidget *host, guint sid );

void set_tooltip( GtkWidget *wid, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
void junk_tooltips( void );

typedef void (*TooltipGenerateFn)( GtkWidget *, VipsBuf *, void *a, void *b );
void set_tooltip_generate( GtkWidget *widget, 
	TooltipGenerateFn fn, void *a, void *b );

/* Set/get a label/entry, printf style.
 */
void set_gentryv( GtkWidget *edit, const char *fmt, va_list ap );
void set_gentry( GtkWidget *entry, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
void set_glabel( GtkWidget *label, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
void set_glabel1( GtkWidget *label, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
void set_gcaption( GtkWidget *label, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
gboolean get_geditable_string( GtkWidget *text, char *out, int sz );
gboolean get_geditable_name( GtkWidget *text, char *out, int sz );
gboolean get_geditable_filename( GtkWidget *text, char *out, int sz );
gboolean get_geditable_double( GtkWidget *text, double *out );
gboolean get_geditable_int( GtkWidget *text, int *n );
gboolean get_geditable_uint( GtkWidget *text, int *n );
gboolean get_geditable_pint( GtkWidget *text, int *n );

/* Make widget groups.
 */
GtkWidget *build_glabelframe2( GtkWidget *box, const char *label );
GtkWidget *build_glabeltext3( GtkWidget *box, const char *label );
GtkWidget *build_glabeltext4( GtkWidget *box, GtkSizeGroup *group, 
	const char *label );
GtkWidget *build_gtoggle( GtkWidget *box, const char *caption );
GtkWidget *build_goption( GtkWidget *box, GtkSizeGroup *group, 
	const char *name, const char *item_names[], int nitem,
	GtkSignalFunc fn, void *value );

typedef gboolean (*FiledropFunc)( void *client, const char *file );
void filedrop_register( GtkWidget *widget, FiledropFunc fn, void *client );

/* Tag our thumbnail drag-n-drops with these. Start up a bit to leave room for 
 * filedrop.
 */
enum {
	TARGET_SYMBOL = 99
};

void set_symbol_drag_type( GtkWidget *widget );

void listen_add( GObject *gobject, GObject **zap, 
	const char *name, GCallback gcallback );

void widget_update_pointer( GtkWidget *widget, GdkEvent *ev );

void *gobject_print( GObject *gobject );

int get_dpi( void );

GtkWidget *image_new_from_file( const char *name );

void vfatal( GError **error );

char *text_view_get_text( GtkTextView *text_view );
void text_view_set_text( GtkTextView *text_view, 
	const char *text, gboolean editable );
void text_view_select_text( GtkTextView *text_view, int start, int end );

typedef void (*DestroyFn)( GObject * );
void destroy_if_destroyed( GObject *child, GObject *parent, 
	DestroyFn destroy_fn );
