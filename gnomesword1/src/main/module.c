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



#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif
#include <string.h>

#include "gui/gnomesword.h"

#include "main/module.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

#include "backend/module.hh"
#include "backend/key.hh"

extern int search_dialog;


const char *get_mod_config_entry(const char * module_name, const char * entry)
{
	return backend_get_module_config_entry(module_name, entry);
}


char *get_key_from_module(int manager, char *module_name)
{
	return backend_get_key_from_module(manager, module_name);
}
/******************************************************************************
 * Name
 *  get_preverse_header
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   char *get_preverse_header(char * module_name, char * key, int pvHeading)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */

char *get_preverse_header(char * module_name, char * key, int pvHeading)
{
	backend_get_preverse_header(module_name, key, pvHeading);
}


/******************************************************************************
 * Name
 *  get_module_key
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   char *get_module_key(void)	
 *
 * Description
 *    returns module key
 *
 * Return value
 *   char *
 */

char *get_module_key(void)
{
	if (settings.havebible) {
		switch (settings.whichwindow) {
		case MAIN_TEXT_WINDOW:
			return (char *) settings.currentverse;
			break;
		case COMMENTARY_WINDOW:
			return (char *) settings.comm_key;
			break;
		case DICTIONARY_WINDOW:
			return (char *) settings.dictkey;
			break;
		case parallel_WINDOW:
			return (char *) settings.cvparallel;
			break;
		case BOOK_WINDOW:
			return (char *) settings.book_key;
			break;
		}
	}
	return NULL;
}


/******************************************************************************
 * Name
 *  get_module_name
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   char *get_module_name(void)	
 *
 * Description
 *    returns module name
 *
 * Return value
 *   char *
 */

char *get_module_name(void)
{
	if (settings.havebible) {
		switch (settings.whichwindow) {
		case MAIN_TEXT_WINDOW:
			return (char *) xml_get_value("modules",
						       "bible");
			break;
		case COMMENTARY_WINDOW:
			return (char *) xml_get_value("modules",
						       "comm");
			break;
		case DICTIONARY_WINDOW:
			return (char *) settings.DictWindowModule;
			break;
		case BOOK_WINDOW:
			return (char *) settings.book_mod;
			break;
		}
	}
	return NULL;
}
char *get_crossref(char * mod_name, char * key, char * note_number)
{
	return backend_get_crossref(mod_name,  key, note_number);
}
/******************************************************************************
 * Name
 *   get_footnote_type
 *
 * Synopsis
 *   #include "main/module.h"
 *
 *   char *get_footnote_type(char *note)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

const char *get_footnote_type(char * module_name, char * key, char * note_number)
{	
	return backend_get_footnote_type(module_name, key, note_number);
}

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

char *get_footnote_body(char * mod_name, char * key, char * note_number)
{
	return backend_get_footnote_body(mod_name, key, note_number);
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
 *   #include "main/module.h"
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
	gchar *retval = NULL;
	gsize bytes_read;
	gsize bytes_written;
	GError **error;	
	char *mykey = g_convert(key,
			     -1,
			     OLD_CODESET,
			     UTF_8,
			     &bytes_read,
			     &bytes_written,
			     error);	 
	retval = backend_get_module_text(manager, mod_name, mykey); 
	g_free(mykey);
	return retval;	
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
 *   #include "main/module.h"
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
 *   #include "main/module.h"
 *
 *   int get_module_number(char *module_name, char *module_type)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 

int get_module_number(const char *module_name, const char *module_type)
{
	return backend_get_module_page(module_name, module_type);
}



/******************************************************************************
 * Name
 *  get_mod_about_info
 *
 * Synopsis
 *   #include "main/module.h"
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
 *   #include "main/module.h"
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
 *   #include "main/module.h"
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
 *   #include "main/module.h"
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
 *   #include "main/module.h"
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
 *  module_name_from_description
 *
 * Synopsis
 *   #include ".h"
 *
 *   void module_name_from_description(gchar *mod_name, gchar *description)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

char *module_name_from_description(char *description)
{
	backend_module_name_from_description(description);
}
