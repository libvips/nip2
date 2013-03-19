/* Declarations for mainwtab.
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

#define TYPE_MAINWTAB (mainwtab_get_type())
#define MAINWTAB( obj ) (GTK_CHECK_CAST( (obj), TYPE_MAINWTAB, Mainwtab ))
#define MAINWTAB_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_MAINWTAB, MainwtabClass ))
#define IS_MAINWTAB( obj ) (GTK_CHECK_TYPE( (obj), TYPE_MAINWTAB ))
#define IS_MAINWTAB_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_MAINWTAB ))

struct _Mainwtab {
	vObject parent_object;

	/* Workspace we display, also in parent_object->iobject.
	 */
	Workspace *ws;

	/* The last row we visited with the 'next-error' button.
	 */
	Row *row_last_error;

	/* Component widgets.
	 */
	Toolkitbrowser *toolkitbrowser;
	Workspacedefs *workspacedefs;
	Workspaceview *wsview;

	GtkWidget *popup;
	GtkWidget *popup_jump;

	/* Left and right panes ... program window and toolkit browser.
	 */
	Pane *lpane;
	Pane *rpane;
};

typedef struct _MainwtabClass {
	vObjectClass parent_class;

	/* My methods.
	 */
} MainwtabClass;

GType mainwtab_get_type( void );

void mainwtab_jump_update( Mainwtab *mainwtab, GtkWidget *menu );

int mainwtab_clone( Mainwtab *mainwtab );
gboolean mainwtab_ungroup( Mainwtab *mainwtab );

Workspace *mainwtab_open_file_into_workspace( Mainwtab *mainwtab, 
	const char *filename );
Mainwtab *mainwtab_new( Workspace *ws );

