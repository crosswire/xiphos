/*
 * Xiphos Bible Study Tool
 * sidebar.h - sidebar interface to sword
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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

typedef struct
{
	GdkPixbuf *pixbuf_opened;
	GdkPixbuf *pixbuf_closed;
	GdkPixbuf *pixbuf_helpdoc;
} TreePixbufs;
extern TreePixbufs *pixbufs;

#ifdef USE_TREEVIEW_PATH
gboolean main_expand_treeview_to_path(GtkTreeModel *model,
				      GtkTreeIter iter);
#endif

void main_open_bookmark_in_new_tab(gchar *mod_name, gchar *key);
void main_display_verse_list_in_sidebar(gchar *key,
					gchar *module_name,
					gchar *verse_list);
void main_create_pixbufs(void);
void main_load_module_tree(GtkWidget *tree);
void main_add_mod_tree_columns(GtkTreeView *tree);
void main_mod_treeview_button_one(GtkTreeModel *model,
				  GtkTreeIter selected);

#ifdef __cplusplus
}
#endif
