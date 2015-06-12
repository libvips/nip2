/* an editable expression in a workspace
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

#define TYPE_EXPRESSION (expression_get_type())
#define EXPRESSION( obj ) (G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_EXPRESSION, Expression ))
#define EXPRESSION_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_EXPRESSION, ExpressionClass ))
#define IS_EXPRESSION( obj ) (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_EXPRESSION ))
#define IS_EXPRESSION_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_EXPRESSION ))

struct _Expression {
	Classmodel parent_class;

	/* We don't have a model for the expression: instead we just grab the
	 * value/formula from our MEMBER_VALUE itext. Much simpler.
	 */
};

typedef struct _ExpressionClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} ExpressionClass;

GType expression_get_type( void );

iText *expression_get_itext( Expression *expression );

