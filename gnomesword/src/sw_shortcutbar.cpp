/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                                 sw_shortcutbar.cpp
                             -------------------
				  Sat June 30 2001
			copyright : (C) 2001 by Terry Biggs
 				tbiggs@users.sf.net   
 ***************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <swmgr.h>
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
#include "sw_shortcutbar.h"
#include "support.h"

extern gchar *shortcutbarDir;
extern SETTINGS *settings;
list <string> sbfiles;	

void
save_iconsizeSW(gchar *filename, gchar *large_icons)
{
	gchar 
		conffile[256];
	gint 
		j=0;
	
	sprintf(conffile,"%s/%s", shortcutbarDir, filename);	
	SWConfig sbInfo(conffile);
	sbInfo["Shortcut Info"]["Large Icon"] = large_icons;	
	sbInfo.Save();
}


GList *loadshortcutbarSW(gchar *filename, gchar *group_name, gchar *use_largeicons)
{
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	SWConfig *sbconf;
	ConfigEntMap emap;
	gchar 
		*buf,
		conffile[256];
	gint j=0;
	GList *list;
	
	list = NULL;
	sprintf(conffile,"%s/%s", shortcutbarDir, filename);
	
	SWConfig sbInfo(conffile);
	sprintf(group_name,"%s", sbInfo["Shortcut Info"]["Group Name"].c_str());
	sprintf(use_largeicons,"%s",  sbInfo["Shortcut Info"]["Large Icon"].c_str());	
	sbInfo.Save();
	
	sbconf = new SWConfig(conffile);	
	if ((sit = sbconf->Sections.find("ROOT")) != sbconf->Sections.end()) {
		if ((eit = (*sit).second.begin()) != (*sit).second.end()) {	
			for(eit = (*sit).second.begin(); eit != (*sit).second.end(); eit++){					
				buf = g_strdup((char*)(*eit).second.c_str());
				list = g_list_append(list,buf);
			}
		}
	}
	delete sbconf;
	return list;
}
	

void
saveshortcutbarSW(gchar *filename, gchar *group_name, GList *list, gchar *large_icons)
{	
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	SWConfig *sbconf;
	ConfigEntMap emap;
	gchar 
		buf[500],
		conffile[256];
	gint j=0;
	
	sprintf(conffile,"%s/%s", shortcutbarDir, filename);
	unlink(conffile);
	
	SWConfig sbInfo(conffile);
	sbInfo["Shortcut Info"]["Group Name"] = group_name;
	sbInfo["Shortcut Info"]["Large Icon"] = large_icons;	
	sbInfo.Save();
	
	sbconf = new SWConfig(conffile);
	emap = sbconf->Sections["ROOT"];
	while (list != NULL) { 
		sprintf(buf, "branch%d", j++);
		emap.erase(buf); emap.insert(ConfigEntMap::value_type(buf, (gchar *) list->data));	
		list = g_list_next(list);	
	}
	sbconf->Sections["ROOT"] = emap;
	sbconf->Save();
	delete sbconf;  	
}

GList *getModlistSW(gchar *modtype)
{
	GList *list;
	ModMap::iterator it;	
	SWMgr *mgr;
	gchar *buf;
	
	mgr = new SWMgr();	//-- create sword mgrs
	list = NULL;
	for(it = mgr->Modules.begin(); it != mgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), modtype)){
			buf = g_strdup((gchar*)(*it).second->Description());
			list = g_list_append(list,buf);	
		}else if (!strcmp((*it).second->Type(),modtype )){ 
			buf = g_strdup((gchar*)(*it).second->Description());
			list = g_list_append(list,buf);	
		}else if (!strcmp((*it).second->Type(),modtype )){ 
			buf = g_strdup((gchar*)(*it).second->Description());
			list = g_list_append(list,buf);	
		}
	}
	delete mgr;				
	return list;
}
