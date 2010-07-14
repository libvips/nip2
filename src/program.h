/* Decls for program.c ... edit window
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

#define TYPE_PROGRAM (program_get_type())
#define PROGRAM( obj ) (GTK_CHECK_CAST( (obj), TYPE_PROGRAM, Program ))
#define PROGRAM_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_PROGRAM, ProgramClass ))
#define IS_PROGRAM( obj ) (GTK_CHECK_TYPE( (obj), TYPE_PROGRAM ))
#define IS_PROGRAM_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_PROGRAM ))

struct _Program {
	iWindow parent_class;

	/* The set of kits we manage.
	 */
	Toolkitgroup *kitg;

	GtkWidget *text;
	gboolean dirty;		/* Has the text changed since we set it */
	guint text_hash;	/* Hash of the last text we set */
	GtkWidget *tree;
	GtkWidget *pane;
	int pane_position;
	guint refresh_timeout;	/* Timeout for UI refresh */
	guint select_changed_sid;	
	guint row_inserted_sid;	
	guint row_deleted_sid;	

	/* Track during drags.
	 */
	Toolkit *to_kit;
	int to_pos;

	/* Store for kit/tool view.
	 */
	GtkTreeStore *store;

	/* Listen for all kit changes here.
	 */
	guint kitgroup_changed_sid;	
	guint kitgroup_destroy_sid;		
	
	/* The current kit.
	 */
	Toolkit *kit;		
	guint kit_destroy_sid;	

	/* The selected tool.
	 */
	Tool *tool;	
	int pos;		/* Position of tool in kit */
	guint tool_destroy_sid;	

	/* Current search settings.
	 */
	char *search;
	gboolean csens;		/* Case sensitive */
	gboolean fromtop;	/* Start search from beginning again */
#ifdef HAVE_GREGEX
	gboolean regexp;	/* Interpret as regexp */
	GRegex *comp;		/* Compiled pattern */
#endif /*HAVE_GREGEX*/

	/* Current search position.
	 */
	Symbol *find_sym;	/* Tool containing search point */
	size_t find_start;	/* Offset into tool text of found string */
	size_t find_end;	
	guint find_sym_destroy_sid;/* Watch for find_sym death here */
};

typedef struct _ProgramClass {
	iWindowClass parent_class;

	/* My methods.
	 */
} ProgramClass;

GtkType program_get_type( void );
GtkWidget *program_text_new( void );
Program *program_new( Toolkitgroup *kitg );

gboolean program_select( Program *program, Model *model );
