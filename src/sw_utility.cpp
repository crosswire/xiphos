/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_utility.cpp
    * -------------------
    * Thur Apr 5 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
    *
 */
 
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
#include <gbfplain.h>
#include <plainhtml.h>
//#include <gbfhtmlhref.h>
#include "gbfhtmlhref.h"
#include <rwphtml.h>
#include <thmlhtml.h>
//#include <thmlhtmlhref.h>
#include "thmlhtmlhref.h"
//#include <latin1utf8.h>

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#include "sw_latin1utf8.h"
#include "sw_gnomesword.h"
#include "sw_utility.h"
SWFilter 
    * gbftohtml,		/* sword render filters */
    *plaintohtml, 
    *thmltohtml, 
    *rwptohtml, 
    *lattoutf8;
/*** initRenderFiltersUTIL ***/
void initRenderFiltersUTIL(void)
{
	plaintohtml = new PLAINHTML();	/* sword renderfilter plain to html */
	thmltohtml = new ThMLHTMLHREF();	/* sword renderfilter thml to html */
	rwptohtml = new RWPHTML();  /* sword renderfilter rwp to html */
	gbftohtml = new GBFHTMLHREF();  /* sword renderfilter gbf to html */
	lattoutf8 = new SW_Latin1UTF8();  /* sword renderfilter latin1 to utf8 */
}

/********************************************************************************************** 
 * addrenderfilters - 
 *           code originally taken form BibleTime 0.31    
 * 
 *********************************************************************************************/
void addrenderfiltersSWORD(SWModule *module, ConfigEntMap &section)
{
	string sourceformat;
	string moduleDriver;
	string encoding;
	gchar *retval;
	ConfigEntMap::iterator entry;
	bool noDriver = true;
	
	
	sourceformat = ((entry = section.find("SourceType")) != section.end()) ? (*entry).second : (string) "";
	moduleDriver = ((entry = section.find("ModDrv")) != section.end()) ? (*entry).second : (string) "";
	encoding = ((entry = section.find("Encoding")) != section.end()) ? (*entry).second : (string) "";
	
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
	
	if(!module->isUnicode()) { 
		module->AddRenderFilter(lattoutf8);
	}
}

void deleteRenderfilters(void)
{
	//-- delete render filters
	if (thmltohtml != 0)
		delete thmltohtml;
	if (rwptohtml != 0)
		delete rwptohtml;
	if (gbftohtml != 0)
		delete gbftohtml;
	if (plaintohtml != 0)
		delete plaintohtml;
	if (lattoutf8 != 0)
		delete lattoutf8;
}

/* path to sword modules 
void pathtomods(char *path)
{		
	SWMgr *mgr;
	gchar buf[255];
	
	mgr = new SWMgr();	//-- create sword mgrs
	sprintf(buf,"%s",mgr->prefixPath);
	path = g_strdup(buf); 
	delete mgr;
}
*/
void
changeModuleUTILITY(SWModule *module, SWMgr *Mgr, char *modName)
{
	ModMap::iterator it; 
	
	it = Mgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != Mgr->Modules.end()){ //-- if we find the module	
		module = (*it).second;  //-- change module to new module
	}		
}

