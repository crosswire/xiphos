/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            sw_utility.cpp
                             -------------------
    begin                : Thur Apr 5 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <swmodule.h>
#include <swmgr.h>

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#include "gs_sword.h"
#include "sw_utility.h"

extern SWFilter *gbftohtml;
extern SWFilter *plaintohtml;
extern SWFilter *thmltohtml;
extern SWFilter *rwptohtml;
extern SWFilter *lattoutf8;

/********************************************************************************************** 
 * addrenderfilters - 
 *           code originally taken form BibleTime 0.31    
 * 
 *********************************************************************************************/
gchar *addrenderfiltersSWORD(SWModule *module, ConfigEntMap &section)
{
	string sourceformat;
	string moduleDriver;
	string encoding;
	string lang;
	gchar *retval;
	ConfigEntMap::iterator entry;
	bool noDriver = true;
 	bool isGBF = false;
	sourceformat = ((entry = section.find("SourceType")) != section.end()) ? (*entry).second : (string) "";
	moduleDriver = ((entry = section.find("ModDrv")) != section.end()) ? (*entry).second : (string) "";
	encoding = ((entry = section.find("Encoding")) != section.end()) ? (*entry).second : (string) "";
	lang = ((entry = section.find("Lang")) != section.end()) ? (*entry).second : (string) "";
	
	if (!stricmp(sourceformat.c_str(), "GBF")) {
		module->AddRenderFilter(gbftohtml);
		noDriver = false; 
		isGBF = true;
	}

	if (!stricmp(sourceformat.c_str(), "PLAIN")) {
		module->AddRenderFilter(plaintohtml);
		noDriver = false;
	}

	if (!stricmp(sourceformat.c_str(), "ThML")) {
		module->AddRenderFilter(thmltohtml);
		noDriver = false;
	}
	
	if (!stricmp(module->Name(), "RWP")) {
		module->AddRenderFilter(rwptohtml);
		noDriver = false;
	}

	if (noDriver){
		if (!stricmp(moduleDriver.c_str(), "RawCom")) {
			module->AddRenderFilter(plaintohtml);
			noDriver = false;
		}
		if (!stricmp(moduleDriver.c_str(), "RawLD")) {
			module->AddRenderFilter(plaintohtml);
			noDriver = false;
		}
	}	
	if(!module->isUnicode()) {                                                     //(stricmp(encoding.c_str(), "UTF-8")){ // &&( !isGBF)) {
		module->AddRenderFilter(lattoutf8);
	}
	retval = (char*)lang.c_str();
	//g_warning(retval);
	return retval;
}

void
changeModuleUTILITY(SWModule *module, SWMgr *Mgr, char *modName)
{
	ModMap::iterator it; 
	
	it = Mgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != Mgr->Modules.end()){ //-- if we find the module	
		module = (*it).second;  //-- change current module to new module
	}		
}

