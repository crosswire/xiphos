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
#include "main/sword.h"


/******************************************************************************
 * Name
 *   backend_save_custom_modlist
 *
 * Synopsis
 *   #include "backend/config.hh"
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

void erase_config_section(char *section)
{
	backend_erase_config_section(section);
}

int close_config_file(void)
{
	return backend_close_config_file();
}

void add_to_config_file(char * section, 
					char * label, char * value)
{
	backend_add_to_config_file(section,  label, value)	;
	
}

const char *get_config_value(char * section, char * label)
{
	return backend_get_config_value(section, label);
	
}

int set_config_to_get_labels(char * section)
{
	backend_set_config_to_get_labels(section);
}

const char *get_next_config_label(void)
{
	return backend_get_next_config_label();
}

