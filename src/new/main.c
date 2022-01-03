/*
#define DEBUG
 */

#include "nip.h"

/* How much to scale view by each frame.
 */
#define SCALE_STEP (2.0)

struct _Main
{
        GtkApplicationWindow parent;

        GtkWidget *title;
        GtkWidget *subtitle;
        GtkWidget *gears;

	GtkWidget *progress_bar;
        GtkWidget *progress;
        GtkWidget *progress_cancel;

        GtkWidget *error_bar;
        GtkWidget *error_label;

        GtkWidget *scrolled_maindow;

        /* Throttle progress bar updates to a few per second with this.
         */
        GTimer *progress_timer;
        double last_progress_time;

	GSettings *settings;
};

G_DEFINE_TYPE( Main, main, GTK_TYPE_APPLICATION_WINDOW );

/* Our signals. 
 */
enum {
        SIG_LAST
};

static guint main_signals[SIG_LAST] = { 0 };

static void
main_dispose( GObject *object )
{
        Main *main = MAIN( object );

#ifdef DEBUG
        printf( "main_dispose:\n" ); 
#endif /*DEBUG*/

        VIPS_UNREF( main->settings );
        VIPS_FREEF( g_timer_destroy, main->progress_timer );

        G_OBJECT_CLASS( main_parent_class )->dispose( object );
}

static void
main_cancel_clicked( GtkWidget *button, Main *main )
{
}

static void
main_error( Main *main )
{
        char *err;
        int i;

        /* Remove any trailing \n.
         */
        err = vips_error_buffer_copy();
        for( i = strlen( err ); i > 0 && err[i - 1] == '\n'; i-- )
                err[i - 1] = '\0';
        gtk_label_set_text( GTK_LABEL( main->error_label ), err );
        g_free( err );

        gtk_info_bar_set_revealed( GTK_INFO_BAR( main->error_bar ), TRUE );
}

static void
main_error_hide( Main *main )
{
        gtk_info_bar_set_revealed( GTK_INFO_BAR( main->error_bar ), FALSE );
}

static void
main_error_response( GtkWidget *button, int response, Main *main )
{
        gtk_info_bar_set_revealed( GTK_INFO_BAR( main->error_bar ), FALSE );
}

static void
main_toggle_debug( Main *main )
{
	gboolean debug;

        g_object_get( main->imagedisplay, 
		"debug", &debug,
		NULL );

        g_object_set( main->imagedisplay, 
		"debug", !debug,
		NULL );
}

static void
main_duplicate_action( GSimpleAction *action, 
        GVariant *parameter, gpointer user_data )
{
        Main *main = MAIN( user_data );

        App *app;
        int width, height;

        g_object_get( main, "application", &app, NULL );
        new = main_new( app ); 
        gtk_maindow_present( GTK_WINDOW( new ) );

        gtk_maindow_get_default_size( GTK_WINDOW( main ), &width, &height );
        gtk_maindow_set_default_size( GTK_WINDOW( new ), width, height );

        /* falsecolour etc. are copied when we copy the tile_source. We
         * just copy the maindow state here.
         */
        copy_state( GTK_WIDGET( new ), GTK_WIDGET( main ), "info" );

        /* We want to init the scroll position, but we can't do that until the
         * adj range is set, and that won't happen until the image is loaded.
         *
         * Just copy the adj settings from the current maindow.
         */
        copy_adj( 
                gtk_scrolled_maindow_get_hadjustment( 
                        GTK_SCROLLED_WINDOW( new->scrolled_maindow ) ),
                gtk_scrolled_maindow_get_hadjustment( 
                        GTK_SCROLLED_WINDOW( main->scrolled_maindow ) ) );
        copy_adj( 
                gtk_scrolled_maindow_get_vadjustment( 
                        GTK_SCROLLED_WINDOW( new->scrolled_maindow ) ),
                gtk_scrolled_maindow_get_vadjustment( 
                        GTK_SCROLLED_WINDOW( main->scrolled_maindow ) ) );
}

static void
main_close_action( GSimpleAction *action, 
        GVariant *parameter, gpointer user_data )
{
        Main *main = MAIN( user_data );

        gtk_maindow_destroy( GTK_WINDOW( main ) );
}

static void
main_toggle( GSimpleAction *action, GVariant *parameter, gpointer user_data )
{
        GVariant *state;

        state = g_action_get_state( G_ACTION( action ) );
        g_action_change_state( G_ACTION( action ), 
                g_variant_new_boolean( !g_variant_get_boolean( state ) ) );
        g_variant_unref( state );
} 

static void
main_fullscreen( GSimpleAction *action, GVariant *state, gpointer user_data )
{
        Main *main = MAIN( user_data );

        g_object_set( main, 
                "fullscreened", g_variant_get_boolean( state ),
                NULL );

        g_simple_action_set_state( action, state );
}

static void
main_info( GSimpleAction *action, GVariant *state, gpointer user_data )
{
        Main *main = MAIN( user_data );

        g_object_set( main->info_bar,
                "revealed", g_variant_get_boolean( state ),
                NULL );

        g_simple_action_set_state( action, state );
}

static void
main_radio( GSimpleAction *action, GVariant *parameter, gpointer user_data )
{
        g_action_change_state( G_ACTION( action ), parameter );
}

static GActionEntry main_entries[] = {
        { "duplicate", main_duplicate_action },
        { "close", main_close_action },

        { "fullscreen", main_toggle, NULL, "false", main_fullscreen },
        { "info", main_toggle, NULL, "false", main_info },
};

static void
main_init( Main *main )
{
        GtkBuilder *builder;
        GMenuModel *menu;
        GtkEventController *controller;

        main->progress_timer = g_timer_new();
        main->last_progress_time = -1;
        main->settings = g_settings_new( APP_ID );

        gtk_widget_init_template( GTK_WIDGET( main ) );

        builder = gtk_builder_new_from_resource( 
                APP_PATH "/imagemaindow-menu.ui" );
        menu = G_MENU_MODEL( gtk_builder_get_object( builder, 
                "imagemaindow-menu" ) );
        gtk_menu_button_set_menu_model( GTK_MENU_BUTTON( main->gears ), menu );
        g_object_unref( builder );

        g_object_set( main->info_bar,
                "image-maindow", main,
                NULL );

	g_signal_connect_object( main->progress_cancel, "clicked",
                G_CALLBACK( main_cancel_clicked ), main, 0 );

        g_signal_connect_object( main->error_bar, "response", 
                G_CALLBACK( main_error_response ), main, 0 );

        g_action_map_add_action_entries( G_ACTION_MAP( main ),
                main_entries, G_N_ELEMENTS( main_entries ),
                main );

        g_settings_bind( main->settings, "info",
                G_OBJECT( main->info_bar ),
                "revealed", 
                G_SETTINGS_BIND_DEFAULT );

        /* Initial menu state from settings.
         */
        change_state( GTK_WIDGET( main ), "info", 
                g_settings_get_value( main->settings, "info" ) );

}

#define BIND( field ) \
        gtk_widget_class_bind_template_child( GTK_WIDGET_CLASS( class ), \
                Main, field );

static void
main_class_init( MainClass *class )
{
        G_OBJECT_CLASS( class )->dispose = main_dispose;

        gtk_widget_class_set_template_from_resource( GTK_WIDGET_CLASS( class ),
                APP_PATH "/imagemaindow.ui");

        BIND( title );
        BIND( subtitle );
        BIND( gears );
        BIND( progress_bar );
        BIND( progress );
        BIND( progress_cancel );
        BIND( error_bar );
        BIND( error_label );
        BIND( scrolled_maindow );
        BIND( info_bar );

}

Main *
main_new( VipsdispApp *app )
{
        return( g_object_new( IMAGE_WINDOW_TYPE, "application", app, NULL ) );
}

void
main_open( Main *main, GFile *file )
{
}

