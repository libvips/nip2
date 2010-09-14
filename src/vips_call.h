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

/* Maxiumum number of args to a VIPS function.
 */
#define MAX_VIPS_ARGS (100)

/* Maximum length of a vector we pass to INTVEC etc.
 */
#define MAX_VEC (10000)

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
	VIPS_GVALUE,
	VIPS_INTERPOLATE
} VipsArgumentType;

#define TYPE_VIPS_INFO (vips_info_get_type())
#define VIPS_INFO( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_VIPS_INFO, VipsInfo ))
#define VIPS_INFO_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_VIPS_INFO, VipsInfoClass))
#define IS_VIPS_INFO( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_VIPS_INFO ))
#define IS_VIPS_INFO_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_VIPS_INFO ))
#define VIPS_INFO_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_VIPS_INFO, VipsInfoClass ))

/* Stuff we hold about a call to a VIPS function.
 */
typedef struct _VipsInfo {
	iObject parent_object;

	/* Environment.
	 */
	const char *name;
	im_function *fn;		/* Function we call */
	Reduce *rc;			/* RC we run inside */

	/* Args we build. Images in vargv are IMAGE* pointers.
	 */
	im_object *vargv;		/* vargv we build for VIPS */
	int nargs;			/* Number of args needed from ip */
	int nres;			/* Number of objects we write back */
	int nires;			/* Number of images we write back */
	int inpos[MAX_VIPS_ARGS];	/* Positions of inputs */
	int outpos[MAX_VIPS_ARGS];	/* Positions of outputs */

	/* Input images. Need to track "destroy" on each one (and kill us
	 * in turn). 
	 *
	 * RW images are a bit different. These are really output images (we
	 * create the image that gets passed to the operation, just like
	 * output images), but it's a "t" image and we im_copy() an input to
	 * it to init it.
	 *
	 * So RW images appear in both inii and outii, but we don't look for
	 * destroy for it.
	 */
	int ninii;			
	Imageinfo *inii[MAX_VIPS_ARGS];	
	unsigned int inii_destroy_sid[MAX_VIPS_ARGS];
	unsigned int inii_invalidate_sid[MAX_VIPS_ARGS];

	/* Output images. 
	 */
	int noutii;
	Imageinfo *outii[MAX_VIPS_ARGS];
	unsigned int outii_destroy_sid[MAX_VIPS_ARGS];

	gboolean use_lut;		/* TRUE for using a lut */

	/* Cache hash code here.
	 */
	unsigned int hash;
	gboolean found_hash;

	/* Set if we're in the history cache.
	 */
	gboolean in_cache;
} VipsInfo;

typedef struct _VipsInfoClass {
	iObjectClass parent_class;

} VipsInfoClass;

extern GSList *vips_info_all;

VipsArgumentType vips_lookup_type( im_arg_type type );
void vips_error( VipsInfo *vi );

GType vips_info_get_type( void );

void vips_check_all_destroyed( void );
gboolean vips_type_needs_input( im_type_desc *ty );
gboolean vips_type_makes_output( im_type_desc *ty );

gboolean vips_is_callable( im_function *fn );
int vips_n_args( im_function *fn );
void vips_usage( VipsBuf *buf, im_function *fn );
void vips_spine( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out );
void vips_run( Reduce *rc, Compile *compile,
	int op, const char *name, HeapNode **arg, PElement *out,
	im_function *function );
void vipsva( Reduce *rc, PElement *out, const char *name, ... );
