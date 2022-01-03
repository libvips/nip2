/* a option in a workspace
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

#define TYPE_OPTION (option_get_type())
#define OPTION( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_OPTION, Option ))
#define OPTION_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_OPTION, OptionClass))
#define IS_OPTION( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_OPTION ))
#define IS_OPTION_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_OPTION ))
#define OPTION_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_OPTION, OptionClass ))

typedef struct _Option {
	Classmodel parent_class;

	/* Base class fields.
	 */
	GSList *labels;		/* [[char]] for option fields */
	int value;		/* Index of current option */
} Option;

typedef struct _OptionClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} OptionClass;

GType option_get_type( void );
