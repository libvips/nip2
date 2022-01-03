#include "nip.h"

/*
#define DEBUG_VERBOSE
#define DEBUG
#define DEBUG_RENDER_TIME
 */

static void
app_init( App *app )
{
}

static void
app_activate( GApplication *app )
{
        Main *main;

        main = main_new( APP( app ) );
        gtk_window_present( GTK_WINDOW( main ) );
}

static void
app_quit_activated( GSimpleAction *action, GVariant *parameter, gpointer app )
{
        g_application_quit( G_APPLICATION( app ) );
}

static void
app_new_activated( GSimpleAction *action,
        GVariant *parameter, gpointer user_data )
{
        app_activate( G_APPLICATION( user_data ) ); 
}

static Main *
app_win( App *app )
{
        GList *windows = gtk_application_get_windows( GTK_APPLICATION( app ) );

        if( windows )
                return( MAIN( windows->data ) );
        else 
                return( NULL ); 
}

static void
app_about_activated( GSimpleAction *action,
        GVariant *parameter, gpointer user_data )
{
        App *app = APP( user_data );
        ImageWindow *win = app_win( app );

        static const char *authors[] = {
                "John Cupitt",
                NULL
        };

#ifdef DEBUG
        printf( "app_about_activated:\n" );
#endif /*DEBUG*/

        gtk_show_about_dialog( win ? GTK_WINDOW( win ) : NULL, 
                "program-name", "nip4",
                "logo-icon-name", "org.libvips.nip4",
                "title", _( "About nip4" ),
                "authors", authors,
                "version", "v1.0",
                "comments", _( "An image-processing spreadsheet" ),
                "license-type", GTK_LICENSE_MIT_X11,
                "website-label", "Visit nip4 on github",
                "website", "https://github.com/libvips/nip4",
                NULL );
}

static GActionEntry app_entries[] =
{
        { "quit", app_quit_activated },
        { "new", app_new_activated },
        { "about", app_about_activated },
};

static void
app_startup( GApplication *app )
{
        int i;
        GtkSettings *settings;

        struct {
                const gchar *action_and_target;
                const gchar *accelerators[2];
        } accels[] = {
                { "app.quit", { "<Primary>q", NULL } },
                { "app.new", { "<Primary>n", NULL } },

                { "win.duplicate", { "<Primary>d", NULL } },
                { "win.close", { "<Primary>w", NULL } },
                { "win.replace", { "<Primary>o", NULL } },
                { "win.prev", { "<primary>comma", NULL } },
                { "win.next", { "<primary>period", NULL } },
                { "win.fullscreen", { "F11", NULL } },
        };

        G_APPLICATION_CLASS( app_parent_class )->startup( app );

        /* Image display programs are supposed to default to a dark theme,
         * according to the HIG.
         */
        settings = gtk_settings_get_default(); 
        g_object_set( settings, 
                "gtk-application-prefer-dark-theme", TRUE,
                NULL );

        /* Build our classes.
        IMAGEDISPLAY_TYPE;
        DISPLAYBAR_TYPE;
        TSLIDER_TYPE;
        INFOBAR_TYPE;
         */

        g_action_map_add_action_entries( G_ACTION_MAP( app ),
                app_entries, G_N_ELEMENTS( app_entries ),
                app );

        for( i = 0; i < G_N_ELEMENTS( accels ); i++)
                gtk_application_set_accels_for_action( GTK_APPLICATION( app ),
                        accels[i].action_and_target, accels[i].accelerators );
}

static void
app_open( GApplication *app, 
        GFile **files, int n_files, const char *hint )
{
        int i;

        for( i = 0; i < n_files; i++ ) {
                Main *main = main_new( APP( app ) );

                main_open( main, files[i] );
                gtk_window_present( GTK_WINDOW( main ) );
        }
}

static void
app_shutdown( GApplication *app )
{
        Main *main; 

#ifdef DEBUG
        printf( "app_shutdown:\n" );
#endif /*DEBUG*/

        /* Force down all our windows ... this will not happen automatically
         * on _quit().
         */
        while( (main = app_win( APP( app ) )) ) 
                gtk_window_destroy( GTK_WINDOW( main ) );

        G_APPLICATION_CLASS( app_parent_class )->shutdown( app );
}

static void
app_class_init( AppClass *class )
{
        G_APPLICATION_CLASS( class )->startup = app_startup;
        G_APPLICATION_CLASS( class )->activate = app_activate;
        G_APPLICATION_CLASS( class )->open = app_open;
        G_APPLICATION_CLASS( class )->shutdown = app_shutdown;
}

App *
app_new( void )
{
        return( g_object_new( APP_TYPE, 
                "application-id", APP_ID,
                "flags", G_APPLICATION_HANDLES_OPEN,
                "inactivity-timeout", 3000,
                "register-session", TRUE,
                NULL ) );
}
