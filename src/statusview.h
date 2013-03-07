/* Decls for statusview.c ... display image info and mouse posn
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

#define TYPE_STATUSVIEW (statusview_get_type())
#define STATUSVIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_STATUSVIEW, Statusview ))
#define STATUSVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_STATUSVIEW, StatusviewClass ))
#define IS_STATUSVIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_STATUSVIEW ))
#define IS_STATUSVIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_STATUSVIEW ))

/* A band element display in the status bar.
 */
typedef struct _StatusviewBand {
	Statusview *sv;		/* Bar we're in */
	int bandno;		/* Band we extract */
	GtkWidget *val;		/* Label we write to */
} StatusviewBand;

struct _Statusview {
	GtkFrame parent_class;

	Imagemodel *imagemodel;
	guint changed_sid;

	GtkWidget *top;		/* Top label */
	GtkWidget *pos;		/* Position */
	GtkWidget *hb;		/* Band element hbox */
	GtkWidget *mag;		/* Magnification display */
	GSList *bands;		/* List of StatusviewBand */
	int nb;			/* Last number of bands we saw */
	int fmt;		/* The last bandfmt we set ... for spacing */
};

typedef struct _StatusviewClass {
	GtkFrameClass parent_class;

	/* My methods.
	 */
} StatusviewClass;

GtkType statusview_get_type( void );
Statusview *statusview_new( Imagemodel *imagemodel );

void statusview_mouse( Statusview *sv, int x, int y );
