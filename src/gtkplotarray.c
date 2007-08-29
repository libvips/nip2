/* gtkplotarray - array plots widget for gtk+
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gtk/gtk.h>
#include "gtkplot.h"
#include "gtkplot3d.h"
#include "gtkplotdata.h"
#include "gtkplotarray.h"
#include "gtkpsfont.h"

#define P_(string) string

static void gtk_plot_array_class_init 	(GtkPlotArrayClass *klass);
static void gtk_plot_array_init 	(GtkPlotArray *array);
static void gtk_plot_array_finalize 	(GObject *object);
static void gtk_plot_array_set_property (GObject *object,
                                         guint            prop_id,
                                         const GValue          *value,
                                         GParamSpec      *pspec);
static void gtk_plot_array_get_property (GObject *object,
                                         guint            prop_id,
                                         GValue    *value,
                                         GParamSpec      *pspec);
static void gtk_plot_array_list_class_init (GtkPlotArrayListClass *klass);
static void gtk_plot_array_list_init 	(GtkPlotArrayList *array_list);
static void gtk_plot_array_list_finalize (GObject *object);

static GObjectClass *array_parent_class = NULL;
static GObjectClass *array_list_parent_class = NULL;

enum {
  PROP_0,
  PROP_NAME,
  PROP_LABEL,
  PROP_DESCRIPTION,
  PROP_TYPE,
  PROP_SIZE,
  PROP_DATA,
  PROP_SCALE,
  PROP_REQUIRED,
  PROP_INDEPENDENT,
  PROP_OWN,
};

GType
gtk_plot_array_get_type (void)
{
  static GType array_type = 0;

  if (!array_type)
    {
      static const GTypeInfo array_info =
      {
        sizeof (GtkPlotArrayClass),
        NULL,           /* base_init */
        NULL,           /* base_finalize */
        (GClassInitFunc) gtk_plot_array_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (GtkPlotArray),
        0,              /* n_preallocs */
        (GInstanceInitFunc) gtk_plot_array_init,
        NULL,
      };
                                                                                
      array_type = g_type_register_static (G_TYPE_OBJECT, "GtkPlotArray",
                                               &array_info, 0);
    }
                                                                                
  return array_type;
}

static void
gtk_plot_array_class_init (GtkPlotArrayClass *klass)
{
  GtkPlotArrayClass *array_class;
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  array_parent_class = g_type_class_peek_parent (klass);

  array_class = (GtkPlotArrayClass *) klass;

  gobject_class->finalize = gtk_plot_array_finalize;
  gobject_class->set_property = gtk_plot_array_set_property;
  gobject_class->get_property = gtk_plot_array_get_property;

  g_object_class_install_property (gobject_class,
                           PROP_NAME,
  g_param_spec_string ("name",
                           P_("Name"),
                           P_("Name"),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_LABEL,
  g_param_spec_string ("label",
                           P_("Label"),
                           P_("Label"),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_DESCRIPTION,
  g_param_spec_string ("description",
                           P_("Description"),
                           P_("Description"),
                           NULL,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_TYPE,
  g_param_spec_int ("type",
                           P_("Type"),
                           P_("Type"),
                           0,G_MAXINT, 0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_SIZE,
  g_param_spec_int ("size",
                           P_("Size"),
                           P_("Size"),
                           0, G_MAXINT, 0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_OWN,
  g_param_spec_boolean ("own_data",
                           P_("Own Data"),
                           P_("Specifies if data should be freed when the array is destroyed"),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_REQUIRED,
  g_param_spec_boolean ("required",
                           P_("Required"),
                           P_("Required"),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_INDEPENDENT,
  g_param_spec_boolean ("independent",
                           P_("Independent"),
                           P_("Independent"),
                           FALSE,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_SCALE,
  g_param_spec_double ("scale",
                           P_("Scale"),
                           P_("Scale"),
                           0, G_MAXDOUBLE, 1.0,
                           G_PARAM_READABLE|G_PARAM_WRITABLE));
  g_object_class_install_property (gobject_class,
                           PROP_DATA,
  g_param_spec_pointer ("array_data",
                           P_("Data"),
                           P_("Data"),
                           G_PARAM_READABLE|G_PARAM_WRITABLE));

}

static void
gtk_plot_array_set_property (GObject      *object,
                             guint            prop_id,
                             const GValue          *value,
                             GParamSpec      *pspec)
{
  GtkPlotArray *data;
                                                                                
  data = GTK_PLOT_ARRAY (object);
                                                                                
  switch (prop_id)
    {
      case PROP_NAME:
        if(data->name) g_free(data->name);
        data->name = g_strdup(g_value_get_string(value));
        break;
      case PROP_LABEL:
        if(data->label) g_free(data->label);
        data->label = g_strdup(g_value_get_string(value));
        break;
      case PROP_DESCRIPTION:
        if(data->description) g_free(data->description);
        data->description = g_strdup(g_value_get_string(value));
        break;
      case PROP_TYPE:
        data->type  = g_value_get_int(value);
        break;
      case PROP_SIZE:
        data->size  = g_value_get_int(value);
        break;
      case PROP_OWN:
        data->own_data  = g_value_get_boolean(value);
        break;
      case PROP_REQUIRED:
        data->required  = g_value_get_boolean(value);
        break;
      case PROP_INDEPENDENT:
        data->independent  = g_value_get_boolean(value);
        break;
      case PROP_SCALE:
        data->scale  = g_value_get_double(value);
        break;
      case PROP_DATA:
        data->data = *((GtkPlotArrayArg *)g_value_get_pointer(value));
        break;
    }
}

static void
gtk_plot_array_get_property (GObject      *object,
                             guint            prop_id,
                             GValue          *value,
                             GParamSpec      *pspec)
{
  GtkPlotArray *data;

  data = GTK_PLOT_ARRAY (object);

  switch (prop_id)
    {
      case PROP_NAME:
        g_value_set_string(value, data->name);
        break;
      case PROP_LABEL:
        g_value_set_string(value, data->label);
        break;
      case PROP_DESCRIPTION:
        g_value_set_string(value, data->description);
        break;
      case PROP_TYPE:
        g_value_set_int(value, data->type);
        break;
      case PROP_SIZE:
        g_value_set_int(value, data->size);
        break;
      case PROP_OWN:
        g_value_set_boolean(value, data->own_data);
        break;
      case PROP_REQUIRED:
        g_value_set_boolean(value, data->required);
        break;
      case PROP_INDEPENDENT:
        g_value_set_boolean(value, data->independent);
        break;
      case PROP_SCALE:
        g_value_set_double(value, data->scale);
        break;
      case PROP_DATA:
        g_value_set_pointer(value, &data->data);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
gtk_plot_array_init (GtkPlotArray *array)
{
  array->name = NULL;
  array->label = NULL;
  array->description = NULL;
  array->type = GTK_TYPE_DOUBLE;
  array->own_data = FALSE;
  array->required = FALSE;
  array->independent = FALSE;
  array->size = 0;
  array->scale = 1.;
}

GObject*
gtk_plot_array_new (const gchar *name, gpointer data, gint size, GtkType type, gboolean own_data)
{
  GObject *object;

  object = g_object_new (gtk_plot_array_get_type (), NULL);
  GTK_PLOT_ARRAY(object)->type = type;
  GTK_PLOT_ARRAY(object)->own_data = own_data;
  GTK_PLOT_ARRAY(object)->name = g_strdup(name);
  GTK_PLOT_ARRAY(object)->label = g_strdup(name);
  GTK_PLOT_ARRAY(object)->description = NULL;
  GTK_PLOT_ARRAY(object)->size = size;
  gtk_plot_array_set(GTK_PLOT_ARRAY(object), data, size, type);

  return (object);
}

static void
gtk_plot_array_finalize(GObject *object)
{
  GtkPlotArray *array = GTK_PLOT_ARRAY(object);
  if(array->name) g_free(array->name);
  array->name = NULL;
  if(array->label) g_free(array->label);
  array->label = NULL;
  if(array->description) g_free(array->description);
  array->description = NULL;
  if(array->own_data) gtk_plot_array_free(array);
  array->size = 0;
}

void
gtk_plot_array_set_label(GtkPlotArray *array, const gchar *label)
{
  if(array->label) g_free(array->label);
  array->label = NULL;
  if(label) array->label = g_strdup(label);
}

void
gtk_plot_array_set_description(GtkPlotArray *array, const gchar *description)
{
  if(array->description) g_free(array->description);
  array->description = NULL;
  if(description) array->description = g_strdup(description);
}

void
gtk_plot_array_set_scale(GtkPlotArray *array, gdouble scale)
{
  array->scale = scale;
}

void
gtk_plot_array_set_independent(GtkPlotArray *array, gboolean independent)
{
  array->independent = independent;
}

void
gtk_plot_array_set_required(GtkPlotArray *array, gboolean required)
{
  array->required = required;
}

void
gtk_plot_array_set(GtkPlotArray *array, gpointer data, gint size, GtkType type)
{
  if(array->own_data) gtk_plot_array_free(array);
  array->type = type;
  array->size = size;
  switch(type){
    case GTK_TYPE_DOUBLE:
      array->data.data_double = (gdouble *)data;
      break;
    case GTK_TYPE_FLOAT:
      array->data.data_float = (gfloat *)data;
      break;
    case GTK_TYPE_INT:
      array->data.data_int = (gint *)data;
      break;
    case GTK_TYPE_BOOL:
      array->data.data_bool = (gboolean *)data;
      break;
    case GTK_TYPE_STRING:
      array->data.data_string = (gchar **)data;
      break;
    default:
      break; 
  }
}

void
gtk_plot_array_free(GtkPlotArray *array)
{
  gint i = 0;

  switch(array->type){
    case GTK_TYPE_DOUBLE:
      g_free(array->data.data_double);
      array->data.data_double = NULL;
      break;
    case GTK_TYPE_FLOAT:
      g_free(array->data.data_float);
      array->data.data_float = NULL;
      break;
    case GTK_TYPE_INT:
      g_free(array->data.data_int);
      array->data.data_int = NULL;
      break;
    case GTK_TYPE_BOOL:
      g_free(array->data.data_bool);
      array->data.data_bool = NULL;
      break;
    case GTK_TYPE_STRING:
      for(i = 0; i < array->size; i++)
        if(array->data.data_string && array->data.data_string[i]) 
          g_free(array->data.data_string[i]);
      g_free(array->data.data_string);
      array->data.data_string = NULL;
      break;
    default:
      break; 
  }
}

gint
gtk_plot_array_get_size(GtkPlotArray *array)
{
  return array->size;
}

gdouble
gtk_plot_array_get_scale(GtkPlotArray *array)
{
  return array->scale;
}

gboolean
gtk_plot_array_required(GtkPlotArray *array)
{
  return array->required;
}

gboolean
gtk_plot_array_independent(GtkPlotArray *array)
{
  return array->independent;
}

GtkType
gtk_plot_array_get_data_type(GtkPlotArray *array)
{
  return array->type;
}

const gchar *
gtk_plot_array_get_name(GtkPlotArray *array)
{
  return array->name;
}

const gchar *
gtk_plot_array_get_label(GtkPlotArray *array)
{
  return array->label;
}

const gchar *
gtk_plot_array_get_description(GtkPlotArray *array)
{
  return array->description;
}

gboolean *
gtk_plot_array_get_bool(GtkPlotArray *array)
{
  if(array->type != GTK_TYPE_BOOL) return NULL;
  return array->data.data_bool;
}

gdouble *
gtk_plot_array_get_double(GtkPlotArray *array)
{
  if(array->type != GTK_TYPE_DOUBLE) return NULL;
  return array->data.data_double;
}

gfloat *
gtk_plot_array_get_float(GtkPlotArray *array)
{
  if(array->type != GTK_TYPE_FLOAT) return NULL;
  return array->data.data_float;
}

gint *
gtk_plot_array_get_int(GtkPlotArray *array)
{
  if(array->type != GTK_TYPE_INT) return NULL;
  return array->data.data_int;
}

gchar **
gtk_plot_array_get_string(GtkPlotArray *array)
{
  if(array->type != GTK_TYPE_STRING) return NULL;
  return array->data.data_string;
}

gpointer *
gtk_plot_array_get_pointer(GtkPlotArray *array)
{
  if(array->type != GTK_TYPE_POINTER) return NULL;
  return array->data.data_pointer;
}

GType
gtk_plot_array_list_get_type (void)
{
  static GType array_list_type = 0;

  if (!array_list_type)
    {
      static const GTypeInfo data_info =
      {
        sizeof (GtkPlotArrayListClass),
        NULL,           /* base_init */
        NULL,           /* base_finalize */
        (GClassInitFunc) gtk_plot_array_list_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (GtkPlotArray),
        0,              /* n_preallocs */
        (GInstanceInitFunc) gtk_plot_array_list_init,
        NULL
      };
                                                                                
      array_list_type = g_type_register_static (G_TYPE_OBJECT, "GtkPlotArrayList",
                                               &data_info, 0);
    }
  return array_list_type;
}

static void
gtk_plot_array_list_class_init (GtkPlotArrayListClass *klass)
{
  GObjectClass *gobject_class;
  GtkPlotArrayListClass *array_list_class;

  array_list_parent_class = g_type_class_peek_parent (klass);

  gobject_class = (GObjectClass *) klass;
  array_list_class = (GtkPlotArrayListClass *) klass;

  gobject_class->finalize = gtk_plot_array_list_finalize;
}


static void
gtk_plot_array_list_init (GtkPlotArrayList *array_list)
{
  array_list->arrays = NULL;
}

GObject*
gtk_plot_array_list_new (void)
{
  GObject *object;

  object = g_object_new (gtk_plot_array_list_get_type (), NULL);

  return (object);
}

static void
gtk_plot_array_list_finalize(GObject *object)
{
  GtkPlotArrayList *array_list = GTK_PLOT_ARRAY_LIST(object);

  gtk_plot_array_list_clear(array_list);
}

GList *
find_array(GtkPlotArrayList *array_list, const gchar *name)
{
  GList *list;

  if(!array_list->arrays) return NULL;

  list = array_list->arrays;
  while(list){
    if(list->data && GTK_IS_PLOT_ARRAY(list->data))
      if(GTK_PLOT_ARRAY(list->data)->name && strcmp(GTK_PLOT_ARRAY(list->data)->name, name) == 0) return list;
    list = list->next;
  }
  return NULL;
}

void
gtk_plot_array_list_add(GtkPlotArrayList *array_list, GtkPlotArray *array)
{
  GList *list = NULL;
  list = find_array(array_list, array->name);
  if(list){
    g_object_unref(G_OBJECT(list->data));
    list->data = array;
    g_object_ref(G_OBJECT(array));
  } else {
    array_list->arrays = g_list_append(array_list->arrays, array);
    g_object_ref(G_OBJECT(array));
  }
}

void
gtk_plot_array_list_remove(GtkPlotArrayList *array_list, GtkPlotArray *array)
{
  GList *list;
  list = g_list_find(array_list->arrays, array);
  if(list){
    g_object_unref(G_OBJECT(array));
    array_list->arrays = g_list_remove_link(array_list->arrays, list);
    g_list_free_1(list); 
  }
}

GtkPlotArray *
gtk_plot_array_list_get(GtkPlotArrayList *array_list, const gchar *name)
{
  GList *list = NULL;;
  list = find_array(array_list, name);
  if(list) return GTK_PLOT_ARRAY(list->data);
  return NULL;
}

void
gtk_plot_array_list_clear(GtkPlotArrayList *array_list)
{
  GList *list;

  list = array_list->arrays;
  while(list){
    if(list->data && G_IS_OBJECT(list->data)) 
      g_object_unref(G_OBJECT(list->data));
    list->data = NULL;
    array_list->arrays = g_list_remove_link(array_list->arrays, list);
    g_list_free_1(list);

    list = array_list->arrays;
  }
  array_list->arrays = NULL;
}
