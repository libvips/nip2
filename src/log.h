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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#define TYPE_LOG (log_get_type())
#define LOG( obj ) (G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_LOG, Log ))
#define LOG_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_LOG, LogClass))
#define IS_LOG( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_LOG ))
#define IS_LOG_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_LOG ))
#define LOG_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_LOG, LogClass ))

struct _Log {
	iWindow parent_class;

	GtkWidget *view;	/* The textview we use to show the log */
};

typedef struct _LogClass {
	iWindowClass parent_class;

	/* How we want the menu bar built.
	 */
	GtkActionEntry *actions;
	int n_actions;
	GtkToggleActionEntry *toggle_actions;
	int n_toggle_actions;
	const char *action_name;
	const char *ui_description;
	const char *menu_bar_name;
} LogClass;

GType log_get_type( void );

void log_clear_action_cb( GtkAction *action, Log *log );
void log_text( Log *log, const char *buf );
void log_textf( Log *log, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
