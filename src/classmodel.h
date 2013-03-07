/* like a heapmodel, but we represent a class in the heap
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

/* Member types we automate.
 */
typedef enum {
	CLASSMODEL_MEMBER_INT,
	CLASSMODEL_MEMBER_ENUM,		/* Like int, but extent has max value */
	CLASSMODEL_MEMBER_BOOLEAN,
	CLASSMODEL_MEMBER_DOUBLE,
	CLASSMODEL_MEMBER_STRING,
	CLASSMODEL_MEMBER_STRING_LIST,
	CLASSMODEL_MEMBER_REALVEC_FIXED,/* Eg. Colour's triplet */
	CLASSMODEL_MEMBER_MATRIX,
	CLASSMODEL_MEMBER_OPTIONS,
	CLASSMODEL_MEMBER_IMAGE
} ClassmodelMemberType;

/* A matrix value.
 */
typedef struct _MatrixValue {
	double *coeff;			/* Base coeffs */
	int width;			/* Size of matrix */
	int height;
} MatrixValue;

/* An image value.
 */
typedef struct {
	Imageinfo *ii;

	/* Can get "changed" for reload if the file changes behind our backs.
	 * Recalc the classmodel if this happens.
	 */
	guint file_changed_sid;
	Classmodel *classmodel;
} ImageValue;

/* A member needing automation.
 */
typedef struct {
	ClassmodelMemberType type;
	void *details;			/* eg. the set of allowed options */
	int extent;			/* Vector length, enum max, etc. */

	const char *member_name;	/* Name as known in nip class defs */
	const char *save_name;		/* As known in save files */
	const char *user_name;		/* i18n'd name for dialogs */

	guint offset;			/* Struct offset */
} ClassmodelMember;

#define TYPE_CLASSMODEL (classmodel_get_type())
#define CLASSMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_CLASSMODEL, Classmodel ))
#define CLASSMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_CLASSMODEL, ClassmodelClass))
#define IS_CLASSMODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_CLASSMODEL ))
#define IS_CLASSMODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_CLASSMODEL ))
#define CLASSMODEL_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_CLASSMODEL, ClassmodelClass ))

struct _Classmodel {
	Heapmodel parent_class;

	/* Set if we have graphic mods applied which should be saved.
	 */
	gboolean edited;

	/* xtras for Region/Arrow/etc.
	 */
	GSList *iimages;	/* All the iimage we are defined on */
	GSList *views;		/* All the regionview we have made */

	/* For things which have been loaded or saved from files (eg. image
	 * and matrix). Used to set the filename for the "save" dialog.
	 */
	char *filename;
};

typedef struct _ClassmodelClass {
	HeapmodelClass parent_class;

	/* Get a pointer to the class instance vars ... just used by
	 * iarrow/iregion for code sharing.
	 */
	void *(*get_instance)( Classmodel * );

	/* Read from heap into model, and create new heap class from model.
	 */
	gboolean (*class_get)( Classmodel *, PElement *root );
	gboolean (*class_new)( Classmodel *, PElement *fn, PElement *out );

	/* Save and replace graphic displays ... eg. image/matrix.
	 */
	gboolean (*graphic_save)( Classmodel *, GtkWidget *, const char * );
	gboolean (*graphic_replace)( Classmodel *, GtkWidget *, const char * );

	FileselFileType **filetype;
	const char *filetype_pref;

	ClassmodelMember *members;
	int n_members;
	void (*reset)( Classmodel * );
} ClassmodelClass;

void image_value_init( ImageValue *image, Classmodel *classmodel );
void image_value_destroy( ImageValue *image );
void image_value_set( ImageValue *image, Imageinfo *ii );
void image_value_caption( ImageValue *value, VipsBuf *buf );

void *classmodel_get_instance( Classmodel *classmodel );
void classmodel_graphic_save( Classmodel *classmodel, GtkWidget *parent );
void classmodel_graphic_replace( Classmodel *classmodel, GtkWidget *parent );

void *classmodel_iimage_unlink( Classmodel *classmodel, iImage *iimage );
void classmodel_iimage_update( Classmodel *classmodel, Imageinfo *ii );

GType classmodel_get_type( void );

void classmodel_update( Classmodel *classmodel );
void classmodel_set_edited( Classmodel *classmodel, gboolean edited );

Classmodel *classmodel_new_classmodel( GType type, Rhs *rhs );
