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
#include <libxml/parser.h>
#include <ctype.h>
#include <time.h>

#ifdef USE_GTKHTML38
#include "editor/html-editor.h"
#endif

#include "gui/gnomesword.h"
#include "gui/bookmarks_treeview.h"
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
#include "gui/bookmarks.h"
#include "gui/studypad.h"
#include "gui/parallel_view.h"
#include "gui/dialog.h"
#include "gui/hints.h"
#include "gui/widgets.h"
#include "gui/sidebar.h"
#include "gui/mod_mgr.h"
#include "gui/tabbed_browser.h"

#include "main/parallel_view.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/sidebar.h"
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
	guint number_of_pages = 0;
#ifdef DEBUG	
	g_print("%s\n", _("Initiating GnomeSword"));
#endif

	
	hint.in_popup = FALSE;
	hint.use_hints = FALSE;
	
	settings.comm_showing = TRUE;
	settings.displaySearchResults = FALSE;
	settings.havethayer = main_is_module("Thayer");
	settings.havebdb = main_is_module("BDB");
	
	/*
	 * get and load books of the Bible 
	 */
	main_navbar_fill_book_combo(navbar_main);
	/*
	 *  setup sidebar
	 */
	gui_create_sidebar(widgets.epaned);
	main_init_sidebar_search_backend();
	/*
	 *  parallel stuff
	 */
	if (settings.havebible) {
		main_check_parallel_modules();
		main_init_paraellel_view();
		//main_set_parallel_options_at_start();
		//gui_create_parallel_popup(get_list(TEXT_DESC_LIST));
	}

	main_dialogs_setup();
	// setup passage notebook
	if(settings.browsing)
		gui_notebook_main_setup();

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
	gchar *url;
	extern GList *passage_list;
#ifdef DEBUG	
	g_print("%s\n", _("Displaying GnomeSword"));
#endif
	gui_show_main_window();

//	gui_add_history_Item(widgets.app, settings.currentverse);
	
	main_clear_viewer();
	
	gtk_widget_realize(widgets.html_dict);
	url = g_strdup_printf("sword://%s/%s",settings.DictWindowModule,
					      settings.dictkey);
	main_url_handler(url);
	g_free(url);	
	
	gtk_widget_realize(widgets.html_book);
	url = g_strdup_printf("sword://%s/%d",settings.book_mod,
					      settings.book_offset);
	main_url_handler(url);
	g_free(url);
	
	settings.addhistoryitem = FALSE;
	url = g_strdup_printf("sword://%s/%s",settings.MainWindowModule,
					      settings.currentverse);
	main_url_handler(url);
	g_free(url);	
/*			
	if(!settings.first_run) {
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
		if(!settings.havedict) {
			gui_show_hide_dicts(FALSE);
			gtk_widget_set_sensitive(widgets.viewdicts_item, FALSE);
		}
	}
*/		
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewtexts_item),
				       settings.showtexts);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewcomms_item),
				       settings.showcomms);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewdicts_item),
				       settings.showdicts);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.viewpreview_item),
				       settings.showpreview);
	gui_show_hide_preview(settings.showpreview);
	/* showing the devotional must come after the the app is shown or
	 *  it will mess up the sidebar display
	 */
	/*
	 * FIXME: maybe we need to move the devotional?
	 */
	if (settings.showdevotional) {
		main_display_devotional();
	}
	
 	gtk_window_move(GTK_WINDOW(widgets.app),settings.app_x,settings.app_y);
	if(settings.setup_canceled) {
		str = g_string_new("");
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			_("The Setup Druid was cancelled."),
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
	
	/*
	gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.notebook_comm_book),
					      0);
	gui_notebook_main_switch_page(GTK_NOTEBOOK(widgets.notebook_main),
					 NULL,
					 0, 
					 &passage_list);*/
	gtk_widget_grab_focus (sidebar.module_list);
#ifdef DEBUG	
	g_print("%s\n\n", _("done"));
#endif
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
	RESULTS *list_item;
	if(pixbufs->pixbuf_closed)
		g_object_unref(pixbufs->pixbuf_closed);
	if(pixbufs->pixbuf_opened)
		g_object_unref(pixbufs->pixbuf_opened);
	if(pixbufs->pixbuf_helpdoc)
		g_object_unref(pixbufs->pixbuf_helpdoc);

	if(navbar_main.module_name) 
		g_free(navbar_main.module_name);
	if(navbar_main.key) 
		g_free(navbar_main.key);
	
	/* free verse list used for saving search results */
	if(list_of_verses) {
		while(list_of_verses) {
			list_item = (RESULTS*)list_of_verses->data;
			g_free(list_item->module);
			g_free(list_item->key);
			g_free(list_item);
			list_of_verses = g_list_next(list_of_verses);
		}
		g_list_free(list_of_verses);
	}
	list_of_verses = NULL;

	gui_save_bookmarks_treeview();

	/* if study pad file has changed since last save */

#ifdef USE_GTKHTML38
	editor_close_all();
#else
	gui_studypad_can_close();
#endif

	xml_save_settings_doc(settings.fnconfigure);
	xml_free_settings_doc();

	main_shutdown_list();

	if(settings.browsing)
		gui_notebook_main_shutdown();

	/* free dir and file stuff */
	g_free(settings.gSwordDir);
	g_free(settings.shortcutbarDir);
	g_free(settings.fnconfigure);
	g_free(settings.swbmDir);
	
	main_dialogs_shutdown();
	main_delete_sidebar_search_backend();
	main_delete_paraellel_view();
	
#ifdef DEBUG	
	g_print("\n%s\n", _("GnomeSword is shutdown"));
#endif

}
