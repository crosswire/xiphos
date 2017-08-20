/*
 * Xiphos Bible Study Tool
 * editor.h - webkit stuff
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

#ifndef _EDITOR_H
#define _EDITOR_H

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <gtk/gtk.h>

#ifdef USE_WEBKIT_EDITOR

#include "editor/webkit_editor.h"

gchar *editor_get_selected_text(EDITOR *e);
void editor_find_string(gchar *needle, EDITOR *e);
void editor_replace_string(gchar *old_string, gchar *new_string,
			   EDITOR *e);
gint editor_insert_new_outline_level(gint level, EDITOR *e);
gboolean editor_cut(EDITOR *e);
gboolean editor_copy(EDITOR *e);
gboolean editor_paste(EDITOR *e);
void editor_get_document_content(GString *data, EDITOR *e);
//void editor_open_recent (const gchar * uri, EDITOR * e);
gboolean editor_insert_link(void);
gboolean editor_insert_sword_link(void);
void editor_insert_link_ok(void);
void editor_execute_script(gchar *script, EDITOR *e);
void editor_insert_html(const gchar *html, EDITOR *e);
void create_editor_window(GtkWidget *scrollwindow, EDITOR *e);

#endif /* USE_WEBKIT_EDITOR */

#ifdef __cplusplus
}
#endif
#endif /* _EDITOR_H */
