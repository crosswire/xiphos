/*
 * GnomeSword Bible Study Tool
 * search_dialog.cc - glue  (: very sticky :)
 *
 * Copyright (C) 2004 GnomeSword Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gnome.h>
#include <regex.h>
#include <swbuf.h>
#include <swmodule.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>
#ifdef __cplusplus
}
#endif
	

#include "main/search_dialog.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"
 
#include "gui/search_dialog.h"
#include "gui/sidebar.h"
#include "gui/widgets.h"
#include "gui/html.h"
#include "gui/dialog.h"
#include "gui/utilities.h"

#include "backend/sword_main.hh"

#define SEARCHING _("Searching the ")
#define SMODULE _(" Module")
#define FINDS _("found in")		
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"

static BackEnd *backendSearch;

static gboolean is_running = FALSE;
extern int search_dialog;
static GList *get_current_list(void);
static gchar *get_modlist_string(GList * mods);
static GList *get_custom_list_from_name(const gchar * label);
static void add_ranges(void);
static void add_modlist(void);

int drag_module_type;

static GList *list_of_finds;
/******************************************************************************
 * Name
 *  search_percent_update
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   void search_percent_update(char percent, void *userData)	
 *
 * Description
 *    updates the progress bar during shortcut bar search
 *
 * Return value
 *   void
 */ 

void main_dialog_search_percent_update(char percent, void *userData)
{
	char maxHashes = *((char *) userData);
	float num;
	char buf[80];
	static char printed = 0;
	
	/* update search dialog progress */
	while ((((float) percent) / 100) * maxHashes > printed) {
		sprintf(buf, "%f", (((float) percent) / 100));
		num = (float) percent / 100;
		gui_set_progressbar_fraction(search1.progressbar, (gdouble)num);
		/*gnome_appbar_set_progress_percentage(
			(GnomeAppBar *)search1.progressbar, num);*/
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
}

/******************************************************************************
 * Name
 *   main_range_text_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void main_range_text_changed(GtkEditable * editable,
 *			       gpointer user_data)
 *
 * Description
 *   text in the range text entry has changed
 *   text is sent to the backend to get the search range list
 *   then list_ranges is updated to the range list
 *   and the range text in the clist_range is updated
 *
 * Return value
 *   void
 */

void main_range_text_changed(GtkEditable * editable)
{
	const gchar *entry;
	gchar *buf = NULL;
	gint count;
	gint i = 0;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeModel *model_list_ranges;
	GtkListStore *store_list_ranges;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GtkTreeIter iter;
	GList *tmp = NULL;

	/*    */
	model_list_ranges =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.list_ranges));
	store_list_ranges = GTK_LIST_STORE(model_list_ranges);
	/*    */
	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.list_range_name));
	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;

	gtk_list_store_clear(store_list_ranges);
	entry = gtk_entry_get_text(GTK_ENTRY(editable));
	//count = start_parse_range_list(entry);
	tmp = backend->parse_range_list(entry);
	while (tmp) {
		buf = (gchar*)tmp->data;
		if (!buf)
			break;
		gtk_list_store_append(store_list_ranges, &iter);
		gtk_list_store_set(store_list_ranges, &iter,
				   0, buf, -1);
		g_free(buf);
		tmp = g_list_next(tmp);
	}

	gtk_list_store_set(list_store, &selected, 1, entry, -1);
}



/******************************************************************************
 * Name
 *   set_search_global_option
 *
 * Synopsis
 *   #include "main/search1.h"
 *
 *   void set_search_global_option(gchar * option, gchar * choice)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void set_search_global_option(gchar * option, gboolean choice)
{	
	char *on_off;
	SWMgr *mgr = backendSearch->get_display_mgr();

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}
	mgr->setGlobalOption(option, on_off);
	GS_print(("option = %s is %s\n",option,on_off));
}


/******************************************************************************
 * Name
 *   save_modlist
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void save_modlist(GtkButton * button, gpointer user_data)
 *
 * Description
 *   saves the custom module list
 *
 * Return value
 *   void
 */

void main_save_modlist(void)
{
	gint i = 0;
	gchar *text1 = NULL;
	gchar *text2 = NULL;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	list_store = GTK_LIST_STORE(model);

	if (!gtk_tree_model_get_iter_first(model, &iter))
		return;
	do {
		gtk_tree_model_get(model, &iter,
				   0, &text1, 1, &text2, -1);
		xml_set_list_item("modlists", "modlist", text1, text2);
		GS_warning((text2));
		g_free(text1);
		g_free(text2);
	} while (gtk_tree_model_iter_next(model, &iter));
	add_modlist();
}



/******************************************************************************
 * Name
 *   save_range
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void save_range(GtkButton * button, gpointer user_data)
 *
 * Description
 *   saves the custom range list
 *
 * Return value
 *   void
 */

void main_save_range(void)
{
	gint i = 0;
	gchar *text1 = NULL;
	gchar *text2 = NULL;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	list_store = GTK_LIST_STORE(model);

	if (!gtk_tree_model_get_iter_first(model, &iter))
		return;
	do {
		gtk_tree_model_get(model, &iter,
				   0, &text1, 1, &text2, -1);

		xml_set_list_item("ranges", "range", text1, text2);
		GS_warning((text2));
		g_free(text1);
		g_free(text2);
	} while (gtk_tree_model_iter_next(model, &iter));
	add_ranges();
}


/******************************************************************************
 * Name
 *   delete_range
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void delete_range(GtkButton * button, gpointer user_data)
 *
 * Description
 *   delete the selected custom range
 *
 * Return value
 *   void
 */

void main_delete_range(void)
{
	gchar *name_string = NULL;
	gint test;
	GS_DIALOG *info;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GString *str;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.list_range_name));

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;
	gtk_tree_model_get(model, &selected, 0, &name_string, -1);

	str = g_string_new("");
	info = gui_new_dialog();
	info->stock_icon = GTK_STOCK_DIALOG_WARNING;
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s %s",
			_("Delete Range?"),
			_("Are you sure you want to delete this range?"),
			name_string);
	info->label_top = str->str;
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_alert_dialog(info);
	if (test != GS_YES) {
		g_free(name_string);
		g_free(info);
		return;
	}

	gtk_list_store_remove(list_store, &selected);
	xml_remove_node("ranges", "range", name_string);
	--search1.list_rows;
	main_save_range();

	g_free(info);
	g_free(name_string);
	g_string_free(str,TRUE);
}



/******************************************************************************
 * Name
 *   add_module_finds
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void 
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_module_finds(GList * versekeys)
{
	gchar *buf,*buf2;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	GList *tmp = g_list_first(versekeys);

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.listview_verses));
	list_store = GTK_LIST_STORE(model);

	gtk_list_store_clear(list_store);

	while (tmp) {	
		buf =  (char*) tmp->data;
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, 
				   &iter,
				   0,buf, 
				   -1);
		tmp = g_list_next(tmp);
	}
}



/******************************************************************************
 * Name
 *   add_ranges
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void add_ranges(void)
 *
 * Description
 *   add ranges to clist_range
 *
 * Return value
 *   void
 */

static void add_ranges(void)
{
	gchar *buf[2];
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	list_store = GTK_LIST_STORE(model);

	gtk_list_store_clear(list_store);

	if (xml_set_section_ptr("ranges")) {
		if (xml_get_label()) {
			buf[0] = xml_get_label();
			buf[1] = xml_get_list();

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, 
					   &iter,
					   0, buf[0], 
					   1, buf[1], 
					   -1);
			g_free(buf[0]);
			g_free(buf[1]);
		}
		
		gui_clear_combo(search1.combo_range);
		while (xml_next_item()) {
			if (xml_get_label()) {
				buf[0] = xml_get_label();
				buf[1] = xml_get_list();

				gtk_list_store_append(list_store, &iter);
				gtk_list_store_set(list_store, 
						   &iter, 
						   0, buf[0], 
						   1, buf[1],
						   -1);
				gui_add_item_to_combo(search1.combo_range,
						   buf[0]);
				g_free(buf[0]);
				g_free(buf[1]);
			}
		}
		gui_set_combo_index(search1.combo_range,0);
	}
}


/******************************************************************************
 * Name
 *   add_modlist
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void add_modlist(void)
 *
 * Description
 *   add custom module lists at search dialog start and afer save
 *
 * Return value
 *   void
 */

static void add_modlist(void)
{
	gchar *buf[2];
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	list_store = GTK_LIST_STORE(model);

	gtk_list_store_clear(list_store);

	if (xml_set_section_ptr("modlists")) {
		if (xml_get_label()) {
			buf[0] = xml_get_label();
			buf[1] = xml_get_list();

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, 
					   &iter,
					   0, buf[0], 
					   1, buf[1], 
					   -1);
			g_free(buf[0]);
			g_free(buf[1]);
		}

		gui_clear_combo(search1.combo_list);
		while (xml_next_item()) {
			if (xml_get_label()) {
				buf[0] = xml_get_label();
				buf[1] = xml_get_list();

				gtk_list_store_append(list_store, &iter);
				gtk_list_store_set(list_store, 
						   &iter, 
						   0, buf[0], 
						   1, buf[1],
						   -1);
				gui_add_item_to_combo(search1.combo_list,
						   buf[0]);
				g_free(buf[0]);
				g_free(buf[1]);
			}
		}
		gui_set_combo_index(search1.combo_list,0);
	}
}


/******************************************************************************
 * Name
 *   change_mods_select_label
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void change_mods_select_label(char *mod_name)
 *
 * Description
 *   change module name to current search module name
 *   or list name if searching a list of modules
 *
 * Return value
 *   void
 */

void main_change_mods_select_label(char *mod_name)
{
	GString *str;	
	
	if (GTK_TOGGLE_BUTTON(search1.rb_current_module)->active) {
		str = g_string_new("");
		g_string_printf(str,"<b>%s</b>%s",Q_("Search: "),mod_name);
		gtk_label_set_markup(GTK_LABEL(search1.label_mod_select),
                                     str->str);
		g_string_free(str,TRUE);
	} else
		main_add_modlist_to_label();
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void main_delete_module(GtkTreeView *treeview)
{
	gint test;
	GS_DIALOG *info;
	GList *mods = NULL;
	gchar *mod_list;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GString *str;


	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(treeview));

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;


	str = g_string_new("");
	info = gui_new_dialog();
	info->stock_icon = GTK_STOCK_DIALOG_WARNING;
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			_("Remove Module?"),
			_("Are you sure you want to remove the selected module?"));
	info->label_top = str->str;
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_alert_dialog(info);
	if (test != GS_YES) {
		g_free(info);
		g_string_free(str,TRUE);		
		return;
	}
	gtk_list_store_remove(list_store, &selected);

	mods = get_current_list();
	mod_list = get_modlist_string(mods);

	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.module_lists));


	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	list_store = GTK_LIST_STORE(model);

	if (gtk_tree_selection_get_selected(selection, NULL, &selected))
		if (mod_list) {
			gtk_list_store_set(list_store, &selected, 1,
					   mod_list, -1);
			g_free(mod_list);
		}
	g_free(info);
	g_string_free(str,TRUE);
}


void main_add_mod_to_list(GtkWidget * tree_widget, gchar * mod_name)
{
	GtkTreeModel *model_mods;
	GtkTreeModel *model_modules_lists;
	GtkTreeIter iter;
	GtkTreeIter selected_modules_lists;
	GtkListStore *list_store;
	GtkListStore *store_modules_lists;
	gchar *mod_description = NULL;
	gchar *mod_list = NULL;
	GList *mods = NULL;
	GtkTreeSelection *selection_modules_lists;
	
	model_modules_lists =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	store_modules_lists = GTK_LIST_STORE(model_modules_lists);

	selection_modules_lists = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.module_lists));
	    
	model_mods =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));
	list_store = GTK_LIST_STORE(model_mods);
	mod_description = backendSearch->module_description(mod_name);

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter,
					   0, mod_description,
					   1, mod_name, -1);
			mods = get_current_list();
			mod_list = get_modlist_string(mods);

			if (mod_list) {
				gtk_tree_selection_get_selected
	   			       (selection_modules_lists, NULL, 
					          &selected_modules_lists);
				
				gtk_list_store_set(store_modules_lists,
						   &selected_modules_lists,
						   1, mod_list, -1);
				g_free(mod_list);
			}
			++search1.module_count;
}

void main_drag_drop_display_in_main(const char * uri)
{


	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book),
						      1);

}


/******************************************************************************
 * Name
 *   mod_selection_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void mod_selection_changed(GtkTreeSelection * selection,
 *		      GtkWidget * tree_widget)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_mod_selection_changed(GtkTreeSelection * selection,
				  GtkWidget * tree_widget)
{
	gchar *mod = NULL;
	gchar *mod_description = NULL;
	gchar *mod_list = NULL;
	GList *mods = NULL;
	GtkListStore *store_modules_lists;
	GtkListStore *list_store;
	GtkTreeSelection *selection_modules_lists;
	GtkTreeIter selected_modules_lists;
	GtkTreeIter selected;
	GtkTreeIter iter;
	GtkTreeModel *model_mods;
	GtkTreeModel *model_modules_lists;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));


	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	
	model_mods =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.listview_modules));
	list_store = GTK_LIST_STORE(model_mods);


	model_modules_lists =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	store_modules_lists = GTK_LIST_STORE(model_modules_lists);

	selection_modules_lists = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.module_lists));

	if (gtk_tree_model_iter_has_child(model, &selected))
		return;

	gtk_tree_model_get(model, &selected, 0, &mod, -1);
	if (mod) {
		mod_description = 
			backendSearch->module_description(mod);

		if (GTK_TOGGLE_BUTTON(search1.rb_current_module)->active) {
			search1.search_mod = g_strdup(mod);
			search1.module_count = 1;
		} else {
			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter,
					   0, mod_description,
					   1, mod, -1);
			mods = get_current_list();
			mod_list = get_modlist_string(mods);

			if (mod_list) {
				gtk_tree_selection_get_selected
	   			       (selection_modules_lists, NULL, 
					          &selected_modules_lists);
				
				gtk_list_store_set(store_modules_lists,
						   &selected_modules_lists,
						   1, mod_list, -1);
				g_free(mod_list);
			}
			++search1.module_count;
		}
		main_change_mods_select_label(mod);
		g_free(mod);
	}
}


/******************************************************************************
 * Name
 *    main_selection_finds_list_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void main_selection_finds_list_changed(GtkTreeSelection * selection,
 *		     					 gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_selection_finds_list_changed(GtkTreeSelection *
					    selection, gpointer data)
{
	gchar *text, *path_str;
	GList *tmp = NULL;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkListStore *list_store;
	GtkTreeIter selected;
	GtkTreePath *path;
	
 	if (!gtk_tree_selection_get_selected (selection,&model,&selected))
		return;
	gtk_tree_model_get(model, &selected, 0, &text, -1);
	path = gtk_tree_model_get_path(model,&selected);
	path_str = gtk_tree_path_to_string(path);
	GS_message(("\npath: %s\ntext: %s",path_str,text));
	tmp = g_list_nth(list_of_finds,atoi(path_str));
	tmp = (GList*)tmp->data;
	add_module_finds(tmp);
	
	if (text)
		g_free(text);
	if (path_str)
		g_free(path_str);
	gtk_tree_path_free(path);
	
}


/******************************************************************************
 * Name
 *    main_finds_verselist_selection_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void main_finds_verselist_selection_changed(GtkTreeSelection * selection,
 *		     					 gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_finds_verselist_selection_changed(GtkTreeSelection * selection, 
					    gpointer data)
{
	gchar *text, *text_str, *buf, *module, *key;
	gint textlen;
	GtkTreeModel *model;
	GtkTreeIter selected;

 	if (!gtk_tree_selection_get_selected (selection, &model, &selected))
		return;
	gtk_tree_model_get(model, &selected, 0, &text, -1);
	GS_message(("\ntext: %s", text));
	textlen = strlen(text);
	module = text;

	// first `:' finds end of module name.
	buf = strchr(text, ':');
	*(module+(buf-text)) = '\0';	// strncpy unreliably terminates?

	// key begins 2 chars after module.
	key = buf + 2;
					     
	if (verse_selected)
		g_free(verse_selected);
	drag_module_type = backendSearch->module_type(module);
	if ((drag_module_type == BOOK_TYPE) ||
	    (drag_module_type == DICTIONARY_TYPE)) {
		verse_selected = g_strdup_printf("sword://%s/%lu", module,
						 backendSearch->
						 get_treekey_offset_from_key
						 (module, key));
	}
	else {
		if (drag_module_type == TEXT_TYPE) {
			// now find verse spec's `:'; then SPC to end key.
			buf = strchr(buf+1, ':');
			buf = strchr(buf, ' ');
			*buf = '\0';
		}
		verse_selected = g_strdup_printf("sword://%s/%s", module, key);
	}
	text_str = backendSearch->get_render_text(module,key);
	
	gtk_html_load_from_string(GTK_HTML(search1.preview_html),
				  text_str, strlen(text_str));

	GS_message(("main_finds_verselist_selection_changed: %s %s", module, key));

	if (text)
		g_free(text);
	g_free(text_str);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void (GtkTreeSelection * selection,
 *		     					 gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_selection_modules_lists_changed(GtkTreeSelection *
					    selection, gpointer data)
{
	gchar *name, *modules, *module[2];
	GList *tmp = NULL;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkListStore *list_store;
	GtkTreeIter selected;

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	gtk_tree_model_get(model, &selected, 0, &name, 1, &modules, -1);

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.listview_modules));
	list_store = GTK_LIST_STORE(model);

	gtk_entry_set_text(GTK_ENTRY(search1.entry_list_name), name);

	gtk_list_store_clear(list_store);
	tmp =
	    get_custom_list_from_name(gtk_entry_get_text
				      (GTK_ENTRY
				       (search1.entry_list_name)));

	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter,
			0,
		       backendSearch->module_description((gchar *)
			tmp->data), 1,
			(gchar *) tmp->data, -1);
		g_free((gchar *) tmp->data);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);

	if (!GTK_TOGGLE_BUTTON(search1.rb_current_module)->active)
		main_add_modlist_to_label();
	g_free(name);
	g_free(modules);

}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

gchar *get_modlist_string(GList * mods)
{
	gchar *rv;
	GString *str;

	str = g_string_new("");

	while (mods != NULL) {
		str = g_string_append(str, (gchar *) mods->data);
		mods = g_list_next(mods);
		if (mods != NULL)
			str = g_string_append_c(str, ',');
	}
	g_list_free(mods);
	rv = g_strdup(str->str);
	g_string_free(str, TRUE);
	return rv;
}



/******************************************************************************
 * Name
 *   add_modlist_to_label
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void add_modlist_to_label(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_add_modlist_to_label(void)
{
	GList *mods = NULL;
	gchar *mod_list;
	GString *str;

	mods = get_current_list();
	mod_list = get_modlist_string(mods);
	str = g_string_new("");
	g_string_printf(str,"<b>%s</b>%s",Q_("Search: "),mod_list);
	gtk_label_set_markup(GTK_LABEL(search1.label_mod_select),
			     str->str);
	g_free(mod_list);
	g_string_free(str,TRUE);
}



void main_comboboxentry2_changed(GtkComboBox * combobox, gpointer user_data)
{
	GList *mod_list = NULL;
	gchar *str = NULL;
	gchar *mod_list_str = NULL;
	const gchar *name = NULL;
	
	if (!GTK_TOGGLE_BUTTON(search1.rb_custom_list)->active)
		return;
	name = gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combobox)->child)); 
	mod_list = get_custom_list_from_name(name);
	mod_list_str = get_modlist_string(mod_list);
	str = g_strdup_printf("<b>%s</b>%s",Q_("Search: "),mod_list_str);
	gtk_label_set_markup(GTK_LABEL(search1.label_mod_select), str);
	g_free(mod_list_str);
	g_free(str);
}
/******************************************************************************
 * Name
 *   gui_add_to_found_list
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void gui_add_to_found_list(gchar * result_text, gchar * module)
 *
 * Description
 *   loads result_text (key) and modlue (name) to the results html widget
 *   we got here from  
 *   on_button_begin_search_clicked -> 
 *   do_dialog_search -> 
 *   backend_do_dialog_search ->
 *   add_to_found_list ->
 *
 * Return value
 *   void
 */

static void add_to_found_list(gchar * result_text, gchar * module)
{
	//gchar buf[256];
	GString *str;
	gchar *buf[2];
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_verses));
	list_store = GTK_LIST_STORE(model);

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, 
			   &iter,
			   0, module, 
			   1, result_text, 
			   -1);
		
	if (result_text) /* allocated by g_convert() in BackEnd::get_next_listkey() */
		g_free(result_text);
	g_string_free(str, TRUE);
}
/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void check_search_global_options(void)
{
	set_search_global_option("Strong's Numbers",
				 GTK_TOGGLE_BUTTON(search1.
						   cb_include_strongs)->
				 active);

	set_search_global_option("Morphological Tags",
				 GTK_TOGGLE_BUTTON(search1.
						   cb_include_morphs)->
				 active);

	set_search_global_option("Footnotes",
				 GTK_TOGGLE_BUTTON(search1.
						   cb_include_footnotes)->
				 active);
}


/******************************************************************************
 * Name
 *   get_current_search_mod
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   GList *get_current_search_mod(void)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

static GList *get_current_search_mod(void)
{
	GList *items = NULL;

	items =
	    g_list_append(items, (gchar *) g_strdup(search1.search_mod));
	return items;
}


/******************************************************************************
 * Name
 *   get_current_list
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   GList *get_current_list(void)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

static GList *get_current_list(void)
{
	GList *items = NULL;
	gchar *buf;
	gint i = 0;
	GtkTreeModel *model;
	GtkTreeIter iter;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.listview_modules));
	if (gtk_tree_model_get_iter_first(model, &iter)) {
		do {
			gtk_tree_model_get(model, &iter, 1, &buf, -1);
			items =
			    g_list_append(items,
					  (gchar *) g_strdup(buf));

		} while (gtk_tree_model_iter_next(model, &iter));
	}
	return items;
}



/******************************************************************************
 * Name
 *   get_custom_list_from_name
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   gchar *get_custom_list_from_name(gchar * name)
 *
 * Description
 *   used by setup_search() - returns the search range (scope)
 *
 * Return value
 *   gchar *
 */

static GList *get_custom_list_from_name(const gchar * label)
{
	GList *items = NULL;
	gchar *mod_list = NULL;
	gchar *t;
	gchar *token;

	search1.module_count = 0;
	t = ",";

	mod_list =
	    (gchar *) xml_get_list_from_label("modlists", "modlist",
					      label);
	token = strtok(mod_list, t);
	++search1.module_count;
	while (token != NULL) {
		++search1.module_count;
		items = g_list_append(items, (gchar *)
				      g_strdup(token));
		token = strtok(NULL, t);
	}
	return items;
}


/******************************************************************************
 * Name
 *   set_up_search
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void set_up_search(void)
 *
 * Description
 *   checks for and setups some of the search options
 *
 * Return value
 *   void
 */

static void set_up_dialog_search(void)
{
	const gchar *label;
	gchar *range = NULL;

	//gui_begin_html(search1.results_html, TRUE);
	backendSearch->clear_scope();
	if (GTK_TOGGLE_BUTTON(search1.rb_custom_range)->active) {
		backendSearch->clear_search_list();
		label =
		    gtk_entry_get_text(GTK_ENTRY(GTK_BIN(search1.combo_range)->child));
		range =
		    (gchar *) xml_get_list_from_label("ranges", "range",
						      label);
		if (range) {
			backendSearch->set_range(range);
			backendSearch->set_scope2range();
		}
	}

	else if (GTK_TOGGLE_BUTTON(search1.rb_last)->active) {
		backendSearch->set_scope2last_search();
	}

	else {
		backendSearch->clear_search_list();
	}
}



/******************************************************************************
 * Name
 *   main_get_rendered_text
 *
 * Synopsis
 *   #include "main/sword.h"
 *
 *   char *main_get_rendered_text(char *module_name, char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *main_get_search_rendered_text(char *module_name, char *key)
{
	check_search_global_options();
	return backendSearch->get_render_text(module_name, key);
}

static
void _clear_find_lists(void)
{
	GList *tmp = NULL;
	gchar *tmp_buf = NULL;
	
	list_of_finds = g_list_first(list_of_finds);
	while (list_of_finds) {
		tmp = (GList*) list_of_finds->data;
		while (tmp) {
			tmp_buf = (char*) tmp->data;
			GS_message((tmp_buf));
			if (tmp_buf) g_free(tmp_buf);
			tmp = g_list_next(tmp);
		}
		list_of_finds = g_list_next(list_of_finds);
	}
	if (list_of_finds)
		g_list_free(list_of_finds);
	list_of_finds = NULL;
}

void main_do_dialog_search(void)
{
	gint search_type, search_params, finds;
	const gchar *search_string;
	const gchar *attribute_search_string = NULL;
	gchar *module;
	gchar buf[256], *utf8str;
	gchar *tmp_buf;
	gint utf8len;
	GList *search_mods = NULL;
	const gchar *key_buf;
	GString *str;
	GList *tmp = NULL;
	GList *tmp_list = NULL;
	SWBuf swbuf = "";
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	GtkTreeModel *model2;
	GtkListStore *list_store2;
	GtkTreeIter iter2;
	gint x = 0;
	gint mod_type;
	
	_clear_find_lists();
	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.listview_results));
	list_store = GTK_LIST_STORE(model);

	gtk_list_store_clear(list_store);
	model2 =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.listview_verses));
	list_store2 = GTK_LIST_STORE(model2);

	gtk_list_store_clear(list_store2);
	
	
	search_string =
	    gtk_entry_get_text(GTK_ENTRY(search1.search_entry));
	
	if (strlen(search_string) < 1)
		return;
	str = g_string_new("");

	set_up_dialog_search();

	gtk_notebook_set_current_page(GTK_NOTEBOOK(search1.notebook), 1);
	//search1.notebook
	search_type = 
	    GTK_TOGGLE_BUTTON(search1.rb_regexp)->active ? 0 :
	    GTK_TOGGLE_BUTTON(search1.rb_exact_phrase)->active ? -1 :
	    GTK_TOGGLE_BUTTON(search1.rb_words)->active ? -2 : -3;
	   // GTK_TOGGLE_BUTTON(search1.rb_attributes)->active ? -4 : -3;
	GS_message(("search_type = %d",search_type));

	search_params = GTK_TOGGLE_BUTTON
	    (search1.cb_case_sensitive)->active ? 0 : REG_ICASE;

	if(search_type == -3) {
		if(GTK_TOGGLE_BUTTON(search1.rb_strongs)->active) {
			// For attribute-based searches, e.g. "Word//Lemma/G140",
			// we must constrain the match to whole words.  Otherwise,
			// we will inadvertently return e.g. 140 plus 1401 and 1404.		
			search_params |= SEARCHFLAG_MATCHWHOLEENTRY;
			attribute_search_string = g_strdup_printf(
					"Word//Lemma/%s",
					search_string);
		} else if(GTK_TOGGLE_BUTTON(search1.rb_morphs)->active) {
			// For attribute-based searches, e.g. "Word//Lemma/G140",
			// we must constrain the match to whole words.  Otherwise,
			// we will inadvertently return e.g. 140 plus 1401 and 1404.		
			search_params |= SEARCHFLAG_MATCHWHOLEENTRY;
			attribute_search_string = g_strdup_printf(
					"Word//Morph/%s",
					search_string);
		} else if(GTK_TOGGLE_BUTTON(search1.rb_footnotes)->active) {
			attribute_search_string = g_strdup_printf(
					"Footnote//body/%s",
					search_string);
		}
		GS_message((attribute_search_string));
	}
	if (GTK_TOGGLE_BUTTON(search1.rb_custom_list)->active) {
		const gchar *name;
		name =
		    gtk_entry_get_text(GTK_ENTRY(
				GTK_BIN(search1.combo_list)->child));
		search_mods = get_custom_list_from_name(name);
	} else if (GTK_TOGGLE_BUTTON(search1.rb_mod_list)->active) {
		search_mods = get_current_list();
	} else
		search_mods = get_current_search_mod();
	search_mods = g_list_first(search_mods);
	
	
	settings.searchType = search_type;
	//if (search_type != -4)
	check_search_global_options();

	
	
	while (search_mods != NULL) {
		module = (gchar *) search_mods->data;

		sprintf(buf, "%s %s %s", SEARCHING, module, SMODULE);
		
		gui_set_progressbar_text(search1.progressbar, buf);

		if (search_type == -2 || search_type == -4)
			search_type = backendSearch->check_for_optimal_search(module);
		GS_message(("search_type = %d",search_type));
		
		finds = backendSearch->do_module_search(module, 
					(attribute_search_string)?
					attribute_search_string:
					search_string,
					search_type, 
					search_params, 
					TRUE);
		
		tmp_list = g_list_first(tmp_list);
		tmp_list = NULL;
		
		while ((key_buf = backendSearch->get_next_listkey()) != NULL) {
			/* test for mod type */
			mod_type = backendSearch->module_type(module);	
			if (mod_type == TEXT_TYPE)			
				g_string_printf(str, "%s: %s  %s", module,  key_buf, backendSearch->get_strip_text(module, key_buf));
			else			
				g_string_printf(str, "%s: %s", module,  key_buf);				
			tmp_list = g_list_append(tmp_list, (char*) g_strdup(str->str));
			g_free((char*)key_buf);	
		}
		list_of_finds = g_list_append(list_of_finds, (GList*)tmp_list);
		// add number of hits in each module to finds listview
		g_string_printf(str, "%d %s %s",
				finds, FINDS, module);
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, 
				   &iter,
				   0, str->str, 
				   -1);
		++x;
		if (x == 1) { // add verse list for hits in first module to verse listview
			tmp = (GList*) list_of_finds->data;
			add_module_finds(g_list_first(tmp));
		}
		g_free(module);
		search_mods = g_list_next(search_mods);
	}
	if(attribute_search_string)
		g_free((gchar*)attribute_search_string);
	g_list_free(search_mods);
	gui_set_progressbar_text(search1.progressbar, _("Search finished"));
	gui_set_progressbar_fraction(search1.progressbar, 0);
	g_string_free(str, TRUE);	
}

/******************************************************************************
 * Name
 *   main_open_search_dialog
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void main_open_search_dialog(void)
 *
 * Description
 *   create and setup the search dialog
 *
 * Return value
 *   void
 */

void main_open_search_dialog(void)
{
	if (!is_running) {
		GtkTreeModel *model;
		GtkListStore *list_store;
		GtkTreeIter iter;
		
		backendSearch = new BackEnd();
		
		/* create and show search dialog */
		gui_create_search_dialog();
		backendSearch->init_SWORD(2);
		
		
		/* initiate module count to 0 */
		search1.module_count = 0;

		/* add custom ranges to list_ranges */
		add_ranges();

		/* add custom modlist to module_list */
		add_modlist();

		/* set search module to current module  */
		search1.search_mod = settings.MainWindowModule;
		main_change_mods_select_label(settings.MainWindowModule);
		/* add one to module count */
		++search1.module_count;

		search1.show_in_main = FALSE;
		is_running = TRUE;
	} else
		gdk_window_raise(GTK_WIDGET(search1.dialog)->window);
	search_dialog = TRUE;
}

void main_close_search_dialog(void)
{
	_clear_find_lists();
	is_running = FALSE;
	delete backendSearch;
}
