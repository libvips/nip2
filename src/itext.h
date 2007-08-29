/* a text button in a workspace
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

#define TYPE_ITEXT (itext_get_type())
#define ITEXT( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_ITEXT, iText ))
#define ITEXT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_ITEXT, iTextClass))
#define IS_ITEXT( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_ITEXT ))
#define IS_ITEXT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_ITEXT ))
#define ITEXT_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_ITEXT, iTextClass ))

struct _iText {
	Heapmodel parent_class;

	BufInfo value;			/* The value displayed as a [char] */
	char *formula;			/* The formula we edit */
	char *formula_default;		/* Formula we inherit */
	BufInfo decompile;		/* The value decompiled to a [char] */

	/* TRUE if the formula has been entered by the user and should be
	 * saved.
	 * 
	 * Can't use classmodel edited, as text must inherit from heapmodel.
	 * Some duplication of code ... see itext_clear_edited()
	 */
	gboolean edited;
};

typedef struct _iTextClass {
	HeapmodelClass parent_class;

	/* My methods.
	 */
} iTextClass;

GType itext_get_type( void );
iText *itext_new( Rhs *rhs );

gboolean itext_value( Reduce *rc, BufInfo *buf, PElement *root );
void itext_value_ev( Reduce *rc, BufInfo *buf, PElement *root );
gboolean itext_make_value_string( Expr *expr, BufInfo *buf );

void itext_set_edited( iText *text, gboolean edited );
gboolean itext_set_formula( iText *text, const char *formula );
