/* Decls for imageinfo.c
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

/* Meta we attach for the filename we loaded from.
 */
#define ORIGINAL_FILENAME "original-filename"

/* Group imageinfo with this.
 */

#define TYPE_IMAGEINFOGROUP (imageinfogroup_get_type())
#define IMAGEINFOGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), \
		TYPE_IMAGEINFOGROUP, Imageinfogroup ))
#define IMAGEINFOGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), \
		TYPE_IMAGEINFOGROUP, ImageinfogroupClass))
#define IS_IMAGEINFOGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IMAGEINFOGROUP ))
#define IS_IMAGEINFOGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IMAGEINFOGROUP ))
#define IMAGEINFOGROUP_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), \
		TYPE_IMAGEINFOGROUP, ImageinfogroupClass ))

typedef struct _Imageinfogroup {
	iContainer parent_object;

	/* Hash from filename to list of imageinfo. We can't use the
	 * icontainer hash, since our filenames are not unique (we can have
	 * the same file loaded several times, if some other application is
	 * changing our files behind our back).
	 */
	GHashTable *filename_hash;
} Imageinfogroup;

typedef struct _ImageinfogroupClass {
	iContainerClass parent_class;

} ImageinfogroupClass;

GType imageinfogroup_get_type( void );
Imageinfogroup *imageinfogroup_new( void );

/* An image.
 */

#define TYPE_IMAGEINFO (imageinfo_get_type())
#define IMAGEINFO( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_IMAGEINFO, Imageinfo ))
#define IMAGEINFO_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_IMAGEINFO, ImageinfoClass))
#define IS_IMAGEINFO( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_IMAGEINFO ))
#define IS_IMAGEINFO_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_IMAGEINFO ))
#define IMAGEINFO_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_IMAGEINFO, ImageinfoClass ))

/* A fragment of an undo buffer.
 */
typedef struct _Undofragment {
	struct _Undobuffer *undo;	/* Main undo area */
	IMAGE *im;			/* Old area */
	Rect pos;			/* Where we took it from */
} Undofragment;

/* Hold a list of the above, a bounding box for this list and a link back to
 * the main imageinfo.
 */
typedef struct _Undobuffer {
	struct _Imageinfo *imageinfo;	/* Main paint area */
	GSList *frags;			/* List of paint fragments */
	Rect bbox;			/* Bounding box for frags */
} Undobuffer;

/* Attach one of these to any IMAGE we monitor. It has the same lifetime as
 * the IMAGE and gets zapped by the imageinfo on dispose. This lets us spot
 * IMAGE events after the holding Imageinfo has gone.
 */
typedef struct _ImageinfoIMAGE {
	IMAGE *im;
	Imageinfo *imageinfo;
} ImageinfoIMAGE;

/* A VIPS image wrapped up nicely.
 */
struct _Imageinfo {
	Managed parent_object;

	IMAGE *im;		/* Image we manage, LUT if delayed */
	IMAGE *mapped_im;	/* Cache image mapped-thru-lut here */
	IMAGE *identity_lut;	/* For base images, keep an id lut if poss */
	Imageinfo *underlying;	/* If we're a LUT, the image we are a LUT of */
	ImageinfoIMAGE *proxy;	/* Proxy for IMAGE callbacks */

	gboolean dfile;		/* delete_file on final close */

	gboolean from_file;	/* Set if ->name is a user file */
	time_t mtime;		/* What mtime was when we loaded this file */

	/* Exprs which are thought to have this image as their value. See
	 * expr_value_new().
	 */
	GSList *exprs;

	/* Set if we've checked with the user that it's OK to paint on this
	 * imageinfo.
	 */
	gboolean ok_to_paint;

	/* Undo/redo buffers.
	 */
	GSList *undo;			/* List of undo buffers */
	GSList *redo;			/* List of redo buffers */
	Undobuffer *cundo;		/* Current buffer */

	/* Have we attached progress stuff to this ii?
	 */
	gboolean monitored;

	/* If we're from a file, the timestamp on the file we loaded from ...
	 * used to spot changes.
	 */
	time_t check_mtime;
	guint check_tid;
};

typedef struct _ImageinfoClass {
	ManagedClass parent_class;

	/* An area of the screen needs repainting.
	 */
	void (*area_changed)( Imageinfo *, Rect * );

	/* An area of the image has been paintboxed ... invalidate caches and
	 * trigger area_changed.
	 */
	void (*area_painted)( Imageinfo *, Rect * );

	/* Update undo/redo button sensitivities.
	 */
	void (*undo_changed)( Imageinfo * );

	/* The underlying file has changed ... higher levels should try to 
	 * reload.
	 */
	void (*file_changed)( Imageinfo * );
} ImageinfoClass;

void *imageinfo_area_changed( Imageinfo *imageinfo, Rect *dirty );
void *imageinfo_area_painted( Imageinfo *imageinfo, Rect *dirty );

void *imageinfo_expr_remove( Expr *expr, Imageinfo *imageinfo );
void imageinfo_expr_add( Imageinfo *imageinfo, Expr *expr );
GSList *imageinfo_expr_which( Imageinfo *imageinfo );
IMAGE *imageinfo_get_underlying( Imageinfo *imageinfo );

GType imageinfo_get_type( void );
Imageinfo *imageinfo_new( Imageinfogroup *imageinfogroup, 
	Heap *heap, IMAGE *im, const char *name );
Imageinfo *imageinfo_new_temp( Imageinfogroup *imageinfogroup, 
	Heap *heap, const char *name, const char *mode );
Imageinfo *imageinfo_new_lut( Imageinfogroup *imageinfogroup, 
	Heap *heap, Imageinfo *imageinfo );
Imageinfo *imageinfo_new_modlut( Imageinfogroup *imageinfogroup, 
	Heap *heap, Imageinfo *imageinfo, IMAGE *im );
gboolean imageinfo_is_from_file( Imageinfo *imageinfo );
Imageinfo *imageinfo_new_input( Imageinfogroup *imageinfogroup, 
	GtkWidget *parent, Heap *heap, const char *name );

IMAGE *imageinfo_get( gboolean use_lut, Imageinfo *imageinfo );
gboolean imageinfo_same_underlying( Imageinfo *imageinfo[], int n );

gboolean imageinfo_write( Imageinfo *imageinfo, 
	GtkWidget *parent, const char *filename );
gboolean imageinfo_check_paintable( Imageinfo *imageinfo, 
	GtkWidget *parent, iWindowNotifyFn nfn, void *sys );

void imageinfo_note( Symbol *sym, Imageinfo *imageinfo );
void imageinfo_forget( Symbol *sym, Imageinfo *imageinfo );
GSList *imageinfo_which( Imageinfo *im );

void imageinfo_make_sub( Imageinfo *out, int n, Imageinfo **in );
void imageinfo_mark( Imageinfo *imageinfo );

Imageinfo *imageinfo_sym_image( Symbol *sym );

void imageinfo_undo_mark( Imageinfo *imageinfo );
gboolean imageinfo_undo( Imageinfo *imageinfo );
gboolean imageinfo_redo( Imageinfo *imageinfo );
void imageinfo_undo_clear( Imageinfo *imageinfo );

gboolean imageinfo_paint_line( Imageinfo *imageinfo, Imageinfo *ink, 
	int nib, int x1, int y1, int x2, int y2 );
gboolean imageinfo_paint_flood( Imageinfo *imageinfo, Imageinfo *ink,  
	int x, int y, gboolean blob );
gboolean imageinfo_paint_smudge( Imageinfo *imageinfo, 
	Rect *oper, int x1, int y1, int x2, int y2 );
gboolean imageinfo_paint_dropper( Imageinfo *imageinfo, Imageinfo *ink, 
	int x, int iy );
gboolean imageinfo_paint_rect( Imageinfo *imageinfo, Imageinfo *ink, 
	Rect *area );
gboolean imageinfo_paint_text( Imageinfo *imageinfo, 
	const char *font_name, const char *text, Rect *tarea );
gboolean imageinfo_paint_mask( Imageinfo *imageinfo, 
	Imageinfo *ink, Imageinfo *mask, int x, int y );

void imageinfo_to_text( Imageinfo *imageinfo, BufInfo *buf );
gboolean imageinfo_from_text( Imageinfo *imageinfo, const char *text );
void imageinfo_to_rgb( Imageinfo *imageinfo, double *rgb );
void imageinfo_from_rgb( Imageinfo *imageinfo, double *rgb );
void imageinfo_colour_edit( GtkWidget *parent, Imageinfo *imageinfo );

