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

typedef struct {
	GdkPixbuf *pixbuf_opened;
	GdkPixbuf *pixbuf_closed;
	GdkPixbuf *pixbuf_helpdoc;
} TreePixbufs;
extern TreePixbufs *pixbufs;

void main_open_bookmark_in_new_tab(gchar * mod_name, gchar * key);
void main_display_verse_list_in_sidebar(gchar * key, gchar * module_name, 
						gchar * verse_list);	
void main_create_pixbufs(void);
void main_load_module_tree(GtkWidget * tree);
void main_add_mod_tree_columns(GtkTreeView * tree);
void main_mod_treeview_button_one(GtkTreeModel *model, GtkTreeIter selected);
gboolean main_sidebar_display_dictlex(char *mod_name, char *key);

#ifdef __cplusplus
}
#endif
