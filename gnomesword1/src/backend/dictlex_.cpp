/*
 * GnomeSword Bible Study Tool
 * dictlex_.cpp - support for Sword commentary modules
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
#include <swmodule.h>

#include "main/settings.h"

#include "backend/sword.h"
#include "backend/dictlex_.h"


/******************************************************************************
 * global to this file only 
 */
static SWModule *mod;
static SWMgr *mgr;


/******************************************************************************
 * Name
 *   backend_get_first_key
 *
 * Synopsis
 *   #include ".h"
 *
 *   char * backend_get_first_key(void)	
 *
 * Description
 *    returns the first key in the module
 * 
 * Return value
 *   char *
 */
 
char *backend_get_first_dictlex_key(void)
{
	char *retval = NULL;

	if (!stricmp(mod->Name(), "WebstersDict"))
		mod->SetKey("A");
	else
		(*mod) = TOP;

	if (!mod->Error()) {
		//- retval must be freed by calling function
		retval = g_strdup((const char *) mod->KeyText());
	}
	return retval;
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include ".h"
 *
 *   
 *
 * Description
 *    returns the next key in the module
 * 
 * Return value
 *   char *
 */

char *backend_get_next_dictlex_key(void)
{
	char *retval = NULL;

	(*mod)++;
	if (!mod->Error()) {
		//- retval must be freed by calling function
		retval = g_strdup((const char *) mod->KeyText());
	}
	return retval;
}

/******************************************************************************
 * Name
 *  backend_set_dictlex_module
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   int backend_set_dictlex_module(char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   int
 */
 
int backend_set_dictlex_module(char * mod_name)
{
	mod = mgr->Modules[mod_name];
	if(mod)
		return 1;
	return 0;
}

/******************************************************************************
 * Name
 *  
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   	
 *
 * Description
 *    
 *
 * Return value
 *   
 */
 
int backend_set_dictlex_key(char * key)
{
	if(mod) {
		mod->SetKey(key);
		return 1;
	}
	return 0;
}

/******************************************************************************
 * Name
 *  backend_get_dictlex_key
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   char *backend_get_dictlex_key(int direction)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *backend_get_dictlex_key(int direction)
{
	if(mod) {
		if(direction == -1)
			return strdup((char*)mod->KeyText());
		else if(direction == 1)
			(*mod)++;
		else
			(*mod)--;
		mod->Error();
		return strdup((char*)mod->KeyText());
	}
	return NULL;
}

/******************************************************************************
 * Name
 *   backend_get_dictlex_text
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   char *backend_get_dictlex_text(char * mod_name, char * key)	
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */
 
char *backend_get_dictlex_text(char * mod_name, char * key)
{
	mod = mgr->Modules[mod_name];
	
	if(mod) {
		mod->SetKey(key);
		return strdup((char*)mod->RenderText());
	}
	return NULL;
}

/******************************************************************************
 * Name
 *  backend_setup_dictlex
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   void backend_setup_dictlex(SETTINGS * s)
 *
 * Description
 *   setup sword support for dict/lex modules 
 *
 * Return value
 *   void
 */
 
void backend_setup_dictlex(void)
{
	ModMap::iterator it;	//-- iteratior
	
	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));	//-- create sword mgrs
	for (it = mgr->Modules.begin();
	     it != mgr->Modules.end(); it++) {
		if (!strcmp
		    ((*it).second->Type(), "Lexicons / Dictionaries")) {
			mod = (*it).second;
			if(!strcmp((*it).second->Name(),"Thayer"))
				settings.havethayer = true;
			if(!strcmp((*it).second->Name(),"BDB"))
				settings.havebdb = true;
		}
	}
}

/******************************************************************************
 * Name
 *  backend_shutdown_dictlex
 *
 * Synopsis
 *   #include "dictlex_.h"
 *
 *   void backend_shutdown_dictlex(void)	
 *
 * Description
 *    shutdown and cleanup
 *
 * Return value
 *   void
 */
 
void backend_shutdown_dictlex(void)
{
	delete mgr;
}

