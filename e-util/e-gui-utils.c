/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * GUI utility functions
 *
 * Author:
 *   Miguel de Icaza (miguel@helixcode.com)
 *
 * (C) 1999 Miguel de Icaza
 * (C) 2000 Helix Code, Inc.
 */
#include <config.h>
#include <gtk/gtksignal.h>
#include <libgnomeui/gnome-messagebox.h>
#include <libgnomeui/gnome-stock.h>
#include <gnome.h>
#include "e-gui-utils.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf/gnome-canvas-pixbuf.h>

void
e_notice (GtkWindow *window, const char *type, const char *format, ...)
{
	GtkWidget *dialog;
	va_list args;
	char *str;

	va_start (args, format);
	str = g_strdup_vprintf (format, args);
	dialog = gnome_message_box_new (str, type, GNOME_STOCK_BUTTON_OK, NULL);
	va_end (args);
	g_free (str);
	
	if (window)
		gnome_dialog_set_parent (GNOME_DIALOG (dialog), window);

	gnome_dialog_run (GNOME_DIALOG (dialog));
}

static void
kill_popup_menu (GtkWidget *widget, GtkMenu *menu)
{
	g_return_if_fail (menu != NULL);
	g_return_if_fail (GTK_IS_MENU (menu));

	gtk_object_unref (GTK_OBJECT (menu));
}

void
e_auto_kill_popup_menu_on_hide (GtkMenu *menu)
{
	g_return_if_fail (menu != NULL);
	g_return_if_fail (GTK_IS_MENU (menu));

	gtk_signal_connect (GTK_OBJECT (menu), "hide",
			    GTK_SIGNAL_FUNC (kill_popup_menu), menu);
}

void
e_popup_menu (GtkMenu *menu, GdkEventButton *event)
{
	g_return_if_fail (menu != NULL);
	g_return_if_fail (GTK_IS_MENU (menu));

	e_auto_kill_popup_menu_on_hide (menu);
	gtk_menu_popup (menu, NULL, NULL, 0, NULL, event->button, event->time);
}

GtkWidget *e_create_image_widget(gchar *name,
				 gchar *string1, gchar *string2,
				 gint int1, gint int2)
{
	char *filename;
	GdkPixbuf *pixbuf;
	double width, height;
	GtkWidget *canvas, *alignment;
	if (string1) {
		if (*string1 == '/')
			filename = g_strdup(string1);
		else
			filename = g_concat_dir_and_file(EVOLUTION_IMAGES, string1);
		pixbuf = gdk_pixbuf_new_from_file(filename);
		width = gdk_pixbuf_get_width(pixbuf);
		height = gdk_pixbuf_get_height(pixbuf);

		canvas = gnome_canvas_new_aa();
		GTK_OBJECT_UNSET_FLAGS(GTK_WIDGET(canvas), GTK_CAN_FOCUS);
		gnome_canvas_item_new(gnome_canvas_root(GNOME_CANVAS(canvas)),
				      gnome_canvas_pixbuf_get_type(),

				      "pixbuf", pixbuf,
				      NULL);

		alignment = gtk_widget_new(gtk_alignment_get_type(),
					   "child", canvas,
					   "xalign", (double) 0,
					   "yalign", (double) 0,
					   "xscale", (double) 0,
					   "yscale", (double) 0,
					   NULL);
	
		gtk_widget_set_usize(canvas, width, height);

		gdk_pixbuf_unref(pixbuf);

		gtk_widget_show(canvas);
		gtk_widget_show(alignment);
		g_free(filename);

		return alignment;
	} else
		return NULL;
}

typedef struct {
	GtkCallback callback;
	gpointer closure;
} CallbackClosure;

static void
e_container_foreach_leaf_callback(GtkWidget *widget, CallbackClosure *callback_closure)
{
	if (GTK_IS_CONTAINER(widget)) {
		e_container_foreach_leaf(GTK_CONTAINER(widget), callback_closure->callback, callback_closure->closure);
	} else {
		(*callback_closure->callback) (widget, callback_closure->closure);
	}
}

void
e_container_foreach_leaf(GtkContainer *container,
			 GtkCallback callback,
			 gpointer closure)
{
	CallbackClosure callback_closure;
	callback_closure.callback = callback;
	callback_closure.closure = closure;
	gtk_container_foreach(container, (GtkCallback) e_container_foreach_leaf_callback, &callback_closure);
}
