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
#include <gtkhtml/gtkhtml.h>

#include "gui/about_sword.h"

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

static void about_sword_ok(GtkButton * button,
                                   gpointer user_data)
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
        GtkWidget *about_sword;
        GtkWidget *dialog_vbox7;
        GtkWidget *vbox21;
        GtkWidget *pixmap1;
        GtkWidget *version_label;
        GtkWidget *scrolledwindow26;
        GtkWidget *txtabout_sword;
        GtkWidget *label95;
        GtkWidget *href1;
        GtkWidget *dialog_action_area7;
        GtkWidget *button_ok;
        gchar *about = _(
             "The SWORD Project is an effort to create an ever expanding "
             "software package for research and study of God and His Word. "
             "The SWORD Bible Framework allows easy manipulation of Bible "
             "texts, commentaries, lexicons, dictionaries, etc.  Many "
             "frontends are build using this framework. An installed "
             "module set may be shared between any frontend using the"
             "framework."
        );

        about_sword =
            gnome_dialog_new(_("About The Sword Project"), NULL);
        gtk_object_set_data(GTK_OBJECT(about_sword), "about_sword",
                            about_sword);
        gtk_container_set_border_width(GTK_CONTAINER(about_sword), 4);
        gtk_window_set_policy(GTK_WINDOW(about_sword), FALSE, FALSE,
                              FALSE);

        dialog_vbox7 = GNOME_DIALOG(about_sword)->vbox;
        gtk_object_set_data(GTK_OBJECT(about_sword), "dialog_vbox7",
                            dialog_vbox7);
        gtk_widget_show(dialog_vbox7);

        vbox21 = gtk_vbox_new(FALSE, 0);
        gtk_widget_ref(vbox21);
        gtk_object_set_data_full(GTK_OBJECT(about_sword), "vbox21",
                                 vbox21,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(vbox21);
        gtk_box_pack_start(GTK_BOX(dialog_vbox7), vbox21, TRUE, TRUE,
                           0);

        pixmap1 =
            gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
                                       "/sword3.xpm");
        gtk_widget_ref(pixmap1);
        gtk_object_set_data_full(GTK_OBJECT(about_sword), "pixmap1",
                                 pixmap1,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(pixmap1);
        gtk_box_pack_start(GTK_BOX(vbox21), pixmap1, FALSE, FALSE, 0);

        version_label = gtk_label_new("Sword-");
        gtk_widget_ref(version_label);
        gtk_object_set_data_full(GTK_OBJECT(about_sword),
                                 "version_label", version_label,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(version_label);
        gtk_box_pack_start(GTK_BOX(vbox21), version_label, FALSE, FALSE,
                           0);

        scrolledwindow26 = gtk_scrolled_window_new(NULL, NULL);
        gtk_widget_ref(scrolledwindow26);
        gtk_object_set_data_full(GTK_OBJECT(about_sword),
                                 "scrolledwindow26", scrolledwindow26,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(scrolledwindow26);
        gtk_box_pack_start(GTK_BOX(vbox21), scrolledwindow26, TRUE,
                           TRUE, 0);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
                                       (scrolledwindow26),
                                       GTK_POLICY_NEVER,
                                       GTK_POLICY_AUTOMATIC);

        txtabout_sword = gtk_text_new(NULL, NULL);
        gtk_widget_ref(txtabout_sword);
        gtk_object_set_data_full(GTK_OBJECT(about_sword),
                                 "txtabout_sword", txtabout_sword,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(txtabout_sword);
        gtk_container_add(GTK_CONTAINER(scrolledwindow26),
                          txtabout_sword);
        gtk_widget_set_usize(txtabout_sword, -2, 133);
	gtk_text_set_word_wrap(GTK_TEXT(txtabout_sword), TRUE);	
        gtk_text_insert(GTK_TEXT(txtabout_sword), NULL, NULL, NULL,
                        about, strlen(about));
        
        label95 =
            gtk_label_new(_(
                    "Modules can be downloaded from the Sword Project ")
            );
        gtk_widget_ref(label95);
        gtk_object_set_data_full(GTK_OBJECT(about_sword), "label95",
                                 label95,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(label95);
        gtk_box_pack_start(GTK_BOX(vbox21), label95, FALSE, FALSE, 0);
        gtk_widget_set_usize(label95, -2, 24);

        href1 =
            gnome_href_new("www.crosswire.org", _("The Sword Project"));
        gtk_widget_ref(href1);
        gtk_object_set_data_full(GTK_OBJECT(about_sword), "href1",
                                 href1,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(href1);
        gtk_box_pack_start(GTK_BOX(vbox21), href1, FALSE, FALSE, 0);

        dialog_action_area7 = GNOME_DIALOG(about_sword)->action_area;
        gtk_object_set_data(GTK_OBJECT(about_sword),
                            "dialog_action_area7", dialog_action_area7);
        gtk_widget_show(dialog_action_area7);
        gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area7),
                                  GTK_BUTTONBOX_END);
        gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area7),
                                   8);

        gnome_dialog_append_button(GNOME_DIALOG(about_sword),
                                   GNOME_STOCK_BUTTON_OK);
        button_ok =
            g_list_last(GNOME_DIALOG(about_sword)->buttons)->data;
        gtk_widget_ref(button_ok);
        gtk_object_set_data_full(GTK_OBJECT(about_sword),
                                 "button_ok", button_ok,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(button_ok);
        GTK_WIDGET_SET_FLAGS(button_ok, GTK_CAN_DEFAULT);

        gtk_signal_connect(GTK_OBJECT(button_ok), "clicked",
                           GTK_SIGNAL_FUNC
                           (about_sword_ok), NULL);

        return about_sword;
}
