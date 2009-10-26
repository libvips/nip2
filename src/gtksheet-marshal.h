
#ifndef __gtksheet_MARSHAL_H__
#define __gtksheet_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* BOOL:INT,INT,POINTER,POINTER (gtksheet-marshal.list:1) */
extern void gtksheet_BOOLEAN__INT_INT_POINTER_POINTER (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);
#define gtksheet_BOOL__INT_INT_POINTER_POINTER	gtksheet_BOOLEAN__INT_INT_POINTER_POINTER

/* BOOL:BOXED,POINTER (gtksheet-marshal.list:2) */
extern void gtksheet_BOOLEAN__BOXED_POINTER (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);
#define gtksheet_BOOL__BOXED_POINTER	gtksheet_BOOLEAN__BOXED_POINTER

/* BOOL:BOXED,STRING (gtksheet-marshal.list:3) */
extern void gtksheet_BOOLEAN__BOXED_STRING (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);
#define gtksheet_BOOL__BOXED_STRING	gtksheet_BOOLEAN__BOXED_STRING

/* BOOL:BOXED,BOXED (gtksheet-marshal.list:4) */
extern void gtksheet_BOOLEAN__BOXED_BOXED (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);
#define gtksheet_BOOL__BOXED_BOXED	gtksheet_BOOLEAN__BOXED_BOXED

/* BOOL:BOXED,DOUBLE,DOUBLE (gtksheet-marshal.list:5) */
extern void gtksheet_BOOLEAN__BOXED_DOUBLE_DOUBLE (GClosure     *closure,
                                                   GValue       *return_value,
                                                   guint         n_param_values,
                                                   const GValue *param_values,
                                                   gpointer      invocation_hint,
                                                   gpointer      marshal_data);
#define gtksheet_BOOL__BOXED_DOUBLE_DOUBLE	gtksheet_BOOLEAN__BOXED_DOUBLE_DOUBLE

/* BOOL:POINTER,POINTER (gtksheet-marshal.list:6) */
extern void gtksheet_BOOLEAN__POINTER_POINTER (GClosure     *closure,
                                               GValue       *return_value,
                                               guint         n_param_values,
                                               const GValue *param_values,
                                               gpointer      invocation_hint,
                                               gpointer      marshal_data);
#define gtksheet_BOOL__POINTER_POINTER	gtksheet_BOOLEAN__POINTER_POINTER

/* BOOL:POINTER,BOXED (gtksheet-marshal.list:7) */
extern void gtksheet_BOOLEAN__POINTER_BOXED (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);
#define gtksheet_BOOL__POINTER_BOXED	gtksheet_BOOLEAN__POINTER_BOXED

/* BOOL:POINTER,STRING (gtksheet-marshal.list:8) */
extern void gtksheet_BOOLEAN__POINTER_STRING (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);
#define gtksheet_BOOL__POINTER_STRING	gtksheet_BOOLEAN__POINTER_STRING

/* BOOL:POINTER (gtksheet-marshal.list:9) */
extern void gtksheet_BOOLEAN__POINTER (GClosure     *closure,
                                       GValue       *return_value,
                                       guint         n_param_values,
                                       const GValue *param_values,
                                       gpointer      invocation_hint,
                                       gpointer      marshal_data);
#define gtksheet_BOOL__POINTER	gtksheet_BOOLEAN__POINTER

/* BOOL:BOXED (gtksheet-marshal.list:10) */
extern void gtksheet_BOOLEAN__BOXED (GClosure     *closure,
                                     GValue       *return_value,
                                     guint         n_param_values,
                                     const GValue *param_values,
                                     gpointer      invocation_hint,
                                     gpointer      marshal_data);
#define gtksheet_BOOL__BOXED	gtksheet_BOOLEAN__BOXED

/* BOOL:INT,INT (gtksheet-marshal.list:11) */
extern void gtksheet_BOOLEAN__INT_INT (GClosure     *closure,
                                       GValue       *return_value,
                                       guint         n_param_values,
                                       const GValue *param_values,
                                       gpointer      invocation_hint,
                                       gpointer      marshal_data);
#define gtksheet_BOOL__INT_INT	gtksheet_BOOLEAN__INT_INT

/* VOID:INT (gtksheet-marshal.list:12) */
#define gtksheet_VOID__INT	g_cclosure_marshal_VOID__INT

/* VOID:INT,STRING (gtksheet-marshal.list:13) */
extern void gtksheet_VOID__INT_STRING (GClosure     *closure,
                                       GValue       *return_value,
                                       guint         n_param_values,
                                       const GValue *param_values,
                                       gpointer      invocation_hint,
                                       gpointer      marshal_data);

/* VOID:BOXED (gtksheet-marshal.list:14) */
#define gtksheet_VOID__BOXED	g_cclosure_marshal_VOID__BOXED

/* VOID:VOID (gtksheet-marshal.list:15) */
#define gtksheet_VOID__VOID	g_cclosure_marshal_VOID__VOID

/* VOID:BOOL (gtksheet-marshal.list:16) */
#define gtksheet_VOID__BOOLEAN	g_cclosure_marshal_VOID__BOOLEAN
#define gtksheet_VOID__BOOL	gtksheet_VOID__BOOLEAN

/* VOID:POINTER (gtksheet-marshal.list:17) */
#define gtksheet_VOID__POINTER	g_cclosure_marshal_VOID__POINTER

/* VOID:INT,INT (gtksheet-marshal.list:18) */
extern void gtksheet_VOID__INT_INT (GClosure     *closure,
                                    GValue       *return_value,
                                    guint         n_param_values,
                                    const GValue *param_values,
                                    gpointer      invocation_hint,
                                    gpointer      marshal_data);

/* VOID:INT,POINTER (gtksheet-marshal.list:19) */
extern void gtksheet_VOID__INT_POINTER (GClosure     *closure,
                                        GValue       *return_value,
                                        guint         n_param_values,
                                        const GValue *param_values,
                                        gpointer      invocation_hint,
                                        gpointer      marshal_data);

/* VOID:POINTER,POINTER (gtksheet-marshal.list:20) */
extern void gtksheet_VOID__POINTER_POINTER (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

/* VOID:BOXED,POINTER (gtksheet-marshal.list:21) */
extern void gtksheet_VOID__BOXED_POINTER (GClosure     *closure,
                                          GValue       *return_value,
                                          guint         n_param_values,
                                          const GValue *param_values,
                                          gpointer      invocation_hint,
                                          gpointer      marshal_data);

/* VOID:BOXED,BOXED (gtksheet-marshal.list:22) */
extern void gtksheet_VOID__BOXED_BOXED (GClosure     *closure,
                                        GValue       *return_value,
                                        guint         n_param_values,
                                        const GValue *param_values,
                                        gpointer      invocation_hint,
                                        gpointer      marshal_data);

/* VOID:OBJECT,OBJECT (gtksheet-marshal.list:23) */
extern void gtksheet_VOID__OBJECT_OBJECT (GClosure     *closure,
                                          GValue       *return_value,
                                          guint         n_param_values,
                                          const GValue *param_values,
                                          gpointer      invocation_hint,
                                          gpointer      marshal_data);

/* VOID:DOUBLE,DOUBLE,DOUBLE,DOUBLE (gtksheet-marshal.list:24) */
extern void gtksheet_VOID__DOUBLE_DOUBLE_DOUBLE_DOUBLE (GClosure     *closure,
                                                        GValue       *return_value,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint,
                                                        gpointer      marshal_data);

G_END_DECLS

#endif /* __gtksheet_MARSHAL_H__ */

