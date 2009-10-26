/* gtksheet
 * Copyright 1999-2001 Adrian E. Feiguin <feiguin@ifir.edu.ar>
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

#include <string.h>
#include <gtk/gtk.h>
#include "gtksheetfeatures.h"
#include <gobject/gvaluecollector.h>

const guint gtksheet_major_version = GTKSHEET_MAJOR_VERSION;
const guint gtksheet_minor_version = GTKSHEET_MINOR_VERSION;
const guint gtksheet_micro_version = GTKSHEET_MICRO_VERSION;
const guint gtksheet_binary_age = GTKSHEET_BINARY_AGE;
const guint gtksheet_interface_age = GTKSHEET_INTERFACE_AGE;

const gchar * 
gtksheet_check_version (guint required_major,
                        guint required_minor,
                        guint required_micro)
{
  if (required_major > GTKSHEET_MAJOR_VERSION)
    return "GtkSheet version too old (major mismatch)";
  if (required_major < GTKSHEET_MAJOR_VERSION)
    return "GtkSheet version too new (major mismatch)";
  if (required_minor > GTKSHEET_MINOR_VERSION)
    return "GtkSheet version too old (minor mismatch)";
  if (required_minor < GTKSHEET_MINOR_VERSION)
    return "GtkSheet version too new (minor mismatch)";
  if (required_micro < GTKSHEET_MICRO_VERSION - GTKSHEET_BINARY_AGE)
    return "GtkSheet version too new (micro mismatch)";
  if (required_micro > GTKSHEET_MICRO_VERSION)
    return "GtkSheet version too old (micro mismatch)";
  return NULL;
}

void
_gtksheet_signal_emit(GtkObject *object, guint signal_id, ...)
{
  gboolean *result;
  GValue ret = { 0, };
  GValue instance_and_params [10] = { {0, }, };
  va_list var_args;
  GSignalQuery query;
  gchar *error;
  int i;

  va_start (var_args, signal_id);

  g_value_init(instance_and_params + 0, GTK_OBJECT_TYPE(object));
  g_value_set_instance (instance_and_params + 0, G_OBJECT(object));

  g_signal_query(signal_id, &query);

  for (i = 0; i < query.n_params; i++)
    {
      gboolean static_scope = query.param_types[i]&~G_SIGNAL_TYPE_STATIC_SCOPE;
      g_value_init(instance_and_params + i + 1, query.param_types[i]);


      G_VALUE_COLLECT (instance_and_params + i + 1,
                       var_args,
                       static_scope ? G_VALUE_NOCOPY_CONTENTS : 0,
                       &error);

      if (error)
        {
          g_warning ("%s: %s", G_STRLOC, error);
          g_free (error);
          while (i-- > 0)
            g_value_unset (instance_and_params + i);

          va_end (var_args);
          return;
        }
  

    }

  g_value_init(&ret, query.return_type);
  result = va_arg(var_args,gboolean *);
  g_value_set_boolean(&ret, *result);    
  g_signal_emitv(instance_and_params, signal_id, 0, &ret);
  *result = g_value_get_boolean(&ret);    
  g_value_unset (&ret);

  for (i = 0; i < query.n_params; i++)
    g_value_unset (instance_and_params + 1 + i);
  g_value_unset (instance_and_params + 0);

  va_end (var_args);
}
