/* a slider in a workspace
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

#define TYPE_SLIDER (slider_get_type())
#define SLIDER( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_SLIDER, Slider ))
#define SLIDER_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_SLIDER, SliderClass))
#define IS_SLIDER( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_SLIDER ))
#define IS_SLIDER_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_SLIDER ))
#define SLIDER_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_SLIDER, SliderClass ))

typedef struct _Slider {
	Classmodel parent_object;

	double from, to, value;
} Slider;

typedef struct _SliderClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} SliderClass;

GType slider_get_type( void );
