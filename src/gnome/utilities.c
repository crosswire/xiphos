/*
 * GnomeSword Bible Study Tool
 * utilities.c - support functions
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
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "gui/utilities.h"
#include "gui/gnomesword.h"
#include "main/lists.h"
#include "main/settings.h"


/******************************************************************************
 * Name
 *   remove_linefeeds
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   gchar * remove_linefeeds(gchar * buf)
 *
 * Description
 *   remove line feeds so we can display string in appbar
 *
 * Return value
 *   gchar * 
 */
 
gchar * remove_linefeeds(gchar * buf)
{	
	gchar * key = NULL;
	
	key = g_strdelimit(buf, "\n", ' ');
	
	if(key)
		return g_strdup(key);
	else
		return NULL;
		
}


/******************************************************************************
 * Name
 *   gui_add_item2gnome_menu
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   void gui_add_item2gnome_menu(GtkWidget * MainFrm, gchar * itemname, 
 *	gchar * itemdata, gchar * menuname, GtkMenuCallback mycallback)
 *
 * Description
 *   adds an item to the main menu bar
 *
 * Return value
 *   void
 */

void gui_add_item2gnome_menu(GtkWidget * MainFrm, gchar * itemname, 
	gchar * itemdata, gchar * menuname, GtkMenuCallback mycallback)
{
	GnomeUIInfo *menuitem;

	menuitem = g_new(GnomeUIInfo, 2);
	menuitem->type = GNOME_APP_UI_ITEM;
	menuitem->moreinfo = (gpointer) mycallback;
	menuitem->user_data = g_strdup(itemdata);
	menuitem->label = itemname;
	menuitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
	menuitem->pixmap_info = GNOME_STOCK_MENU_BOOK_OPEN;
	menuitem->accelerator_key = 0;
	menuitem[1].type = GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm), menuname,
					 menuitem, NULL);
}


/******************************************************************************
 * Name
 *   gui_add_separator2menu
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   void gui_add_separator2menu(GtkWidget * MainFrm, gchar * subtreelabel)
 *
 * Description
 *   add separator line to menu
 *
 * Return value
 *   void
 */

void gui_add_separator2menu(GtkWidget * MainFrm, gchar * subtreelabel)
{
	GnomeUIInfo *bookmarkitem;
	bookmarkitem = g_new(GnomeUIInfo, 2);
	bookmarkitem->type = GNOME_APP_UI_SEPARATOR;
	bookmarkitem->pixmap_type = GNOME_APP_PIXMAP_NONE;
	bookmarkitem->accelerator_key = 0;
	bookmarkitem[1].type = GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm), subtreelabel,
					 bookmarkitem, NULL);
	//g_free(bookmarkitem); 
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   void add_mods_to_menus(GList * modlist, gchar * menu,
 *				GtkMenuCallback callback) 
 *
 * Description
 *   add a list of modules to a menu
 *
 * Return value
 *   void
 */

void gui_add_mods_to_menus(GList * modlist, gchar * menu,
				GtkMenuCallback callback) 
{	
	gchar	
		view_remember_last_item[80];
	//gint	i = 0;
	GList 	*tmp = NULL;

	sprintf(view_remember_last_item,"%s", menu);
	
	
	tmp = modlist;
	while (tmp != NULL) {	
		gui_add_item2gnome_menu(widgets.app, 
			(gchar *) tmp->data, 
			(gchar *) tmp->data,
			view_remember_last_item, 
			callback);
		/* remember last item - so next item will be placed below it */
		sprintf(view_remember_last_item,"%s%s", 
				menu, 
				(gchar *) tmp->data);	
		//++i;
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
}			


/******************************************************************************
 * Name
 *   gui_remove_menu_items
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   void gui_remove_menu_items(gchar * startitem, gint numberofitems)
 *
 * Description
 *   remove a number(numberofitems) of items form a menu or submenu(startitem)
 *
 * Return value
 *   void
 */

void gui_remove_menu_items(gchar * startitem, gint numberofitems)
{				
	gnome_app_remove_menus(GNOME_APP(widgets.app), startitem,
			       numberofitems);
}			

/******************************************************************************
 * Name
 *  add_mods_2_gtk_menu
 *
 * Synopsis
 *   #include "gui/utilities.h"
 *
 *   void add_mods_2_gtk_menu(gchar * mod_type, GtkMenu * menu,
				GtkMenuCallback callback)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_add_mods_2_gtk_menu(gint mod_type, GtkWidget * menu,
				GtkMenuCallback callback)
{
	GList 	*tmp = NULL;
	GtkWidget * item;
	
	tmp = get_list(mod_type);
	while (tmp != NULL) {	
		item =
		    gtk_menu_item_new_with_label((gchar *) tmp->data);
		gtk_widget_show(item);
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC
				   (callback),
				   (gchar *) tmp->data);		
		gtk_container_add(GTK_CONTAINER(menu), item); 
		tmp = g_list_next(tmp);	
	}	
}



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

GList *gui_fill_count_list(int count)
{
	GList *glist = NULL;
	gint start;
	gchar buf[32];
	
	for(start = 1; start <= count ; start++) {
		sprintf(buf, "%d", start);
		glist = g_list_append(glist,
				 (gchar *) g_strdup(buf));
	}
	return glist;
}			


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/utilities.h
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

void gui_free_count_list(GList *glist)
{
	glist = g_list_first(glist);
	while (glist != NULL) {
		gchar *buf = (gchar *) glist->data;
		g_free(buf);
		glist = g_list_next(glist);
	}
	g_list_free(glist);
}

/******   end of file   ******/
