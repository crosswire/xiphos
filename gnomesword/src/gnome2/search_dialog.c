/*
 * GnomeSword Bible Study Tool
 * search_dialog.c - gui for searching Sword modules
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <glade/glade-xml.h>

#include <gtkhtml/gtkhtml.h>

#include <regex.h>
#include <string.h>
#include <glib.h>

#include "gui/search_dialog.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/dialog.h"
#include "gui/utilities.h"

#include "main/search_dialog.h"
#include "main/configs.h"
#include "main/lists.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#define SEARCHING N_("Searching the ")
#define SMODULE N_(" Module")
#define FINDS N_("found in ")
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"





SEARCH_DIALOG1 search1;
static GtkWidget *dialog;
static gboolean _preview_on;
static gboolean _in_url;



void on_comboboxentry2_changed(GtkComboBox * combobox, gpointer user_data)
{
	main_comboboxentry2_changed(combobox, user_data);
	//g_message("on_comboboxentry2_changed");
}


/******************************************************************************
 * Name
 *   button_clean
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void button_clean(GtkButton * button, 
 *						gpointer user_data)
 *
 * Description
 *   FIXME: this is really ugly :(
 *
 * Return value
 *   void
 */

void button_clean(GtkButton * button, gpointer user_data)
{
	gboolean editable;

	editable = gtk_html_get_editable(GTK_HTML(search1.report_html));
	if (!editable)
		gtk_html_set_editable(GTK_HTML(search1.report_html),
				      TRUE);
	gtk_html_select_all(GTK_HTML(search1.report_html));
	gtk_html_cut(GTK_HTML(search1.report_html));
	gtk_html_set_editable(GTK_HTML(search1.report_html), editable);


	editable =
	    gtk_html_get_editable(GTK_HTML(search1.results_html));
	if (!editable)
		gtk_html_set_editable(GTK_HTML(search1.results_html),
				      TRUE);
	gtk_html_select_all(GTK_HTML(search1.results_html));
	gtk_html_cut(GTK_HTML(search1.results_html));
	gtk_html_set_editable(GTK_HTML(search1.results_html), editable);

	editable =
	    gtk_html_get_editable(GTK_HTML(search1.preview_html));
	if (!editable)
		gtk_html_set_editable(GTK_HTML(search1.preview_html),
				      TRUE);
	gtk_html_select_all(GTK_HTML(search1.preview_html));
	gtk_html_cut(GTK_HTML(search1.preview_html));
	gtk_html_set_editable(GTK_HTML(search1.preview_html), editable);


}

/******************************************************************************
 * Name
 *   button_save
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void button_save(GtkButton * button, 
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void button_save(GtkButton * button, gpointer user_data)
{
	g_printf("FIXME: please");
}


/******************************************************************************
 * Name
 *   on_destroy
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_destroy(GtkWidget * dialog, 
 *				    gpointer user_data)
 *
 * Description
 *   destroy the search dialog
 *
 * Return value
 *   void
 */

void on_destroy(GtkWidget * dialog, gpointer user_data)
{
	main_close_search_dialog();
	gtk_widget_destroy(search1.dialog);
}




/******************************************************************************
 * Name
 *   on_button_begin_search
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_button_begin_search(GtkButton * button, gpointer user_data)
 *
 * Description
 *   starts the search 
 *
 * Return value
 *   void
 */

void on_button_begin_search(GtkButton * button, gpointer user_data)
{
	g_message("on_button_begin_search");
	main_do_dialog_search();
}


/******************************************************************************
 * Name
 *   on_togglebutton_show_main
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_togglebutton_show_main(GtkToggleButton * togglebutton,
 *						    gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void on_togglebutton_show_main(GtkToggleButton * togglebutton,
			       gpointer user_data)
{
	search1.show_in_main = togglebutton->active;
}


/******************************************************************************
 * Name
 *   url
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void url(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   called when mouse moves over an url (link)
 *
 * Return value
 *   void
 */

static void _url(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar *buf = NULL;
	gchar *modbuf = NULL;
	gchar *text = NULL;
	gchar *newmod;
	gchar *newref;
	gint i = 0, havemod = 0;


	/***  moved out of url  ***/
	if (url == NULL) {
		_in_url = FALSE;
		return;
	} else
		_in_url = TRUE;

/*	if (!_preview_on)
		return;
*/
	/***  we are in an url  ***/
	newmod = g_new0(gchar, strlen(url));
	newref = g_new0(gchar, strlen(url));
	/*** thml verse reference ***/
	if (!strncmp(url, "version=", 7)
	    || !strncmp(url, "passage=", 7)) {
		gchar *mybuf = NULL;
		mybuf = strstr(url, "version=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			i = 0;
			while (mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i + 1] = '\0';
				++i;
				++havemod;
			}
		}
		mybuf = NULL;
		mybuf = strstr(url, "passage=");
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			while (i < strlen(mybuf)) {
				newref[i] = mybuf[i];
				newref[i + 1] = '\0';
				++i;
			}
		}
		if (havemod > 2) {
			modbuf = newmod;
		} else {
			modbuf = settings.MainWindowModule;
		}
		buf = g_strdup(newref);

		if (buf) {
			gchar *str;

			text =
			    main_get_search_rendered_text(modbuf, buf);
			if (text) {
				main_entry_display(search1.preview_html,
						   modbuf, text, buf,
						   TRUE);
				free(text);
			}
			g_free(buf);
		}
	}
	g_free(newmod);
	g_free(newref);
}


/******************************************************************************
 * Name
 *   link_clicked
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void link_clicked(GtkHTML * html, const gchar * url, gpointer data)
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

static void _link_clicked(GtkHTML * html, const gchar * url,
			  gpointer data)
{
	gchar *buf = NULL, *modbuf = NULL;
	gchar *newmod, *newref;
	gint i = 0, havemod = 0;
	gchar *text = NULL;
	gchar *url2 = NULL;

	if (_preview_on)
		_preview_on = FALSE;
	else
		_preview_on = TRUE;


	newmod = g_new0(gchar, strlen(url));
	newref = g_new0(gchar, strlen(url));
	/*** thml verse reference ***/
	if (!strncmp(url, "version=", 7)
	    || !strncmp(url, "passage=", 7)) {
		gchar *mybuf = NULL;
		mybuf = strstr(url, "version=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			i = 0;
			while (mybuf[i] != ' ') {
				newmod[i] = mybuf[i];
				newmod[i + 1] = '\0';
				++i;
				++havemod;
			}
		}
		mybuf = NULL;
		mybuf = strstr(url, "passage=");
		i = 0;
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			while (i < strlen(mybuf)) {
				newref[i] = mybuf[i];
				newref[i + 1] = '\0';
				++i;
			}
		}
		if (havemod > 2) {
			modbuf = newmod;
		} else {
			modbuf = settings.MainWindowModule;
		}
		buf = g_strdup(newref);
		settings.displaySearchResults = TRUE;

		url2 = g_strdup_printf("sword://%s/%s", modbuf, buf);

		switch (GPOINTER_TO_INT(data)) {
		case 0:
			if (search1.show_in_main ||
			    gtk_toggle_tool_button_get_active
			    (GTK_TOGGLE_TOOL_BUTTON
			     (search1.togglebutton_show_main))) {

				main_url_handler(url2, TRUE);
			}

			text =
			    main_get_search_rendered_text(modbuf, buf);
			if (text) {
				main_entry_display(search1.preview_html,
						   modbuf, text, buf,
						   TRUE);
				free(text);
			}
			break;
		case 1:
			main_url_handler(url2, TRUE);
			break;
		}
		settings.displaySearchResults = FALSE;
		g_free(buf);
		g_free(url2);
	}
	g_free(newmod);
	g_free(newref);
}


/******************************************************************************
 * Name
 *   report_link_clicked
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void report_link_clicked(GtkHTML * html, const gchar * url,
 *				gpointer data)
 *
 * Description
 *   jumps to the start of the module list in result_html
 *
 * Return value
 *   void
 */

static void report_link_clicked(GtkHTML * html, const gchar * url,
				gpointer data)
{
	gtk_html_jump_to_anchor(GTK_HTML(search1.results_html), url);
}


/******************************************************************************
 * Name
 *   list_name_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void list_name_changed(GtkEditable * editable,
 *			       gpointer user_data)
 *
 * Description
 *   text in the range name entry has changed
 *   name text in the clist_range is updated to match
 *
 * Return value
 *   void
 */

void list_name_changed(GtkEditable * editable, gpointer user_data)
{
	const gchar *text;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.module_lists));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.module_lists));

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;


	text = gtk_entry_get_text(GTK_ENTRY(editable));
	gtk_list_store_set(list_store, &selected, 0, text, -1);
}


/******************************************************************************
 * Name
 *   range_name_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void range_name_changed(GtkEditable * editable,
 *			       gpointer user_data)
 *
 * Description
 *   text in the range name entry has changed
 *   name text in the clist_range is updated to match
 *
 * Return value
 *   void
 */

void range_name_changed(GtkEditable * editable, gpointer user_data)
{
	const gchar *text;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.list_range_name));

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;


	text = gtk_entry_get_text(GTK_ENTRY(editable));
	gtk_list_store_set(list_store, &selected, 0, text, -1);
/*
	gtk_clist_set_text((GtkCList *) search1.list_range_name,
			   search1.custom_range_row, 0, text);
	*/
}


/******************************************************************************
 * Name
 *   range_text_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void range_text_changed(GtkEditable * editable,
 *			       gpointer user_data)
 *
 * Description
 *   text in the range text entry has changed
 *
 * Return value
 *   void
 */

void range_text_changed(GtkEditable * editable, gpointer user_data)
{
	main_range_text_changed(editable);
}


/******************************************************************************
 * Name
 *   new_modlist
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void new_modlist(GtkButton * button, gpointer user_data)
 *
 * Description
 *   adds a new custom module list to the 
 *
 * Return value
 *   void
 */

void new_modlist(GtkButton * button, gpointer user_data)
{
	gchar *text[2], buf[80];
	GList *mods = NULL;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeModel *model2;
	GtkListStore *list_store2;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	GtkTreePath *path;

	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.module_lists));


	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.listview_modules));
	list_store = GTK_LIST_STORE(model);

	model2 =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.module_lists));
	list_store2 = GTK_LIST_STORE(model2);

	sprintf(buf, "New List%d", search1.list_rows);

	search1.module_count = 0;
	gtk_list_store_clear(list_store);

	gtk_list_store_append(list_store2, &iter);
	gtk_list_store_set(list_store2, &iter, 0, buf, -1);
	path = gtk_tree_model_get_path(model2, &iter);
	gtk_tree_selection_select_path(selection, path);
	/*search1.custom_list_row = gtk_clist_append((GtkCList *) search1.
	   module_lists, text); */
	//search1.list_rows = search1.custom_list_row;
	gtk_entry_set_text(GTK_ENTRY(search1.entry_list_name), buf);
	gtk_tree_path_free(path);
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

void clear_modules(GtkButton * button, gpointer user_data)
{
	gint test;
	GS_DIALOG *info;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GString *str;

	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.module_lists));
	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.listview_modules));
	list_store = GTK_LIST_STORE(model);

	str = g_string_new("");
	info = gui_new_dialog();
	info->stock_icon = GTK_STOCK_DIALOG_WARNING;
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s",
			_("Clear List?"),
			_
			("Are you sure you want to clear the module list?"));
	info->label_top = str->str;
	info->yes = TRUE;
	info->no = TRUE;

	test = gui_alert_dialog(info);
	if (test != GS_YES) {
		g_free(info);
		g_string_free(str, TRUE);
		return;
	}

	gtk_list_store_clear(list_store);

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.module_lists));
	list_store = GTK_LIST_STORE(model);
	if (gtk_tree_selection_get_selected(selection, NULL, &selected))
		gtk_list_store_set(list_store, &selected, 1, "", -1);
	g_free(info);
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

void delete_module(GtkButton * button, gpointer user_data)
{
	main_delete_module(GTK_TREE_VIEW(search1.listview_modules));
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

void save_modlist(GtkButton * button, gpointer user_data)
{
	main_save_modlist();
}


/******************************************************************************
 * Name
 *   new_range
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void new_range(GtkButton * button, gpointer user_data)
 *
 * Description
 *   adds a new custom range to the clist_range
 *
 * Return value
 *   void
 */

void new_range(GtkButton * button, gpointer user_data)
{
	gchar *text[2];
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GtkTreeIter iter;
	GtkTreePath *path;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.list_range_name));

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;

	text[0] = "[New Range]";
	text[1] = "";

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, text[0], 1, text[1], -1);

	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_selection_select_path(selection, path);
	gtk_tree_path_free(path);

	gtk_entry_set_text(GTK_ENTRY(search1.entry_range_name),
			   text[0]);
	gtk_entry_set_text(GTK_ENTRY(search1.entry_range_text), "");
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

void save_range(GtkButton * button, gpointer user_data)
{
	main_save_range();
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

void delete_range(GtkButton * button, gpointer user_data)
{
	main_delete_range();
}


/******************************************************************************
 * Name
 *   delete_list
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void delete_list(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void delete_list(GtkButton * button, gpointer user_data)
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
				    (search1.module_lists));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.module_lists));

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;
	gtk_tree_model_get(model, &selected, 0, &name_string, -1);

	str = g_string_new("");
	info = gui_new_dialog();
	info->stock_icon = GTK_STOCK_DIALOG_WARNING;
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s %s",
			_("Delete list?"),
			_("Are you sure you want to delete:"),
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
	xml_remove_node("modlists", "modlist", name_string);
	--search1.list_rows;
	save_modlist(NULL, NULL);

	g_free(info);
	g_free(name_string);
	g_string_free(str, TRUE);
}


/******************************************************************************
 * Name
 *   scope_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void scope_toggled(GtkToggleButton *togglebutton,
						gpointer user_data)
 *
 * Description
 *   remember which scope button was pressed last
 *   does not remember rb_last
 *
 * Return value
 *   void
 */

void scope_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	search1.which_scope = togglebutton;
}


/******************************************************************************
 * Name
 *   mod_list_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void mod_list_toggled(GtkToggleButton *togglebutton,
						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void mod_list_toggled(GtkToggleButton * togglebutton,
		      gpointer user_data)
{
	if (togglebutton->active) {
		main_comboboxentry2_changed((GtkComboBox*)search1.combo_list, 
						user_data);
		//main_add_modlist_to_label();
	}
}


/******************************************************************************
 * Name
 *   current_module_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void current_module_toggled(GtkToggleButton *togglebutton,
						gpointer user_data)
 *
 * Description
 *   sets rb_last to insensitive if either use module list
 *   or use custom list is clicked
 *   also set scope button to last one used before rb_last
 *
 * Return value
 *   void
 */

void current_module_toggled(GtkToggleButton * togglebutton,
			    gpointer user_data)
{
	if (togglebutton->active) {
		main_change_mods_select_label(search1.search_mod);
		gtk_widget_set_sensitive(search1.rb_last, TRUE);
	} else {
		gtk_widget_set_sensitive(search1.rb_last, FALSE);
		gtk_toggle_button_set_active(search1.which_scope, TRUE);
	}
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

gboolean _on_button_release_event(GtkWidget * widget,
				  GdkEventButton * event, gpointer data)
{
	if (_in_url)
		return FALSE;

	if (GPOINTER_TO_INT(data))
		_preview_on = TRUE;
	return FALSE;
}



/******************************************************************************
 * Name
 *   indexed_word_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void indexed_word_toggled(GtkToggleButton *togglebutton,
						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void indexed_word_toggled(GtkToggleButton * togglebutton,
				 gpointer user_data)
{
/*	if (togglebutton->active) {
		//main_change_mods_select_label(search1.search_mod);
		gtk_widget_set_sensitive(search1.rb_last, FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(search1.rb_no_scope), TRUE);		
		gtk_widget_set_sensitive(search1.rb_no_scope, FALSE);
		gtk_widget_set_sensitive(search1.rb_custom_range, FALSE);
		gtk_widget_set_sensitive(search1.cb_case_sensitive, FALSE);
		gtk_widget_set_sensitive(search1.cb_include_strongs, FALSE);
		gtk_widget_set_sensitive(search1.cb_include_morphs, FALSE);
		gtk_widget_set_sensitive(search1.cb_include_footnotes, FALSE);
	}
	else {
		gtk_widget_set_sensitive(search1.rb_last, TRUE);
		gtk_widget_set_sensitive(search1.rb_no_scope, TRUE);
		gtk_widget_set_sensitive(search1.rb_custom_range, TRUE);
		gtk_widget_set_sensitive(search1.cb_case_sensitive, TRUE);
		gtk_widget_set_sensitive(search1.cb_include_strongs, TRUE);
		gtk_widget_set_sensitive(search1.cb_include_morphs, TRUE);
		gtk_widget_set_sensitive(search1.cb_include_footnotes, TRUE);
	}*/
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

static void mod_selection_changed(GtkTreeSelection * selection,
				  GtkWidget * tree_widget)
{
	main_mod_selection_changed(selection, tree_widget);
}

/******************************************************************************
 * Name
 *   selection_modules_lists_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void selection_modules_lists_changed(GtkTreeSelection * selection,
 *		     					 gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void selection_modules_lists_changed(GtkTreeSelection *
					    selection, gpointer data)
{
	main_selection_modules_lists_changed(selection, data);
}


/******************************************************************************
 * Name
 *   selection_range_lists_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void selection_range_lists_changed(GtkTreeSelection * selection,
 *		     					 gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void selection_range_lists_changed(GtkTreeSelection * selection,
					  gpointer data)
{
	gchar *name = NULL;
	gchar *range = NULL;
	GtkTreeModel *model;
	GtkTreeIter selected;

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	gtk_tree_model_get(model, &selected, 0, &name, 1, &range, -1);

	gtk_entry_set_text(GTK_ENTRY(search1.entry_range_name), name);
	gtk_entry_set_text(GTK_ENTRY(search1.entry_range_text), range);
	g_free(name);
	g_free(range);
}


/******************************************************************************
 * Name
 *   add_two_text_columns
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void add_two_text_columns(GtkTreeView * treeview)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void _add_two_text_columns(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);

	/* column for fixed toggles */
	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Module",
							  renderer,
							  "text", 0,
							  "text", 1,
							  NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);

	gtk_tree_view_append_column(treeview, column);
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

static
void _setup_combobox(GtkComboBox * combo)
{
	GtkListStore *store;

	store = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(combo, GTK_TREE_MODEL(store));
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo),
					    0);
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

static
void _setup_listviews(GtkWidget * listview, GCallback callback)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkListStore *model;
	GObject *selection;

	model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(listview),
				GTK_TREE_MODEL(model));
	_add_two_text_columns(GTK_TREE_VIEW(listview));
	if (!callback)
		return;
	selection =
	    G_OBJECT(gtk_tree_view_get_selection
		     (GTK_TREE_VIEW(listview)));
	g_signal_connect(selection, "changed", G_CALLBACK(callback),
			 NULL);

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

static
void _setup_treetview(GtkWidget * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GObject *selection;

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Found",
							  renderer,
							  "text",
							  0, NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	gui_load_module_tree(treeview, FALSE);

	selection =
	    G_OBJECT(gtk_tree_view_get_selection
		     (GTK_TREE_VIEW(treeview)));
	g_signal_connect(selection, "changed",
			 G_CALLBACK(mod_selection_changed), treeview);

}


static
void _setup_treetview2(GtkWidget * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	//GObject *selection;

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Found",
							  renderer,
							  "text",
							  0, NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	gui_load_module_tree(treeview, FALSE);

	/*selection =
	    G_OBJECT(gtk_tree_view_get_selection
		     (GTK_TREE_VIEW(treeview)));
	g_signal_connect(selection, "changed",
			 G_CALLBACK(mod_selection_changed), treeview);*/

}


void
on_treeview8_drag_begin                (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gpointer         user_data)
{
	g_message("on_treeview8_drag_begin");
}


void
on_treeview8_drag_data_get             (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        GtkSelectionData *data,
                                        guint            info,
                                        guint            time,
                                        gpointer         user_data)
{
	g_message("on_treeview8_drag_data_get");

}


void
on_treeview7_drag_data_received        (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        GtkSelectionData *data,
                                        guint            info,
                                        guint            time,
                                        gpointer         user_data)
{
	g_message("on_treeview7_drag_data_received");

}


gboolean
on_treeview7_drag_drop                 (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        guint            time,
                                        gpointer         user_data)
{

	g_message("on_treeview7_drag_drop");
  return FALSE;
}


void
on_treeview7_drag_end                  (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gpointer         user_data)
{
	g_message("on_treeview7_drag_end");

}


void
on_closebutton2_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide(search1.mod_sel_dialog);
}


void
on_toolbutton12_clicked                (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	gtk_widget_show(search1.mod_sel_dialog);
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

/* add html widgets */
static
void _add_html_widgets(GtkWidget * vbox_report,
		       GtkWidget * vbox_preview,
		       GtkWidget * vbox_results)
{
	
	GtkWidget *scrolledwindow;
/*	
#ifdef USE_GTKMOZEMBED 
	
#else	
*/	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox_report), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

	search1.report_html = gtk_html_new();
	gtk_widget_show(search1.report_html);
	gtk_html_load_empty(GTK_HTML(search1.report_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  search1.report_html);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox_preview), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

	search1.preview_html = gtk_html_new();
	gtk_widget_show(search1.preview_html);
	gtk_html_load_empty(GTK_HTML(search1.preview_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  search1.preview_html);
			  
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox_results), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_IN);

	
	search1.results_html = gtk_html_new();
	gtk_widget_show(search1.results_html);
	gtk_html_load_empty(GTK_HTML(search1.results_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  search1.results_html);

	g_signal_connect(G_OBJECT(search1.results_html), "link_clicked",
			 G_CALLBACK(_link_clicked), GINT_TO_POINTER(0));
	g_signal_connect(G_OBJECT(search1.results_html), "on_url",
			 G_CALLBACK(_url), GINT_TO_POINTER(0));
	g_signal_connect(G_OBJECT(search1.results_html),
			 "button_release_event",
			 G_CALLBACK(_on_button_release_event),
			 GINT_TO_POINTER(1));
			 
	g_signal_connect(G_OBJECT(search1.preview_html), "link_clicked",
			 G_CALLBACK(_link_clicked), GINT_TO_POINTER(1));
	g_signal_connect(G_OBJECT(search1.preview_html), "on_url",
			 G_CALLBACK(_url), GINT_TO_POINTER(1));
	g_signal_connect(G_OBJECT(search1.preview_html),
			 "button_release_event",
			 G_CALLBACK(_on_button_release_event),
			 GINT_TO_POINTER(1));
			 
	g_signal_connect(G_OBJECT(search1.report_html),
			 "link_clicked",
			 G_CALLBACK(report_link_clicked), NULL);
//#endif			 


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

void _on_dialog_response(GtkDialog * dialog, gint response_id,
			 gpointer user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CLOSE:
		on_destroy(GTK_WIDGET(dialog), user_data);
		break;
	}
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

static
void _create_search_dialog(void)
{
	gchar *glade_file;
	GladeXML *gxml;
	GladeXML *gxml2;
	//GtkWidget *mod_sel_dialog;

	_preview_on = TRUE;

	glade_file =
	    gui_general_user_file("search-dialog.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
	g_message(glade_file);

	/* build the widget */
	gxml = glade_xml_new(glade_file, "dialog", NULL);
	
	gxml2 = glade_xml_new(glade_file, "dialog2", NULL);
	search1.mod_sel_dialog = glade_xml_get_widget(gxml2, "dialog2");
	search1.mod_sel_dlg_treeview = glade_xml_get_widget(gxml2, "treeview8");
	_setup_treetview2(search1.mod_sel_dlg_treeview);
	gtk_widget_hide(search1.mod_sel_dialog);
	
	g_free(glade_file);
	g_return_if_fail(gxml != NULL);

	/* lookup the root widget */
	search1.dialog = glade_xml_get_widget(gxml, "dialog");
	search1.label_search_module =
	    glade_xml_get_widget(gxml, "label5");
	search1.search_entry = glade_xml_get_widget(gxml, "entry1");
	search1.notebook = glade_xml_get_widget(gxml, "notebook1");

	search1.treeview = glade_xml_get_widget(gxml, "treeview1");
	_setup_treetview(search1.treeview);

	search1.list_range_name =
	    glade_xml_get_widget(gxml, "treeview4");
	_setup_listviews(search1.list_range_name,
			 (GCallback) selection_range_lists_changed);

	search1.list_ranges = glade_xml_get_widget(gxml, "treeview5");
	_setup_listviews(search1.list_ranges, NULL);

	search1.module_lists = glade_xml_get_widget(gxml, "treeview6");
	_setup_listviews(search1.module_lists,
			 (GCallback) selection_modules_lists_changed);

	search1.listview_modules =
	    glade_xml_get_widget(gxml, "treeview7");
	_setup_listviews(search1.listview_modules, NULL);

	search1.rb_no_scope =
	    glade_xml_get_widget(gxml, "radiobutton1");
	search1.rb_last = glade_xml_get_widget(gxml, "radiobutton2");
	search1.which_scope = GTK_TOGGLE_BUTTON(search1.rb_no_scope);
	search1.rb_custom_range =
	    glade_xml_get_widget(gxml, "radiobutton3");
	/* modules radio buttons */
	search1.rb_current_module =
	    glade_xml_get_widget(gxml, "radiobutton4");
	search1.rb_mod_list =
	    glade_xml_get_widget(gxml, "radiobutton5");
	search1.rb_custom_list =
	    glade_xml_get_widget(gxml, "radiobutton6");
	/*  */
	search1.rb_words = glade_xml_get_widget(gxml, "radiobutton9");
	search1.rb_regexp = glade_xml_get_widget(gxml, "radiobutton10");
	search1.rb_exact_phrase =
	    glade_xml_get_widget(gxml, "radiobutton11");
	search1.rb_attributes =
	    glade_xml_get_widget(gxml, "radiobutton12");
	
	search1.cb_case_sensitive =
	    glade_xml_get_widget(gxml, "checkbutton1");
	search1.cb_include_strongs =
	    glade_xml_get_widget(gxml, "checkbutton2");
	search1.cb_include_morphs =
	    glade_xml_get_widget(gxml, "checkbutton3");
	search1.cb_include_footnotes =
	    glade_xml_get_widget(gxml, "checkbutton4");

	search1.togglebutton_show_main =
	    glade_xml_get_widget(gxml, "toggletoolbutton1");

	search1.combo_list =
	    glade_xml_get_widget(gxml, "comboboxentry2");
	_setup_combobox(GTK_COMBO_BOX(search1.combo_list));

	search1.entry_list_name = glade_xml_get_widget(gxml, "entry4");

	search1.combo_range =
	    glade_xml_get_widget(gxml, "comboboxentry1");
	_setup_combobox(GTK_COMBO_BOX(search1.combo_range));

	search1.entry_range_name = glade_xml_get_widget(gxml, "entry2");
	search1.entry_range_text = glade_xml_get_widget(gxml, "entry3");

	search1.progressbar =
	    glade_xml_get_widget(gxml, "progressbar1");
	search1.label_mod_select = glade_xml_get_widget(gxml, "label5");

	_add_html_widgets(glade_xml_get_widget(gxml, "vbox11"),
			  glade_xml_get_widget(gxml, "vbox12"),
			  glade_xml_get_widget(gxml, "vbox13"));

	glade_xml_signal_autoconnect_full
	    (gxml, (GladeXMLConnectFunc) gui_glade_signal_connect_func,
	     NULL);
	glade_xml_signal_autoconnect_full
	    (gxml2, (GladeXMLConnectFunc) gui_glade_signal_connect_func,
	     NULL);
}


/******************************************************************************
 * Name
 *   gui_create_search_dialog
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void gui_create_search_dialog(void)
 *
 * Description
 *   calls _create_search_dialog() to create the search dialog
 *
 * Return value
 *   void
 */

void gui_create_search_dialog(void)
{
	_create_search_dialog();
}
