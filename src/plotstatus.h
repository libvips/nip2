/* display plot info and mouse posn
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

#define TYPE_PLOTSTATUS (plotstatus_get_type())
#define PLOTSTATUS( obj ) (GTK_CHECK_CAST( (obj), TYPE_PLOTSTATUS, Plotstatus ))
#define PLOTSTATUS_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_PLOTSTATUS, PlotstatusClass ))
#define IS_PLOTSTATUS( obj ) (GTK_CHECK_TYPE( (obj), TYPE_PLOTSTATUS ))
#define IS_PLOTSTATUS_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PLOTSTATUS ))

struct _Plotstatus {
	GtkFrame parent_class;

	Plotmodel *plotmodel;

	GtkWidget *top;		/* Top label */
	GtkWidget *pos;		/* Position */
	GtkWidget *hb;		/* Band element hbox */
	GtkWidget *mag;		/* Magnification display */

	GtkWidget **label;	/* A label for displaying each series */
	int columns;		/* Last number of columns we saw */
};

typedef struct _PlotstatusClass {
	GtkFrameClass parent_class;

	/* My methods.
	 */
} PlotstatusClass;

GtkType plotstatus_get_type( void );
Plotstatus *plotstatus_new( Plotmodel *plotmodel );
void plotstatus_mouse( Plotstatus *plotstatus, double x, double y );
