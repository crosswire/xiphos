/*
 * Xiphos Bible Study Tool
 * gbs.h - generic book support - the gui
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

#ifndef ___GBS_H_
#define ___GBS_H_

GtkWidget *gui_create_book_pane(void);
void gui_update_gbs_global_ops(gchar *option, gboolean choice);
void gui_set_book_page_and_key(gchar *mod_name, gchar *key);
void gui_set_book_mod_and_key(gchar *mod_name, gchar *key);

#endif
