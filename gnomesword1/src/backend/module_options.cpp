
/*
 * GnomeSword Bible Study Tool
 * module_options.cpp - load and save individual module options
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

#include "main/gs_gnomesword.h"
#include "backend/module_options.h"
#include "main/support.h"
#include "main/settings.h"

/******************************************************************************
 * load module option - using sword SWConfig
 ******************************************************************************/
gboolean backend_load_module_options(char * modName, char * option)
{
	char buf[255], *yesno;
	bool retval = false;

	sprintf(buf, "%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);
	module_options.Load();
	yesno = (char *) module_options[modName][option].c_str();
	if (!strcmp(yesno, "On"))
		retval = true;
	else
		retval = false;
	return retval;
}

/******************************************************************************
 * save module option - using sword SWConfig
 ******************************************************************************/
gboolean backend_save_module_options(char * modName, char * option, char * value)
{
	char buf[80], buf2[255];

	sprintf(buf, "%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);

	module_options[modName][option] = value;

	module_options.Save();
	return true;
}


/******  end of file  ******/
