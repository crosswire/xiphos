/*
 * GnomeSword Bible Study Tool
 * dictlex.c - SHORT DESCRIPTION
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


/* backend */
#include "sword.h"
#include "dictlex_.h"

/* gnome */
#include "_dictlex.h"
#include "cipher_key_dialog.h"

/* main */
#include "settings.h"
#include "dictlex.h"
#include "gs_gnomesword.h"
#include "gs_html.h"
#include "gs_viewdict_dlg.h"

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
 *   GList* setup_dictlex(SETTINGS * s)
 *
 * Description
 *   setup dictlex support 
 *
 * Return value
 *   GList*
 */

GList* setup_dictlex(SETTINGS * s)
{
	GtkWidget *popup;
	GList *tmp = NULL;
	GList *mods = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	DL_DATA *dl;
	gint count = 0;

	dl_list = NULL;

	mods = backend_get_list_of_mods_by_type(DICT_MODS);
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
		gui_create_dictlex_pane(s, dl, count);
		popup = gui_create_dictlex_pm(dl, mods);
		gnome_popup_menu_attach(popup, dl->html, NULL);
		backend_new_dictlex_display(dl->html, dl->mod_num, s);
		dl_list = g_list_append(dl_list, (DL_DATA *) dl);
		++count;
		tmp = g_list_next(tmp);
	}


	gtk_signal_connect(GTK_OBJECT(s->notebookDL), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebook_dictlex_switch_page),
			   dl_list);

	modbuf = g_strdup(s->DictWindowModule);
	keybuf = g_strdup(s->dictkey);

	set_page_dictlex(modbuf, dl_list);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
	s->dict_last_page = 0;
	return mods;
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
