/*
 * GnomeSword Bible Study Tool
 * bookmarks.h - gui for bookmarks
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

#ifndef ___BOOKMARKS_H_
#define ___BOOKMARKS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main/settings.h"

typedef struct _bmtree BM_TREE;
struct _bmtree {
	GtkWidget *ctree_widget;
	GtkCTree *ctree;
};

void gui_add_bookmark_to_tree(GtkCTreeNode * node,
		   gchar * modName, gchar * verse);
void gui_load_bookmark_tree(void);
void create_bookmark_menu(void);
void on_new_subgroup_activate(GtkMenuItem * menuitem,
			  gpointer user_data);
void on_add_new_group1_activate(GtkMenuItem * menuitem,
			    gpointer user_data);
void gui_save_bookmarks(GtkMenuItem * menuitem,
			     gpointer user_data);
void on_edit_item_activate(GtkMenuItem * menuitem,
		       gpointer user_data);
void on_delete_item_activate(GtkMenuItem * menuitem,
			 gpointer user_data);
void on_allow_reordering_activate(GtkMenuItem * menuitem,
			      gpointer user_data);
void gui_verselist_to_bookmarks(GList *list);
void create_add_bookmark_menu(GtkWidget * menu,
			  GtkWidget * bookmark_menu_widget);


#ifdef __cplusplus
}
#endif

#endif

