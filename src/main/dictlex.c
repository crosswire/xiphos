/*
 * GnomeSword Bible Study Tool
 * dictlex.c - dictlex glue
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
#include <gtkhtml/gtkhtml.h>


#include "backend/sword.h"
#include "backend/dictlex_.h"
#include "backend/viewdict.h"

#include "gui/dictlex_dialog.h"
#include "gui/_dictlex.h"
#include "gui/cipher_key_dialog.h"

#include "main/settings.h"
#include "main/dictlex.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"

/******************************************************************************
 * externs
 */ 
extern gboolean isrunningSD;    /* is the view dictionary dialog runing */

/******************************************************************************
 * global to this file only 
 */
static GList *dl_list;

/******************************************************************************
 * globals
 */
DL_DATA *cur_d;


/******************************************************************************
 * Name
 *   fill_dictlex_keys
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   GList *fill_dictlex_keys(int mod_num, int count)
 *
 * Description
 *    get a list of dictlex key to fill clist widget by
 *    calling backend_fill_dictlex_keys();
 *
 * Return value
 *   GList*
 */

GList *fill_dictlex_keys(int mod_num, int count)
{
	return backend_fill_dictlex_keys(mod_num, count);
}

/******************************************************************************
 * Name
 *   display_dictionary_page_and_key
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void display_dictionary_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *   calls backend_display_dictlex to display new module and key 
 *
 * Return value
 *   void
 */

void display_dictionary_page_and_key(gint page_num, gchar * key)
{
	backend_display_dictlex(page_num, key);
}

/******************************************************************************
 * Name
 *   set_dictionary_page_and_key
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void set_dictionary_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *   change notebook page and set new key in entry widget
 *
 * Return value
 *   void
 */

void set_dictionary_page_and_key(gint page_num, gchar * key)
{
	DL_DATA *d;

	d = (DL_DATA *) g_list_nth_data(dl_list, page_num);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebookDL),
			      page_num);
	gtk_entry_set_text(GTK_ENTRY(d->entry), key);

}

/******************************************************************************
 * Name
 *   set_page_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void set_page_dictlex(gchar * modname, GList * dl_list)
 *
 * Description
 *    set dictlex notebook page on startup
 *
 * Return value
 *   void
 */

static void set_page_dictlex(gchar * modname, GList * dl_list)
{
	gint page = 0;
	DL_DATA *d = NULL;

	dl_list = g_list_first(dl_list);
	while (dl_list != NULL) {
		d = (DL_DATA *) dl_list->data;
		if (!strcmp(d->modName, modname))
			break;
		++page;
		dl_list = g_list_next(dl_list);
	}

	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebookDL), page);
	gtk_entry_set_text(GTK_ENTRY(d->entry), settings.dictkey);

	settings.dict_last_page = page;
}

/******************************************************************************
 * Name
 *   setup_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void setup_dictlex(GList *mods)
 *
 * Description
 *   setup dictlex support 
 *
 * Return value
 *  void
 */

void setup_dictlex(GList *mods)
{
	GtkWidget *popup;
	GList *tmp = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	DL_DATA *dl;
	gint count = 0;

	dl_list = NULL;

	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		dl = g_new(DL_DATA, 1);
		dl->modName = modname;
		dl->mod_num = count;
		dl->searchstring = NULL;
		dl->find_dialog = NULL;	
		dl->has_key = backend_module_is_locked(dl->modName);
		gui_create_dictlex_pane(dl, count);
		popup = gui_create_dictlex_pm(dl, mods);
		gnome_popup_menu_attach(popup, dl->html, NULL);
		backend_new_dictlex_display(dl->html, dl->mod_num);
		dl_list = g_list_append(dl_list, (DL_DATA *) dl);
		++count;
		tmp = g_list_next(tmp);
	}


	gtk_signal_connect(GTK_OBJECT(settings.notebookDL), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook_dictlex_switch_page),
			   dl_list);

	modbuf = g_strdup(settings.DictWindowModule);
	keybuf = g_strdup(settings.dictkey);

	set_page_dictlex(modbuf, dl_list);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
	settings.dict_last_page = 0;
}

/******************************************************************************
 * Name
 *   shutdown_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void shutdown_dictlex(void)
 *
 * Description
 *    shutdown and cleanup dictlex support
 *
 * Return value
 *   void
 */

void shutdown_dictlex(void)
{
	dl_list = g_list_first(dl_list);
	while (dl_list != NULL) {
		DL_DATA *d = (DL_DATA *) dl_list->data;
		/* free any search dialogs created */
		if (d->find_dialog)
			g_free(d->find_dialog);
		g_free((DL_DATA *) dl_list->data);
		dl_list = g_list_next(dl_list);
	}
	g_list_free(dl_list);

}

/******************************************************************************
 * Name
 *   display_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void display_dictlex(gchar * key)	
 *
 * Description
 *    display new key in current dictionary
 *
 * Return value
 *   void
 */
 
void display_dictlex(gchar * key)
{
	strcpy(settings.dictkey,key);
	backend_display_dictlex(settings.dict_last_page, key);
}

/******************************************************************************
 * Name
 *   get_first_key_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   gchar *get_first_key_viewdict(void)
 *
 * Description
 *   
 *
 * Return value
 *   gchar *
 */

gchar *get_first_key_viewdict(void)
{
	return backend_get_first_key_viewdict();
}

/******************************************************************************
 * Name
 *   get_next_key_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   gchar *get_next_key_viewdict(void)
 *
 * Description
 *   
 *
 * Return value
 *   gchar *
 */

gchar *get_next_key_viewdict(void)
{
	return backend_get_next_key_viewdict();
}

/******************************************************************************
 * Name
 *  search_text_changed_viewdict 
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void search_text_changed_viewdict(gchar * key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void search_text_changed_viewdict(gchar * key)
{
	backend_search_text_changed_viewdict(key);
}

/******************************************************************************
 * Name
 *   shutdown_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void shutdown_viewdict(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void shutdown_viewdict(void)
{
	backend_shutdown_viewdict();
}

/******************************************************************************
 * Name
 *   load_module_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void load_module_viewdict(gchar * module_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void load_module_viewdict(gchar * module_name)
{
	backend_load_module_viewdict(module_name);
}

/******************************************************************************
 * Name
 *   goto_key_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void goto_key_viewdict(gchar * key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void goto_key_viewdict(gchar * key)
{
	backend_goto_key_viewdict(key);
}

/******************************************************************************
 * Name
 *  setup_viewdict 
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void setup_viewdict(GtkWidget * text)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void setup_viewdict(GtkWidget * text)
{
	backend_setup_viewdict(text);
}

