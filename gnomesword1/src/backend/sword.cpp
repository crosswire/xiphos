/*
 * GnomeSword Bible Study Tool
 * sword.cpp support for sword modules
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include "gs_menu.h"
#include "gs_popup_cb.h"
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
#include "commentary_.h"
#include "commentary.h"
#include "bibletext.h"
#include "bibletext_.h"
#include "percomm_.h"
#include "search_.h"
#include "interlinear.h"
#include "gs_interlinear.h"
#include "bibletext.h"
#include "settings.h"

typedef map < string, string > modDescMap;
typedef map < string, string > bookAbrevMap;

/******************************************************************************
 * Sword globals 
 */
SWDisplay *FPNDisplay; /* to display formatted personal notes */

SWMgr * percomMgr,  /* sword mgr for personal comments editor */
    *mainMgr;		/* sword mgr for curMod */

VerseKey swKey = "Romans 8:28",	/* temp storage for verse keys */
 vkText, vkComm;
SWModule *curMod,		/* module for main text window */
*percomMod;			/* module for personal commentary  window */

modDescMap descriptionMap;
bookAbrevMap abrevationMap;

/******************************************************************************
 * globals
 */


GtkWidget *NEtext,		/* note edit widget */
*MainFrm;			/* main form widget  */

gboolean noteModified = false,	/* set to true is personal note has changed */
 usepersonalcomments = false,	/* do we setup for personal comments - default is false  */
 autoSave = true,		/* we want to auto save changes to personal comments */
 havebible = false,		/* let us know if we have at least one bibletext module */
 havedict = false,		/* let us know if we have at least one lex-dict module */
 havecomm = false;		/* let us know if we have at least one commentary module */

gchar com_key[80] = "Rom 8:28",	/* current commentary key */
*textmod, *commod, *dictmod;

gint 
 dictpages,		/* number of dictionaries */
 compages,			/* number of commentaries */
 textpages,			/* number of Bible text */
 bookpages;

extern GtkWidget *htmlComments;
extern gchar current_verse[80];	

/******************************************************************************
 * Name
 *   backend_first_init
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_first_init(void)	
 *
 * Description
 *   create swmgrs 
 *
 * Return value
 *   void
 */
void backend_first_init(void)
{
	mainMgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
	percomMgr = new SWMgr();
	/*
	   char locale_name[40];
	   sprintf(locale_name,"%s",(char *)LocaleMgr::systemLocaleMgr.getDefaultLocaleName());
	   cout << locale_name << "\n";
	   if(strlen(locale_name) > 2) {
	   char buf[40];
	   strncpy(buf,locale_name,2);
	   buf[2] = '\0';
	   LocaleMgr::systemLocaleMgr.setDefaultLocaleName(buf);
	   }
	 */
	curMod = NULL;		//-- set mods to null
	percomMod = NULL;

	FPNDisplay = 0;
}

/******************************************************************************
 * Name
 *  backend_init_sword 
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_init_sword(SETTINGS * s)	
 *
 * Description
 *   setup the Sword stuff
 *
 * Return value
 *   void
 */
void backend_init_sword(SETTINGS * s)
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
	vkText = settings.currentverse;
	vkComm = settings.currentverse;


	s->displaySearchResults = false;
	s->havethayer = false;
	s->havebdb = false;

	MainFrm = s->app;	//-- save mainform for use latter
//	NEtext = lookup_widget(s->app, "textComments");	//-- get note edit widget

	//-- setup displays for sword modules
	FPNDisplay = new GtkHTMLEntryDisp(htmlComments, s);
	compages = 0;
	dictpages = 0;
	textpages = 0;
	bookpages = 0;


	if (s->showsplash) {
		while (gtk_events_pending())
			gtk_main_iteration();
	}

	g_print("Sword locale is %s\n",
		LocaleMgr::systemLocaleMgr.getDefaultLocaleName());

	g_print("%s\n", "Loading SWORD Modules");

	for (it = mainMgr->Modules.begin();
	     it != mainMgr->Modules.end(); it++) {
		descriptionMap[string
			       ((char *) (*it).second->Description())] =
		    string((char *) (*it).second->Name());		

		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
			curMod = (*it).second;
			havebible = TRUE;
			++textpages;
		}

		else if (!strcmp((*it).second->Type(), COMM_MODS)) {
			havecomm = TRUE;	//-- we have at least one commentay module
			++compages;	//-- how many pages do we have 
		}

		else if (!strcmp
			 ((*it).second->Type(), DICT_MODS)) {
			havedict = TRUE;	//-- we have at least one lex / dict module
			++dictpages;	//-- how many pages do we have

		}

		else if (!strcmp((*it).second->Type(), BOOK_MODS)) {
			++bookpages;
		}
	}

	g_print("\nNumber of Text modules = %d\n", textpages);
	g_print("Number of Commentary modules = %d\n", compages);
	g_print("Number of Dict/Lex modules = %d\n", dictpages);
	g_print("Number of Book modules = %d\n\n", bookpages);

	//-- setup Commentary Support, Generic Book Support and Dict/Lex Support
	backend_setup_text(s);
	backend_setup_commentary(s);
	backend_setupDL(s);
	backend_setupGBS(s);
	backend_setup_interlinear(s);
	backend_setup_percomm(s);
	/*
	//-- set up percom editor module
	for (it = percomMgr->Modules.begin();
	     it != percomMgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Commentaries")) {
			//-- if driver is RawFiles                     
			if ((*percomMgr->config->
			     Sections[(*it).second->Name()].
			     find("ModDrv")).second == "RawFiles") {
				percomMod = (*it).second;
				percomMod->Disp(FPNDisplay);
				usepersonalcomments = TRUE;	
				percomMod->SetKey(s->currentverse);
			}
		}
	}*/
}

GList * backend_get_global_options_list(void)
{
	GList * tmp = NULL;
	//-- get list of  globalOptions for menus
	OptionsList optionslist = mainMgr->getGlobalOptions();
	for (OptionsList::iterator it = optionslist.begin();
	     it != optionslist.end(); it++) {
		//-- save options in a glist for popup menus
		tmp =
		    g_list_append(tmp, (gchar *) (*it).c_str());
	}
	return tmp;
}
/******************************************************************************
 * Name
 *   backend_module_name_from_description
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_module_name_from_description(gchar * mod_name, gchar * mod_desc)	
 *
 * Description
 *   get mod name from mod description
 *
 * Return value
 *   void
 */
void backend_module_name_from_description(gchar * mod_name,
					  gchar * mod_desc)
{
	strcpy(mod_name, descriptionMap[mod_desc].c_str());
}

/******************************************************************************
 * Name
 *   backend_shutdown
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_shutdown(SETTINGS * s)	
 *
 * Description
 *   close down sword 
 *
 * Return value
 *   void
 */
void backend_shutdown(SETTINGS * s)
{
	savebookmarks(s->ctree_widget);
	backend_save_properties(s, true);

	backend_shut_down_verselist();
	backend_shutdown_text();
	backend_shutdownGBS();
	backend_shutdownDL();
	backend_shutdown_commentary();
	backend_shutdown_search_results_display();
	backend_shutdown_sb_viewer();
	backend_shutdown_interlinear();
	backend_shutdown_percomm();
	
	//-- delete Sword managers
	delete mainMgr;
	delete percomMgr;

	//-- delete Sword displays
	if (FPNDisplay)
		delete FPNDisplay;
	g_print("\nwe are done with Sword\n");
}



char *backend_get_valid_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return g_strdup((char *) vkey.getText());
}

char *backend_get_book_from_key(char *key)
{
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return (char *) vkey.books[vkey.Testament() - 1][vkey.Book() -
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

/******************************************************************************
 * 
 * option - option to set
 * yesno - yes or no
******************************************************************************/
void backend_set_global_option(gint window, gchar * option,
			       gchar * yesno)
{
	/* turn option on or off */
	switch (window) {
	case MAIN_TEXT_WINDOW:
			      /*** Bible text window ***/
		mainMgr->setGlobalOption(option, yesno);
		break;
	case INTERLINEAR_WINDOW:
				/*** interlinear window ***/
		backend_set_interlinear_global_option(option, yesno);
		break;
	}
}

/**********************************************************************
 * Name
 *   backend_get_books
 *
 * Synopsis
 *   #include "sw_sword.h"
 *   
 *   GList *backend_get_books(void)
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
const char *backend_get_sword_version(void)
{
	SWVersion retval;
	retval = SWVersion::currentVersion;
	return retval;
}

/*** the changes are already made we just need to show them ***/
void backend_display_new_font_color_and_size(SETTINGS * s)
{
	curMod->Display();
	display_commentary(s->currentverse);
	backend_displayinDL(s->DictWindowModule, s->dictkey);
	update_interlinear_page(&settings);
}

/*** most of this code is from an example in swmgr.h sword-1.5.2 ***/
void backend_save_module_key(char *mod_name, char *key)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	DIR *dir;
	gchar buf[256], conffile[256];
	struct dirent *ent;

	sprintf(buf, "%s", mainMgr->configPath);
	dir = opendir(buf);
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
				    myConfig->Sections.find(mod_name);
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
}

/*** display daily devotional ***/
void backend_display_devotional(SETTINGS * s)
{
	gchar buf[80];
	time_t curtime;
	struct tm *loctime;

	/* 
	 * Get the current time. 
	 */
	curtime = time(NULL);

	/* 
	 * Convert it to local time representation. 
	 */
	loctime = localtime(&curtime);

	/* 
	 * Print it out in a nice format. 
	 */
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

		if (!strcmp((*it).second->Type(), DICT_MODS)) {
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
			sit =
			    mainMgr->config->Sections.find((*it).
							   second->
							   Name());
			ConfigEntMap & section = (*sit).second;
			feature = ((entry = section.find("Feature")) !=
				   section.end())? (*entry).
			    second : (string) "";
			if (!stricmp(feature.c_str(), "DailyDevotion")) {
				mods =
				    g_list_append(mods,
						  (*it).second->Name());
			}
		}
	}
	return mods;
}

GList *backend_get_list_of_percom_modules(void)
{
	ModMap::iterator it;
	GList *mods = NULL;
	for (it = mainMgr->Modules.begin();
	     it != mainMgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), COMM_MODS)) {
			//-- if driver is RawFiles                     
			if ((*percomMgr->config->
			     Sections[(*it).second->Name()].
			     find("ModDrv")).second == "RawFiles") {
				mods =
				    g_list_append(mods,
						  (*it).second->Name());
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

int backend_get_module_page(char *module_name, char *module_type)
{
	ModMap::iterator it;
	gint module_index = 0;

	for (it = mainMgr->Modules.begin();
	     it != mainMgr->Modules.end(); it++) {

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
	SWModule *mod = mainMgr->Modules[mod_name];
	char *buf = (gchar *) mod->getConfigEntry("Font");
	return buf;
}

gboolean backend_module_is_locked(char *mod_name)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	DIR *dir;
	gchar buf[256], conffile[256];
	struct dirent *ent;
	bool retval = false;

	sprintf(buf, "%s", mainMgr->configPath);
	dir = opendir(buf);
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
				    myConfig->Sections.find(mod_name);
				if (section != myConfig->Sections.end()) {
					entry =
					    section->second.
					    find("CipherKey");
					if (entry !=
					    section->second.end()) {
						if (strlen
						    (entry->second.
						     c_str()) ==
						    CIPHER_KEY_LEN)
							retval = false;
						else
							retval = true;
						delete myConfig;
						closedir(dir);
						return retval;
					}
				}
				delete myConfig;
			}
		}
	}
	closedir(dir);
	return false;
}

char *backend_get_cipher_key(char *mod_name)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;
	DIR *dir;
	gchar buf[256], conffile[256];
	struct dirent *ent;
	char *retval = NULL;

	sprintf(buf, "%s", mainMgr->configPath);
	dir = opendir(buf);
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
				    myConfig->Sections.find(mod_name);
				if (section != myConfig->Sections.end()) {
					entry =
					    section->second.
					    find("CipherKey");
					if (entry !=
					    section->second.end()) {
						if (strlen
						    (entry->second.
						     c_str()) ==
						    CIPHER_KEY_LEN)
							retval = (char*)entry->second.
						     c_str();
						else
							retval = NULL;
						delete myConfig;
						closedir(dir);
						return retval;
					}
				}
				delete myConfig;
			}
		}
	}
	closedir(dir);
	return NULL;	
}
