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
#include <config.h>
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
#include <string.h>
#include <time.h>
#include <gal/widgets/e-unicode.h>

#include "gs_gnomesword.h"
#include "gs_history.h"
#include "display.h"
#include "gs_gui_cb.h"
#include "sword.h"
#include "support.h"
#include "gs_preferences_dlg.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "gs_popup_cb.h"
#include "gs_mainmenu_cb.h"
#include "gs_html.h"
#include "gs_info_box.h"
#include "gs_setup.h"
#include "gs_shortcutbar.h"
#include "shortcutbar.h"
#include "properties.h"
#include "bookmarks.h"
#include "verselist.h"
#include "module_options.h"
#include "gs_editor.h"
#include "gbs.h"
#include "gs_gbs.h"
#include "dictlex.h"
#include "gs_dictlex.h"
#include "commentary.h"
#include "gs_commentary.h"
#include "search.h"


typedef map < string, string > modDescMap;
typedef map < string, string > bookAbrevMap;

/***********************************************************************************************
 * Sword globals 
***********************************************************************************************/
//SWDisplay *chapDisplay;               /* to display modules using GtkText a chapter at a time */
SWDisplay * comp1Display,	/* to display interlinear modules  a verse at a time */
    *FPNDisplay,		/* to display formatted personal notes using GtkText */
    //*commDisplay,             /* to display commentary modules */
    *UTF8Display;		/* to display modules in utf8 */

SWMgr * percomMgr,		/* sword mgr for percomMod - personal comments editor */
    *listMgr,			/* sword mgr for ListEditor */
    *mainMgr1,			/* sword mgr for comp1Mod - interlinear modules */
    *mainMgr;			/* sword mgr for curMod - curcomMod - curdictMod */

VerseKey swKey = "Romans 8:28",	/* temp storage for verse keys */
 vkText, vkComm;
SWModule * curMod,		/* module for main text window */
    *comp1Mod,			/* module for first interlinear window */
    *interlinearMod0,		/* module for first interlinear window */
    *interlinearMod1,		/* module for first interlinear window */
    *interlinearMod2,		/* module for first interlinear window */
    *interlinearMod3,		/* module for first interlinear window */
    *interlinearMod4,		/* module for first interlinear window */
    *curcomMod,			/* module for commentary  window */
    //  *curbookMod,                      /* module for gen book  window */
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
    *sbbiblemods, *sbdictmods, *sbcommods, *sbbookmods, *options;

GtkWidget *NEtext,		/* note edit widget */
*MainFrm;			/* main form widget  */

gint curChapter = 8,		/* keep up with current chapter */
    curVerse = 28,		/* keep up with current verse */
    answer,			/* do we want to save studybad file on shutdown */
    hebrewpage = 0, greekpage = 0;

gboolean noteModified = false,	/* set to true is personal note has changed */
    usepersonalcomments = false,	/* do we setup for personal comments - default is false  */
    ApplyChange = true,		/* should we make changes when cbBook is changed */
    autoSave = true,		/* we want to auto save changes to personal comments */
    havebible = false,		/* let us know if we have at least one bibletext module */
    havedict = false,		/* let us know if we have at least one lex-dict module */
    havecomm = false,		/* let us know if we have at least one commentary module */
    autoscroll = true;		/* commentary module auto scroll when true -- in sync with main text window */

gchar com_key[80] = "Rom 8:28",	/* current commentary key */
    *textmod, *commod, *dictmod;

/***********************************************************************************************
 externals
***********************************************************************************************/
extern gboolean changemain,	/* change verse of Bible text window */
 addhistoryitem,		/* do we need to add item to history */
 file_changed,			/* set to true if text is study pad has changed - and file is not saved */
 firstsearch;
extern gint dictpages,		/* number of dictionaries */
 compages,			/* number of commentaries */
 textpages,			/* number of Bible text */
 historyitems;			/* number of history items */
extern SETTINGS *settings, myset;
extern GtkWidget
    * lang_options_menu,
    *shortcut_bar,
    *strongsnum,
    *footnotes,
    *hebrewpoints,
    *cantillationmarks, *greekaccents, *morphs, *htmlComments;
extern gchar *current_filename,	/* filename for open file in study pad window  */
 current_verse[80],		/* current verse showing in main window, interlinear window - commentary window */
*mycolor, *mycolor;
extern HISTORY historylist[];	/* sturcture for storing history items */

void backend_first_init(void)
{
	mainMgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
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
	FPNDisplay = 0;
	UTF8Display = 0;

}

/***********************************************************************************************
 *initSwrod to setup all the Sword stuff
 *mainform - sent here by main.cpp
 ***********************************************************************************************/
void initSWORD(SETTINGS * s)
{
	ModMap::iterator it;	//-- iteratior
	ConfigEntMap::iterator eit;	//-- iteratior
	int i,			//-- counter
	 j;			//-- counter 
	GList *tmp;

	g_print("gnomesword-%s\n", VERSION);
	g_print("%s\n", "Initiating Sword\n");

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
	UTF8Display =
	    new GtkHTMLChapDisp(lookup_widget(s->app, "htmlTexts"), s);
	//commDisplay = new GtkHTMLEntryDisp(lookup_widget(s->app, "htmlCommentaries"),s);
	comp1Display = new InterlinearDisp(s->htmlInterlinear, s);
	FPNDisplay = new GtkHTMLEntryDisp(htmlComments, s);
	//dictDisplay = new GtkHTMLEntryDisp(lookup_widget(s->app, "htmlDict"),s);
	compages = 0;
	dictpages = 0;


	if (s->showsplash) {
		while (gtk_events_pending())
			gtk_main_iteration();
	}

	g_print("Sword locale is %s\n",
		LocaleMgr::systemLocaleMgr.getDefaultLocaleName());

	g_print("%s\n", "Loading SWORD Moudules");

	for (it = mainMgr->Modules.begin();
	     it != mainMgr->Modules.end(); it++) {
		descriptionMap[string
			       ((char *) (*it).second->Description())] =
		    string((char *) (*it).second->Name());

		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			curMod = (*it).second;
			havebible = TRUE;
			++textpages;
			biblemods =
			    g_list_append(biblemods, curMod->Name());
			sbbiblemods =
			    g_list_append(sbbiblemods,
					  curMod->Description());
			curMod->Disp(UTF8Display);
			curMod->SetKey(vkText);
		}

		else if (!strcmp((*it).second->Type(), "Commentaries")) {	//-- set commentary modules                
			curcomMod = (*it).second;
			commentarymods =
			    g_list_append(commentarymods,
					  (*it).second->Name());
			sbcommods =
			    g_list_append(sbcommods,
					  (*it).second->Description());
			havecomm = TRUE;	//-- we have at least one commentay module
			++compages;	//-- how many pages do we have 
		}

		else if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")) {	//-- set dictionary modules        
			havedict = TRUE;	//-- we have at least one lex / dict module
			++dictpages;	//-- how many pages do we have
			curdictMod = (*it).second;
			dictionarymods =
			    g_list_append(dictionarymods,
					  (*it).second->Name());
			sbdictmods =
			    g_list_append(sbdictmods,
					  (*it).second->Description());
		}

		else if (!strcmp((*it).second->Type(), "Generic Books")) {
			bookmods =
			    g_list_append(bookmods,
					  (*it).second->Name());
			sbbookmods =
			    g_list_append(sbbookmods,
					  (*it).second->Description());
		}
	}
	//-- setup Commentary Support, Generic Book Support and Dict/Lex Support
	backend_setupCOMM(s);
	backend_setupDL(s);
	backend_setupGBS(s);
	//-- set up percom editor module
	for (it = percomMgr->Modules.begin();
	     it != percomMgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Commentaries")) {	//-- if type is 
			//-- if driver is RawFiles                     
			if ((*percomMgr->config->
			     Sections[(*it).second->Name()].
			     find("ModDrv")).second == "RawFiles") {
				percomMod = (*it).second;
				percomMod->Disp(FPNDisplay);
				percommods =
				    g_list_append(percommods,
						  percomMod->Name());
				usepersonalcomments = TRUE;	//-- used by verseChange function (sw_gnomesword.cpp)
				percomMod->SetKey(s->currentverse);
				gtk_widget_show(lookup_widget(s->app, "vbox2"));	//-- show personal comments page because we
			}	//-- have at least one personl module
		}
	}

	//-- interlinear        
	for (it = mainMgr1->Modules.begin();
	     it != mainMgr1->Modules.end(); it++) {
		comp1Mod = (*it).second;
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			interlinearMod0 = (*it).second;
			interlinearMod1 = (*it).second;
			interlinearMod2 = (*it).second;
			interlinearMod3 = (*it).second;
			interlinearMod4 = (*it).second;
			comp1Mod->Disp(comp1Display);
			if (interlinearMod0)
				interlinearMod0->Disp(comp1Display);
			if (interlinearMod1)
				interlinearMod1->Disp(comp1Display);
			if (interlinearMod2)
				interlinearMod2->Disp(comp1Display);
			if (interlinearMod3)
				interlinearMod3->Disp(comp1Display);
			if (interlinearMod4)
				interlinearMod4->Disp(comp1Display);
		}
	}

	//-- get list of  globalOptions for menus
	OptionsList optionslist = mainMgr->getGlobalOptions();
	for (OptionsList::iterator it = optionslist.begin();
	     it != optionslist.end(); it++) {
		//-- save options in a glist for popup menus
		options =
		    g_list_append(options, (gchar *) (*it).c_str());
	}
}

GtkHTMLStream *htmlstream;
/*** please fix me ***/
void updateIntDlg(SETTINGS * s)
{
	gchar * utf8str, *bgColor, *textColor, buf[500], *tmpkey;
	gint utf8len;

	//-- setup gtkhtml widget
	GtkHTMLStreamStatus status1;
	GtkHTML *html = GTK_HTML(settings->htmlInterlinear);
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");

	interlinearMod0 = mainMgr1->Modules[s->Interlinear1Module];
	interlinearMod1 = mainMgr1->Modules[s->Interlinear2Module];
	interlinearMod2 = mainMgr1->Modules[s->Interlinear3Module];
	interlinearMod3 = mainMgr1->Modules[s->Interlinear4Module];
	interlinearMod4 = mainMgr1->Modules[s->Interlinear5Module];


	sprintf(buf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><table align=\"left\" valign=\"top\"><tr valign=\"top\" >",
		s->bible_bg_color, s->bible_text_color, s->link_color);
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	if (interlinearMod0) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear1Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (interlinearMod1) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear2Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (interlinearMod2) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear3Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (interlinearMod3) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear4Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	if (interlinearMod4) {
		sprintf(buf,
			"<td valign=\"top\" width=\"20%\" bgcolor=\"#f1f1f1\"><b>%s</b></td>",
			s->Interlinear5Module);
		utf8str =
		    e_utf8_from_gtk_string(s->htmlInterlinear, buf);
		utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream,
				       utf8str, utf8len);
		}
	}

	sprintf(buf, "%s", "</tr>");
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}


	/******      ******/
	IntDisplay(s);

	sprintf(buf, "%s", "</table></body></html>");
	utf8str = e_utf8_from_gtk_string(s->htmlInterlinear, buf);
	utf8len = strlen(utf8str);	//g_utf8_strlen (utf8str , -1) ;        
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
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
char *backend_get_interlinear_module_text(char * mod_name, char *key)
{
	interlinearMod0 =
		    mainMgr1->Modules[mod_name];
	if (interlinearMod0)
			interlinearMod0->SetKey(key);
	else 
		return NULL;
	return g_strdup((gchar*)interlinearMod0->RenderText());	
	
}

void backend_text_module_change_verse(gchar * key)
{
	vkText = key;
	//--------------------------------------------------- change main window                        
	if (havebible) {
		if (curMod) {
			curMod->SetKey(vkText);
			curMod->Display();
		}
	}

	//------------------------------- change personal notes editor   if not in edit mode
	if (settings->notebook3page == 1) {
		if (settings->notefollow) {	//-- if personal notes follow button is active (on)                   
			if (!settings->editnote) {
				if (usepersonalcomments && percomMod) {
					percomMod->SetKey(key);	//-- set personal module to current verse
					percomMod->Display();	//-- show change
					noteModified = false;	//-- we just loaded comment so it is not modified                                       
				}
			}
		}
	}
}


void backend_module_name_from_description(gchar * mod_name, gchar * mod_desc)
{
	strcpy(mod_name, descriptionMap[mod_desc].c_str());
}

//-------------------------------------------------------------------------------------------
void backend_shutdown(SETTINGS * s)	//-- close down GnomeSword program
{
	savebookmarks(s->ctree_widget);
	backend_save_properties(s, true);

	backend_shut_down_verselist();
	backend_shutdownGBS();
	backend_shutdownDL();
	backend_shutdownCOMM();
	backend_shutdown_search_results_display();
	backend_shutdown_sb_viewer();


	//-- delete Sword managers
	delete mainMgr;
	delete mainMgr1;
	delete percomMgr;

	//-- delete Sword displays
	if (UTF8Display)
		delete UTF8Display;
	if (comp1Display)
		delete comp1Display;
	if (FPNDisplay)
		delete FPNDisplay;
	g_print("\nwe are done with Sword\n");
}

/******************************************************************************
* change text module for main window
* someone clicked 
*******************************************************************************/
void backend_change_text_module(gchar * modName, gboolean showchange)
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
			if ((sit =
			     mainMgr->config->Sections.find(curMod->
							    Name())) !=
			    mainMgr->config->Sections.end()) {
				ConfigEntMap & section = (*sit).second;
				//-- do we have a CipherKey= tag?
				if ((entry = section.find("CipherKey")) != section.end()) {	//-- set sensitivity of unlock mod menu item
					gtk_widget_set_sensitive
					    (settings->
					     unlocktextmod_item, TRUE);
				} else {
					gtk_widget_set_sensitive
					    (settings->
					     unlocktextmod_item, FALSE);
				}
				/*** set global options here ***/
				tmp = options;
				while (tmp != NULL) {
					value =
					    backend_load_module_options((*it).second->Name(), (gchar *) tmp->data);
					set_module_global_options((gchar *)
							   tmp->data,
							   MAIN_TEXT_WINDOW,
							   value,
							   FALSE);
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
* set verse style -- verses or paragraphs
*******************************************************************************/
void backend_set_verse_style(gboolean choice)
{
	settings->versestyle = choice;	//-- remember our choice for the next program startup
	if (havebible)
		curMod->Display();	//-- show the change
}


char *backend_get_valid_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return 	g_strdup((char *) vkey.getText());
}

char *backend_get_book_from_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return 	(char *) vkey.books[vkey.Testament() - 1][vkey.Book() -
						       1].name;
}

int backend_get_chapter_from_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.Chapter();	
}

int backend_get_verse_from_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.Verse();	
}
//-------------------------------------------------------------------------------------------
void backend_save_personal_comment(gchar * buf)	//-- save personal comments
{
	if (buf)
		*percomMod << (const char *) buf;	//-- save note!
	noteModified = false;	//-- we just saved the note so it has not been modified 
}


//-------------------------------------------------------------------------------------------
void backend_delete_personal_comment(void)	//-- delete personal comment
{
	GtkWidget * label1, *label2, *label3, *msgbox;

	msgbox = create_InfoBox();
	label1 = lookup_widget(msgbox, "lbInfoBox1");
	label2 = lookup_widget(msgbox, "lbInfoBox2");
	label3 = lookup_widget(msgbox, "lbInfoBox3");
	gtk_label_set_text(GTK_LABEL(label1), "Are you sure you want");
	gtk_label_set_text(GTK_LABEL(label2), "to delete the note for");
	gtk_label_set_text(GTK_LABEL(label3),
			   (gchar *) percomMod->KeyText());

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
void backend_change_percom_module(gchar * modName)	//-- change personal comments module
{
	GtkWidget *notebook,	//-- pointer to a notebook widget
	*label;			//-- pointer to a label widget
	ModMap::iterator it;	//-- module iterator

	if (noteModified)
		return;		//backend_save_personal_comment(noteModified);  //-- if personal comments changed save changes before we change modules and lose our changes
	it = percomMgr->Modules.find(modName);	//-- find commentary module (modName from page label)
	if (it != percomMgr->Modules.end()) {	//-- if we don't run out of mods before we find the one we are looking for 
		percomMod = (*it).second;	//-- set curcomMod to modName
		strcpy(settings->personalcommentsmod,
		       percomMod->Name());
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



/******************************************************************************
 * 
 * option - option to set
 * yesno - yes or no
******************************************************************************/
void backend_set_global_option(gint window, gchar * option, gchar * yesno)
{
	/* turn option on or off */
	switch (window) {
	case MAIN_TEXT_WINDOW:/*** Bible text window ***/
		mainMgr->setGlobalOption(option, yesno);
		break;
	case INTERLINEAR_WINDOW:/*** interlinear window ***/
		mainMgr1->setGlobalOption(option, yesno);
		break;
	}
}

/**********************************************************************
 * Name
 *   backend_getBibleBooksSWORD(void)
 *
 * Synopsis
 *   #include "sw_sword.h"
 *   
 *   GList *backend_getBibleBooksSWORD(void);
 *
 * Description
 *   Returns a list of the books of the Bible.
 *   GList *list needs to be freed by calling function.
 *
 *   This is called before initSWORD! :o(
 *
 * Return value
 *   GList pointer of books of the Bible
 */

GList *backend_get_books(void)
{
	VerseKey key;
	GList *glist = NULL;

	/*
	   * Load Bible books.
	 */

	for (int i = 0; i <= 1; ++i) {
		for (int j = 0; j < key.BMAX[i]; ++j) {
			glist =
			    g_list_append(glist,
					  (char *) key.books[i][j].
					  name);
		}
	}
	return glist;
}

/*** returns the version number of the sword libs ***/
const char *backend_get_sword_verion(void)
{
	SWVersion retval;

	retval = SWVersion::currentVersion;
	return retval;
}

/******************************************************************************
 * swaps interlinear mod with mod in main text window
 * intmod - interlinear mod name
 ******************************************************************************/
void backend_swap_interlinear_with_main(gchar * intmod, SETTINGS *s)
{
	gchar *modname;

	modname = s->MainWindowModule;
	if (!stricmp(s->Interlinear5Module, intmod)) {
		sprintf(s->Interlinear5Module, "%s", modname);
	}
	if (!stricmp(s->Interlinear4Module, intmod)) {
		sprintf(s->Interlinear4Module, "%s", modname);
	}
	if (!stricmp(s->Interlinear3Module, intmod)) {
		sprintf(s->Interlinear3Module, "%s", modname);
	}
	if (!stricmp(s->Interlinear2Module, intmod)) {
		sprintf(s->Interlinear2Module, "%s", modname);
	}
	if (!stricmp(s->Interlinear1Module, intmod)) {
		sprintf(s->Interlinear1Module, "%s", modname);
	}
	change_module_and_key(intmod, current_verse);
	update_interlinear_page(settings);
}

/*** the changes are already made we just need to show them ***/
void backend_display_new_font_color_and_size(SETTINGS *s)
{
	curMod->Display();
	gui_displayCOMM(s->currentverse);
	backend_displayinDL(s->DictWindowModule, s->dictkey);
	update_interlinear_page(settings);
}

/*** most of this code is from an example in swmgr.h sword-1.5.2 ***/
void backend_save_module_key(gint modwindow, gchar * key, SETTINGS *s)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	DIR *dir;
	gchar buf[256], conffile[256], *modName;
	struct dirent *ent;

	switch (modwindow) {
	case MAIN_TEXT_WINDOW:
		modName = s->MainWindowModule;
		break;
	case COMMENTARY_WINDOW:
		modName = s->CommWindowModule;
		break;
	case DICTIONARY_WINDOW:
		modName = s->DictWindowModule;
		break;
	}

	sprintf(buf, "%s", mainMgr->configPath);
	dir = opendir(buf);
	//char *modFile;
	if (dir) {		//-- find and update .conf file
		rewinddir(dir);
		while ((ent = readdir(dir))) {
			if ((strcmp(ent->d_name, "."))
			    && (strcmp(ent->d_name, ".."))) {
				sprintf(conffile, "%s/%s", buf,
					ent->d_name);
				SWConfig *myConfig =
				    new SWConfig(conffile);
				section =
				    myConfig->Sections.find(modName);
				if (section != myConfig->Sections.end()) {
					entry =
					    section->second.
					    find("CipherKey");
					if (entry !=
					    section->second.end()) {
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
	switch (modwindow) {
	case MAIN_TEXT_WINDOW:
		mainMgr1->setCipherKey(modName, key);	/* show change on interlinear page */
	case COMMENTARY_WINDOW:
		mainMgr->setCipherKey(modName, key);
		//backend_text_module_change_verse(key);
		break;
	case DICTIONARY_WINDOW:
		mainMgr->setCipherKey(modName, key);
		curdictMod->Display();
		break;
	}
}

/*** display daily devotional ***/
void backend_display_devotional(SETTINGS * s)
{
	gchar buf[80];
	time_t curtime;
	struct tm *loctime;

	/* Get the current time. */
	curtime = time(NULL);

	/* Convert it to local time representation. */
	loctime = localtime(&curtime);

	/* Print it out in a nice format. */
	strftime(buf, 80, "%m.%d", loctime);

	display_dictlex_in_viewer(s->devotionalmod, buf, s);
	set_sb_for_daily_devotion(s);
}

/*** we come here to get module type - Bible text, Commentary, Dict/Lex or Book ***/
int backend_get_mod_type(gchar * mod_name)
{

	ModMap::iterator it;	//-- iteratior
	SectionMap::iterator sit;	//-- iteratior
	ConfigEntMap::iterator cit;	//-- iteratior
	SWMgr *mgr;

	mgr = new SWMgr();	//-- create sword mgrs
	it = mgr->Modules.find(mod_name);	//-- iterate through the modules until we find modName
	if (it != mgr->Modules.end()) {	//-- if we find the module   

		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
		    /*** delete Sword manager ***/
			delete mgr;
			return TEXT_TYPE;
		}

		if (!strcmp((*it).second->Type(), COMM_MODS)) {
		    /*** delete Sword manager ***/
			delete mgr;
			return COMMENTARY_TYPE;
		}

		if (!strcmp
		    ((*it).second->Type(), DICT_MODS)) {
		    /*** delete Sword manager ***/
			delete mgr;
			return DICTIONARY_TYPE;
		}

		if (!strcmp((*it).second->Type(), BOOK_MODS)) {
		    /*** delete Sword manager ***/
			delete mgr;
			return BOOK_TYPE;
		}
	}
	/*** delete Sword manager ***/
	delete mgr;
	return -1;
}

GList *backend_get_list_of_mods_by_type(char *mod_type)
{
	ModMap::iterator it;
	GList *mods = NULL;

	for (it = mainMgr->Modules.begin();
	     it != mainMgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), mod_type)) {
			mods =
			    g_list_append(mods,
					  (gchar *) (*it).second->
					  Name());
		}
	}
	return mods;
}

GList *backend_get_list_of_devotion_modules(void)
{
	ModMap::iterator it;
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;	
	
	string feature;
	GList *mods = NULL;
	for (it = mainMgr->Modules.begin();
	     it != mainMgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), DICT_MODS)) {
			sit = mainMgr->config->Sections.find((*it).second->Name());
			ConfigEntMap & section = (*sit).second;
			feature = ((entry = section.find("Feature")) !=
			     section.end())? (*entry).
			    second : (string) "";
			if(!stricmp(feature.c_str(), "DailyDevotion")) {
				mods = g_list_append(mods, (*it).second->Name());
			}
		}
	}
	return mods;
}

GList *backend_get_list_of_percom_modules(void)
{
	ModMap::iterator it;
	GList *mods = NULL;
	for (it = mainMgr->Modules.begin();it != mainMgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), COMM_MODS)) {	
			//-- if driver is RawFiles                     
			if ((*percomMgr->config->Sections[(*it).second->Name()].
			     find("ModDrv")).second == "RawFiles") {
				mods = g_list_append(mods, (*it).second->Name());
			}	
		}
	}
	return mods;
}

/* returns a glist of module descriptions
 * modType - type of modules 
 */
GList *backend_get_mod_description_list_SWORD(char *mod_type)
{
	ModMap::iterator it;
	GList *mods = NULL;

	for (it = mainMgr->Modules.begin();
	     it != mainMgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), mod_type)) {
			mods =
			    g_list_append(mods,
					  (gchar *) (*it).second->
					  Description());
		}
	}
	return mods;
}

gchar *backend_get_module_description(gchar * modName)
{
	ModMap::iterator it;	//-- iteratior

	it = mainMgr->Modules.find(modName);
	if (it != mainMgr->Modules.end()) {
		return (*it).second->Description();
	}
	return NULL;
}

/***  returns path to sword modules must be freed by calling function  ***/
gchar *backend_get_path_to_mods(void)
{
	return g_strdup(mainMgr->prefixPath);
}

/******************************************************************************
 * Name
 *   backend_get_mod_aboutSWORD
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   gchar *backend_get_mod_aboutSWORD(gchar *modname)
 *
 * Description
 *   return the about information from the <module>.conf
 *
 * Return value
 *   gchar *
 */
gchar *backend_get_mod_aboutSWORD(gchar * modname)
{
	return g_strdup((gchar *) mainMgr->Modules[modname]->
			getConfigEntry("About"));
}

int backend_get_module_page(char * module_name, char * module_type)
{	
	ModMap::iterator it;
	gint module_index = 0;

	for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
		     
		if (!strcmp((*it).second->Type(), module_type)) {
			
			if (!strcmp((*it).second->Name(), module_name)) {
				return module_index;
			}
			++module_index;		
		}
	}
	return -1;
}

char *backend_get_module_font_name(char *mod_name)
{
	SWModule *mod = mainMgr1->Modules[mod_name];
	char *buf = (gchar*)mod->getConfigEntry("Font");
	return buf;	
}
/******   end of file   ******/
