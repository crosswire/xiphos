
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
#  include <config.h>
#endif

#include <gnome.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swmodule.h>
#include <versekey.h>
#include <plainhtml.h>
#include <gbfhtml.h>
#include <rwphtml.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>

#include "search.h"
#include "display.h"
#include "gs_gnomesword.h"
#include "shortcutbar.h"
#include "gs_html.h"
#include "gs_editor.h"
#include "sword.h"
#include "support.h"
#include "verselist_sb.h"
#include "gbs.h"


/***********************************************************************************************
 externals
***********************************************************************************************/
extern SWModule *curMod, *curcomMod, *percomMod;
extern gboolean firstsearch;
extern SETTINGS *settings;
extern GtkWidget *clistSearchResults;
/***********************************************************************************************
 static
***********************************************************************************************/
static void percentUpdate(char percent, void *userData) ;
static char printed = 0;
static SWDisplay 
	*searchresultssbDisplay;	/* to display modules in searchresults */
	//*searchresultstextsbDisplay;
static SWMgr 
	*searchresultssbMgr; 
static SWModule 
	*searchresultssbMod;   /* module for searchresults */



/******************************************************************************
 * Name
 *   backend_do_search
 *
 * Synopsis
 *   #include "backend_do_search.h"
 *
 *   void backend_do_search(SETTINGS *s, SEARCH_OPT *search_opts)	
 *
 * Description
 *   search Sword module for word(s), phrase, or reg_ex.
 *
 * Return value
 *   GList *
 */
GList*   /* search Bible text, commentaries or generic books*/
backend_do_search(SETTINGS *s, SEARCH_OPT *search_opts)	
{	
	
	VerseKey searchScopeLowUp; //----------- sets lower and upper search bounds
        ListKey	searchScopeList; //----------- search list for searching verses found on last search
        SWKey	*currentScope; //----------- use to set scope of search	
	SWModule *searchMod,
			*tmpMod;
	SWMgr *searchMgr;
	ModMap::iterator 
		it;
			
	const gchar	
		*resultText;   //-- temp storage for verse found
	gchar     
		buf[256], 
		buf2[256], 
		*mybuf[2],
		scount[5],	//-- string from gint count for label
		firstkey[80];
	gint            
		count;		//-- number of hits
	GList 
		*list;
	GString 
		*tmpbuf;
		
	tmpbuf = g_string_new("");	
	list = NULL;	
	searchMgr = new SWMgr();	//-- create sword mgr
	
	searchMod = NULL;

	g_warning("module name = %s",search_opts->module_name);
	
	if(search_opts->module_name) {
		  it = searchMgr->Modules.find (search_opts->module_name);
		  if (it != searchMgr->Modules.end ()) {
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
	count = 0;		/* we have not found anything yet */
	printed = 0;
	
	sprintf(s->searchText, "%s",search_opts->search_string);
	
	if (searchMod) {		/* must have a good module - not null */	 
                int searchType = search_opts->search_type;
		int searchParams = search_opts->search_params;
			  
		//-- give search string to sword for search
		for (ListKey & searchResults = searchMod->Search (search_opts->search_string,
				searchType,
				searchParams,
				currentScope, 0,
				&percentUpdate,
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
	
	if(count){
		ModMap::iterator it; 	
		it = searchresultssbMgr->Modules.find(searchMod->Name()); //-- iterate through the modules until we find modName - modName was passed by the callback
		if (it != searchresultssbMgr->Modules.end()){ //-- if we find the module	
			searchresultssbMod = (*it).second;  //-- change module to new module
		}
	}
			
	delete searchMgr;
	g_string_free(tmpbuf,TRUE);
	return list;
}

//-------------------------------------------------------------------------------------------
static void percentUpdate(char percent, void *userData) 
{	
	char maxHashes = *((char *)userData);
	float num;
	char buf[80];
	
	while (gtk_events_pending ())
		gtk_main_iteration ();
	while ((((float)percent)/100) * maxHashes > printed) {
		sprintf(buf,"%f",(((float)percent)/100));
		num = (float)percent/100;
		gnome_appbar_set_progress ((GnomeAppBar *)settings->appbar,
					  num );
		printed++;	
	} 
	while (gtk_events_pending ())
		gtk_main_iteration (); 
}

/****************************************************************************************
 *setupVLSWORD - set up the sword stuff for the searchresults dialog
 ****************************************************************************************/
void setupsearchresultsSBSW(GtkWidget *html_widget)
{	
	ModMap::iterator it; //-- iteratior	
	SectionMap::iterator sit; //-- iteratior
	
	searchresultssbMgr	= new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	
	searchresultssbMod     = NULL;
	searchresultssbDisplay = new  GtkHTMLEntryDisp(html_widget,settings);
	//searchresultstextsbDisplay = new  GTKutf8ChapDisp(html_widget);
	
	for(it = searchresultssbMgr->Modules.begin(); it != searchresultssbMgr->Modules.end(); it++){
		searchresultssbMod = (*it).second;
		/*if(!strcmp((*it).second->Type(), "Biblical Texts")){
			searchresultssbMod->Disp(searchresultstextsbDisplay);			
		}else{*/
			searchresultssbMod->Disp(searchresultssbDisplay);
		//}
	}
}

/*** close down searchresults ***/
void shutdownsearchresultsSBSW(void) 
{	
	delete searchresultssbMgr;	
	if(searchresultssbDisplay)
		delete searchresultssbDisplay;	
	/*
	if(searchresultstextsbDisplay)
		delete searchresultstextsbDisplay;
	*/
}

void
backend_search_results_item_display(gchar *url)
{	
	searchresultssbMod->SetKey(url);
	searchresultssbMod->Display();
}

