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

#include <glib-1.2/glib.h>


#include "main/settings.h"
#include "main/bibletext.h"
#include "main/sword.h"

#include "backend/sword.h"


/******************************************************************************
 * Name
 *  set_module_unlocked
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void set_module_unlocked(char *mod_name, char *key)	
 *
 * Description
 *   unlocks locked module - 
 *
 * Return value
 *   void
 */

void set_module_unlocked(char * mod_name, char * key)
{	
	backend_set_module_unlocked(mod_name, key);
}


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
 *  check_for_global_option
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  	gint check_for_global_option(gchar * mod_name, gchar * option)
 *
 * Description
 *    get global options for a module
 *
 * Return value
 *   gint
 */

gint check_for_global_option(gchar * mod_name, gchar * option)
{
	return backend_check_for_global_option(mod_name, option);
}

/******************************************************************************
 * Name
 *  set_text_module_global_options
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *  void set_text_module_global_options(gchar * option, gboolean choice)	
 *
 * Description
 *   set sword global option 
 *
 * Return value
 *   void
 */

void set_text_module_global_option(gchar * option, gboolean choice)
{
	char *on_off;

	if (choice) {
		on_off = "On";
	} else {
		on_off = "Off";
	}
	backend_set_global_option(0, option, on_off);
}

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


/******************************************************************************
 * Name
 *   set_text_global_option
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void set_text_global_option(gchar * option, gchar * choice)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void set_text_global_option(gchar * option, gchar * choice)
{
	backend_set_global_option(0, option, choice);
}

/******************************************************************************
 * Name
 *  get_interlinear_module_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   char *get_interlinear_module_text(char * mod_name, char * tmpkey)	
 *
 * Description
 *    
 *
 * Return value
 *   char*
 */ 

char *get_interlinear_module_text(char * mod_name, char * tmpkey)
{
	return backend_get_module_text(8, mod_name, tmpkey);
}

/******************************************************************************
 * Name
 *  set_interlinear_global_option
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void set_interlinear_global_option(char * option, char * yesno)	
 *
 * Description
 *    
 *
 * Return value
 *   char*
 */ 

void set_interlinear_global_option(char * option, char * yesno)
{
	backend_set_global_option(8, option, yesno);
	/*backend_set_interlinear_global_option(option, yesno);*/
}
