/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
 /*
    * GnomeSword Bible Study Tool
    * gs_abouts.c
    * -------------------
    * Tue Mar 13 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
  */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
 
#include "gs_abouts.h"
#include "support.h"




/*********************************************************************************
**********************************************************************************
 *callbacks About dialogs
**********************************************************************************
*********************************************************************************/

//-------------------------------------------------
void on_btnAboutModuleOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;

	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}


//------------------------------------------------
void on_btnAboutSwordOK_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg;
	
	dlg = gtk_widget_get_toplevel(GTK_WIDGET(button));
	gtk_widget_destroy(dlg);
}

/**********************************************************************
 * Name
 *   gui_create_about_gnomesword
 *
 * Synopsis
 *   #include "gs_abouts.h"
 *
 *   void gui_create_about_gnomesword(void);
 *
 * Description
 *   Create GnomeSword about dialog and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *
gui_create_about_gnomesword(void)
{
  const gchar *authors[] = {
    "Terry Biggs  tbiggs@users.sf.net",
    "Moses McKnight  m_mcknight@users.sf.net",
    "Jan Paul Schmidt japas@users.sf.net",
    "Victor Porton  porton@users.sf.net",
    "Stephen Binks steveb@icenet.com.au",
    "Carl Constantine duckwing@users.sf.net",
    "Jorge Chacon jgchacon@itelgua.com",
    "Jp Robinson  robinson@knightmail.com",
    NULL
  };
  GtkWidget *about;

  about = gnome_about_new("gnomesword", VERSION, "Copyright 2000-2002",
    authors, _("We would like to thank Troy Griffitts and all the other "
    "folks who have given us The SWORD Project."), PACKAGE_PIXMAPS_DIR"/about.png");
  gtk_object_set_data(GTK_OBJECT(about), "about", about);
  gtk_container_set_border_width(GTK_CONTAINER(about), 4);
  gtk_window_set_modal(GTK_WINDOW(about), TRUE);

  return about;
}

/**********************************************************************
 * Name
 *   gui_create_about_sword
 *
 * Synopsis
 *   #include "gs_abouts.h"
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
  GtkWidget *AboutSword;
  GtkWidget *dialog_vbox7;
  GtkWidget *vbox21;
  GtkWidget *pixmap1;
  GtkWidget *version_label;
  GtkWidget *scrolledwindow26;
  GtkWidget *txtAboutSword;
  GtkWidget *label95;
  GtkWidget *href1;
  GtkWidget *dialog_action_area7;
  GtkWidget *btnAboutSwordOK;
  gchar *about = _(
    "The SWORD Project is an effort to create an ever expanding "
    "software package for research and study of God and His Word. "
    "The SWORD Bible Framework allows easy manipulation of Bible "
    "texts, commentaries, lexicons, dictionaries, etc.  Many "
    "frontends are build using this framework. An installed "
    "module set may be shared between any frontend using the framework."
  );
  
  AboutSword = gnome_dialog_new(_("About The Sword Project"), NULL);
  gtk_object_set_data(GTK_OBJECT(AboutSword), "AboutSword",
    AboutSword);
  gtk_container_set_border_width(GTK_CONTAINER(AboutSword), 4);
  gtk_window_set_policy(GTK_WINDOW(AboutSword), FALSE, FALSE, FALSE);

  dialog_vbox7 = GNOME_DIALOG(AboutSword)->vbox;
  gtk_object_set_data(GTK_OBJECT(AboutSword), "dialog_vbox7",
    dialog_vbox7);
  gtk_widget_show(dialog_vbox7);

  vbox21 = gtk_vbox_new(FALSE, 0);
  gtk_widget_ref(vbox21);
  gtk_object_set_data_full(GTK_OBJECT(AboutSword), "vbox21", vbox21,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(vbox21);
  gtk_box_pack_start(GTK_BOX(dialog_vbox7), vbox21, TRUE, TRUE, 0);

  pixmap1 = gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR "/sword3.xpm");
  gtk_widget_ref(pixmap1);
  gtk_object_set_data_full(GTK_OBJECT(AboutSword), "pixmap1", pixmap1,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(pixmap1);
  gtk_box_pack_start(GTK_BOX(vbox21), pixmap1, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(pixmap1), 4, 4);

  version_label = gtk_label_new("Sword-");
  gtk_widget_ref(version_label);
  gtk_object_set_data_full(GTK_OBJECT(AboutSword), "version_label",
    version_label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(version_label);
  gtk_box_pack_start(GTK_BOX(vbox21), version_label, FALSE, FALSE, 0);

  scrolledwindow26 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_ref(scrolledwindow26);
  gtk_object_set_data_full(GTK_OBJECT(AboutSword), "scrolledwindow26",
    scrolledwindow26, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(scrolledwindow26);
  gtk_box_pack_start(GTK_BOX(vbox21), scrolledwindow26, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow26),
    GTK_POLICY_NEVER,
    GTK_POLICY_AUTOMATIC);

  txtAboutSword = gtk_text_new(NULL, NULL);
  gtk_widget_ref(txtAboutSword);
  gtk_object_set_data_full(GTK_OBJECT(AboutSword), "txtAboutSword",
    txtAboutSword, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(txtAboutSword);
  gtk_container_add(GTK_CONTAINER(scrolledwindow26), txtAboutSword);
  gtk_widget_set_usize(txtAboutSword, -2, 133);
  gtk_text_insert(GTK_TEXT(txtAboutSword), NULL, NULL, NULL, about,
    strlen(about));

  label95 = gtk_label_new(
    _("Modules can be downloaded from the Sword Project ")
  );
  gtk_widget_ref(label95);
  gtk_object_set_data_full(GTK_OBJECT(AboutSword), "label95", label95,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(label95);
  gtk_box_pack_start(GTK_BOX(vbox21), label95, FALSE, FALSE, 0);
  gtk_widget_set_usize(label95, -2, 24);

  href1 = gnome_href_new("www.crosswire.org", _("The Sword Project"));
  gtk_widget_ref(href1);
  gtk_object_set_data_full(GTK_OBJECT(AboutSword), "href1", href1,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(href1);
  gtk_box_pack_start(GTK_BOX(vbox21), href1, FALSE, FALSE, 0);
  
  dialog_action_area7 = GNOME_DIALOG(AboutSword)->action_area;
  gtk_object_set_data(GTK_OBJECT(AboutSword), "dialog_action_area7",
    dialog_action_area7);
  gtk_widget_show(dialog_action_area7);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area7),
    GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area7), 8);

  gnome_dialog_append_button(GNOME_DIALOG(AboutSword),
    GNOME_STOCK_BUTTON_OK);
  btnAboutSwordOK =
    g_list_last(GNOME_DIALOG(AboutSword)->buttons)->data;
  gtk_widget_ref(btnAboutSwordOK);
  gtk_object_set_data_full(GTK_OBJECT(AboutSword), "btnAboutSwordOK",
    btnAboutSwordOK, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(btnAboutSwordOK);
  GTK_WIDGET_SET_FLAGS(btnAboutSwordOK, GTK_CAN_DEFAULT);

  gtk_signal_connect(GTK_OBJECT(btnAboutSwordOK), "clicked",
    GTK_SIGNAL_FUNC(on_btnAboutSwordOK_clicked), NULL);

  return AboutSword;
}

/**********************************************************************
 * Name
 *   gui_create_about_modules
 *
 * Synopsis
 *   #include "gs_abouts.h"
 *
 *   void gui_create_about_modules(void);
 *
 * Description
 *   Create about dialog for Sword modules and return pointer on it.
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *
gui_create_about_modules(void)
{
  GtkWidget *aboutmodules;
  GtkWidget *dialog_vbox8;
  GtkWidget *vbox25;
  GtkWidget *hbox21;
  GtkWidget *pixmap3;
  GtkWidget *label97;
  GtkWidget *pixmap4;
  GtkWidget *scrolledwindow30;
  GtkWidget *textModAbout;
  GtkWidget *dialog_action_area8;
  GtkWidget *btnAboutModuleOK;

  aboutmodules = gnome_dialog_new("About Sword Modules", NULL);
  gtk_object_set_data(GTK_OBJECT(aboutmodules), "aboutmodules",
    aboutmodules);
  gtk_container_set_border_width(GTK_CONTAINER(aboutmodules), 4);
  gtk_window_set_policy(GTK_WINDOW(aboutmodules), FALSE, FALSE, FALSE);

  dialog_vbox8 = GNOME_DIALOG(aboutmodules)->vbox;
  gtk_object_set_data(GTK_OBJECT(aboutmodules), "dialog_vbox8", dialog_vbox8);
  gtk_widget_show(dialog_vbox8);

  vbox25 = gtk_vbox_new(FALSE, 0);
  gtk_widget_ref(vbox25);
  gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "vbox25", vbox25,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(vbox25);
  gtk_box_pack_start(GTK_BOX(dialog_vbox8), vbox25, TRUE, TRUE, 0);

  hbox21 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox21);
  gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "hbox21", hbox21,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(hbox21);
  gtk_box_pack_start(GTK_BOX(vbox25), hbox21, TRUE, TRUE, 0);

  pixmap3 = gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR "/sword3.xpm");
  gtk_widget_ref(pixmap3);
  gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "pixmap3", pixmap3,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(pixmap3);
  gtk_box_pack_start(GTK_BOX(hbox21), pixmap3, TRUE, TRUE, 0);

  label97 = gtk_label_new("The SWORD Project");
  gtk_widget_ref(label97);
  gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "label97", label97,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(label97);
  gtk_box_pack_start(GTK_BOX(hbox21), label97, FALSE, FALSE, 0);

  pixmap4 = gnome_pixmap_new_from_file(PACKAGE_PIXMAPS_DIR "GnomeSword.xpm");
  gtk_widget_ref(pixmap4);
  gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "pixmap4", pixmap4,
    (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(pixmap4);
  gtk_box_pack_start(GTK_BOX(hbox21), pixmap4, TRUE, TRUE, 0);

  scrolledwindow30 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_ref(scrolledwindow30);
  gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "scrolledwindow30",
    scrolledwindow30, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(scrolledwindow30);
  gtk_box_pack_start(GTK_BOX(vbox25), scrolledwindow30, TRUE, TRUE, 0);
  gtk_widget_set_usize(scrolledwindow30, 304, 183);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow30),
    GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  textModAbout = gtk_html_new();
  gtk_widget_ref(textModAbout);
  gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "textModAbout",
    textModAbout, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(textModAbout);
  gtk_container_add(GTK_CONTAINER(scrolledwindow30), textModAbout);

  dialog_action_area8 = GNOME_DIALOG(aboutmodules)->action_area;
  gtk_object_set_data(GTK_OBJECT(aboutmodules), "dialog_action_area8",
    dialog_action_area8);
  gtk_widget_show(dialog_action_area8);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area8), 
    GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area8), 8);

  gnome_dialog_append_button(GNOME_DIALOG(aboutmodules),
    GNOME_STOCK_BUTTON_OK);
  btnAboutModuleOK = g_list_last(GNOME_DIALOG(aboutmodules)->buttons)->data;
  gtk_widget_ref(btnAboutModuleOK);
  gtk_object_set_data_full(GTK_OBJECT(aboutmodules), "btnAboutModuleOK",
    btnAboutModuleOK, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show(btnAboutModuleOK);
  GTK_WIDGET_SET_FLAGS(btnAboutModuleOK, GTK_CAN_DEFAULT);

  gtk_signal_connect(GTK_OBJECT(btnAboutModuleOK), "clicked",
    GTK_SIGNAL_FUNC(on_btnAboutModuleOK_clicked), NULL);

  return aboutmodules;
}
