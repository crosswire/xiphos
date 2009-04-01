/*
 * Xiphos Bible Study Tool
 * xiphos.c -
 *
 * Copyright (C) 2000-2008 Xiphos Developer Team
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
#include <libxml/parser.h>
#include <ctype.h>
#include <time.h>

#ifdef USE_GTKHTML3_14_23
#include "editor/slib-editor.h"
#else
#include "editor/bonobo-editor.h"
#endif

#include "gui/xiphos.h"
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
#include "gui/widgets.h"
#include "gui/sidebar.h"
#include "gui/mod_mgr.h"
#include "gui/tabbed_browser.h"
#include "gui/bookmarks_menu.h"

#include "main/sword_treekey.h"
#include "main/navbar_book.h"
#include "main/parallel_view.h"
#include "main/previewer.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/search_sidebar.h"
#include "main/lists.h"
#include "main/xml.h"

#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
#endif

/******************************************************************************
 * Name
 *  init_xiphos
 *
 * Synopsis
 *   #include "xiphos.h"
 *
 *   void init_xiphos(void)
 *
 * Description
 *    sets up the interface
 *
 * Return value
 *   void
 */


void frontend_init(void)
{
	GS_print(("%s\n", "Initiating Xiphos"));
	settings.comm_showing = TRUE;
	settings.displaySearchResults = FALSE;
	settings.havethayer = main_is_module("Thayer");
	settings.havebdb = main_is_module("BDB");
	
	/*
	 * get and load books of the Bible 
	 */
#ifdef OLD_NAVBAR
	main_navbar_fill_book_combo(navbar_main);
#endif
	/*
	 *  setup sidebar
	 */
	gui_create_sidebar(widgets.epaned);
	main_init_sidebar_search_backend();
	main_search_sidebar_fill_bounds_combos();
	
	/*
	 *  parallel stuff
	 */	
	if (settings.havebible) {
		main_check_parallel_modules();
		main_init_parallel_view();
	}
	
	settings.paratab_showing = FALSE;
	
	main_dialogs_setup();

	gui_set_sidebar_program_start();
}


/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "xiphos.h"
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
	GS_print(("%s\n", "Displaying Xiphos"));
	gui_show_main_window();
	
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
	gui_show_hide_texts(settings.showtexts);
	gui_show_hide_dicts(settings.showdicts);
	gui_show_hide_comms(settings.showcomms);
	
 	gtk_window_move(GTK_WINDOW(widgets.app),settings.app_x,settings.app_y);
	
#ifdef USE_GTKMOZEMBED	/* gecko  needs the widgets to be visible before
	                   writing */
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
#endif
	// setup passage notebook
//	if(settings.browsing) {
	gui_notebook_main_setup (settings.browsing);
/*	} else {	
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
	} */
	/* must be set after tab stuff is done */

#ifdef USE_PARALLEL_TAB
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.parallel_tab_item),
				       settings.showparatab);
	g_signal_connect(GTK_OBJECT(widgets.parallel_tab_item),
			   "toggled",
			   G_CALLBACK(gui_parallel_tab_activate),
			   NULL	);
	gui_show_previewer_in_sidebar(settings.show_previewer_in_sidebar);
#endif /*  USE_PARALLEL_TAB  */	

		
	if (settings.showdevotional) 
		main_display_devotional();
	else 
		main_clear_viewer();
	gtk_widget_grab_focus (sidebar.module_list);
	
	GS_print(("%s\n\n", "done"));
} 


/******************************************************************************
 * Name
 *  shutdown_xiphos
 *
 * Synopsis
 *   #include "xiphos.h"
 *
 *   void shutdown_xiphos(void)
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

	editor_close_all();

	xml_save_settings_doc(settings.fnconfigure);
	xml_free_settings_doc();

	main_shutdown_list();
#ifdef USE_GTKMOZEMBED	
	gecko_html_shutdown();
#endif
//	if(settings.browsing)
	gui_notebook_main_shutdown (settings.browsing);

	/* free dir and file stuff */
	g_free(settings.gSwordDir);
	g_free(settings.shortcutbarDir);
	g_free(settings.fnconfigure);
	g_free(settings.swbmDir);
	
	main_dialogs_shutdown();
	main_delete_sidebar_search_backend();
	main_delete_parallel_view();
#ifndef OLD_NAVBAR	
	g_string_free(navbar_versekey.module_name,TRUE);
	g_string_free(navbar_versekey.key,TRUE);
#endif
	
	GS_print(("\n%s\n", "Xiphos is shutdown"));
}
