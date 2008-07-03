/* Call vips functions from the graph reducer.
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

/* Keep in sync with vips_supported[].
 */
typedef enum _VipsArgumentType {
	VIPS_NONE = -1,
	VIPS_DOUBLE = 0,
	VIPS_INT,
	VIPS_COMPLEX,
	VIPS_STRING,
	VIPS_IMAGE,
	VIPS_DOUBLEVEC,
	VIPS_DMASK,
	VIPS_IMASK,
	VIPS_IMAGEVEC,
	VIPS_INTVEC,
	VIPS_GVALUE
} VipsArgumentType;

VipsArgumentType vips_type_find( im_arg_type type );

gboolean vips_is_callable( im_function *fn );
int vips_n_args( im_function *fn );
void vips_usage( BufInfo *buf, im_function *fn );
void vips_spine( Reduce *rc, const char *name, HeapNode **arg, PElement *out );
void vips_run( Reduce *rc, Compile *compile,
	int op, const char *name, HeapNode **arg, PElement *out,
	im_function *function );
void vipsva( Reduce *rc, PElement *out, const char *name, ... );
