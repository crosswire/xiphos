/*
 * GnomeSword Bible Study Tool
 * bibletext_.cpp - support for Sword Bible text modules
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

#include <gnome.h>

#include "backend/bibletext_.h"
#include "backend/sword_defs.h"


/******************************************************************************
 * globals to this file only 
 */
static SWMgr *mgr;


/******************************************************************************
 * Name
 *   backend_setup_bibletext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void backend_setup_bibletext(void)	
 *
 * Description
 *   setup the bibletext sword manager
 *
 * Return value
 *   void
 */

void backend_setup_bibletext(void)
{
	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
}

/******************************************************************************
 * Name
 *   backend_shutdown_bibletext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   void backend_shutdown_bibletext(void)	
 *
 * Description
 *   shutdown down bibletext support
 *
 * Return value
 *   void
 */

void backend_shutdown_bibletext(void)
{
	delete mgr;
}

/******************************************************************************
 * Name
 *  backend_get_text_module_description
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   const char* backend_get_text_module_description(char * mod_name)	
 *
 * Description
 *    
 *
 * Return value
 *   const char*
 */
 
const char* backend_get_text_module_description(char * mod_name)
{	
	SWModule *mod = mgr->Modules[mod_name];
	if (mod)
		return mod->Description(); 
	else
		return NULL;
}

/******************************************************************************
 * Name
 *   backend_get_bibletext_text
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   char *backend_get_bibletext_text(char *mod_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */

char *backend_get_bibletext_text(char *mod_name, char *key)
{
	SWModule *mod = mgr->Modules[mod_name];
	if (mod)
		mod->SetKey(key);
	else
		return NULL;
	return strdup((char *) mod->RenderText());
}


/******************************************************************************
 * Name
 *   backend_get_bibletext_striptext
 *
 * Synopsis
 *   #include "bibletext.h"
 *
 *   char *backend_get_bibletext_striptext(char *mod_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */

char *backend_get_bibletext_striptext(char *mod_name, char *key)
{
	SWModule *mod = mgr->Modules[mod_name];
	if (mod)
		mod->SetKey(key);
	else
		return NULL;
	return strdup((char *) mod->StripText());
}


/******************************************************************************
 * Name
 *  backend_set_text_global_option
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void backend_set_text_global_option(char * option, char * yesno)	
 *
 * Description
 *   sets sword module global options ie strongs, morph tags or footnotes
 *
 * Return value
 *   void
 */

void backend_set_text_global_option(char * option, char * yesno)
{
	/* 
	 * turn option on or off 
	 */
	mgr->setGlobalOption(option, yesno);
	
	sw_mgr.search->setGlobalOption(option, yesno);
	sw_mgr.results->setGlobalOption(option, yesno);
}

/******************************************************************************
 * Name
 *  backend_set_module_unlocked
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   void backend_set_module_unlocked(char *mod_name, char *key)	
 *
 * Description
 *   unlocks locked module - FIXME: does not work :(
 *
 * Return value
 *   void
 */

void backend_set_module_unlocked(char * mod_name, char * key)
{	/* this does not work */
	//g_warning("module is %s\nkey = %s",mod_name,key);
	//int rslts = mgr->setCipherKey(mod_name, key);
	delete mgr;
	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
	//g_warning("rslts = %d",rslts);
}

/******************************************************************************
 * Name
 *  backend_check_for_global_option
 *
 * Synopsis
 *   #include "bibletext.h"
 *   
 *   int backend_check_for_global_option(int mod_num, char *option)	
 *
 * Description
 *   returns true is module has option
 *
 * Return value
 *   int
 */

int backend_check_for_global_option(char * mod_name, char * option)
{		
	SWModule *mod = mgr->Modules[mod_name];
	if (mod)
		return mod->getConfig().has("GlobalOptionFilter",option);
	else
		return 0;
}

/******   end of file   ******/
