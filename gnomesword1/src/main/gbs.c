/*
 * GnomeSword Bible Study Tool
 * gbs.c - generic book support - the glue
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
#include <glib-2.0/glib.h>

#include "main/gbs.h"
#include "main/settings.h"
#include "main/xml.h"

#include "gui/widgets.h"

#include "backend/sword.h"
//#include "backend/tree.hh"
 

/******************************************************************************
 * Name
 *   display_gbs
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void display_gbs(char * book_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
char * display_gbs(char * book_name, char * key)
{
	return backend_get_module_text(3, book_name, key);
}

/******************************************************************************
 * Name
 *   treekey_next_sibling
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   int treekey_next_sibling(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int treekey_next_sibling(unsigned long offset)
{
	return backend_treekey_next_sibling(offset);
}

/******************************************************************************
 * Name
 *   gbs_treekey_has_children
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   int gbs_treekey_has_children(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int gbs_treekey_has_children(unsigned long offset)
{
	return backend_gbs_treekey_has_children(offset);
}

/******************************************************************************
 * Name
 *   gbs_get_treekey_local_name
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   char * gbs_get_treekey_local_name(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
char * gbs_get_treekey_local_name(unsigned long offset)
{
	return backend_gbs_get_treekey_local_name(offset);
}

/******************************************************************************
 * Name
 *   gbs_get_treekey_offset 
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   unsigned long gbs_get_treekey_offset(void)	
 *
 * Description
 *    
 *
 * Return value
 *   unsigned long
 */ 
 
unsigned long gbs_get_treekey_offset(void)
{
	return backend_gbs_get_treekey_offset();
}

/******************************************************************************
 * Name
 *   gbs_treekey_first_child
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   int gbs_treekey_first_child(unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */ 
 
int gbs_treekey_first_child(unsigned long offset)
{
	return backend_gbs_treekey_first_child(offset);
}
 
/******************************************************************************
 * Name
 *  get_book_key
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   char *get_book_key(char * book_name)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 
 
char *get_book_key(char * book_name)
{
	return backend_get_key_from_module(3, book_name);
}

/******************************************************************************
 * Name
 *  change_book
 *
 * Synopsis
 *   #include "gbs.h"
 *
 *   void change_book(char * mod_name, unsigned long offset)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 
 
void main_set_book_mod(char * mod_name, unsigned long offset)
{
	gchar * buf;
	
	xml_set_value("GnomeSword", "modules", "book", mod_name);
	settings.book_mod = xml_get_value( "modules", "book");
	
	buf = g_strdup_printf("%lu", offset);
	//g_warning("main_set_book_mod = %s",buf);
	xml_set_value("GnomeSword", "keys", "offset", buf);
	settings.book_offset = atol(xml_get_value( "keys", "offset"));
	g_free(buf);
	
	backend_set_module(3, mod_name);
	backend_set_treekey(mod_name, offset);
}


void main_display_gbs(char * mod_name, unsigned long offset)
{
	entry_display(widgets.html_comm, mod_name,
			get_text_from_offset(mod_name, offset), 
			backend_get_key_from_offset (offset), 
			FALSE);
	
}
