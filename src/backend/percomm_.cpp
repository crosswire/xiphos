/*
 * GnomeSword Bible Study Tool
 * percomm_.cpp - support for Sword personal commentary modules
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
#  include <config.h>
#endif

#include <glib-1.2/glib.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>

#include "backend/percomm_.h"

/******************************************************************************
 * globals to this file only 
 */

static SWMgr *mgr;
static SWModule *mod;


/******************************************************************************
 * Name
 *   backend_get_percomm_text
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   char *backend_get_percomm_text(char *mod_name, char *key)	
 *
 * Description
 *   return formated text for a verse
 *
 * Return value
 *   char *
 */

char *backend_get_percomm_text(char * key)
{
	if (mod){
		mod->SetKey(key);
		return strdup((char *) mod->RenderText());		
	}
	return NULL;
}

/******************************************************************************
 * Name
 *   backend_set_percomm_key
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_set_percomm_key(char * key)	
 *
 * Description
 *   set commentary key
 *
 * Return value
 *   void
 */

void backend_set_percomm_key(char * key)
{
	if (mod){
		mod->SetKey(key);
	}
}


/******************************************************************************
 * Name
 *   backend_change_percomm_module
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_change_percomm_module(char * mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
 
void backend_change_percomm_module(char * mod_name)
{	
	mod = mgr->Modules[mod_name];
}


/******************************************************************************
 * Name
 *  backend_get_percomm_key
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   char *backend_get_percomm_key(int mod_num)	
 *
 * Description
 *    returns the key for the current personal commentary
 *
 * Return value
 *   char *
 */
 
char *backend_get_percomm_key(void)
{	
	return strdup((char*)mod->KeyText());
}

/******************************************************************************
 * Name
 *  backend_save_personal_comment
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_save_personal_comment(int mod_num, char * buf)
 *
 * Description
 *   save buf to the current personal commentary at current key 
 *
 * Return value
 *   void
 */
 
void backend_save_personal_comment(char * note)
{	
	if(note)
		*mod << (const char *) note;
}

/******************************************************************************
 * Name
 *  backend_delete_personal_comment
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_delete_personal_comment(void)
 *
 * Description
 *    delete the data at the currnet key in current personal commentary
 *
 * Return value
 *   void
 */
 
void backend_delete_personal_comment(void)
{
	mod->deleteEntry();
	g_warning("key %s of module %s has been deleted",mod->KeyText(),mod->Name());
}
/******************************************************************************
 * Name
 *  backend_setup_percomm
 *
 * Synopsis
 *   #include "percomm_.h"
 *   
 *   void backend_setup_percomm(void)	
 *
 * Description
 *   setup sword text module support
 *
 * Return value
 *   void
 */

void backend_setup_percomm(void)
{
	mgr = new SWMgr(new MarkupFilterMgr(FMT_HTMLHREF));
}

/******************************************************************************
 * Name
 *  backend_shutdown_percomm
 *
 * Synopsis
 *   #include "percomm_.h"
 *   
 *   void backend_shutdown_percomm(void)	
 *
 * Description
 *   shut down sword suppoet
 *
 * Return value
 *   void
 */

void backend_shutdown_percomm(void)
{
	delete mgr;	
}
	
/******   end of file   ******/
