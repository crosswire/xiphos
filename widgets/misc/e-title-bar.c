/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* e-title-bar.c
 *
 * Copyright (C) 2000 Helix Code, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Ettore Perazzoli
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtkbox.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkpixmap.h>
#include <gtk/gtksignal.h>
#include <gtk/gtktypeutils.h>

#include "e-util/e-util.h"
#include "e-clipped-label.h"

#include "e-title-bar.h"


enum {
	TITLE_BUTTON_PRESS_EVENT,
	CLOSE_BUTTON_CLICKED,
	LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = { 0 };


static char *close_button_xpm[] = {
	"12 12 2 1",
	" 	c none",
	".	c #000000000000",
	"            ",
	" ..      .. ",
	" ...    ... ",
	"  ...  ...  ",
	"   ......   ",
	"    ....    ",
	"    ....    ",
	"   ......   ",
	"  ...  ...  ",
	" ...    ... ",
	" ..      .. ",
	"            "
};

#define PARENT_TYPE GTK_TYPE_FRAME
static GtkFrameClass *parent_class = NULL;

struct _ETitleBarPrivate {
	GtkWidget *label;
	GtkWidget *close_button;
	GtkWidget *close_button_gtk_pixmap;
};


/* Child signal callbacks.  */

static void
close_button_realize_cb (GtkWidget *widget,
			 gpointer data)
{
	GdkPixmap *close_button_pixmap;
	GdkBitmap *close_button_mask;
	ETitleBar *title_bar;
	ETitleBarPrivate *priv;

	title_bar = E_TITLE_BAR (data);
	priv = title_bar->priv;

	if (priv->close_button_gtk_pixmap != NULL)
		return;

	close_button_pixmap = gdk_pixmap_create_from_xpm_d (GTK_WIDGET (priv->close_button)->window,
							    &close_button_mask,
							    NULL, close_button_xpm);
	priv->close_button_gtk_pixmap = gtk_pixmap_new (close_button_pixmap, close_button_mask);

	gtk_container_add (GTK_CONTAINER (priv->close_button), priv->close_button_gtk_pixmap);
	gtk_widget_show (priv->close_button_gtk_pixmap);

	gdk_pixmap_unref (close_button_pixmap);
	gdk_bitmap_unref (close_button_mask);
}

static void
close_button_clicked_cb (GtkButton *button,
			 gpointer data)
{
	ETitleBar *title_bar;

	title_bar = E_TITLE_BAR (data);

	gtk_signal_emit (GTK_OBJECT (title_bar), signals[CLOSE_BUTTON_CLICKED]);
}

static void
label_button_press_event_cb (GtkWidget *widget,
			     GdkEventButton *event,
			     gpointer data)
{
	ETitleBar *title_bar;

	title_bar = E_TITLE_BAR (data);

	gtk_signal_emit (GTK_OBJECT (title_bar), signals[TITLE_BUTTON_PRESS_EVENT], event);
}


/* GtkObject methods.  */

static void
destroy (GtkObject *object)
{
	ETitleBar *title_bar;
	ETitleBarPrivate *priv;

	title_bar = E_TITLE_BAR (object);
	priv = title_bar->priv;

	g_free (priv);

	(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}


static void
class_init (ETitleBarClass *klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass*) klass;
	object_class->destroy = destroy;

	parent_class = gtk_type_class (gtk_frame_get_type ());

	signals[TITLE_BUTTON_PRESS_EVENT] = 
		gtk_signal_new ("title_button_press_event",
				GTK_RUN_FIRST,
				object_class->type,
				GTK_SIGNAL_OFFSET (ETitleBarClass, title_button_press_event),
				gtk_marshal_NONE__POINTER,
				GTK_TYPE_NONE, 1,
				GTK_TYPE_GDK_EVENT);

	signals[CLOSE_BUTTON_CLICKED] = 
		gtk_signal_new ("close_button_clicked",
				GTK_RUN_FIRST,
				object_class->type,
				GTK_SIGNAL_OFFSET (ETitleBarClass, close_button_clicked),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);
}

static void
init (ETitleBar *title_bar)
{
	ETitleBarPrivate *priv;

	priv = g_new (ETitleBarPrivate, 1);

	priv->label                   = NULL;
	priv->close_button            = NULL;
	priv->close_button_gtk_pixmap = NULL;

	title_bar->priv = priv;
}


void
e_title_bar_construct (ETitleBar *title_bar,
		       const char *title)
{
	ETitleBarPrivate *priv;
	GtkWidget *hbox;

	g_return_if_fail (title_bar != NULL);
	g_return_if_fail (E_IS_TITLE_BAR (title_bar));

	priv = title_bar->priv;

	priv->label = e_clipped_label_new (title);
	gtk_misc_set_alignment (GTK_MISC (priv->label), 0.0, 0.5);
	gtk_widget_show (priv->label);

	priv->close_button = gtk_button_new ();
	GTK_WIDGET_UNSET_FLAGS (priv->close_button, GTK_CAN_FOCUS);
	gtk_container_set_border_width (GTK_CONTAINER (priv->close_button), 1);
	gtk_button_set_relief (GTK_BUTTON (priv->close_button), GTK_RELIEF_NONE);
	gtk_widget_show (priv->close_button);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), priv->label, TRUE, TRUE, 2);
	gtk_box_pack_start (GTK_BOX (hbox), priv->close_button, FALSE, TRUE, 1);
	gtk_widget_show (hbox);

	gtk_container_add (GTK_CONTAINER (title_bar), hbox);

	gtk_signal_connect (GTK_OBJECT (priv->close_button), "realize",
			    GTK_SIGNAL_FUNC (close_button_realize_cb), title_bar);
	gtk_signal_connect (GTK_OBJECT (priv->close_button), "clicked",
			    GTK_SIGNAL_FUNC (close_button_clicked_cb), title_bar);
	gtk_signal_connect (GTK_OBJECT (priv->label), "button_press_event",
			    GTK_SIGNAL_FUNC (label_button_press_event_cb), title_bar);
}

GtkWidget *
e_title_bar_new (const char *title)
{
	ETitleBar *title_bar;

	title_bar = gtk_type_new (e_title_bar_get_type ());

	e_title_bar_construct (title_bar, title);

	return GTK_WIDGET (title_bar);
}


void
e_title_bar_set_title (ETitleBar *title_bar,
		       const char *title)
{
	g_return_if_fail (title_bar != NULL);
	g_return_if_fail (E_IS_TITLE_BAR (title_bar));

	e_clipped_label_set_text (E_CLIPPED_LABEL (title_bar->priv->label), title);
}

void
e_title_bar_show_close_button (ETitleBar *title_bar,
			       gboolean show)
{
	ETitleBarPrivate *priv;

	g_return_if_fail (title_bar != NULL);
	g_return_if_fail (E_IS_TITLE_BAR (title_bar));

	priv = title_bar->priv;

	if (show)
		gtk_widget_show (priv->close_button);
	else
		gtk_widget_hide (priv->close_button);
}


E_MAKE_TYPE (e_title_bar, "ETitleBar", ETitleBar, class_init, init, PARENT_TYPE)
