#ifndef __MAIN_H
#define __MAIN_H

#define MAIN_TYPE (main_get_type())
#define MAIN( obj ) \
        (G_TYPE_CHECK_INSTANCE_CAST( (obj), TYPE_MAIN, Main ))
#define MAIN_CLASS( klass ) \
        (G_TYPE_CHECK_CLASS_CAST( (klass), TYPE_MAIN, MainClass))
#define IS_MAIN( obj ) \
        (G_TYPE_CHECK_INSTANCE_TYPE( (obj), TYPE_MAIN ))
#define IS_MAIN_CLASS( klass ) \
        (G_TYPE_CHECK_CLASS_TYPE( (klass), TYPE_MAIN ))
#define MAIN_GET_CLASS( obj ) \
        (G_TYPE_INSTANCE_GET_CLASS( (obj), TYPE_MAIN, MainClass ))

typedef struct _Main {
        GtkApplicationWindow parent_instance;

} Main;

typedef struct _MainClass {
        GtkApplicationWindowClass parent_class;

} MainClass;

GType main_get_type( void );
Main *main_new( App *app );

#endif /* __MAIN_H */

