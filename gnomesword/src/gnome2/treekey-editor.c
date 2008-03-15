/*
 * GnomeSword Bible Study Tool
 * prayer_list.c - 
 *
 * Copyright (C) 2007 GnomeSword Developer Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>

#include "editor/html-editor.h"

#include "gui/treekey-editor.h"
#include "gui/dialog.h"

#include "main/sidebar.h"
#include "main/sword_treekey.h"



typedef struct _item_info INFO;
struct _item_info
{
	gchar *book;
	gchar *local_name;	/* tree node name */
	gchar *offset;

	GtkTreeIter iter;
	GtkTreeModel *model;
};


enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_CAPTION,
	COL_MODULE,
	COL_OFFSET,
	N_COLUMNS
};

static GtkWidget *menu;

INFO *
_get_info (GtkWidget * tree)
{;
	GtkTreeSelection *selection = NULL;	
	GtkTreeView *t = GTK_TREE_VIEW(tree);
	
	INFO *info = g_new0 (INFO, 1);

	selection = gtk_tree_view_get_selection(t);
	if (gtk_tree_selection_get_selected(selection, &info->model, &info->iter))
		gtk_tree_model_get(GTK_TREE_MODEL(info->model), &info->iter,
				   2, &info->local_name,
				   3, &info->book,
				   4, &info->offset,
				   -1);
	return info;

}

static void _button_one(EDITOR * e)
{
	INFO *info;
	
	editor_save_book(e);
	
	info = _get_info (e->treeview);
	
	if(atol(info->offset) == 0) 
		gtk_widget_set_sensitive(e->html_widget,FALSE);
	else
		gtk_widget_set_sensitive(e->html_widget,TRUE);
		
	if(e->module)
		g_free(e->module);
	e->module = g_strdup(info->book);
		
	if(e->key)
		g_free(e->key);
	e->key = g_strdup(info->offset);
	editor_load_book(e);
	
	g_free(info->book);
	g_free(info->local_name);
	g_free(info->offset);
	g_free(info);	
}

static void
on_add_sibling_activate (GtkMenuItem * menuitem, gpointer user_data)
{	
	INFO *info;
	char *buf = NULL;
	unsigned long l_offset = 0;
	EDITOR * e = (EDITOR*) user_data;
	GtkWidget *tree = GTK_WIDGET (e->treeview);
	gint test;
	GS_DIALOG *d;
	GtkTreeIter sibling;
	
	info = _get_info (tree);	
	
	d = gui_new_dialog ();
	d->stock_icon = GTK_STOCK_DIALOG_QUESTION;
	d->title = _("Prayer List Item");
	d->label_top = _("New name");
	d->label1 = _("Name: ");
	d->text1 = g_strdup (info->local_name);
	d->ok = TRUE;
	d->cancel = TRUE;
	
	test = gui_gs_dialog (d);
	if (test == GS_OK) {
		l_offset = main_treekey_append_sibling(info->book, 
					    d->text1, 
					    info->offset);
		if(l_offset) {
			buf = g_strdup_printf("%d",l_offset);
			gtk_tree_store_insert_after(GTK_TREE_STORE(info->model), 
						&sibling, 
						NULL, 
						&info->iter);
			gtk_tree_store_set(GTK_TREE_STORE(info->model),
					&sibling,
					COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
					COL_CLOSED_PIXBUF, NULL,
					COL_CAPTION, d->text1,
					COL_MODULE, info->book,
					COL_OFFSET, buf,
					-1);
			if(e->key)
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
	g_free (d->text1);
	g_free (d);
}



static void
on_add_child_activate (GtkMenuItem * menuitem, gpointer user_data)
{	
	INFO *info;
	char *buf = NULL;
	unsigned long l_offset = 0;
	EDITOR * e = (EDITOR*) user_data;
	GtkWidget *tree = GTK_WIDGET (e->treeview);
	gint test;
	GS_DIALOG *d;
	GtkTreeIter child;
	
	info = _get_info (tree);	
	
	d = gui_new_dialog ();
	d->stock_icon = GTK_STOCK_DIALOG_QUESTION;
	d->title = _("Prayer List Item");
	d->label_top = _("New name");
	d->label1 = _("Name: ");
	d->text1 = g_strdup (info->local_name);
	d->ok = TRUE;
	d->cancel = TRUE;
	
	test = gui_gs_dialog (d);
	if (test == GS_OK) {
		l_offset = main_treekey_append_child(info->book, 
					    d->text1, 
					    info->offset);
		if(l_offset) {
			gtk_tree_store_set(GTK_TREE_STORE(info->model), /* change treenode pixbuf from leaf to branch */
					&info->iter,
			    		COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
			    		COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
					-1);
			buf = g_strdup_printf("%d",l_offset);
			gtk_tree_store_append (GTK_TREE_STORE (info->model), 
						&child, 
						&info->iter);
			gtk_tree_store_set(GTK_TREE_STORE(info->model),
					&child,
					COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
					COL_CLOSED_PIXBUF, NULL,
					COL_CAPTION, d->text1,
					COL_MODULE, info->book,
					COL_OFFSET, buf,
					-1);
			if(e->key)
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
	g_free (d->text1);
	g_free (d);
}


static void
on_remove_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	INFO *info;
	EDITOR * editor = (EDITOR*) user_data;
	GtkWidget *tree = GTK_WIDGET (editor->treeview);
	gint test;
	GS_DIALOG *d;
	GString *str = g_string_new(NULL);
	
	info = _get_info (tree);
	d = gui_new_dialog();
	d->stock_icon = GTK_STOCK_DIALOG_WARNING;
	d->title = _("Book");
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s/%s",
			_("Remove the selected item"),
			info->book, info->local_name);
	d->label_top = str->str;
	d->yes = TRUE;
	d->no = TRUE;

	test = gui_alert_dialog(d);
	if (test == GS_YES) {
		gtk_tree_store_remove(GTK_TREE_STORE(info->model), &info->iter);
		main_treekey_remove (info->book, info->local_name, info->offset);
	}
	
	g_free(info->book);
	g_free(info->local_name);
	g_free(info->offset);
	g_free(info);
	g_free (d);
	g_string_free(str, TRUE);
}


static void
on_edit_activate (GtkMenuItem * menuitem, gpointer user_data)
{
	INFO *info;
	EDITOR * editor = (EDITOR*) user_data;
	GtkWidget *tree = GTK_WIDGET (editor->treeview);
	gint test;
	GS_DIALOG *d;
	
	info = _get_info (tree);	
	
	d = gui_new_dialog ();
	d->stock_icon = GTK_STOCK_DIALOG_QUESTION;
	d->title = _("Prayer List Item");
	d->label_top = _("New name");
	d->label1 = _("Name: ");
	d->text1 = g_strdup (info->local_name);
	d->ok = TRUE;
	d->cancel = TRUE;
	
	test = gui_gs_dialog (d);
	if (test == GS_OK) {
		main_treekey_set_local_name(info->book, 
					    d->text1, 
					    info->offset);
		gtk_tree_store_set(GTK_TREE_STORE(info->model),
				&info->iter, 
				COL_CAPTION, (gchar *) d->text1, 
				-1);
	}
	
	g_free(info->book);
	g_free(info->local_name);
	g_free(info->offset);
	g_free(info);
	g_free (d->text1);
	g_free (d);
}


static GnomeUIInfo edit_tree_menu_uiinfo[] = {
	{
	 GNOME_APP_UI_ITEM, N_("Add Child"),
	 N_("Add an item"),
	 (gpointer) on_add_child_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-add",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Add Sibling"),
	 N_("Add an item"),
	 (gpointer) on_add_sibling_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-add",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Remove"),
	 N_("Remove this item"),
	 (gpointer) on_remove_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-remove",
	 0, (GdkModifierType) 0, NULL},
	{
	 GNOME_APP_UI_ITEM, N_("Edit"),
	 N_("Edit this item"),
	 (gpointer) on_edit_activate, NULL, NULL,
	 GNOME_APP_PIXMAP_STOCK, "gtk-edit",
	 0, (GdkModifierType) 0, NULL},
	GNOMEUIINFO_END
};

GtkWidget *
create_edit_tree_menu (EDITOR * editor)
{
	GtkWidget *edit_tree_menu;
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new ();

	edit_tree_menu = gtk_menu_new ();
	gnome_app_fill_menu_with_data (GTK_MENU_SHELL (edit_tree_menu),
				       edit_tree_menu_uiinfo,
				       accel_group, FALSE, 0, editor);
	// gnome_app_fill_menu (GTK_MENU_SHELL (edit_tree_menu), edit_tree_menu_uiinfo, accel_group, FALSE, 0);
	gtk_menu_set_accel_group (GTK_MENU (edit_tree_menu), accel_group);

	return edit_tree_menu;
}

static gboolean on_button_release(GtkWidget *widget,
			       GdkEventButton *event,
			       EDITOR * editor)
{
	switch (event->button) {
	case 1:
		_button_one(editor);
		break;

	case 2:
		
		break;

	case 3:
		gtk_menu_popup(GTK_MENU(menu),
				       NULL, NULL, NULL, NULL,
				       0, gtk_get_current_event_time());
		return FALSE;		
	}
	return FALSE;
}

GtkWidget *
gui_create_editor_tree (EDITOR * editor)
{
	GtkWidget *treeview;
	treeview = gtk_tree_view_new ();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);

	main_add_mod_tree_columns (GTK_TREE_VIEW (treeview));
	main_load_book_tree_in_editor (GTK_TREE_VIEW (treeview), editor->module);
	menu = create_edit_tree_menu (editor);

	g_signal_connect_after((gpointer) treeview,
			       "button_release_event",
			       G_CALLBACK
			       (on_button_release), editor);
	return treeview;
}
