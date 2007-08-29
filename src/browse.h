/* thumbnail window
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

#define TYPE_BROWSE (browse_get_type())
#define BROWSE( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_BROWSE, Browse ))
#define BROWSE_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_BROWSE, BrowseClass ))
#define IS_BROWSE( obj ) (GTK_CHECK_TYPE( (obj), TYPE_BROWSE ))
#define IS_BROWSE_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_BROWSE ))

typedef struct _Browse {
	iDialog parent;

	/* My instance vars.
	 */
	Filesel *filesel;		/* FSB we are attached to */

	GtkWidget *label;		/* Directory label */
	GtkWidget *swin;		/* ScrolledWindow */
	GtkWidget *table;		/* Table containing buttons */

	/* Stuff for refresh-in-idle.
	 */
	gint idle_id;			/* Idle job, or zero */
	GSList *files;			/* List of files we load on idle */
	char *dirname;			/* Dir we are displaying files from */
	int row;			/* Current # of rows in table */
	int column;			/* Column we write next widget to */
} Browse;

typedef struct _BrowseClass {
	iDialogClass parent_class;

	/* My methods.
	 */
} BrowseClass;

GtkType browse_get_type( void );
GtkWidget *browse_new( void );
void browse_set_filesel( Browse *browse, Filesel *filesel );
void browse_refresh( Browse *browse, const gchar *dirname );
