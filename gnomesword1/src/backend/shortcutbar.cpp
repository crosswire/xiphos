/*
 * GnomeSword Bible Study Tool
 * shortcutbar.cpp - functions for saving and loading shortcutbar groups
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
#include <versekey.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "main/gs_gnomesword.h"
#include "main/settings.h"

#include "backend/shortcutbar.h"


/******************************************************************************
 * Name
 *   backend_save_sb_iconsize
 *
 * Synopsis
 *   #include "backend/shortcutbar.h"
 *
 *   void backend_save_sb_iconsize(char * filename, char * large_icons)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_save_sb_iconsize(char * filename, char * large_icons)
{
    char conffile[256];
    int j = 0;

    sprintf(conffile, "%s/%s", settings.shortcutbarDir, filename);
    SWConfig sbInfo(conffile);
    sbInfo["Shortcut Info"]["Large Icon"] = large_icons;
    sbInfo.Save();
}

/******************************************************************************
 * Name
 *   backend_load_sb_group
 *
 * Synopsis
 *   #include "backend/shortcutbar.h"
 *
 *   GList *backend_load_sb_group(char * filename, char * group_name,
			 char * use_largeicons)
 *
 * Description
 *   
 *
 * Return value
 *   GList *
 */

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

/******************************************************************************
 * Name
 *   backend_save_sb_group
 *
 * Synopsis
 *   #include "backend/shortcutbar.h"
 *
 *   void backend_save_sb_group(char * filename, char * group_name, 
 *			int group_num, char * large_icons)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void backend_save_sb_group(char * filename, char * group_name, 
			int group_num, char * large_icons)
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


/******************************************************************************
 * Name
 *   backend_get_verse_list
 *
 * Synopsis
 *   #include "backend/shortcutbar.h"
 *
 *   GList * backend_get_verse_list(gchar *module_name, gchar *vlist, SETTINGS *s)	
 *
 * Description
 *   get a list of verses from a string
 *
 * Return value
 *   GList *
 */

GList *backend_get_verse_list(char * module_name, char * vlist)
{
	GList *retval = NULL;
	char firstkey[256];
	ListKey tmp_verse_list;
	VerseKey default_verse_key;
	int count = 0;

	for (int i = 0; i < strlen(vlist); i++) {
		if (vlist[i] == '+')
			vlist[i] = ' ';
		if (vlist[i] == ',')
			vlist[i] = ';';
	}
	tmp_verse_list =
	    default_verse_key.ParseVerseList((char *) vlist,
					     default_verse_key);

	while (!tmp_verse_list.Error()) {
		retval =
		    g_list_append(retval,
				  g_strdup((const char *)
					   tmp_verse_list));
		if (!count)
			sprintf(firstkey, "%s",
				(const char *) tmp_verse_list);
		tmp_verse_list++;
		++count;
	}
	return retval;
}


