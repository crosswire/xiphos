/*
 * GnomeSword Bible Study Tool
 * sword.cpp support for sword modules
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

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#include <glib-1.2/glib.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swversion.h>
#include <swmodule.h>
#include <swconfig.h>
#include <versekey.h>
#include <localemgr.h>
#include <dirent.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"

#include "backend/sword.h"
#include "backend/sword_defs.h"
#include "backend/shortcutbar.h"
#include "backend/properties.h"
#include "backend/bookmarks.h"
#include "backend/gbs_.h"
#include "backend/dictlex_.h"
#include "backend/commentary_.h"
#include "backend/bibletext_.h"
#include "backend/percomm_.h"
#include "backend/interlinear.h"

using std::string;
using std::map;
//using std::list;
using namespace sword;

typedef map < string, string > modDescMap;
//typedef map < string, string > bookAbrevMap;


#define CIPHER_KEY_LEN 16



/******************************************************************************
 * externs
 */

extern SWMgr *main_mgr;	

/******************************************************************************
 * static  global to this file only 
 */


/******************************************************************************
 * globals
 */

//bookAbrevMap abrevationMap;
MANAGERS sw_mgr;

modDescMap descriptionMap;


/******************************************************************************
 * Name
 *   backend_init
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_init(void)	
 *
 * Description
 *   start sword 
 *
 * Return value
 *   void
 */
 
void backend_init(void)
{	
	ModMap::iterator it;
	g_print("gnomesword-%s\n", VERSION);
	g_print("%s\n", "Initiating Sword\n");		
	g_print("Sword locale is %s\n",
		LocaleMgr::systemLocaleMgr.getDefaultLocaleName());
	g_print("%s\n", "Checking for SWORD Modules");
	
	/*
	 *create sword mgrs
	 */
	main_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw_mgr.search = new SWMgr();
	sw_mgr.results = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	
	for (it = main_mgr->Modules.begin();
	     it != main_mgr->Modules.end(); it++) {
		descriptionMap[string
			((char *) (*it).second->Description())] =
				  string((char *) (*it).second->Name());
	 }
	/* 
	 * fill module lists
	 */
	init_lists();
	
	/*
	 *   setup Commentary, Personal Comments
	 *   Generic Book and Dict/Lex Support
	 */
	if(settings.havebible)
		backend_setup_bibletext();
	if(settings.havecomm)
		backend_setup_commentary();
	if(settings.havepercomm)
		backend_setup_percomm();
	if(settings.havedict)
		backend_setup_dictlex();
	if(settings.havebook)
		backend_setup_books();
	if(settings.havebible)
		backend_setup_interlinear();
}


/******************************************************************************
 * Name
 *   backend_module_name_from_description
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_module_name_from_description(gchar * mod_name, gchar * mod_desc)	
 *
 * Description
 *   get mod name from mod description
 *
 * Return value
 *   void
 */

void backend_module_name_from_description(char * mod_name,
					  char * mod_desc)
{
	strcpy(mod_name, descriptionMap[mod_desc].c_str());
}


/******************************************************************************
 * Name
 *   backend_shutdown
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_shutdown(void)	
 *
 * Description
 *   close down sword 
 *
 * Return value
 *   void
 */

void backend_shutdown(int save_properties)
{
	if(save_properties)
		backend_save_properties(true);
	
	backend_shutdown_bibletext();
	backend_shutdown_commentary();
	backend_shutdown_percomm();
	backend_shutdown_dictlex();
	backend_shutdown_books();
	backend_shutdown_interlinear();
	
	/*
	 * delete Sword managers
	 */
	delete main_mgr;
	delete sw_mgr.search;
	delete sw_mgr.results; 
	g_print("\nSword is shutdown\n");
}

/*** returns the version number of the sword libs ***/
const char *backend_get_sword_version(void)
{
	SWVersion retval;
	retval = SWVersion::currentVersion;
	return retval;
}

/*** most of this code is from an example in swmgr.h sword-1.5.2 ***/
void backend_save_module_key(char *mod_name, char *key)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	DIR *dir;
	char buf[256], conffile[256];
	struct dirent *ent;

	strcpy(buf, main_mgr->configPath);
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

/*** we come here to get module type - Bible text, Commentary, Dict/Lex or Book ***/
int backend_get_mod_type(char * mod_name)
{

	ModMap::iterator it;	//-- iteratior
	SWMgr *mgr;

	mgr = new SWMgr();	//-- create sword mgrs
	it = mgr->Modules.find(mod_name);	//-- iterate through the modules until we find modName
	if (it != mgr->Modules.end()) {	//-- if we find the module   

		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
		    /*** delete Sword manager ***/
			delete mgr;
			return TEXT_TYPE;
		}

		if (!strcmp((*it).second->Type(), COMM_MODS)) {
		    /*** delete Sword manager ***/
			delete mgr;
			return COMMENTARY_TYPE;
		}

		if (!strcmp((*it).second->Type(), DICT_MODS)) {
		    /*** delete Sword manager ***/
			delete mgr;
			return DICTIONARY_TYPE;
		}

		if (!strcmp((*it).second->Type(), BOOK_MODS)) {
		    /*** delete Sword manager ***/
			delete mgr;
			return BOOK_TYPE;
		}
	}
	/*** delete Sword manager ***/
	delete mgr;
	return -1;
}


char *backend_get_module_description(char * modName)
{
	ModMap::iterator it;	//-- iteratior

	it = main_mgr->Modules.find(modName);
	if (it != main_mgr->Modules.end()) {
		return (*it).second->Description();
	}
	return NULL;
}

/***  returns path to sword modules must be freed by calling function  ***/
char *backend_get_path_to_mods(void)
{
	return g_strdup(main_mgr->prefixPath);
}

/******************************************************************************
 * Name
 *   backend_get_mod_about_info
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   gchar *backend_get_mod_about_info(gchar *modname)
 *
 * Description
 *   return the about information from the <module>.conf
 *
 * Return value
 *   gchar *
 */
char *backend_get_mod_about_info(char * modname)
{
	return g_strdup((char *) main_mgr->Modules[modname]->
			getConfigEntry("About"));
}

int backend_get_module_page(char *module_name, char *module_type)
{
	ModMap::iterator it;
	int module_index = 0;

	for (it = main_mgr->Modules.begin();
	     it != main_mgr->Modules.end(); it++) {

		if (!strcmp((*it).second->Type(), module_type)) {

			if (!strcmp((*it).second->Name(), module_name)) {
				return module_index;
			}
			++module_index;
		}
	}
	return -1;
}

int backend_module_is_locked(char *mod_name)
{
	char *tmpbuf = (char *) main_mgr->Modules[mod_name]->
				getConfigEntry("CipherKey");
	if(tmpbuf != NULL) {
		if (strlen(tmpbuf) == CIPHER_KEY_LEN) {
		/* the key is the right length so we assume it is right */
			return false;
		}
		else
			return true;
	}
	
}

char *backend_get_cipher_key(char *mod_name)
{
	return strdup((char *) main_mgr->Modules[mod_name]->
				getConfigEntry("CipherKey"));
}

int backend_has_cipher_tag(char *mod_name)
{
	char *tmpbuf = (char *) main_mgr->Modules[mod_name]->
				getConfigEntry("CipherKey");	
	if(tmpbuf != NULL)
		return 1;
	else 
		return 0;
}


/******************************************************************************
 * Name
 *   backend_get_module_text
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *backend_get_module_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *backend_get_module_text(char * mod_name, char * key)
{
	SWModule *mod = main_mgr->Modules[mod_name];
	
	if(mod) {
		mod->SetKey(key);
		return strdup((char*)mod->RenderText());
	}
	return NULL;
}

/******************************************************************************
 * Name
 *   backend_check_for_module
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   int backend_check_for_module(char * mod_name)
 *
 * Description
 *    check for presents of a module by name
 *
 * Return value
 *   int
 */
 
int backend_check_for_module(char * mod_name)
{
	SWModule *mod = main_mgr->Modules[mod_name];
	
	if(mod) {
		
		return 1;
	}
	return 0;
}


/******************************************************************************
 * Name
 *   backend_get_striptext
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   char *backend_get_striptext(char *mod_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */

char *backend_get_striptext(char *mod_name, char *key)
{
	SWModule *mod = main_mgr->Modules[mod_name];
	if (mod)
		mod->SetKey(key);
	else
		return NULL;
	return strdup((char *) mod->StripText());
}

