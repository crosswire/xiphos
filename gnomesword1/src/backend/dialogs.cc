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

#include "backend/dialogs.hh"
#include "backend/gs_markupfiltmgr.h"

#include "main/settings.h"
#include "main/sword.h"

using namespace sword;
using namespace std;



ModuleDialogs::ModuleDialogs() {
	mgr = new SWMgr(new GSMarkupFilterMgr(FMT_HTMLHREF));
	mod = NULL;	
	tree_key = NULL;	
	entryDisplay      = 0;	// set in create
	chapDisplay    = 0;	// set in create
}


ModuleDialogs::~ModuleDialogs() {
	delete mgr;
	
	if (entryDisplay)
		delete entryDisplay;
	if (chapDisplay)
		delete chapDisplay;
}


void ModuleDialogs::init_SWORD() {
	//main_setup_new_displays();
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


GList *ModuleDialogs::fill_Bible_books(int testament) {
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


void ModuleDialogs::get_module_options(GList * options) {
	StringList optionslist = mgr->getGlobalOptions();	
	for (StringList::iterator it = optionslist.begin(); 
				  it != optionslist.end(); it++) {
		options = g_list_append(options, strdup((char *) (*it).c_str()));
	}
}


int ModuleDialogs::is_Bible_key(const char * list, char * current_key) {
	VerseKey key;
	
	key.setText(current_key);
	ListKey verses = key.ParseVerseList(list, key);
	return verses.Count();
}


char *ModuleDialogs::get_valid_key(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return strdup((char *) vkey.getText());
}

char *ModuleDialogs::key_get_book(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return (char*) vkey.books[vkey.Testament() - 1][vkey.Book() -1].name;
}


int ModuleDialogs::key_get_chapter(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	return vkey.Chapter();
}


const unsigned int ModuleDialogs::key_chapter_count(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	
	char testament = vkey.Testament() ;
	char book = vkey.Book();
	return (vkey.books[testament-1][book-1].chapmax);
}


const unsigned int ModuleDialogs::key_verse_count(const char *key) {
	VerseKey vkey;
	vkey.AutoNormalize(1);
	vkey = key;
	
	char testament = vkey.Testament() ;
	char book = vkey.Book();
	int chapter = vkey.Chapter();	
	return (vkey.books[testament-1][book-1].versemax[chapter-1]);
}


char *ModuleDialogs::get_module_key() {
	(const char *) *mod;
	return strdup(mod->KeyText());
}


void ModuleDialogs::save_entry(const char * entry) {
	mod->setEntry((const char *) entry);
}

void ModuleDialogs::delete_entry(void) {
	mod->deleteEntry();
}

int ModuleDialogs::is_module(const char *mod_name) {
	SWMgr *tmp_mgr = new SWMgr();
	ModMap::iterator it = tmp_mgr->Modules.find(mod_name);
	if (it != tmp_mgr->Modules.end()) {
		delete tmp_mgr;
		return 1;
	}
	delete tmp_mgr;
	return 0;
}


int ModuleDialogs::module_type(void) {
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


int ModuleDialogs::module_has_testament(int testament) {
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


char *ModuleDialogs::get_entry_attribute(const char *level1, const char *level2, const char *level3) {
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


int ModuleDialogs::set_module(const char *module_name) {
	ModMap::iterator it = mgr->Modules.find(module_name);
	if (it != mgr->Modules.end()) {
		mod = (*it).second;
		if (mod) 
			return 1;
	}
	return 0;	
}

int ModuleDialogs::set_module_key(const char *module_name, const char *key) {
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


int ModuleDialogs::set_key(const char *key) {
	if (mod) {
		mod->setKey(key);
		return 1;
	}
	else 
		return 0;
	
}
char *ModuleDialogs::get_key_form_offset(unsigned long offset) {
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


void ModuleDialogs::set_treekey(unsigned long offset) {
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

unsigned long ModuleDialogs::get_treekey_offset(void) {
        if (tree_key) 
                return tree_key->getOffset();
        return 0;
}


int ModuleDialogs::treekey_has_children(unsigned long offset) {	
        if (tree_key) {
                tree_key->setOffset(offset);
		return tree_key->hasChildren();
        }
        return false;
}


int ModuleDialogs::treekey_first_child(unsigned long offset) {
        if (tree_key) {
                tree_key->setOffset(offset);
		return tree_key->firstChild();
	}
        return false;
}


char *ModuleDialogs::treekey_get_local_name(unsigned long offset) {	
        if (tree_key) {
                tree_key->setOffset(offset);
                //-- returned value must be freed by calling function
                return strdup((char *) tree_key->getLocalName());
        }
        return NULL;
}


int ModuleDialogs::treekey_next_sibling(unsigned long offset) {
        if (tree_key) {
                tree_key->setOffset(offset);
                if(tree_key->nextSibling()) {
			return 1;
		}	
        }
        return 0;
}


char *ModuleDialogs::navigate_module(int direction) {
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


void ModuleDialogs::setup_displays(void) {
	
}

/* end of file */
