/* Heap management.
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

/* Node type. Generally represent data objects.
 *
 * Don't use enum, as we want this to fit in 1 byte. 
 */
typedef unsigned char NodeType;
#define TAG_APPL (0)		/* Application */
#define TAG_CONS (1)		/* List cons */
#define TAG_FREE (2) 		/* On free list */
#define TAG_DOUBLE (3)		/* Constant double */
#define TAG_COMPLEX (4)		/* Constant complex */
#define TAG_GEN (5)		/* List generator */
#define TAG_CLASS (8)		/* Class object */
#define TAG_SHARED (9)		/* Root of a common sub-expression */
#define TAG_REFERENCE (10)	/* Reference to a common sub-expression */
#define TAG_FILE (12)		/* Generate list from file */

/* Element types. Generally represent operators.
 */
typedef unsigned char EType;
#define ELEMENT_NOVAL (0)	/* No value */
#define ELEMENT_NODE (1)	/* Pointer to another node */
#define ELEMENT_SYMBOL (2)	/* Pointer to Symbol, reduces to value */
#define ELEMENT_SYMREF (3)	/* Pointer to Symbol, does not reduce */
#define ELEMENT_COMPILEREF (4)	/* Pointer to Compile, does not reduce */
#define ELEMENT_CHAR (5)	/* Boxed char type */
#define ELEMENT_BOOL (6)	/* Boxed bool type */
#define ELEMENT_BINOP (7)	/* Binary operator */
#define ELEMENT_UNOP (8)	/* Unary operator */
#define ELEMENT_COMB (9)	/* Combinator */
#define ELEMENT_TAG (10)	/* RHS of '.' operator */
#define ELEMENT_MANAGED (11)	/* A managed object */
#define ELEMENT_CONSTRUCTOR (12)/* Class constructor */
#define ELEMENT_ELIST (13)	/* Empty list */

/* Flags we attach to a node.
 */
typedef unsigned char NodeFlags;
#define FLAG_SERIAL (31)	/* Serial number mask  .. must be 1st */
#define FLAG_PRINT (32)		/* Marked (for decompile print) */
#define FLAG_DEBUG (64)		/* Marked (for debug traverse) */
#define FLAG_MARK (128)		/* Marked (for mark-sweep) */
#define FLAG_ALL (255)		/* All flags mask */

/* Set the serial number without disturbing other stuff.
 */
#define SETSERIAL( FLAGS, SERIAL ) { \
	(FLAGS) = ((FLAGS) & (FLAG_SERIAL ^ FLAG_ALL)) | \
		((SERIAL) & FLAG_SERIAL); \
}

/* Combinators. Don't change the order of these! See reduce.c for an array
 * indexed with a CombinatorType.
 */
typedef enum combinator_type {
	COMB_S = 0,		/* S combinator */
	COMB_SL,		/* S-left combinator */
	COMB_SR,		/* S-right combinator */
	COMB_I,			/* Identity combinator */
	COMB_K,			/* K combinator */
	COMB_GEN 		/* List generator combinator */
} CombinatorType;

/* An element ... a tag plus a pointer. Use one of these to hold a pointer
 * into a heap.
 */
typedef struct _Element {
	EType type;
	void *ele;
} Element;

/* A node on the heap. Should fit in 12 bytes on most machines.
 */
typedef struct _HeapNode {
	/* Elements: either a pair of pointers, or a double. Sensible on most
	 * 32-bit systems, not so great on 64 bitters.
	 */
	union {
		struct {
			void *left;
			void *right;		
		} ptrs;
		double num;
	} body;

	/* Flags ... should fit in 4 bytes.
	 */
	NodeType type;		/* What this is */
	NodeFlags flgs;		/* GC flags etc */
	EType ltype;		/* Type of left element */
	EType rtype;		/* Type of right element */
} HeapNode;

/* Put type/value pairs into nodes. Make sure we completely read before we 
 * write.
 */
#define PPUTLEFT(N,T,V) {\
	EType t99 = (T);\
	void *v99 = (void*)(V);\
	\
	(N)->ltype = t99;\
	(N)->body.ptrs.left = v99;\
}
#define PPUTRIGHT(N,T,V) {\
	EType t99 = (T);\
	void *v99 = (void*)(V);\
	\
	(N)->rtype = t99;\
	(N)->body.ptrs.right = v99;\
}
#define PPUT(N,Tl,Vl,Tr,Vr) {PPUTLEFT( N, Tl, Vl ); PPUTRIGHT( N, Tr, Vr );}

/* Get value as a HeapNode pointer (most common case).
 */
#define GETLEFT(N) ((HeapNode*)((N)->body.ptrs.left))
#define GETRIGHT(N) ((HeapNode*)((N)->body.ptrs.right))
#define GETLT(N) ((N)->ltype)
#define GETRT(N) ((N)->rtype)

/* A pointer to an element inside a HeapNode, or to an Element.
 */
typedef struct pelement {
	EType *type;
	void **ele;
} PElement;

/* Make a PElement point to a node.
 */
#define PEPOINTLEFT(N,P) \
	{(P)->type=&((N)->ltype);(P)->ele=&((N)->body.ptrs.left);}
#define PEPOINTRIGHT(N,P) \
	{(P)->type=&((N)->rtype);(P)->ele=&((N)->body.ptrs.right);}

/* Make a PElement point to an element.
 */
#define PEPOINTE(PE,E) \
	{(PE)->type=&((E)->type);(PE)->ele=&((E)->ele);}

/* Get from a PE.
 */
#define PEGETTYPE(P) (*((P)->type))
#define PEGETVAL(P) ((HeapNode*)(*((P)->ele)))
#define PEGETE(P,E) ((E)->type = PEGETTYPE(P),(E)->ele = PEGETVAL(P))
#define PEGETP(PE,T,V) ((T)=*((PE)->type),(V)=*((PE)->ele))

/* Write to a PE. We are careful to eval all args before writing, in case we
 * are writing to one of the inputs.
 */
#define PEPUTE(PE,E) {*((PE)->type)=(E)->type;*((PE)->ele)=(E)->ele;}
#define PEPUTPE(PEto,PEfrom) {\
	EType t99 = PEGETTYPE(PEfrom);\
	void *v99 = PEGETVAL(PEfrom);\
	\
	*((PEto)->type) = t99;\
	*((PEto)->ele) = v99;\
}
#define PEPUTP(PE,T,V) {\
	EType t99 = (T);\
	void *v99 = GUINT_TO_POINTER(V);\
	\
	*((PE)->type) = t99;\
	*((PE)->ele) = v99;\
}

/* Write a PE to a node. Again, make sure we read both before we write, in
 * case we are writing an expression to ourselves.
 */
#define PEPUTLEFT(N,PE) {\
	EType t99 = PEGETTYPE(PE);\
	void *v99 = PEGETVAL(PE);\
	\
	(N)->ltype = t99;\
	(N)->body.ptrs.left = v99;\
}
#define PEPUTRIGHT(N,PE) {\
	EType t99 = PEGETTYPE(PE);\
	void *v99 = PEGETVAL(PE);\
	\
	(N)->rtype = t99;\
	(N)->body.ptrs.right = v99;\
}

/* Predicates.
 */
#define PEISBINOP(P) (PEGETTYPE(P) == ELEMENT_BINOP)
#define PEISBOOL(P) (PEGETTYPE(P) == ELEMENT_BOOL)
#define PEISCHAR(P) (PEGETTYPE(P) == ELEMENT_CHAR)
#define PEISCLASS(P) (PEISNODE(P) && PEGETVAL(P)->type == TAG_CLASS)
#define PEISCONSTRUCTOR(P) (PEGETTYPE(P) == ELEMENT_CONSTRUCTOR)
#define PEISCOMB(P) (PEGETTYPE(P) == ELEMENT_COMB)
#define PEISCOMPLEX(P) (PEISNODE(P) && PEGETVAL(P)->type == TAG_COMPLEX)
#define PEISTAG(P) (PEGETTYPE(P) == ELEMENT_TAG)
#define PEISMANAGED(P) (PEGETTYPE(P) == ELEMENT_MANAGED)
#define PEISMANAGEDGOBJECT(P) (PEISMANAGED(P) && \
	IS_MANAGEDGOBJECT( PEGETVAL(P) ))
#define PEISMANAGEDSTRING(P) (PEISMANAGED(P) && \
	IS_MANAGEDSTRING(PEGETVAL(P)))
#define PEISIMAGE(P) (PEISMANAGED(P) && IS_IMAGEINFO( PEGETVAL(P) ))
#define PEISVIPSOBJECT(P) \
	(PEISMANAGEDGOBJECT(P) && VIPS_IS_OBJECT( PEGETMANAGEDGOBJECT(P) )) 
#define PEISFILE(P) (PEISMANAGED(P) && IS_MANAGEDFILE(PEGETVAL(P)))
#define PEISELIST(P) (PEGETTYPE(P) == ELEMENT_ELIST)
#define PEISFLIST(P) ((PEISNODE(P) && PEGETVAL(P)->type == TAG_CONS) || \
	PEISMANAGEDSTRING(P))
#define PEISLIST(P) (PEISELIST(P) || PEISFLIST(P))
#define PEISNOVAL(P) (PEGETTYPE(P) == ELEMENT_NOVAL)
#define PEISNUM(P) (PEISREAL(P) || PEISCOMPLEX(P))
#define PEISNODE(P) (PEGETTYPE(P) == ELEMENT_NODE)
#define PEISREAL(P) (PEISNODE(P) && PEGETVAL(P)->type == TAG_DOUBLE)
#define PEISSYMBOL(P) (PEGETTYPE(P) == ELEMENT_SYMBOL)
#define PEISSYMREF(P) (PEGETTYPE(P) == ELEMENT_SYMREF)
#define PEISCOMPILEREF(P) (PEGETTYPE(P) == ELEMENT_COMPILEREF)
#define PEISUNOP(P) (PEGETTYPE(P) == ELEMENT_UNOP)

/* Extract bits of primitive compound types.
 */
#define PEGETSYMBOL(P) ((Symbol*)PEGETVAL(P))
#define PEGETSYMREF(P) ((Symbol*)PEGETVAL(P))
#define PEGETCOMPILE(P) ((Compile*)(PEGETVAL(P)))
#define PEGETBINOP(P) ((BinOp)PEGETVAL(P))
#define PEGETUNOP(P) ((UnOp)PEGETVAL(P))
#define PEGETCOMB(P) ((CombinatorType)PEGETVAL(P))
#define PEGETTAG(P) ((char*)PEGETVAL(P))
#define PEGETREAL(P) (PEGETVAL(P)->body.num)
#define PEGETBOOL(P) ((gboolean)GPOINTER_TO_UINT(PEGETVAL(P)))
#define PEGETCHAR(P) ((unsigned char)(GPOINTER_TO_UINT(PEGETVAL(P))))
#define PEGETIMAGE(P) (((Imageinfo*)PEGETVAL(P))->im)
#define PEGETII(P) ((Imageinfo*)PEGETVAL(P))
#define PEGETFILE(P) ((Managedfile*)PEGETVAL(P))
#define PEGETMANAGED(P) ((Managed*)PEGETVAL(P))
#define PEGETMANAGEDSTRING(P) ((Managedstring*)PEGETVAL(P))
#define PEGETMANAGEDGOBJECT(P) (((Managedgobject*)PEGETVAL(P))->object)
#define PEGETVIPSOBJECT(P) \
		((VipsObject*)(((Managedgobject*)PEGETVAL(P))->object))

#define PEGETHD(P1,P2) PEPOINTLEFT(PEGETVAL(P2), P1)
#define PEGETTL(P1,P2) PEPOINTRIGHT(PEGETVAL(P2), P1)

#define PEGETREALPART(P) (GETLEFT(PEGETVAL(P))->body.num)
#define PEGETIMAGPART(P) (GETRIGHT(PEGETVAL(P))->body.num)

#define PEGETCLASSCOMPILE(P) (COMPILE(GETLEFT(PEGETVAL(P))))
#define PEGETCLASSSECRET(P1,P2) PEPOINTLEFT(GETRIGHT(PEGETVAL(P2)),P1)
#define PEGETCLASSMEMBER(P1,P2) PEPOINTRIGHT(GETRIGHT(PEGETVAL(P2)),P1)

/* A block on the heap.
 */
struct _HeapBlock {
	Heap *heap;		/* Heap we are part of */
	HeapBlock *next;	/* Next block in chain */
	HeapNode *node;		/* Nodes on this block */
	int sz;			/* Number of nodes in this block */
};

/* Function to get max heap size.
 */
typedef int (*heap_max_fn)( Heap * );

#define TYPE_HEAP (heap_get_type())
#define HEAP( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_HEAP, Heap ))
#define HEAP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_HEAP, HeapClass))
#define IS_HEAP( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_HEAP ))
#define IS_HEAP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_HEAP ))
#define HEAP_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_HEAP, HeapClass ))

struct _Heap {
	iObject parent_object;

	Compile *compile;	/* If non-null, assoc. compile */

	heap_max_fn max_fn;	/* Max nodes in this heap */
	int mxb;		/* Max blocks until next check */
	int rsz;		/* Nodes to allocate in each extra block */
	int nb;			/* Number of blocks attached */
	HeapBlock *hb;		/* List of current blocks */
	HeapNode *free;		/* Start of free-node chain (sweep to here) */

	int ncells;		/* Cells allocated */
	int nfree;		/* Cells free */
	int serial;		/* Last serial number we used */
	gboolean filled;	/* Set on heap full */

	GHashTable *emark;	/* Set of elements to mark on GC */
	GHashTable *rmark;	/* Set of Reduce to mark on GC */
	GHashTable *mtable;	/* Managed associated with this heap */

        guint gc_tid;		/* id of gc delay timer */

	/* Set this to force unreffed objects out immediately. Handy for leak 
	 * testing.
	 */
	gboolean flush;
};

typedef struct _HeapClass {
	iObjectClass parent_class;

	/* My methods.
	 */
} HeapClass;

/* Get a node from the free-list. No check for free-list exhausted! Set sym
 * pointer in node to heap sym pointer.
 */
#ifdef DEBUG_HEAP
#define EXTRACTNODE(H,A) \
	(heap_sanity(H),(A)=(H)->free,(H)->free=GETLEFT(A),0)
#else /*!DEBUG_HEAP*/
#define EXTRACTNODE(H,A) \
	((A)=(H)->free,(H)->free=GETLEFT(A),0)
#endif /*DEBUG_HEAP*/

/* Allocate a new node from heap H, pop the pointer into A, return non-zero if
 * alloc failed. Node is uninitialised!
 */
#define NEWNODE(H,A) ( \
	(H)->free ?\
		EXTRACTNODE(H,A):\
		(((A)=heap_getmem(H)) ? 0 : -1)\
	)

typedef void *(*heap_safe_pointer_fn)( Heap *heap, PElement *, 
	void *, void *, void *, void * );
void *heap_safe_pointer( Heap *heap, heap_safe_pointer_fn fn, 
	void *a, void *b, void *c, void *d );

typedef void *(*heap_map_fn)( HeapNode *, void *, void *);
void *heap_map( HeapNode *hn, heap_map_fn fn, void *a, void *b );

int heap_sanity( Heap *heap );

void heap_check_all_destroyed( void );
void heap_destroy( Heap *heap );
GType heap_get_type( void );
Heap *heap_new( Compile *compile, heap_max_fn max_fn, int stsz, int rsz );
HeapNode *heap_getmem( Heap *heap );
gboolean heap_gc( Heap *heap );
void heap_gc_request( Heap *heap );
void heap_register_element( Heap *heap, Element *root );
void heap_unregister_element( Heap *heap, Element *root );
void heap_register_reduce( Heap *heap, Reduce *rc );
void heap_unregister_reduce( Heap *heap, Reduce *rc );
void heap_set( Heap *heap, NodeFlags setmask );
void heap_clear( Heap *heap, NodeFlags clearmask );
int heap_serial_new( Heap *heap );

gboolean heap_bool_new( Heap *heap, gboolean val, PElement *out );
gboolean heap_real_new( Heap *heap, double in, PElement *out );
gboolean heap_element_new( Heap *heap, Element *e, PElement *out );
gboolean heap_complex_element_new( Heap *heap, 
	PElement *rp, PElement *ip, PElement *out );
gboolean heap_complex_new( Heap *heap, double rp, double ip, PElement *out );
gboolean heap_realvec_new( Heap *heap, int n, double *vec, PElement *out );
gboolean heap_intvec_new( Heap *heap, int n, int *vec, PElement *out );
void heap_list_init( PElement *list );
gboolean heap_list_add( Heap *heap, PElement *list, PElement *data );
gboolean heap_list_next( PElement *list );
gboolean heap_list_cat( Reduce *rc, PElement *a, PElement *b, PElement *out );
void heap_appl_init( PElement *base, PElement *func );
gboolean heap_appl_add( Heap *heap, PElement *base, PElement *parm );
gboolean heap_matrix_new( Heap *heap, 
	int xsize, int ysize, double *vec, PElement *out );
gboolean heap_string_new( Heap *heap, const char *str, PElement *out );
gboolean heap_managedstring_new( Heap *heap, const char *str, PElement *out );
gboolean heap_lstring_new( Heap *heap, GSList *labels, PElement *out );
gboolean heap_file_new( Heap *heap, const char *filename, PElement *out );

gboolean heap_error_typecheck( PElement *e, 
	const char *name, const char *type );
typedef void *(*heap_map_list_fn)( PElement *, void *, void * );
void *heap_map_list( PElement *base, heap_map_list_fn fn, void *a, void *b );
typedef void *(*heap_map_dict_fn)( const char *, PElement *, void *a, void *b );
void *heap_map_dict( PElement *base, heap_map_dict_fn fn, void *a, void *b );

gboolean heap_get_list( PElement *list );
gboolean heap_get_list_next( PElement *list, PElement *data );
gboolean heap_get_string( PElement *base, char *buf, int n );
gboolean heap_get_lstring( PElement *base, GSList **labels );
gboolean heap_get_bool( PElement *base, gboolean *out );
gboolean heap_get_real( PElement *base, double *out );
gboolean heap_get_class( PElement *base, PElement *out );
gboolean heap_get_image( PElement *base, Imageinfo **out );
int heap_get_realvec( PElement *base, double *buf, int n );
int heap_get_imagevec( PElement *base, Imageinfo **buf, int n );
gboolean heap_get_matrix_size( PElement *base, int *xsize, int *ysize );
gboolean heap_get_matrix( PElement *base, 
	double *buf, int n, int *xsize, int *ysize );

gboolean heap_is_elist( PElement *base, gboolean *out );
gboolean heap_is_list( PElement *base, gboolean *out );
gboolean heap_is_string( PElement *base, gboolean *out );
gboolean heap_is_realvec( PElement *base, gboolean *out );
gboolean heap_is_matrix( PElement *base, gboolean *out );
gboolean heap_is_class( PElement *base, gboolean *out );
gboolean heap_is_instanceof_exact( const char *name, PElement *klass, 
	gboolean *out);
gboolean heap_is_instanceof( const char *name, PElement *klass, gboolean *out );

int heap_list_length( PElement *base );
int heap_list_length_max( PElement *base, int max_length );
gboolean heap_list_index( PElement *base, int n, PElement *out );
gboolean heap_reduce_strict( PElement *base );

gboolean heap_copy( Heap *heap, Compile *compile, PElement *out );

gboolean heap_ip_to_gvalue( PElement *in, GValue *out );
gboolean heap_gvalue_to_ip( GValue *in, PElement *out );

void graph_node( Heap *heap, VipsBuf *buf, HeapNode *root, gboolean fn );
void graph_pelement( Heap *heap, VipsBuf *buf, PElement *root, gboolean fn );
void graph_element( Heap *heap, VipsBuf *buf, Element *root, gboolean fn );
void graph_pointer( PElement *root );

/* Reduce and print, csh-style output.
 */
void graph_value( PElement *root );
