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


#include <glib-2.0/glib.h>
#include <gnome.h>
#include <swversion.h>
#include <swmodule.h>
#include <localemgr.h>

#include "main/sword.h"
#include "main/lists.h"
#include "main/display.hh"

#include "backend/sword.h"
#include "backend/sword_main.hh"
char *OLD_CODESET;

using namespace sword; 
  
static char *get_sword_locale(void)
{
	const char *sys_local;
	char *retval = NULL;
	char buf[32];
	int i = 0;
	
	sys_local = LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName();
	
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
	
	LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(sys_local);
	SWLocale *sw_locale = LocaleMgr::getSystemLocaleMgr()->getLocale(sys_local);
	if(sw_locale)
		OLD_CODESET = (char*)sw_locale->getEncoding();
	else {
		OLD_CODESET = "iso8859-1";
		g_warning("locale not found");
	}
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
	char *sys_locale = NULL;
	const char *sword_version = get_sword_version();
	
#ifdef DEBUG	
	g_print("gnomesword-%s\n", VERSION);
	g_print("sword-%s\n", sword_version);
	g_print("%s\n\n", _("Initiating SWORD"));
#endif
	 
	sys_locale = strdup((char*)LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName());

	sword_locale = get_sword_locale();
#ifdef DEBUG	
	g_print("%s %s\n", _("System locale is"),sys_locale);
	g_print("%s %s\n\n", _("SWORD locale is"), sword_locale);
	g_print("%s\n", _("Checking for SWORD Modules"));	
#endif
	free((char*)sword_locale);
	free(sys_locale);
	backend = new BackEnd();
	backend->init_SWORD(0);
	main_init_lists();
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
	delete backend;
#ifdef DEBUG	
	g_print("%s\n", _("SWORD is shutdown"));
#endif
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
