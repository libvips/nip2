/* Tools ... mostly a menu item.
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

/* Build a tree of these for each tool we make.
 */
struct _Toolitem {
	/* The thing for which we are making an item. Eg. if the .def file
	 * has 'fred = class Menuitem "poop" "lots of poop" {}', this is the
	 * Compile for fred.
	 *
	 * compile is not always valid .. eg. for #dialog or #separator
	 */
	Compile *compile;

	/* The top-level tool we come from.
	 */
	Tool *tool;

	/* The symbol we perform the action with (eg. get nparam from this).
	 */
	Symbol *action_sym;

	/* Set for a separator.
	 */
	gboolean is_separator;

	/* Set if we decide during build that this item should be a pullright.
	 */
	gboolean is_pullright;

	/* If this is a pullright, the children of this item. If we are a
	 * child, the parent.
	 */
	GSList *children;
	Toolitem *parent;

	/* Set if we decide this should have an action.
	 */
	gboolean is_action;

	char *label;		/* eg. "W_hite Balance" */
	char *name;		/* eg. "White Balance" */
	char *icon;		/* eg. "$VIPSHOME/icons/wb.png" */
	char *tooltip;		/* eg. "move whitepoint to region neutral" */
	char *help;		/* eg. "White Balance r: move ..." */
	char *action;		/* eg. "White_balance_widget._action" */
	char *path;		/* eg. "<mainw>/Toolkits/Image" */
	char *user_path;	/* eg. "Image / White Balance" */
};

#define TYPE_TOOL (tool_get_type())
#define TOOL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_TOOL, Tool ))
#define TOOL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_TOOL, ToolClass))
#define IS_TOOL( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_TOOL ))
#define IS_TOOL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_TOOL ))
#define TOOL_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_TOOL, ToolClass ))

/* Tool types: a def (sym points to symbol for this def), a dialog (keep
 * filename and prompt name), or a separator.
 */
typedef enum {
	TOOL_SYM,
	TOOL_DIA,
	TOOL_SEP
} Tooltype;

/* What we hold for each tool. 
 */
struct _Tool {
	Filemodel parent_class;

	Tooltype type;		
	Symbol *sym;		/* For SYM tools: symbol this tool represents */
	guint new_value_sid;	/* Watch for new_value with this */
	Symbol *link_sym;	/* the sym we are watching (in case ->sym is
				   NULLed before we try to disconnect */

	Toolkit *kit; 		/* Link back to toolkit */
	int lineno;		/* -1 for not known, or lineno in kit */

	Toolitem *toolitem;	/* Items made by this tool */
};

typedef struct _ToolClass {
	FilemodelClass parent_class;

	/* My methods.
	 */
} ToolClass;

GType tool_get_type( void );

Tool *tool_new_sym( Toolkit *kit, int pos, Symbol *sym );
Tool *tool_new_sep( Toolkit *kit, int pos );
Tool *tool_new_dia( Toolkit *kit, int pos, 
	const char *filename, const char *name );

Toolitem *toolitem_lookup( Toolkitgroup *kitg, Symbol *action );
