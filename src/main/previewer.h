/*
 * Xiphos Bible Study Tool
 * previewer.h -
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

#ifndef _PREVIEWER_H
#define _PREVIEWER_H

#ifdef __cplusplus
extern "C" {
#endif

void main_init_previewer(void);
void main_set_previewer_widget(int in_sidebar);
GtkWidget *main_get_previewer_widget(void);
void main_clear_viewer(void);
void main_information_viewer(const char *mod_name,
			     const char *text, const char *key,
			     const char *action, const char *type,
			     const char *morph_text,
			     const char *morph);
void mark_search_words(GString *str);
void main_entry_display(gpointer data, char *mod_name,
			char *text, char *key, gboolean show_key);

#ifdef __cplusplus
}
#endif
#endif /* _PREVIEWER_H */
