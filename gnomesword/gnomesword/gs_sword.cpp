/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
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
#include <plainhtml.h>

#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>

#if USE_SHORTCUTBAR
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#endif /* USE_SHORTCUTBAR */

#include "gs_rwphtml.h"
#include "gs_thmlhtml.h"
#include "gs_thmlplain.h"
#include "gs_gbfhtml.h"
#include "gs_gnomesword.h"
#include "gs_history.h"
#include "display.h"
#include "callback.h"
#include "gs_sword.h"
#include "support.h"
#include "interface.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "gs_listeditor.h"
#include "noteeditor.h"

/***********************************************************************************************
Sword global to this file
***********************************************************************************************/
SWDisplay *chapDisplay; /* to display modules using GtkText a chapter at a time */
SWDisplay *entryDisplay; /* to display modules using GtkText a verse at a time */
SWDisplay *comp1Display; /* to display modules using GtkText a verse at a time */
SWDisplay *comp2Display; /* to display modules using GtkText a verse at a time */
SWDisplay *comp3Display; /* to display modules using GtkText a verse at a time */
SWDisplay *comDisplay; /* to display modules using GtkText a verse at a time */
SWDisplay *percomDisplay; /* to display personal comment modules using GtkText a verse at a time */
SWDisplay *dictDisplay; /* to display modules using GtkText a verse at a time */
SWDisplay *FPNDisplay; /* to display formatted personal notes using GtkText */
SWDisplay *HTMLDisplay; /* to display formatted html */
SWDisplay *HTMLchapDisplay; /* to display formatted html */
SWDisplay *listDisplay;	/* to display modules in list editor */
SWDisplay *SDDisplay;	/* to display modules in view dict dialog */

SWMgr *mainMgr; /* sword mgr for curMod - curcomMod - curdictMod */
SWMgr *mainMgr1; /* sword mgr for comp1Mod - first interlinear module */
SWMgr *mainMgr2; /* sword mgr for comp2Mod - second interlinear module */
SWMgr *mainMgr3; /* sword mgr for comp3Mod - third interlinear module */
SWMgr *percomMgr; /* sword mgr for percomMod - personal comments editor */
SWMgr *listMgr;	/* sword mgr for ListEditor */
SWMgr *SDMgr;	/* sword mgr for view dict dialog */

VerseKey swKey = "Romans 8:28";	/* temp storage for verse keys */

SWModule *curMod; /* module for main text window */
SWModule *comp1Mod; /* module for first interlinear window */
SWModule *comp2Mod; /* module for second interlinear window */	
SWModule *comp3Mod; /* module for third interlinear window */
SWModule *curcomMod; /* module for commentary  window */	
SWModule *percomMod; /* module for personal commentary  window */	
SWModule *curdictMod; /* module for dict window */
SWModule *listMod;   /* module for ListEditor */
SWModule *SDMod;   /* module for view dict dialog */

SWFilter *gbftohtml;
SWFilter *plaintohtml;
SWFilter *thmltohtml;
SWFilter *thmltoplain;
SWFilter *rwphtml;
/***********************************************************************************************
GnomeSword global to this file
***********************************************************************************************/ 	
GList 	*biblemods,
	*commentarymods,
	*dictionarymods,
	*percommods;
GtkWidget *NEtext;  /* note edit widget */
gboolean ApplyChange = true;	/* should we make changes when cbBook is changed */
gboolean bVerseStyle = true;	/* should we show verses or paragraphs in main text window */
gint curChapter = 8;	/* keep up with current chapter */
gint curVerse =28;	/* keep up with current verse */
gboolean noteModified = false; /* set to true is personal note has changed */
gboolean usepersonalcomments = false; /* do we setup for personal comments - default is false
                                         will change to true if we find any personal modules  */
gboolean autoSave = true; /* we want to auto save changes to personal comments */
gboolean havebible = false; /* let us know if we have at least one bibletext module */
gboolean havedict = false; /* let us know if we have at least one lex-dict module */
gboolean havecomm = false; /* let us know if we have at least one commentary module */
gboolean autoscroll = true; /* commentary module auto scroll when true -- in sync with main text window */

gint answer; /* do we want to save studybad file on shutdown */
gchar com_key[80] ="Rom 8:28"; /* current commentary key */
gint    groupnum1 = 0,
        groupnum2 = 0,
        groupnum3 = 0,
        groupnum4 = 0,
        greekpage = 0,
        hebrewpage = 0;
gchar *textmod,
        *commod,
        *dictmod;
GtkWidget *MainFrm; /* main form widget  */

/***********************************************************************************************
 externals
***********************************************************************************************/
extern gboolean changemain; /* change verse of Bible text window */
extern NoteEditor *noteeditor; /* we need this to set up sword display */
extern gint 	dictpages, /* number of dictionaries */
	 	compages;  /* number of commentaries */
extern gboolean file_changed;	/* set to true if text is study pad has changed - and file is not saved */
extern SETTINGS *settings;
extern SETTINGS myset;
extern GtkWidget *shortcut_bar;
#if USE_SHORTCUTBAR
extern gchar *shortcut_types[];
#endif /* USE_SHORTCUTBAR */
extern gchar *current_filename;	/* filename for open file in study pad window  */
extern gchar current_verse[80];	/* current verse showing in main window - 1st - 2nd - 3rd
				   interlinear window - commentary window */
extern gint historyitems;      /* number of history items */
extern HISTORY historylist[];  /* sturcture for storing history items */
extern gboolean addhistoryitem; /* do we need to add item to history */
extern gchar *mycolor;
/***********************************************************************************************
 *initSwrod to setup all the Sword stuff
 *mainform - sent here by main.cpp
 ***********************************************************************************************/
void
initSWORD(GtkWidget *mainform)
{
	ModMap::iterator it; //-- iteratior
	SectionMap::iterator sit; //-- iteratior
	ConfigEntMap::iterator cit; //-- iteratior
	int   	i, //-- counter
		j; //-- counter					
   	gchar *sourceformat;
	GnomeUIInfo *menuitem; //--  gnome menuitem
  	GtkWidget *menu_items;
  	
  	gbftohtml	= new GS_GBFHTML(); //-- sword renderfilter gbf to html
  	plaintohtml   	= new PLAINHTML(); //-- sword renderfilter plain to html
  	thmltohtml	= new GS_ThMLHTML(); /* sword renderfilter thml to html */
        thmltoplain	= new GS_ThMLPlain(); /* sword renderfilter thml to plain */  	
        rwphtml		= new GS_RWPHTML();

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
	percomDisplay   = 0;// set in create
	FPNDisplay			= 0;
	HTMLDisplay			= 0;
	HTMLchapDisplay			= 0;
	biblemods = NULL;
	commentarymods = NULL;
	dictionarymods = NULL;
	percommods = NULL;
	MainFrm = lookup_widget(mainform,"mainwindow"); //-- save mainform for use latter
	NEtext =  lookup_widget(mainform,"textComments"); //-- get note edit widget
	mycolor = settings->currentverse_color;
#if USE_SHORTCUTBAR	
  	//-- create shortcut bar groups
	if(settings->showtextgroup){
	    	groupnum1 = add_sb_group((EShortcutBar *)shortcut_bar, "Bible Text");
	}
	if(settings->showcomgroup){
	    	groupnum2 = add_sb_group((EShortcutBar *)shortcut_bar, "Commentaries");
	}
	if(settings->showdictgroup){
		groupnum3 = add_sb_group((EShortcutBar *)shortcut_bar, "Dict/Lex");
	}
	if(settings->showhistorygroup){
		groupnum4 = add_sb_group((EShortcutBar *)shortcut_bar, "History");	
	}
#endif /* USE_SHORTCUTBAR */		
        //-- setup displays for sword modules
    	noteeditor = new NoteEditor();
	GTKEntryDisp::__initialize();
	chapDisplay = new GTKChapDisp(lookup_widget(mainform,"moduleText"));
	dictDisplay = new GTKEntryDisp(lookup_widget(mainform,"textDict"));
	comDisplay = new  GTKEntryDisp(lookup_widget(mainform,"textCommentaries"));
	percomDisplay = new  GTKPerComDisp(lookup_widget(mainform,"textComments"));
	comp1Display = new ComEntryDisp(lookup_widget(mainform,"textComp1"));
	comp2Display = new ComEntryDisp(lookup_widget(mainform,"textComp2"));
	comp3Display = new ComEntryDisp(lookup_widget(mainform,"textComp3"));
	//RWPDisplay = new GTKRWPDisp(lookup_widget(mainform,"textCommentaries")); 	
	FPNDisplay = new ComEntryDisp(lookup_widget(mainform,"htmlComments"));
	HTMLDisplay = new ComEntryDisp(lookup_widget(mainform,"htmlCommentaries"));

	HTMLchapDisplay = new GTKhtmlChapDisp(lookup_widget(mainform,"htmlTexts"));
	compages = 0;
	dictpages = 0;
	for(it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			curMod = (*it).second;
			havebible = TRUE;
			biblemods = g_list_append(biblemods,curMod->Name());
			sit = mainMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters
	        	if(sit !=mainMgr->config->Sections.end()){
	    	    		cit = (*sit).second.find("SourceType");
				if(cit != (*sit).second.end()) sourceformat = (char *)(*cit).second.c_str();
				else sourceformat = "Plain";
				//g_warning(sourceformat);
			}
			if(!strcmp(sourceformat, "GBF")){ //-- we need gbf to html filter			
				curMod->AddRenderFilter(gbftohtml);
				curMod->Disp(HTMLchapDisplay);
			}else if(!strcmp(sourceformat, "Plain")){ //-- we need gbf to html filter			
			  	curMod->AddRenderFilter(plaintohtml);
				curMod->Disp(HTMLchapDisplay);
			} /*else			
				curMod->Disp(chapDisplay); */
#if USE_SHORTCUTBAR			
		  	//--  add choice to shortcut bar
		  	if(settings->showtextgroup){
		    		e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
						      groupnum1, -1,
						      shortcut_types[0],
						      curMod->Name());
		  	}
#endif /* USE_SHORTCUTBAR */	
	  			
		}else if (!strcmp((*it).second->Type(), "Commentaries")){    //-- set commentary modules and add to notebook		
			curcomMod = (*it).second;
			commentarymods = g_list_append(commentarymods,curcomMod->Name());
			havecomm = TRUE; //-- we have at least one commentay module
			++compages; //-- how many pages do we have  								
			sit = mainMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters
	    		if (sit !=mainMgr->config->Sections.end()){
	    			cit = (*sit).second.find("SourceType");
				if (cit != (*sit).second.end())	sourceformat = (char *)(*cit).second.c_str();
				else sourceformat ="Plain";
			}			
			if (!strcmp(sourceformat, "GBF")){ //-- we need gbf to html filter			
				curcomMod->AddRenderFilter(gbftohtml);
				curcomMod->Disp(HTMLDisplay);
			} else if(!strcmp(sourceformat,"ThML")) {
				curcomMod->AddRenderFilter(thmltohtml);
				curcomMod->Disp(HTMLDisplay);				
			} else if(!strcmp(sourceformat,"HTML")) {
			        //curcomMod->AddRenderFilter(thmltoplain);
				curcomMod->Disp(HTMLDisplay);			
			} else if(!strcmp(sourceformat,"Plain")&& strcmp(curcomMod->Name(),"RWP")) {
			        curcomMod->AddRenderFilter(plaintohtml);
				curcomMod->Disp(HTMLDisplay);
			} else if(!strcmp(curcomMod->Name(),"RWP")){   			
			        curcomMod->AddRenderFilter(rwphtml);
				curcomMod->Disp(HTMLDisplay);	//RWPDisplaycomDisplay		
			/* if driver is RawFiles - personal notes*/
			}else if((*mainMgr->config->Sections[(*it).second->Name()].find("ModDrv")).second == "RawFiles"){
				 if(settings->formatpercom) curcomMod->Disp(HTMLDisplay);
				 else curcomMod->Disp(comDisplay);
			}else curcomMod->Disp(comDisplay);
#if USE_SHORTCUTBAR			
			if(settings->showcomgroup){
			    e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
						      groupnum2, -1,
						      shortcut_types[1],
						      curcomMod->Name());
			}
#endif /* USE_SHORTCUTBAR */

		}else if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){ //-- set dictionary modules and add to notebook	
			havedict = TRUE; //-- we have at least one lex / dict module
			++dictpages; //-- how many pages do we have
			curdictMod = (*it).second;
			dictionarymods = g_list_append(dictionarymods,curdictMod->Name());			
			curdictMod->Disp(dictDisplay);
#if USE_SHORTCUTBAR			
			if(settings->showdictgroup){			
			        e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
						      groupnum3, -1,
						      shortcut_types[2],
						      curdictMod->Name());
			}
#endif /* USE_SHORTCUTBAR */
		}
	} 		
  	//-- set up percom editor module
	for (it = percomMgr->Modules.begin(); it != percomMgr->Modules.end(); it++){
		if (!strcmp((*it).second->Type(), "Commentaries")){ //-- if type is Commentaries
		 	 //-- if driver is RawFiles			
			if((*percomMgr->config->Sections[(*it).second->Name()].find("ModDrv")).second == "RawFiles"){ 
				 percomMod = (*it).second;
				 if(settings->formatpercom) percomMod->Disp(FPNDisplay);  //-- if TRUE use formatted display
				 else percomMod->Disp(percomDisplay);                     //-- else standard display
				 percommods = g_list_append(percommods,percomMod->Name());
				 usepersonalcomments = TRUE; //-- used by verseChange function (GnomeSword.cpp)
				 percomMod->SetKey(settings->currentverse);
				 gtk_widget_show(lookup_widget(MainFrm,"vbox2")); //-- show personal comments page because we
			} 	                                                 //-- have at least one personl module
	  	}
	}	
	//-- interlinear 1
	
	for (it = mainMgr1->Modules.begin(); it != mainMgr1->Modules.end(); it++){	
		comp1Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")){
		        sit = mainMgr1->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters
	        	if(sit !=mainMgr1->config->Sections.end()){
	    	    		cit = (*sit).second.find("SourceType");
				if(cit != (*sit).second.end()) sourceformat = (char *)(*cit).second.c_str();
				else sourceformat = "Plain";
				//g_warning(sourceformat);
			}
			if(!strcmp(sourceformat, "GBF")){ //-- we need gbf to html filter			
				comp1Mod->AddRenderFilter(gbftohtml);
				comp1Mod->Disp(comp1Display);
			}else if(!strcmp(sourceformat, "Plain")){ //-- we need gbf to html filter			
			  	comp1Mod->AddRenderFilter(plaintohtml);
				comp1Mod->Disp(comp1Display);
			}
		}
	}
	//-- interlinear 2	
	for (it = mainMgr2->Modules.begin(); it != mainMgr2->Modules.end(); it++){
		comp2Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")){						
		
			if(!strcmp(sourceformat, "GBF")){ //-- we need gbf to html filter			
				comp2Mod->AddRenderFilter(gbftohtml);
				comp2Mod->Disp(comp2Display);
			}else if(!strcmp(sourceformat, "Plain")){ //-- we need gbf to html filter			
			  	comp2Mod->AddRenderFilter(plaintohtml);
				comp2Mod->Disp(comp2Display);
			}	
		}
	}
	//-- interlinear 3	
	for (it = mainMgr3->Modules.begin(); it != mainMgr3->Modules.end(); it++){
		comp3Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")){
		
			if(!strcmp(sourceformat, "GBF")){ //-- we need gbf to html filter			
				comp3Mod->AddRenderFilter(gbftohtml);
				comp3Mod->Disp(comp3Display);
			}else if(!strcmp(sourceformat, "Plain")){ //-- we need gbf to html filter			
			  	comp3Mod->AddRenderFilter(plaintohtml);
				comp3Mod->Disp(comp3Display);
			}	
		}
	}	
}


/********************************************************************************************** 
 * changeVerse - this function changes all currently used sword Bible and commentary modules 
 *               to (ref)
 * gchar *ref - pointer to new module key (verse)
 *********************************************************************************************/
void 
changeVerseSWORD(gchar *ref) //-- change main text, interlinear texts and commentary text together
{
	int l;

	if((GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active) && noteModified){ //-- save any changes to personal notes		
		if(autoSave){                          //-- if we are in edit mode
			savenoteSWORD(noteModified); 	//-- save if text in note window has changed			
		}
	}
	 		
	strcpy(current_verse,ref);		
	ApplyChange = false;
	if(changemain && havebible) {
		if(curMod){  //--------------------------------------------------- change main window
		
			curMod->SetKey(ref);
			curMod->Display();			
			swKey = curMod->KeyText();
			if(addhistoryitem){
			        if(strcmp(settings->currentverse,historylist[historyitems-1].verseref))
			                addHistoryItem(MainFrm,
			                        GTK_WIDGET(shortcut_bar),
			                        settings->currentverse);			
			}
			addhistoryitem = TRUE;
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
			ApplyChange = false;
			gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeBook")),s2);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter")),curChapter);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbVerse")),curVerse);
			gtk_entry_set_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeFreeformLookup")),swKey);
		}
	}
	changemain = TRUE;
	//--------------------------------------------------------------- change interlinear verses
	if(settings->notebook3page == 2){
		//interlinearDisplay(mods);
		if(comp1Mod){
			comp1Mod->SetKey(current_verse);   //-- interlinear1
			comp1Mod->Display();
		}
		if(comp2Mod){
			comp2Mod->SetKey(current_verse);  //-- interlinear2
			comp2Mod->Display();
		}		
		if(comp3Mod){
			comp3Mod->SetKey(current_verse);  //-- interlinear3
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
					percomMod->SetKey(current_verse); //-- set personal module to current verse
					percomMod->Display();            //-- show change
					noteModified = false; //-- we just loaded comment so it is not modified	
				}
			}
		}
	}
	if(settings->notebook3page == 0 && autoscroll){
		if(curcomMod){	
			curcomMod->SetKey(current_verse); //keyText.c_str()); //-- set comments module to current verse
			curcomMod->Display(); //-- show change
			strcpy(com_key,swKey);
			//g_warning(current_verse);
		}
	}			
	ApplyChange = TRUE;	
}

//-------------------------------------------------------------------------------------------
void 
FillDictKeysSWORD(void)  //-- fill clist with dictionary keys -
{                            //-- number of keys depends on hight of list widget and size of font
	ModMap::iterator it; //-- iterator to go through modules and find modName
	int index = 0;      //-- for index into list widget
	SWKey saveKey;      //-- for starting point
	gchar *entryText;   //-- pointer to string to search for
	int i, j, count ;   //-- counters
	gchar *listitem; //-- hold item until added to list
	GtkWidget *list;     //-- list widget to display items found

	list = lookup_widget(MainFrm, "list1"); //-- get pointer to list widget
	entryText = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm, "dictionarySearchText"))); //-- get key to use form user
	j=0;
	it = mainMgr->Modules.find(curdictMod->Name()); //-- find module to use for search
	if (it != mainMgr->Modules.end()){ //-- if we dont reach the end of our modules	
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
shutdownSDSWORD(void)  //-- close down GnomeSword program
{
	
	delete SDMgr;	
	if(SDDisplay)
		delete SDDisplay;		

}


//-------------------------------------------------------------------------------------------
void 
shutdownSWORD(void)  //-- close down GnomeSword program
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

        if (thmltohtml != 0)
		delete thmltohtml;
	if (gbftohtml != 0)
		delete gbftohtml;
	if (plaintohtml != 0)
		delete plaintohtml;	
	if (chapDisplay)    //-- delete Sword displays
		delete chapDisplay;
	if (entryDisplay)
		delete entryDisplay;	
	if(comp1Display)
		delete comp1Display;
	if(comp2Display)
		delete comp2Display;
	if(comp3Display)
		delete comp3Display;	
	if(comDisplay)
		delete comDisplay;
	if(dictDisplay)
		delete dictDisplay;
	if(percomDisplay)
		delete percomDisplay;
	if(FPNDisplay)
		delete FPNDisplay;		
	if(HTMLDisplay)
		delete HTMLDisplay;
	if(HTMLchapDisplay)
		delete HTMLchapDisplay;	
	if(noteeditor)
		delete noteeditor;	
	gtk_exit(0);           //-- exit	
}

//-------------------------------------------------------------------------------------------
void
strongsSWORD(gboolean choice) //-- toogle strongs numbers for modules that have strongs
{
	if(choice){ //-- if choice is TRUE - we want strongs numbers	
		mainMgr->setGlobalOption("Strong's Numbers","On");  //-- turn strongs on 		
	}else{   //-- we don't want strongs numbers	
		mainMgr->setGlobalOption("Strong's Numbers","Off");	//-- turn strongs off	
	}
	settings->strongs = choice;   //-- store choice in settings
	if(havebible) curMod->Display(); //-- we need to show change
}

//-------------------------------------------------------------------------------------------
void
footnotesSWORD(gboolean choice) //-- toogle gbf footnotes for modules that have them
{
	if(choice){ //-- we want footnotes	
		mainMgr->setGlobalOption("Footnotes","On"); //-- turn footnotes on
	}else{ //-- we don't want footnotes	
		mainMgr->setGlobalOption("Footnotes","Off");	//-- turn footnotes off	
	}
	settings->footnotes = choice;   //-- store choice in settings
	if(havebible) curMod->Display(); //-- we need to show change
}

//-------------------------------------------------------------------------------------------
void
changecurModSWORD(gchar *modName, gboolean showchange) //-- change sword module for main window
{                                 //-- someone clicked View->MainWindow->module
	ModMap::iterator it;            //-- or clicked the mainwindow popup menu and the callback sent us here
        if(havebible) {
	        it = mainMgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	        if (it != mainMgr->Modules.end()){ //-- if we find the module	
		        curMod = (*it).second;  //-- change current module to new module
		        if(showchange) {
		                curMod->SetKey(current_verse); //-- set key to current verse
		                curMod->Display();            //-- show it to the world
		        }
	        }
	        strcpy(settings->MainWindowModule, curMod->Name()); //-- remember where we are so we can open here next time we startup
	
	        gtk_frame_set_label( GTK_FRAME(lookup_widget(MainFrm,"frame9")), //-- set main window frame label of
                                 curMod->Name());                  //-- to current Module name
        }
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
setversestyleSWORD(gboolean choice)  //-- set verse style -- verses or paragraphs
{
	if(choice){
		bVerseStyle = TRUE;  //-- tells chapter display we want verses
	} else {
		bVerseStyle = false;     //-- tells chapter display we want paragraphs
	}
	settings->versestyle = choice; //-- remember our choice for the next program startup
 	if(havebible) curMod->Display(); //-- show the change
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
	changeVerseSWORD(buf);	  //-- change all our modules to new chapter
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
	
        changeVerseSWORD(buf);	//-- change all our modules to new verse		
        g_free(buf);
}

//-------------------------------------------------------------------------------------------
void
btnlookupSWORD(void)  //-- add current verse to history menu
{
	gchar *buf; //-- pointer to entry string

	ApplyChange = false;	//-- do not want to start loop with book combo box
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeFreeformLookup")));	//-- set pointer to entry text

	changeVerseSWORD(buf);	//-- change verse to entry text	
	
}

//-------------------------------------------------------------------------------------------
void
freeformlookupSWORD(GdkEventKey  *event) //-- change to verse in freeformlookup entry
{
 	gchar *buf; //-- pointer to entry string

	ApplyChange = false;	//-- do not want to start loop with book combo box
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(MainFrm,"cbeFreeformLookup")));	//-- set pointer to entry text
	if(event->keyval == 65293 || event->keyval == 65421){  //-- if user hit return key continue

		changeVerseSWORD(buf);	//-- change verse to entry text
		
	}	
}

//-------------------------------------------------------------------------------------------
void
changcurcomModSWORD(gchar *modName, gint page_num, gboolean showchange)  //-- someone changed commentary notebook page (sent here by callback function notebook page change)
{
	ModMap::iterator it;
	GtkWidget *frame;//-- pointer to commentary frame *notebook, //-- pointer to commentary notebook
	if(havebible) {			
	        //notebook = lookup_widget(MainFrm,"notebook1"); //-- set notebook pointer to commentary notebook
	        frame = lookup_widget(MainFrm,"framecom"); //-- set frame to commentary frame
	        settings->notebook1page = page_num; //-- save current page
	        //g_print("page=%d\n",settings->notebook1page);
	        it = mainMgr->Modules.find(modName); //-- find commentary module (modName from page label)
	        if (it != mainMgr->Modules.end()){	
		        curcomMod = (*it).second;  //-- set curcomMod to modName
		        if(showchange) {
		                if(autoscroll) curcomMod->SetKey(curMod->KeyText()); //-- go to text (verse)
		                curcomMod->Display(); //-- show the change
		        }
		        //gtk_frame_set_label( GTK_FRAME(frame),curcomMod->Name()); //-- set frame label
	        }
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
editnoteSWORD(gboolean editbuttonactive) //-- someone clicked the note edit button
{
 	if(editbuttonactive){
 		gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(MainFrm,"nbPerCom")),1);
		percomMod->Disp(percomDisplay);
		gtk_text_set_editable (GTK_TEXT (lookup_widget(MainFrm,"textComments")), TRUE); //-- set text widget to editable	
		gtk_widget_show(lookup_widget(MainFrm,"sbNotes")); //-- show comments status bar
		noteModified = false;	 //-- we just turned edit mode on no changes yet
        } else {	
		if(settings->formatpercom) {
			percomMod->Disp(FPNDisplay);
			gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(MainFrm,"nbPerCom")),0);
		}
		gtk_text_set_editable (GTK_TEXT (lookup_widget(MainFrm,"textComments")), false); //-- set text widget to not editable
		gtk_widget_hide(lookup_widget(MainFrm,"sbNotes"));//-- hide comments status bar
	}
	percomMod->Display(); 	
}

//-------------------------------------------------------------------------------------------
void
savenoteSWORD(gboolean noteisModified) //-- save personal comments
{  	
        if(noteisModified){ //-- if note modified save the changes
		VerseKey mykey; //-- verse key text
		gchar *buf;     //-- pointer to a string 					
		//GtkWidget *text; //-- pointer to commentary text widget		
		//text = lookup_widget(MainFrm,"textComments"); //-- get text widget
		buf = gtk_editable_get_chars((GtkEditable *)NEtext,0,-1); //-- get comments from text widget
		*percomMod << (const char *)buf; //-- save note!
		//g_warning(buf);
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
	percomMod->deleteEntry();        //-- delete note
	percomMod->Display();        //-- show change
}

//-------------------------------------------------------------------------------------------
void
changcurdictModSWORD(gchar *modName, gchar *keyText, gint page_num) //-- someone changed dict notebook page - sent here by notebook callback
{	                    //-- modName form page label - keyText from dict lookup entry
	ModMap::iterator it;
        GtkWidget   *frame;  //-- pointer to dict&lex frame
            						
	frame = lookup_widget(MainFrm,"frame10"); //-- set frame to dict&lex frame
	settings->notebook2page = page_num; //-- save current page
	it = mainMgr->Modules.find(modName);  //-- find module we want to use
	if (it != mainMgr->Modules.end()){	
		curdictMod = (*it).second;  //-- set curdictMod to new choice
		curdictMod->SetKey(keyText);   //-- set key to text from lookup entry
		curdictMod->Display();	 //-- display new dict
		//curdictMod->Name();
		FillDictKeysSWORD(); //-- fill the list widget with keys	
		gtk_frame_set_label( GTK_FRAME(frame),curdictMod->Name()); //-- set frame label
	}
}

/*
 *dictSearchTextChangedSWORD - dict lookup text changed
 *  mytext from the dict dict lookup entry
 */
void
dictSearchTextChangedSWORD(char* mytext)
{	                        //-- mytext from the dict dict lookup entry
	strcpy(settings->dictkey,mytext); //-- remember the key
	if (curdictMod){ //-- if we have a dict module	
		if(strcmp(mytext,"")){  //-- if text is not null		
			curdictMod->SetKey(mytext); //-- set key to our text
			curdictMod->Display();	//-- show what we found
			FillDictKeysSWORD(); //-- fill the list widget with keys	
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
		if(havebible) percomMod->SetKey(curMod->KeyText()); //-- go to text (verse)
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
	gtk_text_set_word_wrap(GTK_TEXT (text) , TRUE ); //-- set word wrap to TRUE for text widget
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
	gtk_text_set_word_wrap(GTK_TEXT(text), TRUE ); //-- set word wrap to TRUE for text widget
	AboutModsDisplay(text, bufabout) ; //-- send about info and text widget to display function (display.cpp)
}


//---------------------------------------------------------------------------------------------
void
lookupStrongsSWORD(gint theNumber) /* theNumber - strongs number was double clicked selected and sent here */
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
        if(havedict){ /* let's don't do this if we don't have at least one dictionary / lexicon */    			            	
                notebook = lookup_widget(MainFrm,"notebook4"); //-- get notebook
                gtk_notebook_set_page(GTK_NOTEBOOK(notebook), pagenum); //-- set notebook page
                entry = lookup_widget(MainFrm,"dictionarySearchText"); //-- get the entry so we can send it the new key
                sprintf(buf,"%d",theNumber); //-- put the number into a string
                gtk_entry_set_text(GTK_ENTRY(entry),buf); //-- put key string into the dict entry (which will cause a on_dictionarySearchText_changed event)
        }
}

/******************************************************************************
 * setglobalopsSWORD
 * option - option to set
 * yesno - yes or no
******************************************************************************/
void setglobalopsSWORD(gchar *option, gchar *yesno)
{
	/* turn option on or off */
	mainMgr->setGlobalOption(option, yesno);	
}

/******************************************************************************
 *redisplayTextSWORD - display Bible text in main window
******************************************************************************/
void redisplayTextSWORD(void)
{
	if(havebible) curMod->Display();	
}

/******************************************************************************
 *getmodnameSWORD
 * num
 * returns module name
******************************************************************************/
gchar* getmodnameSWORD(gint num)
{
        if(havebible) {
	        switch(num)
	        {
		        case 0: return curMod->Name();
			        break;
		        case 1: return curcomMod->Name();
			        break;
		        case 2: return curdictMod->Name();
			        break;
		        case 3: return comp1Mod->Name();
			        break;
		        case 4: return comp2Mod->Name();
			        break;	
		        case 5: return comp3Mod->Name();
			        break;
	        }
	}		
}

/******************************************************************************
 *navcurcommModSWORD
 * 
 * 
******************************************************************************/
void navcurcommModSWORD(gint backfoward)
{
	if (curcomMod) {
		if(backfoward) (*curcomMod)++;
		else (*curcomMod)--;		
		curcomMod->Display();
	}
}

/******************************************************************************
 *setuplisteditSWORD
 *
 *
******************************************************************************/
void setuplisteditSWORD(GtkWidget *text)
{
	listMgr = new SWMgr();
	listMod = NULL;
	ModMap::iterator it;	/* sword manager iterator */
	
	/* set sword display */
	listDisplay = new GTKInterlinearDisp(text);	
	/* iterator through modules */	
	for (it = listMgr->Modules.begin(); it != listMgr->Modules.end(); it++){
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			listMod = (*it).second;	/* set listMod  */
			listMod->Disp(listDisplay);	/* set listeditor display for modules */
		}
	}
	it = listMgr->Modules.find(curMod->Name()); /* set listMod to curMod */
	if (it != listMgr->Modules.end()) {
		listMod = (*it).second;
	}
}

//-------------------------------------------------------------------------------------------
void changeLEverseSWORD(gchar * verse)
{
	if (listMod) {
		listMod->SetKey(verse);
		listMod->Display();
	}
}

//-------------------------------------------------------------------------------------------
void destroyListEditorSWORD(void)	//-- destroy ListEditor
{
	delete listMgr;
	if (listDisplay)	//-- delete Sword display
		delete listDisplay;
}

/*
 *setupSDSWORD - set up the sword stuff for the view dictionary dialog
 *returns a list of dict modules
 */
GList* setupSDSWORD(GtkWidget *text)
{
	GList *list;
	ModMap::iterator it; //-- iteratior	
	
	SDMgr	= new SWMgr();
	SDMod     = NULL;
	SDDisplay = new  GTKEntryDisp(text);
	list = NULL;
	for(it = SDMgr->Modules.begin(); it != SDMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			SDMod = (*it).second;
			list = g_list_append(list,SDMod->Name());			
			SDMod->Disp(SDDisplay);
		}
	}
	return list;
	
}

/* 
 *
 */
gchar* getdictmodSWORD(void)
{
	return curdictMod->Name();
}

/*
 *
 */
gchar* gettextmodSWORD(void)
{      	
	
	gchar buf[80];
	
	strcpy(buf,curMod->Name());
	return buf;
}

/*
 *
 */
gchar* getcommodSWORD(void)
{
	return curcomMod->Name();
}

/*
 *loadSDmodSWORD - load a dictionary module into the view dictionary dialog
 *returns a list of keys
 */
void loadSDmodSWORD(GtkWidget *clist, gchar *modName)
{

        ModMap::iterator it;
        gchar *listitem;
        //GdkCursor *cursor;
        /*
	gtk_widget_show(MainFrm);
	cursor = gdk_cursor_new(GDK_CLOCK);
	gdk_window_set_cursor(MainFrm->window,cursor);
	*/
        gtk_clist_clear( GTK_CLIST(clist)); //-- start with empty list	
        it = SDMgr->Modules.find(modName);  //-- find module we want to use
	if (it != SDMgr->Modules.end()){
		
		SDMod = (*it).second;  //-- set curdictMod to new choice
		SDMod->SetKey("");		
		SDMod->Display();	 //-- display new dict
		
		SDMod->Error();
		for (;!SDMod->Error();(*SDMod)++){
		        listitem = g_strdup((const char *)SDMod->KeyText()); //-- key to listitem
		        //g_warning(listitem);
		        gtk_clist_append(GTK_CLIST(clist) , &listitem); //-- listitem to list
		        g_free(listitem);
		}
	}
}

/*
 *gotokeySWORD - find new key for view dictionary dialog
 *
 */
void gotokeySWORD(gchar *newkey)
{
        SDMod->SetKey(newkey); //-- set key to our text
        SDMod->Display();
}
/*
 * SDdictSearchTextChangedSWORD - dict lookup text changed
 *  newkey from the dict lookup entry
 */
void SDdictSearchTextChangedSWORD(char* newkey)
{	
	strcpy(settings->dictkey,newkey); //-- remember the key
	if (SDMod){ //-- if we have a dict module	
		if(strcmp(newkey,"")){  //-- if text is not null		
			SDMod->SetKey(newkey); //-- set key to our text
			SDMod->Display();	//-- show what we found	
		}
	}
}

