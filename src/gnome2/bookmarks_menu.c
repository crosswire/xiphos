/*
 * Xiphos Bible Study Tool
 * bookmarks_menu.c - gui for bookmarks using menu
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
#include "main/sidebar.h"
#include "main/sword.h"
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
	//      xmlAttrPtr root_attr;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;

	if (!bookmarks_changed)
		return;

	root_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)"SwordBookmarks");
		//root_attr =
		xmlNewProp(root_node, (const xmlChar *)"syntaxVersion",
			   (const xmlChar *)"1");
		xmlDocSetRootElement(root_doc, root_node);
	}

	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
				   2, &caption,
				   3, &key,
				   4, &module,
				   5, &mod_desc, 6, &description, -1);
		if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), iter)) {
			cur_node =
			    xml_add_folder_to_parent(root_node, caption);
			utilities_parse_treeview(cur_node, iter,
						 GTK_TREE_MODEL(model));
		} else
			xml_add_bookmark_to_parent(root_node,
						   description,
						   key, module, mod_desc);
		g_free(caption);
		g_free(key);
		g_free(module);
		g_free(mod_desc);
		g_free(description);
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
			   COL_DESCRIPTION, data->description, -1);
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
	GS_DIALOG *info;
	gint test;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	//      GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;
	gboolean is_leaf;
	GString *str;

	str = g_string_new(NULL);
	g_string_printf(str, "<span weight=\"bold\">%s</span>", _("Edit"));

	selection = gtk_tree_view_get_selection(bookmark_tree);
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
			   2, &caption,
			   3, &key,
			   4, &module, 5, &mod_desc, 6, &description, -1);

	info = gui_new_dialog();
	info->title = _("Bookmark");
	info->label_top = str->str;
	if (gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &selected)) {
		info->label1 = _("Folder name: ");
		is_leaf = FALSE;
	} else {
		info->label1 = _("Bookmark name: ");
		info->text2 = g_strdup(key);
		info->text3 = g_strdup(module);
		info->label2 = _("Verse: ");
		info->label3 = _("Module: ");
		is_leaf = TRUE;
	}

	info->text1 = g_strdup(caption);
	info->ok = TRUE;
	info->cancel = TRUE;

	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		BOOKMARK_DATA *data = g_new(BOOKMARK_DATA, 1);
		data->caption = info->text1;
		data->key = NULL;
		data->module = NULL;
		data->module_desc = NULL;
		data->description = NULL;
		if (is_leaf) {
			data->opened = bm_pixbufs->pixbuf_helpdoc;
			data->closed = NULL;
			data->key = info->text2;
			data->module = info->text3;
			data->module_desc =
			    g_strdup(main_get_module_description(info->text3));
			if ((strlen(description) > 1) || (strcmp(caption, info->text1))) {
				data->description = info->text1;
			} else
				data->description = NULL;
			data->is_leaf = TRUE;
		} else {
			data->opened = bm_pixbufs->pixbuf_opened;
			data->closed = bm_pixbufs->pixbuf_closed;
			data->is_leaf = FALSE;
		}

		gtk_tree_store_set(GTK_TREE_STORE(model), &selected,
				   COL_OPEN_PIXBUF, data->opened,
				   COL_CLOSED_PIXBUF, data->closed,
				   COL_CAPTION, data->caption,
				   COL_KEY, data->key,
				   COL_MODULE, data->module,
				   COL_MODULE_DESC, data->module_desc,
				   COL_DESCRIPTION, data->description, -1);
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
		g_free(data);
	}
	g_free(info->text1); // we used g_strdup()
	if (info->text2)
		g_free(info->text2);
	if (info->text3)
		g_free(info->text3);
	g_free(info);
	g_free(caption);
	g_free(key);
	g_free(module);
	g_free(mod_desc);
	g_free(description);
	g_string_free(str, TRUE);
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
	//      gchar *caption = NULL;
	//      gchar *key = NULL;
	//      gchar *module = NULL;
	char *t;
	gint test;
	GS_DIALOG *info;
	BOOKMARK_DATA *data;
	GString *str;

	if (!gtk_tree_selection_get_selected(current_selection, NULL, &selected))
		return;

	t = "/|><.'`\"";
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
		add_item_to_tree(&iter, &selected, data);
		bookmarks_changed = TRUE;
		gui_save_bookmarks(NULL, NULL);
		g_free(data->caption);
	}
	g_free(data);
	g_free(info->text1);
	g_free(info);
	g_string_free(str, TRUE);
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

void gui_create_bookmark_menu(void)
{
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
	menu.remove = UI_GET_ITEM(gxml, "remove_folder");

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
