/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * sw_shortcutbar.h
     * -------------------
     * Sat June 30 2001
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

#ifndef __SW_SHORTCUTBAR_H__
#define __SW_SHORTCUTBAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main/gs_gnomesword.h"

int backend_sb_type_from_modname(char *mod_name);
void backend_save_sb_iconsize(char *filename, char *large_icons);
GList *backend_load_sb_group(char *filename, char *group_name,
			char *use_largeicons);
void backend_save_sb_group(char *filename, char *group_name, int group_num,
		char *large_icons);
void backend_display_sb_dictlex(char *modName, char *vlist);
void backend_setup_viewer(GtkWidget *html_widget);
void backend_shutdown_sb_viewer(void);

#ifdef __cplusplus
}
#endif

#endif

