/*
 * Xiphos Bible Study Tool
 * tabbed_browser.h - functions to facilitate tabbed browsing of different passages at once
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

#ifndef __TABBED_BROWSER_H__
#define __TABBED_BROWSER_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <gtk/gtk.h>
#include "main/tab_history.h"

struct _passage_tab_info
{
	GtkWidget *page_widget;
	GtkLabel *tab_label;
	GtkWidget *button_close;
	GtkWidget *editor;
	GtkWidget *paratab;
	gchar *text_mod;
	gchar *commentary_mod;
	gchar *dictlex_mod;
	gchar *book_mod;
	gchar *text_commentary_key;
	gchar *dictlex_key;
	gchar *book_offset;
	gboolean comm_showing;
	TAB_HISTORY history_list[TABHISTORYLENGTH];
	gint history_items;
	gint current_history_item;
	gboolean first_back_click;

	/* per-tab show indicators */
	gboolean showtexts;
	gboolean showpreview;
	gboolean showcomms;
	gboolean showdicts;
	gboolean showbookeditor;
	gboolean showparallel;
};
typedef struct _passage_tab_info PASSAGE_TAB_INFO;

extern PASSAGE_TAB_INFO *cur_passage_tab;
//need to update this every time one of the variables changes (i.e. new verse selected)

void gui_save_tabs(const gchar *filename);
void gui_load_tabs(const gchar *filename);
void gui_select_nth_tab(gint page_num);
void gui_set_tab_label(const gchar *key, gboolean one_tab);
void gui_set_named_tab_label(const gchar *key,
			     PASSAGE_TAB_INFO *pt,
			     gboolean update);
void gui_open_module_in_new_tab(gchar *module);
void gui_open_passage_in_new_tab(gchar *key);
void gui_open_parallel_view_in_new_tab(void);
void gui_close_passage_tab(gint pagenum);
void gui_update_tab_struct(const gchar *text_mod,
			   const gchar *commentary_mod,
			   const gchar *dictlex_mod,
			   const gchar *book_mod,
			   const gchar *dictlex_key,
			   const gchar *book_offset,
			   gboolean comm_showing,
			   gboolean showtexts,
			   gboolean showpreview,
			   gboolean showcomms, gboolean showdicts);
void gui_close_all_tabs(void);
void gui_open_tabs(void);

void gui_notebook_main_setup(int tabs, const char *tabsfile);
void gui_notebook_main_shutdown(int tabs);

void gui_tab_set_showtexts(int show);
void gui_tab_set_showpreview(int show);
void gui_tab_set_showcomms(int show);
void gui_tab_set_showdicts(int show);
void gui_tabs_on_off(int on);

void on_notebook_main_new_tab_clicked(GtkButton *button,
				      gpointer user_data);

extern gboolean change_tabs_no_redisplay;

void gui_recompute_shows(gboolean flush);
void gui_recompute_view_menu_choices(void);
void setup_book_editor_tab(PASSAGE_TAB_INFO *pt);
GString *pick_tab_label(PASSAGE_TAB_INFO *pt);
#ifdef USE_GTK_3
void gui_notebook_main_switch_page(GtkNotebook *notebook,
				   gpointer arg1, gint page_num,
				   GList **tl);
#else
void gui_notebook_main_switch_page(GtkNotebook *notebook,
				   GtkNotebookPage *page,
				   gint page_num, GList **tl);
#endif

#ifdef __cplusplus
}
#endif
#endif
