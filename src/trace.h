/* Decls for trace.c ... a trace window
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

#define TYPE_TRACE (trace_get_type())
#define TRACE( obj ) (GTK_CHECK_CAST( (obj), TYPE_TRACE, Trace ))
#define TRACE_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_TRACE, TraceClass ))
#define IS_TRACE( obj ) (GTK_CHECK_TYPE( (obj), TYPE_TRACE ))
#define IS_TRACE_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_TRACE ))

/* The various things we can trace.
 */
typedef enum {
	TRACE_BUILTIN = 1,		/* Calls to built in functions */
	TRACE_OPERATOR = 2,		/* +, -, etc. */
	TRACE_CLASS_NEW = 4,		/* Class construction */
	TRACE_VIPS = 8			/* VIPS operations */
} TraceFlags;

struct _Trace {
	Log parent_class;

	TraceFlags flags;
};

typedef struct _TraceClass {
	LogClass parent_class;

	/* My methods.
	 */
} TraceClass;

extern TraceFlags trace_flags;

void trace_block( void );
void trace_unblock( void );

void trace_reset( void );
void trace_check( void );
VipsBuf *trace_push( void );
void trace_pop( void );
VipsBuf *trace_current( void );
void trace_pop_to( int n );
int trace_get_mark( void );

GtkType trace_get_type( void );
Trace *trace_new( void );

void trace_text( TraceFlags flags, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));

void trace_pelement( PElement *pe );
void trace_node( HeapNode *node );
void trace_args( HeapNode **arg, int n );
void trace_binop( Compile *compile, 
	PElement *left, BinOp bop, PElement *right );
void trace_uop( UnOp uop, PElement *arg );
void trace_result( TraceFlags flags, PElement *out );

