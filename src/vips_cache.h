/* Cache recent vips operations
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

/* Maxiumum number of args to a VIPS function.
 */
#define MAX_VIPS_ARGS (100)

/* Stuff we hold about a call to a VIPS function.
 */
typedef struct _VipsCache {
	/* Environment.
	 */
	const char *name;
	im_function *fn;		/* Function we call */

	/* Args we build. Images in vargv are IMAGE* pointers.
	 */
	im_object *vargv;

	/* Input images. 
	 */
	int ninim;			
	IMAGE *inim[MAX_VIPS_ARGS];	

	/* Cache hash code here.
	 */
	unsigned int hash;
	gboolean found_hash;

	/* Set if we're in the cache. vips_cache_new() makes a VipsCache not in
	 * the cache, then we marshall args into it and lookup. If we find a
	 * previous call, this VipsCache will be junked without ever making it
	 * into the cache.
	 */
	gboolean in_cache;

	/* List of the proxys linked to this VipsCache. When we destroy this
	 * VipsCache we need to zap the ->call members in all the associated
	 * proxys.
	 */
	GSList *proxys;
} VipsCache;

VipsCache *vips_cache_dispatch( VipsCache *cache );
