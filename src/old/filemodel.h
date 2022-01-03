/* abstract base class for things which are loaded or saved from files
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

#define FILEMODEL_LOAD_STATE( obj ) ((FilemodelLoadState *) obj)

#define TYPE_FILEMODEL (filemodel_get_type())
#define FILEMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_FILEMODEL, Filemodel ))
#define FILEMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_FILEMODEL, FilemodelClass))
#define IS_FILEMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_FILEMODEL ))
#define IS_FILEMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_FILEMODEL ))
#define FILEMODEL_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_FILEMODEL, FilemodelClass ))

struct _Filemodel {
	Model model;

        char *filename;         /* File we read this thing from */
	gboolean modified;	/* Set if modified (and should be saved) */
	gboolean registered;	/* Set if on list of things to save on quit */
	gboolean auto_load;	/* TRUE if loaded from path_start */

	int x_off, y_off;	/* Save offset for things below this */

	/* When we loaded this filemodel, the version numbers we saw in the
	 * XML file.
	 */
	gboolean versioned;	/* Set means from a versioned file */
	int major;
	int minor;
	int micro;

	iWindow *window_hint;	/* Our views set this as a hint */
};

typedef struct _FilemodelClass {
	ModelClass parent_class;

	/* 

		top_load	top level load function ... controls how the
				rest of the load happens ... eg. merge,
				rename, etc.

		set_modified	set/clear the modified state

		top_save	top level save ... intercept this to override

	 */

	gboolean (*top_load)( Filemodel *filemodel, 
		ModelLoadState *state, Model *parent, xmlNode *xnode );
	void (*set_modified)( Filemodel *filemodel, gboolean modified );
	gboolean (*top_save)( Filemodel *filemodel, const char *filename );

	FileselFileType **filetype;
	const char *filetype_pref;
} FilemodelClass;

void filemodel_register( Filemodel *filemodel );
void filemodel_unregister( Filemodel *filemodel );

void *filemodel_top_load( Filemodel *filemodel, 
	ModelLoadState *state, Model *parent, xmlNode *xnode );

void filemodel_set_filename( Filemodel *filemodel, const char *filename );
void filemodel_set_modified( Filemodel *filemodel, gboolean state );
void filemodel_set_window_hint( Filemodel *filemodel, iWindow *iwnd );
iWindow *filemodel_get_window_hint( Filemodel *filemodel );

GType filemodel_get_type( void );

void filemodel_set_offset( Filemodel *filemodel, int x_off, int y_off );
gboolean filemodel_top_save( Filemodel *filemodel, const char *filename );
gboolean filemodel_load_all( Filemodel *filemodel, Model *parent, 
	const char *filename, const char *filename_user );
gboolean filemodel_load_all_openfile( Filemodel *filemodel, 
	Model *parent, iOpenFile *of );

void filemodel_inter_saveas( iWindow *parent, Filemodel *filemodel );
void filemodel_inter_save( iWindow *parent, Filemodel *filemodel );
void filemodel_inter_savenempty_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys );
void filemodel_inter_savenempty( iWindow *parent, Filemodel *filemodel );
void filemodel_inter_savenclose_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys );
void filemodel_inter_savenclose( iWindow *parent, Filemodel *filemodel );
void filemodel_inter_loadas( iWindow *parent, Filemodel *filemodel );
void filemodel_inter_replace( iWindow *parent, Filemodel *filemodel );

void filemodel_inter_close_registered_cb( iWindow *iwnd, void *client, 
	iWindowNotifyFn nfn, void *sys );

void filemodel_set_auto_load( Filemodel *filemodel );
