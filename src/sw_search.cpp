/***************************************************************************
                          gs_search.cpp  -  description
                             -------------------
    begin                : Wed Jul 26 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

#include "sw_search.h"
#include "display.h"
#include "gs_gnomesword.h"
#include "gs_sword.h"
#include "support.h"
#include "sw_utility.h"
//#include "sw_verselist_dlg.h"
#include "sw_verselist_sb.h"


extern SWModule *curMod, *curcomMod, *percomMod;
static void percentUpdate(char percent, void *userData) ;
extern gboolean firstsearch;
static char printed = 0;
extern SETTINGS *settings;

//-------------------------------------------------------------------------------------------
void    /* search Bible text or commentaries */
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
		**clistText = (gchar **)&resultText;
	gint            
		count;		//-- number of hits
	GString 
		*string;

	searchMgr = new SWMgr();	//-- create sword mgrs
	searchMod = NULL;
	searchText = lookup_widget (widget, "entrySearch");	//-- pointer to text entry
	//lbSearchHits = lookup_widget (widget, "lbSearchHits");	//-- pointer to count label
	//resultList = lookup_widget (widget, "resultList");	//-- pointer to list
	regexSearch = lookup_widget (widget, "rbRegExp");	//-- pointer to radio button
	phraseSearch = lookup_widget (widget, "rbPhraseSearch");	//-- pointer to radio button
	caseSensitive = lookup_widget (widget, "ckbCaseSensitive");	//-- pointer to check box
	bounds = lookup_widget (widget, "rrbUseBounds");	//-- pointer to check box
	lastsearch = lookup_widget (widget, "rbLastSearch");	//-- pointer to radio button
	comToggle = lookup_widget (widget, "ckbCommentary");	//-- pointer to check box
	percomToggle = lookup_widget (widget, "ckbPerCom");	//-- pointer to check box   
	//textWindow = lookup_widget (widget, "txtSearch");	//-- pointer to text widget



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
	//gtk_label_set_text (GTK_LABEL (lbSearchHits), "0");	//-- set hits label to 0
	string = g_string_new("");
	if (searchMod) {		/* must have a good module - not null */	 
                int searchType =
			  GTK_TOGGLE_BUTTON (regexSearch)->
			  active ? 0 : GTK_TOGGLE_BUTTON (phraseSearch)->
			  active ? -1 : -2;	//-- get search type		
		int searchParams =
			  GTK_TOGGLE_BUTTON (caseSensitive)->
			  active ? 0 : REG_ICASE;	/* get search params - case sensitive */
		//-- give search string to module to search
		for (ListKey & searchResults = searchMod->Search (entryText,
		                                                searchType,
		                                                searchParams,
		                                                currentScope, 0,
		                                                &percentUpdate,
		                                                (void*)&progressunits);
		                                                !searchResults.Error ();
		                                                searchResults++) {		
			resultText = (const char *)searchResults;	//-- put verse key string of find into a string
			if(count){
				sprintf(buf,";%s",resultText);
				string = g_string_append(string,buf);
			}else{
				sprintf(buf,"%s",resultText);
				string = g_string_append(string,buf);
			}
			searchScopeList << (const char *) searchResults;	/* remember finds for next search's scope
			                                                           in case we want to use them */
			++count;	
                }
        }
	if(count){
		getVerseListSBSWORD(searchMod->Name(), string->str, s);
		//g_warning("count = %d\nstring = %s",count,string->str);
		sprintf(buf,"%d found",count);
		gnome_appbar_set_status (GNOME_APPBAR (s->appbar), buf);
	}
	g_string_free(string,TRUE);
	delete searchMgr;
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

