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

#include <gnome.h>

#include "e-title-bar.h"

static void
delete_event_cb (GtkWidget *widget,
		 GdkEventAny *event,
		 gpointer data)
{
	gtk_main_quit ();
}

int
main (int argc, char **argv)
{
	GtkWidget *app;
	GtkWidget *title_bar;
	GtkWidget *text;
	GtkWidget *vbox;

	gnome_init ("test-title-bar", "0.0", argc, argv);

	app = gnome_app_new ("Test", "Test");
	gtk_window_set_default_size (GTK_WINDOW (app), 400, 400);
	gtk_window_set_policy (GTK_WINDOW (app), FALSE, TRUE, FALSE);

	gtk_signal_connect (GTK_OBJECT (app), "delete_event", GTK_SIGNAL_FUNC (delete_event_cb), NULL);

	title_bar = e_title_bar_new ("This is a very annoyingly long title bar");
	gtk_widget_show (title_bar);

	text = gtk_text_new (NULL, NULL);
	gtk_widget_show (text);

	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), title_bar, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), text, TRUE, TRUE, 0);
	gtk_widget_show (vbox);

	gnome_app_set_contents (GNOME_APP (app), vbox);
	gtk_widget_show (app);

	gtk_main ();

	return 0;
}
