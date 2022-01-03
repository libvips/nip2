#include "nip.h"

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

        (void) vips_vsnprintf( buf, 1000, fmt, ap );

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
                g_free( txt );
                return( FALSE );
        }
        if( strspn( end, WHITESPACE ) != strlen( end ) ) {
                g_free( txt );
                return( FALSE );
        }
        g_free( txt );

        *out = t;

        return( TRUE );
}

/* Build a GtkEntry, with a widget width specified in characters.
 */
GtkWidget *
build_entry( int nchars )
{
        GtkWidget *entry;

        entry = gtk_entry_new();
        // FIXME 
        // gtk_entry_set_width_chars( GTK_ENTRY( entry ), nchars );

        return( entry );
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

        gtk_widget_set_tooltip_text( wid, txt );

        g_free( txt );
}

void
copy_adj( GtkAdjustment *to, GtkAdjustment *from )
{
        double value = gtk_adjustment_get_value( from );
        double lower = gtk_adjustment_get_lower( from );
        double upper = gtk_adjustment_get_upper( from );
        double step_increment = gtk_adjustment_get_step_increment( from );
        double page_increment = gtk_adjustment_get_page_increment( from );
        double page_size = gtk_adjustment_get_page_size( from );

        gtk_adjustment_configure( to, value, 
                lower, upper, 
                step_increment, page_increment, page_size );
}

void
change_state( GtkWidget *widget, const char *name, GVariant *state )
{
        GAction *action;

        action = g_action_map_lookup_action( G_ACTION_MAP( widget ), name );
        if( action )
                g_action_change_state( action, state );
}

GVariant *
get_state( GtkWidget *widget, const char *name )
{
        GAction *action;

        action = g_action_map_lookup_action( G_ACTION_MAP( widget ), name );
        if( !action ) 
                return( NULL );

        return( g_action_get_state( action ) );
}

void
copy_state( GtkWidget *to, GtkWidget *from, const char *name )
{
        GVariant *state;

        if( (state = get_state( from, name )) ) {
                change_state( to, name, state );
                g_variant_unref( state );
        }
}
