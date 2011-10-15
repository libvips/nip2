/* an input group ... put/get methods
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

static ValueClass *parent_class = NULL;

static gboolean
group_save_list( PElement *list, char *filename );

/* Exported, since main.c uses this to save 'main' to a file.
 */
gboolean
group_save_item( PElement *item, char *filename )
{
	gboolean result;
	Imageinfo *ii;

	if( !heap_is_instanceof( CLASS_GROUP, item, &result ) )
		return( FALSE );
	if( result ) {
		PElement value;

		if( !class_get_member( item, MEMBER_VALUE, NULL, &value ) ||
			!group_save_list( &value, filename ) )
			return( FALSE );
	}

	if( !heap_is_instanceof( CLASS_IMAGE, item, &result ) )
		return( FALSE );
	if( result ) {
		char filename_image[MAX_STRSIZE];
		PElement value;

		strcpy( filename_image, filename );
		filesel_add_mode( filename_image );

		if( !class_get_member( item, MEMBER_VALUE, NULL, &value ) || 
			!heap_get_image( &value, &ii ) ||
			!imageinfo_write( ii, filename_image ) )
			return( FALSE );

		increment_filename( filename );
	}

	if( !heap_is_instanceof( CLASS_MATRIX, item, &result ) )
		return( FALSE );
	if( result ) {
		DOUBLEMASK *dmask;

		if( !(dmask = matrix_ip_to_dmask( item )) )
			return( FALSE );
		if( im_write_dmask_name( dmask, filename ) ) {
			error_vips_all();
			IM_FREEF( im_free_dmask, dmask );

			return( FALSE );
		}
		IM_FREEF( im_free_dmask, dmask );

		increment_filename( filename );
	}

	if( PEISIMAGE( item ) ) {
		char filename_image[MAX_STRSIZE];

		strcpy( filename_image, filename );
		filesel_add_mode( filename_image );

		if( !heap_get_image( item, &ii ) ||
			!imageinfo_write( ii, filename_image ) )
			return( FALSE );

		increment_filename( filename );
	}

	if( PEISLIST( item ) ) {
		if( !group_save_list( item, filename ) )
			return( FALSE );
	}

	return( TRUE );
}

static gboolean
group_save_list( PElement *list, char *filename )
{
	int i;
	int length;

	if( (length = heap_list_length( list )) < 0 ) 
		return( FALSE );

	for( i = 0; i < length; i++ ) {
		PElement item;

		if( !heap_list_index( list, i, &item ) ||
			!group_save_item( &item, filename ) )
			return( FALSE );
	}

	return( TRUE );
}

static gboolean
group_graphic_save( Classmodel *classmodel, 
	GtkWidget *parent, const char *filename )
{
	Group *group = GROUP( classmodel );
	Row *row = HEAPMODEL( group )->row;
	PElement *root = &row->expr->root;
	char buf[FILENAME_MAX];

	/* We are going to increment the filename ... make sure there's some
	 * space at the end of the string.
	 */
	im_strncpy( buf, filename, FILENAME_MAX - 5 );

	if( !group_save_item( root, buf ) )
		return( FALSE );

	return( TRUE );
}

static void
group_class_init( GroupClass *class )
{
	ClassmodelClass *classmodel_class = (ClassmodelClass *) class;

	parent_class = g_type_class_peek_parent( class );

	/* Create signals.
	 */

	classmodel_class->graphic_save = group_graphic_save;

	model_register_loadable( MODEL_CLASS( class ) );
}

static void
group_init( Group *group )
{
	iobject_set( IOBJECT( group ), CLASS_GROUP, NULL );
}

GType
group_get_type( void )
{
	static GType type = 0;

	if( !type ) {
		static const GTypeInfo info = {
			sizeof( GroupClass ),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) group_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof( Group ),
			32,             /* n_preallocs */
			(GInstanceInitFunc) group_init,
		};

		type = g_type_register_static( TYPE_VALUE, 
			"Group", &info, 0 );
	}

	return( type );
}
