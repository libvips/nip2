/* a colour colour in a workspace
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

#define TYPE_COLOUR (colour_get_type())
#define COLOUR( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_COLOUR, Colour ))
#define COLOUR_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_COLOUR, ColourClass))
#define IS_COLOUR( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_COLOUR ))
#define IS_COLOUR_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_COLOUR ))
#define COLOUR_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_COLOUR, ColourClass ))

struct _Colour {
	Classmodel parent_class;

	/* Class fields.
	 */
	double value[3];
	char *colour_space;

	/* Build view caption here.
	 */
	VipsBuf caption;
};

typedef struct _ColourClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} ColourClass;

Imageinfo *colour_ii_new( Colour *colour );
void colour_set_rgb( Colour *colour, double rgb[3] );

GType colour_get_type( void );
