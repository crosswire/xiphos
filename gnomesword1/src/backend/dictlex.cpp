/*
 * GnomeSword Bible Study Tool
 * dictlex.cpp - support for Sword commentary modules
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
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "gs_gnomesword.h"
#include "sword.h"
#include "display.h"
#include "dictlex.h"

typedef struct _backend_dl BE_DL;
struct _backend_dl {
	SWModule *mod;
	SWDisplay *dlDisp;
	int num;
};

/***  globals  ***/
SWMgr *swmgrDL;
GList *be_dl_list;

/***  start code  ***/

static SWModule *backend_getModule(char *modname)
{
	GList *tmp = NULL;
	BE_DL *d;
	SWModule *mod = NULL;

	tmp = be_dl_list;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		d = (BE_DL *) tmp->data;
		if (!strcmp(d->mod->Name(), modname)) {
			mod = d->mod;
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return mod;
}

void backend_newDisplayDL(GtkWidget * html, char *bookname,
			  SETTINGS * s)
{
	GList *tmp = NULL;
	BE_DL *d;

	tmp = g_list_first(be_dl_list);
	while (tmp != NULL) {
		d = (BE_DL *) tmp->data;
		if (!strcmp(d->mod->Name(), bookname)) {
			d->dlDisp = new GtkHTMLEntryDisp(html, s);
			d->mod->Disp(d->dlDisp);
		}
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

void backend_setupDL(SETTINGS * s)
{
	ModMap::iterator it;	//-- iteratior
	gint count = 0;

	swmgrDL = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
	be_dl_list = NULL;
	for (it = swmgrDL->Modules.begin();
	     it != swmgrDL->Modules.end(); it++) {
		if (!strcmp
		    ((*it).second->Type(), "Lexicons / Dictionaries")) {	    
			if(!strcmp((*it).second->Name(),"Thayer"))
				s->havethayer = true;
			if(!strcmp((*it).second->Name(),"BDB"))
				s->havebdb = true;
			BE_DL *be_dl = new BE_DL;
			be_dl->mod = (*it).second;
			be_dl->num = count;
			be_dl_list =
			    g_list_append(be_dl_list, (BE_DL *) be_dl);
			++count;
		}
	}
}

void backend_shutdownDL(void)
{
	delete swmgrDL;

	/***  free backend stuff  ***/
	be_dl_list = g_list_first(be_dl_list);
	while (be_dl_list != NULL) {
		BE_DL *d = (BE_DL *) be_dl_list->data;
		if (d->dlDisp)
			delete d->dlDisp;	//-- delete any swdisplays created
		delete(BE_DL *) be_dl_list->data;
		be_dl_list = g_list_next(be_dl_list);
	}
	g_list_free(be_dl_list);
}

void backend_displayinDL(gchar * modname, gchar * key)
{
	SWModule *mod = backend_getModule(modname);
	mod->SetKey(key);
	mod->Display();
}

/******************************************************************************
* return list of dictionary keys 
* count = the number of keys to retrun
*******************************************************************************/
GList *backend_fillDictKeysDL(char *modname, gint count)
{
	ModMap::iterator it;	//-- iterator to go through modules and find modName
	int			//-- for index into list widget
	 i,			//-- counters
	 j;
	SWKey saveKey;		//-- for starting point
	gchar *listitem;	//-- list widget to display items found
	GList *retList = NULL;

	j = 0;
	it = swmgrDL->Modules.find(modname);	//-- find module to use for search
	if (it != swmgrDL->Modules.end()) {	//-- if we dont reach the end of our modules 
		SWModule *mod = (*it).second;	//-- temp module to work with
		mod->KeyText();	//-- snap to entry
		saveKey = mod->KeyText();	//-- save our place

		if (mod) {	//-- make sure we have module to work with             

			for (i = 0; i < (count / 2); i++)
				(*mod)++;	//-- get equal number of keys before and after our starting key(saveKey)

			for (i = 0; i < count - 1; i++)
				(*mod)--;

			mod->Error();	//-- clear errors

			for (; !mod->Error() && count;
			     count--, (*mod)++) {
				++j;
				listitem = g_strdup((const char *) mod->KeyText());	//-- key to listitem
				retList = g_list_append(retList, (gchar *) listitem);	//-- listitem to list
				//g_free(listitem);
			}
		}
		mod->SetKey(saveKey);
	}
	return retList;
}

void backend_dictentryTextChangedDL(gchar * modname, char *mytext)
{
	SWModule *mod = backend_getModule(modname);
	if (mod) {		//-- if we have a dict module  
		if (strcmp(mytext, "")) {	//-- if text is not null                
			mod->SetKey(mytext);	//-- set key to our text
			mod->Display();	//-- show what we found       
		}
	}
}
