/*
 * GnomeSword Bible Study Tool
 * tab_history.c - add, remove and navigate history
 *
 * Copyright (C) 2005 GnomeSword Developer Team
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


#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/tabbed_browser.h"
#include "gui/toolbar_nav.h"
#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/tab_history.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"
#include "main/navbar_versekey.h"



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
	gint i;
	PASSAGE_TAB_INFO *tab = (PASSAGE_TAB_INFO*) cur_passage_tab;

	gui_remove_menu_items(_("H_istory/<Separator>"),
			      tab->history_items + 1);
	gui_add_separator2menu(widgets.app, _("H_istory/C_lear"));
	
	/* set sensitivity of history buttons */
#ifdef OLD_NAVBAR
	gtk_widget_set_sensitive(nav_bar.button_back, FALSE);
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
#else
	gtk_widget_set_sensitive(navbar_versekey.button_history_back, FALSE);
	gtk_widget_set_sensitive(navbar_versekey.button_history_next, FALSE);
	
#endif
	
	tab->history_items = 0;
	tab->current_history_item = 0;
	tab->first_back_click = TRUE;
	//main_add_tab_history_item(tab);
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
	gint i;
	gchar buf[80];
	GnomeUIInfo *menuitem;
	PASSAGE_TAB_INFO *tab = (PASSAGE_TAB_INFO*) data;
	
	gui_remove_menu_items(_("H_istory/<Separator>"),
			      25); //tab->history_items + 1);
	gui_add_separator2menu(widgets.app, _("H_istory/C_lear"));
	
	for (i = 0; i < tab->history_items; i++) {
		menuitem = g_new(GnomeUIInfo, 2);
		menuitem->type = GNOME_APP_UI_ITEM;
		menuitem->moreinfo = (gpointer) on_mnuHistoryitem1_activate;
		menuitem->user_data = GINT_TO_POINTER(tab->history_list[i].itemnum);
		menuitem->label = tab->history_list[i].verseref;
		menuitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
		menuitem->pixmap_info = GNOME_STOCK_BOOK_OPEN;
		menuitem->accelerator_key = 0;
		menuitem[1].type = GNOME_APP_UI_ENDOFINFO;
		gnome_app_insert_menus_with_data(GNOME_APP(widgets.app), 
						_("H_istory/<Separator>"),
						menuitem, NULL);
	}
	/* set sensitivity of history buttons */
	if (tab->current_history_item > 1)
#ifdef OLD_NAVBAR
		gtk_widget_set_sensitive(nav_bar.button_back, TRUE);
#else
		gtk_widget_set_sensitive(navbar_versekey.button_history_back, TRUE);
#endif
	if (tab->current_history_item < tab->history_items)
#ifdef OLD_NAVBAR
		gtk_widget_set_sensitive(nav_bar.button_forward,
					 TRUE);
#else
		gtk_widget_set_sensitive(navbar_versekey.button_history_next,
					 TRUE);
#endif

	
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
	PASSAGE_TAB_INFO *tab = (PASSAGE_TAB_INFO*) data;
	
	/* check to see if item is already in list  
	   if so do nothing */
	for (i = 0; i < tab->history_items; i++) {
		if (!strcmp(tab->history_list[i].verseref, tab->text_commentary_key))
			return;
	}
	/* add item to history menu */
	if (tab->history_items >= 24) {
		for (i = 0; i < 24; i++) {
			tab->history_list[i] = tab->history_list[i + 1];
		}
		tab->history_items = 23;
	}
	tab->history_list[tab->history_items].itemnum = tab->history_items;
	
	strcpy(tab->history_list[tab->history_items].verseref, tab->text_commentary_key);
	if(tab->text_mod)
		strcpy(tab->history_list[tab->history_items].textmod, tab->text_mod);
	if(tab->commentary_mod)
		strcpy(tab->history_list[tab->history_items].commod, tab->commentary_mod);

	++tab->history_items;
	tab->current_history_item = tab->history_items;
	/* set sensitivity of history buttons */
	if (tab->current_history_item > 1)
#ifdef OLD_NAVBAR
		gtk_widget_set_sensitive(nav_bar.button_back, TRUE);
#else
		gtk_widget_set_sensitive(navbar_versekey.button_history_back, TRUE);
#endif
#ifdef OLD_NAVBAR
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
#else
	gtk_widget_set_sensitive(navbar_versekey.button_history_next, FALSE);
#endif
	main_update_tab_history_menu(data);
	tab->first_back_click = TRUE;
}

/******************************************************************************
 * Name
 *  on_clear_activate
 *
 * Synopsis
 *   #include ".h"
 *
 *   void on_clear_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)	
 *
 * Description
 *   remove all items from history list by calling
 *   gui_clear_history();
 *
 * Return value
 *   void
 */

void on_clear_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	main_clear_tab_history();//gui_clear_history(widgets.app);
}

/*
struct _tab_history {
        gint itemnum;
        gchar verseref[80];
        gchar textmod[80];
        gchar commod[80];
};
*/
GtkWidget *main_versekey_drop_down_new(gpointer data)
{	
	gint i;
	gchar *tmpbuf = NULL;
	GtkWidget *menu;
	GtkWidget *item;
	unsigned long offset;
	
	PASSAGE_TAB_INFO *tab = (PASSAGE_TAB_INFO*) data;
	menu = gtk_menu_new();
	item = gtk_menu_item_new_with_label("Clear History");
	gtk_widget_show(item);
		g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK
				   (on_clear_activate),
				   NULL);		
		gtk_container_add(GTK_CONTAINER(menu), item); 
	for (i = 0; i < tab->history_items; i++) {
		item = gtk_menu_item_new_with_label(tab->history_list[i].verseref);
		gtk_widget_show(item);	
		gtk_container_add(GTK_CONTAINER(menu), item);
		g_signal_connect(GTK_OBJECT(item), "activate",
				   G_CALLBACK
				   (on_mnuHistoryitem1_activate),
				   GINT_TO_POINTER(tab->history_list[i].itemnum));	
		if (!strcmp(tab->history_list[i].verseref, tab->text_commentary_key))
			gtk_widget_set_sensitive(item,FALSE);
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
	gchar *url;	
	PASSAGE_TAB_INFO *tab = (PASSAGE_TAB_INFO*) cur_passage_tab;
	
	tab->current_history_item = historynum;
	if (tab->first_back_click) {
		settings.addhistoryitem = TRUE;
	} else {
		settings.addhistoryitem = FALSE;
	}
	GS_print(("commod = %s\n",tab->history_list[historynum].commod));
	GS_print(("textmod = %s\n",tab->history_list[historynum].textmod));	
	// ** change commentary mod **
	url = g_strdup_printf("sword://%s/%s",tab->history_list[historynum].commod,
				  tab->history_list[historynum].verseref);
	main_url_handler(url, TRUE);
	g_free(url);
	// ** change text mod **
	url = g_strdup_printf("sword://%s/%s",tab->history_list[historynum].textmod,
				  tab->history_list[historynum].verseref);
	main_url_handler(url, TRUE);
	g_free(url);

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
	PASSAGE_TAB_INFO *tab = (PASSAGE_TAB_INFO*) cur_passage_tab;
	settings.addhistoryitem = FALSE;
	if (direction) {
		if (tab->current_history_item < tab->history_items - 1) {
			++tab->current_history_item;
			main_change_verse_tab_history(tab->current_history_item);
		}
		/* set sensitivity of history buttons */
#ifdef OLD_NAVBAR
		if (tab->current_history_item >= tab->history_items - 1)
			gtk_widget_set_sensitive(nav_bar.button_forward,
						 FALSE);
		if (tab->current_history_item >= 0)
			gtk_widget_set_sensitive(nav_bar.button_back,
						 TRUE);
#else
		if (tab->current_history_item >= tab->history_items - 1)
			gtk_widget_set_sensitive(navbar_versekey.button_history_next,
						 FALSE);
		if (tab->current_history_item >= 0)
			gtk_widget_set_sensitive(navbar_versekey.button_history_back,
						 TRUE);
#endif
	} else {
		if (tab->current_history_item > 0) {
			--tab->current_history_item;
			if (tab->first_back_click)
				--tab->current_history_item;
			
			main_change_verse_tab_history(tab->current_history_item);
			tab->first_back_click = FALSE;
		}
		/* set sensitivity of history buttons */
#ifdef OLD_NAVBAR
		if (tab->current_history_item < 1)
			gtk_widget_set_sensitive(nav_bar.button_back,
						 FALSE);
		if (tab->current_history_item < tab->history_items)
			gtk_widget_set_sensitive(nav_bar.button_forward,
						 TRUE);
#else
		if (tab->current_history_item < 1)
			gtk_widget_set_sensitive(navbar_versekey.button_history_back,
						 FALSE);
		if (tab->current_history_item < tab->history_items)
			gtk_widget_set_sensitive(navbar_versekey.button_history_next,
						 TRUE);
#endif
	}
	settings.addhistoryitem = TRUE;
	
}
