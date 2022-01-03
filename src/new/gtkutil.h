#define WHITESPACE " \t\r\b\n"

void set_gentryv( GtkWidget *edit, const char *fmt, va_list ap );
void set_gentry( GtkWidget *edit, const char *fmt, ... );
gboolean get_geditable_double( GtkWidget *text, double *out );
GtkWidget *build_entry( int nchars );
void set_tooltip( GtkWidget *wid, const char *fmt, ... );
void copy_adj( GtkAdjustment *to, GtkAdjustment *from );

void change_state( GtkWidget *widget, const char *name, GVariant *state );
GVariant *get_state( GtkWidget *widget, const char *name );
void copy_state( GtkWidget *to, GtkWidget *from, const char *name );
