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

#include <gal/util/e-util.h>
#include "e-clipped-label.h"

#include "e-title-bar.h"


enum {
	LABEL_BUTTON_PRESS_EVENT,
	BUTTON_CLICKED,
	LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = { 0 };


/* The pixmaps.  */

static char *close_xpm[] = {
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

static char *pin_xpm[] = {
	"16 16 33 1",
	" 	c None",
	".	c #000000",
	"+	c #74744D",
	"@	c #B2B279",
	"#	c #6C6C46",
	"$	c #CACA9C",
	"%	c #F4F4AD",
	"&	c #85855A",
	"*	c #B1B175",
	"=	c #9A9A66",
	"-	c #A9A98A",
	";	c #B0B07B",
	">	c #535353",
	",	c #818181",
	"'	c #B7B7B7",
	")	c #D8D8D8",
	"!	c #FFFFFF",
	"~	c #EBEBA1",
	"{	c #8A8A75",
	"]	c #9F9F76",
	"^	c #9E9E75",
	"/	c #8A8A66",
	"(	c #979770",
	"_	c #6B6B46",
	":	c #28281A",
	"<	c #505034",
	"[	c #666645",
	"}	c #61614D",
	"|	c #818155",
	"1	c #4A4A31",
	"2	c #4D4D34",
	"3	c #6C6C48",
	"4	c #5D5D3E",
	"                ",
	"                ",
	"                ",
	"      .     .   ",
	"      ..   .+.  ",
	"      .@...#$.  ",
	" ......%&*=-;.  ",
	".>,')!.~{]^/(.  ",
	" ......_:<[}|.  ",
	"      .1...23.  ",
	"      ..   .4.  ",
	"      .     .   ",
	"                ",
	"                ",
	"                ",
	"                "
};
	

#define PARENT_TYPE GTK_TYPE_FRAME
static GtkFrameClass *parent_class = NULL;

struct _ETitleBarPrivate {
	ETitleBarButtonMode button_mode;
	GtkWidget *label;
	GtkWidget *button;
	GtkWidget *close_gtk_pixmap;
	GtkWidget *pin_gtk_pixmap;
};


/* Mode handling.  We put both the close and pin GtkPixmaps into an hbox in the
   button, and hide one of them according to the mode.  */

static void
show_and_hide_pixmaps_according_to_mode (ETitleBar *title_bar)
{
	ETitleBarPrivate *priv;

	priv = title_bar->priv;

	if (priv->close_gtk_pixmap == NULL || priv->pin_gtk_pixmap == NULL)
		return;

	switch (priv->button_mode) {
	case E_TITLE_BAR_BUTTON_MODE_PIN:
		gtk_widget_hide (priv->close_gtk_pixmap);
		gtk_widget_show (priv->pin_gtk_pixmap);
		break;
	case E_TITLE_BAR_BUTTON_MODE_CLOSE:
		gtk_widget_hide (priv->pin_gtk_pixmap);
		gtk_widget_show (priv->close_gtk_pixmap);
		break;
	default:
		g_assert_not_reached ();
	}
}


/* Child signal callbacks.  */

static void
button_realize_cb (GtkWidget *widget,
		   gpointer data)
{
	GdkPixmap *close_pixmap;
	GdkBitmap *close_mask;
	GdkPixmap *pin_pixmap;
	GdkBitmap *pin_mask;
	GtkWidget *hbox;
	ETitleBar *title_bar;
	ETitleBarPrivate *priv;

	title_bar = E_TITLE_BAR (data);
	priv = title_bar->priv;

	if (priv->close_gtk_pixmap != NULL)
		return;

	close_pixmap = gdk_pixmap_create_from_xpm_d (GTK_WIDGET (priv->button)->window,
						     &close_mask, NULL, close_xpm);
	priv->close_gtk_pixmap = gtk_pixmap_new (close_pixmap, close_mask);

	pin_pixmap = gdk_pixmap_create_from_xpm_d (GTK_WIDGET (priv->button)->window,
						   &pin_mask, NULL, pin_xpm);
	priv->pin_gtk_pixmap = gtk_pixmap_new (pin_pixmap, pin_mask);

	hbox = gtk_hbox_new (TRUE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (hbox), priv->pin_gtk_pixmap, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), priv->close_gtk_pixmap, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (priv->button), hbox);

	gdk_pixmap_unref (close_pixmap);
	gdk_bitmap_unref (close_mask);
	gdk_pixmap_unref (pin_pixmap);
	gdk_bitmap_unref (pin_mask);

	show_and_hide_pixmaps_according_to_mode (title_bar);
}

static void
button_clicked_cb (GtkButton *button,
		   gpointer data)
{
	ETitleBar *title_bar;

	title_bar = E_TITLE_BAR (data);

	gtk_signal_emit (GTK_OBJECT (title_bar), signals[BUTTON_CLICKED]);
}

static void
label_button_press_event_cb (GtkWidget *widget,
			     GdkEventButton *event,
			     gpointer data)
{
	ETitleBar *title_bar;

	title_bar = E_TITLE_BAR (data);

	gtk_signal_emit (GTK_OBJECT (title_bar), signals[LABEL_BUTTON_PRESS_EVENT], event);
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

	signals[LABEL_BUTTON_PRESS_EVENT] = 
		gtk_signal_new ("label_button_press_event",
				GTK_RUN_FIRST,
				object_class->type,
				GTK_SIGNAL_OFFSET (ETitleBarClass, label_button_press_event),
				gtk_marshal_NONE__POINTER,
				GTK_TYPE_NONE, 1,
				GTK_TYPE_GDK_EVENT);

	signals[BUTTON_CLICKED] = 
		gtk_signal_new ("button_clicked",
				GTK_RUN_FIRST,
				object_class->type,
				GTK_SIGNAL_OFFSET (ETitleBarClass, button_clicked),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);

	gtk_object_class_add_signals (object_class, signals, LAST_SIGNAL);
}

static void
init (ETitleBar *title_bar)
{
	ETitleBarPrivate *priv;

	priv = g_new (ETitleBarPrivate, 1);

	priv->button_mode      = E_TITLE_BAR_BUTTON_MODE_CLOSE;
	priv->label            = NULL;
	priv->button           = NULL;
	priv->close_gtk_pixmap = NULL;
	priv->pin_gtk_pixmap   = NULL;

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

	priv->button = gtk_button_new ();
	GTK_WIDGET_UNSET_FLAGS (priv->button, GTK_CAN_FOCUS);
	gtk_container_set_border_width (GTK_CONTAINER (priv->button), 1);
	gtk_button_set_relief (GTK_BUTTON (priv->button), GTK_RELIEF_NONE);
	gtk_widget_show (priv->button);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), priv->label, TRUE, TRUE, 2);
	gtk_box_pack_start (GTK_BOX (hbox), priv->button, FALSE, TRUE, 1);
	gtk_widget_show (hbox);

	gtk_container_add (GTK_CONTAINER (title_bar), hbox);

	gtk_signal_connect (GTK_OBJECT (priv->button), "realize",
			    GTK_SIGNAL_FUNC (button_realize_cb), title_bar);
	gtk_signal_connect (GTK_OBJECT (priv->button), "clicked",
			    GTK_SIGNAL_FUNC (button_clicked_cb), title_bar);
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
e_title_bar_show_button (ETitleBar *title_bar,
			 gboolean show)
{
	ETitleBarPrivate *priv;

	g_return_if_fail (title_bar != NULL);
	g_return_if_fail (E_IS_TITLE_BAR (title_bar));

	priv = title_bar->priv;

	if (show)
		gtk_widget_show (priv->button);
	else
		gtk_widget_hide (priv->button);
}

void
e_title_bar_set_button_mode (ETitleBar *title_bar,
			     ETitleBarButtonMode button_mode)
{
	ETitleBarPrivate *priv;

	g_return_if_fail (title_bar != NULL);
	g_return_if_fail (E_IS_TITLE_BAR (title_bar));
	g_return_if_fail (button_mode == E_TITLE_BAR_BUTTON_MODE_CLOSE
			  || button_mode == E_TITLE_BAR_BUTTON_MODE_PIN);

	priv = title_bar->priv;

	if (priv->button_mode == button_mode)
		return;

	priv->button_mode = button_mode;
	show_and_hide_pixmaps_according_to_mode (title_bar);
}

ETitleBarButtonMode
e_title_bar_get_button_mode (ETitleBar *title_bar)
{
	g_return_val_if_fail (title_bar != NULL, E_TITLE_BAR_BUTTON_MODE_CLOSE);
	g_return_val_if_fail (E_IS_TITLE_BAR (title_bar), E_TITLE_BAR_BUTTON_MODE_CLOSE);

	return title_bar->priv->button_mode;
}


E_MAKE_TYPE (e_title_bar, "ETitleBar", ETitleBar, class_init, init, PARENT_TYPE)
