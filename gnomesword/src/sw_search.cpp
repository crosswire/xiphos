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

#include "sw_search.h"
#include "sw_display.h"
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_html.h"
#include "gs_editor.h"
#include "sw_sword.h"
#include "support.h"
#include "sw_utility.h"
#include "sw_verselist_sb.h"
#include "sw_gbs.h"


/***********************************************************************************************
 externals
***********************************************************************************************/
extern SWModule *curMod, *curcomMod, *percomMod;
//extern SWModule *curbookMod;
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

//-------------------------------------------------------------------------------------------
GList*   /* search Bible text or commentaries */
searchSWORD (SETTINGS *s, SEARCH_OPT *so)	
{	
	
	VerseKey searchScopeLowUp; //----------- sets lower and upper search bounds
        ListKey	searchScopeList; //----------- search list for searching verses found on last search
        SWKey	*currentScope; //----------- use to set scope of search	
	SWModule *searchMod,
			*tmpMod;
	SWMgr *searchMgr;
	ModMap::iterator 
		it;
	GtkWidget       
		*searchText,	//-- what we want to find -text entry
	        *lbSearchHits,	//-- label for showing how many verses we found
	        *resultList,	//-- list of verses found
	        *regexSearch,	//-- do we want to use regular expression search - radio button
	        //*phraseSearch,	//-- do we want to use phrase seach - radio button
	        *caseSensitive,	//-- do we want search to be case sensitive - check box
	       //	*comToggle,	//-- do we want to search current commentary - check box
	       //  *percomToggle,	//-- do we want to search personal commentary - check box
	       //	*bookToggle,	//-- do we want to search current book - check box	
	       	*bounds,	//-- do we want to use bounds for our search - check box
	       	*lowerbound,	//-------- lower bounds entry widget
	        *upperbound,	//-------- upper bounds entry widget
	        *lastsearch,	//-- use verses from last search for bounds of this search
	        *textWindow;	//-- text widget to display verses
		
	const gchar	
		*resultText;   //-- temp storage for verse found
	gchar     
		buf[256], 
		buf2[256], 
		*mybuf[2],
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
	searchMgr = new SWMgr();	//-- create sword mgr
	
	searchMod = NULL;
	searchText = lookup_widget (s->app, "entrySearch");	//-- pointer to text entry
	regexSearch = lookup_widget (s->app, "rbRegExp");	//-- pointer to radio button
	//phraseSearch = lookup_widget (s->app, "rbPhraseSearch");	//-- pointer to radio button
	caseSensitive = lookup_widget (s->app, "ckbCaseSensitive");	//-- pointer to check box
	bounds = lookup_widget (s->app, "rrbUseBounds");	//-- pointer to check box
	lastsearch = lookup_widget (s->app, "rbLastSearch");	//-- pointer to radio button
	//comToggle = lookup_widget (s->app, "ckbCommentary");	//-- pointer to check box
	//percomToggle = lookup_widget (s->app, "ckbPerCom");	//-- pointer to check box
	//bookToggle = lookup_widget (s->app, "ckbGBS");	//-- pointer to check box   

	if (GTK_TOGGLE_BUTTON (so->ckbCommentary)->active) {	/* if true search commentary */	  
		  it = searchMgr->Modules.find (curcomMod->Name ());	/* find commentary module */
		  if (it != searchMgr->Modules.end ()) {
			    searchMod = (*it).second;	/* set search module to current commentary module */
			    tmpMod = (*it).second;
		  }
	} 
	
	else if (GTK_TOGGLE_BUTTON (so->ckbPerCom)->active) {	/* if true search personal commentary */	  
		  it = searchMgr->Modules.find (percomMod->Name ());	/* find personal commentary module */
		  if (it != searchMgr->Modules.end ()) {
			    searchMod = (*it).second;	/* set search module to current personalcommentary module */
			    tmpMod = (*it).second;
		  }
	} 
	
	else if (GTK_TOGGLE_BUTTON (so->ckbGBS)->active) {	/* if true search personal commentary */	  
		  it = searchMgr->Modules.find(s->BookWindowModule);	/* find personal commentary module */
		  if (it != searchMgr->Modules.end ()) {
			    searchMod = (*it).second;	/* set search module to current personalcommentary module */
			    tmpMod = (*it).second;
		  }
	} 
	
	else {			/* if neither commertary nor personal check box checked */	 
		  it = searchMgr->Modules.find (curMod->Name());	/* find personal commentary module */
		  if (it != searchMgr->Modules.end ()) {
			    searchMod = (*it).second;	/* set search module to current personalcommentary module */
			    tmpMod = (*it).second;
		  }
	}
	
	if (GTK_TOGGLE_BUTTON (bounds)->active) {
		  lowerbound = lookup_widget (s->app, "entryLower");	/* get Lower bounds entry widget from search form */
		  upperbound = lookup_widget (s->app, "entryUpper");	/* get Upper bounds entry widget from search form */
		  searchScopeLowUp.ClearBounds ();	/* clear old bounds */
		  searchScopeLowUp.LowerBound (gtk_entry_get_text (GTK_ENTRY (lowerbound)));	/* read lower bounds entry and set lower bounds for search */
		  searchScopeLowUp.UpperBound (gtk_entry_get_text (GTK_ENTRY (upperbound)));	/* read upper bounds entry and set upper bounds for search */
		  currentScope = &searchScopeLowUp;	/* set scope of search to use bounds */
	} 
	
	else if (GTK_TOGGLE_BUTTON (lastsearch)->active) {	/* check to see if we want to use results of search last for this search */	  
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
	entryText = gtk_entry_get_text (GTK_ENTRY (searchText));	//-- what to search for
	sprintf(s->searchText, "%s",entryText);
	
	if (searchMod) {		/* must have a good module - not null */	 
                int searchType =
			  GTK_TOGGLE_BUTTON (regexSearch)->
			  active ? 0 : GTK_TOGGLE_BUTTON (so->rbPhraseSearch)->
			  active ? -1 : -2;	//-- get search type	
		s->searchType = searchType;
		int searchParams =
			  GTK_TOGGLE_BUTTON (caseSensitive)->
			  active ? 0 : REG_ICASE;	/* get search params - case sensitive */
		//-- give search string to sword for search
		gtk_clist_clear(GTK_CLIST(clistSearchResults));
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
			list = g_list_append(list,g_strdup(tmpbuf->str)); //-- list retruned so user can save search results as bookmarks
			/* fill clist */
			tmpMod->SetKey((const char *)searchResults);
			VerseKey *key = (VerseKey *) &tmpMod->Key();
			int curVerse = key->Verse();
			int curChapter = key->Chapter();
			//GString* vref = g_string_new((const char *)searchResults);
			/* let's shorten the book name */
			/*vref = g_string_truncate (vref, 4);*/
			sprintf(buf,"%s",(const char *)searchResults);	
					
			mybuf[0] = buf; 							
			//g_string_free(vref,TRUE);
			/*
			vref = g_string_new((char *)tmpMod->StripText());			
			vref = g_string_truncate (vref, 45);
			sprintf(buf2,"%s....",vref->str);	
			mybuf[1] = buf2;
			*/
			gtk_clist_insert(GTK_CLIST(clistSearchResults), count, mybuf);
			//g_string_free(vref,TRUE);
			/* remember finds for next search's scope in case we want to use them */						
			searchScopeList << (const char *) searchResults;	
			if(!count) 
				sprintf(firstkey,"%s",(const char *)searchResults);
			++count;	
                }
        } 
	
	if(count){
		ModMap::iterator it; 	
		it = searchresultssbMgr->Modules.find(searchMod->Name()); //-- iterate through the modules until we find modName - modName was passed by the callback
		if (it != searchresultssbMgr->Modules.end()){ //-- if we find the module	
			searchresultssbMod = (*it).second;  //-- change module to new module
			searchresultssbMod->SetKey(firstkey); //-- set key to the first one in the list			
			settings->displaySearchResults = TRUE;
			searchresultssbMod->Display(); 		
			settings->displaySearchResults = FALSE;	
		}
		sprintf(s->groupName,"%s","Search Results");
		sprintf(buf,"%d matches",count);
		gnome_appbar_set_status (GNOME_APPBAR (s->appbar), buf);
		gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(s->app, "nbVL")), 1);
		showSBVerseList(s);
	}
	
	beginHTML(s->htmlRP, TRUE);
	sprintf(buf,"<html><body><center>%d Occurrences of <br><font color=\"%s\"><b>\"%s\"</b></font><br>found in <font color=\"%s\"><b>[%s]</b></font></center></body</html>", 
				count, s->found_color,s->searchText,
				s->bible_verse_num_color,searchresultssbMod->Name());	
	utf8str = e_utf8_from_gtk_string(s->htmlRP, buf);
	displayHTML(s->htmlRP, utf8str, strlen(utf8str));
	endHTML(s->htmlRP);			
	/* cleanup appbar progress */
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
	
	searchresultssbMgr	= new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	
	searchresultssbMod     = NULL;
	searchresultssbDisplay = new  GtkHTMLEntryDisp(html_widget);
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
changesearchresultsSW_SEARCH(SETTINGS *s, SEARCH_OPT *so, gchar *url)
{	
	searchresultssbMod->SetKey(url);
	searchresultssbMod->Display();
	
	if(s->showinmain) {
		if(GTK_TOGGLE_BUTTON(so->ckbCommentary)->active){
			curcomMod->SetKey(url);
			curcomMod->Display();
		}
				
		else if(GTK_TOGGLE_BUTTON(so->ckbPerCom)->active) {
			percomMod->SetKey(url);
			percomMod->Display();
		}
			
		else if(GTK_TOGGLE_BUTTON(so->ckbGBS)->active) {
			displayinGBS(url);
		}
			
		else
			changeVerseSWORD(url);	
	}
}

