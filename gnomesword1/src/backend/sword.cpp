/*
 * GnomeSword Bible Study Tool
 * sword.cpp support for sword modules
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

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#include <glib-1.2/glib.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swversion.h>
#include <swmodule.h>
#include <localemgr.h>

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"

#include "backend/sword.h"
#include "backend/sword_defs.h"
#include "backend/bookmarks.h"
#include "backend/percomm_.h"

using namespace sword;



/******************************************************************************
 * Name
 *   backend_init
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_init(void)	
 *
 * Description
 *   start sword 
 *
 * Return value
 *   void
 */
 
void backend_init(void)
{	
	ModMap::iterator it;
	g_print("gnomesword-%s\n", VERSION);
	g_print("%s\n", "Initiating Sword\n");		
	g_print("Sword locale is %s\n",
		LocaleMgr::systemLocaleMgr.getDefaultLocaleName());
	g_print("%s\n", "Checking for SWORD Modules");
	
	/*
	 *create sword mgrs
	 */
	backend_init_managers();
	
	init_lists();
	backend_setup_treekey();
	
	/*
	 *   setup Personal Comments
	 */	
	if(settings.havepercomm)
		backend_setup_percomm();
}



/******************************************************************************
 * Name
 *   backend_shutdown
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   void backend_shutdown(void)	
 *
 * Description
 *   close down sword 
 *
 * Return value
 *   void
 */

void backend_shutdown(int save_properties)
{
	backend_shutdown_treekey();	
	backend_shutdown_percomm();
	
	/*
	 * delete Sword managers
	 */
	backend_delete_managers();
	g_print("\nSword is shutdown\n");
}


/******************************************************************************
 * Name
 *   backend_get_sword_version
 *
 * Synopsis
 *   #include "sword.h"
 *
 *   const char *backend_get_sword_version(void)	
 *
 * Description
 *    returns the version number of the sword libs
 *
 * Return value
 *   const char*
 */

const char *backend_get_sword_version(void)
{
	SWVersion retval;
	retval = SWVersion::currentVersion;
	return retval;
}
