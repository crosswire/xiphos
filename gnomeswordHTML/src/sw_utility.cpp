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

//#include <versekey.h>
//#include <plainhtml.h>
//#include <gbfhtml.h>

//#include "gs_rwphtml.h"
//#include "gs_thmlhtml.h"
#include "gs_sword.h"
#include "sw_utility.h"

extern SWFilter *gbftohtml;
extern SWFilter *plaintohtml;
extern SWFilter *thmltohtml;
extern SWFilter *rwptohtml;
extern SWFilter *wgreektosil;



/********************************************************************************************** 
 * get module from module name 
 * modName - module name             
 * returns - module
 *********************************************************************************************/
SWModule *getModulefrmName(gchar *modName)
{
	/*
	ModMap::iterator it; //-- iterator to go through modules and find modName
	
	it = mainMgr->Modules.find(modName); //-- find module to use for search
	if (it != mainMgr->Modules.end()){ //-- if we dont reach the end of our modules	
		SWModule *mod = (*it).second;  //-- temp module to work with
		return mod;
	}*/
	return NULL;
	
}
/********************************************************************************************** 
 * addrenderfilters - 
 *           code taken form BibleTime 0.31    
 * 
 *********************************************************************************************/
void addrenderfiltersSWORD(SWModule *module, ConfigEntMap &section)
{
	string sourceformat;
	string moduleDriver;
	ConfigEntMap::iterator entry;
	bool noDriver = true;
	
	sourceformat = ((entry = section.find("SourceType")) != section.end()) ? (*entry).second : (string) "";
	moduleDriver = ((entry = section.find("ModDrv")) != section.end()) ? (*entry).second : (string) "";
	
	if (!stricmp(sourceformat.c_str(), "GBF")) {
		module->AddRenderFilter(gbftohtml);  
		noDriver = false; 
	}

	if (!stricmp(sourceformat.c_str(), "PLAIN")) {
		module->AddRenderFilter(plaintohtml);
		noDriver = false;
	}

	if (!stricmp(sourceformat.c_str(), "ThML")) {
		module->AddRenderFilter(thmltohtml);
		noDriver = false;
		//if(!stricmp(module->Name(),"TDavid") ) g_warning("TDavid");
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
}


/******************************************************************************
 * save personal comments
 * module - the module to save comments in
 *data - user comments to save
*******************************************************************************/
void savepersonalcommentsSWORD(SWModule *module, const gchar *data)
{
	*module << data; //-- save note!
	module->Display(); 
}

/******************************************************************************
 * delete personal comments
 * module - the module to save comments in
*******************************************************************************/
void deletepersonalcommentsSWORD(SWModule *module)
{	
	module->deleteEntry();        //-- delete note
	module->Display();        //-- show change	
}



