/*
 * GnomeSword Bible Study Tool
 * about_modules.c - Sword modules about dialog
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

#include "gui/about_modules.h"

/******************************************************************************
 * privat
 *****************************************************************************/

/******************************************************************************
 * Name
 *   about_modules_ok
 *
 * Description
 *   Callback if about module button is clicked.
 *
 * Return value
 *   void
 */

static void about_modules_ok(GtkButton * button, gpointer user_data)
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
 *   gui_create_about_modules
 *
 * Synopsis
 *   #include "about_modules.h"
 *
 *   void gui_create_about_modules(void);
 *
 * Description
 *   Create Sword modules about dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */
 
GtkWidget *gui_create_about_modules(void)
{
        GtkWidget *aboutmodules;
        GtkWidget *dialog_vbox8;
        GtkWidget *vbox25;
        GtkWidget *hbox21;
        GtkWidget *pixmap3;
        GtkWidget *label97;
        GtkWidget *pixmap4;
        GtkWidget *scrolledwindow30;
        GtkWidget *text;
        GtkWidget *dialog_action_area8;
        GtkWidget *btn_ok;

        aboutmodules = gnome_dialog_new("About Sword Modules", NULL);
        gtk_object_set_data(GTK_OBJECT(aboutmodules), "aboutmodules",
                            aboutmodules);
        gtk_container_set_border_width(GTK_CONTAINER(aboutmodules), 4);
        gtk_window_set_policy(GTK_WINDOW(aboutmodules), FALSE, FALSE,
                              FALSE);

        dialog_vbox8 = GNOME_DIALOG(aboutmodules)->vbox;
        gtk_object_set_data(GTK_OBJECT(aboutmodules), "dialog_vbox8",
                            dialog_vbox8);
        gtk_widget_show(dialog_vbox8);

        vbox25 = gtk_vbox_new(FALSE, 0);
        gtk_widget_ref(vbox25);
        gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "vbox25",
                                 vbox25,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(vbox25);
        gtk_box_pack_start(GTK_BOX(dialog_vbox8), vbox25, TRUE, TRUE,
                           0);

        hbox21 = gtk_hbox_new(FALSE, 0);
        gtk_widget_ref(hbox21);
        gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "hbox21",
                                 hbox21,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(hbox21);
        gtk_box_pack_start(GTK_BOX(vbox25), hbox21, TRUE, TRUE, 0);

        pixmap3 =
            gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
                                       "/sword3.xpm");
        gtk_widget_ref(pixmap3);
        gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "pixmap3",
                                 pixmap3,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(pixmap3);
        gtk_box_pack_start(GTK_BOX(hbox21), pixmap3, TRUE, TRUE, 0);

        label97 = gtk_label_new("The SWORD Project");
        gtk_widget_ref(label97);
        gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "label97",
                                 label97,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(label97);
        gtk_box_pack_start(GTK_BOX(hbox21), label97, FALSE, FALSE, 0);

        pixmap4 =
            gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR
                                       "GnomeSword.xpm");
        gtk_widget_ref(pixmap4);
        gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "pixmap4",
                                 pixmap4,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(pixmap4);
        gtk_box_pack_start(GTK_BOX(hbox21), pixmap4, TRUE, TRUE, 0);

        scrolledwindow30 = gtk_scrolled_window_new(NULL, NULL);
        gtk_widget_ref(scrolledwindow30);
        gtk_object_set_data_full(GTK_OBJECT(aboutmodules),
                                 "scrolledwindow30", scrolledwindow30,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(scrolledwindow30);
        gtk_box_pack_start(GTK_BOX(vbox25), scrolledwindow30, TRUE,
                           TRUE, 0);
        gtk_widget_set_usize(scrolledwindow30, 304, 183);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
                                       (scrolledwindow30),
                                       GTK_POLICY_NEVER,
                                       GTK_POLICY_ALWAYS);

        text = gtk_html_new();
        gtk_widget_ref(text);
        gtk_object_set_data_full(GTK_OBJECT(aboutmodules),
                                 "text", text,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(text);
        gtk_container_add(GTK_CONTAINER(scrolledwindow30),
                          text);

        dialog_action_area8 = GNOME_DIALOG(aboutmodules)->action_area;
        gtk_object_set_data(GTK_OBJECT(aboutmodules),
                            "dialog_action_area8", dialog_action_area8);
        gtk_widget_show(dialog_action_area8);
        gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area8),
                                  GTK_BUTTONBOX_END);
        gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area8),
                                   8);

        gnome_dialog_append_button(GNOME_DIALOG(aboutmodules),
                                   GNOME_STOCK_BUTTON_OK);
        btn_ok =
            g_list_last(GNOME_DIALOG(aboutmodules)->buttons)->data;
        gtk_widget_ref(btn_ok);
        gtk_object_set_data_full(GTK_OBJECT(aboutmodules),
                                 "btn_ok", btn_ok,
                                 (GtkDestroyNotify) gtk_widget_unref);
        gtk_widget_show(btn_ok);
        GTK_WIDGET_SET_FLAGS(btn_ok, GTK_CAN_DEFAULT);

        gtk_signal_connect(GTK_OBJECT(btn_ok), "clicked",
                           GTK_SIGNAL_FUNC(about_modules_ok),
                           NULL);

        return aboutmodules;
}
