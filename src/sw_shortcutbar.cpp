/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_shortcutbar.cpp
    * -------------------
    * Sat June 30 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
    *
 */
 
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
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include  <gal/shortcut-bar/e-shortcut-bar.h>

#include "gs_gnomesword.h"
#include "sw_shortcutbar.h"
#include "sw_utility.h"
#include "sw_display.h"
#include "gs_shortcutbar.h"
#include "support.h"

extern gint groupnum7;
extern gchar *shortcutbarDir;
extern SETTINGS *settings;
list <string> sbfiles;	

static SWDisplay 
	*viewersbDisplay;	/* to display modules in viewer dialog */
static SWMgr 
	*viewersbMgr; 
static SWModule 
	*viewersbMod;   /* module for viewer dialog */

gint
sbtypefromModNameSBSW(gchar *modName)
{
	ModMap::iterator it; //-- iteratior
	SWMgr *mgr;
	gint retval = -1;
		
	mgr = new SWMgr();	//-- create sword mgrs
	/*** get module lists ***/
	for(it = mgr->Modules.begin(); it != mgr->Modules.end(); it++){
		if(!strcmp((*it).second->Name(), modName)){
			if(!strcmp((*it).second->Type(), "Biblical Texts")){
				retval = 0;
			}if(!strcmp((*it).second->Type(), "Commentaries")){
				retval = 1;
			}if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
				retval = 2;
			} 
			break;
		}
	}
	/*** delete Sword manager ***/
	delete mgr;
	return retval;
}

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
saveshortcutbarSW(gchar *filename, gchar *group_name, gint group_num, gchar *large_icons)
{	
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	SWConfig *sbconf;
	ConfigEntMap emap;
	gchar 
		buf[500],
		conffile[256],
		*item_url,
		*item_name;
	gint 
		j=0,
		number_of_items;
	
	sprintf(conffile,"%s/%s", shortcutbarDir, filename);
	unlink(conffile);
	
	SWConfig sbInfo(conffile);
	sbInfo["Shortcut Info"]["Group Name"] = group_name;
	sbInfo["Shortcut Info"]["Large Icon"] = large_icons;	
	sbInfo.Save();
	
	sbconf = new SWConfig(conffile);
	emap = sbconf->Sections["ROOT"];
	number_of_items = e_shortcut_model_get_num_items(E_SHORTCUT_BAR(settings->shortcut_bar)->model, group_num);
	
	for (j = 0; j < number_of_items; j++) {
		e_shortcut_model_get_item_info(E_SHORTCUT_BAR
					       (settings->shortcut_bar)->model,
					       group_num,
					       j, &item_url, &item_name,
					       NULL);
		sprintf(buf, "branch%d", j);
		emap.erase(buf); emap.insert(ConfigEntMap::value_type(buf, (gchar *) item_name));	
		g_warning("saving list item: %s",(gchar*)item_name);
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
		}
	}
	delete mgr;				
	return list;
}

/*
 * 
 * 
 */
gboolean
displaydictlexSBSW(gchar *modName, gchar *key, SETTINGS *s)
{
	gchar 
		buf[256], 
		*utf8str,
		tmpbuf[256];	
	
	gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(s->app, "nbVL")), 2);	
	sprintf(s->groupName,"%s","Viewer");
	changegroupnameSB(s, s->groupName, groupnum7);
	if(!strcmp(modName,viewersbMod->Name())){
		viewersbMod->SetKey(key); //-- set key to the first one in the list
		viewersbMod->Display(); 
	}else{
		ModMap::iterator it; 	
		it = viewersbMgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
		if (it != viewersbMgr->Modules.end()){ //-- if we find the module	
			viewersbMod = (*it).second;  //-- change module to new module
			viewersbMod->SetKey(key); //-- set key to the first one in the list
			viewersbMod->Display(); 
		}
	}
	return TRUE;	
}

/****************************************************************************************
 *setupVLSWORD - set up the sword stuff for the viewer dialog
 ****************************************************************************************/
void setupviewerSBSW(GtkWidget *html_widget)
{	
	ModMap::iterator it; //-- iteratior	
	SectionMap::iterator sit; //-- iteratior
	
	viewersbMgr	= new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
	viewersbMod     = NULL;
	viewersbDisplay = new  GtkHTMLEntryDisp(html_widget);
	
	for(it = viewersbMgr->Modules.begin(); it != viewersbMgr->Modules.end(); it++){
		viewersbMod = (*it).second;
		sit = viewersbMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters			
		ConfigEntMap &section = (*sit).second;
		addrenderfiltersSWORD(viewersbMod, section);
		if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			viewersbMod->Disp(viewersbDisplay);
		}
	}
}

/*** close down viewer dialog ***/
void shutdownviewerSBSW(void) 
{	
	delete viewersbMgr;	
	if(viewersbDisplay)
		delete viewersbDisplay;	
}


