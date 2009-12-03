#ifndef _GUI_SPLASH_H_
#define _GUI_SPLASH_H_


#define E_MAKE_TYPE(l,str,t,ci,i,parent) \
GtkType l##_get_type(void)\
{\
	static GType type = 0;				\
	if (!type){					\
		static GTypeInfo const object_info = {	\
			sizeof (t##Class),		\
							\
			(GBaseInitFunc) NULL,		\
			(GBaseFinalizeFunc) NULL,	\
							\
			(GClassInitFunc) ci,		\
			(GClassFinalizeFunc) NULL,	\
			NULL,		\
							\
			sizeof (t),			\
			0,		\
			(GInstanceInitFunc) i,		\
		};					\
		type = g_type_register_static (parent, str, &object_info, 0);	\
	}						\
	return type;					\
}

void gui_splash_init(void);
gboolean gui_splash_done(void);
void gui_splash_step(gchar *text, gdouble progress, gint step);

#endif
