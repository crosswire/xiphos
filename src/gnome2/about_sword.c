/*
 * Xiphos Bible Study Tool
 * about_sword.c - Sword about dialog
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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

#include "gui/about_sword.h"
#include "gui/utilities.h"
#include "main/settings.h"
#include "main/sword.h"

/******************************************************************************
 * Name
 *   about_sword_ok
 *
 * Description
 *   Callback if about sword button is clicked.
 *
 * Return value
 *   void
 */

static void
about_sword_ok(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

/******************************************************************************
 * Name
 *   gui_create_about_sword
 *
 * Synopsis
 *   #include "about_sword.h"
 *
 *   void gui_create_about_sword(void);
 *
 * Description
 *   Create Sword about dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *
gui_create_about_sword(void)
{
	GtkWidget *dialog_about_sword;
	GtkWidget *dialog_vbox;
	GtkWidget *vbox;
	GtkWidget *pixmap;
	GtkWidget *version_label;
	GtkWidget *scrolledwindow;
	GtkWidget *label;
	GtkWidget *textview;
	GtkWidget *href;
	GtkWidget *dialog_action_area;
	GtkWidget *hbuttonbox;
	GtkWidget *button;
	const char *ver;
	gchar version[40];
	gchar *about =
	    _
	    ("The SWORD Project is an effort to create an ever-expanding "
	     "software package for research and study of God and His Word. "
	     "The SWORD Project framework allows easy use and study of Bible "
	     "texts, commentaries, lexicons, dictionaries, and other books. "
	     "Many frontends are built using this framework. An installed "
	     "set of books may be shared among all frontends using the "
	     "framework.");

	/*
	 * get sword version
	 */
	ver = main_get_sword_version();
	sprintf(version, "<b>SWORD-%s</b>", ver);

	dialog_about_sword = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog_about_sword),
			    "dialog_about_sword", dialog_about_sword);
	gtk_window_set_title(GTK_WINDOW(dialog_about_sword),
			     _("About the SWORD Project"));
	set_window_icon (GTK_WINDOW(dialog_about_sword));
        gtk_window_set_resizable(GTK_WINDOW(dialog_about_sword), FALSE);

	dialog_vbox = GTK_DIALOG(dialog_about_sword)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_about_sword),
			    "dialog_vbox", dialog_vbox);
	gtk_widget_show(dialog_vbox);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox);
	gtk_box_pack_start(GTK_BOX(dialog_vbox), vbox, TRUE, TRUE,
			   0);

	pixmap = pixmap_finder("sword3.png");
	gtk_widget_show(pixmap);
	gtk_box_pack_start(GTK_BOX(vbox), pixmap, FALSE, FALSE, 4);

	version_label = gtk_label_new(_("Sword Version"));
	gtk_widget_show(version_label);
	gtk_box_pack_start(GTK_BOX(vbox), version_label, FALSE, FALSE,
			   0);

	gtk_label_set_markup(GTK_LABEL(version_label), version);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE,
			   TRUE, 4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
  	gtk_widget_set_size_request (scrolledwindow, 160, 180);
  	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)scrolledwindow,
                                             settings.shadow_type);

	textview = gtk_text_view_new ();
	gtk_widget_show (textview);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), textview);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview),FALSE);
	gtk_text_buffer_set_text (gtk_text_view_get_buffer
				(GTK_TEXT_VIEW (textview)), about, -1);
	gtk_widget_set_sensitive(textview,FALSE);

	label =
	    gtk_label_new(_
			  ("Books can be downloaded from the SWORD Project"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	gtk_widget_set_usize(label, -2, 24);

	href =  gtk_link_button_new_with_label ("http://www.crosswire.org/sword",
                                                       			_("The SWORD Project") );
	gtk_widget_show(href);
	gtk_box_pack_start(GTK_BOX(vbox), href, FALSE, FALSE, 0);

	dialog_action_area =
	    GTK_DIALOG(dialog_about_sword)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog_about_sword),
			    "dialog_action_area",
			    dialog_action_area);
	gtk_widget_show(dialog_action_area);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area), 10);

	hbuttonbox = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox);
	gtk_box_pack_start(GTK_BOX(dialog_action_area), hbuttonbox,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox),
				  GTK_BUTTONBOX_END);

	button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
	gtk_widget_show(button);
	gtk_container_add(GTK_CONTAINER(hbuttonbox), button);
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);

	g_signal_connect(GTK_OBJECT(button), "clicked",
			   G_CALLBACK(about_sword_ok), NULL);
	return dialog_about_sword;

}
