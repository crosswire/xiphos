/*
 * GnomeSword Bible Study Tool
 * configs.c - 
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

#include "backend/config.hh"

#include "main/configs.h"
#include "main/settings.h"
#include "main/sword.h"


/******************************************************************************
 * Name
 *   backend_save_custom_modlist
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   void backend_save_custom_modlist(GList * modlist)
 *
 * Description
 *   saves custom search module lists in preference.conf
 *
 * Return value
 *   void
 */
 
int open_config_file(char *file)
{
	return backend_open_config_file(file);
	
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   
 */ 

void erase_config_section(char *section)
{
	backend_erase_config_section(section);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   
 */ 

int close_config_file(void)
{
	return backend_close_config_file();
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   
 */ 

void add_to_config_file(char * section, 
					char * label, char * value)
{
	backend_add_to_config_file(section,  label, value)	;
	
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   
 */ 

const char *get_config_value(char * section, char * label)
{
	return backend_get_config_value(section, label);
	
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   
 */ 

int set_config_to_get_labels(char * section)
{
	backend_set_config_to_get_labels(section);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   
 */ 

const char *get_next_config_label(void)
{
	return backend_get_next_config_label();
}


/******************************************************************************
 * Name
 *  get_module_font_name
 *
 * Synopsis
 *   #include "main/configs.h"
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
	return backend_get_module_font_name(mod_name, settings.gSwordDir);
}


/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   
 */ 

char *get_conf_file_item(const char * file, const char * mod_name, const char * item)
{
	return backend_get_conf_item(file, mod_name, item);
}


/******************************************************************************
 * Name
 *  save_conf_file_item
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   void save_conf_file_item(char * file, char * mod_name, char * item,
 *			char * value)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void save_conf_file_item(const char * file, const char * mod_name, const char * item,
			const char * value)
{
	backend_save_conf_item(file, mod_name, item, value);
}


/******************************************************************************
 * Name
 *  get_module_font_size
 *
 * Synopsis
 *   #include "main/configs.h"
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
	return backend_get_module_font_size(mod_name, settings.gSwordDir);
}


/******************************************************************************
 * Name
 *  save_module_key
 *
 * Synopsis
 *   #include "main/configs.h"
 *
 *   void save_module_key(gchar * mod_name, gchar * key)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void save_module_key(char * mod_name, char * key)
{
	backend_save_module_key(mod_name, key);

	/* FIXME: we need to display change */
}
