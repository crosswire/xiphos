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

#include "gui/setup_druid.h"

#include "main/sword.h"
#include "main/lists.h"
#include "main/settings.h"



/******************************************************************************
 * globals
 */
SETTINGS_DRUID widgets;

/******************************************************************************
 * globals to this file only 
 */
static GtkWidget *dialog_setup;

/******************************************************************************
 * Name
 *  save_frist_run_settings
 *
 * Synopsis
 *   #include "setup_druid.h"
 *
 *   void save_frist_run_settings(SETTINGS_DRUID widgets)	
 *
 * Description
 *   gets settings from druid widgets and stores them in 
 *   the settings struct then calls create_properties_from_setup
 *   to save them
 *
 * Return value
 *   void
 */

static void save_frist_run_settings(SETTINGS_DRUID widgets)
{
	strcpy(settings.MainWindowModule,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_text)));
	strcpy(settings.DictWindowModule,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_dict)));
	strcpy(settings.CommWindowModule,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_comm)));
	strcpy(settings.Interlinear1Module,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_int1)));
	strcpy(settings.Interlinear2Module,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_int2)));
	strcpy(settings.Interlinear3Module,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_int3)));
	strcpy(settings.Interlinear4Module,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_int4)));
	strcpy(settings.Interlinear5Module,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_int5)));
	strcpy(settings.personalcommentsmod,
	       gtk_entry_get_text(GTK_ENTRY
				  (widgets.combo_entry_personal)));
	strcpy(settings.BookWindowModule,
	       gtk_entry_get_text(GTK_ENTRY(widgets.combo_entry_gbs)));
	strcpy(settings.personalcommentsmod,
	       gtk_entry_get_text(GTK_ENTRY
				  (widgets.combo_entry_personal)));
	strcpy(settings.devotionalmod,
	       gtk_entry_get_text(GTK_ENTRY
				  (widgets.combo_entry_devotion)));
	strcpy(settings.lex_greek,
	       gtk_entry_get_text(GTK_ENTRY(widgets.entry_greek_lex)));
	strcpy(settings.lex_hebrew,
	       gtk_entry_get_text(GTK_ENTRY(widgets.entry_hebrew_lex)));


	settings.usedefault =
	    GTK_TOGGLE_BUTTON(widgets.radiobutton_use_default)->active;
	settings.text_tabs =
	    GTK_TOGGLE_BUTTON(widgets.checkbutton_text_tabs)->active;
	settings.comm_tabs =
	    GTK_TOGGLE_BUTTON(widgets.checkbutton_comm_tabs)->active;
	settings.dict_tabs =
	    GTK_TOGGLE_BUTTON(widgets.checkbutton_dict_tabs)->active;
	settings.versestyle =
	    GTK_TOGGLE_BUTTON(widgets.checkbutton_verse_style)->active;


	settings.showtexts =
	    GTK_TOGGLE_BUTTON(widgets.checkbutton_text_window)->active;
	settings.showcomms =
	    GTK_TOGGLE_BUTTON(widgets.checkbutton_upper_workbook)->
	    active;
	settings.showdicts =
	    GTK_TOGGLE_BUTTON(widgets.checkbutton_lower_workbook)->
	    active;


	/*
	   settings.autosavepersonalcomments =
	   GTK_TOGGLE_BUTTON(widgets.checkbutton1)->active;
	   settings.interlinearpage =
	   GTK_TOGGLE_BUTTON(widgets.checkbutton3)->active;
	 */

	create_properties_from_setup();
}


static void dialog_destroy(GtkObject * object, gpointer user_data)
{
	gtk_main_quit();
}

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
	//gnome_dialog_close(GNOME_DIALOG(druid_dialog));

	gtk_widget_destroy(dialog_setup);
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
	//gnome_dialog_close(GNOME_DIALOG(druid_dialog));

	gtk_widget_destroy(dialog_setup);

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

static GtkWidget *gui_create_setup_druid(GList * biblemods,
					 GList * commmods,
					 GList * dictmods,
					 GList * percommods,
					 GList * gbsmods,
					 GList * devotionmods)
{
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
	GtkWidget *radiobutton_last_run;
	GtkWidget *hseparator1;
	GtkWidget *vbox34;
	GtkWidget *hbox27;
	GtkWidget *hbox28;
	GtkWidget *hbox29;
	GtkWidget *hbox84;
	GtkWidget *druidpagestandard2;
	GdkColor druidpagestandard2_bg_color =
	    { 0, 44204, 24929, 13107 };
	GdkColor druidpagestandard2_logo_bg_color =
	    { 0, 43176, 25186, 12850 };
	GdkColor druidpagestandard2_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkWidget *druid_vbox2;
	GtkWidget *table7;
	GtkWidget *label104;
	GtkWidget *combo_text;
	GtkWidget *combo_int1;
	GtkWidget *combo_int2;
	GtkWidget *combo_int3;
	GtkWidget *label140;
	GtkWidget *combo_int4;
	GtkWidget *combo_int5;
	GtkWidget *combo_comm;
	GtkWidget *label139;
	GtkWidget *label138;
	GtkWidget *label107;
	GtkWidget *label106;
	GtkWidget *label105;
	GtkWidget *label240;
	GtkWidget *combo_gbs;
	GtkWidget *label142;
	GtkWidget *combo_personal;
	GtkWidget *label141;
	GtkWidget *combo_dict;
	GtkWidget *label250;
	GtkWidget *combo_greek_lex;
	GtkWidget *label251;
	GtkWidget *combo_hebrew_lex;
	GtkWidget *label252;
	GtkWidget *combo_devotion;
	GtkWidget *label111;
	GtkWidget *druidpagefinish1;
	GdkColor druidpagefinish1_bg_color = { 0, 6425, 6425, 28784 };
	GdkColor druidpagefinish1_textbox_color =
	    { 0, 65535, 65535, 65535 };
	GdkColor druidpagefinish1_logo_bg_color =
	    { 0, 6425, 3855, 27242 };
	GdkColor druidpagefinish1_title_color =
	    { 0, 65535, 65535, 65535 };
	GtkTooltips *tooltips;


	gchar *homedir, version[40];
	const char *ver;
	gchar gs_version[80];
	gchar gs_gnomesword[80];
	gchar gtextmods[40];
	gchar gcommmods[40];
	gchar gdictmods[40];
	gchar *pathtomods;


	tooltips = gtk_tooltips_new();
	sprintf(gtextmods, "%d", g_list_length(biblemods));
	sprintf(gcommmods, "%d", g_list_length(commmods));
	sprintf(gdictmods, "%d", g_list_length(dictmods));

	pathtomods = get_path_to_mods();

	sprintf(gs_version, "%s-%s", _("Welcome To GnomeSword"),
		VERSION);
	sprintf(gs_gnomesword, "%s-%s", _("GnomeSword"), VERSION);



	dialog_setup = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_object_set_data(GTK_OBJECT(dialog_setup), "dialog_setup",
			    dialog_setup);
	gtk_window_set_title(GTK_WINDOW(dialog_setup), _("Setup"));
	gtk_window_set_modal(GTK_WINDOW(dialog_setup), TRUE);

	druid1 = gnome_druid_new();
	gtk_widget_ref(druid1);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "druid1",
				 druid1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid1);
	gtk_container_add(GTK_CONTAINER(dialog_setup), druid1);

	druidpagestart1 = gnome_druid_page_start_new();
	gtk_widget_ref(druidpagestart1);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
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
					 (druidpagestart1),
					 _("Welcome To GnomeSword"));
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
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
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
					    _("GnomeSword"));
	gnome_druid_page_standard_set_logo(GNOME_DRUID_PAGE_STANDARD
					   (druidpagestandard3),
					   gdk_imlib_load_image
					   (PACKAGE_PIXMAPS_DIR
					    "/GnomeSword.xpm"));

	druid_vbox3 =
	    GNOME_DRUID_PAGE_STANDARD(druidpagestandard3)->vbox;
	gtk_widget_ref(druid_vbox3);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "druid_vbox3", druid_vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox3);

	vbox27 = gtk_vbox_new(TRUE, 0);
	gtk_widget_ref(vbox27);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "vbox27",
				 vbox27,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox27);
	gtk_box_pack_start(GTK_BOX(druid_vbox3), vbox27, TRUE, TRUE, 0);

	label112 = gtk_label_new(_("Your Home Directory:"));
	gtk_widget_ref(label112);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label112",
				 label112,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label112);
	gtk_box_pack_start(GTK_BOX(vbox27), label112, FALSE, FALSE, 0);

	/* 
	 * get home directory 
	 */

	if ((homedir = getenv("HOME")) == NULL) {
		g_error("$HOME is not set!");
	}


	label113 = gtk_label_new(homedir);
	gtk_widget_ref(label113);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label113",
				 label113,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label113);
	gtk_box_pack_start(GTK_BOX(vbox27), label113, FALSE, FALSE, 0);


	/* 
	 * get sword version 
	 */
	ver = get_sword_version();
	sprintf(version, "%s-%s", _("Sword"), ver);


	label143 = gtk_label_new(_("SWORD version"));
	gtk_widget_ref(label143);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label143",
				 label143,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label143);
	gtk_box_pack_start(GTK_BOX(vbox27), label143, FALSE, FALSE, 0);

	label144 = gtk_label_new(version);
	gtk_widget_ref(label144);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label144",
				 label144,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label144);
	gtk_box_pack_start(GTK_BOX(vbox27), label144, FALSE, FALSE, 0);

	label114 = gtk_label_new(_("Sword Directory:"));
	gtk_widget_ref(label114);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label114",
				 label114,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label114);
	gtk_box_pack_start(GTK_BOX(vbox27), label114, FALSE, FALSE, 0);

	label115 = gtk_label_new(pathtomods);
	gtk_widget_ref(label115);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label115",
				 label115,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label115);
	gtk_box_pack_start(GTK_BOX(vbox27), label115, FALSE, FALSE, 0);

	label116 = gtk_label_new(_("Number of Bible Text Modules:"));
	gtk_widget_ref(label116);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label116",
				 label116,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label116);
	gtk_box_pack_start(GTK_BOX(vbox27), label116, FALSE, FALSE, 0);

	label117 = gtk_label_new(gtextmods);
	gtk_widget_ref(label117);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label117",
				 label117,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label117);
	gtk_box_pack_start(GTK_BOX(vbox27), label117, FALSE, FALSE, 0);

	label118 = gtk_label_new(_("Number of Commentary Modules:"));
	gtk_widget_ref(label118);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label118",
				 label118,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label118);
	gtk_box_pack_start(GTK_BOX(vbox27), label118, FALSE, FALSE, 0);

	label119 = gtk_label_new(gcommmods);
	gtk_widget_ref(label119);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label119",
				 label119,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label119);
	gtk_box_pack_start(GTK_BOX(vbox27), label119, FALSE, FALSE, 0);

	label120 =
	    gtk_label_new(_("Number of Lexicon / Dictionary Modules"));
	gtk_widget_ref(label120);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label120",
				 label120,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label120);
	gtk_box_pack_start(GTK_BOX(vbox27), label120, FALSE, FALSE, 0);

	label121 = gtk_label_new(gdictmods);
	gtk_widget_ref(label121);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label121",
				 label121,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label121);
	gtk_box_pack_start(GTK_BOX(vbox27), label121, FALSE, FALSE, 0);

	druidpagestandard1 =
	    gnome_druid_page_standard_new_with_vals("", NULL);
	gtk_widget_ref(druidpagestandard1);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
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
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "druid_vbox1", druid_vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox1);

	vbox26 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox26);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "vbox26",
				 vbox26,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox26);
	gtk_box_pack_start(GTK_BOX(druid_vbox1), vbox26, TRUE, TRUE, 0);

	widgets.radiobutton_use_default =
	    gtk_radio_button_new_with_label(vbox26_group,
					    _
					    ("Settings you choose as default"));
	vbox26_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON
				   (widgets.radiobutton_use_default));
	gtk_widget_ref(widgets.radiobutton_use_default);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.radiobutton_use_default",
				 widgets.radiobutton_use_default,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.radiobutton_use_default);
	gtk_box_pack_start(GTK_BOX(vbox26),
			   widgets.radiobutton_use_default, FALSE,
			   FALSE, 0);

	radiobutton_last_run =
	    gtk_radio_button_new_with_label(vbox26_group,
					    _
					    ("Settings used the last time GnomeSword ran"));
	vbox26_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON
				   (radiobutton_last_run));
	gtk_widget_ref(radiobutton_last_run);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "radiobutton_last_run",
				 radiobutton_last_run,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(radiobutton_last_run);
	gtk_box_pack_start(GTK_BOX(vbox26), radiobutton_last_run, FALSE,
			   FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (radiobutton_last_run), TRUE);

	hseparator1 = gtk_hseparator_new();
	gtk_widget_ref(hseparator1);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "hseparator1", hseparator1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hseparator1);
	gtk_box_pack_start(GTK_BOX(vbox26), hseparator1, FALSE, TRUE,
			   0);

	vbox34 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox34);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "vbox34",
				 vbox34,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox34);
	gtk_box_pack_start(GTK_BOX(vbox26), vbox34, FALSE, TRUE, 0);

	hbox27 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox27);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "hbox27",
				 hbox27,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox27);
	gtk_box_pack_start(GTK_BOX(vbox34), hbox27, TRUE, TRUE, 0);

	widgets.checkbutton_verse_style =
	    gtk_check_button_new_with_label(_("Use Verse Style"));
	gtk_widget_ref(widgets.checkbutton_verse_style);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.checkbutton_verse_style",
				 widgets.checkbutton_verse_style,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton_verse_style);
	gtk_box_pack_start(GTK_BOX(hbox27),
			   widgets.checkbutton_verse_style, FALSE, TRUE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton_verse_style, 202, -2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (widgets.checkbutton_verse_style),
				     TRUE);

	widgets.checkbutton_text_tabs =
	    gtk_check_button_new_with_label(_("Show Bible Tabs"));
	gtk_widget_ref(widgets.checkbutton_text_tabs);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.checkbutton_text_tabs",
				 widgets.checkbutton_text_tabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton_text_tabs);
	gtk_box_pack_start(GTK_BOX(hbox27),
			   widgets.checkbutton_text_tabs, FALSE, TRUE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton_text_tabs, 162, -2);

	widgets.checkbutton_text_window =
	    gtk_check_button_new_with_label(_("Show Bible Window"));
	gtk_widget_ref(widgets.checkbutton_text_window);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.checkbutton_text_window",
				 widgets.checkbutton_text_window,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton_text_window);
	gtk_box_pack_start(GTK_BOX(hbox27),
			   widgets.checkbutton_text_window, FALSE, TRUE,
			   0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (widgets.checkbutton_text_window),
				     TRUE);

	hbox28 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox28);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "hbox28",
				 hbox28,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox28);
	gtk_box_pack_start(GTK_BOX(vbox34), hbox28, TRUE, TRUE, 0);

	widgets.checkbutton_comm_tabs =
	    gtk_check_button_new_with_label(_("Show Commentary Tabs"));
	gtk_widget_ref(widgets.checkbutton_comm_tabs);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.checkbutton_comm_tabs",
				 widgets.checkbutton_comm_tabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton_comm_tabs);
	gtk_box_pack_start(GTK_BOX(hbox28),
			   widgets.checkbutton_comm_tabs, FALSE, TRUE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton_comm_tabs, 162, -2);

	widgets.checkbutton_upper_workbook =
	    gtk_check_button_new_with_label(_("Show Upper Workbook"));
	gtk_widget_ref(widgets.checkbutton_upper_workbook);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.checkbutton_upper_workbook",
				 widgets.checkbutton_upper_workbook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton_upper_workbook);
	gtk_box_pack_start(GTK_BOX(hbox28),
			   widgets.checkbutton_upper_workbook, FALSE,
			   TRUE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (widgets.
				      checkbutton_upper_workbook),
				     TRUE);

	hbox29 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox29);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "hbox29",
				 hbox29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox29);
	gtk_box_pack_start(GTK_BOX(vbox34), hbox29, TRUE, TRUE, 0);

	widgets.checkbutton_dict_tabs =
	    gtk_check_button_new_with_label(_("Show Dict/Lex Tabs"));
	gtk_widget_ref(widgets.checkbutton_dict_tabs);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.checkbutton_dict_tabs",
				 widgets.checkbutton_dict_tabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton_dict_tabs);
	gtk_box_pack_start(GTK_BOX(hbox29),
			   widgets.checkbutton_dict_tabs, FALSE, TRUE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton_dict_tabs, 162, -2);

	widgets.checkbutton_lower_workbook =
	    gtk_check_button_new_with_label(_("Show Lower Workbook"));
	gtk_widget_ref(widgets.checkbutton_lower_workbook);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.checkbutton_lower_workbook",
				 widgets.checkbutton_lower_workbook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton_lower_workbook);
	gtk_box_pack_start(GTK_BOX(hbox29),
			   widgets.checkbutton_lower_workbook, FALSE,
			   TRUE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (widgets.
				      checkbutton_lower_workbook),
				     TRUE);

	hbox84 = gtk_hbox_new(TRUE, 0);
	gtk_widget_ref(hbox84);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "hbox84",
				 hbox84,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox84);
	gtk_box_pack_start(GTK_BOX(vbox34), hbox84, TRUE, TRUE, 0);

	widgets.checkbutton_book_tabs =
	    gtk_check_button_new_with_label(_("Show Book Tabs"));
	gtk_widget_ref(widgets.checkbutton_book_tabs);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.checkbutton_book_tabs",
				 widgets.checkbutton_book_tabs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.checkbutton_book_tabs);
	gtk_box_pack_start(GTK_BOX(hbox84),
			   widgets.checkbutton_book_tabs, FALSE, TRUE,
			   0);
	gtk_widget_set_usize(widgets.checkbutton_book_tabs, 162, -2);

	druidpagestandard2 =
	    gnome_druid_page_standard_new_with_vals("", NULL);
	gtk_widget_ref(druidpagestandard2);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
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
					    ("Choose default Sword modules"));
	gnome_druid_page_standard_set_logo(GNOME_DRUID_PAGE_STANDARD
					   (druidpagestandard2),
					   gdk_imlib_load_image
					   (PACKAGE_PIXMAPS_DIR
					    "/GnomeSword.xpm"));

	druid_vbox2 =
	    GNOME_DRUID_PAGE_STANDARD(druidpagestandard2)->vbox;
	gtk_widget_ref(druid_vbox2);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "druid_vbox2", druid_vbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(druid_vbox2);

	table7 = gtk_table_new(15, 2, FALSE);
	gtk_widget_ref(table7);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "table7",
				 table7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(table7);
	gtk_box_pack_start(GTK_BOX(druid_vbox2), table7, TRUE, TRUE, 0);

	label104 = gtk_label_new(_("Text Module       "));
	gtk_widget_ref(label104);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label104",
				 label104,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label104);
	gtk_table_attach(GTK_TABLE(table7), label104, 0, 1, 0, 1,
			 (GtkAttachOptions) (0),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label104), GTK_JUSTIFY_LEFT);

	combo_text = gtk_combo_new();
	gtk_widget_ref(combo_text);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_text",
				 combo_text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_text);
	gtk_table_attach(GTK_TABLE(table7), combo_text, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_text), biblemods);


	widgets.combo_entry_text = GTK_COMBO(combo_text)->entry;
	gtk_widget_ref(widgets.combo_entry_text);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_text",
				 widgets.combo_entry_text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_text);

	combo_int1 = gtk_combo_new();
	gtk_widget_ref(combo_int1);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_int1",
				 combo_int1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_int1);
	gtk_table_attach(GTK_TABLE(table7), combo_int1, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_int1), biblemods);

	widgets.combo_entry_int1 = GTK_COMBO(combo_int1)->entry;
	gtk_widget_ref(widgets.combo_entry_int1);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_int1",
				 widgets.combo_entry_int1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_int1);

	combo_int2 = gtk_combo_new();
	gtk_widget_ref(combo_int2);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_int2",
				 combo_int2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_int2);
	gtk_table_attach(GTK_TABLE(table7), combo_int2, 1, 2, 2, 3,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_int2), biblemods);

	widgets.combo_entry_int2 = GTK_COMBO(combo_int2)->entry;
	gtk_widget_ref(widgets.combo_entry_int2);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_int2",
				 widgets.combo_entry_int2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_int2);

	combo_int3 = gtk_combo_new();
	gtk_widget_ref(combo_int3);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_int3",
				 combo_int3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_int3);
	gtk_table_attach(GTK_TABLE(table7), combo_int3, 1, 2, 3, 4,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_int3), biblemods);

	widgets.combo_entry_int3 = GTK_COMBO(combo_int3)->entry;
	gtk_widget_ref(widgets.combo_entry_int3);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_int3",
				 widgets.combo_entry_int3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_int3);

	label140 = gtk_label_new(_("Commentary"));
	gtk_widget_ref(label140);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label140",
				 label140,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label140);
	gtk_table_attach(GTK_TABLE(table7), label140, 0, 1, 6, 7,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label140), 0, 0.5);

	combo_int4 = gtk_combo_new();
	gtk_widget_ref(combo_int4);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_int4",
				 combo_int4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_int4);
	gtk_table_attach(GTK_TABLE(table7), combo_int4, 1, 2, 4, 5,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_int4), biblemods);

	widgets.combo_entry_int4 = GTK_COMBO(combo_int4)->entry;
	gtk_widget_ref(widgets.combo_entry_int4);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_int4",
				 widgets.combo_entry_int4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_int4);

	combo_int5 = gtk_combo_new();
	gtk_widget_ref(combo_int5);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_int5",
				 combo_int5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_int5);
	gtk_table_attach(GTK_TABLE(table7), combo_int5, 1, 2, 5, 6,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_int5), biblemods);

	widgets.combo_entry_int5 = GTK_COMBO(combo_int5)->entry;
	gtk_widget_ref(widgets.combo_entry_int5);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_int5",
				 widgets.combo_entry_int5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_int5);

	combo_comm = gtk_combo_new();
	gtk_widget_ref(combo_comm);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_comm",
				 combo_comm,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_comm);
	gtk_table_attach(GTK_TABLE(table7), combo_comm, 1, 2, 6, 7,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_comm), commmods);

	widgets.combo_entry_comm = GTK_COMBO(combo_comm)->entry;
	gtk_widget_ref(widgets.combo_entry_comm);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_comm",
				 widgets.combo_entry_comm,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_comm);

	label139 = gtk_label_new(_("Interlinear 5"));
	gtk_widget_ref(label139);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label139",
				 label139,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label139);
	gtk_table_attach(GTK_TABLE(table7), label139, 0, 1, 5, 6,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label139), 0, 0.5);

	label138 = gtk_label_new(_("Interlinear 4"));
	gtk_widget_ref(label138);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label138",
				 label138,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label138);
	gtk_table_attach(GTK_TABLE(table7), label138, 0, 1, 4, 5,
			 (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label138), 0, 0.5);

	label107 = gtk_label_new(_("Interlinear 3"));
	gtk_widget_ref(label107);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label107",
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
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label106",
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
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label105",
				 label105,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label105);
	gtk_table_attach(GTK_TABLE(table7), label105, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (GTK_SHRINK), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label105), 1.2666e-07,
			       7.45058e-09);

	label240 = gtk_label_new(_("Generic Book"));
	gtk_widget_ref(label240);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label240",
				 label240,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label240);
	gtk_table_attach(GTK_TABLE(table7), label240, 0, 1, 8, 9,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label240), 0, 0.5);

	combo_gbs = gtk_combo_new();
	gtk_widget_ref(combo_gbs);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_gbs",
				 combo_gbs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_gbs);
	gtk_table_attach(GTK_TABLE(table7), combo_gbs, 1, 2, 8, 9,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_gbs), gbsmods);

	widgets.combo_entry_gbs = GTK_COMBO(combo_gbs)->entry;
	gtk_widget_ref(widgets.combo_entry_gbs);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_gbs",
				 widgets.combo_entry_gbs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_gbs);

	label142 = gtk_label_new(_("Personal"));
	gtk_widget_ref(label142);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label142",
				 label142,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label142);
	gtk_table_attach(GTK_TABLE(table7), label142, 0, 1, 7, 8,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label142), 0, 0.5);

	combo_personal = gtk_combo_new();
	gtk_widget_ref(combo_personal);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "combo_personal", combo_personal,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_personal);
	gtk_table_attach(GTK_TABLE(table7), combo_personal, 1, 2, 7, 8,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_personal),
				      percommods);

	widgets.combo_entry_personal = GTK_COMBO(combo_personal)->entry;
	gtk_widget_ref(widgets.combo_entry_personal);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_personal",
				 widgets.combo_entry_personal,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_personal);

	label141 = gtk_label_new(_("Dict/Lex"));
	gtk_widget_ref(label141);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label141",
				 label141,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label141);
	gtk_table_attach(GTK_TABLE(table7), label141, 0, 1, 9, 10,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label141), 0, 0.5);

	combo_dict = gtk_combo_new();
	gtk_widget_ref(combo_dict);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "combo_dict",
				 combo_dict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_dict);
	gtk_table_attach(GTK_TABLE(table7), combo_dict, 1, 2, 9, 10,
			 (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_dict), dictmods);

	widgets.combo_entry_dict = GTK_COMBO(combo_dict)->entry;
	gtk_widget_ref(widgets.combo_entry_dict);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_dict",
				 widgets.combo_entry_dict,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_dict);

	label250 = gtk_label_new(_("Greek Lexicon"));
	gtk_widget_ref(label250);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label250",
				 label250,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label250);
	gtk_table_attach(GTK_TABLE(table7), label250, 0, 1, 10, 11,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label250), 0, 0.5);

	combo_greek_lex = gtk_combo_new();
	gtk_widget_ref(combo_greek_lex);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "combo_greek_lex", combo_greek_lex,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_greek_lex);
	gtk_table_attach(GTK_TABLE(table7), combo_greek_lex, 1, 2, 10,
			 11, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_greek_lex),
				      dictmods);

	widgets.entry_greek_lex = GTK_COMBO(combo_greek_lex)->entry;
	gtk_widget_ref(widgets.entry_greek_lex);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.entry_greek_lex",
				 widgets.entry_greek_lex,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.entry_greek_lex);
	gtk_tooltips_set_tip(tooltips, widgets.entry_greek_lex,
			     _
			     ("Which Greek Lexicon to display in Dict/Lex Window when a link or word is clicked"),
			     NULL);

	label251 = gtk_label_new(_("Hebrew Lexicon"));
	gtk_widget_ref(label251);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label251",
				 label251,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label251);
	gtk_table_attach(GTK_TABLE(table7), label251, 0, 1, 11, 12,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label251), 0, 0.5);

	combo_hebrew_lex = gtk_combo_new();
	gtk_widget_ref(combo_hebrew_lex);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "combo_hebrew_lex", combo_hebrew_lex,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_hebrew_lex);
	gtk_table_attach(GTK_TABLE(table7), combo_hebrew_lex, 1, 2, 11,
			 12, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_hebrew_lex),
				      dictmods);


	widgets.entry_hebrew_lex = GTK_COMBO(combo_hebrew_lex)->entry;
	gtk_widget_ref(widgets.entry_hebrew_lex);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.entry_hebrew_lex",
				 widgets.entry_hebrew_lex,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.entry_hebrew_lex);
	gtk_tooltips_set_tip(tooltips, widgets.entry_hebrew_lex,
			     _
			     ("Which Hebrew Lexicon to display in Dict/Lex Window when a link or word is clicked"),
			     NULL);

	label252 = gtk_label_new(_("Daily Devotional"));
	gtk_widget_ref(label252);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label252",
				 label252,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label252);
	gtk_table_attach(GTK_TABLE(table7), label252, 0, 1, 12, 13,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label252), 0, 0.5);

	combo_devotion = gtk_combo_new();
	gtk_widget_ref(combo_devotion);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "combo_devotion", combo_devotion,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo_devotion);
	gtk_table_attach(GTK_TABLE(table7), combo_devotion, 1, 2, 12,
			 13, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);

	gtk_combo_set_popdown_strings(GTK_COMBO(combo_devotion),
				      devotionmods);

	widgets.combo_entry_devotion = GTK_COMBO(combo_devotion)->entry;
	gtk_widget_ref(widgets.combo_entry_devotion);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
				 "widgets.combo_entry_devotion",
				 widgets.combo_entry_devotion,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(widgets.combo_entry_devotion);

	label111 =
	    gtk_label_new(_
			  ("These setting will be the default settings if you chose to use default settings. "));
	gtk_widget_ref(label111);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup), "label111",
				 label111,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label111);
	gtk_box_pack_start(GTK_BOX(druid_vbox2), label111, FALSE, FALSE,
			   0);
	gtk_label_set_justify(GTK_LABEL(label111), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label111), TRUE);

	druidpagefinish1 = gnome_druid_page_finish_new();
	gtk_widget_ref(druidpagefinish1);
	gtk_object_set_data_full(GTK_OBJECT(dialog_setup),
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
					  _("GnomeSword"));
	gnome_druid_page_finish_set_text(GNOME_DRUID_PAGE_FINISH
					 (druidpagefinish1),
					 _
					 ("            Thank-you for using GnomeSword. \nClick Finish to close this dialog and run GnomeSword."));
	gnome_druid_page_finish_set_logo(GNOME_DRUID_PAGE_FINISH
					 (druidpagefinish1),
					 gdk_imlib_load_image
					 (PACKAGE_PIXMAPS_DIR
					  "/GnomeSword.xpm"));

	gtk_object_set_data(GTK_OBJECT(dialog_setup), "tooltips",
			    tooltips);


	gtk_signal_connect(GTK_OBJECT(dialog_setup), "destroy",
			   GTK_SIGNAL_FUNC(dialog_destroy), NULL);
	gtk_signal_connect(GTK_OBJECT
			   (GNOME_DRUID_PAGE(druidpagefinish1)),
			   "finish", GTK_SIGNAL_FUNC(on_finish_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(druid1), "cancel",
			   GTK_SIGNAL_FUNC(on_cancel_clicked), NULL);


	g_free(pathtomods);
	return dialog_setup;

}

/******************************************************************************
 * Name
 *  first_run
 *
 * Synopsis
 *   #include "setup.h"
 *
 *   void first_run(void)
 *
 * Description
 *    loads module list and calls 
 *    gui_create_setup_druid(biblemods, commmods, dictmods, 
 *				     percommmods, 
 *				     gbsmods,
 *				     devotionmods)
 *
 * Return value
 *   void
 */

gint gui_first_run(void)
{
	GtkWidget *dlg;
	GList *biblemods = NULL;
	GList *commmods = NULL;
	GList *dictmods = NULL;
	GList *percommmods = NULL;
	GList *gbsmods = NULL;
	GList *devotionmods = NULL;

	biblemods = get_list(TEXT_LIST);
	commmods = get_list(COMM_LIST);
	dictmods = get_list(DICT_LIST);
	percommmods = get_list(PERCOMM_LIST);
	gbsmods = get_list(GBS_LIST);
	devotionmods = get_list(DEVOTION_LIST);

	dlg = gui_create_setup_druid(biblemods,
				     commmods,
				     dictmods, 
				     percommmods, 
				     gbsmods,
				     devotionmods);

	/*
	 * hold util we are done 
	 */	
	gtk_widget_show(dlg);
	gtk_main();
	
	return 1;
}

/******  end of file   ******/
