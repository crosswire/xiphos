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
#include <gbfplain.h>
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
#include "dialogs.h"
#include "listeditor.h"

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
SWDisplay *percomDisplay; //--- to display personal comment modules using GtkText a verse at a time
SWDisplay *dictDisplay; //--- to display modules using GtkText a verse at a time
SWDisplay *GBFcomDisplay; //--- to display modules using GtkText a verse at a time
SWDisplay *GBFsearchDisplay; //--- to display modules using GtkText a verse at a time

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
SWModule *percomMod; //-- module for personal commentary  window	
SWModule *curdictMod; //-- module for dict window
SWModule *searchMod; //-- module for searching and search window


VerseKey searchScopeLowUp; //----------- sets lower and upper search bounds
ListKey	searchScopeList; //----------- search list for searching verses found on last search
SWKey	*currentScope; //----------- use to set scope of search



//---------------------------------------------------------------- GnomeSword global to this file

SETTINGS *settings;
SETTINGS myset;	
GtkWidget *MainFrm,	//-- main form widget
					*bookmark_mnu; //-- popup menu for bookmarks

gchar *current_filename= NULL;	//-- filename for open file in study pad window 
gchar current_verse[45]="Romans 8:28";	//-- current verse showing in main window - 1st - 2nd - 3rd interlinear window - commentary window
bool ApplyChange = TRUE;	//-- should we make changes when cbBook is changed
bool bVerseStyle = TRUE;	//-- should we show verses of paragraphs in main text window

VerseKey swKey = "Romans 8:28";	//-- ?? 
gint curChapter = 8;	//-- keep up with current chapter
gint curVerse =28;	//-- keep up with current verse


gboolean file_changed = false;	//-- set to true if text is study pad has changed - and file is not saved
gchar //options[16][80],	//-- array to store a number of setting - read in form file when program starts - saved to file at normal shut down
		 bmarks[50][80];	//-- array to store bookmarks - read in form file when program starts - saved to file after edit
GtkWidget 	*versestyle,	//-- widget to access toggle menu - for versestyle
						*footnotes,	//-- widget to access toggle menu - for footnotes
						*notepage,	//-- widget to access toggle menu - for interlinear notebook page
						*autosaveitem; //-- widget to access toggle menu - for personal comments auto save
			
extern gint ibookmarks;	//-- number of items in bookmark menu  -- declared in filestuff.cpp
extern GdkColor myGreen; //-- current verse color
GtkWidget* studypad;  //-- studypad text widget
GtkWidget* notes;    //-- notes text widget
bool noteModified = false; //-- set to true is personal note has changed
bool waitonmessage = true; //-- wait for user input
bool usepersonalcomments = false; //-- do we setup for personal comments - default is false
																	//-- will change to true if we find any personal modules																																									
//gboolean saveChanges = true; //-- save changes to personal comments
gboolean autoSave = true; //-- we want to auto save changes to personal comments
//gboolean personalCom = true; //-- let us know if curcomMod is a personal comment mod
extern gchar rememberlastitem[255]; //-- used for bookmark menus declared in filestuff.cpp
extern gchar remembersubtree[256];  //-- used for bookmark menus declared in filestuff.cpp
gint historyitems = 0;
gint answer;
//----------------------------------------------------------------------------------------------
void
initSword(GtkWidget *mainform,  //-- app's main form
				GtkWidget *menu1,  //-- main text window's popup menu
				GtkWidget *menu2,  //-- 1st inetrlinear window's popup menu
				GtkWidget *menu3,  //-- 2nd inetrlinear window's popup menu
				GtkWidget *menu4,  //-- 3rd inetrlinear window's popup menu
				GtkWidget *menu5) //-- edit note wintow popup menu
{
	ModMap::iterator it; //-- iteratior
	SectionMap::iterator sit; //-- iteratior
	ConfigEntMap::iterator eit; //-- iteratior
	int pg=0, //-- notebook page number - for creating notebook pages
			pg1=0, //-- notebook page number - for creating notebook pages
	    pg2=0; //-- notebook page numger
	GtkWidget  *empty_notebook_page, //-- used to create new pages
					*notebook, //-- to access notebooks for adding pages
					*menuChoice, //-- gtk menu item
					*popmenu, //-- to access popup menus for adding items
					*menuChoice1, //-- gtk menu item
					*label, //-- labels for adding notebook pages
					*menu; //-- ??
				//iBibletext=0; //--
	char          menuName[64], //--  for menu item label
					menuName1[64], //--  for menu item label
					mybuf[80], //-- ??
					rememberlastitem[80], //--  use to store last menu item so we can add the next item under it - gnome menus
					aboutrememberlastitem[80], //--  use to store last menu item so we can add the next item under it - gnome menus
					aboutrememberlastitem2[80], //--  use to store last menu item so we can add the next item under it - gnome menus
					aboutrememberlastitem3[80]; //--  use to store last menu item so we can add the next item under it - gnome menus
	int             viewNumber = 1, //--  for numbering menu items
					viewNumber1= 1, //-- for numbering menu items
					itemNum = 0, //-- for numbering menu items
					i, //-- counter
					j; //-- counter

	GnomeUIInfo *menuitem; //--  gnome menuitem

  myset = readsettings();  //-- load settings into structure
  settings = &myset;       //-- set pointer to structure


	mainMgr         = new SWMgr();	//-- create sword mgrs
	mainMgr1        = new SWMgr();
	mainMgr2        = new SWMgr();
	mainMgr3        = new SWMgr();
	searchMgr				= new SWMgr();

	
	curMod           = NULL; //-- set mods to null
	comp1Mod      = NULL;
	comp2Mod      = NULL;
	comp3Mod      = NULL;
	curcomMod     = NULL;
	curdictMod    = NULL;
	searchMod     = NULL;
	percomMod     = NULL;

	
	chapDisplay    = 0;// set in create
	entryDisplay    = 0;// set in create
	comp1Display    = 0;// set in create
	comp2Display    = 0;// set in create
	comp3Display    = 0;// set in create
	comDisplay   	  = 0;// set in create
	dictDisplay   	   = 0;// set in create
	GBFcomDisplay = 0; // set in create
	GBFsearchDisplay =0; // set in create
  percomDisplay   	  = 0;// set in create


//	gtk_rc_parse( "gsword.rc" );

  myGreen.red =  settings->currentverse_red; //-- set color for current verse
	myGreen.green = settings->currentverse_green;
	myGreen.blue =  settings->currentverse_blue;
	
	MainFrm = lookup_widget(mainform,"mainwindow"); //-- save mainform for use latter

	//--------------------------------------------------------------------- setup displays for sword modules
	GTKEntryDisp::__initialize();
	chapDisplay = new GTKChapDisp(lookup_widget(mainform,"moduleText"));
	dictDisplay = new GTKEntryDisp(lookup_widget(mainform,"textDict"));
	comDisplay = new  GTKEntryDisp(lookup_widget(mainform,"textCommentaries"));
	percomDisplay = new  GTKEntryDisp(lookup_widget(mainform,"textComments"));
	GBFcomDisplay = new  GTKInterlinearDisp(lookup_widget(mainform,"textCommentaries"));
	comp1Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp1"));
	comp2Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp2"));
	comp3Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp3"));

//----------------------------------------------------------------------- set text windows to word warp
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"moduleText")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp1")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp2")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp3")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textDict")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textCommentaries")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComments")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"text3")) , TRUE );
  //------------------------------------------------------------------ store text widgets for spell checker
  notes =  lookup_widget(mainform,"textComments");
  studypad = lookup_widget(mainform,"text3");
	//-------------------------------------------------------------- set main window modules and add to menus	
	sprintf(rememberlastitem,"%s","_View/Main Window/");
	sprintf(aboutrememberlastitem,"%s","_Help/About Sword Modules/Bible Texts/<Separator>");
	sprintf(aboutrememberlastitem2,"%s","_Help/About Sword Modules/Commentaries/<Separator>");
	sprintf(aboutrememberlastitem3,"%s","_Help/About Sword Modules/Dictionaries-Lexicons/<Separator>");
	
	for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++)
	{

		if (!strcmp((*it).second->Type(), "Biblical Texts"))
		{
			curMod = (*it).second;
			 //------------------------------------------------------------------ add to menubar
			additemtognomemenu(MainFrm, curMod->Name(), rememberlastitem , (GtkMenuCallback)on_mainText_activate );
			additemtognomemenu(MainFrm, curMod->Name(), aboutrememberlastitem , (GtkMenuCallback)on_kjv1_activate );
			//--------------------------------------- remember last item - so next item will be place below it       	
			sprintf(rememberlastitem,"%s%s","_View/Main Window/",curMod->Name());	
			sprintf(aboutrememberlastitem,"%s%s","_Help/About Sword Modules/Bible Texts/",curMod->Name());			
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
				if((*mainMgr->config->Sections[(*it).second->Name()].find("ModDrv")).second == "RawFiles")
				{
				 	percomMod = (*it).second;
				 	percomMod->Disp(percomDisplay);	
				 	additemtognomemenu(MainFrm,percomMod->Name(), aboutrememberlastitem2 , (GtkMenuCallback)on_kjv1_activate );
				 	sprintf(aboutrememberlastitem2,"%s%s","_Help/About Sword Modules/Commentaries/",percomMod->Name());	
				 	additemtopopupmenu(MainFrm, menu5, percomMod->Name(), (GtkMenuCallback)on_change_module_activate);
				 	usepersonalcomments = true; //-- this does nothing now
				 	gtk_widget_show(lookup_widget(MainFrm,"vbox2")); //-- show personal comments page because we
				} 				                                         //-- have at least one personl module
				else
				{
					curcomMod = (*it).second;
					notebook = lookup_widget(mainform,"notebook1");
					empty_notebook_page = gtk_vbox_new (FALSE, 0);
  				gtk_widget_show (empty_notebook_page);
  				gtk_container_add (GTK_CONTAINER (notebook), empty_notebook_page);
					label = gtk_label_new (curcomMod->Name());
			    gtk_widget_show (label);
					gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), pg1++), label);
				  additemtognomemenu(MainFrm,curcomMod->Name(), aboutrememberlastitem2 , (GtkMenuCallback)on_kjv1_activate );
				 	sprintf(aboutrememberlastitem2,"%s%s","_Help/About Sword Modules/Commentaries/",curcomMod->Name());	
					if(!strcmp(curcomMod->Name(),"Family"))
						curcomMod->Disp(GBFcomDisplay);
					else
						curcomMod->Disp(comDisplay);
				}
			}	
			//------------------------------------------------------------------ set dictionary modules and add to notebook
			if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries"))
			{	
				curdictMod = (*it).second;
				notebook = lookup_widget(mainform,"notebook4");
				additemtognomemenu(MainFrm,curdictMod->Name(), aboutrememberlastitem3 , (GtkMenuCallback)on_kjv1_activate );
				sprintf(aboutrememberlastitem3,"%s%s","_Help/About Sword Modules/Dictionaries-Lexicons/",curdictMod->Name());	
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
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook),settings->notebook2page );	
	label1 = (GtkLabel *)gtk_notebook_get_tab_label (GTK_NOTEBOOK(notebook),
						gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),settings->notebook2page));
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
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook),settings->notebook1page );
	label1 = (GtkLabel *)gtk_notebook_get_tab_label (GTK_NOTEBOOK(notebook),
						gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),settings->notebook1page));
	it = mainMgr->Modules.find((char *)label1->label); 
	if (it != mainMgr->Modules.end()) 
	{
		curcomMod = (*it).second;	
	}	
	gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                      GTK_SIGNAL_FUNC (on_notebook1_switch_page),
                      NULL);
	//---------------------------------------------------------------- set main notebook page
	notebook = lookup_widget(mainform,"notebook3");	
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook),settings->notebook3page );
		
	//---------------------------------------------------------------- set personal commets notebook label and display module
	if(usepersonalcomments)
	{
		it = mainMgr->Modules.find(settings->personalcommentsmod); //-- set percomMod to last used
		if (it != mainMgr->Modules.end())
		{
			percomMod = (*it).second;			
		}	
		if(percomMod) //-- if we found the module set up page and show the world
		{	
			notebook = lookup_widget(mainform,"notebook3");
			label = gtk_label_new (percomMod->Name());
			gtk_widget_show (label);
			gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label);
			percomMod->Display();
		}
	}
	//--------------------------------------------- use module in options list to set main window text module
	it = mainMgr->Modules.find(settings->MainWindowModule);
	if (it != mainMgr->Modules.end()) 
	{
		curMod = (*it).second;
		gtk_frame_set_label( GTK_FRAME(lookup_widget(MainFrm,"frame9")),curMod->Name()); //-- set main text window	
	}                                                                                  //-- label to modName

	//-------------------------------------------------- use module in options list to set 1st int text module
	it = mainMgr1->Modules.find(settings->Interlinear1Module);
	if (it != mainMgr1->Modules.end()) 
	{
		comp1Mod = (*it).second;	
	}
	//-------------------------------------------------- use module in options list to set 2nt int text module
	it = mainMgr2->Modules.find(settings->Interlinear2Module);
	if (it != mainMgr2->Modules.end()) 
	{
		comp2Mod = (*it).second;	
	}
	//--------------------------------------------------- use module in options list to set 3rd int text module
	it = mainMgr3->Modules.find(settings->Interlinear3Module);
	if (it != mainMgr3->Modules.end()) 
	{
		comp3Mod = (*it).second;	
	}

	//-------------------------------------------------------- Add options to Options Menu and get toggle item widget
	autosaveitem = additemtooptionmenu(MainFrm, "_Settings/", "Auto Save Personal Comments", (GtkMenuCallback)on_auto_save_notes1_activate);
	notepage  = additemtooptionmenu(MainFrm, "_Settings/", "Show Interlinear Page", (GtkMenuCallback)on_show_interlinear_page1_activate);
	versestyle = additemtooptionmenu(MainFrm, "_Settings/", "Verse Style", (GtkMenuCallback)on_verse_style1_activate);
	footnotes   = additemtooptionmenu(MainFrm, "_Settings/", "Show Footnotes", (GtkMenuCallback)on_footnotes1_activate);
 	
	//-------------------------------------------------------------- attach popup menus
	gnome_popup_menu_attach(menu1,lookup_widget(mainform,"moduleText"),"1");
	gnome_popup_menu_attach(menu2,lookup_widget(mainform,"textComp1"),"1");
	gnome_popup_menu_attach(menu3,lookup_widget(mainform,"textComp2"),"1");
	gnome_popup_menu_attach(menu4,lookup_widget(mainform,"textComp3"),"1");
	gnome_popup_menu_attach(menu5,lookup_widget(mainform,"textComments"),"1");	
  //----------------------------------------------------------------------- set dictionary key
  gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"dictionarySearchText")),settings->dictkey);

  //loadbookmarks(MainFrm); //--------------------------------- add bookmarks to menubar
  loadbookmarks_programstart(); //--------------------------------- add bookmarks to menubar
  changeVerse(settings->currentverse); //---------------------------------------------- set Text

  if(settings->studypadfilename != NULL) loadStudyPadFile(settings->studypadfilename); //-- load last used file into studypad

//-- hide buttons - only show them if their options are enabled
	gtk_widget_hide(lookup_widget(MainFrm,"btnPrint"));
	gtk_widget_hide(lookup_widget(MainFrm,"btnSpell"));
	gtk_widget_hide(lookup_widget(MainFrm,"btnSpellNotes"));
		
#ifdef  USE_GNOMEPRINT                   //-- don't show print button if printing not enabled
  gtk_widget_show(lookup_widget(MainFrm,"btnPrint"));
#endif /* USE_GNOMEPRINT */

#ifdef USE_ASPELL                         //-- don't show spell buttons if spellcheck not enabled
  gtk_widget_show (lookup_widget(MainFrm,"btnSpell"));
  gtk_widget_show (lookup_widget(MainFrm,"btnSpellNotes"));
#endif /* USE_ASPELL */
}

//-------------------------------------------------------------------------------------------
void 
changeVerse(gchar *ref) //-- change main text, interlinear texts and commentary text together
{
	string keyText; //-- string for verse key text to change to
//	GtkWidget *msgBox;
	int l;


		if((GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active) && noteModified) //-- save any changes to personal notes
		{
			if(autoSave)
			{                                                              //-- if we are in edit mode
				savenoteSWORD(noteModified); 	//-- save if text in note window has changed			
			}
		}
		
		strcpy(current_verse,ref);		
		keyText = ref;		
		ApplyChange = FALSE;			
		
		if(curMod)  //--------------------------------------------------- change main window
		{
			curMod->SetKey(keyText.c_str());
			curMod->Display();
			gtk_label_set(GTK_LABEL(lookup_widget(MainFrm,"lbText")),curMod->KeyText( )); //------- set text label to current verse
			swKey = curMod->KeyText();
			strcpy(settings->currentverse, curMod->KeyText()); //----------------------- remember last verse
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
			//------------------------- set book, chapter,verse and freeform lookup entries to new verse
			gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook")),s2);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")),curChapter);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse")),curVerse);
			gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeFreeformLookup")),swKey);
		}
		//--------------------------------------------------------------- change interlinear verses
		if(comp1Mod)
		{
			comp1Mod->SetKey(swKey);   //-- interlinear1
			comp1Mod->Display();
		}
		if(comp2Mod)
		{
			comp2Mod->SetKey(swKey);  //-- interlinear2
			comp2Mod->Display();
		}		
		if(comp3Mod)
		{
			comp3Mod->SetKey(swKey);  //-- interlinear3
			comp3Mod->Display();
		}
		//---------------------------------------------------------------- change commentary
		if((GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active) && (!autoSave))
		{
		  //-- do nothing
		}
		else
		{
		 	if(usepersonalcomments && percomMod)
		 	{
		 	  percomMod->SetKey(keyText.c_str()); //-- set personal module to current verse
				percomMod->Display();            //-- show change
				noteModified = false; //-- we just loaded comment so it is not modified	
		 	}
		}
		if(curcomMod)
		{	
			curcomMod->SetKey(keyText.c_str()); //-- set comments module to current verse
			curcomMod->Display(); //-- show change
		}  		
	ApplyChange = TRUE;	
}

//-------------------------------------------------------------------------------------------
void 
FillDictKeys(char *ModName)  //-- fill clist with dictionary keys -
{                            //-- number of keys depends on hight of list widget and size of font
	ModMap::iterator it; //-- iterator to go through modules and find modName
	int index = 0;      //-- for index into list widget
	SWKey saveKey;      //-- for starting point
	string keyText;     //-- string to search for
	gchar *entryText;   //-- pointer to string to search for
	int i, j, count ;   //-- counters
	const gchar *listitem; //-- hold item until added to list
	GtkWidget *list;     //-- list widget to display items found

	list = lookup_widget(MainFrm, "list1"); //-- get pointer to list widget
	entryText = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm, "dictionarySearchText"))); //-- get key to form user
	j=0;
	keyText = entryText; //-- put key in string
	it = mainMgr->Modules.find(ModName); //-- find module to use for search
	if (it != mainMgr->Modules.end()) //-- if we don't reach the end of our modules 	
	{
		SWModule *mod = (*it).second;  //-- temp module to work with
		mod->KeyText(); //-- snap to entry
		saveKey = mod->KeyText(); //-- save our place
		count = GTK_CLIST(list)->clist_window_height / GTK_CLIST(list)->row_height;	//-- how many items do we need to fill our list	
		gtk_clist_clear( GTK_CLIST(list)); //-- start with empty list
		if(mod) //-- make sure we have module to work with
		{
			for (i = 0; i < (count / 2); i++) (*mod)++; //-- get equal number of keys before and after our starting key(saveKey)
			for (i = 0; i < count-1; i++) (*mod)--;
			mod->Error(); //-- clear errors
			for (;!mod->Error() && count;count--,(*mod)++) 
			{
				++j;
				listitem = (const char *)mod->KeyText(); //-- key to listitem
				gtk_clist_append(GTK_CLIST(list) , &listitem); //-- listitem to list
				if (saveKey == mod->Key()) //-- if we are back to starting place set index
				index = j-1; 
			}
		}
		gtk_clist_moveto( GTK_CLIST(list),  //-- move in list to index
                            index,
                            0,
                            0.5,
                            0.0 );
		mod->SetKey(saveKey);
	}
}

//-------------------------------------------------------------------------------------------
void 
UpdateChecks(GtkWidget *mainform) //-- update chech menu items and toggle buttons
{                                 //-- called on start up
	
	if(settings->versestyle)	//--------------------- set verse style to last setting used
		bVerseStyle = TRUE; //-- keep VerseStyle in sync with menu
	else
		bVerseStyle = FALSE; //-- keep VerseStyle in sync with menu
	GTK_CHECK_MENU_ITEM (versestyle)->active = settings->versestyle;
	
	
	if(settings->footnotes)  //----------------------- set footnotes to last setting used
		mainMgr->setGlobalOption("Footnotes","On");	//-- keep footnotes in sync with menu		
	else
		mainMgr->setGlobalOption("Footnotes","Off");	//-- keep footnotes in sync with menu			
	GTK_CHECK_MENU_ITEM (footnotes)->active = settings->footnotes;
	
	
	if(settings->interlinearpage)	//---------------------------- set interlinear page to last setting
		gtk_widget_show(lookup_widget(mainform,"vbox3"));	
	else
		gtk_widget_hide(lookup_widget(mainform,"vbox3"));		
	GTK_CHECK_MENU_ITEM (notepage)->active = settings->interlinearpage; //-- set interlinear page menu check item
	
	
	autoSave = settings->autosavepersonalcomments; //-- set auto save personal comments to last setting	
	GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings->autosavepersonalcomments; //-- set auto save menu check item
	
	
	if(settings->strongs)	//--------------------------- set Strong's numbers to last setting
		mainMgr->setGlobalOption("Strong's Numbers","On"); //-- keep strongs numbers in sync with menu	
	else
		mainMgr->setGlobalOption("Strong's Numbers","Off"); //-- keep strongs numbers in sync with menu
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(mainform,"btnStrongs")), settings->strongs);	//-- set strongs toogle button
	
  FillDictKeys(curdictMod->Name()); //-- fill the dict key clist
  changeVerse(settings->currentverse); //--------------------------------------- set Text - apply changes
}

//-------------------------------------------------------------------------------------------
void 
ShutItDown(void)  //------------- close down GnomeSword program
{
  char *msg;
  GtkWidget *msgbox;


  sprintf(settings->studypadfilename,"%s",current_filename); //-- store studypad filename
	writesettings(myset); //-- save setting (myset structure) to use when we start back up
	if(file_changed)
	{
		
		msg = g_strdup_printf(_("``%s'' has been modified.  Do you wish to save it?"), current_filename);
		msgbox = create_InfoBox(); /*gnome_message_box_new(msg,
						GNOME_MESSAGE_BOX_QUESTION,
						GNOME_STOCK_BUTTON_YES,
						GNOME_STOCK_BUTTON_NO,						
						NULL); */
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
		g_free (msg);
		switch (answer)
		{
		case 0:			
					saveFile(current_filename);
					break;
		default:
					break;
		}
	}   	
	
	delete mainMgr;   //-- delete Sword managers
	delete mainMgr1;
	delete mainMgr2;
	delete mainMgr3;
	delete searchMgr;

	if (chapDisplay)    //-- delete Sword displays
		delete chapDisplay;
	if (entryDisplay)
		delete entryDisplay;	
	if(comp1Display)
		delete comp1Display;
	if(comp2Display)
		delete comp2Display;
	if(comDisplay)
		delete comDisplay;
	if(GBFcomDisplay)
		delete GBFcomDisplay;
	if(dictDisplay)
		delete dictDisplay;
	if(GBFsearchDisplay)
		delete GBFsearchDisplay;
	if(percomDisplay)
		delete percomDisplay;
	gtk_exit(0);           //-- exit
	
}


//-------------------------------------------------------------------------------------------
void
searchSWORD(GtkWidget *searchFrm)  //-- search Bible text or commentaries
{
	GtkWidget	*searchText,   //-- what we want to find -text entry
					 	*lbSearchHits,  //-- label for showing how many verses we found
					 	*resultList,    //-- list of verses found
					 	*regexSearch,   //-- do we want to use regular expression search - radio button
					 	*phraseSearch,  //-- do we want to use phrase seach - radio button
					 	*caseSensitive, //-- do we want search to be case sensitive - check box
					 	*comToggle,     //-- do we want to search current commentary - check box
					 	*bounds,        //-- do we want to use bounds for our search - check box
					 	*lastsearch,    //-- use verses from last search for bounds of this search
					 	*textWindow,    //-- text widget to display verses
					 	*percomToggle;  //-- do we want to search personal commentary - check box
	string 		srchText;       //-- string to search for - from entryText
	gchar     *entryText,//-- pointer to text in searchText entry
			      scount[5];  //-- string from gint count for label
	const gchar *resultText; //-- temp storage for verse found
	gint count;              //-- number of hits

	searchText = lookup_widget(searchFrm,"entry1"); //-- pointer to text entry
	lbSearchHits = lookup_widget(searchFrm,"lbSearchHits");//-- pointer to count label
	resultList = lookup_widget(searchFrm,"resultList");   //-- pointer to list
	regexSearch = lookup_widget(searchFrm,"regexSearch"); //-- pointer to radio button
	phraseSearch = lookup_widget(searchFrm,"phraseSearch");//-- pointer to radio button
	caseSensitive = lookup_widget(searchFrm,"caseSensitive");//-- pointer to check box
	bounds = lookup_widget(searchFrm,"rbUseBounds"); //-- pointer to check box
	lastsearch = lookup_widget(searchFrm,"rbLastSearch"); //-- pointer to radio button
	comToggle = lookup_widget(searchFrm,"ckbCom"); //-- pointer to check box
	percomToggle = lookup_widget(searchFrm,"cbpercom"); //-- pointer to check box	
	textWindow= lookup_widget(searchFrm,"txtSearch");	//-- pointer to text widget
	
	
	gtk_text_set_point(GTK_TEXT(textWindow), 0); //-- clear text window
	gtk_text_forward_delete (GTK_TEXT (textWindow), gtk_text_get_length((GTK_TEXT(textWindow))));

	if(GTK_TOGGLE_BUTTON(comToggle)->active)//-- if true search commentary
		searchMod = curcomMod; //-- set search module to current commentary module
	else if(GTK_TOGGLE_BUTTON(percomToggle)->active) //-- if true search personal commentary
		searchMod = percomMod; //-- set search module to personal commentary module
	else                    //-- if neither commertary nor personal check box checked
	  searchMod = curMod;  //-- set search module to current text module
	
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
		currentScope = &searchScopeList; //-- if we do = move searchlist into currentScope
	}
	else
	{
		searchScopeLowUp.ClearBounds();  //-------- clear old bounds
		searchScopeList.ClearList(); //------------ clear scope search list
		currentScope = 0; //------------ clear scope
	}

	count = 0;    //-- set count to 0 - we have not found anything yet
	gtk_clist_clear(GTK_CLIST(resultList));	//-- clear list widget for new results

	entryText = gtk_entry_get_text(GTK_ENTRY(searchText)); //-- what to search for
	srchText = entryText; //-- move from char* to string
	gtk_label_set_text( GTK_LABEL(lbSearchHits) ,"0" ); //-- set hits label to 0
	if (searchMod)  //-- must have a good module - not null
	{
		int searchType = GTK_TOGGLE_BUTTON(regexSearch)->active ? 0 : GTK_TOGGLE_BUTTON(phraseSearch)->active ? -1 : -2; //-- get search type
		int searchParams = GTK_TOGGLE_BUTTON(caseSensitive)->active ? 0 : REG_ICASE; //-- get search params - case sensitive
		gtk_clist_freeze(GTK_CLIST(resultList)); //-- keep list form scrolling until we are done
		for (ListKey &searchResults = searchMod->Search(srchText.c_str(), searchType, //-- give search string to module to search
									searchParams, currentScope); !searchResults.Error(); searchResults++)
		{
			resultText = (const char *)searchResults;  //-- put verse key string of find into a string
			gtk_clist_append(GTK_CLIST(resultList), &resultText); //-- store find in list
			searchScopeList << (const char *)searchResults;  //-- remember finds for next search's scope
			++count;                                         //-- if we want to use them
		}
		gtk_clist_thaw(GTK_CLIST(resultList)); //-- thaw list so we can look through the results
		sprintf(scount,"%d",count); //-- put count into string
		gtk_label_set_text( GTK_LABEL(lbSearchHits) ,scount ); //-- tell user how many hits we had
	}
}

//-------------------------------------------------------------------------------------------
void
resultsListSWORD(GtkWidget *searchFrm, gint row, gint column) //-- someone clicked the results list
{                                                             //-- from our search and sent us here
	GtkWidget	*resultList,  //-- pointer to resultlist
					 *textWindow,   //-- pointer to search dlg textwindow
					 *comToggle,    //-- pointer to search commentary check box
					 *percomToggle, //-- pointer to search personal check box
					 *contextToggle;//-- pointer to context check box
 	gchar 	 *text;     //-- pointer to resultlist key text
	SWModule *searchMod;  //-- pointer to search module
	ModMap::iterator it;	//-- manager iterator
	
	
	resultList = lookup_widget(searchFrm,"resultList"); //-- set pointer to resultList
	textWindow = lookup_widget(searchFrm,"txtSearch");  //-- set pointer to text window
	comToggle = lookup_widget(searchFrm,"ckbCom");      //-- set pointer to commentary check box
	percomToggle = lookup_widget(searchFrm,"cbpercom");      //-- set pointer to personal check box
	contextToggle = lookup_widget(searchFrm,"cbContext");      //-- set pointer to context check box
	
  gtk_clist_get_text(GTK_CLIST(resultList), row, column, &text); //-- get key text from resultlist

  if(GTK_TOGGLE_BUTTON(comToggle)->active) //-- check state of commentary check box
	{
		it = searchMgr->Modules.find(curcomMod->Name());  //-- if checked (true) use curMod for display	
		if (it != searchMgr->Modules.end()) 
		{
			searchMod = (*it).second;  //-- set search module to same as curMod
		}			
	}
	if(GTK_TOGGLE_BUTTON(percomToggle)->active) //-- check state of personal check box
	{
		it = searchMgr->Modules.find(percomMod->Name());  //-- if checked (true) use percomMod for display	
		if (it != searchMgr->Modules.end())
		{
			searchMod = (*it).second;  //-- set search module to same as percomMod
		}			
	}
	else
	{
		it = searchMgr->Modules.find(curMod->Name()); //-- nothing checked so we use curMod for display
		if (it != searchMgr->Modules.end()) 
		{
			searchMod = (*it).second; //-- set search module to curMod
		}			
	}		
	
	if(searchMod) //-- make sure module is not null
	{
		searchMod->SetKey(text); //-- set module to verse key text
		searchMod->Display();    //-- show verse or commentary
	}
	if(GTK_TOGGLE_BUTTON(contextToggle)->active) //-- check state of context check box
	{
		curMod->SetKey(text);  //-- set module to verse key text
		curMod->Display();    //-- show verse or commentary
	}
}

//-------------------------------------------------------------------------------------------
void
savelistinfo(gchar *filename, GtkWidget *list) //-- get info we need to save search results list
{
/*
	GtkWidget 	*label;
	gchar 			*text;
	gint				howmany;
	
	label = lookup_widget(list,"lbSearchHits");
	text = (char *)GTK_LABEL(label)->label;
	howmany = atoi(text);
	savelist(filename, list, howmany);	
*/
}

//-------------------------------------------------------------------------------------------
void
setupSearchDlg(GtkWidget *searchDlg) //-- init search dialog
{
		ModMap::iterator it;  //-- sword manager iterator
		gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(searchDlg,"txtSearch")) , TRUE ); //-- set text window to word wrap
		GBFsearchDisplay = new GTKInterlinearDisp(lookup_widget(searchDlg,"txtSearch")); //-- set sword display
    	//--------------------------------------------------------------------------------------- searchmodule	
		for (it = searchMgr->Modules.begin(); it != searchMgr->Modules.end(); it++) //-- iterator through modules
		{
			searchMod  = (*it).second;  //-- set searchMod
			searchMod->Disp(GBFsearchDisplay); //-- set search display for modules
		}
}

//-------------------------------------------------------------------------------------------
void
strongsSWORD(bool choice) //-- toogle strongs numbers for modules that have strongs
{
	if(choice) //-- if choice is true - we want strongs numbers
	{
		mainMgr->setGlobalOption("Strong's Numbers","On");  //-- turn strongs on 		
	}
	else   //-- we don't want strongs numbers
	{
		mainMgr->setGlobalOption("Strong's Numbers","Off");	//-- turn strongs off	
	}
	settings->strongs = choice;   //-- store choice in settings
	curMod->Display(); //-- we need to show change
}

//-------------------------------------------------------------------------------------------
void
footnotesSWORD(bool choice) //-- toogle gbf footnotes for modules that have them
{
	if(choice) //-- we want footnotes
	{
		mainMgr->setGlobalOption("Footnotes","On"); //-- turn footnotes on
	}
	else //-- we don't want footnotes
	{
		mainMgr->setGlobalOption("Footnotes","Off");	//-- turn footnotes off	
	}
	settings->footnotes = choice;   //-- store choice in settings
	curMod->Display(); //-- we need to show change
}

//-------------------------------------------------------------------------------------------
void
addBookmark(void)  //-- someone clicked add bookmark to get us here
{
	//GnomeUIInfo *bookmarkitem;  //-- pointer to gnome menu item structure
	gchar    		*bookname;      //-- pointer to the Bible book we want to mark
	gint       	iVerse,         //-- verse we want to mark
							iChap;          //-- chapter we want to mark
	gchar				buf[255];

	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook"))); //-- get book name
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse"))); //-- get verse number
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter"))); //-- get chapter
	sprintf(buf,"%s %d:%d\0",bookname, iChap,iVerse ); //-- put book chapter and verse into bookmarks array
	 ++ibookmarks;  //-- increment number of bookmark item + 1
	
	savebookmark(buf);  //-- save to file so we don't forget -- function in filestuff.cpp
	removemenuitems(MainFrm, "_Bookmarks/<Separator>", ibookmarks); //-- remove old bookmarks form menu -- menustuff.cpp	
  sprintf(buf,"%s%s", "_Bookmarks/",remembersubtree);
  addseparator(MainFrm, buf);
  //loadbookmarkarray(); //-- load edited bookmarks  -- filestuff.cpp
  loadbookmarks_afterSeparator(); //-- let's show what we did -- GnomeSword.cpp
}

//-------------------------------------------------------------------------------------------
void
editbookmarksLoad(GtkWidget *editdlg) //-- load bookmarks into an editor dialog
{
	GtkWidget *text;  //-- pointer to text widget for editing
	gchar buf[255];   //-- temp storage of each bookmark
	gint i=0;         //-- counter

	text = lookup_widget(editdlg,"text4");  //-- set text widger pointer
 	gtk_text_freeze (GTK_TEXT (text));    //-- freeze text until all bookmarks are loaded
  	gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);  //-- clear text widget
	while(i < ibookmarks)  //-- loop through bookmarks - ibookmarks the number of bookmarks we have
	{ 	
		sprintf(buf,"%s\n",bmarks[i]); //-- copy bookmark string from array to buf and add newline
		gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL,buf , -1); //-- put buf into text wigdet
		++i;	//-- increment our counter     	
	}
	sprintf(buf,"%s\n","-end-");  //-- last item to add to text widget '-end-' is used to signal last item
	gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL,buf , -1); //-- add to text widget
	gtk_text_thaw (GTK_TEXT (text)); //-- thaw text widget so we can work
}


//-------------------------------------------------------------------------------------------
void
editbookmarks(GtkWidget *editdlg) //-- load bookmarks into an editor dialog
{
	GtkWidget *list;
	struct LISTITEM listitems[ibookmarks];
	list = lookup_widget(editdlg,"clLElist");
	loadbookmarksnew(list);
}
//-------------------------------------------------------------------------------------------
gchar *
getlistitem(GtkWidget *list, gint row) //--
{
  /* 	gchar *buf,
   				*returnbuf[0][5];
   	
   	gtk_clist_get_text(GTK_CLIST(list), row, 0, &buf); //-- get item from list
		gtk_clist_get_text(GTK_CLIST(list), row, 1, &buf); //-- get type from list
		gtk_clist_get_text(GTK_CLIST(list), row, 2, &buf); //-- get level from list
		gtk_clist_get_text(GTK_CLIST(list), row, 3, &buf); //-- get item from list
		gtk_clist_get_text(GTK_CLIST(list), row, 4, &buf); //-- get item from list
		return(&returnbuf);
	*/
}



//-------------------------------------------------------------------------------------------
void
addHistoryItem(void)  //-- add an item to the history menu
{
	GnomeUIInfo *historyitem;  //-- pointer to gnome menu item structure
	gchar    *bookname,        //-- pointer to name of book of Bible to add to history
				ref[255];            //-- string to store Bible ref
	gint       iVerse,         //-- varible to store verse number
				iChap;               //-- varible to store chapter number

	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook"))); //-- set bookname to book
	                                                                            //-- showing in book combo box
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse"))); //-- get verse number from verse spin button
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")));//-- get chapter number from chapter spin button
	sprintf(ref,"%s %d:%d",bookname, iChap,iVerse ); //-- store book, chapter and verse in ref string
	additemtognomemenu(MainFrm, ref, "_History/<Separator>",(GtkMenuCallback) on_john_3_1_activate); //-- add item to history menu
	++historyitems;
}

//-------------------------------------------------------------------------------------------
void
changecurModSWORD(gchar *modName) //-- change sword module for main window
{                                 //-- someone clicked View->MainWindow->module
	ModMap::iterator it;            //-- or clicked the mainwindow popup menu and the callback sent us here

	it = mainMgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != mainMgr->Modules.end()) //-- if we find the module
	{
		curMod = (*it).second;  //-- change current module to new module
		curMod->SetKey(current_verse); //-- set key to current verse
		curMod->Display();            //-- show it to the world
	}
	strcpy(settings->MainWindowModule, curMod->Name()); //-- remember where we are so we can open here next time we startup
	gtk_frame_set_label( GTK_FRAME(lookup_widget(MainFrm,"frame9")), //-- set main window frame label of
                                 curMod->Name());                  //-- to current Module name
}

//-------------------------------------------------------------------------------------------
void
changecomp1ModSWORD(gchar *modName)  //-- change sword module for 1st interlinear window
{
	ModMap::iterator it;

	it = mainMgr1->Modules.find(modName);  //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != mainMgr1->Modules.end())     //-- if we find the module
	{
		comp1Mod = (*it).second;    //-- change current module to new module
		comp1Mod->SetKey(current_verse);  //-- set key to current verse
		comp1Mod->Display();              //-- show it to the world
	}
	strcpy(settings->Interlinear1Module, comp1Mod->Name()); //-- remember where we are so we can open here next time we startup
}

//-------------------------------------------------------------------------------------------
void
changecomp2ModSWORD(gchar *modName)  //-- change sword module for 2nd interlinear window
{
	ModMap::iterator it;

	it = mainMgr2->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != mainMgr2->Modules.end())    //-- if we find the module
	{
		comp2Mod = (*it).second;    //-- change current module to new module
		comp2Mod->SetKey(current_verse); //-- set key to current verse
		comp2Mod->Display();            //-- show it to the world
	}
	strcpy(settings->Interlinear2Module, comp2Mod->Name()); //-- remember where we are so we can open here next time we startup
}

//-------------------------------------------------------------------------------------------
void
changecomp3ModSWORD(gchar *modName)   //-- change sword module for 3rd interlinear window
{
	ModMap::iterator it;

	it = mainMgr3->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != mainMgr3->Modules.end())    //-- if we find the module
	{
		comp3Mod = (*it).second;     //-- change current module to new module
		comp3Mod->SetKey(current_verse); //-- set key to current verse
		comp3Mod->Display();          //-- show it to the world
	}
	strcpy(settings->Interlinear3Module, comp3Mod->Name()); //-- remember where we are so we can open here next time we startup
}

//-------------------------------------------------------------------------------------------
void
setversestyleSWORD(bool choice)  //-- set verse style -- verses or paragraphs
{
	if(choice)
	{
		bVerseStyle = TRUE;  //-- tells chapter display we want verses
	}
	else
	{
		bVerseStyle = FALSE;     //-- tells chapter display we want paragraphs
	}
	settings->versestyle = choice; //-- remember our choice for the next program startup
 	curMod->Display(); //-- show the change
}

//-------------------------------------------------------------------------------------------
void
showIntPage(bool choice)  //-- do we want to see interlinear page?
{
	GtkWidget *intpage; //-- pointer to interlinear notebook page

	intpage= lookup_widget(MainFrm,"vbox3"); //-- set pointer to page
	if(choice)
	{
		gtk_widget_show(intpage);		//-- show page
	}
	else
	{
		gtk_widget_hide(intpage);		//-- hide page
	}
	settings->interlinearpage = choice;  //-- remember choice for next program startup
}


//-------------------------------------------------------------------------------------------
void
chapterSWORD(void)  //-- someone clicked the chapter spin button
{
	gint       iChap;

	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter"))); //-- set iChap to value in spin button
	swKey.Chapter(iChap); //-- let sword set chapter for us - sword knows when to go to next or previous book - so we don't have to keep up
	changeVerse(swKey);	  //-- change all our modules to new chapter
}

//-------------------------------------------------------------------------------------------
void
verseSWORD(void)  //-- someone clicked the verse spin button
{
	gint       iVerse;

	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse"))); //-- set iVerse to value in spin button
	swKey.Verse(iVerse ); //-- let sword set verse for us - sword knows when to go to next or previous chapter - so we don't have to keep up
	changeVerse(swKey);	//-- change all our modules to new verse	
}

//-------------------------------------------------------------------------------------------
void
btnlookupSWORD(void)  //-- add current verse to history menu
{
	gchar *buf; //-- pointer to entry string

	ApplyChange = FALSE;	//-- do not want to start loop with book combo box
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeFreeformLookup")));	//-- set pointer to entry text
	changeVerse(buf);	//-- change verse to entry text
	addHistoryItem(); //-- verse to history menu
}

//-------------------------------------------------------------------------------------------
void
freeformlookupSWORD(GdkEventKey  *event) //-- change to verse in freeformlookup entry
{
 	gchar *buf; //-- pointer to entry string

	ApplyChange = FALSE;	//-- do not want to start loop with book combo box
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeFreeformLookup")));	//-- set pointer to entry text
	if(event->keyval == 65293 || event->keyval == 65421)  //-- if user hit return key continue
	{
		changeVerse(buf);	//-- change verse to entry text
		addHistoryItem(); //-- add verse to history menu
	}	
}

//-------------------------------------------------------------------------------------------
void
changcurcomModSWORD(gchar *modName, gint page_num)  //-- someone changed commentary notebook page (sent here by callback function notebook page change)
{
	ModMap::iterator it;
	GtkWidget *notebook; //-- pointer to commentary notebook
						
	notebook = lookup_widget(MainFrm,"notebook1"); //-- set notebook to commentary notebook
	settings->notebook1page = page_num; //-- save current page

	it = mainMgr->Modules.find(modName); //-- find commentary module (modName from page label)
	if (it != mainMgr->Modules.end()) 
	{
		curcomMod = (*it).second;  //-- set curcomMod to modName
		curcomMod->SetKey(curMod->KeyText()); //-- go to text (verse)
		curcomMod->Display(); //-- show the change
	}	
}

//-------------------------------------------------------------------------------------------
void
editnoteSWORD(bool editbuttonactive) //-- someone clicked the note edit button
{
 	if(editbuttonactive)
	{
			gtk_text_set_editable (GTK_TEXT (lookup_widget(MainFrm,"textComments")), TRUE); //-- set text widget to editable	
			gtk_widget_show(lookup_widget(MainFrm,"sbNotes")); //-- show comments status bar
			noteModified = FALSE;	 //-- we just turned edit mode on no changes yet
	}
	else
	{
			gtk_text_set_editable (GTK_TEXT (lookup_widget(MainFrm,"textComments")), FALSE); //-- set text widget to not editable
			gtk_widget_hide(lookup_widget(MainFrm,"sbNotes"));//-- hide comments status bar
			//if(noteModified) ;
	}
	percomMod->Display(); 	
}

//-------------------------------------------------------------------------------------------
void
savenoteSWORD(bool noteModified) //-- save personal comments
{  	
  if(noteModified) //-- if note modified save the changes
  {
		VerseKey mykey; //-- verse key text
		gchar *buf;     //-- pointer to a string
		GtkWidget *text; //-- pointer to commentary text widget
		
		text = lookup_widget(MainFrm,"textComments"); //-- get text widget
		buf = gtk_editable_get_chars((GtkEditable *)text,0,-1); //-- get comments from text widget
		*percomMod << (const char *)buf; //-- save note!
		noteModified = false; //-- we just saved the note so it has not been modified
	} 	
}

//-------------------------------------------------------------------------------------------
void
deletenoteSWORD(void)  //-- delete personal comment
{
		VerseKey mykey;
		gchar *buf;
		
		//-- first we clear the note window
		gtk_text_set_point(GTK_TEXT(lookup_widget(MainFrm,"textComments")), 0); //-- start clearing at beginning of text widget
		gtk_text_forward_delete(GTK_TEXT(lookup_widget(MainFrm,"textComments")), //-- delete from beginning to end
							 gtk_text_get_length(GTK_TEXT(lookup_widget(MainFrm,"textComments"))));
							
		//-- then we delete the note	
		percomMod->Delete();        //-- delete note
		percomMod->Display();        //-- show change
}

//-------------------------------------------------------------------------------------------
void
changcurdictModSWORD(gchar *modName, string keyText, gint page_num) //-- someone changed dict notebook page - sent here by notebook callback
{	                    //-- modName form page label - keyText from dict lookup entry
	ModMap::iterator it;
  GtkWidget *notebook; //-- pointer to dict&lex notebook
						
	notebook = lookup_widget(MainFrm,"notebook4"); //-- set notebook to dict&lex notebook
	settings->notebook2page = page_num; //-- save current page
	
	it = mainMgr->Modules.find(modName);  //-- find module we want to use
	if (it != mainMgr->Modules.end()) 
	{
		curdictMod = (*it).second;  //-- set curdictMod to new choice
		curdictMod->SetKey(keyText.c_str());   //-- set key to text from lookup entry
		curdictMod->Display();	 //-- display new dict
		FillDictKeys(curdictMod->Name()); //-- fill the list widget with keys	
	}
}

//-------------------------------------------------------------------------------------------
void
dictSearchTextChangedSWORD(char* mytext)   //-- dict lookup text changed
{	                        //-- mytext from the dict dict lookup entry
	string keyText;

	keyText = mytext;
	strcpy(settings->dictkey,mytext);
	if (curdictMod) //-- if we have a dict module
	{
		if(strcmp(mytext,""))  //-- if text is not null
		{
			curdictMod->SetKey(keyText.c_str()); //-- set key to our text
			curdictMod->Display();	//-- show what we found
			FillDictKeys(curdictMod->Name()); //-- fill the list widget with keys	
		}
	}
}

//-------------------------------------------------------------------------------------------
void
dictchangekeySWORD(gint direction)   //-- dict change key up or down -- arrow buttons
{	
   if(direction == 1) //-- next key
   {
			(*curdictMod)++;  //-- move up one
   }
   else if(direction == 0)      //-- previous key
   {
      (*curdictMod)--;  //-- move down one
   }
   curdictMod->Display(); //-- show the changes
	//-- put new key into dictionary text entry
   gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"dictionarySearchText")), curdictMod->KeyText());
}

//-------------------------------------------------------------------------------------------
void
changepercomModSWORD(gchar* modName)   //-- change personal comments module
{	
	GtkWidget *notebook, //-- pointer to a notebook widget
						*label;    //-- pointer to a label widget
	ModMap::iterator it; //-- module iterator
	
  if(noteModified) savenoteSWORD(noteModified);  //-- if personal comments changed save changes before we change modules and lose our changes
	it = mainMgr->Modules.find(modName); //-- find commentary module (modName from page label)
	if (it != mainMgr->Modules.end()) //-- if we don't run out of mods before we find the one we are looking for
	{
		percomMod = (*it).second;  //-- set curcomMod to modName
		strcpy(settings->personalcommentsmod, percomMod->Name());
		percomMod->SetKey(curMod->KeyText()); //-- go to text (verse)
		percomMod->Display(); //-- show the change 	
		//-- let's change the notebook label to match our percomMod (current personal comments module)
  	notebook = lookup_widget(MainFrm,"notebook3");  //-- get the notebook our page is in]
		label = gtk_label_new (percomMod->Name());   //-- create new label with mod name as the text
		gtk_widget_show (label);   //-- make is visible
		gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label); //-- put label on personal comments page
	}	
}

//-------------------------------------------------------------------------------------------
void
setcurrentversecolor(gint arg1, gint arg2, gint arg3)    //-- someone change the color setting for the current verse
{
 	gchar buf[10]; //-- temp storage for args
 	
  //-- set color for current session
	myGreen.red = arg1;  //-- new red setting
	settings->currentverse_red = arg1; //-- remember setting
	myGreen.green = arg2;//-- new green setting
	settings->currentverse_green= arg2; //-- remember setting
	myGreen.blue = arg3; //-- new blue setting
	settings->currentverse_blue = arg3; //-- remember setting 	
	curMod->Display(); //-- display change
}

//-------------------------------------------------------------------------------------------
void
setautosave(gboolean choice)    //-- someone clicked auto save personal  comments
{
	if(choice) //-- if choice was to autosave
	{
		autoSave = true;
	}
	else      //-- if choice was not to autosave
	{
		autoSave = false;
	}
	settings->autosavepersonalcomments = choice; //-- remember our choice for next startup
}

//-------------------------------------------------------------------------------------------
void
clearhistory(void)    //-- someone clicked clear history
{
  removemenuitems(MainFrm, "_History/<Separator>", historyitems+1);
  historyitems = 0;
  addseparator(MainFrm, "_History/C_lear");
}

//-------------------------------------------------------------------------------------------
void
printfile(void)    //-- someone clicked print in studypad
{
  if(current_filename != NULL)
  {
  	gchar buf[255];
  	
  	sprintf(buf,"lpr %s",current_filename);
  }
}

//-------------------------------------------------------------------------------------------
void
openpropertiesbox(void)    //-- someone clicked properties
{
	GtkWidget *Propertybox,  //-- pointer to propertybox dialog
						*cpcurrentverse;  //-- pointer to current verse color picker
	gushort 	red,       //-- vars for setting color
						green,
						blue,
						a;
	
	Propertybox = create_propertybox1(); //-- create propertybox dialog
	cpcurrentverse = lookup_widget(Propertybox,"cpfgCurrentverse"); //-- set cpcurrentverse to point to color picker
  a = 000000;
  //-- setup current verse color picker
	red = settings->currentverse_red;  //-- get color from settings structure
	green = settings->currentverse_green;
	blue =settings->currentverse_blue;
	gnome_color_picker_set_i16 (GNOME_COLOR_PICKER(cpcurrentverse),red ,green , blue, a); //-- set color of current verse color picker button	
	gtk_widget_show(Propertybox); //-- show propertybox
}

//-------------------------------------------------------------------------------------------
void
changepagenotebook(GtkNotebook *notebook,gint page_num) //-- someone changed the page in the main notebook
{
   settings->notebook3page = page_num; //-- store the page number so we can open to it the next time we start
}

//-------------------------------------------------------------------------------------------
void
showmoduleinfoSWORD(char *modName) //--  show module about information in an about dialog
{
	GtkWidget *aboutbox,  //-- pointer to about dialog
 						*text,       //-- pointer to text widget of dialog
    				*label;     //-- pointer to label in dialog
	char 			*buf,       //-- pointer to text buffer for label (mod name)
 						*bufabout;  //-- pointer to text buffer for text widget (mod about)
  ModMap::iterator it; //-- module iterator
	SectionMap::iterator sit; //--
	ConfigEntMap::iterator cit; //--
	
	it = mainMgr->Modules.find(modName); //-- find module (modName)
	if (it != mainMgr->Modules.end()) //-- if we don't run out of mods before we find the one we are looking for
	{
	    buf = (char *)(*it).second->Description();  //-- get discription of module
	    sit = mainMgr->config->Sections.find((*it).second->Name());
	    if (sit !=mainMgr->config->Sections.end())
	    {
	    	cit = (*sit).second.find("About");
					if (cit != (*sit).second.end()) 				
						bufabout = (*cit).second.c_str(); //-- get module about information
	    }
	}
	aboutbox = create_aboutmodules(); //-- create about dialog
	text = lookup_widget(aboutbox,"textModAbout"); //-- get text widget
	label = lookup_widget(aboutbox,"lbModName");    //-- get label
	gtk_label_set_text( GTK_LABEL(label),buf);  //-- set label to module discription
	gtk_text_set_word_wrap(GTK_TEXT (text) , TRUE ); //-- set word wrap to true for text widget
	AboutModsDisplay(text, bufabout) ; //-- send about info and text widget to display function (display.cpp)
	gtk_widget_show(aboutbox); //-- show the about dialog   	
}

//-------------------------------------------------------------------------------------------
void
showinfoSWORD(GtkWidget *text, GtkLabel *label) //--  show text module about information in the Sword Project about dialog
{
	char 			*buf,       //-- pointer to text buffer for label (mod name)
 						*bufabout;  //-- pointer to text buffer for text widget (mod about)
  ModMap::iterator it; //-- module iterator
	SectionMap::iterator sit; //--
	ConfigEntMap::iterator cit; //--
	
	it = mainMgr->Modules.find(curMod->Name()); //-- find module (modName)
	if (it != mainMgr->Modules.end()) //-- if we don't run out of mods before we find the one we are looking for
	{
	    buf = (char *)(*it).second->Description();  //-- get discription of module
	    sit = mainMgr->config->Sections.find((*it).second->Name());
	    if (sit !=mainMgr->config->Sections.end())
	    {
	    	cit = (*sit).second.find("About");
					if (cit != (*sit).second.end()) 				
						bufabout = (*cit).second.c_str(); //-- get module about information
	    }
	}
	
	gtk_label_set_text( GTK_LABEL(label),buf);  //-- set label to module discription
	gtk_text_set_word_wrap(GTK_TEXT(text), TRUE ); //-- set word wrap to true for text widget
	AboutModsDisplay(text, bufabout) ; //-- send about info and text widget to display function (display.cpp)
}