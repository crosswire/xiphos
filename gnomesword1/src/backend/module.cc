/*
 * GnomeSword Bible Study Tool
 * module.cc - SHORT DESCRIPTION
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

#include <swmodule.h>

#include "backend/module.hh"
#include "backend/key.hh"
#include "backend/sword.h"
#include "backend/sword_defs.h"

#include "main/settings.h"
#include "main/search.h"
#include "main/sword.h"

using std::string;
using std::map;
using std::list;
using namespace sword;

#include "backend/mgr.hh"
static SWModule * search_module;
static ModMap::iterator begin;
static ModMap::iterator end;

extern ListKey results;
extern ListKey search_scope_list;
extern SWKey *current_scope;
extern SWMgr *module_mgr;
//extern modDescMap descriptionMap;

int  backend_do_module_search(char * module_name ,char *search_string, 
		     int search_type, int search_params)
{
	char progressunits = 70;
	
	strcpy(settings.searchText, search_string);
	results.ClearList();
	search_module = NULL;
	
	backend_new_module_mgr();
	search_module = module_mgr->Modules[module_name];
	if (!search_module)
		return -1;
	results = search_module->Search(
				search_string,
				search_type,
				search_params,
				current_scope, 0,
				&search_percent_update,
				(void*)&progressunits);
	search_scope_list = results;
	backend_delete_module_mgr();
	return results.Count();
}


/******************************************************************************
 * Name
 *   backend_get_search_results_text
 *
 * Synopsis
 *   #include "backend/.hh"
 *
 *   char *backend_get_search_results_text(char * mod_name, char * key)	
 *
 * Description
 *    returns the module text for key
 *
 * Return value
 *   char *
 */
 
char *backend_get_search_results_text(char * mod_name, char * key)
{
	SWModule *mod = sw_mgr.results->Modules[mod_name];
	
	if(mod) {
		mod->SetKey(key);
		return strdup((char*)mod->RenderText());
	}
	return NULL;
}


void backend_set_module_iterators(void)
{
	begin = module_mgr->Modules.begin();
	end = module_mgr->Modules.end();
}

/******************************************************************************
 * Name
 *   backend_get_next_module_name
 *
 * Synopsis
 *   #include "backend/.hh"
 *
 *   NAME_TYPE *backend_get_next_module_name(void)	
 *
 * Description
 *   
 *
 * Return value
 *   NAME_TYPE
 */
 
NAME_TYPE *backend_get_next_module_name(void)
{
	NAME_TYPE nt, *retval = NULL;
		
	retval = &nt;
	
	if(begin != end) {
		if(!strcmp((*begin).second->Type(),TEXT_MODS)) {
			nt.type = TEXT_TYPE;			
			nt.name = strdup((char*)(*begin).second->Name());
		}
		if(!strcmp((*begin).second->Type(),COMM_MODS)) {
			nt.type = COMMENTARY_TYPE;			
			nt.name = strdup((char*)(*begin).second->Name());
		}
		if(!strcmp((*begin).second->Type(),DICT_MODS)) {
			nt.type = DICTIONARY_TYPE;			
			nt.name = strdup((char*)(*begin).second->Name());
		}
		if(!strcmp((*begin).second->Type(),BOOK_MODS)) {
			nt.type = BOOK_TYPE;			
			nt.name = strdup((char*)(*begin).second->Name());
		}
		begin++;
		return retval;
	}
	else 
		return NULL;
}


/******************************************************************************
 * Name
 *   backend_get_next_module_description
 *
 * Synopsis
 *   #include "backend/.hh"
 *
 *   	NAME_TYPE *backend_get_next_module_description(void)
 *
 * Description
 *   
 *
 * Return value
 *   NAME_TYPE
 */
 
NAME_TYPE *backend_get_next_module_description(void)
{
	NAME_TYPE nt, *retval = NULL;
		
	retval = &nt;
	
	if(begin != end) {
		if(!strcmp((*begin).second->Type(),TEXT_MODS)) {
			nt.type = TEXT_TYPE;			
			nt.name = strdup((char *) (*begin).second->Description());
		}
		if(!strcmp((*begin).second->Type(),COMM_MODS)) {
			nt.type = COMMENTARY_TYPE;			
			nt.name = strdup((char *) (*begin).second->Description());
		}
		if(!strcmp((*begin).second->Type(),DICT_MODS)) {
			nt.type = DICTIONARY_TYPE;			
			nt.name = strdup((char *) (*begin).second->Description());
		}
		if(!strcmp((*begin).second->Type(),BOOK_MODS)) {
			nt.type = BOOK_TYPE;			
			nt.name = strdup((char *) (*begin).second->Description());
		}
		begin++;
		return retval;
	}
	else 
		return NULL;
}



/**********************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/.hh"
 *   
 *   
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *backend_get_next_percom_name(void)
{
	char *retval = NULL;
	
	if(begin != end) {		
		if(!strcmp((*begin).second->getConfigEntry("ModDrv")
			,"RawFiles")) {
			retval = strdup((char *)(*begin).second->Name());
			begin++;
			return retval;
		}
		else {
			begin++;
			return "+";
		}
	}
	else
		return NULL;
}


/**********************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/.hh"
 *   
 *   
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *backend_get_next_devotion_name(void)
{
	char *retval = NULL;
	char *feature = NULL;
	
	if(begin != end) {
		feature = (char*)(*begin).second->getConfigEntry("Feature");
		if(!feature) {	
			begin++;
			return "+";
		}	
		if(!strcmp(feature,"DailyDevotion")) {
			retval = strdup((char *)(*begin).second->Name());
			begin++;
			return retval;
		}
		else {
			begin++;
			return "+";
		}
	}
	else
		return NULL;
}


