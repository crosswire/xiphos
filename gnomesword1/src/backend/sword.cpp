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

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#include <gnome.h>
#else
#include <glib-1.2/glib.h>
#endif
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <swversion.h>
#include <swmodule.h>
#include <localemgr.h>

#include "main/sword.h"
#include "main/lists.h"
#include "main/display.hh"

#include "backend/sword.h"
#include "backend/sword_defs.h"
#include "backend/sword_main.hh"

using namespace sword;
  
  
  
static char *get_sword_locale(void)
{
	const char *sys_local;
	char *retval = NULL;
	char buf[32];
	int i = 0;
	
#ifdef USE_SWORD_CVS
	sys_local = LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName();
#else	
	sys_local = LocaleMgr::systemLocaleMgr.getDefaultLocaleName();	
#endif
	
	if(!strncmp(sys_local,"ru_RU",5)) {
		/*if(strlen(sys_local) > 12 ) {
			for(i = 0; i < 12; i++) {
				buf[i] = sys_local[i];
				buf[i+1] = '\0';
			}
			sys_local = "ru_RU-koi8-r";//buf;
		}*/
		sys_local = "ru_RU-koi8-r";//buf;
		
	} else if(!strncmp(sys_local,"ru_RU-koi8-r",10)){
		if(strlen(sys_local) >  12) {
			for(i = 0; i < 12; i++) {
				buf[i] = sys_local[i];
				buf[i+1] = '\0';
			}
			sys_local = buf;
		}
		
	} else if(!strncmp(sys_local,"uk_UA-cp1251",10)){
		if(strlen(sys_local) > 12 ) {
			for(i = 0; i < 12; i++) {
				buf[i] = sys_local[i];
				buf[i+1] = '\0';
			}
			sys_local = buf;
		}
		
	} else if(!strncmp(sys_local,"uk_UA-koi8-u",10)){
		if(strlen(sys_local) > 12 ) {
			for(i = 0; i < 12; i++) {
				buf[i] = sys_local[i];
				buf[i+1] = '\0';
			}
			sys_local = buf;
		}
		
	} else if(!strncmp(sys_local,"pt_BR",5)){
		if(strlen(sys_local) > 5 ) {
			for(i = 0; i < 5; i++) {
				buf[i] = sys_local[i];
				buf[i+1] = '\0';
			}
			sys_local = buf;
		}
		
	} else if(!strncmp(sys_local,"en_GB",5)){
		if(strlen(sys_local) > 5 ) {
			for(i = 0; i < 5; i++) {
				buf[i] = sys_local[i];
				buf[i+1] = '\0';
			}
			sys_local = buf;
		}
	} else {
		if(strlen(sys_local) > 2 ) {
			buf[0] = sys_local[0];
			buf[1] = sys_local[1];
			buf[2] = '\0';
			sys_local = buf;
		}
	}
	retval = strdup(sys_local);
	
#ifdef USE_SWORD_CVS
	LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(sys_local);
#else	
	LocaleMgr::systemLocaleMgr.setDefaultLocaleName(sys_local);
#endif
	return retval;
}

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
	char *sword_locale = NULL;
	sw.version = get_sword_version();
	ModMap::iterator it; 
	g_print("gnomesword-%s\n", VERSION);
	g_print("sword-%s\n", sw.version);
	g_print("%s\n\n", _("Initiating SWORD"));
	g_print("%s %s\n", _("System locale is"),
#ifdef USE_SWORD_CVS
		LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName());
#else		
		LocaleMgr::systemLocaleMgr.getDefaultLocaleName());
#endif
	sword_locale = get_sword_locale();
	g_print("%s %s\n\n", _("SWORD locale is"), sword_locale);
	g_print("%s\n", _("Checking for SWORD Modules"));
	free((char*)sword_locale);
	//backend_init_language_map();
	/* create sword mgrs */
	backend_init_managers();
	backend = new BackEnd();
	backend->init_SWORD(0);
	main_init_lists();
//	backend_setup_treekey();
	backend_setup_display_mgr();
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
//	backend_shutdown_treekey();
	//main_delete_displays();
	if(sw.entryDisplay)
		delete sw.entryDisplay;
	if(sw.dictDisplay)
		delete sw.dictDisplay;
	/* delete Sword managers */
	backend_delete_managers();
	delete backend;
	g_print("%s\n", _("SWORD is shutdown"));
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
