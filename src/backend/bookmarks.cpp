/*
 * GnomeSword Bible Study Tool
 * bookmarks.cpp- functions to load and save bookmarks
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
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "backend/bookmarks.h"

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
 *   add_section_to_conf
 *
 * Synopsis
 *   #include "backend/bookmarks.h"
 *
 *   void add_section_to_conf(SWConfig * config, string section,
 *                     GNode * node, int index)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void add_section_to_conf(SWConfig * config, string section,
                      GNode * node, int index)
{
        GNode *work;
        ConfigEntMap sit;
        char buf[15], tmpbuf[500];
        static int i = 0;

        ExportStruct * es = g_new (ExportStruct, 1);
	es = (ExportStruct *)node->data;
	
        if (!es->is_leaf) {
                sit = config->Sections[section.c_str()];
                for (work = node->children; work;
                     work = work->next) {
			es = (ExportStruct *)work->data;     
                        ++i;
                        sprintf(buf, "branch%d", i);
                        sprintf(tmpbuf, "%s|%s|%s",
                                es->label,
				es->key,
				es->module);
			     
                        sit.erase(buf);
                        sit.insert(ConfigEntMap::
                                   value_type(buf, tmpbuf));
                        add_section_to_conf(config, buf, work, i);
                }
                config->Sections[section.c_str()] = sit;
        }
}


/******************************************************************************
 * Name
 *   backend_save_bookmarks
 *
 * Synopsis
 *   #include "backend/bookmarks.h"
 *
 *   void backend_save_bookmarks(GNode * bookmark_tree)
 *
 * Description
 *   save gnomesword bookmarks - using sword SWConfig
 *
 * Return value
 *   void
 */

void backend_save_bookmarks(GNode * bookmark_tree, char *dir)
{
        int is_leaf;
        char persfile[256], buf[80], tmpbuf[500], conffile[256];
        SWConfig *bmconf;
        ConfigEntMap emap;
        SectionMap::iterator sit;
        list < string >::iterator it;
	int x = 0, i = 0;
        sprintf(persfile, "%spersonal.conf", dir);

        /*** delete all bookmark files before saving in case a top level was deleted ***/
        for (it = bmfiles.begin(); it != bmfiles.end(); it++) {
                if ((!strcmp((*it).c_str(), persfile)) || (strcmp((*it).c_str(), persfile)))    /* we used this code so we can add autosave options */
                        unlink((*it).c_str());
        }
	
	GNode * gnode = g_node_first_child(bookmark_tree);
        ExportStruct * es = g_new (ExportStruct, 1);
        
	 es = (ExportStruct *)gnode->data;        
		
	
	if (es->is_leaf) {
		sprintf(conffile, "%s%s.conf", dir, "root");
		bmconf = new SWConfig(conffile);
		emap = bmconf->Sections["ROOT"];
		sprintf(buf, "branch%d", x++);
		sprintf(tmpbuf, "%s|%s|%s",
			es->label,
			es->key,
			es->module);
		
		emap.erase(buf);
		emap.
		    insert(ConfigEntMap::
			   value_type(buf, tmpbuf));
		bmconf->Sections["ROOT"] = emap;
		bmconf->Save();
		delete bmconf;
	}
	
	else {
		if(!strcmp(es->module,"ROOT")) {
		
			sprintf(conffile, "%s%s", dir,es->key);
			bmconf = new SWConfig(conffile);
			emap = bmconf->Sections["ROOT"];
			sprintf(buf, "branch%d", i++);
			sprintf(tmpbuf, "%s|%s|%s",
				es->label,
				es->key,
				es->module);
				
			emap.erase(buf);
			emap.
			    insert(ConfigEntMap::
				   value_type(buf, tmpbuf));
			add_section_to_conf(bmconf, buf, gnode, i);
			bmconf->Sections["ROOT"] = emap;
			bmconf->Save();
			delete bmconf;
		}
	}
	
	while((gnode = g_node_next_sibling(gnode)) != NULL) {
		es = (ExportStruct *)gnode->data; 
		if (es->is_leaf) {
			sprintf(conffile, "%s%s.conf", dir, "root");
			bmconf = new SWConfig(conffile);
			emap = bmconf->Sections["ROOT"];
			sprintf(buf, "branch%d", x++);
			sprintf(tmpbuf, "%s|%s|%s",
				es->label,
				es->key,
				es->module);
			
			emap.erase(buf);
			emap.
			    insert(ConfigEntMap::
				   value_type(buf, tmpbuf));
			bmconf->Sections["ROOT"] = emap;
			bmconf->Save();
			delete bmconf;
		}
		
		else {
			sprintf(conffile, "%s%s", dir,es->key);
			bmconf = new SWConfig(conffile);
			emap = bmconf->Sections["ROOT"];
			sprintf(buf, "branch%d", i++);
			sprintf(tmpbuf, "%s|%s|%s",
				es->label,
				es->key,
				es->module);
				
			emap.erase(buf);
			emap.
			    insert(ConfigEntMap::
				   value_type(buf, tmpbuf));
			add_section_to_conf(bmconf, buf, gnode, i);
			bmconf->Sections["ROOT"] = emap;
			bmconf->Save();
			delete bmconf;
		}
	}
       
}


/******************************************************************************
 * Name
 *   backend_create_bookmarks
 *
 * Synopsis
 *   #include "backend/bookmarks.h"
 *
 *   void backend_create_bookmarks(char *dir)
 *
 * Description
 *   no bookmark dir was found on startup - so create one
 *
 * Return value
 *   void
 */

void backend_create_bookmarks(char *dir)
{
        char buf[255];

        sprintf(buf, "%s/personal.conf", dir);
        SWConfig bmInfo(buf);
        /** seperators are '|' the pipe char **/
        bmInfo["ROOT"]["branch0"] = "Personal|personal.conf|ROOT";

        bmInfo["branch0"]["branch1"] =
            "[What must I do to be saved?]|GROUP|GROUP";
        bmInfo["branch0"]["branch5"] =
            "[What is the Gospel?]|GROUP|GROUP";
        bmInfo["branch0"]["branch7"] =
            "Romans 8:28 KJV|Romans 8:28|KJV";
        bmInfo["branch0"]["branch8"] = "Rev 1:5 KJV|Rev 1:5|KJV";

        bmInfo["branch1"]["branch2"] =
            "Romans 1:16 KJV|Romans 1:16|KJV";
        bmInfo["branch1"]["branch3"] = "Eph 2:8 KJV|Eph 2:8|KJV";
        bmInfo["branch1"]["branch4"] = "Acts 16:31 KJV|Acts 16:31|KJV";

        bmInfo["branch5"]["branch6"] = "1 Cor 15:1 KJV|1 Cor 15:1|KJV";

        bmInfo.Save();
}



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
 *   
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
//			g_warning((*eit).first.c_str());
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
 *   #include "backend/bookmarks.h"
 *
 *   GNode * backend_load_bookmarks(char *dir)
 *
 * Description
 *   load bookmarks - using sword SWConfig
 *   most of this code is form sword-1.5.2 bibleCS bookmarkfrm.cpp
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
        root_node = g_node_insert(root_node,-1,root_node);
	
        t = "|";
        sprintf(conffile, "%spersonal.conf", dir);
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
                                sprintf(conffile, "%s%s", dir,
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
	
        sprintf(conffile, "%sroot.conf", dir);
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

/*
	<scripRef version=\"KJV\" passage=\"John 3:16\">John 3:16</scripRef>
	
*/
