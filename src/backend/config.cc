/*
 * GnomeSword Bible Study Tool
 * config.cc - 
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

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif


#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swmodule.h>
#include <swconfig.h>
#include <versekey.h>
#include <regex.h>
#include <pthread.h>
#include <string>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
 
#include <glib-2.0/glib.h>

#include "config.hh"
#include "sword_main.hh"

#include "main/sword.h"

using namespace sword;
using namespace std;


struct _ExportStruct {
  char *label;
  char *key;
  char *module;
  int is_leaf;
};

typedef struct _ExportStruct ExportStruct;

 
static SWConfig *config;
static ConfigEntMap::iterator loop, end;

extern SWMgr *main_mgr;

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


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
void backend_erase_config_section(char *section)
{
	config->Sections[section].erase(
		config->Sections[section].begin(), 
		config->Sections[section].end());
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
int backend_close_config_file(void)
{
	config->Save();	
	delete config;
	return 0;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
void backend_add_to_config_file(char * section, 
					char * label, char * value)
{
	config->Sections[section].insert(
			ConfigEntMap::value_type(label, value));
}


/******************************************************************************
 * Name
 *   backend_save_value_to_config_file
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   void backend_save_value_to_config_file(char * section, 
 *					char * label, char * value)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
 
void backend_save_value_to_config_file(char * section, 
					char * label, char * value)
{
	config->Sections[section][label] = value;
	
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
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


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
int backend_set_config_to_get_labels(char * section)
{
	loop = config->Sections[section].begin();
	end = config->Sections[section].end();
	if(loop != end)
		return 1;
	else 
		return 0;
}


/******************************************************************************
 * Name
 *   backend_get_next_config_label
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   const char *backend_get_next_config_label(void)
 *
 * Description
 *   
 *
 * Return value
 *   const char*
 */
 
const char *backend_get_next_config_label(void)
{
	while (loop != end) {
		return (loop++)->first.c_str();
	}
	return NULL; 
}


/******************************************************************************
 * Name
 *   backend_get_next_config_value
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   const char *backend_get_next_config_value(void)
 *
 * Description
 *   
 *
 * Return value
 *   const char*
 */
 
const char *backend_get_next_config_value(void)
{
	while (loop != end) {
		return strdup((loop++)->second.c_str());
	}
	return NULL; 
}


/******************************************************************************
 * Name
 *   backend_save_module_key
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   void backend_save_module_key(char *mod_name, char *key)
 *
 * Description
 *   most of this code is from an example in swmgr.h sword-1.5.2
 *
 * Return value
 *   void
 */
 
void backend_save_module_key(char *mod_name, char *key)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	return; // this needs help
	
	DIR *dir;
	char buf[256], conffile[256];
	struct dirent *ent;

//	strcpy(buf, sw.main_mgr->configPath);
	dir = opendir(buf);
	if (dir) {		//-- find and update .conf file
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, "."))
			    && (strcmp(ent->d_name, ".."))) {
				sprintf(conffile, "%s/%s", buf,
					ent->d_name);
				SWConfig *myConfig =
				    new SWConfig(conffile);
				section =
				    myConfig->Sections.find(mod_name);
				if (section != myConfig->Sections.end()) {
					entry =
					    section->second.
					    find("CipherKey");
					if (entry !=
					    section->second.end()) {
						//-- set cipher key
						entry->second = key;
						//-- save config file						    
						myConfig->Save();
					}
				}
				delete myConfig;
			}
		}
	}
	closedir(dir);
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
char *backend_get_module_font_name(char *mod_name, char * dir)
{
	char file[255];
	char *buf = NULL;

	sprintf(file, "%s/fonts.conf", dir);
	SWConfig module_options(file);
	module_options.Load();

	buf =
	    (char *) module_options[mod_name]["Font"].c_str();
	if(buf)
		return strdup(buf);
	else
		return NULL;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
char *backend_get_module_font_size(char *mod_name, char * dir)
{
	char file[255];
	char *buf = NULL;

	sprintf(file, "%s/fonts.conf", dir);
	SWConfig module_options(file);
	module_options.Load();

	buf =
	    (char *) module_options[mod_name]["Fontsize"].c_str();
	if(buf)
		return strdup(buf);
	else
		return NULL;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
char *backend_get_conf_item(const char * file, const char * mod_name, const char * item)
{	
	char *buf = NULL;
	SWConfig conf_file(file);
	conf_file.Load();

	buf = (char *) conf_file[mod_name][item].c_str();
	if(strlen(buf))
		return strdup(buf);
	else
		return NULL;
}


/******************************************************************************
 * Name
 *   backend_save_conf_item
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   void backend_save_conf_item(char * file, char * mod_name, char * item,
 *				char * value)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
 
void backend_save_conf_item(const char * file, const char * mod_name, const char * item,
				const char * value)
{
	SWConfig conf_file(file);

	conf_file[mod_name][item] = value;
	conf_file.Save();
}
