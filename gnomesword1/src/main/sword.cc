/*
 * GnomeSword Bible Study Tool
 * sword.c - glue 
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
#include <swmgr.h>
#include <swmodule.h>

#include <ctype.h>
#include <time.h>

#include "gui/main_window.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"
#include "gui/commentary.h"
#include "gui/tabbed_browser.h"

#include "main/display.hh"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
 
#include "backend/sword.h"
#include "backend/sword_defs.h"
#include "backend/sword_main.hh"
#include "backend/mgr.hh"

void delete_module_mgr(void)
{
	backend_delete_module_mgr();
}

int gbs_treekey_get_parent(unsigned long offset)
{
	return backend_get_parent(offset);
}


/******************************************************************************
 * Name
 *  get_sword_version
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   const char *get_sword_version(void)	
 *
 * Description
 *    
 *
 * Return value
 *   const char *
 */ 

const char *get_sword_version(void)
{
	return backend_get_sword_version();
}


/******************************************************************************
 * Name
 *   get_search_results_text
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *get_search_results_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *get_search_results_text(char * mod_name, char * key)	
{
	return backend_get_search_results_text(mod_name, key);
}



/******************************************************************************
 * Name
 *  get_path_to_mods
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   	gchar *get_path_to_mods(void)
 *
 * Description
 *    returns the path to the sword modules
 *
 * Return value
 *   gchar *
 */ 

char *get_path_to_mods(void)
{
	return backend_get_path_to_mods();
}


void init_sword(void)
{
	backend_init();
}
/******************************************************************************
 * Name
 *   shutdown_sword
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void shutdown_sword(void)	
 *
 * Description
 *   close down sword by calling backend_shutdown();
 *
 * Return value
 *   void
 */

void shutdown_backend(void)
{
	backend_shutdown(TRUE);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include ""
 *
 *    	
 *
 * Description
 *    
 *
 * Return value
 *   char * 
 */ 
 
char * get_text_from_offset(char * module_name, unsigned long offset)
{        
	gchar * buf;
	
	xml_set_value("GnomeSword", "keys", "book", 
			backend_get_key_from_offset (offset));
	settings.book_key = xml_get_value( "key", "book");
	
	buf = g_strdup_printf("%lu", offset);
	xml_set_value("GnomeSword", "keys", "offset", buf);
	settings.book_offset = atol(xml_get_value( "keys", "offset"));
	g_free(buf);
	
	return backend_get_text_from_offset(module_name, offset);
}


void main_dictionary_entery_changed(char * mod_name)
{
	gint count = 7, i;
	gchar *new_key, *text = NULL;
	gchar *key = NULL;
	static gboolean firsttime = TRUE;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	gint height;
	
	key = (gchar*)gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict));
	backend->set_module_key(mod_name, key);
	key = backend->get_module_key();
	
	xml_set_value("GnomeSword", "keys", "dictionary", key);
	settings.dictkey = xml_get_value("keys", "dictionary");
	
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(widgets.listview_dict));
	list_store = GTK_LIST_STORE(model);
	
	if (!firsttime) {
		gdk_drawable_get_size ((GdkDrawable *)widgets.listview_dict->window,
                                             NULL,
                                             &height);
		count = height / settings.cell_height;
	}	 

	if (count) {
		gtk_list_store_clear(list_store);
		new_key = backend->navigate_module(-1);

		for (i = 0; i < (count / 2); i++) {
			free(new_key);
			new_key = backend->navigate_module(0);
		}

		for (i = 0; i < count; i++) {
			free(new_key);
			new_key = backend->navigate_module(1);
			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, 0,
					   new_key, -1);
		}
		free(new_key);
	}
	firsttime = FALSE;
} 


void main_display_book(void)
{
	//backend->set_module(settings.book_mod,settings.book_offset);
	sw.gbs_mod->Display();
	
/*	if(settings.browsing)
		gui_update_tab_struct(NULL,
				      NULL,
				      NULL,
				      mod_name,
				      NULL,
				      key?key:NULL,
				      FALSE);
*/
}

void main_display_commentary(const char * mod_name, const char * key)
{
	if(!settings.havecomm)
		return;
	
	if(strcmp(settings.CommWindowModule,mod_name)) {
		xml_set_value("GnomeSword", "modules", "comm", mod_name);
		settings.CommWindowModule = xml_get_value( "modules", "comm");
		gtk_label_set_text (GTK_LABEL(widgets.label_comm),mod_name);
		gui_change_window_title(settings.CommWindowModule);
	}
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	
	if(settings.browsing)
		gui_update_tab_struct(NULL,
				      mod_name,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      TRUE);
}

void main_display_dictionary(char * mod_name, char * key)
{
	const gchar *old_key;
	
	if(!settings.havedict)
		return;
	if(strcmp(settings.DictWindowModule,mod_name)) {
		xml_set_value("GnomeSword", "modules", "dict",
					mod_name);
		settings.DictWindowModule = xml_get_value(
					"modules", "dict");
	}
	gtk_label_set_text (GTK_LABEL(widgets.label_dict),mod_name);
	
	if(key == NULL)
		key = "Grace";
	
	old_key = gtk_entry_get_text(GTK_ENTRY(widgets.entry_dict));
	if(!strcmp(old_key, key))
		main_dictionary_entery_changed(settings.DictWindowModule);
	else
		gtk_entry_set_text(GTK_ENTRY(widgets.entry_dict), key);
	
	if(settings.browsing)
		gui_update_tab_struct(NULL,
				      NULL,
				      mod_name,
				      NULL,
				      key,
				      NULL,
				      settings.comm_showing);
} 


void main_display_bible(const char * mod_name, const char * key)
{
	if(!settings.havebible)
		return;
	
	if(strcmp(settings.MainWindowModule,mod_name)) {
		xml_set_value("GnomeSword", "modules", "bible",
					mod_name);
		settings.MainWindowModule = xml_get_value(
					"modules", "bible");
	}
	backend->set_module_key(mod_name, key);
	backend->display_mod->Display();
	
	if(settings.browsing) {
		gui_update_tab_struct(mod_name,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      NULL,
				      settings.comm_showing);
		gui_set_tab_label(key);
	}
}

void main_setup_displays(void)
{
	sw.entryDisplay = new GTKEntryDisp(widgets.html_comm);
	sw.dictDisplay = new GTKEntryDisp(widgets.html_dict);
}

void main_setup_new_displays(void)
{
	backend->commDisplay = new GTKEntryDisp(widgets.html_comm);
	backend->dictDisplay = new GTKEntryDisp(widgets.html_dict);
	backend->textDisplay = new GTKChapDisp(widgets.html_text);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include ""
 *
 *   char *	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
int set_module(int manager, char * module_name)
{
	backend_set_module(manager, module_name);
}
