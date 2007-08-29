/* A slider with an entry widget.
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

#define TYPE_TSLIDER (tslider_get_type())
#define TSLIDER( obj ) (GTK_CHECK_CAST( (obj), TYPE_TSLIDER, Tslider ))
#define TSLIDER_CLASS( klass ) \
	(GTK_CHECK_CLASS_CAST( (klass), TYPE_TSLIDER, TsliderClass ))
#define IS_TSLIDER( obj ) (GTK_CHECK_TYPE( (obj), TYPE_TSLIDER ))
#define IS_TSLIDER_CLASS( klass ) \
	(GTK_CHECK_CLASS_TYPE( (klass), TYPE_TSLIDER ))

typedef double (*tslider_fn)( double from, double to, double value );

typedef struct _Tslider {
	GtkHBox parent_class;

	/* Our state.
	 */
	double from;
	double to;

	double value;		/* Real value, as displayed in text */
	double svalue;		/* Slider value ... secret linear scale */
	int digits;		/* How many sf to display */

	/* Keep last from/to/value settings here. Can't
	 * use from/to since double and float don't compare reliably.
	 */
	double last_from, last_to, last_svalue;

	GtkWidget *entry;
	GtkWidget *slider;
	GtkAdjustment *adj;

	/* Optional functions ... how to make a value from a slider
	 * position, how to make a slider position from a value.
	 * If these are defined, text and slider are linked for you.
	 */
	gboolean auto_link;
	tslider_fn value_to_slider;
	tslider_fn slider_to_value;
} Tslider;

typedef struct _TsliderClass {
	GtkHBoxClass parent_class;

	void (*changed)( Tslider * );		/* from/to/value change */
	void (*activate)( Tslider * );		/* enter in text */
	void (*slider_changed)( Tslider * );	/* slider drag */
	void (*text_changed)( Tslider * );	/* text has been touched */
} TsliderClass;

void tslider_changed( Tslider * );

GtkType tslider_get_type( void );
Tslider *tslider_new( void );

void tslider_set_conversions( Tslider *tslider, 
	tslider_fn value_to_slider, tslider_fn slider_to_value );
double tslider_log_value_to_slider( double from, double to, double value );
double tslider_log_slider_to_value( double from, double to, double value );
