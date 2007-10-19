/* Declarations for some basic util functions.
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

/* chartype strings. Useful with break_token().
 */
#define NUMERIC "0123456789"
#define WHITESPACE " \t\r\b\n"

/* Like IM_NEW() etc, but set ip's error buffer.
 */
#define INEW(IM,A) ((A *)imalloc((IM),sizeof(A)))
#define IARRAY(IM,N,T) ((T *)imalloc((IM),(N) * sizeof(T)))

/* No nulls! Handy for printf() %s
 */
#define NN( S ) ((S)?(S):"(null)")

#define UNREF( X ) do { \
	if( X ) { \
		g_object_unref( G_OBJECT( X ) ); \
		(X) = NULL; \
	} \
} while( 0 )

#define FREESID( SID, OBJ ) do { \
	if( (SID) && (OBJ) ) { \
		g_signal_handler_disconnect( (OBJ), (SID) ); \
		(SID) = 0; \
	} \
} while( 0 )

/* Swap two pointers.
 */
#define SWAPP( A, B ) { \
	void *swapp_t; \
 	\
	swapp_t = (A); \
	(A) = (B); \
	(B) = swapp_t; \
}

/* A string in the process of being written to ... multiple calls to 
 * buf_append add to it, on overflow append "..." and block further writes.
 */
typedef struct {
	char *base;		/* String base */
	int mx;			/* Maximum length */
	int i;			/* Current write point */
	gboolean full;		/* String has filled, block writes */
	int lasti;		/* For read-recent */
	gboolean dynamic;	/* We own the string with malloc() */
} BufInfo;

/* Static init of one of these.
 */
#define BUF_STATIC( TEXT, MAX ) \
	{ &TEXT[0], MAX, 0, FALSE, 0, FALSE }

/* Init and append to one of the above.
 */
void buf_rewind( BufInfo *buf );
void buf_destroy( BufInfo *buf );
void buf_init( BufInfo *buf );
void buf_set_static( BufInfo *buf, char *base, int mx );
void buf_set_dynamic( BufInfo *buf, int mx );
void buf_init_static( BufInfo *buf, char *base, int mx );
void buf_init_dynamic( BufInfo *buf, int mx );
gboolean buf_appendns( BufInfo *buf, const char *str, int sz );
gboolean buf_appends( BufInfo *buf, const char *str );
gboolean buf_appendf( BufInfo *buf, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
gboolean buf_vappendf( BufInfo *buf, const char *fmt, va_list ap );
gboolean buf_appendc( BufInfo *buf, char ch );
gboolean buf_appendsc( BufInfo *buf, gboolean quote, const char *str );
void buf_appendi( BufInfo *buf, IMAGE *im );
void buf_appendgv( BufInfo *buf, GValue *value );
gboolean buf_removec( BufInfo *buf, char ch );
gboolean buf_change( BufInfo *buf, const char *old, const char *new );
gboolean buf_is_empty( BufInfo *buf );
gboolean buf_is_full( BufInfo *buf );
const char *buf_all( BufInfo *buf );
const char *buf_firstline( BufInfo *buf );
gboolean buf_appendg( BufInfo *buf, double g );
gboolean buf_appendd( BufInfo *buf, int d );
int buf_len( BufInfo *buf );

gboolean set_prop( xmlNode *xnode, const char *name, const char *fmt, ... )
	__attribute__((format(printf, 3, 4)));
gboolean set_sprop( xmlNode *xnode, const char *name, const char *value );
gboolean set_dprop( xmlNode *xnode, const char *name, double value );
gboolean set_slprop( xmlNode *xnode, const char *name, GSList *labels );
gboolean set_dlprop( xmlNode *xnode, const char *name, double *values, int n );

gboolean get_sprop( xmlNode *xnode, const char *name, char *buf, int sz );
gboolean get_spropb( xmlNode *xnode, const char *name, BufInfo *buf );
gboolean get_iprop( xmlNode *xnode, const char *name, int *out );
gboolean get_dprop( xmlNode *xnode, const char *name, double *out );
gboolean get_bprop( xmlNode *xnode, const char *name, gboolean *out );
gboolean get_slprop( xmlNode *xnode, const char *name, GSList **out );
gboolean get_dlprop( xmlNode *xnode, const char *name, double **out );

void prettify_tree( xmlDoc *doc );

xmlNode *get_node( xmlNode *base, const char *name );

Rect *rect_dup( Rect *init );
void *rect_free( Rect *rect );

/* Like GFunc, but return a value.
 */
typedef gpointer (*SListMapFn)( gpointer, gpointer );
typedef gpointer (*SListMap2Fn)( gpointer, gpointer, gpointer );
typedef gpointer (*SListMap3Fn)( gpointer, gpointer, gpointer, gpointer );
typedef gpointer (*SListMap4Fn)( gpointer, gpointer, gpointer, gpointer,
	gpointer );
typedef gpointer (*SListMap5Fn)( gpointer, gpointer, gpointer, gpointer,
	gpointer, gpointer );
typedef gpointer (*SListFoldFn)( gpointer, gpointer, gpointer );
typedef gpointer (*SListFold2Fn)( gpointer, gpointer, gpointer, gpointer );

/* Like foreach, but allow abandon.
 */
void *slist_map( GSList *list, SListMapFn fn, gpointer a );
void *slist_map2( GSList *list, SListMap2Fn fn, gpointer a, gpointer b );
void *slist_map3( GSList *list, 
	SListMap3Fn fn, gpointer a, gpointer b, gpointer c );
void *slist_map4( GSList *list, 
	SListMap4Fn fn, gpointer a, gpointer b, gpointer c, gpointer d );
void *slist_map5( GSList *list, 
	SListMap5Fn fn, gpointer a, gpointer b, gpointer c, gpointer d,
	gpointer e );
void *slist_map_rev( GSList *list, SListMapFn fn, gpointer a );
void *slist_map2_rev( GSList *list, 
	SListMap2Fn fn, gpointer a, gpointer b );
void *slist_map3_rev( GSList *list, 
	SListMap3Fn fn, void *a, void *b, void *c );
void *map_equal( void *a, void *b );
gboolean slist_equal( GSList *l1, GSList *l2 );
void *slist_fold( GSList *list, void *start, SListFoldFn fn, void *a );
void *slist_fold2( GSList *list, 
	void *start, SListFold2Fn fn, void *a, void *b );
void slist_free_all( GSList *list );
GSList *slist_remove_all( GSList *list, gpointer data );

/* An slist, which tracks the end of the list, for fast append.
 */
typedef struct _Queue {
	GSList *list;
	GSList *tail;
	int length;
} Queue;

Queue *queue_new( void );

/* All we need for now.
 */
void *queue_head( Queue *queue );
void queue_add( Queue *queue, void *data );
gboolean queue_remove( Queue *q, void *data );
int queue_length( Queue *q );

extern BufInfo error_top_buf;
extern BufInfo error_sub_buf;

void error( const char *fmt, ... )
	__attribute__((noreturn, format(printf, 1, 2)));
void error_block( void );		/* Block updates to error_string */
void error_unblock( void );
void error_clear( void );
void error_top( const char *fmt, ... )
	__attribute__((format(printf, 1, 2)));
void error_sub( const char *fmt, ... )
	__attribute__((format(printf, 1, 2)));
void error_vips( void );
void error_vips_all( void );
const char *error_get_top( void );
const char *error_get_sub( void );

gboolean is_postfix( const char *a, const char *b );
gboolean is_prefix( const char *a, const char *b );
gboolean is_caseprefix( const char *a, const char *b );
gboolean is_casepostfix( const char *a, const char *b );
const char *findrightmost( const char *a, const char *b );
char *my_strcasestr( const char *haystack, const char *needle );
void change_suffix( const char *name, char *out, 
	const char *new, const char **olds, int nolds );

char *my_strccpy( char *output, const char *input );
char *my_strecpy( char *output, const char *input, gboolean quote );
const char *my_strrspn( const char *p, const char *spn );

char *trim_nonalpha( char *text );
char *trim_white( char *text );

void *get_element( REGION *ireg, int x, int y, int b );

const char *decode_bandfmt( int f );
const char *decode_type( int t );

void get_image_info( BufInfo *buf, const char *name );

void expand_variables( const char *in, char *out );
void nativeize_path( char *buf );
void absoluteize_path( char *path );
void canonicalize_path( char *path );
const char *get_vipshome( const char *argv0 );

typedef void *(*callv_string_fn)( const char *name, void *a, void *b, void *c );

void *callv_string( callv_string_fn fn, 
	const char *name, void *a, void *b, void *c );
void *callv_stringva( callv_string_fn fn, 
	const char *name, va_list ap, void *a, void *b, void *c );
void *callv_stringf( callv_string_fn fn, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));

void *callv_string_filename( callv_string_fn fn, 
	const char *filename, void *a, void *b, void *c );
void *callv_string_filenameva( callv_string_fn fn, 
	const char *name, va_list ap, void *a, void *b, void *c );
void *callv_string_filenamef( callv_string_fn fn, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));

typedef int (*calli_string_fn)( const char *name, void *a, void *b, void *c );

int calli_string( calli_string_fn fn, 
	const char *name, void *a, void *b, void *c );
int calli_stringva( calli_string_fn fn, 
	const char *name, va_list ap, void *a, void *b, void *c );
int calli_stringf( calli_string_fn fn, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));

int calli_string_filename( calli_string_fn fn, 
	const char *filename, void *a, void *b, void *c );
int calli_string_filenameva( calli_string_fn fn, 
	const char *name, va_list ap, void *a, void *b, void *c );
int calli_string_filenamef( calli_string_fn fn, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));

char *f2utf8( const char *filename );

char *im_strdupn( const char *str );
void setenvf( const char *name, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
gboolean existsf( const char *name, ... )
	__attribute__((format(printf, 1, 2)));
gboolean mkdirf( const char *name, ... )
	__attribute__((format(printf, 1, 2)));
int systemf( const char *fmt, ... )
	__attribute__((format(printf, 1, 2)));
FILE *popenf( const char *fmt, const char *mode, ... )
	__attribute__((format(printf, 1, 3)));
gboolean touchf( const char *fmt, ... )
	__attribute__((format(printf, 1, 2)));
int unlinkf( const char *fmt, ... )
	__attribute__((format(printf, 1, 2)));
gboolean is_absolute( const char *fname );
gboolean is_valid_filename( const char *name );

/* Text IO to/from files. Track the filename too, to help error messages.
 */
typedef struct _iOpenFile {
	FILE *fp;	
	char *fname;		/* File we were passed to make this open_file */
	char *fname_real;	/* File we opened (maybe after search) */
	int last_errno;		/* On error, last value for errno */
	gboolean read;		/* True for open read, false for open write */
} iOpenFile;

void file_close( iOpenFile *of );
iOpenFile *file_open_read( const char *name, ... )
	__attribute__((format(printf, 1, 2)));
iOpenFile *file_open_read_stdin();
iOpenFile *file_open_write( const char *name, ... )
	__attribute__((format(printf, 1, 2)));
gboolean file_write( iOpenFile *of, const char *fmt, ... )
	__attribute__((format(printf, 2, 3)));
gboolean file_write_var( iOpenFile *of, const char *name, const char *value );
char *file_read( iOpenFile *of );
char *file_read_buffer( iOpenFile *of, char *buffer, size_t len );
int file_getc( iOpenFile *of );

double directory_size( const char *dirname );

char *escape_percent( const char *in, char *out, int len );
char *escape_markup( const char *in, char *out, int len );
char *escape_mode( const char *in, char *out, int len );
char *break_token( char *str, const char *brk );
const char *rpt( char ch, int n );
const char *spc( int n );
void number_to_string( int n, char *buf );

double find_space( const char *name );
void to_size( BufInfo *buf, double sz );
gboolean temp_name( char *name, const char *ext );
int findmaxmin( IMAGE *in, 
	int left, int top, int width, int height, double *min, double *max );

gboolean char_to_bool( char *str, void *out );
char *bool_to_char( gboolean b );

void increment_name( char *buf );
void increment_filename( char *filename );

int extract_first_line( char *buf, char *str, int len );

void name_from_filename( const char *in, char *out );

void util_check_all_destroyed( void );

void *imalloc( IMAGE *im, size_t len );

GSList *recent_add( GSList *recent, const char *filename );
GSList *recent_load( const char *filename );
void recent_free( GSList *recent );
void recent_save( GSList *recent, const char *filename );

const char *get_savedir( void );
