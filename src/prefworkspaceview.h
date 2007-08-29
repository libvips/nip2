/* a view of a workspace for the preferences window
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

#define TYPE_PREFWORKSPACEVIEW (prefworkspaceview_get_type())
#define PREFWORKSPACEVIEW( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_PREFWORKSPACEVIEW, Prefworkspaceview ))
#define PREFWORKSPACEVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), \
		TYPE_PREFWORKSPACEVIEW, PrefworkspaceviewClass ))
#define IS_PREFWORKSPACEVIEW( obj ) \
	(GTK_CHECK_TYPE( (obj), TYPE_PREFWORKSPACEVIEW ))
#define IS_PREFWORKSPACEVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PREFWORKSPACEVIEW ))

struct _Prefworkspaceview {
	View view;

	Workspacegroupview *wgview;

	/* If set, only display the columns whose caption includes this string 
	 * (eg. "JPEG"). Used to display tiny prefs windows for jpeg save etc.
	 */
	char *caption_filter;
};

typedef struct _PrefworkspaceviewClass {
	ViewClass parent_class;

	/* My methods.
	 */
} PrefworkspaceviewClass;

GtkType prefworkspaceview_get_type( void );
View *prefworkspaceview_new( void );

void prefworkspaceview_set_caption_filter( Prefworkspaceview *pwview, 
	const char *caption_filter );
