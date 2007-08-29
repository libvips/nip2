
#ifndef __gtkextra_MARSHAL_H__
#define __gtkextra_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* BOOL:INT,INT,POINTER,POINTER (gtkextra-marshal.list:1) */
extern void gtkextra_BOOLEAN__INT_INT_POINTER_POINTER (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);
#define gtkextra_BOOL__INT_INT_POINTER_POINTER	gtkextra_BOOLEAN__INT_INT_POINTER_POINTER

/* BOOL:BOXED,POINTER (gtkextra-marshal.list:2) */
extern void gtkextra_BOOLEAN__BOXED_POINTER (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);
#define gtkextra_BOOL__BOXED_POINTER	gtkextra_BOOLEAN__BOXED_POINTER

/* BOOL:BOXED,STRING (gtkextra-marshal.list:3) */
extern void gtkextra_BOOLEAN__BOXED_STRING (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);
#define gtkextra_BOOL__BOXED_STRING	gtkextra_BOOLEAN__BOXED_STRING

/* BOOL:BOXED,BOXED (gtkextra-marshal.list:4) */
extern void gtkextra_BOOLEAN__BOXED_BOXED (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);
#define gtkextra_BOOL__BOXED_BOXED	gtkextra_BOOLEAN__BOXED_BOXED

/* BOOL:BOXED,DOUBLE,DOUBLE (gtkextra-marshal.list:5) */
extern void gtkextra_BOOLEAN__BOXED_DOUBLE_DOUBLE (GClosure     *closure,
                                                   GValue       *return_value,
                                                   guint         n_param_values,
                                                   const GValue *param_values,
                                                   gpointer      invocation_hint,
                                                   gpointer      marshal_data);
#define gtkextra_BOOL__BOXED_DOUBLE_DOUBLE	gtkextra_BOOLEAN__BOXED_DOUBLE_DOUBLE

/* BOOL:POINTER,POINTER (gtkextra-marshal.list:6) */
extern void gtkextra_BOOLEAN__POINTER_POINTER (GClosure     *closure,
                                               GValue       *return_value,
                                               guint         n_param_values,
                                               const GValue *param_values,
                                               gpointer      invocation_hint,
                                               gpointer      marshal_data);
#define gtkextra_BOOL__POINTER_POINTER	gtkextra_BOOLEAN__POINTER_POINTER

/* BOOL:POINTER,BOXED (gtkextra-marshal.list:7) */
extern void gtkextra_BOOLEAN__POINTER_BOXED (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);
#define gtkextra_BOOL__POINTER_BOXED	gtkextra_BOOLEAN__POINTER_BOXED

/* BOOL:POINTER,STRING (gtkextra-marshal.list:8) */
extern void gtkextra_BOOLEAN__POINTER_STRING (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);
#define gtkextra_BOOL__POINTER_STRING	gtkextra_BOOLEAN__POINTER_STRING

/* BOOL:POINTER (gtkextra-marshal.list:9) */
extern void gtkextra_BOOLEAN__POINTER (GClosure     *closure,
                                       GValue       *return_value,
                                       guint         n_param_values,
                                       const GValue *param_values,
                                       gpointer      invocation_hint,
                                       gpointer      marshal_data);
#define gtkextra_BOOL__POINTER	gtkextra_BOOLEAN__POINTER

/* BOOL:BOXED (gtkextra-marshal.list:10) */
extern void gtkextra_BOOLEAN__BOXED (GClosure     *closure,
                                     GValue       *return_value,
                                     guint         n_param_values,
                                     const GValue *param_values,
                                     gpointer      invocation_hint,
                                     gpointer      marshal_data);
#define gtkextra_BOOL__BOXED	gtkextra_BOOLEAN__BOXED

/* BOOL:INT,INT (gtkextra-marshal.list:11) */
extern void gtkextra_BOOLEAN__INT_INT (GClosure     *closure,
                                       GValue       *return_value,
                                       guint         n_param_values,
                                       const GValue *param_values,
                                       gpointer      invocation_hint,
                                       gpointer      marshal_data);
#define gtkextra_BOOL__INT_INT	gtkextra_BOOLEAN__INT_INT

/* VOID:INT (gtkextra-marshal.list:12) */
#define gtkextra_VOID__INT	g_cclosure_marshal_VOID__INT

/* VOID:INT,STRING (gtkextra-marshal.list:13) */
extern void gtkextra_VOID__INT_STRING (GClosure     *closure,
                                       GValue       *return_value,
                                       guint         n_param_values,
                                       const GValue *param_values,
                                       gpointer      invocation_hint,
                                       gpointer      marshal_data);

/* VOID:BOXED (gtkextra-marshal.list:14) */
#define gtkextra_VOID__BOXED	g_cclosure_marshal_VOID__BOXED

/* VOID:VOID (gtkextra-marshal.list:15) */
#define gtkextra_VOID__VOID	g_cclosure_marshal_VOID__VOID

/* VOID:BOOL (gtkextra-marshal.list:16) */
#define gtkextra_VOID__BOOLEAN	g_cclosure_marshal_VOID__BOOLEAN
#define gtkextra_VOID__BOOL	gtkextra_VOID__BOOLEAN

/* VOID:POINTER (gtkextra-marshal.list:17) */
#define gtkextra_VOID__POINTER	g_cclosure_marshal_VOID__POINTER

/* VOID:INT,INT (gtkextra-marshal.list:18) */
extern void gtkextra_VOID__INT_INT (GClosure     *closure,
                                    GValue       *return_value,
                                    guint         n_param_values,
                                    const GValue *param_values,
                                    gpointer      invocation_hint,
                                    gpointer      marshal_data);

/* VOID:INT,POINTER (gtkextra-marshal.list:19) */
extern void gtkextra_VOID__INT_POINTER (GClosure     *closure,
                                        GValue       *return_value,
                                        guint         n_param_values,
                                        const GValue *param_values,
                                        gpointer      invocation_hint,
                                        gpointer      marshal_data);

/* VOID:INT,BOXED (gtkextra-marshal.list:20) */
extern void gtkextra_VOID__INT_BOXED (GClosure     *closure,
                                      GValue       *return_value,
                                      guint         n_param_values,
                                      const GValue *param_values,
                                      gpointer      invocation_hint,
                                      gpointer      marshal_data);

/* VOID:POINTER,POINTER (gtkextra-marshal.list:21) */
extern void gtkextra_VOID__POINTER_POINTER (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

/* VOID:BOXED,POINTER (gtkextra-marshal.list:22) */
extern void gtkextra_VOID__BOXED_POINTER (GClosure     *closure,
                                          GValue       *return_value,
                                          guint         n_param_values,
                                          const GValue *param_values,
                                          gpointer      invocation_hint,
                                          gpointer      marshal_data);

/* VOID:BOXED,BOXED (gtkextra-marshal.list:23) */
extern void gtkextra_VOID__BOXED_BOXED (GClosure     *closure,
                                        GValue       *return_value,
                                        guint         n_param_values,
                                        const GValue *param_values,
                                        gpointer      invocation_hint,
                                        gpointer      marshal_data);

/* VOID:OBJECT,OBJECT (gtkextra-marshal.list:24) */
extern void gtkextra_VOID__OBJECT_OBJECT (GClosure     *closure,
                                          GValue       *return_value,
                                          guint         n_param_values,
                                          const GValue *param_values,
                                          gpointer      invocation_hint,
                                          gpointer      marshal_data);

/* VOID:DOUBLE,DOUBLE,DOUBLE,DOUBLE (gtkextra-marshal.list:25) */
extern void gtkextra_VOID__DOUBLE_DOUBLE_DOUBLE_DOUBLE (GClosure     *closure,
                                                        GValue       *return_value,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint,
                                                        gpointer      marshal_data);

G_END_DECLS

#endif /* __gtkextra_MARSHAL_H__ */

