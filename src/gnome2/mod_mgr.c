/*
 * GnomeSword Bible Study Tool
 * gnome2/mod_mgr.c 
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <bonobo.h>
#include <gnome.h>

#include "gui/mod_mgr.h"
#include "gui/dialog.h"

#include "main/mod_mgr.h"
#include "main/settings.h"
#include "main/sword.h"

#define GTK_RESPONSE_INSTALL 303
#define GTK_RESPONSE_REMOVE 302
#define GTK_RESPONSE_REFRESH 301
#define INSTALL 1
#define REMOVE 0

enum {
	COLUMN_NAME,
	COLUMN_FIXED,
	COLUMN_OLD_VERSION,
	COLUMN_NEW_VERSION,
	COLUMN_VISIBLE,
	NUM_COLUMNS
};


static GtkWidget *treeview;
static GtkWidget *treeview1;
static GtkWidget *treeview2;
static GtkWidget *notebook1;
static GtkWidget *button5;
static GtkWidget *button6;
static GtkWidget *button7;
static GtkWidget *label_home;
static GtkWidget *label_system;
static GtkWidget *progressbar;
static GtkWidget *radiobutton_source;
static GtkWidget *radiobutton2;
static GtkWidget *radiobutton_dest;
static GtkWidget *radiobutton4;
static GtkWidget *fileentry1;
static GtkWidget *combo_entry1;
static GtkWidget *combo_entry2;
static GtkWidget *combo1;
static GtkWidget *dialog;

static gboolean local;
static const gchar *source;
static gboolean dot_sword;
static const gchar *destination;
static gboolean have_configs;

static gboolean mod_mgr_check_for_configs(const gchar *filename)
{
	return g_file_test(filename, G_FILE_TEST_EXISTS);
}


/******************************************************************************
 * Name
 *   install_modules
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *    void install_modules(GList * modules)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void remove_install_modules(GList * modules, gboolean install)
{
	GList *tmp = NULL;
	gchar *buf;
	gint test;
	GS_DIALOG *yes_no_dialog;
	GString *mods = g_string_new(NULL);
	GString *dialog_text = g_string_new(NULL);


	tmp = modules;
	while (tmp) {
		buf = (gchar *) tmp->data;
		if (buf[0] == '*')
			++buf;
		mods = g_string_append(mods, buf);
		mods = g_string_append(mods, ", ");
		tmp = g_list_next(tmp);
	}
	mods = g_string_truncate(mods, mods->len - 2);
	yes_no_dialog = gui_new_dialog();
	yes_no_dialog->stock_icon = GTK_STOCK_DIALOG_QUESTION;
	if(install)
		g_string_printf(dialog_text,
			"<span weight=\"bold\">%s</span>\n\n%s",
			_("Install the following modules:"), mods->str);
	else
		g_string_printf(dialog_text,
			"<span weight=\"bold\">%s</span>\n\n%s",
			_("Remove the following modules:"), mods->str);

	yes_no_dialog->label_top = dialog_text->str;
	yes_no_dialog->yes = TRUE;
	yes_no_dialog->no = TRUE;

	test = gui_alert_dialog(yes_no_dialog);
	if (test != GS_YES) {
		g_list_free(tmp);
		g_free(yes_no_dialog);
		g_string_free(mods, TRUE);
		g_string_free(dialog_text, TRUE);
		return;
	}
	g_free(yes_no_dialog);
	g_string_free(dialog_text, TRUE);
	gtk_widget_queue_draw(dialog);

	while (gtk_events_pending()) {
		gtk_main_iteration();
	}  
	
	tmp = modules;
	while (tmp) {
		while (gtk_events_pending()) {
			gtk_main_iteration();
		}
		buf = (gchar *) tmp->data;
		g_string_printf(mods,"%s: %s",_("Installing: "),buf);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar),
			mods->str);
		if (buf[0] == '*') {
			++buf;
			g_print("uninstalling %s\n", buf);
			if(!mod_mgr_uninstall(buf)) {				
				mod_mgr_shut_down();
				g_warning("destination = %s",destination);
				if(destination) {
					g_warning("dest");
					mod_mgr_init(NULL);
				}
				else {
					g_warning("no dest");
					mod_mgr_init(gtk_label_get_text(
						GTK_LABEL(label_home)));
				}
				mod_mgr_uninstall(buf);			
			}
				
			while (gtk_events_pending()) {
				gtk_main_iteration();
			}
		}
		if(install) {
			g_print("installing %s\n", buf);
			if(local)
				mod_mgr_local_install_module(source, buf);
			else
				mod_mgr_remote_install(source, buf);
		}		
		g_free(tmp->data);
		tmp = g_list_next(tmp);
	}
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar),
		_("Finished"));
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(
		progressbar), 0);
	g_list_free(tmp);
	g_string_free(mods, TRUE);

}

/******************************************************************************
 * Name
 *   parse_treeview
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *    GList *parse_treeview(GtkTreeModel * model, GtkTreeIter * tree_parent)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

static GList *parse_treeview(GList * list, GtkTreeModel * model,
			     GtkTreeIter * tree_parent)
{
	//GList *retval 
	GtkTreeIter child;
	gchar *name = NULL;
	gboolean fixed;

	gtk_tree_model_iter_children(model, &child, tree_parent);

	do {
		gtk_tree_model_get(model, &child,
				   COLUMN_FIXED, &fixed, COLUMN_NAME,
				   &name, -1);
		if (gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &child)) {
			list = parse_treeview(list, model, &child);
		} else {
			if (fixed) {
				list =
				    g_list_append(list, (gchar *) name);
			}
		}

		//g_free (name);
	}
	while (gtk_tree_model_iter_next(model, &child));
	return list;
}


/******************************************************************************
 * Name
 *   get_list_mods_to_install
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void get_list_mods_to_install(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static GList *get_list_mods_to_remove_install(gboolean install)
{
	GList *retval = NULL;
	GtkTreeIter root;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *name;
	gboolean fixed;
	if(install)
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	else
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview2));
	if (!gtk_tree_model_get_iter_first
	    (GTK_TREE_MODEL(model), &root))
		return retval;
	if (!gtk_tree_model_iter_has_child(model, &root))
		return retval;

	do {
		gtk_tree_model_get(model, &root,
				   COLUMN_FIXED, &fixed, COLUMN_NAME,
				   &name, -1);
		if (gtk_tree_model_iter_has_child(model, &root)) {
			retval = parse_treeview(retval, model, &root);
		} else {
			if (fixed) {
				retval = g_list_append(retval,
						       (gchar *) name);
			}

		}
		//g_free (name);
	}
	while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &root));
	return retval;
}


/******************************************************************************
 * Name
 *   add_module_to_language_folder
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void add_module_to_language_folder(GtkTreeModel * model,
 *		      GtkTreeIter iter, gchar * language, gchar * module_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_module_to_language_folder(GtkTreeModel * model,
					  GtkTreeIter iter,
					  MOD_MGR * info)
{
	GtkTreeIter iter_iter;
	GtkTreeIter parent;
	GtkTreeIter child_iter;
	gboolean valid;
	GString *str = g_string_new(NULL);

	if ((!g_ascii_isalnum(info->language[0]))
	    || (info->language == NULL))
		info->language = N_("Unknown");

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		gtk_tree_model_get(model, &iter_iter, COLUMN_NAME,
				   &str_data, -1);
		if (!strcmp(info->language, str_data)) {
			if (info->installed)
				g_string_printf(str, "*%s", info->name);
			else
				g_string_printf(str, "%s", info->name);

			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter_iter);
			gtk_tree_store_set(GTK_TREE_STORE(model),
					   &child_iter, COLUMN_FIXED,
					   FALSE, COLUMN_NAME, str->str,
					   COLUMN_OLD_VERSION,
					   info->old_version,
					   COLUMN_NEW_VERSION,
					   info->new_version,
					   COLUMN_VISIBLE, TRUE, -1);

			g_free(str_data);
			return;
		}
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
}

/******************************************************************************
 * Name
 *   add_language_folder
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void add_language_folder(GtkTreeModel * model, GtkTreeIter iter,
 *			 gchar * language)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_language_folder(GtkTreeModel * model, GtkTreeIter iter,
				const gchar * language)
{
	GtkTreeIter iter_iter;
	GtkTreeIter parent;
	GtkTreeIter child_iter;
	gchar *buf;
	gsize bytes_read;
	gsize bytes_written;
	GError **error;
	gboolean valid;

	if ((!g_ascii_isalnum(language[0])) || (language == NULL))
		language = N_("Unknown");

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		gtk_tree_model_get(model, &iter_iter, COLUMN_NAME,
				   &str_data, -1);
		if (!strcmp(language, str_data)) {
			g_free(str_data);
			return;
		}
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
	gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter,
			      &iter);
	buf =
	    g_convert(language, -1, UTF_8, OLD_CODESET, &bytes_read,
		      &bytes_written, error);
	gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
			   COLUMN_VISIBLE, FALSE, COLUMN_NAME,
			   (gchar *) buf, -1);

}

/******************************************************************************
 * Name
 *   load_module_tree
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void load_module_tree(GtkWidget * tree)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void load_module_tree(GtkTreeView * treeview, gboolean install)
{
	gint i;
	static gboolean need_column = TRUE;
	GtkTreeStore *store;
	GtkTreeIter iter;
	GtkTreeIter text;
	GtkTreeIter commentary;
	GtkTreeIter dictoinary;
	GtkTreeIter book;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeIter child_iter;
	GList *tmp = NULL;
	GList *tmp2 = NULL;
	MOD_MGR *info;
	//GString *str = g_string_new(NULL);
	mod_mgr_shut_down();
	
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
	
	mod_mgr_init(destination);
	
	if(install) {
		if (GTK_TOGGLE_BUTTON(radiobutton_source)->active) {
			source = gtk_entry_get_text(GTK_ENTRY(combo_entry1));
			local = TRUE;
		} else {
			source = gtk_entry_get_text(GTK_ENTRY(combo_entry2));
			local = FALSE;
		}
		if (local)
			tmp = mod_mgr_list_local_modules(source);
		else
			tmp = mod_mgr_remote_list_modules(source);
	} else {
		
		tmp = mod_mgr_list_local_modules(get_path_to_mods());
	}

	store = GTK_TREE_STORE(gtk_tree_view_get_model(treeview));
	gtk_tree_store_clear(store);
	if(!g_list_length(tmp))
		return;
	tmp2 = tmp;

	/*  add lBiblical Texts folder */
	gtk_tree_store_append(store, &text, NULL);
	gtk_tree_store_set(store, &text, 0, _("Biblical Texts"), -1);

	/*  add Commentaries folder */
	gtk_tree_store_append(store, &commentary, NULL);
	gtk_tree_store_set(store, &commentary, 0, _("Commentaries"),
			   -1);

	/*  add Dictionaries folder */
	gtk_tree_store_append(store, &dictoinary, NULL);
	gtk_tree_store_set(store, &dictoinary, 0, _("Dictionaries"),
			   -1);

	/*  add Books folder */
	gtk_tree_store_append(store, &book, NULL);
	gtk_tree_store_set(store, &book, 0, _("General Books"), -1);



	while (tmp2) {
		info = (MOD_MGR *) tmp2->data;
		if (!strcmp(info->type, TEXT_MODS)) {
			add_language_folder(GTK_TREE_MODEL(store), text,
					    info->language);
		}
		if (!strcmp(info->type, COMM_MODS)) {
			add_language_folder(GTK_TREE_MODEL(store),
					    commentary, info->language);
		}
		if (!strcmp(info->type, DICT_MODS)) {
			add_language_folder(GTK_TREE_MODEL(store),
					    dictoinary, info->language);
		}
		if (!strcmp(info->type, BOOK_MODS)) {
			add_language_folder(GTK_TREE_MODEL(store), book,
					    info->language);
		}
		tmp2 = g_list_next(tmp2);
	}

	while (tmp) {
		info = (MOD_MGR *) tmp->data;
		if (!strcmp(info->type, TEXT_MODS)) {
			add_module_to_language_folder(GTK_TREE_MODEL
						      (store), text,
						      info);
		}
		if (!strcmp(info->type, COMM_MODS)) {
			add_module_to_language_folder(GTK_TREE_MODEL
						      (store),
						      commentary, info);
		}
		if (!strcmp(info->type, DICT_MODS)) {
			add_module_to_language_folder(GTK_TREE_MODEL
						      (store),
						      dictoinary, info);
		}
		if (!strcmp(info->type, BOOK_MODS)) {
			add_module_to_language_folder(GTK_TREE_MODEL
						      (store), book,
						      info);
		}
		g_free(info->name);
		g_free(info->type);
		g_free(info->new_version);
		g_free(info);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	//g_string_free(str,TRUE);
}


/******************************************************************************
 * Name
 *   on_dialog_response
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_dialog_response(GtkDialog * dialog, gint response_id,
			       gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void
on_dialog_response(GtkDialog * dialog, gint response_id, gpointer data)
{
	GList *modules = NULL;

	switch (response_id) {
	case GTK_RESPONSE_REFRESH:
		mod_mgr_refresh_remote_source("crosswire");
		load_module_tree(GTK_TREE_VIEW(treeview), TRUE);
		break;
	case GTK_RESPONSE_CLOSE:
		gtk_widget_destroy(GTK_WIDGET(dialog));
		mod_mgr_shut_down();
		break;
	case GTK_RESPONSE_INSTALL:
		modules = get_list_mods_to_remove_install(INSTALL);
		remove_install_modules(modules, INSTALL);
		load_module_tree(GTK_TREE_VIEW(treeview), TRUE);
		break;
	case GTK_RESPONSE_REMOVE:
		modules = get_list_mods_to_remove_install(REMOVE);
		remove_install_modules(modules, REMOVE);
		load_module_tree(GTK_TREE_VIEW(treeview2), FALSE);
		break;
	}
}



/******************************************************************************
 * Name
 *   fixed_toggled
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void fixed_toggled (GtkCellRendererToggle *cell, gchar *path_str,
 *							gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void
fixed_toggled(GtkCellRendererToggle * cell, gchar * path_str,
	      gpointer data)
{
	GtkTreeModel *model = (GtkTreeModel *) data;
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
	gboolean fixed;
	gchar *name;
	gchar *buf;

	/* get toggled iter */
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter, COLUMN_FIXED, &fixed, -1);
	buf = g_strdup(name);

	/* do something with the value */
	fixed ^= 1;

	/* set new value */
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, COLUMN_FIXED,
			   fixed, -1);
	/* clean up */
	gtk_tree_path_free(path);
}


/******************************************************************************
 * Name
 *   add_columns
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void add_columns (GtkTreeView *treeview)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_columns(GtkTreeView * treeview, gboolean remove)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);

	/* column for sword module name */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Module Name"),
						     renderer, "text",
						     COLUMN_NAME, NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
	gtk_tree_view_append_column(treeview, column);

	/* column for fixed toggles */
	renderer = gtk_cell_renderer_toggle_new();
	g_signal_connect(renderer, "toggled",
			 G_CALLBACK(fixed_toggled), model);

	column = gtk_tree_view_column_new_with_attributes(_("Install"),
							  renderer,
							  "active",
							  COLUMN_FIXED,
							  "visible",
							  COLUMN_VISIBLE,
							  NULL);

	/* set this column to a fixed sizing (of 50 pixels) */
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN
					     (column), 50);
	gtk_tree_view_append_column(treeview, column);
	
	/* column for installed module version */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Old Verson"),
						     renderer, "text",
						     COLUMN_OLD_VERSION,
						     NULL);
	gtk_tree_view_column_set_sort_column_id(column,
						COLUMN_OLD_VERSION);
	gtk_tree_view_append_column(treeview, column);
	
	if(remove)
		return;
	
	/* column for source module version */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("New Verson"),
						     renderer, "text",
						     COLUMN_NEW_VERSION,
						     NULL);
	gtk_tree_view_column_set_sort_column_id(column,
						COLUMN_NEW_VERSION);
	gtk_tree_view_append_column(treeview, column);
}


/******************************************************************************
 * Name
 *   add_columns
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void add_columns (GtkTreeView *treeview)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_columns_to_first(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);


	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Configure"),
						     renderer, "text",
						     COLUMN_NAME, NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
	gtk_tree_view_append_column(treeview, column);


}


static GtkTreeModel *create_model_to_first(void)
{
	GtkTreeStore *model;
	GtkTreeIter iter;
	GtkTreeIter child_iter;


	model = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_INT);



	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, "Configure", -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, "SWORD", 1, 1, -1);


	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, "Modules", -1);
	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, "Install", 1, 2, -1);
	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, "Remove", 1, 3, -1);

	return GTK_TREE_MODEL(model);
}

/******************************************************************************
 * Name
 *   create_model
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   GtkTreeModel *create_model (void)
 *
 * Description
 *   
 *
 * Return value
 *   GtkTreeModel *
 */

static GtkTreeModel *create_model(void)
{
	gint i = 0;
	GtkTreeStore *store;
	GtkTreeIter iter;
	GtkTreeIter parent;

	/* create list store */
	store = gtk_tree_store_new(NUM_COLUMNS,
				   G_TYPE_STRING,
				   G_TYPE_BOOLEAN,
				   G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_BOOLEAN);
	return GTK_TREE_MODEL(store);
}


static gboolean button_press_event(GtkWidget * widget, 
				   GdkEventButton * event,
		   		   gpointer data)
{
	GtkTreeSelection *selection = NULL;
	GtkTreeIter selected;
	gboolean is_selected = FALSE;
	gint sel;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview1));
	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview1));
	if (gtk_tree_selection_get_selected(selection, NULL, &selected)) {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   1, &sel, -1);
		if (!gtk_tree_model_iter_has_child
		    (GTK_TREE_MODEL(model), &selected)) {
			gtk_notebook_set_current_page(GTK_NOTEBOOK
						      (notebook1), sel);
			switch (sel) {
			case 1:
				if (GTK_TOGGLE_BUTTON(radiobutton2)->active)
					gtk_widget_show(button5);
				else
					gtk_widget_hide(button5);
				gtk_widget_hide(button6);
				gtk_widget_hide(button7);
				break;
			case 2:
				if (GTK_TOGGLE_BUTTON(radiobutton2)->active)
					gtk_widget_show(button5);
				else
					gtk_widget_hide(button5);
				gtk_widget_show(button6);
				gtk_widget_hide(button7);
				break;
			case 3:
				gtk_widget_show(button7);
				gtk_widget_hide(button5);
				gtk_widget_hide(button6);
				break;
			}

		}
	}
}


static void on_notebook1_switch_page(GtkNotebook * notebook,
				     GtkNotebookPage * page,
				     guint page_num, gpointer user_data)
{
	gint test;
	GS_DIALOG *yes_no_dialog;
	GString *str = g_string_new(NULL);
	
	switch (page_num) {
	case 0:
		break;
	case 1:

		break;
	case 2:
		if(!have_configs) {
			yes_no_dialog = gui_new_dialog();
			yes_no_dialog->stock_icon = GTK_STOCK_DIALOG_QUESTION;
			g_string_printf(str,
				"<span weight=\"bold\">%s</span>\n\n%s",
				_("Please Refresh"), 
				_("Your module list is not up to date!"));
			yes_no_dialog->label_top = str->str;
			yes_no_dialog->ok = TRUE;
		
			test = gui_alert_dialog(yes_no_dialog);
			if (test != GS_OK) {
				
			}
		}
		if (GTK_TOGGLE_BUTTON(radiobutton_dest)->active) {
			destination =
			    gtk_label_get_text(GTK_LABEL(label_home));
			dot_sword = TRUE;
		} else {
			destination = NULL;
			local = FALSE;
		}
		mod_mgr_shut_down();
		mod_mgr_init(destination);
		load_module_tree(GTK_TREE_VIEW(treeview), TRUE);
		break;
	case 3:
		if (GTK_TOGGLE_BUTTON(radiobutton_dest)->active) {
			destination =
			    gtk_label_get_text(GTK_LABEL(label_home));
			dot_sword = TRUE;
		} else {
			destination = NULL;
			local = FALSE;
		}
		mod_mgr_shut_down();
		mod_mgr_init(destination);
		load_module_tree(GTK_TREE_VIEW(treeview2), FALSE);
		break;
	}

}


void on_radiobutton2_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	if(togglebutton->active)
		gtk_widget_show(button5);
	else
		gtk_widget_hide(button5);
}



/******************************************************************************
 * Name
 *   create_dialog
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   GtkWidget *create_dialog (void)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *create_dialog(void)
{
	
	GtkWidget *dialog_vbox1;
	GtkWidget *hpaned1;
	GtkWidget *scrolledwindow2;
	GtkWidget *vbox6;
	GtkWidget *label12;
	GtkWidget *image5;
	GtkWidget *label13;
	GtkWidget *label14;
	GtkWidget *label11;
	GtkWidget *vbox3;
	GtkWidget *label5;
	GtkWidget *vbox4;
	GtkWidget *hbox4;
	GtkWidget *hbox2;
	//GList *combo1_items = NULL;
	GtkWidget *label6;
	GtkWidget *vbox5;
	GtkWidget *hbox9;
	GtkWidget *image4;
	GtkWidget *label10;
	GtkWidget *label2;
	GtkWidget *vbox1;
	GtkWidget *scrolledwindow1;
	GtkWidget *label3;
	GtkWidget *vbox2;
	GtkWidget *scrolledwindow3;
	GtkWidget *progressbar1;
	GtkWidget *label4;
	GtkWidget *dialog_action_area1;
	GtkWidget *button4;
	GtkWidget *alignment3;
	GtkWidget *hbox8;
	GtkWidget *image3;
	GtkWidget *label9;
	GtkWidget *alignment2;
	GtkWidget *hbox7;
	GtkWidget *image2;
	GtkWidget *label8;
	GtkTreeModel *model;
	GError *error = NULL;
	GtkTooltips *tooltips;
	tooltips = gtk_tooltips_new();

	dialog = gtk_dialog_new();
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 6);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Module Manager"));
	gtk_window_set_default_size(GTK_WINDOW(dialog), 570, 259);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);

	dialog_vbox1 = GTK_DIALOG(dialog)->vbox;
	gtk_widget_show(dialog_vbox1);

	hpaned1 = gtk_hpaned_new();
	gtk_widget_show(hpaned1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), hpaned1, TRUE, TRUE,
			   0);
	gtk_container_set_border_width(GTK_CONTAINER(hpaned1), 6);
	gtk_paned_set_position(GTK_PANED(hpaned1), 145);

	scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow2);
	gtk_paned_pack1(GTK_PANED(hpaned1), scrolledwindow2, FALSE,
			TRUE);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow2),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	model = create_model_to_first();
	treeview1 = gtk_tree_view_new_with_model(model);
	gtk_widget_show(treeview1);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), treeview1);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview1),
					  FALSE);
	add_columns_to_first(GTK_TREE_VIEW(treeview1));
	gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview1));

	notebook1 = gtk_notebook_new();
	gtk_widget_show(notebook1);
	gtk_paned_pack2(GTK_PANED(hpaned1), notebook1, TRUE, TRUE);
	GTK_WIDGET_UNSET_FLAGS(notebook1, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook1), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook1), FALSE);

	vbox6 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox6);
	gtk_container_add(GTK_CONTAINER(notebook1), vbox6);

	label12 = gtk_label_new(_("<b>SWORD</b>"));
	gtk_widget_show(label12);
	gtk_box_pack_start(GTK_BOX(vbox6), label12, TRUE, TRUE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label12), TRUE);
	gtk_label_set_justify(GTK_LABEL(label12), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label12), 0.5, 0.81);

	image5 =
	    gtk_image_new_from_file(PACKAGE_PIXMAPS_DIR "/sword3.png");
	gtk_widget_show(image5);
	gtk_box_pack_start(GTK_BOX(vbox6), image5, TRUE, TRUE, 0);

	label13 = gtk_label_new(_("<b>Module Manager</b>"));
	gtk_widget_show(label13);
	gtk_box_pack_start(GTK_BOX(vbox6), label13, TRUE, TRUE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label13), TRUE);
	gtk_label_set_justify(GTK_LABEL(label13), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label13), 0.5, 0.28);

	label14 =
	    gtk_label_new(_
			  ("<b>WARNING:</b> If you live in a persecuted country use with care."));
	gtk_widget_show(label14);
	gtk_box_pack_start(GTK_BOX(vbox6), label14, FALSE, FALSE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label14), TRUE);
	gtk_label_set_justify(GTK_LABEL(label14), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label14), TRUE);

	label11 = gtk_label_new(_("Cover"));
	gtk_widget_show(label11);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook1), 0),
				   label11);
	gtk_label_set_justify(GTK_LABEL(label11), GTK_JUSTIFY_LEFT);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(notebook1), vbox3);
	gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(notebook1),
					   vbox3, TRUE, TRUE,
					   GTK_PACK_START);
	gtk_container_set_border_width(GTK_CONTAINER(vbox3), 4);

	label5 = gtk_label_new(_("<b>Install Source</b>"));
	gtk_widget_show(label5);
	gtk_box_pack_start(GTK_BOX(vbox3), label5, FALSE, FALSE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label5), TRUE);
	gtk_label_set_justify(GTK_LABEL(label5), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

	vbox4 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox4);
	gtk_box_pack_start(GTK_BOX(vbox3), vbox4, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox4), 10);

	hbox4 = gtk_hbox_new(FALSE, 21);
	gtk_widget_show(hbox4);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox4, FALSE, TRUE, 0);


	radiobutton_source =
	    gtk_radio_button_new_with_mnemonic(NULL, _("Local"));
	gtk_widget_show(radiobutton_source);
	gtk_box_pack_start(GTK_BOX(hbox4), radiobutton_source, FALSE,
			   FALSE, 0);

	fileentry1 = gnome_file_entry_new(NULL, _("Local Source"));
	gtk_widget_show(fileentry1);
	gtk_box_pack_start(GTK_BOX(hbox4), fileentry1, TRUE, TRUE, 0);
	gnome_file_entry_set_directory_entry(GNOME_FILE_ENTRY
					     (fileentry1), TRUE);

	combo_entry1 =
	    gnome_file_entry_gtk_entry(GNOME_FILE_ENTRY(fileentry1));
	gtk_widget_show(combo_entry1);

	hbox2 = gtk_hbox_new(FALSE, 6);
	gtk_widget_show(hbox2);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox2, FALSE, TRUE, 0);

	radiobutton2 =
	    gtk_radio_button_new_with_mnemonic_from_widget((GtkRadioButton *) radiobutton_source, _("Remote"));
	gtk_widget_show(radiobutton2);
	gtk_box_pack_start(GTK_BOX(hbox2), radiobutton2, FALSE, FALSE,
			   0);

	combo1 = gtk_combo_new();
	g_object_set_data(G_OBJECT(GTK_COMBO(combo1)->popwin),
			  "GladeParentKey", combo1);
	gtk_widget_show(combo1);
	gtk_box_pack_start(GTK_BOX(hbox2), combo1, TRUE, TRUE, 0);
	
	combo_entry2 = GTK_COMBO(combo1)->entry;
	gtk_widget_show(combo_entry2);

	label6 = gtk_label_new(_("<b>Install Destination</b>"));
	gtk_widget_show(label6);
	gtk_box_pack_start(GTK_BOX(vbox3), label6, FALSE, FALSE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label6), TRUE);
	gtk_label_set_justify(GTK_LABEL(label6), GTK_JUSTIFY_LEFT);
	gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

	vbox5 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox5);
	gtk_box_pack_start(GTK_BOX(vbox3), vbox5, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox5), 10);

	label_home = gtk_label_new(NULL);
	gtk_widget_show(label_home);
	radiobutton_dest = gtk_radio_button_new_from_widget(NULL);
	gtk_widget_show(radiobutton_dest);
	gtk_box_pack_start(GTK_BOX(vbox5), radiobutton_dest, FALSE,
			   FALSE, 0);
	gtk_container_add(GTK_CONTAINER(radiobutton_dest), label_home);


	label_system = gtk_label_new(NULL);
	gtk_widget_show(label_system);
	radiobutton4 =
	    gtk_radio_button_new_from_widget((GtkRadioButton *)
					     radiobutton_dest);
	gtk_widget_show(radiobutton4);
	gtk_box_pack_start(GTK_BOX(vbox5), radiobutton4, FALSE, FALSE,
			   0);
	gtk_container_add(GTK_CONTAINER(radiobutton4), label_system);

	hbox9 = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox9);
	gtk_box_pack_start(GTK_BOX(vbox3), hbox9, FALSE, FALSE, 0);

	image4 = gtk_image_new_from_stock("gtk-dialog-warning",
					  GTK_ICON_SIZE_DIALOG);
	gtk_widget_show(image4);
	gtk_box_pack_start(GTK_BOX(hbox9), image4, FALSE, TRUE, 0);
	gtk_misc_set_alignment(GTK_MISC(image4), 0, 0);

	label10 =
	    gtk_label_new(_
			  ("<b>WARNING:</b> If you live in a persecuted country and do not wish to risk detection you should NOT use  the remote installation feature! "));
	gtk_widget_show(label10);
	gtk_box_pack_start(GTK_BOX(hbox9), label10, TRUE, TRUE, 0);
	gtk_label_set_use_markup(GTK_LABEL(label10), TRUE);
	gtk_label_set_justify(GTK_LABEL(label10), GTK_JUSTIFY_LEFT);
	gtk_label_set_line_wrap(GTK_LABEL(label10), TRUE);

	label2 = gtk_label_new(_("Configure"));
	gtk_widget_show(label2);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook1), 1),
				   label2);
	gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_LEFT);

	vbox1 = gtk_vbox_new(FALSE, 12);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(notebook1), vbox1);
	gtk_container_set_border_width(GTK_CONTAINER(vbox1), 6);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow1);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	model = create_model();

	treeview = gtk_tree_view_new_with_model(model);
	gtk_widget_show(treeview);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview),
					  TRUE);

	/* add columns to the tree view */
	add_columns(GTK_TREE_VIEW(treeview), FALSE);
	//load_module_tree (GTK_TREE_VIEW (treeview));


	progressbar = gtk_progress_bar_new();
	gtk_widget_show(progressbar);
	gtk_box_pack_start(GTK_BOX(vbox1), progressbar, FALSE, FALSE,
			   0);

	label3 = gtk_label_new(_("Install Modules"));
	gtk_widget_show(label3);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook1), 2),
				   label3);
	gtk_label_set_justify(GTK_LABEL(label3), GTK_JUSTIFY_LEFT);

	vbox2 = gtk_vbox_new(FALSE, 12);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(notebook1), vbox2);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2), 6);

	scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow3);
	gtk_box_pack_start(GTK_BOX(vbox2), scrolledwindow3, TRUE, TRUE,
			   0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow3),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	model = create_model();

	treeview2 = gtk_tree_view_new_with_model(model);
	//treeview2 = gtk_tree_view_new();
	gtk_widget_show(treeview2);
	gtk_container_add(GTK_CONTAINER(scrolledwindow3), treeview2);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview2),
					  TRUE);
	/* add columns to the tree view */
	add_columns(GTK_TREE_VIEW(treeview2), TRUE);			  
					  
	progressbar1 = gtk_progress_bar_new();
	gtk_widget_show(progressbar1);
	gtk_box_pack_start(GTK_BOX(vbox2), progressbar1, FALSE, FALSE,
			   0);

	label4 = gtk_label_new(_("Remove Modules"));
	gtk_widget_show(label4);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook1),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(notebook1), 3),
				   label4);
	gtk_label_set_justify(GTK_LABEL(label4), GTK_JUSTIFY_LEFT);

	dialog_action_area1 = GTK_DIALOG(dialog)->action_area;
	gtk_widget_show(dialog_action_area1);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1),
				  GTK_BUTTONBOX_END);

	button4 = gtk_button_new_from_stock("gtk-close");
	gtk_widget_show(button4);
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button4,
				     GTK_RESPONSE_CLOSE);
	GTK_WIDGET_SET_FLAGS(button4, GTK_CAN_DEFAULT);

	button5 = gtk_button_new_from_stock("gtk-refresh");
	//gtk_widget_show (button5);
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button5,
				     GTK_RESPONSE_REFRESH);
	GTK_WIDGET_SET_FLAGS(button5, GTK_CAN_DEFAULT);
	gtk_tooltips_set_tip(tooltips, button5,
			     _("Refresh remote sources"), NULL);

	button6 = gtk_button_new();
	//gtk_widget_show (button6);
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button6,
				     GTK_RESPONSE_INSTALL);
	GTK_WIDGET_SET_FLAGS(button6, GTK_CAN_DEFAULT);
	gtk_tooltips_set_tip(tooltips, button6,
			     _("Upgrade or install selected modules"),
			     NULL);

	alignment3 = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_widget_show(alignment3);
	gtk_container_add(GTK_CONTAINER(button6), alignment3);

	hbox8 = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox8);
	gtk_container_add(GTK_CONTAINER(alignment3), hbox8);

	image3 =
	    gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image3);
	gtk_box_pack_start(GTK_BOX(hbox8), image3, FALSE, FALSE, 0);

	label9 = gtk_label_new_with_mnemonic(_("Install"));
	gtk_widget_show(label9);
	gtk_box_pack_start(GTK_BOX(hbox8), label9, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(label9), GTK_JUSTIFY_LEFT);

	button7 = gtk_button_new();
	//gtk_widget_show (button7);
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button7,
				     GTK_RESPONSE_REMOVE);
	GTK_WIDGET_SET_FLAGS(button7, GTK_CAN_DEFAULT);
	gtk_tooltips_set_tip(tooltips, button7,
			     _("Remove selected module(s)"), NULL);

	alignment2 = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_widget_show(alignment2);
	gtk_container_add(GTK_CONTAINER(button7), alignment2);

	hbox7 = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox7);
	gtk_container_add(GTK_CONTAINER(alignment2), hbox7);

	image2 = gtk_image_new_from_stock("gtk-remove",
					  GTK_ICON_SIZE_BUTTON);
	gtk_widget_show(image2);
	gtk_box_pack_start(GTK_BOX(hbox7), image2, FALSE, FALSE, 0);

	label8 = gtk_label_new_with_mnemonic(_("Remove"));
	gtk_widget_show(label8);
	gtk_box_pack_start(GTK_BOX(hbox7), label8, FALSE, FALSE, 0);
	gtk_label_set_justify(GTK_LABEL(label8), GTK_JUSTIFY_LEFT);


	g_signal_connect((gpointer) dialog, "response",
			 G_CALLBACK(on_dialog_response), NULL);
	g_signal_connect(G_OBJECT(treeview1),
			 "button_release_event",
			 G_CALLBACK(button_press_event), NULL);
	g_signal_connect((gpointer) notebook1, "switch_page",
			 G_CALLBACK(on_notebook1_switch_page), NULL);
	g_signal_connect ((gpointer) radiobutton2, "toggled",
		    	G_CALLBACK (on_radiobutton2_toggled),
		    	NULL);
	return dialog;
}

/******************************************************************************
 * Name
 *   gui_open_mod_mgr
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void gui_open_mod_mgr(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_open_mod_mgr(void)
{
	GtkWidget *dlg;
	GString *str = g_string_new(NULL);
	GList *tmp = NULL;
	GList *combo1_items = NULL;
	MOD_MGR_SOURCE *mms;
	
	g_string_printf(str, "%s/%s", settings.homedir, 
			".sword/InstallMgr/InstallMgr.conf");
	
	if(!mod_mgr_check_for_configs(str->str)) {
		have_configs = FALSE;
		mod_mgr_init_config();
	}	
	have_configs = TRUE;
	
	open_config_file(str->str);
	mod_mgr_init(NULL);
	dlg = create_dialog();

	g_string_printf(str, "%s/%s", settings.homedir, ".sword");
	gtk_label_set_text(GTK_LABEL(label_home), str->str);
	gtk_label_set_text(GTK_LABEL(label_system), get_path_to_mods());
	
	
	gtk_entry_set_text(GTK_ENTRY(combo_entry1),
			get_config_value("Sources", "Local"));
	
	tmp = mod_mgr_list_remote_sources();
	while (tmp) {
		mms = (MOD_MGR_SOURCE *) tmp->data;
		combo1_items =
		    g_list_append(combo1_items, (gchar *) mms->caption);
		tmp = g_list_next(tmp);
	}
	gtk_combo_set_popdown_strings(GTK_COMBO(combo1), combo1_items);
	g_list_free(combo1_items);
	g_list_free(tmp);

	close_config_file();
	gtk_widget_show(dlg);
	g_string_free(str, TRUE);
}

void gui_update_install_status(glong total, glong done, const gchar *message)
{
	 gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar),message);
}

void gui_update_install_progressbar(gdouble fraction)
{
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), fraction);
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}
}
