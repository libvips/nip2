/* Util functions for heaps.
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

void graph_node( Heap *heap, VipsBuf *buf, HeapNode *root, gboolean fn );
void graph_pelement( Heap *heap, VipsBuf *buf, PElement *root, gboolean fn );
void graph_element( Heap *heap, VipsBuf *buf, Element *root, gboolean fn );
void graph_pointer( PElement *root );

/* Reduce and print, csh-style output.
 */
void graph_value( PElement *root );

/* Make sure all images are written to disc, and won't be deleted on exit.
 */
void *save_objects( PElement *base, char *filename );
