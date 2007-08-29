/* Call vips8 functions with args from C and from the graph.
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

void call8_vips( Reduce *rc, const char *name, 
	PElement *required, PElement *optional, PElement *out );
void call8_header_get( Reduce *rc, 
	const char *name, VipsObject *object, PElement *out );
Managed *call8_image_new( Reduce *rc, 
	const char *filename, PElement *optional );
