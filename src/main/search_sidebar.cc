/*
 * Xiphos Bible Study Tool
 * search_sidebar.cc - glue  (: very sticky :)
 *
 * Copyright (C) 2004-2017 Xiphos Developer Team
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
#include <regex.h>
#include <ctype.h>
#include <swmodule.h>

#include "main/search_dialog.h"
#include "main/search_sidebar.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "gui/search_dialog.h"
#include "gui/search_sidebar.h"
#include "gui/sidebar.h"
#include "gui/widgets.h"
#include "gui/dialog.h"
#include "gui/utilities.h"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

#ifdef HAVE_DBUS
#include "gui/ipc.h"
#endif

#define SEARCHING N_("Searching the ")
#define SMODULE N_(" Module")
#define FINDS N_("found in ")

static BackEnd *backendSearch;

int search_dialog;

/**********************************************************************/
/****************************** sidebar search ************************/
/**********************************************************************/

/******************************************************************************
 * Name
 *    fill_search_results_list
 *
 * Synopsis
 *   #include "sidebar_search.h"
 *
 *   void fill_search_results_list(int finds)
 *
 * Description
 *   display a list of keys found during search
 *
 * Return value
 *   void
 */

static void fill_search_results_list(int finds)
{
	gchar buf[256];
	const gchar *key_buf = NULL;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	GtkTreePath *path;
	gchar *buf1 = _("matches");
	RESULTS *list_item;
	gchar *num;

	if (!backendSearch)
		main_init_sidebar_search_backend();

	is_search_result = TRUE;

	if (list_of_verses) {
		GList *chaser = list_of_verses;
		while (chaser) {
			list_item = (RESULTS *)chaser->data;
			g_free(list_item->module);
			g_free(list_item->key);
			g_free(list_item);
			chaser = g_list_next(chaser);
		}
		g_list_free(list_of_verses);
		list_of_verses = NULL;
	}

	gtk_widget_set_sensitive(sidebar.menu_item_save_search, FALSE);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sidebar.results_list));
	list_store = gtk_list_store_new(1, G_TYPE_STRING);

	backendSearch->set_listkey_position((char)1); /* TOP */
	while ((key_buf = backendSearch->get_next_listkey()) != NULL) {
		gchar *tmpbuf = (gchar *)key_buf;
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 0,
				   tmpbuf, -1);
		list_item = g_new(RESULTS, 1);
		list_item->module = g_strdup(settings.sb_search_mod);
		list_item->key = g_strdup(tmpbuf);
		list_of_verses = g_list_append(list_of_verses,
					       (RESULTS *)list_item);
	}

	model = GTK_TREE_MODEL(list_store);
	gtk_tree_view_set_model(GTK_TREE_VIEW(sidebar.results_list), model);

	num = main_format_number(finds);
	sprintf(buf, "%s %s", num, buf1);
	g_free(num);
	gui_set_statusbar(buf);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar), 3);
	/* cleanup progress bar */
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(ss.progressbar_search),
				      0.0);
	/* display first item in list by selection row*/
	if (!gtk_tree_model_get_iter_first(model, &iter))
		return;

	gtk_widget_set_sensitive(sidebar.menu_item_save_search, TRUE);
	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_selection_select_path(selection,
				       path);
	gtk_tree_path_free(path);
	gui_verselist_button_release_event(NULL, NULL, NULL);
	return;
}

#ifdef HAVE_DBUS
GList *get_list_of_references()
{
	GList *tmp = list_of_verses;
	GList *references = NULL;
	while (tmp) {
		RESULTS *list_item = (RESULTS *)tmp->data;
		references = g_list_append(references, list_item->key);
		tmp = g_list_next(tmp);
	}
	return g_list_first(references);
}
#endif

/******************************************************************************
 * Name
 *    main_do_sidebar_search
 *
 * Synopsis
 *    main_do_sidebar_search(gpointer user_data)
 *
 * Description
 *   search from sidebar
 *
 * Return value
 *   void
 */

void main_do_sidebar_search(gpointer user_data)
{
	GString *new_search = g_string_new(NULL);
	gint search_params, finds;
	const char *search_string = NULL;
	char *search_module;

	if (!backendSearch)
		main_init_sidebar_search_backend();

	gtk_widget_set_sensitive(sidebar.menu_item_save_search, FALSE);
	search_dialog = FALSE;
	search_string = gtk_entry_get_text(GTK_ENTRY(ss.entrySearch));

	if (strlen(search_string) < 1)
		return;

	/* text -vs- commentary search selection. */
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ss.radiobutton_search_text))) {
		strcpy(settings.sb_search_mod,
		       settings.MainWindowModule);
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ss.radiobutton_search_comm))) {
		if (!settings.CommWindowModule ||
		    (strlen(settings.CommWindowModule) == 0)) {
			gui_generic_warning(_("There is no commentary module."));
			return;
		}
		strcpy(settings.sb_search_mod, settings.CommWindowModule);
	}

	search_module = settings.sb_search_mod;

	backendSearch->clear_scope();

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ss.rrbUseBounds))) {
		gchar *str;
		backendSearch->clear_search_list();
		str = g_strdup_printf("%s - %s",
#ifdef USE_GTK_3
				      gtk_combo_box_text_get_active_text((GtkComboBoxText *)
									 ss.entryLower),
				      gtk_combo_box_text_get_active_text((GtkComboBoxText *)
									 ss.entryUpper));
#else
				      gtk_combo_box_get_active_text(GTK_COMBO_BOX(ss.entryLower)),
				      gtk_combo_box_get_active_text(GTK_COMBO_BOX(ss.entryUpper)));
#endif
		backendSearch->set_range(settings.MainWindowModule, str);
		backendSearch->set_scope2range();
		g_free(str);
	}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ss.rbLastSearch)))
		backendSearch->set_scope2last_search();

	snprintf(settings.searchText, 255, "%s", search_string);

	settings.searchType =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ss.rbRegExp)) ? 0 : gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ss.rbPhraseSearch)) ? -1 : -2;

	if (settings.searchType == -2)
		settings.searchType = backendSearch->check_for_optimal_search(search_module);

	// if we do lucene search (-4), we assume AND for simple sidebar search.
	// therefore, we must prepend '+' to each word to force that semantic.
	if (settings.searchType == -4) {
		g_string_append_c(new_search, '+');
		while (*search_string) {
			while (((*search_string) != ' ') && ((*search_string) != '\0'))
				g_string_append_c(new_search, *(search_string++));
			if ((*search_string) == ' ') {
				g_string_append_c(new_search, ' ');
				search_string++;
				if ((*search_string) != '\0')
					g_string_append_c(new_search, '+');
			}
		}
		search_string = new_search->str;
	}

	search_params =
	    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ss.ckbCaseSensitive)) ? 0 : REG_ICASE;

	terminate_search = FALSE;
	search_active = TRUE;

	// must ensure that no accents or vowel points are enabled.
	SWMgr *mgr = backendSearch->get_mgr();
	mgr->setGlobalOption("Greek Accents", "Off");
	mgr->setGlobalOption("Hebrew Vowel Points", "Off");
	mgr->setGlobalOption("Arabic Vowel Points", "Off");

	finds = backendSearch->do_module_search(search_module,
						mgr->getModule(search_module)->stripText(search_string),
						settings.searchType,
						search_params,
						FALSE);
	g_string_free(new_search, TRUE);

	search_active = FALSE;

	fill_search_results_list(finds);

#ifdef HAVE_DBUS
	IpcObject *obj = ipc_get_main_ipc();
	if (obj) {
		obj->references = get_list_of_references();
		ipc_object_search_performed(obj, settings.searchText,
					    &finds, NULL);
	}
#endif
}

void main_sidebar_perscomm_dump(void)
{
	if (!backendSearch)
		main_init_sidebar_search_backend();

	strcpy(settings.sb_search_mod, settings.MainWindowModule);
	backendSearch->clear_scope();
	backendSearch->clear_search_list();
	fill_search_results_list(backendSearch->do_module_search
				 /* personal commentary */ (settings.CommWindowModule,
							    /* find two-plus characters */ "..+",
							    /* regexp */ 0,
							    /* case is irrelevant */ 0,
							    /* happening in sidebar */ FALSE));
}

void main_init_sidebar_search_backend(void)
{
	if (!backendSearch) {
		backendSearch = new BackEnd();
		main_search_sidebar_fill_bounds_combos();
	}
}

void main_delete_sidebar_search_backend(void)
{
	delete backendSearch;
}

void main_search_sidebar_fill_bounds_combos(void)
{
	char *book = NULL;
	int i = 0;

	if (!backendSearch)
		main_init_sidebar_search_backend();

	char *module_name = settings.MainWindowModule;
	SWModule *mod = backendSearch->get_SWModule(module_name);
	if (!mod)
		return;

	VerseKey *key = (VerseKey *)mod->createKey();

	GtkTreeModel *upper_model = gtk_combo_box_get_model(
	    GTK_COMBO_BOX(ss.entryUpper));
	gtk_list_store_clear(GTK_LIST_STORE(upper_model));
	GtkTreeModel *lower_model = gtk_combo_box_get_model(
	    GTK_COMBO_BOX(ss.entryLower));
	gtk_list_store_clear(GTK_LIST_STORE(lower_model));

	if (backendSearch->module_has_testament(module_name, 1)) {
		while (i < key->BMAX[0]) {
			key->setTestament(1);
			key->setBook(i + 1);
			book = strdup((const char *)key->getBookName());
#ifdef USE_GTK_3
			gtk_combo_box_text_append_text((GtkComboBoxText *)ss.entryUpper, book);
			gtk_combo_box_text_append_text((GtkComboBoxText *)ss.entryLower, book);
#else
			gtk_combo_box_append_text(GTK_COMBO_BOX(ss.entryUpper), book);
			gtk_combo_box_append_text(GTK_COMBO_BOX(ss.entryLower), book);
#endif
			++i;
			g_free(book);
		}
	}

	i = 0;
	if (backendSearch->module_has_testament(module_name, 2)) {
		while (i < key->BMAX[1]) {
			key->setTestament(2);
			key->setBook(i + 1);
			book = strdup((const char *)key->getBookName());
#ifdef USE_GTK_3
			gtk_combo_box_text_append_text((GtkComboBoxText *)ss.entryUpper, book);
			gtk_combo_box_text_append_text((GtkComboBoxText *)ss.entryLower, book);
#else
			gtk_combo_box_append_text(GTK_COMBO_BOX(ss.entryUpper), book);
			gtk_combo_box_append_text(GTK_COMBO_BOX(ss.entryLower), book);
#endif
			++i;
			g_free(book);
		}
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(ss.entryLower), 0);
	gtk_combo_box_set_active(GTK_COMBO_BOX(ss.entryUpper),
				 key->BMAX[0] + key->BMAX[1] - 1);
	delete key;
}

/******************************************************************************
 * Name
 *  search_percent_update
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   void search_percent_update(char percent, void *userData)
 *
 * Description
 *    updates the progress bar during shortcut bar search
 *
 * Return value
 *   void
 */

void main_sidebar_search_percent_update(char percent, void *userData)
{
	char maxHashes = *((char *)userData);
	static char printed = 0;
	if (!backendSearch)
		main_init_sidebar_search_backend();

	if (terminate_search) {
		backendSearch->terminate_search();
	} else {
		while ((((float)percent) / 100) * maxHashes > printed) {
			float num = (float)percent / 100;
			gchar buf[80];

			sprintf(buf, "%f", (((float)percent) / 100));
			gtk_progress_bar_set_fraction((GtkProgressBar *)
						      ss.progressbar_search,
						      num);
			printed++;
		}
	}
	sync_windows();
	printed = 0;
}
