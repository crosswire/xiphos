/*
 * GnomeSword Bible Study Tool
 * shortcutbar_main.c - create and maintain the main shortcut bar
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
#include <gtkhtml/gtkhtml.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#include <gal/shortcut-bar/e-vscrolled-bar.h>
#include <gal/widgets/e-unicode.h>

#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_dialog.h"
#include "gui/shortcutbar_search.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/_editor.h"
#include "gui/bookmarks.h"
#include "gui/utilities.h"
#include "gui/html.h"
#include "gui/about_modules.h"

#include "main/gs_gnomesword.h"
#include "main/gbs.h"
#include "main/settings.h"
#include "main/lists.h"

extern SB_VIEWER sb_v, *sv ;

extern gchar *shortcut_types[];
extern gboolean havedict;	/* let us know if we have at least one lex/dict module */
extern gboolean havecomm;	/* let us know if we have at least one commentary module */
extern gboolean havebible;	/* let us know if we have at least one Bible text module */


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
		e_paned_set_position(E_PANED(settings.epaned),
				     settings.shortcutbar_width);
		e_paned_set_position(E_PANED
				     (gui_lookup_widget
				      (settings.app, "hpaned1")),
				     biblepanesize);
		settings.showshortcutbar = TRUE;
	}
	bar1 = E_SHORTCUT_BAR(settings.shortcut_bar);
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar1),
					  groupnum, TRUE);
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

	bar1 = E_SHORTCUT_BAR(settings.shortcut_bar);
	//g_warning(groupName);
	label = gtk_label_new(groupName);
	gtk_widget_show(label);
	e_group_bar_set_group_button_label(E_GROUP_BAR(bar1),
					   groupNum, label);
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

	group_name =
	    e_shortcut_model_get_group_name(E_SHORTCUT_BAR
					    (shortcut_bar)->model,
					    group_num);

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
	}

	if (group_num == groupnum8) {
		if (e_shortcut_bar_get_view_type
		    (E_SHORTCUT_BAR(shortcut_bar),
		     groupnum8) == E_ICON_BAR_LARGE_ICONS)
			save_sb_group("Books.conf", group_name,
					  group_num, "1");
		else
			save_sb_group("Books.conf", group_name,
					  group_num, "0");
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
	gchar modname[16];

	item_num = GPOINTER_TO_INT(data);
	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				       (shortcut_bar)->model,
				       group_num, item_num, &item_url,
				       &item_name, NULL);	
	memset(modname, 0, 16);
	module_name_from_description(modname, item_name);
	gui_display_about_module_dialog(modname, FALSE);
	if(modname) g_free(modname);
	g_free(item_url);
	g_free(item_name);
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
	}
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
	gint number_of_items = 0, group_num, sbtype;
	EShortcutBar *bar1;
	gchar *group_name, modName[16];
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
		memset(modName, 0, 16);
		module_name_from_description(modName, (gchar *) glist->data);
		sbtype = 0;
		sbtype = get_mod_type(modName);
		
		if (sbtype < 0)
			sbtype = 0;

		switch (sbtype) {
		case 0:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-un.png");
			break;
		case 1:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-bl.png");
			break;
		case 2:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-green.png");
			break;
		case 3:
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-gold.png");
			break;
		}

		e_shortcut_model_add_item(E_SHORTCUT_BAR
					  (shortcut_bar)->model,
					  group_num, -1, "not null",
					  (gchar *) glist->data,
					  icon_pixbuf);
		++number_of_items;
		glist = g_list_next(glist);
	}
	g_list_free(glist);
	savegroup(E_SHORTCUT_BAR(shortcut_bar), group_num);
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
	gchar *group_name;
	gint group_num, sbtype;
	EShortcutBar *bar1;
	gchar modName[16];
	GdkPixbuf *icon_pixbuf = NULL;

	bar1 = E_SHORTCUT_BAR(shortcut_bar);
	group_num =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar1));
	memset(modName, 0, 16);
	module_name_from_description(modName, (gchar *) user_data);
	sbtype = 0;
	sbtype = get_mod_type(modName);
	
	if (sbtype < 0)
		sbtype = 0;

	switch (sbtype) {
	case 0:
		icon_pixbuf =
		    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					     "/book-un.png");
		break;
	case 1:
		icon_pixbuf =
		    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					     "/book-bl.png");
		break;
	case 2:
		icon_pixbuf =
		    gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					     "/book-green.png");
		break;
	}
	e_shortcut_model_add_item(E_SHORTCUT_BAR(shortcut_bar)->
				  model, group_num, -1, "not null",
				  (gchar *) user_data, icon_pixbuf);

	group_name = "";
	group_name = e_shortcut_model_get_group_name(E_SHORTCUT_BAR
						     (shortcut_bar)->
						     model, group_num);

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
	}
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
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, TEXT_TYPE);

		menuitem =
		    gtk_menu_item_new_with_label(_("Add Commentary"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, COMMENTARY_TYPE);


		menuitem =
		    gtk_menu_item_new_with_label(_("Add Dictionary"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),
					  menu_item_menu);
		gui_create_mod_list_menu(group_num, menuitem, menu_item_menu, DICTIONARY_TYPE);
		

		menuitem = gtk_menu_item_new_with_label(_("Add Book"));
		gtk_widget_show(menuitem);
		gtk_menu_append(GTK_MENU(menu), menuitem);

		menu_item_menu = gtk_menu_new();
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
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
		gtk_widget_ref(menu_item_menu);
		gtk_object_set_data_full(GTK_OBJECT(menu),
					 "menu_item_menu",
					 menu_item_menu,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
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
	GtkWidget *app;
	gchar *type, *ref;
	gchar modName[16];
	gint remItemNum = 0;
	GdkPixbuf *icon_pixbuf = NULL;

	remItemNum = item_num;

//      if(item_num == -1) {
//              if(group_num == groupnum2) /* change work space notebook to commentary page */
//                      gtk_notebook_set_page(GTK_NOTEBOOK(settings.workbook_upper),0);
//              if(group_num == groupnum3) /* change Dictionayr - Books notebook to Dict page */
//                      gtk_notebook_set_page(GTK_NOTEBOOK(settings.workbook_lower),0);
//              if(group_num == groupnum8) /* change Dictionayr - Books notebook to Dict page */
//                      gtk_notebook_set_page(GTK_NOTEBOOK(settings.workbook_lower),1);
//      }

	if (event->button.button == 1) {
		if (item_num > -1) {
			app =
			    gtk_widget_get_toplevel(GTK_WIDGET
						    (shortcut_bar));
			e_shortcut_model_get_item_info(E_SHORTCUT_BAR
						       (shortcut_bar)->
						       model,
						       group_num,
						       item_num,
						       &type, &ref,
						       &icon_pixbuf);
			
			
			memset(modName, 0, 16);
			module_name_from_description(modName, ref);

			if (group_num == groupnum0) {
				gint sbtype;
				sbtype = get_mod_type(modName);
				if (sbtype == 0 || sbtype == 1)
					change_module_and_key(modName,
							  settings.currentverse);
				else if (sbtype == 3) {
					gtk_notebook_set_page
					    (GTK_NOTEBOOK
					     (settings.workbook_lower), 1);
					change_module_and_key(modName,
							  NULL);
				}

				else
					change_module_and_key(modName,
							  settings.dictkey);
			}

			if (group_num == groupnum1) {
				if (havebible) {
					change_module_and_key(modName,
							  settings.currentverse);
				}
			}

			if (group_num == groupnum2) {
				if (havecomm) {
					change_module_and_key(modName,
							  settings.currentverse);
					gtk_notebook_set_page
					    (GTK_NOTEBOOK
					     (settings.workbook_upper), 0);
				}
			}

			if (group_num == groupnum3) {
				if (havedict) {
					change_module_and_key(modName,
							  settings.dictkey);
					gtk_notebook_set_page
					    (GTK_NOTEBOOK
					     (settings.workbook_lower), 0);
				}
			}

			if (group_num == groupnum4) {
				change_verse(ref);
			}

			if (group_num == groupnum8) {
				change_module_and_key(modName, NULL);
				gtk_notebook_set_page(GTK_NOTEBOOK
						      (settings.workbook_lower),
						      1);
			}
			g_free(type);
			g_free(ref);
		}
	} else if (event->button.button == 3) {
		if (remItemNum == -1)
			show_standard_popup(shortcut_bar, event,
					    group_num);
		else
			show_context_popup(shortcut_bar, event,
					   group_num, item_num);
	}

}

/******************************************************************************
 * Name
 *    gui_get_num_shortcut_items
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   gint gui_get_num_shortcut_items(GtkWidget * shortcutbar_widget,
						gint group_num)	
 *
 * Description
 *   returns the number of shortcut items in the current group
 *
 * Return value
 *   gint
 */

gint gui_get_num_shortcut_items(GtkWidget * shortcutbar_widget,
						gint group_num)
{
	return e_shortcut_model_get_num_items(E_SHORTCUT_BAR
					      (shortcutbar_widget)->
					      model, group_num);

}

/******************************************************************************
 * Name
 *    gui_get_shortcut_item_info
 *
 * Synopsis
 *   #include "shortcutbar_main.h"
 *
 *   void gui_get_shortcut_item_info(GtkWidget *shortcutbar_widget, 
 *    gint group_num, gint item_num, gchar **item_url, gchar **item_name)	
 *
 * Description
 *   get shortcut item information
 *
 * Return value
 *   void
 */

void gui_get_shortcut_item_info(GtkWidget *shortcutbar_widget, 
     gint group_num, gint item_num, gchar **item_url, gchar **item_name)
{
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR
				(shortcutbar_widget)->model,
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
	GList *tmp;
	GtkWidget
	    * button,
	    *searchbutton,
	    *ctree,
	    *scrolledwindow1,
	    *scrolledwindow2,
	    *vpSearch, *vboxVL, *vpVL, *html, *VLbutton;
	gint sbtype = 0, large_icons = 0;
	gchar *filename, group_name[256], icon_size[10], modName[16];
	GdkPixbuf *icon_pixbuf = NULL;

	/*
	 * shortcut bar 
	 */
	shortcut_model = e_shortcut_model_new();

	shortcut_bar = e_shortcut_bar_new();
	e_shortcut_bar_set_model(E_SHORTCUT_BAR(shortcut_bar),
				 shortcut_model);
	if(settings.showshortcutbar)
		gtk_widget_show(shortcut_bar);
	else
		gtk_widget_hide(shortcut_bar);

	settings.shortcut_bar = shortcut_bar;

	e_paned_pack1(E_PANED(settings.epaned), shortcut_bar, FALSE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(shortcut_bar), 4);
	/*
	 * end shortcut bar 
	*/
	

	tmp = NULL;
	if (settings.showfavoritesgroup) {
		groupnum0 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Favorites"));
		filename = "Favorites.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		
		//g_warning("group = %s",group_name);
		//g_warning("size  = %s",icon_size);
		
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum0,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			memset(modName, 0, 16);
			module_name_from_description(modName, (gchar *) tmp->data);
			sbtype = 0;
			sbtype = get_mod_type(modName);
			
			if (sbtype < 0)
				sbtype = 0;
			switch (sbtype) {
			case 0:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-un.png");
				break;
			case 1:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-bl.png");
				break;
			case 2:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-green.png");
				break;
			case 3:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-gold.png");
				break;
			default :
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-un.png");
			
			}
			
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum0,
						  -1, "favorite",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	g_list_free(tmp);
	tmp = NULL;
	
	if (settings.showtextgroup) {
		groupnum1 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Bible Text"));
		filename = "BibleText.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum1,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-un.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum1,
						  -1, "bible text",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (settings.showcomgroup) {
		groupnum2 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Commentaries"));
		filename = "Commentaries.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum2,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-bl.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum2,
						  -1, "commentary",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (settings.showdictgroup) {
		groupnum3 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 e_utf8_from_locale_string(_
							   ("Dict/Lex")));
		filename = "Dictionaries.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum3,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-green.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum3,
						  -1, "dictionary",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}

	/* GBS */
	if (settings.showbookgroup) {
		groupnum8 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Book"));
		filename = "Books.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum8,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-gold.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum8,
						  -1, "book",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	g_list_free(tmp);

	if (settings.showhistorygroup) {
		groupnum4 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("History"));
	}

	/*** add bookmark group to shortcut bar ***/
	scrolledwindow1 = e_vscrolled_bar_new(NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindow1", scrolledwindow1,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);

	ctree = gtk_ctree_new(3, 0);
	gtk_widget_ref(ctree);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "ctree",
				 ctree, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(ctree);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), ctree);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 0, 280);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 1, 80);
	gtk_clist_set_column_width(GTK_CLIST(ctree), 2, 80);

	settings.ctree_widget = ctree;

	button = gtk_button_new_with_label(_("Bookmarks"));
	gtk_widget_ref(button);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "button",
				 button, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(button);
	groupnum5 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
				  scrolledwindow1, button, -1);

	gui_load_bookmark_tree();

	scrolledwindow2 = e_vscrolled_bar_new(NULL);
	gtk_widget_ref(scrolledwindow2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindow2", scrolledwindow2,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow2);

	vpSearch = gtk_viewport_new(NULL, NULL);
	gtk_widget_ref(vpSearch);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vpSearch",
				 vpSearch, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vpSearch);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), vpSearch);

	searchbutton = gtk_button_new_with_label(_("Search"));
	gtk_widget_ref(searchbutton);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "searchbutton", searchbutton,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(searchbutton);

	gui_create_shortcutbar_search(vpSearch);

	groupnum6 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
				  scrolledwindow2, searchbutton, -1);
	settings.searchbargroup = groupnum6;

	/*********************************************************************/

	vboxVL = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vboxVL);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vboxVL",
				 vboxVL, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vboxVL);

	vpVL = gtk_viewport_new(NULL, NULL);
	gtk_widget_ref(vpVL);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vpVL", vpVL,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vpVL);
	gtk_container_add(GTK_CONTAINER(vpVL), vboxVL);

	VLbutton = gtk_button_new_with_label(_("Verse List"));
	gtk_widget_ref(VLbutton);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "VLbutton",
				 VLbutton, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(VLbutton);

	/*** create viewer group and add to shortcut bar ****/
	html = gui_create_shortcutbar_viewer(vboxVL);
	
	groupnum7 =
	    e_group_bar_add_group(E_GROUP_BAR(shortcut_bar), vpVL,
				  VLbutton, -1);
	e_shortcut_bar_set_enable_drags((EShortcutBar *)
					shortcut_bar, TRUE);/*
	gtk_signal_connect(GTK_OBJECT(VLbutton), "clicked",
			   GTK_SIGNAL_FUNC(on_VLbutton_clicked), NULL);*/
	gtk_signal_connect(GTK_OBJECT(shortcut_bar),
			   "item_selected",
			   GTK_SIGNAL_FUNC
			   (on_shortcut_bar_item_selected), NULL);
			   /*
	gtk_signal_connect(GTK_OBJECT(shortcut_model),
			   "item_added",
			   GTK_SIGNAL_FUNC(on_item_added), NULL);
	gtk_signal_connect(GTK_OBJECT(shortcut_model),
			   "item_removed",
			   GTK_SIGNAL_FUNC(on_item_removed), NULL);
			   */
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
	gint count, i, large_icons, current_group, sbtype = 0;
	GList *tmp;
	EShortcutBar *bar;
	gchar *filename, group_name[256], icon_size[10];
	GdkPixbuf *icon_pixbuf = NULL;
	gchar modName[16];
	
	groupnum0 = -1;
	groupnum1 = -1; 
	groupnum2 = -1; 
	groupnum3 = -1; 
	groupnum4 = -1; 
	groupnum8 = -1;	
	
	bar = E_SHORTCUT_BAR(shortcut_bar);
	tmp = NULL;
	current_group =
	    e_group_bar_get_current_group_num(E_GROUP_BAR(bar));
	count =
	    e_shortcut_model_get_num_groups(E_SHORTCUT_MODEL
					    (shortcut_model));
	/*** remove shortcutbar groups so we can load the one the user has sellected ***/
	for (i = count - 1; i > -1; i--) {
		e_shortcut_model_remove_group(E_SHORTCUT_MODEL
					      (shortcut_model), i);
	}
	tmp = NULL;
	if (settings.showfavoritesgroup) {
		groupnum0 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Favorites"));
		filename = "Favorites.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum0,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			memset(modName, 0, 16);
			module_name_from_description(modName, (gchar *) tmp->data);
			sbtype = 0;
			sbtype = get_mod_type(modName);
			if (sbtype < 0)
				sbtype = 0;
			switch (sbtype) {
			case 0:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-un.png");
				break;
			case 1:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-bl.png");
				break;
			case 2:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-green.png");
				break;
			case 3:
				icon_pixbuf =
				    gdk_pixbuf_new_from_file
				    (PACKAGE_PIXMAPS_DIR
				     "/book-gold.png");
				break;
			}
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum0,
						  -1, "favorites",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (settings.showtextgroup) {
		groupnum1 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Bible Text"));
		filename = "BibleText.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum1,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-un.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum1,
						  -1, "bible text",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (settings.showcomgroup) {
		groupnum2 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Commentaries"));
		filename = "Commentaries.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum2,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-bl.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum2,
						  -1, "commentary",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}
	if (settings.showdictgroup) {
		groupnum3 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 e_utf8_from_locale_string(_
							   ("Dict/Lex")));
		filename = "Dictionaries.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum3,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-green.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum3,
						  -1, "dictionary",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}

	/* GBS */
	if (settings.showbookgroup) {
		groupnum8 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("Book"));
		filename = "Books.conf";
		tmp =
		    load_sb_group(filename, group_name, icon_size);
		large_icons = atoi(icon_size);
		if (large_icons == 1)
			e_shortcut_bar_set_view_type((EShortcutBar *)
						     shortcut_bar,
						     groupnum8,
						     E_ICON_BAR_LARGE_ICONS);
		while (tmp != NULL) {
			icon_pixbuf =
			    gdk_pixbuf_new_from_file
			    (PACKAGE_PIXMAPS_DIR "/book-gold.png");
			e_shortcut_model_add_item(E_SHORTCUT_BAR
						  (shortcut_bar)->
						  model, groupnum8,
						  -1, "book",
						  (gchar *) tmp->
						  data, icon_pixbuf);
			tmp = g_list_next(tmp);
		}
	}

	if (settings.showhistorygroup) {
		groupnum4 =
		    add_sb_group((EShortcutBar *) shortcut_bar,
				 _("History"));
	}
	g_list_free(tmp);
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar),
					  current_group, TRUE);
	count =
	    e_shortcut_model_get_num_groups(E_SHORTCUT_MODEL
					    (shortcut_model));
	/* set manually added groups number */
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
		gtk_widget_ref(item);
		gtk_object_set_data_full(GTK_OBJECT(menu), "item",
					 item, (GtkDestroyNotify)
					 gtk_widget_unref);
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
		gtk_widget_ref(item);
		gtk_object_set_data_full(GTK_OBJECT(menu), "item",
					 item, (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER
				  (shortcut_menu_widget), item);
		gtk_signal_connect(GTK_OBJECT(item), "activate",
				   GTK_SIGNAL_FUNC
				   (on_add_shortcut_activate),
				   (gchar *) glist->data);
		glist = g_list_next(glist);
	}
	g_list_free(glist);
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
		gdk_window_raise(GTK_WIDGET(settings.dockSB)->window);
		return;
	}

	if (settings.showshortcutbar) {
		settings.showshortcutbar = FALSE;
		settings.biblepane_width = settings.gs_width / 2;
		gtk_widget_hide(settings.shortcut_bar);
		e_paned_set_position(E_PANED(settings.epaned), 0);
		e_paned_set_position(E_PANED
				     (gui_lookup_widget
				      (settings.app, "hpaned1")),
				     settings.biblepane_width);
	} else {
		settings.showshortcutbar = TRUE;
		settings.biblepane_width =
		    (settings.gs_width - settings.shortcutbar_width) / 2;
		e_paned_set_position(E_PANED(settings.epaned),
				     settings.shortcutbar_width);
		e_paned_set_position(E_PANED
				     (gui_lookup_widget(settings.app, "hpaned1")),
				     settings.biblepane_width);
		gtk_widget_show(settings.shortcut_bar);
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
		gtk_widget_show(settings.shortcut_bar);
		e_paned_set_position(E_PANED(settings.epaned),
				     settings.shortcutbar_width);
	}

	else if (!settings.showshortcutbar && settings.showdevotional) {
		gtk_widget_show(settings.shortcut_bar);
		gui_shortcutbar_showhide();
	}

	else {
		gtk_widget_hide(settings.shortcut_bar);
		e_paned_set_position(E_PANED(settings.epaned),
				     1);
	}

	/* set hight of bible and commentary pane */
	e_paned_set_position(E_PANED(gui_lookup_widget(settings.app, "vpaned1")),
			     settings.upperpane_hight);

	/* set width of bible pane */
	e_paned_set_position(E_PANED(gui_lookup_widget(settings.app, "hpaned1")),
			     settings.biblepane_width);

	if (!settings.docked) {
		settings.docked = TRUE;
		gui_attach_detach_shortcutbar();
	}
}
