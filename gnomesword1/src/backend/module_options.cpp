/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_module_options.cpp
    * -------------------
    * Thu Nov 15 13:20:56 2001
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
#include <swmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <gbfplain.h>
#include <plainhtml.h>
#include <rwphtml.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>

#include "gs_gnomesword.h"
#include "module_options.h"
#include "support.h"


/***********************************************************************************************
 externals
***********************************************************************************************/
extern gchar *gSwordDir;
extern SETTINGS *settings;

/******************************************************************************
 * load module option - using sword SWConfig
 ******************************************************************************/
bool load_module_options(gchar *modName, gchar *option)
{
	gchar buf[255], *yesno;
	bool retval = false;
	
	sprintf(buf, "%s/modops.conf", gSwordDir);
	SWConfig module_options(buf);
	module_options.Load();	
	yesno = (gchar*)module_options[modName][option].c_str();
	if(!strcmp(yesno,"On"))
		retval = true;
	else
		retval = false;
	return retval;	
}
 
/******************************************************************************
 * save module option - using sword SWConfig
 ******************************************************************************/
bool save_module_options(gchar *modName, gchar *option, gchar *value)
{
	gchar buf[80], buf2[255];

	sprintf(buf, "%s/modops.conf", gSwordDir);
	SWConfig module_options(buf);
	
	module_options[modName][option] = value;	
	
	module_options.Save();
	return true;
}

/******************************************************************************
 * save module font - using sword SWConfig
 ******************************************************************************/
bool save_module_font(gchar *modName, gchar *font)
{
	
	return false;
}


