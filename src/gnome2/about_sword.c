/*
 * Xiphos Bible Study Tool
 * about_sword.c - Xiphos "About Sword" dialog
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "gui/about_sword.h"
#include "gui/utilities.h"
#include "main/settings.h"
#include "main/sword.h"

/******************************************************************************
 * Name
 *   on_dialog_response
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_dialog_response(GtkDialog * dialog, gint response_id,
					gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void
on_dialog_response(GtkDialog *dialog, gint response_id,
		   gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

/******************************************************************************
 * Name
 *   gui_create_about_sword
 *
 * Synopsis
 *   #include "gui/about_sword.h"
 *
 *   void gui_create_about_sword(void);
 *
 * Description
 *   Create Sword about dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_about_sword(void)
{
	GdkPixbuf *about1_logo_pixbuf;
	GtkWidget *about1;
	gchar *about =
	    _("The SWORD Project is an effort to create an ever-expanding \
software package for research and study of God and His Word. \
The SWORD Project framework allows easy use and study of Bible \
texts, commentaries, lexicons, dictionaries, and other books. \
Many frontends are built using this framework. An installed \
set of books may be shared among all frontends using the \
framework.\n\n \
Books can be downloaded from the SWORD Project.");

	about1_logo_pixbuf = pixbuf_finder("sword-big.png", 0, NULL);

	about1 = gtk_about_dialog_new();
	g_signal_connect(about1, "response",
			 G_CALLBACK(on_dialog_response), NULL);
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about1), "The SWORD Project");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about1), (gchar *)main_get_sword_version());
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about1), about);

	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about1), "http://www.crosswire.org/");
	gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about1), _("The SWORD Project"));

	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about1), about1_logo_pixbuf);

	set_window_icon(GTK_WINDOW(about1));

	return about1;
}
