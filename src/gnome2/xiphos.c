/*
 * Xiphos Bible Study Tool
 * xiphos.c -
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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

#include "editor/slib-editor.h"

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
#include "gui/parallel_view.h"
#include "gui/dialog.h"
#include "gui/widgets.h"
#include "gui/sidebar.h"
#include "gui/mod_mgr.h"
#include "gui/tabbed_browser.h"
#include "gui/bookmarks_menu.h"
#include "gui/utilities.h"

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
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "gecko/gecko-html.h"
#endif
#endif

#include "gui/debug_glib_null.h"

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

	/*
	 *  setup sidebar
	 */
	gui_create_sidebar(widgets.epaned);

	/*
	 *  parallel stuff
	 */
	if (settings.havebible) {
		main_check_parallel_modules();
		main_init_parallel_view();	
		gui_create_parallel_page();
		gtk_widget_realize(widgets.html_parallel);	
		main_set_parallel_options_at_start();
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


void frontend_display(const char *tabs)
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

	/*
	 * a little paranoia:
	 * clamp geometry values to a reasonable bound.
	 * sometimes xiphos gets insane reconfig events as it dies,
	 * especially if it's due to just shutting linux down.
	 */
	if ((settings.app_x < 0) || (settings.app_x > 400))
		settings.app_x = 0;
	if ((settings.app_y < 0) || (settings.app_y > 400))
		settings.app_y = 0;

 	gtk_window_move(GTK_WINDOW(widgets.app),settings.app_x,settings.app_y);

#ifdef USE_GTKMOZEMBED
	/* gecko  needs the widgets to be visible before writing */
	sync_windows();
#endif
	// setup passage notebook
//	if (settings.browsing) {
	gui_notebook_main_setup (settings.browsing, tabs);
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

	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				       (widgets.parallel_tab_item),
				       settings.showparatab);
	g_signal_connect(GTK_OBJECT(widgets.parallel_tab_item),
			   "toggled",
			   G_CALLBACK(gui_parallel_tab_activate),
			   NULL	);
	gui_show_previewer_in_sidebar(settings.show_previewer_in_sidebar);

	if (settings.parallelpage)
		on_undockInt_activate(NULL);

	if (settings.showdevotional)
		main_display_devotional();
	else
		main_init_previewer();
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

	/* free verse list used for saving search results */
	if (list_of_verses) {
		GList *chaser = list_of_verses;
		while (chaser) {
			list_item = (RESULTS*)chaser->data;
			g_free(list_item->module);
			g_free(list_item->key);
			g_free(list_item);
			chaser = g_list_next(chaser);
		}
		g_list_free(list_of_verses);
		list_of_verses = NULL;
	}

	gui_save_bookmarks_treeview();

	/* if study pad file has changed since last save */


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

	g_string_free(navbar_versekey.module_name,TRUE);
	g_string_free(navbar_versekey.key,TRUE);

	GS_print(("\n%s\n", "Xiphos is shutdown"));
}
