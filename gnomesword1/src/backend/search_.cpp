/*
 * GnomeSword Bible Study Tool
 * search.cpp - search Sword modules
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

#include <glib-1.2/glib.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swmodule.h>
#include <versekey.h>
#include <regex.h>

#include "backend/search_.h"
#include "backend/sword_defs.h"

#include "main/settings.h"
#include "main/gs_gnomesword.h"



/******************************************************************************
 * Name
 *   backend_do_search
 *
 * Synopsis
 *   #include "search.h"
 *
 *   GList* backend_do_search(SEARCH_OPT *search_opts)	
 *
 * Description
 *   search Sword module for word(s), phrase, or reg_ex.
 *
 * Return value
 *   GList *
 */
 
GList* backend_do_search(gpointer *usr_data)	
{	
	SEARCH_OPT *search_opts;
	VerseKey searchScopeLowUp; //----------- sets lower and upper search bounds
        ListKey	searchScopeList; //----------- search list for searching verses found on last search
        SWKey	*currentScope; //----------- use to set scope of search	
	SWModule *searchMod,
			*tmpMod;
	ModMap::iterator 
		it;
			
	const char	
		*resultText;   //-- temp storage for verse found
	char     
		buf[256], 
		buf2[256], 
		*mybuf[2],
		scount[5],	//-- string from gint count for label
		firstkey[80];
	int            
		count;		//-- number of hits
	GList 
		*list;
	GString 
		*tmpbuf;
		
	search_opts = (SEARCH_OPT *)usr_data;	
	tmpbuf = g_string_new("");	
	list = NULL;	
	
	searchMod = NULL;
	
	if(search_opts->module_name) {
		  it = sw_mgr.search->Modules.find (search_opts->module_name);
		  if (it != sw_mgr.search->Modules.end ()) {
			    searchMod = (*it).second;
			    tmpMod = (*it).second;
		  } else return list;
	} else return list;
	
	
	
	if (search_opts->use_bonds) {
		  searchScopeLowUp.ClearBounds ();	/* clear old bounds */
		  searchScopeLowUp.LowerBound (search_opts->lower_bond);	/* read lower bounds entry and set lower bounds for search */
		  searchScopeLowUp.UpperBound (search_opts->upper_bond);	/* read upper bounds entry and set upper bounds for search */
		  currentScope = &searchScopeLowUp;	/* set scope of search to use bounds */
	} 
	
	else if (search_opts->use_lastsearch_for_bonds) {	/* check to see if we want to use results of search last for this search */	  
		  currentScope = &searchScopeList;	//-- if we do _ move searchlist into currentScope
	} 
	
	else {
		  searchScopeLowUp.ClearBounds ();	//-------- clear old bounds
		  searchScopeList.ClearList ();	//------------ clear scope search list
		  currentScope = 0;	//------------ clear scope
	}
	
	char progressunits = 70;
	count = 0;		
	
	sprintf(settings.searchText, "%s",search_opts->search_string);
	
	if (searchMod) {		/* must have a good module - not null */	 
                int searchType = search_opts->search_type;
		int searchParams = search_opts->search_params;
			  
		//-- give search string to sword for search
		for (ListKey & searchResults = searchMod->Search (search_opts->search_string,
				searchType,
				searchParams,
				currentScope, 0,
				&search_percent_update,
				(void*)&progressunits);
				!searchResults.Error ();
				searchResults++) {		
			resultText = (const char *)searchResults;	//-- put verse key string of find into a string
			g_string_sprintf(tmpbuf,"%s, %s|%s|%s",
				resultText,
				searchMod->Name(),
				resultText,
				searchMod->Name());
			list = g_list_append(list,g_strdup(tmpbuf->str));
			/* remember finds for next search's scope in case we want to use them */						
			searchScopeList << (const char *) searchResults;	
			if(!count) 
				sprintf(firstkey,"%s",(const char *)searchResults);
			++count;	
                }
        } 
	search_opts->found_count = count;
		
	g_string_free(tmpbuf,TRUE);
	return list;
}

/******************************************************************************
 * Name
 *   backend_get_search_results_text
 *
 * Synopsis
 *   #include "search_.h"
 *
 *   char *backend_get_search_results_text(char * mod_name, char * key)	
 *
 * Description
 *    
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
