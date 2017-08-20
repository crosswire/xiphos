/*
 * Xiphos Bible Study Tool
 * export_bookmarks.h -
 *
 * Copyright (C) 2003-2017 Xiphos Developer Team
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

#ifndef ___EXPORT_BOOKMARKS_H_
#define ___EXPORT_BOOKMARKS_H_

#ifdef __cplusplus
extern "C" {
#endif

struct _export_bookmarks
{
	gboolean as_bookmarks;
	gboolean as_html;
	gboolean as_plain;
	gboolean with_scripture;
	gint type;
	gint verselist;
	GtkWidget *rb_bookmarks;
	GtkWidget *rb_html;
	GtkWidget *rb_plain;
	GtkWidget *cb_scripture;
	GtkWidget *filechooserwidget;
	gchar *filename;
	gchar *verselist_name;
	GList *verses;
};
typedef struct _export_bookmarks BK_EXPORT;

enum {
	BOOKMARKS_EXPORT,
	VERSE_LIST_EXPORT,
	SEARCH_RESULTS_EXPORT,
	ADV_SEARCH_RESULTS_EXPORT
};

void gui_set_html_item(GString *str,
		       const gchar *description,
		       const gchar *module,
		       const gchar *key, gboolean with_scripture);
void gui_set_plain_text_item(GString *str,
			     const gchar *description,
			     const gchar *module,
			     const gchar *key,
			     gboolean with_scripture);
void gui_export_bookmarks_dialog(gint is_verselist,
				 GList *verses);
gboolean dialog_vbox1_key_press_event_cb(GtkWidget *widget,
					 GdkEventKey *event,
					 gpointer user_data);

#ifdef __cplusplus
}
#endif
#endif /* ___EXPORT_BOOKMARKS_H_ */
