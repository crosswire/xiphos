/*
 * GnomeSword Bible Study Tool
 * search_dialog.h - glue
 *
 * Copyright (C) 2004 GnomeSword Developer Team
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

#ifndef __SEARCH_DIALOG_H__
#define __SEARCH_DIALOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <glib-2.0/glib.h>

void main_dialog_search_percent_update(char percent, void *userData);	
void main_save_modlist(void);
void main_save_range(void);
void main_delete_range(void);
void main_add_modlist_to_label(void);
void main_change_mods_select_label(char *mod_name);
void main_delete_module(GtkTreeView *treeview);
void main_selection_modules_lists_changed(GtkTreeSelection *
				    selection, gpointer data);
void main_mod_selection_changed(GtkTreeSelection * selection,
				     GtkWidget * tree_widget);
void main_do_dialog_search(gpointer user_data);
void main_open_search_dialog(void);
void main_close_search_dialog(void);

#ifdef __cplusplus
}
#endif

#endif
