/*
 * GnomeSword Bible Study Tool
 * sword_main.hh - 
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

#ifndef _DIALOGS_HH_
#define _DIALOGS_HH_

	
#include <swmgr.h>
#include <swbuf.h>
#include <treekeyidx.h>
#include "main/lists.h"

using std::string;
using std::map;
using std::list;
using namespace sword;
	

class ModuleDialogs {
	SWMgr *mgr;
	TreeKeyIdx *tree_key;
	
public:	
	SWModule *mod;
		
	const char *version;	
	SWDisplay *entryDisplay;	
	SWDisplay *chapDisplay;

	ModuleDialogs();
	~ModuleDialogs();
	void init_SWORD(const char * mod_name);
	void init_lists(MOD_LISTS * mods);
	void setup_displays(void);

	GList *fill_Bible_books(int testament);

	void get_module_options(GList * options);
	int is_Bible_key(const char * list, char * current_key);
	char *get_valid_key(const char *key);
	char *key_get_book(const char *key);
	int key_get_chapter(const char *key);
	const unsigned int key_chapter_count(const char *key);
	const unsigned int key_verse_count(const char *key);
	int set_key(const char *key);
	
	
	char *get_module_key(void);
	int set_module_key(const char *module_name, const char *key);
	int is_module(const char *mod_name);
	int set_module(const char *module_name);
	char *navigate_module(int direction);
	
	int module_type(void);
	const char *module_get_language(const char *module_name);
	int module_has_testament(int testament);
	
	char *get_entry_attribute(const char *level1, const char *level2, const char *level3);
	
	char *get_key_form_offset(unsigned long offset);
	void set_treekey(unsigned long offset);
	unsigned long get_treekey_offset(void);
	int treekey_has_children(unsigned long offset);
	int treekey_first_child(unsigned long offset);
	char *treekey_get_local_name(unsigned long offset);
	int treekey_next_sibling(unsigned long offset);
	
	
	//SWMgr *get_display_mgr(void) {return(display_mgr);};
};

#endif /* _SWORD_MAIN_HH_ */
