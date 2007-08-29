/* gtkplotarray - 3d scientific plots widget for gtk+
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

#ifndef __GTK_PLOT_ARRAY_H__
#define __GTK_PLOT_ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GTK_PLOT_ARRAY_LIST(obj)        GTK_CHECK_CAST (obj, gtk_plot_array_list_get_type (), GtkPlotArrayList)
#define GTK_TYPE_PLOT_ARRAY_LIST        (gtk_plot_array_list_get_type ())
#define GTK_PLOT_ARRAY_LIST_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_array_list_get_type(), GtkPlotArrayListClass)
#define GTK_IS_PLOT_ARRAY_LIST(obj)     GTK_CHECK_TYPE (obj, gtk_plot_array_list_get_type ())

#define GTK_PLOT_ARRAY(obj)        GTK_CHECK_CAST (obj, gtk_plot_array_get_type (), GtkPlotArray)
#define GTK_TYPE_PLOT_ARRAY        (gtk_plot_array_get_type ())
#define GTK_PLOT_ARRAY_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_array_get_type(), GtkPlotArrayClass)
#define GTK_IS_PLOT_ARRAY(obj)     GTK_CHECK_TYPE (obj, gtk_plot_array_get_type ())

typedef struct _GtkPlotArrayList          GtkPlotArrayList;
typedef struct _GtkPlotArrayListClass     GtkPlotArrayListClass;
typedef struct _GtkPlotArray             GtkPlotArray;
typedef struct _GtkPlotArrayClass        GtkPlotArrayClass;

typedef union
{
  gdouble *data_double;
  gfloat *data_float;
  gint *data_int;
  gboolean *data_bool;
  gchar **data_string;
  gpointer *data_pointer;
} GtkPlotArrayArg;

struct _GtkPlotArrayList
{
  GObject object;

  GList *arrays;
};

struct _GtkPlotArrayListClass
{
  GObjectClass parent_class;
};


struct _GtkPlotArray
{
  GObject object;

  gchar *name;
  gchar *label;
  gchar *description;

  GtkPlotArrayArg data;
  GtkType type;
  gboolean own_data;
  gint size;

  gboolean required;
  gboolean independent;

  gdouble scale;
};

struct _GtkPlotArrayClass
{
  GObjectClass parent_class;
};


GType		gtk_plot_array_get_type		(void);
GObject*	gtk_plot_array_new		(const gchar *name, 
						 gpointer array,
						 gint size,
						 GtkType type,
						 gboolean own_data);
void		gtk_plot_array_set		(GtkPlotArray *array,
						 gpointer data_array,
						 gint size,
						 GtkType type); 
void		gtk_plot_array_set_label	(GtkPlotArray *array,
						 const gchar *label);
void		gtk_plot_array_set_description	(GtkPlotArray *array,
						 const gchar *desc);
void	 	gtk_plot_array_set_scale	(GtkPlotArray *array,
						 gdouble scale);
void	 	gtk_plot_array_set_required	(GtkPlotArray *array,
						 gboolean required);
void	 	gtk_plot_array_set_independent	(GtkPlotArray *array,
						 gboolean independent);
GtkType		gtk_plot_array_get_data_type	(GtkPlotArray *array);
const gchar *	gtk_plot_array_get_name		(GtkPlotArray *array);
const gchar *	gtk_plot_array_get_label	(GtkPlotArray *array);
const gchar *	gtk_plot_array_get_description	(GtkPlotArray *array);
gboolean	gtk_plot_array_required		(GtkPlotArray *array);
gboolean	gtk_plot_array_independent	(GtkPlotArray *array);
gdouble 	gtk_plot_array_get_scale	(GtkPlotArray *array);
gint		gtk_plot_array_get_size		(GtkPlotArray *array);
gfloat*		gtk_plot_array_get_float	(GtkPlotArray *array);
gdouble*	gtk_plot_array_get_double	(GtkPlotArray *array);
gint*		gtk_plot_array_get_int		(GtkPlotArray *array);
gboolean*	gtk_plot_array_get_bool		(GtkPlotArray *array);
gchar**		gtk_plot_array_get_string	(GtkPlotArray *array);
gpointer*	gtk_plot_array_get_pointer	(GtkPlotArray *array);
void		gtk_plot_array_free		(GtkPlotArray *array);
						
GType		gtk_plot_array_list_get_type	(void);
GObject*	gtk_plot_array_list_new		(void);
void		gtk_plot_array_list_add		(GtkPlotArrayList *set,
						 GtkPlotArray *array);
void		gtk_plot_array_list_remove	(GtkPlotArrayList *set,
						 GtkPlotArray *array);
GtkPlotArray*	gtk_plot_array_list_get		(GtkPlotArrayList *set,
						 const gchar *name);
void		gtk_plot_array_list_clear	(GtkPlotArrayList *set);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_ARRAY_H__ */
