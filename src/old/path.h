/* Declarations supporting search.c
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

extern GSList *path_search_default;
extern GSList *path_start_default;
extern const char *path_tmp_default;

/* Type of path_map functions.
 */
typedef void *(*path_map_fn)( const char *, void *, void *, void * );

void path_rewrite_free_all( void );
void path_rewrite_add( const char *old, const char *new, gboolean lock );
void path_rewrite( char *buf );
void path_compact( char *path );
void path_expand( char *path );
char *path_rewrite_file( const char *patt );

GSList *path_parse( const char *path );
char *path_unparse( GSList *path );
void path_free2( GSList *path );
void *path_map( GSList *path, const char *patt, path_map_fn fn, void *a );
void *path_map_dir( const char *dir, const char *patt, 
	path_map_fn fn, void *a );
char *path_find_file( const char *patt );

void path_init( void );
