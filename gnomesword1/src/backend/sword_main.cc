/*
 * GnomeSword Bible Study Tool
 * sword_main.cc - 
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

#include <swmgr.h>
//#include <markupfiltmgr.h>

#include <gnome.h>
#include <versekey.h>
#include <regex.h>
#include <gbfplain.h>
#include <thmlplain.h>
#include <string.h>
#include <utf8html.h>

#include "backend/sword_main.hh"
#include "backend/gs_markupfiltmgr.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/search_sidebar.h"
#include "main/search_dialog.h"

using namespace sword;
using namespace std;

BackEnd *backend = NULL;



BackEnd::BackEnd() {
	main_mgr = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	display_mgr = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	display_mod = NULL;
	
	tree_key = NULL;
	
	commDisplay          = 0;
	dictDisplay          = 0;
	textDisplay          = 0;
	entryDisplay         = 0;
	chapDisplay          = 0;
	dialogRTOLDisplay    = 0;
	verselistDisplay     = 0;
	viewerDisplay        = 0;
}
 

BackEnd::~BackEnd() {
	delete main_mgr;
	delete display_mgr;
	
	if (commDisplay)
		delete commDisplay;
	if (dictDisplay)
		delete dictDisplay;
	if (textDisplay)
		delete textDisplay;	
	if(entryDisplay)
		delete entryDisplay;
	if(chapDisplay)
		delete chapDisplay;
	if(dialogRTOLDisplay)
		delete dialogRTOLDisplay;
	if(verselistDisplay)
		delete verselistDisplay;
	if(viewerDisplay)
		delete viewerDisplay;
}


void BackEnd::init_SWORD(int gsType) {
	ModMap::iterator it;
	if(gsType == 0) {
		main_setup_new_displays();
		
		for (it = display_mgr->Modules.begin();
					it != display_mgr->Modules.end(); it++) {
			display_mod = (*it).second;
			if (!strcmp(display_mod->Type(), TEXT_MODS)) {
				const char *direction = display_mod->getConfigEntry("Direction");
				if(direction && !strcmp(direction,"RtoL" ))
					display_mod->Disp(RTOLDisplay);
				else
					display_mod->Disp(textDisplay);
			}
			if (!strcmp(display_mod->Type(), COMM_MODS)) {
				display_mod->Disp(commDisplay);
			}
			if (!strcmp(display_mod->Type(), DICT_MODS)) {
				display_mod->Disp(dictDisplay);
			}
			if (!strcmp(display_mod->Type(), BOOK_MODS)) {
				display_mod->Disp(commDisplay);
			}			
		}
		init_language_map();
	} else if(gsType == 1) {
		for (it = display_mgr->Modules.begin(); it != display_mgr->Modules.end(); it++) {	
			display_mod = (*it).second;
			if (!strcmp(display_mod->Type(), TEXT_MODS)) {const char *direction = display_mod->getConfigEntry("Direction");
				if(direction && !strcmp(direction,"RtoL" ))
					display_mod->Disp(dialogRTOLDisplay);
				else
					display_mod->Disp(chapDisplay);
			} else {
				display_mod->Disp(entryDisplay);
			}
		}	
	}
}


void BackEnd::init_lists(MOD_LISTS * mods) {
	ModMap::iterator it;
	
	for (it = main_mgr->Modules.begin();
				it != main_mgr->Modules.end(); it++) {
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

GList *BackEnd::fill_Bible_books(int testament) {
	VerseKey key;
	GList *retlist = NULL;
	char *book = NULL;
	unsigned int bytes_read;
	unsigned int bytes_written;
	GError *error;
	int i = 0, j = 0, x = 2;
	
	switch(testament) {
		case 0: i = 0; x = 1;
			break;
		case 1:i = 1; x = 2;
			break;
		case 2: i = 0; x = 2;
			break;
	}
	
	while(i < x) {
		while(j < key.BMAX[i]) { 
			book = g_convert((const char *)key.books[i][j++].name,
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     &error);
			if(book == NULL) {
				g_print ("error: %s\n", error->message);
				g_error_free (error);
				continue;
			}
			retlist = g_list_append(retlist, (char *)book);
		}
		j = 0;
		i++;
	}
	return retlist;
}

GList *BackEnd::get_module_options(void) {
	GList *options = NULL;
	StringList optionslist = main_mgr->getGlobalOptions();	
	for (StringList::iterator it = optionslist.begin(); 
				  it != optionslist.end(); it++) {
		options = g_list_append(options, strdup((char *) (*it).c_str()));
	}
	return options;
}

int BackEnd::has_global_option(char * module_name, char * option) {
	SWModule *mod;
	ModMap::iterator it;	//-- iteratior
	//-- iterate through the modules until we find modName  
	it = main_mgr->Modules.find(module_name);
	//-- if we find the module
	if (it != main_mgr->Modules.end()) {
		mod = (*it).second;
		return mod->getConfig().has("GlobalOptionFilter", option);
	} else
		return 0;
}

char *BackEnd::get_config_entry(char * module_name, char * entry) {
	SWModule *mod;
	ModMap::iterator it;	//-- iteratior
	//-- iterate through the modules until we find modName  
	it = main_mgr->Modules.find(module_name);
	//-- if we find the module
	if (it != main_mgr->Modules.end()) {
		mod = (*it).second;
		return g_strdup((char *) mod->getConfigEntry(entry));
	} else
		return NULL;
}

void BackEnd::set_cipher_key(char * mod_name, char * key) {
	display_mgr->setCipherKey(mod_name, key);	
}

int BackEnd::is_Bible_key(const char * list, char * current_key) {
	VerseKey key;
	
	key.setText(current_key);
	ListKey vs = key.ParseVerseList(list, key);
	return vs.Count();
}


char *BackEnd::get_render_text(const char *module_name, const char *key) {
	SWModule *mod;
	ModMap::iterator it;	//-- iteratior
	//-- iterate through the modules until we find modName  
	it = display_mgr->Modules.find(module_name);
	//-- if we find the module
	if (it != display_mgr->Modules.end()) {
		mod = (*it).second;
		mod->setKey(key);
		return strdup((char *) mod->RenderText());
	}
	return NULL;	
}


char *BackEnd::get_strip_text(const char *module_name, const char *key) {
	SWModule *mod;
	ModMap::iterator it;	//-- iteratior
	//-- iterate through the modules until we find modName  
	it = main_mgr->Modules.find(module_name);
	//-- if we find the module
	if (it != main_mgr->Modules.end()) {
		mod = (*it).second;
		mod->setKey(key);
		return strdup((char *) mod->StripText());
	}
	return NULL;	
}


char *BackEnd::get_valid_key(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return strdup((char *) vkey.getText());
}

char *BackEnd::key_get_book(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return (char*) vkey.books[vkey.Testament() - 1][vkey.Book() -1].name;
}


int BackEnd::key_get_chapter(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.Chapter();
}

int BackEnd::key_get_verse(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.Verse();
}

const unsigned int BackEnd::key_chapter_count(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	
	char testament = vkey.Testament() ;
	char book = vkey.Book();
	return (vkey.books[testament-1][book-1].chapmax);
}


const unsigned int BackEnd::key_verse_count(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	
	char testament = vkey.Testament() ;
	char book = vkey.Book();
	int chapter = vkey.Chapter();	
	return (vkey.books[testament-1][book-1].versemax[chapter-1]);
}


char *BackEnd::get_module_key() {
	(const char *) *display_mod;
	return strdup(display_mod->KeyText());
}

void BackEnd::save_entry(const char * entry) {
	display_mod->setEntry((const char *) entry);
}

void BackEnd::delete_entry(void) {
	display_mod->deleteEntry();
}


const char *BackEnd::module_get_language(const char *module_name) {
	ModMap::iterator it;	//-- iteratior
	//-- iterate through the modules until we find modName  
	it = main_mgr->Modules.find(module_name);
	//-- if we find the module
	if (it != main_mgr->Modules.end())
		return languageMap[(*it).second->Lang()].c_str();
	return "unknown";
}


const char *BackEnd::get_language_map(const char *language) {
	return languageMap[language].c_str();
}


int BackEnd::is_module(const char *mod_name) {
	ModMap::iterator it = main_mgr->Modules.find(mod_name);
	if (it != main_mgr->Modules.end()) {
		return 1;
	}
	return 0;
}


int BackEnd::module_type(char *mod_name) {
	ModMap::iterator it;	//-- iteratior
	if((!mod_name) || (strlen(mod_name) < 2)) 
		return -1;
	//-- iterate through the modules until we find modName 
	it = main_mgr->Modules.find(mod_name);
	//-- if we find the module
	if (it != main_mgr->Modules.end()) {

		if (!strcmp((*it).second->Type(), TEXT_MODS)) {
			return TEXT_TYPE;
		}

		if (!strcmp((*it).second->Type(), COMM_MODS)) {
			if (!strcmp((char *) (*it).second->getConfigEntry("ModDrv"), "RawFiles")) 
				return PERCOM_TYPE;			
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

char *BackEnd::module_description(char *mod_name) {
	ModMap::iterator it;	//-- iteratior
	if((!mod_name) || (strlen(mod_name) < 2)) 
		return NULL;
	//-- iterate through the modules until we find modName 
	it = main_mgr->Modules.find(mod_name);
	//-- if we find the module
	if (it != main_mgr->Modules.end()) {
		return (*it).second->Description();
	}
	return NULL;
}

char *BackEnd::module_name_from_description(char *description) {
	ModMap::iterator it;
	char *retval = NULL;
	
	if(!description)
		return NULL;
	
	for (it = main_mgr->Modules.begin();
	     it != main_mgr->Modules.end(); it++) {
		  if (!strcmp((*it).second->Description(), description))
			retval = strdup((*it).second->Name());
	}
	return retval;
}



int BackEnd::module_has_testament(const char * module_name,  int testament) {
	ModMap::iterator it;
	SWModule *module;
	int ot = 0;
	int nt = 0;
	
	it = main_mgr->Modules.find(module_name);
	if (it != main_mgr->Modules.end()) {
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


char *BackEnd::get_entry_attribute(const char *level1, const char *level2, const char *level3) {
	UTF8HTML u2html;
	display_mod->RenderText();                 	
	SWBuf attribute2 = display_mod->getEntryAttributes()[level1][level2][level3].c_str();
	
	u2html.processText(attribute2);
	
	if (attribute2.length()) {  
		return strdup(attribute2.c_str());
	}  
	return NULL;	
}


int BackEnd::set_module(const char *module_name) {
	display_mod = display_mgr->Modules[module_name];
	if (display_mod) 
		return 1;
	else 
		return 0;
	
}

int BackEnd::set_module_key(const char *module_name, const char *key) {
	display_mod = display_mgr->Modules[module_name];
	if (display_mod) {
		display_mod->setKey(key);
		return 1;
	}
	else 
		return 0;
	
}

int BackEnd::set_key(const char *key) {
	if (display_mod) {
		display_mod->setKey(key);
		return 1;
	}
	else 
		return 0;
	
}

char *BackEnd::get_key_form_offset(unsigned long offset) {
	char *retval = NULL;
	if (tree_key) {
                TreeKeyIdx treenode = *tree_key;
                treenode.setOffset(offset);
                /** if not root node then display **/
                if (treenode.getOffset() > 0) {
                        display_mod->SetKey(treenode);
                        display_mod->KeyText();      //snap to entry
                }
		retval = strdup(display_mod->KeyText());
        }
        return retval;
}


void BackEnd::set_treekey(unsigned long offset) {
	if(tree_key)
		delete tree_key;
	tree_key = (TreeKeyIdx *) display_mod->CreateKey();
	if (tree_key) {
                TreeKeyIdx treenode = *tree_key;
                treenode.setOffset(offset);
                /** if not root node then display **/
                if (treenode.getOffset() > 0) {
                        display_mod->SetKey(treenode);
                        display_mod->KeyText();      //snap to entry
                }
        }
}

unsigned long BackEnd::get_treekey_offset(void) {
        if (tree_key) 
                return tree_key->getOffset();
        return 0;
}


int BackEnd::treekey_has_children(unsigned long offset) {	
        if (tree_key) {
                tree_key->setOffset(offset);
		return tree_key->hasChildren();
        }
        return false;
}


int BackEnd::treekey_first_child(unsigned long offset) {
        if (tree_key) {
                tree_key->setOffset(offset);
		return tree_key->firstChild();
	}
        return false;
}


char *BackEnd::treekey_get_local_name(unsigned long offset) {	
        if (tree_key) {
                tree_key->setOffset(offset);
                //-- returned value must be freed by calling function
                return strdup((char *) tree_key->getLocalName());
        }
        return NULL;
}


int BackEnd::treekey_next_sibling(unsigned long offset) {
        if (tree_key) {
                tree_key->setOffset(offset);
                if(tree_key->nextSibling()) {
			return 1;
		}	
        }
        return 0;
}


char *BackEnd::navigate_module(int direction) {
	if (direction == -1)
		return strdup((char *) display_mod->KeyText());

	switch (direction) {
	case 0:
		(*display_mod)--;
		break;
	case 1:
		(*display_mod)++;
		break;
	}
	display_mod->Error();
	return strdup((char *) display_mod->KeyText());
}


void BackEnd::setup_displays(void) {
	
}

GList *BackEnd::parse_verse_list(const char * list, char * current_key) {
	GList *retlist = NULL;
	VerseKey key;
	ListKey vs;                                                   
        gsize bytes_read;
        gsize bytes_written;
        GError **error;
	
	key.setText(current_key);
	vs = key.ParseVerseList(list, key);
	if(!vs.Count())
		return retlist;
	while(!vs.Error()) {
		char *mykey = g_convert((char*)vs.getText(),
                             -1,
                             OLD_CODESET,
                             UTF_8,
                             &bytes_read,
                             &bytes_written,
                             error);

		retlist = g_list_append(retlist, (char*)mykey);
		vs++;
	}
	return retlist;
}

GList *BackEnd::parse_range_list(const char * list) {
	GList *retlist = NULL;
	char *buf = NULL;
	VerseKey key;
	int count = 0;
	
	verses.ClearList();
	verses = key.ParseVerseList(list, key, true);
	
	while(!verses.Error()) {	
		VerseKey *element = SWDYNAMIC_CAST(VerseKey, 
					verses.GetElement(count));
		if (element) {
			buf = g_strdup_printf("%s - %s",(const char *)element->LowerBound(),
				(const char *)element->UpperBound());
			retlist = g_list_append(retlist,(char*)buf);
		}
		verses++;
		count++;
	}
	return retlist;
}

void BackEnd::set_listkey_position(char pos) {
	results.setPosition((char)pos);
}

const char *BackEnd::get_next_listkey(void) {
	const char *retval = NULL;	
	while(!results.Error()) {
		retval = results.getText();
		results++;
		return retval;
	}
	return NULL;	
}

int BackEnd::clear_scope(void) {
	current_scope = 0;	
	return 1;
}

int BackEnd::clear_search_list(void) {
	search_scope_list.ClearList();
	return search_scope_list.Count ();
}

int BackEnd::set_range(char * list) {
	search_range = VerseKey().ParseVerseList(list, "", true);
	return search_range.Count ();
}

void BackEnd::set_scope2range(void) {
	current_scope = &search_range;
}

int BackEnd::set_scope2last_search(void) {
	current_scope = &search_scope_list;//-- move searchlist into current_scope
	return 1;
}

int BackEnd::do_module_search(char *module_name, const char *search_string, 
				int search_type, int search_params, int is_dialog) {
	char progressunits = 70;
	results.ClearList();
	search_mod = NULL;

	search_mod = main_mgr->Modules[module_name];
	if (!search_mod)
		return -1;
	results = search_mod->Search(search_string,
					search_type,
					search_params,
					current_scope, 0,
					(is_dialog)
					?main_dialog_search_percent_update
					:main_sidebar_search_percent_update,
					(void *) &progressunits);
	search_scope_list = results;
	return results.Count();
}

void BackEnd::init_language_map(void) {
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


/* end of file */
