/*
 * GnomeSword Bible Study Tool
 * sword_main.cc - 
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

#include <swmgr.h>

#include <gs_markupfiltmgr.h>
#include <versekey.h>
#include <regex.h>
#include <gbfplain.h>
#include <thmlplain.h>
#include <string.h>

#include "backend/sword_main.hh"

#include "main/settings.h"
#include "main/sword.h"

using namespace sword;
using namespace std;

SwordMain *backend = NULL;

SwordMain::SwordMain() {
	main_mgr = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	display_mgr = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	search_mgr = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	results = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	percom_mgr = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	
	text_mod = NULL;
	comm_mod = NULL;
	dict_mod = NULL;
	gbs_mod = NULL;
	percom_mod = NULL;
	
	commDisplay      = 0;	// set in create
	dictDisplay     = 0;	// set in create
}


SwordMain::~SwordMain() {
	delete main_mgr;
	delete display_mgr;
	delete search_mgr;
	delete results;
	delete percom_mgr;
	
	if (commDisplay)
		delete commDisplay;
	if (dictDisplay)
		delete dictDisplay;
}

void SwordMain::init_SWORD() {
	ModMap::iterator it;
	
	for (it = display_mgr->Modules.begin();
				it != display_mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
			if (!strcmp((*it).second->Name(), settings.MainWindowModule))	
				text_mod = (*it).second;
		}
		if (!strcmp((*it).second->Type(), COMM_MODS)) {
			if (!strcmp((*it).second->Name(), settings.CommWindowModule))	
				comm_mod = (*it).second;
			(*it).second->Disp(commDisplay);
		}
		if (!strcmp((*it).second->Type(), DICT_MODS)) {
			if (!strcmp((*it).second->Name(), settings.DictWindowModule))	
				dict_mod = (*it).second;
			(*it).second->Disp(dictDisplay);
		}
		if (!strcmp((*it).second->Type(), BOOK_MODS)) {
			if (!strcmp((*it).second->Name(), settings.book_mod))	
				gbs_mod = (*it).second;
			(*it).second->Disp(commDisplay);
		}			
	}
}


void SwordMain::init_lists(MOD_LISTS * mods) {
	ModMap::iterator it;
	
	for (it = main_mgr->Modules.begin();
				it != main_mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
			mods->biblemods =
			    g_list_append(mods->biblemods,
				strdup((char *) (*it).second->Name()));
			mods->text_descriptions =
			    g_list_append(mods->text_descriptions,
				strdup((char *) (*it).second->
				   Description()));
		}
		if (!strcmp((*it).second->Type(), COMM_MODS)) {
			mods->commentarymods =
			    g_list_append(mods->commentarymods,
			    strdup((char *) (*it).second->Name()));
			mods->comm_descriptions =
			    g_list_append(mods->comm_descriptions,
				strdup((char *) (*it).second->
				   Description()));
			if (!strcmp((*it).second->getConfigEntry("ModDrv")
				    , "RawFiles")) {
				mods->percommods = g_list_append(mods->percommods,
				    strdup((char *) (*it).second->Name()));
			} 
		}
		if (!strcmp((*it).second->Type(), DICT_MODS)) {
			mods->dictionarymods =
			    g_list_append(mods->dictionarymods,
			    strdup((char *) (*it).second->Name()));
			mods->dict_descriptions =
			    g_list_append(mods->dict_descriptions,
			    strdup((char *) (*it).second->
				   Description()));
			
			char *feature =
			    (char *) (*it).second->getConfigEntry("Feature");
			if(!feature)
				continue;
			if (!strcmp(feature, "DailyDevotion"))
				mods->devotionmods 
				   = g_list_append(mods->devotionmods,
				    strdup((char *) (*it).second->Name()));
		}
		if (!strcmp((*it).second->Type(), BOOK_MODS)) {
			mods->bookmods =
			    g_list_append(mods->bookmods,
			    strdup((char *) (*it).second->Name()));
			mods->book_descriptions =
			    g_list_append(mods->book_descriptions,
			    strdup((char *) (*it).second->
				   Description()));
		}			
	}
}

GList *SwordMain::fill_Bible_books(int testament) {
	VerseKey key;
	GList *retlist = NULL;
	char *book = NULL;
	unsigned int bytes_read;
	unsigned int bytes_written;
	GError *error;
	int i = 0, j = 0, x = 2;
	
	switch(testament) {
		case 0: i = 0; x = 1;
			break;
		case 1:i = 1; x = 2;
			break;
		case 2: i = 0; x = 2;
			break;
	}
	
	while(i < x) {
		while(j < key.BMAX[i]) { 
			book = g_convert((const char *)key.books[i][j++].name,
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     &error);
			if(book == NULL) {
				g_print ("error: %s\n", error->message);
				g_error_free (error);
				continue;
			}
			retlist = g_list_append(retlist, (char *)book);
		}
		j = 0;
		i++;
	}
	return retlist;
}


void SwordMain::get_module_options(GList * options) {
	StringList optionslist = main_mgr->getGlobalOptions();	
	for (StringList::iterator it = optionslist.begin(); 
				  it != optionslist.end(); it++) {
		options = g_list_append(options, strdup((char *) (*it).c_str()));
	}
}

/* end of file */
