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
#include "gui/widgets.h"

#include "gecko/gecko-html.h"

#include "main/search_dialog.h"
#include "main/configs.h"
#include "main/lists.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"

/******************************************************************************/
#define _BYTE   8
#define _WORD   16
#define _DWORD  32

#define SEARCHING N_("Searching the ")
#define SMODULE N_(" Module")
#define FINDS N_("found in ")
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"


/******************************************************************************/
/* Define a list of data types called "targets" that a destination widget will
 * accept. The string type is arbitrary, and negotiated between DnD widgets by 
 * the developer. An enum or GQuark can serve as the integer target id. */
enum {
        TARGET_INT32,
        TARGET_STRING,
        TARGET_ROOTWIN
};

/* datatype (string), restrictions on DnD (GtkTargetFlags), datatype (int) */
static GtkTargetEntry target_list[] = {
        { "INTEGER",    0, TARGET_INT32 },
        { "STRING",     0, TARGET_STRING },     
        { "text/plain", 0, TARGET_STRING },
        { "application/x-rootwindow-drop", 0, TARGET_ROOTWIN }
};

static guint n_targets = G_N_ELEMENTS (target_list);




SEARCH_DIALOG1 search1;

static GtkWidget *dialog;
static gboolean _preview_on;
static gboolean _in_url;
static gchar *module_selected;
gchar *verse_selected;

extern int drag_module_type;

void on_comboboxentry2_changed(GtkComboBox * combobox,
			       gpointer user_data)
{
	main_comboboxentry2_changed(combobox, user_data);
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
/*
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
*/

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

void _on_destroy(GtkWidget * dialog, gpointer user_data)
{
	main_close_search_dialog();
	if(module_selected) g_free(module_selected);
	if(verse_selected) g_free(verse_selected);
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
//	gtk_html_jump_to_anchor(GTK_HTML(search1.results_html), url);
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
	if(GTK_TOGGLE_BUTTON(search1.rb_custom_range)->active)
		gtk_widget_set_sensitive(search1.combo_range,TRUE);
	else
		gtk_widget_set_sensitive(search1.combo_range,FALSE);
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
		main_comboboxentry2_changed((GtkComboBox *) search1.
					    combo_list, user_data);
	}
	if(GTK_TOGGLE_BUTTON(search1.rb_custom_list)->active)
		gtk_widget_set_sensitive(search1.combo_list,TRUE);
	else
		gtk_widget_set_sensitive(search1.combo_list,FALSE);
		
		
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
		gtk_widget_set_sensitive(search1.combo_list,FALSE);
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

static void _selection_finds_list_changed(GtkTreeSelection *
					    selection, gpointer data)
{
	main_selection_finds_list_changed(selection, data);
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

static void _selection_modules_lists_changed(GtkTreeSelection *
					    selection, gpointer data)
{
	main_selection_modules_lists_changed(selection, data);
}




/******************************************************************************
 * Name
 *   _modules_lists_changed
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void _modules_lists_changed(GtkTreeSelection * selection,
 *		     					 gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void _modules_lists_changed(GtkTreeSelection *
				    selection, GtkTreeView * tree_widget)
{
	gchar *mod = NULL;
	GList *mods = NULL;
	GtkTreeIter selected;
	
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	if (gtk_tree_model_iter_has_child(model, &selected)) {
		
		if(module_selected) g_free(module_selected);
		module_selected = NULL;	
		return;
	}
	gtk_tree_model_get(model, &selected, 0, &mod, -1);
	if (mod) {
		
		if(module_selected) g_free(module_selected);
		module_selected = g_strdup(mod);
		g_free(mod);
	}	
	
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

static void _finds_verselist_selection_changed(GtkTreeSelection *
					    selection, gpointer data)
{
	main_finds_verselist_selection_changed(selection, data);
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

	
	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Module",
							  renderer,
							  "text", 0,
							  NULL);
	gtk_tree_view_append_column(treeview, column);
	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Module",
							  renderer,
							  "text", 1,
							  NULL);
	gtk_tree_view_append_column(treeview, column);
	gtk_tree_view_column_set_sort_column_id(column, 0);

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


static void setup_tag (GtkTextTag *tag, gpointer user_data)
{
/*	g_signal_connect (G_OBJECT (tag),
		    "event",
		    G_CALLBACK (tag_event_handler),
		    user_data);*/
}

static void create_text_tags(GtkTextBuffer * buffer)
{
	GtkTextTag *tag;
	GdkColor color;
	GdkColor color2;
	GdkColor color_red;
	GdkColor colorLink;
	PangoFontDescription *font_desc;



	/* verse number tag verse style*/
	tag = gtk_text_buffer_create_tag (buffer, "verseNumber", NULL);
	setup_tag (tag, buffer);
	color.red = color.green = 0;
	color.blue = 0xffff;
		"scale", PANGO_SCALE_XX_SMALL,
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);

	/* verse number tag verse style*/
	/*tag = gtk_text_buffer_create_tag (buffer, "verse", NULL);
	setup_tag (tag, buffer);
	g_object_set (G_OBJECT (tag),
                NULL);	*/

	/* current verse color tag */
	tag = gtk_text_buffer_create_tag (buffer, "fg_currentverse", NULL);
	color.blue = 0;
	color.green = 0xbbbb;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);

	/*  verse color tag */
	tag = gtk_text_buffer_create_tag (buffer, "fg_verse", NULL);
	color.blue = 0;
	color.green = 0;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);

	/* right to left tag */
	tag = gtk_text_buffer_create_tag (buffer, "rtl_text", NULL);
        g_object_set (G_OBJECT (tag),
		//"font", rtl_font,
                "wrap_mode", GTK_WRAP_WORD,
                "direction", GTK_TEXT_DIR_RTL,
                "indent", 0,
                "left_margin", 0,
                "right_margin", 0,
                NULL);

	/* large tag */
	tag = gtk_text_buffer_create_tag (buffer, "large", NULL);
        g_object_set (G_OBJECT (tag),
		"scale", (double)1.928, //PANGO_SCALE_XX_LARGE,
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
void _setup_textview(GtkWidget *textview , GCallback callback, GtkTextBuffer * buf)
{
	
	gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
	create_text_tags(buf);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW (textview),
						GTK_WRAP_WORD);
	
	
	if (!callback)
		return;

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
/*
static
void _setup_listview_results(GtkWidget * listview, GCallback callback)
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
*/

void
on_treeview10_drag_begin               (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gpointer         user_data)
{
#ifdef DEBUG
	g_message("on_treeview10_drag_begin");
#endif
	
}


gboolean
on_treeview10_drag_motion              (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        gint             x,
                                        gint             y,
                                        guint            time,
                                        gpointer         user_data)
{
#ifdef DEBUG
	g_message("on_treeview10_drag_motion");
#endif

  return FALSE;
}


void
on_treeview10_drag_leave               (GtkWidget       *widget,
                                        GdkDragContext  *drag_context,
                                        guint            time,
                                        gpointer         user_data)
{
#ifdef DEBUG
	g_message("on_treeview10_drag_leave");
#endif

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
	   G_CALLBACK(_modules_lists_changed), treeview);

}


void on_closebutton2_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(search1.mod_sel_dialog);
}


void
on_toolbutton12_clicked(GtkToolButton * toolbutton, gpointer user_data)
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
scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox_report), scrolledwindow, TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW
					    (scrolledwindow),
					    GTK_SHADOW_IN);

	search1.report_html = gtk_html_new();
	gtk_widget_show(search1.report_html);
	gtk_html_load_empty(GTK_HTML(search1.report_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  search1.report_html);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox_preview), scrolledwindow, TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW
					    (scrolledwindow),
					    GTK_SHADOW_IN);

	search1.preview_html = gtk_html_new();
	gtk_widget_show(search1.preview_html);
	gtk_html_load_empty(GTK_HTML(search1.preview_html));
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  search1.preview_html);

	scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox_results), scrolledwindow, TRUE,
			   TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW
					    (scrolledwindow),
					    GTK_SHADOW_IN);


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
*/

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
void _add_html_widget(GtkWidget * vbox)
{	
	search1.preview_html = GTK_WIDGET(gecko_html_new(NULL, FALSE, DIALOG_SEARCH_PREVIEW_TYPE));
	gtk_widget_show(search1.preview_html);
	gtk_box_pack_start(GTK_BOX(vbox), search1.preview_html, TRUE,
			   TRUE, 0);
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
		gtk_drag_dest_unset(widgets.html_text);
		gtk_drag_dest_unset(widgets.html_comm);
		gtk_drag_dest_unset(widgets.html_dict);
		gtk_drag_dest_unset(widgets.html_book);
		gtk_widget_destroy(GTK_WIDGET(search1.mod_sel_dialog));
		gtk_widget_destroy(GTK_WIDGET(dialog));
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

void _on_dialog2_response(GtkDialog * dialog, gint response_id,
			 gpointer user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CLOSE:
		gtk_widget_hide(GTK_WIDGET(dialog));
		break;
	case GTK_RESPONSE_APPLY:
		main_add_mod_to_list(search1.listview_modules, module_selected);
		break;
	}
}

/******************************************************************************/
/* Signal receivable by destination */

/* Emitted when the data has been received from the source. It should check 
 * the GtkSelectionData sent by the source, and do something with it. Finally
 * it needs to finish the operation by calling gtk_drag_finish, which will emit
 * the "data-delete" signal if told to. */
static void drag_data_received_handl(GtkWidget *widget, GdkDragContext *context, 
                         gint x, gint y, GtkSelectionData *selection_data, 
			 guint target_type, guint time, gpointer data)
{       
        glong   *_idata;
        gchar   *_sdata;
	gchar 	*_desc;
        
        gboolean dnd_success = FALSE;
        gboolean delete_selection_data = FALSE;
        
        const gchar *name = gtk_widget_get_name (widget);
#ifdef DEBUG
        g_print ("%s: drag_data_received_handl\n", name);
#endif
        
        
        /* Deal with what we are given from source */
        if((selection_data != NULL) && (selection_data-> length >= 0)) {
                if (context-> action == GDK_ACTION_ASK) {
                /* Ask the user to move or copy, then set the context action. */
                }
        
                if (context-> action == GDK_ACTION_MOVE)
                        delete_selection_data = TRUE;
                        
                /* Check that we got the format we can use */
#ifdef DEBUG
                g_print (" Receiving ");
#endif
                switch (target_type) {
                        case TARGET_INT32:
                                _idata = (glong*)selection_data-> data;
#ifdef DEBUG
                                g_print ("integer: %ld", *_idata);
#endif
                                dnd_success = TRUE;
                                break;
                                        
                        case TARGET_STRING:
                                _sdata = (gchar*)selection_data-> data;
				if(_sdata)
					main_add_mod_to_list(widget, _sdata);
#ifdef DEBUG
                                g_print ("string: %s", _sdata);
#endif
                                dnd_success = TRUE;
                                break;
                                        
                        default:
                                g_print ("nothing good");
                }                      
#ifdef DEBUG  
                g_print (".\n");
#endif
        }

        if (dnd_success == FALSE) {
#ifdef DEBUG
                g_print ("DnD data transfer failed!\n");
#endif
        }
        
        gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}

/* Emitted when a drag is over the destination */
static gboolean drag_motion_handl(GtkWidget *widget, GdkDragContext *context, 
				gint x, gint y, guint t, gpointer user_data)
{
        // Fancy stuff here. This signal spams the console something horrible.
        //const gchar *name = gtk_widget_get_name (widget);
        //g_print ("%s: drag_motion_handl\n", name);
        return  FALSE;
}

/* Emitted when a drag leaves the destination */
static void drag_leave_handl(GtkWidget *widget, GdkDragContext *context, 
				guint time, gpointer user_data)
{
        const gchar *name = gtk_widget_get_name (widget);
        g_print ("%s: drag_leave_handl\n", name);
}

/* Emitted when the user releases (drops) the selection. It should check that
 * the drop is over a valid part of the widget (if its a complex widget), and
 * itself to return true if the operation should continue. Next choose the 
 * target type it wishes to ask the source for. Finally call gtk_drag_get_data
 * which will emit "drag-data-get" on the source. */
static gboolean drag_drop_handl(GtkWidget *widget, GdkDragContext *context, 
				gint x, gint y, guint time, gpointer user_data)
{
        gboolean        is_valid_drop_site;
        GdkAtom         target_type;
        
        const gchar *name = gtk_widget_get_name (widget);
#ifdef DEBUG
        g_print ("%s: drag_drop_handl\n", name);
#endif
        
        /* Check to see if (x,y) is a valid drop site within widget */
        is_valid_drop_site = TRUE;
        
        /* If the source offers a target */
        if (context-> targets) {
                /* Choose the best target type */
                target_type = GDK_POINTER_TO_ATOM 
                        (g_list_nth_data (context-> targets, TARGET_STRING));
                
                /* Request the data from the source. */
                gtk_drag_get_data(widget,         /* will receive 'drag-data-received' signal */
                        	context,        /* represents the current state of the DnD */
                        	target_type,    /* the target type we want */
                        	time );            /* time stamp */
        }
        /* No target offered by source => error */
        else {
                is_valid_drop_site = FALSE;
        }
        
        return  is_valid_drop_site;
}


/******************************************************************************/
/* Signals receivable by source */

/* Emitted after "drag-data-received" is handled, and gtk_drag_finish is called
 * with the "delete" parameter set to TRUE (when DnD is GDK_ACTION_MOVE). */
static void drag_data_delete_handl(GtkWidget *widget, GdkDragContext *context, 
					gpointer user_data)
{
        // We aren't moving or deleting anything here
        const gchar *name = gtk_widget_get_name (widget);
        g_print ("%s: drag_data_delete_handl\n", name);
}

/* Emitted when the destination requests data from the source via 
 * gtk_drag_get_data. It should attempt to provide its data in the form 
 * requested in the target_type passed to it from the destination. If it cannot,
 * it should default to a "safe" type such as a string or text, even if only to
 * print an error. Then use gtk_selection_data_set to put the source data into
 * the allocated selection_data object, which will then be passed to the 
 * destination. This will cause "drag-data-received" to be emitted on the 
 * destination. GdkSelectionData is based on X's selection mechanism which, 
 * via X properties, is only capable of storing data in blocks of 8, 16, or 
 * 32 bit units. */
static void drag_data_get_handl(GtkWidget *widget, GdkDragContext *context, 
			GtkSelectionData *selection_data, guint target_type, 
			guint time, gpointer user_data)
{
        const gchar *name = gtk_widget_get_name (widget);
        const gchar *string_data = "This is data from the source.";
        const glong integer_data = 42;

#ifdef DEBUG               
        g_print ("%s: drag_data_get_handl\n", name);
#endif
        g_assert (selection_data != NULL);
#ifdef DEBUG       
        g_print (" Sending ");
#endif
        switch (target_type) {
                /* case TARGET_SOME_OBJECT:
                 * Serialize the object and send as a string of bytes. 
                 * Pixbufs, (UTF-8) text, and URIs have their own convenience 
                 * setter functions */

        case TARGET_INT32:
#ifdef DEBUG       
                g_print ("integer: %ld", integer_data);
#endif
                gtk_selection_data_set(selection_data,         /* Allocated GdkSelectionData object */
                        	selection_data-> target,/* target type */
                        	_DWORD,                 /* number of bits per 'unit' */
                        	(guchar*) &integer_data,/* pointer to data to be sent */
                        	sizeof (integer_data));   /* length of data in units */
                break;
                                
        case TARGET_STRING:
		if(!module_selected) 
			break;
#ifdef DEBUG       
                g_print ("module_selected: %s", module_selected);
#endif
                gtk_selection_data_set( selection_data,         
                        	selection_data-> target,
                        	_BYTE,                  
                        	(guchar*) module_selected,
                        	(module_selected)?strlen(module_selected):0);
                break;
                
        case TARGET_ROOTWIN:
#ifdef DEBUG       
                g_print ("Dropped on the root window!\n");
#endif
                break;
                
        default:
                /* Default to some safe target instead of fail. */
                g_assert_not_reached ();
        }
        
#ifdef DEBUG       
        g_print (".\n");
#endif
}

/* Emitted when DnD begins. This is often used to present custom graphics. */
static void drag_begin_handl(GtkWidget *widget, GdkDragContext *context, 
					gpointer user_data)
{
        const gchar *name = gtk_widget_get_name (widget);
#ifdef DEBUG
        g_print ("%s: drag_begin_handl\n", name);
#endif
}

/* Emitted when DnD ends. This is used to clean up any leftover data. */
static void drag_end_handl(GtkWidget *widget, GdkDragContext *context, 
				gpointer user_data)
{
        const gchar *name = gtk_widget_get_name (widget);
#ifdef DEBUG
        g_print ("%s: drag_end_handl\n", name);
#endif
}

/* Emitted when DnD begins. This is often used to present custom graphics. */
static void verses_drag_begin_handl(GtkWidget *widget, GdkDragContext *context, 
					gpointer user_data)
{
        const gchar *name = gtk_widget_get_name (widget);
#ifdef DEBUG
        g_print ("%s: drag_begin_handl\n", name);
#endif
}

/******************************************************************************/
/* Signal receivable by destination */

/* Emitted when the data has been received from the source. It should check 
 * the GtkSelectionData sent by the source, and do something with it. Finally
 * it needs to finish the operation by calling gtk_drag_finish, which will emit
 * the "data-delete" signal if told to. */
static void verses_drag_data_received_handl(GtkWidget *widget, GdkDragContext *context, 
                         gint x, gint y, GtkSelectionData *selection_data, 
			 guint target_type, guint time, gpointer data)
{       
        glong   *_idata;
        gchar   *_sdata;
	gchar 	*_desc;
        
        gboolean dnd_success = FALSE;
        gboolean delete_selection_data = FALSE;
        
        const gchar *name = gtk_widget_get_name (widget);
#ifdef DEBUG
        g_print ("%s: verses_drag_data_received_handl\n", name);
#endif
        
        
        /* Deal with what we are given from source */
        if((selection_data != NULL) && (selection_data-> length >= 0)) {
                if (context-> action == GDK_ACTION_ASK) {
                /* Ask the user to move or copy, then set the context action. */
                }
        
                if (context-> action == GDK_ACTION_MOVE)
                        delete_selection_data = TRUE;
                        
                /* Check that we got the format we can use */
#ifdef DEBUG
                g_print ("verses_ Receiving ");
#endif
                switch (target_type) {
                        case TARGET_INT32:
                                _idata = (glong*)selection_data-> data;
#ifdef DEBUG
                                g_print ("integer: %ld", *_idata);
#endif
                                dnd_success = TRUE;
                                break;
                                        
                        case TARGET_STRING:
                                _sdata = (gchar*)selection_data-> data;
				if(_sdata) {
					if(drag_module_type == COMMENTARY_TYPE)						
						gtk_notebook_set_current_page(
						GTK_NOTEBOOK(
						widgets.notebook_comm_book),
						0);
					if(drag_module_type == BOOK_TYPE)						
						gtk_notebook_set_current_page(
						GTK_NOTEBOOK(
						widgets.notebook_comm_book),
						1);						
					main_url_handler(verse_selected,TRUE);
#ifdef DEBUG
                                	g_print ("string: %s", _sdata);
#endif
				}
                                dnd_success = TRUE;
                                break;
                                        
                        default:
                                g_print ("nothing good");
                }
                        
#ifdef DEBUG
                g_print (".\n");
#endif
        }

        if (dnd_success == FALSE) {
#ifdef DEBUG
                g_print ("verses_DnD data transfer failed!\n");
#endif
        }
        
        gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}


/* Emitted when the user releases (drops) the selection. It should check that
 * the drop is over a valid part of the widget (if its a complex widget), and
 * itself to return true if the operation should continue. Next choose the 
 * target type it wishes to ask the source for. Finally call gtk_drag_get_data
 * which will emit "drag-data-get" on the source. */
static gboolean verses_drag_drop_handl(GtkWidget *widget, GdkDragContext *context, 
				gint x, gint y, guint time, gpointer user_data)
{
        gboolean        is_valid_drop_site;
        GdkAtom         target_type;
        
        const gchar *name = gtk_widget_get_name (widget);
#ifdef DEBUG
        g_print ("%s: verses_drag_drop_handl\n", name);
#endif
        
        /* Check to see if (x,y) is a valid drop site within widget */
        is_valid_drop_site = TRUE;
        
        /* If the source offers a target */
        if (context-> targets) {
                /* Choose the best target type */
                target_type = GDK_POINTER_TO_ATOM 
                        (g_list_nth_data (context-> targets, TARGET_STRING));
                
                /* Request the data from the source. */
                gtk_drag_get_data(widget,         /* will receive 'drag-data-received' signal */
                        	context,        /* represents the current state of the DnD */
                        	target_type,    /* the target type we want */
                        	time );            /* time stamp */
        }
        /* No target offered by source => error */
        else {
                is_valid_drop_site = FALSE;
        }
        
        return  is_valid_drop_site;
}


/* Emitted when the destination requests data from the source via 
 * gtk_drag_get_data. It should attempt to provide its data in the form 
 * requested in the target_type passed to it from the destination. If it cannot,
 * it should default to a "safe" type such as a string or text, even if only to
 * print an error. Then use gtk_selection_data_set to put the source data into
 * the allocated selection_data object, which will then be passed to the 
 * destination. This will cause "drag-data-received" to be emitted on the 
 * destination. GdkSelectionData is based on X's selection mechanism which, 
 * via X properties, is only capable of storing data in blocks of 8, 16, or 
 * 32 bit units. */
static 
void verses_drag_data_get_handl(GtkWidget *widget, GdkDragContext *context, 
			GtkSelectionData *selection_data, guint target_type, 
			guint time, gpointer user_data)
{
        const gchar *name = gtk_widget_get_name (widget);
        const gchar *string_data = "This is data from the source.";
        const gchar *string_data2 = NULL;
        const glong integer_data = 42;

#ifdef DEBUG               
        g_print ("%s: verses_drag_data_get_handl\n", name);
#endif
        g_assert (selection_data != NULL);
#ifdef DEBUG       
        g_print (" Sending ");
#endif
        switch (target_type) {
                /* case TARGET_SOME_OBJECT:
                 * Serialize the object and send as a string of bytes. 
                 * Pixbufs, (UTF-8) text, and URIs have their own convenience 
                 * setter functions */

        case TARGET_INT32:
#ifdef DEBUG       
                g_print ("verses_integer: %ld", integer_data);
#endif
                gtk_selection_data_set(selection_data,         /* Allocated GdkSelectionData object */
                        	selection_data-> target,/* target type */
                        	_DWORD,                 /* number of bits per 'unit' */
                        	(guchar*) &integer_data,/* pointer to data to be sent */
                        	sizeof (integer_data));   /* length of data in units */
                break;
                                
        case TARGET_STRING:
		if(!verse_selected) 
			break;
#ifdef DEBUG       
                g_print ("verse_selected: %s", verse_selected);
#endif
                gtk_selection_data_set( selection_data,         
                        	selection_data-> target,
                        	_BYTE,                  
                        	(guchar*) verse_selected,
                        	(verse_selected)?strlen(verse_selected):0);
                break;
                
        case TARGET_ROOTWIN:
#ifdef DEBUG       
                g_print ("verses_Dropped on the root window!\n");
#endif
                break;
                
        default:
                /* Default to some safe target instead of fail. */
                g_assert_not_reached ();
        }
        
#ifdef DEBUG       
        g_print (".\n");
#endif
}

/******************************************************************************
 * Name
 *   _setup_dnd
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void _setup_dnd(void)
 *
 * Description
 *   setup source and destination widgets for drag and drop
 *
 * Return value
 *   void
 */

void _setup_dnd(void)
{
	GdkModifierType start_button_mask;
	GdkDragAction actions;
	GtkTargetEntry target_entry;	        
        
        /* Make the "well label" a DnD destination. */
        gtk_drag_dest_set( search1.listview_modules,              /* widget that will accept a drop */
                	GTK_DEST_DEFAULT_MOTION /* default actions for dest on DnD */
                	| GTK_DEST_DEFAULT_HIGHLIGHT,
                	target_list,            /* lists of target to support */
                	n_targets,              /* size of list */
                	GDK_ACTION_COPY);         /* what to do with data after dropped */
                
        gtk_drag_source_set( search1.mod_sel_dlg_treeview,  /* widget will be drag-able */
	        		GDK_BUTTON1_MASK,       /* modifier that will start a drag */
                		target_list,            /* lists of target to support */
                		n_targets,              /* size of list */
                		GDK_ACTION_COPY);        /* what to do with data after dropped */
	
        gtk_drag_dest_set(widgets.html_text,              /* widget that will accept a drop */
                	GTK_DEST_DEFAULT_MOTION /* default actions for dest on DnD */
                	| GTK_DEST_DEFAULT_HIGHLIGHT,
                	target_list,            /* lists of target to support */
                	n_targets,              /* size of list */
                	GDK_ACTION_COPY);         /* what to do with data after dropped */
	
        gtk_drag_dest_set(widgets.html_comm,              /* widget that will accept a drop */
                	GTK_DEST_DEFAULT_MOTION /* default actions for dest on DnD */
                	| GTK_DEST_DEFAULT_HIGHLIGHT,
                	target_list,            /* lists of target to support */
                	n_targets,              /* size of list */
                	GDK_ACTION_COPY);         /* what to do with data after dropped */
	
        gtk_drag_dest_set(widgets.html_dict,              /* widget that will accept a drop */
                	GTK_DEST_DEFAULT_MOTION /* default actions for dest on DnD */
                	| GTK_DEST_DEFAULT_HIGHLIGHT,
                	target_list,            /* lists of target to support */
                	n_targets,              /* size of list */
                	GDK_ACTION_COPY);         /* what to do with data after dropped */
	
        gtk_drag_dest_set(widgets.html_book,              /* widget that will accept a drop */
                	GTK_DEST_DEFAULT_MOTION /* default actions for dest on DnD */
                	| GTK_DEST_DEFAULT_HIGHLIGHT,
                	target_list,            /* lists of target to support */
                	n_targets,              /* size of list */
                	GDK_ACTION_COPY);         /* what to do with data after dropped */

	gtk_drag_source_set(search1.listview_verses,  /* widget will be drag-able */
	        		GDK_BUTTON1_MASK,       /* modifier that will start a drag */
                		target_list,            /* lists of target to support */
                		n_targets,              /* size of list */
                		GDK_ACTION_COPY);        /* what to do with data after dropped */
        
	       
        /* All possible destination signals */
        g_signal_connect (search1.listview_modules, "drag-data-received", 
                G_CALLBACK(drag_data_received_handl), NULL);
                        
        g_signal_connect (search1.listview_modules, "drag-leave",
                G_CALLBACK (drag_leave_handl), NULL);

        g_signal_connect (search1.listview_modules, "drag-motion",
                G_CALLBACK (drag_motion_handl), NULL);

        g_signal_connect (search1.listview_modules, "drag-drop",
                G_CALLBACK (drag_drop_handl), NULL);
        
        /* All possible source signals */
        g_signal_connect (search1.mod_sel_dlg_treeview, "drag-data-get",
                G_CALLBACK (drag_data_get_handl), NULL);
               
        g_signal_connect (search1.mod_sel_dlg_treeview, "drag-data-delete",
                G_CALLBACK (drag_data_delete_handl), NULL);

        g_signal_connect (search1.mod_sel_dlg_treeview, "drag-begin",
                G_CALLBACK (drag_begin_handl), NULL);
                
        g_signal_connect (search1.mod_sel_dlg_treeview, "drag-end",
                G_CALLBACK (drag_end_handl), NULL); 
      
      /* All possible destination signals verses*/
        g_signal_connect (widgets.html_text, "drag-data-received", 
                G_CALLBACK(verses_drag_data_received_handl), NULL);
        g_signal_connect (widgets.html_text, "drag-drop",
                G_CALLBACK (verses_drag_drop_handl), NULL); 
      
      /* All possible destination signals verses*/
        g_signal_connect (widgets.html_comm, "drag-data-received", 
                G_CALLBACK(verses_drag_data_received_handl), NULL);
        g_signal_connect (widgets.html_comm, "drag-drop",
                G_CALLBACK (verses_drag_drop_handl), NULL); 
      
      /* All possible destination signals verses*/
        g_signal_connect (widgets.html_dict, "drag-data-received", 
                G_CALLBACK(verses_drag_data_received_handl), NULL);
        g_signal_connect (widgets.html_dict, "drag-drop",
                G_CALLBACK (verses_drag_drop_handl), NULL); 
      
      /* All possible destination signals verses*/
        g_signal_connect (widgets.html_book, "drag-data-received", 
                G_CALLBACK(verses_drag_data_received_handl), NULL);
        g_signal_connect (widgets.html_book, "drag-drop",
                G_CALLBACK (verses_drag_drop_handl), NULL); 
		
        /* All possible source signals verses*/
        g_signal_connect (search1.listview_verses, "drag-data-get",
                G_CALLBACK (verses_drag_data_get_handl), NULL);
        g_signal_connect (search1.listview_verses, "drag-begin",
                G_CALLBACK (verses_drag_begin_handl), NULL);
                
}


/******************************************************************************
 * Name
 *   _create_search_dialog
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void _create_search_dialog(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static
void _create_search_dialog(void)
{
	gchar *glade_file;
	GladeXML *gxml;
	GladeXML *gxml2;
	GtkWidget *button1;
	GtkWidget *toolbutton1;
	GtkWidget *toolbutton2;
	GtkWidget *toolbutton3;
	GtkWidget *toolbutton4;
	GtkWidget *toolbutton5;
	GtkWidget *toolbutton6;
	GtkWidget *toolbutton7;
	GtkWidget *toolbutton8;
	GtkWidget *toolbutton10;
	GtkWidget *toolbutton11;
	GtkWidget *toolbutton12;
	
	module_selected = NULL;
	verse_selected = NULL;
	_preview_on = TRUE;

	glade_file =
	    gui_general_user_file("search-dialog.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
#ifdef DEBUG
	g_message(glade_file);
#endif

	/* build the widget */
	gxml = glade_xml_new(glade_file, "dialog", NULL);
	

	gxml2 = glade_xml_new(glade_file, "dialog2", NULL);
	search1.mod_sel_dialog = glade_xml_get_widget(gxml2, "dialog2");
	g_signal_connect((gpointer)search1.mod_sel_dialog, "response",
			 G_CALLBACK(_on_dialog2_response), NULL);
	search1.mod_sel_dlg_treeview =
	    glade_xml_get_widget(gxml2, "treeview8");
	_setup_treetview2(search1.mod_sel_dlg_treeview);
	gtk_widget_hide(search1.mod_sel_dialog);

	g_free(glade_file);
	g_return_if_fail(gxml != NULL);

/*	g_signal_connect(search1., "",
			 G_CALLBACK(), NULL);
*/

	/* lookup the root widget */
	search1.dialog = glade_xml_get_widget(gxml, "dialog");
	g_signal_connect(search1.dialog, "response",
			 G_CALLBACK(_on_dialog_response), NULL);
	g_signal_connect(search1.dialog, "destroy",
			 G_CALLBACK(_on_destroy), NULL);
			 
	button1 = glade_xml_get_widget(gxml, "button1");	
	g_signal_connect(button1, "clicked",
			 G_CALLBACK(on_button_begin_search), NULL);	
			 
	search1.label_search_module =
	    glade_xml_get_widget(gxml, "label5");
	search1.search_entry = glade_xml_get_widget(gxml, "entry1");
	g_signal_connect(search1.search_entry, "activate",
			 G_CALLBACK(on_button_begin_search), NULL);	
	
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
			 (GCallback) _selection_modules_lists_changed);

	search1.listview_modules =
	    glade_xml_get_widget(gxml, "treeview7");
	_setup_listviews(search1.listview_modules, NULL);
	
	
	/* scope radio buttons */
	search1.rb_no_scope = glade_xml_get_widget(gxml, "radiobutton1");
	g_signal_connect(search1.rb_no_scope, "toggled",
			 G_CALLBACK(scope_toggled), NULL);		    
	search1.rb_last = glade_xml_get_widget(gxml, "radiobutton2");
	search1.which_scope = GTK_TOGGLE_BUTTON(search1.rb_no_scope);
	search1.rb_custom_range = glade_xml_get_widget(gxml, "radiobutton3");
	g_signal_connect(search1.rb_custom_range, "toggled",
			 G_CALLBACK(scope_toggled), NULL);	    
	/* modules radio buttons */
	search1.rb_current_module = glade_xml_get_widget(gxml, "radiobutton4");
	g_signal_connect(search1.rb_current_module, "toggled",
			 G_CALLBACK(current_module_toggled), NULL);	
	search1.rb_mod_list = glade_xml_get_widget(gxml, "radiobutton5");
	g_signal_connect(search1.rb_mod_list, "toggled",
			 G_CALLBACK(mod_list_toggled), NULL);	
	search1.rb_custom_list = glade_xml_get_widget(gxml, "radiobutton6");
	g_signal_connect(search1.rb_custom_list, "toggled",
			 G_CALLBACK(mod_list_toggled), NULL);	
	/*  */
	search1.rb_words = glade_xml_get_widget(gxml, "radiobutton9");
	search1.rb_regexp = glade_xml_get_widget(gxml, "radiobutton10");
	search1.rb_exact_phrase =
	    glade_xml_get_widget(gxml, "radiobutton11");
	    
	    
	/* attributes radio buttons */
	search1.rb_attributes =
	    glade_xml_get_widget(gxml, "radiobutton12");
	search1.rb_strongs =
	    glade_xml_get_widget(gxml, "radiobutton13");
	search1.rb_morphs =
	    glade_xml_get_widget(gxml, "radiobutton15");
	search1.rb_footnotes =
	    glade_xml_get_widget(gxml, "radiobutton14");

	/*   */
	search1.cb_case_sensitive =
	    glade_xml_get_widget(gxml, "checkbutton1");
	    
	/* display options check buttons */
	search1.cb_include_strongs =
	    glade_xml_get_widget(gxml, "checkbutton2");
	search1.cb_include_morphs =
	    glade_xml_get_widget(gxml, "checkbutton3");
	search1.cb_include_footnotes =
	    glade_xml_get_widget(gxml, "checkbutton4");
	    
	toolbutton1 = glade_xml_get_widget(gxml, "toolbutton1");
	g_signal_connect(toolbutton1, "clicked",
			 G_CALLBACK(button_save), NULL);
	    
	toolbutton2 = glade_xml_get_widget(gxml, "toolbutton2");
	g_signal_connect(toolbutton2, "clicked",
			 G_CALLBACK(button_clean), NULL);
	    
	toolbutton3 = glade_xml_get_widget(gxml, "toolbutton3");
	g_signal_connect(toolbutton3, "clicked",
			 G_CALLBACK(new_range), NULL);
	    
	toolbutton4 = glade_xml_get_widget(gxml, "toolbutton4");
	g_signal_connect(toolbutton4, "clicked",
			 G_CALLBACK(save_range), NULL);
	    
	toolbutton5 = glade_xml_get_widget(gxml, "toolbutton5");
	g_signal_connect(toolbutton5, "clicked",
			 G_CALLBACK(delete_range), NULL);
	    
	toolbutton6 = glade_xml_get_widget(gxml, "toolbutton6");
	g_signal_connect(toolbutton6, "clicked",
			 G_CALLBACK(new_modlist), NULL);
	    
	toolbutton7 = glade_xml_get_widget(gxml, "toolbutton7");
	g_signal_connect(toolbutton7, "clicked",
			 G_CALLBACK(save_modlist), NULL);
	    
	toolbutton8 = glade_xml_get_widget(gxml, "toolbutton8");
	g_signal_connect(toolbutton8, "clicked",
			 G_CALLBACK(delete_list), NULL);
	    
	toolbutton10 = glade_xml_get_widget(gxml, "toolbutton10");
	g_signal_connect(toolbutton10, "clicked",
			 G_CALLBACK(clear_modules), NULL);
	    
	toolbutton11 = glade_xml_get_widget(gxml, "toolbutton11");
	g_signal_connect(toolbutton11, "clicked",
			 G_CALLBACK(delete_module), NULL);
	    
	toolbutton12 = glade_xml_get_widget(gxml, "toolbutton12");
	g_signal_connect(toolbutton12, "clicked",
			 G_CALLBACK(on_toolbutton12_clicked), NULL);
			 
	search1.togglebutton_show_main =
	    glade_xml_get_widget(gxml, "toggletoolbutton1");
	g_signal_connect(search1.togglebutton_show_main, "toggled",
			 G_CALLBACK(on_togglebutton_show_main), NULL);
	search1.combo_list =
	    glade_xml_get_widget(gxml, "comboboxentry2");
	_setup_combobox(GTK_COMBO_BOX(search1.combo_list));
	g_signal_connect(search1.combo_list, "changed",
			 G_CALLBACK(on_comboboxentry2_changed), NULL);
			 
	search1.entry_list_name = glade_xml_get_widget(gxml, "entry4");
	g_signal_connect(search1.entry_list_name, "changed",
			 G_CALLBACK(list_name_changed), NULL);

	search1.combo_range =
	    glade_xml_get_widget(gxml, "comboboxentry1");
	_setup_combobox(GTK_COMBO_BOX(search1.combo_range));

	search1.entry_range_name = glade_xml_get_widget(gxml, "entry2");
	g_signal_connect(search1.entry_range_name, "changed",
			 G_CALLBACK(range_name_changed), NULL);
	search1.entry_range_text = glade_xml_get_widget(gxml, "entry3");
	g_signal_connect(search1.entry_range_text, "changed",
			 G_CALLBACK(range_text_changed), NULL);	

	search1.progressbar =
	    glade_xml_get_widget(gxml, "progressbar1");
	search1.label_mod_select = glade_xml_get_widget(gxml, "label5");

	search1.listview_results = glade_xml_get_widget(gxml, "treeview9");
	_setup_listviews(search1.listview_results, (GCallback) _selection_finds_list_changed);
	search1.listview_verses = glade_xml_get_widget(gxml, "treeview10");  		
	_setup_listviews(search1.listview_verses, (GCallback) _finds_verselist_selection_changed); 
	_setup_dnd();
	_add_html_widget(glade_xml_get_widget(gxml, "vbox12"));
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
