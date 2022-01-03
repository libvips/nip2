/* Declarations supporting main.c.
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

extern Workspaceroot *main_workspaceroot;	/* All the workspaces */
extern Toolkitgroup *main_toolkitgroup;		/* All the toolkits */
extern Symbol *main_symbol_root;		/* Root of symtable */
extern Watchgroup *main_watchgroup;		/* All of the watches */
extern Imageinfogroup *main_imageinfogroup;	/* All of the images */

extern void *main_c_stack_base;			/* Base of C stack */

extern gboolean main_starting;			/* In startup */

extern gboolean main_option_time_save;		/* Time save image ops */
extern gboolean main_option_profile;		/* Profile calcualtion */
extern gboolean main_option_i18n;		/* Output i18n strings */
extern gboolean main_option_batch;		/* Running in batch mode */
extern gboolean main_option_verbose;		/* Verbose output */

/* Styles for buttons etc.
 */
extern GtkStyle *default_style;
extern GtkStyle *selected_style;
extern GtkStyle *error_style;
extern GtkStyle *ok_style;
extern GtkStyle *tooltip_style;
extern GtkStyle *leaf_style;
extern GtkStyle *dirty_style;

void main_quit_test( void );
void main_reload( void );
const char *get_prefix( void );

