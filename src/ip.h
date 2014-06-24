/* All ip headers.
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

/* We can get multiple includes sometimes, gah, thank you bison.
 */
#ifndef IP_H
#define IP_H

/* DEBUG everywhere.
#define DEBUG
 */

/* Turn off VIPS's old and broken defines, we don't need them.
 */
#define IM_NO_VIPS7_COMPAT

/* Enable heap sanity checks on every alloc ... very slow ... also see heap.c
#define DEBUG_HEAP
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /*HAVE_CONFIG_H*/

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) gettext(String)
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) (String)
#endif
#else /* NLS is disabled */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(String) (String)
#define gettext(String) (String)
#define dgettext(Domain,String) (String)
#define dcgettext(Domain,String,Type) (String)
#define bindtextdomain(Domain,Directory) (Domain) 
#define bind_textdomain_codeset(Domain,Codeset) (Codeset) 
#define ngettext(S, P, N) ((N) == 1 ? (S) : (P))
#endif /* ENABLE_NLS */

#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#include <locale.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif /*HAVE_PWD_H*/
#ifdef HAVE_FNMATCH_H
#include <fnmatch.h>
#endif /*HAVE_FNMATCH_H*/
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif /*HAVE_SYS_PARAM_H*/
#include <sys/stat.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /*HAVE_SYS_TIME_H*/
#include <sys/types.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif /*HAVE_SYS_RESOURCE_H*/
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif /*HAVE_SYS_WAIT_H*/
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /*HAVE_UNISTD_H*/
#ifdef HAVE_SYS_STATVFS_H
#include <sys/statvfs.h>
#endif /*HAVE_SYS_STATVFS_H*/
#ifdef HAVE_SYS_VFS_H
#include <sys/vfs.h>
extern int statfs();
#endif /*HAVE_SYS_VFS_H*/
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif /*HAVE_SYS_MOUNT_H*/
#ifdef OS_WIN32
#include <windows.h>
#endif /*OS_WIN32*/
#ifdef HAVE_FFTW
#include <fftw.h>
#endif /*HAVE_FFTW*/
#ifdef HAVE_FFTW3
#include <fftw3.h>
#endif /*HAVE_FFTW3*/
#include <fcntl.h>

/* Have to include glib before dmalloc ... dmalloc may be included by vips.h
 */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#ifdef HAVE_LIBGOFFICE
#include <goffice/goffice.h>

#include <goffice/app/go-plugin.h>
#include <goffice/app/go-plugin-loader-module.h>

#include <goffice/data/go-data-simple.h>

#include <goffice/graph/gog-data-set.h>
#include <goffice/graph/gog-label.h>
#include <goffice/graph/gog-object.h>
#include <goffice/graph/gog-plot.h>
#include <goffice/graph/gog-series.h>
#include <goffice/graph/gog-grid.h>
#include <goffice/graph/gog-grid-line.h>
#include <goffice/graph/gog-legend.h>
#include <goffice/graph/gog-chart-map.h>

#include <goffice/gtk/go-graph-widget.h>

#include <goffice/utils/go-color.h>
#include <goffice/utils/go-marker.h>
#endif /*HAVE_LIBGOFFICE*/

#ifdef HAVE_LIBGVC
#include <gvc.h>
#endif /*HAVE_LIBGVC*/

#include <vips/vips.h>
#include <vips/debug.h>

#include <libxml/tree.h>
#include <libxml/parser.h>

/* If we're not using GNU C, elide __attribute__ 
 */
#ifndef __GNUC__
#  ifndef __attribute__
#    define  __attribute__(x)  /*NOTHING*/
#  endif
#endif

/* Our general widgets.
 */
#include "formula.h"
#include "doubleclick.h"

/* Generated marshallers.
 */
#include "nipmarshal.h"

#define MAXFILES (4000)		/* Max. no of files in path */
#define STACK_SIZE (1000)	/* Depth of eval stack */
#define LEN_LABEL (512)		/* Label on windows */
#define MAX_SYSTEM (50)		/* Max number of args we allow */
#define MAX_BANDS (64)		/* Max number of bands in image */
#define MAX_CSTACK (10)		/* Max number of cursors we stack */
#define MAX_STRSIZE (100000)	/* Size of text for user defs */
#define MAX_TRACE (1024)	/* Biggest thing we print in trace */
#define MAX_SSTACK (40)		/* Scope stack for parser */
#define VIPS_HOMEPAGE "http://www.vips.ecs.soton.ac.uk"
#define IP_NAME PACKAGE "-" VERSION
#define NIP_DOCPATH "$VIPSHOME" G_DIR_SEPARATOR_S "share" G_DIR_SEPARATOR_S \
        "doc" G_DIR_SEPARATOR_S PACKAGE G_DIR_SEPARATOR_S "html"
#define VIPS_DOCPATH "$VIPSHOME" G_DIR_SEPARATOR_S "share" G_DIR_SEPARATOR_S \
	"doc" G_DIR_SEPARATOR_S "vips" G_DIR_SEPARATOR_S "html"
#define IP_NAME PACKAGE "-" VERSION
#define NAMESPACE VIPS_HOMEPAGE "/" "nip" 
				/* XML namespace ... note, not nip2! */
#define MAX_LINELENGTH (120)	/* Max chars we display of value */
#define MAX_RECENT (10)		/* Number of recent items in file menu */
#define NIP_COPYRIGHT "%s: &#0169;2014 Imperial College, London"

/* Our stock_ids.
 */
#define STOCK_NEXT_ERROR "nip-next-error"
#define STOCK_DROPPER "nip-dropper"
#define STOCK_DUPLICATE "nip-duplicate"
#define STOCK_PAINTBRUSH "nip-paintbrush"
#define STOCK_LINE "nip-linedraw"
#define STOCK_TEXT "nip-text"
#define STOCK_SMUDGE "nip-smudge"
#define STOCK_FLOOD "nip-flood"
#define STOCK_FLOOD_BLOB "nip-floodblob"
#define STOCK_RECT "nip-rect"
#define STOCK_MOVE "nip-move"
#define STOCK_LOCK "nip-lock"
#define STOCK_ALERT "nip-alert"
#define STOCK_SELECT "nip-select"
#define STOCK_LED_RED "nip-led-red"
#define STOCK_LED_GREEN "nip-led-green"
#define STOCK_LED_BLUE "nip-led-blue"
#define STOCK_LED_YELLOW "nip-led-yellow"
#define STOCK_LED_CYAN "nip-led-cyan"
#define STOCK_LED_OFF "nip-led-off"

/* How much we decompile for error messages.
 */
#define MAX_ERROR_FRAG (100)

/* win32 adds '_', sometimes. 
 */
#ifdef OS_WIN32
#ifndef popen
#define popen(b,m) _popen(b,m)
#endif /*popen*/
#ifndef pclose
#define pclose(f) _pclose(f)
#endif /*pclose*/
#define mktemp(f) _mktemp(f)
#endif /*OS_WIN32*/

/* Fwd ref these.
 */
typedef struct _Watch Watch;
typedef struct _Toolitem Toolitem;
typedef struct _BuiltinInfo BuiltinInfo;
typedef struct _Classmodel Classmodel;
typedef struct _Colour Colour;
typedef struct _Column Column;
typedef struct _Columnview Columnview;
typedef struct _Compile Compile;
typedef struct _Conversion Conversion;
typedef struct _Conversionview Conversionview;
typedef struct _Expr Expr;
typedef struct _Filemodel Filemodel;
typedef struct _Heap Heap;
typedef struct _HeapBlock HeapBlock;
typedef struct _Heapmodel Heapmodel;
typedef struct _iArrow iArrow;
typedef struct _iImage iImage;
typedef struct _Imagedisplay Imagedisplay;
typedef struct _Managed Managed;
typedef struct _Managedfile Managedfile;
typedef struct _Managedgvalue Managedgvalue;
typedef struct _Managedgobject Managedgobject;
typedef struct _Managedstring Managedstring;
typedef struct _Imageinfo Imageinfo;
typedef struct _Imagepresent Imagepresent;
typedef struct _Imagemodel Imagemodel;
typedef struct _iRegion iRegion;
typedef struct _iRegiongroup iRegiongroup;
typedef struct _Link Link;
typedef struct _LinkExpr LinkExpr;
typedef struct _Model Model;
typedef struct _iObject iObject;
typedef struct _iContainer iContainer;
typedef struct _Paintboxview Paintboxview;
typedef struct _ParseConst ParseConst;
typedef struct _ParseNode ParseNode;
typedef struct _Program Program;
typedef struct _String String;
typedef struct _Number Number;
typedef struct _Reduce Reduce;
typedef struct _Regionview Regionview;
typedef struct _Rhs Rhs;
typedef struct _Rhsview Rhsview;
typedef struct _Row Row;
typedef struct _Rowview Rowview;
typedef struct _Statusview Statusview;
typedef struct _Plotstatus Plotstatus;
typedef struct _Plot Plot;
typedef struct _Plotwindow Plotwindow;
typedef struct _Plotpresent Plotpresent;
typedef struct _Plotmodel Plotmodel;
typedef struct _Graphwindow Graphwindow;
typedef struct _Subcolumn Subcolumn;
typedef struct _Subcolumnview Subcolumnview;
typedef struct _Symbol Symbol;
typedef struct _Tool Tool;
typedef struct _Toolkit Toolkit;
typedef struct _Toolkitgroup Toolkitgroup;
typedef struct _Toolkitgroupview Toolkitgroupview;
typedef struct _Toolkitview Toolkitview;
typedef struct _Toolview Toolview;
typedef struct _Trace Trace;
typedef struct _Preview Preview;
typedef struct _Infobar Infobar;
typedef struct _iError iError;
typedef struct _Log Log;
typedef struct _vObject vObject;
typedef struct _View View;
typedef struct _Workspace Workspace;
typedef struct _Workspaceview Workspaceview;
typedef struct _Workspaceroot Workspaceroot;
typedef struct _Workspacegroup Workspacegroup;
typedef struct _Workspacegroupview Workspacegroupview;
typedef struct _Prefworkspaceview Prefworkspaceview;
typedef struct _Prefcolumnview Prefcolumnview;
typedef struct _iText iText;
typedef struct _Expression Expression;
typedef struct _Mainw Mainw;
typedef struct _Toolviewitemgroup Toolviewitemgroup;
typedef struct _Panechild Panechild;
typedef struct _Toolkitbrowser Toolkitbrowser;
typedef struct _Workspacedefs Workspacedefs;

/* container map function typedefs.
 */
typedef void *(*row_map_fn)( Row *, void *, void *, void * );
typedef void *(*symbol_map_fn)( Symbol *, void *, void *, void * );
typedef void *(*column_map_fn)( Column *, void * );
typedef void *(*view_map_fn)( View *, void *, void * );
typedef void *(*rowview_map_fn)( Rowview *, void * );
typedef void *(*workspace_map_fn)( Workspace *, void * );
typedef void *(*toolkit_map_fn)( Toolkit *, void *, void * );
typedef void *(*tool_map_fn)( Tool *, void *, void * );

/* Util stuff.
 */
#include "util.h"
#include "gtkutil.h"
#include "path.h"
#include "iobject.h"
#include "icontainer.h"
#include "iwindow.h"
#include "idialog.h"
#include "boxes.h"
#include "popupbutton.h"
#include "imageheader.h"
#include "filesel.h"
#include "managed.h"
#include "managedfile.h"
#include "managedgvalue.h"
#include "managedgobject.h"
#include "imageinfo.h"
#include "imagedisplay.h"
#include "colourdisplay.h"
#include "imagemodel.h"
#include "imagepresent.h"
#include "floatwindow.h"
#include "imageview.h"
#include "tslider.h"
#include "pane.h"
#include "progress.h"

/* Basic ip includes (order important).
 */
#include "tree.h"
#include "heap.h"
#include "managedstring.h"
#include "class.h"
#include "link.h"
#include "expr.h"
#include "model.h"
#include "paintboxview.h"
#include "conversion.h"
#include "heapmodel.h"
#include "classmodel.h"
#include "filemodel.h"
#include "symbol.h"
#include "workspace.h"
#include "workspaceroot.h"
#include "workspacegroup.h"
#include "toolkitgroup.h"
#include "secret.h"
#include "action.h"
#include "reduce.h"
#include "vobject.h"
#include "vipsobject.h"
#include "view.h"
#include "graphicview.h"
#include "spin.h"
#include "row.h"
#include "rowview.h"
#include "subcolumn.h"
#include "subcolumnview.h"
#include "rhs.h"
#include "rhsview.h"
#include "workspaceview.h"
#include "workspacegroupview.h"
#include "toolkitgroupview.h"
#include "column.h"
#include "columnview.h"
#include "toolkit.h"
#include "tool.h"
#include "toolkitview.h"
#include "toolview.h"
#include "watch.h"
#include "value.h"
#include "panechild.h"

/* Per module includes, any order
 */
#include "workspacedefs.h"
#include "toolkitbrowser.h"
#include "defbrowser.h"
#include "log.h"
#include "error.h"
#include "trace.h"
#include "program.h"
#include "conversionview.h"
#include "statusview.h"
#include "plotstatus.h"
#include "mainw.h"
#include "preview.h"
#include "builtin.h"
#include "compile.h"
#include "dump.h"
#include "main.h"
#include "predicate.h"
#include "slider.h"
#include "clock.h"
#include "pathname.h"
#include "fontname.h"
#include "group.h"
#include "real.h"
#include "vector.h"
#include "colour.h"
#include "number.h"
#include "istring.h"
#include "editview.h"
#include "expression.h"
#include "expressionview.h"
#include "stringview.h"
#include "numberview.h"
#include "matrix.h"
#include "matrixview.h"
#include "plot.h"
#ifdef HAVE_LIBGOFFICE
#include "plotview.h"
#endif /*HAVE_LIBGOFFICE*/
#include "plotmodel.h"
#include "plotpresent.h"
#include "plotwindow.h"
#include "graphwindow.h"
#include "option.h"
#include "optionview.h"
#include "iimage.h"
#include "iregion.h"
#include "iregiongroup.h"
#include "iarrow.h"
#include "valueview.h"
#include "sliderview.h"
#include "pathnameview.h"
#include "fontnameview.h"
#include "colourview.h"
#include "iimageview.h"
#include "iregionview.h"
#include "iregiongroupview.h"
#include "prefs.h"
#include "prefworkspaceview.h"
#include "prefcolumnview.h"
#include "regionview.h"
#include "itext.h"
#include "itextview.h"
#include "toggle.h"
#include "toggleview.h"
#include "call.h"
#include "cache.h"
#include "parser.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif /*WITH_DMALLOC*/

#endif /*IP_H*/
