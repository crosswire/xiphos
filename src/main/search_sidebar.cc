/*
 * GnomeSword Bible Study Tool
 * search_sidebar.cc - glue  (: very sticky :)
 *
 * Copyright (C) 2004 GnomeSword Developer Team
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
#include <regex.h>
//#include <swmgr.h>
//#include <versekey.h>


#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>
#ifdef __cplusplus
}
#endif
	

#include "main/search_sidebar.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"
 
#include "gui/search_dialog.h"
#include "gui/search_sidebar.h"
#include "gui/sidebar.h"
#include "gui/widgets.h"
//#include "gui/html.h"
#include "gui/dialog.h"

//#include "backend/module_search.hh"
#include "backend/sword_main.hh"

#define SEARCHING N_("Searching the ")
#define SMODULE N_(" Module")
#define FINDS N_("found in ")		
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"

static BackEnd *backendSearch;
static gboolean is_running = FALSE;

int search_dialog;

static GList *get_current_list(void);
static gchar *get_modlist_string(GList * mods);
static GList *get_custom_list_from_name(const gchar * label);
static void add_ranges(void);
static void add_modlist(void);


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
	char maxHashes = *((char *) userData);
	float num;
	char buf[80];
	static char printed = 0;
	
	while ((((float) percent) / 100) * maxHashes > printed) {
		sprintf(buf, "%f", (((float) percent) / 100));
		num = (float) percent / 100;
		gtk_progress_bar_update((GtkProgressBar *)
				   ss.progressbar_search, num);
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
}



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
	gchar *utf8str, buf[256];
	gchar *tmpbuf;
	const gchar *key_buf = NULL;
	gint i = 0;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	GtkTreePath *path;	
	gchar *buf1 = _("matches");
	RESULTS *list_item;
	
	if(list_of_verses) {
		while(list_of_verses) {
			list_item = (RESULTS*)list_of_verses->data;
			g_free(list_item->module);
			g_free(list_item->key);
			g_free(list_item);
			list_of_verses = g_list_next(list_of_verses);
		}
		g_list_free(list_of_verses);
	}
	list_of_verses = NULL;
	
	gtk_widget_set_sensitive(sidebar.menu_item_save_search,FALSE);
	selection = gtk_tree_view_get_selection
                                          (GTK_TREE_VIEW(sidebar.results_list));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);
	backendSearch->set_listkey_position((char) 1);	/* TOP */
	while ((key_buf = backendSearch->get_next_listkey()) != NULL) {
		tmpbuf = (gchar*) key_buf;
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 0,
					   tmpbuf, -1);
		list_item = g_new(RESULTS,1);
		list_item->module = g_strdup(settings.sb_search_mod);
		list_item->key = g_strdup(tmpbuf);
		list_of_verses = g_list_append(list_of_verses, 
						(RESULTS *) list_item);
		if(key_buf) /* allocated by g_convert() in BackEnd::get_next_listkey() */
			g_free((gchar*)key_buf);
	}
	
	sprintf(buf, "%d %s", finds, buf1);
	gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), buf);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),3);
	/* cleanup progress bar */
	gtk_progress_bar_update(GTK_PROGRESS_BAR(ss.progressbar_search),
				0.0);
	/* display first item in list by selection row*/	
	if(!gtk_tree_model_get_iter_first(model,&iter))
		return;
	
	gtk_widget_set_sensitive(sidebar.menu_item_save_search,TRUE);
	path = gtk_tree_model_get_path(model,&iter);				
	gtk_tree_selection_select_path(selection,
                                             path);
	gtk_tree_path_free(path);	 
	gui_verselist_button_release_event(NULL,NULL,NULL);
}


/******************************************************************************
 * Name
 *    on_search_botton_clicked
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *   void on_search_botton_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   prepare to begin search
 *
 * Return value
 *   void
 */

void main_do_sidebar_search(gpointer user_data)
{
	GString *str;
	int search_params, finds;
	const char *search_string = NULL;
	char *search_module;	
	
	gtk_widget_set_sensitive(sidebar.menu_item_save_search,FALSE);
	search_dialog = FALSE;
	search_string = gtk_entry_get_text(GTK_ENTRY(ss.entrySearch));
	
	if(strlen(search_string) < 1) 
		return;
	
	if (GTK_TOGGLE_BUTTON(ss.radiobutton_search_text)->active) {
		strcpy(settings.sb_search_mod,
		       settings.MainWindowModule);
	}
	else if (GTK_TOGGLE_BUTTON(ss.radiobutton_search_comm)->active) {
		strcpy(settings.sb_search_mod,settings.CommWindowModule);
	} 	
	
	search_module = settings.sb_search_mod;

	backendSearch->clear_scope();

	if (GTK_TOGGLE_BUTTON(ss.rrbUseBounds)->active) {
		backendSearch->clear_search_list();
		str = g_string_new(" ");
		g_string_sprintf(str, "%s - %s",
				 gtk_combo_box_get_active_text(GTK_COMBO_BOX
						    (ss.entryLower)),
				 gtk_combo_box_get_active_text(GTK_COMBO_BOX
						    (ss.entryUpper)));
				 //gtk_entry_get_text(GTK_COMBO_BOX
				//		    (ss.entryUpper)));
		backendSearch->set_range(str->str);
		backendSearch->set_scope2range();
		g_string_free(str, TRUE);
	}

	if (GTK_TOGGLE_BUTTON(ss.rbLastSearch)->active)
		backendSearch->set_scope2last_search();

	sprintf(settings.searchText, "%s", search_string);

	settings.searchType = 
		GTK_TOGGLE_BUTTON(ss.rbRegExp)->active ? 0 :
	    	GTK_TOGGLE_BUTTON(ss.rbPhraseSearch)->active ? -1 : -2;

	if(settings.searchType == -2)
		settings.searchType = backendSearch->check_for_optimal_search(search_module);

		
	search_params =
	    GTK_TOGGLE_BUTTON(ss.ckbCaseSensitive)->active ? 0 : REG_ICASE;

	finds = backendSearch->do_module_search(search_module, search_string,
				 settings.searchType, search_params, FALSE);

	fill_search_results_list(finds);
}
 
void main_sidebar_perscomm_dump(void)
{
	backendSearch->clear_scope();
	backendSearch->clear_search_list();
	fill_search_results_list(backendSearch->do_module_search
		/* personal commentary */	(settings.CommWindowModule,
		/* find one character */	".",
		/* regexp */			0,
		/* case is irrelevant */	0,
		/* happening in sidebar */	FALSE));
}

void main_init_sidebar_search_backend(void)
{
	backendSearch = new BackEnd();
	
}

void main_delete_sidebar_search_backend(void)
{
	delete backendSearch;
	
}

void main_search_sidebar_fill_bounds_combos(void)
{
	VerseKey key; 
	gsize bytes_read;
	gsize bytes_written;
	GError *error = NULL;
	char *book = NULL;
	char *module_name;
	GtkTreeIter iter;
	int i = 0, j = 0, x = 2;
	int testaments;
	
	//module_name = settings.sb_search_mod;
	module_name = g_strdup(settings.MainWindowModule);

	testaments = backendSearch->module_get_testaments(module_name);
	
	GtkTreeModel* upper_model = gtk_combo_box_get_model(
			GTK_COMBO_BOX(ss.entryUpper));
	gtk_list_store_clear(GTK_LIST_STORE(upper_model));
	GtkTreeModel* lower_model = gtk_combo_box_get_model(
			GTK_COMBO_BOX(ss.entryLower));
	gtk_list_store_clear(GTK_LIST_STORE(lower_model));
	
	if (backendSearch->module_has_testament(module_name, 1)) {
		while(i < key.BMAX[0]) { 			
			book = g_convert((const char *) key.books[0][i].name,
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     &error);

			if(book == NULL) {
				g_print ("error: %s\n", error->message);
				g_error_free (error);
				continue;
			}
			gtk_combo_box_append_text(GTK_COMBO_BOX(ss.entryUpper), book);
			gtk_combo_box_append_text(GTK_COMBO_BOX(ss.entryLower), book);
			++i;
			g_free(book);
		}
	}
	
	i = 0;
	if (backendSearch->module_has_testament(module_name, 2)) {
		while(i < key.BMAX[1]) {			
			book = g_convert((const char *) key.books[1][i].name,
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     &error);
			if(book == NULL) {
				g_print ("error: %s\n", error->message);
				g_error_free (error);
				continue;
			}
			gtk_combo_box_append_text(GTK_COMBO_BOX(ss.entryUpper), book);
			gtk_combo_box_append_text(GTK_COMBO_BOX(ss.entryLower), book);
			++i;
			g_free(book);
		}
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(ss.entryLower),0);
	gtk_combo_box_set_active(GTK_COMBO_BOX(ss.entryUpper),65);	
}
