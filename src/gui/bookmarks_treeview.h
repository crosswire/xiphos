/*
 * Xiphos Bible Study Tool
 * bookmarks_treeview.h - gui for bookmarks in treeview
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

#ifndef ___BOOKMARKS_TREEVIEW_H_
#define ___BOOKMARKS_TREEVIEW_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <gtk/gtk.h>
#include <libxml/parser.h>
enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_CAPTION,
	COL_KEY,
	COL_MODULE,
	COL_MODULE_DESC,
	COL_DESCRIPTION,
	N_COLUMNS
};

typedef struct
{
	GdkPixbuf *pixbuf_opened;
	GdkPixbuf *pixbuf_closed;
	GdkPixbuf *pixbuf_helpdoc;
} BookMarksPixbufs;

struct _bookmark_data
{
	xmlNodePtr parent;
	gchar *caption;
	gchar *key;
	gchar *module;
	gchar *module_desc;
	gchar *description;
	gboolean is_leaf;
	GdkPixbuf *opened;
	GdkPixbuf *closed;
};
typedef struct _bookmark_data BOOKMARK_DATA;

extern BookMarksPixbufs *bm_pixbufs;
extern GtkTreeView *bookmark_tree;

void gui_load_removed(const xmlChar *file);
void gui_add_columns(GtkTreeView *tree);
void gui_add_item_to_tree(GtkTreeIter *iter, GtkTreeIter *parent,
			  BOOKMARK_DATA *data);
void gui_verselist_to_bookmarks(GList *verses,
				gint save_as_single);
GtkWidget *gui_create_bookmark_tree(void);
void gui_parse_bookmarks(GtkTreeView *tree, const xmlChar *file,
			 GtkTreeIter *parent);
GtkWidget *gui_create_dialog_add_bookmark(gchar *label,
					  gchar *module_name,
					  gchar *key);

#ifdef __cplusplus
}
#endif
#endif
