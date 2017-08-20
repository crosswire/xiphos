/*
 * Xiphos Bible Study Tool
 * dictlex.h - gui for dictionary/lexicon modules
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

#ifndef ___DICTLEX_H_
#define ___DICTLEX_H_

#ifdef __cplusplus
extern "C" {
#endif

void gui_get_clipboard_text_for_lookup(GtkClipboard *clipboard,
				       const gchar *text,
				       gpointer data);
void gui_create_pm_dictionary(void);
void gui_lookup_dictlex_selection(GtkMenuItem *menuitem,
				  gchar *dict_mod_description);
void gui_set_dictlex_mod_and_key(gchar *mod, gchar *key);
void gui_display_dictlex(gchar *key);
GtkWidget *gui_create_dictionary_pane(void);
void on_entryDictLookup_changed(GtkEditable *editable,
				gpointer data);
void dict_key_entry_changed(GtkEntry *entry, gpointer data);
void button_back_clicked(GtkButton *button, gpointer user_data);
void button_forward_clicked(GtkButton *button,
			    gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif
