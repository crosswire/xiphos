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
#include <errno.h>

#include <bonobo.h>
#include <gnome.h>
#include <glade/glade-xml.h>

#include "gui/mod_mgr.h"
#include "gui/dialog.h"
#include "gui/sidebar.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/lists.h"
#include "main/mod_mgr.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"
#include "main/modulecache.hh"

#define GTK_RESPONSE_REFRESH 301
#define GTK_RESPONSE_REMOVE  302
#define GTK_RESPONSE_INSTALL 303
#define GTK_RESPONSE_ARCHIVE 304
#define GTK_RESPONSE_FASTMOD 305
#define GTK_RESPONSE_DELFAST 306
/* see these codes' use in ui/module-manager.glade. */

enum {
	REMOVE  = 0,
	INSTALL = 1,
	ARCHIVE = 2,
	FASTMOD = 3,
	DELFAST = 4,
};

enum {
	COLUMN_NAME,
	COLUMN_INSTALLED,
	COLUMN_FASTREADY,
	COLUMN_LOCKED,
	COLUMN_FIXED,
	COLUMN_OLD_VERSION,
	COLUMN_DIFFERENT,
	COLUMN_NEW_VERSION,
	COLUMN_INSTALLSIZE,
	COLUMN_DESC,
	COLUMN_VISIBLE,
	NUM_COLUMNS
};

enum {
	COLUMN_TYPE,
	COLUMN_CAPTION,
	COLUMN_SOURCE,
	COLUMN_DIRECTORY,
	NUM_REMOTE_COLUMNS
};

static GtkWidget *treeview;
static GtkWidget *treeview1;
static GtkWidget *treeview2;
static GtkWidget *notebook1;
static GtkWidget *button_close;
static GtkWidget *button_cancel;
static GtkWidget *button1;
static GtkWidget *button2;
static GtkWidget *button3;
static GtkWidget *button_arch;
static GtkWidget *button_idx;
static GtkWidget *button_delidx;
static GtkWidget *label_home;
static GtkWidget *label_system;
//static GtkWidget *progressbar;
//static GtkWidget *progressbar1;
static GtkWidget *progressbar_refresh;
static GtkWidget *radiobutton_source;
static GtkWidget *radiobutton2;
static GtkWidget *radiobutton_dest;
static GtkWidget *radiobutton4;
static GtkWidget *fileentry1;
static GtkWidget *combo_entry1;
static GtkWidget *combo_entry2;
static GtkWidget *combo1;
static GtkWidget *dialog;
static GtkWidget *treeview_local;
static GtkWidget *treeview_remote;
static GtkWidget *button_add_remote;
static GtkWidget *button_remove_remote;

static gboolean local;
static const gchar *source;
static gboolean dot_sword;
static const gchar *destination;
static gboolean have_configs;
static gboolean have_changes;
static gboolean need_update;
static gint current_page;
static gint response;
static GdkPixbuf *INSTALLED;
static GdkPixbuf *FASTICON;
static GdkPixbuf *NO_INDEX;
static GdkPixbuf *LOCKED;
static GdkPixbuf *REFRESH;
static GdkPixbuf *BLANK;
static gchar *current_mod;
static gchar *remote_source;
static gboolean first_time_user = FALSE;

GladeXML *gxml;

static void load_module_tree(GtkTreeView * treeview, gboolean install);

/******************************************************************************
 * Name
 *   create_pixbufs
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void create_pixbufs(void)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void create_pixbufs(void)
{
	INSTALLED = gtk_widget_render_icon(dialog,
					   GTK_STOCK_APPLY,
					   GTK_ICON_SIZE_MENU, NULL);
	FASTICON = gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					    "/dlg-un-16.png", NULL);
	NO_INDEX = gtk_widget_render_icon(dialog,
					  GTK_STOCK_CANCEL,
					  GTK_ICON_SIZE_MENU, NULL);
	LOCKED = gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR
					  "/epiphany-secure.png", NULL);
	REFRESH = gtk_widget_render_icon(dialog,
					 GTK_STOCK_REFRESH,
					 GTK_ICON_SIZE_MENU, NULL);
	BLANK = gtk_widget_render_icon(dialog,
				       "gnome-stock-blank",
				       GTK_ICON_SIZE_MENU, NULL);
}


/******************************************************************************
 * Name
 *   mod_mgr_check_for_file
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   gboolean mod_mgr_check_for_file(const gchar * filename)
 *
 * Description
 *    
 *
 * Return value
 *   gboolean
 */

static gboolean mod_mgr_check_for_file(const gchar * filename)
{
	return g_file_test(filename, G_FILE_TEST_EXISTS);
}


/******************************************************************************
 * Name
 *   remove_install_modules
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *    void remove_install_modules(GList * modules, int activity)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

#define ZIP_DIR	".sword/zip"

static void remove_install_modules(GList * modules, int activity)
{
	GList *tmp = NULL;
	gchar *buf;
	gchar *verb;
	const gchar *new_dest;
	gint test;
	gint failed = 1;
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

	switch (activity)
	{
	case INSTALL:
		verb = _("Install these modules:"); break;
	case REMOVE:
		verb = _("Remove these modules:"); break;
	case ARCHIVE:
		verb = _("Archive these modules:"); break;
	case FASTMOD:
		verb = _("Build fast-search index for these\nmodules (may take minutes/module):"); break;
	case DELFAST:
		verb = _("Delete fast-search index for these modules:"); break;
	}
	g_string_printf(dialog_text,
			"<span weight=\"bold\">%s</span>\n\n%s",
			verb, mods->str);

	yes_no_dialog->label_top = dialog_text->str;
	yes_no_dialog->yes = TRUE;
	yes_no_dialog->no = TRUE;

	test = gui_alert_dialog(yes_no_dialog);
	if (test != GS_YES) {
		tmp = modules; /* free list data */
		while (tmp) {
			g_free((gchar*)tmp->data);
			tmp = g_list_next(tmp);
		}
		g_list_free(tmp);
		g_free(yes_no_dialog);
		g_string_free(mods, TRUE);
		g_string_free(dialog_text, TRUE);
		return;
	}
	switch (activity)
	{
	case INSTALL:
		verb = _("Preparing to install"); break;
	case REMOVE:
		verb = _("Preparing to remove"); break;
	case ARCHIVE:
		verb = _("Preparing to archive"); break;
	case FASTMOD:
		verb = _("Preparing to index"); break;
	case DELFAST:
		verb = _("Preparing to delete index"); break;
	}
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), verb);
	gtk_widget_show(progressbar_refresh);	
	while (gtk_events_pending())
		gtk_main_iteration();
	g_free(yes_no_dialog);
	g_string_free(dialog_text, TRUE);
	gtk_widget_queue_draw(dialog);
	gtk_widget_hide(button_close);
	gtk_widget_hide(button1);
	gtk_widget_hide(button2);
	gtk_widget_hide(button3);
	gtk_widget_hide(button_arch);
	gtk_widget_hide(button_idx);
	gtk_widget_hide(button_delidx);
	gtk_widget_show(button_cancel);
	while (gtk_events_pending())
		gtk_main_iteration();

	tmp = modules;
	while (tmp) {
		buf = (gchar *) tmp->data;
		current_mod = buf;
		switch (activity)
		{
		case INSTALL:
			verb = _("Installing"); break;
		case REMOVE:
			verb = _("Removing"); break;
		case ARCHIVE:
			verb = _("Archiving"); break;
		case FASTMOD:
			verb = _("Indexing"); break;
		case DELFAST:
			verb = _("Deleting index"); break;
		}
		g_string_printf(mods, "%s: %s", verb, buf);
		gtk_progress_bar_set_text(GTK_PROGRESS_BAR
					  (progressbar_refresh),
					  mods->str);

		if (activity == ARCHIVE) {
			GString *cmd = g_string_new(NULL);
			gchar *dir = g_new(char, strlen(settings.homedir) +
					   strlen(ZIP_DIR) + 2);
			gchar *zipfile;
			char *datapath, *conf_file;
			FILE *result;

			GS_print(("archive %s in %s\n", buf, destination));
			sprintf(dir, "%s/%s", settings.homedir, ZIP_DIR);
			if ((access(dir, F_OK) == -1) &&
			    (mkdir(dir, S_IRWXU) != 0)) {
				char msg[300];
				sprintf(msg, _("`mkdir %s' failed:\n%s."),
					dir, strerror(errno));
				g_free(dir);
				gui_generic_warning(msg);
				return;
			}

			zipfile = g_new(char, strlen(dir) + 100); // excess
			sprintf(zipfile, "%s/%s.zip", dir, buf);
			datapath = main_get_mod_config_entry(buf, "DataPath");
			if (access(datapath, F_OK) == -1)
			    *(strrchr(datapath, '/')) = '\0';

			conf_file = main_get_mod_config_file(buf, destination);
			g_string_printf(cmd,
				"( rm -f '%s' && cd '%s' && zip -r '%s' 'mods.d/%s' '%s' ) 2>&1",
				zipfile,
				destination,
				zipfile,
				conf_file,
				datapath);
			g_free(conf_file);
			g_free(datapath);
			
			if ((result = popen(cmd->str, "r")) == NULL) {
				gui_generic_warning
				    (_("GnomeSword could not execute archiver"));
			} else {
				gchar output[258];
				g_string_truncate(cmd, 0);
				g_string_append(cmd, buf);
				g_string_append(cmd, _(" archival result:\n\n"));
				while (fgets(output, 256, result) != NULL)
					g_string_append(cmd, output);
				pclose(result);
				g_string_append(cmd, _("\nArchive in "));
				g_string_append(cmd, zipfile);
				gui_generic_warning(cmd->str);
				while (gtk_events_pending())
					gtk_main_iteration();
			}
			g_free(zipfile);
			g_free(dir);
			g_string_free(cmd, TRUE);
			failed = 0;
		}

		if ((activity == REMOVE) ||	// just delete it
		    (!first_time_user &&	// don't trip on "no modules".
		     (activity == INSTALL) &&
		     main_is_module(buf))) {	// delete before re-install
			GS_print(("remove %s from %s\n", buf, destination));
			failed = mod_mgr_uninstall(destination, buf);
			if (failed == -1) {
				//mod_mgr_shut_down();
				if (destination) {
					new_dest = NULL;
				} else {
					new_dest = gtk_label_get_text
					    (GTK_LABEL(label_home));
					GS_warning((new_dest));
				}
				GS_print(("removing %s from %s\n",
					  buf, new_dest));
				failed =
				    mod_mgr_uninstall(new_dest, buf);
			}

			// annihilate cache of removed module.
			ModuleCacheErase((const char *)buf);

			while (gtk_events_pending())
				gtk_main_iteration();
		}

		if (activity == INSTALL) {
			GS_print(("install %s, source=%s\n", buf,source));
			if (local)
				failed =
				    mod_mgr_local_install_module(source, buf);
			else
				failed =
				    mod_mgr_remote_install(source, buf);
		}

		if (activity == FASTMOD) {
			GS_print(("index %s\n", buf));
			if (main_module_mgr_index_mod(buf))
				failed = 0;
			else
				failed = 1;
		}

		if (activity == DELFAST) {
			GS_print(("deleting index %s\n", buf));
			if (main_module_mgr_delete_index_mod(buf))
				failed = 0;
			else
				failed = 1;
		}

		g_free(tmp->data);
		tmp = g_list_next(tmp);
	}
	have_changes = TRUE;
	g_list_free(tmp);
	if (failed) {
		switch (activity)
		{
		case INSTALL:
		    verb = _("Install"); break;
		case REMOVE:
		    verb = _("Remove"); break;
		case ARCHIVE:
		    verb = _("Archive"); break;
		case FASTMOD:
		    verb = _("Index"); break;
		case DELFAST:
		    verb = _("Deletion"); break;
		}
		g_string_printf(mods, _("%s failed"), verb);
	} else {
		g_string_printf(mods, "%s", _("Finished"));		
	}
	if (!failed && ((activity == FASTMOD) || (activity == DELFAST)))
		load_module_tree(GTK_TREE_VIEW(treeview2), 0);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), mods->str);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), 0);
	g_string_free(mods, TRUE);
	gtk_widget_hide(button_cancel);
	gtk_widget_show(button_close);
	switch(current_page) {
		case 3:
			if(need_update) gtk_widget_show(button1);
			gtk_widget_show(button2);
			gtk_widget_hide(button_arch);
			gtk_widget_hide(button_idx);
			gtk_widget_hide(button_delidx);
		break;
		case 4:
			gtk_widget_show(button3);
			gtk_widget_show(button_arch);
			gtk_widget_show(button_idx);
			gtk_widget_show(button_delidx);
		break;		
	}
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
	}
	while (gtk_tree_model_iter_next(model, &child));
	/*if(name)
		g_free(name);*/
	return list;
}


/******************************************************************************
 * Name
 *   get_list_mods_to_remove_install
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void get_list_mods_to_remove_install(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static GList *get_list_mods_to_remove_install(int activity)
{
	GList *retval = NULL;
	GtkTreeIter root;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *name;
	gboolean fixed;
	if (activity == INSTALL)
		model =
		    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	else
		model =
		    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview2));
	if (!gtk_tree_model_get_iter_first
	    (GTK_TREE_MODEL(model), &root))
		return retval;
	/*if (!gtk_tree_model_iter_has_child(model, &root))
		return retval;*/

	do {
		gtk_tree_model_get(model, &root,
				   COLUMN_FIXED, &fixed, COLUMN_NAME,
				   &name, -1);
		if (gtk_tree_model_iter_has_child(model, &root)) {
			retval = parse_treeview(retval, model, &root);
		} else {
			if (fixed) {
				retval = g_list_append(retval, (gchar *)name);
			}

		}
	}
	while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &root));
	/*if(name)
		g_free(name);*/
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
	GdkPixbuf *installed;
	GdkPixbuf *fasticon;
	GdkPixbuf *locked;
	GdkPixbuf *refresh;
	gchar *description = NULL;
	gsize bytes_read;
	gsize bytes_written;
//	GError *error = NULL;
	gchar *buf;

	/* Check language */
	buf = strdup(info->language);
	if (!g_utf8_validate(buf,-1,NULL))
		info->language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (info->language == NULL))
		info->language = _("Unknown");
	g_free(buf);

/*	description = g_convert(info->description, -1, UTF_8, OLD_CODESET, &bytes_read,
			 &bytes_written, &error);    */
	description = info->description;
//	if(description == NULL) {
//		GS_print(("error: %s\n", error->message));
//		g_error_free (error);
//	}

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		gtk_tree_model_get(model, &iter_iter, COLUMN_NAME,
				   &str_data, -1);
		if (!strcmp(info->language, str_data)) {
			if (info->installed)
				installed = INSTALLED;
			else
				installed = BLANK;

			if (info->locked)
				locked = LOCKED;
			else
				locked = BLANK;

			if (info->installed &&
			    (!info->old_version && info->new_version &&
			     strcmp(info->new_version, " ")) ||
			    (info->old_version && !info->new_version) ||
			    (info->old_version && info->new_version &&
			     strcmp(info->new_version, info->old_version) > 0))
				refresh = REFRESH;
			else
				refresh = BLANK;

			if (!first_time_user &&
			    info->installed &&
			    main_has_search_framework(info->name)) {
				if (main_optimal_search(info->name))
					fasticon = FASTICON;
				else
					fasticon = NO_INDEX;
			} else
				fasticon = BLANK;

			gtk_tree_store_append(GTK_TREE_STORE(model),
					      &child_iter, &iter_iter);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter,
					   COLUMN_NAME, info->name,
					   COLUMN_INSTALLED, installed,
					   COLUMN_FASTREADY, fasticon,
					   COLUMN_LOCKED, locked,
					   COLUMN_FIXED, FALSE,
					   COLUMN_OLD_VERSION,
					   info->old_version,
					   COLUMN_DIFFERENT, refresh,
					   COLUMN_NEW_VERSION,
					   info->new_version,
					   COLUMN_INSTALLSIZE,
					   info->installsize,
					   COLUMN_DESC, description,
					   COLUMN_VISIBLE, TRUE, -1);
			g_free(str_data);
			return;
		}
		g_free(str_data);
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
				const gchar *language)
{
	GtkTreeIter iter_iter;
	GtkTreeIter parent;
	GtkTreeIter child_iter;
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	gboolean valid;
	gchar *buf;

	/* Check language */
	buf = strdup(language);
	if (!g_utf8_validate(buf,-1,NULL))
		language = _("Unknown");
	if (!g_unichar_isalnum(g_utf8_get_char(buf)) || (language == NULL))
		language = _("Unknown");

	valid = gtk_tree_model_iter_children(model, &iter_iter, &iter);
	while (valid) {
		/* Walk through the list, reading each row */
		gchar *str_data;

		buf = strdup(language);
		if(buf == NULL) {
			GS_print(("error: %s\n", error->message));
			g_error_free (error);
			return;
		}
		
		gtk_tree_model_get(model, 
				&iter_iter, 
				COLUMN_NAME, 
				&str_data, 
				-1);

		if(!g_utf8_collate(g_utf8_casefold(buf,-1),
				      g_utf8_casefold(str_data,-1))) {
		/*if(!strcmp(buf,str_data)) {*/
			g_free(str_data);
			g_free(buf);
			return;
		}
		g_free(str_data);
		g_free(buf);
		valid = gtk_tree_model_iter_next(model, &iter_iter);
	}
	gtk_tree_store_append(GTK_TREE_STORE(model), &child_iter, &iter);
//	buf = g_convert(language, 
//			-1, 
//			UTF_8, 
//			OLD_CODESET, 
//			&bytes_read,
//			&bytes_written, 
//			&error);
	buf = strdup(language); 

	gtk_tree_store_set(GTK_TREE_STORE(model), 
			&child_iter,
			COLUMN_VISIBLE, 
			FALSE, 
			COLUMN_NAME,
			(gchar *) buf, 
			-1);
	g_free(buf);

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
	GtkTreeIter dictionary;
	GtkTreeIter devotional;
	GtkTreeIter book;
	GtkTreeIter map;
	GtkTreeIter image;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeIter child_iter;
	GList *tmp = NULL;
	GList *tmp2 = NULL;
	MOD_MGR *info;

	mod_mgr_shut_down();
	while (gtk_events_pending())
		gtk_main_iteration();
	mod_mgr_init(destination);

	if (install) {
		if (GTK_TOGGLE_BUTTON(radiobutton_source)->active) {
			source =
			    gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combo_entry1)->child));
			local = TRUE;
		} else {
			source =
			    gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combo_entry2)->child));
			local = FALSE;
		}
		if (local)
			tmp = mod_mgr_list_local_modules(source);
		else
			tmp = mod_mgr_remote_list_modules(source);
	} else {
		tmp = mod_mgr_list_local_modules(main_get_path_to_mods());
	}

	store = GTK_TREE_STORE(gtk_tree_view_get_model(treeview));
	gtk_tree_store_clear(store);
	if (!g_list_length(tmp))
		return;
	tmp2 = tmp;

	/*  add Biblical Texts folder */
	gtk_tree_store_append(store, &text, NULL);
	gtk_tree_store_set(store, &text, 0, _("Biblical Texts"), -1);

	/*  add Commentaries folder */
	gtk_tree_store_append(store, &commentary, NULL);
	gtk_tree_store_set(store, &commentary, 0, _("Commentaries"), -1);

	/*  add Dictionaries folder */
	gtk_tree_store_append(store, &dictionary, NULL);
	gtk_tree_store_set(store, &dictionary, 0, _("Dictionaries"), -1);

	/*  add Devotionals folder */
	gtk_tree_store_append(store, &devotional, NULL);
	gtk_tree_store_set(store, &devotional, 0, _("Daily Devotionals"), -1);

	/*  add Books folder */
	gtk_tree_store_append(store, &book, NULL);
	gtk_tree_store_set(store, &book, 0, _("General Books"), -1);

	/*  add Maps folder */
	gtk_tree_store_append(store, &map, NULL);
	gtk_tree_store_set(store, &map, 0, _("Maps"), -1);

	/*  add Images folder */
	gtk_tree_store_append(store, &image, NULL);
	gtk_tree_store_set(store, &image, 0, _("Images"), -1);

	while (tmp2) {
		info = (MOD_MGR *) tmp2->data;
		if (!strcmp(info->type, TEXT_MODS)) {
			add_language_folder(GTK_TREE_MODEL(store), text,
					    info->language);
			add_module_to_language_folder(GTK_TREE_MODEL
						      (store), text,
						      info);
		}
		else if (!strcmp(info->type, COMM_MODS)) {
			add_language_folder(GTK_TREE_MODEL(store),
					    commentary, info->language);
			add_module_to_language_folder(GTK_TREE_MODEL
						      (store),
						      commentary, info);
		}
		else if (!strcmp(info->type, DICT_MODS)) {
			if (info->is_devotional) {
				add_language_folder(GTK_TREE_MODEL(store),
						    devotional, info->language);
				add_module_to_language_folder(GTK_TREE_MODEL
							      (store),
							      devotional, info);
			} else {
				add_language_folder(GTK_TREE_MODEL(store),
						    dictionary, info->language);
				add_module_to_language_folder(GTK_TREE_MODEL
							      (store),
							      dictionary, info);
			}
		}
		else if (!strcmp(info->type, BOOK_MODS)) {
			add_language_folder(GTK_TREE_MODEL(store), book,
					    info->language);
			add_module_to_language_folder(GTK_TREE_MODEL
						      (store), book,
						      info);
		}

		if (info->is_maps) {
				add_language_folder(GTK_TREE_MODEL(store),
						    map, info->language);
				add_module_to_language_folder(GTK_TREE_MODEL
							      (store),
							      map, info);
		}
		if (info->is_images) {
				add_language_folder(GTK_TREE_MODEL(store),
						    image, info->language);
				add_module_to_language_folder(GTK_TREE_MODEL
							      (store),
							      image, info);
		}

		g_free(info->name);
		g_free(info->type);
		g_free(info->new_version);
		g_free(info);
		tmp2 = g_list_next(tmp2);
	}
	g_list_free(tmp);
}


/******************************************************************************
 * Name
 *   remove_install_wrapper
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *    void remove_install_wrapper(GList * modules)
 *
 * Description
 *   generalization of install/remove/archive/index methodology.
 *
 * Return value
 *   void
 */

static void remove_install_wrapper(int activity)
{
	GList *modules = NULL;
	modules = get_list_mods_to_remove_install(activity);
	while (gtk_events_pending())
		gtk_main_iteration();
	remove_install_modules(modules, activity);
	load_module_tree(GTK_TREE_VIEW(treeview), (activity == INSTALL));
	while (gtk_events_pending())
		gtk_main_iteration();
}

/******************************************************************************
 * Name
 *   response_refresh
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void response_refresh(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void response_refresh(void)
{
	gint failed = 1;
	gchar *buf = NULL;
	
	buf = g_strdup_printf("%s: %s", _("Refreshing remote"),remote_source);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), buf);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), 0);
	gtk_widget_show(progressbar_refresh);	
	while (gtk_events_pending())
		gtk_main_iteration();
	failed = mod_mgr_refresh_remote_source(remote_source);
	
	/*failed =
		mod_mgr_refresh_remote_source(
		  gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combo_entry2)->child)));*/
	
	if (failed) {
		gtk_progress_bar_set_text(
			GTK_PROGRESS_BAR(progressbar_refresh), _("Remote not found"));
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), 0);
		return;
	}
	load_module_tree(GTK_TREE_VIEW(treeview), TRUE);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), _("Finished"));
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), 0);
	if(!need_update) {
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook1), 3);
		gtk_widget_hide(button1);
		gtk_widget_show(button2);
		gtk_widget_hide(button3);
		gtk_widget_hide(button_arch);
		gtk_widget_hide(button_idx);
		gtk_widget_hide(button_delidx);
	}
	g_free(buf);
}

/******************************************************************************
 * Name
 *   response_close
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void response_close(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void response_close(void)
{
	if(need_update) {
		GString *str = g_string_new(NULL);
		
		gtk_widget_destroy(GTK_WIDGET(dialog));
		g_string_printf(str, "%s/dirlist", settings.homedir);
		if (mod_mgr_check_for_file(str->str)) {
#ifdef DEBUG
			/* special case: don't use GS_warning. */
			g_warning(str->str);
#else
			unlink(str->str);
#endif
		}
		g_string_free(str,TRUE);
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

static void fixed_toggled(GtkCellRendererToggle * cell,
			  gchar * path_str, gpointer data)
{
	GtkTreeModel *model = (GtkTreeModel *) data;
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
	gboolean fixed;

	/* get toggled iter */
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter, COLUMN_FIXED, &fixed, -1);

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
	GtkWidget *image;

	/* column for sword module name */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Module Name"),
						     renderer, "text",
						     COLUMN_NAME, NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
	/* set this column to a fixed sizing (of 185 pixels) */
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN
					     (column), 185);	
	gtk_tree_view_append_column(treeview, column);

	column = gtk_tree_view_column_new();
	image = gtk_image_new_from_stock(GTK_STOCK_APPLY,
					 GTK_ICON_SIZE_MENU);
	gtk_widget_show(image);
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COLUMN_INSTALLED,
					    NULL);
	gtk_tree_view_append_column(treeview, column);

	column = gtk_tree_view_column_new();
	image = gtk_image_new_from_file(PACKAGE_PIXMAPS_DIR
					"/dlg-un-16.png");
	gtk_widget_show(image);
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COLUMN_FASTREADY,
					    NULL);
	gtk_tree_view_append_column(treeview, column);

	column = gtk_tree_view_column_new();
	image = gtk_image_new_from_file(PACKAGE_PIXMAPS_DIR
					"/epiphany-secure.png");
	gtk_widget_show(image);
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COLUMN_LOCKED,
					    NULL);
	gtk_tree_view_append_column(treeview, column);
	/*column =
	   gtk_tree_view_column_new_with_attributes(_("Locked"),
	   renderer, "pixbuf",
	   COLUMN_PIXBUF, NULL);
	   gtk_tree_view_column_set_widget (column, image);
	   gtk_tree_view_column_set_sort_column_id(column, COLUMN_PIXBUF);
	   gtk_tree_view_append_column(treeview, column); */

	/* column for fixed toggles */
	renderer = gtk_cell_renderer_toggle_new();
	g_signal_connect(renderer, "toggled",
			 G_CALLBACK(fixed_toggled), model);

	column = gtk_tree_view_column_new();
	if (remove)
		image = gtk_image_new_from_stock("gtk-yes",//GTK_STOCK_REMOVE,
						 GTK_ICON_SIZE_MENU);
	else
		image = gtk_image_new_from_stock(GTK_STOCK_ADD,
						 GTK_ICON_SIZE_MENU);
	gtk_widget_show(image);

	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "active",
					    COLUMN_FIXED,
					    "visible",
					    COLUMN_VISIBLE, NULL);
	//"pixbuf",COLUMN_LOCKED, NULL);
/*	column = gtk_tree_view_column_new_with_attributes(_("Add"),
							  renderer,
							  "active",
							  COLUMN_FIXED,
							  "visible",
							  COLUMN_VISIBLE,
							  NULL);*/

	/* set this column to a fixed sizing (of 50 pixels) */
	gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(column),
					GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN
					     (column), 25);
	gtk_tree_view_append_column(treeview, column);

	/* column for installed module version */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Old"),
						     renderer, "text",
						     COLUMN_OLD_VERSION,
						     NULL);
	/*gtk_tree_view_column_set_sort_column_id(column,
	   COLUMN_OLD_VERSION); */
	gtk_tree_view_append_column(treeview, column);

	if (remove)
		return;

	/* column for refresh/update */
	column = gtk_tree_view_column_new();
	image = gtk_image_new_from_stock(GTK_STOCK_REFRESH,
					 GTK_ICON_SIZE_MENU);
	gtk_widget_show(image);
	renderer = GTK_CELL_RENDERER(gtk_cell_renderer_pixbuf_new());
	gtk_tree_view_column_set_widget(column, image);
	gtk_tree_view_column_pack_start(column, renderer, TRUE);
	gtk_tree_view_column_set_attributes(column, renderer,
					    "pixbuf", COLUMN_DIFFERENT,
					    NULL);
	gtk_tree_view_append_column(treeview, column);

	/* column for source module version */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("New"),
						     renderer, "text",
						     COLUMN_NEW_VERSION,
						     NULL);
	/*gtk_tree_view_column_set_sort_column_id(column,
	   COLUMN_NEW_VERSION); */
	gtk_tree_view_append_column(treeview, column);

	/* column for source install size */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Size"),
						     renderer, "text",
						     COLUMN_INSTALLSIZE,
						     NULL);
	/*gtk_tree_view_column_set_sort_column_id(column,
	   COLUMN_INSTALLSIZE); */
	gtk_tree_view_append_column(treeview, column);

	/* column for description */
	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Description"),
						     renderer, "text",
						     COLUMN_DESC,
						     NULL);
	/*gtk_tree_view_column_set_sort_column_id(column,
	   COLUMN_DESC); */
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


/******************************************************************************
 * Name
 *   add_columns_to_remote_treeview
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void  add_columns_to_remote_treeview(GtkTreeView *treeview)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_columns_to_remote_treeview(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Type"),
						     renderer, "text",
						     COLUMN_TYPE, NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Caption"),
						     renderer, "text",
						     COLUMN_CAPTION,
						     NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Source"),
						     renderer, "text",
						     COLUMN_SOURCE,
						     NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
	gtk_tree_view_append_column(treeview, column);

	renderer = gtk_cell_renderer_text_new();
	column =
	    gtk_tree_view_column_new_with_attributes(_("Directory"),
						     renderer, "text",
						     COLUMN_DIRECTORY,
						     NULL);
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
	gtk_tree_store_set(model, &iter, 0, "SWORD", -1);

	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Sources"), 1, 1, -1);
	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Configure"), 1, 2, -1);


	gtk_tree_store_append(model, &iter, NULL);
	gtk_tree_store_set(model, &iter, 0, _("Modules"), -1);
	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Install/Update"), 1, 3, -1);
	gtk_tree_store_append(model, &child_iter, &iter);
	gtk_tree_store_set(model, &child_iter, 0, _("Maintenance"), 1, 4, -1);

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
				   GDK_TYPE_PIXBUF,
				   GDK_TYPE_PIXBUF,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_BOOLEAN,
				   G_TYPE_STRING,
				   GDK_TYPE_PIXBUF,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_BOOLEAN);
	return GTK_TREE_MODEL(store);
}

static GtkTreeModel *create_remote_source_treeview_model(void)
{
	gint i = 0;
	GtkListStore *store;
	GtkTreeIter iter;

	/* create list store */
	store = gtk_list_store_new(NUM_REMOTE_COLUMNS,
				   G_TYPE_STRING,
				   G_TYPE_STRING,
				   G_TYPE_STRING, G_TYPE_STRING);

	return GTK_TREE_MODEL(store);

}


static void load_source_treeviews(void)
{
	GList *tmp = NULL;
	GList *tmp2 = NULL;
	GList *combo1_items = NULL;
	GtkTreeIter iter;
	GtkTreeIter combo_iter;
	MOD_MGR_SOURCE *mms;
	GtkTreeModel *remote_model = 
		gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_remote));
	GtkTreeModel *local_model =
		gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_local));
	GtkTreeModel* module_box_local = 
		gtk_combo_box_get_model(GTK_COMBO_BOX(combo_entry1));
	GtkTreeModel* module_box_remote = 
		gtk_combo_box_get_model(GTK_COMBO_BOX(combo_entry2));
	/* remote */
	gtk_list_store_clear(GTK_LIST_STORE(remote_model));
	gtk_list_store_clear(GTK_LIST_STORE(module_box_remote));
	tmp = mod_mgr_list_remote_sources();
	tmp2 = g_list_first(tmp);
	while (tmp) {
		mms = (MOD_MGR_SOURCE *) tmp->data;
		gtk_list_store_append(GTK_LIST_STORE(remote_model),
				      &iter);
		gtk_list_store_set(GTK_LIST_STORE(remote_model), &iter,
				   COLUMN_TYPE, mms->type,
				   COLUMN_CAPTION, mms->caption,
				   COLUMN_SOURCE, mms->source,
				   COLUMN_DIRECTORY, mms->directory,
				   -1);
		gtk_list_store_append(GTK_LIST_STORE(module_box_remote), &combo_iter);
		gtk_list_store_set(GTK_LIST_STORE(module_box_remote), 
					&combo_iter, 
					0, 
					(gchar*)mms->caption, 
					-1);
		tmp = g_list_next(tmp);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_entry2), 0);
	while (tmp2) {
		mms = (MOD_MGR_SOURCE *) tmp2->data;
		g_free((gchar*)mms->type);
		g_free((gchar*)mms->caption);
		g_free((gchar*)mms->source);
		g_free((gchar*)mms->directory);
		g_free(mms);
		tmp2 = g_list_next(tmp2); 
	}
	g_list_free(tmp);
	tmp = NULL;
	g_list_free(tmp2);
	tmp2 = NULL;

	/* local */
	gtk_list_store_clear(GTK_LIST_STORE(local_model));
	gtk_list_store_clear(GTK_LIST_STORE(module_box_local));
	tmp = mod_mgr_list_local_sources();
	/*if (tmp) {
		mms = (MOD_MGR_SOURCE *) tmp->data;
		gnome_file_entry_set_filename((GnomeFileEntry *)
					      fileentry1,
					      mms->directory);
	}*/
	while (tmp) {
		mms = (MOD_MGR_SOURCE *) tmp->data;
		gtk_list_store_append(GTK_LIST_STORE(local_model),
				      &iter);
		gtk_list_store_set(GTK_LIST_STORE(local_model), &iter,
				   COLUMN_TYPE, mms->type,
				   COLUMN_CAPTION, mms->caption,
				   COLUMN_SOURCE, mms->source,
				   COLUMN_DIRECTORY, mms->directory,
				   -1);
		gtk_list_store_append(GTK_LIST_STORE(module_box_local), &combo_iter);
		gtk_list_store_set(GTK_LIST_STORE(module_box_local), 
					&combo_iter, 
					0, 
					(gchar*)mms->caption, 
					-1);
		g_free(mms);
		tmp = g_list_next(tmp);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_entry1), 0);
	g_list_free(tmp);


}


/******************************************************************************
 * Name
 *   clear_and_hide_progress_bar
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void clear_and_hide_progress_bar(void) 
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void clear_and_hide_progress_bar(void)
{
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), "");
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh),0);
	gtk_widget_hide(progressbar_refresh);	
}


/******************************************************************************
 * Name
 *   on_notebook1_switch_page
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_notebook1_switch_page(GtkNotebook * notebook,
 *				     GtkNotebookPage * page,
 *				     guint page_num, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
            
void on_notebook1_switch_page(GtkNotebook * notebook,
				     GtkNotebookPage * page,
				     guint page_num, gpointer user_data)
{
	gint test;
	GS_DIALOG *yes_no_dialog;
	GString *str = g_string_new(NULL);

	current_page = page_num;
	switch (page_num) {
	case 0:
		clear_and_hide_progress_bar();
		break;
	case 1:
		clear_and_hide_progress_bar();	
		mod_mgr_shut_down();
		mod_mgr_init(destination);	
		break;
	case 2:
		clear_and_hide_progress_bar();
		break;
	case 3:
		clear_and_hide_progress_bar();
		if (!have_configs) {
			yes_no_dialog = gui_new_dialog();
			yes_no_dialog->stock_icon =
			    GTK_STOCK_DIALOG_QUESTION;
			g_string_printf(str,
					"<span weight=\"bold\">%s</span>\n\n%s",
					_("Please Refresh"),
					_("Your module list is not up to date!"));
			yes_no_dialog->label_top = str->str;
			yes_no_dialog->ok = TRUE;

			test = gui_alert_dialog(yes_no_dialog);
			if (test == GS_OK) {

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
	case 4:
		clear_and_hide_progress_bar();
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


/******************************************************************************
 * Name
 *   on_radiobutton2_toggled
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_radiobutton2_toggled(GtkToggleButton * togglebutton,
 *			     gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void on_radiobutton2_toggled(GtkToggleButton * togglebutton,
			     gpointer user_data)
{
	if (togglebutton->active) {
		gtk_widget_show(button1);
		if(remote_source)
		        g_free(remote_source);
		remote_source = g_strdup(gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_entry2)));
	} else {
		gtk_widget_hide(button1);
		gtk_widget_hide(progressbar_refresh);
	}
}


/******************************************************************************
 * Name
 *   save_sources
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void save_sources(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void save_sources(void)
{
	gchar *type = NULL;
	gchar *caption = NULL;
	gchar *source = NULL;
	gchar *directory = NULL;
	gboolean valid;
	GtkTreeIter iter;
	GtkTreeModel *remote_model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_remote));
	GtkTreeModel *local_model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_local));

	mod_mgr_clear_config();

	valid = gtk_tree_model_get_iter_first(remote_model, &iter);
	while (valid) {
		gtk_tree_model_get(remote_model, &iter,
				   COLUMN_TYPE, &type,
				   COLUMN_CAPTION, &caption,
				   COLUMN_SOURCE, &source,
				   COLUMN_DIRECTORY, &directory, -1);

		mod_mgr_add_source("FTPSource",
				   type, caption, source, directory);
		g_free(type);
		g_free(caption);
		g_free(source);
		g_free(directory);
		valid = gtk_tree_model_iter_next(remote_model, &iter);
	}

	valid = gtk_tree_model_get_iter_first(local_model, &iter);
	while (valid) {
		gtk_tree_model_get(local_model, &iter,
				   COLUMN_TYPE, &type,
				   COLUMN_CAPTION, &caption,
				   COLUMN_SOURCE, &source,
				   COLUMN_DIRECTORY, &directory, -1);

		mod_mgr_add_source("DIRSource",
				   type, caption, source, directory);
		g_free(type);
		g_free(caption);
		g_free(source);
		g_free(directory);
		valid = gtk_tree_model_iter_next(local_model, &iter);
	}

	load_source_treeviews();
}


/******************************************************************************
 * Name
 *   on_fileselection_local_source_response
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_fileselection_local_source_response (GtkDialog * dialog,
 *                                gint response_id, GtkFileSelection * filesel)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void on_fileselection_local_source_response(GtkDialog * dialog,
					    gint response_id,
					    GtkFileSelection * filesel)
{
	GtkTreeIter iter;
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_local));

	switch (response_id) {
	case GTK_RESPONSE_OK:
		gtk_list_store_append(GTK_LIST_STORE(model), &iter);
		gtk_list_store_set(GTK_LIST_STORE(model), &iter,
				   COLUMN_TYPE, "DIR",
				   COLUMN_CAPTION,
				   gtk_file_selection_get_filename
				   (filesel), COLUMN_SOURCE, "[local]",
				   COLUMN_DIRECTORY,
				   gtk_file_selection_get_filename
				   (filesel), -1);
		save_sources();
		GS_warning((gtk_file_selection_get_filename(filesel)));

		break;
	case GTK_RESPONSE_CANCEL:


		break;

	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}


/******************************************************************************
 * Name
 *   create_fileselection_local_source
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   GtkWidget* create_fileselection_local_source (void)
 *
 * Description
 *   
 *
 * Return value
 *   GtkWidget*
 */

GtkWidget *create_fileselection_local_source(void)
{
	GtkWidget *fileselection_local_source;
	GtkWidget *ok_;
	GtkWidget *cancel_;

	fileselection_local_source =
	    gtk_file_selection_new(_("Select Local Source"));
	gtk_container_set_border_width(GTK_CONTAINER
				       (fileselection_local_source),
				       10);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION
					       (fileselection_local_source));

	gtk_widget_set_sensitive(GTK_FILE_SELECTION
				 (fileselection_local_source)->
				 file_list, FALSE);
	gtk_widget_hide(GTK_FILE_SELECTION(fileselection_local_source)->
			selection_entry);

	ok_ =
	    GTK_FILE_SELECTION(fileselection_local_source)->ok_button;
	gtk_widget_show(ok_);
	GTK_WIDGET_SET_FLAGS(ok_, GTK_CAN_DEFAULT);

	cancel_ =
	    GTK_FILE_SELECTION(fileselection_local_source)->
	    cancel_button;
	gtk_widget_show(cancel_);
	GTK_WIDGET_SET_FLAGS(cancel_, GTK_CAN_DEFAULT);

	g_signal_connect((gpointer) fileselection_local_source,
			 "response",
			 G_CALLBACK
			 (on_fileselection_local_source_response),
			 (GtkFileSelection *)
			 fileselection_local_source);

	return fileselection_local_source;
}


/******************************************************************************
 * Name
 *   on_dialog_destroy
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_dialog_destroy(GtkObject * object, gpointer user_data)
 *
 * Description
 *   when the dialog is destroyed and changes have been made 
 *   Sword is shut down and restarted
 *   module lists are cleared and refilled with current modules
 *   the sidebar module tree is reloaded 
 *
 * Return value
 *   void
 */

void on_dialog_destroy(GtkObject * object, gpointer user_data)
{
	GS_message(("on_destroy"));
	if(remote_source) {
	        g_free(remote_source);
		remote_source = NULL;
	}
	
	mod_mgr_shut_down();
	while (gtk_events_pending())
		gtk_main_iteration();
	if(have_changes && need_update) {
		main_update_module_lists();
		main_load_module_tree(sidebar.module_list);
	}
}


void on_refresh_clicked(GtkButton * button, gpointer  user_data)
{
	response_refresh();
}

void on_install_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(INSTALL);
}

void on_remove_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(REMOVE);
}

void on_archive_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(ARCHIVE);
}

void on_index_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(FASTMOD);
}

void on_delete_index_clicked(GtkButton * button, gpointer  user_data)
{
	remove_install_wrapper(DELFAST);
}

void on_cancel_clicked(GtkButton * button, gpointer  user_data)
{
	mod_mgr_terminate();
	while (gtk_events_pending())
		gtk_main_iteration();
}


/******************************************************************************
 * Name
 *   on_mod_mgr_response
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_mod_mgr_response(GtkDialog * dialog, gint response_id, gpointer user_data)
 *
 * Description
 *   these are local defines at the top of this file.
 *
 * Return value
 *   void
 */

void on_mod_mgr_response(GtkDialog * dialog, gint response_id, gpointer user_data)
{
	GList *modules = NULL;

	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
		mod_mgr_terminate();
		while (gtk_events_pending())
			gtk_main_iteration();
		break;
	case GTK_RESPONSE_REFRESH:
		response_refresh();
		break;
	case GTK_RESPONSE_CLOSE:
		response_close();
		break;
	case GTK_RESPONSE_INSTALL:
		remove_install_wrapper(INSTALL);
		break;
	case GTK_RESPONSE_REMOVE:
		remove_install_wrapper(REMOVE);
		break;
	case GTK_RESPONSE_ARCHIVE:
		remove_install_wrapper(ARCHIVE);
		break;
	case GTK_RESPONSE_FASTMOD:
		remove_install_wrapper(FASTMOD);
		break;
	case GTK_RESPONSE_DELFAST:
		remove_install_wrapper(DELFAST);
		break;
	}
}


/******************************************************************************
 * Name
 *   on_button1_clicked
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_button1_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   add local source
 *
 * Return value
 *   void
 */

void on_button5_clicked(GtkButton * button, gpointer  user_data)
{
	GtkWidget *dlg = create_fileselection_local_source();
	gtk_widget_show(dlg);
	gtk_dialog_run(GTK_DIALOG(dlg));
}


/******************************************************************************
 * Name
 *   on_button2_clicked
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_button2_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   remove local source
 *
 * Return value
 *   void
 */

void on_button6_clicked(GtkButton * button, gpointer user_data)
{
	gint test;
	GS_DIALOG *yes_no_dialog;
	gchar *name_string;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *type = NULL;
	gchar *source = NULL;
	gchar *directory = NULL;
	GString *str = g_string_new(NULL);
	GtkTreeModel *model;

	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_local));
	if (!gtk_tree_selection_get_selected
	    (selection, &model, &selected))
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
			   COLUMN_TYPE, &type,
			   COLUMN_CAPTION, &caption,
			   COLUMN_SOURCE, &source,
			   COLUMN_DIRECTORY, &directory, -1);
	name_string = caption;

	yes_no_dialog = gui_new_dialog();
	yes_no_dialog->stock_icon = GTK_STOCK_DIALOG_WARNING;
	yes_no_dialog->title = _("Bookmark");
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s|%s|%s|%s",
			_("Remove the selected source"),
			caption, type, source, directory);
	yes_no_dialog->label_top = str->str;
	yes_no_dialog->yes = TRUE;
	yes_no_dialog->no = TRUE;

	test = gui_alert_dialog(yes_no_dialog);
	if (test == GS_YES) {
		gtk_list_store_remove(GTK_LIST_STORE(model), &selected);
		save_sources();
	}
	g_free(yes_no_dialog);
	g_free(type);
	g_free(caption);
	g_free(source);
	g_free(directory);
	g_string_free(str, TRUE);

}


/******************************************************************************
 * Name
 *   on_button3_clicked
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_button3_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   add remote source
 *
 * Return value
 *   void
 */

void on_button7_clicked(GtkButton * button, gpointer user_data)
{
	gint test;
	GS_DIALOG *dialog;
	GtkTreeIter iter;
	GString *str = g_string_new(NULL);
	GList *tmp;
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2),TRUE);
	gtk_widget_hide(button1);
	
	GtkTreeModel *model =
	    gtk_tree_view_get_model(GTK_TREE_VIEW(treeview_remote));
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>",
			_("Enter a remote source"));
	dialog = gui_new_dialog();
	dialog->stock_icon = GTK_STOCK_DIALOG_INFO;
	dialog->label_top = str->str;
	dialog->label1 = _("Caption:");
	dialog->label2 = _("Type:");
	dialog->label3 = _("Host:");
	dialog->label4 = _("Directory:");
	dialog->text1 = g_strdup("Crosswire");
	dialog->text2 = g_strdup("FTP");
	dialog->text3 = g_strdup("ftp.crosswire.org");
	dialog->text4 = g_strdup("/pub/sword/raw");
	dialog->cancel = TRUE;
	dialog->ok = TRUE;

	test = gui_gs_dialog(dialog);
	if (test != GS_OK) {
		g_free(dialog->text1);
		g_free(dialog->text2);
		g_free(dialog->text3);
		g_free(dialog->text4);
		g_free(dialog);
		g_string_free(str, TRUE);
		return;
	}

	tmp = mod_mgr_list_remote_sources();
	while (tmp) {
		if (!strcmp(((MOD_MGR_SOURCE *)tmp->data)->caption,
			    dialog->text1)) {
			gui_generic_warning(_("A source by that name already exists."));
			return;
		}
		tmp = g_list_next(tmp);
	}

	gtk_list_store_append(GTK_LIST_STORE(model), &iter);
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			   COLUMN_TYPE, dialog->text2,
			   COLUMN_CAPTION, dialog->text1,
			   COLUMN_SOURCE, dialog->text3,
			   COLUMN_DIRECTORY, dialog->text4, -1);
	
	if(remote_source)
	        g_free(remote_source);
	remote_source = g_strdup(dialog->text1);
	
	g_free(dialog->text1);
	g_free(dialog->text2);
	g_free(dialog->text3);
	g_free(dialog->text4);
	g_free(dialog);
	g_string_free(str, TRUE);
	save_sources();	
	while (gtk_events_pending())
		gtk_main_iteration();
	mod_mgr_shut_down();
	mod_mgr_init(destination);
}


/******************************************************************************
 * Name
 *   on_button8_clicked
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void on_button8_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   remove remote source
 *
 * Return value
 *   void
 */

void on_button8_clicked(GtkButton * button, gpointer user_data)
{
	gint test;
	GS_DIALOG *yes_no_dialog;
	gchar *name_string;
	GtkTreeSelection *selection;
	GtkTreeIter selected;
	GtkTreeIter iter;
	gchar *caption = NULL;
	gchar *type = NULL;
	gchar *source = NULL;
	gchar *directory = NULL;
	GString *str = g_string_new(NULL);
	GtkTreeModel *model;

	selection =
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview_remote));
	if (!gtk_tree_selection_get_selected
	    (selection, &model, &selected))
		return;
	gtk_tree_model_get(model, &selected,
			   COLUMN_TYPE, &type,
			   COLUMN_CAPTION, &caption,
			   COLUMN_SOURCE, &source,
			   COLUMN_DIRECTORY, &directory, -1);
	name_string = caption;

	yes_no_dialog = gui_new_dialog();
	yes_no_dialog->stock_icon = GTK_STOCK_DIALOG_WARNING;
	yes_no_dialog->title = _("Bookmark");
	g_string_printf(str,
			"<span weight=\"bold\">%s</span>\n\n%s|%s|%s|%s",
			_("Remove the selected source"),
			caption, type, source, directory);
	yes_no_dialog->label_top = str->str;
	yes_no_dialog->yes = TRUE;
	yes_no_dialog->no = TRUE;

	test = gui_alert_dialog(yes_no_dialog);
	if (test == GS_YES) {
		gtk_list_store_remove(GTK_LIST_STORE(model), &selected);
		save_sources();
	}
	g_free(yes_no_dialog);
	g_free(type);
	g_free(caption);
	g_free(source);
	g_free(directory);
	g_string_free(str, TRUE);
}


/******************************************************************************
 * Name
 *   on_treeview1_button_release_event
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   gboolean on_treeview1_button_release_event(GtkWidget * widget,
 *                           GdkEventButton * event, gpointer user_data)
 *
 * Description
 *   button release in main treeview 
 *   change notebook page to 'sel' returned from selection
 *   show hide dialog responce buttons as needed
 *
 * Return value
 *   void
 */

gboolean on_treeview1_button_release_event(GtkWidget * widget,
                            GdkEventButton * event, gpointer user_data)
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
				gtk_widget_hide(button1);
				gtk_widget_hide(button2);
				gtk_widget_hide(button3);
				gtk_widget_hide(button_arch);
				gtk_widget_hide(button_idx);
				gtk_widget_hide(button_delidx);
				break;
			case 2:
				if (GTK_TOGGLE_BUTTON(radiobutton2)->
				    active)
					gtk_widget_show(button1);
				else
					gtk_widget_hide(button1);
				gtk_widget_hide(button2);
				gtk_widget_hide(button3);
				gtk_widget_hide(button_arch);
				gtk_widget_hide(button_idx);
				gtk_widget_hide(button_delidx);
				break;
			case 3:
				if (GTK_TOGGLE_BUTTON(radiobutton2)->
				    active)
					gtk_widget_show(button1);
				else
					gtk_widget_hide(button1);
				gtk_widget_show(button2);
				gtk_widget_hide(button3);
				gtk_widget_hide(button_arch);
				gtk_widget_hide(button_idx);
				gtk_widget_hide(button_delidx);
				break;
			case 4:
				gtk_widget_show(button3);
				gtk_widget_show(button_arch);
				gtk_widget_show(button_idx);
				gtk_widget_show(button_delidx);
				gtk_widget_hide(button1);
				gtk_widget_hide(button2);
				break;
			}

		}
	}
  return FALSE;
}


void
on_filechooserdialog_selection_changed (GtkFileChooser  *filechooser,
                                        gpointer         user_data)
{

}


void
on_filechooserdialog_response          (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{

}


static
void setup_treeview_main(GtkTreeView * tree_view)
{
	GtkTreeModel *model;
	
	model = create_model_to_first();	
	gtk_tree_view_set_model(tree_view, model);
	add_columns_to_first(tree_view);
	gtk_tree_view_expand_all(tree_view); 	
	
	g_signal_connect(tree_view, "button_release_event",
			 G_CALLBACK(on_treeview1_button_release_event), NULL);
}

static
void setup_treeviews_local_remote(GtkTreeView * local, GtkTreeView * remote)
{
	GtkTreeModel *model;
	
	model = create_remote_source_treeview_model();	
	gtk_tree_view_set_model(local, model);
	add_columns_to_remote_treeview(local);
	model = create_remote_source_treeview_model();	
	gtk_tree_view_set_model(remote, model);
	add_columns_to_remote_treeview(remote);
}

static
void setup_treeviews_install_remove(GtkTreeView * install, GtkTreeView * remove)
{
	GtkTreeModel *model;
	
	model = create_model();
	gtk_tree_view_set_model(install, model);
	add_columns(install, FALSE);
	model =  create_model();	
	gtk_tree_view_set_model(remove, model);
	add_columns(remove, TRUE);
}

static
void set_combobox(GtkComboBox * combo)
{	
	GtkListStore *store;
	
	store = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(combo, GTK_TREE_MODEL(store));
	gtk_combo_box_entry_set_text_column(GTK_COMBO_BOX_ENTRY(combo), 0);
}


static
void setup_dialog_action_area(GtkDialog * dialog)
{	
	GtkWidget *alignment1;
	GtkWidget *hbox1;
	GtkWidget *image1;
	GtkWidget *label1;
	GtkWidget *dialog_action_area1 = GTK_DIALOG (dialog)->action_area;
	
	gtk_widget_show (dialog_action_area1);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

	/* responce buttons */
/* close */	
	button_close = gtk_button_new_from_stock ("gtk-close");
	gtk_widget_show (button_close);
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button_close, GTK_RESPONSE_CLOSE);
	GTK_WIDGET_SET_FLAGS (button_close, GTK_CAN_DEFAULT);
/* connect/refresh */	
	button1 = gtk_button_new_from_stock ("gtk-connect");
	gtk_box_pack_start(GTK_BOX(dialog_action_area1),button1,FALSE, FALSE,0);
	g_signal_connect(button1, "clicked", G_CALLBACK(on_refresh_clicked), NULL);
	//gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button1, 301);
	//GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT); 
/* install */	
	button2 = gtk_button_new ();
	gtk_box_pack_start(GTK_BOX(dialog_action_area1),button2,FALSE, FALSE,0);
	g_signal_connect(button2, "clicked", G_CALLBACK(on_install_clicked), NULL);
	//gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button2, 303);
	//GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

	alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (alignment1);
	gtk_container_add (GTK_CONTAINER (button2), alignment1);

	hbox1 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox1);
	gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

	image1 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (image1);
	gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

	label1 = gtk_label_new_with_mnemonic (_("Install"));
	gtk_widget_show (label1);
	gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);

  
/* remove */	
	button3 =  gtk_button_new_from_stock ("gtk-remove");
	gtk_box_pack_start(GTK_BOX(dialog_action_area1),button3,FALSE, FALSE,0);
	g_signal_connect(button3, "clicked", G_CALLBACK(on_remove_clicked), NULL);
	//gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button3, 302);
	//GTK_WIDGET_SET_FLAGS (button3, GTK_CAN_DEFAULT);

/* archive */	
	button_arch =  gtk_button_new_from_stock ("gtk-save");
	gtk_box_pack_start(GTK_BOX(dialog_action_area1),button_arch,FALSE, FALSE,0);
	g_signal_connect(button_arch, "clicked", G_CALLBACK(on_archive_clicked), NULL);
	//gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button_arch, 304);
	//GTK_WIDGET_SET_FLAGS (button_arch, GTK_CAN_DEFAULT);

/* index */	
	button_idx =  gtk_button_new_from_stock ("gtk-save-as");
	gtk_box_pack_start(GTK_BOX(dialog_action_area1),button_idx,FALSE, FALSE,0);
	g_signal_connect(button_idx, "clicked", G_CALLBACK(on_index_clicked), NULL);
	//gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button_idx, 305);
	//GTK_WIDGET_SET_FLAGS (button_idx, GTK_CAN_DEFAULT);

/* index */	
	button_delidx =  gtk_button_new_from_stock ("edit-delete");
	gtk_box_pack_start(GTK_BOX(dialog_action_area1),button_delidx,FALSE, FALSE,0);
	g_signal_connect(button_delidx, "clicked", G_CALLBACK(on_delete_index_clicked), NULL);
	//gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button_delidx, 306);
	//GTK_WIDGET_SET_FLAGS (button_delidx, GTK_CAN_DEFAULT);

/* cancel */	
	button_cancel =  gtk_button_new_from_stock ("gtk-cancel");
	gtk_box_pack_start(GTK_BOX(dialog_action_area1),button_cancel,FALSE, FALSE,0);
	g_signal_connect(button_cancel, "clicked", G_CALLBACK(on_cancel_clicked), NULL);
	//gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button_cancel, GTK_RESPONSE_CANCEL);
	//GTK_WIDGET_SET_FLAGS (button_cancel, GTK_CAN_DEFAULT);
	
}

static void setup_ui_labels()
{

	GString *str = g_string_new(NULL);
	gchar *path = NULL;
	gchar *homedir = NULL;
		
	if((homedir = getenv("HOME")) == NULL) {
		
		
	}	
	have_changes = FALSE;
	g_string_printf(str, "%s/%s", homedir,
			".sword/InstallMgr/InstallMgr.conf");
	if (!mod_mgr_check_for_file(str->str)) {
		have_configs = FALSE;
		mod_mgr_init_config();
	}
	have_configs = TRUE;
	mod_mgr_init(NULL);
	g_string_printf(str, "%s/%s", homedir, ".sword");
	gtk_label_set_text(GTK_LABEL(label_home), str->str);

	path = main_module_mgr_get_path_to_mods();
	gtk_label_set_text(GTK_LABEL(label_system), path);
	if (access(path, W_OK) == -1) {
		GS_print(("%s is write protected\n", path));
		gtk_widget_set_sensitive(label_system, FALSE);
		gtk_widget_set_sensitive(radiobutton4, FALSE);
	} else {
		gtk_widget_set_sensitive(label_system, TRUE);
		gtk_widget_set_sensitive(radiobutton4, TRUE);
	}
	create_pixbufs();
	load_source_treeviews();
	g_string_free(str, TRUE);
	g_free(path);
}


static void
on_comboboxentry_remote_changed(GtkComboBox *combobox, gpointer user_data)
{
	if(remote_source)
		g_free(remote_source);
	remote_source = g_strdup(gtk_entry_get_text(GTK_ENTRY(GTK_BIN(combobox)->child)));
}

static
GtkWidget *create_module_manager_dialog(gboolean first_run)
{
	gchar *glade_file;
	GtkWidget *dialog_vbox;
	GtkWidget *hpaned;
	GtkWidget *chooser;
	GtkWidget *button5;
	GtkWidget *button6;
	GtkWidget *button7;
	GtkWidget *button8;
	GtkWidget *widget;
	gint index = 0;
	GString *str = g_string_new(NULL);
	gchar *homedir = NULL;
	gchar *path = NULL;

	glade_file = gui_general_user_file ("module-manager.glade", FALSE);
	g_return_if_fail(glade_file != NULL);
	GS_message((glade_file));
	
	/* build the widget */
	if(first_run) {		
		gxml = glade_xml_new (glade_file, "hpaned1", NULL);       //"dialog", NULL);
	} else {
		gxml = glade_xml_new (glade_file, "dialog", NULL);
		
	}
	//gxml = glade_xml_new (glade_file, "hpaned1", NULL);       //"dialog", NULL);
	g_free (glade_file);
	g_return_if_fail (gxml != NULL);

	/* lookup the root widget */
	if(first_run) {		
		hpaned = glade_xml_get_widget (gxml, "hpaned1"); 
		
		dialog = gtk_dialog_new();
		gtk_widget_show(dialog);
		gtk_widget_set_size_request(dialog, 500, 341);
		dialog_vbox = GTK_DIALOG(dialog)->vbox;
		gtk_widget_show(dialog_vbox);
		gtk_box_pack_start(GTK_BOX(dialog_vbox),hpaned, TRUE, TRUE, 6);
		widget = glade_xml_get_widget (gxml, "scrolledwindow1");
		gtk_widget_hide(widget);
		/* setup dialog action area */
		setup_dialog_action_area(GTK_DIALOG (dialog));
	} else {
		dialog =  glade_xml_get_widget (gxml, "dialog");
		/* responce buttons */
		button_close = glade_xml_get_widget (gxml, "button1"); /* close */
		button_cancel = glade_xml_get_widget (gxml, "button12"); /* close */
		button1 = glade_xml_get_widget (gxml, "button2"); /* refresh */
		button2 = glade_xml_get_widget (gxml, "button3"); /* install */
		button3 = glade_xml_get_widget (gxml, "button9"); /* remove */
		button_arch = glade_xml_get_widget (gxml, "button13"); /* archive */
		button_idx = glade_xml_get_widget (gxml, "button14"); /* index */
		button_delidx = glade_xml_get_widget (gxml, "button15"); /* delete index */
	}
		
	g_signal_connect(dialog, "response",
			 G_CALLBACK(on_mod_mgr_response), NULL);
	g_signal_connect(dialog, "destroy",
			 G_CALLBACK(on_dialog_destroy), NULL);
	
/*	g_signal_connect(, "",
			 G_CALLBACK(), NULL);
*/
	
	/* progress bars */
	progressbar_refresh = glade_xml_get_widget (gxml, "progressbar1"); /* refresh */
			
	/* treeviews */
	treeview1 = glade_xml_get_widget (gxml, "treeview1");
	setup_treeview_main(GTK_TREE_VIEW(treeview1));
	
	treeview_local = glade_xml_get_widget (gxml, "treeview2");
	treeview_remote = glade_xml_get_widget (gxml, "treeview3");
	setup_treeviews_local_remote(GTK_TREE_VIEW(treeview_local), GTK_TREE_VIEW(treeview_remote));
	
	treeview = glade_xml_get_widget (gxml, "treeview4");
	treeview2 = glade_xml_get_widget (gxml, "treeview5");
	setup_treeviews_install_remove(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW(treeview2));
	
	/* notebook */
	notebook1 = glade_xml_get_widget (gxml, "notebook1");		
	g_signal_connect(notebook1, "switch_page",
			 G_CALLBACK(on_notebook1_switch_page), NULL);	
	if(first_run) gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook1),1);
	/* labels */
	label_home = glade_xml_get_widget (gxml, "label_home");
	label_system = glade_xml_get_widget (gxml, "label_sword_sys");
	
	
	/* sources buttons */
	button5 = glade_xml_get_widget (gxml, "button5"); /* close */
	button6 = glade_xml_get_widget (gxml, "button6"); /* close */
	button7 = glade_xml_get_widget (gxml, "button7"); /* refresh */
	button8 = glade_xml_get_widget (gxml, "button8"); /* install */
	g_signal_connect(button5, "clicked",
			 G_CALLBACK(on_button5_clicked), NULL);
	g_signal_connect(button6, "clicked",
			 G_CALLBACK(on_button6_clicked), NULL);
	g_signal_connect(button7, "clicked",
			 G_CALLBACK(on_button7_clicked), NULL);
	g_signal_connect(button8, "clicked",
			 G_CALLBACK(on_button8_clicked), NULL);
	
	/* combo box entrys */
	combo_entry1 = glade_xml_get_widget (gxml, "comboboxentry1"); /* local soruce */
	set_combobox(GTK_COMBO_BOX(combo_entry1));
	combo_entry2 = glade_xml_get_widget (gxml, "comboboxentry2"); /* remote source */
	set_combobox(GTK_COMBO_BOX(combo_entry2));
	g_signal_connect ((gpointer) combo_entry2, "changed",
                    G_CALLBACK (on_comboboxentry_remote_changed),
                    NULL);
	/* radio buttons */
	radiobutton_source = glade_xml_get_widget (gxml, "radiobutton1"); /* local */
	radiobutton2 = glade_xml_get_widget (gxml, "radiobutton2"); /* remote */
	radiobutton_dest = glade_xml_get_widget (gxml, "radiobutton3"); /* homedir */
	radiobutton4 = glade_xml_get_widget (gxml, "radiobutton4"); /* homedir */
/*	g_signal_connect(radiobutton_source, "toggled",
			 G_CALLBACK(), NULL);
			 */
	g_signal_connect(radiobutton2, "toggled",
			 G_CALLBACK(on_radiobutton2_toggled), NULL);
	if(first_run) 
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radiobutton2),TRUE);
	
/*	g_signal_connect(radiobutton_dest, "toggled",
			 G_CALLBACK(), NULL);
	g_signal_connect(radiobutton4, "toggled",
			 G_CALLBACK(), NULL);
*/	
	setup_ui_labels();
		
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
	need_update = TRUE;
	create_module_manager_dialog(FALSE);
}


/******************************************************************************
 * Name
 *   gui_open_mod_mgr_initial_run
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void gui_open_mod_mgr_initial_run(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_open_mod_mgr_initial_run(void)
{
	GtkWidget *dlg;
	need_update = FALSE;
	//const char *lang = getenv("LANG");	
	//if (!lang) lang="C";
	//main_module_mgr_set_sword_locale(lang);
	//OLD_CODESET = "iso8859-1";
	first_time_user = TRUE;
	dlg = create_module_manager_dialog(TRUE);
	gtk_dialog_run((GtkDialog *) dlg);
	gtk_widget_destroy(dlg);
	first_time_user = FALSE;
}

/******************************************************************************
 * Name
 *   gui_update_install_status
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *  void gui_update_install_status(glong total, glong done, const gchar * message) 
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_install_status(glong total, glong done,
			       const gchar * message)
{
	gchar *buf;
	
	buf = g_strdup_printf("%s: %s",current_mod,message);
	gui_set_progressbar_text(progressbar_refresh,buf);
/*	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progressbar_refresh), buf);
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}*/
	g_free(buf);
}


/******************************************************************************
 * Name
 *   gui_update_install_progressbar
 *
 * Synopsis
 *   #include "gui/mod_mgr.h"
 *
 *   void gui_update_install_progressbar(gdouble fraction)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_update_install_progressbar(gdouble fraction)
{
	gui_set_progressbar_fraction(progressbar_refresh, fraction);
	/*gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar_refresh), fraction);
	while (gtk_events_pending()) {
		gtk_main_iteration();
	}*/
}
