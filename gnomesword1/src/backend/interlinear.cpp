/*
 * GnomeSword Bible Study Tool
 * interlinear.cpp - support for displaying multiple modules
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
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>

#include "backend/interlinear.h"

using namespace sword;

/******************************************************************************
 * locals
 */
static SWMgr *mgr;
static SWModule *mod;

/******************************************************************************
 * Name
 *   backend_setup_interlinear
 *
 * Synopsis
 *   #include "interlinear.h"
 *
 *   int backend_setup_interlinear(void)	
 *
 * Description
 *   setup interlinear sword support
 *
 * Return value
 *   int
 */
int backend_setup_interlinear(void)
{
	ModMap::iterator it;	//-- iteratior     
	SectionMap::iterator sit;	//-- iteratior
	bool retval = false;

	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	mod = NULL;

	for (it = mgr->Modules.begin(); it != mgr->Modules.end(); it++) {
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			mod = (*it).second;
			retval = true;
		}
	}
	return retval;
}

/******************************************************************************
 * Name
 *   backend_shutdown_interlinear
 *
 * Synopsis
 *   #include "interlinear.h"
 *
 *   void backend_shutdown_interlinear(void)	
 *
 * Description
 *   shutdown down interlinear support
 *
 * Return value
 *   void
 */
void backend_shutdown_interlinear(void)
{
	delete mgr;
}

/******************************************************************************
 * Name
 *   backend_get_interlinear_module_text
 *
 * Synopsis
 *   #include "interlinear.h"
 *
 *   char *backend_get_interlinear_module_text(char * mod_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */
char *backend_get_interlinear_module_text(char *mod_name, char *key)
{
	mod = mgr->Modules[mod_name];
	if (mod)
		mod->SetKey(key);
	else
		return NULL;
	return strdup((char *) mod->RenderText());
}

/******************************************************************************
 * Name
 *   backend_set_interlinear_global_option
 *
 * Synopsis
 *   #include "interlinear.h"
 *
 *   void backend_set_interlinear_global_option(char * option, char * yesno)	
 *
 * Description
 *   set module global option for interlinear window
 *
 * Return value
 *   void
 */
void backend_set_interlinear_global_option(char * option, char * yesno)
{
	mgr->setGlobalOption(option, yesno);
}

/*******   end of file   ******/
