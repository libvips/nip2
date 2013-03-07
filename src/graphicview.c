/* run the display for a graphic in a workspace 
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

/*
#define DEBUG
 */

#include "ip.h"

static ViewClass *parent_class = NULL;

static void
graphicview_link( View *view, Model *model, View *parent )
{
	Graphicview *graphicview = GRAPHICVIEW( view );
	View *v;

	VIEW_CLASS( parent_class )->link( view, model, parent );

	/* Find the enclosing subcolumnview.
	 */
	for( v = parent; v && !IS_SUBCOLUMNVIEW( v ); v = v->parent )
		;
	if( v )
		graphicview->sview = SUBCOLUMNVIEW( v );
}

static void
graphicview_class_init( GraphicviewClass *class )
{
	ViewClass *view_class = (ViewClass *) class;

	parent_class = g_type_class_peek_parent( class );

	view_class->link = graphicview_link;
}

static void
graphicview_init( Graphicview *graphicview )
{
	graphicview->sview = NULL;
}

GtkType
graphicview_get_type( void )
{
	static GtkType graphicview_type = 0;

	if( !graphicview_type ) {
		static const GtkTypeInfo sinfo = {
			"Graphicview",
			sizeof( Graphicview ),
			sizeof( GraphicviewClass ),
			(GtkClassInitFunc) graphicview_class_init,
			(GtkObjectInitFunc) graphicview_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		graphicview_type = gtk_type_unique( TYPE_VIEW, &sinfo );
	}

	return( graphicview_type );
}
