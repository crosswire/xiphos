/*
 * GnomeSword Bible Study Tool
 * history.c - add, remove and nav history
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
#include "main/url.hh"
#include "main/xml.h"


/******************************************************************************
 * global
 */

//gint history_items = 0;

/******************************************************************************
 * static 
 */
//static gint current_history_item = 0;
//static gboolean first_back_click = TRUE;


/******************************************************************************
 * Name
 *  gui_clear_history
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void gui_clear_history(GtkWidget * app)	
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
	gtk_widget_set_sensitive(nav_bar.button_back, FALSE);
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
	tab->history_items = 0;
	tab->current_history_item = 0;
	tab->first_back_click = TRUE;
	//main_add_tab_history_item(tab);
}


/******************************************************************************
 * Name
 *  updatehistorymenu
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void update_history_menu(GtkWidget * app)	
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
			      24); //tab->history_items + 1);
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
		gtk_widget_set_sensitive(nav_bar.button_back, TRUE);
	if (tab->current_history_item < tab->history_items)
		gtk_widget_set_sensitive(nav_bar.button_forward,
					 TRUE);

	
}

/******************************************************************************
 * Name
 *  gui_add_history_Item
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void gui_add_history_Item(GtkWidget * app, gchar * ref)	
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
	//g_message("history = %s",ref);
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
	strcpy(tab->history_list[tab->history_items].textmod, tab->text_mod);
	strcpy(tab->history_list[tab->history_items].commod, tab->commentary_mod);

	++tab->history_items;
	tab->current_history_item = tab->history_items;
	/* set sensitivity of history buttons */
	if (tab->current_history_item > 1)
		gtk_widget_set_sensitive(nav_bar.button_back, TRUE);
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
	main_update_tab_history_menu(data); //update_history_menu(app);
	tab->first_back_click = TRUE;
}


/******************************************************************************
 * Name
 *  gui_change_verse_history
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void gui_change_verse_history(gint historynum)	
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
#ifdef DEBUG 
	g_print("commod = %s\n",tab->history_list[historynum].commod);
	g_print("textmod = %s\n",tab->history_list[historynum].textmod);	
#endif
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
 *  gui_navigate_history
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void gui_navigate_history(GtkWidget * app, gint direction)	
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
		if (tab->current_history_item >= tab->history_items - 1)
			gtk_widget_set_sensitive(nav_bar.button_forward,
						 FALSE);
		if (tab->current_history_item >= 0)
			gtk_widget_set_sensitive(nav_bar.button_back,
						 TRUE);
	} else {
		if (tab->current_history_item > 0) {
			--tab->current_history_item;
			if (tab->first_back_click)
				--tab->current_history_item;
			
			main_change_verse_tab_history(tab->current_history_item);
			tab->first_back_click = FALSE;
		}
		/* set sensitivity of history buttons */
		if (tab->current_history_item < 1)
			gtk_widget_set_sensitive(nav_bar.button_back,
						 FALSE);
		if (tab->current_history_item < tab->history_items)
			gtk_widget_set_sensitive(nav_bar.button_forward,
						 TRUE);
	}
	settings.addhistoryitem = TRUE;
	
}
