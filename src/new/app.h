#ifndef __APP_H
#define __APP_H

#define APP_TYPE (app_get_type())
#define APP( obj ) \
        (G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_APP, App ))
#define APP_CLASS( klass ) \
        (G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_APP, AppClass))
#define IS_APP( obj ) \
        (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_APP ))
#define IS_APP_CLASS( klass ) \
        (G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_APP ))
#define APP_GET_CLASS( obj ) \
        (G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_APP, AppClass ))

typedef struct _App {
        GtkApplication parent_instance;

} App;

typedef struct _AppClass {
        GtkApplicationClass parent_class;

} AppClass;

GType app_get_type( void );
App *app_new( void );

#endif /* __APP_H */

