/*
 * Xiphos Bible Study Tool
 * search_dialog.c - gui for searching Sword modules
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
#include <glade/glade-xml.h>

#ifdef USE_GTKMOZEMBED
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "webkit/wk-html.h"
#endif
#else
#include <gtkhtml/gtkhtml.h>
#include "gui/html.h"
#endif

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

#include "gui/debug_glib_null.h"

/******************************************************************************/
#define _BYTE   8
#define _WORD   16
#define _DWORD  32

#define SEARCHING N_("Searching the ")
#define SMODULE N_(" Module")
#define FINDS N_("found in ")
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"


SEARCH_DIALOG1 search1;

static gboolean _preview_on;
static gchar *module_selected;
gchar *verse_selected;

GtkWidget *remember_search;	/* needed to change button in search stop */

/* click on treeview folder to expand or collapse it */
static gboolean button_release_event(GtkWidget * widget,
				     GdkEventButton * event,
				     gpointer data)
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
	if (gtk_tree_view_row_expanded (GTK_TREE_VIEW(widget), path))
	       gtk_tree_view_collapse_row ( GTK_TREE_VIEW(widget), path );
        else
	       gtk_tree_view_expand_row ( GTK_TREE_VIEW(widget), path, FALSE );
	gtk_tree_path_free ( path );
	return FALSE;
}

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
 *   void button_clean(GtkButton * button, gpointer user_data)
 *
 * Description
 *   user pressed clear button - clears search results page
 *
 * Return value
 *   void
 */

void button_clean(GtkButton * button, gpointer user_data)
{
	GtkTreeModel *model;
	GtkListStore *list_store;
#ifdef USE_GTKMOZEMBED
	GString *html_text;
#endif

	GS_message(("button_clean"));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_results));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_verses));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);

#ifdef USE_GTKMOZEMBED
	html_text = g_string_new(HTML_START);
	g_string_append(html_text," ");

	g_string_append(html_text,"</html>");	
	wk_html_open_stream(WK_HTML(search1.preview_html),"text/html");
	wk_html_write(WK_HTML(search1.preview_html),html_text->str,html_text->len);
	wk_html_close(WK_HTML(search1.preview_html));
	g_string_free(html_text,TRUE);
#else
	gtk_html_load_from_string(GTK_HTML(search1.preview_html),
				  " ", strlen(" "));
#endif
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

void button_save(GtkButton * button, gpointer user_data)
{
	main_save_current_adv_search_as_bookmarks ();
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

void button_export(GtkButton * button, gpointer user_data)
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

void _on_destroy(GtkWidget * dialog, gpointer user_data)
{
	/* main_do_dialog_search() initializes these as search starts */
	if (search_active) {
		terminate_search = TRUE;
		sync_windows();
	} else {
		main_close_search_dialog();
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

void on_button_begin_search(GtkButton * button, gpointer user_data)
{
	if (search_active) {
		terminate_search = TRUE;
		gtk_button_set_label((GtkButton *)remember_search, "gtk-find");
		gtk_button_set_use_stock((GtkButton *)remember_search, TRUE);
		sync_windows();
	} else {
		gtk_button_set_label((GtkButton *)remember_search, "gtk-stop");
		gtk_button_set_use_stock((GtkButton *)remember_search, TRUE);
		main_do_dialog_search();
		gtk_button_set_label((GtkButton *)remember_search, "gtk-find");
		gtk_button_set_use_stock((GtkButton *)remember_search, TRUE);
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

void list_name_changed(GtkEditable * editable, gpointer user_data)
{
	const gchar *text;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.module_lists));

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

void range_name_changed(GtkEditable * editable, gpointer user_data)
{
	const gchar *text;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.list_range_name));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.list_range_name));

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
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
	gchar buf[80];
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeModel *model2;
	GtkListStore *list_store2;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	GtkTreePath *path;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(search1.module_lists));

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_modules));
	list_store = GTK_LIST_STORE(model);

	model2 = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
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

void clear_modules(GtkButton * button, gpointer user_data)
{
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *str;

	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
			      _("Clear List?"),
			      _("Are you sure you want to clear the module list?"));

	if (gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_modules));
		list_store = GTK_LIST_STORE(model);
		gtk_list_store_clear(list_store);

		model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.module_lists));
		list_store = GTK_LIST_STORE(model);
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(search1.module_lists));
		if (gtk_tree_selection_get_selected(selection, NULL, &selected))
			gtk_list_store_set(list_store, &selected, 1, "", -1);
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
	GtkTreeIter iter;
	GtkTreePath *path;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	list_store = GTK_LIST_STORE(model);
	selection = gtk_tree_view_get_selection
	    (GTK_TREE_VIEW(search1.list_range_name));

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
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *str;

#if 0
	// #if 0'd until list_rows is properly managed.
	if (search1.list_rows < 2) {
		gui_generic_warning(_("The last module list may not be deleted"));
		return;
	}
#endif

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

	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s %s",
			      _("Delete list?"),
			      _("Are you sure you want to delete:"),
			      name_string);

	if (!gui_yes_no_dialog(str, GTK_STOCK_DIALOG_WARNING)) {
		g_free(name_string);
		g_free(str);
		return;
	}

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

void scope_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	search1.which_scope = togglebutton;
	if (GTK_TOGGLE_BUTTON(search1.rb_custom_range)->active)
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
 *			   gpointer user_data)
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
	if (GTK_TOGGLE_BUTTON(search1.rb_custom_list)->active)
		gtk_widget_set_sensitive(search1.combo_list,TRUE);
	else
		gtk_widget_set_sensitive(search1.combo_list,FALSE);
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

void optimized_toggled(GtkToggleButton * togglebutton,
		      gpointer user_data)
{
	if (togglebutton->active)
		gtk_widget_show(search1.button_intro_lucene);
	else
		gtk_widget_hide(search1.button_intro_lucene);
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

#define	LUCENE_INTRO	\
_("<b>Syntax overview for optimized \"lucene\" searches</b>\nSearch for verses that contain...\n\nloved one\n\t \"loved\" or \"one\"\n\tThis is the same as searching for loved OR one\n\"loved one\"\n\tThe phrase \"loved one\"\nlove*\n\tA word starting with \"love\"\n\t(love OR loves OR loved OR etc...)\nloved AND one\n\tThe word \"loved\" and the word \"one\"\n\t&amp;&amp; can be used in place of AND\n+loved one\n\tVerses that <b>must</b> contain \"loved\" and <b>may</b> contain \"one\"\nloved NOT one\n\t\"loved\" but not \"one\"\n(loved one) AND God\n\t\"loved\" or \"one\" and \"God\"\nlemma:G2316\n\tSearch for the Strong's Greek (\"G\") word number 2316.\n\tAlso, select Strong's display on the <i>Attribute Search</i> tab.\n\nFor complete details, search the web for \"lucene search syntax\".")

void
on_lucene_intro_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new_with_markup
	    (NULL,	/* no need for a parent window */
	     GTK_DIALOG_DESTROY_WITH_PARENT,
	     GTK_MESSAGE_INFO,
	     GTK_BUTTONS_OK,
	     LUCENE_INTRO);
	g_signal_connect_swapped (dialog, "response",
				  G_CALLBACK (gtk_widget_destroy),
				  dialog);
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

void attributes_toggled(GtkToggleButton * togglebutton,
		      gpointer user_data)
{
	if (togglebutton->active)
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

#define	ATTRIBUTES_INTRO	\
_("<b>Attribute-based searches</b>\nSearches for content contained in markup outside the main text. Attributes are footnotes, Strong's numbers, and morphological symbols.\n\nBe aware that most such searches can now be done faster via optimized \"lucene\" searches using keyword qualifiers.\n\nTo use attribute searches, you must select the appropriate button on the <i>Attribute Search</i> tab.\n* Footnote text is searched just like regular text.\n* Strong's words are specified as a prefix letter H or G (Hebrew or Greek) and the numeric word identifier, e.g. G2316 to find \"θεός\" (\"God\").\n* Morphological tags are identified literally, e.g. N-ASF for \"noun, accusative singular feminine\" -- see the Robinson module for details.")

void
on_attributes_intro_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new_with_markup
	    (NULL,	/* no need for a parent window */
	     GTK_DIALOG_DESTROY_WITH_PARENT,
	     GTK_MESSAGE_INFO,
	     GTK_BUTTONS_OK,
	     ATTRIBUTES_INTRO);
	g_signal_connect_swapped (dialog, "response",
				  G_CALLBACK (gtk_widget_destroy),
				  dialog);
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
	GtkTreeIter selected;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_widget));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	if (gtk_tree_model_iter_has_child(model, &selected)) {
		g_free(module_selected);
		module_selected = NULL;
		return;
	}

	gtk_tree_model_get(model, &selected, 0, &mod, -1);
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

static void _finds_verselist_selection_changed(GtkWidget * widget,
						GdkEventButton * event,
					    	gpointer data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter selected;
	gchar *key = NULL;

	selection =
	    gtk_tree_view_get_selection((GtkTreeView *) search1.listview_verses);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(search1.listview_verses));

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected, 0, &key, -1);

	main_finds_verselist_selection_changed(selection, model, event->type == GDK_2BUTTON_PRESS);
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

static void selection_verselist_changed(GtkTreeSelection * selection,
					  gpointer data)
{
	GtkTreeModel *model;
	GtkTreeIter selected;

	if (!gtk_tree_selection_get_selected
	    (selection, NULL, &selected))
		return;

	model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW
				    (search1.list_range_name));
	main_finds_verselist_selection_changed(selection,
					    model,
					    FALSE);
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
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo), 0);
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
	GtkListStore *model;
	GObject *selection;

	model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(listview), GTK_TREE_MODEL(model));
	_add_two_text_columns(GTK_TREE_VIEW(listview));
	if (!callback)
		return;
	selection = G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(listview)));
	g_signal_connect(selection, "changed", G_CALLBACK(callback), NULL);

}

static
void _setup_listviews2(GtkWidget * listview, GCallback callback)
{
	GtkListStore *model;
	GObject *selection;

	model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model(GTK_TREE_VIEW(listview), GTK_TREE_MODEL(model));
	_add_two_text_columns(GTK_TREE_VIEW(listview));
	if (!callback)
		return;
	g_signal_connect((gpointer) listview,
			 "button_press_event",
			 G_CALLBACK(callback), NULL);
	g_signal_connect((gpointer) listview,
				"button_release_event",
				G_CALLBACK(callback), NULL);
	selection = G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(listview)));
	g_signal_connect(selection, "changed", G_CALLBACK(selection_verselist_changed),
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
void _setup_treeview(GtkWidget * treeview)
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

	selection = G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
	g_signal_connect(selection, "changed",
			 G_CALLBACK(mod_selection_changed), treeview);

	g_signal_connect_after(G_OBJECT(treeview),
			       "button_release_event",
			       G_CALLBACK(button_release_event),
			       GINT_TO_POINTER(0));
}


static
void _setup_treeview2(GtkWidget * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GObject *selection;

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Found",
							  renderer,
							  "text",
							  0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
	gtk_tree_view_column_set_sort_column_id(column, 0);
	gui_load_module_tree(treeview, FALSE);

	selection = G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview)));
	g_signal_connect(selection, "changed", G_CALLBACK(_modules_lists_changed), treeview);
}


void on_closebutton2_clicked(GtkButton * button, gpointer user_data)
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

static
void _create_mod_sel_dialog(void)
{
	gchar *glade_file;
	GladeXML *gxml2;

	glade_file = gui_general_user_file("search-dialog.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
	GS_message(("%s",glade_file));

	gxml2 = glade_xml_new(glade_file, "dialog2", NULL);
	search1.mod_sel_dialog = glade_xml_get_widget(gxml2, "dialog2");
	g_signal_connect((gpointer)search1.mod_sel_dialog, "response",
			 G_CALLBACK(_on_dialog2_response), NULL);
	search1.mod_sel_dlg_treeview = glade_xml_get_widget(gxml2, "treeview8");
	_setup_treeview2(search1.mod_sel_dlg_treeview);
	gtk_widget_hide(search1.mod_sel_dialog);

	g_free(glade_file);

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
 *   Shows the module selection dialog
 *
 * Return value
 *   void
 */

void
on_toolbutton12_clicked(GtkToolButton * toolbutton, gpointer user_data)
{
        _create_mod_sel_dialog();
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
void _add_html_widget(GtkWidget * vbox)
{
#ifdef USE_GTKMOZEMBED
	search1.preview_html = GTK_WIDGET(wk_html_new());
	gtk_box_pack_start(GTK_BOX(vbox), search1.preview_html, TRUE, TRUE, 0);
#else
	GtkWidget *scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow);
	gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
					    scrolledwindow,
					    settings.shadow_type);
	search1.preview_html = gtk_html_new();
	gtk_container_add(GTK_CONTAINER(scrolledwindow), search1.preview_html);
	gtk_box_pack_start(GTK_BOX(scrolledwindow), search1.preview_html, TRUE, TRUE, 0);
#endif
	gtk_widget_show(search1.preview_html);
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

static
void _create_search_dialog(void)
{
	gchar *glade_file;
	GladeXML *gxml;
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

	glade_file = gui_general_user_file("search-dialog.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
	GS_message(("%s",glade_file));

	/* build the widget */
	gxml = glade_xml_new(glade_file, "dialog", NULL);
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

	remember_search = glade_xml_get_widget(gxml, "button1");
	g_signal_connect(remember_search, "clicked",
			 G_CALLBACK(on_button_begin_search), NULL);

	search1.label_search_module =
	    glade_xml_get_widget(gxml, "label5");
	search1.search_entry = glade_xml_get_widget(gxml, "entry1");
	g_signal_connect(search1.search_entry, "activate",
			 G_CALLBACK(on_button_begin_search), NULL);

	search1.notebook = glade_xml_get_widget(gxml, "notebook1");

	search1.treeview = glade_xml_get_widget(gxml, "treeview1");
	_setup_treeview(search1.treeview);

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

	/* search type selection */
	search1.rb_words        = glade_xml_get_widget(gxml, "radiobutton9");
	search1.rb_regexp       = glade_xml_get_widget(gxml, "radiobutton10");
	search1.rb_exact_phrase = glade_xml_get_widget(gxml, "radiobutton11");

	search1.rb_optimized    = glade_xml_get_widget(gxml, "radiobutton16");
	g_signal_connect(search1.rb_optimized, "toggled",
			 G_CALLBACK(optimized_toggled), NULL);

	search1.button_intro_lucene = glade_xml_get_widget(gxml, "button_intro_lucene");
	g_signal_connect(search1.button_intro_lucene, "clicked",
			 G_CALLBACK(on_lucene_intro_clicked), NULL);
	gtk_widget_hide(search1.button_intro_lucene);

	search1.rb_attributes = glade_xml_get_widget(gxml, "radiobutton12");
	g_signal_connect(search1.rb_attributes, "toggled",
			 G_CALLBACK(attributes_toggled), NULL);

	search1.button_intro_attributes = glade_xml_get_widget(gxml, "button_intro_attributes");
	g_signal_connect(search1.button_intro_attributes, "clicked",
			 G_CALLBACK(on_attributes_intro_clicked), NULL);
	gtk_widget_hide(search1.button_intro_attributes);

	/* attributes radio buttons */
	search1.rb_strongs    = glade_xml_get_widget(gxml, "radiobutton13");
	search1.rb_morphs     = glade_xml_get_widget(gxml, "radiobutton15");
	search1.rb_footnotes  = glade_xml_get_widget(gxml, "radiobutton14");

	/*   */
	search1.cb_case_sensitive = glade_xml_get_widget(gxml, "checkbutton1");

	/* display options check buttons */
	search1.cb_include_strongs   = glade_xml_get_widget(gxml, "checkbutton2");
	search1.cb_include_morphs    = glade_xml_get_widget(gxml, "checkbutton3");
	search1.cb_include_footnotes = glade_xml_get_widget(gxml, "checkbutton4");

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

	toolbutton13 = glade_xml_get_widget(gxml, "toolbutton_export");
	g_signal_connect(toolbutton13, "clicked",
			 G_CALLBACK(button_export), NULL);

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
	_setup_listviews2(search1.listview_verses, (GCallback) _finds_verselist_selection_changed);
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
