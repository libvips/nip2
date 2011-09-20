/* Search paths for files.
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

/* just load .defs/.wses from "."
#define DEBUG_LOCAL
 */

/* show path searches
#define DEBUG_SEARCH
 */

#include "ip.h"

/* List of directories we have visited this session. Added to by fsb stuff.
 */
GSList *path_session = NULL;

/* Default search paths if prefs fail.
 */
GSList *path_start_default = NULL;
GSList *path_search_default = NULL;
const char *path_tmp_default = NULL;

/* Turn a search path (eg. "/pics/lr:/pics/hr") into a list of directory names.
 */
GSList *
path_parse( const char *path )
{
	GSList *op = NULL;
	const char *p;
	const char *e;
	int len;
	char name[FILENAME_MAX + 1];

	for( p = path; *p; p = e ) {
		/* Find the start of the next component, or the NULL
		 * character.
		 */
		if( !(e = strchr( p, G_SEARCHPATH_SEPARATOR )) )
			e = p + strlen( p );
		len = e - p + 1;

		/* Copy to our buffer, turn to string.
		 */
		im_strncpy( name, p, IM_MIN( len, FILENAME_MAX ) );

		/* Add to path list.
		 */
		op = g_slist_append( op, im_strdupn( name ) );

		/* Skip G_SEARCHPATH_SEPARATOR.
		 */
		if( *e == G_SEARCHPATH_SEPARATOR )
			e++;
	}

	return( op );
}

/* Free a path. path_free() is reserved n OS X :(
 */
void
path_free2( GSList *path )
{
	slist_map( path, (SListMapFn) im_free, NULL );
	g_slist_free( path );
}

/* Sub-fn of below. Add length of this string + 1 (for ':').
 */
static int
path_add_component( const char *str, int c )
{
	return( c + strlen( str ) + 1 );
}

/* Sub-fn of below. Copy string to buffer, append ':', return new end.
 */
static char *
path_add_string( const char *str, char *buf )
{
	strcpy( buf, str );
	strcat( buf, G_SEARCHPATH_SEPARATOR_S );

	return( buf + strlen( buf ) );
}

/* Turn a list of directory names into a search path.
 */
char *
path_unparse( GSList *path )
{	
	int len = GPOINTER_TO_INT( slist_fold( path, 0, 
		 (SListFoldFn) path_add_component, NULL ) );
	char *buf = imalloc( NULL, len + 1 );

	/* Build new string.
	 */
	slist_fold( path, buf, (SListFoldFn) path_add_string, NULL );

	/* Fix '\0' to remove trailing G_SEARCHPATH_SEPARATOR.
	 */
	if( len > 0 )
		buf[len - 1] = '\0';

	return( buf );
}

/* Track this stuff during a file search.
 */
typedef struct _Search {
	/* Pattern we search for, and it's compiled form. This does not
	 * include any directory components. 
	 */
	char *basename;
	GPatternSpec *wild;

	/* Directory offset. If the original pattern is a relative path, eg.
	 * "poop/x*.v", we search every directory on path for a subdirectory
	 * called "poop" and then search all files within that.
	 */
	char *dirname; 

	/* User function to call for every matching file.
	 */
	path_map_fn fn;
	void *a;

	/* Files we've previously offered to the user function: we remove
	 * duplicates. So "path1/wombat.def" hides "path2/wombat.def".
	 */
	GSList *previous;
} Search;

static void
path_search_free( Search *search )
{
	IM_FREEF( g_free, search->basename );
	IM_FREEF( g_free, search->dirname );
	IM_FREEF( slist_free_all, search->previous );
	IM_FREEF( g_pattern_spec_free, search->wild );
}

static gboolean
path_search_init( Search *search, const char *patt, path_map_fn fn, void *a )
{
	search->basename = g_path_get_basename( patt );
	search->dirname = g_path_get_dirname( patt );
	search->wild = NULL;
	search->fn = fn;
	search->a = a;
	search->previous = NULL;

	if( !(search->wild = g_pattern_spec_new( search->basename )) ) {
		path_search_free( search );
		return( FALSE );
	}

	return( TRUE );
}

static void *
path_str_eq( const char *s1, const char *s2 )
{
        if( strcmp( s1, s2 ) == 0 )
                return( (void *) s1 );
        else
                return( NULL );
}

/* Test for string matches pattern. If the match is successful, call a user 
 * function.
 */
static void *
path_search_match( Search *search, const char *dir_name, const char *name )
{
	if( g_pattern_match_string( search->wild, name ) &&
		!slist_map( search->previous, 
			(SListMapFn) path_str_eq, (gpointer) name ) ) {
		char buf[FILENAME_MAX + 10];
		void *result;

		/* Add to exclusion list.
		 */
		search->previous = 
			g_slist_prepend( search->previous, g_strdup( name ) );

		im_snprintf( buf, FILENAME_MAX, 
			"%s" G_DIR_SEPARATOR_S "%s", dir_name, name );
#ifdef DEBUG_SEARCH
		printf( "path_search_match: matched \"%s\"\n", buf );
#endif /*DEBUG_SEARCH*/

		if( (result = search->fn( buf, search->a, NULL, NULL )) )
			return( result );
	}

	return( NULL );
}

/* Scan a directory, calling a function for every entry. Abort scan if 
 * function returns non-NULL.
 */
static void *
path_scan_dir( const char *dir_name, Search *search )
{
	char buf[FILENAME_MAX];
	GDir *dir;
	const char *name;
	void *result;

	/* Add the pattern offset, if any. It's '.' for no offset.
	 */
	im_snprintf( buf, FILENAME_MAX, 
		"%s" G_DIR_SEPARATOR_S "%s", dir_name, search->dirname );

	if( !(dir = (GDir *) callv_string_filename( 
		(callv_string_fn) g_dir_open, buf, NULL, NULL, NULL )) ) 
		return( NULL );

	while( (name = g_dir_read_name( dir )) ) 
		if( (result = path_search_match( search, buf, name )) ) {
			g_dir_close( dir );
			return( result );
		}

	g_dir_close( dir );

	return( NULL );
}

/* Scan a search path, applying a function to every file name which matches a
 * pattern. If the user function returns NULL, keep looking, otherwise return
 * its result. We return NULL on error, or if the user function returns NULL
 * for all filenames which match. 
 *
 * Remove duplicates: if fred.wombat is in the first and second dirs on the
 * path, only apply to the first occurence.

 	FIXME ... speed up with a hash and a (date based) cache at some point

 */
void *
path_map_exact( GSList *path, const char *patt, path_map_fn fn, void *a )
{
	Search search;
	void *result;

#ifdef DEBUG_SEARCH
	printf( "path_map_exact: searching for \"%s\"\n", patt );
#endif /*DEBUG_SEARCH*/

	if( !path_search_init( &search, patt, fn, a ) )
		return( NULL );

	result = slist_map( path, (SListMapFn) path_scan_dir, &search );

	path_search_free( &search );

	return( result );
}

/* As above, but walk the session path too.
 */
void *
path_map( GSList *path, const char *patt, path_map_fn fn, void *a )
{
	Search search;
	void *result;

#ifdef DEBUG_SEARCH
	printf( "path_map: searching for \"%s\"\n", patt );
#endif /*DEBUG_SEARCH*/

	if( !path_search_init( &search, patt, fn, a ) )
		return( NULL );

	result = slist_map( path, (SListMapFn) path_scan_dir, &search );
	if( !result )
		result = slist_map( path_session, 
			(SListMapFn) path_scan_dir, &search );

	path_search_free( &search );

	return( result );
}

/* As above, but scan a single directory.
 */
void *
path_map_dir( const char *dir, const char *patt, path_map_fn fn, void *a )
{
	Search search;
	void *result;

#ifdef DEBUG_SEARCH
	printf( "path_map_dir: searching for \"%s\"\n", patt );
#endif /*DEBUG_SEARCH*/

	if( !path_search_init( &search, patt, fn, a ) )
		return( NULL );

	if( !(result = path_scan_dir( dir, &search )) ) {
		/* Not found? Maybe - error message anyway.
		 */
		error_top( _( "Not found." ) );
		error_sub( _( "File \"%s\" not found." ), patt );
	}

	path_search_free( &search );

	return( result );
}

/* Search for a file and return it's path. NULL for not found. Return a new
 * string.
 */
char *
path_find_file( GSList *path, const char *filename )
{
	char *fname;

	/* Try file name exactly.
	 */
	if( existsf( "%s", filename ) )
		return( im_strdupn( filename ) );

	/* Search everywhere.
	 */
	if( (fname = path_map( path, filename,
		(path_map_fn) im_strdupn, NULL )) )
		return( fname );
	
	error_top( _( "Not found." ) );
	error_sub( _( "File \"%s\" not found on path" ), filename );

	return( NULL );
}

/* Add a directory to the session path, if it's not there already.
 */
void
path_add_dir( const char *dir )
{
        if( !slist_map( path_session, 
		(SListMapFn) path_str_eq, (gpointer) dir ) ) {
#ifdef DEBUG_SEARCH
		printf( "path_add_dir: adding \"%s\"\n", dir );
#endif /*DEBUG_SEARCH*/
                path_session = g_slist_prepend( path_session, 
			im_strdup( NULL, dir ) );
	}
}

void
path_add_file( const char *filename )
{
	char *p;
	char buf[FILENAME_MAX];

	im_strncpy( buf, filename, FILENAME_MAX );
	if( (p = strrchr( buf, G_DIR_SEPARATOR )) )
		*p = '\0';

	path_add_dir( buf );
}

void
path_init( void )
{
#ifdef DEBUG_LOCAL
	printf( "path_init: loading start from \".\" only\n" );
	path_start_default = path_parse( "." );
	path_search_default = path_parse( "." );
	path_tmp_default = im_strdup( NULL, "." );
#else /*!DEBUG_LOCAL*/
	char buf[FILENAME_MAX];

	im_snprintf( buf, FILENAME_MAX,
		"%s" G_DIR_SEPARATOR_S "start" G_SEARCHPATH_SEPARATOR_S
		"$VIPSHOME" G_DIR_SEPARATOR_S "share" G_DIR_SEPARATOR_S
		"$PACKAGE" G_DIR_SEPARATOR_S "start",
		get_savedir() );
	path_start_default = path_parse( buf );

	im_snprintf( buf, FILENAME_MAX,
		"%s" G_DIR_SEPARATOR_S "data" G_SEARCHPATH_SEPARATOR_S
		"$VIPSHOME" G_DIR_SEPARATOR_S "share" G_DIR_SEPARATOR_S 
		"$PACKAGE" G_DIR_SEPARATOR_S "data" G_SEARCHPATH_SEPARATOR_S 
		".",
		get_savedir() );
	path_search_default = path_parse( buf );

	im_snprintf( buf, FILENAME_MAX, 
		"%s" G_DIR_SEPARATOR_S "tmp", get_savedir() );
	path_tmp_default = im_strdup( NULL, buf );
#endif /*DEBUG_LOCAL*/
}
