/* Watch stuff in the prefs workspace.
 */

/*

    Copyright (C) 1991-2003 The National Gallery

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your watch) any later version.

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

/* Group watches with this.
 */

#define TYPE_WATCHGROUP (watchgroup_get_type())
#define WATCHGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WATCHGROUP, Watchgroup ))
#define WATCHGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WATCHGROUP, WatchgroupClass))
#define IS_WATCHGROUP( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WATCHGROUP ))
#define IS_WATCHGROUP_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WATCHGROUP ))
#define WATCHGROUP_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WATCHGROUP, WatchgroupClass ))

typedef struct _Watchgroup {
	iContainer parent_object;

	/* Workspaces we work within. Assume we are destroyed before this.
	 */
	Workspaceroot *workspaceroot;

	/* Name of workspace our watchers check for their syms.
	 */
	const char *name;

	/* Autosave timeout ... save our workspace automatically when this
	 * ticks away.
	 */
	guint auto_save_timeout;
} Watchgroup;

typedef struct _WatchgroupClass {
	iContainerClass parent_class;

	/* One of the watches in this group has changed. 
	 * People interested in several watches can connect to
	 * this, rather than having to try listening for many "changed" signals
	 * on the watches.
	 */
	void (*watch_changed)( Watchgroup *, Watch * );
} WatchgroupClass;

GType watchgroup_get_type( void );
Watchgroup *watchgroup_new( Workspaceroot *workspaceroot, const char *name );
void watchgroup_flush( Watchgroup *watchgroup );

/* Abstract base class for something that watches a row.
 */

#define TYPE_WATCH (watch_get_type())
#define WATCH( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WATCH, Watch ))
#define WATCH_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WATCH, WatchClass))
#define IS_WATCH( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WATCH ))
#define IS_WATCH_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WATCH ))
#define WATCH_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WATCH, WatchClass ))

typedef void (*WatchCallbackFn)( void * );

struct _Watch {
	iContainer parent_class;

	Row *row;			/* Row we watch */
	gboolean ok;			/* Value read OK on last change */

	guint destroy_sid;		/* Listen for events */
	guint changed_sid;
};

typedef struct _WatchClass {
	iContainerClass parent_class;

	/* Update value from row.
	 */
	gboolean (*update)( Watch * );

	/* Get a pointer to value.
	 */
	void *(*get_value)( Watch * );
} WatchClass;

Watch *watch_find( Watchgroup *watchgroup, const char *name );
GtkType watch_get_type( void );
void watch_relink_all( void );
void watch_vset( Watch *watch, const char *fmt, va_list args );
void watch_set( Watch *watch, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));

/* A watch that watches something with an int value.
 */

typedef struct _WatchInt WatchInt;

#define TYPE_WATCH_INT (watch_int_get_type())
#define WATCH_INT( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WATCH_INT, WatchInt ))
#define WATCH_INT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WATCH_INT, WatchIntClass))
#define IS_WATCH_INT( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WATCH_INT ))
#define IS_WATCH_INT_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WATCH_INT ))
#define WATCH_INT_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WATCH_INT, WatchIntClass ))

struct _WatchInt {
	Watch parent_class;

	int value;
};

typedef struct _WatchIntClass {
	WatchClass parent_class;

} WatchIntClass;

GtkType watch_int_get_type( void );
int watch_int_get( Watchgroup *, const char *name, int fallback );

/* A watch that watches something with a double value.
 */

typedef struct _WatchDouble WatchDouble;

#define TYPE_WATCH_DOUBLE (watch_double_get_type())
#define WATCH_DOUBLE( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WATCH_DOUBLE, WatchDouble ))
#define WATCH_DOUBLE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WATCH_DOUBLE, WatchDoubleClass))
#define IS_WATCH_DOUBLE( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WATCH_DOUBLE ))
#define IS_WATCH_DOUBLE_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WATCH_DOUBLE ))
#define WATCH_DOUBLE_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WATCH_DOUBLE, \
		WatchDoubleClass ))

struct _WatchDouble {
	Watch parent_class;

	double value;
};

typedef struct _WatchDoubleClass {
	WatchClass parent_class;

} WatchDoubleClass;

GtkType watch_double_get_type( void );
double watch_double_get( Watchgroup *, const char *name, double fallback );

/* A watch that watches a path.
 */

typedef struct _WatchPath WatchPath;

#define TYPE_WATCH_PATH (watch_path_get_type())
#define WATCH_PATH( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WATCH_PATH, WatchPath ))
#define WATCH_PATH_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WATCH_PATH, WatchPathClass))
#define IS_WATCH_PATH( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WATCH_PATH ))
#define IS_WATCH_PATH_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WATCH_PATH ))
#define WATCH_PATH_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WATCH_PATH, WatchPathClass ))

struct _WatchPath {
	Watch parent_class;

	GSList *value;
};

typedef struct _WatchPathClass {
	WatchClass parent_class;

} WatchPathClass;

GtkType watch_path_get_type( void );
GSList *watch_path_get( Watchgroup *, const char *name, GSList *fallback );

typedef struct _WatchBool WatchBool;

#define TYPE_WATCH_BOOL (watch_bool_get_type())
#define WATCH_BOOL( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WATCH_BOOL, WatchBool ))
#define WATCH_BOOL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WATCH_BOOL, WatchBoolClass))
#define IS_WATCH_BOOL( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WATCH_BOOL ))
#define IS_WATCH_BOOL_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WATCH_BOOL ))
#define WATCH_BOOL_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WATCH_BOOL, WatchBoolClass ))

struct _WatchBool {
	Watch parent_class;

	gboolean value;
};

typedef struct _WatchBoolClass {
	WatchClass parent_class;

} WatchBoolClass;

GtkType watch_bool_get_type( void );
gboolean watch_bool_get( Watchgroup *, const char *name, gboolean fallback );

typedef struct _WatchString WatchString;

#define TYPE_WATCH_STRING (watch_string_get_type())
#define WATCH_STRING( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_WATCH_STRING, WatchString ))
#define WATCH_STRING_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_WATCH_STRING, WatchStringClass))
#define IS_WATCH_STRING( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_WATCH_STRING ))
#define IS_WATCH_STRING_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_WATCH_STRING ))
#define WATCH_STRING_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_WATCH_STRING, \
		WatchStringClass ))

struct _WatchString {
	Watch parent_class;

	char *value;
};

typedef struct _WatchStringClass {
	WatchClass parent_class;

} WatchStringClass;

GtkType watch_string_get_type( void );
const char *watch_string_get( Watchgroup *, 
	const char *name, const char *fallback );

/* Prefs we follow from C.
 */

/* Default show states.
 */
#define DISPLAY_RULERS \
	(watch_bool_get( main_watchgroup, "DISPLAY_RULERS", FALSE ))
#define DISPLAY_STATUS \
	(watch_bool_get( main_watchgroup, "DISPLAY_STATUS", FALSE ))
#define DISPLAY_CONVERSION \
	(watch_bool_get( main_watchgroup, "DISPLAY_CONVERSION", FALSE ))

/* Display a crosshair on image windows ... turn-off-able, since some desktop
 * themes have almost invisible crosshairs.
 */
#define DISPLAY_CROSSHAIR \
	(watch_bool_get( main_watchgroup, "DISPLAY_CROSSHAIR", TRUE ))

/* Update children during paint.
 */
#define PAINTBOX_RECOMP \
	(watch_bool_get( main_watchgroup, "PAINTBOX_RECOMP", TRUE ))

/* Help browser.
 */
#define BOX_BROWSER (watch_string_get( main_watchgroup, "BROWSER", "mozilla" ))
#define BOX_BROWSER_REMOTE (watch_string_get( main_watchgroup, \
	"BROWSER_REMOTE", "-remote 'openURL(%s)'" ))

/* Thumbnail size.
 */
#define DISPLAY_THUMBNAIL \
	(watch_int_get( main_watchgroup, "DISPLAY_THUMBNAIL", 64 ))

/* High-quality thumbnails.
 */
#define DISPLAY_THUMBNAIL_HQ \
	(watch_bool_get( main_watchgroup, "DISPLAY_THUMBNAIL_HQ", FALSE ))

/* File stuff.
 */
#define PIN_FILESEL \
	(watch_bool_get( main_watchgroup, "CALC_PIN_FILESEL", FALSE ))
#define IP_JPEG_Q \
	(watch_int_get( main_watchgroup, "JPEG_Q", 75 ))
#define IP_JPEG_ICC_PROFILE \
	(watch_int_get( main_watchgroup, "JPEG_ICC_PROFILE", 0 ))
#define IP_JPEG_ICC_PROFILE_FILE \
	(watch_string_get( main_watchgroup, "JPEG_ICC_PROFILE_FILE", \
		"$VIPSHOME/share/nip2/data/sRGB.icm" ))
#define IP_PPM_MODE \
	(watch_int_get( main_watchgroup, "PPM_MODE", 0 ))
#define IP_CSV_SEPARATOR \
	(watch_string_get( main_watchgroup, "CSV_SEPARATOR", "\t" ))
#define IP_PNG_COMPRESSION \
	(watch_int_get( main_watchgroup, "PNG_COMPRESSION", 6 ))
#define IP_PNG_INTERLACE \
	(watch_int_get( main_watchgroup, "PNG_INTERLACE", 0 ))
#define IP_TIFF_COMPRESSION \
	(watch_int_get( main_watchgroup, "TIFF_COMPRESSION", 0 ))
#define IP_TIFF_JPEG_Q \
	(watch_int_get( main_watchgroup, "TIFF_JPEG_Q", 75 ))
#define IP_TIFF_LAYOUT \
	(watch_int_get( main_watchgroup, "TIFF_LAYOUT", 0 ))
#define IP_TIFF_TILE_WIDTH \
	(watch_int_get( main_watchgroup, "TIFF_TILE_WIDTH", 128 ))
#define IP_TIFF_TILE_HEIGHT \
	(watch_int_get( main_watchgroup, "TIFF_TILE_HEIGHT", 128 ))
#define IP_TIFF_MULTI_RES \
	(watch_int_get( main_watchgroup, "TIFF_MULTI_RES", 0 ))
#define IP_TIFF_FORMAT \
	(watch_int_get( main_watchgroup, "TIFF_FORMAT", 0 ))
#define IP_TIFF_PREDICTOR \
	(watch_int_get( main_watchgroup, "TIFF_PREDICTOR", 0 ))
#define IP_TIFF_BIGTIFF \
	(watch_bool_get( main_watchgroup, "TIFF_BIGTIFF", FALSE ))

/* Autoreload. 
 */
#define CALC_RELOAD (watch_bool_get( main_watchgroup, "CALC_RELOAD", FALSE ))

/* Max chars we print. 
 */
#define LINELENGTH \
	IM_CLIP( 10, \
		watch_int_get( main_watchgroup, "CALC_LINELENGTH", 80 ), \
		MAX_LINELENGTH )

/* CPUs we work over.
 */
#define VIPS_CPUS \
	(watch_int_get( main_watchgroup, "VIPS_CPUS", 1 ))

/* Bar prefs.
 */
#define MAINW_TOOLBAR \
	(watch_bool_get( main_watchgroup, "MAINW_TOOLBAR", TRUE ))
#define MAINW_TOOLBAR_STYLE \
	(watch_int_get( main_watchgroup, "MAINW_TOOLBAR_STYLE", 0 ))
#define MAINW_STATUSBAR \
	(watch_bool_get( main_watchgroup, "MAINW_STATUSBAR", TRUE ))

#define WORKSPACE_LPANE_OPEN \
	(watch_bool_get( main_watchgroup, "WORKSPACE_LPANE_OPEN", FALSE ))
#define WORKSPACE_LPANE_POSITION \
	(watch_int_get( main_watchgroup, "WORKSPACE_LPANE_POSITION", 200 ))
#define WORKSPACE_RPANE_OPEN \
	(watch_bool_get( main_watchgroup, "WORKSPACE_RPANE_OPEN", FALSE ))
#define WORKSPACE_RPANE_POSITION \
	(watch_int_get( main_watchgroup, "WORKSPACE_RPANE_POSITION", 400 ))

/* Heap size. Big enough to always load prefs, small enough that it doesn't 
 * trash the computer.
 */
#define MAX_HEAPSIZE \
	IM_CLIP( 100000, \
		watch_int_get( main_watchgroup, "CALC_MAX_HEAP", 200000 ), \
		10000000 )

/* Region dragging.
 */
#ifdef NO_UPDATE
#define CALC_RECOMP_REGION \
	(watch_bool_get( main_watchgroup, "CALC_RECOMP_REGION", FALSE ))
#else
#define CALC_RECOMP_REGION \
	(watch_bool_get( main_watchgroup, "CALC_RECOMP_REGION", TRUE ))
#endif

/* Slider dragging.
 */
#define CALC_RECOMP_SLIDER \
	(watch_bool_get( main_watchgroup, "CALC_RECOMP_SLIDER", FALSE ))

/* Popup new objects.
 */
#define POPUP_NEW_ROWS \
	(watch_bool_get( main_watchgroup, "POPUP_NEW_ROWS", FALSE ))

/* Draw LEDs rather than recolouring tally buttons.
 */
#define CALC_DISPLAY_LED \
	(watch_bool_get( main_watchgroup, "CALC_DISPLAY_LED", FALSE ))

/* Number of vips calls to memoise.
 */
#define CALL_HISTORY_MAX \
	(watch_int_get( main_watchgroup, "VIPS_HISTORY_MAX", 200 ))

/* Auto save wses.
 */
#define AUTO_WS_SAVE \
	(watch_bool_get( main_watchgroup, "CALC_AUTO_WS_SAVE", TRUE ))

/* Image window geometry.
 */
#define IMAGE_WINDOW_WIDTH \
	(watch_int_get( main_watchgroup, "IMAGE_WINDOW_WIDTH", 600 ))
#define IMAGE_WINDOW_HEIGHT \
	(watch_int_get( main_watchgroup, "IMAGE_WINDOW_HEIGHT", 650 ))

/* Default font.
 */
#define PAINTBOX_FONT \
	(watch_string_get( main_watchgroup, "PAINTBOX_FONT", "Sans 12" ))

/* Max undo steps ... -1 == unlimited.
 */
#define PAINTBOX_MAX_UNDO \
	(watch_int_get( main_watchgroup, "PAINTBOX_MAX_UNDO", -1 ))

/* Default image file type.
 */
#define IMAGE_FILE_TYPE \
	(watch_int_get( main_watchgroup, "IMAGE_FILE_TYPE", 0 ))

/* Prefs we watch.
 */
#define PATH_SEARCH (watch_path_get( main_watchgroup, "CALC_PATH_SEARCH", \
	path_search_default ))
#define PATH_START (watch_path_get( main_watchgroup, "CALC_PATH_START", \
	path_start_default ))
#define PATH_TMP (watch_string_get( main_watchgroup, "CALC_PATH_TMP", \
	path_tmp_default ))

/* How we print stuff.
 */
#define TRACE_FUNCTIONS \
	(watch_bool_get( main_watchgroup, "CALC_TRACE_FUNCTIONS", FALSE ))
#define PRINT_CARTESIAN \
	(watch_bool_get( main_watchgroup, "CALC_PRINT_CARTIESIAN", FALSE ))

/* Program window.
 */
#define PROGRAM_PANE_POSITION \
	(watch_double_get( main_watchgroup, "PROGRAM_PANE_POSITION", 200 ))

