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

#include <gnome.h>
#include <versekey.h>
#include <regex.h>
#include <gbfplain.h>
#include <thmlplain.h>
#include <string.h>
#include <utf8html.h>

#include "backend/module_search.hh"
#include "backend/gs_markupfiltmgr.h"

#include "main/search_sidebar.h"
#include "main/search_dialog.h"
#include "main/settings.h"
#include "main/sword.h"

using namespace sword;
using namespace std;



ModuleSearch::ModuleSearch() {
	mgr = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	mod = NULL;	
	tree_key = NULL;	
	entryDisplay      = 0;	// set in create
	chapDisplay    = 0;	// set in create
}


ModuleSearch::~ModuleSearch() {
	delete mgr;
	
	if (entryDisplay)
		delete entryDisplay;
	if (chapDisplay)
		delete chapDisplay;
}


void ModuleSearch::init_SWORD() {
	ModMap::iterator it;
	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {	
		mod = (*it).second;
		if (!strcmp(mod->Type(), TEXT_MODS)) {
			mod->Disp(chapDisplay);
		} else {
			mod->Disp(entryDisplay);
		}
	}
}

char *ModuleSearch::get_config_entry(char * entry) {
	return g_strdup((char *) mod->getConfigEntry(entry));
}


GList *ModuleSearch::fill_Bible_books(int testament) {
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


void ModuleSearch::get_module_options(GList * options) {
	StringList optionslist = mgr->getGlobalOptions();	
	for (StringList::iterator it = optionslist.begin(); 
				  it != optionslist.end(); it++) {
		options = g_list_append(options, strdup((char *) (*it).c_str()));
	}
}


int ModuleSearch::is_Bible_key(const char * list, char * current_key) {
	VerseKey key;
	
	key.setText(current_key);
	ListKey verses = key.ParseVerseList(list, key);
	return verses.Count();
}


char *ModuleSearch::get_valid_key(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return strdup((char *) vkey.getText());
}

char *ModuleSearch::key_get_book(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return (char*) vkey.books[vkey.Testament() - 1][vkey.Book() -1].name;
}


int ModuleSearch::key_get_chapter(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.Chapter();
}


const unsigned int ModuleSearch::key_chapter_count(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	
	char testament = vkey.Testament() ;
	char book = vkey.Book();
	return (vkey.books[testament-1][book-1].chapmax);
}


const unsigned int ModuleSearch::key_verse_count(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	
	char testament = vkey.Testament() ;
	char book = vkey.Book();
	int chapter = vkey.Chapter();	
	return (vkey.books[testament-1][book-1].versemax[chapter-1]);
}


char *ModuleSearch::get_module_key() {
	(const char *) *mod;
	return strdup(mod->KeyText());
}


void ModuleSearch::save_entry(const char * entry) {
	mod->setEntry((const char *) entry);
}

void ModuleSearch::delete_entry(void) {
	mod->deleteEntry();
}

int ModuleSearch::is_module(const char *mod_name) {
	SWMgr *tmp_mgr = new SWMgr();
	ModMap::iterator it = tmp_mgr->Modules.find(mod_name);
	if (it != tmp_mgr->Modules.end()) {
		delete tmp_mgr;
		return 1;
	}
	delete tmp_mgr;
	return 0;
}


int ModuleSearch::module_type(void) {
	if (!strcmp(mod->Type(), TEXT_MODS)) {
		return TEXT_TYPE;
	} else if (!strcmp(mod->Type(), COMM_MODS)) {
		if (!strcmp((char *) mod->getConfigEntry("ModDrv"), "RawFiles")) 
			return PERCOM_TYPE;		
		return COMMENTARY_TYPE;
	} else if (!strcmp(mod->Type(), DICT_MODS)) {
		return DICTIONARY_TYPE;
	} else if (!strcmp(mod->Type(), BOOK_MODS)) {
		return BOOK_TYPE;
	} else
		return -1;
}

char *ModuleSearch::module_description(char *mod_name) {
	ModMap::iterator it;	//-- iteratior
	if((!mod_name) || (strlen(mod_name) < 2)) 
		return NULL;
	//-- iterate through the modules until we find modName 
	it = mgr->Modules.find(mod_name);
	//-- if we find the module
	if (it != mgr->Modules.end()) {
		return (*it).second->Description();
	}
	return NULL;
}

int ModuleSearch::module_has_testament(int testament) {
	int ot = 0;
	int nt = 0;
	
	mod->setSkipConsecutiveLinks(true);
		*mod = sword::TOP; //position to first entry
		sword::VerseKey key( mod->KeyText() );
		if (key.Testament() == 1) { // OT && NT
			ot = 1;
		} else if (key.Testament() == 2) { //no OT
			ot = 0;
		}

		*mod = sword::BOTTOM;
		key = mod->KeyText();
		if (key.Testament() == 1) { // only OT, no NT
			nt = 0;
		} else if (key.Testament() == 2) { //has NT
			nt = 1;
		}
	mod->setSkipConsecutiveLinks(false);
	
	switch (testament) {
		case 1:
			return ot>0;
		case 2:
			return nt>0;

		default:
			return false;
	}
}


char *ModuleSearch::get_entry_attribute(const char *level1, const char *level2, const char *level3) {
	UTF8HTML u2html;	
	mod->RenderText();                 	
	SWBuf preverseHeading = mod->getEntryAttributes()
					   [level1][level2][level3].c_str();
	u2html.processText(preverseHeading);
	if (preverseHeading.length()) {  
		return strdup(preverseHeading.c_str());
	}  
	return NULL;	
}


int ModuleSearch::set_module(const char *module_name) {
	ModMap::iterator it = mgr->Modules.find(module_name);
	if (it != mgr->Modules.end()) {
		mod = (*it).second;
		if (mod) 
			return 1;
	}
	return 0;	
}

int ModuleSearch::set_module_key(const char *module_name, const char *key) {
	ModMap::iterator it = mgr->Modules.find(module_name);
	if (it != mgr->Modules.end()) {	
		mod = (*it).second;
		if (mod) {
			mod->setKey(key);
			return 1;
		}
	}
	return 0;	
}


int ModuleSearch::set_key(const char *key) {
	if (mod) {
		mod->setKey(key);
		return 1;
	}
	else 
		return 0;
	
}
char *ModuleSearch::get_key_form_offset(unsigned long offset) {
	char *retval = NULL;
	if (tree_key) {
                TreeKeyIdx treenode = *tree_key;
                treenode.setOffset(offset);
                /** if not root node then display **/
                if (treenode.getOffset() > 0) {
                        mod->SetKey(treenode);
                        mod->KeyText();      //snap to entry
                }
		retval = strdup(mod->KeyText());
        }
        return retval;
}


void ModuleSearch::set_treekey(unsigned long offset) {
	if(tree_key)
		delete tree_key;
	tree_key = (TreeKeyIdx *) mod->CreateKey();
	if (tree_key) {
                TreeKeyIdx treenode = *tree_key;
                treenode.setOffset(offset);
                /** if not root node then display **/
                if (treenode.getOffset() > 0) {
                        mod->SetKey(treenode);
                        mod->KeyText();      //snap to entry
                }
        }
}

unsigned long ModuleSearch::get_treekey_offset(void) {
        if (tree_key) 
                return tree_key->getOffset();
        return 0;
}


int ModuleSearch::treekey_has_children(unsigned long offset) {	
        if (tree_key) {
                tree_key->setOffset(offset);
		return tree_key->hasChildren();
        }
        return false;
}


int ModuleSearch::treekey_first_child(unsigned long offset) {
        if (tree_key) {
                tree_key->setOffset(offset);
		return tree_key->firstChild();
	}
        return false;
}


char *ModuleSearch::treekey_get_local_name(unsigned long offset) {	
        if (tree_key) {
                tree_key->setOffset(offset);
                //-- returned value must be freed by calling function
                return strdup((char *) tree_key->getLocalName());
        }
        return NULL;
}


int ModuleSearch::treekey_next_sibling(unsigned long offset) {
        if (tree_key) {
                tree_key->setOffset(offset);
                if(tree_key->nextSibling()) {
			return 1;
		}	
        }
        return 0;
}


char *ModuleSearch::navigate_module(int direction) {
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


void ModuleSearch::setup_displays(void) {
	
}

void ModuleSearch::set_listkey_position(char pos) {
	results.setPosition((char)pos);
}

const char *ModuleSearch::get_next_listkey(void) {
	const char *retval = NULL;	
	while(!results.Error()) {
		retval = results.getText();
		results++;
		return retval;
	}
	return NULL;	
}

int ModuleSearch::clear_scope(void) {
	current_scope = 0;	
	return 1;
}

int ModuleSearch::clear_search_list(void) {
	search_scope_list.ClearList();
	return search_scope_list.Count ();
}

int ModuleSearch::set_range(char * list) {
	search_range = VerseKey().ParseVerseList(list, "", true);
	return search_range.Count ();
}

void ModuleSearch::set_scope2range(void) {
	current_scope = &search_range;
}

int ModuleSearch::set_scope2last_search(void) {
	current_scope = &search_scope_list;//-- move searchlist into current_scope
	return 1;
}

int ModuleSearch::do_module_search(char *module_name, const char *search_string, 
				int search_type, int search_params, int is_dialog) {
	char progressunits = 70;
	results.ClearList();
	search_module = NULL;

	search_module = mgr->Modules[module_name];
	if (!search_module)
		return -1;
	results = search_module->Search(search_string,
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


/* end of file */
