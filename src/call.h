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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */

/*

    These files are distributed with CALL - http://www.vips.ecs.soton.ac.uk

*/

/* Maxiumum number of args to a CALL function.
 */
#define MAX_CALL_ARGS (100)

/* Maximum length of a vector we pass to INTVEC etc.
 */
#define MAX_VEC (10000)

typedef enum _CallArgumentType {
	CALL_NONE = -1,
	CALL_DOUBLE = 0,
	CALL_INT,
	CALL_COMPLEX,
	CALL_STRING,
	CALL_IMAGE,
	CALL_DOUBLEVEC,
	CALL_DMASK,
	CALL_IMASK,
	CALL_IMAGEVEC,
	CALL_INTVEC,
	CALL_GVALUE,
	CALL_INTERPOLATE
} CallArgumentType;

#define TYPE_CALL_INFO (call_info_get_type())
#define CALL_INFO( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_CALL_INFO, CallInfo ))
#define CALL_INFO_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_CALL_INFO, CallInfoClass))
#define IS_CALL_INFO( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_CALL_INFO ))
#define IS_CALL_INFO_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_CALL_INFO ))
#define CALL_INFO_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_CALL_INFO, CallInfoClass ))

/* Stuff we hold about a call to a CALL function.
 */
typedef struct _CallInfo {
	iObject parent_object;

	/* Environment.
	 */
	const char *name;
	im_function *fn;		/* Function we call */
	Reduce *rc;			/* RC we run inside */

	/* Args we build. Images in vargv are IMAGE* pointers.
	 */
	im_object *vargv;		/* vargv we build for CALL */
	int nargs;			/* Number of args needed from ip */
	int nres;			/* Number of objects we write back */
	int nires;			/* Number of images we write back */
	int inpos[MAX_CALL_ARGS];	/* Positions of inputs */
	int outpos[MAX_CALL_ARGS];	/* Positions of outputs */

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
	Imageinfo *inii[MAX_CALL_ARGS];	
	unsigned int inii_destroy_sid[MAX_CALL_ARGS];
	unsigned int inii_invalidate_sid[MAX_CALL_ARGS];

	/* Output images. 
	 */
	int noutii;
	Imageinfo *outii[MAX_CALL_ARGS];
	unsigned int outii_destroy_sid[MAX_CALL_ARGS];

	gboolean use_lut;		/* TRUE for using a lut */

	/* Cache hash code here.
	 */
	unsigned int hash;
	gboolean found_hash;

	/* Set if we're in the history cache.
	 */
	gboolean in_cache;

	/* Set if we hold refs in inii/outii that must be dropped.
	 */
	gboolean must_drop;
} CallInfo;

typedef struct _CallInfoClass {
	iObjectClass parent_class;

} CallInfoClass;

extern GSList *call_info_all;

CallArgumentType call_lookup_type( im_arg_type type );
void call_error( CallInfo *vi );
void call_error_toomany( CallInfo *vi );

GType call_info_get_type( void );

void call_check_all_destroyed( void );
gboolean call_type_needs_input( im_type_desc *ty );
gboolean call_type_makes_output( im_type_desc *ty );

gboolean call_is_callable( im_function *fn );
int call_n_args( im_function *fn );
void call_usage( VipsBuf *buf, im_function *fn );
void call_spine( Reduce *rc, 
	const char *name, HeapNode **arg, PElement *out );
void call_run( Reduce *rc, Compile *compile,
	int op, const char *name, HeapNode **arg, PElement *out,
	im_function *function );
void callva( Reduce *rc, PElement *out, const char *name, ... );
