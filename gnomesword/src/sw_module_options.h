/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * sw_module_options.h
     * -------------------
     * Thu Nov 15 13:19:18 2001
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

#ifndef __SW_MODULE_OPTIONS_H__
#define __SW_MODULE_OPTIONS_H__


#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 * load module option - using sword SWConfig
 ******************************************************************************/
bool load_module_options(gchar *modName, gchar *option);

/******************************************************************************
 * save module option - using sword SWConfig
 ******************************************************************************/
bool save_module_options(gchar *modName, gchar *option, gchar *value);

/******************************************************************************
 * load module font - using sword SWConfig
 ******************************************************************************/
string load_module_font(gchar *modName, gchar *tag);	
 
/******************************************************************************
 * save module font - using sword SWConfig
 ******************************************************************************/
bool save_module_font(gchar *modName, gchar *font);

#ifdef __cplusplus
}
#endif
#endif				/* __SW_MODULE_OPTIONS_H__ */
