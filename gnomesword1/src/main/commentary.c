/*
 * GnomeSword Bible Study Tool
 * commentary.c - glue for commentary modules and commentary dialog
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

#include "gui/_commentary.h"
#include "gui/commentary_dialog.h"
#include "gui/cipher_key_dialog.h"
#include "gui/shortcutbar_main.h"

#include "main/commentary.h"
#include "main/settings.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"

#include "backend/shortcutbar.h"
#include "backend/sword.h"
#include "backend/commentary_.h"
#include "backend/viewcomm.h"

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
 *   backend_display_book_heading
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void backend_display_book_heading(gint modnum)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void display_book_heading(int mod_num)
{
	backend_display_book_heading(mod_num);
}

/******************************************************************************
 * Name
 *   display_chap_heading
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   	void display_chap_heading(int mod_num)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void display_chap_heading(int mod_num)
{
	backend_display_chap_heading(mod_num);
}

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
	s->comm_last_page = page;
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
        if(!cur_c) return;
	strcpy(settings.comm_key,key);
	strcpy(cur_c->key, key);
	backend_display_commentary(settings.comm_last_page, key);
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
 
void setup_commentary(SETTINGS * s, GList *mods)
{
	GtkWidget *popupmenu;
	GList *tmp = NULL;
	gchar *modname;
	gchar *modbuf;
	gchar *keybuf;
	COMM_DATA *c;
	gint count = 0;
	extern gboolean comm_find_running;
	
	comm_list = NULL;
	comm_find_running = FALSE;
	
	//mods = backend_get_list_of_mods_by_type(COMM_MODS);
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
		 * free any find dialogs created 
		 */
		if (c->find_dialog)	
			g_free(c->find_dialog);
		
		g_free((COMM_DATA *) comm_list->data);
		comm_list = g_list_next(comm_list);
	}
	g_list_free(comm_list);
}

/******************************************************************************
 * Name
 *   goto_verse_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void goto_verse_viewcomm(gchar *verse)	
 *
 * Description
 *   change to new verse in viewcomm and display in commentary_dialog 
 *
 * Return value
 *   void
 */
 
void goto_verse_viewcomm(gchar *verse)
{
	backend_goto_verse_viewcomm(verse);
}

/******************************************************************************
 * Name
 *   load_module_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void load_module_viewcomm(gchar *module_name)	
 *
 * Description
 *    load module into commentary_dialog
 *
 * Return value
 *   void
 */
 
void load_module_viewcomm(gchar *module_name)
{
	backend_load_module_viewcomm(module_name);
}

/******************************************************************************
 * Name
 *   nav_module_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void nav_module_viewcomm(gint direction)	
 *
 * Description
 *    navagate commentary called by commentary_dialog
 *
 * Return value
 *   void
 */
 
void nav_module_viewcomm(gint direction)
{
	backend_nav_module_viewcomm(direction);
}

/******************************************************************************
 * Name
 *   setup_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void setup_viewcomm(GtkWidget * text)	
 *
 * Description
 *    setup viewcomm for commentary_dialog
 *
 * Return value
 *   void
 */
 
void setup_viewcomm(GtkWidget * text)
{
	backend_setup_viewcomm(text);
}

/******************************************************************************
 * Name
 *   shutdown_viewcomm
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   void shutdown_viewcomm(void)	
 *
 * Description
 *    call backend_shutdown_viewcomm to shutdown viewcomm
 *
 * Return value
 *   void
 */
 
void shutdown_viewcomm(void)
{
	backend_shutdown_viewcomm();
}
/******  end of file  ******/
