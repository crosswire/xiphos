/*
 * GnomeSword Bible Study Tool
 * about_sword.c - Sword about dialog
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>

#include "gui/about_sword.h"

#include "main/sword.h"

/******************************************************************************
 * privat
 *****************************************************************************/

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

static void about_sword_ok(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

/******************************************************************************
 * public
 *****************************************************************************/

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

GtkWidget *gui_create_about_sword(void)
{
	GtkWidget *dialog_about_sword;
	GtkWidget *dialog_vbox27;
	GtkWidget *vbox21;
	GtkWidget *pixmap1;
	GtkWidget *version_label;
	GtkWidget *scrolledwindow26;
	GtkWidget *label95;
	GtkWidget *textview1;
	GtkWidget *href1;
	GtkWidget *dialog_action_area27;
	GtkWidget *hbuttonbox6;
	GtkWidget *button_ok;
	const char *ver;
	gchar version[40];
	gchar *about =
	    _
	    ("The SWORD Project is an effort to create an ever expanding "
	     "software package for research and study of God and His Word. "
	     "The SWORD Bible Framework allows easy manipulation of Bible "
	     "texts, commentaries, lexicons, dictionaries, etc.  Many "
	     "frontends are built using this framework. An installed "
	     "module set may be shared between any frontend using the "
	     "framework.");

	/* 
	 * get sword version 
	 */
	ver = get_sword_version();
	sprintf(version, "<b>Sword-%s</b>", ver);
	
	dialog_about_sword = gtk_dialog_new();
	gtk_object_set_data(GTK_OBJECT(dialog_about_sword),
			    "dialog_about_sword", dialog_about_sword);
	gtk_window_set_title(GTK_WINDOW(dialog_about_sword),
			     _("About the SWORD Project"));
        gtk_window_set_resizable(GTK_WINDOW(dialog_about_sword), FALSE);

	dialog_vbox27 = GTK_DIALOG(dialog_about_sword)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_about_sword),
			    "dialog_vbox27", dialog_vbox27);
	gtk_widget_show(dialog_vbox27);

	vbox21 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox21);
	gtk_box_pack_start(GTK_BOX(dialog_vbox27), vbox21, TRUE, TRUE,
			   0);

	pixmap1 = gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
					     "/sword3.xpm");
	gtk_widget_show(pixmap1);
	gtk_box_pack_start(GTK_BOX(vbox21), pixmap1, FALSE, FALSE, 4);

	version_label = gtk_label_new(_("Sword Version"));
	gtk_widget_show(version_label);
	gtk_box_pack_start(GTK_BOX(vbox21), version_label, FALSE, FALSE,
			   0);

	gtk_label_set_markup(GTK_LABEL(version_label), version);
	
	scrolledwindow26 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow26);
	gtk_box_pack_start(GTK_BOX(vbox21), scrolledwindow26, TRUE,
			   TRUE, 4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow26),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
  	gtk_widget_set_size_request (scrolledwindow26, 160, 160);
  	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow26), 4);
	
	textview1 = gtk_text_view_new ();
	gtk_widget_show (textview1);
	gtk_container_add (GTK_CONTAINER (scrolledwindow26), textview1);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview1), GTK_WRAP_WORD);	
	gtk_text_view_set_editable(GTK_TEXT_VIEW(textview1),FALSE);
	gtk_text_buffer_set_text (gtk_text_view_get_buffer 
				(GTK_TEXT_VIEW (textview1)), about, -1);
	gtk_widget_set_sensitive(textview1,FALSE);
	
	label95 =
	    gtk_label_new(_
			  ("Modules can be downloaded from the Sword Project "));
	gtk_widget_show(label95);
	gtk_box_pack_start(GTK_BOX(vbox21), label95, FALSE, FALSE, 0);
	gtk_widget_set_usize(label95, -2, 24);

	href1 =
	    gnome_href_new("www.crosswire.org", _("The Sword Project"));
	gtk_widget_show(href1);
	gtk_box_pack_start(GTK_BOX(vbox21), href1, FALSE, FALSE, 0);

	dialog_action_area27 =
	    GTK_DIALOG(dialog_about_sword)->action_area;
	gtk_object_set_data(GTK_OBJECT(dialog_about_sword),
			    "dialog_action_area27",
			    dialog_action_area27);
	gtk_widget_show(dialog_action_area27);
	gtk_container_set_border_width(GTK_CONTAINER
				       (dialog_action_area27), 10);

	hbuttonbox6 = gtk_hbutton_box_new();
	gtk_widget_show(hbuttonbox6);
	gtk_box_pack_start(GTK_BOX(dialog_action_area27), hbuttonbox6,
			   TRUE, TRUE, 0);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbuttonbox6),
				  GTK_BUTTONBOX_END);

	button_ok = gtk_button_new_from_stock ("gtk-ok");
	gtk_widget_show(button_ok);
	gtk_container_add(GTK_CONTAINER(hbuttonbox6), button_ok);
	GTK_WIDGET_SET_FLAGS(button_ok, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(button_ok), "clicked",
			   G_CALLBACK(about_sword_ok), NULL);
	return dialog_about_sword;

}
