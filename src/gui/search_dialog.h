/*
 * GnomeSword Bible Study Tool
 * search_dialog.h - gui for searching Sword modules
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

#ifndef _SEARCH_DIALOG_
#define _SEARCH_DIALOG_

#ifdef __cplusplus
extern "C" {
#endif
#include <gnome.h>	
	
typedef struct _search_dialog SEARCH_DIALOG;
struct _search_dialog {
	GtkWidget *dialog;
	GtkWidget *label_search_module;
	GtkWidget *notebook;
	GtkWidget *togglebutton_show_main;
	GtkWidget *combo_list;
	GtkWidget *combo_entry_list;
	GtkWidget *module_lists;
	GtkWidget *listview_modules;
	GtkWidget *entry_list_name;
	GtkWidget *rb_current_module;
	GtkWidget *label_mod_select;
	GtkWidget *label_scope;
	GtkWidget *rb_custom_range;
	GtkWidget *rb_mod_list;
	GtkWidget *rb_custom_list;
	GtkWidget *rb_last;
	GtkWidget *rb_no_scope;
	GtkToggleButton *which_scope;
	GtkWidget *combo_range;
	GtkWidget *combo_entry_range;
	GtkWidget *list_range_name;
	GtkWidget *list_ranges;
	GtkWidget *entry_range_name;
	GtkWidget *entry_range_text;
	GtkWidget *results_html;
	GtkWidget *report_html;
	GtkWidget *preview_html;
	GtkWidget *search_entry;
	GtkWidget *rb_words;
	GtkWidget *rb_regexp;
	GtkWidget *rb_exact_phrase;
	GtkWidget *rb_attribute;
	GtkWidget *cb_case_sensitive;
	GtkWidget *ctree;
	GtkWidget *treeview;
	GtkWidget *progressbar;
	GtkWidget *cb_include_strongs;
	GtkWidget *cb_include_morphs;
	GtkWidget *cb_include_footnotes;
	GSList *_201_group;

	gint text_group;
	gint comm_group;
	gint dict_group;
	gint book_group;
	gint module_group;
	gint range_group;
	gint results_group;
	gint which_bounds;
	gint module_count;
	gint modules_row;
	gint custom_range_row;
	gint custom_list_row;
	gint range_rows;
	gint list_rows;

	gboolean show_in_main;

	gchar *search_mod;
};
extern SEARCH_DIALOG search;

GtkWidget *gui_create_search_dialog(void);
void gui_do_dialog_search(void);
	
#ifdef __cplusplus
}
#endif

#endif
