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
#include <ctype.h>
#include <time.h>


#include "gui/main_window.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
 
#include "backend/search_.h"
#include "backend/sword.h"
#include "backend/interlinear.h"
#include "backend/bibletext_.h"
#include "backend/commentary_.h"
#include "backend/percomm_.h"
#include "backend/gbs_.h"
#include "backend/dictlex_.h"
#include "backend/properties.h"
#include "backend/shortcutbar.h"
#include "backend/bookmarks.h"


/******************************************************************************
 * Name
 *  search_percent_update
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void search_percent_update(char percent, void *userData)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void search_percent_update(char percent, void *userData)
{
	gui_search_appbar_update(percent, userData);
}


/******************************************************************************
 * Name
 *  module_name_from_description
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void module_name_from_description(gchar *mod_name, gchar *description)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void module_name_from_description(gchar *mod_name, gchar *description)
{
	backend_module_name_from_description(mod_name, description);
}


/******************************************************************************
 * Name
 *  get_module_number
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   int get_module_number(char *module_name, char *module_type)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int get_module_number(char *module_name, char *module_type)
{
	return backend_get_module_page(module_name, module_type);
}


/******************************************************************************
 * Name
 *  save_module_key
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void save_module_key(gchar * mod_name, gchar * key)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void save_module_key(gchar * mod_name, gchar * key)
{
	backend_save_module_key(mod_name, key);

	/* FIXME: we need to display change */
}


/******************************************************************************
 * Name
 *  get_sword_version
 *
 * Synopsis
 *   #include "gnomesword.h"
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
 *  get_module_description
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   gchar *get_module_description(gchar * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   gchar *
 */ 

gchar *get_module_description(gchar * mod_name)
{
	return backend_get_module_description(mod_name);
}


/******************************************************************************
 * Name
 *  get_book_from_key
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   const char *get_book_from_key(char *key)	
 *
 * Description
 *    
 *
 * Return value
 *   const char *
 */ 

const char *get_book_from_key(char *key)
{
	return backend_get_book_from_key(key);
}


/******************************************************************************
 * Name
 *  get_chapter_from_key
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   int get_chapter_from_key(char *key)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int get_chapter_from_key(char *key)
{
	return backend_get_chapter_from_key(key);
}


/******************************************************************************
 * Name
 *  get_verse_from_key
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   int get_verse_from_key(char *key)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int get_verse_from_key(char *key)
{
	return backend_get_verse_from_key(key);
}


/******************************************************************************
 * Name
 *  save_properties
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void save_properties(gboolean use_default)	
 *
 * Description
 *   save properties stored in SETTINGS structure
 *
 * Return value
 *   void
 */

void save_properties(gboolean use_default)
{
	backend_save_properties(use_default);
}


/******************************************************************************
 * Name
 *  do_search
 *
 * Synopsis
 *   #include "gnomesword.h"
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
	return backend_do_search(usr_data);
}


/******************************************************************************
 * Name
 *   get_verse_list
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   GList *get_verse_list(gchar* module_name, gchar *verse_list)	
 *
 * Description
 *    
 *
 * Return value
 *   GList *
 */ 


GList *get_verse_list(gchar* module_name, gchar *verse_list)
{
	return backend_get_verse_list(module_name, verse_list);
}


/******************************************************************************
 * Name
 *  get_module_font_name
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   char *get_module_font_name(char *mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 

char *get_module_font_name(char *mod_name)
{
	return backend_get_module_font_name(mod_name);
}


/******************************************************************************
 * Name
 *  get_module_font_size
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   char *get_module_font_size(char *mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *  char * 
 */ 

char *get_module_font_size(char *mod_name)
{
	return backend_get_module_font_size(mod_name);
}


/******************************************************************************
 * Name
 *  get_mod_type
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   int get_mod_type(char * mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int get_mod_type(char * mod_name)
{
	return backend_get_mod_type(mod_name);
}


/******************************************************************************
 * Name
 *  module_is_locked
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   int module_is_locked(char * mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int module_is_locked(char * mod_name)
{
	return backend_module_is_locked(mod_name);
}


/******************************************************************************
 * Name
 *   get_valid_key
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *get_valid_key(char *key)	
 *
 * Description
 *    returns a valid Bible reference - must be freed by calling function
 *
 * Return value
 *   char *
 */

char *get_valid_key(char *key)
{
	return backend_get_valid_key(key);
}


/******************************************************************************
 * Name
 *   get_module_text
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   char *get_module_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *get_module_text(char * mod_name, char * key)
{
	backend_get_module_text(mod_name, key);	
}


/******************************************************************************
 * Name
 *   get_search_results_text
 *
 * Synopsis
 *   #include "gnomesword.h"
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
 *   create_properties_from_setup
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   int create_properties_from_setup(void)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */

int create_properties_from_setup(void)
{
	return backend_create_properties_from_setup();
}


/******************************************************************************
 * Name
 *  get_path_to_mods
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   	gchar *get_path_to_mods(void)
 *
 * Description
 *    returns the path to the sword modules
 *
 * Return value
 *   gchar *
 */ 

gchar *get_path_to_mods(void)
{
	return backend_get_path_to_mods();
}


/******************************************************************************
 * Name
 *  get_list_of_mods_by_type
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   GList *get_list_of_mods_by_type(char *mod_type)	
 *
 * Description
 *    
 *
 * Return value
 *   GList *
 */ 

GList *get_list_of_mods_by_type(char *mod_type)
{
	return backend_get_list_of_mods_by_type(mod_type);
}


/******************************************************************************
 * Name
 *  get_mod_about_info
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   gchar *get_mod_about_info(char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   gchar *
 */ 

gchar *get_mod_about_info(char * mod_name)
{
	return backend_get_mod_about_info(mod_name);
}


/******************************************************************************
 * Name
 *   check_for_module
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   int check_for_module(char * mod_name)
 *
 * Description
 *    check for presents of a module by name
 *
 * Return value
 *   int
 */
 
int check_for_module(char * mod_name)
{
	return backend_check_for_module(mod_name);
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

void shutdown_sword(void)
{
	backend_shutdown();
}
