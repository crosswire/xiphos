#ifndef _GUI_SPLASH_H_
#define _GUI_SPLASH_H_

#ifndef USE_GTK_3

#define E_MAKE_TYPE(l, str, t, ci, i, parent)                                                                                                                                                                            \
	GType l##_get_type(void)                                                                                                                                                                                         \
	{                                                                                                                                                                                                                \
		static GType type = 0;                                                                                                                                                                                   \
		if (!type) {                                                                                                                                                                                             \
			static GTypeInfo const object_info = {sizeof(t##Class), (GBaseInitFunc)NULL, (GBaseFinalizeFunc)NULL, (GClassInitFunc)ci, (GClassFinalizeFunc)NULL, NULL, sizeof(t), 0, (GInstanceInitFunc)i, }; \
			type = g_type_register_static(parent, str, &object_info, 0);                                                                                                                                     \
		}                                                                                                                                                                                                        \
		return type;                                                                                                                                                                                             \
	}

void gui_splash_init(void);
gboolean gui_splash_done(void);
void gui_splash_step(gchar *text, gdouble progress, gint step);
gboolean expose_event_callback(GtkWidget *widget, GdkEventExpose *event,
			       gpointer data);

#else

gboolean gui_splash_done(void);
void gui_splash_step(gchar *text, gdouble progress, gint step);
void gui_splash_init(void);

#endif /* !USE_GTK_3  */

#endif /* !_GUI_SPLASH_H_ */
