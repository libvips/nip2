
#ifndef __nip_MARSHAL_H__
#define __nip_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,INT (nipmarshal.list:25) */
extern void nip_VOID__OBJECT_INT (GClosure     *closure,
                                  GValue       *return_value,
                                  guint         n_param_values,
                                  const GValue *param_values,
                                  gpointer      invocation_hint,
                                  gpointer      marshal_data);

/* VOID:DOUBLE,DOUBLE (nipmarshal.list:26) */
extern void nip_VOID__DOUBLE_DOUBLE (GClosure     *closure,
                                     GValue       *return_value,
                                     guint         n_param_values,
                                     const GValue *param_values,
                                     gpointer      invocation_hint,
                                     gpointer      marshal_data);

/* BOOLEAN:INT,INT (nipmarshal.list:27) */
extern void nip_BOOLEAN__INT_INT (GClosure     *closure,
                                  GValue       *return_value,
                                  guint         n_param_values,
                                  const GValue *param_values,
                                  gpointer      invocation_hint,
                                  gpointer      marshal_data);

G_END_DECLS

#endif /* __nip_MARSHAL_H__ */

