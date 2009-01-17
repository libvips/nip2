/* The thing that sits in a pane showing the title and close button.
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

#define TYPE_PANECHILD (panechild_get_type())
#define PANECHILD( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_PANECHILD, Panechild ))
#define PANECHILD_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_PANECHILD, PanechildClass ))
#define IS_PANECHILD( obj ) \
	(GTK_CHECK_TYPE( (obj), TYPE_PANECHILD ))
#define IS_PANECHILD_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PANECHILD ))

typedef struct _Panechild {
	vObject parent_object;

	Pane *pane;			/* The pane we are part of */

	const char *title;		/* Title we display */
	GtkWidget *label;		/* Titlebar label */
} Panechild;

typedef struct _PanechildClass {
	vObjectClass parent_class;

} PanechildClass;

GtkType panechild_get_type( void );
Panechild *panechild_new( Pane *pane, const char *title );
