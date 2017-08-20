/*
 * Xiphos Bible Study Tool
 * search_dialog.h - glue
 *
 * Copyright (C) 2004-2017 Xiphos Developer Team
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

#ifndef __SEARCH_DIALOG_H__
#define __SEARCH_DIALOG_H__

#ifdef __cplusplus
extern "C" {
#endif

gboolean main_export_current_adv_search(GString *str,
					gboolean html,
					gboolean with_scripture);
void main_save_current_adv_search_as_bookmarks(void);
void main_add_mod_to_list(GtkWidget *tree_widget,
			  gchar *mod_name);
void main_finds_verselist_selection_changed(GtkTreeSelection *
						selection,
					    GtkTreeModel *model,
					    gboolean
						is_double_click);
void main_selection_modules_lists_changed(GtkTreeSelection *
					      selection,
					  gpointer data);
void main_selection_finds_list_changed(GtkTreeSelection *
					   selection,
				       gpointer data);
void main_comboboxentry2_changed(GtkComboBox *combobox,
				 gpointer user_data);
void main_dialog_search_percent_update(char percent,
				       void *userData);
void main_range_text_changed(GtkEditable *editable);
void main_save_modlist(void);
void main_save_range(void);
void main_delete_range(void);
void main_add_modlist_to_label(void);
void main_change_mods_select_label(char *mod_name);
void main_delete_module(GtkTreeView *treeview);
void main_selection_modules_lists_changed(GtkTreeSelection *
					      selection,
					  gpointer data);
void main_mod_selection_changed(GtkTreeSelection *selection,
				GtkWidget *tree_widget);
void main_do_dialog_search(void);
void main_open_search_dialog(void);
void main_close_search_dialog(void);
char *main_get_search_rendered_text(char *module_name, char *key);

#ifdef __cplusplus
}
#else

#endif

extern gboolean terminate_search; // def'd in search_dialog.cc.
extern gboolean search_active;    // def'd in search_dialog.cc.

#endif
