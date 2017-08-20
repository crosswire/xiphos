/*
 * Xiphos Bible Study Tool
 * bookmark_dialog.c - gui to popup a dialog for adding a bookmark
 *
 * Copyright (C) 2005-2017 Xiphos Developer Team
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

#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_menu.h"
#include "gui/bookmarks_treeview.h"
#include "gui/dialog.h"
#include "gui/utilities.h"

#include "main/display.hh"
#include "main/sword.h"
#include "main/settings.h"
#include "main/xml.h"

#include "xiphos_html/xiphos_html.h"

#include "gui/debug_glib_null.h"

extern GtkTreeStore *model;

static GtkWidget *treeview;
static GtkWidget *button_new_folder;
static GtkWidget *button_add_bookmark;
static GtkWidget *entry_label;
static GtkWidget *entry_key;
static GtkWidget *entry_module;
static GtkWidget *textview;
static GtkTextBuffer *textbuffer;
static gchar *note;

GtkWidget *bookmark_dialog;
GtkWidget *mark_verse_dialog;

void on_buffer_changed(GtkTextBuffer *textbuffer, gpointer user_data)
{
	GtkTextIter start;
	GtkTextIter end;

	gtk_text_buffer_get_start_iter(textbuffer, &start);
	gtk_text_buffer_get_end_iter(textbuffer, &end);
	if (note)
		g_free(note);
	note = gtk_text_buffer_get_text(textbuffer, &start, &end, FALSE);
	XI_message(("note: %s", note));
}

/******************************************************************************
 * Name
 *   add_bookmark_button
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 *   void add_bookmark_button(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void add_bookmark_button(void)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
	BOOKMARK_DATA *data;
	GtkTreeSelection *selection;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	data = g_new(BOOKMARK_DATA, 1);
	data->caption = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entry_label)));
	data->key = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entry_key)));
	data->module = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entry_module)));

	if (!strcmp(data->module, "studypad"))
		data->module_desc = "studypad";
	else
		data->module_desc =
		    g_strdup(main_get_module_description(data->module));

	data->description = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY(entry_label)));

	data->is_leaf = TRUE;
	data->opened = bm_pixbufs->pixbuf_helpdoc;
	data->closed = NULL;
	gui_add_item_to_tree(&iter, &selected, data);
	bookmarks_changed = TRUE;
	gui_save_bookmarks(NULL, NULL);

	g_free(data->caption);
	g_free(data->key);
	g_free(data->module);
	g_free(data->description);
	g_free(data);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

static void add_folder_button(void)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
	BOOKMARK_DATA *data;
	GtkTreeSelection *selection;
	char *t;
	gint test;
	GS_DIALOG *info;
	GString *str;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	t = "|";
	str = g_string_new("");
	info = gui_new_dialog();
	//info->stock_icon = GTK_STOCK_OPEN;
	info->title = _("Bookmark");
	g_string_printf(str, "<span weight=\"bold\">%s</span>",
			_("Enter Folder Name"));
	info->label_top = str->str;
	info->text1 = g_strdup(_("Folder Name"));
	info->label1 = _("Folder: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	data = g_new(BOOKMARK_DATA, 1);
	/*** open dialog to get name for new folder ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		char *buf = g_strdelimit(info->text1, t, ' ');
		data->caption = g_strdup(buf);
		data->key = NULL;
		data->module = NULL;
		data->module_desc = NULL;
		data->description = NULL;
		data->is_leaf = FALSE;
		data->opened = bm_pixbufs->pixbuf_opened;
		data->closed = bm_pixbufs->pixbuf_closed;
		gui_add_item_to_tree(&iter, &selected, data);
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
		g_free(data->caption);
		GtkTreePath *path =
		    gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
		gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeview),
					     path);
		gtk_tree_selection_select_path(selection, path);
		gtk_tree_path_free(path);
	}
	g_free(data);
	g_free(info->text1);
	g_free(info);
	g_string_free(str, TRUE);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

void on_dialog_response(GtkDialog *dialog,
			gint response_id, gpointer user_data)
{
	switch (response_id) {
	case GTK_RESPONSE_CANCEL: /*  cancel button pressed  */
	case GTK_RESPONSE_NONE:   /*  dialog destroyed  */
		gtk_widget_destroy(GTK_WIDGET(dialog));
		break;
	case GTK_RESPONSE_OK: /*  add button pressed  */
		add_bookmark_button();
		gtk_widget_destroy(GTK_WIDGET(dialog));
		break;
	case GTK_RESPONSE_ACCEPT: /*  add folder pressed  */
		add_folder_button();
		break;
	}
}

/******************************************************************************
 * Name
 *   on_dialog_enter
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 * Description
 *  "Enter" key route to on_dialog_response.
 *
 * Return value
 *   void
 */

void on_dialog_enter(void)
{
	on_dialog_response(GTK_DIALOG(bookmark_dialog),
			   GTK_RESPONSE_OK, NULL);
}

/******************************************************************************
 * Name
 *   on_mark_verse_response
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 * Description
 *   finish off a verse-markin event.
 *
 * Return value
 *   void
 */

void on_mark_verse_response(GtkDialog *dialog,
			    gint response_id, gpointer user_data)
{
	gchar reference[100], *module, *key;

	module = (gchar *)gtk_entry_get_text(GTK_ENTRY(entry_module));
	key = (gchar *)gtk_entry_get_text(GTK_ENTRY(entry_key));

	g_snprintf(reference, 100, "%s %s", module,
		   main_get_osisref_from_key((const char *)module,
					     (const char *)key));

	switch (response_id) {
	case GTK_RESPONSE_CANCEL: /*  cancel button pressed  */
	case GTK_RESPONSE_NONE:   /*  dialog destroyed  */
		break;
	case GTK_RESPONSE_ACCEPT: /*  mark the verse  */
		xml_set_list_item("osisrefmarkedverses", "markedverse",
				  reference,
				  (note ? note : "user content"));
		marked_cache_fill(settings.MainWindowModule,
				  settings.currentverse);
		main_display_bible(NULL, settings.currentverse);
		break;
	case GTK_RESPONSE_OK: /*  unmark the verse  */
		xml_remove_node("osisrefmarkedverses", "markedverse",
				reference);
		marked_cache_fill(settings.MainWindowModule,
				  settings.currentverse);
		main_display_bible(NULL, settings.currentverse);
		break;
	}
	g_free(note);
	gtk_widget_destroy(GTK_WIDGET(dialog));
	xml_save_settings_doc(settings.fnconfigure);
}

/******************************************************************************
 * Name
 *   on_mark_verse_enter
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 * Description
 *  "Enter" key route to on_mark_verse_response.
 *
 * Return value
 *   void
 */

void on_mark_verse_enter(void)
{
	on_mark_verse_response(GTK_DIALOG(mark_verse_dialog),
			       GTK_RESPONSE_ACCEPT, NULL);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

gboolean on_treeview_button_release_event(GtkWidget *widget,
					  GdkEventButton *event,
					  gpointer user_data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeModel *gmodel;
	GtkTreeIter selected;
	gchar *key = NULL;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	if (gtk_tree_selection_get_selected(selection, &gmodel, &selected)) {
		gtk_tree_model_get(gmodel, &selected, 3, &key, -1);
		if (!gtk_tree_model_iter_has_child(gmodel, &selected) && key != NULL) {
			gtk_widget_set_sensitive(button_new_folder, FALSE);
			gtk_widget_set_sensitive(button_add_bookmark,
						 FALSE);
		} else {
			gtk_widget_set_sensitive(button_new_folder, TRUE);
			gtk_widget_set_sensitive(button_add_bookmark,
						 TRUE);
		}
		if (key)
			g_free(key);
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

static void setup_treeview(void)
{
	GtkTreePath *path;
	GtkTreeIter iter;
	GtkTreeSelection *selection = NULL;

	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview),
				GTK_TREE_MODEL(model));
	gui_add_columns(GTK_TREE_VIEW(treeview));
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
	gtk_tree_view_expand_to_path(GTK_TREE_VIEW(treeview), path);
	gtk_tree_selection_select_path(selection, path);
	gtk_tree_path_free(path);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

static GtkWidget *_create_bookmark_dialog(gchar *label,
					  gchar *module, gchar *key)
{
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
#else
	GladeXML *gxml;
#endif
	gchar *glade_file =
	    gui_general_user_file("bookmarks" UI_SUFFIX, TRUE);
	g_return_val_if_fail(glade_file != NULL, NULL);
	XI_message(("%s", glade_file));

/* build the widget */
#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, NULL, NULL);
#endif
	g_free(glade_file);
	g_return_val_if_fail(gxml != NULL, NULL);

	/* lookup the root widget */
	bookmark_dialog = UI_GET_ITEM(gxml, "dialog");
	g_signal_connect(bookmark_dialog, "response",
			 G_CALLBACK(on_dialog_response), NULL);

	/* treeview */
	treeview = UI_GET_ITEM(gxml, "treeview");
	setup_treeview();
	g_signal_connect(treeview, "button-release-event",
			 G_CALLBACK(on_treeview_button_release_event),
			 NULL);
	/* entrys */
	entry_label = UI_GET_ITEM(gxml, "entry1");
	entry_key = UI_GET_ITEM(gxml, "entry2");
	entry_module = UI_GET_ITEM(gxml, "entry3");

	gtk_entry_set_text(GTK_ENTRY(entry_label), label);
	gtk_entry_set_text(GTK_ENTRY(entry_key), key);
	gtk_entry_set_text(GTK_ENTRY(entry_module), module);
	g_signal_connect(entry_label, "activate",
			 G_CALLBACK(on_dialog_enter), NULL);
	g_signal_connect(entry_key, "activate",
			 G_CALLBACK(on_dialog_enter), NULL);
	g_signal_connect(entry_module, "activate",
			 G_CALLBACK(on_dialog_enter), NULL);

	/* dialog buttons */
	button_new_folder = UI_GET_ITEM(gxml, "button1");
	button_add_bookmark = UI_GET_ITEM(gxml, "button3");

	return bookmark_dialog;
}

/******************************************************************************
 * Name
 *   _create_mark_verse_dialog
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 * Description
 *   marks/unmarks a verse for display highlight.
 *
 * Return value
 *   void
 */

static GtkWidget *_create_mark_verse_dialog(gchar *module, gchar *key)
{
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
#else
	GladeXML *gxml;
#endif
	GtkWidget *sw;
	gchar osisreference[100];
	gchar *old_note = NULL;

	gchar *glade_file =
	    gui_general_user_file("markverse" UI_SUFFIX, TRUE);
	g_return_val_if_fail(glade_file != NULL, NULL);
	XI_message(("%s", glade_file));

	g_snprintf(osisreference, 100, "%s %s", module,
		   main_get_osisref_from_key((const char *)module,
					     (const char *)key));
	note = NULL;

/* build the widget */
#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, NULL, NULL);
#endif
	g_free(glade_file);
	g_return_val_if_fail(gxml != NULL, NULL);

	/* lookup the root widget */
	mark_verse_dialog = UI_GET_ITEM(gxml, "dialog");
	gtk_window_set_default_size(GTK_WINDOW(mark_verse_dialog),
				    300, 350);

	g_signal_connect(mark_verse_dialog, "response",
			 G_CALLBACK(on_mark_verse_response), NULL);

	/* entrys */
	entry_key = UI_GET_ITEM(gxml, "entry2");
	entry_module = UI_GET_ITEM(gxml, "entry3");
	textview = UI_GET_ITEM(gxml, "textview");

	textbuffer = gtk_text_view_get_buffer((GtkTextView *)textview);
	gtk_entry_set_text(GTK_ENTRY(entry_key), key);
	gtk_entry_set_text(GTK_ENTRY(entry_module), module);

	sw = UI_GET_ITEM(gxml, "scrolledwindow1");

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview),
				    GTK_WRAP_WORD);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	g_signal_connect(entry_key, "activate",
			 G_CALLBACK(on_mark_verse_enter), NULL);
	g_signal_connect(entry_module, "activate",
			 G_CALLBACK(on_mark_verse_enter), NULL);

	old_note =
	    xml_get_list_from_label("osisrefmarkedverses", "markedverse",
				    osisreference);
	note = g_strdup((old_note) ? old_note : "");
	gtk_text_buffer_set_text(textbuffer, (old_note) ? old_note : "",
				 -1);
	g_signal_connect(textbuffer, "changed",
			 G_CALLBACK(on_buffer_changed), NULL);

	return mark_verse_dialog;
}

/******************************************************************************
 * Name
 *   gui_bookmark_dialog
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *
 *   void gui_bookmark_dialog(gchar * label, gchar * module_name, gchar * key)
 *
 * Description
 *   calls _create_bookmark_dialog() and use gtk_dialog_run()
 *   to make it modal (needed for saving multiple search results)
 *
 * Return value
 *   void
 */

void gui_bookmark_dialog(gchar *label, gchar *module_name, gchar *key)
{
	GtkWidget *dialog =
	    _create_bookmark_dialog(label, module_name, key);
	if (!dialog)
		return;
	gtk_dialog_run(GTK_DIALOG(dialog));
}

/******************************************************************************
 * Name
 *   gui_mark_verse_dialog
 *
 * Synopsis
 *   #include "gui/bookmark_dialog.h"
 *   void gui_mark_verse_dialog(gchar * module_name, gchar * key)
 *
 * Description
 *   marks/unmarks a verse for display highlighting.
 *
 * Return value
 *   void
 */

void gui_mark_verse_dialog(gchar *module_name, gchar *key)
{
	GtkWidget *dialog = _create_mark_verse_dialog(module_name, key);
	if (!dialog)
		return;
	gtk_dialog_run(GTK_DIALOG(dialog));
}
