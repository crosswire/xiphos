/*
 * Xiphos Bible Study Tool
 * search_dialog.c - gui for searching Sword modules
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#ifndef USE_GTKBUILDER
#include <glade/glade-xml.h>
#endif

#include "xiphos_html/xiphos_html.h"

#include <regex.h>
#include <string.h>
#include <glib.h>

#include "gui/search_dialog.h"
#include "gui/main_window.h"
#include "gui/dialog.h"
#include "gui/utilities.h"
#include "gui/widgets.h"
#include "gtk/gtk.h"
#include "gui/export_bookmarks.h"

#include "main/search_dialog.h"
#include "main/configs.h"
#include "main/lists.h"
#include "main/previewer.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"
#include "main/xml.h"
#include "main/biblesync_glue.h"

#include "gui/debug_glib_null.h"

/******************************************************************************/
#define _BYTE 8
#define _WORD 16
#define _DWORD 32

#define SEARCHING N_("Searching the ")
#define SMODULE N_(" Module")
#define FINDS N_("found in ")
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\"><!-- A { text-decoration:none } *[dir=rtl] { text-align: right; } --></style></head>"

SEARCH_DIALOG1 search1;

static gboolean _preview_on;
static gchar *module_selected;
gchar *verse_selected;

GtkWidget *remember_search; /* needed to change button in search stop */

/******************************************************************************
 * Name
 *   on_advsearch_configure_event
 *
 * Synopsis
 *   #include "gui/main_window.h"
 *
 *   gboolean on_advsearch_configure_event(GtkWidget * widget,
 *				   GdkEventConfigure * event,
 *				   gpointer user_data)
 *
 * Description
 *   remember placement+size of parallel window.
 *   cloned from on_configure_event
 *
 * Return value
 *   gboolean
 */

static gboolean on_advsearch_configure_event(GtkWidget *widget,
					     GdkEventConfigure *event,
					     gpointer user_data)
{
	gchar layout[10];
	gint x;
	gint y;

	gdk_window_get_root_origin(GDK_WINDOW(gtk_widget_get_window(search1.dialog)),
				   &x, &y);

	settings.advsearch_width = event->width;
	settings.advsearch_height = event->height;
	settings.advsearch_x = x;
	settings.advsearch_y = y;

	sprintf(layout, "%d", settings.advsearch_width);
	xml_set_value("Xiphos", "layout", "advsearch_width", layout);

	sprintf(layout, "%d", settings.advsearch_height);
	xml_set_value("Xiphos", "layout", "advsearch_height", layout);

	sprintf(layout, "%d", settings.advsearch_x);
	xml_set_value("Xiphos", "layout", "advsearch_x", layout);

	sprintf(layout, "%d", settings.advsearch_y);
	xml_set_value("Xiphos", "layout", "advsearch_y", layout);
	xml_save_settings_doc(settings.fnconfigure);

	return FALSE;
}

/* click on treeview folder to expand or collapse it */
static gboolean button_release_event(GtkWidget *widget,
				     GdkEventButton *event, gpointer data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeIter selected;
	GtkTreeModel *model;
	GtkTreePath *path;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

	if (!gtk_tree_selection_get_selected(selection, &model, &selected))
		return FALSE;

	if (!gtk_tree_model_iter_has_child(model, &selected))
		return FALSE;

	path = gtk_tree_model_get_path(model, &selected);
	if (gtk_tree_view_row_expanded(GTK_TREE_VIEW(widget), path))
		gtk_tree_view_collapse_row(GTK_TREE_VIEW(widget), path);
	else
		gtk_tree_view_expand_row(GTK_TREE_VIEW(widget), path,
					 FALSE);
	gtk_tree_path_free(path);
	return FALSE;
}

void on_comboboxentry2_changed(GtkComboBox *combobox, gpointer user_data)
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
 *   void button_clean(GtkButton * button, gpointer user_data)
 *
 * Description
 *   user pressed clear button - clears search results page
 *
 * Return value
 *   void
 */

void button_clean(GtkButton *button, gpointer user_data)
{
	GtkTreeModel *model;
	GtkListStore *list_store;
	GString *html_text = g_string_new("");

	XI_message(("button_clean"));
	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_results));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_verses));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);

	g_string_printf(html_text,
			HTML_START
			"<body text=\"%s\" bgcolor=\"%s\"> </body></html>",
			settings.bible_text_color,
			settings.bible_bg_color);
	XIPHOS_HTML_OPEN_STREAM(search1.preview_html, "text/html");
	XIPHOS_HTML_WRITE(search1.preview_html, html_text->str,
			  html_text->len);
	XIPHOS_HTML_CLOSE(search1.preview_html);
	g_string_free(html_text, TRUE);
}

/******************************************************************************
 * Name
 *   button_save
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void button_save(GtkButton * button, gpointer user_data)
 *
 * Description
 *   calls main_save_current_adv_search_as_bookmarks() in main/search_dialog.cc
 *   to do the work of saving current search as bookmarks
 *
 * Return value
 *   void
 */

void button_save(GtkButton *button, gpointer user_data)
{
	main_save_current_adv_search_as_bookmarks();
}

/******************************************************************************
 * Name
 *   button_export
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void button_export(GtkButton * button, gpointer user_data)
 *
 * Description
 *   calls main_save_current_adv_search_as_bookmarks() in main/search_dialog.cc
 *   to do the work of saving current search as bookmarks
 *
 * Return value
 *   void
 */

void button_export(GtkButton *button, gpointer user_data)
{
	gui_export_bookmarks_dialog(ADV_SEARCH_RESULTS_EXPORT, NULL);
}

/******************************************************************************
 * Name
 *   on_destroy
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_destroy(GtkWidget * dialog, gpointer user_data)
 *
 * Description
 *   destroy the search dialog
 *
 * Return value
 *   void
 */

void _on_destroy(GtkWidget *dialog, gpointer user_data)
{
	/* main_do_dialog_search() initializes these as search starts */
	if (search_active) {
		terminate_search = TRUE;
		sync_windows();
	} else {
		main_close_search_dialog();

		settings.display_advsearch = 0;
		xml_set_value("Xiphos", "layout", "advsearchopen", "0");

		if (module_selected) {
			g_free(module_selected);
			module_selected = NULL;
		}
		if (verse_selected) {
			g_free(verse_selected);
			verse_selected = NULL;
		}
	}
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

void on_button_begin_search(GtkButton *button, gpointer user_data)
{
	if (search_active) {
		terminate_search = TRUE;
#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_button_set_image((GtkButton *)remember_search,
				     gtk_image_new_from_icon_name("edit-find", GTK_ICON_SIZE_BUTTON));
#else
		gtk_button_set_label((GtkButton *)remember_search,
				     "gtk-find");
		gtk_button_set_use_stock((GtkButton *)remember_search,
					 TRUE);

#endif
		sync_windows();
	} else {
#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_button_set_image((GtkButton *)remember_search,
				     gtk_image_new_from_icon_name("process-stop",
								  GTK_ICON_SIZE_BUTTON));
#else
		gtk_button_set_label((GtkButton *)remember_search,
				     "gtk-stop");
		gtk_button_set_use_stock((GtkButton *)remember_search,
					 TRUE);
#endif

		// do the search
		main_do_dialog_search();

#if GTK_CHECK_VERSION(3, 10, 0)
		gtk_button_set_image((GtkButton *)remember_search,
				     gtk_image_new_from_icon_name("edit-find", GTK_ICON_SIZE_BUTTON));
#else
		gtk_button_set_label((GtkButton *)remember_search,
				     "gtk-find");
		gtk_button_set_use_stock((GtkButton *)remember_search,
					 TRUE);
#endif
	}
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

void list_name_changed(GtkEditable *editable, gpointer user_data)
{
	const gchar *text;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(search1.module_lists));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
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

void range_name_changed(GtkEditable *editable, gpointer user_data)
{
	const gchar *text;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.list_range_name));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(search1.list_range_name));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	text = gtk_entry_get_text(GTK_ENTRY(editable));
	gtk_list_store_set(list_store, &selected, 0, text, -1);
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

void range_text_changed(GtkEditable *editable, gpointer user_data)
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

void new_modlist(GtkButton *button, gpointer user_data)
{
	gchar buf[80];
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeModel *model2;
	GtkListStore *list_store2;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	GtkTreePath *path;

	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(search1.module_lists));

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_modules));
	list_store = GTK_LIST_STORE(model);

	model2 =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	list_store2 = GTK_LIST_STORE(model2);

	{
		// must encode locale-sensitively
		char *num = main_format_number(search1.list_rows);
		sprintf(buf, _("New List %s"), num);
		g_free(num);
	}

	search1.module_count = 0;
	gtk_list_store_clear(list_store);

	gtk_list_store_append(list_store2, &iter);
	gtk_list_store_set(list_store2, &iter, 0, buf, -1);
	path = gtk_tree_model_get_path(model2, &iter);
	gtk_tree_selection_select_path(selection, path);
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

void clear_modules(GtkButton *button, gpointer user_data)
{
	gchar *str;

	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
			      _("Clear List?"),
			      _("Are you sure you want to clear the module list?"));

#if GTK_CHECK_VERSION(3, 10, 0)
	if (gui_yes_no_dialog(str, "dialog-warning")) {
#else
	if (gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
#endif
		GtkTreeIter selected;
		GtkTreeModel *model =
		    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_modules));
		GtkListStore *list_store = GTK_LIST_STORE(model);
		gtk_list_store_clear(list_store);

		model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
		list_store = GTK_LIST_STORE(model);
		GtkTreeSelection *selection =
		    gtk_tree_view_get_selection(GTK_TREE_VIEW(search1.module_lists));
		if (gtk_tree_selection_get_selected(selection, NULL, &selected))
			gtk_list_store_set(list_store, &selected, 1, "",
					   -1);
	}
	g_free(str);
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

void delete_module(GtkButton *button, gpointer user_data)
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

void save_modlist(GtkButton *button, gpointer user_data)
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

void new_range(GtkButton *button, gpointer user_data)
{
	gchar *text[2];
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GtkTreePath *path;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.list_range_name));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(search1.list_range_name));

	text[0] = "[New Range]";
	text[1] = "";

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, 0, text[0], 1, text[1], -1);

	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_selection_select_path(selection, path);
	gtk_tree_path_free(path);

	gtk_entry_set_text(GTK_ENTRY(search1.entry_range_name), text[0]);
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

void save_range(GtkButton *button, gpointer user_data)
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

void delete_range(GtkButton *button, gpointer user_data)
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

void delete_list(GtkButton *button, gpointer user_data)
{
	gchar *name_string = NULL;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *str;

#if 0
	// #if 0'd until list_rows is properly managed.
	if (search1.list_rows < 2) {
		gui_generic_warning_modal(_("The last module list may not be deleted"));
		return;
	}
#endif

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(search1.module_lists));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	gtk_tree_model_get(model, &selected, 0, &name_string, -1);

	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s %s",
			      _("Delete list?"),
			      _("Are you sure you want to delete:"),
			      name_string);

#if GTK_CHECK_VERSION(3, 10, 0)
	if (!gui_yes_no_dialog(str, "dialog-warning")) {
		g_free(name_string);
		g_free(str);
		return;
	}
#else
	if (!gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
		g_free(name_string);
		g_free(str);
		return;
	}
#endif

	gtk_list_store_remove(list_store, &selected);
	xml_remove_node("modlists", "modlist", name_string);
	--search1.list_rows;
	save_modlist(NULL, NULL);

	g_free(name_string);
	g_free(str);
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

void scope_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	search1.which_scope = togglebutton;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(search1.rb_custom_range)))
		gtk_widget_set_sensitive(search1.combo_range, TRUE);
	else
		gtk_widget_set_sensitive(search1.combo_range, FALSE);
}

/******************************************************************************
 * Name
 *   mod_list_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void mod_list_toggled(GtkToggleButton *togglebutton,
 *			   gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void mod_list_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		main_comboboxentry2_changed((GtkComboBox *)
					    search1.combo_list,
					    user_data);
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(search1.rb_custom_list)))
		gtk_widget_set_sensitive(search1.combo_list, TRUE);
	else
		gtk_widget_set_sensitive(search1.combo_list, FALSE);
}

/******************************************************************************
 * Name
 *   optimized_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void optimized_toggled(GtkToggleButton *togglebutton,
 *			    gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void optimized_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		gtk_widget_show(search1.button_intro_lucene);
		gtk_widget_set_sensitive(search1.cb_case_sensitive, FALSE);
	} else {
		gtk_widget_hide(search1.button_intro_lucene);
		gtk_widget_set_sensitive(search1.cb_case_sensitive, TRUE);
	}
}

/******************************************************************************
 * Name
 *   on_lucene_intro_clicked
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_lucene_intro_clicked(GtkToggleButton *button,
 *				  gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

#define LUCENE_INTRO \
	_("<b>Syntax overview for optimized \"lucene\" searches</b>\nSearch for verses that contain...\n\nloved one\n\t \"loved\" or \"one\"\n\tThis is the same as searching for loved OR one\n\"loved one\"\n\tThe phrase \"loved one\"\nlove*\n\tA word starting with \"love\"\n\t(love OR loves OR loved OR etc...)\nloved AND one\n\tThe word \"loved\" and the word \"one\"\n\t&amp;&amp; can be used in place of AND\n+loved one\n\tVerses that <b>must</b> contain \"loved\" and <b>may</b> contain \"one\"\nloved NOT one\n\t\"loved\" but not \"one\"\n(loved one) AND God\n\t\"loved\" or \"one\" and \"God\"\nlemma:G2316\n\tSearch for the Strong's Greek (\"G\") word number 2316.\n\tAlso, select Strong's display on the <i>Attribute Search</i> tab.\n\nFor complete details, search the web for \"lucene search syntax\".")

void on_lucene_intro_clicked(GtkButton *button, gpointer user_data)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new_with_markup(NULL, /* no need for a parent window */
						    GTK_DIALOG_DESTROY_WITH_PARENT,
						    GTK_MESSAGE_INFO,
						    GTK_BUTTONS_OK,
						    LUCENE_INTRO);
	g_signal_connect_swapped(dialog, "response",
				 G_CALLBACK(gtk_widget_destroy), dialog);
	gtk_widget_show(dialog);
}

/******************************************************************************
 * Name
 *   attributes_toggled
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void attributes_toggled(GtkToggleButton *togglebutton,
 *			     gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void attributes_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton))
		gtk_widget_show(search1.button_intro_attributes);
	else
		gtk_widget_hide(search1.button_intro_attributes);
}

/******************************************************************************
 * Name
 *   on_attributes_intro_clicked
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *
 *   void on_attributes_intro_clicked(GtkToggleButton *button,
 *				  gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

#define ATTRIBUTES_INTRO \
	_("<b>Attribute-based searches</b>\nSearches for content contained in markup outside the main text. Attributes are footnotes, Strong's numbers, and morphological symbols.\n\nBe aware that most such searches can now be done faster via optimized \"lucene\" searches using keyword qualifiers.\n\nTo use attribute searches, you must select the appropriate button on the <i>Attribute Search</i> tab.\n* Footnote text is searched just like regular text.\n* Strong's words are specified as a prefix letter H or G (Hebrew or Greek) and the numeric word identifier, e.g. G2316 to find \"θεός\" (\"God\").\n* Morphological tags are identified literally, e.g. N-ASF for \"noun, accusative singular feminine\" -- see the Robinson module for details.")

void on_attributes_intro_clicked(GtkButton *button, gpointer user_data)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new_with_markup(NULL, /* no need for a parent window */
						    GTK_DIALOG_DESTROY_WITH_PARENT,
						    GTK_MESSAGE_INFO,
						    GTK_BUTTONS_OK,
						    ATTRIBUTES_INTRO);
	g_signal_connect_swapped(dialog, "response",
				 G_CALLBACK(gtk_widget_destroy), dialog);
	gtk_widget_show(dialog);
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

void current_module_toggled(GtkToggleButton *togglebutton,
			    gpointer user_data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		main_change_mods_select_label(search1.search_mod);
		gtk_widget_set_sensitive(search1.rb_last, TRUE);
		gtk_widget_set_sensitive(search1.combo_list, FALSE);
	} else {
		gtk_widget_set_sensitive(search1.rb_last, FALSE);
		gtk_toggle_button_set_active(search1.which_scope, TRUE);
	}
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

static void mod_selection_changed(GtkTreeSelection *selection,
				  GtkWidget *tree_widget)
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
					      selection,
					  gpointer data)
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
						 selection,
					     gpointer data)
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
				       selection,
				   GtkTreeView *tree_widget)
{
	gchar *mod = NULL;
	GtkTreeIter selected;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	if (gtk_tree_model_iter_has_child(model, &selected)) {
		g_free(module_selected);
		module_selected = NULL;
		return;
	}

	gtk_tree_model_get(model, &selected, UTIL_COL_MODULE, &mod, -1);
	if (mod) {
		g_free(module_selected);
		module_selected = mod;
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

static void _finds_verselist_selection_changed(GtkWidget *widget,
					       GdkEventButton *event,
					       gpointer data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter selected;
	gchar *key = NULL;

	selection = gtk_tree_view_get_selection((GtkTreeView *)
						search1.listview_verses);
	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_verses));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &key, -1);

	main_finds_verselist_selection_changed(selection, model,
					       event->type ==
						   GDK_2BUTTON_PRESS);
}

/******************************************************************************
 * Name
 *  on_treeview_button_advsearch_press_event
 *
 * Synopsis
 *   #include "gui/search_dialog.h"
 *   gboolean on_treeview_button_advsearch_press_event(GtkWidget * widget,
 *						       GdkEventButton * event,
 *						       gpointer data)
 *
 * Description
 *   handles context menu kickoff.
 *
 * Return value
 *   gboolean
 */

static gboolean on_treeview_button_press_event_advsearch(GtkWidget *
							     widget,
							 GdkEventButton *
							     event,
							 gpointer
							     user_data)
{
	if (event->button == 3) {
#if GTK_CHECK_VERSION(3, 22, 0)
		gtk_menu_popup_at_pointer((GtkMenu *)search1.menu_item_send_search, NULL);
#else
		gtk_menu_popup((GtkMenu *)search1.menu_item_send_search,
			       NULL, NULL, NULL, NULL, 2,
			       gtk_get_current_event_time());
#endif
		return TRUE;
	} else {
		return FALSE;
	}
}

/******************************************************************************
 * Name
 *   on_send_list_via_biblesync_advsearch_activate
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *   GtkWidget* on_send_list_via_biblesync_advsearch_activate
 *
 * Description
 *   ship the adv.search window's current verse list.
 *
 * Return value
 *   GtkWidget*
 */

G_MODULE_EXPORT void
on_send_list_via_biblesync_advsearch_activate(GtkMenuItem *menuitem,
					      gpointer user_data)
{
	if (biblesync_active_xmit_allowed()) {
		GtkTreeIter iter;
		GtkTreeModel *model =
		    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_verses));

		GString *vlist = g_string_new("");
		gchar *module = NULL, *text, *buf;

		gboolean first = TRUE;
		gboolean valid =
		    gtk_tree_model_get_iter_first(model, &iter);

		while (valid) {
			// retrieve a verse ref line.
			gtk_tree_model_get(model, &iter, 0, &text, -1);

			// painful parse of "NET: Revelation of John 2:12 and text here..."
			// first `:' finds end of module name.
			buf = strchr(text, ':');
			*buf = '\0';
			// key starts 2 characters after the (former) ':'.
			gchar *key = buf + 2;
			// next : is the middle of chapter:verse.
			buf = strchr(key, ':');
			// if that works, then space after that ends key.
			// it might not work, if not bible/comm...to be caught just below.
			if (buf)
				buf = strchr(buf, ' ');
			if (buf)
				*buf = '\0';

			if (first) {
				/* we can send only verse nav -- no lex, no books */
				module = g_strdup(text);
				if (!main_is_module(module) ||
				    ((main_get_mod_type(module) !=
				      TEXT_TYPE) &&
				     (main_get_mod_type(module) !=
				      COMMENTARY_TYPE))) {
					g_free(module);
					g_free(text);
					g_string_free(vlist, TRUE);
					gui_generic_warning(_("Module is neither Bible nor commentary"));
					return;
				}
			} else {
				vlist = g_string_append_c(vlist, ';');
			}
			vlist = g_string_append(vlist, (char *)
						main_get_osisref_from_key(module, key));

			g_free(text);
			first = FALSE;
			valid = gtk_tree_model_iter_next(model, &iter);
		}

		biblesync_transmit_verse_list(module, vlist->str);
		g_free(module);
		g_string_free(vlist, TRUE);
	} else {
		gui_generic_warning(_("BibleSync is not active for transmit."));
	}
}

/******************************************************************************
 * Name
 *   create_results_menu_advsearch
 *
 * Synopsis
 *   #include "gui/sidebar.h"
 *   GtkWidget* create_results_menu_advsearch (void)
 *
 * Description
 *   initialize context menu in adv.search results.
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *create_results_menu_advsearch(void)
{
	GtkWidget *menu;
	gchar *glade_file;
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
#if GTK_CHECK_VERSION(3, 14, 0)
	glade_file =
	    gui_general_user_file("xi-menus-popup.gtkbuilder", FALSE);
#else
	glade_file =
	    gui_general_user_file("xi-menus-popup_old.gtkbuilder", FALSE);
#endif
#else
	GladeXML *gxml;
	glade_file = gui_general_user_file("xi-menus.glade", FALSE);
#endif
	g_return_val_if_fail((glade_file != NULL), NULL);

#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, "menu_verselist_advsearch", NULL);
#endif

	g_free(glade_file);
	g_return_val_if_fail((gxml != NULL), NULL);

	menu = UI_GET_ITEM(gxml, "menu_verselist_advsearch");
#ifdef USE_GTKBUILDER
	/* connect signals and data */
	gtk_builder_connect_signals(gxml, NULL);
/*gtk_builder_connect_signals_full
	   (gxml, (GtkBuilderConnectFunc)gui_glade_signal_connect_func, NULL); */
#else
	/* connect signals and data */
	glade_xml_signal_autoconnect_full(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func,
					  NULL);
#endif

	return menu;
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

static void selection_range_lists_changed(GtkTreeSelection *selection,
					  gpointer data)
{
	gchar *name = NULL;
	gchar *range = NULL;
	GtkTreeModel *model;
	GtkTreeIter selected;

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.list_range_name));
	gtk_tree_model_get(model, &selected, 0, &name, 1, &range, -1);

	gtk_entry_set_text(GTK_ENTRY(search1.entry_range_name), name);
	gtk_entry_set_text(GTK_ENTRY(search1.entry_range_text), range);
	g_free(name);
	g_free(range);
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

static void selection_verselist_changed(GtkTreeSelection *selection,
					gpointer data)
{
	GtkTreeModel *model;
	GtkTreeIter selected;

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(search1.list_range_name));
	main_finds_verselist_selection_changed(selection, model, FALSE);
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

static void _add_two_text_columns(GtkTreeView *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Module",
							  renderer,
							  "text", 0, NULL);
	gtk_tree_view_append_column(treeview, column);
	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Module",
							  renderer,
							  "text", 1, NULL);
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

static void _setup_combobox(GtkComboBox *combo)
{
	GtkListStore *store;

	store = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(combo, GTK_TREE_MODEL(store));
#ifdef USE_GTK_3
	gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(combo), 0);
#else
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo), 0);
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

static void _setup_listviews(GtkWidget *listview, GCallback callback)
{
	GtkListStore *model;
	GObject *selection;

	model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(listview),
				GTK_TREE_MODEL(model));
	_add_two_text_columns(GTK_TREE_VIEW(listview));
	if (!callback)
		return;
	selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(listview)));
	g_signal_connect(selection, "changed", G_CALLBACK(callback), NULL);
}

static void _setup_listviews2(GtkWidget *listview, GCallback callback)
{
	GtkListStore *model;
	GObject *selection;

	model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(listview),
				GTK_TREE_MODEL(model));
	_add_two_text_columns(GTK_TREE_VIEW(listview));
	if (!callback)
		return;
	g_signal_connect((gpointer)listview,
			 "button_press_event", G_CALLBACK(callback), NULL);
	g_signal_connect((gpointer)listview,
			 "button_release_event",
			 G_CALLBACK(callback), NULL);
	selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(listview)));
	g_signal_connect(selection, "changed",
			 G_CALLBACK(selection_verselist_changed), NULL);
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

static void _setup_treeview(GtkWidget *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GObject *selection;

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Found",
							  renderer,
							  "text", 0, NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	gui_load_module_tree(treeview, FALSE);

	selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
	g_signal_connect(selection, "changed",
			 G_CALLBACK(mod_selection_changed), treeview);

	g_signal_connect_after(G_OBJECT(treeview),
			       "button_release_event",
			       G_CALLBACK(button_release_event),
			       GINT_TO_POINTER(0));
}

static void _setup_treeview2(GtkWidget *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GObject *selection;

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Found",
							  renderer,
							  "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gui_load_module_tree(treeview, FALSE);

	selection =
	    G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
	g_signal_connect(selection, "changed",
			 G_CALLBACK(_modules_lists_changed), treeview);
}

void on_closebutton2_clicked(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(search1.mod_sel_dialog);
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

void _on_dialog2_response(GtkDialog *dialog, gint response_id,
			  gpointer user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CLOSE:
		gtk_widget_hide(GTK_WIDGET(dialog));
		break;
	case GTK_RESPONSE_APPLY:
		main_add_mod_to_list(search1.listview_modules,
				     module_selected);
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
 *   Creates the module selection dialog
 *
 * Return value
 *   void
 */

#ifndef USE_GTKBUILDER
static void _create_mod_sel_dialog(void)
{
	gchar *glade_file;
	GladeXML *gxml2;

	glade_file = gui_general_user_file("search-dialog.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
	XI_message(("%s", glade_file));

	gxml2 = glade_xml_new(glade_file, "dialog2", NULL);
	search1.mod_sel_dialog = glade_xml_get_widget(gxml2, "dialog2");

	g_signal_connect((gpointer)search1.mod_sel_dialog, "response",
			 G_CALLBACK(_on_dialog2_response), NULL);

	search1.mod_sel_dlg_treeview =
	    glade_xml_get_widget(gxml2, "treeview8");

	_setup_treeview2(search1.mod_sel_dlg_treeview);
	gtk_widget_hide(search1.mod_sel_dialog);

	g_free(glade_file);
}
#endif

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
 *   Shows the module selection dialog
 *
 * Return value
 *   void
 */

void
on_toolbutton12_clicked(GtkToolButton *toolbutton, gpointer user_data)
{
#ifndef USE_GTKBUILDER
	_create_mod_sel_dialog();
#endif
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
static void _add_html_widget(GtkWidget *vbox)
{
#ifndef USE_WEBKIT2
	GtkWidget *scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow,
					    settings.shadow_type);
#endif

	search1.preview_html =
	    GTK_WIDGET(XIPHOS_HTML_NEW(NULL, FALSE, DIALOG_SEARCH_PREVIEW_TYPE));
	gtk_widget_show(search1.preview_html);
#ifdef USE_WEBKIT2
	gtk_box_pack_start(GTK_BOX(vbox), search1.preview_html, TRUE, TRUE, 0);
#else
	gtk_container_add(GTK_CONTAINER(scrolledwindow),
			  search1.preview_html);
	gtk_box_pack_start(GTK_BOX(scrolledwindow), search1.preview_html,
			   TRUE, TRUE, 0);
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

void _on_dialog_response(GtkDialog *dialog, gint response_id,
			 gpointer user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CLOSE:
		if (search1.mod_sel_dialog) {
			gtk_widget_destroy(GTK_WIDGET(search1.mod_sel_dialog));
			search1.mod_sel_dialog = NULL;
		}
		gtk_widget_destroy(GTK_WIDGET(dialog));
		break;
	}
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

static void _create_search_dialog(void)
{
	gchar *glade_file;
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
#else
	GladeXML *gxml;
#endif
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
	GtkWidget *toolbutton13;
	module_selected = NULL;
	verse_selected = NULL;
	_preview_on = TRUE;

	glade_file =
	    gui_general_user_file("search-dialog" UI_SUFFIX, FALSE);
	g_return_if_fail(glade_file != NULL);
	XI_message(("%s", glade_file));

/* build the widget */
#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, "dialog", NULL);
#endif
	g_return_if_fail(gxml != NULL);

	/* lookup the root widget */
	search1.dialog = UI_GET_ITEM(gxml, "dialog");
	gtk_window_resize(GTK_WINDOW(search1.dialog),
			  settings.advsearch_width,
			  settings.advsearch_height);

	g_signal_connect(search1.dialog, "response",
			 G_CALLBACK(_on_dialog_response), NULL);
	g_signal_connect(search1.dialog, "destroy",
			 G_CALLBACK(_on_destroy), NULL);

	remember_search = UI_GET_ITEM(gxml, "button1");
	g_signal_connect(remember_search, "clicked",
			 G_CALLBACK(on_button_begin_search), NULL);

	search1.label_search_module = UI_GET_ITEM(gxml, "label5");
	search1.search_entry = UI_GET_ITEM(gxml, "entry1");
	g_signal_connect(search1.search_entry, "activate",
			 G_CALLBACK(on_button_begin_search), NULL);

	search1.notebook = UI_GET_ITEM(gxml, "notebook1");
	search1.treeview = UI_GET_ITEM(gxml, "treeview1");
	_setup_treeview(search1.treeview);

	search1.list_range_name = UI_GET_ITEM(gxml, "treeview4");
	_setup_listviews(search1.list_range_name,
			 (GCallback)selection_range_lists_changed);

	search1.list_ranges = UI_GET_ITEM(gxml, "treeview5");
	_setup_listviews(search1.list_ranges, NULL);

	search1.module_lists = UI_GET_ITEM(gxml, "treeview6");
	_setup_listviews(search1.module_lists,
			 (GCallback)_selection_modules_lists_changed);

	search1.listview_modules = UI_GET_ITEM(gxml, "treeview7");
	_setup_listviews(search1.listview_modules, NULL);

	/* scope radio buttons */
	search1.rb_no_scope = UI_GET_ITEM(gxml, "radiobutton1");
	g_signal_connect(search1.rb_no_scope, "toggled",
			 G_CALLBACK(scope_toggled), NULL);

	search1.rb_last = UI_GET_ITEM(gxml, "radiobutton2");
	search1.which_scope = GTK_TOGGLE_BUTTON(search1.rb_no_scope);
	search1.rb_custom_range = UI_GET_ITEM(gxml, "radiobutton3");
	g_signal_connect(search1.rb_custom_range, "toggled",
			 G_CALLBACK(scope_toggled), NULL);
	/* modules radio buttons */
	search1.rb_current_module = UI_GET_ITEM(gxml, "radiobutton4");
	g_signal_connect(search1.rb_current_module, "toggled",
			 G_CALLBACK(current_module_toggled), NULL);
	search1.rb_mod_list = UI_GET_ITEM(gxml, "radiobutton5");
	g_signal_connect(search1.rb_mod_list, "toggled",
			 G_CALLBACK(mod_list_toggled), NULL);
	search1.rb_custom_list = UI_GET_ITEM(gxml, "radiobutton6");
	g_signal_connect(search1.rb_custom_list, "toggled",
			 G_CALLBACK(mod_list_toggled), NULL);

	/* search type selection */
	search1.rb_words = UI_GET_ITEM(gxml, "radiobutton9");
	search1.rb_regexp = UI_GET_ITEM(gxml, "radiobutton10");
	search1.rb_exact_phrase = UI_GET_ITEM(gxml, "radiobutton11");
	search1.rb_optimized = UI_GET_ITEM(gxml, "radiobutton16");
	g_signal_connect(search1.rb_optimized, "toggled",
			 G_CALLBACK(optimized_toggled), NULL);

	search1.button_intro_lucene =
	    UI_GET_ITEM(gxml, "button_intro_lucene");
	g_signal_connect(search1.button_intro_lucene, "clicked",
			 G_CALLBACK(on_lucene_intro_clicked), NULL);
	gtk_widget_show(search1.button_intro_lucene);

	search1.rb_attributes = UI_GET_ITEM(gxml, "radiobutton12");
	g_signal_connect(search1.rb_attributes, "toggled",
			 G_CALLBACK(attributes_toggled), NULL);

	search1.button_intro_attributes =
	    UI_GET_ITEM(gxml, "button_intro_attributes");
	g_signal_connect(search1.button_intro_attributes, "clicked",
			 G_CALLBACK(on_attributes_intro_clicked), NULL);
	gtk_widget_hide(search1.button_intro_attributes);

	/* attributes radio buttons */
	search1.rb_strongs = UI_GET_ITEM(gxml, "radiobutton13");
	search1.rb_morphs = UI_GET_ITEM(gxml, "radiobutton15");
	search1.rb_footnotes = UI_GET_ITEM(gxml, "radiobutton14");

	/*   */
	search1.cb_case_sensitive = UI_GET_ITEM(gxml, "checkbutton1");

	/* display options check buttons */
	search1.cb_include_strongs = UI_GET_ITEM(gxml, "checkbutton2");
	search1.cb_include_morphs = UI_GET_ITEM(gxml, "checkbutton3");
	search1.cb_include_footnotes = UI_GET_ITEM(gxml, "checkbutton4");

	toolbutton1 = UI_GET_ITEM(gxml, "toolbutton1");
	toolbutton2 = UI_GET_ITEM(gxml, "toolbutton2");
	toolbutton3 = UI_GET_ITEM(gxml, "toolbutton3");
	toolbutton4 = UI_GET_ITEM(gxml, "toolbutton4");
	toolbutton5 = UI_GET_ITEM(gxml, "toolbutton5");
	toolbutton6 = UI_GET_ITEM(gxml, "toolbutton6");
	toolbutton7 = UI_GET_ITEM(gxml, "toolbutton7");
	toolbutton8 = UI_GET_ITEM(gxml, "toolbutton8");
	toolbutton10 = UI_GET_ITEM(gxml, "toolbutton10");
	toolbutton11 = UI_GET_ITEM(gxml, "toolbutton11");
	toolbutton12 = UI_GET_ITEM(gxml, "toolbutton12");
	toolbutton13 = UI_GET_ITEM(gxml, "toolbutton_export");
	search1.combo_list = UI_GET_ITEM(gxml, "comboboxentry2");

	g_signal_connect(toolbutton1, "clicked",
			 G_CALLBACK(button_save), NULL);

	g_signal_connect(toolbutton2, "clicked",
			 G_CALLBACK(button_clean), NULL);

	g_signal_connect(toolbutton3, "clicked",
			 G_CALLBACK(new_range), NULL);

	g_signal_connect(toolbutton4, "clicked",
			 G_CALLBACK(save_range), NULL);

	g_signal_connect(toolbutton5, "clicked",
			 G_CALLBACK(delete_range), NULL);

	g_signal_connect(toolbutton6, "clicked",
			 G_CALLBACK(new_modlist), NULL);

	g_signal_connect(toolbutton7, "clicked",
			 G_CALLBACK(save_modlist), NULL);

	g_signal_connect(toolbutton8, "clicked",
			 G_CALLBACK(delete_list), NULL);

	g_signal_connect(toolbutton10, "clicked",
			 G_CALLBACK(clear_modules), NULL);

	g_signal_connect(toolbutton11, "clicked",
			 G_CALLBACK(delete_module), NULL);

	g_signal_connect(toolbutton12, "clicked",
			 G_CALLBACK(on_toolbutton12_clicked), NULL);

	g_signal_connect(toolbutton13, "clicked",
			 G_CALLBACK(button_export), NULL);

	_setup_combobox(GTK_COMBO_BOX(search1.combo_list));
	g_signal_connect(search1.combo_list, "changed",
			 G_CALLBACK(on_comboboxentry2_changed), NULL);

	search1.entry_list_name = UI_GET_ITEM(gxml, "entry4");
	g_signal_connect(search1.entry_list_name, "changed",
			 G_CALLBACK(list_name_changed), NULL);

	search1.combo_range = UI_GET_ITEM(gxml, "comboboxentry1");
	_setup_combobox(GTK_COMBO_BOX(search1.combo_range));

	search1.entry_range_name = UI_GET_ITEM(gxml, "entry2");
	g_signal_connect(search1.entry_range_name, "changed",
			 G_CALLBACK(range_name_changed), NULL);
	search1.entry_range_text = UI_GET_ITEM(gxml, "entry3");
	g_signal_connect(search1.entry_range_text, "changed",
			 G_CALLBACK(range_text_changed), NULL);

	search1.progressbar = UI_GET_ITEM(gxml, "progressbar1");
#ifdef USE_GTKBUILDER
	gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(search1.progressbar), TRUE);
#endif
	search1.label_mod_select = UI_GET_ITEM(gxml, "label5");
	search1.listview_results = UI_GET_ITEM(gxml, "treeview9");

#ifdef USE_GTKBUILDER
	/* setup module select dialog */
	search1.mod_sel_dialog = UI_GET_ITEM(gxml, "dialog2");
	g_signal_connect((gpointer)search1.mod_sel_dialog, "response",
			 G_CALLBACK(_on_dialog2_response), NULL);
	search1.mod_sel_dlg_treeview = UI_GET_ITEM(gxml, "treeview8");
	_setup_treeview2(search1.mod_sel_dlg_treeview);
	gtk_widget_hide(search1.mod_sel_dialog);
#endif

	_setup_listviews(search1.listview_results,
			 (GCallback)_selection_finds_list_changed);
	search1.listview_verses = UI_GET_ITEM(gxml, "treeview10");
	search1.menu_item_send_search = create_results_menu_advsearch();
	_setup_listviews2(search1.listview_verses,
			  (GCallback)_finds_verselist_selection_changed);
	g_signal_connect((gpointer)search1.listview_verses,
			 "button_press_event",
			 G_CALLBACK(on_treeview_button_press_event_advsearch), NULL);

#ifdef USE_GTKBUILDER
	_add_html_widget(GTK_WIDGET(gtk_builder_get_object(gxml, "vbox12")));
#else
	_add_html_widget(glade_xml_get_widget(gxml, "vbox12"));
#endif

	g_signal_connect((gpointer)search1.dialog,
			 "configure_event",
			 G_CALLBACK(on_advsearch_configure_event), NULL);

	settings.display_advsearch = 1;
	xml_set_value("Xiphos", "layout", "advsearchopen", "1");

	/* disable match case initially */
	gtk_widget_set_sensitive(search1.cb_case_sensitive, FALSE);

	/*
	 * (from xiphos.c)
	 * a little paranoia:
	 * clamp geometry values to a reasonable bound.
	 * sometimes xiphos gets insane reconfig events as it dies,
	 * especially if it's due to just shutting linux down.
	 */
	if ((settings.advsearch_x < 0) || (settings.advsearch_x > 2000))
		settings.advsearch_x = 40;
	if ((settings.advsearch_y < 0) || (settings.advsearch_y > 2000))
		settings.advsearch_y = 40;

	gtk_window_move(GTK_WINDOW(search1.dialog), settings.advsearch_x,
			settings.advsearch_y);
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
