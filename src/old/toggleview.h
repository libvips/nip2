/* a toggleview button in a workspace
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

#define TYPE_TOGGLEVIEW (toggleview_get_type())
#define TOGGLEVIEW( obj ) (G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_TOGGLEVIEW, Toggleview ))
#define TOGGLEVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_TOGGLEVIEW, ToggleviewClass ))
#define IS_TOGGLEVIEW( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_TOGGLEVIEW ))
#define IS_TOGGLEVIEW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_TOGGLEVIEW ))

typedef struct _Toggleview {
	Graphicview parent_object;

	/* My instance vars.
	 */
	GtkWidget *toggle;
} Toggleview;

typedef struct _ToggleviewClass {
	GraphicviewClass parent_class;

	/* My methods.
	 */
} ToggleviewClass;

GType toggleview_get_type( void );
View *toggleview_new( void );
