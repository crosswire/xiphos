/*
 * GnomeSword Bible Study Tool
 * bookmarks.cpp- functions to load old style bookmarks
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

#include <glib-1.2/glib.h>
#include <swmgr.h>
#include <swconfig.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "backend/bookmarks.h"


using std::string;
using std::list;
using namespace sword;

list < string > bmfiles;

struct _ExportStruct {
  char *label;
  char *key;
  char *module;
  int is_leaf;
};

typedef struct _ExportStruct ExportStruct;


/******************************************************************************
 * Name
 *   add_section
 *
 * Synopsis
 *   #include "backend/bookmarks.h"
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
 * Synopsisbackend_load_bookmarks(char *dir)
 *   #include "backend/bookmarks.h"
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
                        *bmfiles.insert(bmfiles.begin(), conffile);
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
						*bmfiles.insert(bmfiles.begin(), conffile);
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
			*bmfiles.insert(bmfiles.begin(), conffile);
			add_section(bookmarkInfo,
				   (*eit).first.c_str(), node); 
                }
        }
        delete bookmarkInfo;        
	
	return root_node;
}
