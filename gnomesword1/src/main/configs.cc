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


#include <glib-2.0/glib.h>

#include "main/configs.h"
#include "main/settings.h"
#include "main/sword.h"

#include "backend/sword_main.hh"

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
	return backend->get_conf_file_item(file, mod_name, item);
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
	backend->save_conf_file_item(file, mod_name, item, value);
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
 *    to unlock locked modules
 *
 * Return value
 *   void
 */ 

void save_module_key(char * mod_name, char * key)
{
	backend->save_module_key(mod_name, key);

	/* FIXME: we need to display change */
}
