/***************************************************************************
                          searchstuff.cpp  -  description
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
//#include <thmlgbf.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>

#include "searchstuff.h"
#include "display.h"
#include "callback.h"
#include "GnomeSword.h"
#include "support.h"
#include "interface.h"

SearchWindow *searchWindow;
extern SWModule *curMod, *curcomMod, *percomMod;
static void percentUpdate(char percent, void *userData) ;

SearchWindow::SearchWindow ()
{
	searchType_group = NULL;
	_3_group = NULL;
	searchMgr = new SWMgr ();
	searchMod = NULL;
	plaintohtml = new PLAINHTML ();	/* sword renderfilter Plain to html */
	gbftohtml = new GBFHTML ();	/* sword renderfilter gbf to html */
	PLAINsearchDisplay = 0;	/* set in create */
	HTMLsearchDisplay = 0;
	RWPsearchDisplay = 0;
}


SearchWindow::~SearchWindow ()
{
	delete searchMgr;

	if (plaintohtml != 0)	/* delete Sword render filters */
		delete plaintohtml;
	if (gbftohtml != 0)	/* delete Sword render filters */
		delete gbftohtml;
	if (PLAINsearchDisplay)
		delete PLAINsearchDisplay;
	if (HTMLsearchDisplay)
		delete HTMLsearchDisplay;
	if (RWPsearchDisplay)
		delete RWPsearchDisplay;
}

GtkWidget *
create_dlgSearch ()
{
	GtkWidget *retWidget;
	searchWindow = new SearchWindow ();
	retWidget = searchWindow->create ();
	return retWidget;
}

//-------------------------------------------------------------------------------------------
void
SearchWindow::initsearchWindow (GtkWidget * searchDlg)	//-- init search dialog
{
	ModMap::iterator it;	//-- sword manager iterator
	SectionMap::iterator sit;	//-- iteratior
	ConfigEntMap::iterator cit;	//-- iteratior
	gchar *sourceformat;

	gtk_text_set_word_wrap (GTK_TEXT(lookup_widget (searchDlg, "txtSearch")), TRUE); //-- set text window to word wrap
	PLAINsearchDisplay = new GTKEntryDisp (lookup_widget (searchDlg, "txtSearch"));	//-- set sword display for gbf
	HTMLsearchDisplay = new HTMLentryDisp (lookup_widget (searchDlg, "txtSearch"));	//-- set sword display for html
	RWPsearchDisplay = new GTKRWPDisp (lookup_widget (searchDlg, "txtSearch"));	//-- set sword display for html
	//--------------------------------------------------------------------------------------- searchmodule      
        for (it = searchMgr->Modules.begin (); it != searchMgr->Modules.end (); it++) {	/* iterate through modules */	  
		searchMod = (*it).second;	//-- set searchMod
		sit = searchMgr->config->Sections.find ((*it).second->Name ());	/* check to see if we need render filters */
		if (sit != searchMgr->config->Sections.end ()) {
		        cit = (*sit).second.find ("SourceType");
			if (cit != (*sit).second.end ())
				    sourceformat = g_strdup ((*cit).second.c_str ());
			else
				    sourceformat = g_strdup ("Plain");
		}
		if (!strcmp (sourceformat, "GBF")) {	/* we need gbf to html filter */		    
			searchMod->AddRenderFilter (gbftohtml);
			searchMod->Disp (HTMLsearchDisplay);
		}
		else if (!strcmp (sourceformat, "ThML")) {	/* we need ThML to html filter */		    
			//curcomMod->AddRenderFilter(thmltohtml);
			searchMod->Disp (HTMLsearchDisplay);
		} else if ((*searchMgr->config->Sections[(*it).second->Name ()].find ("ModDrv")).second == "RawFiles") {	/* if driver is RawFiles */		    
			searchMod->Disp (HTMLsearchDisplay);	//-- no filter needed
		} else if (!strcmp (searchMod->Name (), "RWP"))	/* the rwp to html filter needs work */
			searchMod->Disp (RWPsearchDisplay);	//-- so we will use our own
                else {
			//searchMod->AddRenderFilter(plaintohtml);
			searchMod->Disp (PLAINsearchDisplay);
			//cout << searchMod->Name() << '\n';                                    
                }
		g_free (sourceformat);
        }
}

//-------------------------------------------------------------------------------------------
void    /* search Bible text or commentaries */
SearchWindow::searchSWORD (GtkWidget * searchFrm)	
{
	ModMap::iterator it;
	GtkWidget       *searchText,	//-- what we want to find -text entry
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
	gchar           *entryText,	//-- pointer to text in searchText entry
	                scount[5];	//-- string from gint count for label
	const char      *resultText;	//-- temp storage for verse found
	gint            count;		//-- number of hits

	searchText = lookup_widget (searchFrm, "entry1");	//-- pointer to text entry
	lbSearchHits = lookup_widget (searchFrm, "lbSearchHits");	//-- pointer to count label
	resultList = lookup_widget (searchFrm, "resultList");	//-- pointer to list
	regexSearch = lookup_widget (searchFrm, "regexSearch");	//-- pointer to radio button
	phraseSearch = lookup_widget (searchFrm, "phraseSearch");	//-- pointer to radio button
	caseSensitive = lookup_widget (searchFrm, "caseSensitive");	//-- pointer to check box
	bounds = lookup_widget (searchFrm, "rbUseBounds");	//-- pointer to check box
	lastsearch = lookup_widget (searchFrm, "rbLastSearch");	//-- pointer to radio button
	comToggle = lookup_widget (searchFrm, "ckbCom");	//-- pointer to check box
	percomToggle = lookup_widget (searchFrm, "cbpercom");	//-- pointer to check box   
	textWindow = lookup_widget (searchFrm, "txtSearch");	//-- pointer to text widget


	gtk_text_set_point (GTK_TEXT (textWindow), 0);	/* clear text window */
	gtk_text_forward_delete (GTK_TEXT (textWindow),
				 gtk_text_get_length ((GTK_TEXT(textWindow))));

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
		  lowerbound = lookup_widget (searchFrm, "entryLower");	/* get Lower bounds entry widget from search form */
		  upperbound = lookup_widget (searchFrm, "entryUpper");	/* get Upper bounds entry widget from search form */
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
	gtk_clist_clear (GTK_CLIST (resultList));	//-- clear list widget for new results
	entryText = gtk_entry_get_text (GTK_ENTRY (searchText));	//-- what to search for
	gtk_label_set_text (GTK_LABEL (lbSearchHits), "0");	//-- set hits label to 0
	if (searchMod) {		/* must have a good module - not null */	 
                int searchType =
			  GTK_TOGGLE_BUTTON (regexSearch)->
			  active ? 0 : GTK_TOGGLE_BUTTON (phraseSearch)->
			  active ? -1 : -2;	//-- get search type		
		int searchParams =
			  GTK_TOGGLE_BUTTON (caseSensitive)->
			  active ? 0 : REG_ICASE;	/* get search params - case sensitive */
		gtk_clist_freeze (GTK_CLIST (resultList));	//-- keep list form scrolling until we are done
		//-- give search string to module to search
		for (ListKey & searchResults = searchMod->Search (entryText, searchType, searchParams, 
											currentScope, 0, 
											&percentUpdate,  (void*)&progressunits);
		       !searchResults.Error (); 
		       searchResults++) {		    
			    resultText = (const char *) searchResults;	//-- put verse key string of find into a string
			    gtk_clist_append (GTK_CLIST (resultList), &resultText);	//-- store find in list
			    searchScopeList << (const char *) searchResults;	/* remember finds for next search's scope */
			    ++count;	//-- if we want to use them
                }
		gtk_clist_thaw (GTK_CLIST (resultList));	//-- thaw list so we can look through the results
		sprintf (scount, "%d", count);	//-- put count into string
		gtk_label_set_text (GTK_LABEL (lbSearchHits), scount);	//-- tell user how many hits we had
        }
}

//-------------------------------------------------------------------------------------------
void
SearchWindow::resultsListSWORD (GtkWidget * searchFrm, gint row, gint column)	//-- someone clicked the results list
{				                                                //-- from our search and sent us here
	GtkWidget       *resultList,	//-- pointer to resultlist        
	                *contextToggle;	//-- pointer to context check box        
	gchar           *text;		//-- pointer to resultlist key text

	resultList = lookup_widget (searchFrm, "resultList");	//-- set pointer to resultList
	contextToggle = lookup_widget (searchFrm, "cbContext");	//-- set pointer to context check box     
	gtk_clist_get_text (GTK_CLIST (resultList), row, column, &text);	//-- get key text from resultlist
	if (searchMod) {	/* make sure module is not null */	  
		  searchMod->SetKey (text);	//-- set module to verse key text
		  searchMod->Display ();	//-- show verse or commentary
	}
	if (GTK_TOGGLE_BUTTON (contextToggle)->active) {	/* check state of context check box */        
		  /* if true */
		  changeVerse (text);	/* show selection in main window */
	}
}

//-------------------------------------------------------------------------------------------
static void percentUpdate(char percent, void *userData) {
	/*static char printed = 0;
	char maxHashes = *((char *)userData);
	char buf[80];
	
	while ((((float)percent)/100) * maxHashes > printed) {
		gtk_progress_set_value(GTK_PROGRESS(searchWindow->progressbar),(float)percent) ;
		gtk_widget_grab_focus(searchWindow->progressbar);
		sprintf(buf,"%f",(((float)percent)/100));
		cout << buf << '\n';
		printed++;	
	}*/
}
//-------------------------------------------------------------------------------------------
GtkWidget *
SearchWindow::create ()
{
	dlgSearch = gnome_dialog_new ("GnomeSword - Search", NULL);
	gtk_object_set_data (GTK_OBJECT (dlgSearch), "dlgSearch", dlgSearch);
	gtk_container_set_border_width (GTK_CONTAINER (dlgSearch), 4);
	GTK_WINDOW (dlgSearch)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_policy (GTK_WINDOW (dlgSearch), TRUE, TRUE, TRUE);

	dialog_vbox1 = GNOME_DIALOG (dlgSearch)->vbox;
	gtk_object_set_data (GTK_OBJECT (dlgSearch), "dialog_vbox1",
			     dialog_vbox1);
	gtk_widget_show (dialog_vbox1);
  	/* Create a GtkAdjusment object to hold the range of the
           * progress bar */
          //adj = (GtkAdjustment *) gtk_adjustment_new (0, 1, 100, 0, 0, 0);

          /* Create the GtkProgressBar using the adjustment */
        /* progressbar = gtk_progress_bar_new_with_adjustment (adj);

 
  	//progressbar = gtk_progress_bar_new ();
  	gtk_widget_ref (progressbar);
  	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "progressbar", progressbar,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (progressbar);
  	gtk_box_pack_end (GTK_BOX (dialog_vbox1), progressbar, FALSE, TRUE, 0);
  	gtk_progress_set_show_text (GTK_PROGRESS (progressbar), TRUE);
	gtk_progress_bar_set_bar_style (GTK_PROGRESS_BAR (progressbar),
                                          GTK_PROGRESS_CONTINUOUS);	
        */		      
				      
	frame8 = gtk_frame_new (NULL);
	gtk_widget_ref (frame8);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "frame8", frame8,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (frame8);
	gtk_box_pack_start (GTK_BOX (dialog_vbox1), frame8, TRUE, TRUE, 0);

	searchPanel = gtk_vbox_new (FALSE, 0);
	gtk_widget_ref (searchPanel);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "searchPanel",
				  searchPanel,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (searchPanel);
	gtk_container_add (GTK_CONTAINER (frame8), searchPanel);
	gtk_widget_set_usize (searchPanel, 279, -2);
	gtk_container_set_border_width (GTK_CONTAINER (searchPanel), 2);

	hbox4 = gtk_hbox_new (FALSE, 0);
	gtk_widget_ref (hbox4);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "hbox4", hbox4,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (hbox4);
	gtk_box_pack_start (GTK_BOX (searchPanel), hbox4, FALSE, TRUE, 0);

	toolbar24 =
		gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL,
				 GTK_TOOLBAR_ICONS);
	gtk_widget_ref (toolbar24);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "toolbar24",
				  toolbar24,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (toolbar24);
	gtk_box_pack_start (GTK_BOX (hbox4), toolbar24, FALSE, TRUE, 0);
	gtk_widget_set_usize (toolbar24, -2, 27);
	gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar24),
				       GTK_RELIEF_NONE);

	tmp_toolbar_icon =
		gnome_stock_pixmap_widget (dlgSearch,
					   GNOME_STOCK_PIXMAP_SAVE);
	btnSearchSaveList =
		gtk_toolbar_append_element (GTK_TOOLBAR (toolbar24),
					    GTK_TOOLBAR_CHILD_BUTTON, NULL,
					    "Save List",
					    "Save Results of search", NULL,
					    tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref (btnSearchSaveList);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "btnSearchSaveList",
				  btnSearchSaveList,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btnSearchSaveList);

	entry1 = gtk_entry_new ();
	gtk_widget_ref (entry1);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "entry1", entry1,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (entry1);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar24), entry1, NULL,
				   NULL);
	gtk_widget_set_usize (entry1, 216, 24);

	tmp_toolbar_icon =
		gnome_stock_pixmap_widget (dlgSearch,
					   GNOME_STOCK_PIXMAP_SEARCH);
	btnSearch1 =
		gtk_toolbar_append_element (GTK_TOOLBAR (toolbar24),
					    GTK_TOOLBAR_CHILD_BUTTON, NULL,
					    "button4", "Start Search", NULL,
					    tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref (btnSearch1);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "btnSearch1",
				  btnSearch1,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btnSearch1);

	searchSettingsPanel = gtk_hbox_new (FALSE, 0);
	gtk_widget_ref (searchSettingsPanel);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch),
				  "searchSettingsPanel", searchSettingsPanel,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (searchSettingsPanel);
	gtk_box_pack_start (GTK_BOX (searchPanel), searchSettingsPanel, FALSE,
			    FALSE, 0);

	frame15 = gtk_frame_new ("Search Type");
	gtk_widget_ref (frame15);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "frame15", frame15,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (frame15);
	gtk_box_pack_start (GTK_BOX (searchSettingsPanel), frame15, FALSE,
			    FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (frame15), 1);

	searchTypePanel = gtk_vbox_new (FALSE, 0);
	gtk_widget_ref (searchTypePanel);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "searchTypePanel",
				  searchTypePanel,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (searchTypePanel);
	gtk_container_add (GTK_CONTAINER (frame15), searchTypePanel);

	regexSearch =
		gtk_radio_button_new_with_label (searchType_group,
						 "Regular Expression");
	searchType_group =
		gtk_radio_button_group (GTK_RADIO_BUTTON (regexSearch));
	gtk_widget_ref (regexSearch);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "regexSearch",
				  regexSearch,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (regexSearch);
	gtk_box_pack_start (GTK_BOX (searchTypePanel), regexSearch, FALSE,
			    FALSE, 0);

	phraseSearch =
		gtk_radio_button_new_with_label (searchType_group,
						 "Exact Phrase");
	searchType_group =
		gtk_radio_button_group (GTK_RADIO_BUTTON (phraseSearch));
	gtk_widget_ref (phraseSearch);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "phraseSearch",
				  phraseSearch,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (phraseSearch);
	gtk_box_pack_start (GTK_BOX (searchTypePanel), phraseSearch, FALSE,
			    FALSE, 0);

	multiWordSearch =
		gtk_radio_button_new_with_label (searchType_group,
						 "Multi Word");
	searchType_group =
		gtk_radio_button_group (GTK_RADIO_BUTTON (multiWordSearch));
	gtk_widget_ref (multiWordSearch);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "multiWordSearch",
				  multiWordSearch,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (multiWordSearch);
	gtk_box_pack_start (GTK_BOX (searchTypePanel), multiWordSearch, FALSE,
			    FALSE, 0);

	frame16 = gtk_frame_new ("Search Options");
	gtk_widget_ref (frame16);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "frame16", frame16,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (frame16);
	gtk_box_pack_start (GTK_BOX (searchSettingsPanel), frame16, FALSE,
			    FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (frame16), 1);

	searchOptionsPanel = gtk_vbox_new (FALSE, 0);
	gtk_widget_ref (searchOptionsPanel);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch),
				  "searchOptionsPanel", searchOptionsPanel,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (searchOptionsPanel);
	gtk_container_add (GTK_CONTAINER (frame16), searchOptionsPanel);

	caseSensitive = gtk_check_button_new_with_label ("Case Sensitive");
	gtk_widget_ref (caseSensitive);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "caseSensitive",
				  caseSensitive,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (caseSensitive);
	gtk_box_pack_start (GTK_BOX (searchOptionsPanel), caseSensitive,
			    FALSE, FALSE, 0);

	ckbCom = gtk_check_button_new_with_label ("Search Commentary");
	gtk_widget_ref (ckbCom);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "ckbCom", ckbCom,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (ckbCom);
	gtk_box_pack_start (GTK_BOX (searchOptionsPanel), ckbCom, FALSE,
			    FALSE, 0);

	cbpercom = gtk_check_button_new_with_label ("Search Personal");
	gtk_widget_ref (cbpercom);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "cbpercom",
				  cbpercom,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (cbpercom);
	gtk_box_pack_start (GTK_BOX (searchOptionsPanel), cbpercom, FALSE,
			    FALSE, 0);

	frame20 = gtk_frame_new ("Search Scope");
	gtk_widget_ref (frame20);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "frame20", frame20,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (frame20);
	gtk_box_pack_start (GTK_BOX (searchPanel), frame20, FALSE, TRUE, 0);

	hbox17 = gtk_hbox_new (FALSE, 0);
	gtk_widget_ref (hbox17);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "hbox17", hbox17,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (hbox17);
	gtk_container_add (GTK_CONTAINER (frame20), hbox17);

	rbNoScope = gtk_radio_button_new_with_label (_3_group, "No Scope");
	_3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbNoScope));
	gtk_widget_ref (rbNoScope);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "rbNoScope",
				  rbNoScope,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (rbNoScope);
	gtk_box_pack_start (GTK_BOX (hbox17), rbNoScope, FALSE, FALSE, 0);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rbNoScope), TRUE);

	rbUseBounds =
		gtk_radio_button_new_with_label (_3_group, "Use Bounds");
	_3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbUseBounds));
	gtk_widget_ref (rbUseBounds);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "rbUseBounds",
				  rbUseBounds,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (rbUseBounds);
	gtk_box_pack_start (GTK_BOX (hbox17), rbUseBounds, FALSE, FALSE, 0);

	rbLastSearch =
		gtk_radio_button_new_with_label (_3_group, "Last Search");
	_3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbLastSearch));
	gtk_widget_ref (rbLastSearch);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "rbLastSearch",
				  rbLastSearch,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (rbLastSearch);
	gtk_box_pack_start (GTK_BOX (hbox17), rbLastSearch, FALSE, FALSE, 0);

	table6 = gtk_table_new (2, 2, FALSE);
	gtk_widget_ref (table6);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "table6", table6,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (table6);
	gtk_box_pack_start (GTK_BOX (searchPanel), table6, FALSE, TRUE, 0);

	label82 = gtk_label_new ("Lower bounds");
	gtk_widget_ref (label82);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "label82", label82,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label82);
	gtk_table_attach (GTK_TABLE (table6), label82, 0, 1, 0, 1,
			  (GtkAttachOptions) (0),
			  (GtkAttachOptions) (0), 0, 0);

	label83 = gtk_label_new ("Uppen bounds");
	gtk_widget_ref (label83);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "label83", label83,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label83);
	gtk_table_attach (GTK_TABLE (table6), label83, 0, 1, 1, 2,
			  (GtkAttachOptions) (0),
			  (GtkAttachOptions) (0), 0, 0);

	entryLower = gtk_entry_new ();
	gtk_widget_ref (entryLower);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "entryLower",
				  entryLower,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (entryLower);
	gtk_table_attach (GTK_TABLE (table6), entryLower, 1, 2, 0, 1,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	gtk_entry_set_text (GTK_ENTRY (entryLower), "Genesis");

	entryUpper = gtk_entry_new ();
	gtk_widget_ref (entryUpper);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "entryUpper",
				  entryUpper,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (entryUpper);
	gtk_table_attach (GTK_TABLE (table6), entryUpper, 1, 2, 1, 2,
			  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
			  (GtkAttachOptions) (0), 0, 0);
	gtk_entry_set_text (GTK_ENTRY (entryUpper), "Revelation");

	scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_ref (scrolledwindow2);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "scrolledwindow2",
				  scrolledwindow2,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (scrolledwindow2);
	gtk_box_pack_start (GTK_BOX (searchPanel), scrolledwindow2, TRUE,
			    TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2),
					GTK_POLICY_NEVER,
					GTK_POLICY_AUTOMATIC);

	resultList = gtk_clist_new (1);
	gtk_widget_ref (resultList);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "resultList",
				  resultList,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (resultList);
	gtk_container_add (GTK_CONTAINER (scrolledwindow2), resultList);
	gtk_widget_set_usize (resultList, -2, 87);
	gtk_clist_set_column_width (GTK_CLIST (resultList), 0, 80);
	gtk_clist_column_titles_hide (GTK_CLIST (resultList));

	label46 = gtk_label_new ("label46");
	gtk_widget_ref (label46);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "label46", label46,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label46);
	gtk_clist_set_column_widget (GTK_CLIST (resultList), 0, label46);

	scrolledwindow20 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_ref (scrolledwindow20);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "scrolledwindow20",
				  scrolledwindow20,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (scrolledwindow20);
	gtk_box_pack_start (GTK_BOX (searchPanel), scrolledwindow20, FALSE,
			    TRUE, 0);
	gtk_widget_set_usize (scrolledwindow20, -2, 72);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW
					(scrolledwindow20), GTK_POLICY_NEVER,
					GTK_POLICY_AUTOMATIC);

	txtSearch = gtk_text_new (NULL, NULL);
	gtk_widget_ref (txtSearch);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "txtSearch",
				  txtSearch,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (txtSearch);
	gtk_container_add (GTK_CONTAINER (scrolledwindow20), txtSearch);

	cbContext =
		gtk_check_button_new_with_label ("Show Verse in Main window");
	gtk_widget_ref (cbContext);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "cbContext",
				  cbContext,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (cbContext);
	gtk_box_pack_start (GTK_BOX (searchPanel), cbContext, FALSE, FALSE,
			    0);

	hbox15 = gtk_hbox_new (FALSE, 0);
	gtk_widget_ref (hbox15);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "hbox15", hbox15,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (hbox15);
	gtk_box_pack_start (GTK_BOX (searchPanel), hbox15, FALSE, TRUE, 0);

	label57 = gtk_label_new ("Found: ");
	gtk_widget_ref (label57);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "label57", label57,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (label57);
	gtk_box_pack_start (GTK_BOX (hbox15), label57, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label57), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_padding (GTK_MISC (label57), 4, 0);

	lbSearchHits = gtk_label_new ("0");
	gtk_widget_ref (lbSearchHits);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "lbSearchHits",
				  lbSearchHits,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (lbSearchHits);
	gtk_box_pack_start (GTK_BOX (hbox15), lbSearchHits, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (lbSearchHits), GTK_JUSTIFY_LEFT);

	dialog_action_area1 = GNOME_DIALOG (dlgSearch)->action_area;
	gtk_object_set_data (GTK_OBJECT (dlgSearch), "dialog_action_area1",
			     dialog_action_area1);
	gtk_widget_show (dialog_action_area1);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1),
				   GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area1), 8);

	gnome_dialog_append_button (GNOME_DIALOG (dlgSearch),
				    GNOME_STOCK_BUTTON_CLOSE);
	btnSearchOK =
		(GtkWidget *) g_list_last (GNOME_DIALOG (dlgSearch)->
					   buttons)->data;
	gtk_widget_ref (btnSearchOK);
	gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "btnSearchOK",
				  btnSearchOK,
				  (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (btnSearchOK);
	GTK_WIDGET_SET_FLAGS (btnSearchOK, GTK_CAN_DEFAULT);

	gtk_signal_connect (GTK_OBJECT (btnSearchSaveList), "clicked",
			    GTK_SIGNAL_FUNC (on_btnSearchSaveList_clicked),
			    NULL);
	gtk_signal_connect (GTK_OBJECT (btnSearch1), "clicked",
			    GTK_SIGNAL_FUNC (on_btnSearch1_clicked), NULL);
	gtk_signal_connect (GTK_OBJECT (resultList), "select_row",
			    GTK_SIGNAL_FUNC (on_resultList_select_row), NULL);
	gtk_signal_connect (GTK_OBJECT (cbContext), "toggled",
			    GTK_SIGNAL_FUNC (on_cbContext_toggled), NULL);
	gtk_signal_connect (GTK_OBJECT (btnSearchOK), "clicked",
			    GTK_SIGNAL_FUNC (on_btnSearchOK_clicked), NULL);

	return dlgSearch;
}

/* check to see if we want to set bounds for search */
