/*
 * GnomeSword Bible Study Tool
 * mgr.cc - SHORT DESCRIPTION
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

#include <swmgr.h>
#include <markupfiltmgr.h>

#include "backend/sword.h"
#include "backend/sword_defs.h"

#include "main/settings.h"
#include "main/search.h"

using std::string;
using std::map;
using std::list;
using namespace sword;

#include "backend/mgr.hh"


/******************************************************************************
 * externs
 */	


/******************************************************************************
 * globals
 */

SWMgr *main_mgr;
SWMgr *search_mgr;
SWMgr *module_mgr;

/******************************************************************************
 * static  global to this file only 
 */

static OptionsList optionslist;
static OptionsList::iterator it;



/******************************************************************************
 * Name
 *   backend_init_main_mgr
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_init_main_mgr(void)	
 *
 * Description
 *   start sword main_mgr for settings (new bookmarks or preferences)
 *
 * Return value
 *   void
 */
 
void backend_init_main_mgr(void)
{
	main_mgr = new SWMgr();
}


/******************************************************************************
 * Name
 *   backend_delete_main_mgr
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   void backend_delete_main_mgr(void)	
 *
 * Description
 *   close down sword main_mgr
 *
 * Return value
 *   void
 */

void backend_delete_main_mgr(void)
{
	delete main_mgr;
}


void backend_new_module_mgr(void)
{
	module_mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
}

void backend_delete_module_mgr(void)
{
	delete module_mgr;	
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   	
 *
 * Description
 *   
 *
 * Return value
 *   
 */
 
void backend_set_global_option_iterator(void)
{
	optionslist = module_mgr->getGlobalOptions();
	it = optionslist.begin();
}


/******************************************************************************
 * Name
 *   backend_get_next_global_option
 *
 * Synopsis
 *   #include "backend/mgr.hh"
 *
 *   char *backend_get_next_global_option(void)	
 *
 * Description
 *   
 *
 * Return value
 *   int
 */
 
char *backend_get_next_global_option(void)
{
	char *retval = NULL;
	
	if(it != optionslist.end()) {
		retval = strdup((char *) (*it).c_str());
		it++;
		return retval;	
	}
	else
		return NULL;
}

