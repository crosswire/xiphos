/*
 * GnomeSword Bible Study Tool
 * commentary.c - glue for commentary modules and commentary dialog
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
#  include <config.h>
#endif

#include <glib-1.2/glib.h>

#include "main/commentary.h"
//#include "main/gs_gnomesword.h"

#include "backend/sword.h"
#include "backend/commentary_.h"


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   	
 *
 * Description
 *    
 *
 * Return value
 *   
 */ 

void set_commentary_key(char * mod_name, char * key)
{
	backend_set_commentary_key(mod_name, key);
}

/******************************************************************************
 * Name
 *   get_commentary_key
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   char *get_commentary_key(char *mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 

char *get_commentary_key(char *mod_name)
{
	return backend_get_commentary_key(mod_name);
}

/******************************************************************************
 * Name
 *   get_commentary_text
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   char *get_commentary_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 

char *get_commentary_text(char * mod_name, char * key)
{
	return backend_get_commentary_text(mod_name, key);
}

/******************************************************************************
 * Name
 *   get_book_heading
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   char *get_book_heading(char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 

char *get_book_heading(char * mod_name, char * key)
{
	return backend_get_book_heading(mod_name, key);
}

/******************************************************************************
 * Name
 *   get_chap_heading
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   char *get_chap_heading(char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 

char *get_chapter_heading(char * mod_name, char * key)
{
	return backend_get_chap_heading(mod_name, key);
}

/******************************************************************************
 * Name
 *   navigate_commentary
 *
 * Synopsis
 *   #include "commentary.h"
 *
 *   char* navigate_commentary(char * mod_name, gint direction)	
 *
 * Description
 *    navigate foward or backward through commentary
 *
 * Return value
 *   char*
 */
 
char* navigate_commentary(char * mod_name, gint direction)
{
	return backend_nav_commentary(mod_name, direction);
}

/******  end of file  ******/
