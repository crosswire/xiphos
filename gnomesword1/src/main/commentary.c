/*
 * GnomeSword Bible Study Tool
 * commentary.c - glue for commentary modules
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
#  include <config.h>
#endif

#include <gnome.h>


#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_html.h"
#include "shortcutbar.h"
#include "gs_viewcomm_dlg.h"
#include "cipher_key_dialog.h"

/* gnome */
#include "_commentary.h"

/* main */
#include "commentary.h"
#include "settings.h"

/* backend */
#include "sword.h"
#include "commentary_.h"

/******************************************************************************
 * externs
 */ 

extern gboolean isrunningVC; 

/******************************************************************************
 * globals
 */ 
 
COMM_DATA *cur_c;
gboolean comm_display_change = TRUE;

/******************************************************************************
 * global to this file only 
 */
 
static GList *comm_list;


/******************************************************************************
 * Name
 *   navigate_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   const char* navigate_commentary(gint modnum, gint direction)	
 *
 * Description
 *    navigate foward or backward through commentary
 *
 * Return value
 *   void
 */
 
const char* navigate_commentary(gint modnum, gint direction)
{
	return backend_nav_commentary(modnum, direction);
}

/******************************************************************************
 * Name
 *  set_commentary_page_and_key
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void set_commentary_page_and_key(gint page_num, gchar *key)	
 *
 * Description
 *   change commentary page and key 
 *
 * Return value
 *   void
 */
 
void set_commentary_page_and_key(gint page_num, gchar *key)
{
	comm_display_change = FALSE;
	strcpy(settings.comm_key,key);
	strcpy(cur_c->key,key);
	gtk_notebook_set_page(GTK_NOTEBOOK(settings.notebook_comm), page_num);
	backend_display_commentary(page_num,key);
	comm_display_change = TRUE;
}

/******************************************************************************
 * Name
 *  set_commentary_page
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void set_commentary_page(gchar * modname, GList * comm_list,
 *							SETTINGS * s)	
 *
 * Description
 *    change commentary page without changing key
 *
 * Return value
 *   void
 */
 
static void set_commentary_page(gchar * modname, GList * comm_list,
							SETTINGS * s)
{
	gint page = 0;
	COMM_DATA *c = NULL;

	comm_list = g_list_first(comm_list);
	while (comm_list != NULL) {
		c = (COMM_DATA *) comm_list->data;
		if (!strcmp(c->modName, modname))
			break;
		++page;
		comm_list = g_list_next(comm_list);
	}

	gtk_notebook_set_page(GTK_NOTEBOOK(s->notebook_comm), page);
	s->commLastPage = page;
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(s->notebook_comm),
				   s->comm_tabs);
}

/******************************************************************************
 * Name
 *   display_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void display_commentary(gchar * key)	
 *
 * Description
 *    display new key in current commentary
 *
 * Return value
 *   void
 */
 
void display_commentary(gchar * key)
{
	strcpy(settings.comm_key,key);
	strcpy(cur_c->key, key);
	backend_display_commentary(settings.commLastPage, key);
}

/******************************************************************************
 * Name
 *  setup_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   GList* setup_commentary(SETTINGS * s)	
 *
 * Description
 *    setup gui commentary support and return list on commentary names
 *
 * Return value
 *   void
 */
 
GList* setup_commentary(SETTINGS * s)
{
	GtkWidget *popupmenu;
	GList *tmp = NULL;
	GList *mods = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	COMM_DATA *c;
	gint count = 0;

	comm_list = NULL;

	mods = backend_get_list_of_mods_by_type(COMM_MODS);
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		modname = (gchar *) tmp->data;
		c = g_new(COMM_DATA, 1);
		c->modName = modname;
		c->modnum = count;
		c->searchstring = NULL;
		c->key[0] = '\0';
		c->find_dialog = NULL;		
		c->has_key = backend_module_is_locked(c->modName);
		gui_create_commentary_pane(s, c, count);
		popupmenu = gui_create_pm(c);
		gnome_popup_menu_attach(popupmenu, c->html, NULL);
		backend_new_display_commentary(c->html, c->modName, s);
		comm_list = g_list_append(comm_list, (COMM_DATA *) c);
		++count;
		tmp = g_list_next(tmp);
	}

	gtk_signal_connect(GTK_OBJECT(s->notebook_comm),
			   "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebook_comm_switch_page), comm_list);

	modbuf = g_strdup(s->CommWindowModule);
	keybuf = g_strdup(s->currentverse);

	set_commentary_page(modbuf, comm_list, s);

	g_free(modbuf);
	g_free(keybuf);
	g_list_free(tmp);
	return mods;
}

/******************************************************************************
 * Name
 *   shutdown_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void shutdown_commentary(void)	
 *
 * Description
 *   shut down and cleanup 
 *
 * Return value
 *   void
 */
 
void shutdown_commentary(void)
{
	comm_list = g_list_first(comm_list);
	while (comm_list != NULL) {
		COMM_DATA *c = (COMM_DATA *) comm_list->data;
		/* 
		 * free any search dialogs created 
		 */
		if (c->find_dialog)	
			g_free(c->find_dialog);
		
		g_free((COMM_DATA *) comm_list->data);
		comm_list = g_list_next(comm_list);
	}
	g_list_free(comm_list);
}

/******  end of file  ******/
