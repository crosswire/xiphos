/*
 * Xiphos Bible Study Tool
 * bookmarks_menu.c - gui for bookmarks using menu
 *
 * Copyright (C) 2003-2025 Xiphos Developer Team
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

#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "gui/xiphos.h"
#include "gui/bookmarks_menu.h"
#include "gui/bookmarks_treeview.h"
#include "gui/export_bookmarks.h"
#include "gui/utilities.h"
#include "gui/main_window.h"
#include "gui/dialog.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex_dialog.h"
#include "gui/gbs_dialog.h"
#include "gui/widgets.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/sidebar.h"
#include "main/xml.h"
#include "main/module_dialogs.h"
#include "main/url.hh"

#ifdef USE_WEBKIT_EDITOR
#include "editor/webkit_editor.h"
#else
#include "editor/slib-editor.h"
#endif

#include "gui/debug_glib_null.h"

BOOKMARK_MENU menu;

gboolean bookmarks_changed;

/******************************************************************************
 * Name
 *  save_treeview_to_xml_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_treeview_to_xml_bookmarks(GtkTreeIter * iter, gchar *file_buf)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

static void save_treeview_to_xml_bookmarks(GtkTreeIter *iter,
					   gchar *filename)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;
	gchar *color = NULL;

	if (!bookmarks_changed)
		return;

	root_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)"SwordBookmarks");
		xmlNewProp(root_node, (const xmlChar *)"syntaxVersion",
			   (const xmlChar *)"1");
		xmlDocSetRootElement(root_doc, root_node);
	}

	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
				   COL_CAPTION, &caption,
				   COL_KEY, &key,
				   COL_MODULE, &module,
				   COL_MODULE_DESC, &mod_desc,
				   COL_DESCRIPTION, &description,
				   COL_COLOR, &color,
				   -1);
		if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), iter)) {
			/* folder node — write color attribute when present */
			cur_node = xml_add_folder_to_parent_colored(root_node,
								    caption,
								    color);
			utilities_parse_treeview(cur_node, iter,
						 GTK_TREE_MODEL(model));
		} else {
			xml_add_bookmark_to_parent(root_node,
						   description,
						   key, module, mod_desc);
		}
		g_free(caption);
		g_free(key);
		g_free(module);
		g_free(mod_desc);
		g_free(description);
		g_free(color);
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), iter));

	xmlSaveFormatFile(filename, root_doc, 1);
	xmlFreeDoc(root_doc);
	g_free(filename);
	bookmarks_changed = FALSE;
}

/******************************************************************************
 * Name
 *  add_node_to_ctree
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   GtkCTreeNode *add_node_to_ctree(GtkCTree * ctree,
 *			GtkCTreeNode *node, BOOKMARK_DATA * data)
 *
 * Description
 *    actually add the GtkCTreeNode to the bookmark ctree
 *
 * Return value
 *   GtkCTreeNode
 */

static void add_item_to_tree(GtkTreeIter *iter, GtkTreeIter *parent,
			     BOOKMARK_DATA *data)
{
	gtk_tree_store_append(GTK_TREE_STORE(model), iter, parent);

	gtk_tree_store_set(GTK_TREE_STORE(model), iter,
			   COL_OPEN_PIXBUF, data->opened,
			   COL_CLOSED_PIXBUF, data->closed,
			   COL_CAPTION, data->caption,
			   COL_KEY, data->key,
			   COL_MODULE, data->module,
			   COL_MODULE_DESC, data->module_desc,
			   COL_DESCRIPTION, data->description,
			   COL_COLOR, data->color,
			   -1);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void bibletime_bookmarks_activate(GtkMenuItem *menuitem,
						  gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeIter parent;
	gchar *fname;
	GtkWidget *dialog;

	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &parent))
		return;

	dialog = gtk_file_chooser_dialog_new(_("Specify bookmarks file"),
					     GTK_WINDOW(widgets.app),
					     GTK_FILE_CHOOSER_ACTION_OPEN,
#if GTK_CHECK_VERSION(3, 10, 0)
					     "_Cancel",
					     GTK_RESPONSE_CANCEL, "_OK",
					     GTK_RESPONSE_ACCEPT,
#else
					     GTK_STOCK_CANCEL,
					     GTK_RESPONSE_CANCEL,
					     GTK_STOCK_OK,
					     GTK_RESPONSE_ACCEPT,
#endif
					     NULL);
	fname =
	    g_strdup_printf("%s/%s", settings.homedir,
			    ".bibletime/bookmarks.xml");
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), fname);
	g_free(fname);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		gtk_tree_store_append(GTK_TREE_STORE(model), &iter,
				      &parent);
		gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
				   COL_OPEN_PIXBUF,
				   bm_pixbufs->pixbuf_opened,
				   COL_CLOSED_PIXBUF,
				   bm_pixbufs->pixbuf_closed, COL_CAPTION,
				   "Imported", COL_KEY, NULL, COL_MODULE,
				   NULL, -1);

		fname =
		    gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		gui_parse_bookmarks(bookmark_tree, (const xmlChar *)fname,
				    &iter);
		g_free(fname);
	}
	gtk_widget_destroy(dialog);
}

/******************************************************************************
 * Name
 *   on_allow_reordering_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_allow_reordering_activate(GtkMenuItem * menuitem,
 *				  gpointer user_data)
 *
 * Description
 *   allow reordering of bookmarks
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_allow_reordering_activate(GtkMenuItem *menuitem,
						  gpointer user_data)
{
	gtk_tree_view_set_reorderable(bookmark_tree,
				      gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem)));
}

/******************************************************************************
 * Name
 *   on_dialog_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_dialog_activate(GtkMenuItem * menuitem,
 *				  gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_dialog_activate(GtkMenuItem *menuitem,
					gpointer user_data)
{
	GtkTreeIter selected;
	gchar *key = NULL;
	gchar *module = NULL;

	GtkTreeSelection *selection =
	    gtk_tree_view_get_selection(bookmark_tree);

	use_dialog = TRUE;
	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model),
				   &selected, 3, &key, 4, &module, -1);

		if (module && (main_get_mod_type(module) == PERCOM_TYPE)) {
			editor_create_new(module, key, TRUE);
			use_dialog = FALSE;
			g_free(module);
			g_free(key);
			return;
		}

		gchar *url =
		    g_strdup_printf("passagestudy.jsp?action=showBookmark&"
				    "type=%s&value=%s&module=%s",
				    "newDialog", main_url_encode(key),
				    main_url_encode(module));
		main_url_handler(url, TRUE);
		g_free(url);
	}
	use_dialog = FALSE;
	g_free(module);
	g_free(key);
}

/******************************************************************************
 * Name
 *   on_edit_item_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_edit_item_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   edit bookmark
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_edit_item_activate(GtkMenuItem *menuitem,
					   gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	gchar *caption = NULL, *key = NULL, *module = NULL;
	gchar *mod_desc = NULL, *description = NULL, *current_color = NULL;

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
			   COL_CAPTION, &caption, COL_KEY, &key,
			   COL_MODULE, &module, COL_MODULE_DESC, &mod_desc,
			   COL_DESCRIPTION, &description,
			   COL_COLOR, &current_color, -1);

if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)) {
		/* --- Folder: use the dedicated folder dialog --- */
		gchar *glade_file = gui_general_user_file("folder" UI_SUFFIX, TRUE);
		if (!glade_file) goto cleanup;
#ifdef USE_GTKBUILDER
		GtkBuilder *gxml = gtk_builder_new();
		gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
		GladeXML *gxml = glade_xml_new(glade_file, NULL, NULL);
#endif
		GtkWidget *dialog  = GTK_WIDGET(UI_GET_ITEM(gxml, "dialog_folder"));
		GtkWidget *entry   = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_entry_name"));
		GtkWidget *colorbtn = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_color_button"));
		GtkWidget *clearbtn = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_clear_color"));

#ifdef USE_GTKBUILDER
		if (!dialog || !entry || !colorbtn || !clearbtn) {
			g_object_unref(gxml); goto cleanup;
		}
#else
		if (!dialog || !entry || !colorbtn || !clearbtn) goto cleanup;
		
		GtkWidget *btn_ok = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_ok"));
		GtkWidget *btn_cancel = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_cancel"));
		if (btn_ok) gtk_widget_destroy(btn_ok);
		if (btn_cancel) gtk_widget_destroy(btn_cancel);
		
		gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
		gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
		gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
#endif
		gtk_window_set_title(GTK_WINDOW(dialog), _("Edit Tag"));
		gtk_entry_set_text(GTK_ENTRY(entry), caption ? caption : "");

		if (current_color && *current_color) {
#if defined(USE_GTKBUILDER) && GTK_CHECK_VERSION(3, 4, 0)
			GdkRGBA rgba;
			if (gdk_rgba_parse(&rgba, current_color))
				gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(colorbtn), &rgba);
#else
			GdkColor gdk_color;
			if (gdk_color_parse(current_color, &gdk_color))
				gtk_color_button_set_color(GTK_COLOR_BUTTON(colorbtn), &gdk_color);
#endif
		}

		g_signal_connect_swapped(clearbtn, "clicked",
			G_CALLBACK(gtk_widget_set_sensitive), colorbtn);

		if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
			const gchar *name = gtk_entry_get_text(GTK_ENTRY(entry));
			gchar *new_color = NULL;

			if (gtk_widget_is_sensitive(colorbtn)) {
#if defined(USE_GTKBUILDER) && GTK_CHECK_VERSION(3, 4, 0)
				GdkRGBA rgba;
				gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colorbtn), &rgba);
				if (rgba.red < 0.99 || rgba.green < 0.99 || rgba.blue < 0.99)
					new_color = g_strdup_printf("#%02X%02X%02X",
						(guint)(rgba.red * 255),
						(guint)(rgba.green * 255),
						(guint)(rgba.blue * 255));
#else
				GdkColor gdk_color;
				gtk_color_button_get_color(GTK_COLOR_BUTTON(colorbtn), &gdk_color);
				if (gdk_color.red < 65000 || gdk_color.green < 65000 || gdk_color.blue < 65000)
					new_color = g_strdup_printf("#%02X%02X%02X",
						gdk_color.red >> 8,
						gdk_color.green >> 8,
						gdk_color.blue >> 8);
#endif
			}

			gchar *new_caption = g_strdelimit(g_strdup(name), "/|><.'`\"", ' ');
			bookmarks_changed = TRUE;
			gtk_tree_store_set(GTK_TREE_STORE(model), &selected,
					   COL_CAPTION, new_caption,
					   COL_COLOR, new_color, -1);
			gui_save_bookmarks(NULL, NULL);
			main_display_bible(NULL, settings.currentverse);
			g_free(new_caption);
			g_free(new_color);
		}
		gtk_widget_destroy(dialog);
#ifdef USE_GTKBUILDER
		g_object_unref(gxml);
#endif
	} else {
		/* --- Leaf bookmark: generic dialog --- */
		GS_DIALOG *info = gui_new_dialog();
		GString *str = g_string_new(NULL);
		g_string_printf(str, "<span weight=\"bold\">%s</span>", _("Edit"));
		info->title     = _("Bookmark");
		info->label_top = str->str;
		info->label1    = _("Bookmark name: ");
		info->text1     = g_strdup(caption);
		info->text2     = g_strdup(key);
		info->text3     = g_strdup(module);
		info->label2    = _("Verse: ");
		info->label3    = _("Module: ");
		info->ok        = TRUE;
		info->cancel    = TRUE;
		if (gui_gs_dialog(info) == GS_OK) {
			BOOKMARK_DATA *data = g_new0(BOOKMARK_DATA, 1);
			data->caption     = info->text1;
			data->key         = info->text2;
			data->module      = info->text3;
			data->module_desc = g_strdup(main_get_module_description(info->text3));
			data->description = ((description && strlen(description) > 1) ||
					    (caption && strcmp(caption, info->text1)))
					   ? info->text1 : NULL;
			data->is_leaf     = TRUE;
			data->opened      = bm_pixbufs->pixbuf_helpdoc;
			gtk_tree_store_set(GTK_TREE_STORE(model), &selected,
					   COL_OPEN_PIXBUF,   data->opened,
					   COL_CLOSED_PIXBUF, data->closed,
					   COL_CAPTION,       data->caption,
					   COL_KEY,           data->key,
					   COL_MODULE,        data->module,
					   COL_MODULE_DESC,   data->module_desc,
					   COL_DESCRIPTION,   data->description, -1);
			bookmarks_changed = TRUE;
			gui_save_bookmarks(NULL, NULL);
			g_free(data->module_desc);
			g_free(data);
		}
		g_free(info->text1);
		if (info->text2) g_free(info->text2);
		if (info->text3) g_free(info->text3);
		g_free(info);
		g_string_free(str, TRUE);
	}
cleanup:
	g_free(caption); g_free(key); g_free(module);
	g_free(mod_desc); g_free(description); g_free(current_color);
}

//dialog_export_bookmarks_response_cb
//dialog_export_bookmarks_close_cb
/******************************************************************************
 * Name
 *   on_remove_folder_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_remove_folder_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   remove folder - and save it
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_export_folder_activate(GtkMenuItem *menuitem,
					       gpointer user_data)
{
	gui_export_bookmarks_dialog(BOOKMARKS_EXPORT, NULL);
}

/******************************************************************************
 * Name
 *   on_delete_item_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_delete_item_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   delete bookmark - if a group delete all in the group
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_delete_item_activate(GtkMenuItem *menuitem,
					     gpointer user_data)
{
	gchar *name_string;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	//      GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *str;

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
			   2, &caption, 3, &key, 4, &module, -1);
	name_string = caption;

	if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)) {
		str =
		    g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s %s",
				    _("Remove the selected folder"), name_string,
				    _("(and all its contents)?"));
	} else {
		str =
		    g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s",
				    _("Remove the selected bookmark"), name_string);
	}

	if (gui_yes_no_dialog(str,
#if GTK_CHECK_VERSION(3, 10, 0)
			      "dialog-warning")) {
#else
			      GTK_STOCK_DIALOG_WARNING)) {
#endif
		gtk_tree_store_remove(GTK_TREE_STORE(model), &selected);
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
	}
	g_free(caption);
	g_free(key);
	g_free(module);
	g_free(str);
}

/******************************************************************************
 * Name
 *   gui_save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void gui_save_bookmarks(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *   save bookmark tree
 *
 * Return value
 *   void
 */

void gui_save_bookmarks(GtkMenuItem *menuitem, gpointer user_data)
{

	GtkTreeIter root;
	GtkTreeIter first_child;
	gchar buf[256];

	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &root))
		return;
	if (!gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &first_child, &root))
		return;

	sprintf(buf, "%s/bookmarks/bookmarks.xml", settings.gSwordDir);
	save_treeview_to_xml_bookmarks(&first_child, g_strdup(buf));
}

/******************************************************************************
 * Name
 *   gui_save_bookmarks
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void gui_save_bookmarks(void)
 *
 * Description
 *   save bookmark tree
 *
 * Return value
 *   void
 */

void gui_save_bookmarks_treeview(void)
{
	gui_save_bookmarks(NULL, NULL);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *
 *
 * Description
 *   expand the bookmark tree
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_expand_activate(GtkMenuItem *menuitem,
					gpointer user_data)
{
	gtk_tree_view_expand_all(bookmark_tree);
}

/******************************************************************************
 * Name
 *  on_collapse_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_collapse_activate(GtkMenuItem * menuitem,
 *				 gpointer user_data)
 *
 * Description
 *   collapse the bookmark tree
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_collapse_activate(GtkMenuItem *menuitem,
					  gpointer user_data)
{
	gtk_tree_view_collapse_all(bookmark_tree);
}

/******************************************************************************
 * Name
 *  on_add_bookmark_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_add_bookmark_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)
 *
 * Description
 *   add bookmark (current mod and key of active window (text, comm, or dict)
 *   to root node chosen by user
 *
 * Return value
 *   void
 */

void on_add_bookmark_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
	//      gchar *caption = NULL;
	gchar *key = NULL;
	gchar *mod_name = NULL;
	gint test;
	BOOKMARK_DATA *data;
	GS_DIALOG *info;
	gchar buf[256];
	GString *str = g_string_new(NULL);

	if (!gtk_tree_selection_get_selected(current_selection, NULL, &selected))
		return;

	mod_name = main_get_active_pane_module();
	key = main_get_active_pane_key();
	data = g_new(BOOKMARK_DATA, 1);
	info = gui_new_dialog();
	info->title = N_("Bookmark");
	g_string_printf(str, "<span weight=\"bold\">%s</span>", _("Add"));
	info->label_top = str->str;
	sprintf(buf, "%s, %s", key, mod_name);
	info->text1 = g_strdup(buf);
	info->text2 = g_strdup(key);
	info->text3 = g_strdup(mod_name);
	info->label1 = N_("Label: ");
	info->label2 = N_("Verse: ");
	info->label3 = N_("Module: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		data->caption = info->text1;
		data->key = info->text2;
		data->module = info->text3;
		data->module_desc =
		    g_strdup(main_get_module_description(info->text3));
		if (!strcmp(data->caption, buf))
			data->description = NULL;
		else
			data->description = info->text1;
		data->color = NULL;  /* bookmark leaves never have a color */
		data->is_leaf = TRUE;
		data->opened = bm_pixbufs->pixbuf_helpdoc;
		data->closed = NULL;
		add_item_to_tree(&iter, &selected, data);
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
	}
	g_free(info->text1); /* we used g_strdup() */
	g_free(info->text2);
	g_free(info->text3);
	g_free(info);
	g_free(data);
	g_string_free(str, TRUE);
}

/******************************************************************************
 * Name
 *  on_insert_bookmark_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_insert_bookmark_activate(GtkMenuItem * menuitem,
 *						gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_insert_bookmark_activate(GtkMenuItem *menuitem,
						 gpointer user_data)
{
	on_add_bookmark_activate(menuitem, NULL);
}

/******************************************************************************
 * Name
 *   on_new_subgroup_activate
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void on_new_subgroup_activate(GtkMenuItem * menuitem,
 *			      gpointer user_data)
 *
 * Description
 *   add new sub group to selected group
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_new_folder_activate(GtkMenuItem *menuitem,
					    gpointer user_data)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
	BOOKMARK_DATA *data;

	if (!gtk_tree_selection_get_selected(current_selection, NULL, &selected))
		return;

	gchar *glade_file = gui_general_user_file("folder" UI_SUFFIX, TRUE);
	g_return_if_fail(glade_file != NULL);
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	GladeXML *gxml = glade_xml_new(glade_file, NULL, NULL);
#endif
	g_free(glade_file);

	GtkWidget *dialog     = GTK_WIDGET(UI_GET_ITEM(gxml, "dialog_folder"));
	GtkWidget *entry      = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_entry_name"));
	GtkWidget *colorbtn   = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_color_button"));
	GtkWidget *clearbtn   = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_clear_color"));

#ifdef USE_GTKBUILDER
	if (!dialog || !entry || !colorbtn || !clearbtn) {
		g_printerr("ERROR: dialog_folder widgets not found\n");
		g_object_unref(gxml);
		return;
	}
#else
	if (!dialog || !entry || !colorbtn || !clearbtn) return;
	
	GtkWidget *btn_ok = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_ok"));
	GtkWidget *btn_cancel = GTK_WIDGET(UI_GET_ITEM(gxml, "folder_cancel"));
	if (btn_ok) gtk_widget_destroy(btn_ok);
	if (btn_cancel) gtk_widget_destroy(btn_cancel);
	
	gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
	gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
#endif

	gtk_window_set_title(GTK_WINDOW(dialog), _("New Folder"));
	gtk_entry_set_text(GTK_ENTRY(entry), "");

	g_signal_connect_swapped(clearbtn, "clicked",
		G_CALLBACK(gtk_widget_set_sensitive), colorbtn);

	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	if (response == GTK_RESPONSE_OK) {
		const gchar *name = gtk_entry_get_text(GTK_ENTRY(entry));
		gchar *color = NULL;

		if (gtk_widget_is_sensitive(colorbtn)) {
#if defined(USE_GTKBUILDER) && GTK_CHECK_VERSION(3, 4, 0)
			GdkRGBA rgba;
			gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colorbtn), &rgba);
			if (rgba.red < 0.99 || rgba.green < 0.99 || rgba.blue < 0.99)
				color = g_strdup_printf("#%02X%02X%02X",
					(guint)(rgba.red   * 255),
					(guint)(rgba.green * 255),
					(guint)(rgba.blue  * 255));
#else
			GdkColor gdk_color;
			gtk_color_button_get_color(GTK_COLOR_BUTTON(colorbtn), &gdk_color);
			if (gdk_color.red < 65000 || gdk_color.green < 65000 || gdk_color.blue < 65000)
				color = g_strdup_printf("#%02X%02X%02X",
					gdk_color.red >> 8,
					gdk_color.green >> 8,
					gdk_color.blue >> 8);
#endif
		}

		data = g_new0(BOOKMARK_DATA, 1);
		data->caption = g_strdelimit(g_strdup(name), "/|><.'`\"", ' ');
		data->color   = color;
		data->is_leaf = FALSE;
		data->opened  = bm_pixbufs->pixbuf_opened;
		data->closed  = bm_pixbufs->pixbuf_closed;
		bookmarks_changed = TRUE;
		add_item_to_tree(&iter, &selected, data);
		gui_save_bookmarks(NULL, NULL);
		g_free(data->caption);
		g_free(data->color);
		g_free(data);
	}
	gtk_widget_destroy(dialog);
#ifdef USE_GTKBUILDER
	g_object_unref(gxml);
#endif
}

/******************************************************************************
 * Name
 *   on_open_in_tab_activate
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void on_open_in_tab_activate(GtkMenuItem * menuitem, gpointer user_data)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

G_MODULE_EXPORT void on_open_in_tab_activate(GtkMenuItem *menuitem,
					     gpointer user_data)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	//      GtkTreeIter iter;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *url = NULL;

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
			   3, &key, 4, &module, -1);
	url = g_strdup_printf("passagestudy.jsp?action=showBookmark&"
			      "type=%s&value=%s&module=%s",
			      "newTab",
			      main_url_encode(key),
			      main_url_encode(module));
	main_url_handler(url, TRUE);
	g_free(key);
	g_free(module);
	g_free(url);
}

/******************************************************************************
 * Name
 *   create_bookmark_menu
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void create_bookmark_menu(void)
 *
 * Description
 *   create bookmark tree popup menu
 *  !! CHANGING MENU STRUCTURE DON'T FORGET ALSO CHANGE create_bookmark_menu !!
 *
 * Return value
 *   void
 */

#if GTK_CHECK_VERSION(3, 4, 0)
G_MODULE_EXPORT void on_set_tag_color_activate(GtkMenuItem *menuitem,
                                               gpointer user_data)
{
	GtkTreeIter selected;
	gchar *color = NULL;
	GtkTreeSelection *selection = gtk_tree_view_get_selection(bookmark_tree);

	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	/* Only folders get a color */
	if (!gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected))
		return;

	GtkWidget *dialog = gtk_color_chooser_dialog_new(
		_("Choose folder color"), GTK_WINDOW(widgets.app));

	/* Pre-load existing color if any */
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
			   COL_COLOR, &color, -1);
	if (color && *color) {
		GdkRGBA rgba;
		if (gdk_rgba_parse(&rgba, color))
			gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(dialog), &rgba);
	}
	g_free(color);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
		GdkRGBA rgba;
		gchar *hex;
		gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(dialog), &rgba);
		hex = g_strdup_printf("#%02X%02X%02X",
			(guint)(rgba.red   * 255),
			(guint)(rgba.green * 255),
			(guint)(rgba.blue  * 255));
		bookmarks_changed = TRUE;
		gtk_tree_store_set(GTK_TREE_STORE(model), &selected,
				   COL_COLOR, hex, -1);
		g_free(hex);
		gui_save_bookmarks(NULL, NULL);
		main_display_bible(NULL, settings.currentverse);
	}
	gtk_widget_destroy(dialog);
}
#endif

void gui_create_bookmark_menu(void)
{
	gchar *glade_file;
#ifdef USE_GTKBUILDER
	GtkBuilder *gxml;
	glade_file = gui_general_user_file("xi-menus-popup.gtkbuilder", FALSE);
#else
	GladeXML *gxml;
	glade_file = gui_general_user_file("xi-menus.glade", FALSE);
#endif
	g_return_if_fail((glade_file != NULL));

#ifdef USE_GTKBUILDER
	gxml = gtk_builder_new();
	gtk_builder_add_from_file(gxml, glade_file, NULL);
#else
	gxml = glade_xml_new(glade_file, "menu_bookmark", NULL);
#endif
	g_free(glade_file);
	g_return_if_fail((gxml != NULL));

	menu.menu = UI_GET_ITEM(gxml, "menu_bookmark");
	menu.in_tab = UI_GET_ITEM(gxml, "open_in_new_tab");
	menu.in_dialog = UI_GET_ITEM(gxml, "open_in_a_dialog");
	menu.new = UI_GET_ITEM(gxml, "new_folder");
	menu.insert = UI_GET_ITEM(gxml, "insert_bookmark");
	menu.edit = UI_GET_ITEM(gxml, "edit_item");
	menu.delete = UI_GET_ITEM(gxml, "delete_item");
	menu.reorder = UI_GET_ITEM(gxml, "allow_reordering");
	menu.bibletime = UI_GET_ITEM(gxml, "import_bibletime_bookmarks1");
	menu.remove      = UI_GET_ITEM(gxml, "remove_folder");
	menu.set_color   = UI_GET_ITEM(gxml, "set_tag_color");

	gtk_widget_set_sensitive(menu.in_tab, FALSE);
	gtk_widget_set_sensitive(menu.in_dialog, FALSE);
	gtk_widget_set_sensitive(menu.new, FALSE);
	gtk_widget_set_sensitive(menu.insert, FALSE);
	gtk_widget_set_sensitive(menu.edit, FALSE);
	gtk_widget_set_sensitive(menu.delete, FALSE);
	gtk_widget_set_sensitive(menu.bibletime, TRUE);

	gtk_widget_set_sensitive(menu.remove, TRUE);
	gtk_widget_hide(menu.remove);
/* connect signals and data */
#ifdef USE_GTKBUILDER
	gtk_builder_connect_signals(gxml, NULL);
/*gtk_builder_connect_signals_full
	   (gxml, (GtkBuilderConnectFunc)gui_glade_signal_connect_func, NULL); */
#else
	glade_xml_signal_autoconnect_full(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func,
					  NULL);
#endif
}
