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


#include "gui/history.h"
#include "gui/main_menu.h"
#include "gui/main_window.h"
#include "gui/toolbar_nav.h"
#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/xml.h"


/******************************************************************************
 * global
 */
HISTORY history_list[25];
gint history_items = 0;

/******************************************************************************
 * static 
 */
static gint current_history_item = 0;
static gboolean first_back_click = TRUE;


/******************************************************************************
 * Name
 *  gui_clear_history
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void gui_clear_history(GtkWidget * app, GtkWidget * shortcut_bar)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void gui_clear_history(GtkWidget * app, GtkWidget * shortcut_bar)
{
	gint i;

	gui_remove_menu_items(_("H_istory/<Separator>"),
			      history_items + 1);
	gui_add_separator2menu(app, _("H_istory/C_lear"));
	/* set sensitivity of history buttons */
	gtk_widget_set_sensitive(nav_bar.button_back, FALSE);
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
	history_items = 0;
	current_history_item = 0;
	first_back_click = TRUE;
	gui_add_history_Item(widgets.app, NULL, settings.currentverse);
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

static void update_history_menu(GtkWidget * app)
{
	gint i;
	gchar buf[80];
	GnomeUIInfo *menuitem;
	
	gui_remove_menu_items(_("H_istory/<Separator>"),
			      history_items + 1);
	gui_add_separator2menu(app, _("H_istory/C_lear"));
	
	for (i = 0; i < history_items; i++) {
		menuitem = g_new(GnomeUIInfo, 2);
		menuitem->type = GNOME_APP_UI_ITEM;
		menuitem->moreinfo = (gpointer) on_mnuHistoryitem1_activate;
		menuitem->user_data = GINT_TO_POINTER(history_list[i].itemnum);
		menuitem->label = history_list[i].verseref;
		menuitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
		menuitem->pixmap_info = GNOME_STOCK_BOOK_OPEN;
		menuitem->accelerator_key = 0;
		menuitem[1].type = GNOME_APP_UI_ENDOFINFO;
		gnome_app_insert_menus_with_data(GNOME_APP(app), 
						_("H_istory/<Separator>"),
						menuitem, NULL);
	}
}

/******************************************************************************
 * Name
 *  gui_add_history_Item
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void gui_add_history_Item(GtkWidget * app, GtkWidget * shortcut_bar,
 *		    gchar * ref)	
 *
 * Description
 *    add an item to the history menu
 *
 * Return value
 *   void
 */

void gui_add_history_Item(GtkWidget * app, GtkWidget * shortcut_bar, gchar * ref)
{
	gint i;

	/* check to see if item is already in list  
	   if so do nothing */
	for (i = 0; i < history_items; i++) {
		if (!strcmp(history_list[i].verseref, ref))
			return;
	}
	/* add item to history menu */
	if (history_items >= 24) {
		for (i = 0; i < 24; i++) {
			history_list[i] = history_list[i + 1];
		}
		history_items = 23;
	}
	history_list[history_items].itemnum = history_items;
	
	sprintf(history_list[history_items].verseref, "%s", ref);
	if(xml_get_value("modules", "bible"))
		sprintf(history_list[history_items].textmod, "%s",
			xml_get_value("modules", "bible"));
	if(xml_get_value("modules", "comm"))
		sprintf(history_list[history_items].commod, "%s",
			xml_get_value("modules", "comm"));

	++history_items;
	current_history_item = history_items;
	/* set sensitivity of history buttons */
	if (current_history_item > 1)
		gtk_widget_set_sensitive(nav_bar.button_back, TRUE);
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
	update_history_menu(app);
	first_back_click = TRUE;
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

void gui_change_verse_history(gint historynum)
{
	current_history_item = historynum;
	if (first_back_click) {
		settings.addhistoryitem = TRUE;
	} else {
		settings.addhistoryitem = FALSE;
	}
	/* change text mod */
	gui_change_module_and_key(history_list[historynum].textmod,
				  history_list[historynum].verseref);
	/* change commentary mod */
	gui_change_module_and_key(history_list[historynum].commod,
				  history_list[historynum].verseref);
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

void gui_navigate_history(GtkWidget * app, gint direction)
{
	settings.addhistoryitem = FALSE;
	if (direction) {
		if (current_history_item < history_items - 1) {
			++current_history_item;
			gui_change_verse_history(current_history_item);
		}
		/* set sensitivity of history buttons */
		if (current_history_item >= history_items - 1)
			gtk_widget_set_sensitive(nav_bar.button_forward,
						 FALSE);
		if (current_history_item >= 0)
			gtk_widget_set_sensitive(nav_bar.button_back,
						 TRUE);
	} else {
		if (current_history_item > 0) {
			--current_history_item;
			if (first_back_click)
				--current_history_item;
			
			gui_change_verse_history(current_history_item);
			first_back_click = FALSE;
		}
		/* set sensitivity of history buttons */
		if (current_history_item < 1)
			gtk_widget_set_sensitive(nav_bar.button_back,
						 FALSE);
		if (current_history_item < history_items)
			gtk_widget_set_sensitive(nav_bar.button_forward,
						 TRUE);
	}
	settings.addhistoryitem = TRUE;
	
}
