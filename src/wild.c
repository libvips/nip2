/* Regular expression match
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

/*
#define DEBUG
 */

#include "ip.h"

#ifdef HAVE_REGEXEC

void
wild_destroy( Wild *wild )
{
	if( wild ) {
		regfree( wild );
		im_free( wild );
	}
}

/* Turn a shell-style regular expression into an ex-style regular expression.
 * We also put ^$ around the pattern, to make sure we match the whole filename.
 */
static void
wild_translate_pattern( const char *s, char *buf )
{	
	char *p = buf;

	*p++ = '^';
	for( ; *s; s++ )
		switch( *s ) {
		case '*':
			*p++ = '.';
			*p++ = '*';
			break;

		case '.':
			*p++ = '\\';
			*p++ = '.';
			break;

		case '?':
			*p++ = '.';
			break;

		case '{':
			*p++ = '(';
			break;

		case '}':
			*p++ = ')';
			break;

		case ',':
			*p++ = '|';
			break;

		default:
			*p++ = *s;
		}
	*p++ = '$';
	*p++ = '\0';
}

Wild *
wild_new( const char *patt )
{
	Wild *wild;
	char tpatt[5000];

	/* How dumb.
	 */
	assert( strlen( patt ) < 4000 );

	wild_translate_pattern( patt, tpatt );
	wild = INEW( NULL, Wild );
	if( regcomp( wild, tpatt, 
		REG_EXTENDED | REG_NOSUB | REG_ICASE ) != 0 ) {
		wild_destroy( wild );
		error_top( _( "Parse error." ) );
		error_sub( _( "Bad regular expression \"%s\"." ), patt );
		return( NULL );
	}

#ifdef DEBUG
	printf( "wild_new: \"%s\"\n", patt );
	printf( "  using regexp \"%s\"\n", tpatt );
#endif /*DEBUG*/

	return( wild );
}

gboolean
wild_match( Wild *wild, const char *text )
{
	int result;

	result = regexec( wild, text, 0, NULL, 0 );

#ifdef DEBUG
	printf( "wild_match: \"%s\" == %d\n", text, result );
#endif /*DEBUG*/

	return( result == 0 ) ;
}

#else /*!HAVE_REGEXEC*/

/*
**  Extracted from li_wild.c July 18, 1997
**		
*/

void
wild_destroy( Wild *wild )
{
	IM_FREE( wild );
}

Wild *
wild_new( const char *patt )
{
	/* Should test patt for syntax :-(
	 */
	return( im_strdupn( patt ) );
}

static int
wild_internal_match( char *p, char *text )
{
  int last;
  int matched;
  int reverse;
  int submatch;
  int len;
  char *s, *subpat;

  for (; *p; text++, p++)
    {
      if (*text == '\0' && *p != '*')
	return (-1);

      switch (*p)
	{
	case '\\':
	  /* Literal match with following character. 
	   */
	  p++;

	  /* FALLTHROUGH 
	   */
	default:
	  if (*text != *p)
	    return FALSE;

	  continue;

	case '?':
	  /* Match anything. 
	   */
	  continue;

	case '*':
	  /* Consecutive stars act just like one. 
	   */
	  while (*++p == '*')
	    continue;

	  if (*p == '\0')
	    /* Trailing star matches everything. 
	     */
	    return (TRUE);

	  while (*text)
	    if ((matched = wild_match (p, text++)) != FALSE)
	      return (matched);

	  return -1;

	case '{':
	  /* choices of patterns separated by , 
	   */

	  p++;
	  do
	    {
	      char *s2;

	      s = strchr (p, ',');
	      s2 = strchr (p, '}');

	      if (!s || (s2 && s2 < s))
		s = s2;

	      if (s)
		{
		  len = (long) (s - p);
		  subpat = alloca (len + 2);
		  strncpy (subpat, p, len);
		  subpat[len] = '*';
		  subpat[len + 1] = '\0';

#ifdef TEST
		  printf ("Matching %s to %s\n", text, subpat);
#endif

		  if ((submatch = wild_match (subpat, text)) == TRUE)
		    {
		      text += len - 1;
		      p = strchr (s, '}');
		      if (!p)
			return -1;

		      if (!*(p + 1) && !*(text + 1))
			return (TRUE);

		      break;
		    }
		  else if (submatch == -1)
		    return (-1);
		  p = s + 1;
		}
	    }
	  while (s);

	  if (submatch != TRUE)
	    return (submatch);

	  continue;

	case '[':
	  reverse = p[1] == '^' ? TRUE : FALSE;
	  if (reverse)
	    /* Inverted character class. 
	     */
	    p++;

	  matched = FALSE;

	  if (p[1] == ']' || p[1] == '-')
	    if (*++p == *text)
	      matched = TRUE;

	  for (last = *p; *++p && *p != ']'; last = *p)
	    /* This next line requires a good C compiler.
	     */

	    if (*p == '-' && p[1] != ']'
		? *text <= *++p && *text >= last : *text == *p)
	      matched = TRUE;

	  if (matched == reverse)
	    return FALSE;

	  continue;
	}
    }

  return *text == '\0';
}

gboolean
wild_match( Wild *wild, const char *text )
{
	return( wild_internal_match( wild, (char *) text ) == TRUE );
}

#endif /*HAVE_REGEXEC*/

/* Convenience function.
 */
gboolean 
wild_match_patt( const char *patt, const char *text )
{
	Wild *wild;
	gboolean match;

	if( !(wild = wild_new( patt )) )
		return( FALSE );
	match = wild_match( wild, text );
	wild_destroy( wild );

	return( match );
}
