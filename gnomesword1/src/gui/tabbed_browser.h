/*
 * GnomeSword Bible Study Tool
 * tabbed_browser.h - functions to facilitate tabbed browsing of different passages at once
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#ifndef __TABBED_BROWSER_H__
#define __TABBED_BROWSER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _passage_tab_info PASSAGE_TAB_INFO;
struct _passage_tab_info {
	GtkWidget *page_widget;
	GtkLabel *tab_label;
	GtkWidget *button_close;
	GtkWidget *close_pixmap;
	gchar *text_mod;
	gchar *commentary_mod;
	gchar *dictlex_mod;
	gchar *book_mod;
	gchar *text_commentary_key;
	gchar *dictlex_key;
	gchar *book_key;
};
extern PASSAGE_TAB_INFO *cur_passage_tab; //need to update this every time one of the variables changes (i.e. new verse selected)

void gui_set_tab_label(const gchar * key);
void gui_open_passage_in_new_tab(gchar *key);
void gui_close_passage_tab(gint pagenum);
void gui_update_tab_struct(const gchar * text_mod, 
			   const gchar * commentary_mod, 
			   const gchar * dictlex_mod, 
			   const gchar * book_mod, 
			   const gchar * dictlex_key, 
			   const gchar * book_key);

void gui_notebook_main_setup(GList *ptlist);
void gui_notebook_main_shutdown(void);


#ifdef __cplusplus
}
#endif

#endif
