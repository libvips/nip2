/* Decls for class.c
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

/* The builtin member names we know about.
 */
#define MEMBER_BANDS "bands"
#define MEMBER_CAPTION "caption"
#define MEMBER_DISPLAY "display"
#define MEMBER_CHECK "check"
#define MEMBER_FILENAME "filename"
#define MEMBER_FORMAT "format"
#define MEMBER_FROM "from"
#define MEMBER_HEIGHT "height"
#define MEMBER_LABELS "labels"
#define MEMBER_NAME "name"
#define MEMBER_OFFSET "offset"
#define MEMBER_SCALE "scale"
#define MEMBER_SUPER "super"
#define MEMBER_THIS "this"
#define MEMBER_TO "to"
#define MEMBER_VALUE "value"
#define MEMBER_WIDTH "width"
#define MEMBER_LEFT "left"
#define MEMBER_TOP "top"
#define MEMBER_IMAGE "image"
#define MEMBER_OO_BINARY "oo_binary"
#define MEMBER_OO_BINARY2 "oo_binary'"
#define MEMBER_OO_UNARY "oo_unary"
#define MEMBER_COLOUR_SPACE "colour_space"
#define MEMBER_EXPR "expr"
#define MEMBER_INTERVAL "interval"
#define MEMBER_OPTIONS "options"

#define MEMBER_VISLEVEL "_vislevel"
#define MEMBER_ACTION "action"
#define MEMBER_LABEL "label"
#define MEMBER_ICON "icon"
#define MEMBER_TOOLTIP "tooltip"

/* The class names we know about.
 */
#define CLASS_SLIDER "Scale"
#define CLASS_TOGGLE "Toggle"
#define CLASS_IMAGE "Image"
#define CLASS_COLOUR "Colour"
#define CLASS_NUMBER "Number"
#define CLASS_STRING "String"
#define CLASS_OPTION "Option"
#define CLASS_MATRIX "Matrix_vips"
#define CLASS_ARROW "Arrow"
#define CLASS_REGION "Region"
#define CLASS_AREA "Area"
#define CLASS_HGUIDE "HGuide"
#define CLASS_VGUIDE "VGuide"
#define CLASS_MARK "Mark"
#define CLASS_POINT "Point"
#define CLASS_PATHNAME "Pathname"
#define CLASS_FONTNAME "Fontname"
#define CLASS_SEPARATOR "Separator"
#define CLASS_GROUP "Group"
#define CLASS_LIST "List"
#define CLASS_MENU "Menu"
#define CLASS_MENUITEM "Menuitem"
#define CLASS_MENUACTION "Menuaction"
#define CLASS_MENUPULLRIGHT "Menupullright"
#define CLASS_MENUSEPARATOR "Menuseparator"
#define CLASS_EXPRESSION "Expression"
#define CLASS_CLOCK "Clock"
#define CLASS_REAL "Real"
#define CLASS_VECTOR "Vector"
#define CLASS_PLOT "Plot"

/* What we loop over a class instance with.
 */
typedef void *(*class_map_fn)( Symbol *, PElement *, void *, void * );

Compile *class_get_compile( PElement *instance );
gboolean class_get_super( PElement *instance, PElement *out );
void *class_map( PElement *instance, class_map_fn fn, void *a, void *b );
gboolean class_get_member( PElement *instance, const char *name, 
	Symbol **sym_out, PElement *value );
gboolean class_get_symbol( PElement *class, Symbol *sym, PElement *out );
gboolean class_get_exact( PElement *instance, const char *name, PElement *out );

gboolean class_new_super( Heap *heap, 
	Compile *compile, PElement *this, PElement *instance );
gboolean class_new( Heap *heap, 
	Compile *compile, HeapNode **args, PElement *out );
gboolean class_clone( Heap *heap, PElement *class, PElement *out );
gboolean class_clone_args( Heap *heap, PElement *klass, PElement *out );
gboolean class_newv( Heap *heap, const char *name, PElement *out, ... );

gboolean class_get_member_real( PElement *klass, const char *name, 
	double *out );
gboolean class_get_member_int( PElement *instance, const char *name, 
	int *out );
gboolean class_get_member_bool( PElement *klass, const char *name, 
	gboolean *out );
gboolean class_get_member_image( PElement *instance, const char *name, 
	Imageinfo **out );
gboolean class_get_member_class( PElement *instance, const char *name, 
	const char *type, PElement *out );
gboolean class_get_member_lstring( PElement *instance, const char *name, 
	GSList **labels );
gboolean class_get_member_string( PElement *klass, const char *name, 
	char *buf, int sz );
gboolean class_get_member_instance( PElement *instance, const char *name, 
	const char *klass, PElement *out );
gboolean class_get_member_matrix_size( PElement *instance, const char *name, 
	int *xsize, int *ysize );
gboolean class_get_member_matrix( PElement *instance, const char *name, 
	double *buf, int n, int *xsize, int *ysize );
gboolean class_get_member_realvec( PElement *instance, const char *name, 
	double *buf, int n, int *length );
