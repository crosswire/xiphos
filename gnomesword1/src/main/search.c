/*
 * GnomeSword Bible Study Tool
 * shortcutbar.c - glue  (: very sticky :)
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

#include <glib-1.2/glib.h>

#include "gui/main_window.h"
#include "gui/search_dialog.h"
#include "gui/shortcutbar_search.h"

#include "main/search.h"
 
#include "backend/sword.h"
#include "backend/search.h"



void save_custom_modlist(GList * modlist)
{
	backend_save_custom_modlist(modlist);
}


GList * load_custom_modlist(void)
{
	return backend_load_custom_modlist();
}


/******************************************************************************
 * Name
 *   get_element
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   GList * get_element(char * entry)	
 *
 * Description
 *    returns the elements of a search range (scope)
 *
 * Return value
 *   GList *
 */ 

GList * get_element(char * entry)
{
	return backend_get_element(entry);
}


/******************************************************************************
 * Name
 *   set_range
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int set_range(char * list)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int set_range(char * list)
{
	return backend_set_range(list);
}


/******************************************************************************
 * Name
 *   save_custom_ranges
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   void save_custom_ranges(GList * ranges)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void save_custom_ranges(GList * ranges)
{
	backend_save_custom_ranges(ranges);
}


/******************************************************************************
 * Name
 *   load_custom_ranges
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   GList * load_custom_ranges(void)	
 *
 * Description
 *    
 *
 * Return value
 *   GList *
 */ 

GList * load_custom_ranges(void)
{
	return backend_load_custom_ranges();
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

void search_percent_update(char percent, void *userData)
{
	gui_search_update(percent, userData);
}


/******************************************************************************
 * Name
 *   search_dialog_appbar_update
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   void search_dialog_appbar_update(char percent, void *userData)	
 *
 * Description
 *    called by backend search to update progress during search
 *
 * Return value
 *   void
 */ 

void search_dialog_appbar_update(char percent, void *userData)
{
	gui_search_dialog_appbar_update(percent, userData);
}


/******************************************************************************
 * Name
 *  do_search
 *
 * Synopsis
 *   #include "ssearch.h"
 *
 *   GList *do_search(gpointer *usr_data)	
 *
 * Description
 *    
 *
 * Return value
 *  GList * 
 */ 

GList *do_search(gpointer *usr_data)
{
	return backend_do_sb_search(usr_data);
}


/******************************************************************************
 * Name
 *   clear_scope
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int clear_scope(void)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int clear_scope(void)
{
	return backend_clear_scope();
}


/******************************************************************************
 * Name
 *   clear_search_list
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int clear_search_list(void)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int clear_search_list(void)
{
	return backend_clear_search_list();
}


/******************************************************************************
 * Name
 *   set_scope2last_search
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int set_scope2last_search(void)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int set_scope2last_search(void)
{
	return backend_set_scope2last_search();
}


/******************************************************************************
 * Name
 *   set_scope2range
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   void set_scope2range(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void set_scope2range(void)
{
	backend_set_scope2range();
}


/******************************************************************************
 * Name
 *   set_search_module
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int set_search_module(char * module)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int set_search_module(char * module)
{
	return backend_set_search_module(module);
}


/******************************************************************************
 * Name
 *   do_dialog_search
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   int do_dialog_search(char *search_string, int search_type, 
 *						int search_params)	
 *
 * Description
 *    returns the number of finds
 *
 * Return value
 *   int
 */ 

int do_dialog_search(char *search_string, int search_type, 
						int search_params)
{
	return backend_do_dialog_search(search_string, 
					search_type, search_params);
}


/******************************************************************************
 * Name
 *   add_to_found_list
 *
 * Synopsis
 *   #include "main/search.h"
 *
 *   void add_to_found_list(const char * result_text, const char * module)	
 *
 * Description
 *   called by backend search when a find is made 
 *
 * Return value
 *   void
 */ 

void add_to_found_list(const char * result_text, const char * module)
{
	gui_add_to_found_list((char*)result_text, (char*)module);
}
