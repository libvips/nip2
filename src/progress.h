/* Handle feedback about eval progress.
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

/* The max size of the feedback message.
 */
#define PROGRESS_FEEDBACK_SIZE (100)

#define TYPE_PROGRESS (progress_get_type())
#define PROGRESS( obj ) \
	(G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_PROGRESS, Progress ))
#define PROGRESS_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_PROGRESS, ProgressClass))
#define IS_PROGRESS( obj ) \
	(G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_PROGRESS ))
#define IS_PROGRESS_CLASS( klass ) \
	(G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_PROGRESS ))
#define PROGRESS_GET_CLASS( obj ) \
	(G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_PROGRESS, ProgressClass ))

typedef struct _Progress {
	iObject parent_object;

	/* Nest progress_begin() calls with this.
	 */
	int count;

	/* How long we've been busy, time since last update.
	 */
	GTimer *busy_timer;
	GTimer *update_timer;

	/* Trying to cancel.
	 */
	gboolean cancel;

	/* In the "busy" state, ie. we've emitted "begin" and so we need to
	 * emit "end" on progress_end().
	 */
	gboolean busy;

	/* The feedback message we suggest, percent for progress bar.
	 */
	VipsBuf feedback;
	char buf[PROGRESS_FEEDBACK_SIZE];
	int percent;
} Progress;

typedef struct _ProgressClass {
	iObjectClass parent_class;

	/* Entering busy state: display progress bar, change cursor, etc.
	 */
	void (*begin)( Progress * );

	/* Progress update. Set cancel to cancel computation.
	 */
	void (*update)( Progress *, gboolean *cancel );

	/* End busy state. Restore screen.
	 */
	void (*end)( Progress * );
} ProgressClass;

/* Called from all over nip2 as computation proceeds.
 */
void progress_begin( void );
gboolean progress_update_percent( int percent, int eta );
gboolean progress_update_expr( Expr *expr );
gboolean progress_update_loading( int percent, const char *filename );
gboolean progress_update_tick( void );
void progress_end( void );

GType progress_get_type( void );
Progress *progress_get( void );
