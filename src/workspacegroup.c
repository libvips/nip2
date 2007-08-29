/* Group workspacegroup files together.
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

static ModelClass *parent_class = NULL;

static void
workspacegroup_dispose( GObject *gobject )
{
	Workspacegroup *wsg;

#ifdef DEBUG
	printf( "workspacegroup_dispose\n" );
#endif /*DEBUG*/

	g_return_if_fail( gobject != NULL );
	g_return_if_fail( IS_WORKSPACEGROUP( gobject ) );

	wsg = WORKSPACEGROUP( gobject );

	wsg->sym = NULL;

	G_OBJECT_CLASS( parent_class )->dispose( gobject );
}

static void
workspacegroup_child_add( iContainer *parent, iContainer *child, int pos )
{
	ICONTAINER_CLASS( parent_class )->child_add( parent, child, pos );

#ifdef DEBUG
	printf( "workspacegroup_child_add: added %s\n",
		IOBJECT( child )->name );
#endif /*DEBUG*/
}

static void
workspacegroup_child_remove( iContainer *parent, iContainer *child )
{
	ICONTAINER_CLASS( parent_class )->child_remove( parent, child );
}

static void
workspacegroup_class_init( WorkspacegroupClass *class )
{
	GObjectClass *gobject_class = (GObjectClass *) class;
	iContainerClass *icontainer_class = (iContainerClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	gobject_class->dispose = workspacegroup_dispose;

	icontainer_class->child_add = workspacegroup_child_add;
	icontainer_class->child_remove = workspacegroup_child_remove;
}

static void
workspacegroup_init( Workspacegroup *wsg )
{
	wsg->sym = NULL;
}

GType
workspacegroup_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( WorkspacegroupClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) workspacegroup_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Workspacegroup ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) workspacegroup_init,
		};

		type = g_type_register_static( TYPE_MODEL, 
			"Workspacegroup", &info, 0 );
	}

	return( type );
}

static void
workspacegroup_link( Workspacegroup *wsg, const char *name )
{
	Symbol *sym;

	iobject_set( IOBJECT( wsg ), name, NULL );

	wsg->sym = sym = symbol_new( symbol_root->expr->compile, name );
	sym->type = SYM_WORKSPACEGROUP;
	sym->wsg = wsg;
	sym->expr = expr_new( sym );
	(void) compile_new( sym->expr );
	symbol_made( sym );
}

Workspacegroup *
workspacegroup_new( const char *name )
{
	Workspacegroup *wsg;

	if( compile_lookup( symbol_root->expr->compile, name ) ) {
		error_top( _( "Name clash." ) );
		error_sub( _( "Can't create workspacegroup \"%s\". "
			"A symbol with that name already exists." ), name );
		return( NULL );
	}

	wsg = WORKSPACEGROUP( g_object_new( TYPE_WORKSPACEGROUP, NULL ) );
	workspacegroup_link( wsg, name );

	return( wsg );
}

/* Make up a new workspace name.
 */
void
workspacegroup_name_new( Workspacegroup *wsg, char *name )
{
	Compile *compile = wsg->sym->expr->compile;

	strcpy( name, "untitled" );
	while( compile_lookup( compile, name ) )
		increment_name( name );
}

/* Done button hit.
 */
static void
workspacegroup_workspace_new_done_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys )
{
	Workspacegroup *wsg = WORKSPACEGROUP( client );
	Stringset *ss = STRINGSET( iwnd );
	StringsetChild *name = stringset_child_get( ss, _( "Name" ) );
	StringsetChild *caption = stringset_child_get( ss, _( "Caption" ) );

	Workspace *ws;
	Mainw *mainw;

	char name_text[1024];
	char caption_text[1024];

	if( !get_geditable_name( name->entry, name_text, 1024 ) ||
		!get_geditable_string( caption->entry, caption_text, 1024 ) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	if( !(ws = workspace_new_blank( wsg, name_text )) ) {
		nfn( sys, IWINDOW_ERROR );
		return;
	}

	iobject_set( IOBJECT( ws ), NULL, caption_text );

	mainw = mainw_new( ws );
	gtk_widget_show( GTK_WIDGET( mainw ) );

	nfn( sys, IWINDOW_TRUE );
}

void
workspacegroup_workspace_new( Workspacegroup *wsg, GtkWidget *parent )
{
	GtkWidget *ss = stringset_new();
	char name[256];

	workspacegroup_name_new( wsg, name );
	stringset_child_new( STRINGSET( ss ), 
		_( "Name" ), name, _( "Set workspace name here" ) );
	stringset_child_new( STRINGSET( ss ), 
		_( "Caption" ), "", _( "Set workspace caption here" ) );

	iwindow_set_title( IWINDOW( ss ), _( "New Workspace" ) );
	idialog_set_callbacks( IDIALOG( ss ), 
		iwindow_true_cb, NULL, NULL, wsg );
	idialog_add_ok( IDIALOG( ss ), 
		workspacegroup_workspace_new_done_cb, _( "Create Workspace" ) );
	iwindow_set_parent( IWINDOW( ss ), parent );
	iwindow_build( IWINDOW( ss ) );

	gtk_widget_show( ss );
}

