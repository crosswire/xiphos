/*
 * Xiphos Bible Study Tool
 * treekey-editor.c - functions to manage a treeview of keys
 *
 * Copyright (C) 2007-2017 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef USE_GTKBUILDER
#include <glade/glade-xml.h>
#endif

#ifdef USE_WEBKIT_EDITOR
#include "editor/webkit_editor.h"
#else
#include "editor/slib-editor.h"
#endif

#include "gui/treekey-editor.h"
#include "gui/dialog.h"
#include "gui/utilities.h"

#include "main/sidebar.h"
#include "main/sword_treekey.h"
#include "main/sword.h"

#include "gui/debug_glib_null.h"

void on_add_sibling_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_add_child_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_remove_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_edit_activate2(GtkMenuItem *menuitem, gpointer user_data);

typedef struct _item_info INFO;
struct _item_info
{
	gchar *book;
	gchar *local_name; /* tree node name */
	gchar *offset;

	GtkTreeIter iter;
	GtkTreeModel *model;
};

INFO *_get_info(GtkWidget *tree);

enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_CAPTION,
	COL_MODULE,
	COL_OFFSET,
	N_COLUMNS
};

static GtkWidget *menu;

INFO *_get_info(GtkWidget *tree)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeView *t = GTK_TREE_VIEW(tree);

	INFO *info = g_new0(INFO, 1);

	selection = gtk_tree_view_get_selection(t);
	if (gtk_tree_selection_get_selected(selection, &info->model, &info->iter))
		gtk_tree_model_get(GTK_TREE_MODEL(info->model),
				   &info->iter, 2, &info->local_name, 3,
				   &info->book, 4, &info->offset, -1);
	return info;
}

static void _button_one(EDITOR *e)
{
	INFO *info;

	editor_save_book(e);

	info = _get_info(e->treeview);

	if (atol(info->offset) == 0)
		gtk_widget_set_sensitive(e->html_widget, FALSE);
	else
		gtk_widget_set_sensitive(e->html_widget, TRUE);

	if (e->module)
		g_free(e->module);
	e->module = g_strdup(info->book);

	if (e->key)
		g_free(e->key);
	e->key = g_strdup(info->offset);
	editor_load_book(e);

	g_free(info->book);
	g_free(info->local_name);
	g_free(info->offset);
	g_free(info);
}

G_MODULE_EXPORT void
on_add_sibling_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	INFO *info;
	EDITOR *e = (EDITOR *)user_data;
	GtkWidget *tree = GTK_WIDGET(e->treeview);
	gint test;
	GS_DIALOG *d;
	GtkTreeIter sibling;

	info = _get_info(tree);

	d = gui_new_dialog();
#if GTK_CHECK_VERSION(3, 10, 0)
	d->stock_icon = "dialog-question";
#else
	d->stock_icon = GTK_STOCK_DIALOG_QUESTION;
#endif
	d->title = _("Prayer List/Journal Item");
	d->label_top = _("New name");
	d->label1 = _("Name: ");
	d->text1 = g_strdup(info->local_name);
	d->ok = TRUE;
	d->cancel = TRUE;

	test = gui_gs_dialog(d);
	if (test == GS_OK) {
		unsigned long l_offset = main_treekey_append_sibling(info->book,
								     d->text1,
								     info->offset);
		if (l_offset) {
			char *buf = g_strdup_printf("%ld", l_offset);
			gtk_tree_store_insert_after(GTK_TREE_STORE(info->model),
						    &sibling, NULL,
						    &info->iter);
			gtk_tree_store_set(GTK_TREE_STORE(info->model),
					   &sibling, COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION, d->text1,
					   COL_MODULE, info->book,
					   COL_OFFSET, buf, -1);
			if (e->key)
				g_free(e->key);
			e->key = g_strdup(buf);
			editor_load_book(e);
			g_free(buf);
		}
	}
	g_free(info->book);
	g_free(info->local_name);
	g_free(info->offset);
	g_free(info);
	g_free(d->text1);
	g_free(d);
}

G_MODULE_EXPORT void
on_add_child_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	INFO *info;
	EDITOR *e = (EDITOR *)user_data;
	GtkWidget *tree = GTK_WIDGET(e->treeview);
	gint test;
	GS_DIALOG *d;
	GtkTreeIter child;

	info = _get_info(tree);

	d = gui_new_dialog();
#if GTK_CHECK_VERSION(3, 10, 0)
	d->stock_icon = "dialog-question";
#else
	d->stock_icon = GTK_STOCK_DIALOG_QUESTION;
#endif
	d->title = _("Prayer List/Journal Item");
	d->label_top = _("New name");
	d->label1 = _("Name: ");
	d->text1 = g_strdup(info->local_name);
	d->ok = TRUE;
	d->cancel = TRUE;

	test = gui_gs_dialog(d);
	if (test == GS_OK) {
		unsigned long l_offset = main_treekey_append_child(info->book,
								   d->text1,
								   info->offset);
		if (l_offset) {
			char *buf = NULL;

			gtk_tree_store_set(GTK_TREE_STORE(info->model), /* change treenode pixbuf from leaf to branch */
					   &info->iter,
					   COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_closed,
					   COL_CLOSED_PIXBUF,
					   pixbufs->pixbuf_closed, -1);
			buf = g_strdup_printf("%ld", l_offset);
			gtk_tree_store_append(GTK_TREE_STORE(info->model),
					      &child, &info->iter);
			gtk_tree_store_set(GTK_TREE_STORE(info->model),
					   &child,
					   COL_OPEN_PIXBUF,
					   pixbufs->pixbuf_helpdoc,
					   COL_CLOSED_PIXBUF, NULL,
					   COL_CAPTION, d->text1,
					   COL_MODULE, info->book,
					   COL_OFFSET, buf, -1);
			if (e->key)
				g_free(e->key);
			e->key = g_strdup(buf);
			editor_load_book(e);
			g_free(buf);
		}
	}

	g_free(info->book);
	g_free(info->local_name);
	g_free(info->offset);
	g_free(info);
	g_free(d->text1);
	g_free(d);
}

G_MODULE_EXPORT void
on_remove_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	INFO *info;
	EDITOR *editor = (EDITOR *)user_data;
	GtkWidget *tree = GTK_WIDGET(editor->treeview);
	gchar *str;
	gchar *icon_name;

	info = _get_info(tree);
	str = g_strdup_printf("<span weight=\"bold\">%s</span>\n\n%s/%s",
			      _("Remove the selected item"),
			      info->book, info->local_name);
#if GTK_CHECK_VERSION(3, 10, 0)
	icon_name = g_strdup("dialog-warning");
#else
	icon_name = g_strdup(GTK_STOCK_DIALOG_WARNING);
#endif
	if (gui_yes_no_dialog(str, icon_name)) {
		gtk_tree_store_remove(GTK_TREE_STORE(info->model),
				      &info->iter);
		main_treekey_remove(info->book, info->local_name,
				    info->offset);
	}

	g_free(info->book);
	g_free(info->local_name);
	g_free(info->offset);
	g_free(info);
	g_free(str);
	g_free(icon_name);
}

G_MODULE_EXPORT void
on_edit_activate2(GtkMenuItem *menuitem, gpointer user_data)
{
	INFO *info;
	EDITOR *editor = (EDITOR *)user_data;
	GtkWidget *tree = GTK_WIDGET(editor->treeview);
	gint test;
	GS_DIALOG *d;

	info = _get_info(tree);

	d = gui_new_dialog();
#if GTK_CHECK_VERSION(3, 10, 0)
	d->stock_icon = "dialog-question";
#else
	d->stock_icon = GTK_STOCK_DIALOG_QUESTION;
#endif
	d->title = _("Prayer List/Journal Item");
	d->label_top = _("New name");
	d->label1 = _("Name: ");
	d->text1 = g_strdup(info->local_name);
	d->ok = TRUE;
	d->cancel = TRUE;

	test = gui_gs_dialog(d);
	if (test == GS_OK) {
		main_treekey_set_local_name(info->book,
					    d->text1, info->offset);
		gtk_tree_store_set(GTK_TREE_STORE(info->model),
				   &info->iter,
				   COL_CAPTION, (gchar *)d->text1, -1);
	}

	g_free(info->book);
	g_free(info->local_name);
	g_free(info->offset);
	g_free(info);
	g_free(d->text1);
	g_free(d);
}

GtkWidget *create_edit_tree_menu(EDITOR *editor)
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
	gxml = glade_xml_new(glade_file, "menu_edit_tree", NULL);
#endif

	g_free(glade_file);
	g_return_val_if_fail((gxml != NULL), NULL);

	menu = UI_GET_ITEM(gxml, "menu_edit_tree");
#ifdef USE_GTKBUILDER
	gtk_builder_connect_signals(gxml, editor);
/* gtk_builder_connect_signals_full
	   (gxml, (GtkBuilderConnectFunc)gui_glade_signal_connect_func, editor); */
#else
	/* connect signals and data */
	glade_xml_signal_autoconnect_full(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func,
					  editor);
#endif
	return menu;
}

static gboolean on_button_release(GtkWidget *widget,
				  GdkEventButton *event, EDITOR *editor)
{
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GtkTreeModel *model;
	GtkTreePath *path;
	gint depth = 0;

	switch (event->button) {
	case 1:
		_button_one(editor);
		break;

	case 2:

		break;

	case 3:
		selection =
		    gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
		gtk_tree_selection_get_selected(selection, &model,
						&selected);
		path = gtk_tree_model_get_path(model, &selected);
		depth = gtk_tree_path_get_depth(path);

		if (depth > 1)
			gtk_menu_popup(GTK_MENU(menu),
				       NULL, NULL, NULL, NULL,
				       0, gtk_get_current_event_time());

		gtk_tree_path_free(path);
		return FALSE;
	}
	return FALSE;
}

GtkWidget *gui_create_editor_tree(EDITOR *editor)
{
	GtkWidget *treeview;
	treeview = gtk_tree_view_new();
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);

	main_add_mod_tree_columns(GTK_TREE_VIEW(treeview));
	XI_message(("\ngui_create_editor_tree Mod Name:%s\n",
		    editor->module));
	main_load_book_tree_in_editor(GTK_TREE_VIEW(treeview),
				      editor->module);
	menu = create_edit_tree_menu(editor);

	g_signal_connect_after((gpointer)treeview,
			       "button_release_event",
			       G_CALLBACK(on_button_release), editor);
	return treeview;
}
