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
	
	// Temporary: To support old module types
//	if (sourceformat.empty())	{
//		if ((RawGBF*)(module))
//			sourceformat = "GBF";
//	}
	
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

