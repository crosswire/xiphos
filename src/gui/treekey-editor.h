/*
 * Xiphos Bible Study Tool
 * treekey-editor.h -
 *
 * Copyright (C) 2007-2017 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _TREEKEY_EDITOR_H
#define _TREEKEY_EDITOR_H

#ifdef __cplusplus
extern "C" {
#endif

GtkWidget *gui_create_editor_tree(EDITOR *editor);
GtkWidget *create_edit_tree_menu(EDITOR *editor);

#ifdef __cplusplus
}
#endif
#endif /* _TREEKEY_EDITOR_H */
