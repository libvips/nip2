/* make and manage dialogs ... subclass off this for dialog boxes
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

#ifndef IDIALOG_H
#define IDIALOG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TYPE_IDIALOG (idialog_get_type())
#define IDIALOG( obj ) (GTK_CHECK_CAST( (obj), TYPE_IDIALOG, iDialog ))
#define IDIALOG_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_IDIALOG, iDialogClass ))
#define IS_IDIALOG( obj ) (GTK_CHECK_TYPE( (obj), TYPE_IDIALOG ))
#define IS_IDIALOG_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_IDIALOG ))

typedef struct _iDialog iDialog;

typedef void (*iDialogFreeFn)( iDialog *, void * );

struct _iDialog {
	iWindow parent_object;

	/* My instance vars.
	 */
	iObject *iobject;		/* Kill dialog if this obj goes */
	guint destroy_sid;		/* Signal id for obj destroy */

	GtkWidget *work;		/* Our work area */
	GtkWidget *hb;			
	GtkWidget *bb;

	GSList *ok_l;			/* List of OK buttons */
	GSList *ok_txt_l;		/* List of text of OK buttons */
	GtkWidget *but_cancel;
	GtkWidget *but_help;
	GtkWidget *tog_pin;		/* Optional pinup widget */

	GtkEntry *entry;		/* Last entry we added as default */

	/* Flags.
	 */
	gboolean modal;			/* Modal/non-modal */
	gboolean pinup;			/* Stay up on OK */
	gboolean nosep;			/* Suppress hseparator */
	gboolean button_focus;		/* TRUE to focus buttons */

	/* Name of help tag ... if set, make a help button and link to display
	 * of this.
	 */
	char *help_tag;

	/* What we label the cancel button as (if any). Usually
	 * GTK_STOCK_CANCEL, but instant-apply dialogs should change this to
	 * GTK_STOCK_CLOSE.
	 */
	const char *cancel_text;

	/* Per-instance build function.
	 */
	iWindowBuildFn build;
	void *build_a, *build_b, *build_c;

	/* Our callbacks.
	 */
	GSList *ok_cb_l;		/* List of OK callbacks */
	iWindowFn cancel_cb;
	iWindowFn popdown_cb;
	iDialogFreeFn destroy_cb;	/* Called from _destroy() */
	void *client;			/* Client data for callbacks */

	void *arg;			/* Misc thing provided to client */

	/* Notify our parent when we finish.
	 */
	iWindowNotifyFn nfn;
	void *sys;
};

typedef struct _iDialogClass {
	iWindowClass parent_class;

	/* Our methods.
	 */
} iDialogClass;

void idialog_free_client( iDialog *idlg, void *client );

void idialog_set_ok_button_state( iDialog *idlg, gboolean state );

GtkType idialog_get_type( void );
GtkWidget *idialog_new( void );

void idialog_set_iobject( iDialog *idlg, iObject *iobject );
void idialog_set_modal( iDialog *, gboolean );
void idialog_set_pinup( iDialog *idlg, gboolean pinup );
void idialog_set_nosep( iDialog *, gboolean );
void idialog_set_button_focus( iDialog *idlg, gboolean button_focus );
void idialog_set_help_tag( iDialog *, const char *help_tag );
void idialog_set_callbacks( iDialog *, 
	iWindowFn cancel_cb, iWindowFn popdown_cb, 
	iDialogFreeFn destroy_cb, void *client );
void idialog_add_ok( iDialog *, iWindowFn done_cb, const char *fmt, ... )
	__attribute__((format(printf, 3, 4)));
void idialog_set_notify( iDialog *, iWindowNotifyFn, void * );
void idialog_set_build( iDialog *, iWindowBuildFn, void *, void *, void * );
void idialog_set_cancel_text( iDialog *, const char *cancel_text );
void idialog_set_default_entry( iDialog *idlg, GtkEntry *entry );
void idialog_init_entry( iDialog *idlg, GtkWidget *entry, 
	const char *tip, const char *fmt, ... )
	__attribute__((format(printf, 4, 5)));

void idialog_done_trigger( iDialog *idlg, int pos );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IDIALOG_H */
