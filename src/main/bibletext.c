/*
 * GnomeSword Bible Study Tool
 * bibletext.c - support for Bible text modules
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

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif


#include "main/settings.h"
#include "main/bibletext.h"
#include "main/sword.h"

#include "backend/sword.h"



/******************************************************************************
 * Name
 *  get_bibletext_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   char *get_bibletext_text(char *mod_name, char *key)	
 *
 * Description
 *    
 *
 * Return value
 *   char*
 */ 
/*
char *get_bibletext_text(char *mod_name, char *key)
{
	return backend_get_bibletext_text(mod_name, key);
}
*/
/*
char *get_bibletext_striptext(char *mod_name, char *key)
{
	return backend_get_bibletext_striptext(mod_name, key);
}
*/



/******************************************************************************
 * Name
 *   save_module_options
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   int save_module_options(char * mod_name, char * option, 
 *				    int choice)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */

int save_module_options(char * mod_name, char * option, 
				    int choice)
{
	char *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}
	return backend_save_module_options(mod_name, option, on_off, 
				settings.gSwordDir , "modops.conf");
}

/******************************************************************************
 * Name
 *   load_module_options
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  	int load_module_options(char * modName, char * option)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */

int load_module_options(char * modName, char * option)
{
	return backend_load_module_options(modName, option, 
				settings.gSwordDir, "modops.conf");
}
