/* Declarations for the preferences dialog.
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

#define TYPE_PREFS (prefs_get_type())
#define PREFS( obj ) (GTK_CHECK_CAST( (obj), TYPE_PREFS, Prefs ))
#define PREFS_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_PREFS, PrefsClass ))
#define IS_PREFS( obj ) (GTK_CHECK_TYPE( (obj), TYPE_PREFS ))
#define IS_PREFS_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PREFS ))

typedef struct _Prefs {
	iDialog parent_object;

	/* Workspace we display.
	 */
	Workspace *ws;
	guint destroy_sid;

	Prefworkspaceview *pwview;

	/* (optionally) filter prefs with this.
	 */
	char *caption_filter;
} Prefs;

typedef struct _PrefsClass {
	iWindowClass parent_class;

	/* My methods.
	 */
} PrefsClass;

GType prefs_get_type( void );
Prefs *prefs_new( const char *caption_filter );
gboolean prefs_set( const char *name, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
