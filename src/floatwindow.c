/* abstract base class for floatwindow / plotwindow etc.
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

/*
#define DEBUG
 */

#include "ip.h"

static iWindowClass *parent_class = NULL;

static void
floatwindow_destroy( GtkObject *object )
{
	Floatwindow *floatwindow;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_FLOATWINDOW( object ) );

	floatwindow = FLOATWINDOW( object );

#ifdef DEBUG
	printf( "floatwindow_destroy\n" );
#endif /*DEBUG*/

	/* My instance destroy stuff.
	 */

	GTK_OBJECT_CLASS( parent_class )->destroy( object );
}

static void
floatwindow_popdown( GtkWidget *widget )
{
	Floatwindow *floatwindow = FLOATWINDOW( widget );
	Classmodel *classmodel = floatwindow->classmodel;

	/* We have to note position/size in popdown rather than destroy, since
	 * the widgets have to all still be extant.
	 */

	/* Note position/size for later reuse.
	 */
	classmodel->window_width = 
		GTK_WIDGET( floatwindow )->allocation.width;
	classmodel->window_height = 
		GTK_WIDGET( floatwindow )->allocation.height;
	gdk_window_get_root_origin( 
		gtk_widget_get_toplevel( GTK_WIDGET( floatwindow ) )->window, 
		&classmodel->window_x, &classmodel->window_y );

	IWINDOW_CLASS( parent_class )->popdown( widget );
}

static void
floatwindow_build( GtkWidget *widget )
{
	Floatwindow *floatwindow = FLOATWINDOW( widget );
	Classmodel *classmodel = floatwindow->classmodel;

	IWINDOW_CLASS( parent_class )->build( widget );

	/* Must be set with floatmodel_link before build.
	 */
	g_assert( floatwindow->classmodel );

	/* Position and size to restore? Come here after parent build, so we
	 * can override any default settings from there.
	 */
	if( classmodel->window_width != -1 ) {
		GdkScreen *screen = 
			gtk_widget_get_screen( GTK_WIDGET( floatwindow ) );
		int screen_width = gdk_screen_get_width( screen );
		int screen_height = gdk_screen_get_height( screen );

		/* We need to clip x/y against the desktop size ... we may be
		 * loading a workspace made on a machine with a big screen on
		 * a machine with a small screen.

		 	FIXME ... we could only clip if the window will be
			completely off the screen? ie. ignore
			iimage->window_width etc.

		 */

		int window_x = IM_CLIP( 0, classmodel->window_x,
			screen_width - classmodel->window_width );
		int window_y = IM_CLIP( 0, classmodel->window_y,
			screen_height - classmodel->window_height );
		int window_width = IM_MIN( classmodel->window_width,
			screen_width );
		int window_height = IM_MIN( classmodel->window_height,
			screen_height );

		gtk_widget_set_uposition( GTK_WIDGET( floatwindow ), 
			window_x, window_y );
		gtk_window_set_default_size( GTK_WINDOW( floatwindow ),
			window_width, window_height );
	}
}

static void
floatwindow_class_init( FloatwindowClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	iWindowClass *iwindow_class = (iWindowClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = floatwindow_destroy;

	iwindow_class->build = floatwindow_build;
	iwindow_class->popdown = floatwindow_popdown;

	/* Hmm, this rather negates the point of this class. If we make plot
	 * and image windows transient for the main window, we don't get
	 * maximise buttons :-( (on gnome and win anyway).
	 *
	 * Keep this class around for now, maybe it'll still be useful.
	 */
	iwindow_class->transient = FALSE;

	/* Create signals.
	 */

	/* Init methods.
	 */
}

static void
floatwindow_init( Floatwindow *floatwindow )
{
	floatwindow->classmodel = NULL;
}

GtkType
floatwindow_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Floatwindow",
			sizeof( Floatwindow ),
			sizeof( FloatwindowClass ),
			(GtkClassInitFunc) floatwindow_class_init,
			(GtkObjectInitFunc) floatwindow_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_IWINDOW, &info );
	}

	return( type );
}

void
floatwindow_link( Floatwindow *floatwindow, Classmodel *classmodel )
{
	floatwindow->classmodel = classmodel;
	destroy_if_destroyed( G_OBJECT( floatwindow ), 
		G_OBJECT( classmodel ), (DestroyFn) gtk_widget_destroy );
}
