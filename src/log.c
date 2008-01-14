/* Abstract base class for a log window: errors, link report, log, etc.
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

static iWindowClass *parent_class = NULL;

static void
log_build( GtkWidget *widget ) 
{
	Log *log = LOG( widget );
	iWindow *iwnd = IWINDOW( widget );

	GtkWidget *swin;
	PangoFontDescription *font_desc;

	IWINDOW_CLASS( parent_class )->build( widget );

	swin = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( swin ),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_box_pack_start( GTK_BOX( iwnd->work ), swin, TRUE, TRUE, 0 );
	gtk_widget_show( swin );

	log->view = gtk_text_view_new();
	gtk_text_view_set_editable( GTK_TEXT_VIEW( log->view ), FALSE );
	gtk_text_view_set_cursor_visible( GTK_TEXT_VIEW( log->view ), 
		FALSE );
	font_desc = pango_font_description_from_string( "Mono" );
	gtk_widget_modify_font( log->view, font_desc );
	pango_font_description_free( font_desc );

	gtk_container_add( GTK_CONTAINER( swin ), log->view );
	gtk_widget_show( log->view );
}

static void
log_class_init( LogClass *class )
{
	iWindowClass *iwindow_class = (iWindowClass *) class;

	parent_class = g_type_class_peek_parent( class );

	iwindow_class->build = log_build;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
log_init( Log *log )
{
}

GtkType
log_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Log",
			sizeof( Log ),
			sizeof( LogClass ),
			(GtkClassInitFunc) log_class_init,
			(GtkObjectInitFunc) log_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_IWINDOW, &info );
	}

	return( type );
}

void
log_clear_action_cb( GtkAction *action, Log *log )
{
	GtkTextView *text_view = GTK_TEXT_VIEW( log->view );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );

	gtk_text_buffer_set_text( text_buffer, "", 0 );
}

void
log_text( Log *log, const char *buf )
{
	GtkTextView *text_view = GTK_TEXT_VIEW( log->view );
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer( text_view );
	GtkTextMark *mark = gtk_text_buffer_get_insert( text_buffer );
	GtkTextIter iter;

	gtk_text_buffer_get_end_iter( text_buffer, &iter );
	gtk_text_buffer_move_mark( text_buffer, mark, &iter );
	gtk_text_buffer_insert_at_cursor( text_buffer, buf, -1 );
	gtk_text_view_scroll_to_mark( text_view, mark, 
		0.0, TRUE, 0.5, 1 );
}

void
log_textf( Log *log, const char *fmt, ... )
{
	va_list ap;
 	char buf[MAX_STRSIZE];

        va_start( ap, fmt );
        (void) im_vsnprintf( buf, MAX_STRSIZE, fmt, ap );
        va_end( ap );

	log_text( log, buf );
}
