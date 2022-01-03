/* subclass imagedisplay ... show a patch of plain colour from a 1x1 pixel
 * imageinfo
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

#define TYPE_COLOURDISPLAY (colourdisplay_get_type())
#define COLOURDISPLAY( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_COLOURDISPLAY, Colourdisplay ))
#define COLOURDISPLAY_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_COLOURDISPLAY, ColourdisplayClass ))
#define IS_COLOURDISPLAY( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_COLOURDISPLAY ))
#define IS_COLOURDISPLAY_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_COLOURDISPLAY ))

typedef struct _Colourdisplay {
	GtkWidget parent_instance;

	Imagedisplay *imagedisplay;

	/* Set this to indicate that we prefer to drag as text rather than 
	 * colour.
	 */
	gboolean drag_as_text;
} Colourdisplay;

typedef struct _ColourdisplayClass {
	GtkWidgetClass parent_class;

	/* My methods.
	 */
} ColourdisplayClass;

GType colourdisplay_get_type( void );
Colourdisplay *colourdisplay_new( Conversion *conv );
