/*
 * GnomeSword Bible Study Tool
 * gbs.h - generic book support - the glue
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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

#ifndef __GBS_H_
#define __GBS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "settings.h"


typedef struct _gbsdata GBS_DATA;
/* gnome */
#include "gbs_find.h"
struct _gbsdata {
	GtkWidget *html;
	GtkWidget *ctree;
	GtkWidget *showtabs;
	gchar *bookName;
	gchar *searchstring;
	gint booknum;
	gboolean has_key;
	GBF_FIND_DIALOG *find_dialog;
};


typedef struct _nodedata NODEDATA;
struct _nodedata {
	GtkCTreeNode *parent;
	GtkCTreeNode *sibling;
	gchar *buf[3];
	GdkPixmap *pixmap1;
	GdkPixmap *pixmap2;
	GdkBitmap *mask1;
	GdkBitmap *mask2;
	gboolean is_leaf;
	gboolean expanded;
};

char *get_book_key(int book_num);
gboolean display_row_gbs(gint book_num, gchar *offset);
GBS_DATA *get_gbs(GList * gbs);
void add_node_children(SETTINGS *s, GtkCTreeNode *node, gchar *bookname,
						unsigned long offset);
void set_book_page_and_key(gint page_num, gchar * key);
void setup_gbs(SETTINGS * s, GList *mods);
void shutdown_gbs(void);
GtkCTreeNode *add_node_gbs(SETTINGS * s, NODEDATA * data);

#ifdef __cplusplus
}
#endif
#endif	/* __GBS_H_ */
