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

#define TYPE_LOG (log_get_type())
#define LOG( obj ) (GTK_CHECK_CAST( (obj), TYPE_LOG, Log ))
#define LOG_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_LOG, LogClass ))
#define IS_LOG( obj ) (GTK_CHECK_TYPE( (obj), TYPE_LOG ))
#define IS_LOG_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_LOG ))

struct _Log {
	iWindow parent_class;

	GtkWidget *view;	/* The textview we use to show the log */
};

typedef struct _LogClass {
	iWindowClass parent_class;

	/* My methods.
	 */
} LogClass;

GtkType log_get_type( void );

void log_clear_action_cb( GtkAction *action, Log *log );
void log_text( Log *log, const char *buf );
void log_textf( Log *log, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
