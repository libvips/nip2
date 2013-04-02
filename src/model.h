/* abstract base class for things which form the model of a model/view pair 
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

/* When scrolling, do we want the top or the bottom of the object visible.
 * Important for Columns, since we sometimes want to see the title bar and
 * sometimes the edit box at the bottom.
 */
typedef enum {
	MODEL_SCROLL_TOP,
	MODEL_SCROLL_BOTTOM
} ModelScrollPosition;

/* How to rename symbols.
 */
typedef struct _ModelRename {
	char *old_name;
	char *new_name;
} ModelRename;

/* What we track during a load operation.
 */
typedef struct _ModelLoadState {
	char *filename;		/* Name we loaded from */
	char *filename_user;	/* The filename to record in the model */
	xmlDoc *xdoc;		/* Document we load from */

	/* 

		FIXME ... a linked list? try a hash sometime
		see model_loadstate_rewrite_name()

		would probably only see a speedup for merging large
		workspaces, not something we do often

	 */
	GSList *renames;	/* Rename table for this load context */

	/* Version info we read from this XML file.
	 */
	int major;
	int minor;
	int micro;

	/* Log error messages here.
	 */
	char error_log_buffer[MAX_STRSIZE];
	VipsBuf error_log;

	/* Set this bool to rewrite string constants using the filename
	 * rewrite system.
	 */
	gboolean rewrite_path;

	/* The old_dir we added with path_rewrite_add() ... if non, NULL,
	 * unset this rewrite rule on close.
	 */
	char *old_dir;
} ModelLoadState;

#define TYPE_MODEL (model_get_type())
#define MODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_MODEL, Model ))
#define MODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_MODEL, ModelClass))
#define IS_MODEL( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_MODEL ))
#define IS_MODEL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_MODEL ))
#define MODEL_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_MODEL, ModelClass ))

struct _Model {
	iContainer parent_object;

	/* My instance vars.
	 */
	gboolean display;	/* This model should have a view */

	/* For things that have a pop-up window (eg. iimage, plot), the
	 * position and size of the window.
	 */
	int window_x, window_y;
	int window_width, window_height;
};

typedef struct _ModelClass {
	iContainerClass parent_class;

	/* Build display methods.

		view_new	make a view for this model ... make the top
				view yourself, thereafter view will watch
				child_add etc. and manage subviews
				automatically ... use model->display to create
				and destroy views

	 */

	View *(*view_new)( Model *model, View *parent );

	/* Change methods

		edit		open an editor on the model

		header		view model header 

		scrollto	try to make views visible

		reset		signals views to reset ... eg. textview pops
				back to whatever the ws says it should be
				displaying (value or formula)

		layout 		try to lay child view out

		front		trigger view_child_front() for all views

	 */

	void (*edit)( GtkWidget *, Model * );
	void (*header)( GtkWidget *, Model * );
	void (*scrollto)( Model *, ModelScrollPosition );
	void (*reset)( Model * );
	void (*layout)( Model * );
	void (*front)( Model * );

	/* Load and save methods.

		save		write model as child of node

		save_test	predicate ... save model if save_test is 
				defined and true

		save_text	plain text save ... eg. for toolkits

		load		_init() model from xmlNode

		load_text	_init() from plain text ... eg. toolkit

		empty		remove contents of model

	 */
	xmlNode *(*save)( Model *, xmlNode * );
	gboolean (*save_test)( Model * );
	gboolean (*save_text)( Model *, iOpenFile * );
	gboolean (*load)( Model *model, 
		ModelLoadState *state, Model *parent, xmlNode *xnode );
	gboolean (*load_text)( Model *model, Model *parent, iOpenFile * );
	void (*empty)( Model * );
} ModelClass;

extern ModelLoadState *model_loadstate;

ModelRename *model_loadstate_rename_new( ModelLoadState *state, 
	const char *old_name, const char *new_name );
ModelLoadState *model_loadstate_new( 
	const char *filename, const char *filename_user );
ModelLoadState *model_loadstate_new_openfile( iOpenFile *of );
void model_loadstate_destroy( ModelLoadState *state );
char *model_loadstate_rewrite_name( char *name );
void model_loadstate_rewrite( ModelLoadState *state, 
	char *old_rhs, char *new_rhs );

void model_register_loadable( ModelClass *model_class );

View *model_view_new( Model *model, View *parent );
void model_scrollto( Model *model, ModelScrollPosition position );
void model_layout( Model *model );
void *model_reset( Model *model );
void *model_edit( GtkWidget *parent, Model *model );
void *model_header( GtkWidget *parent, Model *model );
void model_front( Model *model );

void *model_save( Model *model, xmlNode * );
gboolean model_save_test( Model *model );
void *model_save_text( Model *model, iOpenFile * );
void *model_load( Model *model,
	ModelLoadState *state, Model *parent, xmlNode *xnode );
void *model_load_text( Model *model, Model *parent, iOpenFile * );
void *model_empty( Model *model );

gboolean model_new_xml( ModelLoadState *state, Model *parent, xmlNode *xnode );

GType model_get_type( void );

void model_base_init( void );

View *model_build_display_all( Model *model, View *parent );

void model_check_destroy( GtkWidget *parent, Model *model );

gboolean model_set_display( Model *model, gboolean display );

void *model_clear_edited( Model *model );
