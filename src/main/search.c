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

#include "main/search.h"
 
#include "backend/sword.h"
#include "backend/search.h"

GList * get_element(char * entry)
{
	return backend_get_element(entry);
}


char * set_range(char * list)
{
	backend_set_range(list);
}


void save_custom_ranges(GList * ranges)
{
	backend_save_custom_ranges(ranges);
}


GList * load_custom_ranges(void)
{
	return backend_load_custom_ranges();
}


/******************************************************************************
 * Name
 *  search_percent_update
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void search_percent_update(char percent, void *userData)	
 *
 * Description
 *    updates the progress bar during search
 *
 * Return value
 *   void
 */ 

void search_percent_update(char percent, void *userData)
{
	gui_search_appbar_update(percent, userData);
}

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


int clear_scope(void)
{
	backend_clear_scope();
}


int clear_search_list(void)
{
	backend_clear_search_list();
}


int clear_bounds(void)
{
	backend_clear_bounds();
}


int set_bounds_upper(char * bound)
{
	backend_set_bounds_upper(bound);
}


int set_bounds_lower(char * bound)
{
	backend_set_bounds_lower(bound);
}


int set_scope2bounds(void)
{
	backend_set_scope2bounds();
}


int set_scope2last_search(void)
{
	backend_set_scope2last_search();
}

void set_scope2range(void)
{
	backend_set_scope2range();
}


int set_search_module(char * module)
{
	return backend_set_search_module(module);
}


int do_dialog_search(char *search_string, int search_type, 
						int search_params)
{
	return backend_do_dialog_search(search_string, 
					search_type, search_params);
}

void add_to_found_list(const char * result_text, const char * module)
{
	gui_add_to_found_list((char*)result_text, (char*)module);
}
