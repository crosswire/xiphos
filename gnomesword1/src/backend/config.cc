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
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>

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
 
/*** most of this code is from an example in swmgr.h sword-1.5.2 ***/
void backend_save_module_key(char *mod_name, char *key)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	DIR *dir;
	char buf[256], conffile[256];
	struct dirent *ent;

	strcpy(buf, sw.main_mgr->configPath);
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
						entry->second = key;	//-- set cipher key
						myConfig->Save();	//-- save config file
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
 
void backend_load_font_info(MOD_FONT * mf, char * dir)
{
	char buf[255];
	char *tmpbuf = NULL;

	sprintf(buf, "%s/fonts.conf", dir);
	SWConfig module_options(buf);
	module_options.Load();

	mf->old_font =
	    (char *) module_options[mf->mod_name]["Font"].c_str();
	mf->old_gdk_font =
	    (char *) module_options[mf->mod_name]["GdkFont"].c_str();
	mf->old_font_size =
	    (char *) module_options[mf->mod_name]["Fontsize"].c_str();
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
 
void backend_save_font_info(MOD_FONT * mf, char * dir)
{
	char buf[80], buf2[255];

	sprintf(buf, "%s/fonts.conf", dir);
	SWConfig module_options(buf);

	module_options[mf->mod_name]["Font"] = mf->new_font;
	module_options[mf->mod_name]["GdkFont"] = mf->new_gdk_font;
	module_options[mf->mod_name]["Fontsize"] = mf->new_font_size;

	module_options.Save();
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
 

int backend_load_module_options(char * modName, char * option, 
					char * dir, char * conf)
{
	char buf[255], *yesno;
	bool retval = false;

	sprintf(buf, "%s/%s", dir, conf);
	SWConfig module_options(buf);
	module_options.Load();
	yesno = (char *) module_options[modName][option].c_str();
	if (!strcmp(yesno, "On"))
		retval = true;
	else
		retval = false;
	return retval;
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
 
int backend_save_module_options(char * modName, char * option, 
			char * value, char * dir, char * conf)
{
	char buf[80], buf2[255];

	sprintf(buf, "%s/%s", dir, conf);
	SWConfig module_options(buf);

	module_options[modName][option] = value;

	module_options.Save();
	return true;
}
