/*
 * GnomeSword Bible Study Tool
 * module.cc - SHORT DESCRIPTION
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
#include <string.h>

#include "main/module.h"
#include "main/settings.h"
#include "main/xml.h"

#include "backend/module.hh"
#include "backend/key.hh"

extern int search_dialog;



/******************************************************************************
 * Name
 *   get_footnote_body
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   char *get_footnote_body(char *note)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *get_footnote_body(char *note)
{
	gchar *buf = NULL;
	gchar *token;
	gchar *type;
	gchar *note_number;
	gchar *key;
	
	token = strtok(note, ".");
	if(token)
		key = g_strdup(token);
	else return NULL;
	token = strtok(NULL, ".");
	if(token)
		type = g_strdup(token);
	else return NULL;
	token = strtok(NULL, ".");
	if(token)
		note_number = strdup(token);
	else return NULL;
	
	//key = g_strdup_printf("%s %s:%s",book,chapter,verse);
		
	buf = backend_get_footnote_body(
		xml_get_value("modules", "bible"), 
		key, note_number);
		
	g_free(type);
	g_free(key);
	g_free(note_number);
	if(buf) 
		return buf;
	else
		return NULL;
	
}

/******************************************************************************
 * Name
 *   get_display_level
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int get_display_level(gchar *modname)
 *
 * Description
 *   return the DisplayLevel information from the <module>.conf
 *
 * Return value
 *   int
 */

int get_display_level(char * module_name)
{
	return backend_get_display_level(module_name);
}


/******************************************************************************
 * Name
 *   is_module_rtl
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int is_module_rtl(char * mod_name)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int is_module_rtl(char * mod_name)
{
	return backend_is_module_rtl(mod_name);
	
}


/******************************************************************************
 * Name
 *   do_module_search
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   int do_module_search(char * module_name ,char * search_string, 
 *		int search_type, int search_params, int dialog)	
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int do_module_search(char * module_name ,const char * search_string, 
		int search_type, int search_params, int dialog)
{
	search_dialog = dialog;
	strcpy(settings.searchText, search_string);
	return backend_do_module_search(module_name, search_string, 
			     search_type, search_params);
}


/******************************************************************************
 * Name
 *  get_module_description
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   gchar *get_module_description(gchar * module_name)	
 *
 * Description
 *    
 *
 * Return value
 *   gchar *
 */ 

char *get_module_description(char * module_name)
{
	return backend_get_module_description(module_name);
}



/******************************************************************************
 * Name
 *   get_module_text
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *get_module_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *get_module_text(int manager, char * mod_name, char * key)
{
	backend_get_module_text(manager, mod_name, key);	
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *get_striptext(int manager, char *module_name, char *key)
{
	return backend_get_striptext(manager, module_name, key);
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
 *  get_module_number
 *
 * Synopsis
 *   #include "sword.h"
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
 *  get_mod_about_info
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   gchar *get_mod_about_info(char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   gchar *
 */ 

char *get_mod_about_info(char * mod_name)
{
	return backend_get_mod_about_info(mod_name);
}



/******************************************************************************
 * Name
 *  module_is_locked
 *
 * Synopsis
 *   #include "sword.h"
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
 *  get_cipher_key
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *get_cipher_key(char *mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 

char *get_cipher_key(char *mod_name)
{
	return backend_get_cipher_key(mod_name);
}


/******************************************************************************
 * Name
 *  has_cipher_tag
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   int has_cipher_tag(char *mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int has_cipher_tag(char *mod_name)
{
	return backend_has_cipher_tag(mod_name);
}


/******************************************************************************
 * Name
 *  get_mod_type
 *
 * Synopsis
 *   #include "sword.h"
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
