/*
 * GnomeSword Bible Study Tool
 * splash.c - Splash related functions
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * Contains code from Evolution as written by Ettore Perazzoli.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtk/gtk.h>
#include <gtk/gtkwindow.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
//#include <gdk-pixbuf/gnome-canvas-pixbuf.h>
#include <gal/util/e-util.h>

#include "main/settings.h"
#include "gui/splash.h"
#include "gui/gnomesword.h"

/*****************************************************************************/
/* FIXME: clean up below */

#define E_TYPE_SPLASH			(e_splash_get_type ())
#define E_SPLASH(obj)			(GTK_CHECK_CAST ((obj), E_TYPE_SPLASH, ESplash))
#define E_SPLASH_CLASS(klass)		(GTK_CHECK_CLASS_CAST ((klass), E_TYPE_SPLASH, ESplashClass))
#define E_IS_SPLASH(obj)			(GTK_CHECK_TYPE ((obj), E_TYPE_SPLASH))
#define E_IS_SPLASH_CLASS(klass)		(GTK_CHECK_CLASS_TYPE ((obj), E_TYPE_SPLASH))

typedef struct _ESplash        ESplash;
typedef struct _ESplashPrivate ESplashPrivate;
typedef struct _ESplashClass   ESplashClass;

struct _ESplash {
	GtkWindow parent;

	ESplashPrivate *priv;
};

struct _ESplashClass {
	GtkWindowClass parent_class;
};

GtkType    e_splash_get_type            (void);
void       e_splash_construct           (ESplash   *splash,
					 GdkPixbuf *splash_image);
GtkWidget *e_splash_new                 (void);

int        e_splash_add_icon            (ESplash   *splash,
					 GdkPixbuf *icon);
void       e_splash_set_icon_highlight  (ESplash   *splash,
					 int        num,
					 gboolean   highlight);

#define PARENT_TYPE gtk_window_get_type ()
static GtkWindowClass *parent_class = NULL;

struct _Icon {
	GdkPixbuf *dark_pixbuf;
	GdkPixbuf *light_pixbuf;
	GnomeCanvasItem *canvas_item;
};
typedef struct _Icon Icon;

struct _ESplashPrivate {
	GnomeCanvas *canvas;
	GdkPixbuf *splash_image_pixbuf;

	GList *icons;		/* (Icon *) */
	int num_icons;

	int layout_idle_id;
};


/* Layout constants.  These need to be changed if the splash changes.  */

#define ICON_Y    290
#define ICON_SIZE 25


/* Icon management.  */

static GdkPixbuf *
create_darkened_pixbuf (GdkPixbuf *pixbuf)
{
	GdkPixbuf *new;
	unsigned char *rowp;
	int width, height;
	int rowstride;
	int i, j;

	new = gdk_pixbuf_copy (pixbuf);
	if (! gdk_pixbuf_get_has_alpha (new))
		return new;

	width     = gdk_pixbuf_get_width (new);
	height    = gdk_pixbuf_get_height (new);
	rowstride = gdk_pixbuf_get_rowstride (new);

	rowp = gdk_pixbuf_get_pixels (new);
	for (i = 0; i < height; i ++) {
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

static Icon *
icon_new (ESplash *splash,
	  GdkPixbuf *image_pixbuf)
{
	ESplashPrivate *priv;
	GnomeCanvasGroup *canvas_root_group;
	Icon *icon;

	priv = splash->priv;

	icon = g_new (Icon, 1);

	icon->light_pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, ICON_SIZE, ICON_SIZE);
	gdk_pixbuf_scale (image_pixbuf, icon->light_pixbuf,
			  0, 0,
			  ICON_SIZE, ICON_SIZE,
			  0, 0,
			  (double) ICON_SIZE / gdk_pixbuf_get_width (image_pixbuf),
			  (double) ICON_SIZE / gdk_pixbuf_get_height (image_pixbuf),
			  GDK_INTERP_HYPER);

	icon->dark_pixbuf  = create_darkened_pixbuf (icon->light_pixbuf);

	/* Set up the canvas item to point to the dark pixbuf initially.  */

	canvas_root_group = GNOME_CANVAS_GROUP (GNOME_CANVAS (priv->canvas)->root);

	icon->canvas_item = gnome_canvas_item_new (canvas_root_group,
						   GNOME_TYPE_CANVAS_PIXBUF,
						   "pixbuf", icon->dark_pixbuf,
						   NULL);

	return icon;
}

static void
icon_free (Icon *icon)
{
	gdk_pixbuf_unref (icon->dark_pixbuf);
	gdk_pixbuf_unref (icon->light_pixbuf);
/*  	gtk_object_unref (GTK_OBJECT (icon->canvas_item)); */

	g_free (icon);
}


/* Icon layout management.  */

static void
layout_icons (ESplash *splash)
{
	ESplashPrivate *priv;
	GList *p;
	double x_step;
	double x, y;

	priv = splash->priv;

	x_step = ((double) gdk_pixbuf_get_width (priv->splash_image_pixbuf)) / priv->num_icons;

	x = (x_step - ICON_SIZE) / 2.0;
	y = ICON_Y;

	for (p = priv->icons; p != NULL; p = p->next) {
		Icon *icon;

		icon = (Icon *) p->data;

		gtk_object_set (GTK_OBJECT (icon->canvas_item),
				"x", (double) x,
				"y", (double) ICON_Y,
				NULL);

		x += x_step;
	}
}

static int
layout_idle_cb (void *data)
{
	ESplash *splash;
	ESplashPrivate *priv;

	splash = E_SPLASH (data);
	priv = splash->priv;

	layout_icons (splash);

	priv->layout_idle_id = 0;

	return FALSE;
}

static void
schedule_relayout (ESplash *splash)
{
	ESplashPrivate *priv;

	priv = splash->priv;

	if (priv->layout_idle_id != 0)
		return;

	priv->layout_idle_id = gtk_idle_add (layout_idle_cb, splash);
}


/* GtkObject methods.  */

static void
impl_destroy (GtkObject *object)
{
	ESplash *splash;
	ESplashPrivate *priv;
	GList *p;

	splash = E_SPLASH (object);
	priv = splash->priv;

	if (priv->splash_image_pixbuf != NULL)
		gdk_pixbuf_unref (priv->splash_image_pixbuf);

	for (p = priv->icons; p != NULL; p = p->next) {
		Icon *icon;

		icon = (Icon *) p->data;
		icon_free (icon);
	}

	g_list_free (priv->icons);

	if (priv->layout_idle_id != 0)
		gtk_idle_remove (priv->layout_idle_id);

	g_free (priv);
}


static void
class_init (ESplashClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS (klass);
	object_class->destroy = impl_destroy;

	parent_class = gtk_type_class (gtk_window_get_type ());
}

static void
init (ESplash *splash)
{
	ESplashPrivate *priv;

	priv = g_new (ESplashPrivate, 1);
	priv->canvas              = NULL;
	priv->splash_image_pixbuf = NULL;
	priv->icons               = NULL;
	priv->num_icons           = 0;
	priv->layout_idle_id      = 0;

	splash->priv = priv;
}

static gboolean
button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	ESplash *splash = (ESplash *) data;
	
	gtk_widget_hide (GTK_WIDGET (splash));
	
	return TRUE;
}

/**
 * e_splash_construct:
 * @splash: A pointer to an ESplash widget
 * @splash_image_pixbuf: The pixbuf for the image to appear in the splash dialog
 * 
 * Construct @splash with @splash_image_pixbuf as the splash image.
 **/
void
e_splash_construct (ESplash *splash,
		    GdkPixbuf *splash_image_pixbuf)
{
	ESplashPrivate *priv;
	GtkWidget *canvas, *frame;
	int image_width, image_height;

	g_return_if_fail (splash != NULL);
	g_return_if_fail (E_IS_SPLASH (splash));
	g_return_if_fail (splash_image_pixbuf != NULL);

	priv = splash->priv;

	priv->splash_image_pixbuf = gdk_pixbuf_ref (splash_image_pixbuf);

	canvas = gnome_canvas_new_aa ();
	priv->canvas = GNOME_CANVAS (canvas);

	image_width = gdk_pixbuf_get_width (splash_image_pixbuf);
	image_height = gdk_pixbuf_get_height (splash_image_pixbuf);

	gtk_widget_set_usize (canvas, image_width, image_height);
	gnome_canvas_set_scroll_region (GNOME_CANVAS (canvas), 0, 0, image_width, image_height);
	gtk_widget_show (canvas);

	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	gtk_container_add (GTK_CONTAINER (frame), canvas);
	gtk_widget_show (frame);

	gtk_container_add (GTK_CONTAINER (splash), frame);

	gnome_canvas_item_new (GNOME_CANVAS_GROUP (priv->canvas->root),
			       GNOME_TYPE_CANVAS_PIXBUF,
			       "pixbuf", splash_image_pixbuf,
			       NULL);
	
	gtk_signal_connect (GTK_OBJECT (splash), "button-press-event",
			    G_CALLBACK (button_press_event), splash);
	
	gtk_object_set (GTK_OBJECT (splash), "type", GTK_WINDOW_POPUP, NULL);
	gtk_window_set_position (GTK_WINDOW (splash), GTK_WIN_POS_CENTER);
	gtk_window_set_policy (GTK_WINDOW (splash), FALSE, FALSE, FALSE);
	gtk_window_set_default_size (GTK_WINDOW (splash), image_width, image_height);
	gtk_window_set_title (GTK_WINDOW (splash), "GnomeSword");

}

/**
 * e_splash_new:
 *
 * Create a new ESplash widget.
 * 
 * Return value: A pointer to the newly created ESplash widget.
 **/
GtkWidget *
e_splash_new (void) 
{
	ESplash *new;
	GdkPixbuf *splash_image_pixbuf;
	GError *error = NULL;
	splash_image_pixbuf =
		gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/splash.png", 
						&error);
	if(!splash_image_pixbuf){
		fprintf (stderr, "pixmap file error: %s\n", error->message);
		g_error_free (error);
		error = NULL;
		settings.showsplash = FALSE;
		return NULL;
	}
	g_return_val_if_fail (splash_image_pixbuf != NULL, NULL);

	new = gtk_type_new (e_splash_get_type ());
	e_splash_construct (new, splash_image_pixbuf);

	gdk_pixbuf_unref (splash_image_pixbuf);

	return GTK_WIDGET (new);
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
int
e_splash_add_icon (ESplash *splash,
		   GdkPixbuf *icon_pixbuf)
{
	ESplashPrivate *priv;
	Icon *icon;

	g_return_val_if_fail (splash != NULL, 0);
	g_return_val_if_fail (E_IS_SPLASH (splash), 0);
	g_return_val_if_fail (icon_pixbuf != NULL, 0);

	if (!splash)
		return 0;
	
	priv = splash->priv;

	icon = icon_new (splash, icon_pixbuf);
	priv->icons = g_list_append (priv->icons, icon);

	priv->num_icons ++;

	schedule_relayout (splash);

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
e_splash_set_icon_highlight  (ESplash *splash,
			      int num,
			      gboolean highlight)
{
	ESplashPrivate *priv;
	Icon *icon;

	g_return_if_fail (splash != NULL);
	g_return_if_fail (E_IS_SPLASH (splash));

	if (!splash)
		return;
	
	priv = splash->priv;

	icon = (Icon *) g_list_nth (priv->icons, num)->data;

	gtk_object_set (GTK_OBJECT (icon->canvas_item),
			"pixbuf", highlight ? icon->light_pixbuf : icon->dark_pixbuf,
			NULL);
}

E_MAKE_TYPE (e_splash, "ESplash", ESplash, class_init, init, PARENT_TYPE)

/*****************************************************************************/

static GtkWidget *splash = NULL;

void gui_splash_init() {
	GdkPixbuf *icon_pixbuf;
	GError *error = NULL;

	if (settings.showsplash) {
		splash = e_splash_new();
		if(!splash) return;
			
		gtk_widget_show(splash);
		gtk_object_ref(GTK_OBJECT(splash));

		icon_pixbuf = gdk_pixbuf_new_from_file(
				PACKAGE_PIXMAPS_DIR "/sword.xpm", &error);
		if(!icon_pixbuf){
			fprintf (stderr, "pixmap file error: %s\n", error->message);
			g_error_free (error);
		}
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);

		icon_pixbuf = gdk_pixbuf_new_from_file(
				PACKAGE_PIXMAPS_DIR "/book-gold.png", &error);
		if(!icon_pixbuf){
			fprintf (stderr, "pixmap file error: %s\n", error->message);
			g_error_free (error);
			error = NULL;
		}
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);

		icon_pixbuf = gdk_pixbuf_new_from_file(
				PACKAGE_PIXMAPS_DIR "/book-green.png", &error);
		if(!icon_pixbuf){
			fprintf (stderr, "pixmap file error: %s\n", error->message);
			g_error_free (error);
			error = NULL;
		}
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);

		icon_pixbuf = gdk_pixbuf_new_from_file(
				PACKAGE_PIXMAPS_DIR "/GnomeSword.png", &error);
		if(!icon_pixbuf){
			fprintf (stderr, "pixmap file error: %s\n", error->message);
			g_error_free (error);
			error = NULL;
		}
		e_splash_add_icon(E_SPLASH(splash), icon_pixbuf);
		gdk_pixbuf_unref(icon_pixbuf);
	}
}

void gui_splash_step1() {
	if (settings.showsplash) {
		e_splash_set_icon_highlight(E_SPLASH(splash), 0, TRUE);

		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
	}
}


void gui_splash_step2() {
	if (settings.showsplash) {
		e_splash_set_icon_highlight(E_SPLASH(splash), 1, TRUE);

		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
	}
}

void gui_splash_step3() {
	if (settings.showsplash) {
		e_splash_set_icon_highlight(E_SPLASH(splash), 2, TRUE);

		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
	}
}

void gui_splash_step4() {
	if (settings.showsplash) {
		e_splash_set_icon_highlight(E_SPLASH(splash), 3, TRUE);

		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
	}
}

void gui_splash_done() {
	if (settings.showsplash) {
		
		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
		gtk_widget_unref(splash);
		gtk_widget_destroy(splash);
	}
}
