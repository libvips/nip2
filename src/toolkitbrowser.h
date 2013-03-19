/* Toolkit browser
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

#define TYPE_TOOLKITBROWSER (toolkitbrowser_get_type())
#define TOOLKITBROWSER( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_TOOLKITBROWSER, Toolkitbrowser ))
#define TOOLKITBROWSER_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), \
		TYPE_TOOLKITBROWSER, ToolkitbrowserClass ))
#define IS_TOOLKITBROWSER( obj ) \
	(GTK_CHECK_TYPE( (obj), TYPE_TOOLKITBROWSER ))
#define IS_TOOLKITBROWSER_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_TOOLKITBROWSER ))

typedef struct _Toolkitbrowser {
	vObject parent_object;

	Toolkitgroup *kitg;
	Workspace *ws;

	GtkListStore *store;		/* Model for list view */
	GtkTreeModel *filter;		/* After filtering with search box */
	GtkWidget *tree;		/* Displayed tree */
	GtkWidget *entry;		/* Search widget */
	GtkWidget *top;			/* hbox for top bar */
} Toolkitbrowser;

typedef struct _ToolkitbrowserClass {
	vObjectClass parent_class;

} ToolkitbrowserClass;

GtkType toolkitbrowser_get_type( void );
void toolkitbrowser_set_mainw( Toolkitbrowser *toolkitbrowser, Mainw *mainw );
Toolkitbrowser *toolkitbrowser_new( void );
int toolkitbrowser_get_width( Toolkitbrowser *toolkitbrowser );
void toolkitbrowser_set_workspace( Toolkitbrowser *toolkitbrowser, 
	Workspace *ws );

