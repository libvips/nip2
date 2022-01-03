/* Declarations for doubleclick determiner.
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

typedef void (*DoubleclickFunc)( GtkWidget *, GdkEvent *, void * );
#define DOUBLECLICK_FUNC( fn ) ((DoubleclickFunc) (fn))

void doubleclick_add( GtkWidget *wid, gboolean dsingle,
	DoubleclickFunc single, void *clients,
	DoubleclickFunc dub, void *clientd );
