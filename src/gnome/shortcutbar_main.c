/*
 * GnomeSword Bible Study Tool
 * shortcutbar_main.c - create and maintain the main shortcut bar
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
#include <gtkhtml/gtkhtml.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/shortcut-bar/e-vscrolled-bar.h>
#include <gal/widgets/e-unicode.h>

#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_dialog.h"
#include "gui/shortcutbar_search.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/editor.h"
#include "gui/bookmarks.h"
#include "gui/utilities.h"
#include "gui/html.h"
#include "gui/about_modules.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"

#include "main/shortcutbar.h"
#include "main/sword.h"
#include "main/gbs.h"
#include "main/settings.h"
#include "main/lists.h"

extern SB_VIEWER sb_v, *sv ;

extern gchar *shortcut_types[];


GtkWidget *shortcut_bar;
EShortcutModel *shortcut_model;
gint 
	groupnum0 = -1, 
	groupnum1 = -1, 
	groupnum2 = -1, 
	groupnum3 = -1, 
	groupnum4 = -1, 
	groupnum5 = -1, 
	groupnum6 = -1, 
	groupnum7 = -1, 
	groupnum8 = -1;	


static void on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
					  GdkEvent * event,
					  gint group_num,
					  gint item_num);
static void show_standard_popup(EShortcutBar * shortcut_bar,
				GdkEvent * event, gint group_num);
static void show_context_popup(EShortcutBar * shortcut_bar,
			       GdkEvent * event,
			       gint group_num, gint item_num);
static void set_small_icons(GtkWidget * menuitem,
			    EShortcutBar * shortcut_bar);
static void set_large_icons(GtkWidget * menuitem,
			    EShortcutBar * shortcut_bar);
static gint add_sb_group(EShortcutBar * shortcut_bar,
			 gchar * group_name);
static void on_add_all_activate(GtkMenuItem * menuitem,
				gpointer user_data);
static void remove_all_items(gint group_num);
static void gui_create_mod_list_menu(gint group_num, GtkWidget * menu,
		      GtkWidget * shortcut_menu_widget, gint mod_type);      
		      
		      
/******************************************************************************
 * Name
 *   showSBGroup 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void showSBGroup(gint groupnum)	
 *
 * Description
 *   display shortcut bar group - display shortbut bar if not showing
 *
 * Return value
 *   void
 */

void showSBGroup(gint groupnum)
{
	EShortcutBar *bar1;
	if (!settings.showshortcutbar) {
		gint biblepanesize;
		biblepanesize =
		    (settings.gs_width -
		     settings.shortcutbar_width) / 2;
		e_paned_set_position(E_PANED(widgets.epaned),
				     settings.shortcutbar_width);
		e_paned_set_position(E_PANED
				     (widgets.hpaned),
				     biblepanesize);
		settings.showshortcutbar = TRUE;
	}
	bar1 = E_SHORTCUT_BAR(widgets.shortcutbar);
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar1),
					  groupnum, TRUE);
}
		      
/******************************************************************************
 * Name
 *    get_pixbuf
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   GdkPixbuf * get_pixbuf(gint type, gchar *item_url)	
 *
 * Description
 *   return pixbuf to use for creating a shortcut item
 *
 * Return value
 *   GdkPixbuf *
 */

GdkPixbuf * get_pixbuf(gint type, gchar *item_url)
{
	GdkPixbuf *icon_pixbuf = NULL;	
	switch (type) {
		case 0:
			if(!strcmp(item_url,"dialog"))
				icon_pixbuf =
					gdk_pixbuf_new_from_file(
					PACKAGE_PIXMAPS_DIR
					"/dlg-un.png");
			else
				icon_pixbuf =
					gdk_pixbuf_new_from_file
					(PACKAGE_PIXMAPS_DIR
					"/book-un.png");
			break;
		case 1:
			if(!strcmp(item_url,"dialog"))
				icon_pixbuf =
					gdk_pixbuf_new_from_file(
					PACKAGE_PIXMAPS_DIR
					"/dlg-bl.png");
			else
				icon_pixbuf =
					gdk_pixbuf_new_from_file
					(PACKAGE_PIXMAPS_DIR
					"/book-bl.png");
			break;
		case 2:
			if(!strcmp(item_url,"dialog"))
				icon_pixbuf =
					gdk_pixbuf_new_from_file(
					PACKAGE_PIXMAPS_DIR
					"/dlg-un.png");
			else
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-green.png");
			break;
		case 3:
			if(!strcmp(item_url,"dialog"))
				icon_pixbuf =
					gdk_pixbuf_new_from_file(
					PACKAGE_PIXMAPS_DIR
					"/dlg-un.png");
			else
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-gold.png");
			break;
		default :
			if(!strcmp(item_url,"dialog"))
				icon_pixbuf =
					gdk_pixbuf_new_from_file(
					PACKAGE_PIXMAPS_DIR
					"/dlg-un.png");
			else
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-un.png");
		
	}
	return icon_pixbuf;	
}
/******************************************************************************
 * Name
 *   changegroupnameSB 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void changegroupnameSB(gchar * groupName, gint groupNum)	
 *
 * Description
 *   change group name for group number (used to change viewer name)
 *
 * Return value
 *   void
 */

void changegroupnameSB(gchar * groupName, gint groupNum)
{
	GtkWidget *label;
	EShortcutBar *bar1;

	bar1 = E_SHORTCUT_BAR(widgets.shortcutbar);
	//g_warning(groupName);
	label = gtk_label_new(groupName);
	gtk_widget_show(label);
	e_group_bar_set_group_button_label(E_GROUP_BAR(bar1),
					   groupNum, label);
}


/******************************************************************************
 * Name
 *    
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void (EShortcutBar * shortcut_bar, gint group_num)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static GList *fill_group_list(gint group_num)
{
	GList *group_list = NULL;
	gchar *item_url, *item_name;
	gint j, number_of_items = 0;
	GString *str;
	
	number_of_items = gui_get_num_shortcut_items(group_num);
	str = g_string_new("");
	for (j = 0; j < number_of_items; j++) {
		gui_get_shortcut_item_info(group_num, j, &item_url,
				       &item_name);
		if(!strcmp(item_url,"dialog"))
			g_string_sprintf(str, "%s@dialog",item_name);
		else
			g_string_sprintf(str, "%s@main",item_name);
		
		group_list = g_list_append(group_list, (char *) g_strdup(str->str));
		//g_warning("%s", item_name);
		g_free(item_url);
		g_free(item_name);
	}
	g_string_free(str,FALSE);
	return group_list;
	
}


/******************************************************************************
 * Name
 *    
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void load_group_shortcuts_from_list(gchar * path, gint group_num)
{
	GList *tmp = NULL;
	gchar *buf;
	gchar *token;
	gchar *item_url;
	gchar *item_name;
	gchar *mod_name = NULL;
	gchar group_name[256];
	gchar icon_size[10];
	gint sbtype = 0;
	GdkPixbuf *icon_pixbuf = NULL;
	
	tmp = load_sb_group(path, group_name, icon_size); 
	if(atoi(icon_size) == 1)
		e_shortcut_bar_set_view_type((EShortcutBar *)
				     shortcut_bar,
				     group_num,
				     E_ICON_BAR_LARGE_ICONS);
	while (tmp != NULL) {
		/* seperate name and url */
		buf = (gchar *) tmp->data;
		if(!strstr(buf,"@"))
			buf = g_strconcat((gchar *) tmp->data, "@main",
                                           			NULL);
		token = strtok(buf, "@");
		item_name = g_strdup(token);
		token = strtok(NULL, "@");
		item_url = strdup(token);
		
		
		mod_name = module_name_from_description(item_name);
		sbtype = 0;
		sbtype = get_mod_type(mod_name);
		
		if (sbtype < 0)
			sbtype = 0;
		icon_pixbuf = get_pixbuf(sbtype, item_url);
		
		e_shortcut_model_add_item(E_SHORTCUT_BAR
					  (shortcut_bar)->
					  model, group_num,
					  -1, item_url,
					  item_name, icon_pixbuf);
		g_free(item_url);
		g_free(item_name);
		if(mod_name) g_free(mod_name);
		g_free((gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}
}
/******************************************************************************
 * Name
 *   savegroup 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void savegroup(EShortcutBar * shortcut_bar, gint group_num)	
 *
 * Description
 *   save shortcuts for group
 *
 * Return value
 *   void
 */

static void savegroup(EShortcutBar * shortcut_bar, gint group_num)
{
	gchar *group_name;
	GList *group_list;
	
	group_name =
	    e_shortcut_model_get_group_name(E_SHORTCUT_BAR
					    (shortcut_bar)->model,
					    group_num);
	group_list = fill_group_list(group_num);
	
	if (group_num == groupnum0) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum0) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group(group_list, "Favorites.conf",
					  group_name, "1");
		else
			save_sb_group(group_list,"Favorites.conf",
					  group_name, "0");
	}

	if (group_num == groupnum1) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum1) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group(group_list,"BibleText.conf",
					  group_name, "1");
		else
			save_sb_group(group_list,"BibleText.conf",
					  group_name, "0");
	}

	if (group_num == groupnum2) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum2) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group(group_list,"Commentaries.conf",
					  group_name, "1");
		else
			save_sb_group(group_list,"Commentaries.conf",
					  group_name, "0");
	}

	if (group_num == groupnum3) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum3) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group(group_list,"Dictionaries.conf",
					  group_name, "1");
		else
			save_sb_group(group_list,"Dictionaries.conf",
					  group_name, "0");
	}

	if (group_num == groupnum8) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum8) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group(group_list,"Books.conf", group_name,
					  "1");
		else
			save_sb_group(group_list,"Books.conf", group_name,
					  "0");
	}
}

/******************************************************************************
 * Name
 *   on_shortcut_dragged 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void on_shortcut_dragged(EShortcutBar * shortcut_bar,
 *				gint group_num, gint item_num)	
 *
 * Description
 *   shortcut being dragged
 *
 * Return value
 *   void
 */

static void on_shortcut_dragged(EShortcutBar * shortcut_bar,
				gint group_num, gint item_num)
{
	g_print("In on_shortcut_dragged Group:%i Item:%i\n",
		group_num, item_num);

	e_shortcut_model_remove_item(shortcut_bar->model, group_num,
				     item_num);
	savegroup((EShortcutBar *) shortcut_bar, group_num);
}


/******************************************************************************
 * Name
 *    on_shortcut_dropped
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void on_shortcut_dropped(EShortcutBar * shortcut_bar,
 *	    gint group_num, gint item_num, gchar * url, gchar * name)
 *
 * Description
 *   shortcut dropped
 *
 * Return value
 *   void
 */

static void on_shortcut_dropped(EShortcutBar * shortcut_bar,
	    gint group_num, gint item_num, gchar * url, gchar * name)
{
	GdkPixbuf *icon_pixbuf = NULL;

	g_print("In on_shortcut_dropped Group:%i Item:%i\n",
		group_num, item_num);
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcut_bar)->model,
				       group_num, item_num, NULL,
				       NULL, &icon_pixbuf);
	e_shortcut_model_add_item(shortcut_bar->model, group_num,
				  item_num, url, name, icon_pixbuf);
	savegroup((EShortcutBar *) shortcut_bar, group_num);
}

/******************************************************************************
 * Name
 *    on_about_item_activate
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void on_about_item_activate(GtkMenuItem * menuitem,
 *						gpointer data)	
 *
 * Description
 *   display about dialog with module info
 *
 * Return value
 *   void
 */

static void on_about_item_activate(GtkMenuItem * menuitem,
						gpointer data)
{
	gint group_num, item_num;
	gchar *item_url, *item_name;
	EShortcutBar *bar1;
	gchar *mod_name = NULL;

	item_num = GPOINTER_TO_INT(data);
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcut_bar)->model,
				       group_num, item_num, &item_url,
				       &item_name, NULL);	
	mod_name = module_name_from_description(item_name);
	gui_display_about_module_dialog(mod_name, FALSE);
	g_free(item_url);
	g_free(item_name);
	if(mod_name) g_free(mod_name);
}

/******************************************************************************
 * Name
 *    remove_all_items
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void remove_all_items(gint group_num)	
 *
 * Description
 *   remove all shortcuts in group
 *
 * Return value
 *   void
 */

static void remove_all_items(gint group_num)
{
	gint items, i;

	items = e_shortcut_model_get_num_items(shortcut_model,
					       group_num);
	for (i = items - 1; i > -1; i--) {
		e_shortcut_model_remove_item(shortcut_model,
					     group_num, i);
	}
}

/******************************************************************************
 * Name
 *   on_remove_item_activate 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void on_remove_item_activate(GtkMenuItem * menuitem,
 *						gpointer data)	
 *
 * Description
 *   remove shortcut 
 *
 * Return value
 *   void
 */

static void on_remove_item_activate(GtkMenuItem * menuitem,
						gpointer data)
{
	gint group_num, item_num;
	gchar *group_name;
	EShortcutBar *bar1;


	item_num = GPOINTER_TO_INT(data);
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));

	e_shortcut_model_remove_item(shortcut_model, group_num,
				     item_num);
	group_name =
	    e_shortcut_model_get_group_name(shortcut_model, group_num);
	
	savegroup(bar1, group_num);
/*
	if (group_num == groupnum0) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum0) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group("Favorites.conf",
					  group_name, group_num, "1");
		else
			save_sb_group("Favorites.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum1) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum1) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group("BibleText.conf",
					  group_name, group_num, "1");
		else
			save_sb_group("BibleText.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum2) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum2) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group("Commentaries.conf",
					  group_name, group_num, "1");
		else
			save_sb_group("Commentaries.conf",
					  group_name, group_num, "0");
	}

	if (group_num == groupnum3) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum3) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group("Dictionaries.conf",
					  group_name, group_num, "1");
		else
			save_sb_group("Dictionaries.conf",
					  group_name, group_num, "0");
	}*/
}

/******************************************************************************
 * Name
 *   on_add_all_activate 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void on_add_all_activate(GtkMenuItem * menuitem, gpointer user_data)	
 *
 * Description
 *   add all modules in menu to shortcut bar group
 *
 * Return value
 *   void
 */

void on_add_all_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GList *glist;
	gint group_num, sbtype;
	EShortcutBar *bar1;
	gchar *group_name;
	gchar *mod_name;
	GdkPixbuf *icon_pixbuf = NULL;

	glist = NULL;
	switch(GPOINTER_TO_INT(user_data)) {
		case TEXT_TYPE:
			glist = get_list(TEXT_DESC_LIST);
		break;
		case COMMENTARY_TYPE:
			glist = get_list(COMM_DESC_LIST);
		break;
		case DICTIONARY_TYPE:
			glist = get_list(DICT_DESC_LIST);
		break;
		case BOOK_TYPE:
			glist = get_list(GBS_DESC_LIST);
		break;
	}
	
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	group_name = "";
	group_name =
	    e_shortcut_model_get_group_name(E_SHORTCUT_BAR
					    (shortcut_bar)->model,
					    group_num);
	remove_all_items(group_num);

	while (glist != NULL) {	
		mod_name = module_name_from_description((gchar *) glist->data);
		sbtype = 0;
		sbtype = get_mod_type(mod_name);
		
		if (sbtype < 0)
			sbtype = 0;
		icon_pixbuf = get_pixbuf(sbtype, "main");

		e_shortcut_model_add_item(E_SHORTCUT_BAR
					  (shortcut_bar)->model,
					  group_num, -1, "main",
					  (gchar *) glist->data,
					  icon_pixbuf);
		glist = g_list_next(glist);
	}
	//g_list_free(glist);
	savegroup(E_SHORTCUT_BAR(shortcut_bar), group_num);
	if(mod_name) g_free(mod_name);
}

/******************************************************************************
 * Name
 *   on_add_shortcut_activate 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void on_add_shortcut_activate(GtkMenuItem * menuitem, 
 *						gpointer user_data)
 *
 * Description
 *   add a shortcut item
 *
 * Return value
 *   void
 */

static void on_add_shortcut_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{
	//gchar *group_name;
	gint group_num;
	EShortcutBar *bar1;

	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	
	gui_add_sb_item("main", (gchar *) user_data, group_num);
}

/******************************************************************************
 * Name
 *   add_sb_group 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   gint add_sb_group(EShortcutBar * shortcut_bar,
					gchar * group_name)	
 *
 * Description
 *   add group to shourtcut bar
 *
 * Return value
 *   gint
 */

static gint add_sb_group(EShortcutBar * shortcut_bar,
					gchar * group_name)
{
	gint group_num;

	group_num =
	    e_shortcut_model_add_group(shortcut_bar->model, -1,
				       group_name);
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	return group_num;
}


/******************************************************************************
 * Name
 *   gui_add_sb_item 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void gui_add_sb_item(gchar * url, gchar * name,
 *					gint group_num)
 *
 * Description
 *   add item to shourtcut bar
 *
 * Return value
 *   gint
 */

void gui_add_sb_item(gchar * url, gchar * name,
					gint group_num)
{
	
	EShortcutBar *bar1;
	GdkPixbuf *icon_pixbuf = NULL;
	gchar *mod_name = NULL;
	gint sbtype;
	
	mod_name = module_name_from_description(name);
	sbtype = 0;
	sbtype = get_mod_type(mod_name);
	
	if (sbtype < 0)
		sbtype = 0;
	
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	icon_pixbuf = get_pixbuf(sbtype, url);
	e_shortcut_model_add_item(bar1->
				  model, group_num, -1, url,
				  name, icon_pixbuf);
	savegroup(bar1, group_num);
	if(mod_name) g_free(mod_name);
}


/******************************************************************************
 * Name
 *   set_large_icons 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void set_large_icons(GtkWidget * menuitem, 
				EShortcutBar * shortcut_bar)	
 *
 * Description
 *   set shortcut icons to large for  current group
 *
 * Return value
 *   void
 */

static void set_large_icons(GtkWidget * menuitem, 
				EShortcutBar * shortcut_bar)
{
	GtkWidget *menu;
	gint group_num;

	menu = menuitem->parent;
	g_return_if_fail(GTK_IS_MENU(menu));

	group_num =
	    GPOINTER_TO_INT(gtk_object_get_data
			    (GTK_OBJECT(menu), "group_num"));
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_LARGE_ICONS);
	if (group_num == groupnum0)
		save_sb_iconsize("Favorites.conf", "1");
	if (group_num == groupnum1)
		save_sb_iconsize("BibleText.conf", "1");
	if (group_num == groupnum2)
		save_sb_iconsize("Commentaries.conf", "1");
	if (group_num == groupnum3)
		save_sb_iconsize("Dictionaries.conf", "1");
}

/******************************************************************************
 * Name
 *    set_small_icons
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void set_small_icons(GtkWidget * menuitem, 
 *				EShortcutBar * shortcut_bar)
 *
 * Description
 *   set shortcut icons to small for  current group
 *
 * Return value
 *   void
 */

static void set_small_icons(GtkWidget * menuitem, 
				EShortcutBar * shortcut_bar)
{
	GtkWidget *menu;
	gint group_num;

	menu = menuitem->parent;
	g_return_if_fail(GTK_IS_MENU(menu));

	group_num =
	    GPOINTER_TO_INT(gtk_object_get_data
			    (GTK_OBJECT(menu), "group_num"));
	e_shortcut_bar_set_view_type(shortcut_bar, group_num,
				     E_ICON_BAR_SMALL_ICONS);
	if (group_num == groupnum0)
		save_sb_iconsize("Favorites.conf", "0");
	if (group_num == groupnum1)
		save_sb_iconsize("BibleText.conf", "0");
	if (group_num == groupnum2)
		save_sb_iconsize("Commentaries.conf", "0");
	if (group_num == groupnum3)
		save_sb_iconsize("Dictionaries.conf", "0");
}

/******************************************************************************
 * Name
 *    show_standard_popup
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void show_standard_popup(EShortcutBar * shortcut_bar,
 *				GdkEvent * event, gint group_num)	
 *
 * Description
 *   create popup menu for shortcut bar group
 *
 * Return value
 *   void
 */

static void show_standard_popup(EShortcutBar * shortcut_bar,
				GdkEvent * event, gint group_num)
{
	GtkWidget *menu, *menuitem, *menu_item_menu;

	/* We don't have any commands if there aren't any groups yet. */
	if (group_num == -1)
		return;

	menu = gtk_menu_new();

	menuitem = gtk_menu_item_new_with_label(_("Large Icons"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(set_large_icons),
			   shortcut_bar);

	menuitem = gtk_menu_item_new_with_label(_("Small Icons"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(set_small_icons),
			   shortcut_bar);

	menuitem = gtk_menu_item_new();
	gtk_widget_set_sensitive(menuitem, FALSE);
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);

	if (group_num == groupnum0) {
		menuitem =
		    gtk_menu_item_new_with_label(_("Add Bible Text"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, TEXT_TYPE);

		menuitem =
		    gtk_menu_item_new_with_label(_("Add Commentary"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, COMMENTARY_TYPE);


		menuitem =
		    gtk_menu_item_new_with_label(_("Add Dictionary"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, DICTIONARY_TYPE);
		

		menuitem = gtk_menu_item_new_with_label(_("Add Book"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, BOOK_TYPE);	
	}

	else {
		menuitem =
		    gtk_menu_item_new_with_label(_("Add Shortcut"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
	/*** add bookmark items ***/
		if (group_num == groupnum1)
			gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, TEXT_TYPE);

		if (group_num == groupnum2)
			gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, COMMENTARY_TYPE);

		if (group_num == groupnum3)
			gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, DICTIONARY_TYPE);	

		if (group_num == groupnum8)
			gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, BOOK_TYPE);

	}
	/* Save the group num so we can get it in the callbacks. */
	gtk_object_set_data(GTK_OBJECT(menu), "group_num",
			    GINT_TO_POINTER(group_num));

	/* FIXME: Destroy menu when finished with it somehow? */
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
		       event->button.button, event->button.time);
}

/******************************************************************************
 * Name
 *    show_context_popup
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   	void show_context_popup(EShortcutBar * shortcut_bar,
 *		   GdkEvent * event, gint group_num, gint item_num)
 *
 * Description
 *   create popup menu for shortcut bar item
 *
 * Return value
 *   void
 */

static void show_context_popup(EShortcutBar * shortcut_bar,
		   GdkEvent * event, gint group_num, gint item_num)
{
	GtkWidget *menu, *menuitem;

	menu = gtk_menu_new();

	menuitem = gtk_menu_item_new_with_label(_("About this Module"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC(on_about_item_activate),
			   GINT_TO_POINTER(item_num));

	menuitem = gtk_menu_item_new();
	gtk_widget_set_sensitive(menuitem, FALSE);
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);

	menuitem =
	    gtk_menu_item_new_with_label(_("Remove from Shortcut Bar"));
	gtk_widget_show(menuitem);
	gtk_menu_append(GTK_MENU(menu), menuitem);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate",
			   GTK_SIGNAL_FUNC
			   (on_remove_item_activate),
			   GINT_TO_POINTER(item_num));

	/* Save the group & item nums so we can get them in the callbacks. */
	gtk_object_set_data(GTK_OBJECT(menu), "group_num",
			    GINT_TO_POINTER(group_num));
	gtk_object_set_data(GTK_OBJECT(menu), "item_num",
			    GINT_TO_POINTER(item_num));

	/* FIXME: Destroy menu when finished with it somehow? */
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
		       event->button.button, event->button.time);
}

/******************************************************************************
 * Name
 *    on_shortcut_bar_item_selected
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   	void on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
 *			GdkEvent * event,gint group_num, gint item_num)
 *
 * Description
 *   for any shortcut bar item clicked or group button
 *
 * Return value
 *   void
 */

static void on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
			GdkEvent * event,gint group_num, gint item_num)
{
	gchar *item_url, *item_name;
	gchar *mod_name = NULL;
	gint remItemNum = 0;
	gint sbtype;
	GdkPixbuf *icon_pixbuf = NULL;

	remItemNum = item_num;


	if (event->button.button == 1) {
		if (item_num > -1) {
			e_shortcut_model_get_item_info(E_SHORTCUT_BAR
						       (shortcut_bar)->
						       model,
						       group_num,
						       item_num,
						       &item_url, 
						       &item_name,
						       &icon_pixbuf);
			
			mod_name = module_name_from_description(item_name);
			
			if(!strcmp(item_url,"main")){	
				if (group_num == groupnum0) {
					sbtype = get_mod_type(mod_name);
					if (sbtype == 0 || sbtype == 1)
						gui_change_module_and_key(mod_name,
								  settings.currentverse);
					else if (sbtype == 3) {
						gtk_notebook_set_page
						    (GTK_NOTEBOOK
						     (widgets.workbook_lower), 1);
						gui_change_module_and_key(mod_name,
								  NULL);
					}
	
					else
						gui_change_module_and_key(mod_name,
								  settings.dictkey);
				}
	
				if (group_num == groupnum1) {
					if (settings.havebible) {
						gui_change_module_and_key(mod_name,
								  settings.currentverse);
					}
				}
	
				if (group_num == groupnum2) {
					if (settings.havecomm) {
						gui_change_module_and_key(mod_name,
								  settings.currentverse);
						/*gtk_notebook_set_page
						    (GTK_NOTEBOOK
						     (widgets.workbook_upper), 0);*/
					}
				}
	
				if (group_num == groupnum3) {
					if (settings.havedict) {
						gui_change_module_and_key(mod_name,
								  settings.dictkey);
						gtk_notebook_set_page
						    (GTK_NOTEBOOK
						     (widgets.workbook_lower), 0);
					}
				}
	
				if (group_num == groupnum4) {
					gui_change_verse(item_name);
				}
	
				if (group_num == groupnum8) {
					gui_change_module_and_key(mod_name, NULL);
					gtk_notebook_set_page(GTK_NOTEBOOK
							      (widgets.workbook_lower),
							      1);
				}
			}
			else {
				sbtype = get_mod_type(mod_name);
				switch(sbtype) {
					case 0: gui_open_bibletext_dialog(mod_name);
						break;
					case 1: gui_open_commentary_dialog(mod_name);
						break;
					case 2: gui_open_dictlex_dialog(mod_name);
						break;
					case 3: gui_open_gbs_dialog(mod_name);
						break;
				}
			}
			g_free(item_url);
			g_free(item_name);
		}
	} 
	else if (event->button.button == 3) {
		if (remItemNum == -1)
			show_standard_popup(shortcut_bar, event,
					    group_num);
		else
			show_context_popup(shortcut_bar, event,
					   group_num, item_num);
	}
	if(mod_name) g_free(mod_name);
}

/******************************************************************************
 * Name
 *    gui_get_num_shortcut_items
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   gint gui_get_num_shortcut_items(gint group_num)	
 *
 * Description
 *   returns the number of shortcut items in the current group
 *
 * Return value
 *   gint
 */

gint gui_get_num_shortcut_items(gint group_num)
{
	return e_shortcut_model_get_num_items(E_SHORTCUT_BAR
					      (widgets.shortcutbar)->
					      model, group_num);

}

/******************************************************************************
 * Name
 *    gui_get_shortcut_item_info
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void gui_get_shortcut_item_info(gint group_num, gint item_num, 
 *				gchar **item_url, gchar **item_name)	
 *
 * Description
 *   get shortcut item information
 *
 * Return value
 *   void
 */

void gui_get_shortcut_item_info(gint group_num, gint item_num, 
				gchar **item_url, gchar **item_name)
{
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				(widgets.shortcutbar)->model,
				group_num,
				item_num,
				item_url, item_name, NULL);
}


/******************************************************************************
 * Name
 *    gui_setup_shortcut_bar
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void gui_setup_shortcut_bar(void)
 *
 * Description
 *   setup the main shortcut bar
 *
 * Return value
 *   void
 */

void gui_setup_shortcut_bar(void)
{
	GtkWidget
	    * button,
	    *searchbutton,
	    *ctree,
	    *scrolledwindow1,
	    *scrolledwindow2,
	    *vpSearch, *vboxVL, *vpVL, *html, *VLbutton;
	gchar *text;

	/*
	 * create shortcut bar 
	 */
	shortcut_model = e_shortcut_model_new();

	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				 shortcut_model);
	if(settings.showshortcutbar)
		gtk_widget_show(shortcut_bar);
	else
		gtk_widget_hide(shortcut_bar);

	widgets.shortcutbar = shortcut_bar;

	e_paned_pack1(E_PANED(widgets.epaned), shortcut_bar, FALSE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(shortcut_bar), 4);
	/*
	 * shortcut bar created now add groups and shortcuts
	*/	

	/* Favorites */
	if (settings.showfavoritesgroup) {
		text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("Favorites.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"Favorites.conf");
		groupnum0 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Favorites"));
		
		load_group_shortcuts_from_list(text, groupnum0);
		g_free(text);
	}
	
	/* bibletext */
	if (settings.showtextgroup) {
		text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("BibleText.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"BibleText.conf");		
		groupnum1 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Bible Text"));		
		load_group_shortcuts_from_list(text, groupnum1);
		g_free(text);		
	}
	
	/* commentary */
	if (settings.showcomgroup) {
		text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("Commentaries.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"Commentaries.conf");	
		groupnum2 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Commentaries"));		
		load_group_shortcuts_from_list(text, groupnum2);
		g_free(text);		
	}
	
	/* dictionary/lexicon */
	if (settings.showdictgroup) {
		text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("Dictionaries.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"Dictionaries.conf");
		groupnum3 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 e_utf8_from_locale_string(_
							   ("Dict/Lex")));
		load_group_shortcuts_from_list(text, groupnum3);
		g_free(text);
	}

	/* GBS */
	if (settings.showbookgroup) {
		text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("Books.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"Books.conf");
		groupnum8 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Book"));
		load_group_shortcuts_from_list(text, groupnum8);
		g_free(text);
	}

	if (settings.showhistorygroup) {
		groupnum4 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("History"));
	}

	/*** add bookmark group to shortcut bar ***/
	scrolledwindow1 = e_vscrolled_bar_new(NULL);
	gtk_widget_show(scrolledwindow1);

	ctree = gtk_ctree_new(3, 0);
	gtk_widget_show(ctree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), ctree);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 0, 280);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 1, 80);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 2, 80);

	widgets.ctree_widget = ctree;

	button = gtk_button_new_with_label(_("Bookmarks"));
	gtk_widget_show(button);
	groupnum5 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
				  scrolledwindow1, button, -1);

	gui_load_bookmark_tree();

	scrolledwindow2 = e_vscrolled_bar_new(NULL);
	gtk_widget_show(scrolledwindow2);

	vpSearch = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(vpSearch);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), vpSearch);

	searchbutton = gtk_button_new_with_label(_("Search"));
	gtk_widget_show(searchbutton);

	gui_create_shortcutbar_search(vpSearch);

	groupnum6 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
				  scrolledwindow2, searchbutton, -1);
	settings.searchbargroup = groupnum6;

	/*********************************************************************/

	vboxVL = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vboxVL);

	vpVL = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(vpVL);
	gtk_container_add(GTK_CONTAINER(vpVL), vboxVL);

	VLbutton = gtk_button_new_with_label(_("Verse List"));
	gtk_widget_show(VLbutton);

	/*** create viewer group and add to shortcut bar ****/
	html = gui_create_shortcutbar_viewer(vboxVL);
	
	groupnum7 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar), vpVL,
				  VLbutton, -1);
				  
	e_shortcut_bar_set_enable_drags((EShortcutBar *)
					shortcut_bar, TRUE);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar),
			   "item_selected",
			   GTK_SIGNAL_FUNC
			   (on_shortcut_bar_item_selected), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar),
			   "shortcut_dragged",
			   GTK_SIGNAL_FUNC(on_shortcut_dragged), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_bar),
			   "shortcut_dropped",
			   GTK_SIGNAL_FUNC(on_shortcut_dropped), NULL);
}

/******************************************************************************
 * Name
 *    gui_update_shortcut_bar
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void gui_update_shortcut_bar(void)	
 *
 * Description
 *   called by preferences dialog to update user choices
 *
 * Return value
 *   void
 */

void gui_update_shortcut_bar(void)
{
	gint count, i, current_group;
	EShortcutBar *bar;
	
	groupnum0 = -1;
	groupnum1 = -1; 
	groupnum2 = -1; 
	groupnum3 = -1; 
	groupnum4 = -1; 
	groupnum8 = -1;	
	
	bar = E_SHORTCUT_BAR(shortcut_bar);
	current_group =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar));
	count =
	    e_shortcut_model_get_num_groups(E_SHORTCUT_MODEL
					    (shortcut_model));
	/*** remove shortcutbar groups so we can 
	     load the one the user has sellected ***/
	for (i = count - 1; i > -1; i--) {
		e_shortcut_model_remove_group(E_SHORTCUT_MODEL
					      (shortcut_model), i);
	}
	
	if (settings.showfavoritesgroup) {
		gchar *text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("Favorites.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"Favorites.conf");
		groupnum0 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Favorites"));
		
		load_group_shortcuts_from_list(text, groupnum0);
		g_free(text);		
	}
	
	if (settings.showtextgroup) {
		gchar *text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("BibleText.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"BibleText.conf");
		groupnum1 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Bible Text"));
		
		load_group_shortcuts_from_list(text, groupnum1);
		g_free(text);
	}
	
	if (settings.showcomgroup) {
		gchar *text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("Commentaries.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"Commentaries.conf");		
		groupnum2 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Commentaries"));
		load_group_shortcuts_from_list(text, groupnum2);
		g_free(text);
	}
	
	if (settings.showdictgroup) {
		gchar *text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("Dictionaries.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"Dictionaries.conf");
		groupnum3 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 e_utf8_from_locale_string(_
							("Dict/Lex")));
		load_group_shortcuts_from_list(text, groupnum3);		
		g_free(text);
	}

	/* GBS */
	if (settings.showbookgroup) {
		gchar *text = g_new(char, strlen(settings.shortcutbarDir) +
				strlen("Books.conf") + 3);
		sprintf(text, "%s/%s", settings.shortcutbarDir,
				"Books.conf");
		groupnum8 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Book"));
		load_group_shortcuts_from_list(text, groupnum8);
		g_free(text);
	}

	if (settings.showhistorygroup) {
		groupnum4 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("History"));
	}
	
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar),
					  current_group, TRUE);
	count =
	    e_shortcut_model_get_num_groups(E_SHORTCUT_MODEL
					    (shortcut_model));
	/* set group numbers that were added manually */
	groupnum5 = count; 
	groupnum6 = count + 1; 
	groupnum7 = count + 2; 
	
}

/******************************************************************************
 * Name
 *    gui_create_mod_list_menu
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void gui_create_mod_list_menu(gint group_num, GtkWidget * menu,
		      GtkWidget * shortcut_menu_widget, gint mod_type)	
 *
 * Description
 *   creates a menu with a list of modules for adding shortcuts
 *
 * Return value
 *   void
 */

void gui_create_mod_list_menu(gint group_num, GtkWidget * menu,
		      GtkWidget * shortcut_menu_widget, gint mod_type)
{
	GtkWidget *item;
	GList *glist = NULL;
		
	if (group_num != groupnum0) {
		item =
		    gtk_menu_item_new_with_label(_("Add All Modules"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER
				  (shortcut_menu_widget), item);

		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC
				   (on_add_all_activate),
				   GINT_TO_POINTER(mod_type));
	}

	
	switch(mod_type) {
		case TEXT_TYPE:
			glist = get_list(TEXT_DESC_LIST);
		break;
		case COMMENTARY_TYPE:
			glist = get_list(COMM_DESC_LIST);
		break;
		case DICTIONARY_TYPE:
			glist = get_list(DICT_DESC_LIST);
		break;
		case BOOK_TYPE:
			glist = get_list(GBS_DESC_LIST);
		break;
	}
		
	while (glist != NULL) {
		item =
		    gtk_menu_item_new_with_label((gchar *) glist->data);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER
				  (shortcut_menu_widget), item);
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC
				   (on_add_shortcut_activate),
				   (gchar *) glist->data);
		glist = g_list_next(glist);
	}
	//g_list_free(glist);
}

/******************************************************************************
 * Name
 *    gui_shortcutbar_showhide
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void gui_shortcutbar_showhide()	
 *
 * Description
 *   Show/hide shortcutbar
 *
 * Return value
 *   void
 */

void gui_shortcutbar_showhide(void)
{
	if (!settings.docked) {
		gdk_window_raise(GTK_WIDGET(widgets.dock_sb)->window);
		return;
	}

	if (settings.showshortcutbar) {
		settings.showshortcutbar = FALSE;
		settings.biblepane_width = settings.gs_width / 2;
		gtk_widget_hide(widgets.shortcutbar);
		e_paned_set_position(E_PANED(widgets.epaned), 0);
		e_paned_set_position(E_PANED
				     (widgets.hpaned),
				     settings.biblepane_width);
	} else {
		settings.showshortcutbar = TRUE;
		settings.biblepane_width =
		    (settings.gs_width - settings.shortcutbar_width) / 2;
		e_paned_set_position(E_PANED(widgets.epaned),
				     settings.shortcutbar_width);
		e_paned_set_position(E_PANED
				     (widgets.hpaned),
				     settings.biblepane_width);
		gtk_widget_show(widgets.shortcutbar);
	}
}


/******************************************************************************
 * Name
 *   gui_set_shortcutbar_porgram_start 
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void gui_set_shortcutbar_porgram_start(void)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_set_shortcutbar_porgram_start(void)
{
	/*
	 *  show hide shortcut bar - set to options setting 
	 */
	if (settings.showshortcutbar) {
		gtk_widget_show(widgets.shortcutbar);
		e_paned_set_position(E_PANED(widgets.epaned),
				     settings.shortcutbar_width);
	}

	else if (!settings.showshortcutbar && settings.showdevotional) {
		gtk_widget_show(widgets.shortcutbar);
		gui_shortcutbar_showhide();
	}

	else {
		gtk_widget_hide(widgets.shortcutbar);
		e_paned_set_position(E_PANED(widgets.epaned),
				     1);
	}

	/* set hight of bible and commentary pane */
	e_paned_set_position(E_PANED(widgets.vpaned),
			     settings.upperpane_hight);

	/* set width of bible pane */
	e_paned_set_position(E_PANED(widgets.hpaned),
			     settings.biblepane_width);

	if (!settings.docked) {
		settings.docked = TRUE;
		gui_attach_detach_shortcutbar();
	}
}
