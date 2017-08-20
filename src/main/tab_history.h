/*
 * Xiphos Bible Study Tool
 * tab_history.h - add, remove and navigate history
 *
 * Copyright (C) 2005-2017 Xiphos Developer Team
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

#ifndef __TAB_HISTORY_H_
#define __TAB_HISTORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TABHISTORYLENGTH 50

struct _tab_history
{
	gchar verseref[TABHISTORYLENGTH];
	gchar textmod[TABHISTORYLENGTH];
	gchar commod[TABHISTORYLENGTH];
};
typedef struct _tab_history TAB_HISTORY;

void on_menu_historyitem_activate(GtkMenuItem *menuitem,
				  gpointer user_data);
void main_update_tab_history_menu(gpointer data);
void on_clear_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_history_to_verse_list_activate(GtkMenuItem *menuitem,
				       gpointer user_data);
GtkWidget *main_versekey_drop_down_new(gpointer data);
void main_clear_tab_history(void);
void main_add_tab_history_item(gpointer data);
void main_fake_tab_history_item(char *reference);
void main_navigate_tab_history(gint direction);
void main_change_verse_tab_history(gint historynum);

#ifdef __cplusplus
}
#endif
#endif
