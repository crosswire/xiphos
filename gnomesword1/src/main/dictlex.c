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

#include "gui/dictlex_dialog.h"
#include "gui/dictlex.h"
#include "gui/cipher_key_dialog.h"
#include "gui/html.h"

#include "main/settings.h"
#include "main/dictlex.h"
#include "main/gs_gnomesword.h"

/******************************************************************************
 * Name
 *  set_dictlex_module 
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   int set_dictlex_module(char * mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */
 
int set_dictlex_module(char * mod_name)
{
	return backend_set_dictlex_module(mod_name);
}

/******************************************************************************
 * Name
 *   set_dictlex_key
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   int set_dictlex_key(char * key)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */
 
int set_dictlex_key(char * key)
{
	return backend_set_dictlex_key(key);
}

/******************************************************************************
 * Name
 *   get_dictlex_key
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   char *get_dictlex_key(int direction)
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *get_dictlex_key(int direction)
{
	return backend_get_dictlex_key(direction);
}

/******************************************************************************
 * Name
 *   get_dictlex_text
 *
 * Synopsis
 *   #include "dictlex.h"
 *
 *   char *get_dictlex_text(char * mod_name, char * key)
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */

char *get_dictlex_text(char * mod_name, char * key)
{
	return backend_get_dictlex_text(mod_name, key);
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

gchar *get_first_dictlex_key(void)
{
	return backend_get_first_dictlex_key();
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

gchar *get_next_dictlex_key(void)
{
	return backend_get_next_dictlex_key();	
}
