/*
 * GnomeSword Bible Study Tool
 * utilities.h - support functions
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

#ifdef __cplusplus
extern "C" {
#endif
	
#include <gnome.h>

void main_create_pixbufs(void);
void main_load_module_tree(GtkWidget * tree);
void main_add_mod_tree_columns(GtkTreeView * tree);
void main_add_books_to_bible(GtkTreeModel * model, GtkTreeIter iter,
				const gchar * mod_name);
void main_add_chapters_to_book(GtkTreeModel * model, GtkTreeIter iter, 
				const gchar * key);
void main_add_children_to_tree(GtkTreeModel * model, GtkTreeIter iter,
				 gchar *mod_name, unsigned long offset);
void main_mod_treeview_button_one(GtkTreeModel *model, GtkTreeIter selected);
	
#ifdef __cplusplus
}
#endif
