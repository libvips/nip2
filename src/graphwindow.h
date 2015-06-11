/* display workspaces with graphviz
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

#define TYPE_GRAPHWINDOW (graphwindow_get_type())
#define GRAPHWINDOW( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_GRAPHWINDOW, Graphwindow ))
#define GRAPHWINDOW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_GRAPHWINDOW, GraphwindowClass ))
#define IS_GRAPHWINDOW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_GRAPHWINDOW ))
#define IS_GRAPHWINDOW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_GRAPHWINDOW ))

struct _Graphwindow {
	Floatwindow parent_class;

	/* The last dot graph we generated.
	 */
	char *dot;

	/* Regenerate the graph on a timeout to avoid regen on many small 
	 * changes.
	 */
	guint layout_timeout;

	/* The imagedisplay we make.
	 */
	Imagemodel *imagemodel;
	Imagepresent *ip;

	/* Watch the ws with this.
	 */
	guint workspace_changed_sid;

#ifdef HAVE_LIBGVC
	GVC_t *gvc;
	graph_t *graph;
#endif /*HAVE_LIBGVC*/
};

typedef struct _GraphwindowClass {
	FloatwindowClass parent_class;

	/* My methods.
	 */
} GraphwindowClass;

void graph_write( Workspace *ws );

GType graphwindow_get_type( void );
Graphwindow *graphwindow_new( Workspace *ws, GtkWidget *parent );

