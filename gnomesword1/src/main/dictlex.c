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

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif


#include "backend/sword.h"

#include "main/dictlex.h"


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
 *    int manager, char * module_name, int direction
 *
 * Return value
 *   char *
 */
 
char *get_dictlex_key(int manager, char * module_name, int direction)
{
	return backend_nav_module(manager, module_name, direction);
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

char *get_dictlex_text(char * module_name, char * key)
{
	return backend_get_module_text(2, module_name, key);
}

