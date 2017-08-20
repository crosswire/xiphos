/*
 * Xiphos Bible Study Tool
 * search_dialog.h - gui for searching Sword modules
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

#ifndef _SEARCH_DIALOG_
#define _SEARCH_DIALOG_

#ifdef __cplusplus
extern "C" {
#endif
#include <gtk/gtk.h>

typedef struct _search_dialog1 SEARCH_DIALOG1;
struct _search_dialog1
{
	GtkWidget *dialog;
	GtkWidget *mod_sel_dialog;
	GtkWidget *label_search_module;
	GtkWidget *notebook;
	GtkWidget *togglebutton_show_main;
	GtkWidget *combo_list;
	//      GtkWidget *combo_entry_list;
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
	//      GtkWidget *combo_entry_range;
	GtkWidget *list_range_name;
	GtkWidget *list_ranges;
	GtkWidget *entry_range_name;
	GtkWidget *entry_range_text;
	//      GtkWidget *results_html;
	//      GtkWidget *report_html;
	GtkWidget *preview_html;
	GtkWidget *search_entry;
	GtkWidget *rb_words;
	GtkWidget *rb_regexp;
	GtkWidget *rb_exact_phrase;
	GtkWidget *rb_optimized;
	GtkWidget *button_intro_lucene;
	GtkWidget *rb_attributes;
	GtkWidget *button_intro_attributes;
	GtkWidget *rb_strongs;
	GtkWidget *rb_morphs;
	GtkWidget *rb_footnotes;
	//GtkWidget *rb_indexed_word;
	GtkWidget *cb_case_sensitive;
	GtkWidget *ctree;
	GtkWidget *treeview;
	GtkWidget *mod_sel_dlg_treeview;
	GtkWidget *progressbar;
	GtkWidget *cb_include_strongs;
	GtkWidget *cb_include_morphs;
	GtkWidget *cb_include_footnotes;
	GtkWidget *listview_results;
	GtkWidget *listview_verses;
	GtkWidget *menu_item_send_search;
	//      GtkWidget *textview_preview;
	//      GtkTextBuffer *text_buffer;
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

	gchar *search_mod;
};
extern SEARCH_DIALOG1 search1;
extern gchar *verse_selected;

void gui_create_search_dialog(void);
void gui_set_drop_target(GtkWidget *target);
void on_comboboxentry2_changed(GtkComboBox *combobox,
			       gpointer user_data);
void button_clean(GtkButton *button, gpointer user_data);
void button_save(GtkButton *button, gpointer user_data);
void button_export(GtkButton *button, gpointer user_data);
void _on_destroy(GtkWidget *dialog, gpointer user_data);
void on_button_begin_search(GtkButton *button,
			    gpointer user_data);
void on_togglebutton_show_main(GtkToggleButton *togglebutton,
			       gpointer user_data);
void list_name_changed(GtkEditable *editable, gpointer user_data);
void range_name_changed(GtkEditable *editable,
			gpointer user_data);
void range_text_changed(GtkEditable *editable,
			gpointer user_data);
void new_modlist(GtkButton *button, gpointer user_data);
void clear_modules(GtkButton *button, gpointer user_data);
void delete_module(GtkButton *button, gpointer user_data);
void save_modlist(GtkButton *button, gpointer user_data);
void new_range(GtkButton *button, gpointer user_data);
void save_range(GtkButton *button, gpointer user_data);
void delete_range(GtkButton *button, gpointer user_data);
void delete_list(GtkButton *button, gpointer user_data);
void scope_toggled(GtkToggleButton *togglebutton,
		   gpointer user_data);
void mod_list_toggled(GtkToggleButton *togglebutton,
		      gpointer user_data);
void optimized_toggled(GtkToggleButton *togglebutton,
		       gpointer user_data);
void on_lucene_intro_clicked(GtkButton *button,
			     gpointer user_data);
void attributes_toggled(GtkToggleButton *togglebutton,
			gpointer user_data);
void on_attributes_intro_clicked(GtkButton *button,
				 gpointer user_data);
void current_module_toggled(GtkToggleButton *togglebutton,
			    gpointer user_data);
void on_closebutton2_clicked(GtkButton *button,
			     gpointer user_data);
void _on_dialog2_response(GtkDialog *dialog, gint response_id,
			  gpointer user_data);
void on_toolbutton12_clicked(GtkToolButton *toolbutton,
			     gpointer user_data);
void _on_dialog_response(GtkDialog *dialog, gint response_id,
			 gpointer user_data);

GtkWidget *create_results_menu_advsearch(void);
void on_send_list_via_biblesync_advsearch_activate(GtkMenuItem *
						       menuitem,
						   gpointer
						       user_data);

#ifdef __cplusplus
}
#endif
#endif
