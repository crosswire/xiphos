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
#include <gbfhtml.h>
#include <plainhtml.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>

#include  <widgets/shortcut-bar/e-shortcut-bar.h>


#include "display.h"
#include "callback.h"
#include "GnomeSword.h"
#include "support.h"
#include "interface.h"
#include "filestuff.h"
#include "menustuff.h"
#include "listeditor.h"
#include "noteeditor.h"

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
SWDisplay *RWPDisplay; //--- to display Robertson's Word Pictures in the New Testament using GtkText
SWDisplay *FPNDisplay; //--- to display formatted personal notes using GtkText
SWDisplay *GBFDisplay; //--- to display formatted gbf
SWDisplay *HTMLDisplay; //--- to display formatted html
SWDisplay *HTMLchapDisplay; //--- to display formatted html

SWMgr *mainMgr; //-- sword mgr for curMod - curcomMod - curdictMod
SWMgr *mainMgr1; //-- sword mgr for comp1Mod - first interlinear module
SWMgr *mainMgr2; //-- sword mgr for comp2Mod - second interlinear module 
SWMgr *mainMgr3; //-- sword mgr for comp3Mod - third interlinear module
SWMgr *percomMgr; //-- sword mgr for percomMod - personal comments editor

VerseKey vsKey; //-- ??
SWModule *curMod; //-- module for main text window
SWModule *comp1Mod; //-- module for first interlinear window
SWModule *comp2Mod; //-- module for second interlinear window	
SWModule *comp3Mod;	 //-- module for third interlinear window
SWModule *curcomMod; //-- module for commentary  window	
SWModule *percomMod; //-- module for personal commentary  window	
SWModule *curdictMod; //-- module for dict window



SWFilter *gbftoplain;
SWFilter *gbftohtml;
SWFilter *plaintohtml;

//---------------------------------------------------------------- GnomeSword global to this file

SETTINGS *settings;
SETTINGS myset;	
extern GtkWidget    *MainFrm,	//-- main form widget 					
			        *shortcut_bar;
GtkWidget           *NEtext,  //-- note edit widget
					*bookmark_mnu; //-- popup menu for bookmarks	
							
extern EShortcutModel *shortcut_model;
extern gchar *shortcut_types[];
gchar groupitems[5][50][256];

gchar *current_filename= NULL;	//-- filename for open file in study pad window 
gchar current_verse[80]="Romans 8:28";	//-- current verse showing in main window - 1st - 2nd - 3rd interlinear window - commentary window
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
						*strongsnum,//-- widget to access toggle menu - for strongs numbers
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
gboolean havedict = false; //-- let us know if we have at least one lex-dict module
gboolean havecomm = false; //-- let us know if we have at least one commentary module
gboolean autoscroll = true; //-- commentary module auto scroll when true -- in sync with main text window
gboolean isstrongs = false; //-- main window selection is not storngs number
extern gchar rememberlastitem[255]; //-- used for bookmark menus declared in filestuff.cpp
extern gchar remembersubtree[256];  //-- used for bookmark menus declared in filestuff.cpp
extern NoteEditor *noteeditor;
gint historyitems = 0;
gint answer;
gint dictpages,
	 compages;
gchar com_key[80] ="Rom 8:28"; //-- current commentary key
extern GnomeUIInfo pmDict_uiinfo[5];
extern GnomeUIInfo pmComments_uiinfo[7];
extern GnomeUIInfo view_module1_menu_uiinfo[2];
extern GtkWidget *pmComments;
gint    groupnum1 = 0,
        groupnum2 = 0,
        groupnum3 = 0,
        greekpage = 0,
        hebrewpage = 0;
//----------------------------------------------------------------------------------------------
void
initSword(GtkWidget *mainform,  //-- apps main form
	GtkWidget *menu1,  //-- main text window's popup menu
	GtkWidget *menu2,  //-- 1st inetrlinear window's popup menu
	GtkWidget *menu3,  //-- 2nd inetrlinear window's popup menu
	GtkWidget *menu4,  //-- 3rd inetrlinear window's popup menu
	GtkWidget *menu5) //-- edit note wintow popup menu
{
	ModMap::iterator it; //-- iteratior
	SectionMap::iterator sit; //-- iteratior
	ConfigEntMap::iterator cit; //-- iteratior
	int	pg=0, //-- notebook page number - for creating notebook pages
		pg1=0, //-- notebook page number - for creating notebook pages
		pg2=0; //-- notebook page numger
	GtkWidget
		*empty_notebook_page, //-- used to create new pages
		*notebook, //-- to access notebooks for adding pages
		*menuChoice, //-- gtk menu item
		*popmenu, //-- to access popup menus for adding items
		*menuChoice1, //-- gtk menu item
		*label, //-- labels for adding notebook pages
		*menu, //-- ??
		*menuNE,
		*menuCom,
		*menuDict;
		//iBibletext=0; //--
	char    
		menuName[64], //--  for menu item label
		menuName1[64], //--  for menu item label
		mybuf[80], //-- ??
		rememberlastitem[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem2[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		aboutrememberlastitem3[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		rememberlastitemCom[80], //--  use to store last menu item so we can add the next item under it - gnome menus
		rememberlastitemDict[80]; //--  use to store last menu item so we can add the next item under it - gnome menus	
		//rememberlookup[80]; //--  use to store last menu item
	int   
		viewNumber = 1, //--  for numbering menu items
		viewNumber1= 1, //-- for numbering menu items
		itemNum = 0, //-- for numbering shortbar items
		itemNum2 = 0,//-- for numbering shortbar items
		itemNum3 = 0,//-- for numbering shortbar items
		i, //-- counter
		j; //-- counter
		
	GList   *biblemods = NULL,
	        *commentarymods = NULL,
	        *dictionarymods = NULL;
					
   	gchar *sourceformat;
	GnomeUIInfo *menuitem; //--  gnome menuitem
  	GtkWidget *menu_items;
  	myset = readsettings();  //-- load settings into structure
  	settings = &myset;       //-- set pointer to structure
  	gbftoplain	= new GBFPlain(); //-- renderfilter
  	gbftohtml		= new GBFHTML(); //-- sword renderfilter gbf to html
  	plaintohtml   = new PLAINHTML(); //-- sword renderfilter plain to html

	mainMgr         = new SWMgr();	//-- create sword mgrs
	mainMgr1        = new SWMgr();
	mainMgr2        = new SWMgr();
	mainMgr3        = new SWMgr();	
  	percomMgr	= new SWMgr();
	
	curMod        = NULL; //-- set mods to null
	comp1Mod      = NULL;
	comp2Mod      = NULL;
	comp3Mod      = NULL;
	curcomMod     = NULL;
	curdictMod    = NULL; 	
	percomMod     = NULL;

	
	chapDisplay     = 0;// set in create
	entryDisplay    = 0;// set in create
	comp1Display    = 0;// set in create
	comp2Display    = 0;// set in create
	comp3Display    = 0;// set in create
	comDisplay      = 0;// set in create
	dictDisplay     = 0;// set in create
	GBFcomDisplay   = 0; // set in create 	
	percomDisplay   = 0;// set in create
	RWPDisplay			= 0;
	FPNDisplay			= 0;
	GBFDisplay			= 0;
	HTMLDisplay			= 0;
	HTMLchapDisplay			= 0;

  	myGreen.red =  settings->currentverse_red; //-- set color for current verse
	myGreen.green = settings->currentverse_green;
	myGreen.blue =  settings->currentverse_blue;
	
	MainFrm = lookup_widget(mainform,"mainwindow"); //-- save mainform for use latter
	NEtext =  lookup_widget(MainFrm,"textComments"); //-- get note edit widget	
  	
        //------------------------------------------------------------- setup displays for sword modules
    	noteeditor = new NoteEditor();
	GTKEntryDisp::__initialize();
	chapDisplay = new GTKChapDisp(lookup_widget(mainform,"moduleText"));
	dictDisplay = new GTKEntryDisp(lookup_widget(mainform,"textDict"));
	comDisplay = new  GTKEntryDisp(lookup_widget(mainform,"textCommentaries"));
	percomDisplay = new  GTKPerComDisp(lookup_widget(mainform,"textComments"));
	GBFcomDisplay = new  GTKInterlinearDisp(lookup_widget(mainform,"textCommentaries"));
	comp1Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp1"));
	comp2Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp2"));
	comp3Display = new GTKInterlinearDisp(lookup_widget(mainform,"textComp3"));
	RWPDisplay = new GTKRWPDisp(lookup_widget(mainform,"textCommentaries"));
	FPNDisplay = new HTMLentryDisp(lookup_widget(mainform,"textComments"));
	//GBFDisplay = new GBFentryDisp(lookup_widget(mainform,"textCommentaries"));
	HTMLDisplay = new HTMLentryDisp(lookup_widget(mainform,"textCommentaries"));
	HTMLchapDisplay = new HTMLChapDisp(lookup_widget(mainform,"moduleText"));	

        //---------------------------------------------------------------- set text windows to word warp
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"moduleText")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp1")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp2")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComp3")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textDict")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textCommentaries")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"textComments")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(mainform,"text3")) , TRUE );	
	gnome_popup_menu_attach(menu1,lookup_widget(mainform,"moduleText"),(gchar*)"1");
	
        //--------------------------------------------------------------- create shortcut bar groups
	if(settings->showtextgroup){
	    add_test_group((EShortcutBar *)shortcut_bar, "Bible Text");
	    groupnum1 = 0;
	}
	else groupnum1 = -1;
	if(settings->showcomgroup){
	    add_test_group((EShortcutBar *)shortcut_bar, "Commentaries");
	    if(groupnum1 == 0) groupnum2 = 1;
	    else groupnum2 = 0;
	}else groupnum2 = -1;
	if(settings->showdictgroup){
	    add_test_group((EShortcutBar *)shortcut_bar, "Dict/Lex");
	    if(groupnum2 == 1) groupnum3 = 2;
	    if(groupnum2 == 0) groupnum3 = 1;
	    if(groupnum2 == -1 && groupnum1 == 0) groupnum3 = 1;
	    if(groupnum2 == -1 && groupnum1 == -1) groupnum3 = 0;
	}
        //--------------------------------------------------------- store text widgets for spell checker
	notes =  lookup_widget(mainform,"textComments");
	studypad = lookup_widget(mainform,"text3");
        //----------------------------------------------------- set main window modules and add to menus	
	sprintf(rememberlastitem,"%s","_View/Main Window/");
	sprintf(rememberlastitemCom,"%s","_View/Commentary Window/");
	sprintf(rememberlastitemDict,"%s","_View/Dict-Lex Window/");

	//sprintf(remberlookup,"%s","");
	sprintf(aboutrememberlastitem,"%s","_Help/About Sword Modules/Bible Texts/<Separator>");
	sprintf(aboutrememberlastitem2,"%s","_Help/About Sword Modules/Commentaries/<Separator>");
	sprintf(aboutrememberlastitem3,"%s","_Help/About Sword Modules/Dictionaries-Lexicons/<Separator>");
	compages = 0;
	dictpages = 0;
	for(it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			curMod = (*it).second;
			biblemods = g_list_append(biblemods,curMod->Name());
			//------------------------------------------------------------------ add to menubar
			additemtognomemenu(MainFrm, curMod->Name(), curMod->Name(), rememberlastitem , (GtkMenuCallback)on_mainText_activate );
			additemtognomemenu(MainFrm, curMod->Name(), curMod->Name(), aboutrememberlastitem , (GtkMenuCallback)on_kjv1_activate );
			//--------------------------------------- remember last item - so next item will be place below it       	
			sprintf(rememberlastitem,"%s%s","_View/Main Window/",curMod->Name());	
			sprintf(aboutrememberlastitem,"%s%s","_Help/About Sword Modules/Bible Texts/",curMod->Name());			
			//---------------------------------------------------------------- add to popup menu
			additemtopopupmenu(MainFrm, menu1, curMod->Name(), (GtkMenuCallback)on_mainText_activate );
			//------------------------------------------------------- set GTK display for each module
			sit = mainMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters
	        	if(sit !=mainMgr->config->Sections.end()){
	    	    		cit = (*sit).second.find("SourceType");
				if(cit != (*sit).second.end()) sourceformat = (char *)(*cit).second.c_str();
				else sourceformat = "Plain";
			}
			if(!strcmp(sourceformat, "GBF")){ //-- we need gbf to html filter			
				curMod->AddRenderFilter(gbftohtml);
				curMod->Disp(HTMLchapDisplay);
			}else if(!strcmp(sourceformat, "Plain")){ //-- we need gbf to html filter			
			  	curMod->AddRenderFilter(plaintohtml);
				curMod->Disp(HTMLchapDisplay);
			}else			
				curMod->Disp(chapDisplay);
			//g_free(sourceformat);
		  	//-----------   add choice to shortcut bar
		  	if(settings->showtextgroup){
		    		sprintf(groupitems[groupnum1][itemNum++], "%s", curMod->Name());
		    		e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
						      groupnum1, -1,
						      shortcut_types[0],
						      curMod->Name());
		  	}//gtk_menu_shell_append		  			
		}else if (!strcmp((*it).second->Type(), "Commentaries")){ //-- set commentary modules and add to notebook		
			curcomMod = (*it).second;
			commentarymods = g_list_append(commentarymods,curcomMod->Name());
			havecomm = true; //-- we have at least one commentay module
			++compages; //-- how many pages do we have
			curcomMod = (*it).second;
			notebook = lookup_widget(mainform,"notebook1");
			empty_notebook_page = gtk_vbox_new (FALSE, 0);
  		    	gtk_widget_show (empty_notebook_page);
  		    	gtk_container_add (GTK_CONTAINER (notebook), empty_notebook_page);
			label = gtk_label_new (curcomMod->Name());
			gtk_widget_show (label);
			sprintf(mybuf,"%d",pg1);
			additemtognomemenu(MainFrm, curcomMod->Name(), mybuf, rememberlastitemCom, (GtkMenuCallback)on_com_select_activate);
			gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), pg1++), label); 
			sprintf(rememberlastitemCom,"%s%s","_View/Commentary Window/",curcomMod->Name());	
			additemtognomemenu(MainFrm,curcomMod->Name(), curcomMod->Name(),aboutrememberlastitem2 , (GtkMenuCallback)on_kjv1_activate );
			/*						
			//---------------------------------------------------------------- add to popup menu
			gtk_menu_item_set_submenu (GTK_MENU_ITEM (pmComments_uiinfo[6].widget),GTK_WIDGET(view_module1_menu_uiinfo));
			additemtopopupmenu(MainFrm, pmComments_uiinfo[6].widget, curcomMod->Name(), (GtkMenuCallback)on_com_select_activate);
			*/
			sprintf(aboutrememberlastitem2,"%s%s","_Help/About Sword Modules/Commentaries/",curcomMod->Name());			
			sit = mainMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters
	    		if (sit !=mainMgr->config->Sections.end()){
	    			cit = (*sit).second.find("SourceType");
				if (cit != (*sit).second.end())	sourceformat = (char *)(*cit).second.c_str();
				else sourceformat ="Plain";
			}
			if (!strcmp(sourceformat, "GBF")){ //-- we need gbf to html filter
			
				curcomMod->AddRenderFilter(gbftohtml);
				curcomMod->Disp(HTMLDisplay);
			}
			else if(!strcmp(sourceformat,"ThML")) curcomMod->Disp(HTMLDisplay);
			else if(!strcmp(curcomMod->Name(),"RWP")) curcomMod->Disp(RWPDisplay);
			else if((*mainMgr->config->Sections[(*it).second->Name()].find("ModDrv")).second == "RawFiles"){ //-- if driver is RawFiles			   				 	
				 if(settings->formatpercom) curcomMod->Disp(HTMLDisplay);
				 else curcomMod->Disp(comDisplay);
			}else curcomMod->Disp(comDisplay);
			//g_free(sourceformat);
			if(settings->showcomgroup){
			    sprintf(groupitems[groupnum2][itemNum2++], "%s", curcomMod->Name());
			    e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
						      groupnum2, -1,
						      shortcut_types[0],
						      curcomMod->Name());
			}
		}else if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){ //-- set dictionary modules and add to notebook	
			havedict = true; //-- we have at least one lex / dict module
			++dictpages; //-- how many pages do we have
			curdictMod = (*it).second;
			dictionarymods = g_list_append(dictionarymods,curdictMod->Name());
			notebook = lookup_widget(mainform,"notebook4");
			additemtognomemenu(MainFrm,curdictMod->Name(), curdictMod->Name(),aboutrememberlastitem3 ,
						 (GtkMenuCallback)on_kjv1_activate );
			sprintf(aboutrememberlastitem3,"%s%s","_Help/About Sword Modules/Dictionaries-Lexicons/",curdictMod->Name());	
			empty_notebook_page = gtk_vbox_new (FALSE, 0);
  		    	gtk_widget_show (empty_notebook_page);
  		    	gtk_container_add (GTK_CONTAINER (notebook), empty_notebook_page);
			label = gtk_label_new (curdictMod->Name());
			gtk_widget_show (label);
			sprintf(mybuf,"%d",pg2);			
			if(!strcmp(curdictMod->Name(),"StrongsHebrew")) hebrewpage = pg2;
			if(!strcmp(curdictMod->Name(),"StrongsGreek")) greekpage = pg2;
			additemtognomemenu(MainFrm, curdictMod->Name(), mybuf, rememberlastitemDict,
						 (GtkMenuCallback)on_dict_select_activate );
			gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 
						pg2++), label);
			sprintf(rememberlastitemDict,"%s%s","_View/Dict-Lex Window/",curdictMod->Name());
			curdictMod->Disp(dictDisplay);
			if(settings->showcomgroup){			
			        sprintf(groupitems[groupnum3][itemNum3++], "%s", curdictMod->Name());
			        e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
						      groupnum3, -1,
						      shortcut_types[0],
						      curdictMod->Name());
			}
		}
	} 		
  	//----------------------------------------------------------------- set up percom editor module and menu
	for (it = percomMgr->Modules.begin(); it != percomMgr->Modules.end(); it++){
		if (!strcmp((*it).second->Type(), "Commentaries")){ //-- if type is Commentaries
		 	 //-- if driver is RawFiles			
			if((*percomMgr->config->Sections[(*it).second->Name()].find("ModDrv")).second == "RawFiles"){ 
				 percomMod = (*it).second;
				 if(settings->formatpercom) percomMod->Disp(FPNDisplay);  //-- if true use formatted display
				 else percomMod->Disp(percomDisplay);                     //-- else standard displey
				 additemtopopupmenu(MainFrm, menu5, percomMod->Name(), (GtkMenuCallback)on_change_module_activate); //-- add module to popup menu
				 usepersonalcomments = true; //-- used by verseChange function (GnomeSword.cpp)
				 percomMod->SetKey(settings->currentverse);
				 gtk_widget_show(lookup_widget(MainFrm,"vbox2")); //-- show personal comments page because we
			} 	                                                 //-- have at least one personl module
	  	}
	}	
	//---------------------------------------------------------------------------------- interlinear 1	
	sprintf(rememberlastitem,"%s","_View/Interlinear1 Window/");
	for (it = mainMgr1->Modules.begin(); it != mainMgr1->Modules.end(); it++){	
		comp1Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")){						
			 //------------------------------------------------------------------------------- add to menubar
			additemtognomemenu(MainFrm, comp1Mod->Name(), comp1Mod->Name(),rememberlastitem ,
						 (GtkMenuCallback)on_1st_interlinear_window1_activate);
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
	for (it = mainMgr2->Modules.begin(); it != mainMgr2->Modules.end(); it++){
		comp2Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")){						
			 //------------------------------------------------------------------------------- add to menubar
			additemtognomemenu(MainFrm, comp2Mod->Name(), comp2Mod->Name(),rememberlastitem ,
						 (GtkMenuCallback)on_2nd_interlinear_window1_activate);
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
	for (it = mainMgr3->Modules.begin(); it != mainMgr3->Modules.end(); it++){
		comp3Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")){
			 //------------------------------------------------------------------------------- add to menubar
			additemtognomemenu(MainFrm, comp3Mod->Name(), comp3Mod->Name(),rememberlastitem ,
						 (GtkMenuCallback)on_3rd_interlinear_window1_activate);
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
	if(havedict){ //-- let's don't do this if we don't have at least one dictionary / lexicon
			
		gint pagenum = 0; //-- set page to 0 (first page in notebook)
		//-- if save page is less than actual number of pages use saved page number else 0
		if(settings->notebook2page < dictpages) pagenum = settings->notebook2page; 
		notebook = lookup_widget(mainform,"notebook4"); //-- get notebook
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),pagenum ); //-- set notebook page	
		//-- get the label from the notebook page (module name)		
		label1 = (GtkLabel *)gtk_notebook_get_tab_label (GTK_NOTEBOOK(notebook), 
						gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),pagenum));
		it = mainMgr->Modules.find((char *)label1->label); //-- fint the module using text from label	
		if (it != mainMgr->Modules.end()){
			curdictMod = (*it).second; //-- set dictionary module
			curdictMod->SetKey(settings->dictkey); //-- set key to the key saved in settings
			curdictMod->Display(); //-- show what we have done	
			FillDictKeys(curdictMod->Name()); //-- fill dictionary key clist widget	
		}
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                      GTK_SIGNAL_FUNC (on_notebook4_switch_page),
                      NULL);
                if(settings->showdicttabs) gtk_widget_show(notebook);
                else gtk_widget_hide(notebook);
	}else gtk_widget_hide(lookup_widget(MainFrm,"hbox8")); //-- hide dictionary section of window if we do not have at least one dict/lex
	
	//---------------------------------------------------------------------------- set com module to open notebook page	
	if(havecomm){ //-- let's don't do this if we don't have at least one commentary	
		gint pagenum = 0;
		
		if(settings->notebook1page < compages) pagenum = settings->notebook1page;
		notebook = lookup_widget(mainform,"notebook1");	
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),pagenum );
		label1 = (GtkLabel *)gtk_notebook_get_tab_label (GTK_NOTEBOOK(notebook),
						gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),pagenum));
		it = mainMgr->Modules.find((char *)label1->label); 
		if (it != mainMgr->Modules.end()) curcomMod = (*it).second;
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                                        GTK_SIGNAL_FUNC (on_notebook1_switch_page),
                                        NULL);
                if(settings->showcomtabs) gtk_widget_show(notebook);
                else gtk_widget_hide(notebook);
        }else gtk_notebook_remove_page( GTK_NOTEBOOK(lookup_widget(MainFrm,"notebook3")) , 0);
    
	//---------------------------------------------------------------- set main notebook page
	notebook = lookup_widget(mainform,"notebook3");	
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook),settings->notebook3page );
		
	//---------------------------------------------------------------- set personal commets notebook label and display module
	if(usepersonalcomments){
		it = percomMgr->Modules.find(settings->personalcommentsmod); //-- set percomMod to last used
		if (it != percomMgr->Modules.end()) percomMod = (*it).second;
		if(percomMod){ //-- if we found the module set up page and show the world			
			notebook = lookup_widget(mainform,"notebook3");
			label = gtk_label_new (percomMod->Name());
			gtk_widget_show (label);
			gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label);
			percomMod->Display();
		}
	}
	
	//--------------------------------------------- use module in options list to set main window text module
	it = mainMgr->Modules.find(settings->MainWindowModule);
	if (it != mainMgr->Modules.end()){
	        curMod = (*it).second;
		gtk_frame_set_label( GTK_FRAME(lookup_widget(MainFrm,"frame9")),curMod->Name()); //-- set main text window	
	}                                                                                  //-- label to modName

	//-------------------------------------------------- use module in options list to set 1st int text module
	it = mainMgr1->Modules.find(settings->Interlinear1Module);
	if (it != mainMgr1->Modules.end())  comp1Mod = (*it).second;
	else comp1Mod = NULL;
	//-------------------------------------------------- use module in options list to set 2nt int text module
	it = mainMgr2->Modules.find(settings->Interlinear2Module);
	if (it != mainMgr2->Modules.end()) comp2Mod = (*it).second;	
        else comp2Mod = NULL;
	//--------------------------------------------------- use module in options list to set 3rd int text module
	it = mainMgr3->Modules.find(settings->Interlinear3Module);
	if (it != mainMgr3->Modules.end()) comp3Mod = (*it).second;	
	else comp3Mod = NULL;
	
	//-------------------------------------------------------- Add options to Options Menu and get toggle item widget
	autosaveitem = additemtooptionmenu(MainFrm, "_Settings/", "Auto Save Personal Comments", (GtkMenuCallback)on_auto_save_notes1_activate);
	notepage  = additemtooptionmenu(MainFrm, "_Settings/", "Show Interlinear Page", (GtkMenuCallback)on_show_interlinear_page1_activate);
	versestyle = additemtooptionmenu(MainFrm, "_Settings/", "Verse Style", (GtkMenuCallback)on_verse_style1_activate);
	footnotes   = additemtooptionmenu(MainFrm, "_Settings/", "Show Footnotes", (GtkMenuCallback)on_footnotes1_activate);
 	strongsnum   = additemtooptionmenu(MainFrm, "_Settings/", "Show Strongs Numbers", (GtkMenuCallback)on_strongs_numbers1_activate); 	
 	/* create pop menu for commentaries */
	menuCom = create_pmComments2(commentarymods);
	/* create popup menu for dict/lex window */
	menuDict = create_pmDict(dictionarymods); 	
	//-------------------------------------------------------------- attach popup menus
	
	gnome_popup_menu_attach(menu2,lookup_widget(mainform,"textComp1"),(gchar*)"1");
	gnome_popup_menu_attach(menu3,lookup_widget(mainform,"textComp2"),(gchar*)"1");
	gnome_popup_menu_attach(menu4,lookup_widget(mainform,"textComp3"),(gchar*)"1");
	gnome_popup_menu_attach(menu5,lookup_widget(mainform,"textComments"),(gchar*)"1");
	gnome_popup_menu_attach(menuDict,lookup_widget(mainform,"textDict"),(gchar*)"1");
	GTK_CHECK_MENU_ITEM (lookup_widget(menuDict,"show_tabs1"))->active = settings->showdicttabs;
	gnome_popup_menu_attach(menuCom,lookup_widget(mainform,"textCommentaries"),(gchar*)"1");
	GTK_CHECK_MENU_ITEM (lookup_widget(menuCom,"show_tabs1"))->active = settings->showcomtabs;	
		
        //----------------------------------------------------------------------- set dictionary key
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"dictionarySearchText")),settings->dictkey);

        //loadbookmarks(MainFrm); //--------------------------------- add bookmarks to menubar
        loadbookmarks_programstart(); //--------------------------------- add bookmarks to menubar
        changeVerse(settings->currentverse); //---------------------------------------------- set Text
        //----------------------- show hide shortcut bar - set to options setting
        if(settings->showshortcutbar){
                gtk_widget_show(GTK_WIDGET(shortcut_bar));
        }else{
                gtk_widget_hide(GTK_WIDGET(shortcut_bar));
        }

        if(settings->studypadfilename != NULL) loadStudyPadFile(settings->studypadfilename); //-- load last used file into studypad

//-- hide buttons - only show them if their options are enabled
	gtk_widget_hide(lookup_widget(MainFrm,"btnPrint"));
	gtk_widget_hide(lookup_widget(MainFrm,"btnSpell"));
	gtk_widget_hide(lookup_widget(MainFrm,"btnSpellNotes"));

        g_list_free(biblemods);
        g_list_free(commentarymods);
        g_list_free(dictionarymods);
		
#ifdef  USE_GNOMEPRINT                   //-- do not show print button if printing not enabled
        gtk_widget_show(lookup_widget(MainFrm,"btnPrint"));
#endif /* USE_GNOMEPRINT */

#ifdef USE_ASPELL                         //-- do not show spell buttons if spellcheck not enabled
        gtk_widget_show (lookup_widget(MainFrm,"btnSpell"));
        gtk_widget_show (lookup_widget(MainFrm,"btnSpellNotes"));
#endif /* USE_ASPELL */
}

//-------------------------------------------------------------------------------------------
void 
changeVerse(gchar *ref) //-- change main text, interlinear texts and commentary text together
{
	//string keyText; //-- string for verse key text to change to
//	GtkWidget *msgBox;
	int l;


	if((GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active) && noteModified){ //-- save any changes to personal notes		
		if(autoSave){                          //-- if we are in edit mode
			savenoteSWORD(noteModified); 	//-- save if text in note window has changed			
		}
	}		
	strcpy(current_verse,ref);		
	ApplyChange = FALSE;		
	if(curMod){  //--------------------------------------------------- change main window		
		curMod->SetKey(ref); //keyText.c_str());
		curMod->Display();
		gtk_label_set(GTK_LABEL(lookup_widget(MainFrm,"lbText")),curMod->KeyText( )); //------- set text label to current verse
		swKey = curMod->KeyText();
		strcpy(settings->currentverse, curMod->KeyText()); //----------------------- remember last verse
		char s1[255],s2[255];
		curVerse = swKey.Verse();
		curChapter = swKey.Chapter();		
		sprintf(s1,"%s",(const char *)swKey);
		for(l=0;l<strlen(s1);l++){ //------------------------ seperate book name			
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
	if(settings->notebook3page == 2){
		if(comp1Mod){
			comp1Mod->SetKey(swKey);   //-- interlinear1
			comp1Mod->Display();
		}
		if(comp2Mod){
			comp2Mod->SetKey(swKey);  //-- interlinear2
			comp2Mod->Display();
		}		
		if(comp3Mod){
			comp3Mod->SetKey(swKey);  //-- interlinear3
			comp3Mod->Display();
		}
	}
	//---------------------------------------------------------------- change personal notes editor
	if(settings->notebook3page == 1){ 		
		if(GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"tbtnFollow"))->active){ //-- if personal notes follow button is active (on)			
			if((GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active) && (!autoSave)){
					//-- do nothing
			}else{
				if(usepersonalcomments && percomMod){
					percomMod->SetKey(ref); //-- set personal module to current verse
					percomMod->Display();            //-- show change
					noteModified = false; //-- we just loaded comment so it is not modified	
				}
			}
		}
	}
	if(settings->notebook3page == 0 && autoscroll){
		if(curcomMod){	
			curcomMod->SetKey(ref); //keyText.c_str()); //-- set comments module to current verse
			curcomMod->Display(); //-- show change
			strcpy(com_key,ref);
		}
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
	gchar *entryText;   //-- pointer to string to search for
	int i, j, count ;   //-- counters
	gchar *listitem; //-- hold item until added to list
	GtkWidget *list;     //-- list widget to display items found

	list = lookup_widget(MainFrm, "list1"); //-- get pointer to list widget
	entryText = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm, "dictionarySearchText"))); //-- get key to form user
	j=0;
	it = mainMgr->Modules.find(ModName); //-- find module to use for search
	if (it != mainMgr->Modules.end()){ //-- if we don't reach the end of our modules	
		SWModule *mod = (*it).second;  //-- temp module to work with
		mod->KeyText(); //-- snap to entry
		saveKey = mod->KeyText(); //-- save our place
		count = GTK_CLIST(list)->clist_window_height / GTK_CLIST(list)->row_height;	//-- how many items do we need to fill our list	
		gtk_clist_clear( GTK_CLIST(list)); //-- start with empty list
		if(mod){ //-- make sure we have module to work with		
			for (i = 0; i < (count / 2); i++) (*mod)++; //-- get equal number of keys before and after our starting key(saveKey)
			for (i = 0; i < count-1; i++) (*mod)--;
			mod->Error(); //-- clear errors
			for (;!mod->Error() && count;count--,(*mod)++){
				++j;
				listitem = g_strdup((const char *)mod->KeyText()); //-- key to listitem
				gtk_clist_append(GTK_CLIST(list) , &listitem); //-- listitem to list
				g_free(listitem);
				if (saveKey == mod->Key()) //-- if we are back to starting place set index
				index = j-1; 
			}
		}		
		gtk_clist_moveto( GTK_CLIST(list), index, 0, 0.5, 0.0 ); //-- move in list to index
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
	GTK_CHECK_MENU_ITEM (strongsnum)->active = settings->strongs;
	
        if(havedict) FillDictKeys(curdictMod->Name()); //-- fill the dict key clist
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
	if(file_changed){ //-- if study pad file has changed since last save		
		msg = g_strdup_printf(_("``%s'' has been modified.  Do you wish to save it?"), current_filename);
		msgbox = create_InfoBox();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
		g_free (msg);
		switch (answer){		
			case 0: saveFile(current_filename);
				break;
			default:
				break;
		}
	}   	
	
	delete mainMgr;   //-- delete Sword managers
	delete mainMgr1;
	delete mainMgr2;
	delete mainMgr3;	
        if (gbftoplain != 0)  //-- delete Sword render filters
		delete gbftoplain;
	
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
	if(percomDisplay)
		delete percomDisplay;
	if(HTMLDisplay)
		delete HTMLDisplay;	
	if(GBFDisplay)
		delete GBFDisplay;	
	if(noteeditor)
		delete noteeditor;	
	gtk_exit(0);           //-- exit
	
}

//-------------------------------------------------------------------------------------------
void
strongsSWORD(bool choice) //-- toogle strongs numbers for modules that have strongs
{
	if(choice){ //-- if choice is true - we want strongs numbers	
		mainMgr->setGlobalOption("Strong's Numbers","On");  //-- turn strongs on 		
	}else{   //-- we don't want strongs numbers	
		mainMgr->setGlobalOption("Strong's Numbers","Off");	//-- turn strongs off	
	}
	settings->strongs = choice;   //-- store choice in settings
	curMod->Display(); //-- we need to show change
}

//-------------------------------------------------------------------------------------------
void
footnotesSWORD(bool choice) //-- toogle gbf footnotes for modules that have them
{
	if(choice){ //-- we want footnotes	
		mainMgr->setGlobalOption("Footnotes","On"); //-- turn footnotes on
	}else{ //-- we don't want footnotes	
		mainMgr->setGlobalOption("Footnotes","Off");	//-- turn footnotes off	
	}
	settings->footnotes = choice;   //-- store choice in settings
	curMod->Display(); //-- we need to show change
}

//-------------------------------------------------------------------------------------------
void
addBookmark(void)  //-- someone clicked add bookmark to get us here
{
	gchar    *bookname;      //-- pointer to the Bible book we want to mark
	gint     iVerse,         //-- verse we want to mark
		 iChap;          //-- chapter we want to mark
	gchar	 buf[255];

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
	while(i < ibookmarks){  //-- loop through bookmarks - ibookmarks the number of bookmarks we have	 	
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
addHistoryItem(void)  //-- add an item to the history menu
{
	GnomeUIInfo *historyitem;  //-- pointer to gnome menu item structure
	gchar   *bookname,        //-- pointer to name of book of Bible to add to history
		ref[255];            //-- string to store Bible ref
	gint    iVerse,         //-- varible to store verse number
		iChap;               //-- varible to store chapter number

	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook"))); //-- set bookname to book
	                                                                            //-- showing in book combo box
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse"))); //-- get verse number from verse spin button
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")));//-- get chapter number from chapter spin button
	sprintf(ref,"%s %d:%d",bookname, iChap,iVerse ); //-- store book, chapter and verse in ref string
	additemtognomemenu(MainFrm, ref, ref, "_History/<Separator>",(GtkMenuCallback) on_mnuHistoryitem1_activate); //-- add item to history menu
	++historyitems;
}

//-------------------------------------------------------------------------------------------
void
changecurModSWORD(gchar *modName) //-- change sword module for main window
{                                 //-- someone clicked View->MainWindow->module
	ModMap::iterator it;            //-- or clicked the mainwindow popup menu and the callback sent us here

	it = mainMgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != mainMgr->Modules.end()){ //-- if we find the module	
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
	if (it != mainMgr1->Modules.end()){     //-- if we find the module	
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
	if (it != mainMgr2->Modules.end()){    //-- if we find the module	
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
	if (it != mainMgr3->Modules.end()){    //-- if we find the module	
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
	if(choice){
		bVerseStyle = TRUE;  //-- tells chapter display we want verses
	} else {
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
	if(choice){
		gtk_widget_show(intpage);		//-- show page
	} else {
		gtk_widget_hide(intpage);		//-- hide page
	}
	settings->interlinearpage = choice;  //-- remember choice for next program startup
}


//-------------------------------------------------------------------------------------------
void
chapterSWORD(void)  //-- someone clicked the chapter spin button
{
	gint       iChap;
        gchar      *buf;
        
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter"))); //-- set iChap to value in spin button
	swKey.Chapter(iChap); //-- let sword set chapter for us - sword knows when to go to next or previous book - so we don't have to keep up
	buf = g_strdup(swKey); 
	changeVerse(buf);	  //-- change all our modules to new chapter
	g_free(buf);
}

//-------------------------------------------------------------------------------------------
void
verseSWORD(void)  //-- someone clicked the verse spin button
{
	gint       iVerse;
        gchar      *buf;
        
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse"))); //-- set iVerse to value in spin button
	swKey.Verse(iVerse ); //-- let sword set verse for us - sword knows when to go to next or previous chapter - so we don't have to keep up
	buf = g_strdup(swKey);	
	changeVerse(buf);	//-- change all our modules to new verse	
        g_free(buf);
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
	if(event->keyval == 65293 || event->keyval == 65421){  //-- if user hit return key continue	
		changeVerse(buf);	//-- change verse to entry text
		addHistoryItem(); //-- add verse to history menu
	}	
}

//-------------------------------------------------------------------------------------------
void
changcurcomModSWORD(gchar *modName, gint page_num)  //-- someone changed commentary notebook page (sent here by callback function notebook page change)
{
	ModMap::iterator it;
	GtkWidget   *notebook, //-- pointer to commentary notebook
	            *frame;//-- pointer to commentary frame
						
	notebook = lookup_widget(MainFrm,"notebook1"); //-- set notebook to commentary notebook
	frame = lookup_widget(MainFrm,"framecom"); //-- set frame to commentary frame
	settings->notebook1page = page_num; //-- save current page
	it = mainMgr->Modules.find(modName); //-- find commentary module (modName from page label)
	if (it != mainMgr->Modules.end()){	
		curcomMod = (*it).second;  //-- set curcomMod to modName
		if(autoscroll) curcomMod->SetKey(curMod->KeyText()); //-- go to text (verse)
		//else  curcomMod->SetKey(com_key); //-- keep all commentary modules together (as close as we can)
		curcomMod->Display(); //-- show the change
		gtk_frame_set_label( GTK_FRAME(frame),curcomMod->Name()); //-- set frame label
	}	
}

//-------------------------------------------------------------------------------------------
void
navcurcomModSWORD(gint direction)  //-- navigate the current commentary module
{
        switch(direction){   
		case 0: (*curcomMod)--;
			break;
		case 1: (*curcomMod)++;
			break;
        }
        curcomMod->Error(); //-- clear any errors
        curcomMod->Display();
}

//-------------------------------------------------------------------------------------------
void
editnoteSWORD(bool editbuttonactive) //-- someone clicked the note edit button
{
 	if(editbuttonactive){	
		percomMod->Disp(percomDisplay);
		gtk_text_set_editable (GTK_TEXT (lookup_widget(MainFrm,"textComments")), TRUE); //-- set text widget to editable	
		gtk_widget_show(lookup_widget(MainFrm,"sbNotes")); //-- show comments status bar
		noteModified = FALSE;	 //-- we just turned edit mode on no changes yet
        } else {	
		if(settings->formatpercom) percomMod->Disp(FPNDisplay);
		gtk_text_set_editable (GTK_TEXT (lookup_widget(MainFrm,"textComments")), FALSE); //-- set text widget to not editable
		gtk_widget_hide(lookup_widget(MainFrm,"sbNotes"));//-- hide comments status bar
	}
	percomMod->Display(); 	
}

//-------------------------------------------------------------------------------------------
void
savenoteSWORD(bool noteModified) //-- save personal comments
{  	
        if(noteModified){ //-- if note modified save the changes  
		VerseKey mykey; //-- verse key text
		gchar *buf;     //-- pointer to a string 					
		GtkWidget *text; //-- pointer to commentary text widget		
		//text = lookup_widget(MainFrm,"textComments"); //-- get text widget
		buf = gtk_editable_get_chars((GtkEditable *)NEtext,0,-1); //-- get comments from text widget
		*percomMod << (const char *)buf; //-- save note!  		
	}
	noteModified = false; //-- we just saved the note so it has not been modified 	
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
        GtkWidget   *frame;  //-- pointer to dict&lex frame
            						
	frame = lookup_widget(MainFrm,"frame10"); //-- set frame to dict&lex frame
	settings->notebook2page = page_num; //-- save current page
	it = mainMgr->Modules.find(modName);  //-- find module we want to use
	if (it != mainMgr->Modules.end()){	
		curdictMod = (*it).second;  //-- set curdictMod to new choice
		curdictMod->SetKey(keyText.c_str());   //-- set key to text from lookup entry
		curdictMod->Display();	 //-- display new dict
		FillDictKeys(curdictMod->Name()); //-- fill the list widget with keys	
		gtk_frame_set_label( GTK_FRAME(frame),curdictMod->Name()); //-- set frame label
	}
}

//-------------------------------------------------------------------------------------------
void
dictSearchTextChangedSWORD(char* mytext)   //-- dict lookup text changed
{	                        //-- mytext from the dict dict lookup entry
	strcpy(settings->dictkey,mytext); //-- remember the key
	if (curdictMod){ //-- if we have a dict module	
		if(strcmp(mytext,"")){  //-- if text is not null		
			curdictMod->SetKey(mytext); //-- set key to our text
			curdictMod->Display();	//-- show what we found
			FillDictKeys(curdictMod->Name()); //-- fill the list widget with keys	
		}
	}
}

//-------------------------------------------------------------------------------------------
void
dictchangekeySWORD(gint direction)   //-- dict change key up or down -- arrow buttons
{
        gchar   *buf;
        	
        if(direction == 1){   //-- next key  
	        (*curdictMod)++;  //-- move up one
        } else if(direction == 0) {     //-- previous key  
                (*curdictMod)--;  //-- move down one
        }
        curdictMod->Display(); //-- show the changes
	//-- put new key into dictionary text entry
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"dictionarySearchText")), curdictMod->KeyText());
        buf = g_strdup(curdictMod->KeyText());
        dictSearchTextChangedSWORD(buf);
        g_free(buf);
}

//-------------------------------------------------------------------------------------------
void
changepercomModSWORD(gchar* modName)   //-- change personal comments module
{	
	GtkWidget *notebook, //-- pointer to a notebook widget
                *label;    //-- pointer to a label widget
	ModMap::iterator it; //-- module iterator
	
        if(noteModified) return;//savenoteSWORD(noteModified);  //-- if personal comments changed save changes before we change modules and lose our changes
	it = percomMgr->Modules.find(modName); //-- find commentary module (modName from page label)
	if (it != percomMgr->Modules.end()){ //-- if we don't run out of mods before we find the one we are looking for	
		percomMod = (*it).second;  //-- set curcomMod to modName
		strcpy(settings->personalcommentsmod, percomMod->Name());
		percomMod->SetKey(curMod->KeyText()); //-- go to text (verse)
		percomMod->Display(); //-- show the change 	
		//-- let's change the notebook label to match our percomMod (current personal comments module)
  	        notebook = lookup_widget(MainFrm,"notebook3");  //-- get the notebook our page is in]
		label = gtk_label_new (percomMod->Name());   //-- create new label with mod name as the text
		gtk_widget_show (label);   //-- make is visible
		gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label); //-- put label on personal comments page
		noteModified = false;
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
	if(choice){ //-- if choice was to autosave	
		autoSave = true;
	} else {     //-- if choice was not to autosave	
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
        if(current_filename != NULL){
  	        gchar buf[255];  	
        	sprintf(buf,"lpr %s",current_filename);
        }
}

//-------------------------------------------------------------------------------------------
void
openpropertiesbox(void)    //-- someone clicked properties
{
	GtkWidget   *Propertybox,  //-- pointer to propertybox dialog
                *cpcurrentverse,  //-- pointer to current verse color picker
                *comtabsbutton,  //-- show commentary notebook tabs toggle button
                *dicttabsbutton, //-- show dict/lex notebook tabs toggle button
                *shortcutbarbutton, //-- show shortcut bar toggle button
                *textgroupbutton,   //-- show text group toggle button
                *comgroupbutton,    //-- show commentary group toggle button
                *dictgroupbutton;   //-- show dict/lex group toggle button

	gushort red,       //-- vars for setting color
		green,
		blue,
		a;
	
	Propertybox = create_dlgSettings(); //-- create propertybox dialog
	shortcutbarbutton = lookup_widget(Propertybox,"cbtnShowSCB");
	comtabsbutton = lookup_widget(Propertybox,"cbtnShowCOMtabs");
	dicttabsbutton  = lookup_widget(Propertybox,"cbtnShowDLtabs");
	cpcurrentverse = lookup_widget(Propertybox,"cpfgCurrentverse"); //-- set cpcurrentverse to point to color picker
        textgroupbutton = lookup_widget(Propertybox,"cbtnShowTextgroup");
	//comgroupbutton  = lookup_widget(Propertybox,"cbtnShowComGroup");
	//dictgroupbutton = lookup_widget(Propertybox,"cbtnShowDictGroup");

        a = 000000;
        //-- setup current verse color picker
	red = settings->currentverse_red;  //-- get color from settings structure
	green = settings->currentverse_green;
	blue =settings->currentverse_blue;
	gnome_color_picker_set_i16 (GNOME_COLOR_PICKER(cpcurrentverse),red ,green , blue, a); //-- set color of current verse color picker button
	//-- set toggle buttons to settings structur
	GTK_TOGGLE_BUTTON(shortcutbarbutton)->active = settings->showshortcutbar;
	GTK_TOGGLE_BUTTON(comtabsbutton)->active = settings->showcomtabs;
	GTK_TOGGLE_BUTTON(dicttabsbutton)->active = settings->showdicttabs;
	GTK_TOGGLE_BUTTON(textgroupbutton)->active = settings->showtextgroup;
	/*GTK_TOGGLE_BUTTON(comgroupbutton)->active = settings->showcomgroup;
	GTK_TOGGLE_BUTTON(dictgroupbutton)->active = settings->showdictgroup;*/
		
	GTK_TOGGLE_BUTTON(GTK_BUTTON(lookup_widget(Propertybox,"cbtnPNformat")))->active = settings->formatpercom; //-- set Personal note format check button	
	gtk_widget_show(Propertybox); //-- show propertybox
}

//-------------------------------------------------------------------------------------------
void
changepagenotebook(GtkNotebook *notebook,gint page_num) //-- someone changed the page in the main notebook
{
        settings->notebook3page = page_num; //-- store the page number so we can open to it the next time we start
        changeVerse(current_verse);
}

//-------------------------------------------------------------------------------------------
void
showmoduleinfoSWORD(char *modName) //--  show module information in an about dialog
{
	GtkWidget *aboutbox,  //-- pointer to about dialog
 		*text,       //-- pointer to text widget of dialog
    		*label;     //-- pointer to label in dialog
	char 	*buf,       //-- pointer to text buffer for label (mod name)
 		*bufabout;  //-- pointer to text buffer for text widget (mod about)
        ModMap::iterator it; //-- module iterator
	SectionMap::iterator sit; //--
	ConfigEntMap::iterator cit; //--
	
	it = mainMgr->Modules.find(modName); //-- find module (modName)
	if (it != mainMgr->Modules.end()){ //-- if we don't run out of mods before we find the one we are looking for	
	        buf = (char *)(*it).second->Description();  //-- get discription of module
	        sit = mainMgr->config->Sections.find((*it).second->Name());
	        if (sit !=mainMgr->config->Sections.end()){
	                cit = (*sit).second.find("About");
			if (cit != (*sit).second.end()) 				
				bufabout = (char *)(*cit).second.c_str(); //-- get module about information
				cout << bufabout << '\n';
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
	char 	*buf,       //-- pointer to text buffer for label (mod name)
 	        *bufabout;  //-- pointer to text buffer for text widget (mod about)
        ModMap::iterator it; //-- module iterator
	SectionMap::iterator sit; //--
	ConfigEntMap::iterator cit; //--
	
	it = mainMgr->Modules.find(curMod->Name()); //-- find module (modName)
	if (it != mainMgr->Modules.end()){ //-- if we don't run out of mods before we find the one we are looking for	
	        buf = (char *)(*it).second->Description();  //-- get discription of module
	        sit = mainMgr->config->Sections.find((*it).second->Name());
	        if (sit !=mainMgr->config->Sections.end()){
	        	cit = (*sit).second.find("About");
			if (cit != (*sit).second.end()) 				
			        bufabout = (char *)(*cit).second.c_str(); //-- get module about information
	        }
	}	
	gtk_label_set_text( GTK_LABEL(label),buf);  //-- set label to module discription
	gtk_text_set_word_wrap(GTK_TEXT(text), TRUE ); //-- set word wrap to true for text widget
	AboutModsDisplay(text, bufabout) ; //-- send about info and text widget to display function (display.cpp)
}

//-------------------------------------------------------------------------------------------
void
newSP(GtkWidget *text) //--  start new file in studypad
{
        GtkWidget *statusbar;

        current_filename = NULL;
        gtk_text_set_point(GTK_TEXT(text), 0);
	gtk_text_forward_delete (GTK_TEXT (text), gtk_text_get_length((GTK_TEXT(text))));
	statusbar = lookup_widget((text),"statusbar2");
	gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, "-untitled-");
	file_changed = false;	
}

//-------------------------------------------------------------------------------------------
void
setformatoption(GtkWidget *button)
{
   settings->formatpercom = GTK_TOGGLE_BUTTON(GTK_BUTTON(button))->active;
   /*if(settings->formatpercom){
   		//percomMod->Disp(FPNDisplay);   		
   }*/
}

//-------------------------------------------------------------------------------------------
/*		return verse number form verse in main Bible window
			starting index must be in the verse number
			else we return 0			
*/
gint
getversenumber(GtkWidget *text)
{
	gchar   *buf, //-- buffer for storing the verse number
	        cbuf; //-- char for checking for numbers (isdigit)
	        //tmpbuf[256];
	gint	startindex, //-- first digit in verse number
	        endindex,   //-- last digit in verse number
	        index;      //-- current position in text widget
						
	 index = gtk_editable_get_position(GTK_EDITABLE(text));	//-- get current position for a starting point
	 cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), index); //-- get char at current position (index)
	 if(!isdigit(cbuf)) return 0; //-- if cbuf is not a number stop - do no more
	 endindex = index;  //-- set endindex to index
	 while(isdigit(cbuf)){ //-- loop until cbuf is not a number	 
	 	cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), endindex); //-- get next char
	 	if(cbuf == ')' || cbuf == '>') isstrongs = true;
	 	++endindex;   //-- increment endindex
	 } 	
	 --endindex; //-- our last char was not a number so back up one
	 startindex = index; //-- set startindex to index
	 cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), startindex); //-- get char at index - we know it is a number
	 while(isdigit(cbuf)){  //-- loop backward util cbuf is not a number	 
	        cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), startindex); //-- get previous char
	 	if(cbuf == '(' || cbuf == '<') isstrongs = true;
	 	--startindex; //-- decrement startindex  	 		
	 }	
	 ++startindex; //-- last char (cbuf) was not a number
	 ++startindex; //-- last char (cbuf) was not a number
	 buf = gtk_editable_get_chars(GTK_EDITABLE(text), startindex, endindex); //-- get verse number
	 //cout << buf << '\n';
	 return atoi(buf); //-- send it back as an integer
}

//---------------------------------------------------------------------------------------------
void
sbchangeModSword(gint group_num, gint item_num)
{
        GtkWidget       *notebook;
        gint            num = 0;

        if(groupnum3 == 2) num = group_num;
        else if( groupnum3 == 1)num = group_num + 1;
        else if( groupnum3 == 0)num = group_num + 2;

        switch(num){
                case 0: changecurModSWORD(groupitems[0][item_num]);
                        break;
                case 1: if(havecomm) { //-- let's don't do this if we don't have at least one commentary	           			            	
		                notebook = lookup_widget(MainFrm,"notebook1"); //-- get notebook
		                gtk_notebook_set_page(GTK_NOTEBOOK(notebook), item_num); //-- set notebook page
                        }
                        break;
                case 2: if(havedict) { //-- let's don't do this if we don't have at least one dictionary / lexicon	           			            	
		                notebook = lookup_widget(MainFrm,"notebook4"); //-- get notebook
		                gtk_notebook_set_page(GTK_NOTEBOOK(notebook), item_num); //-- set notebook page
                        }	
                        break;
                case 3:
                        break;
        }

}

//---------------------------------------------------------------------------------------------
void
applyoptions(bool showshortcut, bool showcomtabs, bool showdicttabs, bool showtextgroup,
                bool showcomgroup, bool showdictgroup)
{
         GtkWidget    *dict,
                      *comm;

        dict = lookup_widget(MainFrm,"notebook4");
        comm = lookup_widget(MainFrm,"notebook1");

        settings->showshortcutbar = showshortcut;
        settings->showcomtabs = showcomtabs;
        settings->showdicttabs = showdicttabs;

        settings->showtextgroup = showtextgroup;
        settings->showcomgroup =  showcomgroup;
        settings->showdictgroup = showdictgroup;

        if(settings->showshortcutbar){
                gtk_widget_show(shortcut_bar);
        } else {
                gtk_widget_hide(shortcut_bar);
        }
        if(settings->showcomtabs) {
                gtk_widget_show(comm);
        } else {
                gtk_widget_hide(comm);
        }
        if(settings->showdicttabs) {
                gtk_widget_show(dict);
        } else {
                gtk_widget_hide(dict);
        }
}

//---------------------------------------------------------------------------------------------
void
add_test_group (EShortcutBar *shortcut_bar,gchar *group_name)
{
	gint group_num;
	
	group_num = e_shortcut_model_add_group (shortcut_bar->model, -1, group_name); 	
        e_shortcut_bar_set_view_type (shortcut_bar, group_num, E_ICON_BAR_SMALL_ICONS);
}
//---------------------------------------------------------------------------------------------
void
lookupStrongsSWORD(gint theNumber) //-- theNumber - strongs number was double clicked selected and
                                    //-- sent here
{
        GtkWidget       *notebook, //-- pointer to dict/lex notebook (we use the notebook even if it is not showing)
                        *entry;    //-- pointer to dictionarySearchText entry
        gint            pagenum;  //-- temp storage for notebook page num
        gchar           buf[40]; //-- char string to put our strongs number in
        char            a;        //-- char to store testament (old - 001 or new - 002)

        VerseKey *key = (VerseKey *)(SWKey *)(*curMod); //-- get a versekey form the current text module
                                                    //-- so we can find which testament we are using
        a = key->Testament();  //-- find out if we are in old or new testament
        if(a == 001) pagenum = hebrewpage; //-- if old testament use hebrew lex
        else pagenum = greekpage;          //-- if new testament use greek lex
        if(havedict){ //-- let's don't do this if we don't have at least one dictionary / lexicon    			            	
                notebook = lookup_widget(MainFrm,"notebook4"); //-- get notebook
                gtk_notebook_set_page(GTK_NOTEBOOK(notebook), pagenum); //-- set notebook page
                entry = lookup_widget(MainFrm,"dictionarySearchText"); //-- get the entry so we can send it the new key
                sprintf(buf,"%d",theNumber); //-- put the number into a string
                gtk_entry_set_text(GTK_ENTRY(entry),buf); //-- put key string into the dict entry (which will cause a on_dictionarySearchText_changed event)
        }
}