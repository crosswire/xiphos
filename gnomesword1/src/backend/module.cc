/*
 * GnomeSword Bible Study Tool
 * module.cc - SHORT DESCRIPTION
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
#ifdef USE_GNOME2
#include <gnome.h>
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif
#include <swmodule.h>
#include <flatapi.h>
#include <versekey.h>
#include <utf8html.h>


#include "backend/mgr.hh"
#include "backend/module.hh"
#include "backend/key.hh"
#include "backend/sword.h"
#include "backend/sword_defs.h"
#include "backend/sword_main.hh"

#include "main/display.hh"
#include "main/lists.h"
#include "main/search.h"
#include "main/sword.h"
//#include "main/mod_search.h"


#include "gui/widgets.h"

using std::string;
using std::map;
using std::list;
using namespace sword;

//typedef map < SWBuf, SWBuf > ModLanguageMap;
//ModLanguageMap languageMap;

#include "backend/mgr.hh"

#define CIPHER_KEY_LEN 16


/******************************************************************************
 * static  global to this file only
 */

static SWModule *search_module;
static ModMap::iterator begin;
static ModMap::iterator end;
static VerseKey versekey;

/******************************************************************************
 * externs
 */

extern ListKey results;
extern ListKey search_scope_list;
extern SWKey *current_scope;


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   c
 */

/*int backend_module_has_testament(const char * module_name,  int testament)
{
	ModMap::iterator it;
	SWModule *module;
	int ot = 0;
	int nt = 0;
	
	it = sw.main_mgr->Modules.find(module_name);
	if (it != sw.main_mgr->Modules.end()) {
		SWModule *module = (*it).second;
		module->setSkipConsecutiveLinks(true);
			*module = sword::TOP; //position to first entry
			sword::VerseKey key( module->KeyText() );
			if (key.Testament() == 1) { // OT && NT
				ot = 1;
			} else if (key.Testament() == 2) { //no OT
				ot = 0;
			}
	
			*module = sword::BOTTOM;
			key = module->KeyText();
			if (key.Testament() == 1) { // only OT, no NT
				nt = 0;
			} else if (key.Testament() == 2) { //has NT
				nt = 1;
			}
	         module->setSkipConsecutiveLinks(false);
	}
	switch (testament) {
		case 1:
			return ot>0;
		case 2:
			return nt>0;

		default:
			return false;
	}
}
*/
/******************************************************************************
 * Name
 *   backend_get_module_config_entry
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   const char *backend_get_module_config_entry(const char * module_name, 
 *					    const char * entry)
 *
 * Description
 *   
 *
 * Return value
 *   const char *
 */

/*const char *backend_get_module_config_entry(const char * module_name, 
					    const char * entry)
{
	const char *buf = NULL;
	ModMap::iterator it;
	
	it = sw.main_mgr->Modules.find(module_name);
	if (it != sw.main_mgr->Modules.end()) {
		if (!strcmp((*it).second->Name(), module_name)){
			buf = (*it).second->getConfigEntry(entry);
			if(buf)
				return buf;
		}
	}
	return NULL;	
}
*/
/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include ""
 *
 *   int (gchar *modname)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

/*const char *backend_get_module_version(char *module_name)
{
	const char *buf = NULL;
	ModMap::iterator it;
	
	it = sw.main_mgr->Modules.find(module_name);
	if (it != sw.main_mgr->Modules.end()) {
		if (!strcmp((*it).second->Name(), module_name)){
			buf = (*it).second->getConfigEntry("Version");
			if(buf)
				return buf;
		}
	}
	return " ";	
}
*/
/******************************************************************************
 * Name
 *   backend_get_footnote_type
 *
 * Synopsis
 *   #include ""
 *
 *   char *backend_get_footnote_type(char * module_name, 
 *                                       char * key, char * note)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

/*const char *backend_get_footnote_type(char *module_name,
				char *key, char *note)
{
	SWModule *module = sw.text_mgr->Modules[module_name];
	
	module->SetKey(key);
	module->RenderText();
	
	return module->getEntryAttributes()["Footnote"][note]["type"].c_str();
}
*/
/******************************************************************************
 * Name
 *   backend_get_footnote_type
 *
 * Synopsis
 *   #include ""
 *
 *   char *backend_get_footnote_type(char * module_name, 
 *                                       char * key, char * note)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

/*char *backend_get_crossref(char *module_name, char *key, char *note)
{
	SWModule *module = sw.text_mgr->Modules[module_name];
	module->Error();
	module->SetKey(key);
	module->RenderText();
	
	return strdup(module->getEntryAttributes()["Footnote"][note]["refList"].c_str());
}
*/

/******************************************************************************
 * Name
 *   backend_get_footnote_body
 *
 * Synopsis
 *   #include ""
 *
 *   char *backend_get_footnote_body(char * module_name, 
 *                                       char * key, char * note)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

/*char *backend_get_footnote_body(char *module_name,
				char *key, char *note)
{
	SWKey *keybuf;
	ListKey tmpVerseList;
	VerseKey *DefaultVSKey;
	SWModule *module = sw.text_mgr->Modules[module_name];
	
	DefaultVSKey = new VerseKey();
	DefaultVSKey->setText(key);
	module->SetKey(*DefaultVSKey);
	module->RenderText();
	keybuf = module->getKey();
	SWBuf type = 
		module->getEntryAttributes()["Footnote"][note]["type"].c_str();
	SWBuf body = module->getEntryAttributes()
					["Footnote"][note]["body"].c_str();	
	if (type == "crossReference") {
		SWBuf refList = module->getEntryAttributes()["Footnote"][note]["refList"].c_str();
		//tmpVerseList = DefaultVSKey->ParseVerseList(refList.c_str(), *DefaultVSKey);
		if (tmpVerseList.Count())
			printf("%s\n", refList.c_str());
			return strdup(refList.c_str());
		module->stripFilter(body, keybuf);
		
	} else {
		
		module->renderFilter(body, keybuf);

	}		
	if (body)
		return strdup(body.c_str());
	
	return NULL;
}
*/

/*
char *backend_get_footnote_body_new(char *module_name,
				char *key, char *note)
{
	ModMap::iterator it;
	VerseKey *DefaultVSKey;
	ListKey tmpVerseList;
	SWKey *keybuf;
	
	DefaultVSKey = new VerseKey();
	DefaultVSKey->setText(key);
	keybuf = DefaultVSKey;
	printf("%module_name = s\n", module_name);
	printf("key = %s\n", keybuf->getText());
	printf("note = %s\n", note);
	it = sw.text_mgr->Modules.find(module_name);
	if (it != sw.text_mgr->Modules.end()) {
		SWModule *module = it->second;
		module->setKey(*DefaultVSKey);
		int verseNum = DefaultVSKey->Verse();
		printf("verseNum = %i\n", verseNum);
		DefaultVSKey->Verse(verseNum);
		module->RenderText();	// force entry attributes to get set
		SWBuf type = module->getEntryAttributes()["Footnote"][note]["type"].c_str();
		printf("type = %s\n", type.c_str());
		SWBuf body = module->getEntryAttributes()["Footnote"][note]["body"].c_str();
	module->renderFilter(body, keybuf);
		printf("body = %s\n", body.c_str());
		if (type == "crossReference") {
			SWBuf refList = module->getEntryAttributes()["Footnote"][note]["refList"].c_str();
			tmpVerseList = DefaultVSKey->ParseVerseList(refList.c_str(), *DefaultVSKey);
			if (tmpVerseList.Count())
				printf("%s", refList.c_str());//AddVerseChoices(menu, refList.c_str(), rtf);
		}
	} 
}
*/

/******************************************************************************
 * Name
 *   backend_get_display_level
 *
 * Synopsis
 *   #include ""
 *
 *   int backend_get_display_level(gchar *modname)
 *
 * Description
 *   return the DisplayLevel information from the <module>.conf
 *
 * Return value
 *   int
 */

/*int backend_get_display_level(char *module_name)
{
	char *buf = NULL;
	buf = (char *) sw.main_mgr->Modules[module_name]->
	    getConfigEntry("DisplayLevel");
	if (buf != NULL)
		return atoi(buf);
	else
		return 0;
}
*/
/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

/*int backend_is_module_rtl(char *mod_name)
{
	char *direction = NULL;

	SWModule *module = sw.main_mgr->Modules[mod_name];
	if (module)
		direction =
		    (char *) module->getConfigEntry("Direction");
	if (direction)
		if (!strcmp(direction, "RtoL"))
			return 1;
	return 0;
}
*/


/******************************************************************************
 * Name
 *   backend_module_name_from_description
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   void backend_module_name_from_description(gchar * mod_name, gchar * mod_desc)	
 *
 * Description
 *   get mod name from mod description
 *
 * Return value
 *   char*
 */

char *backend_module_name_from_description(char *mod_desc)
{
	
	ModMap::iterator it;
	char *retval = NULL;
	
	if(!mod_desc)
		return NULL;
	backend_new_module_mgr();
	for (it = sw.module_mgr->Modules.begin();
	     it != sw.module_mgr->Modules.end(); it++) {
		  if (!strcmp((*it).second->Description(), mod_desc))
			retval = strdup((*it).second->Name());
	}
	backend_delete_module_mgr();
	return retval;
}


/******************************************************************************
 * Name
 *   backend_do_module_search
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   int backend_do_module_search(char *module_name, char *search_string,
			     int search_type, int search_params)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */

/*int backend_do_module_search(char *module_name,
			     const char *search_string, int search_type,
			     int search_params)
{
	char progressunits = 70;

	results.ClearList();
	search_module = NULL;

	search_module = sw.search_mgr->Modules[module_name];
	if (!search_module)
		return -1;
	results = search_module->Search(search_string,
					search_type,
					search_params,
					current_scope, 0,
					&main_search_percent_update,
					(void *) &progressunits);
	search_scope_list = results;
	return results.Count();
}
*/

/******************************************************************************
 * Name
 *   backend_get_search_results_text
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_search_results_text(char * mod_name, char * key)	
 *
 * Description
 *    returns the module text for key
 *
 * Return value
 *   char *
 */

/*char *backend_get_search_results_text(char *mod_name, char *key)
{
	SWModule *mod = sw.results->Modules[mod_name];

	if (mod) {
		mod->SetKey(key);
		return strdup((char *) mod->RenderText());
	}
	return NULL;
}
*/


/******************************************************************************
 * Name
 *   backend_get_module_lists
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   void backend_get_module_lists(MOD_LISTS * mods)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */


/*void backend_get_module_lists(MOD_LISTS * mods)
{
	backend->init_lists(mods);
		ModMap::iterator it;
	for (it = sw.module_mgr->Modules.begin(); it != 
				sw.module_mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
			mods->biblemods =
			    g_list_append(mods->biblemods,
				strdup((char *) (*it).second->Name()));
			mods->text_descriptions =
			    g_list_append(mods->text_descriptions,
				strdup((char *) (*it).second->
				   Description()));
		}
		if (!strcmp((*it).second->Type(), COMM_MODS)) {
			mods->commentarymods =
			    g_list_append(mods->commentarymods,
			    strdup((char *) (*it).second->Name()));
			mods->comm_descriptions =
			    g_list_append(mods->comm_descriptions,
				strdup((char *) (*it).second->
				   Description()));
			if (!strcmp((*it).second->getConfigEntry("ModDrv")
				    , "RawFiles")) {
				mods->percommods = g_list_append(mods->percommods,
				    strdup((char *) (*it).second->Name()));
			} 
		}
		if (!strcmp((*it).second->Type(), DICT_MODS)) {
			mods->dictionarymods =
			    g_list_append(mods->dictionarymods,
			    strdup((char *) (*it).second->Name()));
			mods->dict_descriptions =
			    g_list_append(mods->dict_descriptions,
			    strdup((char *) (*it).second->
				   Description()));
			
			char *feature =
			    (char *) (*it).second->getConfigEntry("Feature");
			if(!feature)
				continue;
			if (!strcmp(feature, "DailyDevotion"))
				mods->devotionmods 
				   = g_list_append(mods->devotionmods,
				    strdup((char *) (*it).second->Name()));
		}
		if (!strcmp((*it).second->Type(), BOOK_MODS)) {
			mods->bookmods =
			    g_list_append(mods->bookmods,
			    strdup((char *) (*it).second->Name()));
			mods->book_descriptions =
			    g_list_append(mods->book_descriptions,
			    strdup((char *) (*it).second->
				   Description()));
		}
		
	}	
	
}
*/
/******************************************************************************
 * Name
 *  backend_is_personal_comment_module
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   int backend_is_personal_comment_module(char * module_name)
 *
 * Description
 *    
 *
 * Return value
 *   int
 */

/*int backend_is_personal_comment_module(char *module_name)
{
	SWModule *module = sw.main_mgr->Modules[module_name];
	if (!module)
		return FALSE;
	if (!strcmp((char *) module->getConfigEntry("ModDrv"),
		    "RawFiles")) {
		return TRUE;
	}
	return FALSE;

}
*/

/******************************************************************************
 * Name
 *   backend_get_module_description
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_module_description(char * module_name)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

/*char *backend_get_module_description(char *module_name)
{
	ModMap::iterator it;	//-- iteratior

	it = sw.main_mgr->Modules.find(module_name);
	if (it != sw.main_mgr->Modules.end()) {
		return (*it).second->Description();
	}
	return NULL;
}
*/

/******************************************************************************
 * Name
 *   backend_module_is_locked
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   int backend_module_is_locked(char *mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */
/*
int backend_module_is_locked(char *mod_name)
{
	char *tmpbuf = (char *) sw.main_mgr->Modules[mod_name]->
	    getConfigEntry("CipherKey");
	if (tmpbuf != NULL) {
		if (strlen(tmpbuf)) {
			
			return false;
		} else
			return true;
	}

}
*/

/******************************************************************************
 * Name
 *   backend_get_cipher_key
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_cipher_key(char *mod_name)	
 *
 * Description
 *    returns the value of the "CipherKey"
 *
 * Return value
 *   char *
 */

/*char *backend_get_cipher_key(char *mod_name)
{
	return strdup((char *) sw.main_mgr->Modules[mod_name]->
		      getConfigEntry("CipherKey"));
}
*/

/******************************************************************************
 * Name
 *   backend_has_cipher_tag
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   int backend_has_cipher_tag(char *mod_name)	
 *
 * Description
 *   
 *
 * Return value
 *   int
 */
/*
int backend_has_cipher_tag(char *mod_name)
{
	char *tmpbuf = (char *) sw.main_mgr->Modules[mod_name]->
	    getConfigEntry("CipherKey");
	if (tmpbuf != NULL)
		return 1;
	else
		return 0;
}
*/

/******************************************************************************
 * Name
 *   backend_main_is_module
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   int backend_main_is_module(char * mod_name)
 *
 * Description
 *    check for presents of a module by name
 *
 * Return value
 *   int
 */
/*
int backend_main_is_module(const char *mod_name)
{
	ModMap::iterator it;
	
	it = sw.main_mgr->Modules.find(mod_name);
	if (it != sw.main_mgr->Modules.end()) {
		return 1;
	}
	return 0;
}
*/

/******************************************************************************
 * Name
 *   backend_get_mod_about_info
 *
 * Synopsis
 *   #include ""
 *
 *   gchar *backend_get_mod_about_info(gchar *modname)
 *
 * Description
 *   return the about information from the <module>.conf
 *
 * Return value
 *   gchar *
 */

/*char *backend_get_mod_about_info(char *modname)
{
	return g_strdup((char *) sw.main_mgr->Modules[modname]->
			getConfigEntry("About"));
}
*/

/******************************************************************************
 * Name
 *   backend_get_module_page
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   int backend_get_module_page(char *module_name, char *module_type)
 *
 * Description
 *
 *
 * Return value
 *   int
 */

/*int backend_get_module_page(const char *module_name,
			    const char *module_type)
{
	ModMap::iterator it;
	int module_index = 0;

	for (it = sw.main_mgr->Modules.begin();
	     it != sw.main_mgr->Modules.end(); it++) {
		if ((*it).second) {	// Sometimes Module is NULL - bug somewhere
			if (!strcmp((*it).second->Type(), module_type)) {
				if (!strcmp
				    ((*it).second->Name(),
				     module_name)) {
					return module_index;
				}
				++module_index;
			}
		}
	}
	return -1;
}
*/

/******************************************************************************
 * Name
 *   backend_main_get_mod_type
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   int backend_main_get_mod_type(char * mod_name)
 *
 * Description
 *   we come here to get module type - 
 *   Bible text, Commentary, Dict/Lex or Book 
 *
 * Return value
 *   int
 */
/*
int backend_main_get_mod_type(char *mod_name)
{

	ModMap::iterator it;	//-- iteratior
	if((!mod_name) || (strlen(mod_name) < 2)) 
		return -1;
	//-- iterate through the modules until we find modName 
	it = sw.main_mgr->Modules.find(mod_name);
	//-- if we find the module
	if (it != sw.main_mgr->Modules.end()) {

		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
			return TEXT_TYPE;
		}

		if (!strcmp((*it).second->Type(), COMM_MODS)) {
			return COMMENTARY_TYPE;
		}

		if (!strcmp((*it).second->Type(), DICT_MODS)) {
			return DICTIONARY_TYPE;
		}

		if (!strcmp((*it).second->Type(), BOOK_MODS)) {
			return BOOK_TYPE;
		}
	}
	return -1;
}
*/

/******************************************************************************
 * Name
 *   backend_get_module_text
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_module_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */

/*char *backend_get_module_text(int manager, char *module_name, char *key)
{
	SWModule *mod = NULL;
	bool if_true = false;
	
	switch (manager) {
	case TEXT_MGR:
		mod = sw.text_mgr->Modules[module_name];
		break;
	case COMM_MGR:
		mod = sw.comm_mgr->Modules[module_name];
		break;
	case DICT_MGR:
		mod = sw.dict_mgr->Modules[module_name];
		break;
	case GBS_MGR:
		mod = sw.gbs_mgr->Modules[module_name];
		break;
	case MAIN_MGR:
		mod = sw.main_mgr->Modules[module_name];
		break;
	case SEARCH_MGR:
		mod = sw.search_mgr->Modules[module_name];
		break;
	case INTER_MGR:
		mod = sw.inter_mgr->Modules[module_name];
		break;
	}
	if (mod) {
		mod->SetKey(key);
		// work-a-round for bug in thmlhtmlhref filter
		if(!strcmp(sw.version,"1.5.6")) {
			if ((!strcmp(mod->Name(), "AmTract")) ||
			    (!strcmp(mod->Name(), "Scofield"))) 
				return strdup((char *) mod->getRawEntry());
			else
				return strdup((char *) mod->RenderText());
		} 
		if(if_true)
			g_warning((char *) mod->getRawEntry());
		
			return strdup((char *) mod->RenderText());
		
	}
	return NULL;
}
*/


/******************************************************************************
 * Name
 *   backend_main_get_striptext
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_main_get_striptext(char *module_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */
/*
char *backend_main_get_striptext(int manager, char *module_name, char *key)
{
	SWModule *mod = NULL;
	switch (manager) {
	case TEXT_MGR:
		mod = sw.text_mgr->Modules[module_name];
		break;
	case COMM_MGR:
		mod = sw.comm_mgr->Modules[module_name];
		break;
	case MAIN_MGR:
		mod = sw.main_mgr->Modules[module_name];
		break;
	case SEARCH_MGR:
		mod = sw.search_mgr->Modules[module_name];
		break;
	}

	if (mod)
		mod->SetKey(key);
	else
		return NULL;
	return strdup((char *) mod->StripText());
}
*/


/******************************************************************************
 * Name
 *  backend_main_check_for_global_option
 *
 * Synopsis
 *   #include "backend/module.hh"
 *   
 *   int backend_main_check_for_global_option(int mod_num, char *option)	
 *
 * Description
 *   returns true is module has option
 *
 * Return value
 *   int
 */
/*
int backend_main_check_for_global_option(char *mod_name, char *option)
{
	SWModule *mod = sw.main_mgr->Modules[mod_name];
	if (mod)
		return mod->getConfig().has("GlobalOptionFilter",
					    option);
	else
		return 0;
}
*/
/******************************************************************************
 * Name
 *  backend_nav_commentary
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *    char* backend_nav(int manager, char * mod_name, gint direction)	
 *
 * Description
 *    navigate the current module and return key
 *    return value must be freed by calling function
 *
 * Return value
 *   char *
 */

/*char *backend_nav_module(int manager, char *module_name, int direction)
{
	SWModule *mod = NULL;
	switch (manager) {
	case TEXT_MGR:
		mod = sw.text_mgr->Modules[module_name];
		break;
	case COMM_MGR:
		mod = sw.comm_mgr->Modules[module_name];
		break;
	case DICT_MGR:
		mod = sw.dict_mgr->Modules[module_name];
		break;
	case MAIN_MGR:
		mod = sw.main_mgr->Modules[module_name];
		break;
	case SEARCH_MGR:
		mod = sw.search_mgr->Modules[module_name];
		break;
	}

	if (mod) {
		if (direction == -1)
			return strdup((char *) mod->KeyText());

		switch (direction) {
		case 0:
			(*mod)--;
			break;
		case 1:
			(*mod)++;
			break;
		}
		mod->Error();
		return strdup((char *) mod->KeyText());
	}
	return NULL;
}
*/

/******************************************************************************
 * Name
 *   backend_get_chap_heading
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_chap_heading(char * mod_name, char * key)
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */

/*char *backend_get_chap_heading(int manager, char *module_name,
			       char *key)
{
	char newkey[256];
	char *buf;
	SWModule *mod = NULL;
	
	switch (manager) {
	case TEXT_MGR:
		mod = sw.text_mgr->Modules[module_name];
		break;
	case COMM_MGR:
		mod = sw.comm_mgr->Modules[module_name];
		break;
	case MAIN_MGR:
		mod = sw.main_mgr->Modules[module_name];
		break;
	case SEARCH_MGR:
		mod = sw.search_mgr->Modules[module_name];
		break;
	}

	if (mod) {
		versekey = key;
		mod->SetKey(versekey);
		mod->SetKey(key);

		VerseKey vkey;
		vkey = key;
		const char *book =
		    vkey.books[vkey.Testament() - 1][vkey.Book() -
						     1].name;
		int chapter = vkey.Chapter();
		sprintf(newkey, "%s %d:0", book, chapter);

		versekey.AutoNormalize(0);
		versekey = newkey;
		mod->SetKey(versekey);
		mod->Error();
		buf = (char *) mod->RenderText();
		versekey.AutoNormalize(1);
		return strdup(buf);
	}
	return NULL;
}
*/

/******************************************************************************
 * Name
 *  backend_get_book_heading
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *    char *backend_get_book_heading(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */

/*char *backend_get_book_heading(int manager, char *module_name,
			       char *key)
{
	char newkey[256];
	char *buf;
	SWModule *mod = NULL;
	
	switch (manager) {
	case TEXT_MGR:
		mod = sw.text_mgr->Modules[module_name];
		break;
	case COMM_MGR:
		mod = sw.comm_mgr->Modules[module_name];
		break;
	case MAIN_MGR:
		mod = sw.main_mgr->Modules[module_name];
		break;
	case SEARCH_MGR:
		mod = sw.search_mgr->Modules[module_name];
		break;
	}

	if (mod) {
		versekey = key;
		mod->SetKey(versekey);
		VerseKey vkey;
		vkey = key;		
		SWBuf book =
		    vkey.books[vkey.Testament() - 1][vkey.Book() -
						     1].name;
		book += "0:0";
		versekey.AutoNormalize(0);
		versekey = book.c_str();
		mod->SetKey(versekey);
		mod->Error();
		buf = (char *) mod->RenderText();
		versekey.AutoNormalize(1);
		return strdup(buf);
	}
	return NULL;
}
*/

/******************************************************************************
 * Name
 *   backend_set_commentary_key
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   void backend_set_commentary_key(char * mod_name, char * key)	
 *
 * Description
 *   set commentary key
 *
 * Return value
 *   void
 */

/*void backend_set_commentary_key(char *mod_name, char *key)
{
	sw.comm_mod = sw.display_mgr->Modules[mod_name];
	if (sw.comm_mod) {
		versekey = key;
		sw.comm_mod->SetKey(versekey);
	}
}
*/


/******************************************************************************
 * Name
 *   backend_get_commentary_key
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_commentary_key(char *mod_name)	
 *
 * Description
 *   return commentary key
 *
 * Return value
 *   char *
 */

/*char *backend_get_commentary_key(char *mod_name)
{
	SWModule *mod = sw.display_mgr->Modules[mod_name];
	if (mod) {
		char *key = (char *) mod->KeyText();
		if (key)
			return strdup(key);
	}
	return NULL;
}
*/
/******************************************************************************
 * Name
 *   backend_get_commentary_text
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_commentary_text(char *mod_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */

/*char *backend_get_commentary_text(char *mod_name, char *key)
{
	SWModule *mod = sw.comm_mgr->Modules[mod_name];
	gsize bytes_read;
	gsize bytes_written;
	GError **error;	
	char *mykey = g_convert(key,
			     -1,
			     "iso8859-1",
			     "UTF-8",
			     &bytes_read,
			     &bytes_written,
			     error);
	if (mod) {
		versekey.Persist(1);
		versekey = mykey;
		mod->SetKey(versekey);
		mod->SetKey(mykey);
		g_free(mykey);
		// work-a-round for bug in thmlhtmlhref filter
		if(!strcmp(sw.version,"1.5.6")) {
			if ((!strcmp(mod->Name(), "AmTract"))
			    || (!strcmp(mod->Name(), "Scofield")))
				return strdup(mod->getRawEntry());
			else 
				return strdup(mod->RenderText());
			
		} 
		return strdup(mod->RenderText());
	}
	return NULL;
}
*/
/******************************************************************************
 * Name
 *  backend_get_key_from_module
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_key_from_module(int manager, char * module_name)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
/*
char *backend_get_key_from_module(int manager, char *module_name)
{
	SWModule *mod = NULL;
	switch (manager) {
	case TEXT_MGR:
		mod = sw.text_mgr->Modules[module_name];
		break;
	case COMM_MGR:
		mod = sw.comm_mgr->Modules[module_name];
		break;
	case DICT_MGR:
		mod = sw.dict_mgr->Modules[module_name];
		(const char *)mod;
		break;
	case GBS_MGR:
		mod = sw.gbs_mgr->Modules[module_name];
		break;
	case MAIN_MGR:
		mod = sw.main_mgr->Modules[module_name];
		break;
	case SEARCH_MGR:
		mod = sw.search_mgr->Modules[module_name];
		break;
	default:
		mod = sw.main_mgr->Modules[module_name];
	}
	if (mod) {
		(const char *) *mod;
//              printf("mod->KeyText = %s\n",mod->KeyText());
		return strdup(mod->KeyText());
	}
	return NULL;
}
*/
/*char *backend_get_book_key(void)
{
	sw.gbs_mod->KeyText();;
	return strdup(sw.gbs_mod->KeyText());
}
*/
/******************************************************************************
 * Name
 *  backend_set_module
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   int backend_set_module(int manager, char * module_name)	
 *
 * Description
 *    I don't think this is being used 
 *
 * Return value
 *   int
 */

/*int backend_set_module(int manager, char *module_name)
{
	switch (manager) {
	case TEXT_MGR:
		sw.text_mod = sw.text_mgr->Modules[module_name];
		if (sw.text_mod)
			return 1;
		break;
	case COMM_MGR:
		sw.comm_mod = sw.display_mgr->Modules[module_name];
		if (sw.comm_mod)
			return 1;
		break;
	case DICT_MGR:
		sw.dict_mod = sw.display_mgr->Modules[module_name];
		if (sw.dict_mod)
			return 1;
		break;
	case PERCOM_MGR:
		sw.percom_mod = sw.percom_mgr->Modules[module_name];
		if (sw.percom_mod)
			return 1;
		break;
	case GBS_MGR:
		sw.gbs_mod = sw.display_mgr->Modules[module_name];
		if (sw.gbs_mod)
			return 1;
		break;
	}
	return 0;
}
*/
