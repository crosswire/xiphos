/*
 * GnomeSword Bible Study Tool
 * dictlex.c - dictlex glue
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
#include <gtkhtml/gtkhtml.h>


#include "backend/sword.h"
#include "backend/dictlex_.h"
#include "backend/viewdict.h"

#include "gui/dictlex_dialog.h"
#include "gui/dictlex.h"
#include "gui/cipher_key_dialog.h"

#include "main/settings.h"
#include "main/dictlex.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   
 */
 
void new_dictlex_display(GtkWidget * html, gint mod_num)
{
	backend_new_dictlex_display(html, mod_num);
}

/******************************************************************************
 * Name
 *   fill_dictlex_keys
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   GList *fill_dictlex_keys(int mod_num, int count)
 *
 * Description
 *    get a list of dictlex key to fill clist widget by
 *    calling backend_fill_dictlex_keys();
 *
 * Return value
 *   GList*
 */

GList *fill_dictlex_keys(int mod_num, int count)
{
	return backend_fill_dictlex_keys(mod_num, count);
}

/******************************************************************************
 * Name
 *   display_dictionary_page_and_key
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void display_dictionary_page_and_key(gint page_num, gchar * key)
 *
 * Description
 *   calls backend_display_dictlex to display new module and key 
 *
 * Return value
 *   void
 */

void display_dictionary_page_and_key(gint page_num, gchar * key)
{
	backend_display_dictlex(page_num, key);
}


/******************************************************************************
 * Name
 *   display_dictlex
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void display_dictlex(gchar * key)	
 *
 * Description
 *    display new key in current dictionary
 *
 * Return value
 *   void
 */
 
void display_dictlex(gchar * key)
{
	strcpy(settings.dictkey,key);
	backend_display_dictlex(settings.dict_last_page, key);
}

/******************************************************************************
 * Name
 *   get_first_key_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   gchar *get_first_key_viewdict(void)
 *
 * Description
 *   
 *
 * Return value
 *   gchar *
 */

gchar *get_first_key_viewdict(void)
{
	return backend_get_first_key_viewdict();
}

/******************************************************************************
 * Name
 *   get_next_key_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   gchar *get_next_key_viewdict(void)
 *
 * Description
 *   
 *
 * Return value
 *   gchar *
 */

gchar *get_next_key_viewdict(void)
{
	return backend_get_next_key_viewdict();
}

/******************************************************************************
 * Name
 *  search_text_changed_viewdict 
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void search_text_changed_viewdict(gchar * key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void search_text_changed_viewdict(gchar * key)
{
	backend_search_text_changed_viewdict(key);
}

/******************************************************************************
 * Name
 *   shutdown_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void shutdown_viewdict(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void shutdown_viewdict(void)
{
	backend_shutdown_viewdict();
}

/******************************************************************************
 * Name
 *   load_module_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void load_module_viewdict(gchar * module_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void load_module_viewdict(gchar * module_name)
{
	backend_load_module_viewdict(module_name);
}

/******************************************************************************
 * Name
 *   goto_key_viewdict
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void goto_key_viewdict(gchar * key)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void goto_key_viewdict(gchar * key)
{
	backend_goto_key_viewdict(key);
}

/******************************************************************************
 * Name
 *  setup_viewdict 
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   void setup_viewdict(GtkWidget * text)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void setup_viewdict(GtkWidget * text)
{
	backend_setup_viewdict(text);
}

