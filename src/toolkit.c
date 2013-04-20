/* Manage toolkits and their display.
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

/*
#define DEBUG
 */

#include "ip.h"

static FilemodelClass *parent_class = NULL;

Tool *
toolkit_map( Toolkit *kit, tool_map_fn fn, void *a, void *b )
{
	return( (Tool *) icontainer_map( ICONTAINER( kit ), 
		(icontainer_map_fn) fn, a, b ) );
}

static void
toolkit_changed( iObject *iobject )
{
	/* If we change, signal change on our parent too (toolkitgroup) ...
	 * things like Program and Toolkitbrowser which need to spot any 
 	 * change to any kit can connect to that, rather than having to 
	 * connect to all kits independently.
	 */
	if( IS_ICONTAINER( iobject ) && ICONTAINER( iobject )->parent )
		iobject_changed( IOBJECT( ICONTAINER( iobject )->parent ) );
}

static void
toolkit_info( iObject *iobject, VipsBuf *buf )
{
	Toolkit *kit = TOOLKIT( iobject );

	IOBJECT_CLASS( parent_class )->info( iobject, buf );

	vips_buf_appendf( buf, "group = \"%s\"\n", IOBJECT( kit->kitg )->name );
}

static View *
toolkit_view_new( Model *model, View *parent )
{
	return( toolkitview_new() );
}

static gboolean
toolkit_save_text( Model *model, iOpenFile *of )
{
	if( icontainer_map( ICONTAINER( model ), 
		(icontainer_map_fn) model_save_text, of, NULL ) )
		return( FALSE );

	return( TRUE );
}

/* Load from an iOpenFile.
 */
static gboolean
toolkit_load_text( Model *model, Model *parent, iOpenFile *of )
{
	Toolkit *kit = TOOLKIT( model );
	int pos = icontainer_pos_last( ICONTAINER( model ) ) + 1;
	gboolean res;

	/* Load up definitions.
	 */
	filemodel_set_filename( FILEMODEL( model ), of->fname_real );
	attach_input_file( of );
	if( !(res = parse_toplevel( kit, pos )) ) 
		/* The sub won't have filename or line number: zap them in.
		 */
		error_sub( "%s:%d\n%s\n", 
			FILEMODEL( kit )->filename, input_state.lineno, 
			error_get_sub() );

#ifdef DEBUG
	(void) dump_kit( kit );
#endif /*DEBUG*/

	return( res );
}

static void
toolkit_class_init( ToolkitClass *class )
{
	iObjectClass *iobject_class = (iObjectClass *) class;
	ModelClass *model_class = (ModelClass *) class;
	FilemodelClass *filemodel_class = (FilemodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	/* Init methods.
	 */
	iobject_class->info = toolkit_info;
	iobject_class->changed = toolkit_changed;

	model_class->view_new = toolkit_view_new;
	model_class->save_text = toolkit_save_text;
	model_class->load_text = toolkit_load_text;

	filemodel_class->filetype = filesel_type_definition;
}

static void
toolkit_init( Toolkit *kit )
{
	kit->kitg = NULL;
	kit->pseudo = FALSE;
}

GType
toolkit_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( ToolkitClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) toolkit_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Toolkit ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) toolkit_init,
		};

		type = g_type_register_static( TYPE_FILEMODEL, 
			"Toolkit", &info, 0 );
	}

	return( type );
}

static void
toolkit_link( Toolkit *kit, Toolkitgroup *kitg, const char *name )
{
	iobject_set( IOBJECT( kit ), name, NULL );
	icontainer_child_add( ICONTAINER( kitg ), ICONTAINER( kit ), -1 );
	kit->kitg = kitg;
	filemodel_register( FILEMODEL( kit ) );
	if( name[0] == '_' )
		MODEL( kit )->display = FALSE;
	toolkitgroup_sort( kitg );
}

/* Find a kit by kit name.
 */
Toolkit *
toolkit_find( Toolkitgroup *kitg, const char *name )
{	
	return( (Toolkit *) icontainer_map( ICONTAINER( kitg ), 
		(icontainer_map_fn) iobject_test_name, (char *) name, NULL ) );
}

Toolkit *
toolkit_new( Toolkitgroup *kitg, const char *name )
{	
	Toolkit *kit;

#ifdef DEBUG
	printf( "toolkit_new: %s\n", name );
#endif /*DEBUG*/

	/* Exists already?
	 */
	if( (kit = toolkit_find( kitg, name )) ) 
		IDESTROY( kit );

	/* Make a new kit.
	 */
	kit = TOOLKIT( g_object_new( TYPE_TOOLKIT, NULL ) );
	toolkit_link( kit, kitg, name );

	return( kit );
}

Toolkit *
toolkit_new_filename( Toolkitgroup *kitg, const char *filename )
{
	char name[FILENAME_MAX];
	Toolkit *kit;

	name_from_filename( filename, name );
	kit = toolkit_new( kitg, name );
	filemodel_set_filename( FILEMODEL( kit ), filename );

	return( kit );
}

/* Load a file as a toolkit.
 */
Toolkit *
toolkit_new_from_file( Toolkitgroup *kitg, const char *filename )
{
	Toolkit *kit = toolkit_new_filename( kitg, filename );
	gboolean res;

	res = filemodel_load_all( FILEMODEL( kit ), MODEL( kitg ), 
		filename, NULL );
	filemodel_set_modified( FILEMODEL( kit ), FALSE );

	/* Don't remove the kit if load failed, we want to leave it so the 
	 * user can try to fix the problem.
	 */

	if( res )
		return( kit );
	else
		return( NULL );
}

/* Load from an iOpenFile.
 */
Toolkit *
toolkit_new_from_openfile( Toolkitgroup *kitg, iOpenFile *of )
{
	Toolkit *kit = toolkit_new_filename( kitg, of->fname );
	gboolean res;

	res = filemodel_load_all_openfile( FILEMODEL( kit ), 
		MODEL( kitg ), of );
	filemodel_set_modified( FILEMODEL( kit ), FALSE );

	/* Don't remove the kit if load failed, we want to leave it so the 
	 * user can try to fix the problem.
	 */

	if( res )
		return( kit );
	else
		return( NULL );
}

/* Look up a toolkit, make an empty one if not there.
 */
Toolkit *
toolkit_by_name( Toolkitgroup *kitg, const char *name )
{
	Toolkit *kit;

	if( !(kit = toolkit_find( kitg, name )) ) {
		char file[FILENAME_MAX];

		im_snprintf( file, FILENAME_MAX,
			"$SAVEDIR" G_DIR_SEPARATOR_S "start" G_DIR_SEPARATOR_S 
			"%s.def", 
			name );
		kit = toolkit_new_filename( kitg, file );
	}

	return( kit );
}
