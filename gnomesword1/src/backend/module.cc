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

#include "main/search.h"
#include "main/sword.h"

using std::string;
using std::map;
using std::list;
using namespace sword;

typedef map < SWBuf, SWBuf > ModLanguageMap;
ModLanguageMap languageMap;

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

const char *backend_get_module_version(char *module_name)
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


/******************************************************************************
 * Name
 *   backend_get_preverse_header
 *
 * Synopsis
 *   #include ""
 *
 *   char *backend_get_preverse_header(char * module_name, char * key, int pvHeading)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *backend_get_preverse_header(char * module_name, char * key, int pvHeading)
{
	UTF8HTML u2html;
	SWBuf newtext;
	char buf[12];
	char *retval = NULL;	
	SWModule *module = sw.text_mgr->Modules[module_name];
	
	sprintf(buf, "%i", pvHeading);  
	module->SetKey(key);	
	module->RenderText();                 	
	SWBuf preverseHeading = module->getEntryAttributes()
					   ["Heading"]["Preverse"][buf].c_str();
	u2html.processText(preverseHeading);
	if (preverseHeading.length()) {  
		retval = strdup(preverseHeading.c_str());
	}  
	return retval;	
}


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

const char *backend_get_footnote_type(char *module_name,
				char *key, char *note)
{
	SWModule *module = sw.text_mgr->Modules[module_name];
	
	module->SetKey(key);
	module->RenderText();
	
	return module->getEntryAttributes()["Footnote"][note]["type"].c_str();
}

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

char *backend_get_crossref(char *module_name, char *key, char *note)
{
	SWModule *module = sw.text_mgr->Modules[module_name];
	module->Error();
	module->SetKey(key);
	module->RenderText();
	
	return strdup(module->getEntryAttributes()["Footnote"][note]["refList"].c_str());
}


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

char *backend_get_footnote_body(char *module_name,
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
/*		SWBuf refList = module->getEntryAttributes()["Footnote"][note]["refList"].c_str();
		//tmpVerseList = DefaultVSKey->ParseVerseList(refList.c_str(), *DefaultVSKey);
		if (tmpVerseList.Count())
			printf("%s\n", refList.c_str());
			return strdup(refList.c_str());*/
		module->stripFilter(body, keybuf);
		
	} else {
		
		module->renderFilter(body, keybuf);

	}		
	if (body)
		return strdup(body.c_str());
	
	return NULL;
}


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

int backend_get_display_level(char *module_name)
{
	char *buf = NULL;
	buf = (char *) sw.main_mgr->Modules[module_name]->
	    getConfigEntry("DisplayLevel");
	if (buf != NULL)
		return atoi(buf);
	else
		return 0;
}

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

int backend_is_module_rtl(char *mod_name)
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

int backend_do_module_search(char *module_name,
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
					&search_percent_update,
					(void *) &progressunits);
	search_scope_list = results;
	return results.Count();
}


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

char *backend_get_search_results_text(char *mod_name, char *key)
{
	SWModule *mod = sw.results->Modules[mod_name];

	if (mod) {
		mod->SetKey(key);
		return strdup((char *) mod->RenderText());
	}
	return NULL;
}



/******************************************************************************
 * Name
 *   backend_set_module_iterators
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   void backend_set_module_iterators(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void backend_set_module_iterators(void)
{
	begin = sw.module_mgr->Modules.begin();
	end = sw.module_mgr->Modules.end();
}

/******************************************************************************
 * Name
 *   backend_get_next_module_name
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   NAME_TYPE *backend_get_next_module_name(void)
 *
 * Description
 *
 *
 * Return value
 *   NAME_TYPE
 */

NAME_TYPE *backend_get_next_module_name(NAME_TYPE * nt)
{
	if (begin != end) {

		/*descriptionMap[string
		   ((char *) (*begin).second->
		   Description())] =
		   string((char *) (*begin).second->Name()); */

		if (!strcmp((*begin).second->Type(), TEXT_MODS)) {
			nt->type = TEXT_TYPE;
			nt->name =
			    strdup((char *) (*begin).second->Name());
		}
		if (!strcmp((*begin).second->Type(), COMM_MODS)) {
			nt->type = COMMENTARY_TYPE;
			nt->name =
			    strdup((char *) (*begin).second->Name());
		}
		if (!strcmp((*begin).second->Type(), DICT_MODS)) {
			nt->type = DICTIONARY_TYPE;
			nt->name =
			    strdup((char *) (*begin).second->Name());
		}
		if (!strcmp((*begin).second->Type(), BOOK_MODS)) {
			nt->type = BOOK_TYPE;
			nt->name =
			    strdup((char *) (*begin).second->Name());
		}
		begin++;
		return nt;
	} else
		return NULL;
}


/******************************************************************************
 * Name
 *   backend_get_next_module_description
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   	NAME_TYPE *backend_get_next_module_description(void)
 *
 * Description
 *   
 *
 * Return value
 *   NAME_TYPE
 */

NAME_TYPE *backend_get_next_module_description(NAME_TYPE * nt)
{
	if (begin != end) {

		if (!strcmp((*begin).second->Type(), TEXT_MODS)) {
			nt->type = TEXT_TYPE;
			nt->name =
			    strdup((char *) (*begin).second->
				   Description());
		}
		if (!strcmp((*begin).second->Type(), COMM_MODS)) {
			nt->type = COMMENTARY_TYPE;
			nt->name =
			    strdup((char *) (*begin).second->
				   Description());
		}
		if (!strcmp((*begin).second->Type(), DICT_MODS)) {
			nt->type = DICTIONARY_TYPE;
			nt->name =
			    strdup((char *) (*begin).second->
				   Description());
		}
		if (!strcmp((*begin).second->Type(), BOOK_MODS)) {
			nt->type = BOOK_TYPE;
			nt->name =
			    strdup((char *) (*begin).second->
				   Description());
		}
		begin++;
		return nt;
	} else
		return NULL;
}


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

int backend_is_personal_comment_module(char *module_name)
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


/**********************************************************************
 * Name
 *   backend_get_next_percom_name
 *
 * Synopsis
 *   #include "backend/module.hh"
 *   
 *   char *backend_get_next_percom_name(void)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *backend_get_next_percom_name(void)
{
	char *retval = NULL;

	if (begin != end) {
		if (!strcmp((*begin).second->getConfigEntry("ModDrv")
			    , "RawFiles")) {
			retval =
			    strdup((char *) (*begin).second->Name());
			begin++;
			return retval;
		} else {
			begin++;
			return "+";
		}
	} else
		return NULL;
}


/**********************************************************************
 * Name
 *   backend_get_next_devotion_name
 *
 * Synopsis
 *   #include "backend/module.hh"
 *   
 *   char *backend_get_next_devotion_name(void)
 *
 * Description
 *   
 *
 * Return value
 *   char*
 */

char *backend_get_next_devotion_name(void)
{
	char *retval = NULL;
	char *feature = NULL;

	if (begin != end) {
		feature =
		    (char *) (*begin).second->getConfigEntry("Feature");
		if (!feature) {
			begin++;
			return "+";
		}
		if (!strcmp(feature, "DailyDevotion")) {
			retval =
			    strdup((char *) (*begin).second->Name());
			begin++;
			return retval;
		} else {
			begin++;
			return "+";
		}
	} else
		return NULL;
}


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

char *backend_get_module_description(char *module_name)
{
	ModMap::iterator it;	//-- iteratior

	it = sw.main_mgr->Modules.find(module_name);
	if (it != sw.main_mgr->Modules.end()) {
		return (*it).second->Description();
	}
	return NULL;
}


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

char *backend_get_cipher_key(char *mod_name)
{
	return strdup((char *) sw.main_mgr->Modules[mod_name]->
		      getConfigEntry("CipherKey"));
}


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

int backend_has_cipher_tag(char *mod_name)
{
	char *tmpbuf = (char *) sw.main_mgr->Modules[mod_name]->
	    getConfigEntry("CipherKey");
	if (tmpbuf != NULL)
		return 1;
	else
		return 0;
}


/******************************************************************************
 * Name
 *   backend_check_for_module
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   int backend_check_for_module(char * mod_name)
 *
 * Description
 *    check for presents of a module by name
 *
 * Return value
 *   int
 */

int backend_check_for_module(const char *mod_name)
{
	ModMap::iterator it;
	
	it = sw.main_mgr->Modules.find(mod_name);
	if (it != sw.main_mgr->Modules.end()) {
		return 1;
	}
	return 0;
}


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

char *backend_get_mod_about_info(char *modname)
{
	return g_strdup((char *) sw.main_mgr->Modules[modname]->
			getConfigEntry("About"));
}


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

int backend_get_module_page(const char *module_name,
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


/******************************************************************************
 * Name
 *   backend_get_mod_type
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   int backend_get_mod_type(char * mod_name)
 *
 * Description
 *   we come here to get module type - 
 *   Bible text, Commentary, Dict/Lex or Book 
 *
 * Return value
 *   int
 */

int backend_get_mod_type(char *mod_name)
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

/******************************************************************************
 * Name
 *   backend_get_module_language
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_module_language(const char * mod_name)
 *
 * Description
 *   we come here to get module language - 
 *   Bible text, Commentary, Dict/Lex or Book 
 *
 * Return value
 *   char
 */

const char *backend_get_module_language(const char *module_name)
{
	ModMap::iterator it;	//-- iteratior
	//-- iterate through the modules until we find modName  
	it = sw.main_mgr->Modules.find(module_name);
	//-- if we find the module
	if (it != sw.main_mgr->Modules.end())
		return languageMap[(*it).second->Lang()].c_str();
	return "unknown";
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   const char *backend_get_language_map(const char *language)
 *
 * Description
 *   we come here to get language map - 
 *   Bible text, Commentary, Dict/Lex or Book 
 *
 * Return value
 *   char
 */

const char *backend_get_language_map(const char *language)
{
	return languageMap[language].c_str();
}

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

char *backend_get_module_text(int manager, char *module_name, char *key)
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
		return strdup((char *) mod->RenderText());
		
	}
	return NULL;
}




/******************************************************************************
 * Name
 *   backend_get_striptext
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_striptext(char *module_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */

char *backend_get_striptext(int manager, char *module_name, char *key)
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



/******************************************************************************
 * Name
 *  backend_check_for_global_option
 *
 * Synopsis
 *   #include "backend/module.hh"
 *   
 *   int backend_check_for_global_option(int mod_num, char *option)	
 *
 * Description
 *   returns true is module has option
 *
 * Return value
 *   int
 */

int backend_check_for_global_option(char *mod_name, char *option)
{
	SWModule *mod = sw.main_mgr->Modules[mod_name];
	if (mod)
		return mod->getConfig().has("GlobalOptionFilter",
					    option);
	else
		return 0;
}

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

char *backend_nav_module(int manager, char *module_name, int direction)
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

char *backend_get_chap_heading(int manager, char *module_name,
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

char *backend_get_book_heading(int manager, char *module_name,
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

void backend_set_commentary_key(char *mod_name, char *key)
{
	SWModule *mod = sw.comm_mgr->Modules[mod_name];
	if (mod) {
		versekey = key;
		mod->SetKey(versekey);
	}
}



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

char *backend_get_commentary_key(char *mod_name)
{
	SWModule *mod = sw.comm_mgr->Modules[mod_name];
	if (mod) {
		char *key = (char *) mod->KeyText();
		if (key)
			return strdup(key);
	}
	return NULL;
}

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

char *backend_get_commentary_text(char *mod_name, char *key)
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

int backend_set_module(int manager, char *module_name)
{
	switch (manager) {
	case TEXT_MGR:
		sw.text_mod = sw.text_mgr->Modules[module_name];
		if (sw.text_mod)
			return 1;
		break;
	case COMM_MGR:
		sw.comm_mod = sw.comm_mgr->Modules[module_name];
		if (sw.comm_mod)
			return 1;
		break;
	case DICT_MGR:
		sw.dict_mod = sw.dict_mgr->Modules[module_name];
		if (sw.dict_mod)
			return 1;
		break;
	case PERCOM_MGR:
		sw.percom_mod = sw.percom_mgr->Modules[module_name];
		if (sw.percom_mod)
			return 1;
		break;
	case GBS_MGR:
		sw.gbs_mod = sw.gbs_mgr->Modules[module_name];
		if (sw.gbs_mod)
			return 1;
		break;
	}
	return 0;
}


/******************************************************************************
 * Name
 *   backend_set_percomm_key
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   void backend_set_percomm_key(char * key)	
 *
 * Description
 *   set commentary key
 *
 * Return value
 *   void
 */

void backend_set_percomm_key(char *key)
{
	if (sw.percom_mod) {
		sw.percom_mod->SetKey(key);
	}
}


/******************************************************************************
 * Name
 *  backend_save_personal_comment
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   void backend_save_personal_comment(int mod_num, char * buf)
 *
 * Description
 *   save buf to the current personal commentary at current key 
 *
 * Return value
 *   void
 */

void backend_save_personal_comment(char *note)
{


	if (note) {
		sw.percom_mod->setEntry((const char *) note);
/*		g_warning("mod = %s\nkey = %s\nnote = %s",sw.percom_mod->Name(),
					sw.percom_mod->KeyText(),note);
*/
	}
}

/******************************************************************************
 * Name
 *  backend_delete_personal_comment
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   void backend_delete_personal_comment(void)
 *
 * Description
 *    delete the data at the currnet key in current personal commentary
 *
 * Return value
 *   void
 */

void backend_delete_personal_comment(void)
{
	sw.percom_mod->deleteEntry();
	g_print("\nkey %s of module %s has been deleted\n",
		sw.percom_mod->KeyText(), sw.percom_mod->Name());
}

/******************************************************************************
 * Name
 *   backend_get_percomm_text
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   char *backend_get_percomm_text(char *key)	
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *backend_get_percomm_text(char *key)
{
	if (sw.percom_mod) {
		sw.percom_mod->SetKey(key);
		return strdup((char *) sw.percom_mod->getRawEntry());
	}
	return NULL;
}


/******************************************************************************
 * Name
 *   backend_init_language_map
 *
 * Synopsis
 *   #include "backend/module.hh"
 *
 *   void backend_init_language_map(void)	
 *
 * Description
 *   maps language abbreviation to language name
 *
 * Return value
 *   void
 */

void backend_init_language_map(void)
{
	/* --list form Bibletime-1.3-- */
	//languageMap[SWBuf("aa")] = SWBuf("Afar");
	//languageMap[SWBuf("ab")] = SWBuf("Abkhazian");
	//languageMap[SWBuf("ae")] = SWBuf("Avestan");
	languageMap[SWBuf("af")] = SWBuf(_("Afrikaans"));
	//languageMap[SWBuf("am")] = SWBuf("Amharic");
	languageMap[SWBuf("ang")] = SWBuf(_("English, Old (ca.450-1100)"));
	languageMap[SWBuf("ar")] = SWBuf(_("Arabic"));
	//languageMap[SWBuf("as")] = SWBuf("Assamese");
	//languageMap[SWBuf("ay")] = SWBuf("Aymara");
	languageMap[SWBuf("az")] = SWBuf(_("Azerbaijani"));
	//languageMap[SWBuf("ba")] = SWBuf("Bashkir");
	languageMap[SWBuf("be")] = SWBuf(_("Belarusian"));
	languageMap[SWBuf("bg")] = SWBuf(_("Bulgarian"));
	//languageMap[SWBuf("bh")] = SWBuf("Bihari");
	//languageMap[SWBuf("bi")] = SWBuf("Bislama");
	//languageMap[SWBuf("bn")] = SWBuf("Bengali");
	//languageMap[SWBuf("bo")] = SWBuf("Tibetan");
	languageMap[SWBuf("br")] = SWBuf(_("Breton"));
	languageMap[SWBuf("bs")] = SWBuf(_("Bosnian"));
	languageMap[SWBuf("ca")] = SWBuf(_("Catalan"));
	//languageMap[SWBuf("ce")] = SWBuf("Chechen");
	languageMap[SWBuf("ceb")] = SWBuf(_("Cebuano"));
	//languageMap[SWBuf("ch")] = SWBuf("Chamorro");
	//languageMap[SWBuf("co")] = SWBuf("Corsican");
	languageMap[SWBuf("cop")] = SWBuf(_("Coptic"));
	languageMap[SWBuf("cs")] = SWBuf(_("Czech"));
	languageMap[SWBuf("cu")] = SWBuf(_("Church Slavic"));
	//languageMap[SWBuf("cv")] = SWBuf("Chuvash");
	languageMap[SWBuf("cy")] = SWBuf(_("Welsh"));
	languageMap[SWBuf("da")] = SWBuf(_("Danish"));
	languageMap[SWBuf("de")] = SWBuf(_("German"));
	//languageMap[SWBuf("dz")] = SWBuf("Dzongkha");
	languageMap[SWBuf("el")] = SWBuf(_("Greek, Modern (1453-)"));
	languageMap[SWBuf("en")] = SWBuf(_("English"));
	languageMap[SWBuf("en_US")] = SWBuf(_("American English"));
	languageMap[SWBuf("enm")] =
	    SWBuf(_("English, Middle (1100-1500)"));
	languageMap[SWBuf("eo")] = SWBuf(_("Esperanto"));
	languageMap[SWBuf("es")] = SWBuf(_("Spanish"));
	languageMap[SWBuf("et")] = SWBuf(_("Estonian"));
	languageMap[SWBuf("eu")] = SWBuf(_("Basque"));
	//languageMap[SWBuf("fa")] = SWBuf("Persian");
	languageMap[SWBuf("fi")] = SWBuf(_("Finnish"));
	//languageMap[SWBuf("fj")] = SWBuf("Fijian");
	//languageMap[SWBuf("fo")] = SWBuf("Faroese");
	languageMap[SWBuf("fr")] = SWBuf(_("French"));
	languageMap[SWBuf("fy")] = SWBuf(_("Frisian"));
	languageMap[SWBuf("ga")] = SWBuf(_("Irish"));
	languageMap[SWBuf("gd")] = SWBuf(_("Gaelic (Scots)"));
	//languageMap[SWBuf("gl")] = SWBuf("Gallegan");
	//languageMap[SWBuf("gn")] = SWBuf("Guarani");
	//languageMap[SWBuf("gn")] = SWBuf("Gujarati");
	languageMap[SWBuf("got")] = SWBuf(_("Gothic"));
	languageMap[SWBuf("gv")] = SWBuf(_("Manx"));
	languageMap[SWBuf("grc")] = SWBuf(_("Greek, Ancient (to 1453)"));
	languageMap[SWBuf("he")] = SWBuf(_("Hebrew"));
	languageMap[SWBuf("haw")] = SWBuf(_("Hawaiian"));
	//languageMap[SWBuf("hi")] = SWBuf("Hindi");
	//languageMap[SWBuf("ho")] = SWBuf("Hiri Motu");
	//languageMap[SWBuf("hr")] = SWBuf("Croatian");
	languageMap[SWBuf("hu")] = SWBuf(_("Hungarian"));
	languageMap[SWBuf("hy")] = SWBuf(_("Armenian"));
	//languageMap[SWBuf("hz")] = SWBuf("Herero");
	//languageMap[SWBuf("ia")] = SWBuf("Interlingua");
	languageMap[SWBuf("id")] = SWBuf(_("Indonesian"));
	//languageMap[SWBuf("ie")] = SWBuf("Interlingue");
	//languageMap[SWBuf("ik")] = SWBuf("Inupiaq");
	languageMap[SWBuf("is")] = SWBuf(_("Icelandic"));
	languageMap[SWBuf("it")] = SWBuf(_("Italian"));
	//languageMap[SWBuf("iu")] = SWBuf("Inuktitut");
	languageMap[SWBuf("ja")] = SWBuf(_("Japanese"));
	languageMap[SWBuf("ka")] = SWBuf(_("Georgian"));
	//languageMap[SWBuf("ki")] = SWBuf("Kikuyu");
	//languageMap[SWBuf("kj")] = SWBuf("Kuanyama");
	//languageMap[SWBuf("kk")] = SWBuf("Kazakh");
	//languageMap[SWBuf("kl")] = SWBuf("Kalaallisut");
	//languageMap[SWBuf("km")] = SWBuf("Khmer");
	//languageMap[SWBuf("kn")] = SWBuf("Kannada");
	languageMap[SWBuf("ko")] = SWBuf(_("Korean"));
	//languageMap[SWBuf("ks")] = SWBuf("Kashmiri");
	languageMap[SWBuf("ku")] = SWBuf(_("Kurdish"));
	//languageMap[SWBuf("kv")] = SWBuf("Komi");
	//languageMap[SWBuf("kw")] = SWBuf("Cornish");
	languageMap[SWBuf("ky")] = SWBuf(_("Kirghiz"));
	languageMap[SWBuf("la")] = SWBuf(_("Latin"));
	//languageMap[SWBuf("lb")] = SWBuf("Letzeburgesch");
	//languageMap[SWBuf("ln")] = SWBuf("Lingala");
	//languageMap[SWBuf("lo")] = SWBuf("Lao");
	//languageMap[SWBuf("lt")] = SWBuf("Lithuanian");
	languageMap[SWBuf("lv")] = SWBuf(_("Latvian"));
	//languageMap[SWBuf("mg")] = SWBuf("Malagasy");
	//languageMap[SWBuf("mh")] = SWBuf("Marshall");
	languageMap[SWBuf("mi")] = SWBuf(_("Maori"));
	languageMap[SWBuf("mk")] = SWBuf(_("Macedonian"));
	//languageMap[SWBuf("ml")] = SWBuf("Malayalam");
	//languageMap[SWBuf("mn")] = SWBuf("Mongolian");
	//languageMap[SWBuf("mo")] = SWBuf("Moldavian");
	//languageMap[SWBuf("mr")] = SWBuf("Marathi");
	languageMap[SWBuf("ms")] = SWBuf(_("Malay"));
	languageMap[SWBuf("mt")] = SWBuf(_("Maltese"));
	//languageMap[SWBuf("my")] = SWBuf("Burmese");
	//languageMap[SWBuf("na")] = SWBuf("Nauru");
	//languageMap[SWBuf("nb")] = SWBuf("Norwegian Bokm");
	//languageMap[SWBuf("nd")] = SWBuf("Ndebele, North");
	languageMap[SWBuf("nds")] = SWBuf(_("Low German; Low Saxon"));
	//languageMap[SWBuf("ne")] = SWBuf("Nepali");
	//languageMap[SWBuf("ng")] = SWBuf("Ndonga");
	languageMap[SWBuf("nl")] = SWBuf(_("Dutch"));
	//languageMap[SWBuf("nn")] = SWBuf("Norwegian Nynorsk");
	languageMap[SWBuf("no")] = SWBuf(_("Norwegian"));
	//languageMap[SWBuf("nr")] = SWBuf("Ndebele, South");
	//languageMap[SWBuf("nv")] = SWBuf("Navajo");
	//languageMap[SWBuf("ny")] = SWBuf("Chichewa; Nyanja");
	//languageMap[SWBuf("oc")] = SWBuf("Occitan (post 1500); Proven");
	//languageMap[SWBuf("om")] = SWBuf("Oromo");
	//languageMap[SWBuf("or")] = SWBuf("Oriya");
	//languageMap[SWBuf("os")] = SWBuf("Ossetian; Ossetic");
	//languageMap[SWBuf("pa")] = SWBuf("Panjabi");
	languageMap[SWBuf("pap")] = SWBuf(_("Papiamento"));
	//languageMap[SWBuf("pi")] = SWBuf("Pali");
	languageMap[SWBuf("pl")] = SWBuf(_("Polish"));
	//languageMap[SWBuf("ps")] = SWBuf("Pushto");
	languageMap[SWBuf("pt")] = SWBuf(_("Portuguese"));
	//languageMap[SWBuf("qu")] = SWBuf("Quechua");
	//languageMap[SWBuf("rm")] = SWBuf("Raeto-Romance");
	//languageMap[SWBuf("rn")] = SWBuf("Rundi");
	languageMap[SWBuf("ro")] = SWBuf(_("Romanian"));
	languageMap[SWBuf("ru")] = SWBuf(_("Russian"));
	//languageMap[SWBuf("rw")] = SWBuf("Kinyarwanda");
	//languageMap[SWBuf("sa")] = SWBuf("Sanskrit");
	//languageMap[SWBuf("sc")] = SWBuf("Sardinian");
	languageMap[SWBuf("sco")] = SWBuf(_("Scots"));
	//languageMap[SWBuf("sd")] = SWBuf("Sindhi");
	//languageMap[SWBuf("se")] = SWBuf("Northern Sami");
	//languageMap[SWBuf("sg")] = SWBuf("Sango");
	//languageMap[SWBuf("si")] = SWBuf("Sinhalese");
	languageMap[SWBuf("sk")] = SWBuf(_("Slovak"));
	languageMap[SWBuf("sl")] = SWBuf(_("Slovenian"));
	//languageMap[SWBuf("sm")] = SWBuf("Samoan");
	//languageMap[SWBuf("sn")] = SWBuf("Shona");
	languageMap[SWBuf("so")] = SWBuf(_("Somali"));
	languageMap[SWBuf("sq")] = SWBuf(_("Albanian"));
	//languageMap[SWBuf("sr")] = SWBuf("Serbian");
	//languageMap[SWBuf("ss")] = SWBuf("Swati");
	//languageMap[SWBuf("st")] = SWBuf("Sotho, Southern");
	//languageMap[SWBuf("su")] = SWBuf("Sundanese");
	languageMap[SWBuf("sv")] = SWBuf(_("Swedish"));
	languageMap[SWBuf("sw")] = SWBuf(_("Swahili"));
	languageMap[SWBuf("syr")] = SWBuf(_("Syriac"));
	languageMap[SWBuf("ta")] = SWBuf(_("Tamil"));
	//languageMap[SWBuf("te")] = SWBuf("Telugu");
	//languageMap[SWBuf("tg")] = SWBuf("Tajik");
	languageMap[SWBuf("th")] = SWBuf(_("Thai"));
	//languageMap[SWBuf("tk")] = SWBuf("Turkmen");
	languageMap[SWBuf("tl")] = SWBuf(_("Tagalog"));
	languageMap[SWBuf("tn")] = SWBuf(_("Tswana"));
	languageMap[SWBuf("tr")] = SWBuf(_("Turkish"));
	//languageMap[SWBuf("ts")] = SWBuf("Tsonga");
	//languageMap[SWBuf("tt")] = SWBuf("Tatar");
	//languageMap[SWBuf("tw")] = SWBuf("Twi");
	languageMap[SWBuf("ty")] = SWBuf(_("Tahitian"));
	//languageMap[SWBuf("ug")] = SWBuf("Uighur");
	languageMap[SWBuf("uk")] = SWBuf(_("Ukrainian"));
	//languageMap[SWBuf("ur")] = SWBuf("Urdu");
	//languageMap[SWBuf("uz")] = SWBuf("Uzbek");
	languageMap[SWBuf("vi")] = SWBuf(_("Vietnamese"));
	//languageMap[SWBuf("vo")] = SWBuf("Volapük");
	//languageMap[SWBuf("wo")] = SWBuf("Wolof");
	languageMap[SWBuf("xh")] = SWBuf(_("Xhosa"));
	languageMap[SWBuf("x-E-BAR")] = SWBuf(_("Bavarian"));
	languageMap[SWBuf("x-E-GSW")] = SWBuf(_("Alemannisch"));
	languageMap[SWBuf("x-E-HAT")] = SWBuf(_("Haitian Creole French"));
	languageMap[SWBuf("x-E-ITZ")] = SWBuf(_("Itz"));
	languageMap[SWBuf("x-E-JIV")] = SWBuf(_("Shuar"));
	languageMap[SWBuf("x-E-KEK")] = SWBuf(_("Kekchí"));
	languageMap[SWBuf("x-E-LMO")] = SWBuf(_("Lombard"));
	languageMap[SWBuf("x-E-MKJ")] = SWBuf(_("Macedonian"));
	languageMap[SWBuf("x-E-PDG")] = SWBuf(_("Tok Pisin"));
	languageMap[SWBuf("x-E-PPK")] = SWBuf(_("Uma"));
	languageMap[SWBuf("x-E-RMY")] = SWBuf(_("Romani, Vlax"));
	languageMap[SWBuf("x-E-SAJ")] = SWBuf(_("Sango"));
	languageMap[SWBuf("x-E-SRN")] = SWBuf(_("Sranan"));
	//languageMap[SWBuf("yi")] = SWBuf("Yiddish");
	//languageMap[SWBuf("za")] = SWBuf("Zhuang");
	languageMap[SWBuf("zh")] = SWBuf(_("Chinese"));
	languageMap[SWBuf("zu")] = SWBuf(_("Zulu"));
}
