/* gtkextra
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
#include "gtkextrafeatures.h"
#include <gobject/gvaluecollector.h>

const guint gtkextra_major_version = GTKEXTRA_MAJOR_VERSION;
const guint gtkextra_minor_version = GTKEXTRA_MINOR_VERSION;
const guint gtkextra_micro_version = GTKEXTRA_MICRO_VERSION;
const guint gtkextra_binary_age = GTKEXTRA_BINARY_AGE;
const guint gtkextra_interface_age = GTKEXTRA_INTERFACE_AGE;

gchar * 
gtkextra_check_version (guint required_major,
                        guint required_minor,
                        guint required_micro)
{
  if (required_major > GTKEXTRA_MAJOR_VERSION)
    return "GtkExtra version too old (major mismatch)";
  if (required_major < GTKEXTRA_MAJOR_VERSION)
    return "GtkExtra version too new (major mismatch)";
  if (required_minor > GTKEXTRA_MINOR_VERSION)
    return "GtkExtra version too old (minor mismatch)";
  if (required_minor < GTKEXTRA_MINOR_VERSION)
    return "GtkExtra version too new (minor mismatch)";
  if (required_micro < GTKEXTRA_MICRO_VERSION - GTKEXTRA_BINARY_AGE)
    return "GtkExtra version too new (micro mismatch)";
  if (required_micro > GTKEXTRA_MICRO_VERSION)
    return "GtkExtra version too old (micro mismatch)";
  return NULL;
}

/*
void
_gtkextra_signal_test(GtkObject *object, guint signal_id, gint arg1, gint arg2, gboolean *default_ret)
{
  gboolean result;
  GValue ret = { 0, };
  GValue instance_and_param[3] = { { 0, }, {0, }, {0, } };

  g_value_init(instance_and_param + 0, GTK_OBJECT_TYPE(object));
  g_value_set_instance(instance_and_param + 0, G_OBJECT(object));

  g_value_init(instance_and_param + 1, G_TYPE_INT);
  g_value_set_int(instance_and_param + 1, arg1);

  g_value_init(instance_and_param + 2, G_TYPE_INT);
  g_value_set_int(instance_and_param + 2, arg2);

  g_value_init(&ret, G_TYPE_BOOLEAN);
  g_value_set_boolean(&ret, *default_ret);

  g_signal_emitv(instance_and_param, signal_id, 0, &ret);
  *default_ret = g_value_get_boolean(&ret);

  g_value_unset(instance_and_param + 0);
  g_value_unset(instance_and_param + 1);
  g_value_unset(instance_and_param + 2);
}
*/

void
_gtkextra_signal_emit(GtkObject *object, guint signal_id, ...)
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
