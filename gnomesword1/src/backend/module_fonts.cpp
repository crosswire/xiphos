/*
 * GnomeSword Bible Study Tool
 * module_fonts.cpp - load and save individual module font information
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

#include <swconfig.h>

#include "main/sword.h"

#include "backend/module_fonts.h"

using namespace sword;

char *backend_get_module_font_name(char *mod_name, char * dir)
{
	char file[255];
	char *buf = NULL;

	sprintf(file, "%s/fonts.conf", dir);
	SWConfig module_options(file);
	module_options.Load();

	buf =
	    (char *) module_options[mod_name]["Font"].c_str();
	if(buf)
		return strdup(buf);
	else
		return NULL;
}

char *backend_get_module_font_size(char *mod_name, char * dir)
{
	char file[255];
	char *buf = NULL;

	sprintf(file, "%s/fonts.conf", dir);
	SWConfig module_options(file);
	module_options.Load();

	buf =
	    (char *) module_options[mod_name]["Fontsize"].c_str();
	if(buf)
		return strdup(buf);
	else
		return NULL;
}


void backend_load_font_info(MOD_FONT * mf, char * dir)
{
	char buf[255];
	char *tmpbuf = NULL;

	sprintf(buf, "%s/fonts.conf", dir);
	SWConfig module_options(buf);
	module_options.Load();

	mf->old_font =
	    (char *) module_options[mf->mod_name]["Font"].c_str();
	mf->old_gdk_font =
	    (char *) module_options[mf->mod_name]["GdkFont"].c_str();
	mf->old_font_size =
	    (char *) module_options[mf->mod_name]["Fontsize"].c_str();
}


void backend_save_font_info(MOD_FONT * mf, char * dir)
{
	char buf[80], buf2[255];

	sprintf(buf, "%s/fonts.conf", dir);
	SWConfig module_options(buf);

	module_options[mf->mod_name]["Font"] = mf->new_font;
	module_options[mf->mod_name]["GdkFont"] = mf->new_gdk_font;
	module_options[mf->mod_name]["Fontsize"] = mf->new_font_size;

	module_options.Save();
}




/******  end of file  ******/
