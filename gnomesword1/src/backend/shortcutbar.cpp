/*
 * GnomeSword Bible Study Tool
 * shortcutbar.cpp - support for Sword commentary modules
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
#  include <config.h>
#endif

#include <gnome.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <gbfplain.h>
#include <plainhtml.h>
#include <rwphtml.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "display.h"
#include "shortcutbar.h"
#include "sword.h"
#include "support.h"
#include "settings.h"

extern gint groupnum7;

list < string > sbfiles;

static SWDisplay *display;	//-- to display modules in viewer 
static SWMgr *mgr;
static SWModule *mod;	//-- module for viewer 

int backend_sb_type_from_modname(char * mod_name)
{
    ModMap::iterator it;	//-- iteratior
    SWMgr *mgr;
    gint retval = -1;
    gchar *modType;


    mgr = new SWMgr();		//-- create sword mgrs
	/*** get module type ***/
    it = mgr->Modules.find(mod_name);	//-- iterate through the modules until we find modName
    if (it != mgr->Modules.end()) {
	modType = (*it).second->Type();

	if (!strcmp(modType, TEXT_MODS)) {
	    retval = 0;
	}

	if (!strcmp(modType, COMM_MODS)) {
	    retval = 1;
	}

	if (!strcmp(modType, DICT_MODS)) {
	    retval = 2;
	}

	if (!strcmp(modType, BOOK_MODS)) {
	    retval = 3;
	}
    }
	/*** delete Sword manager ***/
    delete mgr;
    return retval;
}

void backend_save_sb_iconsize(char * filename, char * large_icons)
{
    char conffile[256];
    int j = 0;

    sprintf(conffile, "%s/%s", settings.shortcutbarDir, filename);
    SWConfig sbInfo(conffile);
    sbInfo["Shortcut Info"]["Large Icon"] = large_icons;
    sbInfo.Save();
}


GList *backend_load_sb_group(char * filename, char * group_name,
			 char * use_largeicons)
{
    SectionMap::iterator sit;
    ConfigEntMap::iterator eit;
    SWConfig *sbconf;
    ConfigEntMap emap;
    char *buf, conffile[256];
    int j = 0;
    GList *list;

    list = NULL;
    sprintf(conffile, "%s/%s", settings.shortcutbarDir, filename);

    SWConfig sbInfo(conffile);
    sprintf(group_name, "%s",
	    sbInfo["Shortcut Info"]["Group Name"].c_str());
    sprintf(use_largeicons, "%s",
	    sbInfo["Shortcut Info"]["Large Icon"].c_str());
    

    sbconf = new SWConfig(conffile);
    if ((sit = sbconf->Sections.find("ROOT")) != sbconf->Sections.end()) {
	if ((eit = (*sit).second.begin()) != (*sit).second.end()) {
	    for (eit = (*sit).second.begin(); eit != (*sit).second.end();
		 eit++) {
		buf = g_strdup((char *) (*eit).second.c_str());
		list = g_list_append(list, buf);
	    }
	}
    }
    delete sbconf;
    return list;
}

void backend_save_sb_group(char * filename, char * group_name, int group_num,
		  char * large_icons)
{
    SectionMap::iterator sit;
    ConfigEntMap::iterator eit;
    SWConfig *sbconf;
    ConfigEntMap emap;
    char buf[500], conffile[256], *item_url, *item_name;
    int j = 0, number_of_items;

    sprintf(conffile, "%s/%s", settings.shortcutbarDir, filename);
    unlink(conffile);

    SWConfig sbInfo(conffile);
    sbInfo["Shortcut Info"]["Group Name"] = group_name;
    sbInfo["Shortcut Info"]["Large Icon"] = large_icons;
    sbInfo.Save();

    sbconf = new SWConfig(conffile);
    emap = sbconf->Sections["ROOT"];

    number_of_items =
	get_num_shortcut_items(settings.shortcut_bar, group_num);

    for (j = 0; j < number_of_items; j++) {
	get_shortcut_item_info(settings.shortcut_bar,
					group_num,
					j, &item_url, &item_name);
	sprintf(buf, "branch%d", j);
	emap.erase(buf);
	emap.insert(ConfigEntMap::value_type(buf, (char *) item_name));
	g_print("saving list item: %s\n", (char *) item_name);
    }

    sbconf->Sections["ROOT"] = emap;
    sbconf->Save();
    delete sbconf;
}

/*GList *backend_get_sb_mod_list(char * modtype)
{
    GList *list;
    ModMap::iterator it;
    SWMgr *mgr;
    char *buf;

    mgr = new SWMgr();		//-- create sword mgrs
    list = NULL;
    for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
	if (!strcmp((*it).second->Type(), modtype)) {
	    buf = g_strdup((char *) (*it).second->Description());
	    list = g_list_append(list, buf);	// calling function must free each item 
	}
    }
    delete mgr;
    return list;
}
*/
/*
 * 
 * 
 */
void backend_display_sb_dictlex(char * modName, char * key)
{
    	if (!strcmp(modName, mod->Name())) {
	    mod->SetKey(key);	//-- set key to the first one in the list
	    mod->Display();
	} else {
	    ModMap::iterator it;
	    it = mgr->Modules.find(modName);	//-- iterate through the modules until we find modName - modName was passed by the callback
	    if (it != mgr->Modules.end()) {	//-- if we find the module       
		mod = (*it).second;	//-- change module to new module
		mod->SetKey(key);	//-- set key
		mod->Display();
	    }
	}    
}

/****************************************************************************************
 *setupVLSWORD - set up the sword stuff for the viewer dialog
 ****************************************************************************************/
void backend_setup_viewer(GtkWidget * html_widget)
{
    ModMap::iterator it;	//-- iteratior     
    SectionMap::iterator sit;	//-- iteratior

    mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
    mod = NULL;
    display = new GtkHTMLEntryDisp(html_widget, &settings);

    for (it = mgr->Modules.begin();
	 it != mgr->Modules.end(); it++) {
	mod = (*it).second;
	if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")) {
	    mod->Disp(display);
	}
    }
}

/*** close down viewer dialog ***/
void backend_shutdown_sb_viewer(void)
{
    delete mgr;
    if (display)
	delete display;
}
