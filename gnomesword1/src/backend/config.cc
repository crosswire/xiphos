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

#include "backend/config.hh"
#include "backend/sword_defs.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/search.h"

//using std::string;
//using std::map;
//using std::list;
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

//list < String *> bmfiles;

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


/******************************************************************************
 * Name
 *   add_section
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   void add_section(SWConfig * config,
 *               const gchar * section, GNode * parent)
 *
 * Description
 *   this function is required by backend_load_bookmarks()
 *   - this is for compatibility with GnomeSword-0.7.x -
 *
 * Return value
 *   void
 */

static void add_section(SWConfig * config,
                const char * section, GNode * parent)
{
        SectionMap::iterator sit;
        ConfigEntMap::iterator eit;
        char * t;
	
        t = "|";
        if ((sit = config->Sections.find(section)) != config->Sections.end()) {
                for (eit = (*sit).second.begin(); eit != (*sit).second.end(); eit++) {
			
			ExportStruct * es = g_new (ExportStruct, 1);
                        char *token;
			token =
                            strtok((char *) (*eit).second.c_str(), t);
                        es->label = strdup(token);
                        token = strtok(NULL, t);
                        es->key = strdup(token);
                        token = strtok(NULL, t);
                        es->module = strdup(token);
			     
                        if (!strcmp(es->key, "GROUP")) {
                                es->is_leaf = false;				
			}
                        else {
                                es->is_leaf = true;
			}
			
			GNode *node = NULL; 
			node = g_node_append_data(parent, (ExportStruct *)es);	
                        add_section(config, (*eit).first.c_str(),
                                   node);
                }
        }
}


/******************************************************************************
 * Name
 *   backend_load_bookmarks
 *
 * Synopsis
 *   #include "backend/config.hh"
 *
 *   GNode * backend_load_bookmarks(char *dir)
 *
 * Description
 *   load bookmarks - using sword SWConfig
 *   most of this code is form sword-1.5.2 bibleCS bookmarkfrm.cpp
 *   - this is for compatibility with GnomeSword-0.7.x -
 *
 * Return value
 *   GNode *
 */

GNode * backend_load_bookmarks(char *dir)
{
	SWConfig *bookmarkInfo;
        SectionMap::iterator sit;
        ConfigEntMap::iterator eit;
        char *t, conffile[500];
        DIR *directory;
        struct dirent *ent;
        int i;
	int parent = 0;
	int node = 0;
	GNode * root_node = NULL;
	
	
	/* set up root node */
	ExportStruct * es = g_new (ExportStruct, 1);
	es->label = strdup("Bookmarks");
	es->key = strdup("root");
	es->module = strdup("root");
	es->is_leaf = false;	    
	root_node = g_node_new((ExportStruct *)es);	
        //root_node = g_node_insert(root_node,-1,root_node);
	
        t = "|";
        sprintf(conffile, "%s/personal.conf", dir);
        if (access(conffile, F_OK) == -1)
                return NULL;
        bookmarkInfo = new SWConfig(conffile);
        if ((sit =
             bookmarkInfo->Sections.find("ROOT")) !=
            bookmarkInfo->Sections.end()) {
                if ((eit =
                     (*sit).second.begin()) != (*sit).second.end()) {
			ExportStruct * es = g_new (ExportStruct, 1);
                        char *token;
                        token =
                            strtok((char *) (*eit).second.c_str(), t);
                        es->label = strdup(token);
                        token = strtok(NULL, t);
                        es->key = strdup(token);
                        token = strtok(NULL, t);
                        es->module = strdup(token);
			es->is_leaf = false;  		
			GNode * gnode = NULL;	
                        gnode = g_node_append_data(root_node, (ExportStruct *)es);
//                        *bmfiles.insert(bmfiles.begin(), conffile);
                        add_section(bookmarkInfo,
                                   (*eit).first.c_str(), gnode);
			     
                }
        }
        delete bookmarkInfo;

        if (directory = opendir(dir)) {
                rewinddir(directory);
                while ((ent = readdir(directory))) {
                        if ((strcmp(ent->d_name, "root.conf"))
                            && (strcmp(ent->d_name, "personal.conf"))
                            && (strcmp(ent->d_name, "."))
                            && (strcmp(ent->d_name, ".."))) {
                                sprintf(conffile, "%s/%s", dir,
                                        ent->d_name);
                                bookmarkInfo = new SWConfig(conffile);
                                if ((sit =
                                     bookmarkInfo->Sections.
                                     find("ROOT")) !=
                                    bookmarkInfo->Sections.end()) {
                                        if ((eit = (*sit).second.begin()) != 
							(*sit).second.end()) {  
                                                ExportStruct * es = g_new (ExportStruct, 1);
						char *token;
						token =
						    strtok((char *) (*eit).second.c_str(), t);
						es->label = strdup(token);
						token = strtok(NULL, t);
						es->key = strdup(token);
						token = strtok(NULL, t);
						es->module = strdup(token);
						es->is_leaf = false;
						GNode *node = NULL; 
						node = g_node_append_data(root_node, (ExportStruct *)es);	
//						*bmfiles.insert(bmfiles.begin(), conffile);
						add_section(bookmarkInfo,
							   (*eit).first.c_str(), node);
						
					}
				}
                                delete bookmarkInfo;
                        }
                }
                closedir(directory);
        }
	
        sprintf(conffile, "%s/root.conf", dir);
        if (access(conffile, F_OK) == -1)
                return root_node;
        bookmarkInfo = new SWConfig(conffile);
        if ((sit =
             bookmarkInfo->Sections.find("ROOT")) !=
            bookmarkInfo->Sections.end()) {
                for (eit = (*sit).second.begin();
                     eit != (*sit).second.end(); eit++) {  
                        ExportStruct * es = g_new (ExportStruct, 1);
			char *token;
			token =
			    strtok((char *) (*eit).second.c_str(), t);
			es->label = strdup(token);
			token = strtok(NULL, t);
			es->key = strdup(token);
			token = strtok(NULL, t);
			es->module = strdup(token);
			es->is_leaf = true;
			GNode *node = NULL; 
			node = g_node_append_data(root_node, (ExportStruct *)es);	
//			*bmfiles.insert(bmfiles.begin(), conffile);
			add_section(bookmarkInfo,
				   (*eit).first.c_str(), node); 
                }
        }
        delete bookmarkInfo;        
	
	return root_node;
}
