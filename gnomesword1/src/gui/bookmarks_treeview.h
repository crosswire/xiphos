/*
 * GnomeSword Bible Study Tool
 * bookmarks_treeview.h - gui for bookmarks 
 *
 * Copyright (C) 2003 GnomeSword Developer Team
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

#ifndef ___BOOKMARKS_TREEVIEW_H_
#define ___BOOKMARKS_TREEVIEW_H_

#ifdef __cplusplus
extern "C" {
#endif

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


typedef struct {
	GdkPixbuf *pixbuf_opened;
	GdkPixbuf *pixbuf_closed;
	GdkPixbuf *pixbuf_helpdoc;
} TreePixbufs;

struct _bookmark_data {
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


extern TreePixbufs *pixbufs;
extern GtkTreeView *bookmark_tree;

void gui_verselist_to_bookmarks(GList * verses);	
GtkWidget *gui_create_bookmark_tree(void);
void gui_parse_bookmarks(GtkTreeView *tree, const xmlChar * file,
			    GtkTreeIter *parent);

#ifdef __cplusplus
}
#endif

#endif
