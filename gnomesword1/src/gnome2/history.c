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

#include  <gal/shortcut-bar/e-shortcut-bar.h>

/******************************************************************************
 * externs
 */
extern gint groupnum4;

/******************************************************************************
 * global
 */
HISTORY historylist[25];
gint historyitems = 0;

/******************************************************************************
 * static 
 */
static gint currenthistoryitem = 0;
static gboolean firstbackclick = TRUE;


/******************************************************************************
 * Name
 *  clearhistory
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void clearhistory(GtkWidget * app, GtkWidget * shortcut_bar)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void clearhistory(GtkWidget * app, GtkWidget * shortcut_bar)
{
	gint i;

	gui_remove_menu_items(_("_History/<Separator>"),
			      historyitems + 1);
	gui_add_separator2menu(app, _("_History/C_lear"));
	/* set sensitivity of history buttons */
	gtk_widget_set_sensitive(nav_bar.button_back, FALSE);
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);

	if (settings.showhistorygroup) {
		for (i = historyitems - 1; i >= 0; i--) {
			e_shortcut_model_remove_item(E_SHORTCUT_BAR
						     (shortcut_bar)->
						     model, groupnum4,
						     i);
		}
	}
	historyitems = 0;
	currenthistoryitem = 0;
	firstbackclick = TRUE;
}


/******************************************************************************
 * Name
 *  addHistoryItem
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void addHistoryItem(GtkWidget * app, GtkWidget * shortcut_bar,
 *		    gchar * ref)	
 *
 * Description
 *    add an item to the history menu
 *
 * Return value
 *   void
 */

void addHistoryItem(GtkWidget * app, GtkWidget * shortcut_bar,
		    gchar * ref)
{
	gint i;

	/* check to see if item is already in list  
	   if so do nothing */
	for (i = 0; i < historyitems; i++) {
		if (!strcmp(historylist[i].verseref, ref))
			return;
	}
	/* add item to history menu */
	if (historyitems >= 24) {
		for (i = 0; i < 24; i++) {
			historylist[i] = historylist[i + 1];
		}
		historyitems = 23;
	}
	historylist[historyitems].itemnum = historyitems;
	historylist[historyitems].compagenum =
	    gtk_notebook_get_current_page(GTK_NOTEBOOK
					  (widgets.notebook_comm));
	sprintf(historylist[historyitems].verseref, "%s", ref);
	sprintf(historylist[historyitems].textmod, "%s",
		xml_get_value("modules", "bible"));//settings.MainWindowModule);
	sprintf(historylist[historyitems].commod, "%s",
		xml_get_value("modules", "comm"));//settings.CommWindowModule);

	++historyitems;
	currenthistoryitem = historyitems;
	/* set sensitivity of history buttons */
	if (currenthistoryitem > 0)
		gtk_widget_set_sensitive(nav_bar.button_back, TRUE);
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
	updatehistorymenu(app);
	firstbackclick = TRUE;
	if (settings.showhistorygroup) {
		updatehistoryshortcutbar(app, shortcut_bar);
	}
}


/******************************************************************************
 * Name
 *  changeverseHistory
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void changeverseHistory(gint historynum)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void changeverseHistory(gint historynum)
{
	currenthistoryitem = historynum;
	if (firstbackclick) {
		settings.addhistoryitem = TRUE;
	} else {
		settings.addhistoryitem = FALSE;
	}
	/* change text mod */
	gui_change_module_and_key(historylist[historynum].textmod,
				  historylist[historynum].verseref);
	/* change commentary mod */
	gui_change_module_and_key(historylist[historynum].commod,
				  historylist[historynum].verseref);
	if (firstbackclick) {
		--currenthistoryitem;
		--currenthistoryitem;
		firstbackclick = FALSE;
	}
}


/******************************************************************************
 * Name
 *  historynav
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void historynav(GtkWidget * app, gint direction)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void historynav(GtkWidget * app, gint direction)
{
	settings.addhistoryitem = FALSE;
	if (direction) {
		if (currenthistoryitem < historyitems - 1) {
			++currenthistoryitem;
			changeverseHistory(currenthistoryitem);
		}
		/* set sensitivity of history buttons */
		if (currenthistoryitem >= historyitems - 1)
			gtk_widget_set_sensitive(nav_bar.button_forward,
						 FALSE);
		if (currenthistoryitem >= 0)
			gtk_widget_set_sensitive(nav_bar.button_back,
						 TRUE);
	} else {
		if (currenthistoryitem > 0) {
			--currenthistoryitem;
			if (firstbackclick)
				settings.addhistoryitem = TRUE;
			changeverseHistory(currenthistoryitem);
			firstbackclick = FALSE;
		}
		/* set sensitivity of history buttons */
		if (currenthistoryitem < 1)
			gtk_widget_set_sensitive(nav_bar.button_back,
						 FALSE);
		if (currenthistoryitem < historyitems)
			gtk_widget_set_sensitive(nav_bar.button_forward,
						 TRUE);
	}
}


/******************************************************************************
 * Name
 *  updatehistorymenu
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void updatehistorymenu(GtkWidget * app)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void updatehistorymenu(GtkWidget * app)
{
	gint i;
	gchar buf[80];

	gui_remove_menu_items(_("_History/<Separator>"),
			      historyitems + 1);
	gui_add_separator2menu(app, _("_History/C_lear"));
	for (i = 0; i < historyitems; i++) {
		sprintf(buf, "%d", historylist[i].itemnum);
		gui_add_item2gnome_menu(app, historylist[i].verseref,
					buf, _("_History/<Separator>"),
					(GtkMenuCallback)
					on_mnuHistoryitem1_activate);
	}
}


/******************************************************************************
 * Name
 *  updatehistoryshortcutbar
 *
 * Synopsis
 *   #include "gui/history.h"
 *
 *   void updatehistoryshortcutbar(GtkWidget * app, GtkWidget * shortcut_bar)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void updatehistoryshortcutbar(GtkWidget * app, GtkWidget * shortcut_bar)
{
	gint i;
	GdkPixbuf *icon_pixbuf = NULL;
	GError **error;
	
	icon_pixbuf = gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					       "/book-un.png",
						error);

	for (i = historyitems - 2; i >= 0; i--) {
		e_shortcut_model_remove_item(E_SHORTCUT_BAR
					     (shortcut_bar)->model,
					     groupnum4, i);
	}
	for (i = 0; i < historyitems; i++) {
		e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->
					  model, groupnum4, -1,
					  historylist[i].verseref,
					  historylist[i].verseref,
					  icon_pixbuf);
	}
}
