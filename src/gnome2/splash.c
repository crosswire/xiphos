/*
 * Xiphos Bible Study Tool
 * splash.c - Splash related functions
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Contains code from Evolution as written by Ettore Perazzoli.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "main/sword.h"
#include "main/settings.h"
#include "gui/splash.h"
#include "gui/utilities.h"

#ifndef USE_GTK_3
/*****************************************************************************/
/* FIXME: clean up below */

#define E_TYPE_SPLASH (e_splash_get_type())
#define E_SPLASH(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), \
						  E_TYPE_SPLASH, ESplash))
#define E_SPLASH_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), \
						       E_TYPE_SPLASH, ESplashClass))
#define E_IS_SPLASH(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), E_TYPE_SPLASH))
#define E_IS_SPLASH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((obj), \
							  E_TYPE_SPLASH))

typedef struct _ESplash ESplash;
typedef struct _ESplashPrivate ESplashPrivate;
typedef struct _ESplashClass ESplashClass;

struct _ESplash
{
	GtkWindow parent;

	ESplashPrivate *priv;
};

struct _ESplashClass
{
	GtkWindowClass parent_class;
};

GType e_splash_get_type(void);
void e_splash_construct(ESplash *splash, GdkPixbuf *splash_image);
GtkWidget *e_splash_new(void);

int e_splash_add_icon(ESplash *splash, GdkPixbuf *icon);
void e_splash_set_icon_highlight(ESplash *splash,
				 int num, gboolean highlight);

#define PARENT_TYPE gtk_window_get_type()
static GtkWindowClass *parent_class = NULL;

struct _Icon
{
	GdkPixbuf *dark_pixbuf;
	GdkPixbuf *light_pixbuf;
	double x;
	double y;
	gboolean light;
};
typedef struct _Icon Icon;

struct _ESplashPrivate
{
	GtkDrawingArea *canvas;
	GdkPixbuf *splash_image_pixbuf;
	GtkProgressBar *progressbar;
	GList *icons; /* (Icon *) */
	int num_icons;

	int layout_idle_id;
};

/* Layout constants.  These need to be changed if the splash changes.  */

#define ICON_Y 340
#define ICON_SIZE 28

/* Icon management.  */

static GdkPixbuf *create_darkened_pixbuf(GdkPixbuf *pixbuf)
{
	GdkPixbuf *new;
	unsigned char *rowp;
	int width, height;
	int rowstride;
	int i, j;

	new = gdk_pixbuf_copy(pixbuf);
	if (!gdk_pixbuf_get_has_alpha(new))
		return new;

	width = gdk_pixbuf_get_width(new);
	height = gdk_pixbuf_get_height(new);
	rowstride = gdk_pixbuf_get_rowstride(new);

	rowp = gdk_pixbuf_get_pixels(new);
	for (i = 0; i < height; i++) {
		unsigned char *p;

		p = rowp;
		for (j = 0; j < width; j++) {
			p[3] *= .25;
			p += 4;
		}

		rowp += rowstride;
	}

	return new;
}

static Icon *icon_new(ESplash *splash, GdkPixbuf *image_pixbuf)
{
	Icon *icon;

	icon = g_new(Icon, 1);

	icon->light_pixbuf =
	    gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, ICON_SIZE,
			   ICON_SIZE);
	gdk_pixbuf_scale(image_pixbuf, icon->light_pixbuf, 0, 0,
			 ICON_SIZE, ICON_SIZE, 0, 0,
			 (double)ICON_SIZE /
			     gdk_pixbuf_get_width(image_pixbuf),
			 (double)ICON_SIZE /
			     gdk_pixbuf_get_height(image_pixbuf),
			 GDK_INTERP_HYPER);

	icon->dark_pixbuf = create_darkened_pixbuf(icon->light_pixbuf);

	return icon;
}

static void icon_free(Icon *icon)
{
	g_object_unref(icon->dark_pixbuf);
	g_object_unref(icon->light_pixbuf);

	g_free(icon);
}

/* Icon layout management.  */

static void layout_icons(ESplash *splash)
{
	ESplashPrivate *priv;
	GList *p;
	double x_step;
	double x, y;

	priv = splash->priv;

	x_step =
	    ((double)gdk_pixbuf_get_width(priv->splash_image_pixbuf)) /
	    priv->num_icons;

	x = (x_step - ICON_SIZE) / 2.0;
	y = ICON_Y;

	/* for all icons, determine their x,y locations
	   and store them in the icon struct */
	for (p = priv->icons; p != NULL; p = p->next) {
		Icon *icon;
		icon = (Icon *)p->data;
		icon->x = x;
		icon->y = y;

		x += x_step;
	}
}

static int layout_idle_cb(void *data)
{
	ESplash *splash;
	ESplashPrivate *priv;

	splash = E_SPLASH(data);
	priv = splash->priv;

	layout_icons(splash);

	priv->layout_idle_id = 0;

	return FALSE;
}

static void schedule_relayout(ESplash *splash)
{
	ESplashPrivate *priv;

	priv = splash->priv;

	if (priv->layout_idle_id != 0)
		return;

	layout_idle_cb(splash);
}

/* GtkObject methods.  */

#ifdef USE_GTK_3
static void impl_destroy(GObject *object)
#else
static void impl_destroy(GtkObject *object)
#endif
{
	ESplash *splash;
	ESplashPrivate *priv;
	GList *p;

	splash = E_SPLASH(object);
	priv = splash->priv;

	if (priv->splash_image_pixbuf != NULL)
		g_object_unref(priv->splash_image_pixbuf);

	for (p = priv->icons; p != NULL; p = p->next) {
		Icon *icon;

		icon = (Icon *)p->data;
		icon_free(icon);
	}

	g_list_free(priv->icons);

	if (priv->layout_idle_id != 0)
		g_source_remove(priv->layout_idle_id);

	g_free(priv);
}

static void class_init(ESplashClass *klass)
{
#ifdef USE_GTK_3
	GtkWidgetClass *object_class;
	object_class = GTK_WIDGET_CLASS(klass);
#else
	GtkObjectClass *object_class;
	object_class = GTK_OBJECT_CLASS(klass);
#endif

	object_class->destroy = impl_destroy;

	parent_class = g_type_class_ref(gtk_window_get_type());
}

static void init(ESplash *splash)
{
	ESplashPrivate *priv;

	priv = g_new(ESplashPrivate, 1);
	priv->canvas = NULL;
	priv->splash_image_pixbuf = NULL;
	priv->icons = NULL;
	priv->num_icons = 0;
	priv->layout_idle_id = 0;

	splash->priv = priv;
}

static gboolean
button_press_event(GtkWidget *widget, GdkEventButton *event,
		   gpointer data)
{
	ESplash *splash = (ESplash *)data;

	gtk_widget_hide(GTK_WIDGET(splash));

	return TRUE;
}

/**
 * expose_event_callback
 * @widget: The canvas #GtkWidget
 * @event: #GdkEventExpose
 * @data: #gpointer which holds the ESplashPrivate struct
 *
 * this function is called every time the splash screen is redrawn
 * it draws the main image and each icon every time
 **/

gboolean
expose_event_callback(GtkWidget *widget, GdkEventExpose *event,
		      gpointer data)
{
#ifndef USE_GTK_3
	ESplashPrivate *priv = (ESplashPrivate *)data;
	GtkWidget *canvas = widget;
	GList *p;

	/* draws main image */
	gdk_draw_pixbuf(gtk_widget_get_window(canvas),
			NULL,
			GDK_PIXBUF(priv->splash_image_pixbuf),
			0, 0, 0, 0,
			gdk_pixbuf_get_width(priv->splash_image_pixbuf),
			gdk_pixbuf_get_height(priv->splash_image_pixbuf),
			2, 0, 0);

	/* draws each of the small icons */
	for (p = priv->icons; p != NULL; p = p->next) {
		Icon *icon;

		icon = (Icon *)p->data;

		gdk_draw_pixbuf(gtk_widget_get_window(canvas),
				NULL,
				GDK_PIXBUF(icon->light ? icon->light_pixbuf : icon->dark_pixbuf), 0, 0,
				icon->x, icon->y,
				gdk_pixbuf_get_width(icon->light_pixbuf),
				gdk_pixbuf_get_height(icon->light_pixbuf),
				2, 0, 0);
	}
	return TRUE;
#endif
	return FALSE;
}

/**
 * e_splash_construct:
 * @splash: A pointer to an ESplash widget
 * @splash_image_pixbuf: The pixbuf for the image to appear in the splash dialog
 *
 * Construct @splash with @splash_image_pixbuf as the splash image.
 **/
void e_splash_construct(ESplash *splash, GdkPixbuf *splash_image_pixbuf)
{
	ESplashPrivate *priv;
	GtkWidget *canvas, *frame, *vbox;
	int image_width, image_height;

	g_return_if_fail(splash != NULL);
	g_return_if_fail(E_IS_SPLASH(splash));
	g_return_if_fail(splash_image_pixbuf != NULL);

	priv = splash->priv;

	priv->splash_image_pixbuf = g_object_ref(splash_image_pixbuf);

	canvas = gtk_drawing_area_new();
	priv->canvas = GTK_DRAWING_AREA(canvas);

	image_width = gdk_pixbuf_get_width(splash_image_pixbuf);
	image_height = gdk_pixbuf_get_height(splash_image_pixbuf);

	gtk_widget_set_size_request(canvas, image_width, image_height);

	g_signal_connect(G_OBJECT(canvas), "expose_event",
			 G_CALLBACK(expose_event_callback), priv);

	gtk_widget_show(canvas);

	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
	gtk_widget_show(frame);

	gtk_container_add(GTK_CONTAINER(splash), frame);

	UI_VBOX(vbox, FALSE, 2);
	gtk_widget_show(vbox);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gtk_box_pack_start((GtkBox *)vbox, canvas, TRUE, TRUE, 0);

	priv->progressbar = (GtkProgressBar *)gtk_progress_bar_new();
	gtk_widget_show(GTK_WIDGET(priv->progressbar));
	gtk_box_pack_start((GtkBox *)vbox, GTK_WIDGET(priv->progressbar),
			   FALSE, TRUE, 0);

	g_signal_connect(G_OBJECT(splash), "button-press-event",
			 G_CALLBACK(button_press_event), splash);

	gtk_window_set_decorated(GTK_WINDOW(splash), FALSE);
	gtk_window_set_position(GTK_WINDOW(splash), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(splash), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(splash), image_width,
				    image_height + 20);
	gtk_window_set_title(GTK_WINDOW(splash),
			     _("Powered by the SWORD Project"));
}

/**
 * e_splash_new:
 *
 * Create a new ESplash widget.
 *
 * Return value: A pointer to the newly created ESplash widget.
 **/
GtkWidget *e_splash_new(void)
{
	ESplash *new;
	GError *error = NULL;
	GdkPixbuf *splash_image_pixbuf;

	splash_image_pixbuf = pixbuf_finder("splash.png", 0, &error);
	if (!splash_image_pixbuf) {
		XI_warning(("pixmap file error: %s\n", error->message));
		g_error_free(error);
		error = NULL;
		settings.showsplash = FALSE;
		return NULL;
	}
	g_return_val_if_fail(splash_image_pixbuf != NULL, NULL);

	new = g_object_new(e_splash_get_type(), NULL);
	e_splash_construct(new, splash_image_pixbuf);

	g_object_unref(splash_image_pixbuf);

	return GTK_WIDGET(new);
}

/**
 * e_splash_add_icon:
 * @splash: A pointer to an ESplash widget
 * @icon_pixbuf: Pixbuf for the icon to be added
 *
 * Add @icon_pixbuf to the @splash.
 *
 * Return value: The total number of icons in the splash after the new icon has
 * been added.
 **/
int e_splash_add_icon(ESplash *splash, GdkPixbuf *icon_pixbuf)
{
	ESplashPrivate *priv;
	Icon *icon;

	g_return_val_if_fail(splash != NULL, 0);
	g_return_val_if_fail(E_IS_SPLASH(splash), 0);
	g_return_val_if_fail(icon_pixbuf != NULL, 0);

	if (!splash)
		return 0;

	priv = splash->priv;

	icon = icon_new(splash, icon_pixbuf);
	icon->light = FALSE;
	icon->x = 0;
	icon->y = ICON_Y;

	GtkTextDirection dir =
	    gtk_widget_get_direction(GTK_WIDGET(splash));

	if (dir == GTK_TEXT_DIR_LTR)
		priv->icons = g_list_append(priv->icons, icon);
	else
		priv->icons = g_list_prepend(priv->icons, icon);

	priv->num_icons++;

	schedule_relayout(splash);

	return priv->num_icons;
}

/**
 * e_splash_set_icon_highlight:
 * @splash: A pointer to an ESplash widget
 * @num: Number of the icon whose highlight state must be changed
 * @highlight: Whether the icon must be highlit or not
 *
 * Change the highlight state of the @num-th icon.
 **/
void
e_splash_set_icon_highlight(ESplash *splash, int num, gboolean highlight)
{
	ESplashPrivate *priv;
	Icon *icon;
	GtkWidget *canvas;
	GdkRectangle rectangle;

	g_return_if_fail(splash != NULL);
	g_return_if_fail(E_IS_SPLASH(splash));

	if (!splash || (num >= splash->priv->num_icons))
		return; // no noisy logging

	priv = splash->priv;

	GtkTextDirection dir =
	    gtk_widget_get_direction(GTK_WIDGET(splash));

	if (dir == GTK_TEXT_DIR_LTR)
		icon = (Icon *)g_list_nth(priv->icons, num)->data;
	else
		icon =
		    (Icon *)g_list_nth(priv->icons,
				       priv->num_icons - num - 1)->data;

	icon->light = TRUE;

	rectangle.x = icon->x;
	rectangle.y = icon->y;
	rectangle.width = gdk_pixbuf_get_width(icon->light_pixbuf);
	rectangle.height = gdk_pixbuf_get_height(icon->light_pixbuf);

	canvas = GTK_WIDGET(priv->canvas);
	gdk_window_invalidate_rect(gtk_widget_get_window(canvas),
				   &rectangle, TRUE);
	gdk_window_process_updates(gtk_widget_get_window(canvas), TRUE);
}

E_MAKE_TYPE(e_splash, "ESplash", ESplash, class_init, init, PARENT_TYPE)

/*****************************************************************************/
static GtkWidget *splash = NULL;

void gui_splash_init()
{
	GError *error = NULL;

	if (settings.showsplash) {
		GList *icons = NULL;

		splash = e_splash_new();
		if (!splash)
			return;

		gtk_widget_show(splash);
		set_window_icon(GTK_WINDOW(splash));
		g_object_ref(G_OBJECT(splash));

		icons = g_list_append(icons, "gnome-windows.png");
		icons = g_list_append(icons, "sword.png");
		icons = g_list_append(icons, "gnome-session.png");
		icons = g_list_append(icons, "gs2-48x48.png");

		while (icons != NULL) {
			GdkPixbuf *icon_pixbuf =
			    pixbuf_finder(icons->data, 0, &error);
			if (!icon_pixbuf) {
				XI_warning(("pixmap file error: %s\n",
					    error->message));
				error = NULL;
				/* this is ugly but better than a crash */
				settings.showsplash = 0;
				g_object_unref(splash);
				gtk_widget_destroy(splash);
				return;
			}
			e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
			g_object_unref(icon_pixbuf);
			icons = g_list_next(icons);
		}

		g_list_free(icons);
	}
	sync_windows();
}

void gui_splash_step(gchar *text, gdouble progress, gint step)
{
	if (settings.showsplash) {
		ESplashPrivate *priv = E_SPLASH(splash)->priv;
		gtk_progress_bar_set_text(priv->progressbar, text);
		gtk_progress_bar_set_fraction(priv->progressbar, progress);
		e_splash_set_icon_highlight(E_SPLASH(splash), step, TRUE);

		sync_windows();
	}
}

gboolean gui_splash_done()
{
	if (settings.showsplash) {
		sync_windows();
		g_object_unref(splash);
		gtk_widget_destroy(splash);
	}
	return FALSE;
}

#else

GtkWidget *splash;
GtkWidget *progressbar;
GtkWidget *image1;
GtkWidget *image2;
GtkWidget *image3;
GtkWidget *image4;

gboolean gui_splash_done(void)
{
	if (settings.showsplash) {
		sync_windows();
		/*g_object_unref(splash); */
		gtk_widget_destroy(splash);
	}
	return FALSE;
}

void gui_splash_step(gchar *text, gdouble progress, gint step)
{
	if (settings.showsplash) {
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar),
					  text);
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), progress);

		sync_windows();
	}
}

void gui_splash_init(void)
{
	gchar *builder_file;
	GtkBuilder *builder;
	GError *error = NULL;

	if (!settings.showsplash)
		return;

	builder_file = gui_general_user_file("xi-splash.ui", FALSE);
	g_return_if_fail((builder_file != NULL));

	builder = gtk_builder_new();
	if (!gtk_builder_add_from_file(builder, builder_file, &error)) {
		g_warning("Couldn't load builder file: %s",
			  error->message);
		g_error_free(error);
	}

	/* This is important */
	splash = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	image1 = GTK_WIDGET(gtk_builder_get_object(builder, "image2"));
	image2 = GTK_WIDGET(gtk_builder_get_object(builder, "image3"));
	image3 = GTK_WIDGET(gtk_builder_get_object(builder, "image4"));
	image4 = GTK_WIDGET(gtk_builder_get_object(builder, "image5"));
	progressbar =
	    GTK_WIDGET(gtk_builder_get_object(builder, "progressbar1"));
	gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progressbar),
				       TRUE);

	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(builder);
}
#endif /* !USE_GTK_3 */
