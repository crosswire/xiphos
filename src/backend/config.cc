/*
 * GnomeSword Bible Study Tool
 * config.cc - search Sword modules
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
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swmodule.h>
#include <swconfig.h>
#include <versekey.h>
#include <regex.h>
#include <pthread.h>
#include <string.h>

#include "backend/config.hh"
#include "backend/sword_defs.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/search.h"

using std::string;
using std::map;
using std::list;
using namespace sword;


static SWConfig *config;
static ConfigEntMap::iterator loop, end;
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
 
int backend_open_config_file(char *file)
{
	config = new SWConfig(file);
	if(config)
		return 1;
	else
		return 0;
	
}

void backend_erase_config_section(char *section)
{
	config->Sections[section].erase(
		config->Sections[section].begin(), 
		config->Sections[section].end());
}

int backend_close_config_file(void)
{
	config->Save();	
	delete config;
	return 0;
}

void backend_add_to_config_file(char * section, 
					char * label, char * value)
{
	config->Sections[section].insert(
			ConfigEntMap::value_type(label, value));
}

const char *backend_get_config_value(char * section, char * label)
{
	ConfigEntMap::iterator loop, end;
	
	loop = config->Sections[section].begin();
	end = config->Sections[section].end();
	while (loop != end) {
		if(!strcmp(loop->first.c_str(),label))
			return loop->second.c_str();
		loop++;
	}
	return NULL; 
}

int backend_set_config_to_get_labels(char * section)
{
	loop = config->Sections[section].begin();
	end = config->Sections[section].end();
	if(loop != end)
		return 1;
	else 
		return 0;
}

const char *backend_get_next_config_label(void)
{
	while (loop != end) {
		return (loop++)->first.c_str();
	}
	return NULL; 
}

