/*
 * GnomeSword Bible Study Tool
 * gbs.h - generic book support - the gui
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

#ifndef ___GBS_H_
#define ___GBS_H_


typedef struct _gbsdata GBS_DATA;
struct _gbsdata {
	GtkWidget *dialog;
	GtkWidget *statusbar;	
	GtkWidget *vbox;
	GtkWidget *html;
	GtkWidget *frame;
	GtkWidget *ctree;
	GtkWidget *showtabs;
	GtkWidget *module_options_menu;
	gchar *mod_name;
	gchar *search_string;
	gchar *key;
	gchar *cipher_key;
	gchar *cipher_old;
	gint mod_num;
	gint display_level;
	unsigned long offset;
	
	gboolean has_key;
	gboolean is_locked;
	gboolean is_dialog;
	gboolean is_rtol;
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

GBS_DATA *get_gbs(GList * gbs);
void gui_set_book_page_and_key(gint page_num, gchar * key);
void gui_set_gbs_frame_label(void);
void on_notebook_gbs_switch_page(GtkNotebook *notebook, 
		GtkNotebookPage *page, gint page_num, GList *data_gbs);
void gui_add_new_gbs_pane(GBS_DATA * g);
void gui_setup_gbs(GList *mods, gint starting_page);
void gui_shutdown_gbs(void);

#endif
