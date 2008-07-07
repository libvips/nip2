/* Call recent vips operations
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

/* Stuff we hold about a call to a VIPS function.
 */
typedef struct _VipsCall {
	/* Environment.
	 */
	im_function *fn;		/* Function we call */
	int result;			/* What we got from calling it */

	/* Args we build. Images in vargv are IMAGE* pointers.
	 */
	im_object *vargv;

	/* Call hash code here.
	 */
	unsigned int hash;
	gboolean found_hash;

	/* Set if we're in the cache. vips_call_new() makes a VipsCall not in
	 * the cache, then we marshall args into it and lookup. If we find a
	 * previous call, this VipsCall will be junked without ever making it
	 * into the cache.
	 */
	gboolean in_cache;

	/* List of the proxys linked to this VipsCall. When we destroy this
	 * VipsCall we need to zap the ->call members in all the associated
	 * proxys.
	 */
	GSList *proxys;
} VipsCall;

extern int vips_call_size;

VipsCall *vips_call_lookup( VipsCall *call );
VipsCall *vips_call_begin( im_function *fn );
VipsCall *vips_call_dispatch( VipsCall *call );
void vips_call_end( VipsCall *call );

void vips_call_check_all_destroyed( void );
