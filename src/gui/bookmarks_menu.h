/*
 * Xiphos Bible Study Tool
 * bookmarks_menu.h - gui for bookmarks in a menu
 *
 * Copyright (C) 2003-2017 Xiphos Developer Team
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

#ifndef ___BOOKMARKS_MENU_H_
#define ___BOOKMARKS_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif
struct _bookmark_menu
{
	GtkWidget *menu;
	GtkWidget *in_tab;
	GtkWidget *in_dialog;
	GtkWidget *new;
	GtkWidget *insert;
	GtkWidget *add;
	GtkWidget *edit;
	GtkWidget *point;
	GtkWidget *delete;
	GtkWidget *reorder;
	GtkWidget *save;
	GtkWidget *bibletime;
	GtkWidget *rr_submenu;
	GtkWidget *remove;
};
typedef struct _bookmark_menu BOOKMARK_MENU;
extern BOOKMARK_MENU menu;
extern GtkTreeSelection *current_selection;
extern GtkTreeStore *model;
extern gboolean use_dialog;
extern gboolean bookmarks_changed;

void gui_save_bookmarks(GtkMenuItem *menuitem,
			gpointer user_data);
void gui_create_bookmark_menu(void);
void gui_save_bookmarks_treeview(void);
void bibletime_bookmarks_activate(GtkMenuItem *menuitem,
				  gpointer user_data);
void on_allow_reordering_activate(GtkMenuItem *menuitem,
				  gpointer user_data);
void on_dialog_activate(GtkMenuItem *menuitem,
			gpointer user_data);
void on_edit_item_activate(GtkMenuItem *menuitem,
			   gpointer user_data);
void on_export_folder_activate(GtkMenuItem *menuitem,
			       gpointer user_data);
void on_delete_item_activate(GtkMenuItem *menuitem,
			     gpointer user_data);
void on_expand_activate(GtkMenuItem *menuitem,
			gpointer user_data);
void on_collapse_activate(GtkMenuItem *menuitem,
			  gpointer user_data);
void on_add_bookmark_activate(GtkMenuItem *menuitem,
			      gpointer user_data);
void on_insert_bookmark_activate(GtkMenuItem *menuitem,
				 gpointer user_data);
void on_new_folder_activate(GtkMenuItem *menuitem,
			    gpointer user_data);
void on_open_in_tab_activate(GtkMenuItem *menuitem,
			     gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif
