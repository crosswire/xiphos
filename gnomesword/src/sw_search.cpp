/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * gs_search.cpp
    * -------------------
    * Wed Aug 02 2001
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

#include "sw_search.h"
#include "sw_display.h"
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_html.h"
#include "sw_gnomesword.h"
#include "support.h"
#include "sw_utility.h"
#include "sw_verselist_sb.h"


/***********************************************************************************************
 externals
***********************************************************************************************/
extern SWModule *curMod, *curcomMod, *percomMod;
extern gboolean firstsearch;
extern SETTINGS *settings;

/***********************************************************************************************
 static
***********************************************************************************************/
static void percentUpdate(char percent, void *userData) ;
static char printed = 0;
static SWDisplay 
	*searchresultssbDisplay,	/* to display modules in searchresults */
	*searchresultstextsbDisplay;
static SWMgr 
	*searchresultssbMgr; 
static SWModule 
	*searchresultssbMod;   /* module for searchresults */

//-------------------------------------------------------------------------------------------
GList*   /* search Bible text or commentaries */
searchSWORD (GtkWidget *widget, SETTINGS *s)	
{	
	
	VerseKey searchScopeLowUp; //----------- sets lower and upper search bounds
        ListKey	searchScopeList; //----------- search list for searching verses found on last search
        SWKey	*currentScope; //----------- use to set scope of search
	SWModule *searchMod;
	SWMgr *searchMgr;
	ModMap::iterator 
		it;
	GtkWidget       
		*searchText,	//-- what we want to find -text entry
	        *lbSearchHits,	//-- label for showing how many verses we found
	        *resultList,	//-- list of verses found
	        *regexSearch,	//-- do we want to use regular expression search - radio button
	        *phraseSearch,	//-- do we want to use phrase seach - radio button
	        *caseSensitive,	//-- do we want search to be case sensitive - check box
	       	*comToggle,	//-- do we want to search current commentary - check box
	       	*bounds,	//-- do we want to use bounds for our search - check box
	       	*lowerbound,	//-------- lower bounds entry widget
	        *upperbound,	//-------- upper bounds entry widget
	        *lastsearch,	//-- use verses from last search for bounds of this search
	        *textWindow,	//-- text widget to display verses
	       *percomToggle;	//-- do we want to search personal commentary - check box	
	const gchar	
		*resultText;   //-- temp storage for verse found
	gchar     
		buf[256],
		*entryText,	//-- pointer to text in searchText entry
	        scount[5],	//-- string from gint count for label
		*utf8str,
		firstkey[80];
	gint            
		count;		//-- number of hits
	GList 
		*list;
	GString 
		*tmpbuf;
		
	tmpbuf = g_string_new("");	
	list = NULL;	
	searchMgr = new SWMgr();	//-- create sword mgrs
	searchMod = NULL;
	searchText = lookup_widget (widget, "entrySearch");	//-- pointer to text entry
	regexSearch = lookup_widget (widget, "rbRegExp");	//-- pointer to radio button
	phraseSearch = lookup_widget (widget, "rbPhraseSearch");	//-- pointer to radio button
	caseSensitive = lookup_widget (widget, "ckbCaseSensitive");	//-- pointer to check box
	bounds = lookup_widget (widget, "rrbUseBounds");	//-- pointer to check box
	lastsearch = lookup_widget (widget, "rbLastSearch");	//-- pointer to radio button
	comToggle = lookup_widget (widget, "ckbCommentary");	//-- pointer to check box
	percomToggle = lookup_widget (widget, "ckbPerCom");	//-- pointer to check box   

	if (GTK_TOGGLE_BUTTON (comToggle)->active) {	/* if true search commentary */	  
		  it = searchMgr->Modules.find (curcomMod->Name ());	/* find commentary module */
		  if (it != searchMgr->Modules.end ()) {
			    searchMod = (*it).second;	/* set search module to current commentary module */
		  }
	} else if (GTK_TOGGLE_BUTTON (percomToggle)->active) {	/* if true search personal commentary */	  
		  it = searchMgr->Modules.find (percomMod->Name ());	/* find personal commentary module */
		  if (it != searchMgr->Modules.end ()) {
			    searchMod = (*it).second;	/* set search module to current personalcommentary module */
		  }
	} else {			/* if neither commertary nor personal check box checked */	 
		  it = searchMgr->Modules.find (curMod->Name ());	/* find personal commentary module */
		  if (it != searchMgr->Modules.end ()) {
			    searchMod = (*it).second;	/* set search module to current personalcommentary module */
		  }
	}	
	ModMap::iterator it2; 	
	it2 = searchresultssbMgr->Modules.find(searchMod->Name()); //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it2 != searchresultssbMgr->Modules.end()){ //-- if we find the module	
			searchresultssbMod = (*it2).second;  //-- change module to new module
	}
	if (GTK_TOGGLE_BUTTON (bounds)->active) {
		  lowerbound = lookup_widget (widget, "entryLower");	/* get Lower bounds entry widget from search form */
		  upperbound = lookup_widget (widget, "entryUpper");	/* get Upper bounds entry widget from search form */
		  searchScopeLowUp.ClearBounds ();	/* clear old bounds */
		  searchScopeLowUp.LowerBound (gtk_entry_get_text (GTK_ENTRY (lowerbound)));	/* read lower bounds entry and set lower bounds for search */
		  searchScopeLowUp.UpperBound (gtk_entry_get_text (GTK_ENTRY (upperbound)));	/* read upper bounds entry and set upper bounds for search */
		  currentScope = &searchScopeLowUp;	/* set scope of search to use bounds */
	} else if (GTK_TOGGLE_BUTTON (lastsearch)->active) {	/* check to see if we want to use results of search last for this search */	  
		  currentScope = &searchScopeList;	//-- if we do _ move searchlist into currentScope
	} else {
		  searchScopeLowUp.ClearBounds ();	//-------- clear old bounds
		  searchScopeList.ClearList ();	//------------ clear scope search list
		  currentScope = 0;	//------------ clear scope
	  }
	char progressunits = 70;
	count = 0;		/* we have not found anything yet */
	printed = 0;
	entryText = gtk_entry_get_text (GTK_ENTRY (searchText));	//-- what to search for
	sprintf(s->searchText,"%s",entryText);
	  
	if (searchMod) {		/* must have a good module - not null */	 
                int searchType =
			  GTK_TOGGLE_BUTTON (regexSearch)->
			  active ? 0 : GTK_TOGGLE_BUTTON (phraseSearch)->
			  active ? -1 : -2;	//-- get search type	
		s->searchType = searchType;
		int searchParams =
			  GTK_TOGGLE_BUTTON (caseSensitive)->
			  active ? 0 : REG_ICASE;	/* get search params - case sensitive */
		beginHTML(s->srhtml, TRUE);
		sprintf(buf,"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\" vlink=\"#459BD0\" alink=\"#208796\"><font color=\"%s\"><b>[%s]</b><br></font>",
		s->bible_bg_color, 
		s->bible_text_color,
		s->link_color,
		s->bible_verse_num_color,
		searchMod->Name());
		utf8str = e_utf8_from_gtk_string(s->srhtml, buf);
		displayHTML(s->srhtml, utf8str, strlen(utf8str));
		//-- give search string to sword for search
		for (ListKey & searchResults = searchMod->Search (entryText,
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
			list = g_list_append(list,g_strdup(tmpbuf->str));/*
			sprintf(buf,"<font size=\"%s\"><a href=\"%s\">%s</a></font><br>",
			s->verselist_font_size,
			resultText,
			resultText);
			utf8str = e_utf8_from_gtk_string(s->srhtml, buf);
			displayHTML(s->srhtml, utf8str, strlen(utf8str));*/
			searchresultssbMod->SetKey((const char *) searchResults); //-- set key to the first one in the list
			searchresultssbMod->Display(); 
			searchScopeList << (const char *) searchResults;	/* remember finds for next search's scope
			                                                           in case we want to use them */
			if(!count) 
				sprintf(firstkey,"%s",(const char *)searchResults);
			++count;	
                }
        }
	sprintf(buf,"</body</html>");	
	utf8str = e_utf8_from_gtk_string(s->srhtml, buf);
	displayHTML(s->srhtml, utf8str, strlen(utf8str));
	endHTML(s->srhtml);
	if(count){
	/*	ModMap::iterator it; 	
		it = searchresultssbMgr->Modules.find(searchMod->Name()); //-- iterate through the modules until we find modName - modName was passed by the callback
		if (it != searchresultssbMgr->Modules.end()){ //-- if we find the module	
			searchresultssbMod = (*it).second;  //-- change module to new module
			searchresultssbMod->SetKey(firstkey); //-- set key to the first one in the list
			searchresultssbMod->Display(); 
		}	*/	
		/* cleanup appbar progress */
		sprintf(s->groupName,"%s","Search Results");
		sprintf(buf,"%d matches",count);
		gnome_appbar_set_status (GNOME_APPBAR (s->appbar), buf);
		gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(s->app, "nbVL")), 1);
		showSBVerseList(s);
		markSearchWordsHTML(s->srhtml, entryText);
	}
	gnome_appbar_set_progress ((GnomeAppBar *)s->appbar, 0);
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
	
	searchresultssbMgr	= new SWMgr();
	searchresultssbMod     = NULL;
	searchresultssbDisplay = new  SearchResultsDisp(html_widget);
	//searchresultstextsbDisplay = new  GTKutf8ChapDisp(html_widget);
	
	for(it = searchresultssbMgr->Modules.begin(); it != searchresultssbMgr->Modules.end(); it++){
		searchresultssbMod = (*it).second;
		sit = searchresultssbMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters			
		ConfigEntMap &section = (*sit).second;
		addrenderfiltersSWORD(searchresultssbMod, section);
		//if(!strcmp((*it).second->Type(), "Biblical Texts")){
		//	searchresultssbMod->Disp(searchresultstextsbDisplay);			
		//}else{
			searchresultssbMod->Disp(searchresultssbDisplay);
		//}
	}
}

/*** close down searchresults dialog ***/
void shutdownsearchresultsSBSW(void) 
{	
	delete searchresultssbMgr;	
	if(searchresultssbDisplay)
		delete searchresultssbDisplay;	
	if(searchresultstextsbDisplay)
		delete searchresultstextsbDisplay ;
}

void
changesearchresultsSBSW(SETTINGS *s, gchar *url)
{
	
	searchresultssbMod->SetKey(url);
	searchresultssbMod->Display();
	if(s->showinmain)
		changeVerseSWORD(url);
}


