/***************************************************************************
                          GnomeSword.cpp  -  description
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/
 
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
 *  You should have received a copy of the GNU Library General Public License
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
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>

#include "display.h"
#include "callback.h"
#include "GnomeSword.h"
#include "support.h"
#include "interface.h"
#include "filestuff.h"
#include "menustuff.h"

/*
 *
 *  -- add to interface.cpp after glade builds code --
 *  -- must be insereted following creation of appbar --
 * gnome_app_install_menu_hints(GNOME_APP(mainwindow), menubar1_uiinfo);
 * 
 */


//------------------------------------------------- sword global to this file
SWDisplay *chapDisplay; //--- to display modules using GtkText a chapter at a time
SWDisplay *entryDisplay; //--- to display modules using GtkText a verse at a time
SWDisplay *comp1Display; //--- to display modules using GtkText a verse at a time
SWDisplay *comp2Display; //--- to display modules using GtkText a verse at a time
SWDisplay *comp3Display; //--- to display modules using GtkText a verse at a time
SWDisplay *comDisplay; //--- to display modules using GtkText a verse at a time
SWDisplay *dictDisplay; //--- to display modules using GtkText a verse at a time
SWDisplay *BTFcomDisplay; //--- to display modules using GtkText a verse at a time
SWDisplay *BTFsearchDisplay; //--- to display modules using GtkText a verse at a time
SWMgr *mainMgr; //-- sword mgr for curMod - curcomMod - curdictMod
SWMgr *mainMgr1; //-- sword mgr for comp1Mod - first interlinear module
SWMgr *mainMgr2; //-- sword mgr for comp2Mod - second interlinear module 
SWMgr *mainMgr3; //-- sword mgr for comp3Mod - third interlinear module 
SWMgr *searchMgr; //-- sword mgr for searchMod - module used for searching
VerseKey vsKey; //-- ??
SWModule *curMod; //-- module for main text window
SWModule *comp1Mod; //-- module for first interlinear window
SWModule *comp2Mod; //-- module for second interlinear window	
SWModule *comp3Mod;	 //-- module for third interlinear window
SWModule *curcomMod; //-- module for commentary  window	
SWModule *curdictMod; //-- module for dict window
SWModule *searchMod; //-- module for searching and search window

VerseKey searchScopeLowUp; //----------- sets lower and upper search bounds
ListKey	searchScopeList; //----------- search list for searching verses found on last search
SWKey	*currentScope; //----------- use to set scope of search



//---------------------------------------------------------------- gSword global to this file		
GtkWidget *MainFrm,	//-- main form widget
					*bookmark_mnu; //-- popup menu for bookmarks

gchar *current_filename= NULL;	//-- filename for open file in study pad window 
gchar current_verse[45]="Romans 8:28";	//-- current verse showing in main window - 1st - 2nd - 3rd interlinear window - commentary window
bool ApplyChange = TRUE;	//-- should we make changes when cbBook is changed
bool bVerseStyle = TRUE;	//-- should we show verses of paragraphs in main text window

VerseKey swKey = "Romans 8:28";	//-- ?? 
gint curChapter = 8;	//-- keep up with current chapter
gint curVerse =28;	//-- keep up with current verse

gboolean file_changed = FALSE;	//-- set to true if text is study pad has changed - and file is not saved
gchar options[11][80],	//-- array to store a number of setting - read in form file when program starts - saved to file at normal shut down
		 bmarks[50][80];	//-- array to store bookmarks - read in form file when program starts - saved to file after edit
GtkWidget 	*versestyle,	//-- widget to access toggle menu - for versestyle
				*footnotes,	//-- widget to access toggle menu - for footnotes
				*notepage;	//-- widget to access toggle menu - for interlinear notebook page
				
extern gint ibookmarks;	//-- number of items in bookmark menu
extern GdkColor myGreen;



//----------------------------------------------------------------------------------------------
void
initSword(GtkWidget *mainform,  //-- app's main form
				GtkWidget *menu1,  //-- main text window's popup menu
				GtkWidget *menu2,  //-- 1st inetrlinear window's popup menu
				GtkWidget *menu3,  //-- 2nd inetrlinear window's popup menu
				GtkWidget *menu4,  //-- 3rd inetrlinear window's popup menu
				GtkWidget *menu5) //-- bookmark button popup menu
{
	ModMap::iterator it; //-- iteratior
	SectionMap::iterator sit; //-- iteratior
	ConfigEntMap::iterator eit; //-- iteratior
	int pg=0; //-- notebook page number - for creating notebook pages
	GtkWidget  *empty_notebook_page, //-- used to create new pages
					*notebook, //-- to access notebooks for adding pages
					*menuChoice, //-- gtk menu item
					*popmenu, //-- to access popup menus for adding items
					*menuChoice1, //-- gtk menu item
					*label, //-- labels for adding notebook pages
					*menu; //-- ??
	int 			pg2=0; //-- notebook page numger
				//iBibletext=0; //--
	char          menuName[64], //--  for menu item label
					menuName1[64], //--  for menu item label
					mybuf[80], //-- ??
					rememberlastitem[80]; //--  use to store last menu item so we can add the next item under it - gnome menus
	int             viewNumber = 1, //--  for numbering menu items
					viewNumber1= 1, //-- for numbering menu items
					itemNum = 0, //-- for numbering menu items
					i, //-- counter
					j; //-- counter

	GnomeUIInfo *menuitem; //--  gnome menuitem


	mainMgr         = new SWMgr();	//-- create sword mgrs
	mainMgr1        = new SWMgr();
	mainMgr2        = new SWMgr();
	mainMgr3        = new SWMgr();
	searchMgr		= new SWMgr();
	curMod           = NULL; //-- set mods to null
	comp1Mod      = NULL;
	comp2Mod      = NULL;
	comp3Mod      = NULL;
	curcomMod     = NULL;
	curdictMod      = NULL;
	searchMod      = NULL;
	chapDisplay    = 0;// set in create
	entryDisplay    = 0;// set in create
	comp1Display    = 0;// set in create
	comp2Display    = 0;// set in create
	comp3Display    = 0;// set in create
	comDisplay   	  = 0;// set in create
	dictDisplay   	   = 0;// set in create
	BTFcomDisplay = 0; // set in create
	BTFsearchDisplay =0; // set in create


//	gtk_rc_parse( "gsword.rc" );

  myGreen.red = 0x0000; //-- set color for current verse
	myGreen.green = 0xbbbb;
	myGreen.blue = 0x0000;
	
	MainFrm = lookup_widget(mainform,"mainwindow"); //-- save mainform for use latter
//	bookmark_mnu = lookup_widget(menu5,"pumuBookmarks"); //-- save menu for use latter
	//--------------------------------------------------------------------- setup displays for sword modules
	GTKEntryDisp::__initialize();
	chapDisplay = new GTKChapDisp(lookup_widget(mainform,"moduleText"));
	dictDisplay = new GTKEntryDisp(lookup_widget(mainform,"textDict"));
	comDisplay = new  GTKEntryDisp(lookup_widget(mainform,"textComments"));
	BTFcomDisplay = new  GTKInterlinearDisp(lookup_widget(mainform,"textComments"));
	comp1Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp1"));
	comp2Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp2"));
	comp3Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp3"));

//----------------------------------------------------------------------- set text windows to word warp
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"moduleText")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp1")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp2")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp3")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textDict")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComments")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"text3")) , TRUE );

	//-------------------------------------------------------------- set main window modules and add to menus	
	sprintf(rememberlastitem,"%s","_View/Main Window/");
	for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) 
	{

		if (!strcmp((*it).second->Type(), "Biblical Texts")) 
		{
			curMod = (*it).second;
			 //------------------------------------------------------------------ add to menubar
			additemtognomemenu(MainFrm, curMod->Name(), rememberlastitem , (GtkMenuCallback)on_mainText_activate );  
			//--------------------------------------- remember last item - so next item will be place below it       	
			sprintf(rememberlastitem,"%s%s","_View/Main Window/",curMod->Name());			
			//---------------------------------------------------------------- add to popup menu
			additemtopopupmenu(MainFrm, menu1, curMod->Name(), (GtkMenuCallback)on_mainText_activate );
			//------------------------------------------------------- set GTK display for each module
			curMod->Disp(chapDisplay); 
		}
		else	
		{
			//------------------------------------------------------------------ set commentary modules and add to notebook
			if (!strcmp((*it).second->Type(), "Commentaries"))
			{
				curcomMod = (*it).second;
				notebook = lookup_widget(mainform,"notebook1");
				empty_notebook_page = gtk_vbox_new (FALSE, 0);
  				gtk_widget_show (empty_notebook_page);
  				gtk_container_add (GTK_CONTAINER (notebook), empty_notebook_page);
				label = gtk_label_new (curcomMod->Name());
			    gtk_widget_show (label);
				gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), pg++), label);
				if(strcmp(curcomMod->Name(),"Family")) curcomMod->Disp(comDisplay);
				else curcomMod->Disp(BTFcomDisplay);
			}	
			//------------------------------------------------------------------ set dictionary modules and add to notebook
			if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries"))
			{	
				curdictMod = (*it).second;
				notebook = lookup_widget(mainform,"notebook4");
				
				empty_notebook_page = gtk_vbox_new (FALSE, 0);
  				gtk_widget_show (empty_notebook_page);
  				gtk_container_add (GTK_CONTAINER (notebook), empty_notebook_page);
				label = gtk_label_new (curdictMod->Name());
			    gtk_widget_show (label);
				gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), pg2++), label);
				curdictMod->Disp(dictDisplay);
			}
		}
	}	
 
	//------------------------------------------------------------------------------------------- interlinear 1	
	sprintf(rememberlastitem,"%s","_View/Interlinear1 Window/");
	for (it = mainMgr1->Modules.begin(); it != mainMgr1->Modules.end(); it++) 
	{	
		comp1Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")) 
		{						
			 //------------------------------------------------------------------------------- add to menubar
			additemtognomemenu(MainFrm, comp1Mod->Name(), rememberlastitem , (GtkMenuCallback)on_1st_interlinear_window1_activate);  
			//---------------------------------------------------- remember last item - so next item will be place below it       	
			sprintf(rememberlastitem,"%s%s","_View/Interlinear1 Window/",comp1Mod->Name());			
			//----------------------------------------------------------------------------- add to popup menu
			additemtopopupmenu(MainFrm, menu2, comp1Mod->Name(), (GtkMenuCallback)on_1st_interlinear_window1_activate);
			//-------------------------------------------------------------------- set GTK display for each module
			comp1Mod->Disp(comp1Display);
		}
	}

	//------------------------------------------------------------------------------------------- interlinear 2	
	sprintf(rememberlastitem,"%s","_View/Interlinear2 Window/"); //----- set rememberlastitem for interlinear 2
	for (it = mainMgr2->Modules.begin(); it != mainMgr2->Modules.end(); it++) 
	{
		comp2Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")) 
		{						
			 //------------------------------------------------------------------------------- add to menubar
			additemtognomemenu(MainFrm, comp2Mod->Name(), rememberlastitem , (GtkMenuCallback)on_2nd_interlinear_window1_activate);  
			//---------------------------------------------------- remember last item - so next item will be place below it       	
			sprintf(rememberlastitem,"%s%s","_View/Interlinear2 Window/",comp2Mod->Name());			
			//----------------------------------------------------------------------------- add to popup menu
			additemtopopupmenu(MainFrm, menu3, comp2Mod->Name(), (GtkMenuCallback)on_2nd_interlinear_window1_activate);
			//-------------------------------------------------------------------- set GTK display for each module
			comp2Mod->Disp(comp2Display);
		}
	}

	//------------------------------------------------------------------------------------------- interlinear 3	
	sprintf(rememberlastitem,"%s","_View/Interlinear3 Window/");	
	for (it = mainMgr3->Modules.begin(); it != mainMgr3->Modules.end(); it++) 
	{
		comp3Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")) 
		{
			 //------------------------------------------------------------------------------- add to menubar
			additemtognomemenu(MainFrm, comp3Mod->Name(), rememberlastitem , (GtkMenuCallback)on_3rd_interlinear_window1_activate);  
			//---------------------------------------------------- remember last item - so next item will be place below it       	
			sprintf(rememberlastitem,"%s%s","_View/Interlinear3 Window/",comp3Mod->Name());			
			//----------------------------------------------------------------------------- add to popup menu
			additemtopopupmenu(MainFrm, menu4, comp3Mod->Name(), (GtkMenuCallback)on_3rd_interlinear_window1_activate);
			//-------------------------------------------------------------------- set GTK display for each module
			comp3Mod->Disp(comp3Display);
		}
	}

	//----------------------------------------------------------------------------- set dict module to open notebook page
	GtkLabel *label1;		
	notebook = lookup_widget(mainform,"notebook4");	
	label1 = (GtkLabel *)gtk_notebook_get_tab_label (GTK_NOTEBOOK(notebook),
						gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),0));
	it = mainMgr->Modules.find((char *)label1->label); 	
	if (it != mainMgr->Modules.end()) 
	{
		curdictMod = (*it).second;
		curdictMod->SetKey("GRACE");
		curdictMod->Display();	
		FillDictKeys(curdictMod->Name()); 	
	}
	//---------------------------------------------------------------------------- set com module to open notebook page	
	notebook = lookup_widget(mainform,"notebook1");	
	label1 = (GtkLabel *)gtk_notebook_get_tab_label (GTK_NOTEBOOK(notebook),
						gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),0));
	it = mainMgr->Modules.find((char *)label1->label); 
	if (it != mainMgr->Modules.end()) 
	{
		curcomMod = (*it).second;	
	}	
	gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                      GTK_SIGNAL_FUNC (on_notebook1_switch_page),
                      NULL);
	

	//--------------------------------------------- use module in options list to set main window text module
	it = mainMgr->Modules.find(options[1]); 
	if (it != mainMgr->Modules.end()) 
	{
		curMod = (*it).second;	
	}

	//-------------------------------------------------- use module in options list to set 1st int text module
	it = mainMgr1->Modules.find(options[2]); 
	if (it != mainMgr1->Modules.end()) 
	{
		comp1Mod = (*it).second;	
	}
	//-------------------------------------------------- use module in options list to set 2nt int text module
	it = mainMgr2->Modules.find(options[3]); 
	if (it != mainMgr2->Modules.end()) 
	{
		comp2Mod = (*it).second;	
	}
	//--------------------------------------------------- use module in options list to set 3rd int text module
	it = mainMgr3->Modules.find(options[4]); 
	if (it != mainMgr3->Modules.end()) 
	{
		comp3Mod = (*it).second;	
	}

	//-------------------------------------------------------- Add options to Options Menu and get toggle item widget
	notepage  = additemtooptionmenu(MainFrm, "_Settings/", "Show Interlinear Page", (GtkMenuCallback)on_show_interlinear_page1_activate);
	versestyle = additemtooptionmenu(MainFrm, "_Settings/", "Verse Style", (GtkMenuCallback)on_verse_style1_activate);
	footnotes   = additemtooptionmenu(MainFrm, "_Settings/", "Show Footnotes", (GtkMenuCallback)on_footnotes1_activate);

	//-------------------------------------------------------------- attach popup menus
	gnome_popup_menu_attach(menu1,lookup_widget(mainform,"moduleText"),"1");
	gnome_popup_menu_attach(menu2,lookup_widget(mainform,"textComp1"),"1");
	gnome_popup_menu_attach(menu3,lookup_widget(mainform,"textComp2"),"1");
	gnome_popup_menu_attach(menu4,lookup_widget(mainform,"textComp3"),"1");
	gnome_popup_menu_attach(menu5,lookup_widget(mainform,"btnBookmarks"),"1");	

	loadbookmarks(MainFrm); //--------------------------------- add bookmarks to menubar
//    changeVerse(options[9]); //-------------------------------------------------- set Text
}


//-------------------------------------------------------------------------------------------
void
loadbookmarks(GtkWidget *MainFrm)
{
	gchar 	 subtreelabel[255];
	gint       i;	
		
	sprintf(subtreelabel, "%s","_Bookmarks/Edit Bookmarks");	//--- create label for menu subtree
	for(i=0;i<ibookmarks;i++)
	{		
		if(bmarks[i][0] == '[')  //--- if item starts with a '[' it is a subtree (submenu)
		{			
			addsubtreeitem(MainFrm, "_Bookmarks/", bmarks[i]);
			sprintf(subtreelabel, "%s/%s/","_Bookmarks" ,bmarks[i] );
		}
		else if(bmarks[i][0] == '<')  //--- if item starts with '<' remaining items are added to _Bookmarks menu
																	//---  below Edit Bookmarks item
		{
			sprintf(subtreelabel, "%s","_Bookmarks/Edit Bookmarks");			
		}
		else
		{
			additemtosubtree(MainFrm, subtreelabel, bmarks[i]); //--- add menu item to subtree
		}	
	}
}

//-------------------------------------------------------------------------------------------
void 
changeVerse(gchar *ref)
{
	string keyText;
	GtkWidget *msgBox;
	int l;

		strcpy(current_verse,ref);
		
		keyText = ref;		
		ApplyChange = FALSE;	
		if(curMod)  //--------------------------------------------------- change main window
		{
			curMod->SetKey(keyText.c_str());
			curMod->Display();
			gtk_label_set(GTK_LABEL(lookup_widget(MainFrm,"lbText")),curMod->KeyText( )); //------- set text label to current verse
			swKey = curMod->KeyText();
			strcpy(options[9],curMod->KeyText()); //----------------------- remember last verse
			char s1[255],s2[255];
			curVerse = swKey.Verse();
			curChapter = swKey.Chapter();		
			sprintf(s1,"%s",(const char *)swKey);
			for(l=0;l<strlen(s1);l++) //------------------------ seperate book name
			{
				if(isdigit(s1[l])) break;
			}
			strncpy(s2,s1,l);
			s2[l] = '\0';
			//---------------------------------------------------------- set entries to new verse
			gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook")),s2);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")),curChapter);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse")),curVerse);
			gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeFreeformLookup")),swKey);
		}
		//--------------------------------------------------------------- change interlinear verses
		if(comp1Mod)
		{
			comp1Mod->SetKey(swKey);
			comp1Mod->Display();
		}
		if(comp2Mod)
		{
			comp2Mod->SetKey(swKey);
			comp2Mod->Display();
		}		
		if(comp3Mod)
		{
			comp3Mod->SetKey(swKey);
			comp3Mod->Display();
		}
		//---------------------------------------------------------------- change commentary 
		if(curcomMod)
		{	
			if(!GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active)
			{
				curcomMod->SetKey(swKey);
				curcomMod->Display();	
			}	
		}	

	ApplyChange = TRUE;	
}

//-------------------------------------------------------------------------------------------
void 
FillDictKeys(char *ModName) 
{
	ModMap::iterator it;
	int index = 0;
	SWKey saveKey;
	string keyText;
	gchar *entryText;
	int i, j, count ;
	const gchar *listitem;
	GtkWidget *list;

	list = lookup_widget(MainFrm, "list1");
	entryText = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm, "dictionarySearchText")));
	j=0;
	keyText = entryText;
	it = mainMgr->Modules.find(ModName);
	if (it != mainMgr->Modules.end()) 	
	{
		SWModule *mod = (*it).second;
		mod->KeyText(); // snap to entry
		saveKey = mod->KeyText(); 
		count = GTK_CLIST(list)->clist_window_height / GTK_CLIST(list)->row_height;		
		gtk_clist_clear( GTK_CLIST(list));
		if(mod)
		{
			for (i = 0; i < (count / 2); i++) (*mod)++;
			for (i = 0; i < count-1; i++) (*mod)--;
			mod->Error();
			for (;!mod->Error() && count;count--,(*mod)++) 
			{
				++j;
				listitem = (const char *)mod->KeyText();
				gtk_clist_append(GTK_CLIST(list) , &listitem);
				if (saveKey == mod->Key())
				index = j-1; 
			}
		}
		gtk_clist_moveto( GTK_CLIST(list),
                            index,
                            0,
                            0.5,
                            0.0 );
	}
}

//-------------------------------------------------------------------------------------------
void 
setHistory(void)
{
	
	GtkWidget *menuChoice;	
	gchar   menuName[64];
/*
	menuChoice = gtk_menu_item_new_with_label (curMod->KeyText());
	sprintf(menuName, "%s",curMod->KeyText() );
	gtk_object_set_data (GTK_OBJECT (MainFrm), menuName, menuChoice);
	gtk_widget_show (menuChoice);
	gtk_signal_connect(GTK_OBJECT (menuChoice), "activate",
			      GTK_SIGNAL_FUNC (on_mnuHistoryitem1_activate),
			      g_strdup(curMod->KeyText() ));
	gtk_container_add (GTK_CONTAINER (lookup_widget(MainFrm,"history1_menu")), menuChoice);
*/
}

//-------------------------------------------------------------------------------------------
void 
UpdateChecks(GtkWidget *mainform)
{
	if(options[5][0] == 'T')	//--------------------- set verse style to last setting used
	{
		GTK_CHECK_MENU_ITEM (versestyle)->active = TRUE;
		bVerseStyle = TRUE;
	}
	if(options[5][0] == 'F')
	{
		GTK_CHECK_MENU_ITEM (versestyle)->active = FALSE;
		bVerseStyle = FALSE;
	}
	if(options[7][0] == 'T')  //----------------------- set footnotes to last setting used
	{
		GTK_CHECK_MENU_ITEM (footnotes)->active = TRUE;	
		mainMgr->setGlobalOption("Footnotes","On");			
	}	
	if(options[7][0] == 'F')
	{
		GTK_CHECK_MENU_ITEM (footnotes)->active = FALSE;
		mainMgr->setGlobalOption("Footnotes","Off");				
	}
	if(options[8][0] == 'T')	//---------------------------- set interlinear page to last setting
	{
		GTK_CHECK_MENU_ITEM (notepage)->active = TRUE;
		gtk_widget_show(lookup_widget(mainform,"vbox3"));	
	}
	if(options[8][0] == 'F')
	{
		GTK_CHECK_MENU_ITEM (notepage)->active = FALSE;
		gtk_widget_hide(lookup_widget(mainform,"vbox3"));		
	}
	if(options[6][0] == 'T')	//--------------------------- set Strong's numbers to last setting
	{	
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(mainform,"btnStrongs")), TRUE);
		mainMgr->setGlobalOption("Strong's Numbers","On");
	}
	if(options[6][0] == 'F')
	{
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(lookup_widget(mainform,"btnStrongs")), FALSE);
		mainMgr->setGlobalOption("Strong's Numbers","Off");
	}		
    changeVerse(options[9]); //--------------------------------------- set Text - apply changes
}

//-------------------------------------------------------------------------------------------
void 
ShutItDown(void)
{

	saveoptions();
	
	delete mainMgr;
	delete mainMgr1;
	delete mainMgr2;
	delete mainMgr3;
	delete searchMgr;

	if (chapDisplay)
		delete chapDisplay;
	if (entryDisplay)
		delete entryDisplay;	
	if(comp1Display)
		delete comp1Display;
	if(comp2Display)
		delete comp2Display;
	if(comDisplay)
		delete comDisplay;
	if(BTFcomDisplay)
		delete BTFcomDisplay;
	if(dictDisplay)
		delete dictDisplay;
	gtk_exit(0);
}


//-------------------------------------------------------------------------------------------
void
searchSWORD(GtkWidget *searchFrm)
{
	GtkWidget	*searchText,
					 *lbSearchHits,
					 *resultList,
					 *regexSearch,
					 *phraseSearch,
					 *caseSensitive,
					 *comToggle,
					 *bounds,
					 *lastsearch,
					 *textWindow;
    string srchText;
	gchar         *entryText,
			         scount[5];
	const gchar *resultText;
	gint count;



	searchText = lookup_widget(searchFrm,"entry1");
	lbSearchHits = lookup_widget(searchFrm,"lbSearchHits");
	resultList = lookup_widget(searchFrm,"resultList");
	regexSearch = lookup_widget(searchFrm,"regexSearch");
	phraseSearch = lookup_widget(searchFrm,"phraseSearch");
	caseSensitive = lookup_widget(searchFrm,"caseSensitive");
	bounds = lookup_widget(searchFrm,"rbUseBounds");
	lastsearch = lookup_widget(searchFrm,"rbLastSearch");
	comToggle = lookup_widget(searchFrm,"ckbCom");	
	textWindow= lookup_widget(searchFrm,"txtSearch");	
	
	/********************************************************************************************************** clear text window */	
	gtk_text_set_point(GTK_TEXT(textWindow), 0);
	gtk_text_forward_delete (GTK_TEXT (textWindow), gtk_text_get_length((GTK_TEXT(textWindow))));

	/********************************************************************************************************* search text or com */
	if(!GTK_TOGGLE_BUTTON(comToggle)->active)
		searchMod = curMod;
	else
		searchMod = curcomMod;
	
	if(GTK_TOGGLE_BUTTON(bounds)->active) //---------- check to see if we want to set bounds for search
	{
		GtkWidget  *lower, //-------- lower bounds entry widget
						*upper; //-------- upper bounds entry widget
		lower = lookup_widget(searchFrm,"entryLower"); //----------- get Lower bounds entry widget from search form
		upper = lookup_widget(searchFrm,"entryUpper"); //----------- get Upper bounds entry widget from search form	
		searchScopeLowUp.ClearBounds(); //-------- clear old bounds
		searchScopeLowUp.LowerBound(gtk_entry_get_text(GTK_ENTRY(lower))); //--read lower bounds entry and set lower bounds for search
		searchScopeLowUp.UpperBound(gtk_entry_get_text(GTK_ENTRY(upper))); //--read upper bounds entry and set upper bounds for search
		currentScope = &searchScopeLowUp; //------------- set scope of search to use bounds	
	}
	else if(GTK_TOGGLE_BUTTON(lastsearch)->active) //---------- check to see if we want to use results of search last for this search
	{
		currentScope = &searchScopeList;
	}
	else
	{
		searchScopeLowUp.ClearBounds();  //-------- clear old bounds
		searchScopeList.ClearList(); //------------ clear scope search list
		currentScope = 0; //------------ clear scope
	}

	count = 0;
	gtk_clist_clear(GTK_CLIST(resultList));	

	entryText = gtk_entry_get_text(GTK_ENTRY(searchText));
	srchText = entryText;
	gtk_label_set_text( GTK_LABEL(lbSearchHits) ,"0" );
	if (searchMod)
	{
		int searchType = GTK_TOGGLE_BUTTON(regexSearch)->active ? 0 : GTK_TOGGLE_BUTTON(phraseSearch)->active ? -1 : -2;
		int searchParams = GTK_TOGGLE_BUTTON(caseSensitive)->active ? 0 : REG_ICASE;
		gtk_clist_freeze(GTK_CLIST(resultList));
		for (ListKey &searchResults = searchMod->Search(srchText.c_str(), searchType,
									searchParams, currentScope); !searchResults.Error(); searchResults++)
		{
			resultText = (const char *)searchResults;
			gtk_clist_append(GTK_CLIST(resultList), &resultText);
			searchScopeList << (const char *)searchResults;
			++count;
		}
		gtk_clist_thaw(GTK_CLIST(resultList));
		sprintf(scount,"%d",count);
		gtk_label_set_text( GTK_LABEL(lbSearchHits) ,scount );
	}
}

//-------------------------------------------------------------------------------------------
void
resultsListSWORD(GtkWidget *searchFrm, gint row, gint column)
{
	GtkWidget	*resultList,
					 *textWindow,
					 *comToggle;
 	gchar 		  *text,
					 tmpBuf[255];
	SWModule *searchMod;
	ModMap::iterator it;	
	
	
	resultList = lookup_widget(searchFrm,"resultList");
	textWindow = lookup_widget(searchFrm,"txtSearch");
	comToggle = lookup_widget(searchFrm,"ckbCom");
	
    gtk_clist_get_text(GTK_CLIST(resultList), row, column, &text);

    if(!GTK_TOGGLE_BUTTON(comToggle)->active)
	{
		it = searchMgr->Modules.find(curMod->Name()); 	
		if (it != searchMgr->Modules.end()) 
		{
			searchMod = (*it).second;
		}			
	}
	else
	{
		it = searchMgr->Modules.find(curcomMod->Name()); 	
		if (it != searchMgr->Modules.end()) 
		{
			searchMod = (*it).second;
		}			
	}		
	
	if(searchMod)
	{
		searchMod->SetKey(text);
		searchMod->Display();
	}
}

//-------------------------------------------------------------------------------------------
void
setupSearchDlg(GtkWidget *searchDlg)
{
		ModMap::iterator it;
		gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(searchDlg,"txtSearch")) , TRUE );
		BTFsearchDisplay = new GTKInterlinearDisp(lookup_widget(searchDlg,"txtSearch"));
    	//--------------------------------------------------------------------------------------- searchmodule	
		for (it = searchMgr->Modules.begin(); it != searchMgr->Modules.end(); it++)
		{
			searchMod  = (*it).second;
			searchMod->Disp(BTFsearchDisplay);
		}
}

//-------------------------------------------------------------------------------------------
void
strongsSWORD(bool choice)
{
	if(choice)
	{
		mainMgr->setGlobalOption("Strong's Numbers","On");
		strcpy(options[6],"TRUE");
	}
	else
	{
		mainMgr->setGlobalOption("Strong's Numbers","Off");		
		strcpy(options[6],"FALSE");		
	}
	curMod->Display();
}

//-------------------------------------------------------------------------------------------
void
footnotesSWORD(bool choice)
{
	if(choice)
	{
		mainMgr->setGlobalOption("Footnotes","On");
		strcpy(options[7],"TRUE");
	}
	else
	{
		mainMgr->setGlobalOption("Footnotes","Off");		
		strcpy(options[7],"FALSE");		
	}
	curMod->Display();
}

//-------------------------------------------------------------------------------------------
void
addBookmark(void)
{
	GnomeUIInfo *bookmarkitem;
	gchar    *bookname,
				ref[255];
	gint       iVerse,
				iChap,
				i,j;
	FILE *flbookmarks;

	if(ibookmarks < 49)
	{
		bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook")));
		iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse")));
		iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")));
		sprintf(bmarks[ibookmarks],"%s %d:%d\0",bookname, iChap,iVerse );
		bookmarkitem = g_new(GnomeUIInfo,2);
		bookmarkitem->type = GNOME_APP_UI_ITEM;
		bookmarkitem->moreinfo=(gpointer)on_john_3_1_activate;
		bookmarkitem->user_data=g_strdup(bmarks[ibookmarks]);
		bookmarkitem->label = bmarks[ibookmarks];
		bookmarkitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
		bookmarkitem->pixmap_info =GNOME_STOCK_MENU_BOOK_OPEN;
		bookmarkitem->accelerator_key = 0;
		bookmarkitem[1].type=GNOME_APP_UI_ENDOFINFO;
		gnome_app_insert_menus_with_data(GNOME_APP(MainFrm),"_Bookmarks/",bookmarkitem,NULL);	
		 ++ibookmarks;
	}
	savebookmarks();
}

//-------------------------------------------------------------------------------------------
void
editbookmarksLoad(GtkWidget *editdlg)
{
	GtkWidget *text;
	gchar buf[255];
	gint i=0;

	text = lookup_widget(editdlg,"text4");
 	gtk_text_freeze (GTK_TEXT (text));
  	gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
	while(i < ibookmarks)
	{ 	
		sprintf(buf,"%s\n",bmarks[i]);
		gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL,buf , -1);
		++i;	     	
	}
	sprintf(buf,"%s\n","-end-");
	gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL,buf , -1);
	gtk_text_thaw (GTK_TEXT (text));
}

//-------------------------------------------------------------------------------------------
void
addHistoryItem(void)
{
	GnomeUIInfo *historyitem;
	gchar    *bookname,
				ref[255];
	gint       iVerse,
				iChap;

	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook")));
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse")));
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")));
	sprintf(ref,"%s %d:%d",bookname, iChap,iVerse );
	historyitem = g_new(GnomeUIInfo,2);
	historyitem->type = GNOME_APP_UI_ITEM;
	historyitem->moreinfo=(gpointer)on_john_3_1_activate;
	historyitem->user_data=g_strdup(ref);
	historyitem->label = ref;
	historyitem->pixmap_type = GNOME_APP_PIXMAP_STOCK;
	historyitem->pixmap_info =GNOME_STOCK_MENU_BOOK_OPEN;
	historyitem->accelerator_key = 0;
	historyitem[1].type=GNOME_APP_UI_ENDOFINFO;
	gnome_app_insert_menus_with_data(GNOME_APP(MainFrm),"_History/C_lear",historyitem,NULL);	
}
//-------------------------------------------------------------------------------------------
void
changecurModSWORD(gchar *modName)
{
	ModMap::iterator it;

	it = mainMgr->Modules.find(modName);
	if (it != mainMgr->Modules.end())
	{
		curMod = (*it).second;
		curMod->SetKey(current_verse);
		curMod->Display();
	}
	strcpy(options[1],curMod->Name());
	gtk_frame_set_label( GTK_FRAME(lookup_widget(MainFrm,"frame9")),
                                 curMod->Name());
}

//-------------------------------------------------------------------------------------------
void
changecomp1ModSWORD(gchar *modName)
{
	ModMap::iterator it;

	it = mainMgr1->Modules.find(modName);
	if (it != mainMgr1->Modules.end())
	{
		comp1Mod = (*it).second;
		comp1Mod->SetKey(current_verse);
		comp1Mod->Display();
	}
	strcpy(options[2],comp1Mod->Name());
}

//-------------------------------------------------------------------------------------------
void
changecomp2ModSWORD(gchar *modName)
{
	ModMap::iterator it;

	it = mainMgr2->Modules.find(modName);
	if (it != mainMgr2->Modules.end())
	{
		comp2Mod = (*it).second;
		comp2Mod->SetKey(current_verse);
		comp2Mod->Display();
	}
	strcpy(options[3],comp2Mod->Name());
}

//-------------------------------------------------------------------------------------------
void
changecomp3ModSWORD(gchar *modName)
{
	ModMap::iterator it;

	it = mainMgr3->Modules.find(modName);
	if (it != mainMgr3->Modules.end())
	{
		comp3Mod = (*it).second;
		comp3Mod->SetKey(current_verse);
		comp3Mod->Display();
	}
	strcpy(options[4],comp3Mod->Name());
}

//-------------------------------------------------------------------------------------------
void
setversestyleSWORD(bool choice)
{
	if(choice)
	{
		strcpy(options[5],"TRUE");
		bVerseStyle = TRUE;
	}
	else
	{
		strcpy(options[5],"FALSE");
		bVerseStyle = FALSE;
	}
 	curMod->Display();
}

//-------------------------------------------------------------------------------------------
void
showIntPage(bool choice)
{
	GtkWidget *intpage;

	intpage= lookup_widget(MainFrm,"vbox3");
	if(choice)
	{
		strcpy(options[8],"TRUE");
		gtk_widget_show(intpage);		
	}
	else
	{
		strcpy(options[8],"FALSE");
		gtk_widget_hide(intpage);		
	}
}


//-------------------------------------------------------------------------------------------
void
chapterSWORD(void)
{
	gint       iChap;

	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")));
	swKey.Chapter(iChap);
	changeVerse(swKey);	
}

//-------------------------------------------------------------------------------------------
void
verseSWORD(void)
{
	gint       iVerse;

	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse")));
	swKey.Verse(iVerse );
	changeVerse(swKey);		
}

//-------------------------------------------------------------------------------------------
void
btnlookupSWORD(void)
{
	gchar    *bookname,
				ref[255];
	gint       iVerse,
				iChap;

	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook")));
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")));
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse")));
	sprintf(ref,"%s %d:%d",bookname, iChap,iVerse );
	changeVerse(ref);	
	addHistoryItem();
}

//-------------------------------------------------------------------------------------------
void
freeformlookupSWORD(GdkEventKey  *event)
{
 	gchar *buf;  

	ApplyChange = FALSE;	
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeFreeformLookup")));	
	if(event->keyval == 65293 || event->keyval == 65421)
	{
		changeVerse(buf);	
		addHistoryItem();
	}	
}

//-------------------------------------------------------------------------------------------
void
changcurcomModSWORD(gchar *modName)
{
	ModMap::iterator it;

	it = mainMgr->Modules.find(modName);
	if (it != mainMgr->Modules.end()) 
	{
		curcomMod = (*it).second;
		curcomMod->SetKey(curMod->KeyText());
		curcomMod->Display();
	}	
}

//-------------------------------------------------------------------------------------------
void
savenoteSWORD(bool noteModified)
{
		VerseKey mykey;
		gchar *buf;
		GtkWidget *text;
		text = lookup_widget(MainFrm,"textComments");
		buf = strrchr(gtk_editable_get_chars((GtkEditable *)text,0,-1),']');
		buf = strrchr(gtk_editable_get_chars((GtkEditable *)text,0,-1),']');
		buf = strchr(buf,' ');
		mykey = curcomMod->KeyText();
		mykey.Persist(1);
		curcomMod->SetKey(mykey);
		if(noteModified) *curcomMod << (const char *)buf;
}

//-------------------------------------------------------------------------------------------
void
deletenoteSWORD(void)
{
		VerseKey mykey;
		gchar *buf;
		
		gtk_text_set_point(GTK_TEXT(lookup_widget(MainFrm,"textComments")), 0);											gtk_text_forward_delete(GTK_TEXT(lookup_widget(MainFrm,"textComments")), gtk_text_get_length(GTK_TEXT(lookup_widget(MainFrm,"textComments"))));
		mykey = curcomMod->KeyText();
		mykey.Persist(1);
		curcomMod->SetKey(mykey);
		curcomMod->Delete();
		changeVerse(mykey);
}

//-------------------------------------------------------------------------------------------
void
changcurdictModSWORD(gchar *modName, string keyText)
{	
	ModMap::iterator it;

	it = mainMgr->Modules.find(modName);
	if (it != mainMgr->Modules.end()) 
	{
		curdictMod = (*it).second;
		curdictMod->SetKey(keyText.c_str());
		curdictMod->Display();	
		FillDictKeys(curdictMod->Name()); 	
	}
}

//-------------------------------------------------------------------------------------------
void
dictSearchTextChangedSWORD(char* mytext)
{	
	string keyText;

	keyText = mytext;
	if (curdictMod)
	{
		if(strcmp(mytext,""))
		{
			curdictMod->SetKey(keyText.c_str());
			curdictMod->Display();	
			FillDictKeys(curdictMod->Name());
		}
	}
}