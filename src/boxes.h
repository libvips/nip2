/* decls for boxes.
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

void box_alert( GtkWidget *par );
void box_vinfo( GtkWidget *par, const char *top, const char *sub, va_list ap );
void box_info( GtkWidget *par, const char *top, const char *sub, ... )
	__attribute__((format(printf, 3, 4)));
iDialog *box_yesno( GtkWidget *par, 
	iWindowFn okcb, iWindowFn cancelcb, void *client, /* Call client */
	iWindowNotifyFn nfn, void *sys,			  /* Call parent */
	const char *yes_label, 
	const char *top, const char *sub, ... )
	__attribute__((format(printf, 9, 10)));
void box_savenosave( GtkWidget *par, 
	iWindowFn save, iWindowFn nosave, void *client,   /* Call client */
	iWindowNotifyFn nfn, void *sys,			  /* Call parent */
	const char *top, const char *sub, ... )
	__attribute__((format(printf, 8, 9)));
void box_about( GtkWidget *par );
void box_help( GtkWidget *par, const char *name );

/* A dialog showing a bunch of editable strings ... eg. name and caption for
 * new toolkit etc. etc.
 */
#define TYPE_STRINGSET (stringset_get_type())
#define STRINGSET( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_STRINGSET, Stringset ))
#define STRINGSET_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_STRINGSET, StringsetClass ))
#define IS_STRINGSET( obj ) (GTK_CHECK_TYPE( (obj), TYPE_STRINGSET ))
#define IS_STRINGSET_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_STRINGSET ))

/* A Stringset is a bunch of these.
 */
typedef struct {
	struct _Stringset *ss;

	GtkWidget *entry;	
	char *label;	
	char *text;	/* Current text value */
	char *tooltip;
} StringsetChild;

typedef struct _Stringset {
	iDialog parent;

	GSList *children;
	GtkSizeGroup *group;	/* Align labels with this */     
} Stringset;

typedef struct _StringsetClass {
	iDialogClass parent_class;

	/* My methods.
	 */
} StringsetClass;

void *stringset_child_destroy( StringsetChild *ssc );
StringsetChild *stringset_child_new( Stringset *ss,
	const char *label, const char *text, const char *tooltip );
GtkType stringset_get_type( void );
GtkWidget *stringset_new( void );
StringsetChild *stringset_child_get( Stringset *, const char *label );

/* Find dialog.
 */
#define TYPE_FIND (find_get_type())
#define FIND( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_FIND, Find ))
#define FIND_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_FIND, FindClass ))
#define IS_FIND( obj ) (GTK_CHECK_TYPE( (obj), TYPE_FIND ))
#define IS_FIND_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_FIND ))

typedef struct _Find {
	iDialog parent;

	/* My instance vars.
	 */
	GtkWidget *search;
#ifdef HAVE_GREGEX
	GtkWidget *regexp;
#endif /*HAVE_GREGEX*/
	GtkWidget *csens;
	GtkWidget *fromtop;
} Find;

typedef struct _FindClass {
	iDialogClass parent_class;

	/* My methods.
	 */
} FindClass;

GtkType find_get_type( void );
GtkWidget *find_new( void );

/* Image header dialog.
 */
#define TYPE_IMAGEHEADER (imageheader_get_type())
#define IMAGEHEADER( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_IMAGEHEADER, Imageheader ))
#define IMAGEHEADER_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_IMAGEHEADER, ImageheaderClass ))
#define IS_IMAGEHEADER( obj ) (GTK_CHECK_TYPE( (obj), TYPE_IMAGEHEADER ))
#define IS_IMAGEHEADER_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_IMAGEHEADER ))

typedef struct _Imageheader {
	iDialog parent;

	Conversion *conv;
	GtkWidget *swin_fields;
	GtkWidget *fields;
	GtkSizeGroup *group;
	GtkWidget *history;
} Imageheader;

typedef struct _ImageheaderClass {
	iDialogClass parent_class;

	/* My methods.
	 */
} ImageheaderClass;

GtkType imageheader_get_type( void );
GtkWidget *imageheader_new( Conversion *conv );

void box_url( GtkWidget *par, const char *url );

/* Splash window.
 */
#define TYPE_SPLASH (splash_get_type())
#define SPLASH( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_SPLASH, Splash ))
#define SPLASH_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_SPLASH, SplashClass ))
#define IS_SPLASH( obj ) (GTK_CHECK_TYPE( (obj), TYPE_SPLASH ))
#define IS_SPLASH_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_SPLASH ))

/* The file we make to suppress the splash screen.
 */
#define NO_SPLASH "no_splash"

typedef struct _Splash {
	GtkWindow parent_object;

	GtkWidget *progress;		/* Display progress here */
	guint update_sid;		/* Link to update messages */
} Splash;

typedef struct _SplashClass {
	GtkWindowClass parent_class;

	/* My methods.
	 */
} SplashClass;

GtkType splash_get_type( void );
Splash *splash_new( void );

/* Font chooser window.
 */
#define TYPE_FONTCHOOSER (fontchooser_get_type())
#define FONTCHOOSER( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_FONTCHOOSER, Fontchooser ))
#define FONTCHOOSER_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_FONTCHOOSER, FontchooserClass ))
#define IS_FONTCHOOSER( obj ) (GTK_CHECK_TYPE( (obj), TYPE_FONTCHOOSER ))
#define IS_FONTCHOOSER_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_FONTCHOOSER ))

typedef struct _Fontchooser {
	iDialog parent_object;

	GtkWidget *fontchooser;		/* gtk font select widget */
} Fontchooser;

typedef struct _FontchooserClass {
	iDialogClass parent_class;

	/* My methods.
	 */
} FontchooserClass;

GtkType fontchooser_get_type( void );
Fontchooser *fontchooser_new( void );
gboolean fontchooser_set_font_name( Fontchooser *fontchooser, 
	const char *font_name );
char *fontchooser_get_font_name( Fontchooser * );

/* Font button.
 */
#define TYPE_FONTBUTTON (fontbutton_get_type())
#define FONTBUTTON( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_FONTBUTTON, Fontbutton ))
#define FONTBUTTON_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_FONTBUTTON, FontbuttonClass ))
#define IS_FONTBUTTON( obj ) (GTK_CHECK_TYPE( (obj), TYPE_FONTBUTTON ))
#define IS_FONTBUTTON_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_FONTBUTTON ))

typedef struct _Fontbutton {
	GtkButton parent_object;

	char *font_name;		/* Current name */

	Fontchooser *fontchooser;	/* Pop up dialog */
} Fontbutton;

typedef struct _FontbuttonClass {
	GtkButtonClass parent_class;

	void (*changed)( Fontbutton * );
} FontbuttonClass;

GtkType fontbutton_get_type( void );
Fontbutton *fontbutton_new( void );
void fontbutton_set_font_name( Fontbutton *fontbutton, const char *font_name );
const char *fontbutton_get_font_name( Fontbutton * );

/* Infobar subclass, with a close animation and a label.
 */
#define TYPE_INFOBAR (infobar_get_type())
#define INFOBAR( obj ) \
	(GTK_CHECK_CAST( (obj), TYPE_INFOBAR, Infobar ))
#define INFOBAR_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_INFOBAR, InfobarClass ))
#define IS_INFOBAR( obj ) (GTK_CHECK_TYPE( (obj), TYPE_INFOBAR ))
#define IS_INFOBAR_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_INFOBAR ))

struct _Infobar {
#ifdef USE_INFOBAR
	GtkInfoBar parent_object;
#endif /*USE_INFOBAR*/

	GtkWidget *top;
	GtkWidget *sub;
	GtkWidget *info;
	guint close_timeout;
	guint close_animation_timeout;
	int height;
};

typedef struct _InfobarClass {
#ifdef USE_INFOBAR
	GtkInfoBarClass parent_class;
#endif /*USE_INFOBAR*/

} InfobarClass;

GtkType infobar_get_type( void );
Infobar *infobar_new( void );
void infobar_vset( Infobar *infobar, GtkMessageType type, 
	const char *top, const char *sub, va_list ap );
void infobar_set( Infobar *infobar, GtkMessageType type, 
	const char *top, const char *sub, ... );
