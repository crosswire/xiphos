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
#include <treekeyidx.h>
#include "main/lists.h"

using namespace sword;
	

class SwordMain {
	SWMgr *main_mgr;
	SWMgr *search_mgr;
	SWMgr *module_mgr;
	SWMgr *text_mgr;
	SWMgr *inter_mgr;
	SWMgr *comm_mgr;
	SWMgr *dict_mgr;
	SWMgr *gbs_mgr;
	SWMgr *percom_mgr;
	SWMgr *results;
	SWMgr *display_mgr;
public:
	
	SWModule *text_mod;
	SWModule *comm_mod;
	SWModule *dict_mod;
	SWModule *gbs_mod;
	SWModule *percom_mod;
	
	TreeKeyIdx *treeKey;	
	const char *version;	
	SWDisplay *commDisplay;	
	SWDisplay *dictDisplay;

	SwordMain();
	~SwordMain();
	void init_SWORD();
	void init_lists(MOD_LISTS * mods);
	GList *fill_Bible_books(int testament);
	void get_module_options(GList * options);
	
	SWMgr *get_display_mgr(void) {return(display_mgr);};
};
extern SwordMain *backend;

#endif /* _SWORD_MAIN_HH_ */
