/* Header for reduction machine.
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

/* Huge :-( this pushes sizeof(Reduce) up to 14MB. But we need to be able to
 * loop down very long lists, eg. for a 65k x 3 LUT held as a Matrix. Drop
 * this down when we represent matricies more sensibly.
 */
#define SPINE_SIZE (80000)

/* Reduction machine state. Not very opaque ... see mark_reduce()
 */
struct _Reduce {
	/* Stack of heap nodes for spine.
	 */
	HeapNode *nstack[SPINE_SIZE];

	/* Index of free element above node stack top.
	 */
	int sp;

	/* Frame stack ... top of fstack is sp we block GET above.
	 */
	int fstack[SPINE_SIZE];

	/* Writeback stack ... where the result of each frame goes.
	 */
	PElement wbstack[SPINE_SIZE];

	/* Frame stack pointer.
	 */
	int fsp;

	/* Heap we evaluate.
	 */
	Heap *heap;

	/* Nested reductions ... need to be able to longjmp() out of stuff,
	 * and restore the machine state.
	 */
	int running;
	jmp_buf error[SPINE_SIZE];
	int sps[SPINE_SIZE];
	int fsps[SPINE_SIZE];
	int tsp[SPINE_SIZE];
};

#define RSPUSH(RC,N) { \
	if( (RC)->sp == SPINE_SIZE ) { \
		error_top( _( "Stack overflow." ) ); \
		error_sub( _( "Spine stack overflow, runaway recursion?" ) ); \
		reduce_throw( (RC) ); \
	} \
	else \
		(RC)->nstack[(RC)->sp++]=(N); \
}

/* Number of items in current frame.
 */
#define RSFRAMESIZE(RC) ((RC)->sp - (RC)->fstack[(RC)->fsp - 1])

/* Check for at least N args present.
 */
#define RSCHECKARGS(RC,N) (RSFRAMESIZE(RC) >= (N))

/* Frame is empty?
 */
#define RSFRAMEEMPTY(RC) (RSFRAMESIZE(RC) == 0)

/* Get offset from stack top, offset 0 is top item.
 */
#define RSGET(RC,N) ((RC)->nstack[(RC)->sp - ((N) + 1)])

/* Get the writeback for this frame.
 */
#define RSGETWB(RC) ((RC)->wbstack[(RC)->fsp - 1])

#define RSPUSHFRAME(RC,OUT) { \
	if( (RC)->fsp == SPINE_SIZE )  { \
		error_top( _( "Stack overflow." ) ); \
		error_sub( _( "Frame stack overflow, " \
			"expression too complex." ) ); \
		reduce_throw( (RC) ); \
	} \
	else { \
		(RC)->wbstack[(RC)->fsp] = *out; \
		(RC)->fstack[(RC)->fsp] = (RC)->sp; \
		(RC)->fsp++; \
	} \
}

#define RSPOPFRAME(RC) { \
	if( (RC)->fsp == 0 ) { \
		error_top( _( "Stack underflow." ) ); \
		error_sub( _( "Frame stack underflow, you've found a bug!" ) ); \
		reduce_throw( (RC) ); \
	} \
	else { \
		(RC)->fsp--; \
		(RC)->sp = (RC)->fstack[(RC)->fsp]; \
	} \
}

#define RSPOP(RC,N) { \
	if( !RSCHECKARGS(RC,N) ) { \
		error_top( _( "Stack underflow." ) ); \
		error_sub( _( "Spine stack underflow, you've found a bug!" ) ); \
		reduce_throw( (RC) ); \
	} \
	else \
		(RC)->sp -= (N); \
}

/* Pop this code before any calls to reduce_*() to init stuff and catch 
 * errors. Arg is function return value. The missing running decrement is done
 * by throw().
 */
#define REDUCE_CATCH_START( R ) \
{ \
	rc->sps[rc->running] = rc->sp; \
	rc->fsps[rc->running] = rc->fsp; \
	rc->tsp[rc->running] = trace_get_mark(); \
	if( setjmp( rc->error[rc->running++] ) ) { \
		g_assert( rc->running >= 0 ); \
		rc->sp = rc->sps[rc->running]; \
		rc->fsp = rc->fsps[rc->running]; \
		trace_pop_to( rc->tsp[rc->running] ); \
		return( (R) ); \
	} \
}

/* After any calls to reduce_*().
 */
#define REDUCE_CATCH_STOP \
{ \
	rc->running -= 1; \
	g_assert( rc->running >= 0 ); \
}

/* Util.
 */
void reduce_throw( Reduce *rc ) __attribute__((noreturn));

typedef void *(*reduce_safe_pointer_fn)( Reduce *rc, PElement *, 
	void *, void *, void *, void * );
void *reduce_safe_pointer( Reduce *rc, reduce_safe_pointer_fn fn, 
	void *a, void *b, void *c, void *d );

void reduce_error_typecheck( Reduce *rc, 
	PElement *e, const char *name, const char *type );
void reduce_list_get( Reduce *rc, PElement *base );
typedef void *(*reduce_map_list_fn)( Reduce *rc,
	PElement *, void *, void * );
void *reduce_map_list( Reduce *rc, 
	PElement *base, reduce_map_list_fn fn, void *a, void *b );
void reduce_clone_list( Reduce *rc, PElement *base, PElement *out );
int reduce_get_string( Reduce *rc, PElement *base, char *buf, int n );
int reduce_get_lstring( Reduce *rc, PElement *base, GSList **labels );
gboolean reduce_get_bool( Reduce *rc, PElement *base );
double reduce_get_real( Reduce *rc, PElement *base );
void reduce_get_class( Reduce *rc, PElement *base );
Imageinfo *reduce_get_image( Reduce *rc, PElement *base );
int reduce_get_realvec( Reduce *rc, PElement *base, double *buf, int n );
int reduce_get_imagevec( Reduce *rc, PElement *base, Imageinfo **buf, int n );
int reduce_get_matrix( Reduce *rc, 
	PElement *base, double *buf, int n, int *xsize, int *ysize );
void reduce_get_matrix_size( Reduce *rc, 
	PElement *base, int *xsize, int *ysize );
gboolean reduce_is_elist( Reduce *rc, PElement *base );
gboolean reduce_is_list( Reduce *rc, PElement *base );
gboolean reduce_is_string( Reduce *rc, PElement *base );
gboolean reduce_is_finitestring( Reduce *rc, PElement *base );
gboolean reduce_is_realvec( Reduce *rc, PElement *base );
gboolean reduce_is_matrix( Reduce *rc, PElement *base );
gboolean reduce_is_class( Reduce *rc, PElement *klass );
int reduce_list_length( Reduce *rc, PElement *base );
int reduce_list_length_max( Reduce *rc, PElement *base, int max_length );
void reduce_list_index( Reduce *rc, PElement *base, int n, PElement *out );
gboolean reduce_is_instanceof_exact( Reduce *rc, 
	const char *name, PElement *instance );
gboolean reduce_is_instanceof( Reduce *rc, 
	const char *name, PElement *instance );

/* Main.
 */
extern Reduce *reduce_context;
extern int reduce_total_recomputations;

void reduce_destroy( Reduce *rc );
Reduce *reduce_new( void );
gboolean reduce_regenerate( Expr *expr, PElement *out );
gboolean reduce_regenerate_member( Expr *expr, PElement *ths, PElement *out );

void reduce_spine( Reduce *rc, PElement *out );
void reduce_spine_strict( Reduce *rc, PElement *out );

gboolean reduce_pelement( Reduce *, ReduceFunction fn, PElement *out );

/* Register and unregister values.
 */
void reduce_register( Symbol *sym );
void reduce_unregister( Symbol *sym );
