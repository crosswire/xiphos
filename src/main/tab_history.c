/*
 * Xiphos Bible Study Tool
 * tab_history.c - add, remove and navigate history
 *
 * Copyright (C) 2005-2017 Xiphos Developer Team
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

#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/tabbed_browser.h"
//#include "gui/toolbar_nav.h"
#include "gui/utilities.h"
#include "gui/xiphos.h"
#include "gui/widgets.h"

#include "main/tab_history.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"
#include "main/navbar_versekey.h"

/* de-uglify all references to button sensitivitiy */
#define BUTTON_BACK navbar_versekey.button_history_back
#define BUTTON_FORW navbar_versekey.button_history_next

/******************************************************************************
 * Name
 *  on_menu_historyitem_activate
 *
 * Synopsis
 *   #include "main/tab_history.h"
 *
 *   void on_menu_historyitem_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)
 *
 * Description
 *   change text module to chosen histor item
 *
 * Return value
 *   void
 */

void
on_menu_historyitem_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	main_change_verse_tab_history(GPOINTER_TO_INT(user_data));
}

/******************************************************************************
 * Name
 *   main_clear_tab_history
 *
 * Synopsis
 *   #include "main/tab_history.h"
 *
 *   void main_clear_tab_history(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_clear_tab_history(void)
{
	PASSAGE_TAB_INFO *tab = cur_passage_tab;

	//gui_remove_menu_items(_("H_istory/<Separator>"),
	//                    tab->history_items + 1);
	//gui_add_separator2menu(widgets.app, _("H_istory/C_lear"));

	/* set sensitivity of history buttons */
	gtk_widget_set_sensitive(BUTTON_BACK, FALSE);
	gtk_widget_set_sensitive(BUTTON_FORW, FALSE);

	tab->history_items = 0;
	tab->current_history_item = 0;
	tab->first_back_click = TRUE;
}

/******************************************************************************
 * Name
 *   main_update_tab_history_menu
 *
 * Synopsis
 *   #include "main/tab_history.h"
 *
 *   void main_update_tab_history_menu(gpointer data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_update_tab_history_menu(gpointer data)
{
	PASSAGE_TAB_INFO *tab = (PASSAGE_TAB_INFO *)data;

	/* set sensitivity of history buttons */
	gtk_widget_set_sensitive(BUTTON_BACK,
				 (tab->current_history_item > 0));

	gtk_widget_set_sensitive(BUTTON_FORW,
				 (tab->current_history_item <
				  (tab->history_items - 1)));
}

/******************************************************************************
 * Name
 *   main_add_tab_history_item
 *
 * Synopsis
 *   #include "main/tab_history.h"
 *
 *   void main_add_tab_history_item(gpointer data)
 *
 * Description
 *    add an item to the history menu
 *
 * Return value
 *   void
 */
void main_add_tab_history_item(gpointer data)
{
	gint i;
	PASSAGE_TAB_INFO *tab = (PASSAGE_TAB_INFO *)data;

	/* check to see if item is already in list
	   if so do nothing except set button sensitivity */
	for (i = 0; i < tab->history_items; ++i) {
		if (!strcmp(tab->history_list[i].verseref,
			    tab->text_commentary_key)) {
			tab->current_history_item = i;
			main_update_tab_history_menu(data);
			return;
		}
	}

	/* if we have hit max length, dispose of oldest */
	if (tab->history_items == TABHISTORYLENGTH) {
		for (i = 0; i < (TABHISTORYLENGTH - 1); ++i) {
			tab->history_list[i] = tab->history_list[i + 1];
		}
		--tab->history_items;
	}

	/* add item to history menu */
	strcpy(tab->history_list[tab->history_items].verseref,
	       tab->text_commentary_key);
	if (tab->text_mod)
		strcpy(tab->history_list[tab->history_items].textmod,
		       tab->text_mod);
	if (tab->commentary_mod)
		strcpy(tab->history_list[tab->history_items].commod,
		       tab->commentary_mod);

	tab->current_history_item = tab->history_items;
	++tab->history_items;

	main_update_tab_history_menu(data);
	tab->first_back_click = TRUE;
}

/******************************************************************************
 * Name
 *   main_fake_tab_history_item
 *
 * Synopsis
 *   #include "main/tab_history.h"
 *
 *   void main_fake_tab_history_item(char *reference)
 *
 * Description
 *    fake an unreal history item into the history menu
 *
 * Return value
 *   void
 */
void main_fake_tab_history_item(char *reference)
{
	PASSAGE_TAB_INFO *tab = cur_passage_tab;

	/* if we have hit max length, dispose of oldest */
	if (tab->history_items == TABHISTORYLENGTH) {
		int i;
		for (i = 0; i < (TABHISTORYLENGTH - 1); ++i) {
			tab->history_list[i] = tab->history_list[i + 1];
		}
		--tab->history_items;
	}

	/* add item to history menu */
	strcpy(tab->history_list[tab->history_items].verseref, reference);
	if (tab->text_mod)
		strcpy(tab->history_list[tab->history_items].textmod,
		       tab->text_mod);
	if (tab->commentary_mod)
		strcpy(tab->history_list[tab->history_items].commod,
		       tab->commentary_mod);

	++tab->history_items;
	/* do not update what's current in history list. */
	main_update_tab_history_menu(tab);
}

/******************************************************************************
 * Name
 *  on_clear_activate
 *
 * Synopsis
 *   #include ".h"
 *
 *   void on_clear_activate(GtkMenuItem * menuitem,
 *			    gpointer user_data)
 *
 * Description
 *   remove all items from history list by calling
 *   gui_clear_history();
 *
 * Return value
 *   void
 */

void on_clear_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	main_clear_tab_history();
}

/******************************************************************************
 * Name
 *  on_history_to_verse_list_activate
 *
 * Synopsis
 *   #include ".h"
 *
 *   void on_history_to_verse_list_activate(GtkMenuItem * menuitem,
 *					    gpointer user_data)
 *
 * Description
 *   ship the history content to the verse list.
 *
 * Return value
 *   void
 */

void on_history_to_verse_list_activate(GtkMenuItem *menuitem,
				       gpointer user_data)
{
	int i;
	gboolean first = TRUE;
	GString *list = g_string_new("");

	for (i = 0; i < cur_passage_tab->history_items; ++i) {
		if (!first)
			g_string_append_c(list, ';');
		g_string_append(list,
				cur_passage_tab->history_list[i].verseref);
		first = FALSE;
	}

	main_display_verse_list_in_sidebar(settings.currentverse, settings.MainWindowModule, list->str);

	g_string_free(list, TRUE);
	return;
}

GtkWidget *main_versekey_drop_down_new(gpointer data)
{
	gint i;
	GtkWidget *menu;
	GtkWidget *item;
	PASSAGE_TAB_INFO *tab = NULL;

	tab = (PASSAGE_TAB_INFO *)data;
	menu = gtk_menu_new();

	item = gtk_menu_item_new_with_label(_("Clear History"));
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(on_clear_activate), NULL);
	gtk_container_add(GTK_CONTAINER(menu), item);

	item = gtk_menu_item_new_with_label(_("History ➛ Verse List"));
	gtk_widget_show(item);
	g_signal_connect(G_OBJECT(item), "activate",
			 G_CALLBACK(on_history_to_verse_list_activate),
			 NULL);
	gtk_container_add(GTK_CONTAINER(menu), item);

	for (i = 0; i < tab->history_items; ++i) {
		item =
		    gtk_menu_item_new_with_label(tab->history_list[i].verseref);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		g_signal_connect(G_OBJECT(item), "activate",
				 G_CALLBACK(on_menu_historyitem_activate),
				 GINT_TO_POINTER(i));
		if (!strcmp(tab->history_list[i].verseref,
			    tab->text_commentary_key))
			gtk_widget_set_sensitive(item, FALSE);
	}
	return menu;
}

/******************************************************************************
 * Name
 *   main_change_verse_tab_history
 *
 * Synopsis
 *   #include "main/tab_history.h"
 *
 *   void main_change_verse_tab_history(gint historynum)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_change_verse_tab_history(gint historynum)
{
	PASSAGE_TAB_INFO *tab = cur_passage_tab;
	gchar *key;

	tab->current_history_item = historynum;
	settings.addhistoryitem = tab->first_back_click;

	XI_print(("commod = %s\n", tab->history_list[historynum].commod));
	XI_print(("textmod = %s\n",
		  tab->history_list[historynum].textmod));

	key =
	    main_update_nav_controls(tab->history_list[historynum].textmod,
				     tab->history_list[historynum].verseref);
	main_display_commentary(tab->history_list[historynum].commod, key);
	main_display_bible(tab->history_list[historynum].textmod, key);
	main_keep_bibletext_dialog_in_sync(key);
	if (key)
		g_free(key);
}

/******************************************************************************
 * Name
 *   main_navigate_tab_history
 *
 * Synopsis
 *   #include "main/tab_history.h"
 *
 *   void main_navigate_tab_history(gint direction)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void main_navigate_tab_history(gint direction)
{
	PASSAGE_TAB_INFO *tab = cur_passage_tab;

	settings.addhistoryitem = FALSE;
	if (direction) {
		if (tab->current_history_item < tab->history_items - 1) {
			++tab->current_history_item;
			main_change_verse_tab_history(tab->current_history_item);
		}
	} else {
		if (tab->current_history_item > 0) {
			--tab->current_history_item;
#if 0
			if (tab->first_back_click &&
			    (tab->current_history_item > 0))
				--tab->current_history_item;
#endif

			main_change_verse_tab_history(tab->current_history_item);
			tab->first_back_click = FALSE;
		}
	}

	main_update_tab_history_menu((gpointer)tab);
	settings.addhistoryitem = TRUE;
}
