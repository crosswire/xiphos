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
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/gbs.h"
#include "gui/gbs_dialog.h"
#include "gui/bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex.h"
#include "gui/dictlex_dialog.h"
#include "gui/percomm.h"
#include "gui/interlinear.h"
#include "gui/bookmarks.h"
#include "gui/studypad.h"
#include "gui/interlinear.h"
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
	g_print("%s\n", "Initiating GnomeSword\n");
	
	hint.in_popup = FALSE;
	hint.use_hints = FALSE;
	
	settings.displaySearchResults = FALSE;
	settings.havethayer = check_for_module("Thayer");
	settings.havebdb = check_for_module("BDB");

	/*
	 *  setup shortcut bar 
	 */
	gui_setup_shortcut_bar();

	/*
	 *  interlinear stuff 
	 */
	gui_check_interlinear_modules();
	gui_set_interlinear_options_at_start();
	gui_create_interlinear_popup(get_list(TEXT_DESC_LIST));

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
	 *  setup general book gui support 
	 */
	if (settings.havebook) {
		gui_setup_gbs(get_list(GBS_LIST));
		gui_setup_gbs_dialog(get_list(GBS_LIST));
	}

	/*
	 *  setup Dict/Lex gui support 
	 */
	if (settings.havedict) {
		gui_setup_dictlex(get_list(DICT_LIST));
		gui_setup_dictlex_dialog(get_list(DICT_LIST));
	}

	/*
	 *  setup Bible text gui 
	 */
	if (settings.havebible) {
		gui_setup_text(get_list(TEXT_LIST));
		gui_setup_bibletext_dialog(get_list(TEXT_LIST));
	}
	
	gui_set_shortcutbar_porgram_start();
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
	g_print("%s\n", "Displaying GnomeSword\n");
	gui_show_main_window();

	settings.addhistoryitem = FALSE;

	gui_change_verse(settings.currentverse);

	gui_change_module_and_key(settings.DictWindowModule, 
						settings.dictkey);
	
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
		gui_display_devotional();
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
	gui_save_bookmarks();
	xml_save_settings_doc(settings.fnconfigure);
	xml_free_settings_doc();
	
	/* if study pad file has changed since last save */
	if (widgets.studypad_dialog)
		gtk_widget_destroy(widgets.studypad_dialog);
	else
		gui_studypad_can_close();

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
