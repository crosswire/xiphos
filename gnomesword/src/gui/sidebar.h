/*
 * GnomeSword Bible Study Tool
 * sidebar.h - create and maintain the sidebar
 *
 * Copyright (C) 2000-2008 GnomeSword Developer Team
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

#ifndef __SIDEBAR_H_
#define __SIDEBAR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sidebar SIDEBAR;
struct _sidebar {
	GtkWidget *module_list;
	GtkWidget *menu_modules;
	GtkWidget *menu_prayerlist;
	GtkWidget *menu_prayerlist_mod;
	GtkWidget *results_list,
		*notebook,
		*tbtn_view_main,
		*btn_save,
		*htmlshow,
		*html_widget,
		*html_viewer_widget;
	GtkWidget *optionmenu1;
	GtkWidget *menu_item_save_search;
	gchar   mod_name[80];
};
extern SIDEBAR sidebar;

typedef struct _search_results RESULTS;
struct _search_results {
	gchar *module;
	gchar *key;
};

extern GList *list_of_verses;

void gui_set_sidebar_porgram_start(void);
void gui_sidebar_showhide(void);
//gboolean main_sidebar_display_dictlex(char *mod_name, char *key);
GtkWidget *gui_create_sidebar(GtkWidget * paned);
gboolean gui_verselist_button_release_event(GtkWidget * widget,
						 GdkEventButton * event,
						 gpointer user_data);
gboolean vpaned_srch_rslt_button_release_event(GtkWidget * widget,
					       GdkEventButton * event,
					       gpointer user_data);

#ifdef __cplusplus
}
#endif

#endif
