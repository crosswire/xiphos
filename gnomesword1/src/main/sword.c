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

#include <glib-1.2/glib.h>
#include <ctype.h>
#include <time.h>

#include "gui/main_window.h"
#include "gui/font_dialog.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
 
#include "backend/sword.h"


/******************************************************************************
 * Name
 *  module_name_from_description
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void module_name_from_description(gchar *mod_name, gchar *description)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void module_name_from_description(char *mod_name, char *description)
{
	backend_module_name_from_description(mod_name, description);
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

void shutdown_sword(void)
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
 
char * get_text_from_offset(char * module_name, char * offset)
{        
	return backend_get_text_from_offset(module_name, offset);
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
