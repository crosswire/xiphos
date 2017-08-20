/*
 * Xiphos Bible Study Tool
 * export_bookmarks.c -
 *
 * Copyright (C) 2003-2017 Xiphos Developer Team
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
#include <libxml/parser.h>
#ifndef USE_GTKBUILDER
#include <glade/glade-xml.h>
#endif

#include "gui/export_bookmarks.h"
#include "gui/xiphos.h"
#include "gui/bookmarks_menu.h"
#include "gui/bookmarks_treeview.h"
#include "gui/sidebar.h"
#include "gui/utilities.h"

#include "main/settings.h"
#include "main/sidebar.h"
#include "main/search_dialog.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/url.hh"

enum {
	BOOKMARKS,
	HTML,
	PLAIN
};

//extern GList *list_for_bookmarking;

void dialog_export_bookmarks_response_cb(GtkDialog *dialog,
					 gint response_id,
					 BK_EXPORT *data);
void checkbutton_include_text_toggled_cb(GtkToggleButton *togglebutton,
					 BK_EXPORT *data);
void radiobutton_plain_text_toggled_cb(GtkToggleButton *togglebutton,
				       BK_EXPORT *data);
void radiobutton_xiphos_bookmarks_toggled_cb(GtkToggleButton *
						 togglebutton,
					     BK_EXPORT *data);
void radiobutton_html_toggled_cb(GtkToggleButton *togglebutton,
				 BK_EXPORT *data);
gboolean dialog_vbox1_key_press_event_cb(GtkWidget *widget,
					 GdkEventKey *event,
					 gpointer user_data);

static gchar *get_module(const gchar *key)
{
	if (main_is_Bible_key(settings.MainWindowModule, (gchar *)key))
		return g_strdup(settings.MainWindowModule);
	return NULL;
}

void gui_set_html_item(GString *str,
		       const gchar *description,
		       const gchar *module,
		       const gchar *key, gboolean with_scripture)
{
	gchar *scripture = NULL;
	gchar *buf = NULL;

	if (with_scripture) {
		gchar *mod_tmp = NULL;
		gint modtype;

		if (strlen(module) < 2)
			mod_tmp = get_module(key);
		else
			mod_tmp = g_strdup(module);

		if (((modtype = main_get_mod_type(mod_tmp)) == TEXT_TYPE) || (modtype == COMMENTARY_TYPE)) {
			gchar *text = main_get_rendered_text(mod_tmp, key);
			scripture = g_strdup_printf("<li>%s</li>", text);
			g_free(text);
		}

		if (mod_tmp)
			g_free(mod_tmp);
	} else
		scripture = g_strdup("");

	buf = g_strdup_printf("<li>%s<ul><li>%s %s</li>%s</ul><br/></li>",
			      description,
			      module, key, (scripture ? scripture : ""));

	g_string_append(str, buf);
	g_free(buf);
	g_free(scripture);
}

void gui_set_plain_text_item(GString *str,
			     const gchar *description,
			     const gchar *module,
			     const gchar *key, gboolean with_scripture)
{
	gchar *scripture = NULL;
	gchar *buf = NULL;

	if (with_scripture) {
		gchar *mod_tmp = NULL;
		gint modtype;

		if (strlen(module) < 2)
			mod_tmp = get_module(key);
		else
			mod_tmp = g_strdup(module);

		if (((modtype = main_get_mod_type(mod_tmp)) == TEXT_TYPE) || (modtype == COMMENTARY_TYPE))
			scripture =
			    main_get_striptext(mod_tmp, (gchar *)key);

		if (mod_tmp)
			g_free(mod_tmp);
	} else
		scripture = g_strdup("");

	buf = g_strdup_printf("\t%s\n\t\t%s %s\n\t\t%s\n\n",
			      description,
			      module, key, (scripture ? scripture : ""));

	g_string_append(str, buf);
	g_free(buf);
	g_free(scripture);
}

static gboolean _save_verselist_2_xml(BK_EXPORT *data)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	//xmlAttrPtr root_attr;
	gchar filename[256];
	GString *name = g_string_new(NULL);
	GString *str = g_string_new("");

	sprintf(filename, "%s.xml", data->filename);

	root_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)"SwordBookmarks");
		//root_attr =
		xmlNewProp(root_node, (const xmlChar *)"syntaxVersion",
			   (const xmlChar *)"1");
		xmlDocSetRootElement(root_doc, root_node);
	}

	if (is_search_result)
		g_string_printf(name, _("Search result: %s"),
				settings.searchText);
	else
		g_string_printf(name, _("Verse List"));

	cur_node = xml_add_folder_to_parent(root_node, name->str); /* Verse List or Search result: */

	while (data->verses) {
		RESULTS *list_item = (RESULTS *)data->verses->data;
		g_string_printf(str, "%s, %s", list_item->key,
				list_item->module);

		xml_add_bookmark_to_parent(cur_node, str->str,				    // bookmark description
					   list_item->key,				    // module key
					   list_item->module,				    // module name
					   main_get_module_description(list_item->module)); // module description

		data->verses = g_list_next(data->verses);
	}

	xmlSaveFormatFile(filename, root_doc, 1);
	xmlFreeDoc(root_doc);
	return 1;
}

static gboolean _save_verselist_2_html(BK_EXPORT *data)
{
	gchar filename[256];
	GString *name = g_string_new(NULL);
	GString *str = g_string_new("");
	GString *des = g_string_new("");
	void (*catenate)(GString *str,
			 const gchar *description,
			 const gchar *module,
			 const gchar *key,
			 gboolean with_scripture) = NULL;

	switch (data->type) {
	case HTML:
		catenate = gui_set_html_item;
		sprintf(filename, "%s.html", data->filename);
		break;
	case PLAIN:
		catenate = gui_set_plain_text_item;
		sprintf(filename, "%s.txt", data->filename);
		break;
	}

	if (data->verselist == ADV_SEARCH_RESULTS_EXPORT) {

		main_export_current_adv_search(str, (data->type == HTML),
					       data->with_scripture);

	} else {
		gchar *buf;

		if (is_search_result)
			g_string_printf(name, _("Search result: %s"),
					settings.searchText);
		else
			g_string_printf(name, _("Verse List"));

		switch (data->type) {
		case HTML:
			buf =
			    g_strdup_printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /></head><body><ul><b>%s</b><br/><br/>",
					    name->str);
			break;
		case PLAIN:
			buf = g_strdup_printf("%s\n\n", name->str);
			break;
		default:
			buf =
			    g_strdup_printf("bogus data type, cannot happen");
			break;
		}
		g_string_append(str, buf);
		g_string_free(name, 1);
		g_free(buf);

		while (data->verses) {
			RESULTS *list_item = (RESULTS *)data->verses->data;
			g_string_printf(des, "%s, %s", list_item->key,
					list_item->module);

			(*catenate)(str, des->str, list_item->module,
				    list_item->key, data->with_scripture);
			data->verses = g_list_next(data->verses);
		}
		g_string_free(des, 1);
	}

	switch (data->type) {
	case HTML:
		g_string_append(str, "</ul></body></html>");
		break;
	case PLAIN:
		g_string_append(str, "\n");
		break;
	}

	/* not sure how well this will work with windows */
	if (!g_file_set_contents(filename, str->str, str->len, NULL))
		XI_warning(("Unable to save %s", filename));

	g_string_free(str, 1);
	return 1;
}

static void _export_verselist(BK_EXPORT *data)
{
	switch (data->type) {
	case BOOKMARKS:
		_save_verselist_2_xml(data);
		break;
	case HTML:
	case PLAIN:
		_save_verselist_2_html(data);
		break;
	}
}

/******************************************************************************
 * Name
 *  _parse_treeview_html
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void _parse_treeview_html (xmlNodePtr parent, GtkTreeIter * tree_parent)
 *
 * Description
 *
 *
 * Return value
 *   void
 */
static void _parse_treeview(GString *str, GtkTreeIter *tree_parent,
			    GtkTreeStore *model, BK_EXPORT *data)
{
	GtkTreeIter child;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;
	gchar *buf = NULL;
	void (*catenate)(GString *str,
			 const gchar *description,
			 const gchar *module,
			 const gchar *key,
			 gboolean with_scripture) = NULL;

	switch (data->type) {
	case HTML:
		catenate = gui_set_html_item;
		break;
	case PLAIN:
		catenate = gui_set_plain_text_item;
		break;
	}

	gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &child,
				     tree_parent);

	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &child,
				   2, &caption,
				   3, &key,
				   4, &module,
				   5, &mod_desc, 6, &description, -1);
		if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &child)) {
			switch (data->type) {
			case HTML:
				buf =
				    g_strdup_printf("<ul><b>%s</b>",
						    caption);
				break;
			case PLAIN:
				buf = g_strdup_printf("%s\n\n", caption);
				break;
			}
			g_string_append(str, buf);
			g_free(buf);
			_parse_treeview(str, &child, model, data);
		}
		if (key) {
			(*catenate)(str, description, module, key,
				    data->with_scripture);
		} else {
			switch (data->type) {
			case HTML:
				buf = g_strdup_printf("</ul>");
				break;
			case PLAIN:
				buf = g_strdup_printf(" ");
				break;
			}
			g_string_append(str, buf);
			g_free(buf);
		}

		g_free(caption);
		g_free(key);
		g_free(module);
		g_free(mod_desc);
		g_free(description);
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &child));
}

/******************************************************************************
 * Name
 *   save_iter_to_xml
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_iter_to_xml(GtkTreeIter * iter)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void save_iter_to_xml(GtkTreeIter *iter, BK_EXPORT *data)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	//      xmlAttrPtr root_attr;
	gchar *caption = NULL;
	gchar filename[256];
	GtkTreeModel *tm;

	if (data->verselist == VERSE_LIST_EXPORT)
		tm = GTK_TREE_MODEL(model_verselist);
	else
		tm = GTK_TREE_MODEL(model);

	gtk_tree_model_get(tm, iter, 2, &caption, -1);
	sprintf(filename, "%s.xml", data->filename);

	root_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)"SwordBookmarks");
		//root_attr =
		xmlNewProp(root_node, (const xmlChar *)"syntaxVersion",
			   (const xmlChar *)"1");
		xmlDocSetRootElement(root_doc, root_node);
	}
	gtk_tree_model_get(tm, iter, 2, &caption, -1);
	if (gtk_tree_model_iter_has_child(tm, iter)) {
		cur_node = xml_add_folder_to_parent(root_node, caption);
		utilities_parse_treeview(cur_node, iter, tm);
	}
	g_free(caption);
	xmlSaveFormatFile(filename, root_doc, 1);
	xmlFreeDoc(root_doc);

	bookmarks_changed = FALSE;
}

/******************************************************************************
 * Name
 *   _save_iter
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_iter_to_html(GtkTreeIter * iter, BK_EXPORT *data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void _save_iter(GtkTreeIter *iter, BK_EXPORT *data)
{
	gchar *caption = NULL;
	gchar filename[256];
	GString *str = g_string_new(NULL);

	gtk_tree_model_get(GTK_TREE_MODEL(model), iter, 2, &caption, -1);
	switch (data->type) {
	case HTML:
		sprintf(filename, "%s.html", data->filename);
		break;
	case PLAIN:
		sprintf(filename, "%s.txt", data->filename);
		break;
	}

	gtk_tree_model_get(GTK_TREE_MODEL(model), iter, 2, &caption, -1);
	if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), iter)) {
		switch (data->type) {
		case HTML:
			g_string_printf(str,
					"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /></head><body><ul><b>%s</b><br/><br/>",
					caption);
			break;
		case PLAIN:
			g_string_printf(str, "%s\n\n\n", caption);
			break;
		}

		_parse_treeview(str, iter, model, data);
	}

	g_free(caption);
	switch (data->type) {
	case HTML:
		g_string_append(str, "</ul></body></html>");
		break;
	case PLAIN:
		g_string_append(str, "\n");
		break;
	}

	/* not sure how well this will work with windows */
	if (!g_file_set_contents(filename, str->str, str->len, NULL))
		XI_warning(("Unable to save %s", filename));

	g_string_free(str, 1);
}

static void export_2_bookmarks(BK_EXPORT *data)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GtkTreeView *tree;

	if (data->verselist == VERSE_LIST_EXPORT)
		tree = GTK_TREE_VIEW(sidebar.results_list);
	else
		tree = bookmark_tree;

	selection = gtk_tree_view_get_selection(tree);
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	save_iter_to_xml(&selected, data);
}

static void export_2_html(BK_EXPORT *data)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar buf[256];

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	sprintf(buf, "%s.html", data->filename);
	_save_iter(&selected, data);
}

static void export_2_text(BK_EXPORT *data)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	_save_iter(&selected, data);
}

static void _export(BK_EXPORT *data)
{
	switch (data->type) {
	case BOOKMARKS:
		export_2_bookmarks(data);
		break;
	case HTML:
		export_2_html(data);
		break;
	case PLAIN:
		export_2_text(data);
		break;
	}
}

static void setup_filechooserwidget(GtkFileChooser *chooser)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *caption = NULL;

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
			   2, &caption, -1);
	gtk_file_chooser_set_current_name(chooser, caption);

	g_free(caption);
}

G_MODULE_EXPORT void
checkbutton_include_text_toggled_cb(GtkToggleButton *togglebutton,
				    BK_EXPORT *data)
{
	data->with_scripture = gtk_toggle_button_get_active(togglebutton);
}

G_MODULE_EXPORT void
radiobutton_xiphos_bookmarks_toggled_cb(GtkToggleButton *togglebutton,
					BK_EXPORT *data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		data->type = BOOKMARKS;
		gtk_widget_set_sensitive(data->cb_scripture, FALSE);
	}
}

G_MODULE_EXPORT void
radiobutton_html_toggled_cb(GtkToggleButton *togglebutton,
			    BK_EXPORT *data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		data->type = HTML;
		gtk_widget_set_sensitive(data->cb_scripture, TRUE);
	}
}

G_MODULE_EXPORT void
radiobutton_plain_text_toggled_cb(GtkToggleButton *togglebutton,
				  BK_EXPORT *data)
{
	if (gtk_toggle_button_get_active(togglebutton)) {
		data->type = PLAIN;
		gtk_widget_set_sensitive(data->cb_scripture, TRUE);
	}
}

G_MODULE_EXPORT
void dialog_export_bookmarks_response_cb(GtkDialog *dialog,
					 gint response_id,
					 BK_EXPORT *data)
{
	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
		break;
	case GTK_RESPONSE_OK:
		data->filename =
		    gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(data->filechooserwidget));
		if (data->verselist)
			_export_verselist(data);
		else
			_export(data);
		break;
	}
	if (data->filename)
		g_free(data->filename);
	g_free(data);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

G_MODULE_EXPORT
gboolean dialog_vbox1_key_press_event_cb(GtkWidget *widget,
					 GdkEventKey *event,
					 gpointer user_data)
{

	return TRUE;
}

void gui_export_bookmarks_dialog(gint export_type, GList *verses)
{
	GtkBuilder *builder;
	gchar *gbuilder_file;
	GtkWidget *dialog;
	GString *name = g_string_new(NULL);

	BK_EXPORT *ex_data = g_new0(BK_EXPORT, 1);

	ex_data->filename = NULL;
	ex_data->type = BOOKMARKS;
	ex_data->verselist = export_type;

	if (ex_data->verselist == VERSE_LIST_EXPORT) {
		ex_data->verses = verses;
		g_string_printf(name, "%s", _("VerseList"));
	} else if (ex_data->verselist == SEARCH_RESULTS_EXPORT ||
		   ex_data->verselist == ADV_SEARCH_RESULTS_EXPORT) {
		ex_data->verses = verses;
		g_string_printf(name, "%s", _("SearchResults"));
	}

	builder = gtk_builder_new();
	gbuilder_file =
	    gui_general_user_file("xi-export-bookmarks.glade", FALSE);
	gtk_builder_add_from_file(builder, gbuilder_file, NULL);

	dialog =
	    GTK_WIDGET(gtk_builder_get_object(builder, "dialog_export_bookmarks"));
	ex_data->rb_bookmarks =
	    GTK_WIDGET(gtk_builder_get_object(builder, "radiobutton_xiphos_bookmarks"));
	ex_data->rb_html =
	    GTK_WIDGET(gtk_builder_get_object(builder, "radiobutton_html"));
	ex_data->rb_plain =
	    GTK_WIDGET(gtk_builder_get_object(builder, "radiobutton_plain_text"));
	ex_data->cb_scripture =
	    GTK_WIDGET(gtk_builder_get_object(builder, "checkbutton_include_text"));
	ex_data->filechooserwidget =
	    GTK_WIDGET(gtk_builder_get_object(builder, "filechooserwidget"));

	if (ex_data->verselist) {
		gtk_file_chooser_set_current_name((GtkFileChooser *)
						  ex_data->filechooserwidget,
						  name->str);
		g_string_free(name, 1);
	} else
		setup_filechooserwidget((GtkFileChooser *)
					ex_data->filechooserwidget);

	gtk_widget_set_sensitive(ex_data->cb_scripture, FALSE);

	gtk_builder_connect_signals(builder, ex_data);

	if (ex_data->verselist == ADV_SEARCH_RESULTS_EXPORT) {
		gtk_widget_set_sensitive(ex_data->rb_bookmarks, FALSE);
		gtk_toggle_button_set_active((GtkToggleButton *)
					     ex_data->rb_html,
					     TRUE);
	}

	if (ex_data->verselist == ADV_SEARCH_RESULTS_EXPORT) {
		gtk_widget_set_sensitive(ex_data->rb_bookmarks, FALSE);
		gtk_toggle_button_set_active((GtkToggleButton *)
					     ex_data->rb_html,
					     TRUE);
	}

	g_object_unref(G_OBJECT(builder));
	gtk_widget_show(dialog);
}
