/*
 * GnomeSword Bible Study Tool
 * setup_druid.c gnomesword fristrun druid
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
#  include <config.h>
#endif

#include <gnome.h>

#include "setup_druid.h"
#include "gs_gnomesword.h"
#include "setup.h"

/* FIXME: this is broken - causes a segfault after finish button is pressed */

/******************************************************************************
 * globals
 */
SETTINGS_DRUID widgets;

/******************************************************************************
 * globals to this file only 
 */
static GtkWidget *druid_dialog;

/******************************************************************************
 * Name
 *  on_finish_clicked
 *
 * Synopsis
 *   #include "setup_druid.h"
 *
 *   void on_finish_clicked(GnomeDialog * gnomedialog, gint arg1,
 *						gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
 
static void on_finish_clicked(GnomeDialog * gnomedialog, gint arg1,
						gpointer user_data)
{
	save_frist_run_settings(widgets);
	gnome_dialog_close(GNOME_DIALOG(druid_dialog));
}

/******************************************************************************
 * Name
 *  on_cancel_clicked
 *
 * Synopsis
 *   #include "setup_druid.h"
 *
 *   void on_cancel_clicked(GnomeDialog * gnomedialog, gint arg1,
 *						  gpointer user_data)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
 
static void on_cancel_clicked(GnomeDialog * gnomedialog, gint arg1,
						  gpointer user_data)
{
	gnome_dialog_close(GNOME_DIALOG(druid_dialog));
}

/******************************************************************************
 * Name
 *  gui_create_setup_druid
 *
 * Synopsis
 *   #include "setup_druid.h"
 *
 *   GtkWidget *gui_create_setup_druid(GList *biblemods, 
				GList *commmods, GList *dictmods)	
 *
 * Description
 *    
 *
 * Return value
 *   GtkWidget *
 */
 
GtkWidget *gui_create_setup_druid(GList *biblemods, 
				GList *commmods, GList *dictmods)
{
	
	gchar gtextmods[40];
	gchar gcommmods[40];
	gchar gdictmods[40];
	gchar *pathtomods;
	GtkWidget *dialog_vbox10;
	GtkWidget *druid1;
	GtkWidget *druidpagestart1;
	GdkColor druidpagestart1_bg_color = { 0, 6168, 6168, 28527 };
	GdkColor druidpagestart1_textbox_color =
	    { 0, 65535, 65535, 65535 };
	GdkColor druidpagestart1_logo_bg_color =
	    { 0, 6425, 6682, 29298 };
	GdkColor druidpagestart1_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *druidpagestandard3;
	GdkColor druidpagestandard3_bg_color =
	    { 0, 46260, 7967, 11308 };
	GdkColor druidpagestandard3_logo_bg_color =
	    { 0, 45232, 8481, 11822 };
	GdkColor druidpagestandard3_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *druid_vbox3;
	GtkWidget *vbox27;
	GtkWidget *label112;
	GtkWidget *label113;
	GtkWidget *label143;
	GtkWidget *label144;
	GtkWidget *label114;
	GtkWidget *label115;
	GtkWidget *label116;
	GtkWidget *label117;
	GtkWidget *label118;
	GtkWidget *label119;
	GtkWidget *label120;
	GtkWidget *label121;
	GtkWidget *druidpagestandard1;
	GdkColor druidpagestandard1_bg_color = { 0, 6168, 6168, 28527 };
	GdkColor druidpagestandard1_logo_bg_color =
	    { 0, 8481, 6425, 31354 };
	GdkColor druidpagestandard1_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *druid_vbox1;
	GtkWidget *vbox26;
	GSList *vbox26_group = NULL;
	GtkWidget *radiobutton2;
	GtkWidget *druidpagestandard2;
	GdkColor druidpagestandard2_bg_color =
	    { 0, 44186, 24978, 13159 };
	GdkColor druidpagestandard2_logo_bg_color =
	    { 0, 43193, 25239, 12874 };
	GdkColor druidpagestandard2_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *druid_vbox2;
	GtkWidget *table7;
	GtkWidget *label104;
	GtkWidget *combo1;
	GtkWidget *combo2;
	GtkWidget *combo3;
	GtkWidget *combo4;
	GtkWidget *label140;
	GtkWidget *label141;
	GtkWidget *label142;
	GtkWidget *combo10;
	GtkWidget *combo11;
	GtkWidget *combo12;
	GtkWidget *combo13;
	GtkWidget *combo14;
	GtkWidget *label139;
	GtkWidget *label138;
	GtkWidget *label107;
	GtkWidget *label106;
	GtkWidget *label105;
	GtkWidget *vbox34;
	GtkWidget *hbox27;
	GtkWidget *hbox28;
	GtkWidget *hbox29;
	GtkWidget *label111;
	GtkWidget *druidpagefinish1;
	GdkColor druidpagefinish1_bg_color = { 0, 6425, 6425, 28784 };
	GdkColor druidpagefinish1_textbox_color =
	    { 0, 65535, 65535, 65535 };
	GdkColor druidpagefinish1_logo_bg_color =
	    { 0, 6425, 3855, 27242 };
	GdkColor druidpagefinish1_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *dialog_action_area10;
	gchar *homedir, version[40];
	const char *ver;
	gchar gs_version[80];
	gchar gs_gnomesword[80];


	sprintf(gtextmods, "%d", g_list_length(biblemods));
	sprintf(gcommmods, "%d", g_list_length(commmods));
	sprintf(gdictmods, "%d", g_list_length(dictmods));

	pathtomods = get_path_to_mods();

	sprintf(gs_version, "%s-%s", _("Welcome To GnomeSword"),
		VERSION);
	sprintf(gs_gnomesword, "%s-%s", _("GnomeSword"), VERSION);

	druid_dialog = gnome_dialog_new(_("GnomeSword - Setup"), NULL);
	gtk_object_set_data(GTK_OBJECT(druid_dialog), "druid_dialog", druid_dialog);
	gtk_container_set_border_width(GTK_CONTAINER(druid_dialog), 2);
	gtk_window_set_policy(GTK_WINDOW(druid_dialog), FALSE, FALSE,
			      FALSE);
	//gnome_dialog_set_close (GNOME_DIALOG(druid_dialog), FALSE);
	
	
	dialog_vbox10 = GNOME_DIALOG(druid_dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(druid_dialog), "dialog_vbox10",
			    dialog_vbox10);
	gtk_widget_show(dialog_vbox10);

	druid1 = gnome_druid_new();
	gtk_widget_ref(druid1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "druid1", druid1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox10), druid1, TRUE, TRUE,
			   0);

	druidpagestart1 = gnome_druid_page_start_new();
	gtk_widget_ref(druidpagestart1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog),
				 "druidpagestart1", druidpagestart1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druidpagestart1);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagestart1));
	gnome_druid_set_page(GNOME_DRUID(druid1),
			     GNOME_DRUID_PAGE(druidpagestart1));
	gnome_druid_page_start_set_bg_color(GNOME_DRUID_PAGE_START
					    (druidpagestart1),
					    &druidpagestart1_bg_color);
	gnome_druid_page_start_set_textbox_color(GNOME_DRUID_PAGE_START
						 (druidpagestart1),
						 &druidpagestart1_textbox_color);
	gnome_druid_page_start_set_logo_bg_color(GNOME_DRUID_PAGE_START
						 (druidpagestart1),
						 &druidpagestart1_logo_bg_color);
	gnome_druid_page_start_set_title_color(GNOME_DRUID_PAGE_START
					       (druidpagestart1),
					       &druidpagestart1_title_color);
	gnome_druid_page_start_set_title(GNOME_DRUID_PAGE_START
					 (druidpagestart1), gs_version);
	gnome_druid_page_start_set_text(GNOME_DRUID_PAGE_START
					(druidpagestart1),
					_
					("This guide will take you through the initial setup of GnomeSword"));
	gnome_druid_page_start_set_logo(GNOME_DRUID_PAGE_START
					(druidpagestart1),
					gdk_imlib_load_image
					(PACKAGE_PIXMAPS_DIR
					 "/GnomeSword.xpm"));

	druidpagestandard3 =
	    gnome_druid_page_standard_new_with_vals("", NULL);
	gtk_widget_ref(druidpagestandard3);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog),
				 "druidpagestandard3",
				 druidpagestandard3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show_all(druidpagestandard3);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagestandard3));
	gnome_druid_page_standard_set_bg_color(GNOME_DRUID_PAGE_STANDARD
					       (druidpagestandard3),
					       &druidpagestandard3_bg_color);
	gnome_druid_page_standard_set_logo_bg_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard3),
	     &druidpagestandard3_logo_bg_color);
	gnome_druid_page_standard_set_title_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard3),
	     &druidpagestandard3_title_color);
	gnome_druid_page_standard_set_title(GNOME_DRUID_PAGE_STANDARD
					    (druidpagestandard3),
					    gs_gnomesword);
	gnome_druid_page_standard_set_logo(GNOME_DRUID_PAGE_STANDARD
					   (druidpagestandard3),
					   gdk_imlib_load_image
					   (PACKAGE_PIXMAPS_DIR
					    "/GnomeSword.xpm"));

	druid_vbox3 =
	    GNOME_DRUID_PAGE_STANDARD(druidpagestandard3)->vbox;
	gtk_widget_ref(druid_vbox3);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "druid_vbox3",
				 druid_vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox3);

	vbox27 = gtk_vbox_new(TRUE, 0);
	gtk_widget_ref(vbox27);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "vbox27", vbox27,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox27);
	gtk_box_pack_start(GTK_BOX(druid_vbox3), vbox27, TRUE, TRUE, 0);

	label112 = gtk_label_new(_("Your Home Directory:"));
	gtk_widget_ref(label112);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label112",
				 label112,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label112);
	gtk_box_pack_start(GTK_BOX(vbox27), label112, FALSE, FALSE, 0);

	if ((homedir = getenv("HOME")) == NULL) {
		g_error("$HOME is not set!");
	}


	label113 = gtk_label_new(_(homedir));
	gtk_widget_ref(label113);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label113",
				 label113,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label113);
	gtk_box_pack_start(GTK_BOX(vbox27), label113, FALSE, FALSE, 0);

	label143 = gtk_label_new(_("SWORD version"));
	gtk_widget_ref(label143);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label143",
				 label143,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label143);
	gtk_box_pack_start(GTK_BOX(vbox27), label143, FALSE, FALSE, 0);

	/* 
	 * get sword version 
	 */
	ver = get_sword_version();
	sprintf(version, "Sword-%s", ver);

	label144 = gtk_label_new(_(version));
	gtk_widget_ref(label144);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label144",
				 label144,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label144);
	gtk_box_pack_start(GTK_BOX(vbox27), label144, FALSE, FALSE, 0);

	label114 = gtk_label_new(_("Sword Directory:"));
	gtk_widget_ref(label114);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label114",
				 label114,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label114);
	gtk_box_pack_start(GTK_BOX(vbox27), label114, FALSE, FALSE, 0);

	label115 = gtk_label_new(_(pathtomods));
	gtk_widget_ref(label115);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label115",
				 label115,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label115);
	gtk_box_pack_start(GTK_BOX(vbox27), label115, FALSE, FALSE, 0);

	label116 = gtk_label_new(_("Number of Bible Text Modules:"));
	gtk_widget_ref(label116);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label116",
				 label116,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label116);
	gtk_box_pack_start(GTK_BOX(vbox27), label116, FALSE, FALSE, 0);

	label117 = gtk_label_new(_(gtextmods));
	gtk_widget_ref(label117);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label117",
				 label117,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label117);
	gtk_box_pack_start(GTK_BOX(vbox27), label117, FALSE, FALSE, 0);

	label118 = gtk_label_new(_("Number of Commentary Modules:"));
	gtk_widget_ref(label118);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label118",
				 label118,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label118);
	gtk_box_pack_start(GTK_BOX(vbox27), label118, FALSE, FALSE, 0);

	label119 = gtk_label_new(_(gcommmods));
	gtk_widget_ref(label119);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label119",
				 label119,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label119);
	gtk_box_pack_start(GTK_BOX(vbox27), label119, FALSE, FALSE, 0);

	label120 =
	    gtk_label_new(_("Number of Lexicon / Dictionary Modules"));
	gtk_widget_ref(label120);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label120",
				 label120,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label120);
	gtk_box_pack_start(GTK_BOX(vbox27), label120, FALSE, FALSE, 0);

	label121 = gtk_label_new(_(gdictmods));
	gtk_widget_ref(label121);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label121",
				 label121,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label121);
	gtk_box_pack_start(GTK_BOX(vbox27), label121, FALSE, FALSE, 0);

	druidpagestandard1 =
	    gnome_druid_page_standard_new_with_vals("", NULL);
	gtk_widget_ref(druidpagestandard1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog),
				 "druidpagestandard1",
				 druidpagestandard1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show_all(druidpagestandard1);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagestandard1));
	gnome_druid_page_standard_set_bg_color(GNOME_DRUID_PAGE_STANDARD
					       (druidpagestandard1),
					       &druidpagestandard1_bg_color);
	gnome_druid_page_standard_set_logo_bg_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard1),
	     &druidpagestandard1_logo_bg_color);
	gnome_druid_page_standard_set_title_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard1),
	     &druidpagestandard1_title_color);
	gnome_druid_page_standard_set_title(GNOME_DRUID_PAGE_STANDARD
					    (druidpagestandard1),
					    _
					    ("Do you want GnomeSword to open with"));
	gnome_druid_page_standard_set_logo(GNOME_DRUID_PAGE_STANDARD
					   (druidpagestandard1),
					   gdk_imlib_load_image
					   (PACKAGE_PIXMAPS_DIR
					    "/GnomeSword.xpm"));

	druid_vbox1 =
	    GNOME_DRUID_PAGE_STANDARD(druidpagestandard1)->vbox;
	gtk_widget_ref(druid_vbox1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "druid_vbox1",
				 druid_vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox1);

	vbox26 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox26);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "vbox26", vbox26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox26);
	gtk_box_pack_start(GTK_BOX(druid_vbox1), vbox26, TRUE, TRUE, 0);

	widgets.radiobutton1 =
	    gtk_radio_button_new_with_label(vbox26_group,
					    _
					    ("Settings you choose as default"));
	vbox26_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(widgets.radiobutton1));
	gtk_widget_ref(widgets.radiobutton1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.radiobutton1",
				 widgets.radiobutton1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.radiobutton1);
	gtk_box_pack_start(GTK_BOX(vbox26), widgets.radiobutton1, FALSE, FALSE,
			   0);

	radiobutton2 =
	    gtk_radio_button_new_with_label(vbox26_group,
					    _
					    ("Settings used the last time GnomeSword ran"));
	vbox26_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(radiobutton2));
	gtk_widget_ref(radiobutton2);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "radiobutton2",
				 radiobutton2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(radiobutton2);
	gtk_box_pack_start(GTK_BOX(vbox26), radiobutton2, FALSE, FALSE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2),
				     TRUE);

	druidpagestandard2 =
	    gnome_druid_page_standard_new_with_vals("", NULL);
	gtk_widget_ref(druidpagestandard2);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog),
				 "druidpagestandard2",
				 druidpagestandard2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show_all(druidpagestandard2);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagestandard2));
	gnome_druid_page_standard_set_bg_color(GNOME_DRUID_PAGE_STANDARD
					       (druidpagestandard2),
					       &druidpagestandard2_bg_color);
	gnome_druid_page_standard_set_logo_bg_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard2),
	     &druidpagestandard2_logo_bg_color);
	gnome_druid_page_standard_set_title_color
	    (GNOME_DRUID_PAGE_STANDARD(druidpagestandard2),
	     &druidpagestandard2_title_color);
	gnome_druid_page_standard_set_title(GNOME_DRUID_PAGE_STANDARD
					    (druidpagestandard2),
					    _
					    ("Settings to use on first program run"));
	gnome_druid_page_standard_set_logo(GNOME_DRUID_PAGE_STANDARD
					   (druidpagestandard2),
					   gdk_imlib_load_image
					   (PACKAGE_PIXMAPS_DIR
					    "/GnomeSword.xpm"));

	druid_vbox2 =
	    GNOME_DRUID_PAGE_STANDARD(druidpagestandard2)->vbox;
	gtk_widget_ref(druid_vbox2);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "druid_vbox2",
				 druid_vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox2);

	table7 = gtk_table_new(12, 2, FALSE);
	gtk_widget_ref(table7);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "table7", table7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table7);
	gtk_box_pack_start(GTK_BOX(druid_vbox2), table7, TRUE, TRUE, 0);

	label104 = gtk_label_new(_("Main Text Module"));
	gtk_widget_ref(label104);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label104",
				 label104,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label104);
	gtk_table_attach(GTK_TABLE(table7), label104, 0, 1, 0, 1,
			 (GtkAttachOptions) (0), (GtkAttachOptions) (0),
			 0, 0);

	combo1 = gtk_combo_new();
	gtk_widget_ref(combo1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo1", combo1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo1);
	gtk_table_attach(GTK_TABLE(table7), combo1, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo1), biblemods);

	widgets.combo_entry1 = GTK_COMBO(combo1)->entry;
	gtk_widget_ref(widgets.combo_entry1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry1",
				 widgets.combo_entry1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry1);

	combo2 = gtk_combo_new();
	gtk_widget_ref(combo2);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo2", combo2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo2);
	gtk_table_attach(GTK_TABLE(table7), combo2, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo2), biblemods);

	widgets.combo_entry2 = GTK_COMBO(combo2)->entry;
	gtk_widget_ref(widgets.combo_entry2);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry2",
				 widgets.combo_entry2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry2);

	combo3 = gtk_combo_new();
	gtk_widget_ref(combo3);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo3", combo3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo3);
	gtk_table_attach(GTK_TABLE(table7), combo3, 1, 2, 2, 3,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo3), biblemods);

	widgets.combo_entry3 = GTK_COMBO(combo3)->entry;
	gtk_widget_ref(widgets.combo_entry3);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry3",
				 widgets.combo_entry3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry3);

	combo4 = gtk_combo_new();
	gtk_widget_ref(combo4);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo4", combo4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo4);
	gtk_table_attach(GTK_TABLE(table7), combo4, 1, 2, 3, 4,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo4), biblemods);

	widgets.combo_entry4 = GTK_COMBO(combo4)->entry;
	gtk_widget_ref(widgets.combo_entry4);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry4",
				 widgets.combo_entry4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry4);

	label140 = gtk_label_new(_("Commentary"));
	gtk_widget_ref(label140);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label140",
				 label140,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label140);
	gtk_table_attach(GTK_TABLE(table7), label140, 0, 1, 6, 7,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label140), 0, 0.5);

	label141 = gtk_label_new(_("Dict/Lex"));
	gtk_widget_ref(label141);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label141",
				 label141,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label141);
	gtk_table_attach(GTK_TABLE(table7), label141, 0, 1, 7, 8,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label141), 0, 0.5);

	label142 = gtk_label_new(_("Personal"));
	gtk_widget_ref(label142);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label142",
				 label142,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label142);
	gtk_table_attach(GTK_TABLE(table7), label142, 0, 1, 8, 9,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label142), 0, 0.5);

	combo10 = gtk_combo_new();
	gtk_widget_ref(combo10);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo10",
				 combo10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo10);
	gtk_table_attach(GTK_TABLE(table7), combo10, 1, 2, 4, 5,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo10), biblemods);
	widgets.combo_entry10 = GTK_COMBO(combo10)->entry;
	gtk_widget_ref(widgets.combo_entry10);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry10",
				 widgets.combo_entry10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry10);

	combo11 = gtk_combo_new();
	gtk_widget_ref(combo11);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo11",
				 combo11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo11);
	gtk_table_attach(GTK_TABLE(table7), combo11, 1, 2, 5, 6,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo11), biblemods);
	widgets.combo_entry11 = GTK_COMBO(combo11)->entry;
	gtk_widget_ref(widgets.combo_entry11);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry11",
				 widgets.combo_entry11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry11);

	combo12 = gtk_combo_new();
	gtk_widget_ref(combo12);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo12",
				 combo12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo12);
	gtk_table_attach(GTK_TABLE(table7), combo12, 1, 2, 6, 7,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo12), commmods);
	widgets.combo_entry12 = GTK_COMBO(combo12)->entry;
	gtk_widget_ref(widgets.combo_entry12);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry12",
				 widgets.combo_entry12,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry12);

	combo13 = gtk_combo_new();
	gtk_widget_ref(combo13);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo13",
				 combo13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo13);
	gtk_table_attach(GTK_TABLE(table7), combo13, 1, 2, 7, 8,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo13), dictmods);
	widgets.combo_entry13 = GTK_COMBO(combo13)->entry;
	gtk_widget_ref(widgets.combo_entry13);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry13",
				 widgets.combo_entry13,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry13);

	combo14 = gtk_combo_new();
	gtk_widget_ref(combo14);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "combo14",
				 combo14,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo14);
	gtk_table_attach(GTK_TABLE(table7), combo14, 1, 2, 8, 9,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo14), commmods);
	widgets.combo_entry14 = GTK_COMBO(combo14)->entry;
	gtk_widget_ref(widgets.combo_entry14);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.combo_entry14",
				 widgets.combo_entry14,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry14);

	label139 = gtk_label_new(_("Interlinear 5"));
	gtk_widget_ref(label139);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label139",
				 label139,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label139);
	gtk_table_attach(GTK_TABLE(table7), label139, 0, 1, 5, 6,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label139), 0, 0.5);

	label138 = gtk_label_new(_("Interlinear 4"));
	gtk_widget_ref(label138);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label138",
				 label138,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label138);
	gtk_table_attach(GTK_TABLE(table7), label138, 0, 1, 4, 5,
			 (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label138), 0, 0.5);

	label107 = gtk_label_new(_("Interlinear 3"));
	gtk_widget_ref(label107);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label107",
				 label107,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label107);
	gtk_table_attach(GTK_TABLE(table7), label107, 0, 1, 3, 4,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label107), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label107), 7.45058e-09,
			       7.45058e-09);

	label106 = gtk_label_new(_("Interlinear 2"));
	gtk_widget_ref(label106);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label106",
				 label106,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label106);
	gtk_table_attach(GTK_TABLE(table7), label106, 0, 1, 2, 3,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label106), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label106), 7.45058e-09,
			       7.45058e-09);

	label105 = gtk_label_new(_("Interlinear 1"));
	gtk_widget_ref(label105);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label105",
				 label105,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label105);
	gtk_table_attach(GTK_TABLE(table7), label105, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label105), 1.2666e-07,
			       7.45058e-09);

	vbox34 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox34);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "vbox34", vbox34,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox34);
	gtk_box_pack_start(GTK_BOX(druid_vbox2), vbox34, TRUE, TRUE, 0);

	hbox27 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox27);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "hbox27", hbox27,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox27);
	gtk_box_pack_start(GTK_BOX(vbox34), hbox27, TRUE, TRUE, 0);

	widgets.checkbutton2 =
	    gtk_check_button_new_with_label(_("Use Verse Style"));
	gtk_widget_ref(widgets.checkbutton2);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.checkbutton2",
				 widgets.checkbutton2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton2);
	gtk_box_pack_start(GTK_BOX(hbox27), widgets.checkbutton2, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton2, 202, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.checkbutton2),
				     TRUE);

	widgets.checkbutton4 =
	    gtk_check_button_new_with_label(_("Show Bible Tabs"));
	gtk_widget_ref(widgets.checkbutton4);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.checkbutton4",
				 widgets.checkbutton4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton4);
	gtk_box_pack_start(GTK_BOX(hbox27), widgets.checkbutton4, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton4, 162, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.checkbutton4),
				     TRUE);

	hbox28 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox28);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "hbox28", hbox28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox28);
	gtk_box_pack_start(GTK_BOX(vbox34), hbox28, TRUE, TRUE, 0);

	widgets.checkbutton1 =
	    gtk_check_button_new_with_label(_
					    ("Auto Save Personal comments "));
	gtk_widget_ref(widgets.checkbutton1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.checkbutton1",
				 widgets.checkbutton1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton1);
	gtk_box_pack_start(GTK_BOX(hbox28), widgets.checkbutton1, FALSE, TRUE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton1, 202, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.checkbutton1),
				     TRUE);

	widgets.checkbutton5 =
	    gtk_check_button_new_with_label(_("Show Commentary Tabs"));
	gtk_widget_ref(widgets.checkbutton5);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.checkbutton5",
				 widgets.checkbutton5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton5);
	gtk_box_pack_start(GTK_BOX(hbox28), widgets.checkbutton5, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton5, 162, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.checkbutton5),
				     TRUE);

	hbox29 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox29);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "hbox29", hbox29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox29);
	gtk_box_pack_start(GTK_BOX(vbox34), hbox29, TRUE, TRUE, 0);

	widgets.checkbutton3 =
	    gtk_check_button_new_with_label(_("Show Interlinear Page"));
	gtk_widget_ref(widgets.checkbutton3);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.checkbutton3",
				 widgets.checkbutton3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton3);
	gtk_box_pack_start(GTK_BOX(hbox29), widgets.checkbutton3, FALSE, TRUE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton3, 202, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.checkbutton3),
				     TRUE);

	widgets.checkbutton6 =
	    gtk_check_button_new_with_label(_("Show Dict/Lex Tabs"));
	gtk_widget_ref(widgets.checkbutton6);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "widgets.checkbutton6",
				 widgets.checkbutton6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton6);
	gtk_box_pack_start(GTK_BOX(hbox29), widgets.checkbutton6, FALSE, FALSE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton6, 162, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.checkbutton6),
				     TRUE);


	label111 =
	    gtk_label_new(_
			  ("These setting will be the default settings if you chose to use default settings. "));
	gtk_widget_ref(label111);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog), "label111",
				 label111,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label111);
	gtk_box_pack_start(GTK_BOX(druid_vbox2), label111, FALSE, FALSE,
			   0);
	gtk_label_set_justify(GTK_LABEL(label111), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label111), TRUE);

	druidpagefinish1 = gnome_druid_page_finish_new();
	gtk_widget_ref(druidpagefinish1);
	gtk_object_set_data_full(GTK_OBJECT(druid_dialog),
				 "druidpagefinish1", druidpagefinish1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druidpagefinish1);
	gnome_druid_append_page(GNOME_DRUID(druid1),
				GNOME_DRUID_PAGE(druidpagefinish1));
	gnome_druid_page_finish_set_bg_color(GNOME_DRUID_PAGE_FINISH
					     (druidpagefinish1),
					     &druidpagefinish1_bg_color);
	gnome_druid_page_finish_set_textbox_color
	    (GNOME_DRUID_PAGE_FINISH(druidpagefinish1),
	     &druidpagefinish1_textbox_color);
	gnome_druid_page_finish_set_logo_bg_color
	    (GNOME_DRUID_PAGE_FINISH(druidpagefinish1),
	     &druidpagefinish1_logo_bg_color);
	gnome_druid_page_finish_set_title_color(GNOME_DRUID_PAGE_FINISH
						(druidpagefinish1),
						&druidpagefinish1_title_color);
	gnome_druid_page_finish_set_title(GNOME_DRUID_PAGE_FINISH
					  (druidpagefinish1),
					  gs_gnomesword);
	gnome_druid_page_finish_set_text(GNOME_DRUID_PAGE_FINISH
					 (druidpagefinish1),
					 _
					 ("            Thank-you for using GnomeSword.\nClick Finish to close this dialog and run GnomeSword."));
	gnome_druid_page_finish_set_logo(GNOME_DRUID_PAGE_FINISH
					 (druidpagefinish1),
					 gdk_imlib_load_image
					 (PACKAGE_PIXMAPS_DIR
					  "/GnomeSword.xpm"));

	dialog_action_area10 = GNOME_DIALOG(druid_dialog)->action_area;
	gtk_object_set_data(GTK_OBJECT(druid_dialog),
			    "dialog_action_area10",
			    dialog_action_area10);
	gtk_widget_show(dialog_action_area10);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area10),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area10),
				   8);

	
	gtk_signal_connect(GTK_OBJECT
			   (GNOME_DRUID_PAGE(druidpagefinish1)),
			   "finish", GTK_SIGNAL_FUNC(on_finish_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(druid1), "cancel",
			   GTK_SIGNAL_FUNC(on_cancel_clicked), NULL);


	g_list_free(biblemods);
	g_list_free(commmods);
	g_list_free(dictmods);
	g_free(pathtomods);

	return druid_dialog;
}

/******  end of file   ******/
