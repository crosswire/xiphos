/*
 * Xiphos Bible Study Tool
 * bookmark_dialog.h - gui to popup a dialog for adding a bookmark
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

#ifndef _BOOKMARK_DIALOG_H
#define _BOOKMARK_DIALOG_H

#ifdef __cplusplus
extern "C" {
#endif
void on_dialog_response(GtkDialog *dialog,
			gint response_id, gpointer user_data);
void gui_bookmark_dialog(gchar *label, gchar *module_name,
			 gchar *key);
void gui_mark_verse_dialog(gchar *module_name, gchar *key);
void on_buffer_changed(GtkTextBuffer *textbuffer,
		       gpointer user_data);
void on_mark_verse_response(GtkDialog *dialog, gint response_id,
			    gpointer user_data);
void on_dialog_enter(void);
void on_mark_verse_enter(void);
gboolean on_treeview_button_release_event(GtkWidget *widget,
					  GdkEventButton *event,
					  gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif /* _BOOKMARK_DIALOG_H */
