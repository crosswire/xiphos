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

#ifndef _SWORD_MAIN_HH_
#define _SWORD_MAIN_HH_

	
#include <swmgr.h>
#include <swbuf.h>
#include <versekey.h>
#include <treekeyidx.h>
#include "main/lists.h"

using std::string;
using std::map;
using std::list;
using namespace sword;
	

class BackEnd {
	SWMgr *main_mgr;
	SWMgr *display_mgr;
	TreeKeyIdx *tree_key;
	ListKey results;
	ListKey search_range;
	ListKey	search_scope_list; //--search list for searching verses found on last search
	ListKey verses;
	VerseKey search_scope_bounds; //----- sets lower and upper search bounds
	SWKey *current_scope;
	SWModule *search_mod;
	typedef map < SWBuf, SWBuf > ModLanguageMap;
	ModLanguageMap languageMap;
	
public:	
	SWModule *display_mod;
		
	const char *version;	
	SWDisplay *commDisplay;	
	SWDisplay *dictDisplay;
	SWDisplay *textDisplay;
	SWDisplay *RTOLDisplay;
	SWDisplay *entryDisplay;		
	SWDisplay *chapDisplay;			
	SWDisplay *dialogRTOLDisplay;
	SWDisplay *verselistDisplay;		
	SWDisplay *viewerDisplay;

	BackEnd();
	~BackEnd();
	void init_SWORD(int gsType);
	void init_lists(MOD_LISTS * mods);
	void init_language_map(void);
	void setup_displays(void);

	GList *fill_Bible_books(int testament);

	GList *get_module_options(void);
	int has_global_option(char *mod_name, char *option);
	char *get_config_entry(char * module_name, char * entry);
	int is_Bible_key(const char * list, char * current_key);
	char *get_render_text(const char *module_name, const char *key);
	char *get_strip_text(const char *module_name, const char *key);
	char *get_valid_key(const char *key);
	char *key_get_book(const char *key);
	int key_get_chapter(const char *key);
	const unsigned int key_chapter_count(const char *key);
	const unsigned int key_verse_count(const char *key);
	
	
	char *get_module_key(void);
	void save_entry(const char * entry);
	void delete_entry(void);
	int set_module_key(const char *module_name, const char *key);
	int set_key(const char *key);
	int is_module(const char *mod_name);
	int set_module(const char *module_name);
	char *navigate_module(int direction);
	
	int module_type(char *mod_name);
	char *module_description(char *mod_name);
	char *module_name_from_description(char *description);
	const char *module_get_language(const char *module_name);
	int module_has_testament(const char * module_name,  int testament);
	const char *get_language_map(const char *language);
	
	char *get_entry_attribute(const char *level1, const char *level2, const char *level3);
	
	char *get_key_form_offset(unsigned long offset);
	void set_treekey(unsigned long offset);
	unsigned long get_treekey_offset(void);
	int treekey_has_children(unsigned long offset);
	int treekey_first_child(unsigned long offset);
	char *treekey_get_local_name(unsigned long offset);
	int treekey_next_sibling(unsigned long offset);
	
	void set_listkey_position(char pos);
	const char *get_next_listkey(void);
	int clear_scope(void);
	int clear_search_list(void);
	int set_range(char * list);
	void set_scope2range(void);
	int set_scope2last_search(void);
	int do_module_search(char *module_name, const char *search_string, 
				int search_type, int search_params, int is_dialog);
	
	
	SWMgr *get_display_mgr(void) {return(display_mgr);};
};
extern BackEnd *backend;

#endif /* _SWORD_MAIN_HH_ */
