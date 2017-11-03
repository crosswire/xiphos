/*
 * Xiphos Bible Study Tool
 * sword_main.cc -
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <set>

#include <swmgr.h>
#include <swmodule.h>
#include <localemgr.h>
#include <swversion.h>
#include <filemgr.h>

#include <markupfiltmgr.h>
#include <swlocale.h>

#include <gtk/gtk.h>
#include <versekey.h>
#include <regex.h>
#include <string.h>
#include <utf8html.h>
#include <url.h>

#include "backend/sword_main.hh"

#include "main/mod_mgr.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/search_sidebar.h"
#include "main/search_dialog.h"
#include "gui/dialog.h"

#include "gui/debug_glib_null.h"

using namespace sword;
using namespace std;

BackEnd *backend = NULL;

// rule of thumb for VerseKey usage: you can use
//    VerseKey k = dynamic_cast<VerseKey *>(mod->getKey());
// if you intend to do nothing more than setText() once plus
// some gets (getTestament, getBook, getChapter, getVerse),
// with no need to garbage collect when you're done.
//
// otherwise, if you're going to do more than a single setSomething,
// you must go the somewhat harder route of
//    VerseKey *k = (VerseKey *)mod->createKey();
// and do whatever you like, but remember when you're done to
//    delete k;

#ifdef CHATTY
static const char *f_message = "backend/sword_main.cc line #%d \"%s\" = %s";
#endif

BackEnd::BackEnd()
{
#ifdef CHATTY
	GTimer *t;
	double d;
	t = g_timer_new();
#endif

	main_mgr = new SWMgr(new MarkupFilterMgr(FMT_XHTML));

#ifdef CHATTY
	g_timer_stop(t);
	d = g_timer_elapsed(t, NULL);
	XI_message(("create main_mgr time is %f", d));
#endif

	display_mod = NULL;
	tree_key = NULL;
	commDisplay = 0;
	bookDisplay = 0;
	dictDisplay = 0;
	textDisplay = 0;
	entryDisplay = 0;
	chapDisplay = 0;
	verselistDisplay = 0;
	viewerDisplay = 0;
}

BackEnd::~BackEnd()
{
	if (main_mgr)
		delete main_mgr;
	main_mgr = 0;
	if (commDisplay)
		delete commDisplay;
	if (bookDisplay)
		delete bookDisplay;
	if (dictDisplay)
		delete dictDisplay;
	if (textDisplay)
		delete textDisplay;
	if (entryDisplay)
		delete entryDisplay;
	if (chapDisplay)
		delete chapDisplay;
	if (verselistDisplay)
		delete verselistDisplay;
	if (viewerDisplay)
		delete viewerDisplay;

	commDisplay = 0;
	bookDisplay = 0;
	dictDisplay = 0;
	textDisplay = 0;
	entryDisplay = 0;
	chapDisplay = 0;
	verselistDisplay = 0;
	viewerDisplay = 0;
}

void BackEnd::init_SWORD(int gsType)
{
	ModMap::iterator it;
	if (gsType == 0) {
		main_setup_displays();
		for (it = main_mgr->Modules.begin();
		     it != main_mgr->Modules.end();
		     ++it) {
			display_mod = (*it).second;
			if (!strcmp(display_mod->getType(), TEXT_MODS)) {
				display_mod->setDisplay(textDisplay);
			}
			if (!strcmp(display_mod->getType(), COMM_MODS)) {
				display_mod->setDisplay(commDisplay);
			}
			if (!strcmp(display_mod->getType(), DICT_MODS)) {
				display_mod->setDisplay(dictDisplay);
			}
			if (!strcmp(display_mod->getType(), BOOK_MODS)) {
				display_mod->setDisplay(bookDisplay);
			}
		}
	} else if (gsType == 1) { // dialogs
		for (it = main_mgr->Modules.begin();
		     it != main_mgr->Modules.end();
		     ++it) {
			display_mod = (*it).second;
			if (!strcmp(display_mod->getType(), TEXT_MODS)) {
				display_mod->setDisplay(chapDisplay);
			} else {
				display_mod->setDisplay(entryDisplay);
			}
		}
	} else if (gsType == 2) { // search
		for (it = main_mgr->Modules.begin();
		     it != main_mgr->Modules.end();
		     ++it) {
			display_mod = (*it).second;
			display_mod->setDisplay(entryDisplay);
		}
	}
}

void BackEnd::init_lists(MOD_LISTS *mods)
{
	ModMap::iterator it;

	for (it = main_mgr->Modules.begin();
	     it != main_mgr->Modules.end();
	     ++it) {
		SWModule *m = it->second;
		const char *modtype = m->getType();
		const char *modname = m->getName();
		const char *abbreviation = m->getConfigEntry("Abbreviation");

		if (abbreviation) {
			main_add_abbreviation(modname, abbreviation);
		}

		if (!strcmp(modtype, TEXT_MODS)) {
			mods->biblemods =
			    g_list_append(mods->biblemods,
					  strdup((char *)modname));
			mods->text_descriptions =
			    g_list_append(mods->text_descriptions,
					  strdup((char *)m->getDescription()));
		}
		if (!strcmp(modtype, COMM_MODS)) {
			mods->commentarymods =
			    g_list_append(mods->commentarymods,
					  strdup((char *)modname));
			mods->comm_descriptions =
			    g_list_append(mods->comm_descriptions,
					  strdup((char *)m->getDescription()));
			if (!strcmp(m->getConfigEntry("ModDrv"), "RawFiles")) {
				mods->percommods = g_list_append(mods->percommods,
								 strdup((char *)modname));
			}
		}
		if (!strcmp(modtype, DICT_MODS)) {
			char *feature =
			    (char *)m->getConfigEntry("Feature");

			if (feature && !strcmp(feature, "DailyDevotion")) {
				mods->devotionmods =
				    g_list_append(mods->devotionmods,
						  strdup((char *)modname));
			} else {
				mods->dictionarymods =
				    g_list_append(mods->dictionarymods,
						  strdup((char *)modname));
				mods->dict_descriptions =
				    g_list_append(mods->dict_descriptions,
						  strdup((char *)m->getDescription()));
			}
		}
		if (!strcmp(modtype, BOOK_MODS)) {
			if (m->getConfigEntry("GSType") &&
			    !strcmp(m->getConfigEntry("GSType"), "PrayerList")) {
				mods->prayermods =
				    g_list_append(mods->prayermods,
						  strdup((char *)modname));
			} else {
				mods->bookmods =
				    g_list_append(mods->bookmods,
						  strdup((char *)modname));
				mods->book_descriptions =
				    g_list_append(mods->book_descriptions,
						  strdup((char *)m->getDescription()));
			}
		}

		char *category = (char *)m->getConfigEntry("Category");
		if (!category)
			continue;
		if (!strcmp(category, "Maps"))
			mods->mapmods =
			    g_list_append(mods->mapmods,
					  strdup((char *)modname));
		if (!strcmp(category, "Images"))
			mods->imagemods =
			    g_list_append(mods->imagemods,
					  strdup((char *)modname));
	}
}

const char *BackEnd::get_sword_version(void)
{
	SWVersion retval;
	retval = SWVersion::currentVersion;
	return retval;
}

// in the new-to-us av11n world, we need to add a module name
// parameter to many backend requests for VerseKey context.
// this replaces excess use of "main_mgr->Modules.find(name)".
SWModule *BackEnd::get_SWModule(const char *module_name)
{
	ModMap::iterator it = main_mgr->Modules.find(module_name);
	return ((it != main_mgr->Modules.end())
		    ? it->second
		    : NULL);
}

GList *BackEnd::get_module_options(void)
{
	GList *options = NULL;
	StringList optionslist = main_mgr->getGlobalOptions();
	for (StringList::iterator it = optionslist.begin();
	     it != optionslist.end(); ++it) {
		options = g_list_append(options, strdup((char *)(*it).c_str()));
	}
	return options;
}

int BackEnd::has_option(const char *module_name, const char *key, const char *option)
{
	SWModule *mod = get_SWModule(module_name);

	if (mod) {
		return mod->getConfig().has(key, option);
	} else
		return 0;
}

int BackEnd::has_global_option(const char *module_name, const char *option)
{
	return has_option(module_name, "GlobalOptionFilter", option);
}

char *BackEnd::get_config_entry(char *module_name, char *entry)
{
	if ((this == NULL) || (main_mgr == NULL))
		return NULL;

	SWModule *mod = get_SWModule(module_name);

	if (mod) {
		return g_strdup((char *)mod->getConfigEntry(entry));
	} else
		return NULL;
}

void BackEnd::set_cipher_key(char *module_name, char *key)
{
	main_mgr->setCipherKey(module_name, key);
}

int BackEnd::is_Bible_key(const char *module_name, const char *list, const char *current_key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return 0;

	VerseKey *key = dynamic_cast<VerseKey *>(mod->getKey());
	if (!key)
		return 0;

	key->setText(current_key);
	ListKey vs = key->parseVerseList(list, *key);
	return vs.getCount();
}

char *BackEnd::get_render_text(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);

	if (mod) {
		mod->setKey(key);
		return strdup(mod->renderText().c_str());
	}
	return NULL;
}

char *BackEnd::get_raw_text(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);

	if (mod) {
		mod->setKey(key);
		return strdup((char *)mod->getRawEntry());
	}
	return NULL;
}

char *BackEnd::render_this_text(const char *module_name, const char *text)
{
	SWModule *mod = get_SWModule(module_name);

	if (mod) {
		return strdup(mod->renderText(text).c_str());
	}
	return NULL;
}

char *BackEnd::get_strip_text_from_string(const char *module_name, const char *string)
{
	SWModule *mod = get_SWModule(module_name);

	if (mod) {
		return strdup((char *)mod->stripText(string));
	}
	return NULL;
}
char *BackEnd::get_strip_text(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);

	if (mod) {
		mod->setKey(key);
		return strdup((char *)mod->stripText());
	}
	return NULL;
}

char *BackEnd::get_valid_key(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return strdup("Gen.1.1");

	VerseKey *vkey = (VerseKey *)(SWKey *)(*mod);
	char *mykey;

	int t = module_type(module_name);
	if ((t == TEXT_TYPE) || (t == COMMENTARY_TYPE))
		vkey->setAutoNormalize(1);
	vkey->setText(key);

	// what's with this flagrant anglocentrism?
	if (sword_locale && !strcmp(sword_locale, "en"))
		mykey = strdup((char *)vkey->getShortText());
	else
		mykey = strdup((char *)vkey->getText());
	return mykey;
}

char *BackEnd::key_get_book(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return strdup("Genesis");

	VerseKey *vkey = (VerseKey *)(SWKey *)(*mod);

	vkey->setAutoNormalize(1);
	vkey->setText(key);
	char *s = strdup((char *)vkey->getBookName());
	return s;
}

int BackEnd::key_get_chapter(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return 1;

	VerseKey *vkey = (VerseKey *)(SWKey *)(*mod);

	vkey->setAutoNormalize(1);
	vkey->setText(key);
	int c = vkey->getChapter();
	return c;
}

int BackEnd::key_get_verse(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return 1;

	VerseKey *vkey = (VerseKey *)(SWKey *)(*mod);

	vkey->setAutoNormalize(1);
	vkey->setText(key);
	int v = vkey->getVerse();
	return v;
}

unsigned int BackEnd::key_chapter_count(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return 1;

	VerseKey *vkey = (VerseKey *)(SWKey *)(*mod);

	vkey->setAutoNormalize(1);
	vkey->setText(key);
	int max = vkey->getChapterMax();
	return max;
}

unsigned int BackEnd::key_verse_count(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return 1;

	VerseKey *vkey = (VerseKey *)(SWKey *)(*mod);

	vkey->setAutoNormalize(1);
	vkey->setText(key);
	int max = vkey->getVerseMax();
	return max;
}

char *BackEnd::get_module_key()
{
	display_mod->getRawEntry();

	return strdup((char *)display_mod->getKeyText());
}

void BackEnd::save_entry(const char *entry)
{
	display_mod->setEntry((const char *)entry);
}

void BackEnd::save_note_entry(const char *module, const char *key, const char *entry)
{
	display_mod = main_mgr->Modules[module];

	if (display_mod) {
		display_mod->setKey(key);
		display_mod->getKeyText(); /* snap to entry */
		display_mod->setEntry((const char *)entry);
		XI_message(("\nsave_note_entry\nmod: %s\nkey: %s\nentry: %s",
			    display_mod->getName(), display_mod->getKeyText(),
			    display_mod->renderText().c_str()));
	}
}

void BackEnd::delete_entry(void)
{
	display_mod->deleteEntry();
}

const char *BackEnd::module_get_language(const char *module_name)
{
	SWModule *mod = get_SWModule(module_name);

	if (mod)
		return main_get_language_map(mod->getLanguage());
	return "unknown";
}

char **BackEnd::get_module_language_list(void)
{
	char **retval;
	int i;

	std::set<SWBuf> module_languages;
	std::set<SWBuf>::iterator ml_it;

	// create a set of unique language names in use by modules.
	ModMap::iterator it;
	for (it = main_mgr->Modules.begin(); it != main_mgr->Modules.end(); ++it)
		module_languages.insert((*it).second->getLanguage());

	// construct a list of char* from the uniquified set.
	retval = g_new0(char *, module_languages.size() + 1);
	for (i = 0, ml_it = module_languages.begin();
	     ml_it != module_languages.end();
	     ++i, ++ml_it) {
		retval[i] = g_strdup(ml_it->c_str());
	}
	return retval;
}

int BackEnd::is_module(const char *module_name)
{
	//	if (module_name == NULL)
	//		return 0;

	SWModule *mod = get_SWModule(module_name);
	return ((mod != NULL) ? 1 : 0);
}

int BackEnd::module_type(const char *mod_name)
{
	//	if ((!mod_name) || (strlen(mod_name) < 2))
	//		return -1;

	SWModule *mod = get_SWModule(mod_name);

	if (mod) {

		if (!strcmp(mod->getType(), TEXT_MODS)) {
			return TEXT_TYPE;
		}

		if (!strcmp(mod->getType(), COMM_MODS)) {
			if (!strcmp((char *)mod->getConfigEntry("ModDrv"), "RawFiles"))
				return PERCOM_TYPE;
			return COMMENTARY_TYPE;
		}

		if (!strcmp(mod->getType(), DICT_MODS)) {
			return DICTIONARY_TYPE;
		}

		if (!strcmp(mod->getType(), BOOK_MODS)) {
			if (mod->getConfigEntry("GSType") &&
			    !strcmp((char *)mod->getConfigEntry("GSType"), "PrayerList"))
				return PRAYERLIST_TYPE;
			return BOOK_TYPE;
		}
	}
	return -1;
}

const char *BackEnd::module_description(const char *mod_name)
{
	if ((!mod_name) || (strlen(mod_name) < 2))
		return NULL;

	SWModule *mod = get_SWModule(mod_name);

	if (mod) {
		return mod->getDescription();
	}
	return NULL;
}

char *BackEnd::module_name_from_description(char *description)
{
	ModMap::iterator it;
	char *retval = NULL;

	if (!description)
		return NULL;

	for (it = main_mgr->Modules.begin();
	     it != main_mgr->Modules.end(); ++it) {
		if (!strcmp((*it).second->getDescription(), description))
			retval = strdup((*it).second->getName());
	}
	return retval;
}

int BackEnd::get_key_testament(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return 1;

	VerseKey *ikey = (VerseKey *)(SWKey *)(*mod);

	ikey->setText(key);
	return ikey->getTestament();
}

int BackEnd::module_has_testament(const char *module_name, int testament)
{
	SWModule *module = get_SWModule(module_name);

	int ot = 0;
	int nt = 0;

	if (module) {
		module->setSkipConsecutiveLinks(true);

		*module = sword::TOP; //position to first entry
		VerseKey *key = (VerseKey *)module->createKey();

		key->setText(module->getKeyText());
		if (key->getTestament() == 1) { // OT && NT
			ot = 1;
		} else if (key->getTestament() == 2) { //no OT
			ot = 0;
		}

		*module = sword::BOTTOM;
		key->setText(module->getKeyText());
		if (key->getTestament() == 1) { // only OT, no NT
			nt = 0;
		} else if (key->getTestament() == 2) { //has NT
			nt = 1;
		}

		delete key;
		module->setSkipConsecutiveLinks(false);
	}

	switch (testament) {
	case 1:
		return ot > 0;
	case 2:
		return nt > 0;
	default:
		return false;
	}
}

int BackEnd::module_get_testaments(const char *module_name)
{
	SWModule *module = get_SWModule(module_name);

	int ot = 0;
	int nt = 0;

	if (module) {
		module->setSkipConsecutiveLinks(true);

		*module = sword::TOP; //position to first entry
		VerseKey *key = (VerseKey *)module->createKey();
		key->setText(module->getKeyText());

		if (key->getTestament() == 1) { // OT && NT
			ot = 1;
		} else if (key->getTestament() == 2) { //no OT
			ot = 0;
		}

		*module = sword::BOTTOM;
		key->setText(module->getKeyText());

		if (key->getTestament() == 1) { // only OT, no NT
			nt = 0;
		} else if (key->getTestament() == 2) { //has NT
			nt = 1;
		}

		delete key;
		module->setSkipConsecutiveLinks(false);
	}

	if (ot && nt)
		return 2;
	else if (!ot && nt)
		return 1;
	else if (ot && !nt)
		return 0;
	return -1;
}

char *BackEnd::get_entry_attribute(const char *level1,
				   const char *level2,
				   const char *level3,
				   bool render)
{
	UTF8HTML u2html;
	if (render)
		display_mod->renderText();
	SWBuf attribute2 = display_mod->getEntryAttributes()[level1][level2][level3].c_str();

	u2html.processText(attribute2);

	if (attribute2.length()) {
		return strdup(attribute2.c_str());
	}
	return NULL;
}

int BackEnd::set_module(const char *module_name)
{
	display_mod = main_mgr->Modules[module_name];
	if (display_mod)
		return 1;
	else
		return 0;
}

int BackEnd::set_module_key(const char *module_name, const char *key)
{
	display_mod = main_mgr->Modules[module_name];

	if (display_mod) {
		display_mod->setKey(key);
		return 1;
	} else
		return 0;
}

int BackEnd::set_key(const char *key)
{
	if (!key)
		return 0;
	if (display_mod) {
		XI_message((f_message, 758, "key", key));
		display_mod->setKey(key);
		return 1;
	}
	return 0;
}

char *BackEnd::get_key_from_offset(unsigned long offset)
{
	if (tree_key) {
		TreeKeyIdx treenode = *tree_key;
		treenode.setOffset(offset);
		/** if not root node then display **/
		if (treenode.getOffset() > 0) {
			display_mod->setKey(treenode);
			display_mod->getKeyText(); //snap to entry
		}
		return strdup(display_mod->getKeyText());
	}
	return NULL;
}

unsigned long BackEnd::treekey_set_key(char *key)
{
	if (tree_key) {
		TreeKeyIdx treenode = *tree_key;
		treenode.setText(key);
		display_mod->setKey(treenode);
		display_mod->getKeyText(); //snap to entry
		return treenode.getOffset();
	}
	return 0;
}

void BackEnd::set_treekey(unsigned long offset)
{
	if (!display_mod) {
		gui_generic_warning(_("Xiphos failure: \"can't happen\", display_mod: null"
				      "\nPlease report this. sword_main.cc:set_treekey"
				      "\nWhat were you doing when this occurred?"));
		return;
	}
	if (tree_key)
		delete tree_key;
	tree_key = (TreeKeyIdx *)display_mod->createKey();
	if (tree_key) {
		TreeKeyIdx treenode = *tree_key;
		treenode.setOffset(offset);
		/** if not root node then display **/
		if (treenode.getOffset() > 0) {
			display_mod->setKey(treenode);
			display_mod->getKeyText(); //snap to entry
		}
	}
}

unsigned long BackEnd::get_treekey_offset_from_key(const char *module_name, const char *key)
{
	SWModule *mod = get_SWModule(module_name);
	unsigned long retval = 0;

	if (mod) {
		TreeKeyIdx *tree_key_idx = (TreeKeyIdx *)mod->createKey();
		tree_key_idx->setText(key);
		mod->setKey(tree_key_idx);
		//(char*)mod;
		retval = tree_key_idx->getOffset();
		delete tree_key_idx;
	}
	return retval;
}

unsigned long BackEnd::get_treekey_offset(void)
{
	if (tree_key)
		return tree_key->getOffset();
	return 0;
}

int BackEnd::treekey_has_children(unsigned long offset)
{
	if (tree_key) {
		tree_key->setOffset(offset);
		return tree_key->hasChildren();
	}
	return false;
}

int BackEnd::treekey_first_child(unsigned long offset)
{
	if (tree_key) {
		tree_key->setOffset(offset);
		return tree_key->firstChild();
	}
	return false;
}

int BackEnd::treekey_parent(unsigned long offset)
{
	if (tree_key) {
		tree_key->setOffset(offset);
		return tree_key->parent();
	}
	return false;
}

char *BackEnd::treekey_get_local_name(unsigned long offset)
{
	if (tree_key) {
		tree_key->setOffset(offset);
		return strdup((char *)tree_key->getLocalName());
	}
	return NULL;
}

int BackEnd::treekey_next_sibling(unsigned long offset)
{
	if (tree_key) {
		tree_key->setOffset(offset);
		if (tree_key->nextSibling()) {
			return 1;
		}
	}
	return 0;
}

int BackEnd::treekey_prev_sibling(unsigned long offset)
{
	if (tree_key) {
		tree_key->setOffset(offset);
		if (tree_key->previousSibling()) {
			return 1;
		}
	}
	return 0;
}

char *BackEnd::navigate_module(int direction)
{
	if (direction == -1)
		return strdup((char *)display_mod->getKeyText());

	switch (direction) {
	case 0:
		(*display_mod)--;
		break;
	case 1:
		(*display_mod)++;
		break;
	}
	display_mod->popError();
	return strdup((char *)display_mod->getKeyText());
}

GList *BackEnd::parse_verse_list(const char *module_name, const char *list, char *current_key)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return NULL;

	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	GList *retlist = NULL;
	ListKey vs;

	if (!list)
		return retlist;
	XI_message(("current_key=%s", current_key));
	key->setText(current_key);
	vs = key->parseVerseList(list, *key, TRUE);

	if (!vs.getCount())
		return retlist;
	while (!vs.popError()) {
		retlist = g_list_append(retlist, strdup((char *)vs.getText()));
		vs++;
	}
	return retlist;
}

GList *BackEnd::parse_range_list(const char *module_name, const char *list)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return NULL;

	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	GList *retlist = NULL;
	const char *buf = NULL;

	key->setText(mod->getKeyText());

	verses.clear();
	verses = key->parseVerseList(list, *key, true);

	buf = verses.getRangeText();
	retlist = g_list_append(retlist, g_strdup(buf));

	return retlist;
}

void BackEnd::set_listkey_position(char pos)
{
	results.setPosition((char)pos);
}

const char *BackEnd::get_next_listkey(void)
{
	// formerly, this used a const char *retval, which was assigned
	// to results.getText().  this seems to have a problem in
	// 64-bit worlds (only), where retval would be trashed by results++
	// because the underlying keytext (sword/src/keys/listkey.cpp)
	// would be updated, leaving our retval bogus.  OW.
	// we solve the problem by copying the data while it's current.
	// it's ugly (returning a pointer to a static char []) but it works.

	static char retval[128];

	while (!results.popError()) {
		(void)g_strlcpy(retval, results.getText(), 126);
		results++;
		return retval;
	}
	return NULL;
}

int BackEnd::clear_scope(void)
{
	current_scope = 0;
	return 1;
}

int BackEnd::clear_search_list(void)
{
	search_scope_list.clear();
	return search_scope_list.getCount();
}

int BackEnd::set_range(const char *module_name, const char *list)
{
	SWModule *mod = get_SWModule(module_name);
	if (!mod)
		return 0;

	VerseKey *key = (VerseKey *)(SWKey *)(*mod);

	key->setText(mod->getKeyText());
	search_range = key->parseVerseList(list, "", true);
	return search_range.getCount();
}

void BackEnd::set_scope2range(void)
{
	current_scope = &search_range;
}

int BackEnd::set_scope2last_search(void)
{
	current_scope = &search_scope_list; //-- move searchlist into current_scope
	return 1;
}

int BackEnd::do_module_index(char *module_name)
{

	search_mod = main_mgr->Modules[module_name];
	if (!search_mod)
		return -1;
	char progressunits = 70;
	if (!search_mod->hasSearchFramework())
		return 0;

	search_mod->deleteSearchFramework();
	search_mod->createSearchFramework(main_index_percent_update,
					  (void *)&progressunits);
	return 1;
}

int BackEnd::do_module_delete_index(char *module_name)
{

	search_mod = main_mgr->Modules[module_name];
	if (!search_mod)
		return -1;
	if (!search_mod->hasSearchFramework())
		return 0;

	search_mod->deleteSearchFramework();
	return 1;
}

int BackEnd::check_for_optimal_search(char *module_name)
{
	search_mod = main_mgr->Modules[module_name];

	if (!search_mod)
		return -2;
	if (search_mod->hasSearchFramework() &&
	    search_mod->isSearchOptimallySupported("God", -4, 0, 0))
		return -4; // ** indexed search - clucene **
	else
		return -2; // ** word search **
}

int BackEnd::do_module_search(char *module_name,
			      const char *search_string,
			      int search_type,
			      int search_params,
			      int is_dialog)
{
	char progressunits = 70;
	results.clear();
	//search_scope_list.clear()
	search_mod = NULL;

	search_mod = main_mgr->Modules[module_name];
	if (!search_mod)
		return -1;
	if ((current_scope == &search_scope_list) &&
	    (search_scope_list.getCount() == 0))
		return 0;

	results = search_mod->search(search_string,
				     search_type,
				     search_params,
				     current_scope, 0,
				     (is_dialog
					  ? main_dialog_search_percent_update
					  : main_sidebar_search_percent_update),
				     (void *)&progressunits);
	search_scope_list = results;
	if (search_type == -4)
		results.sort();
	return results.getCount();
}

void BackEnd::terminate_search()
{
	if (search_mod)
		search_mod->terminateSearch = true;
}

char *BackEnd::get_conf_file_item(const char *file, const char *mod_name, const char *item)
{
	char *buf = NULL;
	SWConfig conf_file(file);
	conf_file.load();

	buf = (char *)conf_file[mod_name][item].c_str();
	if (strlen(buf))
		return strdup(buf);
	else
		return NULL;
}

void BackEnd::save_conf_file_item(const char *file,
				  const char *mod_name,
				  const char *item,
				  const char *value)
{
	SWConfig conf_file(file);
	conf_file[mod_name][item] = value;
	conf_file.save();
}

void BackEnd::save_module_key(char *mod_name, char *key)
{
	SectionMap::iterator section;
	ConfigEntMap::iterator entry;

	char *fullfilename;

	// first try: $HOME.
	char *confdir = g_strdup_printf("%s/%s",
					settings.homedir, DOTSWORD);
	char *conffile = main_get_mod_config_file(mod_name, confdir);

	if (conffile) {
		fullfilename = g_strdup_printf("%s/mods.d/%s",
					       confdir, conffile);
		g_free(confdir);
		g_free(conffile);
	} else {
		g_free(confdir);

		// second try: system location.
		conffile = main_get_mod_config_file(mod_name,
						    settings.path_to_mods);

		if (conffile) {
			fullfilename = g_strdup_printf("%s/mods.d/%s",
						       settings.path_to_mods,
						       conffile);
			g_free(conffile);
		} else {
			gui_generic_warning(_("Configuration not found"));
			return;
		}
	}

	SWConfig *myConfig = new SWConfig(fullfilename);
	g_free(fullfilename);

	section = myConfig->getSections().find(mod_name);
	if (section != myConfig->getSections().end()) {
		entry = section->second.find("CipherKey");
		if (entry != section->second.end()) {
			//-- set cipher key
			entry->second = key;
			//-- save config file
			myConfig->save();
		}
	}
	delete myConfig;
}

const char *BackEnd::get_osisref_from_key(const char *module, const char *key)
{
	ModMap::iterator it;

	// we need an OSISRef in module-sensitive context.
	// fallback positions: we hope for a supplied module.
	// if not, assume KJV (v11n historical reasons; not anglocentric).
	// if not, use the 1st module...by which time we're just guessing.

	it = main_mgr->Modules.find((module && *module) ? module : "KJV");
	if (it == main_mgr->Modules.end()) {
		it = main_mgr->Modules.begin();
		if (it == main_mgr->Modules.end())
			return strdup("");
	}

	SWModule *mod = it->second;
	VerseKey *vkey = (VerseKey *)(SWKey *)(*mod);
	vkey->setText(key);
	return strdup(vkey->getOSISRef());
}

/* end of file */
