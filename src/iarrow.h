/* a ip region class in a workspace
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

#define TYPE_IARROW (iarrow_get_type())
#define IARROW( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_IARROW, iArrow ))
#define IARROW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_IARROW, iArrowClass))
#define IS_IARROW( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IARROW ))
#define IS_IARROW_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IARROW ))
#define IARROW_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_IARROW, iArrowClass ))

struct _iArrow {
	Classmodel parent_class;

	/* Class fields.
	 */
	iRegionInstance instance;

	/* Private ... build iobject caption here.
	 */
	VipsBuf caption_buffer;
};

typedef struct _iArrowClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} iArrowClass;

GType iarrow_get_type( void );
