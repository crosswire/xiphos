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
//#include <gbfhtml.h>
#include <rwphtml.h>
//#include <thmlhtml.h>
//#include <latin1utf8.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>
#include <gal/widgets/e-unicode.h>

#include "sw_gbfhtml.h"
#include "sw_thmlhtml.h"
#include "sw_latin1utf8.h"
#include "gs_gnomesword.h"
#include "gs_history.h"
#include "display.h"
#include "gs_display.h"
#include "callback.h"
#include "gs_sword.h"
#include "support.h"
#include "gs_preferences_dlg.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "gs_listeditor.h"
#include "gs_html.h"
#include "gs_search.h"
#include "gs_abouts.h"
#include "gs_info_box.h"
#include "gs_verselist_dlg.h"
#include "gs_setup.h"
#include "gs_shortcutbar.h"
#include "sw_utility.h"
#include "sw_properties.h"
#include "sw_bookmarks.h"

/***********************************************************************************************
Sword global to this file
***********************************************************************************************/
//SWDisplay *chapDisplay; /* to display modules using GtkText a chapter at a time */
SWDisplay *entryDisplay; /* to display modules using GtkText a verse at a time */
SWDisplay *comp1Display; /* to display modules using GtkText a verse at a time */
SWDisplay *comp2Display; /* to display modules using GtkText a verse at a time */
SWDisplay *comp3Display; /* to display modules using GtkText a verse at a time */
SWDisplay *comDisplay; /* to display modules using GtkText a verse at a time */
SWDisplay *percomDisplay; /* to display personal comment modules using GtkText a verse at a time */
SWDisplay *dictDisplay; /* to display modules using GtkText a verse at a time */
SWDisplay *FPNDisplay; /* to display formatted personal notes using GtkText */
SWDisplay *HTMLDisplay; /* to display formatted html */
SWDisplay *listDisplay;	/* to display modules in list editor */
SWDisplay *SDDisplay;	/* to display modules in view dict dialog */
SWDisplay *RWPDisplay;	/* to display rwp module in gtktext window */
SWDisplay *VCDisplay;	/* to display modules in view comm dialog */
SWDisplay *UTF8Display;	/* to display modules in utf8 */
 
SWMgr *mainMgr; /* sword mgr for curMod - curcomMod - curdictMod */
SWMgr *mainMgr1; /* sword mgr for comp1Mod - first interlinear module */
SWMgr *percomMgr; /* sword mgr for percomMod - personal comments editor */
SWMgr *listMgr;	/* sword mgr for ListEditor */
SWMgr *SDMgr;	/* sword mgr for view dict dialog */
SWMgr *VCMgr;	/* sword mgr for view comm dialog */

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
SWModule *VCMod;   /* module for view comm dialog */

SWFilter *gbftohtml;
SWFilter *plaintohtml;
SWFilter *thmltohtml;
SWFilter *rwptohtml;
SWFilter *lattoutf8;
/***********************************************************************************************
GnomeSword globals
***********************************************************************************************/ 	
GList 	*biblemods,
	*commentarymods,
	*dictionarymods,
	*percommods,
	*sbbiblemods,
	*sbdictmods,
	*sbcommods;
GtkWidget *NEtext;  /* note edit widget */
gboolean ApplyChange = true;	/* should we make changes when cbBook is changed */
//gboolean bVerseStyle = true;	/* should we show verses or paragraphs in main text window */
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
gint	hebrewpage = 0,
	greekpage = 0;
gchar *textmod,
        *commod,
        *dictmod;
GtkWidget *MainFrm; /* main form widget  */
//INTERLINEAR interlinearMods;
GS_MODS *p_textmodule;
GS_MODS textmodule;
/***********************************************************************************************
 externals
***********************************************************************************************/
extern gboolean changemain; /* change verse of Bible text window */
extern gint 	dictpages, /* number of dictionaries */
	 		compages,  /* number of commentaries */
			textpages;/* number of Bible text */
extern gboolean file_changed;	/* set to true if text is study pad has changed - and file is not saved */
extern SETTINGS *settings;
extern SETTINGS myset;
extern GtkWidget *shortcut_bar;
extern gchar *current_filename;	/* filename for open file in study pad window  */
extern gchar current_verse[80];	/* current verse showing in main window - 1st - 2nd - 3rd
				                           interlinear window - commentary window */
extern gint historyitems;      /* number of history items */
extern HISTORY historylist[];  /* sturcture for storing history items */
extern gboolean addhistoryitem; /* do we need to add item to history */
extern gchar *mycolor;
extern GString *gs_clipboard;
extern gboolean firstsearch;
extern GS_APP gs;
//extern GS_FONTS *gsfonts;
//extern GS_NB_PAGES *nbpages;
extern GtkWidget *htmlVL;
extern gboolean isrunningVL
;
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
	//ConfigEntMap section;
	int   	i, //-- counter
		j; //-- counter					
   	gchar 	*sourceformat;
	GnomeUIInfo *menuitem; //--  gnome menuitem
  	GtkWidget *menu_items;
	string encoding;
	
  	g_print("Initiating Sword\n"); 
	
 	plaintohtml   	= new PLAINHTML(); //-- sword renderfilter plain to html
  	thmltohtml	= new SW_ThMLHTML(); /* sword renderfilter thml to html */	
        rwptohtml	= new RWPHTML();
        gbftohtml		= new SW_GBFHTML();
        lattoutf8		= new SW_Latin1UTF8();

	mainMgr         = new SWMgr();	//-- create sword mgrs
	mainMgr1        = new SWMgr();	
  	percomMgr	= new SWMgr();
	
	curMod		= NULL; //-- set mods to null
	comp1Mod      = NULL;
	comp2Mod      = NULL;
	comp3Mod      = NULL;
	curcomMod     = NULL;
	curdictMod      = NULL; 	
	percomMod     = NULL;
	
	//chapDisplay     = 0;// set in create
	entryDisplay    = 0;// set in create
	comp1Display    = 0;// set in create
	comp2Display    = 0;// set in create
	comp3Display    = 0;// set in create
	comDisplay      = 0;// set in create
	dictDisplay     = 0;// set in create	
	percomDisplay   = 0;// set in create
	FPNDisplay			= 0;
	HTMLDisplay			= 0;
	UTF8Display	= 0;
	
	
	/* set glist to null */
	biblemods = NULL;
	commentarymods = NULL;
	dictionarymods = NULL;
	percommods = NULL;
	sbbiblemods = NULL;
	sbcommods = NULL;
	sbdictmods = NULL;
	
	MainFrm = lookup_widget(mainform,"settings->app"); //-- save mainform for use latter
	NEtext =  lookup_widget(mainform,"textComments"); //-- get note edit widget
	//mycolor = settings->currentverse_color; /* for GtkHTML widgets */
	textmodule.name = NULL;
	textmodule.type = NULL;
	textmodule.description = NULL;
	textmodule.key = NULL;
	p_textmodule = &textmodule;

        //-- setup displays for sword modules
	GTKEntryDisp::__initialize();
	//chapDisplay = new HTMLChapDisp(lookup_widget(mainform,"moduleText"));	
	comDisplay = new  GTKEntryDisp(lookup_widget(mainform,"textCommentaries"));
	percomDisplay = new  GTKPerComDisp(lookup_widget(mainform,"textComments"));
	UTF8Display = new GTKutf8ChapDisp(lookup_widget(mainform,"htmlTexts"));
	HTMLDisplay = new GtkHTMLEntryDisp(lookup_widget(mainform,"htmlCommentaries"));
	comp1Display = new InterlinearDisp(lookup_widget(mainform,"textComp1"));
	FPNDisplay = new ComEntryDisp(lookup_widget(mainform,"htmlComments"));
	dictDisplay = new GtkHTMLEntryDisp(lookup_widget(mainform,"htmlDict"));
	compages = 0;
	dictpages = 0;
	
	
	if(settings->showsplash){
		while (gtk_events_pending ())
				gtk_main_iteration ();
	}
	g_print("Loading SWORD Moudules\n");
	
	for(it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			curMod = (*it).second;
			sit = mainMgr->config->Sections.find((*it).second->Name()); 
			ConfigEntMap &section = (*sit).second;
			havebible = TRUE;
			++textpages;
			biblemods = g_list_append(biblemods,curMod->Name());
			sbbiblemods = g_list_append(sbbiblemods,curMod->Description());
			addrenderfiltersSWORD(curMod, section);			
			curMod->Disp(UTF8Display);
		}else if (!strcmp((*it).second->Type(), "Commentaries")){    //-- set commentary modules		
			curcomMod = (*it).second;
			commentarymods = g_list_append(commentarymods,curcomMod->Name());
			sbcommods = g_list_append(sbcommods,curcomMod->Description());
			havecomm = TRUE; //-- we have at least one commentay module
			++compages; //-- how many pages do we have  
			sit = mainMgr->config->Sections.find((*it).second->Name()); 
			ConfigEntMap &section = (*sit).second;
			addrenderfiltersSWORD(curcomMod, section); 
			curcomMod->Disp(HTMLDisplay);
		}else if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){ //-- set dictionary modules	
			havedict = TRUE; //-- we have at least one lex / dict module
			++dictpages; //-- how many pages do we have
			curdictMod = (*it).second;
			dictionarymods = g_list_append(dictionarymods,curdictMod->Name());
			sbdictmods = g_list_append(sbdictmods,curdictMod->Description());
			sit = mainMgr->config->Sections.find((*it).second->Name()); 
			ConfigEntMap &section = (*sit).second;
			addrenderfiltersSWORD(curdictMod, section);
			curdictMod->Disp(dictDisplay);
		}
	} 		
  	//-- set up percom editor module
	for (it = percomMgr->Modules.begin(); it != percomMgr->Modules.end(); it++){
		if (!strcmp((*it).second->Type(), "Commentaries")){ //-- if type is 
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
				sit = mainMgr1->config->Sections.find((*it).second->Name()); 
				ConfigEntMap &section = (*sit).second;
				addrenderfiltersSWORD(comp1Mod, section);
				comp1Mod->Disp(comp1Display);
			}
		}
	}
	
	if(settings->showsplash){
		while (gtk_events_pending ())
				gtk_main_iteration ();
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
	GList *mods;
	gchar 
		*currRef;
	if((GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active) && noteModified){ //-- save any changes to personal notes		
		if(autoSave){                          //-- if we are in edit mode
			savenoteSWORD(noteModified); 	//-- save if text in note window has changed			
		}
	}
	 		
	strcpy(current_verse,ref);		
	ApplyChange = false;
	if(changemain && havebible) {
		if(curMod){  //--------------------------------------------------- change main window
			currRef = g_strdup(curMod->KeyText());
			curMod->SetKey(ref);
			if(!stricmp(curMod->KeyText(),"Genesis 1:1")) 
				if(!stricmp(ref,"Gen 1:1") || !stricmp(ref,"Genesis 1:1") || !stricmp(ref,"Gene 1:1")){	
					g_warning("Genesis 1:1");
				}else{
					ref=currRef;
					curMod->SetKey(ref);
					strcpy(current_verse,ref);
				}			
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
			curMod->Display();	
			g_free(currRef);
		}
	}
	changemain = TRUE;
	//--------------------------------------------------------------- change interlinear verses
	updateinterlinearpage(); 
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

/*
 *
 */
void
updateinterlinearpage(void)
{
	GtkWidget *html_widget;
	gchar tmpBuf[256];
	gchar *utf8str;
	gint utf8len;
	
	if(settings->notebook3page == 2){
		html_widget = lookup_widget(MainFrm,"textComp1");
		beginHTML(html_widget,TRUE);
		sprintf(tmpBuf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings->bible_bg_color, settings->bible_text_color,
		settings->link_color);
		utf8str = e_utf8_from_gtk_string(html_widget, tmpBuf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;
		displayHTML(GTK_WIDGET(html_widget), utf8str, utf8len);		
		changecomp1ModSWORD(settings->Interlinear1Module);
		changecomp1ModSWORD(settings->Interlinear2Module);
		changecomp1ModSWORD(settings->Interlinear3Module);
		changecomp1ModSWORD(settings->Interlinear4Module);
		changecomp1ModSWORD(settings->Interlinear5Module);
		endHTML(html_widget);
	}
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
	//writesettings(myset); //-- save setting (myset structure) to use when we start back up
	savebookmarks(gs.ctree_widget);
	saveconfig();
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
	g_string_free(gs_clipboard,TRUE);
	delete mainMgr;   //-- delete Sword managers
	delete mainMgr1;
	
        if (thmltohtml != 0)
		delete thmltohtml;
	if (gbftohtml != 0)
		delete gbftohtml;
	if (plaintohtml != 0)
		delete plaintohtml;	
	if (lattoutf8 != 0)
		delete lattoutf8;	
	/*if (chapDisplay)    //-- delete Sword displays
		delete chapDisplay;*/
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
	if(RWPDisplay)
		delete RWPDisplay;		
	/*if(noteeditor)
		delete noteeditor;	*/
	gtk_exit(0);           //-- exit	
}

/*******************************************************************************
 * toggle morphological tags on and off
 * window - the window to effect
 * choice - true = on, false = off
 ******************************************************************************/
void
morphsSWORD(gint window, gboolean choice) //-- toogle strongs numbers for modules that have strongs
{
	switch(window){
		case 0: // main text window	
			if(choice){ 	
				mainMgr->setGlobalOption("Morphological Tags","On");  
			}else{   
				mainMgr->setGlobalOption("Morphological Tags","Off");	
			}
			settings->strongs = choice;   //-- store choice in settings
			if(havebible){
				curMod->Display(); //-- we need to show change
			}
		break;
		case 1: // interlinear window	
			if(choice){ 
				mainMgr1->setGlobalOption("Morphological Tags","On"); 
			}else{   				
				mainMgr1->setGlobalOption("Morphological Tags","Off");	
			}
			if(havebible){
				updateinterlinearpage();
			}
		break;
	}
}


/*******************************************************************************
 * toggle Strongs numbers on and off
 * window - the window to effect
 * choice - true = on, false = off
 ******************************************************************************/
void
strongsSWORD(gint window, gboolean choice) //-- toogle strongs numbers for modules that have strongs
{
	switch(window){
		case 0: // main text window	
			if(choice){ //-- if choice is TRUE - we want strongs numbers	
				mainMgr->setGlobalOption("Strong's Numbers","On");  
			}else{   //-- we don't want strongs numbers	
				mainMgr->setGlobalOption("Strong's Numbers","Off");
			}
			settings->strongs = choice;   //-- store choice in settings
			if(havebible){
				curMod->Display(); //-- we need to show change
			}
		break;
		case 1: // interlinear window	
			if(choice){ //-- if choice is TRUE - we want strongs numbers
				mainMgr1->setGlobalOption("Strong's Numbers","On");  
			}else{   //-- we don't want strongs numbers					
				mainMgr1->setGlobalOption("Strong's Numbers","Off");
			}
			if(havebible){
				updateinterlinearpage();
			}
		break;
	}
}

/*******************************************************************************
 * toggle gbf footnotes on and off
 * window - the window to effect
 * choice - true = on, false = off
 ******************************************************************************/
void
footnotesSWORD(gint window, gboolean choice) //-- toogle gbf footnotes for modules that have them
{
	switch(window){
		case 0: // main text window	
			if(choice){ 	
				mainMgr->setGlobalOption("Footnotes","On");  
			}else{   
				mainMgr->setGlobalOption("Footnotes","Off");	
			}
			settings->strongs = choice;  
			if(havebible){
				curMod->Display(); 
			}
		break;
		case 1: // interlinear window	
			if(choice){ 
				mainMgr1->setGlobalOption("Footnotes","On");  
			}else{   				
				mainMgr1->setGlobalOption("Footnotes","Off");	
			}
			if(havebible){
				updateinterlinearpage();
			}
		break;
	}
}

/* 
 * gotoBookmarkSWORD - bad hack but it works
 * 
 */
void
gotoBookmarkSWORD(gchar *modName, gchar *key)
{
	GtkWidget *notebook, *entry;
	ModMap::iterator it;
	gint bibleindex = 0;
	gint commindex = 0;
	gint dictindex = 0;	
	
	for(it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			if(!strcmp((*it).second->Name(), modName)){
				//curMod = (*it).second;
				notebook = lookup_widget(MainFrm,"nbTextMods");
				gtk_notebook_set_page(GTK_NOTEBOOK(notebook),bibleindex ); 
				changeVerseSWORD(key);
				return;
			} 				
			++bibleindex;
		}else if (!strcmp((*it).second->Type(), "Commentaries")){    //-- set commentary modules		
			if(!strcmp((*it).second->Name(), modName)){
				//curcomMod = (*it).second;  //-- change current module to new module				
				notebook = lookup_widget(MainFrm,"notebook1");
				gtk_notebook_set_page(GTK_NOTEBOOK(notebook),commindex ); 
				changeVerseSWORD(key);
				return;
			}
			++commindex;
		}else if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){ //-- set dictionary modules	
			if(!strcmp((*it).second->Name(), modName)){
				//curdictMod = (*it).second;  //-- change current module to new module
				//curdictMod->SetKey(key);
				entry = lookup_widget(MainFrm,"dictionarySearchText");
				notebook = lookup_widget(MainFrm,"notebook4");
				gtk_notebook_set_page(GTK_NOTEBOOK(notebook),dictindex );
				gtk_entry_set_text(GTK_ENTRY(entry), key); 				
				return;				
			} 
			++dictindex;
		}
	} 
}

//-------------------------------------------------------------------------------------------
void
changecurModSWORD(gchar *modName, gboolean showchange) //-- change sword module for main window
{                                 //-- someone clicked View->MainWindow->module
	ModMap::iterator it;            //-- or clicked the mainwindow popup menu and the callback sent us here
	gchar title[200];
	
        if(havebible) {
	        it = mainMgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	        if (it != mainMgr->Modules.end()){ //-- if we find the module	
		        curMod = (*it).second;  //-- change current module to new module
		        if(showchange) {
		                curMod->SetKey(current_verse); //-- set key to current verse
		                curMod->Display();            //-- show it to the world
		        }
	        }			
		p_textmodule->name = (gchar*)curMod->Name(); 
		p_textmodule->description = (gchar*)curMod->Description();
		p_textmodule->type = (gchar*)curMod->Type();
		p_textmodule->key = (gchar*)curMod->KeyText();
		
	       strcpy(settings->MainWindowModule, p_textmodule->name);  //(gchar*)curMod->Name()); //-- remember where we are so we can open here next time we startup
	
		sprintf(title,"GnomeSword - %s", p_textmodule->description); //curMod->Description());		
		gtk_window_set_title(GTK_WINDOW(MainFrm), title);
		
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
		//g_warning(current_verse);
		comp1Mod->SetKey(current_verse);  //-- set key to current verse
		comp1Mod->Display();              //-- show it to the world
	}
	//strcpy(settings->Interlinear1Module, comp1Mod->Name()); //-- remember where we are so we can open here next time we startup
}


//-------------------------------------------------------------------------------------------
void
setversestyleSWORD(gboolean choice)  //-- set verse style -- verses or paragraphs
{
	settings->versestyle = choice; //-- remember our choice for the next program startup
 	if(havebible) curMod->Display(); //-- show the change
}

//-------------------------------------------------------------------------------------------
void
chapterSWORD(void)  //-- someone clicked the chapter spin button
{
	gint       iChap;
        gchar      *buf;
        //-- set iChap to value in spin button
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(MainFrm,"spbChapter"))); 
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

void
nbchangecurModSWORD(gchar *modName, gint page_num, gboolean showchange)  //-- someone changed commentary notebook page (sent here by callback function notebook page change)
{
	//nbpages->nbTextModspage = page_num;
	changecurModSWORD(modName, showchange); 
}


//-------------------------------------------------------------------------------------------
void
changcurcomModSWORD(gchar *modName, gboolean showchange)  //-- someone changed commentary notebook page (sent here by callback function notebook page change)
{
	ModMap::iterator it;
	//GtkWidget *frame;//-- pointer to commentary frame *notebook, //-- pointer to commentary notebook
	//GtkWidget *label;
	if(havebible) {			
	        //notebook = lookup_widget(MainFrm,"notebook1"); //-- set notebook pointer to commentary notebook
	        //frame = lookup_widget(MainFrm,"framecom"); //-- set frame to commentary frame
	        //nbpages->notebook1page = page_num; //-- save current page
	        //g_print("page=%d\n",settings->notebook1page);
	        it = mainMgr->Modules.find(modName); //-- find commentary module (modName from page label)
	        if (it != mainMgr->Modules.end()){	
		        curcomMod = (*it).second;  //-- set curcomMod to modName
		        if(showchange) {
		                if(autoscroll) curcomMod->SetKey(curMod->KeyText()); //-- go to text (verse)
		                curcomMod->Display(); //-- show the change
				strcpy(settings->CommWindowModule, curcomMod->Name()); 
		        }
		        //gtk_frame_set_label( GTK_FRAME(frame),curcomMod->Name()); //-- set frame label
		        //label = gtk_label_new(curcomMod->Name());
		       // gtk_widget_show(label);
		        /*gtk_notebook_set_tab_label(GTK_NOTEBOOK(lookup_widget(MainFrm,"notebook3")),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (lookup_widget(MainFrm,"notebook3")),
							     0), label);*/
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
	percomMod->deleteEntry();        //-- delete note
	percomMod->Display();        //-- show change
}

//-------------------------------------------------------------------------------------------
void
changcurdictModSWORD(gchar *modName, gchar *keyText) //-- someone changed dict notebook page - sent here by notebook callback
{	                    //-- modName form page label - keyText from dict lookup entry
	ModMap::iterator it;
        //GtkWidget   *frame;  //-- pointer to dict&lex frame
            						
	//frame = lookup_widget(MainFrm,"frame10"); //-- set frame to dict&lex frame
	//nbpages->notebook2page = page_num; //-- save current page
	it = mainMgr->Modules.find(modName);  //-- find module we want to use
	if (it != mainMgr->Modules.end()){	
		curdictMod = (*it).second;  //-- set curdictMod to new choice
		curdictMod->SetKey(keyText);   //-- set key to text from lookup entry
		curdictMod->Display();	 //-- display new dict
		FillDictKeysSWORD(); //-- fill the list widget with keys
		strcpy(settings->DictWindowModule, curdictMod->Name()); 
		//gtk_frame_set_label( GTK_FRAME(frame),curdictMod->Name()); //-- set frame label
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
		//percomMod->Display(); //-- show the change 	
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
	GtkWidget *aboutbox;  //-- pointer to about dialog
	
 	GtkWidget	*text;       //-- pointer to text widget of dialog
	gchar 		*buf,       //-- pointer to text buffer for label (mod name)
 				*bufabout,  //-- pointer to text buffer for text widget (mod about)
				*newbuf,
				discription[500];
	gint 		len;
	
        ModMap::iterator it; //-- module iterator
	SectionMap::iterator sit; //--
	ConfigEntMap::iterator cit; //--
		
	bufabout = "oops";
	it = mainMgr->Modules.find(modName); //-- find module (modName)
	if (it != mainMgr->Modules.end()){ //-- if we don't run out of mods before we find the one we are looking for	
	        buf = (char *)(*it).second->Description();  //-- get discription of module
	        sit = mainMgr->config->Sections.find((*it).second->Name());
	        if (sit !=mainMgr->config->Sections.end()){
	                cit = (*sit).second.find("About");
			if (cit != (*sit).second.end()) 				
				 bufabout = (char *)(*cit).second.c_str(); //-- get module about information
				//cout << bufabout << '\n';
	        }
	}
	
	sprintf(discription,"<FONT COLOR=\"#000FCF\"><center><b>%s</b></center></font><HR>",buf); 
	aboutbox = create_aboutmodules(); //-- create about dialog
	gtk_widget_show(aboutbox);
	if(strcmp(bufabout,"oops")){
		len = strlen(bufabout);
		newbuf = new char[len + 600];
		text = lookup_widget(aboutbox,"textModAbout"); //-- get text widget
		AboutModsDisplayHTML(newbuf, bufabout) ; //-- send about info and alocated new text buffer to display function (gs_display.cpp)
		beginHTML(text, FALSE);
		displayHTML(text, "<html><body>", strlen("<html><body>"));
		displayHTML(text, discription, strlen(discription));
		displayHTML(text, newbuf,strlen(newbuf));
		displayHTML(text, "</body></html>", strlen("</body></html>"));
		endHTML(text);
		delete[]newbuf; 
	}else 
		g_warning(bufabout);
}


//-------------------------------------------------------------------------------------------
void
showinfoSWORD(GtkWidget *text, GtkLabel *label, GtkLabel *version_label) //--  show text module about information in the Sword Project about dialog
{
	char 	*buf,       //-- pointer to text buffer for label (mod name)
 	        *bufabout,  //-- pointer to text buffer for text widget (mod about)
		version[40];
        ModMap::iterator it; //-- module iterator
	SectionMap::iterator sit; //--
	ConfigEntMap::iterator cit; //--
	gfloat ver;
	
	ver = getSwordVerionSWORD();
	sprintf(version,"Sword-%.2f",ver);
	gtk_label_set_text( GTK_LABEL(version_label),version);  //-- set label to sword version
	
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

/************************************************************************************************************************* 
 * 
 *************************************************************************************************************************/
gchar *showfirstlineStrongsSWORD(gint theNumber)
{
 	ModMap::iterator it;
 	SWModule *strongsMod;
 	gchar *returnVal;
 	gchar    *workingVal, wbuf[1024];
 	gint i,j,len;
        gchar           buf[40]; //-- char string to put our strongs number in
        char            a;        //-- char to store testament (old - 001 or new - 002)

        if(havedict) {  /* let's don't do this if we don't have at least one dictionary / lexicon */   
        	
        	sprintf(buf,"%d",theNumber); 	       
        	VerseKey *key = (VerseKey *)(SWKey *)(*curMod); //-- get a versekey form the current text module
                                                    //-- so we can find which testament we are using
        	a = key->Testament();  //-- find out if we are in old or new testament        
        	if(a == 001) {     //-- if old testament use hebrew lex
			it = mainMgr->Modules.find("StrongsHebrew");  //-- find module we want to use
			if (it != mainMgr->Modules.end()){	
				strongsMod = (*it).second;  //-- set curdictMod to new choice
				strongsMod->SetKey(buf);   //-- set key to text from lookup entry				
				workingVal = g_strdup((const char*)*strongsMod);
			}
		}else{ 
			it = mainMgr->Modules.find("StrongsGreek");  //-- find module we want to use
			if (it != mainMgr->Modules.end()){	
				strongsMod = (*it).second;  //-- set curdictMod to new choice
				strongsMod->SetKey(buf);   //-- set key to text from lookup entry
				workingVal = g_strdup((const char*)*strongsMod);
			}
		}
		for(i=0; i< strlen(workingVal); i++) {
				
		}
        }
        g_warning(returnVal);
        
	return returnVal;
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
 *getmodkeySWORD
 * num
 * returns module key
******************************************************************************/
gchar* getmodkeySWORD(gint num)
{
        if(havebible) {
	        switch(num)
	        {
		        case 0: return (gchar*)curMod->KeyText();
			        break;
		        case 1: return (gchar*)curcomMod->KeyText();
			        break;
		        case 2: return (gchar*)curdictMod->KeyText();
			        break;
		        case 3: return (gchar*)comp1Mod->KeyText();
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
	SectionMap::iterator sit; //-- iteratior	
	
	/* set sword display */
	listDisplay = new HTMLentryDisp(text);	
	/* iterator through modules */	
	for (it = listMgr->Modules.begin(); it != listMgr->Modules.end(); it++){
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			listMod = (*it).second;	/* set listMod  */
			sit = listMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters
	    		if (sit !=listMgr->config->Sections.end()){	    					
				ConfigEntMap &section = (*sit).second;
				addrenderfiltersSWORD(listMod , section);	
				listMod->Disp(listDisplay);
			}
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

/****************************************************************************************
 *setupSDSWORD - set up the sword stuff for the view dictionary dialog
 *returns a list of dict modules
 ****************************************************************************************/
GList* setupSDSWORD(GtkWidget *text)
{
	GList *list;
	ModMap::iterator it; //-- iteratior			
	SectionMap::iterator sit; //-- iteratior
	
	SDMgr	= new SWMgr();
	SDMod     = NULL;
	SDDisplay = new  GtkHTMLEntryDisp(text);
	//SDDisplay = new  GtkHTMLEntryDisp(text);
	list = NULL;
	for(it = SDMgr->Modules.begin(); it != SDMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			SDMod = (*it).second;
			list = g_list_append(list,SDMod->Name());
			sit = SDMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters			
			ConfigEntMap &section = (*sit).second;
			addrenderfiltersSWORD(SDMod, section);		
			SDMod->Disp(SDDisplay);
		}
	}
	return list;
}

/****************************************************************************** 
 * returns the name of the current dict/lex module
 ******************************************************************************/
gchar* getdictmodSWORD(void)
{
	return curdictMod->Name();
}

/******************************************************************************
 * returns the name of the current Bible text module
 ******************************************************************************/
gchar* gettextmodSWORD(void)
{     
	return curMod->Name();    
}

/******************************************************************************
 * returns the name of the current commentary module
 ******************************************************************************/
gchar* getcommodSWORD(void)
{
	return curcomMod->Name();
}

/******************************************************************************
 * returns the description of the current commentary module
 ******************************************************************************/
gchar* getcommodDescriptionSWORD(void)
{
	return (char *) curcomMod->Description();;
}

/******************************************************************************
 * returns the description of the view commentary dialog module
 ******************************************************************************/
gchar* getVCmodDescriptionSWORD(void)
{
	return (char *) VCMod->Description();;
}

/******************************************************************************
 * returns the description of the view commentary dialog module
 ******************************************************************************/
gchar* getSDmodDescriptionSWORD(void)
{
	return (char *) SDMod->Description();;
}

/******************************************************************************
 *loadSDmodSWORD - load a dictionary module into the view dictionary dialog
 *
 ******************************************************************************/
void loadSDmodSWORD(GtkWidget *clist, gchar *modName)
{
	ModMap::iterator it;
        
        
        gtk_clist_clear( GTK_CLIST(clist)); //-- start with empty list	
        it = SDMgr->Modules.find(modName);  //-- find module we want to use
	if (it != SDMgr->Modules.end()){		
		SDMod = (*it).second;  //-- set curdictMod to new choice
		SDMod->SetKey("");		
		SDMod->Display();	 //-- display new dict		
	}
}

/******************************************************************************
 *loadSDkeysSWORD - load a dictionary keys into clist
 *returns a list of keys
 ******************************************************************************/
void loadSDkeysSWORD(GtkWidget *clist)
{
	gchar *listitem;
	//SDMod->SetKey("A");
	if(!stricmp(SDMod->Name(),"WebstersDict"))
		SDMod->SetKey("A");
	else
		(*SDMod)=TOP; 
	SDMod->Display();
	SDMod->Error();
	for (;!SDMod->Error();(*SDMod)++){
		listitem = g_strdup((const char *)SDMod->KeyText()); //-- key to listitem
		gtk_clist_append(GTK_CLIST(clist) , &listitem); //-- listitem to list
		if(listitem)
			g_free(listitem);
	}
}

/******************************************************************************
 *gotokeySWORD - find new key for view dictionary dialog
 *
 ******************************************************************************/
void gotokeySWORD(gchar *newkey)
{
        SDMod->SetKey(newkey); //-- set key to our text
        SDMod->Display();
}

/******************************************************************************
 * SDdictSearchTextChangedSWORD - dict lookup text changed
 *  newkey from the dict lookup entry
 ******************************************************************************/
void SDdictSearchTextChangedSWORD(char* newkey)
{		
	if (SDMod){ //-- if we have a dict module	
		if(strcmp(newkey,"")){  //-- if text is not null		
			SDMod->SetKey(newkey); //-- set key to our text
			SDMod->Display();	//-- show what we found	
		}
	}
}

/******************************************************************************  
 * 
 ******************************************************************************/
GtkWidget *createSearchDlgSWORD(void)
{
	GtkWidget* searchDlg;
	
	searchDlg = create_dlgSearch();
	searchWindow->initsearchWindow(searchDlg);
	firstsearch = FALSE;
	gtk_signal_connect(GTK_OBJECT(searchDlg), "destroy",
				GTK_SIGNAL_FUNC(on_dlgSearch_destroy),
				 NULL);
	return searchDlg;
}

/* *****************************************************************************
 *
 ******************************************************************************/
void startsearchSWORD(GtkWidget *searchFrm)
{	
	gtk_clist_clear(GTK_CLIST(searchWindow->resultList));
	searchWindow->searchSWORD(searchFrm);	
}


/****************************************************************************************
 *setupCommSWORD - set up the sword stuff for the view dictionary dialog
 *returns a list of commentary modules
 ****************************************************************************************/
GList* setupCommSWORD(GtkWidget *text)
{
	GList *list;
	ModMap::iterator it; //-- iteratior	
	SectionMap::iterator sit; //-- iteratior
	
	VCMgr	= new SWMgr();
	VCMod     = NULL;
	VCDisplay = new  GtkHTMLEntryDisp(text);
	//VCDisplay = new  GtkHTMLEntryDisp(text);
	list = NULL;
	for(it = VCMgr->Modules.begin(); it != VCMgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Commentaries")){
			VCMod = (*it).second;
			sit = VCMgr->config->Sections.find((*it).second->Name()); //-- check to see if we need render filters			
			ConfigEntMap &section = (*sit).second;
			addrenderfiltersSWORD(VCMod, section);
			havebible = TRUE;		
			list = g_list_append(list,VCMod->Name());
			VCMod->Disp(VCDisplay);
		}
	}
	return list;
}

//-------------------------------------------------------------------------------------------
void shutdownVCSWORD(void)  //-- close down view comm dialog program
{	
	delete VCMgr;	
	if(VCDisplay)
		delete VCDisplay;	
}

/******************************************************************************
 *loadVCmodSWORD - load a commentary module into the view commentary dialog
 *
 ******************************************************************************/
void loadVCmodSWORD(gchar *modName)
{
        ModMap::iterator it;
        
        it = VCMgr->Modules.find(modName);  //-- find module we want to use
	if (it != VCMgr->Modules.end()){
		
		VCMod = (*it).second;  //-- set curdictMod to new choice
		VCMod->SetKey("");		
		VCMod->Display();	 //-- display new dict
	}
}

/******************************************************************************
 *gotoverseVCSWORD - find new verse for view commentary dialog
 *
 ******************************************************************************/
void gotoverseVCSWORD(gchar *newkey)
{
        VCMod->SetKey(newkey); //-- set key to our text
        VCMod->Display();
}

void navVCModSWORD(gint direction)  //-- navigate the current commentary module
{
        switch(direction){   
		case 0: (*VCMod)--;
			break;
		case 1: (*VCMod)++;
			break;
        }
        VCMod->Error(); //-- clear any errors
        VCMod->Display();
}


GList *getBibleBooks(void)
{	
  	VerseKey DefaultVSKey;
	GList *list = NULL;
	GString *s1;
		/*** load Bible books ***/
	DefaultVSKey = TOP;	
	while (!DefaultVSKey.Error()) {
		s1 = g_string_new((const char *)DefaultVSKey);
		s1 = g_string_truncate(s1, (s1->len - 4));
		list = g_list_append(list, s1->str);
		//g_warning(s1->str);
		DefaultVSKey.Book(DefaultVSKey.Book() + 1);
		g_string_free(s1,FALSE);
	}
	return list;	
}
/*
 * parse vlist for verses
 */
gboolean getVerseListSWORD(gchar *vlist)
{
	gboolean retval=FALSE;
	gchar buf[256];
	ListKey tmpVerseList;
	VerseKey DefaultVSKey;
	gint count=0;
	static GtkWidget *dlg;
	
	if(!isrunningVL){
		dlg = create_dglVerseList();
		isrunningVL = TRUE;
	}		
	for(int i=0; i<strlen(vlist); i++){
		if(vlist[i] == '+') 
			vlist[i] = ' ';
		if(vlist[i] == ',') 
			vlist[i] = ';';
	}
	tmpVerseList = DefaultVSKey.ParseVerseList((char *)vlist, DefaultVSKey);
	beginHTML(htmlVL, FALSE);
	while (!tmpVerseList.Error()) {
		sprintf(buf,"<font size=\"%s\"><a href=\"%s\">%s</a></font><br>",
					settings->verselist_font_size,
					(const char *)tmpVerseList,
					(const char *)tmpVerseList);
		displayHTML(htmlVL, buf, strlen(buf));
		tmpVerseList++;
		++count;
	}
	endHTML(htmlVL);
	if(count>1){ 
		gtk_widget_show(dlg);
		retval = TRUE;
	}
	return retval;
}

gfloat 
getSwordVerionSWORD(void)
{
	gfloat retval;
	
	retval = mainMgr->Version();
	return retval;
}

/******************************************************************************
 * swaps interlinear mod with mod in main text window
 * intmod - interlinear mod name
 ******************************************************************************/
void
swapmodsSWORD(gchar *intmod)
{
	gchar *modname;
	
	modname = curMod->Name();
	if(!stricmp(settings->Interlinear5Module,intmod)) {
		sprintf(settings->Interlinear5Module,"%s",modname);
	}
	if(!stricmp(settings->Interlinear4Module,intmod)) {
		sprintf(settings->Interlinear4Module,"%s",modname);
	}
	if(!stricmp(settings->Interlinear3Module,intmod)) {
		sprintf(settings->Interlinear3Module,"%s",modname);
	}
	if(!stricmp(settings->Interlinear2Module,intmod)) {
		sprintf(settings->Interlinear2Module,"%s",modname);
	}
	if(!stricmp(settings->Interlinear1Module,intmod)) {
		sprintf(settings->Interlinear1Module,"%s",modname);
	}
	gotoBookmarkSWORD(intmod, current_verse);
	updateinterlinearpage();
}

/*
 *
 */
void loadpreferencemodsSWORD(void)
{
	GtkWidget *dlg;
	ModMap::iterator it; //-- iteratior
	SWMgr *mainMgr2;  
	GList *textMods = NULL;
	GList *commMods = NULL;
	GList *dictMods = NULL;
	GList *percomMods = NULL;	
	
	mainMgr2         = new SWMgr();	//-- create sword mgrs
	for(it = mainMgr2->Modules.begin(); it != mainMgr2->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			textMods = g_list_append(textMods,(*it).second->Name());
		}if(!strcmp((*it).second->Type(), "Commentaries")){
			commMods = g_list_append(commMods,(*it).second->Name());	
		}if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			dictMods = g_list_append(dictMods,(*it).second->Name());
		} 
	}
	//-- set up percom editor module
	for (it = mainMgr2->Modules.begin(); it != mainMgr2->Modules.end(); it++){
		if (!strcmp((*it).second->Type(), "Commentaries")){ //-- if type is 
		 	 //-- if driver is RawFiles			
			if((*mainMgr2->config->Sections[(*it).second->Name()].find("ModDrv")).second == "RawFiles"){ 
				 percomMods = g_list_append(percomMods,(*it).second->Name());	
				g_warning((*it).second->Name());
			} 	                                               
	  	}
	}
	
	dlg = create_dlgSettings(settings,
						textMods,
						commMods,
						dictMods,
						percomMods); /* create propertybox dialog */
	gtk_widget_show(dlg); /* show propertybox */
	
	g_list_free(textMods); //-- free GLists
        g_list_free(commMods);
        g_list_free(dictMods);
	g_list_free(percomMods);
	delete mainMgr2;   //-- delete Sword manager		
}

/*
 *
 */
void gs_firstrunSWORD(void)
{
	GtkWidget *setup;
	ModMap::iterator it; //-- iteratior
	SectionMap::iterator sit; //-- iteratior
	ConfigEntMap::iterator cit; //-- iteratior
	SWMgr *mgr;
	mgr         = new SWMgr();	//-- create sword mgrs
	GList *textMods = NULL;
	GList *commMods = NULL;
	GList *dictMods = NULL;
	GList *percomMods = NULL;
	gint 
		itextmods=0, 
		icommmods=0, 
		idictmods=0,
		ipercommods;
	gchar 
		gtextmods[40],
		gcommmods[40],
		gdictmods[40],
		gpercommods[40],
		*pathtoswordmods;
	
	pathtoswordmods = mgr->prefixPath;
	
	for(it = mgr->Modules.begin(); it != mgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			textMods = g_list_append(textMods,(*it).second->Name());
			++itextmods;
		}if(!strcmp((*it).second->Type(), "Commentaries")){
			commMods = g_list_append(commMods,(*it).second->Name());
			++icommmods;			
		}if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			dictMods = g_list_append(dictMods,(*it).second->Name());
			++idictmods;
		} 
	}
	g_warning(pathtoswordmods);
	sprintf(gtextmods,"%d",itextmods);
	sprintf(gcommmods,"%d",icommmods);
	sprintf(gdictmods,"%d",idictmods);
	setup = create_dlgSetup (textMods, 
					commMods, 
					dictMods,
					gtextmods,
					gcommmods,
					gdictmods,
					pathtoswordmods
					); 
  		gnome_dialog_set_default(GNOME_DIALOG(setup), 2);
		gnome_dialog_run_and_close(GNOME_DIALOG(setup));	
	g_list_free(textMods); //-- free GLists
        g_list_free(commMods);
        g_list_free(dictMods);
	delete mgr;   //-- delete Sword manager		
}

void
applyfontcolorandsizeSWORD(void)
{
	curMod->Display();
	curcomMod->Display();
	curdictMod->Display();
	updateinterlinearpage();	
}

void
updateshortcutbarSWORD(void)
{
	ModMap::iterator it; //-- iteratior
	SWMgr *mgr;
	
	GList *textMods = NULL;
	GList *commMods = NULL;
	GList *dictMods = NULL;
	GList *percomMods = NULL;
		
	mgr = new SWMgr();	//-- create sword mgrs
	
	for(it = mgr->Modules.begin(); it != mgr->Modules.end(); it++){
		if(!strcmp((*it).second->Type(), "Biblical Texts")){
			textMods = g_list_append(textMods,(*it).second->Description());
		}if(!strcmp((*it).second->Type(), "Commentaries")){
			commMods = g_list_append(commMods,(*it).second->Description());
		}if(!strcmp((*it).second->Type(), "Lexicons / Dictionaries")){
			dictMods = g_list_append(dictMods,(*it).second->Description());
		} 
	}
  	update_shortcut_bar(settings,
				textMods, 
				commMods, 
				dictMods);
	g_list_free(textMods); //-- free GLists
        g_list_free(commMods);
        g_list_free(dictMods);
	delete mgr;   //-- delete Sword manager		
	
	
}


