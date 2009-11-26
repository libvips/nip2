/* Declarations for ifileselect.c
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

/* How we define a file type. Pass a NULL-terminated array of pointers
 * to these puppies to gtk_file_selection2_set_file_types().
 * gtk_file_selection2_set_file_types() makes a copy of the data itself,
 * so you can free if you want.
 */
typedef struct _FileselFileType {
  /* Descriptive name for this file type. Eg:
   *	"TIFF image file (*.tif; *.tiff)"
   */
  const char *name;

  /* NULL-terminated array of suffixes identifying this 
   * file type. Put the default first. Eg:
   *	{ ".tif", ".tiff", NULL }, or 
   *	{ ".htm", ".html", NULL }
   */
  const char **suffixes;
} FileselFileType;

/* Basic types.
 */
extern FileselFileType 
	filesel_wfile_type, filesel_rfile_type, 
	filesel_mfile_type, filesel_cfile_type, filesel_xfile_type, 
	filesel_dfile_type, filesel_ifile_type;

/* Suffix sets we support.
 */
extern FileselFileType *filesel_type_definition[];
extern FileselFileType *filesel_type_workspace[];
extern FileselFileType *filesel_type_matrix[];
extern FileselFileType **filesel_type_image;
extern FileselFileType **filesel_type_mainw;
extern FileselFileType **filesel_type_any;

/* Subclass off gtkfilesel2.c to make one of our fileselectors.
 */
#define TYPE_FILESEL (filesel_get_type())
#define FILESEL( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_FILESEL, Filesel ))
#define FILESEL_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_FILESEL, FileselClass ))
#define IS_FILESEL( obj ) (GTK_CHECK_TYPE( (obj), TYPE_FILESEL ))
#define IS_FILESEL_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_FILESEL ))

/* Must be enough. 
 */
#define FILESEL_MAX_FILTERS (100)

typedef struct _Filesel {
	iDialog parent;

        /* Widgets.
         */
        GtkWidget *chooser;             /* Filechooser widget */
        GtkWidget *space;               /* Space available */
        GtkWidget *info;                /* File info */
	GtkFileFilter *filter[FILESEL_MAX_FILTERS];	

	/* State. 
	 */
	gboolean incr;			/* True for increment filename */
	gboolean imls;			/* True if this is image load/save */
	gboolean save;			/* True if this is a save dialog */
	gboolean multi;			/* Multiple-select */
	gboolean start_name;		/* True if we have a suggested name */

	FileselFileType **type;		/* Allowable types for this filesel */
	int ntypes;
	int default_type;
	const char *type_pref;		/* Pref to set on type change */

	/* Last dir we entered. Used to stop dir_changed being emitted too
	 * often.
	 */
	char *current_dir;

	iWindowFn done_cb;		/* On OK */
	void *client;
} Filesel;

typedef struct _FileselClass {
	iDialogClass parent_class;

	/* My methods.
	 */
} FileselClass;

void filesel_startup( void );

gboolean is_file_type( const FileselFileType *type, const char *filename );

typedef void *(*FileselMapFn)( Filesel *, const char *, void *, void * );

void filesel_add_mode( char *filename );

GtkType filesel_get_type( void );
GtkWidget *filesel_new( void );

gboolean filesel_set_filename( Filesel *filesel, const char *name );
char *filesel_get_filename( Filesel *filesel );
int filesel_nselected( Filesel *filesel );
void *filesel_map_filename_multi( Filesel *filesel,
	FileselMapFn fn, void *a, void *b );

void filesel_set_done( Filesel *filesel, iWindowFn done_cb, void *client );
void filesel_set_filetype( Filesel *filesel, 
	FileselFileType **type, int default_type );
void filesel_set_filetype_pref( Filesel *filesel, const char *type_pref );
int filesel_get_filetype( Filesel *filesel ); 
void filesel_make_patt( FileselFileType *type, VipsBuf *patt );
void filesel_set_flags( Filesel *filesel, gboolean imls, gboolean save );
void filesel_set_multi( Filesel *filesel, gboolean multi );
