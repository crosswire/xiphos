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
//#include <glib-2.0/glib.h>

#include <ctype.h>
#include <time.h>

#include "gui/main_window.h"
#include "gui/font_dialog.h"
#include "gui/widgets.h"

#include "main/display.hh"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/xml.h"
 
#include "backend/sword.h"
#include "backend/sword_defs.h"

void set_global_option(int manager, char * option, gboolean choice)
{
	char *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}
	backend_set_global_option(manager, option, on_off);
}

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

void main_display_book(void)
{
	sw.gbs_mod->Display();
}

void main_display_commentary(void)
{
	sw.comm_mod->Display();
}

void main_setup_displays(void)
{
	sw.entryDisplay = new GTKEntryDisp(widgets.html_comm);
}

void main_delete_displays(void)
{
	if(sw.entryDisplay)
		delete sw.entryDisplay;
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
	return backend_set_module(manager, module_name);
}
