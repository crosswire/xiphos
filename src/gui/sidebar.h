/*
 * Xiphos Bible Study Tool
 * sidebar.h - create and maintain the sidebar
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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

#ifndef __SIDEBAR_H_
#define __SIDEBAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>
typedef struct _sidebar SIDEBAR;
struct _sidebar
{
	GtkWidget *module_list;
	GtkWidget *menu_modules;
	GtkWidget *menu_prayerlist;
	GtkWidget *menu_prayerlist_mod;
	GtkWidget *results_list;
	GtkWidget *notebook;
	GtkWidget *tbtn_view_main;
	GtkWidget *btn_save;
	GtkWidget *htmlshow;
	GtkWidget *html_widget;
	GtkWidget *html_viewer_eventbox;
	GtkWidget *html_viewer_widget;
	GtkWidget *optionmenu1;
	GtkWidget *menu_item_save_search;
	gchar mod_name[80];
};
extern SIDEBAR sidebar;

typedef struct _search_results RESULTS;
struct _search_results
{
	gchar *module;
	gchar *key;
};

extern GList *list_of_verses;
extern GtkListStore *model_verselist;
extern gboolean is_search_result;

#ifdef USE_TREEVIEW_PATH
void gui_collapse_treeview_to_book(GtkTreeView *tree,
				   const gchar *book_name);
void gui_save_treeview_path_string(const gchar *path_str,
				   const gchar *book_name);
gboolean gui_expand_treeview_to_path(GtkTreeView *tree,
				     const gchar *book_name);
#endif
void gui_set_sidebar_program_start(void);
void gui_sidebar_showhide(void);
//gboolean main_sidebar_display_dictlex(char *mod_name, char *key);
GtkWidget *gui_create_sidebar(GtkWidget *paned);
gboolean gui_verselist_button_release_event(GtkWidget *widget,
					    GdkEventButton *event,
					    gpointer user_data);
gboolean vpaned_srch_rslt_button_release_event(GtkWidget *widget,
					       GdkEventButton *
						   event,
					       gpointer user_data);
void gui_show_previewer_in_sidebar(gint choice);
void gui_sync_module_treeview(gint direction);
void on_save_list_as_a_single_bookmark_activate(GtkMenuItem *
						    menuitem,
						gpointer
						    user_data);
void on_save_list_as_a_series_of_bookmarks_activate(GtkMenuItem *
							menuitem,
						    gpointer
							user_data);
void on_populate_verse_list_activate(GtkMenuItem *menuitem,
				     gpointer user_data);
void on_preload_history_from_verse_list_activate(GtkMenuItem *
						     menuitem,
						 gpointer
						     user_data);
void on_send_list_via_biblesync_activate(GtkMenuItem *menuitem,
					 gpointer user_data);
GtkWidget *create_results_menu(void);
void on_open_in_dialog_activate(GtkMenuItem *menuitem,
				gpointer user_data);
void on_open_in_tab_activate2(GtkMenuItem *menuitem,
			      gpointer user_data);
void on_about2_activate(GtkMenuItem *menuitem,
			gpointer user_data);
void on_simple_activate(GtkMenuItem *menuitem,
			gpointer user_data);
void on_subject_activate(GtkMenuItem *menuitem,
			 gpointer user_data);
void on_monthly_activate(GtkMenuItem *menuitem,
			 gpointer user_data);
void on_journal_activate(GtkMenuItem *menuitem,
			 gpointer user_data);
void on_outlined_topic_activate(GtkMenuItem *menuitem,
				gpointer user_data);
GtkWidget *create_menu_prayerlist(void);
void on_edit_activate(GtkMenuItem *menuitem, gpointer user_data);
GtkWidget *create_menu_prayerlist_mod(void);
void gui_menu_prayerlist_popup(GtkMenuItem *menuitem,
			       gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif
