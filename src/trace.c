/* trace window
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

/*
#define DEBUG
 */

#include "ip.h"

/* OR of the flags in all the trace windows.
 */
TraceFlags trace_flags = (TraceFlags) 0;

static LogClass *parent_class = NULL;

/* All trace windows.
 */
static GSList *trace_all = NULL;

/* Trace buffer stack.
 */
static BufInfo trace_buffer_stack[SPINE_SIZE];
static int trace_buffer_stack_p = 0;

/* Number of active trace blocks.
 */
static int trace_block_count = 0;

/* All the trace menus we have.
 */
typedef struct _TraceTypeMenu {
	const char *name;
	TraceFlags flag;
} TraceTypeMenu;

/* Map action names to trace flags for the radio menu.
 */
static const TraceTypeMenu trace_types[] = {
	{ "Operator", TRACE_OPERATOR },
	{ "Builtin", TRACE_BUILTIN },
	{ "Class", TRACE_CLASS_NEW },
	{ "VIPS", TRACE_VIPS }
};

static TraceFlags
trace_get_trace_flag( GtkAction *action )
{
	const char *name = gtk_action_get_name( action );

	int i;

	for( i = 0; i < IM_NUMBER( trace_types ); i++ )
		if( strcmp( name, trace_types[i].name ) == 0 )
			return( trace_types[i].flag );

	g_assert( FALSE );
}

void
trace_block( void )
{
	trace_block_count += 1;
}

void
trace_unblock( void )
{
	trace_block_count -= 1;

	g_assert( trace_block_count >= 0 );
}

void
trace_reset( void )
{
	int i;

	for( i = 0; i < trace_buffer_stack_p; i++ )
		buf_destroy( &trace_buffer_stack[i] );

	trace_buffer_stack_p = 0;
}

void
trace_check( void )
{
	g_assert( trace_buffer_stack_p == 0 );
}

BufInfo *
trace_push( void )
{
	int i;

#ifdef DEBUG
	printf( "trace_push: %d\n", trace_buffer_stack_p );
#endif 

	if( trace_buffer_stack_p >= SPINE_SIZE ) {
		error_top( _( "Overflow error." ) );
		error_sub( _( "Trace buffer stack overflow." ) );
		reduce_throw( reduce_context );
	}

	i = trace_buffer_stack_p++;
	buf_init_dynamic( &trace_buffer_stack[i], MAX_TRACE );

	return( &trace_buffer_stack[i] );
}

void
trace_pop( void )
{
	int i;

#ifdef DEBUG
	printf( "trace_pop: %d\n", trace_buffer_stack_p );
#endif 

	g_assert( trace_buffer_stack_p > 0 );

	i = --trace_buffer_stack_p;
	buf_destroy( &trace_buffer_stack[i] );
}

BufInfo *
trace_current( void )
{
	g_assert( trace_buffer_stack_p > 0 );

	return( &trace_buffer_stack[trace_buffer_stack_p - 1] );
}

int
trace_get_mark( void )
{
	return( trace_buffer_stack_p );
}

void
trace_pop_to( int n )
{
	g_assert( n >= 0 && n <= trace_buffer_stack_p );

	while( trace_buffer_stack_p > n ) 
		trace_pop();
}

static void *
trace_global_rethink_sub( Trace *trace )
{
	trace_flags |= trace->flags;

	return( NULL );
}

/* Rethink the global trace_flags.
 */
static void
trace_global_rethink( void )
{
	trace_flags = 0;

	slist_map( trace_all, (SListMapFn) trace_global_rethink_sub, NULL );
}

static void
trace_destroy( GtkObject *object )
{
	Trace *trace;

	g_return_if_fail( object != NULL );
	g_return_if_fail( IS_TRACE( object ) );

	trace = TRACE( object );

	/* My instance destroy stuff.
	 */

	trace_all = g_slist_remove( trace_all, trace );

	GTK_OBJECT_CLASS( parent_class )->destroy( object );

	trace_global_rethink();
}

static void
trace_view_action_cb( GtkToggleAction *action, Trace *trace )
{
	TraceFlags flag = trace_get_trace_flag( GTK_ACTION( action ) );

	if( gtk_toggle_action_get_active( action ) ) 
		trace->flags |= flag;
	else
		trace->flags &= flag ^ ((TraceFlags) -1);

	trace_global_rethink();
}

/* Our actions.
 */
static GtkActionEntry trace_actions[] = {
	/* Menu items.
	 */
	{ "FileMenu", NULL, "_File" },
	{ "ViewMenu", NULL, "_View" },
	{ "HelpMenu", NULL, "_Help" },

	/* Actions.
	 */
	{ "Clear", 
		NULL, N_( "_Clear" ), NULL, 
		N_( "Clear trace window" ), 
		G_CALLBACK( log_clear_action_cb ) },

	{ "Close", 
		GTK_STOCK_CLOSE, N_( "_Close" ), NULL, 
		N_( "Close trace window" ), 
		G_CALLBACK( iwindow_kill_action_cb ) },

	{ "Guide", 
		GTK_STOCK_HELP, N_( "_Contents" ), "F1",
		N_( "Open the users guide" ), 
		G_CALLBACK( mainw_guide_action_cb ) },

	{ "About", 
		NULL, N_( "_About" ), NULL,
		N_( "About this program" ), 
		G_CALLBACK( mainw_about_action_cb ) }
};

static GtkToggleActionEntry trace_toggle_actions[] = {
	{ "Operator",
		NULL, N_( "_Operators" ), NULL,
		N_( "trace operators" ),
		G_CALLBACK( trace_view_action_cb ), FALSE },

	{ "Builtin",
		NULL, N_( "_Builtin Functions" ), NULL,
		N_( "trace calls to built in functions" ),
		G_CALLBACK( trace_view_action_cb ), FALSE },

	{ "Class",
		NULL, N_( "_Class Construction" ), NULL,
		N_( "trace class constructors" ),
		G_CALLBACK( trace_view_action_cb ), FALSE },

	{ "VIPS",
		NULL, N_( "_VIPS Operations" ), NULL,
		N_( "trace calls to VIPS" ),
		G_CALLBACK( trace_view_action_cb ), FALSE }
};

static const char *trace_menubar_ui_description =
"<ui>"
"  <menubar name='TraceMenubar'>"
"    <menu action='FileMenu'>"
"      <menuitem action='Clear'/>"
"      <separator/>"
"      <menuitem action='Close'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='Operator'/>"
"      <menuitem action='Builtin'/>"
"      <menuitem action='Class'/>"
"      <menuitem action='VIPS'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
"      <menuitem action='Guide'/>"
"      <menuitem action='About'/>"
"    </menu>"
"  </menubar>"
"</ui>";

static void
trace_class_init( TraceClass *class )
{
	GtkObjectClass *object_class = (GtkObjectClass *) class;
	LogClass *log_class = (LogClass *) class;

	parent_class = g_type_class_peek_parent( class );

	object_class->destroy = trace_destroy;

	log_class->actions = trace_actions;
	log_class->n_actions = IM_NUMBER( trace_actions );
	log_class->toggle_actions = trace_toggle_actions;
	log_class->n_toggle_actions = IM_NUMBER( trace_toggle_actions );
	log_class->action_name = "TraceActions";
	log_class->ui_description = trace_menubar_ui_description;
	log_class->menu_bar_name = "/TraceMenubar";
}

static void
trace_init( Trace *trace )
{
	trace->flags = 0;
}

GtkType
trace_get_type( void )
{
	static GtkType type = 0;

	if( !type ) {
		static const GtkTypeInfo info = {
			"Trace",
			sizeof( Trace ),
			sizeof( TraceClass ),
			(GtkClassInitFunc) trace_class_init,
			(GtkObjectInitFunc) trace_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		type = gtk_type_unique( TYPE_LOG, &info );
	}

	return( type );
}

static void
trace_link( Trace *trace )
{
        iwindow_set_title( IWINDOW( trace ), _( "Trace" ) );
	gtk_window_set_default_size( GTK_WINDOW( trace ), 640, 480 );
	iwindow_set_size_prefs( IWINDOW( trace ), 
		"TRACE_WIDTH", "TRACE_HEIGHT" );
	iwindow_build( IWINDOW( trace ) );
	trace_all = g_slist_prepend( trace_all, trace );

	gtk_widget_show( GTK_WIDGET( trace ) ); 
}

Trace *
trace_new( void )
{
	Trace *trace = gtk_type_new( TYPE_TRACE );

	trace_link( trace );

	return( trace );
}

static void *
trace_text_sub( Trace *trace, const char *buf, TraceFlags flags )
{
	if( !trace_block_count && trace->flags & flags ) 
		log_text( LOG( trace ), buf );

	return( NULL );
}

void
trace_text( TraceFlags flags, const char *fmt, ... )
{
	va_list ap;
 	char buf[MAX_STRSIZE];

	if( !(trace_flags & flags) )
		return;

        va_start( ap, fmt );
        (void) im_vsnprintf( buf, MAX_STRSIZE, fmt, ap );
        va_end( ap );

	slist_map2( trace_all, 
		(SListMap2Fn) trace_text_sub, buf, (void *) flags );
}

void
trace_pelement( PElement *pe )
{
	BufInfo *buf = trace_current();
	Heap *heap = reduce_context->heap;

	graph_pelement( heap, buf, pe, TRACE_FUNCTIONS );
}

void
trace_node( HeapNode *node )
{
	Element e;
	PElement pe;

	PEPOINTE( &pe, &e );
	PEPUTP( &pe, ELEMENT_NODE, node );
	trace_pelement( &pe );
}

void
trace_args( HeapNode **arg, int n )
{
	BufInfo *buf = trace_current();
	int i;

	for( i = n - 1; i >= 0; i-- ) {
		PElement rhs;

		PEPOINTRIGHT( arg[i], &rhs );
		trace_pelement( &rhs ); 
		buf_appends( buf, " " ); 
	}

	buf_appendf( buf, "->\n" ); 
}

void
trace_binop( Compile *compile, PElement *left, BinOp bop, PElement *right )
{
	BufInfo *buf = trace_current();

	buf_appendf( buf, "\"%s\" ", decode_BinOp( bop ) );
	trace_pelement( left );
	buf_appends( buf, " " );
	trace_pelement( right );
	buf_appends( buf, " -> (" );
	compile_name( compile, buf );
	buf_appends( buf, ")\n" );
}

void
trace_uop( UnOp uop, PElement *arg )
{
	BufInfo *buf = trace_current();

	buf_appendf( buf, "\"%s\" ", decode_UnOp( uop ) );
	trace_pelement( arg );
	buf_appends( buf, " ->\n" ); 
}

void
trace_result( TraceFlags flags, PElement *out )
{
	BufInfo *buf = trace_current();

	buf_appendf( buf, "    " ); 
	trace_pelement( out );
	buf_appends( buf, "\n" ); 

	trace_text( flags, "%s", buf_all( buf ) ); 
}
