/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_gnomesword.cpp
    * -------------------
    * Mon May 8 2000
    * copyright (C) 2000 by Terry Biggs
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

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#include <gnome.h>
#include <swmgr.h>
#include <markupfiltmgr.h>

#include <swversion.h>
#include <swmodule.h>
#include <swconfig.h>
#include <versekey.h>
#include <localemgr.h>
#include <dirent.h> 
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <gal/widgets/e-unicode.h>

#include "gs_gnomesword.h"
#include "gs_history.h"
#include "sw_display.h"
#include "gs_gui_cb.h"
#include "sw_sword.h"
#include "support.h"
#include "gs_preferences_dlg.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "gs_popup_cb.h"
#include "gs_mainmenu_cb.h"
#include "gs_html.h"
#include "gs_abouts.h"
#include "gs_info_box.h"
#include "gs_setup.h"
#include "gs_shortcutbar.h"
#include "sw_shortcutbar.h"
#include "sw_properties.h"
#include "sw_bookmarks.h"
#include "sw_verselist_sb.h"
#include "sw_module_options.h"
#include "gs_editor.h"
#include "sw_gbs.h"


typedef map < string, string > modDescMap;
typedef map < string, string > bookAbrevMap;
/***********************************************************************************************
 * Sword globals 
***********************************************************************************************/
//SWDisplay *chapDisplay;		/* to display modules using GtkText a chapter at a time */
SWDisplay 
    *comp1Display,		/* to display interlinear modules  a verse at a time */
    *dictDisplay,			/* to display lex/dict modules  */
    *FPNDisplay,		/* to display formatted personal notes using GtkText */
    *commDisplay,		/* to display commentary modules */
   // *bookDisplay,		/* to display gbs modules */
    *UTF8Display;		/* to display modules in utf8 */
    
SWMgr 
    *percomMgr,			/* sword mgr for percomMod - personal comments editor */
    *listMgr,		/* sword mgr for ListEditor */
    *mainMgr1,			/* sword mgr for comp1Mod - interlinear modules */
    *mainMgr;		/* sword mgr for curMod - curcomMod - curdictMod */

VerseKey swKey = "Romans 8:28",	/* temp storage for verse keys */
    vkText,
    vkComm;
SWModule 
    * curMod,		/* module for main text window */
    *comp1Mod,			/* module for first interlinear window */
    *interlinearMod0,			/* module for first interlinear window */
    *interlinearMod1,			/* module for first interlinear window */
    *interlinearMod2,			/* module for first interlinear window */
    *interlinearMod3,			/* module for first interlinear window */
    *interlinearMod4,			/* module for first interlinear window */
    *curcomMod,			/* module for commentary  window */
  //  *curbookMod,			/* module for gen book  window */
    *percomMod,			/* module for personal commentary  window */
    *curdictMod,		/* module for dict window */
    *listMod;			/* module for ListEditor */

modDescMap descriptionMap;
bookAbrevMap abrevationMap;
/***********************************************************************************************
 * GnomeSword globals
***********************************************************************************************/
GList
    * biblemods,
    *commentarymods,
    *dictionarymods,
    *percommods,
    *bookmods, 
    *sbfavoritesmods, 
    *sbbiblemods, 
    *sbdictmods, 
    *sbcommods,
    *sbbookmods,
    *options;
    
GtkWidget * NEtext,		/* note edit widget */
    *MainFrm;			/* main form widget  */
    
gint 
    curChapter = 8,		/* keep up with current chapter */
    curVerse = 28,		/* keep up with current verse */
    answer,			/* do we want to save studybad file on shutdown */
    hebrewpage = 0, greekpage = 0;
    
gboolean 
    noteModified = false,	/* set to true is personal note has changed */
    usepersonalcomments = false,	/* do we setup for personal comments - default is false  */
    ApplyChange = true,		/* should we make changes when cbBook is changed */
    autoSave = true,		/* we want to auto save changes to personal comments */
    havebible = false,		/* let us know if we have at least one bibletext module */
    havedict = false,		/* let us know if we have at least one lex-dict module */
    havecomm = false,		/* let us know if we have at least one commentary module */
    autoscroll = true;		/* commentary module auto scroll when true -- in sync with main text window */
    
gchar 
    com_key[80] = "Rom 8:28",	/* current commentary key */
    *textmod, *commod, *dictmod;

/***********************************************************************************************
 externals
***********************************************************************************************/
extern gboolean 
 changemain,	/* change verse of Bible text window */
 addhistoryitem,		/* do we need to add item to history */
 file_changed,			/* set to true if text is study pad has changed - and file is not saved */
 firstsearch;
extern gint 
 dictpages,		/* number of dictionaries */
 compages,			/* number of commentaries */
 textpages,			/* number of Bible text */
 historyitems;			/* number of history items */
extern SETTINGS * settings, myset;
extern GtkWidget 
        *lang_options_menu,
	*shortcut_bar,
	*strongsnum,
	*footnotes,
	*hebrewpoints,
	*cantillationmarks,
	*greekaccents,
	*morphs,
	*htmlComments;
extern gchar * current_filename,	/* filename for open file in study pad window  */
 current_verse[80],		/* current verse showing in main window, interlinear window - commentary window */
*mycolor, *mycolor;
extern HISTORY historylist[];	/* sturcture for storing history items */

/***********************************************************************************************
 *initSwrod to setup all the Sword stuff
 *mainform - sent here by main.cpp
 ***********************************************************************************************/
void initSWORD(SETTINGS *s)
{ 
	ModMap::iterator it;	//-- iteratior
	ConfigEntMap::iterator eit;	//-- iteratior
	int i,			//-- counter
	 j;			//-- counter 
	gchar * lang;
	GList *tmp;
 
	g_print("gnomesword-%s\n", VERSION);
	g_print("%s\n","Initiating Sword\n");

	mainMgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));  //-- create sword mgrs
	mainMgr1 = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF)); 
	percomMgr = new SWMgr(); 

	curMod = NULL;		//-- set mods to null
	comp1Mod = NULL;
	curcomMod = NULL;
	curdictMod = NULL;
	percomMod = NULL;
	interlinearMod0 = NULL;	//-- module for first interlinear window 
	interlinearMod1 = NULL;			
	interlinearMod2 = NULL;			
	interlinearMod3 = NULL;			
	interlinearMod4 = NULL;			

	comp1Display = 0;	
	dictDisplay = 0;	  
	FPNDisplay = 0;
	commDisplay = 0;
	UTF8Display = 0;
	
	//-- setup versekeys for text and comm windows
	vkText.Persist(1);
	vkComm.Persist(1);
	vkText = settings->currentverse;
	vkComm = settings->currentverse;
	
	//-- set glist to null 
	biblemods = NULL;
	commentarymods = NULL;
	dictionarymods = NULL;
	percommods = NULL;
	sbfavoritesmods = NULL;
	sbbiblemods = NULL;
	sbcommods = NULL;
	sbdictmods = NULL;	
	bookmods = NULL;	
	sbbookmods = NULL;
	options = NULL;
	
	s->displaySearchResults = false;
	s->havethayer = false;
	s->havebdb = false;
	//LocaleMgr::systemLocaleMgr.setDefaultLocaleName( "de" );
	
	MainFrm = s->app;	//-- save mainform for use latter
	NEtext = lookup_widget(s->app, "textComments");	//-- get note edit widget
	//-- setup displays for sword modules
	UTF8Display = new GtkHTMLChapDisp(lookup_widget(s->app, "htmlTexts"),s);
	commDisplay = new GtkHTMLEntryDisp(lookup_widget(s->app, "htmlCommentaries"),s);
	comp1Display = new InterlinearDisp(s->htmlInterlinear,s);
	FPNDisplay = new GtkHTMLEntryDisp(htmlComments,s);
	dictDisplay = new GtkHTMLEntryDisp(lookup_widget(s->app, "htmlDict"),s);
	compages = 0;
	dictpages = 0;


	if (s->showsplash) {
		while (gtk_events_pending())
			gtk_main_iteration();
	}
	
	g_print("%s\n", "Loading SWORD Moudules");
	
	for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
		descriptionMap[string ((char *) (*it).second->Description())] =
		    string((char *) (*it).second->Name());
		
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			curMod = (*it).second;
			havebible = TRUE;
			++textpages;
			biblemods = g_list_append(biblemods, curMod->Name());
			sbbiblemods = g_list_append(sbbiblemods, curMod->Description());
			curMod->Disp(UTF8Display);
			curMod->SetKey(vkText);
		} 
		
		else if (!strcmp((*it).second->Type(), "Commentaries")) {	//-- set commentary modules                
			curcomMod = (*it).second;
			commentarymods =  g_list_append(commentarymods, curcomMod->Name());
			sbcommods = g_list_append(sbcommods, curcomMod->Description());
			havecomm = TRUE;	//-- we have at least one commentay module
			++compages;	//-- how many pages do we have  
			curcomMod->Disp(commDisplay);
			curcomMod->SetKey(vkComm);    
		} 
		
		else if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")) {	//-- set dictionary modules        
			havedict = TRUE;	//-- we have at least one lex / dict module
			++dictpages;	//-- how many pages do we have
			curdictMod = (*it).second;
			dictionarymods = g_list_append(dictionarymods, curdictMod->Name());
			sbdictmods = g_list_append(sbdictmods, curdictMod->Description());
			curdictMod->Disp(dictDisplay);	
			if(!strcmp(curdictMod->Name(),"Thayer"))
				s->havethayer = true;
			if(!strcmp(curdictMod->Name(),"BDB"))
				s->havebdb = true;
		} 
		
		else if (!strcmp((*it).second->Type(), "Generic Books")) { 			
			bookmods = g_list_append(bookmods, (*it).second->Name());
			sbbookmods = g_list_append(sbbookmods, (*it).second->Description());
		}
	}
	//-- setup Generic Book Support
	setupSW_GBS(s);
	
	//-- set up percom editor module
	for (it = percomMgr->Modules.begin();
	     it != percomMgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Commentaries")) {	//-- if type is 
			//-- if driver is RawFiles                     
			if ((*percomMgr->config->Sections[(*it).second->Name()].find("ModDrv")).second == "RawFiles") {
				percomMod = (*it).second;
				percomMod->Disp(FPNDisplay);	
				percommods = g_list_append(percommods, percomMod->Name());
				usepersonalcomments = TRUE;	//-- used by verseChange function (sw_gnomesword.cpp)
				percomMod->SetKey(s->currentverse);
				gtk_widget_show(lookup_widget(s->app, "vbox2"));	//-- show personal comments page because we
			}	//-- have at least one personl module
		}
	}
	
	//-- interlinear	
	for (it = mainMgr1->Modules.begin(); it != mainMgr1->Modules.end(); it++) {
		comp1Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			interlinearMod0 = (*it).second;
			interlinearMod1 = (*it).second;
			interlinearMod2 = (*it).second;
			interlinearMod3 = (*it).second;
			interlinearMod4 = (*it).second;
			comp1Mod->Disp(comp1Display);
			if(interlinearMod0) interlinearMod0->Disp(comp1Display);
			if(interlinearMod1) interlinearMod1->Disp(comp1Display);
			if(interlinearMod2) interlinearMod2->Disp(comp1Display);
			if(interlinearMod3) interlinearMod3->Disp(comp1Display);
			if(interlinearMod4) interlinearMod4->Disp(comp1Display);
		}
	} 
	
	//-- get list of  globalOptions for menus
	OptionsList optionslist = mainMgr->getGlobalOptions();
	for (OptionsList::iterator it = optionslist.begin(); it != optionslist.end(); it++) {	
		//-- save options in a glist for popup menus
		options = g_list_append(options, (gchar *)(*it).c_str());
	}	
}

void modNameFromDesc(gchar * modName, gchar * modDesc)
{
	strcpy(modName, descriptionMap[modDesc].c_str());
}

/*** new changeVerseSWORD  ***/
void ChangeVerseSWORD(void)	
{
	int l;
	GList * mods;
	gboolean searchresults = FALSE;
	//gchar * currRef;
	
	strcpy(current_verse, vkText);
	ApplyChange = false;
	//--------------------------------------------------- change main window			
	if (changemain && havebible) {
		if (curMod) {	curMod->SetKey(vkText);
			//----------- add verse to history menu
			if (addhistoryitem) {
				if (strcmp(settings->currentverse, historylist[historyitems - 1].verseref))
					addHistoryItem(settings->app,
							GTK_WIDGET(shortcut_bar),
						       settings->currentverse);
			}			
			addhistoryitem = TRUE;
			//----------------------- remember last verse
			strcpy(settings->currentverse, curMod->KeyText());	
			char s1[255], s2[255];
			curVerse = vkText.Verse();
			curChapter = vkText.Chapter();
			sprintf(s1, "%s", (const char *)vkText );
			//------------------------- set book, chapter,verse and freeform lookup entries to new verse
			gtk_entry_set_text(GTK_ENTRY(lookup_widget
					    (settings->app, "cbeBook")),
					   vkText.books[vkText.Testament()-1][vkText.Book()-1].name);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(settings->app,"spbChapter")),
						  curChapter);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(settings->app, "spbVerse")),
						  curVerse);
			gtk_entry_set_text(GTK_ENTRY(lookup_widget(settings->app, "cbeFreeformLookup")), 
						vkText);
			curMod->Display();
		}
	}
	changemain = TRUE;
	
	//--------------------------------------------------------------- change interlinear verses
	if(settings->dockedInt)
		updateinterlinearpage();
	//else
		//updateIntDlg(settings);
	
	//------------------------------- change personal notes editor	 if not in edit mode
	if (settings->notebook3page == 1) {
		if (settings->notefollow) {  //-- if personal notes follow button is active (on)                   
			if (!settings->editnote){
				if (usepersonalcomments && percomMod) {
					percomMod->SetKey(vkComm); //-- set personal module to current verse
					percomMod->Display();	//-- show change
					noteModified = false;	//-- we just loaded comment so it is not modified 					
				}
			}
		}
	}
	
	//-- set commentary module to current verse
	if (settings->notebook3page == 0 && autoscroll) {
		
		searchresults = settings->displaySearchResults;
		settings->displaySearchResults = FALSE;
		
		if (curcomMod) {
			curcomMod->SetKey(vkComm);	
			curcomMod->Display();	//-- show change
			strcpy(com_key, vkComm);
		}
				
		settings->displaySearchResults = searchresults;
	}
	
	ApplyChange = TRUE;
}

/********************************************************************************************** 
 * changeVerse - this function changes all currently used sword Bible and commentary modules 
 *               to (ref)
 * gchar *ref - pointer to new module key (verse)
 *********************************************************************************************/
void changeVerseSWORD(gchar * ref)	
{
	VerseKey key;
	
	key = ref;
	
	if((!key.Chapter()) || (!key.Verse())) {	
		vkText.AutoNormalize(0);
		vkComm.AutoNormalize(0);
	}
	
	vkText = ref;
	vkComm = ref;
	
	ChangeVerseSWORD();
	
	vkText.AutoNormalize(1);
	vkComm.AutoNormalize(1);
}

void changeVerseComSWORD(void)	
{
	VerseKey key;
	
	key = current_verse;
	
	if((!key.Chapter()) || (!key.Verse())) {
		vkComm.AutoNormalize(0);
	}
	
	vkComm = current_verse;
	
	curcomMod->SetKey(vkComm);
	curcomMod->Display();
	
	vkComm.AutoNormalize(1);
}

GtkHTMLStream *htmlstream;
/*** please fix me ***/
void updateIntDlg(SETTINGS *s)
{	
	gchar 
		*utf8str,
		*bgColor,
		*textColor,
		buf[500], 
		*tmpkey;
	gint utf8len;
	
	//-- setup gtkhtml widget
	GtkHTMLStreamStatus status1;	
	GtkHTML *html = GTK_HTML(settings->htmlInterlinear);
	gboolean was_editable = gtk_html_get_editable (html);	
	if (was_editable)
	   gtk_html_set_editable (html, FALSE); 
	htmlstream = gtk_html_begin_content(html, "text/html; charset=utf-8");
	
	interlinearMod0 = mainMgr1->Modules[s->Interlinear1Module];
	interlinearMod1 = mainMgr1->Modules[s->Interlinear2Module];
	interlinearMod2 = mainMgr1->Modules[s->Interlinear3Module];
	interlinearMod3 = mainMgr1->Modules[s->Interlinear4Module];
	interlinearMod4 = mainMgr1->Modules[s->Interlinear5Module];
	
	
	sprintf(buf,
	 "<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
			s->bible_bg_color,
			s->bible_text_color, s->link_color);
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	

	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear1Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear2Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear3Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear4Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	sprintf(buf,"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",s->Interlinear5Module);		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	sprintf(buf,"%s","</tr>");		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	
	/******      ******/
	IntDisplay(s);
	
	sprintf(buf,"%s","</table></body></html>");		
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
	}
	
	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable); 
	sprintf(buf, "%d", s->intCurVerse);
	gtk_html_jump_to_anchor(html, buf);
}

/*
 * Sets up the interlinear html widget for each 
 * interlinear module
 */
void updateinterlinearpage(void)
{	
	gchar tmpBuf[256];
	gchar *utf8str;
	gint utf8len;
	
	if(havebible) {
		interlinearMod0 = mainMgr1->Modules[settings->Interlinear1Module];
		interlinearMod1 = mainMgr1->Modules[settings->Interlinear2Module];
		interlinearMod2 = mainMgr1->Modules[settings->Interlinear3Module];
		interlinearMod3 = mainMgr1->Modules[settings->Interlinear4Module];
		interlinearMod4 = mainMgr1->Modules[settings->Interlinear5Module];
		//-- setup gtkhtml widget
		GtkHTMLStreamStatus status1;	
		GtkHTML *html = GTK_HTML(settings->htmlInterlinear);
		gboolean was_editable = gtk_html_get_editable (html);	
		if (was_editable)
		   gtk_html_set_editable (html, FALSE); 
		htmlstream = gtk_html_begin_content(html, "text/html; charset=utf-8");
		
		
		sprintf(tmpBuf,
			"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table>",
			settings->bible_bg_color,
			settings->bible_text_color, settings->link_color);
		utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, tmpBuf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}
		
		if(interlinearMod0) interlinearMod0->SetKey(current_verse);
		if(interlinearMod1) interlinearMod1->SetKey(current_verse);
		if(interlinearMod2) interlinearMod2->SetKey(current_verse);
		if(interlinearMod3) interlinearMod3->SetKey(current_verse);
		if(interlinearMod4) interlinearMod4->SetKey(current_verse);
		if(interlinearMod0) interlinearMod0->Display();
		if(interlinearMod1) interlinearMod1->Display();
		if(interlinearMod2) interlinearMod2->Display();
		if(interlinearMod3) interlinearMod3->Display();
		if(interlinearMod4) interlinearMod4->Display();
		sprintf(tmpBuf, "</table></body></html>");
		utf8str = e_utf8_from_gtk_string(settings->htmlInterlinear, tmpBuf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;	
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}
		
		gtk_html_end(GTK_HTML(html), htmlstream, status1);
		gtk_html_set_editable(html, was_editable); 		
	}
}


/******************************************************************************
* fill clist with dictionary keys 
* number of keys depends on hight of list widget and size of font
* code orginally from BibleCS
*******************************************************************************/
void FillDictKeysSWORD(void)
{
	ModMap::iterator it;	//-- iterator to go through modules and find modName
	int index = 0,		//-- for index into list widget
	 i,			//-- counters
	 j, count;
	SWKey saveKey;		//-- for starting point
	gchar * entryText,	//-- pointer to string to search for
	    *listitem;		//-- hold item until added to list
	GtkWidget * list;	//-- list widget to display items found

	list = lookup_widget(settings->app, "list1");	//-- get pointer to list widget
	entryText = gtk_entry_get_text(GTK_ENTRY(lookup_widget(settings->app, "dictionarySearchText")));	//-- get key to use form user
	j = 0;
	it = mainMgr->Modules.find(curdictMod->Name());	//-- find module to use for search
	if (it != mainMgr->Modules.end()) {	//-- if we dont reach the end of our modules 
		SWModule *mod = (*it).second;	//-- temp module to work with
		mod->KeyText();	//-- snap to entry
		saveKey = mod->KeyText();	//-- save our place
		count = GTK_CLIST(list)->clist_window_height / GTK_CLIST(list)->row_height;	//-- how many items do we need to fill our list 
		gtk_clist_clear(GTK_CLIST(list));	//-- start with empty list
		if (mod) {	//-- make sure we have module to work with             
			for (i = 0; i < (count / 2); i++)
				(*mod)++;	//-- get equal number of keys before and after our starting key(saveKey)
			for (i = 0; i < count - 1; i++)
				(*mod)--;
			mod->Error();	//-- clear errors
			for (; !mod->Error() && count; count--, (*mod)++) {
				++j;
				listitem = g_strdup((const char *) mod->KeyText());	//-- key to listitem
				gtk_clist_append(GTK_CLIST(list), &listitem);	//-- listitem to list
				g_free(listitem);
				if (saveKey == mod->Key())	//-- if we are back to starting place set index
					index = j - 1;
			}
		}
		gtk_clist_moveto(GTK_CLIST(list), index, 0, 0.5, 0.0);	//-- move in list to index
		mod->SetKey(saveKey);
	}
}

//-------------------------------------------------------------------------------------------
void shutdownSWORD(void)	//-- close down GnomeSword program
{
	//char *msg;
	GtkWidget *msgbox;
	extern gchar
		*gSwordDir,
		*shortcutbarDir,
		*fnquickmarks,
		*fnconfigure,
		*swbmDir;
	
	savebookmarks(settings->ctree_widget);
	saveconfig(settings, true);
	
	if (settings->modifiedSP) {	//-- if study pad file has changed since last save  
		msgbox = create_InfoBox();
		gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
		answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
		
		switch (answer) {
		case 0:
			if (settings->studypadfilename) 
				save_file_program_end(settings->htmlSP, settings->studypadfilename);
			break;
		default:
			break;
		}
	}
	
	g_free(gSwordDir);
	g_free(shortcutbarDir);
	g_free(fnquickmarks);
	g_free(fnconfigure);
	g_free(swbmDir);
	
	g_list_free(options);
	g_list_free(settings->settingslist);
	shutdownverselistSBSWORD();
	shutdownSW_GBS();

	//-- delete Sword managers
	delete mainMgr;
	delete mainMgr1;
	delete percomMgr;
	//-- delete Sword displays
	if (UTF8Display)
		delete UTF8Display;
	if (comp1Display)
		delete comp1Display;
	if (dictDisplay)
		delete dictDisplay;
	if (FPNDisplay)
		delete FPNDisplay;
	if (commDisplay)
		delete commDisplay;
	gtk_exit(0);		//-- exit
}

/*******************************************************************************
 * toggle global options on and off
 * option - the option user wants to toggle
 * window - the window to effect - text or interlinear
 * choice - true = on, false = off
 ******************************************************************************/
void globaloptionsSWORD(gchar *option, gint window, gboolean choice, gboolean showchange)
{
	gchar *on_off;
	if (choice) {
			on_off = "On";
		} else {
			on_off = "Off";
		}
	switch (window) {
	case 0:		// main text window 
		mainMgr->setGlobalOption(option, on_off);		
		save_module_options(curMod->Name(), option, on_off);
		
		if(!strcmp(option, "Strong's Numbers")) {		
			GTK_CHECK_MENU_ITEM(settings->strongsnum)->active = choice;			
			/* set strongs toogle button */
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(settings->app,"btnStrongs")), choice);	
		}
		
		if(!strcmp(option,"Footnotes" )) {
			GTK_CHECK_MENU_ITEM(settings->footnotes)->active = choice;		
			/* set footnotes toogle button */
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(settings->app,"btnFootnotes")), choice);	
		}
		
		if(!strcmp(option, "Morphological Tags")) {
			GTK_CHECK_MENU_ITEM(settings->morphs)->active = choice;	
			/* set morphs toogle button */
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(settings->app,"btnMorphs")), choice);	
		}	
		
		if(!strcmp(option, "Hebrew Vowel Points")) {
			GTK_CHECK_MENU_ITEM(settings->hebrewpoints)->active = choice;
			//settings->hebrewpoints = choice;
		}
		
		if(!strcmp(option, "Hebrew Cantillation")) {
			GTK_CHECK_MENU_ITEM(settings->cantillationmarks)->active = choice;
			//settings->cantillationmarks = choice;
		}
		
		if(!strcmp(option, "Greek Accents")) {
			GTK_CHECK_MENU_ITEM(settings->greekaccents)->active = choice;
			//settings->greekaccents = choice;
		}
		
		if (havebible && showchange) {
			curMod->Display();
		}
		
		break;
	case 1:		// interlinear window   
		mainMgr1->setGlobalOption(option, on_off);
		
		
		if(!strcmp(option, "Strong's Numbers")) {		
			settings->strongsint = choice;	
		}
		
		if(!strcmp(option,"Footnotes" )) {
			settings->footnotesint = choice;
		}
		
		if(!strcmp(option, "Morphological Tags")) {
			settings->morphsint = choice;
		}			
		
		if(!strcmp(option, "Hebrew Vowel Points")) {
			settings->hebrewpointsint = choice;
		}
		
		if(!strcmp(option, "Hebrew Cantillation")) {
			settings->cantillationmarksint = choice;
		}
		
		if(!strcmp(option, "Greek Accents")) {
			settings->greekaccentsint = choice;
		}
		
		if(settings->dockedInt && havebible)
			updateinterlinearpage();
		else
			updateIntDlg(settings);
		
		break;
	}
}

/* 
 * gotoBookmarkSWORD - bad hack but it works
 * bad name since we now use it for more than bookmarks
 *
 * searchs for module by modName - increments bibleindex, commindex or dictindex
 * until module is found then sets the text, comm or dict notebook page to index. 
 * key is new verse key or dict/lex key
 */
void gotoBookmarkSWORD(gchar * modName, gchar * key)
{
	GtkWidget * notebook, *entry;
	ModMap::iterator it;
	gint bibleindex = 0, commindex = 0, dictindex = 0, bookindex = 0;

	for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
		     
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			
			if (!strcmp((*it).second->Name(), modName)) {
				notebook =
				    lookup_widget(settings->app, "nbTextMods");
				gtk_notebook_set_page(GTK_NOTEBOOK(notebook), bibleindex);
				vkText = key;
				if (settings->notebook3page == 0 && autoscroll) 
					vkComm = key;
				ChangeVerseSWORD();
				return;
			}
			++bibleindex;
			
		} 
		
		else if (!strcmp((*it).second->Type(), "Commentaries")) {
			if (!strcmp((*it).second->Name(), modName)) {
				notebook =
				    lookup_widget(settings->app, "notebook1");
				gtk_notebook_set_page(GTK_NOTEBOOK(notebook), commindex);
				vkComm = key;
				if (autoscroll) 
					vkText = key;
				ChangeVerseSWORD();
				return;
			}
			++commindex;
			
		} 
		
		else if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")) {
			if (!strcmp((*it).second->Name(), modName)) {
				entry =
				    lookup_widget(settings->app, "dictionarySearchText");
				notebook =
				    lookup_widget(settings->app, "notebook4");
				gtk_notebook_set_page(GTK_NOTEBOOK(notebook), dictindex);
				gtk_entry_set_text(GTK_ENTRY(entry), key);
				return;
			}
			++dictindex;
		} 
		
		else if (!strcmp((*it).second->Type(), "Generic Books")) {
			if (!strcmp((*it).second->Name(), modName)) {				
				gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebookGBS), bookindex);	
				return;
			}
			++bookindex;
		}
	}
}

/******************************************************************************
* change text module for main window
* someone clicked 
*******************************************************************************/
void changecurModSWORD(gchar * modName, gboolean showchange)
{
	ModMap::iterator it;	//-- or clicked the mainwindow popup menu and the callback sent us here
	GtkWidget *frame;
	gchar title[200];
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;
	GList *tmp;
	bool value;
	
	tmp = NULL;
	if (havebible) {
		it = mainMgr->Modules.find(modName);	//-- iterate through the modules until we find modName
		if (it != mainMgr->Modules.end()) {	//-- if we find the module   
			curMod = (*it).second;	//-- change current module to new module
			if ((sit = mainMgr->config->Sections.find(curMod->Name())) != mainMgr->config->Sections.end()) {				
				ConfigEntMap &section = (*sit).second;
				//-- do we have a CipherKey= tag?
				if((entry = section.find("CipherKey")) != section.end()) { //-- set sensitivity of unlock mod menu item
					gtk_widget_set_sensitive(settings->unlocktextmod_item, TRUE);
				}else{
					gtk_widget_set_sensitive(settings->unlocktextmod_item, FALSE);
				}
				/*** set global options here ***/
				tmp = options;
				while(tmp != NULL) {
					value = load_module_options((*it).second->Name(), (gchar*)tmp->data);					
					globaloptionsSWORD((gchar*)tmp->data, MAIN_TEXT_WINDOW, value, FALSE);
					//g_warning("%s = %d",(gchar*)tmp->data,value);	
					tmp = g_list_next(tmp);
				}
				g_list_free(tmp);
			} 
			if (showchange) {
				curMod->SetKey(current_verse);	//-- set key to current verse
				curMod->Display();	//-- show it to the world
			}
		}
		strcpy(settings->MainWindowModule, (gchar *) curMod->Name());	//-- remember where we are so we can open here next time we startup
		sprintf(title, "GnomeSWORD - %s", (gchar *) curMod->Description());	//curMod->Description());              
		gtk_window_set_title(GTK_WINDOW(settings->app), title);
	}
	frame = lookup_widget(settings->app, "frame9");
	if (!settings->text_tabs)
		gtk_frame_set_label(GTK_FRAME(frame), curMod->Name());	//-- set frame label
	else
		gtk_frame_set_label(GTK_FRAME(frame), NULL);	//-- set frame label

}

/******************************************************************************
* change sword module for interlinear window
* sent here by updateinterlinearpage()
*******************************************************************************/
void changecomp1ModSWORD(gchar * modName)
{
	ModMap::iterator it;
	
	it = mainMgr1->Modules.find(modName);	//-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != mainMgr1->Modules.end()) {	//-- if we find the module      
		comp1Mod = (*it).second;	//-- change current module to new module
		comp1Mod->SetKey(current_verse);	//-- set key to current verse
		comp1Mod->Display();	//-- show it to the world
	}
}


/******************************************************************************
* set verse style -- verses or paragraphs
*******************************************************************************/
void setversestyleSWORD(gboolean choice)
{
	settings->versestyle = choice;	//-- remember our choice for the next program startup
	if (havebible)
		curMod->Display();	//-- show the change
}


/*** change Bible book ***/
void bookSWORD(void)		//-- someone changed book combo
{
	gchar *bookname, buf[256];
	gint iChap, iVerse;
	
	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(settings->app,"cbeBook")));
	
	sprintf(buf,"%s %d:%d", bookname, 1, 1);
	vkText = buf;
	vkComm = buf;
	
	ChangeVerseSWORD();
}

/*
//-------------------------------------------------------------------------------------------
void chapterSWORD(void)		//-- someone clicked the chapter spin button
{
	gchar *bookname, buf[256];
	gint iChap, iVerse;
	
	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(settings->app,"cbeBook")));
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(settings->app,"spbChapter")));
	
	if((!iChap)||(!iVerse)){
		vkText.AutoNormalize(0);
		vkComm.AutoNormalize(0);
	}
		
	if(iChap < 0){
		++iChap;
	}
	sprintf(buf,"%s %d:%d", bookname, iChap, 1);
	vkText = buf;
	vkComm = buf;
		
	ChangeVerseSWORD();
	vkText.AutoNormalize(1);
	vkComm.AutoNormalize(1);
}
*/
//-------------------------------------------------------------------------------------------
void verseSWORD(void)		//-- someone clicked the verse spin button
{
	gchar *bookname, buf[256];
	gint iChap, iVerse;
	
	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(settings->app,"cbeBook")));
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(settings->app,"spbChapter")));
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(settings->app,"spbVerse")));
	if((!iChap)||(!iVerse)){
		vkText.AutoNormalize(0);
		vkComm.AutoNormalize(0);
	}
	
	if(iVerse < 0){
		++iVerse;
	}
	if(iChap < 0){
		++iChap;
	}
	sprintf(buf,"%s %d:%d", bookname, iChap, iVerse);
	vkText = buf;
	vkComm = buf;
		
	ChangeVerseSWORD();
	vkText.AutoNormalize(1);
	vkComm.AutoNormalize(1);
}

//-------------------------------------------------------------------------------------------
gchar *intchangeverseSWORD(GtkWidget *book, GtkWidget *chapter, GtkWidget *verse, GtkWidget *entry)		//-- someone clicked the verse spin button
{
	VerseKey vkey;
	gchar *retval;	
	gchar *bookname, *newbook, buf[256];
	gint iChap, iVerse;
	
	bookname = gtk_entry_get_text(GTK_ENTRY(book));
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(chapter));
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(verse));
	
	sprintf(buf,"%s %d:%d", bookname, iChap, iVerse);
	vkey.AutoNormalize(1);
	vkey = buf;
	iChap = vkey.Chapter();
	iVerse = vkey.Verse();
	newbook = (gchar*)vkey.books[vkey.Testament()-1][vkey.Book()-1].name;
	if(strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(book), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(chapter), iChap);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(verse), iVerse);
	strcpy(buf,vkey);
	gtk_entry_set_text(GTK_ENTRY(entry), buf);
	retval = buf;
	return retval;		
}
//-------------------------------------------------------------------------------------------
gchar *intsyncSWORD(GtkWidget *book,  //-- someone clicked the sync button on the interlinary
			GtkWidget *chapter, 
			GtkWidget *verse, 
			GtkWidget *entry,
			gchar *key)		
{
	VerseKey 
		vkey;	
	
	gchar 
		*bookname, 
		*newbook,  
		*retval, 
		buf[256];
	
	gint 
		iChap, 
		iVerse;	
		
	cout << "key = " << key << "\n";
	vkey.AutoNormalize(1);
	vkey = key;
	iChap = vkey.Chapter();
	iVerse = vkey.Verse();
	newbook = (gchar*)vkey.books[vkey.Testament()-1][vkey.Book()-1].name;
	if(strcmp(bookname, newbook))
		gtk_entry_set_text(GTK_ENTRY(book), newbook);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(chapter), iChap);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(verse), iVerse);
	strcpy(buf,vkey);
	gtk_entry_set_text(GTK_ENTRY(entry), buf);
	retval = g_strdup(buf);
	return retval;
}

//-------------------------------------------------------------------------------------------
void btnlookupSWORD(void)	
{
	gchar *buf;		//-- pointer to entry string

	//ApplyChange = false;	//-- do not want to start loop with book combo box
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(settings->app, "cbeFreeformLookup")));	//-- set pointer to entry text
	changeVerseSWORD(buf);	//-- change verse to entry text 
}

//-------------------------------------------------------------------------------------------
void freeformlookupSWORD(GdkEventKey * event)	//-- change to verse in freeformlookup entry
{
	gchar *buf;		//-- pointer to entry string

	//ApplyChange = false;	//-- do not want to start loop with book combo box
	buf = gtk_entry_get_text(GTK_ENTRY(lookup_widget(settings->app, "cbeFreeformLookup")));	//-- set pointer to entry text
	if (event->keyval == 65293 || event->keyval == 65421) {	//-- if user hit return key continue
		changeVerseSWORD(buf);	//-- change verse to entry text         
	}
}

//-------------------------------------------------------------------------------------------
void changcurcomModSWORD(gchar * modName, gboolean showchange)	//-- someone changed commentary notebook page (sent here by callback function notebook page change)
{
	ModMap::iterator it;
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;
	GtkWidget *frame;
	gchar *cKey;

	if (havecomm) {
		curcomMod = mainMgr->Modules[modName];
		cKey = (gchar*)curcomMod->getConfigEntry("CipherKey");
		//-- change tab label to current commentary name
		gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(settings->workbook),
			gtk_notebook_get_nth_page(GTK_NOTEBOOK(settings->workbook),0), 
			(gchar*)curcomMod->Name());
		
		if(cKey)
			gtk_widget_set_sensitive(settings->unlockcommmod_item, TRUE);
		else
			gtk_widget_set_sensitive(settings->unlockcommmod_item, FALSE);
		
		if(curcomMod) {
			if (showchange) {
				if (autoscroll)
					curcomMod->SetKey(curMod->KeyText());	//-- go to text (verse)
				curcomMod->Display();	//-- show the change
				strcpy(settings->CommWindowModule,
				       curcomMod->Name());
			}
			
		}
		
	}
	
	frame = lookup_widget(settings->app, "frameCom");
	gtk_frame_set_label(GTK_FRAME(frame), NULL);	//-- set frame label
}

//-------------------------------------------------------------------------------------------
void savenoteSWORD(gchar *buf)	//-- save personal comments
{
	if(buf)
		*percomMod << (const char *) buf;	//-- save note!
	noteModified = false;	//-- we just saved the note so it has not been modified 
}


//-------------------------------------------------------------------------------------------
void deletenoteSWORD(void)	//-- delete personal comment
{	
	GtkWidget 
		*label1,
		*label2,
		*label3,
		*msgbox;
	
	msgbox = create_InfoBox();
	label1 = lookup_widget(msgbox,"lbInfoBox1");	
	label2 = lookup_widget(msgbox,"lbInfoBox2");	
	label3 = lookup_widget(msgbox,"lbInfoBox3");
	gtk_label_set_text(GTK_LABEL(label1), "Are you sure you want");
	gtk_label_set_text(GTK_LABEL(label2), "to delete the note for");
	gtk_label_set_text(GTK_LABEL(label3), (gchar*)percomMod->KeyText());
	
	gnome_dialog_set_default(GNOME_DIALOG(msgbox), 2);
	answer = gnome_dialog_run_and_close(GNOME_DIALOG(msgbox));
	switch (answer) {
		case 0:				 
			percomMod->deleteEntry();	//-- delete note
			percomMod->Display();	//-- show change
			noteModified = false;	//-- we just deleted the note so it has not been modified 
			break;
		default:
			break;
	}	
}

//-------------------------------------------------------------------------------------------
void changcurdictModSWORD(gchar * modName, gchar * keyText)	//-- someone changed dict notebook page - sent here by notebook callback
{				//-- modName form page label - keyText from dict lookup entry
	ModMap::iterator it;
	GtkWidget *frame;

	it = mainMgr->Modules.find(modName);	//-- find module we want to use
	if (it != mainMgr->Modules.end()) {
		curdictMod = (*it).second;	//-- set curdictMod to new choice
		curdictMod->SetKey(keyText);	//-- set key to text from lookup entry
		curdictMod->Display();	//-- display new dict
		FillDictKeysSWORD();	//-- fill the list widget with keys
		strcpy(settings->DictWindowModule, curdictMod->Name());
		
		//-- change tab label to current dict/lex name
		gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(settings->workbook_lower),
			gtk_notebook_get_nth_page(GTK_NOTEBOOK(settings->workbook_lower),0), 
			settings->DictWindowModule);
		
	}
	frame = lookup_widget(settings->app, "frame10");
	gtk_frame_set_label(GTK_FRAME(frame), NULL);	//-- set frame label
}

/*
 *dictSearchTextChangedSWORD - dict lookup text changed
 *  mytext from the dict dict lookup entry
 */
void dictSearchTextChangedSWORD(char *mytext)
{				//-- mytext from the dict lookup entry
	strcpy(settings->dictkey, mytext);	//-- remember the key
	if (curdictMod) {	//-- if we have a dict module  
		if (strcmp(mytext, "")) {	//-- if text is not null                
			curdictMod->SetKey(mytext);	//-- set key to our text
			curdictMod->Display();	//-- show what we found
			FillDictKeysSWORD();	//-- fill the list widget with keys        
		}
	}
}

//-------------------------------------------------------------------------------------------
void dictchangekeySWORD(gint direction)	//-- dict change key up or down -- arrow buttons
{
	gchar *buf;

	if (direction == 1) {	//-- next key  
		(*curdictMod)++;	//-- move up one
	} else if (direction == 0) {	//-- previous key  
		(*curdictMod)--;	//-- move down one
	}
	curdictMod->Display();	//-- show the changes
	//-- put new key into dictionary text entry
	gtk_entry_set_text(GTK_ENTRY
			   (lookup_widget
			    (settings->app, "dictionarySearchText")),
			   curdictMod->KeyText());
	buf = g_strdup(curdictMod->KeyText());
	dictSearchTextChangedSWORD(buf);
	g_free(buf);
}

//-------------------------------------------------------------------------------------------
void changepercomModSWORD(gchar * modName)	//-- change personal comments module
{
	GtkWidget *notebook,	//-- pointer to a notebook widget
	*label;			//-- pointer to a label widget
	ModMap::iterator it;	//-- module iterator

	if (noteModified)
		return;		//savenoteSWORD(noteModified);  //-- if personal comments changed save changes before we change modules and lose our changes
	it = percomMgr->Modules.find(modName);	//-- find commentary module (modName from page label)
	if (it != percomMgr->Modules.end()) {	//-- if we don't run out of mods before we find the one we are looking for 
		percomMod = (*it).second;	//-- set curcomMod to modName
		strcpy(settings->personalcommentsmod, percomMod->Name());
		if (havebible)
			percomMod->SetKey(curMod->KeyText());	//-- go to text (verse)
		//-- let's change the notebook label to match our percomMod (current personal comments module)
		notebook = lookup_widget(settings->app, "notebook3");	//-- get the notebook our page is in]
		label = gtk_label_new(percomMod->Name());	//-- create new label with mod name as the text
		gtk_widget_show(label);	//-- make is visible
		gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 1), label);	//-- put label on personal comments page
		noteModified = false;
	}
}

//-------------------------------------------------------------------------------------------
void showmoduleinfoSWORD(char *modName, gboolean isGBS)	//--  show module information in an about dialog
{
	GtkWidget * aboutbox;	//-- pointer to about dialog        
	GtkWidget * text;	//-- pointer to text widget of dialog
	gchar * buf,		//-- pointer to text buffer for label (mod name)
	    *bufabout,		//-- pointer to text buffer for text widget (mod about)
	    *newbuf, discription[500];
	gint len;
	ModMap::iterator it;	//-- module iterator
	SectionMap::iterator sit;	//--  iterator
	ConfigEntMap::iterator cit;	//-- iterator

	bufabout = "oops";
	buf = mainMgr->Modules[modName]->Description();
	bufabout = (gchar*)mainMgr->Modules[modName]->getConfigEntry("About");
	
	sprintf(discription,
		"<FONT COLOR=\"#000FCF\"><center><b>%s</b></center></font><HR>",
		buf);
	if(!isGBS) {
		aboutbox = create_aboutmodules();	//-- create about dialog
		gtk_widget_show(aboutbox);
	}
	
	if (strcmp(bufabout, "oops")) {
		len = strlen(bufabout);
		newbuf = new char[len + 600];
		
		if(!isGBS) {
			text = lookup_widget(aboutbox, "textModAbout");	//-- get text widget
		}
		
		else {
			text = settings->htmlBook;
		}
		
		AboutModsDisplayHTML(newbuf, bufabout);	//-- send about info and alocated new text buffer to display function (sw_display.cpp)
		beginHTML(text, FALSE);
		displayHTML(text, "<html><body>", strlen("<html><body>"));
		displayHTML(text, discription, strlen(discription));
		displayHTML(text, newbuf, strlen(newbuf));
		displayHTML(text, "</body></html>",
			    strlen("</body></html>"));
		endHTML(text);
		delete[]newbuf;		
	} 
	
	else
		g_warning(bufabout);
}

/******************************************************************************
 * setglobalopsSWORD
 * option - option to set
 * yesno - yes or no
******************************************************************************/
void setglobalopsSWORD(gint window, gchar * option, gchar * yesno)
{
	/* turn option on or off */
	switch (window) {
	case 0:	/*** Bible text window ***/
		mainMgr->setGlobalOption(option, yesno);
		break;
	case 1:	/*** interlinear window ***/
		mainMgr1->setGlobalOption(option, yesno);
		break;
	}
}

/******************************************************************************
 *redisplayTextSWORD - display Bible text in main window
******************************************************************************/
void redisplayTextSWORD(void)
{
	if (havebible)
		curMod->Display();
}

/******************************************************************************
 *getmodnameSWORD
 * num
 * returns module name
******************************************************************************/
gchar *getmodnameSWORD(gint num)
{
	if (havebible) {
		switch (num) {
		case 0:
			return curMod->Name();
			break;
		case 1:
			return curcomMod->Name();
			break;
		case 2:
			return curdictMod->Name();
			break;
		case 3:
			return comp1Mod->Name();
			break;
		}
	}
}

/******************************************************************************
 *getmodkeySWORD
 * num
 * returns module key
******************************************************************************/
gchar *getmodkeySWORD(gint num)
{
	if (havebible) {
		switch (num) {
		case 0:
			return (gchar *) curMod->KeyText();
			break;
		case 1:
			return (gchar *) curcomMod->KeyText();
			break;
		case 2:
			return (gchar *) curdictMod->KeyText();
			break;
		case 3:
			return (gchar *) comp1Mod->KeyText();
			break;
		}
	}

}

/*** navigate the current commentary module ***/
void navcurcomModSWORD(gint direction)	//-- navigate the current commentary module
{
	switch (direction) {
	case 0:
		(*curcomMod)--;
		break;
	case 1:
		(*curcomMod)++;
		break;
	}
	curcomMod->Error();	//-- clear any errors
	curcomMod->Display();
}


/*** returns the name of the current commentary module ***/
gchar *getcommodSWORD(void)
{
	return curcomMod->Name();
}

/*** returns the name of the current dict/lex module ***/
gchar *getdictmodSWORD(void)
{
	return curdictMod->Name();
}

/*** returns the name of the current Bible text module ***/
gchar *gettextmodSWORD(void)
{
	return curMod->Name();
}


/*** returns the description of the current commentary module ***/
gchar *getcommodDescriptionSWORD(void)
{
	return (char *) curcomMod->Description();;
}

/******************************************************************************
* returns a list of the books of the Bible
* GList *list needs to be freed by calling function
*******************************************************************************/
GList *getBibleBooksSWORD(void)
{
	VerseKey DefaultVSKey;
	GList *glist = NULL;
	GString *s1;

	/*** load Bible books ***/
	DefaultVSKey = TOP;
	while (!DefaultVSKey.Error()) {
		s1 = g_string_new((const char *) DefaultVSKey);
		s1 = g_string_truncate(s1, (s1->len - 4));
		glist = g_list_append(glist, s1->str);
		DefaultVSKey.Book(DefaultVSKey.Book() + 1);
		g_string_free(s1, FALSE);
	}
	return glist;
}

/*** returns the version number of the sword libs ***/
const char *getSwordVerionSWORD(void)
{
	SWVersion retval;
	
	retval = SWVersion::currentVersion;
	return retval;
}

/******************************************************************************
 * swaps interlinear mod with mod in main text window
 * intmod - interlinear mod name
 ******************************************************************************/
void swapmodsSWORD(gchar * intmod)
{
	gchar *modname;

	modname = curMod->Name();
	if (!stricmp(settings->Interlinear5Module, intmod)) {
		sprintf(settings->Interlinear5Module, "%s", modname);
	}
	if (!stricmp(settings->Interlinear4Module, intmod)) {
		sprintf(settings->Interlinear4Module, "%s", modname);
	}
	if (!stricmp(settings->Interlinear3Module, intmod)) {
		sprintf(settings->Interlinear3Module, "%s", modname);
	}
	if (!stricmp(settings->Interlinear2Module, intmod)) {
		sprintf(settings->Interlinear2Module, "%s", modname);
	}
	if (!stricmp(settings->Interlinear1Module, intmod)) {
		sprintf(settings->Interlinear1Module, "%s", modname);
	}
	gotoBookmarkSWORD(intmod, current_verse);
	updateinterlinearpage();
}

/*** we come here first to get module list to pass to the preferences dialog ***/
void loadpreferencemodsSWORD(void)
{
	GtkWidget *dlg;
	ModMap::iterator it;	//-- iteratior
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;	
	SWMgr *Mgr;
	GList *textMods = NULL;
	GList *commMods = NULL;
	GList *dictMods = NULL;
	GList *percomMods = NULL;
	GList *devotionMods = NULL;
	string feature;
	gint devotionals = 0;
	Mgr = new SWMgr();	//-- create sword mgr
	for (it = Mgr->Modules.begin(); it != Mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			textMods =
			    g_list_append(textMods, (*it).second->Name());
		}
		if (!strcmp((*it).second->Type(), "Commentaries")) {
			commMods =
			    g_list_append(commMods, (*it).second->Name());
		}
		if (!strcmp
		    ((*it).second->Type(), "Lexicons / Dictionaries")) {
			sit = Mgr->config->Sections.find((*it).second->Name());
			ConfigEntMap & section = (*sit).second;
			feature = ((entry = section.find("Feature")) != section.end()) ? (*entry).second : (string) "";
			dictMods =
			    g_list_append(dictMods, (*it).second->Name());
			if (!stricmp(feature.c_str(), "DailyDevotion")) {
				devotionMods = g_list_append(devotionMods,(*it).second->Name());
				++devotionals;
			}
		}
	}
		
	if(!devotionals)
		settings->showdevotional = FALSE;
	
	//-- set up percom editor module
	for (it = Mgr->Modules.begin(); it != Mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Commentaries")) {	//-- if type is 
			//-- if driver is RawFiles                     
			if ((*Mgr->config->Sections[(*it).second->Name()].
			     find("ModDrv")).second == "RawFiles") {
				percomMods =
				    g_list_append(percomMods,
						  (*it).second->Name());
			}
		}
	}
	/* create preferences dialog */
	dlg = create_dlgSettings(settings,
				 textMods, commMods, dictMods, percomMods, devotionMods);
	gtk_widget_show(dlg);	/* show preferences dialog */

	g_list_free(textMods);	//-- free GLists
	g_list_free(commMods);
	g_list_free(dictMods);
	g_list_free(percomMods);
	g_list_free(devotionMods);
	delete Mgr;		//-- delete Sword manager         
}

/******************************************************************************
 * we come here first to get module lists and other info  to 
 * pass to the setup durid - this should happen the first time gnomesword
 * is started
 ******************************************************************************/
void gs_firstrunSWORD(void)
{
	GtkWidget *setup;
	ModMap::iterator it;	//-- iteratior
	SectionMap::iterator sit;	//-- iteratior
	ConfigEntMap::iterator cit;	//-- iteratior
	SWMgr *mgr;
	GList
	    * textMods = NULL,
	    *commMods = NULL, *dictMods = NULL, *percomMods = NULL;
	gint itextmods = 0, icommmods = 0, idictmods = 0, ipercommods;
	gchar
	    gtextmods[40],
	    gcommmods[40],
	    gdictmods[40], gpercommods[40], *pathtoswordmods;

	mgr = new SWMgr();	//-- create sword mgrs
	pathtoswordmods = mgr->prefixPath;

	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			textMods =
			    g_list_append(textMods, (*it).second->Name());
			++itextmods;
		}
		if (!strcmp((*it).second->Type(), "Commentaries")) {
			commMods =
			    g_list_append(commMods, (*it).second->Name());
			++icommmods;
		}
		if (!strcmp
		    ((*it).second->Type(), "Lexicons / Dictionaries")) {
			dictMods =
			    g_list_append(dictMods, (*it).second->Name());
			++idictmods;
		}
	}
	sprintf(gtextmods, "%d", itextmods);
	sprintf(gcommmods, "%d", icommmods);
	sprintf(gdictmods, "%d", idictmods);
	/*** create setup durid ***/
	setup = create_dlgSetup(textMods,
				commMods,
				dictMods,
				gtextmods,
				gcommmods, gdictmods, pathtoswordmods);
	/*** hold util we are done ***/
	gnome_dialog_set_default(GNOME_DIALOG(setup), 2);
	gnome_dialog_run_and_close(GNOME_DIALOG(setup));
	/*** free GLists ***/
	g_list_free(textMods);
	g_list_free(commMods);
	g_list_free(dictMods);
	/*** delete Sword manager ***/
	delete mgr;
}

/*** the changes are already made we just need to show them ***/
void applyfontcolorandsizeSWORD(void)
{
	curMod->Display();
	curcomMod->Display();
	curdictMod->Display();
	updateinterlinearpage();
}

/*** most of this code is from an example in swmgr.h sword-1.5.2 ***/
void savekeySWORD(gint modwindow, gchar * key)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	DIR *dir;
	gchar buf[256], conffile[256], *modName;
	struct dirent *ent;
	
	switch(modwindow) {
		case MAIN_TEXT_WINDOW:
			modName = curMod->Name();
		break;
		case COMMENTARY_WINDOW:
			modName = curcomMod->Name();
		break;
		case DICTIONARY_WINDOW:
			modName = curdictMod->Name();
		break;
	}
	
	sprintf(buf,"%s",mainMgr->configPath); 
	dir = opendir(buf);	
	//char *modFile;
	if (dir) {		//-- find and update .conf file
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, "."))
			    && (strcmp(ent->d_name, ".."))) {
				sprintf(conffile,"%s/%s",buf,ent->d_name);
				SWConfig *myConfig = new SWConfig(conffile);
				section = myConfig->Sections.find(modName);
				if (section != myConfig->Sections.end()) {
					entry = section->second.find("CipherKey");
					if (entry != section->second.end()) {
						entry->second = key;	//-- set cipher key
						myConfig->Save();	//-- save config file
					}
				}
				delete myConfig;
			}
		}
	}
	closedir(dir);
	
	//-- display module with new cipher key 	
	switch(modwindow) {
		case MAIN_TEXT_WINDOW:	
			mainMgr1->setCipherKey(modName, key); /* show change on interlinear page */
		case COMMENTARY_WINDOW:
			mainMgr->setCipherKey(modName, key);
			ChangeVerseSWORD();
		break;
		case DICTIONARY_WINDOW:			
			mainMgr->setCipherKey(modName, key);
			curdictMod->Display();
		break;
	}
}

/*** write individual module information to <module>.conf ***/
gboolean savefontinfoSWORD(gchar *modName, gchar *modtag, gchar * info)
{
	ModMap::iterator it; 
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	DIR *dir;
	gchar buf[256], conffile[256], *path;
	struct dirent *ent;	
	bool retval = false;
	
	sprintf(buf,"%s",mainMgr->configPath); 
	dir = opendir(buf);
	if (dir) {		//-- find and update .conf file
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
				sprintf(conffile,"%s/%s",buf,ent->d_name);
				SWConfig myConfig(conffile);
				myConfig[modName][modtag] = info;
				myConfig.Save();
				retval = true;
			}
		}
	}
	closedir(dir);
	return retval;
}

/*** display daily devotional ***/
void displayDevotional(void)
{
	gchar buf[80];
	time_t curtime;
	struct tm *loctime;	
	
	/* Get the current time. */
	curtime = time (NULL);

	/* Convert it to local time representation. */
	loctime = localtime (&curtime);

	/* Print it out in a nice format. */
	strftime (buf, 80, "%m.%d", loctime);

	displaydictlexSBSW(settings->devotionalmod, buf, settings);
	setupforDailyDevotion(settings);
}

/*** we come here to get module type - Bible text, Commentary or Dict/Lex ***/
gint get_mod_typeSWORD(gchar *modName)
{
	
	ModMap::iterator it;	//-- iteratior
	SectionMap::iterator sit;	//-- iteratior
	ConfigEntMap::iterator cit;	//-- iteratior
	SWMgr *mgr;
	
	mgr = new SWMgr();	//-- create sword mgrs
	it = mgr->Modules.find(modName);	//-- iterate through the modules until we find modName
	if (it != mgr->Modules.end()) {	//-- if we find the module   
	
			if (!strcmp((*it).second->Type(), "Biblical Texts")) {				
				return 0;
			}
		
			if (!strcmp((*it).second->Type(), "Commentaries")) {				
				return 1;				
			}
			
			if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")) {				
				return 2;				
			}
			
			if (!strcmp((*it).second->Type(), "Generic Books")) {				
				return 3;				
			}
	}
	return -1;
	/*** delete Sword manager ***/
	delete mgr;
}

