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

#ifndef __GNUC__
#include <io.h>
#else
#include <unistd.h>
#include <unixstr.h>
#endif

#include "sw_gnomesword.h"
#include "sw_utility.h"


void
changeModuleUTILITY(SWModule *module, SWMgr *Mgr, char *modName)
{
	ModMap::iterator it; 
	
	it = Mgr->Modules.find(modName); //-- iterate through the modules until we find modName - modName was passed by the callback
	if (it != Mgr->Modules.end()){ //-- if we find the module	
		module = (*it).second;  //-- change module to new module
	}		
}

string
get_module_lang_UTILITY(gchar *modName)
{
	string retval = "";	
	SWMgr *Mgr;
	SectionMap::iterator sit;
	ConfigEntMap::iterator entry;
	
	Mgr = new SWMgr();	//-- create sword mgr
	if ((sit = Mgr->config->Sections.find(modName)) != Mgr->config->Sections.end()) {
		ConfigEntMap & section = (*sit).second;
		retval = ((entry = section.find("Lang")) != section.end())? (*entry).second : (string) "";
	}
	delete Mgr;
	return retval;
}

