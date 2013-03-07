/* a matrix in a workspace
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

#define TYPE_MATRIX (matrix_get_type())
#define MATRIX( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_MATRIX, Matrix ))
#define MATRIX_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_MATRIX, MatrixClass))
#define IS_MATRIX( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_MATRIX ))
#define IS_MATRIX_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_MATRIX ))
#define MATRIX_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_MATRIX, MatrixClass ))

/* What kind of ui bits have we asked for for this matrix?
 */
typedef enum {
	MATRIX_DISPLAY_TEXT = 0,	/* Set of text widgets */
	MATRIX_DISPLAY_SLIDER,		/* Set of sliders */
	MATRIX_DISPLAY_TOGGLE,		/* Set of 3 value toggles */
	MATRIX_DISPLAY_TEXT_SCALE_OFFSET,/* Text, with scale/offset widgets */
	MATRIX_DISPLAY_LAST
} MatrixDisplayType;

typedef struct _Matrix {
	Classmodel model;

	/* Base class fields.
	 */
	MatrixValue value;

	/* Other class fields.
	 */
	MatrixDisplayType display;	/* Display as */
	double scale;
	double offset;

	/* Is there a current selection on the matrixview? And if there is,
	 * the cells it covers.
	 */
	gboolean selected;
	Rect range;
} Matrix;

typedef struct _MatrixClass {
	ClassmodelClass parent_class;

	/* My methods.
	 */
} MatrixClass;

gboolean matrix_value_resize( MatrixValue *value, int width, int height );

GType matrix_get_type( void );

/* Select rectangular areas of matricies.
 */
void matrix_select( Matrix *matrix, int left, int top, int width, int height );
void matrix_deselect( Matrix *matrix );

void matrix_dmask_to_ip( DOUBLEMASK *dmask, VipsBuf *buf );
gboolean matrix_dmask_to_heap( Heap *heap, DOUBLEMASK *dmask, PElement *out );
DOUBLEMASK *matrix_imask_to_dmask( INTMASK *imask );
INTMASK *matrix_dmask_to_imask( DOUBLEMASK *dmask );
gboolean matrix_imask_to_heap( Heap *heap, INTMASK *imask, PElement *out );
DOUBLEMASK *matrix_ip_to_dmask( PElement *root );
INTMASK *matrix_ip_to_imask( PElement *root );
DOUBLEMASK *matrix_model_to_dmask( Matrix *matrix );
gboolean matrix_dmask_to_model( Matrix *matrix, DOUBLEMASK *dmask );
