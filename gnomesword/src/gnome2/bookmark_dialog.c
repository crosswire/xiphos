/*
 * GnomeSword Bible Study Tool
 * bookmark_dialog.c - gui to popup a dialog for adding a bookmark
 *
 * Copyright (C) 2005 GnomeSword Developer Team
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


#include "gui/bookmark_dialog.h"
#include "gui/bookmarks_menu.h"
#include "gui/bookmarks_treeview.h"
#include "gui/dialog.h"
#include "gui/utilities.h"

#include "main/sword.h"


extern GtkTreeStore *model;

static GtkWidget *treeview;
static GtkWidget *button_new_folder;
static GtkWidget *button_add_bookmark;
static GtkWidget *entry_label;
static GtkWidget *entry_key;
static GtkWidget *entry_module;



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
 
static 
void add_bookmark_button(void)
{
	GtkTreeIter selected;
	GtkTreeIter iter;
	BOOKMARK_DATA *data;
	GtkTreeSelection *selection;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	data = g_new(BOOKMARK_DATA, 1);
	data->caption = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY
						    (entry_label)));
	data->key = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY
						(entry_key)));
	data->module = g_strdup((gchar *)gtk_entry_get_text(GTK_ENTRY
						   (entry_module)));
	
	if(!strcmp(data->module,"studypad"))
		data->module_desc = "studypad";
	else
		data->module_desc =
		    	main_get_module_description(data->module);
	
	data->description = g_strdup((gchar *)gtk_entry_get_text
				     (GTK_ENTRY(entry_label)));

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
	char *t, *buf;
	gint test;
	GS_DIALOG *info;
	GString *str;
	GtkTreePath *path;
	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, NULL, &selected))
		return;

	t = "|";
	str = g_string_new("");
	info = gui_new_dialog();
	//info->stock_icon = GTK_STOCK_OPEN;
	info->title = N_("Bookmark");
	g_string_printf(str, "<span weight=\"bold\">%s</span>",
			_("Enter Folder Name"));
	info->label_top = str->str;
	info->text1 = g_strdup(_("Folder Name"));
	info->label1 = N_("Folder: ");
	info->ok = TRUE;
	info->cancel = TRUE;

	data = g_new(BOOKMARK_DATA, 1);
	/*** open dialog to get name for new folder ***/
	test = gui_gs_dialog(info);
	if (test == GS_OK) {
		buf = g_strdelimit(info->text1, t, ' ');
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
		path =
		    gtk_tree_model_get_path(GTK_TREE_MODEL(model), &iter);
		gtk_tree_view_expand_to_path(GTK_TREE_VIEW (treeview), path);
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
 
void on_dialog_response(GtkDialog * dialog, 
			gint response_id, 
			gpointer user_data)
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
 
gboolean on_treeview_button_release_event(GtkWidget       *widget,
                                 	  GdkEventButton  *event,
                                 	  gpointer        user_data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeModel *gmodel;
	GtkTreeIter selected;
	gchar *key = NULL;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	if (gtk_tree_selection_get_selected(selection, &gmodel, &selected)) {
		gtk_tree_model_get(gmodel, &selected, 3, &key, -1);
		if (!gtk_tree_model_iter_has_child(gmodel, &selected) 
				&& key != NULL) {
			gtk_widget_set_sensitive(button_new_folder, FALSE);
			gtk_widget_set_sensitive(button_add_bookmark, FALSE);
		} else {
			gtk_widget_set_sensitive(button_new_folder, TRUE);
			gtk_widget_set_sensitive(button_add_bookmark, TRUE);
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
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeview),GTK_TREE_MODEL(model));	
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
 
static void setup_entrys(gchar * label, 
			 gchar * module, 
			 gchar * key)
{
	gtk_entry_set_text(GTK_ENTRY(entry_label), label);
	gtk_entry_set_text(GTK_ENTRY(entry_key), key);
	gtk_entry_set_text(GTK_ENTRY(entry_module), module);
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
 
static void create_bookmark_dialog(gchar * label, 
				   gchar * module, 
				   gchar * key)
{
	GladeXML *gxml;
	gchar *glade_file;
	GtkWidget *dialog;
	GtkWidget *chooser;
	gint index = 0;

	glade_file = gui_general_user_file ("bookmarks.glade", TRUE);
	g_return_if_fail(glade_file != NULL);
	g_message(glade_file);
	
	/* build the widget */
	gxml = glade_xml_new (glade_file, NULL, NULL);
	g_free (glade_file);
	g_return_if_fail (gxml != NULL);

	/* lookup the root widget */
	dialog = glade_xml_get_widget (gxml, "dialog");
	g_signal_connect(dialog, "response",
			 G_CALLBACK(on_dialog_response), NULL);
	
	/* treeview */
	treeview = glade_xml_get_widget (gxml, "treeview");
	setup_treeview();
	g_signal_connect(treeview, "button-release-event",
			 G_CALLBACK(on_treeview_button_release_event), NULL);
	/* entrys */	
	entry_label = glade_xml_get_widget (gxml, "entry1");
	entry_key = glade_xml_get_widget (gxml, "entry2");
	entry_module = glade_xml_get_widget (gxml, "entry3");
	setup_entrys(label, module, key);
	/* dialog buttons */
	button_new_folder = glade_xml_get_widget (gxml, "button1");
	button_add_bookmark = glade_xml_get_widget (gxml, "button3");
	/* connect signals and data */
/*	glade_xml_signal_autoconnect_full
		(gxml, (GladeXMLConnectFunc)gui_glade_signal_connect_func, NULL);*/
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
 
void gui_bookmark_dialog(gchar * label, gchar * module_name, gchar * key)
{
	create_bookmark_dialog(label, module_name, key);
}
