/* display a caption/value label pair, on a click display the formula
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

#define TYPE_FORMULA (formula_get_type())
#define FORMULA( obj ) (GTK_CHECK_CAST( (obj), \
	TYPE_FORMULA, Formula ))
#define FORMULA_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), \
		TYPE_FORMULA, FormulaClass ))
#define IS_FORMULA( obj ) (GTK_CHECK_TYPE( (obj), TYPE_FORMULA ))
#define IS_FORMULA_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_FORMULA ))

typedef struct _Formula {
	GtkEventBox parent_object;

	/* State.
	 */
	char *caption;
	char *value;
	char *expr;
        gboolean edit;              	/* In edit mode */
        gboolean sensitive;            	/* Flick to edit on click */
        gboolean changed;            	/* ->entry changed since we set it */
	gboolean refresh_queued;	/* Awaiting refresh */
	gboolean needs_focus;		/* Grab focus on refresh */

	/* Widgets.
	 */
	GtkWidget *hbox;		/* Container for our stuff */
        GtkWidget *left_label;		/* Caption label */
        GtkWidget *right_label;		/* Display value here */
        GtkWidget *entry_frame;		/* Frame edit text with this */
        GtkWidget *entry;		/* Edit formula here */
} Formula;

typedef struct _FormulaClass {
	GtkEventBoxClass parent_class;

	/* My methods.
	 */
	void (*edit)( Formula * );	/* Formula has flicked to edit mode */
	void (*changed)( Formula * );	/* Formula change */
	void (*activate)( Formula * );	/* Pressed "Enter" key in formula */
	void (*enter)( Formula * );	/* Highlight change */
	void (*leave)( Formula * );	/* on eg. mouse enter/exit */
} FormulaClass;

void formula_set_edit( Formula *formula, gboolean edit );
void formula_set_sensitive( Formula *formula, gboolean sensitive );
void formula_set_needs_focus( Formula *formula, gboolean needs_focus );
gboolean formula_scan( Formula *formula );

GType formula_get_type( void );
Formula *formula_new( void );

void formula_set_caption( Formula *formula, const char *caption );
void formula_set_value_expr( Formula *formula,
	const char *value, const char *expr );
