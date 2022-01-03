/* a plotpresent in a floating window 
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

#define TYPE_PLOTWINDOW (plotwindow_get_type())
#define PLOTWINDOW( obj ) (G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_PLOTWINDOW, Plotwindow ))
#define PLOTWINDOW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_PLOTWINDOW, PlotwindowClass ))
#define IS_PLOTWINDOW( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_PLOTWINDOW ))
#define IS_PLOTWINDOW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_PLOTWINDOW ))

struct _Plotwindow {
	Floatwindow parent_class;

	/* The model we watch.
	 */
	Plotmodel *plotmodel;

	/* Widgets.
	 */
	Plotstatus *plotstatus;
	Plotpresent *plotpresent;
};

typedef struct _PlotwindowClass {
	FloatwindowClass parent_class;

	/* My methods.
	 */
} PlotwindowClass;

GType plotwindow_get_type( void );
Plotwindow *plotwindow_new( Plot *plot, GtkWidget *parent );
