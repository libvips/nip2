/* make and manage windows ... subclass off this for dialog boxes
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

#ifndef IWINDOW_H
#define IWINDOW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TYPE_IWINDOW (iwindow_get_type())
#define IWINDOW( obj ) (GTK_CHECK_CAST( (obj), TYPE_IWINDOW, iWindow ))
#define IWINDOW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_IWINDOW, iWindowClass ))
#define IS_IWINDOW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_IWINDOW ))
#define IS_IWINDOW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_IWINDOW ))
#define IWINDOW_GET_CLASS( obj ) \
	(GTK_CHECK_GET_CLASS( (obj), TYPE_IWINDOW, iWindowClass ))

typedef struct _iWindow iWindow;

/* Our cursor shapes. 
 */
typedef enum _iWindowShape {
	/* Tool shapes.
	 */
	IWINDOW_SHAPE_DROPPER = 0,
	IWINDOW_SHAPE_PEN,
	IWINDOW_SHAPE_SMUDGE,
	IWINDOW_SHAPE_SMEAR,
	IWINDOW_SHAPE_TEXT,
	IWINDOW_SHAPE_RECT,
	IWINDOW_SHAPE_FLOOD,
	IWINDOW_SHAPE_MOVE,
	IWINDOW_SHAPE_EDIT,
	IWINDOW_SHAPE_MAGIN,	
	IWINDOW_SHAPE_MAGOUT,	

	/* Resize shapes.
	 */
	IWINDOW_SHAPE_TOP,
	IWINDOW_SHAPE_BOTTOM,
	IWINDOW_SHAPE_LEFT,
	IWINDOW_SHAPE_RIGHT,
	IWINDOW_SHAPE_TOPRIGHT,
	IWINDOW_SHAPE_TOPLEFT,
	IWINDOW_SHAPE_BOTTOMRIGHT,
	IWINDOW_SHAPE_BOTTOMLEFT,

	/* Watch positions.
	 */
	IWINDOW_SHAPE_HGLASS1,
	IWINDOW_SHAPE_HGLASS2,
	IWINDOW_SHAPE_HGLASS3,
	IWINDOW_SHAPE_HGLASS4,
	IWINDOW_SHAPE_HGLASS5,
	IWINDOW_SHAPE_HGLASS6,
	IWINDOW_SHAPE_HGLASS7,
	IWINDOW_SHAPE_HGLASS8,

	/* No shape set (shape we inherit).
	 */
	IWINDOW_SHAPE_NONE,

	IWINDOW_SHAPE_LAST
} iWindowShape;

/* Keep a set of these, one for each of the clients who might want to set the
 * shape for a window.
 */
typedef struct {
	iWindow *iwnd;
	int priority;			/* Higher priority == more on top */
	const char *name;		/* For debugging */

	/* Shape currently requested by this user.
	 */
	iWindowShape shape;
} iWindowCursorContext;

/* The result from a window/dialog/whatever ... not just a bool.
 */
typedef enum iwindow_result {
	IWINDOW_ERROR = 0,		/* Tried but failed */
	IWINDOW_YES,			/* User tried the action */
	IWINDOW_NO			/* User cancelled */
} iWindowResult;

/* Our callbacks don't return iWindowResult, instead
 * they are given a notify function (plus an environment parameter) which
 * they use to inform their caller of their iWindowResult.
 */
typedef void (*iWindowNotifyFn)( void *, iWindowResult );

/* What our callbacks look like.
 */
typedef void (*iWindowFn)( iWindow *, void *, iWindowNotifyFn, void * );

/* Build function for window contents.
 */
typedef void (*iWindowBuildFn)( iWindow *, 
	GtkWidget *, void *, void *, void * );

/* A suspension ... an iWindowFn plus a set of args we are saving for later.
 */
typedef struct {
	iWindowFn fn;
	iWindow *iwnd;
	void *client;			
	iWindowNotifyFn nfn;
	void *sys;
} iWindowSusp;

#define IWINDOW_SUSP( X ) ((iWindowSusp *) (X))

struct _iWindow {
	GtkWindow parent_object;

	/* Parent window. Used for (eg.) image displays windows which we need
	 * to float over the main workspace.
	 */
	GtkWidget *parent;		/* Our parent widget */
	iWindow *parent_window;		/* Our parent window */
	guint parent_unmap_sid; 	/* Watch parent death here */

	GtkWidget *work;
	GtkAccelGroup *accel_group;
	Infobar *infobar;

	char *title;

	/* Action stuff. We init this and add a few common actions to help out
	 * subclasses.
	 */
	GtkActionGroup *action_group;
	GtkUIManager *ui_manager;

	/* Per instance build function.
	 */
	iWindowBuildFn build;
	void *build_a, *build_b, *build_c;

	/* Called before cancellable popdown ... _TRUE from this will 
	 * destroy window, _FALSE won't, _ERROR won't and pops an error box.
	 */
	iWindowFn popdown;
	void *popdown_a;

	/* Notify handling.
	 */
	gboolean destroy;		/* True if being destroyed */
	int pending;			/* Number of notifies waiting on */

	/* Cursor handling.
	 */
	iWindowShape shape;		/* Global shape ... for hglass */
	GSList *contexts;		/* Set of other requested shapes */
	GdkWindow *work_window;		/* The window we actually set */

	/* Size memorization.
	 */
	const char *width_pref;		/* Prefs we save width/height in */
	const char *height_pref;
};

typedef struct _iWindowClass {
	GtkWindowClass parent_class;

	/* Per class build/popdown functions.
	 */
	void (*build)( GtkWidget * );
	void (*popdown)( GtkWidget * );

	/* Whether windows of this class should be marked as transient for
	 * their parents (eg. dialogs usually are).
	 */
	gboolean transient;
} iWindowClass;

int iwindow_number( void );
iWindow *iwindow_pick_one( void );
typedef void (*iWindowMapFn)( iWindow *, void * );
void *iwindow_map_all( iWindowMapFn fn, void *a );

iWindowCursorContext *iwindow_cursor_context_new( iWindow *, 
	int, const char * );
void iwindow_cursor_context_set_cursor( iWindowCursorContext *, iWindowShape );
void iwindow_cursor_context_destroy( iWindowCursorContext *cntxt );

iWindowSusp *iwindow_susp_new( iWindowFn, 
	iWindow *, void *, iWindowNotifyFn, void * );
void iwindow_susp_trigger( iWindowSusp * ); 
void iwindow_susp_return( void *, iWindowResult );
void iwindow_susp_comp( void *, iWindowResult );

void iwindow_notify_send( iWindow *iwnd, 
	iWindowFn fn, void *client, iWindowNotifyFn back, void *sys );
void iwindow_notify_return( iWindow *iwnd );

void iwindow_true_cb( iWindow *, void *, iWindowNotifyFn nfn, void *sys );
void iwindow_false_cb( iWindow *, void *, iWindowNotifyFn nfn, void *sys );
void iwindow_notify_null( void *client, iWindowResult result );

GType iwindow_get_type( void );

GtkWidget *iwindow_new( GtkWindowType );
void iwindow_set_title( iWindow *, const char *, ... )
	__attribute__((format(printf, 2, 3)));
void iwindow_set_build( iWindow *, iWindowBuildFn, void *, void *, void * );
void iwindow_set_popdown( iWindow *, iWindowFn, void * );
void iwindow_set_size_prefs( iWindow *, const char *, const char * );
void iwindow_set_work_window( iWindow *iwnd, GdkWindow *work_window );
void iwindow_set_parent( iWindow *, GtkWidget *par );

void iwindow_build( iWindow * );
void *iwindow_kill( iWindow * );
void iwindow_kill_action_cb( GtkAction *action, iWindow *iwnd );

GtkWidget *iwindow_get_root( GtkWidget *widget );
GtkWidget *iwindow_get_root_noparent( GtkWidget *widget );
void iwindow_alert( GtkWidget *parent, GtkMessageType type );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IWINDOW_H */
