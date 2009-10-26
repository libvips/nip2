/* gtksheet - set of widgets for gtk+
 * Copyright 1999-2001  Adrian E. Feiguin <feiguin@ifir.edu.ar>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef GTK_SHEET_FEATURES_H
#define GTK_SHEET_FEATURES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* GtkExtra version.
 */

#define GTKSHEET_MAJOR_VERSION			(0)
#define GTKSHEET_MINOR_VERSION			(2)
#define GTKSHEET_MICRO_VERSION			(0)
#define GTKSHEET_BINARY_AGE			(0)
#define GTKSHEET_INTERFACE_AGE			(0)
#define GTKSHEET_CHECK_VERSION(major,minor,micro)    \
   (GTKSHEET_MAJOR_VERSION > (major) || \
    (GTKSHEET_MAJOR_VERSION == (major) && GTKSHEET_MINOR_VERSION > (minor)) || \
    (GTKSHEET_MAJOR_VERSION == (major) && GTKSHEET_MINOR_VERSION == (minor) && \
     GTKSHEET_MICRO_VERSION >= (micro)))


extern const guint gtksheet_major_version;
extern const guint gtksheet_minor_version;
extern const guint gtksheet_micro_version;
extern const guint gtksheet_binary_age;
extern const guint gtksheet_interface_age;
const gchar* gtksheet_check_version (guint required_major,
                               guint required_minor,
                               guint required_micro);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* GTK_SHEET_FEATURES_H */
