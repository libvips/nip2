/* abstract base class for imageview / plotwindow etc.
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

#define TYPE_FLOATWINDOW (floatwindow_get_type())
#define FLOATWINDOW( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_FLOATWINDOW, Floatwindow ))
#define FLOATWINDOW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_FLOATWINDOW, FloatwindowClass ))
#define IS_FLOATWINDOW( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_FLOATWINDOW ))
#define IS_FLOATWINDOW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_FLOATWINDOW ))

typedef struct _Floatwindow {
	iWindow parent_class;

	/* Model stuff here.
	 */
	Model *model;
} Floatwindow;

typedef struct _FloatwindowClass {
	iWindowClass parent_class;

	/* My methods.
	 */
} FloatwindowClass;

GType floatwindow_get_type( void );
void floatwindow_link( Floatwindow *floatwindow, Model *model );
