/*
 * GnomeSword Bible Study Tool
 * gnomesword.c -
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
#include <ctype.h>
#include <time.h>

#include "gui/gnomesword.h"
#include "gui/main_window.h"
#include "gui/main_menu.h"
#include "gui/about_modules.h"
#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex.h"
#include "gui/dictlex_dialog.h"
#include "gui/percomm.h"
#include "gui/bookmarks.h"
#include "gui/studypad.h"
#include "gui/parallel_view.h"
#include "gui/dialog.h"
#include "gui/hints.h"
#include "gui/widgets.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"



/******************************************************************************
 * Name
 *  init_gnomesword
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void init_gnomesword(void)	
 *
 * Description
 *    sets up the interface
 *
 * Return value
 *   void
 */


void frontend_init(void)
{
	//gint number_of_pages = 0;
	guint number_of_pages = 0; 
	g_print("%s\n", "Initiating GnomeSword\n");
	
	hint.in_popup = FALSE;
	hint.use_hints = FALSE;
	
	settings.displaySearchResults = FALSE;
	settings.havethayer = check_for_module("Thayer");
	settings.havebdb = check_for_module("BDB");

	/*
	 *  setup shortcut bar 
	 */
	//gui_setup_shortcut_bar();
	gui_create_sidebar(widgets.epaned);

	/*
	 *  setup commentary gui support 
	 */
	if (settings.havecomm) {
		gui_setup_commentary(get_list(COMM_LIST));
		gui_setup_commentary_dialog(get_list(COMM_LIST));
	}

	/*
	 *  setup personal comments gui support 
	 */
	if (settings.havepercomm) {
		//gui_setup_percomm_dialog();
		if (!settings.use_percomm_dialog) {
			gui_setup_percomm(get_list(PERCOMM_LIST));
			gtk_box_pack_start(GTK_BOX
					   (widgets.vbox_percomm),
					   widgets.notebook_percomm,
					   TRUE, TRUE, 0);
		}
	}

	/*
	 *  setup Dict/Lex gui support 
	 */
	if (settings.havedict) {
		number_of_pages = gui_setup_dictlex(get_list(DICT_LIST));
		gui_setup_dictlex_dialog(get_list(DICT_LIST));
	}

	/*
	 *  setup general book gui support 
	 */
	if (settings.havebook) {
		gui_setup_gbs(get_list(GBS_LIST),number_of_pages);
		gui_setup_gbs_dialog(get_list(GBS_LIST));
	}
	
	/*
	 *  setup Bible text gui 
	 */
	if (settings.havebible) {
		gui_setup_text(get_list(TEXT_LIST));
		gui_setup_bibletext_dialog(get_list(TEXT_LIST));
	}
	
	/*
	 *  parallel stuff 
	 */
	if (settings.havebible) {
		gui_check_parallel_modules();
		gui_set_parallel_options_at_start();
		gui_create_parallel_page();
		gui_create_parallel_popup(get_list(TEXT_DESC_LIST));
	}

	gui_set_sidebar_porgram_start();
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void (void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */


void frontend_display(void)
{
	gint test;
	GS_DIALOG *info;
	GString *str;
	
	g_print("%s\n", "Displaying GnomeSword\n");
	gui_show_main_window();

	settings.addhistoryitem = FALSE;

	gui_change_verse(settings.currentverse);

	gui_change_module_and_key(settings.DictWindowModule, 
						settings.dictkey);
	
	if(!settings.havebible){
		settings.showtexts = FALSE;
		gui_show_hide_texts(FALSE);
		gtk_widget_set_sensitive(widgets.viewtexts_item, FALSE);
	}
	if(!settings.havecomm) {
		settings.showcomms = FALSE;
		gui_show_hide_comms(FALSE);
		gtk_widget_set_sensitive(widgets.viewcomms_item, FALSE);
	}
	if((!settings.havedict)&&(!settings.havebook)) {
		gui_show_hide_dicts(FALSE);
		gtk_widget_set_sensitive(widgets.button_dict_book, FALSE);
		gtk_widget_set_sensitive(widgets.viewdicts_item, FALSE);		
	}
	if((!settings.havedict)&&(settings.havebook)) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.workbook_lower),
					      1);
		gtk_widget_set_sensitive(widgets.button_dict_book, FALSE);
	}
	if((settings.havedict)&&(!settings.havebook)) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.workbook_lower),
					      0);
		gtk_widget_set_sensitive(widgets.button_dict_book, FALSE);
	}
	
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewtexts_item),
				       settings.showtexts);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewcomms_item),
				       settings.showcomms);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewdicts_item),
				       settings.showdicts);
	
	/* showing the devotional must come after the the app is shown or
	 *  it will mess up the shortcut bar display 
	 */
	/* 
	 * FIXME: maybe we need to move the devotional ? 
	 */
	if (settings.showdevotional) {
		gui_display_devotional_in_sidebar();
	}
 	gtk_window_move(GTK_WINDOW(widgets.app),settings.app_x,settings.app_y);
	if(settings.setup_canceled) {
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = "gtk-dialog-warning";
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			_("The Setup Druid was canceled."),
			_("Do you wish to set preferences now?"));
		info->label_top = str->str;
		info->yes = TRUE;
		info->no = TRUE;

		test = gui_alert_dialog(info);
		if (test == GS_YES) {
			gui_setup_preferences_dialog();
		}		
		g_free(info);
		g_string_free(str,TRUE);
		settings.setup_canceled = FALSE;
		xml_set_value("GnomeSword", "misc", "setup_canceled", "0");
	}
	g_print("done\n");
}


/******************************************************************************
 * Name
 *  shutdown_gnomesword
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void shutdown_gnomesword(void)
 *
 * Description
 *    do an orderly shutdown
 *
 * Return value
 *   void
 */

void shutdown_frontend(void)
{
	
	gui_save_bookmarks_treeview();
	/* if study pad file has changed since last save */
	if (widgets.studypad_dialog)
		gtk_widget_destroy(widgets.studypad_dialog);
	
	xml_save_settings_doc(settings.fnconfigure);
	xml_free_settings_doc();
	
	shutdown_list();

	/* free dir and file stuff */
	g_free(settings.gSwordDir);
	g_free(settings.shortcutbarDir);
	g_free(settings.fnconfigure);
	g_free(settings.swbmDir);

	if (settings.havebible) {
		gui_shutdown_text();
		gui_shutdown_bibletext_dialog();
	}

	if (settings.havebook) {
		gui_shutdown_gbs();
		gui_shutdown_gbs_dialog();
	}

	if (settings.havecomm) {
		gui_shutdown_commentary();
		gui_shutdown_commentary_dialog();
	}

	if (settings.havedict) {
		gui_shutdown_dictlex();
		gui_shutdown_dictlex_dialog();
	}

	if (settings.havepercomm)
		gui_shutdown_percomm();

	g_print("\nGnomeSword is shutdown\n");
}
