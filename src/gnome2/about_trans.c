/*
 * Xiphos Bible Study Tool
 * about_trans.c - About Translation
 *
 * Copyright (C) 2008-2017 Xiphos Developer Team
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

#include "gui/about_trans.h"
#include "gui/utilities.h"
#include "main/settings.h"
#include "main/sword.h"

/******************************************************************************
 * Name
 *   about_trans_ok
 *
 * Description
 *   Callback if about sword button is clicked.
 *
 * Return value
 *   void
 */

static void about_trans_ok(GtkButton *button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

/******************************************************************************
 * Name
 *   gui_create_about_trans
 *
 * Synopsis
 *   #include "about_trans.h"
 *
 *   void gui_create_about_trans(void);
 *
 * Description
 *   Create About Translation dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_about_trans(void)
{
	GtkWidget *dialog_about_trans;
	GtkWidget *dialog_vbox;
	GtkWidget *vbox;
	GtkWidget *pixmap;
	GtkWidget *scrolledwindow;
	GtkWidget *label;
	GtkWidget *textview;
	GtkWidget *href;
	GtkWidget *dialog_action_area;
	GtkWidget *hbuttonbox;
	GtkWidget *button;
	gchar *about =
	    _("Do you like using Xiphos to study the Bible? "
	      "Would you like to see its user interface in your native language? "
	      "You could translate Xiphos! "
	      "\n\n"
	      "We are always looking for contributions of new "
	      "translations of Xiphos into other languages. "
	      "If you are able to translate for us, please see the link "
	      "below, contact us, and get involved with our efforts. Your help will "
	      "be much appreciated!");

	dialog_about_trans = gtk_dialog_new();
	g_object_set_data(G_OBJECT(dialog_about_trans),
			  "dialog_about_trans", dialog_about_trans);
	gtk_window_set_title(GTK_WINDOW(dialog_about_trans),
			     _("About Xiphos Translation"));
	set_window_icon(GTK_WINDOW(dialog_about_trans));
	gtk_window_set_resizable(GTK_WINDOW(dialog_about_trans), FALSE);

	dialog_vbox =
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_about_trans));
	g_object_set_data(G_OBJECT(dialog_about_trans), "dialog_vbox",
			  dialog_vbox);
	gtk_widget_show(dialog_vbox);

	UI_VBOX(vbox, FALSE, 0);
	gtk_widget_show(vbox);
	gtk_box_pack_start(GTK_BOX(dialog_vbox), vbox, TRUE, TRUE, 0);

	pixmap = pixmap_finder("logo.png");
	gtk_widget_show(pixmap);
	gtk_box_pack_start(GTK_BOX(vbox), pixmap, FALSE, FALSE, 4);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolledwindow, 400, 150);
	gtk_container_set_border_width(GTK_CONTAINER(scrolledwindow), 4);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow,
					    settings.shadow_type);

	textview = gtk_text_view_new();
	gtk_widget_show(textview);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), textview);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview),
				    GTK_WRAP_WORD);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview)), about, -1);
	gtk_widget_set_sensitive(textview, FALSE);

	label = gtk_label_new(_("See TRANSLATION-HOWTO in Xiphos source"));
	gtk_widget_show(label);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	gtk_widget_set_size_request(label, -2, 24);

	href =
	    gtk_link_button_new_with_label("http://xiphos.org/development/", _("Xiphos development"));
	gtk_widget_show(href);
	gtk_box_pack_start(GTK_BOX(vbox), href, FALSE, FALSE, 0);

	dialog_action_area =
#if GTK_CHECK_VERSION(3, 12, 0)
	    gtk_dialog_get_content_area(GTK_DIALOG(dialog_about_trans));
#else
	    gtk_dialog_get_action_area(GTK_DIALOG(dialog_about_trans));
#endif
	g_object_set_data(G_OBJECT(dialog_about_trans),
			  "dialog_action_area", dialog_action_area);
	gtk_widget_show(dialog_action_area);
	gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area), 10);

#ifdef USE_GTK_3
	hbuttonbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
#else
	hbuttonbox = gtk_hbutton_box_new();
#endif
	gtk_widget_show(hbuttonbox);
	gtk_box_pack_start(GTK_BOX(dialog_action_area), hbuttonbox,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox),
				  GTK_BUTTONBOX_END);
	button =
#if GTK_CHECK_VERSION(3, 10, 0)
	    gtk_button_new_with_mnemonic(_("Close"));
#else
	    gtk_button_new_from_stock(GTK_STOCK_CLOSE);
#endif
	gtk_widget_show(button);
	gtk_container_add(GTK_CONTAINER(hbuttonbox), button);
#if GTK_CHECK_VERSION(2, 18, 0)
	gtk_widget_set_can_default(button, TRUE);
#elif defined(USE_GTK_3)
	gtk_widget_set_can_default(button, 1);
#else
	GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
#endif
	g_signal_connect(G_OBJECT(button), "clicked",
			 G_CALLBACK(about_trans_ok), NULL);
	return dialog_about_trans;
}
