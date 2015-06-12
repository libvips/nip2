/* Def browser
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

#define TYPE_DEFBROWSER (defbrowser_get_type())
#define DEFBROWSER( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_DEFBROWSER, Defbrowser ))
#define DEFBROWSER_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_DEFBROWSER, DefbrowserClass ))
#define IS_DEFBROWSER( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_DEFBROWSER ))
#define IS_DEFBROWSER_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_DEFBROWSER ))

typedef struct _Defbrowser {
	vObject parent_object;

	Program *program;		/* Program whose kits we explore */

	GtkListStore *store;		/* Model for list view */
	GtkTreeModel *filter;		/* After filtering with search box */
	GtkWidget *tree;		/* Displayed tree */
	GtkWidget *entry;		/* Search widget */
	GtkWidget *top;			/* hbox for top bar */
} Defbrowser;

typedef struct _DefbrowserClass {
	vObjectClass parent_class;

} DefbrowserClass;

GType defbrowser_get_type( void );
void defbrowser_set_program( Defbrowser *defbrowser, Program *program );
Defbrowser *defbrowser_new( void );
int defbrowser_get_width( Defbrowser *defbrowser );
void defbrowser_set_filter( Defbrowser *defbrowser, const char *filter );

