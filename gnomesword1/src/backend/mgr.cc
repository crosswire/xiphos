/*
 * GnomeSword Bible Study Tool
 * mgr.cc - SHORT DESCRIPTION
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

#include "backend/sword.h"
#include "backend/sword_defs.h"

#include "main/search.h"

//using std::string;
//using std::map;
//using std::list;
using namespace sword;

#include "backend/mgr.hh"


/******************************************************************************
 * externs
 */	


/******************************************************************************
 * globals
 */
SWORD sw;

/******************************************************************************
 * static  global to this file only 
 */

static StringList optionslist;
static StringList::iterator it;



/******************************************************************************
 * Name
 *   backend_init_managers
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_init_managers(void)	
 *
 * Description
 *   start sword main_mgr for settings (new bookmarks or preferences)
 *
 * Return value
 *   void
 */
 
void backend_init_managers(void)
{
	sw.main_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw.text_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw.inter_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw.comm_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw.dict_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw.gbs_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw.search_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw.results = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	sw.percom_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
}

/******************************************************************************
 * Name
 *   backend_delete_managers
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_delete_managers(void)	
 *
 * Description
 *   close down sword main_mgr
 *
 * Return value
 *   void
 */

void backend_delete_managers(void)
{
	delete sw.text_mgr;
	delete sw.inter_mgr;
	delete sw.comm_mgr;
	delete sw.dict_mgr;
	delete sw.gbs_mgr;
	delete sw.main_mgr;
	delete sw.search_mgr;
	delete sw.results; 
	delete sw.percom_mgr; 
}
/******************************************************************************
 * Name
 *   backend_init_main_mgr
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_init_main_mgr(void)	
 *
 * Description
 *   start sword main_mgr for settings (new bookmarks or preferences)
 *
 * Return value
 *   void
 */
 
void backend_init_main_mgr(void)
{
	sw.main_mgr = new SWMgr();
}


/******************************************************************************
 * Name
 *   backend_delete_main_mgr
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_delete_main_mgr(void)	
 *
 * Description
 *   close down sword main_mgr
 *
 * Return value
 *   void
 */

void backend_delete_main_mgr(void)
{
	delete sw.main_mgr;
}


/******************************************************************************
 * Name
 *  backend_new_module_mgr 
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_new_module_mgr(void)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
 
void backend_new_module_mgr(void)
{
	sw.module_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
}


/******************************************************************************
 * Name
 *   backend_delete_module_mgr
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_delete_module_mgr(void)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
 
void backend_delete_module_mgr(void)
{
	delete sw.module_mgr;	
}


/******************************************************************************
 * Name
 *  backend_set_global_option_iterator 
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_set_global_option_iterator(void)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */
 
void backend_set_global_option_iterator(void)
{
	optionslist = sw.module_mgr->getGlobalOptions();
	it = optionslist.begin();
}


/******************************************************************************
 * Name
 *   backend_get_next_global_option
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   char *backend_get_next_global_option(void)	
 *
 * Description
 *   
 *
 * Return value
 *   int
 */
 
char *backend_get_next_global_option(void)
{
	char *retval = NULL;
	
	if(it != optionslist.end()) {
		retval = strdup((char *) (*it).c_str());
		it++;
		return retval;	
	}
	else
		return NULL;
}


/******************************************************************************
 * Name
 *  backend_get_path_to_mods
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *   
 *   	char *backend_get_path_to_mods(void)
 *
 * Description
 *   returns path to sword modules must be freed by calling function
 *
 * Return value
 *   char*
 */

char *backend_get_path_to_mods(void)
{
	return strdup(sw.main_mgr->prefixPath);
}


/******************************************************************************
 * Name
 *  backend_set_global_option
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *   
 *   void backend_set_global_option(int manager, char * option, 
 *							char * yesno)	
 *
 * Description
 *   sets sword module global options ie strongs, 
 *   morph tags or footnotes
 *
 * Return value
 *   void
 */

void backend_set_global_option(int manager, char * option, char * yesno)
{
	/* 
	 * turn option on or off INTER_MGR
	 */
	switch(manager) {
		case TEXT_MGR:
			sw.text_mgr->setGlobalOption(option, yesno);
		break;
		case COMM_MGR:
			sw.comm_mgr->setGlobalOption(option, yesno);
		break;
		case MAIN_MGR:			
			sw.main_mgr->setGlobalOption(option, yesno);
		break;
		case SEARCH_MGR:
			sw.search_mgr->setGlobalOption(option, yesno);
		break;	
		case INTER_MGR:
			sw.inter_mgr->setGlobalOption(option, yesno);
		break;
	}	
	sw.results->setGlobalOption(option, yesno);
}


/******************************************************************************
 * Name
 *  backend_set_module_unlocked
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *   
 *   void backend_set_module_unlocked(char *mod_name, char *key)	
 *
 * Description
 *   unlocks locked module - FIXME: does not work :(
 *
 * Return value
 *   void
 */

void backend_set_module_unlocked(char * mod_name, char * key)
{	/* this does not work */
	int rslts = sw.text_mgr->setCipherKey(mod_name, key);
}
