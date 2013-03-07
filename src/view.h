/* abstract base class for our UI widgets
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

#define TYPE_VIEW (view_get_type())
#define VIEW( obj ) (GTK_CHECK_CAST( (obj), TYPE_VIEW, View ))
#define VIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_VIEW, ViewClass ))
#define IS_VIEW( obj ) (GTK_CHECK_TYPE( (obj), TYPE_VIEW ))
#define IS_VIEW_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_VIEW ))
#define VIEW_GET_CLASS( obj ) \
	(GTK_CHECK_GET_CLASS( (obj), TYPE_VIEW, ViewClass ))

/* We track all of the children of our model, listening to "changed", so we
 * can lazily add or remove child views of us as the model requests.
 */
typedef struct {
	View *parent_view;		/* Us */
	Model *child_model;		/* The child we are watching */
	guint child_model_changed_sid;	/* Listen to "changed" on child here */
	View *child_view;		/* The child view for this model */
} ViewChild;

struct _View {
	vObject parent_object;

	/* My instance vars.
	 */
	guint pos_changed_sid;		/* Signals we use to watch iObject */
	guint scrollto_sid;
	guint layout_sid;
	guint reset_sid;
	guint child_add_sid;
	guint child_remove_sid;

	View *parent;			/* Enclosing view (if any) */
	GSList *managed;		/* List of ViewChild for us */

	gboolean scannable;		/* On scannable list */
	gboolean resettable;		/* On resettable list */
};

typedef struct _ViewClass {
	vObjectClass parent_class;

	/* Create/destroy

		link 		this view is about to be linked to this model
				with this parent view

		child_add	this view has just gained a child 

		child_remove	this view is about to lose a child 

		child_position	this child needs repositioning

		child_front	pop this child to the front

		display		should this child be displayed
	
	 */

	void (*link)( View *, Model *, View * );
	void (*child_add)( View *parent, View *child );
	void (*child_remove)( View *parent, View *child );
	void (*child_position)( View *parent, View *child );
	void (*child_front)( View *parent, View *child );
	gboolean (*display)( View *parent, Model *child );

	/* State change

		reset		reset edit mode ... eg. text pops back to 
				value display

		scan		scan widgets, reading any new text off the
				display

		scrollto	try to make this view visible

		layout		try to lay children out

	 */
	void (*reset)( View * );
	void *(*scan)( View * );	
	void (*scrollto)( View *, ModelScrollPosition );
	void (*layout)( View * );
} ViewClass;

void view_scannable_register( View *view );
void view_scannable_unregister( View *view );
gboolean view_scan_all( void );

void view_resettable_register( View *view );
void view_resettable_unregister( View *view );
void view_reset_all( void );

gboolean view_hasmodel( View *view );
void *view_model_test( View *child, Model *model );

GtkType view_get_type( void );

void view_link( View *view, Model *model, View *parent );
void view_unlink( View *view );
void view_child_add( View *parent, View *child );
void view_child_remove( View *child );
void view_child_position( View *child );
void view_child_front( View *child );

void *view_reset( View *view );
void *view_scan( View *view );
void *view_scrollto( View *view, ModelScrollPosition );
void *view_layout( View *view );

void *view_map( View *view, view_map_fn fn, void *a, void *b );
void *view_map_all( View *view, view_map_fn fn, void *a );

void view_save_as_cb( GtkWidget *menu, GtkWidget *host, View *view );
void view_save_cb( GtkWidget *menu, GtkWidget *host, View *view );
void view_close_cb( GtkWidget *menu, GtkWidget *host, View *view );

void view_activate_cb( View *view );
void view_changed_cb( View *view );

void view_not_implemented_cb( GtkWidget *menu, GtkWidget *host, View *view );

GtkWidget *view_get_toplevel( View *view );

Columnview *view_get_columnview( View *child );
void *view_resize( View *view );
