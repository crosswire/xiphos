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

#include <gnome.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "backend/percomm_.h"
#include "backend/sword.h"
#include "backend/display.h"

/******************************************************************************
 * globals to this file only 
 */

static SWMgr *mgr;

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
 *   const char *
 */
 
char *backend_get_percomm_key(char * mod_name)
{
	SWModule *mod = mgr->Modules[mod_name];
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
 
void backend_save_personal_comment(char * mod_name, char * buf)
{	
	SWModule *mod = mgr->Modules[mod_name];
	if (buf)
		*mod << (const char *) buf;
}

/******************************************************************************
 * Name
 *  backend_delete_personal_comment
 *
 * Synopsis
 *   #include "percomm_.h"
 *
 *   void backend_delete_personal_comment(int mod_num)
 *
 * Description
 *    delete the data at the currnet key in current personal commentary
 *
 * Return value
 *   void
 */
 
void backend_delete_personal_comment(char * mod_name)
{
	SWModule *mod = mgr->Modules[mod_name];
	g_warning("key %s of module %s would have be deleted",mod->KeyText(),mod->Name());
	/* not finished - does nothing */ 
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
